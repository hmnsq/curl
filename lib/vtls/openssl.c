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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_OPENSSL

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "formdata.h" /* for the boundary function */
#অন্তর্ভাব "url.h" /* for the SSL config check function */
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "vtls/openssl.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "vtls/keylog.h"
#অন্তর্ভাব "vtls/hostcheck.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "multiif.h"
#অন্তর্ভাব "curlx/strerr.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/strcopy.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "vtls/apple.h"
#ifdef USE_ECH
#অন্তর্ভাব "curlx/base64.h"
#endif

#অন্তর্ভাব <openssl/rand.h>
#অন্তর্ভাব <openssl/x509v3.h>
#ifndef OPENSSL_NO_DSA
#অন্তর্ভাব <openssl/dsa.h>
#endif
#অন্তর্ভাব <openssl/dh.h>
#অন্তর্ভাব <openssl/err.h>
#অন্তর্ভাব <openssl/conf.h>
#অন্তর্ভাব <openssl/bn.h>
#অন্তর্ভাব <openssl/rsa.h>
#অন্তর্ভাব <openssl/bio.h>
#অন্তর্ভাব <openssl/pkcs12.h>
#অন্তর্ভাব <openssl/tls1.h>
#অন্তর্ভাব <openssl/evp.h>

#যদি defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#অন্তর্ভাব <openssl/ech.h>
#endif

#ifndef OPENSSL_NO_OCSP
#অন্তর্ভাব <openssl/ocsp.h>
#endif

#যদি !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#define USE_OPENSSL_ENGINE
#অন্তর্ভাব <openssl/engine.h>
#endif

#ifdef LIBRESSL_VERSION_সংখ্যা
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  যদি LIBRESSL_VERSION_সংখ্যা < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#নতুবা_যদি !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#endif

#যদি defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#অন্তর্ভাব <openssl/provider.h>
#অন্তর্ভাব <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#define OPENSSL_HAS_PROVIDERS
#endif

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#যদি !defined(LIBRESSL_VERSION_সংখ্যা) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_চাবি_EXPLICIT_PARAMS))
#define HAVE_SSL_CTX_SET_DEFAULT_READ_বাফার_LEN 1
#endif

#যদি defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#অন্তর্ভাব <openssl/ui.h>
#endif

#ifdef HAVE_OPENSSL3
#define HAVE_EVP_PKEY_GET_PARAMS 1
#endif

#ifdef HAVE_EVP_PKEY_GET_PARAMS
#অন্তর্ভাব <openssl/core_names.h>
#define DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = ফাঁকা
#define FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#অন্যথা
#define DECLARE_PKEY_PARAM_BIGNUM(name) ধ্রুবক BIGNUM *name
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
#নতুবা_যদি defined(OPENSSL_IS_AWSLC)
#define OSSL_PACKAGE "AWS-LC"
#নতুবা_যদি defined(OPENSSL_IS_BORINGSSL)
#define OSSL_PACKAGE "BoringSSL"
#নতুবা_যদি defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#define OSSL_PACKAGE "quictls"
#অন্যথা
#define OSSL_PACKAGE "OpenSSL"
#endif

#ifdef HAVE_BORINGSSL_LIKE
প্রকারলেখ size_t numcert_t;
প্রকারলেখ uint32_t sslerr_t;
#অন্যথা
প্রকারলেখ পূর্ণসংখ্যা numcert_t;
প্রকারলেখ unsigned long sslerr_t;
#endif
#define ossl_valsize_t numcert_t

#যদি OPENSSL_VERSION_সংখ্যা >= 0x40100000L
#define ossl_ASN1_STRING_get_দৈর্ঘ্য ASN1_STRING_get_দৈর্ঘ্য
#অন্যথা
স্থির size_t ossl_ASN1_STRING_get_দৈর্ঘ্য(ধ্রুবক ASN1_STRING *str)
{
   ধ্রুবক পূর্ণসংখ্যা দৈর্ঘ্য = ASN1_STRING_দৈর্ঘ্য(str);
   ফেরত দৈর্ঘ্য >= 0 ? (size_t)দৈর্ঘ্য : 0;
}
#endif

স্থির CURLcode ঠেলো_certinfo(কাঠামো Curl_easy *তথ্য,
                              BIO *mem, ধ্রুবক অক্ষর *label, পূর্ণসংখ্যা অঙ্ক)
  WARN_UNUSED_ফলাফল;

স্থির CURLcode ঠেলো_certinfo(কাঠামো Curl_easy *তথ্য,
                              BIO *mem, ধ্রুবক অক্ষর *label, পূর্ণসংখ্যা অঙ্ক)
{
  অক্ষর *ptr;
  long len = BIO_get_mem_তথ্য(mem, &ptr);
  CURLcode ফলাফল = Curl_ssl_ঠেলো_certinfo_len(তথ্য, অঙ্ক, label, ptr, len);
  (শূন্য)BIO_reset(mem);
  ফেরত ফলাফল;
}

স্থির CURLcode pubkey_show(কাঠামো Curl_easy *তথ্য,
                            BIO *mem,
                            পূর্ণসংখ্যা অঙ্ক,
                            ধ্রুবক অক্ষর *type,
                            ধ্রুবক অক্ষর *name,
                            ধ্রুবক BIGNUM *bn) WARN_UNUSED_ফলাফল;

স্থির CURLcode pubkey_show(কাঠামো Curl_easy *তথ্য,
                            BIO *mem,
                            পূর্ণসংখ্যা অঙ্ক,
                            ধ্রুবক অক্ষর *type,
                            ধ্রুবক অক্ষর *name,
                            ধ্রুবক BIGNUM *bn)
{
  অক্ষর namebuf[32];

  curl_msnprintf(namebuf, আকার(namebuf), "%s(%s)", type, name);

  যদি(bn)
    BN_print(mem, bn);
  ফেরত ঠেলো_certinfo(তথ্য, mem, namebuf, অঙ্ক);
}

#define print_pubkey_BN(_type, _name, অঙ্ক_2)           \
  pubkey_show(তথ্য, mem, অঙ্ক_2, #_type, #_name, _name)

স্থির পূর্ণসংখ্যা asn1_object_dump(ধ্রুবক ASN1_OBJECT *a, অক্ষর *buf, size_t len)
{
  পূর্ণসংখ্যা i = i2t_ASN1_OBJECT(buf, (পূর্ণসংখ্যা)len, a);
  ফেরত (i >= (পূর্ণসংখ্যা)len);  /* buffer too small */
}

স্থির CURLcode X509V3_ext(কাঠামো Curl_easy *তথ্য,
                           পূর্ণসংখ্যা certnum,
                           ধ্রুবক স্তূপ_OF(X509_EXTENSION) *extsarg)
{
  পূর্ণসংখ্যা i;
  CURLcode ফলাফল = CURLE_OK;
#ifdef LIBRESSL_VERSION_সংখ্যা
  স্তূপ_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#অন্যথা
  ধ্রুবক স্তূপ_OF(X509_EXTENSION) *exts = extsarg;
#endif

  যদি((পূর্ণসংখ্যা)sk_X509_EXTENSION_অঙ্ক(exts) <= 0)
    /* no extensions, bail out */
    ফেরত ফলাফল;

  জন্য(i = 0; i < (পূর্ণসংখ্যা)sk_X509_EXTENSION_অঙ্ক(exts); i++) {
    ধ্রুবক ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_মান(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    অক্ষর namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    যদি(!bio_out)
      ফেরত ফলাফল;

    obj = X509_EXTENSION_get_object(ext);

    যদি(asn1_object_dump(obj, namebuf, আকার(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    যদি(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (ধ্রুবক ASN1_STRING *)X509_EXTENSION_get_তথ্য(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    ফলাফল = Curl_ssl_ঠেলো_certinfo_len(তথ্য, certnum, namebuf, biomem->তথ্য,
                                        biomem->দৈর্ঘ্য);
    BIO_free(bio_out);
    যদি(ফলাফল)
      বিরতি;
  }
  ফেরত ফলাফল;
}

স্থির CURLcode get_pkey_rsa(কাঠামো Curl_easy *তথ্য,
                             EVP_PKEY *pubkey, BIO *mem, পূর্ণসংখ্যা i)
{
  CURLcode ফলাফল = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  RSA *rsa = EVP_PKEY_get0_RSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(n);
  DECLARE_PKEY_PARAM_BIGNUM(e);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_N, &n);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_E, &e);
#অন্যথা
  RSA_get0_চাবি(rsa, &n, &e, ফাঁকা);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (পূর্ণসংখ্যা)(n ? BN_অঙ্ক_bits(n) : 0));
  ফলাফল = ঠেলো_certinfo(তথ্য, mem, "RSA Public Key", i);
  যদি(!ফলাফল) {
    ফলাফল = print_pubkey_BN(rsa, n, i);
    যদি(!ফলাফল)
      ফলাফল = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  ফেরত ফলাফল;
}

#ifndef OPENSSL_NO_DSA
স্থির CURLcode get_pkey_dsa(কাঠামো Curl_easy *তথ্য,
                             EVP_PKEY *pubkey, BIO *mem, পূর্ণসংখ্যা i)
{
  CURLcode ফলাফল = CURLE_OK;
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
  DSA_get0_চাবি(dsa, &pub_চাবি, ফাঁকা);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  ফলাফল = print_pubkey_BN(dsa, p, i);
  যদি(!ফলাফল)
    ফলাফল = print_pubkey_BN(dsa, q, i);
  যদি(!ফলাফল)
    ফলাফল = print_pubkey_BN(dsa, g, i);
  যদি(!ফলাফল)
    ফলাফল = print_pubkey_BN(dsa, pub_চাবি, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_চাবি);
  ফেরত ফলাফল;
}
#endif /* !OPENSSL_NO_DSA */

স্থির CURLcode get_pkey_dh(কাঠামো Curl_easy *তথ্য,
                            EVP_PKEY *pubkey, BIO *mem, পূর্ণসংখ্যা i)
{
  CURLcode ফলাফল;
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
  DH_get0_চাবি(dh, &pub_চাবি, ফাঁকা);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  ফলাফল = print_pubkey_BN(dh, p, i);
  যদি(!ফলাফল)
    ফলাফল = print_pubkey_BN(dh, q, i);
  যদি(!ফলাফল)
    ফলাফল = print_pubkey_BN(dh, g, i);
  যদি(!ফলাফল)
    ফলাফল = print_pubkey_BN(dh, pub_চাবি, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_চাবি);
  ফেরত ফলাফল;
}

#ifdef HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
প্রকারলেখ ধ্রুবক X509_PUBKEY pubkeytype_t;
#অন্যথা
প্রকারলেখ X509_PUBKEY pubkeytype_t;
#endif

স্থির CURLcode ossl_certchain(কাঠামো Curl_easy *তথ্য, SSL *ssl)
{
  CURLcode ফলাফল;
  স্তূপ_OF(X509) *sk;
  পূর্ণসংখ্যা i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  যদি(!sk)
    ফেরত CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_অঙ্ক(sk);
  যদি(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(তথ্য, "%d certificates is more than allowed (%d)", (পূর্ণসংখ্যা)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  ফলাফল = Curl_ssl_init_certinfo(তথ্য, (পূর্ণসংখ্যা)numcerts);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  mem = BIO_new(BIO_s_mem());
  যদি(!mem)
    ফলাফল = CURLE_OUT_OF_MEMORY;

  জন্য(i = 0; !ফলাফল && (i < (পূর্ণসংখ্যা)numcerts); i++) {
    X509 *x = sk_X509_মান(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = ফাঁকা;
    size_t j;
    ধ্রুবক ASN1_BIT_STRING *psig = ফাঁকা;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Subject", i);
    যদি(ফলাফল)
      বিরতি;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Issuer", i);
    যদি(ফলাফল)
      বিরতি;

    BIO_printf(mem, "%lx", (unsigned long)X509_get_version(x));
    ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Version", i);
    যদি(ফলাফল)
      বিরতি;

    {
      ASN1_INTEGER *অঙ্ক = X509_get_serial_সংখ্যা(x);
      ধ্রুবক unsigned অক্ষর *numdata = ASN1_STRING_get0_তথ্য(অঙ্ক);
      ধ্রুবক size_t numlen = ossl_ASN1_STRING_get_দৈর্ঘ্য(অঙ্ক);
      যদি(ASN1_STRING_type(অঙ্ক) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      জন্য(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Serial Number", i);
    যদি(ফলাফল)
      বিরতি;

    {
      ধ্রুবক X509_ALGOR *sigalg = ফাঁকা;
      pubkeytype_t *xpubkey = ফাঁকা;
      ASN1_OBJECT *pubkeyoid = ফাঁকা;

      X509_get0_signature(&psig, &sigalg, x);
      যদি(sigalg) {
        ধ্রুবক ASN1_OBJECT *sigalgoid = ফাঁকা;
        X509_ALGOR_get0(&sigalgoid, ফাঁকা, ফাঁকা, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Signature Algorithm", i);
        যদি(ফলাফল)
          বিরতি;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      যদি(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, ফাঁকা, ফাঁকা, ফাঁকা, xpubkey);
        যদি(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Public Key Algorithm", i);
          যদি(ফলাফল)
            বিরতি;
        }
      }

      ফলাফল = X509V3_ext(তথ্য, i, X509_get0_extensions(x));
      যদি(ফলাফল)
        বিরতি;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Start date", i);
    যদি(ফলাফল)
      বিরতি;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Expire date", i);
    যদি(ফলাফল)
      বিরতি;

    pubkey = X509_get_pubkey(x);
    যদি(!pubkey)
      infof(তথ্য, "   Unable to load public key");
    অন্যথা {
      নির্বাচন(EVP_PKEY_id(pubkey)) {
      ক্ষেত্র EVP_PKEY_RSA:
        ফলাফল = get_pkey_rsa(তথ্য, pubkey, mem, i);
        বিরতি;

#ifndef OPENSSL_NO_DSA
      ক্ষেত্র EVP_PKEY_DSA:
        ফলাফল = get_pkey_dsa(তথ্য, pubkey, mem, i);
        বিরতি;
#endif

      ক্ষেত্র EVP_PKEY_DH:
        ফলাফল = get_pkey_dh(তথ্য, pubkey, mem, i);
        বিরতি;
      }
      EVP_PKEY_free(pubkey);
    }

    যদি(!ফলাফল && psig) {
      ধ্রুবক unsigned অক্ষর *psigdata = ASN1_STRING_get0_তথ্য(psig);
      ধ্রুবক size_t psiglen = ossl_ASN1_STRING_get_দৈর্ঘ্য(psig);
      জন্য(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Signature", i);
    }

    যদি(!ফলাফল) {
      PEM_write_bio_X509(mem, x);
      ফলাফল = ঠেলো_certinfo(তথ্য, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  যদি(ফলাফল)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(তথ্য);

  ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_তথ্য(bio, ফাঁকা);
  ফেরত 1;
}

স্থির পূর্ণসংখ্যা ossl_bio_cf_destroy(BIO *bio)
{
  যদি(!bio)
    ফেরত 0;
  ফেরত 1;
}

স্থির long ossl_bio_cf_ctrl(BIO *bio, পূর্ণসংখ্যা cmd, long অঙ্ক, শূন্য *ptr)
{
  কাঠামো Curl_cfilter *cf = BIO_get_তথ্য(bio);
  long ret = 1;

  (শূন্য)cf;
  (শূন্য)ptr;
  নির্বাচন(cmd) {
  ক্ষেত্র BIO_CTRL_GET_CLOSE:
    ret = (long)BIO_get_shutdown(bio);
    বিরতি;
  ক্ষেত্র BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (পূর্ণসংখ্যা)অঙ্ক);
    বিরতি;
  ক্ষেত্র BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    বিরতি;
  ক্ষেত্র BIO_CTRL_DUP:
    ret = 1;
    বিরতি;
  ক্ষেত্র BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
    ফেরত connssl->peer_closed;
  }
  পূর্বনির্ধারিত:
    ret = 0;
    বিরতি;
  }
  ফেরত ret;
}

স্থির পূর্ণসংখ্যা ossl_bio_cf_out_write(BIO *bio, ধ্রুবক অক্ষর *buf, পূর্ণসংখ্যা blen)
{
  কাঠামো Curl_cfilter *cf = BIO_get_তথ্য(bio);
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  size_t nwritten;
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য);
  যদি(blen < 0)
    ফেরত 0;

  ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য,
                             (ধ্রুবক uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(তথ্য, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (পূর্ণসংখ্যা)ফলাফল, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_ফলাফল = ফলাফল;
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    ফেরত -1;
  }
  ফেরত (পূর্ণসংখ্যা)nwritten;
}

স্থির পূর্ণসংখ্যা ossl_bio_cf_in_read(BIO *bio, অক্ষর *buf, পূর্ণসংখ্যা blen)
{
  কাঠামো Curl_cfilter *cf = BIO_get_তথ্য(bio);
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  size_t nread;
  CURLcode ফলাফল, r2;

  DEBUGASSERT(তথ্য);
  /* OpenSSL catches this case, so should we. */
  যদি(!buf)
    ফেরত 0;
  যদি(blen < 0)
    ফেরত 0;

  ফলাফল = Curl_conn_cf_recv(cf->পরবর্তী, তথ্য, buf, (size_t)blen, &nread);
  CURL_TRC_CF(তথ্য, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (পূর্ণসংখ্যা)ফলাফল, nread);
  BIO_clear_retry_flags(bio);
  octx->io_ফলাফল = ফলাফল;
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  অন্যথা {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->প্রবেশ_pending = TRUE;
    যদি(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  যদি(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, তথ্য, octx);
    যদি(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_ফলাফল = r2;
      ফেরত -1;
    }
    octx->x509_store_setup = TRUE;
  }
  ফেরত ফলাফল ? -1 : (পূর্ণসংখ্যা)nread;
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
  ফেরত m;
}

স্থির শূন্য ossl_bio_cf_method_free(BIO_METHOD *m)
{
  যদি(m)
    BIO_meth_free(m);
}

#ifndef HAVE_KEYLOG_UPSTREAM
#ifdef HAVE_KEYLOG_CALLBACK
স্থির শূন্য ossl_keylog_callback(ধ্রুবক SSL *ssl, ধ্রুবক অক্ষর *সারি)
{
  (শূন্য)ssl;

  Curl_tls_keylog_write_সারি(সারি);
}
#অন্যথা
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
স্থির শূন্য ossl_log_tls12_secret(ধ্রুবক SSL *ssl, bool *keylog_done)
{
  ধ্রুবক SSL_SESSION *session;
  unsigned অক্ষর client_random[SSL3_RANDOM_আকার];
  unsigned অক্ষর master_চাবি[SSL_MAX_MASTER_চাবি_দৈর্ঘ্য];
  পূর্ণসংখ্যা master_চাবি_দৈর্ঘ্য = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  যদি(!session || *keylog_done) {
    ERR_বের_করো_to_mark();
    ফেরত;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_আকার);
  master_চাবি_দৈর্ঘ্য = (পূর্ণসংখ্যা)
    SSL_SESSION_get_master_চাবি(session, master_চাবি, SSL_MAX_MASTER_চাবি_দৈর্ঘ্য);

  ERR_বের_করো_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  যদি(master_চাবি_দৈর্ঘ্য <= 0)
    ফেরত;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        আকার(client_random),
                        master_চাবি, master_চাবি_দৈর্ঘ্য);
}
#endif /* !HAVE_KEYLOG_CALLBACK */
#endif /* HAVE_KEYLOG_UPSTREAM */

স্থির ধ্রুবক অক্ষর *SSL_ERROR_to_str(পূর্ণসংখ্যা err)
{
  নির্বাচন(err) {
  ক্ষেত্র SSL_ERROR_NONE:
    ফেরত "SSL_ERROR_NONE";
  ক্ষেত্র SSL_ERROR_SSL:
    ফেরত "SSL_ERROR_SSL";
  ক্ষেত্র SSL_ERROR_WANT_READ:
    ফেরত "SSL_ERROR_WANT_READ";
  ক্ষেত্র SSL_ERROR_WANT_WRITE:
    ফেরত "SSL_ERROR_WANT_WRITE";
  ক্ষেত্র SSL_ERROR_WANT_X509_LOOKUP:
    ফেরত "SSL_ERROR_WANT_X509_LOOKUP";
  ক্ষেত্র SSL_ERROR_SYSCALL:
    ফেরত "SSL_ERROR_SYSCALL";
  ক্ষেত্র SSL_ERROR_ZERO_RETURN:
    ফেরত "SSL_ERROR_ZERO_RETURN";
  ক্ষেত্র SSL_ERROR_WANT_CONNECT:
    ফেরত "SSL_ERROR_WANT_CONNECT";
  ক্ষেত্র SSL_ERROR_WANT_ACCEPT:
    ফেরত "SSL_ERROR_WANT_ACCEPT";
#ifdef SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ক্ষেত্র SSL_ERROR_WANT_ASYNC:
    ফেরত "SSL_ERROR_WANT_ASYNC";
#endif
#ifdef SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  ক্ষেত্র SSL_ERROR_WANT_ASYNC_JOB:
    ফেরত "SSL_ERROR_WANT_ASYNC_JOB";
#endif
#ifdef SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  ক্ষেত্র SSL_ERROR_WANT_CLIENT_HELLO_CB:
    ফেরত "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#endif
  পূর্বনির্ধারিত:
    ফেরত "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
স্থির অক্ষর *ossl_strerror(unsigned long error, অক্ষর *buf, size_t আকার_2)
{
  size_t len;
  DEBUGASSERT(আকার_2);
  *buf = '\0';

  len = Curl_ossl_version(buf, আকার_2);
  DEBUGASSERT(len < (আকার_2 - 2));
  যদি(len < (আকার_2 - 2)) {
    buf += len;
    আকার_2 -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ifdef HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, আকার_2);
#অন্যথা
  ERR_error_string_n(error, buf, আকার_2);
#endif

  যদি(!*buf) {
    ধ্রুবক অক্ষর *বার্তা = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, আকার_2, বার্তা, strlen(বার্তা));
  }

  ফেরত buf;
}

স্থির পূর্ণসংখ্যা passwd_callback(অক্ষর *buf, পূর্ণসংখ্যা অঙ্ক, পূর্ণসংখ্যা encrypting, শূন্য *password)
{
  DEBUGASSERT(encrypting == 0);

  যদি(!encrypting && অঙ্ক >= 0 && password) {
    পূর্ণসংখ্যা klen = curlx_uztosi(strlen((অক্ষর *)password));
    যদি(অঙ্ক > klen) {
      memcpy(buf, password, klen + 1);
      ফেরত klen;
    }
  }
  ফেরত 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
স্থির bool rand_enough(শূন্য)
{
  ফেরত RAND_status() != 0;
}

স্থির CURLcode ossl_seed(কাঠামো Curl_easy *তথ্য)
{
  /* This might get called before it has been added to a multi handle */
  যদি(তথ্য->multi && তথ্য->multi->ssl_seeded)
    ফেরত CURLE_OK;

  যদি(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    যদি(তথ্য->multi)
      তথ্য->multi->ssl_seeded = TRUE;
    ফেরত CURLE_OK;
  }
  failf(তথ্য, "Insufficient randomness");
  ফেরত CURLE_SSL_CONNECT_ERROR;
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
স্থির পূর্ণসংখ্যা ossl_do_file_type(ধ্রুবক অক্ষর *type)
{
  যদি(!type || !type[0])
    ফেরত SSL_FILETYPE_PEM;
  যদি(curl_strequal(type, "PEM"))
    ফেরত SSL_FILETYPE_PEM;
  যদি(curl_strequal(type, "DER"))
    ফেরত SSL_FILETYPE_ASN1;
  যদি(curl_strequal(type, "PROV"))
    ফেরত SSL_FILETYPE_PROVIDER;
  যদি(curl_strequal(type, "ENG"))
    ফেরত SSL_FILETYPE_ENGINE;
  যদি(curl_strequal(type, "P12"))
    ফেরত SSL_FILETYPE_PKCS12;
  ফেরত -1;
}

#যদি defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
স্থির পূর্ণসংখ্যা ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ধ্রুবক অক্ষর *password;
  নির্বাচন(UI_get_string_type(uis)) {
  ক্ষেত্র UIT_PROMPT:
  ক্ষেত্র UIT_VERIFY:
    password = (ধ্রুবক অক্ষর *)UI_get0_user_তথ্য(ui);
    যদি(password && (UI_get_প্রবেশ_flags(uis) & UI_প্রবেশ_FLAG_DEFAULT_PWD)) {
      UI_set_ফলাফল(ui, uis, password);
      ফেরত 1;
    }
    FALLTHROUGH();
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
স্থির পূর্ণসংখ্যা ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  নির্বাচন(UI_get_string_type(uis)) {
  ক্ষেত্র UIT_PROMPT:
  ক্ষেত্র UIT_VERIFY:
    যদি(UI_get0_user_তথ্য(ui) &&
       (UI_get_প্রবেশ_flags(uis) & UI_প্রবেশ_FLAG_DEFAULT_PWD)) {
      ফেরত 1;
    }
    FALLTHROUGH();
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
স্থির bool is_pkcs11_uri(ধ্রুবক অক্ষর *string)
{
  ফেরত string && curl_strnequal(string, "pkcs11:", 7);
}

#endif

স্থির CURLcode ossl_set_engine(কাঠামো Curl_easy *তথ্য, কাঠামো ossl_ctx *octx,
                                ধ্রুবক অক্ষর *name);
#ifdef OPENSSL_HAS_PROVIDERS
স্থির CURLcode ossl_set_provider(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ossl_ctx *octx,
                                  ধ্রুবক অক্ষর *iname);
#endif

স্থির পূর্ণসংখ্যা use_certificate_blob(SSL_CTX *ctx, ধ্রুবক কাঠামো curl_blob *blob,
                                পূর্ণসংখ্যা type, ধ্রুবক অক্ষর *চাবি_passwd)
{
  পূর্ণসংখ্যা ret = 0;
  X509 *x = ফাঁকা;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->তথ্য, (পূর্ণসংখ্যা)blob->len);
  যদি(!in)
    ফেরত CURLE_OUT_OF_MEMORY;

  যদি(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, ফাঁকা);
  }
  অন্যথা যদি(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, ফাঁকা, passwd_callback, CURL_UNCONST(চাবি_passwd));
  }
  অন্যথা {
    ret = 0;
    যাও end;
  }

  যদি(!x) {
    ret = 0;
    যাও end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  ফেরত ret;
}

স্থির পূর্ণসংখ্যা use_privatekey_blob(SSL_CTX *ctx, ধ্রুবক কাঠামো curl_blob *blob,
                               পূর্ণসংখ্যা type, ধ্রুবক অক্ষর *চাবি_passwd)
{
  পূর্ণসংখ্যা ret = 0;
  EVP_PKEY *pkey = ফাঁকা;
  BIO *in = BIO_new_mem_buf(blob->তথ্য, (পূর্ণসংখ্যা)blob->len);
  যদি(!in)
    ফেরত CURLE_OUT_OF_MEMORY;

  যদি(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_Private_চাবি(in, ফাঁকা, passwd_callback,
                                   CURL_UNCONST(চাবি_passwd));
  অন্যথা যদি(type == SSL_FILETYPE_ASN1)
    pkey = d2i_Private_চাবি_bio(in, ফাঁকা);
  অন্যথা
    যাও end;

  যদি(!pkey)
    যাও end;

  ret = SSL_CTX_use_Private_চাবি(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  ফেরত ret;
}

স্থির পূর্ণসংখ্যা use_certificate_chain_blob(SSL_CTX *ctx,
                                      ধ্রুবক কাঠামো curl_blob *blob,
                                      ধ্রুবক অক্ষর *চাবি_passwd)
{
  পূর্ণসংখ্যা ret = 0;
  X509 *x = ফাঁকা;
  BIO *in = BIO_new_mem_buf(blob->তথ্য, (পূর্ণসংখ্যা)blob->len);
  যদি(!in)
    ফেরত CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, ফাঁকা,
                            passwd_callback, CURL_UNCONST(চাবি_passwd));
  যদি(!x)
    যাও end;

  ret = SSL_CTX_use_certificate(ctx, x);

  যদি(ERR_peek_error() != 0)
    ret = 0;

  যদি(ret) {
    X509 *ca;
    sslerr_t err;

    যদি(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      যাও end;
    }

    যখন((ca = PEM_read_bio_X509(in, ফাঁকা, passwd_callback,
                                  CURL_UNCONST(চাবি_passwd))) != ফাঁকা) {

      যদি(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        যাও end;
      }
    }

    err = ERR_peek_last_error();
    যদি((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_সারি))
      ERR_clear_error();
    অন্যথা
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  ফেরত ret;
}

স্থির পূর্ণসংখ্যা enginecheck(কাঠামো Curl_easy *তথ্য,
                       কাঠামো ossl_ctx *octx,
                       ধ্রুবক অক্ষর *চাবি_file,
                       ধ্রুবক অক্ষর *চাবি_passwd)
{
#ifdef USE_OPENSSL_ENGINE
  EVP_PKEY *priv_চাবি = ফাঁকা;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  যদি(!octx->p.engine && is_pkcs11_uri(চাবি_file) &&
     ossl_set_engine(তথ্য, octx, "pkcs11") != CURLE_OK)
    ফেরত 0;

  যদি(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    যদি(!ui_method) {
      failf(তথ্য, "unable to create " OSSL_PACKAGE " user-interface method");
      ফেরত 0;
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
      failf(তথ্য, "failed to load private key from crypto engine");
      ফেরত 0;
    }
    যদি(SSL_CTX_use_Private_চাবি(ctx, priv_চাবি) != 1) {
      failf(তথ্য, "unable to set private key");
      EVP_PKEY_free(priv_চাবি);
      ফেরত 0;
    }
    EVP_PKEY_free(priv_চাবি);  /* we do not need the handle any more... */
  }
  অন্যথা {
    failf(তথ্য, "crypto engine not set, cannot load private key");
    ফেরত 0;
  }
  ফেরত 1;
#অন্যথা
  (শূন্য)octx;
  (শূন্য)চাবি_file;
  (শূন্য)চাবি_passwd;
  failf(তথ্য, "SSL_FILETYPE_ENGINE not supported for private key");
  ফেরত 0;
#endif
}

স্থির পূর্ণসংখ্যা providercheck(কাঠামো Curl_easy *তথ্য,
                         কাঠামো ossl_ctx *octx,
                         ধ্রুবক অক্ষর *চাবি_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  অক্ষর error_বাফার[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  যদি(!octx->p.provider_loaded && is_pkcs11_uri(চাবি_file) &&
     ossl_set_provider(তথ্য, octx, "pkcs11") != CURLE_OK) {
    ফেরত 0;
  }

  যদি(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_চাবি = ফাঁকা;
    OSSL_STORE_CTX *store = ফাঁকা;
    OSSL_STORE_INFO *info = ফাঁকা;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    যদি(!ui_method) {
      failf(তথ্য, "unable to create " OSSL_PACKAGE " user-interface method");
      ফেরত 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(চাবি_file, octx->p.libctx,
                               octx->p.propq, ui_method, ফাঁকা, ফাঁকা,
                               ফাঁকা, ফাঁকা);
    যদি(!store) {
      failf(তথ্য, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      UI_destroy_method(ui_method);
      ফেরত 0;
    }
    যদি(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(তথ্য, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
    }

    info = OSSL_STORE_load(store);
    যদি(info) {
      পূর্ণসংখ্যা ossl_type = OSSL_STORE_INFO_get_type(info);

      যদি(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_চাবি = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    যদি(!priv_চাবি) {
      failf(তথ্য, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      ফেরত 0;
    }

    যদি(SSL_CTX_use_Private_চাবি(ctx, priv_চাবি) != 1) {
      failf(তথ্য, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      EVP_PKEY_free(priv_চাবি);
      ফেরত 0;
    }
    EVP_PKEY_free(priv_চাবি); /* we do not need the handle any more... */
  }
  অন্যথা {
    failf(তথ্য, "crypto provider not set, cannot load private key");
    ফেরত 0;
  }
  ফেরত 1;
#অন্যথা
  (শূন্য)octx;
  (শূন্য)চাবি_file;
  failf(তথ্য, "SSL_FILETYPE_PROVIDER not supported for private key");
  ফেরত 0;
#endif
}

স্থির পূর্ণসংখ্যা engineload(কাঠামো Curl_easy *তথ্য,
                      কাঠামো ossl_ctx *octx,
                      ধ্রুবক অক্ষর *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#যদি defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  অক্ষর error_বাফার[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  যদি(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(তথ্য, octx, "pkcs11") != CURLE_OK)
    ফেরত 0;

  যদি(octx->p.engine) {
    স্থির ধ্রুবক অক্ষর cmd_name[] = "LOAD_CERT_CTRL";
    কাঠামো {
      ধ্রুবক অক্ষর *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = ফাঁকা;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    যদি(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), ফাঁকা)) {
      failf(তথ্য, "SSL engine does not support loading certificates");
      ফেরত 0;
    }

    /* Load the certificate from the engine */
    যদি(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, ফাঁকা, 1)) {
      failf(তথ্য, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      ফেরত 0;
    }

    যদি(!params.cert) {
      failf(তথ্য, "SSL engine did not initialize the certificate properly.");
      ফেরত 0;
    }

    যদি(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(তথ্য, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      X509_free(params.cert);
      ফেরত 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  অন্যথা {
    failf(তথ্য, "crypto engine not set, cannot load certificate");
    ফেরত 0;
  }
  ফেরত 1;
#অন্যথা
  (শূন্য)octx;
  (শূন্য)cert_file;
  failf(তথ্য, "SSL_FILETYPE_ENGINE not supported for certificate");
  ফেরত 0;
#endif
}

স্থির পূর্ণসংখ্যা providerload(কাঠামো Curl_easy *তথ্য,
                        কাঠামো ossl_ctx *octx,
                        ধ্রুবক অক্ষর *cert_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  অক্ষর error_বাফার[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  যদি(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(তথ্য, octx, "pkcs11") != CURLE_OK)
    ফেরত 0;

  যদি(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = ফাঁকা;
    X509 *cert = ফাঁকা;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা);
    পূর্ণসংখ্যা rc;

    যদি(!store) {
      failf(তথ্য, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      ফেরত 0;
    }
    যদি(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(তথ্য, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
    }

    info = OSSL_STORE_load(store);
    যদি(info) {
      পূর্ণসংখ্যা ossl_type = OSSL_STORE_INFO_get_type(info);

      যদি(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    যদি(!cert) {
      failf(তথ্য, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      ফেরত 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    যদি(rc != 1) {
      failf(তথ্য, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      ফেরত 0;
    }
  }
  অন্যথা {
    failf(তথ্য, "crypto provider not set, cannot load certificate");
    ফেরত 0;
  }
  ফেরত 1;
#অন্যথা
  (শূন্য)octx;
  (শূন্য)cert_file;
  failf(তথ্য, "SSL_FILETYPE_PROVIDER not supported for certificate");
  ফেরত 0;
#endif
}

স্থির পূর্ণসংখ্যা pkcs12load(কাঠামো Curl_easy *তথ্য,
                      SSL_CTX* ctx,
                      ধ্রুবক কাঠামো curl_blob *cert_blob,
                      ধ্রুবক অক্ষর *cert_file,
                      ধ্রুবক অক্ষর *চাবি_passwd)
{
  অক্ষর error_বাফার[256];
  BIO *cert_bio = ফাঁকা;
  PKCS12 *p12 = ফাঁকা;
  EVP_PKEY *pri;
  X509 *x509;
  পূর্ণসংখ্যা cert_done = 0;
  স্তূপ_OF(X509) *ca = ফাঁকা;
  যদি(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->তথ্য, (পূর্ণসংখ্যা)cert_blob->len);
    যদি(!cert_bio) {
      failf(তথ্য, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      ফেরত 0;
    }
  }
  অন্যথা {
    cert_bio = BIO_new(BIO_s_file());
    যদি(!cert_bio) {
      failf(তথ্য, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
      ফেরত 0;
    }

    যদি(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(তথ্য, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      ফেরত 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, ফাঁকা);
  BIO_free(cert_bio);

  যদি(!p12) {
    failf(তথ্য, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    ফেরত 0;
  }

  যদি(!PKCS12_parse(p12, চাবি_passwd, &pri, &x509, &ca)) {
    failf(তথ্য, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_বাফার, আকার(error_বাফার)));
    PKCS12_free(p12);
    ফেরত 0;
  }

  PKCS12_free(p12);

  যদি(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(তথ্য, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_বাফার, আকার(error_বাফার)));
    যাও fail;
  }

  যদি(SSL_CTX_use_Private_চাবি(ctx, pri) != 1) {
    failf(তথ্য, "unable to use private key from PKCS12 file '%s'", cert_file);
    যাও fail;
  }

  যদি(!SSL_CTX_check_private_চাবি(ctx)) {
    failf(তথ্য, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    যাও fail;
  }
  /* Set Certificate Verification chain */
  যদি(ca) {
    যখন(sk_X509_অঙ্ক(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_বের_করো(ca);
      যদি(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(তথ্য, "cannot add certificate to client CA list");
        যাও fail;
      }
      যদি(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(তথ্য, "cannot add certificate to certificate chain");
        যাও fail;
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
  sk_X509_বের_করো_free(ca, X509_free);
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
#endif
  যদি(!cert_done)
    ফেরত 0; /* failure! */
  ফেরত 1;
}

স্থির CURLcode client_cert(কাঠামো Curl_easy *তথ্য,
                            কাঠামো ossl_ctx *octx,
                            অক্ষর *cert_file,
                            ধ্রুবক কাঠামো curl_blob *cert_blob,
                            ধ্রুবক অক্ষর *cert_type,
                            অক্ষর *চাবি_file,
                            ধ্রুবক কাঠামো curl_blob *চাবি_blob,
                            ধ্রুবক অক্ষর *চাবি_type,
                            অক্ষর *চাবি_passwd)
{
  অক্ষর error_বাফার[256];
  bool check_privkey = TRUE;
  পূর্ণসংখ্যা file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  যদি(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    পূর্ণসংখ্যা cert_use_ফলাফল;

    যদি(চাবি_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, চাবি_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    নির্বাচন(file_type) {
    ক্ষেত্র SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_ফলাফল = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, চাবি_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      যদি(cert_use_ফলাফল != 1) {
        failf(তথ্য,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_বাফার,
                            আকার(error_বাফার)));
        ফেরত CURLE_SSL_CERTPROBLEM;
      }
      বিরতি;

    ক্ষেত্র SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_ফলাফল = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, চাবি_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      যদি(cert_use_ফলাফল != 1) {
        failf(তথ্য,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_বাফার,
                            আকার(error_বাফার)));
        ফেরত CURLE_SSL_CERTPROBLEM;
      }
      বিরতি;

    ক্ষেত্র SSL_FILETYPE_ENGINE:
      যদি(!cert_file || !engineload(তথ্য, octx, cert_file))
        ফেরত CURLE_SSL_CERTPROBLEM;
      বিরতি;

    ক্ষেত্র SSL_FILETYPE_PROVIDER:
      যদি(!cert_file || !providerload(তথ্য, octx, cert_file))
        ফেরত CURLE_SSL_CERTPROBLEM;
      বিরতি;

    ক্ষেত্র SSL_FILETYPE_PKCS12:
      যদি(!pkcs12load(তথ্য, ctx, cert_blob, cert_file, চাবি_passwd))
        ফেরত CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      বিরতি;

    পূর্বনির্ধারিত:
      failf(তথ্য, "not supported file type '%s' for certificate", cert_type);
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }

    যদি(!চাবি_file && !চাবি_blob) {
      চাবি_file = cert_file;
      চাবি_blob = cert_blob;
    }
    অন্যথা
      file_type = ossl_do_file_type(চাবি_type);

    নির্বাচন(file_type) {
    ক্ষেত্র SSL_FILETYPE_PEM:
    ক্ষেত্র SSL_FILETYPE_ASN1:
      cert_use_ফলাফল = চাবি_blob ?
        use_privatekey_blob(ctx, চাবি_blob, file_type, চাবি_passwd) :
      SSL_CTX_use_Private_চাবি_file(ctx, চাবি_file, file_type);
      যদি(cert_use_ফলাফল != 1) {
        failf(তথ্য, "unable to set private key file: '%s' type %s",
              চাবি_file ? চাবি_file : "(memory blob)",
              চাবি_type ? চাবি_type : "PEM");
        ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
      }
      বিরতি;
    ক্ষেত্র SSL_FILETYPE_ENGINE:
      যদি(!enginecheck(তথ্য, octx, চাবি_file, চাবি_passwd))
        ফেরত CURLE_SSL_CERTPROBLEM;
      বিরতি;

    ক্ষেত্র SSL_FILETYPE_PROVIDER:
      যদি(!providercheck(তথ্য, octx, চাবি_file))
        ফেরত CURLE_SSL_CERTPROBLEM;
      বিরতি;

    ক্ষেত্র SSL_FILETYPE_PKCS12:
      যদি(!pcks12_done) {
        failf(তথ্য, "file type P12 for private key not supported");
        ফেরত CURLE_SSL_CERTPROBLEM;
      }
      বিরতি;
    পূর্বনির্ধারিত:
      failf(তথ্য, "not supported file type for private key");
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    যদি(!ssl) {
      failf(তথ্য, "unable to create an SSL structure");
      ফেরত CURLE_OUT_OF_MEMORY;
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
        failf(তথ্য, "Private key does not match the certificate public key");
        ফেরত CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  ফেরত CURLE_OK;
}

#ifdef CURLVERBOSE
/* returns non-zero on failure */
স্থির CURLcode x509_name_oneline(ধ্রুবক X509_NAME *a, কাঠামো dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  পূর্ণসংখ্যা rc;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  যদি(bio_out) {
    unsigned long flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    যদি(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      ফলাফল = curlx_dyn_addn(d, biomem->তথ্য, biomem->দৈর্ঘ্য);
    }
    BIO_free(bio_out);
  }
  ফেরত ফলাফল;
}
#endif

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
স্থির পূর্ণসংখ্যা ossl_init(শূন্য)
{
  ধ্রুবক uint64_t flags =
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
  OPENSSL_init_ssl(flags, ফাঁকা);

#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#endif

  ফেরত 1;
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
স্থির CURLcode ossl_set_engine(কাঠামো Curl_easy *তথ্য, কাঠামো ossl_ctx *octx,
                                ধ্রুবক অক্ষর *name)
{
#ifdef USE_OPENSSL_ENGINE
  CURLcode ফলাফল = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  যদি(e) {

    যদি(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = ফাঁকা;
    }
    যদি(!ENGINE_init(e)) {
      অক্ষর buf[256];

      ENGINE_free(e);
      failf(তথ্য, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, আকার(buf)));
      ফলাফল = CURLE_SSL_ENGINE_INITFAILED;
      e = ফাঁকা;
    }
    অন্যথা {
      ফলাফল = CURLE_OK;
    }
    octx->p.engine = e;
    ফেরত ফলাফল;
  }
#endif
#ifdef OPENSSL_HAS_PROVIDERS
  ফেরত ossl_set_provider(তথ্য, octx, name);
#অন্যথা
  (শূন্য)octx;
  (শূন্য)name;
  failf(তথ্য, "OpenSSL engine not found");
  ফেরত CURLE_SSL_ENGINE_NOTFOUND;
#endif
}

/* Sets engine as default for all SSL operations
 */
স্থির CURLcode ossl_set_engine_default(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ossl_ctx *octx)
{
#ifdef USE_OPENSSL_ENGINE
  যদি(octx->p.engine) {
    যদি(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(তথ্য, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    অন্যথা {
      failf(তথ্য, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      ফেরত CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)octx;
#endif
  ফেরত CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
স্থির কাঠামো curl_slist *ossl_engines_list(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো curl_slist *list = ফাঁকা;
#ifdef USE_OPENSSL_ENGINE
  কাঠামো curl_slist *beg;
  ENGINE *e;

  জন্য(e = ENGINE_get_first(); e; e = ENGINE_get_পরবর্তী(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    যদি(!beg) {
      curl_slist_free_all(list);
      ফেরত ফাঁকা;
    }
    list = beg;
  }
#endif
  (শূন্য)তথ্য;
  ফেরত list;
}

#ifdef OPENSSL_HAS_PROVIDERS

স্থির শূন্য ossl_provider_cleanup(কাঠামো ossl_ctx *octx)
{
  যদি(!octx)
    ফেরত;
  যদি(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = ফাঁকা;
  }
  যদি(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = ফাঁকা;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = ফাঁকা;
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
স্থির CURLcode ossl_set_provider(কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ossl_ctx *octx,
                                  ধ্রুবক অক্ষর *iname)
{
  অক্ষর name[MAX_PROVIDER_LEN + 1];
  কাঠামো Curl_str prov;
  ধ্রুবক অক্ষর *propq = ফাঁকা;

  যদি(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    ফেরত CURLE_OK;
  }
  যদি(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  memcpy(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  যদি(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    যদি(!libctx)
      ফেরত CURLE_OUT_OF_MEMORY;
    যদি(propq) {
      octx->p.propq = curlx_strdup(propq);
      যদি(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        ফেরত CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#ifndef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  যদি(!OSSL_LIB_CTX_load_config(octx->p.libctx, ফাঁকা)) {
    infof(তথ্য, "Failed to load default openssl config. Proceeding.");
  }
#endif

  যদি(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    ফেরত CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  যদি(!octx->p.provider) {
    অক্ষর error_বাফার[256];
    failf(তথ্য, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_বাফার, আকার(error_বাফার)));
    ossl_provider_cleanup(octx);
    ফেরত CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  যদি(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(তথ্য, "Failed to load base");
    ফেরত CURLE_SSL_ENGINE_NOTFOUND;
  }
  অন্যথা
    octx->p.provider_loaded = TRUE;
  ফেরত CURLE_OK;
}
#অন্যথা /* OPENSSL_HAS_PROVIDERS */
#define ossl_provider_cleanup(x)
#endif

স্থির CURLcode ossl_shutdown(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              bool send_shutdown, bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  অক্ষর buf[1024];
  পূর্ণসংখ্যা nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  যদি(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    যাও out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  যদি(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    জন্য(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (পূর্ণসংখ্যা)আকার(buf));
      CURL_TRC_CF(তথ্য, cf, "SSL shutdown not sent, read -> %d", nread);
      যদি(nread <= 0)
        বিরতি;
    }
    err = SSL_get_error(octx->ssl, nread);
    যদি(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool প্রবেশ_pending;
      /* Yes, it did. */
      যদি(!send_shutdown) {
        CURL_TRC_CF(তথ্য, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        যাও out;
      }
      অন্যথা যদি(!cf->পরবর্তী->cft->is_alive(cf->পরবর্তী, তথ্য, &প্রবেশ_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(তথ্য, cf, "peer closed connection");
        *done = TRUE;
        যাও out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  যদি(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    পূর্ণসংখ্যা rc;
    ERR_clear_error();
    CURL_TRC_CF(তথ্য, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    যদি(rc == 1) {
      CURL_TRC_CF(তথ্য, cf, "SSL shutdown finished");
      *done = TRUE;
      যাও out;
    }
    যদি(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(তথ্য, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      যাও out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  জন্য(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (পূর্ণসংখ্যা)আকার(buf));
    CURL_TRC_CF(তথ্য, cf, "SSL shutdown read -> %d", nread);
    যদি(nread <= 0)
      বিরতি;
  }
  err = SSL_get_error(octx->ssl, nread);
  নির্বাচন(err) {
  ক্ষেত্র SSL_ERROR_ZERO_RETURN: /* no more data */
    যদি(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(তথ্য, cf, "SSL shutdown finished");
    অন্যথা
      CURL_TRC_CF(তথ্য, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    বিরতি;
  ক্ষেত্র SSL_ERROR_NONE: /* did not get anything */
  ক্ষেত্র SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(তথ্য, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    বিরতি;
  ক্ষেত্র SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(তথ্য, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    বিরতি;
  পূর্বনির্ধারিত:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(unsigned long sslerr = ERR_get_error());
      CURL_TRC_CF(তথ্য, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, আকার(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    ফলাফল = CURLE_OK;
    বিরতি;
  }

out:
  cf->shutdown = (ফলাফল || *done);
  যদি(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->প্রবেশ_pending = FALSE;
  ফেরত ফলাফল;
}

/* also used by quic-tls */
শূন্য Curl_osslctx_cleanup(কাঠামো ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  যদি(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = ফাঁকা;
  }
  যদি(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = ফাঁকা;
    octx->x509_store_setup = FALSE;
  }
  যদি(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = ফাঁকা;
  }
#ifdef USE_OPENSSL_ENGINE
  যদি(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = ফাঁকা;
  }
#endif
  ossl_provider_cleanup(octx);
}

স্থির শূন্য ossl_close(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  (শূন্য)তথ্য;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
স্থির শূন্য ossl_close_all(কাঠামো Curl_easy *তথ্য)
{
  (শূন্য)তথ্য;
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
স্থির CURLcode ossl_verifyhost(কাঠামো Curl_easy *তথ্য,
                                কাঠামো connectdata *conn,
                                কাঠামো ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  পূর্ণসংখ্যা target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  স্তূপ_OF(GENERAL_NAME) *altnames;
#ifdef USE_IPV6
  কাঠামো in6_addr addr;
#অন্যথা
  কাঠামো in_addr addr;
#endif
  CURLcode ফলাফল = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = strlen(peer->origin->hostname);

  (শূন্য)conn;
  নির্বাচন(peer->type) {
  ক্ষেত্র CURL_SSL_PEER_IPV4:
    যদি(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      ফেরত CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = আকার(কাঠামো in_addr);
    বিরতি;
#ifdef USE_IPV6
  ক্ষেত্র CURL_SSL_PEER_IPV6:
    যদি(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      ফেরত CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = আকার(কাঠামো in6_addr);
    বিরতি;
#endif
  ক্ষেত্র CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    বিরতি;
  পূর্বনির্ধারিত:
    DEBUGASSERT(0);
    failf(তথ্য, "unexpected SSL peer type: %d", (পূর্ণসংখ্যা)peer->type);
    ফেরত CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, ফাঁকা, ফাঁকা);

  যদি(altnames) {
#ifdef HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#অন্যথা
    পূর্ণসংখ্যা numalts;
    পূর্ণসংখ্যা i;
#endif

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_অঙ্ক(altnames);

    /* loop through all alternatives - until a dnsmatch */
    জন্য(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ধ্রুবক GENERAL_NAME *check = sk_GENERAL_NAME_মান(altnames, i);

      যদি(check->type == GEN_DNS)
        dNSName = TRUE;
      অন্যথা যদি(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      যদি(check->type == target) {
        /* get data and length */
        ধ্রুবক অক্ষর *altptr = (ধ্রুবক অক্ষর *)ASN1_STRING_get0_তথ্য(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_দৈর্ঘ্য(check->d.ia5);

        নির্বাচন(target) {
        ক্ষেত্র GEN_DNS: /* name/pattern comparison */
          যদি(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(তথ্য, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (পূর্ণসংখ্যা)altlen, altptr);
          }
          বিরতি;

        ক্ষেত্র GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          যদি((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(তথ্য, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          বিরতি;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  যদি(matched)
    /* an alternative name matched */
    ;
  অন্যথা যদি(dNSName || iPAddress) {
    ধ্রুবক অক্ষর *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(তথ্য, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(তথ্য, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
  }
  অন্যথা {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    পূর্ণসংখ্যা i = -1;
    unsigned অক্ষর *cn = ফাঁকা;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ধ্রুবক X509_NAME *name = X509_get_subject_name(server_cert);
    যদি(name) {
      পূর্ণসংখ্যা j;
      যখন((j = X509_NAME_get_সূচক_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    যদি(i >= 0) {
      ধ্রুবক ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_তথ্য(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      যদি(tmp) {
        যদি(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_দৈর্ঘ্য(tmp);
          cn = (unsigned অক্ষর *)CURL_UNCONST(ASN1_STRING_get0_তথ্য(tmp));
        }
        অন্যথা { /* not a UTF8 name */
          ধ্রুবক পূর্ণসংখ্যা utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          যদি(utf8len < 0)
            cnlen = 0;
          অন্যথা
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        যদি((cnlen == 0) || !cn)
          ফলাফল = CURLE_OUT_OF_MEMORY;
        অন্যথা যদি(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(তথ্য, "SSL: illegal cert name field");
          ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    যদি(ফলাফল)
      /* error already detected, pass through */
      ;
    অন্যথা যদি(!cn) {
      failf(তথ্য, "SSL: unable to obtain common name from peer certificate");
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    }
    অন্যথা যদি(!Curl_cert_hostcheck((ধ্রুবক অক্ষর *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(তথ্য, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (পূর্ণসংখ্যা)cnlen, cn,
            peer->origin->user_hostname);
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    }
    অন্যথা {
      infof(তথ্য, " common name: %.*s (matched)", (পূর্ণসংখ্যা)cnlen, cn);
    }
    যদি(free_cn)
      OPENSSL_free(cn);
  }

  ফেরত ফলাফল;
}

#ifndef OPENSSL_NO_OCSP
স্থির CURLcode verifystatus(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             কাঠামো ossl_ctx *octx)
{
  পূর্ণসংখ্যা i, ocsp_status;
#ifdef HAVE_BORINGSSL_LIKE
  ধ্রুবক uint8_t *status;
#অন্যথা
  unsigned অক্ষর *status;
#endif
  ধ্রুবক unsigned অক্ষর *p;
  CURLcode ফলাফল = CURLE_OK;
  OCSP_RESPONSE *rsp = ফাঁকা;
  OCSP_BASICRESP *br = ফাঁকা;
  X509_STORE     *st = ফাঁকা;
  স্তূপ_OF(X509) *ch = ফাঁকা;
  X509 *cert;
  OCSP_CERTID *id = ফাঁকা;
  পূর্ণসংখ্যা cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  পূর্ণসংখ্যা ret;
  long len;

  (শূন্য)cf;
  DEBUGASSERT(octx);

  len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  যদি(!status) {
    failf(তথ্য, "No OCSP response received");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(ফাঁকা, &p, len);
  যদি(!rsp) {
    failf(তথ্য, "Invalid OCSP response");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }

  ocsp_status = OCSP_response_status(rsp);
  যদি(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(তথ্য, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }

  br = OCSP_response_get1_basic(rsp);
  যদি(!br) {
    failf(তথ্য, "Invalid OCSP response");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }

#যদি !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_সংখ্যা) && LIBRESSL_VERSION_সংখ্যা < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#অন্যথা
  ch = SSL_get_peer_cert_chain(octx->ssl);
#endif
  যদি(!ch) {
    failf(তথ্য, "Could not get peer certificate chain");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  যদি(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(তথ্য, "OCSP response verification failed");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  যদি(!cert) {
    failf(তথ্য, "Error getting peer certificate");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }

  জন্য(i = 0; i < (পূর্ণসংখ্যা)sk_X509_অঙ্ক(ch); i++) {
    X509 *issuer = sk_X509_মান(ch, (ossl_valsize_t)i);
    যদি(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      বিরতি;
    }
  }
  X509_free(cert);

  যদি(!id) {
    failf(তথ্য, "Error computing OCSP ID");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  যদি(ret != 1) {
    failf(তথ্য, "Could not find certificate ID in OCSP response");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
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
    failf(তথ্য, "OCSP response has expired");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }

  infof(তথ্য, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  নির্বাচন(cert_status) {
  ক্ষেত্র V_OCSP_CERTSTATUS_GOOD:
    বিরতি;

  ক্ষেত্র V_OCSP_CERTSTATUS_REVOKED:
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    failf(তথ্য, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    যাও end;

  ক্ষেত্র V_OCSP_CERTSTATUS_UNKNOWN:
  পূর্বনির্ধারিত:
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও end;
  }

end:
  যদি(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  ফেরত ফলাফল;
}
#endif

স্থির ধ্রুবক অক্ষর *ssl_বার্তা_type(পূর্ণসংখ্যা ssl_ver, পূর্ণসংখ্যা বার্তা)
{
  যদি(ssl_ver == SSL3_VERSION_MAJOR) {
    নির্বাচন(বার্তা) {
    ক্ষেত্র SSL3_MT_HELLO_REQUEST:
      ফেরত "Hello request";
    ক্ষেত্র SSL3_MT_CLIENT_HELLO:
      ফেরত "Client hello";
    ক্ষেত্র SSL3_MT_SERVER_HELLO:
      ফেরত "Server hello";
#ifdef SSL3_MT_NEWSESSION_TICKET
    ক্ষেত্র SSL3_MT_NEWSESSION_TICKET:
      ফেরত "Newsession Ticket";
#endif
    ক্ষেত্র SSL3_MT_CERTIFICATE:
      ফেরত "Certificate";
    ক্ষেত্র SSL3_MT_SERVER_চাবি_EXCHANGE:
      ফেরত "Server key exchange";
    ক্ষেত্র SSL3_MT_CLIENT_চাবি_EXCHANGE:
      ফেরত "Client key exchange";
    ক্ষেত্র SSL3_MT_CERTIFICATE_REQUEST:
      ফেরত "Request CERT";
    ক্ষেত্র SSL3_MT_SERVER_DONE:
      ফেরত "Server finished";
    ক্ষেত্র SSL3_MT_CERTIFICATE_VERIFY:
      ফেরত "CERT verify";
    ক্ষেত্র SSL3_MT_FINISHED:
      ফেরত "Finished";
#ifdef SSL3_MT_CERTIFICATE_STATUS
    ক্ষেত্র SSL3_MT_CERTIFICATE_STATUS:
      ফেরত "Certificate Status";
#endif
#ifdef SSL3_MT_ENCRYPTED_EXTENSIONS
    ক্ষেত্র SSL3_MT_ENCRYPTED_EXTENSIONS:
      ফেরত "Encrypted Extensions";
#endif
#ifdef SSL3_MT_SUPPLEMENTAL_তথ্য
    ক্ষেত্র SSL3_MT_SUPPLEMENTAL_তথ্য:
      ফেরত "Supplemental data";
#endif
#ifdef SSL3_MT_END_OF_EARLY_তথ্য
    ক্ষেত্র SSL3_MT_END_OF_EARLY_তথ্য:
      ফেরত "End of early data";
#endif
#ifdef SSL3_MT_চাবি_UPDATE
    ক্ষেত্র SSL3_MT_চাবি_UPDATE:
      ফেরত "Key update";
#endif
#ifdef SSL3_MT_পরবর্তী_PROTO
    ক্ষেত্র SSL3_MT_পরবর্তী_PROTO:
      ফেরত "Next protocol";
#endif
#ifdef SSL3_MT_MESSAGE_HASH
    ক্ষেত্র SSL3_MT_MESSAGE_HASH:
      ফেরত "Message hash";
#endif
    }
  }
  ফেরত "Unknown";
}

স্থির ধ্রুবক অক্ষর *tls_rt_type(পূর্ণসংখ্যা type)
{
  নির্বাচন(type) {
#ifdef SSL3_RT_HEADER
  ক্ষেত্র SSL3_RT_HEADER:
    ফেরত "TLS header";
#endif
  ক্ষেত্র SSL3_RT_CHANGE_CIPHER_SPEC:
    ফেরত "TLS change cipher";
  ক্ষেত্র SSL3_RT_ALERT:
    ফেরত "TLS alert";
  ক্ষেত্র SSL3_RT_HANDSHAKE:
    ফেরত "TLS handshake";
  ক্ষেত্র SSL3_RT_APPLICATION_তথ্য:
    ফেরত "TLS app data";
  পূর্বনির্ধারিত:
    ফেরত "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
স্থির শূন্য ossl_trace(পূর্ণসংখ্যা direction, পূর্ণসংখ্যা ssl_ver, পূর্ণসংখ্যা content_type,
                       ধ্রুবক শূন্য *buf, size_t len, SSL *ssl,
                       শূন্য *userp)
{
  ধ্রুবক অক্ষর *verstr;
  কাঠামো Curl_cfilter *cf = userp;
  কাঠামো Curl_easy *তথ্য = ফাঁকা;
  অক্ষর unknown[32];

  যদি(!cf)
    ফেরত;
  তথ্য = CF_তথ্য_CURRENT(cf);
  যদি(!তথ্য || !তথ্য->set.fdebug || (direction && direction != 1))
    ফেরত;

  নির্বাচন(ssl_ver) {
#ifdef SSL3_VERSION
  ক্ষেত্র SSL3_VERSION:
    verstr = "SSLv3";
    বিরতি;
#endif
  ক্ষেত্র TLS1_VERSION:
    verstr = "TLSv1.0";
    বিরতি;
#ifdef TLS1_1_VERSION
  ক্ষেত্র TLS1_1_VERSION:
    verstr = "TLSv1.1";
    বিরতি;
#endif
#ifdef TLS1_2_VERSION
  ক্ষেত্র TLS1_2_VERSION:
    verstr = "TLSv1.2";
    বিরতি;
#endif
  ক্ষেত্র TLS1_3_VERSION:
    verstr = "TLSv1.3";
    বিরতি;
  পূর্বনির্ধারিত:
    curl_msnprintf(unknown, আকার(unknown), "(%x)", (unsigned পূর্ণসংখ্যা)ssl_ver);
    verstr = unknown;
    বিরতি;
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
    ধ্রুবক অক্ষর *বার্তা_name = "Truncated message";
    ধ্রুবক অক্ষর *tls_rt_name;
    অক্ষর ssl_buf[1024];
    পূর্ণসংখ্যা বার্তা_type = 0;
    পূর্ণসংখ্যা txt_len;

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
        বার্তা_type = *(ধ্রুবক unsigned অক্ষর *)buf;
        বার্তা_name = "Change cipher spec";
      }
    }
    অন্যথা যদি(content_type == SSL3_RT_ALERT) {
      যদি(len >= 2) {
        বার্তা_type =
          (((ধ্রুবক unsigned অক্ষর *)buf)[0] << 8) +
           ((ধ্রুবক unsigned অক্ষর *)buf)[1];
        বার্তা_name = SSL_alert_desc_string_long(বার্তা_type);
      }
    }
    অন্যথা যদি(len) {
      বার্তা_type = *(ধ্রুবক unsigned অক্ষর *)buf;
      বার্তা_name = ssl_বার্তা_type(ssl_ver, বার্তা_type);
    }

    txt_len = curl_msnprintf(ssl_buf, আকার(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, বার্তা_name, বার্তা_type);
    Curl_debug(তথ্য, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(তথ্য, (direction == 1) ? CURLINFO_SSL_তথ্য_OUT :
             CURLINFO_SSL_তথ্য_IN, (ধ্রুবক অক্ষর *)buf, len);
  (শূন্য)ssl;
}

স্থির CURLcode ossl_set_ssl_version_min_max(কাঠামো Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             unsigned পূর্ণসংখ্যা ssl_version_min)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
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
  নির্বাচন(curl_ssl_version_min) {
  ক্ষেত্র CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  ক্ষেত্র CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    বিরতি;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    বিরতি;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    বিরতি;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    বিরতি;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (long)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  নির্বাচন(curl_ssl_version_max) {
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    বিরতি;
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    বিরতি;
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    বিরতি;
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    বিরতি;
  ক্ষেত্র CURL_SSLVERSION_MAX_NONE:  /* none selected */
  ক্ষেত্র CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  পূর্বনির্ধারিত:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    বিরতি;
  }

  যদি(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    ফেরত CURLE_SSL_CONNECT_ERROR;

  ফেরত CURLE_OK;
}

CURLcode Curl_ossl_add_session(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো ossl_ctx *octx,
                               ধ্রুবক অক্ষর *ssl_peer_চাবি,
                               SSL_SESSION *session,
                               ধ্রুবক অক্ষর *alpn,
                               unsigned অক্ষর *quic_tp,
                               size_t quic_tp_len,
                               কাঠামো Curl_ssl_session **psession)
{
  কাঠামো Curl_ssl_session *sc_session = ফাঁকা, *sc_dup = ফাঁকা;
  unsigned অক্ষর *der_session_buf = ফাঁকা;
  unsigned অক্ষর *qtp_clone = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  যদি(psession)
    *psession = ফাঁকা;
  যদি(!cf || !তথ্য)
    যাও out;

  যদি(Curl_ssl_scache_use(cf, তথ্য)) {
    size_t der_session_আকার;
    unsigned অক্ষর *der_session_ptr;
    size_t earlydata_max = 0;
    পূর্ণসংখ্যা ietf_tls_id = SSL_version(octx->ssl);

    der_session_আকার = i2d_SSL_SESSION(session, ফাঁকা);
    যদি(der_session_আকার == 0) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_আকার);
    যদি(!der_session_buf) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }

    der_session_আকার = i2d_SSL_SESSION(session, &der_session_ptr);
    যদি(der_session_আকার == 0) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }

#ifdef HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_তথ্য(session);
#endif
    যদি(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ধ্রুবক অক্ষর *)quic_tp, quic_tp_len);
      যদি(!qtp_clone) {
        ফলাফল = CURLE_OUT_OF_MEMORY;
        যাও out;
      }
    }

    ফলাফল = Curl_ssl_session_create2(der_session_buf, der_session_আকার,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(ফাঁকা) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = ফাঁকা;  /* took ownership of sdata */
#ifdef USE_APPLE_SECTRUST
    যদি(!ফলাফল)
      sc_session->sectrust_verified = octx->sectrust_verified;
#endif
    যদি(!ফলাফল && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        ফলাফল = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    যদি(!ফলাফল) {
      ফলাফল = Curl_ssl_scache_put(cf, তথ্য, ssl_peer_চাবি, sc_session);
      /* took ownership of `sc_session` */
      sc_session = ফাঁকা;
    }
  }

out:
  curlx_free(der_session_buf);
  যদি(!ফলাফল && psession) {
    *psession = sc_dup;
    sc_dup = ফাঁকা;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ফেরত ফলাফল;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
স্থির পূর্ণসংখ্যা ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  কাঠামো Curl_cfilter *cf = (কাঠামো Curl_cfilter *)SSL_get_app_তথ্য(ssl);
  যদি(cf) {
    কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
    কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
    কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
    কাঠামো Curl_ssl_session *session = ফাঁকা;
    Curl_ossl_add_session(cf, তথ্য, octx, connssl->peer.scache_চাবি,
                          ssl_sessionid, connssl->negotiated.alpn, ফাঁকা,
                          0, &session);
    যদি(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  ফেরত 0;
}

স্থির CURLcode load_cacert_from_memory(X509_STORE *store,
                                        ধ্রুবক কাঠামো curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = ফাঁকা;
  স্তূপ_OF(X509_INFO) *inf = ফাঁকা;

  /* everything else is a reference */
  পূর্ণসংখ্যা i, গুনতি = 0;
  X509_INFO *itmp = ফাঁকা;

  যদি(ca_info_blob->len > (size_t)INT_MAX)
    ফেরত CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->তথ্য, (পূর্ণসংখ্যা)ca_info_blob->len);
  যদি(!cbio)
    ফেরত CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, ফাঁকা, ফাঁকা, ফাঁকা);
  যদি(!inf) {
    BIO_free(cbio);
    ফেরত CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  জন্য(i = 0; i < (পূর্ণসংখ্যা)sk_X509_INFO_অঙ্ক(inf); ++i) {
    itmp = sk_X509_INFO_মান(inf, (ossl_valsize_t)i);
    যদি(itmp->x509) {
      যদি(X509_STORE_add_cert(store, itmp->x509)) {
        ++গুনতি;
      }
      অন্যথা {
        /* set count to 0 to return an error */
        গুনতি = 0;
        বিরতি;
      }
    }
    যদি(itmp->crl) {
      যদি(X509_STORE_add_crl(store, itmp->crl)) {
        ++গুনতি;
      }
      অন্যথা {
        /* set count to 0 to return an error */
        গুনতি = 0;
        বিরতি;
      }
    }
  }

#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ঠেলো
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_INFO_বের_করো_free(inf, X509_INFO_free);
#যদি defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic বের_করো
#endif
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  ফেরত (গুনতি > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ifdef USE_WIN32_CRYPTO
স্থির CURLcode ossl_win_load_store(কাঠামো Curl_easy *তথ্য,
                                    কাঠামো Curl_cfilter *cf,
                                    ধ্রুবক অক্ষর *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode ফলাফল = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  যদি(hStore) {
    PCCERT_CONTEXT pContext = ফাঁকা;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = ফাঁকা;
    DWORD enhkey_usage_আকার = 0;
    VERBOSE(size_t যোগ = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    ফলাফল = CURLE_OK;
    জন্য(;;) {
      X509 *x509;
      FILETIME now;
      BYTE চাবি_usage[2];
      DWORD req_আকার;
      ধ্রুবক unsigned অক্ষর *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      যদি(!pContext)
        বিরতি;

      VERBOSE(++যোগ);

#যদি defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        অক্ষর cert_name[256];
        যদি(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               ফাঁকা, cert_name, আকার(cert_name)))
          infof(তথ্য, "SSL: unknown cert name");
        অন্যথা
          infof(তথ্য, "SSL: Checking cert \"%s\"", cert_name);
      }
#endif
      encoded_cert = (ধ্রুবক unsigned অক্ষর *)pContext->pbCertEncoded;
      যদি(!encoded_cert)
        চালু;

      GetSystemTimeAsFileTime(&now);
      যদি(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        চালু;

      /* If key usage exists check for signing attribute */
      যদি(Cert_Get_Intended_চাবি_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 চাবি_usage, আকার(চাবি_usage))) {
        যদি(!(চাবি_usage[0] & CERT_চাবি_CERT_SIGN_চাবি_USAGE))
          চালু;
      }
      অন্যথা যদি(GetLastError())
        চালু;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      যদি(Cert_Get_Enhanced_চাবি_Usage(pContext, 0, ফাঁকা, &req_আকার) && req_আকার) {
        যদি(req_আকার > enhkey_usage_আকার) {
          শূন্য *tmp = curlx_realloc(enhkey_usage, req_আকার);

          যদি(!tmp) {
            failf(তথ্য, "SSL: Out of memory allocating for OID list");
            ফলাফল = CURLE_OUT_OF_MEMORY;
            বিরতি;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_আকার = req_আকার;
        }

        যদি(Cert_Get_Enhanced_চাবি_Usage(pContext, 0, enhkey_usage, &req_আকার)) {
          যদি(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            যদি((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              চালু;
          }
          অন্যথা {
            DWORD i;
            bool found = FALSE;

            জন্য(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              যদি(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                বিরতি;
              }
            }

            যদি(!found)
              চালু;
          }
        }
        অন্যথা
          চালু;
      }
      অন্যথা
        চালু;

      x509 = d2i_X509(ফাঁকা, &encoded_cert, (long)pContext->cbCertEncoded);
      যদি(!x509)
        চালু;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      যদি(X509_STORE_add_cert(store, x509) == 1) {
        VERBOSE(++imported);
#ifdef DEBUGBUILD
        infof(তথ্য, "SSL: Imported cert");
#endif
        *padded = TRUE;
      }
      X509_free(x509);
    }

    curlx_free(enhkey_usage);
    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    CURL_TRC_CF(তথ্য, cf,
                "ossl_win_load_store() found: %zu imported: %zu certs in %s.",
                যোগ, imported, win_store);

    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  ফেরত ফলাফল;
}

স্থির CURLcode ossl_windows_load_anchors(কাঠামো Curl_cfilter *cf,
                                          কাঠামো Curl_easy *তথ্য,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  স্থির ধ্রুবক অক্ষর * ধ্রুবক win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode ফলাফল = CURLE_OK;

  *padded = FALSE;
  জন্য(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    ফলাফল = ossl_win_load_store(তথ্য, cf, win_stores[i], store, &store_added);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    যদি(store_added) {
      CURL_TRC_CF(তথ্য, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    অন্যথা
      infof(তথ্য, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  ফেরত ফলাফল;
}

#endif /* USE_WIN32_CRYPTO */

স্থির CURLcode ossl_load_trust_anchors(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো ossl_ctx *octx,
                                        X509_STORE *store)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর * ধ্রুবক ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? ফাঁকা : conn_config->CAfile);
  ধ্রুবক অক্ষর * ধ্রুবক ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  যদি(conn_config->native_ca_store) {
#ifdef USE_WIN32_CRYPTO
    bool added = FALSE;
    ফলাফল = ossl_windows_load_anchors(cf, তথ্য, store, &added);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    যদি(added) {
      infof(তথ্য, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#নতুবা_যদি defined(USE_APPLE_SECTRUST)
    infof(তথ্য, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#endif
  }

  যদি(conn_config->ca_info_blob) {
    ফলাফল = load_cacert_from_memory(store, conn_config->ca_info_blob);
    যদি(ফলাফল) {
      failf(তথ্য, "error adding trust anchors from certificate blob: %d",
            (পূর্ণসংখ্যা)ফলাফল);
      ফেরত ফলাফল;
    }
    infof(তথ্য, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  যদি(ssl_cafile || ssl_capath) {
#ifdef HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    যদি(ssl_cafile) {
      যদি(!X509_STORE_load_file(store, ssl_cafile)) {
        যদি(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(তথ্য, "error adding trust anchors from file: %s", ssl_cafile);
          ফেরত CURLE_SSL_CACERT_BADFILE;
        }
        অন্যথা
          infof(তথ্য, "error setting certificate file, continuing anyway");
      }
      infof(তথ্য, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    যদি(ssl_capath) {
      যদি(!X509_STORE_load_path(store, ssl_capath)) {
        যদি(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(তথ্য, "error adding trust anchors from path: %s", ssl_capath);
          ফেরত CURLE_SSL_CACERT_BADFILE;
        }
        অন্যথা
          infof(তথ্য, "error setting certificate path, continuing anyway");
      }
      infof(তথ্য, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#অন্যথা
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    যদি(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      যদি(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(তথ্য, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        ফেরত CURLE_SSL_CACERT_BADFILE;
      }
      অন্যথা {
        infof(তথ্য, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    যদি(ssl_cafile)
      infof(তথ্য, "  CAfile: %s", ssl_cafile);
    যদি(ssl_capath)
      infof(তথ্য, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#endif
  }

#ifdef CURL_CA_FALLBACK
  যদি(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(তথ্য, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#endif
  যদি(octx->store_is_empty && !have_native_check)
    infof(তথ্য, "  no trust anchors configured");

  ফেরত ফলাফল;
}

স্থির CURLcode ossl_populate_x509_store(কাঠামো Curl_cfilter *cf,
                                         কাঠামো Curl_easy *তথ্য,
                                         কাঠামো ossl_ctx *octx,
                                         X509_STORE *store)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ফলাফল = CURLE_OK;
  X509_LOOKUP *lookup = ফাঁকা;
  ধ্রুবক অক্ষর * ধ্রুবক ssl_crlfile = conn_config->CRLfile;
  unsigned long x509flags = 0;

  CURL_TRC_CF(তথ্য, cf, "configuring OpenSSL's x509 trust store");
  যদি(!store)
    ফেরত CURLE_OUT_OF_MEMORY;

  যদি(!conn_config->verifypeer) {
    infof(তথ্য, "SSL Trust: peer verification disabled");
    ফেরত CURLE_OK;
  }

  infof(তথ্য, "SSL Trust Anchors:");
  ফলাফল = ossl_load_trust_anchors(cf, তথ্য, octx, store);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  /* Does not make sense to load a CRL file without peer verification */
#ifdef USE_APPLE_SECTRUST
  যদি(ssl_crlfile && conn_config->native_ca_store) {
    failf(তথ্য, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ফেরত CURLE_NOT_BUILT_IN;
  }
#endif
  যদি(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    যদি(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(তথ্য, "error loading CRL file: %s", ssl_crlfile);
      ফেরত CURLE_SSL_CRL_BADFILE;
    }
    x509flags = X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL;
    infof(তথ্য, " CRLfile: %s", ssl_crlfile);
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

  ফেরত ফলাফল;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_OSSL_X509_চাবি  "tls:ossl:x509:share"

কাঠামো ossl_x509_share {
  অক্ষর *CAfile;         /* CAfile path used to generate X509 store */
  অক্ষর *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  কাঠামো curltime time; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

স্থির শূন্য oss_x509_share_free(ধ্রুবক শূন্য *চাবি, size_t চাবি_len, শূন্য *p)
{
  কাঠামো ossl_x509_share *share = p;
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

স্থির bool ossl_cached_x509_store_expired(কাঠামো Curl_easy *তথ্য,
                                           ধ্রুবক কাঠামো ossl_x509_share *mb)
{
  যদি(তথ্য->set.ssl_ca_cache_timeout < 0)
    ফেরত FALSE;
  অন্যথা {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &mb->time);
    timediff_t timeout_ms = তথ্য->set.ssl_ca_cache_timeout * (timediff_t)1000;

    ফেরত elapsed_ms >= timeout_ms;
  }
}

স্থির bool ossl_cached_x509_store_different(কাঠামো Curl_cfilter *cf,
                                             ধ্রুবক কাঠামো Curl_easy *তথ্য,
                                             ধ্রুবক কাঠামো ossl_x509_share *mb)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (শূন্য)তথ্য;
  যদি(mb->no_partialchain != conn_config->no_partialchain)
    ফেরত TRUE;
  যদি(!mb->CAfile || !conn_config->CAfile) {
    যদি(mb->CAfile != conn_config->CAfile)
      ফেরত TRUE;
  }
  অন্যথা যদি(strcmp(mb->CAfile, conn_config->CAfile))
    ফেরত TRUE;

  যদি(!mb->CApath || !conn_config->CApath) {
    যদি(mb->CApath != conn_config->CApath)
      ফেরত TRUE;
  }
  অন্যথা যদি(strcmp(mb->CApath, conn_config->CApath))
    ফেরত TRUE;

  ফেরত FALSE;
}

স্থির X509_STORE *ossl_get_cached_x509_store(কাঠামো Curl_cfilter *cf,
                                              কাঠামো Curl_easy *তথ্য,
                                              bool *pempty)
{
  কাঠামো Curl_multi *multi = তথ্য->multi;
  কাঠামো ossl_x509_share *share;
  X509_STORE *store = ফাঁকা;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_চাবি,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_চাবি)) : ফাঁকা;
  যদি(share && share->store &&
     !ossl_cached_x509_store_expired(তথ্য, share) &&
     !ossl_cached_x509_store_different(cf, তথ্য, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  ফেরত store;
}

স্থির শূন্য ossl_set_cached_x509_store(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       X509_STORE *store,
                                       bool is_empty)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো Curl_multi *multi = তথ্য->multi;
  কাঠামো ossl_x509_share *share;

  DEBUGASSERT(multi);
  যদি(!multi)
    ফেরত;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_চাবি,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_চাবি));

  যদি(!share) {
    share = curlx_calloc(1, আকার(*share));
    যদি(!share)
      ফেরত;
    যদি(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_চাবি,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_চাবি),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      ফেরত;
    }
  }

  যদি(X509_STORE_up_ref(store)) {
    অক্ষর *CAfile = ফাঁকা;
    অক্ষর *CApath = ফাঁকা;

    যদি(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      যদি(!CAfile) {
        X509_STORE_free(store);
        ফেরত;
      }
    }

    যদি(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      যদি(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        ফেরত;
      }
    }

    যদি(share->store) {
      X509_STORE_free(share->store);
      curlx_free(share->CAfile);
      curlx_free(share->CApath);
    }

    share->time = *Curl_pgrs_now(তথ্য);
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

CURLcode Curl_ssl_setup_x509_store(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ossl_ctx *octx)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ফলাফল = CURLE_OK;
  X509_STORE *cached_store;
  bool cache_criteria_met, is_empty;

  /* Consider the X509 store cacheable if it comes from CAfile/CApath,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (তথ্য->set.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !USING_PROVIDER(octx);

  ERR_set_mark();

  cached_store = ossl_get_cached_x509_store(cf, তথ্য, &is_empty);
  যদি(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  অন্যথা {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    ফলাফল = ossl_populate_x509_store(cf, তথ্য, octx, store);
    যদি(ফলাফল == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, তথ্য, store, (bool)octx->store_is_empty);
    }
  }

  ERR_বের_করো_to_mark();

  ফেরত ফলাফল;
}

স্থির bool ossl_apply_session(
  কাঠামো ossl_ctx *octx,
  কাঠামো Curl_cfilter *cf,
  কাঠামো Curl_easy *তথ্য,
  কাঠামো alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  কাঠামো Curl_ssl_session *scs)
{
  কাঠামো ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ধ্রুবক unsigned অক্ষর *der_sessionid = scs->sdata;
  size_t der_sessionid_আকার = scs->sdata_len;
  SSL_SESSION *ssl_session = ফাঁকা;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(ফাঁকা, &der_sessionid,
                                (long)der_sessionid_আকার);
  যদি(ssl_session) {
    যদি(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(অক্ষর error_বাফার[256]);
      infof(তথ্য, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_বাফার,
                          আকার(error_বাফার)));
    }
    অন্যথা {
      যদি(conn_cfg->verifypeer &&
         (SSL_get_verify_ফলাফল(octx->ssl) != X509_V_OK)
#ifdef USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#endif
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, ফাঁকা);
        infof(তথ্য, "SSL session not peer verified, not reusing");
      }
      অন্যথা {
        infof(তথ্য, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ifdef USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#endif
        infof(তথ্য, "SSL verify result: %lx",
              (unsigned long)SSL_get_verify_ফলাফল(octx->ssl));
#ifdef HAVE_OPENSSL_EARLYDATA
        যদি(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_তথ্য(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_তথ্য = FALSE;
          যদি(sess_reuse_cb)
            (শূন্য)sess_reuse_cb(cf, তথ্য, alpns, scs, &do_early_তথ্য);
          যদি(do_early_তথ্য) {
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
    infof(তথ্য, "SSL session not accepted by OpenSSL, continuing without");
  }
  ফেরত (bool)octx->reused_session;
}

স্থির CURLcode ossl_init_session_and_alpns(
  কাঠামো ossl_ctx *octx,
  কাঠামো Curl_cfilter *cf,
  কাঠামো Curl_easy *তথ্য,
  কাঠামো ssl_peer *peer,
  ধ্রুবক কাঠামো alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  কাঠামো ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো alpn_spec alpns;
  CURLcode ফলাফল;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  যদি((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    কাঠামো Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(তথ্য, cf->cft, FIRSTSOCKET);
    যদি(scs) {
      যদি(ossl_apply_session(octx, cf, তথ্য, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(তথ্য, cf, "applied SSL session from control connection");
    }
  }

  যদি(!octx->reused_session &&
     Curl_ssl_scache_use(cf, তথ্য) && !conn_cfg->verifystatus) {
    কাঠামো Curl_ssl_session *scs = ফাঁকা;

    ফলাফল = Curl_ssl_scache_take(cf, তথ্য, peer->scache_চাবি, &scs);
    যদি(!ফলাফল && scs && scs->sdata && scs->sdata_len) {
      (শূন্য)ossl_apply_session(octx, cf, তথ্য, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, তথ্য, peer->scache_চাবি, scs);
  }

  যদি(alpns.গুনতি) {
    কাঠামো alpn_proto_buf proto;
    memset(&proto, 0, আকার(proto));
    ফলাফল = Curl_alpn_to_proto_buf(&proto, &alpns);
    যদি(ফলাফল) {
      failf(তথ্য, "Error determining ALPN");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
    যদি(SSL_set_alpn_protos(octx->ssl, proto.তথ্য, proto.len)) {
      failf(তথ্য, "Error setting ALPN");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }

  ফেরত CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
স্থির CURLcode ossl_init_ech(কাঠামো ossl_ctx *octx,
                              কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              কাঠামো ssl_peer *peer)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  পূর্ণসংখ্যা trying_ech_now = 0;

  যদি(!Curl_ssl_ech_enabled(cf))
    ফেরত CURLE_OK;

  যদি(conn_config->ech == CURLECH_GREASE) {
    infof(তথ্য, "ECH: will GREASE ClientHello");
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
    CURLcode ফলাফল;

    যদি(!conn_config->ech_config[0]) {
      infof(তথ্য, "ECH: ECHConfig from command line empty");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * strlen(conn_config->ech_config);
    ফলাফল = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    যদি(ফলাফল || !ech_config) {
      infof(তথ্য, "ECH: cannot base64 decode ECHConfig from command line");
      যদি(conn_config->ech == CURLECH_HARD)
        ফেরত ফলাফল;
    }
    যদি(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(তথ্য, "ECH: SSL_ECH_set1_ech_config_list failed");
      যদি(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        ফেরত CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#অন্যথা
    size_t ech_config_len = 0;
    যদি(!conn_config->ech_config[0]) {
      infof(তথ্য, "ECH: ECHConfig from command line empty");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = strlen(conn_config->ech_config);
    যদি(SSL_set1_ech_config_list(octx->ssl,
                                (ধ্রুবক uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(তথ্য, "ECH: SSL_ECH_set1_ech_config_list failed");
      যদি(conn_config->ech == CURLECH_HARD)
        ফেরত CURLE_SSL_CONNECT_ERROR;
    }
    অন্যথা
      trying_ech_now = 1;
#endif /* HAVE_BORINGSSL_LIKE */
    infof(তথ্য, "ECH: ECHConfig from command line");
  }
  অন্যথা {
    ধ্রুবক কাঠামো Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(তথ্য, cf->sockindex, peer->origin);

    যদি(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ধ্রুবক unsigned অক্ষর *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(তথ্য, "ECH: ECHConfig from HTTPS RR");
      যদি(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(তথ্য, "ECH: SSL_set1_ech_config_list failed");
        যদি(conn_config->ech == CURLECH_HARD)
          ফেরত CURLE_SSL_CONNECT_ERROR;
      }
      অন্যথা {
        trying_ech_now = 1;
        infof(তথ্য, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    অন্যথা {
      infof(তথ্য, "ECH: requested but no ECHConfig available");
      যদি(conn_config->ech == CURLECH_HARD)
        ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }
#ifdef HAVE_BORINGSSL_LIKE
  (শূন্য)peer;
  যদি(trying_ech_now && conn_config->ech_public) {
    infof(তথ্য, "ECH: setting public_name not supported with BoringSSL");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }
#অন্যথা
  যদি(trying_ech_now && conn_config->ech_public) {
    পূর্ণসংখ্যা ret;
    infof(তথ্য, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    যদি(ret != 1) {
      infof(তথ্য, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }
#endif /* HAVE_BORINGSSL_LIKE */
  যদি(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(তথ্য, "ECH: cannot force TLSv1.3 [ERROR]");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  ফেরত CURLE_OK;
}
#endif /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

স্থির CURLcode ossl_init_ssl(কাঠামো ossl_ctx *octx,
                              কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              কাঠামো ssl_peer *peer,
                              ধ্রুবক কাঠামো alpn_spec *alpns_requested,
                              শূন্য *ssl_user_তথ্য,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  যদি(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  যদি(!octx->ssl) {
    failf(তথ্য, "SSL: could not create a context (handle)");
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_তথ্য(octx->ssl, ssl_user_তথ্য);

#ifndef OPENSSL_NO_OCSP
  যদি(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#endif

  SSL_set_connect_state(octx->ssl);

  যদি(peer->sni) {
    যদি(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(তথ্য, "Failed set SNI");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode ফলাফল = ossl_init_ech(octx, cf, তথ্য, peer);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  ফেরত ossl_init_session_and_alpns(octx, cf, তথ্য, peer,
                                     alpns_requested, sess_reuse_cb);
}

স্থির CURLcode ossl_init_method(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ssl_peer *peer,
                                 ধ্রুবক SSL_METHOD **pmethod,
                                 unsigned পূর্ণসংখ্যা *pssl_version_min)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = ফাঁকা;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  নির্বাচন(peer->transport) {
  ক্ষেত্র TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    নির্বাচন(*pssl_version_min) {
    ক্ষেত্র CURL_SSLVERSION_TLSv1:
    ক্ষেত্র CURL_SSLVERSION_TLSv1_0:
    ক্ষেত্র CURL_SSLVERSION_TLSv1_1:
    ক্ষেত্র CURL_SSLVERSION_TLSv1_2:
    ক্ষেত্র CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      বিরতি;
    ক্ষেত্র CURL_SSLVERSION_SSLv2:
      failf(তথ্য, "No SSLv2 support");
      ফেরত CURLE_NOT_BUILT_IN;
    ক্ষেত্র CURL_SSLVERSION_SSLv3:
      failf(তথ্য, "No SSLv3 support");
      ফেরত CURLE_NOT_BUILT_IN;
    পূর্বনির্ধারিত:
      failf(তথ্য, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
    বিরতি;
  ক্ষেত্র TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    যদি(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(তথ্য, "QUIC needs at least TLS version 1.3");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    বিরতি;
  পূর্বনির্ধারিত:
    failf(তথ্য, "unsupported transport %d in SSL init", peer->transport);
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  ফেরত *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(কাঠামো ossl_ctx *octx,
                            কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো ssl_peer *peer,
                            ধ্রুবক কাঠামো alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            শূন্য *cb_user_তথ্য,
                            Curl_ossl_new_session_cb *cb_new_session,
                            শূন্য *ssl_user_তথ্য,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *ciphers;
  ধ্রুবক SSL_METHOD *req_method = ফাঁকা;
  ctx_option_t ctx_options = 0;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, তথ্য);
  অক্ষর * ধ্রুবক ssl_cert = conn_config->clientcert;
  ধ্রুবক কাঠামো curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ধ্রুবক অক্ষর * ধ্রুবক ssl_cert_type = conn_config->cert_type;
  unsigned পূর্ণসংখ্যা ssl_version_min = CURL_SSLVERSION_DEFAULT;
  অক্ষর error_বাফার[256];

  /* Make funny stuff to get random input */
  ফলাফল = ossl_seed(তথ্য);
  যদি(!ফলাফল) {
    যদি(CURL_EASY_STR(তথ্য, STRING_SSL_ENGINE))
      ফলাফল = ossl_set_engine(তথ্য, octx,
                               CURL_EASY_STR(তথ্য, STRING_SSL_ENGINE));
    যদি(!ফলাফল && তথ্য->set.engine_default)
      ফলাফল = ossl_set_engine_default(তথ্য, octx);
  }

  যদি(!ফলাফল) {
    ssl_config->certverifyresult = !X509_V_OK;
    ফলাফল = ossl_init_method(cf, তথ্য, peer, &req_method, &ssl_version_min);
  }
  যদি(ফলাফল)
    ফেরত ফলাফল;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ifdef OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#endif
    SSL_CTX_new(req_method);

  যদি(!octx->ssl_ctx) {
    failf(তথ্য, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_বাফার, আকার(error_বাফার)));
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(cb_setup) {
    ফলাফল = cb_setup(cf, তথ্য, cb_user_তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(তথ্য->set.fdebug && তথ্য->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_বার্তা_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_বার্তা_callback_arg(octx->ssl_ctx, cf);
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
  নির্বাচন(ssl_version_min) {
  ক্ষেত্র CURL_SSLVERSION_SSLv2:
  ক্ষেত্র CURL_SSLVERSION_SSLv3:
    ফেরত CURLE_NOT_BUILT_IN;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  ক্ষেত্র CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  ক্ষেত্র CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  ক্ষেত্র CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  ক্ষেত্র CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  ক্ষেত্র CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    ফলাফল = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    বিরতি;

  পূর্বনির্ধারিত:
    failf(তথ্য, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ফেরত CURLE_SSL_CONNECT_ERROR;
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
#ifdef HAVE_SSL_CTX_SET_DEFAULT_READ_বাফার_LEN
  SSL_CTX_set_default_read_বাফার_len(octx->ssl_ctx, 0x401e * 4);
#endif

  /* We do retry writes sometimes from another buffer address */
  SSL_CTX_set_mode(octx->ssl_ctx, SSL_MODE_ACCEPT_MOVING_WRITE_বাফার);

  ciphers = conn_config->cipher_list;
  যদি(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = ফাঁকা;
  যদি(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    যদি(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(তথ্য, "failed setting cipher list: %s", ciphers);
      ফেরত CURLE_SSL_CIPHER;
    }
    infof(তথ্য, "Cipher selection: %s", ciphers);
  }

#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    ধ্রুবক অক্ষর *ciphers13 = conn_config->cipher_list13;
    যদি(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      যদি(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(তথ্য, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        ফেরত CURLE_SSL_CIPHER;
      }
      infof(তথ্য, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#endif

  যদি(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    ফলাফল = client_cert(তথ্য, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->চাবি, conn_config->চাবি_blob,
                         conn_config->চাবি_type,
                         conn_config->চাবি_passwd);
    যদি(ফলাফল)
      /* failf() is already done in client_cert() */
      ফেরত ফলাফল;
  }

#ifdef HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#endif

  {
    ধ্রুবক অক্ষর *curves = conn_config->curves;
    যদি(curves) {
#ifdef HAVE_BORINGSSL_LIKE
#define OSSL_CURVE_CAST(x) (x)
#অন্যথা
#define OSSL_CURVE_CAST(x) (অক্ষর *)CURL_UNCONST(x)
#endif
      যদি(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(তথ্য, "failed setting curves list: '%s'", curves);
        ফেরত CURLE_SSL_CIPHER;
      }
    }
  }

#ifdef HAVE_SSL_CTX_SET1_SIGALGS
#define OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    ধ্রুবক অক্ষর *signature_algorithms = conn_config->signature_algorithms;
    যদি(signature_algorithms) {
      যদি(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(তথ্য, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        ফেরত CURLE_SSL_CIPHER;
      }
    }
  }
#endif

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, ফাঁকা);

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
  যদি(তথ্য->set.ssl_fsslctx) {
    কাঠামো Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    যদি(!octx->x509_store_setup) {
      ফলাফল = Curl_ssl_setup_x509_store(cf, তথ্য, octx);
      যদি(ফলাফল)
        ফেরত ফলাফল;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, তথ্য, easy_fsslctx);
    ফলাফল = (*তথ্য->set.ssl_fsslctx)(তথ্য, octx->ssl_ctx,
                                      তথ্য->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    যদি(ফলাফল) {
      failf(তথ্য, "error signaled by SSL ctx callback");
      ফেরত ফলাফল;
    }
  }

  ফেরত ossl_init_ssl(octx, cf, তথ্য, peer, alpns_requested,
                       ssl_user_তথ্য, sess_reuse_cb);
}

স্থির CURLcode ossl_on_session_reuse(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো alpn_spec *alpns,
                                      কাঠামো Curl_ssl_session *scs,
                                      bool *do_early_তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  ফেরত Curl_on_session_reuse(cf, তথ্য, alpns, scs, do_early_তথ্য,
                               connssl->earlydata_max);
}

শূন্য Curl_ossl_report_handshake(কাঠামো Curl_easy *তথ্য, কাঠামো ossl_ctx *octx)
{
#ifdef CURLVERBOSE
  যদি(Curl_trc_is_verbose(তথ্য)) {
    পূর্ণসংখ্যা psigtype_nid = NID_undef;
    ধ্রুবক অক্ষর *negotiated_group_name = ফাঁকা;

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
    infof(তথ্য, "SSL connection using %s / %s / %s / %s",
          SSL_get_version(octx->ssl),
          SSL_get_cipher(octx->ssl),
          negotiated_group_name ? negotiated_group_name : "[blank]",
          OBJ_nid2sn(psigtype_nid));
  }
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)octx;
#endif /* CURLVERBOSE */
}

স্থির CURLcode ossl_connect_step1(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode ফলাফল;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  ফলাফল = Curl_ossl_ctx_init(octx, cf, তথ্য, &connssl->peer,
                              connssl->alpn, ফাঁকা, ফাঁকা,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  octx->bio_method = ossl_bio_cf_method_create();
  যদি(!octx->bio_method)
    ফেরত CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  যদি(!bio)
    ফেরত CURLE_OUT_OF_MEMORY;

  BIO_set_তথ্য(bio, cf);
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
    কাঠামো alpn_proto_buf proto;
    memset(&proto, 0, আকার(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(তথ্য, VTLS_INFOF_ALPN_OFFER_1STR, proto.তথ্য);
  }

  connssl->connecting_state = ssl_connect_2;
  ফেরত CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
স্থির পূর্ণসংখ্যা ossl_trace_ech_retry_configs(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য, SSL *ssl,
                                        পূর্ণসংখ্যা reason)
{
  CURLcode ফলাফল = CURLE_OK;
  size_t rcl = 0;
  পূর্ণসংখ্যা rv = 1;
#ifndef HAVE_BORINGSSL_LIKE
  অক্ষর *inner = ফাঁকা;
  uint8_t *rcs = ফাঁকা;
  অক্ষর *outer = ফাঁকা;
#অন্যথা
  ধ্রুবক অক্ষর *inner = ফাঁকা;
  ধ্রুবক uint8_t *rcs = ফাঁকা;
  ধ্রুবক অক্ষর *outer = ফাঁকা;
  size_t out_name_len = 0;
  পূর্ণসংখ্যা servername_type = 0;
#endif
  NOVERBOSE((শূন্য)reason);

  /* nothing to trace if not doing ECH */
  যদি(!Curl_ssl_ech_enabled(cf))
    ফেরত rv;
#ifndef HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#অন্যথা
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (পূর্ণসংখ্যা)rcl;
#endif

  যদি(rv && rcs) {
    অক্ষর *b64str = ফাঁকা;
    size_t blen = 0;

    ফলাফল = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    যদি(!ফলাফল && b64str) {
      infof(তথ্য, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ifndef HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(তথ্য, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#অন্যথা
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(তথ্য, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#endif
    }
  }
  অন্যথা
    infof(তথ্য, "ECH: no retry_configs (rv = %d)", rv);
#ifndef HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#endif
  ফেরত rv;
}

#endif

স্থির CURLcode ossl_connect_step2(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  কাঠামো ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, তথ্য);
  পূর্ণসংখ্যা err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  যদি(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode ফলাফল = Curl_ssl_setup_x509_store(cf, তথ্য, octx);
    যদি(ফলাফল)
      ফেরত ফলাফল;
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
    পূর্ণসংখ্যা detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(তথ্য, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    যদি(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(তথ্য, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ফেরত CURLE_AGAIN;
    }
    যদি(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(তথ্য, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ফেরত CURLE_AGAIN;
    }
#ifdef SSL_ERROR_WANT_ASYNC
    যদি(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(তথ্য, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ফেরত CURLE_AGAIN;
    }
#endif
#ifdef SSL_ERROR_WANT_RETRY_VERIFY
    যদি(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(তথ্য, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(তথ্য, TRUE);
      ফেরত CURLE_AGAIN;
    }
#endif
    অন্যথা {
      /* untreated error */
      sslerr_t errdetail;
      অক্ষর error_বাফার[256] = "";
      CURLcode ফলাফল;
      long lerr;
      পূর্ণসংখ্যা lib;
      পূর্ণসংখ্যা reason;

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
        ফলাফল = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_ফলাফল(octx->ssl);
        যদি(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(তথ্য, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        অন্যথা
          failf(তথ্য, "%s", "SSL certificate verification failed");
      }
#ifdef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      অন্যথা যদি((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        ফলাফল = CURLE_SSL_CLIENTCERT;
        failf(তথ্য, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_বাফার, আকার(error_বাফার)));
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
        ossl_trace_ech_retry_configs(cf, তথ্য, octx->ssl, reason);

        ফলাফল = CURLE_ECH_REQUIRED;
        failf(তথ্য, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_বাফার, আকার(error_বাফার)));
      }
#endif
      অন্যথা {
        ফলাফল = CURLE_SSL_CONNECT_ERROR;
        failf(তথ্য, "TLS connect error: %s",
              ossl_strerror(errdetail, error_বাফার, আকার(error_বাফার)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      যদি(ফলাফল == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        অক্ষর extramsg[80] = "";
        পূর্ণসংখ্যা sockerr = SOCKERRNO;

        যদি(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, আকার(extramsg));
        failf(তথ্য, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      ফেরত ফলাফল;
    }
  }
  অন্যথা {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(তথ্য, octx);

#যদি defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    যদি(Curl_ssl_ech_enabled(cf)) {
      কাঠামো ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      অক্ষর *inner = ফাঁকা, *outer = ফাঁকা;
      পূর্ণসংখ্যা rv;
      VERBOSE(ধ্রুবক অক্ষর *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      নির্বাচন(rv) {
      ক্ষেত্র SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        বিরতি;
      ক্ষেত্র SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        বিরতি;
      ক্ষেত্র SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        বিরতি;
      ক্ষেত্র SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        বিরতি;
      ক্ষেত্র SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        বিরতি;
      ক্ষেত্র SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        বিরতি;
      ক্ষেত্র SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        বিরতি;
      ক্ষেত্র SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        বিরতি;
      ক্ষেত্র SSL_ECH_STATUS_BAD_NAME: {
        যদি(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        অন্যথা {
          VERBOSE(status = "bad name (unexpected)");
        }
        বিরতি;
      }
      পূর্বনির্ধারিত:
        VERBOSE(status = "unexpected status");
        infof(তথ্য, "ECH: unexpected status %d", rv);
      }
      infof(তথ্য, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      যদি(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, তথ্য, octx->ssl, 0);
      }
      যদি(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(তথ্য, "ECH: ech-hard failed");
        ফেরত CURLE_SSL_CONNECT_ERROR;
      }
    }
    অন্যথা {
      infof(তথ্য, "ECH: result: status is not attempted");
    }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    যদি(connssl->alpn) {
      ধ্রুবক unsigned অক্ষর *neg_protocol;
      unsigned পূর্ণসংখ্যা len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      ফেরত Curl_alpn_set_negotiated(cf, তথ্য, connssl, neg_protocol, len);
    }

    ফেরত CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
স্থির CURLcode ossl_pkp_pin_peer_pubkey(কাঠামো Curl_easy *তথ্য, X509 *cert,
                                         ধ্রুবক অক্ষর *pinnedpubkey)
{
  /* Scratch */
  পূর্ণসংখ্যা len1 = 0, len2 = 0;
  unsigned অক্ষর *buff1 = ফাঁকা, *অস্থায়ী = ফাঁকা;

  /* Result is returned to caller */
  CURLcode ফলাফল = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  যদি(!pinnedpubkey)
    ফেরত CURLE_OK;

  যদি(!cert)
    ফেরত ফলাফল;

  করো {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), ফাঁকা);
    যদি(len1 < 1)
      বিরতি; /* failed */

    buff1 = অস্থায়ী = curlx_malloc(len1);
    যদি(!buff1)
      বিরতি; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &অস্থায়ী);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    যদি((len1 != len2) || !অস্থায়ী || ((অস্থায়ী - buff1) != len1))
      বিরতি; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    ফলাফল = Curl_pin_peer_pubkey(তথ্য, pinnedpubkey, buff1, len1);
  } যখন(0);

  যদি(buff1)
    curlx_free(buff1);

  ফেরত ফলাফল;
}

#ifdef CURLVERBOSE
#যদি !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_সংখ্যা) && LIBRESSL_VERSION_সংখ্যা < 0x3060000fL)
স্থির শূন্য infof_certstack(কাঠামো Curl_easy *তথ্য, ধ্রুবক SSL *ssl)
{
  স্তূপ_OF(X509) *certstack;
  long verify_ফলাফল;
  পূর্ণসংখ্যা অঙ্ক_cert_levels;
  পূর্ণসংখ্যা cert_level;

  যদি(!Curl_trc_is_verbose(তথ্য))
    ফেরত;

  verify_ফলাফল = SSL_get_verify_ফলাফল(ssl);
  যদি(verify_ফলাফল != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  অন্যথা
    certstack = SSL_get0_verified_chain(ssl);
  যদি(!certstack)
    ফেরত;
  অঙ্ক_cert_levels = sk_X509_অঙ্ক(certstack);

  জন্য(cert_level = 0; cert_level < অঙ্ক_cert_levels; cert_level++) {
    অক্ষর cert_algorithm[80] = "";
    অক্ষর group_name_final[80] = "";
    ধ্রুবক X509_ALGOR *palg_cert = ফাঁকা;
    ধ্রুবক ASN1_OBJECT *paobj_cert = ফাঁকা;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    পূর্ণসংখ্যা চাবি_bits;
    পূর্ণসংখ্যা চাবি_sec_bits;
    পূর্ণসংখ্যা get_group_name;
    ধ্রুবক অক্ষর *type_name;

    current_cert = sk_X509_মান(certstack, cert_level);
    যদি(!current_cert)
      চালু;

    current_pkey = X509_get0_pubkey(current_cert);
    যদি(!current_pkey)
      চালু;

    X509_get0_signature(ফাঁকা, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, ফাঁকা, ফাঁকা, palg_cert);
    OBJ_obj2txt(cert_algorithm, আকার(cert_algorithm), paobj_cert, 0);

    চাবি_bits = EVP_PKEY_bits(current_pkey);
#ifndef HAVE_OPENSSL3
#define EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#endif
    চাবি_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#ifdef HAVE_OPENSSL3
    {
      অক্ষর group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               আকার(group_name), ফাঁকা);
      curl_msnprintf(group_name_final, আকার(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#অন্যথা
    get_group_name = 0;
    type_name = ফাঁকা;
#endif

    infof(তথ্য, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          চাবি_bits, চাবি_sec_bits, cert_algorithm);
  }
}
#অন্যথা
#define infof_certstack(তথ্য, ssl)
#endif
#endif /* CURLVERBOSE */

স্থির CURLcode ossl_check_issuer(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  X509 *server_cert)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = ফাঁকা;
  BIO *fp = ফাঁকা;
  অক্ষর err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode ফলাফল = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  যদি(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->তথ্য,
                         (পূর্ণসংখ্যা)conn_config->issuercert_blob->len);
    যদি(!fp) {
      failf(তথ্য, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, আকার(err_buf)));
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }
  }
  অন্যথা যদি(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    যদি(!fp) {
      failf(তথ্য, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, আকার(err_buf)));
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও out;
    }

    যদি(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      যদি(verify_enabled)
        failf(তথ্য, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      ফলাফল = CURLE_SSL_ISSUER_ERROR;
      যাও out;
    }
  }

  যদি(fp) {
    issuer = PEM_read_bio_X509(fp, ফাঁকা, ZERO_NULL, ফাঁকা);
    যদি(!issuer) {
      যদি(verify_enabled)
        failf(তথ্য, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      ফলাফল = CURLE_SSL_ISSUER_ERROR;
      যাও out;
    }

    যদি(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      যদি(verify_enabled)
        failf(তথ্য, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      ফলাফল = CURLE_SSL_ISSUER_ERROR;
      যাও out;
    }

    infof(তথ্য, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  যদি(fp)
    BIO_free(fp);
  যদি(issuer)
    X509_free(issuer);
  ফেরত ফলাফল;
}

স্থির ধ্রুবক অক্ষর *pinned(কাঠামো Curl_cfilter *cf,
                          কাঠামো Curl_easy *তথ্য)
{
  (শূন্য)cf;
  ফেরত
#ifndef CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#endif
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY);
}

স্থির CURLcode ossl_check_pinned_চাবি(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      X509 *server_cert)
{
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক অক্ষর *ptr = pinned(cf, তথ্য);
  যদি(ptr) {
    ফলাফল = ossl_pkp_pin_peer_pubkey(তথ্য, server_cert, ptr);
    যদি(ফলাফল)
      failf(তথ্য, "SSL: public key does not match pinned public key");
  }
  ফেরত ফলাফল;
}

#ifdef CURLVERBOSE
#define MAX_CERT_NAME_দৈর্ঘ্য 2048
স্থির CURLcode ossl_infof_cert(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                X509 *server_cert)
{
  BIO *mem = ফাঁকা;
  কাঠামো dynbuf dname;
  অক্ষর err_buf[256] = "";
  অক্ষর *buf;
  long len;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!Curl_trc_is_verbose(তথ্য))
    ফেরত CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_দৈর্ঘ্য);
  mem = BIO_new(BIO_s_mem());
  যদি(!mem) {
    failf(তথ্য, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, আকার(err_buf)));
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  infof(তথ্য, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  ফলাফল = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(তথ্য, "  subject: %s", ফলাফল ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_তথ্য(mem, (অক্ষর **)&buf);
  infof(তথ্য, "  start date: %.*s", (পূর্ণসংখ্যা)len, buf);
  (শূন্য)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_তথ্য(mem, (অক্ষর **)&buf);
  infof(তথ্য, "  expire date: %.*s", (পূর্ণসংখ্যা)len, buf);
  (শূন্য)BIO_reset(mem);

  ফলাফল = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  যদি(ফলাফল) /* should be only fatal stuff like OOM */
    যাও out;
  infof(তথ্য, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  ফেরত ফলাফল;
}
#endif /* CURLVERBOSE */

#ifdef USE_APPLE_SECTRUST
কাঠামো ossl_certs_ctx {
  স্তূপ_OF(X509) *sk;
  size_t অঙ্ক_certs;
  unsigned অক্ষর *last_der;
};

স্থির CURLcode ossl_chain_get_der(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   শূন্য *user_তথ্য,
                                   size_t i,
                                   unsigned অক্ষর **pder,
                                   size_t *pder_len)
{
  কাঠামো ossl_certs_ctx *chain = user_তথ্য;
  X509 *cert;
  পূর্ণসংখ্যা der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = ফাঁকা;

  (শূন্য)cf;
  (শূন্য)তথ্য;
  *pder_len = 0;
  *pder = ফাঁকা;

  যদি(i >= chain->অঙ্ক_certs)
    ফেরত CURLE_TOO_LARGE;
  cert = sk_X509_মান(chain->sk, (পূর্ণসংখ্যা)i);
  যদি(!cert)
    ফেরত CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  যদি(der_len < 0)
    ফেরত CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (size_t)der_len;
  ফেরত CURLE_OK;
}

স্থির CURLcode ossl_apple_verify(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ossl_ctx *octx,
                                  কাঠামো ssl_peer *peer)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো ossl_certs_ctx chain;
  CURLcode ফলাফল;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, আকার(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.অঙ্ক_certs = chain.sk ? sk_X509_অঙ্ক(chain.sk) : 0;

  যদি(!chain.অঙ্ক_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    যদি(!octx->reused_session) {
      failf(তথ্য, "SSL: could not get peer certificate chain");
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    }
    অন্যথা {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(তথ্য, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      ফেরত CURLE_OK;
    }
  }
  অন্যথা {
#ifdef HAVE_BORINGSSL_LIKE
    ধ্রুবক uint8_t *ocsp_তথ্য = ফাঁকা;
#অন্যথা
    unsigned অক্ষর *ocsp_তথ্য = ফাঁকা;
#endif
    long ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    যদি(!octx->reused_session)
      ocsp_len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_তথ্য);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    যদি(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_তথ্য = ফাঁকা;
    }

    ফলাফল = Curl_vtls_apple_verify(cf, তথ্য, peer, chain.অঙ্ক_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_তথ্য, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = ফাঁকা;
  }
  octx->sectrust_verified = !ফলাফল;
  ফেরত ফলাফল;
}
#endif /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ossl_ctx *octx,
                                   কাঠামো ssl_peer *peer)
{
  কাঠামো connectdata *conn = cf->conn;
  কাঠামো ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, তথ্য);
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ফলাফল = CURLE_OK;
  long ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  যদি(তথ্য->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    ফলাফল = ossl_certchain(তথ্য, octx->ssl);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  যদি(!server_cert) {
    /* no verification at all, this maybe acceptable */
    যদি(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, তথ্য))
      যাও out;

    failf(তথ্য, "SSL: could not get peer certificate");
    ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    যাও out;
  }

#ifdef CURLVERBOSE
  ফলাফল = ossl_infof_cert(cf, তথ্য, server_cert);
  যদি(ফলাফল)
    যাও out;
  infof_certstack(তথ্য, octx->ssl);
#endif

  যদি(conn_config->verifyhost) {
    ফলাফল = ossl_verifyhost(তথ্য, conn, peer, server_cert);
    যদি(ফলাফল)
      যাও out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_ফলাফল(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(তথ্য, "OpenSSL verify result: %lx", (unsigned long)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  যদি(verified)
    infof(তথ্য, "SSL certificate verified via OpenSSL.");

#ifdef USE_APPLE_SECTRUST
  যদি(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    ফলাফল = ossl_apple_verify(cf, তথ্য, octx, peer);
    যদি(ফলাফল && (ফলাফল != CURLE_PEER_FAILED_VERIFICATION))
      যাও out; /* unexpected error */
    যদি(octx->sectrust_verified) {
      infof(তথ্য, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#endif

  যদি(!verified) {
    /* no trust established, report the OpenSSL status */
    যদি(conn_config->verifypeer) {
      failf(তথ্য, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
      যাও out;
    }
    infof(তথ্য, " SSL certificate verification failed, continuing anyway!");
  }

#ifndef OPENSSL_NO_OCSP
  যদি(conn_config->verifystatus &&
#ifdef USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, তথ্য)) &&
#endif
     !octx->reused_session) {
    ফলাফল = verifystatus(cf, তথ্য, octx);
    যদি(ফলাফল)
      যাও out;
  }
#endif

  ফলাফল = ossl_check_issuer(cf, তথ্য, server_cert);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = ossl_check_pinned_চাবি(cf, তথ্য, server_cert);

out:
  X509_free(server_cert);
  ফেরত ফলাফল;
}

স্থির CURLcode ossl_connect_step3(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  ফলাফল = Curl_ossl_check_peer_cert(cf, তথ্য, octx, &connssl->peer);
  যদি(ফলাফল)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, তথ্য, connssl->peer.scache_চাবি);

  ফেরত ফলাফল;
}

#ifdef HAVE_OPENSSL_EARLYDATA
স্থির CURLcode ossl_send_earlydata(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক unsigned অক্ষর *buf;
  size_t blen, nwritten;
  পূর্ণসংখ্যা rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_ফলাফল = CURLE_OK;
  যখন(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_তথ্য(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(তথ্য, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    যদি(rc <= 0) {
      long sslerror;
      অক্ষর error_বাফার[256];
      পূর্ণসংখ্যা err = SSL_get_error(octx->ssl, rc);

      নির্বাচন(err) {
      ক্ষেত্র SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        ফলাফল = CURLE_AGAIN;
        যাও out;
      ক্ষেত্র SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ফলাফল = CURLE_AGAIN;
        যাও out;
      ক্ষেত্র SSL_ERROR_SYSCALL: {
        পূর্ণসংখ্যা sockerr = SOCKERRNO;

        যদি(octx->io_ফলাফল == CURLE_AGAIN) {
          ফলাফল = CURLE_AGAIN;
          যাও out;
        }
        sslerror = ERR_get_error();
        যদি(sslerror)
          ossl_strerror(sslerror, error_বাফার, আকার(error_বাফার));
        অন্যথা যদি(sockerr)
          curlx_strerror(sockerr, error_বাফার, আকার(error_বাফার));
        অন্যথা
          curl_msnprintf(error_বাফার, আকার(error_বাফার), "%s",
                         SSL_ERROR_to_str(err));

        failf(তথ্য, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_বাফার, sockerr);
        ফলাফল = CURLE_SEND_ERROR;
        যাও out;
      }
      ক্ষেত্র SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(তথ্য, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_বাফার, আকার(error_বাফার)));
        ফলাফল = CURLE_SEND_ERROR;
        যাও out;
      }
      পূর্বনির্ধারিত:
        /* a true error */
        failf(তথ্য, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        ফলাফল = CURLE_SEND_ERROR;
        যাও out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(তথ্য, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ফেরত ফলাফল;
}
#endif /* HAVE_OPENSSL_EARLYDATA */

স্থির CURLcode ossl_connect(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             bool *done)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;

  /* check if the connection has already been established */
  যদি(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  যদি(connssl->connecting_state == ssl_connect_1) {
    যদি(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(তথ্য, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(তথ্য, cf, "need HTTPS-RR, delaying connect");
      ফেরত CURLE_OK;
    }
    CURL_TRC_CF(তথ্য, cf, "ossl_connect, step1");
    ফলাফল = ossl_connect_step1(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
  }

  যদি(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(তথ্য, cf, "ossl_connect, step2");
#ifdef HAVE_OPENSSL_EARLYDATA
    যদি(connssl->earlydata_state == ssl_earlydata_await) {
      যাও out;
    }
    অন্যথা যদি(connssl->earlydata_state == ssl_earlydata_sending) {
      ফলাফল = ossl_send_earlydata(cf, তথ্য);
      যদি(ফলাফল)
        যাও out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#endif
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    ফলাফল = ossl_connect_step2(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
  }

  যদি(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(তথ্য, cf, "ossl_connect, step3");
    ফলাফল = ossl_connect_step3(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
    connssl->connecting_state = ssl_connect_done;
#ifdef HAVE_OPENSSL_EARLYDATA
    যদি(connssl->earlydata_state > ssl_earlydata_none) {
      কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_তথ্য_status(octx->ssl) == SSL_EARLY_তথ্য_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
  }

  যদি(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(তথ্য, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  যদি(ফলাফল == CURLE_AGAIN) {
    *done = FALSE;
    ফেরত CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  ফেরত ফলাফল;
}

স্থির bool ossl_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                              ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  (শূন্য)তথ্য;
  ফেরত (bool)connssl->প্রবেশ_pending;
}

স্থির CURLcode ossl_send(কাঠামো Curl_cfilter *cf,
                          কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক শূন্য *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  পূর্ণসংখ্যা err;
  অক্ষর error_বাফার[256];
  sslerr_t sslerror;
  পূর্ণসংখ্যা memlen;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)INT_MAX) ? INT_MAX : (পূর্ণসংখ্যা)len;
  যদি(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    যদি(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  যদি(nwritten > 0) {
    যদি(nwritten > memlen) {
      failf(তথ্য, "SSL_write(len=%d) returned %d", memlen, nwritten);
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
    *pnwritten = (size_t)nwritten;
  }
  অন্যথা {
    err = SSL_get_error(octx->ssl, nwritten);

    নির্বাচন(err) {
    ক্ষেত্র SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      ফলাফল = CURLE_AGAIN;
      যাও out;
    ক্ষেত্র SSL_ERROR_WANT_WRITE:
      ফলাফল = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      যাও out;
    ক্ষেত্র SSL_ERROR_SYSCALL: {
      পূর্ণসংখ্যা sockerr = SOCKERRNO;

      যদি(octx->io_ফলাফল == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        ফলাফল = CURLE_AGAIN;
        যাও out;
      }
      sslerror = ERR_get_error();
      যদি(sslerror)
        ossl_strerror(sslerror, error_বাফার, আকার(error_বাফার));
      অন্যথা যদি(sockerr)
        curlx_strerror(sockerr, error_বাফার, আকার(error_বাফার));
      অন্যথা
        curl_msnprintf(error_বাফার, আকার(error_বাফার), "%s",
                       SSL_ERROR_to_str(err));

      failf(তথ্য, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_বাফার, sockerr);
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
    ক্ষেত্র SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(তথ্য, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_বাফার, আকার(error_বাফার)));
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
    পূর্বনির্ধারিত:
      /* a true error */
      failf(তথ্য, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      ফলাফল = CURLE_SEND_ERROR;
      যাও out;
    }
  }

out:
  ফেরত ফলাফল;
}

স্থির CURLcode ossl_recv(কাঠামো Curl_cfilter *cf,
                          কাঠামো Curl_easy *তথ্য,   /* transfer */
                          অক্ষর *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  অক্ষর error_বাফার[256];
  unsigned long sslerror;
  পূর্ণসংখ্যা buffsize;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)INT_MAX) ? INT_MAX : (পূর্ণসংখ্যা)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  যদি(nread > 0)
    *pnread = (size_t)nread;
  অন্যথা {
    /* failed SSL_read */
    পূর্ণসংখ্যা err = SSL_get_error(octx->ssl, nread);

    নির্বাচন(err) {
    ক্ষেত্র SSL_ERROR_NONE: /* this is not an error */
      বিরতি;
    ক্ষেত্র SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      যদি(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(তথ্য, cf, "TLS close_notify");
      বিরতি;
    ক্ষেত্র SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ফলাফল = CURLE_AGAIN;
      যাও out;
    ক্ষেত্র SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ফলাফল = CURLE_AGAIN;
      যাও out;
    পূর্বনির্ধারিত:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      যদি(octx->io_ফলাফল == CURLE_AGAIN) {
        ফলাফল = CURLE_AGAIN;
        যাও out;
      }
      sslerror = ERR_get_error();
      যদি((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        পূর্ণসংখ্যা sockerr = SOCKERRNO;
        যদি(sslerror)
          ossl_strerror(sslerror, error_বাফার, আকার(error_বাফার));
        অন্যথা যদি(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_বাফার, আকার(error_বাফার));
        অন্যথা
          curl_msnprintf(error_বাফার, আকার(error_বাফার), "%s",
                         SSL_ERROR_to_str(err));
        failf(তথ্য, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_বাফার, sockerr);
        ফলাফল = CURLE_RECV_ERROR;
        যাও out;
      }
      অন্যথা যদি(err == SSL_ERROR_SYSCALL) {
        যদি(octx->io_ফলাফল) {
          /* logging handling in underlying filter already */
          ফলাফল = octx->io_ফলাফল;
        }
        অন্যথা যদি(connssl->peer_closed) {
          failf(তথ্য, "Connection closed abruptly");
          ফলাফল = CURLE_RECV_ERROR;
        }
        অন্যথা {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          পূর্ণসংখ্যা sockerr = SOCKERRNO;
          যদি(sockerr)
            curlx_strerror(sockerr, error_বাফার, আকার(error_বাফার));
          অন্যথা {
            curl_msnprintf(error_বাফার, আকার(error_বাফার),
                           "Connection closed abruptly");
          }
          failf(তথ্য, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_বাফার, sockerr);
          ফলাফল = CURLE_RECV_ERROR;
        }
        যাও out;
      }
    }
  }

out:
  যদি((!ফলাফল && !*pnread) || (ফলাফল == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->প্রবেশ_pending = FALSE;
  }
  CURL_TRC_CF(তথ্য, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (পূর্ণসংখ্যা)ফলাফল, *pnread, connssl->প্রবেশ_pending);
  ফেরত ফলাফল;
}

স্থির CURLcode ossl_get_channel_binding(কাঠামো Curl_easy *তথ্য,
                                         int8_t sockindex,
                                         কাঠামো dynbuf *binding)
{
  X509 *cert;
  পূর্ণসংখ্যা mdnid;
  bool no_digest_acceptable = FALSE;
  ধ্রুবক EVP_MD *algo_type = ফাঁকা;
  ধ্রুবক অক্ষর *algo_name = ফাঁকা;
  unsigned পূর্ণসংখ্যা দৈর্ঘ্য;
  unsigned অক্ষর buf[EVP_MAX_MD_আকার];

  স্থির ধ্রুবক অক্ষর prefix[] = "tls-server-end-point:";
  কাঠামো connectdata *conn = তথ্য->conn;
  কাঠামো Curl_cfilter *cf = conn->cfilter[sockindex];
  কাঠামো ossl_ctx *octx = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  করো {
    ধ্রুবক কাঠামো Curl_cftype *cft = cf->cft;
    কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;

    যদি(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (কাঠামো ossl_ctx *)connssl->backend;
      বিরতি;
    }

    cf = cf->পরবর্তী;
  } যখন(cf);

  যদি(!octx) {
    failf(তথ্য, "Failed to find the SSL filter");
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  যদি(!cert)
    /* No server certificate, do not do channel binding */
    ফেরত CURLE_OK;

#ifdef HAVE_OPENSSL3
  {
    পূর্ণসংখ্যা pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    যদি(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(তথ্য, "certificate signature algorithm not recognized");
      ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
      যাও out;
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
      অক্ষর mdname[128] = "";
      পূর্ণসংখ্যা rc = EVP_PKEY_get_default_digest_name(pkey, mdname, আকার(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      যদি(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      অন্যথা যদি(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(তথ্য, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#অন্যথা /* HAVE_OPENSSL3 */

  যদি(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, ফাঁকা)) {
    failf(তথ্য,
          "Unable to find digest NID for certificate signature algorithm");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  যদি(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  অন্যথা {
    algo_type = EVP_get_digestbynid(mdnid);
    যদি(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(তথ্য, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
      যাও out;
    }
  }

#endif /* HAVE_OPENSSL3, else */

  যদি(!algo_type) {
    যদি(no_digest_acceptable) {
      infof(তথ্য, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      ফলাফল = CURLE_OK;
      যাও out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(তথ্য, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও out;
  }

  যদি(!X509_digest(cert, algo_type, buf, &দৈর্ঘ্য)) {
    failf(তথ্য, "X509_digest() failed for channel binding");
    ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    যাও out;
  }

  /* Append "tls-server-end-point:" */
  ফলাফল = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  যদি(ফলাফল)
    যাও out;

  /* Append digest */
  ফলাফল = curlx_dyn_addn(binding, buf, দৈর্ঘ্য);

out:
  X509_free(cert);
  ফেরত ফলাফল;
}

size_t Curl_ossl_version(অক্ষর *বাফার, size_t আকার_2)
{
#ifdef LIBRESSL_VERSION_সংখ্যা
  অক্ষর *p;
  size_t গুনতি;
  ধ্রুবক অক্ষর *ver = OpenSSL_version(OPENSSL_VERSION);
  স্থির ধ্রুবক অক্ষর expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  যদি(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  গুনতি = curl_msnprintf(বাফার, আকার_2, "%s/%s", OSSL_PACKAGE, ver);
  জন্য(p = বাফার; *p; ++p) {
    যদি(ISBLANK(*p))
      *p = '_';
  }
  ফেরত গুনতি;
#নতুবা_যদি defined(OPENSSL_IS_AWSLC)
  ফেরত curl_msnprintf(বাফার, আকার_2, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_সংখ্যা_STRING);
#নতুবা_যদি defined(OPENSSL_IS_BORINGSSL)
#ifdef CURL_BORINGSSL_VERSION
  ফেরত curl_msnprintf(বাফার, আকার_2, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#অন্যথা
  ফেরত curl_msnprintf(বাফার, আকার_2, OSSL_PACKAGE);
#endif
#অন্যথা /* OpenSSL 3+ */
  ফেরত curl_msnprintf(বাফার, আকার_2, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
}

/* can be called with data == NULL */
স্থির CURLcode ossl_random(কাঠামো Curl_easy *তথ্য,
                            unsigned অক্ষর *entropy, size_t দৈর্ঘ্য)
{
  পূর্ণসংখ্যা rc;
  যদি(তথ্য) {
    যদি(ossl_seed(তথ্য)) /* Initiate the seed if not already done */
      ফেরত CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  অন্যথা {
    যদি(!rand_enough())
      ফেরত CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(দৈর্ঘ্য));
  ফেরত rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

স্থির CURLcode ossl_sha256sum(ধ্রুবক unsigned অক্ষর *প্রবেশ,
                               size_t len,
                               unsigned অক্ষর *sha256sum /* output */,
                               size_t unused)
{
  CURLcode ফলাফল = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (শূন্য)unused;

  mdctx = EVP_MD_CTX_new();
  যদি(!mdctx)
    ফেরত CURLE_OUT_OF_MEMORY;
  যদি(!EVP_DigestInit_ex(mdctx, EVP_sha256(), ফাঁকা)) {
    ফলাফল = CURLE_FAILED_INIT;
    যাও out;
  }
  যদি(!EVP_DigestUpdate(mdctx, প্রবেশ, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, ফাঁকা))
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  ফেরত ফলাফল;
}

স্থির bool ossl_cert_status_request(শূন্য)
{
#ifndef OPENSSL_NO_OCSP
  ফেরত TRUE;
#অন্যথা
  ফেরত FALSE;
#endif
}

স্থির শূন্য *ossl_get_internals(কাঠামো ssl_connect_তথ্য *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  কাঠামো ossl_ctx *octx = (কাঠামো ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  ফেরত info == CURLINFO_TLS_SESSION ?
    (শূন্য *)octx->ssl_ctx : (শূন্য *)octx->ssl;
}

ধ্রুবক কাঠামো Curl_ssl Curl_ssl_openssl = {
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

  আকার(কাঠামো ossl_ctx),

  ossl_init,                /* init */
  ossl_cleanup,             /* cleanup */
  Curl_ossl_version,        /* version */
  ossl_shutdown,            /* shutdown */
  ossl_তথ্য_pending,        /* data_pending */
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
