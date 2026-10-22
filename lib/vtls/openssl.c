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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_OPENSSL

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "formdata.h" /* for the boundary function */
#ଅନ୍ତର୍ଭାଵ "url.h" /* for the SSL config check function */
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "vtls/openssl.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_int.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "vtls/keylog.h"
#ଅନ୍ତର୍ଭାଵ "vtls/hostcheck.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "multiif.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strerr.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"
#ଅନ୍ତର୍ଭାଵ "vtls/apple.h"
#ifdef USE_ECH
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#endif

#ଅନ୍ତର୍ଭାଵ <openssl/rand.h>
#ଅନ୍ତର୍ଭାଵ <openssl/x509v3.h>
#ifndef OPENSSL_NO_DSA
#ଅନ୍ତର୍ଭାଵ <openssl/dsa.h>
#endif
#ଅନ୍ତର୍ଭାଵ <openssl/dh.h>
#ଅନ୍ତର୍ଭାଵ <openssl/err.h>
#ଅନ୍ତର୍ଭାଵ <openssl/conf.h>
#ଅନ୍ତର୍ଭାଵ <openssl/bn.h>
#ଅନ୍ତର୍ଭାଵ <openssl/rsa.h>
#ଅନ୍ତର୍ଭାଵ <openssl/bio.h>
#ଅନ୍ତର୍ଭାଵ <openssl/pkcs12.h>
#ଅନ୍ତର୍ଭାଵ <openssl/tls1.h>
#ଅନ୍ତର୍ଭାଵ <openssl/evp.h>

#ଯଦି defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#ଅନ୍ତର୍ଭାଵ <openssl/ech.h>
#endif

#ifndef OPENSSL_NO_OCSP
#ଅନ୍ତର୍ଭାଵ <openssl/ocsp.h>
#endif

#ଯଦି !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#define USE_OPENSSL_ENGINE
#ଅନ୍ତର୍ଭାଵ <openssl/engine.h>
#endif

#ifdef LIBRESSL_VERSION_ସଂଖ୍ୟା
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  ଯଦି LIBRESSL_VERSION_ସଂଖ୍ୟା < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#ନଚେତଯଦି !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#endif

#ଯଦି defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#ଅନ୍ତର୍ଭାଵ <openssl/provider.h>
#ଅନ୍ତର୍ଭାଵ <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#define OPENSSL_HAS_PROVIDERS
#endif

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#ଯଦି !defined(LIBRESSL_VERSION_ସଂଖ୍ୟା) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_ଚାବି_EXPLICIT_PARAMS))
#define HAVE_SSL_CTX_SET_DEFAULT_READ_BUFFER_LEN 1
#endif

#ଯଦି defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#ଅନ୍ତର୍ଭାଵ <openssl/ui.h>
#endif

#ifdef HAVE_OPENSSL3
#define HAVE_EVP_PKEY_GET_PARAMS 1
#endif

#ifdef HAVE_EVP_PKEY_GET_PARAMS
#ଅନ୍ତର୍ଭାଵ <openssl/core_names.h>
#define DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = ଖାଲି
#define FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#ଅନ୍ୟଥା
#define DECLARE_PKEY_PARAM_BIGNUM(name) ଅଚଳ BIGNUM *name
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
#ifndef LIBRESSL_VERSION_ସଂଖ୍ୟା
#define HAVE_SSL_CTX_SET1_SIGALGS
#endif

#ifdef LIBRESSL_VERSION_ସଂଖ୍ୟା
#define OSSL_PACKAGE "LibreSSL"
#ନଚେତଯଦି defined(OPENSSL_IS_AWSLC)
#define OSSL_PACKAGE "AWS-LC"
#ନଚେତଯଦି defined(OPENSSL_IS_BORINGSSL)
#define OSSL_PACKAGE "BoringSSL"
#ନଚେତଯଦି defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#define OSSL_PACKAGE "quictls"
#ଅନ୍ୟଥା
#define OSSL_PACKAGE "OpenSSL"
#endif

#ifdef HAVE_BORINGSSL_LIKE
ପ୍ରକାରବ୍ୟାଖ୍ୟା size_t numcert_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା uint32_t sslerr_t;
#ଅନ୍ୟଥା
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା numcert_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned long sslerr_t;
#endif
#define ossl_valsize_t numcert_t

#ଯଦି OPENSSL_VERSION_ସଂଖ୍ୟା >= 0x40100000L
#define ossl_ASN1_STRING_get_length ASN1_STRING_get_length
#ଅନ୍ୟଥା
ସ୍ଥିର size_t ossl_ASN1_STRING_get_length(ଅଚଳ ASN1_STRING *str)
{
   ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା length = ASN1_STRING_length(str);
   ଫେରନ୍ତୁ length >= 0 ? (size_t)length : 0;
}
#endif

ସ୍ଥିର CURLcode ଠେଲ_certinfo(ସଂରଚନା Curl_easy *data,
                              BIO *mem, ଅଚଳ ଅକ୍ଷର *label, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ)
  WARN_UNUSED_RESULT;

ସ୍ଥିର CURLcode ଠେଲ_certinfo(ସଂରଚନା Curl_easy *data,
                              BIO *mem, ଅଚଳ ଅକ୍ଷର *label, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ)
{
  ଅକ୍ଷର *ptr;
  long len = BIO_get_mem_data(mem, &ptr);
  CURLcode result = Curl_ssl_ଠେଲ_certinfo_len(data, ଅଙ୍କ, label, ptr, len);
  (ଶୂନ୍ୟ)BIO_reset(mem);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode pubkey_show(ସଂରଚନା Curl_easy *data,
                            BIO *mem,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ,
                            ଅଚଳ ଅକ୍ଷର *type,
                            ଅଚଳ ଅକ୍ଷର *name,
                            ଅଚଳ BIGNUM *bn) WARN_UNUSED_RESULT;

ସ୍ଥିର CURLcode pubkey_show(ସଂରଚନା Curl_easy *data,
                            BIO *mem,
                            ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ,
                            ଅଚଳ ଅକ୍ଷର *type,
                            ଅଚଳ ଅକ୍ଷର *name,
                            ଅଚଳ BIGNUM *bn)
{
  ଅକ୍ଷର namebuf[32];

  curl_msnprintf(namebuf, ଆକାର(namebuf), "%s(%s)", type, name);

  ଯଦି(bn)
    BN_print(mem, bn);
  ଫେରନ୍ତୁ ଠେଲ_certinfo(data, mem, namebuf, ଅଙ୍କ);
}

#define print_pubkey_BN(_type, _name, ଅଙ୍କ_2)           \
  pubkey_show(data, mem, ଅଙ୍କ_2, #_type, #_name, _name)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା asn1_object_dump(ଅଚଳ ASN1_OBJECT *a, ଅକ୍ଷର *buf, size_t len)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = i2t_ASN1_OBJECT(buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, a);
  ଫେରନ୍ତୁ (i >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);  /* buffer too small */
}

ସ୍ଥିର CURLcode X509V3_ext(ସଂରଚନା Curl_easy *data,
                           ପୂର୍ଣ୍ଣସଂଖ୍ୟା certnum,
                           ଅଚଳ ଗଦା_OF(X509_EXTENSION) *extsarg)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  CURLcode result = CURLE_OK;
#ifdef LIBRESSL_VERSION_ସଂଖ୍ୟା
  ଗଦା_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#ଅନ୍ୟଥା
  ଅଚଳ ଗଦା_OF(X509_EXTENSION) *exts = extsarg;
#endif

  ଯଦି((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sk_X509_EXTENSION_ଅଙ୍କ(exts) <= 0)
    /* no extensions, bail out */
    ଫେରନ୍ତୁ result;

  ପାଇଁ(i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sk_X509_EXTENSION_ଅଙ୍କ(exts); i++) {
    ଅଚଳ ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_ମୂଲ୍ୟ(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    ଅକ୍ଷର namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    ଯଦି(!bio_out)
      ଫେରନ୍ତୁ result;

    obj = X509_EXTENSION_get_object(ext);

    ଯଦି(asn1_object_dump(obj, namebuf, ଆକାର(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    ଯଦି(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (ଅଚଳ ASN1_STRING *)X509_EXTENSION_get_data(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    result = Curl_ssl_ଠେଲ_certinfo_len(data, certnum, namebuf, biomem->data,
                                        biomem->length);
    BIO_free(bio_out);
    ଯଦି(result)
      ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode get_pkey_rsa(ସଂରଚନା Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
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
#ଅନ୍ୟଥା
  RSA_get0_ଚାବି(rsa, &n, &e, ଖାଲି);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(n ? BN_ଅଙ୍କ_bits(n) : 0));
  result = ଠେଲ_certinfo(data, mem, "RSA Public Key", i);
  ଯଦି(!result) {
    result = print_pubkey_BN(rsa, n, i);
    ଯଦି(!result)
      result = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  ଫେରନ୍ତୁ result;
}

#ifndef OPENSSL_NO_DSA
ସ୍ଥିର CURLcode get_pkey_dsa(ସଂରଚନା Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
  CURLcode result = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ଚାବି);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ଚାବି, &pub_ଚାବି);
#ଅନ୍ୟଥା
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_ଚାବି(dsa, &pub_ଚାବି, ଖାଲି);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dsa, p, i);
  ଯଦି(!result)
    result = print_pubkey_BN(dsa, q, i);
  ଯଦି(!result)
    result = print_pubkey_BN(dsa, g, i);
  ଯଦି(!result)
    result = print_pubkey_BN(dsa, pub_ଚାବି, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ଚାବି);
  ଫେରନ୍ତୁ result;
}
#endif /* !OPENSSL_NO_DSA */

ସ୍ଥିର CURLcode get_pkey_dh(ସଂରଚନା Curl_easy *data,
                            EVP_PKEY *pubkey, BIO *mem, ପୂର୍ଣ୍ଣସଂଖ୍ୟା i)
{
  CURLcode result;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ଚାବି);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ଚାବି, &pub_ଚାବି);
#ଅନ୍ୟଥା
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_ଚାବି(dh, &pub_ଚାବି, ଖାଲି);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dh, p, i);
  ଯଦି(!result)
    result = print_pubkey_BN(dh, q, i);
  ଯଦି(!result)
    result = print_pubkey_BN(dh, g, i);
  ଯଦି(!result)
    result = print_pubkey_BN(dh, pub_ଚାବି, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ଚାବି);
  ଫେରନ୍ତୁ result;
}

#ifdef HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଅଚଳ X509_PUBKEY pubkeytype_t;
#ଅନ୍ୟଥା
ପ୍ରକାରବ୍ୟାଖ୍ୟା X509_PUBKEY pubkeytype_t;
#endif

ସ୍ଥିର CURLcode ossl_certchain(ସଂରଚନା Curl_easy *data, SSL *ssl)
{
  CURLcode result;
  ଗଦା_OF(X509) *sk;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  ଯଦି(!sk)
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_ଅଙ୍କ(sk);
  ଯଦି(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(data, "%d certificates is more than allowed (%d)", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  result = Curl_ssl_init_certinfo(data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)numcerts);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  mem = BIO_new(BIO_s_mem());
  ଯଦି(!mem)
    result = CURLE_OUT_OF_MEMORY;

  ପାଇଁ(i = 0; !result && (i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)numcerts); i++) {
    X509 *x = sk_X509_ମୂଲ୍ୟ(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = ଖାଲି;
    size_t j;
    ଅଚଳ ASN1_BIT_STRING *psig = ଖାଲି;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    result = ଠେଲ_certinfo(data, mem, "Subject", i);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    result = ଠେଲ_certinfo(data, mem, "Issuer", i);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    BIO_printf(mem, "%lx", (unsigned long)X509_get_version(x));
    result = ଠେଲ_certinfo(data, mem, "Version", i);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    {
      ASN1_INTEGER *ଅଙ୍କ = X509_get_serial_ସଂଖ୍ୟା(x);
      ଅଚଳ unsigned ଅକ୍ଷର *numdata = ASN1_STRING_get0_data(ଅଙ୍କ);
      ଅଚଳ size_t numlen = ossl_ASN1_STRING_get_length(ଅଙ୍କ);
      ଯଦି(ASN1_STRING_type(ଅଙ୍କ) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      ପାଇଁ(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    result = ଠେଲ_certinfo(data, mem, "Serial Number", i);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    {
      ଅଚଳ X509_ALGOR *sigalg = ଖାଲି;
      pubkeytype_t *xpubkey = ଖାଲି;
      ASN1_OBJECT *pubkeyoid = ଖାଲି;

      X509_get0_signature(&psig, &sigalg, x);
      ଯଦି(sigalg) {
        ଅଚଳ ASN1_OBJECT *sigalgoid = ଖାଲି;
        X509_ALGOR_get0(&sigalgoid, ଖାଲି, ଖାଲି, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        result = ଠେଲ_certinfo(data, mem, "Signature Algorithm", i);
        ଯଦି(result)
          ଭାଙ୍ଗ;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      ଯଦି(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, ଖାଲି, ଖାଲି, ଖାଲି, xpubkey);
        ଯଦି(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          result = ଠେଲ_certinfo(data, mem, "Public Key Algorithm", i);
          ଯଦି(result)
            ଭାଙ୍ଗ;
        }
      }

      result = X509V3_ext(data, i, X509_get0_extensions(x));
      ଯଦି(result)
        ଭାଙ୍ଗ;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    result = ଠେଲ_certinfo(data, mem, "Start date", i);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    result = ଠେଲ_certinfo(data, mem, "Expire date", i);
    ଯଦି(result)
      ଭାଙ୍ଗ;

    pubkey = X509_get_pubkey(x);
    ଯଦି(!pubkey)
      infof(data, "   Unable to load public key");
    ଅନ୍ୟଥା {
      ବାଛ(EVP_PKEY_id(pubkey)) {
      ସ୍ଥିତି EVP_PKEY_RSA:
        result = get_pkey_rsa(data, pubkey, mem, i);
        ଭାଙ୍ଗ;

#ifndef OPENSSL_NO_DSA
      ସ୍ଥିତି EVP_PKEY_DSA:
        result = get_pkey_dsa(data, pubkey, mem, i);
        ଭାଙ୍ଗ;
#endif

      ସ୍ଥିତି EVP_PKEY_DH:
        result = get_pkey_dh(data, pubkey, mem, i);
        ଭାଙ୍ଗ;
      }
      EVP_PKEY_free(pubkey);
    }

    ଯଦି(!result && psig) {
      ଅଚଳ unsigned ଅକ୍ଷର *psigdata = ASN1_STRING_get0_data(psig);
      ଅଚଳ size_t psiglen = ossl_ASN1_STRING_get_length(psig);
      ପାଇଁ(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      result = ଠେଲ_certinfo(data, mem, "Signature", i);
    }

    ଯଦି(!result) {
      PEM_write_bio_X509(mem, x);
      result = ଠେଲ_certinfo(data, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  ଯଦି(result)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(data);

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_data(bio, ଖାଲି);
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_bio_cf_destroy(BIO *bio)
{
  ଯଦି(!bio)
    ଫେରନ୍ତୁ 0;
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର long ossl_bio_cf_ctrl(BIO *bio, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, long ଅଙ୍କ, ଶୂନ୍ୟ *ptr)
{
  ସଂରଚନା Curl_cfilter *cf = BIO_get_data(bio);
  long ret = 1;

  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)ptr;
  ବାଛ(cmd) {
  ସ୍ଥିତି BIO_CTRL_GET_CLOSE:
    ret = (long)BIO_get_shutdown(bio);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି BIO_CTRL_DUP:
    ret = 1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
    ଫେରନ୍ତୁ connssl->peer_closed;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ret = 0;
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_bio_cf_out_write(BIO *bio, ଅଚଳ ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା blen)
{
  ସଂରଚନା Curl_cfilter *cf = BIO_get_data(bio);
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  ଯଦି(blen < 0)
    ଫେରନ୍ତୁ 0;

  result = Curl_conn_cf_send(cf->next, data,
                             (ଅଚଳ uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    ଫେରନ୍ତୁ -1;
  }
  ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nwritten;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_bio_cf_in_read(BIO *bio, ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା blen)
{
  ସଂରଚନା Curl_cfilter *cf = BIO_get_data(bio);
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result, r2;

  DEBUGASSERT(data);
  /* OpenSSL catches this case, so should we. */
  ଯଦି(!buf)
    ଫେରନ୍ତୁ 0;
  ଯଦି(blen < 0)
    ଫେରନ୍ତୁ 0;

  result = Curl_conn_cf_recv(cf->next, data, buf, (size_t)blen, &nread);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nread);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  ଅନ୍ୟଥା {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->ନିବେଶ_pending = TRUE;
    ଯଦି(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  ଯଦି(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, data, octx);
    ଯଦି(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_result = r2;
      ଫେରନ୍ତୁ -1;
    }
    octx->x509_store_setup = TRUE;
  }
  ଫେରନ୍ତୁ result ? -1 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nread;
}

ସ୍ଥିର BIO_METHOD *ossl_bio_cf_method_create(ଶୂନ୍ୟ)
{
  BIO_METHOD *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  ଯଦି(m) {
    BIO_meth_set_write(m, &ossl_bio_cf_out_write);
    BIO_meth_set_read(m, &ossl_bio_cf_in_read);
    BIO_meth_set_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_set_create(m, &ossl_bio_cf_create);
    BIO_meth_set_destroy(m, &ossl_bio_cf_destroy);
  }
  ଫେରନ୍ତୁ m;
}

ସ୍ଥିର ଶୂନ୍ୟ ossl_bio_cf_method_free(BIO_METHOD *m)
{
  ଯଦି(m)
    BIO_meth_free(m);
}

#ifndef HAVE_KEYLOG_UPSTREAM
#ifdef HAVE_KEYLOG_CALLBACK
ସ୍ଥିର ଶୂନ୍ୟ ossl_keylog_callback(ଅଚଳ SSL *ssl, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
  (ଶୂନ୍ୟ)ssl;

  Curl_tls_keylog_write_ଧାଡ଼ି(ଧାଡ଼ି);
}
#ଅନ୍ୟଥା
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
ସ୍ଥିର ଶୂନ୍ୟ ossl_log_tls12_secret(ଅଚଳ SSL *ssl, bool *keylog_done)
{
  ଅଚଳ SSL_SESSION *session;
  unsigned ଅକ୍ଷର client_random[SSL3_RANDOM_SIZE];
  unsigned ଅକ୍ଷର master_ଚାବି[SSL_MAX_MASTER_ଚାବି_LENGTH];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା master_ଚାବି_length = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  ଯଦି(!session || *keylog_done) {
    ERR_କାଢ଼_to_mark();
    ଫେରନ୍ତୁ;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_SIZE);
  master_ଚାବି_length = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
    SSL_SESSION_get_master_ଚାବି(session, master_ଚାବି, SSL_MAX_MASTER_ଚାବି_LENGTH);

  ERR_କାଢ଼_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  ଯଦି(master_ଚାବି_length <= 0)
    ଫେରନ୍ତୁ;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        ଆକାର(client_random),
                        master_ଚାବି, master_ଚାବି_length);
}
#endif /* !HAVE_KEYLOG_CALLBACK */
#endif /* HAVE_KEYLOG_UPSTREAM */

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *SSL_ERROR_to_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା err)
{
  ବାଛ(err) {
  ସ୍ଥିତି SSL_ERROR_NONE:
    ଫେରନ୍ତୁ "SSL_ERROR_NONE";
  ସ୍ଥିତି SSL_ERROR_SSL:
    ଫେରନ୍ତୁ "SSL_ERROR_SSL";
  ସ୍ଥିତି SSL_ERROR_WANT_READ:
    ଫେରନ୍ତୁ "SSL_ERROR_WANT_READ";
  ସ୍ଥିତି SSL_ERROR_WANT_WRITE:
    ଫେରନ୍ତୁ "SSL_ERROR_WANT_WRITE";
  ସ୍ଥିତି SSL_ERROR_WANT_X509_LOOKUP:
    ଫେରନ୍ତୁ "SSL_ERROR_WANT_X509_LOOKUP";
  ସ୍ଥିତି SSL_ERROR_SYSCALL:
    ଫେରନ୍ତୁ "SSL_ERROR_SYSCALL";
  ସ୍ଥିତି SSL_ERROR_ZERO_RETURN:
    ଫେରନ୍ତୁ "SSL_ERROR_ZERO_RETURN";
  ସ୍ଥିତି SSL_ERROR_WANT_CONNECT:
    ଫେରନ୍ତୁ "SSL_ERROR_WANT_CONNECT";
  ସ୍ଥିତି SSL_ERROR_WANT_ACCEPT:
    ଫେରନ୍ତୁ "SSL_ERROR_WANT_ACCEPT";
#ifdef SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ସ୍ଥିତି SSL_ERROR_WANT_ASYNC:
    ଫେରନ୍ତୁ "SSL_ERROR_WANT_ASYNC";
#endif
#ifdef SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ସ୍ଥିତି SSL_ERROR_WANT_ASYNC_JOB:
    ଫେରନ୍ତୁ "SSL_ERROR_WANT_ASYNC_JOB";
#endif
#ifdef SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  ସ୍ଥିତି SSL_ERROR_WANT_CLIENT_HELLO_CB:
    ଫେରନ୍ତୁ "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
ସ୍ଥିର ଅକ୍ଷର *ossl_strerror(unsigned long error, ଅକ୍ଷର *buf, size_t size)
{
  size_t len;
  DEBUGASSERT(size);
  *buf = '\0';

  len = Curl_ossl_version(buf, size);
  DEBUGASSERT(len < (size - 2));
  ଯଦି(len < (size - 2)) {
    buf += len;
    size -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ifdef HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, size);
#ଅନ୍ୟଥା
  ERR_error_string_n(error, buf, size);
#endif

  ଯଦି(!*buf) {
    ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, size, ସନ୍ଦେଶ, strlen(ସନ୍ଦେଶ));
  }

  ଫେରନ୍ତୁ buf;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା passwd_callback(ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା encrypting, ଶୂନ୍ୟ *password)
{
  DEBUGASSERT(encrypting == 0);

  ଯଦି(!encrypting && ଅଙ୍କ >= 0 && password) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା klen = curlx_uztosi(strlen((ଅକ୍ଷର *)password));
    ଯଦି(ଅଙ୍କ > klen) {
      memcpy(buf, password, klen + 1);
      ଫେରନ୍ତୁ klen;
    }
  }
  ଫେରନ୍ତୁ 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
ସ୍ଥିର bool rand_enough(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ RAND_status() != 0;
}

ସ୍ଥିର CURLcode ossl_seed(ସଂରଚନା Curl_easy *data)
{
  /* This might get called before it has been added to a multi handle */
  ଯଦି(data->multi && data->multi->ssl_seeded)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    ଯଦି(data->multi)
      data->multi->ssl_seeded = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  failf(data, "Insufficient randomness");
  ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_do_file_type(ଅଚଳ ଅକ୍ଷର *type)
{
  ଯଦି(!type || !type[0])
    ଫେରନ୍ତୁ SSL_FILETYPE_PEM;
  ଯଦି(curl_strequal(type, "PEM"))
    ଫେରନ୍ତୁ SSL_FILETYPE_PEM;
  ଯଦି(curl_strequal(type, "DER"))
    ଫେରନ୍ତୁ SSL_FILETYPE_ASN1;
  ଯଦି(curl_strequal(type, "PROV"))
    ଫେରନ୍ତୁ SSL_FILETYPE_PROVIDER;
  ଯଦି(curl_strequal(type, "ENG"))
    ଫେରନ୍ତୁ SSL_FILETYPE_ENGINE;
  ଯଦି(curl_strequal(type, "P12"))
    ଫେରନ୍ତୁ SSL_FILETYPE_PKCS12;
  ଫେରନ୍ତୁ -1;
}

#ଯଦି defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ଅଚଳ ଅକ୍ଷର *password;
  ବାଛ(UI_get_string_type(uis)) {
  ସ୍ଥିତି UIT_PROMPT:
  ସ୍ଥିତି UIT_VERIFY:
    password = (ଅଚଳ ଅକ୍ଷର *)UI_get0_user_data(ui);
    ଯଦି(password && (UI_get_ନିବେଶ_flags(uis) & UI_ନିବେଶ_FLAG_DEFAULT_PWD)) {
      UI_set_result(ui, uis, password);
      ଫେରନ୍ତୁ 1;
    }
    FALLTHROUGH();
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  ବାଛ(UI_get_string_type(uis)) {
  ସ୍ଥିତି UIT_PROMPT:
  ସ୍ଥିତି UIT_VERIFY:
    ଯଦି(UI_get0_user_data(ui) &&
       (UI_get_ନିବେଶ_flags(uis) & UI_ନିବେଶ_FLAG_DEFAULT_PWD)) {
      ଫେରନ୍ତୁ 1;
    }
    FALLTHROUGH();
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
ସ୍ଥିର bool is_pkcs11_uri(ଅଚଳ ଅକ୍ଷର *string)
{
  ଫେରନ୍ତୁ string && curl_strnequal(string, "pkcs11:", 7);
}

#endif

ସ୍ଥିର CURLcode ossl_set_engine(ସଂରଚନା Curl_easy *data, ସଂରଚନା ossl_ctx *octx,
                                ଅଚଳ ଅକ୍ଷର *name);
#ifdef OPENSSL_HAS_PROVIDERS
ସ୍ଥିର CURLcode ossl_set_provider(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ossl_ctx *octx,
                                  ଅଚଳ ଅକ୍ଷର *iname);
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_certificate_blob(SSL_CTX *ctx, ଅଚଳ ସଂରଚନା curl_blob *blob,
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଅଚଳ ଅକ୍ଷର *ଚାବି_passwd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
  X509 *x = ଖାଲି;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)blob->len);
  ଯଦି(!in)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଯଦି(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, ଖାଲି);
  }
  ଅନ୍ୟଥା ଯଦି(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, ଖାଲି, passwd_callback, CURL_UNCONST(ଚାବି_passwd));
  }
  ଅନ୍ୟଥା {
    ret = 0;
    ଯାଅ end;
  }

  ଯଦି(!x) {
    ret = 0;
    ଯାଅ end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_privatekey_blob(SSL_CTX *ctx, ଅଚଳ ସଂରଚନା curl_blob *blob,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା type, ଅଚଳ ଅକ୍ଷର *ଚାବି_passwd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
  EVP_PKEY *pkey = ଖାଲି;
  BIO *in = BIO_new_mem_buf(blob->data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)blob->len);
  ଯଦି(!in)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଯଦି(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_Private_ଚାବି(in, ଖାଲି, passwd_callback,
                                   CURL_UNCONST(ଚାବି_passwd));
  ଅନ୍ୟଥା ଯଦି(type == SSL_FILETYPE_ASN1)
    pkey = d2i_Private_ଚାବି_bio(in, ଖାଲି);
  ଅନ୍ୟଥା
    ଯାଅ end;

  ଯଦି(!pkey)
    ଯାଅ end;

  ret = SSL_CTX_use_Private_ଚାବି(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_certificate_chain_blob(SSL_CTX *ctx,
                                      ଅଚଳ ସଂରଚନା curl_blob *blob,
                                      ଅଚଳ ଅକ୍ଷର *ଚାବି_passwd)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
  X509 *x = ଖାଲି;
  BIO *in = BIO_new_mem_buf(blob->data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)blob->len);
  ଯଦି(!in)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, ଖାଲି,
                            passwd_callback, CURL_UNCONST(ଚାବି_passwd));
  ଯଦି(!x)
    ଯାଅ end;

  ret = SSL_CTX_use_certificate(ctx, x);

  ଯଦି(ERR_peek_error() != 0)
    ret = 0;

  ଯଦି(ret) {
    X509 *ca;
    sslerr_t err;

    ଯଦି(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      ଯାଅ end;
    }

    ଯେତେବେଳେ((ca = PEM_read_bio_X509(in, ଖାଲି, passwd_callback,
                                  CURL_UNCONST(ଚାବି_passwd))) != ଖାଲି) {

      ଯଦି(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        ଯାଅ end;
      }
    }

    err = ERR_peek_last_error();
    ଯଦି((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_ଧାଡ଼ି))
      ERR_clear_error();
    ଅନ୍ୟଥା
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା enginecheck(ସଂରଚନା Curl_easy *data,
                       ସଂରଚନା ossl_ctx *octx,
                       ଅଚଳ ଅକ୍ଷର *ଚାବି_file,
                       ଅଚଳ ଅକ୍ଷର *ଚାବି_passwd)
{
#ifdef USE_OPENSSL_ENGINE
  EVP_PKEY *priv_ଚାବି = ଖାଲି;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  ଯଦି(!octx->p.engine && is_pkcs11_uri(ଚାବି_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    ଫେରନ୍ତୁ 0;

  ଯଦି(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    ଯଦି(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      ଫେରନ୍ତୁ 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);
    priv_ଚାବି = ENGINE_load_private_ଚାବି(octx->p.engine, ଚାବି_file,
                                       ui_method,
                                       CURL_UNCONST(ଚାବି_passwd));
    UI_destroy_method(ui_method);
    ଯଦି(!priv_ଚାବି) {
      failf(data, "failed to load private key from crypto engine");
      ଫେରନ୍ତୁ 0;
    }
    ଯଦି(SSL_CTX_use_Private_ଚାବି(ctx, priv_ଚାବି) != 1) {
      failf(data, "unable to set private key");
      EVP_PKEY_free(priv_ଚାବି);
      ଫେରନ୍ତୁ 0;
    }
    EVP_PKEY_free(priv_ଚାବି);  /* we do not need the handle any more... */
  }
  ଅନ୍ୟଥା {
    failf(data, "crypto engine not set, cannot load private key");
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ 1;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)octx;
  (ଶୂନ୍ୟ)ଚାବି_file;
  (ଶୂନ୍ୟ)ଚାବି_passwd;
  failf(data, "SSL_FILETYPE_ENGINE not supported for private key");
  ଫେରନ୍ତୁ 0;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା providercheck(ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା ossl_ctx *octx,
                         ଅଚଳ ଅକ୍ଷର *ଚାବି_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  ଅକ୍ଷର error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  ଯଦି(!octx->p.provider_loaded && is_pkcs11_uri(ଚାବି_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK) {
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_ଚାବି = ଖାଲି;
    OSSL_STORE_CTX *store = ଖାଲି;
    OSSL_STORE_INFO *info = ଖାଲି;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    ଯଦି(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      ଫେରନ୍ତୁ 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(ଚାବି_file, octx->p.libctx,
                               octx->p.propq, ui_method, ଖାଲି, ଖାଲି,
                               ଖାଲି, ଖାଲି);
    ଯଦି(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      UI_destroy_method(ui_method);
      ଫେରନ୍ତୁ 0;
    }
    ଯଦି(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    ଯଦି(info) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_type = OSSL_STORE_INFO_get_type(info);

      ଯଦି(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_ଚାବି = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    ଯଦି(!priv_ଚାବି) {
      failf(data, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      ଫେରନ୍ତୁ 0;
    }

    ଯଦି(SSL_CTX_use_Private_ଚାବି(ctx, priv_ଚାବି) != 1) {
      failf(data, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      EVP_PKEY_free(priv_ଚାବି);
      ଫେରନ୍ତୁ 0;
    }
    EVP_PKEY_free(priv_ଚାବି); /* we do not need the handle any more... */
  }
  ଅନ୍ୟଥା {
    failf(data, "crypto provider not set, cannot load private key");
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ 1;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)octx;
  (ଶୂନ୍ୟ)ଚାବି_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for private key");
  ଫେରନ୍ତୁ 0;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା engineload(ସଂରଚନା Curl_easy *data,
                      ସଂରଚନା ossl_ctx *octx,
                      ଅଚଳ ଅକ୍ଷର *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#ଯଦି defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  ଅକ୍ଷର error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  ଯଦି(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    ଫେରନ୍ତୁ 0;

  ଯଦି(octx->p.engine) {
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର cmd_name[] = "LOAD_CERT_CTRL";
    ସଂରଚନା {
      ଅଚଳ ଅକ୍ଷର *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = ଖାଲି;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    ଯଦି(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), ଖାଲି)) {
      failf(data, "SSL engine does not support loading certificates");
      ଫେରନ୍ତୁ 0;
    }

    /* Load the certificate from the engine */
    ଯଦି(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, ଖାଲି, 1)) {
      failf(data, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      ଫେରନ୍ତୁ 0;
    }

    ଯଦି(!params.cert) {
      failf(data, "SSL engine did not initialize the certificate properly.");
      ଫେରନ୍ତୁ 0;
    }

    ଯଦି(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      X509_free(params.cert);
      ଫେରନ୍ତୁ 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  ଅନ୍ୟଥା {
    failf(data, "crypto engine not set, cannot load certificate");
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ 1;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)octx;
  (ଶୂନ୍ୟ)cert_file;
  failf(data, "SSL_FILETYPE_ENGINE not supported for certificate");
  ଫେରନ୍ତୁ 0;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା providerload(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା ossl_ctx *octx,
                        ଅଚଳ ଅକ୍ଷର *cert_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  ଅକ୍ଷର error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  ଯଦି(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK)
    ଫେରନ୍ତୁ 0;

  ଯଦି(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = ଖାଲି;
    X509 *cert = ଖାଲି;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

    ଯଦି(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      ଫେରନ୍ତୁ 0;
    }
    ଯଦି(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    ଯଦି(info) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_type = OSSL_STORE_INFO_get_type(info);

      ଯଦି(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    ଯଦି(!cert) {
      failf(data, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      ଫେରନ୍ତୁ 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    ଯଦି(rc != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      ଫେରନ୍ତୁ 0;
    }
  }
  ଅନ୍ୟଥା {
    failf(data, "crypto provider not set, cannot load certificate");
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ 1;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)octx;
  (ଶୂନ୍ୟ)cert_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for certificate");
  ଫେରନ୍ତୁ 0;
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pkcs12load(ସଂରଚନା Curl_easy *data,
                      SSL_CTX* ctx,
                      ଅଚଳ ସଂରଚନା curl_blob *cert_blob,
                      ଅଚଳ ଅକ୍ଷର *cert_file,
                      ଅଚଳ ଅକ୍ଷର *ଚାବି_passwd)
{
  ଅକ୍ଷର error_buffer[256];
  BIO *cert_bio = ଖାଲି;
  PKCS12 *p12 = ଖାଲି;
  EVP_PKEY *pri;
  X509 *x509;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cert_done = 0;
  ଗଦା_OF(X509) *ca = ଖାଲି;
  ଯଦି(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cert_blob->len);
    ଯଦି(!cert_bio) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      ଫେରନ୍ତୁ 0;
    }
  }
  ଅନ୍ୟଥା {
    cert_bio = BIO_new(BIO_s_file());
    ଯଦି(!cert_bio) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
      ଫେରନ୍ତୁ 0;
    }

    ଯଦି(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(data, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      ଫେରନ୍ତୁ 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, ଖାଲି);
  BIO_free(cert_bio);

  ଯଦି(!p12) {
    failf(data, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(!PKCS12_parse(p12, ଚାବି_passwd, &pri, &x509, &ca)) {
    failf(data, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, ଆକାର(error_buffer)));
    PKCS12_free(p12);
    ଫେରନ୍ତୁ 0;
  }

  PKCS12_free(p12);

  ଯଦି(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(data, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, ଆକାର(error_buffer)));
    ଯାଅ fail;
  }

  ଯଦି(SSL_CTX_use_Private_ଚାବି(ctx, pri) != 1) {
    failf(data, "unable to use private key from PKCS12 file '%s'", cert_file);
    ଯାଅ fail;
  }

  ଯଦି(!SSL_CTX_check_private_ଚାବି(ctx)) {
    failf(data, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    ଯାଅ fail;
  }
  /* Set Certificate Verification chain */
  ଯଦି(ca) {
    ଯେତେବେଳେ(sk_X509_ଅଙ୍କ(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_କାଢ଼(ca);
      ଯଦି(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to client CA list");
        ଯାଅ fail;
      }
      ଯଦି(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to certificate chain");
        ଯାଅ fail;
      }
    }
  }

  cert_done = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_କାଢ଼_free(ca, X509_free);
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic କାଢ଼
#endif
  ଯଦି(!cert_done)
    ଫେରନ୍ତୁ 0; /* failure! */
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର CURLcode client_cert(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା ossl_ctx *octx,
                            ଅକ୍ଷର *cert_file,
                            ଅଚଳ ସଂରଚନା curl_blob *cert_blob,
                            ଅଚଳ ଅକ୍ଷର *cert_type,
                            ଅକ୍ଷର *ଚାବି_file,
                            ଅଚଳ ସଂରଚନା curl_blob *ଚାବି_blob,
                            ଅଚଳ ଅକ୍ଷର *ଚାବି_type,
                            ଅକ୍ଷର *ଚାବି_passwd)
{
  ଅକ୍ଷର error_buffer[256];
  bool check_privkey = TRUE;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  ଯଦି(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cert_use_result;

    ଯଦି(ଚାବି_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, ଚାବି_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    ବାଛ(file_type) {
    ସ୍ଥିତି SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_result = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, ଚାବି_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      ଯଦି(cert_use_result != 1) {
        failf(data,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            ଆକାର(error_buffer)));
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_result = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, ଚାବି_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      ଯଦି(cert_use_result != 1) {
        failf(data,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            ଆକାର(error_buffer)));
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSL_FILETYPE_ENGINE:
      ଯଦି(!cert_file || !engineload(data, octx, cert_file))
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSL_FILETYPE_PROVIDER:
      ଯଦି(!cert_file || !providerload(data, octx, cert_file))
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSL_FILETYPE_PKCS12:
      ଯଦି(!pkcs12load(data, ctx, cert_blob, cert_file, ଚାବି_passwd))
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "not supported file type '%s' for certificate", cert_type);
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ଯଦି(!ଚାବି_file && !ଚାବି_blob) {
      ଚାବି_file = cert_file;
      ଚାବି_blob = cert_blob;
    }
    ଅନ୍ୟଥା
      file_type = ossl_do_file_type(ଚାବି_type);

    ବାଛ(file_type) {
    ସ୍ଥିତି SSL_FILETYPE_PEM:
    ସ୍ଥିତି SSL_FILETYPE_ASN1:
      cert_use_result = ଚାବି_blob ?
        use_privatekey_blob(ctx, ଚାବି_blob, file_type, ଚାବି_passwd) :
      SSL_CTX_use_Private_ଚାବି_file(ctx, ଚାବି_file, file_type);
      ଯଦି(cert_use_result != 1) {
        failf(data, "unable to set private key file: '%s' type %s",
              ଚାବି_file ? ଚାବି_file : "(memory blob)",
              ଚାବି_type ? ଚାବି_type : "PEM");
        ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSL_FILETYPE_ENGINE:
      ଯଦି(!enginecheck(data, octx, ଚାବି_file, ଚାବି_passwd))
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSL_FILETYPE_PROVIDER:
      ଯଦି(!providercheck(data, octx, ଚାବି_file))
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି SSL_FILETYPE_PKCS12:
      ଯଦି(!pcks12_done) {
        failf(data, "file type P12 for private key not supported");
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      }
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "not supported file type for private key");
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    ଯଦି(!ssl) {
      failf(data, "unable to create an SSL structure");
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }

    x509 = SSL_get_certificate(ssl);

    ଯଦି(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#ଯଦି !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_ଚାବି = SSL_get_privatekey(ssl);
      ଯଦି(EVP_PKEY_id(priv_ଚାବି) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_ଚାବି);
        ଯଦି(RSA_flags(rsa) & RSA_METHOD_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#endif

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    ଯଦି(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      ଯଦି(!SSL_CTX_check_private_ଚାବି(ctx)) {
        failf(data, "Private key does not match the certificate public key");
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef CURLVERBOSE
/* returns non-zero on failure */
ସ୍ଥିର CURLcode x509_name_oneline(ଅଚଳ X509_NAME *a, ସଂରଚନା dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ଯଦି(bio_out) {
    unsigned long flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    ଯଦି(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      result = curlx_dyn_addn(d, biomem->data, biomem->length);
    }
    BIO_free(bio_out);
  }
  ଫେରନ୍ତୁ result;
}
#endif

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_init(ଶୂନ୍ୟ)
{
  ଅଚଳ uint64_t flags =
#ifdef OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#endif
#ifdef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    OPENSSL_INIT_NO_LOAD_CONFIG |
#ଅନ୍ୟଥା
    OPENSSL_INIT_LOAD_CONFIG |
#endif
    0;
  OPENSSL_init_ssl(flags, ଖାଲି);

#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#endif

  ଫେରନ୍ତୁ 1;
}

/* Global cleanup */
ସ୍ଥିର ଶୂନ୍ୟ ossl_cleanup(ଶୂନ୍ୟ)
{
#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#endif
}

/* Selects an OpenSSL crypto engine or provider.
 */
ସ୍ଥିର CURLcode ossl_set_engine(ସଂରଚନା Curl_easy *data, ସଂରଚନା ossl_ctx *octx,
                                ଅଚଳ ଅକ୍ଷର *name)
{
#ifdef USE_OPENSSL_ENGINE
  CURLcode result = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  ଯଦି(e) {

    ଯଦି(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = ଖାଲି;
    }
    ଯଦି(!ENGINE_init(e)) {
      ଅକ୍ଷର buf[256];

      ENGINE_free(e);
      failf(data, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, ଆକାର(buf)));
      result = CURLE_SSL_ENGINE_INITFAILED;
      e = ଖାଲି;
    }
    ଅନ୍ୟଥା {
      result = CURLE_OK;
    }
    octx->p.engine = e;
    ଫେରନ୍ତୁ result;
  }
#endif
#ifdef OPENSSL_HAS_PROVIDERS
  ଫେରନ୍ତୁ ossl_set_provider(data, octx, name);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)octx;
  (ଶୂନ୍ୟ)name;
  failf(data, "OpenSSL engine not found");
  ଫେରନ୍ତୁ CURLE_SSL_ENGINE_NOTFOUND;
#endif
}

/* Sets engine as default for all SSL operations
 */
ସ୍ଥିର CURLcode ossl_set_engine_default(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ossl_ctx *octx)
{
#ifdef USE_OPENSSL_ENGINE
  ଯଦି(octx->p.engine) {
    ଯଦି(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(data, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    ଅନ୍ୟଥା {
      failf(data, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      ଫେରନ୍ତୁ CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)octx;
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
ସ୍ଥିର ସଂରଚନା curl_slist *ossl_engines_list(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା curl_slist *list = ଖାଲି;
#ifdef USE_OPENSSL_ENGINE
  ସଂରଚନା curl_slist *beg;
  ENGINE *e;

  ପାଇଁ(e = ENGINE_get_first(); e; e = ENGINE_get_next(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    ଯଦି(!beg) {
      curl_slist_free_all(list);
      ଫେରନ୍ତୁ ଖାଲି;
    }
    list = beg;
  }
#endif
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ list;
}

#ifdef OPENSSL_HAS_PROVIDERS

ସ୍ଥିର ଶୂନ୍ୟ ossl_provider_cleanup(ସଂରଚନା ossl_ctx *octx)
{
  ଯଦି(!octx)
    ଫେରନ୍ତୁ;
  ଯଦି(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = ଖାଲି;
  }
  ଯଦି(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = ଖାଲି;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = ଖାଲି;
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
ସ୍ଥିର CURLcode ossl_set_provider(ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ossl_ctx *octx,
                                  ଅଚଳ ଅକ୍ଷର *iname)
{
  ଅକ୍ଷର name[MAX_PROVIDER_LEN + 1];
  ସଂରଚନା Curl_str prov;
  ଅଚଳ ଅକ୍ଷର *propq = ଖାଲି;

  ଯଦି(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଯଦି(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  memcpy(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  ଯଦି(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    ଯଦି(!libctx)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଯଦି(propq) {
      octx->p.propq = curlx_strdup(propq);
      ଯଦି(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#ifndef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  ଯଦି(!OSSL_LIB_CTX_load_config(octx->p.libctx, ଖାଲି)) {
    infof(data, "Failed to load default openssl config. Proceeding.");
  }
#endif

  ଯଦି(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  ଯଦି(!octx->p.provider) {
    ଅକ୍ଷର error_buffer[256];
    failf(data, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_buffer, ଆକାର(error_buffer)));
    ossl_provider_cleanup(octx);
    ଫେରନ୍ତୁ CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  ଯଦି(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(data, "Failed to load base");
    ଫେରନ୍ତୁ CURLE_SSL_ENGINE_NOTFOUND;
  }
  ଅନ୍ୟଥା
    octx->p.provider_loaded = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}
#ଅନ୍ୟଥା /* OPENSSL_HAS_PROVIDERS */
#define ossl_provider_cleanup(x)
#endif

ସ୍ଥିର CURLcode ossl_shutdown(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ଅକ୍ଷର buf[1024];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  ଯଦି(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    ଯାଅ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  ଯଦି(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    ପାଇଁ(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(buf));
      CURL_TRC_CF(data, cf, "SSL shutdown not sent, read -> %d", nread);
      ଯଦି(nread <= 0)
        ଭାଙ୍ଗ;
    }
    err = SSL_get_error(octx->ssl, nread);
    ଯଦି(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool ନିବେଶ_pending;
      /* Yes, it did. */
      ଯଦି(!send_shutdown) {
        CURL_TRC_CF(data, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        ଯାଅ out;
      }
      ଅନ୍ୟଥା ଯଦି(!cf->next->cft->is_alive(cf->next, data, &ନିବେଶ_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(data, cf, "peer closed connection");
        *done = TRUE;
        ଯାଅ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ଯଦି(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
    ERR_clear_error();
    CURL_TRC_CF(data, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    ଯଦି(rc == 1) {
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
      *done = TRUE;
      ଯାଅ out;
    }
    ଯଦି(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ଯାଅ out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  ପାଇଁ(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(buf));
    CURL_TRC_CF(data, cf, "SSL shutdown read -> %d", nread);
    ଯଦି(nread <= 0)
      ଭାଙ୍ଗ;
  }
  err = SSL_get_error(octx->ssl, nread);
  ବାଛ(err) {
  ସ୍ଥିତି SSL_ERROR_ZERO_RETURN: /* no more data */
    ଯଦି(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
    ଅନ୍ୟଥା
      CURL_TRC_CF(data, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SSL_ERROR_NONE: /* did not get anything */
  ସ୍ଥିତି SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(data, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(data, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(unsigned long sslerr = ERR_get_error());
      CURL_TRC_CF(data, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, ଆକାର(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    result = CURLE_OK;
    ଭାଙ୍ଗ;
  }

out:
  cf->shutdown = (result || *done);
  ଯଦି(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->ନିବେଶ_pending = FALSE;
  ଫେରନ୍ତୁ result;
}

/* also used by quic-tls */
ଶୂନ୍ୟ Curl_osslctx_cleanup(ସଂରଚନା ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  ଯଦି(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = ଖାଲି;
  }
  ଯଦି(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = ଖାଲି;
    octx->x509_store_setup = FALSE;
  }
  ଯଦି(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = ଖାଲି;
  }
#ifdef USE_OPENSSL_ENGINE
  ଯଦି(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = ଖାଲି;
  }
#endif
  ossl_provider_cleanup(octx);
}

ସ୍ଥିର ଶୂନ୍ୟ ossl_close(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  (ଶୂନ୍ୟ)data;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
ସ୍ଥିର ଶୂନ୍ୟ ossl_close_all(ସଂରଚନା Curl_easy *data)
{
  (ଶୂନ୍ୟ)data;
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
ସ୍ଥିର CURLcode ossl_verifyhost(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା connectdata *conn,
                                ସଂରଚନା ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  ଗଦା_OF(GENERAL_NAME) *altnames;
#ifdef USE_IPV6
  ସଂରଚନା in6_addr addr;
#ଅନ୍ୟଥା
  ସଂରଚନା in_addr addr;
#endif
  CURLcode result = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = strlen(peer->origin->hostname);

  (ଶୂନ୍ୟ)conn;
  ବାଛ(peer->type) {
  ସ୍ଥିତି CURL_SSL_PEER_IPV4:
    ଯଦି(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = ଆକାର(ସଂରଚନା in_addr);
    ଭାଙ୍ଗ;
#ifdef USE_IPV6
  ସ୍ଥିତି CURL_SSL_PEER_IPV6:
    ଯଦି(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = ଆକାର(ସଂରଚନା in6_addr);
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUGASSERT(0);
    failf(data, "unexpected SSL peer type: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)peer->type);
    ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, ଖାଲି, ଖାଲି);

  ଯଦି(altnames) {
#ifdef HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#ଅନ୍ୟଥା
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା numalts;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
#endif

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_ଅଙ୍କ(altnames);

    /* loop through all alternatives - until a dnsmatch */
    ପାଇଁ(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ଅଚଳ GENERAL_NAME *check = sk_GENERAL_NAME_ମୂଲ୍ୟ(altnames, i);

      ଯଦି(check->type == GEN_DNS)
        dNSName = TRUE;
      ଅନ୍ୟଥା ଯଦି(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      ଯଦି(check->type == target) {
        /* get data and length */
        ଅଚଳ ଅକ୍ଷର *altptr = (ଅଚଳ ଅକ୍ଷର *)ASN1_STRING_get0_data(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_length(check->d.ia5);

        ବାଛ(target) {
        ସ୍ଥିତି GEN_DNS: /* name/pattern comparison */
          ଯଦି(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)altlen, altptr);
          }
          ଭାଙ୍ଗ;

        ସ୍ଥିତି GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          ଯଦି((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          ଭାଙ୍ଗ;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  ଯଦି(matched)
    /* an alternative name matched */
    ;
  ଅନ୍ୟଥା ଯଦି(dNSName || iPAddress) {
    ଅଚଳ ଅକ୍ଷର *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(data, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(data, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    result = CURLE_PEER_FAILED_VERIFICATION;
  }
  ଅନ୍ୟଥା {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = -1;
    unsigned ଅକ୍ଷର *cn = ଖାଲି;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ଅଚଳ X509_NAME *name = X509_get_subject_name(server_cert);
    ଯଦି(name) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;
      ଯେତେବେଳେ((j = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    ଯଦି(i >= 0) {
      ଅଚଳ ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      ଯଦି(tmp) {
        ଯଦି(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_length(tmp);
          cn = (unsigned ଅକ୍ଷର *)CURL_UNCONST(ASN1_STRING_get0_data(tmp));
        }
        ଅନ୍ୟଥା { /* not a UTF8 name */
          ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          ଯଦି(utf8len < 0)
            cnlen = 0;
          ଅନ୍ୟଥା
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        ଯଦି((cnlen == 0) || !cn)
          result = CURLE_OUT_OF_MEMORY;
        ଅନ୍ୟଥା ଯଦି(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(data, "SSL: illegal cert name field");
          result = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    ଯଦି(result)
      /* error already detected, pass through */
      ;
    ଅନ୍ୟଥା ଯଦି(!cn) {
      failf(data, "SSL: unable to obtain common name from peer certificate");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    ଅନ୍ୟଥା ଯଦି(!Curl_cert_hostcheck((ଅଚଳ ଅକ୍ଷର *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(data, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cnlen, cn,
            peer->origin->user_hostname);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    ଅନ୍ୟଥା {
      infof(data, " common name: %.*s (matched)", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cnlen, cn);
    }
    ଯଦି(free_cn)
      OPENSSL_free(cn);
  }

  ଫେରନ୍ତୁ result;
}

#ifndef OPENSSL_NO_OCSP
ସ୍ଥିର CURLcode verifystatus(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା ossl_ctx *octx)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ocsp_status;
#ifdef HAVE_BORINGSSL_LIKE
  ଅଚଳ uint8_t *status;
#ଅନ୍ୟଥା
  unsigned ଅକ୍ଷର *status;
#endif
  ଅଚଳ unsigned ଅକ୍ଷର *p;
  CURLcode result = CURLE_OK;
  OCSP_RESPONSE *rsp = ଖାଲି;
  OCSP_BASICRESP *br = ଖାଲି;
  X509_STORE     *st = ଖାଲି;
  ଗଦା_OF(X509) *ch = ଖାଲି;
  X509 *cert;
  OCSP_CERTID *id = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
  long len;

  (ଶୂନ୍ୟ)cf;
  DEBUGASSERT(octx);

  len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  ଯଦି(!status) {
    failf(data, "No OCSP response received");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(ଖାଲି, &p, len);
  ଯଦି(!rsp) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }

  ocsp_status = OCSP_response_status(rsp);
  ଯଦି(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(data, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }

  br = OCSP_response_get1_basic(rsp);
  ଯଦି(!br) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }

#ଯଦି !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_ସଂଖ୍ୟା) && LIBRESSL_VERSION_ସଂଖ୍ୟା < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#ଅନ୍ୟଥା
  ch = SSL_get_peer_cert_chain(octx->ssl);
#endif
  ଯଦି(!ch) {
    failf(data, "Could not get peer certificate chain");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  ଯଦି(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(data, "OCSP response verification failed");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  ଯଦି(!cert) {
    failf(data, "Error getting peer certificate");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }

  ପାଇଁ(i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sk_X509_ଅଙ୍କ(ch); i++) {
    X509 *issuer = sk_X509_ମୂଲ୍ୟ(ch, (ossl_valsize_t)i);
    ଯଦି(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      ଭାଙ୍ଗ;
    }
  }
  X509_free(cert);

  ଯଦି(!id) {
    failf(data, "Error computing OCSP ID");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  ଯଦି(ret != 1) {
    failf(data, "Could not find certificate ID in OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  ଯଦି(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(data, "OCSP response has expired");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }

  infof(data, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  ବାଛ(cert_status) {
  ସ୍ଥିତି V_OCSP_CERTSTATUS_GOOD:
    ଭାଙ୍ଗ;

  ସ୍ଥିତି V_OCSP_CERTSTATUS_REVOKED:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    failf(data, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    ଯାଅ end;

  ସ୍ଥିତି V_OCSP_CERTSTATUS_UNKNOWN:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ end;
  }

end:
  ଯଦି(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  ଫେରନ୍ତୁ result;
}
#endif

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *ssl_ସନ୍ଦେଶ_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_ver, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ)
{
  ଯଦି(ssl_ver == SSL3_VERSION_MAJOR) {
    ବାଛ(ସନ୍ଦେଶ) {
    ସ୍ଥିତି SSL3_MT_HELLO_REQUEST:
      ଫେରନ୍ତୁ "Hello request";
    ସ୍ଥିତି SSL3_MT_CLIENT_HELLO:
      ଫେରନ୍ତୁ "Client hello";
    ସ୍ଥିତି SSL3_MT_SERVER_HELLO:
      ଫେରନ୍ତୁ "Server hello";
#ifdef SSL3_MT_NEWSESSION_TICKET
    ସ୍ଥିତି SSL3_MT_NEWSESSION_TICKET:
      ଫେରନ୍ତୁ "Newsession Ticket";
#endif
    ସ୍ଥିତି SSL3_MT_CERTIFICATE:
      ଫେରନ୍ତୁ "Certificate";
    ସ୍ଥିତି SSL3_MT_SERVER_ଚାବି_EXCHANGE:
      ଫେରନ୍ତୁ "Server key exchange";
    ସ୍ଥିତି SSL3_MT_CLIENT_ଚାବି_EXCHANGE:
      ଫେରନ୍ତୁ "Client key exchange";
    ସ୍ଥିତି SSL3_MT_CERTIFICATE_REQUEST:
      ଫେରନ୍ତୁ "Request CERT";
    ସ୍ଥିତି SSL3_MT_SERVER_DONE:
      ଫେରନ୍ତୁ "Server finished";
    ସ୍ଥିତି SSL3_MT_CERTIFICATE_VERIFY:
      ଫେରନ୍ତୁ "CERT verify";
    ସ୍ଥିତି SSL3_MT_FINISHED:
      ଫେରନ୍ତୁ "Finished";
#ifdef SSL3_MT_CERTIFICATE_STATUS
    ସ୍ଥିତି SSL3_MT_CERTIFICATE_STATUS:
      ଫେରନ୍ତୁ "Certificate Status";
#endif
#ifdef SSL3_MT_ENCRYPTED_EXTENSIONS
    ସ୍ଥିତି SSL3_MT_ENCRYPTED_EXTENSIONS:
      ଫେରନ୍ତୁ "Encrypted Extensions";
#endif
#ifdef SSL3_MT_SUPPLEMENTAL_DATA
    ସ୍ଥିତି SSL3_MT_SUPPLEMENTAL_DATA:
      ଫେରନ୍ତୁ "Supplemental data";
#endif
#ifdef SSL3_MT_END_OF_EARLY_DATA
    ସ୍ଥିତି SSL3_MT_END_OF_EARLY_DATA:
      ଫେରନ୍ତୁ "End of early data";
#endif
#ifdef SSL3_MT_ଚାବି_UPDATE
    ସ୍ଥିତି SSL3_MT_ଚାବି_UPDATE:
      ଫେରନ୍ତୁ "Key update";
#endif
#ifdef SSL3_MT_NEXT_PROTO
    ସ୍ଥିତି SSL3_MT_NEXT_PROTO:
      ଫେରନ୍ତୁ "Next protocol";
#endif
#ifdef SSL3_MT_MESSAGE_HASH
    ସ୍ଥିତି SSL3_MT_MESSAGE_HASH:
      ଫେରନ୍ତୁ "Message hash";
#endif
    }
  }
  ଫେରନ୍ତୁ "Unknown";
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *tls_rt_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
  ବାଛ(type) {
#ifdef SSL3_RT_HEADER
  ସ୍ଥିତି SSL3_RT_HEADER:
    ଫେରନ୍ତୁ "TLS header";
#endif
  ସ୍ଥିତି SSL3_RT_CHANGE_CIPHER_SPEC:
    ଫେରନ୍ତୁ "TLS change cipher";
  ସ୍ଥିତି SSL3_RT_ALERT:
    ଫେରନ୍ତୁ "TLS alert";
  ସ୍ଥିତି SSL3_RT_HANDSHAKE:
    ଫେରନ୍ତୁ "TLS handshake";
  ସ୍ଥିତି SSL3_RT_APPLICATION_DATA:
    ଫେରନ୍ତୁ "TLS app data";
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
ସ୍ଥିର ଶୂନ୍ୟ ossl_trace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା direction, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_ver, ପୂର୍ଣ୍ଣସଂଖ୍ୟା content_type,
                       ଅଚଳ ଶୂନ୍ୟ *buf, size_t len, SSL *ssl,
                       ଶୂନ୍ୟ *userp)
{
  ଅଚଳ ଅକ୍ଷର *verstr;
  ସଂରଚନା Curl_cfilter *cf = userp;
  ସଂରଚନା Curl_easy *data = ଖାଲି;
  ଅକ୍ଷର unknown[32];

  ଯଦି(!cf)
    ଫେରନ୍ତୁ;
  data = CF_DATA_CURRENT(cf);
  ଯଦି(!data || !data->set.fdebug || (direction && direction != 1))
    ଫେରନ୍ତୁ;

  ବାଛ(ssl_ver) {
#ifdef SSL3_VERSION
  ସ୍ଥିତି SSL3_VERSION:
    verstr = "SSLv3";
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି TLS1_VERSION:
    verstr = "TLSv1.0";
    ଭାଙ୍ଗ;
#ifdef TLS1_1_VERSION
  ସ୍ଥିତି TLS1_1_VERSION:
    verstr = "TLSv1.1";
    ଭାଙ୍ଗ;
#endif
#ifdef TLS1_2_VERSION
  ସ୍ଥିତି TLS1_2_VERSION:
    verstr = "TLSv1.2";
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି TLS1_3_VERSION:
    verstr = "TLSv1.3";
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    curl_msnprintf(unknown, ଆକାର(unknown), "(%x)", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ssl_ver);
    verstr = unknown;
    ଭାଙ୍ଗ;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  ଯଦି(ssl_ver
#ifdef SSL3_RT_HEADER
     && content_type != SSL3_RT_HEADER
#endif
#ifdef SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#endif
    ) {
    ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ_name = "Truncated message";
    ଅଚଳ ଅକ୍ଷର *tls_rt_name;
    ଅକ୍ଷର ssl_buf[1024];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ_type = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    ଯଦି(ssl_ver == SSL3_VERSION_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    ଅନ୍ୟଥା
      tls_rt_name = "";

    ଯଦି(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      ଯଦି(len) {
        ସନ୍ଦେଶ_type = *(ଅଚଳ unsigned ଅକ୍ଷର *)buf;
        ସନ୍ଦେଶ_name = "Change cipher spec";
      }
    }
    ଅନ୍ୟଥା ଯଦି(content_type == SSL3_RT_ALERT) {
      ଯଦି(len >= 2) {
        ସନ୍ଦେଶ_type =
          (((ଅଚଳ unsigned ଅକ୍ଷର *)buf)[0] << 8) +
           ((ଅଚଳ unsigned ଅକ୍ଷର *)buf)[1];
        ସନ୍ଦେଶ_name = SSL_alert_desc_string_long(ସନ୍ଦେଶ_type);
      }
    }
    ଅନ୍ୟଥା ଯଦି(len) {
      ସନ୍ଦେଶ_type = *(ଅଚଳ unsigned ଅକ୍ଷର *)buf;
      ସନ୍ଦେଶ_name = ssl_ସନ୍ଦେଶ_type(ssl_ver, ସନ୍ଦେଶ_type);
    }

    txt_len = curl_msnprintf(ssl_buf, ଆକାର(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, ସନ୍ଦେଶ_name, ସନ୍ଦେଶ_type);
    Curl_debug(data, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(data, (direction == 1) ? CURLINFO_SSL_DATA_OUT :
             CURLINFO_SSL_DATA_IN, (ଅଚଳ ଅକ୍ଷର *)buf, len);
  (ଶୂନ୍ୟ)ssl;
}

ସ୍ଥିର CURLcode ossl_set_ssl_version_min_max(ସଂରଚନା Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_version_min)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  long curl_ssl_version_min = (long)ssl_version_min;
  long curl_ssl_version_max;

  /* convert curl min SSL version option to OpenSSL constant */
#ଯଦି defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_VERSION_ସଂଖ୍ୟା)
  uint16_t ossl_ssl_version_min = 0;
  uint16_t ossl_ssl_version_max = 0;
#ଅନ୍ୟଥା
  long ossl_ssl_version_min = 0;
  long ossl_ssl_version_max = 0;
#endif
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_version_min != CURL_SSLVERSION_DEFAULT);
  ବାଛ(curl_ssl_version_min) {
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    ଭାଙ୍ଗ;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (long)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  ବାଛ(curl_ssl_version_max) {
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_SSLVERSION_MAX_NONE:  /* none selected */
  ସ୍ଥିତି CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    ଭାଙ୍ଗ;
  }

  ଯଦି(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;

  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_ossl_add_session(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା ossl_ctx *octx,
                               ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                               SSL_SESSION *session,
                               ଅଚଳ ଅକ୍ଷର *alpn,
                               unsigned ଅକ୍ଷର *quic_tp,
                               size_t quic_tp_len,
                               ସଂରଚନା Curl_ssl_session **psession)
{
  ସଂରଚନା Curl_ssl_session *sc_session = ଖାଲି, *sc_dup = ଖାଲି;
  unsigned ଅକ୍ଷର *der_session_buf = ଖାଲି;
  unsigned ଅକ୍ଷର *qtp_clone = ଖାଲି;
  CURLcode result = CURLE_OK;

  ଯଦି(psession)
    *psession = ଖାଲି;
  ଯଦି(!cf || !data)
    ଯାଅ out;

  ଯଦି(Curl_ssl_scache_use(cf, data)) {
    size_t der_session_size;
    unsigned ଅକ୍ଷର *der_session_ptr;
    size_t earlydata_max = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ietf_tls_id = SSL_version(octx->ssl);

    der_session_size = i2d_SSL_SESSION(session, ଖାଲି);
    ଯଦି(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_size);
    ଯଦି(!der_session_buf) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }

    der_session_size = i2d_SSL_SESSION(session, &der_session_ptr);
    ଯଦି(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }

#ifdef HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_data(session);
#endif
    ଯଦି(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ଅଚଳ ଅକ୍ଷର *)quic_tp, quic_tp_len);
      ଯଦି(!qtp_clone) {
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ out;
      }
    }

    result = Curl_ssl_session_create2(der_session_buf, der_session_size,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(ଖାଲି) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = ଖାଲି;  /* took ownership of sdata */
#ifdef USE_APPLE_SECTRUST
    ଯଦି(!result)
      sc_session->sectrust_verified = octx->sectrust_verified;
#endif
    ଯଦି(!result && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        result = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    ଯଦି(!result) {
      result = Curl_ssl_scache_put(cf, data, ssl_peer_ଚାବି, sc_session);
      /* took ownership of `sc_session` */
      sc_session = ଖାଲି;
    }
  }

out:
  curlx_free(der_session_buf);
  ଯଦି(!result && psession) {
    *psession = sc_dup;
    sc_dup = ଖାଲି;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ଫେରନ୍ତୁ result;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ସଂରଚନା Curl_cfilter *cf = (ସଂରଚନା Curl_cfilter *)SSL_get_app_data(ssl);
  ଯଦି(cf) {
    ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
    ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
    ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
    ସଂରଚନା Curl_ssl_session *session = ଖାଲି;
    Curl_ossl_add_session(cf, data, octx, connssl->peer.scache_ଚାବି,
                          ssl_sessionid, connssl->negotiated.alpn, ଖାଲି,
                          0, &session);
    ଯଦି(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର CURLcode load_cacert_from_memory(X509_STORE *store,
                                        ଅଚଳ ସଂରଚନା curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = ଖାଲି;
  ଗଦା_OF(X509_INFO) *inf = ଖାଲି;

  /* everything else is a reference */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ଗଣନା_2 = 0;
  X509_INFO *itmp = ଖାଲି;

  ଯଦି(ca_info_blob->len > (size_t)INT_MAX)
    ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ca_info_blob->len);
  ଯଦି(!cbio)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, ଖାଲି, ଖାଲି, ଖାଲି);
  ଯଦି(!inf) {
    BIO_free(cbio);
    ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  ପାଇଁ(i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sk_X509_INFO_ଅଙ୍କ(inf); ++i) {
    itmp = sk_X509_INFO_ମୂଲ୍ୟ(inf, (ossl_valsize_t)i);
    ଯଦି(itmp->x509) {
      ଯଦି(X509_STORE_add_cert(store, itmp->x509)) {
        ++ଗଣନା_2;
      }
      ଅନ୍ୟଥା {
        /* set count to 0 to return an error */
        ଗଣନା_2 = 0;
        ଭାଙ୍ଗ;
      }
    }
    ଯଦି(itmp->crl) {
      ଯଦି(X509_STORE_add_crl(store, itmp->crl)) {
        ++ଗଣନା_2;
      }
      ଅନ୍ୟଥା {
        /* set count to 0 to return an error */
        ଗଣନା_2 = 0;
        ଭାଙ୍ଗ;
      }
    }
  }

#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ଠେଲ
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_INFO_କାଢ଼_free(inf, X509_INFO_free);
#ଯଦି defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic କାଢ଼
#endif
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  ଫେରନ୍ତୁ (ଗଣନା_2 > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ifdef USE_WIN32_CRYPTO
ସ୍ଥିର CURLcode ossl_win_load_store(ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା Curl_cfilter *cf,
                                    ଅଚଳ ଅକ୍ଷର *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode result = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  ଯଦି(hStore) {
    PCCERT_CONTEXT pContext = ଖାଲି;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = ଖାଲି;
    DWORD enhkey_usage_size = 0;
    VERBOSE(size_t total = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    result = CURLE_OK;
    ପାଇଁ(;;) {
      X509 *x509;
      FILETIME now;
      BYTE ଚାବି_usage[2];
      DWORD req_size;
      ଅଚଳ unsigned ଅକ୍ଷର *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      ଯଦି(!pContext)
        ଭାଙ୍ଗ;

      VERBOSE(++total);

#ଯଦି defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        ଅକ୍ଷର cert_name[256];
        ଯଦି(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               ଖାଲି, cert_name, ଆକାର(cert_name)))
          infof(data, "SSL: unknown cert name");
        ଅନ୍ୟଥା
          infof(data, "SSL: Checking cert \"%s\"", cert_name);
      }
#endif
      encoded_cert = (ଅଚଳ unsigned ଅକ୍ଷର *)pContext->pbCertEncoded;
      ଯଦି(!encoded_cert)
        ଜାରି;

      GetSystemTimeAsFileTime(&now);
      ଯଦି(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        ଜାରି;

      /* If key usage exists check for signing attribute */
      ଯଦି(Cert_Get_Intended_ଚାବି_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 ଚାବି_usage, ଆକାର(ଚାବି_usage))) {
        ଯଦି(!(ଚାବି_usage[0] & CERT_ଚାବି_CERT_SIGN_ଚାବି_USAGE))
          ଜାରି;
      }
      ଅନ୍ୟଥା ଯଦି(GetLastError())
        ଜାରି;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      ଯଦି(Cert_Get_Enhanced_ଚାବି_Usage(pContext, 0, ଖାଲି, &req_size) && req_size) {
        ଯଦି(req_size > enhkey_usage_size) {
          ଶୂନ୍ୟ *tmp = curlx_realloc(enhkey_usage, req_size);

          ଯଦି(!tmp) {
            failf(data, "SSL: Out of memory allocating for OID list");
            result = CURLE_OUT_OF_MEMORY;
            ଭାଙ୍ଗ;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_size = req_size;
        }

        ଯଦି(Cert_Get_Enhanced_ଚାବି_Usage(pContext, 0, enhkey_usage, &req_size)) {
          ଯଦି(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            ଯଦି((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              ଜାରି;
          }
          ଅନ୍ୟଥା {
            DWORD i;
            bool found = FALSE;

            ପାଇଁ(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              ଯଦି(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                ଭାଙ୍ଗ;
              }
            }

            ଯଦି(!found)
              ଜାରି;
          }
        }
        ଅନ୍ୟଥା
          ଜାରି;
      }
      ଅନ୍ୟଥା
        ଜାରି;

      x509 = d2i_X509(ଖାଲି, &encoded_cert, (long)pContext->cbCertEncoded);
      ଯଦି(!x509)
        ଜାରି;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      ଯଦି(X509_STORE_add_cert(store, x509) == 1) {
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

    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ossl_windows_load_anchors(ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode result = CURLE_OK;

  *padded = FALSE;
  ପାଇଁ(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    result = ossl_win_load_store(data, cf, win_stores[i], store, &store_added);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଯଦି(store_added) {
      CURL_TRC_CF(data, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    ଅନ୍ୟଥା
      infof(data, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  ଫେରନ୍ତୁ result;
}

#endif /* USE_WIN32_CRYPTO */

ସ୍ଥିର CURLcode ossl_load_trust_anchors(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା ossl_ctx *octx,
                                        X509_STORE *store)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? ଖାଲି : conn_config->CAfile);
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  ଯଦି(conn_config->native_ca_store) {
#ifdef USE_WIN32_CRYPTO
    bool added = FALSE;
    result = ossl_windows_load_anchors(cf, data, store, &added);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଯଦି(added) {
      infof(data, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#ନଚେତଯଦି defined(USE_APPLE_SECTRUST)
    infof(data, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#endif
  }

  ଯଦି(conn_config->ca_info_blob) {
    result = load_cacert_from_memory(store, conn_config->ca_info_blob);
    ଯଦି(result) {
      failf(data, "error adding trust anchors from certificate blob: %d",
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଫେରନ୍ତୁ result;
    }
    infof(data, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  ଯଦି(ssl_cafile || ssl_capath) {
#ifdef HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    ଯଦି(ssl_cafile) {
      ଯଦି(!X509_STORE_load_file(store, ssl_cafile)) {
        ଯଦି(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from file: %s", ssl_cafile);
          ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
        }
        ଅନ୍ୟଥା
          infof(data, "error setting certificate file, continuing anyway");
      }
      infof(data, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    ଯଦି(ssl_capath) {
      ଯଦି(!X509_STORE_load_path(store, ssl_capath)) {
        ଯଦି(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from path: %s", ssl_capath);
          ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
        }
        ଅନ୍ୟଥା
          infof(data, "error setting certificate path, continuing anyway");
      }
      infof(data, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#ଅନ୍ୟଥା
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    ଯଦି(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      ଯଦି(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(data, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
      }
      ଅନ୍ୟଥା {
        infof(data, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    ଯଦି(ssl_cafile)
      infof(data, "  CAfile: %s", ssl_cafile);
    ଯଦି(ssl_capath)
      infof(data, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#endif
  }

#ifdef CURL_CA_FALLBACK
  ଯଦି(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(data, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#endif
  ଯଦି(octx->store_is_empty && !have_native_check)
    infof(data, "  no trust anchors configured");

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ossl_populate_x509_store(ସଂରଚନା Curl_cfilter *cf,
                                         ସଂରଚନା Curl_easy *data,
                                         ସଂରଚନା ossl_ctx *octx,
                                         X509_STORE *store)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  X509_LOOKUP *lookup = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_crlfile = conn_config->CRLfile;
  unsigned long x509flags = 0;

  CURL_TRC_CF(data, cf, "configuring OpenSSL's x509 trust store");
  ଯଦି(!store)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ଯଦି(!conn_config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    ଫେରନ୍ତୁ CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  result = ossl_load_trust_anchors(cf, data, octx, store);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  /* Does not make sense to load a CRL file without peer verification */
#ifdef USE_APPLE_SECTRUST
  ଯଦି(ssl_crlfile && conn_config->native_ca_store) {
    failf(data, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  }
#endif
  ଯଦି(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    ଯଦି(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(data, "error loading CRL file: %s", ssl_crlfile);
      ଫେରନ୍ତୁ CURLE_SSL_CRL_BADFILE;
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

  ଯଦି(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (ଶୂନ୍ୟ)X509_STORE_set_flags(store, x509flags);

  ଫେରନ୍ତୁ result;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_OSSL_X509_ଚାବି  "tls:ossl:x509:share"

ସଂରଚନା ossl_x509_share {
  ଅକ୍ଷର *CAfile;         /* CAfile path used to generate X509 store */
  ଅକ୍ଷର *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  ସଂରଚନା curltime time; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

ସ୍ଥିର ଶୂନ୍ୟ oss_x509_share_free(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t ଚାବି_len, ଶୂନ୍ୟ *p)
{
  ସଂରଚନା ossl_x509_share *share = p;
  DEBUGASSERT(ଚାବି_len == CURL_CSTRLEN(MPROTO_OSSL_X509_ଚାବି));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_ଚାବି, ଚାବି, ଚାବି_len));
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)ଚାବି_len;
  ଯଦି(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

ସ୍ଥିର bool ossl_cached_x509_store_expired(ସଂରଚନା Curl_easy *data,
                                           ଅଚଳ ସଂରଚନା ossl_x509_share *mb)
{
  ଯଦି(data->set.ssl_ca_cache_timeout < 0)
    ଫେରନ୍ତୁ FALSE;
  ଅନ୍ୟଥା {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &mb->time);
    timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

    ଫେରନ୍ତୁ elapsed_ms >= timeout_ms;
  }
}

ସ୍ଥିର bool ossl_cached_x509_store_different(ସଂରଚନା Curl_cfilter *cf,
                                             ଅଚଳ ସଂରଚନା Curl_easy *data,
                                             ଅଚଳ ସଂରଚନା ossl_x509_share *mb)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (ଶୂନ୍ୟ)data;
  ଯଦି(mb->no_partialchain != conn_config->no_partialchain)
    ଫେରନ୍ତୁ TRUE;
  ଯଦି(!mb->CAfile || !conn_config->CAfile) {
    ଯଦି(mb->CAfile != conn_config->CAfile)
      ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(strcmp(mb->CAfile, conn_config->CAfile))
    ଫେରନ୍ତୁ TRUE;

  ଯଦି(!mb->CApath || !conn_config->CApath) {
    ଯଦି(mb->CApath != conn_config->CApath)
      ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(strcmp(mb->CApath, conn_config->CApath))
    ଫେରନ୍ତୁ TRUE;

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର X509_STORE *ossl_get_cached_x509_store(ସଂରଚନା Curl_cfilter *cf,
                                              ସଂରଚନା Curl_easy *data,
                                              bool *pempty)
{
  ସଂରଚନା Curl_multi *multi = data->multi;
  ସଂରଚନା ossl_x509_share *share;
  X509_STORE *store = ଖାଲି;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_ଚାବି,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_ଚାବି)) : ଖାଲି;
  ଯଦି(share && share->store &&
     !ossl_cached_x509_store_expired(data, share) &&
     !ossl_cached_x509_store_different(cf, data, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  ଫେରନ୍ତୁ store;
}

ସ୍ଥିର ଶୂନ୍ୟ ossl_set_cached_x509_store(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       X509_STORE *store,
                                       bool is_empty)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା Curl_multi *multi = data->multi;
  ସଂରଚନା ossl_x509_share *share;

  DEBUGASSERT(multi);
  ଯଦି(!multi)
    ଫେରନ୍ତୁ;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_ଚାବି,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_ଚାବି));

  ଯଦି(!share) {
    share = curlx_calloc(1, ଆକାର(*share));
    ଯଦି(!share)
      ଫେରନ୍ତୁ;
    ଯଦି(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_ଚାବି,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_ଚାବି),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      ଫେରନ୍ତୁ;
    }
  }

  ଯଦି(X509_STORE_up_ref(store)) {
    ଅକ୍ଷର *CAfile = ଖାଲି;
    ଅକ୍ଷର *CApath = ଖାଲି;

    ଯଦି(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      ଯଦି(!CAfile) {
        X509_STORE_free(store);
        ଫେରନ୍ତୁ;
      }
    }

    ଯଦି(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      ଯଦି(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        ଫେରନ୍ତୁ;
      }
    }

    ଯଦି(share->store) {
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
#ଅନ୍ୟଥା
#define USING_PROVIDER(x) 0
#endif

CURLcode Curl_ssl_setup_x509_store(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ossl_ctx *octx)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
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
  ଯଦି(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  ଅନ୍ୟଥା {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    result = ossl_populate_x509_store(cf, data, octx, store);
    ଯଦି(result == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, data, store, (bool)octx->store_is_empty);
    }
  }

  ERR_କାଢ଼_to_mark();

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool ossl_apply_session(
  ସଂରଚନା ossl_ctx *octx,
  ସଂରଚନା Curl_cfilter *cf,
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  ସଂରଚନା Curl_ssl_session *scs)
{
  ସଂରଚନା ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ଅଚଳ unsigned ଅକ୍ଷର *der_sessionid = scs->sdata;
  size_t der_sessionid_size = scs->sdata_len;
  SSL_SESSION *ssl_session = ଖାଲି;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(ଖାଲି, &der_sessionid,
                                (long)der_sessionid_size);
  ଯଦି(ssl_session) {
    ଯଦି(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(ଅକ୍ଷର error_buffer[256]);
      infof(data, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          ଆକାର(error_buffer)));
    }
    ଅନ୍ୟଥା {
      ଯଦି(conn_cfg->verifypeer &&
         (SSL_get_verify_result(octx->ssl) != X509_V_OK)
#ifdef USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#endif
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, ଖାଲି);
        infof(data, "SSL session not peer verified, not reusing");
      }
      ଅନ୍ୟଥା {
        infof(data, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ifdef USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#endif
        infof(data, "SSL verify result: %lx",
              (unsigned long)SSL_get_verify_result(octx->ssl));
#ifdef HAVE_OPENSSL_EARLYDATA
        ଯଦି(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_data(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_data = FALSE;
          ଯଦି(sess_reuse_cb)
            (ଶୂନ୍ୟ)sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          ଯଦି(do_early_data) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_to(alpns, scs->alpn);
          }
        }
#ଅନ୍ୟଥା
        (ଶୂନ୍ୟ)alpns;
        (ଶୂନ୍ୟ)conn_cfg;
        (ଶୂନ୍ୟ)sess_reuse_cb;
#endif
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  ଅନ୍ୟଥା {
    infof(data, "SSL session not accepted by OpenSSL, continuing without");
  }
  ଫେରନ୍ତୁ (bool)octx->reused_session;
}

ସ୍ଥିର CURLcode ossl_init_session_and_alpns(
  ସଂରଚନା ossl_ctx *octx,
  ସଂରଚନା Curl_cfilter *cf,
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା ssl_peer *peer,
  ଅଚଳ ସଂରଚନା alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  ସଂରଚନା ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା alpn_spec alpns;
  CURLcode result;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  ଯଦି((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ସଂରଚନା Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    ଯଦି(scs) {
      ଯଦି(ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
    }
  }

  ଯଦି(!octx->reused_session &&
     Curl_ssl_scache_use(cf, data) && !conn_cfg->verifystatus) {
    ସଂରଚନା Curl_ssl_session *scs = ଖାଲି;

    result = Curl_ssl_scache_take(cf, data, peer->scache_ଚାବି, &scs);
    ଯଦି(!result && scs && scs->sdata && scs->sdata_len) {
      (ଶୂନ୍ୟ)ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, data, peer->scache_ଚାବି, scs);
  }

  ଯଦି(alpns.ଗଣନା_2) {
    ସଂରଚନା alpn_proto_buf proto;
    memset(&proto, 0, ଆକାର(proto));
    result = Curl_alpn_to_proto_buf(&proto, &alpns);
    ଯଦି(result) {
      failf(data, "Error determining ALPN");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
    ଯଦି(SSL_set_alpn_protos(octx->ssl, proto.data, proto.len)) {
      failf(data, "Error setting ALPN");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
ସ୍ଥିର CURLcode ossl_init_ech(ସଂରଚନା ossl_ctx *octx,
                              ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ssl_peer *peer)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା trying_ech_now = 0;

  ଯଦି(!Curl_ssl_ech_enabled(cf))
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(conn_config->ech == CURLECH_GREASE) {
    infof(data, "ECH: will GREASE ClientHello");
#ifdef HAVE_BORINGSSL_LIKE
    SSL_set_enable_ech_grease(octx->ssl, 1);
#ଅନ୍ୟଥା
    SSL_set_options(octx->ssl, SSL_OP_ECH_GREASE);
#endif
  }
  ଅନ୍ୟଥା ଯଦି(conn_config->ech && conn_config->ech_config) {
#ifdef HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode result;

    ଯଦି(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * strlen(conn_config->ech_config);
    result = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    ଯଦି(result || !ech_config) {
      infof(data, "ECH: cannot base64 decode ECHConfig from command line");
      ଯଦି(conn_config->ech == CURLECH_HARD)
        ଫେରନ୍ତୁ result;
    }
    ଯଦି(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      ଯଦି(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#ଅନ୍ୟଥା
    size_t ech_config_len = 0;
    ଯଦି(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = strlen(conn_config->ech_config);
    ଯଦି(SSL_set1_ech_config_list(octx->ssl,
                                (ଅଚଳ uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      ଯଦି(conn_config->ech == CURLECH_HARD)
        ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
    ଅନ୍ୟଥା
      trying_ech_now = 1;
#endif /* HAVE_BORINGSSL_LIKE */
    infof(data, "ECH: ECHConfig from command line");
  }
  ଅନ୍ୟଥା {
    ଅଚଳ ସଂରଚନା Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, peer->origin);

    ଯଦି(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ଅଚଳ unsigned ଅକ୍ଷର *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(data, "ECH: ECHConfig from HTTPS RR");
      ଯଦି(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(data, "ECH: SSL_set1_ech_config_list failed");
        ଯଦି(conn_config->ech == CURLECH_HARD)
          ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
      }
      ଅନ୍ୟଥା {
        trying_ech_now = 1;
        infof(data, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    ଅନ୍ୟଥା {
      infof(data, "ECH: requested but no ECHConfig available");
      ଯଦି(conn_config->ech == CURLECH_HARD)
        ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
  }
#ifdef HAVE_BORINGSSL_LIKE
  (ଶୂନ୍ୟ)peer;
  ଯଦି(trying_ech_now && conn_config->ech_public) {
    infof(data, "ECH: setting public_name not supported with BoringSSL");
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }
#ଅନ୍ୟଥା
  ଯଦି(trying_ech_now && conn_config->ech_public) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
    infof(data, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    ଯଦି(ret != 1) {
      infof(data, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
  }
#endif /* HAVE_BORINGSSL_LIKE */
  ଯଦି(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(data, "ECH: cannot force TLSv1.3 [ERROR]");
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

ସ୍ଥିର CURLcode ossl_init_ssl(ସଂରଚନା ossl_ctx *octx,
                              ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା ssl_peer *peer,
                              ଅଚଳ ସଂରଚନା alpn_spec *alpns_requested,
                              ଶୂନ୍ୟ *ssl_user_data,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  ଯଦି(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  ଯଦି(!octx->ssl) {
    failf(data, "SSL: could not create a context (handle)");
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_data(octx->ssl, ssl_user_data);

#ifndef OPENSSL_NO_OCSP
  ଯଦି(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#endif

  SSL_set_connect_state(octx->ssl);

  ଯଦି(peer->sni) {
    ଯଦି(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(data, "Failed set SNI");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
  }

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode result = ossl_init_ech(octx, cf, data, peer);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  ଫେରନ୍ତୁ ossl_init_session_and_alpns(octx, cf, data, peer,
                                     alpns_requested, sess_reuse_cb);
}

ସ୍ଥିର CURLcode ossl_init_method(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ssl_peer *peer,
                                 ଅଚଳ SSL_METHOD **pmethod,
                                 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pssl_version_min)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = ଖାଲି;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  ବାଛ(peer->transport) {
  ସ୍ଥିତି TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    ବାଛ(*pssl_version_min) {
    ସ୍ଥିତି CURL_SSLVERSION_TLSv1:
    ସ୍ଥିତି CURL_SSLVERSION_TLSv1_0:
    ସ୍ଥିତି CURL_SSLVERSION_TLSv1_1:
    ସ୍ଥିତି CURL_SSLVERSION_TLSv1_2:
    ସ୍ଥିତି CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_SSLVERSION_SSLv2:
      failf(data, "No SSLv2 support");
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
    ସ୍ଥିତି CURL_SSLVERSION_SSLv3:
      failf(data, "No SSLv3 support");
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    ଯଦି(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(data, "QUIC needs at least TLS version 1.3");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "unsupported transport %d in SSL init", peer->transport);
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  ଫେରନ୍ତୁ *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(ସଂରଚନା ossl_ctx *octx,
                            ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା ssl_peer *peer,
                            ଅଚଳ ସଂରଚନା alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            ଶୂନ୍ୟ *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            ଶୂନ୍ୟ *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ciphers;
  ଅଚଳ SSL_METHOD *req_method = ଖାଲି;
  ctx_option_t ctx_options = 0;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  ଅକ୍ଷର * ଅଚଳ ssl_cert = conn_config->clientcert;
  ଅଚଳ ସଂରଚନା curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_cert_type = conn_config->cert_type;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_version_min = CURL_SSLVERSION_DEFAULT;
  ଅକ୍ଷର error_buffer[256];

  /* Make funny stuff to get random input */
  result = ossl_seed(data);
  ଯଦି(!result) {
    ଯଦି(CURL_EASY_STR(data, STRING_SSL_ENGINE))
      result = ossl_set_engine(data, octx,
                               CURL_EASY_STR(data, STRING_SSL_ENGINE));
    ଯଦି(!result && data->set.engine_default)
      result = ossl_set_engine_default(data, octx);
  }

  ଯଦି(!result) {
    ssl_config->certverifyresult = !X509_V_OK;
    result = ossl_init_method(cf, data, peer, &req_method, &ssl_version_min);
  }
  ଯଦି(result)
    ଫେରନ୍ତୁ result;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ifdef OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#endif
    SSL_CTX_new(req_method);

  ଯଦି(!octx->ssl_ctx) {
    failf(data, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_buffer, ଆକାର(error_buffer)));
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଯଦି(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(data->set.fdebug && data->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_ସନ୍ଦେଶ_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_ସନ୍ଦେଶ_callback_arg(octx->ssl_ctx, cf);
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
  ଯଦି(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_ଭର୍ତ୍ତି_EMPTY_FRAGMENTS;

  DEBUGASSERT(ssl_version_min != CURL_SSLVERSION_DEFAULT);
  ବାଛ(ssl_version_min) {
  ସ୍ଥିତି CURL_SSLVERSION_SSLv2:
  ସ୍ଥିତି CURL_SSLVERSION_SSLv3:
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    result = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
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
  ଯଦି(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = ଖାଲି;
  ଯଦି(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    ଯଦି(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(data, "failed setting cipher list: %s", ciphers);
      ଫେରନ୍ତୁ CURLE_SSL_CIPHER;
    }
    infof(data, "Cipher selection: %s", ciphers);
  }

#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    ଅଚଳ ଅକ୍ଷର *ciphers13 = conn_config->cipher_list13;
    ଯଦି(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      ଯଦି(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(data, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        ଫେରନ୍ତୁ CURLE_SSL_CIPHER;
      }
      infof(data, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#endif

  ଯଦି(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    result = client_cert(data, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->ଚାବି, conn_config->ଚାବି_blob,
                         conn_config->ଚାବି_type,
                         conn_config->ଚାବି_passwd);
    ଯଦି(result)
      /* failf() is already done in client_cert() */
      ଫେରନ୍ତୁ result;
  }

#ifdef HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#endif

  {
    ଅଚଳ ଅକ୍ଷର *curves = conn_config->curves;
    ଯଦି(curves) {
#ifdef HAVE_BORINGSSL_LIKE
#define OSSL_CURVE_CAST(x) (x)
#ଅନ୍ୟଥା
#define OSSL_CURVE_CAST(x) (ଅକ୍ଷର *)CURL_UNCONST(x)
#endif
      ଯଦି(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(data, "failed setting curves list: '%s'", curves);
        ଫେରନ୍ତୁ CURLE_SSL_CIPHER;
      }
    }
  }

#ifdef HAVE_SSL_CTX_SET1_SIGALGS
#define OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    ଅଚଳ ଅକ୍ଷର *signature_algorithms = conn_config->signature_algorithms;
    ଯଦି(signature_algorithms) {
      ଯଦି(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(data, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        ଫେରନ୍ତୁ CURLE_SSL_CIPHER;
      }
    }
  }
#endif

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, ଖାଲି);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#ଯଦି !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  ଯଦି(Curl_tls_keylog_enabled()) {
    SSL_CTX_set_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#endif

  ଯଦି(cb_new_session) {
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
  ଯଦି(data->set.ssl_fsslctx) {
    ସଂରଚନା Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    ଯଦି(!octx->x509_store_setup) {
      result = Curl_ssl_setup_x509_store(cf, data, octx);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, data, easy_fsslctx);
    result = (*data->set.ssl_fsslctx)(data, octx->ssl_ctx,
                                      data->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    ଯଦି(result) {
      failf(data, "error signaled by SSL ctx callback");
      ଫେରନ୍ତୁ result;
    }
  }

  ଫେରନ୍ତୁ ossl_init_ssl(octx, cf, data, peer, alpns_requested,
                       ssl_user_data, sess_reuse_cb);
}

ସ୍ଥିର CURLcode ossl_on_session_reuse(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା alpn_spec *alpns,
                                      ସଂରଚନା Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  ଫେରନ୍ତୁ Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max);
}

ଶୂନ୍ୟ Curl_ossl_report_handshake(ସଂରଚନା Curl_easy *data, ସଂରଚନା ossl_ctx *octx)
{
#ifdef CURLVERBOSE
  ଯଦି(Curl_trc_is_verbose(data)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା psigtype_nid = NID_undef;
    ଅଚଳ ଅକ୍ଷର *negotiated_group_name = ଖାଲି;

#ifdef HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#ଯଦି OPENSSL_VERSION_ସଂଖ୍ୟା >= 0x30200000L
    negotiated_group_name = SSL_get0_group_name(octx->ssl);
#ଅନ୍ୟଥା
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
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)octx;
#endif /* CURLVERBOSE */
}

ସ୍ଥିର CURLcode ossl_connect_step1(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  result = Curl_ossl_ctx_init(octx, cf, data, &connssl->peer,
                              connssl->alpn, ଖାଲି, ଖାଲି,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  octx->bio_method = ossl_bio_cf_method_create();
  ଯଦି(!octx->bio_method)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  ଯଦି(!bio)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

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
#ଅନ୍ୟଥା
  SSL_set_bio(octx->ssl, bio, bio);
#endif

  ଯଦି(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ସଂରଚନା alpn_proto_buf proto;
    memset(&proto, 0, ଆକାର(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }

  connssl->connecting_state = ssl_connect_2;
  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ossl_trace_ech_retry_configs(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data, SSL *ssl,
                                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason)
{
  CURLcode result = CURLE_OK;
  size_t rcl = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv = 1;
#ifndef HAVE_BORINGSSL_LIKE
  ଅକ୍ଷର *inner = ଖାଲି;
  uint8_t *rcs = ଖାଲି;
  ଅକ୍ଷର *outer = ଖାଲି;
#ଅନ୍ୟଥା
  ଅଚଳ ଅକ୍ଷର *inner = ଖାଲି;
  ଅଚଳ uint8_t *rcs = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *outer = ଖାଲି;
  size_t out_name_len = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା servername_type = 0;
#endif
  NOVERBOSE((ଶୂନ୍ୟ)reason);

  /* nothing to trace if not doing ECH */
  ଯଦି(!Curl_ssl_ech_enabled(cf))
    ଫେରନ୍ତୁ rv;
#ifndef HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#ଅନ୍ୟଥା
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rcl;
#endif

  ଯଦି(rv && rcs) {
    ଅକ୍ଷର *b64str = ଖାଲି;
    size_t blen = 0;

    result = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    ଯଦି(!result && b64str) {
      infof(data, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ifndef HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#ଅନ୍ୟଥା
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#endif
    }
  }
  ଅନ୍ୟଥା
    infof(data, "ECH: no retry_configs (rv = %d)", rv);
#ifndef HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#endif
  ଫେରନ୍ତୁ rv;
}

#endif

ସ୍ଥିର CURLcode ossl_connect_step2(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  ସଂରଚନା ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  ଯଦି(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, octx);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    octx->x509_store_setup = TRUE;
  }

#ଯଦି !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  ଯଦି(Curl_tls_keylog_enabled() && !octx->keylog_done)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_done);
#endif

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  ଯଦି(err != 1) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(data, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    ଯଦି(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
    ଯଦି(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
#ifdef SSL_ERROR_WANT_ASYNC
    ଯଦି(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
#endif
#ifdef SSL_ERROR_WANT_RETRY_VERIFY
    ଯଦି(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(data, TRUE);
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
#endif
    ଅନ୍ୟଥା {
      /* untreated error */
      sslerr_t errdetail;
      ଅକ୍ଷର error_buffer[256] = "";
      CURLcode result;
      long lerr;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା lib;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      ଯଦି((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#ifdef SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#endif
         )) {
        result = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_result(octx->ssl);
        ଯଦି(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        ଅନ୍ୟଥା
          failf(data, "%s", "SSL certificate verification failed");
      }
#ifdef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      ଅନ୍ୟଥା ଯଦି((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        result = CURLE_SSL_CLIENTCERT;
        failf(data, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_buffer, ଆକାର(error_buffer)));
      }
#endif
#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
      ଅନ୍ୟଥା ଯଦି((lib == ERR_LIB_SSL) &&
#ifndef HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#ଅନ୍ୟଥା
              (reason == SSL_R_ECH_REJECTED)) {
#endif /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, reason);

        result = CURLE_ECH_REQUIRED;
        failf(data, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_buffer, ଆକାର(error_buffer)));
      }
#endif
      ଅନ୍ୟଥା {
        result = CURLE_SSL_CONNECT_ERROR;
        failf(data, "TLS connect error: %s",
              ossl_strerror(errdetail, error_buffer, ଆକାର(error_buffer)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      ଯଦି(result == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        ଅକ୍ଷର extramsg[80] = "";
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;

        ଯଦି(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, ଆକାର(extramsg));
        failf(data, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      ଫେରନ୍ତୁ result;
    }
  }
  ଅନ୍ୟଥା {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(data, octx);

#ଯଦି defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    ଯଦି(Curl_ssl_ech_enabled(cf)) {
      ସଂରଚନା ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      ଅକ୍ଷର *inner = ଖାଲି, *outer = ଖାଲି;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv;
      VERBOSE(ଅଚଳ ଅକ୍ଷର *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      ବାଛ(rv) {
      ସ୍ଥିତି SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି SSL_ECH_STATUS_BAD_NAME: {
        ଯଦି(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        ଅନ୍ୟଥା {
          VERBOSE(status = "bad name (unexpected)");
        }
        ଭାଙ୍ଗ;
      }
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        VERBOSE(status = "unexpected status");
        infof(data, "ECH: unexpected status %d", rv);
      }
      infof(data, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      ଯଦି(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, 0);
      }
      ଯଦି(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(data, "ECH: ech-hard failed");
        ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
      }
    }
    ଅନ୍ୟଥା {
      infof(data, "ECH: result: status is not attempted");
    }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    ଯଦି(connssl->alpn) {
      ଅଚଳ unsigned ଅକ୍ଷର *neg_protocol;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      ଫେରନ୍ତୁ Curl_alpn_set_negotiated(cf, data, connssl, neg_protocol, len);
    }

    ଫେରନ୍ତୁ CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
ସ୍ଥିର CURLcode ossl_pkp_pin_peer_pubkey(ସଂରଚନା Curl_easy *data, X509 *cert,
                                         ଅଚଳ ଅକ୍ଷର *pinnedpubkey)
{
  /* Scratch */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା len1 = 0, len2 = 0;
  unsigned ଅକ୍ଷର *buff1 = ଖାଲି, *temp = ଖାଲି;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ଯଦି(!pinnedpubkey)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(!cert)
    ଫେରନ୍ତୁ result;

  କର {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), ଖାଲି);
    ଯଦି(len1 < 1)
      ଭାଙ୍ଗ; /* failed */

    buff1 = temp = curlx_malloc(len1);
    ଯଦି(!buff1)
      ଭାଙ୍ଗ; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &temp);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    ଯଦି((len1 != len2) || !temp || ((temp - buff1) != len1))
      ଭାଙ୍ଗ; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } ଯେତେବେଳେ(0);

  ଯଦି(buff1)
    curlx_free(buff1);

  ଫେରନ୍ତୁ result;
}

#ifdef CURLVERBOSE
#ଯଦି !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_ସଂଖ୍ୟା) && LIBRESSL_VERSION_ସଂଖ୍ୟା < 0x3060000fL)
ସ୍ଥିର ଶୂନ୍ୟ infof_certstack(ସଂରଚନା Curl_easy *data, ଅଚଳ SSL *ssl)
{
  ଗଦା_OF(X509) *certstack;
  long verify_result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_cert_levels;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cert_level;

  ଯଦି(!Curl_trc_is_verbose(data))
    ଫେରନ୍ତୁ;

  verify_result = SSL_get_verify_result(ssl);
  ଯଦି(verify_result != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  ଅନ୍ୟଥା
    certstack = SSL_get0_verified_chain(ssl);
  ଯଦି(!certstack)
    ଫେରନ୍ତୁ;
  ଅଙ୍କ_cert_levels = sk_X509_ଅଙ୍କ(certstack);

  ପାଇଁ(cert_level = 0; cert_level < ଅଙ୍କ_cert_levels; cert_level++) {
    ଅକ୍ଷର cert_algorithm[80] = "";
    ଅକ୍ଷର group_name_final[80] = "";
    ଅଚଳ X509_ALGOR *palg_cert = ଖାଲି;
    ଅଚଳ ASN1_OBJECT *paobj_cert = ଖାଲି;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି_bits;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଚାବି_sec_bits;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_group_name;
    ଅଚଳ ଅକ୍ଷର *type_name;

    current_cert = sk_X509_ମୂଲ୍ୟ(certstack, cert_level);
    ଯଦି(!current_cert)
      ଜାରି;

    current_pkey = X509_get0_pubkey(current_cert);
    ଯଦି(!current_pkey)
      ଜାରି;

    X509_get0_signature(ଖାଲି, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, ଖାଲି, ଖାଲି, palg_cert);
    OBJ_obj2txt(cert_algorithm, ଆକାର(cert_algorithm), paobj_cert, 0);

    ଚାବି_bits = EVP_PKEY_bits(current_pkey);
#ifndef HAVE_OPENSSL3
#define EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#endif
    ଚାବି_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#ifdef HAVE_OPENSSL3
    {
      ଅକ୍ଷର group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               ଆକାର(group_name), ଖାଲି);
      curl_msnprintf(group_name_final, ଆକାର(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#ଅନ୍ୟଥା
    get_group_name = 0;
    type_name = ଖାଲି;
#endif

    infof(data, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          ଚାବି_bits, ଚାବି_sec_bits, cert_algorithm);
  }
}
#ଅନ୍ୟଥା
#define infof_certstack(data, ssl)
#endif
#endif /* CURLVERBOSE */

ସ୍ଥିର CURLcode ossl_check_issuer(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  X509 *server_cert)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = ଖାଲି;
  BIO *fp = ଖାଲି;
  ଅକ୍ଷର err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode result = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  ଯଦି(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->data,
                         (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)conn_config->issuercert_blob->len);
    ଯଦି(!fp) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, ଆକାର(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }
  }
  ଅନ୍ୟଥା ଯଦି(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    ଯଦି(!fp) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, ଆକାର(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ out;
    }

    ଯଦି(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      ଯଦି(verify_enabled)
        failf(data, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ଯାଅ out;
    }
  }

  ଯଦି(fp) {
    issuer = PEM_read_bio_X509(fp, ଖାଲି, ZERO_NULL, ଖାଲି);
    ଯଦି(!issuer) {
      ଯଦି(verify_enabled)
        failf(data, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ଯାଅ out;
    }

    ଯଦି(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      ଯଦି(verify_enabled)
        failf(data, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ଯାଅ out;
    }

    infof(data, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  ଯଦି(fp)
    BIO_free(fp);
  ଯଦି(issuer)
    X509_free(issuer);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *pinned(ସଂରଚନା Curl_cfilter *cf,
                          ସଂରଚନା Curl_easy *data)
{
  (ଶୂନ୍ୟ)cf;
  ଫେରନ୍ତୁ
#ifndef CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#endif
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
}

ସ୍ଥିର CURLcode ossl_check_pinned_ଚାବି(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      X509 *server_cert)
{
  CURLcode result = CURLE_OK;
  ଅଚଳ ଅକ୍ଷର *ptr = pinned(cf, data);
  ଯଦି(ptr) {
    result = ossl_pkp_pin_peer_pubkey(data, server_cert, ptr);
    ଯଦି(result)
      failf(data, "SSL: public key does not match pinned public key");
  }
  ଫେରନ୍ତୁ result;
}

#ifdef CURLVERBOSE
#define MAX_CERT_NAME_LENGTH 2048
ସ୍ଥିର CURLcode ossl_infof_cert(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                X509 *server_cert)
{
  BIO *mem = ଖାଲି;
  ସଂରଚନା dynbuf dname;
  ଅକ୍ଷର err_buf[256] = "";
  ଅକ୍ଷର *buf;
  long len;
  CURLcode result = CURLE_OK;

  ଯଦି(!Curl_trc_is_verbose(data))
    ଫେରନ୍ତୁ CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_LENGTH);
  mem = BIO_new(BIO_s_mem());
  ଯଦି(!mem) {
    failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, ଆକାର(err_buf)));
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  infof(data, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  result = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(data, "  subject: %s", result ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_data(mem, (ଅକ୍ଷର **)&buf);
  infof(data, "  start date: %.*s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, buf);
  (ଶୂନ୍ୟ)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_data(mem, (ଅକ୍ଷର **)&buf);
  infof(data, "  expire date: %.*s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, buf);
  (ଶୂନ୍ୟ)BIO_reset(mem);

  result = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  ଯଦି(result) /* should be only fatal stuff like OOM */
    ଯାଅ out;
  infof(data, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  ଫେରନ୍ତୁ result;
}
#endif /* CURLVERBOSE */

#ifdef USE_APPLE_SECTRUST
ସଂରଚନା ossl_certs_ctx {
  ଗଦା_OF(X509) *sk;
  size_t ଅଙ୍କ_certs;
  unsigned ଅକ୍ଷର *last_der;
};

ସ୍ଥିର CURLcode ossl_chain_get_der(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ଶୂନ୍ୟ *user_data,
                                   size_t i,
                                   unsigned ଅକ୍ଷର **pder,
                                   size_t *pder_len)
{
  ସଂରଚନା ossl_certs_ctx *chain = user_data;
  X509 *cert;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = ଖାଲି;

  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
  *pder_len = 0;
  *pder = ଖାଲି;

  ଯଦି(i >= chain->ଅଙ୍କ_certs)
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  cert = sk_X509_ମୂଲ୍ୟ(chain->sk, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)i);
  ଯଦି(!cert)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  ଯଦି(der_len < 0)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (size_t)der_len;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ossl_apple_verify(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ossl_ctx *octx,
                                  ସଂରଚନା ssl_peer *peer)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା ossl_certs_ctx chain;
  CURLcode result;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, ଆକାର(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.ଅଙ୍କ_certs = chain.sk ? sk_X509_ଅଙ୍କ(chain.sk) : 0;

  ଯଦି(!chain.ଅଙ୍କ_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    ଯଦି(!octx->reused_session) {
      failf(data, "SSL: could not get peer certificate chain");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    ଅନ୍ୟଥା {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(data, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      ଫେରନ୍ତୁ CURLE_OK;
    }
  }
  ଅନ୍ୟଥା {
#ifdef HAVE_BORINGSSL_LIKE
    ଅଚଳ uint8_t *ocsp_data = ଖାଲି;
#ଅନ୍ୟଥା
    unsigned ଅକ୍ଷର *ocsp_data = ଖାଲି;
#endif
    long ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    ଯଦି(!octx->reused_session)
      ocsp_len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_data);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    ଯଦି(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_data = ଖାଲି;
    }

    result = Curl_vtls_apple_verify(cf, data, peer, chain.ଅଙ୍କ_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_data, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = ଖାଲି;
  }
  octx->sectrust_verified = !result;
  ଫେରନ୍ତୁ result;
}
#endif /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ossl_ctx *octx,
                                   ସଂରଚନା ssl_peer *peer)
{
  ସଂରଚନା connectdata *conn = cf->conn;
  ସଂରଚନା ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  long ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  ଯଦି(data->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    result = ossl_certchain(data, octx->ssl);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  ଯଦି(!server_cert) {
    /* no verification at all, this maybe acceptable */
    ଯଦି(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, data))
      ଯାଅ out;

    failf(data, "SSL: could not get peer certificate");
    result = CURLE_PEER_FAILED_VERIFICATION;
    ଯାଅ out;
  }

#ifdef CURLVERBOSE
  result = ossl_infof_cert(cf, data, server_cert);
  ଯଦି(result)
    ଯାଅ out;
  infof_certstack(data, octx->ssl);
#endif

  ଯଦି(conn_config->verifyhost) {
    result = ossl_verifyhost(data, conn, peer, server_cert);
    ଯଦି(result)
      ଯାଅ out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_result(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(data, "OpenSSL verify result: %lx", (unsigned long)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  ଯଦି(verified)
    infof(data, "SSL certificate verified via OpenSSL.");

#ifdef USE_APPLE_SECTRUST
  ଯଦି(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    result = ossl_apple_verify(cf, data, octx, peer);
    ଯଦି(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      ଯାଅ out; /* unexpected error */
    ଯଦି(octx->sectrust_verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#endif

  ଯଦି(!verified) {
    /* no trust established, report the OpenSSL status */
    ଯଦି(conn_config->verifypeer) {
      failf(data, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      result = CURLE_PEER_FAILED_VERIFICATION;
      ଯାଅ out;
    }
    infof(data, " SSL certificate verification failed, continuing anyway!");
  }

#ifndef OPENSSL_NO_OCSP
  ଯଦି(conn_config->verifystatus &&
#ifdef USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, data)) &&
#endif
     !octx->reused_session) {
    result = verifystatus(cf, data, octx);
    ଯଦି(result)
      ଯାଅ out;
  }
#endif

  result = ossl_check_issuer(cf, data, server_cert);
  ଯଦି(result)
    ଯାଅ out;

  result = ossl_check_pinned_ଚାବି(cf, data, server_cert);

out:
  X509_free(server_cert);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ossl_connect_step3(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  result = Curl_ossl_check_peer_cert(cf, data, octx, &connssl->peer);
  ଯଦି(result)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, data, connssl->peer.scache_ଚାବି);

  ଫେରନ୍ତୁ result;
}

#ifdef HAVE_OPENSSL_EARLYDATA
ସ୍ଥିର CURLcode ossl_send_earlydata(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ଅଚଳ unsigned ଅକ୍ଷର *buf;
  size_t blen, nwritten;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_result = CURLE_OK;
  ଯେତେବେଳେ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_data(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    ଯଦି(rc <= 0) {
      long sslerror;
      ଅକ୍ଷର error_buffer[256];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = SSL_get_error(octx->ssl, rc);

      ବାଛ(err) {
      ସ୍ଥିତି SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        result = CURLE_AGAIN;
        ଯାଅ out;
      ସ୍ଥିତି SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_AGAIN;
        ଯାଅ out;
      ସ୍ଥିତି SSL_ERROR_SYSCALL: {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;

        ଯଦି(octx->io_result == CURLE_AGAIN) {
          result = CURLE_AGAIN;
          ଯାଅ out;
        }
        sslerror = ERR_get_error();
        ଯଦି(sslerror)
          ossl_strerror(sslerror, error_buffer, ଆକାର(error_buffer));
        ଅନ୍ୟଥା ଯଦି(sockerr)
          curlx_strerror(sockerr, error_buffer, ଆକାର(error_buffer));
        ଅନ୍ୟଥା
          curl_msnprintf(error_buffer, ଆକାର(error_buffer), "%s",
                         SSL_ERROR_to_str(err));

        failf(data, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_SEND_ERROR;
        ଯାଅ out;
      }
      ସ୍ଥିତି SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(data, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_buffer, ଆକାର(error_buffer)));
        result = CURLE_SEND_ERROR;
        ଯାଅ out;
      }
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* a true error */
        failf(data, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        result = CURLE_SEND_ERROR;
        ଯାଅ out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ଫେରନ୍ତୁ result;
}
#endif /* HAVE_OPENSSL_EARLYDATA */

ସ୍ଥିର CURLcode ossl_connect(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             bool *done)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  ଯଦି(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ଯଦି(connssl->connecting_state == ssl_connect_1) {
    ଯଦି(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
      ଫେରନ୍ତୁ CURLE_OK;
    }
    CURL_TRC_CF(data, cf, "ossl_connect, step1");
    result = ossl_connect_step1(cf, data);
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(data, cf, "ossl_connect, step2");
#ifdef HAVE_OPENSSL_EARLYDATA
    ଯଦି(connssl->earlydata_state == ssl_earlydata_await) {
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(connssl->earlydata_state == ssl_earlydata_sending) {
      result = ossl_send_earlydata(cf, data);
      ଯଦି(result)
        ଯାଅ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#endif
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    result = ossl_connect_step2(cf, data);
    ଯଦି(result)
      ଯାଅ out;
  }

  ଯଦି(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(data, cf, "ossl_connect, step3");
    result = ossl_connect_step3(cf, data);
    ଯଦି(result)
      ଯାଅ out;
    connssl->connecting_state = ssl_connect_done;
#ifdef HAVE_OPENSSL_EARLYDATA
    ଯଦି(connssl->earlydata_state > ssl_earlydata_none) {
      ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_data_status(octx->ssl) == SSL_EARLY_DATA_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
  }

  ଯଦି(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(data, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  ଯଦି(result == CURLE_AGAIN) {
    *done = FALSE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool ossl_data_pending(ସଂରଚନା Curl_cfilter *cf,
                              ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ (bool)connssl->ନିବେଶ_pending;
}

ସ୍ଥିର CURLcode ossl_send(ସଂରଚନା Curl_cfilter *cf,
                          ସଂରଚନା Curl_easy *data,
                          ଅଚଳ ଶୂନ୍ୟ *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
  ଅକ୍ଷର error_buffer[256];
  sslerr_t sslerror;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା memlen;
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)INT_MAX) ? INT_MAX : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len;
  ଯଦି(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    ଯଦି(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  ଯଦି(nwritten > 0) {
    ଯଦି(nwritten > memlen) {
      failf(data, "SSL_write(len=%d) returned %d", memlen, nwritten);
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
    *pnwritten = (size_t)nwritten;
  }
  ଅନ୍ୟଥା {
    err = SSL_get_error(octx->ssl, nwritten);

    ବାଛ(err) {
    ସ୍ଥିତି SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      result = CURLE_AGAIN;
      ଯାଅ out;
    ସ୍ଥିତି SSL_ERROR_WANT_WRITE:
      result = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      ଯାଅ out;
    ସ୍ଥିତି SSL_ERROR_SYSCALL: {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;

      ଯଦି(octx->io_result == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        result = CURLE_AGAIN;
        ଯାଅ out;
      }
      sslerror = ERR_get_error();
      ଯଦି(sslerror)
        ossl_strerror(sslerror, error_buffer, ଆକାର(error_buffer));
      ଅନ୍ୟଥା ଯଦି(sockerr)
        curlx_strerror(sockerr, error_buffer, ଆକାର(error_buffer));
      ଅନ୍ୟଥା
        curl_msnprintf(error_buffer, ଆକାର(error_buffer), "%s",
                       SSL_ERROR_to_str(err));

      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_buffer, sockerr);
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
    ସ୍ଥିତି SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(data, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_buffer, ଆକାର(error_buffer)));
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* a true error */
      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      result = CURLE_SEND_ERROR;
      ଯାଅ out;
    }
  }

out:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ossl_recv(ସଂରଚନା Curl_cfilter *cf,
                          ସଂରଚନା Curl_easy *data,   /* transfer */
                          ଅକ୍ଷର *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  ଅକ୍ଷର error_buffer[256];
  unsigned long sslerror;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା buffsize;
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)INT_MAX) ? INT_MAX : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  ଯଦି(nread > 0)
    *pnread = (size_t)nread;
  ଅନ୍ୟଥା {
    /* failed SSL_read */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = SSL_get_error(octx->ssl, nread);

    ବାଛ(err) {
    ସ୍ଥିତି SSL_ERROR_NONE: /* this is not an error */
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      ଯଦି(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(data, cf, "TLS close_notify");
      ଭାଙ୍ଗ;
    ସ୍ଥିତି SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      ଯାଅ out;
    ସ୍ଥିତି SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      ଯାଅ out;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      ଯଦି(octx->io_result == CURLE_AGAIN) {
        result = CURLE_AGAIN;
        ଯାଅ out;
      }
      sslerror = ERR_get_error();
      ଯଦି((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
        ଯଦି(sslerror)
          ossl_strerror(sslerror, error_buffer, ଆକାର(error_buffer));
        ଅନ୍ୟଥା ଯଦି(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_buffer, ଆକାର(error_buffer));
        ଅନ୍ୟଥା
          curl_msnprintf(error_buffer, ଆକାର(error_buffer), "%s",
                         SSL_ERROR_to_str(err));
        failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_RECV_ERROR;
        ଯାଅ out;
      }
      ଅନ୍ୟଥା ଯଦି(err == SSL_ERROR_SYSCALL) {
        ଯଦି(octx->io_result) {
          /* logging handling in underlying filter already */
          result = octx->io_result;
        }
        ଅନ୍ୟଥା ଯଦି(connssl->peer_closed) {
          failf(data, "Connection closed abruptly");
          result = CURLE_RECV_ERROR;
        }
        ଅନ୍ୟଥା {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
          ଯଦି(sockerr)
            curlx_strerror(sockerr, error_buffer, ଆକାର(error_buffer));
          ଅନ୍ୟଥା {
            curl_msnprintf(error_buffer, ଆକାର(error_buffer),
                           "Connection closed abruptly");
          }
          failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_buffer, sockerr);
          result = CURLE_RECV_ERROR;
        }
        ଯାଅ out;
      }
    }
  }

out:
  ଯଦି((!result && !*pnread) || (result == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->ନିବେଶ_pending = FALSE;
  }
  CURL_TRC_CF(data, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread, connssl->ନିବେଶ_pending);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ossl_get_channel_binding(ସଂରଚନା Curl_easy *data,
                                         int8_t sockindex,
                                         ସଂରଚନା dynbuf *binding)
{
  X509 *cert;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mdnid;
  bool no_digest_acceptable = FALSE;
  ଅଚଳ EVP_MD *algo_type = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *algo_name = ଖାଲି;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length;
  unsigned ଅକ୍ଷର buf[EVP_MAX_MD_SIZE];

  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର prefix[] = "tls-server-end-point:";
  ସଂରଚନା connectdata *conn = data->conn;
  ସଂରଚନା Curl_cfilter *cf = conn->cfilter[sockindex];
  ସଂରଚନା ossl_ctx *octx = ଖାଲି;
  CURLcode result = CURLE_OK;

  କର {
    ଅଚଳ ସଂରଚନା Curl_cftype *cft = cf->cft;
    ସଂରଚନା ssl_connect_data *connssl = cf->ctx;

    ଯଦି(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
      ଭାଙ୍ଗ;
    }

    cf = cf->next;
  } ଯେତେବେଳେ(cf);

  ଯଦି(!octx) {
    failf(data, "Failed to find the SSL filter");
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  ଯଦି(!cert)
    /* No server certificate, do not do channel binding */
    ଫେରନ୍ତୁ CURLE_OK;

#ifdef HAVE_OPENSSL3
  {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    ଯଦି(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(data, "certificate signature algorithm not recognized");
      result = CURLE_SSL_INVALIDCERTSTATUS;
      ଯାଅ out;
    }

    ଯଦି(mdnid != NID_undef) {
      ଯଦି(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      ଅନ୍ୟଥା
        algo_type = EVP_get_digestbynid(mdnid);
    }
    ଅନ୍ୟଥା ଯଦି(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      ଅକ୍ଷର mdname[128] = "";
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = EVP_PKEY_get_default_digest_name(pkey, mdname, ଆକାର(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      ଯଦି(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      ଅନ୍ୟଥା ଯଦି(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(data, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#ଅନ୍ୟଥା /* HAVE_OPENSSL3 */

  ଯଦି(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, ଖାଲି)) {
    failf(data,
          "Unable to find digest NID for certificate signature algorithm");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  ଯଦି(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  ଅନ୍ୟଥା {
    algo_type = EVP_get_digestbynid(mdnid);
    ଯଦି(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(data, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      result = CURLE_SSL_INVALIDCERTSTATUS;
      ଯାଅ out;
    }
  }

#endif /* HAVE_OPENSSL3, else */

  ଯଦି(!algo_type) {
    ଯଦି(no_digest_acceptable) {
      infof(data, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      result = CURLE_OK;
      ଯାଅ out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(data, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ out;
  }

  ଯଦି(!X509_digest(cert, algo_type, buf, &length)) {
    failf(data, "X509_digest() failed for channel binding");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    ଯାଅ out;
  }

  /* Append "tls-server-end-point:" */
  result = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  ଯଦି(result)
    ଯାଅ out;

  /* Append digest */
  result = curlx_dyn_addn(binding, buf, length);

out:
  X509_free(cert);
  ଫେରନ୍ତୁ result;
}

size_t Curl_ossl_version(ଅକ୍ଷର *buffer, size_t size)
{
#ifdef LIBRESSL_VERSION_ସଂଖ୍ୟା
  ଅକ୍ଷର *p;
  size_t ଗଣନା_2;
  ଅଚଳ ଅକ୍ଷର *ver = OpenSSL_version(OPENSSL_VERSION);
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  ଯଦି(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  ଗଣନା_2 = curl_msnprintf(buffer, size, "%s/%s", OSSL_PACKAGE, ver);
  ପାଇଁ(p = buffer; *p; ++p) {
    ଯଦି(ISBLANK(*p))
      *p = '_';
  }
  ଫେରନ୍ତୁ ଗଣନା_2;
#ନଚେତଯଦି defined(OPENSSL_IS_AWSLC)
  ଫେରନ୍ତୁ curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_ସଂଖ୍ୟା_STRING);
#ନଚେତଯଦି defined(OPENSSL_IS_BORINGSSL)
#ifdef CURL_BORINGSSL_VERSION
  ଫେରନ୍ତୁ curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ curl_msnprintf(buffer, size, OSSL_PACKAGE);
#endif
#ଅନ୍ୟଥା /* OpenSSL 3+ */
  ଫେରନ୍ତୁ curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
}

/* can be called with data == NULL */
ସ୍ଥିର CURLcode ossl_random(ସଂରଚନା Curl_easy *data,
                            unsigned ଅକ୍ଷର *entropy, size_t length)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ଯଦି(data) {
    ଯଦି(ossl_seed(data)) /* Initiate the seed if not already done */
      ଫେରନ୍ତୁ CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  ଅନ୍ୟଥା {
    ଯଦି(!rand_enough())
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(length));
  ଫେରନ୍ତୁ rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

ସ୍ଥିର CURLcode ossl_sha256sum(ଅଚଳ unsigned ଅକ୍ଷର *ନିବେଶ,
                               size_t len,
                               unsigned ଅକ୍ଷର *sha256sum /* output */,
                               size_t unused)
{
  CURLcode result = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (ଶୂନ୍ୟ)unused;

  mdctx = EVP_MD_CTX_new();
  ଯଦି(!mdctx)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  ଯଦି(!EVP_DigestInit_ex(mdctx, EVP_sha256(), ଖାଲି)) {
    result = CURLE_FAILED_INIT;
    ଯାଅ out;
  }
  ଯଦି(!EVP_DigestUpdate(mdctx, ନିବେଶ, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, ଖାଲି))
    result = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool ossl_cert_status_request(ଶୂନ୍ୟ)
{
#ifndef OPENSSL_NO_OCSP
  ଫେରନ୍ତୁ TRUE;
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ FALSE;
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ *ossl_get_internals(ସଂରଚନା ssl_connect_data *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  ସଂରଚନା ossl_ctx *octx = (ସଂରଚନା ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  ଫେରନ୍ତୁ info == CURLINFO_TLS_SESSION ?
    (ଶୂନ୍ୟ *)octx->ssl_ctx : (ଶୂନ୍ୟ *)octx->ssl;
}

ଅଚଳ ସଂରଚନା Curl_ssl Curl_ssl_openssl = {
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

  ଆକାର(ସଂରଚନା ossl_ctx),

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
