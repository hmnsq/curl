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

#ifdef USE_OPENSSL

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
#ifdef USE_ECH
#شامل "curlx/base64.h"
#endif

#شامل <openssl/rand.h>
#شامل <openssl/x509v3.h>
#ifndef OPENSSL_NO_DSA
#شامل <openssl/dsa.h>
#endif
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
#endif

#ifndef OPENSSL_NO_OCSP
#شامل <openssl/ocsp.h>
#endif

#اگر !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#define USE_OPENSSL_ENGINE
#شامل <openssl/engine.h>
#endif

#ifdef LIBRESSL_VERSION_عدد
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  اگر LIBRESSL_VERSION_عدد < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#وگرنهاگر !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#endif

#اگر defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#شامل <openssl/provider.h>
#شامل <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#define OPENSSL_HAS_PROVIDERS
#endif

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#اگر !defined(LIBRESSL_VERSION_عدد) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_کلید_EXPLICIT_PARAMS))
#define HAVE_SSL_CTX_SET_DEFAULT_READ_BUFFER_LEN 1
#endif

#اگر defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#شامل <openssl/ui.h>
#endif

#ifdef HAVE_OPENSSL3
#define HAVE_EVP_PKEY_GET_PARAMS 1
#endif

#ifdef HAVE_EVP_PKEY_GET_PARAMS
#شامل <openssl/core_names.h>
#define DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = پوچ
#define FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#وگرنه
#define DECLARE_PKEY_PARAM_BIGNUM(name) ثابت BIGNUM *name
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
#ifndef LIBRESSL_VERSION_عدد
#define HAVE_SSL_CTX_SET1_SIGALGS
#endif

#ifdef LIBRESSL_VERSION_عدد
#define OSSL_PACKAGE "LibreSSL"
#وگرنهاگر defined(OPENSSL_IS_AWSLC)
#define OSSL_PACKAGE "AWS-LC"
#وگرنهاگر defined(OPENSSL_IS_BORINGSSL)
#define OSSL_PACKAGE "BoringSSL"
#وگرنهاگر defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#define OSSL_PACKAGE "quictls"
#وگرنه
#define OSSL_PACKAGE "OpenSSL"
#endif

#ifdef HAVE_BORINGSSL_LIKE
تعریفنوع size_t numcert_t;
تعریفنوع uint32_t sslerr_t;
#وگرنه
تعریفنوع صحیح numcert_t;
تعریفنوع unsigned long sslerr_t;
#endif
#define ossl_valsize_t numcert_t

#اگر OPENSSL_VERSION_عدد >= 0x40100000L
#define ossl_ASN1_STRING_get_length ASN1_STRING_get_length
#وگرنه
ایستا size_t ossl_ASN1_STRING_get_length(ثابت ASN1_STRING *str)
{
   ثابت صحیح length = ASN1_STRING_length(str);
   بازگشت length >= 0 ? (size_t)length : 0;
}
#endif

ایستا CURLcode افزودن_certinfo(ساختار Curl_easy *data,
                              BIO *mem, ثابت نویسه *label, صحیح رقم)
  WARN_UNUSED_RESULT;

ایستا CURLcode افزودن_certinfo(ساختار Curl_easy *data,
                              BIO *mem, ثابت نویسه *label, صحیح رقم)
{
  نویسه *ptr;
  long len = BIO_get_mem_data(mem, &ptr);
  CURLcode result = Curl_ssl_افزودن_certinfo_len(data, رقم, label, ptr, len);
  (تهی)BIO_reset(mem);
  بازگشت result;
}

ایستا CURLcode pubkey_show(ساختار Curl_easy *data,
                            BIO *mem,
                            صحیح رقم,
                            ثابت نویسه *type,
                            ثابت نویسه *name,
                            ثابت BIGNUM *bn) WARN_UNUSED_RESULT;

ایستا CURLcode pubkey_show(ساختار Curl_easy *data,
                            BIO *mem,
                            صحیح رقم,
                            ثابت نویسه *type,
                            ثابت نویسه *name,
                            ثابت BIGNUM *bn)
{
  نویسه namebuf[32];

  curl_msnprintf(namebuf, اندازه(namebuf), "%s(%s)", type, name);

  اگر(bn)
    BN_print(mem, bn);
  بازگشت افزودن_certinfo(data, mem, namebuf, رقم);
}

#define print_pubkey_BN(_type, _name, رقم_2)           \
  pubkey_show(data, mem, رقم_2, #_type, #_name, _name)

ایستا صحیح asn1_object_dump(ثابت ASN1_OBJECT *a, نویسه *buf, size_t len)
{
  صحیح i = i2t_ASN1_OBJECT(buf, (صحیح)len, a);
  بازگشت (i >= (صحیح)len);  /* buffer too small */
}

ایستا CURLcode X509V3_ext(ساختار Curl_easy *data,
                           صحیح certnum,
                           ثابت پشته_OF(X509_EXTENSION) *extsarg)
{
  صحیح i;
  CURLcode result = CURLE_OK;
#ifdef LIBRESSL_VERSION_عدد
  پشته_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#وگرنه
  ثابت پشته_OF(X509_EXTENSION) *exts = extsarg;
#endif

  اگر((صحیح)sk_X509_EXTENSION_رقم(exts) <= 0)
    /* no extensions, bail out */
    بازگشت result;

  برای(i = 0; i < (صحیح)sk_X509_EXTENSION_رقم(exts); i++) {
    ثابت ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_مقدار(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    نویسه namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    اگر(!bio_out)
      بازگشت result;

    obj = X509_EXTENSION_get_object(ext);

    اگر(asn1_object_dump(obj, namebuf, اندازه(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    اگر(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (ثابت ASN1_STRING *)X509_EXTENSION_get_data(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    result = Curl_ssl_افزودن_certinfo_len(data, certnum, namebuf, biomem->data,
                                        biomem->length);
    BIO_free(bio_out);
    اگر(result)
      بشکن;
  }
  بازگشت result;
}

ایستا CURLcode get_pkey_rsa(ساختار Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, صحیح i)
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
#وگرنه
  RSA_get0_کلید(rsa, &n, &e, پوچ);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (صحیح)(n ? BN_رقم_bits(n) : 0));
  result = افزودن_certinfo(data, mem, "RSA Public Key", i);
  اگر(!result) {
    result = print_pubkey_BN(rsa, n, i);
    اگر(!result)
      result = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  بازگشت result;
}

#ifndef OPENSSL_NO_DSA
ایستا CURLcode get_pkey_dsa(ساختار Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, صحیح i)
{
  CURLcode result = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_کلید);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_کلید, &pub_کلید);
#وگرنه
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_کلید(dsa, &pub_کلید, پوچ);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dsa, p, i);
  اگر(!result)
    result = print_pubkey_BN(dsa, q, i);
  اگر(!result)
    result = print_pubkey_BN(dsa, g, i);
  اگر(!result)
    result = print_pubkey_BN(dsa, pub_کلید, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_کلید);
  بازگشت result;
}
#endif /* !OPENSSL_NO_DSA */

ایستا CURLcode get_pkey_dh(ساختار Curl_easy *data,
                            EVP_PKEY *pubkey, BIO *mem, صحیح i)
{
  CURLcode result;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_کلید);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_کلید, &pub_کلید);
#وگرنه
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_کلید(dh, &pub_کلید, پوچ);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dh, p, i);
  اگر(!result)
    result = print_pubkey_BN(dh, q, i);
  اگر(!result)
    result = print_pubkey_BN(dh, g, i);
  اگر(!result)
    result = print_pubkey_BN(dh, pub_کلید, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_کلید);
  بازگشت result;
}

#ifdef HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
تعریفنوع ثابت X509_PUBKEY pubkeytype_t;
#وگرنه
تعریفنوع X509_PUBKEY pubkeytype_t;
#endif

ایستا CURLcode ossl_certchain(ساختار Curl_easy *data, SSL *ssl)
{
  CURLcode result;
  پشته_OF(X509) *sk;
  صحیح i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  اگر(!sk)
    بازگشت CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_رقم(sk);
  اگر(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(data, "%d certificates is more than allowed (%d)", (صحیح)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  result = Curl_ssl_init_certinfo(data, (صحیح)numcerts);
  اگر(result)
    بازگشت result;

  mem = BIO_new(BIO_s_mem());
  اگر(!mem)
    result = CURLE_OUT_OF_MEMORY;

  برای(i = 0; !result && (i < (صحیح)numcerts); i++) {
    X509 *x = sk_X509_مقدار(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = پوچ;
    size_t j;
    ثابت ASN1_BIT_STRING *psig = پوچ;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    result = افزودن_certinfo(data, mem, "Subject", i);
    اگر(result)
      بشکن;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    result = افزودن_certinfo(data, mem, "Issuer", i);
    اگر(result)
      بشکن;

    BIO_printf(mem, "%lx", (unsigned long)X509_get_version(x));
    result = افزودن_certinfo(data, mem, "Version", i);
    اگر(result)
      بشکن;

    {
      ASN1_INTEGER *رقم = X509_get_serial_عدد(x);
      ثابت unsigned نویسه *numdata = ASN1_STRING_get0_data(رقم);
      ثابت size_t numlen = ossl_ASN1_STRING_get_length(رقم);
      اگر(ASN1_STRING_type(رقم) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      برای(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    result = افزودن_certinfo(data, mem, "Serial Number", i);
    اگر(result)
      بشکن;

    {
      ثابت X509_ALGOR *sigalg = پوچ;
      pubkeytype_t *xpubkey = پوچ;
      ASN1_OBJECT *pubkeyoid = پوچ;

      X509_get0_signature(&psig, &sigalg, x);
      اگر(sigalg) {
        ثابت ASN1_OBJECT *sigalgoid = پوچ;
        X509_ALGOR_get0(&sigalgoid, پوچ, پوچ, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        result = افزودن_certinfo(data, mem, "Signature Algorithm", i);
        اگر(result)
          بشکن;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      اگر(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, پوچ, پوچ, پوچ, xpubkey);
        اگر(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          result = افزودن_certinfo(data, mem, "Public Key Algorithm", i);
          اگر(result)
            بشکن;
        }
      }

      result = X509V3_ext(data, i, X509_get0_extensions(x));
      اگر(result)
        بشکن;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    result = افزودن_certinfo(data, mem, "Start date", i);
    اگر(result)
      بشکن;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    result = افزودن_certinfo(data, mem, "Expire date", i);
    اگر(result)
      بشکن;

    pubkey = X509_get_pubkey(x);
    اگر(!pubkey)
      infof(data, "   Unable to load public key");
    وگرنه {
      انتخاب(EVP_PKEY_id(pubkey)) {
      حالت EVP_PKEY_RSA:
        result = get_pkey_rsa(data, pubkey, mem, i);
        بشکن;

#ifndef OPENSSL_NO_DSA
      حالت EVP_PKEY_DSA:
        result = get_pkey_dsa(data, pubkey, mem, i);
        بشکن;
#endif

      حالت EVP_PKEY_DH:
        result = get_pkey_dh(data, pubkey, mem, i);
        بشکن;
      }
      EVP_PKEY_free(pubkey);
    }

    اگر(!result && psig) {
      ثابت unsigned نویسه *psigdata = ASN1_STRING_get0_data(psig);
      ثابت size_t psiglen = ossl_ASN1_STRING_get_length(psig);
      برای(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      result = افزودن_certinfo(data, mem, "Signature", i);
    }

    اگر(!result) {
      PEM_write_bio_X509(mem, x);
      result = افزودن_certinfo(data, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  اگر(result)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(data);

  بازگشت result;
}

ایستا صحیح ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_data(bio, پوچ);
  بازگشت 1;
}

ایستا صحیح ossl_bio_cf_destroy(BIO *bio)
{
  اگر(!bio)
    بازگشت 0;
  بازگشت 1;
}

ایستا long ossl_bio_cf_ctrl(BIO *bio, صحیح cmd, long رقم, تهی *ptr)
{
  ساختار Curl_cfilter *cf = BIO_get_data(bio);
  long ret = 1;

  (تهی)cf;
  (تهی)ptr;
  انتخاب(cmd) {
  حالت BIO_CTRL_GET_CLOSE:
    ret = (long)BIO_get_shutdown(bio);
    بشکن;
  حالت BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (صحیح)رقم);
    بشکن;
  حالت BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    بشکن;
  حالت BIO_CTRL_DUP:
    ret = 1;
    بشکن;
  حالت BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    ساختار ssl_connect_data *connssl = cf->ctx;
    بازگشت connssl->peer_closed;
  }
  پیشفرض:
    ret = 0;
    بشکن;
  }
  بازگشت ret;
}

ایستا صحیح ossl_bio_cf_out_write(BIO *bio, ثابت نویسه *buf, صحیح blen)
{
  ساختار Curl_cfilter *cf = BIO_get_data(bio);
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  اگر(blen < 0)
    بازگشت 0;

  result = Curl_conn_cf_send(cf->next, data,
                             (ثابت uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (صحیح)result, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  اگر(result) {
    اگر(result == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    بازگشت -1;
  }
  بازگشت (صحیح)nwritten;
}

ایستا صحیح ossl_bio_cf_in_read(BIO *bio, نویسه *buf, صحیح blen)
{
  ساختار Curl_cfilter *cf = BIO_get_data(bio);
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result, r2;

  DEBUGASSERT(data);
  /* OpenSSL catches this case, so should we. */
  اگر(!buf)
    بازگشت 0;
  اگر(blen < 0)
    بازگشت 0;

  result = Curl_conn_cf_recv(cf->next, data, buf, (size_t)blen, &nread);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (صحیح)result, nread);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  اگر(result) {
    اگر(result == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  وگرنه {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->درون_داد_pending = TRUE;
    اگر(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  اگر(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, data, octx);
    اگر(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_result = r2;
      بازگشت -1;
    }
    octx->x509_store_setup = TRUE;
  }
  بازگشت result ? -1 : (صحیح)nread;
}

ایستا BIO_METHOD *ossl_bio_cf_method_create(تهی)
{
  BIO_METHOD *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  اگر(m) {
    BIO_meth_set_write(m, &ossl_bio_cf_out_write);
    BIO_meth_set_read(m, &ossl_bio_cf_in_read);
    BIO_meth_set_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_set_create(m, &ossl_bio_cf_create);
    BIO_meth_set_destroy(m, &ossl_bio_cf_destroy);
  }
  بازگشت m;
}

ایستا تهی ossl_bio_cf_method_free(BIO_METHOD *m)
{
  اگر(m)
    BIO_meth_free(m);
}

#ifndef HAVE_KEYLOG_UPSTREAM
#ifdef HAVE_KEYLOG_CALLBACK
ایستا تهی ossl_keylog_callback(ثابت SSL *ssl, ثابت نویسه *سطر)
{
  (تهی)ssl;

  Curl_tls_keylog_write_سطر(سطر);
}
#وگرنه
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
ایستا تهی ossl_log_tls12_secret(ثابت SSL *ssl, bool *keylog_done)
{
  ثابت SSL_SESSION *session;
  unsigned نویسه client_random[SSL3_RANDOM_SIZE];
  unsigned نویسه master_کلید[SSL_MAX_MASTER_کلید_LENGTH];
  صحیح master_کلید_length = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  اگر(!session || *keylog_done) {
    ERR_برداشت_to_mark();
    بازگشت;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_SIZE);
  master_کلید_length = (صحیح)
    SSL_SESSION_get_master_کلید(session, master_کلید, SSL_MAX_MASTER_کلید_LENGTH);

  ERR_برداشت_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  اگر(master_کلید_length <= 0)
    بازگشت;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        اندازه(client_random),
                        master_کلید, master_کلید_length);
}
#endif /* !HAVE_KEYLOG_CALLBACK */
#endif /* HAVE_KEYLOG_UPSTREAM */

ایستا ثابت نویسه *SSL_ERROR_to_str(صحیح err)
{
  انتخاب(err) {
  حالت SSL_ERROR_NONE:
    بازگشت "SSL_ERROR_NONE";
  حالت SSL_ERROR_SSL:
    بازگشت "SSL_ERROR_SSL";
  حالت SSL_ERROR_WANT_READ:
    بازگشت "SSL_ERROR_WANT_READ";
  حالت SSL_ERROR_WANT_WRITE:
    بازگشت "SSL_ERROR_WANT_WRITE";
  حالت SSL_ERROR_WANT_X509_LOOKUP:
    بازگشت "SSL_ERROR_WANT_X509_LOOKUP";
  حالت SSL_ERROR_SYSCALL:
    بازگشت "SSL_ERROR_SYSCALL";
  حالت SSL_ERROR_ZERO_RETURN:
    بازگشت "SSL_ERROR_ZERO_RETURN";
  حالت SSL_ERROR_WANT_CONNECT:
    بازگشت "SSL_ERROR_WANT_CONNECT";
  حالت SSL_ERROR_WANT_ACCEPT:
    بازگشت "SSL_ERROR_WANT_ACCEPT";
#ifdef SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  حالت SSL_ERROR_WANT_ASYNC:
    بازگشت "SSL_ERROR_WANT_ASYNC";
#endif
#ifdef SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  حالت SSL_ERROR_WANT_ASYNC_JOB:
    بازگشت "SSL_ERROR_WANT_ASYNC_JOB";
#endif
#ifdef SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  حالت SSL_ERROR_WANT_CLIENT_HELLO_CB:
    بازگشت "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#endif
  پیشفرض:
    بازگشت "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
ایستا نویسه *ossl_strerror(unsigned long error, نویسه *buf, size_t size)
{
  size_t len;
  DEBUGASSERT(size);
  *buf = '\0';

  len = Curl_ossl_version(buf, size);
  DEBUGASSERT(len < (size - 2));
  اگر(len < (size - 2)) {
    buf += len;
    size -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ifdef HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, size);
#وگرنه
  ERR_error_string_n(error, buf, size);
#endif

  اگر(!*buf) {
    ثابت نویسه *پیام = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, size, پیام, strlen(پیام));
  }

  بازگشت buf;
}

ایستا صحیح passwd_callback(نویسه *buf, صحیح رقم, صحیح encrypting, تهی *password)
{
  DEBUGASSERT(encrypting == 0);

  اگر(!encrypting && رقم >= 0 && password) {
    صحیح klen = curlx_uztosi(strlen((نویسه *)password));
    اگر(رقم > klen) {
      memcpy(buf, password, klen + 1);
      بازگشت klen;
    }
  }
  بازگشت 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
ایستا bool rand_enough(تهی)
{
  بازگشت RAND_status() != 0;
}

ایستا CURLcode ossl_seed(ساختار Curl_easy *data)
{
  /* This might get called before it has been added to a multi handle */
  اگر(data->multi && data->multi->ssl_seeded)
    بازگشت CURLE_OK;

  اگر(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    اگر(data->multi)
      data->multi->ssl_seeded = TRUE;
    بازگشت CURLE_OK;
  }
  failf(data, "Insufficient randomness");
  بازگشت CURLE_SSL_CONNECT_ERROR;
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
ایستا صحیح ossl_do_file_type(ثابت نویسه *type)
{
  اگر(!type || !type[0])
    بازگشت SSL_FILETYPE_PEM;
  اگر(curl_strequal(type, "PEM"))
    بازگشت SSL_FILETYPE_PEM;
  اگر(curl_strequal(type, "DER"))
    بازگشت SSL_FILETYPE_ASN1;
  اگر(curl_strequal(type, "PROV"))
    بازگشت SSL_FILETYPE_PROVIDER;
  اگر(curl_strequal(type, "ENG"))
    بازگشت SSL_FILETYPE_ENGINE;
  اگر(curl_strequal(type, "P12"))
    بازگشت SSL_FILETYPE_PKCS12;
  بازگشت -1;
}

#اگر defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
ایستا صحیح ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ثابت نویسه *password;
  انتخاب(UI_get_string_type(uis)) {
  حالت UIT_PROMPT:
  حالت UIT_VERIFY:
    password = (ثابت نویسه *)UI_get0_user_data(ui);
    اگر(password && (UI_get_درون_داد_flags(uis) & UI_درون_داد_FLAG_DEFAULT_PWD)) {
      UI_set_result(ui, uis, password);
      بازگشت 1;
    }
    FALLTHROUGH();
  پیشفرض:
    بشکن;
  }
  بازگشت UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
ایستا صحیح ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  انتخاب(UI_get_string_type(uis)) {
  حالت UIT_PROMPT:
  حالت UIT_VERIFY:
    اگر(UI_get0_user_data(ui) &&
       (UI_get_درون_داد_flags(uis) & UI_درون_داد_FLAG_DEFAULT_PWD)) {
      بازگشت 1;
    }
    FALLTHROUGH();
  پیشفرض:
    بشکن;
  }
  بازگشت UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
ایستا bool is_pkcs11_uri(ثابت نویسه *string)
{
  بازگشت string && curl_strnequal(string, "pkcs11:", 7);
}

#endif

ایستا CURLcode ossl_set_engine(ساختار Curl_easy *data, ساختار ossl_ctx *octx,
                                ثابت نویسه *name);
#ifdef OPENSSL_HAS_PROVIDERS
ایستا CURLcode ossl_set_provider(ساختار Curl_easy *data,
                                  ساختار ossl_ctx *octx,
                                  ثابت نویسه *iname);
#endif

ایستا صحیح use_certificate_blob(SSL_CTX *ctx, ثابت ساختار curl_blob *blob,
                                صحیح type, ثابت نویسه *کلید_passwd)
{
  صحیح ret = 0;
  X509 *x = پوچ;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->data, (صحیح)blob->len);
  اگر(!in)
    بازگشت CURLE_OUT_OF_MEMORY;

  اگر(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, پوچ);
  }
  وگرنه اگر(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, پوچ, passwd_callback, CURL_UNCONST(کلید_passwd));
  }
  وگرنه {
    ret = 0;
    برو end;
  }

  اگر(!x) {
    ret = 0;
    برو end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  بازگشت ret;
}

ایستا صحیح use_privatekey_blob(SSL_CTX *ctx, ثابت ساختار curl_blob *blob,
                               صحیح type, ثابت نویسه *کلید_passwd)
{
  صحیح ret = 0;
  EVP_PKEY *pkey = پوچ;
  BIO *in = BIO_new_mem_buf(blob->data, (صحیح)blob->len);
  اگر(!in)
    بازگشت CURLE_OUT_OF_MEMORY;

  اگر(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_Private_کلید(in, پوچ, passwd_callback,
                                   CURL_UNCONST(کلید_passwd));
  وگرنه اگر(type == SSL_FILETYPE_ASN1)
    pkey = d2i_Private_کلید_bio(in, پوچ);
  وگرنه
    برو end;

  اگر(!pkey)
    برو end;

  ret = SSL_CTX_use_Private_کلید(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  بازگشت ret;
}

ایستا صحیح use_certificate_chain_blob(SSL_CTX *ctx,
                                      ثابت ساختار curl_blob *blob,
                                      ثابت نویسه *کلید_passwd)
{
  صحیح ret = 0;
  X509 *x = پوچ;
  BIO *in = BIO_new_mem_buf(blob->data, (صحیح)blob->len);
  اگر(!in)
    بازگشت CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, پوچ,
                            passwd_callback, CURL_UNCONST(کلید_passwd));
  اگر(!x)
    برو end;

  ret = SSL_CTX_use_certificate(ctx, x);

  اگر(ERR_peek_error() != 0)
    ret = 0;

  اگر(ret) {
    X509 *ca;
    sslerr_t err;

    اگر(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      برو end;
    }

    وقتی((ca = PEM_read_bio_X509(in, پوچ, passwd_callback,
                                  CURL_UNCONST(کلید_passwd))) != پوچ) {

      اگر(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        برو end;
      }
    }

    err = ERR_peek_last_error();
    اگر((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_سطر))
      ERR_clear_error();
    وگرنه
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  بازگشت ret;
}

ایستا صحیح enginecheck(ساختار Curl_easy *data,
                       ساختار ossl_ctx *octx,
                       ثابت نویسه *کلید_file,
                       ثابت نویسه *کلید_passwd)
{
#ifdef USE_OPENSSL_ENGINE
  EVP_PKEY *priv_کلید = پوچ;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  اگر(!octx->p.engine && is_pkcs11_uri(کلید_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    بازگشت 0;

  اگر(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    اگر(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      بازگشت 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);
    priv_کلید = ENGINE_load_private_کلید(octx->p.engine, کلید_file,
                                       ui_method,
                                       CURL_UNCONST(کلید_passwd));
    UI_destroy_method(ui_method);
    اگر(!priv_کلید) {
      failf(data, "failed to load private key from crypto engine");
      بازگشت 0;
    }
    اگر(SSL_CTX_use_Private_کلید(ctx, priv_کلید) != 1) {
      failf(data, "unable to set private key");
      EVP_PKEY_free(priv_کلید);
      بازگشت 0;
    }
    EVP_PKEY_free(priv_کلید);  /* we do not need the handle any more... */
  }
  وگرنه {
    failf(data, "crypto engine not set, cannot load private key");
    بازگشت 0;
  }
  بازگشت 1;
#وگرنه
  (تهی)octx;
  (تهی)کلید_file;
  (تهی)کلید_passwd;
  failf(data, "SSL_FILETYPE_ENGINE not supported for private key");
  بازگشت 0;
#endif
}

ایستا صحیح providercheck(ساختار Curl_easy *data,
                         ساختار ossl_ctx *octx,
                         ثابت نویسه *کلید_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  نویسه error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  اگر(!octx->p.provider_loaded && is_pkcs11_uri(کلید_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK) {
    بازگشت 0;
  }

  اگر(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_کلید = پوچ;
    OSSL_STORE_CTX *store = پوچ;
    OSSL_STORE_INFO *info = پوچ;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    اگر(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      بازگشت 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(کلید_file, octx->p.libctx,
                               octx->p.propq, ui_method, پوچ, پوچ,
                               پوچ, پوچ);
    اگر(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      UI_destroy_method(ui_method);
      بازگشت 0;
    }
    اگر(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    اگر(info) {
      صحیح ossl_type = OSSL_STORE_INFO_get_type(info);

      اگر(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_کلید = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    اگر(!priv_کلید) {
      failf(data, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      بازگشت 0;
    }

    اگر(SSL_CTX_use_Private_کلید(ctx, priv_کلید) != 1) {
      failf(data, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      EVP_PKEY_free(priv_کلید);
      بازگشت 0;
    }
    EVP_PKEY_free(priv_کلید); /* we do not need the handle any more... */
  }
  وگرنه {
    failf(data, "crypto provider not set, cannot load private key");
    بازگشت 0;
  }
  بازگشت 1;
#وگرنه
  (تهی)octx;
  (تهی)کلید_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for private key");
  بازگشت 0;
#endif
}

ایستا صحیح engineload(ساختار Curl_easy *data,
                      ساختار ossl_ctx *octx,
                      ثابت نویسه *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#اگر defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  نویسه error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  اگر(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    بازگشت 0;

  اگر(octx->p.engine) {
    ایستا ثابت نویسه cmd_name[] = "LOAD_CERT_CTRL";
    ساختار {
      ثابت نویسه *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = پوچ;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    اگر(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), پوچ)) {
      failf(data, "SSL engine does not support loading certificates");
      بازگشت 0;
    }

    /* Load the certificate from the engine */
    اگر(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, پوچ, 1)) {
      failf(data, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      بازگشت 0;
    }

    اگر(!params.cert) {
      failf(data, "SSL engine did not initialize the certificate properly.");
      بازگشت 0;
    }

    اگر(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      X509_free(params.cert);
      بازگشت 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  وگرنه {
    failf(data, "crypto engine not set, cannot load certificate");
    بازگشت 0;
  }
  بازگشت 1;
#وگرنه
  (تهی)octx;
  (تهی)cert_file;
  failf(data, "SSL_FILETYPE_ENGINE not supported for certificate");
  بازگشت 0;
#endif
}

ایستا صحیح providerload(ساختار Curl_easy *data,
                        ساختار ossl_ctx *octx,
                        ثابت نویسه *cert_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  نویسه error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  اگر(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK)
    بازگشت 0;

  اگر(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = پوچ;
    X509 *cert = پوچ;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         پوچ, پوچ, پوچ, پوچ, پوچ, پوچ);
    صحیح rc;

    اگر(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      بازگشت 0;
    }
    اگر(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    اگر(info) {
      صحیح ossl_type = OSSL_STORE_INFO_get_type(info);

      اگر(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    اگر(!cert) {
      failf(data, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      بازگشت 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    اگر(rc != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      بازگشت 0;
    }
  }
  وگرنه {
    failf(data, "crypto provider not set, cannot load certificate");
    بازگشت 0;
  }
  بازگشت 1;
#وگرنه
  (تهی)octx;
  (تهی)cert_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for certificate");
  بازگشت 0;
#endif
}

ایستا صحیح pkcs12load(ساختار Curl_easy *data,
                      SSL_CTX* ctx,
                      ثابت ساختار curl_blob *cert_blob,
                      ثابت نویسه *cert_file,
                      ثابت نویسه *کلید_passwd)
{
  نویسه error_buffer[256];
  BIO *cert_bio = پوچ;
  PKCS12 *p12 = پوچ;
  EVP_PKEY *pri;
  X509 *x509;
  صحیح cert_done = 0;
  پشته_OF(X509) *ca = پوچ;
  اگر(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->data, (صحیح)cert_blob->len);
    اگر(!cert_bio) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      بازگشت 0;
    }
  }
  وگرنه {
    cert_bio = BIO_new(BIO_s_file());
    اگر(!cert_bio) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
      بازگشت 0;
    }

    اگر(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(data, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      بازگشت 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, پوچ);
  BIO_free(cert_bio);

  اگر(!p12) {
    failf(data, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    بازگشت 0;
  }

  اگر(!PKCS12_parse(p12, کلید_passwd, &pri, &x509, &ca)) {
    failf(data, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, اندازه(error_buffer)));
    PKCS12_free(p12);
    بازگشت 0;
  }

  PKCS12_free(p12);

  اگر(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(data, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, اندازه(error_buffer)));
    برو fail;
  }

  اگر(SSL_CTX_use_Private_کلید(ctx, pri) != 1) {
    failf(data, "unable to use private key from PKCS12 file '%s'", cert_file);
    برو fail;
  }

  اگر(!SSL_CTX_check_private_کلید(ctx)) {
    failf(data, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    برو fail;
  }
  /* Set Certificate Verification chain */
  اگر(ca) {
    وقتی(sk_X509_رقم(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_برداشت(ca);
      اگر(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to client CA list");
        برو fail;
      }
      اگر(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to certificate chain");
        برو fail;
      }
    }
  }

  cert_done = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_برداشت_free(ca, X509_free);
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic برداشت
#endif
  اگر(!cert_done)
    بازگشت 0; /* failure! */
  بازگشت 1;
}

ایستا CURLcode client_cert(ساختار Curl_easy *data,
                            ساختار ossl_ctx *octx,
                            نویسه *cert_file,
                            ثابت ساختار curl_blob *cert_blob,
                            ثابت نویسه *cert_type,
                            نویسه *کلید_file,
                            ثابت ساختار curl_blob *کلید_blob,
                            ثابت نویسه *کلید_type,
                            نویسه *کلید_passwd)
{
  نویسه error_buffer[256];
  bool check_privkey = TRUE;
  صحیح file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  اگر(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    صحیح cert_use_result;

    اگر(کلید_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, کلید_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    انتخاب(file_type) {
    حالت SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_result = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, کلید_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      اگر(cert_use_result != 1) {
        failf(data,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            اندازه(error_buffer)));
        بازگشت CURLE_SSL_CERTPROBLEM;
      }
      بشکن;

    حالت SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_result = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, کلید_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      اگر(cert_use_result != 1) {
        failf(data,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            اندازه(error_buffer)));
        بازگشت CURLE_SSL_CERTPROBLEM;
      }
      بشکن;

    حالت SSL_FILETYPE_ENGINE:
      اگر(!cert_file || !engineload(data, octx, cert_file))
        بازگشت CURLE_SSL_CERTPROBLEM;
      بشکن;

    حالت SSL_FILETYPE_PROVIDER:
      اگر(!cert_file || !providerload(data, octx, cert_file))
        بازگشت CURLE_SSL_CERTPROBLEM;
      بشکن;

    حالت SSL_FILETYPE_PKCS12:
      اگر(!pkcs12load(data, ctx, cert_blob, cert_file, کلید_passwd))
        بازگشت CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      بشکن;

    پیشفرض:
      failf(data, "not supported file type '%s' for certificate", cert_type);
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }

    اگر(!کلید_file && !کلید_blob) {
      کلید_file = cert_file;
      کلید_blob = cert_blob;
    }
    وگرنه
      file_type = ossl_do_file_type(کلید_type);

    انتخاب(file_type) {
    حالت SSL_FILETYPE_PEM:
    حالت SSL_FILETYPE_ASN1:
      cert_use_result = کلید_blob ?
        use_privatekey_blob(ctx, کلید_blob, file_type, کلید_passwd) :
      SSL_CTX_use_Private_کلید_file(ctx, کلید_file, file_type);
      اگر(cert_use_result != 1) {
        failf(data, "unable to set private key file: '%s' type %s",
              کلید_file ? کلید_file : "(memory blob)",
              کلید_type ? کلید_type : "PEM");
        بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
      }
      بشکن;
    حالت SSL_FILETYPE_ENGINE:
      اگر(!enginecheck(data, octx, کلید_file, کلید_passwd))
        بازگشت CURLE_SSL_CERTPROBLEM;
      بشکن;

    حالت SSL_FILETYPE_PROVIDER:
      اگر(!providercheck(data, octx, کلید_file))
        بازگشت CURLE_SSL_CERTPROBLEM;
      بشکن;

    حالت SSL_FILETYPE_PKCS12:
      اگر(!pcks12_done) {
        failf(data, "file type P12 for private key not supported");
        بازگشت CURLE_SSL_CERTPROBLEM;
      }
      بشکن;
    پیشفرض:
      failf(data, "not supported file type for private key");
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    اگر(!ssl) {
      failf(data, "unable to create an SSL structure");
      بازگشت CURLE_OUT_OF_MEMORY;
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
        اگر(RSA_flags(rsa) & RSA_METHOD_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#endif

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    اگر(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      اگر(!SSL_CTX_check_private_کلید(ctx)) {
        failf(data, "Private key does not match the certificate public key");
        بازگشت CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  بازگشت CURLE_OK;
}

#ifdef CURLVERBOSE
/* returns non-zero on failure */
ایستا CURLcode x509_name_oneline(ثابت X509_NAME *a, ساختار dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  صحیح rc;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  اگر(bio_out) {
    unsigned long flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    اگر(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      result = curlx_dyn_addn(d, biomem->data, biomem->length);
    }
    BIO_free(bio_out);
  }
  بازگشت result;
}
#endif

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ایستا صحیح ossl_init(تهی)
{
  ثابت uint64_t flags =
#ifdef OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#endif
#ifdef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    OPENSSL_INIT_NO_LOAD_CONFIG |
#وگرنه
    OPENSSL_INIT_LOAD_CONFIG |
#endif
    0;
  OPENSSL_init_ssl(flags, پوچ);

#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#endif

  بازگشت 1;
}

/* Global cleanup */
ایستا تهی ossl_cleanup(تهی)
{
#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#endif
}

/* Selects an OpenSSL crypto engine or provider.
 */
ایستا CURLcode ossl_set_engine(ساختار Curl_easy *data, ساختار ossl_ctx *octx,
                                ثابت نویسه *name)
{
#ifdef USE_OPENSSL_ENGINE
  CURLcode result = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  اگر(e) {

    اگر(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = پوچ;
    }
    اگر(!ENGINE_init(e)) {
      نویسه buf[256];

      ENGINE_free(e);
      failf(data, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, اندازه(buf)));
      result = CURLE_SSL_ENGINE_INITFAILED;
      e = پوچ;
    }
    وگرنه {
      result = CURLE_OK;
    }
    octx->p.engine = e;
    بازگشت result;
  }
#endif
#ifdef OPENSSL_HAS_PROVIDERS
  بازگشت ossl_set_provider(data, octx, name);
#وگرنه
  (تهی)octx;
  (تهی)name;
  failf(data, "OpenSSL engine not found");
  بازگشت CURLE_SSL_ENGINE_NOTFOUND;
#endif
}

/* Sets engine as default for all SSL operations
 */
ایستا CURLcode ossl_set_engine_default(ساختار Curl_easy *data,
                                        ساختار ossl_ctx *octx)
{
#ifdef USE_OPENSSL_ENGINE
  اگر(octx->p.engine) {
    اگر(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(data, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    وگرنه {
      failf(data, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      بازگشت CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#وگرنه
  (تهی)data;
  (تهی)octx;
#endif
  بازگشت CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
ایستا ساختار curl_slist *ossl_engines_list(ساختار Curl_easy *data)
{
  ساختار curl_slist *list = پوچ;
#ifdef USE_OPENSSL_ENGINE
  ساختار curl_slist *beg;
  ENGINE *e;

  برای(e = ENGINE_get_first(); e; e = ENGINE_get_next(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    اگر(!beg) {
      curl_slist_free_all(list);
      بازگشت پوچ;
    }
    list = beg;
  }
#endif
  (تهی)data;
  بازگشت list;
}

#ifdef OPENSSL_HAS_PROVIDERS

ایستا تهی ossl_provider_cleanup(ساختار ossl_ctx *octx)
{
  اگر(!octx)
    بازگشت;
  اگر(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = پوچ;
  }
  اگر(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = پوچ;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = پوچ;
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
ایستا CURLcode ossl_set_provider(ساختار Curl_easy *data,
                                  ساختار ossl_ctx *octx,
                                  ثابت نویسه *iname)
{
  نویسه name[MAX_PROVIDER_LEN + 1];
  ساختار Curl_str prov;
  ثابت نویسه *propq = پوچ;

  اگر(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    بازگشت CURLE_OK;
  }
  اگر(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  اگر(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  memcpy(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  اگر(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    اگر(!libctx)
      بازگشت CURLE_OUT_OF_MEMORY;
    اگر(propq) {
      octx->p.propq = curlx_strdup(propq);
      اگر(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        بازگشت CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#ifndef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  اگر(!OSSL_LIB_CTX_load_config(octx->p.libctx, پوچ)) {
    infof(data, "Failed to load default openssl config. Proceeding.");
  }
#endif

  اگر(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    بازگشت CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  اگر(!octx->p.provider) {
    نویسه error_buffer[256];
    failf(data, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_buffer, اندازه(error_buffer)));
    ossl_provider_cleanup(octx);
    بازگشت CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  اگر(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(data, "Failed to load base");
    بازگشت CURLE_SSL_ENGINE_NOTFOUND;
  }
  وگرنه
    octx->p.provider_loaded = TRUE;
  بازگشت CURLE_OK;
}
#وگرنه /* OPENSSL_HAS_PROVIDERS */
#define ossl_provider_cleanup(x)
#endif

ایستا CURLcode ossl_shutdown(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  نویسه buf[1024];
  صحیح nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  اگر(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    برو out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  اگر(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    برای(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (صحیح)اندازه(buf));
      CURL_TRC_CF(data, cf, "SSL shutdown not sent, read -> %d", nread);
      اگر(nread <= 0)
        بشکن;
    }
    err = SSL_get_error(octx->ssl, nread);
    اگر(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool درون_داد_pending;
      /* Yes, it did. */
      اگر(!send_shutdown) {
        CURL_TRC_CF(data, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        برو out;
      }
      وگرنه اگر(!cf->next->cft->is_alive(cf->next, data, &درون_داد_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(data, cf, "peer closed connection");
        *done = TRUE;
        برو out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  اگر(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    صحیح rc;
    ERR_clear_error();
    CURL_TRC_CF(data, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    اگر(rc == 1) {
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
      *done = TRUE;
      برو out;
    }
    اگر(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      برو out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  برای(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (صحیح)اندازه(buf));
    CURL_TRC_CF(data, cf, "SSL shutdown read -> %d", nread);
    اگر(nread <= 0)
      بشکن;
  }
  err = SSL_get_error(octx->ssl, nread);
  انتخاب(err) {
  حالت SSL_ERROR_ZERO_RETURN: /* no more data */
    اگر(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
    وگرنه
      CURL_TRC_CF(data, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    بشکن;
  حالت SSL_ERROR_NONE: /* did not get anything */
  حالت SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(data, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    بشکن;
  حالت SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(data, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    بشکن;
  پیشفرض:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(unsigned long sslerr = ERR_get_error());
      CURL_TRC_CF(data, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, اندازه(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    result = CURLE_OK;
    بشکن;
  }

out:
  cf->shutdown = (result || *done);
  اگر(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->درون_داد_pending = FALSE;
  بازگشت result;
}

/* also used by quic-tls */
تهی Curl_osslctx_cleanup(ساختار ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  اگر(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = پوچ;
  }
  اگر(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = پوچ;
    octx->x509_store_setup = FALSE;
  }
  اگر(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = پوچ;
  }
#ifdef USE_OPENSSL_ENGINE
  اگر(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = پوچ;
  }
#endif
  ossl_provider_cleanup(octx);
}

ایستا تهی ossl_close(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  (تهی)data;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
ایستا تهی ossl_close_all(ساختار Curl_easy *data)
{
  (تهی)data;
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
ایستا CURLcode ossl_verifyhost(ساختار Curl_easy *data,
                                ساختار connectdata *conn,
                                ساختار ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  صحیح target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  پشته_OF(GENERAL_NAME) *altnames;
#ifdef USE_IPV6
  ساختار in6_addr addr;
#وگرنه
  ساختار in_addr addr;
#endif
  CURLcode result = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = strlen(peer->origin->hostname);

  (تهی)conn;
  انتخاب(peer->type) {
  حالت CURL_SSL_PEER_IPV4:
    اگر(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      بازگشت CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = اندازه(ساختار in_addr);
    بشکن;
#ifdef USE_IPV6
  حالت CURL_SSL_PEER_IPV6:
    اگر(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      بازگشت CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = اندازه(ساختار in6_addr);
    بشکن;
#endif
  حالت CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    بشکن;
  پیشفرض:
    DEBUGASSERT(0);
    failf(data, "unexpected SSL peer type: %d", (صحیح)peer->type);
    بازگشت CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, پوچ, پوچ);

  اگر(altnames) {
#ifdef HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#وگرنه
    صحیح numalts;
    صحیح i;
#endif

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_رقم(altnames);

    /* loop through all alternatives - until a dnsmatch */
    برای(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ثابت GENERAL_NAME *check = sk_GENERAL_NAME_مقدار(altnames, i);

      اگر(check->type == GEN_DNS)
        dNSName = TRUE;
      وگرنه اگر(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      اگر(check->type == target) {
        /* get data and length */
        ثابت نویسه *altptr = (ثابت نویسه *)ASN1_STRING_get0_data(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_length(check->d.ia5);

        انتخاب(target) {
        حالت GEN_DNS: /* name/pattern comparison */
          اگر(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (صحیح)altlen, altptr);
          }
          بشکن;

        حالت GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          اگر((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          بشکن;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  اگر(matched)
    /* an alternative name matched */
    ;
  وگرنه اگر(dNSName || iPAddress) {
    ثابت نویسه *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(data, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(data, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    result = CURLE_PEER_FAILED_VERIFICATION;
  }
  وگرنه {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    صحیح i = -1;
    unsigned نویسه *cn = پوچ;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ثابت X509_NAME *name = X509_get_subject_name(server_cert);
    اگر(name) {
      صحیح j;
      وقتی((j = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    اگر(i >= 0) {
      ثابت ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      اگر(tmp) {
        اگر(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_length(tmp);
          cn = (unsigned نویسه *)CURL_UNCONST(ASN1_STRING_get0_data(tmp));
        }
        وگرنه { /* not a UTF8 name */
          ثابت صحیح utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          اگر(utf8len < 0)
            cnlen = 0;
          وگرنه
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        اگر((cnlen == 0) || !cn)
          result = CURLE_OUT_OF_MEMORY;
        وگرنه اگر(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(data, "SSL: illegal cert name field");
          result = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    اگر(result)
      /* error already detected, pass through */
      ;
    وگرنه اگر(!cn) {
      failf(data, "SSL: unable to obtain common name from peer certificate");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    وگرنه اگر(!Curl_cert_hostcheck((ثابت نویسه *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(data, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (صحیح)cnlen, cn,
            peer->origin->user_hostname);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    وگرنه {
      infof(data, " common name: %.*s (matched)", (صحیح)cnlen, cn);
    }
    اگر(free_cn)
      OPENSSL_free(cn);
  }

  بازگشت result;
}

#ifndef OPENSSL_NO_OCSP
ایستا CURLcode verifystatus(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             ساختار ossl_ctx *octx)
{
  صحیح i, ocsp_status;
#ifdef HAVE_BORINGSSL_LIKE
  ثابت uint8_t *status;
#وگرنه
  unsigned نویسه *status;
#endif
  ثابت unsigned نویسه *p;
  CURLcode result = CURLE_OK;
  OCSP_RESPONSE *rsp = پوچ;
  OCSP_BASICRESP *br = پوچ;
  X509_STORE     *st = پوچ;
  پشته_OF(X509) *ch = پوچ;
  X509 *cert;
  OCSP_CERTID *id = پوچ;
  صحیح cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  صحیح ret;
  long len;

  (تهی)cf;
  DEBUGASSERT(octx);

  len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  اگر(!status) {
    failf(data, "No OCSP response received");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(پوچ, &p, len);
  اگر(!rsp) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }

  ocsp_status = OCSP_response_status(rsp);
  اگر(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(data, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }

  br = OCSP_response_get1_basic(rsp);
  اگر(!br) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }

#اگر !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_عدد) && LIBRESSL_VERSION_عدد < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#وگرنه
  ch = SSL_get_peer_cert_chain(octx->ssl);
#endif
  اگر(!ch) {
    failf(data, "Could not get peer certificate chain");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  اگر(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(data, "OCSP response verification failed");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  اگر(!cert) {
    failf(data, "Error getting peer certificate");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }

  برای(i = 0; i < (صحیح)sk_X509_رقم(ch); i++) {
    X509 *issuer = sk_X509_مقدار(ch, (ossl_valsize_t)i);
    اگر(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      بشکن;
    }
  }
  X509_free(cert);

  اگر(!id) {
    failf(data, "Error computing OCSP ID");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  اگر(ret != 1) {
    failf(data, "Could not find certificate ID in OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
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
    failf(data, "OCSP response has expired");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }

  infof(data, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  انتخاب(cert_status) {
  حالت V_OCSP_CERTSTATUS_GOOD:
    بشکن;

  حالت V_OCSP_CERTSTATUS_REVOKED:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    failf(data, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    برو end;

  حالت V_OCSP_CERTSTATUS_UNKNOWN:
  پیشفرض:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو end;
  }

end:
  اگر(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  بازگشت result;
}
#endif

ایستا ثابت نویسه *ssl_پیام_type(صحیح ssl_ver, صحیح پیام)
{
  اگر(ssl_ver == SSL3_VERSION_MAJOR) {
    انتخاب(پیام) {
    حالت SSL3_MT_HELLO_REQUEST:
      بازگشت "Hello request";
    حالت SSL3_MT_CLIENT_HELLO:
      بازگشت "Client hello";
    حالت SSL3_MT_SERVER_HELLO:
      بازگشت "Server hello";
#ifdef SSL3_MT_NEWSESSION_TICKET
    حالت SSL3_MT_NEWSESSION_TICKET:
      بازگشت "Newsession Ticket";
#endif
    حالت SSL3_MT_CERTIFICATE:
      بازگشت "Certificate";
    حالت SSL3_MT_SERVER_کلید_EXCHANGE:
      بازگشت "Server key exchange";
    حالت SSL3_MT_CLIENT_کلید_EXCHANGE:
      بازگشت "Client key exchange";
    حالت SSL3_MT_CERTIFICATE_REQUEST:
      بازگشت "Request CERT";
    حالت SSL3_MT_SERVER_DONE:
      بازگشت "Server finished";
    حالت SSL3_MT_CERTIFICATE_VERIFY:
      بازگشت "CERT verify";
    حالت SSL3_MT_FINISHED:
      بازگشت "Finished";
#ifdef SSL3_MT_CERTIFICATE_STATUS
    حالت SSL3_MT_CERTIFICATE_STATUS:
      بازگشت "Certificate Status";
#endif
#ifdef SSL3_MT_ENCRYPTED_EXTENSIONS
    حالت SSL3_MT_ENCRYPTED_EXTENSIONS:
      بازگشت "Encrypted Extensions";
#endif
#ifdef SSL3_MT_SUPPLEMENTAL_DATA
    حالت SSL3_MT_SUPPLEMENTAL_DATA:
      بازگشت "Supplemental data";
#endif
#ifdef SSL3_MT_END_OF_EARLY_DATA
    حالت SSL3_MT_END_OF_EARLY_DATA:
      بازگشت "End of early data";
#endif
#ifdef SSL3_MT_کلید_UPDATE
    حالت SSL3_MT_کلید_UPDATE:
      بازگشت "Key update";
#endif
#ifdef SSL3_MT_NEXT_PROTO
    حالت SSL3_MT_NEXT_PROTO:
      بازگشت "Next protocol";
#endif
#ifdef SSL3_MT_MESSAGE_HASH
    حالت SSL3_MT_MESSAGE_HASH:
      بازگشت "Message hash";
#endif
    }
  }
  بازگشت "Unknown";
}

ایستا ثابت نویسه *tls_rt_type(صحیح type)
{
  انتخاب(type) {
#ifdef SSL3_RT_HEADER
  حالت SSL3_RT_HEADER:
    بازگشت "TLS header";
#endif
  حالت SSL3_RT_CHANGE_CIPHER_SPEC:
    بازگشت "TLS change cipher";
  حالت SSL3_RT_ALERT:
    بازگشت "TLS alert";
  حالت SSL3_RT_HANDSHAKE:
    بازگشت "TLS handshake";
  حالت SSL3_RT_APPLICATION_DATA:
    بازگشت "TLS app data";
  پیشفرض:
    بازگشت "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
ایستا تهی ossl_trace(صحیح direction, صحیح ssl_ver, صحیح content_type,
                       ثابت تهی *buf, size_t len, SSL *ssl,
                       تهی *userp)
{
  ثابت نویسه *verstr;
  ساختار Curl_cfilter *cf = userp;
  ساختار Curl_easy *data = پوچ;
  نویسه unknown[32];

  اگر(!cf)
    بازگشت;
  data = CF_DATA_CURRENT(cf);
  اگر(!data || !data->set.fdebug || (direction && direction != 1))
    بازگشت;

  انتخاب(ssl_ver) {
#ifdef SSL3_VERSION
  حالت SSL3_VERSION:
    verstr = "SSLv3";
    بشکن;
#endif
  حالت TLS1_VERSION:
    verstr = "TLSv1.0";
    بشکن;
#ifdef TLS1_1_VERSION
  حالت TLS1_1_VERSION:
    verstr = "TLSv1.1";
    بشکن;
#endif
#ifdef TLS1_2_VERSION
  حالت TLS1_2_VERSION:
    verstr = "TLSv1.2";
    بشکن;
#endif
  حالت TLS1_3_VERSION:
    verstr = "TLSv1.3";
    بشکن;
  پیشفرض:
    curl_msnprintf(unknown, اندازه(unknown), "(%x)", (unsigned صحیح)ssl_ver);
    verstr = unknown;
    بشکن;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  اگر(ssl_ver
#ifdef SSL3_RT_HEADER
     && content_type != SSL3_RT_HEADER
#endif
#ifdef SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#endif
    ) {
    ثابت نویسه *پیام_name = "Truncated message";
    ثابت نویسه *tls_rt_name;
    نویسه ssl_buf[1024];
    صحیح پیام_type = 0;
    صحیح txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    اگر(ssl_ver == SSL3_VERSION_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    وگرنه
      tls_rt_name = "";

    اگر(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      اگر(len) {
        پیام_type = *(ثابت unsigned نویسه *)buf;
        پیام_name = "Change cipher spec";
      }
    }
    وگرنه اگر(content_type == SSL3_RT_ALERT) {
      اگر(len >= 2) {
        پیام_type =
          (((ثابت unsigned نویسه *)buf)[0] << 8) +
           ((ثابت unsigned نویسه *)buf)[1];
        پیام_name = SSL_alert_desc_string_long(پیام_type);
      }
    }
    وگرنه اگر(len) {
      پیام_type = *(ثابت unsigned نویسه *)buf;
      پیام_name = ssl_پیام_type(ssl_ver, پیام_type);
    }

    txt_len = curl_msnprintf(ssl_buf, اندازه(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, پیام_name, پیام_type);
    Curl_debug(data, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(data, (direction == 1) ? CURLINFO_SSL_DATA_OUT :
             CURLINFO_SSL_DATA_IN, (ثابت نویسه *)buf, len);
  (تهی)ssl;
}

ایستا CURLcode ossl_set_ssl_version_min_max(ساختار Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             unsigned صحیح ssl_version_min)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  long curl_ssl_version_min = (long)ssl_version_min;
  long curl_ssl_version_max;

  /* convert curl min SSL version option to OpenSSL constant */
#اگر defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_VERSION_عدد)
  uint16_t ossl_ssl_version_min = 0;
  uint16_t ossl_ssl_version_max = 0;
#وگرنه
  long ossl_ssl_version_min = 0;
  long ossl_ssl_version_max = 0;
#endif
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_version_min != CURL_SSLVERSION_DEFAULT);
  انتخاب(curl_ssl_version_min) {
  حالت CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  حالت CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    بشکن;
  حالت CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    بشکن;
  حالت CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    بشکن;
  حالت CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    بشکن;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (long)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  انتخاب(curl_ssl_version_max) {
  حالت CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    بشکن;
  حالت CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    بشکن;
  حالت CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    بشکن;
  حالت CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    بشکن;
  حالت CURL_SSLVERSION_MAX_NONE:  /* none selected */
  حالت CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  پیشفرض:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    بشکن;
  }

  اگر(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    بازگشت CURLE_SSL_CONNECT_ERROR;

  بازگشت CURLE_OK;
}

CURLcode Curl_ossl_add_session(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار ossl_ctx *octx,
                               ثابت نویسه *ssl_peer_کلید,
                               SSL_SESSION *session,
                               ثابت نویسه *alpn,
                               unsigned نویسه *quic_tp,
                               size_t quic_tp_len,
                               ساختار Curl_ssl_session **psession)
{
  ساختار Curl_ssl_session *sc_session = پوچ, *sc_dup = پوچ;
  unsigned نویسه *der_session_buf = پوچ;
  unsigned نویسه *qtp_clone = پوچ;
  CURLcode result = CURLE_OK;

  اگر(psession)
    *psession = پوچ;
  اگر(!cf || !data)
    برو out;

  اگر(Curl_ssl_scache_use(cf, data)) {
    size_t der_session_size;
    unsigned نویسه *der_session_ptr;
    size_t earlydata_max = 0;
    صحیح ietf_tls_id = SSL_version(octx->ssl);

    der_session_size = i2d_SSL_SESSION(session, پوچ);
    اگر(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_size);
    اگر(!der_session_buf) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }

    der_session_size = i2d_SSL_SESSION(session, &der_session_ptr);
    اگر(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }

#ifdef HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_data(session);
#endif
    اگر(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ثابت نویسه *)quic_tp, quic_tp_len);
      اگر(!qtp_clone) {
        result = CURLE_OUT_OF_MEMORY;
        برو out;
      }
    }

    result = Curl_ssl_session_create2(der_session_buf, der_session_size,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(پوچ) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = پوچ;  /* took ownership of sdata */
#ifdef USE_APPLE_SECTRUST
    اگر(!result)
      sc_session->sectrust_verified = octx->sectrust_verified;
#endif
    اگر(!result && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        result = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    اگر(!result) {
      result = Curl_ssl_scache_put(cf, data, ssl_peer_کلید, sc_session);
      /* took ownership of `sc_session` */
      sc_session = پوچ;
    }
  }

out:
  curlx_free(der_session_buf);
  اگر(!result && psession) {
    *psession = sc_dup;
    sc_dup = پوچ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  بازگشت result;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
ایستا صحیح ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  ساختار Curl_cfilter *cf = (ساختار Curl_cfilter *)SSL_get_app_data(ssl);
  اگر(cf) {
    ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
    ساختار ssl_connect_data *connssl = cf->ctx;
    ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
    ساختار Curl_ssl_session *session = پوچ;
    Curl_ossl_add_session(cf, data, octx, connssl->peer.scache_کلید,
                          ssl_sessionid, connssl->negotiated.alpn, پوچ,
                          0, &session);
    اگر(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  بازگشت 0;
}

ایستا CURLcode load_cacert_from_memory(X509_STORE *store,
                                        ثابت ساختار curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = پوچ;
  پشته_OF(X509_INFO) *inf = پوچ;

  /* everything else is a reference */
  صحیح i, شمار = 0;
  X509_INFO *itmp = پوچ;

  اگر(ca_info_blob->len > (size_t)INT_MAX)
    بازگشت CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->data, (صحیح)ca_info_blob->len);
  اگر(!cbio)
    بازگشت CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, پوچ, پوچ, پوچ);
  اگر(!inf) {
    BIO_free(cbio);
    بازگشت CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  برای(i = 0; i < (صحیح)sk_X509_INFO_رقم(inf); ++i) {
    itmp = sk_X509_INFO_مقدار(inf, (ossl_valsize_t)i);
    اگر(itmp->x509) {
      اگر(X509_STORE_add_cert(store, itmp->x509)) {
        ++شمار;
      }
      وگرنه {
        /* set count to 0 to return an error */
        شمار = 0;
        بشکن;
      }
    }
    اگر(itmp->crl) {
      اگر(X509_STORE_add_crl(store, itmp->crl)) {
        ++شمار;
      }
      وگرنه {
        /* set count to 0 to return an error */
        شمار = 0;
        بشکن;
      }
    }
  }

#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic افزودن
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_INFO_برداشت_free(inf, X509_INFO_free);
#اگر defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic برداشت
#endif
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  بازگشت (شمار > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ifdef USE_WIN32_CRYPTO
ایستا CURLcode ossl_win_load_store(ساختار Curl_easy *data,
                                    ساختار Curl_cfilter *cf,
                                    ثابت نویسه *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode result = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  اگر(hStore) {
    PCCERT_CONTEXT pContext = پوچ;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = پوچ;
    DWORD enhkey_usage_size = 0;
    VERBOSE(size_t total = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    result = CURLE_OK;
    برای(;;) {
      X509 *x509;
      FILETIME now;
      BYTE کلید_usage[2];
      DWORD req_size;
      ثابت unsigned نویسه *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      اگر(!pContext)
        بشکن;

      VERBOSE(++total);

#اگر defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        نویسه cert_name[256];
        اگر(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               پوچ, cert_name, اندازه(cert_name)))
          infof(data, "SSL: unknown cert name");
        وگرنه
          infof(data, "SSL: Checking cert \"%s\"", cert_name);
      }
#endif
      encoded_cert = (ثابت unsigned نویسه *)pContext->pbCertEncoded;
      اگر(!encoded_cert)
        ادامه;

      GetSystemTimeAsFileTime(&now);
      اگر(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        ادامه;

      /* If key usage exists check for signing attribute */
      اگر(Cert_Get_Intended_کلید_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 کلید_usage, اندازه(کلید_usage))) {
        اگر(!(کلید_usage[0] & CERT_کلید_CERT_SIGN_کلید_USAGE))
          ادامه;
      }
      وگرنه اگر(GetLastError())
        ادامه;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      اگر(Cert_Get_Enhanced_کلید_Usage(pContext, 0, پوچ, &req_size) && req_size) {
        اگر(req_size > enhkey_usage_size) {
          تهی *tmp = curlx_realloc(enhkey_usage, req_size);

          اگر(!tmp) {
            failf(data, "SSL: Out of memory allocating for OID list");
            result = CURLE_OUT_OF_MEMORY;
            بشکن;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_size = req_size;
        }

        اگر(Cert_Get_Enhanced_کلید_Usage(pContext, 0, enhkey_usage, &req_size)) {
          اگر(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            اگر((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              ادامه;
          }
          وگرنه {
            DWORD i;
            bool found = FALSE;

            برای(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              اگر(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                بشکن;
              }
            }

            اگر(!found)
              ادامه;
          }
        }
        وگرنه
          ادامه;
      }
      وگرنه
        ادامه;

      x509 = d2i_X509(پوچ, &encoded_cert, (long)pContext->cbCertEncoded);
      اگر(!x509)
        ادامه;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      اگر(X509_STORE_add_cert(store, x509) == 1) {
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

    اگر(result)
      بازگشت result;
  }

  بازگشت result;
}

ایستا CURLcode ossl_windows_load_anchors(ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  ایستا ثابت نویسه * ثابت win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode result = CURLE_OK;

  *padded = FALSE;
  برای(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    result = ossl_win_load_store(data, cf, win_stores[i], store, &store_added);
    اگر(result)
      بازگشت result;
    اگر(store_added) {
      CURL_TRC_CF(data, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    وگرنه
      infof(data, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  بازگشت result;
}

#endif /* USE_WIN32_CRYPTO */

ایستا CURLcode ossl_load_trust_anchors(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        ساختار ossl_ctx *octx,
                                        X509_STORE *store)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  ثابت نویسه * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? پوچ : conn_config->CAfile);
  ثابت نویسه * ثابت ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  اگر(conn_config->native_ca_store) {
#ifdef USE_WIN32_CRYPTO
    bool added = FALSE;
    result = ossl_windows_load_anchors(cf, data, store, &added);
    اگر(result)
      بازگشت result;
    اگر(added) {
      infof(data, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#وگرنهاگر defined(USE_APPLE_SECTRUST)
    infof(data, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#endif
  }

  اگر(conn_config->ca_info_blob) {
    result = load_cacert_from_memory(store, conn_config->ca_info_blob);
    اگر(result) {
      failf(data, "error adding trust anchors from certificate blob: %d",
            (صحیح)result);
      بازگشت result;
    }
    infof(data, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  اگر(ssl_cafile || ssl_capath) {
#ifdef HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    اگر(ssl_cafile) {
      اگر(!X509_STORE_load_file(store, ssl_cafile)) {
        اگر(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from file: %s", ssl_cafile);
          بازگشت CURLE_SSL_CACERT_BADFILE;
        }
        وگرنه
          infof(data, "error setting certificate file, continuing anyway");
      }
      infof(data, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    اگر(ssl_capath) {
      اگر(!X509_STORE_load_path(store, ssl_capath)) {
        اگر(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from path: %s", ssl_capath);
          بازگشت CURLE_SSL_CACERT_BADFILE;
        }
        وگرنه
          infof(data, "error setting certificate path, continuing anyway");
      }
      infof(data, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#وگرنه
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    اگر(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      اگر(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(data, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        بازگشت CURLE_SSL_CACERT_BADFILE;
      }
      وگرنه {
        infof(data, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    اگر(ssl_cafile)
      infof(data, "  CAfile: %s", ssl_cafile);
    اگر(ssl_capath)
      infof(data, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#endif
  }

#ifdef CURL_CA_FALLBACK
  اگر(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(data, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#endif
  اگر(octx->store_is_empty && !have_native_check)
    infof(data, "  no trust anchors configured");

  بازگشت result;
}

ایستا CURLcode ossl_populate_x509_store(ساختار Curl_cfilter *cf,
                                         ساختار Curl_easy *data,
                                         ساختار ossl_ctx *octx,
                                         X509_STORE *store)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  X509_LOOKUP *lookup = پوچ;
  ثابت نویسه * ثابت ssl_crlfile = conn_config->CRLfile;
  unsigned long x509flags = 0;

  CURL_TRC_CF(data, cf, "configuring OpenSSL's x509 trust store");
  اگر(!store)
    بازگشت CURLE_OUT_OF_MEMORY;

  اگر(!conn_config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    بازگشت CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  result = ossl_load_trust_anchors(cf, data, octx, store);
  اگر(result)
    بازگشت result;

  /* Does not make sense to load a CRL file without peer verification */
#ifdef USE_APPLE_SECTRUST
  اگر(ssl_crlfile && conn_config->native_ca_store) {
    failf(data, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    بازگشت CURLE_NOT_BUILT_IN;
  }
#endif
  اگر(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    اگر(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(data, "error loading CRL file: %s", ssl_crlfile);
      بازگشت CURLE_SSL_CRL_BADFILE;
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

  اگر(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (تهی)X509_STORE_set_flags(store, x509flags);

  بازگشت result;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_OSSL_X509_کلید  "tls:ossl:x509:share"

ساختار ossl_x509_share {
  نویسه *CAfile;         /* CAfile path used to generate X509 store */
  نویسه *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  ساختار curltime time; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

ایستا تهی oss_x509_share_free(ثابت تهی *کلید, size_t کلید_len, تهی *p)
{
  ساختار ossl_x509_share *share = p;
  DEBUGASSERT(کلید_len == CURL_CSTRLEN(MPROTO_OSSL_X509_کلید));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_کلید, کلید, کلید_len));
  (تهی)کلید;
  (تهی)کلید_len;
  اگر(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

ایستا bool ossl_cached_x509_store_expired(ساختار Curl_easy *data,
                                           ثابت ساختار ossl_x509_share *mb)
{
  اگر(data->set.ssl_ca_cache_timeout < 0)
    بازگشت FALSE;
  وگرنه {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &mb->time);
    timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

    بازگشت elapsed_ms >= timeout_ms;
  }
}

ایستا bool ossl_cached_x509_store_different(ساختار Curl_cfilter *cf,
                                             ثابت ساختار Curl_easy *data,
                                             ثابت ساختار ossl_x509_share *mb)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (تهی)data;
  اگر(mb->no_partialchain != conn_config->no_partialchain)
    بازگشت TRUE;
  اگر(!mb->CAfile || !conn_config->CAfile) {
    اگر(mb->CAfile != conn_config->CAfile)
      بازگشت TRUE;
  }
  وگرنه اگر(strcmp(mb->CAfile, conn_config->CAfile))
    بازگشت TRUE;

  اگر(!mb->CApath || !conn_config->CApath) {
    اگر(mb->CApath != conn_config->CApath)
      بازگشت TRUE;
  }
  وگرنه اگر(strcmp(mb->CApath, conn_config->CApath))
    بازگشت TRUE;

  بازگشت FALSE;
}

ایستا X509_STORE *ossl_get_cached_x509_store(ساختار Curl_cfilter *cf,
                                              ساختار Curl_easy *data,
                                              bool *pempty)
{
  ساختار Curl_multi *multi = data->multi;
  ساختار ossl_x509_share *share;
  X509_STORE *store = پوچ;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_کلید,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_کلید)) : پوچ;
  اگر(share && share->store &&
     !ossl_cached_x509_store_expired(data, share) &&
     !ossl_cached_x509_store_different(cf, data, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  بازگشت store;
}

ایستا تهی ossl_set_cached_x509_store(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       X509_STORE *store,
                                       bool is_empty)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار Curl_multi *multi = data->multi;
  ساختار ossl_x509_share *share;

  DEBUGASSERT(multi);
  اگر(!multi)
    بازگشت;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_کلید,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_کلید));

  اگر(!share) {
    share = curlx_calloc(1, اندازه(*share));
    اگر(!share)
      بازگشت;
    اگر(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_کلید,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_کلید),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      بازگشت;
    }
  }

  اگر(X509_STORE_up_ref(store)) {
    نویسه *CAfile = پوچ;
    نویسه *CApath = پوچ;

    اگر(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      اگر(!CAfile) {
        X509_STORE_free(store);
        بازگشت;
      }
    }

    اگر(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      اگر(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        بازگشت;
      }
    }

    اگر(share->store) {
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
#وگرنه
#define USING_PROVIDER(x) 0
#endif

CURLcode Curl_ssl_setup_x509_store(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   ساختار ossl_ctx *octx)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
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
  اگر(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  وگرنه {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    result = ossl_populate_x509_store(cf, data, octx, store);
    اگر(result == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, data, store, (bool)octx->store_is_empty);
    }
  }

  ERR_برداشت_to_mark();

  بازگشت result;
}

ایستا bool ossl_apply_session(
  ساختار ossl_ctx *octx,
  ساختار Curl_cfilter *cf,
  ساختار Curl_easy *data,
  ساختار alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  ساختار Curl_ssl_session *scs)
{
  ساختار ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ثابت unsigned نویسه *der_sessionid = scs->sdata;
  size_t der_sessionid_size = scs->sdata_len;
  SSL_SESSION *ssl_session = پوچ;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(پوچ, &der_sessionid,
                                (long)der_sessionid_size);
  اگر(ssl_session) {
    اگر(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(نویسه error_buffer[256]);
      infof(data, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          اندازه(error_buffer)));
    }
    وگرنه {
      اگر(conn_cfg->verifypeer &&
         (SSL_get_verify_result(octx->ssl) != X509_V_OK)
#ifdef USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#endif
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, پوچ);
        infof(data, "SSL session not peer verified, not reusing");
      }
      وگرنه {
        infof(data, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ifdef USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#endif
        infof(data, "SSL verify result: %lx",
              (unsigned long)SSL_get_verify_result(octx->ssl));
#ifdef HAVE_OPENSSL_EARLYDATA
        اگر(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_data(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_data = FALSE;
          اگر(sess_reuse_cb)
            (تهی)sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          اگر(do_early_data) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_to(alpns, scs->alpn);
          }
        }
#وگرنه
        (تهی)alpns;
        (تهی)conn_cfg;
        (تهی)sess_reuse_cb;
#endif
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  وگرنه {
    infof(data, "SSL session not accepted by OpenSSL, continuing without");
  }
  بازگشت (bool)octx->reused_session;
}

ایستا CURLcode ossl_init_session_and_alpns(
  ساختار ossl_ctx *octx,
  ساختار Curl_cfilter *cf,
  ساختار Curl_easy *data,
  ساختار ssl_peer *peer,
  ثابت ساختار alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  ساختار ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ساختار alpn_spec alpns;
  CURLcode result;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  اگر((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ساختار Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    اگر(scs) {
      اگر(ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
    }
  }

  اگر(!octx->reused_session &&
     Curl_ssl_scache_use(cf, data) && !conn_cfg->verifystatus) {
    ساختار Curl_ssl_session *scs = پوچ;

    result = Curl_ssl_scache_take(cf, data, peer->scache_کلید, &scs);
    اگر(!result && scs && scs->sdata && scs->sdata_len) {
      (تهی)ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, data, peer->scache_کلید, scs);
  }

  اگر(alpns.شمار) {
    ساختار alpn_proto_buf proto;
    memset(&proto, 0, اندازه(proto));
    result = Curl_alpn_to_proto_buf(&proto, &alpns);
    اگر(result) {
      failf(data, "Error determining ALPN");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
    اگر(SSL_set_alpn_protos(octx->ssl, proto.data, proto.len)) {
      failf(data, "Error setting ALPN");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
  }

  بازگشت CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
ایستا CURLcode ossl_init_ech(ساختار ossl_ctx *octx,
                              ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              ساختار ssl_peer *peer)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحیح trying_ech_now = 0;

  اگر(!Curl_ssl_ech_enabled(cf))
    بازگشت CURLE_OK;

  اگر(conn_config->ech == CURLECH_GREASE) {
    infof(data, "ECH: will GREASE ClientHello");
#ifdef HAVE_BORINGSSL_LIKE
    SSL_set_enable_ech_grease(octx->ssl, 1);
#وگرنه
    SSL_set_options(octx->ssl, SSL_OP_ECH_GREASE);
#endif
  }
  وگرنه اگر(conn_config->ech && conn_config->ech_config) {
#ifdef HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode result;

    اگر(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * strlen(conn_config->ech_config);
    result = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    اگر(result || !ech_config) {
      infof(data, "ECH: cannot base64 decode ECHConfig from command line");
      اگر(conn_config->ech == CURLECH_HARD)
        بازگشت result;
    }
    اگر(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      اگر(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        بازگشت CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#وگرنه
    size_t ech_config_len = 0;
    اگر(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = strlen(conn_config->ech_config);
    اگر(SSL_set1_ech_config_list(octx->ssl,
                                (ثابت uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      اگر(conn_config->ech == CURLECH_HARD)
        بازگشت CURLE_SSL_CONNECT_ERROR;
    }
    وگرنه
      trying_ech_now = 1;
#endif /* HAVE_BORINGSSL_LIKE */
    infof(data, "ECH: ECHConfig from command line");
  }
  وگرنه {
    ثابت ساختار Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, peer->origin);

    اگر(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ثابت unsigned نویسه *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(data, "ECH: ECHConfig from HTTPS RR");
      اگر(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(data, "ECH: SSL_set1_ech_config_list failed");
        اگر(conn_config->ech == CURLECH_HARD)
          بازگشت CURLE_SSL_CONNECT_ERROR;
      }
      وگرنه {
        trying_ech_now = 1;
        infof(data, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    وگرنه {
      infof(data, "ECH: requested but no ECHConfig available");
      اگر(conn_config->ech == CURLECH_HARD)
        بازگشت CURLE_SSL_CONNECT_ERROR;
    }
  }
#ifdef HAVE_BORINGSSL_LIKE
  (تهی)peer;
  اگر(trying_ech_now && conn_config->ech_public) {
    infof(data, "ECH: setting public_name not supported with BoringSSL");
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }
#وگرنه
  اگر(trying_ech_now && conn_config->ech_public) {
    صحیح ret;
    infof(data, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    اگر(ret != 1) {
      infof(data, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
  }
#endif /* HAVE_BORINGSSL_LIKE */
  اگر(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(data, "ECH: cannot force TLSv1.3 [ERROR]");
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  بازگشت CURLE_OK;
}
#endif /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

ایستا CURLcode ossl_init_ssl(ساختار ossl_ctx *octx,
                              ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              ساختار ssl_peer *peer,
                              ثابت ساختار alpn_spec *alpns_requested,
                              تهی *ssl_user_data,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  اگر(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  اگر(!octx->ssl) {
    failf(data, "SSL: could not create a context (handle)");
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_data(octx->ssl, ssl_user_data);

#ifndef OPENSSL_NO_OCSP
  اگر(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#endif

  SSL_set_connect_state(octx->ssl);

  اگر(peer->sni) {
    اگر(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(data, "Failed set SNI");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
  }

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode result = ossl_init_ech(octx, cf, data, peer);
    اگر(result)
      بازگشت result;
  }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  بازگشت ossl_init_session_and_alpns(octx, cf, data, peer,
                                     alpns_requested, sess_reuse_cb);
}

ایستا CURLcode ossl_init_method(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار ssl_peer *peer,
                                 ثابت SSL_METHOD **pmethod,
                                 unsigned صحیح *pssl_version_min)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = پوچ;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  انتخاب(peer->transport) {
  حالت TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    انتخاب(*pssl_version_min) {
    حالت CURL_SSLVERSION_TLSv1:
    حالت CURL_SSLVERSION_TLSv1_0:
    حالت CURL_SSLVERSION_TLSv1_1:
    حالت CURL_SSLVERSION_TLSv1_2:
    حالت CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      بشکن;
    حالت CURL_SSLVERSION_SSLv2:
      failf(data, "No SSLv2 support");
      بازگشت CURLE_NOT_BUILT_IN;
    حالت CURL_SSLVERSION_SSLv3:
      failf(data, "No SSLv3 support");
      بازگشت CURLE_NOT_BUILT_IN;
    پیشفرض:
      failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
    بشکن;
  حالت TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    اگر(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(data, "QUIC needs at least TLS version 1.3");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    بشکن;
  پیشفرض:
    failf(data, "unsupported transport %d in SSL init", peer->transport);
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  بازگشت *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(ساختار ossl_ctx *octx,
                            ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            ساختار ssl_peer *peer,
                            ثابت ساختار alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            تهی *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            تهی *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *ciphers;
  ثابت SSL_METHOD *req_method = پوچ;
  ctx_option_t ctx_options = 0;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  نویسه * ثابت ssl_cert = conn_config->clientcert;
  ثابت ساختار curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ثابت نویسه * ثابت ssl_cert_type = conn_config->cert_type;
  unsigned صحیح ssl_version_min = CURL_SSLVERSION_DEFAULT;
  نویسه error_buffer[256];

  /* Make funny stuff to get random input */
  result = ossl_seed(data);
  اگر(!result) {
    اگر(CURL_EASY_STR(data, STRING_SSL_ENGINE))
      result = ossl_set_engine(data, octx,
                               CURL_EASY_STR(data, STRING_SSL_ENGINE));
    اگر(!result && data->set.engine_default)
      result = ossl_set_engine_default(data, octx);
  }

  اگر(!result) {
    ssl_config->certverifyresult = !X509_V_OK;
    result = ossl_init_method(cf, data, peer, &req_method, &ssl_version_min);
  }
  اگر(result)
    بازگشت result;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ifdef OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#endif
    SSL_CTX_new(req_method);

  اگر(!octx->ssl_ctx) {
    failf(data, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_buffer, اندازه(error_buffer)));
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  اگر(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    اگر(result)
      بازگشت result;
  }

  اگر(data->set.fdebug && data->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_پیام_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_پیام_callback_arg(octx->ssl_ctx, cf);
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
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_درج_EMPTY_FRAGMENTS;

  DEBUGASSERT(ssl_version_min != CURL_SSLVERSION_DEFAULT);
  انتخاب(ssl_version_min) {
  حالت CURL_SSLVERSION_SSLv2:
  حالت CURL_SSLVERSION_SSLv3:
    بازگشت CURLE_NOT_BUILT_IN;

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

    result = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    اگر(result)
      بازگشت result;
    بشکن;

  پیشفرض:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    بازگشت CURLE_SSL_CONNECT_ERROR;
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
  اگر(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = پوچ;
  اگر(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    اگر(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(data, "failed setting cipher list: %s", ciphers);
      بازگشت CURLE_SSL_CIPHER;
    }
    infof(data, "Cipher selection: %s", ciphers);
  }

#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    ثابت نویسه *ciphers13 = conn_config->cipher_list13;
    اگر(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      اگر(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(data, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        بازگشت CURLE_SSL_CIPHER;
      }
      infof(data, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#endif

  اگر(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    result = client_cert(data, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->کلید, conn_config->کلید_blob,
                         conn_config->کلید_type,
                         conn_config->کلید_passwd);
    اگر(result)
      /* failf() is already done in client_cert() */
      بازگشت result;
  }

#ifdef HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#endif

  {
    ثابت نویسه *curves = conn_config->curves;
    اگر(curves) {
#ifdef HAVE_BORINGSSL_LIKE
#define OSSL_CURVE_CAST(x) (x)
#وگرنه
#define OSSL_CURVE_CAST(x) (نویسه *)CURL_UNCONST(x)
#endif
      اگر(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(data, "failed setting curves list: '%s'", curves);
        بازگشت CURLE_SSL_CIPHER;
      }
    }
  }

#ifdef HAVE_SSL_CTX_SET1_SIGALGS
#define OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    ثابت نویسه *signature_algorithms = conn_config->signature_algorithms;
    اگر(signature_algorithms) {
      اگر(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(data, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        بازگشت CURLE_SSL_CIPHER;
      }
    }
  }
#endif

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, پوچ);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#اگر !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  اگر(Curl_tls_keylog_enabled()) {
    SSL_CTX_set_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#endif

  اگر(cb_new_session) {
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
  اگر(data->set.ssl_fsslctx) {
    ساختار Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    اگر(!octx->x509_store_setup) {
      result = Curl_ssl_setup_x509_store(cf, data, octx);
      اگر(result)
        بازگشت result;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, data, easy_fsslctx);
    result = (*data->set.ssl_fsslctx)(data, octx->ssl_ctx,
                                      data->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    اگر(result) {
      failf(data, "error signaled by SSL ctx callback");
      بازگشت result;
    }
  }

  بازگشت ossl_init_ssl(octx, cf, data, peer, alpns_requested,
                       ssl_user_data, sess_reuse_cb);
}

ایستا CURLcode ossl_on_session_reuse(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      ساختار alpn_spec *alpns,
                                      ساختار Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  بازگشت Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max);
}

تهی Curl_ossl_report_handshake(ساختار Curl_easy *data, ساختار ossl_ctx *octx)
{
#ifdef CURLVERBOSE
  اگر(Curl_trc_is_verbose(data)) {
    صحیح psigtype_nid = NID_undef;
    ثابت نویسه *negotiated_group_name = پوچ;

#ifdef HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#اگر OPENSSL_VERSION_عدد >= 0x30200000L
    negotiated_group_name = SSL_get0_group_name(octx->ssl);
#وگرنه
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
#وگرنه
  (تهی)data;
  (تهی)octx;
#endif /* CURLVERBOSE */
}

ایستا CURLcode ossl_connect_step1(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  result = Curl_ossl_ctx_init(octx, cf, data, &connssl->peer,
                              connssl->alpn, پوچ, پوچ,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  اگر(result)
    بازگشت result;

  octx->bio_method = ossl_bio_cf_method_create();
  اگر(!octx->bio_method)
    بازگشت CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  اگر(!bio)
    بازگشت CURLE_OUT_OF_MEMORY;

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
#وگرنه
  SSL_set_bio(octx->ssl, bio, bio);
#endif

  اگر(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ساختار alpn_proto_buf proto;
    memset(&proto, 0, اندازه(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }

  connssl->connecting_state = ssl_connect_2;
  بازگشت CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
ایستا صحیح ossl_trace_ech_retry_configs(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data, SSL *ssl,
                                        صحیح reason)
{
  CURLcode result = CURLE_OK;
  size_t rcl = 0;
  صحیح rv = 1;
#ifndef HAVE_BORINGSSL_LIKE
  نویسه *inner = پوچ;
  uint8_t *rcs = پوچ;
  نویسه *outer = پوچ;
#وگرنه
  ثابت نویسه *inner = پوچ;
  ثابت uint8_t *rcs = پوچ;
  ثابت نویسه *outer = پوچ;
  size_t out_name_len = 0;
  صحیح servername_type = 0;
#endif
  NOVERBOSE((تهی)reason);

  /* nothing to trace if not doing ECH */
  اگر(!Curl_ssl_ech_enabled(cf))
    بازگشت rv;
#ifndef HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#وگرنه
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (صحیح)rcl;
#endif

  اگر(rv && rcs) {
    نویسه *b64str = پوچ;
    size_t blen = 0;

    result = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    اگر(!result && b64str) {
      infof(data, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ifndef HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#وگرنه
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#endif
    }
  }
  وگرنه
    infof(data, "ECH: no retry_configs (rv = %d)", rv);
#ifndef HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#endif
  بازگشت rv;
}

#endif

ایستا CURLcode ossl_connect_step2(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  ساختار ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  صحیح err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  اگر(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, octx);
    اگر(result)
      بازگشت result;
    octx->x509_store_setup = TRUE;
  }

#اگر !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  اگر(Curl_tls_keylog_enabled() && !octx->keylog_done)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_done);
#endif

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  اگر(err != 1) {
    صحیح detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(data, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    اگر(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      بازگشت CURLE_AGAIN;
    }
    اگر(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      بازگشت CURLE_AGAIN;
    }
#ifdef SSL_ERROR_WANT_ASYNC
    اگر(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      بازگشت CURLE_AGAIN;
    }
#endif
#ifdef SSL_ERROR_WANT_RETRY_VERIFY
    اگر(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(data, TRUE);
      بازگشت CURLE_AGAIN;
    }
#endif
    وگرنه {
      /* untreated error */
      sslerr_t errdetail;
      نویسه error_buffer[256] = "";
      CURLcode result;
      long lerr;
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
#ifdef SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#endif
         )) {
        result = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_result(octx->ssl);
        اگر(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        وگرنه
          failf(data, "%s", "SSL certificate verification failed");
      }
#ifdef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      وگرنه اگر((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        result = CURLE_SSL_CLIENTCERT;
        failf(data, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_buffer, اندازه(error_buffer)));
      }
#endif
#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
      وگرنه اگر((lib == ERR_LIB_SSL) &&
#ifndef HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#وگرنه
              (reason == SSL_R_ECH_REJECTED)) {
#endif /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, reason);

        result = CURLE_ECH_REQUIRED;
        failf(data, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_buffer, اندازه(error_buffer)));
      }
#endif
      وگرنه {
        result = CURLE_SSL_CONNECT_ERROR;
        failf(data, "TLS connect error: %s",
              ossl_strerror(errdetail, error_buffer, اندازه(error_buffer)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      اگر(result == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        نویسه extramsg[80] = "";
        صحیح sockerr = SOCKERRNO;

        اگر(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, اندازه(extramsg));
        failf(data, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      بازگشت result;
    }
  }
  وگرنه {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(data, octx);

#اگر defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    اگر(Curl_ssl_ech_enabled(cf)) {
      ساختار ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      نویسه *inner = پوچ, *outer = پوچ;
      صحیح rv;
      VERBOSE(ثابت نویسه *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      انتخاب(rv) {
      حالت SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        بشکن;
      حالت SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        بشکن;
      حالت SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        بشکن;
      حالت SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        بشکن;
      حالت SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        بشکن;
      حالت SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        بشکن;
      حالت SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        بشکن;
      حالت SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        بشکن;
      حالت SSL_ECH_STATUS_BAD_NAME: {
        اگر(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        وگرنه {
          VERBOSE(status = "bad name (unexpected)");
        }
        بشکن;
      }
      پیشفرض:
        VERBOSE(status = "unexpected status");
        infof(data, "ECH: unexpected status %d", rv);
      }
      infof(data, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      اگر(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, 0);
      }
      اگر(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(data, "ECH: ech-hard failed");
        بازگشت CURLE_SSL_CONNECT_ERROR;
      }
    }
    وگرنه {
      infof(data, "ECH: result: status is not attempted");
    }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    اگر(connssl->alpn) {
      ثابت unsigned نویسه *neg_protocol;
      unsigned صحیح len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      بازگشت Curl_alpn_set_negotiated(cf, data, connssl, neg_protocol, len);
    }

    بازگشت CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
ایستا CURLcode ossl_pkp_pin_peer_pubkey(ساختار Curl_easy *data, X509 *cert,
                                         ثابت نویسه *pinnedpubkey)
{
  /* Scratch */
  صحیح len1 = 0, len2 = 0;
  unsigned نویسه *buff1 = پوچ, *temp = پوچ;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  اگر(!pinnedpubkey)
    بازگشت CURLE_OK;

  اگر(!cert)
    بازگشت result;

  انجامبده {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), پوچ);
    اگر(len1 < 1)
      بشکن; /* failed */

    buff1 = temp = curlx_malloc(len1);
    اگر(!buff1)
      بشکن; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &temp);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    اگر((len1 != len2) || !temp || ((temp - buff1) != len1))
      بشکن; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } وقتی(0);

  اگر(buff1)
    curlx_free(buff1);

  بازگشت result;
}

#ifdef CURLVERBOSE
#اگر !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_عدد) && LIBRESSL_VERSION_عدد < 0x3060000fL)
ایستا تهی infof_certstack(ساختار Curl_easy *data, ثابت SSL *ssl)
{
  پشته_OF(X509) *certstack;
  long verify_result;
  صحیح رقم_cert_levels;
  صحیح cert_level;

  اگر(!Curl_trc_is_verbose(data))
    بازگشت;

  verify_result = SSL_get_verify_result(ssl);
  اگر(verify_result != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  وگرنه
    certstack = SSL_get0_verified_chain(ssl);
  اگر(!certstack)
    بازگشت;
  رقم_cert_levels = sk_X509_رقم(certstack);

  برای(cert_level = 0; cert_level < رقم_cert_levels; cert_level++) {
    نویسه cert_algorithm[80] = "";
    نویسه group_name_final[80] = "";
    ثابت X509_ALGOR *palg_cert = پوچ;
    ثابت ASN1_OBJECT *paobj_cert = پوچ;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    صحیح کلید_bits;
    صحیح کلید_sec_bits;
    صحیح get_group_name;
    ثابت نویسه *type_name;

    current_cert = sk_X509_مقدار(certstack, cert_level);
    اگر(!current_cert)
      ادامه;

    current_pkey = X509_get0_pubkey(current_cert);
    اگر(!current_pkey)
      ادامه;

    X509_get0_signature(پوچ, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, پوچ, پوچ, palg_cert);
    OBJ_obj2txt(cert_algorithm, اندازه(cert_algorithm), paobj_cert, 0);

    کلید_bits = EVP_PKEY_bits(current_pkey);
#ifndef HAVE_OPENSSL3
#define EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#endif
    کلید_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#ifdef HAVE_OPENSSL3
    {
      نویسه group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               اندازه(group_name), پوچ);
      curl_msnprintf(group_name_final, اندازه(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#وگرنه
    get_group_name = 0;
    type_name = پوچ;
#endif

    infof(data, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          کلید_bits, کلید_sec_bits, cert_algorithm);
  }
}
#وگرنه
#define infof_certstack(data, ssl)
#endif
#endif /* CURLVERBOSE */

ایستا CURLcode ossl_check_issuer(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  X509 *server_cert)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = پوچ;
  BIO *fp = پوچ;
  نویسه err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode result = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  اگر(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->data,
                         (صحیح)conn_config->issuercert_blob->len);
    اگر(!fp) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, اندازه(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }
  }
  وگرنه اگر(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    اگر(!fp) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, اندازه(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      برو out;
    }

    اگر(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      اگر(verify_enabled)
        failf(data, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      برو out;
    }
  }

  اگر(fp) {
    issuer = PEM_read_bio_X509(fp, پوچ, ZERO_NULL, پوچ);
    اگر(!issuer) {
      اگر(verify_enabled)
        failf(data, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      برو out;
    }

    اگر(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      اگر(verify_enabled)
        failf(data, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      برو out;
    }

    infof(data, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  اگر(fp)
    BIO_free(fp);
  اگر(issuer)
    X509_free(issuer);
  بازگشت result;
}

ایستا ثابت نویسه *pinned(ساختار Curl_cfilter *cf,
                          ساختار Curl_easy *data)
{
  (تهی)cf;
  بازگشت
#ifndef CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#endif
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
}

ایستا CURLcode ossl_check_pinned_کلید(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      X509 *server_cert)
{
  CURLcode result = CURLE_OK;
  ثابت نویسه *ptr = pinned(cf, data);
  اگر(ptr) {
    result = ossl_pkp_pin_peer_pubkey(data, server_cert, ptr);
    اگر(result)
      failf(data, "SSL: public key does not match pinned public key");
  }
  بازگشت result;
}

#ifdef CURLVERBOSE
#define MAX_CERT_NAME_LENGTH 2048
ایستا CURLcode ossl_infof_cert(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                X509 *server_cert)
{
  BIO *mem = پوچ;
  ساختار dynbuf dname;
  نویسه err_buf[256] = "";
  نویسه *buf;
  long len;
  CURLcode result = CURLE_OK;

  اگر(!Curl_trc_is_verbose(data))
    بازگشت CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_LENGTH);
  mem = BIO_new(BIO_s_mem());
  اگر(!mem) {
    failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, اندازه(err_buf)));
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  infof(data, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  result = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(data, "  subject: %s", result ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_data(mem, (نویسه **)&buf);
  infof(data, "  start date: %.*s", (صحیح)len, buf);
  (تهی)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_data(mem, (نویسه **)&buf);
  infof(data, "  expire date: %.*s", (صحیح)len, buf);
  (تهی)BIO_reset(mem);

  result = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  اگر(result) /* should be only fatal stuff like OOM */
    برو out;
  infof(data, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  بازگشت result;
}
#endif /* CURLVERBOSE */

#ifdef USE_APPLE_SECTRUST
ساختار ossl_certs_ctx {
  پشته_OF(X509) *sk;
  size_t رقم_certs;
  unsigned نویسه *last_der;
};

ایستا CURLcode ossl_chain_get_der(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   تهی *user_data,
                                   size_t i,
                                   unsigned نویسه **pder,
                                   size_t *pder_len)
{
  ساختار ossl_certs_ctx *chain = user_data;
  X509 *cert;
  صحیح der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = پوچ;

  (تهی)cf;
  (تهی)data;
  *pder_len = 0;
  *pder = پوچ;

  اگر(i >= chain->رقم_certs)
    بازگشت CURLE_TOO_LARGE;
  cert = sk_X509_مقدار(chain->sk, (صحیح)i);
  اگر(!cert)
    بازگشت CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  اگر(der_len < 0)
    بازگشت CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (size_t)der_len;
  بازگشت CURLE_OK;
}

ایستا CURLcode ossl_apple_verify(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ساختار ossl_ctx *octx,
                                  ساختار ssl_peer *peer)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار ossl_certs_ctx chain;
  CURLcode result;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, اندازه(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.رقم_certs = chain.sk ? sk_X509_رقم(chain.sk) : 0;

  اگر(!chain.رقم_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    اگر(!octx->reused_session) {
      failf(data, "SSL: could not get peer certificate chain");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    وگرنه {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(data, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      بازگشت CURLE_OK;
    }
  }
  وگرنه {
#ifdef HAVE_BORINGSSL_LIKE
    ثابت uint8_t *ocsp_data = پوچ;
#وگرنه
    unsigned نویسه *ocsp_data = پوچ;
#endif
    long ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    اگر(!octx->reused_session)
      ocsp_len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_data);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    اگر(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_data = پوچ;
    }

    result = Curl_vtls_apple_verify(cf, data, peer, chain.رقم_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_data, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = پوچ;
  }
  octx->sectrust_verified = !result;
  بازگشت result;
}
#endif /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   ساختار ossl_ctx *octx,
                                   ساختار ssl_peer *peer)
{
  ساختار connectdata *conn = cf->conn;
  ساختار ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  long ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  اگر(data->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    result = ossl_certchain(data, octx->ssl);
    اگر(result)
      بازگشت result;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  اگر(!server_cert) {
    /* no verification at all, this maybe acceptable */
    اگر(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, data))
      برو out;

    failf(data, "SSL: could not get peer certificate");
    result = CURLE_PEER_FAILED_VERIFICATION;
    برو out;
  }

#ifdef CURLVERBOSE
  result = ossl_infof_cert(cf, data, server_cert);
  اگر(result)
    برو out;
  infof_certstack(data, octx->ssl);
#endif

  اگر(conn_config->verifyhost) {
    result = ossl_verifyhost(data, conn, peer, server_cert);
    اگر(result)
      برو out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_result(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(data, "OpenSSL verify result: %lx", (unsigned long)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  اگر(verified)
    infof(data, "SSL certificate verified via OpenSSL.");

#ifdef USE_APPLE_SECTRUST
  اگر(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    result = ossl_apple_verify(cf, data, octx, peer);
    اگر(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      برو out; /* unexpected error */
    اگر(octx->sectrust_verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#endif

  اگر(!verified) {
    /* no trust established, report the OpenSSL status */
    اگر(conn_config->verifypeer) {
      failf(data, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      result = CURLE_PEER_FAILED_VERIFICATION;
      برو out;
    }
    infof(data, " SSL certificate verification failed, continuing anyway!");
  }

#ifndef OPENSSL_NO_OCSP
  اگر(conn_config->verifystatus &&
#ifdef USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, data)) &&
#endif
     !octx->reused_session) {
    result = verifystatus(cf, data, octx);
    اگر(result)
      برو out;
  }
#endif

  result = ossl_check_issuer(cf, data, server_cert);
  اگر(result)
    برو out;

  result = ossl_check_pinned_کلید(cf, data, server_cert);

out:
  X509_free(server_cert);
  بازگشت result;
}

ایستا CURLcode ossl_connect_step3(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  result = Curl_ossl_check_peer_cert(cf, data, octx, &connssl->peer);
  اگر(result)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, data, connssl->peer.scache_کلید);

  بازگشت result;
}

#ifdef HAVE_OPENSSL_EARLYDATA
ایستا CURLcode ossl_send_earlydata(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  ثابت unsigned نویسه *buf;
  size_t blen, nwritten;
  صحیح rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_result = CURLE_OK;
  وقتی(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_data(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    اگر(rc <= 0) {
      long sslerror;
      نویسه error_buffer[256];
      صحیح err = SSL_get_error(octx->ssl, rc);

      انتخاب(err) {
      حالت SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        result = CURLE_AGAIN;
        برو out;
      حالت SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_AGAIN;
        برو out;
      حالت SSL_ERROR_SYSCALL: {
        صحیح sockerr = SOCKERRNO;

        اگر(octx->io_result == CURLE_AGAIN) {
          result = CURLE_AGAIN;
          برو out;
        }
        sslerror = ERR_get_error();
        اگر(sslerror)
          ossl_strerror(sslerror, error_buffer, اندازه(error_buffer));
        وگرنه اگر(sockerr)
          curlx_strerror(sockerr, error_buffer, اندازه(error_buffer));
        وگرنه
          curl_msnprintf(error_buffer, اندازه(error_buffer), "%s",
                         SSL_ERROR_to_str(err));

        failf(data, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_SEND_ERROR;
        برو out;
      }
      حالت SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(data, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_buffer, اندازه(error_buffer)));
        result = CURLE_SEND_ERROR;
        برو out;
      }
      پیشفرض:
        /* a true error */
        failf(data, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        result = CURLE_SEND_ERROR;
        برو out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  بازگشت result;
}
#endif /* HAVE_OPENSSL_EARLYDATA */

ایستا CURLcode ossl_connect(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             bool *done)
{
  CURLcode result = CURLE_OK;
  ساختار ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  اگر(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  اگر(connssl->connecting_state == ssl_connect_1) {
    اگر(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
      بازگشت CURLE_OK;
    }
    CURL_TRC_CF(data, cf, "ossl_connect, step1");
    result = ossl_connect_step1(cf, data);
    اگر(result)
      برو out;
  }

  اگر(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(data, cf, "ossl_connect, step2");
#ifdef HAVE_OPENSSL_EARLYDATA
    اگر(connssl->earlydata_state == ssl_earlydata_await) {
      برو out;
    }
    وگرنه اگر(connssl->earlydata_state == ssl_earlydata_sending) {
      result = ossl_send_earlydata(cf, data);
      اگر(result)
        برو out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#endif
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    result = ossl_connect_step2(cf, data);
    اگر(result)
      برو out;
  }

  اگر(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(data, cf, "ossl_connect, step3");
    result = ossl_connect_step3(cf, data);
    اگر(result)
      برو out;
    connssl->connecting_state = ssl_connect_done;
#ifdef HAVE_OPENSSL_EARLYDATA
    اگر(connssl->earlydata_state > ssl_earlydata_none) {
      ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_data_status(octx->ssl) == SSL_EARLY_DATA_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
  }

  اگر(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(data, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  اگر(result == CURLE_AGAIN) {
    *done = FALSE;
    بازگشت CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  بازگشت result;
}

ایستا bool ossl_data_pending(ساختار Curl_cfilter *cf,
                              ثابت ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  (تهی)data;
  بازگشت (bool)connssl->درون_داد_pending;
}

ایستا CURLcode ossl_send(ساختار Curl_cfilter *cf,
                          ساختار Curl_easy *data,
                          ثابت تهی *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  صحیح err;
  نویسه error_buffer[256];
  sslerr_t sslerror;
  صحیح memlen;
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  صحیح nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)INT_MAX) ? INT_MAX : (صحیح)len;
  اگر(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    اگر(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  اگر(nwritten > 0) {
    اگر(nwritten > memlen) {
      failf(data, "SSL_write(len=%d) returned %d", memlen, nwritten);
      result = CURLE_SEND_ERROR;
      برو out;
    }
    *pnwritten = (size_t)nwritten;
  }
  وگرنه {
    err = SSL_get_error(octx->ssl, nwritten);

    انتخاب(err) {
    حالت SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      result = CURLE_AGAIN;
      برو out;
    حالت SSL_ERROR_WANT_WRITE:
      result = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      برو out;
    حالت SSL_ERROR_SYSCALL: {
      صحیح sockerr = SOCKERRNO;

      اگر(octx->io_result == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        result = CURLE_AGAIN;
        برو out;
      }
      sslerror = ERR_get_error();
      اگر(sslerror)
        ossl_strerror(sslerror, error_buffer, اندازه(error_buffer));
      وگرنه اگر(sockerr)
        curlx_strerror(sockerr, error_buffer, اندازه(error_buffer));
      وگرنه
        curl_msnprintf(error_buffer, اندازه(error_buffer), "%s",
                       SSL_ERROR_to_str(err));

      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_buffer, sockerr);
      result = CURLE_SEND_ERROR;
      برو out;
    }
    حالت SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(data, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_buffer, اندازه(error_buffer)));
      result = CURLE_SEND_ERROR;
      برو out;
    }
    پیشفرض:
      /* a true error */
      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      result = CURLE_SEND_ERROR;
      برو out;
    }
  }

out:
  بازگشت result;
}

ایستا CURLcode ossl_recv(ساختار Curl_cfilter *cf,
                          ساختار Curl_easy *data,   /* transfer */
                          نویسه *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  نویسه error_buffer[256];
  unsigned long sslerror;
  صحیح buffsize;
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  صحیح nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)INT_MAX) ? INT_MAX : (صحیح)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  اگر(nread > 0)
    *pnread = (size_t)nread;
  وگرنه {
    /* failed SSL_read */
    صحیح err = SSL_get_error(octx->ssl, nread);

    انتخاب(err) {
    حالت SSL_ERROR_NONE: /* this is not an error */
      بشکن;
    حالت SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      اگر(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(data, cf, "TLS close_notify");
      بشکن;
    حالت SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      برو out;
    حالت SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      برو out;
    پیشفرض:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      اگر(octx->io_result == CURLE_AGAIN) {
        result = CURLE_AGAIN;
        برو out;
      }
      sslerror = ERR_get_error();
      اگر((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        صحیح sockerr = SOCKERRNO;
        اگر(sslerror)
          ossl_strerror(sslerror, error_buffer, اندازه(error_buffer));
        وگرنه اگر(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_buffer, اندازه(error_buffer));
        وگرنه
          curl_msnprintf(error_buffer, اندازه(error_buffer), "%s",
                         SSL_ERROR_to_str(err));
        failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_RECV_ERROR;
        برو out;
      }
      وگرنه اگر(err == SSL_ERROR_SYSCALL) {
        اگر(octx->io_result) {
          /* logging handling in underlying filter already */
          result = octx->io_result;
        }
        وگرنه اگر(connssl->peer_closed) {
          failf(data, "Connection closed abruptly");
          result = CURLE_RECV_ERROR;
        }
        وگرنه {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          صحیح sockerr = SOCKERRNO;
          اگر(sockerr)
            curlx_strerror(sockerr, error_buffer, اندازه(error_buffer));
          وگرنه {
            curl_msnprintf(error_buffer, اندازه(error_buffer),
                           "Connection closed abruptly");
          }
          failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_buffer, sockerr);
          result = CURLE_RECV_ERROR;
        }
        برو out;
      }
    }
  }

out:
  اگر((!result && !*pnread) || (result == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->درون_داد_pending = FALSE;
  }
  CURL_TRC_CF(data, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (صحیح)result, *pnread, connssl->درون_داد_pending);
  بازگشت result;
}

ایستا CURLcode ossl_get_channel_binding(ساختار Curl_easy *data,
                                         int8_t sockindex,
                                         ساختار dynbuf *binding)
{
  X509 *cert;
  صحیح mdnid;
  bool no_digest_acceptable = FALSE;
  ثابت EVP_MD *algo_type = پوچ;
  ثابت نویسه *algo_name = پوچ;
  unsigned صحیح length;
  unsigned نویسه buf[EVP_MAX_MD_SIZE];

  ایستا ثابت نویسه prefix[] = "tls-server-end-point:";
  ساختار connectdata *conn = data->conn;
  ساختار Curl_cfilter *cf = conn->cfilter[sockindex];
  ساختار ossl_ctx *octx = پوچ;
  CURLcode result = CURLE_OK;

  انجامبده {
    ثابت ساختار Curl_cftype *cft = cf->cft;
    ساختار ssl_connect_data *connssl = cf->ctx;

    اگر(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (ساختار ossl_ctx *)connssl->backend;
      بشکن;
    }

    cf = cf->next;
  } وقتی(cf);

  اگر(!octx) {
    failf(data, "Failed to find the SSL filter");
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  اگر(!cert)
    /* No server certificate, do not do channel binding */
    بازگشت CURLE_OK;

#ifdef HAVE_OPENSSL3
  {
    صحیح pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    اگر(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(data, "certificate signature algorithm not recognized");
      result = CURLE_SSL_INVALIDCERTSTATUS;
      برو out;
    }

    اگر(mdnid != NID_undef) {
      اگر(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      وگرنه
        algo_type = EVP_get_digestbynid(mdnid);
    }
    وگرنه اگر(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    وگرنه اگر(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      نویسه mdname[128] = "";
      صحیح rc = EVP_PKEY_get_default_digest_name(pkey, mdname, اندازه(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      اگر(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      وگرنه اگر(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(data, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#وگرنه /* HAVE_OPENSSL3 */

  اگر(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, پوچ)) {
    failf(data,
          "Unable to find digest NID for certificate signature algorithm");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  اگر(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  وگرنه {
    algo_type = EVP_get_digestbynid(mdnid);
    اگر(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(data, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      result = CURLE_SSL_INVALIDCERTSTATUS;
      برو out;
    }
  }

#endif /* HAVE_OPENSSL3, else */

  اگر(!algo_type) {
    اگر(no_digest_acceptable) {
      infof(data, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      result = CURLE_OK;
      برو out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(data, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو out;
  }

  اگر(!X509_digest(cert, algo_type, buf, &length)) {
    failf(data, "X509_digest() failed for channel binding");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    برو out;
  }

  /* Append "tls-server-end-point:" */
  result = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  اگر(result)
    برو out;

  /* Append digest */
  result = curlx_dyn_addn(binding, buf, length);

out:
  X509_free(cert);
  بازگشت result;
}

size_t Curl_ossl_version(نویسه *buffer, size_t size)
{
#ifdef LIBRESSL_VERSION_عدد
  نویسه *p;
  size_t شمار;
  ثابت نویسه *ver = OpenSSL_version(OPENSSL_VERSION);
  ایستا ثابت نویسه expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  اگر(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  شمار = curl_msnprintf(buffer, size, "%s/%s", OSSL_PACKAGE, ver);
  برای(p = buffer; *p; ++p) {
    اگر(ISBLANK(*p))
      *p = '_';
  }
  بازگشت شمار;
#وگرنهاگر defined(OPENSSL_IS_AWSLC)
  بازگشت curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_عدد_STRING);
#وگرنهاگر defined(OPENSSL_IS_BORINGSSL)
#ifdef CURL_BORINGSSL_VERSION
  بازگشت curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#وگرنه
  بازگشت curl_msnprintf(buffer, size, OSSL_PACKAGE);
#endif
#وگرنه /* OpenSSL 3+ */
  بازگشت curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
}

/* can be called with data == NULL */
ایستا CURLcode ossl_random(ساختار Curl_easy *data,
                            unsigned نویسه *entropy, size_t length)
{
  صحیح rc;
  اگر(data) {
    اگر(ossl_seed(data)) /* Initiate the seed if not already done */
      بازگشت CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  وگرنه {
    اگر(!rand_enough())
      بازگشت CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(length));
  بازگشت rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

ایستا CURLcode ossl_sha256sum(ثابت unsigned نویسه *درون_داد,
                               size_t len,
                               unsigned نویسه *sha256sum /* output */,
                               size_t unused)
{
  CURLcode result = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (تهی)unused;

  mdctx = EVP_MD_CTX_new();
  اگر(!mdctx)
    بازگشت CURLE_OUT_OF_MEMORY;
  اگر(!EVP_DigestInit_ex(mdctx, EVP_sha256(), پوچ)) {
    result = CURLE_FAILED_INIT;
    برو out;
  }
  اگر(!EVP_DigestUpdate(mdctx, درون_داد, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, پوچ))
    result = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  بازگشت result;
}

ایستا bool ossl_cert_status_request(تهی)
{
#ifndef OPENSSL_NO_OCSP
  بازگشت TRUE;
#وگرنه
  بازگشت FALSE;
#endif
}

ایستا تهی *ossl_get_internals(ساختار ssl_connect_data *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  ساختار ossl_ctx *octx = (ساختار ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  بازگشت info == CURLINFO_TLS_SESSION ?
    (تهی *)octx->ssl_ctx : (تهی *)octx->ssl;
}

ثابت ساختار Curl_ssl Curl_ssl_openssl = {
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

  اندازه(ساختار ossl_ctx),

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
