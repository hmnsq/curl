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
#ضمن "curl_setup.h"

#اگر_تعریف_شده USE_OPENSSL

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "formdata.h" /* for the boundary function */
#ضمن "url.h" /* for the SSL config check function */
#ضمن "curlx/inet_pton.h"
#ضمن "vtls/openssl.h"
#ضمن "connect.h"
#ضمن "progress.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_int.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vauth/vauth.h"
#ضمن "vtls/keylog.h"
#ضمن "vtls/hostcheck.h"
#ضمن "transfer.h"
#ضمن "multiif.h"
#ضمن "curlx/strerr.h"
#ضمن "curlx/strparse.h"
#ضمن "curlx/strcopy.h"
#ضمن "curlx/strdup.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vdns/httpsrr.h"
#ضمن "vtls/apple.h"
#اگر_تعریف_شده USE_ECH
#ضمن "curlx/base64.h"
#پایان_اگر

#ضمن <openssl/rand.h>
#ضمن <openssl/x509v3.h>
#اگر_تعریف_نشده OPENSSL_NO_DSA
#ضمن <openssl/dsa.h>
#پایان_اگر
#ضمن <openssl/dh.h>
#ضمن <openssl/err.h>
#ضمن <openssl/conf.h>
#ضمن <openssl/bn.h>
#ضمن <openssl/rsa.h>
#ضمن <openssl/bio.h>
#ضمن <openssl/pkcs12.h>
#ضمن <openssl/tls1.h>
#ضمن <openssl/evp.h>

#إذا defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#ضمن <openssl/ech.h>
#پایان_اگر

#اگر_تعریف_نشده OPENSSL_NO_OCSP
#ضمن <openssl/ocsp.h>
#پایان_اگر

#إذا !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#تعریف_کن USE_OPENSSL_ENGINE
#ضمن <openssl/engine.h>
#پایان_اگر

#اگر_تعریف_شده LIBRESSL_نسخه_عدد
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  إذا LIBRESSL_نسخه_عدد < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#وإلاإذا !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#پایان_اگر

#إذا defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#ضمن <openssl/provider.h>
#ضمن <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#تعریف_کن OPENSSL_HAS_PROVIDERS
#پایان_اگر

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#إذا !defined(LIBRESSL_نسخه_عدد) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_کلید_EXPLICIT_PARAMS))
#تعریف_کن HAVE_SSL_CTX_مجموعه_DEFAULT_READ_میانگیر_LEN 1
#پایان_اگر

#إذا defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#ضمن <openssl/ui.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_OPENSSL3
#تعریف_کن HAVE_EVP_PKEY_GET_PARAMS 1
#پایان_اگر

#اگر_تعریف_شده HAVE_EVP_PKEY_GET_PARAMS
#ضمن <openssl/core_names.h>
#تعریف_کن DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = عدم
#تعریف_کن FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#وإلا
#تعریف_کن DECLARE_PKEY_PARAM_BIGNUM(name) ثابت BIGNUM *name
#تعریف_کن FREE_PKEY_PARAM_BIGNUM(name)
#پایان_اگر

/* Whether SSL_CTX_set_ciphersuites is available.
 * BoringSSL: no
 * LibreSSL: supported since 3.4.1 (released 2021-10-14)
 * OpenSSL: supported since 1.1.1 (commit a53b5be6a05)
 */
#اگر_تعریف_نشده OPENSSL_IS_BORINGSSL
#  define HAVE_SSL_CTX_مجموعه_CIPHERSUITES
#  ifndef OPENSSL_IS_AWSLC
#    define HAVE_SSL_CTX_مجموعه_POST_HANDSHAKE_AUTH
#  endif
#پایان_اگر

/* Whether SSL_CTX_set1_sigalgs_list is available
 * BoringSSL: supported since 0.20240913.0 (commit 826ce15)
 * LibreSSL: no
 * OpenSSL: supported since 1.0.2 (commit 0b362de5f575)
 */
#اگر_تعریف_نشده LIBRESSL_نسخه_عدد
#تعریف_کن HAVE_SSL_CTX_SET1_SIGALGS
#پایان_اگر

#اگر_تعریف_شده LIBRESSL_نسخه_عدد
#تعریف_کن OSSL_PACKAGE "LibreSSL"
#وإلاإذا defined(OPENSSL_IS_AWSLC)
#تعریف_کن OSSL_PACKAGE "AWS-LC"
#وإلاإذا defined(OPENSSL_IS_BORINGSSL)
#تعریف_کن OSSL_PACKAGE "BoringSSL"
#وإلاإذا defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#تعریف_کن OSSL_PACKAGE "quictls"
#وإلا
#تعریف_کن OSSL_PACKAGE "OpenSSL"
#پایان_اگر

#اگر_تعریف_شده HAVE_BORINGSSL_LIKE
تعريفنوع size_t numcert_t;
تعريفنوع uint32_t sslerr_t;
#وإلا
تعريفنوع صحيح numcert_t;
تعريفنوع بیعلامت دراز sslerr_t;
#پایان_اگر
#تعریف_کن ossl_valsize_t numcert_t

#إذا OPENSSL_نسخه_عدد >= 0x40100000L
#تعریف_کن ossl_ASN1_STRING_get_درازی ASN1_STRING_get_درازی
#وإلا
ساكن size_t ossl_ASN1_STRING_get_درازی(ثابت ASN1_STRING *str)
{
   ثابت صحيح درازی = ASN1_STRING_درازی(str);
   أرجع درازی >= 0 ? (size_t)درازی : 0;
}
#پایان_اگر

ساكن CURLcode افزودن_certinfo(بنية Curl_easy *داده,
                              BIO *mem, ثابت حرف *label, صحيح رقم)
  WARN_استفادهنشده_نتیجه;

ساكن CURLcode افزودن_certinfo(بنية Curl_easy *داده,
                              BIO *mem, ثابت حرف *label, صحيح رقم)
{
  حرف *ptr;
  دراز len = BIO_get_mem_داده(mem, &ptr);
  CURLcode نتیجه = Curl_ssl_افزودن_certinfo_len(داده, رقم, label, ptr, len);
  (فراغ)BIO_reset(mem);
  أرجع نتیجه;
}

ساكن CURLcode pubkey_show(بنية Curl_easy *داده,
                            BIO *mem,
                            صحيح رقم,
                            ثابت حرف *type,
                            ثابت حرف *name,
                            ثابت BIGNUM *bn) WARN_استفادهنشده_نتیجه;

ساكن CURLcode pubkey_show(بنية Curl_easy *داده,
                            BIO *mem,
                            صحيح رقم,
                            ثابت حرف *type,
                            ثابت حرف *name,
                            ثابت BIGNUM *bn)
{
  حرف namebuf[32];

  curl_msnprintf(namebuf, حجم(namebuf), "%s(%s)", type, name);

  إذا(bn)
    BN_print(mem, bn);
  أرجع افزودن_certinfo(داده, mem, namebuf, رقم);
}

#تعریف_کن print_pubkey_BN(_type, _name, رقم_2)           \
  pubkey_show(داده, mem, رقم_2, #_type, #_name, _name)

ساكن صحيح asn1_object_dump(ثابت ASN1_OBJECT *a, حرف *buf, size_t len)
{
  صحيح i = i2t_ASN1_OBJECT(buf, (صحيح)len, a);
  أرجع (i >= (صحيح)len);  /* buffer too small */
}

ساكن CURLcode X509V3_ext(بنية Curl_easy *داده,
                           صحيح certnum,
                           ثابت پشته_از(X509_EXTENSION) *extsarg)
{
  صحيح i;
  CURLcode نتیجه = CURLE_OK;
#اگر_تعریف_شده LIBRESSL_نسخه_عدد
  پشته_از(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#وإلا
  ثابت پشته_از(X509_EXTENSION) *exts = extsarg;
#پایان_اگر

  إذا((صحيح)sk_X509_EXTENSION_رقم(exts) <= 0)
    /* no extensions, bail out */
    أرجع نتیجه;

  لكل(i = 0; i < (صحيح)sk_X509_EXTENSION_رقم(exts); i++) {
    ثابت ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_مقدار(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    حرف namebuf[128];
    BIO *bio_بیرون = BIO_new(BIO_s_mem());

    إذا(!bio_بیرون)
      أرجع نتیجه;

    obj = X509_EXTENSION_get_object(ext);

    إذا(asn1_object_dump(obj, namebuf, حجم(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    إذا(!X509V3_EXT_print(bio_بیرون, ext, 0, 0))
      ASN1_STRING_print(bio_بیرون,
                        (ثابت ASN1_STRING *)X509_EXTENSION_get_داده(ext));

    BIO_get_mem_ptr(bio_بیرون, &biomem);
    نتیجه = Curl_ssl_افزودن_certinfo_len(داده, certnum, namebuf, biomem->داده,
                                        biomem->درازی);
    BIO_free(bio_بیرون);
    إذا(نتیجه)
      اكسر;
  }
  أرجع نتیجه;
}

ساكن CURLcode get_pkey_rsa(بنية Curl_easy *داده,
                             EVP_PKEY *pubkey, BIO *mem, صحيح i)
{
  CURLcode نتیجه = CURLE_OK;
#اگر_تعریف_نشده HAVE_EVP_PKEY_GET_PARAMS
  RSA *rsa = EVP_PKEY_get0_RSA(pubkey);
#پایان_اگر /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(n);
  DECLARE_PKEY_PARAM_BIGNUM(e);
#اگر_تعریف_شده HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_N, &n);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_E, &e);
#وإلا
  RSA_get0_کلید(rsa, &n, &e, عدم);
#پایان_اگر /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (صحيح)(n ? BN_رقم_بیتها(n) : 0));
  نتیجه = افزودن_certinfo(داده, mem, "RSA Public Key", i);
  إذا(!نتیجه) {
    نتیجه = print_pubkey_BN(rsa, n, i);
    إذا(!نتیجه)
      نتیجه = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  أرجع نتیجه;
}

#اگر_تعریف_نشده OPENSSL_NO_DSA
ساكن CURLcode get_pkey_dsa(بنية Curl_easy *داده,
                             EVP_PKEY *pubkey, BIO *mem, صحيح i)
{
  CURLcode نتیجه = CURLE_OK;
#اگر_تعریف_نشده HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#پایان_اگر /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_کلید);
#اگر_تعریف_شده HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_کلید, &pub_کلید);
#وإلا
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_کلید(dsa, &pub_کلید, عدم);
#پایان_اگر /* HAVE_EVP_PKEY_GET_PARAMS */
  نتیجه = print_pubkey_BN(dsa, p, i);
  إذا(!نتیجه)
    نتیجه = print_pubkey_BN(dsa, q, i);
  إذا(!نتیجه)
    نتیجه = print_pubkey_BN(dsa, g, i);
  إذا(!نتیجه)
    نتیجه = print_pubkey_BN(dsa, pub_کلید, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_کلید);
  أرجع نتیجه;
}
#پایان_اگر /* !OPENSSL_NO_DSA */

ساكن CURLcode get_pkey_dh(بنية Curl_easy *داده,
                            EVP_PKEY *pubkey, BIO *mem, صحيح i)
{
  CURLcode نتیجه;
#اگر_تعریف_نشده HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#پایان_اگر /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_کلید);
#اگر_تعریف_شده HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_کلید, &pub_کلید);
#وإلا
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_کلید(dh, &pub_کلید, عدم);
#پایان_اگر /* HAVE_EVP_PKEY_GET_PARAMS */
  نتیجه = print_pubkey_BN(dh, p, i);
  إذا(!نتیجه)
    نتیجه = print_pubkey_BN(dh, q, i);
  إذا(!نتیجه)
    نتیجه = print_pubkey_BN(dh, g, i);
  إذا(!نتیجه)
    نتیجه = print_pubkey_BN(dh, pub_کلید, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_کلید);
  أرجع نتیجه;
}

#اگر_تعریف_شده HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
تعريفنوع ثابت X509_PUBKEY pubkeytype_t;
#وإلا
تعريفنوع X509_PUBKEY pubkeytype_t;
#پایان_اگر

ساكن CURLcode ossl_certchain(بنية Curl_easy *داده, SSL *ssl)
{
  CURLcode نتیجه;
  پشته_از(X509) *sk;
  صحيح i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  إذا(!sk)
    أرجع CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_رقم(sk);
  إذا(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(داده, "%d certificates is more than allowed (%d)", (صحيح)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  نتیجه = Curl_ssl_init_certinfo(داده, (صحيح)numcerts);
  إذا(نتیجه)
    أرجع نتیجه;

  mem = BIO_new(BIO_s_mem());
  إذا(!mem)
    نتیجه = CURLE_بیرون_از_حافظه;

  لكل(i = 0; !نتیجه && (i < (صحيح)numcerts); i++) {
    X509 *x = sk_X509_مقدار(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = عدم;
    size_t j;
    ثابت ASN1_بیت_STRING *psig = عدم;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    نتیجه = افزودن_certinfo(داده, mem, "Subject", i);
    إذا(نتیجه)
      اكسر;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    نتیجه = افزودن_certinfo(داده, mem, "Issuer", i);
    إذا(نتیجه)
      اكسر;

    BIO_printf(mem, "%lx", (بیعلامت دراز)X509_get_نسخه(x));
    نتیجه = افزودن_certinfo(داده, mem, "Version", i);
    إذا(نتیجه)
      اكسر;

    {
      ASN1_INTEGER *رقم = X509_get_serial_عدد(x);
      ثابت بیعلامت حرف *numdata = ASN1_STRING_get0_داده(رقم);
      ثابت size_t numlen = ossl_ASN1_STRING_get_درازی(رقم);
      إذا(ASN1_STRING_type(رقم) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      لكل(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    نتیجه = افزودن_certinfo(داده, mem, "Serial Number", i);
    إذا(نتیجه)
      اكسر;

    {
      ثابت X509_ALGOR *sigalg = عدم;
      pubkeytype_t *xpubkey = عدم;
      ASN1_OBJECT *pubkeyoid = عدم;

      X509_get0_signature(&psig, &sigalg, x);
      إذا(sigalg) {
        ثابت ASN1_OBJECT *sigalgoid = عدم;
        X509_ALGOR_get0(&sigalgoid, عدم, عدم, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        نتیجه = افزودن_certinfo(داده, mem, "Signature Algorithm", i);
        إذا(نتیجه)
          اكسر;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      إذا(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, عدم, عدم, عدم, xpubkey);
        إذا(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          نتیجه = افزودن_certinfo(داده, mem, "Public Key Algorithm", i);
          إذا(نتیجه)
            اكسر;
        }
      }

      نتیجه = X509V3_ext(داده, i, X509_get0_extensions(x));
      إذا(نتیجه)
        اكسر;
    }

    ASN1_زمان_print(mem, X509_get0_notBefore(x));
    نتیجه = افزودن_certinfo(داده, mem, "Start date", i);
    إذا(نتیجه)
      اكسر;

    ASN1_زمان_print(mem, X509_get0_notAfter(x));
    نتیجه = افزودن_certinfo(داده, mem, "Expire date", i);
    إذا(نتیجه)
      اكسر;

    pubkey = X509_get_pubkey(x);
    إذا(!pubkey)
      infof(داده, "   Unable to load public key");
    وإلا {
      بدل(EVP_PKEY_id(pubkey)) {
      حالة EVP_PKEY_RSA:
        نتیجه = get_pkey_rsa(داده, pubkey, mem, i);
        اكسر;

#اگر_تعریف_نشده OPENSSL_NO_DSA
      حالة EVP_PKEY_DSA:
        نتیجه = get_pkey_dsa(داده, pubkey, mem, i);
        اكسر;
#پایان_اگر

      حالة EVP_PKEY_DH:
        نتیجه = get_pkey_dh(داده, pubkey, mem, i);
        اكسر;
      }
      EVP_PKEY_free(pubkey);
    }

    إذا(!نتیجه && psig) {
      ثابت بیعلامت حرف *psigdata = ASN1_STRING_get0_داده(psig);
      ثابت size_t psiglen = ossl_ASN1_STRING_get_درازی(psig);
      لكل(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      نتیجه = افزودن_certinfo(داده, mem, "Signature", i);
    }

    إذا(!نتیجه) {
      PEM_write_bio_X509(mem, x);
      نتیجه = افزودن_certinfo(داده, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  إذا(نتیجه)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(داده);

  أرجع نتیجه;
}

ساكن صحيح ossl_bio_cf_create(BIO *bio)
{
  BIO_مجموعه_shutdown(bio, 1);
  BIO_مجموعه_init(bio, 1);
  BIO_مجموعه_داده(bio, عدم);
  أرجع 1;
}

ساكن صحيح ossl_bio_cf_destroy(BIO *bio)
{
  إذا(!bio)
    أرجع 0;
  أرجع 1;
}

ساكن دراز ossl_bio_cf_ctrl(BIO *bio, صحيح cmd, دراز رقم, فراغ *ptr)
{
  بنية Curl_cfilter *cf = BIO_get_داده(bio);
  دراز ret = 1;

  (فراغ)cf;
  (فراغ)ptr;
  بدل(cmd) {
  حالة BIO_CTRL_GET_CLOSE:
    ret = (دراز)BIO_get_shutdown(bio);
    اكسر;
  حالة BIO_CTRL_مجموعه_CLOSE:
    BIO_مجموعه_shutdown(bio, (صحيح)رقم);
    اكسر;
  حالة BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    اكسر;
  حالة BIO_CTRL_DUP:
    ret = 1;
    اكسر;
  حالة BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    بنية ssl_connect_داده *connssl = cf->ctx;
    أرجع connssl->peer_closed;
  }
  افتراضي:
    ret = 0;
    اكسر;
  }
  أرجع ret;
}

ساكن صحيح ossl_bio_cf_بیرون_write(BIO *bio, ثابت حرف *buf, صحيح blen)
{
  بنية Curl_cfilter *cf = BIO_get_داده(bio);
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجه;

  DEBUGASSERT(داده);
  إذا(blen < 0)
    أرجع 0;

  نتیجه = Curl_conn_cf_send(cf->بعدی, داده,
                             (ثابت uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(داده, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (صحيح)نتیجه, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_نتیجه = نتیجه;
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN)
      BIO_مجموعه_retry_write(bio);
    أرجع -1;
  }
  أرجع (صحيح)nwritten;
}

ساكن صحيح ossl_bio_cf_درون_read(BIO *bio, حرف *buf, صحيح blen)
{
  بنية Curl_cfilter *cf = BIO_get_داده(bio);
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  size_t nread;
  CURLcode نتیجه, r2;

  DEBUGASSERT(داده);
  /* OpenSSL catches this case, so should we. */
  إذا(!buf)
    أرجع 0;
  إذا(blen < 0)
    أرجع 0;

  نتیجه = Curl_conn_cf_recv(cf->بعدی, داده, buf, (size_t)blen, &nread);
  CURL_TRC_CF(داده, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (صحيح)نتیجه, nread);
  BIO_clear_retry_flags(bio);
  octx->io_نتیجه = نتیجه;
  إذا(نتیجه) {
    إذا(نتیجه == CURLE_AGAIN)
      BIO_مجموعه_retry_read(bio);
  }
  وإلا {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->درون_داد_pending = TRUE;
    إذا(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  إذا(!octx->x509_store_راهاندازی) {
    r2 = Curl_ssl_راهاندازی_x509_store(cf, داده, octx);
    إذا(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_نتیجه = r2;
      أرجع -1;
    }
    octx->x509_store_راهاندازی = TRUE;
  }
  أرجع نتیجه ? -1 : (صحيح)nread;
}

ساكن BIO_روش *ossl_bio_cf_روش_create(فراغ)
{
  BIO_روش *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  إذا(m) {
    BIO_meth_مجموعه_write(m, &ossl_bio_cf_بیرون_write);
    BIO_meth_مجموعه_read(m, &ossl_bio_cf_درون_read);
    BIO_meth_مجموعه_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_مجموعه_create(m, &ossl_bio_cf_create);
    BIO_meth_مجموعه_destroy(m, &ossl_bio_cf_destroy);
  }
  أرجع m;
}

ساكن فراغ ossl_bio_cf_روش_free(BIO_روش *m)
{
  إذا(m)
    BIO_meth_free(m);
}

#اگر_تعریف_نشده HAVE_KEYLOG_UPSTREAM
#اگر_تعریف_شده HAVE_KEYLOG_CALLBACK
ساكن فراغ ossl_keylog_callback(ثابت SSL *ssl, ثابت حرف *سطر)
{
  (فراغ)ssl;

  Curl_tls_keylog_write_سطر(سطر);
}
#وإلا
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
ساكن فراغ ossl_log_tls12_secret(ثابت SSL *ssl, bool *keylog_انجامشده)
{
  ثابت SSL_SESSION *session;
  بیعلامت حرف client_random[SSL3_RANDOM_اندازه];
  بیعلامت حرف master_کلید[SSL_MAX_MASTER_کلید_درازی];
  صحيح master_کلید_درازی = 0;

  ERR_مجموعه_mark();

  session = SSL_get_session(ssl);

  إذا(!session || *keylog_انجامشده) {
    ERR_برداشت_مقصد_mark();
    أرجع;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_اندازه);
  master_کلید_درازی = (صحيح)
    SSL_SESSION_get_master_کلید(session, master_کلید, SSL_MAX_MASTER_کلید_درازی);

  ERR_برداشت_مقصد_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  إذا(master_کلید_درازی <= 0)
    أرجع;

  *keylog_انجامشده = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        حجم(client_random),
                        master_کلید, master_کلید_درازی);
}
#پایان_اگر /* !HAVE_KEYLOG_CALLBACK */
#پایان_اگر /* HAVE_KEYLOG_UPSTREAM */

ساكن ثابت حرف *SSL_ERROR_مقصد_str(صحيح err)
{
  بدل(err) {
  حالة SSL_ERROR_NONE:
    أرجع "SSL_ERROR_NONE";
  حالة SSL_ERROR_SSL:
    أرجع "SSL_ERROR_SSL";
  حالة SSL_ERROR_WANT_READ:
    أرجع "SSL_ERROR_WANT_READ";
  حالة SSL_ERROR_WANT_WRITE:
    أرجع "SSL_ERROR_WANT_WRITE";
  حالة SSL_ERROR_WANT_X509_LOOKUP:
    أرجع "SSL_ERROR_WANT_X509_LOOKUP";
  حالة SSL_ERROR_SYSCALL:
    أرجع "SSL_ERROR_SYSCALL";
  حالة SSL_ERROR_ZERO_RETURN:
    أرجع "SSL_ERROR_ZERO_RETURN";
  حالة SSL_ERROR_WANT_CONNECT:
    أرجع "SSL_ERROR_WANT_CONNECT";
  حالة SSL_ERROR_WANT_ACCEPT:
    أرجع "SSL_ERROR_WANT_ACCEPT";
#اگر_تعریف_شده SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  حالة SSL_ERROR_WANT_ASYNC:
    أرجع "SSL_ERROR_WANT_ASYNC";
#پایان_اگر
#اگر_تعریف_شده SSL_ERROR_WANT_ASYNC_کار  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  حالة SSL_ERROR_WANT_ASYNC_کار:
    أرجع "SSL_ERROR_WANT_ASYNC_JOB";
#پایان_اگر
#اگر_تعریف_شده SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  حالة SSL_ERROR_WANT_CLIENT_HELLO_CB:
    أرجع "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#پایان_اگر
  افتراضي:
    أرجع "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
ساكن حرف *ossl_strerror(بیعلامت دراز error, حرف *buf, size_t اندازه)
{
  size_t len;
  DEBUGASSERT(اندازه);
  *buf = '\0';

  len = Curl_ossl_نسخه(buf, اندازه);
  DEBUGASSERT(len < (اندازه - 2));
  إذا(len < (اندازه - 2)) {
    buf += len;
    اندازه -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#اگر_تعریف_شده HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, اندازه);
#وإلا
  ERR_error_string_n(error, buf, اندازه);
#پایان_اگر

  إذا(!*buf) {
    ثابت حرف *پیام = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, اندازه, پیام, طول_رشته(پیام));
  }

  أرجع buf;
}

ساكن صحيح passwd_callback(حرف *buf, صحيح رقم, صحيح encrypting, فراغ *password)
{
  DEBUGASSERT(encrypting == 0);

  إذا(!encrypting && رقم >= 0 && password) {
    صحيح klen = curlx_uztosi(طول_رشته((حرف *)password));
    إذا(رقم > klen) {
      کپی_حافظه(buf, password, klen + 1);
      أرجع klen;
    }
  }
  أرجع 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
ساكن bool rand_enough(فراغ)
{
  أرجع RAND_وضعیت() != 0;
}

ساكن CURLcode ossl_دانه(بنية Curl_easy *داده)
{
  /* This might get called before it has been added to a multi handle */
  إذا(داده->multi && داده->multi->ssl_seeded)
    أرجع CURLE_OK;

  إذا(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    إذا(داده->multi)
      داده->multi->ssl_seeded = TRUE;
    أرجع CURLE_OK;
  }
  failf(داده, "Insufficient randomness");
  أرجع CURLE_SSL_CONNECT_ERROR;
}

#اگر_تعریف_نشده SSL_FILETYPE_ENGINE
#تعریف_کن SSL_FILETYPE_ENGINE 42
#پایان_اگر
#اگر_تعریف_نشده SSL_FILETYPE_PKCS12
#تعریف_کن SSL_FILETYPE_PKCS12 43
#پایان_اگر
#اگر_تعریف_نشده SSL_FILETYPE_PROVIDER
#تعریف_کن SSL_FILETYPE_PROVIDER 44
#پایان_اگر
ساكن صحيح ossl_do_file_type(ثابت حرف *type)
{
  إذا(!type || !type[0])
    أرجع SSL_FILETYPE_PEM;
  إذا(curl_strequal(type, "PEM"))
    أرجع SSL_FILETYPE_PEM;
  إذا(curl_strequal(type, "DER"))
    أرجع SSL_FILETYPE_ASN1;
  إذا(curl_strequal(type, "PROV"))
    أرجع SSL_FILETYPE_PROVIDER;
  إذا(curl_strequal(type, "ENG"))
    أرجع SSL_FILETYPE_ENGINE;
  إذا(curl_strequal(type, "P12"))
    أرجع SSL_FILETYPE_PKCS12;
  أرجع -1;
}

#إذا defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
ساكن صحيح ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ثابت حرف *password;
  بدل(UI_get_string_type(uis)) {
  حالة UIT_PROMPT:
  حالة UIT_VERIFY:
    password = (ثابت حرف *)UI_get0_user_داده(ui);
    إذا(password && (UI_get_درون_داد_flags(uis) & UI_درون_داد_FLAG_DEFAULT_PWD)) {
      UI_مجموعه_نتیجه(ui, uis, password);
      أرجع 1;
    }
    FALLTHROUGH();
  افتراضي:
    اكسر;
  }
  أرجع UI_روش_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
ساكن صحيح ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  بدل(UI_get_string_type(uis)) {
  حالة UIT_PROMPT:
  حالة UIT_VERIFY:
    إذا(UI_get0_user_داده(ui) &&
       (UI_get_درون_داد_flags(uis) & UI_درون_داد_FLAG_DEFAULT_PWD)) {
      أرجع 1;
    }
    FALLTHROUGH();
  افتراضي:
    اكسر;
  }
  أرجع UI_روش_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
ساكن bool is_pkcs11_uri(ثابت حرف *string)
{
  أرجع string && curl_strnequal(string, "pkcs11:", 7);
}

#پایان_اگر

ساكن CURLcode ossl_مجموعه_engine(بنية Curl_easy *داده, بنية ossl_ctx *octx,
                                ثابت حرف *name);
#اگر_تعریف_شده OPENSSL_HAS_PROVIDERS
ساكن CURLcode ossl_مجموعه_provider(بنية Curl_easy *داده,
                                  بنية ossl_ctx *octx,
                                  ثابت حرف *iname);
#پایان_اگر

ساكن صحيح use_certificate_blob(SSL_CTX *ctx, ثابت بنية curl_blob *blob,
                                صحيح type, ثابت حرف *کلید_passwd)
{
  صحيح ret = 0;
  X509 *x = عدم;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *درون = BIO_new_mem_buf(blob->داده, (صحيح)blob->len);
  إذا(!درون)
    أرجع CURLE_بیرون_از_حافظه;

  إذا(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(درون, عدم);
  }
  وإلا إذا(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(درون, عدم, passwd_callback, CURL_UNCONST(کلید_passwd));
  }
  وإلا {
    ret = 0;
    اذهب انتها;
  }

  إذا(!x) {
    ret = 0;
    اذهب انتها;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
انتها:
  X509_free(x);
  BIO_free(درون);
  أرجع ret;
}

ساكن صحيح use_privatekey_blob(SSL_CTX *ctx, ثابت بنية curl_blob *blob,
                               صحيح type, ثابت حرف *کلید_passwd)
{
  صحيح ret = 0;
  EVP_PKEY *pkey = عدم;
  BIO *درون = BIO_new_mem_buf(blob->داده, (صحيح)blob->len);
  إذا(!درون)
    أرجع CURLE_بیرون_از_حافظه;

  إذا(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_خصوصی_کلید(درون, عدم, passwd_callback,
                                   CURL_UNCONST(کلید_passwd));
  وإلا إذا(type == SSL_FILETYPE_ASN1)
    pkey = d2i_خصوصی_کلید_bio(درون, عدم);
  وإلا
    اذهب انتها;

  إذا(!pkey)
    اذهب انتها;

  ret = SSL_CTX_use_خصوصی_کلید(ctx, pkey);
  EVP_PKEY_free(pkey);
انتها:
  BIO_free(درون);
  أرجع ret;
}

ساكن صحيح use_certificate_chain_blob(SSL_CTX *ctx,
                                      ثابت بنية curl_blob *blob,
                                      ثابت حرف *کلید_passwd)
{
  صحيح ret = 0;
  X509 *x = عدم;
  BIO *درون = BIO_new_mem_buf(blob->داده, (صحيح)blob->len);
  إذا(!درون)
    أرجع CURLE_بیرون_از_حافظه;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(درون, عدم,
                            passwd_callback, CURL_UNCONST(کلید_passwd));
  إذا(!x)
    اذهب انتها;

  ret = SSL_CTX_use_certificate(ctx, x);

  إذا(ERR_peek_error() != 0)
    ret = 0;

  إذا(ret) {
    X509 *ca;
    sslerr_t err;

    إذا(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      اذهب انتها;
    }

    طالما((ca = PEM_read_bio_X509(درون, عدم, passwd_callback,
                                  CURL_UNCONST(کلید_passwd))) != عدم) {

      إذا(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        اذهب انتها;
      }
    }

    err = ERR_peek_آخرین_error();
    إذا((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_شروع_سطر))
      ERR_clear_error();
    وإلا
      ret = 0;
  }

انتها:
  X509_free(x);
  BIO_free(درون);
  أرجع ret;
}

ساكن صحيح enginecheck(بنية Curl_easy *داده,
                       بنية ossl_ctx *octx,
                       ثابت حرف *کلید_file,
                       ثابت حرف *کلید_passwd)
{
#اگر_تعریف_شده USE_OPENSSL_ENGINE
  EVP_PKEY *priv_کلید = عدم;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  إذا(!octx->p.engine && is_pkcs11_uri(کلید_file) &&
     ossl_مجموعه_engine(داده, octx, "pkcs11") != CURLE_OK)
    أرجع 0;

  إذا(octx->p.engine) {
    UI_روش *ui_روش = UI_create_روش("curl user interface");
    إذا(!ui_روش) {
      failf(داده, "unable to create " OSSL_PACKAGE " user-interface method");
      أرجع 0;
    }
    UI_روش_مجموعه_opener(ui_روش, UI_روش_get_opener(UI_OpenSSL()));
    UI_روش_مجموعه_closer(ui_روش, UI_روش_get_closer(UI_OpenSSL()));
    UI_روش_مجموعه_reader(ui_روش, ssl_ui_reader);
    UI_روش_مجموعه_writer(ui_روش, ssl_ui_writer);
    priv_کلید = ENGINE_بارگیری_خصوصی_کلید(octx->p.engine, کلید_file,
                                       ui_روش,
                                       CURL_UNCONST(کلید_passwd));
    UI_destroy_روش(ui_روش);
    إذا(!priv_کلید) {
      failf(داده, "failed to load private key from crypto engine");
      أرجع 0;
    }
    إذا(SSL_CTX_use_خصوصی_کلید(ctx, priv_کلید) != 1) {
      failf(داده, "unable to set private key");
      EVP_PKEY_free(priv_کلید);
      أرجع 0;
    }
    EVP_PKEY_free(priv_کلید);  /* we do not need the handle any more... */
  }
  وإلا {
    failf(داده, "crypto engine not set, cannot load private key");
    أرجع 0;
  }
  أرجع 1;
#وإلا
  (فراغ)octx;
  (فراغ)کلید_file;
  (فراغ)کلید_passwd;
  failf(داده, "SSL_FILETYPE_ENGINE not supported for private key");
  أرجع 0;
#پایان_اگر
}

ساكن صحيح providercheck(بنية Curl_easy *داده,
                         بنية ossl_ctx *octx,
                         ثابت حرف *کلید_file)
{
#اگر_تعریف_شده OPENSSL_HAS_PROVIDERS
  حرف error_میانگیر[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  إذا(!octx->p.provider_loaded && is_pkcs11_uri(کلید_file) &&
     ossl_مجموعه_provider(داده, octx, "pkcs11") != CURLE_OK) {
    أرجع 0;
  }

  إذا(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_کلید = عدم;
    OSSL_STORE_CTX *store = عدم;
    OSSL_STORE_INFO *info = عدم;
    UI_روش *ui_روش = UI_create_روش("curl user interface");
    إذا(!ui_روش) {
      failf(داده, "unable to create " OSSL_PACKAGE " user-interface method");
      أرجع 0;
    }
    UI_روش_مجموعه_opener(ui_روش, UI_روش_get_opener(UI_OpenSSL()));
    UI_روش_مجموعه_closer(ui_روش, UI_روش_get_closer(UI_OpenSSL()));
    UI_روش_مجموعه_reader(ui_روش, ssl_ui_reader);
    UI_روش_مجموعه_writer(ui_روش, ssl_ui_writer);

    store = OSSL_STORE_open_ex(کلید_file, octx->p.libctx,
                               octx->p.propq, ui_روش, عدم, عدم,
                               عدم, عدم);
    إذا(!store) {
      failf(داده, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      UI_destroy_روش(ui_روش);
      أرجع 0;
    }
    إذا(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(داده, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
    }

    info = OSSL_STORE_بارگیری(store);
    إذا(info) {
      صحيح ossl_type = OSSL_STORE_INFO_get_type(info);

      إذا(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_کلید = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_روش(ui_روش);
    إذا(!priv_کلید) {
      failf(داده, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      أرجع 0;
    }

    إذا(SSL_CTX_use_خصوصی_کلید(ctx, priv_کلید) != 1) {
      failf(داده, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      EVP_PKEY_free(priv_کلید);
      أرجع 0;
    }
    EVP_PKEY_free(priv_کلید); /* we do not need the handle any more... */
  }
  وإلا {
    failf(داده, "crypto provider not set, cannot load private key");
    أرجع 0;
  }
  أرجع 1;
#وإلا
  (فراغ)octx;
  (فراغ)کلید_file;
  failf(داده, "SSL_FILETYPE_PROVIDER not supported for private key");
  أرجع 0;
#پایان_اگر
}

ساكن صحيح engineload(بنية Curl_easy *داده,
                      بنية ossl_ctx *octx,
                      ثابت حرف *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#إذا defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  حرف error_میانگیر[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  إذا(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_مجموعه_engine(داده, octx, "pkcs11") != CURLE_OK)
    أرجع 0;

  إذا(octx->p.engine) {
    ساكن ثابت حرف cmd_name[] = "LOAD_CERT_CTRL";
    بنية {
      ثابت حرف *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = عدم;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    إذا(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), عدم)) {
      failf(داده, "SSL engine does not support loading certificates");
      أرجع 0;
    }

    /* Load the certificate from the engine */
    إذا(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, عدم, 1)) {
      failf(داده, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      أرجع 0;
    }

    إذا(!params.cert) {
      failf(داده, "SSL engine did not initialize the certificate properly.");
      أرجع 0;
    }

    إذا(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(داده, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      X509_free(params.cert);
      أرجع 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  وإلا {
    failf(داده, "crypto engine not set, cannot load certificate");
    أرجع 0;
  }
  أرجع 1;
#وإلا
  (فراغ)octx;
  (فراغ)cert_file;
  failf(داده, "SSL_FILETYPE_ENGINE not supported for certificate");
  أرجع 0;
#پایان_اگر
}

ساكن صحيح providerload(بنية Curl_easy *داده,
                        بنية ossl_ctx *octx,
                        ثابت حرف *cert_file)
{
#اگر_تعریف_شده OPENSSL_HAS_PROVIDERS
  حرف error_میانگیر[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  إذا(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_مجموعه_provider(داده, octx, "pkcs11") != CURLE_OK)
    أرجع 0;

  إذا(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = عدم;
    X509 *cert = عدم;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         عدم, عدم, عدم, عدم, عدم, عدم);
    صحيح rc;

    إذا(!store) {
      failf(داده, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      أرجع 0;
    }
    إذا(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(داده, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
    }

    info = OSSL_STORE_بارگیری(store);
    إذا(info) {
      صحيح ossl_type = OSSL_STORE_INFO_get_type(info);

      إذا(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    إذا(!cert) {
      failf(داده, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      أرجع 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    إذا(rc != 1) {
      failf(داده, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      أرجع 0;
    }
  }
  وإلا {
    failf(داده, "crypto provider not set, cannot load certificate");
    أرجع 0;
  }
  أرجع 1;
#وإلا
  (فراغ)octx;
  (فراغ)cert_file;
  failf(داده, "SSL_FILETYPE_PROVIDER not supported for certificate");
  أرجع 0;
#پایان_اگر
}

ساكن صحيح pkcs12load(بنية Curl_easy *داده,
                      SSL_CTX* ctx,
                      ثابت بنية curl_blob *cert_blob,
                      ثابت حرف *cert_file,
                      ثابت حرف *کلید_passwd)
{
  حرف error_میانگیر[256];
  BIO *cert_bio = عدم;
  PKCS12 *p12 = عدم;
  EVP_PKEY *pri;
  X509 *x509;
  صحيح cert_انجامشده = 0;
  پشته_از(X509) *ca = عدم;
  إذا(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->داده, (صحيح)cert_blob->len);
    إذا(!cert_bio) {
      failf(داده, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      أرجع 0;
    }
  }
  وإلا {
    cert_bio = BIO_new(BIO_s_file());
    إذا(!cert_bio) {
      failf(داده, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
      أرجع 0;
    }

    إذا(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(داده, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      أرجع 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, عدم);
  BIO_free(cert_bio);

  إذا(!p12) {
    failf(داده, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    أرجع 0;
  }

  إذا(!PKCS12_parse(p12, کلید_passwd, &pri, &x509, &ca)) {
    failf(داده, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_میانگیر, حجم(error_میانگیر)));
    PKCS12_free(p12);
    أرجع 0;
  }

  PKCS12_free(p12);

  إذا(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(داده, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_میانگیر, حجم(error_میانگیر)));
    اذهب fail;
  }

  إذا(SSL_CTX_use_خصوصی_کلید(ctx, pri) != 1) {
    failf(داده, "unable to use private key from PKCS12 file '%s'", cert_file);
    اذهب fail;
  }

  إذا(!SSL_CTX_check_خصوصی_کلید(ctx)) {
    failf(داده, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    اذهب fail;
  }
  /* Set Certificate Verification chain */
  إذا(ca) {
    طالما(sk_X509_رقم(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_برداشت(ca);
      إذا(!SSL_CTX_جمع_کن_client_CA(ctx, x)) {
        X509_free(x);
        failf(داده, "cannot add certificate to client CA list");
        اذهب fail;
      }
      إذا(!SSL_CTX_جمع_کن_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(داده, "cannot add certificate to certificate chain");
        اذهب fail;
      }
    }
  }

  cert_انجامشده = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic افزودن
#رهنمود clang diagnostic ignored "-Wcast-function-type-strict"
#پایان_اگر
  sk_X509_برداشت_free(ca, X509_free);
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic برداشت
#پایان_اگر
  إذا(!cert_انجامشده)
    أرجع 0; /* failure! */
  أرجع 1;
}

ساكن CURLcode client_cert(بنية Curl_easy *داده,
                            بنية ossl_ctx *octx,
                            حرف *cert_file,
                            ثابت بنية curl_blob *cert_blob,
                            ثابت حرف *cert_type,
                            حرف *کلید_file,
                            ثابت بنية curl_blob *کلید_blob,
                            ثابت حرف *کلید_type,
                            حرف *کلید_passwd)
{
  حرف error_میانگیر[256];
  bool check_privkey = TRUE;
  صحيح file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  إذا(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_انجامشده = FALSE;
    صحيح cert_use_نتیجه;

    إذا(کلید_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_مجموعه_default_passwd_cb_userdata(ctx, کلید_passwd);
      /* Set passwd callback: */
      SSL_CTX_مجموعه_default_passwd_cb(ctx, passwd_callback);
    }

    بدل(file_type) {
    حالة SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_نتیجه = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, کلید_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      إذا(cert_use_نتیجه != 1) {
        failf(داده,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_میانگیر,
                            حجم(error_میانگیر)));
        أرجع CURLE_SSL_CERTPROBLEM;
      }
      اكسر;

    حالة SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_نتیجه = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, کلید_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      إذا(cert_use_نتیجه != 1) {
        failf(داده,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_میانگیر,
                            حجم(error_میانگیر)));
        أرجع CURLE_SSL_CERTPROBLEM;
      }
      اكسر;

    حالة SSL_FILETYPE_ENGINE:
      إذا(!cert_file || !engineload(داده, octx, cert_file))
        أرجع CURLE_SSL_CERTPROBLEM;
      اكسر;

    حالة SSL_FILETYPE_PROVIDER:
      إذا(!cert_file || !providerload(داده, octx, cert_file))
        أرجع CURLE_SSL_CERTPROBLEM;
      اكسر;

    حالة SSL_FILETYPE_PKCS12:
      إذا(!pkcs12load(داده, ctx, cert_blob, cert_file, کلید_passwd))
        أرجع CURLE_SSL_CERTPROBLEM;
      pcks12_انجامشده = TRUE;
      اكسر;

    افتراضي:
      failf(داده, "not supported file type '%s' for certificate", cert_type);
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }

    إذا(!کلید_file && !کلید_blob) {
      کلید_file = cert_file;
      کلید_blob = cert_blob;
    }
    وإلا
      file_type = ossl_do_file_type(کلید_type);

    بدل(file_type) {
    حالة SSL_FILETYPE_PEM:
    حالة SSL_FILETYPE_ASN1:
      cert_use_نتیجه = کلید_blob ?
        use_privatekey_blob(ctx, کلید_blob, file_type, کلید_passwd) :
      SSL_CTX_use_خصوصی_کلید_file(ctx, کلید_file, file_type);
      إذا(cert_use_نتیجه != 1) {
        failf(داده, "unable to set private key file: '%s' type %s",
              کلید_file ? کلید_file : "(memory blob)",
              کلید_type ? کلید_type : "PEM");
        أرجع CURLE_BAD_FUNCTION_آرگومان;
      }
      اكسر;
    حالة SSL_FILETYPE_ENGINE:
      إذا(!enginecheck(داده, octx, کلید_file, کلید_passwd))
        أرجع CURLE_SSL_CERTPROBLEM;
      اكسر;

    حالة SSL_FILETYPE_PROVIDER:
      إذا(!providercheck(داده, octx, کلید_file))
        أرجع CURLE_SSL_CERTPROBLEM;
      اكسر;

    حالة SSL_FILETYPE_PKCS12:
      إذا(!pcks12_انجامشده) {
        failf(داده, "file type P12 for private key not supported");
        أرجع CURLE_SSL_CERTPROBLEM;
      }
      اكسر;
    افتراضي:
      failf(داده, "not supported file type for private key");
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }

    ssl = SSL_new(ctx);
    إذا(!ssl) {
      failf(داده, "unable to create an SSL structure");
      أرجع CURLE_بیرون_از_حافظه;
    }

    x509 = SSL_get_certificate(ssl);

    إذا(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#إذا !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_کلید = SSL_get_privatekey(ssl);
      إذا(EVP_PKEY_id(priv_کلید) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_کلید);
        إذا(RSA_flags(rsa) & RSA_روش_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#پایان_اگر

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    إذا(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      إذا(!SSL_CTX_check_خصوصی_کلید(ctx)) {
        failf(داده, "Private key does not match the certificate public key");
        أرجع CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  أرجع CURLE_OK;
}

#اگر_تعریف_شده CURLVERBOSE
/* returns non-zero on failure */
ساكن CURLcode x509_name_oneline(ثابت X509_NAME *a, بنية dynbuf *d)
{
  BIO *bio_بیرون = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  صحيح rc;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  إذا(bio_بیرون) {
    بیعلامت دراز flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_بیرون, a, 0, flags);
    إذا(rc != -1) {
      BIO_get_mem_ptr(bio_بیرون, &biomem);
      نتیجه = curlx_dyn_addn(d, biomem->داده, biomem->درازی);
    }
    BIO_free(bio_بیرون);
  }
  أرجع نتیجه;
}
#پایان_اگر

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ساكن صحيح ossl_init(فراغ)
{
  ثابت uint64_t flags =
#اگر_تعریف_شده OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#پایان_اگر
#اگر_تعریف_شده CURL_DISABLE_OPENSSL_AUTO_بارگیری_CONFIG
    OPENSSL_INIT_NO_بارگیری_CONFIG |
#وإلا
    OPENSSL_INIT_بارگیری_CONFIG |
#پایان_اگر
    0;
  OPENSSL_init_ssl(flags, عدم);

#اگر_تعریف_نشده HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#پایان_اگر

  أرجع 1;
}

/* Global cleanup */
ساكن فراغ ossl_cleanup(فراغ)
{
#اگر_تعریف_نشده HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#پایان_اگر
}

/* Selects an OpenSSL crypto engine or provider.
 */
ساكن CURLcode ossl_مجموعه_engine(بنية Curl_easy *داده, بنية ossl_ctx *octx,
                                ثابت حرف *name)
{
#اگر_تعریف_شده USE_OPENSSL_ENGINE
  CURLcode نتیجه = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  إذا(e) {

    إذا(octx->p.engine) {
      ENGINE_پایان(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = عدم;
    }
    إذا(!ENGINE_init(e)) {
      حرف buf[256];

      ENGINE_free(e);
      failf(داده, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, حجم(buf)));
      نتیجه = CURLE_SSL_ENGINE_INITFAILED;
      e = عدم;
    }
    وإلا {
      نتیجه = CURLE_OK;
    }
    octx->p.engine = e;
    أرجع نتیجه;
  }
#پایان_اگر
#اگر_تعریف_شده OPENSSL_HAS_PROVIDERS
  أرجع ossl_مجموعه_provider(داده, octx, name);
#وإلا
  (فراغ)octx;
  (فراغ)name;
  failf(داده, "OpenSSL engine not found");
  أرجع CURLE_SSL_ENGINE_NOTFOUND;
#پایان_اگر
}

/* Sets engine as default for all SSL operations
 */
ساكن CURLcode ossl_مجموعه_engine_default(بنية Curl_easy *داده,
                                        بنية ossl_ctx *octx)
{
#اگر_تعریف_شده USE_OPENSSL_ENGINE
  إذا(octx->p.engine) {
    إذا(ENGINE_مجموعه_default(octx->p.engine, ENGINE_روش_ALL) > 0) {
      infof(داده, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    وإلا {
      failf(داده, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      أرجع CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#وإلا
  (فراغ)داده;
  (فراغ)octx;
#پایان_اگر
  أرجع CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
ساكن بنية curl_slist *ossl_engines_list(بنية Curl_easy *داده)
{
  بنية curl_slist *list = عدم;
#اگر_تعریف_شده USE_OPENSSL_ENGINE
  بنية curl_slist *beg;
  ENGINE *e;

  لكل(e = ENGINE_get_اولی(); e; e = ENGINE_get_بعدی(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    إذا(!beg) {
      curl_slist_free_all(list);
      أرجع عدم;
    }
    list = beg;
  }
#پایان_اگر
  (فراغ)داده;
  أرجع list;
}

#اگر_تعریف_شده OPENSSL_HAS_PROVIDERS

ساكن فراغ ossl_provider_cleanup(بنية ossl_ctx *octx)
{
  إذا(!octx)
    أرجع;
  إذا(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = عدم;
  }
  إذا(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = عدم;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = عدم;
  curlx_safefree(octx->p.propq);
  octx->p.provider_loaded = FALSE;
}

#تعریف_کن MAX_PROVIDER_LEN 128 /* reasonable */

/* Selects an OpenSSL crypto provider.
 *
 * A provider might need an associated property, a string passed on to
 * OpenSSL. Specify this as [PROVIDER][:PROPERTY]: separate the name and the
 * property with a colon. No colon means no property is set.
 *
 * An example provider + property looks like "tpm2:?provider=tpm2".
 */
ساكن CURLcode ossl_مجموعه_provider(بنية Curl_easy *داده,
                                  بنية ossl_ctx *octx,
                                  ثابت حرف *iname)
{
  حرف name[MAX_PROVIDER_LEN + 1];
  بنية Curl_str prov;
  ثابت حرف *propq = عدم;

  إذا(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    أرجع CURLE_OK;
  }
  إذا(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  إذا(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  کپی_حافظه(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  إذا(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    إذا(!libctx)
      أرجع CURLE_بیرون_از_حافظه;
    إذا(propq) {
      octx->p.propq = curlx_strdup(propq);
      إذا(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        أرجع CURLE_بیرون_از_حافظه;
      }
    }
    octx->p.libctx = libctx;
  }

#اگر_تعریف_نشده CURL_DISABLE_OPENSSL_AUTO_بارگیری_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  إذا(!OSSL_LIB_CTX_بارگیری_config(octx->p.libctx, عدم)) {
    infof(داده, "Failed to load default openssl config. Proceeding.");
  }
#پایان_اگر

  إذا(OSSL_PROVIDER_موجود(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    أرجع CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_بارگیری(octx->p.libctx, name, 1);
  إذا(!octx->p.provider) {
    حرف error_میانگیر[256];
    failf(داده, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_میانگیر, حجم(error_میانگیر)));
    ossl_provider_cleanup(octx);
    أرجع CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_بارگیری(octx->p.libctx, "base", 1);
  إذا(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(داده, "Failed to load base");
    أرجع CURLE_SSL_ENGINE_NOTFOUND;
  }
  وإلا
    octx->p.provider_loaded = TRUE;
  أرجع CURLE_OK;
}
#وإلا /* OPENSSL_HAS_PROVIDERS */
#تعریف_کن ossl_provider_cleanup(x)
#پایان_اگر

ساكن CURLcode ossl_shutdown(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              bool send_shutdown, bool *انجامشده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;
  حرف buf[1024];
  صحيح nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  إذا(!octx->ssl || cf->shutdown) {
    *انجامشده = TRUE;
    اذهب بیرون;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *انجامشده = FALSE;
  إذا(!(SSL_get_shutdown(octx->ssl) & SSL_فرستاده_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    لكل(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (صحيح)حجم(buf));
      CURL_TRC_CF(داده, cf, "SSL shutdown not sent, read -> %d", nread);
      إذا(nread <= 0)
        اكسر;
    }
    err = SSL_get_error(octx->ssl, nread);
    إذا(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool درون_داد_pending;
      /* Yes, it did. */
      إذا(!send_shutdown) {
        CURL_TRC_CF(داده, cf, "SSL shutdown received, not sending");
        *انجامشده = TRUE;
        اذهب بیرون;
      }
      وإلا إذا(!cf->بعدی->cft->is_alive(cf->بعدی, داده, &درون_داد_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(داده, cf, "peer closed connection");
        *انجامشده = TRUE;
        اذهب بیرون;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  إذا(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_فرستاده_SHUTDOWN)) {
    صحيح rc;
    ERR_clear_error();
    CURL_TRC_CF(داده, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    إذا(rc == 1) {
      CURL_TRC_CF(داده, cf, "SSL shutdown finished");
      *انجامشده = TRUE;
      اذهب بیرون;
    }
    إذا(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(داده, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      اذهب بیرون;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  لكل(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (صحيح)حجم(buf));
    CURL_TRC_CF(داده, cf, "SSL shutdown read -> %d", nread);
    إذا(nread <= 0)
      اكسر;
  }
  err = SSL_get_error(octx->ssl, nread);
  بدل(err) {
  حالة SSL_ERROR_ZERO_RETURN: /* no more data */
    إذا(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(داده, cf, "SSL shutdown finished");
    وإلا
      CURL_TRC_CF(داده, cf, "SSL shutdown not received, but closed");
    *انجامشده = TRUE;
    اكسر;
  حالة SSL_ERROR_NONE: /* did not get anything */
  حالة SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(داده, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    اكسر;
  حالة SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(داده, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    اكسر;
  افتراضي:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(بیعلامت دراز sslerr = ERR_get_error());
      CURL_TRC_CF(داده, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, حجم(buf)) :
                   SSL_ERROR_مقصد_str(err)),
                  SOCKERRNO);
    }
    *انجامشده = TRUE;
    نتیجه = CURLE_OK;
    اكسر;
  }

بیرون:
  cf->shutdown = (نتیجه || *انجامشده);
  إذا(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->درون_داد_pending = FALSE;
  أرجع نتیجه;
}

/* also used by quic-tls */
فراغ Curl_osslctx_cleanup(بنية ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  إذا(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = عدم;
  }
  إذا(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = عدم;
    octx->x509_store_راهاندازی = FALSE;
  }
  إذا(octx->bio_روش) {
    ossl_bio_cf_روش_free(octx->bio_روش);
    octx->bio_روش = عدم;
  }
#اگر_تعریف_شده USE_OPENSSL_ENGINE
  إذا(octx->p.engine) {
    ENGINE_پایان(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = عدم;
  }
#پایان_اگر
  ossl_provider_cleanup(octx);
}

ساكن فراغ ossl_close(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  (فراغ)داده;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
ساكن فراغ ossl_close_all(بنية Curl_easy *داده)
{
  (فراغ)داده;
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
ساكن CURLcode ossl_verifyhost(بنية Curl_easy *داده,
                                بنية connectdata *conn,
                                بنية ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  صحيح هدف; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  پشته_از(GENERAL_NAME) *altnames;
#اگر_تعریف_شده USE_IPV6
  بنية in6_addr addr;
#وإلا
  بنية in_addr addr;
#پایان_اگر
  CURLcode نتیجه = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = طول_رشته(peer->origin->hostname);

  (فراغ)conn;
  بدل(peer->type) {
  حالة CURL_SSL_PEER_IPV4:
    إذا(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    هدف = GEN_IPADD;
    addrlen = حجم(بنية in_addr);
    اكسر;
#اگر_تعریف_شده USE_IPV6
  حالة CURL_SSL_PEER_IPV6:
    إذا(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      أرجع CURLE_PEER_FAILED_VERIFICATION;
    هدف = GEN_IPADD;
    addrlen = حجم(بنية in6_addr);
    اكسر;
#پایان_اگر
  حالة CURL_SSL_PEER_DNS:
    هدف = GEN_DNS;
    اكسر;
  افتراضي:
    DEBUGASSERT(0);
    failf(داده, "unexpected SSL peer type: %d", (صحيح)peer->type);
    أرجع CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, عدم, عدم);

  إذا(altnames) {
#اگر_تعریف_شده HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#وإلا
    صحيح numalts;
    صحيح i;
#پایان_اگر

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_رقم(altnames);

    /* loop through all alternatives - until a dnsmatch */
    لكل(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ثابت GENERAL_NAME *check = sk_GENERAL_NAME_مقدار(altnames, i);

      إذا(check->type == GEN_DNS)
        dNSName = TRUE;
      وإلا إذا(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      إذا(check->type == هدف) {
        /* get data and length */
        ثابت حرف *altptr = (ثابت حرف *)ASN1_STRING_get0_داده(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_درازی(check->d.ia5);

        بدل(هدف) {
        حالة GEN_DNS: /* name/pattern comparison */
          إذا(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(داده, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (صحيح)altlen, altptr);
          }
          اكسر;

        حالة GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          إذا((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(داده, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          اكسر;
        }
      }
    }
    GENERAL_نامها_free(altnames);
  }

  إذا(matched)
    /* an alternative name matched */
    ;
  وإلا إذا(dNSName || iPAddress) {
    ثابت حرف *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(داده, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(داده, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    نتیجه = CURLE_PEER_FAILED_VERIFICATION;
  }
  وإلا {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    صحيح i = -1;
    بیعلامت حرف *cn = عدم;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ثابت X509_NAME *name = X509_get_subject_name(server_cert);
    إذا(name) {
      صحيح j;
      طالما((j = X509_NAME_get_شاخص_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    إذا(i >= 0) {
      ثابت ASN1_STRING *tmp =
        X509_NAME_مدخل_get_داده(X509_NAME_get_مدخل(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      إذا(tmp) {
        إذا(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_درازی(tmp);
          cn = (بیعلامت حرف *)CURL_UNCONST(ASN1_STRING_get0_داده(tmp));
        }
        وإلا { /* not a UTF8 name */
          ثابت صحيح utf8len = ASN1_STRING_مقصد_UTF8(&cn, tmp);
          إذا(utf8len < 0)
            cnlen = 0;
          وإلا
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        إذا((cnlen == 0) || !cn)
          نتیجه = CURLE_بیرون_از_حافظه;
        وإلا إذا(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(داده, "SSL: illegal cert name field");
          نتیجه = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    إذا(نتیجه)
      /* error already detected, pass through */
      ;
    وإلا إذا(!cn) {
      failf(داده, "SSL: unable to obtain common name from peer certificate");
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    }
    وإلا إذا(!Curl_cert_hostcheck((ثابت حرف *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(داده, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (صحيح)cnlen, cn,
            peer->origin->user_hostname);
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    }
    وإلا {
      infof(داده, " common name: %.*s (matched)", (صحيح)cnlen, cn);
    }
    إذا(free_cn)
      OPENSSL_free(cn);
  }

  أرجع نتیجه;
}

#اگر_تعریف_نشده OPENSSL_NO_OCSP
ساكن CURLcode verifystatus(بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده,
                             بنية ossl_ctx *octx)
{
  صحيح i, ocsp_وضعیت;
#اگر_تعریف_شده HAVE_BORINGSSL_LIKE
  ثابت uint8_t *وضعیت;
#وإلا
  بیعلامت حرف *وضعیت;
#پایان_اگر
  ثابت بیعلامت حرف *p;
  CURLcode نتیجه = CURLE_OK;
  OCSP_RESPONSE *rsp = عدم;
  OCSP_BASICRESP *br = عدم;
  X509_STORE     *st = عدم;
  پشته_از(X509) *ch = عدم;
  X509 *cert;
  OCSP_CERTID *id = عدم;
  صحيح cert_وضعیت, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  صحيح ret;
  دراز len;

  (فراغ)cf;
  DEBUGASSERT(octx);

  len = (دراز)SSL_get_tlsext_وضعیت_ocsp_resp(octx->ssl, &وضعیت);

  إذا(!وضعیت) {
    failf(داده, "No OCSP response received");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }
  p = وضعیت;
  rsp = d2i_OCSP_RESPONSE(عدم, &p, len);
  إذا(!rsp) {
    failf(داده, "Invalid OCSP response");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }

  ocsp_وضعیت = OCSP_response_وضعیت(rsp);
  إذا(ocsp_وضعیت != OCSP_RESPONSE_وضعیت_SUCCESSFUL) {
    failf(داده, "Invalid OCSP response status: %s (%d)",
          OCSP_response_وضعیت_str(ocsp_وضعیت), ocsp_وضعیت);
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }

  br = OCSP_response_get1_basic(rsp);
  إذا(!br) {
    failf(داده, "Invalid OCSP response");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }

#إذا !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_نسخه_عدد) && LIBRESSL_نسخه_عدد < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#وإلا
  ch = SSL_get_peer_cert_chain(octx->ssl);
#پایان_اگر
  إذا(!ch) {
    failf(داده, "Could not get peer certificate chain");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  إذا(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(داده, "OCSP response verification failed");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  إذا(!cert) {
    failf(داده, "Error getting peer certificate");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }

  لكل(i = 0; i < (صحيح)sk_X509_رقم(ch); i++) {
    X509 *issuer = sk_X509_مقدار(ch, (ossl_valsize_t)i);
    إذا(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_مقصد_id(EVP_sha1(), cert, issuer);
      اكسر;
    }
  }
  X509_free(cert);

  إذا(!id) {
    failf(داده, "Error computing OCSP ID");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_پیدا_کن_وضعیت(br, id, &cert_وضعیت, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  إذا(ret != 1) {
    failf(داده, "Could not find certificate ID in OCSP response");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  إذا(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(داده, "OCSP response has expired");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }

  infof(داده, "SSL certificate status: %s (%d)",
        OCSP_cert_وضعیت_str(cert_وضعیت), cert_وضعیت);

  بدل(cert_وضعیت) {
  حالة V_OCSP_CERTSTATUS_GOOD:
    اكسر;

  حالة V_OCSP_CERTSTATUS_REVOKED:
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    failf(داده, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    اذهب انتها;

  حالة V_OCSP_CERTSTATUS_UNKNOWN:
  افتراضي:
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب انتها;
  }

انتها:
  إذا(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  أرجع نتیجه;
}
#پایان_اگر

ساكن ثابت حرف *ssl_پیام_type(صحيح ssl_ver, صحيح پیام)
{
  إذا(ssl_ver == SSL3_نسخه_MAJOR) {
    بدل(پیام) {
    حالة SSL3_MT_HELLO_REQUEST:
      أرجع "Hello request";
    حالة SSL3_MT_CLIENT_HELLO:
      أرجع "Client hello";
    حالة SSL3_MT_SERVER_HELLO:
      أرجع "Server hello";
#اگر_تعریف_شده SSL3_MT_NEWSESSION_TICKET
    حالة SSL3_MT_NEWSESSION_TICKET:
      أرجع "Newsession Ticket";
#پایان_اگر
    حالة SSL3_MT_CERTIFICATE:
      أرجع "Certificate";
    حالة SSL3_MT_SERVER_کلید_EXCHANGE:
      أرجع "Server key exchange";
    حالة SSL3_MT_CLIENT_کلید_EXCHANGE:
      أرجع "Client key exchange";
    حالة SSL3_MT_CERTIFICATE_REQUEST:
      أرجع "Request CERT";
    حالة SSL3_MT_SERVER_انجامشده:
      أرجع "Server finished";
    حالة SSL3_MT_CERTIFICATE_VERIFY:
      أرجع "CERT verify";
    حالة SSL3_MT_پایانیافته:
      أرجع "Finished";
#اگر_تعریف_شده SSL3_MT_CERTIFICATE_وضعیت
    حالة SSL3_MT_CERTIFICATE_وضعیت:
      أرجع "Certificate Status";
#پایان_اگر
#اگر_تعریف_شده SSL3_MT_ENCRYPTED_EXTENSIONS
    حالة SSL3_MT_ENCRYPTED_EXTENSIONS:
      أرجع "Encrypted Extensions";
#پایان_اگر
#اگر_تعریف_شده SSL3_MT_SUPPLEMENTAL_داده
    حالة SSL3_MT_SUPPLEMENTAL_داده:
      أرجع "Supplemental data";
#پایان_اگر
#اگر_تعریف_شده SSL3_MT_انتها_از_EARLY_داده
    حالة SSL3_MT_انتها_از_EARLY_داده:
      أرجع "End of early data";
#پایان_اگر
#اگر_تعریف_شده SSL3_MT_کلید_UPDATE
    حالة SSL3_MT_کلید_UPDATE:
      أرجع "Key update";
#پایان_اگر
#اگر_تعریف_شده SSL3_MT_بعدی_PROTO
    حالة SSL3_MT_بعدی_PROTO:
      أرجع "Next protocol";
#پایان_اگر
#اگر_تعریف_شده SSL3_MT_پیام_HASH
    حالة SSL3_MT_پیام_HASH:
      أرجع "Message hash";
#پایان_اگر
    }
  }
  أرجع "Unknown";
}

ساكن ثابت حرف *tls_rt_type(صحيح type)
{
  بدل(type) {
#اگر_تعریف_شده SSL3_RT_سرآیند
  حالة SSL3_RT_سرآیند:
    أرجع "TLS header";
#پایان_اگر
  حالة SSL3_RT_CHANGE_CIPHER_SPEC:
    أرجع "TLS change cipher";
  حالة SSL3_RT_ALERT:
    أرجع "TLS alert";
  حالة SSL3_RT_HANDSHAKE:
    أرجع "TLS handshake";
  حالة SSL3_RT_APPLICATION_داده:
    أرجع "TLS app data";
  افتراضي:
    أرجع "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
ساكن فراغ ossl_trace(صحيح direction, صحيح ssl_ver, صحيح content_type,
                       ثابت فراغ *buf, size_t len, SSL *ssl,
                       فراغ *userp)
{
  ثابت حرف *verstr;
  بنية Curl_cfilter *cf = userp;
  بنية Curl_easy *داده = عدم;
  حرف unknown[32];

  إذا(!cf)
    أرجع;
  داده = CF_داده_CURRENT(cf);
  إذا(!داده || !داده->مجموعه.fdebug || (direction && direction != 1))
    أرجع;

  بدل(ssl_ver) {
#اگر_تعریف_شده SSL3_نسخه
  حالة SSL3_نسخه:
    verstr = "SSLv3";
    اكسر;
#پایان_اگر
  حالة TLS1_نسخه:
    verstr = "TLSv1.0";
    اكسر;
#اگر_تعریف_شده TLS1_1_نسخه
  حالة TLS1_1_نسخه:
    verstr = "TLSv1.1";
    اكسر;
#پایان_اگر
#اگر_تعریف_شده TLS1_2_نسخه
  حالة TLS1_2_نسخه:
    verstr = "TLSv1.2";
    اكسر;
#پایان_اگر
  حالة TLS1_3_نسخه:
    verstr = "TLSv1.3";
    اكسر;
  افتراضي:
    curl_msnprintf(unknown, حجم(unknown), "(%x)", (بیعلامت صحيح)ssl_ver);
    verstr = unknown;
    اكسر;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  إذا(ssl_ver
#اگر_تعریف_شده SSL3_RT_سرآیند
     && content_type != SSL3_RT_سرآیند
#پایان_اگر
#اگر_تعریف_شده SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#پایان_اگر
    ) {
    ثابت حرف *پیام_name = "Truncated message";
    ثابت حرف *tls_rt_name;
    حرف ssl_buf[1024];
    صحيح پیام_type = 0;
    صحيح txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    إذا(ssl_ver == SSL3_نسخه_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    وإلا
      tls_rt_name = "";

    إذا(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      إذا(len) {
        پیام_type = *(ثابت بیعلامت حرف *)buf;
        پیام_name = "Change cipher spec";
      }
    }
    وإلا إذا(content_type == SSL3_RT_ALERT) {
      إذا(len >= 2) {
        پیام_type =
          (((ثابت بیعلامت حرف *)buf)[0] << 8) +
           ((ثابت بیعلامت حرف *)buf)[1];
        پیام_name = SSL_alert_desc_string_long(پیام_type);
      }
    }
    وإلا إذا(len) {
      پیام_type = *(ثابت بیعلامت حرف *)buf;
      پیام_name = ssl_پیام_type(ssl_ver, پیام_type);
    }

    txt_len = curl_msnprintf(ssl_buf, حجم(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, پیام_name, پیام_type);
    Curl_debug(داده, CURLINFO_متن, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(داده, (direction == 1) ? CURLINFO_SSL_داده_بیرون :
             CURLINFO_SSL_داده_درون, (ثابت حرف *)buf, len);
  (فراغ)ssl;
}

ساكن CURLcode ossl_مجموعه_ssl_نسخه_min_max(بنية Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             بیعلامت صحيح ssl_نسخه_min)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  دراز curl_ssl_نسخه_min = (دراز)ssl_نسخه_min;
  دراز curl_ssl_نسخه_max;

  /* convert curl min SSL version option to OpenSSL constant */
#إذا defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_نسخه_عدد)
  uint16_t ossl_ssl_نسخه_min = 0;
  uint16_t ossl_ssl_نسخه_max = 0;
#وإلا
  دراز ossl_ssl_نسخه_min = 0;
  دراز ossl_ssl_نسخه_max = 0;
#پایان_اگر
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_نسخه_min != CURL_SSLVERSION_DEFAULT);
  بدل(curl_ssl_نسخه_min) {
  حالة CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  حالة CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_نسخه_min = TLS1_نسخه;
    اكسر;
  حالة CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_نسخه_min = TLS1_1_نسخه;
    اكسر;
  حالة CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_نسخه_min = TLS1_2_نسخه;
    اكسر;
  حالة CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_نسخه_min = TLS1_3_نسخه;
    اكسر;
  }

  /* ... then, TLS max version */
  curl_ssl_نسخه_max = (دراز)conn_config->نسخه_max;

  /* convert curl max SSL version option to OpenSSL constant */
  بدل(curl_ssl_نسخه_max) {
  حالة CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_نسخه_max = TLS1_نسخه;
    اكسر;
  حالة CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_نسخه_max = TLS1_1_نسخه;
    اكسر;
  حالة CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_نسخه_max = TLS1_2_نسخه;
    اكسر;
  حالة CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_نسخه_max = TLS1_3_نسخه;
    اكسر;
  حالة CURL_SSLVERSION_MAX_NONE:  /* none selected */
  حالة CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  افتراضي:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_نسخه_max = 0;
    اكسر;
  }

  إذا(!SSL_CTX_مجموعه_min_proto_نسخه(ctx, ossl_ssl_نسخه_min) ||
     !SSL_CTX_مجموعه_max_proto_نسخه(ctx, ossl_ssl_نسخه_max))
    أرجع CURLE_SSL_CONNECT_ERROR;

  أرجع CURLE_OK;
}

CURLcode Curl_ossl_جمع_کن_session(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية ossl_ctx *octx,
                               ثابت حرف *ssl_peer_کلید,
                               SSL_SESSION *session,
                               ثابت حرف *alpn,
                               بیعلامت حرف *quic_tp,
                               size_t quic_tp_len,
                               بنية Curl_ssl_session **psession)
{
  بنية Curl_ssl_session *sc_session = عدم, *sc_dup = عدم;
  بیعلامت حرف *der_session_buf = عدم;
  بیعلامت حرف *qtp_clone = عدم;
  CURLcode نتیجه = CURLE_OK;

  إذا(psession)
    *psession = عدم;
  إذا(!cf || !داده)
    اذهب بیرون;

  إذا(Curl_ssl_scache_use(cf, داده)) {
    size_t der_session_اندازه;
    بیعلامت حرف *der_session_ptr;
    size_t earlydata_max = 0;
    صحيح ietf_tls_id = SSL_نسخه(octx->ssl);

    der_session_اندازه = i2d_SSL_SESSION(session, عدم);
    إذا(der_session_اندازه == 0) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_اندازه);
    إذا(!der_session_buf) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }

    der_session_اندازه = i2d_SSL_SESSION(session, &der_session_ptr);
    إذا(der_session_اندازه == 0) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }

#اگر_تعریف_شده HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_داده(session);
#پایان_اگر
    إذا(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ثابت حرف *)quic_tp, quic_tp_len);
      إذا(!qtp_clone) {
        نتیجه = CURLE_بیرون_از_حافظه;
        اذهب بیرون;
      }
    }

    نتیجه = Curl_ssl_session_create2(der_session_buf, der_session_اندازه,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)زمان(عدم) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = عدم;  /* took ownership of sdata */
#اگر_تعریف_شده USE_APPLE_SECTRUST
    إذا(!نتیجه)
      sc_session->sectrust_verified = octx->sectrust_verified;
#پایان_اگر
    إذا(!نتیجه && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        نتیجه = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    إذا(!نتیجه) {
      نتیجه = Curl_ssl_scache_put(cf, داده, ssl_peer_کلید, sc_session);
      /* took ownership of `sc_session` */
      sc_session = عدم;
    }
  }

بیرون:
  curlx_free(der_session_buf);
  إذا(!نتیجه && psession) {
    *psession = sc_dup;
    sc_dup = عدم;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  أرجع نتیجه;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ساكن صحيح ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  بنية Curl_cfilter *cf = (بنية Curl_cfilter *)SSL_get_app_داده(ssl);
  إذا(cf) {
    بنية Curl_easy *داده = CF_داده_CURRENT(cf);
    بنية ssl_connect_داده *connssl = cf->ctx;
    بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
    بنية Curl_ssl_session *session = عدم;
    Curl_ossl_جمع_کن_session(cf, داده, octx, connssl->peer.scache_کلید,
                          ssl_sessionid, connssl->negotiated.alpn, عدم,
                          0, &session);
    إذا(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  أرجع 0;
}

ساكن CURLcode بارگیری_cacert_from_حافظه(X509_STORE *store,
                                        ثابت بنية curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = عدم;
  پشته_از(X509_INFO) *inf = عدم;

  /* everything else is a reference */
  صحيح i, شمار = 0;
  X509_INFO *itmp = عدم;

  إذا(ca_info_blob->len > (size_t)بیشترین_صحیح)
    أرجع CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->داده, (صحيح)ca_info_blob->len);
  إذا(!cbio)
    أرجع CURLE_بیرون_از_حافظه;

  inf = PEM_X509_INFO_read_bio(cbio, عدم, عدم, عدم);
  إذا(!inf) {
    BIO_free(cbio);
    أرجع CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  لكل(i = 0; i < (صحيح)sk_X509_INFO_رقم(inf); ++i) {
    itmp = sk_X509_INFO_مقدار(inf, (ossl_valsize_t)i);
    إذا(itmp->x509) {
      إذا(X509_STORE_جمع_کن_cert(store, itmp->x509)) {
        ++شمار;
      }
      وإلا {
        /* set count to 0 to return an error */
        شمار = 0;
        اكسر;
      }
    }
    إذا(itmp->crl) {
      إذا(X509_STORE_جمع_کن_crl(store, itmp->crl)) {
        ++شمار;
      }
      وإلا {
        /* set count to 0 to return an error */
        شمار = 0;
        اكسر;
      }
    }
  }

#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic افزودن
#رهنمود clang diagnostic ignored "-Wcast-function-type-strict"
#پایان_اگر
  sk_X509_INFO_برداشت_free(inf, X509_INFO_free);
#إذا defined(__clang__) && __clang_major__ >= 16
#رهنمود clang diagnostic برداشت
#پایان_اگر
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  أرجع (شمار > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#اگر_تعریف_شده USE_WIN32_CRYPTO
ساكن CURLcode ossl_win_بارگیری_store(بنية Curl_easy *داده,
                                    بنية Curl_cfilter *cf,
                                    ثابت حرف *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode نتیجه = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  إذا(hStore) {
    PCCERT_CONTEXT pContext = عدم;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = عدم;
    DWORD enhkey_usage_اندازه = 0;
    VERBOSE(size_t مجموع = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    نتیجه = CURLE_OK;
    لكل(;;) {
      X509 *x509;
      FILETIME now;
      BYTE کلید_usage[2];
      DWORD req_اندازه;
      ثابت بیعلامت حرف *encoded_cert;
      pContext = Cert_Enum_Certificates_درون_Store(hStore, pContext);
      إذا(!pContext)
        اكسر;

      VERBOSE(++مجموع);

#إذا defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        حرف cert_name[256];
        إذا(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               عدم, cert_name, حجم(cert_name)))
          infof(داده, "SSL: unknown cert name");
        وإلا
          infof(داده, "SSL: Checking cert \"%s\"", cert_name);
      }
#پایان_اگر
      encoded_cert = (ثابت بیعلامت حرف *)pContext->pbCertEncoded;
      إذا(!encoded_cert)
        استمر;

      Get_System_زمان_As_File_زمان(&now);
      إذا(Compare_File_زمان(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         Compare_File_زمان(&now, &pContext->pCertInfo->NotAfter) > 0)
        استمر;

      /* If key usage exists check for signing attribute */
      إذا(Cert_Get_Intended_کلید_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 کلید_usage, حجم(کلید_usage))) {
        إذا(!(کلید_usage[0] & CERT_کلید_CERT_SIGN_کلید_USAGE))
          استمر;
      }
      وإلا إذا(Get_آخرین_Error())
        استمر;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      إذا(Cert_Get_Enhanced_کلید_Usage(pContext, 0, عدم, &req_اندازه) && req_اندازه) {
        إذا(req_اندازه > enhkey_usage_اندازه) {
          فراغ *tmp = curlx_realloc(enhkey_usage, req_اندازه);

          إذا(!tmp) {
            failf(داده, "SSL: Out of memory allocating for OID list");
            نتیجه = CURLE_بیرون_از_حافظه;
            اكسر;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_اندازه = req_اندازه;
        }

        إذا(Cert_Get_Enhanced_کلید_Usage(pContext, 0, enhkey_usage, &req_اندازه)) {
          إذا(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            إذا((HRESULT)Get_آخرین_Error() != CRYPT_E_NOT_یافت)
              استمر;
          }
          وإلا {
            DWORD i;
            bool یافت = FALSE;

            لكل(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              إذا(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                یافت = TRUE;
                اكسر;
              }
            }

            إذا(!یافت)
              استمر;
          }
        }
        وإلا
          استمر;
      }
      وإلا
        استمر;

      x509 = d2i_X509(عدم, &encoded_cert, (دراز)pContext->cbCertEncoded);
      إذا(!x509)
        استمر;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      إذا(X509_STORE_جمع_کن_cert(store, x509) == 1) {
        VERBOSE(++imported);
#اگر_تعریف_شده DEBUGBUILD
        infof(داده, "SSL: Imported cert");
#پایان_اگر
        *padded = TRUE;
      }
      X509_free(x509);
    }

    curlx_free(enhkey_usage);
    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    CURL_TRC_CF(داده, cf,
                "ossl_win_load_store() found: %zu imported: %zu certs in %s.",
                مجموع, imported, win_store);

    إذا(نتیجه)
      أرجع نتیجه;
  }

  أرجع نتیجه;
}

ساكن CURLcode ossl_windows_بارگیری_anchors(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *داده,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  ساكن ثابت حرف * ثابت win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode نتیجه = CURLE_OK;

  *padded = FALSE;
  لكل(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    نتیجه = ossl_win_بارگیری_store(داده, cf, win_stores[i], store, &store_added);
    إذا(نتیجه)
      أرجع نتیجه;
    إذا(store_added) {
      CURL_TRC_CF(داده, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    وإلا
      infof(داده, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  أرجع نتیجه;
}

#پایان_اگر /* USE_WIN32_CRYPTO */

ساكن CURLcode ossl_بارگیری_trust_anchors(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده,
                                        بنية ossl_ctx *octx,
                                        X509_STORE *store)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? عدم : conn_config->CAfile);
  ثابت حرف * ثابت ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_خالی = TRUE;
  إذا(conn_config->native_ca_store) {
#اگر_تعریف_شده USE_WIN32_CRYPTO
    bool added = FALSE;
    نتیجه = ossl_windows_بارگیری_anchors(cf, داده, store, &added);
    إذا(نتیجه)
      أرجع نتیجه;
    إذا(added) {
      infof(داده, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_خالی = FALSE;
    }
#وإلاإذا defined(USE_APPLE_SECTRUST)
    infof(داده, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#پایان_اگر
  }

  إذا(conn_config->ca_info_blob) {
    نتیجه = بارگیری_cacert_from_حافظه(store, conn_config->ca_info_blob);
    إذا(نتیجه) {
      failf(داده, "error adding trust anchors from certificate blob: %d",
            (صحيح)نتیجه);
      أرجع نتیجه;
    }
    infof(داده, "  CA Blob from configuration");
    octx->store_is_خالی = FALSE;
  }

  إذا(ssl_cafile || ssl_capath) {
#اگر_تعریف_شده HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    إذا(ssl_cafile) {
      إذا(!X509_STORE_بارگیری_file(store, ssl_cafile)) {
        إذا(octx->store_is_خالی && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(داده, "error adding trust anchors from file: %s", ssl_cafile);
          أرجع CURLE_SSL_CACERT_BADFILE;
        }
        وإلا
          infof(داده, "error setting certificate file, continuing anyway");
      }
      infof(داده, "  CAfile: %s", ssl_cafile);
      octx->store_is_خالی = FALSE;
    }
    إذا(ssl_capath) {
      إذا(!X509_STORE_بارگیری_مسیر(store, ssl_capath)) {
        إذا(octx->store_is_خالی && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(داده, "error adding trust anchors from path: %s", ssl_capath);
          أرجع CURLE_SSL_CACERT_BADFILE;
        }
        وإلا
          infof(داده, "error setting certificate path, continuing anyway");
      }
      infof(داده, "  CApath: %s", ssl_capath);
      octx->store_is_خالی = FALSE;
    }
#وإلا
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    إذا(!X509_STORE_بارگیری_locations(store, ssl_cafile, ssl_capath)) {
      إذا(octx->store_is_خالی && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(داده, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
      وإلا {
        infof(داده, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    إذا(ssl_cafile)
      infof(داده, "  CAfile: %s", ssl_cafile);
    إذا(ssl_capath)
      infof(داده, "  CApath: %s", ssl_capath);
    octx->store_is_خالی = FALSE;
#پایان_اگر
  }

#اگر_تعریف_شده CURL_CA_FALLBACK
  إذا(octx->store_is_خالی) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_مجموعه_default_paths(store);
    infof(داده, "  OpenSSL default paths (fallback)");
    octx->store_is_خالی = FALSE;
  }
#پایان_اگر
  إذا(octx->store_is_خالی && !have_native_check)
    infof(داده, "  no trust anchors configured");

  أرجع نتیجه;
}

ساكن CURLcode ossl_populate_x509_store(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *داده,
                                         بنية ossl_ctx *octx,
                                         X509_STORE *store)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجه = CURLE_OK;
  X509_LOOKUP *lookup = عدم;
  ثابت حرف * ثابت ssl_crlfile = conn_config->CRLfile;
  بیعلامت دراز x509flags = 0;

  CURL_TRC_CF(داده, cf, "configuring OpenSSL's x509 trust store");
  إذا(!store)
    أرجع CURLE_بیرون_از_حافظه;

  إذا(!conn_config->verifypeer) {
    infof(داده, "SSL Trust: peer verification disabled");
    أرجع CURLE_OK;
  }

  infof(داده, "SSL Trust Anchors:");
  نتیجه = ossl_بارگیری_trust_anchors(cf, داده, octx, store);
  إذا(نتیجه)
    أرجع نتیجه;

  /* Does not make sense to load a CRL file without peer verification */
#اگر_تعریف_شده USE_APPLE_SECTRUST
  إذا(ssl_crlfile && conn_config->native_ca_store) {
    failf(داده, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    أرجع CURLE_NOT_BUILT_درون;
  }
#پایان_اگر
  إذا(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_جمع_کن_lookup(store, X509_LOOKUP_file());
    إذا(!lookup ||
       (!X509_بارگیری_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(داده, "error loading CRL file: %s", ssl_crlfile);
      أرجع CURLE_SSL_CRL_BADFILE;
    }
    x509flags = X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL;
    infof(داده, " CRLfile: %s", ssl_crlfile);
  }

  /* Try building a chain using issuers in the trusted store first to avoid
     problems with server-sent legacy intermediates. Newer versions of
     OpenSSL do alternate chain checking by default but we do not know how to
     determine that in a reliable manner.
     https://web.archive.org/web/20190422050538/rt.openssl.org/Ticket/Display.html?id=3621
   */
  x509flags |= X509_V_FLAG_TRUSTED_اولی;

  إذا(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (فراغ)X509_STORE_مجموعه_flags(store, x509flags);

  أرجع نتیجه;
}

/* key to use at `multi->proto_hash` */
#تعریف_کن MPROTO_OSSL_X509_کلید  "tls:ossl:x509:share"

بنية ossl_x509_share {
  حرف *CAfile;         /* CAfile path used to generate X509 store */
  حرف *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  بنية curltime زمان; /* when the cached store was created */
  بیت(store_is_خالی);  /* no certs/paths/blobs are in the store */
  بیت(no_partialchain); /* keep partial chain state */
};

ساكن فراغ oss_x509_share_free(ثابت فراغ *کلید, size_t کلید_len, فراغ *p)
{
  بنية ossl_x509_share *share = p;
  DEBUGASSERT(کلید_len == CURL_CSTRLEN(MPROTO_OSSL_X509_کلید));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_کلید, کلید, کلید_len));
  (فراغ)کلید;
  (فراغ)کلید_len;
  إذا(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

ساكن bool ossl_cached_x509_store_expired(بنية Curl_easy *داده,
                                           ثابت بنية ossl_x509_share *mb)
{
  إذا(داده->مجموعه.ssl_ca_cache_timeout < 0)
    أرجع FALSE;
  وإلا {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(داده), &mb->زمان);
    timediff_t timeout_ms = داده->مجموعه.ssl_ca_cache_timeout * (timediff_t)1000;

    أرجع elapsed_ms >= timeout_ms;
  }
}

ساكن bool ossl_cached_x509_store_different(بنية Curl_cfilter *cf,
                                             ثابت بنية Curl_easy *داده,
                                             ثابت بنية ossl_x509_share *mb)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (فراغ)داده;
  إذا(mb->no_partialchain != conn_config->no_partialchain)
    أرجع TRUE;
  إذا(!mb->CAfile || !conn_config->CAfile) {
    إذا(mb->CAfile != conn_config->CAfile)
      أرجع TRUE;
  }
  وإلا إذا(strcmp(mb->CAfile, conn_config->CAfile))
    أرجع TRUE;

  إذا(!mb->CApath || !conn_config->CApath) {
    إذا(mb->CApath != conn_config->CApath)
      أرجع TRUE;
  }
  وإلا إذا(strcmp(mb->CApath, conn_config->CApath))
    أرجع TRUE;

  أرجع FALSE;
}

ساكن X509_STORE *ossl_get_cached_x509_store(بنية Curl_cfilter *cf,
                                              بنية Curl_easy *داده,
                                              bool *pempty)
{
  بنية Curl_multi *multi = داده->multi;
  بنية ossl_x509_share *share;
  X509_STORE *store = عدم;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_کلید,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_کلید)) : عدم;
  إذا(share && share->store &&
     !ossl_cached_x509_store_expired(داده, share) &&
     !ossl_cached_x509_store_different(cf, داده, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_خالی;
  }

  أرجع store;
}

ساكن فراغ ossl_مجموعه_cached_x509_store(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *داده,
                                       X509_STORE *store,
                                       bool is_خالی)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_multi *multi = داده->multi;
  بنية ossl_x509_share *share;

  DEBUGASSERT(multi);
  إذا(!multi)
    أرجع;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_کلید,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_کلید));

  إذا(!share) {
    share = curlx_calloc(1, حجم(*share));
    إذا(!share)
      أرجع;
    إذا(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_کلید,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_کلید),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      أرجع;
    }
  }

  إذا(X509_STORE_به_بالا_ref(store)) {
    حرف *CAfile = عدم;
    حرف *CApath = عدم;

    إذا(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      إذا(!CAfile) {
        X509_STORE_free(store);
        أرجع;
      }
    }

    إذا(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      إذا(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        أرجع;
      }
    }

    إذا(share->store) {
      X509_STORE_free(share->store);
      curlx_free(share->CAfile);
      curlx_free(share->CApath);
    }

    share->زمان = *Curl_pgrs_now(داده);
    share->store = store;
    share->store_is_خالی = is_خالی;
    share->CAfile = CAfile;
    share->CApath = CApath;
    share->no_partialchain = conn_config->no_partialchain;
  }
}

#اگر_تعریف_شده OPENSSL_HAS_PROVIDERS
/* CApath certificates loaded under a provider libctx can outlive that libctx
   when the store is shared with the multi handle. */
#تعریف_کن USING_PROVIDER(octx) octx->p.libctx
#وإلا
#تعریف_کن USING_PROVIDER(x) 0
#پایان_اگر

CURLcode Curl_ssl_راهاندازی_x509_store(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية ossl_ctx *octx)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجه = CURLE_OK;
  X509_STORE *cached_store;
  bool cache_criteria_met, is_خالی;

  /* Consider the X509 store cacheable if it comes from CAfile/CApath,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (داده->مجموعه.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !USING_PROVIDER(octx);

  ERR_مجموعه_mark();

  cached_store = ossl_get_cached_x509_store(cf, داده, &is_خالی);
  إذا(cached_store && cache_criteria_met && X509_STORE_به_بالا_ref(cached_store)) {
    SSL_CTX_مجموعه_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_خالی = is_خالی;
  }
  وإلا {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    نتیجه = ossl_populate_x509_store(cf, داده, octx, store);
    إذا(نتیجه == CURLE_OK && cache_criteria_met) {
      ossl_مجموعه_cached_x509_store(cf, داده, store, (bool)octx->store_is_خالی);
    }
  }

  ERR_برداشت_مقصد_mark();

  أرجع نتیجه;
}

ساكن bool ossl_apply_session(
  بنية ossl_ctx *octx,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *داده,
  بنية alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  بنية Curl_ssl_session *scs)
{
  بنية ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ثابت بیعلامت حرف *der_sessionid = scs->sdata;
  size_t der_sessionid_اندازه = scs->sdata_len;
  SSL_SESSION *ssl_session = عدم;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(عدم, &der_sessionid,
                                (دراز)der_sessionid_اندازه);
  إذا(ssl_session) {
    إذا(!SSL_مجموعه_session(octx->ssl, ssl_session)) {
      VERBOSE(حرف error_میانگیر[256]);
      infof(داده, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_میانگیر,
                          حجم(error_میانگیر)));
    }
    وإلا {
      إذا(conn_cfg->verifypeer &&
         (SSL_get_verify_نتیجه(octx->ssl) != X509_V_OK)
#اگر_تعریف_شده USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#پایان_اگر
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_مجموعه_session(octx->ssl, عدم);
        infof(داده, "SSL session not peer verified, not reusing");
      }
      وإلا {
        infof(داده, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#اگر_تعریف_شده USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#پایان_اگر
        infof(داده, "SSL verify result: %lx",
              (بیعلامت دراز)SSL_get_verify_نتیجه(octx->ssl));
#اگر_تعریف_شده HAVE_OPENSSL_EARLYDATA
        إذا(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_داده(ssl_session) &&
           !cf->conn->بیتها.connect_only &&
           (SSL_نسخه(octx->ssl) == TLS1_3_نسخه)) {
          bool do_early_داده = FALSE;
          إذا(sess_reuse_cb)
            (فراغ)sess_reuse_cb(cf, داده, alpns, scs, &do_early_داده);
          إذا(do_early_داده) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_مقصد(alpns, scs->alpn);
          }
        }
#وإلا
        (فراغ)alpns;
        (فراغ)conn_cfg;
        (فراغ)sess_reuse_cb;
#پایان_اگر
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  وإلا {
    infof(داده, "SSL session not accepted by OpenSSL, continuing without");
  }
  أرجع (bool)octx->reused_session;
}

ساكن CURLcode ossl_init_session_and_alpns(
  بنية ossl_ctx *octx,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *داده,
  بنية ssl_peer *peer,
  ثابت بنية alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  بنية ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  بنية alpn_spec alpns;
  CURLcode نتیجه;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  إذا((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    بنية Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(داده, cf->cft, FIRSTSOCKET);
    إذا(scs) {
      إذا(ossl_apply_session(octx, cf, داده, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(داده, cf, "applied SSL session from control connection");
    }
  }

  إذا(!octx->reused_session &&
     Curl_ssl_scache_use(cf, داده) && !conn_cfg->verifystatus) {
    بنية Curl_ssl_session *scs = عدم;

    نتیجه = Curl_ssl_scache_take(cf, داده, peer->scache_کلید, &scs);
    إذا(!نتیجه && scs && scs->sdata && scs->sdata_len) {
      (فراغ)ossl_apply_session(octx, cf, داده, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, داده, peer->scache_کلید, scs);
  }

  إذا(alpns.شمار) {
    بنية alpn_proto_buf proto;
    memset(&proto, 0, حجم(proto));
    نتیجه = Curl_alpn_مقصد_proto_buf(&proto, &alpns);
    إذا(نتیجه) {
      failf(داده, "Error determining ALPN");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    إذا(SSL_مجموعه_alpn_protos(octx->ssl, proto.داده, proto.len)) {
      failf(داده, "Error setting ALPN");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  أرجع CURLE_OK;
}

#اگر_تعریف_شده HAVE_SSL_SET1_ECH_CONFIG_LIST
ساكن CURLcode ossl_init_ech(بنية ossl_ctx *octx,
                              بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              بنية ssl_peer *peer)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح trying_ech_now = 0;

  إذا(!Curl_ssl_ech_enabled(cf))
    أرجع CURLE_OK;

  إذا(conn_config->ech == CURLECH_GREASE) {
    infof(داده, "ECH: will GREASE ClientHello");
#اگر_تعریف_شده HAVE_BORINGSSL_LIKE
    SSL_مجموعه_enable_ech_grease(octx->ssl, 1);
#وإلا
    SSL_مجموعه_options(octx->ssl, SSL_OP_ECH_GREASE);
#پایان_اگر
  }
  وإلا إذا(conn_config->ech && conn_config->ech_config) {
#اگر_تعریف_شده HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode نتیجه;

    إذا(!conn_config->ech_config[0]) {
      infof(داده, "ECH: ECHConfig from command line empty");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * طول_رشته(conn_config->ech_config);
    نتیجه = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    إذا(نتیجه || !ech_config) {
      infof(داده, "ECH: cannot base64 decode ECHConfig from command line");
      إذا(conn_config->ech == CURLECH_HARD)
        أرجع نتیجه;
    }
    إذا(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(داده, "ECH: SSL_ECH_set1_ech_config_list failed");
      إذا(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#وإلا
    size_t ech_config_len = 0;
    إذا(!conn_config->ech_config[0]) {
      infof(داده, "ECH: ECHConfig from command line empty");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = طول_رشته(conn_config->ech_config);
    إذا(SSL_set1_ech_config_list(octx->ssl,
                                (ثابت uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(داده, "ECH: SSL_ECH_set1_ech_config_list failed");
      إذا(conn_config->ech == CURLECH_HARD)
        أرجع CURLE_SSL_CONNECT_ERROR;
    }
    وإلا
      trying_ech_now = 1;
#پایان_اگر /* HAVE_BORINGSSL_LIKE */
    infof(داده, "ECH: ECHConfig from command line");
  }
  وإلا {
    ثابت بنية Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(داده, cf->sockindex, peer->origin);

    إذا(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ثابت بیعلامت حرف *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(داده, "ECH: ECHConfig from HTTPS RR");
      إذا(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(داده, "ECH: SSL_set1_ech_config_list failed");
        إذا(conn_config->ech == CURLECH_HARD)
          أرجع CURLE_SSL_CONNECT_ERROR;
      }
      وإلا {
        trying_ech_now = 1;
        infof(داده, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    وإلا {
      infof(داده, "ECH: requested but no ECHConfig available");
      إذا(conn_config->ech == CURLECH_HARD)
        أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }
#اگر_تعریف_شده HAVE_BORINGSSL_LIKE
  (فراغ)peer;
  إذا(trying_ech_now && conn_config->ech_عمومی) {
    infof(داده, "ECH: setting public_name not supported with BoringSSL");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }
#وإلا
  إذا(trying_ech_now && conn_config->ech_عمومی) {
    صحيح ret;
    infof(داده, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_عمومی);
    ret = SSL_ech_set1_server_نامها(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_عمومی,
                                    0 /* do send outer */);
    إذا(ret != 1) {
      infof(داده, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }
#پایان_اگر /* HAVE_BORINGSSL_LIKE */
  إذا(trying_ech_now &&
     SSL_مجموعه_min_proto_نسخه(octx->ssl, TLS1_3_نسخه) != 1) {
    infof(داده, "ECH: cannot force TLSv1.3 [ERROR]");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  أرجع CURLE_OK;
}
#پایان_اگر /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

ساكن CURLcode ossl_init_ssl(بنية ossl_ctx *octx,
                              بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              بنية ssl_peer *peer,
                              ثابت بنية alpn_spec *alpns_requested,
                              فراغ *ssl_user_داده,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  إذا(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  إذا(!octx->ssl) {
    failf(داده, "SSL: could not create a context (handle)");
    أرجع CURLE_بیرون_از_حافظه;
  }

  SSL_مجموعه_app_داده(octx->ssl, ssl_user_داده);

#اگر_تعریف_نشده OPENSSL_NO_OCSP
  إذا(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_مجموعه_tlsext_وضعیت_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#پایان_اگر

  SSL_مجموعه_connect_state(octx->ssl);

  إذا(peer->sni) {
    إذا(!SSL_مجموعه_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(داده, "Failed set SNI");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

#اگر_تعریف_شده HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode نتیجه = ossl_init_ech(octx, cf, داده, peer);
    إذا(نتیجه)
      أرجع نتیجه;
  }
#پایان_اگر /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  أرجع ossl_init_session_and_alpns(octx, cf, داده, peer,
                                     alpns_requested, sess_reuse_cb);
}

ساكن CURLcode ossl_init_روش(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية ssl_peer *peer,
                                 ثابت SSL_روش **pmethod,
                                 بیعلامت صحيح *pssl_نسخه_min)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = عدم;
  *pssl_نسخه_min = conn_config->نسخه_2;
  DEBUGASSERT(conn_config->نسخه_2 != CURL_SSLVERSION_DEFAULT);
  بدل(peer->transport) {
  حالة TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    بدل(*pssl_نسخه_min) {
    حالة CURL_SSLVERSION_TLSv1:
    حالة CURL_SSLVERSION_TLSv1_0:
    حالة CURL_SSLVERSION_TLSv1_1:
    حالة CURL_SSLVERSION_TLSv1_2:
    حالة CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_روش();
      اكسر;
    حالة CURL_SSLVERSION_SSLv2:
      failf(داده, "No SSLv2 support");
      أرجع CURLE_NOT_BUILT_درون;
    حالة CURL_SSLVERSION_SSLv3:
      failf(داده, "No SSLv3 support");
      أرجع CURLE_NOT_BUILT_درون;
    افتراضي:
      failf(داده, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    اكسر;
  حالة TRNSPRT_QUIC:
    *pssl_نسخه_min = CURL_SSLVERSION_TLSv1_3;
    إذا(conn_config->نسخه_max &&
       (conn_config->نسخه_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->نسخه_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(داده, "QUIC needs at least TLS version 1.3");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_روش();
    اكسر;
  افتراضي:
    failf(داده, "unsupported transport %d in SSL init", peer->transport);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  أرجع *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(بنية ossl_ctx *octx,
                            بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده,
                            بنية ssl_peer *peer,
                            ثابت بنية alpn_spec *alpns_requested,
                            Curl_ossl_ctx_راهاندازی_cb *cb_راهاندازی,
                            فراغ *cb_user_داده,
                            Curl_ossl_new_session_cb *cb_new_session,
                            فراغ *ssl_user_داده,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *ciphers;
  ثابت SSL_روش *req_روش = عدم;
  ctx_option_t ctx_options = 0;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, داده);
  حرف * ثابت ssl_cert = conn_config->clientcert;
  ثابت بنية curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
  بیعلامت صحيح ssl_نسخه_min = CURL_SSLVERSION_DEFAULT;
  حرف error_میانگیر[256];

  /* Make funny stuff to get random input */
  نتیجه = ossl_دانه(داده);
  إذا(!نتیجه) {
    إذا(CURL_EASY_STR(داده, STRING_SSL_ENGINE))
      نتیجه = ossl_مجموعه_engine(داده, octx,
                               CURL_EASY_STR(داده, STRING_SSL_ENGINE));
    إذا(!نتیجه && داده->مجموعه.engine_default)
      نتیجه = ossl_مجموعه_engine_default(داده, octx);
  }

  إذا(!نتیجه) {
    ssl_config->certverifyresult = !X509_V_OK;
    نتیجه = ossl_init_روش(cf, داده, peer, &req_روش, &ssl_نسخه_min);
  }
  إذا(نتیجه)
    أرجع نتیجه;
  DEBUGASSERT(req_روش);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#اگر_تعریف_شده OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_روش):
#پایان_اگر
    SSL_CTX_new(req_روش);

  إذا(!octx->ssl_ctx) {
    failf(داده, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_میانگیر, حجم(error_میانگیر)));
    أرجع CURLE_بیرون_از_حافظه;
  }

  إذا(cb_راهاندازی) {
    نتیجه = cb_راهاندازی(cf, داده, cb_user_داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(داده->مجموعه.fdebug && داده->مجموعه.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_مجموعه_پیام_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_مجموعه_پیام_callback_arg(octx->ssl_ctx, cf);
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
  إذا(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_درج_خالی_FRAGMENTS;

  DEBUGASSERT(ssl_نسخه_min != CURL_SSLVERSION_DEFAULT);
  بدل(ssl_نسخه_min) {
  حالة CURL_SSLVERSION_SSLv2:
  حالة CURL_SSLVERSION_SSLv3:
    أرجع CURLE_NOT_BUILT_درون;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  حالة CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  حالة CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  حالة CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  حالة CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  حالة CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    نتیجه = ossl_مجموعه_ssl_نسخه_min_max(cf, octx->ssl_ctx, ssl_نسخه_min);
    إذا(نتیجه)
      أرجع نتیجه;
    اكسر;

  افتراضي:
    failf(داده, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  SSL_CTX_مجموعه_options(octx->ssl_ctx, ctx_options);
  SSL_CTX_مجموعه_read_ahead(octx->ssl_ctx, 1);

  /* Max TLS1.2 record size 0x4000 + 0x800.
     OpenSSL supports processing "jumbo TLS record" (8 TLS records) in one go
     for some algorithms, so match that here.
     Experimentation shows that a slightly larger buffer is needed
     to avoid short reads.

     However using a large buffer (8 packets) actually decreases performance.
     4 packets is better.
   */
#اگر_تعریف_شده HAVE_SSL_CTX_مجموعه_DEFAULT_READ_میانگیر_LEN
  SSL_CTX_مجموعه_default_read_میانگیر_len(octx->ssl_ctx, 0x401e * 4);
#پایان_اگر

  /* We do retry writes sometimes from another buffer address */
  SSL_CTX_مجموعه_نما(octx->ssl_ctx, SSL_نما_ACCEPT_MOVING_WRITE_میانگیر);

  ciphers = conn_config->cipher_list;
  إذا(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = عدم;
  إذا(ciphers && (ssl_نسخه_min < CURL_SSLVERSION_TLSv1_3)) {
    إذا(!SSL_CTX_مجموعه_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(داده, "failed setting cipher list: %s", ciphers);
      أرجع CURLE_SSL_CIPHER;
    }
    infof(داده, "Cipher selection: %s", ciphers);
  }

#اگر_تعریف_شده HAVE_SSL_CTX_مجموعه_CIPHERSUITES
  {
    ثابت حرف *ciphers13 = conn_config->cipher_list13;
    إذا(ciphers13 &&
       (!conn_config->نسخه_max ||
        (conn_config->نسخه_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->نسخه_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      إذا(!SSL_CTX_مجموعه_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(داده, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        أرجع CURLE_SSL_CIPHER;
      }
      infof(داده, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#پایان_اگر

  إذا(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    نتیجه = client_cert(داده, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->کلید, conn_config->کلید_blob,
                         conn_config->کلید_type,
                         conn_config->کلید_passwd);
    إذا(نتیجه)
      /* failf() is already done in client_cert() */
      أرجع نتیجه;
  }

#اگر_تعریف_شده HAVE_SSL_CTX_مجموعه_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_مجموعه_post_handshake_auth(octx->ssl_ctx, 1);
#پایان_اگر

  {
    ثابت حرف *curves = conn_config->curves;
    إذا(curves) {
#اگر_تعریف_شده HAVE_BORINGSSL_LIKE
#تعریف_کن OSSL_منحنی_CAST(x) (x)
#وإلا
#تعریف_کن OSSL_منحنی_CAST(x) (حرف *)CURL_UNCONST(x)
#پایان_اگر
      إذا(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_منحنی_CAST(curves))) {
        failf(داده, "failed setting curves list: '%s'", curves);
        أرجع CURLE_SSL_CIPHER;
      }
    }
  }

#اگر_تعریف_شده HAVE_SSL_CTX_SET1_SIGALGS
#تعریف_کن OSSL_SIGALG_CAST(x) OSSL_منحنی_CAST(x)
  {
    ثابت حرف *signature_algorithms = conn_config->signature_algorithms;
    إذا(signature_algorithms) {
      إذا(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(داده, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        أرجع CURLE_SSL_CIPHER;
      }
    }
  }
#پایان_اگر

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_مجموعه_verify(octx->ssl_ctx, SSL_VERIFY_NONE, عدم);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#إذا !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  إذا(Curl_tls_keylog_enabled()) {
    SSL_CTX_مجموعه_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#پایان_اگر

  إذا(cb_new_session) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_مجموعه_session_cache_نما(octx->ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_داخلی);
    SSL_CTX_sess_مجموعه_new_cb(octx->ssl_ctx, cb_new_session);
  }

  /* give application a chance to interfere with SSL set up. */
  إذا(داده->مجموعه.ssl_fsslctx) {
    بنية Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    إذا(!octx->x509_store_راهاندازی) {
      نتیجه = Curl_ssl_راهاندازی_x509_store(cf, داده, octx);
      إذا(نتیجه)
        أرجع نتیجه;
      octx->x509_store_راهاندازی = TRUE;
    }
    CURL_CBAPI_شروع(&guard, داده, easy_fsslctx);
    نتیجه = (*داده->مجموعه.ssl_fsslctx)(داده, octx->ssl_ctx,
                                      داده->مجموعه.ssl_fsslctxp);
    CURL_CBAPI_انتها(&guard);
    إذا(نتیجه) {
      failf(داده, "error signaled by SSL ctx callback");
      أرجع نتیجه;
    }
  }

  أرجع ossl_init_ssl(octx, cf, داده, peer, alpns_requested,
                       ssl_user_داده, sess_reuse_cb);
}

ساكن CURLcode ossl_on_session_reuse(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      بنية alpn_spec *alpns,
                                      بنية Curl_ssl_session *scs,
                                      bool *do_early_داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  أرجع Curl_on_session_reuse(cf, داده, alpns, scs, do_early_داده,
                               connssl->earlydata_max);
}

فراغ Curl_ossl_report_handshake(بنية Curl_easy *داده, بنية ossl_ctx *octx)
{
#اگر_تعریف_شده CURLVERBOSE
  إذا(Curl_trc_is_verbose(داده)) {
    صحيح psigtype_nid = NID_undef;
    ثابت حرف *negotiated_گروه_name = عدم;

#اگر_تعریف_شده HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#إذا OPENSSL_نسخه_عدد >= 0x30200000L
    negotiated_گروه_name = SSL_get0_گروه_name(octx->ssl);
#وإلا
    negotiated_گروه_name =
      OBJ_nid2sn(SSL_get_negotiated_گروه(octx->ssl) & 0x0000FFFF);
#پایان_اگر
#پایان_اگر

    /* Informational message */
    infof(داده, "SSL connection using %s / %s / %s / %s",
          SSL_get_نسخه(octx->ssl),
          SSL_get_cipher(octx->ssl),
          negotiated_گروه_name ? negotiated_گروه_name : "[blank]",
          OBJ_nid2sn(psigtype_nid));
  }
#وإلا
  (فراغ)داده;
  (فراغ)octx;
#پایان_اگر /* CURLVERBOSE */
}

ساكن CURLcode ossl_connect_step1(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode نتیجه;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  نتیجه = Curl_ossl_ctx_init(octx, cf, داده, &connssl->peer,
                              connssl->alpn, عدم, عدم,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  إذا(نتیجه)
    أرجع نتیجه;

  octx->bio_روش = ossl_bio_cf_روش_create();
  إذا(!octx->bio_روش)
    أرجع CURLE_بیرون_از_حافظه;
  bio = BIO_new(octx->bio_روش);
  إذا(!bio)
    أرجع CURLE_بیرون_از_حافظه;

  BIO_مجموعه_داده(bio, cf);
#اگر_تعریف_شده HAVE_SSL_SET0_WBIO
  /* with OpenSSL v1.1.1 we get an alternative to SSL_set_bio() that works
   * without backward compat quirks. Every call takes one reference, so we
   * up it and pass. SSL* then owns and frees it.
   * We check on the function in configure, since LibreSSL and friends
   * each have their own versions to add support for this. */
  BIO_به_بالا_ref(bio);
  SSL_set0_rbio(octx->ssl, bio);
  SSL_set0_wbio(octx->ssl, bio);
#وإلا
  SSL_مجموعه_bio(octx->ssl, bio, bio);
#پایان_اگر

  إذا(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    بنية alpn_proto_buf proto;
    memset(&proto, 0, حجم(proto));
    Curl_alpn_مقصد_proto_str(&proto, connssl->alpn);
    infof(داده, VTLS_INFOF_ALPN_OFFER_1STR, proto.داده);
  }

  connssl->connecting_state = ssl_connect_2;
  أرجع CURLE_OK;
}

#اگر_تعریف_شده HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
ساكن صحيح ossl_trace_ech_retry_configs(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *داده, SSL *ssl,
                                        صحيح reason)
{
  CURLcode نتیجه = CURLE_OK;
  size_t rcl = 0;
  صحيح rv = 1;
#اگر_تعریف_نشده HAVE_BORINGSSL_LIKE
  حرف *inner = عدم;
  uint8_t *rcs = عدم;
  حرف *outer = عدم;
#وإلا
  ثابت حرف *inner = عدم;
  ثابت uint8_t *rcs = عدم;
  ثابت حرف *outer = عدم;
  size_t بیرون_name_len = 0;
  صحيح servername_type = 0;
#پایان_اگر
  NOVERBOSE((فراغ)reason);

  /* nothing to trace if not doing ECH */
  إذا(!Curl_ssl_ech_enabled(cf))
    أرجع rv;
#اگر_تعریف_نشده HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#وإلا
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (صحيح)rcl;
#پایان_اگر

  إذا(rv && rcs) {
    حرف *b64str = عدم;
    size_t blen = 0;

    نتیجه = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    إذا(!نتیجه && b64str) {
      infof(داده, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#اگر_تعریف_نشده HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_وضعیت(ssl, &inner, &outer);
      infof(داده, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#وإلا
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &بیرون_name_len);
      infof(داده, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#پایان_اگر
    }
  }
  وإلا
    infof(داده, "ECH: no retry_configs (rv = %d)", rv);
#اگر_تعریف_نشده HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#پایان_اگر
  أرجع rv;
}

#پایان_اگر

ساكن CURLcode ossl_connect_step2(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, داده);
  صحيح err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  إذا(!octx->x509_store_راهاندازی) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode نتیجه = Curl_ssl_راهاندازی_x509_store(cf, داده, octx);
    إذا(نتیجه)
      أرجع نتیجه;
    octx->x509_store_راهاندازی = TRUE;
  }

#إذا !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  إذا(Curl_tls_keylog_enabled() && !octx->keylog_انجامشده)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_انجامشده);
#پایان_اگر

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  إذا(err != 1) {
    صحيح detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(داده, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    إذا(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(داده, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      أرجع CURLE_AGAIN;
    }
    إذا(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(داده, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      أرجع CURLE_AGAIN;
    }
#اگر_تعریف_شده SSL_ERROR_WANT_ASYNC
    إذا(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(داده, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      أرجع CURLE_AGAIN;
    }
#پایان_اگر
#اگر_تعریف_شده SSL_ERROR_WANT_RETRY_VERIFY
    إذا(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(داده, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(داده, TRUE);
      أرجع CURLE_AGAIN;
    }
#پایان_اگر
    وإلا {
      /* untreated error */
      sslerr_t errdetail;
      حرف error_میانگیر[256] = "";
      CURLcode نتیجه;
      دراز lerr;
      صحيح lib;
      صحيح reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      إذا((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#اگر_تعریف_شده SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#پایان_اگر
         )) {
        نتیجه = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_نتیجه(octx->ssl);
        إذا(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(داده, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        وإلا
          failf(داده, "%s", "SSL certificate verification failed");
      }
#اگر_تعریف_شده SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      وإلا إذا((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        نتیجه = CURLE_SSL_CLIENTCERT;
        failf(داده, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_میانگیر, حجم(error_میانگیر)));
      }
#پایان_اگر
#اگر_تعریف_شده HAVE_SSL_SET1_ECH_CONFIG_LIST
      وإلا إذا((lib == ERR_LIB_SSL) &&
#اگر_تعریف_نشده HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#وإلا
              (reason == SSL_R_ECH_REJECTED)) {
#پایان_اگر /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, داده, octx->ssl, reason);

        نتیجه = CURLE_ECH_REQUIRED;
        failf(داده, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_میانگیر, حجم(error_میانگیر)));
      }
#پایان_اگر
      وإلا {
        نتیجه = CURLE_SSL_CONNECT_ERROR;
        failf(داده, "TLS connect error: %s",
              ossl_strerror(errdetail, error_میانگیر, حجم(error_میانگیر)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      إذا(نتیجه == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        حرف extramsg[80] = "";
        صحيح sockerr = SOCKERRNO;

        إذا(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, حجم(extramsg));
        failf(داده, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_مقصد_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->پورت);
      }

      أرجع نتیجه;
    }
  }
  وإلا {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(داده, octx);

#إذا defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    إذا(Curl_ssl_ech_enabled(cf)) {
      بنية ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      حرف *inner = عدم, *outer = عدم;
      صحيح rv;
      VERBOSE(ثابت حرف *وضعیت);

      rv = SSL_ech_get1_وضعیت(octx->ssl, &inner, &outer);
      بدل(rv) {
      حالة SSL_ECH_وضعیت_SUCCESS:
        VERBOSE(وضعیت = "succeeded");
        اكسر;
      حالة SSL_ECH_وضعیت_GREASE_ECH:
        VERBOSE(وضعیت = "sent GREASE, got retry-configs");
        اكسر;
      حالة SSL_ECH_وضعیت_GREASE:
        VERBOSE(وضعیت = "sent GREASE");
        اكسر;
      حالة SSL_ECH_وضعیت_NOT_TRIED:
        VERBOSE(وضعیت = "not attempted");
        اكسر;
      حالة SSL_ECH_وضعیت_NOT_CONFIGURED:
        VERBOSE(وضعیت = "not configured");
        اكسر;
      حالة SSL_ECH_وضعیت_BACKEND:
        VERBOSE(وضعیت = "backend (unexpected)");
        اكسر;
      حالة SSL_ECH_وضعیت_FAILED:
        VERBOSE(وضعیت = "failed");
        اكسر;
      حالة SSL_ECH_وضعیت_BAD_CALL:
        VERBOSE(وضعیت = "bad call (unexpected)");
        اكسر;
      حالة SSL_ECH_وضعیت_BAD_NAME: {
        إذا(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(وضعیت = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_وضعیت_SUCCESS;
        }
        وإلا {
          VERBOSE(وضعیت = "bad name (unexpected)");
        }
        اكسر;
      }
      افتراضي:
        VERBOSE(وضعیت = "unexpected status");
        infof(داده, "ECH: unexpected status %d", rv);
      }
      infof(داده, "ECH: result '%s' (inner=%s, outer=%s)",
            (وضعیت ? وضعیت : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      إذا(rv == SSL_ECH_وضعیت_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, داده, octx->ssl, 0);
      }
      إذا(rv != SSL_ECH_وضعیت_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(داده, "ECH: ech-hard failed");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
    وإلا {
      infof(داده, "ECH: result: status is not attempted");
    }
#پایان_اگر /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    إذا(connssl->alpn) {
      ثابت بیعلامت حرف *neg_protocol;
      بیعلامت صحيح len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      أرجع Curl_alpn_مجموعه_negotiated(cf, داده, connssl, neg_protocol, len);
    }

    أرجع CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
ساكن CURLcode ossl_pkp_pin_peer_pubkey(بنية Curl_easy *داده, X509 *cert,
                                         ثابت حرف *pinnedpubkey)
{
  /* Scratch */
  صحيح len1 = 0, len2 = 0;
  بیعلامت حرف *buff1 = عدم, *موقت = عدم;

  /* Result is returned to caller */
  CURLcode نتیجه = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  إذا(!pinnedpubkey)
    أرجع CURLE_OK;

  إذا(!cert)
    أرجع نتیجه;

  افعل {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), عدم);
    إذا(len1 < 1)
      اكسر; /* failed */

    buff1 = موقت = curlx_malloc(len1);
    إذا(!buff1)
      اكسر; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &موقت);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    إذا((len1 != len2) || !موقت || ((موقت - buff1) != len1))
      اكسر; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    نتیجه = Curl_pin_peer_pubkey(داده, pinnedpubkey, buff1, len1);
  } طالما(0);

  إذا(buff1)
    curlx_free(buff1);

  أرجع نتیجه;
}

#اگر_تعریف_شده CURLVERBOSE
#إذا !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_نسخه_عدد) && LIBRESSL_نسخه_عدد < 0x3060000fL)
ساكن فراغ infof_certstack(بنية Curl_easy *داده, ثابت SSL *ssl)
{
  پشته_از(X509) *certstack;
  دراز verify_نتیجه;
  صحيح رقم_cert_سطوح;
  صحيح cert_level;

  إذا(!Curl_trc_is_verbose(داده))
    أرجع;

  verify_نتیجه = SSL_get_verify_نتیجه(ssl);
  إذا(verify_نتیجه != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  وإلا
    certstack = SSL_get0_verified_chain(ssl);
  إذا(!certstack)
    أرجع;
  رقم_cert_سطوح = sk_X509_رقم(certstack);

  لكل(cert_level = 0; cert_level < رقم_cert_سطوح; cert_level++) {
    حرف cert_algorithm[80] = "";
    حرف گروه_name_final[80] = "";
    ثابت X509_ALGOR *palg_cert = عدم;
    ثابت ASN1_OBJECT *paobj_cert = عدم;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    صحيح کلید_بیتها;
    صحيح کلید_sec_بیتها;
    صحيح get_گروه_name;
    ثابت حرف *type_name;

    current_cert = sk_X509_مقدار(certstack, cert_level);
    إذا(!current_cert)
      استمر;

    current_pkey = X509_get0_pubkey(current_cert);
    إذا(!current_pkey)
      استمر;

    X509_get0_signature(عدم, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, عدم, عدم, palg_cert);
    OBJ_obj2txt(cert_algorithm, حجم(cert_algorithm), paobj_cert, 0);

    کلید_بیتها = EVP_PKEY_بیتها(current_pkey);
#اگر_تعریف_نشده HAVE_OPENSSL3
#تعریف_کن EVP_PKEY_get_security_بیتها EVP_PKEY_security_بیتها
#پایان_اگر
    کلید_sec_بیتها = EVP_PKEY_get_security_بیتها(current_pkey);
#اگر_تعریف_شده HAVE_OPENSSL3
    {
      حرف گروه_name[80] = "";
      get_گروه_name = EVP_PKEY_get_گروه_name(current_pkey, گروه_name,
                                               حجم(گروه_name), عدم);
      curl_msnprintf(گروه_name_final, حجم(گروه_name_final), "/%s",
                     گروه_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#وإلا
    get_گروه_name = 0;
    type_name = عدم;
#پایان_اگر

    infof(داده, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_گروه_name == 0 ? "" : گروه_name_final,
          کلید_بیتها, کلید_sec_بیتها, cert_algorithm);
  }
}
#وإلا
#تعریف_کن infof_certstack(داده, ssl)
#پایان_اگر
#پایان_اگر /* CURLVERBOSE */

ساكن CURLcode ossl_check_issuer(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  X509 *server_cert)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = عدم;
  BIO *fp = عدم;
  حرف err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode نتیجه = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  إذا(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->داده,
                         (صحيح)conn_config->issuercert_blob->len);
    إذا(!fp) {
      failf(داده, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, حجم(err_buf)));
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }
  }
  وإلا إذا(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    إذا(!fp) {
      failf(داده, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, حجم(err_buf)));
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب بیرون;
    }

    إذا(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      إذا(verify_enabled)
        failf(داده, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      نتیجه = CURLE_SSL_ISSUER_ERROR;
      اذهب بیرون;
    }
  }

  إذا(fp) {
    issuer = PEM_read_bio_X509(fp, عدم, ZERO_NULL, عدم);
    إذا(!issuer) {
      إذا(verify_enabled)
        failf(داده, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      نتیجه = CURLE_SSL_ISSUER_ERROR;
      اذهب بیرون;
    }

    إذا(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      إذا(verify_enabled)
        failf(داده, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      نتیجه = CURLE_SSL_ISSUER_ERROR;
      اذهب بیرون;
    }

    infof(داده, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

بیرون:
  إذا(fp)
    BIO_free(fp);
  إذا(issuer)
    X509_free(issuer);
  أرجع نتیجه;
}

ساكن ثابت حرف *pinned(بنية Curl_cfilter *cf,
                          بنية Curl_easy *داده)
{
  (فراغ)cf;
  أرجع
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#پایان_اگر
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY);
}

ساكن CURLcode ossl_check_pinned_کلید(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      X509 *server_cert)
{
  CURLcode نتیجه = CURLE_OK;
  ثابت حرف *ptr = pinned(cf, داده);
  إذا(ptr) {
    نتیجه = ossl_pkp_pin_peer_pubkey(داده, server_cert, ptr);
    إذا(نتیجه)
      failf(داده, "SSL: public key does not match pinned public key");
  }
  أرجع نتیجه;
}

#اگر_تعریف_شده CURLVERBOSE
#تعریف_کن MAX_CERT_NAME_درازی 2048
ساكن CURLcode ossl_infof_cert(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                X509 *server_cert)
{
  BIO *mem = عدم;
  بنية dynbuf dname;
  حرف err_buf[256] = "";
  حرف *buf;
  دراز len;
  CURLcode نتیجه = CURLE_OK;

  إذا(!Curl_trc_is_verbose(داده))
    أرجع CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_درازی);
  mem = BIO_new(BIO_s_mem());
  إذا(!mem) {
    failf(داده, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, حجم(err_buf)));
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  infof(داده, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  نتیجه = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(داده, "  subject: %s", نتیجه ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_زمان_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_داده(mem, (حرف **)&buf);
  infof(داده, "  start date: %.*s", (صحيح)len, buf);
  (فراغ)BIO_reset(mem);

  ASN1_زمان_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_داده(mem, (حرف **)&buf);
  infof(داده, "  expire date: %.*s", (صحيح)len, buf);
  (فراغ)BIO_reset(mem);

  نتیجه = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  إذا(نتیجه) /* should be only fatal stuff like OOM */
    اذهب بیرون;
  infof(داده, "  issuer: %s", curlx_dyn_ptr(&dname));

بیرون:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  أرجع نتیجه;
}
#پایان_اگر /* CURLVERBOSE */

#اگر_تعریف_شده USE_APPLE_SECTRUST
بنية ossl_certs_ctx {
  پشته_از(X509) *sk;
  size_t رقم_certs;
  بیعلامت حرف *آخرین_der;
};

ساكن CURLcode ossl_chain_get_der(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   فراغ *user_داده,
                                   size_t i,
                                   بیعلامت حرف **pder,
                                   size_t *pder_len)
{
  بنية ossl_certs_ctx *chain = user_داده;
  X509 *cert;
  صحيح der_len;

  OPENSSL_free(chain->آخرین_der);
  chain->آخرین_der = عدم;

  (فراغ)cf;
  (فراغ)داده;
  *pder_len = 0;
  *pder = عدم;

  إذا(i >= chain->رقم_certs)
    أرجع CURLE_TOO_LARGE;
  cert = sk_X509_مقدار(chain->sk, (صحيح)i);
  إذا(!cert)
    أرجع CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  إذا(der_len < 0)
    أرجع CURLE_FAILED_INIT;
  chain->آخرین_der = *pder;
  *pder_len = (size_t)der_len;
  أرجع CURLE_OK;
}

ساكن CURLcode ossl_apple_verify(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  بنية ossl_ctx *octx,
                                  بنية ssl_peer *peer)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية ossl_certs_ctx chain;
  CURLcode نتیجه;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, حجم(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.رقم_certs = chain.sk ? sk_X509_رقم(chain.sk) : 0;

  إذا(!chain.رقم_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    إذا(!octx->reused_session) {
      failf(داده, "SSL: could not get peer certificate chain");
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    }
    وإلا {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(داده, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      أرجع CURLE_OK;
    }
  }
  وإلا {
#اگر_تعریف_شده HAVE_BORINGSSL_LIKE
    ثابت uint8_t *ocsp_داده = عدم;
#وإلا
    بیعلامت حرف *ocsp_داده = عدم;
#پایان_اگر
    دراز ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    إذا(!octx->reused_session)
      ocsp_len = (دراز)SSL_get_tlsext_وضعیت_ocsp_resp(octx->ssl, &ocsp_داده);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    إذا(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_داده = عدم;
    }

    نتیجه = Curl_vtls_apple_verify(cf, داده, peer, chain.رقم_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_داده, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.آخرین_der);
    chain.آخرین_der = عدم;
  }
  octx->sectrust_verified = !نتیجه;
  أرجع نتیجه;
}
#پایان_اگر /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   بنية ossl_ctx *octx,
                                   بنية ssl_peer *peer)
{
  بنية connectdata *conn = cf->conn;
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, داده);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجه = CURLE_OK;
  دراز ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  إذا(داده->مجموعه.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    نتیجه = ossl_certchain(داده, octx->ssl);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  إذا(!server_cert) {
    /* no verification at all, this maybe acceptable */
    إذا(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, داده))
      اذهب بیرون;

    failf(داده, "SSL: could not get peer certificate");
    نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    اذهب بیرون;
  }

#اگر_تعریف_شده CURLVERBOSE
  نتیجه = ossl_infof_cert(cf, داده, server_cert);
  إذا(نتیجه)
    اذهب بیرون;
  infof_certstack(داده, octx->ssl);
#پایان_اگر

  إذا(conn_config->verifyhost) {
    نتیجه = ossl_verifyhost(داده, conn, peer, server_cert);
    إذا(نتیجه)
      اذهب بیرون;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_نتیجه(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(داده, "OpenSSL verify result: %lx", (بیعلامت دراز)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  إذا(verified)
    infof(داده, "SSL certificate verified via OpenSSL.");

#اگر_تعریف_شده USE_APPLE_SECTRUST
  إذا(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    نتیجه = ossl_apple_verify(cf, داده, octx, peer);
    إذا(نتیجه && (نتیجه != CURLE_PEER_FAILED_VERIFICATION))
      اذهب بیرون; /* unexpected error */
    إذا(octx->sectrust_verified) {
      infof(داده, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#پایان_اگر

  إذا(!verified) {
    /* no trust established, report the OpenSSL status */
    إذا(conn_config->verifypeer) {
      failf(داده, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
      اذهب بیرون;
    }
    infof(داده, " SSL certificate verification failed, continuing anyway!");
  }

#اگر_تعریف_نشده OPENSSL_NO_OCSP
  إذا(conn_config->verifystatus &&
#اگر_تعریف_شده USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, داده)) &&
#پایان_اگر
     !octx->reused_session) {
    نتیجه = verifystatus(cf, داده, octx);
    إذا(نتیجه)
      اذهب بیرون;
  }
#پایان_اگر

  نتیجه = ossl_check_issuer(cf, داده, server_cert);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = ossl_check_pinned_کلید(cf, داده, server_cert);

بیرون:
  X509_free(server_cert);
  أرجع نتیجه;
}

ساكن CURLcode ossl_connect_step3(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  نتیجه = Curl_ossl_check_peer_cert(cf, داده, octx, &connssl->peer);
  إذا(نتیجه)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, داده, connssl->peer.scache_کلید);

  أرجع نتیجه;
}

#اگر_تعریف_شده HAVE_OPENSSL_EARLYDATA
ساكن CURLcode ossl_send_earlydata(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;
  ثابت بیعلامت حرف *buf;
  size_t blen, nwritten;
  صحيح rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_نتیجه = CURLE_OK;
  طالما(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_داده(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(داده, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    إذا(rc <= 0) {
      دراز sslerror;
      حرف error_میانگیر[256];
      صحيح err = SSL_get_error(octx->ssl, rc);

      بدل(err) {
      حالة SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        نتیجه = CURLE_AGAIN;
        اذهب بیرون;
      حالة SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        نتیجه = CURLE_AGAIN;
        اذهب بیرون;
      حالة SSL_ERROR_SYSCALL: {
        صحيح sockerr = SOCKERRNO;

        إذا(octx->io_نتیجه == CURLE_AGAIN) {
          نتیجه = CURLE_AGAIN;
          اذهب بیرون;
        }
        sslerror = ERR_get_error();
        إذا(sslerror)
          ossl_strerror(sslerror, error_میانگیر, حجم(error_میانگیر));
        وإلا إذا(sockerr)
          curlx_strerror(sockerr, error_میانگیر, حجم(error_میانگیر));
        وإلا
          curl_msnprintf(error_میانگیر, حجم(error_میانگیر), "%s",
                         SSL_ERROR_مقصد_str(err));

        failf(داده, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_میانگیر, sockerr);
        نتیجه = CURLE_SEND_ERROR;
        اذهب بیرون;
      }
      حالة SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(داده, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_میانگیر, حجم(error_میانگیر)));
        نتیجه = CURLE_SEND_ERROR;
        اذهب بیرون;
      }
      افتراضي:
        /* a true error */
        failf(داده, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_مقصد_str(err), SOCKERRNO);
        نتیجه = CURLE_SEND_ERROR;
        اذهب بیرون;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(داده, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
بیرون:
  أرجع نتیجه;
}
#پایان_اگر /* HAVE_OPENSSL_EARLYDATA */

ساكن CURLcode ossl_connect(بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده,
                             bool *انجامشده)
{
  CURLcode نتیجه = CURLE_OK;
  بنية ssl_connect_داده *connssl = cf->ctx;

  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  إذا(connssl->connecting_state == ssl_connect_1) {
    إذا(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(داده, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(داده, cf, "need HTTPS-RR, delaying connect");
      أرجع CURLE_OK;
    }
    CURL_TRC_CF(داده, cf, "ossl_connect, step1");
    نتیجه = ossl_connect_step1(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(داده, cf, "ossl_connect, step2");
#اگر_تعریف_شده HAVE_OPENSSL_EARLYDATA
    إذا(connssl->earlydata_state == ssl_earlydata_await) {
      اذهب بیرون;
    }
    وإلا إذا(connssl->earlydata_state == ssl_earlydata_sending) {
      نتیجه = ossl_send_earlydata(cf, داده);
      إذا(نتیجه)
        اذهب بیرون;
      connssl->earlydata_state = ssl_earlydata_فرستاده;
    }
#پایان_اگر
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_فرستاده));

    نتیجه = ossl_connect_step2(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
  }

  إذا(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(داده, cf, "ossl_connect, step3");
    نتیجه = ossl_connect_step3(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
    connssl->connecting_state = ssl_connect_انجامشده;
#اگر_تعریف_شده HAVE_OPENSSL_EARLYDATA
    إذا(connssl->earlydata_state > ssl_earlydata_none) {
      بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_فرستاده);
      connssl->earlydata_state =
        (SSL_get_early_داده_وضعیت(octx->ssl) == SSL_EARLY_داده_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#پایان_اگر
  }

  إذا(connssl->connecting_state == ssl_connect_انجامشده) {
    CURL_TRC_CF(داده, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

بیرون:
  إذا(نتیجه == CURLE_AGAIN) {
    *انجامشده = FALSE;
    أرجع CURLE_OK;
  }
  *انجامشده = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  أرجع نتیجه;
}

ساكن bool ossl_داده_pending(بنية Curl_cfilter *cf,
                              ثابت بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  (فراغ)داده;
  أرجع (bool)connssl->درون_داد_pending;
}

ساكن CURLcode ossl_send(بنية Curl_cfilter *cf,
                          بنية Curl_easy *داده,
                          ثابت فراغ *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  صحيح err;
  حرف error_میانگیر[256];
  sslerr_t sslerror;
  صحيح memlen;
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;
  صحيح nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)بیشترین_صحیح) ? بیشترین_صحیح : (صحيح)len;
  إذا(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    إذا(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  إذا(nwritten > 0) {
    إذا(nwritten > memlen) {
      failf(داده, "SSL_write(len=%d) returned %d", memlen, nwritten);
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    *pnwritten = (size_t)nwritten;
  }
  وإلا {
    err = SSL_get_error(octx->ssl, nwritten);

    بدل(err) {
    حالة SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      نتیجه = CURLE_AGAIN;
      اذهب بیرون;
    حالة SSL_ERROR_WANT_WRITE:
      نتیجه = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      اذهب بیرون;
    حالة SSL_ERROR_SYSCALL: {
      صحيح sockerr = SOCKERRNO;

      إذا(octx->io_نتیجه == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        نتیجه = CURLE_AGAIN;
        اذهب بیرون;
      }
      sslerror = ERR_get_error();
      إذا(sslerror)
        ossl_strerror(sslerror, error_میانگیر, حجم(error_میانگیر));
      وإلا إذا(sockerr)
        curlx_strerror(sockerr, error_میانگیر, حجم(error_میانگیر));
      وإلا
        curl_msnprintf(error_میانگیر, حجم(error_میانگیر), "%s",
                       SSL_ERROR_مقصد_str(err));

      failf(داده, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_میانگیر, sockerr);
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    حالة SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(داده, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_میانگیر, حجم(error_میانگیر)));
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    افتراضي:
      /* a true error */
      failf(داده, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_مقصد_str(err), SOCKERRNO);
      نتیجه = CURLE_SEND_ERROR;
      اذهب بیرون;
    }
  }

بیرون:
  أرجع نتیجه;
}

ساكن CURLcode ossl_recv(بنية Curl_cfilter *cf,
                          بنية Curl_easy *داده,   /* transfer */
                          حرف *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  حرف error_میانگیر[256];
  بیعلامت دراز sslerror;
  صحيح buffsize;
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;
  صحيح nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)بیشترین_صحیح) ? بیشترین_صحیح : (صحيح)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  إذا(nread > 0)
    *pnread = (size_t)nread;
  وإلا {
    /* failed SSL_read */
    صحيح err = SSL_get_error(octx->ssl, nread);

    بدل(err) {
    حالة SSL_ERROR_NONE: /* this is not an error */
      اكسر;
    حالة SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      إذا(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(داده, cf, "TLS close_notify");
      اكسر;
    حالة SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      نتیجه = CURLE_AGAIN;
      اذهب بیرون;
    حالة SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتیجه = CURLE_AGAIN;
      اذهب بیرون;
    افتراضي:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      إذا(octx->io_نتیجه == CURLE_AGAIN) {
        نتیجه = CURLE_AGAIN;
        اذهب بیرون;
      }
      sslerror = ERR_get_error();
      إذا((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
صف*/
        صحيح sockerr = SOCKERRNO;
        إذا(sslerror)
          ossl_strerror(sslerror, error_میانگیر, حجم(error_میانگیر));
        وإلا إذا(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_میانگیر, حجم(error_میانگیر));
        وإلا
          curl_msnprintf(error_میانگیر, حجم(error_میانگیر), "%s",
                         SSL_ERROR_مقصد_str(err));
        failf(داده, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_میانگیر, sockerr);
        نتیجه = CURLE_RECV_ERROR;
        اذهب بیرون;
      }
      وإلا إذا(err == SSL_ERROR_SYSCALL) {
        إذا(octx->io_نتیجه) {
          /* logging handling in underlying filter already */
          نتیجه = octx->io_نتیجه;
        }
        وإلا إذا(connssl->peer_closed) {
          failf(داده, "Connection closed abruptly");
          نتیجه = CURLE_RECV_ERROR;
        }
        وإلا {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          صحيح sockerr = SOCKERRNO;
          إذا(sockerr)
            curlx_strerror(sockerr, error_میانگیر, حجم(error_میانگیر));
          وإلا {
            curl_msnprintf(error_میانگیر, حجم(error_میانگیر),
                           "Connection closed abruptly");
          }
          failf(داده, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_میانگیر, sockerr);
          نتیجه = CURLE_RECV_ERROR;
        }
        اذهب بیرون;
      }
    }
  }

بیرون:
  إذا((!نتیجه && !*pnread) || (نتیجه == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->درون_داد_pending = FALSE;
  }
  CURL_TRC_CF(داده, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (صحيح)نتیجه, *pnread, connssl->درون_داد_pending);
  أرجع نتیجه;
}

ساكن CURLcode ossl_get_channel_binding(بنية Curl_easy *داده,
                                         int8_t sockindex,
                                         بنية dynbuf *binding)
{
  X509 *cert;
  صحيح mdnid;
  bool no_digest_acceptable = FALSE;
  ثابت EVP_MD *algo_type = عدم;
  ثابت حرف *algo_name = عدم;
  بیعلامت صحيح درازی;
  بیعلامت حرف buf[EVP_MAX_MD_اندازه];

  ساكن ثابت حرف پیشوند[] = "tls-server-end-point:";
  بنية connectdata *conn = داده->conn;
  بنية Curl_cfilter *cf = conn->cfilter[sockindex];
  بنية ossl_ctx *octx = عدم;
  CURLcode نتیجه = CURLE_OK;

  افعل {
    ثابت بنية Curl_cftype *cft = cf->cft;
    بنية ssl_connect_داده *connssl = cf->ctx;

    إذا(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (بنية ossl_ctx *)connssl->backend;
      اكسر;
    }

    cf = cf->بعدی;
  } طالما(cf);

  إذا(!octx) {
    failf(داده, "Failed to find the SSL filter");
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  إذا(!cert)
    /* No server certificate, do not do channel binding */
    أرجع CURLE_OK;

#اگر_تعریف_شده HAVE_OPENSSL3
  {
    صحيح pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    إذا(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(داده, "certificate signature algorithm not recognized");
      نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
      اذهب بیرون;
    }

    إذا(mdnid != NID_undef) {
      إذا(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      وإلا
        algo_type = EVP_get_digestbynid(mdnid);
    }
    وإلا إذا(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    وإلا إذا(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      حرف mdname[128] = "";
      صحيح rc = EVP_PKEY_get_default_digest_name(pkey, mdname, حجم(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      إذا(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      وإلا إذا(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(داده, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#وإلا /* HAVE_OPENSSL3 */

  إذا(!OBJ_پیدا_کن_sigid_algs(X509_get_signature_nid(cert), &mdnid, عدم)) {
    failf(داده,
          "Unable to find digest NID for certificate signature algorithm");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب بیرون;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  إذا(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  وإلا {
    algo_type = EVP_get_digestbynid(mdnid);
    إذا(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(داده, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
      اذهب بیرون;
    }
  }

#پایان_اگر /* HAVE_OPENSSL3, else */

  إذا(!algo_type) {
    إذا(no_digest_acceptable) {
      infof(داده, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      نتیجه = CURLE_OK;
      اذهب بیرون;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(داده, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب بیرون;
  }

  إذا(!X509_digest(cert, algo_type, buf, &درازی)) {
    failf(داده, "X509_digest() failed for channel binding");
    نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    اذهب بیرون;
  }

  /* Append "tls-server-end-point:" */
  نتیجه = curlx_dyn_addn(binding, پیشوند, CURL_CSTRLEN(پیشوند));
  إذا(نتیجه)
    اذهب بیرون;

  /* Append digest */
  نتیجه = curlx_dyn_addn(binding, buf, درازی);

بیرون:
  X509_free(cert);
  أرجع نتیجه;
}

size_t Curl_ossl_نسخه(حرف *میانگیر, size_t اندازه)
{
#اگر_تعریف_شده LIBRESSL_نسخه_عدد
  حرف *p;
  size_t شمار;
  ثابت حرف *ver = Open_SSL_نسخه(OPENSSL_نسخه);
  ساكن ثابت حرف expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  إذا(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  شمار = curl_msnprintf(میانگیر, اندازه, "%s/%s", OSSL_PACKAGE, ver);
  لكل(p = میانگیر; *p; ++p) {
    إذا(ISBLANK(*p))
      *p = '_';
  }
  أرجع شمار;
#وإلاإذا defined(OPENSSL_IS_AWSLC)
  أرجع curl_msnprintf(میانگیر, اندازه, "%s/%s",
                        OSSL_PACKAGE, AWSLC_نسخه_عدد_STRING);
#وإلاإذا defined(OPENSSL_IS_BORINGSSL)
#اگر_تعریف_شده CURL_BORINGSSL_نسخه
  أرجع curl_msnprintf(میانگیر, اندازه, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_نسخه);
#وإلا
  أرجع curl_msnprintf(میانگیر, اندازه, OSSL_PACKAGE);
#پایان_اگر
#وإلا /* OpenSSL 3+ */
  أرجع curl_msnprintf(میانگیر, اندازه, "%s/%s",
                        OSSL_PACKAGE, Open_SSL_نسخه(OPENSSL_نسخه_STRING));
#پایان_اگر
}

/* can be called with data == NULL */
ساكن CURLcode ossl_random(بنية Curl_easy *داده,
                            بیعلامت حرف *entropy, size_t درازی)
{
  صحيح rc;
  إذا(داده) {
    إذا(ossl_دانه(داده)) /* Initiate the seed if not already done */
      أرجع CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  وإلا {
    إذا(!rand_enough())
      أرجع CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_بایتها(entropy, (ossl_valsize_t)curlx_uztosi(درازی));
  أرجع rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

ساكن CURLcode ossl_sha256sum(ثابت بیعلامت حرف *درون_داد,
                               size_t len,
                               بیعلامت حرف *sha256sum /* output */,
                               size_t استفادهنشده)
{
  CURLcode نتیجه = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (فراغ)استفادهنشده;

  mdctx = EVP_MD_CTX_new();
  إذا(!mdctx)
    أرجع CURLE_بیرون_از_حافظه;
  إذا(!EVP_DigestInit_ex(mdctx, EVP_sha256(), عدم)) {
    نتیجه = CURLE_FAILED_INIT;
    اذهب بیرون;
  }
  إذا(!EVP_DigestUpdate(mdctx, درون_داد, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, عدم))
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
بیرون:
  EVP_MD_CTX_free(mdctx);
  أرجع نتیجه;
}

ساكن bool ossl_cert_وضعیت_request(فراغ)
{
#اگر_تعریف_نشده OPENSSL_NO_OCSP
  أرجع TRUE;
#وإلا
  أرجع FALSE;
#پایان_اگر
}

ساكن فراغ *ossl_get_internals(بنية ssl_connect_داده *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  بنية ossl_ctx *octx = (بنية ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  أرجع info == CURLINFO_TLS_SESSION ?
    (فراغ *)octx->ssl_ctx : (فراغ *)octx->ssl;
}

ثابت بنية Curl_ssl Curl_ssl_openssl = {
  { CURLSSLBACKEND_OPENSSL, "openssl" }, /* info */

  SSLSUPP_CA_مسیر |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_SSL_CTX |
#اگر_تعریف_شده HAVE_SSL_CTX_مجموعه_CIPHERSUITES
  SSLSUPP_TLS13_CIPHERSUITES |
#پایان_اگر
#اگر_تعریف_شده HAVE_SSL_CTX_SET1_SIGALGS
  SSLSUPP_SIGNATURE_ALGORITHMS |
#پایان_اگر
#اگر_تعریف_شده HAVE_SSL_SET1_ECH_CONFIG_LIST
  SSLSUPP_ECH |
#پایان_اگر
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_ISSUERCERT_BLOB |
  SSLSUPP_SSL_EC_CURVES |
  SSLSUPP_CRLFILE,

  حجم(بنية ossl_ctx),

  ossl_init,                /* init */
  ossl_cleanup,             /* cleanup */
  Curl_ossl_نسخه,        /*نسخه*/
  ossl_shutdown,            /* shutdown */
  ossl_داده_pending,        /* data_pending */
  ossl_random,              /* random */
  ossl_cert_وضعیت_request, /* cert_status_request */
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

#پایان_اگر /* USE_OPENSSL */
