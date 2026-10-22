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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL

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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <openssl/rand.h>
#ಅನ್ತರ್ಭಾವ <openssl/x509v3.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_NO_DSA
#ಅನ್ತರ್ಭಾವ <openssl/dsa.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <openssl/dh.h>
#ಅನ್ತರ್ಭಾವ <openssl/err.h>
#ಅನ್ತರ್ಭಾವ <openssl/conf.h>
#ಅನ್ತರ್ಭಾವ <openssl/bn.h>
#ಅನ್ತರ್ಭಾವ <openssl/rsa.h>
#ಅನ್ತರ್ಭಾವ <openssl/bio.h>
#ಅನ್ತರ್ಭಾವ <openssl/pkcs12.h>
#ಅನ್ತರ್ಭಾವ <openssl/tls1.h>
#ಅನ್ತರ್ಭಾವ <openssl/evp.h>

#ಆದರೆ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#ಅನ್ತರ್ಭಾವ <openssl/ech.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_NO_OCSP
#ಅನ್ತರ್ಭಾವ <openssl/ocsp.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#ವ್ಯಾಖ್ಯಾನಿಸು USE_OPENSSL_ENGINE
#ಅನ್ತರ್ಭಾವ <openssl/engine.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  ಆದರೆ LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#ಅಥವಾಆದರೆ !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#ಅನ್ತರ್ಭಾವ <openssl/provider.h>
#ಅನ್ತರ್ಭಾವ <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#ವ್ಯಾಖ್ಯಾನಿಸು OPENSSL_HAS_PROVIDERS
#ಆದರೆ_ಅಂತ್ಯ

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#ಆದರೆ !defined(LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_ಕೀಲಿ_EXPLICIT_PARAMS))
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SSL_CTX_ಗಣ_DEFAULT_READ_ಬಫರ್_LEN 1
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#ಅನ್ತರ್ಭಾವ <openssl/ui.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL3
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_EVP_PKEY_GET_PARAMS 1
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_EVP_PKEY_GET_PARAMS
#ಅನ್ತರ್ಭಾವ <openssl/core_names.h>
#ವ್ಯಾಖ್ಯಾನಿಸು DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು DECLARE_PKEY_PARAM_BIGNUM(name) ಸ್ಥಿರಾಂಕ BIGNUM *name
#ವ್ಯಾಖ್ಯಾನಿಸು FREE_PKEY_PARAM_BIGNUM(name)
#ಆದರೆ_ಅಂತ್ಯ

/* Whether SSL_CTX_set_ciphersuites is available.
 * BoringSSL: no
 * LibreSSL: supported since 3.4.1 (released 2021-10-14)
 * OpenSSL: supported since 1.1.1 (commit a53b5be6a05)
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_IS_BORINGSSL
#  define HAVE_SSL_CTX_ಗಣ_CIPHERSUITES
#  ifndef OPENSSL_IS_AWSLC
#    define HAVE_SSL_CTX_ಗಣ_POST_HANDSHAKE_AUTH
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/* Whether SSL_CTX_set1_sigalgs_list is available
 * BoringSSL: supported since 0.20240913.0 (commit 826ce15)
 * LibreSSL: no
 * OpenSSL: supported since 1.0.2 (commit 0b362de5f575)
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_SSL_CTX_SET1_SIGALGS
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ
#ವ್ಯಾಖ್ಯಾನಿಸು OSSL_PACKAGE "LibreSSL"
#ಅಥವಾಆದರೆ defined(OPENSSL_IS_AWSLC)
#ವ್ಯಾಖ್ಯಾನಿಸು OSSL_PACKAGE "AWS-LC"
#ಅಥವಾಆದರೆ defined(OPENSSL_IS_BORINGSSL)
#ವ್ಯಾಖ್ಯಾನಿಸು OSSL_PACKAGE "BoringSSL"
#ಅಥವಾಆದರೆ defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#ವ್ಯಾಖ್ಯಾನಿಸು OSSL_PACKAGE "quictls"
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು OSSL_PACKAGE "OpenSSL"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BORINGSSL_LIKE
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ size_t numcert_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t sslerr_t;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ numcert_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sslerr_t;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು ossl_valsize_t numcert_t

#ಆದರೆ OPENSSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x40100000L
#ವ್ಯಾಖ್ಯಾನಿಸು ossl_ASN1_STRING_get_ಉದ್ದಳತೆ ASN1_STRING_get_ಉದ್ದಳತೆ
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ size_t ossl_ASN1_STRING_get_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ASN1_STRING *str)
{
   ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ASN1_STRING_ಉದ್ದಳತೆ(str);
   ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ >= 0 ? (size_t)ಉದ್ದಳತೆ : 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode ತಳ್ಳು_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              BIO *mem, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label, ಪೂರ್ಣಾಂಕ ಅಂಕೆ)
  WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;

ಸ್ಥಿರ CURLcode ತಳ್ಳು_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              BIO *mem, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label, ಪೂರ್ಣಾಂಕ ಅಂಕೆ)
{
  ಅಕ್ಷರ *ptr;
  ದೀರ್ಘ len = BIO_get_mem_ದತ್ತಾಂಶ(mem, &ptr);
  CURLcode ಫಲಿತಾಂಶ = Curl_ssl_ತಳ್ಳು_certinfo_len(ದತ್ತಾಂಶ, ಅಂಕೆ, label, ptr, len);
  (ಶೂನ್ಯ)BIO_reset(mem);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode pubkey_show(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            BIO *mem,
                            ಪೂರ್ಣಾಂಕ ಅಂಕೆ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                            ಸ್ಥಿರಾಂಕ BIGNUM *bn) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;

ಸ್ಥಿರ CURLcode pubkey_show(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            BIO *mem,
                            ಪೂರ್ಣಾಂಕ ಅಂಕೆ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                            ಸ್ಥಿರಾಂಕ BIGNUM *bn)
{
  ಅಕ್ಷರ namebuf[32];

  curl_msnprintf(namebuf, ಗಾತ್ರ(namebuf), "%s(%s)", type, name);

  ಆದರೆ(bn)
    BN_print(mem, bn);
  ಹಿಂತಿರುಗಿ ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, namebuf, ಅಂಕೆ);
}

#ವ್ಯಾಖ್ಯಾನಿಸು print_pubkey_BN(_type, _name, ಅಂಕೆ_2)           \
  pubkey_show(ದತ್ತಾಂಶ, mem, ಅಂಕೆ_2, #_type, #_name, _name)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ asn1_object_dump(ಸ್ಥಿರಾಂಕ ASN1_OBJECT *a, ಅಕ್ಷರ *buf, size_t len)
{
  ಪೂರ್ಣಾಂಕ i = i2t_ASN1_OBJECT(buf, (ಪೂರ್ಣಾಂಕ)len, a);
  ಹಿಂತಿರುಗಿ (i >= (ಪೂರ್ಣಾಂಕ)len);  /* buffer too small */
}

ಸ್ಥಿರ CURLcode X509V3_ext(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ಪೂರ್ಣಾಂಕ certnum,
                           ಸ್ಥಿರಾಂಕ ರಾಶಿ_ದ(X509_EXTENSION) *extsarg)
{
  ಪೂರ್ಣಾಂಕ i;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ
  ರಾಶಿ_ದ(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ ರಾಶಿ_ದ(X509_EXTENSION) *exts = extsarg;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ((ಪೂರ್ಣಾಂಕ)sk_X509_EXTENSION_ಅಂಕೆ(exts) <= 0)
    /* no extensions, bail out */
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಗಾಗಿ(i = 0; i < (ಪೂರ್ಣಾಂಕ)sk_X509_EXTENSION_ಅಂಕೆ(exts); i++) {
    ಸ್ಥಿರಾಂಕ ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_ಮೌಲ್ಯ(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    ಅಕ್ಷರ namebuf[128];
    BIO *bio_ಹೊರಹೋಗು = BIO_new(BIO_s_mem());

    ಆದರೆ(!bio_ಹೊರಹೋಗು)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    obj = X509_EXTENSION_get_object(ext);

    ಆದರೆ(asn1_object_dump(obj, namebuf, ಗಾತ್ರ(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    ಆದರೆ(!X509V3_EXT_print(bio_ಹೊರಹೋಗು, ext, 0, 0))
      ASN1_STRING_print(bio_ಹೊರಹೋಗು,
                        (ಸ್ಥಿರಾಂಕ ASN1_STRING *)X509_EXTENSION_get_ದತ್ತಾಂಶ(ext));

    BIO_get_mem_ptr(bio_ಹೊರಹೋಗು, &biomem);
    ಫಲಿತಾಂಶ = Curl_ssl_ತಳ್ಳು_certinfo_len(ದತ್ತಾಂಶ, certnum, namebuf, biomem->ದತ್ತಾಂಶ,
                                        biomem->ಉದ್ದಳತೆ);
    BIO_free(bio_ಹೊರಹೋಗು);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode get_pkey_rsa(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             EVP_PKEY *pubkey, BIO *mem, ಪೂರ್ಣಾಂಕ i)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_EVP_PKEY_GET_PARAMS
  RSA *rsa = EVP_PKEY_get0_RSA(pubkey);
#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(n);
  DECLARE_PKEY_PARAM_BIGNUM(e);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_N, &n);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_E, &e);
#ಇಲ್ಲದಿದ್ದರೆ
  RSA_get0_ಕೀಲಿ(rsa, &n, &e, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (ಪೂರ್ಣಾಂಕ)(n ? BN_ಅಂಕೆ_ಬಿಟ್ಟುಗಳು(n) : 0));
  ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "RSA Public Key", i);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = print_pubkey_BN(rsa, n, i);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_NO_DSA
ಸ್ಥಿರ CURLcode get_pkey_dsa(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             EVP_PKEY *pubkey, BIO *mem, ಪೂರ್ಣಾಂಕ i)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ಕೀಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ಕೀಲಿ, &pub_ಕೀಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_ಕೀಲಿ(dsa, &pub_ಕೀಲಿ, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_EVP_PKEY_GET_PARAMS */
  ಫಲಿತಾಂಶ = print_pubkey_BN(dsa, p, i);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = print_pubkey_BN(dsa, q, i);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = print_pubkey_BN(dsa, g, i);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = print_pubkey_BN(dsa, pub_ಕೀಲಿ, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ಕೀಲಿ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* !OPENSSL_NO_DSA */

ಸ್ಥಿರ CURLcode get_pkey_dh(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            EVP_PKEY *pubkey, BIO *mem, ಪೂರ್ಣಾಂಕ i)
{
  CURLcode ಫಲಿತಾಂಶ;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ಕೀಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ಕೀಲಿ, &pub_ಕೀಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_ಕೀಲಿ(dh, &pub_ಕೀಲಿ, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_EVP_PKEY_GET_PARAMS */
  ಫಲಿತಾಂಶ = print_pubkey_BN(dh, p, i);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = print_pubkey_BN(dh, q, i);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = print_pubkey_BN(dh, g, i);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = print_pubkey_BN(dh, pub_ಕೀಲಿ, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ಕೀಲಿ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಸ್ಥಿರಾಂಕ X509_PUBKEY pubkeytype_t;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ X509_PUBKEY pubkeytype_t;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode ossl_certchain(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, SSL *ssl)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಾಶಿ_ದ(X509) *sk;
  ಪೂರ್ಣಾಂಕ i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  ಆದರೆ(!sk)
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_ಅಂಕೆ(sk);
  ಆದರೆ(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(ದತ್ತಾಂಶ, "%d certificates is more than allowed (%d)", (ಪೂರ್ಣಾಂಕ)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಫಲಿತಾಂಶ = Curl_ssl_init_certinfo(ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)numcerts);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  mem = BIO_new(BIO_s_mem());
  ಆದರೆ(!mem)
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಗಾಗಿ(i = 0; !ಫಲಿತಾಂಶ && (i < (ಪೂರ್ಣಾಂಕ)numcerts); i++) {
    X509 *x = sk_X509_ಮೌಲ್ಯ(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = ಖಾಲಿ;
    size_t j;
    ಸ್ಥಿರಾಂಕ ASN1_ಬಿಟ್_STRING *psig = ಖಾಲಿ;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Subject", i);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Issuer", i);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    BIO_printf(mem, "%lx", (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)X509_get_ಆವೃತ್ತಿ(x));
    ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Version", i);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    {
      ASN1_INTEGER *ಅಂಕೆ = X509_get_serial_ಸಂಖ್ಯೆ(x);
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *numdata = ASN1_STRING_get0_ದತ್ತಾಂಶ(ಅಂಕೆ);
      ಸ್ಥಿರಾಂಕ size_t numlen = ossl_ASN1_STRING_get_ಉದ್ದಳತೆ(ಅಂಕೆ);
      ಆದರೆ(ASN1_STRING_type(ಅಂಕೆ) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      ಗಾಗಿ(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Serial Number", i);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    {
      ಸ್ಥಿರಾಂಕ X509_ALGOR *sigalg = ಖಾಲಿ;
      pubkeytype_t *xpubkey = ಖಾಲಿ;
      ASN1_OBJECT *pubkeyoid = ಖಾಲಿ;

      X509_get0_signature(&psig, &sigalg, x);
      ಆದರೆ(sigalg) {
        ಸ್ಥಿರಾಂಕ ASN1_OBJECT *sigalgoid = ಖಾಲಿ;
        X509_ALGOR_get0(&sigalgoid, ಖಾಲಿ, ಖಾಲಿ, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Signature Algorithm", i);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ನಿಲ್ಲಿಸು;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      ಆದರೆ(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, xpubkey);
        ಆದರೆ(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Public Key Algorithm", i);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ನಿಲ್ಲಿಸು;
        }
      }

      ಫಲಿತಾಂಶ = X509V3_ext(ದತ್ತಾಂಶ, i, X509_get0_extensions(x));
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
    }

    ASN1_ಸಮಯ_print(mem, X509_get0_notBefore(x));
    ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Start date", i);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    ASN1_ಸಮಯ_print(mem, X509_get0_notAfter(x));
    ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Expire date", i);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    pubkey = X509_get_pubkey(x);
    ಆದರೆ(!pubkey)
      infof(ದತ್ತಾಂಶ, "   Unable to load public key");
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆಯ್ಕೆ(EVP_PKEY_id(pubkey)) {
      ಸ್ಥಿತಿ EVP_PKEY_RSA:
        ಫಲಿತಾಂಶ = get_pkey_rsa(ದತ್ತಾಂಶ, pubkey, mem, i);
        ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_NO_DSA
      ಸ್ಥಿತಿ EVP_PKEY_DSA:
        ಫಲಿತಾಂಶ = get_pkey_dsa(ದತ್ತಾಂಶ, pubkey, mem, i);
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

      ಸ್ಥಿತಿ EVP_PKEY_DH:
        ಫಲಿತಾಂಶ = get_pkey_dh(ದತ್ತಾಂಶ, pubkey, mem, i);
        ನಿಲ್ಲಿಸು;
      }
      EVP_PKEY_free(pubkey);
    }

    ಆದರೆ(!ಫಲಿತಾಂಶ && psig) {
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *psigdata = ASN1_STRING_get0_ದತ್ತಾಂಶ(psig);
      ಸ್ಥಿರಾಂಕ size_t psiglen = ossl_ASN1_STRING_get_ಉದ್ದಳತೆ(psig);
      ಗಾಗಿ(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Signature", i);
    }

    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      PEM_write_bio_X509(mem, x);
      ಫಲಿತಾಂಶ = ತಳ್ಳು_certinfo(ದತ್ತಾಂಶ, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  ಆದರೆ(ಫಲಿತಾಂಶ)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(ದತ್ತಾಂಶ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ossl_bio_cf_create(BIO *bio)
{
  BIO_ಗಣ_shutdown(bio, 1);
  BIO_ಗಣ_init(bio, 1);
  BIO_ಗಣ_ದತ್ತಾಂಶ(bio, ಖಾಲಿ);
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ossl_bio_cf_destroy(BIO *bio)
{
  ಆದರೆ(!bio)
    ಹಿಂತಿರುಗಿ 0;
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ದೀರ್ಘ ossl_bio_cf_ctrl(BIO *bio, ಪೂರ್ಣಾಂಕ cmd, ದೀರ್ಘ ಅಂಕೆ, ಶೂನ್ಯ *ptr)
{
  ರಚನೆ Curl_cfilter *cf = BIO_get_ದತ್ತಾಂಶ(bio);
  ದೀರ್ಘ ret = 1;

  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ptr;
  ಆಯ್ಕೆ(cmd) {
  ಸ್ಥಿತಿ BIO_CTRL_GET_CLOSE:
    ret = (ದೀರ್ಘ)BIO_get_shutdown(bio);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ BIO_CTRL_ಗಣ_CLOSE:
    BIO_ಗಣ_shutdown(bio, (ಪೂರ್ಣಾಂಕ)ಅಂಕೆ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ BIO_CTRL_DUP:
    ret = 1;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
    ಹಿಂತಿರುಗಿ connssl->peer_closed;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ret = 0;
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ossl_bio_cf_ಹೊರಹೋಗು_write(BIO *bio, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ blen)
{
  ರಚನೆ Curl_cfilter *cf = BIO_get_ದತ್ತಾಂಶ(bio);
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(blen < 0)
    ಹಿಂತಿರುಗಿ 0;

  ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ,
                             (ಸ್ಥಿರಾಂಕ uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      BIO_ಗಣ_retry_write(bio);
    ಹಿಂತಿರುಗಿ -1;
  }
  ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)nwritten;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ossl_bio_cf_ಒಳ_read(BIO *bio, ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ blen)
{
  ರಚನೆ Curl_cfilter *cf = BIO_get_ದತ್ತಾಂಶ(bio);
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  size_t nread;
  CURLcode ಫಲಿತಾಂಶ, r2;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  /* OpenSSL catches this case, so should we. */
  ಆದರೆ(!buf)
    ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(blen < 0)
    ಹಿಂತಿರುಗಿ 0;

  ಫಲಿತಾಂಶ = Curl_conn_cf_recv(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, (size_t)blen, &nread);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nread);
  BIO_clear_retry_flags(bio);
  octx->io_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      BIO_ಗಣ_retry_read(bio);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->ಒಳಹರಿವು_pending = TRUE;
    ಆದರೆ(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  ಆದರೆ(!octx->x509_store_ಸಿದ್ಧತೆ) {
    r2 = Curl_ssl_ಸಿದ್ಧತೆ_x509_store(cf, ದತ್ತಾಂಶ, octx);
    ಆದರೆ(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_ಫಲಿತಾಂಶ = r2;
      ಹಿಂತಿರುಗಿ -1;
    }
    octx->x509_store_ಸಿದ್ಧತೆ = TRUE;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? -1 : (ಪೂರ್ಣಾಂಕ)nread;
}

ಸ್ಥಿರ BIO_ವಿಧಾನ *ossl_bio_cf_ವಿಧಾನ_create(ಶೂನ್ಯ)
{
  BIO_ವಿಧಾನ *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  ಆದರೆ(m) {
    BIO_meth_ಗಣ_write(m, &ossl_bio_cf_ಹೊರಹೋಗು_write);
    BIO_meth_ಗಣ_read(m, &ossl_bio_cf_ಒಳ_read);
    BIO_meth_ಗಣ_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_ಗಣ_create(m, &ossl_bio_cf_create);
    BIO_meth_ಗಣ_destroy(m, &ossl_bio_cf_destroy);
  }
  ಹಿಂತಿರುಗಿ m;
}

ಸ್ಥಿರ ಶೂನ್ಯ ossl_bio_cf_ವಿಧಾನ_free(BIO_ವಿಧಾನ *m)
{
  ಆದರೆ(m)
    BIO_meth_free(m);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_KEYLOG_UPSTREAM
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_KEYLOG_CALLBACK
ಸ್ಥಿರ ಶೂನ್ಯ ossl_keylog_callback(ಸ್ಥಿರಾಂಕ SSL *ssl, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
  (ಶೂನ್ಯ)ssl;

  Curl_tls_keylog_write_ಸಾಲು(ಸಾಲು);
}
#ಇಲ್ಲದಿದ್ದರೆ
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ossl_log_tls12_secret(ಸ್ಥಿರಾಂಕ SSL *ssl, bool *keylog_ಮುಗಿದ)
{
  ಸ್ಥಿರಾಂಕ SSL_SESSION *session;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ client_random[SSL3_RANDOM_ಗಾತ್ರ];
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ master_ಕೀಲಿ[SSL_MAX_MASTER_ಕೀಲಿ_ಉದ್ದಳತೆ];
  ಪೂರ್ಣಾಂಕ master_ಕೀಲಿ_ಉದ್ದಳತೆ = 0;

  ERR_ಗಣ_mark();

  session = SSL_get_session(ssl);

  ಆದರೆ(!session || *keylog_ಮುಗಿದ) {
    ERR_ತೆಗೆ_ಗಮ್ಯ_mark();
    ಹಿಂತಿರುಗಿ;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_ಗಾತ್ರ);
  master_ಕೀಲಿ_ಉದ್ದಳತೆ = (ಪೂರ್ಣಾಂಕ)
    SSL_SESSION_get_master_ಕೀಲಿ(session, master_ಕೀಲಿ, SSL_MAX_MASTER_ಕೀಲಿ_ಉದ್ದಳತೆ);

  ERR_ತೆಗೆ_ಗಮ್ಯ_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  ಆದರೆ(master_ಕೀಲಿ_ಉದ್ದಳತೆ <= 0)
    ಹಿಂತಿರುಗಿ;

  *keylog_ಮುಗಿದ = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        ಗಾತ್ರ(client_random),
                        master_ಕೀಲಿ, master_ಕೀಲಿ_ಉದ್ದಳತೆ);
}
#ಆದರೆ_ಅಂತ್ಯ /* !HAVE_KEYLOG_CALLBACK */
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_KEYLOG_UPSTREAM */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *SSL_ERROR_ಗಮ್ಯ_str(ಪೂರ್ಣಾಂಕ err)
{
  ಆಯ್ಕೆ(err) {
  ಸ್ಥಿತಿ SSL_ERROR_NONE:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_NONE";
  ಸ್ಥಿತಿ SSL_ERROR_SSL:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_SSL";
  ಸ್ಥಿತಿ SSL_ERROR_WANT_READ:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_WANT_READ";
  ಸ್ಥಿತಿ SSL_ERROR_WANT_WRITE:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_WANT_WRITE";
  ಸ್ಥಿತಿ SSL_ERROR_WANT_X509_LOOKUP:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_WANT_X509_LOOKUP";
  ಸ್ಥಿತಿ SSL_ERROR_SYSCALL:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_SYSCALL";
  ಸ್ಥಿತಿ SSL_ERROR_ZERO_RETURN:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_ZERO_RETURN";
  ಸ್ಥಿತಿ SSL_ERROR_WANT_CONNECT:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_WANT_CONNECT";
  ಸ್ಥಿತಿ SSL_ERROR_WANT_ACCEPT:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_WANT_ACCEPT";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ಸ್ಥಿತಿ SSL_ERROR_WANT_ASYNC:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_WANT_ASYNC";
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_ERROR_WANT_ASYNC_ಕೆಲಸ  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ಸ್ಥಿತಿ SSL_ERROR_WANT_ASYNC_ಕೆಲಸ:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_WANT_ASYNC_JOB";
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  ಸ್ಥಿತಿ SSL_ERROR_WANT_CLIENT_HELLO_CB:
    ಹಿಂತಿರುಗಿ "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
ಸ್ಥಿರ ಅಕ್ಷರ *ossl_strerror(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ error, ಅಕ್ಷರ *buf, size_t ಗಾತ್ರ_2)
{
  size_t len;
  DEBUGASSERT(ಗಾತ್ರ_2);
  *buf = '\0';

  len = Curl_ossl_ಆವೃತ್ತಿ(buf, ಗಾತ್ರ_2);
  DEBUGASSERT(len < (ಗಾತ್ರ_2 - 2));
  ಆದರೆ(len < (ಗಾತ್ರ_2 - 2)) {
    buf += len;
    ಗಾತ್ರ_2 -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, ಗಾತ್ರ_2);
#ಇಲ್ಲದಿದ್ದರೆ
  ERR_error_string_n(error, buf, ಗಾತ್ರ_2);
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!*buf) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, ಗಾತ್ರ_2, ಸಂದೇಶ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಂದೇಶ));
  }

  ಹಿಂತಿರುಗಿ buf;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ passwd_callback(ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ ಅಂಕೆ, ಪೂರ್ಣಾಂಕ encrypting, ಶೂನ್ಯ *password)
{
  DEBUGASSERT(encrypting == 0);

  ಆದರೆ(!encrypting && ಅಂಕೆ >= 0 && password) {
    ಪೂರ್ಣಾಂಕ klen = curlx_uztosi(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಅಕ್ಷರ *)password));
    ಆದರೆ(ಅಂಕೆ > klen) {
      ಸ್ಮರಣೆ_ನಕಲು(buf, password, klen + 1);
      ಹಿಂತಿರುಗಿ klen;
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
ಸ್ಥಿರ bool rand_enough(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ RAND_ಸ್ಥಿತಿ() != 0;
}

ಸ್ಥಿರ CURLcode ossl_ಬೀಜ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  /* This might get called before it has been added to a multi handle */
  ಆದರೆ(ದತ್ತಾಂಶ->multi && ದತ್ತಾಂಶ->multi->ssl_seeded)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    ಆದರೆ(ದತ್ತಾಂಶ->multi)
      ದತ್ತಾಂಶ->multi->ssl_seeded = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  failf(ದತ್ತಾಂಶ, "Insufficient randomness");
  ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SSL_FILETYPE_ENGINE
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_FILETYPE_ENGINE 42
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SSL_FILETYPE_PKCS12
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_FILETYPE_PKCS12 43
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SSL_FILETYPE_PROVIDER
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_FILETYPE_PROVIDER 44
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ossl_do_file_type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type)
{
  ಆದರೆ(!type || !type[0])
    ಹಿಂತಿರುಗಿ SSL_FILETYPE_PEM;
  ಆದರೆ(curl_strequal(type, "PEM"))
    ಹಿಂತಿರುಗಿ SSL_FILETYPE_PEM;
  ಆದರೆ(curl_strequal(type, "DER"))
    ಹಿಂತಿರುಗಿ SSL_FILETYPE_ASN1;
  ಆದರೆ(curl_strequal(type, "PROV"))
    ಹಿಂತಿರುಗಿ SSL_FILETYPE_PROVIDER;
  ಆದರೆ(curl_strequal(type, "ENG"))
    ಹಿಂತಿರುಗಿ SSL_FILETYPE_ENGINE;
  ಆದರೆ(curl_strequal(type, "P12"))
    ಹಿಂತಿರುಗಿ SSL_FILETYPE_PKCS12;
  ಹಿಂತಿರುಗಿ -1;
}

#ಆದರೆ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *password;
  ಆಯ್ಕೆ(UI_get_string_type(uis)) {
  ಸ್ಥಿತಿ UIT_PROMPT:
  ಸ್ಥಿತಿ UIT_VERIFY:
    password = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)UI_get0_user_ದತ್ತಾಂಶ(ui);
    ಆದರೆ(password && (UI_get_ಒಳಹರಿವು_flags(uis) & UI_ಒಳಹರಿವು_FLAG_DEFAULT_PWD)) {
      UI_ಗಣ_ಫಲಿತಾಂಶ(ui, uis, password);
      ಹಿಂತಿರುಗಿ 1;
    }
    FALLTHROUGH();
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ UI_ವಿಧಾನ_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  ಆಯ್ಕೆ(UI_get_string_type(uis)) {
  ಸ್ಥಿತಿ UIT_PROMPT:
  ಸ್ಥಿತಿ UIT_VERIFY:
    ಆದರೆ(UI_get0_user_ದತ್ತಾಂಶ(ui) &&
       (UI_get_ಒಳಹರಿವು_flags(uis) & UI_ಒಳಹರಿವು_FLAG_DEFAULT_PWD)) {
      ಹಿಂತಿರುಗಿ 1;
    }
    FALLTHROUGH();
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ UI_ವಿಧಾನ_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
ಸ್ಥಿರ bool is_pkcs11_uri(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string)
{
  ಹಿಂತಿರುಗಿ string && curl_strnequal(string, "pkcs11:", 7);
}

#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode ossl_ಗಣ_engine(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ossl_ctx *octx,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_HAS_PROVIDERS
ಸ್ಥಿರ CURLcode ossl_ಗಣ_provider(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ossl_ctx *octx,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *iname);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ use_certificate_blob(SSL_CTX *ctx, ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *blob,
                                ಪೂರ್ಣಾಂಕ type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_passwd)
{
  ಪೂರ್ಣಾಂಕ ret = 0;
  X509 *x = ಖಾಲಿ;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *ಒಳ = BIO_new_mem_buf(blob->ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)blob->len);
  ಆದರೆ(!ಒಳ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(ಒಳ, ಖಾಲಿ);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(ಒಳ, ಖಾಲಿ, passwd_callback, CURL_UNCONST(ಕೀಲಿ_passwd));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ret = 0;
    ಹೋಗು ಕೊನೆ;
  }

  ಆದರೆ(!x) {
    ret = 0;
    ಹೋಗು ಕೊನೆ;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
ಕೊನೆ:
  X509_free(x);
  BIO_free(ಒಳ);
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ use_privatekey_blob(SSL_CTX *ctx, ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *blob,
                               ಪೂರ್ಣಾಂಕ type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_passwd)
{
  ಪೂರ್ಣಾಂಕ ret = 0;
  EVP_PKEY *pkey = ಖಾಲಿ;
  BIO *ಒಳ = BIO_new_mem_buf(blob->ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)blob->len);
  ಆದರೆ(!ಒಳ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_ಖಾಸಗಿ_ಕೀಲಿ(ಒಳ, ಖಾಲಿ, passwd_callback,
                                   CURL_UNCONST(ಕೀಲಿ_passwd));
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type == SSL_FILETYPE_ASN1)
    pkey = d2i_ಖಾಸಗಿ_ಕೀಲಿ_bio(ಒಳ, ಖಾಲಿ);
  ಇಲ್ಲದಿದ್ದರೆ
    ಹೋಗು ಕೊನೆ;

  ಆದರೆ(!pkey)
    ಹೋಗು ಕೊನೆ;

  ret = SSL_CTX_use_ಖಾಸಗಿ_ಕೀಲಿ(ctx, pkey);
  EVP_PKEY_free(pkey);
ಕೊನೆ:
  BIO_free(ಒಳ);
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ use_certificate_chain_blob(SSL_CTX *ctx,
                                      ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *blob,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_passwd)
{
  ಪೂರ್ಣಾಂಕ ret = 0;
  X509 *x = ಖಾಲಿ;
  BIO *ಒಳ = BIO_new_mem_buf(blob->ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)blob->len);
  ಆದರೆ(!ಒಳ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(ಒಳ, ಖಾಲಿ,
                            passwd_callback, CURL_UNCONST(ಕೀಲಿ_passwd));
  ಆದರೆ(!x)
    ಹೋಗು ಕೊನೆ;

  ret = SSL_CTX_use_certificate(ctx, x);

  ಆದರೆ(ERR_peek_error() != 0)
    ret = 0;

  ಆದರೆ(ret) {
    X509 *ca;
    sslerr_t err;

    ಆದರೆ(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      ಹೋಗು ಕೊನೆ;
    }

    ಆಗ((ca = PEM_read_bio_X509(ಒಳ, ಖಾಲಿ, passwd_callback,
                                  CURL_UNCONST(ಕೀಲಿ_passwd))) != ಖಾಲಿ) {

      ಆದರೆ(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        ಹೋಗು ಕೊನೆ;
      }
    }

    err = ERR_peek_ಕೊನೆಯ_error();
    ಆದರೆ((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_ಆರಂಭ_ಸಾಲು))
      ERR_clear_error();
    ಇಲ್ಲದಿದ್ದರೆ
      ret = 0;
  }

ಕೊನೆ:
  X509_free(x);
  BIO_free(ಒಳ);
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ enginecheck(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       ರಚನೆ ossl_ctx *octx,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_file,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_passwd)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL_ENGINE
  EVP_PKEY *priv_ಕೀಲಿ = ಖಾಲಿ;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  ಆದರೆ(!octx->p.engine && is_pkcs11_uri(ಕೀಲಿ_file) &&
     ossl_ಗಣ_engine(ದತ್ತಾಂಶ, octx, "pkcs11") != CURLE_OK)
    ಹಿಂತಿರುಗಿ 0;

  ಆದರೆ(octx->p.engine) {
    UI_ವಿಧಾನ *ui_ವಿಧಾನ = UI_create_ವಿಧಾನ("curl user interface");
    ಆದರೆ(!ui_ವಿಧಾನ) {
      failf(ದತ್ತಾಂಶ, "unable to create " OSSL_PACKAGE " user-interface method");
      ಹಿಂತಿರುಗಿ 0;
    }
    UI_ವಿಧಾನ_ಗಣ_opener(ui_ವಿಧಾನ, UI_ವಿಧಾನ_get_opener(UI_OpenSSL()));
    UI_ವಿಧಾನ_ಗಣ_closer(ui_ವಿಧಾನ, UI_ವಿಧಾನ_get_closer(UI_OpenSSL()));
    UI_ವಿಧಾನ_ಗಣ_reader(ui_ವಿಧಾನ, ssl_ui_reader);
    UI_ವಿಧಾನ_ಗಣ_writer(ui_ವಿಧಾನ, ssl_ui_writer);
    priv_ಕೀಲಿ = ENGINE_ತುಂಬು_ಖಾಸಗಿ_ಕೀಲಿ(octx->p.engine, ಕೀಲಿ_file,
                                       ui_ವಿಧಾನ,
                                       CURL_UNCONST(ಕೀಲಿ_passwd));
    UI_destroy_ವಿಧಾನ(ui_ವಿಧಾನ);
    ಆದರೆ(!priv_ಕೀಲಿ) {
      failf(ದತ್ತಾಂಶ, "failed to load private key from crypto engine");
      ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(SSL_CTX_use_ಖಾಸಗಿ_ಕೀಲಿ(ctx, priv_ಕೀಲಿ) != 1) {
      failf(ದತ್ತಾಂಶ, "unable to set private key");
      EVP_PKEY_free(priv_ಕೀಲಿ);
      ಹಿಂತಿರುಗಿ 0;
    }
    EVP_PKEY_free(priv_ಕೀಲಿ);  /* we do not need the handle any more... */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "crypto engine not set, cannot load private key");
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ 1;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)ಕೀಲಿ_file;
  (ಶೂನ್ಯ)ಕೀಲಿ_passwd;
  failf(ದತ್ತಾಂಶ, "SSL_FILETYPE_ENGINE not supported for private key");
  ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ providercheck(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ ossl_ctx *octx,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_file)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_HAS_PROVIDERS
  ಅಕ್ಷರ error_ಬಫರ್[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  ಆದರೆ(!octx->p.provider_loaded && is_pkcs11_uri(ಕೀಲಿ_file) &&
     ossl_ಗಣ_provider(ದತ್ತಾಂಶ, octx, "pkcs11") != CURLE_OK) {
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_ಕೀಲಿ = ಖಾಲಿ;
    OSSL_STORE_CTX *store = ಖಾಲಿ;
    OSSL_STORE_INFO *info = ಖಾಲಿ;
    UI_ವಿಧಾನ *ui_ವಿಧಾನ = UI_create_ವಿಧಾನ("curl user interface");
    ಆದರೆ(!ui_ವಿಧಾನ) {
      failf(ದತ್ತಾಂಶ, "unable to create " OSSL_PACKAGE " user-interface method");
      ಹಿಂತಿರುಗಿ 0;
    }
    UI_ವಿಧಾನ_ಗಣ_opener(ui_ವಿಧಾನ, UI_ವಿಧಾನ_get_opener(UI_OpenSSL()));
    UI_ವಿಧಾನ_ಗಣ_closer(ui_ವಿಧಾನ, UI_ವಿಧಾನ_get_closer(UI_OpenSSL()));
    UI_ವಿಧಾನ_ಗಣ_reader(ui_ವಿಧಾನ, ssl_ui_reader);
    UI_ವಿಧಾನ_ಗಣ_writer(ui_ವಿಧಾನ, ssl_ui_writer);

    store = OSSL_STORE_open_ex(ಕೀಲಿ_file, octx->p.libctx,
                               octx->p.propq, ui_ವಿಧಾನ, ಖಾಲಿ, ಖಾಲಿ,
                               ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ(!store) {
      failf(ದತ್ತಾಂಶ, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      UI_destroy_ವಿಧಾನ(ui_ವಿಧಾನ);
      ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(ದತ್ತಾಂಶ, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
    }

    info = OSSL_STORE_ತುಂಬು(store);
    ಆದರೆ(info) {
      ಪೂರ್ಣಾಂಕ ossl_type = OSSL_STORE_INFO_get_type(info);

      ಆದರೆ(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_ಕೀಲಿ = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_ವಿಧಾನ(ui_ವಿಧಾನ);
    ಆದರೆ(!priv_ಕೀಲಿ) {
      failf(ದತ್ತಾಂಶ, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(SSL_CTX_use_ಖಾಸಗಿ_ಕೀಲಿ(ctx, priv_ಕೀಲಿ) != 1) {
      failf(ದತ್ತಾಂಶ, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      EVP_PKEY_free(priv_ಕೀಲಿ);
      ಹಿಂತಿರುಗಿ 0;
    }
    EVP_PKEY_free(priv_ಕೀಲಿ); /* we do not need the handle any more... */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "crypto provider not set, cannot load private key");
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ 1;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)ಕೀಲಿ_file;
  failf(ದತ್ತಾಂಶ, "SSL_FILETYPE_PROVIDER not supported for private key");
  ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ engineload(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ರಚನೆ ossl_ctx *octx,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#ಆದರೆ defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  ಅಕ್ಷರ error_ಬಫರ್[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  ಆದರೆ(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_ಗಣ_engine(ದತ್ತಾಂಶ, octx, "pkcs11") != CURLE_OK)
    ಹಿಂತಿರುಗಿ 0;

  ಆದರೆ(octx->p.engine) {
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ cmd_name[] = "LOAD_CERT_CTRL";
    ರಚನೆ {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = ಖಾಲಿ;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    ಆದರೆ(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), ಖಾಲಿ)) {
      failf(ದತ್ತಾಂಶ, "SSL engine does not support loading certificates");
      ಹಿಂತಿರುಗಿ 0;
    }

    /* Load the certificate from the engine */
    ಆದರೆ(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, ಖಾಲಿ, 1)) {
      failf(ದತ್ತಾಂಶ, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(!params.cert) {
      failf(ದತ್ತಾಂಶ, "SSL engine did not initialize the certificate properly.");
      ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(ದತ್ತಾಂಶ, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      X509_free(params.cert);
      ಹಿಂತಿರುಗಿ 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "crypto engine not set, cannot load certificate");
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ 1;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)cert_file;
  failf(ದತ್ತಾಂಶ, "SSL_FILETYPE_ENGINE not supported for certificate");
  ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ providerload(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ ossl_ctx *octx,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cert_file)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_HAS_PROVIDERS
  ಅಕ್ಷರ error_ಬಫರ್[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  ಆದರೆ(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_ಗಣ_provider(ದತ್ತಾಂಶ, octx, "pkcs11") != CURLE_OK)
    ಹಿಂತಿರುಗಿ 0;

  ಆದರೆ(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = ಖಾಲಿ;
    X509 *cert = ಖಾಲಿ;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    ಪೂರ್ಣಾಂಕ rc;

    ಆದರೆ(!store) {
      failf(ದತ್ತಾಂಶ, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(ದತ್ತಾಂಶ, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
    }

    info = OSSL_STORE_ತುಂಬು(store);
    ಆದರೆ(info) {
      ಪೂರ್ಣಾಂಕ ossl_type = OSSL_STORE_INFO_get_type(info);

      ಆದರೆ(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    ಆದರೆ(!cert) {
      failf(ದತ್ತಾಂಶ, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      ಹಿಂತಿರುಗಿ 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    ಆದರೆ(rc != 1) {
      failf(ದತ್ತಾಂಶ, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      ಹಿಂತಿರುಗಿ 0;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "crypto provider not set, cannot load certificate");
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ 1;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)cert_file;
  failf(ದತ್ತಾಂಶ, "SSL_FILETYPE_PROVIDER not supported for certificate");
  ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pkcs12load(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      SSL_CTX* ctx,
                      ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *cert_blob,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cert_file,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_passwd)
{
  ಅಕ್ಷರ error_ಬಫರ್[256];
  BIO *cert_bio = ಖಾಲಿ;
  PKCS12 *p12 = ಖಾಲಿ;
  EVP_PKEY *pri;
  X509 *x509;
  ಪೂರ್ಣಾಂಕ cert_ಮುಗಿದ = 0;
  ರಾಶಿ_ದ(X509) *ca = ಖಾಲಿ;
  ಆದರೆ(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)cert_blob->len);
    ಆದರೆ(!cert_bio) {
      failf(ದತ್ತಾಂಶ, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      ಹಿಂತಿರುಗಿ 0;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    cert_bio = BIO_new(BIO_s_file());
    ಆದರೆ(!cert_bio) {
      failf(ದತ್ತಾಂಶ, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
      ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(ದತ್ತಾಂಶ, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      ಹಿಂತಿರುಗಿ 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, ಖಾಲಿ);
  BIO_free(cert_bio);

  ಆದರೆ(!p12) {
    failf(ದತ್ತಾಂಶ, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(!PKCS12_parse(p12, ಕೀಲಿ_passwd, &pri, &x509, &ca)) {
    failf(ದತ್ತಾಂಶ, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್)));
    PKCS12_free(p12);
    ಹಿಂತಿರುಗಿ 0;
  }

  PKCS12_free(p12);

  ಆದರೆ(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(ದತ್ತಾಂಶ, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್)));
    ಹೋಗು fail;
  }

  ಆದರೆ(SSL_CTX_use_ಖಾಸಗಿ_ಕೀಲಿ(ctx, pri) != 1) {
    failf(ದತ್ತಾಂಶ, "unable to use private key from PKCS12 file '%s'", cert_file);
    ಹೋಗು fail;
  }

  ಆದರೆ(!SSL_CTX_check_ಖಾಸಗಿ_ಕೀಲಿ(ctx)) {
    failf(ದತ್ತಾಂಶ, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    ಹೋಗು fail;
  }
  /* Set Certificate Verification chain */
  ಆದರೆ(ca) {
    ಆಗ(sk_X509_ಅಂಕೆ(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_ತೆಗೆ(ca);
      ಆದರೆ(!SSL_CTX_ಕೂಡಿಸು_client_CA(ctx, x)) {
        X509_free(x);
        failf(ದತ್ತಾಂಶ, "cannot add certificate to client CA list");
        ಹೋಗು fail;
      }
      ಆದರೆ(!SSL_CTX_ಕೂಡಿಸು_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(ದತ್ತಾಂಶ, "cannot add certificate to certificate chain");
        ಹೋಗು fail;
      }
    }
  }

  cert_ಮುಗಿದ = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
  sk_X509_ತೆಗೆ_free(ca, X509_free);
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!cert_ಮುಗಿದ)
    ಹಿಂತಿರುಗಿ 0; /* failure! */
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ CURLcode client_cert(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ossl_ctx *octx,
                            ಅಕ್ಷರ *cert_file,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *cert_blob,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cert_type,
                            ಅಕ್ಷರ *ಕೀಲಿ_file,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ಕೀಲಿ_blob,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_type,
                            ಅಕ್ಷರ *ಕೀಲಿ_passwd)
{
  ಅಕ್ಷರ error_ಬಫರ್[256];
  bool check_privkey = TRUE;
  ಪೂರ್ಣಾಂಕ file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  ಆದರೆ(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_ಮುಗಿದ = FALSE;
    ಪೂರ್ಣಾಂಕ cert_use_ಫಲಿತಾಂಶ;

    ಆದರೆ(ಕೀಲಿ_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_ಗಣ_default_passwd_cb_userdata(ctx, ಕೀಲಿ_passwd);
      /* Set passwd callback: */
      SSL_CTX_ಗಣ_default_passwd_cb(ctx, passwd_callback);
    }

    ಆಯ್ಕೆ(file_type) {
    ಸ್ಥಿತಿ SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_ಫಲಿತಾಂಶ = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, ಕೀಲಿ_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      ಆದರೆ(cert_use_ಫಲಿತಾಂಶ != 1) {
        failf(ದತ್ತಾಂಶ,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                            ಗಾತ್ರ(error_ಬಫರ್)));
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_ಫಲಿತಾಂಶ = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, ಕೀಲಿ_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      ಆದರೆ(cert_use_ಫಲಿತಾಂಶ != 1) {
        failf(ದತ್ತಾಂಶ,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                            ಗಾತ್ರ(error_ಬಫರ್)));
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSL_FILETYPE_ENGINE:
      ಆದರೆ(!cert_file || !engineload(ದತ್ತಾಂಶ, octx, cert_file))
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSL_FILETYPE_PROVIDER:
      ಆದರೆ(!cert_file || !providerload(ದತ್ತಾಂಶ, octx, cert_file))
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSL_FILETYPE_PKCS12:
      ಆದರೆ(!pkcs12load(ದತ್ತಾಂಶ, ctx, cert_blob, cert_file, ಕೀಲಿ_passwd))
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      pcks12_ಮುಗಿದ = TRUE;
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "not supported file type '%s' for certificate", cert_type);
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }

    ಆದರೆ(!ಕೀಲಿ_file && !ಕೀಲಿ_blob) {
      ಕೀಲಿ_file = cert_file;
      ಕೀಲಿ_blob = cert_blob;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      file_type = ossl_do_file_type(ಕೀಲಿ_type);

    ಆಯ್ಕೆ(file_type) {
    ಸ್ಥಿತಿ SSL_FILETYPE_PEM:
    ಸ್ಥಿತಿ SSL_FILETYPE_ASN1:
      cert_use_ಫಲಿತಾಂಶ = ಕೀಲಿ_blob ?
        use_privatekey_blob(ctx, ಕೀಲಿ_blob, file_type, ಕೀಲಿ_passwd) :
      SSL_CTX_use_ಖಾಸಗಿ_ಕೀಲಿ_file(ctx, ಕೀಲಿ_file, file_type);
      ಆದರೆ(cert_use_ಫಲಿತಾಂಶ != 1) {
        failf(ದತ್ತಾಂಶ, "unable to set private key file: '%s' type %s",
              ಕೀಲಿ_file ? ಕೀಲಿ_file : "(memory blob)",
              ಕೀಲಿ_type ? ಕೀಲಿ_type : "PEM");
        ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSL_FILETYPE_ENGINE:
      ಆದರೆ(!enginecheck(ದತ್ತಾಂಶ, octx, ಕೀಲಿ_file, ಕೀಲಿ_passwd))
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSL_FILETYPE_PROVIDER:
      ಆದರೆ(!providercheck(ದತ್ತಾಂಶ, octx, ಕೀಲಿ_file))
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SSL_FILETYPE_PKCS12:
      ಆದರೆ(!pcks12_ಮುಗಿದ) {
        failf(ದತ್ತಾಂಶ, "file type P12 for private key not supported");
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "not supported file type for private key");
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }

    ssl = SSL_new(ctx);
    ಆದರೆ(!ssl) {
      failf(ದತ್ತಾಂಶ, "unable to create an SSL structure");
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    x509 = SSL_get_certificate(ssl);

    ಆದರೆ(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#ಆದರೆ !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_ಕೀಲಿ = SSL_get_privatekey(ssl);
      ಆದರೆ(EVP_PKEY_id(priv_ಕೀಲಿ) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_ಕೀಲಿ);
        ಆದರೆ(RSA_flags(rsa) & RSA_ವಿಧಾನ_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#ಆದರೆ_ಅಂತ್ಯ

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    ಆದರೆ(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      ಆದರೆ(!SSL_CTX_check_ಖಾಸಗಿ_ಕೀಲಿ(ctx)) {
        failf(ದತ್ತಾಂಶ, "Private key does not match the certificate public key");
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
/* returns non-zero on failure */
ಸ್ಥಿರ CURLcode x509_name_oneline(ಸ್ಥಿರಾಂಕ X509_NAME *a, ರಚನೆ dynbuf *d)
{
  BIO *bio_ಹೊರಹೋಗು = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  ಪೂರ್ಣಾಂಕ rc;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(bio_ಹೊರಹೋಗು) {
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_ಹೊರಹೋಗು, a, 0, flags);
    ಆದರೆ(rc != -1) {
      BIO_get_mem_ptr(bio_ಹೊರಹೋಗು, &biomem);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(d, biomem->ದತ್ತಾಂಶ, biomem->ಉದ್ದಳತೆ);
    }
    BIO_free(bio_ಹೊರಹೋಗು);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ossl_init(ಶೂನ್ಯ)
{
  ಸ್ಥಿರಾಂಕ uint64_t flags =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_OPENSSL_AUTO_ತುಂಬು_CONFIG
    OPENSSL_INIT_NO_ತುಂಬು_CONFIG |
#ಇಲ್ಲದಿದ್ದರೆ
    OPENSSL_INIT_ತುಂಬು_CONFIG |
#ಆದರೆ_ಅಂತ್ಯ
    0;
  OPENSSL_init_ssl(flags, ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ 1;
}

/* Global cleanup */
ಸ್ಥಿರ ಶೂನ್ಯ ossl_cleanup(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#ಆದರೆ_ಅಂತ್ಯ
}

/* Selects an OpenSSL crypto engine or provider.
 */
ಸ್ಥಿರ CURLcode ossl_ಗಣ_engine(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ossl_ctx *octx,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL_ENGINE
  CURLcode ಫಲಿತಾಂಶ = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  ಆದರೆ(e) {

    ಆದರೆ(octx->p.engine) {
      ENGINE_ಮುಕ್ತಾಯ(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = ಖಾಲಿ;
    }
    ಆದರೆ(!ENGINE_init(e)) {
      ಅಕ್ಷರ buf[256];

      ENGINE_free(e);
      failf(ದತ್ತಾಂಶ, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, ಗಾತ್ರ(buf)));
      ಫಲಿತಾಂಶ = CURLE_SSL_ENGINE_INITFAILED;
      e = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಫಲಿತಾಂಶ = CURLE_OK;
    }
    octx->p.engine = e;
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_HAS_PROVIDERS
  ಹಿಂತಿರುಗಿ ossl_ಗಣ_provider(ದತ್ತಾಂಶ, octx, name);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)octx;
  (ಶೂನ್ಯ)name;
  failf(ದತ್ತಾಂಶ, "OpenSSL engine not found");
  ಹಿಂತಿರುಗಿ CURLE_SSL_ENGINE_NOTFOUND;
#ಆದರೆ_ಅಂತ್ಯ
}

/* Sets engine as default for all SSL operations
 */
ಸ್ಥಿರ CURLcode ossl_ಗಣ_engine_default(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ossl_ctx *octx)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL_ENGINE
  ಆದರೆ(octx->p.engine) {
    ಆದರೆ(ENGINE_ಗಣ_default(octx->p.engine, ENGINE_ವಿಧಾನ_ALL) > 0) {
      infof(ದತ್ತಾಂಶ, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      ಹಿಂತಿರುಗಿ CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)octx;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
ಸ್ಥಿರ ರಚನೆ curl_slist *ossl_engines_list(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_slist *list = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL_ENGINE
  ರಚನೆ curl_slist *beg;
  ENGINE *e;

  ಗಾಗಿ(e = ENGINE_get_ಮೊದಲ(); e; e = ENGINE_get_ಮುಂದಿನ(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    ಆದರೆ(!beg) {
      curl_slist_free_all(list);
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    list = beg;
  }
#ಆದರೆ_ಅಂತ್ಯ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ list;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_HAS_PROVIDERS

ಸ್ಥಿರ ಶೂನ್ಯ ossl_provider_cleanup(ರಚನೆ ossl_ctx *octx)
{
  ಆದರೆ(!octx)
    ಹಿಂತಿರುಗಿ;
  ಆದರೆ(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = ಖಾಲಿ;
  }
  ಆದರೆ(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = ಖಾಲಿ;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = ಖಾಲಿ;
  curlx_safefree(octx->p.propq);
  octx->p.provider_loaded = FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_PROVIDER_LEN 128 /* reasonable */

/* Selects an OpenSSL crypto provider.
 *
 * A provider might need an associated property, a string passed on to
 * OpenSSL. Specify this as [PROVIDER][:PROPERTY]: separate the name and the
 * property with a colon. No colon means no property is set.
 *
 * An example provider + property looks like "tpm2:?provider=tpm2".
 */
ಸ್ಥಿರ CURLcode ossl_ಗಣ_provider(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ossl_ctx *octx,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *iname)
{
  ಅಕ್ಷರ name[MAX_PROVIDER_LEN + 1];
  ರಚನೆ Curl_str prov;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *propq = ಖಾಲಿ;

  ಆದರೆ(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಆದರೆ(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  ಸ್ಮರಣೆ_ನಕಲು(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  ಆದರೆ(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    ಆದರೆ(!libctx)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಆದರೆ(propq) {
      octx->p.propq = curlx_strdup(propq);
      ಆದರೆ(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      }
    }
    octx->p.libctx = libctx;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_OPENSSL_AUTO_ತುಂಬು_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  ಆದರೆ(!OSSL_LIB_CTX_ತುಂಬು_config(octx->p.libctx, ಖಾಲಿ)) {
    infof(ದತ್ತಾಂಶ, "Failed to load default openssl config. Proceeding.");
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(OSSL_PROVIDER_ಲಭ್ಯ(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_ತುಂಬು(octx->p.libctx, name, 1);
  ಆದರೆ(!octx->p.provider) {
    ಅಕ್ಷರ error_ಬಫರ್[256];
    failf(ದತ್ತಾಂಶ, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್)));
    ossl_provider_cleanup(octx);
    ಹಿಂತಿರುಗಿ CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_ತುಂಬು(octx->p.libctx, "base", 1);
  ಆದರೆ(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(ದತ್ತಾಂಶ, "Failed to load base");
    ಹಿಂತಿರುಗಿ CURLE_SSL_ENGINE_NOTFOUND;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    octx->p.provider_loaded = TRUE;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಇಲ್ಲದಿದ್ದರೆ /* OPENSSL_HAS_PROVIDERS */
#ವ್ಯಾಖ್ಯಾನಿಸು ossl_provider_cleanup(x)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode ossl_shutdown(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              bool send_shutdown, bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಅಕ್ಷರ buf[1024];
  ಪೂರ್ಣಾಂಕ nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  ಆದರೆ(!octx->ssl || cf->shutdown) {
    *ಮುಗಿದ = TRUE;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *ಮುಗಿದ = FALSE;
  ಆದರೆ(!(SSL_get_shutdown(octx->ssl) & SSL_ಕಳುಹಿಸಿದ_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    ಗಾಗಿ(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf));
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown not sent, read -> %d", nread);
      ಆದರೆ(nread <= 0)
        ನಿಲ್ಲಿಸು;
    }
    err = SSL_get_error(octx->ssl, nread);
    ಆದರೆ(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool ಒಳಹರಿವು_pending;
      /* Yes, it did. */
      ಆದರೆ(!send_shutdown) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown received, not sending");
        *ಮುಗಿದ = TRUE;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!cf->ಮುಂದಿನ->cft->is_alive(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, &ಒಳಹರಿವು_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "peer closed connection");
        *ಮುಗಿದ = TRUE;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ಆದರೆ(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_ಕಳುಹಿಸಿದ_SHUTDOWN)) {
    ಪೂರ್ಣಾಂಕ rc;
    ERR_clear_error();
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    ಆದರೆ(rc == 1) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown finished");
      *ಮುಗಿದ = TRUE;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  ಗಾಗಿ(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf));
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown read -> %d", nread);
    ಆದರೆ(nread <= 0)
      ನಿಲ್ಲಿಸು;
  }
  err = SSL_get_error(octx->ssl, nread);
  ಆಯ್ಕೆ(err) {
  ಸ್ಥಿತಿ SSL_ERROR_ZERO_RETURN: /* no more data */
    ಆದರೆ(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown finished");
    ಇಲ್ಲದಿದ್ದರೆ
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown not received, but closed");
    *ಮುಗಿದ = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SSL_ERROR_NONE: /* did not get anything */
  ಸ್ಥಿತಿ SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sslerr = ERR_get_error());
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, ಗಾತ್ರ(buf)) :
                   SSL_ERROR_ಗಮ್ಯ_str(err)),
                  SOCKERRNO);
    }
    *ಮುಗಿದ = TRUE;
    ಫಲಿತಾಂಶ = CURLE_OK;
    ನಿಲ್ಲಿಸು;
  }

ಹೊರಹೋಗು:
  cf->shutdown = (ಫಲಿತಾಂಶ || *ಮುಗಿದ);
  ಆದರೆ(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->ಒಳಹರಿವು_pending = FALSE;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* also used by quic-tls */
ಶೂನ್ಯ Curl_osslctx_cleanup(ರಚನೆ ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  ಆದರೆ(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = ಖಾಲಿ;
  }
  ಆದರೆ(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = ಖಾಲಿ;
    octx->x509_store_ಸಿದ್ಧತೆ = FALSE;
  }
  ಆದರೆ(octx->bio_ವಿಧಾನ) {
    ossl_bio_cf_ವಿಧಾನ_free(octx->bio_ವಿಧಾನ);
    octx->bio_ವಿಧಾನ = ಖಾಲಿ;
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL_ENGINE
  ಆದರೆ(octx->p.engine) {
    ENGINE_ಮುಕ್ತಾಯ(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = ಖಾಲಿ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ossl_provider_cleanup(octx);
}

ಸ್ಥಿರ ಶೂನ್ಯ ossl_close(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
ಸ್ಥಿರ ಶೂನ್ಯ ossl_close_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
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
ಸ್ಥಿರ CURLcode ossl_verifyhost(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ connectdata *conn,
                                ರಚನೆ ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  ಪೂರ್ಣಾಂಕ ಗುರಿ; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  ರಾಶಿ_ದ(GENERAL_NAME) *altnames;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ರಚನೆ in6_addr addr;
#ಇಲ್ಲದಿದ್ದರೆ
  ರಚನೆ in_addr addr;
#ಆದರೆ_ಅಂತ್ಯ
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(peer->origin->hostname);

  (ಶೂನ್ಯ)conn;
  ಆಯ್ಕೆ(peer->type) {
  ಸ್ಥಿತಿ CURL_SSL_PEER_IPV4:
    ಆದರೆ(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
    ಗುರಿ = GEN_IPADD;
    addrlen = ಗಾತ್ರ(ರಚನೆ in_addr);
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಸ್ಥಿತಿ CURL_SSL_PEER_IPV6:
    ಆದರೆ(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
    ಗುರಿ = GEN_IPADD;
    addrlen = ಗಾತ್ರ(ರಚನೆ in6_addr);
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURL_SSL_PEER_DNS:
    ಗುರಿ = GEN_DNS;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUGASSERT(0);
    failf(ದತ್ತಾಂಶ, "unexpected SSL peer type: %d", (ಪೂರ್ಣಾಂಕ)peer->type);
    ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, ಖಾಲಿ, ಖಾಲಿ);

  ಆದರೆ(altnames) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#ಇಲ್ಲದಿದ್ದರೆ
    ಪೂರ್ಣಾಂಕ numalts;
    ಪೂರ್ಣಾಂಕ i;
#ಆದರೆ_ಅಂತ್ಯ

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_ಅಂಕೆ(altnames);

    /* loop through all alternatives - until a dnsmatch */
    ಗಾಗಿ(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ಸ್ಥಿರಾಂಕ GENERAL_NAME *check = sk_GENERAL_NAME_ಮೌಲ್ಯ(altnames, i);

      ಆದರೆ(check->type == GEN_DNS)
        dNSName = TRUE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      ಆದರೆ(check->type == ಗುರಿ) {
        /* get data and length */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *altptr = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ASN1_STRING_get0_ದತ್ತಾಂಶ(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_ಉದ್ದಳತೆ(check->d.ia5);

        ಆಯ್ಕೆ(ಗುರಿ) {
        ಸ್ಥಿತಿ GEN_DNS: /* name/pattern comparison */
          ಆದರೆ(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(ದತ್ತಾಂಶ, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (ಪೂರ್ಣಾಂಕ)altlen, altptr);
          }
          ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          ಆದರೆ((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(ದತ್ತಾಂಶ, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          ನಿಲ್ಲಿಸು;
        }
      }
    }
    GENERAL_ಹೆಸರುಗಳು_free(altnames);
  }

  ಆದರೆ(matched)
    /* an alternative name matched */
    ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dNSName || iPAddress) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(ದತ್ತಾಂಶ, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(ದತ್ತಾಂಶ, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    ಪೂರ್ಣಾಂಕ i = -1;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *cn = ಖಾಲಿ;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ಸ್ಥಿರಾಂಕ X509_NAME *name = X509_get_subject_name(server_cert);
    ಆದರೆ(name) {
      ಪೂರ್ಣಾಂಕ j;
      ಆಗ((j = X509_NAME_get_ಸೂಚ್ಯಂಕ_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    ಆದರೆ(i >= 0) {
      ಸ್ಥಿರಾಂಕ ASN1_STRING *tmp =
        X509_NAME_ನಮೂದು_get_ದತ್ತಾಂಶ(X509_NAME_get_ನಮೂದು(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      ಆದರೆ(tmp) {
        ಆದರೆ(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_ಉದ್ದಳತೆ(tmp);
          cn = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)CURL_UNCONST(ASN1_STRING_get0_ದತ್ತಾಂಶ(tmp));
        }
        ಇಲ್ಲದಿದ್ದರೆ { /* not a UTF8 name */
          ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ utf8len = ASN1_STRING_ಗಮ್ಯ_UTF8(&cn, tmp);
          ಆದರೆ(utf8len < 0)
            cnlen = 0;
          ಇಲ್ಲದಿದ್ದರೆ
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        ಆದರೆ((cnlen == 0) || !cn)
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(ದತ್ತಾಂಶ, "SSL: illegal cert name field");
          ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    ಆದರೆ(ಫಲಿತಾಂಶ)
      /* error already detected, pass through */
      ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!cn) {
      failf(ದತ್ತಾಂಶ, "SSL: unable to obtain common name from peer certificate");
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_cert_hostcheck((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(ದತ್ತಾಂಶ, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (ಪೂರ್ಣಾಂಕ)cnlen, cn,
            peer->origin->user_hostname);
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, " common name: %.*s (matched)", (ಪೂರ್ಣಾಂಕ)cnlen, cn);
    }
    ಆದರೆ(free_cn)
      OPENSSL_free(cn);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_NO_OCSP
ಸ್ಥಿರ CURLcode verifystatus(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ ossl_ctx *octx)
{
  ಪೂರ್ಣಾಂಕ i, ocsp_ಸ್ಥಿತಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BORINGSSL_LIKE
  ಸ್ಥಿರಾಂಕ uint8_t *ಸ್ಥಿತಿ_2;
#ಇಲ್ಲದಿದ್ದರೆ
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಸ್ಥಿತಿ_2;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *p;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  OCSP_RESPONSE *rsp = ಖಾಲಿ;
  OCSP_BASICRESP *br = ಖಾಲಿ;
  X509_STORE     *st = ಖಾಲಿ;
  ರಾಶಿ_ದ(X509) *ch = ಖಾಲಿ;
  X509 *cert;
  OCSP_CERTID *id = ಖಾಲಿ;
  ಪೂರ್ಣಾಂಕ cert_ಸ್ಥಿತಿ, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  ಪೂರ್ಣಾಂಕ ret;
  ದೀರ್ಘ len;

  (ಶೂನ್ಯ)cf;
  DEBUGASSERT(octx);

  len = (ದೀರ್ಘ)SSL_get_tlsext_ಸ್ಥಿತಿ_ocsp_resp(octx->ssl, &ಸ್ಥಿತಿ_2);

  ಆದರೆ(!ಸ್ಥಿತಿ_2) {
    failf(ದತ್ತಾಂಶ, "No OCSP response received");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }
  p = ಸ್ಥಿತಿ_2;
  rsp = d2i_OCSP_RESPONSE(ಖಾಲಿ, &p, len);
  ಆದರೆ(!rsp) {
    failf(ದತ್ತಾಂಶ, "Invalid OCSP response");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }

  ocsp_ಸ್ಥಿತಿ = OCSP_response_ಸ್ಥಿತಿ(rsp);
  ಆದರೆ(ocsp_ಸ್ಥಿತಿ != OCSP_RESPONSE_ಸ್ಥಿತಿ_SUCCESSFUL) {
    failf(ದತ್ತಾಂಶ, "Invalid OCSP response status: %s (%d)",
          OCSP_response_ಸ್ಥಿತಿ_str(ocsp_ಸ್ಥಿತಿ), ocsp_ಸ್ಥಿತಿ);
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }

  br = OCSP_response_get1_basic(rsp);
  ಆದರೆ(!br) {
    failf(ದತ್ತಾಂಶ, "Invalid OCSP response");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }

#ಆದರೆ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ) && LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#ಇಲ್ಲದಿದ್ದರೆ
  ch = SSL_get_peer_cert_chain(octx->ssl);
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ch) {
    failf(ದತ್ತಾಂಶ, "Could not get peer certificate chain");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  ಆದರೆ(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(ದತ್ತಾಂಶ, "OCSP response verification failed");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  ಆದರೆ(!cert) {
    failf(ದತ್ತಾಂಶ, "Error getting peer certificate");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }

  ಗಾಗಿ(i = 0; i < (ಪೂರ್ಣಾಂಕ)sk_X509_ಅಂಕೆ(ch); i++) {
    X509 *issuer = sk_X509_ಮೌಲ್ಯ(ch, (ossl_valsize_t)i);
    ಆದರೆ(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_ಗಮ್ಯ_id(EVP_sha1(), cert, issuer);
      ನಿಲ್ಲಿಸು;
    }
  }
  X509_free(cert);

  ಆದರೆ(!id) {
    failf(ದತ್ತಾಂಶ, "Error computing OCSP ID");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_ಕಂಡುಹಿಡಿ_ಸ್ಥಿತಿ(br, id, &cert_ಸ್ಥಿತಿ, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  ಆದರೆ(ret != 1) {
    failf(ದತ್ತಾಂಶ, "Could not find certificate ID in OCSP response");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  ಆದರೆ(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(ದತ್ತಾಂಶ, "OCSP response has expired");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }

  infof(ದತ್ತಾಂಶ, "SSL certificate status: %s (%d)",
        OCSP_cert_ಸ್ಥಿತಿ_str(cert_ಸ್ಥಿತಿ), cert_ಸ್ಥಿತಿ);

  ಆಯ್ಕೆ(cert_ಸ್ಥಿತಿ) {
  ಸ್ಥಿತಿ V_OCSP_CERTSTATUS_GOOD:
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ V_OCSP_CERTSTATUS_REVOKED:
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    failf(ದತ್ತಾಂಶ, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    ಹೋಗು ಕೊನೆ;

  ಸ್ಥಿತಿ V_OCSP_CERTSTATUS_UNKNOWN:
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಕೊನೆ;
  }

ಕೊನೆ:
  ಆದರೆ(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_ಸಂದೇಶ_type(ಪೂರ್ಣಾಂಕ ssl_ver, ಪೂರ್ಣಾಂಕ ಸಂದೇಶ)
{
  ಆದರೆ(ssl_ver == SSL3_ಆವೃತ್ತಿ_MAJOR) {
    ಆಯ್ಕೆ(ಸಂದೇಶ) {
    ಸ್ಥಿತಿ SSL3_MT_HELLO_REQUEST:
      ಹಿಂತಿರುಗಿ "Hello request";
    ಸ್ಥಿತಿ SSL3_MT_CLIENT_HELLO:
      ಹಿಂತಿರುಗಿ "Client hello";
    ಸ್ಥಿತಿ SSL3_MT_SERVER_HELLO:
      ಹಿಂತಿರುಗಿ "Server hello";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_MT_NEWSESSION_TICKET
    ಸ್ಥಿತಿ SSL3_MT_NEWSESSION_TICKET:
      ಹಿಂತಿರುಗಿ "Newsession Ticket";
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ SSL3_MT_CERTIFICATE:
      ಹಿಂತಿರುಗಿ "Certificate";
    ಸ್ಥಿತಿ SSL3_MT_SERVER_ಕೀಲಿ_EXCHANGE:
      ಹಿಂತಿರುಗಿ "Server key exchange";
    ಸ್ಥಿತಿ SSL3_MT_CLIENT_ಕೀಲಿ_EXCHANGE:
      ಹಿಂತಿರುಗಿ "Client key exchange";
    ಸ್ಥಿತಿ SSL3_MT_CERTIFICATE_REQUEST:
      ಹಿಂತಿರುಗಿ "Request CERT";
    ಸ್ಥಿತಿ SSL3_MT_SERVER_ಮುಗಿದ:
      ಹಿಂತಿರುಗಿ "Server finished";
    ಸ್ಥಿತಿ SSL3_MT_CERTIFICATE_VERIFY:
      ಹಿಂತಿರುಗಿ "CERT verify";
    ಸ್ಥಿತಿ SSL3_MT_ಪೂರ್ಣಗೊಂಡ:
      ಹಿಂತಿರುಗಿ "Finished";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_MT_CERTIFICATE_ಸ್ಥಿತಿ
    ಸ್ಥಿತಿ SSL3_MT_CERTIFICATE_ಸ್ಥಿತಿ:
      ಹಿಂತಿರುಗಿ "Certificate Status";
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_MT_ENCRYPTED_EXTENSIONS
    ಸ್ಥಿತಿ SSL3_MT_ENCRYPTED_EXTENSIONS:
      ಹಿಂತಿರುಗಿ "Encrypted Extensions";
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_MT_SUPPLEMENTAL_ದತ್ತಾಂಶ
    ಸ್ಥಿತಿ SSL3_MT_SUPPLEMENTAL_ದತ್ತಾಂಶ:
      ಹಿಂತಿರುಗಿ "Supplemental data";
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_MT_ಕೊನೆ_ದ_EARLY_ದತ್ತಾಂಶ
    ಸ್ಥಿತಿ SSL3_MT_ಕೊನೆ_ದ_EARLY_ದತ್ತಾಂಶ:
      ಹಿಂತಿರುಗಿ "End of early data";
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_MT_ಕೀಲಿ_UPDATE
    ಸ್ಥಿತಿ SSL3_MT_ಕೀಲಿ_UPDATE:
      ಹಿಂತಿರುಗಿ "Key update";
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_MT_ಮುಂದಿನ_PROTO
    ಸ್ಥಿತಿ SSL3_MT_ಮುಂದಿನ_PROTO:
      ಹಿಂತಿರುಗಿ "Next protocol";
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_MT_ಸಂದೇಶ_HASH
    ಸ್ಥಿತಿ SSL3_MT_ಸಂದೇಶ_HASH:
      ಹಿಂತಿರುಗಿ "Message hash";
#ಆದರೆ_ಅಂತ್ಯ
    }
  }
  ಹಿಂತಿರುಗಿ "Unknown";
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tls_rt_type(ಪೂರ್ಣಾಂಕ type)
{
  ಆಯ್ಕೆ(type) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_RT_ಶೀರ್ಷಿಕೆ
  ಸ್ಥಿತಿ SSL3_RT_ಶೀರ್ಷಿಕೆ:
    ಹಿಂತಿರುಗಿ "TLS header";
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ SSL3_RT_CHANGE_CIPHER_SPEC:
    ಹಿಂತಿರುಗಿ "TLS change cipher";
  ಸ್ಥಿತಿ SSL3_RT_ALERT:
    ಹಿಂತಿರುಗಿ "TLS alert";
  ಸ್ಥಿತಿ SSL3_RT_HANDSHAKE:
    ಹಿಂತಿರುಗಿ "TLS handshake";
  ಸ್ಥಿತಿ SSL3_RT_APPLICATION_ದತ್ತಾಂಶ:
    ಹಿಂತಿರುಗಿ "TLS app data";
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ossl_trace(ಪೂರ್ಣಾಂಕ direction, ಪೂರ್ಣಾಂಕ ssl_ver, ಪೂರ್ಣಾಂಕ content_type,
                       ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t len, SSL *ssl,
                       ಶೂನ್ಯ *userp)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *verstr;
  ರಚನೆ Curl_cfilter *cf = userp;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = ಖಾಲಿ;
  ಅಕ್ಷರ unknown[32];

  ಆದರೆ(!cf)
    ಹಿಂತಿರುಗಿ;
  ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  ಆದರೆ(!ದತ್ತಾಂಶ || !ದತ್ತಾಂಶ->ಗಣ.fdebug || (direction && direction != 1))
    ಹಿಂತಿರುಗಿ;

  ಆಯ್ಕೆ(ssl_ver) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_ಆವೃತ್ತಿ
  ಸ್ಥಿತಿ SSL3_ಆವೃತ್ತಿ:
    verstr = "SSLv3";
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ TLS1_ಆವೃತ್ತಿ:
    verstr = "TLSv1.0";
    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TLS1_1_ಆವೃತ್ತಿ
  ಸ್ಥಿತಿ TLS1_1_ಆವೃತ್ತಿ:
    verstr = "TLSv1.1";
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TLS1_2_ಆವೃತ್ತಿ
  ಸ್ಥಿತಿ TLS1_2_ಆವೃತ್ತಿ:
    verstr = "TLSv1.2";
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ TLS1_3_ಆವೃತ್ತಿ:
    verstr = "TLSv1.3";
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    curl_msnprintf(unknown, ಗಾತ್ರ(unknown), "(%x)", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ssl_ver);
    verstr = unknown;
    ನಿಲ್ಲಿಸು;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  ಆದರೆ(ssl_ver
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_RT_ಶೀರ್ಷಿಕೆ
     && content_type != SSL3_RT_ಶೀರ್ಷಿಕೆ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#ಆದರೆ_ಅಂತ್ಯ
    ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ_name = "Truncated message";
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tls_rt_name;
    ಅಕ್ಷರ ssl_buf[1024];
    ಪೂರ್ಣಾಂಕ ಸಂದೇಶ_type = 0;
    ಪೂರ್ಣಾಂಕ txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    ಆದರೆ(ssl_ver == SSL3_ಆವೃತ್ತಿ_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    ಇಲ್ಲದಿದ್ದರೆ
      tls_rt_name = "";

    ಆದರೆ(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      ಆದರೆ(len) {
        ಸಂದೇಶ_type = *(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf;
        ಸಂದೇಶ_name = "Change cipher spec";
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(content_type == SSL3_RT_ALERT) {
      ಆದರೆ(len >= 2) {
        ಸಂದೇಶ_type =
          (((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf)[0] << 8) +
           ((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf)[1];
        ಸಂದೇಶ_name = SSL_alert_desc_string_long(ಸಂದೇಶ_type);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len) {
      ಸಂದೇಶ_type = *(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf;
      ಸಂದೇಶ_name = ssl_ಸಂದೇಶ_type(ssl_ver, ಸಂದೇಶ_type);
    }

    txt_len = curl_msnprintf(ssl_buf, ಗಾತ್ರ(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, ಸಂದೇಶ_name, ಸಂದೇಶ_type);
    Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ಪಠ್ಯ, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(ದತ್ತಾಂಶ, (direction == 1) ? CURLINFO_SSL_ದತ್ತಾಂಶ_ಹೊರಹೋಗು :
             CURLINFO_SSL_ದತ್ತಾಂಶ_ಒಳ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf, len);
  (ಶೂನ್ಯ)ssl;
}

ಸ್ಥಿರ CURLcode ossl_ಗಣ_ssl_ಆವೃತ್ತಿ_min_max(ರಚನೆ Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ssl_ಆವೃತ್ತಿ_min)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  ದೀರ್ಘ curl_ssl_ಆವೃತ್ತಿ_min = (ದೀರ್ಘ)ssl_ಆವೃತ್ತಿ_min;
  ದೀರ್ಘ curl_ssl_ಆವೃತ್ತಿ_max;

  /* convert curl min SSL version option to OpenSSL constant */
#ಆದರೆ defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ)
  uint16_t ossl_ssl_ಆವೃತ್ತಿ_min = 0;
  uint16_t ossl_ssl_ಆವೃತ್ತಿ_max = 0;
#ಇಲ್ಲದಿದ್ದರೆ
  ದೀರ್ಘ ossl_ssl_ಆವೃತ್ತಿ_min = 0;
  ದೀರ್ಘ ossl_ssl_ಆವೃತ್ತಿ_max = 0;
#ಆದರೆ_ಅಂತ್ಯ
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_ಆವೃತ್ತಿ_min != CURL_SSLVERSION_DEFAULT);
  ಆಯ್ಕೆ(curl_ssl_ಆವೃತ್ತಿ_min) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_ಆವೃತ್ತಿ_min = TLS1_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_ಆವೃತ್ತಿ_min = TLS1_1_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_ಆವೃತ್ತಿ_min = TLS1_2_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_ಆವೃತ್ತಿ_min = TLS1_3_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;
  }

  /* ... then, TLS max version */
  curl_ssl_ಆವೃತ್ತಿ_max = (ದೀರ್ಘ)conn_config->ಆವೃತ್ತಿ_max;

  /* convert curl max SSL version option to OpenSSL constant */
  ಆಯ್ಕೆ(curl_ssl_ಆವೃತ್ತಿ_max) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_ಆವೃತ್ತಿ_max = TLS1_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_ಆವೃತ್ತಿ_max = TLS1_1_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_ಆವೃತ್ತಿ_max = TLS1_2_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_ಆವೃತ್ತಿ_max = TLS1_3_ಆವೃತ್ತಿ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_NONE:  /* none selected */
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  ಪೂರ್ವನಿಯೋಜಿತ:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_ಆವೃತ್ತಿ_max = 0;
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(!SSL_CTX_ಗಣ_min_proto_ಆವೃತ್ತಿ(ctx, ossl_ssl_ಆವೃತ್ತಿ_min) ||
     !SSL_CTX_ಗಣ_max_proto_ಆವೃತ್ತಿ(ctx, ossl_ssl_ಆವೃತ್ತಿ_max))
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_ossl_ಕೂಡಿಸು_session(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ossl_ctx *octx,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                               SSL_SESSION *session,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn,
                               ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *quic_tp,
                               size_t quic_tp_len,
                               ರಚನೆ Curl_ssl_session **psession)
{
  ರಚನೆ Curl_ssl_session *sc_session = ಖಾಲಿ, *sc_dup = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *der_session_buf = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *qtp_clone = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(psession)
    *psession = ಖಾಲಿ;
  ಆದರೆ(!cf || !ದತ್ತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ)) {
    size_t der_session_ಗಾತ್ರ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *der_session_ptr;
    size_t earlydata_max = 0;
    ಪೂರ್ಣಾಂಕ ietf_tls_id = SSL_ಆವೃತ್ತಿ(octx->ssl);

    der_session_ಗಾತ್ರ = i2d_SSL_SESSION(session, ಖಾಲಿ);
    ಆದರೆ(der_session_ಗಾತ್ರ == 0) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_ಗಾತ್ರ);
    ಆದರೆ(!der_session_buf) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    der_session_ಗಾತ್ರ = i2d_SSL_SESSION(session, &der_session_ptr);
    ಆದರೆ(der_session_ಗಾತ್ರ == 0) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_ದತ್ತಾಂಶ(session);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)quic_tp, quic_tp_len);
      ಆದರೆ(!qtp_clone) {
        ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }

    ಫಲಿತಾಂಶ = Curl_ssl_session_create2(der_session_buf, der_session_ಗಾತ್ರ,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)ಸಮಯ(ಖಾಲಿ) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = ಖಾಲಿ;  /* took ownership of sdata */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      sc_session->sectrust_verified = octx->sectrust_verified;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(!ಫಲಿತಾಂಶ && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        ಫಲಿತಾಂಶ = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಫಲಿತಾಂಶ = Curl_ssl_scache_put(cf, ದತ್ತಾಂಶ, ssl_peer_ಕೀಲಿ, sc_session);
      /* took ownership of `sc_session` */
      sc_session = ಖಾಲಿ;
    }
  }

ಹೊರಹೋಗು:
  curlx_free(der_session_buf);
  ಆದರೆ(!ಫಲಿತಾಂಶ && psession) {
    *psession = sc_dup;
    sc_dup = ಖಾಲಿ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ರಚನೆ Curl_cfilter *cf = (ರಚನೆ Curl_cfilter *)SSL_get_app_ದತ್ತಾಂಶ(ssl);
  ಆದರೆ(cf) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
    ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
    ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
    ರಚನೆ Curl_ssl_session *session = ಖಾಲಿ;
    Curl_ossl_ಕೂಡಿಸು_session(cf, ದತ್ತಾಂಶ, octx, connssl->peer.scache_ಕೀಲಿ,
                          ssl_sessionid, connssl->negotiated.alpn, ಖಾಲಿ,
                          0, &session);
    ಆದರೆ(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ CURLcode ತುಂಬು_cacert_from_ಸ್ಮರಣೆ(X509_STORE *store,
                                        ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = ಖಾಲಿ;
  ರಾಶಿ_ದ(X509_INFO) *inf = ಖಾಲಿ;

  /* everything else is a reference */
  ಪೂರ್ಣಾಂಕ i, ಎಣಿಕೆ_2 = 0;
  X509_INFO *itmp = ಖಾಲಿ;

  ಆದರೆ(ca_info_blob->len > (size_t)ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
    ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)ca_info_blob->len);
  ಆದರೆ(!cbio)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  inf = PEM_X509_INFO_read_bio(cbio, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
  ಆದರೆ(!inf) {
    BIO_free(cbio);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  ಗಾಗಿ(i = 0; i < (ಪೂರ್ಣಾಂಕ)sk_X509_INFO_ಅಂಕೆ(inf); ++i) {
    itmp = sk_X509_INFO_ಮೌಲ್ಯ(inf, (ossl_valsize_t)i);
    ಆದರೆ(itmp->x509) {
      ಆದರೆ(X509_STORE_ಕೂಡಿಸು_cert(store, itmp->x509)) {
        ++ಎಣಿಕೆ_2;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* set count to 0 to return an error */
        ಎಣಿಕೆ_2 = 0;
        ನಿಲ್ಲಿಸು;
      }
    }
    ಆದರೆ(itmp->crl) {
      ಆದರೆ(X509_STORE_ಕೂಡಿಸು_crl(store, itmp->crl)) {
        ++ಎಣಿಕೆ_2;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* set count to 0 to return an error */
        ಎಣಿಕೆ_2 = 0;
        ನಿಲ್ಲಿಸು;
      }
    }
  }

#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತಳ್ಳು
#ಸೂಚನೆ clang diagnostic ignored "-Wcast-function-type-strict"
#ಆದರೆ_ಅಂತ್ಯ
  sk_X509_INFO_ತೆಗೆ_free(inf, X509_INFO_free);
#ಆದರೆ defined(__clang__) && __clang_major__ >= 16
#ಸೂಚನೆ clang diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  ಹಿಂತಿರುಗಿ (ಎಣಿಕೆ_2 > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_CRYPTO
ಸ್ಥಿರ CURLcode ossl_win_ತುಂಬು_store(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ Curl_cfilter *cf,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  ಆದರೆ(hStore) {
    PCCERT_CONTEXT pContext = ಖಾಲಿ;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = ಖಾಲಿ;
    DWORD enhkey_usage_ಗಾತ್ರ = 0;
    VERBOSE(size_t ಒಟ್ಟು = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    ಫಲಿತಾಂಶ = CURLE_OK;
    ಗಾಗಿ(;;) {
      X509 *x509;
      FILETIME now;
      BYTE ಕೀಲಿ_usage[2];
      DWORD req_ಗಾತ್ರ;
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *encoded_cert;
      pContext = Cert_Enum_Certificates_ಒಳ_Store(hStore, pContext);
      ಆದರೆ(!pContext)
        ನಿಲ್ಲಿಸು;

      VERBOSE(++ಒಟ್ಟು);

#ಆದರೆ defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        ಅಕ್ಷರ cert_name[256];
        ಆದರೆ(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               ಖಾಲಿ, cert_name, ಗಾತ್ರ(cert_name)))
          infof(ದತ್ತಾಂಶ, "SSL: unknown cert name");
        ಇಲ್ಲದಿದ್ದರೆ
          infof(ದತ್ತಾಂಶ, "SSL: Checking cert \"%s\"", cert_name);
      }
#ಆದರೆ_ಅಂತ್ಯ
      encoded_cert = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)pContext->pbCertEncoded;
      ಆದರೆ(!encoded_cert)
        ಮುಂದುವರಿ;

      Get_System_ಸಮಯ_As_File_ಸಮಯ(&now);
      ಆದರೆ(Compare_File_ಸಮಯ(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         Compare_File_ಸಮಯ(&now, &pContext->pCertInfo->NotAfter) > 0)
        ಮುಂದುವರಿ;

      /* If key usage exists check for signing attribute */
      ಆದರೆ(Cert_Get_Intended_ಕೀಲಿ_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 ಕೀಲಿ_usage, ಗಾತ್ರ(ಕೀಲಿ_usage))) {
        ಆದರೆ(!(ಕೀಲಿ_usage[0] & CERT_ಕೀಲಿ_CERT_SIGN_ಕೀಲಿ_USAGE))
          ಮುಂದುವರಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Get_ಕೊನೆಯ_Error())
        ಮುಂದುವರಿ;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      ಆದರೆ(Cert_Get_Enhanced_ಕೀಲಿ_Usage(pContext, 0, ಖಾಲಿ, &req_ಗಾತ್ರ) && req_ಗಾತ್ರ) {
        ಆದರೆ(req_ಗಾತ್ರ > enhkey_usage_ಗಾತ್ರ) {
          ಶೂನ್ಯ *tmp = curlx_realloc(enhkey_usage, req_ಗಾತ್ರ);

          ಆದರೆ(!tmp) {
            failf(ದತ್ತಾಂಶ, "SSL: Out of memory allocating for OID list");
            ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
            ನಿಲ್ಲಿಸು;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_ಗಾತ್ರ = req_ಗಾತ್ರ;
        }

        ಆದರೆ(Cert_Get_Enhanced_ಕೀಲಿ_Usage(pContext, 0, enhkey_usage, &req_ಗಾತ್ರ)) {
          ಆದರೆ(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            ಆದರೆ((HRESULT)Get_ಕೊನೆಯ_Error() != CRYPT_E_NOT_ಸಿಕ್ಕಿತು)
              ಮುಂದುವರಿ;
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            DWORD i;
            bool ಸಿಕ್ಕಿತು = FALSE;

            ಗಾಗಿ(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              ಆದರೆ(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                ಸಿಕ್ಕಿತು = TRUE;
                ನಿಲ್ಲಿಸು;
              }
            }

            ಆದರೆ(!ಸಿಕ್ಕಿತು)
              ಮುಂದುವರಿ;
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಮುಂದುವರಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಮುಂದುವರಿ;

      x509 = d2i_X509(ಖಾಲಿ, &encoded_cert, (ದೀರ್ಘ)pContext->cbCertEncoded);
      ಆದರೆ(!x509)
        ಮುಂದುವರಿ;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      ಆದರೆ(X509_STORE_ಕೂಡಿಸು_cert(store, x509) == 1) {
        VERBOSE(++imported);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
        infof(ದತ್ತಾಂಶ, "SSL: Imported cert");
#ಆದರೆ_ಅಂತ್ಯ
        *padded = TRUE;
      }
      X509_free(x509);
    }

    curlx_free(enhkey_usage);
    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    CURL_TRC_CF(ದತ್ತಾಂಶ, cf,
                "ossl_win_load_store() found: %zu imported: %zu certs in %s.",
                ಒಟ್ಟು, imported, win_store);

    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ossl_windows_ತುಂಬು_anchors(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *padded = FALSE;
  ಗಾಗಿ(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    ಫಲಿತಾಂಶ = ossl_win_ತುಂಬು_store(ದತ್ತಾಂಶ, cf, win_stores[i], store, &store_added);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(store_added) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_WIN32_CRYPTO */

ಸ್ಥಿರ CURLcode ossl_ತುಂಬು_trust_anchors(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ ossl_ctx *octx,
                                        X509_STORE *store)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? ಖಾಲಿ : conn_config->CAfile);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_ಖಾಲಿಗಳು = TRUE;
  ಆದರೆ(conn_config->native_ca_store) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_CRYPTO
    bool added = FALSE;
    ಫಲಿತಾಂಶ = ossl_windows_ತುಂಬು_anchors(cf, ದತ್ತಾಂಶ, store, &added);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(added) {
      infof(ದತ್ತಾಂಶ, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_ಖಾಲಿಗಳು = FALSE;
    }
#ಅಥವಾಆದರೆ defined(USE_APPLE_SECTRUST)
    infof(ದತ್ತಾಂಶ, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆದರೆ(conn_config->ca_info_blob) {
    ಫಲಿತಾಂಶ = ತುಂಬು_cacert_from_ಸ್ಮರಣೆ(store, conn_config->ca_info_blob);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "error adding trust anchors from certificate blob: %d",
            (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    infof(ದತ್ತಾಂಶ, "  CA Blob from configuration");
    octx->store_is_ಖಾಲಿಗಳು = FALSE;
  }

  ಆದರೆ(ssl_cafile || ssl_capath) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    ಆದರೆ(ssl_cafile) {
      ಆದರೆ(!X509_STORE_ತುಂಬು_file(store, ssl_cafile)) {
        ಆದರೆ(octx->store_is_ಖಾಲಿಗಳು && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(ದತ್ತಾಂಶ, "error adding trust anchors from file: %s", ssl_cafile);
          ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          infof(ದತ್ತಾಂಶ, "error setting certificate file, continuing anyway");
      }
      infof(ದತ್ತಾಂಶ, "  CAfile: %s", ssl_cafile);
      octx->store_is_ಖಾಲಿಗಳು = FALSE;
    }
    ಆದರೆ(ssl_capath) {
      ಆದರೆ(!X509_STORE_ತುಂಬು_ಮಾರ್ಗ(store, ssl_capath)) {
        ಆದರೆ(octx->store_is_ಖಾಲಿಗಳು && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(ದತ್ತಾಂಶ, "error adding trust anchors from path: %s", ssl_capath);
          ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          infof(ದತ್ತಾಂಶ, "error setting certificate path, continuing anyway");
      }
      infof(ದತ್ತಾಂಶ, "  CApath: %s", ssl_capath);
      octx->store_is_ಖಾಲಿಗಳು = FALSE;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    ಆದರೆ(!X509_STORE_ತುಂಬು_locations(store, ssl_cafile, ssl_capath)) {
      ಆದರೆ(octx->store_is_ಖಾಲಿಗಳು && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(ದತ್ತಾಂಶ, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        infof(ದತ್ತಾಂಶ, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    ಆದರೆ(ssl_cafile)
      infof(ದತ್ತಾಂಶ, "  CAfile: %s", ssl_cafile);
    ಆದರೆ(ssl_capath)
      infof(ದತ್ತಾಂಶ, "  CApath: %s", ssl_capath);
    octx->store_is_ಖಾಲಿಗಳು = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_CA_FALLBACK
  ಆದರೆ(octx->store_is_ಖಾಲಿಗಳು) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_ಗಣ_default_paths(store);
    infof(ದತ್ತಾಂಶ, "  OpenSSL default paths (fallback)");
    octx->store_is_ಖಾಲಿಗಳು = FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(octx->store_is_ಖಾಲಿಗಳು && !have_native_check)
    infof(ದತ್ತಾಂಶ, "  no trust anchors configured");

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ossl_populate_x509_store(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         ರಚನೆ ossl_ctx *octx,
                                         X509_STORE *store)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  X509_LOOKUP *lookup = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_crlfile = conn_config->CRLfile;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ x509flags = 0;

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "configuring OpenSSL's x509 trust store");
  ಆದರೆ(!store)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(!conn_config->verifypeer) {
    infof(ದತ್ತಾಂಶ, "SSL Trust: peer verification disabled");
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  infof(ದತ್ತಾಂಶ, "SSL Trust Anchors:");
  ಫಲಿತಾಂಶ = ossl_ತುಂಬು_trust_anchors(cf, ದತ್ತಾಂಶ, octx, store);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* Does not make sense to load a CRL file without peer verification */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
  ಆದರೆ(ssl_crlfile && conn_config->native_ca_store) {
    failf(ದತ್ತಾಂಶ, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_ಕೂಡಿಸು_lookup(store, X509_LOOKUP_file());
    ಆದರೆ(!lookup ||
       (!X509_ತುಂಬು_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(ದತ್ತಾಂಶ, "error loading CRL file: %s", ssl_crlfile);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CRL_BADFILE;
    }
    x509flags = X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL;
    infof(ದತ್ತಾಂಶ, " CRLfile: %s", ssl_crlfile);
  }

  /* Try building a chain using issuers in the trusted store first to avoid
     problems with server-sent legacy intermediates. Newer versions of
     OpenSSL do alternate chain checking by default but we do not know how to
     determine that in a reliable manner.
     https://web.archive.org/web/20190422050538/rt.openssl.org/Ticket/Display.html?id=3621
   */
  x509flags |= X509_V_FLAG_TRUSTED_ಮೊದಲ;

  ಆದರೆ(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (ಶೂನ್ಯ)X509_STORE_ಗಣ_flags(store, x509flags);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* key to use at `multi->proto_hash` */
#ವ್ಯಾಖ್ಯಾನಿಸು MPROTO_OSSL_X509_ಕೀಲಿ  "tls:ossl:x509:share"

ರಚನೆ ossl_x509_share {
  ಅಕ್ಷರ *CAfile;         /* CAfile path used to generate X509 store */
  ಅಕ್ಷರ *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  ರಚನೆ curltime ಸಮಯ; /* when the cached store was created */
  ಬಿಟ್(store_is_ಖಾಲಿಗಳು);  /* no certs/paths/blobs are in the store */
  ಬಿಟ್(no_partialchain); /* keep partial chain state */
};

ಸ್ಥಿರ ಶೂನ್ಯ oss_x509_share_free(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p)
{
  ರಚನೆ ossl_x509_share *share = p;
  DEBUGASSERT(ಕೀಲಿ_len == CURL_CSTRLEN(MPROTO_OSSL_X509_ಕೀಲಿ));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_ಕೀಲಿ, ಕೀಲಿ, ಕೀಲಿ_len));
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)ಕೀಲಿ_len;
  ಆದರೆ(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

ಸ್ಥಿರ bool ossl_cached_x509_store_expired(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ಸ್ಥಿರಾಂಕ ರಚನೆ ossl_x509_share *mb)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl_ca_cache_timeout < 0)
    ಹಿಂತಿರುಗಿ FALSE;
  ಇಲ್ಲದಿದ್ದರೆ {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &mb->ಸಮಯ);
    timediff_t timeout_ms = ದತ್ತಾಂಶ->ಗಣ.ssl_ca_cache_timeout * (timediff_t)1000;

    ಹಿಂತಿರುಗಿ elapsed_ms >= timeout_ms;
  }
}

ಸ್ಥಿರ bool ossl_cached_x509_store_different(ರಚನೆ Curl_cfilter *cf,
                                             ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                             ಸ್ಥಿರಾಂಕ ರಚನೆ ossl_x509_share *mb)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆದರೆ(mb->no_partialchain != conn_config->no_partialchain)
    ಹಿಂತಿರುಗಿ TRUE;
  ಆದರೆ(!mb->CAfile || !conn_config->CAfile) {
    ಆದರೆ(mb->CAfile != conn_config->CAfile)
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strcmp(mb->CAfile, conn_config->CAfile))
    ಹಿಂತಿರುಗಿ TRUE;

  ಆದರೆ(!mb->CApath || !conn_config->CApath) {
    ಆದರೆ(mb->CApath != conn_config->CApath)
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strcmp(mb->CApath, conn_config->CApath))
    ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ X509_STORE *ossl_get_cached_x509_store(ರಚನೆ Curl_cfilter *cf,
                                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                              bool *pempty)
{
  ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;
  ರಚನೆ ossl_x509_share *share;
  X509_STORE *store = ಖಾಲಿ;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_ಕೀಲಿ,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_ಕೀಲಿ)) : ಖಾಲಿ;
  ಆದರೆ(share && share->store &&
     !ossl_cached_x509_store_expired(ದತ್ತಾಂಶ, share) &&
     !ossl_cached_x509_store_different(cf, ದತ್ತಾಂಶ, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_ಖಾಲಿಗಳು;
  }

  ಹಿಂತಿರುಗಿ store;
}

ಸ್ಥಿರ ಶೂನ್ಯ ossl_ಗಣ_cached_x509_store(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       X509_STORE *store,
                                       bool is_ಖಾಲಿಗಳು)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ Curl_multi *multi = ದತ್ತಾಂಶ->multi;
  ರಚನೆ ossl_x509_share *share;

  DEBUGASSERT(multi);
  ಆದರೆ(!multi)
    ಹಿಂತಿರುಗಿ;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_ಕೀಲಿ,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_ಕೀಲಿ));

  ಆದರೆ(!share) {
    share = curlx_calloc(1, ಗಾತ್ರ(*share));
    ಆದರೆ(!share)
      ಹಿಂತಿರುಗಿ;
    ಆದರೆ(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_ಕೀಲಿ,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_ಕೀಲಿ),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      ಹಿಂತಿರುಗಿ;
    }
  }

  ಆದರೆ(X509_STORE_ಮೇಲೆ_ref(store)) {
    ಅಕ್ಷರ *CAfile = ಖಾಲಿ;
    ಅಕ್ಷರ *CApath = ಖಾಲಿ;

    ಆದರೆ(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      ಆದರೆ(!CAfile) {
        X509_STORE_free(store);
        ಹಿಂತಿರುಗಿ;
      }
    }

    ಆದರೆ(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      ಆದರೆ(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        ಹಿಂತಿರುಗಿ;
      }
    }

    ಆದರೆ(share->store) {
      X509_STORE_free(share->store);
      curlx_free(share->CAfile);
      curlx_free(share->CApath);
    }

    share->ಸಮಯ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
    share->store = store;
    share->store_is_ಖಾಲಿಗಳು = is_ಖಾಲಿಗಳು;
    share->CAfile = CAfile;
    share->CApath = CApath;
    share->no_partialchain = conn_config->no_partialchain;
  }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_HAS_PROVIDERS
/* CApath certificates loaded under a provider libctx can outlive that libctx
   when the store is shared with the multi handle. */
#ವ್ಯಾಖ್ಯಾನಿಸು USING_PROVIDER(octx) octx->p.libctx
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು USING_PROVIDER(x) 0
#ಆದರೆ_ಅಂತ್ಯ

CURLcode Curl_ssl_ಸಿದ್ಧತೆ_x509_store(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ossl_ctx *octx)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  X509_STORE *cached_store;
  bool cache_criteria_met, is_ಖಾಲಿಗಳು;

  /* Consider the X509 store cacheable if it comes from CAfile/CApath,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (ದತ್ತಾಂಶ->ಗಣ.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !USING_PROVIDER(octx);

  ERR_ಗಣ_mark();

  cached_store = ossl_get_cached_x509_store(cf, ದತ್ತಾಂಶ, &is_ಖಾಲಿಗಳು);
  ಆದರೆ(cached_store && cache_criteria_met && X509_STORE_ಮೇಲೆ_ref(cached_store)) {
    SSL_CTX_ಗಣ_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_ಖಾಲಿಗಳು = is_ಖಾಲಿಗಳು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    ಫಲಿತಾಂಶ = ossl_populate_x509_store(cf, ದತ್ತಾಂಶ, octx, store);
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_OK && cache_criteria_met) {
      ossl_ಗಣ_cached_x509_store(cf, ದತ್ತಾಂಶ, store, (bool)octx->store_is_ಖಾಲಿಗಳು);
    }
  }

  ERR_ತೆಗೆ_ಗಮ್ಯ_mark();

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool ossl_apply_session(
  ರಚನೆ ossl_ctx *octx,
  ರಚನೆ Curl_cfilter *cf,
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  ರಚನೆ Curl_ssl_session *scs)
{
  ರಚನೆ ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *der_sessionid = scs->sdata;
  size_t der_sessionid_ಗಾತ್ರ = scs->sdata_len;
  SSL_SESSION *ssl_session = ಖಾಲಿ;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(ಖಾಲಿ, &der_sessionid,
                                (ದೀರ್ಘ)der_sessionid_ಗಾತ್ರ);
  ಆದರೆ(ssl_session) {
    ಆದರೆ(!SSL_ಗಣ_session(octx->ssl, ssl_session)) {
      VERBOSE(ಅಕ್ಷರ error_ಬಫರ್[256]);
      infof(ದತ್ತಾಂಶ, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_ಬಫರ್,
                          ಗಾತ್ರ(error_ಬಫರ್)));
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(conn_cfg->verifypeer &&
         (SSL_get_verify_ಫಲಿತಾಂಶ(octx->ssl) != X509_V_OK)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#ಆದರೆ_ಅಂತ್ಯ
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_ಗಣ_session(octx->ssl, ಖಾಲಿ);
        infof(ದತ್ತಾಂಶ, "SSL session not peer verified, not reusing");
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        infof(ದತ್ತಾಂಶ, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#ಆದರೆ_ಅಂತ್ಯ
        infof(ದತ್ತಾಂಶ, "SSL verify result: %lx",
              (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)SSL_get_verify_ಫಲಿತಾಂಶ(octx->ssl));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL_EARLYDATA
        ಆದರೆ(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_ದತ್ತಾಂಶ(ssl_session) &&
           !cf->conn->ಬಿಟ್ಟುಗಳು.connect_only &&
           (SSL_ಆವೃತ್ತಿ(octx->ssl) == TLS1_3_ಆವೃತ್ತಿ)) {
          bool do_early_ದತ್ತಾಂಶ = FALSE;
          ಆದರೆ(sess_reuse_cb)
            (ಶೂನ್ಯ)sess_reuse_cb(cf, ದತ್ತಾಂಶ, alpns, scs, &do_early_ದತ್ತಾಂಶ);
          ಆದರೆ(do_early_ದತ್ತಾಂಶ) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_ಗಮ್ಯ(alpns, scs->alpn);
          }
        }
#ಇಲ್ಲದಿದ್ದರೆ
        (ಶೂನ್ಯ)alpns;
        (ಶೂನ್ಯ)conn_cfg;
        (ಶೂನ್ಯ)sess_reuse_cb;
#ಆದರೆ_ಅಂತ್ಯ
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    infof(ದತ್ತಾಂಶ, "SSL session not accepted by OpenSSL, continuing without");
  }
  ಹಿಂತಿರುಗಿ (bool)octx->reused_session;
}

ಸ್ಥಿರ CURLcode ossl_init_session_and_alpns(
  ರಚನೆ ossl_ctx *octx,
  ರಚನೆ Curl_cfilter *cf,
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ ssl_peer *peer,
  ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  ರಚನೆ ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ alpn_spec alpns;
  CURLcode ಫಲಿತಾಂಶ;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  ಆದರೆ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ರಚನೆ Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(ದತ್ತಾಂಶ, cf->cft, FIRSTSOCKET);
    ಆದರೆ(scs) {
      ಆದರೆ(ossl_apply_session(octx, cf, ದತ್ತಾಂಶ, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "applied SSL session from control connection");
    }
  }

  ಆದರೆ(!octx->reused_session &&
     Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ) && !conn_cfg->verifystatus) {
    ರಚನೆ Curl_ssl_session *scs = ಖಾಲಿ;

    ಫಲಿತಾಂಶ = Curl_ssl_scache_take(cf, ದತ್ತಾಂಶ, peer->scache_ಕೀಲಿ, &scs);
    ಆದರೆ(!ಫಲಿತಾಂಶ && scs && scs->sdata && scs->sdata_len) {
      (ಶೂನ್ಯ)ossl_apply_session(octx, cf, ದತ್ತಾಂಶ, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, ದತ್ತಾಂಶ, peer->scache_ಕೀಲಿ, scs);
  }

  ಆದರೆ(alpns.ಎಣಿಕೆ_2) {
    ರಚನೆ alpn_proto_buf proto;
    memset(&proto, 0, ಗಾತ್ರ(proto));
    ಫಲಿತಾಂಶ = Curl_alpn_ಗಮ್ಯ_proto_buf(&proto, &alpns);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Error determining ALPN");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
    ಆದರೆ(SSL_ಗಣ_alpn_protos(octx->ssl, proto.ದತ್ತಾಂಶ, proto.len)) {
      failf(ದತ್ತಾಂಶ, "Error setting ALPN");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_SET1_ECH_CONFIG_LIST
ಸ್ಥಿರ CURLcode ossl_init_ech(ರಚನೆ ossl_ctx *octx,
                              ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ssl_peer *peer)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಪೂರ್ಣಾಂಕ trying_ech_now = 0;

  ಆದರೆ(!Curl_ssl_ech_enabled(cf))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(conn_config->ech == CURLECH_GREASE) {
    infof(ದತ್ತಾಂಶ, "ECH: will GREASE ClientHello");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BORINGSSL_LIKE
    SSL_ಗಣ_enable_ech_grease(octx->ssl, 1);
#ಇಲ್ಲದಿದ್ದರೆ
    SSL_ಗಣ_options(octx->ssl, SSL_OP_ECH_GREASE);
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(conn_config->ech && conn_config->ech_config) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode ಫಲಿತಾಂಶ;

    ಆದರೆ(!conn_config->ech_config[0]) {
      infof(ದತ್ತಾಂಶ, "ECH: ECHConfig from command line empty");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(conn_config->ech_config);
    ಫಲಿತಾಂಶ = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    ಆದರೆ(ಫಲಿತಾಂಶ || !ech_config) {
      infof(ದತ್ತಾಂಶ, "ECH: cannot base64 decode ECHConfig from command line");
      ಆದರೆ(conn_config->ech == CURLECH_HARD)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಆದರೆ(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(ದತ್ತಾಂಶ, "ECH: SSL_ECH_set1_ech_config_list failed");
      ಆದರೆ(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#ಇಲ್ಲದಿದ್ದರೆ
    size_t ech_config_len = 0;
    ಆದರೆ(!conn_config->ech_config[0]) {
      infof(ದತ್ತಾಂಶ, "ECH: ECHConfig from command line empty");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(conn_config->ech_config);
    ಆದರೆ(SSL_set1_ech_config_list(octx->ssl,
                                (ಸ್ಥಿರಾಂಕ uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(ದತ್ತಾಂಶ, "ECH: SSL_ECH_set1_ech_config_list failed");
      ಆದರೆ(conn_config->ech == CURLECH_HARD)
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      trying_ech_now = 1;
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_BORINGSSL_LIKE */
    infof(ದತ್ತಾಂಶ, "ECH: ECHConfig from command line");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(ದತ್ತಾಂಶ, cf->sockindex, peer->origin);

    ಆದರೆ(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(ದತ್ತಾಂಶ, "ECH: ECHConfig from HTTPS RR");
      ಆದರೆ(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(ದತ್ತಾಂಶ, "ECH: SSL_set1_ech_config_list failed");
        ಆದರೆ(conn_config->ech == CURLECH_HARD)
          ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        trying_ech_now = 1;
        infof(ದತ್ತಾಂಶ, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, "ECH: requested but no ECHConfig available");
      ಆದರೆ(conn_config->ech == CURLECH_HARD)
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BORINGSSL_LIKE
  (ಶೂನ್ಯ)peer;
  ಆದರೆ(trying_ech_now && conn_config->ech_ಬಹಿರಂಗ) {
    infof(ದತ್ತಾಂಶ, "ECH: setting public_name not supported with BoringSSL");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(trying_ech_now && conn_config->ech_ಬಹಿರಂಗ) {
    ಪೂರ್ಣಾಂಕ ret;
    infof(ದತ್ತಾಂಶ, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_ಬಹಿರಂಗ);
    ret = SSL_ech_set1_server_ಹೆಸರುಗಳು(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_ಬಹಿರಂಗ,
                                    0 /* do send outer */);
    ಆದರೆ(ret != 1) {
      infof(ದತ್ತಾಂಶ, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_BORINGSSL_LIKE */
  ಆದರೆ(trying_ech_now &&
     SSL_ಗಣ_min_proto_ಆವೃತ್ತಿ(octx->ssl, TLS1_3_ಆವೃತ್ತಿ) != 1) {
    infof(ದತ್ತಾಂಶ, "ECH: cannot force TLSv1.3 [ERROR]");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

ಸ್ಥಿರ CURLcode ossl_init_ssl(ರಚನೆ ossl_ctx *octx,
                              ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ ssl_peer *peer,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpns_requested,
                              ಶೂನ್ಯ *ssl_user_ದತ್ತಾಂಶ,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  ಆದರೆ(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  ಆದರೆ(!octx->ssl) {
    failf(ದತ್ತಾಂಶ, "SSL: could not create a context (handle)");
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  SSL_ಗಣ_app_ದತ್ತಾಂಶ(octx->ssl, ssl_user_ದತ್ತಾಂಶ);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_NO_OCSP
  ಆದರೆ(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_ಗಣ_tlsext_ಸ್ಥಿತಿ_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#ಆದರೆ_ಅಂತ್ಯ

  SSL_ಗಣ_connect_state(octx->ssl);

  ಆದರೆ(peer->sni) {
    ಆದರೆ(!SSL_ಗಣ_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(ದತ್ತಾಂಶ, "Failed set SNI");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode ಫಲಿತಾಂಶ = ossl_init_ech(octx, cf, ದತ್ತಾಂಶ, peer);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  ಹಿಂತಿರುಗಿ ossl_init_session_and_alpns(octx, cf, ದತ್ತಾಂಶ, peer,
                                     alpns_requested, sess_reuse_cb);
}

ಸ್ಥಿರ CURLcode ossl_init_ವಿಧಾನ(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ssl_peer *peer,
                                 ಸ್ಥಿರಾಂಕ SSL_ವಿಧಾನ **pmethod,
                                 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *pssl_ಆವೃತ್ತಿ_min)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = ಖಾಲಿ;
  *pssl_ಆವೃತ್ತಿ_min = conn_config->ಆವೃತ್ತಿ_2;
  DEBUGASSERT(conn_config->ಆವೃತ್ತಿ_2 != CURL_SSLVERSION_DEFAULT);
  ಆಯ್ಕೆ(peer->transport) {
  ಸ್ಥಿತಿ TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    ಆಯ್ಕೆ(*pssl_ಆವೃತ್ತಿ_min) {
    ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1:
    ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0:
    ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1:
    ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_2:
    ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_ವಿಧಾನ();
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_SSLVERSION_SSLv2:
      failf(ದತ್ತಾಂಶ, "No SSLv2 support");
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
    ಸ್ಥಿತಿ CURL_SSLVERSION_SSLv3:
      failf(ದತ್ತಾಂಶ, "No SSLv3 support");
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
    ಪೂರ್ವನಿಯೋಜಿತ:
      failf(ದತ್ತಾಂಶ, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ TRNSPRT_QUIC:
    *pssl_ಆವೃತ್ತಿ_min = CURL_SSLVERSION_TLSv1_3;
    ಆದರೆ(conn_config->ಆವೃತ್ತಿ_max &&
       (conn_config->ಆವೃತ್ತಿ_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->ಆವೃತ್ತಿ_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(ದತ್ತಾಂಶ, "QUIC needs at least TLS version 1.3");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_ವಿಧಾನ();
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "unsupported transport %d in SSL init", peer->transport);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಹಿಂತಿರುಗಿ *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(ರಚನೆ ossl_ctx *octx,
                            ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ssl_peer *peer,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpns_requested,
                            Curl_ossl_ctx_ಸಿದ್ಧತೆ_cb *cb_ಸಿದ್ಧತೆ,
                            ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ,
                            Curl_ossl_new_session_cb *cb_new_session,
                            ಶೂನ್ಯ *ssl_user_ದತ್ತಾಂಶ,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ciphers;
  ಸ್ಥಿರಾಂಕ SSL_ವಿಧಾನ *req_ವಿಧಾನ = ಖಾಲಿ;
  ctx_option_t ctx_options = 0;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ದತ್ತಾಂಶ);
  ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_cert = conn_config->clientcert;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_cert_type = conn_config->cert_type;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ssl_ಆವೃತ್ತಿ_min = CURL_SSLVERSION_DEFAULT;
  ಅಕ್ಷರ error_ಬಫರ್[256];

  /* Make funny stuff to get random input */
  ಫಲಿತಾಂಶ = ossl_ಬೀಜ(ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಆದರೆ(CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_ENGINE))
      ಫಲಿತಾಂಶ = ossl_ಗಣ_engine(ದತ್ತಾಂಶ, octx,
                               CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_ENGINE));
    ಆದರೆ(!ಫಲಿತಾಂಶ && ದತ್ತಾಂಶ->ಗಣ.engine_default)
      ಫಲಿತಾಂಶ = ossl_ಗಣ_engine_default(ದತ್ತಾಂಶ, octx);
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ssl_config->certverifyresult = !X509_V_OK;
    ಫಲಿತಾಂಶ = ossl_init_ವಿಧಾನ(cf, ದತ್ತಾಂಶ, peer, &req_ವಿಧಾನ, &ssl_ಆವೃತ್ತಿ_min);
  }
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  DEBUGASSERT(req_ವಿಧಾನ);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_ವಿಧಾನ):
#ಆದರೆ_ಅಂತ್ಯ
    SSL_CTX_new(req_ವಿಧಾನ);

  ಆದರೆ(!octx->ssl_ctx) {
    failf(ದತ್ತಾಂಶ, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್)));
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(cb_ಸಿದ್ಧತೆ) {
    ಫಲಿತಾಂಶ = cb_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, cb_user_ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.fdebug && ದತ್ತಾಂಶ->ಗಣ.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_ಗಣ_ಸಂದೇಶ_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_ಗಣ_ಸಂದೇಶ_callback_arg(octx->ssl_ctx, cf);
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
  ಆದರೆ(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_ಸೇರಿಸು_ಖಾಲಿಗಳು_FRAGMENTS;

  DEBUGASSERT(ssl_ಆವೃತ್ತಿ_min != CURL_SSLVERSION_DEFAULT);
  ಆಯ್ಕೆ(ssl_ಆವೃತ್ತಿ_min) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_SSLv2:
  ಸ್ಥಿತಿ CURL_SSLVERSION_SSLv3:
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    ಫಲಿತಾಂಶ = ossl_ಗಣ_ssl_ಆವೃತ್ತಿ_min_max(cf, octx->ssl_ctx, ssl_ಆವೃತ್ತಿ_min);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  SSL_CTX_ಗಣ_options(octx->ssl_ctx, ctx_options);
  SSL_CTX_ಗಣ_read_ahead(octx->ssl_ctx, 1);

  /* Max TLS1.2 record size 0x4000 + 0x800.
     OpenSSL supports processing "jumbo TLS record" (8 TLS records) in one go
     for some algorithms, so match that here.
     Experimentation shows that a slightly larger buffer is needed
     to avoid short reads.

     However using a large buffer (8 packets) actually decreases performance.
     4 packets is better.
   */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_CTX_ಗಣ_DEFAULT_READ_ಬಫರ್_LEN
  SSL_CTX_ಗಣ_default_read_ಬಫರ್_len(octx->ssl_ctx, 0x401e * 4);
#ಆದರೆ_ಅಂತ್ಯ

  /* We do retry writes sometimes from another buffer address */
  SSL_CTX_ಗಣ_ಬಹುಲಕ(octx->ssl_ctx, SSL_ಬಹುಲಕ_ACCEPT_MOVING_WRITE_ಬಫರ್);

  ciphers = conn_config->cipher_list;
  ಆದರೆ(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = ಖಾಲಿ;
  ಆದರೆ(ciphers && (ssl_ಆವೃತ್ತಿ_min < CURL_SSLVERSION_TLSv1_3)) {
    ಆದರೆ(!SSL_CTX_ಗಣ_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(ದತ್ತಾಂಶ, "failed setting cipher list: %s", ciphers);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CIPHER;
    }
    infof(ದತ್ತಾಂಶ, "Cipher selection: %s", ciphers);
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_CTX_ಗಣ_CIPHERSUITES
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ciphers13 = conn_config->cipher_list13;
    ಆದರೆ(ciphers13 &&
       (!conn_config->ಆವೃತ್ತಿ_max ||
        (conn_config->ಆವೃತ್ತಿ_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->ಆವೃತ್ತಿ_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      ಆದರೆ(!SSL_CTX_ಗಣ_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(ದತ್ತಾಂಶ, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CIPHER;
      }
      infof(ದತ್ತಾಂಶ, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    ಫಲಿತಾಂಶ = client_cert(ದತ್ತಾಂಶ, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->ಕೀಲಿ, conn_config->ಕೀಲಿ_blob,
                         conn_config->ಕೀಲಿ_type,
                         conn_config->ಕೀಲಿ_passwd);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      /* failf() is already done in client_cert() */
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_CTX_ಗಣ_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_ಗಣ_post_handshake_auth(octx->ssl_ctx, 1);
#ಆದರೆ_ಅಂತ್ಯ

  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *curves = conn_config->curves;
    ಆದರೆ(curves) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BORINGSSL_LIKE
#ವ್ಯಾಖ್ಯಾನಿಸು OSSL_ವಕ್ರ_CAST(x) (x)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು OSSL_ವಕ್ರ_CAST(x) (ಅಕ್ಷರ *)CURL_UNCONST(x)
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_ವಕ್ರ_CAST(curves))) {
        failf(ದತ್ತಾಂಶ, "failed setting curves list: '%s'", curves);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CIPHER;
      }
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_CTX_SET1_SIGALGS
#ವ್ಯಾಖ್ಯಾನಿಸು OSSL_SIGALG_CAST(x) OSSL_ವಕ್ರ_CAST(x)
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *signature_algorithms = conn_config->signature_algorithms;
    ಆದರೆ(signature_algorithms) {
      ಆದರೆ(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(ದತ್ತಾಂಶ, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CIPHER;
      }
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_ಗಣ_verify(octx->ssl_ctx, SSL_VERIFY_NONE, ಖಾಲಿ);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#ಆದರೆ !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  ಆದರೆ(Curl_tls_keylog_enabled()) {
    SSL_CTX_ಗಣ_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(cb_new_session) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_ಗಣ_session_cache_ಬಹುಲಕ(octx->ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_ಆಂತರಿಕ);
    SSL_CTX_sess_ಗಣ_new_cb(octx->ssl_ctx, cb_new_session);
  }

  /* give application a chance to interfere with SSL set up. */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctx) {
    ರಚನೆ Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    ಆದರೆ(!octx->x509_store_ಸಿದ್ಧತೆ) {
      ಫಲಿತಾಂಶ = Curl_ssl_ಸಿದ್ಧತೆ_x509_store(cf, ದತ್ತಾಂಶ, octx);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      octx->x509_store_ಸಿದ್ಧತೆ = TRUE;
    }
    CURL_CBAPI_ಆರಂಭ(&guard, ದತ್ತಾಂಶ, easy_fsslctx);
    ಫಲಿತಾಂಶ = (*ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctx)(ದತ್ತಾಂಶ, octx->ssl_ctx,
                                      ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctxp);
    CURL_CBAPI_ಕೊನೆ(&guard);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "error signaled by SSL ctx callback");
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಹಿಂತಿರುಗಿ ossl_init_ssl(octx, cf, ದತ್ತಾಂಶ, peer, alpns_requested,
                       ssl_user_ದತ್ತಾಂಶ, sess_reuse_cb);
}

ಸ್ಥಿರ CURLcode ossl_on_session_reuse(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ alpn_spec *alpns,
                                      ರಚನೆ Curl_ssl_session *scs,
                                      bool *do_early_ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  ಹಿಂತಿರುಗಿ Curl_on_session_reuse(cf, ದತ್ತಾಂಶ, alpns, scs, do_early_ದತ್ತಾಂಶ,
                               connssl->earlydata_max);
}

ಶೂನ್ಯ Curl_ossl_report_handshake(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ossl_ctx *octx)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    ಪೂರ್ಣಾಂಕ psigtype_nid = NID_undef;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *negotiated_ಗುಂಪು_name = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#ಆದರೆ OPENSSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x30200000L
    negotiated_ಗುಂಪು_name = SSL_get0_ಗುಂಪು_name(octx->ssl);
#ಇಲ್ಲದಿದ್ದರೆ
    negotiated_ಗುಂಪು_name =
      OBJ_nid2sn(SSL_get_negotiated_ಗುಂಪು(octx->ssl) & 0x0000FFFF);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

    /* Informational message */
    infof(ದತ್ತಾಂಶ, "SSL connection using %s / %s / %s / %s",
          SSL_get_ಆವೃತ್ತಿ(octx->ssl),
          SSL_get_cipher(octx->ssl),
          negotiated_ಗುಂಪು_name ? negotiated_ಗುಂಪು_name : "[blank]",
          OBJ_nid2sn(psigtype_nid));
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)octx;
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */
}

ಸ್ಥಿರ CURLcode ossl_connect_step1(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  ಫಲಿತಾಂಶ = Curl_ossl_ctx_init(octx, cf, ದತ್ತಾಂಶ, &connssl->peer,
                              connssl->alpn, ಖಾಲಿ, ಖಾಲಿ,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  octx->bio_ವಿಧಾನ = ossl_bio_cf_ವಿಧಾನ_create();
  ಆದರೆ(!octx->bio_ವಿಧಾನ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  bio = BIO_new(octx->bio_ವಿಧಾನ);
  ಆದರೆ(!bio)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  BIO_ಗಣ_ದತ್ತಾಂಶ(bio, cf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_SET0_WBIO
  /* with OpenSSL v1.1.1 we get an alternative to SSL_set_bio() that works
   * without backward compat quirks. Every call takes one reference, so we
   * up it and pass. SSL* then owns and frees it.
   * We check on the function in configure, since LibreSSL and friends
   * each have their own versions to add support for this. */
  BIO_ಮೇಲೆ_ref(bio);
  SSL_set0_rbio(octx->ssl, bio);
  SSL_set0_wbio(octx->ssl, bio);
#ಇಲ್ಲದಿದ್ದರೆ
  SSL_ಗಣ_bio(octx->ssl, bio, bio);
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ರಚನೆ alpn_proto_buf proto;
    memset(&proto, 0, ಗಾತ್ರ(proto));
    Curl_alpn_ಗಮ್ಯ_proto_str(&proto, connssl->alpn);
    infof(ದತ್ತಾಂಶ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ದತ್ತಾಂಶ);
  }

  connssl->connecting_state = ssl_connect_2;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ossl_trace_ech_retry_configs(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, SSL *ssl,
                                        ಪೂರ್ಣಾಂಕ reason)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t rcl = 0;
  ಪೂರ್ಣಾಂಕ rv = 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_BORINGSSL_LIKE
  ಅಕ್ಷರ *inner = ಖಾಲಿ;
  uint8_t *rcs = ಖಾಲಿ;
  ಅಕ್ಷರ *outer = ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *inner = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ uint8_t *rcs = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *outer = ಖಾಲಿ;
  size_t ಹೊರಹೋಗು_name_len = 0;
  ಪೂರ್ಣಾಂಕ servername_type = 0;
#ಆದರೆ_ಅಂತ್ಯ
  NOVERBOSE((ಶೂನ್ಯ)reason);

  /* nothing to trace if not doing ECH */
  ಆದರೆ(!Curl_ssl_ech_enabled(cf))
    ಹಿಂತಿರುಗಿ rv;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#ಇಲ್ಲದಿದ್ದರೆ
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (ಪೂರ್ಣಾಂಕ)rcl;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(rv && rcs) {
    ಅಕ್ಷರ *b64str = ಖಾಲಿ;
    size_t blen = 0;

    ಫಲಿತಾಂಶ = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    ಆದರೆ(!ಫಲಿತಾಂಶ && b64str) {
      infof(ದತ್ತಾಂಶ, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_ಸ್ಥಿತಿ(ssl, &inner, &outer);
      infof(ದತ್ತಾಂಶ, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#ಇಲ್ಲದಿದ್ದರೆ
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &ಹೊರಹೋಗು_name_len);
      infof(ದತ್ತಾಂಶ, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#ಆದರೆ_ಅಂತ್ಯ
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    infof(ದತ್ತಾಂಶ, "ECH: no retry_configs (rv = %d)", rv);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ rv;
}

#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode ossl_connect_step2(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  ರಚನೆ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ದತ್ತಾಂಶ);
  ಪೂರ್ಣಾಂಕ err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  ಆದರೆ(!octx->x509_store_ಸಿದ್ಧತೆ) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode ಫಲಿತಾಂಶ = Curl_ssl_ಸಿದ್ಧತೆ_x509_store(cf, ದತ್ತಾಂಶ, octx);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    octx->x509_store_ಸಿದ್ಧತೆ = TRUE;
  }

#ಆದರೆ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  ಆದರೆ(Curl_tls_keylog_enabled() && !octx->keylog_ಮುಗಿದ)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_ಮುಗಿದ);
#ಆದರೆ_ಅಂತ್ಯ

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  ಆದರೆ(err != 1) {
    ಪೂರ್ಣಾಂಕ detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    ಆದರೆ(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
    ಆದರೆ(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_ERROR_WANT_ASYNC
    ಆದರೆ(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_ERROR_WANT_RETRY_VERIFY
    ಆದರೆ(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(ದತ್ತಾಂಶ, TRUE);
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಇಲ್ಲದಿದ್ದರೆ {
      /* untreated error */
      sslerr_t errdetail;
      ಅಕ್ಷರ error_ಬಫರ್[256] = "";
      CURLcode ಫಲಿತಾಂಶ;
      ದೀರ್ಘ lerr;
      ಪೂರ್ಣಾಂಕ lib;
      ಪೂರ್ಣಾಂಕ reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      ಆದರೆ((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#ಆದರೆ_ಅಂತ್ಯ
         )) {
        ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_ಫಲಿತಾಂಶ(octx->ssl);
        ಆದರೆ(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(ದತ್ತಾಂಶ, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        ಇಲ್ಲದಿದ್ದರೆ
          failf(ದತ್ತಾಂಶ, "%s", "SSL certificate verification failed");
      }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        ಫಲಿತಾಂಶ = CURLE_SSL_CLIENTCERT;
        failf(ದತ್ತಾಂಶ, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್)));
      }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_SET1_ECH_CONFIG_LIST
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((lib == ERR_LIB_SSL) &&
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#ಇಲ್ಲದಿದ್ದರೆ
              (reason == SSL_R_ECH_REJECTED)) {
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, ದತ್ತಾಂಶ, octx->ssl, reason);

        ಫಲಿತಾಂಶ = CURLE_ECH_REQUIRED;
        failf(ದತ್ತಾಂಶ, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್)));
      }
#ಆದರೆ_ಅಂತ್ಯ
      ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
        failf(ದತ್ತಾಂಶ, "TLS connect error: %s",
              ossl_strerror(errdetail, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        ಅಕ್ಷರ extramsg[80] = "";
        ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;

        ಆದರೆ(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, ಗಾತ್ರ(extramsg));
        failf(ದತ್ತಾಂಶ, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_ಗಮ್ಯ_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->ಪೋರ್ಟ್);
      }

      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(ದತ್ತಾಂಶ, octx);

#ಆದರೆ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    ಆದರೆ(Curl_ssl_ech_enabled(cf)) {
      ರಚನೆ ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      ಅಕ್ಷರ *inner = ಖಾಲಿ, *outer = ಖಾಲಿ;
      ಪೂರ್ಣಾಂಕ rv;
      VERBOSE(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿತಿ_2);

      rv = SSL_ech_get1_ಸ್ಥಿತಿ(octx->ssl, &inner, &outer);
      ಆಯ್ಕೆ(rv) {
      ಸ್ಥಿತಿ SSL_ECH_ಸ್ಥಿತಿ_SUCCESS:
        VERBOSE(ಸ್ಥಿತಿ_2 = "succeeded");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSL_ECH_ಸ್ಥಿತಿ_GREASE_ECH:
        VERBOSE(ಸ್ಥಿತಿ_2 = "sent GREASE, got retry-configs");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSL_ECH_ಸ್ಥಿತಿ_GREASE:
        VERBOSE(ಸ್ಥಿತಿ_2 = "sent GREASE");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSL_ECH_ಸ್ಥಿತಿ_NOT_TRIED:
        VERBOSE(ಸ್ಥಿತಿ_2 = "not attempted");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSL_ECH_ಸ್ಥಿತಿ_NOT_CONFIGURED:
        VERBOSE(ಸ್ಥಿತಿ_2 = "not configured");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSL_ECH_ಸ್ಥಿತಿ_BACKEND:
        VERBOSE(ಸ್ಥಿತಿ_2 = "backend (unexpected)");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSL_ECH_ಸ್ಥಿತಿ_FAILED:
        VERBOSE(ಸ್ಥಿತಿ_2 = "failed");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSL_ECH_ಸ್ಥಿತಿ_BAD_CALL:
        VERBOSE(ಸ್ಥಿತಿ_2 = "bad call (unexpected)");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ SSL_ECH_ಸ್ಥಿತಿ_BAD_NAME: {
        ಆದರೆ(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(ಸ್ಥಿತಿ_2 = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_ಸ್ಥಿತಿ_SUCCESS;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          VERBOSE(ಸ್ಥಿತಿ_2 = "bad name (unexpected)");
        }
        ನಿಲ್ಲಿಸು;
      }
      ಪೂರ್ವನಿಯೋಜಿತ:
        VERBOSE(ಸ್ಥಿತಿ_2 = "unexpected status");
        infof(ದತ್ತಾಂಶ, "ECH: unexpected status %d", rv);
      }
      infof(ದತ್ತಾಂಶ, "ECH: result '%s' (inner=%s, outer=%s)",
            (ಸ್ಥಿತಿ_2 ? ಸ್ಥಿತಿ_2 : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      ಆದರೆ(rv == SSL_ECH_ಸ್ಥಿತಿ_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, ದತ್ತಾಂಶ, octx->ssl, 0);
      }
      ಆದರೆ(rv != SSL_ECH_ಸ್ಥಿತಿ_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(ದತ್ತಾಂಶ, "ECH: ech-hard failed");
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, "ECH: result: status is not attempted");
    }
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    ಆದರೆ(connssl->alpn) {
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *neg_protocol;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      ಹಿಂತಿರುಗಿ Curl_alpn_ಗಣ_negotiated(cf, ದತ್ತಾಂಶ, connssl, neg_protocol, len);
    }

    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
ಸ್ಥಿರ CURLcode ossl_pkp_pin_peer_pubkey(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, X509 *cert,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinnedpubkey)
{
  /* Scratch */
  ಪೂರ್ಣಾಂಕ len1 = 0, len2 = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buff1 = ಖಾಲಿ, *ತಾತ್ಕಾಲಿಕ = ಖಾಲಿ;

  /* Result is returned to caller */
  CURLcode ಫಲಿತಾಂಶ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ಆದರೆ(!pinnedpubkey)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(!cert)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಮಾಡು {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), ಖಾಲಿ);
    ಆದರೆ(len1 < 1)
      ನಿಲ್ಲಿಸು; /* failed */

    buff1 = ತಾತ್ಕಾಲಿಕ = curlx_malloc(len1);
    ಆದರೆ(!buff1)
      ನಿಲ್ಲಿಸು; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &ತಾತ್ಕಾಲಿಕ);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    ಆದರೆ((len1 != len2) || !ತಾತ್ಕಾಲಿಕ || ((ತಾತ್ಕಾಲಿಕ - buff1) != len1))
      ನಿಲ್ಲಿಸು; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    ಫಲಿತಾಂಶ = Curl_pin_peer_pubkey(ದತ್ತಾಂಶ, pinnedpubkey, buff1, len1);
  } ಆಗ(0);

  ಆದರೆ(buff1)
    curlx_free(buff1);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ಆದರೆ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ) && LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x3060000fL)
ಸ್ಥಿರ ಶೂನ್ಯ infof_certstack(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ SSL *ssl)
{
  ರಾಶಿ_ದ(X509) *certstack;
  ದೀರ್ಘ verify_ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ_cert_ಮಟ್ಟಗಳು;
  ಪೂರ್ಣಾಂಕ cert_level;

  ಆದರೆ(!Curl_trc_is_verbose(ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ;

  verify_ಫಲಿತಾಂಶ = SSL_get_verify_ಫಲಿತಾಂಶ(ssl);
  ಆದರೆ(verify_ಫಲಿತಾಂಶ != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  ಇಲ್ಲದಿದ್ದರೆ
    certstack = SSL_get0_verified_chain(ssl);
  ಆದರೆ(!certstack)
    ಹಿಂತಿರುಗಿ;
  ಅಂಕೆ_cert_ಮಟ್ಟಗಳು = sk_X509_ಅಂಕೆ(certstack);

  ಗಾಗಿ(cert_level = 0; cert_level < ಅಂಕೆ_cert_ಮಟ್ಟಗಳು; cert_level++) {
    ಅಕ್ಷರ cert_algorithm[80] = "";
    ಅಕ್ಷರ ಗುಂಪು_name_final[80] = "";
    ಸ್ಥಿರಾಂಕ X509_ALGOR *palg_cert = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ASN1_OBJECT *paobj_cert = ಖಾಲಿ;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    ಪೂರ್ಣಾಂಕ ಕೀಲಿ_ಬಿಟ್ಟುಗಳು;
    ಪೂರ್ಣಾಂಕ ಕೀಲಿ_sec_ಬಿಟ್ಟುಗಳು;
    ಪೂರ್ಣಾಂಕ get_ಗುಂಪು_name;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type_name;

    current_cert = sk_X509_ಮೌಲ್ಯ(certstack, cert_level);
    ಆದರೆ(!current_cert)
      ಮುಂದುವರಿ;

    current_pkey = X509_get0_pubkey(current_cert);
    ಆದರೆ(!current_pkey)
      ಮುಂದುವರಿ;

    X509_get0_signature(ಖಾಲಿ, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, ಖಾಲಿ, ಖಾಲಿ, palg_cert);
    OBJ_obj2txt(cert_algorithm, ಗಾತ್ರ(cert_algorithm), paobj_cert, 0);

    ಕೀಲಿ_ಬಿಟ್ಟುಗಳು = EVP_PKEY_ಬಿಟ್ಟುಗಳು(current_pkey);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_OPENSSL3
#ವ್ಯಾಖ್ಯಾನಿಸು EVP_PKEY_get_security_ಬಿಟ್ಟುಗಳು EVP_PKEY_security_ಬಿಟ್ಟುಗಳು
#ಆದರೆ_ಅಂತ್ಯ
    ಕೀಲಿ_sec_ಬಿಟ್ಟುಗಳು = EVP_PKEY_get_security_ಬಿಟ್ಟುಗಳು(current_pkey);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL3
    {
      ಅಕ್ಷರ ಗುಂಪು_name[80] = "";
      get_ಗುಂಪು_name = EVP_PKEY_get_ಗುಂಪು_name(current_pkey, ಗುಂಪು_name,
                                               ಗಾತ್ರ(ಗುಂಪು_name), ಖಾಲಿ);
      curl_msnprintf(ಗುಂಪು_name_final, ಗಾತ್ರ(ಗುಂಪು_name_final), "/%s",
                     ಗುಂಪು_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#ಇಲ್ಲದಿದ್ದರೆ
    get_ಗುಂಪು_name = 0;
    type_name = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    infof(ದತ್ತಾಂಶ, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_ಗುಂಪು_name == 0 ? "" : ಗುಂಪು_name_final,
          ಕೀಲಿ_ಬಿಟ್ಟುಗಳು, ಕೀಲಿ_sec_ಬಿಟ್ಟುಗಳು, cert_algorithm);
  }
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು infof_certstack(ದತ್ತಾಂಶ, ssl)
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

ಸ್ಥಿರ CURLcode ossl_check_issuer(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  X509 *server_cert)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = ಖಾಲಿ;
  BIO *fp = ಖಾಲಿ;
  ಅಕ್ಷರ err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  ಆದರೆ(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->ದತ್ತಾಂಶ,
                         (ಪೂರ್ಣಾಂಕ)conn_config->issuercert_blob->len);
    ಆದರೆ(!fp) {
      failf(ದತ್ತಾಂಶ, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, ಗಾತ್ರ(err_buf)));
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    ಆದರೆ(!fp) {
      failf(ದತ್ತಾಂಶ, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, ಗಾತ್ರ(err_buf)));
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      ಆದರೆ(verify_enabled)
        failf(ದತ್ತಾಂಶ, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      ಫಲಿತಾಂಶ = CURLE_SSL_ISSUER_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಆದರೆ(fp) {
    issuer = PEM_read_bio_X509(fp, ಖಾಲಿ, ZERO_NULL, ಖಾಲಿ);
    ಆದರೆ(!issuer) {
      ಆದರೆ(verify_enabled)
        failf(ದತ್ತಾಂಶ, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      ಫಲಿತಾಂಶ = CURLE_SSL_ISSUER_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      ಆದರೆ(verify_enabled)
        failf(ದತ್ತಾಂಶ, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      ಫಲಿತಾಂಶ = CURLE_SSL_ISSUER_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    infof(ದತ್ತಾಂಶ, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

ಹೊರಹೋಗು:
  ಆದರೆ(fp)
    BIO_free(fp);
  ಆದರೆ(issuer)
    X509_free(issuer);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinned(ರಚನೆ Curl_cfilter *cf,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  (ಶೂನ್ಯ)cf;
  ಹಿಂತಿರುಗಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#ಆದರೆ_ಅಂತ್ಯ
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY);
}

ಸ್ಥಿರ CURLcode ossl_check_pinned_ಕೀಲಿ(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      X509 *server_cert)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = pinned(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ptr) {
    ಫಲಿತಾಂಶ = ossl_pkp_pin_peer_pubkey(ದತ್ತಾಂಶ, server_cert, ptr);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      failf(ದತ್ತಾಂಶ, "SSL: public key does not match pinned public key");
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_CERT_NAME_ಉದ್ದಳತೆ 2048
ಸ್ಥಿರ CURLcode ossl_infof_cert(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                X509 *server_cert)
{
  BIO *mem = ಖಾಲಿ;
  ರಚನೆ dynbuf dname;
  ಅಕ್ಷರ err_buf[256] = "";
  ಅಕ್ಷರ *buf;
  ದೀರ್ಘ len;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!Curl_trc_is_verbose(ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_ಉದ್ದಳತೆ);
  mem = BIO_new(BIO_s_mem());
  ಆದರೆ(!mem) {
    failf(ದತ್ತಾಂಶ, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, ಗಾತ್ರ(err_buf)));
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  infof(ದತ್ತಾಂಶ, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  ಫಲಿತಾಂಶ = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(ದತ್ತಾಂಶ, "  subject: %s", ಫಲಿತಾಂಶ ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_ಸಮಯ_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_ದತ್ತಾಂಶ(mem, (ಅಕ್ಷರ **)&buf);
  infof(ದತ್ತಾಂಶ, "  start date: %.*s", (ಪೂರ್ಣಾಂಕ)len, buf);
  (ಶೂನ್ಯ)BIO_reset(mem);

  ASN1_ಸಮಯ_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_ದತ್ತಾಂಶ(mem, (ಅಕ್ಷರ **)&buf);
  infof(ದತ್ತಾಂಶ, "  expire date: %.*s", (ಪೂರ್ಣಾಂಕ)len, buf);
  (ಶೂನ್ಯ)BIO_reset(mem);

  ಫಲಿತಾಂಶ = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  ಆದರೆ(ಫಲಿತಾಂಶ) /* should be only fatal stuff like OOM */
    ಹೋಗು ಹೊರಹೋಗು;
  infof(ದತ್ತಾಂಶ, "  issuer: %s", curlx_dyn_ptr(&dname));

ಹೊರಹೋಗು:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* CURLVERBOSE */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
ರಚನೆ ossl_certs_ctx {
  ರಾಶಿ_ದ(X509) *sk;
  size_t ಅಂಕೆ_certs;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಕೊನೆಯ_der;
};

ಸ್ಥಿರ CURLcode ossl_chain_get_der(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                   size_t i,
                                   ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **pder,
                                   size_t *pder_len)
{
  ರಚನೆ ossl_certs_ctx *chain = user_ದತ್ತಾಂಶ;
  X509 *cert;
  ಪೂರ್ಣಾಂಕ der_len;

  OPENSSL_free(chain->ಕೊನೆಯ_der);
  chain->ಕೊನೆಯ_der = ಖಾಲಿ;

  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  *pder_len = 0;
  *pder = ಖಾಲಿ;

  ಆದರೆ(i >= chain->ಅಂಕೆ_certs)
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  cert = sk_X509_ಮೌಲ್ಯ(chain->sk, (ಪೂರ್ಣಾಂಕ)i);
  ಆದರೆ(!cert)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  ಆದರೆ(der_len < 0)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  chain->ಕೊನೆಯ_der = *pder;
  *pder_len = (size_t)der_len;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ossl_apple_verify(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ossl_ctx *octx,
                                  ರಚನೆ ssl_peer *peer)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ ossl_certs_ctx chain;
  CURLcode ಫಲಿತಾಂಶ;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, ಗಾತ್ರ(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.ಅಂಕೆ_certs = chain.sk ? sk_X509_ಅಂಕೆ(chain.sk) : 0;

  ಆದರೆ(!chain.ಅಂಕೆ_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    ಆದರೆ(!octx->reused_session) {
      failf(ದತ್ತಾಂಶ, "SSL: could not get peer certificate chain");
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_BORINGSSL_LIKE
    ಸ್ಥಿರಾಂಕ uint8_t *ocsp_ದತ್ತಾಂಶ = ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ocsp_ದತ್ತಾಂಶ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ದೀರ್ಘ ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    ಆದರೆ(!octx->reused_session)
      ocsp_len = (ದೀರ್ಘ)SSL_get_tlsext_ಸ್ಥಿತಿ_ocsp_resp(octx->ssl, &ocsp_ದತ್ತಾಂಶ);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    ಆದರೆ(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_ದತ್ತಾಂಶ = ಖಾಲಿ;
    }

    ಫಲಿತಾಂಶ = Curl_vtls_apple_verify(cf, ದತ್ತಾಂಶ, peer, chain.ಅಂಕೆ_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_ದತ್ತಾಂಶ, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.ಕೊನೆಯ_der);
    chain.ಕೊನೆಯ_der = ಖಾಲಿ;
  }
  octx->sectrust_verified = !ಫಲಿತಾಂಶ;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ossl_ctx *octx,
                                   ರಚನೆ ssl_peer *peer)
{
  ರಚನೆ connectdata *conn = cf->conn;
  ರಚನೆ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ದತ್ತಾಂಶ);
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ದೀರ್ಘ ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    ಫಲಿತಾಂಶ = ossl_certchain(ದತ್ತಾಂಶ, octx->ssl);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  ಆದರೆ(!server_cert) {
    /* no verification at all, this maybe acceptable */
    ಆದರೆ(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, ದತ್ತಾಂಶ))
      ಹೋಗು ಹೊರಹೋಗು;

    failf(ದತ್ತಾಂಶ, "SSL: could not get peer certificate");
    ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    ಹೋಗು ಹೊರಹೋಗು;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಫಲಿತಾಂಶ = ossl_infof_cert(cf, ದತ್ತಾಂಶ, server_cert);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  infof_certstack(ದತ್ತಾಂಶ, octx->ssl);
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(conn_config->verifyhost) {
    ಫಲಿತಾಂಶ = ossl_verifyhost(ದತ್ತಾಂಶ, conn, peer, server_cert);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_ಫಲಿತಾಂಶ(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(ದತ್ತಾಂಶ, "OpenSSL verify result: %lx", (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  ಆದರೆ(verified)
    infof(ದತ್ತಾಂಶ, "SSL certificate verified via OpenSSL.");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
  ಆದರೆ(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    ಫಲಿತಾಂಶ = ossl_apple_verify(cf, ದತ್ತಾಂಶ, octx, peer);
    ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_PEER_FAILED_VERIFICATION))
      ಹೋಗು ಹೊರಹೋಗು; /* unexpected error */
    ಆದರೆ(octx->sectrust_verified) {
      infof(ದತ್ತಾಂಶ, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!verified) {
    /* no trust established, report the OpenSSL status */
    ಆದರೆ(conn_config->verifypeer) {
      failf(ದತ್ತಾಂಶ, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    infof(ದತ್ತಾಂಶ, " SSL certificate verification failed, continuing anyway!");
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_NO_OCSP
  ಆದರೆ(conn_config->verifystatus &&
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, ದತ್ತಾಂಶ)) &&
#ಆದರೆ_ಅಂತ್ಯ
     !octx->reused_session) {
    ಫಲಿತಾಂಶ = verifystatus(cf, ದತ್ತಾಂಶ, octx);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಫಲಿತಾಂಶ = ossl_check_issuer(cf, ದತ್ತಾಂಶ, server_cert);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = ossl_check_pinned_ಕೀಲಿ(cf, ದತ್ತಾಂಶ, server_cert);

ಹೊರಹೋಗು:
  X509_free(server_cert);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ossl_connect_step3(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  ಫಲಿತಾಂಶ = Curl_ossl_check_peer_cert(cf, ದತ್ತಾಂಶ, octx, &connssl->peer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, ದತ್ತಾಂಶ, connssl->peer.scache_ಕೀಲಿ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL_EARLYDATA
ಸ್ಥಿರ CURLcode ossl_send_earlydata(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf;
  size_t blen, nwritten;
  ಪೂರ್ಣಾಂಕ rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_ಫಲಿತಾಂಶ = CURLE_OK;
  ಆಗ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_ದತ್ತಾಂಶ(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    ಆದರೆ(rc <= 0) {
      ದೀರ್ಘ sslerror;
      ಅಕ್ಷರ error_ಬಫರ್[256];
      ಪೂರ್ಣಾಂಕ err = SSL_get_error(octx->ssl, rc);

      ಆಯ್ಕೆ(err) {
      ಸ್ಥಿತಿ SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        ಫಲಿತಾಂಶ = CURLE_AGAIN;
        ಹೋಗು ಹೊರಹೋಗು;
      ಸ್ಥಿತಿ SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ಫಲಿತಾಂಶ = CURLE_AGAIN;
        ಹೋಗು ಹೊರಹೋಗು;
      ಸ್ಥಿತಿ SSL_ERROR_SYSCALL: {
        ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;

        ಆದರೆ(octx->io_ಫಲಿತಾಂಶ == CURLE_AGAIN) {
          ಫಲಿತಾಂಶ = CURLE_AGAIN;
          ಹೋಗು ಹೊರಹೋಗು;
        }
        sslerror = ERR_get_error();
        ಆದರೆ(sslerror)
          ossl_strerror(sslerror, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್));
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sockerr)
          curlx_strerror(sockerr, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್));
        ಇಲ್ಲದಿದ್ದರೆ
          curl_msnprintf(error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್), "%s",
                         SSL_ERROR_ಗಮ್ಯ_str(err));

        failf(ದತ್ತಾಂಶ, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_ಬಫರ್, sockerr);
        ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಸ್ಥಿತಿ SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(ದತ್ತಾಂಶ, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್)));
        ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಪೂರ್ವನಿಯೋಜಿತ:
        /* a true error */
        failf(ದತ್ತಾಂಶ, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_ಗಮ್ಯ_str(err), SOCKERRNO);
        ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(ದತ್ತಾಂಶ, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* HAVE_OPENSSL_EARLYDATA */

ಸ್ಥಿರ CURLcode ossl_connect(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;

  /* check if the connection has already been established */
  ಆದರೆ(ssl_connection_complete == connssl->state) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ಆದರೆ(connssl->connecting_state == ssl_connect_1) {
    ಆದರೆ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(ದತ್ತಾಂಶ, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "need HTTPS-RR, delaying connect");
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ossl_connect, step1");
    ಫಲಿತಾಂಶ = ossl_connect_step1(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ossl_connect, step2");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL_EARLYDATA
    ಆದರೆ(connssl->earlydata_state == ssl_earlydata_await) {
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(connssl->earlydata_state == ssl_earlydata_sending) {
      ಫಲಿತಾಂಶ = ossl_send_earlydata(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      connssl->earlydata_state = ssl_earlydata_ಕಳುಹಿಸಿದ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_ಕಳುಹಿಸಿದ));

    ಫಲಿತಾಂಶ = ossl_connect_step2(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ossl_connect, step3");
    ಫಲಿತಾಂಶ = ossl_connect_step3(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    connssl->connecting_state = ssl_connect_ಮುಗಿದ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL_EARLYDATA
    ಆದರೆ(connssl->earlydata_state > ssl_earlydata_none) {
      ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_ಕಳುಹಿಸಿದ);
      connssl->earlydata_state =
        (SSL_get_early_ದತ್ತಾಂಶ_ಸ್ಥಿತಿ(octx->ssl) == SSL_EARLY_ದತ್ತಾಂಶ_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_ಮುಗಿದ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    *ಮುಗಿದ = FALSE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  *ಮುಗಿದ = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool ossl_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ (bool)connssl->ಒಳಹರಿವು_pending;
}

ಸ್ಥಿರ CURLcode ossl_send(ರಚನೆ Curl_cfilter *cf,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  ಪೂರ್ಣಾಂಕ err;
  ಅಕ್ಷರ error_ಬಫರ್[256];
  sslerr_t sslerror;
  ಪೂರ್ಣಾಂಕ memlen;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ : (ಪೂರ್ಣಾಂಕ)len;
  ಆದರೆ(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    ಆದರೆ(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  ಆದರೆ(nwritten > 0) {
    ಆದರೆ(nwritten > memlen) {
      failf(ದತ್ತಾಂಶ, "SSL_write(len=%d) returned %d", memlen, nwritten);
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    *pnwritten = (size_t)nwritten;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    err = SSL_get_error(octx->ssl, nwritten);

    ಆಯ್ಕೆ(err) {
    ಸ್ಥಿತಿ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ಹೋಗು ಹೊರಹೋಗು;
    ಸ್ಥಿತಿ SSL_ERROR_WANT_WRITE:
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      ಹೋಗು ಹೊರಹೋಗು;
    ಸ್ಥಿತಿ SSL_ERROR_SYSCALL: {
      ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;

      ಆದರೆ(octx->io_ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        ಫಲಿತಾಂಶ = CURLE_AGAIN;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      sslerror = ERR_get_error();
      ಆದರೆ(sslerror)
        ossl_strerror(sslerror, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್));
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sockerr)
        curlx_strerror(sockerr, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್));
      ಇಲ್ಲದಿದ್ದರೆ
        curl_msnprintf(error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್), "%s",
                       SSL_ERROR_ಗಮ್ಯ_str(err));

      failf(ದತ್ತಾಂಶ, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_ಬಫರ್, sockerr);
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಸ್ಥಿತಿ SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(ದತ್ತಾಂಶ, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್)));
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* a true error */
      failf(ದತ್ತಾಂಶ, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_ಗಮ್ಯ_str(err), SOCKERRNO);
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ossl_recv(ರಚನೆ Curl_cfilter *cf,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,   /* transfer */
                          ಅಕ್ಷರ *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  ಅಕ್ಷರ error_ಬಫರ್[256];
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sslerror;
  ಪೂರ್ಣಾಂಕ buffsize;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) ? ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ : (ಪೂರ್ಣಾಂಕ)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  ಆದರೆ(nread > 0)
    *pnread = (size_t)nread;
  ಇಲ್ಲದಿದ್ದರೆ {
    /* failed SSL_read */
    ಪೂರ್ಣಾಂಕ err = SSL_get_error(octx->ssl, nread);

    ಆಯ್ಕೆ(err) {
    ಸ್ಥಿತಿ SSL_ERROR_NONE: /* this is not an error */
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      ಆದರೆ(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "TLS close_notify");
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ಹೋಗು ಹೊರಹೋಗು;
    ಸ್ಥಿತಿ SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ಹೋಗು ಹೊರಹೋಗು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      ಆದರೆ(octx->io_ಫಲಿತಾಂಶ == CURLE_AGAIN) {
        ಫಲಿತಾಂಶ = CURLE_AGAIN;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      sslerror = ERR_get_error();
      ಆದರೆ((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
ಸರತಿ*/
        ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
        ಆದರೆ(sslerror)
          ossl_strerror(sslerror, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್));
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್));
        ಇಲ್ಲದಿದ್ದರೆ
          curl_msnprintf(error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್), "%s",
                         SSL_ERROR_ಗಮ್ಯ_str(err));
        failf(ದತ್ತಾಂಶ, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_ಬಫರ್, sockerr);
        ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(err == SSL_ERROR_SYSCALL) {
        ಆದರೆ(octx->io_ಫಲಿತಾಂಶ) {
          /* logging handling in underlying filter already */
          ಫಲಿತಾಂಶ = octx->io_ಫಲಿತಾಂಶ;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(connssl->peer_closed) {
          failf(ದತ್ತಾಂಶ, "Connection closed abruptly");
          ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
          ಆದರೆ(sockerr)
            curlx_strerror(sockerr, error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್));
          ಇಲ್ಲದಿದ್ದರೆ {
            curl_msnprintf(error_ಬಫರ್, ಗಾತ್ರ(error_ಬಫರ್),
                           "Connection closed abruptly");
          }
          failf(ದತ್ತಾಂಶ, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_ಬಫರ್, sockerr);
          ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
        }
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
  }

ಹೊರಹೋಗು:
  ಆದರೆ((!ಫಲಿತಾಂಶ && !*pnread) || (ಫಲಿತಾಂಶ == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->ಒಳಹರಿವು_pending = FALSE;
  }
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread, connssl->ಒಳಹರಿವು_pending);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ossl_get_channel_binding(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         int8_t sockindex,
                                         ರಚನೆ dynbuf *binding)
{
  X509 *cert;
  ಪೂರ್ಣಾಂಕ mdnid;
  bool no_digest_acceptable = FALSE;
  ಸ್ಥಿರಾಂಕ EVP_MD *algo_type = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *algo_name = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[EVP_MAX_MD_ಗಾತ್ರ];

  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಪೂರ್ವಪ್ರತ್ಯಯ[] = "tls-server-end-point:";
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  ರಚನೆ Curl_cfilter *cf = conn->cfilter[sockindex];
  ರಚನೆ ossl_ctx *octx = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಮಾಡು {
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cftype *cft = cf->cft;
    ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;

    ಆದರೆ(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (ರಚನೆ ossl_ctx *)connssl->backend;
      ನಿಲ್ಲಿಸು;
    }

    cf = cf->ಮುಂದಿನ;
  } ಆಗ(cf);

  ಆದರೆ(!octx) {
    failf(ದತ್ತಾಂಶ, "Failed to find the SSL filter");
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  ಆದರೆ(!cert)
    /* No server certificate, do not do channel binding */
    ಹಿಂತಿರುಗಿ CURLE_OK;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_OPENSSL3
  {
    ಪೂರ್ಣಾಂಕ pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    ಆದರೆ(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(ದತ್ತಾಂಶ, "certificate signature algorithm not recognized");
      ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಆದರೆ(mdnid != NID_undef) {
      ಆದರೆ(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      ಇಲ್ಲದಿದ್ದರೆ
        algo_type = EVP_get_digestbynid(mdnid);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      ಅಕ್ಷರ mdname[128] = "";
      ಪೂರ್ಣಾಂಕ rc = EVP_PKEY_get_default_digest_name(pkey, mdname, ಗಾತ್ರ(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      ಆದರೆ(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(ದತ್ತಾಂಶ, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ /* HAVE_OPENSSL3 */

  ಆದರೆ(!OBJ_ಕಂಡುಹಿಡಿ_sigid_algs(X509_get_signature_nid(cert), &mdnid, ಖಾಲಿ)) {
    failf(ದತ್ತಾಂಶ,
          "Unable to find digest NID for certificate signature algorithm");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  ಆದರೆ(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    algo_type = EVP_get_digestbynid(mdnid);
    ಆದರೆ(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(ದತ್ತಾಂಶ, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

#ಆದರೆ_ಅಂತ್ಯ /* HAVE_OPENSSL3, else */

  ಆದರೆ(!algo_type) {
    ಆದರೆ(no_digest_acceptable) {
      infof(ದತ್ತಾಂಶ, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      ಫಲಿತಾಂಶ = CURLE_OK;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(ದತ್ತಾಂಶ, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(!X509_digest(cert, algo_type, buf, &ಉದ್ದಳತೆ)) {
    failf(ದತ್ತಾಂಶ, "X509_digest() failed for channel binding");
    ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  /* Append "tls-server-end-point:" */
  ಫಲಿತಾಂಶ = curlx_dyn_addn(binding, ಪೂರ್ವಪ್ರತ್ಯಯ, CURL_CSTRLEN(ಪೂರ್ವಪ್ರತ್ಯಯ));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  /* Append digest */
  ಫಲಿತಾಂಶ = curlx_dyn_addn(binding, buf, ಉದ್ದಳತೆ);

ಹೊರಹೋಗು:
  X509_free(cert);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

size_t Curl_ossl_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ
  ಅಕ್ಷರ *p;
  size_t ಎಣಿಕೆ_2;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ver = Open_SSL_ಆವೃತ್ತಿ(OPENSSL_ಆವೃತ್ತಿ);
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  ಆದರೆ(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  ಎಣಿಕೆ_2 = curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, "%s/%s", OSSL_PACKAGE, ver);
  ಗಾಗಿ(p = ಬಫರ್; *p; ++p) {
    ಆದರೆ(ISBLANK(*p))
      *p = '_';
  }
  ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
#ಅಥವಾಆದರೆ defined(OPENSSL_IS_AWSLC)
  ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, "%s/%s",
                        OSSL_PACKAGE, AWSLC_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ_STRING);
#ಅಥವಾಆದರೆ defined(OPENSSL_IS_BORINGSSL)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_BORINGSSL_ಆವೃತ್ತಿ
  ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_ಆವೃತ್ತಿ);
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, OSSL_PACKAGE);
#ಆದರೆ_ಅಂತ್ಯ
#ಇಲ್ಲದಿದ್ದರೆ /* OpenSSL 3+ */
  ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, "%s/%s",
                        OSSL_PACKAGE, Open_SSL_ಆವೃತ್ತಿ(OPENSSL_ಆವೃತ್ತಿ_STRING));
#ಆದರೆ_ಅಂತ್ಯ
}

/* can be called with data == NULL */
ಸ್ಥಿರ CURLcode ossl_random(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *entropy, size_t ಉದ್ದಳತೆ)
{
  ಪೂರ್ಣಾಂಕ rc;
  ಆದರೆ(ದತ್ತಾಂಶ) {
    ಆದರೆ(ossl_ಬೀಜ(ದತ್ತಾಂಶ)) /* Initiate the seed if not already done */
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(!rand_enough())
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_ಬೈಟುಗಳು(entropy, (ossl_valsize_t)curlx_uztosi(ಉದ್ದಳತೆ));
  ಹಿಂತಿರುಗಿ rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

ಸ್ಥಿರ CURLcode ossl_sha256sum(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು,
                               size_t len,
                               ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha256sum /* output */,
                               size_t ಬಳಸದ)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (ಶೂನ್ಯ)ಬಳಸದ;

  mdctx = EVP_MD_CTX_new();
  ಆದರೆ(!mdctx)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಆದರೆ(!EVP_DigestInit_ex(mdctx, EVP_sha256(), ಖಾಲಿ)) {
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(!EVP_DigestUpdate(mdctx, ಒಳಹರಿವು, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, ಖಾಲಿ))
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
ಹೊರಹೋಗು:
  EVP_MD_CTX_free(mdctx);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool ossl_cert_ಸ್ಥಿತಿ_request(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPENSSL_NO_OCSP
  ಹಿಂತಿರುಗಿ TRUE;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ *ossl_get_internals(ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  ರಚನೆ ossl_ctx *octx = (ರಚನೆ ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  ಹಿಂತಿರುಗಿ info == CURLINFO_TLS_SESSION ?
    (ಶೂನ್ಯ *)octx->ssl_ctx : (ಶೂನ್ಯ *)octx->ssl;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_openssl = {
  { CURLSSLBACKEND_OPENSSL, "openssl" }, /* info */

  SSLSUPP_CA_ಮಾರ್ಗ |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_SSL_CTX |
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_CTX_ಗಣ_CIPHERSUITES
  SSLSUPP_TLS13_CIPHERSUITES |
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_CTX_SET1_SIGALGS
  SSLSUPP_SIGNATURE_ALGORITHMS |
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SSL_SET1_ECH_CONFIG_LIST
  SSLSUPP_ECH |
#ಆದರೆ_ಅಂತ್ಯ
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_ISSUERCERT_BLOB |
  SSLSUPP_SSL_EC_CURVES |
  SSLSUPP_CRLFILE,

  ಗಾತ್ರ(ರಚನೆ ossl_ctx),

  ossl_init,                /* init */
  ossl_cleanup,             /* cleanup */
  Curl_ossl_ಆವೃತ್ತಿ,        /*ಆವೃತ್ತಿ*/
  ossl_shutdown,            /* shutdown */
  ossl_ದತ್ತಾಂಶ_pending,        /* data_pending */
  ossl_random,              /* random */
  ossl_cert_ಸ್ಥಿತಿ_request, /* cert_status_request */
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

#ಆದರೆ_ಅಂತ್ಯ /* USE_OPENSSL */
