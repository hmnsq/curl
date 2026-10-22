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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_OPENSSL

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "formdata.h" /* for the boundary function */
#അന്തര്ഭാവ "url.h" /* for the SSL config check function */
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "vtls/openssl.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/vtls_int.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "vtls/keylog.h"
#അന്തര്ഭാവ "vtls/hostcheck.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "multiif.h"
#അന്തര്ഭാവ "curlx/strerr.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curlx/strcopy.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"
#അന്തര്ഭാവ "vtls/apple.h"
#ifdef USE_ECH
#അന്തര്ഭാവ "curlx/base64.h"
#endif

#അന്തര്ഭാവ <openssl/rand.h>
#അന്തര്ഭാവ <openssl/x509v3.h>
#ifndef OPENSSL_NO_DSA
#അന്തര്ഭാവ <openssl/dsa.h>
#endif
#അന്തര്ഭാവ <openssl/dh.h>
#അന്തര്ഭാവ <openssl/err.h>
#അന്തര്ഭാവ <openssl/conf.h>
#അന്തര്ഭാവ <openssl/bn.h>
#അന്തര്ഭാവ <openssl/rsa.h>
#അന്തര്ഭാവ <openssl/bio.h>
#അന്തര്ഭാവ <openssl/pkcs12.h>
#അന്തര്ഭാവ <openssl/tls1.h>
#അന്തര്ഭാവ <openssl/evp.h>

#എങ്കിൽ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#അന്തര്ഭാവ <openssl/ech.h>
#endif

#ifndef OPENSSL_NO_OCSP
#അന്തര്ഭാവ <openssl/ocsp.h>
#endif

#എങ്കിൽ !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#define USE_OPENSSL_ENGINE
#അന്തര്ഭാവ <openssl/engine.h>
#endif

#ifdef LIBRESSL_VERSION_സംഖ്യ
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  എങ്കിൽ LIBRESSL_VERSION_സംഖ്യ < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#അല്ലെങ്കിലെങ്കിൽ !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#endif

#എങ്കിൽ defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#അന്തര്ഭാവ <openssl/provider.h>
#അന്തര്ഭാവ <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#define OPENSSL_HAS_PROVIDERS
#endif

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#എങ്കിൽ !defined(LIBRESSL_VERSION_സംഖ്യ) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_താക്കോൽ_EXPLICIT_PARAMS))
#define HAVE_SSL_CTX_SET_DEFAULT_READ_BUFFER_LEN 1
#endif

#എങ്കിൽ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#അന്തര്ഭാവ <openssl/ui.h>
#endif

#ifdef HAVE_OPENSSL3
#define HAVE_EVP_PKEY_GET_PARAMS 1
#endif

#ifdef HAVE_EVP_PKEY_GET_PARAMS
#അന്തര്ഭാവ <openssl/core_names.h>
#define DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = ശൂന്യ
#define FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#അല്ലെങ്കിൽ
#define DECLARE_PKEY_PARAM_BIGNUM(name) സ്ഥിരം BIGNUM *name
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
#ifndef LIBRESSL_VERSION_സംഖ്യ
#define HAVE_SSL_CTX_SET1_SIGALGS
#endif

#ifdef LIBRESSL_VERSION_സംഖ്യ
#define OSSL_PACKAGE "LibreSSL"
#അല്ലെങ്കിലെങ്കിൽ defined(OPENSSL_IS_AWSLC)
#define OSSL_PACKAGE "AWS-LC"
#അല്ലെങ്കിലെങ്കിൽ defined(OPENSSL_IS_BORINGSSL)
#define OSSL_PACKAGE "BoringSSL"
#അല്ലെങ്കിലെങ്കിൽ defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#define OSSL_PACKAGE "quictls"
#അല്ലെങ്കിൽ
#define OSSL_PACKAGE "OpenSSL"
#endif

#ifdef HAVE_BORINGSSL_LIKE
തരനിർവചനം size_t numcert_t;
തരനിർവചനം uint32_t sslerr_t;
#അല്ലെങ്കിൽ
തരനിർവചനം പൂർണ്ണസംഖ്യ numcert_t;
തരനിർവചനം unsigned long sslerr_t;
#endif
#define ossl_valsize_t numcert_t

#എങ്കിൽ OPENSSL_VERSION_സംഖ്യ >= 0x40100000L
#define ossl_ASN1_STRING_get_length ASN1_STRING_get_length
#അല്ലെങ്കിൽ
സ്ഥിതം size_t ossl_ASN1_STRING_get_length(സ്ഥിരം ASN1_STRING *str)
{
   സ്ഥിരം പൂർണ്ണസംഖ്യ length = ASN1_STRING_length(str);
   തിരികെ length >= 0 ? (size_t)length : 0;
}
#endif

സ്ഥിതം CURLcode തള്ളുക_certinfo(ഘടന Curl_easy *data,
                              BIO *mem, സ്ഥിരം അക്ഷരം *label, പൂർണ്ണസംഖ്യ അക്കം)
  WARN_UNUSED_RESULT;

സ്ഥിതം CURLcode തള്ളുക_certinfo(ഘടന Curl_easy *data,
                              BIO *mem, സ്ഥിരം അക്ഷരം *label, പൂർണ്ണസംഖ്യ അക്കം)
{
  അക്ഷരം *ptr;
  long len = BIO_get_mem_data(mem, &ptr);
  CURLcode result = Curl_ssl_തള്ളുക_certinfo_len(data, അക്കം, label, ptr, len);
  (ശൂന്യം)BIO_reset(mem);
  തിരികെ result;
}

സ്ഥിതം CURLcode pubkey_show(ഘടന Curl_easy *data,
                            BIO *mem,
                            പൂർണ്ണസംഖ്യ അക്കം,
                            സ്ഥിരം അക്ഷരം *type,
                            സ്ഥിരം അക്ഷരം *name,
                            സ്ഥിരം BIGNUM *bn) WARN_UNUSED_RESULT;

സ്ഥിതം CURLcode pubkey_show(ഘടന Curl_easy *data,
                            BIO *mem,
                            പൂർണ്ണസംഖ്യ അക്കം,
                            സ്ഥിരം അക്ഷരം *type,
                            സ്ഥിരം അക്ഷരം *name,
                            സ്ഥിരം BIGNUM *bn)
{
  അക്ഷരം namebuf[32];

  curl_msnprintf(namebuf, വലുപ്പം(namebuf), "%s(%s)", type, name);

  എങ്കിൽ(bn)
    BN_print(mem, bn);
  തിരികെ തള്ളുക_certinfo(data, mem, namebuf, അക്കം);
}

#define print_pubkey_BN(_type, _name, അക്കം_2)           \
  pubkey_show(data, mem, അക്കം_2, #_type, #_name, _name)

സ്ഥിതം പൂർണ്ണസംഖ്യ asn1_object_dump(സ്ഥിരം ASN1_OBJECT *a, അക്ഷരം *buf, size_t len)
{
  പൂർണ്ണസംഖ്യ i = i2t_ASN1_OBJECT(buf, (പൂർണ്ണസംഖ്യ)len, a);
  തിരികെ (i >= (പൂർണ്ണസംഖ്യ)len);  /* buffer too small */
}

സ്ഥിതം CURLcode X509V3_ext(ഘടന Curl_easy *data,
                           പൂർണ്ണസംഖ്യ certnum,
                           സ്ഥിരം അട്ടി_OF(X509_EXTENSION) *extsarg)
{
  പൂർണ്ണസംഖ്യ i;
  CURLcode result = CURLE_OK;
#ifdef LIBRESSL_VERSION_സംഖ്യ
  അട്ടി_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#അല്ലെങ്കിൽ
  സ്ഥിരം അട്ടി_OF(X509_EXTENSION) *exts = extsarg;
#endif

  എങ്കിൽ((പൂർണ്ണസംഖ്യ)sk_X509_EXTENSION_അക്കം(exts) <= 0)
    /* no extensions, bail out */
    തിരികെ result;

  വേണ്ടി(i = 0; i < (പൂർണ്ണസംഖ്യ)sk_X509_EXTENSION_അക്കം(exts); i++) {
    സ്ഥിരം ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_മൂല്യം(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    അക്ഷരം namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    എങ്കിൽ(!bio_out)
      തിരികെ result;

    obj = X509_EXTENSION_get_object(ext);

    എങ്കിൽ(asn1_object_dump(obj, namebuf, വലുപ്പം(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    എങ്കിൽ(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (സ്ഥിരം ASN1_STRING *)X509_EXTENSION_get_data(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    result = Curl_ssl_തള്ളുക_certinfo_len(data, certnum, namebuf, biomem->data,
                                        biomem->length);
    BIO_free(bio_out);
    എങ്കിൽ(result)
      നിർത്തുക;
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode get_pkey_rsa(ഘടന Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, പൂർണ്ണസംഖ്യ i)
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
#അല്ലെങ്കിൽ
  RSA_get0_താക്കോൽ(rsa, &n, &e, ശൂന്യ);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (പൂർണ്ണസംഖ്യ)(n ? BN_അക്കം_bits(n) : 0));
  result = തള്ളുക_certinfo(data, mem, "RSA Public Key", i);
  എങ്കിൽ(!result) {
    result = print_pubkey_BN(rsa, n, i);
    എങ്കിൽ(!result)
      result = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  തിരികെ result;
}

#ifndef OPENSSL_NO_DSA
സ്ഥിതം CURLcode get_pkey_dsa(ഘടന Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, പൂർണ്ണസംഖ്യ i)
{
  CURLcode result = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_താക്കോൽ);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_താക്കോൽ, &pub_താക്കോൽ);
#അല്ലെങ്കിൽ
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_താക്കോൽ(dsa, &pub_താക്കോൽ, ശൂന്യ);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dsa, p, i);
  എങ്കിൽ(!result)
    result = print_pubkey_BN(dsa, q, i);
  എങ്കിൽ(!result)
    result = print_pubkey_BN(dsa, g, i);
  എങ്കിൽ(!result)
    result = print_pubkey_BN(dsa, pub_താക്കോൽ, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_താക്കോൽ);
  തിരികെ result;
}
#endif /* !OPENSSL_NO_DSA */

സ്ഥിതം CURLcode get_pkey_dh(ഘടന Curl_easy *data,
                            EVP_PKEY *pubkey, BIO *mem, പൂർണ്ണസംഖ്യ i)
{
  CURLcode result;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_താക്കോൽ);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_താക്കോൽ, &pub_താക്കോൽ);
#അല്ലെങ്കിൽ
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_താക്കോൽ(dh, &pub_താക്കോൽ, ശൂന്യ);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dh, p, i);
  എങ്കിൽ(!result)
    result = print_pubkey_BN(dh, q, i);
  എങ്കിൽ(!result)
    result = print_pubkey_BN(dh, g, i);
  എങ്കിൽ(!result)
    result = print_pubkey_BN(dh, pub_താക്കോൽ, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_താക്കോൽ);
  തിരികെ result;
}

#ifdef HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
തരനിർവചനം സ്ഥിരം X509_PUBKEY pubkeytype_t;
#അല്ലെങ്കിൽ
തരനിർവചനം X509_PUBKEY pubkeytype_t;
#endif

സ്ഥിതം CURLcode ossl_certchain(ഘടന Curl_easy *data, SSL *ssl)
{
  CURLcode result;
  അട്ടി_OF(X509) *sk;
  പൂർണ്ണസംഖ്യ i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  എങ്കിൽ(!sk)
    തിരികെ CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_അക്കം(sk);
  എങ്കിൽ(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(data, "%d certificates is more than allowed (%d)", (പൂർണ്ണസംഖ്യ)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  result = Curl_ssl_init_certinfo(data, (പൂർണ്ണസംഖ്യ)numcerts);
  എങ്കിൽ(result)
    തിരികെ result;

  mem = BIO_new(BIO_s_mem());
  എങ്കിൽ(!mem)
    result = CURLE_OUT_OF_MEMORY;

  വേണ്ടി(i = 0; !result && (i < (പൂർണ്ണസംഖ്യ)numcerts); i++) {
    X509 *x = sk_X509_മൂല്യം(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = ശൂന്യ;
    size_t j;
    സ്ഥിരം ASN1_BIT_STRING *psig = ശൂന്യ;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    result = തള്ളുക_certinfo(data, mem, "Subject", i);
    എങ്കിൽ(result)
      നിർത്തുക;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    result = തള്ളുക_certinfo(data, mem, "Issuer", i);
    എങ്കിൽ(result)
      നിർത്തുക;

    BIO_printf(mem, "%lx", (unsigned long)X509_get_version(x));
    result = തള്ളുക_certinfo(data, mem, "Version", i);
    എങ്കിൽ(result)
      നിർത്തുക;

    {
      ASN1_INTEGER *അക്കം = X509_get_serial_സംഖ്യ(x);
      സ്ഥിരം unsigned അക്ഷരം *numdata = ASN1_STRING_get0_data(അക്കം);
      സ്ഥിരം size_t numlen = ossl_ASN1_STRING_get_length(അക്കം);
      എങ്കിൽ(ASN1_STRING_type(അക്കം) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      വേണ്ടി(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    result = തള്ളുക_certinfo(data, mem, "Serial Number", i);
    എങ്കിൽ(result)
      നിർത്തുക;

    {
      സ്ഥിരം X509_ALGOR *sigalg = ശൂന്യ;
      pubkeytype_t *xpubkey = ശൂന്യ;
      ASN1_OBJECT *pubkeyoid = ശൂന്യ;

      X509_get0_signature(&psig, &sigalg, x);
      എങ്കിൽ(sigalg) {
        സ്ഥിരം ASN1_OBJECT *sigalgoid = ശൂന്യ;
        X509_ALGOR_get0(&sigalgoid, ശൂന്യ, ശൂന്യ, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        result = തള്ളുക_certinfo(data, mem, "Signature Algorithm", i);
        എങ്കിൽ(result)
          നിർത്തുക;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      എങ്കിൽ(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, ശൂന്യ, ശൂന്യ, ശൂന്യ, xpubkey);
        എങ്കിൽ(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          result = തള്ളുക_certinfo(data, mem, "Public Key Algorithm", i);
          എങ്കിൽ(result)
            നിർത്തുക;
        }
      }

      result = X509V3_ext(data, i, X509_get0_extensions(x));
      എങ്കിൽ(result)
        നിർത്തുക;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    result = തള്ളുക_certinfo(data, mem, "Start date", i);
    എങ്കിൽ(result)
      നിർത്തുക;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    result = തള്ളുക_certinfo(data, mem, "Expire date", i);
    എങ്കിൽ(result)
      നിർത്തുക;

    pubkey = X509_get_pubkey(x);
    എങ്കിൽ(!pubkey)
      infof(data, "   Unable to load public key");
    അല്ലെങ്കിൽ {
      തിരഞ്ഞെടുപ്പ്(EVP_PKEY_id(pubkey)) {
      അവസ്ഥ EVP_PKEY_RSA:
        result = get_pkey_rsa(data, pubkey, mem, i);
        നിർത്തുക;

#ifndef OPENSSL_NO_DSA
      അവസ്ഥ EVP_PKEY_DSA:
        result = get_pkey_dsa(data, pubkey, mem, i);
        നിർത്തുക;
#endif

      അവസ്ഥ EVP_PKEY_DH:
        result = get_pkey_dh(data, pubkey, mem, i);
        നിർത്തുക;
      }
      EVP_PKEY_free(pubkey);
    }

    എങ്കിൽ(!result && psig) {
      സ്ഥിരം unsigned അക്ഷരം *psigdata = ASN1_STRING_get0_data(psig);
      സ്ഥിരം size_t psiglen = ossl_ASN1_STRING_get_length(psig);
      വേണ്ടി(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      result = തള്ളുക_certinfo(data, mem, "Signature", i);
    }

    എങ്കിൽ(!result) {
      PEM_write_bio_X509(mem, x);
      result = തള്ളുക_certinfo(data, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  എങ്കിൽ(result)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(data);

  തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_data(bio, ശൂന്യ);
  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ossl_bio_cf_destroy(BIO *bio)
{
  എങ്കിൽ(!bio)
    തിരികെ 0;
  തിരികെ 1;
}

സ്ഥിതം long ossl_bio_cf_ctrl(BIO *bio, പൂർണ്ണസംഖ്യ cmd, long അക്കം, ശൂന്യം *ptr)
{
  ഘടന Curl_cfilter *cf = BIO_get_data(bio);
  long ret = 1;

  (ശൂന്യം)cf;
  (ശൂന്യം)ptr;
  തിരഞ്ഞെടുപ്പ്(cmd) {
  അവസ്ഥ BIO_CTRL_GET_CLOSE:
    ret = (long)BIO_get_shutdown(bio);
    നിർത്തുക;
  അവസ്ഥ BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (പൂർണ്ണസംഖ്യ)അക്കം);
    നിർത്തുക;
  അവസ്ഥ BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    നിർത്തുക;
  അവസ്ഥ BIO_CTRL_DUP:
    ret = 1;
    നിർത്തുക;
  അവസ്ഥ BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    ഘടന ssl_connect_data *connssl = cf->ctx;
    തിരികെ connssl->peer_closed;
  }
  സ്ഥിരസ്ഥിതി:
    ret = 0;
    നിർത്തുക;
  }
  തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ossl_bio_cf_out_write(BIO *bio, സ്ഥിരം അക്ഷരം *buf, പൂർണ്ണസംഖ്യ blen)
{
  ഘടന Curl_cfilter *cf = BIO_get_data(bio);
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  എങ്കിൽ(blen < 0)
    തിരികെ 0;

  result = Curl_conn_cf_send(cf->next, data,
                             (സ്ഥിരം uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (പൂർണ്ണസംഖ്യ)result, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    തിരികെ -1;
  }
  തിരികെ (പൂർണ്ണസംഖ്യ)nwritten;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ossl_bio_cf_in_read(BIO *bio, അക്ഷരം *buf, പൂർണ്ണസംഖ്യ blen)
{
  ഘടന Curl_cfilter *cf = BIO_get_data(bio);
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result, r2;

  DEBUGASSERT(data);
  /* OpenSSL catches this case, so should we. */
  എങ്കിൽ(!buf)
    തിരികെ 0;
  എങ്കിൽ(blen < 0)
    തിരികെ 0;

  result = Curl_conn_cf_recv(cf->next, data, buf, (size_t)blen, &nread);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (പൂർണ്ണസംഖ്യ)result, nread);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  അല്ലെങ്കിൽ {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->നിവേശം_pending = TRUE;
    എങ്കിൽ(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  എങ്കിൽ(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, data, octx);
    എങ്കിൽ(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_result = r2;
      തിരികെ -1;
    }
    octx->x509_store_setup = TRUE;
  }
  തിരികെ result ? -1 : (പൂർണ്ണസംഖ്യ)nread;
}

സ്ഥിതം BIO_METHOD *ossl_bio_cf_method_create(ശൂന്യം)
{
  BIO_METHOD *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  എങ്കിൽ(m) {
    BIO_meth_set_write(m, &ossl_bio_cf_out_write);
    BIO_meth_set_read(m, &ossl_bio_cf_in_read);
    BIO_meth_set_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_set_create(m, &ossl_bio_cf_create);
    BIO_meth_set_destroy(m, &ossl_bio_cf_destroy);
  }
  തിരികെ m;
}

സ്ഥിതം ശൂന്യം ossl_bio_cf_method_free(BIO_METHOD *m)
{
  എങ്കിൽ(m)
    BIO_meth_free(m);
}

#ifndef HAVE_KEYLOG_UPSTREAM
#ifdef HAVE_KEYLOG_CALLBACK
സ്ഥിതം ശൂന്യം ossl_keylog_callback(സ്ഥിരം SSL *ssl, സ്ഥിരം അക്ഷരം *വരി)
{
  (ശൂന്യം)ssl;

  Curl_tls_keylog_write_വരി(വരി);
}
#അല്ലെങ്കിൽ
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
സ്ഥിതം ശൂന്യം ossl_log_tls12_secret(സ്ഥിരം SSL *ssl, bool *keylog_done)
{
  സ്ഥിരം SSL_SESSION *session;
  unsigned അക്ഷരം client_random[SSL3_RANDOM_SIZE];
  unsigned അക്ഷരം master_താക്കോൽ[SSL_MAX_MASTER_താക്കോൽ_LENGTH];
  പൂർണ്ണസംഖ്യ master_താക്കോൽ_length = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  എങ്കിൽ(!session || *keylog_done) {
    ERR_എടുക്കുക_to_mark();
    തിരികെ;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_SIZE);
  master_താക്കോൽ_length = (പൂർണ്ണസംഖ്യ)
    SSL_SESSION_get_master_താക്കോൽ(session, master_താക്കോൽ, SSL_MAX_MASTER_താക്കോൽ_LENGTH);

  ERR_എടുക്കുക_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  എങ്കിൽ(master_താക്കോൽ_length <= 0)
    തിരികെ;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        വലുപ്പം(client_random),
                        master_താക്കോൽ, master_താക്കോൽ_length);
}
#endif /* !HAVE_KEYLOG_CALLBACK */
#endif /* HAVE_KEYLOG_UPSTREAM */

സ്ഥിതം സ്ഥിരം അക്ഷരം *SSL_ERROR_to_str(പൂർണ്ണസംഖ്യ err)
{
  തിരഞ്ഞെടുപ്പ്(err) {
  അവസ്ഥ SSL_ERROR_NONE:
    തിരികെ "SSL_ERROR_NONE";
  അവസ്ഥ SSL_ERROR_SSL:
    തിരികെ "SSL_ERROR_SSL";
  അവസ്ഥ SSL_ERROR_WANT_READ:
    തിരികെ "SSL_ERROR_WANT_READ";
  അവസ്ഥ SSL_ERROR_WANT_WRITE:
    തിരികെ "SSL_ERROR_WANT_WRITE";
  അവസ്ഥ SSL_ERROR_WANT_X509_LOOKUP:
    തിരികെ "SSL_ERROR_WANT_X509_LOOKUP";
  അവസ്ഥ SSL_ERROR_SYSCALL:
    തിരികെ "SSL_ERROR_SYSCALL";
  അവസ്ഥ SSL_ERROR_ZERO_RETURN:
    തിരികെ "SSL_ERROR_ZERO_RETURN";
  അവസ്ഥ SSL_ERROR_WANT_CONNECT:
    തിരികെ "SSL_ERROR_WANT_CONNECT";
  അവസ്ഥ SSL_ERROR_WANT_ACCEPT:
    തിരികെ "SSL_ERROR_WANT_ACCEPT";
#ifdef SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  അവസ്ഥ SSL_ERROR_WANT_ASYNC:
    തിരികെ "SSL_ERROR_WANT_ASYNC";
#endif
#ifdef SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  അവസ്ഥ SSL_ERROR_WANT_ASYNC_JOB:
    തിരികെ "SSL_ERROR_WANT_ASYNC_JOB";
#endif
#ifdef SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  അവസ്ഥ SSL_ERROR_WANT_CLIENT_HELLO_CB:
    തിരികെ "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#endif
  സ്ഥിരസ്ഥിതി:
    തിരികെ "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
സ്ഥിതം അക്ഷരം *ossl_strerror(unsigned long error, അക്ഷരം *buf, size_t size)
{
  size_t len;
  DEBUGASSERT(size);
  *buf = '\0';

  len = Curl_ossl_version(buf, size);
  DEBUGASSERT(len < (size - 2));
  എങ്കിൽ(len < (size - 2)) {
    buf += len;
    size -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ifdef HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, size);
#അല്ലെങ്കിൽ
  ERR_error_string_n(error, buf, size);
#endif

  എങ്കിൽ(!*buf) {
    സ്ഥിരം അക്ഷരം *സന്ദേശം = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, size, സന്ദേശം, strlen(സന്ദേശം));
  }

  തിരികെ buf;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ passwd_callback(അക്ഷരം *buf, പൂർണ്ണസംഖ്യ അക്കം, പൂർണ്ണസംഖ്യ encrypting, ശൂന്യം *password)
{
  DEBUGASSERT(encrypting == 0);

  എങ്കിൽ(!encrypting && അക്കം >= 0 && password) {
    പൂർണ്ണസംഖ്യ klen = curlx_uztosi(strlen((അക്ഷരം *)password));
    എങ്കിൽ(അക്കം > klen) {
      memcpy(buf, password, klen + 1);
      തിരികെ klen;
    }
  }
  തിരികെ 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
സ്ഥിതം bool rand_enough(ശൂന്യം)
{
  തിരികെ RAND_status() != 0;
}

സ്ഥിതം CURLcode ossl_seed(ഘടന Curl_easy *data)
{
  /* This might get called before it has been added to a multi handle */
  എങ്കിൽ(data->multi && data->multi->ssl_seeded)
    തിരികെ CURLE_OK;

  എങ്കിൽ(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    എങ്കിൽ(data->multi)
      data->multi->ssl_seeded = TRUE;
    തിരികെ CURLE_OK;
  }
  failf(data, "Insufficient randomness");
  തിരികെ CURLE_SSL_CONNECT_ERROR;
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
സ്ഥിതം പൂർണ്ണസംഖ്യ ossl_do_file_type(സ്ഥിരം അക്ഷരം *type)
{
  എങ്കിൽ(!type || !type[0])
    തിരികെ SSL_FILETYPE_PEM;
  എങ്കിൽ(curl_strequal(type, "PEM"))
    തിരികെ SSL_FILETYPE_PEM;
  എങ്കിൽ(curl_strequal(type, "DER"))
    തിരികെ SSL_FILETYPE_ASN1;
  എങ്കിൽ(curl_strequal(type, "PROV"))
    തിരികെ SSL_FILETYPE_PROVIDER;
  എങ്കിൽ(curl_strequal(type, "ENG"))
    തിരികെ SSL_FILETYPE_ENGINE;
  എങ്കിൽ(curl_strequal(type, "P12"))
    തിരികെ SSL_FILETYPE_PKCS12;
  തിരികെ -1;
}

#എങ്കിൽ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  സ്ഥിരം അക്ഷരം *password;
  തിരഞ്ഞെടുപ്പ്(UI_get_string_type(uis)) {
  അവസ്ഥ UIT_PROMPT:
  അവസ്ഥ UIT_VERIFY:
    password = (സ്ഥിരം അക്ഷരം *)UI_get0_user_data(ui);
    എങ്കിൽ(password && (UI_get_നിവേശം_flags(uis) & UI_നിവേശം_FLAG_DEFAULT_PWD)) {
      UI_set_result(ui, uis, password);
      തിരികെ 1;
    }
    FALLTHROUGH();
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  തിരഞ്ഞെടുപ്പ്(UI_get_string_type(uis)) {
  അവസ്ഥ UIT_PROMPT:
  അവസ്ഥ UIT_VERIFY:
    എങ്കിൽ(UI_get0_user_data(ui) &&
       (UI_get_നിവേശം_flags(uis) & UI_നിവേശം_FLAG_DEFAULT_PWD)) {
      തിരികെ 1;
    }
    FALLTHROUGH();
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
സ്ഥിതം bool is_pkcs11_uri(സ്ഥിരം അക്ഷരം *string)
{
  തിരികെ string && curl_strnequal(string, "pkcs11:", 7);
}

#endif

സ്ഥിതം CURLcode ossl_set_engine(ഘടന Curl_easy *data, ഘടന ossl_ctx *octx,
                                സ്ഥിരം അക്ഷരം *name);
#ifdef OPENSSL_HAS_PROVIDERS
സ്ഥിതം CURLcode ossl_set_provider(ഘടന Curl_easy *data,
                                  ഘടന ossl_ctx *octx,
                                  സ്ഥിരം അക്ഷരം *iname);
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ use_certificate_blob(SSL_CTX *ctx, സ്ഥിരം ഘടന curl_blob *blob,
                                പൂർണ്ണസംഖ്യ type, സ്ഥിരം അക്ഷരം *താക്കോൽ_passwd)
{
  പൂർണ്ണസംഖ്യ ret = 0;
  X509 *x = ശൂന്യ;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->data, (പൂർണ്ണസംഖ്യ)blob->len);
  എങ്കിൽ(!in)
    തിരികെ CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, ശൂന്യ);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, ശൂന്യ, passwd_callback, CURL_UNCONST(താക്കോൽ_passwd));
  }
  അല്ലെങ്കിൽ {
    ret = 0;
    പോകുക end;
  }

  എങ്കിൽ(!x) {
    ret = 0;
    പോകുക end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ use_privatekey_blob(SSL_CTX *ctx, സ്ഥിരം ഘടന curl_blob *blob,
                               പൂർണ്ണസംഖ്യ type, സ്ഥിരം അക്ഷരം *താക്കോൽ_passwd)
{
  പൂർണ്ണസംഖ്യ ret = 0;
  EVP_PKEY *pkey = ശൂന്യ;
  BIO *in = BIO_new_mem_buf(blob->data, (പൂർണ്ണസംഖ്യ)blob->len);
  എങ്കിൽ(!in)
    തിരികെ CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_Private_താക്കോൽ(in, ശൂന്യ, passwd_callback,
                                   CURL_UNCONST(താക്കോൽ_passwd));
  അല്ലെങ്കിൽ എങ്കിൽ(type == SSL_FILETYPE_ASN1)
    pkey = d2i_Private_താക്കോൽ_bio(in, ശൂന്യ);
  അല്ലെങ്കിൽ
    പോകുക end;

  എങ്കിൽ(!pkey)
    പോകുക end;

  ret = SSL_CTX_use_Private_താക്കോൽ(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ use_certificate_chain_blob(SSL_CTX *ctx,
                                      സ്ഥിരം ഘടന curl_blob *blob,
                                      സ്ഥിരം അക്ഷരം *താക്കോൽ_passwd)
{
  പൂർണ്ണസംഖ്യ ret = 0;
  X509 *x = ശൂന്യ;
  BIO *in = BIO_new_mem_buf(blob->data, (പൂർണ്ണസംഖ്യ)blob->len);
  എങ്കിൽ(!in)
    തിരികെ CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, ശൂന്യ,
                            passwd_callback, CURL_UNCONST(താക്കോൽ_passwd));
  എങ്കിൽ(!x)
    പോകുക end;

  ret = SSL_CTX_use_certificate(ctx, x);

  എങ്കിൽ(ERR_peek_error() != 0)
    ret = 0;

  എങ്കിൽ(ret) {
    X509 *ca;
    sslerr_t err;

    എങ്കിൽ(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      പോകുക end;
    }

    ആകുമ്പോൾ((ca = PEM_read_bio_X509(in, ശൂന്യ, passwd_callback,
                                  CURL_UNCONST(താക്കോൽ_passwd))) != ശൂന്യ) {

      എങ്കിൽ(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        പോകുക end;
      }
    }

    err = ERR_peek_last_error();
    എങ്കിൽ((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_വരി))
      ERR_clear_error();
    അല്ലെങ്കിൽ
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ enginecheck(ഘടന Curl_easy *data,
                       ഘടന ossl_ctx *octx,
                       സ്ഥിരം അക്ഷരം *താക്കോൽ_file,
                       സ്ഥിരം അക്ഷരം *താക്കോൽ_passwd)
{
#ifdef USE_OPENSSL_ENGINE
  EVP_PKEY *priv_താക്കോൽ = ശൂന്യ;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  എങ്കിൽ(!octx->p.engine && is_pkcs11_uri(താക്കോൽ_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    തിരികെ 0;

  എങ്കിൽ(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    എങ്കിൽ(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      തിരികെ 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);
    priv_താക്കോൽ = ENGINE_load_private_താക്കോൽ(octx->p.engine, താക്കോൽ_file,
                                       ui_method,
                                       CURL_UNCONST(താക്കോൽ_passwd));
    UI_destroy_method(ui_method);
    എങ്കിൽ(!priv_താക്കോൽ) {
      failf(data, "failed to load private key from crypto engine");
      തിരികെ 0;
    }
    എങ്കിൽ(SSL_CTX_use_Private_താക്കോൽ(ctx, priv_താക്കോൽ) != 1) {
      failf(data, "unable to set private key");
      EVP_PKEY_free(priv_താക്കോൽ);
      തിരികെ 0;
    }
    EVP_PKEY_free(priv_താക്കോൽ);  /* we do not need the handle any more... */
  }
  അല്ലെങ്കിൽ {
    failf(data, "crypto engine not set, cannot load private key");
    തിരികെ 0;
  }
  തിരികെ 1;
#അല്ലെങ്കിൽ
  (ശൂന്യം)octx;
  (ശൂന്യം)താക്കോൽ_file;
  (ശൂന്യം)താക്കോൽ_passwd;
  failf(data, "SSL_FILETYPE_ENGINE not supported for private key");
  തിരികെ 0;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ providercheck(ഘടന Curl_easy *data,
                         ഘടന ossl_ctx *octx,
                         സ്ഥിരം അക്ഷരം *താക്കോൽ_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  അക്ഷരം error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  എങ്കിൽ(!octx->p.provider_loaded && is_pkcs11_uri(താക്കോൽ_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK) {
    തിരികെ 0;
  }

  എങ്കിൽ(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_താക്കോൽ = ശൂന്യ;
    OSSL_STORE_CTX *store = ശൂന്യ;
    OSSL_STORE_INFO *info = ശൂന്യ;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    എങ്കിൽ(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      തിരികെ 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(താക്കോൽ_file, octx->p.libctx,
                               octx->p.propq, ui_method, ശൂന്യ, ശൂന്യ,
                               ശൂന്യ, ശൂന്യ);
    എങ്കിൽ(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      UI_destroy_method(ui_method);
      തിരികെ 0;
    }
    എങ്കിൽ(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    എങ്കിൽ(info) {
      പൂർണ്ണസംഖ്യ ossl_type = OSSL_STORE_INFO_get_type(info);

      എങ്കിൽ(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_താക്കോൽ = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    എങ്കിൽ(!priv_താക്കോൽ) {
      failf(data, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      തിരികെ 0;
    }

    എങ്കിൽ(SSL_CTX_use_Private_താക്കോൽ(ctx, priv_താക്കോൽ) != 1) {
      failf(data, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      EVP_PKEY_free(priv_താക്കോൽ);
      തിരികെ 0;
    }
    EVP_PKEY_free(priv_താക്കോൽ); /* we do not need the handle any more... */
  }
  അല്ലെങ്കിൽ {
    failf(data, "crypto provider not set, cannot load private key");
    തിരികെ 0;
  }
  തിരികെ 1;
#അല്ലെങ്കിൽ
  (ശൂന്യം)octx;
  (ശൂന്യം)താക്കോൽ_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for private key");
  തിരികെ 0;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ engineload(ഘടന Curl_easy *data,
                      ഘടന ossl_ctx *octx,
                      സ്ഥിരം അക്ഷരം *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#എങ്കിൽ defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  അക്ഷരം error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  എങ്കിൽ(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    തിരികെ 0;

  എങ്കിൽ(octx->p.engine) {
    സ്ഥിതം സ്ഥിരം അക്ഷരം cmd_name[] = "LOAD_CERT_CTRL";
    ഘടന {
      സ്ഥിരം അക്ഷരം *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = ശൂന്യ;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    എങ്കിൽ(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), ശൂന്യ)) {
      failf(data, "SSL engine does not support loading certificates");
      തിരികെ 0;
    }

    /* Load the certificate from the engine */
    എങ്കിൽ(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, ശൂന്യ, 1)) {
      failf(data, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      തിരികെ 0;
    }

    എങ്കിൽ(!params.cert) {
      failf(data, "SSL engine did not initialize the certificate properly.");
      തിരികെ 0;
    }

    എങ്കിൽ(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      X509_free(params.cert);
      തിരികെ 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  അല്ലെങ്കിൽ {
    failf(data, "crypto engine not set, cannot load certificate");
    തിരികെ 0;
  }
  തിരികെ 1;
#അല്ലെങ്കിൽ
  (ശൂന്യം)octx;
  (ശൂന്യം)cert_file;
  failf(data, "SSL_FILETYPE_ENGINE not supported for certificate");
  തിരികെ 0;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ providerload(ഘടന Curl_easy *data,
                        ഘടന ossl_ctx *octx,
                        സ്ഥിരം അക്ഷരം *cert_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  അക്ഷരം error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  എങ്കിൽ(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK)
    തിരികെ 0;

  എങ്കിൽ(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = ശൂന്യ;
    X509 *cert = ശൂന്യ;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    പൂർണ്ണസംഖ്യ rc;

    എങ്കിൽ(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      തിരികെ 0;
    }
    എങ്കിൽ(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    എങ്കിൽ(info) {
      പൂർണ്ണസംഖ്യ ossl_type = OSSL_STORE_INFO_get_type(info);

      എങ്കിൽ(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    എങ്കിൽ(!cert) {
      failf(data, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      തിരികെ 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    എങ്കിൽ(rc != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      തിരികെ 0;
    }
  }
  അല്ലെങ്കിൽ {
    failf(data, "crypto provider not set, cannot load certificate");
    തിരികെ 0;
  }
  തിരികെ 1;
#അല്ലെങ്കിൽ
  (ശൂന്യം)octx;
  (ശൂന്യം)cert_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for certificate");
  തിരികെ 0;
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pkcs12load(ഘടന Curl_easy *data,
                      SSL_CTX* ctx,
                      സ്ഥിരം ഘടന curl_blob *cert_blob,
                      സ്ഥിരം അക്ഷരം *cert_file,
                      സ്ഥിരം അക്ഷരം *താക്കോൽ_passwd)
{
  അക്ഷരം error_buffer[256];
  BIO *cert_bio = ശൂന്യ;
  PKCS12 *p12 = ശൂന്യ;
  EVP_PKEY *pri;
  X509 *x509;
  പൂർണ്ണസംഖ്യ cert_done = 0;
  അട്ടി_OF(X509) *ca = ശൂന്യ;
  എങ്കിൽ(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->data, (പൂർണ്ണസംഖ്യ)cert_blob->len);
    എങ്കിൽ(!cert_bio) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      തിരികെ 0;
    }
  }
  അല്ലെങ്കിൽ {
    cert_bio = BIO_new(BIO_s_file());
    എങ്കിൽ(!cert_bio) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
      തിരികെ 0;
    }

    എങ്കിൽ(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(data, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      തിരികെ 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, ശൂന്യ);
  BIO_free(cert_bio);

  എങ്കിൽ(!p12) {
    failf(data, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    തിരികെ 0;
  }

  എങ്കിൽ(!PKCS12_parse(p12, താക്കോൽ_passwd, &pri, &x509, &ca)) {
    failf(data, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, വലുപ്പം(error_buffer)));
    PKCS12_free(p12);
    തിരികെ 0;
  }

  PKCS12_free(p12);

  എങ്കിൽ(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(data, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, വലുപ്പം(error_buffer)));
    പോകുക fail;
  }

  എങ്കിൽ(SSL_CTX_use_Private_താക്കോൽ(ctx, pri) != 1) {
    failf(data, "unable to use private key from PKCS12 file '%s'", cert_file);
    പോകുക fail;
  }

  എങ്കിൽ(!SSL_CTX_check_private_താക്കോൽ(ctx)) {
    failf(data, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    പോകുക fail;
  }
  /* Set Certificate Verification chain */
  എങ്കിൽ(ca) {
    ആകുമ്പോൾ(sk_X509_അക്കം(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_എടുക്കുക(ca);
      എങ്കിൽ(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to client CA list");
        പോകുക fail;
      }
      എങ്കിൽ(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to certificate chain");
        പോകുക fail;
      }
    }
  }

  cert_done = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_എടുക്കുക_free(ca, X509_free);
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic എടുക്കുക
#endif
  എങ്കിൽ(!cert_done)
    തിരികെ 0; /* failure! */
  തിരികെ 1;
}

സ്ഥിതം CURLcode client_cert(ഘടന Curl_easy *data,
                            ഘടന ossl_ctx *octx,
                            അക്ഷരം *cert_file,
                            സ്ഥിരം ഘടന curl_blob *cert_blob,
                            സ്ഥിരം അക്ഷരം *cert_type,
                            അക്ഷരം *താക്കോൽ_file,
                            സ്ഥിരം ഘടന curl_blob *താക്കോൽ_blob,
                            സ്ഥിരം അക്ഷരം *താക്കോൽ_type,
                            അക്ഷരം *താക്കോൽ_passwd)
{
  അക്ഷരം error_buffer[256];
  bool check_privkey = TRUE;
  പൂർണ്ണസംഖ്യ file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  എങ്കിൽ(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    പൂർണ്ണസംഖ്യ cert_use_result;

    എങ്കിൽ(താക്കോൽ_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, താക്കോൽ_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    തിരഞ്ഞെടുപ്പ്(file_type) {
    അവസ്ഥ SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_result = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, താക്കോൽ_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      എങ്കിൽ(cert_use_result != 1) {
        failf(data,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            വലുപ്പം(error_buffer)));
        തിരികെ CURLE_SSL_CERTPROBLEM;
      }
      നിർത്തുക;

    അവസ്ഥ SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_result = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, താക്കോൽ_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      എങ്കിൽ(cert_use_result != 1) {
        failf(data,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            വലുപ്പം(error_buffer)));
        തിരികെ CURLE_SSL_CERTPROBLEM;
      }
      നിർത്തുക;

    അവസ്ഥ SSL_FILETYPE_ENGINE:
      എങ്കിൽ(!cert_file || !engineload(data, octx, cert_file))
        തിരികെ CURLE_SSL_CERTPROBLEM;
      നിർത്തുക;

    അവസ്ഥ SSL_FILETYPE_PROVIDER:
      എങ്കിൽ(!cert_file || !providerload(data, octx, cert_file))
        തിരികെ CURLE_SSL_CERTPROBLEM;
      നിർത്തുക;

    അവസ്ഥ SSL_FILETYPE_PKCS12:
      എങ്കിൽ(!pkcs12load(data, ctx, cert_blob, cert_file, താക്കോൽ_passwd))
        തിരികെ CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      failf(data, "not supported file type '%s' for certificate", cert_type);
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    എങ്കിൽ(!താക്കോൽ_file && !താക്കോൽ_blob) {
      താക്കോൽ_file = cert_file;
      താക്കോൽ_blob = cert_blob;
    }
    അല്ലെങ്കിൽ
      file_type = ossl_do_file_type(താക്കോൽ_type);

    തിരഞ്ഞെടുപ്പ്(file_type) {
    അവസ്ഥ SSL_FILETYPE_PEM:
    അവസ്ഥ SSL_FILETYPE_ASN1:
      cert_use_result = താക്കോൽ_blob ?
        use_privatekey_blob(ctx, താക്കോൽ_blob, file_type, താക്കോൽ_passwd) :
      SSL_CTX_use_Private_താക്കോൽ_file(ctx, താക്കോൽ_file, file_type);
      എങ്കിൽ(cert_use_result != 1) {
        failf(data, "unable to set private key file: '%s' type %s",
              താക്കോൽ_file ? താക്കോൽ_file : "(memory blob)",
              താക്കോൽ_type ? താക്കോൽ_type : "PEM");
        തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
      }
      നിർത്തുക;
    അവസ്ഥ SSL_FILETYPE_ENGINE:
      എങ്കിൽ(!enginecheck(data, octx, താക്കോൽ_file, താക്കോൽ_passwd))
        തിരികെ CURLE_SSL_CERTPROBLEM;
      നിർത്തുക;

    അവസ്ഥ SSL_FILETYPE_PROVIDER:
      എങ്കിൽ(!providercheck(data, octx, താക്കോൽ_file))
        തിരികെ CURLE_SSL_CERTPROBLEM;
      നിർത്തുക;

    അവസ്ഥ SSL_FILETYPE_PKCS12:
      എങ്കിൽ(!pcks12_done) {
        failf(data, "file type P12 for private key not supported");
        തിരികെ CURLE_SSL_CERTPROBLEM;
      }
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      failf(data, "not supported file type for private key");
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    എങ്കിൽ(!ssl) {
      failf(data, "unable to create an SSL structure");
      തിരികെ CURLE_OUT_OF_MEMORY;
    }

    x509 = SSL_get_certificate(ssl);

    എങ്കിൽ(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#എങ്കിൽ !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_താക്കോൽ = SSL_get_privatekey(ssl);
      എങ്കിൽ(EVP_PKEY_id(priv_താക്കോൽ) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_താക്കോൽ);
        എങ്കിൽ(RSA_flags(rsa) & RSA_METHOD_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#endif

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    എങ്കിൽ(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      എങ്കിൽ(!SSL_CTX_check_private_താക്കോൽ(ctx)) {
        failf(data, "Private key does not match the certificate public key");
        തിരികെ CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  തിരികെ CURLE_OK;
}

#ifdef CURLVERBOSE
/* returns non-zero on failure */
സ്ഥിതം CURLcode x509_name_oneline(സ്ഥിരം X509_NAME *a, ഘടന dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  പൂർണ്ണസംഖ്യ rc;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(bio_out) {
    unsigned long flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    എങ്കിൽ(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      result = curlx_dyn_addn(d, biomem->data, biomem->length);
    }
    BIO_free(bio_out);
  }
  തിരികെ result;
}
#endif

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ ossl_init(ശൂന്യം)
{
  സ്ഥിരം uint64_t flags =
#ifdef OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#endif
#ifdef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    OPENSSL_INIT_NO_LOAD_CONFIG |
#അല്ലെങ്കിൽ
    OPENSSL_INIT_LOAD_CONFIG |
#endif
    0;
  OPENSSL_init_ssl(flags, ശൂന്യ);

#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#endif

  തിരികെ 1;
}

/* Global cleanup */
സ്ഥിതം ശൂന്യം ossl_cleanup(ശൂന്യം)
{
#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#endif
}

/* Selects an OpenSSL crypto engine or provider.
 */
സ്ഥിതം CURLcode ossl_set_engine(ഘടന Curl_easy *data, ഘടന ossl_ctx *octx,
                                സ്ഥിരം അക്ഷരം *name)
{
#ifdef USE_OPENSSL_ENGINE
  CURLcode result = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  എങ്കിൽ(e) {

    എങ്കിൽ(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = ശൂന്യ;
    }
    എങ്കിൽ(!ENGINE_init(e)) {
      അക്ഷരം buf[256];

      ENGINE_free(e);
      failf(data, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, വലുപ്പം(buf)));
      result = CURLE_SSL_ENGINE_INITFAILED;
      e = ശൂന്യ;
    }
    അല്ലെങ്കിൽ {
      result = CURLE_OK;
    }
    octx->p.engine = e;
    തിരികെ result;
  }
#endif
#ifdef OPENSSL_HAS_PROVIDERS
  തിരികെ ossl_set_provider(data, octx, name);
#അല്ലെങ്കിൽ
  (ശൂന്യം)octx;
  (ശൂന്യം)name;
  failf(data, "OpenSSL engine not found");
  തിരികെ CURLE_SSL_ENGINE_NOTFOUND;
#endif
}

/* Sets engine as default for all SSL operations
 */
സ്ഥിതം CURLcode ossl_set_engine_default(ഘടന Curl_easy *data,
                                        ഘടന ossl_ctx *octx)
{
#ifdef USE_OPENSSL_ENGINE
  എങ്കിൽ(octx->p.engine) {
    എങ്കിൽ(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(data, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    അല്ലെങ്കിൽ {
      failf(data, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      തിരികെ CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)octx;
#endif
  തിരികെ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
സ്ഥിതം ഘടന curl_slist *ossl_engines_list(ഘടന Curl_easy *data)
{
  ഘടന curl_slist *list = ശൂന്യ;
#ifdef USE_OPENSSL_ENGINE
  ഘടന curl_slist *beg;
  ENGINE *e;

  വേണ്ടി(e = ENGINE_get_first(); e; e = ENGINE_get_next(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    എങ്കിൽ(!beg) {
      curl_slist_free_all(list);
      തിരികെ ശൂന്യ;
    }
    list = beg;
  }
#endif
  (ശൂന്യം)data;
  തിരികെ list;
}

#ifdef OPENSSL_HAS_PROVIDERS

സ്ഥിതം ശൂന്യം ossl_provider_cleanup(ഘടന ossl_ctx *octx)
{
  എങ്കിൽ(!octx)
    തിരികെ;
  എങ്കിൽ(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = ശൂന്യ;
  }
  എങ്കിൽ(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = ശൂന്യ;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = ശൂന്യ;
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
സ്ഥിതം CURLcode ossl_set_provider(ഘടന Curl_easy *data,
                                  ഘടന ossl_ctx *octx,
                                  സ്ഥിരം അക്ഷരം *iname)
{
  അക്ഷരം name[MAX_PROVIDER_LEN + 1];
  ഘടന Curl_str prov;
  സ്ഥിരം അക്ഷരം *propq = ശൂന്യ;

  എങ്കിൽ(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    തിരികെ CURLE_OK;
  }
  എങ്കിൽ(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  memcpy(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  എങ്കിൽ(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    എങ്കിൽ(!libctx)
      തിരികെ CURLE_OUT_OF_MEMORY;
    എങ്കിൽ(propq) {
      octx->p.propq = curlx_strdup(propq);
      എങ്കിൽ(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        തിരികെ CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#ifndef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  എങ്കിൽ(!OSSL_LIB_CTX_load_config(octx->p.libctx, ശൂന്യ)) {
    infof(data, "Failed to load default openssl config. Proceeding.");
  }
#endif

  എങ്കിൽ(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    തിരികെ CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  എങ്കിൽ(!octx->p.provider) {
    അക്ഷരം error_buffer[256];
    failf(data, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_buffer, വലുപ്പം(error_buffer)));
    ossl_provider_cleanup(octx);
    തിരികെ CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  എങ്കിൽ(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(data, "Failed to load base");
    തിരികെ CURLE_SSL_ENGINE_NOTFOUND;
  }
  അല്ലെങ്കിൽ
    octx->p.provider_loaded = TRUE;
  തിരികെ CURLE_OK;
}
#അല്ലെങ്കിൽ /* OPENSSL_HAS_PROVIDERS */
#define ossl_provider_cleanup(x)
#endif

സ്ഥിതം CURLcode ossl_shutdown(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  അക്ഷരം buf[1024];
  പൂർണ്ണസംഖ്യ nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  എങ്കിൽ(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    പോകുക out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  എങ്കിൽ(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    വേണ്ടി(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (പൂർണ്ണസംഖ്യ)വലുപ്പം(buf));
      CURL_TRC_CF(data, cf, "SSL shutdown not sent, read -> %d", nread);
      എങ്കിൽ(nread <= 0)
        നിർത്തുക;
    }
    err = SSL_get_error(octx->ssl, nread);
    എങ്കിൽ(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool നിവേശം_pending;
      /* Yes, it did. */
      എങ്കിൽ(!send_shutdown) {
        CURL_TRC_CF(data, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        പോകുക out;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(!cf->next->cft->is_alive(cf->next, data, &നിവേശം_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(data, cf, "peer closed connection");
        *done = TRUE;
        പോകുക out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  എങ്കിൽ(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    പൂർണ്ണസംഖ്യ rc;
    ERR_clear_error();
    CURL_TRC_CF(data, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    എങ്കിൽ(rc == 1) {
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
      *done = TRUE;
      പോകുക out;
    }
    എങ്കിൽ(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      പോകുക out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  വേണ്ടി(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (പൂർണ്ണസംഖ്യ)വലുപ്പം(buf));
    CURL_TRC_CF(data, cf, "SSL shutdown read -> %d", nread);
    എങ്കിൽ(nread <= 0)
      നിർത്തുക;
  }
  err = SSL_get_error(octx->ssl, nread);
  തിരഞ്ഞെടുപ്പ്(err) {
  അവസ്ഥ SSL_ERROR_ZERO_RETURN: /* no more data */
    എങ്കിൽ(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
    അല്ലെങ്കിൽ
      CURL_TRC_CF(data, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    നിർത്തുക;
  അവസ്ഥ SSL_ERROR_NONE: /* did not get anything */
  അവസ്ഥ SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(data, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    നിർത്തുക;
  അവസ്ഥ SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(data, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(unsigned long sslerr = ERR_get_error());
      CURL_TRC_CF(data, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, വലുപ്പം(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    result = CURLE_OK;
    നിർത്തുക;
  }

out:
  cf->shutdown = (result || *done);
  എങ്കിൽ(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->നിവേശം_pending = FALSE;
  തിരികെ result;
}

/* also used by quic-tls */
ശൂന്യം Curl_osslctx_cleanup(ഘടന ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  എങ്കിൽ(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = ശൂന്യ;
  }
  എങ്കിൽ(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = ശൂന്യ;
    octx->x509_store_setup = FALSE;
  }
  എങ്കിൽ(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = ശൂന്യ;
  }
#ifdef USE_OPENSSL_ENGINE
  എങ്കിൽ(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = ശൂന്യ;
  }
#endif
  ossl_provider_cleanup(octx);
}

സ്ഥിതം ശൂന്യം ossl_close(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  (ശൂന്യം)data;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
സ്ഥിതം ശൂന്യം ossl_close_all(ഘടന Curl_easy *data)
{
  (ശൂന്യം)data;
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
സ്ഥിതം CURLcode ossl_verifyhost(ഘടന Curl_easy *data,
                                ഘടന connectdata *conn,
                                ഘടന ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  പൂർണ്ണസംഖ്യ target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  അട്ടി_OF(GENERAL_NAME) *altnames;
#ifdef USE_IPV6
  ഘടന in6_addr addr;
#അല്ലെങ്കിൽ
  ഘടന in_addr addr;
#endif
  CURLcode result = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = strlen(peer->origin->hostname);

  (ശൂന്യം)conn;
  തിരഞ്ഞെടുപ്പ്(peer->type) {
  അവസ്ഥ CURL_SSL_PEER_IPV4:
    എങ്കിൽ(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      തിരികെ CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = വലുപ്പം(ഘടന in_addr);
    നിർത്തുക;
#ifdef USE_IPV6
  അവസ്ഥ CURL_SSL_PEER_IPV6:
    എങ്കിൽ(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      തിരികെ CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = വലുപ്പം(ഘടന in6_addr);
    നിർത്തുക;
#endif
  അവസ്ഥ CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    DEBUGASSERT(0);
    failf(data, "unexpected SSL peer type: %d", (പൂർണ്ണസംഖ്യ)peer->type);
    തിരികെ CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, ശൂന്യ, ശൂന്യ);

  എങ്കിൽ(altnames) {
#ifdef HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#അല്ലെങ്കിൽ
    പൂർണ്ണസംഖ്യ numalts;
    പൂർണ്ണസംഖ്യ i;
#endif

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_അക്കം(altnames);

    /* loop through all alternatives - until a dnsmatch */
    വേണ്ടി(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      സ്ഥിരം GENERAL_NAME *check = sk_GENERAL_NAME_മൂല്യം(altnames, i);

      എങ്കിൽ(check->type == GEN_DNS)
        dNSName = TRUE;
      അല്ലെങ്കിൽ എങ്കിൽ(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      എങ്കിൽ(check->type == target) {
        /* get data and length */
        സ്ഥിരം അക്ഷരം *altptr = (സ്ഥിരം അക്ഷരം *)ASN1_STRING_get0_data(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_length(check->d.ia5);

        തിരഞ്ഞെടുപ്പ്(target) {
        അവസ്ഥ GEN_DNS: /* name/pattern comparison */
          എങ്കിൽ(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (പൂർണ്ണസംഖ്യ)altlen, altptr);
          }
          നിർത്തുക;

        അവസ്ഥ GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          എങ്കിൽ((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          നിർത്തുക;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  എങ്കിൽ(matched)
    /* an alternative name matched */
    ;
  അല്ലെങ്കിൽ എങ്കിൽ(dNSName || iPAddress) {
    സ്ഥിരം അക്ഷരം *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(data, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(data, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    result = CURLE_PEER_FAILED_VERIFICATION;
  }
  അല്ലെങ്കിൽ {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    പൂർണ്ണസംഖ്യ i = -1;
    unsigned അക്ഷരം *cn = ശൂന്യ;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    സ്ഥിരം X509_NAME *name = X509_get_subject_name(server_cert);
    എങ്കിൽ(name) {
      പൂർണ്ണസംഖ്യ j;
      ആകുമ്പോൾ((j = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    എങ്കിൽ(i >= 0) {
      സ്ഥിരം ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      എങ്കിൽ(tmp) {
        എങ്കിൽ(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_length(tmp);
          cn = (unsigned അക്ഷരം *)CURL_UNCONST(ASN1_STRING_get0_data(tmp));
        }
        അല്ലെങ്കിൽ { /* not a UTF8 name */
          സ്ഥിരം പൂർണ്ണസംഖ്യ utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          എങ്കിൽ(utf8len < 0)
            cnlen = 0;
          അല്ലെങ്കിൽ
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        എങ്കിൽ((cnlen == 0) || !cn)
          result = CURLE_OUT_OF_MEMORY;
        അല്ലെങ്കിൽ എങ്കിൽ(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(data, "SSL: illegal cert name field");
          result = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    എങ്കിൽ(result)
      /* error already detected, pass through */
      ;
    അല്ലെങ്കിൽ എങ്കിൽ(!cn) {
      failf(data, "SSL: unable to obtain common name from peer certificate");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!Curl_cert_hostcheck((സ്ഥിരം അക്ഷരം *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(data, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (പൂർണ്ണസംഖ്യ)cnlen, cn,
            peer->origin->user_hostname);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    അല്ലെങ്കിൽ {
      infof(data, " common name: %.*s (matched)", (പൂർണ്ണസംഖ്യ)cnlen, cn);
    }
    എങ്കിൽ(free_cn)
      OPENSSL_free(cn);
  }

  തിരികെ result;
}

#ifndef OPENSSL_NO_OCSP
സ്ഥിതം CURLcode verifystatus(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             ഘടന ossl_ctx *octx)
{
  പൂർണ്ണസംഖ്യ i, ocsp_status;
#ifdef HAVE_BORINGSSL_LIKE
  സ്ഥിരം uint8_t *status;
#അല്ലെങ്കിൽ
  unsigned അക്ഷരം *status;
#endif
  സ്ഥിരം unsigned അക്ഷരം *p;
  CURLcode result = CURLE_OK;
  OCSP_RESPONSE *rsp = ശൂന്യ;
  OCSP_BASICRESP *br = ശൂന്യ;
  X509_STORE     *st = ശൂന്യ;
  അട്ടി_OF(X509) *ch = ശൂന്യ;
  X509 *cert;
  OCSP_CERTID *id = ശൂന്യ;
  പൂർണ്ണസംഖ്യ cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  പൂർണ്ണസംഖ്യ ret;
  long len;

  (ശൂന്യം)cf;
  DEBUGASSERT(octx);

  len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  എങ്കിൽ(!status) {
    failf(data, "No OCSP response received");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(ശൂന്യ, &p, len);
  എങ്കിൽ(!rsp) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }

  ocsp_status = OCSP_response_status(rsp);
  എങ്കിൽ(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(data, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }

  br = OCSP_response_get1_basic(rsp);
  എങ്കിൽ(!br) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }

#എങ്കിൽ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_സംഖ്യ) && LIBRESSL_VERSION_സംഖ്യ < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#അല്ലെങ്കിൽ
  ch = SSL_get_peer_cert_chain(octx->ssl);
#endif
  എങ്കിൽ(!ch) {
    failf(data, "Could not get peer certificate chain");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  എങ്കിൽ(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(data, "OCSP response verification failed");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  എങ്കിൽ(!cert) {
    failf(data, "Error getting peer certificate");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }

  വേണ്ടി(i = 0; i < (പൂർണ്ണസംഖ്യ)sk_X509_അക്കം(ch); i++) {
    X509 *issuer = sk_X509_മൂല്യം(ch, (ossl_valsize_t)i);
    എങ്കിൽ(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      നിർത്തുക;
    }
  }
  X509_free(cert);

  എങ്കിൽ(!id) {
    failf(data, "Error computing OCSP ID");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  എങ്കിൽ(ret != 1) {
    failf(data, "Could not find certificate ID in OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  എങ്കിൽ(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(data, "OCSP response has expired");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }

  infof(data, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  തിരഞ്ഞെടുപ്പ്(cert_status) {
  അവസ്ഥ V_OCSP_CERTSTATUS_GOOD:
    നിർത്തുക;

  അവസ്ഥ V_OCSP_CERTSTATUS_REVOKED:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    failf(data, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    പോകുക end;

  അവസ്ഥ V_OCSP_CERTSTATUS_UNKNOWN:
  സ്ഥിരസ്ഥിതി:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക end;
  }

end:
  എങ്കിൽ(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  തിരികെ result;
}
#endif

സ്ഥിതം സ്ഥിരം അക്ഷരം *ssl_സന്ദേശം_type(പൂർണ്ണസംഖ്യ ssl_ver, പൂർണ്ണസംഖ്യ സന്ദേശം)
{
  എങ്കിൽ(ssl_ver == SSL3_VERSION_MAJOR) {
    തിരഞ്ഞെടുപ്പ്(സന്ദേശം) {
    അവസ്ഥ SSL3_MT_HELLO_REQUEST:
      തിരികെ "Hello request";
    അവസ്ഥ SSL3_MT_CLIENT_HELLO:
      തിരികെ "Client hello";
    അവസ്ഥ SSL3_MT_SERVER_HELLO:
      തിരികെ "Server hello";
#ifdef SSL3_MT_NEWSESSION_TICKET
    അവസ്ഥ SSL3_MT_NEWSESSION_TICKET:
      തിരികെ "Newsession Ticket";
#endif
    അവസ്ഥ SSL3_MT_CERTIFICATE:
      തിരികെ "Certificate";
    അവസ്ഥ SSL3_MT_SERVER_താക്കോൽ_EXCHANGE:
      തിരികെ "Server key exchange";
    അവസ്ഥ SSL3_MT_CLIENT_താക്കോൽ_EXCHANGE:
      തിരികെ "Client key exchange";
    അവസ്ഥ SSL3_MT_CERTIFICATE_REQUEST:
      തിരികെ "Request CERT";
    അവസ്ഥ SSL3_MT_SERVER_DONE:
      തിരികെ "Server finished";
    അവസ്ഥ SSL3_MT_CERTIFICATE_VERIFY:
      തിരികെ "CERT verify";
    അവസ്ഥ SSL3_MT_FINISHED:
      തിരികെ "Finished";
#ifdef SSL3_MT_CERTIFICATE_STATUS
    അവസ്ഥ SSL3_MT_CERTIFICATE_STATUS:
      തിരികെ "Certificate Status";
#endif
#ifdef SSL3_MT_ENCRYPTED_EXTENSIONS
    അവസ്ഥ SSL3_MT_ENCRYPTED_EXTENSIONS:
      തിരികെ "Encrypted Extensions";
#endif
#ifdef SSL3_MT_SUPPLEMENTAL_DATA
    അവസ്ഥ SSL3_MT_SUPPLEMENTAL_DATA:
      തിരികെ "Supplemental data";
#endif
#ifdef SSL3_MT_END_OF_EARLY_DATA
    അവസ്ഥ SSL3_MT_END_OF_EARLY_DATA:
      തിരികെ "End of early data";
#endif
#ifdef SSL3_MT_താക്കോൽ_UPDATE
    അവസ്ഥ SSL3_MT_താക്കോൽ_UPDATE:
      തിരികെ "Key update";
#endif
#ifdef SSL3_MT_NEXT_PROTO
    അവസ്ഥ SSL3_MT_NEXT_PROTO:
      തിരികെ "Next protocol";
#endif
#ifdef SSL3_MT_MESSAGE_HASH
    അവസ്ഥ SSL3_MT_MESSAGE_HASH:
      തിരികെ "Message hash";
#endif
    }
  }
  തിരികെ "Unknown";
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *tls_rt_type(പൂർണ്ണസംഖ്യ type)
{
  തിരഞ്ഞെടുപ്പ്(type) {
#ifdef SSL3_RT_HEADER
  അവസ്ഥ SSL3_RT_HEADER:
    തിരികെ "TLS header";
#endif
  അവസ്ഥ SSL3_RT_CHANGE_CIPHER_SPEC:
    തിരികെ "TLS change cipher";
  അവസ്ഥ SSL3_RT_ALERT:
    തിരികെ "TLS alert";
  അവസ്ഥ SSL3_RT_HANDSHAKE:
    തിരികെ "TLS handshake";
  അവസ്ഥ SSL3_RT_APPLICATION_DATA:
    തിരികെ "TLS app data";
  സ്ഥിരസ്ഥിതി:
    തിരികെ "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
സ്ഥിതം ശൂന്യം ossl_trace(പൂർണ്ണസംഖ്യ direction, പൂർണ്ണസംഖ്യ ssl_ver, പൂർണ്ണസംഖ്യ content_type,
                       സ്ഥിരം ശൂന്യം *buf, size_t len, SSL *ssl,
                       ശൂന്യം *userp)
{
  സ്ഥിരം അക്ഷരം *verstr;
  ഘടന Curl_cfilter *cf = userp;
  ഘടന Curl_easy *data = ശൂന്യ;
  അക്ഷരം unknown[32];

  എങ്കിൽ(!cf)
    തിരികെ;
  data = CF_DATA_CURRENT(cf);
  എങ്കിൽ(!data || !data->set.fdebug || (direction && direction != 1))
    തിരികെ;

  തിരഞ്ഞെടുപ്പ്(ssl_ver) {
#ifdef SSL3_VERSION
  അവസ്ഥ SSL3_VERSION:
    verstr = "SSLv3";
    നിർത്തുക;
#endif
  അവസ്ഥ TLS1_VERSION:
    verstr = "TLSv1.0";
    നിർത്തുക;
#ifdef TLS1_1_VERSION
  അവസ്ഥ TLS1_1_VERSION:
    verstr = "TLSv1.1";
    നിർത്തുക;
#endif
#ifdef TLS1_2_VERSION
  അവസ്ഥ TLS1_2_VERSION:
    verstr = "TLSv1.2";
    നിർത്തുക;
#endif
  അവസ്ഥ TLS1_3_VERSION:
    verstr = "TLSv1.3";
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    curl_msnprintf(unknown, വലുപ്പം(unknown), "(%x)", (unsigned പൂർണ്ണസംഖ്യ)ssl_ver);
    verstr = unknown;
    നിർത്തുക;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  എങ്കിൽ(ssl_ver
#ifdef SSL3_RT_HEADER
     && content_type != SSL3_RT_HEADER
#endif
#ifdef SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#endif
    ) {
    സ്ഥിരം അക്ഷരം *സന്ദേശം_name = "Truncated message";
    സ്ഥിരം അക്ഷരം *tls_rt_name;
    അക്ഷരം ssl_buf[1024];
    പൂർണ്ണസംഖ്യ സന്ദേശം_type = 0;
    പൂർണ്ണസംഖ്യ txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    എങ്കിൽ(ssl_ver == SSL3_VERSION_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    അല്ലെങ്കിൽ
      tls_rt_name = "";

    എങ്കിൽ(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      എങ്കിൽ(len) {
        സന്ദേശം_type = *(സ്ഥിരം unsigned അക്ഷരം *)buf;
        സന്ദേശം_name = "Change cipher spec";
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(content_type == SSL3_RT_ALERT) {
      എങ്കിൽ(len >= 2) {
        സന്ദേശം_type =
          (((സ്ഥിരം unsigned അക്ഷരം *)buf)[0] << 8) +
           ((സ്ഥിരം unsigned അക്ഷരം *)buf)[1];
        സന്ദേശം_name = SSL_alert_desc_string_long(സന്ദേശം_type);
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(len) {
      സന്ദേശം_type = *(സ്ഥിരം unsigned അക്ഷരം *)buf;
      സന്ദേശം_name = ssl_സന്ദേശം_type(ssl_ver, സന്ദേശം_type);
    }

    txt_len = curl_msnprintf(ssl_buf, വലുപ്പം(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, സന്ദേശം_name, സന്ദേശം_type);
    Curl_debug(data, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(data, (direction == 1) ? CURLINFO_SSL_DATA_OUT :
             CURLINFO_SSL_DATA_IN, (സ്ഥിരം അക്ഷരം *)buf, len);
  (ശൂന്യം)ssl;
}

സ്ഥിതം CURLcode ossl_set_ssl_version_min_max(ഘടന Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             unsigned പൂർണ്ണസംഖ്യ ssl_version_min)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  long curl_ssl_version_min = (long)ssl_version_min;
  long curl_ssl_version_max;

  /* convert curl min SSL version option to OpenSSL constant */
#എങ്കിൽ defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_VERSION_സംഖ്യ)
  uint16_t ossl_ssl_version_min = 0;
  uint16_t ossl_ssl_version_max = 0;
#അല്ലെങ്കിൽ
  long ossl_ssl_version_min = 0;
  long ossl_ssl_version_max = 0;
#endif
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_version_min != CURL_SSLVERSION_DEFAULT);
  തിരഞ്ഞെടുപ്പ്(curl_ssl_version_min) {
  അവസ്ഥ CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  അവസ്ഥ CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    നിർത്തുക;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    നിർത്തുക;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    നിർത്തുക;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    നിർത്തുക;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (long)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  തിരഞ്ഞെടുപ്പ്(curl_ssl_version_max) {
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    നിർത്തുക;
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    നിർത്തുക;
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    നിർത്തുക;
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    നിർത്തുക;
  അവസ്ഥ CURL_SSLVERSION_MAX_NONE:  /* none selected */
  അവസ്ഥ CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  സ്ഥിരസ്ഥിതി:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    നിർത്തുക;
  }

  എങ്കിൽ(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    തിരികെ CURLE_SSL_CONNECT_ERROR;

  തിരികെ CURLE_OK;
}

CURLcode Curl_ossl_add_session(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന ossl_ctx *octx,
                               സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                               SSL_SESSION *session,
                               സ്ഥിരം അക്ഷരം *alpn,
                               unsigned അക്ഷരം *quic_tp,
                               size_t quic_tp_len,
                               ഘടന Curl_ssl_session **psession)
{
  ഘടന Curl_ssl_session *sc_session = ശൂന്യ, *sc_dup = ശൂന്യ;
  unsigned അക്ഷരം *der_session_buf = ശൂന്യ;
  unsigned അക്ഷരം *qtp_clone = ശൂന്യ;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(psession)
    *psession = ശൂന്യ;
  എങ്കിൽ(!cf || !data)
    പോകുക out;

  എങ്കിൽ(Curl_ssl_scache_use(cf, data)) {
    size_t der_session_size;
    unsigned അക്ഷരം *der_session_ptr;
    size_t earlydata_max = 0;
    പൂർണ്ണസംഖ്യ ietf_tls_id = SSL_version(octx->ssl);

    der_session_size = i2d_SSL_SESSION(session, ശൂന്യ);
    എങ്കിൽ(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_size);
    എങ്കിൽ(!der_session_buf) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }

    der_session_size = i2d_SSL_SESSION(session, &der_session_ptr);
    എങ്കിൽ(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }

#ifdef HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_data(session);
#endif
    എങ്കിൽ(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((സ്ഥിരം അക്ഷരം *)quic_tp, quic_tp_len);
      എങ്കിൽ(!qtp_clone) {
        result = CURLE_OUT_OF_MEMORY;
        പോകുക out;
      }
    }

    result = Curl_ssl_session_create2(der_session_buf, der_session_size,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(ശൂന്യ) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = ശൂന്യ;  /* took ownership of sdata */
#ifdef USE_APPLE_SECTRUST
    എങ്കിൽ(!result)
      sc_session->sectrust_verified = octx->sectrust_verified;
#endif
    എങ്കിൽ(!result && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        result = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    എങ്കിൽ(!result) {
      result = Curl_ssl_scache_put(cf, data, ssl_peer_താക്കോൽ, sc_session);
      /* took ownership of `sc_session` */
      sc_session = ശൂന്യ;
    }
  }

out:
  curlx_free(der_session_buf);
  എങ്കിൽ(!result && psession) {
    *psession = sc_dup;
    sc_dup = ശൂന്യ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  തിരികെ result;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ഘടന Curl_cfilter *cf = (ഘടന Curl_cfilter *)SSL_get_app_data(ssl);
  എങ്കിൽ(cf) {
    ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
    ഘടന ssl_connect_data *connssl = cf->ctx;
    ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
    ഘടന Curl_ssl_session *session = ശൂന്യ;
    Curl_ossl_add_session(cf, data, octx, connssl->peer.scache_താക്കോൽ,
                          ssl_sessionid, connssl->negotiated.alpn, ശൂന്യ,
                          0, &session);
    എങ്കിൽ(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  തിരികെ 0;
}

സ്ഥിതം CURLcode load_cacert_from_memory(X509_STORE *store,
                                        സ്ഥിരം ഘടന curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = ശൂന്യ;
  അട്ടി_OF(X509_INFO) *inf = ശൂന്യ;

  /* everything else is a reference */
  പൂർണ്ണസംഖ്യ i, എണ്ണം = 0;
  X509_INFO *itmp = ശൂന്യ;

  എങ്കിൽ(ca_info_blob->len > (size_t)INT_MAX)
    തിരികെ CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->data, (പൂർണ്ണസംഖ്യ)ca_info_blob->len);
  എങ്കിൽ(!cbio)
    തിരികെ CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, ശൂന്യ, ശൂന്യ, ശൂന്യ);
  എങ്കിൽ(!inf) {
    BIO_free(cbio);
    തിരികെ CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  വേണ്ടി(i = 0; i < (പൂർണ്ണസംഖ്യ)sk_X509_INFO_അക്കം(inf); ++i) {
    itmp = sk_X509_INFO_മൂല്യം(inf, (ossl_valsize_t)i);
    എങ്കിൽ(itmp->x509) {
      എങ്കിൽ(X509_STORE_add_cert(store, itmp->x509)) {
        ++എണ്ണം;
      }
      അല്ലെങ്കിൽ {
        /* set count to 0 to return an error */
        എണ്ണം = 0;
        നിർത്തുക;
      }
    }
    എങ്കിൽ(itmp->crl) {
      എങ്കിൽ(X509_STORE_add_crl(store, itmp->crl)) {
        ++എണ്ണം;
      }
      അല്ലെങ്കിൽ {
        /* set count to 0 to return an error */
        എണ്ണം = 0;
        നിർത്തുക;
      }
    }
  }

#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic തള്ളുക
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_INFO_എടുക്കുക_free(inf, X509_INFO_free);
#എങ്കിൽ defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic എടുക്കുക
#endif
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  തിരികെ (എണ്ണം > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ifdef USE_WIN32_CRYPTO
സ്ഥിതം CURLcode ossl_win_load_store(ഘടന Curl_easy *data,
                                    ഘടന Curl_cfilter *cf,
                                    സ്ഥിരം അക്ഷരം *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode result = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  എങ്കിൽ(hStore) {
    PCCERT_CONTEXT pContext = ശൂന്യ;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = ശൂന്യ;
    DWORD enhkey_usage_size = 0;
    VERBOSE(size_t total = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    result = CURLE_OK;
    വേണ്ടി(;;) {
      X509 *x509;
      FILETIME now;
      BYTE താക്കോൽ_usage[2];
      DWORD req_size;
      സ്ഥിരം unsigned അക്ഷരം *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      എങ്കിൽ(!pContext)
        നിർത്തുക;

      VERBOSE(++total);

#എങ്കിൽ defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        അക്ഷരം cert_name[256];
        എങ്കിൽ(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               ശൂന്യ, cert_name, വലുപ്പം(cert_name)))
          infof(data, "SSL: unknown cert name");
        അല്ലെങ്കിൽ
          infof(data, "SSL: Checking cert \"%s\"", cert_name);
      }
#endif
      encoded_cert = (സ്ഥിരം unsigned അക്ഷരം *)pContext->pbCertEncoded;
      എങ്കിൽ(!encoded_cert)
        തുടരുക;

      GetSystemTimeAsFileTime(&now);
      എങ്കിൽ(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        തുടരുക;

      /* If key usage exists check for signing attribute */
      എങ്കിൽ(Cert_Get_Intended_താക്കോൽ_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 താക്കോൽ_usage, വലുപ്പം(താക്കോൽ_usage))) {
        എങ്കിൽ(!(താക്കോൽ_usage[0] & CERT_താക്കോൽ_CERT_SIGN_താക്കോൽ_USAGE))
          തുടരുക;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(GetLastError())
        തുടരുക;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      എങ്കിൽ(Cert_Get_Enhanced_താക്കോൽ_Usage(pContext, 0, ശൂന്യ, &req_size) && req_size) {
        എങ്കിൽ(req_size > enhkey_usage_size) {
          ശൂന്യം *tmp = curlx_realloc(enhkey_usage, req_size);

          എങ്കിൽ(!tmp) {
            failf(data, "SSL: Out of memory allocating for OID list");
            result = CURLE_OUT_OF_MEMORY;
            നിർത്തുക;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_size = req_size;
        }

        എങ്കിൽ(Cert_Get_Enhanced_താക്കോൽ_Usage(pContext, 0, enhkey_usage, &req_size)) {
          എങ്കിൽ(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            എങ്കിൽ((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              തുടരുക;
          }
          അല്ലെങ്കിൽ {
            DWORD i;
            bool found = FALSE;

            വേണ്ടി(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              എങ്കിൽ(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                നിർത്തുക;
              }
            }

            എങ്കിൽ(!found)
              തുടരുക;
          }
        }
        അല്ലെങ്കിൽ
          തുടരുക;
      }
      അല്ലെങ്കിൽ
        തുടരുക;

      x509 = d2i_X509(ശൂന്യ, &encoded_cert, (long)pContext->cbCertEncoded);
      എങ്കിൽ(!x509)
        തുടരുക;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      എങ്കിൽ(X509_STORE_add_cert(store, x509) == 1) {
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

    എങ്കിൽ(result)
      തിരികെ result;
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode ossl_windows_load_anchors(ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode result = CURLE_OK;

  *padded = FALSE;
  വേണ്ടി(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    result = ossl_win_load_store(data, cf, win_stores[i], store, &store_added);
    എങ്കിൽ(result)
      തിരികെ result;
    എങ്കിൽ(store_added) {
      CURL_TRC_CF(data, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    അല്ലെങ്കിൽ
      infof(data, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  തിരികെ result;
}

#endif /* USE_WIN32_CRYPTO */

സ്ഥിതം CURLcode ossl_load_trust_anchors(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ഘടന ossl_ctx *octx,
                                        X509_STORE *store)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? ശൂന്യ : conn_config->CAfile);
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  എങ്കിൽ(conn_config->native_ca_store) {
#ifdef USE_WIN32_CRYPTO
    bool added = FALSE;
    result = ossl_windows_load_anchors(cf, data, store, &added);
    എങ്കിൽ(result)
      തിരികെ result;
    എങ്കിൽ(added) {
      infof(data, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#അല്ലെങ്കിലെങ്കിൽ defined(USE_APPLE_SECTRUST)
    infof(data, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#endif
  }

  എങ്കിൽ(conn_config->ca_info_blob) {
    result = load_cacert_from_memory(store, conn_config->ca_info_blob);
    എങ്കിൽ(result) {
      failf(data, "error adding trust anchors from certificate blob: %d",
            (പൂർണ്ണസംഖ്യ)result);
      തിരികെ result;
    }
    infof(data, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  എങ്കിൽ(ssl_cafile || ssl_capath) {
#ifdef HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    എങ്കിൽ(ssl_cafile) {
      എങ്കിൽ(!X509_STORE_load_file(store, ssl_cafile)) {
        എങ്കിൽ(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from file: %s", ssl_cafile);
          തിരികെ CURLE_SSL_CACERT_BADFILE;
        }
        അല്ലെങ്കിൽ
          infof(data, "error setting certificate file, continuing anyway");
      }
      infof(data, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    എങ്കിൽ(ssl_capath) {
      എങ്കിൽ(!X509_STORE_load_path(store, ssl_capath)) {
        എങ്കിൽ(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from path: %s", ssl_capath);
          തിരികെ CURLE_SSL_CACERT_BADFILE;
        }
        അല്ലെങ്കിൽ
          infof(data, "error setting certificate path, continuing anyway");
      }
      infof(data, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#അല്ലെങ്കിൽ
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    എങ്കിൽ(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      എങ്കിൽ(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(data, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        തിരികെ CURLE_SSL_CACERT_BADFILE;
      }
      അല്ലെങ്കിൽ {
        infof(data, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    എങ്കിൽ(ssl_cafile)
      infof(data, "  CAfile: %s", ssl_cafile);
    എങ്കിൽ(ssl_capath)
      infof(data, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#endif
  }

#ifdef CURL_CA_FALLBACK
  എങ്കിൽ(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(data, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#endif
  എങ്കിൽ(octx->store_is_empty && !have_native_check)
    infof(data, "  no trust anchors configured");

  തിരികെ result;
}

സ്ഥിതം CURLcode ossl_populate_x509_store(ഘടന Curl_cfilter *cf,
                                         ഘടന Curl_easy *data,
                                         ഘടന ossl_ctx *octx,
                                         X509_STORE *store)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  X509_LOOKUP *lookup = ശൂന്യ;
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_crlfile = conn_config->CRLfile;
  unsigned long x509flags = 0;

  CURL_TRC_CF(data, cf, "configuring OpenSSL's x509 trust store");
  എങ്കിൽ(!store)
    തിരികെ CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(!conn_config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    തിരികെ CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  result = ossl_load_trust_anchors(cf, data, octx, store);
  എങ്കിൽ(result)
    തിരികെ result;

  /* Does not make sense to load a CRL file without peer verification */
#ifdef USE_APPLE_SECTRUST
  എങ്കിൽ(ssl_crlfile && conn_config->native_ca_store) {
    failf(data, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    തിരികെ CURLE_NOT_BUILT_IN;
  }
#endif
  എങ്കിൽ(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    എങ്കിൽ(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(data, "error loading CRL file: %s", ssl_crlfile);
      തിരികെ CURLE_SSL_CRL_BADFILE;
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

  എങ്കിൽ(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (ശൂന്യം)X509_STORE_set_flags(store, x509flags);

  തിരികെ result;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_OSSL_X509_താക്കോൽ  "tls:ossl:x509:share"

ഘടന ossl_x509_share {
  അക്ഷരം *CAfile;         /* CAfile path used to generate X509 store */
  അക്ഷരം *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  ഘടന curltime time; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

സ്ഥിതം ശൂന്യം oss_x509_share_free(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t താക്കോൽ_len, ശൂന്യം *p)
{
  ഘടന ossl_x509_share *share = p;
  DEBUGASSERT(താക്കോൽ_len == CURL_CSTRLEN(MPROTO_OSSL_X509_താക്കോൽ));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_താക്കോൽ, താക്കോൽ, താക്കോൽ_len));
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)താക്കോൽ_len;
  എങ്കിൽ(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

സ്ഥിതം bool ossl_cached_x509_store_expired(ഘടന Curl_easy *data,
                                           സ്ഥിരം ഘടന ossl_x509_share *mb)
{
  എങ്കിൽ(data->set.ssl_ca_cache_timeout < 0)
    തിരികെ FALSE;
  അല്ലെങ്കിൽ {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &mb->time);
    timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

    തിരികെ elapsed_ms >= timeout_ms;
  }
}

സ്ഥിതം bool ossl_cached_x509_store_different(ഘടന Curl_cfilter *cf,
                                             സ്ഥിരം ഘടന Curl_easy *data,
                                             സ്ഥിരം ഘടന ossl_x509_share *mb)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (ശൂന്യം)data;
  എങ്കിൽ(mb->no_partialchain != conn_config->no_partialchain)
    തിരികെ TRUE;
  എങ്കിൽ(!mb->CAfile || !conn_config->CAfile) {
    എങ്കിൽ(mb->CAfile != conn_config->CAfile)
      തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(strcmp(mb->CAfile, conn_config->CAfile))
    തിരികെ TRUE;

  എങ്കിൽ(!mb->CApath || !conn_config->CApath) {
    എങ്കിൽ(mb->CApath != conn_config->CApath)
      തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(strcmp(mb->CApath, conn_config->CApath))
    തിരികെ TRUE;

  തിരികെ FALSE;
}

സ്ഥിതം X509_STORE *ossl_get_cached_x509_store(ഘടന Curl_cfilter *cf,
                                              ഘടന Curl_easy *data,
                                              bool *pempty)
{
  ഘടന Curl_multi *multi = data->multi;
  ഘടന ossl_x509_share *share;
  X509_STORE *store = ശൂന്യ;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_താക്കോൽ,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_താക്കോൽ)) : ശൂന്യ;
  എങ്കിൽ(share && share->store &&
     !ossl_cached_x509_store_expired(data, share) &&
     !ossl_cached_x509_store_different(cf, data, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  തിരികെ store;
}

സ്ഥിതം ശൂന്യം ossl_set_cached_x509_store(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       X509_STORE *store,
                                       bool is_empty)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന Curl_multi *multi = data->multi;
  ഘടന ossl_x509_share *share;

  DEBUGASSERT(multi);
  എങ്കിൽ(!multi)
    തിരികെ;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_താക്കോൽ,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_താക്കോൽ));

  എങ്കിൽ(!share) {
    share = curlx_calloc(1, വലുപ്പം(*share));
    എങ്കിൽ(!share)
      തിരികെ;
    എങ്കിൽ(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_താക്കോൽ,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_താക്കോൽ),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      തിരികെ;
    }
  }

  എങ്കിൽ(X509_STORE_up_ref(store)) {
    അക്ഷരം *CAfile = ശൂന്യ;
    അക്ഷരം *CApath = ശൂന്യ;

    എങ്കിൽ(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      എങ്കിൽ(!CAfile) {
        X509_STORE_free(store);
        തിരികെ;
      }
    }

    എങ്കിൽ(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      എങ്കിൽ(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        തിരികെ;
      }
    }

    എങ്കിൽ(share->store) {
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
#അല്ലെങ്കിൽ
#define USING_PROVIDER(x) 0
#endif

CURLcode Curl_ssl_setup_x509_store(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന ossl_ctx *octx)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
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
  എങ്കിൽ(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  അല്ലെങ്കിൽ {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    result = ossl_populate_x509_store(cf, data, octx, store);
    എങ്കിൽ(result == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, data, store, (bool)octx->store_is_empty);
    }
  }

  ERR_എടുക്കുക_to_mark();

  തിരികെ result;
}

സ്ഥിതം bool ossl_apply_session(
  ഘടന ossl_ctx *octx,
  ഘടന Curl_cfilter *cf,
  ഘടന Curl_easy *data,
  ഘടന alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  ഘടന Curl_ssl_session *scs)
{
  ഘടന ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  സ്ഥിരം unsigned അക്ഷരം *der_sessionid = scs->sdata;
  size_t der_sessionid_size = scs->sdata_len;
  SSL_SESSION *ssl_session = ശൂന്യ;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(ശൂന്യ, &der_sessionid,
                                (long)der_sessionid_size);
  എങ്കിൽ(ssl_session) {
    എങ്കിൽ(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(അക്ഷരം error_buffer[256]);
      infof(data, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          വലുപ്പം(error_buffer)));
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(conn_cfg->verifypeer &&
         (SSL_get_verify_result(octx->ssl) != X509_V_OK)
#ifdef USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#endif
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, ശൂന്യ);
        infof(data, "SSL session not peer verified, not reusing");
      }
      അല്ലെങ്കിൽ {
        infof(data, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ifdef USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#endif
        infof(data, "SSL verify result: %lx",
              (unsigned long)SSL_get_verify_result(octx->ssl));
#ifdef HAVE_OPENSSL_EARLYDATA
        എങ്കിൽ(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_data(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_data = FALSE;
          എങ്കിൽ(sess_reuse_cb)
            (ശൂന്യം)sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          എങ്കിൽ(do_early_data) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_to(alpns, scs->alpn);
          }
        }
#അല്ലെങ്കിൽ
        (ശൂന്യം)alpns;
        (ശൂന്യം)conn_cfg;
        (ശൂന്യം)sess_reuse_cb;
#endif
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  അല്ലെങ്കിൽ {
    infof(data, "SSL session not accepted by OpenSSL, continuing without");
  }
  തിരികെ (bool)octx->reused_session;
}

സ്ഥിതം CURLcode ossl_init_session_and_alpns(
  ഘടന ossl_ctx *octx,
  ഘടന Curl_cfilter *cf,
  ഘടന Curl_easy *data,
  ഘടന ssl_peer *peer,
  സ്ഥിരം ഘടന alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  ഘടന ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ഘടന alpn_spec alpns;
  CURLcode result;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  എങ്കിൽ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ഘടന Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    എങ്കിൽ(scs) {
      എങ്കിൽ(ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
    }
  }

  എങ്കിൽ(!octx->reused_session &&
     Curl_ssl_scache_use(cf, data) && !conn_cfg->verifystatus) {
    ഘടന Curl_ssl_session *scs = ശൂന്യ;

    result = Curl_ssl_scache_take(cf, data, peer->scache_താക്കോൽ, &scs);
    എങ്കിൽ(!result && scs && scs->sdata && scs->sdata_len) {
      (ശൂന്യം)ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, data, peer->scache_താക്കോൽ, scs);
  }

  എങ്കിൽ(alpns.എണ്ണം) {
    ഘടന alpn_proto_buf proto;
    memset(&proto, 0, വലുപ്പം(proto));
    result = Curl_alpn_to_proto_buf(&proto, &alpns);
    എങ്കിൽ(result) {
      failf(data, "Error determining ALPN");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
    എങ്കിൽ(SSL_set_alpn_protos(octx->ssl, proto.data, proto.len)) {
      failf(data, "Error setting ALPN");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
  }

  തിരികെ CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
സ്ഥിതം CURLcode ossl_init_ech(ഘടന ossl_ctx *octx,
                              ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              ഘടന ssl_peer *peer)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  പൂർണ്ണസംഖ്യ trying_ech_now = 0;

  എങ്കിൽ(!Curl_ssl_ech_enabled(cf))
    തിരികെ CURLE_OK;

  എങ്കിൽ(conn_config->ech == CURLECH_GREASE) {
    infof(data, "ECH: will GREASE ClientHello");
#ifdef HAVE_BORINGSSL_LIKE
    SSL_set_enable_ech_grease(octx->ssl, 1);
#അല്ലെങ്കിൽ
    SSL_set_options(octx->ssl, SSL_OP_ECH_GREASE);
#endif
  }
  അല്ലെങ്കിൽ എങ്കിൽ(conn_config->ech && conn_config->ech_config) {
#ifdef HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode result;

    എങ്കിൽ(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * strlen(conn_config->ech_config);
    result = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    എങ്കിൽ(result || !ech_config) {
      infof(data, "ECH: cannot base64 decode ECHConfig from command line");
      എങ്കിൽ(conn_config->ech == CURLECH_HARD)
        തിരികെ result;
    }
    എങ്കിൽ(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      എങ്കിൽ(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        തിരികെ CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#അല്ലെങ്കിൽ
    size_t ech_config_len = 0;
    എങ്കിൽ(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = strlen(conn_config->ech_config);
    എങ്കിൽ(SSL_set1_ech_config_list(octx->ssl,
                                (സ്ഥിരം uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      എങ്കിൽ(conn_config->ech == CURLECH_HARD)
        തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
    അല്ലെങ്കിൽ
      trying_ech_now = 1;
#endif /* HAVE_BORINGSSL_LIKE */
    infof(data, "ECH: ECHConfig from command line");
  }
  അല്ലെങ്കിൽ {
    സ്ഥിരം ഘടന Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, peer->origin);

    എങ്കിൽ(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      സ്ഥിരം unsigned അക്ഷരം *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(data, "ECH: ECHConfig from HTTPS RR");
      എങ്കിൽ(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(data, "ECH: SSL_set1_ech_config_list failed");
        എങ്കിൽ(conn_config->ech == CURLECH_HARD)
          തിരികെ CURLE_SSL_CONNECT_ERROR;
      }
      അല്ലെങ്കിൽ {
        trying_ech_now = 1;
        infof(data, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    അല്ലെങ്കിൽ {
      infof(data, "ECH: requested but no ECHConfig available");
      എങ്കിൽ(conn_config->ech == CURLECH_HARD)
        തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
  }
#ifdef HAVE_BORINGSSL_LIKE
  (ശൂന്യം)peer;
  എങ്കിൽ(trying_ech_now && conn_config->ech_public) {
    infof(data, "ECH: setting public_name not supported with BoringSSL");
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }
#അല്ലെങ്കിൽ
  എങ്കിൽ(trying_ech_now && conn_config->ech_public) {
    പൂർണ്ണസംഖ്യ ret;
    infof(data, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    എങ്കിൽ(ret != 1) {
      infof(data, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
  }
#endif /* HAVE_BORINGSSL_LIKE */
  എങ്കിൽ(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(data, "ECH: cannot force TLSv1.3 [ERROR]");
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  തിരികെ CURLE_OK;
}
#endif /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

സ്ഥിതം CURLcode ossl_init_ssl(ഘടന ossl_ctx *octx,
                              ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              ഘടന ssl_peer *peer,
                              സ്ഥിരം ഘടന alpn_spec *alpns_requested,
                              ശൂന്യം *ssl_user_data,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  എങ്കിൽ(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  എങ്കിൽ(!octx->ssl) {
    failf(data, "SSL: could not create a context (handle)");
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_data(octx->ssl, ssl_user_data);

#ifndef OPENSSL_NO_OCSP
  എങ്കിൽ(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#endif

  SSL_set_connect_state(octx->ssl);

  എങ്കിൽ(peer->sni) {
    എങ്കിൽ(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(data, "Failed set SNI");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
  }

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode result = ossl_init_ech(octx, cf, data, peer);
    എങ്കിൽ(result)
      തിരികെ result;
  }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  തിരികെ ossl_init_session_and_alpns(octx, cf, data, peer,
                                     alpns_requested, sess_reuse_cb);
}

സ്ഥിതം CURLcode ossl_init_method(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന ssl_peer *peer,
                                 സ്ഥിരം SSL_METHOD **pmethod,
                                 unsigned പൂർണ്ണസംഖ്യ *pssl_version_min)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = ശൂന്യ;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  തിരഞ്ഞെടുപ്പ്(peer->transport) {
  അവസ്ഥ TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    തിരഞ്ഞെടുപ്പ്(*pssl_version_min) {
    അവസ്ഥ CURL_SSLVERSION_TLSv1:
    അവസ്ഥ CURL_SSLVERSION_TLSv1_0:
    അവസ്ഥ CURL_SSLVERSION_TLSv1_1:
    അവസ്ഥ CURL_SSLVERSION_TLSv1_2:
    അവസ്ഥ CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      നിർത്തുക;
    അവസ്ഥ CURL_SSLVERSION_SSLv2:
      failf(data, "No SSLv2 support");
      തിരികെ CURLE_NOT_BUILT_IN;
    അവസ്ഥ CURL_SSLVERSION_SSLv3:
      failf(data, "No SSLv3 support");
      തിരികെ CURLE_NOT_BUILT_IN;
    സ്ഥിരസ്ഥിതി:
      failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
    നിർത്തുക;
  അവസ്ഥ TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    എങ്കിൽ(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(data, "QUIC needs at least TLS version 1.3");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    failf(data, "unsupported transport %d in SSL init", peer->transport);
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  തിരികെ *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(ഘടന ossl_ctx *octx,
                            ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            ഘടന ssl_peer *peer,
                            സ്ഥിരം ഘടന alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            ശൂന്യം *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            ശൂന്യം *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *ciphers;
  സ്ഥിരം SSL_METHOD *req_method = ശൂന്യ;
  ctx_option_t ctx_options = 0;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  അക്ഷരം * സ്ഥിരം ssl_cert = conn_config->clientcert;
  സ്ഥിരം ഘടന curl_blob *ssl_cert_blob = conn_config->cert_blob;
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_cert_type = conn_config->cert_type;
  unsigned പൂർണ്ണസംഖ്യ ssl_version_min = CURL_SSLVERSION_DEFAULT;
  അക്ഷരം error_buffer[256];

  /* Make funny stuff to get random input */
  result = ossl_seed(data);
  എങ്കിൽ(!result) {
    എങ്കിൽ(CURL_EASY_STR(data, STRING_SSL_ENGINE))
      result = ossl_set_engine(data, octx,
                               CURL_EASY_STR(data, STRING_SSL_ENGINE));
    എങ്കിൽ(!result && data->set.engine_default)
      result = ossl_set_engine_default(data, octx);
  }

  എങ്കിൽ(!result) {
    ssl_config->certverifyresult = !X509_V_OK;
    result = ossl_init_method(cf, data, peer, &req_method, &ssl_version_min);
  }
  എങ്കിൽ(result)
    തിരികെ result;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ifdef OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#endif
    SSL_CTX_new(req_method);

  എങ്കിൽ(!octx->ssl_ctx) {
    failf(data, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_buffer, വലുപ്പം(error_buffer)));
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  എങ്കിൽ(data->set.fdebug && data->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_സന്ദേശം_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_സന്ദേശം_callback_arg(octx->ssl_ctx, cf);
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
  എങ്കിൽ(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_തിരുകുക_EMPTY_FRAGMENTS;

  DEBUGASSERT(ssl_version_min != CURL_SSLVERSION_DEFAULT);
  തിരഞ്ഞെടുപ്പ്(ssl_version_min) {
  അവസ്ഥ CURL_SSLVERSION_SSLv2:
  അവസ്ഥ CURL_SSLVERSION_SSLv3:
    തിരികെ CURLE_NOT_BUILT_IN;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  അവസ്ഥ CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  അവസ്ഥ CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  അവസ്ഥ CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  അവസ്ഥ CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  അവസ്ഥ CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    result = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    എങ്കിൽ(result)
      തിരികെ result;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    തിരികെ CURLE_SSL_CONNECT_ERROR;
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
  എങ്കിൽ(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = ശൂന്യ;
  എങ്കിൽ(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    എങ്കിൽ(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(data, "failed setting cipher list: %s", ciphers);
      തിരികെ CURLE_SSL_CIPHER;
    }
    infof(data, "Cipher selection: %s", ciphers);
  }

#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    സ്ഥിരം അക്ഷരം *ciphers13 = conn_config->cipher_list13;
    എങ്കിൽ(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      എങ്കിൽ(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(data, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        തിരികെ CURLE_SSL_CIPHER;
      }
      infof(data, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#endif

  എങ്കിൽ(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    result = client_cert(data, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->താക്കോൽ, conn_config->താക്കോൽ_blob,
                         conn_config->താക്കോൽ_type,
                         conn_config->താക്കോൽ_passwd);
    എങ്കിൽ(result)
      /* failf() is already done in client_cert() */
      തിരികെ result;
  }

#ifdef HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#endif

  {
    സ്ഥിരം അക്ഷരം *curves = conn_config->curves;
    എങ്കിൽ(curves) {
#ifdef HAVE_BORINGSSL_LIKE
#define OSSL_CURVE_CAST(x) (x)
#അല്ലെങ്കിൽ
#define OSSL_CURVE_CAST(x) (അക്ഷരം *)CURL_UNCONST(x)
#endif
      എങ്കിൽ(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(data, "failed setting curves list: '%s'", curves);
        തിരികെ CURLE_SSL_CIPHER;
      }
    }
  }

#ifdef HAVE_SSL_CTX_SET1_SIGALGS
#define OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    സ്ഥിരം അക്ഷരം *signature_algorithms = conn_config->signature_algorithms;
    എങ്കിൽ(signature_algorithms) {
      എങ്കിൽ(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(data, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        തിരികെ CURLE_SSL_CIPHER;
      }
    }
  }
#endif

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, ശൂന്യ);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#എങ്കിൽ !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  എങ്കിൽ(Curl_tls_keylog_enabled()) {
    SSL_CTX_set_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#endif

  എങ്കിൽ(cb_new_session) {
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
  എങ്കിൽ(data->set.ssl_fsslctx) {
    ഘടന Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    എങ്കിൽ(!octx->x509_store_setup) {
      result = Curl_ssl_setup_x509_store(cf, data, octx);
      എങ്കിൽ(result)
        തിരികെ result;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, data, easy_fsslctx);
    result = (*data->set.ssl_fsslctx)(data, octx->ssl_ctx,
                                      data->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    എങ്കിൽ(result) {
      failf(data, "error signaled by SSL ctx callback");
      തിരികെ result;
    }
  }

  തിരികെ ossl_init_ssl(octx, cf, data, peer, alpns_requested,
                       ssl_user_data, sess_reuse_cb);
}

സ്ഥിതം CURLcode ossl_on_session_reuse(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      ഘടന alpn_spec *alpns,
                                      ഘടന Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  തിരികെ Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max);
}

ശൂന്യം Curl_ossl_report_handshake(ഘടന Curl_easy *data, ഘടന ossl_ctx *octx)
{
#ifdef CURLVERBOSE
  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    പൂർണ്ണസംഖ്യ psigtype_nid = NID_undef;
    സ്ഥിരം അക്ഷരം *negotiated_group_name = ശൂന്യ;

#ifdef HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#എങ്കിൽ OPENSSL_VERSION_സംഖ്യ >= 0x30200000L
    negotiated_group_name = SSL_get0_group_name(octx->ssl);
#അല്ലെങ്കിൽ
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
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)octx;
#endif /* CURLVERBOSE */
}

സ്ഥിതം CURLcode ossl_connect_step1(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  result = Curl_ossl_ctx_init(octx, cf, data, &connssl->peer,
                              connssl->alpn, ശൂന്യ, ശൂന്യ,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  എങ്കിൽ(result)
    തിരികെ result;

  octx->bio_method = ossl_bio_cf_method_create();
  എങ്കിൽ(!octx->bio_method)
    തിരികെ CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  എങ്കിൽ(!bio)
    തിരികെ CURLE_OUT_OF_MEMORY;

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
#അല്ലെങ്കിൽ
  SSL_set_bio(octx->ssl, bio, bio);
#endif

  എങ്കിൽ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ഘടന alpn_proto_buf proto;
    memset(&proto, 0, വലുപ്പം(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }

  connssl->connecting_state = ssl_connect_2;
  തിരികെ CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
സ്ഥിതം പൂർണ്ണസംഖ്യ ossl_trace_ech_retry_configs(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data, SSL *ssl,
                                        പൂർണ്ണസംഖ്യ reason)
{
  CURLcode result = CURLE_OK;
  size_t rcl = 0;
  പൂർണ്ണസംഖ്യ rv = 1;
#ifndef HAVE_BORINGSSL_LIKE
  അക്ഷരം *inner = ശൂന്യ;
  uint8_t *rcs = ശൂന്യ;
  അക്ഷരം *outer = ശൂന്യ;
#അല്ലെങ്കിൽ
  സ്ഥിരം അക്ഷരം *inner = ശൂന്യ;
  സ്ഥിരം uint8_t *rcs = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *outer = ശൂന്യ;
  size_t out_name_len = 0;
  പൂർണ്ണസംഖ്യ servername_type = 0;
#endif
  NOVERBOSE((ശൂന്യം)reason);

  /* nothing to trace if not doing ECH */
  എങ്കിൽ(!Curl_ssl_ech_enabled(cf))
    തിരികെ rv;
#ifndef HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#അല്ലെങ്കിൽ
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (പൂർണ്ണസംഖ്യ)rcl;
#endif

  എങ്കിൽ(rv && rcs) {
    അക്ഷരം *b64str = ശൂന്യ;
    size_t blen = 0;

    result = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    എങ്കിൽ(!result && b64str) {
      infof(data, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ifndef HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#അല്ലെങ്കിൽ
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#endif
    }
  }
  അല്ലെങ്കിൽ
    infof(data, "ECH: no retry_configs (rv = %d)", rv);
#ifndef HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#endif
  തിരികെ rv;
}

#endif

സ്ഥിതം CURLcode ossl_connect_step2(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  ഘടന ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  പൂർണ്ണസംഖ്യ err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  എങ്കിൽ(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, octx);
    എങ്കിൽ(result)
      തിരികെ result;
    octx->x509_store_setup = TRUE;
  }

#എങ്കിൽ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  എങ്കിൽ(Curl_tls_keylog_enabled() && !octx->keylog_done)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_done);
#endif

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  എങ്കിൽ(err != 1) {
    പൂർണ്ണസംഖ്യ detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(data, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    എങ്കിൽ(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      തിരികെ CURLE_AGAIN;
    }
    എങ്കിൽ(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      തിരികെ CURLE_AGAIN;
    }
#ifdef SSL_ERROR_WANT_ASYNC
    എങ്കിൽ(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      തിരികെ CURLE_AGAIN;
    }
#endif
#ifdef SSL_ERROR_WANT_RETRY_VERIFY
    എങ്കിൽ(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(data, TRUE);
      തിരികെ CURLE_AGAIN;
    }
#endif
    അല്ലെങ്കിൽ {
      /* untreated error */
      sslerr_t errdetail;
      അക്ഷരം error_buffer[256] = "";
      CURLcode result;
      long lerr;
      പൂർണ്ണസംഖ്യ lib;
      പൂർണ്ണസംഖ്യ reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      എങ്കിൽ((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#ifdef SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#endif
         )) {
        result = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_result(octx->ssl);
        എങ്കിൽ(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        അല്ലെങ്കിൽ
          failf(data, "%s", "SSL certificate verification failed");
      }
#ifdef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      അല്ലെങ്കിൽ എങ്കിൽ((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        result = CURLE_SSL_CLIENTCERT;
        failf(data, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_buffer, വലുപ്പം(error_buffer)));
      }
#endif
#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
      അല്ലെങ്കിൽ എങ്കിൽ((lib == ERR_LIB_SSL) &&
#ifndef HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#അല്ലെങ്കിൽ
              (reason == SSL_R_ECH_REJECTED)) {
#endif /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, reason);

        result = CURLE_ECH_REQUIRED;
        failf(data, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_buffer, വലുപ്പം(error_buffer)));
      }
#endif
      അല്ലെങ്കിൽ {
        result = CURLE_SSL_CONNECT_ERROR;
        failf(data, "TLS connect error: %s",
              ossl_strerror(errdetail, error_buffer, വലുപ്പം(error_buffer)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      എങ്കിൽ(result == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        അക്ഷരം extramsg[80] = "";
        പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;

        എങ്കിൽ(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, വലുപ്പം(extramsg));
        failf(data, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      തിരികെ result;
    }
  }
  അല്ലെങ്കിൽ {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(data, octx);

#എങ്കിൽ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    എങ്കിൽ(Curl_ssl_ech_enabled(cf)) {
      ഘടന ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      അക്ഷരം *inner = ശൂന്യ, *outer = ശൂന്യ;
      പൂർണ്ണസംഖ്യ rv;
      VERBOSE(സ്ഥിരം അക്ഷരം *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      തിരഞ്ഞെടുപ്പ്(rv) {
      അവസ്ഥ SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        നിർത്തുക;
      അവസ്ഥ SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        നിർത്തുക;
      അവസ്ഥ SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        നിർത്തുക;
      അവസ്ഥ SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        നിർത്തുക;
      അവസ്ഥ SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        നിർത്തുക;
      അവസ്ഥ SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        നിർത്തുക;
      അവസ്ഥ SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        നിർത്തുക;
      അവസ്ഥ SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        നിർത്തുക;
      അവസ്ഥ SSL_ECH_STATUS_BAD_NAME: {
        എങ്കിൽ(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        അല്ലെങ്കിൽ {
          VERBOSE(status = "bad name (unexpected)");
        }
        നിർത്തുക;
      }
      സ്ഥിരസ്ഥിതി:
        VERBOSE(status = "unexpected status");
        infof(data, "ECH: unexpected status %d", rv);
      }
      infof(data, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      എങ്കിൽ(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, 0);
      }
      എങ്കിൽ(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(data, "ECH: ech-hard failed");
        തിരികെ CURLE_SSL_CONNECT_ERROR;
      }
    }
    അല്ലെങ്കിൽ {
      infof(data, "ECH: result: status is not attempted");
    }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    എങ്കിൽ(connssl->alpn) {
      സ്ഥിരം unsigned അക്ഷരം *neg_protocol;
      unsigned പൂർണ്ണസംഖ്യ len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      തിരികെ Curl_alpn_set_negotiated(cf, data, connssl, neg_protocol, len);
    }

    തിരികെ CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
സ്ഥിതം CURLcode ossl_pkp_pin_peer_pubkey(ഘടന Curl_easy *data, X509 *cert,
                                         സ്ഥിരം അക്ഷരം *pinnedpubkey)
{
  /* Scratch */
  പൂർണ്ണസംഖ്യ len1 = 0, len2 = 0;
  unsigned അക്ഷരം *buff1 = ശൂന്യ, *temp = ശൂന്യ;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  എങ്കിൽ(!pinnedpubkey)
    തിരികെ CURLE_OK;

  എങ്കിൽ(!cert)
    തിരികെ result;

  ചെയ്യുക {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), ശൂന്യ);
    എങ്കിൽ(len1 < 1)
      നിർത്തുക; /* failed */

    buff1 = temp = curlx_malloc(len1);
    എങ്കിൽ(!buff1)
      നിർത്തുക; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &temp);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    എങ്കിൽ((len1 != len2) || !temp || ((temp - buff1) != len1))
      നിർത്തുക; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } ആകുമ്പോൾ(0);

  എങ്കിൽ(buff1)
    curlx_free(buff1);

  തിരികെ result;
}

#ifdef CURLVERBOSE
#എങ്കിൽ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_സംഖ്യ) && LIBRESSL_VERSION_സംഖ്യ < 0x3060000fL)
സ്ഥിതം ശൂന്യം infof_certstack(ഘടന Curl_easy *data, സ്ഥിരം SSL *ssl)
{
  അട്ടി_OF(X509) *certstack;
  long verify_result;
  പൂർണ്ണസംഖ്യ അക്കം_cert_levels;
  പൂർണ്ണസംഖ്യ cert_level;

  എങ്കിൽ(!Curl_trc_is_verbose(data))
    തിരികെ;

  verify_result = SSL_get_verify_result(ssl);
  എങ്കിൽ(verify_result != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  അല്ലെങ്കിൽ
    certstack = SSL_get0_verified_chain(ssl);
  എങ്കിൽ(!certstack)
    തിരികെ;
  അക്കം_cert_levels = sk_X509_അക്കം(certstack);

  വേണ്ടി(cert_level = 0; cert_level < അക്കം_cert_levels; cert_level++) {
    അക്ഷരം cert_algorithm[80] = "";
    അക്ഷരം group_name_final[80] = "";
    സ്ഥിരം X509_ALGOR *palg_cert = ശൂന്യ;
    സ്ഥിരം ASN1_OBJECT *paobj_cert = ശൂന്യ;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    പൂർണ്ണസംഖ്യ താക്കോൽ_bits;
    പൂർണ്ണസംഖ്യ താക്കോൽ_sec_bits;
    പൂർണ്ണസംഖ്യ get_group_name;
    സ്ഥിരം അക്ഷരം *type_name;

    current_cert = sk_X509_മൂല്യം(certstack, cert_level);
    എങ്കിൽ(!current_cert)
      തുടരുക;

    current_pkey = X509_get0_pubkey(current_cert);
    എങ്കിൽ(!current_pkey)
      തുടരുക;

    X509_get0_signature(ശൂന്യ, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, ശൂന്യ, ശൂന്യ, palg_cert);
    OBJ_obj2txt(cert_algorithm, വലുപ്പം(cert_algorithm), paobj_cert, 0);

    താക്കോൽ_bits = EVP_PKEY_bits(current_pkey);
#ifndef HAVE_OPENSSL3
#define EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#endif
    താക്കോൽ_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#ifdef HAVE_OPENSSL3
    {
      അക്ഷരം group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               വലുപ്പം(group_name), ശൂന്യ);
      curl_msnprintf(group_name_final, വലുപ്പം(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#അല്ലെങ്കിൽ
    get_group_name = 0;
    type_name = ശൂന്യ;
#endif

    infof(data, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          താക്കോൽ_bits, താക്കോൽ_sec_bits, cert_algorithm);
  }
}
#അല്ലെങ്കിൽ
#define infof_certstack(data, ssl)
#endif
#endif /* CURLVERBOSE */

സ്ഥിതം CURLcode ossl_check_issuer(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  X509 *server_cert)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = ശൂന്യ;
  BIO *fp = ശൂന്യ;
  അക്ഷരം err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode result = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  എങ്കിൽ(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->data,
                         (പൂർണ്ണസംഖ്യ)conn_config->issuercert_blob->len);
    എങ്കിൽ(!fp) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, വലുപ്പം(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    എങ്കിൽ(!fp) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, വലുപ്പം(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      പോകുക out;
    }

    എങ്കിൽ(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      എങ്കിൽ(verify_enabled)
        failf(data, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      പോകുക out;
    }
  }

  എങ്കിൽ(fp) {
    issuer = PEM_read_bio_X509(fp, ശൂന്യ, ZERO_NULL, ശൂന്യ);
    എങ്കിൽ(!issuer) {
      എങ്കിൽ(verify_enabled)
        failf(data, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      പോകുക out;
    }

    എങ്കിൽ(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      എങ്കിൽ(verify_enabled)
        failf(data, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      പോകുക out;
    }

    infof(data, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  എങ്കിൽ(fp)
    BIO_free(fp);
  എങ്കിൽ(issuer)
    X509_free(issuer);
  തിരികെ result;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *pinned(ഘടന Curl_cfilter *cf,
                          ഘടന Curl_easy *data)
{
  (ശൂന്യം)cf;
  തിരികെ
#ifndef CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#endif
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
}

സ്ഥിതം CURLcode ossl_check_pinned_താക്കോൽ(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      X509 *server_cert)
{
  CURLcode result = CURLE_OK;
  സ്ഥിരം അക്ഷരം *ptr = pinned(cf, data);
  എങ്കിൽ(ptr) {
    result = ossl_pkp_pin_peer_pubkey(data, server_cert, ptr);
    എങ്കിൽ(result)
      failf(data, "SSL: public key does not match pinned public key");
  }
  തിരികെ result;
}

#ifdef CURLVERBOSE
#define MAX_CERT_NAME_LENGTH 2048
സ്ഥിതം CURLcode ossl_infof_cert(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                X509 *server_cert)
{
  BIO *mem = ശൂന്യ;
  ഘടന dynbuf dname;
  അക്ഷരം err_buf[256] = "";
  അക്ഷരം *buf;
  long len;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!Curl_trc_is_verbose(data))
    തിരികെ CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_LENGTH);
  mem = BIO_new(BIO_s_mem());
  എങ്കിൽ(!mem) {
    failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, വലുപ്പം(err_buf)));
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  infof(data, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  result = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(data, "  subject: %s", result ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_data(mem, (അക്ഷരം **)&buf);
  infof(data, "  start date: %.*s", (പൂർണ്ണസംഖ്യ)len, buf);
  (ശൂന്യം)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_data(mem, (അക്ഷരം **)&buf);
  infof(data, "  expire date: %.*s", (പൂർണ്ണസംഖ്യ)len, buf);
  (ശൂന്യം)BIO_reset(mem);

  result = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  എങ്കിൽ(result) /* should be only fatal stuff like OOM */
    പോകുക out;
  infof(data, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  തിരികെ result;
}
#endif /* CURLVERBOSE */

#ifdef USE_APPLE_SECTRUST
ഘടന ossl_certs_ctx {
  അട്ടി_OF(X509) *sk;
  size_t അക്കം_certs;
  unsigned അക്ഷരം *last_der;
};

സ്ഥിതം CURLcode ossl_chain_get_der(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ശൂന്യം *user_data,
                                   size_t i,
                                   unsigned അക്ഷരം **pder,
                                   size_t *pder_len)
{
  ഘടന ossl_certs_ctx *chain = user_data;
  X509 *cert;
  പൂർണ്ണസംഖ്യ der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = ശൂന്യ;

  (ശൂന്യം)cf;
  (ശൂന്യം)data;
  *pder_len = 0;
  *pder = ശൂന്യ;

  എങ്കിൽ(i >= chain->അക്കം_certs)
    തിരികെ CURLE_TOO_LARGE;
  cert = sk_X509_മൂല്യം(chain->sk, (പൂർണ്ണസംഖ്യ)i);
  എങ്കിൽ(!cert)
    തിരികെ CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  എങ്കിൽ(der_len < 0)
    തിരികെ CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (size_t)der_len;
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ossl_apple_verify(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  ഘടന ossl_ctx *octx,
                                  ഘടന ssl_peer *peer)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന ossl_certs_ctx chain;
  CURLcode result;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, വലുപ്പം(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.അക്കം_certs = chain.sk ? sk_X509_അക്കം(chain.sk) : 0;

  എങ്കിൽ(!chain.അക്കം_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    എങ്കിൽ(!octx->reused_session) {
      failf(data, "SSL: could not get peer certificate chain");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    അല്ലെങ്കിൽ {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(data, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      തിരികെ CURLE_OK;
    }
  }
  അല്ലെങ്കിൽ {
#ifdef HAVE_BORINGSSL_LIKE
    സ്ഥിരം uint8_t *ocsp_data = ശൂന്യ;
#അല്ലെങ്കിൽ
    unsigned അക്ഷരം *ocsp_data = ശൂന്യ;
#endif
    long ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    എങ്കിൽ(!octx->reused_session)
      ocsp_len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_data);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    എങ്കിൽ(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_data = ശൂന്യ;
    }

    result = Curl_vtls_apple_verify(cf, data, peer, chain.അക്കം_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_data, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = ശൂന്യ;
  }
  octx->sectrust_verified = !result;
  തിരികെ result;
}
#endif /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന ossl_ctx *octx,
                                   ഘടന ssl_peer *peer)
{
  ഘടന connectdata *conn = cf->conn;
  ഘടന ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  long ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  എങ്കിൽ(data->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    result = ossl_certchain(data, octx->ssl);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  എങ്കിൽ(!server_cert) {
    /* no verification at all, this maybe acceptable */
    എങ്കിൽ(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, data))
      പോകുക out;

    failf(data, "SSL: could not get peer certificate");
    result = CURLE_PEER_FAILED_VERIFICATION;
    പോകുക out;
  }

#ifdef CURLVERBOSE
  result = ossl_infof_cert(cf, data, server_cert);
  എങ്കിൽ(result)
    പോകുക out;
  infof_certstack(data, octx->ssl);
#endif

  എങ്കിൽ(conn_config->verifyhost) {
    result = ossl_verifyhost(data, conn, peer, server_cert);
    എങ്കിൽ(result)
      പോകുക out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_result(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(data, "OpenSSL verify result: %lx", (unsigned long)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  എങ്കിൽ(verified)
    infof(data, "SSL certificate verified via OpenSSL.");

#ifdef USE_APPLE_SECTRUST
  എങ്കിൽ(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    result = ossl_apple_verify(cf, data, octx, peer);
    എങ്കിൽ(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      പോകുക out; /* unexpected error */
    എങ്കിൽ(octx->sectrust_verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#endif

  എങ്കിൽ(!verified) {
    /* no trust established, report the OpenSSL status */
    എങ്കിൽ(conn_config->verifypeer) {
      failf(data, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      result = CURLE_PEER_FAILED_VERIFICATION;
      പോകുക out;
    }
    infof(data, " SSL certificate verification failed, continuing anyway!");
  }

#ifndef OPENSSL_NO_OCSP
  എങ്കിൽ(conn_config->verifystatus &&
#ifdef USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, data)) &&
#endif
     !octx->reused_session) {
    result = verifystatus(cf, data, octx);
    എങ്കിൽ(result)
      പോകുക out;
  }
#endif

  result = ossl_check_issuer(cf, data, server_cert);
  എങ്കിൽ(result)
    പോകുക out;

  result = ossl_check_pinned_താക്കോൽ(cf, data, server_cert);

out:
  X509_free(server_cert);
  തിരികെ result;
}

സ്ഥിതം CURLcode ossl_connect_step3(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  result = Curl_ossl_check_peer_cert(cf, data, octx, &connssl->peer);
  എങ്കിൽ(result)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, data, connssl->peer.scache_താക്കോൽ);

  തിരികെ result;
}

#ifdef HAVE_OPENSSL_EARLYDATA
സ്ഥിതം CURLcode ossl_send_earlydata(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  സ്ഥിരം unsigned അക്ഷരം *buf;
  size_t blen, nwritten;
  പൂർണ്ണസംഖ്യ rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_result = CURLE_OK;
  ആകുമ്പോൾ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_data(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    എങ്കിൽ(rc <= 0) {
      long sslerror;
      അക്ഷരം error_buffer[256];
      പൂർണ്ണസംഖ്യ err = SSL_get_error(octx->ssl, rc);

      തിരഞ്ഞെടുപ്പ്(err) {
      അവസ്ഥ SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        result = CURLE_AGAIN;
        പോകുക out;
      അവസ്ഥ SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_AGAIN;
        പോകുക out;
      അവസ്ഥ SSL_ERROR_SYSCALL: {
        പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;

        എങ്കിൽ(octx->io_result == CURLE_AGAIN) {
          result = CURLE_AGAIN;
          പോകുക out;
        }
        sslerror = ERR_get_error();
        എങ്കിൽ(sslerror)
          ossl_strerror(sslerror, error_buffer, വലുപ്പം(error_buffer));
        അല്ലെങ്കിൽ എങ്കിൽ(sockerr)
          curlx_strerror(sockerr, error_buffer, വലുപ്പം(error_buffer));
        അല്ലെങ്കിൽ
          curl_msnprintf(error_buffer, വലുപ്പം(error_buffer), "%s",
                         SSL_ERROR_to_str(err));

        failf(data, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_SEND_ERROR;
        പോകുക out;
      }
      അവസ്ഥ SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(data, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_buffer, വലുപ്പം(error_buffer)));
        result = CURLE_SEND_ERROR;
        പോകുക out;
      }
      സ്ഥിരസ്ഥിതി:
        /* a true error */
        failf(data, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        result = CURLE_SEND_ERROR;
        പോകുക out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  തിരികെ result;
}
#endif /* HAVE_OPENSSL_EARLYDATA */

സ്ഥിതം CURLcode ossl_connect(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             bool *done)
{
  CURLcode result = CURLE_OK;
  ഘടന ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  എങ്കിൽ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  എങ്കിൽ(connssl->connecting_state == ssl_connect_1) {
    എങ്കിൽ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
      തിരികെ CURLE_OK;
    }
    CURL_TRC_CF(data, cf, "ossl_connect, step1");
    result = ossl_connect_step1(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(data, cf, "ossl_connect, step2");
#ifdef HAVE_OPENSSL_EARLYDATA
    എങ്കിൽ(connssl->earlydata_state == ssl_earlydata_await) {
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(connssl->earlydata_state == ssl_earlydata_sending) {
      result = ossl_send_earlydata(cf, data);
      എങ്കിൽ(result)
        പോകുക out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#endif
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    result = ossl_connect_step2(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
  }

  എങ്കിൽ(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(data, cf, "ossl_connect, step3");
    result = ossl_connect_step3(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
    connssl->connecting_state = ssl_connect_done;
#ifdef HAVE_OPENSSL_EARLYDATA
    എങ്കിൽ(connssl->earlydata_state > ssl_earlydata_none) {
      ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_data_status(octx->ssl) == SSL_EARLY_DATA_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
  }

  എങ്കിൽ(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(data, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  എങ്കിൽ(result == CURLE_AGAIN) {
    *done = FALSE;
    തിരികെ CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  തിരികെ result;
}

സ്ഥിതം bool ossl_data_pending(ഘടന Curl_cfilter *cf,
                              സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  (ശൂന്യം)data;
  തിരികെ (bool)connssl->നിവേശം_pending;
}

സ്ഥിതം CURLcode ossl_send(ഘടന Curl_cfilter *cf,
                          ഘടന Curl_easy *data,
                          സ്ഥിരം ശൂന്യം *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  പൂർണ്ണസംഖ്യ err;
  അക്ഷരം error_buffer[256];
  sslerr_t sslerror;
  പൂർണ്ണസംഖ്യ memlen;
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)INT_MAX) ? INT_MAX : (പൂർണ്ണസംഖ്യ)len;
  എങ്കിൽ(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    എങ്കിൽ(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  എങ്കിൽ(nwritten > 0) {
    എങ്കിൽ(nwritten > memlen) {
      failf(data, "SSL_write(len=%d) returned %d", memlen, nwritten);
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
    *pnwritten = (size_t)nwritten;
  }
  അല്ലെങ്കിൽ {
    err = SSL_get_error(octx->ssl, nwritten);

    തിരഞ്ഞെടുപ്പ്(err) {
    അവസ്ഥ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      result = CURLE_AGAIN;
      പോകുക out;
    അവസ്ഥ SSL_ERROR_WANT_WRITE:
      result = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      പോകുക out;
    അവസ്ഥ SSL_ERROR_SYSCALL: {
      പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;

      എങ്കിൽ(octx->io_result == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        result = CURLE_AGAIN;
        പോകുക out;
      }
      sslerror = ERR_get_error();
      എങ്കിൽ(sslerror)
        ossl_strerror(sslerror, error_buffer, വലുപ്പം(error_buffer));
      അല്ലെങ്കിൽ എങ്കിൽ(sockerr)
        curlx_strerror(sockerr, error_buffer, വലുപ്പം(error_buffer));
      അല്ലെങ്കിൽ
        curl_msnprintf(error_buffer, വലുപ്പം(error_buffer), "%s",
                       SSL_ERROR_to_str(err));

      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_buffer, sockerr);
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
    അവസ്ഥ SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(data, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_buffer, വലുപ്പം(error_buffer)));
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
    സ്ഥിരസ്ഥിതി:
      /* a true error */
      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      result = CURLE_SEND_ERROR;
      പോകുക out;
    }
  }

out:
  തിരികെ result;
}

സ്ഥിതം CURLcode ossl_recv(ഘടന Curl_cfilter *cf,
                          ഘടന Curl_easy *data,   /* transfer */
                          അക്ഷരം *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  അക്ഷരം error_buffer[256];
  unsigned long sslerror;
  പൂർണ്ണസംഖ്യ buffsize;
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)INT_MAX) ? INT_MAX : (പൂർണ്ണസംഖ്യ)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  എങ്കിൽ(nread > 0)
    *pnread = (size_t)nread;
  അല്ലെങ്കിൽ {
    /* failed SSL_read */
    പൂർണ്ണസംഖ്യ err = SSL_get_error(octx->ssl, nread);

    തിരഞ്ഞെടുപ്പ്(err) {
    അവസ്ഥ SSL_ERROR_NONE: /* this is not an error */
      നിർത്തുക;
    അവസ്ഥ SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      എങ്കിൽ(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(data, cf, "TLS close_notify");
      നിർത്തുക;
    അവസ്ഥ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      പോകുക out;
    അവസ്ഥ SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      പോകുക out;
    സ്ഥിരസ്ഥിതി:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      എങ്കിൽ(octx->io_result == CURLE_AGAIN) {
        result = CURLE_AGAIN;
        പോകുക out;
      }
      sslerror = ERR_get_error();
      എങ്കിൽ((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
        എങ്കിൽ(sslerror)
          ossl_strerror(sslerror, error_buffer, വലുപ്പം(error_buffer));
        അല്ലെങ്കിൽ എങ്കിൽ(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_buffer, വലുപ്പം(error_buffer));
        അല്ലെങ്കിൽ
          curl_msnprintf(error_buffer, വലുപ്പം(error_buffer), "%s",
                         SSL_ERROR_to_str(err));
        failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_RECV_ERROR;
        പോകുക out;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(err == SSL_ERROR_SYSCALL) {
        എങ്കിൽ(octx->io_result) {
          /* logging handling in underlying filter already */
          result = octx->io_result;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(connssl->peer_closed) {
          failf(data, "Connection closed abruptly");
          result = CURLE_RECV_ERROR;
        }
        അല്ലെങ്കിൽ {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
          എങ്കിൽ(sockerr)
            curlx_strerror(sockerr, error_buffer, വലുപ്പം(error_buffer));
          അല്ലെങ്കിൽ {
            curl_msnprintf(error_buffer, വലുപ്പം(error_buffer),
                           "Connection closed abruptly");
          }
          failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_buffer, sockerr);
          result = CURLE_RECV_ERROR;
        }
        പോകുക out;
      }
    }
  }

out:
  എങ്കിൽ((!result && !*pnread) || (result == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->നിവേശം_pending = FALSE;
  }
  CURL_TRC_CF(data, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (പൂർണ്ണസംഖ്യ)result, *pnread, connssl->നിവേശം_pending);
  തിരികെ result;
}

സ്ഥിതം CURLcode ossl_get_channel_binding(ഘടന Curl_easy *data,
                                         int8_t sockindex,
                                         ഘടന dynbuf *binding)
{
  X509 *cert;
  പൂർണ്ണസംഖ്യ mdnid;
  bool no_digest_acceptable = FALSE;
  സ്ഥിരം EVP_MD *algo_type = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *algo_name = ശൂന്യ;
  unsigned പൂർണ്ണസംഖ്യ length;
  unsigned അക്ഷരം buf[EVP_MAX_MD_SIZE];

  സ്ഥിതം സ്ഥിരം അക്ഷരം prefix[] = "tls-server-end-point:";
  ഘടന connectdata *conn = data->conn;
  ഘടന Curl_cfilter *cf = conn->cfilter[sockindex];
  ഘടന ossl_ctx *octx = ശൂന്യ;
  CURLcode result = CURLE_OK;

  ചെയ്യുക {
    സ്ഥിരം ഘടന Curl_cftype *cft = cf->cft;
    ഘടന ssl_connect_data *connssl = cf->ctx;

    എങ്കിൽ(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (ഘടന ossl_ctx *)connssl->backend;
      നിർത്തുക;
    }

    cf = cf->next;
  } ആകുമ്പോൾ(cf);

  എങ്കിൽ(!octx) {
    failf(data, "Failed to find the SSL filter");
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  എങ്കിൽ(!cert)
    /* No server certificate, do not do channel binding */
    തിരികെ CURLE_OK;

#ifdef HAVE_OPENSSL3
  {
    പൂർണ്ണസംഖ്യ pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    എങ്കിൽ(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(data, "certificate signature algorithm not recognized");
      result = CURLE_SSL_INVALIDCERTSTATUS;
      പോകുക out;
    }

    എങ്കിൽ(mdnid != NID_undef) {
      എങ്കിൽ(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      അല്ലെങ്കിൽ
        algo_type = EVP_get_digestbynid(mdnid);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      അക്ഷരം mdname[128] = "";
      പൂർണ്ണസംഖ്യ rc = EVP_PKEY_get_default_digest_name(pkey, mdname, വലുപ്പം(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      എങ്കിൽ(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(data, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#അല്ലെങ്കിൽ /* HAVE_OPENSSL3 */

  എങ്കിൽ(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, ശൂന്യ)) {
    failf(data,
          "Unable to find digest NID for certificate signature algorithm");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  എങ്കിൽ(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  അല്ലെങ്കിൽ {
    algo_type = EVP_get_digestbynid(mdnid);
    എങ്കിൽ(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(data, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      result = CURLE_SSL_INVALIDCERTSTATUS;
      പോകുക out;
    }
  }

#endif /* HAVE_OPENSSL3, else */

  എങ്കിൽ(!algo_type) {
    എങ്കിൽ(no_digest_acceptable) {
      infof(data, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      result = CURLE_OK;
      പോകുക out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(data, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക out;
  }

  എങ്കിൽ(!X509_digest(cert, algo_type, buf, &length)) {
    failf(data, "X509_digest() failed for channel binding");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    പോകുക out;
  }

  /* Append "tls-server-end-point:" */
  result = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  എങ്കിൽ(result)
    പോകുക out;

  /* Append digest */
  result = curlx_dyn_addn(binding, buf, length);

out:
  X509_free(cert);
  തിരികെ result;
}

size_t Curl_ossl_version(അക്ഷരം *buffer, size_t size)
{
#ifdef LIBRESSL_VERSION_സംഖ്യ
  അക്ഷരം *p;
  size_t എണ്ണം;
  സ്ഥിരം അക്ഷരം *ver = OpenSSL_version(OPENSSL_VERSION);
  സ്ഥിതം സ്ഥിരം അക്ഷരം expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  എങ്കിൽ(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  എണ്ണം = curl_msnprintf(buffer, size, "%s/%s", OSSL_PACKAGE, ver);
  വേണ്ടി(p = buffer; *p; ++p) {
    എങ്കിൽ(ISBLANK(*p))
      *p = '_';
  }
  തിരികെ എണ്ണം;
#അല്ലെങ്കിലെങ്കിൽ defined(OPENSSL_IS_AWSLC)
  തിരികെ curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_സംഖ്യ_STRING);
#അല്ലെങ്കിലെങ്കിൽ defined(OPENSSL_IS_BORINGSSL)
#ifdef CURL_BORINGSSL_VERSION
  തിരികെ curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#അല്ലെങ്കിൽ
  തിരികെ curl_msnprintf(buffer, size, OSSL_PACKAGE);
#endif
#അല്ലെങ്കിൽ /* OpenSSL 3+ */
  തിരികെ curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
}

/* can be called with data == NULL */
സ്ഥിതം CURLcode ossl_random(ഘടന Curl_easy *data,
                            unsigned അക്ഷരം *entropy, size_t length)
{
  പൂർണ്ണസംഖ്യ rc;
  എങ്കിൽ(data) {
    എങ്കിൽ(ossl_seed(data)) /* Initiate the seed if not already done */
      തിരികെ CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(!rand_enough())
      തിരികെ CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(length));
  തിരികെ rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

സ്ഥിതം CURLcode ossl_sha256sum(സ്ഥിരം unsigned അക്ഷരം *നിവേശം,
                               size_t len,
                               unsigned അക്ഷരം *sha256sum /* output */,
                               size_t unused)
{
  CURLcode result = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (ശൂന്യം)unused;

  mdctx = EVP_MD_CTX_new();
  എങ്കിൽ(!mdctx)
    തിരികെ CURLE_OUT_OF_MEMORY;
  എങ്കിൽ(!EVP_DigestInit_ex(mdctx, EVP_sha256(), ശൂന്യ)) {
    result = CURLE_FAILED_INIT;
    പോകുക out;
  }
  എങ്കിൽ(!EVP_DigestUpdate(mdctx, നിവേശം, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, ശൂന്യ))
    result = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  തിരികെ result;
}

സ്ഥിതം bool ossl_cert_status_request(ശൂന്യം)
{
#ifndef OPENSSL_NO_OCSP
  തിരികെ TRUE;
#അല്ലെങ്കിൽ
  തിരികെ FALSE;
#endif
}

സ്ഥിതം ശൂന്യം *ossl_get_internals(ഘടന ssl_connect_data *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  ഘടന ossl_ctx *octx = (ഘടന ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  തിരികെ info == CURLINFO_TLS_SESSION ?
    (ശൂന്യം *)octx->ssl_ctx : (ശൂന്യം *)octx->ssl;
}

സ്ഥിരം ഘടന Curl_ssl Curl_ssl_openssl = {
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

  വലുപ്പം(ഘടന ossl_ctx),

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
