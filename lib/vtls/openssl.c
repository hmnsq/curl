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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_OPENSSL

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "formdata.h" /* for the boundary function */
#অন্তর্ভাৱ "url.h" /* for the SSL config check function */
#অন্তর্ভাৱ "curlx/inet_pton.h"
#অন্তর্ভাৱ "vtls/openssl.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "vtls/vtls_int.h"
#অন্তর্ভাৱ "vtls/vtls_scache.h"
#অন্তর্ভাৱ "vauth/vauth.h"
#অন্তর্ভাৱ "vtls/keylog.h"
#অন্তর্ভাৱ "vtls/hostcheck.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "multiif.h"
#অন্তর্ভাৱ "curlx/strerr.h"
#অন্তর্ভাৱ "curlx/strparse.h"
#অন্তর্ভাৱ "curlx/strcopy.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "vdns/cf-dns.h"
#অন্তর্ভাৱ "vdns/httpsrr.h"
#অন্তর্ভাৱ "vtls/apple.h"
#ifdef USE_ECH
#অন্তর্ভাৱ "curlx/base64.h"
#endif

#অন্তর্ভাৱ <openssl/rand.h>
#অন্তর্ভাৱ <openssl/x509v3.h>
#ifndef OPENSSL_NO_DSA
#অন্তর্ভাৱ <openssl/dsa.h>
#endif
#অন্তর্ভাৱ <openssl/dh.h>
#অন্তর্ভাৱ <openssl/err.h>
#অন্তর্ভাৱ <openssl/conf.h>
#অন্তর্ভাৱ <openssl/bn.h>
#অন্তর্ভাৱ <openssl/rsa.h>
#অন্তর্ভাৱ <openssl/bio.h>
#অন্তর্ভাৱ <openssl/pkcs12.h>
#অন্তর্ভাৱ <openssl/tls1.h>
#অন্তর্ভাৱ <openssl/evp.h>

#যদি defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#অন্তর্ভাৱ <openssl/ech.h>
#endif

#ifndef OPENSSL_NO_OCSP
#অন্তর্ভাৱ <openssl/ocsp.h>
#endif

#যদি !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#define USE_OPENSSL_ENGINE
#অন্তর্ভাৱ <openssl/engine.h>
#endif

#ifdef LIBRESSL_VERSION_সংখ্যা
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  যদি LIBRESSL_VERSION_সংখ্যা < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#নোচেদ্যদি !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#endif

#যদি defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#অন্তর্ভাৱ <openssl/provider.h>
#অন্তর্ভাৱ <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#define OPENSSL_HAS_PROVIDERS
#endif

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#যদি !defined(LIBRESSL_VERSION_সংখ্যা) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_চাবি_EXPLICIT_PARAMS))
#define HAVE_SSL_CTX_SET_DEFAULT_READ_BUFFER_LEN 1
#endif

#যদি defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#অন্তর্ভাৱ <openssl/ui.h>
#endif

#ifdef HAVE_OPENSSL3
#define HAVE_EVP_PKEY_GET_PARAMS 1
#endif

#ifdef HAVE_EVP_PKEY_GET_PARAMS
#অন্তর্ভাৱ <openssl/core_names.h>
#define DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = রিক্ত
#define FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#অন্যথা
#define DECLARE_PKEY_PARAM_BIGNUM(name) ধ্রুৱ BIGNUM *name
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
#ifndef LIBRESSL_VERSION_সংখ্যা
#define HAVE_SSL_CTX_SET1_SIGALGS
#endif

#ifdef LIBRESSL_VERSION_সংখ্যা
#define OSSL_PACKAGE "LibreSSL"
#নোচেদ্যদি defined(OPENSSL_IS_AWSLC)
#define OSSL_PACKAGE "AWS-LC"
#নোচেদ্যদি defined(OPENSSL_IS_BORINGSSL)
#define OSSL_PACKAGE "BoringSSL"
#নোচেদ্যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#define OSSL_PACKAGE "quictls"
#অন্যথা
#define OSSL_PACKAGE "OpenSSL"
#endif

#ifdef HAVE_BORINGSSL_LIKE
প্রকারনির্দেশ size_t numcert_t;
প্রকারনির্দেশ uint32_t sslerr_t;
#অন্যথা
প্রকারনির্দেশ পূর্ণাঙ্ক numcert_t;
প্রকারনির্দেশ unsigned long sslerr_t;
#endif
#define ossl_valsize_t numcert_t

#যদি OPENSSL_VERSION_সংখ্যা >= 0x40100000L
#define ossl_ASN1_STRING_get_length ASN1_STRING_get_length
#অন্যথা
স্থির size_t ossl_ASN1_STRING_get_length(ধ্রুৱ ASN1_STRING *str)
{
   ধ্রুৱ পূর্ণাঙ্ক length = ASN1_STRING_length(str);
   ঘূৰাই length >= 0 ? (size_t)length : 0;
}
#endif

স্থির CURLcode ঠেলো_certinfo(সংরচনা Curl_easy *data,
                              BIO *mem, ধ্রুৱ ৱর্ণ *label, পূর্ণাঙ্ক অঙ্ক)
  WARN_UNUSED_RESULT;

স্থির CURLcode ঠেলো_certinfo(সংরচনা Curl_easy *data,
                              BIO *mem, ধ্রুৱ ৱর্ণ *label, পূর্ণাঙ্ক অঙ্ক)
{
  ৱর্ণ *ptr;
  long len = BIO_get_mem_data(mem, &ptr);
  CURLcode result = Curl_ssl_ঠেলো_certinfo_len(data, অঙ্ক, label, ptr, len);
  (শূন্য)BIO_reset(mem);
  ঘূৰাই result;
}

স্থির CURLcode pubkey_show(সংরচনা Curl_easy *data,
                            BIO *mem,
                            পূর্ণাঙ্ক অঙ্ক,
                            ধ্রুৱ ৱর্ণ *type,
                            ধ্রুৱ ৱর্ণ *name,
                            ধ্রুৱ BIGNUM *bn) WARN_UNUSED_RESULT;

স্থির CURLcode pubkey_show(সংরচনা Curl_easy *data,
                            BIO *mem,
                            পূর্ণাঙ্ক অঙ্ক,
                            ধ্রুৱ ৱর্ণ *type,
                            ধ্রুৱ ৱর্ণ *name,
                            ধ্রুৱ BIGNUM *bn)
{
  ৱর্ণ namebuf[32];

  curl_msnprintf(namebuf, পরিমাণ(namebuf), "%s(%s)", type, name);

  যদি(bn)
    BN_print(mem, bn);
  ঘূৰাই ঠেলো_certinfo(data, mem, namebuf, অঙ্ক);
}

#define print_pubkey_BN(_type, _name, অঙ্ক_2)           \
  pubkey_show(data, mem, অঙ্ক_2, #_type, #_name, _name)

স্থির পূর্ণাঙ্ক asn1_object_dump(ধ্রুৱ ASN1_OBJECT *a, ৱর্ণ *buf, size_t len)
{
  পূর্ণাঙ্ক i = i2t_ASN1_OBJECT(buf, (পূর্ণাঙ্ক)len, a);
  ঘূৰাই (i >= (পূর্ণাঙ্ক)len);  /* buffer too small */
}

স্থির CURLcode X509V3_ext(সংরচনা Curl_easy *data,
                           পূর্ণাঙ্ক certnum,
                           ধ্রুৱ স্তূপ_OF(X509_EXTENSION) *extsarg)
{
  পূর্ণাঙ্ক i;
  CURLcode result = CURLE_OK;
#ifdef LIBRESSL_VERSION_সংখ্যা
  স্তূপ_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#অন্যথা
  ধ্রুৱ স্তূপ_OF(X509_EXTENSION) *exts = extsarg;
#endif

  যদি((পূর্ণাঙ্ক)sk_X509_EXTENSION_অঙ্ক(exts) <= 0)
    /* no extensions, bail out */
    ঘূৰাই result;

  বাবে(i = 0; i < (পূর্ণাঙ্ক)sk_X509_EXTENSION_অঙ্ক(exts); i++) {
    ধ্রুৱ ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_মান(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    ৱর্ণ namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    যদি(!bio_out)
      ঘূৰাই result;

    obj = X509_EXTENSION_get_object(ext);

    যদি(asn1_object_dump(obj, namebuf, পরিমাণ(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    যদি(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (ধ্রুৱ ASN1_STRING *)X509_EXTENSION_get_data(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    result = Curl_ssl_ঠেলো_certinfo_len(data, certnum, namebuf, biomem->data,
                                        biomem->length);
    BIO_free(bio_out);
    যদি(result)
      ভঙ্গ;
  }
  ঘূৰাই result;
}

স্থির CURLcode get_pkey_rsa(সংরচনা Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, পূর্ণাঙ্ক i)
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
#অন্যথা
  RSA_get0_চাবি(rsa, &n, &e, রিক্ত);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (পূর্ণাঙ্ক)(n ? BN_অঙ্ক_bits(n) : 0));
  result = ঠেলো_certinfo(data, mem, "RSA Public Key", i);
  যদি(!result) {
    result = print_pubkey_BN(rsa, n, i);
    যদি(!result)
      result = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  ঘূৰাই result;
}

#ifndef OPENSSL_NO_DSA
স্থির CURLcode get_pkey_dsa(সংরচনা Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, পূর্ণাঙ্ক i)
{
  CURLcode result = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_চাবি);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_চাবি, &pub_চাবি);
#অন্যথা
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_চাবি(dsa, &pub_চাবি, রিক্ত);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dsa, p, i);
  যদি(!result)
    result = print_pubkey_BN(dsa, q, i);
  যদি(!result)
    result = print_pubkey_BN(dsa, g, i);
  যদি(!result)
    result = print_pubkey_BN(dsa, pub_চাবি, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_চাবি);
  ঘূৰাই result;
}
#endif /* !OPENSSL_NO_DSA */

স্থির CURLcode get_pkey_dh(সংরচনা Curl_easy *data,
                            EVP_PKEY *pubkey, BIO *mem, পূর্ণাঙ্ক i)
{
  CURLcode result;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_চাবি);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_চাবি, &pub_চাবি);
#অন্যথা
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_চাবি(dh, &pub_চাবি, রিক্ত);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dh, p, i);
  যদি(!result)
    result = print_pubkey_BN(dh, q, i);
  যদি(!result)
    result = print_pubkey_BN(dh, g, i);
  যদি(!result)
    result = print_pubkey_BN(dh, pub_চাবি, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_চাবি);
  ঘূৰাই result;
}

#ifdef HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
প্রকারনির্দেশ ধ্রুৱ X509_PUBKEY pubkeytype_t;
#অন্যথা
প্রকারনির্দেশ X509_PUBKEY pubkeytype_t;
#endif

স্থির CURLcode ossl_certchain(সংরচনা Curl_easy *data, SSL *ssl)
{
  CURLcode result;
  স্তূপ_OF(X509) *sk;
  পূর্ণাঙ্ক i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  যদি(!sk)
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_অঙ্ক(sk);
  যদি(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(data, "%d certificates is more than allowed (%d)", (পূর্ণাঙ্ক)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }

  result = Curl_ssl_init_certinfo(data, (পূর্ণাঙ্ক)numcerts);
  যদি(result)
    ঘূৰাই result;

  mem = BIO_new(BIO_s_mem());
  যদি(!mem)
    result = CURLE_OUT_OF_MEMORY;

  বাবে(i = 0; !result && (i < (পূর্ণাঙ্ক)numcerts); i++) {
    X509 *x = sk_X509_মান(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = রিক্ত;
    size_t j;
    ধ্রুৱ ASN1_BIT_STRING *psig = রিক্ত;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    result = ঠেলো_certinfo(data, mem, "Subject", i);
    যদি(result)
      ভঙ্গ;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    result = ঠেলো_certinfo(data, mem, "Issuer", i);
    যদি(result)
      ভঙ্গ;

    BIO_printf(mem, "%lx", (unsigned long)X509_get_version(x));
    result = ঠেলো_certinfo(data, mem, "Version", i);
    যদি(result)
      ভঙ্গ;

    {
      ASN1_INTEGER *অঙ্ক = X509_get_serial_সংখ্যা(x);
      ধ্রুৱ unsigned ৱর্ণ *numdata = ASN1_STRING_get0_data(অঙ্ক);
      ধ্রুৱ size_t numlen = ossl_ASN1_STRING_get_length(অঙ্ক);
      যদি(ASN1_STRING_type(অঙ্ক) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      বাবে(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    result = ঠেলো_certinfo(data, mem, "Serial Number", i);
    যদি(result)
      ভঙ্গ;

    {
      ধ্রুৱ X509_ALGOR *sigalg = রিক্ত;
      pubkeytype_t *xpubkey = রিক্ত;
      ASN1_OBJECT *pubkeyoid = রিক্ত;

      X509_get0_signature(&psig, &sigalg, x);
      যদি(sigalg) {
        ধ্রুৱ ASN1_OBJECT *sigalgoid = রিক্ত;
        X509_ALGOR_get0(&sigalgoid, রিক্ত, রিক্ত, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        result = ঠেলো_certinfo(data, mem, "Signature Algorithm", i);
        যদি(result)
          ভঙ্গ;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      যদি(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, রিক্ত, রিক্ত, রিক্ত, xpubkey);
        যদি(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          result = ঠেলো_certinfo(data, mem, "Public Key Algorithm", i);
          যদি(result)
            ভঙ্গ;
        }
      }

      result = X509V3_ext(data, i, X509_get0_extensions(x));
      যদি(result)
        ভঙ্গ;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    result = ঠেলো_certinfo(data, mem, "Start date", i);
    যদি(result)
      ভঙ্গ;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    result = ঠেলো_certinfo(data, mem, "Expire date", i);
    যদি(result)
      ভঙ্গ;

    pubkey = X509_get_pubkey(x);
    যদি(!pubkey)
      infof(data, "   Unable to load public key");
    অন্যথা {
      ৱিকল্প(EVP_PKEY_id(pubkey)) {
      পক্ষ EVP_PKEY_RSA:
        result = get_pkey_rsa(data, pubkey, mem, i);
        ভঙ্গ;

#ifndef OPENSSL_NO_DSA
      পক্ষ EVP_PKEY_DSA:
        result = get_pkey_dsa(data, pubkey, mem, i);
        ভঙ্গ;
#endif

      পক্ষ EVP_PKEY_DH:
        result = get_pkey_dh(data, pubkey, mem, i);
        ভঙ্গ;
      }
      EVP_PKEY_free(pubkey);
    }

    যদি(!result && psig) {
      ধ্রুৱ unsigned ৱর্ণ *psigdata = ASN1_STRING_get0_data(psig);
      ধ্রুৱ size_t psiglen = ossl_ASN1_STRING_get_length(psig);
      বাবে(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      result = ঠেলো_certinfo(data, mem, "Signature", i);
    }

    যদি(!result) {
      PEM_write_bio_X509(mem, x);
      result = ঠেলো_certinfo(data, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  যদি(result)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(data);

  ঘূৰাই result;
}

স্থির পূর্ণাঙ্ক ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_data(bio, রিক্ত);
  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক ossl_bio_cf_destroy(BIO *bio)
{
  যদি(!bio)
    ঘূৰাই 0;
  ঘূৰাই 1;
}

স্থির long ossl_bio_cf_ctrl(BIO *bio, পূর্ণাঙ্ক cmd, long অঙ্ক, শূন্য *ptr)
{
  সংরচনা Curl_cfilter *cf = BIO_get_data(bio);
  long ret = 1;

  (শূন্য)cf;
  (শূন্য)ptr;
  ৱিকল্প(cmd) {
  পক্ষ BIO_CTRL_GET_CLOSE:
    ret = (long)BIO_get_shutdown(bio);
    ভঙ্গ;
  পক্ষ BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (পূর্ণাঙ্ক)অঙ্ক);
    ভঙ্গ;
  পক্ষ BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    ভঙ্গ;
  পক্ষ BIO_CTRL_DUP:
    ret = 1;
    ভঙ্গ;
  পক্ষ BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    সংরচনা ssl_connect_data *connssl = cf->ctx;
    ঘূৰাই connssl->peer_closed;
  }
  স্ৱভাৱ:
    ret = 0;
    ভঙ্গ;
  }
  ঘূৰাই ret;
}

স্থির পূর্ণাঙ্ক ossl_bio_cf_out_write(BIO *bio, ধ্রুৱ ৱর্ণ *buf, পূর্ণাঙ্ক blen)
{
  সংরচনা Curl_cfilter *cf = BIO_get_data(bio);
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  যদি(blen < 0)
    ঘূৰাই 0;

  result = Curl_conn_cf_send(cf->next, data,
                             (ধ্রুৱ uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (পূর্ণাঙ্ক)result, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  যদি(result) {
    যদি(result == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    ঘূৰাই -1;
  }
  ঘূৰাই (পূর্ণাঙ্ক)nwritten;
}

স্থির পূর্ণাঙ্ক ossl_bio_cf_in_read(BIO *bio, ৱর্ণ *buf, পূর্ণাঙ্ক blen)
{
  সংরচনা Curl_cfilter *cf = BIO_get_data(bio);
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result, r2;

  DEBUGASSERT(data);
  /* OpenSSL catches this case, so should we. */
  যদি(!buf)
    ঘূৰাই 0;
  যদি(blen < 0)
    ঘূৰাই 0;

  result = Curl_conn_cf_recv(cf->next, data, buf, (size_t)blen, &nread);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (পূর্ণাঙ্ক)result, nread);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  যদি(result) {
    যদি(result == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  অন্যথা {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->প্ৰবেশ_pending = TRUE;
    যদি(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  যদি(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, data, octx);
    যদি(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_result = r2;
      ঘূৰাই -1;
    }
    octx->x509_store_setup = TRUE;
  }
  ঘূৰাই result ? -1 : (পূর্ণাঙ্ক)nread;
}

স্থির BIO_METHOD *ossl_bio_cf_method_create(শূন্য)
{
  BIO_METHOD *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  যদি(m) {
    BIO_meth_set_write(m, &ossl_bio_cf_out_write);
    BIO_meth_set_read(m, &ossl_bio_cf_in_read);
    BIO_meth_set_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_set_create(m, &ossl_bio_cf_create);
    BIO_meth_set_destroy(m, &ossl_bio_cf_destroy);
  }
  ঘূৰাই m;
}

স্থির শূন্য ossl_bio_cf_method_free(BIO_METHOD *m)
{
  যদি(m)
    BIO_meth_free(m);
}

#ifndef HAVE_KEYLOG_UPSTREAM
#ifdef HAVE_KEYLOG_CALLBACK
স্থির শূন্য ossl_keylog_callback(ধ্রুৱ SSL *ssl, ধ্রুৱ ৱর্ণ *সাৰি)
{
  (শূন্য)ssl;

  Curl_tls_keylog_write_সাৰি(সাৰি);
}
#অন্যথা
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
স্থির শূন্য ossl_log_tls12_secret(ধ্রুৱ SSL *ssl, bool *keylog_done)
{
  ধ্রুৱ SSL_SESSION *session;
  unsigned ৱর্ণ client_random[SSL3_RANDOM_SIZE];
  unsigned ৱর্ণ master_চাবি[SSL_MAX_MASTER_চাবি_LENGTH];
  পূর্ণাঙ্ক master_চাবি_length = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  যদি(!session || *keylog_done) {
    ERR_বেৰ_কৰো_to_mark();
    ঘূৰাই;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_SIZE);
  master_চাবি_length = (পূর্ণাঙ্ক)
    SSL_SESSION_get_master_চাবি(session, master_চাবি, SSL_MAX_MASTER_চাবি_LENGTH);

  ERR_বেৰ_কৰো_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  যদি(master_চাবি_length <= 0)
    ঘূৰাই;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        পরিমাণ(client_random),
                        master_চাবি, master_চাবি_length);
}
#endif /* !HAVE_KEYLOG_CALLBACK */
#endif /* HAVE_KEYLOG_UPSTREAM */

স্থির ধ্রুৱ ৱর্ণ *SSL_ERROR_to_str(পূর্ণাঙ্ক err)
{
  ৱিকল্প(err) {
  পক্ষ SSL_ERROR_NONE:
    ঘূৰাই "SSL_ERROR_NONE";
  পক্ষ SSL_ERROR_SSL:
    ঘূৰাই "SSL_ERROR_SSL";
  পক্ষ SSL_ERROR_WANT_READ:
    ঘূৰাই "SSL_ERROR_WANT_READ";
  পক্ষ SSL_ERROR_WANT_WRITE:
    ঘূৰাই "SSL_ERROR_WANT_WRITE";
  পক্ষ SSL_ERROR_WANT_X509_LOOKUP:
    ঘূৰাই "SSL_ERROR_WANT_X509_LOOKUP";
  পক্ষ SSL_ERROR_SYSCALL:
    ঘূৰাই "SSL_ERROR_SYSCALL";
  পক্ষ SSL_ERROR_ZERO_RETURN:
    ঘূৰাই "SSL_ERROR_ZERO_RETURN";
  পক্ষ SSL_ERROR_WANT_CONNECT:
    ঘূৰাই "SSL_ERROR_WANT_CONNECT";
  পক্ষ SSL_ERROR_WANT_ACCEPT:
    ঘূৰাই "SSL_ERROR_WANT_ACCEPT";
#ifdef SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  পক্ষ SSL_ERROR_WANT_ASYNC:
    ঘূৰাই "SSL_ERROR_WANT_ASYNC";
#endif
#ifdef SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  পক্ষ SSL_ERROR_WANT_ASYNC_JOB:
    ঘূৰাই "SSL_ERROR_WANT_ASYNC_JOB";
#endif
#ifdef SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  পক্ষ SSL_ERROR_WANT_CLIENT_HELLO_CB:
    ঘূৰাই "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#endif
  স্ৱভাৱ:
    ঘূৰাই "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
স্থির ৱর্ণ *ossl_strerror(unsigned long error, ৱর্ণ *buf, size_t size)
{
  size_t len;
  DEBUGASSERT(size);
  *buf = '\0';

  len = Curl_ossl_version(buf, size);
  DEBUGASSERT(len < (size - 2));
  যদি(len < (size - 2)) {
    buf += len;
    size -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ifdef HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, size);
#অন্যথা
  ERR_error_string_n(error, buf, size);
#endif

  যদি(!*buf) {
    ধ্রুৱ ৱর্ণ *বাৰ্তা = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, size, বাৰ্তা, strlen(বাৰ্তা));
  }

  ঘূৰাই buf;
}

স্থির পূর্ণাঙ্ক passwd_callback(ৱর্ণ *buf, পূর্ণাঙ্ক অঙ্ক, পূর্ণাঙ্ক encrypting, শূন্য *password)
{
  DEBUGASSERT(encrypting == 0);

  যদি(!encrypting && অঙ্ক >= 0 && password) {
    পূর্ণাঙ্ক klen = curlx_uztosi(strlen((ৱর্ণ *)password));
    যদি(অঙ্ক > klen) {
      memcpy(buf, password, klen + 1);
      ঘূৰাই klen;
    }
  }
  ঘূৰাই 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
স্থির bool rand_enough(শূন্য)
{
  ঘূৰাই RAND_status() != 0;
}

স্থির CURLcode ossl_seed(সংরচনা Curl_easy *data)
{
  /* This might get called before it has been added to a multi handle */
  যদি(data->multi && data->multi->ssl_seeded)
    ঘূৰাই CURLE_OK;

  যদি(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    যদি(data->multi)
      data->multi->ssl_seeded = TRUE;
    ঘূৰাই CURLE_OK;
  }
  failf(data, "Insufficient randomness");
  ঘূৰাই CURLE_SSL_CONNECT_ERROR;
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
স্থির পূর্ণাঙ্ক ossl_do_file_type(ধ্রুৱ ৱর্ণ *type)
{
  যদি(!type || !type[0])
    ঘূৰাই SSL_FILETYPE_PEM;
  যদি(curl_strequal(type, "PEM"))
    ঘূৰাই SSL_FILETYPE_PEM;
  যদি(curl_strequal(type, "DER"))
    ঘূৰাই SSL_FILETYPE_ASN1;
  যদি(curl_strequal(type, "PROV"))
    ঘূৰাই SSL_FILETYPE_PROVIDER;
  যদি(curl_strequal(type, "ENG"))
    ঘূৰাই SSL_FILETYPE_ENGINE;
  যদি(curl_strequal(type, "P12"))
    ঘূৰাই SSL_FILETYPE_PKCS12;
  ঘূৰাই -1;
}

#যদি defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
স্থির পূর্ণাঙ্ক ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ধ্রুৱ ৱর্ণ *password;
  ৱিকল্প(UI_get_string_type(uis)) {
  পক্ষ UIT_PROMPT:
  পক্ষ UIT_VERIFY:
    password = (ধ্রুৱ ৱর্ণ *)UI_get0_user_data(ui);
    যদি(password && (UI_get_প্ৰবেশ_flags(uis) & UI_প্ৰবেশ_FLAG_DEFAULT_PWD)) {
      UI_set_result(ui, uis, password);
      ঘূৰাই 1;
    }
    FALLTHROUGH();
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
স্থির পূর্ণাঙ্ক ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  ৱিকল্প(UI_get_string_type(uis)) {
  পক্ষ UIT_PROMPT:
  পক্ষ UIT_VERIFY:
    যদি(UI_get0_user_data(ui) &&
       (UI_get_প্ৰবেশ_flags(uis) & UI_প্ৰবেশ_FLAG_DEFAULT_PWD)) {
      ঘূৰাই 1;
    }
    FALLTHROUGH();
  স্ৱভাৱ:
    ভঙ্গ;
  }
  ঘূৰাই UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
স্থির bool is_pkcs11_uri(ধ্রুৱ ৱর্ণ *string)
{
  ঘূৰাই string && curl_strnequal(string, "pkcs11:", 7);
}

#endif

স্থির CURLcode ossl_set_engine(সংরচনা Curl_easy *data, সংরচনা ossl_ctx *octx,
                                ধ্রুৱ ৱর্ণ *name);
#ifdef OPENSSL_HAS_PROVIDERS
স্থির CURLcode ossl_set_provider(সংরচনা Curl_easy *data,
                                  সংরচনা ossl_ctx *octx,
                                  ধ্রুৱ ৱর্ণ *iname);
#endif

স্থির পূর্ণাঙ্ক use_certificate_blob(SSL_CTX *ctx, ধ্রুৱ সংরচনা curl_blob *blob,
                                পূর্ণাঙ্ক type, ধ্রুৱ ৱর্ণ *চাবি_passwd)
{
  পূর্ণাঙ্ক ret = 0;
  X509 *x = রিক্ত;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->data, (পূর্ণাঙ্ক)blob->len);
  যদি(!in)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  যদি(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, রিক্ত);
  }
  অন্যথা যদি(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, রিক্ত, passwd_callback, CURL_UNCONST(চাবি_passwd));
  }
  অন্যথা {
    ret = 0;
    গচ্ছ end;
  }

  যদি(!x) {
    ret = 0;
    গচ্ছ end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  ঘূৰাই ret;
}

স্থির পূর্ণাঙ্ক use_privatekey_blob(SSL_CTX *ctx, ধ্রুৱ সংরচনা curl_blob *blob,
                               পূর্ণাঙ্ক type, ধ্রুৱ ৱর্ণ *চাবি_passwd)
{
  পূর্ণাঙ্ক ret = 0;
  EVP_PKEY *pkey = রিক্ত;
  BIO *in = BIO_new_mem_buf(blob->data, (পূর্ণাঙ্ক)blob->len);
  যদি(!in)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  যদি(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_Private_চাবি(in, রিক্ত, passwd_callback,
                                   CURL_UNCONST(চাবি_passwd));
  অন্যথা যদি(type == SSL_FILETYPE_ASN1)
    pkey = d2i_Private_চাবি_bio(in, রিক্ত);
  অন্যথা
    গচ্ছ end;

  যদি(!pkey)
    গচ্ছ end;

  ret = SSL_CTX_use_Private_চাবি(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  ঘূৰাই ret;
}

স্থির পূর্ণাঙ্ক use_certificate_chain_blob(SSL_CTX *ctx,
                                      ধ্রুৱ সংরচনা curl_blob *blob,
                                      ধ্রুৱ ৱর্ণ *চাবি_passwd)
{
  পূর্ণাঙ্ক ret = 0;
  X509 *x = রিক্ত;
  BIO *in = BIO_new_mem_buf(blob->data, (পূর্ণাঙ্ক)blob->len);
  যদি(!in)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, রিক্ত,
                            passwd_callback, CURL_UNCONST(চাবি_passwd));
  যদি(!x)
    গচ্ছ end;

  ret = SSL_CTX_use_certificate(ctx, x);

  যদি(ERR_peek_error() != 0)
    ret = 0;

  যদি(ret) {
    X509 *ca;
    sslerr_t err;

    যদি(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      গচ্ছ end;
    }

    যেতিয়ালৈ((ca = PEM_read_bio_X509(in, রিক্ত, passwd_callback,
                                  CURL_UNCONST(চাবি_passwd))) != রিক্ত) {

      যদি(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        গচ্ছ end;
      }
    }

    err = ERR_peek_last_error();
    যদি((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_সাৰি))
      ERR_clear_error();
    অন্যথা
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  ঘূৰাই ret;
}

স্থির পূর্ণাঙ্ক enginecheck(সংরচনা Curl_easy *data,
                       সংরচনা ossl_ctx *octx,
                       ধ্রুৱ ৱর্ণ *চাবি_file,
                       ধ্রুৱ ৱর্ণ *চাবি_passwd)
{
#ifdef USE_OPENSSL_ENGINE
  EVP_PKEY *priv_চাবি = রিক্ত;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  যদি(!octx->p.engine && is_pkcs11_uri(চাবি_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    ঘূৰাই 0;

  যদি(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    যদি(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      ঘূৰাই 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);
    priv_চাবি = ENGINE_load_private_চাবি(octx->p.engine, চাবি_file,
                                       ui_method,
                                       CURL_UNCONST(চাবি_passwd));
    UI_destroy_method(ui_method);
    যদি(!priv_চাবি) {
      failf(data, "failed to load private key from crypto engine");
      ঘূৰাই 0;
    }
    যদি(SSL_CTX_use_Private_চাবি(ctx, priv_চাবি) != 1) {
      failf(data, "unable to set private key");
      EVP_PKEY_free(priv_চাবি);
      ঘূৰাই 0;
    }
    EVP_PKEY_free(priv_চাবি);  /* we do not need the handle any more... */
  }
  অন্যথা {
    failf(data, "crypto engine not set, cannot load private key");
    ঘূৰাই 0;
  }
  ঘূৰাই 1;
#অন্যথা
  (শূন্য)octx;
  (শূন্য)চাবি_file;
  (শূন্য)চাবি_passwd;
  failf(data, "SSL_FILETYPE_ENGINE not supported for private key");
  ঘূৰাই 0;
#endif
}

স্থির পূর্ণাঙ্ক providercheck(সংরচনা Curl_easy *data,
                         সংরচনা ossl_ctx *octx,
                         ধ্রুৱ ৱর্ণ *চাবি_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  ৱর্ণ error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  যদি(!octx->p.provider_loaded && is_pkcs11_uri(চাবি_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK) {
    ঘূৰাই 0;
  }

  যদি(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_চাবি = রিক্ত;
    OSSL_STORE_CTX *store = রিক্ত;
    OSSL_STORE_INFO *info = রিক্ত;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    যদি(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      ঘূৰাই 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(চাবি_file, octx->p.libctx,
                               octx->p.propq, ui_method, রিক্ত, রিক্ত,
                               রিক্ত, রিক্ত);
    যদি(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      UI_destroy_method(ui_method);
      ঘূৰাই 0;
    }
    যদি(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    যদি(info) {
      পূর্ণাঙ্ক ossl_type = OSSL_STORE_INFO_get_type(info);

      যদি(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_চাবি = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    যদি(!priv_চাবি) {
      failf(data, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      ঘূৰাই 0;
    }

    যদি(SSL_CTX_use_Private_চাবি(ctx, priv_চাবি) != 1) {
      failf(data, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      EVP_PKEY_free(priv_চাবি);
      ঘূৰাই 0;
    }
    EVP_PKEY_free(priv_চাবি); /* we do not need the handle any more... */
  }
  অন্যথা {
    failf(data, "crypto provider not set, cannot load private key");
    ঘূৰাই 0;
  }
  ঘূৰাই 1;
#অন্যথা
  (শূন্য)octx;
  (শূন্য)চাবি_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for private key");
  ঘূৰাই 0;
#endif
}

স্থির পূর্ণাঙ্ক engineload(সংরচনা Curl_easy *data,
                      সংরচনা ossl_ctx *octx,
                      ধ্রুৱ ৱর্ণ *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#যদি defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  ৱর্ণ error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  যদি(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    ঘূৰাই 0;

  যদি(octx->p.engine) {
    স্থির ধ্রুৱ ৱর্ণ cmd_name[] = "LOAD_CERT_CTRL";
    সংরচনা {
      ধ্রুৱ ৱর্ণ *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = রিক্ত;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    যদি(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), রিক্ত)) {
      failf(data, "SSL engine does not support loading certificates");
      ঘূৰাই 0;
    }

    /* Load the certificate from the engine */
    যদি(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, রিক্ত, 1)) {
      failf(data, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      ঘূৰাই 0;
    }

    যদি(!params.cert) {
      failf(data, "SSL engine did not initialize the certificate properly.");
      ঘূৰাই 0;
    }

    যদি(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      X509_free(params.cert);
      ঘূৰাই 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  অন্যথা {
    failf(data, "crypto engine not set, cannot load certificate");
    ঘূৰাই 0;
  }
  ঘূৰাই 1;
#অন্যথা
  (শূন্য)octx;
  (শূন্য)cert_file;
  failf(data, "SSL_FILETYPE_ENGINE not supported for certificate");
  ঘূৰাই 0;
#endif
}

স্থির পূর্ণাঙ্ক providerload(সংরচনা Curl_easy *data,
                        সংরচনা ossl_ctx *octx,
                        ধ্রুৱ ৱর্ণ *cert_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  ৱর্ণ error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  যদি(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK)
    ঘূৰাই 0;

  যদি(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = রিক্ত;
    X509 *cert = রিক্ত;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত);
    পূর্ণাঙ্ক rc;

    যদি(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      ঘূৰাই 0;
    }
    যদি(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    যদি(info) {
      পূর্ণাঙ্ক ossl_type = OSSL_STORE_INFO_get_type(info);

      যদি(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    যদি(!cert) {
      failf(data, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      ঘূৰাই 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    যদি(rc != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      ঘূৰাই 0;
    }
  }
  অন্যথা {
    failf(data, "crypto provider not set, cannot load certificate");
    ঘূৰাই 0;
  }
  ঘূৰাই 1;
#অন্যথা
  (শূন্য)octx;
  (শূন্য)cert_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for certificate");
  ঘূৰাই 0;
#endif
}

স্থির পূর্ণাঙ্ক pkcs12load(সংরচনা Curl_easy *data,
                      SSL_CTX* ctx,
                      ধ্রুৱ সংরচনা curl_blob *cert_blob,
                      ধ্রুৱ ৱর্ণ *cert_file,
                      ধ্রুৱ ৱর্ণ *চাবি_passwd)
{
  ৱর্ণ error_buffer[256];
  BIO *cert_bio = রিক্ত;
  PKCS12 *p12 = রিক্ত;
  EVP_PKEY *pri;
  X509 *x509;
  পূর্ণাঙ্ক cert_done = 0;
  স্তূপ_OF(X509) *ca = রিক্ত;
  যদি(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->data, (পূর্ণাঙ্ক)cert_blob->len);
    যদি(!cert_bio) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      ঘূৰাই 0;
    }
  }
  অন্যথা {
    cert_bio = BIO_new(BIO_s_file());
    যদি(!cert_bio) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
      ঘূৰাই 0;
    }

    যদি(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(data, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      ঘূৰাই 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, রিক্ত);
  BIO_free(cert_bio);

  যদি(!p12) {
    failf(data, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    ঘূৰাই 0;
  }

  যদি(!PKCS12_parse(p12, চাবি_passwd, &pri, &x509, &ca)) {
    failf(data, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, পরিমাণ(error_buffer)));
    PKCS12_free(p12);
    ঘূৰাই 0;
  }

  PKCS12_free(p12);

  যদি(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(data, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, পরিমাণ(error_buffer)));
    গচ্ছ fail;
  }

  যদি(SSL_CTX_use_Private_চাবি(ctx, pri) != 1) {
    failf(data, "unable to use private key from PKCS12 file '%s'", cert_file);
    গচ্ছ fail;
  }

  যদি(!SSL_CTX_check_private_চাবি(ctx)) {
    failf(data, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    গচ্ছ fail;
  }
  /* Set Certificate Verification chain */
  যদি(ca) {
    যেতিয়ালৈ(sk_X509_অঙ্ক(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_বেৰ_কৰো(ca);
      যদি(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to client CA list");
        গচ্ছ fail;
      }
      যদি(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to certificate chain");
        গচ্ছ fail;
      }
    }
  }

  cert_done = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_বেৰ_কৰো_free(ca, X509_free);
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বেৰ_কৰো
#endif
  যদি(!cert_done)
    ঘূৰাই 0; /* failure! */
  ঘূৰাই 1;
}

স্থির CURLcode client_cert(সংরচনা Curl_easy *data,
                            সংরচনা ossl_ctx *octx,
                            ৱর্ণ *cert_file,
                            ধ্রুৱ সংরচনা curl_blob *cert_blob,
                            ধ্রুৱ ৱর্ণ *cert_type,
                            ৱর্ণ *চাবি_file,
                            ধ্রুৱ সংরচনা curl_blob *চাবি_blob,
                            ধ্রুৱ ৱর্ণ *চাবি_type,
                            ৱর্ণ *চাবি_passwd)
{
  ৱর্ণ error_buffer[256];
  bool check_privkey = TRUE;
  পূর্ণাঙ্ক file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  যদি(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    পূর্ণাঙ্ক cert_use_result;

    যদি(চাবি_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, চাবি_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    ৱিকল্প(file_type) {
    পক্ষ SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_result = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, চাবি_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      যদি(cert_use_result != 1) {
        failf(data,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            পরিমাণ(error_buffer)));
        ঘূৰাই CURLE_SSL_CERTPROBLEM;
      }
      ভঙ্গ;

    পক্ষ SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_result = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, চাবি_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      যদি(cert_use_result != 1) {
        failf(data,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            পরিমাণ(error_buffer)));
        ঘূৰাই CURLE_SSL_CERTPROBLEM;
      }
      ভঙ্গ;

    পক্ষ SSL_FILETYPE_ENGINE:
      যদি(!cert_file || !engineload(data, octx, cert_file))
        ঘূৰাই CURLE_SSL_CERTPROBLEM;
      ভঙ্গ;

    পক্ষ SSL_FILETYPE_PROVIDER:
      যদি(!cert_file || !providerload(data, octx, cert_file))
        ঘূৰাই CURLE_SSL_CERTPROBLEM;
      ভঙ্গ;

    পক্ষ SSL_FILETYPE_PKCS12:
      যদি(!pkcs12load(data, ctx, cert_blob, cert_file, চাবি_passwd))
        ঘূৰাই CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      ভঙ্গ;

    স্ৱভাৱ:
      failf(data, "not supported file type '%s' for certificate", cert_type);
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    }

    যদি(!চাবি_file && !চাবি_blob) {
      চাবি_file = cert_file;
      চাবি_blob = cert_blob;
    }
    অন্যথা
      file_type = ossl_do_file_type(চাবি_type);

    ৱিকল্প(file_type) {
    পক্ষ SSL_FILETYPE_PEM:
    পক্ষ SSL_FILETYPE_ASN1:
      cert_use_result = চাবি_blob ?
        use_privatekey_blob(ctx, চাবি_blob, file_type, চাবি_passwd) :
      SSL_CTX_use_Private_চাবি_file(ctx, চাবি_file, file_type);
      যদি(cert_use_result != 1) {
        failf(data, "unable to set private key file: '%s' type %s",
              চাবি_file ? চাবি_file : "(memory blob)",
              চাবি_type ? চাবি_type : "PEM");
        ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
      }
      ভঙ্গ;
    পক্ষ SSL_FILETYPE_ENGINE:
      যদি(!enginecheck(data, octx, চাবি_file, চাবি_passwd))
        ঘূৰাই CURLE_SSL_CERTPROBLEM;
      ভঙ্গ;

    পক্ষ SSL_FILETYPE_PROVIDER:
      যদি(!providercheck(data, octx, চাবি_file))
        ঘূৰাই CURLE_SSL_CERTPROBLEM;
      ভঙ্গ;

    পক্ষ SSL_FILETYPE_PKCS12:
      যদি(!pcks12_done) {
        failf(data, "file type P12 for private key not supported");
        ঘূৰাই CURLE_SSL_CERTPROBLEM;
      }
      ভঙ্গ;
    স্ৱভাৱ:
      failf(data, "not supported file type for private key");
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    যদি(!ssl) {
      failf(data, "unable to create an SSL structure");
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }

    x509 = SSL_get_certificate(ssl);

    যদি(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#যদি !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_চাবি = SSL_get_privatekey(ssl);
      যদি(EVP_PKEY_id(priv_চাবি) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_চাবি);
        যদি(RSA_flags(rsa) & RSA_METHOD_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#endif

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    যদি(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      যদি(!SSL_CTX_check_private_চাবি(ctx)) {
        failf(data, "Private key does not match the certificate public key");
        ঘূৰাই CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  ঘূৰাই CURLE_OK;
}

#ifdef CURLVERBOSE
/* returns non-zero on failure */
স্থির CURLcode x509_name_oneline(ধ্রুৱ X509_NAME *a, সংরচনা dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  পূর্ণাঙ্ক rc;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  যদি(bio_out) {
    unsigned long flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    যদি(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      result = curlx_dyn_addn(d, biomem->data, biomem->length);
    }
    BIO_free(bio_out);
  }
  ঘূৰাই result;
}
#endif

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
স্থির পূর্ণাঙ্ক ossl_init(শূন্য)
{
  ধ্রুৱ uint64_t flags =
#ifdef OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#endif
#ifdef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    OPENSSL_INIT_NO_LOAD_CONFIG |
#অন্যথা
    OPENSSL_INIT_LOAD_CONFIG |
#endif
    0;
  OPENSSL_init_ssl(flags, রিক্ত);

#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#endif

  ঘূৰাই 1;
}

/* Global cleanup */
স্থির শূন্য ossl_cleanup(শূন্য)
{
#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#endif
}

/* Selects an OpenSSL crypto engine or provider.
 */
স্থির CURLcode ossl_set_engine(সংরচনা Curl_easy *data, সংরচনা ossl_ctx *octx,
                                ধ্রুৱ ৱর্ণ *name)
{
#ifdef USE_OPENSSL_ENGINE
  CURLcode result = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  যদি(e) {

    যদি(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = রিক্ত;
    }
    যদি(!ENGINE_init(e)) {
      ৱর্ণ buf[256];

      ENGINE_free(e);
      failf(data, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, পরিমাণ(buf)));
      result = CURLE_SSL_ENGINE_INITFAILED;
      e = রিক্ত;
    }
    অন্যথা {
      result = CURLE_OK;
    }
    octx->p.engine = e;
    ঘূৰাই result;
  }
#endif
#ifdef OPENSSL_HAS_PROVIDERS
  ঘূৰাই ossl_set_provider(data, octx, name);
#অন্যথা
  (শূন্য)octx;
  (শূন্য)name;
  failf(data, "OpenSSL engine not found");
  ঘূৰাই CURLE_SSL_ENGINE_NOTFOUND;
#endif
}

/* Sets engine as default for all SSL operations
 */
স্থির CURLcode ossl_set_engine_default(সংরচনা Curl_easy *data,
                                        সংরচনা ossl_ctx *octx)
{
#ifdef USE_OPENSSL_ENGINE
  যদি(octx->p.engine) {
    যদি(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(data, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    অন্যথা {
      failf(data, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      ঘূৰাই CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#অন্যথা
  (শূন্য)data;
  (শূন্য)octx;
#endif
  ঘূৰাই CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
স্থির সংরচনা curl_slist *ossl_engines_list(সংরচনা Curl_easy *data)
{
  সংরচনা curl_slist *list = রিক্ত;
#ifdef USE_OPENSSL_ENGINE
  সংরচনা curl_slist *beg;
  ENGINE *e;

  বাবে(e = ENGINE_get_first(); e; e = ENGINE_get_next(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    যদি(!beg) {
      curl_slist_free_all(list);
      ঘূৰাই রিক্ত;
    }
    list = beg;
  }
#endif
  (শূন্য)data;
  ঘূৰাই list;
}

#ifdef OPENSSL_HAS_PROVIDERS

স্থির শূন্য ossl_provider_cleanup(সংরচনা ossl_ctx *octx)
{
  যদি(!octx)
    ঘূৰাই;
  যদি(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = রিক্ত;
  }
  যদি(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = রিক্ত;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = রিক্ত;
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
স্থির CURLcode ossl_set_provider(সংরচনা Curl_easy *data,
                                  সংরচনা ossl_ctx *octx,
                                  ধ্রুৱ ৱর্ণ *iname)
{
  ৱর্ণ name[MAX_PROVIDER_LEN + 1];
  সংরচনা Curl_str prov;
  ধ্রুৱ ৱর্ণ *propq = রিক্ত;

  যদি(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    ঘূৰাই CURLE_OK;
  }
  যদি(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  memcpy(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  যদি(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    যদি(!libctx)
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    যদি(propq) {
      octx->p.propq = curlx_strdup(propq);
      যদি(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        ঘূৰাই CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#ifndef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  যদি(!OSSL_LIB_CTX_load_config(octx->p.libctx, রিক্ত)) {
    infof(data, "Failed to load default openssl config. Proceeding.");
  }
#endif

  যদি(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    ঘূৰাই CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  যদি(!octx->p.provider) {
    ৱর্ণ error_buffer[256];
    failf(data, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_buffer, পরিমাণ(error_buffer)));
    ossl_provider_cleanup(octx);
    ঘূৰাই CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  যদি(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(data, "Failed to load base");
    ঘূৰাই CURLE_SSL_ENGINE_NOTFOUND;
  }
  অন্যথা
    octx->p.provider_loaded = TRUE;
  ঘূৰাই CURLE_OK;
}
#অন্যথা /* OPENSSL_HAS_PROVIDERS */
#define ossl_provider_cleanup(x)
#endif

স্থির CURLcode ossl_shutdown(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ৱর্ণ buf[1024];
  পূর্ণাঙ্ক nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  যদি(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    গচ্ছ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  যদি(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    বাবে(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (পূর্ণাঙ্ক)পরিমাণ(buf));
      CURL_TRC_CF(data, cf, "SSL shutdown not sent, read -> %d", nread);
      যদি(nread <= 0)
        ভঙ্গ;
    }
    err = SSL_get_error(octx->ssl, nread);
    যদি(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool প্ৰবেশ_pending;
      /* Yes, it did. */
      যদি(!send_shutdown) {
        CURL_TRC_CF(data, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        গচ্ছ out;
      }
      অন্যথা যদি(!cf->next->cft->is_alive(cf->next, data, &প্ৰবেশ_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(data, cf, "peer closed connection");
        *done = TRUE;
        গচ্ছ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  যদি(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    পূর্ণাঙ্ক rc;
    ERR_clear_error();
    CURL_TRC_CF(data, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    যদি(rc == 1) {
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
      *done = TRUE;
      গচ্ছ out;
    }
    যদি(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      গচ্ছ out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  বাবে(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (পূর্ণাঙ্ক)পরিমাণ(buf));
    CURL_TRC_CF(data, cf, "SSL shutdown read -> %d", nread);
    যদি(nread <= 0)
      ভঙ্গ;
  }
  err = SSL_get_error(octx->ssl, nread);
  ৱিকল্প(err) {
  পক্ষ SSL_ERROR_ZERO_RETURN: /* no more data */
    যদি(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
    অন্যথা
      CURL_TRC_CF(data, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    ভঙ্গ;
  পক্ষ SSL_ERROR_NONE: /* did not get anything */
  পক্ষ SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(data, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ভঙ্গ;
  পক্ষ SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(data, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ভঙ্গ;
  স্ৱভাৱ:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(unsigned long sslerr = ERR_get_error());
      CURL_TRC_CF(data, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, পরিমাণ(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    result = CURLE_OK;
    ভঙ্গ;
  }

out:
  cf->shutdown = (result || *done);
  যদি(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->প্ৰবেশ_pending = FALSE;
  ঘূৰাই result;
}

/* also used by quic-tls */
শূন্য Curl_osslctx_cleanup(সংরচনা ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  যদি(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = রিক্ত;
  }
  যদি(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = রিক্ত;
    octx->x509_store_setup = FALSE;
  }
  যদি(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = রিক্ত;
  }
#ifdef USE_OPENSSL_ENGINE
  যদি(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = রিক্ত;
  }
#endif
  ossl_provider_cleanup(octx);
}

স্থির শূন্য ossl_close(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  (শূন্য)data;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
স্থির শূন্য ossl_close_all(সংরচনা Curl_easy *data)
{
  (শূন্য)data;
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
স্থির CURLcode ossl_verifyhost(সংরচনা Curl_easy *data,
                                সংরচনা connectdata *conn,
                                সংরচনা ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  পূর্ণাঙ্ক target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  স্তূপ_OF(GENERAL_NAME) *altnames;
#ifdef USE_IPV6
  সংরচনা in6_addr addr;
#অন্যথা
  সংরচনা in_addr addr;
#endif
  CURLcode result = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = strlen(peer->origin->hostname);

  (শূন্য)conn;
  ৱিকল্প(peer->type) {
  পক্ষ CURL_SSL_PEER_IPV4:
    যদি(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      ঘূৰাই CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = পরিমাণ(সংরচনা in_addr);
    ভঙ্গ;
#ifdef USE_IPV6
  পক্ষ CURL_SSL_PEER_IPV6:
    যদি(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      ঘূৰাই CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = পরিমাণ(সংরচনা in6_addr);
    ভঙ্গ;
#endif
  পক্ষ CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    ভঙ্গ;
  স্ৱভাৱ:
    DEBUGASSERT(0);
    failf(data, "unexpected SSL peer type: %d", (পূর্ণাঙ্ক)peer->type);
    ঘূৰাই CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, রিক্ত, রিক্ত);

  যদি(altnames) {
#ifdef HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#অন্যথা
    পূর্ণাঙ্ক numalts;
    পূর্ণাঙ্ক i;
#endif

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_অঙ্ক(altnames);

    /* loop through all alternatives - until a dnsmatch */
    বাবে(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ধ্রুৱ GENERAL_NAME *check = sk_GENERAL_NAME_মান(altnames, i);

      যদি(check->type == GEN_DNS)
        dNSName = TRUE;
      অন্যথা যদি(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      যদি(check->type == target) {
        /* get data and length */
        ধ্রুৱ ৱর্ণ *altptr = (ধ্রুৱ ৱর্ণ *)ASN1_STRING_get0_data(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_length(check->d.ia5);

        ৱিকল্প(target) {
        পক্ষ GEN_DNS: /* name/pattern comparison */
          যদি(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (পূর্ণাঙ্ক)altlen, altptr);
          }
          ভঙ্গ;

        পক্ষ GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          যদি((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          ভঙ্গ;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  যদি(matched)
    /* an alternative name matched */
    ;
  অন্যথা যদি(dNSName || iPAddress) {
    ধ্রুৱ ৱর্ণ *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(data, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(data, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    result = CURLE_PEER_FAILED_VERIFICATION;
  }
  অন্যথা {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    পূর্ণাঙ্ক i = -1;
    unsigned ৱর্ণ *cn = রিক্ত;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ধ্রুৱ X509_NAME *name = X509_get_subject_name(server_cert);
    যদি(name) {
      পূর্ণাঙ্ক j;
      যেতিয়ালৈ((j = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    যদি(i >= 0) {
      ধ্রুৱ ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      যদি(tmp) {
        যদি(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_length(tmp);
          cn = (unsigned ৱর্ণ *)CURL_UNCONST(ASN1_STRING_get0_data(tmp));
        }
        অন্যথা { /* not a UTF8 name */
          ধ্রুৱ পূর্ণাঙ্ক utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          যদি(utf8len < 0)
            cnlen = 0;
          অন্যথা
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        যদি((cnlen == 0) || !cn)
          result = CURLE_OUT_OF_MEMORY;
        অন্যথা যদি(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(data, "SSL: illegal cert name field");
          result = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    যদি(result)
      /* error already detected, pass through */
      ;
    অন্যথা যদি(!cn) {
      failf(data, "SSL: unable to obtain common name from peer certificate");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    অন্যথা যদি(!Curl_cert_hostcheck((ধ্রুৱ ৱর্ণ *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(data, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (পূর্ণাঙ্ক)cnlen, cn,
            peer->origin->user_hostname);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    অন্যথা {
      infof(data, " common name: %.*s (matched)", (পূর্ণাঙ্ক)cnlen, cn);
    }
    যদি(free_cn)
      OPENSSL_free(cn);
  }

  ঘূৰাই result;
}

#ifndef OPENSSL_NO_OCSP
স্থির CURLcode verifystatus(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             সংরচনা ossl_ctx *octx)
{
  পূর্ণাঙ্ক i, ocsp_status;
#ifdef HAVE_BORINGSSL_LIKE
  ধ্রুৱ uint8_t *status;
#অন্যথা
  unsigned ৱর্ণ *status;
#endif
  ধ্রুৱ unsigned ৱর্ণ *p;
  CURLcode result = CURLE_OK;
  OCSP_RESPONSE *rsp = রিক্ত;
  OCSP_BASICRESP *br = রিক্ত;
  X509_STORE     *st = রিক্ত;
  স্তূপ_OF(X509) *ch = রিক্ত;
  X509 *cert;
  OCSP_CERTID *id = রিক্ত;
  পূর্ণাঙ্ক cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  পূর্ণাঙ্ক ret;
  long len;

  (শূন্য)cf;
  DEBUGASSERT(octx);

  len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  যদি(!status) {
    failf(data, "No OCSP response received");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(রিক্ত, &p, len);
  যদি(!rsp) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }

  ocsp_status = OCSP_response_status(rsp);
  যদি(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(data, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }

  br = OCSP_response_get1_basic(rsp);
  যদি(!br) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }

#যদি !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_সংখ্যা) && LIBRESSL_VERSION_সংখ্যা < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#অন্যথা
  ch = SSL_get_peer_cert_chain(octx->ssl);
#endif
  যদি(!ch) {
    failf(data, "Could not get peer certificate chain");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  যদি(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(data, "OCSP response verification failed");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  যদি(!cert) {
    failf(data, "Error getting peer certificate");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }

  বাবে(i = 0; i < (পূর্ণাঙ্ক)sk_X509_অঙ্ক(ch); i++) {
    X509 *issuer = sk_X509_মান(ch, (ossl_valsize_t)i);
    যদি(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      ভঙ্গ;
    }
  }
  X509_free(cert);

  যদি(!id) {
    failf(data, "Error computing OCSP ID");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  যদি(ret != 1) {
    failf(data, "Could not find certificate ID in OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  যদি(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(data, "OCSP response has expired");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }

  infof(data, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  ৱিকল্প(cert_status) {
  পক্ষ V_OCSP_CERTSTATUS_GOOD:
    ভঙ্গ;

  পক্ষ V_OCSP_CERTSTATUS_REVOKED:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    failf(data, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    গচ্ছ end;

  পক্ষ V_OCSP_CERTSTATUS_UNKNOWN:
  স্ৱভাৱ:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ end;
  }

end:
  যদি(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  ঘূৰাই result;
}
#endif

স্থির ধ্রুৱ ৱর্ণ *ssl_বাৰ্তা_type(পূর্ণাঙ্ক ssl_ver, পূর্ণাঙ্ক বাৰ্তা)
{
  যদি(ssl_ver == SSL3_VERSION_MAJOR) {
    ৱিকল্প(বাৰ্তা) {
    পক্ষ SSL3_MT_HELLO_REQUEST:
      ঘূৰাই "Hello request";
    পক্ষ SSL3_MT_CLIENT_HELLO:
      ঘূৰাই "Client hello";
    পক্ষ SSL3_MT_SERVER_HELLO:
      ঘূৰাই "Server hello";
#ifdef SSL3_MT_NEWSESSION_TICKET
    পক্ষ SSL3_MT_NEWSESSION_TICKET:
      ঘূৰাই "Newsession Ticket";
#endif
    পক্ষ SSL3_MT_CERTIFICATE:
      ঘূৰাই "Certificate";
    পক্ষ SSL3_MT_SERVER_চাবি_EXCHANGE:
      ঘূৰাই "Server key exchange";
    পক্ষ SSL3_MT_CLIENT_চাবি_EXCHANGE:
      ঘূৰাই "Client key exchange";
    পক্ষ SSL3_MT_CERTIFICATE_REQUEST:
      ঘূৰাই "Request CERT";
    পক্ষ SSL3_MT_SERVER_DONE:
      ঘূৰাই "Server finished";
    পক্ষ SSL3_MT_CERTIFICATE_VERIFY:
      ঘূৰাই "CERT verify";
    পক্ষ SSL3_MT_FINISHED:
      ঘূৰাই "Finished";
#ifdef SSL3_MT_CERTIFICATE_STATUS
    পক্ষ SSL3_MT_CERTIFICATE_STATUS:
      ঘূৰাই "Certificate Status";
#endif
#ifdef SSL3_MT_ENCRYPTED_EXTENSIONS
    পক্ষ SSL3_MT_ENCRYPTED_EXTENSIONS:
      ঘূৰাই "Encrypted Extensions";
#endif
#ifdef SSL3_MT_SUPPLEMENTAL_DATA
    পক্ষ SSL3_MT_SUPPLEMENTAL_DATA:
      ঘূৰাই "Supplemental data";
#endif
#ifdef SSL3_MT_END_OF_EARLY_DATA
    পক্ষ SSL3_MT_END_OF_EARLY_DATA:
      ঘূৰাই "End of early data";
#endif
#ifdef SSL3_MT_চাবি_UPDATE
    পক্ষ SSL3_MT_চাবি_UPDATE:
      ঘূৰাই "Key update";
#endif
#ifdef SSL3_MT_NEXT_PROTO
    পক্ষ SSL3_MT_NEXT_PROTO:
      ঘূৰাই "Next protocol";
#endif
#ifdef SSL3_MT_MESSAGE_HASH
    পক্ষ SSL3_MT_MESSAGE_HASH:
      ঘূৰাই "Message hash";
#endif
    }
  }
  ঘূৰাই "Unknown";
}

স্থির ধ্রুৱ ৱর্ণ *tls_rt_type(পূর্ণাঙ্ক type)
{
  ৱিকল্প(type) {
#ifdef SSL3_RT_HEADER
  পক্ষ SSL3_RT_HEADER:
    ঘূৰাই "TLS header";
#endif
  পক্ষ SSL3_RT_CHANGE_CIPHER_SPEC:
    ঘূৰাই "TLS change cipher";
  পক্ষ SSL3_RT_ALERT:
    ঘূৰাই "TLS alert";
  পক্ষ SSL3_RT_HANDSHAKE:
    ঘূৰাই "TLS handshake";
  পক্ষ SSL3_RT_APPLICATION_DATA:
    ঘূৰাই "TLS app data";
  স্ৱভাৱ:
    ঘূৰাই "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
স্থির শূন্য ossl_trace(পূর্ণাঙ্ক direction, পূর্ণাঙ্ক ssl_ver, পূর্ণাঙ্ক content_type,
                       ধ্রুৱ শূন্য *buf, size_t len, SSL *ssl,
                       শূন্য *userp)
{
  ধ্রুৱ ৱর্ণ *verstr;
  সংরচনা Curl_cfilter *cf = userp;
  সংরচনা Curl_easy *data = রিক্ত;
  ৱর্ণ unknown[32];

  যদি(!cf)
    ঘূৰাই;
  data = CF_DATA_CURRENT(cf);
  যদি(!data || !data->set.fdebug || (direction && direction != 1))
    ঘূৰাই;

  ৱিকল্প(ssl_ver) {
#ifdef SSL3_VERSION
  পক্ষ SSL3_VERSION:
    verstr = "SSLv3";
    ভঙ্গ;
#endif
  পক্ষ TLS1_VERSION:
    verstr = "TLSv1.0";
    ভঙ্গ;
#ifdef TLS1_1_VERSION
  পক্ষ TLS1_1_VERSION:
    verstr = "TLSv1.1";
    ভঙ্গ;
#endif
#ifdef TLS1_2_VERSION
  পক্ষ TLS1_2_VERSION:
    verstr = "TLSv1.2";
    ভঙ্গ;
#endif
  পক্ষ TLS1_3_VERSION:
    verstr = "TLSv1.3";
    ভঙ্গ;
  স্ৱভাৱ:
    curl_msnprintf(unknown, পরিমাণ(unknown), "(%x)", (unsigned পূর্ণাঙ্ক)ssl_ver);
    verstr = unknown;
    ভঙ্গ;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  যদি(ssl_ver
#ifdef SSL3_RT_HEADER
     && content_type != SSL3_RT_HEADER
#endif
#ifdef SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#endif
    ) {
    ধ্রুৱ ৱর্ণ *বাৰ্তা_name = "Truncated message";
    ধ্রুৱ ৱর্ণ *tls_rt_name;
    ৱর্ণ ssl_buf[1024];
    পূর্ণাঙ্ক বাৰ্তা_type = 0;
    পূর্ণাঙ্ক txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    যদি(ssl_ver == SSL3_VERSION_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    অন্যথা
      tls_rt_name = "";

    যদি(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      যদি(len) {
        বাৰ্তা_type = *(ধ্রুৱ unsigned ৱর্ণ *)buf;
        বাৰ্তা_name = "Change cipher spec";
      }
    }
    অন্যথা যদি(content_type == SSL3_RT_ALERT) {
      যদি(len >= 2) {
        বাৰ্তা_type =
          (((ধ্রুৱ unsigned ৱর্ণ *)buf)[0] << 8) +
           ((ধ্রুৱ unsigned ৱর্ণ *)buf)[1];
        বাৰ্তা_name = SSL_alert_desc_string_long(বাৰ্তা_type);
      }
    }
    অন্যথা যদি(len) {
      বাৰ্তা_type = *(ধ্রুৱ unsigned ৱর্ণ *)buf;
      বাৰ্তা_name = ssl_বাৰ্তা_type(ssl_ver, বাৰ্তা_type);
    }

    txt_len = curl_msnprintf(ssl_buf, পরিমাণ(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, বাৰ্তা_name, বাৰ্তা_type);
    Curl_debug(data, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(data, (direction == 1) ? CURLINFO_SSL_DATA_OUT :
             CURLINFO_SSL_DATA_IN, (ধ্রুৱ ৱর্ণ *)buf, len);
  (শূন্য)ssl;
}

স্থির CURLcode ossl_set_ssl_version_min_max(সংরচনা Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             unsigned পূর্ণাঙ্ক ssl_version_min)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  long curl_ssl_version_min = (long)ssl_version_min;
  long curl_ssl_version_max;

  /* convert curl min SSL version option to OpenSSL constant */
#যদি defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_VERSION_সংখ্যা)
  uint16_t ossl_ssl_version_min = 0;
  uint16_t ossl_ssl_version_max = 0;
#অন্যথা
  long ossl_ssl_version_min = 0;
  long ossl_ssl_version_max = 0;
#endif
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_version_min != CURL_SSLVERSION_DEFAULT);
  ৱিকল্প(curl_ssl_version_min) {
  পক্ষ CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  পক্ষ CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    ভঙ্গ;
  পক্ষ CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    ভঙ্গ;
  পক্ষ CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    ভঙ্গ;
  পক্ষ CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    ভঙ্গ;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (long)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  ৱিকল্প(curl_ssl_version_max) {
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    ভঙ্গ;
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    ভঙ্গ;
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    ভঙ্গ;
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    ভঙ্গ;
  পক্ষ CURL_SSLVERSION_MAX_NONE:  /* none selected */
  পক্ষ CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  স্ৱভাৱ:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    ভঙ্গ;
  }

  যদি(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;

  ঘূৰাই CURLE_OK;
}

CURLcode Curl_ossl_add_session(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা ossl_ctx *octx,
                               ধ্রুৱ ৱর্ণ *ssl_peer_চাবি,
                               SSL_SESSION *session,
                               ধ্রুৱ ৱর্ণ *alpn,
                               unsigned ৱর্ণ *quic_tp,
                               size_t quic_tp_len,
                               সংরচনা Curl_ssl_session **psession)
{
  সংরচনা Curl_ssl_session *sc_session = রিক্ত, *sc_dup = রিক্ত;
  unsigned ৱর্ণ *der_session_buf = রিক্ত;
  unsigned ৱর্ণ *qtp_clone = রিক্ত;
  CURLcode result = CURLE_OK;

  যদি(psession)
    *psession = রিক্ত;
  যদি(!cf || !data)
    গচ্ছ out;

  যদি(Curl_ssl_scache_use(cf, data)) {
    size_t der_session_size;
    unsigned ৱর্ণ *der_session_ptr;
    size_t earlydata_max = 0;
    পূর্ণাঙ্ক ietf_tls_id = SSL_version(octx->ssl);

    der_session_size = i2d_SSL_SESSION(session, রিক্ত);
    যদি(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_size);
    যদি(!der_session_buf) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }

    der_session_size = i2d_SSL_SESSION(session, &der_session_ptr);
    যদি(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }

#ifdef HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_data(session);
#endif
    যদি(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ধ্রুৱ ৱর্ণ *)quic_tp, quic_tp_len);
      যদি(!qtp_clone) {
        result = CURLE_OUT_OF_MEMORY;
        গচ্ছ out;
      }
    }

    result = Curl_ssl_session_create2(der_session_buf, der_session_size,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(রিক্ত) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = রিক্ত;  /* took ownership of sdata */
#ifdef USE_APPLE_SECTRUST
    যদি(!result)
      sc_session->sectrust_verified = octx->sectrust_verified;
#endif
    যদি(!result && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        result = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    যদি(!result) {
      result = Curl_ssl_scache_put(cf, data, ssl_peer_চাবি, sc_session);
      /* took ownership of `sc_session` */
      sc_session = রিক্ত;
    }
  }

out:
  curlx_free(der_session_buf);
  যদি(!result && psession) {
    *psession = sc_dup;
    sc_dup = রিক্ত;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ঘূৰাই result;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
স্থির পূর্ণাঙ্ক ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  সংরচনা Curl_cfilter *cf = (সংরচনা Curl_cfilter *)SSL_get_app_data(ssl);
  যদি(cf) {
    সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
    সংরচনা ssl_connect_data *connssl = cf->ctx;
    সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
    সংরচনা Curl_ssl_session *session = রিক্ত;
    Curl_ossl_add_session(cf, data, octx, connssl->peer.scache_চাবি,
                          ssl_sessionid, connssl->negotiated.alpn, রিক্ত,
                          0, &session);
    যদি(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  ঘূৰাই 0;
}

স্থির CURLcode load_cacert_from_memory(X509_STORE *store,
                                        ধ্রুৱ সংরচনা curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = রিক্ত;
  স্তূপ_OF(X509_INFO) *inf = রিক্ত;

  /* everything else is a reference */
  পূর্ণাঙ্ক i, গুনতি = 0;
  X509_INFO *itmp = রিক্ত;

  যদি(ca_info_blob->len > (size_t)INT_MAX)
    ঘূৰাই CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->data, (পূর্ণাঙ্ক)ca_info_blob->len);
  যদি(!cbio)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, রিক্ত, রিক্ত, রিক্ত);
  যদি(!inf) {
    BIO_free(cbio);
    ঘূৰাই CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  বাবে(i = 0; i < (পূর্ণাঙ্ক)sk_X509_INFO_অঙ্ক(inf); ++i) {
    itmp = sk_X509_INFO_মান(inf, (ossl_valsize_t)i);
    যদি(itmp->x509) {
      যদি(X509_STORE_add_cert(store, itmp->x509)) {
        ++গুনতি;
      }
      অন্যথা {
        /* set count to 0 to return an error */
        গুনতি = 0;
        ভঙ্গ;
      }
    }
    যদি(itmp->crl) {
      যদি(X509_STORE_add_crl(store, itmp->crl)) {
        ++গুনতি;
      }
      অন্যথা {
        /* set count to 0 to return an error */
        গুনতি = 0;
        ভঙ্গ;
      }
    }
  }

#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_INFO_বেৰ_কৰো_free(inf, X509_INFO_free);
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বেৰ_কৰো
#endif
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  ঘূৰাই (গুনতি > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ifdef USE_WIN32_CRYPTO
স্থির CURLcode ossl_win_load_store(সংরচনা Curl_easy *data,
                                    সংরচনা Curl_cfilter *cf,
                                    ধ্রুৱ ৱর্ণ *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode result = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  যদি(hStore) {
    PCCERT_CONTEXT pContext = রিক্ত;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = রিক্ত;
    DWORD enhkey_usage_size = 0;
    VERBOSE(size_t total = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    result = CURLE_OK;
    বাবে(;;) {
      X509 *x509;
      FILETIME now;
      BYTE চাবি_usage[2];
      DWORD req_size;
      ধ্রুৱ unsigned ৱর্ণ *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      যদি(!pContext)
        ভঙ্গ;

      VERBOSE(++total);

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        ৱর্ণ cert_name[256];
        যদি(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               রিক্ত, cert_name, পরিমাণ(cert_name)))
          infof(data, "SSL: unknown cert name");
        অন্যথা
          infof(data, "SSL: Checking cert \"%s\"", cert_name);
      }
#endif
      encoded_cert = (ধ্রুৱ unsigned ৱর্ণ *)pContext->pbCertEncoded;
      যদি(!encoded_cert)
        অনুৱর্ত;

      GetSystemTimeAsFileTime(&now);
      যদি(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        অনুৱর্ত;

      /* If key usage exists check for signing attribute */
      যদি(Cert_Get_Intended_চাবি_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 চাবি_usage, পরিমাণ(চাবি_usage))) {
        যদি(!(চাবি_usage[0] & CERT_চাবি_CERT_SIGN_চাবি_USAGE))
          অনুৱর্ত;
      }
      অন্যথা যদি(GetLastError())
        অনুৱর্ত;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      যদি(Cert_Get_Enhanced_চাবি_Usage(pContext, 0, রিক্ত, &req_size) && req_size) {
        যদি(req_size > enhkey_usage_size) {
          শূন্য *tmp = curlx_realloc(enhkey_usage, req_size);

          যদি(!tmp) {
            failf(data, "SSL: Out of memory allocating for OID list");
            result = CURLE_OUT_OF_MEMORY;
            ভঙ্গ;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_size = req_size;
        }

        যদি(Cert_Get_Enhanced_চাবি_Usage(pContext, 0, enhkey_usage, &req_size)) {
          যদি(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            যদি((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              অনুৱর্ত;
          }
          অন্যথা {
            DWORD i;
            bool found = FALSE;

            বাবে(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              যদি(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                ভঙ্গ;
              }
            }

            যদি(!found)
              অনুৱর্ত;
          }
        }
        অন্যথা
          অনুৱর্ত;
      }
      অন্যথা
        অনুৱর্ত;

      x509 = d2i_X509(রিক্ত, &encoded_cert, (long)pContext->cbCertEncoded);
      যদি(!x509)
        অনুৱর্ত;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      যদি(X509_STORE_add_cert(store, x509) == 1) {
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

    যদি(result)
      ঘূৰাই result;
  }

  ঘূৰাই result;
}

স্থির CURLcode ossl_windows_load_anchors(সংরচনা Curl_cfilter *cf,
                                          সংরচনা Curl_easy *data,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  স্থির ধ্রুৱ ৱর্ণ * ধ্রুৱ win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode result = CURLE_OK;

  *padded = FALSE;
  বাবে(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    result = ossl_win_load_store(data, cf, win_stores[i], store, &store_added);
    যদি(result)
      ঘূৰাই result;
    যদি(store_added) {
      CURL_TRC_CF(data, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    অন্যথা
      infof(data, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  ঘূৰাই result;
}

#endif /* USE_WIN32_CRYPTO */

স্থির CURLcode ossl_load_trust_anchors(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        সংরচনা ossl_ctx *octx,
                                        X509_STORE *store)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ * ধ্রুৱ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? রিক্ত : conn_config->CAfile);
  ধ্রুৱ ৱর্ণ * ধ্রুৱ ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  যদি(conn_config->native_ca_store) {
#ifdef USE_WIN32_CRYPTO
    bool added = FALSE;
    result = ossl_windows_load_anchors(cf, data, store, &added);
    যদি(result)
      ঘূৰাই result;
    যদি(added) {
      infof(data, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#নোচেদ্যদি defined(USE_APPLE_SECTRUST)
    infof(data, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#endif
  }

  যদি(conn_config->ca_info_blob) {
    result = load_cacert_from_memory(store, conn_config->ca_info_blob);
    যদি(result) {
      failf(data, "error adding trust anchors from certificate blob: %d",
            (পূর্ণাঙ্ক)result);
      ঘূৰাই result;
    }
    infof(data, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  যদি(ssl_cafile || ssl_capath) {
#ifdef HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    যদি(ssl_cafile) {
      যদি(!X509_STORE_load_file(store, ssl_cafile)) {
        যদি(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from file: %s", ssl_cafile);
          ঘূৰাই CURLE_SSL_CACERT_BADFILE;
        }
        অন্যথা
          infof(data, "error setting certificate file, continuing anyway");
      }
      infof(data, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    যদি(ssl_capath) {
      যদি(!X509_STORE_load_path(store, ssl_capath)) {
        যদি(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from path: %s", ssl_capath);
          ঘূৰাই CURLE_SSL_CACERT_BADFILE;
        }
        অন্যথা
          infof(data, "error setting certificate path, continuing anyway");
      }
      infof(data, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#অন্যথা
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    যদি(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      যদি(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(data, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        ঘূৰাই CURLE_SSL_CACERT_BADFILE;
      }
      অন্যথা {
        infof(data, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    যদি(ssl_cafile)
      infof(data, "  CAfile: %s", ssl_cafile);
    যদি(ssl_capath)
      infof(data, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#endif
  }

#ifdef CURL_CA_FALLBACK
  যদি(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(data, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#endif
  যদি(octx->store_is_empty && !have_native_check)
    infof(data, "  no trust anchors configured");

  ঘূৰাই result;
}

স্থির CURLcode ossl_populate_x509_store(সংরচনা Curl_cfilter *cf,
                                         সংরচনা Curl_easy *data,
                                         সংরচনা ossl_ctx *octx,
                                         X509_STORE *store)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  X509_LOOKUP *lookup = রিক্ত;
  ধ্রুৱ ৱর্ণ * ধ্রুৱ ssl_crlfile = conn_config->CRLfile;
  unsigned long x509flags = 0;

  CURL_TRC_CF(data, cf, "configuring OpenSSL's x509 trust store");
  যদি(!store)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  যদি(!conn_config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    ঘূৰাই CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  result = ossl_load_trust_anchors(cf, data, octx, store);
  যদি(result)
    ঘূৰাই result;

  /* Does not make sense to load a CRL file without peer verification */
#ifdef USE_APPLE_SECTRUST
  যদি(ssl_crlfile && conn_config->native_ca_store) {
    failf(data, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ঘূৰাই CURLE_NOT_BUILT_IN;
  }
#endif
  যদি(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    যদি(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(data, "error loading CRL file: %s", ssl_crlfile);
      ঘূৰাই CURLE_SSL_CRL_BADFILE;
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

  যদি(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (শূন্য)X509_STORE_set_flags(store, x509flags);

  ঘূৰাই result;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_OSSL_X509_চাবি  "tls:ossl:x509:share"

সংরচনা ossl_x509_share {
  ৱর্ণ *CAfile;         /* CAfile path used to generate X509 store */
  ৱর্ণ *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  সংরচনা curltime time; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

স্থির শূন্য oss_x509_share_free(ধ্রুৱ শূন্য *চাবি, size_t চাবি_len, শূন্য *p)
{
  সংরচনা ossl_x509_share *share = p;
  DEBUGASSERT(চাবি_len == CURL_CSTRLEN(MPROTO_OSSL_X509_চাবি));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_চাবি, চাবি, চাবি_len));
  (শূন্য)চাবি;
  (শূন্য)চাবি_len;
  যদি(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

স্থির bool ossl_cached_x509_store_expired(সংরচনা Curl_easy *data,
                                           ধ্রুৱ সংরচনা ossl_x509_share *mb)
{
  যদি(data->set.ssl_ca_cache_timeout < 0)
    ঘূৰাই FALSE;
  অন্যথা {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &mb->time);
    timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

    ঘূৰাই elapsed_ms >= timeout_ms;
  }
}

স্থির bool ossl_cached_x509_store_different(সংরচনা Curl_cfilter *cf,
                                             ধ্রুৱ সংরচনা Curl_easy *data,
                                             ধ্রুৱ সংরচনা ossl_x509_share *mb)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (শূন্য)data;
  যদি(mb->no_partialchain != conn_config->no_partialchain)
    ঘূৰাই TRUE;
  যদি(!mb->CAfile || !conn_config->CAfile) {
    যদি(mb->CAfile != conn_config->CAfile)
      ঘূৰাই TRUE;
  }
  অন্যথা যদি(strcmp(mb->CAfile, conn_config->CAfile))
    ঘূৰাই TRUE;

  যদি(!mb->CApath || !conn_config->CApath) {
    যদি(mb->CApath != conn_config->CApath)
      ঘূৰাই TRUE;
  }
  অন্যথা যদি(strcmp(mb->CApath, conn_config->CApath))
    ঘূৰাই TRUE;

  ঘূৰাই FALSE;
}

স্থির X509_STORE *ossl_get_cached_x509_store(সংরচনা Curl_cfilter *cf,
                                              সংরচনা Curl_easy *data,
                                              bool *pempty)
{
  সংরচনা Curl_multi *multi = data->multi;
  সংরচনা ossl_x509_share *share;
  X509_STORE *store = রিক্ত;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_চাবি,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_চাবি)) : রিক্ত;
  যদি(share && share->store &&
     !ossl_cached_x509_store_expired(data, share) &&
     !ossl_cached_x509_store_different(cf, data, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  ঘূৰাই store;
}

স্থির শূন্য ossl_set_cached_x509_store(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       X509_STORE *store,
                                       bool is_empty)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা Curl_multi *multi = data->multi;
  সংরচনা ossl_x509_share *share;

  DEBUGASSERT(multi);
  যদি(!multi)
    ঘূৰাই;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_চাবি,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_চাবি));

  যদি(!share) {
    share = curlx_calloc(1, পরিমাণ(*share));
    যদি(!share)
      ঘূৰাই;
    যদি(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_চাবি,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_চাবি),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      ঘূৰাই;
    }
  }

  যদি(X509_STORE_up_ref(store)) {
    ৱর্ণ *CAfile = রিক্ত;
    ৱর্ণ *CApath = রিক্ত;

    যদি(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      যদি(!CAfile) {
        X509_STORE_free(store);
        ঘূৰাই;
      }
    }

    যদি(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      যদি(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        ঘূৰাই;
      }
    }

    যদি(share->store) {
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
#অন্যথা
#define USING_PROVIDER(x) 0
#endif

CURLcode Curl_ssl_setup_x509_store(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা ossl_ctx *octx)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
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
  যদি(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  অন্যথা {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    result = ossl_populate_x509_store(cf, data, octx, store);
    যদি(result == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, data, store, (bool)octx->store_is_empty);
    }
  }

  ERR_বেৰ_কৰো_to_mark();

  ঘূৰাই result;
}

স্থির bool ossl_apply_session(
  সংরচনা ossl_ctx *octx,
  সংরচনা Curl_cfilter *cf,
  সংরচনা Curl_easy *data,
  সংরচনা alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  সংরচনা Curl_ssl_session *scs)
{
  সংরচনা ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ধ্রুৱ unsigned ৱর্ণ *der_sessionid = scs->sdata;
  size_t der_sessionid_size = scs->sdata_len;
  SSL_SESSION *ssl_session = রিক্ত;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(রিক্ত, &der_sessionid,
                                (long)der_sessionid_size);
  যদি(ssl_session) {
    যদি(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(ৱর্ণ error_buffer[256]);
      infof(data, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          পরিমাণ(error_buffer)));
    }
    অন্যথা {
      যদি(conn_cfg->verifypeer &&
         (SSL_get_verify_result(octx->ssl) != X509_V_OK)
#ifdef USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#endif
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, রিক্ত);
        infof(data, "SSL session not peer verified, not reusing");
      }
      অন্যথা {
        infof(data, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ifdef USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#endif
        infof(data, "SSL verify result: %lx",
              (unsigned long)SSL_get_verify_result(octx->ssl));
#ifdef HAVE_OPENSSL_EARLYDATA
        যদি(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_data(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_data = FALSE;
          যদি(sess_reuse_cb)
            (শূন্য)sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          যদি(do_early_data) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_to(alpns, scs->alpn);
          }
        }
#অন্যথা
        (শূন্য)alpns;
        (শূন্য)conn_cfg;
        (শূন্য)sess_reuse_cb;
#endif
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  অন্যথা {
    infof(data, "SSL session not accepted by OpenSSL, continuing without");
  }
  ঘূৰাই (bool)octx->reused_session;
}

স্থির CURLcode ossl_init_session_and_alpns(
  সংরচনা ossl_ctx *octx,
  সংরচনা Curl_cfilter *cf,
  সংরচনা Curl_easy *data,
  সংরচনা ssl_peer *peer,
  ধ্রুৱ সংরচনা alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  সংরচনা ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা alpn_spec alpns;
  CURLcode result;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  যদি((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    সংরচনা Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    যদি(scs) {
      যদি(ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
    }
  }

  যদি(!octx->reused_session &&
     Curl_ssl_scache_use(cf, data) && !conn_cfg->verifystatus) {
    সংরচনা Curl_ssl_session *scs = রিক্ত;

    result = Curl_ssl_scache_take(cf, data, peer->scache_চাবি, &scs);
    যদি(!result && scs && scs->sdata && scs->sdata_len) {
      (শূন্য)ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, data, peer->scache_চাবি, scs);
  }

  যদি(alpns.গুনতি) {
    সংরচনা alpn_proto_buf proto;
    memset(&proto, 0, পরিমাণ(proto));
    result = Curl_alpn_to_proto_buf(&proto, &alpns);
    যদি(result) {
      failf(data, "Error determining ALPN");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
    যদি(SSL_set_alpn_protos(octx->ssl, proto.data, proto.len)) {
      failf(data, "Error setting ALPN");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
  }

  ঘূৰাই CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
স্থির CURLcode ossl_init_ech(সংরচনা ossl_ctx *octx,
                              সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              সংরচনা ssl_peer *peer)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  পূর্ণাঙ্ক trying_ech_now = 0;

  যদি(!Curl_ssl_ech_enabled(cf))
    ঘূৰাই CURLE_OK;

  যদি(conn_config->ech == CURLECH_GREASE) {
    infof(data, "ECH: will GREASE ClientHello");
#ifdef HAVE_BORINGSSL_LIKE
    SSL_set_enable_ech_grease(octx->ssl, 1);
#অন্যথা
    SSL_set_options(octx->ssl, SSL_OP_ECH_GREASE);
#endif
  }
  অন্যথা যদি(conn_config->ech && conn_config->ech_config) {
#ifdef HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode result;

    যদি(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * strlen(conn_config->ech_config);
    result = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    যদি(result || !ech_config) {
      infof(data, "ECH: cannot base64 decode ECHConfig from command line");
      যদি(conn_config->ech == CURLECH_HARD)
        ঘূৰাই result;
    }
    যদি(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      যদি(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        ঘূৰাই CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#অন্যথা
    size_t ech_config_len = 0;
    যদি(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = strlen(conn_config->ech_config);
    যদি(SSL_set1_ech_config_list(octx->ssl,
                                (ধ্রুৱ uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      যদি(conn_config->ech == CURLECH_HARD)
        ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
    অন্যথা
      trying_ech_now = 1;
#endif /* HAVE_BORINGSSL_LIKE */
    infof(data, "ECH: ECHConfig from command line");
  }
  অন্যথা {
    ধ্রুৱ সংরচনা Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, peer->origin);

    যদি(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ধ্রুৱ unsigned ৱর্ণ *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(data, "ECH: ECHConfig from HTTPS RR");
      যদি(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(data, "ECH: SSL_set1_ech_config_list failed");
        যদি(conn_config->ech == CURLECH_HARD)
          ঘূৰাই CURLE_SSL_CONNECT_ERROR;
      }
      অন্যথা {
        trying_ech_now = 1;
        infof(data, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    অন্যথা {
      infof(data, "ECH: requested but no ECHConfig available");
      যদি(conn_config->ech == CURLECH_HARD)
        ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
  }
#ifdef HAVE_BORINGSSL_LIKE
  (শূন্য)peer;
  যদি(trying_ech_now && conn_config->ech_public) {
    infof(data, "ECH: setting public_name not supported with BoringSSL");
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }
#অন্যথা
  যদি(trying_ech_now && conn_config->ech_public) {
    পূর্ণাঙ্ক ret;
    infof(data, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    যদি(ret != 1) {
      infof(data, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
  }
#endif /* HAVE_BORINGSSL_LIKE */
  যদি(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(data, "ECH: cannot force TLSv1.3 [ERROR]");
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }

  ঘূৰাই CURLE_OK;
}
#endif /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

স্থির CURLcode ossl_init_ssl(সংরচনা ossl_ctx *octx,
                              সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              সংরচনা ssl_peer *peer,
                              ধ্রুৱ সংরচনা alpn_spec *alpns_requested,
                              শূন্য *ssl_user_data,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  যদি(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  যদি(!octx->ssl) {
    failf(data, "SSL: could not create a context (handle)");
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_data(octx->ssl, ssl_user_data);

#ifndef OPENSSL_NO_OCSP
  যদি(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#endif

  SSL_set_connect_state(octx->ssl);

  যদি(peer->sni) {
    যদি(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(data, "Failed set SNI");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
  }

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode result = ossl_init_ech(octx, cf, data, peer);
    যদি(result)
      ঘূৰাই result;
  }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  ঘূৰাই ossl_init_session_and_alpns(octx, cf, data, peer,
                                     alpns_requested, sess_reuse_cb);
}

স্থির CURLcode ossl_init_method(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা ssl_peer *peer,
                                 ধ্রুৱ SSL_METHOD **pmethod,
                                 unsigned পূর্ণাঙ্ক *pssl_version_min)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = রিক্ত;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  ৱিকল্প(peer->transport) {
  পক্ষ TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    ৱিকল্প(*pssl_version_min) {
    পক্ষ CURL_SSLVERSION_TLSv1:
    পক্ষ CURL_SSLVERSION_TLSv1_0:
    পক্ষ CURL_SSLVERSION_TLSv1_1:
    পক্ষ CURL_SSLVERSION_TLSv1_2:
    পক্ষ CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      ভঙ্গ;
    পক্ষ CURL_SSLVERSION_SSLv2:
      failf(data, "No SSLv2 support");
      ঘূৰাই CURLE_NOT_BUILT_IN;
    পক্ষ CURL_SSLVERSION_SSLv3:
      failf(data, "No SSLv3 support");
      ঘূৰাই CURLE_NOT_BUILT_IN;
    স্ৱভাৱ:
      failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
    ভঙ্গ;
  পক্ষ TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    যদি(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(data, "QUIC needs at least TLS version 1.3");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    ভঙ্গ;
  স্ৱভাৱ:
    failf(data, "unsupported transport %d in SSL init", peer->transport);
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }

  ঘূৰাই *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(সংরচনা ossl_ctx *octx,
                            সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            সংরচনা ssl_peer *peer,
                            ধ্রুৱ সংরচনা alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            শূন্য *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            শূন্য *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *ciphers;
  ধ্রুৱ SSL_METHOD *req_method = রিক্ত;
  ctx_option_t ctx_options = 0;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  ৱর্ণ * ধ্রুৱ ssl_cert = conn_config->clientcert;
  ধ্রুৱ সংরচনা curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ধ্রুৱ ৱর্ণ * ধ্রুৱ ssl_cert_type = conn_config->cert_type;
  unsigned পূর্ণাঙ্ক ssl_version_min = CURL_SSLVERSION_DEFAULT;
  ৱর্ণ error_buffer[256];

  /* Make funny stuff to get random input */
  result = ossl_seed(data);
  যদি(!result) {
    যদি(CURL_EASY_STR(data, STRING_SSL_ENGINE))
      result = ossl_set_engine(data, octx,
                               CURL_EASY_STR(data, STRING_SSL_ENGINE));
    যদি(!result && data->set.engine_default)
      result = ossl_set_engine_default(data, octx);
  }

  যদি(!result) {
    ssl_config->certverifyresult = !X509_V_OK;
    result = ossl_init_method(cf, data, peer, &req_method, &ssl_version_min);
  }
  যদি(result)
    ঘূৰাই result;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ifdef OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#endif
    SSL_CTX_new(req_method);

  যদি(!octx->ssl_ctx) {
    failf(data, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_buffer, পরিমাণ(error_buffer)));
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  }

  যদি(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    যদি(result)
      ঘূৰাই result;
  }

  যদি(data->set.fdebug && data->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_বাৰ্তা_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_বাৰ্তা_callback_arg(octx->ssl_ctx, cf);
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
  যদি(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_ঢোকাও_EMPTY_FRAGMENTS;

  DEBUGASSERT(ssl_version_min != CURL_SSLVERSION_DEFAULT);
  ৱিকল্প(ssl_version_min) {
  পক্ষ CURL_SSLVERSION_SSLv2:
  পক্ষ CURL_SSLVERSION_SSLv3:
    ঘূৰাই CURLE_NOT_BUILT_IN;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  পক্ষ CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  পক্ষ CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  পক্ষ CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  পক্ষ CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  পক্ষ CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    result = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    যদি(result)
      ঘূৰাই result;
    ভঙ্গ;

  স্ৱভাৱ:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
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
  যদি(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = রিক্ত;
  যদি(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    যদি(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(data, "failed setting cipher list: %s", ciphers);
      ঘূৰাই CURLE_SSL_CIPHER;
    }
    infof(data, "Cipher selection: %s", ciphers);
  }

#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    ধ্রুৱ ৱর্ণ *ciphers13 = conn_config->cipher_list13;
    যদি(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      যদি(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(data, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        ঘূৰাই CURLE_SSL_CIPHER;
      }
      infof(data, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#endif

  যদি(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    result = client_cert(data, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->চাবি, conn_config->চাবি_blob,
                         conn_config->চাবি_type,
                         conn_config->চাবি_passwd);
    যদি(result)
      /* failf() is already done in client_cert() */
      ঘূৰাই result;
  }

#ifdef HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#endif

  {
    ধ্রুৱ ৱর্ণ *curves = conn_config->curves;
    যদি(curves) {
#ifdef HAVE_BORINGSSL_LIKE
#define OSSL_CURVE_CAST(x) (x)
#অন্যথা
#define OSSL_CURVE_CAST(x) (ৱর্ণ *)CURL_UNCONST(x)
#endif
      যদি(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(data, "failed setting curves list: '%s'", curves);
        ঘূৰাই CURLE_SSL_CIPHER;
      }
    }
  }

#ifdef HAVE_SSL_CTX_SET1_SIGALGS
#define OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    ধ্রুৱ ৱর্ণ *signature_algorithms = conn_config->signature_algorithms;
    যদি(signature_algorithms) {
      যদি(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(data, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        ঘূৰাই CURLE_SSL_CIPHER;
      }
    }
  }
#endif

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, রিক্ত);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#যদি !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  যদি(Curl_tls_keylog_enabled()) {
    SSL_CTX_set_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#endif

  যদি(cb_new_session) {
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
  যদি(data->set.ssl_fsslctx) {
    সংরচনা Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    যদি(!octx->x509_store_setup) {
      result = Curl_ssl_setup_x509_store(cf, data, octx);
      যদি(result)
        ঘূৰাই result;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, data, easy_fsslctx);
    result = (*data->set.ssl_fsslctx)(data, octx->ssl_ctx,
                                      data->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    যদি(result) {
      failf(data, "error signaled by SSL ctx callback");
      ঘূৰাই result;
    }
  }

  ঘূৰাই ossl_init_ssl(octx, cf, data, peer, alpns_requested,
                       ssl_user_data, sess_reuse_cb);
}

স্থির CURLcode ossl_on_session_reuse(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      সংরচনা alpn_spec *alpns,
                                      সংরচনা Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  ঘূৰাই Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max);
}

শূন্য Curl_ossl_report_handshake(সংরচনা Curl_easy *data, সংরচনা ossl_ctx *octx)
{
#ifdef CURLVERBOSE
  যদি(Curl_trc_is_verbose(data)) {
    পূর্ণাঙ্ক psigtype_nid = NID_undef;
    ধ্রুৱ ৱর্ণ *negotiated_group_name = রিক্ত;

#ifdef HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#যদি OPENSSL_VERSION_সংখ্যা >= 0x30200000L
    negotiated_group_name = SSL_get0_group_name(octx->ssl);
#অন্যথা
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
#অন্যথা
  (শূন্য)data;
  (শূন্য)octx;
#endif /* CURLVERBOSE */
}

স্থির CURLcode ossl_connect_step1(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  result = Curl_ossl_ctx_init(octx, cf, data, &connssl->peer,
                              connssl->alpn, রিক্ত, রিক্ত,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  যদি(result)
    ঘূৰাই result;

  octx->bio_method = ossl_bio_cf_method_create();
  যদি(!octx->bio_method)
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  যদি(!bio)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

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
#অন্যথা
  SSL_set_bio(octx->ssl, bio, bio);
#endif

  যদি(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    সংরচনা alpn_proto_buf proto;
    memset(&proto, 0, পরিমাণ(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }

  connssl->connecting_state = ssl_connect_2;
  ঘূৰাই CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
স্থির পূর্ণাঙ্ক ossl_trace_ech_retry_configs(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data, SSL *ssl,
                                        পূর্ণাঙ্ক reason)
{
  CURLcode result = CURLE_OK;
  size_t rcl = 0;
  পূর্ণাঙ্ক rv = 1;
#ifndef HAVE_BORINGSSL_LIKE
  ৱর্ণ *inner = রিক্ত;
  uint8_t *rcs = রিক্ত;
  ৱর্ণ *outer = রিক্ত;
#অন্যথা
  ধ্রুৱ ৱর্ণ *inner = রিক্ত;
  ধ্রুৱ uint8_t *rcs = রিক্ত;
  ধ্রুৱ ৱর্ণ *outer = রিক্ত;
  size_t out_name_len = 0;
  পূর্ণাঙ্ক servername_type = 0;
#endif
  NOVERBOSE((শূন্য)reason);

  /* nothing to trace if not doing ECH */
  যদি(!Curl_ssl_ech_enabled(cf))
    ঘূৰাই rv;
#ifndef HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#অন্যথা
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (পূর্ণাঙ্ক)rcl;
#endif

  যদি(rv && rcs) {
    ৱর্ণ *b64str = রিক্ত;
    size_t blen = 0;

    result = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    যদি(!result && b64str) {
      infof(data, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ifndef HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#অন্যথা
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#endif
    }
  }
  অন্যথা
    infof(data, "ECH: no retry_configs (rv = %d)", rv);
#ifndef HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#endif
  ঘূৰাই rv;
}

#endif

স্থির CURLcode ossl_connect_step2(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  সংরচনা ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  পূর্ণাঙ্ক err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  যদি(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, octx);
    যদি(result)
      ঘূৰাই result;
    octx->x509_store_setup = TRUE;
  }

#যদি !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  যদি(Curl_tls_keylog_enabled() && !octx->keylog_done)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_done);
#endif

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  যদি(err != 1) {
    পূর্ণাঙ্ক detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(data, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    যদি(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ঘূৰাই CURLE_AGAIN;
    }
    যদি(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ঘূৰাই CURLE_AGAIN;
    }
#ifdef SSL_ERROR_WANT_ASYNC
    যদি(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ঘূৰাই CURLE_AGAIN;
    }
#endif
#ifdef SSL_ERROR_WANT_RETRY_VERIFY
    যদি(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(data, TRUE);
      ঘূৰাই CURLE_AGAIN;
    }
#endif
    অন্যথা {
      /* untreated error */
      sslerr_t errdetail;
      ৱর্ণ error_buffer[256] = "";
      CURLcode result;
      long lerr;
      পূর্ণাঙ্ক lib;
      পূর্ণাঙ্ক reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      যদি((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#ifdef SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#endif
         )) {
        result = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_result(octx->ssl);
        যদি(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        অন্যথা
          failf(data, "%s", "SSL certificate verification failed");
      }
#ifdef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      অন্যথা যদি((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        result = CURLE_SSL_CLIENTCERT;
        failf(data, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_buffer, পরিমাণ(error_buffer)));
      }
#endif
#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
      অন্যথা যদি((lib == ERR_LIB_SSL) &&
#ifndef HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#অন্যথা
              (reason == SSL_R_ECH_REJECTED)) {
#endif /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, reason);

        result = CURLE_ECH_REQUIRED;
        failf(data, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_buffer, পরিমাণ(error_buffer)));
      }
#endif
      অন্যথা {
        result = CURLE_SSL_CONNECT_ERROR;
        failf(data, "TLS connect error: %s",
              ossl_strerror(errdetail, error_buffer, পরিমাণ(error_buffer)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      যদি(result == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        ৱর্ণ extramsg[80] = "";
        পূর্ণাঙ্ক sockerr = SOCKERRNO;

        যদি(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, পরিমাণ(extramsg));
        failf(data, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      ঘূৰাই result;
    }
  }
  অন্যথা {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(data, octx);

#যদি defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    যদি(Curl_ssl_ech_enabled(cf)) {
      সংরচনা ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      ৱর্ণ *inner = রিক্ত, *outer = রিক্ত;
      পূর্ণাঙ্ক rv;
      VERBOSE(ধ্রুৱ ৱর্ণ *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      ৱিকল্প(rv) {
      পক্ষ SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        ভঙ্গ;
      পক্ষ SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        ভঙ্গ;
      পক্ষ SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        ভঙ্গ;
      পক্ষ SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        ভঙ্গ;
      পক্ষ SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        ভঙ্গ;
      পক্ষ SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        ভঙ্গ;
      পক্ষ SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        ভঙ্গ;
      পক্ষ SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        ভঙ্গ;
      পক্ষ SSL_ECH_STATUS_BAD_NAME: {
        যদি(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        অন্যথা {
          VERBOSE(status = "bad name (unexpected)");
        }
        ভঙ্গ;
      }
      স্ৱভাৱ:
        VERBOSE(status = "unexpected status");
        infof(data, "ECH: unexpected status %d", rv);
      }
      infof(data, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      যদি(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, 0);
      }
      যদি(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(data, "ECH: ech-hard failed");
        ঘূৰাই CURLE_SSL_CONNECT_ERROR;
      }
    }
    অন্যথা {
      infof(data, "ECH: result: status is not attempted");
    }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    যদি(connssl->alpn) {
      ধ্রুৱ unsigned ৱর্ণ *neg_protocol;
      unsigned পূর্ণাঙ্ক len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      ঘূৰাই Curl_alpn_set_negotiated(cf, data, connssl, neg_protocol, len);
    }

    ঘূৰাই CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
স্থির CURLcode ossl_pkp_pin_peer_pubkey(সংরচনা Curl_easy *data, X509 *cert,
                                         ধ্রুৱ ৱর্ণ *pinnedpubkey)
{
  /* Scratch */
  পূর্ণাঙ্ক len1 = 0, len2 = 0;
  unsigned ৱর্ণ *buff1 = রিক্ত, *temp = রিক্ত;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  যদি(!pinnedpubkey)
    ঘূৰাই CURLE_OK;

  যদি(!cert)
    ঘূৰাই result;

  কৰা {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), রিক্ত);
    যদি(len1 < 1)
      ভঙ্গ; /* failed */

    buff1 = temp = curlx_malloc(len1);
    যদি(!buff1)
      ভঙ্গ; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &temp);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    যদি((len1 != len2) || !temp || ((temp - buff1) != len1))
      ভঙ্গ; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } যেতিয়ালৈ(0);

  যদি(buff1)
    curlx_free(buff1);

  ঘূৰাই result;
}

#ifdef CURLVERBOSE
#যদি !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_সংখ্যা) && LIBRESSL_VERSION_সংখ্যা < 0x3060000fL)
স্থির শূন্য infof_certstack(সংরচনা Curl_easy *data, ধ্রুৱ SSL *ssl)
{
  স্তূপ_OF(X509) *certstack;
  long verify_result;
  পূর্ণাঙ্ক অঙ্ক_cert_levels;
  পূর্ণাঙ্ক cert_level;

  যদি(!Curl_trc_is_verbose(data))
    ঘূৰাই;

  verify_result = SSL_get_verify_result(ssl);
  যদি(verify_result != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  অন্যথা
    certstack = SSL_get0_verified_chain(ssl);
  যদি(!certstack)
    ঘূৰাই;
  অঙ্ক_cert_levels = sk_X509_অঙ্ক(certstack);

  বাবে(cert_level = 0; cert_level < অঙ্ক_cert_levels; cert_level++) {
    ৱর্ণ cert_algorithm[80] = "";
    ৱর্ণ group_name_final[80] = "";
    ধ্রুৱ X509_ALGOR *palg_cert = রিক্ত;
    ধ্রুৱ ASN1_OBJECT *paobj_cert = রিক্ত;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    পূর্ণাঙ্ক চাবি_bits;
    পূর্ণাঙ্ক চাবি_sec_bits;
    পূর্ণাঙ্ক get_group_name;
    ধ্রুৱ ৱর্ণ *type_name;

    current_cert = sk_X509_মান(certstack, cert_level);
    যদি(!current_cert)
      অনুৱর্ত;

    current_pkey = X509_get0_pubkey(current_cert);
    যদি(!current_pkey)
      অনুৱর্ত;

    X509_get0_signature(রিক্ত, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, রিক্ত, রিক্ত, palg_cert);
    OBJ_obj2txt(cert_algorithm, পরিমাণ(cert_algorithm), paobj_cert, 0);

    চাবি_bits = EVP_PKEY_bits(current_pkey);
#ifndef HAVE_OPENSSL3
#define EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#endif
    চাবি_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#ifdef HAVE_OPENSSL3
    {
      ৱর্ণ group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               পরিমাণ(group_name), রিক্ত);
      curl_msnprintf(group_name_final, পরিমাণ(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#অন্যথা
    get_group_name = 0;
    type_name = রিক্ত;
#endif

    infof(data, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          চাবি_bits, চাবি_sec_bits, cert_algorithm);
  }
}
#অন্যথা
#define infof_certstack(data, ssl)
#endif
#endif /* CURLVERBOSE */

স্থির CURLcode ossl_check_issuer(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  X509 *server_cert)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = রিক্ত;
  BIO *fp = রিক্ত;
  ৱর্ণ err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode result = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  যদি(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->data,
                         (পূর্ণাঙ্ক)conn_config->issuercert_blob->len);
    যদি(!fp) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, পরিমাণ(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }
  }
  অন্যথা যদি(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    যদি(!fp) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, পরিমাণ(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ out;
    }

    যদি(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      যদি(verify_enabled)
        failf(data, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      গচ্ছ out;
    }
  }

  যদি(fp) {
    issuer = PEM_read_bio_X509(fp, রিক্ত, ZERO_NULL, রিক্ত);
    যদি(!issuer) {
      যদি(verify_enabled)
        failf(data, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      গচ্ছ out;
    }

    যদি(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      যদি(verify_enabled)
        failf(data, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      গচ্ছ out;
    }

    infof(data, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  যদি(fp)
    BIO_free(fp);
  যদি(issuer)
    X509_free(issuer);
  ঘূৰাই result;
}

স্থির ধ্রুৱ ৱর্ণ *pinned(সংরচনা Curl_cfilter *cf,
                          সংরচনা Curl_easy *data)
{
  (শূন্য)cf;
  ঘূৰাই
#ifndef CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#endif
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
}

স্থির CURLcode ossl_check_pinned_চাবি(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      X509 *server_cert)
{
  CURLcode result = CURLE_OK;
  ধ্রুৱ ৱর্ণ *ptr = pinned(cf, data);
  যদি(ptr) {
    result = ossl_pkp_pin_peer_pubkey(data, server_cert, ptr);
    যদি(result)
      failf(data, "SSL: public key does not match pinned public key");
  }
  ঘূৰাই result;
}

#ifdef CURLVERBOSE
#define MAX_CERT_NAME_LENGTH 2048
স্থির CURLcode ossl_infof_cert(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                X509 *server_cert)
{
  BIO *mem = রিক্ত;
  সংরচনা dynbuf dname;
  ৱর্ণ err_buf[256] = "";
  ৱর্ণ *buf;
  long len;
  CURLcode result = CURLE_OK;

  যদি(!Curl_trc_is_verbose(data))
    ঘূৰাই CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_LENGTH);
  mem = BIO_new(BIO_s_mem());
  যদি(!mem) {
    failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, পরিমাণ(err_buf)));
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  infof(data, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  result = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(data, "  subject: %s", result ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_data(mem, (ৱর্ণ **)&buf);
  infof(data, "  start date: %.*s", (পূর্ণাঙ্ক)len, buf);
  (শূন্য)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_data(mem, (ৱর্ণ **)&buf);
  infof(data, "  expire date: %.*s", (পূর্ণাঙ্ক)len, buf);
  (শূন্য)BIO_reset(mem);

  result = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  যদি(result) /* should be only fatal stuff like OOM */
    গচ্ছ out;
  infof(data, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  ঘূৰাই result;
}
#endif /* CURLVERBOSE */

#ifdef USE_APPLE_SECTRUST
সংরচনা ossl_certs_ctx {
  স্তূপ_OF(X509) *sk;
  size_t অঙ্ক_certs;
  unsigned ৱর্ণ *last_der;
};

স্থির CURLcode ossl_chain_get_der(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   শূন্য *user_data,
                                   size_t i,
                                   unsigned ৱর্ণ **pder,
                                   size_t *pder_len)
{
  সংরচনা ossl_certs_ctx *chain = user_data;
  X509 *cert;
  পূর্ণাঙ্ক der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = রিক্ত;

  (শূন্য)cf;
  (শূন্য)data;
  *pder_len = 0;
  *pder = রিক্ত;

  যদি(i >= chain->অঙ্ক_certs)
    ঘূৰাই CURLE_TOO_LARGE;
  cert = sk_X509_মান(chain->sk, (পূর্ণাঙ্ক)i);
  যদি(!cert)
    ঘূৰাই CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  যদি(der_len < 0)
    ঘূৰাই CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (size_t)der_len;
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode ossl_apple_verify(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা ossl_ctx *octx,
                                  সংরচনা ssl_peer *peer)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা ossl_certs_ctx chain;
  CURLcode result;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, পরিমাণ(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.অঙ্ক_certs = chain.sk ? sk_X509_অঙ্ক(chain.sk) : 0;

  যদি(!chain.অঙ্ক_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    যদি(!octx->reused_session) {
      failf(data, "SSL: could not get peer certificate chain");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    অন্যথা {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(data, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      ঘূৰাই CURLE_OK;
    }
  }
  অন্যথা {
#ifdef HAVE_BORINGSSL_LIKE
    ধ্রুৱ uint8_t *ocsp_data = রিক্ত;
#অন্যথা
    unsigned ৱর্ণ *ocsp_data = রিক্ত;
#endif
    long ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    যদি(!octx->reused_session)
      ocsp_len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_data);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    যদি(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_data = রিক্ত;
    }

    result = Curl_vtls_apple_verify(cf, data, peer, chain.অঙ্ক_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_data, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = রিক্ত;
  }
  octx->sectrust_verified = !result;
  ঘূৰাই result;
}
#endif /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   সংরচনা ossl_ctx *octx,
                                   সংরচনা ssl_peer *peer)
{
  সংরচনা connectdata *conn = cf->conn;
  সংরচনা ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  long ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  যদি(data->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    result = ossl_certchain(data, octx->ssl);
    যদি(result)
      ঘূৰাই result;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  যদি(!server_cert) {
    /* no verification at all, this maybe acceptable */
    যদি(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, data))
      গচ্ছ out;

    failf(data, "SSL: could not get peer certificate");
    result = CURLE_PEER_FAILED_VERIFICATION;
    গচ্ছ out;
  }

#ifdef CURLVERBOSE
  result = ossl_infof_cert(cf, data, server_cert);
  যদি(result)
    গচ্ছ out;
  infof_certstack(data, octx->ssl);
#endif

  যদি(conn_config->verifyhost) {
    result = ossl_verifyhost(data, conn, peer, server_cert);
    যদি(result)
      গচ্ছ out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_result(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(data, "OpenSSL verify result: %lx", (unsigned long)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  যদি(verified)
    infof(data, "SSL certificate verified via OpenSSL.");

#ifdef USE_APPLE_SECTRUST
  যদি(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    result = ossl_apple_verify(cf, data, octx, peer);
    যদি(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      গচ্ছ out; /* unexpected error */
    যদি(octx->sectrust_verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#endif

  যদি(!verified) {
    /* no trust established, report the OpenSSL status */
    যদি(conn_config->verifypeer) {
      failf(data, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      result = CURLE_PEER_FAILED_VERIFICATION;
      গচ্ছ out;
    }
    infof(data, " SSL certificate verification failed, continuing anyway!");
  }

#ifndef OPENSSL_NO_OCSP
  যদি(conn_config->verifystatus &&
#ifdef USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, data)) &&
#endif
     !octx->reused_session) {
    result = verifystatus(cf, data, octx);
    যদি(result)
      গচ্ছ out;
  }
#endif

  result = ossl_check_issuer(cf, data, server_cert);
  যদি(result)
    গচ্ছ out;

  result = ossl_check_pinned_চাবি(cf, data, server_cert);

out:
  X509_free(server_cert);
  ঘূৰাই result;
}

স্থির CURLcode ossl_connect_step3(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  result = Curl_ossl_check_peer_cert(cf, data, octx, &connssl->peer);
  যদি(result)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, data, connssl->peer.scache_চাবি);

  ঘূৰাই result;
}

#ifdef HAVE_OPENSSL_EARLYDATA
স্থির CURLcode ossl_send_earlydata(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ধ্রুৱ unsigned ৱর্ণ *buf;
  size_t blen, nwritten;
  পূর্ণাঙ্ক rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_result = CURLE_OK;
  যেতিয়ালৈ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_data(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    যদি(rc <= 0) {
      long sslerror;
      ৱর্ণ error_buffer[256];
      পূর্ণাঙ্ক err = SSL_get_error(octx->ssl, rc);

      ৱিকল্প(err) {
      পক্ষ SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        result = CURLE_AGAIN;
        গচ্ছ out;
      পক্ষ SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_AGAIN;
        গচ্ছ out;
      পক্ষ SSL_ERROR_SYSCALL: {
        পূর্ণাঙ্ক sockerr = SOCKERRNO;

        যদি(octx->io_result == CURLE_AGAIN) {
          result = CURLE_AGAIN;
          গচ্ছ out;
        }
        sslerror = ERR_get_error();
        যদি(sslerror)
          ossl_strerror(sslerror, error_buffer, পরিমাণ(error_buffer));
        অন্যথা যদি(sockerr)
          curlx_strerror(sockerr, error_buffer, পরিমাণ(error_buffer));
        অন্যথা
          curl_msnprintf(error_buffer, পরিমাণ(error_buffer), "%s",
                         SSL_ERROR_to_str(err));

        failf(data, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_SEND_ERROR;
        গচ্ছ out;
      }
      পক্ষ SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(data, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_buffer, পরিমাণ(error_buffer)));
        result = CURLE_SEND_ERROR;
        গচ্ছ out;
      }
      স্ৱভাৱ:
        /* a true error */
        failf(data, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        result = CURLE_SEND_ERROR;
        গচ্ছ out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ঘূৰাই result;
}
#endif /* HAVE_OPENSSL_EARLYDATA */

স্থির CURLcode ossl_connect(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             bool *done)
{
  CURLcode result = CURLE_OK;
  সংরচনা ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  যদি(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  যদি(connssl->connecting_state == ssl_connect_1) {
    যদি(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
      ঘূৰাই CURLE_OK;
    }
    CURL_TRC_CF(data, cf, "ossl_connect, step1");
    result = ossl_connect_step1(cf, data);
    যদি(result)
      গচ্ছ out;
  }

  যদি(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(data, cf, "ossl_connect, step2");
#ifdef HAVE_OPENSSL_EARLYDATA
    যদি(connssl->earlydata_state == ssl_earlydata_await) {
      গচ্ছ out;
    }
    অন্যথা যদি(connssl->earlydata_state == ssl_earlydata_sending) {
      result = ossl_send_earlydata(cf, data);
      যদি(result)
        গচ্ছ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#endif
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    result = ossl_connect_step2(cf, data);
    যদি(result)
      গচ্ছ out;
  }

  যদি(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(data, cf, "ossl_connect, step3");
    result = ossl_connect_step3(cf, data);
    যদি(result)
      গচ্ছ out;
    connssl->connecting_state = ssl_connect_done;
#ifdef HAVE_OPENSSL_EARLYDATA
    যদি(connssl->earlydata_state > ssl_earlydata_none) {
      সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_data_status(octx->ssl) == SSL_EARLY_DATA_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
  }

  যদি(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(data, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  যদি(result == CURLE_AGAIN) {
    *done = FALSE;
    ঘূৰাই CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  ঘূৰাই result;
}

স্থির bool ossl_data_pending(সংরচনা Curl_cfilter *cf,
                              ধ্রুৱ সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  (শূন্য)data;
  ঘূৰাই (bool)connssl->প্ৰবেশ_pending;
}

স্থির CURLcode ossl_send(সংরচনা Curl_cfilter *cf,
                          সংরচনা Curl_easy *data,
                          ধ্রুৱ শূন্য *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  পূর্ণাঙ্ক err;
  ৱর্ণ error_buffer[256];
  sslerr_t sslerror;
  পূর্ণাঙ্ক memlen;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)INT_MAX) ? INT_MAX : (পূর্ণাঙ্ক)len;
  যদি(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    যদি(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  যদি(nwritten > 0) {
    যদি(nwritten > memlen) {
      failf(data, "SSL_write(len=%d) returned %d", memlen, nwritten);
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
    *pnwritten = (size_t)nwritten;
  }
  অন্যথা {
    err = SSL_get_error(octx->ssl, nwritten);

    ৱিকল্প(err) {
    পক্ষ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      result = CURLE_AGAIN;
      গচ্ছ out;
    পক্ষ SSL_ERROR_WANT_WRITE:
      result = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      গচ্ছ out;
    পক্ষ SSL_ERROR_SYSCALL: {
      পূর্ণাঙ্ক sockerr = SOCKERRNO;

      যদি(octx->io_result == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        result = CURLE_AGAIN;
        গচ্ছ out;
      }
      sslerror = ERR_get_error();
      যদি(sslerror)
        ossl_strerror(sslerror, error_buffer, পরিমাণ(error_buffer));
      অন্যথা যদি(sockerr)
        curlx_strerror(sockerr, error_buffer, পরিমাণ(error_buffer));
      অন্যথা
        curl_msnprintf(error_buffer, পরিমাণ(error_buffer), "%s",
                       SSL_ERROR_to_str(err));

      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_buffer, sockerr);
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
    পক্ষ SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(data, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_buffer, পরিমাণ(error_buffer)));
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
    স্ৱভাৱ:
      /* a true error */
      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      result = CURLE_SEND_ERROR;
      গচ্ছ out;
    }
  }

out:
  ঘূৰাই result;
}

স্থির CURLcode ossl_recv(সংরচনা Curl_cfilter *cf,
                          সংরচনা Curl_easy *data,   /* transfer */
                          ৱর্ণ *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  ৱর্ণ error_buffer[256];
  unsigned long sslerror;
  পূর্ণাঙ্ক buffsize;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)INT_MAX) ? INT_MAX : (পূর্ণাঙ্ক)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  যদি(nread > 0)
    *pnread = (size_t)nread;
  অন্যথা {
    /* failed SSL_read */
    পূর্ণাঙ্ক err = SSL_get_error(octx->ssl, nread);

    ৱিকল্প(err) {
    পক্ষ SSL_ERROR_NONE: /* this is not an error */
      ভঙ্গ;
    পক্ষ SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      যদি(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(data, cf, "TLS close_notify");
      ভঙ্গ;
    পক্ষ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      গচ্ছ out;
    পক্ষ SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      গচ্ছ out;
    স্ৱভাৱ:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      যদি(octx->io_result == CURLE_AGAIN) {
        result = CURLE_AGAIN;
        গচ্ছ out;
      }
      sslerror = ERR_get_error();
      যদি((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        পূর্ণাঙ্ক sockerr = SOCKERRNO;
        যদি(sslerror)
          ossl_strerror(sslerror, error_buffer, পরিমাণ(error_buffer));
        অন্যথা যদি(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_buffer, পরিমাণ(error_buffer));
        অন্যথা
          curl_msnprintf(error_buffer, পরিমাণ(error_buffer), "%s",
                         SSL_ERROR_to_str(err));
        failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_RECV_ERROR;
        গচ্ছ out;
      }
      অন্যথা যদি(err == SSL_ERROR_SYSCALL) {
        যদি(octx->io_result) {
          /* logging handling in underlying filter already */
          result = octx->io_result;
        }
        অন্যথা যদি(connssl->peer_closed) {
          failf(data, "Connection closed abruptly");
          result = CURLE_RECV_ERROR;
        }
        অন্যথা {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          পূর্ণাঙ্ক sockerr = SOCKERRNO;
          যদি(sockerr)
            curlx_strerror(sockerr, error_buffer, পরিমাণ(error_buffer));
          অন্যথা {
            curl_msnprintf(error_buffer, পরিমাণ(error_buffer),
                           "Connection closed abruptly");
          }
          failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_buffer, sockerr);
          result = CURLE_RECV_ERROR;
        }
        গচ্ছ out;
      }
    }
  }

out:
  যদি((!result && !*pnread) || (result == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->প্ৰবেশ_pending = FALSE;
  }
  CURL_TRC_CF(data, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (পূর্ণাঙ্ক)result, *pnread, connssl->প্ৰবেশ_pending);
  ঘূৰাই result;
}

স্থির CURLcode ossl_get_channel_binding(সংরচনা Curl_easy *data,
                                         int8_t sockindex,
                                         সংরচনা dynbuf *binding)
{
  X509 *cert;
  পূর্ণাঙ্ক mdnid;
  bool no_digest_acceptable = FALSE;
  ধ্রুৱ EVP_MD *algo_type = রিক্ত;
  ধ্রুৱ ৱর্ণ *algo_name = রিক্ত;
  unsigned পূর্ণাঙ্ক length;
  unsigned ৱর্ণ buf[EVP_MAX_MD_SIZE];

  স্থির ধ্রুৱ ৱর্ণ prefix[] = "tls-server-end-point:";
  সংরচনা connectdata *conn = data->conn;
  সংরচনা Curl_cfilter *cf = conn->cfilter[sockindex];
  সংরচনা ossl_ctx *octx = রিক্ত;
  CURLcode result = CURLE_OK;

  কৰা {
    ধ্রুৱ সংরচনা Curl_cftype *cft = cf->cft;
    সংরচনা ssl_connect_data *connssl = cf->ctx;

    যদি(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (সংরচনা ossl_ctx *)connssl->backend;
      ভঙ্গ;
    }

    cf = cf->next;
  } যেতিয়ালৈ(cf);

  যদি(!octx) {
    failf(data, "Failed to find the SSL filter");
    ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  যদি(!cert)
    /* No server certificate, do not do channel binding */
    ঘূৰাই CURLE_OK;

#ifdef HAVE_OPENSSL3
  {
    পূর্ণাঙ্ক pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    যদি(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(data, "certificate signature algorithm not recognized");
      result = CURLE_SSL_INVALIDCERTSTATUS;
      গচ্ছ out;
    }

    যদি(mdnid != NID_undef) {
      যদি(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      অন্যথা
        algo_type = EVP_get_digestbynid(mdnid);
    }
    অন্যথা যদি(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    অন্যথা যদি(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      ৱর্ণ mdname[128] = "";
      পূর্ণাঙ্ক rc = EVP_PKEY_get_default_digest_name(pkey, mdname, পরিমাণ(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      যদি(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      অন্যথা যদি(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(data, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#অন্যথা /* HAVE_OPENSSL3 */

  যদি(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, রিক্ত)) {
    failf(data,
          "Unable to find digest NID for certificate signature algorithm");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  যদি(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  অন্যথা {
    algo_type = EVP_get_digestbynid(mdnid);
    যদি(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(data, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      result = CURLE_SSL_INVALIDCERTSTATUS;
      গচ্ছ out;
    }
  }

#endif /* HAVE_OPENSSL3, else */

  যদি(!algo_type) {
    যদি(no_digest_acceptable) {
      infof(data, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      result = CURLE_OK;
      গচ্ছ out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(data, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ out;
  }

  যদি(!X509_digest(cert, algo_type, buf, &length)) {
    failf(data, "X509_digest() failed for channel binding");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    গচ্ছ out;
  }

  /* Append "tls-server-end-point:" */
  result = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  যদি(result)
    গচ্ছ out;

  /* Append digest */
  result = curlx_dyn_addn(binding, buf, length);

out:
  X509_free(cert);
  ঘূৰাই result;
}

size_t Curl_ossl_version(ৱর্ণ *buffer, size_t size)
{
#ifdef LIBRESSL_VERSION_সংখ্যা
  ৱর্ণ *p;
  size_t গুনতি;
  ধ্রুৱ ৱর্ণ *ver = OpenSSL_version(OPENSSL_VERSION);
  স্থির ধ্রুৱ ৱর্ণ expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  যদি(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  গুনতি = curl_msnprintf(buffer, size, "%s/%s", OSSL_PACKAGE, ver);
  বাবে(p = buffer; *p; ++p) {
    যদি(ISBLANK(*p))
      *p = '_';
  }
  ঘূৰাই গুনতি;
#নোচেদ্যদি defined(OPENSSL_IS_AWSLC)
  ঘূৰাই curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_সংখ্যা_STRING);
#নোচেদ্যদি defined(OPENSSL_IS_BORINGSSL)
#ifdef CURL_BORINGSSL_VERSION
  ঘূৰাই curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#অন্যথা
  ঘূৰাই curl_msnprintf(buffer, size, OSSL_PACKAGE);
#endif
#অন্যথা /* OpenSSL 3+ */
  ঘূৰাই curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
}

/* can be called with data == NULL */
স্থির CURLcode ossl_random(সংরচনা Curl_easy *data,
                            unsigned ৱর্ণ *entropy, size_t length)
{
  পূর্ণাঙ্ক rc;
  যদি(data) {
    যদি(ossl_seed(data)) /* Initiate the seed if not already done */
      ঘূৰাই CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  অন্যথা {
    যদি(!rand_enough())
      ঘূৰাই CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(length));
  ঘূৰাই rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

স্থির CURLcode ossl_sha256sum(ধ্রুৱ unsigned ৱর্ণ *প্ৰবেশ,
                               size_t len,
                               unsigned ৱর্ণ *sha256sum /* output */,
                               size_t unused)
{
  CURLcode result = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (শূন্য)unused;

  mdctx = EVP_MD_CTX_new();
  যদি(!mdctx)
    ঘূৰাই CURLE_OUT_OF_MEMORY;
  যদি(!EVP_DigestInit_ex(mdctx, EVP_sha256(), রিক্ত)) {
    result = CURLE_FAILED_INIT;
    গচ্ছ out;
  }
  যদি(!EVP_DigestUpdate(mdctx, প্ৰবেশ, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, রিক্ত))
    result = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  ঘূৰাই result;
}

স্থির bool ossl_cert_status_request(শূন্য)
{
#ifndef OPENSSL_NO_OCSP
  ঘূৰাই TRUE;
#অন্যথা
  ঘূৰাই FALSE;
#endif
}

স্থির শূন্য *ossl_get_internals(সংরচনা ssl_connect_data *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  সংরচনা ossl_ctx *octx = (সংরচনা ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  ঘূৰাই info == CURLINFO_TLS_SESSION ?
    (শূন্য *)octx->ssl_ctx : (শূন্য *)octx->ssl;
}

ধ্রুৱ সংরচনা Curl_ssl Curl_ssl_openssl = {
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

  পরিমাণ(সংরচনা ossl_ctx),

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
