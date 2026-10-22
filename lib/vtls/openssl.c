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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_OPENSSL

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "formdata.h" /* for the boundary function */
#ಅನ್ತರ್ಭಾವ "url.h" /* for the SSL config check function */
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "vtls/keylog.h"
#ಅನ್ತರ್ಭಾವ "vtls/hostcheck.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "multiif.h"
#ಅನ್ತರ್ಭಾವ "curlx/strerr.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "vtls/apple.h"
#ifdef USE_ECH
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#endif

#ಅನ್ತರ್ಭಾವ <openssl/rand.h>
#ಅನ್ತರ್ಭಾವ <openssl/x509v3.h>
#ifndef OPENSSL_NO_DSA
#ಅನ್ತರ್ಭಾವ <openssl/dsa.h>
#endif
#ಅನ್ತರ್ಭಾವ <openssl/dh.h>
#ಅನ್ತರ್ಭಾವ <openssl/err.h>
#ಅನ್ತರ್ಭಾವ <openssl/conf.h>
#ಅನ್ತರ್ಭಾವ <openssl/bn.h>
#ಅನ್ತರ್ಭಾವ <openssl/rsa.h>
#ಅನ್ತರ್ಭಾವ <openssl/bio.h>
#ಅನ್ತರ್ಭಾವ <openssl/pkcs12.h>
#ಅನ್ತರ್ಭಾವ <openssl/tls1.h>
#ಅನ್ತರ್ಭಾವ <openssl/evp.h>

#ಯದಿ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#ಅನ್ತರ್ಭಾವ <openssl/ech.h>
#endif

#ifndef OPENSSL_NO_OCSP
#ಅನ್ತರ್ಭಾವ <openssl/ocsp.h>
#endif

#ಯದಿ !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#define USE_OPENSSL_ENGINE
#ಅನ್ತರ್ಭಾವ <openssl/engine.h>
#endif

#ifdef LIBRESSL_VERSION_ಸಂಖ್ಯೆ
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  ಯದಿ LIBRESSL_VERSION_ಸಂಖ್ಯೆ < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#ನೋಚೇದ್ಯದಿ !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#endif

#ಯದಿ defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#ಅನ್ತರ್ಭಾವ <openssl/provider.h>
#ಅನ್ತರ್ಭಾವ <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#define OPENSSL_HAS_PROVIDERS
#endif

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#ಯದಿ !defined(LIBRESSL_VERSION_ಸಂಖ್ಯೆ) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_ಕೀಲಿ_EXPLICIT_PARAMS))
#define HAVE_SSL_CTX_SET_DEFAULT_READ_BUFFER_LEN 1
#endif

#ಯದಿ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#ಅನ್ತರ್ಭಾವ <openssl/ui.h>
#endif

#ifdef HAVE_OPENSSL3
#define HAVE_EVP_PKEY_GET_PARAMS 1
#endif

#ifdef HAVE_EVP_PKEY_GET_PARAMS
#ಅನ್ತರ್ಭಾವ <openssl/core_names.h>
#define DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = ರಿಕ್ತ
#define FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#ಅನ್ಯಥಾ
#define DECLARE_PKEY_PARAM_BIGNUM(name) ಧ್ರುವ BIGNUM *name
#define FREE_PKEY_PARAM_BIGNUM(name)
#endif

/* Whether SSL_CTX_set_ciphersuites is available.
 * BoringSSL: no
 * LibreSSL: supported since 3.4.1 (released 2021-10-14)
 * OpenSSL: supported since 1.1.1 (commit a53b5be6a05)
 */
#ifndef OPENSSL_IS_BORINGSSL
#  define HAVE_SSL_CTX_SET_CIPHERSUITES
#  ifndef OPENSSL_IS_AWSLC
#    define HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
#  endif
#endif

/* Whether SSL_CTX_set1_sigalgs_list is available
 * BoringSSL: supported since 0.20240913.0 (commit 826ce15)
 * LibreSSL: no
 * OpenSSL: supported since 1.0.2 (commit 0b362de5f575)
 */
#ifndef LIBRESSL_VERSION_ಸಂಖ್ಯೆ
#define HAVE_SSL_CTX_SET1_SIGALGS
#endif

#ifdef LIBRESSL_VERSION_ಸಂಖ್ಯೆ
#define OSSL_PACKAGE "LibreSSL"
#ನೋಚೇದ್ಯದಿ defined(OPENSSL_IS_AWSLC)
#define OSSL_PACKAGE "AWS-LC"
#ನೋಚೇದ್ಯದಿ defined(OPENSSL_IS_BORINGSSL)
#define OSSL_PACKAGE "BoringSSL"
#ನೋಚೇದ್ಯದಿ defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#define OSSL_PACKAGE "quictls"
#ಅನ್ಯಥಾ
#define OSSL_PACKAGE "OpenSSL"
#endif

#ifdef HAVE_BORINGSSL_LIKE
ಪ್ರಕಾರನಿರ್ದೇಶ size_t numcert_t;
ಪ್ರಕಾರನಿರ್ದೇಶ uint32_t sslerr_t;
#ಅನ್ಯಥಾ
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ numcert_t;
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned long sslerr_t;
#endif
#define ossl_valsize_t numcert_t

#ಯದಿ OPENSSL_VERSION_ಸಂಖ್ಯೆ >= 0x40100000L
#define ossl_ASN1_STRING_get_length ASN1_STRING_get_length
#ಅನ್ಯಥಾ
ಸ್ಥಿರ size_t ossl_ASN1_STRING_get_length(ಧ್ರುವ ASN1_STRING *str)
{
   ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ length = ASN1_STRING_length(str);
   ಪ್ರತ್ಯಾಗಮ length >= 0 ? (size_t)length : 0;
}
#endif

ಸ್ಥಿರ CURLcode ತಳ್ಳು_certinfo(ಸಂರಚನಾ Curl_easy *data,
                              BIO *mem, ಧ್ರುವ ವರ್ಣ *label, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ)
  WARN_UNUSED_RESULT;

ಸ್ಥಿರ CURLcode ತಳ್ಳು_certinfo(ಸಂರಚನಾ Curl_easy *data,
                              BIO *mem, ಧ್ರುವ ವರ್ಣ *label, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ)
{
  ವರ್ಣ *ptr;
  long len = BIO_get_mem_data(mem, &ptr);
  CURLcode result = Curl_ssl_ತಳ್ಳು_certinfo_len(data, ಅಂಕೆ, label, ptr, len);
  (ಶೂನ್ಯ)BIO_reset(mem);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode pubkey_show(ಸಂರಚನಾ Curl_easy *data,
                            BIO *mem,
                            ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ,
                            ಧ್ರುವ ವರ್ಣ *type,
                            ಧ್ರುವ ವರ್ಣ *name,
                            ಧ್ರುವ BIGNUM *bn) WARN_UNUSED_RESULT;

ಸ್ಥಿರ CURLcode pubkey_show(ಸಂರಚನಾ Curl_easy *data,
                            BIO *mem,
                            ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ,
                            ಧ್ರುವ ವರ್ಣ *type,
                            ಧ್ರುವ ವರ್ಣ *name,
                            ಧ್ರುವ BIGNUM *bn)
{
  ವರ್ಣ namebuf[32];

  curl_msnprintf(namebuf, ಪರಿಮಾಣ(namebuf), "%s(%s)", type, name);

  ಯದಿ(bn)
    BN_print(mem, bn);
  ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_certinfo(data, mem, namebuf, ಅಂಕೆ);
}

#define print_pubkey_BN(_type, _name, ಅಂಕೆ_2)           \
  pubkey_show(data, mem, ಅಂಕೆ_2, #_type, #_name, _name)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ asn1_object_dump(ಧ್ರುವ ASN1_OBJECT *a, ವರ್ಣ *buf, size_t len)
{
  ಪೂರ್ಣಾಙ್ಕ i = i2t_ASN1_OBJECT(buf, (ಪೂರ್ಣಾಙ್ಕ)len, a);
  ಪ್ರತ್ಯಾಗಮ (i >= (ಪೂರ್ಣಾಙ್ಕ)len);  /* buffer too small */
}

ಸ್ಥಿರ CURLcode X509V3_ext(ಸಂರಚನಾ Curl_easy *data,
                           ಪೂರ್ಣಾಙ್ಕ certnum,
                           ಧ್ರುವ ರಾಶಿ_OF(X509_EXTENSION) *extsarg)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  CURLcode result = CURLE_OK;
#ifdef LIBRESSL_VERSION_ಸಂಖ್ಯೆ
  ರಾಶಿ_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#ಅನ್ಯಥಾ
  ಧ್ರುವ ರಾಶಿ_OF(X509_EXTENSION) *exts = extsarg;
#endif

  ಯದಿ((ಪೂರ್ಣಾಙ್ಕ)sk_X509_EXTENSION_ಅಂಕೆ(exts) <= 0)
    /* no extensions, bail out */
    ಪ್ರತ್ಯಾಗಮ result;

  ಪ್ರತಿ(i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)sk_X509_EXTENSION_ಅಂಕೆ(exts); i++) {
    ಧ್ರುವ ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_ಮೌಲ್ಯ(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    ವರ್ಣ namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    ಯದಿ(!bio_out)
      ಪ್ರತ್ಯಾಗಮ result;

    obj = X509_EXTENSION_get_object(ext);

    ಯದಿ(asn1_object_dump(obj, namebuf, ಪರಿಮಾಣ(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    ಯದಿ(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (ಧ್ರುವ ASN1_STRING *)X509_EXTENSION_get_data(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    result = Curl_ssl_ತಳ್ಳು_certinfo_len(data, certnum, namebuf, biomem->data,
                                        biomem->length);
    BIO_free(bio_out);
    ಯದಿ(result)
      ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode get_pkey_rsa(ಸಂರಚನಾ Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, ಪೂರ್ಣಾಙ್ಕ i)
{
  CURLcode result = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  RSA *rsa = EVP_PKEY_get0_RSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(n);
  DECLARE_PKEY_PARAM_BIGNUM(e);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_N, &n);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_E, &e);
#ಅನ್ಯಥಾ
  RSA_get0_ಕೀಲಿ(rsa, &n, &e, ರಿಕ್ತ);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (ಪೂರ್ಣಾಙ್ಕ)(n ? BN_ಅಂಕೆ_bits(n) : 0));
  result = ತಳ್ಳು_certinfo(data, mem, "RSA Public Key", i);
  ಯದಿ(!result) {
    result = print_pubkey_BN(rsa, n, i);
    ಯದಿ(!result)
      result = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef OPENSSL_NO_DSA
ಸ್ಥಿರ CURLcode get_pkey_dsa(ಸಂರಚನಾ Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, ಪೂರ್ಣಾಙ್ಕ i)
{
  CURLcode result = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ಕೀಲಿ);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ಕೀಲಿ, &pub_ಕೀಲಿ);
#ಅನ್ಯಥಾ
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_ಕೀಲಿ(dsa, &pub_ಕೀಲಿ, ರಿಕ್ತ);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dsa, p, i);
  ಯದಿ(!result)
    result = print_pubkey_BN(dsa, q, i);
  ಯದಿ(!result)
    result = print_pubkey_BN(dsa, g, i);
  ಯದಿ(!result)
    result = print_pubkey_BN(dsa, pub_ಕೀಲಿ, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ಕೀಲಿ);
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* !OPENSSL_NO_DSA */

ಸ್ಥಿರ CURLcode get_pkey_dh(ಸಂರಚನಾ Curl_easy *data,
                            EVP_PKEY *pubkey, BIO *mem, ಪೂರ್ಣಾಙ್ಕ i)
{
  CURLcode result;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ಕೀಲಿ);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ಕೀಲಿ, &pub_ಕೀಲಿ);
#ಅನ್ಯಥಾ
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_ಕೀಲಿ(dh, &pub_ಕೀಲಿ, ರಿಕ್ತ);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dh, p, i);
  ಯದಿ(!result)
    result = print_pubkey_BN(dh, q, i);
  ಯದಿ(!result)
    result = print_pubkey_BN(dh, g, i);
  ಯದಿ(!result)
    result = print_pubkey_BN(dh, pub_ಕೀಲಿ, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ಕೀಲಿ);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಧ್ರುವ X509_PUBKEY pubkeytype_t;
#ಅನ್ಯಥಾ
ಪ್ರಕಾರನಿರ್ದೇಶ X509_PUBKEY pubkeytype_t;
#endif

ಸ್ಥಿರ CURLcode ossl_certchain(ಸಂರಚನಾ Curl_easy *data, SSL *ssl)
{
  CURLcode result;
  ರಾಶಿ_OF(X509) *sk;
  ಪೂರ್ಣಾಙ್ಕ i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  ಯದಿ(!sk)
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_ಅಂಕೆ(sk);
  ಯದಿ(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(data, "%d certificates is more than allowed (%d)", (ಪೂರ್ಣಾಙ್ಕ)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  result = Curl_ssl_init_certinfo(data, (ಪೂರ್ಣಾಙ್ಕ)numcerts);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  mem = BIO_new(BIO_s_mem());
  ಯದಿ(!mem)
    result = CURLE_OUT_OF_MEMORY;

  ಪ್ರತಿ(i = 0; !result && (i < (ಪೂರ್ಣಾಙ್ಕ)numcerts); i++) {
    X509 *x = sk_X509_ಮೌಲ್ಯ(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = ರಿಕ್ತ;
    size_t j;
    ಧ್ರುವ ASN1_BIT_STRING *psig = ರಿಕ್ತ;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    result = ತಳ್ಳು_certinfo(data, mem, "Subject", i);
    ಯದಿ(result)
      ಭಙ್ಗ;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    result = ತಳ್ಳು_certinfo(data, mem, "Issuer", i);
    ಯದಿ(result)
      ಭಙ್ಗ;

    BIO_printf(mem, "%lx", (unsigned long)X509_get_version(x));
    result = ತಳ್ಳು_certinfo(data, mem, "Version", i);
    ಯದಿ(result)
      ಭಙ್ಗ;

    {
      ASN1_INTEGER *ಅಂಕೆ = X509_get_serial_ಸಂಖ್ಯೆ(x);
      ಧ್ರುವ unsigned ವರ್ಣ *numdata = ASN1_STRING_get0_data(ಅಂಕೆ);
      ಧ್ರುವ size_t numlen = ossl_ASN1_STRING_get_length(ಅಂಕೆ);
      ಯದಿ(ASN1_STRING_type(ಅಂಕೆ) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      ಪ್ರತಿ(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    result = ತಳ್ಳು_certinfo(data, mem, "Serial Number", i);
    ಯದಿ(result)
      ಭಙ್ಗ;

    {
      ಧ್ರುವ X509_ALGOR *sigalg = ರಿಕ್ತ;
      pubkeytype_t *xpubkey = ರಿಕ್ತ;
      ASN1_OBJECT *pubkeyoid = ರಿಕ್ತ;

      X509_get0_signature(&psig, &sigalg, x);
      ಯದಿ(sigalg) {
        ಧ್ರುವ ASN1_OBJECT *sigalgoid = ರಿಕ್ತ;
        X509_ALGOR_get0(&sigalgoid, ರಿಕ್ತ, ರಿಕ್ತ, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        result = ತಳ್ಳು_certinfo(data, mem, "Signature Algorithm", i);
        ಯದಿ(result)
          ಭಙ್ಗ;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      ಯದಿ(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, xpubkey);
        ಯದಿ(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          result = ತಳ್ಳು_certinfo(data, mem, "Public Key Algorithm", i);
          ಯದಿ(result)
            ಭಙ್ಗ;
        }
      }

      result = X509V3_ext(data, i, X509_get0_extensions(x));
      ಯದಿ(result)
        ಭಙ್ಗ;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    result = ತಳ್ಳು_certinfo(data, mem, "Start date", i);
    ಯದಿ(result)
      ಭಙ್ಗ;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    result = ತಳ್ಳು_certinfo(data, mem, "Expire date", i);
    ಯದಿ(result)
      ಭಙ್ಗ;

    pubkey = X509_get_pubkey(x);
    ಯದಿ(!pubkey)
      infof(data, "   Unable to load public key");
    ಅನ್ಯಥಾ {
      ವಿಕಲ್ಪ(EVP_PKEY_id(pubkey)) {
      ಪಕ್ಷ EVP_PKEY_RSA:
        result = get_pkey_rsa(data, pubkey, mem, i);
        ಭಙ್ಗ;

#ifndef OPENSSL_NO_DSA
      ಪಕ್ಷ EVP_PKEY_DSA:
        result = get_pkey_dsa(data, pubkey, mem, i);
        ಭಙ್ಗ;
#endif

      ಪಕ್ಷ EVP_PKEY_DH:
        result = get_pkey_dh(data, pubkey, mem, i);
        ಭಙ್ಗ;
      }
      EVP_PKEY_free(pubkey);
    }

    ಯದಿ(!result && psig) {
      ಧ್ರುವ unsigned ವರ್ಣ *psigdata = ASN1_STRING_get0_data(psig);
      ಧ್ರುವ size_t psiglen = ossl_ASN1_STRING_get_length(psig);
      ಪ್ರತಿ(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      result = ತಳ್ಳು_certinfo(data, mem, "Signature", i);
    }

    ಯದಿ(!result) {
      PEM_write_bio_X509(mem, x);
      result = ತಳ್ಳು_certinfo(data, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  ಯದಿ(result)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(data);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_data(bio, ರಿಕ್ತ);
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ossl_bio_cf_destroy(BIO *bio)
{
  ಯದಿ(!bio)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ long ossl_bio_cf_ctrl(BIO *bio, ಪೂರ್ಣಾಙ್ಕ cmd, long ಅಂಕೆ, ಶೂನ್ಯ *ptr)
{
  ಸಂರಚನಾ Curl_cfilter *cf = BIO_get_data(bio);
  long ret = 1;

  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ptr;
  ವಿಕಲ್ಪ(cmd) {
  ಪಕ್ಷ BIO_CTRL_GET_CLOSE:
    ret = (long)BIO_get_shutdown(bio);
    ಭಙ್ಗ;
  ಪಕ್ಷ BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ);
    ಭಙ್ಗ;
  ಪಕ್ಷ BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    ಭಙ್ಗ;
  ಪಕ್ಷ BIO_CTRL_DUP:
    ret = 1;
    ಭಙ್ಗ;
  ಪಕ್ಷ BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
    ಪ್ರತ್ಯಾಗಮ connssl->peer_closed;
  }
  ಸ್ವಭಾವ:
    ret = 0;
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ossl_bio_cf_out_write(BIO *bio, ಧ್ರುವ ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ blen)
{
  ಸಂರಚನಾ Curl_cfilter *cf = BIO_get_data(bio);
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  ಯದಿ(blen < 0)
    ಪ್ರತ್ಯಾಗಮ 0;

  result = Curl_conn_cf_send(cf->next, data,
                             (ಧ್ರುವ uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಙ್ಕ)result, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)nwritten;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ossl_bio_cf_in_read(BIO *bio, ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ blen)
{
  ಸಂರಚನಾ Curl_cfilter *cf = BIO_get_data(bio);
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result, r2;

  DEBUGASSERT(data);
  /* OpenSSL catches this case, so should we. */
  ಯದಿ(!buf)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ(blen < 0)
    ಪ್ರತ್ಯಾಗಮ 0;

  result = Curl_conn_cf_recv(cf->next, data, buf, (size_t)blen, &nread);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಙ್ಕ)result, nread);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  ಅನ್ಯಥಾ {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->ಒಳಹರಿವು_pending = TRUE;
    ಯದಿ(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  ಯದಿ(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, data, octx);
    ಯದಿ(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_result = r2;
      ಪ್ರತ್ಯಾಗಮ -1;
    }
    octx->x509_store_setup = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ result ? -1 : (ಪೂರ್ಣಾಙ್ಕ)nread;
}

ಸ್ಥಿರ BIO_METHOD *ossl_bio_cf_method_create(ಶೂನ್ಯ)
{
  BIO_METHOD *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  ಯದಿ(m) {
    BIO_meth_set_write(m, &ossl_bio_cf_out_write);
    BIO_meth_set_read(m, &ossl_bio_cf_in_read);
    BIO_meth_set_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_set_create(m, &ossl_bio_cf_create);
    BIO_meth_set_destroy(m, &ossl_bio_cf_destroy);
  }
  ಪ್ರತ್ಯಾಗಮ m;
}

ಸ್ಥಿರ ಶೂನ್ಯ ossl_bio_cf_method_free(BIO_METHOD *m)
{
  ಯದಿ(m)
    BIO_meth_free(m);
}

#ifndef HAVE_KEYLOG_UPSTREAM
#ifdef HAVE_KEYLOG_CALLBACK
ಸ್ಥಿರ ಶೂನ್ಯ ossl_keylog_callback(ಧ್ರುವ SSL *ssl, ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
  (ಶೂನ್ಯ)ssl;

  Curl_tls_keylog_write_ಸಾಲು(ಸಾಲು);
}
#ಅನ್ಯಥಾ
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ossl_log_tls12_secret(ಧ್ರುವ SSL *ssl, bool *keylog_done)
{
  ಧ್ರುವ SSL_SESSION *session;
  unsigned ವರ್ಣ client_random[SSL3_RANDOM_SIZE];
  unsigned ವರ್ಣ master_ಕೀಲಿ[SSL_MAX_MASTER_ಕೀಲಿ_LENGTH];
  ಪೂರ್ಣಾಙ್ಕ master_ಕೀಲಿ_length = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  ಯದಿ(!session || *keylog_done) {
    ERR_ತೆಗೆ_to_mark();
    ಪ್ರತ್ಯಾಗಮ;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_SIZE);
  master_ಕೀಲಿ_length = (ಪೂರ್ಣಾಙ್ಕ)
    SSL_SESSION_get_master_ಕೀಲಿ(session, master_ಕೀಲಿ, SSL_MAX_MASTER_ಕೀಲಿ_LENGTH);

  ERR_ತೆಗೆ_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  ಯದಿ(master_ಕೀಲಿ_length <= 0)
    ಪ್ರತ್ಯಾಗಮ;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        ಪರಿಮಾಣ(client_random),
                        master_ಕೀಲಿ, master_ಕೀಲಿ_length);
}
#endif /* !HAVE_KEYLOG_CALLBACK */
#endif /* HAVE_KEYLOG_UPSTREAM */

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *SSL_ERROR_to_str(ಪೂರ್ಣಾಙ್ಕ err)
{
  ವಿಕಲ್ಪ(err) {
  ಪಕ್ಷ SSL_ERROR_NONE:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_NONE";
  ಪಕ್ಷ SSL_ERROR_SSL:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_SSL";
  ಪಕ್ಷ SSL_ERROR_WANT_READ:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_WANT_READ";
  ಪಕ್ಷ SSL_ERROR_WANT_WRITE:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_WANT_WRITE";
  ಪಕ್ಷ SSL_ERROR_WANT_X509_LOOKUP:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_WANT_X509_LOOKUP";
  ಪಕ್ಷ SSL_ERROR_SYSCALL:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_SYSCALL";
  ಪಕ್ಷ SSL_ERROR_ZERO_RETURN:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_ZERO_RETURN";
  ಪಕ್ಷ SSL_ERROR_WANT_CONNECT:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_WANT_CONNECT";
  ಪಕ್ಷ SSL_ERROR_WANT_ACCEPT:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_WANT_ACCEPT";
#ifdef SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ಪಕ್ಷ SSL_ERROR_WANT_ASYNC:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_WANT_ASYNC";
#endif
#ifdef SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ಪಕ್ಷ SSL_ERROR_WANT_ASYNC_JOB:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_WANT_ASYNC_JOB";
#endif
#ifdef SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  ಪಕ್ಷ SSL_ERROR_WANT_CLIENT_HELLO_CB:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#endif
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
ಸ್ಥಿರ ವರ್ಣ *ossl_strerror(unsigned long error, ವರ್ಣ *buf, size_t size)
{
  size_t len;
  DEBUGASSERT(size);
  *buf = '\0';

  len = Curl_ossl_version(buf, size);
  DEBUGASSERT(len < (size - 2));
  ಯದಿ(len < (size - 2)) {
    buf += len;
    size -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ifdef HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, size);
#ಅನ್ಯಥಾ
  ERR_error_string_n(error, buf, size);
#endif

  ಯದಿ(!*buf) {
    ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, size, ಸಂದೇಶ, strlen(ಸಂದೇಶ));
  }

  ಪ್ರತ್ಯಾಗಮ buf;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ passwd_callback(ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ, ಪೂರ್ಣಾಙ್ಕ encrypting, ಶೂನ್ಯ *password)
{
  DEBUGASSERT(encrypting == 0);

  ಯದಿ(!encrypting && ಅಂಕೆ >= 0 && password) {
    ಪೂರ್ಣಾಙ್ಕ klen = curlx_uztosi(strlen((ವರ್ಣ *)password));
    ಯದಿ(ಅಂಕೆ > klen) {
      memcpy(buf, password, klen + 1);
      ಪ್ರತ್ಯಾಗಮ klen;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
ಸ್ಥಿರ bool rand_enough(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ RAND_status() != 0;
}

ಸ್ಥಿರ CURLcode ossl_seed(ಸಂರಚನಾ Curl_easy *data)
{
  /* This might get called before it has been added to a multi handle */
  ಯದಿ(data->multi && data->multi->ssl_seeded)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    ಯದಿ(data->multi)
      data->multi->ssl_seeded = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  failf(data, "Insufficient randomness");
  ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
}

#ifndef SSL_FILETYPE_ENGINE
#define SSL_FILETYPE_ENGINE 42
#endif
#ifndef SSL_FILETYPE_PKCS12
#define SSL_FILETYPE_PKCS12 43
#endif
#ifndef SSL_FILETYPE_PROVIDER
#define SSL_FILETYPE_PROVIDER 44
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ossl_do_file_type(ಧ್ರುವ ವರ್ಣ *type)
{
  ಯದಿ(!type || !type[0])
    ಪ್ರತ್ಯಾಗಮ SSL_FILETYPE_PEM;
  ಯದಿ(curl_strequal(type, "PEM"))
    ಪ್ರತ್ಯಾಗಮ SSL_FILETYPE_PEM;
  ಯದಿ(curl_strequal(type, "DER"))
    ಪ್ರತ್ಯಾಗಮ SSL_FILETYPE_ASN1;
  ಯದಿ(curl_strequal(type, "PROV"))
    ಪ್ರತ್ಯಾಗಮ SSL_FILETYPE_PROVIDER;
  ಯದಿ(curl_strequal(type, "ENG"))
    ಪ್ರತ್ಯಾಗಮ SSL_FILETYPE_ENGINE;
  ಯದಿ(curl_strequal(type, "P12"))
    ಪ್ರತ್ಯಾಗಮ SSL_FILETYPE_PKCS12;
  ಪ್ರತ್ಯಾಗಮ -1;
}

#ಯದಿ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ಧ್ರುವ ವರ್ಣ *password;
  ವಿಕಲ್ಪ(UI_get_string_type(uis)) {
  ಪಕ್ಷ UIT_PROMPT:
  ಪಕ್ಷ UIT_VERIFY:
    password = (ಧ್ರುವ ವರ್ಣ *)UI_get0_user_data(ui);
    ಯದಿ(password && (UI_get_ಒಳಹರಿವು_flags(uis) & UI_ಒಳಹರಿವು_FLAG_DEFAULT_PWD)) {
      UI_set_result(ui, uis, password);
      ಪ್ರತ್ಯಾಗಮ 1;
    }
    FALLTHROUGH();
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  ವಿಕಲ್ಪ(UI_get_string_type(uis)) {
  ಪಕ್ಷ UIT_PROMPT:
  ಪಕ್ಷ UIT_VERIFY:
    ಯದಿ(UI_get0_user_data(ui) &&
       (UI_get_ಒಳಹರಿವು_flags(uis) & UI_ಒಳಹರಿವು_FLAG_DEFAULT_PWD)) {
      ಪ್ರತ್ಯಾಗಮ 1;
    }
    FALLTHROUGH();
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
ಸ್ಥಿರ bool is_pkcs11_uri(ಧ್ರುವ ವರ್ಣ *string)
{
  ಪ್ರತ್ಯಾಗಮ string && curl_strnequal(string, "pkcs11:", 7);
}

#endif

ಸ್ಥಿರ CURLcode ossl_set_engine(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ossl_ctx *octx,
                                ಧ್ರುವ ವರ್ಣ *name);
#ifdef OPENSSL_HAS_PROVIDERS
ಸ್ಥಿರ CURLcode ossl_set_provider(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ossl_ctx *octx,
                                  ಧ್ರುವ ವರ್ಣ *iname);
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ use_certificate_blob(SSL_CTX *ctx, ಧ್ರುವ ಸಂರಚನಾ curl_blob *blob,
                                ಪೂರ್ಣಾಙ್ಕ type, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_passwd)
{
  ಪೂರ್ಣಾಙ್ಕ ret = 0;
  X509 *x = ರಿಕ್ತ;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->data, (ಪೂರ್ಣಾಙ್ಕ)blob->len);
  ಯದಿ(!in)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಯದಿ(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, ರಿಕ್ತ);
  }
  ಅನ್ಯಥಾ ಯದಿ(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, ರಿಕ್ತ, passwd_callback, CURL_UNCONST(ಕೀಲಿ_passwd));
  }
  ಅನ್ಯಥಾ {
    ret = 0;
    ಗಚ್ಛ end;
  }

  ಯದಿ(!x) {
    ret = 0;
    ಗಚ್ಛ end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ use_privatekey_blob(SSL_CTX *ctx, ಧ್ರುವ ಸಂರಚನಾ curl_blob *blob,
                               ಪೂರ್ಣಾಙ್ಕ type, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_passwd)
{
  ಪೂರ್ಣಾಙ್ಕ ret = 0;
  EVP_PKEY *pkey = ರಿಕ್ತ;
  BIO *in = BIO_new_mem_buf(blob->data, (ಪೂರ್ಣಾಙ್ಕ)blob->len);
  ಯದಿ(!in)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಯದಿ(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_Private_ಕೀಲಿ(in, ರಿಕ್ತ, passwd_callback,
                                   CURL_UNCONST(ಕೀಲಿ_passwd));
  ಅನ್ಯಥಾ ಯದಿ(type == SSL_FILETYPE_ASN1)
    pkey = d2i_Private_ಕೀಲಿ_bio(in, ರಿಕ್ತ);
  ಅನ್ಯಥಾ
    ಗಚ್ಛ end;

  ಯದಿ(!pkey)
    ಗಚ್ಛ end;

  ret = SSL_CTX_use_Private_ಕೀಲಿ(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ use_certificate_chain_blob(SSL_CTX *ctx,
                                      ಧ್ರುವ ಸಂರಚನಾ curl_blob *blob,
                                      ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_passwd)
{
  ಪೂರ್ಣಾಙ್ಕ ret = 0;
  X509 *x = ರಿಕ್ತ;
  BIO *in = BIO_new_mem_buf(blob->data, (ಪೂರ್ಣಾಙ್ಕ)blob->len);
  ಯದಿ(!in)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, ರಿಕ್ತ,
                            passwd_callback, CURL_UNCONST(ಕೀಲಿ_passwd));
  ಯದಿ(!x)
    ಗಚ್ಛ end;

  ret = SSL_CTX_use_certificate(ctx, x);

  ಯದಿ(ERR_peek_error() != 0)
    ret = 0;

  ಯದಿ(ret) {
    X509 *ca;
    sslerr_t err;

    ಯದಿ(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      ಗಚ್ಛ end;
    }

    ಯಾವತ್((ca = PEM_read_bio_X509(in, ರಿಕ್ತ, passwd_callback,
                                  CURL_UNCONST(ಕೀಲಿ_passwd))) != ರಿಕ್ತ) {

      ಯದಿ(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        ಗಚ್ಛ end;
      }
    }

    err = ERR_peek_last_error();
    ಯದಿ((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_ಸಾಲು))
      ERR_clear_error();
    ಅನ್ಯಥಾ
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ enginecheck(ಸಂರಚನಾ Curl_easy *data,
                       ಸಂರಚನಾ ossl_ctx *octx,
                       ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_file,
                       ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_passwd)
{
#ifdef USE_OPENSSL_ENGINE
  EVP_PKEY *priv_ಕೀಲಿ = ರಿಕ್ತ;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  ಯದಿ(!octx->p.engine && is_pkcs11_uri(ಕೀಲಿ_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    ಪ್ರತ್ಯಾಗಮ 0;

  ಯದಿ(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    ಯದಿ(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);
    priv_ಕೀಲಿ = ENGINE_load_private_ಕೀಲಿ(octx->p.engine, ಕೀಲಿ_file,
                                       ui_method,
                                       CURL_UNCONST(ಕೀಲಿ_passwd));
    UI_destroy_method(ui_method);
    ಯದಿ(!priv_ಕೀಲಿ) {
      failf(data, "failed to load private key from crypto engine");
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(SSL_CTX_use_Private_ಕೀಲಿ(ctx, priv_ಕೀಲಿ) != 1) {
      failf(data, "unable to set private key");
      EVP_PKEY_free(priv_ಕೀಲಿ);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    EVP_PKEY_free(priv_ಕೀಲಿ);  /* we do not need the handle any more... */
  }
  ಅನ್ಯಥಾ {
    failf(data, "crypto engine not set, cannot load private key");
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ 1;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)ಕೀಲಿ_file;
  (ಶೂನ್ಯ)ಕೀಲಿ_passwd;
  failf(data, "SSL_FILETYPE_ENGINE not supported for private key");
  ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ providercheck(ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ ossl_ctx *octx,
                         ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  ವರ್ಣ error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  ಯದಿ(!octx->p.provider_loaded && is_pkcs11_uri(ಕೀಲಿ_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_ಕೀಲಿ = ರಿಕ್ತ;
    OSSL_STORE_CTX *store = ರಿಕ್ತ;
    OSSL_STORE_INFO *info = ರಿಕ್ತ;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    ಯದಿ(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(ಕೀಲಿ_file, octx->p.libctx,
                               octx->p.propq, ui_method, ರಿಕ್ತ, ರಿಕ್ತ,
                               ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      UI_destroy_method(ui_method);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    ಯದಿ(info) {
      ಪೂರ್ಣಾಙ್ಕ ossl_type = OSSL_STORE_INFO_get_type(info);

      ಯದಿ(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_ಕೀಲಿ = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    ಯದಿ(!priv_ಕೀಲಿ) {
      failf(data, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(SSL_CTX_use_Private_ಕೀಲಿ(ctx, priv_ಕೀಲಿ) != 1) {
      failf(data, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      EVP_PKEY_free(priv_ಕೀಲಿ);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    EVP_PKEY_free(priv_ಕೀಲಿ); /* we do not need the handle any more... */
  }
  ಅನ್ಯಥಾ {
    failf(data, "crypto provider not set, cannot load private key");
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ 1;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)ಕೀಲಿ_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for private key");
  ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ engineload(ಸಂರಚನಾ Curl_easy *data,
                      ಸಂರಚನಾ ossl_ctx *octx,
                      ಧ್ರುವ ವರ್ಣ *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#ಯದಿ defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  ವರ್ಣ error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  ಯದಿ(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    ಪ್ರತ್ಯಾಗಮ 0;

  ಯದಿ(octx->p.engine) {
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ cmd_name[] = "LOAD_CERT_CTRL";
    ಸಂರಚನಾ {
      ಧ್ರುವ ವರ್ಣ *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = ರಿಕ್ತ;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    ಯದಿ(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), ರಿಕ್ತ)) {
      failf(data, "SSL engine does not support loading certificates");
      ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* Load the certificate from the engine */
    ಯದಿ(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, ರಿಕ್ತ, 1)) {
      failf(data, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(!params.cert) {
      failf(data, "SSL engine did not initialize the certificate properly.");
      ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      X509_free(params.cert);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  ಅನ್ಯಥಾ {
    failf(data, "crypto engine not set, cannot load certificate");
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ 1;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)cert_file;
  failf(data, "SSL_FILETYPE_ENGINE not supported for certificate");
  ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ providerload(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ ossl_ctx *octx,
                        ಧ್ರುವ ವರ್ಣ *cert_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  ವರ್ಣ error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  ಯದಿ(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK)
    ಪ್ರತ್ಯಾಗಮ 0;

  ಯದಿ(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = ರಿಕ್ತ;
    X509 *cert = ರಿಕ್ತ;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    ಪೂರ್ಣಾಙ್ಕ rc;

    ಯದಿ(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    ಯದಿ(info) {
      ಪೂರ್ಣಾಙ್ಕ ossl_type = OSSL_STORE_INFO_get_type(info);

      ಯದಿ(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    ಯದಿ(!cert) {
      failf(data, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      ಪ್ರತ್ಯಾಗಮ 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    ಯದಿ(rc != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      ಪ್ರತ್ಯಾಗಮ 0;
    }
  }
  ಅನ್ಯಥಾ {
    failf(data, "crypto provider not set, cannot load certificate");
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ 1;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)cert_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for certificate");
  ಪ್ರತ್ಯಾಗಮ 0;
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pkcs12load(ಸಂರಚನಾ Curl_easy *data,
                      SSL_CTX* ctx,
                      ಧ್ರುವ ಸಂರಚನಾ curl_blob *cert_blob,
                      ಧ್ರುವ ವರ್ಣ *cert_file,
                      ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_passwd)
{
  ವರ್ಣ error_buffer[256];
  BIO *cert_bio = ರಿಕ್ತ;
  PKCS12 *p12 = ರಿಕ್ತ;
  EVP_PKEY *pri;
  X509 *x509;
  ಪೂರ್ಣಾಙ್ಕ cert_done = 0;
  ರಾಶಿ_OF(X509) *ca = ರಿಕ್ತ;
  ಯದಿ(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->data, (ಪೂರ್ಣಾಙ್ಕ)cert_blob->len);
    ಯದಿ(!cert_bio) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      ಪ್ರತ್ಯಾಗಮ 0;
    }
  }
  ಅನ್ಯಥಾ {
    cert_bio = BIO_new(BIO_s_file());
    ಯದಿ(!cert_bio) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
      ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(data, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, ರಿಕ್ತ);
  BIO_free(cert_bio);

  ಯದಿ(!p12) {
    failf(data, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(!PKCS12_parse(p12, ಕೀಲಿ_passwd, &pri, &x509, &ca)) {
    failf(data, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, ಪರಿಮಾಣ(error_buffer)));
    PKCS12_free(p12);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  PKCS12_free(p12);

  ಯದಿ(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(data, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, ಪರಿಮಾಣ(error_buffer)));
    ಗಚ್ಛ fail;
  }

  ಯದಿ(SSL_CTX_use_Private_ಕೀಲಿ(ctx, pri) != 1) {
    failf(data, "unable to use private key from PKCS12 file '%s'", cert_file);
    ಗಚ್ಛ fail;
  }

  ಯದಿ(!SSL_CTX_check_private_ಕೀಲಿ(ctx)) {
    failf(data, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    ಗಚ್ಛ fail;
  }
  /* Set Certificate Verification chain */
  ಯದಿ(ca) {
    ಯಾವತ್(sk_X509_ಅಂಕೆ(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_ತೆಗೆ(ca);
      ಯದಿ(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to client CA list");
        ಗಚ್ಛ fail;
      }
      ಯದಿ(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to certificate chain");
        ಗಚ್ಛ fail;
      }
    }
  }

  cert_done = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_ತೆಗೆ_free(ca, X509_free);
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತೆಗೆ
#endif
  ಯದಿ(!cert_done)
    ಪ್ರತ್ಯಾಗಮ 0; /* failure! */
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ CURLcode client_cert(ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ ossl_ctx *octx,
                            ವರ್ಣ *cert_file,
                            ಧ್ರುವ ಸಂರಚನಾ curl_blob *cert_blob,
                            ಧ್ರುವ ವರ್ಣ *cert_type,
                            ವರ್ಣ *ಕೀಲಿ_file,
                            ಧ್ರುವ ಸಂರಚನಾ curl_blob *ಕೀಲಿ_blob,
                            ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_type,
                            ವರ್ಣ *ಕೀಲಿ_passwd)
{
  ವರ್ಣ error_buffer[256];
  bool check_privkey = TRUE;
  ಪೂರ್ಣಾಙ್ಕ file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  ಯದಿ(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    ಪೂರ್ಣಾಙ್ಕ cert_use_result;

    ಯದಿ(ಕೀಲಿ_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, ಕೀಲಿ_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    ವಿಕಲ್ಪ(file_type) {
    ಪಕ್ಷ SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_result = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, ಕೀಲಿ_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      ಯದಿ(cert_use_result != 1) {
        failf(data,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            ಪರಿಮಾಣ(error_buffer)));
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_result = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, ಕೀಲಿ_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      ಯದಿ(cert_use_result != 1) {
        failf(data,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            ಪರಿಮಾಣ(error_buffer)));
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ SSL_FILETYPE_ENGINE:
      ಯದಿ(!cert_file || !engineload(data, octx, cert_file))
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      ಭಙ್ಗ;

    ಪಕ್ಷ SSL_FILETYPE_PROVIDER:
      ಯದಿ(!cert_file || !providerload(data, octx, cert_file))
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      ಭಙ್ಗ;

    ಪಕ್ಷ SSL_FILETYPE_PKCS12:
      ಯದಿ(!pkcs12load(data, ctx, cert_blob, cert_file, ಕೀಲಿ_passwd))
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      failf(data, "not supported file type '%s' for certificate", cert_type);
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ಯದಿ(!ಕೀಲಿ_file && !ಕೀಲಿ_blob) {
      ಕೀಲಿ_file = cert_file;
      ಕೀಲಿ_blob = cert_blob;
    }
    ಅನ್ಯಥಾ
      file_type = ossl_do_file_type(ಕೀಲಿ_type);

    ವಿಕಲ್ಪ(file_type) {
    ಪಕ್ಷ SSL_FILETYPE_PEM:
    ಪಕ್ಷ SSL_FILETYPE_ASN1:
      cert_use_result = ಕೀಲಿ_blob ?
        use_privatekey_blob(ctx, ಕೀಲಿ_blob, file_type, ಕೀಲಿ_passwd) :
      SSL_CTX_use_Private_ಕೀಲಿ_file(ctx, ಕೀಲಿ_file, file_type);
      ಯದಿ(cert_use_result != 1) {
        failf(data, "unable to set private key file: '%s' type %s",
              ಕೀಲಿ_file ? ಕೀಲಿ_file : "(memory blob)",
              ಕೀಲಿ_type ? ಕೀಲಿ_type : "PEM");
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ SSL_FILETYPE_ENGINE:
      ಯದಿ(!enginecheck(data, octx, ಕೀಲಿ_file, ಕೀಲಿ_passwd))
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      ಭಙ್ಗ;

    ಪಕ್ಷ SSL_FILETYPE_PROVIDER:
      ಯದಿ(!providercheck(data, octx, ಕೀಲಿ_file))
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      ಭಙ್ಗ;

    ಪಕ್ಷ SSL_FILETYPE_PKCS12:
      ಯದಿ(!pcks12_done) {
        failf(data, "file type P12 for private key not supported");
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      }
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      failf(data, "not supported file type for private key");
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    ಯದಿ(!ssl) {
      failf(data, "unable to create an SSL structure");
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }

    x509 = SSL_get_certificate(ssl);

    ಯದಿ(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#ಯದಿ !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_ಕೀಲಿ = SSL_get_privatekey(ssl);
      ಯದಿ(EVP_PKEY_id(priv_ಕೀಲಿ) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_ಕೀಲಿ);
        ಯದಿ(RSA_flags(rsa) & RSA_METHOD_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#endif

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    ಯದಿ(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      ಯದಿ(!SSL_CTX_check_private_ಕೀಲಿ(ctx)) {
        failf(data, "Private key does not match the certificate public key");
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef CURLVERBOSE
/* returns non-zero on failure */
ಸ್ಥಿರ CURLcode x509_name_oneline(ಧ್ರುವ X509_NAME *a, ಸಂರಚನಾ dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  ಪೂರ್ಣಾಙ್ಕ rc;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ಯದಿ(bio_out) {
    unsigned long flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    ಯದಿ(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      result = curlx_dyn_addn(d, biomem->data, biomem->length);
    }
    BIO_free(bio_out);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ossl_init(ಶೂನ್ಯ)
{
  ಧ್ರುವ uint64_t flags =
#ifdef OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#endif
#ifdef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    OPENSSL_INIT_NO_LOAD_CONFIG |
#ಅನ್ಯಥಾ
    OPENSSL_INIT_LOAD_CONFIG |
#endif
    0;
  OPENSSL_init_ssl(flags, ರಿಕ್ತ);

#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#endif

  ಪ್ರತ್ಯಾಗಮ 1;
}

/* Global cleanup */
ಸ್ಥಿರ ಶೂನ್ಯ ossl_cleanup(ಶೂನ್ಯ)
{
#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#endif
}

/* Selects an OpenSSL crypto engine or provider.
 */
ಸ್ಥಿರ CURLcode ossl_set_engine(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ossl_ctx *octx,
                                ಧ್ರುವ ವರ್ಣ *name)
{
#ifdef USE_OPENSSL_ENGINE
  CURLcode result = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  ಯದಿ(e) {

    ಯದಿ(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = ರಿಕ್ತ;
    }
    ಯದಿ(!ENGINE_init(e)) {
      ವರ್ಣ buf[256];

      ENGINE_free(e);
      failf(data, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, ಪರಿಮಾಣ(buf)));
      result = CURLE_SSL_ENGINE_INITFAILED;
      e = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ {
      result = CURLE_OK;
    }
    octx->p.engine = e;
    ಪ್ರತ್ಯಾಗಮ result;
  }
#endif
#ifdef OPENSSL_HAS_PROVIDERS
  ಪ್ರತ್ಯಾಗಮ ossl_set_provider(data, octx, name);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)name;
  failf(data, "OpenSSL engine not found");
  ಪ್ರತ್ಯಾಗಮ CURLE_SSL_ENGINE_NOTFOUND;
#endif
}

/* Sets engine as default for all SSL operations
 */
ಸ್ಥಿರ CURLcode ossl_set_engine_default(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ossl_ctx *octx)
{
#ifdef USE_OPENSSL_ENGINE
  ಯದಿ(octx->p.engine) {
    ಯದಿ(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(data, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    ಅನ್ಯಥಾ {
      failf(data, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)octx;
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
ಸ್ಥಿರ ಸಂರಚನಾ curl_slist *ossl_engines_list(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ curl_slist *list = ರಿಕ್ತ;
#ifdef USE_OPENSSL_ENGINE
  ಸಂರಚನಾ curl_slist *beg;
  ENGINE *e;

  ಪ್ರತಿ(e = ENGINE_get_first(); e; e = ENGINE_get_next(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    ಯದಿ(!beg) {
      curl_slist_free_all(list);
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    list = beg;
  }
#endif
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ list;
}

#ifdef OPENSSL_HAS_PROVIDERS

ಸ್ಥಿರ ಶೂನ್ಯ ossl_provider_cleanup(ಸಂರಚನಾ ossl_ctx *octx)
{
  ಯದಿ(!octx)
    ಪ್ರತ್ಯಾಗಮ;
  ಯದಿ(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = ರಿಕ್ತ;
  }
  ಯದಿ(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = ರಿಕ್ತ;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = ರಿಕ್ತ;
  curlx_safefree(octx->p.propq);
  octx->p.provider_loaded = FALSE;
}

#define MAX_PROVIDER_LEN 128 /* reasonable */

/* Selects an OpenSSL crypto provider.
 *
 * A provider might need an associated property, a string passed on to
 * OpenSSL. Specify this as [PROVIDER][:PROPERTY]: separate the name and the
 * property with a colon. No colon means no property is set.
 *
 * An example provider + property looks like "tpm2:?provider=tpm2".
 */
ಸ್ಥಿರ CURLcode ossl_set_provider(ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ossl_ctx *octx,
                                  ಧ್ರುವ ವರ್ಣ *iname)
{
  ವರ್ಣ name[MAX_PROVIDER_LEN + 1];
  ಸಂರಚನಾ Curl_str prov;
  ಧ್ರುವ ವರ್ಣ *propq = ರಿಕ್ತ;

  ಯದಿ(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಯದಿ(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  memcpy(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  ಯದಿ(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    ಯದಿ(!libctx)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಯದಿ(propq) {
      octx->p.propq = curlx_strdup(propq);
      ಯದಿ(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#ifndef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  ಯದಿ(!OSSL_LIB_CTX_load_config(octx->p.libctx, ರಿಕ್ತ)) {
    infof(data, "Failed to load default openssl config. Proceeding.");
  }
#endif

  ಯದಿ(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  ಯದಿ(!octx->p.provider) {
    ವರ್ಣ error_buffer[256];
    failf(data, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_buffer, ಪರಿಮಾಣ(error_buffer)));
    ossl_provider_cleanup(octx);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  ಯದಿ(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(data, "Failed to load base");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_ENGINE_NOTFOUND;
  }
  ಅನ್ಯಥಾ
    octx->p.provider_loaded = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#ಅನ್ಯಥಾ /* OPENSSL_HAS_PROVIDERS */
#define ossl_provider_cleanup(x)
#endif

ಸ್ಥಿರ CURLcode ossl_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ವರ್ಣ buf[1024];
  ಪೂರ್ಣಾಙ್ಕ nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  ಯದಿ(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    ಗಚ್ಛ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  ಯದಿ(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    ಪ್ರತಿ(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf));
      CURL_TRC_CF(data, cf, "SSL shutdown not sent, read -> %d", nread);
      ಯದಿ(nread <= 0)
        ಭಙ್ಗ;
    }
    err = SSL_get_error(octx->ssl, nread);
    ಯದಿ(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool ಒಳಹರಿವು_pending;
      /* Yes, it did. */
      ಯದಿ(!send_shutdown) {
        CURL_TRC_CF(data, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ ಯದಿ(!cf->next->cft->is_alive(cf->next, data, &ಒಳಹರಿವು_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(data, cf, "peer closed connection");
        *done = TRUE;
        ಗಚ್ಛ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ಯದಿ(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    ಪೂರ್ಣಾಙ್ಕ rc;
    ERR_clear_error();
    CURL_TRC_CF(data, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    ಯದಿ(rc == 1) {
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
      *done = TRUE;
      ಗಚ್ಛ out;
    }
    ಯದಿ(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ಗಚ್ಛ out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  ಪ್ರತಿ(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf));
    CURL_TRC_CF(data, cf, "SSL shutdown read -> %d", nread);
    ಯದಿ(nread <= 0)
      ಭಙ್ಗ;
  }
  err = SSL_get_error(octx->ssl, nread);
  ವಿಕಲ್ಪ(err) {
  ಪಕ್ಷ SSL_ERROR_ZERO_RETURN: /* no more data */
    ಯದಿ(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
    ಅನ್ಯಥಾ
      CURL_TRC_CF(data, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ SSL_ERROR_NONE: /* did not get anything */
  ಪಕ್ಷ SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(data, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ಭಙ್ಗ;
  ಪಕ್ಷ SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(data, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(unsigned long sslerr = ERR_get_error());
      CURL_TRC_CF(data, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, ಪರಿಮಾಣ(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    result = CURLE_OK;
    ಭಙ್ಗ;
  }

out:
  cf->shutdown = (result || *done);
  ಯದಿ(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->ಒಳಹರಿವು_pending = FALSE;
  ಪ್ರತ್ಯಾಗಮ result;
}

/* also used by quic-tls */
ಶೂನ್ಯ Curl_osslctx_cleanup(ಸಂರಚನಾ ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  ಯದಿ(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = ರಿಕ್ತ;
  }
  ಯದಿ(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = ರಿಕ್ತ;
    octx->x509_store_setup = FALSE;
  }
  ಯದಿ(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = ರಿಕ್ತ;
  }
#ifdef USE_OPENSSL_ENGINE
  ಯದಿ(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = ರಿಕ್ತ;
  }
#endif
  ossl_provider_cleanup(octx);
}

ಸ್ಥಿರ ಶೂನ್ಯ ossl_close(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  (ಶೂನ್ಯ)data;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
ಸ್ಥಿರ ಶೂನ್ಯ ossl_close_all(ಸಂರಚನಾ Curl_easy *data)
{
  (ಶೂನ್ಯ)data;
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
ಸ್ಥಿರ CURLcode ossl_verifyhost(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ connectdata *conn,
                                ಸಂರಚನಾ ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  ಪೂರ್ಣಾಙ್ಕ target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  ರಾಶಿ_OF(GENERAL_NAME) *altnames;
#ifdef USE_IPV6
  ಸಂರಚನಾ in6_addr addr;
#ಅನ್ಯಥಾ
  ಸಂರಚನಾ in_addr addr;
#endif
  CURLcode result = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = strlen(peer->origin->hostname);

  (ಶೂನ್ಯ)conn;
  ವಿಕಲ್ಪ(peer->type) {
  ಪಕ್ಷ CURL_SSL_PEER_IPV4:
    ಯದಿ(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = ಪರಿಮಾಣ(ಸಂರಚನಾ in_addr);
    ಭಙ್ಗ;
#ifdef USE_IPV6
  ಪಕ್ಷ CURL_SSL_PEER_IPV6:
    ಯದಿ(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = ಪರಿಮಾಣ(ಸಂರಚನಾ in6_addr);
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    DEBUGASSERT(0);
    failf(data, "unexpected SSL peer type: %d", (ಪೂರ್ಣಾಙ್ಕ)peer->type);
    ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, ರಿಕ್ತ, ರಿಕ್ತ);

  ಯದಿ(altnames) {
#ifdef HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#ಅನ್ಯಥಾ
    ಪೂರ್ಣಾಙ್ಕ numalts;
    ಪೂರ್ಣಾಙ್ಕ i;
#endif

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_ಅಂಕೆ(altnames);

    /* loop through all alternatives - until a dnsmatch */
    ಪ್ರತಿ(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ಧ್ರುವ GENERAL_NAME *check = sk_GENERAL_NAME_ಮೌಲ್ಯ(altnames, i);

      ಯದಿ(check->type == GEN_DNS)
        dNSName = TRUE;
      ಅನ್ಯಥಾ ಯದಿ(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      ಯದಿ(check->type == target) {
        /* get data and length */
        ಧ್ರುವ ವರ್ಣ *altptr = (ಧ್ರುವ ವರ್ಣ *)ASN1_STRING_get0_data(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_length(check->d.ia5);

        ವಿಕಲ್ಪ(target) {
        ಪಕ್ಷ GEN_DNS: /* name/pattern comparison */
          ಯದಿ(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (ಪೂರ್ಣಾಙ್ಕ)altlen, altptr);
          }
          ಭಙ್ಗ;

        ಪಕ್ಷ GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          ಯದಿ((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          ಭಙ್ಗ;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  ಯದಿ(matched)
    /* an alternative name matched */
    ;
  ಅನ್ಯಥಾ ಯದಿ(dNSName || iPAddress) {
    ಧ್ರುವ ವರ್ಣ *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(data, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(data, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    result = CURLE_PEER_FAILED_VERIFICATION;
  }
  ಅನ್ಯಥಾ {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    ಪೂರ್ಣಾಙ್ಕ i = -1;
    unsigned ವರ್ಣ *cn = ರಿಕ್ತ;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ಧ್ರುವ X509_NAME *name = X509_get_subject_name(server_cert);
    ಯದಿ(name) {
      ಪೂರ್ಣಾಙ್ಕ j;
      ಯಾವತ್((j = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    ಯದಿ(i >= 0) {
      ಧ್ರುವ ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      ಯದಿ(tmp) {
        ಯದಿ(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_length(tmp);
          cn = (unsigned ವರ್ಣ *)CURL_UNCONST(ASN1_STRING_get0_data(tmp));
        }
        ಅನ್ಯಥಾ { /* not a UTF8 name */
          ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          ಯದಿ(utf8len < 0)
            cnlen = 0;
          ಅನ್ಯಥಾ
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        ಯದಿ((cnlen == 0) || !cn)
          result = CURLE_OUT_OF_MEMORY;
        ಅನ್ಯಥಾ ಯದಿ(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(data, "SSL: illegal cert name field");
          result = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    ಯದಿ(result)
      /* error already detected, pass through */
      ;
    ಅನ್ಯಥಾ ಯದಿ(!cn) {
      failf(data, "SSL: unable to obtain common name from peer certificate");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    ಅನ್ಯಥಾ ಯದಿ(!Curl_cert_hostcheck((ಧ್ರುವ ವರ್ಣ *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(data, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (ಪೂರ್ಣಾಙ್ಕ)cnlen, cn,
            peer->origin->user_hostname);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    ಅನ್ಯಥಾ {
      infof(data, " common name: %.*s (matched)", (ಪೂರ್ಣಾಙ್ಕ)cnlen, cn);
    }
    ಯದಿ(free_cn)
      OPENSSL_free(cn);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef OPENSSL_NO_OCSP
ಸ್ಥಿರ CURLcode verifystatus(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ ossl_ctx *octx)
{
  ಪೂರ್ಣಾಙ್ಕ i, ocsp_status;
#ifdef HAVE_BORINGSSL_LIKE
  ಧ್ರುವ uint8_t *status;
#ಅನ್ಯಥಾ
  unsigned ವರ್ಣ *status;
#endif
  ಧ್ರುವ unsigned ವರ್ಣ *p;
  CURLcode result = CURLE_OK;
  OCSP_RESPONSE *rsp = ರಿಕ್ತ;
  OCSP_BASICRESP *br = ರಿಕ್ತ;
  X509_STORE     *st = ರಿಕ್ತ;
  ರಾಶಿ_OF(X509) *ch = ರಿಕ್ತ;
  X509 *cert;
  OCSP_CERTID *id = ರಿಕ್ತ;
  ಪೂರ್ಣಾಙ್ಕ cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  ಪೂರ್ಣಾಙ್ಕ ret;
  long len;

  (ಶೂನ್ಯ)cf;
  DEBUGASSERT(octx);

  len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  ಯದಿ(!status) {
    failf(data, "No OCSP response received");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(ರಿಕ್ತ, &p, len);
  ಯದಿ(!rsp) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }

  ocsp_status = OCSP_response_status(rsp);
  ಯದಿ(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(data, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }

  br = OCSP_response_get1_basic(rsp);
  ಯದಿ(!br) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }

#ಯದಿ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_ಸಂಖ್ಯೆ) && LIBRESSL_VERSION_ಸಂಖ್ಯೆ < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#ಅನ್ಯಥಾ
  ch = SSL_get_peer_cert_chain(octx->ssl);
#endif
  ಯದಿ(!ch) {
    failf(data, "Could not get peer certificate chain");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  ಯದಿ(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(data, "OCSP response verification failed");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  ಯದಿ(!cert) {
    failf(data, "Error getting peer certificate");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }

  ಪ್ರತಿ(i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)sk_X509_ಅಂಕೆ(ch); i++) {
    X509 *issuer = sk_X509_ಮೌಲ್ಯ(ch, (ossl_valsize_t)i);
    ಯದಿ(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      ಭಙ್ಗ;
    }
  }
  X509_free(cert);

  ಯದಿ(!id) {
    failf(data, "Error computing OCSP ID");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  ಯದಿ(ret != 1) {
    failf(data, "Could not find certificate ID in OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  ಯದಿ(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(data, "OCSP response has expired");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }

  infof(data, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  ವಿಕಲ್ಪ(cert_status) {
  ಪಕ್ಷ V_OCSP_CERTSTATUS_GOOD:
    ಭಙ್ಗ;

  ಪಕ್ಷ V_OCSP_CERTSTATUS_REVOKED:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    failf(data, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    ಗಚ್ಛ end;

  ಪಕ್ಷ V_OCSP_CERTSTATUS_UNKNOWN:
  ಸ್ವಭಾವ:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ end;
  }

end:
  ಯದಿ(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *ssl_ಸಂದೇಶ_type(ಪೂರ್ಣಾಙ್ಕ ssl_ver, ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ)
{
  ಯದಿ(ssl_ver == SSL3_VERSION_MAJOR) {
    ವಿಕಲ್ಪ(ಸಂದೇಶ) {
    ಪಕ್ಷ SSL3_MT_HELLO_REQUEST:
      ಪ್ರತ್ಯಾಗಮ "Hello request";
    ಪಕ್ಷ SSL3_MT_CLIENT_HELLO:
      ಪ್ರತ್ಯಾಗಮ "Client hello";
    ಪಕ್ಷ SSL3_MT_SERVER_HELLO:
      ಪ್ರತ್ಯಾಗಮ "Server hello";
#ifdef SSL3_MT_NEWSESSION_TICKET
    ಪಕ್ಷ SSL3_MT_NEWSESSION_TICKET:
      ಪ್ರತ್ಯಾಗಮ "Newsession Ticket";
#endif
    ಪಕ್ಷ SSL3_MT_CERTIFICATE:
      ಪ್ರತ್ಯಾಗಮ "Certificate";
    ಪಕ್ಷ SSL3_MT_SERVER_ಕೀಲಿ_EXCHANGE:
      ಪ್ರತ್ಯಾಗಮ "Server key exchange";
    ಪಕ್ಷ SSL3_MT_CLIENT_ಕೀಲಿ_EXCHANGE:
      ಪ್ರತ್ಯಾಗಮ "Client key exchange";
    ಪಕ್ಷ SSL3_MT_CERTIFICATE_REQUEST:
      ಪ್ರತ್ಯಾಗಮ "Request CERT";
    ಪಕ್ಷ SSL3_MT_SERVER_DONE:
      ಪ್ರತ್ಯಾಗಮ "Server finished";
    ಪಕ್ಷ SSL3_MT_CERTIFICATE_VERIFY:
      ಪ್ರತ್ಯಾಗಮ "CERT verify";
    ಪಕ್ಷ SSL3_MT_FINISHED:
      ಪ್ರತ್ಯಾಗಮ "Finished";
#ifdef SSL3_MT_CERTIFICATE_STATUS
    ಪಕ್ಷ SSL3_MT_CERTIFICATE_STATUS:
      ಪ್ರತ್ಯಾಗಮ "Certificate Status";
#endif
#ifdef SSL3_MT_ENCRYPTED_EXTENSIONS
    ಪಕ್ಷ SSL3_MT_ENCRYPTED_EXTENSIONS:
      ಪ್ರತ್ಯಾಗಮ "Encrypted Extensions";
#endif
#ifdef SSL3_MT_SUPPLEMENTAL_DATA
    ಪಕ್ಷ SSL3_MT_SUPPLEMENTAL_DATA:
      ಪ್ರತ್ಯಾಗಮ "Supplemental data";
#endif
#ifdef SSL3_MT_END_OF_EARLY_DATA
    ಪಕ್ಷ SSL3_MT_END_OF_EARLY_DATA:
      ಪ್ರತ್ಯಾಗಮ "End of early data";
#endif
#ifdef SSL3_MT_ಕೀಲಿ_UPDATE
    ಪಕ್ಷ SSL3_MT_ಕೀಲಿ_UPDATE:
      ಪ್ರತ್ಯಾಗಮ "Key update";
#endif
#ifdef SSL3_MT_NEXT_PROTO
    ಪಕ್ಷ SSL3_MT_NEXT_PROTO:
      ಪ್ರತ್ಯಾಗಮ "Next protocol";
#endif
#ifdef SSL3_MT_MESSAGE_HASH
    ಪಕ್ಷ SSL3_MT_MESSAGE_HASH:
      ಪ್ರತ್ಯಾಗಮ "Message hash";
#endif
    }
  }
  ಪ್ರತ್ಯಾಗಮ "Unknown";
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *tls_rt_type(ಪೂರ್ಣಾಙ್ಕ type)
{
  ವಿಕಲ್ಪ(type) {
#ifdef SSL3_RT_HEADER
  ಪಕ್ಷ SSL3_RT_HEADER:
    ಪ್ರತ್ಯಾಗಮ "TLS header";
#endif
  ಪಕ್ಷ SSL3_RT_CHANGE_CIPHER_SPEC:
    ಪ್ರತ್ಯಾಗಮ "TLS change cipher";
  ಪಕ್ಷ SSL3_RT_ALERT:
    ಪ್ರತ್ಯಾಗಮ "TLS alert";
  ಪಕ್ಷ SSL3_RT_HANDSHAKE:
    ಪ್ರತ್ಯಾಗಮ "TLS handshake";
  ಪಕ್ಷ SSL3_RT_APPLICATION_DATA:
    ಪ್ರತ್ಯಾಗಮ "TLS app data";
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ossl_trace(ಪೂರ್ಣಾಙ್ಕ direction, ಪೂರ್ಣಾಙ್ಕ ssl_ver, ಪೂರ್ಣಾಙ್ಕ content_type,
                       ಧ್ರುವ ಶೂನ್ಯ *buf, size_t len, SSL *ssl,
                       ಶೂನ್ಯ *userp)
{
  ಧ್ರುವ ವರ್ಣ *verstr;
  ಸಂರಚನಾ Curl_cfilter *cf = userp;
  ಸಂರಚನಾ Curl_easy *data = ರಿಕ್ತ;
  ವರ್ಣ unknown[32];

  ಯದಿ(!cf)
    ಪ್ರತ್ಯಾಗಮ;
  data = CF_DATA_CURRENT(cf);
  ಯದಿ(!data || !data->set.fdebug || (direction && direction != 1))
    ಪ್ರತ್ಯಾಗಮ;

  ವಿಕಲ್ಪ(ssl_ver) {
#ifdef SSL3_VERSION
  ಪಕ್ಷ SSL3_VERSION:
    verstr = "SSLv3";
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ TLS1_VERSION:
    verstr = "TLSv1.0";
    ಭಙ್ಗ;
#ifdef TLS1_1_VERSION
  ಪಕ್ಷ TLS1_1_VERSION:
    verstr = "TLSv1.1";
    ಭಙ್ಗ;
#endif
#ifdef TLS1_2_VERSION
  ಪಕ್ಷ TLS1_2_VERSION:
    verstr = "TLSv1.2";
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ TLS1_3_VERSION:
    verstr = "TLSv1.3";
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    curl_msnprintf(unknown, ಪರಿಮಾಣ(unknown), "(%x)", (unsigned ಪೂರ್ಣಾಙ್ಕ)ssl_ver);
    verstr = unknown;
    ಭಙ್ಗ;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  ಯದಿ(ssl_ver
#ifdef SSL3_RT_HEADER
     && content_type != SSL3_RT_HEADER
#endif
#ifdef SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#endif
    ) {
    ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ_name = "Truncated message";
    ಧ್ರುವ ವರ್ಣ *tls_rt_name;
    ವರ್ಣ ssl_buf[1024];
    ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ_type = 0;
    ಪೂರ್ಣಾಙ್ಕ txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    ಯದಿ(ssl_ver == SSL3_VERSION_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    ಅನ್ಯಥಾ
      tls_rt_name = "";

    ಯದಿ(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      ಯದಿ(len) {
        ಸಂದೇಶ_type = *(ಧ್ರುವ unsigned ವರ್ಣ *)buf;
        ಸಂದೇಶ_name = "Change cipher spec";
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(content_type == SSL3_RT_ALERT) {
      ಯದಿ(len >= 2) {
        ಸಂದೇಶ_type =
          (((ಧ್ರುವ unsigned ವರ್ಣ *)buf)[0] << 8) +
           ((ಧ್ರುವ unsigned ವರ್ಣ *)buf)[1];
        ಸಂದೇಶ_name = SSL_alert_desc_string_long(ಸಂದೇಶ_type);
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(len) {
      ಸಂದೇಶ_type = *(ಧ್ರುವ unsigned ವರ್ಣ *)buf;
      ಸಂದೇಶ_name = ssl_ಸಂದೇಶ_type(ssl_ver, ಸಂದೇಶ_type);
    }

    txt_len = curl_msnprintf(ssl_buf, ಪರಿಮಾಣ(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, ಸಂದೇಶ_name, ಸಂದೇಶ_type);
    Curl_debug(data, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(data, (direction == 1) ? CURLINFO_SSL_DATA_OUT :
             CURLINFO_SSL_DATA_IN, (ಧ್ರುವ ವರ್ಣ *)buf, len);
  (ಶೂನ್ಯ)ssl;
}

ಸ್ಥಿರ CURLcode ossl_set_ssl_version_min_max(ಸಂರಚನಾ Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             unsigned ಪೂರ್ಣಾಙ್ಕ ssl_version_min)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  long curl_ssl_version_min = (long)ssl_version_min;
  long curl_ssl_version_max;

  /* convert curl min SSL version option to OpenSSL constant */
#ಯದಿ defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_VERSION_ಸಂಖ್ಯೆ)
  uint16_t ossl_ssl_version_min = 0;
  uint16_t ossl_ssl_version_max = 0;
#ಅನ್ಯಥಾ
  long ossl_ssl_version_min = 0;
  long ossl_ssl_version_max = 0;
#endif
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_version_min != CURL_SSLVERSION_DEFAULT);
  ವಿಕಲ್ಪ(curl_ssl_version_min) {
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    ಭಙ್ಗ;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (long)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  ವಿಕಲ್ಪ(curl_ssl_version_max) {
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_SSLVERSION_MAX_NONE:  /* none selected */
  ಪಕ್ಷ CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  ಸ್ವಭಾವ:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    ಭಙ್ಗ;
  }

  ಯದಿ(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_ossl_add_session(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ ossl_ctx *octx,
                               ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                               SSL_SESSION *session,
                               ಧ್ರುವ ವರ್ಣ *alpn,
                               unsigned ವರ್ಣ *quic_tp,
                               size_t quic_tp_len,
                               ಸಂರಚನಾ Curl_ssl_session **psession)
{
  ಸಂರಚನಾ Curl_ssl_session *sc_session = ರಿಕ್ತ, *sc_dup = ರಿಕ್ತ;
  unsigned ವರ್ಣ *der_session_buf = ರಿಕ್ತ;
  unsigned ವರ್ಣ *qtp_clone = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  ಯದಿ(psession)
    *psession = ರಿಕ್ತ;
  ಯದಿ(!cf || !data)
    ಗಚ್ಛ out;

  ಯದಿ(Curl_ssl_scache_use(cf, data)) {
    size_t der_session_size;
    unsigned ವರ್ಣ *der_session_ptr;
    size_t earlydata_max = 0;
    ಪೂರ್ಣಾಙ್ಕ ietf_tls_id = SSL_version(octx->ssl);

    der_session_size = i2d_SSL_SESSION(session, ರಿಕ್ತ);
    ಯದಿ(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_size);
    ಯದಿ(!der_session_buf) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }

    der_session_size = i2d_SSL_SESSION(session, &der_session_ptr);
    ಯದಿ(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }

#ifdef HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_data(session);
#endif
    ಯದಿ(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ಧ್ರುವ ವರ್ಣ *)quic_tp, quic_tp_len);
      ಯದಿ(!qtp_clone) {
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ out;
      }
    }

    result = Curl_ssl_session_create2(der_session_buf, der_session_size,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(ರಿಕ್ತ) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = ರಿಕ್ತ;  /* took ownership of sdata */
#ifdef USE_APPLE_SECTRUST
    ಯದಿ(!result)
      sc_session->sectrust_verified = octx->sectrust_verified;
#endif
    ಯದಿ(!result && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        result = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    ಯದಿ(!result) {
      result = Curl_ssl_scache_put(cf, data, ssl_peer_ಕೀಲಿ, sc_session);
      /* took ownership of `sc_session` */
      sc_session = ರಿಕ್ತ;
    }
  }

out:
  curlx_free(der_session_buf);
  ಯದಿ(!result && psession) {
    *psession = sc_dup;
    sc_dup = ರಿಕ್ತ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ಸಂರಚನಾ Curl_cfilter *cf = (ಸಂರಚನಾ Curl_cfilter *)SSL_get_app_data(ssl);
  ಯದಿ(cf) {
    ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
    ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
    ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
    ಸಂರಚನಾ Curl_ssl_session *session = ರಿಕ್ತ;
    Curl_ossl_add_session(cf, data, octx, connssl->peer.scache_ಕೀಲಿ,
                          ssl_sessionid, connssl->negotiated.alpn, ರಿಕ್ತ,
                          0, &session);
    ಯದಿ(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ CURLcode load_cacert_from_memory(X509_STORE *store,
                                        ಧ್ರುವ ಸಂರಚನಾ curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = ರಿಕ್ತ;
  ರಾಶಿ_OF(X509_INFO) *inf = ರಿಕ್ತ;

  /* everything else is a reference */
  ಪೂರ್ಣಾಙ್ಕ i, ಎಣಿಕೆ = 0;
  X509_INFO *itmp = ರಿಕ್ತ;

  ಯದಿ(ca_info_blob->len > (size_t)INT_MAX)
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->data, (ಪೂರ್ಣಾಙ್ಕ)ca_info_blob->len);
  ಯದಿ(!cbio)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
  ಯದಿ(!inf) {
    BIO_free(cbio);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  ಪ್ರತಿ(i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)sk_X509_INFO_ಅಂಕೆ(inf); ++i) {
    itmp = sk_X509_INFO_ಮೌಲ್ಯ(inf, (ossl_valsize_t)i);
    ಯದಿ(itmp->x509) {
      ಯದಿ(X509_STORE_add_cert(store, itmp->x509)) {
        ++ಎಣಿಕೆ;
      }
      ಅನ್ಯಥಾ {
        /* set count to 0 to return an error */
        ಎಣಿಕೆ = 0;
        ಭಙ್ಗ;
      }
    }
    ಯದಿ(itmp->crl) {
      ಯದಿ(X509_STORE_add_crl(store, itmp->crl)) {
        ++ಎಣಿಕೆ;
      }
      ಅನ್ಯಥಾ {
        /* set count to 0 to return an error */
        ಎಣಿಕೆ = 0;
        ಭಙ್ಗ;
      }
    }
  }

#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತಳ್ಳು
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_INFO_ತೆಗೆ_free(inf, X509_INFO_free);
#ಯದಿ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ತೆಗೆ
#endif
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  ಪ್ರತ್ಯಾಗಮ (ಎಣಿಕೆ > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ifdef USE_WIN32_CRYPTO
ಸ್ಥಿರ CURLcode ossl_win_load_store(ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಧ್ರುವ ವರ್ಣ *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode result = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  ಯದಿ(hStore) {
    PCCERT_CONTEXT pContext = ರಿಕ್ತ;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = ರಿಕ್ತ;
    DWORD enhkey_usage_size = 0;
    VERBOSE(size_t total = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    result = CURLE_OK;
    ಪ್ರತಿ(;;) {
      X509 *x509;
      FILETIME now;
      BYTE ಕೀಲಿ_usage[2];
      DWORD req_size;
      ಧ್ರುವ unsigned ವರ್ಣ *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      ಯದಿ(!pContext)
        ಭಙ್ಗ;

      VERBOSE(++total);

#ಯದಿ defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        ವರ್ಣ cert_name[256];
        ಯದಿ(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               ರಿಕ್ತ, cert_name, ಪರಿಮಾಣ(cert_name)))
          infof(data, "SSL: unknown cert name");
        ಅನ್ಯಥಾ
          infof(data, "SSL: Checking cert \"%s\"", cert_name);
      }
#endif
      encoded_cert = (ಧ್ರುವ unsigned ವರ್ಣ *)pContext->pbCertEncoded;
      ಯದಿ(!encoded_cert)
        ಅನುವರ್ತ;

      GetSystemTimeAsFileTime(&now);
      ಯದಿ(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        ಅನುವರ್ತ;

      /* If key usage exists check for signing attribute */
      ಯದಿ(Cert_Get_Intended_ಕೀಲಿ_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 ಕೀಲಿ_usage, ಪರಿಮಾಣ(ಕೀಲಿ_usage))) {
        ಯದಿ(!(ಕೀಲಿ_usage[0] & CERT_ಕೀಲಿ_CERT_SIGN_ಕೀಲಿ_USAGE))
          ಅನುವರ್ತ;
      }
      ಅನ್ಯಥಾ ಯದಿ(GetLastError())
        ಅನುವರ್ತ;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      ಯದಿ(Cert_Get_Enhanced_ಕೀಲಿ_Usage(pContext, 0, ರಿಕ್ತ, &req_size) && req_size) {
        ಯದಿ(req_size > enhkey_usage_size) {
          ಶೂನ್ಯ *tmp = curlx_realloc(enhkey_usage, req_size);

          ಯದಿ(!tmp) {
            failf(data, "SSL: Out of memory allocating for OID list");
            result = CURLE_OUT_OF_MEMORY;
            ಭಙ್ಗ;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_size = req_size;
        }

        ಯದಿ(Cert_Get_Enhanced_ಕೀಲಿ_Usage(pContext, 0, enhkey_usage, &req_size)) {
          ಯದಿ(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            ಯದಿ((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              ಅನುವರ್ತ;
          }
          ಅನ್ಯಥಾ {
            DWORD i;
            bool found = FALSE;

            ಪ್ರತಿ(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              ಯದಿ(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                ಭಙ್ಗ;
              }
            }

            ಯದಿ(!found)
              ಅನುವರ್ತ;
          }
        }
        ಅನ್ಯಥಾ
          ಅನುವರ್ತ;
      }
      ಅನ್ಯಥಾ
        ಅನುವರ್ತ;

      x509 = d2i_X509(ರಿಕ್ತ, &encoded_cert, (long)pContext->cbCertEncoded);
      ಯದಿ(!x509)
        ಅನುವರ್ತ;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      ಯದಿ(X509_STORE_add_cert(store, x509) == 1) {
        VERBOSE(++imported);
#ifdef DEBUGBUILD
        infof(data, "SSL: Imported cert");
#endif
        *padded = TRUE;
      }
      X509_free(x509);
    }

    curlx_free(enhkey_usage);
    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    CURL_TRC_CF(data, cf,
                "ossl_win_load_store() found: %zu imported: %zu certs in %s.",
                total, imported, win_store);

    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ossl_windows_load_anchors(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode result = CURLE_OK;

  *padded = FALSE;
  ಪ್ರತಿ(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    result = ossl_win_load_store(data, cf, win_stores[i], store, &store_added);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(store_added) {
      CURL_TRC_CF(data, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    ಅನ್ಯಥಾ
      infof(data, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* USE_WIN32_CRYPTO */

ಸ್ಥಿರ CURLcode ossl_load_trust_anchors(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ ossl_ctx *octx,
                                        X509_STORE *store)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? ರಿಕ್ತ : conn_config->CAfile);
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  ಯದಿ(conn_config->native_ca_store) {
#ifdef USE_WIN32_CRYPTO
    bool added = FALSE;
    result = ossl_windows_load_anchors(cf, data, store, &added);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(added) {
      infof(data, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#ನೋಚೇದ್ಯದಿ defined(USE_APPLE_SECTRUST)
    infof(data, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#endif
  }

  ಯದಿ(conn_config->ca_info_blob) {
    result = load_cacert_from_memory(store, conn_config->ca_info_blob);
    ಯದಿ(result) {
      failf(data, "error adding trust anchors from certificate blob: %d",
            (ಪೂರ್ಣಾಙ್ಕ)result);
      ಪ್ರತ್ಯಾಗಮ result;
    }
    infof(data, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  ಯದಿ(ssl_cafile || ssl_capath) {
#ifdef HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    ಯದಿ(ssl_cafile) {
      ಯದಿ(!X509_STORE_load_file(store, ssl_cafile)) {
        ಯದಿ(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from file: %s", ssl_cafile);
          ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
        }
        ಅನ್ಯಥಾ
          infof(data, "error setting certificate file, continuing anyway");
      }
      infof(data, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    ಯದಿ(ssl_capath) {
      ಯದಿ(!X509_STORE_load_path(store, ssl_capath)) {
        ಯದಿ(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from path: %s", ssl_capath);
          ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
        }
        ಅನ್ಯಥಾ
          infof(data, "error setting certificate path, continuing anyway");
      }
      infof(data, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#ಅನ್ಯಥಾ
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    ಯದಿ(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      ಯದಿ(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(data, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
      }
      ಅನ್ಯಥಾ {
        infof(data, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    ಯದಿ(ssl_cafile)
      infof(data, "  CAfile: %s", ssl_cafile);
    ಯದಿ(ssl_capath)
      infof(data, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#endif
  }

#ifdef CURL_CA_FALLBACK
  ಯದಿ(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(data, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#endif
  ಯದಿ(octx->store_is_empty && !have_native_check)
    infof(data, "  no trust anchors configured");

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ossl_populate_x509_store(ಸಂರಚನಾ Curl_cfilter *cf,
                                         ಸಂರಚನಾ Curl_easy *data,
                                         ಸಂರಚನಾ ossl_ctx *octx,
                                         X509_STORE *store)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  X509_LOOKUP *lookup = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_crlfile = conn_config->CRLfile;
  unsigned long x509flags = 0;

  CURL_TRC_CF(data, cf, "configuring OpenSSL's x509 trust store");
  ಯದಿ(!store)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ಯದಿ(!conn_config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  result = ossl_load_trust_anchors(cf, data, octx, store);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  /* Does not make sense to load a CRL file without peer verification */
#ifdef USE_APPLE_SECTRUST
  ಯದಿ(ssl_crlfile && conn_config->native_ca_store) {
    failf(data, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  }
#endif
  ಯದಿ(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    ಯದಿ(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(data, "error loading CRL file: %s", ssl_crlfile);
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CRL_BADFILE;
    }
    x509flags = X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL;
    infof(data, " CRLfile: %s", ssl_crlfile);
  }

  /* Try building a chain using issuers in the trusted store first to avoid
     problems with server-sent legacy intermediates. Newer versions of
     OpenSSL do alternate chain checking by default but we do not know how to
     determine that in a reliable manner.
     https://web.archive.org/web/20190422050538/rt.openssl.org/Ticket/Display.html?id=3621
   */
  x509flags |= X509_V_FLAG_TRUSTED_FIRST;

  ಯದಿ(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (ಶೂನ್ಯ)X509_STORE_set_flags(store, x509flags);

  ಪ್ರತ್ಯಾಗಮ result;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_OSSL_X509_ಕೀಲಿ  "tls:ossl:x509:share"

ಸಂರಚನಾ ossl_x509_share {
  ವರ್ಣ *CAfile;         /* CAfile path used to generate X509 store */
  ವರ್ಣ *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  ಸಂರಚನಾ curltime time; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

ಸ್ಥಿರ ಶೂನ್ಯ oss_x509_share_free(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p)
{
  ಸಂರಚನಾ ossl_x509_share *share = p;
  DEBUGASSERT(ಕೀಲಿ_len == CURL_CSTRLEN(MPROTO_OSSL_X509_ಕೀಲಿ));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_ಕೀಲಿ, ಕೀಲಿ, ಕೀಲಿ_len));
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)ಕೀಲಿ_len;
  ಯದಿ(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

ಸ್ಥಿರ bool ossl_cached_x509_store_expired(ಸಂರಚನಾ Curl_easy *data,
                                           ಧ್ರುವ ಸಂರಚನಾ ossl_x509_share *mb)
{
  ಯದಿ(data->set.ssl_ca_cache_timeout < 0)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಅನ್ಯಥಾ {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &mb->time);
    timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

    ಪ್ರತ್ಯಾಗಮ elapsed_ms >= timeout_ms;
  }
}

ಸ್ಥಿರ bool ossl_cached_x509_store_different(ಸಂರಚನಾ Curl_cfilter *cf,
                                             ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data,
                                             ಧ್ರುವ ಸಂರಚನಾ ossl_x509_share *mb)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (ಶೂನ್ಯ)data;
  ಯದಿ(mb->no_partialchain != conn_config->no_partialchain)
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಯದಿ(!mb->CAfile || !conn_config->CAfile) {
    ಯದಿ(mb->CAfile != conn_config->CAfile)
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(strcmp(mb->CAfile, conn_config->CAfile))
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಯದಿ(!mb->CApath || !conn_config->CApath) {
    ಯದಿ(mb->CApath != conn_config->CApath)
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(strcmp(mb->CApath, conn_config->CApath))
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ X509_STORE *ossl_get_cached_x509_store(ಸಂರಚನಾ Curl_cfilter *cf,
                                              ಸಂರಚನಾ Curl_easy *data,
                                              bool *pempty)
{
  ಸಂರಚನಾ Curl_multi *multi = data->multi;
  ಸಂರಚನಾ ossl_x509_share *share;
  X509_STORE *store = ರಿಕ್ತ;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_ಕೀಲಿ,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_ಕೀಲಿ)) : ರಿಕ್ತ;
  ಯದಿ(share && share->store &&
     !ossl_cached_x509_store_expired(data, share) &&
     !ossl_cached_x509_store_different(cf, data, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  ಪ್ರತ್ಯಾಗಮ store;
}

ಸ್ಥಿರ ಶೂನ್ಯ ossl_set_cached_x509_store(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       X509_STORE *store,
                                       bool is_empty)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ Curl_multi *multi = data->multi;
  ಸಂರಚನಾ ossl_x509_share *share;

  DEBUGASSERT(multi);
  ಯದಿ(!multi)
    ಪ್ರತ್ಯಾಗಮ;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_ಕೀಲಿ,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_ಕೀಲಿ));

  ಯದಿ(!share) {
    share = curlx_calloc(1, ಪರಿಮಾಣ(*share));
    ಯದಿ(!share)
      ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_ಕೀಲಿ,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_ಕೀಲಿ),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      ಪ್ರತ್ಯಾಗಮ;
    }
  }

  ಯದಿ(X509_STORE_up_ref(store)) {
    ವರ್ಣ *CAfile = ರಿಕ್ತ;
    ವರ್ಣ *CApath = ರಿಕ್ತ;

    ಯದಿ(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      ಯದಿ(!CAfile) {
        X509_STORE_free(store);
        ಪ್ರತ್ಯಾಗಮ;
      }
    }

    ಯದಿ(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      ಯದಿ(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        ಪ್ರತ್ಯಾಗಮ;
      }
    }

    ಯದಿ(share->store) {
      X509_STORE_free(share->store);
      curlx_free(share->CAfile);
      curlx_free(share->CApath);
    }

    share->time = *Curl_pgrs_now(data);
    share->store = store;
    share->store_is_empty = is_empty;
    share->CAfile = CAfile;
    share->CApath = CApath;
    share->no_partialchain = conn_config->no_partialchain;
  }
}

#ifdef OPENSSL_HAS_PROVIDERS
/* CApath certificates loaded under a provider libctx can outlive that libctx
   when the store is shared with the multi handle. */
#define USING_PROVIDER(octx) octx->p.libctx
#ಅನ್ಯಥಾ
#define USING_PROVIDER(x) 0
#endif

CURLcode Curl_ssl_setup_x509_store(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ossl_ctx *octx)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  X509_STORE *cached_store;
  bool cache_criteria_met, is_empty;

  /* Consider the X509 store cacheable if it comes from CAfile/CApath,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (data->set.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !USING_PROVIDER(octx);

  ERR_set_mark();

  cached_store = ossl_get_cached_x509_store(cf, data, &is_empty);
  ಯದಿ(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  ಅನ್ಯಥಾ {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    result = ossl_populate_x509_store(cf, data, octx, store);
    ಯದಿ(result == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, data, store, (bool)octx->store_is_empty);
    }
  }

  ERR_ತೆಗೆ_to_mark();

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool ossl_apply_session(
  ಸಂರಚನಾ ossl_ctx *octx,
  ಸಂರಚನಾ Curl_cfilter *cf,
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  ಸಂರಚನಾ Curl_ssl_session *scs)
{
  ಸಂರಚನಾ ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ಧ್ರುವ unsigned ವರ್ಣ *der_sessionid = scs->sdata;
  size_t der_sessionid_size = scs->sdata_len;
  SSL_SESSION *ssl_session = ರಿಕ್ತ;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(ರಿಕ್ತ, &der_sessionid,
                                (long)der_sessionid_size);
  ಯದಿ(ssl_session) {
    ಯದಿ(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(ವರ್ಣ error_buffer[256]);
      infof(data, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ಪರಿಮಾಣ(error_buffer)));
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(conn_cfg->verifypeer &&
         (SSL_get_verify_result(octx->ssl) != X509_V_OK)
#ifdef USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#endif
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, ರಿಕ್ತ);
        infof(data, "SSL session not peer verified, not reusing");
      }
      ಅನ್ಯಥಾ {
        infof(data, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ifdef USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#endif
        infof(data, "SSL verify result: %lx",
              (unsigned long)SSL_get_verify_result(octx->ssl));
#ifdef HAVE_OPENSSL_EARLYDATA
        ಯದಿ(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_data(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_data = FALSE;
          ಯದಿ(sess_reuse_cb)
            (ಶೂನ್ಯ)sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          ಯದಿ(do_early_data) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_to(alpns, scs->alpn);
          }
        }
#ಅನ್ಯಥಾ
        (ಶೂನ್ಯ)alpns;
        (ಶೂನ್ಯ)conn_cfg;
        (ಶೂನ್ಯ)sess_reuse_cb;
#endif
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  ಅನ್ಯಥಾ {
    infof(data, "SSL session not accepted by OpenSSL, continuing without");
  }
  ಪ್ರತ್ಯಾಗಮ (bool)octx->reused_session;
}

ಸ್ಥಿರ CURLcode ossl_init_session_and_alpns(
  ಸಂರಚನಾ ossl_ctx *octx,
  ಸಂರಚನಾ Curl_cfilter *cf,
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ ssl_peer *peer,
  ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  ಸಂರಚನಾ ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ alpn_spec alpns;
  CURLcode result;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  ಯದಿ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ಸಂರಚನಾ Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    ಯದಿ(scs) {
      ಯದಿ(ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
    }
  }

  ಯದಿ(!octx->reused_session &&
     Curl_ssl_scache_use(cf, data) && !conn_cfg->verifystatus) {
    ಸಂರಚನಾ Curl_ssl_session *scs = ರಿಕ್ತ;

    result = Curl_ssl_scache_take(cf, data, peer->scache_ಕೀಲಿ, &scs);
    ಯದಿ(!result && scs && scs->sdata && scs->sdata_len) {
      (ಶೂನ್ಯ)ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, data, peer->scache_ಕೀಲಿ, scs);
  }

  ಯದಿ(alpns.ಎಣಿಕೆ) {
    ಸಂರಚನಾ alpn_proto_buf proto;
    memset(&proto, 0, ಪರಿಮಾಣ(proto));
    result = Curl_alpn_to_proto_buf(&proto, &alpns);
    ಯದಿ(result) {
      failf(data, "Error determining ALPN");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
    ಯದಿ(SSL_set_alpn_protos(octx->ssl, proto.data, proto.len)) {
      failf(data, "Error setting ALPN");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
ಸ್ಥಿರ CURLcode ossl_init_ech(ಸಂರಚನಾ ossl_ctx *octx,
                              ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ssl_peer *peer)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಪೂರ್ಣಾಙ್ಕ trying_ech_now = 0;

  ಯದಿ(!Curl_ssl_ech_enabled(cf))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(conn_config->ech == CURLECH_GREASE) {
    infof(data, "ECH: will GREASE ClientHello");
#ifdef HAVE_BORINGSSL_LIKE
    SSL_set_enable_ech_grease(octx->ssl, 1);
#ಅನ್ಯಥಾ
    SSL_set_options(octx->ssl, SSL_OP_ECH_GREASE);
#endif
  }
  ಅನ್ಯಥಾ ಯದಿ(conn_config->ech && conn_config->ech_config) {
#ifdef HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode result;

    ಯದಿ(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * strlen(conn_config->ech_config);
    result = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    ಯದಿ(result || !ech_config) {
      infof(data, "ECH: cannot base64 decode ECHConfig from command line");
      ಯದಿ(conn_config->ech == CURLECH_HARD)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಯದಿ(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      ಯದಿ(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#ಅನ್ಯಥಾ
    size_t ech_config_len = 0;
    ಯದಿ(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = strlen(conn_config->ech_config);
    ಯದಿ(SSL_set1_ech_config_list(octx->ssl,
                                (ಧ್ರುವ uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      ಯದಿ(conn_config->ech == CURLECH_HARD)
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
    ಅನ್ಯಥಾ
      trying_ech_now = 1;
#endif /* HAVE_BORINGSSL_LIKE */
    infof(data, "ECH: ECHConfig from command line");
  }
  ಅನ್ಯಥಾ {
    ಧ್ರುವ ಸಂರಚನಾ Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, peer->origin);

    ಯದಿ(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ಧ್ರುವ unsigned ವರ್ಣ *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(data, "ECH: ECHConfig from HTTPS RR");
      ಯದಿ(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(data, "ECH: SSL_set1_ech_config_list failed");
        ಯದಿ(conn_config->ech == CURLECH_HARD)
          ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
      }
      ಅನ್ಯಥಾ {
        trying_ech_now = 1;
        infof(data, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    ಅನ್ಯಥಾ {
      infof(data, "ECH: requested but no ECHConfig available");
      ಯದಿ(conn_config->ech == CURLECH_HARD)
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
  }
#ifdef HAVE_BORINGSSL_LIKE
  (ಶೂನ್ಯ)peer;
  ಯದಿ(trying_ech_now && conn_config->ech_public) {
    infof(data, "ECH: setting public_name not supported with BoringSSL");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }
#ಅನ್ಯಥಾ
  ಯದಿ(trying_ech_now && conn_config->ech_public) {
    ಪೂರ್ಣಾಙ್ಕ ret;
    infof(data, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    ಯದಿ(ret != 1) {
      infof(data, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
  }
#endif /* HAVE_BORINGSSL_LIKE */
  ಯದಿ(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(data, "ECH: cannot force TLSv1.3 [ERROR]");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

ಸ್ಥಿರ CURLcode ossl_init_ssl(ಸಂರಚನಾ ossl_ctx *octx,
                              ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ ssl_peer *peer,
                              ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpns_requested,
                              ಶೂನ್ಯ *ssl_user_data,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  ಯದಿ(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  ಯದಿ(!octx->ssl) {
    failf(data, "SSL: could not create a context (handle)");
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_data(octx->ssl, ssl_user_data);

#ifndef OPENSSL_NO_OCSP
  ಯದಿ(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#endif

  SSL_set_connect_state(octx->ssl);

  ಯದಿ(peer->sni) {
    ಯದಿ(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(data, "Failed set SNI");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
  }

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode result = ossl_init_ech(octx, cf, data, peer);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  ಪ್ರತ್ಯಾಗಮ ossl_init_session_and_alpns(octx, cf, data, peer,
                                     alpns_requested, sess_reuse_cb);
}

ಸ್ಥಿರ CURLcode ossl_init_method(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ssl_peer *peer,
                                 ಧ್ರುವ SSL_METHOD **pmethod,
                                 unsigned ಪೂರ್ಣಾಙ್ಕ *pssl_version_min)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = ರಿಕ್ತ;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  ವಿಕಲ್ಪ(peer->transport) {
  ಪಕ್ಷ TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    ವಿಕಲ್ಪ(*pssl_version_min) {
    ಪಕ್ಷ CURL_SSLVERSION_TLSv1:
    ಪಕ್ಷ CURL_SSLVERSION_TLSv1_0:
    ಪಕ್ಷ CURL_SSLVERSION_TLSv1_1:
    ಪಕ್ಷ CURL_SSLVERSION_TLSv1_2:
    ಪಕ್ಷ CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_SSLVERSION_SSLv2:
      failf(data, "No SSLv2 support");
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
    ಪಕ್ಷ CURL_SSLVERSION_SSLv3:
      failf(data, "No SSLv3 support");
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
    ಸ್ವಭಾವ:
      failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    ಯದಿ(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(data, "QUIC needs at least TLS version 1.3");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    failf(data, "unsupported transport %d in SSL init", peer->transport);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  ಪ್ರತ್ಯಾಗಮ *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(ಸಂರಚನಾ ossl_ctx *octx,
                            ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ ssl_peer *peer,
                            ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            ಶೂನ್ಯ *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            ಶೂನ್ಯ *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ciphers;
  ಧ್ರುವ SSL_METHOD *req_method = ರಿಕ್ತ;
  ctx_option_t ctx_options = 0;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  ವರ್ಣ * ಧ್ರುವ ssl_cert = conn_config->clientcert;
  ಧ್ರುವ ಸಂರಚನಾ curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_cert_type = conn_config->cert_type;
  unsigned ಪೂರ್ಣಾಙ್ಕ ssl_version_min = CURL_SSLVERSION_DEFAULT;
  ವರ್ಣ error_buffer[256];

  /* Make funny stuff to get random input */
  result = ossl_seed(data);
  ಯದಿ(!result) {
    ಯದಿ(CURL_EASY_STR(data, STRING_SSL_ENGINE))
      result = ossl_set_engine(data, octx,
                               CURL_EASY_STR(data, STRING_SSL_ENGINE));
    ಯದಿ(!result && data->set.engine_default)
      result = ossl_set_engine_default(data, octx);
  }

  ಯದಿ(!result) {
    ssl_config->certverifyresult = !X509_V_OK;
    result = ossl_init_method(cf, data, peer, &req_method, &ssl_version_min);
  }
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ifdef OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#endif
    SSL_CTX_new(req_method);

  ಯದಿ(!octx->ssl_ctx) {
    failf(data, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_buffer, ಪರಿಮಾಣ(error_buffer)));
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಯದಿ(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(data->set.fdebug && data->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_ಸಂದೇಶ_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_ಸಂದೇಶ_callback_arg(octx->ssl_ctx, cf);
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
  ಯದಿ(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_ಸೇರಿಸು_EMPTY_FRAGMENTS;

  DEBUGASSERT(ssl_version_min != CURL_SSLVERSION_DEFAULT);
  ವಿಕಲ್ಪ(ssl_version_min) {
  ಪಕ್ಷ CURL_SSLVERSION_SSLv2:
  ಪಕ್ಷ CURL_SSLVERSION_SSLv3:
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    result = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  SSL_CTX_set_options(octx->ssl_ctx, ctx_options);
  SSL_CTX_set_read_ahead(octx->ssl_ctx, 1);

  /* Max TLS1.2 record size 0x4000 + 0x800.
     OpenSSL supports processing "jumbo TLS record" (8 TLS records) in one go
     for some algorithms, so match that here.
     Experimentation shows that a slightly larger buffer is needed
     to avoid short reads.

     However using a large buffer (8 packets) actually decreases performance.
     4 packets is better.
   */
#ifdef HAVE_SSL_CTX_SET_DEFAULT_READ_BUFFER_LEN
  SSL_CTX_set_default_read_buffer_len(octx->ssl_ctx, 0x401e * 4);
#endif

  /* We do retry writes sometimes from another buffer address */
  SSL_CTX_set_mode(octx->ssl_ctx, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

  ciphers = conn_config->cipher_list;
  ಯದಿ(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = ರಿಕ್ತ;
  ಯದಿ(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    ಯದಿ(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(data, "failed setting cipher list: %s", ciphers);
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CIPHER;
    }
    infof(data, "Cipher selection: %s", ciphers);
  }

#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    ಧ್ರುವ ವರ್ಣ *ciphers13 = conn_config->cipher_list13;
    ಯದಿ(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      ಯದಿ(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(data, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CIPHER;
      }
      infof(data, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#endif

  ಯದಿ(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    result = client_cert(data, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->ಕೀಲಿ, conn_config->ಕೀಲಿ_blob,
                         conn_config->ಕೀಲಿ_type,
                         conn_config->ಕೀಲಿ_passwd);
    ಯದಿ(result)
      /* failf() is already done in client_cert() */
      ಪ್ರತ್ಯಾಗಮ result;
  }

#ifdef HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#endif

  {
    ಧ್ರುವ ವರ್ಣ *curves = conn_config->curves;
    ಯದಿ(curves) {
#ifdef HAVE_BORINGSSL_LIKE
#define OSSL_CURVE_CAST(x) (x)
#ಅನ್ಯಥಾ
#define OSSL_CURVE_CAST(x) (ವರ್ಣ *)CURL_UNCONST(x)
#endif
      ಯದಿ(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(data, "failed setting curves list: '%s'", curves);
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CIPHER;
      }
    }
  }

#ifdef HAVE_SSL_CTX_SET1_SIGALGS
#define OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    ಧ್ರುವ ವರ್ಣ *signature_algorithms = conn_config->signature_algorithms;
    ಯದಿ(signature_algorithms) {
      ಯದಿ(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(data, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CIPHER;
      }
    }
  }
#endif

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, ರಿಕ್ತ);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#ಯದಿ !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  ಯದಿ(Curl_tls_keylog_enabled()) {
    SSL_CTX_set_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#endif

  ಯದಿ(cb_new_session) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_set_session_cache_mode(octx->ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_INTERNAL);
    SSL_CTX_sess_set_new_cb(octx->ssl_ctx, cb_new_session);
  }

  /* give application a chance to interfere with SSL set up. */
  ಯದಿ(data->set.ssl_fsslctx) {
    ಸಂರಚನಾ Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    ಯದಿ(!octx->x509_store_setup) {
      result = Curl_ssl_setup_x509_store(cf, data, octx);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, data, easy_fsslctx);
    result = (*data->set.ssl_fsslctx)(data, octx->ssl_ctx,
                                      data->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    ಯದಿ(result) {
      failf(data, "error signaled by SSL ctx callback");
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  ಪ್ರತ್ಯಾಗಮ ossl_init_ssl(octx, cf, data, peer, alpns_requested,
                       ssl_user_data, sess_reuse_cb);
}

ಸ್ಥಿರ CURLcode ossl_on_session_reuse(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ alpn_spec *alpns,
                                      ಸಂರಚನಾ Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  ಪ್ರತ್ಯಾಗಮ Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max);
}

ಶೂನ್ಯ Curl_ossl_report_handshake(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ ossl_ctx *octx)
{
#ifdef CURLVERBOSE
  ಯದಿ(Curl_trc_is_verbose(data)) {
    ಪೂರ್ಣಾಙ್ಕ psigtype_nid = NID_undef;
    ಧ್ರುವ ವರ್ಣ *negotiated_group_name = ರಿಕ್ತ;

#ifdef HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#ಯದಿ OPENSSL_VERSION_ಸಂಖ್ಯೆ >= 0x30200000L
    negotiated_group_name = SSL_get0_group_name(octx->ssl);
#ಅನ್ಯಥಾ
    negotiated_group_name =
      OBJ_nid2sn(SSL_get_negotiated_group(octx->ssl) & 0x0000FFFF);
#endif
#endif

    /* Informational message */
    infof(data, "SSL connection using %s / %s / %s / %s",
          SSL_get_version(octx->ssl),
          SSL_get_cipher(octx->ssl),
          negotiated_group_name ? negotiated_group_name : "[blank]",
          OBJ_nid2sn(psigtype_nid));
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)octx;
#endif /* CURLVERBOSE */
}

ಸ್ಥಿರ CURLcode ossl_connect_step1(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  result = Curl_ossl_ctx_init(octx, cf, data, &connssl->peer,
                              connssl->alpn, ರಿಕ್ತ, ರಿಕ್ತ,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  octx->bio_method = ossl_bio_cf_method_create();
  ಯದಿ(!octx->bio_method)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  ಯದಿ(!bio)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  BIO_set_data(bio, cf);
#ifdef HAVE_SSL_SET0_WBIO
  /* with OpenSSL v1.1.1 we get an alternative to SSL_set_bio() that works
   * without backward compat quirks. Every call takes one reference, so we
   * up it and pass. SSL* then owns and frees it.
   * We check on the function in configure, since LibreSSL and friends
   * each have their own versions to add support for this. */
  BIO_up_ref(bio);
  SSL_set0_rbio(octx->ssl, bio);
  SSL_set0_wbio(octx->ssl, bio);
#ಅನ್ಯಥಾ
  SSL_set_bio(octx->ssl, bio, bio);
#endif

  ಯದಿ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ಸಂರಚನಾ alpn_proto_buf proto;
    memset(&proto, 0, ಪರಿಮಾಣ(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }

  connssl->connecting_state = ssl_connect_2;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ossl_trace_ech_retry_configs(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data, SSL *ssl,
                                        ಪೂರ್ಣಾಙ್ಕ reason)
{
  CURLcode result = CURLE_OK;
  size_t rcl = 0;
  ಪೂರ್ಣಾಙ್ಕ rv = 1;
#ifndef HAVE_BORINGSSL_LIKE
  ವರ್ಣ *inner = ರಿಕ್ತ;
  uint8_t *rcs = ರಿಕ್ತ;
  ವರ್ಣ *outer = ರಿಕ್ತ;
#ಅನ್ಯಥಾ
  ಧ್ರುವ ವರ್ಣ *inner = ರಿಕ್ತ;
  ಧ್ರುವ uint8_t *rcs = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *outer = ರಿಕ್ತ;
  size_t out_name_len = 0;
  ಪೂರ್ಣಾಙ್ಕ servername_type = 0;
#endif
  NOVERBOSE((ಶೂನ್ಯ)reason);

  /* nothing to trace if not doing ECH */
  ಯದಿ(!Curl_ssl_ech_enabled(cf))
    ಪ್ರತ್ಯಾಗಮ rv;
#ifndef HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#ಅನ್ಯಥಾ
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (ಪೂರ್ಣಾಙ್ಕ)rcl;
#endif

  ಯದಿ(rv && rcs) {
    ವರ್ಣ *b64str = ರಿಕ್ತ;
    size_t blen = 0;

    result = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    ಯದಿ(!result && b64str) {
      infof(data, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ifndef HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#ಅನ್ಯಥಾ
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#endif
    }
  }
  ಅನ್ಯಥಾ
    infof(data, "ECH: no retry_configs (rv = %d)", rv);
#ifndef HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#endif
  ಪ್ರತ್ಯಾಗಮ rv;
}

#endif

ಸ್ಥಿರ CURLcode ossl_connect_step2(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  ಸಂರಚನಾ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  ಪೂರ್ಣಾಙ್ಕ err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  ಯದಿ(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, octx);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    octx->x509_store_setup = TRUE;
  }

#ಯದಿ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  ಯದಿ(Curl_tls_keylog_enabled() && !octx->keylog_done)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_done);
#endif

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  ಯದಿ(err != 1) {
    ಪೂರ್ಣಾಙ್ಕ detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(data, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    ಯದಿ(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
    ಯದಿ(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
#ifdef SSL_ERROR_WANT_ASYNC
    ಯದಿ(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
#endif
#ifdef SSL_ERROR_WANT_RETRY_VERIFY
    ಯದಿ(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(data, TRUE);
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
#endif
    ಅನ್ಯಥಾ {
      /* untreated error */
      sslerr_t errdetail;
      ವರ್ಣ error_buffer[256] = "";
      CURLcode result;
      long lerr;
      ಪೂರ್ಣಾಙ್ಕ lib;
      ಪೂರ್ಣಾಙ್ಕ reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      ಯದಿ((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#ifdef SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#endif
         )) {
        result = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_result(octx->ssl);
        ಯದಿ(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        ಅನ್ಯಥಾ
          failf(data, "%s", "SSL certificate verification failed");
      }
#ifdef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      ಅನ್ಯಥಾ ಯದಿ((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        result = CURLE_SSL_CLIENTCERT;
        failf(data, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_buffer, ಪರಿಮಾಣ(error_buffer)));
      }
#endif
#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
      ಅನ್ಯಥಾ ಯದಿ((lib == ERR_LIB_SSL) &&
#ifndef HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#ಅನ್ಯಥಾ
              (reason == SSL_R_ECH_REJECTED)) {
#endif /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, reason);

        result = CURLE_ECH_REQUIRED;
        failf(data, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_buffer, ಪರಿಮಾಣ(error_buffer)));
      }
#endif
      ಅನ್ಯಥಾ {
        result = CURLE_SSL_CONNECT_ERROR;
        failf(data, "TLS connect error: %s",
              ossl_strerror(errdetail, error_buffer, ಪರಿಮಾಣ(error_buffer)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      ಯದಿ(result == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        ವರ್ಣ extramsg[80] = "";
        ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;

        ಯದಿ(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, ಪರಿಮಾಣ(extramsg));
        failf(data, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಅನ್ಯಥಾ {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(data, octx);

#ಯದಿ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    ಯದಿ(Curl_ssl_ech_enabled(cf)) {
      ಸಂರಚನಾ ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      ವರ್ಣ *inner = ರಿಕ್ತ, *outer = ರಿಕ್ತ;
      ಪೂರ್ಣಾಙ್ಕ rv;
      VERBOSE(ಧ್ರುವ ವರ್ಣ *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      ವಿಕಲ್ಪ(rv) {
      ಪಕ್ಷ SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        ಭಙ್ಗ;
      ಪಕ್ಷ SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        ಭಙ್ಗ;
      ಪಕ್ಷ SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        ಭಙ್ಗ;
      ಪಕ್ಷ SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        ಭಙ್ಗ;
      ಪಕ್ಷ SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        ಭಙ್ಗ;
      ಪಕ್ಷ SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        ಭಙ್ಗ;
      ಪಕ್ಷ SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        ಭಙ್ಗ;
      ಪಕ್ಷ SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        ಭಙ್ಗ;
      ಪಕ್ಷ SSL_ECH_STATUS_BAD_NAME: {
        ಯದಿ(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        ಅನ್ಯಥಾ {
          VERBOSE(status = "bad name (unexpected)");
        }
        ಭಙ್ಗ;
      }
      ಸ್ವಭಾವ:
        VERBOSE(status = "unexpected status");
        infof(data, "ECH: unexpected status %d", rv);
      }
      infof(data, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      ಯದಿ(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, 0);
      }
      ಯದಿ(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(data, "ECH: ech-hard failed");
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
      }
    }
    ಅನ್ಯಥಾ {
      infof(data, "ECH: result: status is not attempted");
    }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    ಯದಿ(connssl->alpn) {
      ಧ್ರುವ unsigned ವರ್ಣ *neg_protocol;
      unsigned ಪೂರ್ಣಾಙ್ಕ len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      ಪ್ರತ್ಯಾಗಮ Curl_alpn_set_negotiated(cf, data, connssl, neg_protocol, len);
    }

    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
ಸ್ಥಿರ CURLcode ossl_pkp_pin_peer_pubkey(ಸಂರಚನಾ Curl_easy *data, X509 *cert,
                                         ಧ್ರುವ ವರ್ಣ *pinnedpubkey)
{
  /* Scratch */
  ಪೂರ್ಣಾಙ್ಕ len1 = 0, len2 = 0;
  unsigned ವರ್ಣ *buff1 = ರಿಕ್ತ, *temp = ರಿಕ್ತ;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ಯದಿ(!pinnedpubkey)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(!cert)
    ಪ್ರತ್ಯಾಗಮ result;

  ಕುರು {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), ರಿಕ್ತ);
    ಯದಿ(len1 < 1)
      ಭಙ್ಗ; /* failed */

    buff1 = temp = curlx_malloc(len1);
    ಯದಿ(!buff1)
      ಭಙ್ಗ; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &temp);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    ಯದಿ((len1 != len2) || !temp || ((temp - buff1) != len1))
      ಭಙ್ಗ; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } ಯಾವತ್(0);

  ಯದಿ(buff1)
    curlx_free(buff1);

  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef CURLVERBOSE
#ಯದಿ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_ಸಂಖ್ಯೆ) && LIBRESSL_VERSION_ಸಂಖ್ಯೆ < 0x3060000fL)
ಸ್ಥಿರ ಶೂನ್ಯ infof_certstack(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ SSL *ssl)
{
  ರಾಶಿ_OF(X509) *certstack;
  long verify_result;
  ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_cert_levels;
  ಪೂರ್ಣಾಙ್ಕ cert_level;

  ಯದಿ(!Curl_trc_is_verbose(data))
    ಪ್ರತ್ಯಾಗಮ;

  verify_result = SSL_get_verify_result(ssl);
  ಯದಿ(verify_result != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  ಅನ್ಯಥಾ
    certstack = SSL_get0_verified_chain(ssl);
  ಯದಿ(!certstack)
    ಪ್ರತ್ಯಾಗಮ;
  ಅಂಕೆ_cert_levels = sk_X509_ಅಂಕೆ(certstack);

  ಪ್ರತಿ(cert_level = 0; cert_level < ಅಂಕೆ_cert_levels; cert_level++) {
    ವರ್ಣ cert_algorithm[80] = "";
    ವರ್ಣ group_name_final[80] = "";
    ಧ್ರುವ X509_ALGOR *palg_cert = ರಿಕ್ತ;
    ಧ್ರುವ ASN1_OBJECT *paobj_cert = ರಿಕ್ತ;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ_bits;
    ಪೂರ್ಣಾಙ್ಕ ಕೀಲಿ_sec_bits;
    ಪೂರ್ಣಾಙ್ಕ get_group_name;
    ಧ್ರುವ ವರ್ಣ *type_name;

    current_cert = sk_X509_ಮೌಲ್ಯ(certstack, cert_level);
    ಯದಿ(!current_cert)
      ಅನುವರ್ತ;

    current_pkey = X509_get0_pubkey(current_cert);
    ಯದಿ(!current_pkey)
      ಅನುವರ್ತ;

    X509_get0_signature(ರಿಕ್ತ, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, ರಿಕ್ತ, ರಿಕ್ತ, palg_cert);
    OBJ_obj2txt(cert_algorithm, ಪರಿಮಾಣ(cert_algorithm), paobj_cert, 0);

    ಕೀಲಿ_bits = EVP_PKEY_bits(current_pkey);
#ifndef HAVE_OPENSSL3
#define EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#endif
    ಕೀಲಿ_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#ifdef HAVE_OPENSSL3
    {
      ವರ್ಣ group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               ಪರಿಮಾಣ(group_name), ರಿಕ್ತ);
      curl_msnprintf(group_name_final, ಪರಿಮಾಣ(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#ಅನ್ಯಥಾ
    get_group_name = 0;
    type_name = ರಿಕ್ತ;
#endif

    infof(data, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          ಕೀಲಿ_bits, ಕೀಲಿ_sec_bits, cert_algorithm);
  }
}
#ಅನ್ಯಥಾ
#define infof_certstack(data, ssl)
#endif
#endif /* CURLVERBOSE */

ಸ್ಥಿರ CURLcode ossl_check_issuer(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  X509 *server_cert)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = ರಿಕ್ತ;
  BIO *fp = ರಿಕ್ತ;
  ವರ್ಣ err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode result = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  ಯದಿ(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->data,
                         (ಪೂರ್ಣಾಙ್ಕ)conn_config->issuercert_blob->len);
    ಯದಿ(!fp) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, ಪರಿಮಾಣ(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    ಯದಿ(!fp) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, ಪರಿಮಾಣ(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ out;
    }

    ಯದಿ(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      ಯದಿ(verify_enabled)
        failf(data, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ಗಚ್ಛ out;
    }
  }

  ಯದಿ(fp) {
    issuer = PEM_read_bio_X509(fp, ರಿಕ್ತ, ZERO_NULL, ರಿಕ್ತ);
    ಯದಿ(!issuer) {
      ಯದಿ(verify_enabled)
        failf(data, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ಗಚ್ಛ out;
    }

    ಯದಿ(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      ಯದಿ(verify_enabled)
        failf(data, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ಗಚ್ಛ out;
    }

    infof(data, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  ಯದಿ(fp)
    BIO_free(fp);
  ಯದಿ(issuer)
    X509_free(issuer);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *pinned(ಸಂರಚನಾ Curl_cfilter *cf,
                          ಸಂರಚನಾ Curl_easy *data)
{
  (ಶೂನ್ಯ)cf;
  ಪ್ರತ್ಯಾಗಮ
#ifndef CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#endif
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
}

ಸ್ಥಿರ CURLcode ossl_check_pinned_ಕೀಲಿ(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      X509 *server_cert)
{
  CURLcode result = CURLE_OK;
  ಧ್ರುವ ವರ್ಣ *ptr = pinned(cf, data);
  ಯದಿ(ptr) {
    result = ossl_pkp_pin_peer_pubkey(data, server_cert, ptr);
    ಯದಿ(result)
      failf(data, "SSL: public key does not match pinned public key");
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef CURLVERBOSE
#define MAX_CERT_NAME_LENGTH 2048
ಸ್ಥಿರ CURLcode ossl_infof_cert(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                X509 *server_cert)
{
  BIO *mem = ರಿಕ್ತ;
  ಸಂರಚನಾ dynbuf dname;
  ವರ್ಣ err_buf[256] = "";
  ವರ್ಣ *buf;
  long len;
  CURLcode result = CURLE_OK;

  ಯದಿ(!Curl_trc_is_verbose(data))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_LENGTH);
  mem = BIO_new(BIO_s_mem());
  ಯದಿ(!mem) {
    failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, ಪರಿಮಾಣ(err_buf)));
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  infof(data, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  result = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(data, "  subject: %s", result ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_data(mem, (ವರ್ಣ **)&buf);
  infof(data, "  start date: %.*s", (ಪೂರ್ಣಾಙ್ಕ)len, buf);
  (ಶೂನ್ಯ)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_data(mem, (ವರ್ಣ **)&buf);
  infof(data, "  expire date: %.*s", (ಪೂರ್ಣಾಙ್ಕ)len, buf);
  (ಶೂನ್ಯ)BIO_reset(mem);

  result = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  ಯದಿ(result) /* should be only fatal stuff like OOM */
    ಗಚ್ಛ out;
  infof(data, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* CURLVERBOSE */

#ifdef USE_APPLE_SECTRUST
ಸಂರಚನಾ ossl_certs_ctx {
  ರಾಶಿ_OF(X509) *sk;
  size_t ಅಂಕೆ_certs;
  unsigned ವರ್ಣ *last_der;
};

ಸ್ಥಿರ CURLcode ossl_chain_get_der(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಶೂನ್ಯ *user_data,
                                   size_t i,
                                   unsigned ವರ್ಣ **pder,
                                   size_t *pder_len)
{
  ಸಂರಚನಾ ossl_certs_ctx *chain = user_data;
  X509 *cert;
  ಪೂರ್ಣಾಙ್ಕ der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = ರಿಕ್ತ;

  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
  *pder_len = 0;
  *pder = ರಿಕ್ತ;

  ಯದಿ(i >= chain->ಅಂಕೆ_certs)
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  cert = sk_X509_ಮೌಲ್ಯ(chain->sk, (ಪೂರ್ಣಾಙ್ಕ)i);
  ಯದಿ(!cert)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  ಯದಿ(der_len < 0)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (size_t)der_len;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ossl_apple_verify(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ossl_ctx *octx,
                                  ಸಂರಚನಾ ssl_peer *peer)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ ossl_certs_ctx chain;
  CURLcode result;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, ಪರಿಮಾಣ(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.ಅಂಕೆ_certs = chain.sk ? sk_X509_ಅಂಕೆ(chain.sk) : 0;

  ಯದಿ(!chain.ಅಂಕೆ_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    ಯದಿ(!octx->reused_session) {
      failf(data, "SSL: could not get peer certificate chain");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    ಅನ್ಯಥಾ {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(data, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
  }
  ಅನ್ಯಥಾ {
#ifdef HAVE_BORINGSSL_LIKE
    ಧ್ರುವ uint8_t *ocsp_data = ರಿಕ್ತ;
#ಅನ್ಯಥಾ
    unsigned ವರ್ಣ *ocsp_data = ರಿಕ್ತ;
#endif
    long ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    ಯದಿ(!octx->reused_session)
      ocsp_len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_data);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    ಯದಿ(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_data = ರಿಕ್ತ;
    }

    result = Curl_vtls_apple_verify(cf, data, peer, chain.ಅಂಕೆ_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_data, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = ರಿಕ್ತ;
  }
  octx->sectrust_verified = !result;
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ossl_ctx *octx,
                                   ಸಂರಚನಾ ssl_peer *peer)
{
  ಸಂರಚನಾ connectdata *conn = cf->conn;
  ಸಂರಚನಾ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  long ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  ಯದಿ(data->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    result = ossl_certchain(data, octx->ssl);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  ಯದಿ(!server_cert) {
    /* no verification at all, this maybe acceptable */
    ಯದಿ(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, data))
      ಗಚ್ಛ out;

    failf(data, "SSL: could not get peer certificate");
    result = CURLE_PEER_FAILED_VERIFICATION;
    ಗಚ್ಛ out;
  }

#ifdef CURLVERBOSE
  result = ossl_infof_cert(cf, data, server_cert);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  infof_certstack(data, octx->ssl);
#endif

  ಯದಿ(conn_config->verifyhost) {
    result = ossl_verifyhost(data, conn, peer, server_cert);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_result(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(data, "OpenSSL verify result: %lx", (unsigned long)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  ಯದಿ(verified)
    infof(data, "SSL certificate verified via OpenSSL.");

#ifdef USE_APPLE_SECTRUST
  ಯದಿ(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    result = ossl_apple_verify(cf, data, octx, peer);
    ಯದಿ(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      ಗಚ್ಛ out; /* unexpected error */
    ಯದಿ(octx->sectrust_verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#endif

  ಯದಿ(!verified) {
    /* no trust established, report the OpenSSL status */
    ಯದಿ(conn_config->verifypeer) {
      failf(data, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      result = CURLE_PEER_FAILED_VERIFICATION;
      ಗಚ್ಛ out;
    }
    infof(data, " SSL certificate verification failed, continuing anyway!");
  }

#ifndef OPENSSL_NO_OCSP
  ಯದಿ(conn_config->verifystatus &&
#ifdef USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, data)) &&
#endif
     !octx->reused_session) {
    result = verifystatus(cf, data, octx);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
#endif

  result = ossl_check_issuer(cf, data, server_cert);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = ossl_check_pinned_ಕೀಲಿ(cf, data, server_cert);

out:
  X509_free(server_cert);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ossl_connect_step3(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  result = Curl_ossl_check_peer_cert(cf, data, octx, &connssl->peer);
  ಯದಿ(result)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, data, connssl->peer.scache_ಕೀಲಿ);

  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef HAVE_OPENSSL_EARLYDATA
ಸ್ಥಿರ CURLcode ossl_send_earlydata(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ಧ್ರುವ unsigned ವರ್ಣ *buf;
  size_t blen, nwritten;
  ಪೂರ್ಣಾಙ್ಕ rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_result = CURLE_OK;
  ಯಾವತ್(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_data(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    ಯದಿ(rc <= 0) {
      long sslerror;
      ವರ್ಣ error_buffer[256];
      ಪೂರ್ಣಾಙ್ಕ err = SSL_get_error(octx->ssl, rc);

      ವಿಕಲ್ಪ(err) {
      ಪಕ್ಷ SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        result = CURLE_AGAIN;
        ಗಚ್ಛ out;
      ಪಕ್ಷ SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_AGAIN;
        ಗಚ್ಛ out;
      ಪಕ್ಷ SSL_ERROR_SYSCALL: {
        ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;

        ಯದಿ(octx->io_result == CURLE_AGAIN) {
          result = CURLE_AGAIN;
          ಗಚ್ಛ out;
        }
        sslerror = ERR_get_error();
        ಯದಿ(sslerror)
          ossl_strerror(sslerror, error_buffer, ಪರಿಮಾಣ(error_buffer));
        ಅನ್ಯಥಾ ಯದಿ(sockerr)
          curlx_strerror(sockerr, error_buffer, ಪರಿಮಾಣ(error_buffer));
        ಅನ್ಯಥಾ
          curl_msnprintf(error_buffer, ಪರಿಮಾಣ(error_buffer), "%s",
                         SSL_ERROR_to_str(err));

        failf(data, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_SEND_ERROR;
        ಗಚ್ಛ out;
      }
      ಪಕ್ಷ SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(data, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_buffer, ಪರಿಮಾಣ(error_buffer)));
        result = CURLE_SEND_ERROR;
        ಗಚ್ಛ out;
      }
      ಸ್ವಭಾವ:
        /* a true error */
        failf(data, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        result = CURLE_SEND_ERROR;
        ಗಚ್ಛ out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* HAVE_OPENSSL_EARLYDATA */

ಸ್ಥಿರ CURLcode ossl_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             bool *done)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  ಯದಿ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ಯದಿ(connssl->connecting_state == ssl_connect_1) {
    ಯದಿ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    CURL_TRC_CF(data, cf, "ossl_connect, step1");
    result = ossl_connect_step1(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(data, cf, "ossl_connect, step2");
#ifdef HAVE_OPENSSL_EARLYDATA
    ಯದಿ(connssl->earlydata_state == ssl_earlydata_await) {
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(connssl->earlydata_state == ssl_earlydata_sending) {
      result = ossl_send_earlydata(cf, data);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#endif
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    result = ossl_connect_step2(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  ಯದಿ(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(data, cf, "ossl_connect, step3");
    result = ossl_connect_step3(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    connssl->connecting_state = ssl_connect_done;
#ifdef HAVE_OPENSSL_EARLYDATA
    ಯದಿ(connssl->earlydata_state > ssl_earlydata_none) {
      ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_data_status(octx->ssl) == SSL_EARLY_DATA_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
  }

  ಯದಿ(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(data, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  ಯದಿ(result == CURLE_AGAIN) {
    *done = FALSE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool ossl_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ (bool)connssl->ಒಳಹರಿವು_pending;
}

ಸ್ಥಿರ CURLcode ossl_send(ಸಂರಚನಾ Curl_cfilter *cf,
                          ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ ಶೂನ್ಯ *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  ಪೂರ್ಣಾಙ್ಕ err;
  ವರ್ಣ error_buffer[256];
  sslerr_t sslerror;
  ಪೂರ್ಣಾಙ್ಕ memlen;
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)INT_MAX) ? INT_MAX : (ಪೂರ್ಣಾಙ್ಕ)len;
  ಯದಿ(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    ಯದಿ(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  ಯದಿ(nwritten > 0) {
    ಯದಿ(nwritten > memlen) {
      failf(data, "SSL_write(len=%d) returned %d", memlen, nwritten);
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
    *pnwritten = (size_t)nwritten;
  }
  ಅನ್ಯಥಾ {
    err = SSL_get_error(octx->ssl, nwritten);

    ವಿಕಲ್ಪ(err) {
    ಪಕ್ಷ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      result = CURLE_AGAIN;
      ಗಚ್ಛ out;
    ಪಕ್ಷ SSL_ERROR_WANT_WRITE:
      result = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      ಗಚ್ಛ out;
    ಪಕ್ಷ SSL_ERROR_SYSCALL: {
      ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;

      ಯದಿ(octx->io_result == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        result = CURLE_AGAIN;
        ಗಚ್ಛ out;
      }
      sslerror = ERR_get_error();
      ಯದಿ(sslerror)
        ossl_strerror(sslerror, error_buffer, ಪರಿಮಾಣ(error_buffer));
      ಅನ್ಯಥಾ ಯದಿ(sockerr)
        curlx_strerror(sockerr, error_buffer, ಪರಿಮಾಣ(error_buffer));
      ಅನ್ಯಥಾ
        curl_msnprintf(error_buffer, ಪರಿಮಾಣ(error_buffer), "%s",
                       SSL_ERROR_to_str(err));

      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_buffer, sockerr);
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
    ಪಕ್ಷ SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(data, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_buffer, ಪರಿಮಾಣ(error_buffer)));
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
    ಸ್ವಭಾವ:
      /* a true error */
      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      result = CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
  }

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ossl_recv(ಸಂರಚನಾ Curl_cfilter *cf,
                          ಸಂರಚನಾ Curl_easy *data,   /* transfer */
                          ವರ್ಣ *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  ವರ್ಣ error_buffer[256];
  unsigned long sslerror;
  ಪೂರ್ಣಾಙ್ಕ buffsize;
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)INT_MAX) ? INT_MAX : (ಪೂರ್ಣಾಙ್ಕ)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  ಯದಿ(nread > 0)
    *pnread = (size_t)nread;
  ಅನ್ಯಥಾ {
    /* failed SSL_read */
    ಪೂರ್ಣಾಙ್ಕ err = SSL_get_error(octx->ssl, nread);

    ವಿಕಲ್ಪ(err) {
    ಪಕ್ಷ SSL_ERROR_NONE: /* this is not an error */
      ಭಙ್ಗ;
    ಪಕ್ಷ SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      ಯದಿ(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(data, cf, "TLS close_notify");
      ಭಙ್ಗ;
    ಪಕ್ಷ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      ಗಚ್ಛ out;
    ಪಕ್ಷ SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      ಗಚ್ಛ out;
    ಸ್ವಭಾವ:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      ಯದಿ(octx->io_result == CURLE_AGAIN) {
        result = CURLE_AGAIN;
        ಗಚ್ಛ out;
      }
      sslerror = ERR_get_error();
      ಯದಿ((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
        ಯದಿ(sslerror)
          ossl_strerror(sslerror, error_buffer, ಪರಿಮಾಣ(error_buffer));
        ಅನ್ಯಥಾ ಯದಿ(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_buffer, ಪರಿಮಾಣ(error_buffer));
        ಅನ್ಯಥಾ
          curl_msnprintf(error_buffer, ಪರಿಮಾಣ(error_buffer), "%s",
                         SSL_ERROR_to_str(err));
        failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_RECV_ERROR;
        ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ ಯದಿ(err == SSL_ERROR_SYSCALL) {
        ಯದಿ(octx->io_result) {
          /* logging handling in underlying filter already */
          result = octx->io_result;
        }
        ಅನ್ಯಥಾ ಯದಿ(connssl->peer_closed) {
          failf(data, "Connection closed abruptly");
          result = CURLE_RECV_ERROR;
        }
        ಅನ್ಯಥಾ {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
          ಯದಿ(sockerr)
            curlx_strerror(sockerr, error_buffer, ಪರಿಮಾಣ(error_buffer));
          ಅನ್ಯಥಾ {
            curl_msnprintf(error_buffer, ಪರಿಮಾಣ(error_buffer),
                           "Connection closed abruptly");
          }
          failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_buffer, sockerr);
          result = CURLE_RECV_ERROR;
        }
        ಗಚ್ಛ out;
      }
    }
  }

out:
  ಯದಿ((!result && !*pnread) || (result == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->ಒಳಹರಿವು_pending = FALSE;
  }
  CURL_TRC_CF(data, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread, connssl->ಒಳಹರಿವು_pending);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ossl_get_channel_binding(ಸಂರಚನಾ Curl_easy *data,
                                         int8_t sockindex,
                                         ಸಂರಚನಾ dynbuf *binding)
{
  X509 *cert;
  ಪೂರ್ಣಾಙ್ಕ mdnid;
  bool no_digest_acceptable = FALSE;
  ಧ್ರುವ EVP_MD *algo_type = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *algo_name = ರಿಕ್ತ;
  unsigned ಪೂರ್ಣಾಙ್ಕ length;
  unsigned ವರ್ಣ buf[EVP_MAX_MD_SIZE];

  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ prefix[] = "tls-server-end-point:";
  ಸಂರಚನಾ connectdata *conn = data->conn;
  ಸಂರಚನಾ Curl_cfilter *cf = conn->cfilter[sockindex];
  ಸಂರಚನಾ ossl_ctx *octx = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  ಕುರು {
    ಧ್ರುವ ಸಂರಚನಾ Curl_cftype *cft = cf->cft;
    ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;

    ಯದಿ(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
      ಭಙ್ಗ;
    }

    cf = cf->next;
  } ಯಾವತ್(cf);

  ಯದಿ(!octx) {
    failf(data, "Failed to find the SSL filter");
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  ಯದಿ(!cert)
    /* No server certificate, do not do channel binding */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

#ifdef HAVE_OPENSSL3
  {
    ಪೂರ್ಣಾಙ್ಕ pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    ಯದಿ(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(data, "certificate signature algorithm not recognized");
      result = CURLE_SSL_INVALIDCERTSTATUS;
      ಗಚ್ಛ out;
    }

    ಯದಿ(mdnid != NID_undef) {
      ಯದಿ(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      ಅನ್ಯಥಾ
        algo_type = EVP_get_digestbynid(mdnid);
    }
    ಅನ್ಯಥಾ ಯದಿ(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      ವರ್ಣ mdname[128] = "";
      ಪೂರ್ಣಾಙ್ಕ rc = EVP_PKEY_get_default_digest_name(pkey, mdname, ಪರಿಮಾಣ(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      ಯದಿ(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      ಅನ್ಯಥಾ ಯದಿ(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(data, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#ಅನ್ಯಥಾ /* HAVE_OPENSSL3 */

  ಯದಿ(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, ರಿಕ್ತ)) {
    failf(data,
          "Unable to find digest NID for certificate signature algorithm");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  ಯದಿ(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  ಅನ್ಯಥಾ {
    algo_type = EVP_get_digestbynid(mdnid);
    ಯದಿ(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(data, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      result = CURLE_SSL_INVALIDCERTSTATUS;
      ಗಚ್ಛ out;
    }
  }

#endif /* HAVE_OPENSSL3, else */

  ಯದಿ(!algo_type) {
    ಯದಿ(no_digest_acceptable) {
      infof(data, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      result = CURLE_OK;
      ಗಚ್ಛ out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(data, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ out;
  }

  ಯದಿ(!X509_digest(cert, algo_type, buf, &length)) {
    failf(data, "X509_digest() failed for channel binding");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ಗಚ್ಛ out;
  }

  /* Append "tls-server-end-point:" */
  result = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  ಯದಿ(result)
    ಗಚ್ಛ out;

  /* Append digest */
  result = curlx_dyn_addn(binding, buf, length);

out:
  X509_free(cert);
  ಪ್ರತ್ಯಾಗಮ result;
}

size_t Curl_ossl_version(ವರ್ಣ *buffer, size_t size)
{
#ifdef LIBRESSL_VERSION_ಸಂಖ್ಯೆ
  ವರ್ಣ *p;
  size_t ಎಣಿಕೆ;
  ಧ್ರುವ ವರ್ಣ *ver = OpenSSL_version(OPENSSL_VERSION);
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  ಯದಿ(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  ಎಣಿಕೆ = curl_msnprintf(buffer, size, "%s/%s", OSSL_PACKAGE, ver);
  ಪ್ರತಿ(p = buffer; *p; ++p) {
    ಯದಿ(ISBLANK(*p))
      *p = '_';
  }
  ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
#ನೋಚೇದ್ಯದಿ defined(OPENSSL_IS_AWSLC)
  ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_ಸಂಖ್ಯೆ_STRING);
#ನೋಚೇದ್ಯದಿ defined(OPENSSL_IS_BORINGSSL)
#ifdef CURL_BORINGSSL_VERSION
  ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, size, OSSL_PACKAGE);
#endif
#ಅನ್ಯಥಾ /* OpenSSL 3+ */
  ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
}

/* can be called with data == NULL */
ಸ್ಥಿರ CURLcode ossl_random(ಸಂರಚನಾ Curl_easy *data,
                            unsigned ವರ್ಣ *entropy, size_t length)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಯದಿ(data) {
    ಯದಿ(ossl_seed(data)) /* Initiate the seed if not already done */
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(!rand_enough())
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(length));
  ಪ್ರತ್ಯಾಗಮ rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

ಸ್ಥಿರ CURLcode ossl_sha256sum(ಧ್ರುವ unsigned ವರ್ಣ *ಒಳಹರಿವು,
                               size_t len,
                               unsigned ವರ್ಣ *sha256sum /* output */,
                               size_t unused)
{
  CURLcode result = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (ಶೂನ್ಯ)unused;

  mdctx = EVP_MD_CTX_new();
  ಯದಿ(!mdctx)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  ಯದಿ(!EVP_DigestInit_ex(mdctx, EVP_sha256(), ರಿಕ್ತ)) {
    result = CURLE_FAILED_INIT;
    ಗಚ್ಛ out;
  }
  ಯದಿ(!EVP_DigestUpdate(mdctx, ಒಳಹರಿವು, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, ರಿಕ್ತ))
    result = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool ossl_cert_status_request(ಶೂನ್ಯ)
{
#ifndef OPENSSL_NO_OCSP
  ಪ್ರತ್ಯಾಗಮ TRUE;
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ *ossl_get_internals(ಸಂರಚನಾ ssl_connect_data *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  ಸಂರಚನಾ ossl_ctx *octx = (ಸಂರಚನಾ ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  ಪ್ರತ್ಯಾಗಮ info == CURLINFO_TLS_SESSION ?
    (ಶೂನ್ಯ *)octx->ssl_ctx : (ಶೂನ್ಯ *)octx->ssl;
}

ಧ್ರುವ ಸಂರಚನಾ Curl_ssl Curl_ssl_openssl = {
  { CURLSSLBACKEND_OPENSSL, "openssl" }, /* info */

  SSLSUPP_CA_PATH |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_SSL_CTX |
#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  SSLSUPP_TLS13_CIPHERSUITES |
#endif
#ifdef HAVE_SSL_CTX_SET1_SIGALGS
  SSLSUPP_SIGNATURE_ALGORITHMS |
#endif
#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  SSLSUPP_ECH |
#endif
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_ISSUERCERT_BLOB |
  SSLSUPP_SSL_EC_CURVES |
  SSLSUPP_CRLFILE,

  ಪರಿಮಾಣ(ಸಂರಚನಾ ossl_ctx),

  ossl_init,                /* init */
  ossl_cleanup,             /* cleanup */
  Curl_ossl_version,        /* version */
  ossl_shutdown,            /* shutdown */
  ossl_data_pending,        /* data_pending */
  ossl_random,              /* random */
  ossl_cert_status_request, /* cert_status_request */
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

#endif /* USE_OPENSSL */
