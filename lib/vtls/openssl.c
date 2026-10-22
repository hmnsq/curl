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
/*
 * Source file for all OpenSSL-specific code for the TLS/SSL layer. No code
 * but vtls.c should ever call or use these functions.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "formdata.h" /* for the boundary function */
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h" /* for the SSL config check function */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/openssl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/keylog.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/hostcheck.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "multiif.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/apple.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/rand.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/x509v3.h>
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_NO_DSA
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/dsa.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/dh.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/err.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/conf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/bn.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/rsa.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/bio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/pkcs12.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/tls1.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/evp.h>

#ਜੇ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/ech.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_NO_OCSP
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/ocsp.h>
#ਜੇ_ਅੰਤ

#ਜੇ !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_OPENSSL_ENGINE
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/engine.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  ਜੇ LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#ਜਾਂਜੇ !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/provider.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPENSSL_HAS_PROVIDERS
#ਜੇ_ਅੰਤ

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#ਜੇ !defined(LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_ਕੁੰਜੀ_EXPLICIT_PARAMS))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SSL_CTX_ਸੈੱਟ_DEFAULT_READ_ਬਫਰ_LEN 1
#ਜੇ_ਅੰਤ

#ਜੇ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/ui.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_EVP_PKEY_GET_PARAMS 1
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_EVP_PKEY_GET_PARAMS
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/core_names.h>
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECLARE_PKEY_PARAM_BIGNUM(name) ਅਚਲ BIGNUM *name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FREE_PKEY_PARAM_BIGNUM(name)
#ਜੇ_ਅੰਤ

/* Whether SSL_CTX_set_ciphersuites is available.
 * BoringSSL: no
 * LibreSSL: supported since 3.4.1 (released 2021-10-14)
 * OpenSSL: supported since 1.1.1 (commit a53b5be6a05)
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_IS_BORINGSSL
#  define HAVE_SSL_CTX_ਸੈੱਟ_CIPHERSUITES
#  ifndef OPENSSL_IS_AWSLC
#    define HAVE_SSL_CTX_ਸੈੱਟ_POST_HANDSHAKE_AUTH
#  endif
#ਜੇ_ਅੰਤ

/* Whether SSL_CTX_set1_sigalgs_list is available
 * BoringSSL: supported since 0.20240913.0 (commit 826ce15)
 * LibreSSL: no
 * OpenSSL: supported since 1.0.2 (commit 0b362de5f575)
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_SSL_CTX_SET1_SIGALGS
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSSL_PACKAGE "LibreSSL"
#ਜਾਂਜੇ defined(OPENSSL_IS_AWSLC)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSSL_PACKAGE "AWS-LC"
#ਜਾਂਜੇ defined(OPENSSL_IS_BORINGSSL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSSL_PACKAGE "BoringSSL"
#ਜਾਂਜੇ defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSSL_PACKAGE "quictls"
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSSL_PACKAGE "OpenSSL"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
ਕਿਸਮਪਰਿਭਾਸ਼ਾ size_t numcert_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t sslerr_t;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ numcert_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sslerr_t;
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ossl_valsize_t numcert_t

#ਜੇ OPENSSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x40100000L
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ossl_ASN1_STRING_get_ਲੰਮਾਈ ASN1_STRING_get_ਲੰਮਾਈ
#ਨਹੀਂਤਾਂ
ਸਥਿਰ size_t ossl_ASN1_STRING_get_ਲੰਮਾਈ(ਅਚਲ ASN1_STRING *str)
{
   ਅਚਲ ਪੂਰਨ ਲੰਮਾਈ = ASN1_STRING_ਲੰਮਾਈ(str);
   ਵਾਪਸ ਲੰਮਾਈ >= 0 ? (size_t)ਲੰਮਾਈ : 0;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode ਧੱਕੋ_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              BIO *mem, ਅਚਲ ਅੱਖਰ *label, ਪੂਰਨ ਅੰਕ)
  WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

ਸਥਿਰ CURLcode ਧੱਕੋ_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              BIO *mem, ਅਚਲ ਅੱਖਰ *label, ਪੂਰਨ ਅੰਕ)
{
  ਅੱਖਰ *ptr;
  ਲੰਮਾ len = BIO_get_mem_ਅੰਕੜੇ(mem, &ptr);
  CURLcode ਨਤੀਜਾ = Curl_ssl_ਧੱਕੋ_certinfo_len(ਅੰਕੜੇ, ਅੰਕ, label, ptr, len);
  (ਖਾਲੀ)BIO_reset(mem);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode pubkey_show(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            BIO *mem,
                            ਪੂਰਨ ਅੰਕ,
                            ਅਚਲ ਅੱਖਰ *type,
                            ਅਚਲ ਅੱਖਰ *name,
                            ਅਚਲ BIGNUM *bn) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;

ਸਥਿਰ CURLcode pubkey_show(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            BIO *mem,
                            ਪੂਰਨ ਅੰਕ,
                            ਅਚਲ ਅੱਖਰ *type,
                            ਅਚਲ ਅੱਖਰ *name,
                            ਅਚਲ BIGNUM *bn)
{
  ਅੱਖਰ namebuf[32];

  curl_msnprintf(namebuf, ਆਕਾਰ(namebuf), "%s(%s)", type, name);

  ਜੇ(bn)
    BN_print(mem, bn);
  ਵਾਪਸ ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, namebuf, ਅੰਕ);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ print_pubkey_BN(_type, _name, ਅੰਕ_2)           \
  pubkey_show(ਅੰਕੜੇ, mem, ਅੰਕ_2, #_type, #_name, _name)

ਸਥਿਰ ਪੂਰਨ asn1_object_dump(ਅਚਲ ASN1_OBJECT *a, ਅੱਖਰ *buf, size_t len)
{
  ਪੂਰਨ i = i2t_ASN1_OBJECT(buf, (ਪੂਰਨ)len, a);
  ਵਾਪਸ (i >= (ਪੂਰਨ)len);  /* buffer too small */
}

ਸਥਿਰ CURLcode X509V3_ext(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਪੂਰਨ certnum,
                           ਅਚਲ ਢੇਰ_ਦਾ(X509_EXTENSION) *extsarg)
{
  ਪੂਰਨ i;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ
  ਢੇਰ_ਦਾ(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#ਨਹੀਂਤਾਂ
  ਅਚਲ ਢੇਰ_ਦਾ(X509_EXTENSION) *exts = extsarg;
#ਜੇ_ਅੰਤ

  ਜੇ((ਪੂਰਨ)sk_X509_EXTENSION_ਅੰਕ(exts) <= 0)
    /* no extensions, bail out */
    ਵਾਪਸ ਨਤੀਜਾ;

  ਲਈ(i = 0; i < (ਪੂਰਨ)sk_X509_EXTENSION_ਅੰਕ(exts); i++) {
    ਅਚਲ ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_ਮੁੱਲ(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    ਅੱਖਰ namebuf[128];
    BIO *bio_ਬਾਹਰਲਾ = BIO_new(BIO_s_mem());

    ਜੇ(!bio_ਬਾਹਰਲਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    obj = X509_EXTENSION_get_object(ext);

    ਜੇ(asn1_object_dump(obj, namebuf, ਆਕਾਰ(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    ਜੇ(!X509V3_EXT_print(bio_ਬਾਹਰਲਾ, ext, 0, 0))
      ASN1_STRING_print(bio_ਬਾਹਰਲਾ,
                        (ਅਚਲ ASN1_STRING *)X509_EXTENSION_get_ਅੰਕੜੇ(ext));

    BIO_get_mem_ptr(bio_ਬਾਹਰਲਾ, &biomem);
    ਨਤੀਜਾ = Curl_ssl_ਧੱਕੋ_certinfo_len(ਅੰਕੜੇ, certnum, namebuf, biomem->ਅੰਕੜੇ,
                                        biomem->ਲੰਮਾਈ);
    BIO_free(bio_ਬਾਹਰਲਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode get_pkey_rsa(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             EVP_PKEY *pubkey, BIO *mem, ਪੂਰਨ i)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_EVP_PKEY_GET_PARAMS
  RSA *rsa = EVP_PKEY_get0_RSA(pubkey);
#ਜੇ_ਅੰਤ /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(n);
  DECLARE_PKEY_PARAM_BIGNUM(e);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_N, &n);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_E, &e);
#ਨਹੀਂਤਾਂ
  RSA_get0_ਕੁੰਜੀ(rsa, &n, &e, ਨਿਰਾ);
#ਜੇ_ਅੰਤ /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (ਪੂਰਨ)(n ? BN_ਅੰਕ_ਬਿੱਟ(n) : 0));
  ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "RSA Public Key", i);
  ਜੇ(!ਨਤੀਜਾ) {
    ਨਤੀਜਾ = print_pubkey_BN(rsa, n, i);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_NO_DSA
ਸਥਿਰ CURLcode get_pkey_dsa(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             EVP_PKEY *pubkey, BIO *mem, ਪੂਰਨ i)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#ਜੇ_ਅੰਤ /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ਕੁੰਜੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ਕੁੰਜੀ, &pub_ਕੁੰਜੀ);
#ਨਹੀਂਤਾਂ
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_ਕੁੰਜੀ(dsa, &pub_ਕੁੰਜੀ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ /* HAVE_EVP_PKEY_GET_PARAMS */
  ਨਤੀਜਾ = print_pubkey_BN(dsa, p, i);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = print_pubkey_BN(dsa, q, i);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = print_pubkey_BN(dsa, g, i);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = print_pubkey_BN(dsa, pub_ਕੁੰਜੀ, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ਕੁੰਜੀ);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* !OPENSSL_NO_DSA */

ਸਥਿਰ CURLcode get_pkey_dh(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            EVP_PKEY *pubkey, BIO *mem, ਪੂਰਨ i)
{
  CURLcode ਨਤੀਜਾ;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#ਜੇ_ਅੰਤ /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ਕੁੰਜੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ਕੁੰਜੀ, &pub_ਕੁੰਜੀ);
#ਨਹੀਂਤਾਂ
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_ਕੁੰਜੀ(dh, &pub_ਕੁੰਜੀ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ /* HAVE_EVP_PKEY_GET_PARAMS */
  ਨਤੀਜਾ = print_pubkey_BN(dh, p, i);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = print_pubkey_BN(dh, q, i);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = print_pubkey_BN(dh, g, i);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = print_pubkey_BN(dh, pub_ਕੁੰਜੀ, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ਕੁੰਜੀ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅਚਲ X509_PUBKEY pubkeytype_t;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ X509_PUBKEY pubkeytype_t;
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode ossl_certchain(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, SSL *ssl)
{
  CURLcode ਨਤੀਜਾ;
  ਢੇਰ_ਦਾ(X509) *sk;
  ਪੂਰਨ i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  ਜੇ(!sk)
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_ਅੰਕ(sk);
  ਜੇ(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(ਅੰਕੜੇ, "%d certificates is more than allowed (%d)", (ਪੂਰਨ)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਨਤੀਜਾ = Curl_ssl_init_certinfo(ਅੰਕੜੇ, (ਪੂਰਨ)numcerts);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  mem = BIO_new(BIO_s_mem());
  ਜੇ(!mem)
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਲਈ(i = 0; !ਨਤੀਜਾ && (i < (ਪੂਰਨ)numcerts); i++) {
    X509 *x = sk_X509_ਮੁੱਲ(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = ਨਿਰਾ;
    size_t j;
    ਅਚਲ ASN1_ਦੋਆਧਾਰੀ_ਅੰਕ_STRING *psig = ਨਿਰਾ;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Subject", i);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Issuer", i);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    BIO_printf(mem, "%lx", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)X509_get_ਸੰਸਕਰਣ(x));
    ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Version", i);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    {
      ASN1_INTEGER *ਅੰਕ = X509_get_serial_ਸੰਖਿਆ(x);
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *numdata = ASN1_STRING_get0_ਅੰਕੜੇ(ਅੰਕ);
      ਅਚਲ size_t numlen = ossl_ASN1_STRING_get_ਲੰਮਾਈ(ਅੰਕ);
      ਜੇ(ASN1_STRING_type(ਅੰਕ) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      ਲਈ(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Serial Number", i);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    {
      ਅਚਲ X509_ALGOR *sigalg = ਨਿਰਾ;
      pubkeytype_t *xpubkey = ਨਿਰਾ;
      ASN1_OBJECT *pubkeyoid = ਨਿਰਾ;

      X509_get0_signature(&psig, &sigalg, x);
      ਜੇ(sigalg) {
        ਅਚਲ ASN1_OBJECT *sigalgoid = ਨਿਰਾ;
        X509_ALGOR_get0(&sigalgoid, ਨਿਰਾ, ਨਿਰਾ, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Signature Algorithm", i);
        ਜੇ(ਨਤੀਜਾ)
          ਰੋਕੋ;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      ਜੇ(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, xpubkey);
        ਜੇ(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Public Key Algorithm", i);
          ਜੇ(ਨਤੀਜਾ)
            ਰੋਕੋ;
        }
      }

      ਨਤੀਜਾ = X509V3_ext(ਅੰਕੜੇ, i, X509_get0_extensions(x));
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
    }

    ASN1_ਸਮਾਂ_print(mem, X509_get0_notBefore(x));
    ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Start date", i);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    ASN1_ਸਮਾਂ_print(mem, X509_get0_notAfter(x));
    ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Expire date", i);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    pubkey = X509_get_pubkey(x);
    ਜੇ(!pubkey)
      infof(ਅੰਕੜੇ, "   Unable to load public key");
    ਨਹੀਂਤਾਂ {
      ਚੋਣ(EVP_PKEY_id(pubkey)) {
      ਹਾਲਤ EVP_PKEY_RSA:
        ਨਤੀਜਾ = get_pkey_rsa(ਅੰਕੜੇ, pubkey, mem, i);
        ਰੋਕੋ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_NO_DSA
      ਹਾਲਤ EVP_PKEY_DSA:
        ਨਤੀਜਾ = get_pkey_dsa(ਅੰਕੜੇ, pubkey, mem, i);
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ

      ਹਾਲਤ EVP_PKEY_DH:
        ਨਤੀਜਾ = get_pkey_dh(ਅੰਕੜੇ, pubkey, mem, i);
        ਰੋਕੋ;
      }
      EVP_PKEY_free(pubkey);
    }

    ਜੇ(!ਨਤੀਜਾ && psig) {
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *psigdata = ASN1_STRING_get0_ਅੰਕੜੇ(psig);
      ਅਚਲ size_t psiglen = ossl_ASN1_STRING_get_ਲੰਮਾਈ(psig);
      ਲਈ(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Signature", i);
    }

    ਜੇ(!ਨਤੀਜਾ) {
      PEM_write_bio_X509(mem, x);
      ਨਤੀਜਾ = ਧੱਕੋ_certinfo(ਅੰਕੜੇ, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  ਜੇ(ਨਤੀਜਾ)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(ਅੰਕੜੇ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ ossl_bio_cf_create(BIO *bio)
{
  BIO_ਸੈੱਟ_shutdown(bio, 1);
  BIO_ਸੈੱਟ_init(bio, 1);
  BIO_ਸੈੱਟ_ਅੰਕੜੇ(bio, ਨਿਰਾ);
  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ ossl_bio_cf_destroy(BIO *bio)
{
  ਜੇ(!bio)
    ਵਾਪਸ 0;
  ਵਾਪਸ 1;
}

ਸਥਿਰ ਲੰਮਾ ossl_bio_cf_ctrl(BIO *bio, ਪੂਰਨ cmd, ਲੰਮਾ ਅੰਕ, ਖਾਲੀ *ptr)
{
  ਬਣਤਰ Curl_cfilter *cf = BIO_get_ਅੰਕੜੇ(bio);
  ਲੰਮਾ ret = 1;

  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ptr;
  ਚੋਣ(cmd) {
  ਹਾਲਤ BIO_CTRL_GET_CLOSE:
    ret = (ਲੰਮਾ)BIO_get_shutdown(bio);
    ਰੋਕੋ;
  ਹਾਲਤ BIO_CTRL_ਸੈੱਟ_CLOSE:
    BIO_ਸੈੱਟ_shutdown(bio, (ਪੂਰਨ)ਅੰਕ);
    ਰੋਕੋ;
  ਹਾਲਤ BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    ਰੋਕੋ;
  ਹਾਲਤ BIO_CTRL_DUP:
    ret = 1;
    ਰੋਕੋ;
  ਹਾਲਤ BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
    ਵਾਪਸ connssl->peer_closed;
  }
  ਮੂਲ:
    ret = 0;
    ਰੋਕੋ;
  }
  ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ ossl_bio_cf_ਬਾਹਰਲਾ_write(BIO *bio, ਅਚਲ ਅੱਖਰ *buf, ਪੂਰਨ blen)
{
  ਬਣਤਰ Curl_cfilter *cf = BIO_get_ਅੰਕੜੇ(bio);
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(blen < 0)
    ਵਾਪਸ 0;

  ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ,
                             (ਅਚਲ uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (ਪੂਰਨ)ਨਤੀਜਾ, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_ਨਤੀਜਾ = ਨਤੀਜਾ;
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      BIO_ਸੈੱਟ_retry_write(bio);
    ਵਾਪਸ -1;
  }
  ਵਾਪਸ (ਪੂਰਨ)nwritten;
}

ਸਥਿਰ ਪੂਰਨ ossl_bio_cf_ਵਿੱਚ_read(BIO *bio, ਅੱਖਰ *buf, ਪੂਰਨ blen)
{
  ਬਣਤਰ Curl_cfilter *cf = BIO_get_ਅੰਕੜੇ(bio);
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  size_t nread;
  CURLcode ਨਤੀਜਾ, r2;

  DEBUGASSERT(ਅੰਕੜੇ);
  /* OpenSSL catches this case, so should we. */
  ਜੇ(!buf)
    ਵਾਪਸ 0;
  ਜੇ(blen < 0)
    ਵਾਪਸ 0;

  ਨਤੀਜਾ = Curl_conn_cf_recv(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, (size_t)blen, &nread);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (ਪੂਰਨ)ਨਤੀਜਾ, nread);
  BIO_clear_retry_flags(bio);
  octx->io_ਨਤੀਜਾ = ਨਤੀਜਾ;
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      BIO_ਸੈੱਟ_retry_read(bio);
  }
  ਨਹੀਂਤਾਂ {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->ਨਿਵੇਸ਼_pending = TRUE;
    ਜੇ(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  ਜੇ(!octx->x509_store_ਤਿਆਰੀ) {
    r2 = Curl_ssl_ਤਿਆਰੀ_x509_store(cf, ਅੰਕੜੇ, octx);
    ਜੇ(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_ਨਤੀਜਾ = r2;
      ਵਾਪਸ -1;
    }
    octx->x509_store_ਤਿਆਰੀ = TRUE;
  }
  ਵਾਪਸ ਨਤੀਜਾ ? -1 : (ਪੂਰਨ)nread;
}

ਸਥਿਰ BIO_ਵਿਧੀ *ossl_bio_cf_ਵਿਧੀ_create(ਖਾਲੀ)
{
  BIO_ਵਿਧੀ *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  ਜੇ(m) {
    BIO_meth_ਸੈੱਟ_write(m, &ossl_bio_cf_ਬਾਹਰਲਾ_write);
    BIO_meth_ਸੈੱਟ_read(m, &ossl_bio_cf_ਵਿੱਚ_read);
    BIO_meth_ਸੈੱਟ_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_ਸੈੱਟ_create(m, &ossl_bio_cf_create);
    BIO_meth_ਸੈੱਟ_destroy(m, &ossl_bio_cf_destroy);
  }
  ਵਾਪਸ m;
}

ਸਥਿਰ ਖਾਲੀ ossl_bio_cf_ਵਿਧੀ_free(BIO_ਵਿਧੀ *m)
{
  ਜੇ(m)
    BIO_meth_free(m);
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_KEYLOG_UPSTREAM
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_KEYLOG_CALLBACK
ਸਥਿਰ ਖਾਲੀ ossl_keylog_callback(ਅਚਲ SSL *ssl, ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
  (ਖਾਲੀ)ssl;

  Curl_tls_keylog_write_ਸਤਰ(ਸਤਰ);
}
#ਨਹੀਂਤਾਂ
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
ਸਥਿਰ ਖਾਲੀ ossl_log_tls12_secret(ਅਚਲ SSL *ssl, bool *keylog_ਪੂਰਾ)
{
  ਅਚਲ SSL_SESSION *session;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ client_random[SSL3_RANDOM_ਮਾਪ];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ master_ਕੁੰਜੀ[SSL_MAX_MASTER_ਕੁੰਜੀ_ਲੰਮਾਈ];
  ਪੂਰਨ master_ਕੁੰਜੀ_ਲੰਮਾਈ = 0;

  ERR_ਸੈੱਟ_mark();

  session = SSL_get_session(ssl);

  ਜੇ(!session || *keylog_ਪੂਰਾ) {
    ERR_ਕੱਢੋ_ਮੰਜ਼ਿਲ_mark();
    ਵਾਪਸ;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_ਮਾਪ);
  master_ਕੁੰਜੀ_ਲੰਮਾਈ = (ਪੂਰਨ)
    SSL_SESSION_get_master_ਕੁੰਜੀ(session, master_ਕੁੰਜੀ, SSL_MAX_MASTER_ਕੁੰਜੀ_ਲੰਮਾਈ);

  ERR_ਕੱਢੋ_ਮੰਜ਼ਿਲ_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  ਜੇ(master_ਕੁੰਜੀ_ਲੰਮਾਈ <= 0)
    ਵਾਪਸ;

  *keylog_ਪੂਰਾ = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        ਆਕਾਰ(client_random),
                        master_ਕੁੰਜੀ, master_ਕੁੰਜੀ_ਲੰਮਾਈ);
}
#ਜੇ_ਅੰਤ /* !HAVE_KEYLOG_CALLBACK */
#ਜੇ_ਅੰਤ /* HAVE_KEYLOG_UPSTREAM */

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *SSL_ERROR_ਮੰਜ਼ਿਲ_str(ਪੂਰਨ err)
{
  ਚੋਣ(err) {
  ਹਾਲਤ SSL_ERROR_NONE:
    ਵਾਪਸ "SSL_ERROR_NONE";
  ਹਾਲਤ SSL_ERROR_SSL:
    ਵਾਪਸ "SSL_ERROR_SSL";
  ਹਾਲਤ SSL_ERROR_WANT_READ:
    ਵਾਪਸ "SSL_ERROR_WANT_READ";
  ਹਾਲਤ SSL_ERROR_WANT_WRITE:
    ਵਾਪਸ "SSL_ERROR_WANT_WRITE";
  ਹਾਲਤ SSL_ERROR_WANT_X509_LOOKUP:
    ਵਾਪਸ "SSL_ERROR_WANT_X509_LOOKUP";
  ਹਾਲਤ SSL_ERROR_SYSCALL:
    ਵਾਪਸ "SSL_ERROR_SYSCALL";
  ਹਾਲਤ SSL_ERROR_ZERO_RETURN:
    ਵਾਪਸ "SSL_ERROR_ZERO_RETURN";
  ਹਾਲਤ SSL_ERROR_WANT_CONNECT:
    ਵਾਪਸ "SSL_ERROR_WANT_CONNECT";
  ਹਾਲਤ SSL_ERROR_WANT_ACCEPT:
    ਵਾਪਸ "SSL_ERROR_WANT_ACCEPT";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ਹਾਲਤ SSL_ERROR_WANT_ASYNC:
    ਵਾਪਸ "SSL_ERROR_WANT_ASYNC";
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_ERROR_WANT_ASYNC_ਕੰਮ  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ਹਾਲਤ SSL_ERROR_WANT_ASYNC_ਕੰਮ:
    ਵਾਪਸ "SSL_ERROR_WANT_ASYNC_JOB";
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  ਹਾਲਤ SSL_ERROR_WANT_CLIENT_HELLO_CB:
    ਵਾਪਸ "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    ਵਾਪਸ "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
ਸਥਿਰ ਅੱਖਰ *ossl_strerror(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ error, ਅੱਖਰ *buf, size_t ਮਾਪ)
{
  size_t len;
  DEBUGASSERT(ਮਾਪ);
  *buf = '\0';

  len = Curl_ossl_ਸੰਸਕਰਣ(buf, ਮਾਪ);
  DEBUGASSERT(len < (ਮਾਪ - 2));
  ਜੇ(len < (ਮਾਪ - 2)) {
    buf += len;
    ਮਾਪ -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, ਮਾਪ);
#ਨਹੀਂਤਾਂ
  ERR_error_string_n(error, buf, ਮਾਪ);
#ਜੇ_ਅੰਤ

  ਜੇ(!*buf) {
    ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, ਮਾਪ, ਸੁਨੇਹਾ, ਲੜੀ_ਲੰਬਾਈ(ਸੁਨੇਹਾ));
  }

  ਵਾਪਸ buf;
}

ਸਥਿਰ ਪੂਰਨ passwd_callback(ਅੱਖਰ *buf, ਪੂਰਨ ਅੰਕ, ਪੂਰਨ encrypting, ਖਾਲੀ *password)
{
  DEBUGASSERT(encrypting == 0);

  ਜੇ(!encrypting && ਅੰਕ >= 0 && password) {
    ਪੂਰਨ klen = curlx_uztosi(ਲੜੀ_ਲੰਬਾਈ((ਅੱਖਰ *)password));
    ਜੇ(ਅੰਕ > klen) {
      ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf, password, klen + 1);
      ਵਾਪਸ klen;
    }
  }
  ਵਾਪਸ 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
ਸਥਿਰ bool rand_enough(ਖਾਲੀ)
{
  ਵਾਪਸ RAND_ਸਥਿਤੀ() != 0;
}

ਸਥਿਰ CURLcode ossl_ਬੀਜ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  /* This might get called before it has been added to a multi handle */
  ਜੇ(ਅੰਕੜੇ->multi && ਅੰਕੜੇ->multi->ssl_seeded)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    ਜੇ(ਅੰਕੜੇ->multi)
      ਅੰਕੜੇ->multi->ssl_seeded = TRUE;
    ਵਾਪਸ CURLE_OK;
  }
  failf(ਅੰਕੜੇ, "Insufficient randomness");
  ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SSL_FILETYPE_ENGINE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_FILETYPE_ENGINE 42
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SSL_FILETYPE_PKCS12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_FILETYPE_PKCS12 43
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SSL_FILETYPE_PROVIDER
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_FILETYPE_PROVIDER 44
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ ossl_do_file_type(ਅਚਲ ਅੱਖਰ *type)
{
  ਜੇ(!type || !type[0])
    ਵਾਪਸ SSL_FILETYPE_PEM;
  ਜੇ(curl_strequal(type, "PEM"))
    ਵਾਪਸ SSL_FILETYPE_PEM;
  ਜੇ(curl_strequal(type, "DER"))
    ਵਾਪਸ SSL_FILETYPE_ASN1;
  ਜੇ(curl_strequal(type, "PROV"))
    ਵਾਪਸ SSL_FILETYPE_PROVIDER;
  ਜੇ(curl_strequal(type, "ENG"))
    ਵਾਪਸ SSL_FILETYPE_ENGINE;
  ਜੇ(curl_strequal(type, "P12"))
    ਵਾਪਸ SSL_FILETYPE_PKCS12;
  ਵਾਪਸ -1;
}

#ਜੇ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
ਸਥਿਰ ਪੂਰਨ ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ਅਚਲ ਅੱਖਰ *password;
  ਚੋਣ(UI_get_string_type(uis)) {
  ਹਾਲਤ UIT_PROMPT:
  ਹਾਲਤ UIT_VERIFY:
    password = (ਅਚਲ ਅੱਖਰ *)UI_get0_user_ਅੰਕੜੇ(ui);
    ਜੇ(password && (UI_get_ਨਿਵੇਸ਼_flags(uis) & UI_ਨਿਵੇਸ਼_FLAG_DEFAULT_PWD)) {
      UI_ਸੈੱਟ_ਨਤੀਜਾ(ui, uis, password);
      ਵਾਪਸ 1;
    }
    FALLTHROUGH();
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ UI_ਵਿਧੀ_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
ਸਥਿਰ ਪੂਰਨ ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  ਚੋਣ(UI_get_string_type(uis)) {
  ਹਾਲਤ UIT_PROMPT:
  ਹਾਲਤ UIT_VERIFY:
    ਜੇ(UI_get0_user_ਅੰਕੜੇ(ui) &&
       (UI_get_ਨਿਵੇਸ਼_flags(uis) & UI_ਨਿਵੇਸ਼_FLAG_DEFAULT_PWD)) {
      ਵਾਪਸ 1;
    }
    FALLTHROUGH();
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ UI_ਵਿਧੀ_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
ਸਥਿਰ bool is_pkcs11_uri(ਅਚਲ ਅੱਖਰ *string)
{
  ਵਾਪਸ string && curl_strnequal(string, "pkcs11:", 7);
}

#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode ossl_ਸੈੱਟ_engine(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ossl_ctx *octx,
                                ਅਚਲ ਅੱਖਰ *name);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_HAS_PROVIDERS
ਸਥਿਰ CURLcode ossl_ਸੈੱਟ_provider(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ossl_ctx *octx,
                                  ਅਚਲ ਅੱਖਰ *iname);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ use_certificate_blob(SSL_CTX *ctx, ਅਚਲ ਬਣਤਰ curl_blob *blob,
                                ਪੂਰਨ type, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_passwd)
{
  ਪੂਰਨ ret = 0;
  X509 *x = ਨਿਰਾ;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *ਵਿੱਚ = BIO_new_mem_buf(blob->ਅੰਕੜੇ, (ਪੂਰਨ)blob->len);
  ਜੇ(!ਵਿੱਚ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(ਵਿੱਚ, ਨਿਰਾ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(ਵਿੱਚ, ਨਿਰਾ, passwd_callback, CURL_UNCONST(ਕੁੰਜੀ_passwd));
  }
  ਨਹੀਂਤਾਂ {
    ret = 0;
    ਜਾਓ ਸਿਰਾ;
  }

  ਜੇ(!x) {
    ret = 0;
    ਜਾਓ ਸਿਰਾ;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
ਸਿਰਾ:
  X509_free(x);
  BIO_free(ਵਿੱਚ);
  ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ use_privatekey_blob(SSL_CTX *ctx, ਅਚਲ ਬਣਤਰ curl_blob *blob,
                               ਪੂਰਨ type, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_passwd)
{
  ਪੂਰਨ ret = 0;
  EVP_PKEY *pkey = ਨਿਰਾ;
  BIO *ਵਿੱਚ = BIO_new_mem_buf(blob->ਅੰਕੜੇ, (ਪੂਰਨ)blob->len);
  ਜੇ(!ਵਿੱਚ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_ਨਿੱਜੀ_ਕੁੰਜੀ(ਵਿੱਚ, ਨਿਰਾ, passwd_callback,
                                   CURL_UNCONST(ਕੁੰਜੀ_passwd));
  ਨਹੀਂਤਾਂ ਜੇ(type == SSL_FILETYPE_ASN1)
    pkey = d2i_ਨਿੱਜੀ_ਕੁੰਜੀ_bio(ਵਿੱਚ, ਨਿਰਾ);
  ਨਹੀਂਤਾਂ
    ਜਾਓ ਸਿਰਾ;

  ਜੇ(!pkey)
    ਜਾਓ ਸਿਰਾ;

  ret = SSL_CTX_use_ਨਿੱਜੀ_ਕੁੰਜੀ(ctx, pkey);
  EVP_PKEY_free(pkey);
ਸਿਰਾ:
  BIO_free(ਵਿੱਚ);
  ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ use_certificate_chain_blob(SSL_CTX *ctx,
                                      ਅਚਲ ਬਣਤਰ curl_blob *blob,
                                      ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_passwd)
{
  ਪੂਰਨ ret = 0;
  X509 *x = ਨਿਰਾ;
  BIO *ਵਿੱਚ = BIO_new_mem_buf(blob->ਅੰਕੜੇ, (ਪੂਰਨ)blob->len);
  ਜੇ(!ਵਿੱਚ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(ਵਿੱਚ, ਨਿਰਾ,
                            passwd_callback, CURL_UNCONST(ਕੁੰਜੀ_passwd));
  ਜੇ(!x)
    ਜਾਓ ਸਿਰਾ;

  ret = SSL_CTX_use_certificate(ctx, x);

  ਜੇ(ERR_peek_error() != 0)
    ret = 0;

  ਜੇ(ret) {
    X509 *ca;
    sslerr_t err;

    ਜੇ(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      ਜਾਓ ਸਿਰਾ;
    }

    ਜਦੋਂ((ca = PEM_read_bio_X509(ਵਿੱਚ, ਨਿਰਾ, passwd_callback,
                                  CURL_UNCONST(ਕੁੰਜੀ_passwd))) != ਨਿਰਾ) {

      ਜੇ(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        ਜਾਓ ਸਿਰਾ;
      }
    }

    err = ERR_peek_ਆਖਰੀ_error();
    ਜੇ((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_ਸ਼ੁਰੂ_ਸਤਰ))
      ERR_clear_error();
    ਨਹੀਂਤਾਂ
      ret = 0;
  }

ਸਿਰਾ:
  X509_free(x);
  BIO_free(ਵਿੱਚ);
  ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ enginecheck(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ ossl_ctx *octx,
                       ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_file,
                       ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_passwd)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL_ENGINE
  EVP_PKEY *priv_ਕੁੰਜੀ = ਨਿਰਾ;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  ਜੇ(!octx->p.engine && is_pkcs11_uri(ਕੁੰਜੀ_file) &&
     ossl_ਸੈੱਟ_engine(ਅੰਕੜੇ, octx, "pkcs11") != CURLE_OK)
    ਵਾਪਸ 0;

  ਜੇ(octx->p.engine) {
    UI_ਵਿਧੀ *ui_ਵਿਧੀ = UI_create_ਵਿਧੀ("curl user interface");
    ਜੇ(!ui_ਵਿਧੀ) {
      failf(ਅੰਕੜੇ, "unable to create " OSSL_PACKAGE " user-interface method");
      ਵਾਪਸ 0;
    }
    UI_ਵਿਧੀ_ਸੈੱਟ_opener(ui_ਵਿਧੀ, UI_ਵਿਧੀ_get_opener(UI_OpenSSL()));
    UI_ਵਿਧੀ_ਸੈੱਟ_closer(ui_ਵਿਧੀ, UI_ਵਿਧੀ_get_closer(UI_OpenSSL()));
    UI_ਵਿਧੀ_ਸੈੱਟ_reader(ui_ਵਿਧੀ, ssl_ui_reader);
    UI_ਵਿਧੀ_ਸੈੱਟ_writer(ui_ਵਿਧੀ, ssl_ui_writer);
    priv_ਕੁੰਜੀ = ENGINE_ਲੱਦੋ_ਨਿੱਜੀ_ਕੁੰਜੀ(octx->p.engine, ਕੁੰਜੀ_file,
                                       ui_ਵਿਧੀ,
                                       CURL_UNCONST(ਕੁੰਜੀ_passwd));
    UI_destroy_ਵਿਧੀ(ui_ਵਿਧੀ);
    ਜੇ(!priv_ਕੁੰਜੀ) {
      failf(ਅੰਕੜੇ, "failed to load private key from crypto engine");
      ਵਾਪਸ 0;
    }
    ਜੇ(SSL_CTX_use_ਨਿੱਜੀ_ਕੁੰਜੀ(ctx, priv_ਕੁੰਜੀ) != 1) {
      failf(ਅੰਕੜੇ, "unable to set private key");
      EVP_PKEY_free(priv_ਕੁੰਜੀ);
      ਵਾਪਸ 0;
    }
    EVP_PKEY_free(priv_ਕੁੰਜੀ);  /* we do not need the handle any more... */
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "crypto engine not set, cannot load private key");
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ 1;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)octx;
  (ਖਾਲੀ)ਕੁੰਜੀ_file;
  (ਖਾਲੀ)ਕੁੰਜੀ_passwd;
  failf(ਅੰਕੜੇ, "SSL_FILETYPE_ENGINE not supported for private key");
  ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ providercheck(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ ossl_ctx *octx,
                         ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_file)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_HAS_PROVIDERS
  ਅੱਖਰ error_ਬਫਰ[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  ਜੇ(!octx->p.provider_loaded && is_pkcs11_uri(ਕੁੰਜੀ_file) &&
     ossl_ਸੈੱਟ_provider(ਅੰਕੜੇ, octx, "pkcs11") != CURLE_OK) {
    ਵਾਪਸ 0;
  }

  ਜੇ(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_ਕੁੰਜੀ = ਨਿਰਾ;
    OSSL_STORE_CTX *store = ਨਿਰਾ;
    OSSL_STORE_INFO *info = ਨਿਰਾ;
    UI_ਵਿਧੀ *ui_ਵਿਧੀ = UI_create_ਵਿਧੀ("curl user interface");
    ਜੇ(!ui_ਵਿਧੀ) {
      failf(ਅੰਕੜੇ, "unable to create " OSSL_PACKAGE " user-interface method");
      ਵਾਪਸ 0;
    }
    UI_ਵਿਧੀ_ਸੈੱਟ_opener(ui_ਵਿਧੀ, UI_ਵਿਧੀ_get_opener(UI_OpenSSL()));
    UI_ਵਿਧੀ_ਸੈੱਟ_closer(ui_ਵਿਧੀ, UI_ਵਿਧੀ_get_closer(UI_OpenSSL()));
    UI_ਵਿਧੀ_ਸੈੱਟ_reader(ui_ਵਿਧੀ, ssl_ui_reader);
    UI_ਵਿਧੀ_ਸੈੱਟ_writer(ui_ਵਿਧੀ, ssl_ui_writer);

    store = OSSL_STORE_open_ex(ਕੁੰਜੀ_file, octx->p.libctx,
                               octx->p.propq, ui_ਵਿਧੀ, ਨਿਰਾ, ਨਿਰਾ,
                               ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ(!store) {
      failf(ਅੰਕੜੇ, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      UI_destroy_ਵਿਧੀ(ui_ਵਿਧੀ);
      ਵਾਪਸ 0;
    }
    ਜੇ(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(ਅੰਕੜੇ, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
    }

    info = OSSL_STORE_ਲੱਦੋ(store);
    ਜੇ(info) {
      ਪੂਰਨ ossl_type = OSSL_STORE_INFO_get_type(info);

      ਜੇ(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_ਕੁੰਜੀ = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_ਵਿਧੀ(ui_ਵਿਧੀ);
    ਜੇ(!priv_ਕੁੰਜੀ) {
      failf(ਅੰਕੜੇ, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      ਵਾਪਸ 0;
    }

    ਜੇ(SSL_CTX_use_ਨਿੱਜੀ_ਕੁੰਜੀ(ctx, priv_ਕੁੰਜੀ) != 1) {
      failf(ਅੰਕੜੇ, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      EVP_PKEY_free(priv_ਕੁੰਜੀ);
      ਵਾਪਸ 0;
    }
    EVP_PKEY_free(priv_ਕੁੰਜੀ); /* we do not need the handle any more... */
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "crypto provider not set, cannot load private key");
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ 1;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)octx;
  (ਖਾਲੀ)ਕੁੰਜੀ_file;
  failf(ਅੰਕੜੇ, "SSL_FILETYPE_PROVIDER not supported for private key");
  ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ engineload(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਬਣਤਰ ossl_ctx *octx,
                      ਅਚਲ ਅੱਖਰ *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#ਜੇ defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  ਅੱਖਰ error_ਬਫਰ[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  ਜੇ(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_ਸੈੱਟ_engine(ਅੰਕੜੇ, octx, "pkcs11") != CURLE_OK)
    ਵਾਪਸ 0;

  ਜੇ(octx->p.engine) {
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ cmd_name[] = "LOAD_CERT_CTRL";
    ਬਣਤਰ {
      ਅਚਲ ਅੱਖਰ *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = ਨਿਰਾ;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    ਜੇ(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), ਨਿਰਾ)) {
      failf(ਅੰਕੜੇ, "SSL engine does not support loading certificates");
      ਵਾਪਸ 0;
    }

    /* Load the certificate from the engine */
    ਜੇ(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, ਨਿਰਾ, 1)) {
      failf(ਅੰਕੜੇ, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      ਵਾਪਸ 0;
    }

    ਜੇ(!params.cert) {
      failf(ਅੰਕੜੇ, "SSL engine did not initialize the certificate properly.");
      ਵਾਪਸ 0;
    }

    ਜੇ(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(ਅੰਕੜੇ, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      X509_free(params.cert);
      ਵਾਪਸ 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "crypto engine not set, cannot load certificate");
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ 1;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)octx;
  (ਖਾਲੀ)cert_file;
  failf(ਅੰਕੜੇ, "SSL_FILETYPE_ENGINE not supported for certificate");
  ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ providerload(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ ossl_ctx *octx,
                        ਅਚਲ ਅੱਖਰ *cert_file)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_HAS_PROVIDERS
  ਅੱਖਰ error_ਬਫਰ[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  ਜੇ(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_ਸੈੱਟ_provider(ਅੰਕੜੇ, octx, "pkcs11") != CURLE_OK)
    ਵਾਪਸ 0;

  ਜੇ(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = ਨਿਰਾ;
    X509 *cert = ਨਿਰਾ;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    ਪੂਰਨ rc;

    ਜੇ(!store) {
      failf(ਅੰਕੜੇ, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      ਵਾਪਸ 0;
    }
    ਜੇ(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(ਅੰਕੜੇ, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
    }

    info = OSSL_STORE_ਲੱਦੋ(store);
    ਜੇ(info) {
      ਪੂਰਨ ossl_type = OSSL_STORE_INFO_get_type(info);

      ਜੇ(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    ਜੇ(!cert) {
      failf(ਅੰਕੜੇ, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      ਵਾਪਸ 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    ਜੇ(rc != 1) {
      failf(ਅੰਕੜੇ, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      ਵਾਪਸ 0;
    }
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "crypto provider not set, cannot load certificate");
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ 1;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)octx;
  (ਖਾਲੀ)cert_file;
  failf(ਅੰਕੜੇ, "SSL_FILETYPE_PROVIDER not supported for certificate");
  ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ pkcs12load(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      SSL_CTX* ctx,
                      ਅਚਲ ਬਣਤਰ curl_blob *cert_blob,
                      ਅਚਲ ਅੱਖਰ *cert_file,
                      ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_passwd)
{
  ਅੱਖਰ error_ਬਫਰ[256];
  BIO *cert_bio = ਨਿਰਾ;
  PKCS12 *p12 = ਨਿਰਾ;
  EVP_PKEY *pri;
  X509 *x509;
  ਪੂਰਨ cert_ਪੂਰਾ = 0;
  ਢੇਰ_ਦਾ(X509) *ca = ਨਿਰਾ;
  ਜੇ(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->ਅੰਕੜੇ, (ਪੂਰਨ)cert_blob->len);
    ਜੇ(!cert_bio) {
      failf(ਅੰਕੜੇ, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      ਵਾਪਸ 0;
    }
  }
  ਨਹੀਂਤਾਂ {
    cert_bio = BIO_new(BIO_s_file());
    ਜੇ(!cert_bio) {
      failf(ਅੰਕੜੇ, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
      ਵਾਪਸ 0;
    }

    ਜੇ(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(ਅੰਕੜੇ, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      ਵਾਪਸ 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, ਨਿਰਾ);
  BIO_free(cert_bio);

  ਜੇ(!p12) {
    failf(ਅੰਕੜੇ, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    ਵਾਪਸ 0;
  }

  ਜੇ(!PKCS12_parse(p12, ਕੁੰਜੀ_passwd, &pri, &x509, &ca)) {
    failf(ਅੰਕੜੇ, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ)));
    PKCS12_free(p12);
    ਵਾਪਸ 0;
  }

  PKCS12_free(p12);

  ਜੇ(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(ਅੰਕੜੇ, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ)));
    ਜਾਓ fail;
  }

  ਜੇ(SSL_CTX_use_ਨਿੱਜੀ_ਕੁੰਜੀ(ctx, pri) != 1) {
    failf(ਅੰਕੜੇ, "unable to use private key from PKCS12 file '%s'", cert_file);
    ਜਾਓ fail;
  }

  ਜੇ(!SSL_CTX_check_ਨਿੱਜੀ_ਕੁੰਜੀ(ctx)) {
    failf(ਅੰਕੜੇ, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    ਜਾਓ fail;
  }
  /* Set Certificate Verification chain */
  ਜੇ(ca) {
    ਜਦੋਂ(sk_X509_ਅੰਕ(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_ਕੱਢੋ(ca);
      ਜੇ(!SSL_CTX_ਜੋੜੋ_client_CA(ctx, x)) {
        X509_free(x);
        failf(ਅੰਕੜੇ, "cannot add certificate to client CA list");
        ਜਾਓ fail;
      }
      ਜੇ(!SSL_CTX_ਜੋੜੋ_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(ਅੰਕੜੇ, "cannot add certificate to certificate chain");
        ਜਾਓ fail;
      }
    }
  }

  cert_ਪੂਰਾ = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
  sk_X509_ਕੱਢੋ_free(ca, X509_free);
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  ਜੇ(!cert_ਪੂਰਾ)
    ਵਾਪਸ 0; /* failure! */
  ਵਾਪਸ 1;
}

ਸਥਿਰ CURLcode client_cert(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ossl_ctx *octx,
                            ਅੱਖਰ *cert_file,
                            ਅਚਲ ਬਣਤਰ curl_blob *cert_blob,
                            ਅਚਲ ਅੱਖਰ *cert_type,
                            ਅੱਖਰ *ਕੁੰਜੀ_file,
                            ਅਚਲ ਬਣਤਰ curl_blob *ਕੁੰਜੀ_blob,
                            ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_type,
                            ਅੱਖਰ *ਕੁੰਜੀ_passwd)
{
  ਅੱਖਰ error_ਬਫਰ[256];
  bool check_privkey = TRUE;
  ਪੂਰਨ file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  ਜੇ(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_ਪੂਰਾ = FALSE;
    ਪੂਰਨ cert_use_ਨਤੀਜਾ;

    ਜੇ(ਕੁੰਜੀ_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_ਸੈੱਟ_default_passwd_cb_userdata(ctx, ਕੁੰਜੀ_passwd);
      /* Set passwd callback: */
      SSL_CTX_ਸੈੱਟ_default_passwd_cb(ctx, passwd_callback);
    }

    ਚੋਣ(file_type) {
    ਹਾਲਤ SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_ਨਤੀਜਾ = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, ਕੁੰਜੀ_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      ਜੇ(cert_use_ਨਤੀਜਾ != 1) {
        failf(ਅੰਕੜੇ,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                            ਆਕਾਰ(error_ਬਫਰ)));
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }
      ਰੋਕੋ;

    ਹਾਲਤ SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_ਨਤੀਜਾ = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, ਕੁੰਜੀ_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      ਜੇ(cert_use_ਨਤੀਜਾ != 1) {
        failf(ਅੰਕੜੇ,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                            ਆਕਾਰ(error_ਬਫਰ)));
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }
      ਰੋਕੋ;

    ਹਾਲਤ SSL_FILETYPE_ENGINE:
      ਜੇ(!cert_file || !engineload(ਅੰਕੜੇ, octx, cert_file))
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      ਰੋਕੋ;

    ਹਾਲਤ SSL_FILETYPE_PROVIDER:
      ਜੇ(!cert_file || !providerload(ਅੰਕੜੇ, octx, cert_file))
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      ਰੋਕੋ;

    ਹਾਲਤ SSL_FILETYPE_PKCS12:
      ਜੇ(!pkcs12load(ਅੰਕੜੇ, ctx, cert_blob, cert_file, ਕੁੰਜੀ_passwd))
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      pcks12_ਪੂਰਾ = TRUE;
      ਰੋਕੋ;

    ਮੂਲ:
      failf(ਅੰਕੜੇ, "not supported file type '%s' for certificate", cert_type);
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }

    ਜੇ(!ਕੁੰਜੀ_file && !ਕੁੰਜੀ_blob) {
      ਕੁੰਜੀ_file = cert_file;
      ਕੁੰਜੀ_blob = cert_blob;
    }
    ਨਹੀਂਤਾਂ
      file_type = ossl_do_file_type(ਕੁੰਜੀ_type);

    ਚੋਣ(file_type) {
    ਹਾਲਤ SSL_FILETYPE_PEM:
    ਹਾਲਤ SSL_FILETYPE_ASN1:
      cert_use_ਨਤੀਜਾ = ਕੁੰਜੀ_blob ?
        use_privatekey_blob(ctx, ਕੁੰਜੀ_blob, file_type, ਕੁੰਜੀ_passwd) :
      SSL_CTX_use_ਨਿੱਜੀ_ਕੁੰਜੀ_file(ctx, ਕੁੰਜੀ_file, file_type);
      ਜੇ(cert_use_ਨਤੀਜਾ != 1) {
        failf(ਅੰਕੜੇ, "unable to set private key file: '%s' type %s",
              ਕੁੰਜੀ_file ? ਕੁੰਜੀ_file : "(memory blob)",
              ਕੁੰਜੀ_type ? ਕੁੰਜੀ_type : "PEM");
        ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
      }
      ਰੋਕੋ;
    ਹਾਲਤ SSL_FILETYPE_ENGINE:
      ਜੇ(!enginecheck(ਅੰਕੜੇ, octx, ਕੁੰਜੀ_file, ਕੁੰਜੀ_passwd))
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      ਰੋਕੋ;

    ਹਾਲਤ SSL_FILETYPE_PROVIDER:
      ਜੇ(!providercheck(ਅੰਕੜੇ, octx, ਕੁੰਜੀ_file))
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      ਰੋਕੋ;

    ਹਾਲਤ SSL_FILETYPE_PKCS12:
      ਜੇ(!pcks12_ਪੂਰਾ) {
        failf(ਅੰਕੜੇ, "file type P12 for private key not supported");
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }
      ਰੋਕੋ;
    ਮੂਲ:
      failf(ਅੰਕੜੇ, "not supported file type for private key");
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }

    ssl = SSL_new(ctx);
    ਜੇ(!ssl) {
      failf(ਅੰਕੜੇ, "unable to create an SSL structure");
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    x509 = SSL_get_certificate(ssl);

    ਜੇ(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#ਜੇ !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_ਕੁੰਜੀ = SSL_get_privatekey(ssl);
      ਜੇ(EVP_PKEY_id(priv_ਕੁੰਜੀ) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_ਕੁੰਜੀ);
        ਜੇ(RSA_flags(rsa) & RSA_ਵਿਧੀ_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#ਜੇ_ਅੰਤ

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    ਜੇ(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      ਜੇ(!SSL_CTX_check_ਨਿੱਜੀ_ਕੁੰਜੀ(ctx)) {
        failf(ਅੰਕੜੇ, "Private key does not match the certificate public key");
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
/* returns non-zero on failure */
ਸਥਿਰ CURLcode x509_name_oneline(ਅਚਲ X509_NAME *a, ਬਣਤਰ dynbuf *d)
{
  BIO *bio_ਬਾਹਰਲਾ = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  ਪੂਰਨ rc;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(bio_ਬਾਹਰਲਾ) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_ਬਾਹਰਲਾ, a, 0, flags);
    ਜੇ(rc != -1) {
      BIO_get_mem_ptr(bio_ਬਾਹਰਲਾ, &biomem);
      ਨਤੀਜਾ = curlx_dyn_addn(d, biomem->ਅੰਕੜੇ, biomem->ਲੰਮਾਈ);
    }
    BIO_free(bio_ਬਾਹਰਲਾ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ਸਥਿਰ ਪੂਰਨ ossl_init(ਖਾਲੀ)
{
  ਅਚਲ uint64_t flags =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_OPENSSL_AUTO_ਲੱਦੋ_CONFIG
    OPENSSL_INIT_NO_ਲੱਦੋ_CONFIG |
#ਨਹੀਂਤਾਂ
    OPENSSL_INIT_ਲੱਦੋ_CONFIG |
#ਜੇ_ਅੰਤ
    0;
  OPENSSL_init_ssl(flags, ਨਿਰਾ);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#ਜੇ_ਅੰਤ

  ਵਾਪਸ 1;
}

/* Global cleanup */
ਸਥਿਰ ਖਾਲੀ ossl_cleanup(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#ਜੇ_ਅੰਤ
}

/* Selects an OpenSSL crypto engine or provider.
 */
ਸਥਿਰ CURLcode ossl_ਸੈੱਟ_engine(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ossl_ctx *octx,
                                ਅਚਲ ਅੱਖਰ *name)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL_ENGINE
  CURLcode ਨਤੀਜਾ = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  ਜੇ(e) {

    ਜੇ(octx->p.engine) {
      ENGINE_ਅੰਤ(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = ਨਿਰਾ;
    }
    ਜੇ(!ENGINE_init(e)) {
      ਅੱਖਰ buf[256];

      ENGINE_free(e);
      failf(ਅੰਕੜੇ, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, ਆਕਾਰ(buf)));
      ਨਤੀਜਾ = CURLE_SSL_ENGINE_INITFAILED;
      e = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ {
      ਨਤੀਜਾ = CURLE_OK;
    }
    octx->p.engine = e;
    ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_HAS_PROVIDERS
  ਵਾਪਸ ossl_ਸੈੱਟ_provider(ਅੰਕੜੇ, octx, name);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)octx;
  (ਖਾਲੀ)name;
  failf(ਅੰਕੜੇ, "OpenSSL engine not found");
  ਵਾਪਸ CURLE_SSL_ENGINE_NOTFOUND;
#ਜੇ_ਅੰਤ
}

/* Sets engine as default for all SSL operations
 */
ਸਥਿਰ CURLcode ossl_ਸੈੱਟ_engine_default(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ossl_ctx *octx)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL_ENGINE
  ਜੇ(octx->p.engine) {
    ਜੇ(ENGINE_ਸੈੱਟ_default(octx->p.engine, ENGINE_ਵਿਧੀ_ALL) > 0) {
      infof(ਅੰਕੜੇ, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      ਵਾਪਸ CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)octx;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
ਸਥਿਰ ਬਣਤਰ curl_slist *ossl_engines_list(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_slist *list = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL_ENGINE
  ਬਣਤਰ curl_slist *beg;
  ENGINE *e;

  ਲਈ(e = ENGINE_get_ਪਹਿਲਾ(); e; e = ENGINE_get_ਅਗਲਾ(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    ਜੇ(!beg) {
      curl_slist_free_all(list);
      ਵਾਪਸ ਨਿਰਾ;
    }
    list = beg;
  }
#ਜੇ_ਅੰਤ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ list;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_HAS_PROVIDERS

ਸਥਿਰ ਖਾਲੀ ossl_provider_cleanup(ਬਣਤਰ ossl_ctx *octx)
{
  ਜੇ(!octx)
    ਵਾਪਸ;
  ਜੇ(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = ਨਿਰਾ;
  }
  ਜੇ(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = ਨਿਰਾ;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = ਨਿਰਾ;
  curlx_safefree(octx->p.propq);
  octx->p.provider_loaded = FALSE;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_PROVIDER_LEN 128 /* reasonable */

/* Selects an OpenSSL crypto provider.
 *
 * A provider might need an associated property, a string passed on to
 * OpenSSL. Specify this as [PROVIDER][:PROPERTY]: separate the name and the
 * property with a colon. No colon means no property is set.
 *
 * An example provider + property looks like "tpm2:?provider=tpm2".
 */
ਸਥਿਰ CURLcode ossl_ਸੈੱਟ_provider(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ossl_ctx *octx,
                                  ਅਚਲ ਅੱਖਰ *iname)
{
  ਅੱਖਰ name[MAX_PROVIDER_LEN + 1];
  ਬਣਤਰ Curl_str prov;
  ਅਚਲ ਅੱਖਰ *propq = ਨਿਰਾ;

  ਜੇ(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    ਵਾਪਸ CURLE_OK;
  }
  ਜੇ(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  ਜੇ(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    ਜੇ(!libctx)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜੇ(propq) {
      octx->p.propq = curlx_strdup(propq);
      ਜੇ(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      }
    }
    octx->p.libctx = libctx;
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_OPENSSL_AUTO_ਲੱਦੋ_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  ਜੇ(!OSSL_LIB_CTX_ਲੱਦੋ_config(octx->p.libctx, ਨਿਰਾ)) {
    infof(ਅੰਕੜੇ, "Failed to load default openssl config. Proceeding.");
  }
#ਜੇ_ਅੰਤ

  ਜੇ(OSSL_PROVIDER_ਉਪਲਬਧ(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_ਲੱਦੋ(octx->p.libctx, name, 1);
  ਜੇ(!octx->p.provider) {
    ਅੱਖਰ error_ਬਫਰ[256];
    failf(ਅੰਕੜੇ, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ)));
    ossl_provider_cleanup(octx);
    ਵਾਪਸ CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_ਲੱਦੋ(octx->p.libctx, "base", 1);
  ਜੇ(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(ਅੰਕੜੇ, "Failed to load base");
    ਵਾਪਸ CURLE_SSL_ENGINE_NOTFOUND;
  }
  ਨਹੀਂਤਾਂ
    octx->p.provider_loaded = TRUE;
  ਵਾਪਸ CURLE_OK;
}
#ਨਹੀਂਤਾਂ /* OPENSSL_HAS_PROVIDERS */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ossl_provider_cleanup(x)
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode ossl_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              bool send_shutdown, bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅੱਖਰ buf[1024];
  ਪੂਰਨ nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  ਜੇ(!octx->ssl || cf->shutdown) {
    *ਪੂਰਾ = TRUE;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *ਪੂਰਾ = FALSE;
  ਜੇ(!(SSL_get_shutdown(octx->ssl) & SSL_ਭੇਜਿਆ_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    ਲਈ(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (ਪੂਰਨ)ਆਕਾਰ(buf));
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown not sent, read -> %d", nread);
      ਜੇ(nread <= 0)
        ਰੋਕੋ;
    }
    err = SSL_get_error(octx->ssl, nread);
    ਜੇ(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool ਨਿਵੇਸ਼_pending;
      /* Yes, it did. */
      ਜੇ(!send_shutdown) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown received, not sending");
        *ਪੂਰਾ = TRUE;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(!cf->ਅਗਲਾ->cft->is_alive(cf->ਅਗਲਾ, ਅੰਕੜੇ, &ਨਿਵੇਸ਼_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "peer closed connection");
        *ਪੂਰਾ = TRUE;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ਜੇ(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_ਭੇਜਿਆ_SHUTDOWN)) {
    ਪੂਰਨ rc;
    ERR_clear_error();
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    ਜੇ(rc == 1) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown finished");
      *ਪੂਰਾ = TRUE;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  ਲਈ(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (ਪੂਰਨ)ਆਕਾਰ(buf));
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown read -> %d", nread);
    ਜੇ(nread <= 0)
      ਰੋਕੋ;
  }
  err = SSL_get_error(octx->ssl, nread);
  ਚੋਣ(err) {
  ਹਾਲਤ SSL_ERROR_ZERO_RETURN: /* no more data */
    ਜੇ(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown finished");
    ਨਹੀਂਤਾਂ
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown not received, but closed");
    *ਪੂਰਾ = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ SSL_ERROR_NONE: /* did not get anything */
  ਹਾਲਤ SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ਰੋਕੋ;
  ਹਾਲਤ SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ਰੋਕੋ;
  ਮੂਲ:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sslerr = ERR_get_error());
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, ਆਕਾਰ(buf)) :
                   SSL_ERROR_ਮੰਜ਼ਿਲ_str(err)),
                  SOCKERRNO);
    }
    *ਪੂਰਾ = TRUE;
    ਨਤੀਜਾ = CURLE_OK;
    ਰੋਕੋ;
  }

ਬਾਹਰਲਾ:
  cf->shutdown = (ਨਤੀਜਾ || *ਪੂਰਾ);
  ਜੇ(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->ਨਿਵੇਸ਼_pending = FALSE;
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* also used by quic-tls */
ਖਾਲੀ Curl_osslctx_cleanup(ਬਣਤਰ ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  ਜੇ(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = ਨਿਰਾ;
  }
  ਜੇ(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = ਨਿਰਾ;
    octx->x509_store_ਤਿਆਰੀ = FALSE;
  }
  ਜੇ(octx->bio_ਵਿਧੀ) {
    ossl_bio_cf_ਵਿਧੀ_free(octx->bio_ਵਿਧੀ);
    octx->bio_ਵਿਧੀ = ਨਿਰਾ;
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL_ENGINE
  ਜੇ(octx->p.engine) {
    ENGINE_ਅੰਤ(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = ਨਿਰਾ;
  }
#ਜੇ_ਅੰਤ
  ossl_provider_cleanup(octx);
}

ਸਥਿਰ ਖਾਲੀ ossl_close(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
ਸਥਿਰ ਖਾਲੀ ossl_close_all(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
}

/* ====================================================== */

/* Quote from RFC2818 section 3.1 "Server Identity"

   If a subjectAltName extension of type dNSName is present, that MUST
   be used as the identity. Otherwise, the (most specific) Common Name
   field in the Subject field of the certificate MUST be used. Although
   the use of the Common Name is existing practice, it is deprecated and
   Certification Authorities are encouraged to use the dNSName instead.

   Matching is performed using the matching rules specified by
   [RFC2459]. If more than one identity of a given type is present in
   the certificate (e.g., more than one dNSName name, a match in any one
   of the set is considered acceptable.) Names may contain the wildcard
   character * which is considered to match any single domain name
   component or component fragment. E.g., *.a.com matches foo.a.com but
   not bar.foo.a.com. f*.com matches foo.com but not bar.com.

   In some cases, the URI is specified as an IP address rather than a
   hostname. In this case, the iPAddress subjectAltName must be present
   in the certificate and must exactly match the IP in the URI.

   This function is now used from ngtcp2 (QUIC) as well.
 */
ਸਥਿਰ CURLcode ossl_verifyhost(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ connectdata *conn,
                                ਬਣਤਰ ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  ਪੂਰਨ ਨਿਸ਼ਾਨਾ; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  ਢੇਰ_ਦਾ(GENERAL_NAME) *altnames;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਬਣਤਰ in6_addr addr;
#ਨਹੀਂਤਾਂ
  ਬਣਤਰ in_addr addr;
#ਜੇ_ਅੰਤ
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = ਲੜੀ_ਲੰਬਾਈ(peer->origin->hostname);

  (ਖਾਲੀ)conn;
  ਚੋਣ(peer->type) {
  ਹਾਲਤ CURL_SSL_PEER_IPV4:
    ਜੇ(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
    ਨਿਸ਼ਾਨਾ = GEN_IPADD;
    addrlen = ਆਕਾਰ(ਬਣਤਰ in_addr);
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਹਾਲਤ CURL_SSL_PEER_IPV6:
    ਜੇ(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
    ਨਿਸ਼ਾਨਾ = GEN_IPADD;
    addrlen = ਆਕਾਰ(ਬਣਤਰ in6_addr);
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURL_SSL_PEER_DNS:
    ਨਿਸ਼ਾਨਾ = GEN_DNS;
    ਰੋਕੋ;
  ਮੂਲ:
    DEBUGASSERT(0);
    failf(ਅੰਕੜੇ, "unexpected SSL peer type: %d", (ਪੂਰਨ)peer->type);
    ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, ਨਿਰਾ, ਨਿਰਾ);

  ਜੇ(altnames) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#ਨਹੀਂਤਾਂ
    ਪੂਰਨ numalts;
    ਪੂਰਨ i;
#ਜੇ_ਅੰਤ

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_ਅੰਕ(altnames);

    /* loop through all alternatives - until a dnsmatch */
    ਲਈ(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ਅਚਲ GENERAL_NAME *check = sk_GENERAL_NAME_ਮੁੱਲ(altnames, i);

      ਜੇ(check->type == GEN_DNS)
        dNSName = TRUE;
      ਨਹੀਂਤਾਂ ਜੇ(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      ਜੇ(check->type == ਨਿਸ਼ਾਨਾ) {
        /* get data and length */
        ਅਚਲ ਅੱਖਰ *altptr = (ਅਚਲ ਅੱਖਰ *)ASN1_STRING_get0_ਅੰਕੜੇ(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_ਲੰਮਾਈ(check->d.ia5);

        ਚੋਣ(ਨਿਸ਼ਾਨਾ) {
        ਹਾਲਤ GEN_DNS: /* name/pattern comparison */
          ਜੇ(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(ਅੰਕੜੇ, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (ਪੂਰਨ)altlen, altptr);
          }
          ਰੋਕੋ;

        ਹਾਲਤ GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          ਜੇ((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(ਅੰਕੜੇ, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          ਰੋਕੋ;
        }
      }
    }
    GENERAL_ਨਾਂ_free(altnames);
  }

  ਜੇ(matched)
    /* an alternative name matched */
    ;
  ਨਹੀਂਤਾਂ ਜੇ(dNSName || iPAddress) {
    ਅਚਲ ਅੱਖਰ *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(ਅੰਕੜੇ, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(ਅੰਕੜੇ, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
  }
  ਨਹੀਂਤਾਂ {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    ਪੂਰਨ i = -1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *cn = ਨਿਰਾ;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ਅਚਲ X509_NAME *name = X509_get_subject_name(server_cert);
    ਜੇ(name) {
      ਪੂਰਨ j;
      ਜਦੋਂ((j = X509_NAME_get_ਸੂਚਕਾਂਕ_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    ਜੇ(i >= 0) {
      ਅਚਲ ASN1_STRING *tmp =
        X509_NAME_ਇੰਦਰਾਜ_get_ਅੰਕੜੇ(X509_NAME_get_ਇੰਦਰਾਜ(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      ਜੇ(tmp) {
        ਜੇ(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_ਲੰਮਾਈ(tmp);
          cn = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)CURL_UNCONST(ASN1_STRING_get0_ਅੰਕੜੇ(tmp));
        }
        ਨਹੀਂਤਾਂ { /* not a UTF8 name */
          ਅਚਲ ਪੂਰਨ utf8len = ASN1_STRING_ਮੰਜ਼ਿਲ_UTF8(&cn, tmp);
          ਜੇ(utf8len < 0)
            cnlen = 0;
          ਨਹੀਂਤਾਂ
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        ਜੇ((cnlen == 0) || !cn)
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਨਹੀਂਤਾਂ ਜੇ(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(ਅੰਕੜੇ, "SSL: illegal cert name field");
          ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    ਜੇ(ਨਤੀਜਾ)
      /* error already detected, pass through */
      ;
    ਨਹੀਂਤਾਂ ਜੇ(!cn) {
      failf(ਅੰਕੜੇ, "SSL: unable to obtain common name from peer certificate");
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!Curl_cert_hostcheck((ਅਚਲ ਅੱਖਰ *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(ਅੰਕੜੇ, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (ਪੂਰਨ)cnlen, cn,
            peer->origin->user_hostname);
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, " common name: %.*s (matched)", (ਪੂਰਨ)cnlen, cn);
    }
    ਜੇ(free_cn)
      OPENSSL_free(cn);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_NO_OCSP
ਸਥਿਰ CURLcode verifystatus(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ ossl_ctx *octx)
{
  ਪੂਰਨ i, ocsp_ਸਥਿਤੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
  ਅਚਲ uint8_t *ਸਥਿਤੀ;
#ਨਹੀਂਤਾਂ
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸਥਿਤੀ;
#ਜੇ_ਅੰਤ
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *p;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  OCSP_RESPONSE *rsp = ਨਿਰਾ;
  OCSP_BASICRESP *br = ਨਿਰਾ;
  X509_STORE     *st = ਨਿਰਾ;
  ਢੇਰ_ਦਾ(X509) *ch = ਨਿਰਾ;
  X509 *cert;
  OCSP_CERTID *id = ਨਿਰਾ;
  ਪੂਰਨ cert_ਸਥਿਤੀ, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  ਪੂਰਨ ret;
  ਲੰਮਾ len;

  (ਖਾਲੀ)cf;
  DEBUGASSERT(octx);

  len = (ਲੰਮਾ)SSL_get_tlsext_ਸਥਿਤੀ_ocsp_resp(octx->ssl, &ਸਥਿਤੀ);

  ਜੇ(!ਸਥਿਤੀ) {
    failf(ਅੰਕੜੇ, "No OCSP response received");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }
  p = ਸਥਿਤੀ;
  rsp = d2i_OCSP_RESPONSE(ਨਿਰਾ, &p, len);
  ਜੇ(!rsp) {
    failf(ਅੰਕੜੇ, "Invalid OCSP response");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }

  ocsp_ਸਥਿਤੀ = OCSP_response_ਸਥਿਤੀ(rsp);
  ਜੇ(ocsp_ਸਥਿਤੀ != OCSP_RESPONSE_ਸਥਿਤੀ_SUCCESSFUL) {
    failf(ਅੰਕੜੇ, "Invalid OCSP response status: %s (%d)",
          OCSP_response_ਸਥਿਤੀ_str(ocsp_ਸਥਿਤੀ), ocsp_ਸਥਿਤੀ);
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }

  br = OCSP_response_get1_basic(rsp);
  ਜੇ(!br) {
    failf(ਅੰਕੜੇ, "Invalid OCSP response");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }

#ਜੇ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ) && LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#ਨਹੀਂਤਾਂ
  ch = SSL_get_peer_cert_chain(octx->ssl);
#ਜੇ_ਅੰਤ
  ਜੇ(!ch) {
    failf(ਅੰਕੜੇ, "Could not get peer certificate chain");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  ਜੇ(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(ਅੰਕੜੇ, "OCSP response verification failed");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  ਜੇ(!cert) {
    failf(ਅੰਕੜੇ, "Error getting peer certificate");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }

  ਲਈ(i = 0; i < (ਪੂਰਨ)sk_X509_ਅੰਕ(ch); i++) {
    X509 *issuer = sk_X509_ਮੁੱਲ(ch, (ossl_valsize_t)i);
    ਜੇ(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_ਮੰਜ਼ਿਲ_id(EVP_sha1(), cert, issuer);
      ਰੋਕੋ;
    }
  }
  X509_free(cert);

  ਜੇ(!id) {
    failf(ਅੰਕੜੇ, "Error computing OCSP ID");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_ਲੱਭੋ_ਸਥਿਤੀ(br, id, &cert_ਸਥਿਤੀ, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  ਜੇ(ret != 1) {
    failf(ਅੰਕੜੇ, "Could not find certificate ID in OCSP response");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  ਜੇ(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(ਅੰਕੜੇ, "OCSP response has expired");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }

  infof(ਅੰਕੜੇ, "SSL certificate status: %s (%d)",
        OCSP_cert_ਸਥਿਤੀ_str(cert_ਸਥਿਤੀ), cert_ਸਥਿਤੀ);

  ਚੋਣ(cert_ਸਥਿਤੀ) {
  ਹਾਲਤ V_OCSP_CERTSTATUS_GOOD:
    ਰੋਕੋ;

  ਹਾਲਤ V_OCSP_CERTSTATUS_REVOKED:
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    failf(ਅੰਕੜੇ, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    ਜਾਓ ਸਿਰਾ;

  ਹਾਲਤ V_OCSP_CERTSTATUS_UNKNOWN:
  ਮੂਲ:
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਸਿਰਾ;
  }

ਸਿਰਾ:
  ਜੇ(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ssl_ਸੁਨੇਹਾ_type(ਪੂਰਨ ssl_ver, ਪੂਰਨ ਸੁਨੇਹਾ)
{
  ਜੇ(ssl_ver == SSL3_ਸੰਸਕਰਣ_MAJOR) {
    ਚੋਣ(ਸੁਨੇਹਾ) {
    ਹਾਲਤ SSL3_MT_HELLO_REQUEST:
      ਵਾਪਸ "Hello request";
    ਹਾਲਤ SSL3_MT_CLIENT_HELLO:
      ਵਾਪਸ "Client hello";
    ਹਾਲਤ SSL3_MT_SERVER_HELLO:
      ਵਾਪਸ "Server hello";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_MT_NEWSESSION_TICKET
    ਹਾਲਤ SSL3_MT_NEWSESSION_TICKET:
      ਵਾਪਸ "Newsession Ticket";
#ਜੇ_ਅੰਤ
    ਹਾਲਤ SSL3_MT_CERTIFICATE:
      ਵਾਪਸ "Certificate";
    ਹਾਲਤ SSL3_MT_SERVER_ਕੁੰਜੀ_EXCHANGE:
      ਵਾਪਸ "Server key exchange";
    ਹਾਲਤ SSL3_MT_CLIENT_ਕੁੰਜੀ_EXCHANGE:
      ਵਾਪਸ "Client key exchange";
    ਹਾਲਤ SSL3_MT_CERTIFICATE_REQUEST:
      ਵਾਪਸ "Request CERT";
    ਹਾਲਤ SSL3_MT_SERVER_ਪੂਰਾ:
      ਵਾਪਸ "Server finished";
    ਹਾਲਤ SSL3_MT_CERTIFICATE_VERIFY:
      ਵਾਪਸ "CERT verify";
    ਹਾਲਤ SSL3_MT_ਮੁਕੰਮਲ:
      ਵਾਪਸ "Finished";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_MT_CERTIFICATE_ਸਥਿਤੀ
    ਹਾਲਤ SSL3_MT_CERTIFICATE_ਸਥਿਤੀ:
      ਵਾਪਸ "Certificate Status";
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_MT_ENCRYPTED_EXTENSIONS
    ਹਾਲਤ SSL3_MT_ENCRYPTED_EXTENSIONS:
      ਵਾਪਸ "Encrypted Extensions";
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_MT_SUPPLEMENTAL_ਅੰਕੜੇ
    ਹਾਲਤ SSL3_MT_SUPPLEMENTAL_ਅੰਕੜੇ:
      ਵਾਪਸ "Supplemental data";
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_MT_ਸਿਰਾ_ਦਾ_EARLY_ਅੰਕੜੇ
    ਹਾਲਤ SSL3_MT_ਸਿਰਾ_ਦਾ_EARLY_ਅੰਕੜੇ:
      ਵਾਪਸ "End of early data";
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_MT_ਕੁੰਜੀ_UPDATE
    ਹਾਲਤ SSL3_MT_ਕੁੰਜੀ_UPDATE:
      ਵਾਪਸ "Key update";
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_MT_ਅਗਲਾ_PROTO
    ਹਾਲਤ SSL3_MT_ਅਗਲਾ_PROTO:
      ਵਾਪਸ "Next protocol";
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_MT_ਸੁਨੇਹਾ_HASH
    ਹਾਲਤ SSL3_MT_ਸੁਨੇਹਾ_HASH:
      ਵਾਪਸ "Message hash";
#ਜੇ_ਅੰਤ
    }
  }
  ਵਾਪਸ "Unknown";
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *tls_rt_type(ਪੂਰਨ type)
{
  ਚੋਣ(type) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_RT_ਸਿਰਲੇਖ
  ਹਾਲਤ SSL3_RT_ਸਿਰਲੇਖ:
    ਵਾਪਸ "TLS header";
#ਜੇ_ਅੰਤ
  ਹਾਲਤ SSL3_RT_CHANGE_CIPHER_SPEC:
    ਵਾਪਸ "TLS change cipher";
  ਹਾਲਤ SSL3_RT_ALERT:
    ਵਾਪਸ "TLS alert";
  ਹਾਲਤ SSL3_RT_HANDSHAKE:
    ਵਾਪਸ "TLS handshake";
  ਹਾਲਤ SSL3_RT_APPLICATION_ਅੰਕੜੇ:
    ਵਾਪਸ "TLS app data";
  ਮੂਲ:
    ਵਾਪਸ "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
ਸਥਿਰ ਖਾਲੀ ossl_trace(ਪੂਰਨ direction, ਪੂਰਨ ssl_ver, ਪੂਰਨ content_type,
                       ਅਚਲ ਖਾਲੀ *buf, size_t len, SSL *ssl,
                       ਖਾਲੀ *userp)
{
  ਅਚਲ ਅੱਖਰ *verstr;
  ਬਣਤਰ Curl_cfilter *cf = userp;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = ਨਿਰਾ;
  ਅੱਖਰ unknown[32];

  ਜੇ(!cf)
    ਵਾਪਸ;
  ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  ਜੇ(!ਅੰਕੜੇ || !ਅੰਕੜੇ->ਸੈੱਟ.fdebug || (direction && direction != 1))
    ਵਾਪਸ;

  ਚੋਣ(ssl_ver) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_ਸੰਸਕਰਣ
  ਹਾਲਤ SSL3_ਸੰਸਕਰਣ:
    verstr = "SSLv3";
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ TLS1_ਸੰਸਕਰਣ:
    verstr = "TLSv1.0";
    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TLS1_1_ਸੰਸਕਰਣ
  ਹਾਲਤ TLS1_1_ਸੰਸਕਰਣ:
    verstr = "TLSv1.1";
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TLS1_2_ਸੰਸਕਰਣ
  ਹਾਲਤ TLS1_2_ਸੰਸਕਰਣ:
    verstr = "TLSv1.2";
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ TLS1_3_ਸੰਸਕਰਣ:
    verstr = "TLSv1.3";
    ਰੋਕੋ;
  ਮੂਲ:
    curl_msnprintf(unknown, ਆਕਾਰ(unknown), "(%x)", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ssl_ver);
    verstr = unknown;
    ਰੋਕੋ;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  ਜੇ(ssl_ver
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_RT_ਸਿਰਲੇਖ
     && content_type != SSL3_RT_ਸਿਰਲੇਖ
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#ਜੇ_ਅੰਤ
    ) {
    ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ_name = "Truncated message";
    ਅਚਲ ਅੱਖਰ *tls_rt_name;
    ਅੱਖਰ ssl_buf[1024];
    ਪੂਰਨ ਸੁਨੇਹਾ_type = 0;
    ਪੂਰਨ txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    ਜੇ(ssl_ver == SSL3_ਸੰਸਕਰਣ_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    ਨਹੀਂਤਾਂ
      tls_rt_name = "";

    ਜੇ(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      ਜੇ(len) {
        ਸੁਨੇਹਾ_type = *(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf;
        ਸੁਨੇਹਾ_name = "Change cipher spec";
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(content_type == SSL3_RT_ALERT) {
      ਜੇ(len >= 2) {
        ਸੁਨੇਹਾ_type =
          (((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf)[0] << 8) +
           ((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf)[1];
        ਸੁਨੇਹਾ_name = SSL_alert_desc_string_long(ਸੁਨੇਹਾ_type);
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(len) {
      ਸੁਨੇਹਾ_type = *(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf;
      ਸੁਨੇਹਾ_name = ssl_ਸੁਨੇਹਾ_type(ssl_ver, ਸੁਨੇਹਾ_type);
    }

    txt_len = curl_msnprintf(ssl_buf, ਆਕਾਰ(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, ਸੁਨੇਹਾ_name, ਸੁਨੇਹਾ_type);
    Curl_debug(ਅੰਕੜੇ, CURLINFO_ਪਾਠ, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(ਅੰਕੜੇ, (direction == 1) ? CURLINFO_SSL_ਅੰਕੜੇ_ਬਾਹਰਲਾ :
             CURLINFO_SSL_ਅੰਕੜੇ_ਵਿੱਚ, (ਅਚਲ ਅੱਖਰ *)buf, len);
  (ਖਾਲੀ)ssl;
}

ਸਥਿਰ CURLcode ossl_ਸੈੱਟ_ssl_ਸੰਸਕਰਣ_min_max(ਬਣਤਰ Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ssl_ਸੰਸਕਰਣ_min)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  ਲੰਮਾ curl_ssl_ਸੰਸਕਰਣ_min = (ਲੰਮਾ)ssl_ਸੰਸਕਰਣ_min;
  ਲੰਮਾ curl_ssl_ਸੰਸਕਰਣ_max;

  /* convert curl min SSL version option to OpenSSL constant */
#ਜੇ defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ)
  uint16_t ossl_ssl_ਸੰਸਕਰਣ_min = 0;
  uint16_t ossl_ssl_ਸੰਸਕਰਣ_max = 0;
#ਨਹੀਂਤਾਂ
  ਲੰਮਾ ossl_ssl_ਸੰਸਕਰਣ_min = 0;
  ਲੰਮਾ ossl_ssl_ਸੰਸਕਰਣ_max = 0;
#ਜੇ_ਅੰਤ
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_ਸੰਸਕਰਣ_min != CURL_SSLVERSION_DEFAULT);
  ਚੋਣ(curl_ssl_ਸੰਸਕਰਣ_min) {
  ਹਾਲਤ CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_ਸੰਸਕਰਣ_min = TLS1_ਸੰਸਕਰਣ;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_ਸੰਸਕਰਣ_min = TLS1_1_ਸੰਸਕਰਣ;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_ਸੰਸਕਰਣ_min = TLS1_2_ਸੰਸਕਰਣ;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_ਸੰਸਕਰਣ_min = TLS1_3_ਸੰਸਕਰਣ;
    ਰੋਕੋ;
  }

  /* ... then, TLS max version */
  curl_ssl_ਸੰਸਕਰਣ_max = (ਲੰਮਾ)conn_config->ਸੰਸਕਰਣ_max;

  /* convert curl max SSL version option to OpenSSL constant */
  ਚੋਣ(curl_ssl_ਸੰਸਕਰਣ_max) {
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_ਸੰਸਕਰਣ_max = TLS1_ਸੰਸਕਰਣ;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_ਸੰਸਕਰਣ_max = TLS1_1_ਸੰਸਕਰਣ;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_ਸੰਸਕਰਣ_max = TLS1_2_ਸੰਸਕਰਣ;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_ਸੰਸਕਰਣ_max = TLS1_3_ਸੰਸਕਰਣ;
    ਰੋਕੋ;
  ਹਾਲਤ CURL_SSLVERSION_MAX_NONE:  /* none selected */
  ਹਾਲਤ CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  ਮੂਲ:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_ਸੰਸਕਰਣ_max = 0;
    ਰੋਕੋ;
  }

  ਜੇ(!SSL_CTX_ਸੈੱਟ_min_proto_ਸੰਸਕਰਣ(ctx, ossl_ssl_ਸੰਸਕਰਣ_min) ||
     !SSL_CTX_ਸੈੱਟ_max_proto_ਸੰਸਕਰਣ(ctx, ossl_ssl_ਸੰਸਕਰਣ_max))
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;

  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_ossl_ਜੋੜੋ_session(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ossl_ctx *octx,
                               ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                               SSL_SESSION *session,
                               ਅਚਲ ਅੱਖਰ *alpn,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *quic_tp,
                               size_t quic_tp_len,
                               ਬਣਤਰ Curl_ssl_session **psession)
{
  ਬਣਤਰ Curl_ssl_session *sc_session = ਨਿਰਾ, *sc_dup = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *der_session_buf = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *qtp_clone = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(psession)
    *psession = ਨਿਰਾ;
  ਜੇ(!cf || !ਅੰਕੜੇ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(Curl_ssl_scache_use(cf, ਅੰਕੜੇ)) {
    size_t der_session_ਮਾਪ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *der_session_ptr;
    size_t earlydata_max = 0;
    ਪੂਰਨ ietf_tls_id = SSL_ਸੰਸਕਰਣ(octx->ssl);

    der_session_ਮਾਪ = i2d_SSL_SESSION(session, ਨਿਰਾ);
    ਜੇ(der_session_ਮਾਪ == 0) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_ਮਾਪ);
    ਜੇ(!der_session_buf) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    der_session_ਮਾਪ = i2d_SSL_SESSION(session, &der_session_ptr);
    ਜੇ(der_session_ਮਾਪ == 0) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_ਅੰਕੜੇ(session);
#ਜੇ_ਅੰਤ
    ਜੇ(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ਅਚਲ ਅੱਖਰ *)quic_tp, quic_tp_len);
      ਜੇ(!qtp_clone) {
        ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }

    ਨਤੀਜਾ = Curl_ssl_session_create2(der_session_buf, der_session_ਮਾਪ,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)ਸਮਾਂ(ਨਿਰਾ) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = ਨਿਰਾ;  /* took ownership of sdata */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
    ਜੇ(!ਨਤੀਜਾ)
      sc_session->sectrust_verified = octx->sectrust_verified;
#ਜੇ_ਅੰਤ
    ਜੇ(!ਨਤੀਜਾ && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        ਨਤੀਜਾ = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    ਜੇ(!ਨਤੀਜਾ) {
      ਨਤੀਜਾ = Curl_ssl_scache_put(cf, ਅੰਕੜੇ, ssl_peer_ਕੁੰਜੀ, sc_session);
      /* took ownership of `sc_session` */
      sc_session = ਨਿਰਾ;
    }
  }

ਬਾਹਰਲਾ:
  curlx_free(der_session_buf);
  ਜੇ(!ਨਤੀਜਾ && psession) {
    *psession = sc_dup;
    sc_dup = ਨਿਰਾ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ਸਥਿਰ ਪੂਰਨ ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ਬਣਤਰ Curl_cfilter *cf = (ਬਣਤਰ Curl_cfilter *)SSL_get_app_ਅੰਕੜੇ(ssl);
  ਜੇ(cf) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
    ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
    ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
    ਬਣਤਰ Curl_ssl_session *session = ਨਿਰਾ;
    Curl_ossl_ਜੋੜੋ_session(cf, ਅੰਕੜੇ, octx, connssl->peer.scache_ਕੁੰਜੀ,
                          ssl_sessionid, connssl->negotiated.alpn, ਨਿਰਾ,
                          0, &session);
    ਜੇ(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ CURLcode ਲੱਦੋ_cacert_from_ਸਮ੍ਰਿਤੀ(X509_STORE *store,
                                        ਅਚਲ ਬਣਤਰ curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = ਨਿਰਾ;
  ਢੇਰ_ਦਾ(X509_INFO) *inf = ਨਿਰਾ;

  /* everything else is a reference */
  ਪੂਰਨ i, ਗਿਣਤੀ_2 = 0;
  X509_INFO *itmp = ਨਿਰਾ;

  ਜੇ(ca_info_blob->len > (size_t)ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
    ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->ਅੰਕੜੇ, (ਪੂਰਨ)ca_info_blob->len);
  ਜੇ(!cbio)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  inf = PEM_X509_INFO_read_bio(cbio, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
  ਜੇ(!inf) {
    BIO_free(cbio);
    ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  ਲਈ(i = 0; i < (ਪੂਰਨ)sk_X509_INFO_ਅੰਕ(inf); ++i) {
    itmp = sk_X509_INFO_ਮੁੱਲ(inf, (ossl_valsize_t)i);
    ਜੇ(itmp->x509) {
      ਜੇ(X509_STORE_ਜੋੜੋ_cert(store, itmp->x509)) {
        ++ਗਿਣਤੀ_2;
      }
      ਨਹੀਂਤਾਂ {
        /* set count to 0 to return an error */
        ਗਿਣਤੀ_2 = 0;
        ਰੋਕੋ;
      }
    }
    ਜੇ(itmp->crl) {
      ਜੇ(X509_STORE_ਜੋੜੋ_crl(store, itmp->crl)) {
        ++ਗਿਣਤੀ_2;
      }
      ਨਹੀਂਤਾਂ {
        /* set count to 0 to return an error */
        ਗਿਣਤੀ_2 = 0;
        ਰੋਕੋ;
      }
    }
  }

#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਧੱਕੋ
#ਹਦਾਇਤ clang diagnostic ignored "-Wcast-function-type-strict"
#ਜੇ_ਅੰਤ
  sk_X509_INFO_ਕੱਢੋ_free(inf, X509_INFO_free);
#ਜੇ defined(__clang__) && __clang_major__ >= 16
#ਹਦਾਇਤ clang diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  ਵਾਪਸ (ਗਿਣਤੀ_2 > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_CRYPTO
ਸਥਿਰ CURLcode ossl_win_ਲੱਦੋ_store(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ Curl_cfilter *cf,
                                    ਅਚਲ ਅੱਖਰ *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  ਜੇ(hStore) {
    PCCERT_CONTEXT pContext = ਨਿਰਾ;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = ਨਿਰਾ;
    DWORD enhkey_usage_ਮਾਪ = 0;
    VERBOSE(size_t ਕੁੱਲ = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    ਨਤੀਜਾ = CURLE_OK;
    ਲਈ(;;) {
      X509 *x509;
      FILETIME now;
      BYTE ਕੁੰਜੀ_usage[2];
      DWORD req_ਮਾਪ;
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *encoded_cert;
      pContext = Cert_Enum_Certificates_ਵਿੱਚ_Store(hStore, pContext);
      ਜੇ(!pContext)
        ਰੋਕੋ;

      VERBOSE(++ਕੁੱਲ);

#ਜੇ defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        ਅੱਖਰ cert_name[256];
        ਜੇ(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               ਨਿਰਾ, cert_name, ਆਕਾਰ(cert_name)))
          infof(ਅੰਕੜੇ, "SSL: unknown cert name");
        ਨਹੀਂਤਾਂ
          infof(ਅੰਕੜੇ, "SSL: Checking cert \"%s\"", cert_name);
      }
#ਜੇ_ਅੰਤ
      encoded_cert = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)pContext->pbCertEncoded;
      ਜੇ(!encoded_cert)
        ਜਾਰੀ;

      Get_System_ਸਮਾਂ_As_File_ਸਮਾਂ(&now);
      ਜੇ(Compare_File_ਸਮਾਂ(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         Compare_File_ਸਮਾਂ(&now, &pContext->pCertInfo->NotAfter) > 0)
        ਜਾਰੀ;

      /* If key usage exists check for signing attribute */
      ਜੇ(Cert_Get_Intended_ਕੁੰਜੀ_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 ਕੁੰਜੀ_usage, ਆਕਾਰ(ਕੁੰਜੀ_usage))) {
        ਜੇ(!(ਕੁੰਜੀ_usage[0] & CERT_ਕੁੰਜੀ_CERT_SIGN_ਕੁੰਜੀ_USAGE))
          ਜਾਰੀ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(Get_ਆਖਰੀ_Error())
        ਜਾਰੀ;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      ਜੇ(Cert_Get_Enhanced_ਕੁੰਜੀ_Usage(pContext, 0, ਨਿਰਾ, &req_ਮਾਪ) && req_ਮਾਪ) {
        ਜੇ(req_ਮਾਪ > enhkey_usage_ਮਾਪ) {
          ਖਾਲੀ *tmp = curlx_realloc(enhkey_usage, req_ਮਾਪ);

          ਜੇ(!tmp) {
            failf(ਅੰਕੜੇ, "SSL: Out of memory allocating for OID list");
            ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
            ਰੋਕੋ;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_ਮਾਪ = req_ਮਾਪ;
        }

        ਜੇ(Cert_Get_Enhanced_ਕੁੰਜੀ_Usage(pContext, 0, enhkey_usage, &req_ਮਾਪ)) {
          ਜੇ(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            ਜੇ((HRESULT)Get_ਆਖਰੀ_Error() != CRYPT_E_NOT_ਮਿਲਿਆ)
              ਜਾਰੀ;
          }
          ਨਹੀਂਤਾਂ {
            DWORD i;
            bool ਮਿਲਿਆ = FALSE;

            ਲਈ(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              ਜੇ(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                ਮਿਲਿਆ = TRUE;
                ਰੋਕੋ;
              }
            }

            ਜੇ(!ਮਿਲਿਆ)
              ਜਾਰੀ;
          }
        }
        ਨਹੀਂਤਾਂ
          ਜਾਰੀ;
      }
      ਨਹੀਂਤਾਂ
        ਜਾਰੀ;

      x509 = d2i_X509(ਨਿਰਾ, &encoded_cert, (ਲੰਮਾ)pContext->cbCertEncoded);
      ਜੇ(!x509)
        ਜਾਰੀ;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      ਜੇ(X509_STORE_ਜੋੜੋ_cert(store, x509) == 1) {
        VERBOSE(++imported);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
        infof(ਅੰਕੜੇ, "SSL: Imported cert");
#ਜੇ_ਅੰਤ
        *padded = TRUE;
      }
      X509_free(x509);
    }

    curlx_free(enhkey_usage);
    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    CURL_TRC_CF(ਅੰਕੜੇ, cf,
                "ossl_win_load_store() found: %zu imported: %zu certs in %s.",
                ਕੁੱਲ, imported, win_store);

    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ossl_windows_ਲੱਦੋ_anchors(ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *padded = FALSE;
  ਲਈ(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    ਨਤੀਜਾ = ossl_win_ਲੱਦੋ_store(ਅੰਕੜੇ, cf, win_stores[i], store, &store_added);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(store_added) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* USE_WIN32_CRYPTO */

ਸਥਿਰ CURLcode ossl_ਲੱਦੋ_trust_anchors(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ ossl_ctx *octx,
                                        X509_STORE *store)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? ਨਿਰਾ : conn_config->CAfile);
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ = TRUE;
  ਜੇ(conn_config->native_ca_store) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_CRYPTO
    bool added = FALSE;
    ਨਤੀਜਾ = ossl_windows_ਲੱਦੋ_anchors(cf, ਅੰਕੜੇ, store, &added);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(added) {
      infof(ਅੰਕੜੇ, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
    }
#ਜਾਂਜੇ defined(USE_APPLE_SECTRUST)
    infof(ਅੰਕੜੇ, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#ਜੇ_ਅੰਤ
  }

  ਜੇ(conn_config->ca_info_blob) {
    ਨਤੀਜਾ = ਲੱਦੋ_cacert_from_ਸਮ੍ਰਿਤੀ(store, conn_config->ca_info_blob);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "error adding trust anchors from certificate blob: %d",
            (ਪੂਰਨ)ਨਤੀਜਾ);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    infof(ਅੰਕੜੇ, "  CA Blob from configuration");
    octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
  }

  ਜੇ(ssl_cafile || ssl_capath) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    ਜੇ(ssl_cafile) {
      ਜੇ(!X509_STORE_ਲੱਦੋ_file(store, ssl_cafile)) {
        ਜੇ(octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(ਅੰਕੜੇ, "error adding trust anchors from file: %s", ssl_cafile);
          ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
        }
        ਨਹੀਂਤਾਂ
          infof(ਅੰਕੜੇ, "error setting certificate file, continuing anyway");
      }
      infof(ਅੰਕੜੇ, "  CAfile: %s", ssl_cafile);
      octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
    }
    ਜੇ(ssl_capath) {
      ਜੇ(!X509_STORE_ਲੱਦੋ_ਰਸਤਾ(store, ssl_capath)) {
        ਜੇ(octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(ਅੰਕੜੇ, "error adding trust anchors from path: %s", ssl_capath);
          ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
        }
        ਨਹੀਂਤਾਂ
          infof(ਅੰਕੜੇ, "error setting certificate path, continuing anyway");
      }
      infof(ਅੰਕੜੇ, "  CApath: %s", ssl_capath);
      octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
    }
#ਨਹੀਂਤਾਂ
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    ਜੇ(!X509_STORE_ਲੱਦੋ_locations(store, ssl_cafile, ssl_capath)) {
      ਜੇ(octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(ਅੰਕੜੇ, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
      }
      ਨਹੀਂਤਾਂ {
        infof(ਅੰਕੜੇ, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    ਜੇ(ssl_cafile)
      infof(ਅੰਕੜੇ, "  CAfile: %s", ssl_cafile);
    ਜੇ(ssl_capath)
      infof(ਅੰਕੜੇ, "  CApath: %s", ssl_capath);
    octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
#ਜੇ_ਅੰਤ
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_CA_FALLBACK
  ਜੇ(octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_ਸੈੱਟ_default_paths(store);
    infof(ਅੰਕੜੇ, "  OpenSSL default paths (fallback)");
    octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
  }
#ਜੇ_ਅੰਤ
  ਜੇ(octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ && !have_native_check)
    infof(ਅੰਕੜੇ, "  no trust anchors configured");

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ossl_populate_x509_store(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         ਬਣਤਰ ossl_ctx *octx,
                                         X509_STORE *store)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  X509_LOOKUP *lookup = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_crlfile = conn_config->CRLfile;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ x509flags = 0;

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "configuring OpenSSL's x509 trust store");
  ਜੇ(!store)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(!conn_config->verifypeer) {
    infof(ਅੰਕੜੇ, "SSL Trust: peer verification disabled");
    ਵਾਪਸ CURLE_OK;
  }

  infof(ਅੰਕੜੇ, "SSL Trust Anchors:");
  ਨਤੀਜਾ = ossl_ਲੱਦੋ_trust_anchors(cf, ਅੰਕੜੇ, octx, store);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* Does not make sense to load a CRL file without peer verification */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
  ਜੇ(ssl_crlfile && conn_config->native_ca_store) {
    failf(ਅੰਕੜੇ, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  }
#ਜੇ_ਅੰਤ
  ਜੇ(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_ਜੋੜੋ_lookup(store, X509_LOOKUP_file());
    ਜੇ(!lookup ||
       (!X509_ਲੱਦੋ_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(ਅੰਕੜੇ, "error loading CRL file: %s", ssl_crlfile);
      ਵਾਪਸ CURLE_SSL_CRL_BADFILE;
    }
    x509flags = X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL;
    infof(ਅੰਕੜੇ, " CRLfile: %s", ssl_crlfile);
  }

  /* Try building a chain using issuers in the trusted store first to avoid
     problems with server-sent legacy intermediates. Newer versions of
     OpenSSL do alternate chain checking by default but we do not know how to
     determine that in a reliable manner.
     https://web.archive.org/web/20190422050538/rt.openssl.org/Ticket/Display.html?id=3621
   */
  x509flags |= X509_V_FLAG_TRUSTED_ਪਹਿਲਾ;

  ਜੇ(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (ਖਾਲੀ)X509_STORE_ਸੈੱਟ_flags(store, x509flags);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* key to use at `multi->proto_hash` */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPROTO_OSSL_X509_ਕੁੰਜੀ  "tls:ossl:x509:share"

ਬਣਤਰ ossl_x509_share {
  ਅੱਖਰ *CAfile;         /* CAfile path used to generate X509 store */
  ਅੱਖਰ *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  ਬਣਤਰ curltime ਸਮਾਂ; /* when the cached store was created */
  ਦੋਆਧਾਰੀ_ਅੰਕ(store_is_ਖਾਲੀ_ਥਾਵਾਂ);  /* no certs/paths/blobs are in the store */
  ਦੋਆਧਾਰੀ_ਅੰਕ(no_partialchain); /* keep partial chain state */
};

ਸਥਿਰ ਖਾਲੀ oss_x509_share_free(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t ਕੁੰਜੀ_len, ਖਾਲੀ *p)
{
  ਬਣਤਰ ossl_x509_share *share = p;
  DEBUGASSERT(ਕੁੰਜੀ_len == CURL_CSTRLEN(MPROTO_OSSL_X509_ਕੁੰਜੀ));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_ਕੁੰਜੀ, ਕੁੰਜੀ, ਕੁੰਜੀ_len));
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)ਕੁੰਜੀ_len;
  ਜੇ(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

ਸਥਿਰ bool ossl_cached_x509_store_expired(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਅਚਲ ਬਣਤਰ ossl_x509_share *mb)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl_ca_cache_timeout < 0)
    ਵਾਪਸ FALSE;
  ਨਹੀਂਤਾਂ {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &mb->ਸਮਾਂ);
    timediff_t timeout_ms = ਅੰਕੜੇ->ਸੈੱਟ.ssl_ca_cache_timeout * (timediff_t)1000;

    ਵਾਪਸ elapsed_ms >= timeout_ms;
  }
}

ਸਥਿਰ bool ossl_cached_x509_store_different(ਬਣਤਰ Curl_cfilter *cf,
                                             ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                             ਅਚਲ ਬਣਤਰ ossl_x509_share *mb)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਜੇ(mb->no_partialchain != conn_config->no_partialchain)
    ਵਾਪਸ TRUE;
  ਜੇ(!mb->CAfile || !conn_config->CAfile) {
    ਜੇ(mb->CAfile != conn_config->CAfile)
      ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(strcmp(mb->CAfile, conn_config->CAfile))
    ਵਾਪਸ TRUE;

  ਜੇ(!mb->CApath || !conn_config->CApath) {
    ਜੇ(mb->CApath != conn_config->CApath)
      ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(strcmp(mb->CApath, conn_config->CApath))
    ਵਾਪਸ TRUE;

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ X509_STORE *ossl_get_cached_x509_store(ਬਣਤਰ Curl_cfilter *cf,
                                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                              bool *pempty)
{
  ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;
  ਬਣਤਰ ossl_x509_share *share;
  X509_STORE *store = ਨਿਰਾ;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_ਕੁੰਜੀ,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_ਕੁੰਜੀ)) : ਨਿਰਾ;
  ਜੇ(share && share->store &&
     !ossl_cached_x509_store_expired(ਅੰਕੜੇ, share) &&
     !ossl_cached_x509_store_different(cf, ਅੰਕੜੇ, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_ਖਾਲੀ_ਥਾਵਾਂ;
  }

  ਵਾਪਸ store;
}

ਸਥਿਰ ਖਾਲੀ ossl_ਸੈੱਟ_cached_x509_store(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       X509_STORE *store,
                                       bool is_ਖਾਲੀ_ਥਾਵਾਂ)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ Curl_multi *multi = ਅੰਕੜੇ->multi;
  ਬਣਤਰ ossl_x509_share *share;

  DEBUGASSERT(multi);
  ਜੇ(!multi)
    ਵਾਪਸ;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_ਕੁੰਜੀ,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_ਕੁੰਜੀ));

  ਜੇ(!share) {
    share = curlx_calloc(1, ਆਕਾਰ(*share));
    ਜੇ(!share)
      ਵਾਪਸ;
    ਜੇ(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_ਕੁੰਜੀ,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_ਕੁੰਜੀ),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      ਵਾਪਸ;
    }
  }

  ਜੇ(X509_STORE_ਉੱਪਰ_ref(store)) {
    ਅੱਖਰ *CAfile = ਨਿਰਾ;
    ਅੱਖਰ *CApath = ਨਿਰਾ;

    ਜੇ(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      ਜੇ(!CAfile) {
        X509_STORE_free(store);
        ਵਾਪਸ;
      }
    }

    ਜੇ(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      ਜੇ(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        ਵਾਪਸ;
      }
    }

    ਜੇ(share->store) {
      X509_STORE_free(share->store);
      curlx_free(share->CAfile);
      curlx_free(share->CApath);
    }

    share->ਸਮਾਂ = *Curl_pgrs_now(ਅੰਕੜੇ);
    share->store = store;
    share->store_is_ਖਾਲੀ_ਥਾਵਾਂ = is_ਖਾਲੀ_ਥਾਵਾਂ;
    share->CAfile = CAfile;
    share->CApath = CApath;
    share->no_partialchain = conn_config->no_partialchain;
  }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_HAS_PROVIDERS
/* CApath certificates loaded under a provider libctx can outlive that libctx
   when the store is shared with the multi handle. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_PROVIDER(octx) octx->p.libctx
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_PROVIDER(x) 0
#ਜੇ_ਅੰਤ

CURLcode Curl_ssl_ਤਿਆਰੀ_x509_store(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ossl_ctx *octx)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  X509_STORE *cached_store;
  bool cache_criteria_met, is_ਖਾਲੀ_ਥਾਵਾਂ;

  /* Consider the X509 store cacheable if it comes from CAfile/CApath,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (ਅੰਕੜੇ->ਸੈੱਟ.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !USING_PROVIDER(octx);

  ERR_ਸੈੱਟ_mark();

  cached_store = ossl_get_cached_x509_store(cf, ਅੰਕੜੇ, &is_ਖਾਲੀ_ਥਾਵਾਂ);
  ਜੇ(cached_store && cache_criteria_met && X509_STORE_ਉੱਪਰ_ref(cached_store)) {
    SSL_CTX_ਸੈੱਟ_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ = is_ਖਾਲੀ_ਥਾਵਾਂ;
  }
  ਨਹੀਂਤਾਂ {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    ਨਤੀਜਾ = ossl_populate_x509_store(cf, ਅੰਕੜੇ, octx, store);
    ਜੇ(ਨਤੀਜਾ == CURLE_OK && cache_criteria_met) {
      ossl_ਸੈੱਟ_cached_x509_store(cf, ਅੰਕੜੇ, store, (bool)octx->store_is_ਖਾਲੀ_ਥਾਵਾਂ);
    }
  }

  ERR_ਕੱਢੋ_ਮੰਜ਼ਿਲ_mark();

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool ossl_apply_session(
  ਬਣਤਰ ossl_ctx *octx,
  ਬਣਤਰ Curl_cfilter *cf,
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  ਬਣਤਰ Curl_ssl_session *scs)
{
  ਬਣਤਰ ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *der_sessionid = scs->sdata;
  size_t der_sessionid_ਮਾਪ = scs->sdata_len;
  SSL_SESSION *ssl_session = ਨਿਰਾ;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(ਨਿਰਾ, &der_sessionid,
                                (ਲੰਮਾ)der_sessionid_ਮਾਪ);
  ਜੇ(ssl_session) {
    ਜੇ(!SSL_ਸੈੱਟ_session(octx->ssl, ssl_session)) {
      VERBOSE(ਅੱਖਰ error_ਬਫਰ[256]);
      infof(ਅੰਕੜੇ, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_ਬਫਰ,
                          ਆਕਾਰ(error_ਬਫਰ)));
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(conn_cfg->verifypeer &&
         (SSL_get_verify_ਨਤੀਜਾ(octx->ssl) != X509_V_OK)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#ਜੇ_ਅੰਤ
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_ਸੈੱਟ_session(octx->ssl, ਨਿਰਾ);
        infof(ਅੰਕੜੇ, "SSL session not peer verified, not reusing");
      }
      ਨਹੀਂਤਾਂ {
        infof(ਅੰਕੜੇ, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#ਜੇ_ਅੰਤ
        infof(ਅੰਕੜੇ, "SSL verify result: %lx",
              (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)SSL_get_verify_ਨਤੀਜਾ(octx->ssl));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL_EARLYDATA
        ਜੇ(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_ਅੰਕੜੇ(ssl_session) &&
           !cf->conn->ਬਿੱਟ.connect_only &&
           (SSL_ਸੰਸਕਰਣ(octx->ssl) == TLS1_3_ਸੰਸਕਰਣ)) {
          bool do_early_ਅੰਕੜੇ = FALSE;
          ਜੇ(sess_reuse_cb)
            (ਖਾਲੀ)sess_reuse_cb(cf, ਅੰਕੜੇ, alpns, scs, &do_early_ਅੰਕੜੇ);
          ਜੇ(do_early_ਅੰਕੜੇ) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_ਮੰਜ਼ਿਲ(alpns, scs->alpn);
          }
        }
#ਨਹੀਂਤਾਂ
        (ਖਾਲੀ)alpns;
        (ਖਾਲੀ)conn_cfg;
        (ਖਾਲੀ)sess_reuse_cb;
#ਜੇ_ਅੰਤ
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  ਨਹੀਂਤਾਂ {
    infof(ਅੰਕੜੇ, "SSL session not accepted by OpenSSL, continuing without");
  }
  ਵਾਪਸ (bool)octx->reused_session;
}

ਸਥਿਰ CURLcode ossl_init_session_and_alpns(
  ਬਣਤਰ ossl_ctx *octx,
  ਬਣਤਰ Curl_cfilter *cf,
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ ssl_peer *peer,
  ਅਚਲ ਬਣਤਰ alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  ਬਣਤਰ ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ alpn_spec alpns;
  CURLcode ਨਤੀਜਾ;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  ਜੇ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ਬਣਤਰ Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(ਅੰਕੜੇ, cf->cft, FIRSTSOCKET);
    ਜੇ(scs) {
      ਜੇ(ossl_apply_session(octx, cf, ਅੰਕੜੇ, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "applied SSL session from control connection");
    }
  }

  ਜੇ(!octx->reused_session &&
     Curl_ssl_scache_use(cf, ਅੰਕੜੇ) && !conn_cfg->verifystatus) {
    ਬਣਤਰ Curl_ssl_session *scs = ਨਿਰਾ;

    ਨਤੀਜਾ = Curl_ssl_scache_take(cf, ਅੰਕੜੇ, peer->scache_ਕੁੰਜੀ, &scs);
    ਜੇ(!ਨਤੀਜਾ && scs && scs->sdata && scs->sdata_len) {
      (ਖਾਲੀ)ossl_apply_session(octx, cf, ਅੰਕੜੇ, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, ਅੰਕੜੇ, peer->scache_ਕੁੰਜੀ, scs);
  }

  ਜੇ(alpns.ਗਿਣਤੀ_2) {
    ਬਣਤਰ alpn_proto_buf proto;
    memset(&proto, 0, ਆਕਾਰ(proto));
    ਨਤੀਜਾ = Curl_alpn_ਮੰਜ਼ਿਲ_proto_buf(&proto, &alpns);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Error determining ALPN");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
    ਜੇ(SSL_ਸੈੱਟ_alpn_protos(octx->ssl, proto.ਅੰਕੜੇ, proto.len)) {
      failf(ਅੰਕੜੇ, "Error setting ALPN");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_SET1_ECH_CONFIG_LIST
ਸਥਿਰ CURLcode ossl_init_ech(ਬਣਤਰ ossl_ctx *octx,
                              ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ssl_peer *peer)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਪੂਰਨ trying_ech_now = 0;

  ਜੇ(!Curl_ssl_ech_enabled(cf))
    ਵਾਪਸ CURLE_OK;

  ਜੇ(conn_config->ech == CURLECH_GREASE) {
    infof(ਅੰਕੜੇ, "ECH: will GREASE ClientHello");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
    SSL_ਸੈੱਟ_enable_ech_grease(octx->ssl, 1);
#ਨਹੀਂਤਾਂ
    SSL_ਸੈੱਟ_options(octx->ssl, SSL_OP_ECH_GREASE);
#ਜੇ_ਅੰਤ
  }
  ਨਹੀਂਤਾਂ ਜੇ(conn_config->ech && conn_config->ech_config) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode ਨਤੀਜਾ;

    ਜੇ(!conn_config->ech_config[0]) {
      infof(ਅੰਕੜੇ, "ECH: ECHConfig from command line empty");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * ਲੜੀ_ਲੰਬਾਈ(conn_config->ech_config);
    ਨਤੀਜਾ = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    ਜੇ(ਨਤੀਜਾ || !ech_config) {
      infof(ਅੰਕੜੇ, "ECH: cannot base64 decode ECHConfig from command line");
      ਜੇ(conn_config->ech == CURLECH_HARD)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਜੇ(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(ਅੰਕੜੇ, "ECH: SSL_ECH_set1_ech_config_list failed");
      ਜੇ(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#ਨਹੀਂਤਾਂ
    size_t ech_config_len = 0;
    ਜੇ(!conn_config->ech_config[0]) {
      infof(ਅੰਕੜੇ, "ECH: ECHConfig from command line empty");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = ਲੜੀ_ਲੰਬਾਈ(conn_config->ech_config);
    ਜੇ(SSL_set1_ech_config_list(octx->ssl,
                                (ਅਚਲ uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(ਅੰਕੜੇ, "ECH: SSL_ECH_set1_ech_config_list failed");
      ਜੇ(conn_config->ech == CURLECH_HARD)
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
    ਨਹੀਂਤਾਂ
      trying_ech_now = 1;
#ਜੇ_ਅੰਤ /* HAVE_BORINGSSL_LIKE */
    infof(ਅੰਕੜੇ, "ECH: ECHConfig from command line");
  }
  ਨਹੀਂਤਾਂ {
    ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(ਅੰਕੜੇ, cf->sockindex, peer->origin);

    ਜੇ(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(ਅੰਕੜੇ, "ECH: ECHConfig from HTTPS RR");
      ਜੇ(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(ਅੰਕੜੇ, "ECH: SSL_set1_ech_config_list failed");
        ਜੇ(conn_config->ech == CURLECH_HARD)
          ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
      }
      ਨਹੀਂਤਾਂ {
        trying_ech_now = 1;
        infof(ਅੰਕੜੇ, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, "ECH: requested but no ECHConfig available");
      ਜੇ(conn_config->ech == CURLECH_HARD)
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
  (ਖਾਲੀ)peer;
  ਜੇ(trying_ech_now && conn_config->ech_ਜਨਤਕ) {
    infof(ਅੰਕੜੇ, "ECH: setting public_name not supported with BoringSSL");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }
#ਨਹੀਂਤਾਂ
  ਜੇ(trying_ech_now && conn_config->ech_ਜਨਤਕ) {
    ਪੂਰਨ ret;
    infof(ਅੰਕੜੇ, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_ਜਨਤਕ);
    ret = SSL_ech_set1_server_ਨਾਂ(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_ਜਨਤਕ,
                                    0 /* do send outer */);
    ਜੇ(ret != 1) {
      infof(ਅੰਕੜੇ, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }
#ਜੇ_ਅੰਤ /* HAVE_BORINGSSL_LIKE */
  ਜੇ(trying_ech_now &&
     SSL_ਸੈੱਟ_min_proto_ਸੰਸਕਰਣ(octx->ssl, TLS1_3_ਸੰਸਕਰਣ) != 1) {
    infof(ਅੰਕੜੇ, "ECH: cannot force TLSv1.3 [ERROR]");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

ਸਥਿਰ CURLcode ossl_init_ssl(ਬਣਤਰ ossl_ctx *octx,
                              ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ ssl_peer *peer,
                              ਅਚਲ ਬਣਤਰ alpn_spec *alpns_requested,
                              ਖਾਲੀ *ssl_user_ਅੰਕੜੇ,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  ਜੇ(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  ਜੇ(!octx->ssl) {
    failf(ਅੰਕੜੇ, "SSL: could not create a context (handle)");
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  SSL_ਸੈੱਟ_app_ਅੰਕੜੇ(octx->ssl, ssl_user_ਅੰਕੜੇ);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_NO_OCSP
  ਜੇ(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_ਸੈੱਟ_tlsext_ਸਥਿਤੀ_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#ਜੇ_ਅੰਤ

  SSL_ਸੈੱਟ_connect_state(octx->ssl);

  ਜੇ(peer->sni) {
    ਜੇ(!SSL_ਸੈੱਟ_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(ਅੰਕੜੇ, "Failed set SNI");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode ਨਤੀਜਾ = ossl_init_ech(octx, cf, ਅੰਕੜੇ, peer);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਅੰਤ /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  ਵਾਪਸ ossl_init_session_and_alpns(octx, cf, ਅੰਕੜੇ, peer,
                                     alpns_requested, sess_reuse_cb);
}

ਸਥਿਰ CURLcode ossl_init_ਵਿਧੀ(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ssl_peer *peer,
                                 ਅਚਲ SSL_ਵਿਧੀ **pmethod,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *pssl_ਸੰਸਕਰਣ_min)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = ਨਿਰਾ;
  *pssl_ਸੰਸਕਰਣ_min = conn_config->ਸੰਸਕਰਣ_2;
  DEBUGASSERT(conn_config->ਸੰਸਕਰਣ_2 != CURL_SSLVERSION_DEFAULT);
  ਚੋਣ(peer->transport) {
  ਹਾਲਤ TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    ਚੋਣ(*pssl_ਸੰਸਕਰਣ_min) {
    ਹਾਲਤ CURL_SSLVERSION_TLSv1:
    ਹਾਲਤ CURL_SSLVERSION_TLSv1_0:
    ਹਾਲਤ CURL_SSLVERSION_TLSv1_1:
    ਹਾਲਤ CURL_SSLVERSION_TLSv1_2:
    ਹਾਲਤ CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_ਵਿਧੀ();
      ਰੋਕੋ;
    ਹਾਲਤ CURL_SSLVERSION_SSLv2:
      failf(ਅੰਕੜੇ, "No SSLv2 support");
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
    ਹਾਲਤ CURL_SSLVERSION_SSLv3:
      failf(ਅੰਕੜੇ, "No SSLv3 support");
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
    ਮੂਲ:
      failf(ਅੰਕੜੇ, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
    ਰੋਕੋ;
  ਹਾਲਤ TRNSPRT_QUIC:
    *pssl_ਸੰਸਕਰਣ_min = CURL_SSLVERSION_TLSv1_3;
    ਜੇ(conn_config->ਸੰਸਕਰਣ_max &&
       (conn_config->ਸੰਸਕਰਣ_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->ਸੰਸਕਰਣ_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(ਅੰਕੜੇ, "QUIC needs at least TLS version 1.3");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_ਵਿਧੀ();
    ਰੋਕੋ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "unsupported transport %d in SSL init", peer->transport);
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਵਾਪਸ *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(ਬਣਤਰ ossl_ctx *octx,
                            ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ssl_peer *peer,
                            ਅਚਲ ਬਣਤਰ alpn_spec *alpns_requested,
                            Curl_ossl_ctx_ਤਿਆਰੀ_cb *cb_ਤਿਆਰੀ,
                            ਖਾਲੀ *cb_user_ਅੰਕੜੇ,
                            Curl_ossl_new_session_cb *cb_new_session,
                            ਖਾਲੀ *ssl_user_ਅੰਕੜੇ,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ciphers;
  ਅਚਲ SSL_ਵਿਧੀ *req_ਵਿਧੀ = ਨਿਰਾ;
  ctx_option_t ctx_options = 0;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ਅੰਕੜੇ);
  ਅੱਖਰ * ਅਚਲ ssl_cert = conn_config->clientcert;
  ਅਚਲ ਬਣਤਰ curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_cert_type = conn_config->cert_type;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ssl_ਸੰਸਕਰਣ_min = CURL_SSLVERSION_DEFAULT;
  ਅੱਖਰ error_ਬਫਰ[256];

  /* Make funny stuff to get random input */
  ਨਤੀਜਾ = ossl_ਬੀਜ(ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ) {
    ਜੇ(CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_ENGINE))
      ਨਤੀਜਾ = ossl_ਸੈੱਟ_engine(ਅੰਕੜੇ, octx,
                               CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_ENGINE));
    ਜੇ(!ਨਤੀਜਾ && ਅੰਕੜੇ->ਸੈੱਟ.engine_default)
      ਨਤੀਜਾ = ossl_ਸੈੱਟ_engine_default(ਅੰਕੜੇ, octx);
  }

  ਜੇ(!ਨਤੀਜਾ) {
    ssl_config->certverifyresult = !X509_V_OK;
    ਨਤੀਜਾ = ossl_init_ਵਿਧੀ(cf, ਅੰਕੜੇ, peer, &req_ਵਿਧੀ, &ssl_ਸੰਸਕਰਣ_min);
  }
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;
  DEBUGASSERT(req_ਵਿਧੀ);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_ਵਿਧੀ):
#ਜੇ_ਅੰਤ
    SSL_CTX_new(req_ਵਿਧੀ);

  ਜੇ(!octx->ssl_ctx) {
    failf(ਅੰਕੜੇ, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ)));
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(cb_ਤਿਆਰੀ) {
    ਨਤੀਜਾ = cb_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, cb_user_ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.fdebug && ਅੰਕੜੇ->ਸੈੱਟ.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_ਸੈੱਟ_ਸੁਨੇਹਾ_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_ਸੈੱਟ_ਸੁਨੇਹਾ_callback_arg(octx->ssl_ctx, cf);
  }

  /* OpenSSL contains code to work around lots of bugs and flaws in various
     SSL-implementations. SSL_CTX_set_options() is used to enable those
     workarounds. The man page for this option states that SSL_OP_ALL enables
     all the workarounds and that "It is usually safe to use SSL_OP_ALL to
     enable the bug workaround options if compatibility with somewhat broken
     implementations is desired."

     The "-no_ticket" option was introduced in OpenSSL 0.9.8j. it is a flag to
     disable "rfc4507bis session ticket support". rfc4507bis was later turned
     into the proper RFC5077: https://datatracker.ietf.org/doc/html/rfc5077

     The enabled extension concerns the session management. I wonder how often
     libcurl stops a connection and then resumes a TLS session. Also, sending
     the session data is some overhead. I suggest that you use your proposed
     patch (which explicitly disables TICKET).

     If someone writes an application with libcurl and OpenSSL who wants to
     enable the feature, one can do this in the SSL callback.

     SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG option enabling allowed proper
     interoperability with web server Netscape Enterprise Server 2.0.1 which
     was released back in 1996.

     Due to CVE-2010-4180, option SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG has
     become ineffective as of OpenSSL 0.9.8q and 1.0.0c. In order to mitigate
     CVE-2010-4180 when using previous OpenSSL versions we no longer enable
     this option regardless of OpenSSL version and SSL_OP_ALL definition.

     OpenSSL added a workaround for an SSL 3.0/TLS 1.0 CBC vulnerability:
     https://web.archive.org/web/20240114184648/openssl.org/~bodo/tls-cbc.txt.
     In 0.9.6e they added a bit to SSL_OP_ALL that _disables_ that workaround
     despite the fact that SSL_OP_ALL is documented to do "rather harmless"
     workarounds. In order to keep the secure workaround, the
     SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS bit must not be set. */

  ctx_options = SSL_OP_ALL | SSL_OP_NO_TICKET | SSL_OP_NO_COMPRESSION;

  /* mitigate CVE-2010-4180 */
  ctx_options &= ~(ctx_option_t)SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG;

  /* unless the user explicitly asks to allow the protocol vulnerability we
     use the workaround */
  ਜੇ(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_ਪਾਓ_ਖਾਲੀ_ਥਾਵਾਂ_FRAGMENTS;

  DEBUGASSERT(ssl_ਸੰਸਕਰਣ_min != CURL_SSLVERSION_DEFAULT);
  ਚੋਣ(ssl_ਸੰਸਕਰਣ_min) {
  ਹਾਲਤ CURL_SSLVERSION_SSLv2:
  ਹਾਲਤ CURL_SSLVERSION_SSLv3:
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  ਹਾਲਤ CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    ਨਤੀਜਾ = ossl_ਸੈੱਟ_ssl_ਸੰਸਕਰਣ_min_max(cf, octx->ssl_ctx, ssl_ਸੰਸਕਰਣ_min);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਰੋਕੋ;

  ਮੂਲ:
    failf(ਅੰਕੜੇ, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  SSL_CTX_ਸੈੱਟ_options(octx->ssl_ctx, ctx_options);
  SSL_CTX_ਸੈੱਟ_read_ahead(octx->ssl_ctx, 1);

  /* Max TLS1.2 record size 0x4000 + 0x800.
     OpenSSL supports processing "jumbo TLS record" (8 TLS records) in one go
     for some algorithms, so match that here.
     Experimentation shows that a slightly larger buffer is needed
     to avoid short reads.

     However using a large buffer (8 packets) actually decreases performance.
     4 packets is better.
   */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_CTX_ਸੈੱਟ_DEFAULT_READ_ਬਫਰ_LEN
  SSL_CTX_ਸੈੱਟ_default_read_ਬਫਰ_len(octx->ssl_ctx, 0x401e * 4);
#ਜੇ_ਅੰਤ

  /* We do retry writes sometimes from another buffer address */
  SSL_CTX_ਸੈੱਟ_ਬਹੁਲਕ(octx->ssl_ctx, SSL_ਬਹੁਲਕ_ACCEPT_MOVING_WRITE_ਬਫਰ);

  ciphers = conn_config->cipher_list;
  ਜੇ(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = ਨਿਰਾ;
  ਜੇ(ciphers && (ssl_ਸੰਸਕਰਣ_min < CURL_SSLVERSION_TLSv1_3)) {
    ਜੇ(!SSL_CTX_ਸੈੱਟ_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(ਅੰਕੜੇ, "failed setting cipher list: %s", ciphers);
      ਵਾਪਸ CURLE_SSL_CIPHER;
    }
    infof(ਅੰਕੜੇ, "Cipher selection: %s", ciphers);
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_CTX_ਸੈੱਟ_CIPHERSUITES
  {
    ਅਚਲ ਅੱਖਰ *ciphers13 = conn_config->cipher_list13;
    ਜੇ(ciphers13 &&
       (!conn_config->ਸੰਸਕਰਣ_max ||
        (conn_config->ਸੰਸਕਰਣ_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->ਸੰਸਕਰਣ_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      ਜੇ(!SSL_CTX_ਸੈੱਟ_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(ਅੰਕੜੇ, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        ਵਾਪਸ CURLE_SSL_CIPHER;
      }
      infof(ਅੰਕੜੇ, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#ਜੇ_ਅੰਤ

  ਜੇ(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    ਨਤੀਜਾ = client_cert(ਅੰਕੜੇ, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->ਕੁੰਜੀ, conn_config->ਕੁੰਜੀ_blob,
                         conn_config->ਕੁੰਜੀ_type,
                         conn_config->ਕੁੰਜੀ_passwd);
    ਜੇ(ਨਤੀਜਾ)
      /* failf() is already done in client_cert() */
      ਵਾਪਸ ਨਤੀਜਾ;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_CTX_ਸੈੱਟ_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_ਸੈੱਟ_post_handshake_auth(octx->ssl_ctx, 1);
#ਜੇ_ਅੰਤ

  {
    ਅਚਲ ਅੱਖਰ *curves = conn_config->curves;
    ਜੇ(curves) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSSL_ਵਕਰ_CAST(x) (x)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSSL_ਵਕਰ_CAST(x) (ਅੱਖਰ *)CURL_UNCONST(x)
#ਜੇ_ਅੰਤ
      ਜੇ(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_ਵਕਰ_CAST(curves))) {
        failf(ਅੰਕੜੇ, "failed setting curves list: '%s'", curves);
        ਵਾਪਸ CURLE_SSL_CIPHER;
      }
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_CTX_SET1_SIGALGS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSSL_SIGALG_CAST(x) OSSL_ਵਕਰ_CAST(x)
  {
    ਅਚਲ ਅੱਖਰ *signature_algorithms = conn_config->signature_algorithms;
    ਜੇ(signature_algorithms) {
      ਜੇ(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(ਅੰਕੜੇ, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        ਵਾਪਸ CURLE_SSL_CIPHER;
      }
    }
  }
#ਜੇ_ਅੰਤ

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_ਸੈੱਟ_verify(octx->ssl_ctx, SSL_VERIFY_NONE, ਨਿਰਾ);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#ਜੇ !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  ਜੇ(Curl_tls_keylog_enabled()) {
    SSL_CTX_ਸੈੱਟ_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#ਜੇ_ਅੰਤ

  ਜੇ(cb_new_session) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_ਸੈੱਟ_session_cache_ਬਹੁਲਕ(octx->ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_ਅੰਦਰੂਨੀ);
    SSL_CTX_sess_ਸੈੱਟ_new_cb(octx->ssl_ctx, cb_new_session);
  }

  /* give application a chance to interfere with SSL set up. */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctx) {
    ਬਣਤਰ Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    ਜੇ(!octx->x509_store_ਤਿਆਰੀ) {
      ਨਤੀਜਾ = Curl_ssl_ਤਿਆਰੀ_x509_store(cf, ਅੰਕੜੇ, octx);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
      octx->x509_store_ਤਿਆਰੀ = TRUE;
    }
    CURL_CBAPI_ਸ਼ੁਰੂ(&guard, ਅੰਕੜੇ, easy_fsslctx);
    ਨਤੀਜਾ = (*ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctx)(ਅੰਕੜੇ, octx->ssl_ctx,
                                      ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctxp);
    CURL_CBAPI_ਸਿਰਾ(&guard);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "error signaled by SSL ctx callback");
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਵਾਪਸ ossl_init_ssl(octx, cf, ਅੰਕੜੇ, peer, alpns_requested,
                       ssl_user_ਅੰਕੜੇ, sess_reuse_cb);
}

ਸਥਿਰ CURLcode ossl_on_session_reuse(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ alpn_spec *alpns,
                                      ਬਣਤਰ Curl_ssl_session *scs,
                                      bool *do_early_ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  ਵਾਪਸ Curl_on_session_reuse(cf, ਅੰਕੜੇ, alpns, scs, do_early_ਅੰਕੜੇ,
                               connssl->earlydata_max);
}

ਖਾਲੀ Curl_ossl_report_handshake(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ossl_ctx *octx)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    ਪੂਰਨ psigtype_nid = NID_undef;
    ਅਚਲ ਅੱਖਰ *negotiated_ਸਮੂਹ_name = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#ਜੇ OPENSSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x30200000L
    negotiated_ਸਮੂਹ_name = SSL_get0_ਸਮੂਹ_name(octx->ssl);
#ਨਹੀਂਤਾਂ
    negotiated_ਸਮੂਹ_name =
      OBJ_nid2sn(SSL_get_negotiated_ਸਮੂਹ(octx->ssl) & 0x0000FFFF);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

    /* Informational message */
    infof(ਅੰਕੜੇ, "SSL connection using %s / %s / %s / %s",
          SSL_get_ਸੰਸਕਰਣ(octx->ssl),
          SSL_get_cipher(octx->ssl),
          negotiated_ਸਮੂਹ_name ? negotiated_ਸਮੂਹ_name : "[blank]",
          OBJ_nid2sn(psigtype_nid));
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)octx;
#ਜੇ_ਅੰਤ /* CURLVERBOSE */
}

ਸਥਿਰ CURLcode ossl_connect_step1(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  ਨਤੀਜਾ = Curl_ossl_ctx_init(octx, cf, ਅੰਕੜੇ, &connssl->peer,
                              connssl->alpn, ਨਿਰਾ, ਨਿਰਾ,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  octx->bio_ਵਿਧੀ = ossl_bio_cf_ਵਿਧੀ_create();
  ਜੇ(!octx->bio_ਵਿਧੀ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  bio = BIO_new(octx->bio_ਵਿਧੀ);
  ਜੇ(!bio)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  BIO_ਸੈੱਟ_ਅੰਕੜੇ(bio, cf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_SET0_WBIO
  /* with OpenSSL v1.1.1 we get an alternative to SSL_set_bio() that works
   * without backward compat quirks. Every call takes one reference, so we
   * up it and pass. SSL* then owns and frees it.
   * We check on the function in configure, since LibreSSL and friends
   * each have their own versions to add support for this. */
  BIO_ਉੱਪਰ_ref(bio);
  SSL_set0_rbio(octx->ssl, bio);
  SSL_set0_wbio(octx->ssl, bio);
#ਨਹੀਂਤਾਂ
  SSL_ਸੈੱਟ_bio(octx->ssl, bio, bio);
#ਜੇ_ਅੰਤ

  ਜੇ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ਬਣਤਰ alpn_proto_buf proto;
    memset(&proto, 0, ਆਕਾਰ(proto));
    Curl_alpn_ਮੰਜ਼ਿਲ_proto_str(&proto, connssl->alpn);
    infof(ਅੰਕੜੇ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ਅੰਕੜੇ);
  }

  connssl->connecting_state = ssl_connect_2;
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
ਸਥਿਰ ਪੂਰਨ ossl_trace_ech_retry_configs(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, SSL *ssl,
                                        ਪੂਰਨ reason)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t rcl = 0;
  ਪੂਰਨ rv = 1;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
  ਅੱਖਰ *inner = ਨਿਰਾ;
  uint8_t *rcs = ਨਿਰਾ;
  ਅੱਖਰ *outer = ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
  ਅਚਲ ਅੱਖਰ *inner = ਨਿਰਾ;
  ਅਚਲ uint8_t *rcs = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *outer = ਨਿਰਾ;
  size_t ਬਾਹਰਲਾ_name_len = 0;
  ਪੂਰਨ servername_type = 0;
#ਜੇ_ਅੰਤ
  NOVERBOSE((ਖਾਲੀ)reason);

  /* nothing to trace if not doing ECH */
  ਜੇ(!Curl_ssl_ech_enabled(cf))
    ਵਾਪਸ rv;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#ਨਹੀਂਤਾਂ
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (ਪੂਰਨ)rcl;
#ਜੇ_ਅੰਤ

  ਜੇ(rv && rcs) {
    ਅੱਖਰ *b64str = ਨਿਰਾ;
    size_t blen = 0;

    ਨਤੀਜਾ = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    ਜੇ(!ਨਤੀਜਾ && b64str) {
      infof(ਅੰਕੜੇ, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_ਸਥਿਤੀ(ssl, &inner, &outer);
      infof(ਅੰਕੜੇ, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#ਨਹੀਂਤਾਂ
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &ਬਾਹਰਲਾ_name_len);
      infof(ਅੰਕੜੇ, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#ਜੇ_ਅੰਤ
    }
  }
  ਨਹੀਂਤਾਂ
    infof(ਅੰਕੜੇ, "ECH: no retry_configs (rv = %d)", rv);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ rv;
}

#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode ossl_connect_step2(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  ਬਣਤਰ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ਅੰਕੜੇ);
  ਪੂਰਨ err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  ਜੇ(!octx->x509_store_ਤਿਆਰੀ) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode ਨਤੀਜਾ = Curl_ssl_ਤਿਆਰੀ_x509_store(cf, ਅੰਕੜੇ, octx);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    octx->x509_store_ਤਿਆਰੀ = TRUE;
  }

#ਜੇ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  ਜੇ(Curl_tls_keylog_enabled() && !octx->keylog_ਪੂਰਾ)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_ਪੂਰਾ);
#ਜੇ_ਅੰਤ

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  ਜੇ(err != 1) {
    ਪੂਰਨ detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    ਜੇ(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ਵਾਪਸ CURLE_AGAIN;
    }
    ਜੇ(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ਵਾਪਸ CURLE_AGAIN;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_ERROR_WANT_ASYNC
    ਜੇ(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ਵਾਪਸ CURLE_AGAIN;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_ERROR_WANT_RETRY_VERIFY
    ਜੇ(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(ਅੰਕੜੇ, TRUE);
      ਵਾਪਸ CURLE_AGAIN;
    }
#ਜੇ_ਅੰਤ
    ਨਹੀਂਤਾਂ {
      /* untreated error */
      sslerr_t errdetail;
      ਅੱਖਰ error_ਬਫਰ[256] = "";
      CURLcode ਨਤੀਜਾ;
      ਲੰਮਾ lerr;
      ਪੂਰਨ lib;
      ਪੂਰਨ reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      ਜੇ((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#ਜੇ_ਅੰਤ
         )) {
        ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_ਨਤੀਜਾ(octx->ssl);
        ਜੇ(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(ਅੰਕੜੇ, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        ਨਹੀਂਤਾਂ
          failf(ਅੰਕੜੇ, "%s", "SSL certificate verification failed");
      }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      ਨਹੀਂਤਾਂ ਜੇ((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        ਨਤੀਜਾ = CURLE_SSL_CLIENTCERT;
        failf(ਅੰਕੜੇ, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ)));
      }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_SET1_ECH_CONFIG_LIST
      ਨਹੀਂਤਾਂ ਜੇ((lib == ERR_LIB_SSL) &&
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#ਨਹੀਂਤਾਂ
              (reason == SSL_R_ECH_REJECTED)) {
#ਜੇ_ਅੰਤ /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, ਅੰਕੜੇ, octx->ssl, reason);

        ਨਤੀਜਾ = CURLE_ECH_REQUIRED;
        failf(ਅੰਕੜੇ, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ)));
      }
#ਜੇ_ਅੰਤ
      ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
        failf(ਅੰਕੜੇ, "TLS connect error: %s",
              ossl_strerror(errdetail, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      ਜੇ(ਨਤੀਜਾ == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        ਅੱਖਰ extramsg[80] = "";
        ਪੂਰਨ sockerr = SOCKERRNO;

        ਜੇ(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, ਆਕਾਰ(extramsg));
        failf(ਅੰਕੜੇ, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_ਮੰਜ਼ਿਲ_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->ਪੋਰਟ);
      }

      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਨਹੀਂਤਾਂ {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(ਅੰਕੜੇ, octx);

#ਜੇ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    ਜੇ(Curl_ssl_ech_enabled(cf)) {
      ਬਣਤਰ ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      ਅੱਖਰ *inner = ਨਿਰਾ, *outer = ਨਿਰਾ;
      ਪੂਰਨ rv;
      VERBOSE(ਅਚਲ ਅੱਖਰ *ਸਥਿਤੀ);

      rv = SSL_ech_get1_ਸਥਿਤੀ(octx->ssl, &inner, &outer);
      ਚੋਣ(rv) {
      ਹਾਲਤ SSL_ECH_ਸਥਿਤੀ_SUCCESS:
        VERBOSE(ਸਥਿਤੀ = "succeeded");
        ਰੋਕੋ;
      ਹਾਲਤ SSL_ECH_ਸਥਿਤੀ_GREASE_ECH:
        VERBOSE(ਸਥਿਤੀ = "sent GREASE, got retry-configs");
        ਰੋਕੋ;
      ਹਾਲਤ SSL_ECH_ਸਥਿਤੀ_GREASE:
        VERBOSE(ਸਥਿਤੀ = "sent GREASE");
        ਰੋਕੋ;
      ਹਾਲਤ SSL_ECH_ਸਥਿਤੀ_NOT_TRIED:
        VERBOSE(ਸਥਿਤੀ = "not attempted");
        ਰੋਕੋ;
      ਹਾਲਤ SSL_ECH_ਸਥਿਤੀ_NOT_CONFIGURED:
        VERBOSE(ਸਥਿਤੀ = "not configured");
        ਰੋਕੋ;
      ਹਾਲਤ SSL_ECH_ਸਥਿਤੀ_BACKEND:
        VERBOSE(ਸਥਿਤੀ = "backend (unexpected)");
        ਰੋਕੋ;
      ਹਾਲਤ SSL_ECH_ਸਥਿਤੀ_FAILED:
        VERBOSE(ਸਥਿਤੀ = "failed");
        ਰੋਕੋ;
      ਹਾਲਤ SSL_ECH_ਸਥਿਤੀ_BAD_CALL:
        VERBOSE(ਸਥਿਤੀ = "bad call (unexpected)");
        ਰੋਕੋ;
      ਹਾਲਤ SSL_ECH_ਸਥਿਤੀ_BAD_NAME: {
        ਜੇ(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(ਸਥਿਤੀ = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_ਸਥਿਤੀ_SUCCESS;
        }
        ਨਹੀਂਤਾਂ {
          VERBOSE(ਸਥਿਤੀ = "bad name (unexpected)");
        }
        ਰੋਕੋ;
      }
      ਮੂਲ:
        VERBOSE(ਸਥਿਤੀ = "unexpected status");
        infof(ਅੰਕੜੇ, "ECH: unexpected status %d", rv);
      }
      infof(ਅੰਕੜੇ, "ECH: result '%s' (inner=%s, outer=%s)",
            (ਸਥਿਤੀ ? ਸਥਿਤੀ : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      ਜੇ(rv == SSL_ECH_ਸਥਿਤੀ_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, ਅੰਕੜੇ, octx->ssl, 0);
      }
      ਜੇ(rv != SSL_ECH_ਸਥਿਤੀ_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(ਅੰਕੜੇ, "ECH: ech-hard failed");
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
      }
    }
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, "ECH: result: status is not attempted");
    }
#ਜੇ_ਅੰਤ /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    ਜੇ(connssl->alpn) {
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *neg_protocol;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      ਵਾਪਸ Curl_alpn_ਸੈੱਟ_negotiated(cf, ਅੰਕੜੇ, connssl, neg_protocol, len);
    }

    ਵਾਪਸ CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
ਸਥਿਰ CURLcode ossl_pkp_pin_peer_pubkey(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, X509 *cert,
                                         ਅਚਲ ਅੱਖਰ *pinnedpubkey)
{
  /* Scratch */
  ਪੂਰਨ len1 = 0, len2 = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buff1 = ਨਿਰਾ, *ਆਰਜ਼ੀ = ਨਿਰਾ;

  /* Result is returned to caller */
  CURLcode ਨਤੀਜਾ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ਜੇ(!pinnedpubkey)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(!cert)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਕਰੋ {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), ਨਿਰਾ);
    ਜੇ(len1 < 1)
      ਰੋਕੋ; /* failed */

    buff1 = ਆਰਜ਼ੀ = curlx_malloc(len1);
    ਜੇ(!buff1)
      ਰੋਕੋ; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &ਆਰਜ਼ੀ);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    ਜੇ((len1 != len2) || !ਆਰਜ਼ੀ || ((ਆਰਜ਼ੀ - buff1) != len1))
      ਰੋਕੋ; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    ਨਤੀਜਾ = Curl_pin_peer_pubkey(ਅੰਕੜੇ, pinnedpubkey, buff1, len1);
  } ਜਦੋਂ(0);

  ਜੇ(buff1)
    curlx_free(buff1);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਜੇ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ) && LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x3060000fL)
ਸਥਿਰ ਖਾਲੀ infof_certstack(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ SSL *ssl)
{
  ਢੇਰ_ਦਾ(X509) *certstack;
  ਲੰਮਾ verify_ਨਤੀਜਾ;
  ਪੂਰਨ ਅੰਕ_cert_ਪੱਧਰ;
  ਪੂਰਨ cert_level;

  ਜੇ(!Curl_trc_is_verbose(ਅੰਕੜੇ))
    ਵਾਪਸ;

  verify_ਨਤੀਜਾ = SSL_get_verify_ਨਤੀਜਾ(ssl);
  ਜੇ(verify_ਨਤੀਜਾ != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  ਨਹੀਂਤਾਂ
    certstack = SSL_get0_verified_chain(ssl);
  ਜੇ(!certstack)
    ਵਾਪਸ;
  ਅੰਕ_cert_ਪੱਧਰ = sk_X509_ਅੰਕ(certstack);

  ਲਈ(cert_level = 0; cert_level < ਅੰਕ_cert_ਪੱਧਰ; cert_level++) {
    ਅੱਖਰ cert_algorithm[80] = "";
    ਅੱਖਰ ਸਮੂਹ_name_final[80] = "";
    ਅਚਲ X509_ALGOR *palg_cert = ਨਿਰਾ;
    ਅਚਲ ASN1_OBJECT *paobj_cert = ਨਿਰਾ;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    ਪੂਰਨ ਕੁੰਜੀ_ਬਿੱਟ;
    ਪੂਰਨ ਕੁੰਜੀ_sec_ਬਿੱਟ;
    ਪੂਰਨ get_ਸਮੂਹ_name;
    ਅਚਲ ਅੱਖਰ *type_name;

    current_cert = sk_X509_ਮੁੱਲ(certstack, cert_level);
    ਜੇ(!current_cert)
      ਜਾਰੀ;

    current_pkey = X509_get0_pubkey(current_cert);
    ਜੇ(!current_pkey)
      ਜਾਰੀ;

    X509_get0_signature(ਨਿਰਾ, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, ਨਿਰਾ, ਨਿਰਾ, palg_cert);
    OBJ_obj2txt(cert_algorithm, ਆਕਾਰ(cert_algorithm), paobj_cert, 0);

    ਕੁੰਜੀ_ਬਿੱਟ = EVP_PKEY_ਬਿੱਟ(current_pkey);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EVP_PKEY_get_security_ਬਿੱਟ EVP_PKEY_security_ਬਿੱਟ
#ਜੇ_ਅੰਤ
    ਕੁੰਜੀ_sec_ਬਿੱਟ = EVP_PKEY_get_security_ਬਿੱਟ(current_pkey);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL3
    {
      ਅੱਖਰ ਸਮੂਹ_name[80] = "";
      get_ਸਮੂਹ_name = EVP_PKEY_get_ਸਮੂਹ_name(current_pkey, ਸਮੂਹ_name,
                                               ਆਕਾਰ(ਸਮੂਹ_name), ਨਿਰਾ);
      curl_msnprintf(ਸਮੂਹ_name_final, ਆਕਾਰ(ਸਮੂਹ_name_final), "/%s",
                     ਸਮੂਹ_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#ਨਹੀਂਤਾਂ
    get_ਸਮੂਹ_name = 0;
    type_name = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    infof(ਅੰਕੜੇ, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_ਸਮੂਹ_name == 0 ? "" : ਸਮੂਹ_name_final,
          ਕੁੰਜੀ_ਬਿੱਟ, ਕੁੰਜੀ_sec_ਬਿੱਟ, cert_algorithm);
  }
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ infof_certstack(ਅੰਕੜੇ, ssl)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

ਸਥਿਰ CURLcode ossl_check_issuer(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  X509 *server_cert)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = ਨਿਰਾ;
  BIO *fp = ਨਿਰਾ;
  ਅੱਖਰ err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  ਜੇ(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->ਅੰਕੜੇ,
                         (ਪੂਰਨ)conn_config->issuercert_blob->len);
    ਜੇ(!fp) {
      failf(ਅੰਕੜੇ, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, ਆਕਾਰ(err_buf)));
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    ਜੇ(!fp) {
      failf(ਅੰਕੜੇ, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, ਆਕਾਰ(err_buf)));
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      ਜੇ(verify_enabled)
        failf(ਅੰਕੜੇ, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      ਨਤੀਜਾ = CURLE_SSL_ISSUER_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਜੇ(fp) {
    issuer = PEM_read_bio_X509(fp, ਨਿਰਾ, ZERO_NULL, ਨਿਰਾ);
    ਜੇ(!issuer) {
      ਜੇ(verify_enabled)
        failf(ਅੰਕੜੇ, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      ਨਤੀਜਾ = CURLE_SSL_ISSUER_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      ਜੇ(verify_enabled)
        failf(ਅੰਕੜੇ, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      ਨਤੀਜਾ = CURLE_SSL_ISSUER_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    infof(ਅੰਕੜੇ, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

ਬਾਹਰਲਾ:
  ਜੇ(fp)
    BIO_free(fp);
  ਜੇ(issuer)
    X509_free(issuer);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *pinned(ਬਣਤਰ Curl_cfilter *cf,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  (ਖਾਲੀ)cf;
  ਵਾਪਸ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#ਜੇ_ਅੰਤ
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY);
}

ਸਥਿਰ CURLcode ossl_check_pinned_ਕੁੰਜੀ(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      X509 *server_cert)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਅੱਖਰ *ptr = pinned(cf, ਅੰਕੜੇ);
  ਜੇ(ptr) {
    ਨਤੀਜਾ = ossl_pkp_pin_peer_pubkey(ਅੰਕੜੇ, server_cert, ptr);
    ਜੇ(ਨਤੀਜਾ)
      failf(ਅੰਕੜੇ, "SSL: public key does not match pinned public key");
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_CERT_NAME_ਲੰਮਾਈ 2048
ਸਥਿਰ CURLcode ossl_infof_cert(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                X509 *server_cert)
{
  BIO *mem = ਨਿਰਾ;
  ਬਣਤਰ dynbuf dname;
  ਅੱਖਰ err_buf[256] = "";
  ਅੱਖਰ *buf;
  ਲੰਮਾ len;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!Curl_trc_is_verbose(ਅੰਕੜੇ))
    ਵਾਪਸ CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_ਲੰਮਾਈ);
  mem = BIO_new(BIO_s_mem());
  ਜੇ(!mem) {
    failf(ਅੰਕੜੇ, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, ਆਕਾਰ(err_buf)));
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  infof(ਅੰਕੜੇ, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  ਨਤੀਜਾ = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(ਅੰਕੜੇ, "  subject: %s", ਨਤੀਜਾ ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_ਸਮਾਂ_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_ਅੰਕੜੇ(mem, (ਅੱਖਰ **)&buf);
  infof(ਅੰਕੜੇ, "  start date: %.*s", (ਪੂਰਨ)len, buf);
  (ਖਾਲੀ)BIO_reset(mem);

  ASN1_ਸਮਾਂ_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_ਅੰਕੜੇ(mem, (ਅੱਖਰ **)&buf);
  infof(ਅੰਕੜੇ, "  expire date: %.*s", (ਪੂਰਨ)len, buf);
  (ਖਾਲੀ)BIO_reset(mem);

  ਨਤੀਜਾ = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  ਜੇ(ਨਤੀਜਾ) /* should be only fatal stuff like OOM */
    ਜਾਓ ਬਾਹਰਲਾ;
  infof(ਅੰਕੜੇ, "  issuer: %s", curlx_dyn_ptr(&dname));

ਬਾਹਰਲਾ:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* CURLVERBOSE */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
ਬਣਤਰ ossl_certs_ctx {
  ਢੇਰ_ਦਾ(X509) *sk;
  size_t ਅੰਕ_certs;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਆਖਰੀ_der;
};

ਸਥਿਰ CURLcode ossl_chain_get_der(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਖਾਲੀ *user_ਅੰਕੜੇ,
                                   size_t i,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **pder,
                                   size_t *pder_len)
{
  ਬਣਤਰ ossl_certs_ctx *chain = user_ਅੰਕੜੇ;
  X509 *cert;
  ਪੂਰਨ der_len;

  OPENSSL_free(chain->ਆਖਰੀ_der);
  chain->ਆਖਰੀ_der = ਨਿਰਾ;

  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  *pder_len = 0;
  *pder = ਨਿਰਾ;

  ਜੇ(i >= chain->ਅੰਕ_certs)
    ਵਾਪਸ CURLE_TOO_LARGE;
  cert = sk_X509_ਮੁੱਲ(chain->sk, (ਪੂਰਨ)i);
  ਜੇ(!cert)
    ਵਾਪਸ CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  ਜੇ(der_len < 0)
    ਵਾਪਸ CURLE_FAILED_INIT;
  chain->ਆਖਰੀ_der = *pder;
  *pder_len = (size_t)der_len;
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ossl_apple_verify(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ossl_ctx *octx,
                                  ਬਣਤਰ ssl_peer *peer)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ ossl_certs_ctx chain;
  CURLcode ਨਤੀਜਾ;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, ਆਕਾਰ(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.ਅੰਕ_certs = chain.sk ? sk_X509_ਅੰਕ(chain.sk) : 0;

  ਜੇ(!chain.ਅੰਕ_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    ਜੇ(!octx->reused_session) {
      failf(ਅੰਕੜੇ, "SSL: could not get peer certificate chain");
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ਨਹੀਂਤਾਂ {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      ਵਾਪਸ CURLE_OK;
    }
  }
  ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_BORINGSSL_LIKE
    ਅਚਲ uint8_t *ocsp_ਅੰਕੜੇ = ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ocsp_ਅੰਕੜੇ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਲੰਮਾ ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    ਜੇ(!octx->reused_session)
      ocsp_len = (ਲੰਮਾ)SSL_get_tlsext_ਸਥਿਤੀ_ocsp_resp(octx->ssl, &ocsp_ਅੰਕੜੇ);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    ਜੇ(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_ਅੰਕੜੇ = ਨਿਰਾ;
    }

    ਨਤੀਜਾ = Curl_vtls_apple_verify(cf, ਅੰਕੜੇ, peer, chain.ਅੰਕ_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_ਅੰਕੜੇ, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.ਆਖਰੀ_der);
    chain.ਆਖਰੀ_der = ਨਿਰਾ;
  }
  octx->sectrust_verified = !ਨਤੀਜਾ;
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ossl_ctx *octx,
                                   ਬਣਤਰ ssl_peer *peer)
{
  ਬਣਤਰ connectdata *conn = cf->conn;
  ਬਣਤਰ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ਅੰਕੜੇ);
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਲੰਮਾ ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    ਨਤੀਜਾ = ossl_certchain(ਅੰਕੜੇ, octx->ssl);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  ਜੇ(!server_cert) {
    /* no verification at all, this maybe acceptable */
    ਜੇ(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, ਅੰਕੜੇ))
      ਜਾਓ ਬਾਹਰਲਾ;

    failf(ਅੰਕੜੇ, "SSL: could not get peer certificate");
    ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਨਤੀਜਾ = ossl_infof_cert(cf, ਅੰਕੜੇ, server_cert);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  infof_certstack(ਅੰਕੜੇ, octx->ssl);
#ਜੇ_ਅੰਤ

  ਜੇ(conn_config->verifyhost) {
    ਨਤੀਜਾ = ossl_verifyhost(ਅੰਕੜੇ, conn, peer, server_cert);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_ਨਤੀਜਾ(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(ਅੰਕੜੇ, "OpenSSL verify result: %lx", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  ਜੇ(verified)
    infof(ਅੰਕੜੇ, "SSL certificate verified via OpenSSL.");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
  ਜੇ(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    ਨਤੀਜਾ = ossl_apple_verify(cf, ਅੰਕੜੇ, octx, peer);
    ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_PEER_FAILED_VERIFICATION))
      ਜਾਓ ਬਾਹਰਲਾ; /* unexpected error */
    ਜੇ(octx->sectrust_verified) {
      infof(ਅੰਕੜੇ, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#ਜੇ_ਅੰਤ

  ਜੇ(!verified) {
    /* no trust established, report the OpenSSL status */
    ਜੇ(conn_config->verifypeer) {
      failf(ਅੰਕੜੇ, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    infof(ਅੰਕੜੇ, " SSL certificate verification failed, continuing anyway!");
  }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_NO_OCSP
  ਜੇ(conn_config->verifystatus &&
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, ਅੰਕੜੇ)) &&
#ਜੇ_ਅੰਤ
     !octx->reused_session) {
    ਨਤੀਜਾ = verifystatus(cf, ਅੰਕੜੇ, octx);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
#ਜੇ_ਅੰਤ

  ਨਤੀਜਾ = ossl_check_issuer(cf, ਅੰਕੜੇ, server_cert);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = ossl_check_pinned_ਕੁੰਜੀ(cf, ਅੰਕੜੇ, server_cert);

ਬਾਹਰਲਾ:
  X509_free(server_cert);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ossl_connect_step3(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  ਨਤੀਜਾ = Curl_ossl_check_peer_cert(cf, ਅੰਕੜੇ, octx, &connssl->peer);
  ਜੇ(ਨਤੀਜਾ)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, ਅੰਕੜੇ, connssl->peer.scache_ਕੁੰਜੀ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL_EARLYDATA
ਸਥਿਰ CURLcode ossl_send_earlydata(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf;
  size_t blen, nwritten;
  ਪੂਰਨ rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_ਨਤੀਜਾ = CURLE_OK;
  ਜਦੋਂ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_ਅੰਕੜੇ(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    ਜੇ(rc <= 0) {
      ਲੰਮਾ sslerror;
      ਅੱਖਰ error_ਬਫਰ[256];
      ਪੂਰਨ err = SSL_get_error(octx->ssl, rc);

      ਚੋਣ(err) {
      ਹਾਲਤ SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        ਨਤੀਜਾ = CURLE_AGAIN;
        ਜਾਓ ਬਾਹਰਲਾ;
      ਹਾਲਤ SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ਨਤੀਜਾ = CURLE_AGAIN;
        ਜਾਓ ਬਾਹਰਲਾ;
      ਹਾਲਤ SSL_ERROR_SYSCALL: {
        ਪੂਰਨ sockerr = SOCKERRNO;

        ਜੇ(octx->io_ਨਤੀਜਾ == CURLE_AGAIN) {
          ਨਤੀਜਾ = CURLE_AGAIN;
          ਜਾਓ ਬਾਹਰਲਾ;
        }
        sslerror = ERR_get_error();
        ਜੇ(sslerror)
          ossl_strerror(sslerror, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ));
        ਨਹੀਂਤਾਂ ਜੇ(sockerr)
          curlx_strerror(sockerr, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ));
        ਨਹੀਂਤਾਂ
          curl_msnprintf(error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ), "%s",
                         SSL_ERROR_ਮੰਜ਼ਿਲ_str(err));

        failf(ਅੰਕੜੇ, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_ਬਫਰ, sockerr);
        ਨਤੀਜਾ = CURLE_SEND_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਹਾਲਤ SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(ਅੰਕੜੇ, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ)));
        ਨਤੀਜਾ = CURLE_SEND_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਮੂਲ:
        /* a true error */
        failf(ਅੰਕੜੇ, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_ਮੰਜ਼ਿਲ_str(err), SOCKERRNO);
        ਨਤੀਜਾ = CURLE_SEND_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(ਅੰਕੜੇ, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* HAVE_OPENSSL_EARLYDATA */

ਸਥਿਰ CURLcode ossl_connect(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;

  /* check if the connection has already been established */
  ਜੇ(ssl_connection_complete == connssl->state) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ਜੇ(connssl->connecting_state == ssl_connect_1) {
    ਜੇ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(ਅੰਕੜੇ, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "need HTTPS-RR, delaying connect");
      ਵਾਪਸ CURLE_OK;
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "ossl_connect, step1");
    ਨਤੀਜਾ = ossl_connect_step1(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "ossl_connect, step2");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL_EARLYDATA
    ਜੇ(connssl->earlydata_state == ssl_earlydata_await) {
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(connssl->earlydata_state == ssl_earlydata_sending) {
      ਨਤੀਜਾ = ossl_send_earlydata(cf, ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      connssl->earlydata_state = ssl_earlydata_ਭੇਜਿਆ;
    }
#ਜੇ_ਅੰਤ
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_ਭੇਜਿਆ));

    ਨਤੀਜਾ = ossl_connect_step2(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "ossl_connect, step3");
    ਨਤੀਜਾ = ossl_connect_step3(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    connssl->connecting_state = ssl_connect_ਪੂਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL_EARLYDATA
    ਜੇ(connssl->earlydata_state > ssl_earlydata_none) {
      ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_ਭੇਜਿਆ);
      connssl->earlydata_state =
        (SSL_get_early_ਅੰਕੜੇ_ਸਥਿਤੀ(octx->ssl) == SSL_EARLY_ਅੰਕੜੇ_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#ਜੇ_ਅੰਤ
  }

  ਜੇ(connssl->connecting_state == ssl_connect_ਪੂਰਾ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    *ਪੂਰਾ = FALSE;
    ਵਾਪਸ CURLE_OK;
  }
  *ਪੂਰਾ = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool ossl_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                              ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ (bool)connssl->ਨਿਵੇਸ਼_pending;
}

ਸਥਿਰ CURLcode ossl_send(ਬਣਤਰ Curl_cfilter *cf,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਖਾਲੀ *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  ਪੂਰਨ err;
  ਅੱਖਰ error_ਬਫਰ[256];
  sslerr_t sslerror;
  ਪੂਰਨ memlen;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ : (ਪੂਰਨ)len;
  ਜੇ(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    ਜੇ(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  ਜੇ(nwritten > 0) {
    ਜੇ(nwritten > memlen) {
      failf(ਅੰਕੜੇ, "SSL_write(len=%d) returned %d", memlen, nwritten);
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    *pnwritten = (size_t)nwritten;
  }
  ਨਹੀਂਤਾਂ {
    err = SSL_get_error(octx->ssl, nwritten);

    ਚੋਣ(err) {
    ਹਾਲਤ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਜਾਓ ਬਾਹਰਲਾ;
    ਹਾਲਤ SSL_ERROR_WANT_WRITE:
      ਨਤੀਜਾ = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      ਜਾਓ ਬਾਹਰਲਾ;
    ਹਾਲਤ SSL_ERROR_SYSCALL: {
      ਪੂਰਨ sockerr = SOCKERRNO;

      ਜੇ(octx->io_ਨਤੀਜਾ == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        ਨਤੀਜਾ = CURLE_AGAIN;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      sslerror = ERR_get_error();
      ਜੇ(sslerror)
        ossl_strerror(sslerror, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ));
      ਨਹੀਂਤਾਂ ਜੇ(sockerr)
        curlx_strerror(sockerr, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ));
      ਨਹੀਂਤਾਂ
        curl_msnprintf(error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ), "%s",
                       SSL_ERROR_ਮੰਜ਼ਿਲ_str(err));

      failf(ਅੰਕੜੇ, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_ਬਫਰ, sockerr);
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਹਾਲਤ SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(ਅੰਕੜੇ, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ)));
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਮੂਲ:
      /* a true error */
      failf(ਅੰਕੜੇ, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_ਮੰਜ਼ਿਲ_str(err), SOCKERRNO);
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ossl_recv(ਬਣਤਰ Curl_cfilter *cf,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,   /* transfer */
                          ਅੱਖਰ *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  ਅੱਖਰ error_ਬਫਰ[256];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sslerror;
  ਪੂਰਨ buffsize;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) ? ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ : (ਪੂਰਨ)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  ਜੇ(nread > 0)
    *pnread = (size_t)nread;
  ਨਹੀਂਤਾਂ {
    /* failed SSL_read */
    ਪੂਰਨ err = SSL_get_error(octx->ssl, nread);

    ਚੋਣ(err) {
    ਹਾਲਤ SSL_ERROR_NONE: /* this is not an error */
      ਰੋਕੋ;
    ਹਾਲਤ SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      ਜੇ(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "TLS close_notify");
      ਰੋਕੋ;
    ਹਾਲਤ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਜਾਓ ਬਾਹਰਲਾ;
    ਹਾਲਤ SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਜਾਓ ਬਾਹਰਲਾ;
    ਮੂਲ:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      ਜੇ(octx->io_ਨਤੀਜਾ == CURLE_AGAIN) {
        ਨਤੀਜਾ = CURLE_AGAIN;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      sslerror = ERR_get_error();
      ਜੇ((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
ਕਤਾਰ*/
        ਪੂਰਨ sockerr = SOCKERRNO;
        ਜੇ(sslerror)
          ossl_strerror(sslerror, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ));
        ਨਹੀਂਤਾਂ ਜੇ(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ));
        ਨਹੀਂਤਾਂ
          curl_msnprintf(error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ), "%s",
                         SSL_ERROR_ਮੰਜ਼ਿਲ_str(err));
        failf(ਅੰਕੜੇ, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_ਬਫਰ, sockerr);
        ਨਤੀਜਾ = CURLE_RECV_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(err == SSL_ERROR_SYSCALL) {
        ਜੇ(octx->io_ਨਤੀਜਾ) {
          /* logging handling in underlying filter already */
          ਨਤੀਜਾ = octx->io_ਨਤੀਜਾ;
        }
        ਨਹੀਂਤਾਂ ਜੇ(connssl->peer_closed) {
          failf(ਅੰਕੜੇ, "Connection closed abruptly");
          ਨਤੀਜਾ = CURLE_RECV_ERROR;
        }
        ਨਹੀਂਤਾਂ {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          ਪੂਰਨ sockerr = SOCKERRNO;
          ਜੇ(sockerr)
            curlx_strerror(sockerr, error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ));
          ਨਹੀਂਤਾਂ {
            curl_msnprintf(error_ਬਫਰ, ਆਕਾਰ(error_ਬਫਰ),
                           "Connection closed abruptly");
          }
          failf(ਅੰਕੜੇ, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_ਬਫਰ, sockerr);
          ਨਤੀਜਾ = CURLE_RECV_ERROR;
        }
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
  }

ਬਾਹਰਲਾ:
  ਜੇ((!ਨਤੀਜਾ && !*pnread) || (ਨਤੀਜਾ == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->ਨਿਵੇਸ਼_pending = FALSE;
  }
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread, connssl->ਨਿਵੇਸ਼_pending);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ossl_get_channel_binding(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         int8_t sockindex,
                                         ਬਣਤਰ dynbuf *binding)
{
  X509 *cert;
  ਪੂਰਨ mdnid;
  bool no_digest_acceptable = FALSE;
  ਅਚਲ EVP_MD *algo_type = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *algo_name = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[EVP_MAX_MD_ਮਾਪ];

  ਸਥਿਰ ਅਚਲ ਅੱਖਰ ਅਗੇਤਰ[] = "tls-server-end-point:";
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  ਬਣਤਰ Curl_cfilter *cf = conn->cfilter[sockindex];
  ਬਣਤਰ ossl_ctx *octx = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਕਰੋ {
    ਅਚਲ ਬਣਤਰ Curl_cftype *cft = cf->cft;
    ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;

    ਜੇ(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
      ਰੋਕੋ;
    }

    cf = cf->ਅਗਲਾ;
  } ਜਦੋਂ(cf);

  ਜੇ(!octx) {
    failf(ਅੰਕੜੇ, "Failed to find the SSL filter");
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  ਜੇ(!cert)
    /* No server certificate, do not do channel binding */
    ਵਾਪਸ CURLE_OK;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL3
  {
    ਪੂਰਨ pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    ਜੇ(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(ਅੰਕੜੇ, "certificate signature algorithm not recognized");
      ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਜੇ(mdnid != NID_undef) {
      ਜੇ(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      ਨਹੀਂਤਾਂ
        algo_type = EVP_get_digestbynid(mdnid);
    }
    ਨਹੀਂਤਾਂ ਜੇ(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      ਅੱਖਰ mdname[128] = "";
      ਪੂਰਨ rc = EVP_PKEY_get_default_digest_name(pkey, mdname, ਆਕਾਰ(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      ਜੇ(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      ਨਹੀਂਤਾਂ ਜੇ(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(ਅੰਕੜੇ, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#ਨਹੀਂਤਾਂ /* HAVE_OPENSSL3 */

  ਜੇ(!OBJ_ਲੱਭੋ_sigid_algs(X509_get_signature_nid(cert), &mdnid, ਨਿਰਾ)) {
    failf(ਅੰਕੜੇ,
          "Unable to find digest NID for certificate signature algorithm");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  ਜੇ(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  ਨਹੀਂਤਾਂ {
    algo_type = EVP_get_digestbynid(mdnid);
    ਜੇ(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(ਅੰਕੜੇ, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

#ਜੇ_ਅੰਤ /* HAVE_OPENSSL3, else */

  ਜੇ(!algo_type) {
    ਜੇ(no_digest_acceptable) {
      infof(ਅੰਕੜੇ, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      ਨਤੀਜਾ = CURLE_OK;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(ਅੰਕੜੇ, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(!X509_digest(cert, algo_type, buf, &ਲੰਮਾਈ)) {
    failf(ਅੰਕੜੇ, "X509_digest() failed for channel binding");
    ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  /* Append "tls-server-end-point:" */
  ਨਤੀਜਾ = curlx_dyn_addn(binding, ਅਗੇਤਰ, CURL_CSTRLEN(ਅਗੇਤਰ));
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* Append digest */
  ਨਤੀਜਾ = curlx_dyn_addn(binding, buf, ਲੰਮਾਈ);

ਬਾਹਰਲਾ:
  X509_free(cert);
  ਵਾਪਸ ਨਤੀਜਾ;
}

size_t Curl_ossl_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ
  ਅੱਖਰ *p;
  size_t ਗਿਣਤੀ_2;
  ਅਚਲ ਅੱਖਰ *ver = Open_SSL_ਸੰਸਕਰਣ(OPENSSL_ਸੰਸਕਰਣ);
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  ਜੇ(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  ਗਿਣਤੀ_2 = curl_msnprintf(ਬਫਰ, ਮਾਪ, "%s/%s", OSSL_PACKAGE, ver);
  ਲਈ(p = ਬਫਰ; *p; ++p) {
    ਜੇ(ISBLANK(*p))
      *p = '_';
  }
  ਵਾਪਸ ਗਿਣਤੀ_2;
#ਜਾਂਜੇ defined(OPENSSL_IS_AWSLC)
  ਵਾਪਸ curl_msnprintf(ਬਫਰ, ਮਾਪ, "%s/%s",
                        OSSL_PACKAGE, AWSLC_ਸੰਸਕਰਣ_ਸੰਖਿਆ_STRING);
#ਜਾਂਜੇ defined(OPENSSL_IS_BORINGSSL)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_BORINGSSL_ਸੰਸਕਰਣ
  ਵਾਪਸ curl_msnprintf(ਬਫਰ, ਮਾਪ, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_ਸੰਸਕਰਣ);
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ curl_msnprintf(ਬਫਰ, ਮਾਪ, OSSL_PACKAGE);
#ਜੇ_ਅੰਤ
#ਨਹੀਂਤਾਂ /* OpenSSL 3+ */
  ਵਾਪਸ curl_msnprintf(ਬਫਰ, ਮਾਪ, "%s/%s",
                        OSSL_PACKAGE, Open_SSL_ਸੰਸਕਰਣ(OPENSSL_ਸੰਸਕਰਣ_STRING));
#ਜੇ_ਅੰਤ
}

/* can be called with data == NULL */
ਸਥਿਰ CURLcode ossl_random(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *entropy, size_t ਲੰਮਾਈ)
{
  ਪੂਰਨ rc;
  ਜੇ(ਅੰਕੜੇ) {
    ਜੇ(ossl_ਬੀਜ(ਅੰਕੜੇ)) /* Initiate the seed if not already done */
      ਵਾਪਸ CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(!rand_enough())
      ਵਾਪਸ CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_ਬਾਈਟਾਂ(entropy, (ossl_valsize_t)curlx_uztosi(ਲੰਮਾਈ));
  ਵਾਪਸ rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

ਸਥਿਰ CURLcode ossl_sha256sum(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                               size_t len,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha256sum /* output */,
                               size_t ਅਣਵਰਤਿਆ)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (ਖਾਲੀ)ਅਣਵਰਤਿਆ;

  mdctx = EVP_MD_CTX_new();
  ਜੇ(!mdctx)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਜੇ(!EVP_DigestInit_ex(mdctx, EVP_sha256(), ਨਿਰਾ)) {
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(!EVP_DigestUpdate(mdctx, ਨਿਵੇਸ਼, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, ਨਿਰਾ))
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
ਬਾਹਰਲਾ:
  EVP_MD_CTX_free(mdctx);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool ossl_cert_ਸਥਿਤੀ_request(ਖਾਲੀ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPENSSL_NO_OCSP
  ਵਾਪਸ TRUE;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ *ossl_get_internals(ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  ਬਣਤਰ ossl_ctx *octx = (ਬਣਤਰ ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  ਵਾਪਸ info == CURLINFO_TLS_SESSION ?
    (ਖਾਲੀ *)octx->ssl_ctx : (ਖਾਲੀ *)octx->ssl;
}

ਅਚਲ ਬਣਤਰ Curl_ssl Curl_ssl_openssl = {
  { CURLSSLBACKEND_OPENSSL, "openssl" }, /* info */

  SSLSUPP_CA_ਰਸਤਾ |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_SSL_CTX |
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_CTX_ਸੈੱਟ_CIPHERSUITES
  SSLSUPP_TLS13_CIPHERSUITES |
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_CTX_SET1_SIGALGS
  SSLSUPP_SIGNATURE_ALGORITHMS |
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SSL_SET1_ECH_CONFIG_LIST
  SSLSUPP_ECH |
#ਜੇ_ਅੰਤ
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_ISSUERCERT_BLOB |
  SSLSUPP_SSL_EC_CURVES |
  SSLSUPP_CRLFILE,

  ਆਕਾਰ(ਬਣਤਰ ossl_ctx),

  ossl_init,                /* init */
  ossl_cleanup,             /* cleanup */
  Curl_ossl_ਸੰਸਕਰਣ,        /*ਸੰਸਕਰਣ*/
  ossl_shutdown,            /* shutdown */
  ossl_ਅੰਕੜੇ_pending,        /* data_pending */
  ossl_random,              /* random */
  ossl_cert_ਸਥਿਤੀ_request, /* cert_status_request */
  ossl_connect,             /* connect */
  Curl_ssl_adjust_pollset,  /* adjust_pollset */
  ossl_get_internals,       /* get_internals */
  ossl_close,               /* close_one */
  ossl_close_all,           /* close_all */
  ossl_engines_list,        /* engines_list */
  ossl_sha256sum,           /* sha256sum */
  ossl_recv,                /* recv decrypted data */
  ossl_send,                /* send data to encrypt */
  ossl_get_channel_binding  /* get_channel_binding */
};

#ਜੇ_ਅੰਤ /* USE_OPENSSL */
