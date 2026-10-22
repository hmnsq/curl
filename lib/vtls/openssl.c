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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_OPENSSL

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "formdata.h" /* for the boundary function */
#شامل "url.h" /* for the SSL config check function */
#شامل "curlx/inet_pton.h"
#شامل "vtls/openssl.h"
#شامل "connect.h"
#شامل "progress.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_int.h"
#شامل "vtls/vtls_scache.h"
#شامل "vauth/vauth.h"
#شامل "vtls/keylog.h"
#شامل "vtls/hostcheck.h"
#شامل "transfer.h"
#شامل "multiif.h"
#شامل "curlx/strerr.h"
#شامل "curlx/strparse.h"
#شامل "curlx/strcopy.h"
#شامل "curlx/strdup.h"
#شامل "vdns/cf-dns.h"
#شامل "vdns/httpsrr.h"
#شامل "vtls/apple.h"
#اگر_تعریف_شدہ USE_ECH
#شامل "curlx/base64.h"
#اگر_ختم

#شامل <openssl/rand.h>
#شامل <openssl/x509v3.h>
#اگر_غیر_تعریف_شدہ OPENSSL_NO_DSA
#شامل <openssl/dsa.h>
#اگر_ختم
#شامل <openssl/dh.h>
#شامل <openssl/err.h>
#شامل <openssl/conf.h>
#شامل <openssl/bn.h>
#شامل <openssl/rsa.h>
#شامل <openssl/bio.h>
#شامل <openssl/pkcs12.h>
#شامل <openssl/tls1.h>
#شامل <openssl/evp.h>

#اگر defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#شامل <openssl/ech.h>
#اگر_ختم

#اگر_غیر_تعریف_شدہ OPENSSL_NO_OCSP
#شامل <openssl/ocsp.h>
#اگر_ختم

#اگر !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#تعریف_کرو USE_OPENSSL_ENGINE
#شامل <openssl/engine.h>
#اگر_ختم

#اگر_تعریف_شدہ LIBRESSL_نسخہ_نمبر
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  اگر LIBRESSL_نسخہ_نمبر < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#وگرنہ_اگر !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#اگر_ختم

#اگر defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#شامل <openssl/provider.h>
#شامل <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#تعریف_کرو OPENSSL_HAS_PROVIDERS
#اگر_ختم

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#اگر !defined(LIBRESSL_نسخہ_نمبر) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_کلید_EXPLICIT_PARAMS))
#تعریف_کرو HAVE_SSL_CTX_سیٹ_DEFAULT_READ_بفر_LEN 1
#اگر_ختم

#اگر defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#شامل <openssl/ui.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_OPENSSL3
#تعریف_کرو HAVE_EVP_PKEY_GET_PARAMS 1
#اگر_ختم

#اگر_تعریف_شدہ HAVE_EVP_PKEY_GET_PARAMS
#شامل <openssl/core_names.h>
#تعریف_کرو DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = تہی
#تعریف_کرو FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#ورنہ
#تعریف_کرو DECLARE_PKEY_PARAM_BIGNUM(name) ثابت BIGNUM *name
#تعریف_کرو FREE_PKEY_PARAM_BIGNUM(name)
#اگر_ختم

/* Whether SSL_CTX_set_ciphersuites is available.
 * BoringSSL: no
 * LibreSSL: supported since 3.4.1 (released 2021-10-14)
 * OpenSSL: supported since 1.1.1 (commit a53b5be6a05)
 */
#اگر_غیر_تعریف_شدہ OPENSSL_IS_BORINGSSL
#  define HAVE_SSL_CTX_سیٹ_CIPHERSUITES
#  ifndef OPENSSL_IS_AWSLC
#    define HAVE_SSL_CTX_سیٹ_POST_HANDSHAKE_AUTH
#  endif
#اگر_ختم

/* Whether SSL_CTX_set1_sigalgs_list is available
 * BoringSSL: supported since 0.20240913.0 (commit 826ce15)
 * LibreSSL: no
 * OpenSSL: supported since 1.0.2 (commit 0b362de5f575)
 */
#اگر_غیر_تعریف_شدہ LIBRESSL_نسخہ_نمبر
#تعریف_کرو HAVE_SSL_CTX_SET1_SIGALGS
#اگر_ختم

#اگر_تعریف_شدہ LIBRESSL_نسخہ_نمبر
#تعریف_کرو OSSL_PACKAGE "LibreSSL"
#وگرنہ_اگر defined(OPENSSL_IS_AWSLC)
#تعریف_کرو OSSL_PACKAGE "AWS-LC"
#وگرنہ_اگر defined(OPENSSL_IS_BORINGSSL)
#تعریف_کرو OSSL_PACKAGE "BoringSSL"
#وگرنہ_اگر defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#تعریف_کرو OSSL_PACKAGE "quictls"
#ورنہ
#تعریف_کرو OSSL_PACKAGE "OpenSSL"
#اگر_ختم

#اگر_تعریف_شدہ HAVE_BORINGSSL_LIKE
قسمتعریف size_t numcert_t;
قسمتعریف uint32_t sslerr_t;
#ورنہ
قسمتعریف صحیح numcert_t;
قسمتعریف غیرعلامتی طویل sslerr_t;
#اگر_ختم
#تعریف_کرو ossl_valsize_t numcert_t

#اگر OPENSSL_نسخہ_نمبر >= 0x40100000L
#تعریف_کرو ossl_ASN1_STRING_get_لمبائی ASN1_STRING_get_لمبائی
#ورنہ
ساکن size_t ossl_ASN1_STRING_get_لمبائی(ثابت ASN1_STRING *str)
{
   ثابت صحیح لمبائی = ASN1_STRING_لمبائی(str);
   واپس لمبائی >= 0 ? (size_t)لمبائی : 0;
}
#اگر_ختم

ساکن CURLcode ڈالو_certinfo(ساخت Curl_easy *ڈیٹا,
                              BIO *mem, ثابت حرف *label, صحیح عدد)
  WARN_غیر_مستعمل_نتیجہ;

ساکن CURLcode ڈالو_certinfo(ساخت Curl_easy *ڈیٹا,
                              BIO *mem, ثابت حرف *label, صحیح عدد)
{
  حرف *ptr;
  طویل len = BIO_get_mem_ڈیٹا(mem, &ptr);
  CURLcode نتیجہ = Curl_ssl_ڈالو_certinfo_len(ڈیٹا, عدد, label, ptr, len);
  (خالی)BIO_reset(mem);
  واپس نتیجہ;
}

ساکن CURLcode pubkey_show(ساخت Curl_easy *ڈیٹا,
                            BIO *mem,
                            صحیح عدد,
                            ثابت حرف *type,
                            ثابت حرف *name,
                            ثابت BIGNUM *bn) WARN_غیر_مستعمل_نتیجہ;

ساکن CURLcode pubkey_show(ساخت Curl_easy *ڈیٹا,
                            BIO *mem,
                            صحیح عدد,
                            ثابت حرف *type,
                            ثابت حرف *name,
                            ثابت BIGNUM *bn)
{
  حرف namebuf[32];

  curl_msnprintf(namebuf, حجم(namebuf), "%s(%s)", type, name);

  اگر(bn)
    BN_print(mem, bn);
  واپس ڈالو_certinfo(ڈیٹا, mem, namebuf, عدد);
}

#تعریف_کرو print_pubkey_BN(_type, _name, عدد_2)           \
  pubkey_show(ڈیٹا, mem, عدد_2, #_type, #_name, _name)

ساکن صحیح asn1_object_dump(ثابت ASN1_OBJECT *a, حرف *buf, size_t len)
{
  صحیح i = i2t_ASN1_OBJECT(buf, (صحیح)len, a);
  واپس (i >= (صحیح)len);  /* buffer too small */
}

ساکن CURLcode X509V3_ext(ساخت Curl_easy *ڈیٹا,
                           صحیح certnum,
                           ثابت ڈھیر_کا(X509_EXTENSION) *extsarg)
{
  صحیح i;
  CURLcode نتیجہ = CURLE_OK;
#اگر_تعریف_شدہ LIBRESSL_نسخہ_نمبر
  ڈھیر_کا(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#ورنہ
  ثابت ڈھیر_کا(X509_EXTENSION) *exts = extsarg;
#اگر_ختم

  اگر((صحیح)sk_X509_EXTENSION_عدد(exts) <= 0)
    /* no extensions, bail out */
    واپس نتیجہ;

  برائے(i = 0; i < (صحیح)sk_X509_EXTENSION_عدد(exts); i++) {
    ثابت ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_قدر(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    حرف namebuf[128];
    BIO *bio_برآمد = BIO_new(BIO_s_mem());

    اگر(!bio_برآمد)
      واپس نتیجہ;

    obj = X509_EXTENSION_get_object(ext);

    اگر(asn1_object_dump(obj, namebuf, حجم(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    اگر(!X509V3_EXT_print(bio_برآمد, ext, 0, 0))
      ASN1_STRING_print(bio_برآمد,
                        (ثابت ASN1_STRING *)X509_EXTENSION_get_ڈیٹا(ext));

    BIO_get_mem_ptr(bio_برآمد, &biomem);
    نتیجہ = Curl_ssl_ڈالو_certinfo_len(ڈیٹا, certnum, namebuf, biomem->ڈیٹا,
                                        biomem->لمبائی);
    BIO_free(bio_برآمد);
    اگر(نتیجہ)
      توڑو;
  }
  واپس نتیجہ;
}

ساکن CURLcode get_pkey_rsa(ساخت Curl_easy *ڈیٹا,
                             EVP_PKEY *pubkey, BIO *mem, صحیح i)
{
  CURLcode نتیجہ = CURLE_OK;
#اگر_غیر_تعریف_شدہ HAVE_EVP_PKEY_GET_PARAMS
  RSA *rsa = EVP_PKEY_get0_RSA(pubkey);
#اگر_ختم /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(n);
  DECLARE_PKEY_PARAM_BIGNUM(e);
#اگر_تعریف_شدہ HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_N, &n);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_E, &e);
#ورنہ
  RSA_get0_کلید(rsa, &n, &e, تہی);
#اگر_ختم /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (صحیح)(n ? BN_عدد_بٹ(n) : 0));
  نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "RSA Public Key", i);
  اگر(!نتیجہ) {
    نتیجہ = print_pubkey_BN(rsa, n, i);
    اگر(!نتیجہ)
      نتیجہ = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ OPENSSL_NO_DSA
ساکن CURLcode get_pkey_dsa(ساخت Curl_easy *ڈیٹا,
                             EVP_PKEY *pubkey, BIO *mem, صحیح i)
{
  CURLcode نتیجہ = CURLE_OK;
#اگر_غیر_تعریف_شدہ HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#اگر_ختم /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_کلید);
#اگر_تعریف_شدہ HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_کلید, &pub_کلید);
#ورنہ
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_کلید(dsa, &pub_کلید, تہی);
#اگر_ختم /* HAVE_EVP_PKEY_GET_PARAMS */
  نتیجہ = print_pubkey_BN(dsa, p, i);
  اگر(!نتیجہ)
    نتیجہ = print_pubkey_BN(dsa, q, i);
  اگر(!نتیجہ)
    نتیجہ = print_pubkey_BN(dsa, g, i);
  اگر(!نتیجہ)
    نتیجہ = print_pubkey_BN(dsa, pub_کلید, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_کلید);
  واپس نتیجہ;
}
#اگر_ختم /* !OPENSSL_NO_DSA */

ساکن CURLcode get_pkey_dh(ساخت Curl_easy *ڈیٹا,
                            EVP_PKEY *pubkey, BIO *mem, صحیح i)
{
  CURLcode نتیجہ;
#اگر_غیر_تعریف_شدہ HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#اگر_ختم /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_کلید);
#اگر_تعریف_شدہ HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_کلید, &pub_کلید);
#ورنہ
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_کلید(dh, &pub_کلید, تہی);
#اگر_ختم /* HAVE_EVP_PKEY_GET_PARAMS */
  نتیجہ = print_pubkey_BN(dh, p, i);
  اگر(!نتیجہ)
    نتیجہ = print_pubkey_BN(dh, q, i);
  اگر(!نتیجہ)
    نتیجہ = print_pubkey_BN(dh, g, i);
  اگر(!نتیجہ)
    نتیجہ = print_pubkey_BN(dh, pub_کلید, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_کلید);
  واپس نتیجہ;
}

#اگر_تعریف_شدہ HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
قسمتعریف ثابت X509_PUBKEY pubkeytype_t;
#ورنہ
قسمتعریف X509_PUBKEY pubkeytype_t;
#اگر_ختم

ساکن CURLcode ossl_certchain(ساخت Curl_easy *ڈیٹا, SSL *ssl)
{
  CURLcode نتیجہ;
  ڈھیر_کا(X509) *sk;
  صحیح i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  اگر(!sk)
    واپس CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_عدد(sk);
  اگر(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(ڈیٹا, "%d certificates is more than allowed (%d)", (صحیح)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  نتیجہ = Curl_ssl_init_certinfo(ڈیٹا, (صحیح)numcerts);
  اگر(نتیجہ)
    واپس نتیجہ;

  mem = BIO_new(BIO_s_mem());
  اگر(!mem)
    نتیجہ = CURLE_برآمد_کا_حافظہ;

  برائے(i = 0; !نتیجہ && (i < (صحیح)numcerts); i++) {
    X509 *x = sk_X509_قدر(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = تہی;
    size_t j;
    ثابت ASN1_ثنائی_ہندسہ_STRING *psig = تہی;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Subject", i);
    اگر(نتیجہ)
      توڑو;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Issuer", i);
    اگر(نتیجہ)
      توڑو;

    BIO_printf(mem, "%lx", (غیرعلامتی طویل)X509_get_نسخہ(x));
    نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Version", i);
    اگر(نتیجہ)
      توڑو;

    {
      ASN1_INTEGER *عدد = X509_get_serial_نمبر(x);
      ثابت غیرعلامتی حرف *numdata = ASN1_STRING_get0_ڈیٹا(عدد);
      ثابت size_t numlen = ossl_ASN1_STRING_get_لمبائی(عدد);
      اگر(ASN1_STRING_type(عدد) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      برائے(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Serial Number", i);
    اگر(نتیجہ)
      توڑو;

    {
      ثابت X509_ALGOR *sigalg = تہی;
      pubkeytype_t *xpubkey = تہی;
      ASN1_OBJECT *pubkeyoid = تہی;

      X509_get0_signature(&psig, &sigalg, x);
      اگر(sigalg) {
        ثابت ASN1_OBJECT *sigalgoid = تہی;
        X509_ALGOR_get0(&sigalgoid, تہی, تہی, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Signature Algorithm", i);
        اگر(نتیجہ)
          توڑو;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      اگر(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, تہی, تہی, تہی, xpubkey);
        اگر(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Public Key Algorithm", i);
          اگر(نتیجہ)
            توڑو;
        }
      }

      نتیجہ = X509V3_ext(ڈیٹا, i, X509_get0_extensions(x));
      اگر(نتیجہ)
        توڑو;
    }

    ASN1_وقت_print(mem, X509_get0_notBefore(x));
    نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Start date", i);
    اگر(نتیجہ)
      توڑو;

    ASN1_وقت_print(mem, X509_get0_notAfter(x));
    نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Expire date", i);
    اگر(نتیجہ)
      توڑو;

    pubkey = X509_get_pubkey(x);
    اگر(!pubkey)
      infof(ڈیٹا, "   Unable to load public key");
    ورنہ {
      انتخاب(EVP_PKEY_id(pubkey)) {
      حالت EVP_PKEY_RSA:
        نتیجہ = get_pkey_rsa(ڈیٹا, pubkey, mem, i);
        توڑو;

#اگر_غیر_تعریف_شدہ OPENSSL_NO_DSA
      حالت EVP_PKEY_DSA:
        نتیجہ = get_pkey_dsa(ڈیٹا, pubkey, mem, i);
        توڑو;
#اگر_ختم

      حالت EVP_PKEY_DH:
        نتیجہ = get_pkey_dh(ڈیٹا, pubkey, mem, i);
        توڑو;
      }
      EVP_PKEY_free(pubkey);
    }

    اگر(!نتیجہ && psig) {
      ثابت غیرعلامتی حرف *psigdata = ASN1_STRING_get0_ڈیٹا(psig);
      ثابت size_t psiglen = ossl_ASN1_STRING_get_لمبائی(psig);
      برائے(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Signature", i);
    }

    اگر(!نتیجہ) {
      PEM_write_bio_X509(mem, x);
      نتیجہ = ڈالو_certinfo(ڈیٹا, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  اگر(نتیجہ)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(ڈیٹا);

  واپس نتیجہ;
}

ساکن صحیح ossl_bio_cf_create(BIO *bio)
{
  BIO_سیٹ_shutdown(bio, 1);
  BIO_سیٹ_init(bio, 1);
  BIO_سیٹ_ڈیٹا(bio, تہی);
  واپس 1;
}

ساکن صحیح ossl_bio_cf_destroy(BIO *bio)
{
  اگر(!bio)
    واپس 0;
  واپس 1;
}

ساکن طویل ossl_bio_cf_ctrl(BIO *bio, صحیح cmd, طویل عدد, خالی *ptr)
{
  ساخت Curl_cfilter *cf = BIO_get_ڈیٹا(bio);
  طویل ret = 1;

  (خالی)cf;
  (خالی)ptr;
  انتخاب(cmd) {
  حالت BIO_CTRL_GET_CLOSE:
    ret = (طویل)BIO_get_shutdown(bio);
    توڑو;
  حالت BIO_CTRL_سیٹ_CLOSE:
    BIO_سیٹ_shutdown(bio, (صحیح)عدد);
    توڑو;
  حالت BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    توڑو;
  حالت BIO_CTRL_DUP:
    ret = 1;
    توڑو;
  حالت BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
    واپس connssl->peer_closed;
  }
  طےشدہ:
    ret = 0;
    توڑو;
  }
  واپس ret;
}

ساکن صحیح ossl_bio_cf_برآمد_write(BIO *bio, ثابت حرف *buf, صحیح blen)
{
  ساخت Curl_cfilter *cf = BIO_get_ڈیٹا(bio);
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا);
  اگر(blen < 0)
    واپس 0;

  نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا,
                             (ثابت uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(ڈیٹا, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (صحیح)نتیجہ, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_نتیجہ = نتیجہ;
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN)
      BIO_سیٹ_retry_write(bio);
    واپس -1;
  }
  واپس (صحیح)nwritten;
}

ساکن صحیح ossl_bio_cf_اندر_read(BIO *bio, حرف *buf, صحیح blen)
{
  ساخت Curl_cfilter *cf = BIO_get_ڈیٹا(bio);
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  size_t nread;
  CURLcode نتیجہ, r2;

  DEBUGASSERT(ڈیٹا);
  /* OpenSSL catches this case, so should we. */
  اگر(!buf)
    واپس 0;
  اگر(blen < 0)
    واپس 0;

  نتیجہ = Curl_conn_cf_recv(cf->اگلا, ڈیٹا, buf, (size_t)blen, &nread);
  CURL_TRC_CF(ڈیٹا, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (صحیح)نتیجہ, nread);
  BIO_clear_retry_flags(bio);
  octx->io_نتیجہ = نتیجہ;
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN)
      BIO_سیٹ_retry_read(bio);
  }
  ورنہ {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->داخلہ_pending = TRUE;
    اگر(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  اگر(!octx->x509_store_تیاری) {
    r2 = Curl_ssl_تیاری_x509_store(cf, ڈیٹا, octx);
    اگر(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_نتیجہ = r2;
      واپس -1;
    }
    octx->x509_store_تیاری = TRUE;
  }
  واپس نتیجہ ? -1 : (صحیح)nread;
}

ساکن BIO_طریقہ *ossl_bio_cf_طریقہ_create(خالی)
{
  BIO_طریقہ *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  اگر(m) {
    BIO_meth_سیٹ_write(m, &ossl_bio_cf_برآمد_write);
    BIO_meth_سیٹ_read(m, &ossl_bio_cf_اندر_read);
    BIO_meth_سیٹ_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_سیٹ_create(m, &ossl_bio_cf_create);
    BIO_meth_سیٹ_destroy(m, &ossl_bio_cf_destroy);
  }
  واپس m;
}

ساکن خالی ossl_bio_cf_طریقہ_free(BIO_طریقہ *m)
{
  اگر(m)
    BIO_meth_free(m);
}

#اگر_غیر_تعریف_شدہ HAVE_KEYLOG_UPSTREAM
#اگر_تعریف_شدہ HAVE_KEYLOG_CALLBACK
ساکن خالی ossl_keylog_callback(ثابت SSL *ssl, ثابت حرف *سطر)
{
  (خالی)ssl;

  Curl_tls_keylog_write_سطر(سطر);
}
#ورنہ
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
ساکن خالی ossl_log_tls12_secret(ثابت SSL *ssl, bool *keylog_مکمل)
{
  ثابت SSL_SESSION *session;
  غیرعلامتی حرف client_random[SSL3_RANDOM_سائز];
  غیرعلامتی حرف master_کلید[SSL_MAX_MASTER_کلید_لمبائی];
  صحیح master_کلید_لمبائی = 0;

  ERR_سیٹ_mark();

  session = SSL_get_session(ssl);

  اگر(!session || *keylog_مکمل) {
    ERR_نکالو_مقصد_mark();
    واپس;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_سائز);
  master_کلید_لمبائی = (صحیح)
    SSL_SESSION_get_master_کلید(session, master_کلید, SSL_MAX_MASTER_کلید_لمبائی);

  ERR_نکالو_مقصد_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  اگر(master_کلید_لمبائی <= 0)
    واپس;

  *keylog_مکمل = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        حجم(client_random),
                        master_کلید, master_کلید_لمبائی);
}
#اگر_ختم /* !HAVE_KEYLOG_CALLBACK */
#اگر_ختم /* HAVE_KEYLOG_UPSTREAM */

ساکن ثابت حرف *SSL_ERROR_مقصد_str(صحیح err)
{
  انتخاب(err) {
  حالت SSL_ERROR_NONE:
    واپس "SSL_ERROR_NONE";
  حالت SSL_ERROR_SSL:
    واپس "SSL_ERROR_SSL";
  حالت SSL_ERROR_WANT_READ:
    واپس "SSL_ERROR_WANT_READ";
  حالت SSL_ERROR_WANT_WRITE:
    واپس "SSL_ERROR_WANT_WRITE";
  حالت SSL_ERROR_WANT_X509_LOOKUP:
    واپس "SSL_ERROR_WANT_X509_LOOKUP";
  حالت SSL_ERROR_SYSCALL:
    واپس "SSL_ERROR_SYSCALL";
  حالت SSL_ERROR_ZERO_RETURN:
    واپس "SSL_ERROR_ZERO_RETURN";
  حالت SSL_ERROR_WANT_CONNECT:
    واپس "SSL_ERROR_WANT_CONNECT";
  حالت SSL_ERROR_WANT_ACCEPT:
    واپس "SSL_ERROR_WANT_ACCEPT";
#اگر_تعریف_شدہ SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  حالت SSL_ERROR_WANT_ASYNC:
    واپس "SSL_ERROR_WANT_ASYNC";
#اگر_ختم
#اگر_تعریف_شدہ SSL_ERROR_WANT_ASYNC_کام  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  حالت SSL_ERROR_WANT_ASYNC_کام:
    واپس "SSL_ERROR_WANT_ASYNC_JOB";
#اگر_ختم
#اگر_تعریف_شدہ SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  حالت SSL_ERROR_WANT_CLIENT_HELLO_CB:
    واپس "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#اگر_ختم
  طےشدہ:
    واپس "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
ساکن حرف *ossl_strerror(غیرعلامتی طویل error, حرف *buf, size_t سائز)
{
  size_t len;
  DEBUGASSERT(سائز);
  *buf = '\0';

  len = Curl_ossl_نسخہ(buf, سائز);
  DEBUGASSERT(len < (سائز - 2));
  اگر(len < (سائز - 2)) {
    buf += len;
    سائز -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#اگر_تعریف_شدہ HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, سائز);
#ورنہ
  ERR_error_string_n(error, buf, سائز);
#اگر_ختم

  اگر(!*buf) {
    ثابت حرف *پیغام = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, سائز, پیغام, لڑی_لمبائی(پیغام));
  }

  واپس buf;
}

ساکن صحیح passwd_callback(حرف *buf, صحیح عدد, صحیح encrypting, خالی *password)
{
  DEBUGASSERT(encrypting == 0);

  اگر(!encrypting && عدد >= 0 && password) {
    صحیح klen = curlx_uztosi(لڑی_لمبائی((حرف *)password));
    اگر(عدد > klen) {
      حافظہ_نقل(buf, password, klen + 1);
      واپس klen;
    }
  }
  واپس 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
ساکن bool rand_enough(خالی)
{
  واپس RAND_حالت() != 0;
}

ساکن CURLcode ossl_بیج(ساخت Curl_easy *ڈیٹا)
{
  /* This might get called before it has been added to a multi handle */
  اگر(ڈیٹا->multi && ڈیٹا->multi->ssl_seeded)
    واپس CURLE_OK;

  اگر(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    اگر(ڈیٹا->multi)
      ڈیٹا->multi->ssl_seeded = TRUE;
    واپس CURLE_OK;
  }
  failf(ڈیٹا, "Insufficient randomness");
  واپس CURLE_SSL_CONNECT_ERROR;
}

#اگر_غیر_تعریف_شدہ SSL_FILETYPE_ENGINE
#تعریف_کرو SSL_FILETYPE_ENGINE 42
#اگر_ختم
#اگر_غیر_تعریف_شدہ SSL_FILETYPE_PKCS12
#تعریف_کرو SSL_FILETYPE_PKCS12 43
#اگر_ختم
#اگر_غیر_تعریف_شدہ SSL_FILETYPE_PROVIDER
#تعریف_کرو SSL_FILETYPE_PROVIDER 44
#اگر_ختم
ساکن صحیح ossl_do_file_type(ثابت حرف *type)
{
  اگر(!type || !type[0])
    واپس SSL_FILETYPE_PEM;
  اگر(curl_strequal(type, "PEM"))
    واپس SSL_FILETYPE_PEM;
  اگر(curl_strequal(type, "DER"))
    واپس SSL_FILETYPE_ASN1;
  اگر(curl_strequal(type, "PROV"))
    واپس SSL_FILETYPE_PROVIDER;
  اگر(curl_strequal(type, "ENG"))
    واپس SSL_FILETYPE_ENGINE;
  اگر(curl_strequal(type, "P12"))
    واپس SSL_FILETYPE_PKCS12;
  واپس -1;
}

#اگر defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
ساکن صحیح ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ثابت حرف *password;
  انتخاب(UI_get_string_type(uis)) {
  حالت UIT_PROMPT:
  حالت UIT_VERIFY:
    password = (ثابت حرف *)UI_get0_user_ڈیٹا(ui);
    اگر(password && (UI_get_داخلہ_flags(uis) & UI_داخلہ_FLAG_DEFAULT_PWD)) {
      UI_سیٹ_نتیجہ(ui, uis, password);
      واپس 1;
    }
    FALLTHROUGH();
  طےشدہ:
    توڑو;
  }
  واپس UI_طریقہ_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
ساکن صحیح ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  انتخاب(UI_get_string_type(uis)) {
  حالت UIT_PROMPT:
  حالت UIT_VERIFY:
    اگر(UI_get0_user_ڈیٹا(ui) &&
       (UI_get_داخلہ_flags(uis) & UI_داخلہ_FLAG_DEFAULT_PWD)) {
      واپس 1;
    }
    FALLTHROUGH();
  طےشدہ:
    توڑو;
  }
  واپس UI_طریقہ_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
ساکن bool is_pkcs11_uri(ثابت حرف *string)
{
  واپس string && curl_strnequal(string, "pkcs11:", 7);
}

#اگر_ختم

ساکن CURLcode ossl_سیٹ_engine(ساخت Curl_easy *ڈیٹا, ساخت ossl_ctx *octx,
                                ثابت حرف *name);
#اگر_تعریف_شدہ OPENSSL_HAS_PROVIDERS
ساکن CURLcode ossl_سیٹ_provider(ساخت Curl_easy *ڈیٹا,
                                  ساخت ossl_ctx *octx,
                                  ثابت حرف *iname);
#اگر_ختم

ساکن صحیح use_certificate_blob(SSL_CTX *ctx, ثابت ساخت curl_blob *blob,
                                صحیح type, ثابت حرف *کلید_passwd)
{
  صحیح ret = 0;
  X509 *x = تہی;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *اندر = BIO_new_mem_buf(blob->ڈیٹا, (صحیح)blob->len);
  اگر(!اندر)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(اندر, تہی);
  }
  ورنہ اگر(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(اندر, تہی, passwd_callback, CURL_UNCONST(کلید_passwd));
  }
  ورنہ {
    ret = 0;
    جاؤ انتہا;
  }

  اگر(!x) {
    ret = 0;
    جاؤ انتہا;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
انتہا:
  X509_free(x);
  BIO_free(اندر);
  واپس ret;
}

ساکن صحیح use_privatekey_blob(SSL_CTX *ctx, ثابت ساخت curl_blob *blob,
                               صحیح type, ثابت حرف *کلید_passwd)
{
  صحیح ret = 0;
  EVP_PKEY *pkey = تہی;
  BIO *اندر = BIO_new_mem_buf(blob->ڈیٹا, (صحیح)blob->len);
  اگر(!اندر)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_نجی_کلید(اندر, تہی, passwd_callback,
                                   CURL_UNCONST(کلید_passwd));
  ورنہ اگر(type == SSL_FILETYPE_ASN1)
    pkey = d2i_نجی_کلید_bio(اندر, تہی);
  ورنہ
    جاؤ انتہا;

  اگر(!pkey)
    جاؤ انتہا;

  ret = SSL_CTX_use_نجی_کلید(ctx, pkey);
  EVP_PKEY_free(pkey);
انتہا:
  BIO_free(اندر);
  واپس ret;
}

ساکن صحیح use_certificate_chain_blob(SSL_CTX *ctx,
                                      ثابت ساخت curl_blob *blob,
                                      ثابت حرف *کلید_passwd)
{
  صحیح ret = 0;
  X509 *x = تہی;
  BIO *اندر = BIO_new_mem_buf(blob->ڈیٹا, (صحیح)blob->len);
  اگر(!اندر)
    واپس CURLE_برآمد_کا_حافظہ;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(اندر, تہی,
                            passwd_callback, CURL_UNCONST(کلید_passwd));
  اگر(!x)
    جاؤ انتہا;

  ret = SSL_CTX_use_certificate(ctx, x);

  اگر(ERR_peek_error() != 0)
    ret = 0;

  اگر(ret) {
    X509 *ca;
    sslerr_t err;

    اگر(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      جاؤ انتہا;
    }

    جب((ca = PEM_read_bio_X509(اندر, تہی, passwd_callback,
                                  CURL_UNCONST(کلید_passwd))) != تہی) {

      اگر(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        جاؤ انتہا;
      }
    }

    err = ERR_peek_آخری_error();
    اگر((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_آغاز_سطر))
      ERR_clear_error();
    ورنہ
      ret = 0;
  }

انتہا:
  X509_free(x);
  BIO_free(اندر);
  واپس ret;
}

ساکن صحیح enginecheck(ساخت Curl_easy *ڈیٹا,
                       ساخت ossl_ctx *octx,
                       ثابت حرف *کلید_file,
                       ثابت حرف *کلید_passwd)
{
#اگر_تعریف_شدہ USE_OPENSSL_ENGINE
  EVP_PKEY *priv_کلید = تہی;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  اگر(!octx->p.engine && is_pkcs11_uri(کلید_file) &&
     ossl_سیٹ_engine(ڈیٹا, octx, "pkcs11") != CURLE_OK)
    واپس 0;

  اگر(octx->p.engine) {
    UI_طریقہ *ui_طریقہ = UI_create_طریقہ("curl user interface");
    اگر(!ui_طریقہ) {
      failf(ڈیٹا, "unable to create " OSSL_PACKAGE " user-interface method");
      واپس 0;
    }
    UI_طریقہ_سیٹ_opener(ui_طریقہ, UI_طریقہ_get_opener(UI_OpenSSL()));
    UI_طریقہ_سیٹ_closer(ui_طریقہ, UI_طریقہ_get_closer(UI_OpenSSL()));
    UI_طریقہ_سیٹ_reader(ui_طریقہ, ssl_ui_reader);
    UI_طریقہ_سیٹ_writer(ui_طریقہ, ssl_ui_writer);
    priv_کلید = ENGINE_بھرو_نجی_کلید(octx->p.engine, کلید_file,
                                       ui_طریقہ,
                                       CURL_UNCONST(کلید_passwd));
    UI_destroy_طریقہ(ui_طریقہ);
    اگر(!priv_کلید) {
      failf(ڈیٹا, "failed to load private key from crypto engine");
      واپس 0;
    }
    اگر(SSL_CTX_use_نجی_کلید(ctx, priv_کلید) != 1) {
      failf(ڈیٹا, "unable to set private key");
      EVP_PKEY_free(priv_کلید);
      واپس 0;
    }
    EVP_PKEY_free(priv_کلید);  /* we do not need the handle any more... */
  }
  ورنہ {
    failf(ڈیٹا, "crypto engine not set, cannot load private key");
    واپس 0;
  }
  واپس 1;
#ورنہ
  (خالی)octx;
  (خالی)کلید_file;
  (خالی)کلید_passwd;
  failf(ڈیٹا, "SSL_FILETYPE_ENGINE not supported for private key");
  واپس 0;
#اگر_ختم
}

ساکن صحیح providercheck(ساخت Curl_easy *ڈیٹا,
                         ساخت ossl_ctx *octx,
                         ثابت حرف *کلید_file)
{
#اگر_تعریف_شدہ OPENSSL_HAS_PROVIDERS
  حرف error_بفر[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  اگر(!octx->p.provider_loaded && is_pkcs11_uri(کلید_file) &&
     ossl_سیٹ_provider(ڈیٹا, octx, "pkcs11") != CURLE_OK) {
    واپس 0;
  }

  اگر(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_کلید = تہی;
    OSSL_STORE_CTX *store = تہی;
    OSSL_STORE_INFO *info = تہی;
    UI_طریقہ *ui_طریقہ = UI_create_طریقہ("curl user interface");
    اگر(!ui_طریقہ) {
      failf(ڈیٹا, "unable to create " OSSL_PACKAGE " user-interface method");
      واپس 0;
    }
    UI_طریقہ_سیٹ_opener(ui_طریقہ, UI_طریقہ_get_opener(UI_OpenSSL()));
    UI_طریقہ_سیٹ_closer(ui_طریقہ, UI_طریقہ_get_closer(UI_OpenSSL()));
    UI_طریقہ_سیٹ_reader(ui_طریقہ, ssl_ui_reader);
    UI_طریقہ_سیٹ_writer(ui_طریقہ, ssl_ui_writer);

    store = OSSL_STORE_open_ex(کلید_file, octx->p.libctx,
                               octx->p.propq, ui_طریقہ, تہی, تہی,
                               تہی, تہی);
    اگر(!store) {
      failf(ڈیٹا, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      UI_destroy_طریقہ(ui_طریقہ);
      واپس 0;
    }
    اگر(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(ڈیٹا, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
    }

    info = OSSL_STORE_بھرو(store);
    اگر(info) {
      صحیح ossl_type = OSSL_STORE_INFO_get_type(info);

      اگر(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_کلید = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_طریقہ(ui_طریقہ);
    اگر(!priv_کلید) {
      failf(ڈیٹا, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      واپس 0;
    }

    اگر(SSL_CTX_use_نجی_کلید(ctx, priv_کلید) != 1) {
      failf(ڈیٹا, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      EVP_PKEY_free(priv_کلید);
      واپس 0;
    }
    EVP_PKEY_free(priv_کلید); /* we do not need the handle any more... */
  }
  ورنہ {
    failf(ڈیٹا, "crypto provider not set, cannot load private key");
    واپس 0;
  }
  واپس 1;
#ورنہ
  (خالی)octx;
  (خالی)کلید_file;
  failf(ڈیٹا, "SSL_FILETYPE_PROVIDER not supported for private key");
  واپس 0;
#اگر_ختم
}

ساکن صحیح engineload(ساخت Curl_easy *ڈیٹا,
                      ساخت ossl_ctx *octx,
                      ثابت حرف *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#اگر defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  حرف error_بفر[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  اگر(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_سیٹ_engine(ڈیٹا, octx, "pkcs11") != CURLE_OK)
    واپس 0;

  اگر(octx->p.engine) {
    ساکن ثابت حرف cmd_name[] = "LOAD_CERT_CTRL";
    ساخت {
      ثابت حرف *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = تہی;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    اگر(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), تہی)) {
      failf(ڈیٹا, "SSL engine does not support loading certificates");
      واپس 0;
    }

    /* Load the certificate from the engine */
    اگر(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, تہی, 1)) {
      failf(ڈیٹا, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      واپس 0;
    }

    اگر(!params.cert) {
      failf(ڈیٹا, "SSL engine did not initialize the certificate properly.");
      واپس 0;
    }

    اگر(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(ڈیٹا, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      X509_free(params.cert);
      واپس 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  ورنہ {
    failf(ڈیٹا, "crypto engine not set, cannot load certificate");
    واپس 0;
  }
  واپس 1;
#ورنہ
  (خالی)octx;
  (خالی)cert_file;
  failf(ڈیٹا, "SSL_FILETYPE_ENGINE not supported for certificate");
  واپس 0;
#اگر_ختم
}

ساکن صحیح providerload(ساخت Curl_easy *ڈیٹا,
                        ساخت ossl_ctx *octx,
                        ثابت حرف *cert_file)
{
#اگر_تعریف_شدہ OPENSSL_HAS_PROVIDERS
  حرف error_بفر[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  اگر(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_سیٹ_provider(ڈیٹا, octx, "pkcs11") != CURLE_OK)
    واپس 0;

  اگر(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = تہی;
    X509 *cert = تہی;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         تہی, تہی, تہی, تہی, تہی, تہی);
    صحیح rc;

    اگر(!store) {
      failf(ڈیٹا, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      واپس 0;
    }
    اگر(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(ڈیٹا, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
    }

    info = OSSL_STORE_بھرو(store);
    اگر(info) {
      صحیح ossl_type = OSSL_STORE_INFO_get_type(info);

      اگر(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    اگر(!cert) {
      failf(ڈیٹا, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      واپس 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    اگر(rc != 1) {
      failf(ڈیٹا, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      واپس 0;
    }
  }
  ورنہ {
    failf(ڈیٹا, "crypto provider not set, cannot load certificate");
    واپس 0;
  }
  واپس 1;
#ورنہ
  (خالی)octx;
  (خالی)cert_file;
  failf(ڈیٹا, "SSL_FILETYPE_PROVIDER not supported for certificate");
  واپس 0;
#اگر_ختم
}

ساکن صحیح pkcs12load(ساخت Curl_easy *ڈیٹا,
                      SSL_CTX* ctx,
                      ثابت ساخت curl_blob *cert_blob,
                      ثابت حرف *cert_file,
                      ثابت حرف *کلید_passwd)
{
  حرف error_بفر[256];
  BIO *cert_bio = تہی;
  PKCS12 *p12 = تہی;
  EVP_PKEY *pri;
  X509 *x509;
  صحیح cert_مکمل = 0;
  ڈھیر_کا(X509) *ca = تہی;
  اگر(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->ڈیٹا, (صحیح)cert_blob->len);
    اگر(!cert_bio) {
      failf(ڈیٹا, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      واپس 0;
    }
  }
  ورنہ {
    cert_bio = BIO_new(BIO_s_file());
    اگر(!cert_bio) {
      failf(ڈیٹا, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
      واپس 0;
    }

    اگر(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(ڈیٹا, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      واپس 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, تہی);
  BIO_free(cert_bio);

  اگر(!p12) {
    failf(ڈیٹا, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    واپس 0;
  }

  اگر(!PKCS12_parse(p12, کلید_passwd, &pri, &x509, &ca)) {
    failf(ڈیٹا, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_بفر, حجم(error_بفر)));
    PKCS12_free(p12);
    واپس 0;
  }

  PKCS12_free(p12);

  اگر(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(ڈیٹا, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_بفر, حجم(error_بفر)));
    جاؤ fail;
  }

  اگر(SSL_CTX_use_نجی_کلید(ctx, pri) != 1) {
    failf(ڈیٹا, "unable to use private key from PKCS12 file '%s'", cert_file);
    جاؤ fail;
  }

  اگر(!SSL_CTX_check_نجی_کلید(ctx)) {
    failf(ڈیٹا, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    جاؤ fail;
  }
  /* Set Certificate Verification chain */
  اگر(ca) {
    جب(sk_X509_عدد(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_نکالو(ca);
      اگر(!SSL_CTX_جوڑو_client_CA(ctx, x)) {
        X509_free(x);
        failf(ڈیٹا, "cannot add certificate to client CA list");
        جاؤ fail;
      }
      اگر(!SSL_CTX_جوڑو_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(ڈیٹا, "cannot add certificate to certificate chain");
        جاؤ fail;
      }
    }
  }

  cert_مکمل = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
  sk_X509_نکالو_free(ca, X509_free);
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
  اگر(!cert_مکمل)
    واپس 0; /* failure! */
  واپس 1;
}

ساکن CURLcode client_cert(ساخت Curl_easy *ڈیٹا,
                            ساخت ossl_ctx *octx,
                            حرف *cert_file,
                            ثابت ساخت curl_blob *cert_blob,
                            ثابت حرف *cert_type,
                            حرف *کلید_file,
                            ثابت ساخت curl_blob *کلید_blob,
                            ثابت حرف *کلید_type,
                            حرف *کلید_passwd)
{
  حرف error_بفر[256];
  bool check_privkey = TRUE;
  صحیح file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  اگر(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_مکمل = FALSE;
    صحیح cert_use_نتیجہ;

    اگر(کلید_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_سیٹ_default_passwd_cb_userdata(ctx, کلید_passwd);
      /* Set passwd callback: */
      SSL_CTX_سیٹ_default_passwd_cb(ctx, passwd_callback);
    }

    انتخاب(file_type) {
    حالت SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_نتیجہ = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, کلید_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      اگر(cert_use_نتیجہ != 1) {
        failf(ڈیٹا,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_بفر,
                            حجم(error_بفر)));
        واپس CURLE_SSL_CERTPROBLEM;
      }
      توڑو;

    حالت SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_نتیجہ = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, کلید_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      اگر(cert_use_نتیجہ != 1) {
        failf(ڈیٹا,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_بفر,
                            حجم(error_بفر)));
        واپس CURLE_SSL_CERTPROBLEM;
      }
      توڑو;

    حالت SSL_FILETYPE_ENGINE:
      اگر(!cert_file || !engineload(ڈیٹا, octx, cert_file))
        واپس CURLE_SSL_CERTPROBLEM;
      توڑو;

    حالت SSL_FILETYPE_PROVIDER:
      اگر(!cert_file || !providerload(ڈیٹا, octx, cert_file))
        واپس CURLE_SSL_CERTPROBLEM;
      توڑو;

    حالت SSL_FILETYPE_PKCS12:
      اگر(!pkcs12load(ڈیٹا, ctx, cert_blob, cert_file, کلید_passwd))
        واپس CURLE_SSL_CERTPROBLEM;
      pcks12_مکمل = TRUE;
      توڑو;

    طےشدہ:
      failf(ڈیٹا, "not supported file type '%s' for certificate", cert_type);
      واپس CURLE_BAD_FUNCTION_دلیل;
    }

    اگر(!کلید_file && !کلید_blob) {
      کلید_file = cert_file;
      کلید_blob = cert_blob;
    }
    ورنہ
      file_type = ossl_do_file_type(کلید_type);

    انتخاب(file_type) {
    حالت SSL_FILETYPE_PEM:
    حالت SSL_FILETYPE_ASN1:
      cert_use_نتیجہ = کلید_blob ?
        use_privatekey_blob(ctx, کلید_blob, file_type, کلید_passwd) :
      SSL_CTX_use_نجی_کلید_file(ctx, کلید_file, file_type);
      اگر(cert_use_نتیجہ != 1) {
        failf(ڈیٹا, "unable to set private key file: '%s' type %s",
              کلید_file ? کلید_file : "(memory blob)",
              کلید_type ? کلید_type : "PEM");
        واپس CURLE_BAD_FUNCTION_دلیل;
      }
      توڑو;
    حالت SSL_FILETYPE_ENGINE:
      اگر(!enginecheck(ڈیٹا, octx, کلید_file, کلید_passwd))
        واپس CURLE_SSL_CERTPROBLEM;
      توڑو;

    حالت SSL_FILETYPE_PROVIDER:
      اگر(!providercheck(ڈیٹا, octx, کلید_file))
        واپس CURLE_SSL_CERTPROBLEM;
      توڑو;

    حالت SSL_FILETYPE_PKCS12:
      اگر(!pcks12_مکمل) {
        failf(ڈیٹا, "file type P12 for private key not supported");
        واپس CURLE_SSL_CERTPROBLEM;
      }
      توڑو;
    طےشدہ:
      failf(ڈیٹا, "not supported file type for private key");
      واپس CURLE_BAD_FUNCTION_دلیل;
    }

    ssl = SSL_new(ctx);
    اگر(!ssl) {
      failf(ڈیٹا, "unable to create an SSL structure");
      واپس CURLE_برآمد_کا_حافظہ;
    }

    x509 = SSL_get_certificate(ssl);

    اگر(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#اگر !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_کلید = SSL_get_privatekey(ssl);
      اگر(EVP_PKEY_id(priv_کلید) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_کلید);
        اگر(RSA_flags(rsa) & RSA_طریقہ_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#اگر_ختم

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    اگر(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      اگر(!SSL_CTX_check_نجی_کلید(ctx)) {
        failf(ڈیٹا, "Private key does not match the certificate public key");
        واپس CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  واپس CURLE_OK;
}

#اگر_تعریف_شدہ CURLVERBOSE
/* returns non-zero on failure */
ساکن CURLcode x509_name_oneline(ثابت X509_NAME *a, ساخت dynbuf *d)
{
  BIO *bio_برآمد = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  صحیح rc;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  اگر(bio_برآمد) {
    غیرعلامتی طویل flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_برآمد, a, 0, flags);
    اگر(rc != -1) {
      BIO_get_mem_ptr(bio_برآمد, &biomem);
      نتیجہ = curlx_dyn_addn(d, biomem->ڈیٹا, biomem->لمبائی);
    }
    BIO_free(bio_برآمد);
  }
  واپس نتیجہ;
}
#اگر_ختم

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ساکن صحیح ossl_init(خالی)
{
  ثابت uint64_t flags =
#اگر_تعریف_شدہ OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#اگر_ختم
#اگر_تعریف_شدہ CURL_DISABLE_OPENSSL_AUTO_بھرو_CONFIG
    OPENSSL_INIT_NO_بھرو_CONFIG |
#ورنہ
    OPENSSL_INIT_بھرو_CONFIG |
#اگر_ختم
    0;
  OPENSSL_init_ssl(flags, تہی);

#اگر_غیر_تعریف_شدہ HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#اگر_ختم

  واپس 1;
}

/* Global cleanup */
ساکن خالی ossl_cleanup(خالی)
{
#اگر_غیر_تعریف_شدہ HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#اگر_ختم
}

/* Selects an OpenSSL crypto engine or provider.
 */
ساکن CURLcode ossl_سیٹ_engine(ساخت Curl_easy *ڈیٹا, ساخت ossl_ctx *octx,
                                ثابت حرف *name)
{
#اگر_تعریف_شدہ USE_OPENSSL_ENGINE
  CURLcode نتیجہ = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  اگر(e) {

    اگر(octx->p.engine) {
      ENGINE_اختتام(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = تہی;
    }
    اگر(!ENGINE_init(e)) {
      حرف buf[256];

      ENGINE_free(e);
      failf(ڈیٹا, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, حجم(buf)));
      نتیجہ = CURLE_SSL_ENGINE_INITFAILED;
      e = تہی;
    }
    ورنہ {
      نتیجہ = CURLE_OK;
    }
    octx->p.engine = e;
    واپس نتیجہ;
  }
#اگر_ختم
#اگر_تعریف_شدہ OPENSSL_HAS_PROVIDERS
  واپس ossl_سیٹ_provider(ڈیٹا, octx, name);
#ورنہ
  (خالی)octx;
  (خالی)name;
  failf(ڈیٹا, "OpenSSL engine not found");
  واپس CURLE_SSL_ENGINE_NOTFOUND;
#اگر_ختم
}

/* Sets engine as default for all SSL operations
 */
ساکن CURLcode ossl_سیٹ_engine_default(ساخت Curl_easy *ڈیٹا,
                                        ساخت ossl_ctx *octx)
{
#اگر_تعریف_شدہ USE_OPENSSL_ENGINE
  اگر(octx->p.engine) {
    اگر(ENGINE_سیٹ_default(octx->p.engine, ENGINE_طریقہ_ALL) > 0) {
      infof(ڈیٹا, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    ورنہ {
      failf(ڈیٹا, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      واپس CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#ورنہ
  (خالی)ڈیٹا;
  (خالی)octx;
#اگر_ختم
  واپس CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
ساکن ساخت curl_slist *ossl_engines_list(ساخت Curl_easy *ڈیٹا)
{
  ساخت curl_slist *list = تہی;
#اگر_تعریف_شدہ USE_OPENSSL_ENGINE
  ساخت curl_slist *beg;
  ENGINE *e;

  برائے(e = ENGINE_get_پہلا(); e; e = ENGINE_get_اگلا(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    اگر(!beg) {
      curl_slist_free_all(list);
      واپس تہی;
    }
    list = beg;
  }
#اگر_ختم
  (خالی)ڈیٹا;
  واپس list;
}

#اگر_تعریف_شدہ OPENSSL_HAS_PROVIDERS

ساکن خالی ossl_provider_cleanup(ساخت ossl_ctx *octx)
{
  اگر(!octx)
    واپس;
  اگر(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = تہی;
  }
  اگر(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = تہی;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = تہی;
  curlx_safefree(octx->p.propq);
  octx->p.provider_loaded = FALSE;
}

#تعریف_کرو MAX_PROVIDER_LEN 128 /* reasonable */

/* Selects an OpenSSL crypto provider.
 *
 * A provider might need an associated property, a string passed on to
 * OpenSSL. Specify this as [PROVIDER][:PROPERTY]: separate the name and the
 * property with a colon. No colon means no property is set.
 *
 * An example provider + property looks like "tpm2:?provider=tpm2".
 */
ساکن CURLcode ossl_سیٹ_provider(ساخت Curl_easy *ڈیٹا,
                                  ساخت ossl_ctx *octx,
                                  ثابت حرف *iname)
{
  حرف name[MAX_PROVIDER_LEN + 1];
  ساخت Curl_str prov;
  ثابت حرف *propq = تہی;

  اگر(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    واپس CURLE_OK;
  }
  اگر(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  حافظہ_نقل(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  اگر(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    اگر(!libctx)
      واپس CURLE_برآمد_کا_حافظہ;
    اگر(propq) {
      octx->p.propq = curlx_strdup(propq);
      اگر(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        واپس CURLE_برآمد_کا_حافظہ;
      }
    }
    octx->p.libctx = libctx;
  }

#اگر_غیر_تعریف_شدہ CURL_DISABLE_OPENSSL_AUTO_بھرو_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  اگر(!OSSL_LIB_CTX_بھرو_config(octx->p.libctx, تہی)) {
    infof(ڈیٹا, "Failed to load default openssl config. Proceeding.");
  }
#اگر_ختم

  اگر(OSSL_PROVIDER_دستیاب(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    واپس CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_بھرو(octx->p.libctx, name, 1);
  اگر(!octx->p.provider) {
    حرف error_بفر[256];
    failf(ڈیٹا, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_بفر, حجم(error_بفر)));
    ossl_provider_cleanup(octx);
    واپس CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_بھرو(octx->p.libctx, "base", 1);
  اگر(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(ڈیٹا, "Failed to load base");
    واپس CURLE_SSL_ENGINE_NOTFOUND;
  }
  ورنہ
    octx->p.provider_loaded = TRUE;
  واپس CURLE_OK;
}
#ورنہ /* OPENSSL_HAS_PROVIDERS */
#تعریف_کرو ossl_provider_cleanup(x)
#اگر_ختم

ساکن CURLcode ossl_shutdown(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              bool send_shutdown, bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  حرف buf[1024];
  صحیح nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  اگر(!octx->ssl || cf->shutdown) {
    *مکمل = TRUE;
    جاؤ برآمد;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *مکمل = FALSE;
  اگر(!(SSL_get_shutdown(octx->ssl) & SSL_بھیجا_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    برائے(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (صحیح)حجم(buf));
      CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown not sent, read -> %d", nread);
      اگر(nread <= 0)
        توڑو;
    }
    err = SSL_get_error(octx->ssl, nread);
    اگر(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool داخلہ_pending;
      /* Yes, it did. */
      اگر(!send_shutdown) {
        CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown received, not sending");
        *مکمل = TRUE;
        جاؤ برآمد;
      }
      ورنہ اگر(!cf->اگلا->cft->is_alive(cf->اگلا, ڈیٹا, &داخلہ_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(ڈیٹا, cf, "peer closed connection");
        *مکمل = TRUE;
        جاؤ برآمد;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  اگر(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_بھیجا_SHUTDOWN)) {
    صحیح rc;
    ERR_clear_error();
    CURL_TRC_CF(ڈیٹا, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    اگر(rc == 1) {
      CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown finished");
      *مکمل = TRUE;
      جاؤ برآمد;
    }
    اگر(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      جاؤ برآمد;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  برائے(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (صحیح)حجم(buf));
    CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown read -> %d", nread);
    اگر(nread <= 0)
      توڑو;
  }
  err = SSL_get_error(octx->ssl, nread);
  انتخاب(err) {
  حالت SSL_ERROR_ZERO_RETURN: /* no more data */
    اگر(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown finished");
    ورنہ
      CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown not received, but closed");
    *مکمل = TRUE;
    توڑو;
  حالت SSL_ERROR_NONE: /* did not get anything */
  حالت SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    توڑو;
  حالت SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    توڑو;
  طےشدہ:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(غیرعلامتی طویل sslerr = ERR_get_error());
      CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, حجم(buf)) :
                   SSL_ERROR_مقصد_str(err)),
                  SOCKERRNO);
    }
    *مکمل = TRUE;
    نتیجہ = CURLE_OK;
    توڑو;
  }

برآمد:
  cf->shutdown = (نتیجہ || *مکمل);
  اگر(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->داخلہ_pending = FALSE;
  واپس نتیجہ;
}

/* also used by quic-tls */
خالی Curl_osslctx_cleanup(ساخت ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  اگر(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = تہی;
  }
  اگر(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = تہی;
    octx->x509_store_تیاری = FALSE;
  }
  اگر(octx->bio_طریقہ) {
    ossl_bio_cf_طریقہ_free(octx->bio_طریقہ);
    octx->bio_طریقہ = تہی;
  }
#اگر_تعریف_شدہ USE_OPENSSL_ENGINE
  اگر(octx->p.engine) {
    ENGINE_اختتام(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = تہی;
  }
#اگر_ختم
  ossl_provider_cleanup(octx);
}

ساکن خالی ossl_close(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  (خالی)ڈیٹا;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
ساکن خالی ossl_close_all(ساخت Curl_easy *ڈیٹا)
{
  (خالی)ڈیٹا;
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
ساکن CURLcode ossl_verifyhost(ساخت Curl_easy *ڈیٹا,
                                ساخت connectdata *conn,
                                ساخت ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  صحیح ہدف; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  ڈھیر_کا(GENERAL_NAME) *altnames;
#اگر_تعریف_شدہ USE_IPV6
  ساخت in6_addr addr;
#ورنہ
  ساخت in_addr addr;
#اگر_ختم
  CURLcode نتیجہ = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = لڑی_لمبائی(peer->origin->hostname);

  (خالی)conn;
  انتخاب(peer->type) {
  حالت CURL_SSL_PEER_IPV4:
    اگر(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      واپس CURLE_PEER_FAILED_VERIFICATION;
    ہدف = GEN_IPADD;
    addrlen = حجم(ساخت in_addr);
    توڑو;
#اگر_تعریف_شدہ USE_IPV6
  حالت CURL_SSL_PEER_IPV6:
    اگر(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      واپس CURLE_PEER_FAILED_VERIFICATION;
    ہدف = GEN_IPADD;
    addrlen = حجم(ساخت in6_addr);
    توڑو;
#اگر_ختم
  حالت CURL_SSL_PEER_DNS:
    ہدف = GEN_DNS;
    توڑو;
  طےشدہ:
    DEBUGASSERT(0);
    failf(ڈیٹا, "unexpected SSL peer type: %d", (صحیح)peer->type);
    واپس CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, تہی, تہی);

  اگر(altnames) {
#اگر_تعریف_شدہ HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#ورنہ
    صحیح numalts;
    صحیح i;
#اگر_ختم

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_عدد(altnames);

    /* loop through all alternatives - until a dnsmatch */
    برائے(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ثابت GENERAL_NAME *check = sk_GENERAL_NAME_قدر(altnames, i);

      اگر(check->type == GEN_DNS)
        dNSName = TRUE;
      ورنہ اگر(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      اگر(check->type == ہدف) {
        /* get data and length */
        ثابت حرف *altptr = (ثابت حرف *)ASN1_STRING_get0_ڈیٹا(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_لمبائی(check->d.ia5);

        انتخاب(ہدف) {
        حالت GEN_DNS: /* name/pattern comparison */
          اگر(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(ڈیٹا, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (صحیح)altlen, altptr);
          }
          توڑو;

        حالت GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          اگر((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(ڈیٹا, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          توڑو;
        }
      }
    }
    GENERAL_نام_free(altnames);
  }

  اگر(matched)
    /* an alternative name matched */
    ;
  ورنہ اگر(dNSName || iPAddress) {
    ثابت حرف *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(ڈیٹا, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(ڈیٹا, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
  }
  ورنہ {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    صحیح i = -1;
    غیرعلامتی حرف *cn = تہی;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ثابت X509_NAME *name = X509_get_subject_name(server_cert);
    اگر(name) {
      صحیح j;
      جب((j = X509_NAME_get_اشاریہ_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    اگر(i >= 0) {
      ثابت ASN1_STRING *tmp =
        X509_NAME_اندراج_get_ڈیٹا(X509_NAME_get_اندراج(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      اگر(tmp) {
        اگر(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_لمبائی(tmp);
          cn = (غیرعلامتی حرف *)CURL_UNCONST(ASN1_STRING_get0_ڈیٹا(tmp));
        }
        ورنہ { /* not a UTF8 name */
          ثابت صحیح utf8len = ASN1_STRING_مقصد_UTF8(&cn, tmp);
          اگر(utf8len < 0)
            cnlen = 0;
          ورنہ
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        اگر((cnlen == 0) || !cn)
          نتیجہ = CURLE_برآمد_کا_حافظہ;
        ورنہ اگر(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(ڈیٹا, "SSL: illegal cert name field");
          نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    اگر(نتیجہ)
      /* error already detected, pass through */
      ;
    ورنہ اگر(!cn) {
      failf(ڈیٹا, "SSL: unable to obtain common name from peer certificate");
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ورنہ اگر(!Curl_cert_hostcheck((ثابت حرف *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(ڈیٹا, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (صحیح)cnlen, cn,
            peer->origin->user_hostname);
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ورنہ {
      infof(ڈیٹا, " common name: %.*s (matched)", (صحیح)cnlen, cn);
    }
    اگر(free_cn)
      OPENSSL_free(cn);
  }

  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ OPENSSL_NO_OCSP
ساکن CURLcode verifystatus(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             ساخت ossl_ctx *octx)
{
  صحیح i, ocsp_حالت;
#اگر_تعریف_شدہ HAVE_BORINGSSL_LIKE
  ثابت uint8_t *حالت_2;
#ورنہ
  غیرعلامتی حرف *حالت_2;
#اگر_ختم
  ثابت غیرعلامتی حرف *p;
  CURLcode نتیجہ = CURLE_OK;
  OCSP_RESPONSE *rsp = تہی;
  OCSP_BASICRESP *br = تہی;
  X509_STORE     *st = تہی;
  ڈھیر_کا(X509) *ch = تہی;
  X509 *cert;
  OCSP_CERTID *id = تہی;
  صحیح cert_حالت, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  صحیح ret;
  طویل len;

  (خالی)cf;
  DEBUGASSERT(octx);

  len = (طویل)SSL_get_tlsext_حالت_ocsp_resp(octx->ssl, &حالت_2);

  اگر(!حالت_2) {
    failf(ڈیٹا, "No OCSP response received");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }
  p = حالت_2;
  rsp = d2i_OCSP_RESPONSE(تہی, &p, len);
  اگر(!rsp) {
    failf(ڈیٹا, "Invalid OCSP response");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }

  ocsp_حالت = OCSP_response_حالت(rsp);
  اگر(ocsp_حالت != OCSP_RESPONSE_حالت_SUCCESSFUL) {
    failf(ڈیٹا, "Invalid OCSP response status: %s (%d)",
          OCSP_response_حالت_str(ocsp_حالت), ocsp_حالت);
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }

  br = OCSP_response_get1_basic(rsp);
  اگر(!br) {
    failf(ڈیٹا, "Invalid OCSP response");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }

#اگر !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_نسخہ_نمبر) && LIBRESSL_نسخہ_نمبر < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#ورنہ
  ch = SSL_get_peer_cert_chain(octx->ssl);
#اگر_ختم
  اگر(!ch) {
    failf(ڈیٹا, "Could not get peer certificate chain");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  اگر(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(ڈیٹا, "OCSP response verification failed");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  اگر(!cert) {
    failf(ڈیٹا, "Error getting peer certificate");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }

  برائے(i = 0; i < (صحیح)sk_X509_عدد(ch); i++) {
    X509 *issuer = sk_X509_قدر(ch, (ossl_valsize_t)i);
    اگر(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_مقصد_id(EVP_sha1(), cert, issuer);
      توڑو;
    }
  }
  X509_free(cert);

  اگر(!id) {
    failf(ڈیٹا, "Error computing OCSP ID");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_ڈھونڈو_حالت(br, id, &cert_حالت, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  اگر(ret != 1) {
    failf(ڈیٹا, "Could not find certificate ID in OCSP response");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  اگر(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(ڈیٹا, "OCSP response has expired");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }

  infof(ڈیٹا, "SSL certificate status: %s (%d)",
        OCSP_cert_حالت_str(cert_حالت), cert_حالت);

  انتخاب(cert_حالت) {
  حالت V_OCSP_CERTSTATUS_GOOD:
    توڑو;

  حالت V_OCSP_CERTSTATUS_REVOKED:
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    failf(ڈیٹا, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    جاؤ انتہا;

  حالت V_OCSP_CERTSTATUS_UNKNOWN:
  طےشدہ:
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ انتہا;
  }

انتہا:
  اگر(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  واپس نتیجہ;
}
#اگر_ختم

ساکن ثابت حرف *ssl_پیغام_type(صحیح ssl_ver, صحیح پیغام)
{
  اگر(ssl_ver == SSL3_نسخہ_MAJOR) {
    انتخاب(پیغام) {
    حالت SSL3_MT_HELLO_REQUEST:
      واپس "Hello request";
    حالت SSL3_MT_CLIENT_HELLO:
      واپس "Client hello";
    حالت SSL3_MT_SERVER_HELLO:
      واپس "Server hello";
#اگر_تعریف_شدہ SSL3_MT_NEWSESSION_TICKET
    حالت SSL3_MT_NEWSESSION_TICKET:
      واپس "Newsession Ticket";
#اگر_ختم
    حالت SSL3_MT_CERTIFICATE:
      واپس "Certificate";
    حالت SSL3_MT_SERVER_کلید_EXCHANGE:
      واپس "Server key exchange";
    حالت SSL3_MT_CLIENT_کلید_EXCHANGE:
      واپس "Client key exchange";
    حالت SSL3_MT_CERTIFICATE_REQUEST:
      واپس "Request CERT";
    حالت SSL3_MT_SERVER_مکمل:
      واپس "Server finished";
    حالت SSL3_MT_CERTIFICATE_VERIFY:
      واپس "CERT verify";
    حالت SSL3_MT_ختم_شدہ:
      واپس "Finished";
#اگر_تعریف_شدہ SSL3_MT_CERTIFICATE_حالت
    حالت SSL3_MT_CERTIFICATE_حالت:
      واپس "Certificate Status";
#اگر_ختم
#اگر_تعریف_شدہ SSL3_MT_ENCRYPTED_EXTENSIONS
    حالت SSL3_MT_ENCRYPTED_EXTENSIONS:
      واپس "Encrypted Extensions";
#اگر_ختم
#اگر_تعریف_شدہ SSL3_MT_SUPPLEMENTAL_ڈیٹا
    حالت SSL3_MT_SUPPLEMENTAL_ڈیٹا:
      واپس "Supplemental data";
#اگر_ختم
#اگر_تعریف_شدہ SSL3_MT_انتہا_کا_EARLY_ڈیٹا
    حالت SSL3_MT_انتہا_کا_EARLY_ڈیٹا:
      واپس "End of early data";
#اگر_ختم
#اگر_تعریف_شدہ SSL3_MT_کلید_UPDATE
    حالت SSL3_MT_کلید_UPDATE:
      واپس "Key update";
#اگر_ختم
#اگر_تعریف_شدہ SSL3_MT_اگلا_PROTO
    حالت SSL3_MT_اگلا_PROTO:
      واپس "Next protocol";
#اگر_ختم
#اگر_تعریف_شدہ SSL3_MT_پیغام_HASH
    حالت SSL3_MT_پیغام_HASH:
      واپس "Message hash";
#اگر_ختم
    }
  }
  واپس "Unknown";
}

ساکن ثابت حرف *tls_rt_type(صحیح type)
{
  انتخاب(type) {
#اگر_تعریف_شدہ SSL3_RT_سرخی
  حالت SSL3_RT_سرخی:
    واپس "TLS header";
#اگر_ختم
  حالت SSL3_RT_CHANGE_CIPHER_SPEC:
    واپس "TLS change cipher";
  حالت SSL3_RT_ALERT:
    واپس "TLS alert";
  حالت SSL3_RT_HANDSHAKE:
    واپس "TLS handshake";
  حالت SSL3_RT_APPLICATION_ڈیٹا:
    واپس "TLS app data";
  طےشدہ:
    واپس "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
ساکن خالی ossl_trace(صحیح direction, صحیح ssl_ver, صحیح content_type,
                       ثابت خالی *buf, size_t len, SSL *ssl,
                       خالی *userp)
{
  ثابت حرف *verstr;
  ساخت Curl_cfilter *cf = userp;
  ساخت Curl_easy *ڈیٹا = تہی;
  حرف unknown[32];

  اگر(!cf)
    واپس;
  ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  اگر(!ڈیٹا || !ڈیٹا->سیٹ.fdebug || (direction && direction != 1))
    واپس;

  انتخاب(ssl_ver) {
#اگر_تعریف_شدہ SSL3_نسخہ
  حالت SSL3_نسخہ:
    verstr = "SSLv3";
    توڑو;
#اگر_ختم
  حالت TLS1_نسخہ:
    verstr = "TLSv1.0";
    توڑو;
#اگر_تعریف_شدہ TLS1_1_نسخہ
  حالت TLS1_1_نسخہ:
    verstr = "TLSv1.1";
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ TLS1_2_نسخہ
  حالت TLS1_2_نسخہ:
    verstr = "TLSv1.2";
    توڑو;
#اگر_ختم
  حالت TLS1_3_نسخہ:
    verstr = "TLSv1.3";
    توڑو;
  طےشدہ:
    curl_msnprintf(unknown, حجم(unknown), "(%x)", (غیرعلامتی صحیح)ssl_ver);
    verstr = unknown;
    توڑو;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  اگر(ssl_ver
#اگر_تعریف_شدہ SSL3_RT_سرخی
     && content_type != SSL3_RT_سرخی
#اگر_ختم
#اگر_تعریف_شدہ SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#اگر_ختم
    ) {
    ثابت حرف *پیغام_name = "Truncated message";
    ثابت حرف *tls_rt_name;
    حرف ssl_buf[1024];
    صحیح پیغام_type = 0;
    صحیح txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    اگر(ssl_ver == SSL3_نسخہ_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    ورنہ
      tls_rt_name = "";

    اگر(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      اگر(len) {
        پیغام_type = *(ثابت غیرعلامتی حرف *)buf;
        پیغام_name = "Change cipher spec";
      }
    }
    ورنہ اگر(content_type == SSL3_RT_ALERT) {
      اگر(len >= 2) {
        پیغام_type =
          (((ثابت غیرعلامتی حرف *)buf)[0] << 8) +
           ((ثابت غیرعلامتی حرف *)buf)[1];
        پیغام_name = SSL_alert_desc_string_long(پیغام_type);
      }
    }
    ورنہ اگر(len) {
      پیغام_type = *(ثابت غیرعلامتی حرف *)buf;
      پیغام_name = ssl_پیغام_type(ssl_ver, پیغام_type);
    }

    txt_len = curl_msnprintf(ssl_buf, حجم(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, پیغام_name, پیغام_type);
    Curl_debug(ڈیٹا, CURLINFO_متن, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(ڈیٹا, (direction == 1) ? CURLINFO_SSL_ڈیٹا_برآمد :
             CURLINFO_SSL_ڈیٹا_اندر, (ثابت حرف *)buf, len);
  (خالی)ssl;
}

ساکن CURLcode ossl_سیٹ_ssl_نسخہ_min_max(ساخت Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             غیرعلامتی صحیح ssl_نسخہ_min)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  طویل curl_ssl_نسخہ_min = (طویل)ssl_نسخہ_min;
  طویل curl_ssl_نسخہ_max;

  /* convert curl min SSL version option to OpenSSL constant */
#اگر defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_نسخہ_نمبر)
  uint16_t ossl_ssl_نسخہ_min = 0;
  uint16_t ossl_ssl_نسخہ_max = 0;
#ورنہ
  طویل ossl_ssl_نسخہ_min = 0;
  طویل ossl_ssl_نسخہ_max = 0;
#اگر_ختم
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_نسخہ_min != CURL_SSLVERSION_DEFAULT);
  انتخاب(curl_ssl_نسخہ_min) {
  حالت CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  حالت CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_نسخہ_min = TLS1_نسخہ;
    توڑو;
  حالت CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_نسخہ_min = TLS1_1_نسخہ;
    توڑو;
  حالت CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_نسخہ_min = TLS1_2_نسخہ;
    توڑو;
  حالت CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_نسخہ_min = TLS1_3_نسخہ;
    توڑو;
  }

  /* ... then, TLS max version */
  curl_ssl_نسخہ_max = (طویل)conn_config->نسخہ_max;

  /* convert curl max SSL version option to OpenSSL constant */
  انتخاب(curl_ssl_نسخہ_max) {
  حالت CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_نسخہ_max = TLS1_نسخہ;
    توڑو;
  حالت CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_نسخہ_max = TLS1_1_نسخہ;
    توڑو;
  حالت CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_نسخہ_max = TLS1_2_نسخہ;
    توڑو;
  حالت CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_نسخہ_max = TLS1_3_نسخہ;
    توڑو;
  حالت CURL_SSLVERSION_MAX_NONE:  /* none selected */
  حالت CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  طےشدہ:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_نسخہ_max = 0;
    توڑو;
  }

  اگر(!SSL_CTX_سیٹ_min_proto_نسخہ(ctx, ossl_ssl_نسخہ_min) ||
     !SSL_CTX_سیٹ_max_proto_نسخہ(ctx, ossl_ssl_نسخہ_max))
    واپس CURLE_SSL_CONNECT_ERROR;

  واپس CURLE_OK;
}

CURLcode Curl_ossl_جوڑو_session(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت ossl_ctx *octx,
                               ثابت حرف *ssl_peer_کلید,
                               SSL_SESSION *session,
                               ثابت حرف *alpn,
                               غیرعلامتی حرف *quic_tp,
                               size_t quic_tp_len,
                               ساخت Curl_ssl_session **psession)
{
  ساخت Curl_ssl_session *sc_session = تہی, *sc_dup = تہی;
  غیرعلامتی حرف *der_session_buf = تہی;
  غیرعلامتی حرف *qtp_clone = تہی;
  CURLcode نتیجہ = CURLE_OK;

  اگر(psession)
    *psession = تہی;
  اگر(!cf || !ڈیٹا)
    جاؤ برآمد;

  اگر(Curl_ssl_scache_use(cf, ڈیٹا)) {
    size_t der_session_سائز;
    غیرعلامتی حرف *der_session_ptr;
    size_t earlydata_max = 0;
    صحیح ietf_tls_id = SSL_نسخہ(octx->ssl);

    der_session_سائز = i2d_SSL_SESSION(session, تہی);
    اگر(der_session_سائز == 0) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_سائز);
    اگر(!der_session_buf) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }

    der_session_سائز = i2d_SSL_SESSION(session, &der_session_ptr);
    اگر(der_session_سائز == 0) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }

#اگر_تعریف_شدہ HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_ڈیٹا(session);
#اگر_ختم
    اگر(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ثابت حرف *)quic_tp, quic_tp_len);
      اگر(!qtp_clone) {
        نتیجہ = CURLE_برآمد_کا_حافظہ;
        جاؤ برآمد;
      }
    }

    نتیجہ = Curl_ssl_session_create2(der_session_buf, der_session_سائز,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)وقت(تہی) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = تہی;  /* took ownership of sdata */
#اگر_تعریف_شدہ USE_APPLE_SECTRUST
    اگر(!نتیجہ)
      sc_session->sectrust_verified = octx->sectrust_verified;
#اگر_ختم
    اگر(!نتیجہ && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        نتیجہ = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    اگر(!نتیجہ) {
      نتیجہ = Curl_ssl_scache_put(cf, ڈیٹا, ssl_peer_کلید, sc_session);
      /* took ownership of `sc_session` */
      sc_session = تہی;
    }
  }

برآمد:
  curlx_free(der_session_buf);
  اگر(!نتیجہ && psession) {
    *psession = sc_dup;
    sc_dup = تہی;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  واپس نتیجہ;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ساکن صحیح ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ساخت Curl_cfilter *cf = (ساخت Curl_cfilter *)SSL_get_app_ڈیٹا(ssl);
  اگر(cf) {
    ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
    ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
    ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
    ساخت Curl_ssl_session *session = تہی;
    Curl_ossl_جوڑو_session(cf, ڈیٹا, octx, connssl->peer.scache_کلید,
                          ssl_sessionid, connssl->negotiated.alpn, تہی,
                          0, &session);
    اگر(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  واپس 0;
}

ساکن CURLcode بھرو_cacert_from_حافظہ(X509_STORE *store,
                                        ثابت ساخت curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = تہی;
  ڈھیر_کا(X509_INFO) *inf = تہی;

  /* everything else is a reference */
  صحیح i, گنتی = 0;
  X509_INFO *itmp = تہی;

  اگر(ca_info_blob->len > (size_t)زیادہ_سے_زیادہ_عدد)
    واپس CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->ڈیٹا, (صحیح)ca_info_blob->len);
  اگر(!cbio)
    واپس CURLE_برآمد_کا_حافظہ;

  inf = PEM_X509_INFO_read_bio(cbio, تہی, تہی, تہی);
  اگر(!inf) {
    BIO_free(cbio);
    واپس CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  برائے(i = 0; i < (صحیح)sk_X509_INFO_عدد(inf); ++i) {
    itmp = sk_X509_INFO_قدر(inf, (ossl_valsize_t)i);
    اگر(itmp->x509) {
      اگر(X509_STORE_جوڑو_cert(store, itmp->x509)) {
        ++گنتی;
      }
      ورنہ {
        /* set count to 0 to return an error */
        گنتی = 0;
        توڑو;
      }
    }
    اگر(itmp->crl) {
      اگر(X509_STORE_جوڑو_crl(store, itmp->crl)) {
        ++گنتی;
      }
      ورنہ {
        /* set count to 0 to return an error */
        گنتی = 0;
        توڑو;
      }
    }
  }

#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic ڈالو
#ہدایت clang diagnostic ignored "-Wcast-function-type-strict"
#اگر_ختم
  sk_X509_INFO_نکالو_free(inf, X509_INFO_free);
#اگر defined(__clang__) && __clang_major__ >= 16
#ہدایت clang diagnostic نکالو
#اگر_ختم
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  واپس (گنتی > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#اگر_تعریف_شدہ USE_WIN32_CRYPTO
ساکن CURLcode ossl_win_بھرو_store(ساخت Curl_easy *ڈیٹا,
                                    ساخت Curl_cfilter *cf,
                                    ثابت حرف *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode نتیجہ = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  اگر(hStore) {
    PCCERT_CONTEXT pContext = تہی;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = تہی;
    DWORD enhkey_usage_سائز = 0;
    VERBOSE(size_t کل = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    نتیجہ = CURLE_OK;
    برائے(;;) {
      X509 *x509;
      FILETIME now;
      BYTE کلید_usage[2];
      DWORD req_سائز;
      ثابت غیرعلامتی حرف *encoded_cert;
      pContext = Cert_Enum_Certificates_اندر_Store(hStore, pContext);
      اگر(!pContext)
        توڑو;

      VERBOSE(++کل);

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        حرف cert_name[256];
        اگر(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               تہی, cert_name, حجم(cert_name)))
          infof(ڈیٹا, "SSL: unknown cert name");
        ورنہ
          infof(ڈیٹا, "SSL: Checking cert \"%s\"", cert_name);
      }
#اگر_ختم
      encoded_cert = (ثابت غیرعلامتی حرف *)pContext->pbCertEncoded;
      اگر(!encoded_cert)
        جاری;

      Get_System_وقت_As_File_وقت(&now);
      اگر(Compare_File_وقت(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         Compare_File_وقت(&now, &pContext->pCertInfo->NotAfter) > 0)
        جاری;

      /* If key usage exists check for signing attribute */
      اگر(Cert_Get_Intended_کلید_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 کلید_usage, حجم(کلید_usage))) {
        اگر(!(کلید_usage[0] & CERT_کلید_CERT_SIGN_کلید_USAGE))
          جاری;
      }
      ورنہ اگر(Get_آخری_Error())
        جاری;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      اگر(Cert_Get_Enhanced_کلید_Usage(pContext, 0, تہی, &req_سائز) && req_سائز) {
        اگر(req_سائز > enhkey_usage_سائز) {
          خالی *tmp = curlx_realloc(enhkey_usage, req_سائز);

          اگر(!tmp) {
            failf(ڈیٹا, "SSL: Out of memory allocating for OID list");
            نتیجہ = CURLE_برآمد_کا_حافظہ;
            توڑو;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_سائز = req_سائز;
        }

        اگر(Cert_Get_Enhanced_کلید_Usage(pContext, 0, enhkey_usage, &req_سائز)) {
          اگر(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            اگر((HRESULT)Get_آخری_Error() != CRYPT_E_NOT_ملا)
              جاری;
          }
          ورنہ {
            DWORD i;
            bool ملا = FALSE;

            برائے(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              اگر(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                ملا = TRUE;
                توڑو;
              }
            }

            اگر(!ملا)
              جاری;
          }
        }
        ورنہ
          جاری;
      }
      ورنہ
        جاری;

      x509 = d2i_X509(تہی, &encoded_cert, (طویل)pContext->cbCertEncoded);
      اگر(!x509)
        جاری;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      اگر(X509_STORE_جوڑو_cert(store, x509) == 1) {
        VERBOSE(++imported);
#اگر_تعریف_شدہ DEBUGBUILD
        infof(ڈیٹا, "SSL: Imported cert");
#اگر_ختم
        *padded = TRUE;
      }
      X509_free(x509);
    }

    curlx_free(enhkey_usage);
    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    CURL_TRC_CF(ڈیٹا, cf,
                "ossl_win_load_store() found: %zu imported: %zu certs in %s.",
                کل, imported, win_store);

    اگر(نتیجہ)
      واپس نتیجہ;
  }

  واپس نتیجہ;
}

ساکن CURLcode ossl_windows_بھرو_anchors(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  ساکن ثابت حرف * ثابت win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode نتیجہ = CURLE_OK;

  *padded = FALSE;
  برائے(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    نتیجہ = ossl_win_بھرو_store(ڈیٹا, cf, win_stores[i], store, &store_added);
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(store_added) {
      CURL_TRC_CF(ڈیٹا, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    ورنہ
      infof(ڈیٹا, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  واپس نتیجہ;
}

#اگر_ختم /* USE_WIN32_CRYPTO */

ساکن CURLcode ossl_بھرو_trust_anchors(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت ossl_ctx *octx,
                                        X509_STORE *store)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? تہی : conn_config->CAfile);
  ثابت حرف * ثابت ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_خالی_خانے = TRUE;
  اگر(conn_config->native_ca_store) {
#اگر_تعریف_شدہ USE_WIN32_CRYPTO
    bool added = FALSE;
    نتیجہ = ossl_windows_بھرو_anchors(cf, ڈیٹا, store, &added);
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(added) {
      infof(ڈیٹا, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_خالی_خانے = FALSE;
    }
#وگرنہ_اگر defined(USE_APPLE_SECTRUST)
    infof(ڈیٹا, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#اگر_ختم
  }

  اگر(conn_config->ca_info_blob) {
    نتیجہ = بھرو_cacert_from_حافظہ(store, conn_config->ca_info_blob);
    اگر(نتیجہ) {
      failf(ڈیٹا, "error adding trust anchors from certificate blob: %d",
            (صحیح)نتیجہ);
      واپس نتیجہ;
    }
    infof(ڈیٹا, "  CA Blob from configuration");
    octx->store_is_خالی_خانے = FALSE;
  }

  اگر(ssl_cafile || ssl_capath) {
#اگر_تعریف_شدہ HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    اگر(ssl_cafile) {
      اگر(!X509_STORE_بھرو_file(store, ssl_cafile)) {
        اگر(octx->store_is_خالی_خانے && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(ڈیٹا, "error adding trust anchors from file: %s", ssl_cafile);
          واپس CURLE_SSL_CACERT_BADFILE;
        }
        ورنہ
          infof(ڈیٹا, "error setting certificate file, continuing anyway");
      }
      infof(ڈیٹا, "  CAfile: %s", ssl_cafile);
      octx->store_is_خالی_خانے = FALSE;
    }
    اگر(ssl_capath) {
      اگر(!X509_STORE_بھرو_راستہ(store, ssl_capath)) {
        اگر(octx->store_is_خالی_خانے && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(ڈیٹا, "error adding trust anchors from path: %s", ssl_capath);
          واپس CURLE_SSL_CACERT_BADFILE;
        }
        ورنہ
          infof(ڈیٹا, "error setting certificate path, continuing anyway");
      }
      infof(ڈیٹا, "  CApath: %s", ssl_capath);
      octx->store_is_خالی_خانے = FALSE;
    }
#ورنہ
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    اگر(!X509_STORE_بھرو_locations(store, ssl_cafile, ssl_capath)) {
      اگر(octx->store_is_خالی_خانے && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(ڈیٹا, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        واپس CURLE_SSL_CACERT_BADFILE;
      }
      ورنہ {
        infof(ڈیٹا, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    اگر(ssl_cafile)
      infof(ڈیٹا, "  CAfile: %s", ssl_cafile);
    اگر(ssl_capath)
      infof(ڈیٹا, "  CApath: %s", ssl_capath);
    octx->store_is_خالی_خانے = FALSE;
#اگر_ختم
  }

#اگر_تعریف_شدہ CURL_CA_FALLBACK
  اگر(octx->store_is_خالی_خانے) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_سیٹ_default_paths(store);
    infof(ڈیٹا, "  OpenSSL default paths (fallback)");
    octx->store_is_خالی_خانے = FALSE;
  }
#اگر_ختم
  اگر(octx->store_is_خالی_خانے && !have_native_check)
    infof(ڈیٹا, "  no trust anchors configured");

  واپس نتیجہ;
}

ساکن CURLcode ossl_populate_x509_store(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         ساخت ossl_ctx *octx,
                                         X509_STORE *store)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_OK;
  X509_LOOKUP *lookup = تہی;
  ثابت حرف * ثابت ssl_crlfile = conn_config->CRLfile;
  غیرعلامتی طویل x509flags = 0;

  CURL_TRC_CF(ڈیٹا, cf, "configuring OpenSSL's x509 trust store");
  اگر(!store)
    واپس CURLE_برآمد_کا_حافظہ;

  اگر(!conn_config->verifypeer) {
    infof(ڈیٹا, "SSL Trust: peer verification disabled");
    واپس CURLE_OK;
  }

  infof(ڈیٹا, "SSL Trust Anchors:");
  نتیجہ = ossl_بھرو_trust_anchors(cf, ڈیٹا, octx, store);
  اگر(نتیجہ)
    واپس نتیجہ;

  /* Does not make sense to load a CRL file without peer verification */
#اگر_تعریف_شدہ USE_APPLE_SECTRUST
  اگر(ssl_crlfile && conn_config->native_ca_store) {
    failf(ڈیٹا, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    واپس CURLE_NOT_BUILT_اندر;
  }
#اگر_ختم
  اگر(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_جوڑو_lookup(store, X509_LOOKUP_file());
    اگر(!lookup ||
       (!X509_بھرو_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(ڈیٹا, "error loading CRL file: %s", ssl_crlfile);
      واپس CURLE_SSL_CRL_BADFILE;
    }
    x509flags = X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL;
    infof(ڈیٹا, " CRLfile: %s", ssl_crlfile);
  }

  /* Try building a chain using issuers in the trusted store first to avoid
     problems with server-sent legacy intermediates. Newer versions of
     OpenSSL do alternate chain checking by default but we do not know how to
     determine that in a reliable manner.
     https://web.archive.org/web/20190422050538/rt.openssl.org/Ticket/Display.html?id=3621
   */
  x509flags |= X509_V_FLAG_TRUSTED_پہلا;

  اگر(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (خالی)X509_STORE_سیٹ_flags(store, x509flags);

  واپس نتیجہ;
}

/* key to use at `multi->proto_hash` */
#تعریف_کرو MPROTO_OSSL_X509_کلید  "tls:ossl:x509:share"

ساخت ossl_x509_share {
  حرف *CAfile;         /* CAfile path used to generate X509 store */
  حرف *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  ساخت curltime وقت; /* when the cached store was created */
  ثنائی_ہندسہ(store_is_خالی_خانے);  /* no certs/paths/blobs are in the store */
  ثنائی_ہندسہ(no_partialchain); /* keep partial chain state */
};

ساکن خالی oss_x509_share_free(ثابت خالی *کلید, size_t کلید_len, خالی *p)
{
  ساخت ossl_x509_share *share = p;
  DEBUGASSERT(کلید_len == CURL_CSTRLEN(MPROTO_OSSL_X509_کلید));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_کلید, کلید, کلید_len));
  (خالی)کلید;
  (خالی)کلید_len;
  اگر(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

ساکن bool ossl_cached_x509_store_expired(ساخت Curl_easy *ڈیٹا,
                                           ثابت ساخت ossl_x509_share *mb)
{
  اگر(ڈیٹا->سیٹ.ssl_ca_cache_timeout < 0)
    واپس FALSE;
  ورنہ {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &mb->وقت);
    timediff_t timeout_ms = ڈیٹا->سیٹ.ssl_ca_cache_timeout * (timediff_t)1000;

    واپس elapsed_ms >= timeout_ms;
  }
}

ساکن bool ossl_cached_x509_store_different(ساخت Curl_cfilter *cf,
                                             ثابت ساخت Curl_easy *ڈیٹا,
                                             ثابت ساخت ossl_x509_share *mb)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (خالی)ڈیٹا;
  اگر(mb->no_partialchain != conn_config->no_partialchain)
    واپس TRUE;
  اگر(!mb->CAfile || !conn_config->CAfile) {
    اگر(mb->CAfile != conn_config->CAfile)
      واپس TRUE;
  }
  ورنہ اگر(strcmp(mb->CAfile, conn_config->CAfile))
    واپس TRUE;

  اگر(!mb->CApath || !conn_config->CApath) {
    اگر(mb->CApath != conn_config->CApath)
      واپس TRUE;
  }
  ورنہ اگر(strcmp(mb->CApath, conn_config->CApath))
    واپس TRUE;

  واپس FALSE;
}

ساکن X509_STORE *ossl_get_cached_x509_store(ساخت Curl_cfilter *cf,
                                              ساخت Curl_easy *ڈیٹا,
                                              bool *pempty)
{
  ساخت Curl_multi *multi = ڈیٹا->multi;
  ساخت ossl_x509_share *share;
  X509_STORE *store = تہی;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_کلید,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_کلید)) : تہی;
  اگر(share && share->store &&
     !ossl_cached_x509_store_expired(ڈیٹا, share) &&
     !ossl_cached_x509_store_different(cf, ڈیٹا, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_خالی_خانے;
  }

  واپس store;
}

ساکن خالی ossl_سیٹ_cached_x509_store(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       X509_STORE *store,
                                       bool is_خالی_خانے)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت Curl_multi *multi = ڈیٹا->multi;
  ساخت ossl_x509_share *share;

  DEBUGASSERT(multi);
  اگر(!multi)
    واپس;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_کلید,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_کلید));

  اگر(!share) {
    share = curlx_calloc(1, حجم(*share));
    اگر(!share)
      واپس;
    اگر(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_کلید,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_کلید),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      واپس;
    }
  }

  اگر(X509_STORE_اوپر_ref(store)) {
    حرف *CAfile = تہی;
    حرف *CApath = تہی;

    اگر(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      اگر(!CAfile) {
        X509_STORE_free(store);
        واپس;
      }
    }

    اگر(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      اگر(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        واپس;
      }
    }

    اگر(share->store) {
      X509_STORE_free(share->store);
      curlx_free(share->CAfile);
      curlx_free(share->CApath);
    }

    share->وقت = *Curl_pgrs_now(ڈیٹا);
    share->store = store;
    share->store_is_خالی_خانے = is_خالی_خانے;
    share->CAfile = CAfile;
    share->CApath = CApath;
    share->no_partialchain = conn_config->no_partialchain;
  }
}

#اگر_تعریف_شدہ OPENSSL_HAS_PROVIDERS
/* CApath certificates loaded under a provider libctx can outlive that libctx
   when the store is shared with the multi handle. */
#تعریف_کرو USING_PROVIDER(octx) octx->p.libctx
#ورنہ
#تعریف_کرو USING_PROVIDER(x) 0
#اگر_ختم

CURLcode Curl_ssl_تیاری_x509_store(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت ossl_ctx *octx)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_OK;
  X509_STORE *cached_store;
  bool cache_criteria_met, is_خالی_خانے;

  /* Consider the X509 store cacheable if it comes from CAfile/CApath,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (ڈیٹا->سیٹ.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !USING_PROVIDER(octx);

  ERR_سیٹ_mark();

  cached_store = ossl_get_cached_x509_store(cf, ڈیٹا, &is_خالی_خانے);
  اگر(cached_store && cache_criteria_met && X509_STORE_اوپر_ref(cached_store)) {
    SSL_CTX_سیٹ_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_خالی_خانے = is_خالی_خانے;
  }
  ورنہ {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    نتیجہ = ossl_populate_x509_store(cf, ڈیٹا, octx, store);
    اگر(نتیجہ == CURLE_OK && cache_criteria_met) {
      ossl_سیٹ_cached_x509_store(cf, ڈیٹا, store, (bool)octx->store_is_خالی_خانے);
    }
  }

  ERR_نکالو_مقصد_mark();

  واپس نتیجہ;
}

ساکن bool ossl_apply_session(
  ساخت ossl_ctx *octx,
  ساخت Curl_cfilter *cf,
  ساخت Curl_easy *ڈیٹا,
  ساخت alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  ساخت Curl_ssl_session *scs)
{
  ساخت ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ثابت غیرعلامتی حرف *der_sessionid = scs->sdata;
  size_t der_sessionid_سائز = scs->sdata_len;
  SSL_SESSION *ssl_session = تہی;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(تہی, &der_sessionid,
                                (طویل)der_sessionid_سائز);
  اگر(ssl_session) {
    اگر(!SSL_سیٹ_session(octx->ssl, ssl_session)) {
      VERBOSE(حرف error_بفر[256]);
      infof(ڈیٹا, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_بفر,
                          حجم(error_بفر)));
    }
    ورنہ {
      اگر(conn_cfg->verifypeer &&
         (SSL_get_verify_نتیجہ(octx->ssl) != X509_V_OK)
#اگر_تعریف_شدہ USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#اگر_ختم
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_سیٹ_session(octx->ssl, تہی);
        infof(ڈیٹا, "SSL session not peer verified, not reusing");
      }
      ورنہ {
        infof(ڈیٹا, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#اگر_تعریف_شدہ USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#اگر_ختم
        infof(ڈیٹا, "SSL verify result: %lx",
              (غیرعلامتی طویل)SSL_get_verify_نتیجہ(octx->ssl));
#اگر_تعریف_شدہ HAVE_OPENSSL_EARLYDATA
        اگر(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_ڈیٹا(ssl_session) &&
           !cf->conn->بٹ.connect_only &&
           (SSL_نسخہ(octx->ssl) == TLS1_3_نسخہ)) {
          bool do_early_ڈیٹا = FALSE;
          اگر(sess_reuse_cb)
            (خالی)sess_reuse_cb(cf, ڈیٹا, alpns, scs, &do_early_ڈیٹا);
          اگر(do_early_ڈیٹا) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_مقصد(alpns, scs->alpn);
          }
        }
#ورنہ
        (خالی)alpns;
        (خالی)conn_cfg;
        (خالی)sess_reuse_cb;
#اگر_ختم
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  ورنہ {
    infof(ڈیٹا, "SSL session not accepted by OpenSSL, continuing without");
  }
  واپس (bool)octx->reused_session;
}

ساکن CURLcode ossl_init_session_and_alpns(
  ساخت ossl_ctx *octx,
  ساخت Curl_cfilter *cf,
  ساخت Curl_easy *ڈیٹا,
  ساخت ssl_peer *peer,
  ثابت ساخت alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  ساخت ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ساخت alpn_spec alpns;
  CURLcode نتیجہ;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  اگر((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ساخت Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(ڈیٹا, cf->cft, FIRSTSOCKET);
    اگر(scs) {
      اگر(ossl_apply_session(octx, cf, ڈیٹا, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(ڈیٹا, cf, "applied SSL session from control connection");
    }
  }

  اگر(!octx->reused_session &&
     Curl_ssl_scache_use(cf, ڈیٹا) && !conn_cfg->verifystatus) {
    ساخت Curl_ssl_session *scs = تہی;

    نتیجہ = Curl_ssl_scache_take(cf, ڈیٹا, peer->scache_کلید, &scs);
    اگر(!نتیجہ && scs && scs->sdata && scs->sdata_len) {
      (خالی)ossl_apply_session(octx, cf, ڈیٹا, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, ڈیٹا, peer->scache_کلید, scs);
  }

  اگر(alpns.گنتی) {
    ساخت alpn_proto_buf proto;
    memset(&proto, 0, حجم(proto));
    نتیجہ = Curl_alpn_مقصد_proto_buf(&proto, &alpns);
    اگر(نتیجہ) {
      failf(ڈیٹا, "Error determining ALPN");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
    اگر(SSL_سیٹ_alpn_protos(octx->ssl, proto.ڈیٹا, proto.len)) {
      failf(ڈیٹا, "Error setting ALPN");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
  }

  واپس CURLE_OK;
}

#اگر_تعریف_شدہ HAVE_SSL_SET1_ECH_CONFIG_LIST
ساکن CURLcode ossl_init_ech(ساخت ossl_ctx *octx,
                              ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              ساخت ssl_peer *peer)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحیح trying_ech_now = 0;

  اگر(!Curl_ssl_ech_enabled(cf))
    واپس CURLE_OK;

  اگر(conn_config->ech == CURLECH_GREASE) {
    infof(ڈیٹا, "ECH: will GREASE ClientHello");
#اگر_تعریف_شدہ HAVE_BORINGSSL_LIKE
    SSL_سیٹ_enable_ech_grease(octx->ssl, 1);
#ورنہ
    SSL_سیٹ_options(octx->ssl, SSL_OP_ECH_GREASE);
#اگر_ختم
  }
  ورنہ اگر(conn_config->ech && conn_config->ech_config) {
#اگر_تعریف_شدہ HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode نتیجہ;

    اگر(!conn_config->ech_config[0]) {
      infof(ڈیٹا, "ECH: ECHConfig from command line empty");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * لڑی_لمبائی(conn_config->ech_config);
    نتیجہ = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    اگر(نتیجہ || !ech_config) {
      infof(ڈیٹا, "ECH: cannot base64 decode ECHConfig from command line");
      اگر(conn_config->ech == CURLECH_HARD)
        واپس نتیجہ;
    }
    اگر(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(ڈیٹا, "ECH: SSL_ECH_set1_ech_config_list failed");
      اگر(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        واپس CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#ورنہ
    size_t ech_config_len = 0;
    اگر(!conn_config->ech_config[0]) {
      infof(ڈیٹا, "ECH: ECHConfig from command line empty");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = لڑی_لمبائی(conn_config->ech_config);
    اگر(SSL_set1_ech_config_list(octx->ssl,
                                (ثابت uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(ڈیٹا, "ECH: SSL_ECH_set1_ech_config_list failed");
      اگر(conn_config->ech == CURLECH_HARD)
        واپس CURLE_SSL_CONNECT_ERROR;
    }
    ورنہ
      trying_ech_now = 1;
#اگر_ختم /* HAVE_BORINGSSL_LIKE */
    infof(ڈیٹا, "ECH: ECHConfig from command line");
  }
  ورنہ {
    ثابت ساخت Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(ڈیٹا, cf->sockindex, peer->origin);

    اگر(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ثابت غیرعلامتی حرف *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(ڈیٹا, "ECH: ECHConfig from HTTPS RR");
      اگر(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(ڈیٹا, "ECH: SSL_set1_ech_config_list failed");
        اگر(conn_config->ech == CURLECH_HARD)
          واپس CURLE_SSL_CONNECT_ERROR;
      }
      ورنہ {
        trying_ech_now = 1;
        infof(ڈیٹا, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    ورنہ {
      infof(ڈیٹا, "ECH: requested but no ECHConfig available");
      اگر(conn_config->ech == CURLECH_HARD)
        واپس CURLE_SSL_CONNECT_ERROR;
    }
  }
#اگر_تعریف_شدہ HAVE_BORINGSSL_LIKE
  (خالی)peer;
  اگر(trying_ech_now && conn_config->ech_عوامی) {
    infof(ڈیٹا, "ECH: setting public_name not supported with BoringSSL");
    واپس CURLE_SSL_CONNECT_ERROR;
  }
#ورنہ
  اگر(trying_ech_now && conn_config->ech_عوامی) {
    صحیح ret;
    infof(ڈیٹا, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_عوامی);
    ret = SSL_ech_set1_server_نام(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_عوامی,
                                    0 /* do send outer */);
    اگر(ret != 1) {
      infof(ڈیٹا, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      واپس CURLE_SSL_CONNECT_ERROR;
    }
  }
#اگر_ختم /* HAVE_BORINGSSL_LIKE */
  اگر(trying_ech_now &&
     SSL_سیٹ_min_proto_نسخہ(octx->ssl, TLS1_3_نسخہ) != 1) {
    infof(ڈیٹا, "ECH: cannot force TLSv1.3 [ERROR]");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  واپس CURLE_OK;
}
#اگر_ختم /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

ساکن CURLcode ossl_init_ssl(ساخت ossl_ctx *octx,
                              ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              ساخت ssl_peer *peer,
                              ثابت ساخت alpn_spec *alpns_requested,
                              خالی *ssl_user_ڈیٹا,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  اگر(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  اگر(!octx->ssl) {
    failf(ڈیٹا, "SSL: could not create a context (handle)");
    واپس CURLE_برآمد_کا_حافظہ;
  }

  SSL_سیٹ_app_ڈیٹا(octx->ssl, ssl_user_ڈیٹا);

#اگر_غیر_تعریف_شدہ OPENSSL_NO_OCSP
  اگر(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_سیٹ_tlsext_حالت_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#اگر_ختم

  SSL_سیٹ_connect_state(octx->ssl);

  اگر(peer->sni) {
    اگر(!SSL_سیٹ_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(ڈیٹا, "Failed set SNI");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
  }

#اگر_تعریف_شدہ HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode نتیجہ = ossl_init_ech(octx, cf, ڈیٹا, peer);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
#اگر_ختم /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  واپس ossl_init_session_and_alpns(octx, cf, ڈیٹا, peer,
                                     alpns_requested, sess_reuse_cb);
}

ساکن CURLcode ossl_init_طریقہ(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت ssl_peer *peer,
                                 ثابت SSL_طریقہ **pmethod,
                                 غیرعلامتی صحیح *pssl_نسخہ_min)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = تہی;
  *pssl_نسخہ_min = conn_config->نسخہ_2;
  DEBUGASSERT(conn_config->نسخہ_2 != CURL_SSLVERSION_DEFAULT);
  انتخاب(peer->transport) {
  حالت TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    انتخاب(*pssl_نسخہ_min) {
    حالت CURL_SSLVERSION_TLSv1:
    حالت CURL_SSLVERSION_TLSv1_0:
    حالت CURL_SSLVERSION_TLSv1_1:
    حالت CURL_SSLVERSION_TLSv1_2:
    حالت CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_طریقہ();
      توڑو;
    حالت CURL_SSLVERSION_SSLv2:
      failf(ڈیٹا, "No SSLv2 support");
      واپس CURLE_NOT_BUILT_اندر;
    حالت CURL_SSLVERSION_SSLv3:
      failf(ڈیٹا, "No SSLv3 support");
      واپس CURLE_NOT_BUILT_اندر;
    طےشدہ:
      failf(ڈیٹا, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
    توڑو;
  حالت TRNSPRT_QUIC:
    *pssl_نسخہ_min = CURL_SSLVERSION_TLSv1_3;
    اگر(conn_config->نسخہ_max &&
       (conn_config->نسخہ_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->نسخہ_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(ڈیٹا, "QUIC needs at least TLS version 1.3");
      واپس CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_طریقہ();
    توڑو;
  طےشدہ:
    failf(ڈیٹا, "unsupported transport %d in SSL init", peer->transport);
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  واپس *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(ساخت ossl_ctx *octx,
                            ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت ssl_peer *peer,
                            ثابت ساخت alpn_spec *alpns_requested,
                            Curl_ossl_ctx_تیاری_cb *cb_تیاری,
                            خالی *cb_user_ڈیٹا,
                            Curl_ossl_new_session_cb *cb_new_session,
                            خالی *ssl_user_ڈیٹا,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ciphers;
  ثابت SSL_طریقہ *req_طریقہ = تہی;
  ctx_option_t ctx_options = 0;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ڈیٹا);
  حرف * ثابت ssl_cert = conn_config->clientcert;
  ثابت ساخت curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
  غیرعلامتی صحیح ssl_نسخہ_min = CURL_SSLVERSION_DEFAULT;
  حرف error_بفر[256];

  /* Make funny stuff to get random input */
  نتیجہ = ossl_بیج(ڈیٹا);
  اگر(!نتیجہ) {
    اگر(CURL_EASY_STR(ڈیٹا, STRING_SSL_ENGINE))
      نتیجہ = ossl_سیٹ_engine(ڈیٹا, octx,
                               CURL_EASY_STR(ڈیٹا, STRING_SSL_ENGINE));
    اگر(!نتیجہ && ڈیٹا->سیٹ.engine_default)
      نتیجہ = ossl_سیٹ_engine_default(ڈیٹا, octx);
  }

  اگر(!نتیجہ) {
    ssl_config->certverifyresult = !X509_V_OK;
    نتیجہ = ossl_init_طریقہ(cf, ڈیٹا, peer, &req_طریقہ, &ssl_نسخہ_min);
  }
  اگر(نتیجہ)
    واپس نتیجہ;
  DEBUGASSERT(req_طریقہ);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#اگر_تعریف_شدہ OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_طریقہ):
#اگر_ختم
    SSL_CTX_new(req_طریقہ);

  اگر(!octx->ssl_ctx) {
    failf(ڈیٹا, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_بفر, حجم(error_بفر)));
    واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(cb_تیاری) {
    نتیجہ = cb_تیاری(cf, ڈیٹا, cb_user_ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(ڈیٹا->سیٹ.fdebug && ڈیٹا->سیٹ.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_سیٹ_پیغام_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_سیٹ_پیغام_callback_arg(octx->ssl_ctx, cf);
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
  اگر(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_داخل_کرو_خالی_خانے_FRAGMENTS;

  DEBUGASSERT(ssl_نسخہ_min != CURL_SSLVERSION_DEFAULT);
  انتخاب(ssl_نسخہ_min) {
  حالت CURL_SSLVERSION_SSLv2:
  حالت CURL_SSLVERSION_SSLv3:
    واپس CURLE_NOT_BUILT_اندر;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  حالت CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  حالت CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  حالت CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  حالت CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  حالت CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    نتیجہ = ossl_سیٹ_ssl_نسخہ_min_max(cf, octx->ssl_ctx, ssl_نسخہ_min);
    اگر(نتیجہ)
      واپس نتیجہ;
    توڑو;

  طےشدہ:
    failf(ڈیٹا, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  SSL_CTX_سیٹ_options(octx->ssl_ctx, ctx_options);
  SSL_CTX_سیٹ_read_ahead(octx->ssl_ctx, 1);

  /* Max TLS1.2 record size 0x4000 + 0x800.
     OpenSSL supports processing "jumbo TLS record" (8 TLS records) in one go
     for some algorithms, so match that here.
     Experimentation shows that a slightly larger buffer is needed
     to avoid short reads.

     However using a large buffer (8 packets) actually decreases performance.
     4 packets is better.
   */
#اگر_تعریف_شدہ HAVE_SSL_CTX_سیٹ_DEFAULT_READ_بفر_LEN
  SSL_CTX_سیٹ_default_read_بفر_len(octx->ssl_ctx, 0x401e * 4);
#اگر_ختم

  /* We do retry writes sometimes from another buffer address */
  SSL_CTX_سیٹ_منوال(octx->ssl_ctx, SSL_منوال_ACCEPT_MOVING_WRITE_بفر);

  ciphers = conn_config->cipher_list;
  اگر(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = تہی;
  اگر(ciphers && (ssl_نسخہ_min < CURL_SSLVERSION_TLSv1_3)) {
    اگر(!SSL_CTX_سیٹ_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(ڈیٹا, "failed setting cipher list: %s", ciphers);
      واپس CURLE_SSL_CIPHER;
    }
    infof(ڈیٹا, "Cipher selection: %s", ciphers);
  }

#اگر_تعریف_شدہ HAVE_SSL_CTX_سیٹ_CIPHERSUITES
  {
    ثابت حرف *ciphers13 = conn_config->cipher_list13;
    اگر(ciphers13 &&
       (!conn_config->نسخہ_max ||
        (conn_config->نسخہ_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->نسخہ_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      اگر(!SSL_CTX_سیٹ_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(ڈیٹا, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        واپس CURLE_SSL_CIPHER;
      }
      infof(ڈیٹا, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#اگر_ختم

  اگر(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    نتیجہ = client_cert(ڈیٹا, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->کلید, conn_config->کلید_blob,
                         conn_config->کلید_type,
                         conn_config->کلید_passwd);
    اگر(نتیجہ)
      /* failf() is already done in client_cert() */
      واپس نتیجہ;
  }

#اگر_تعریف_شدہ HAVE_SSL_CTX_سیٹ_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_سیٹ_post_handshake_auth(octx->ssl_ctx, 1);
#اگر_ختم

  {
    ثابت حرف *curves = conn_config->curves;
    اگر(curves) {
#اگر_تعریف_شدہ HAVE_BORINGSSL_LIKE
#تعریف_کرو OSSL_خم_CAST(x) (x)
#ورنہ
#تعریف_کرو OSSL_خم_CAST(x) (حرف *)CURL_UNCONST(x)
#اگر_ختم
      اگر(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_خم_CAST(curves))) {
        failf(ڈیٹا, "failed setting curves list: '%s'", curves);
        واپس CURLE_SSL_CIPHER;
      }
    }
  }

#اگر_تعریف_شدہ HAVE_SSL_CTX_SET1_SIGALGS
#تعریف_کرو OSSL_SIGALG_CAST(x) OSSL_خم_CAST(x)
  {
    ثابت حرف *signature_algorithms = conn_config->signature_algorithms;
    اگر(signature_algorithms) {
      اگر(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(ڈیٹا, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        واپس CURLE_SSL_CIPHER;
      }
    }
  }
#اگر_ختم

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_سیٹ_verify(octx->ssl_ctx, SSL_VERIFY_NONE, تہی);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#اگر !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  اگر(Curl_tls_keylog_enabled()) {
    SSL_CTX_سیٹ_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#اگر_ختم

  اگر(cb_new_session) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_سیٹ_session_cache_منوال(octx->ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_اندرونی);
    SSL_CTX_sess_سیٹ_new_cb(octx->ssl_ctx, cb_new_session);
  }

  /* give application a chance to interfere with SSL set up. */
  اگر(ڈیٹا->سیٹ.ssl_fsslctx) {
    ساخت Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    اگر(!octx->x509_store_تیاری) {
      نتیجہ = Curl_ssl_تیاری_x509_store(cf, ڈیٹا, octx);
      اگر(نتیجہ)
        واپس نتیجہ;
      octx->x509_store_تیاری = TRUE;
    }
    CURL_CBAPI_آغاز(&guard, ڈیٹا, easy_fsslctx);
    نتیجہ = (*ڈیٹا->سیٹ.ssl_fsslctx)(ڈیٹا, octx->ssl_ctx,
                                      ڈیٹا->سیٹ.ssl_fsslctxp);
    CURL_CBAPI_انتہا(&guard);
    اگر(نتیجہ) {
      failf(ڈیٹا, "error signaled by SSL ctx callback");
      واپس نتیجہ;
    }
  }

  واپس ossl_init_ssl(octx, cf, ڈیٹا, peer, alpns_requested,
                       ssl_user_ڈیٹا, sess_reuse_cb);
}

ساکن CURLcode ossl_on_session_reuse(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت alpn_spec *alpns,
                                      ساخت Curl_ssl_session *scs,
                                      bool *do_early_ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  واپس Curl_on_session_reuse(cf, ڈیٹا, alpns, scs, do_early_ڈیٹا,
                               connssl->earlydata_max);
}

خالی Curl_ossl_report_handshake(ساخت Curl_easy *ڈیٹا, ساخت ossl_ctx *octx)
{
#اگر_تعریف_شدہ CURLVERBOSE
  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    صحیح psigtype_nid = NID_undef;
    ثابت حرف *negotiated_گروہ_name = تہی;

#اگر_تعریف_شدہ HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#اگر OPENSSL_نسخہ_نمبر >= 0x30200000L
    negotiated_گروہ_name = SSL_get0_گروہ_name(octx->ssl);
#ورنہ
    negotiated_گروہ_name =
      OBJ_nid2sn(SSL_get_negotiated_گروہ(octx->ssl) & 0x0000FFFF);
#اگر_ختم
#اگر_ختم

    /* Informational message */
    infof(ڈیٹا, "SSL connection using %s / %s / %s / %s",
          SSL_get_نسخہ(octx->ssl),
          SSL_get_cipher(octx->ssl),
          negotiated_گروہ_name ? negotiated_گروہ_name : "[blank]",
          OBJ_nid2sn(psigtype_nid));
  }
#ورنہ
  (خالی)ڈیٹا;
  (خالی)octx;
#اگر_ختم /* CURLVERBOSE */
}

ساکن CURLcode ossl_connect_step1(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode نتیجہ;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  نتیجہ = Curl_ossl_ctx_init(octx, cf, ڈیٹا, &connssl->peer,
                              connssl->alpn, تہی, تہی,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  اگر(نتیجہ)
    واپس نتیجہ;

  octx->bio_طریقہ = ossl_bio_cf_طریقہ_create();
  اگر(!octx->bio_طریقہ)
    واپس CURLE_برآمد_کا_حافظہ;
  bio = BIO_new(octx->bio_طریقہ);
  اگر(!bio)
    واپس CURLE_برآمد_کا_حافظہ;

  BIO_سیٹ_ڈیٹا(bio, cf);
#اگر_تعریف_شدہ HAVE_SSL_SET0_WBIO
  /* with OpenSSL v1.1.1 we get an alternative to SSL_set_bio() that works
   * without backward compat quirks. Every call takes one reference, so we
   * up it and pass. SSL* then owns and frees it.
   * We check on the function in configure, since LibreSSL and friends
   * each have their own versions to add support for this. */
  BIO_اوپر_ref(bio);
  SSL_set0_rbio(octx->ssl, bio);
  SSL_set0_wbio(octx->ssl, bio);
#ورنہ
  SSL_سیٹ_bio(octx->ssl, bio, bio);
#اگر_ختم

  اگر(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ساخت alpn_proto_buf proto;
    memset(&proto, 0, حجم(proto));
    Curl_alpn_مقصد_proto_str(&proto, connssl->alpn);
    infof(ڈیٹا, VTLS_INFOF_ALPN_OFFER_1STR, proto.ڈیٹا);
  }

  connssl->connecting_state = ssl_connect_2;
  واپس CURLE_OK;
}

#اگر_تعریف_شدہ HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
ساکن صحیح ossl_trace_ech_retry_configs(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا, SSL *ssl,
                                        صحیح reason)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t rcl = 0;
  صحیح rv = 1;
#اگر_غیر_تعریف_شدہ HAVE_BORINGSSL_LIKE
  حرف *inner = تہی;
  uint8_t *rcs = تہی;
  حرف *outer = تہی;
#ورنہ
  ثابت حرف *inner = تہی;
  ثابت uint8_t *rcs = تہی;
  ثابت حرف *outer = تہی;
  size_t برآمد_name_len = 0;
  صحیح servername_type = 0;
#اگر_ختم
  NOVERBOSE((خالی)reason);

  /* nothing to trace if not doing ECH */
  اگر(!Curl_ssl_ech_enabled(cf))
    واپس rv;
#اگر_غیر_تعریف_شدہ HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#ورنہ
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (صحیح)rcl;
#اگر_ختم

  اگر(rv && rcs) {
    حرف *b64str = تہی;
    size_t blen = 0;

    نتیجہ = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    اگر(!نتیجہ && b64str) {
      infof(ڈیٹا, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#اگر_غیر_تعریف_شدہ HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_حالت(ssl, &inner, &outer);
      infof(ڈیٹا, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#ورنہ
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &برآمد_name_len);
      infof(ڈیٹا, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#اگر_ختم
    }
  }
  ورنہ
    infof(ڈیٹا, "ECH: no retry_configs (rv = %d)", rv);
#اگر_غیر_تعریف_شدہ HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#اگر_ختم
  واپس rv;
}

#اگر_ختم

ساکن CURLcode ossl_connect_step2(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  ساخت ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ڈیٹا);
  صحیح err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  اگر(!octx->x509_store_تیاری) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode نتیجہ = Curl_ssl_تیاری_x509_store(cf, ڈیٹا, octx);
    اگر(نتیجہ)
      واپس نتیجہ;
    octx->x509_store_تیاری = TRUE;
  }

#اگر !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  اگر(Curl_tls_keylog_enabled() && !octx->keylog_مکمل)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_مکمل);
#اگر_ختم

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  اگر(err != 1) {
    صحیح detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(ڈیٹا, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    اگر(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(ڈیٹا, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      واپس CURLE_AGAIN;
    }
    اگر(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(ڈیٹا, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      واپس CURLE_AGAIN;
    }
#اگر_تعریف_شدہ SSL_ERROR_WANT_ASYNC
    اگر(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(ڈیٹا, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      واپس CURLE_AGAIN;
    }
#اگر_ختم
#اگر_تعریف_شدہ SSL_ERROR_WANT_RETRY_VERIFY
    اگر(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(ڈیٹا, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(ڈیٹا, TRUE);
      واپس CURLE_AGAIN;
    }
#اگر_ختم
    ورنہ {
      /* untreated error */
      sslerr_t errdetail;
      حرف error_بفر[256] = "";
      CURLcode نتیجہ;
      طویل lerr;
      صحیح lib;
      صحیح reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      اگر((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#اگر_تعریف_شدہ SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#اگر_ختم
         )) {
        نتیجہ = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_نتیجہ(octx->ssl);
        اگر(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(ڈیٹا, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        ورنہ
          failf(ڈیٹا, "%s", "SSL certificate verification failed");
      }
#اگر_تعریف_شدہ SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      ورنہ اگر((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        نتیجہ = CURLE_SSL_CLIENTCERT;
        failf(ڈیٹا, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_بفر, حجم(error_بفر)));
      }
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SSL_SET1_ECH_CONFIG_LIST
      ورنہ اگر((lib == ERR_LIB_SSL) &&
#اگر_غیر_تعریف_شدہ HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#ورنہ
              (reason == SSL_R_ECH_REJECTED)) {
#اگر_ختم /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, ڈیٹا, octx->ssl, reason);

        نتیجہ = CURLE_ECH_REQUIRED;
        failf(ڈیٹا, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_بفر, حجم(error_بفر)));
      }
#اگر_ختم
      ورنہ {
        نتیجہ = CURLE_SSL_CONNECT_ERROR;
        failf(ڈیٹا, "TLS connect error: %s",
              ossl_strerror(errdetail, error_بفر, حجم(error_بفر)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      اگر(نتیجہ == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        حرف extramsg[80] = "";
        صحیح sockerr = SOCKERRNO;

        اگر(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, حجم(extramsg));
        failf(ڈیٹا, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_مقصد_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->پورٹ);
      }

      واپس نتیجہ;
    }
  }
  ورنہ {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(ڈیٹا, octx);

#اگر defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    اگر(Curl_ssl_ech_enabled(cf)) {
      ساخت ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      حرف *inner = تہی, *outer = تہی;
      صحیح rv;
      VERBOSE(ثابت حرف *حالت_2);

      rv = SSL_ech_get1_حالت(octx->ssl, &inner, &outer);
      انتخاب(rv) {
      حالت SSL_ECH_حالت_SUCCESS:
        VERBOSE(حالت_2 = "succeeded");
        توڑو;
      حالت SSL_ECH_حالت_GREASE_ECH:
        VERBOSE(حالت_2 = "sent GREASE, got retry-configs");
        توڑو;
      حالت SSL_ECH_حالت_GREASE:
        VERBOSE(حالت_2 = "sent GREASE");
        توڑو;
      حالت SSL_ECH_حالت_NOT_TRIED:
        VERBOSE(حالت_2 = "not attempted");
        توڑو;
      حالت SSL_ECH_حالت_NOT_CONFIGURED:
        VERBOSE(حالت_2 = "not configured");
        توڑو;
      حالت SSL_ECH_حالت_BACKEND:
        VERBOSE(حالت_2 = "backend (unexpected)");
        توڑو;
      حالت SSL_ECH_حالت_FAILED:
        VERBOSE(حالت_2 = "failed");
        توڑو;
      حالت SSL_ECH_حالت_BAD_CALL:
        VERBOSE(حالت_2 = "bad call (unexpected)");
        توڑو;
      حالت SSL_ECH_حالت_BAD_NAME: {
        اگر(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(حالت_2 = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_حالت_SUCCESS;
        }
        ورنہ {
          VERBOSE(حالت_2 = "bad name (unexpected)");
        }
        توڑو;
      }
      طےشدہ:
        VERBOSE(حالت_2 = "unexpected status");
        infof(ڈیٹا, "ECH: unexpected status %d", rv);
      }
      infof(ڈیٹا, "ECH: result '%s' (inner=%s, outer=%s)",
            (حالت_2 ? حالت_2 : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      اگر(rv == SSL_ECH_حالت_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, ڈیٹا, octx->ssl, 0);
      }
      اگر(rv != SSL_ECH_حالت_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(ڈیٹا, "ECH: ech-hard failed");
        واپس CURLE_SSL_CONNECT_ERROR;
      }
    }
    ورنہ {
      infof(ڈیٹا, "ECH: result: status is not attempted");
    }
#اگر_ختم /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    اگر(connssl->alpn) {
      ثابت غیرعلامتی حرف *neg_protocol;
      غیرعلامتی صحیح len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      واپس Curl_alpn_سیٹ_negotiated(cf, ڈیٹا, connssl, neg_protocol, len);
    }

    واپس CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
ساکن CURLcode ossl_pkp_pin_peer_pubkey(ساخت Curl_easy *ڈیٹا, X509 *cert,
                                         ثابت حرف *pinnedpubkey)
{
  /* Scratch */
  صحیح len1 = 0, len2 = 0;
  غیرعلامتی حرف *buff1 = تہی, *عارضی = تہی;

  /* Result is returned to caller */
  CURLcode نتیجہ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  اگر(!pinnedpubkey)
    واپس CURLE_OK;

  اگر(!cert)
    واپس نتیجہ;

  کرو {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), تہی);
    اگر(len1 < 1)
      توڑو; /* failed */

    buff1 = عارضی = curlx_malloc(len1);
    اگر(!buff1)
      توڑو; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &عارضی);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    اگر((len1 != len2) || !عارضی || ((عارضی - buff1) != len1))
      توڑو; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    نتیجہ = Curl_pin_peer_pubkey(ڈیٹا, pinnedpubkey, buff1, len1);
  } جب(0);

  اگر(buff1)
    curlx_free(buff1);

  واپس نتیجہ;
}

#اگر_تعریف_شدہ CURLVERBOSE
#اگر !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_نسخہ_نمبر) && LIBRESSL_نسخہ_نمبر < 0x3060000fL)
ساکن خالی infof_certstack(ساخت Curl_easy *ڈیٹا, ثابت SSL *ssl)
{
  ڈھیر_کا(X509) *certstack;
  طویل verify_نتیجہ;
  صحیح عدد_cert_سطحیں;
  صحیح cert_level;

  اگر(!Curl_trc_is_verbose(ڈیٹا))
    واپس;

  verify_نتیجہ = SSL_get_verify_نتیجہ(ssl);
  اگر(verify_نتیجہ != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  ورنہ
    certstack = SSL_get0_verified_chain(ssl);
  اگر(!certstack)
    واپس;
  عدد_cert_سطحیں = sk_X509_عدد(certstack);

  برائے(cert_level = 0; cert_level < عدد_cert_سطحیں; cert_level++) {
    حرف cert_algorithm[80] = "";
    حرف گروہ_name_final[80] = "";
    ثابت X509_ALGOR *palg_cert = تہی;
    ثابت ASN1_OBJECT *paobj_cert = تہی;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    صحیح کلید_بٹ;
    صحیح کلید_sec_بٹ;
    صحیح get_گروہ_name;
    ثابت حرف *type_name;

    current_cert = sk_X509_قدر(certstack, cert_level);
    اگر(!current_cert)
      جاری;

    current_pkey = X509_get0_pubkey(current_cert);
    اگر(!current_pkey)
      جاری;

    X509_get0_signature(تہی, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, تہی, تہی, palg_cert);
    OBJ_obj2txt(cert_algorithm, حجم(cert_algorithm), paobj_cert, 0);

    کلید_بٹ = EVP_PKEY_بٹ(current_pkey);
#اگر_غیر_تعریف_شدہ HAVE_OPENSSL3
#تعریف_کرو EVP_PKEY_get_security_بٹ EVP_PKEY_security_بٹ
#اگر_ختم
    کلید_sec_بٹ = EVP_PKEY_get_security_بٹ(current_pkey);
#اگر_تعریف_شدہ HAVE_OPENSSL3
    {
      حرف گروہ_name[80] = "";
      get_گروہ_name = EVP_PKEY_get_گروہ_name(current_pkey, گروہ_name,
                                               حجم(گروہ_name), تہی);
      curl_msnprintf(گروہ_name_final, حجم(گروہ_name_final), "/%s",
                     گروہ_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#ورنہ
    get_گروہ_name = 0;
    type_name = تہی;
#اگر_ختم

    infof(ڈیٹا, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_گروہ_name == 0 ? "" : گروہ_name_final,
          کلید_بٹ, کلید_sec_بٹ, cert_algorithm);
  }
}
#ورنہ
#تعریف_کرو infof_certstack(ڈیٹا, ssl)
#اگر_ختم
#اگر_ختم /* CURLVERBOSE */

ساکن CURLcode ossl_check_issuer(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  X509 *server_cert)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = تہی;
  BIO *fp = تہی;
  حرف err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode نتیجہ = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  اگر(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->ڈیٹا,
                         (صحیح)conn_config->issuercert_blob->len);
    اگر(!fp) {
      failf(ڈیٹا, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, حجم(err_buf)));
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }
  }
  ورنہ اگر(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    اگر(!fp) {
      failf(ڈیٹا, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, حجم(err_buf)));
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ برآمد;
    }

    اگر(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      اگر(verify_enabled)
        failf(ڈیٹا, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      نتیجہ = CURLE_SSL_ISSUER_ERROR;
      جاؤ برآمد;
    }
  }

  اگر(fp) {
    issuer = PEM_read_bio_X509(fp, تہی, ZERO_NULL, تہی);
    اگر(!issuer) {
      اگر(verify_enabled)
        failf(ڈیٹا, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      نتیجہ = CURLE_SSL_ISSUER_ERROR;
      جاؤ برآمد;
    }

    اگر(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      اگر(verify_enabled)
        failf(ڈیٹا, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      نتیجہ = CURLE_SSL_ISSUER_ERROR;
      جاؤ برآمد;
    }

    infof(ڈیٹا, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

برآمد:
  اگر(fp)
    BIO_free(fp);
  اگر(issuer)
    X509_free(issuer);
  واپس نتیجہ;
}

ساکن ثابت حرف *pinned(ساخت Curl_cfilter *cf,
                          ساخت Curl_easy *ڈیٹا)
{
  (خالی)cf;
  واپس
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#اگر_ختم
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY);
}

ساکن CURLcode ossl_check_pinned_کلید(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      X509 *server_cert)
{
  CURLcode نتیجہ = CURLE_OK;
  ثابت حرف *ptr = pinned(cf, ڈیٹا);
  اگر(ptr) {
    نتیجہ = ossl_pkp_pin_peer_pubkey(ڈیٹا, server_cert, ptr);
    اگر(نتیجہ)
      failf(ڈیٹا, "SSL: public key does not match pinned public key");
  }
  واپس نتیجہ;
}

#اگر_تعریف_شدہ CURLVERBOSE
#تعریف_کرو MAX_CERT_NAME_لمبائی 2048
ساکن CURLcode ossl_infof_cert(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                X509 *server_cert)
{
  BIO *mem = تہی;
  ساخت dynbuf dname;
  حرف err_buf[256] = "";
  حرف *buf;
  طویل len;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!Curl_trc_is_verbose(ڈیٹا))
    واپس CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_لمبائی);
  mem = BIO_new(BIO_s_mem());
  اگر(!mem) {
    failf(ڈیٹا, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, حجم(err_buf)));
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  infof(ڈیٹا, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  نتیجہ = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(ڈیٹا, "  subject: %s", نتیجہ ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_وقت_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_ڈیٹا(mem, (حرف **)&buf);
  infof(ڈیٹا, "  start date: %.*s", (صحیح)len, buf);
  (خالی)BIO_reset(mem);

  ASN1_وقت_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_ڈیٹا(mem, (حرف **)&buf);
  infof(ڈیٹا, "  expire date: %.*s", (صحیح)len, buf);
  (خالی)BIO_reset(mem);

  نتیجہ = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  اگر(نتیجہ) /* should be only fatal stuff like OOM */
    جاؤ برآمد;
  infof(ڈیٹا, "  issuer: %s", curlx_dyn_ptr(&dname));

برآمد:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  واپس نتیجہ;
}
#اگر_ختم /* CURLVERBOSE */

#اگر_تعریف_شدہ USE_APPLE_SECTRUST
ساخت ossl_certs_ctx {
  ڈھیر_کا(X509) *sk;
  size_t عدد_certs;
  غیرعلامتی حرف *آخری_der;
};

ساکن CURLcode ossl_chain_get_der(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   خالی *user_ڈیٹا,
                                   size_t i,
                                   غیرعلامتی حرف **pder,
                                   size_t *pder_len)
{
  ساخت ossl_certs_ctx *chain = user_ڈیٹا;
  X509 *cert;
  صحیح der_len;

  OPENSSL_free(chain->آخری_der);
  chain->آخری_der = تہی;

  (خالی)cf;
  (خالی)ڈیٹا;
  *pder_len = 0;
  *pder = تہی;

  اگر(i >= chain->عدد_certs)
    واپس CURLE_TOO_LARGE;
  cert = sk_X509_قدر(chain->sk, (صحیح)i);
  اگر(!cert)
    واپس CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  اگر(der_len < 0)
    واپس CURLE_FAILED_INIT;
  chain->آخری_der = *pder;
  *pder_len = (size_t)der_len;
  واپس CURLE_OK;
}

ساکن CURLcode ossl_apple_verify(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت ossl_ctx *octx,
                                  ساخت ssl_peer *peer)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت ossl_certs_ctx chain;
  CURLcode نتیجہ;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, حجم(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.عدد_certs = chain.sk ? sk_X509_عدد(chain.sk) : 0;

  اگر(!chain.عدد_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    اگر(!octx->reused_session) {
      failf(ڈیٹا, "SSL: could not get peer certificate chain");
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    }
    ورنہ {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(ڈیٹا, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      واپس CURLE_OK;
    }
  }
  ورنہ {
#اگر_تعریف_شدہ HAVE_BORINGSSL_LIKE
    ثابت uint8_t *ocsp_ڈیٹا = تہی;
#ورنہ
    غیرعلامتی حرف *ocsp_ڈیٹا = تہی;
#اگر_ختم
    طویل ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    اگر(!octx->reused_session)
      ocsp_len = (طویل)SSL_get_tlsext_حالت_ocsp_resp(octx->ssl, &ocsp_ڈیٹا);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    اگر(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_ڈیٹا = تہی;
    }

    نتیجہ = Curl_vtls_apple_verify(cf, ڈیٹا, peer, chain.عدد_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_ڈیٹا, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.آخری_der);
    chain.آخری_der = تہی;
  }
  octx->sectrust_verified = !نتیجہ;
  واپس نتیجہ;
}
#اگر_ختم /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت ossl_ctx *octx,
                                   ساخت ssl_peer *peer)
{
  ساخت connectdata *conn = cf->conn;
  ساخت ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ڈیٹا);
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_OK;
  طویل ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  اگر(ڈیٹا->سیٹ.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    نتیجہ = ossl_certchain(ڈیٹا, octx->ssl);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  اگر(!server_cert) {
    /* no verification at all, this maybe acceptable */
    اگر(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, ڈیٹا))
      جاؤ برآمد;

    failf(ڈیٹا, "SSL: could not get peer certificate");
    نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    جاؤ برآمد;
  }

#اگر_تعریف_شدہ CURLVERBOSE
  نتیجہ = ossl_infof_cert(cf, ڈیٹا, server_cert);
  اگر(نتیجہ)
    جاؤ برآمد;
  infof_certstack(ڈیٹا, octx->ssl);
#اگر_ختم

  اگر(conn_config->verifyhost) {
    نتیجہ = ossl_verifyhost(ڈیٹا, conn, peer, server_cert);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_نتیجہ(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(ڈیٹا, "OpenSSL verify result: %lx", (غیرعلامتی طویل)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  اگر(verified)
    infof(ڈیٹا, "SSL certificate verified via OpenSSL.");

#اگر_تعریف_شدہ USE_APPLE_SECTRUST
  اگر(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    نتیجہ = ossl_apple_verify(cf, ڈیٹا, octx, peer);
    اگر(نتیجہ && (نتیجہ != CURLE_PEER_FAILED_VERIFICATION))
      جاؤ برآمد; /* unexpected error */
    اگر(octx->sectrust_verified) {
      infof(ڈیٹا, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#اگر_ختم

  اگر(!verified) {
    /* no trust established, report the OpenSSL status */
    اگر(conn_config->verifypeer) {
      failf(ڈیٹا, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
      جاؤ برآمد;
    }
    infof(ڈیٹا, " SSL certificate verification failed, continuing anyway!");
  }

#اگر_غیر_تعریف_شدہ OPENSSL_NO_OCSP
  اگر(conn_config->verifystatus &&
#اگر_تعریف_شدہ USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, ڈیٹا)) &&
#اگر_ختم
     !octx->reused_session) {
    نتیجہ = verifystatus(cf, ڈیٹا, octx);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
#اگر_ختم

  نتیجہ = ossl_check_issuer(cf, ڈیٹا, server_cert);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = ossl_check_pinned_کلید(cf, ڈیٹا, server_cert);

برآمد:
  X509_free(server_cert);
  واپس نتیجہ;
}

ساکن CURLcode ossl_connect_step3(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  نتیجہ = Curl_ossl_check_peer_cert(cf, ڈیٹا, octx, &connssl->peer);
  اگر(نتیجہ)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, ڈیٹا, connssl->peer.scache_کلید);

  واپس نتیجہ;
}

#اگر_تعریف_شدہ HAVE_OPENSSL_EARLYDATA
ساکن CURLcode ossl_send_earlydata(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  ثابت غیرعلامتی حرف *buf;
  size_t blen, nwritten;
  صحیح rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_نتیجہ = CURLE_OK;
  جب(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_ڈیٹا(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(ڈیٹا, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    اگر(rc <= 0) {
      طویل sslerror;
      حرف error_بفر[256];
      صحیح err = SSL_get_error(octx->ssl, rc);

      انتخاب(err) {
      حالت SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        نتیجہ = CURLE_AGAIN;
        جاؤ برآمد;
      حالت SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        نتیجہ = CURLE_AGAIN;
        جاؤ برآمد;
      حالت SSL_ERROR_SYSCALL: {
        صحیح sockerr = SOCKERRNO;

        اگر(octx->io_نتیجہ == CURLE_AGAIN) {
          نتیجہ = CURLE_AGAIN;
          جاؤ برآمد;
        }
        sslerror = ERR_get_error();
        اگر(sslerror)
          ossl_strerror(sslerror, error_بفر, حجم(error_بفر));
        ورنہ اگر(sockerr)
          curlx_strerror(sockerr, error_بفر, حجم(error_بفر));
        ورنہ
          curl_msnprintf(error_بفر, حجم(error_بفر), "%s",
                         SSL_ERROR_مقصد_str(err));

        failf(ڈیٹا, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_بفر, sockerr);
        نتیجہ = CURLE_SEND_ERROR;
        جاؤ برآمد;
      }
      حالت SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(ڈیٹا, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_بفر, حجم(error_بفر)));
        نتیجہ = CURLE_SEND_ERROR;
        جاؤ برآمد;
      }
      طےشدہ:
        /* a true error */
        failf(ڈیٹا, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_مقصد_str(err), SOCKERRNO);
        نتیجہ = CURLE_SEND_ERROR;
        جاؤ برآمد;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(ڈیٹا, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
برآمد:
  واپس نتیجہ;
}
#اگر_ختم /* HAVE_OPENSSL_EARLYDATA */

ساکن CURLcode ossl_connect(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             bool *مکمل)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;

  /* check if the connection has already been established */
  اگر(ssl_connection_complete == connssl->state) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  اگر(connssl->connecting_state == ssl_connect_1) {
    اگر(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(ڈیٹا, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(ڈیٹا, cf, "need HTTPS-RR, delaying connect");
      واپس CURLE_OK;
    }
    CURL_TRC_CF(ڈیٹا, cf, "ossl_connect, step1");
    نتیجہ = ossl_connect_step1(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(ڈیٹا, cf, "ossl_connect, step2");
#اگر_تعریف_شدہ HAVE_OPENSSL_EARLYDATA
    اگر(connssl->earlydata_state == ssl_earlydata_await) {
      جاؤ برآمد;
    }
    ورنہ اگر(connssl->earlydata_state == ssl_earlydata_sending) {
      نتیجہ = ossl_send_earlydata(cf, ڈیٹا);
      اگر(نتیجہ)
        جاؤ برآمد;
      connssl->earlydata_state = ssl_earlydata_بھیجا;
    }
#اگر_ختم
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_بھیجا));

    نتیجہ = ossl_connect_step2(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  اگر(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(ڈیٹا, cf, "ossl_connect, step3");
    نتیجہ = ossl_connect_step3(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
    connssl->connecting_state = ssl_connect_مکمل;
#اگر_تعریف_شدہ HAVE_OPENSSL_EARLYDATA
    اگر(connssl->earlydata_state > ssl_earlydata_none) {
      ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_بھیجا);
      connssl->earlydata_state =
        (SSL_get_early_ڈیٹا_حالت(octx->ssl) == SSL_EARLY_ڈیٹا_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#اگر_ختم
  }

  اگر(connssl->connecting_state == ssl_connect_مکمل) {
    CURL_TRC_CF(ڈیٹا, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

برآمد:
  اگر(نتیجہ == CURLE_AGAIN) {
    *مکمل = FALSE;
    واپس CURLE_OK;
  }
  *مکمل = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  واپس نتیجہ;
}

ساکن bool ossl_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                              ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  (خالی)ڈیٹا;
  واپس (bool)connssl->داخلہ_pending;
}

ساکن CURLcode ossl_send(ساخت Curl_cfilter *cf,
                          ساخت Curl_easy *ڈیٹا,
                          ثابت خالی *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  صحیح err;
  حرف error_بفر[256];
  sslerr_t sslerror;
  صحیح memlen;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  صحیح nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)زیادہ_سے_زیادہ_عدد) ? زیادہ_سے_زیادہ_عدد : (صحیح)len;
  اگر(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    اگر(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  اگر(nwritten > 0) {
    اگر(nwritten > memlen) {
      failf(ڈیٹا, "SSL_write(len=%d) returned %d", memlen, nwritten);
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    *pnwritten = (size_t)nwritten;
  }
  ورنہ {
    err = SSL_get_error(octx->ssl, nwritten);

    انتخاب(err) {
    حالت SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      نتیجہ = CURLE_AGAIN;
      جاؤ برآمد;
    حالت SSL_ERROR_WANT_WRITE:
      نتیجہ = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      جاؤ برآمد;
    حالت SSL_ERROR_SYSCALL: {
      صحیح sockerr = SOCKERRNO;

      اگر(octx->io_نتیجہ == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        نتیجہ = CURLE_AGAIN;
        جاؤ برآمد;
      }
      sslerror = ERR_get_error();
      اگر(sslerror)
        ossl_strerror(sslerror, error_بفر, حجم(error_بفر));
      ورنہ اگر(sockerr)
        curlx_strerror(sockerr, error_بفر, حجم(error_بفر));
      ورنہ
        curl_msnprintf(error_بفر, حجم(error_بفر), "%s",
                       SSL_ERROR_مقصد_str(err));

      failf(ڈیٹا, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_بفر, sockerr);
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    حالت SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(ڈیٹا, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_بفر, حجم(error_بفر)));
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    طےشدہ:
      /* a true error */
      failf(ڈیٹا, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_مقصد_str(err), SOCKERRNO);
      نتیجہ = CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
  }

برآمد:
  واپس نتیجہ;
}

ساکن CURLcode ossl_recv(ساخت Curl_cfilter *cf,
                          ساخت Curl_easy *ڈیٹا,   /* transfer */
                          حرف *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  حرف error_بفر[256];
  غیرعلامتی طویل sslerror;
  صحیح buffsize;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  صحیح nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)زیادہ_سے_زیادہ_عدد) ? زیادہ_سے_زیادہ_عدد : (صحیح)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  اگر(nread > 0)
    *pnread = (size_t)nread;
  ورنہ {
    /* failed SSL_read */
    صحیح err = SSL_get_error(octx->ssl, nread);

    انتخاب(err) {
    حالت SSL_ERROR_NONE: /* this is not an error */
      توڑو;
    حالت SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      اگر(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(ڈیٹا, cf, "TLS close_notify");
      توڑو;
    حالت SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      نتیجہ = CURLE_AGAIN;
      جاؤ برآمد;
    حالت SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتیجہ = CURLE_AGAIN;
      جاؤ برآمد;
    طےشدہ:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      اگر(octx->io_نتیجہ == CURLE_AGAIN) {
        نتیجہ = CURLE_AGAIN;
        جاؤ برآمد;
      }
      sslerror = ERR_get_error();
      اگر((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
قطار*/
        صحیح sockerr = SOCKERRNO;
        اگر(sslerror)
          ossl_strerror(sslerror, error_بفر, حجم(error_بفر));
        ورنہ اگر(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_بفر, حجم(error_بفر));
        ورنہ
          curl_msnprintf(error_بفر, حجم(error_بفر), "%s",
                         SSL_ERROR_مقصد_str(err));
        failf(ڈیٹا, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_بفر, sockerr);
        نتیجہ = CURLE_RECV_ERROR;
        جاؤ برآمد;
      }
      ورنہ اگر(err == SSL_ERROR_SYSCALL) {
        اگر(octx->io_نتیجہ) {
          /* logging handling in underlying filter already */
          نتیجہ = octx->io_نتیجہ;
        }
        ورنہ اگر(connssl->peer_closed) {
          failf(ڈیٹا, "Connection closed abruptly");
          نتیجہ = CURLE_RECV_ERROR;
        }
        ورنہ {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          صحیح sockerr = SOCKERRNO;
          اگر(sockerr)
            curlx_strerror(sockerr, error_بفر, حجم(error_بفر));
          ورنہ {
            curl_msnprintf(error_بفر, حجم(error_بفر),
                           "Connection closed abruptly");
          }
          failf(ڈیٹا, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_بفر, sockerr);
          نتیجہ = CURLE_RECV_ERROR;
        }
        جاؤ برآمد;
      }
    }
  }

برآمد:
  اگر((!نتیجہ && !*pnread) || (نتیجہ == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->داخلہ_pending = FALSE;
  }
  CURL_TRC_CF(ڈیٹا, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (صحیح)نتیجہ, *pnread, connssl->داخلہ_pending);
  واپس نتیجہ;
}

ساکن CURLcode ossl_get_channel_binding(ساخت Curl_easy *ڈیٹا,
                                         int8_t sockindex,
                                         ساخت dynbuf *binding)
{
  X509 *cert;
  صحیح mdnid;
  bool no_digest_acceptable = FALSE;
  ثابت EVP_MD *algo_type = تہی;
  ثابت حرف *algo_name = تہی;
  غیرعلامتی صحیح لمبائی;
  غیرعلامتی حرف buf[EVP_MAX_MD_سائز];

  ساکن ثابت حرف سابقہ[] = "tls-server-end-point:";
  ساخت connectdata *conn = ڈیٹا->conn;
  ساخت Curl_cfilter *cf = conn->cfilter[sockindex];
  ساخت ossl_ctx *octx = تہی;
  CURLcode نتیجہ = CURLE_OK;

  کرو {
    ثابت ساخت Curl_cftype *cft = cf->cft;
    ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;

    اگر(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (ساخت ossl_ctx *)connssl->backend;
      توڑو;
    }

    cf = cf->اگلا;
  } جب(cf);

  اگر(!octx) {
    failf(ڈیٹا, "Failed to find the SSL filter");
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  اگر(!cert)
    /* No server certificate, do not do channel binding */
    واپس CURLE_OK;

#اگر_تعریف_شدہ HAVE_OPENSSL3
  {
    صحیح pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    اگر(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(ڈیٹا, "certificate signature algorithm not recognized");
      نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
      جاؤ برآمد;
    }

    اگر(mdnid != NID_undef) {
      اگر(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      ورنہ
        algo_type = EVP_get_digestbynid(mdnid);
    }
    ورنہ اگر(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    ورنہ اگر(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      حرف mdname[128] = "";
      صحیح rc = EVP_PKEY_get_default_digest_name(pkey, mdname, حجم(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      اگر(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      ورنہ اگر(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(ڈیٹا, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#ورنہ /* HAVE_OPENSSL3 */

  اگر(!OBJ_ڈھونڈو_sigid_algs(X509_get_signature_nid(cert), &mdnid, تہی)) {
    failf(ڈیٹا,
          "Unable to find digest NID for certificate signature algorithm");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ برآمد;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  اگر(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  ورنہ {
    algo_type = EVP_get_digestbynid(mdnid);
    اگر(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(ڈیٹا, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
      جاؤ برآمد;
    }
  }

#اگر_ختم /* HAVE_OPENSSL3, else */

  اگر(!algo_type) {
    اگر(no_digest_acceptable) {
      infof(ڈیٹا, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      نتیجہ = CURLE_OK;
      جاؤ برآمد;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(ڈیٹا, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ برآمد;
  }

  اگر(!X509_digest(cert, algo_type, buf, &لمبائی)) {
    failf(ڈیٹا, "X509_digest() failed for channel binding");
    نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    جاؤ برآمد;
  }

  /* Append "tls-server-end-point:" */
  نتیجہ = curlx_dyn_addn(binding, سابقہ, CURL_CSTRLEN(سابقہ));
  اگر(نتیجہ)
    جاؤ برآمد;

  /* Append digest */
  نتیجہ = curlx_dyn_addn(binding, buf, لمبائی);

برآمد:
  X509_free(cert);
  واپس نتیجہ;
}

size_t Curl_ossl_نسخہ(حرف *بفر, size_t سائز)
{
#اگر_تعریف_شدہ LIBRESSL_نسخہ_نمبر
  حرف *p;
  size_t گنتی;
  ثابت حرف *ver = Open_SSL_نسخہ(OPENSSL_نسخہ);
  ساکن ثابت حرف expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  اگر(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  گنتی = curl_msnprintf(بفر, سائز, "%s/%s", OSSL_PACKAGE, ver);
  برائے(p = بفر; *p; ++p) {
    اگر(ISBLANK(*p))
      *p = '_';
  }
  واپس گنتی;
#وگرنہ_اگر defined(OPENSSL_IS_AWSLC)
  واپس curl_msnprintf(بفر, سائز, "%s/%s",
                        OSSL_PACKAGE, AWSLC_نسخہ_نمبر_STRING);
#وگرنہ_اگر defined(OPENSSL_IS_BORINGSSL)
#اگر_تعریف_شدہ CURL_BORINGSSL_نسخہ
  واپس curl_msnprintf(بفر, سائز, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_نسخہ);
#ورنہ
  واپس curl_msnprintf(بفر, سائز, OSSL_PACKAGE);
#اگر_ختم
#ورنہ /* OpenSSL 3+ */
  واپس curl_msnprintf(بفر, سائز, "%s/%s",
                        OSSL_PACKAGE, Open_SSL_نسخہ(OPENSSL_نسخہ_STRING));
#اگر_ختم
}

/* can be called with data == NULL */
ساکن CURLcode ossl_random(ساخت Curl_easy *ڈیٹا,
                            غیرعلامتی حرف *entropy, size_t لمبائی)
{
  صحیح rc;
  اگر(ڈیٹا) {
    اگر(ossl_بیج(ڈیٹا)) /* Initiate the seed if not already done */
      واپس CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  ورنہ {
    اگر(!rand_enough())
      واپس CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_بائٹیں(entropy, (ossl_valsize_t)curlx_uztosi(لمبائی));
  واپس rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

ساکن CURLcode ossl_sha256sum(ثابت غیرعلامتی حرف *داخلہ,
                               size_t len,
                               غیرعلامتی حرف *sha256sum /* output */,
                               size_t غیر_مستعمل)
{
  CURLcode نتیجہ = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (خالی)غیر_مستعمل;

  mdctx = EVP_MD_CTX_new();
  اگر(!mdctx)
    واپس CURLE_برآمد_کا_حافظہ;
  اگر(!EVP_DigestInit_ex(mdctx, EVP_sha256(), تہی)) {
    نتیجہ = CURLE_FAILED_INIT;
    جاؤ برآمد;
  }
  اگر(!EVP_DigestUpdate(mdctx, داخلہ, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, تہی))
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
برآمد:
  EVP_MD_CTX_free(mdctx);
  واپس نتیجہ;
}

ساکن bool ossl_cert_حالت_request(خالی)
{
#اگر_غیر_تعریف_شدہ OPENSSL_NO_OCSP
  واپس TRUE;
#ورنہ
  واپس FALSE;
#اگر_ختم
}

ساکن خالی *ossl_get_internals(ساخت ssl_connect_ڈیٹا *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  ساخت ossl_ctx *octx = (ساخت ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  واپس info == CURLINFO_TLS_SESSION ?
    (خالی *)octx->ssl_ctx : (خالی *)octx->ssl;
}

ثابت ساخت Curl_ssl Curl_ssl_openssl = {
  { CURLSSLBACKEND_OPENSSL, "openssl" }, /* info */

  SSLSUPP_CA_راستہ |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_SSL_CTX |
#اگر_تعریف_شدہ HAVE_SSL_CTX_سیٹ_CIPHERSUITES
  SSLSUPP_TLS13_CIPHERSUITES |
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SSL_CTX_SET1_SIGALGS
  SSLSUPP_SIGNATURE_ALGORITHMS |
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SSL_SET1_ECH_CONFIG_LIST
  SSLSUPP_ECH |
#اگر_ختم
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_ISSUERCERT_BLOB |
  SSLSUPP_SSL_EC_CURVES |
  SSLSUPP_CRLFILE,

  حجم(ساخت ossl_ctx),

  ossl_init,                /* init */
  ossl_cleanup,             /* cleanup */
  Curl_ossl_نسخہ,        /*نسخہ*/
  ossl_shutdown,            /* shutdown */
  ossl_ڈیٹا_pending,        /* data_pending */
  ossl_random,              /* random */
  ossl_cert_حالت_request, /* cert_status_request */
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

#اگر_ختم /* USE_OPENSSL */
