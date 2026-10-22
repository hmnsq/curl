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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_OPENSSL

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "formdata.h" /* for the boundary function */
#அந்தர்பாவ "url.h" /* for the SSL config check function */
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "vtls/openssl.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_int.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "vtls/keylog.h"
#அந்தர்பாவ "vtls/hostcheck.h"
#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "multiif.h"
#அந்தர்பாவ "curlx/strerr.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curlx/strcopy.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ "vtls/apple.h"
#ifdef USE_ECH
#அந்தர்பாவ "curlx/base64.h"
#endif

#அந்தர்பாவ <openssl/rand.h>
#அந்தர்பாவ <openssl/x509v3.h>
#ifndef OPENSSL_NO_DSA
#அந்தர்பாவ <openssl/dsa.h>
#endif
#அந்தர்பாவ <openssl/dh.h>
#அந்தர்பாவ <openssl/err.h>
#அந்தர்பாவ <openssl/conf.h>
#அந்தர்பாவ <openssl/bn.h>
#அந்தர்பாவ <openssl/rsa.h>
#அந்தர்பாவ <openssl/bio.h>
#அந்தர்பாவ <openssl/pkcs12.h>
#அந்தர்பாவ <openssl/tls1.h>
#அந்தர்பாவ <openssl/evp.h>

#எனில் defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#அந்தர்பாவ <openssl/ech.h>
#endif

#ifndef OPENSSL_NO_OCSP
#அந்தர்பாவ <openssl/ocsp.h>
#endif

#எனில் !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#define USE_OPENSSL_ENGINE
#அந்தர்பாவ <openssl/engine.h>
#endif

#ifdef LIBRESSL_VERSION_எண்
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  எனில் LIBRESSL_VERSION_எண் < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#அல்லதெனில் !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#endif

#எனில் defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#அந்தர்பாவ <openssl/provider.h>
#அந்தர்பாவ <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#define OPENSSL_HAS_PROVIDERS
#endif

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#எனில் !defined(LIBRESSL_VERSION_எண்) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_சாவி_EXPLICIT_PARAMS))
#define HAVE_SSL_CTX_SET_DEFAULT_READ_BUFFER_LEN 1
#endif

#எனில் defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#அந்தர்பாவ <openssl/ui.h>
#endif

#ifdef HAVE_OPENSSL3
#define HAVE_EVP_PKEY_GET_PARAMS 1
#endif

#ifdef HAVE_EVP_PKEY_GET_PARAMS
#அந்தர்பாவ <openssl/core_names.h>
#define DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = வெற்று
#define FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#இல்லையெனில்
#define DECLARE_PKEY_PARAM_BIGNUM(name) மாறிலி BIGNUM *name
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
#ifndef LIBRESSL_VERSION_எண்
#define HAVE_SSL_CTX_SET1_SIGALGS
#endif

#ifdef LIBRESSL_VERSION_எண்
#define OSSL_PACKAGE "LibreSSL"
#அல்லதெனில் defined(OPENSSL_IS_AWSLC)
#define OSSL_PACKAGE "AWS-LC"
#அல்லதெனில் defined(OPENSSL_IS_BORINGSSL)
#define OSSL_PACKAGE "BoringSSL"
#அல்லதெனில் defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#define OSSL_PACKAGE "quictls"
#இல்லையெனில்
#define OSSL_PACKAGE "OpenSSL"
#endif

#ifdef HAVE_BORINGSSL_LIKE
வகைவரை size_t numcert_t;
வகைவரை uint32_t sslerr_t;
#இல்லையெனில்
வகைவரை முழு numcert_t;
வகைவரை unsigned long sslerr_t;
#endif
#define ossl_valsize_t numcert_t

#எனில் OPENSSL_VERSION_எண் >= 0x40100000L
#define ossl_ASN1_STRING_get_length ASN1_STRING_get_length
#இல்லையெனில்
நிலையான size_t ossl_ASN1_STRING_get_length(மாறிலி ASN1_STRING *str)
{
   மாறிலி முழு length = ASN1_STRING_length(str);
   திரும்பு length >= 0 ? (size_t)length : 0;
}
#endif

நிலையான CURLcode தள்ளு_certinfo(கட்டமைப்பு Curl_easy *data,
                              BIO *mem, மாறிலி எழுத்து *label, முழு இலக்கம்)
  WARN_UNUSED_RESULT;

நிலையான CURLcode தள்ளு_certinfo(கட்டமைப்பு Curl_easy *data,
                              BIO *mem, மாறிலி எழுத்து *label, முழு இலக்கம்)
{
  எழுத்து *ptr;
  long len = BIO_get_mem_data(mem, &ptr);
  CURLcode result = Curl_ssl_தள்ளு_certinfo_len(data, இலக்கம், label, ptr, len);
  (வெறுமை)BIO_reset(mem);
  திரும்பு result;
}

நிலையான CURLcode pubkey_show(கட்டமைப்பு Curl_easy *data,
                            BIO *mem,
                            முழு இலக்கம்,
                            மாறிலி எழுத்து *type,
                            மாறிலி எழுத்து *name,
                            மாறிலி BIGNUM *bn) WARN_UNUSED_RESULT;

நிலையான CURLcode pubkey_show(கட்டமைப்பு Curl_easy *data,
                            BIO *mem,
                            முழு இலக்கம்,
                            மாறிலி எழுத்து *type,
                            மாறிலி எழுத்து *name,
                            மாறிலி BIGNUM *bn)
{
  எழுத்து namebuf[32];

  curl_msnprintf(namebuf, அளவு(namebuf), "%s(%s)", type, name);

  எனில்(bn)
    BN_print(mem, bn);
  திரும்பு தள்ளு_certinfo(data, mem, namebuf, இலக்கம்);
}

#define print_pubkey_BN(_type, _name, இலக்கம்_2)           \
  pubkey_show(data, mem, இலக்கம்_2, #_type, #_name, _name)

நிலையான முழு asn1_object_dump(மாறிலி ASN1_OBJECT *a, எழுத்து *buf, size_t len)
{
  முழு i = i2t_ASN1_OBJECT(buf, (முழு)len, a);
  திரும்பு (i >= (முழு)len);  /* buffer too small */
}

நிலையான CURLcode X509V3_ext(கட்டமைப்பு Curl_easy *data,
                           முழு certnum,
                           மாறிலி அடுக்கு_OF(X509_EXTENSION) *extsarg)
{
  முழு i;
  CURLcode result = CURLE_OK;
#ifdef LIBRESSL_VERSION_எண்
  அடுக்கு_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#இல்லையெனில்
  மாறிலி அடுக்கு_OF(X509_EXTENSION) *exts = extsarg;
#endif

  எனில்((முழு)sk_X509_EXTENSION_இலக்கம்(exts) <= 0)
    /* no extensions, bail out */
    திரும்பு result;

  க்கு(i = 0; i < (முழு)sk_X509_EXTENSION_இலக்கம்(exts); i++) {
    மாறிலி ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_மதிப்பு(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    எழுத்து namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    எனில்(!bio_out)
      திரும்பு result;

    obj = X509_EXTENSION_get_object(ext);

    எனில்(asn1_object_dump(obj, namebuf, அளவு(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    எனில்(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (மாறிலி ASN1_STRING *)X509_EXTENSION_get_data(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    result = Curl_ssl_தள்ளு_certinfo_len(data, certnum, namebuf, biomem->data,
                                        biomem->length);
    BIO_free(bio_out);
    எனில்(result)
      நிறுத்து;
  }
  திரும்பு result;
}

நிலையான CURLcode get_pkey_rsa(கட்டமைப்பு Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, முழு i)
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
#இல்லையெனில்
  RSA_get0_சாவி(rsa, &n, &e, வெற்று);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (முழு)(n ? BN_இலக்கம்_bits(n) : 0));
  result = தள்ளு_certinfo(data, mem, "RSA Public Key", i);
  எனில்(!result) {
    result = print_pubkey_BN(rsa, n, i);
    எனில்(!result)
      result = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  திரும்பு result;
}

#ifndef OPENSSL_NO_DSA
நிலையான CURLcode get_pkey_dsa(கட்டமைப்பு Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, முழு i)
{
  CURLcode result = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_சாவி);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_சாவி, &pub_சாவி);
#இல்லையெனில்
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_சாவி(dsa, &pub_சாவி, வெற்று);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dsa, p, i);
  எனில்(!result)
    result = print_pubkey_BN(dsa, q, i);
  எனில்(!result)
    result = print_pubkey_BN(dsa, g, i);
  எனில்(!result)
    result = print_pubkey_BN(dsa, pub_சாவி, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_சாவி);
  திரும்பு result;
}
#endif /* !OPENSSL_NO_DSA */

நிலையான CURLcode get_pkey_dh(கட்டமைப்பு Curl_easy *data,
                            EVP_PKEY *pubkey, BIO *mem, முழு i)
{
  CURLcode result;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_சாவி);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_சாவி, &pub_சாவி);
#இல்லையெனில்
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_சாவி(dh, &pub_சாவி, வெற்று);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dh, p, i);
  எனில்(!result)
    result = print_pubkey_BN(dh, q, i);
  எனில்(!result)
    result = print_pubkey_BN(dh, g, i);
  எனில்(!result)
    result = print_pubkey_BN(dh, pub_சாவி, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_சாவி);
  திரும்பு result;
}

#ifdef HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
வகைவரை மாறிலி X509_PUBKEY pubkeytype_t;
#இல்லையெனில்
வகைவரை X509_PUBKEY pubkeytype_t;
#endif

நிலையான CURLcode ossl_certchain(கட்டமைப்பு Curl_easy *data, SSL *ssl)
{
  CURLcode result;
  அடுக்கு_OF(X509) *sk;
  முழு i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  எனில்(!sk)
    திரும்பு CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_இலக்கம்(sk);
  எனில்(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(data, "%d certificates is more than allowed (%d)", (முழு)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  result = Curl_ssl_init_certinfo(data, (முழு)numcerts);
  எனில்(result)
    திரும்பு result;

  mem = BIO_new(BIO_s_mem());
  எனில்(!mem)
    result = CURLE_OUT_OF_MEMORY;

  க்கு(i = 0; !result && (i < (முழு)numcerts); i++) {
    X509 *x = sk_X509_மதிப்பு(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = வெற்று;
    size_t j;
    மாறிலி ASN1_BIT_STRING *psig = வெற்று;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    result = தள்ளு_certinfo(data, mem, "Subject", i);
    எனில்(result)
      நிறுத்து;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    result = தள்ளு_certinfo(data, mem, "Issuer", i);
    எனில்(result)
      நிறுத்து;

    BIO_printf(mem, "%lx", (unsigned long)X509_get_version(x));
    result = தள்ளு_certinfo(data, mem, "Version", i);
    எனில்(result)
      நிறுத்து;

    {
      ASN1_INTEGER *இலக்கம் = X509_get_serial_எண்(x);
      மாறிலி unsigned எழுத்து *numdata = ASN1_STRING_get0_data(இலக்கம்);
      மாறிலி size_t numlen = ossl_ASN1_STRING_get_length(இலக்கம்);
      எனில்(ASN1_STRING_type(இலக்கம்) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      க்கு(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    result = தள்ளு_certinfo(data, mem, "Serial Number", i);
    எனில்(result)
      நிறுத்து;

    {
      மாறிலி X509_ALGOR *sigalg = வெற்று;
      pubkeytype_t *xpubkey = வெற்று;
      ASN1_OBJECT *pubkeyoid = வெற்று;

      X509_get0_signature(&psig, &sigalg, x);
      எனில்(sigalg) {
        மாறிலி ASN1_OBJECT *sigalgoid = வெற்று;
        X509_ALGOR_get0(&sigalgoid, வெற்று, வெற்று, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        result = தள்ளு_certinfo(data, mem, "Signature Algorithm", i);
        எனில்(result)
          நிறுத்து;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      எனில்(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, வெற்று, வெற்று, வெற்று, xpubkey);
        எனில்(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          result = தள்ளு_certinfo(data, mem, "Public Key Algorithm", i);
          எனில்(result)
            நிறுத்து;
        }
      }

      result = X509V3_ext(data, i, X509_get0_extensions(x));
      எனில்(result)
        நிறுத்து;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    result = தள்ளு_certinfo(data, mem, "Start date", i);
    எனில்(result)
      நிறுத்து;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    result = தள்ளு_certinfo(data, mem, "Expire date", i);
    எனில்(result)
      நிறுத்து;

    pubkey = X509_get_pubkey(x);
    எனில்(!pubkey)
      infof(data, "   Unable to load public key");
    இல்லையெனில் {
      தேர்வு(EVP_PKEY_id(pubkey)) {
      நிலை EVP_PKEY_RSA:
        result = get_pkey_rsa(data, pubkey, mem, i);
        நிறுத்து;

#ifndef OPENSSL_NO_DSA
      நிலை EVP_PKEY_DSA:
        result = get_pkey_dsa(data, pubkey, mem, i);
        நிறுத்து;
#endif

      நிலை EVP_PKEY_DH:
        result = get_pkey_dh(data, pubkey, mem, i);
        நிறுத்து;
      }
      EVP_PKEY_free(pubkey);
    }

    எனில்(!result && psig) {
      மாறிலி unsigned எழுத்து *psigdata = ASN1_STRING_get0_data(psig);
      மாறிலி size_t psiglen = ossl_ASN1_STRING_get_length(psig);
      க்கு(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      result = தள்ளு_certinfo(data, mem, "Signature", i);
    }

    எனில்(!result) {
      PEM_write_bio_X509(mem, x);
      result = தள்ளு_certinfo(data, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  எனில்(result)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(data);

  திரும்பு result;
}

நிலையான முழு ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_data(bio, வெற்று);
  திரும்பு 1;
}

நிலையான முழு ossl_bio_cf_destroy(BIO *bio)
{
  எனில்(!bio)
    திரும்பு 0;
  திரும்பு 1;
}

நிலையான long ossl_bio_cf_ctrl(BIO *bio, முழு cmd, long இலக்கம், வெறுமை *ptr)
{
  கட்டமைப்பு Curl_cfilter *cf = BIO_get_data(bio);
  long ret = 1;

  (வெறுமை)cf;
  (வெறுமை)ptr;
  தேர்வு(cmd) {
  நிலை BIO_CTRL_GET_CLOSE:
    ret = (long)BIO_get_shutdown(bio);
    நிறுத்து;
  நிலை BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (முழு)இலக்கம்);
    நிறுத்து;
  நிலை BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    நிறுத்து;
  நிலை BIO_CTRL_DUP:
    ret = 1;
    நிறுத்து;
  நிலை BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
    திரும்பு connssl->peer_closed;
  }
  இயல்பு:
    ret = 0;
    நிறுத்து;
  }
  திரும்பு ret;
}

நிலையான முழு ossl_bio_cf_out_write(BIO *bio, மாறிலி எழுத்து *buf, முழு blen)
{
  கட்டமைப்பு Curl_cfilter *cf = BIO_get_data(bio);
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  எனில்(blen < 0)
    திரும்பு 0;

  result = Curl_conn_cf_send(cf->next, data,
                             (மாறிலி uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (முழு)result, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  எனில்(result) {
    எனில்(result == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    திரும்பு -1;
  }
  திரும்பு (முழு)nwritten;
}

நிலையான முழு ossl_bio_cf_in_read(BIO *bio, எழுத்து *buf, முழு blen)
{
  கட்டமைப்பு Curl_cfilter *cf = BIO_get_data(bio);
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result, r2;

  DEBUGASSERT(data);
  /* OpenSSL catches this case, so should we. */
  எனில்(!buf)
    திரும்பு 0;
  எனில்(blen < 0)
    திரும்பு 0;

  result = Curl_conn_cf_recv(cf->next, data, buf, (size_t)blen, &nread);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (முழு)result, nread);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  எனில்(result) {
    எனில்(result == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  இல்லையெனில் {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->உள்ளீடு_pending = TRUE;
    எனில்(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  எனில்(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, data, octx);
    எனில்(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_result = r2;
      திரும்பு -1;
    }
    octx->x509_store_setup = TRUE;
  }
  திரும்பு result ? -1 : (முழு)nread;
}

நிலையான BIO_METHOD *ossl_bio_cf_method_create(வெறுமை)
{
  BIO_METHOD *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  எனில்(m) {
    BIO_meth_set_write(m, &ossl_bio_cf_out_write);
    BIO_meth_set_read(m, &ossl_bio_cf_in_read);
    BIO_meth_set_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_set_create(m, &ossl_bio_cf_create);
    BIO_meth_set_destroy(m, &ossl_bio_cf_destroy);
  }
  திரும்பு m;
}

நிலையான வெறுமை ossl_bio_cf_method_free(BIO_METHOD *m)
{
  எனில்(m)
    BIO_meth_free(m);
}

#ifndef HAVE_KEYLOG_UPSTREAM
#ifdef HAVE_KEYLOG_CALLBACK
நிலையான வெறுமை ossl_keylog_callback(மாறிலி SSL *ssl, மாறிலி எழுத்து *வரி)
{
  (வெறுமை)ssl;

  Curl_tls_keylog_write_வரி(வரி);
}
#இல்லையெனில்
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
நிலையான வெறுமை ossl_log_tls12_secret(மாறிலி SSL *ssl, bool *keylog_done)
{
  மாறிலி SSL_SESSION *session;
  unsigned எழுத்து client_random[SSL3_RANDOM_SIZE];
  unsigned எழுத்து master_சாவி[SSL_MAX_MASTER_சாவி_LENGTH];
  முழு master_சாவி_length = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  எனில்(!session || *keylog_done) {
    ERR_எடு_to_mark();
    திரும்பு;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_SIZE);
  master_சாவி_length = (முழு)
    SSL_SESSION_get_master_சாவி(session, master_சாவி, SSL_MAX_MASTER_சாவி_LENGTH);

  ERR_எடு_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  எனில்(master_சாவி_length <= 0)
    திரும்பு;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        அளவு(client_random),
                        master_சாவி, master_சாவி_length);
}
#endif /* !HAVE_KEYLOG_CALLBACK */
#endif /* HAVE_KEYLOG_UPSTREAM */

நிலையான மாறிலி எழுத்து *SSL_ERROR_to_str(முழு err)
{
  தேர்வு(err) {
  நிலை SSL_ERROR_NONE:
    திரும்பு "SSL_ERROR_NONE";
  நிலை SSL_ERROR_SSL:
    திரும்பு "SSL_ERROR_SSL";
  நிலை SSL_ERROR_WANT_READ:
    திரும்பு "SSL_ERROR_WANT_READ";
  நிலை SSL_ERROR_WANT_WRITE:
    திரும்பு "SSL_ERROR_WANT_WRITE";
  நிலை SSL_ERROR_WANT_X509_LOOKUP:
    திரும்பு "SSL_ERROR_WANT_X509_LOOKUP";
  நிலை SSL_ERROR_SYSCALL:
    திரும்பு "SSL_ERROR_SYSCALL";
  நிலை SSL_ERROR_ZERO_RETURN:
    திரும்பு "SSL_ERROR_ZERO_RETURN";
  நிலை SSL_ERROR_WANT_CONNECT:
    திரும்பு "SSL_ERROR_WANT_CONNECT";
  நிலை SSL_ERROR_WANT_ACCEPT:
    திரும்பு "SSL_ERROR_WANT_ACCEPT";
#ifdef SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  நிலை SSL_ERROR_WANT_ASYNC:
    திரும்பு "SSL_ERROR_WANT_ASYNC";
#endif
#ifdef SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  நிலை SSL_ERROR_WANT_ASYNC_JOB:
    திரும்பு "SSL_ERROR_WANT_ASYNC_JOB";
#endif
#ifdef SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  நிலை SSL_ERROR_WANT_CLIENT_HELLO_CB:
    திரும்பு "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#endif
  இயல்பு:
    திரும்பு "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
நிலையான எழுத்து *ossl_strerror(unsigned long error, எழுத்து *buf, size_t size)
{
  size_t len;
  DEBUGASSERT(size);
  *buf = '\0';

  len = Curl_ossl_version(buf, size);
  DEBUGASSERT(len < (size - 2));
  எனில்(len < (size - 2)) {
    buf += len;
    size -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ifdef HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, size);
#இல்லையெனில்
  ERR_error_string_n(error, buf, size);
#endif

  எனில்(!*buf) {
    மாறிலி எழுத்து *செய்தி = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, size, செய்தி, strlen(செய்தி));
  }

  திரும்பு buf;
}

நிலையான முழு passwd_callback(எழுத்து *buf, முழு இலக்கம், முழு encrypting, வெறுமை *password)
{
  DEBUGASSERT(encrypting == 0);

  எனில்(!encrypting && இலக்கம் >= 0 && password) {
    முழு klen = curlx_uztosi(strlen((எழுத்து *)password));
    எனில்(இலக்கம் > klen) {
      memcpy(buf, password, klen + 1);
      திரும்பு klen;
    }
  }
  திரும்பு 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
நிலையான bool rand_enough(வெறுமை)
{
  திரும்பு RAND_status() != 0;
}

நிலையான CURLcode ossl_seed(கட்டமைப்பு Curl_easy *data)
{
  /* This might get called before it has been added to a multi handle */
  எனில்(data->multi && data->multi->ssl_seeded)
    திரும்பு CURLE_OK;

  எனில்(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    எனில்(data->multi)
      data->multi->ssl_seeded = TRUE;
    திரும்பு CURLE_OK;
  }
  failf(data, "Insufficient randomness");
  திரும்பு CURLE_SSL_CONNECT_ERROR;
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
நிலையான முழு ossl_do_file_type(மாறிலி எழுத்து *type)
{
  எனில்(!type || !type[0])
    திரும்பு SSL_FILETYPE_PEM;
  எனில்(curl_strequal(type, "PEM"))
    திரும்பு SSL_FILETYPE_PEM;
  எனில்(curl_strequal(type, "DER"))
    திரும்பு SSL_FILETYPE_ASN1;
  எனில்(curl_strequal(type, "PROV"))
    திரும்பு SSL_FILETYPE_PROVIDER;
  எனில்(curl_strequal(type, "ENG"))
    திரும்பு SSL_FILETYPE_ENGINE;
  எனில்(curl_strequal(type, "P12"))
    திரும்பு SSL_FILETYPE_PKCS12;
  திரும்பு -1;
}

#எனில் defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
நிலையான முழு ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  மாறிலி எழுத்து *password;
  தேர்வு(UI_get_string_type(uis)) {
  நிலை UIT_PROMPT:
  நிலை UIT_VERIFY:
    password = (மாறிலி எழுத்து *)UI_get0_user_data(ui);
    எனில்(password && (UI_get_உள்ளீடு_flags(uis) & UI_உள்ளீடு_FLAG_DEFAULT_PWD)) {
      UI_set_result(ui, uis, password);
      திரும்பு 1;
    }
    FALLTHROUGH();
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
நிலையான முழு ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  தேர்வு(UI_get_string_type(uis)) {
  நிலை UIT_PROMPT:
  நிலை UIT_VERIFY:
    எனில்(UI_get0_user_data(ui) &&
       (UI_get_உள்ளீடு_flags(uis) & UI_உள்ளீடு_FLAG_DEFAULT_PWD)) {
      திரும்பு 1;
    }
    FALLTHROUGH();
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
நிலையான bool is_pkcs11_uri(மாறிலி எழுத்து *string)
{
  திரும்பு string && curl_strnequal(string, "pkcs11:", 7);
}

#endif

நிலையான CURLcode ossl_set_engine(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு ossl_ctx *octx,
                                மாறிலி எழுத்து *name);
#ifdef OPENSSL_HAS_PROVIDERS
நிலையான CURLcode ossl_set_provider(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ossl_ctx *octx,
                                  மாறிலி எழுத்து *iname);
#endif

நிலையான முழு use_certificate_blob(SSL_CTX *ctx, மாறிலி கட்டமைப்பு curl_blob *blob,
                                முழு type, மாறிலி எழுத்து *சாவி_passwd)
{
  முழு ret = 0;
  X509 *x = வெற்று;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->data, (முழு)blob->len);
  எனில்(!in)
    திரும்பு CURLE_OUT_OF_MEMORY;

  எனில்(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, வெற்று);
  }
  இல்லையெனில் எனில்(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, வெற்று, passwd_callback, CURL_UNCONST(சாவி_passwd));
  }
  இல்லையெனில் {
    ret = 0;
    செல் end;
  }

  எனில்(!x) {
    ret = 0;
    செல் end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  திரும்பு ret;
}

நிலையான முழு use_privatekey_blob(SSL_CTX *ctx, மாறிலி கட்டமைப்பு curl_blob *blob,
                               முழு type, மாறிலி எழுத்து *சாவி_passwd)
{
  முழு ret = 0;
  EVP_PKEY *pkey = வெற்று;
  BIO *in = BIO_new_mem_buf(blob->data, (முழு)blob->len);
  எனில்(!in)
    திரும்பு CURLE_OUT_OF_MEMORY;

  எனில்(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_Private_சாவி(in, வெற்று, passwd_callback,
                                   CURL_UNCONST(சாவி_passwd));
  இல்லையெனில் எனில்(type == SSL_FILETYPE_ASN1)
    pkey = d2i_Private_சாவி_bio(in, வெற்று);
  இல்லையெனில்
    செல் end;

  எனில்(!pkey)
    செல் end;

  ret = SSL_CTX_use_Private_சாவி(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  திரும்பு ret;
}

நிலையான முழு use_certificate_chain_blob(SSL_CTX *ctx,
                                      மாறிலி கட்டமைப்பு curl_blob *blob,
                                      மாறிலி எழுத்து *சாவி_passwd)
{
  முழு ret = 0;
  X509 *x = வெற்று;
  BIO *in = BIO_new_mem_buf(blob->data, (முழு)blob->len);
  எனில்(!in)
    திரும்பு CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, வெற்று,
                            passwd_callback, CURL_UNCONST(சாவி_passwd));
  எனில்(!x)
    செல் end;

  ret = SSL_CTX_use_certificate(ctx, x);

  எனில்(ERR_peek_error() != 0)
    ret = 0;

  எனில்(ret) {
    X509 *ca;
    sslerr_t err;

    எனில்(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      செல் end;
    }

    போது((ca = PEM_read_bio_X509(in, வெற்று, passwd_callback,
                                  CURL_UNCONST(சாவி_passwd))) != வெற்று) {

      எனில்(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        செல் end;
      }
    }

    err = ERR_peek_last_error();
    எனில்((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_வரி))
      ERR_clear_error();
    இல்லையெனில்
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  திரும்பு ret;
}

நிலையான முழு enginecheck(கட்டமைப்பு Curl_easy *data,
                       கட்டமைப்பு ossl_ctx *octx,
                       மாறிலி எழுத்து *சாவி_file,
                       மாறிலி எழுத்து *சாவி_passwd)
{
#ifdef USE_OPENSSL_ENGINE
  EVP_PKEY *priv_சாவி = வெற்று;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  எனில்(!octx->p.engine && is_pkcs11_uri(சாவி_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    திரும்பு 0;

  எனில்(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    எனில்(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      திரும்பு 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);
    priv_சாவி = ENGINE_load_private_சாவி(octx->p.engine, சாவி_file,
                                       ui_method,
                                       CURL_UNCONST(சாவி_passwd));
    UI_destroy_method(ui_method);
    எனில்(!priv_சாவி) {
      failf(data, "failed to load private key from crypto engine");
      திரும்பு 0;
    }
    எனில்(SSL_CTX_use_Private_சாவி(ctx, priv_சாவி) != 1) {
      failf(data, "unable to set private key");
      EVP_PKEY_free(priv_சாவி);
      திரும்பு 0;
    }
    EVP_PKEY_free(priv_சாவி);  /* we do not need the handle any more... */
  }
  இல்லையெனில் {
    failf(data, "crypto engine not set, cannot load private key");
    திரும்பு 0;
  }
  திரும்பு 1;
#இல்லையெனில்
  (வெறுமை)octx;
  (வெறுமை)சாவி_file;
  (வெறுமை)சாவி_passwd;
  failf(data, "SSL_FILETYPE_ENGINE not supported for private key");
  திரும்பு 0;
#endif
}

நிலையான முழு providercheck(கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு ossl_ctx *octx,
                         மாறிலி எழுத்து *சாவி_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  எழுத்து error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  எனில்(!octx->p.provider_loaded && is_pkcs11_uri(சாவி_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK) {
    திரும்பு 0;
  }

  எனில்(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_சாவி = வெற்று;
    OSSL_STORE_CTX *store = வெற்று;
    OSSL_STORE_INFO *info = வெற்று;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    எனில்(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      திரும்பு 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(சாவி_file, octx->p.libctx,
                               octx->p.propq, ui_method, வெற்று, வெற்று,
                               வெற்று, வெற்று);
    எனில்(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      UI_destroy_method(ui_method);
      திரும்பு 0;
    }
    எனில்(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    எனில்(info) {
      முழு ossl_type = OSSL_STORE_INFO_get_type(info);

      எனில்(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_சாவி = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    எனில்(!priv_சாவி) {
      failf(data, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      திரும்பு 0;
    }

    எனில்(SSL_CTX_use_Private_சாவி(ctx, priv_சாவி) != 1) {
      failf(data, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      EVP_PKEY_free(priv_சாவி);
      திரும்பு 0;
    }
    EVP_PKEY_free(priv_சாவி); /* we do not need the handle any more... */
  }
  இல்லையெனில் {
    failf(data, "crypto provider not set, cannot load private key");
    திரும்பு 0;
  }
  திரும்பு 1;
#இல்லையெனில்
  (வெறுமை)octx;
  (வெறுமை)சாவி_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for private key");
  திரும்பு 0;
#endif
}

நிலையான முழு engineload(கட்டமைப்பு Curl_easy *data,
                      கட்டமைப்பு ossl_ctx *octx,
                      மாறிலி எழுத்து *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#எனில் defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  எழுத்து error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  எனில்(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    திரும்பு 0;

  எனில்(octx->p.engine) {
    நிலையான மாறிலி எழுத்து cmd_name[] = "LOAD_CERT_CTRL";
    கட்டமைப்பு {
      மாறிலி எழுத்து *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = வெற்று;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    எனில்(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), வெற்று)) {
      failf(data, "SSL engine does not support loading certificates");
      திரும்பு 0;
    }

    /* Load the certificate from the engine */
    எனில்(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, வெற்று, 1)) {
      failf(data, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      திரும்பு 0;
    }

    எனில்(!params.cert) {
      failf(data, "SSL engine did not initialize the certificate properly.");
      திரும்பு 0;
    }

    எனில்(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      X509_free(params.cert);
      திரும்பு 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  இல்லையெனில் {
    failf(data, "crypto engine not set, cannot load certificate");
    திரும்பு 0;
  }
  திரும்பு 1;
#இல்லையெனில்
  (வெறுமை)octx;
  (வெறுமை)cert_file;
  failf(data, "SSL_FILETYPE_ENGINE not supported for certificate");
  திரும்பு 0;
#endif
}

நிலையான முழு providerload(கட்டமைப்பு Curl_easy *data,
                        கட்டமைப்பு ossl_ctx *octx,
                        மாறிலி எழுத்து *cert_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  எழுத்து error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  எனில்(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK)
    திரும்பு 0;

  எனில்(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = வெற்று;
    X509 *cert = வெற்று;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று);
    முழு rc;

    எனில்(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      திரும்பு 0;
    }
    எனில்(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    எனில்(info) {
      முழு ossl_type = OSSL_STORE_INFO_get_type(info);

      எனில்(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    எனில்(!cert) {
      failf(data, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      திரும்பு 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    எனில்(rc != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      திரும்பு 0;
    }
  }
  இல்லையெனில் {
    failf(data, "crypto provider not set, cannot load certificate");
    திரும்பு 0;
  }
  திரும்பு 1;
#இல்லையெனில்
  (வெறுமை)octx;
  (வெறுமை)cert_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for certificate");
  திரும்பு 0;
#endif
}

நிலையான முழு pkcs12load(கட்டமைப்பு Curl_easy *data,
                      SSL_CTX* ctx,
                      மாறிலி கட்டமைப்பு curl_blob *cert_blob,
                      மாறிலி எழுத்து *cert_file,
                      மாறிலி எழுத்து *சாவி_passwd)
{
  எழுத்து error_buffer[256];
  BIO *cert_bio = வெற்று;
  PKCS12 *p12 = வெற்று;
  EVP_PKEY *pri;
  X509 *x509;
  முழு cert_done = 0;
  அடுக்கு_OF(X509) *ca = வெற்று;
  எனில்(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->data, (முழு)cert_blob->len);
    எனில்(!cert_bio) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      திரும்பு 0;
    }
  }
  இல்லையெனில் {
    cert_bio = BIO_new(BIO_s_file());
    எனில்(!cert_bio) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
      திரும்பு 0;
    }

    எனில்(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(data, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      திரும்பு 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, வெற்று);
  BIO_free(cert_bio);

  எனில்(!p12) {
    failf(data, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    திரும்பு 0;
  }

  எனில்(!PKCS12_parse(p12, சாவி_passwd, &pri, &x509, &ca)) {
    failf(data, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, அளவு(error_buffer)));
    PKCS12_free(p12);
    திரும்பு 0;
  }

  PKCS12_free(p12);

  எனில்(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(data, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, அளவு(error_buffer)));
    செல் fail;
  }

  எனில்(SSL_CTX_use_Private_சாவி(ctx, pri) != 1) {
    failf(data, "unable to use private key from PKCS12 file '%s'", cert_file);
    செல் fail;
  }

  எனில்(!SSL_CTX_check_private_சாவி(ctx)) {
    failf(data, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    செல் fail;
  }
  /* Set Certificate Verification chain */
  எனில்(ca) {
    போது(sk_X509_இலக்கம்(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_எடு(ca);
      எனில்(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to client CA list");
        செல் fail;
      }
      எனில்(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to certificate chain");
        செல் fail;
      }
    }
  }

  cert_done = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_எடு_free(ca, X509_free);
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic எடு
#endif
  எனில்(!cert_done)
    திரும்பு 0; /* failure! */
  திரும்பு 1;
}

நிலையான CURLcode client_cert(கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு ossl_ctx *octx,
                            எழுத்து *cert_file,
                            மாறிலி கட்டமைப்பு curl_blob *cert_blob,
                            மாறிலி எழுத்து *cert_type,
                            எழுத்து *சாவி_file,
                            மாறிலி கட்டமைப்பு curl_blob *சாவி_blob,
                            மாறிலி எழுத்து *சாவி_type,
                            எழுத்து *சாவி_passwd)
{
  எழுத்து error_buffer[256];
  bool check_privkey = TRUE;
  முழு file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  எனில்(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    முழு cert_use_result;

    எனில்(சாவி_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, சாவி_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    தேர்வு(file_type) {
    நிலை SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_result = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, சாவி_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      எனில்(cert_use_result != 1) {
        failf(data,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            அளவு(error_buffer)));
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }
      நிறுத்து;

    நிலை SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_result = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, சாவி_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      எனில்(cert_use_result != 1) {
        failf(data,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            அளவு(error_buffer)));
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }
      நிறுத்து;

    நிலை SSL_FILETYPE_ENGINE:
      எனில்(!cert_file || !engineload(data, octx, cert_file))
        திரும்பு CURLE_SSL_CERTPROBLEM;
      நிறுத்து;

    நிலை SSL_FILETYPE_PROVIDER:
      எனில்(!cert_file || !providerload(data, octx, cert_file))
        திரும்பு CURLE_SSL_CERTPROBLEM;
      நிறுத்து;

    நிலை SSL_FILETYPE_PKCS12:
      எனில்(!pkcs12load(data, ctx, cert_blob, cert_file, சாவி_passwd))
        திரும்பு CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      நிறுத்து;

    இயல்பு:
      failf(data, "not supported file type '%s' for certificate", cert_type);
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }

    எனில்(!சாவி_file && !சாவி_blob) {
      சாவி_file = cert_file;
      சாவி_blob = cert_blob;
    }
    இல்லையெனில்
      file_type = ossl_do_file_type(சாவி_type);

    தேர்வு(file_type) {
    நிலை SSL_FILETYPE_PEM:
    நிலை SSL_FILETYPE_ASN1:
      cert_use_result = சாவி_blob ?
        use_privatekey_blob(ctx, சாவி_blob, file_type, சாவி_passwd) :
      SSL_CTX_use_Private_சாவி_file(ctx, சாவி_file, file_type);
      எனில்(cert_use_result != 1) {
        failf(data, "unable to set private key file: '%s' type %s",
              சாவி_file ? சாவி_file : "(memory blob)",
              சாவி_type ? சாவி_type : "PEM");
        திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
      }
      நிறுத்து;
    நிலை SSL_FILETYPE_ENGINE:
      எனில்(!enginecheck(data, octx, சாவி_file, சாவி_passwd))
        திரும்பு CURLE_SSL_CERTPROBLEM;
      நிறுத்து;

    நிலை SSL_FILETYPE_PROVIDER:
      எனில்(!providercheck(data, octx, சாவி_file))
        திரும்பு CURLE_SSL_CERTPROBLEM;
      நிறுத்து;

    நிலை SSL_FILETYPE_PKCS12:
      எனில்(!pcks12_done) {
        failf(data, "file type P12 for private key not supported");
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }
      நிறுத்து;
    இயல்பு:
      failf(data, "not supported file type for private key");
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    எனில்(!ssl) {
      failf(data, "unable to create an SSL structure");
      திரும்பு CURLE_OUT_OF_MEMORY;
    }

    x509 = SSL_get_certificate(ssl);

    எனில்(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#எனில் !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_சாவி = SSL_get_privatekey(ssl);
      எனில்(EVP_PKEY_id(priv_சாவி) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_சாவி);
        எனில்(RSA_flags(rsa) & RSA_METHOD_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#endif

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    எனில்(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      எனில்(!SSL_CTX_check_private_சாவி(ctx)) {
        failf(data, "Private key does not match the certificate public key");
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  திரும்பு CURLE_OK;
}

#ifdef CURLVERBOSE
/* returns non-zero on failure */
நிலையான CURLcode x509_name_oneline(மாறிலி X509_NAME *a, கட்டமைப்பு dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  முழு rc;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  எனில்(bio_out) {
    unsigned long flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    எனில்(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      result = curlx_dyn_addn(d, biomem->data, biomem->length);
    }
    BIO_free(bio_out);
  }
  திரும்பு result;
}
#endif

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
நிலையான முழு ossl_init(வெறுமை)
{
  மாறிலி uint64_t flags =
#ifdef OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#endif
#ifdef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    OPENSSL_INIT_NO_LOAD_CONFIG |
#இல்லையெனில்
    OPENSSL_INIT_LOAD_CONFIG |
#endif
    0;
  OPENSSL_init_ssl(flags, வெற்று);

#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#endif

  திரும்பு 1;
}

/* Global cleanup */
நிலையான வெறுமை ossl_cleanup(வெறுமை)
{
#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#endif
}

/* Selects an OpenSSL crypto engine or provider.
 */
நிலையான CURLcode ossl_set_engine(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு ossl_ctx *octx,
                                மாறிலி எழுத்து *name)
{
#ifdef USE_OPENSSL_ENGINE
  CURLcode result = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  எனில்(e) {

    எனில்(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = வெற்று;
    }
    எனில்(!ENGINE_init(e)) {
      எழுத்து buf[256];

      ENGINE_free(e);
      failf(data, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, அளவு(buf)));
      result = CURLE_SSL_ENGINE_INITFAILED;
      e = வெற்று;
    }
    இல்லையெனில் {
      result = CURLE_OK;
    }
    octx->p.engine = e;
    திரும்பு result;
  }
#endif
#ifdef OPENSSL_HAS_PROVIDERS
  திரும்பு ossl_set_provider(data, octx, name);
#இல்லையெனில்
  (வெறுமை)octx;
  (வெறுமை)name;
  failf(data, "OpenSSL engine not found");
  திரும்பு CURLE_SSL_ENGINE_NOTFOUND;
#endif
}

/* Sets engine as default for all SSL operations
 */
நிலையான CURLcode ossl_set_engine_default(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு ossl_ctx *octx)
{
#ifdef USE_OPENSSL_ENGINE
  எனில்(octx->p.engine) {
    எனில்(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(data, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    இல்லையெனில் {
      failf(data, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      திரும்பு CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)octx;
#endif
  திரும்பு CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
நிலையான கட்டமைப்பு curl_slist *ossl_engines_list(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு curl_slist *list = வெற்று;
#ifdef USE_OPENSSL_ENGINE
  கட்டமைப்பு curl_slist *beg;
  ENGINE *e;

  க்கு(e = ENGINE_get_first(); e; e = ENGINE_get_next(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    எனில்(!beg) {
      curl_slist_free_all(list);
      திரும்பு வெற்று;
    }
    list = beg;
  }
#endif
  (வெறுமை)data;
  திரும்பு list;
}

#ifdef OPENSSL_HAS_PROVIDERS

நிலையான வெறுமை ossl_provider_cleanup(கட்டமைப்பு ossl_ctx *octx)
{
  எனில்(!octx)
    திரும்பு;
  எனில்(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = வெற்று;
  }
  எனில்(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = வெற்று;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = வெற்று;
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
நிலையான CURLcode ossl_set_provider(கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ossl_ctx *octx,
                                  மாறிலி எழுத்து *iname)
{
  எழுத்து name[MAX_PROVIDER_LEN + 1];
  கட்டமைப்பு Curl_str prov;
  மாறிலி எழுத்து *propq = வெற்று;

  எனில்(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    திரும்பு CURLE_OK;
  }
  எனில்(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  memcpy(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  எனில்(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    எனில்(!libctx)
      திரும்பு CURLE_OUT_OF_MEMORY;
    எனில்(propq) {
      octx->p.propq = curlx_strdup(propq);
      எனில்(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        திரும்பு CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#ifndef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  எனில்(!OSSL_LIB_CTX_load_config(octx->p.libctx, வெற்று)) {
    infof(data, "Failed to load default openssl config. Proceeding.");
  }
#endif

  எனில்(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    திரும்பு CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  எனில்(!octx->p.provider) {
    எழுத்து error_buffer[256];
    failf(data, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_buffer, அளவு(error_buffer)));
    ossl_provider_cleanup(octx);
    திரும்பு CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  எனில்(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(data, "Failed to load base");
    திரும்பு CURLE_SSL_ENGINE_NOTFOUND;
  }
  இல்லையெனில்
    octx->p.provider_loaded = TRUE;
  திரும்பு CURLE_OK;
}
#இல்லையெனில் /* OPENSSL_HAS_PROVIDERS */
#define ossl_provider_cleanup(x)
#endif

நிலையான CURLcode ossl_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  எழுத்து buf[1024];
  முழு nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  எனில்(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    செல் out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  எனில்(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    க்கு(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (முழு)அளவு(buf));
      CURL_TRC_CF(data, cf, "SSL shutdown not sent, read -> %d", nread);
      எனில்(nread <= 0)
        நிறுத்து;
    }
    err = SSL_get_error(octx->ssl, nread);
    எனில்(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool உள்ளீடு_pending;
      /* Yes, it did. */
      எனில்(!send_shutdown) {
        CURL_TRC_CF(data, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        செல் out;
      }
      இல்லையெனில் எனில்(!cf->next->cft->is_alive(cf->next, data, &உள்ளீடு_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(data, cf, "peer closed connection");
        *done = TRUE;
        செல் out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  எனில்(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    முழு rc;
    ERR_clear_error();
    CURL_TRC_CF(data, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    எனில்(rc == 1) {
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
      *done = TRUE;
      செல் out;
    }
    எனில்(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      செல் out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  க்கு(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (முழு)அளவு(buf));
    CURL_TRC_CF(data, cf, "SSL shutdown read -> %d", nread);
    எனில்(nread <= 0)
      நிறுத்து;
  }
  err = SSL_get_error(octx->ssl, nread);
  தேர்வு(err) {
  நிலை SSL_ERROR_ZERO_RETURN: /* no more data */
    எனில்(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
    இல்லையெனில்
      CURL_TRC_CF(data, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    நிறுத்து;
  நிலை SSL_ERROR_NONE: /* did not get anything */
  நிலை SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(data, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    நிறுத்து;
  நிலை SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(data, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    நிறுத்து;
  இயல்பு:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(unsigned long sslerr = ERR_get_error());
      CURL_TRC_CF(data, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, அளவு(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    result = CURLE_OK;
    நிறுத்து;
  }

out:
  cf->shutdown = (result || *done);
  எனில்(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->உள்ளீடு_pending = FALSE;
  திரும்பு result;
}

/* also used by quic-tls */
வெறுமை Curl_osslctx_cleanup(கட்டமைப்பு ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  எனில்(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = வெற்று;
  }
  எனில்(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = வெற்று;
    octx->x509_store_setup = FALSE;
  }
  எனில்(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = வெற்று;
  }
#ifdef USE_OPENSSL_ENGINE
  எனில்(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = வெற்று;
  }
#endif
  ossl_provider_cleanup(octx);
}

நிலையான வெறுமை ossl_close(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  (வெறுமை)data;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
நிலையான வெறுமை ossl_close_all(கட்டமைப்பு Curl_easy *data)
{
  (வெறுமை)data;
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
நிலையான CURLcode ossl_verifyhost(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு connectdata *conn,
                                கட்டமைப்பு ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  முழு target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  அடுக்கு_OF(GENERAL_NAME) *altnames;
#ifdef USE_IPV6
  கட்டமைப்பு in6_addr addr;
#இல்லையெனில்
  கட்டமைப்பு in_addr addr;
#endif
  CURLcode result = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = strlen(peer->origin->hostname);

  (வெறுமை)conn;
  தேர்வு(peer->type) {
  நிலை CURL_SSL_PEER_IPV4:
    எனில்(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      திரும்பு CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = அளவு(கட்டமைப்பு in_addr);
    நிறுத்து;
#ifdef USE_IPV6
  நிலை CURL_SSL_PEER_IPV6:
    எனில்(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      திரும்பு CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = அளவு(கட்டமைப்பு in6_addr);
    நிறுத்து;
#endif
  நிலை CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    நிறுத்து;
  இயல்பு:
    DEBUGASSERT(0);
    failf(data, "unexpected SSL peer type: %d", (முழு)peer->type);
    திரும்பு CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, வெற்று, வெற்று);

  எனில்(altnames) {
#ifdef HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#இல்லையெனில்
    முழு numalts;
    முழு i;
#endif

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_இலக்கம்(altnames);

    /* loop through all alternatives - until a dnsmatch */
    க்கு(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      மாறிலி GENERAL_NAME *check = sk_GENERAL_NAME_மதிப்பு(altnames, i);

      எனில்(check->type == GEN_DNS)
        dNSName = TRUE;
      இல்லையெனில் எனில்(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      எனில்(check->type == target) {
        /* get data and length */
        மாறிலி எழுத்து *altptr = (மாறிலி எழுத்து *)ASN1_STRING_get0_data(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_length(check->d.ia5);

        தேர்வு(target) {
        நிலை GEN_DNS: /* name/pattern comparison */
          எனில்(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (முழு)altlen, altptr);
          }
          நிறுத்து;

        நிலை GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          எனில்((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          நிறுத்து;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  எனில்(matched)
    /* an alternative name matched */
    ;
  இல்லையெனில் எனில்(dNSName || iPAddress) {
    மாறிலி எழுத்து *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(data, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(data, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    result = CURLE_PEER_FAILED_VERIFICATION;
  }
  இல்லையெனில் {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    முழு i = -1;
    unsigned எழுத்து *cn = வெற்று;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    மாறிலி X509_NAME *name = X509_get_subject_name(server_cert);
    எனில்(name) {
      முழு j;
      போது((j = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    எனில்(i >= 0) {
      மாறிலி ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      எனில்(tmp) {
        எனில்(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_length(tmp);
          cn = (unsigned எழுத்து *)CURL_UNCONST(ASN1_STRING_get0_data(tmp));
        }
        இல்லையெனில் { /* not a UTF8 name */
          மாறிலி முழு utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          எனில்(utf8len < 0)
            cnlen = 0;
          இல்லையெனில்
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        எனில்((cnlen == 0) || !cn)
          result = CURLE_OUT_OF_MEMORY;
        இல்லையெனில் எனில்(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(data, "SSL: illegal cert name field");
          result = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    எனில்(result)
      /* error already detected, pass through */
      ;
    இல்லையெனில் எனில்(!cn) {
      failf(data, "SSL: unable to obtain common name from peer certificate");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    இல்லையெனில் எனில்(!Curl_cert_hostcheck((மாறிலி எழுத்து *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(data, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (முழு)cnlen, cn,
            peer->origin->user_hostname);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    இல்லையெனில் {
      infof(data, " common name: %.*s (matched)", (முழு)cnlen, cn);
    }
    எனில்(free_cn)
      OPENSSL_free(cn);
  }

  திரும்பு result;
}

#ifndef OPENSSL_NO_OCSP
நிலையான CURLcode verifystatus(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு ossl_ctx *octx)
{
  முழு i, ocsp_status;
#ifdef HAVE_BORINGSSL_LIKE
  மாறிலி uint8_t *status;
#இல்லையெனில்
  unsigned எழுத்து *status;
#endif
  மாறிலி unsigned எழுத்து *p;
  CURLcode result = CURLE_OK;
  OCSP_RESPONSE *rsp = வெற்று;
  OCSP_BASICRESP *br = வெற்று;
  X509_STORE     *st = வெற்று;
  அடுக்கு_OF(X509) *ch = வெற்று;
  X509 *cert;
  OCSP_CERTID *id = வெற்று;
  முழு cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  முழு ret;
  long len;

  (வெறுமை)cf;
  DEBUGASSERT(octx);

  len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  எனில்(!status) {
    failf(data, "No OCSP response received");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(வெற்று, &p, len);
  எனில்(!rsp) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }

  ocsp_status = OCSP_response_status(rsp);
  எனில்(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(data, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }

  br = OCSP_response_get1_basic(rsp);
  எனில்(!br) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }

#எனில் !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_எண்) && LIBRESSL_VERSION_எண் < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#இல்லையெனில்
  ch = SSL_get_peer_cert_chain(octx->ssl);
#endif
  எனில்(!ch) {
    failf(data, "Could not get peer certificate chain");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  எனில்(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(data, "OCSP response verification failed");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  எனில்(!cert) {
    failf(data, "Error getting peer certificate");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }

  க்கு(i = 0; i < (முழு)sk_X509_இலக்கம்(ch); i++) {
    X509 *issuer = sk_X509_மதிப்பு(ch, (ossl_valsize_t)i);
    எனில்(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      நிறுத்து;
    }
  }
  X509_free(cert);

  எனில்(!id) {
    failf(data, "Error computing OCSP ID");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  எனில்(ret != 1) {
    failf(data, "Could not find certificate ID in OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  எனில்(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(data, "OCSP response has expired");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }

  infof(data, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  தேர்வு(cert_status) {
  நிலை V_OCSP_CERTSTATUS_GOOD:
    நிறுத்து;

  நிலை V_OCSP_CERTSTATUS_REVOKED:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    failf(data, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    செல் end;

  நிலை V_OCSP_CERTSTATUS_UNKNOWN:
  இயல்பு:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் end;
  }

end:
  எனில்(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  திரும்பு result;
}
#endif

நிலையான மாறிலி எழுத்து *ssl_செய்தி_type(முழு ssl_ver, முழு செய்தி)
{
  எனில்(ssl_ver == SSL3_VERSION_MAJOR) {
    தேர்வு(செய்தி) {
    நிலை SSL3_MT_HELLO_REQUEST:
      திரும்பு "Hello request";
    நிலை SSL3_MT_CLIENT_HELLO:
      திரும்பு "Client hello";
    நிலை SSL3_MT_SERVER_HELLO:
      திரும்பு "Server hello";
#ifdef SSL3_MT_NEWSESSION_TICKET
    நிலை SSL3_MT_NEWSESSION_TICKET:
      திரும்பு "Newsession Ticket";
#endif
    நிலை SSL3_MT_CERTIFICATE:
      திரும்பு "Certificate";
    நிலை SSL3_MT_SERVER_சாவி_EXCHANGE:
      திரும்பு "Server key exchange";
    நிலை SSL3_MT_CLIENT_சாவி_EXCHANGE:
      திரும்பு "Client key exchange";
    நிலை SSL3_MT_CERTIFICATE_REQUEST:
      திரும்பு "Request CERT";
    நிலை SSL3_MT_SERVER_DONE:
      திரும்பு "Server finished";
    நிலை SSL3_MT_CERTIFICATE_VERIFY:
      திரும்பு "CERT verify";
    நிலை SSL3_MT_FINISHED:
      திரும்பு "Finished";
#ifdef SSL3_MT_CERTIFICATE_STATUS
    நிலை SSL3_MT_CERTIFICATE_STATUS:
      திரும்பு "Certificate Status";
#endif
#ifdef SSL3_MT_ENCRYPTED_EXTENSIONS
    நிலை SSL3_MT_ENCRYPTED_EXTENSIONS:
      திரும்பு "Encrypted Extensions";
#endif
#ifdef SSL3_MT_SUPPLEMENTAL_DATA
    நிலை SSL3_MT_SUPPLEMENTAL_DATA:
      திரும்பு "Supplemental data";
#endif
#ifdef SSL3_MT_END_OF_EARLY_DATA
    நிலை SSL3_MT_END_OF_EARLY_DATA:
      திரும்பு "End of early data";
#endif
#ifdef SSL3_MT_சாவி_UPDATE
    நிலை SSL3_MT_சாவி_UPDATE:
      திரும்பு "Key update";
#endif
#ifdef SSL3_MT_NEXT_PROTO
    நிலை SSL3_MT_NEXT_PROTO:
      திரும்பு "Next protocol";
#endif
#ifdef SSL3_MT_MESSAGE_HASH
    நிலை SSL3_MT_MESSAGE_HASH:
      திரும்பு "Message hash";
#endif
    }
  }
  திரும்பு "Unknown";
}

நிலையான மாறிலி எழுத்து *tls_rt_type(முழு type)
{
  தேர்வு(type) {
#ifdef SSL3_RT_HEADER
  நிலை SSL3_RT_HEADER:
    திரும்பு "TLS header";
#endif
  நிலை SSL3_RT_CHANGE_CIPHER_SPEC:
    திரும்பு "TLS change cipher";
  நிலை SSL3_RT_ALERT:
    திரும்பு "TLS alert";
  நிலை SSL3_RT_HANDSHAKE:
    திரும்பு "TLS handshake";
  நிலை SSL3_RT_APPLICATION_DATA:
    திரும்பு "TLS app data";
  இயல்பு:
    திரும்பு "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
நிலையான வெறுமை ossl_trace(முழு direction, முழு ssl_ver, முழு content_type,
                       மாறிலி வெறுமை *buf, size_t len, SSL *ssl,
                       வெறுமை *userp)
{
  மாறிலி எழுத்து *verstr;
  கட்டமைப்பு Curl_cfilter *cf = userp;
  கட்டமைப்பு Curl_easy *data = வெற்று;
  எழுத்து unknown[32];

  எனில்(!cf)
    திரும்பு;
  data = CF_DATA_CURRENT(cf);
  எனில்(!data || !data->set.fdebug || (direction && direction != 1))
    திரும்பு;

  தேர்வு(ssl_ver) {
#ifdef SSL3_VERSION
  நிலை SSL3_VERSION:
    verstr = "SSLv3";
    நிறுத்து;
#endif
  நிலை TLS1_VERSION:
    verstr = "TLSv1.0";
    நிறுத்து;
#ifdef TLS1_1_VERSION
  நிலை TLS1_1_VERSION:
    verstr = "TLSv1.1";
    நிறுத்து;
#endif
#ifdef TLS1_2_VERSION
  நிலை TLS1_2_VERSION:
    verstr = "TLSv1.2";
    நிறுத்து;
#endif
  நிலை TLS1_3_VERSION:
    verstr = "TLSv1.3";
    நிறுத்து;
  இயல்பு:
    curl_msnprintf(unknown, அளவு(unknown), "(%x)", (unsigned முழு)ssl_ver);
    verstr = unknown;
    நிறுத்து;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  எனில்(ssl_ver
#ifdef SSL3_RT_HEADER
     && content_type != SSL3_RT_HEADER
#endif
#ifdef SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#endif
    ) {
    மாறிலி எழுத்து *செய்தி_name = "Truncated message";
    மாறிலி எழுத்து *tls_rt_name;
    எழுத்து ssl_buf[1024];
    முழு செய்தி_type = 0;
    முழு txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    எனில்(ssl_ver == SSL3_VERSION_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    இல்லையெனில்
      tls_rt_name = "";

    எனில்(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      எனில்(len) {
        செய்தி_type = *(மாறிலி unsigned எழுத்து *)buf;
        செய்தி_name = "Change cipher spec";
      }
    }
    இல்லையெனில் எனில்(content_type == SSL3_RT_ALERT) {
      எனில்(len >= 2) {
        செய்தி_type =
          (((மாறிலி unsigned எழுத்து *)buf)[0] << 8) +
           ((மாறிலி unsigned எழுத்து *)buf)[1];
        செய்தி_name = SSL_alert_desc_string_long(செய்தி_type);
      }
    }
    இல்லையெனில் எனில்(len) {
      செய்தி_type = *(மாறிலி unsigned எழுத்து *)buf;
      செய்தி_name = ssl_செய்தி_type(ssl_ver, செய்தி_type);
    }

    txt_len = curl_msnprintf(ssl_buf, அளவு(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, செய்தி_name, செய்தி_type);
    Curl_debug(data, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(data, (direction == 1) ? CURLINFO_SSL_DATA_OUT :
             CURLINFO_SSL_DATA_IN, (மாறிலி எழுத்து *)buf, len);
  (வெறுமை)ssl;
}

நிலையான CURLcode ossl_set_ssl_version_min_max(கட்டமைப்பு Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             unsigned முழு ssl_version_min)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  long curl_ssl_version_min = (long)ssl_version_min;
  long curl_ssl_version_max;

  /* convert curl min SSL version option to OpenSSL constant */
#எனில் defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_VERSION_எண்)
  uint16_t ossl_ssl_version_min = 0;
  uint16_t ossl_ssl_version_max = 0;
#இல்லையெனில்
  long ossl_ssl_version_min = 0;
  long ossl_ssl_version_max = 0;
#endif
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_version_min != CURL_SSLVERSION_DEFAULT);
  தேர்வு(curl_ssl_version_min) {
  நிலை CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  நிலை CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    நிறுத்து;
  நிலை CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    நிறுத்து;
  நிலை CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    நிறுத்து;
  நிலை CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    நிறுத்து;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (long)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  தேர்வு(curl_ssl_version_max) {
  நிலை CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    நிறுத்து;
  நிலை CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    நிறுத்து;
  நிலை CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    நிறுத்து;
  நிலை CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    நிறுத்து;
  நிலை CURL_SSLVERSION_MAX_NONE:  /* none selected */
  நிலை CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  இயல்பு:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    நிறுத்து;
  }

  எனில்(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    திரும்பு CURLE_SSL_CONNECT_ERROR;

  திரும்பு CURLE_OK;
}

CURLcode Curl_ossl_add_session(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு ossl_ctx *octx,
                               மாறிலி எழுத்து *ssl_peer_சாவி,
                               SSL_SESSION *session,
                               மாறிலி எழுத்து *alpn,
                               unsigned எழுத்து *quic_tp,
                               size_t quic_tp_len,
                               கட்டமைப்பு Curl_ssl_session **psession)
{
  கட்டமைப்பு Curl_ssl_session *sc_session = வெற்று, *sc_dup = வெற்று;
  unsigned எழுத்து *der_session_buf = வெற்று;
  unsigned எழுத்து *qtp_clone = வெற்று;
  CURLcode result = CURLE_OK;

  எனில்(psession)
    *psession = வெற்று;
  எனில்(!cf || !data)
    செல் out;

  எனில்(Curl_ssl_scache_use(cf, data)) {
    size_t der_session_size;
    unsigned எழுத்து *der_session_ptr;
    size_t earlydata_max = 0;
    முழு ietf_tls_id = SSL_version(octx->ssl);

    der_session_size = i2d_SSL_SESSION(session, வெற்று);
    எனில்(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_size);
    எனில்(!der_session_buf) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }

    der_session_size = i2d_SSL_SESSION(session, &der_session_ptr);
    எனில்(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }

#ifdef HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_data(session);
#endif
    எனில்(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((மாறிலி எழுத்து *)quic_tp, quic_tp_len);
      எனில்(!qtp_clone) {
        result = CURLE_OUT_OF_MEMORY;
        செல் out;
      }
    }

    result = Curl_ssl_session_create2(der_session_buf, der_session_size,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(வெற்று) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = வெற்று;  /* took ownership of sdata */
#ifdef USE_APPLE_SECTRUST
    எனில்(!result)
      sc_session->sectrust_verified = octx->sectrust_verified;
#endif
    எனில்(!result && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        result = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    எனில்(!result) {
      result = Curl_ssl_scache_put(cf, data, ssl_peer_சாவி, sc_session);
      /* took ownership of `sc_session` */
      sc_session = வெற்று;
    }
  }

out:
  curlx_free(der_session_buf);
  எனில்(!result && psession) {
    *psession = sc_dup;
    sc_dup = வெற்று;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  திரும்பு result;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
நிலையான முழு ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  கட்டமைப்பு Curl_cfilter *cf = (கட்டமைப்பு Curl_cfilter *)SSL_get_app_data(ssl);
  எனில்(cf) {
    கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
    கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
    கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
    கட்டமைப்பு Curl_ssl_session *session = வெற்று;
    Curl_ossl_add_session(cf, data, octx, connssl->peer.scache_சாவி,
                          ssl_sessionid, connssl->negotiated.alpn, வெற்று,
                          0, &session);
    எனில்(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  திரும்பு 0;
}

நிலையான CURLcode load_cacert_from_memory(X509_STORE *store,
                                        மாறிலி கட்டமைப்பு curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = வெற்று;
  அடுக்கு_OF(X509_INFO) *inf = வெற்று;

  /* everything else is a reference */
  முழு i, மொத்தம் = 0;
  X509_INFO *itmp = வெற்று;

  எனில்(ca_info_blob->len > (size_t)INT_MAX)
    திரும்பு CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->data, (முழு)ca_info_blob->len);
  எனில்(!cbio)
    திரும்பு CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, வெற்று, வெற்று, வெற்று);
  எனில்(!inf) {
    BIO_free(cbio);
    திரும்பு CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  க்கு(i = 0; i < (முழு)sk_X509_INFO_இலக்கம்(inf); ++i) {
    itmp = sk_X509_INFO_மதிப்பு(inf, (ossl_valsize_t)i);
    எனில்(itmp->x509) {
      எனில்(X509_STORE_add_cert(store, itmp->x509)) {
        ++மொத்தம்;
      }
      இல்லையெனில் {
        /* set count to 0 to return an error */
        மொத்தம் = 0;
        நிறுத்து;
      }
    }
    எனில்(itmp->crl) {
      எனில்(X509_STORE_add_crl(store, itmp->crl)) {
        ++மொத்தம்;
      }
      இல்லையெனில் {
        /* set count to 0 to return an error */
        மொத்தம் = 0;
        நிறுத்து;
      }
    }
  }

#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic தள்ளு
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_INFO_எடு_free(inf, X509_INFO_free);
#எனில் defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic எடு
#endif
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  திரும்பு (மொத்தம் > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ifdef USE_WIN32_CRYPTO
நிலையான CURLcode ossl_win_load_store(கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு Curl_cfilter *cf,
                                    மாறிலி எழுத்து *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode result = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  எனில்(hStore) {
    PCCERT_CONTEXT pContext = வெற்று;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = வெற்று;
    DWORD enhkey_usage_size = 0;
    VERBOSE(size_t total = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    result = CURLE_OK;
    க்கு(;;) {
      X509 *x509;
      FILETIME now;
      BYTE சாவி_usage[2];
      DWORD req_size;
      மாறிலி unsigned எழுத்து *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      எனில்(!pContext)
        நிறுத்து;

      VERBOSE(++total);

#எனில் defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        எழுத்து cert_name[256];
        எனில்(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               வெற்று, cert_name, அளவு(cert_name)))
          infof(data, "SSL: unknown cert name");
        இல்லையெனில்
          infof(data, "SSL: Checking cert \"%s\"", cert_name);
      }
#endif
      encoded_cert = (மாறிலி unsigned எழுத்து *)pContext->pbCertEncoded;
      எனில்(!encoded_cert)
        தொடர்;

      GetSystemTimeAsFileTime(&now);
      எனில்(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        தொடர்;

      /* If key usage exists check for signing attribute */
      எனில்(Cert_Get_Intended_சாவி_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 சாவி_usage, அளவு(சாவி_usage))) {
        எனில்(!(சாவி_usage[0] & CERT_சாவி_CERT_SIGN_சாவி_USAGE))
          தொடர்;
      }
      இல்லையெனில் எனில்(GetLastError())
        தொடர்;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      எனில்(Cert_Get_Enhanced_சாவி_Usage(pContext, 0, வெற்று, &req_size) && req_size) {
        எனில்(req_size > enhkey_usage_size) {
          வெறுமை *tmp = curlx_realloc(enhkey_usage, req_size);

          எனில்(!tmp) {
            failf(data, "SSL: Out of memory allocating for OID list");
            result = CURLE_OUT_OF_MEMORY;
            நிறுத்து;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_size = req_size;
        }

        எனில்(Cert_Get_Enhanced_சாவி_Usage(pContext, 0, enhkey_usage, &req_size)) {
          எனில்(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            எனில்((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              தொடர்;
          }
          இல்லையெனில் {
            DWORD i;
            bool found = FALSE;

            க்கு(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              எனில்(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                நிறுத்து;
              }
            }

            எனில்(!found)
              தொடர்;
          }
        }
        இல்லையெனில்
          தொடர்;
      }
      இல்லையெனில்
        தொடர்;

      x509 = d2i_X509(வெற்று, &encoded_cert, (long)pContext->cbCertEncoded);
      எனில்(!x509)
        தொடர்;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      எனில்(X509_STORE_add_cert(store, x509) == 1) {
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

    எனில்(result)
      திரும்பு result;
  }

  திரும்பு result;
}

நிலையான CURLcode ossl_windows_load_anchors(கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  நிலையான மாறிலி எழுத்து * மாறிலி win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode result = CURLE_OK;

  *padded = FALSE;
  க்கு(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    result = ossl_win_load_store(data, cf, win_stores[i], store, &store_added);
    எனில்(result)
      திரும்பு result;
    எனில்(store_added) {
      CURL_TRC_CF(data, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    இல்லையெனில்
      infof(data, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  திரும்பு result;
}

#endif /* USE_WIN32_CRYPTO */

நிலையான CURLcode ossl_load_trust_anchors(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு ossl_ctx *octx,
                                        X509_STORE *store)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து * மாறிலி ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? வெற்று : conn_config->CAfile);
  மாறிலி எழுத்து * மாறிலி ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  எனில்(conn_config->native_ca_store) {
#ifdef USE_WIN32_CRYPTO
    bool added = FALSE;
    result = ossl_windows_load_anchors(cf, data, store, &added);
    எனில்(result)
      திரும்பு result;
    எனில்(added) {
      infof(data, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#அல்லதெனில் defined(USE_APPLE_SECTRUST)
    infof(data, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#endif
  }

  எனில்(conn_config->ca_info_blob) {
    result = load_cacert_from_memory(store, conn_config->ca_info_blob);
    எனில்(result) {
      failf(data, "error adding trust anchors from certificate blob: %d",
            (முழு)result);
      திரும்பு result;
    }
    infof(data, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  எனில்(ssl_cafile || ssl_capath) {
#ifdef HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    எனில்(ssl_cafile) {
      எனில்(!X509_STORE_load_file(store, ssl_cafile)) {
        எனில்(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from file: %s", ssl_cafile);
          திரும்பு CURLE_SSL_CACERT_BADFILE;
        }
        இல்லையெனில்
          infof(data, "error setting certificate file, continuing anyway");
      }
      infof(data, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    எனில்(ssl_capath) {
      எனில்(!X509_STORE_load_path(store, ssl_capath)) {
        எனில்(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from path: %s", ssl_capath);
          திரும்பு CURLE_SSL_CACERT_BADFILE;
        }
        இல்லையெனில்
          infof(data, "error setting certificate path, continuing anyway");
      }
      infof(data, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#இல்லையெனில்
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    எனில்(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      எனில்(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(data, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        திரும்பு CURLE_SSL_CACERT_BADFILE;
      }
      இல்லையெனில் {
        infof(data, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    எனில்(ssl_cafile)
      infof(data, "  CAfile: %s", ssl_cafile);
    எனில்(ssl_capath)
      infof(data, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#endif
  }

#ifdef CURL_CA_FALLBACK
  எனில்(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(data, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#endif
  எனில்(octx->store_is_empty && !have_native_check)
    infof(data, "  no trust anchors configured");

  திரும்பு result;
}

நிலையான CURLcode ossl_populate_x509_store(கட்டமைப்பு Curl_cfilter *cf,
                                         கட்டமைப்பு Curl_easy *data,
                                         கட்டமைப்பு ossl_ctx *octx,
                                         X509_STORE *store)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  X509_LOOKUP *lookup = வெற்று;
  மாறிலி எழுத்து * மாறிலி ssl_crlfile = conn_config->CRLfile;
  unsigned long x509flags = 0;

  CURL_TRC_CF(data, cf, "configuring OpenSSL's x509 trust store");
  எனில்(!store)
    திரும்பு CURLE_OUT_OF_MEMORY;

  எனில்(!conn_config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    திரும்பு CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  result = ossl_load_trust_anchors(cf, data, octx, store);
  எனில்(result)
    திரும்பு result;

  /* Does not make sense to load a CRL file without peer verification */
#ifdef USE_APPLE_SECTRUST
  எனில்(ssl_crlfile && conn_config->native_ca_store) {
    failf(data, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    திரும்பு CURLE_NOT_BUILT_IN;
  }
#endif
  எனில்(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    எனில்(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(data, "error loading CRL file: %s", ssl_crlfile);
      திரும்பு CURLE_SSL_CRL_BADFILE;
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

  எனில்(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (வெறுமை)X509_STORE_set_flags(store, x509flags);

  திரும்பு result;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_OSSL_X509_சாவி  "tls:ossl:x509:share"

கட்டமைப்பு ossl_x509_share {
  எழுத்து *CAfile;         /* CAfile path used to generate X509 store */
  எழுத்து *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  கட்டமைப்பு curltime time; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

நிலையான வெறுமை oss_x509_share_free(மாறிலி வெறுமை *சாவி, size_t சாவி_len, வெறுமை *p)
{
  கட்டமைப்பு ossl_x509_share *share = p;
  DEBUGASSERT(சாவி_len == CURL_CSTRLEN(MPROTO_OSSL_X509_சாவி));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_சாவி, சாவி, சாவி_len));
  (வெறுமை)சாவி;
  (வெறுமை)சாவி_len;
  எனில்(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

நிலையான bool ossl_cached_x509_store_expired(கட்டமைப்பு Curl_easy *data,
                                           மாறிலி கட்டமைப்பு ossl_x509_share *mb)
{
  எனில்(data->set.ssl_ca_cache_timeout < 0)
    திரும்பு FALSE;
  இல்லையெனில் {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &mb->time);
    timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

    திரும்பு elapsed_ms >= timeout_ms;
  }
}

நிலையான bool ossl_cached_x509_store_different(கட்டமைப்பு Curl_cfilter *cf,
                                             மாறிலி கட்டமைப்பு Curl_easy *data,
                                             மாறிலி கட்டமைப்பு ossl_x509_share *mb)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (வெறுமை)data;
  எனில்(mb->no_partialchain != conn_config->no_partialchain)
    திரும்பு TRUE;
  எனில்(!mb->CAfile || !conn_config->CAfile) {
    எனில்(mb->CAfile != conn_config->CAfile)
      திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(strcmp(mb->CAfile, conn_config->CAfile))
    திரும்பு TRUE;

  எனில்(!mb->CApath || !conn_config->CApath) {
    எனில்(mb->CApath != conn_config->CApath)
      திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(strcmp(mb->CApath, conn_config->CApath))
    திரும்பு TRUE;

  திரும்பு FALSE;
}

நிலையான X509_STORE *ossl_get_cached_x509_store(கட்டமைப்பு Curl_cfilter *cf,
                                              கட்டமைப்பு Curl_easy *data,
                                              bool *pempty)
{
  கட்டமைப்பு Curl_multi *multi = data->multi;
  கட்டமைப்பு ossl_x509_share *share;
  X509_STORE *store = வெற்று;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_சாவி,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_சாவி)) : வெற்று;
  எனில்(share && share->store &&
     !ossl_cached_x509_store_expired(data, share) &&
     !ossl_cached_x509_store_different(cf, data, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  திரும்பு store;
}

நிலையான வெறுமை ossl_set_cached_x509_store(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       X509_STORE *store,
                                       bool is_empty)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு Curl_multi *multi = data->multi;
  கட்டமைப்பு ossl_x509_share *share;

  DEBUGASSERT(multi);
  எனில்(!multi)
    திரும்பு;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_சாவி,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_சாவி));

  எனில்(!share) {
    share = curlx_calloc(1, அளவு(*share));
    எனில்(!share)
      திரும்பு;
    எனில்(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_சாவி,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_சாவி),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      திரும்பு;
    }
  }

  எனில்(X509_STORE_up_ref(store)) {
    எழுத்து *CAfile = வெற்று;
    எழுத்து *CApath = வெற்று;

    எனில்(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      எனில்(!CAfile) {
        X509_STORE_free(store);
        திரும்பு;
      }
    }

    எனில்(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      எனில்(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        திரும்பு;
      }
    }

    எனில்(share->store) {
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
#இல்லையெனில்
#define USING_PROVIDER(x) 0
#endif

CURLcode Curl_ssl_setup_x509_store(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ossl_ctx *octx)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
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
  எனில்(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  இல்லையெனில் {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    result = ossl_populate_x509_store(cf, data, octx, store);
    எனில்(result == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, data, store, (bool)octx->store_is_empty);
    }
  }

  ERR_எடு_to_mark();

  திரும்பு result;
}

நிலையான bool ossl_apply_session(
  கட்டமைப்பு ossl_ctx *octx,
  கட்டமைப்பு Curl_cfilter *cf,
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  கட்டமைப்பு Curl_ssl_session *scs)
{
  கட்டமைப்பு ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  மாறிலி unsigned எழுத்து *der_sessionid = scs->sdata;
  size_t der_sessionid_size = scs->sdata_len;
  SSL_SESSION *ssl_session = வெற்று;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(வெற்று, &der_sessionid,
                                (long)der_sessionid_size);
  எனில்(ssl_session) {
    எனில்(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(எழுத்து error_buffer[256]);
      infof(data, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          அளவு(error_buffer)));
    }
    இல்லையெனில் {
      எனில்(conn_cfg->verifypeer &&
         (SSL_get_verify_result(octx->ssl) != X509_V_OK)
#ifdef USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#endif
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, வெற்று);
        infof(data, "SSL session not peer verified, not reusing");
      }
      இல்லையெனில் {
        infof(data, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ifdef USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#endif
        infof(data, "SSL verify result: %lx",
              (unsigned long)SSL_get_verify_result(octx->ssl));
#ifdef HAVE_OPENSSL_EARLYDATA
        எனில்(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_data(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_data = FALSE;
          எனில்(sess_reuse_cb)
            (வெறுமை)sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          எனில்(do_early_data) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_to(alpns, scs->alpn);
          }
        }
#இல்லையெனில்
        (வெறுமை)alpns;
        (வெறுமை)conn_cfg;
        (வெறுமை)sess_reuse_cb;
#endif
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  இல்லையெனில் {
    infof(data, "SSL session not accepted by OpenSSL, continuing without");
  }
  திரும்பு (bool)octx->reused_session;
}

நிலையான CURLcode ossl_init_session_and_alpns(
  கட்டமைப்பு ossl_ctx *octx,
  கட்டமைப்பு Curl_cfilter *cf,
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு ssl_peer *peer,
  மாறிலி கட்டமைப்பு alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  கட்டமைப்பு ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு alpn_spec alpns;
  CURLcode result;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  எனில்((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    கட்டமைப்பு Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    எனில்(scs) {
      எனில்(ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
    }
  }

  எனில்(!octx->reused_session &&
     Curl_ssl_scache_use(cf, data) && !conn_cfg->verifystatus) {
    கட்டமைப்பு Curl_ssl_session *scs = வெற்று;

    result = Curl_ssl_scache_take(cf, data, peer->scache_சாவி, &scs);
    எனில்(!result && scs && scs->sdata && scs->sdata_len) {
      (வெறுமை)ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, data, peer->scache_சாவி, scs);
  }

  எனில்(alpns.மொத்தம்) {
    கட்டமைப்பு alpn_proto_buf proto;
    memset(&proto, 0, அளவு(proto));
    result = Curl_alpn_to_proto_buf(&proto, &alpns);
    எனில்(result) {
      failf(data, "Error determining ALPN");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
    எனில்(SSL_set_alpn_protos(octx->ssl, proto.data, proto.len)) {
      failf(data, "Error setting ALPN");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }

  திரும்பு CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
நிலையான CURLcode ossl_init_ech(கட்டமைப்பு ossl_ctx *octx,
                              கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு ssl_peer *peer)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  முழு trying_ech_now = 0;

  எனில்(!Curl_ssl_ech_enabled(cf))
    திரும்பு CURLE_OK;

  எனில்(conn_config->ech == CURLECH_GREASE) {
    infof(data, "ECH: will GREASE ClientHello");
#ifdef HAVE_BORINGSSL_LIKE
    SSL_set_enable_ech_grease(octx->ssl, 1);
#இல்லையெனில்
    SSL_set_options(octx->ssl, SSL_OP_ECH_GREASE);
#endif
  }
  இல்லையெனில் எனில்(conn_config->ech && conn_config->ech_config) {
#ifdef HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode result;

    எனில்(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * strlen(conn_config->ech_config);
    result = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    எனில்(result || !ech_config) {
      infof(data, "ECH: cannot base64 decode ECHConfig from command line");
      எனில்(conn_config->ech == CURLECH_HARD)
        திரும்பு result;
    }
    எனில்(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      எனில்(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        திரும்பு CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#இல்லையெனில்
    size_t ech_config_len = 0;
    எனில்(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = strlen(conn_config->ech_config);
    எனில்(SSL_set1_ech_config_list(octx->ssl,
                                (மாறிலி uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      எனில்(conn_config->ech == CURLECH_HARD)
        திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
    இல்லையெனில்
      trying_ech_now = 1;
#endif /* HAVE_BORINGSSL_LIKE */
    infof(data, "ECH: ECHConfig from command line");
  }
  இல்லையெனில் {
    மாறிலி கட்டமைப்பு Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, peer->origin);

    எனில்(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      மாறிலி unsigned எழுத்து *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(data, "ECH: ECHConfig from HTTPS RR");
      எனில்(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(data, "ECH: SSL_set1_ech_config_list failed");
        எனில்(conn_config->ech == CURLECH_HARD)
          திரும்பு CURLE_SSL_CONNECT_ERROR;
      }
      இல்லையெனில் {
        trying_ech_now = 1;
        infof(data, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    இல்லையெனில் {
      infof(data, "ECH: requested but no ECHConfig available");
      எனில்(conn_config->ech == CURLECH_HARD)
        திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }
#ifdef HAVE_BORINGSSL_LIKE
  (வெறுமை)peer;
  எனில்(trying_ech_now && conn_config->ech_public) {
    infof(data, "ECH: setting public_name not supported with BoringSSL");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }
#இல்லையெனில்
  எனில்(trying_ech_now && conn_config->ech_public) {
    முழு ret;
    infof(data, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    எனில்(ret != 1) {
      infof(data, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }
#endif /* HAVE_BORINGSSL_LIKE */
  எனில்(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(data, "ECH: cannot force TLSv1.3 [ERROR]");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  திரும்பு CURLE_OK;
}
#endif /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

நிலையான CURLcode ossl_init_ssl(கட்டமைப்பு ossl_ctx *octx,
                              கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு ssl_peer *peer,
                              மாறிலி கட்டமைப்பு alpn_spec *alpns_requested,
                              வெறுமை *ssl_user_data,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  எனில்(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  எனில்(!octx->ssl) {
    failf(data, "SSL: could not create a context (handle)");
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_data(octx->ssl, ssl_user_data);

#ifndef OPENSSL_NO_OCSP
  எனில்(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#endif

  SSL_set_connect_state(octx->ssl);

  எனில்(peer->sni) {
    எனில்(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(data, "Failed set SNI");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode result = ossl_init_ech(octx, cf, data, peer);
    எனில்(result)
      திரும்பு result;
  }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  திரும்பு ossl_init_session_and_alpns(octx, cf, data, peer,
                                     alpns_requested, sess_reuse_cb);
}

நிலையான CURLcode ossl_init_method(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு ssl_peer *peer,
                                 மாறிலி SSL_METHOD **pmethod,
                                 unsigned முழு *pssl_version_min)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = வெற்று;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  தேர்வு(peer->transport) {
  நிலை TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    தேர்வு(*pssl_version_min) {
    நிலை CURL_SSLVERSION_TLSv1:
    நிலை CURL_SSLVERSION_TLSv1_0:
    நிலை CURL_SSLVERSION_TLSv1_1:
    நிலை CURL_SSLVERSION_TLSv1_2:
    நிலை CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      நிறுத்து;
    நிலை CURL_SSLVERSION_SSLv2:
      failf(data, "No SSLv2 support");
      திரும்பு CURLE_NOT_BUILT_IN;
    நிலை CURL_SSLVERSION_SSLv3:
      failf(data, "No SSLv3 support");
      திரும்பு CURLE_NOT_BUILT_IN;
    இயல்பு:
      failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
    நிறுத்து;
  நிலை TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    எனில்(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(data, "QUIC needs at least TLS version 1.3");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    நிறுத்து;
  இயல்பு:
    failf(data, "unsupported transport %d in SSL init", peer->transport);
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  திரும்பு *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(கட்டமைப்பு ossl_ctx *octx,
                            கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு ssl_peer *peer,
                            மாறிலி கட்டமைப்பு alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            வெறுமை *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            வெறுமை *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *ciphers;
  மாறிலி SSL_METHOD *req_method = வெற்று;
  ctx_option_t ctx_options = 0;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  எழுத்து * மாறிலி ssl_cert = conn_config->clientcert;
  மாறிலி கட்டமைப்பு curl_blob *ssl_cert_blob = conn_config->cert_blob;
  மாறிலி எழுத்து * மாறிலி ssl_cert_type = conn_config->cert_type;
  unsigned முழு ssl_version_min = CURL_SSLVERSION_DEFAULT;
  எழுத்து error_buffer[256];

  /* Make funny stuff to get random input */
  result = ossl_seed(data);
  எனில்(!result) {
    எனில்(CURL_EASY_STR(data, STRING_SSL_ENGINE))
      result = ossl_set_engine(data, octx,
                               CURL_EASY_STR(data, STRING_SSL_ENGINE));
    எனில்(!result && data->set.engine_default)
      result = ossl_set_engine_default(data, octx);
  }

  எனில்(!result) {
    ssl_config->certverifyresult = !X509_V_OK;
    result = ossl_init_method(cf, data, peer, &req_method, &ssl_version_min);
  }
  எனில்(result)
    திரும்பு result;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ifdef OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#endif
    SSL_CTX_new(req_method);

  எனில்(!octx->ssl_ctx) {
    failf(data, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_buffer, அளவு(error_buffer)));
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  எனில்(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(data->set.fdebug && data->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_செய்தி_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_செய்தி_callback_arg(octx->ssl_ctx, cf);
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
  எனில்(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_செருகு_EMPTY_FRAGMENTS;

  DEBUGASSERT(ssl_version_min != CURL_SSLVERSION_DEFAULT);
  தேர்வு(ssl_version_min) {
  நிலை CURL_SSLVERSION_SSLv2:
  நிலை CURL_SSLVERSION_SSLv3:
    திரும்பு CURLE_NOT_BUILT_IN;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  நிலை CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  நிலை CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  நிலை CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  நிலை CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  நிலை CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    result = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    எனில்(result)
      திரும்பு result;
    நிறுத்து;

  இயல்பு:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
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
  எனில்(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = வெற்று;
  எனில்(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    எனில்(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(data, "failed setting cipher list: %s", ciphers);
      திரும்பு CURLE_SSL_CIPHER;
    }
    infof(data, "Cipher selection: %s", ciphers);
  }

#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    மாறிலி எழுத்து *ciphers13 = conn_config->cipher_list13;
    எனில்(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      எனில்(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(data, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        திரும்பு CURLE_SSL_CIPHER;
      }
      infof(data, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#endif

  எனில்(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    result = client_cert(data, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->சாவி, conn_config->சாவி_blob,
                         conn_config->சாவி_type,
                         conn_config->சாவி_passwd);
    எனில்(result)
      /* failf() is already done in client_cert() */
      திரும்பு result;
  }

#ifdef HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#endif

  {
    மாறிலி எழுத்து *curves = conn_config->curves;
    எனில்(curves) {
#ifdef HAVE_BORINGSSL_LIKE
#define OSSL_CURVE_CAST(x) (x)
#இல்லையெனில்
#define OSSL_CURVE_CAST(x) (எழுத்து *)CURL_UNCONST(x)
#endif
      எனில்(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(data, "failed setting curves list: '%s'", curves);
        திரும்பு CURLE_SSL_CIPHER;
      }
    }
  }

#ifdef HAVE_SSL_CTX_SET1_SIGALGS
#define OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    மாறிலி எழுத்து *signature_algorithms = conn_config->signature_algorithms;
    எனில்(signature_algorithms) {
      எனில்(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(data, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        திரும்பு CURLE_SSL_CIPHER;
      }
    }
  }
#endif

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, வெற்று);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#எனில் !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  எனில்(Curl_tls_keylog_enabled()) {
    SSL_CTX_set_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#endif

  எனில்(cb_new_session) {
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
  எனில்(data->set.ssl_fsslctx) {
    கட்டமைப்பு Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    எனில்(!octx->x509_store_setup) {
      result = Curl_ssl_setup_x509_store(cf, data, octx);
      எனில்(result)
        திரும்பு result;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, data, easy_fsslctx);
    result = (*data->set.ssl_fsslctx)(data, octx->ssl_ctx,
                                      data->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    எனில்(result) {
      failf(data, "error signaled by SSL ctx callback");
      திரும்பு result;
    }
  }

  திரும்பு ossl_init_ssl(octx, cf, data, peer, alpns_requested,
                       ssl_user_data, sess_reuse_cb);
}

நிலையான CURLcode ossl_on_session_reuse(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு alpn_spec *alpns,
                                      கட்டமைப்பு Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  திரும்பு Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max);
}

வெறுமை Curl_ossl_report_handshake(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு ossl_ctx *octx)
{
#ifdef CURLVERBOSE
  எனில்(Curl_trc_is_verbose(data)) {
    முழு psigtype_nid = NID_undef;
    மாறிலி எழுத்து *negotiated_group_name = வெற்று;

#ifdef HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#எனில் OPENSSL_VERSION_எண் >= 0x30200000L
    negotiated_group_name = SSL_get0_group_name(octx->ssl);
#இல்லையெனில்
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
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)octx;
#endif /* CURLVERBOSE */
}

நிலையான CURLcode ossl_connect_step1(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  result = Curl_ossl_ctx_init(octx, cf, data, &connssl->peer,
                              connssl->alpn, வெற்று, வெற்று,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  எனில்(result)
    திரும்பு result;

  octx->bio_method = ossl_bio_cf_method_create();
  எனில்(!octx->bio_method)
    திரும்பு CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  எனில்(!bio)
    திரும்பு CURLE_OUT_OF_MEMORY;

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
#இல்லையெனில்
  SSL_set_bio(octx->ssl, bio, bio);
#endif

  எனில்(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    கட்டமைப்பு alpn_proto_buf proto;
    memset(&proto, 0, அளவு(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }

  connssl->connecting_state = ssl_connect_2;
  திரும்பு CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
நிலையான முழு ossl_trace_ech_retry_configs(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data, SSL *ssl,
                                        முழு reason)
{
  CURLcode result = CURLE_OK;
  size_t rcl = 0;
  முழு rv = 1;
#ifndef HAVE_BORINGSSL_LIKE
  எழுத்து *inner = வெற்று;
  uint8_t *rcs = வெற்று;
  எழுத்து *outer = வெற்று;
#இல்லையெனில்
  மாறிலி எழுத்து *inner = வெற்று;
  மாறிலி uint8_t *rcs = வெற்று;
  மாறிலி எழுத்து *outer = வெற்று;
  size_t out_name_len = 0;
  முழு servername_type = 0;
#endif
  NOVERBOSE((வெறுமை)reason);

  /* nothing to trace if not doing ECH */
  எனில்(!Curl_ssl_ech_enabled(cf))
    திரும்பு rv;
#ifndef HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#இல்லையெனில்
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (முழு)rcl;
#endif

  எனில்(rv && rcs) {
    எழுத்து *b64str = வெற்று;
    size_t blen = 0;

    result = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    எனில்(!result && b64str) {
      infof(data, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ifndef HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#இல்லையெனில்
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#endif
    }
  }
  இல்லையெனில்
    infof(data, "ECH: no retry_configs (rv = %d)", rv);
#ifndef HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#endif
  திரும்பு rv;
}

#endif

நிலையான CURLcode ossl_connect_step2(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  கட்டமைப்பு ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  முழு err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  எனில்(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, octx);
    எனில்(result)
      திரும்பு result;
    octx->x509_store_setup = TRUE;
  }

#எனில் !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  எனில்(Curl_tls_keylog_enabled() && !octx->keylog_done)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_done);
#endif

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  எனில்(err != 1) {
    முழு detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(data, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    எனில்(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      திரும்பு CURLE_AGAIN;
    }
    எனில்(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      திரும்பு CURLE_AGAIN;
    }
#ifdef SSL_ERROR_WANT_ASYNC
    எனில்(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      திரும்பு CURLE_AGAIN;
    }
#endif
#ifdef SSL_ERROR_WANT_RETRY_VERIFY
    எனில்(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(data, TRUE);
      திரும்பு CURLE_AGAIN;
    }
#endif
    இல்லையெனில் {
      /* untreated error */
      sslerr_t errdetail;
      எழுத்து error_buffer[256] = "";
      CURLcode result;
      long lerr;
      முழு lib;
      முழு reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      எனில்((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#ifdef SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#endif
         )) {
        result = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_result(octx->ssl);
        எனில்(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        இல்லையெனில்
          failf(data, "%s", "SSL certificate verification failed");
      }
#ifdef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      இல்லையெனில் எனில்((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        result = CURLE_SSL_CLIENTCERT;
        failf(data, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_buffer, அளவு(error_buffer)));
      }
#endif
#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
      இல்லையெனில் எனில்((lib == ERR_LIB_SSL) &&
#ifndef HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#இல்லையெனில்
              (reason == SSL_R_ECH_REJECTED)) {
#endif /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, reason);

        result = CURLE_ECH_REQUIRED;
        failf(data, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_buffer, அளவு(error_buffer)));
      }
#endif
      இல்லையெனில் {
        result = CURLE_SSL_CONNECT_ERROR;
        failf(data, "TLS connect error: %s",
              ossl_strerror(errdetail, error_buffer, அளவு(error_buffer)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      எனில்(result == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        எழுத்து extramsg[80] = "";
        முழு sockerr = SOCKERRNO;

        எனில்(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, அளவு(extramsg));
        failf(data, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      திரும்பு result;
    }
  }
  இல்லையெனில் {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(data, octx);

#எனில் defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    எனில்(Curl_ssl_ech_enabled(cf)) {
      கட்டமைப்பு ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      எழுத்து *inner = வெற்று, *outer = வெற்று;
      முழு rv;
      VERBOSE(மாறிலி எழுத்து *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      தேர்வு(rv) {
      நிலை SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        நிறுத்து;
      நிலை SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        நிறுத்து;
      நிலை SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        நிறுத்து;
      நிலை SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        நிறுத்து;
      நிலை SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        நிறுத்து;
      நிலை SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        நிறுத்து;
      நிலை SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        நிறுத்து;
      நிலை SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        நிறுத்து;
      நிலை SSL_ECH_STATUS_BAD_NAME: {
        எனில்(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        இல்லையெனில் {
          VERBOSE(status = "bad name (unexpected)");
        }
        நிறுத்து;
      }
      இயல்பு:
        VERBOSE(status = "unexpected status");
        infof(data, "ECH: unexpected status %d", rv);
      }
      infof(data, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      எனில்(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, 0);
      }
      எனில்(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(data, "ECH: ech-hard failed");
        திரும்பு CURLE_SSL_CONNECT_ERROR;
      }
    }
    இல்லையெனில் {
      infof(data, "ECH: result: status is not attempted");
    }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    எனில்(connssl->alpn) {
      மாறிலி unsigned எழுத்து *neg_protocol;
      unsigned முழு len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      திரும்பு Curl_alpn_set_negotiated(cf, data, connssl, neg_protocol, len);
    }

    திரும்பு CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
நிலையான CURLcode ossl_pkp_pin_peer_pubkey(கட்டமைப்பு Curl_easy *data, X509 *cert,
                                         மாறிலி எழுத்து *pinnedpubkey)
{
  /* Scratch */
  முழு len1 = 0, len2 = 0;
  unsigned எழுத்து *buff1 = வெற்று, *temp = வெற்று;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  எனில்(!pinnedpubkey)
    திரும்பு CURLE_OK;

  எனில்(!cert)
    திரும்பு result;

  செய் {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), வெற்று);
    எனில்(len1 < 1)
      நிறுத்து; /* failed */

    buff1 = temp = curlx_malloc(len1);
    எனில்(!buff1)
      நிறுத்து; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &temp);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    எனில்((len1 != len2) || !temp || ((temp - buff1) != len1))
      நிறுத்து; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } போது(0);

  எனில்(buff1)
    curlx_free(buff1);

  திரும்பு result;
}

#ifdef CURLVERBOSE
#எனில் !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_எண்) && LIBRESSL_VERSION_எண் < 0x3060000fL)
நிலையான வெறுமை infof_certstack(கட்டமைப்பு Curl_easy *data, மாறிலி SSL *ssl)
{
  அடுக்கு_OF(X509) *certstack;
  long verify_result;
  முழு இலக்கம்_cert_levels;
  முழு cert_level;

  எனில்(!Curl_trc_is_verbose(data))
    திரும்பு;

  verify_result = SSL_get_verify_result(ssl);
  எனில்(verify_result != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  இல்லையெனில்
    certstack = SSL_get0_verified_chain(ssl);
  எனில்(!certstack)
    திரும்பு;
  இலக்கம்_cert_levels = sk_X509_இலக்கம்(certstack);

  க்கு(cert_level = 0; cert_level < இலக்கம்_cert_levels; cert_level++) {
    எழுத்து cert_algorithm[80] = "";
    எழுத்து group_name_final[80] = "";
    மாறிலி X509_ALGOR *palg_cert = வெற்று;
    மாறிலி ASN1_OBJECT *paobj_cert = வெற்று;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    முழு சாவி_bits;
    முழு சாவி_sec_bits;
    முழு get_group_name;
    மாறிலி எழுத்து *type_name;

    current_cert = sk_X509_மதிப்பு(certstack, cert_level);
    எனில்(!current_cert)
      தொடர்;

    current_pkey = X509_get0_pubkey(current_cert);
    எனில்(!current_pkey)
      தொடர்;

    X509_get0_signature(வெற்று, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, வெற்று, வெற்று, palg_cert);
    OBJ_obj2txt(cert_algorithm, அளவு(cert_algorithm), paobj_cert, 0);

    சாவி_bits = EVP_PKEY_bits(current_pkey);
#ifndef HAVE_OPENSSL3
#define EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#endif
    சாவி_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#ifdef HAVE_OPENSSL3
    {
      எழுத்து group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               அளவு(group_name), வெற்று);
      curl_msnprintf(group_name_final, அளவு(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#இல்லையெனில்
    get_group_name = 0;
    type_name = வெற்று;
#endif

    infof(data, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          சாவி_bits, சாவி_sec_bits, cert_algorithm);
  }
}
#இல்லையெனில்
#define infof_certstack(data, ssl)
#endif
#endif /* CURLVERBOSE */

நிலையான CURLcode ossl_check_issuer(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  X509 *server_cert)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = வெற்று;
  BIO *fp = வெற்று;
  எழுத்து err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode result = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  எனில்(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->data,
                         (முழு)conn_config->issuercert_blob->len);
    எனில்(!fp) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, அளவு(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }
  }
  இல்லையெனில் எனில்(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    எனில்(!fp) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, அளவு(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      செல் out;
    }

    எனில்(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      எனில்(verify_enabled)
        failf(data, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      செல் out;
    }
  }

  எனில்(fp) {
    issuer = PEM_read_bio_X509(fp, வெற்று, ZERO_NULL, வெற்று);
    எனில்(!issuer) {
      எனில்(verify_enabled)
        failf(data, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      செல் out;
    }

    எனில்(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      எனில்(verify_enabled)
        failf(data, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      செல் out;
    }

    infof(data, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  எனில்(fp)
    BIO_free(fp);
  எனில்(issuer)
    X509_free(issuer);
  திரும்பு result;
}

நிலையான மாறிலி எழுத்து *pinned(கட்டமைப்பு Curl_cfilter *cf,
                          கட்டமைப்பு Curl_easy *data)
{
  (வெறுமை)cf;
  திரும்பு
#ifndef CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#endif
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
}

நிலையான CURLcode ossl_check_pinned_சாவி(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      X509 *server_cert)
{
  CURLcode result = CURLE_OK;
  மாறிலி எழுத்து *ptr = pinned(cf, data);
  எனில்(ptr) {
    result = ossl_pkp_pin_peer_pubkey(data, server_cert, ptr);
    எனில்(result)
      failf(data, "SSL: public key does not match pinned public key");
  }
  திரும்பு result;
}

#ifdef CURLVERBOSE
#define MAX_CERT_NAME_LENGTH 2048
நிலையான CURLcode ossl_infof_cert(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                X509 *server_cert)
{
  BIO *mem = வெற்று;
  கட்டமைப்பு dynbuf dname;
  எழுத்து err_buf[256] = "";
  எழுத்து *buf;
  long len;
  CURLcode result = CURLE_OK;

  எனில்(!Curl_trc_is_verbose(data))
    திரும்பு CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_LENGTH);
  mem = BIO_new(BIO_s_mem());
  எனில்(!mem) {
    failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, அளவு(err_buf)));
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  infof(data, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  result = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(data, "  subject: %s", result ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_data(mem, (எழுத்து **)&buf);
  infof(data, "  start date: %.*s", (முழு)len, buf);
  (வெறுமை)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_data(mem, (எழுத்து **)&buf);
  infof(data, "  expire date: %.*s", (முழு)len, buf);
  (வெறுமை)BIO_reset(mem);

  result = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  எனில்(result) /* should be only fatal stuff like OOM */
    செல் out;
  infof(data, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  திரும்பு result;
}
#endif /* CURLVERBOSE */

#ifdef USE_APPLE_SECTRUST
கட்டமைப்பு ossl_certs_ctx {
  அடுக்கு_OF(X509) *sk;
  size_t இலக்கம்_certs;
  unsigned எழுத்து *last_der;
};

நிலையான CURLcode ossl_chain_get_der(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   வெறுமை *user_data,
                                   size_t i,
                                   unsigned எழுத்து **pder,
                                   size_t *pder_len)
{
  கட்டமைப்பு ossl_certs_ctx *chain = user_data;
  X509 *cert;
  முழு der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = வெற்று;

  (வெறுமை)cf;
  (வெறுமை)data;
  *pder_len = 0;
  *pder = வெற்று;

  எனில்(i >= chain->இலக்கம்_certs)
    திரும்பு CURLE_TOO_LARGE;
  cert = sk_X509_மதிப்பு(chain->sk, (முழு)i);
  எனில்(!cert)
    திரும்பு CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  எனில்(der_len < 0)
    திரும்பு CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (size_t)der_len;
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ossl_apple_verify(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ossl_ctx *octx,
                                  கட்டமைப்பு ssl_peer *peer)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு ossl_certs_ctx chain;
  CURLcode result;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, அளவு(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.இலக்கம்_certs = chain.sk ? sk_X509_இலக்கம்(chain.sk) : 0;

  எனில்(!chain.இலக்கம்_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    எனில்(!octx->reused_session) {
      failf(data, "SSL: could not get peer certificate chain");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    இல்லையெனில் {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(data, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      திரும்பு CURLE_OK;
    }
  }
  இல்லையெனில் {
#ifdef HAVE_BORINGSSL_LIKE
    மாறிலி uint8_t *ocsp_data = வெற்று;
#இல்லையெனில்
    unsigned எழுத்து *ocsp_data = வெற்று;
#endif
    long ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    எனில்(!octx->reused_session)
      ocsp_len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_data);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    எனில்(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_data = வெற்று;
    }

    result = Curl_vtls_apple_verify(cf, data, peer, chain.இலக்கம்_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_data, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = வெற்று;
  }
  octx->sectrust_verified = !result;
  திரும்பு result;
}
#endif /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ossl_ctx *octx,
                                   கட்டமைப்பு ssl_peer *peer)
{
  கட்டமைப்பு connectdata *conn = cf->conn;
  கட்டமைப்பு ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  long ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  எனில்(data->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    result = ossl_certchain(data, octx->ssl);
    எனில்(result)
      திரும்பு result;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  எனில்(!server_cert) {
    /* no verification at all, this maybe acceptable */
    எனில்(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, data))
      செல் out;

    failf(data, "SSL: could not get peer certificate");
    result = CURLE_PEER_FAILED_VERIFICATION;
    செல் out;
  }

#ifdef CURLVERBOSE
  result = ossl_infof_cert(cf, data, server_cert);
  எனில்(result)
    செல் out;
  infof_certstack(data, octx->ssl);
#endif

  எனில்(conn_config->verifyhost) {
    result = ossl_verifyhost(data, conn, peer, server_cert);
    எனில்(result)
      செல் out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_result(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(data, "OpenSSL verify result: %lx", (unsigned long)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  எனில்(verified)
    infof(data, "SSL certificate verified via OpenSSL.");

#ifdef USE_APPLE_SECTRUST
  எனில்(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    result = ossl_apple_verify(cf, data, octx, peer);
    எனில்(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      செல் out; /* unexpected error */
    எனில்(octx->sectrust_verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#endif

  எனில்(!verified) {
    /* no trust established, report the OpenSSL status */
    எனில்(conn_config->verifypeer) {
      failf(data, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      result = CURLE_PEER_FAILED_VERIFICATION;
      செல் out;
    }
    infof(data, " SSL certificate verification failed, continuing anyway!");
  }

#ifndef OPENSSL_NO_OCSP
  எனில்(conn_config->verifystatus &&
#ifdef USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, data)) &&
#endif
     !octx->reused_session) {
    result = verifystatus(cf, data, octx);
    எனில்(result)
      செல் out;
  }
#endif

  result = ossl_check_issuer(cf, data, server_cert);
  எனில்(result)
    செல் out;

  result = ossl_check_pinned_சாவி(cf, data, server_cert);

out:
  X509_free(server_cert);
  திரும்பு result;
}

நிலையான CURLcode ossl_connect_step3(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  result = Curl_ossl_check_peer_cert(cf, data, octx, &connssl->peer);
  எனில்(result)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, data, connssl->peer.scache_சாவி);

  திரும்பு result;
}

#ifdef HAVE_OPENSSL_EARLYDATA
நிலையான CURLcode ossl_send_earlydata(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  மாறிலி unsigned எழுத்து *buf;
  size_t blen, nwritten;
  முழு rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_result = CURLE_OK;
  போது(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_data(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    எனில்(rc <= 0) {
      long sslerror;
      எழுத்து error_buffer[256];
      முழு err = SSL_get_error(octx->ssl, rc);

      தேர்வு(err) {
      நிலை SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        result = CURLE_AGAIN;
        செல் out;
      நிலை SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_AGAIN;
        செல் out;
      நிலை SSL_ERROR_SYSCALL: {
        முழு sockerr = SOCKERRNO;

        எனில்(octx->io_result == CURLE_AGAIN) {
          result = CURLE_AGAIN;
          செல் out;
        }
        sslerror = ERR_get_error();
        எனில்(sslerror)
          ossl_strerror(sslerror, error_buffer, அளவு(error_buffer));
        இல்லையெனில் எனில்(sockerr)
          curlx_strerror(sockerr, error_buffer, அளவு(error_buffer));
        இல்லையெனில்
          curl_msnprintf(error_buffer, அளவு(error_buffer), "%s",
                         SSL_ERROR_to_str(err));

        failf(data, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_SEND_ERROR;
        செல் out;
      }
      நிலை SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(data, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_buffer, அளவு(error_buffer)));
        result = CURLE_SEND_ERROR;
        செல் out;
      }
      இயல்பு:
        /* a true error */
        failf(data, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        result = CURLE_SEND_ERROR;
        செல் out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  திரும்பு result;
}
#endif /* HAVE_OPENSSL_EARLYDATA */

நிலையான CURLcode ossl_connect(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             bool *done)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  எனில்(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  எனில்(connssl->connecting_state == ssl_connect_1) {
    எனில்(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
      திரும்பு CURLE_OK;
    }
    CURL_TRC_CF(data, cf, "ossl_connect, step1");
    result = ossl_connect_step1(cf, data);
    எனில்(result)
      செல் out;
  }

  எனில்(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(data, cf, "ossl_connect, step2");
#ifdef HAVE_OPENSSL_EARLYDATA
    எனில்(connssl->earlydata_state == ssl_earlydata_await) {
      செல் out;
    }
    இல்லையெனில் எனில்(connssl->earlydata_state == ssl_earlydata_sending) {
      result = ossl_send_earlydata(cf, data);
      எனில்(result)
        செல் out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#endif
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    result = ossl_connect_step2(cf, data);
    எனில்(result)
      செல் out;
  }

  எனில்(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(data, cf, "ossl_connect, step3");
    result = ossl_connect_step3(cf, data);
    எனில்(result)
      செல் out;
    connssl->connecting_state = ssl_connect_done;
#ifdef HAVE_OPENSSL_EARLYDATA
    எனில்(connssl->earlydata_state > ssl_earlydata_none) {
      கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_data_status(octx->ssl) == SSL_EARLY_DATA_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
  }

  எனில்(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(data, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  எனில்(result == CURLE_AGAIN) {
    *done = FALSE;
    திரும்பு CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  திரும்பு result;
}

நிலையான bool ossl_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                              மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  (வெறுமை)data;
  திரும்பு (bool)connssl->உள்ளீடு_pending;
}

நிலையான CURLcode ossl_send(கட்டமைப்பு Curl_cfilter *cf,
                          கட்டமைப்பு Curl_easy *data,
                          மாறிலி வெறுமை *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  முழு err;
  எழுத்து error_buffer[256];
  sslerr_t sslerror;
  முழு memlen;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  முழு nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)INT_MAX) ? INT_MAX : (முழு)len;
  எனில்(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    எனில்(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  எனில்(nwritten > 0) {
    எனில்(nwritten > memlen) {
      failf(data, "SSL_write(len=%d) returned %d", memlen, nwritten);
      result = CURLE_SEND_ERROR;
      செல் out;
    }
    *pnwritten = (size_t)nwritten;
  }
  இல்லையெனில் {
    err = SSL_get_error(octx->ssl, nwritten);

    தேர்வு(err) {
    நிலை SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      result = CURLE_AGAIN;
      செல் out;
    நிலை SSL_ERROR_WANT_WRITE:
      result = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      செல் out;
    நிலை SSL_ERROR_SYSCALL: {
      முழு sockerr = SOCKERRNO;

      எனில்(octx->io_result == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        result = CURLE_AGAIN;
        செல் out;
      }
      sslerror = ERR_get_error();
      எனில்(sslerror)
        ossl_strerror(sslerror, error_buffer, அளவு(error_buffer));
      இல்லையெனில் எனில்(sockerr)
        curlx_strerror(sockerr, error_buffer, அளவு(error_buffer));
      இல்லையெனில்
        curl_msnprintf(error_buffer, அளவு(error_buffer), "%s",
                       SSL_ERROR_to_str(err));

      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_buffer, sockerr);
      result = CURLE_SEND_ERROR;
      செல் out;
    }
    நிலை SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(data, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_buffer, அளவு(error_buffer)));
      result = CURLE_SEND_ERROR;
      செல் out;
    }
    இயல்பு:
      /* a true error */
      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      result = CURLE_SEND_ERROR;
      செல் out;
    }
  }

out:
  திரும்பு result;
}

நிலையான CURLcode ossl_recv(கட்டமைப்பு Curl_cfilter *cf,
                          கட்டமைப்பு Curl_easy *data,   /* transfer */
                          எழுத்து *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  எழுத்து error_buffer[256];
  unsigned long sslerror;
  முழு buffsize;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  முழு nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)INT_MAX) ? INT_MAX : (முழு)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  எனில்(nread > 0)
    *pnread = (size_t)nread;
  இல்லையெனில் {
    /* failed SSL_read */
    முழு err = SSL_get_error(octx->ssl, nread);

    தேர்வு(err) {
    நிலை SSL_ERROR_NONE: /* this is not an error */
      நிறுத்து;
    நிலை SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      எனில்(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(data, cf, "TLS close_notify");
      நிறுத்து;
    நிலை SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      செல் out;
    நிலை SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      செல் out;
    இயல்பு:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      எனில்(octx->io_result == CURLE_AGAIN) {
        result = CURLE_AGAIN;
        செல் out;
      }
      sslerror = ERR_get_error();
      எனில்((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        முழு sockerr = SOCKERRNO;
        எனில்(sslerror)
          ossl_strerror(sslerror, error_buffer, அளவு(error_buffer));
        இல்லையெனில் எனில்(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_buffer, அளவு(error_buffer));
        இல்லையெனில்
          curl_msnprintf(error_buffer, அளவு(error_buffer), "%s",
                         SSL_ERROR_to_str(err));
        failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_RECV_ERROR;
        செல் out;
      }
      இல்லையெனில் எனில்(err == SSL_ERROR_SYSCALL) {
        எனில்(octx->io_result) {
          /* logging handling in underlying filter already */
          result = octx->io_result;
        }
        இல்லையெனில் எனில்(connssl->peer_closed) {
          failf(data, "Connection closed abruptly");
          result = CURLE_RECV_ERROR;
        }
        இல்லையெனில் {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          முழு sockerr = SOCKERRNO;
          எனில்(sockerr)
            curlx_strerror(sockerr, error_buffer, அளவு(error_buffer));
          இல்லையெனில் {
            curl_msnprintf(error_buffer, அளவு(error_buffer),
                           "Connection closed abruptly");
          }
          failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_buffer, sockerr);
          result = CURLE_RECV_ERROR;
        }
        செல் out;
      }
    }
  }

out:
  எனில்((!result && !*pnread) || (result == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->உள்ளீடு_pending = FALSE;
  }
  CURL_TRC_CF(data, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (முழு)result, *pnread, connssl->உள்ளீடு_pending);
  திரும்பு result;
}

நிலையான CURLcode ossl_get_channel_binding(கட்டமைப்பு Curl_easy *data,
                                         int8_t sockindex,
                                         கட்டமைப்பு dynbuf *binding)
{
  X509 *cert;
  முழு mdnid;
  bool no_digest_acceptable = FALSE;
  மாறிலி EVP_MD *algo_type = வெற்று;
  மாறிலி எழுத்து *algo_name = வெற்று;
  unsigned முழு length;
  unsigned எழுத்து buf[EVP_MAX_MD_SIZE];

  நிலையான மாறிலி எழுத்து prefix[] = "tls-server-end-point:";
  கட்டமைப்பு connectdata *conn = data->conn;
  கட்டமைப்பு Curl_cfilter *cf = conn->cfilter[sockindex];
  கட்டமைப்பு ossl_ctx *octx = வெற்று;
  CURLcode result = CURLE_OK;

  செய் {
    மாறிலி கட்டமைப்பு Curl_cftype *cft = cf->cft;
    கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;

    எனில்(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
      நிறுத்து;
    }

    cf = cf->next;
  } போது(cf);

  எனில்(!octx) {
    failf(data, "Failed to find the SSL filter");
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  எனில்(!cert)
    /* No server certificate, do not do channel binding */
    திரும்பு CURLE_OK;

#ifdef HAVE_OPENSSL3
  {
    முழு pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    எனில்(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(data, "certificate signature algorithm not recognized");
      result = CURLE_SSL_INVALIDCERTSTATUS;
      செல் out;
    }

    எனில்(mdnid != NID_undef) {
      எனில்(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      இல்லையெனில்
        algo_type = EVP_get_digestbynid(mdnid);
    }
    இல்லையெனில் எனில்(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    இல்லையெனில் எனில்(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      எழுத்து mdname[128] = "";
      முழு rc = EVP_PKEY_get_default_digest_name(pkey, mdname, அளவு(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      எனில்(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      இல்லையெனில் எனில்(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(data, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#இல்லையெனில் /* HAVE_OPENSSL3 */

  எனில்(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, வெற்று)) {
    failf(data,
          "Unable to find digest NID for certificate signature algorithm");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  எனில்(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  இல்லையெனில் {
    algo_type = EVP_get_digestbynid(mdnid);
    எனில்(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(data, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      result = CURLE_SSL_INVALIDCERTSTATUS;
      செல் out;
    }
  }

#endif /* HAVE_OPENSSL3, else */

  எனில்(!algo_type) {
    எனில்(no_digest_acceptable) {
      infof(data, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      result = CURLE_OK;
      செல் out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(data, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் out;
  }

  எனில்(!X509_digest(cert, algo_type, buf, &length)) {
    failf(data, "X509_digest() failed for channel binding");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    செல் out;
  }

  /* Append "tls-server-end-point:" */
  result = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  எனில்(result)
    செல் out;

  /* Append digest */
  result = curlx_dyn_addn(binding, buf, length);

out:
  X509_free(cert);
  திரும்பு result;
}

size_t Curl_ossl_version(எழுத்து *buffer, size_t size)
{
#ifdef LIBRESSL_VERSION_எண்
  எழுத்து *p;
  size_t மொத்தம்;
  மாறிலி எழுத்து *ver = OpenSSL_version(OPENSSL_VERSION);
  நிலையான மாறிலி எழுத்து expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  எனில்(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  மொத்தம் = curl_msnprintf(buffer, size, "%s/%s", OSSL_PACKAGE, ver);
  க்கு(p = buffer; *p; ++p) {
    எனில்(ISBLANK(*p))
      *p = '_';
  }
  திரும்பு மொத்தம்;
#அல்லதெனில் defined(OPENSSL_IS_AWSLC)
  திரும்பு curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_எண்_STRING);
#அல்லதெனில் defined(OPENSSL_IS_BORINGSSL)
#ifdef CURL_BORINGSSL_VERSION
  திரும்பு curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#இல்லையெனில்
  திரும்பு curl_msnprintf(buffer, size, OSSL_PACKAGE);
#endif
#இல்லையெனில் /* OpenSSL 3+ */
  திரும்பு curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
}

/* can be called with data == NULL */
நிலையான CURLcode ossl_random(கட்டமைப்பு Curl_easy *data,
                            unsigned எழுத்து *entropy, size_t length)
{
  முழு rc;
  எனில்(data) {
    எனில்(ossl_seed(data)) /* Initiate the seed if not already done */
      திரும்பு CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  இல்லையெனில் {
    எனில்(!rand_enough())
      திரும்பு CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(length));
  திரும்பு rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

நிலையான CURLcode ossl_sha256sum(மாறிலி unsigned எழுத்து *உள்ளீடு,
                               size_t len,
                               unsigned எழுத்து *sha256sum /* output */,
                               size_t unused)
{
  CURLcode result = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (வெறுமை)unused;

  mdctx = EVP_MD_CTX_new();
  எனில்(!mdctx)
    திரும்பு CURLE_OUT_OF_MEMORY;
  எனில்(!EVP_DigestInit_ex(mdctx, EVP_sha256(), வெற்று)) {
    result = CURLE_FAILED_INIT;
    செல் out;
  }
  எனில்(!EVP_DigestUpdate(mdctx, உள்ளீடு, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, வெற்று))
    result = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  திரும்பு result;
}

நிலையான bool ossl_cert_status_request(வெறுமை)
{
#ifndef OPENSSL_NO_OCSP
  திரும்பு TRUE;
#இல்லையெனில்
  திரும்பு FALSE;
#endif
}

நிலையான வெறுமை *ossl_get_internals(கட்டமைப்பு ssl_connect_data *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  கட்டமைப்பு ossl_ctx *octx = (கட்டமைப்பு ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  திரும்பு info == CURLINFO_TLS_SESSION ?
    (வெறுமை *)octx->ssl_ctx : (வெறுமை *)octx->ssl;
}

மாறிலி கட்டமைப்பு Curl_ssl Curl_ssl_openssl = {
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

  அளவு(கட்டமைப்பு ossl_ctx),

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
