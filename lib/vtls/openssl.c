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
#include "curl_setup.h"

#ifdef USE_OPENSSL

#include "urldata.h"
#include "curl_trc.h"
#include "formdata.h" /* for the boundary function */
#include "url.h" /* for the SSL config check function */
#include "curlx/inet_pton.h"
#include "vtls/openssl.h"
#include "connect.h"
#include "progress.h"
#include "vtls/vtls.h"
#include "vtls/vtls_int.h"
#include "vtls/vtls_scache.h"
#include "vauth/vauth.h"
#include "vtls/keylog.h"
#include "vtls/hostcheck.h"
#include "transfer.h"
#include "multiif.h"
#include "curlx/strerr.h"
#include "curlx/strparse.h"
#include "curlx/strcopy.h"
#include "curlx/strdup.h"
#include "vdns/cf-dns.h"
#include "vdns/httpsrr.h"
#include "vtls/apple.h"
#ifdef USE_ECH
#include "curlx/base64.h"
#endif

#include <openssl/rand.h>
#include <openssl/x509v3.h>
#ifndef OPENSSL_NO_DSA
#include <openssl/dsa.h>
#endif
#include <openssl/dh.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/pkcs12.h>
#include <openssl/tls1.h>
#include <openssl/evp.h>

#නම් defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#include <openssl/ech.h>
#endif

#ifndef OPENSSL_NO_OCSP
#include <openssl/ocsp.h>
#endif

#නම් !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#define USE_OPENSSL_ENGINE
#include <openssl/engine.h>
#endif

#ifdef LIBRESSL_VERSION_NUMBER
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  නම් LIBRESSL_VERSION_NUMBER < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#elif !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#endif

#නම් defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#include <openssl/provider.h>
#include <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#define OPENSSL_HAS_PROVIDERS
#endif

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#නම් !defined(LIBRESSL_VERSION_NUMBER) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_KEY_EXPLICIT_PARAMS))
#define HAVE_SSL_CTX_SET_DEFAULT_READ_BUFFER_LEN 1
#endif

#නම් defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#include <openssl/ui.h>
#endif

#ifdef HAVE_OPENSSL3
#define HAVE_EVP_PKEY_GET_PARAMS 1
#endif

#ifdef HAVE_EVP_PKEY_GET_PARAMS
#include <openssl/core_names.h>
#define DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = NULL
#define FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#නැතහොත්
#define DECLARE_PKEY_PARAM_BIGNUM(name) const BIGNUM *name
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
#ifndef LIBRESSL_VERSION_NUMBER
#define HAVE_SSL_CTX_SET1_SIGALGS
#endif

#ifdef LIBRESSL_VERSION_NUMBER
#define OSSL_PACKAGE "LibreSSL"
#elif defined(OPENSSL_IS_AWSLC)
#define OSSL_PACKAGE "AWS-LC"
#elif defined(OPENSSL_IS_BORINGSSL)
#define OSSL_PACKAGE "BoringSSL"
#elif defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#define OSSL_PACKAGE "quictls"
#නැතහොත්
#define OSSL_PACKAGE "OpenSSL"
#endif

#ifdef HAVE_BORINGSSL_LIKE
typedef size_t numcert_t;
typedef uint32_t sslerr_t;
#නැතහොත්
typedef පූර්ණ_සංඛ්‍යා numcert_t;
typedef unsigned long sslerr_t;
#endif
#define ossl_valsize_t numcert_t

#නම් OPENSSL_VERSION_NUMBER >= 0x40100000L
#define ossl_ASN1_STRING_get_length ASN1_STRING_get_length
#නැතහොත්
static size_t ossl_ASN1_STRING_get_length(const ASN1_STRING *str)
{
   const පූර්ණ_සංඛ්‍යා length = ASN1_STRING_length(str);
   ආපසු length >= 0 ? (size_t)length : 0;
}
#endif

static CURLcode push_certinfo(sa_nracanaa Curl_easy *data,
                              BIO *mem, const var_na *label, පූර්ණ_සංඛ්‍යා num)
  WARN_UNUSED_RESULT;

static CURLcode push_certinfo(sa_nracanaa Curl_easy *data,
                              BIO *mem, const var_na *label, පූර්ණ_සංඛ්‍යා num)
{
  var_na *ptr;
  long len = BIO_get_mem_data(mem, &ptr);
  CURLcode result = Curl_ssl_push_certinfo_len(data, num, label, ptr, len);
  (void)BIO_reset(mem);
  ආපසු result;
}

static CURLcode pubkey_show(sa_nracanaa Curl_easy *data,
                            BIO *mem,
                            පූර්ණ_සංඛ්‍යා num,
                            const var_na *type,
                            const var_na *name,
                            const BIGNUM *bn) WARN_UNUSED_RESULT;

static CURLcode pubkey_show(sa_nracanaa Curl_easy *data,
                            BIO *mem,
                            පූර්ණ_සංඛ්‍යා num,
                            const var_na *type,
                            const var_na *name,
                            const BIGNUM *bn)
{
  var_na namebuf[32];

  curl_msnprintf(namebuf, parimaa_na(namebuf), "%s(%s)", type, name);

  නම්(bn)
    BN_print(mem, bn);
  ආපසු push_certinfo(data, mem, namebuf, num);
}

#define print_pubkey_BN(_type, _name, _num)           \
  pubkey_show(data, mem, _num, #_type, #_name, _name)

static පූර්ණ_සංඛ්‍යා asn1_object_dump(const ASN1_OBJECT *a, var_na *buf, size_t len)
{
  පූර්ණ_සංඛ්‍යා i = i2t_ASN1_OBJECT(buf, (පූර්ණ_සංඛ්‍යා)len, a);
  ආපසු (i >= (පූර්ණ_සංඛ්‍යා)len);  /* buffer too small */
}

static CURLcode X509V3_ext(sa_nracanaa Curl_easy *data,
                           පූර්ණ_සංඛ්‍යා certnum,
                           const STACK_OF(X509_EXTENSION) *extsarg)
{
  පූර්ණ_සංඛ්‍යා i;
  CURLcode result = CURLE_OK;
#ifdef LIBRESSL_VERSION_NUMBER
  STACK_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#නැතහොත්
  const STACK_OF(X509_EXTENSION) *exts = extsarg;
#endif

  නම්((පූර්ණ_සංඛ්‍යා)sk_X509_EXTENSION_num(exts) <= 0)
    /* no extensions, bail out */
    ආපසු result;

  සඳහා(i = 0; i < (පූර්ණ_සංඛ්‍යා)sk_X509_EXTENSION_num(exts); i++) {
    const ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_value(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    var_na namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    නම්(!bio_out)
      ආපසු result;

    obj = X509_EXTENSION_get_object(ext);

    නම්(asn1_object_dump(obj, namebuf, parimaa_na(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    නම්(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (const ASN1_STRING *)X509_EXTENSION_get_data(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    result = Curl_ssl_push_certinfo_len(data, certnum, namebuf, biomem->data,
                                        biomem->length);
    BIO_free(bio_out);
    නම්(result)
      bha_nga;
  }
  ආපසු result;
}

static CURLcode get_pkey_rsa(sa_nracanaa Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, පූර්ණ_සංඛ්‍යා i)
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
#නැතහොත්
  RSA_get0_key(rsa, &n, &e, NULL);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (පූර්ණ_සංඛ්‍යා)(n ? BN_num_bits(n) : 0));
  result = push_certinfo(data, mem, "RSA Public Key", i);
  නම්(!result) {
    result = print_pubkey_BN(rsa, n, i);
    නම්(!result)
      result = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  ආපසු result;
}

#ifndef OPENSSL_NO_DSA
static CURLcode get_pkey_dsa(sa_nracanaa Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, පූර්ණ_සංඛ්‍යා i)
{
  CURLcode result = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_key);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_KEY, &pub_key);
#නැතහොත්
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_key(dsa, &pub_key, NULL);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dsa, p, i);
  නම්(!result)
    result = print_pubkey_BN(dsa, q, i);
  නම්(!result)
    result = print_pubkey_BN(dsa, g, i);
  නම්(!result)
    result = print_pubkey_BN(dsa, pub_key, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_key);
  ආපසු result;
}
#endif /* !OPENSSL_NO_DSA */

static CURLcode get_pkey_dh(sa_nracanaa Curl_easy *data,
                            EVP_PKEY *pubkey, BIO *mem, පූර්ණ_සංඛ්‍යා i)
{
  CURLcode result;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_key);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_KEY, &pub_key);
#නැතහොත්
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_key(dh, &pub_key, NULL);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dh, p, i);
  නම්(!result)
    result = print_pubkey_BN(dh, q, i);
  නම්(!result)
    result = print_pubkey_BN(dh, g, i);
  නම්(!result)
    result = print_pubkey_BN(dh, pub_key, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_key);
  ආපසු result;
}

#ifdef HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
typedef const X509_PUBKEY pubkeytype_t;
#නැතහොත්
typedef X509_PUBKEY pubkeytype_t;
#endif

static CURLcode ossl_certchain(sa_nracanaa Curl_easy *data, SSL *ssl)
{
  CURLcode result;
  STACK_OF(X509) *sk;
  පූර්ණ_සංඛ්‍යා i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  නම්(!sk)
    ආපසු CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_num(sk);
  නම්(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(data, "%d certificates is more than allowed (%d)", (පූර්ණ_සංඛ්‍යා)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    ආපසු CURLE_SSL_CONNECT_ERROR;
  }

  result = Curl_ssl_init_certinfo(data, (පූර්ණ_සංඛ්‍යා)numcerts);
  නම්(result)
    ආපසු result;

  mem = BIO_new(BIO_s_mem());
  නම්(!mem)
    result = CURLE_OUT_OF_MEMORY;

  සඳහා(i = 0; !result && (i < (පූර්ණ_සංඛ්‍යා)numcerts); i++) {
    X509 *x = sk_X509_value(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = NULL;
    size_t j;
    const ASN1_BIT_STRING *psig = NULL;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    result = push_certinfo(data, mem, "Subject", i);
    නම්(result)
      bha_nga;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    result = push_certinfo(data, mem, "Issuer", i);
    නම්(result)
      bha_nga;

    BIO_printf(mem, "%lx", (unsigned long)X509_get_version(x));
    result = push_certinfo(data, mem, "Version", i);
    නම්(result)
      bha_nga;

    {
      ASN1_INTEGER *num = X509_get_serialNumber(x);
      const unsigned var_na *numdata = ASN1_STRING_get0_data(num);
      const size_t numlen = ossl_ASN1_STRING_get_length(num);
      නම්(ASN1_STRING_type(num) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      සඳහා(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    result = push_certinfo(data, mem, "Serial Number", i);
    නම්(result)
      bha_nga;

    {
      const X509_ALGOR *sigalg = NULL;
      pubkeytype_t *xpubkey = NULL;
      ASN1_OBJECT *pubkeyoid = NULL;

      X509_get0_signature(&psig, &sigalg, x);
      නම්(sigalg) {
        const ASN1_OBJECT *sigalgoid = NULL;
        X509_ALGOR_get0(&sigalgoid, NULL, NULL, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        result = push_certinfo(data, mem, "Signature Algorithm", i);
        නම්(result)
          bha_nga;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      නම්(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, NULL, NULL, NULL, xpubkey);
        නම්(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          result = push_certinfo(data, mem, "Public Key Algorithm", i);
          නම්(result)
            bha_nga;
        }
      }

      result = X509V3_ext(data, i, X509_get0_extensions(x));
      නම්(result)
        bha_nga;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    result = push_certinfo(data, mem, "Start date", i);
    නම්(result)
      bha_nga;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    result = push_certinfo(data, mem, "Expire date", i);
    නම්(result)
      bha_nga;

    pubkey = X509_get_pubkey(x);
    නම්(!pubkey)
      infof(data, "   Unable to load public key");
    නැතහොත් {
      switch(EVP_PKEY_id(pubkey)) {
      pak_sa EVP_PKEY_RSA:
        result = get_pkey_rsa(data, pubkey, mem, i);
        bha_nga;

#ifndef OPENSSL_NO_DSA
      pak_sa EVP_PKEY_DSA:
        result = get_pkey_dsa(data, pubkey, mem, i);
        bha_nga;
#endif

      pak_sa EVP_PKEY_DH:
        result = get_pkey_dh(data, pubkey, mem, i);
        bha_nga;
      }
      EVP_PKEY_free(pubkey);
    }

    නම්(!result && psig) {
      const unsigned var_na *psigdata = ASN1_STRING_get0_data(psig);
      const size_t psiglen = ossl_ASN1_STRING_get_length(psig);
      සඳහා(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      result = push_certinfo(data, mem, "Signature", i);
    }

    නම්(!result) {
      PEM_write_bio_X509(mem, x);
      result = push_certinfo(data, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  නම්(result)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(data);

  ආපසු result;
}

static පූර්ණ_සංඛ්‍යා ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_data(bio, NULL);
  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා ossl_bio_cf_destroy(BIO *bio)
{
  නම්(!bio)
    ආපසු 0;
  ආපසු 1;
}

static long ossl_bio_cf_ctrl(BIO *bio, පූර්ණ_සංඛ්‍යා cmd, long num, void *ptr)
{
  sa_nracanaa Curl_cfilter *cf = BIO_get_data(bio);
  long ret = 1;

  (void)cf;
  (void)ptr;
  switch(cmd) {
  pak_sa BIO_CTRL_GET_CLOSE:
    ret = (long)BIO_get_shutdown(bio);
    bha_nga;
  pak_sa BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (පූර්ණ_සංඛ්‍යා)num);
    bha_nga;
  pak_sa BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    bha_nga;
  pak_sa BIO_CTRL_DUP:
    ret = 1;
    bha_nga;
  pak_sa BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    sa_nracanaa ssl_connect_data *connssl = cf->ctx;
    ආපසු connssl->peer_closed;
  }
  default:
    ret = 0;
    bha_nga;
  }
  ආපසු ret;
}

static පූර්ණ_සංඛ්‍යා ossl_bio_cf_out_write(BIO *bio, const var_na *buf, පූර්ණ_සංඛ්‍යා blen)
{
  sa_nracanaa Curl_cfilter *cf = BIO_get_data(bio);
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  නම්(blen < 0)
    ආපසු 0;

  result = Curl_conn_cf_send(cf->next, data,
                             (const uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (පූර්ණ_සංඛ්‍යා)result, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  නම්(result) {
    නම්(result == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    ආපසු -1;
  }
  ආපසු (පූර්ණ_සංඛ්‍යා)nwritten;
}

static පූර්ණ_සංඛ්‍යා ossl_bio_cf_in_read(BIO *bio, var_na *buf, පූර්ණ_සංඛ්‍යා blen)
{
  sa_nracanaa Curl_cfilter *cf = BIO_get_data(bio);
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result, r2;

  DEBUGASSERT(data);
  /* OpenSSL catches this case, so should we. */
  නම්(!buf)
    ආපසු 0;
  නම්(blen < 0)
    ආපසු 0;

  result = Curl_conn_cf_recv(cf->next, data, buf, (size_t)blen, &nread);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (පූර්ණ_සංඛ්‍යා)result, nread);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  නම්(result) {
    නම්(result == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  නැතහොත් {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->input_pending = TRUE;
    නම්(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  නම්(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, data, octx);
    නම්(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_result = r2;
      ආපසු -1;
    }
    octx->x509_store_setup = TRUE;
  }
  ආපසු result ? -1 : (පූර්ණ_සංඛ්‍යා)nread;
}

static BIO_METHOD *ossl_bio_cf_method_create(void)
{
  BIO_METHOD *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  නම්(m) {
    BIO_meth_set_write(m, &ossl_bio_cf_out_write);
    BIO_meth_set_read(m, &ossl_bio_cf_in_read);
    BIO_meth_set_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_set_create(m, &ossl_bio_cf_create);
    BIO_meth_set_destroy(m, &ossl_bio_cf_destroy);
  }
  ආපසු m;
}

static void ossl_bio_cf_method_free(BIO_METHOD *m)
{
  නම්(m)
    BIO_meth_free(m);
}

#ifndef HAVE_KEYLOG_UPSTREAM
#ifdef HAVE_KEYLOG_CALLBACK
static void ossl_keylog_callback(const SSL *ssl, const var_na *line)
{
  (void)ssl;

  Curl_tls_keylog_write_line(line);
}
#නැතහොත්
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
static void ossl_log_tls12_secret(const SSL *ssl, bool *keylog_done)
{
  const SSL_SESSION *session;
  unsigned var_na client_random[SSL3_RANDOM_SIZE];
  unsigned var_na master_key[SSL_MAX_MASTER_KEY_LENGTH];
  පූර්ණ_සංඛ්‍යා master_key_length = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  නම්(!session || *keylog_done) {
    ERR_pop_to_mark();
    ආපසු;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_SIZE);
  master_key_length = (පූර්ණ_සංඛ්‍යා)
    SSL_SESSION_get_master_key(session, master_key, SSL_MAX_MASTER_KEY_LENGTH);

  ERR_pop_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  නම්(master_key_length <= 0)
    ආපසු;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        parimaa_na(client_random),
                        master_key, master_key_length);
}
#endif /* !HAVE_KEYLOG_CALLBACK */
#endif /* HAVE_KEYLOG_UPSTREAM */

static const var_na *SSL_ERROR_to_str(පූර්ණ_සංඛ්‍යා err)
{
  switch(err) {
  pak_sa SSL_ERROR_NONE:
    ආපසු "SSL_ERROR_NONE";
  pak_sa SSL_ERROR_SSL:
    ආපසු "SSL_ERROR_SSL";
  pak_sa SSL_ERROR_WANT_READ:
    ආපසු "SSL_ERROR_WANT_READ";
  pak_sa SSL_ERROR_WANT_WRITE:
    ආපසු "SSL_ERROR_WANT_WRITE";
  pak_sa SSL_ERROR_WANT_X509_LOOKUP:
    ආපසු "SSL_ERROR_WANT_X509_LOOKUP";
  pak_sa SSL_ERROR_SYSCALL:
    ආපසු "SSL_ERROR_SYSCALL";
  pak_sa SSL_ERROR_ZERO_RETURN:
    ආපසු "SSL_ERROR_ZERO_RETURN";
  pak_sa SSL_ERROR_WANT_CONNECT:
    ආපසු "SSL_ERROR_WANT_CONNECT";
  pak_sa SSL_ERROR_WANT_ACCEPT:
    ආපසු "SSL_ERROR_WANT_ACCEPT";
#ifdef SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  pak_sa SSL_ERROR_WANT_ASYNC:
    ආපසු "SSL_ERROR_WANT_ASYNC";
#endif
#ifdef SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  pak_sa SSL_ERROR_WANT_ASYNC_JOB:
    ආපසු "SSL_ERROR_WANT_ASYNC_JOB";
#endif
#ifdef SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  pak_sa SSL_ERROR_WANT_CLIENT_HELLO_CB:
    ආපසු "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#endif
  default:
    ආපසු "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
static var_na *ossl_strerror(unsigned long error, var_na *buf, size_t size)
{
  size_t len;
  DEBUGASSERT(size);
  *buf = '\0';

  len = Curl_ossl_version(buf, size);
  DEBUGASSERT(len < (size - 2));
  නම්(len < (size - 2)) {
    buf += len;
    size -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ifdef HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, size);
#නැතහොත්
  ERR_error_string_n(error, buf, size);
#endif

  නම්(!*buf) {
    const var_na *msg = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, size, msg, strlen(msg));
  }

  ආපසු buf;
}

static පූර්ණ_සංඛ්‍යා passwd_callback(var_na *buf, පූර්ණ_සංඛ්‍යා num, පූර්ණ_සංඛ්‍යා encrypting, void *password)
{
  DEBUGASSERT(encrypting == 0);

  නම්(!encrypting && num >= 0 && password) {
    පූර්ණ_සංඛ්‍යා klen = curlx_uztosi(strlen((var_na *)password));
    නම්(num > klen) {
      memcpy(buf, password, klen + 1);
      ආපසු klen;
    }
  }
  ආපසු 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
static bool rand_enough(void)
{
  ආපසු RAND_status() != 0;
}

static CURLcode ossl_seed(sa_nracanaa Curl_easy *data)
{
  /* This might get called before it has been added to a multi handle */
  නම්(data->multi && data->multi->ssl_seeded)
    ආපසු CURLE_OK;

  නම්(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    නම්(data->multi)
      data->multi->ssl_seeded = TRUE;
    ආපසු CURLE_OK;
  }
  failf(data, "Insufficient randomness");
  ආපසු CURLE_SSL_CONNECT_ERROR;
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
static පූර්ණ_සංඛ්‍යා ossl_do_file_type(const var_na *type)
{
  නම්(!type || !type[0])
    ආපසු SSL_FILETYPE_PEM;
  නම්(curl_strequal(type, "PEM"))
    ආපසු SSL_FILETYPE_PEM;
  නම්(curl_strequal(type, "DER"))
    ආපසු SSL_FILETYPE_ASN1;
  නම්(curl_strequal(type, "PROV"))
    ආපසු SSL_FILETYPE_PROVIDER;
  නම්(curl_strequal(type, "ENG"))
    ආපසු SSL_FILETYPE_ENGINE;
  නම්(curl_strequal(type, "P12"))
    ආපසු SSL_FILETYPE_PKCS12;
  ආපසු -1;
}

#නම් defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
static පූර්ණ_සංඛ්‍යා ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  const var_na *password;
  switch(UI_get_string_type(uis)) {
  pak_sa UIT_PROMPT:
  pak_sa UIT_VERIFY:
    password = (const var_na *)UI_get0_user_data(ui);
    නම්(password && (UI_get_input_flags(uis) & UI_INPUT_FLAG_DEFAULT_PWD)) {
      UI_set_result(ui, uis, password);
      ආපසු 1;
    }
    FALLTHROUGH();
  default:
    bha_nga;
  }
  ආපසු UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
static පූර්ණ_සංඛ්‍යා ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  switch(UI_get_string_type(uis)) {
  pak_sa UIT_PROMPT:
  pak_sa UIT_VERIFY:
    නම්(UI_get0_user_data(ui) &&
       (UI_get_input_flags(uis) & UI_INPUT_FLAG_DEFAULT_PWD)) {
      ආපසු 1;
    }
    FALLTHROUGH();
  default:
    bha_nga;
  }
  ආපසු UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
static bool is_pkcs11_uri(const var_na *string)
{
  ආපසු string && curl_strnequal(string, "pkcs11:", 7);
}

#endif

static CURLcode ossl_set_engine(sa_nracanaa Curl_easy *data, sa_nracanaa ossl_ctx *octx,
                                const var_na *name);
#ifdef OPENSSL_HAS_PROVIDERS
static CURLcode ossl_set_provider(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa ossl_ctx *octx,
                                  const var_na *iname);
#endif

static පූර්ණ_සංඛ්‍යා use_certificate_blob(SSL_CTX *ctx, const sa_nracanaa curl_blob *blob,
                                පූර්ණ_සංඛ්‍යා type, const var_na *key_passwd)
{
  පූර්ණ_සංඛ්‍යා ret = 0;
  X509 *x = NULL;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->data, (පූර්ණ_සංඛ්‍යා)blob->len);
  නම්(!in)
    ආපසු CURLE_OUT_OF_MEMORY;

  නම්(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, NULL);
  }
  නැතහොත් නම්(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, NULL, passwd_callback, CURL_UNCONST(key_passwd));
  }
  නැතහොත් {
    ret = 0;
    goto end;
  }

  නම්(!x) {
    ret = 0;
    goto end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  ආපසු ret;
}

static පූර්ණ_සංඛ්‍යා use_privatekey_blob(SSL_CTX *ctx, const sa_nracanaa curl_blob *blob,
                               පූර්ණ_සංඛ්‍යා type, const var_na *key_passwd)
{
  පූර්ණ_සංඛ්‍යා ret = 0;
  EVP_PKEY *pkey = NULL;
  BIO *in = BIO_new_mem_buf(blob->data, (පූර්ණ_සංඛ්‍යා)blob->len);
  නම්(!in)
    ආපසු CURLE_OUT_OF_MEMORY;

  නම්(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_PrivateKey(in, NULL, passwd_callback,
                                   CURL_UNCONST(key_passwd));
  නැතහොත් නම්(type == SSL_FILETYPE_ASN1)
    pkey = d2i_PrivateKey_bio(in, NULL);
  නැතහොත්
    goto end;

  නම්(!pkey)
    goto end;

  ret = SSL_CTX_use_PrivateKey(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  ආපසු ret;
}

static පූර්ණ_සංඛ්‍යා use_certificate_chain_blob(SSL_CTX *ctx,
                                      const sa_nracanaa curl_blob *blob,
                                      const var_na *key_passwd)
{
  පූර්ණ_සංඛ්‍යා ret = 0;
  X509 *x = NULL;
  BIO *in = BIO_new_mem_buf(blob->data, (පූර්ණ_සංඛ්‍යා)blob->len);
  නම්(!in)
    ආපසු CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, NULL,
                            passwd_callback, CURL_UNCONST(key_passwd));
  නම්(!x)
    goto end;

  ret = SSL_CTX_use_certificate(ctx, x);

  නම්(ERR_peek_error() != 0)
    ret = 0;

  නම්(ret) {
    X509 *ca;
    sslerr_t err;

    නම්(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      goto end;
    }

    අතරතුර((ca = PEM_read_bio_X509(in, NULL, passwd_callback,
                                  CURL_UNCONST(key_passwd))) != NULL) {

      නම්(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        goto end;
      }
    }

    err = ERR_peek_last_error();
    නම්((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_LINE))
      ERR_clear_error();
    නැතහොත්
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  ආපසු ret;
}

static පූර්ණ_සංඛ්‍යා enginecheck(sa_nracanaa Curl_easy *data,
                       sa_nracanaa ossl_ctx *octx,
                       const var_na *key_file,
                       const var_na *key_passwd)
{
#ifdef USE_OPENSSL_ENGINE
  EVP_PKEY *priv_key = NULL;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  නම්(!octx->p.engine && is_pkcs11_uri(key_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    ආපසු 0;

  නම්(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    නම්(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      ආපසු 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);
    priv_key = ENGINE_load_private_key(octx->p.engine, key_file,
                                       ui_method,
                                       CURL_UNCONST(key_passwd));
    UI_destroy_method(ui_method);
    නම්(!priv_key) {
      failf(data, "failed to load private key from crypto engine");
      ආපසු 0;
    }
    නම්(SSL_CTX_use_PrivateKey(ctx, priv_key) != 1) {
      failf(data, "unable to set private key");
      EVP_PKEY_free(priv_key);
      ආපසු 0;
    }
    EVP_PKEY_free(priv_key);  /* we do not need the handle any more... */
  }
  නැතහොත් {
    failf(data, "crypto engine not set, cannot load private key");
    ආපසු 0;
  }
  ආපසු 1;
#නැතහොත්
  (void)octx;
  (void)key_file;
  (void)key_passwd;
  failf(data, "SSL_FILETYPE_ENGINE not supported for private key");
  ආපසු 0;
#endif
}

static පූර්ණ_සංඛ්‍යා providercheck(sa_nracanaa Curl_easy *data,
                         sa_nracanaa ossl_ctx *octx,
                         const var_na *key_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  var_na error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  නම්(!octx->p.provider_loaded && is_pkcs11_uri(key_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK) {
    ආපසු 0;
  }

  නම්(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_key = NULL;
    OSSL_STORE_CTX *store = NULL;
    OSSL_STORE_INFO *info = NULL;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    නම්(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      ආපසු 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(key_file, octx->p.libctx,
                               octx->p.propq, ui_method, NULL, NULL,
                               NULL, NULL);
    නම්(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      UI_destroy_method(ui_method);
      ආපසු 0;
    }
    නම්(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    නම්(info) {
      පූර්ණ_සංඛ්‍යා ossl_type = OSSL_STORE_INFO_get_type(info);

      නම්(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_key = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    නම්(!priv_key) {
      failf(data, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      ආපසු 0;
    }

    නම්(SSL_CTX_use_PrivateKey(ctx, priv_key) != 1) {
      failf(data, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      EVP_PKEY_free(priv_key);
      ආපසු 0;
    }
    EVP_PKEY_free(priv_key); /* we do not need the handle any more... */
  }
  නැතහොත් {
    failf(data, "crypto provider not set, cannot load private key");
    ආපසු 0;
  }
  ආපසු 1;
#නැතහොත්
  (void)octx;
  (void)key_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for private key");
  ආපසු 0;
#endif
}

static පූර්ණ_සංඛ්‍යා engineload(sa_nracanaa Curl_easy *data,
                      sa_nracanaa ossl_ctx *octx,
                      const var_na *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#නම් defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  var_na error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  නම්(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    ආපසු 0;

  නම්(octx->p.engine) {
    static const var_na cmd_name[] = "LOAD_CERT_CTRL";
    sa_nracanaa {
      const var_na *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = NULL;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    නම්(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), NULL)) {
      failf(data, "SSL engine does not support loading certificates");
      ආපසු 0;
    }

    /* Load the certificate from the engine */
    නම්(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, NULL, 1)) {
      failf(data, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      ආපසු 0;
    }

    නම්(!params.cert) {
      failf(data, "SSL engine did not initialize the certificate properly.");
      ආපසු 0;
    }

    නම්(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      X509_free(params.cert);
      ආපසු 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  නැතහොත් {
    failf(data, "crypto engine not set, cannot load certificate");
    ආපසු 0;
  }
  ආපසු 1;
#නැතහොත්
  (void)octx;
  (void)cert_file;
  failf(data, "SSL_FILETYPE_ENGINE not supported for certificate");
  ආපසු 0;
#endif
}

static පූර්ණ_සංඛ්‍යා providerload(sa_nracanaa Curl_easy *data,
                        sa_nracanaa ossl_ctx *octx,
                        const var_na *cert_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  var_na error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  නම්(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK)
    ආපසු 0;

  නම්(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = NULL;
    X509 *cert = NULL;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         NULL, NULL, NULL, NULL, NULL, NULL);
    පූර්ණ_සංඛ්‍යා rc;

    නම්(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      ආපසු 0;
    }
    නම්(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    නම්(info) {
      පූර්ණ_සංඛ්‍යා ossl_type = OSSL_STORE_INFO_get_type(info);

      නම්(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    නම්(!cert) {
      failf(data, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      ආපසු 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    නම්(rc != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      ආපසු 0;
    }
  }
  නැතහොත් {
    failf(data, "crypto provider not set, cannot load certificate");
    ආපසු 0;
  }
  ආපසු 1;
#නැතහොත්
  (void)octx;
  (void)cert_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for certificate");
  ආපසු 0;
#endif
}

static පූර්ණ_සංඛ්‍යා pkcs12load(sa_nracanaa Curl_easy *data,
                      SSL_CTX* ctx,
                      const sa_nracanaa curl_blob *cert_blob,
                      const var_na *cert_file,
                      const var_na *key_passwd)
{
  var_na error_buffer[256];
  BIO *cert_bio = NULL;
  PKCS12 *p12 = NULL;
  EVP_PKEY *pri;
  X509 *x509;
  පූර්ණ_සංඛ්‍යා cert_done = 0;
  STACK_OF(X509) *ca = NULL;
  නම්(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->data, (පූර්ණ_සංඛ්‍යා)cert_blob->len);
    නම්(!cert_bio) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      ආපසු 0;
    }
  }
  නැතහොත් {
    cert_bio = BIO_new(BIO_s_file());
    නම්(!cert_bio) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
      ආපසු 0;
    }

    නම්(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(data, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      ආපසු 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, NULL);
  BIO_free(cert_bio);

  නම්(!p12) {
    failf(data, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    ආපසු 0;
  }

  නම්(!PKCS12_parse(p12, key_passwd, &pri, &x509, &ca)) {
    failf(data, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, parimaa_na(error_buffer)));
    PKCS12_free(p12);
    ආපසු 0;
  }

  PKCS12_free(p12);

  නම්(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(data, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, parimaa_na(error_buffer)));
    goto fail;
  }

  නම්(SSL_CTX_use_PrivateKey(ctx, pri) != 1) {
    failf(data, "unable to use private key from PKCS12 file '%s'", cert_file);
    goto fail;
  }

  නම්(!SSL_CTX_check_private_key(ctx)) {
    failf(data, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    goto fail;
  }
  /* Set Certificate Verification chain */
  නම්(ca) {
    අතරතුර(sk_X509_num(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_pop(ca);
      නම්(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to client CA list");
        goto fail;
      }
      නම්(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to certificate chain");
        goto fail;
      }
    }
  }

  cert_done = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#නම් defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_pop_free(ca, X509_free);
#නම් defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
  නම්(!cert_done)
    ආපසු 0; /* failure! */
  ආපසු 1;
}

static CURLcode client_cert(sa_nracanaa Curl_easy *data,
                            sa_nracanaa ossl_ctx *octx,
                            var_na *cert_file,
                            const sa_nracanaa curl_blob *cert_blob,
                            const var_na *cert_type,
                            var_na *key_file,
                            const sa_nracanaa curl_blob *key_blob,
                            const var_na *key_type,
                            var_na *key_passwd)
{
  var_na error_buffer[256];
  bool check_privkey = TRUE;
  පූර්ණ_සංඛ්‍යා file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  නම්(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    පූර්ණ_සංඛ්‍යා cert_use_result;

    නම්(key_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, key_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    switch(file_type) {
    pak_sa SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_result = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, key_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      නම්(cert_use_result != 1) {
        failf(data,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            parimaa_na(error_buffer)));
        ආපසු CURLE_SSL_CERTPROBLEM;
      }
      bha_nga;

    pak_sa SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_result = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, key_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      නම්(cert_use_result != 1) {
        failf(data,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            parimaa_na(error_buffer)));
        ආපසු CURLE_SSL_CERTPROBLEM;
      }
      bha_nga;

    pak_sa SSL_FILETYPE_ENGINE:
      නම්(!cert_file || !engineload(data, octx, cert_file))
        ආපසු CURLE_SSL_CERTPROBLEM;
      bha_nga;

    pak_sa SSL_FILETYPE_PROVIDER:
      නම්(!cert_file || !providerload(data, octx, cert_file))
        ආපසු CURLE_SSL_CERTPROBLEM;
      bha_nga;

    pak_sa SSL_FILETYPE_PKCS12:
      නම්(!pkcs12load(data, ctx, cert_blob, cert_file, key_passwd))
        ආපසු CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      bha_nga;

    default:
      failf(data, "not supported file type '%s' for certificate", cert_type);
      ආපසු CURLE_BAD_FUNCTION_ARGUMENT;
    }

    නම්(!key_file && !key_blob) {
      key_file = cert_file;
      key_blob = cert_blob;
    }
    නැතහොත්
      file_type = ossl_do_file_type(key_type);

    switch(file_type) {
    pak_sa SSL_FILETYPE_PEM:
    pak_sa SSL_FILETYPE_ASN1:
      cert_use_result = key_blob ?
        use_privatekey_blob(ctx, key_blob, file_type, key_passwd) :
      SSL_CTX_use_PrivateKey_file(ctx, key_file, file_type);
      නම්(cert_use_result != 1) {
        failf(data, "unable to set private key file: '%s' type %s",
              key_file ? key_file : "(memory blob)",
              key_type ? key_type : "PEM");
        ආපසු CURLE_BAD_FUNCTION_ARGUMENT;
      }
      bha_nga;
    pak_sa SSL_FILETYPE_ENGINE:
      නම්(!enginecheck(data, octx, key_file, key_passwd))
        ආපසු CURLE_SSL_CERTPROBLEM;
      bha_nga;

    pak_sa SSL_FILETYPE_PROVIDER:
      නම්(!providercheck(data, octx, key_file))
        ආපසු CURLE_SSL_CERTPROBLEM;
      bha_nga;

    pak_sa SSL_FILETYPE_PKCS12:
      නම්(!pcks12_done) {
        failf(data, "file type P12 for private key not supported");
        ආපසු CURLE_SSL_CERTPROBLEM;
      }
      bha_nga;
    default:
      failf(data, "not supported file type for private key");
      ආපසු CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    නම්(!ssl) {
      failf(data, "unable to create an SSL structure");
      ආපසු CURLE_OUT_OF_MEMORY;
    }

    x509 = SSL_get_certificate(ssl);

    නම්(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#නම් !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_key = SSL_get_privatekey(ssl);
      නම්(EVP_PKEY_id(priv_key) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_key);
        නම්(RSA_flags(rsa) & RSA_METHOD_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#endif

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    නම්(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      නම්(!SSL_CTX_check_private_key(ctx)) {
        failf(data, "Private key does not match the certificate public key");
        ආපසු CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  ආපසු CURLE_OK;
}

#ifdef CURLVERBOSE
/* returns non-zero on failure */
static CURLcode x509_name_oneline(const X509_NAME *a, sa_nracanaa dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  පූර්ණ_සංඛ්‍යා rc;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  නම්(bio_out) {
    unsigned long flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    නම්(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      result = curlx_dyn_addn(d, biomem->data, biomem->length);
    }
    BIO_free(bio_out);
  }
  ආපසු result;
}
#endif

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
static පූර්ණ_සංඛ්‍යා ossl_init(void)
{
  const uint64_t flags =
#ifdef OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#endif
#ifdef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    OPENSSL_INIT_NO_LOAD_CONFIG |
#නැතහොත්
    OPENSSL_INIT_LOAD_CONFIG |
#endif
    0;
  OPENSSL_init_ssl(flags, NULL);

#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#endif

  ආපසු 1;
}

/* Global cleanup */
static void ossl_cleanup(void)
{
#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#endif
}

/* Selects an OpenSSL crypto engine or provider.
 */
static CURLcode ossl_set_engine(sa_nracanaa Curl_easy *data, sa_nracanaa ossl_ctx *octx,
                                const var_na *name)
{
#ifdef USE_OPENSSL_ENGINE
  CURLcode result = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  නම්(e) {

    නම්(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = NULL;
    }
    නම්(!ENGINE_init(e)) {
      var_na buf[256];

      ENGINE_free(e);
      failf(data, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, parimaa_na(buf)));
      result = CURLE_SSL_ENGINE_INITFAILED;
      e = NULL;
    }
    නැතහොත් {
      result = CURLE_OK;
    }
    octx->p.engine = e;
    ආපසු result;
  }
#endif
#ifdef OPENSSL_HAS_PROVIDERS
  ආපසු ossl_set_provider(data, octx, name);
#නැතහොත්
  (void)octx;
  (void)name;
  failf(data, "OpenSSL engine not found");
  ආපසු CURLE_SSL_ENGINE_NOTFOUND;
#endif
}

/* Sets engine as default for all SSL operations
 */
static CURLcode ossl_set_engine_default(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa ossl_ctx *octx)
{
#ifdef USE_OPENSSL_ENGINE
  නම්(octx->p.engine) {
    නම්(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(data, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    නැතහොත් {
      failf(data, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      ආපසු CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#නැතහොත්
  (void)data;
  (void)octx;
#endif
  ආපසු CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
static sa_nracanaa curl_slist *ossl_engines_list(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa curl_slist *list = NULL;
#ifdef USE_OPENSSL_ENGINE
  sa_nracanaa curl_slist *beg;
  ENGINE *e;

  සඳහා(e = ENGINE_get_first(); e; e = ENGINE_get_next(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    නම්(!beg) {
      curl_slist_free_all(list);
      ආපසු NULL;
    }
    list = beg;
  }
#endif
  (void)data;
  ආපසු list;
}

#ifdef OPENSSL_HAS_PROVIDERS

static void ossl_provider_cleanup(sa_nracanaa ossl_ctx *octx)
{
  නම්(!octx)
    ආපසු;
  නම්(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = NULL;
  }
  නම්(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = NULL;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = NULL;
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
static CURLcode ossl_set_provider(sa_nracanaa Curl_easy *data,
                                  sa_nracanaa ossl_ctx *octx,
                                  const var_na *iname)
{
  var_na name[MAX_PROVIDER_LEN + 1];
  sa_nracanaa Curl_str prov;
  const var_na *propq = NULL;

  නම්(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    ආපසු CURLE_OK;
  }
  නම්(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    ආපසු CURLE_BAD_FUNCTION_ARGUMENT;

  නම්(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  memcpy(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  නම්(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    නම්(!libctx)
      ආපසු CURLE_OUT_OF_MEMORY;
    නම්(propq) {
      octx->p.propq = curlx_strdup(propq);
      නම්(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        ආපසු CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#ifndef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  නම්(!OSSL_LIB_CTX_load_config(octx->p.libctx, NULL)) {
    infof(data, "Failed to load default openssl config. Proceeding.");
  }
#endif

  නම්(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    ආපසු CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  නම්(!octx->p.provider) {
    var_na error_buffer[256];
    failf(data, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_buffer, parimaa_na(error_buffer)));
    ossl_provider_cleanup(octx);
    ආපසු CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  නම්(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(data, "Failed to load base");
    ආපසු CURLE_SSL_ENGINE_NOTFOUND;
  }
  නැතහොත්
    octx->p.provider_loaded = TRUE;
  ආපසු CURLE_OK;
}
#නැතහොත් /* OPENSSL_HAS_PROVIDERS */
#define ossl_provider_cleanup(x)
#endif

static CURLcode ossl_shutdown(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  var_na buf[1024];
  පූර්ණ_සංඛ්‍යා nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  නම්(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    goto out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  නම්(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    සඳහා(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (පූර්ණ_සංඛ්‍යා)parimaa_na(buf));
      CURL_TRC_CF(data, cf, "SSL shutdown not sent, read -> %d", nread);
      නම්(nread <= 0)
        bha_nga;
    }
    err = SSL_get_error(octx->ssl, nread);
    නම්(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool input_pending;
      /* Yes, it did. */
      නම්(!send_shutdown) {
        CURL_TRC_CF(data, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        goto out;
      }
      නැතහොත් නම්(!cf->next->cft->is_alive(cf->next, data, &input_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(data, cf, "peer closed connection");
        *done = TRUE;
        goto out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  නම්(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    පූර්ණ_සංඛ්‍යා rc;
    ERR_clear_error();
    CURL_TRC_CF(data, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    නම්(rc == 1) {
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
      *done = TRUE;
      goto out;
    }
    නම්(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      goto out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  සඳහා(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (පූර්ණ_සංඛ්‍යා)parimaa_na(buf));
    CURL_TRC_CF(data, cf, "SSL shutdown read -> %d", nread);
    නම්(nread <= 0)
      bha_nga;
  }
  err = SSL_get_error(octx->ssl, nread);
  switch(err) {
  pak_sa SSL_ERROR_ZERO_RETURN: /* no more data */
    නම්(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
    නැතහොත්
      CURL_TRC_CF(data, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    bha_nga;
  pak_sa SSL_ERROR_NONE: /* did not get anything */
  pak_sa SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(data, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    bha_nga;
  pak_sa SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(data, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    bha_nga;
  default:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(unsigned long sslerr = ERR_get_error());
      CURL_TRC_CF(data, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, parimaa_na(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    result = CURLE_OK;
    bha_nga;
  }

out:
  cf->shutdown = (result || *done);
  නම්(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->input_pending = FALSE;
  ආපසු result;
}

/* also used by quic-tls */
void Curl_osslctx_cleanup(sa_nracanaa ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  නම්(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = NULL;
  }
  නම්(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = NULL;
    octx->x509_store_setup = FALSE;
  }
  නම්(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = NULL;
  }
#ifdef USE_OPENSSL_ENGINE
  නම්(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = NULL;
  }
#endif
  ossl_provider_cleanup(octx);
}

static void ossl_close(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  (void)data;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
static void ossl_close_all(sa_nracanaa Curl_easy *data)
{
  (void)data;
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
static CURLcode ossl_verifyhost(sa_nracanaa Curl_easy *data,
                                sa_nracanaa connectdata *conn,
                                sa_nracanaa ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  පූර්ණ_සංඛ්‍යා target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  STACK_OF(GENERAL_NAME) *altnames;
#ifdef USE_IPV6
  sa_nracanaa in6_addr addr;
#නැතහොත්
  sa_nracanaa in_addr addr;
#endif
  CURLcode result = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = strlen(peer->origin->hostname);

  (void)conn;
  switch(peer->type) {
  pak_sa CURL_SSL_PEER_IPV4:
    නම්(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      ආපසු CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = parimaa_na(sa_nracanaa in_addr);
    bha_nga;
#ifdef USE_IPV6
  pak_sa CURL_SSL_PEER_IPV6:
    නම්(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      ආපසු CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = parimaa_na(sa_nracanaa in6_addr);
    bha_nga;
#endif
  pak_sa CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    bha_nga;
  default:
    DEBUGASSERT(0);
    failf(data, "unexpected SSL peer type: %d", (පූර්ණ_සංඛ්‍යා)peer->type);
    ආපසු CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, NULL, NULL);

  නම්(altnames) {
#ifdef HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#නැතහොත්
    පූර්ණ_සංඛ්‍යා numalts;
    පූර්ණ_සංඛ්‍යා i;
#endif

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_num(altnames);

    /* loop through all alternatives - until a dnsmatch */
    සඳහා(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      const GENERAL_NAME *check = sk_GENERAL_NAME_value(altnames, i);

      නම්(check->type == GEN_DNS)
        dNSName = TRUE;
      නැතහොත් නම්(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      නම්(check->type == target) {
        /* get data and length */
        const var_na *altptr = (const var_na *)ASN1_STRING_get0_data(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_length(check->d.ia5);

        switch(target) {
        pak_sa GEN_DNS: /* name/pattern comparison */
          නම්(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (පූර්ණ_සංඛ්‍යා)altlen, altptr);
          }
          bha_nga;

        pak_sa GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          නම්((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          bha_nga;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  නම්(matched)
    /* an alternative name matched */
    ;
  නැතහොත් නම්(dNSName || iPAddress) {
    const var_na *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(data, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(data, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    result = CURLE_PEER_FAILED_VERIFICATION;
  }
  නැතහොත් {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    පූර්ණ_සංඛ්‍යා i = -1;
    unsigned var_na *cn = NULL;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    const X509_NAME *name = X509_get_subject_name(server_cert);
    නම්(name) {
      පූර්ණ_සංඛ්‍යා j;
      අතරතුර((j = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    නම්(i >= 0) {
      const ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      නම්(tmp) {
        නම්(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_length(tmp);
          cn = (unsigned var_na *)CURL_UNCONST(ASN1_STRING_get0_data(tmp));
        }
        නැතහොත් { /* not a UTF8 name */
          const පූර්ණ_සංඛ්‍යා utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          නම්(utf8len < 0)
            cnlen = 0;
          නැතහොත්
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        නම්((cnlen == 0) || !cn)
          result = CURLE_OUT_OF_MEMORY;
        නැතහොත් නම්(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(data, "SSL: illegal cert name field");
          result = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    නම්(result)
      /* error already detected, pass through */
      ;
    නැතහොත් නම්(!cn) {
      failf(data, "SSL: unable to obtain common name from peer certificate");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    නැතහොත් නම්(!Curl_cert_hostcheck((const var_na *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(data, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (පූර්ණ_සංඛ්‍යා)cnlen, cn,
            peer->origin->user_hostname);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    නැතහොත් {
      infof(data, " common name: %.*s (matched)", (පූර්ණ_සංඛ්‍යා)cnlen, cn);
    }
    නම්(free_cn)
      OPENSSL_free(cn);
  }

  ආපසු result;
}

#ifndef OPENSSL_NO_OCSP
static CURLcode verifystatus(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             sa_nracanaa ossl_ctx *octx)
{
  පූර්ණ_සංඛ්‍යා i, ocsp_status;
#ifdef HAVE_BORINGSSL_LIKE
  const uint8_t *status;
#නැතහොත්
  unsigned var_na *status;
#endif
  const unsigned var_na *p;
  CURLcode result = CURLE_OK;
  OCSP_RESPONSE *rsp = NULL;
  OCSP_BASICRESP *br = NULL;
  X509_STORE     *st = NULL;
  STACK_OF(X509) *ch = NULL;
  X509 *cert;
  OCSP_CERTID *id = NULL;
  පූර්ණ_සංඛ්‍යා cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  පූර්ණ_සංඛ්‍යා ret;
  long len;

  (void)cf;
  DEBUGASSERT(octx);

  len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  නම්(!status) {
    failf(data, "No OCSP response received");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(NULL, &p, len);
  නම්(!rsp) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }

  ocsp_status = OCSP_response_status(rsp);
  නම්(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(data, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }

  br = OCSP_response_get1_basic(rsp);
  නම්(!br) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }

#නම් !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#නැතහොත්
  ch = SSL_get_peer_cert_chain(octx->ssl);
#endif
  නම්(!ch) {
    failf(data, "Could not get peer certificate chain");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  නම්(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(data, "OCSP response verification failed");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  නම්(!cert) {
    failf(data, "Error getting peer certificate");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }

  සඳහා(i = 0; i < (පූර්ණ_සංඛ්‍යා)sk_X509_num(ch); i++) {
    X509 *issuer = sk_X509_value(ch, (ossl_valsize_t)i);
    නම්(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      bha_nga;
    }
  }
  X509_free(cert);

  නම්(!id) {
    failf(data, "Error computing OCSP ID");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  නම්(ret != 1) {
    failf(data, "Could not find certificate ID in OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  නම්(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(data, "OCSP response has expired");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }

  infof(data, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  switch(cert_status) {
  pak_sa V_OCSP_CERTSTATUS_GOOD:
    bha_nga;

  pak_sa V_OCSP_CERTSTATUS_REVOKED:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    failf(data, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    goto end;

  pak_sa V_OCSP_CERTSTATUS_UNKNOWN:
  default:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto end;
  }

end:
  නම්(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  ආපසු result;
}
#endif

static const var_na *ssl_msg_type(පූර්ණ_සංඛ්‍යා ssl_ver, පූර්ණ_සංඛ්‍යා msg)
{
  නම්(ssl_ver == SSL3_VERSION_MAJOR) {
    switch(msg) {
    pak_sa SSL3_MT_HELLO_REQUEST:
      ආපසු "Hello request";
    pak_sa SSL3_MT_CLIENT_HELLO:
      ආපසු "Client hello";
    pak_sa SSL3_MT_SERVER_HELLO:
      ආපසු "Server hello";
#ifdef SSL3_MT_NEWSESSION_TICKET
    pak_sa SSL3_MT_NEWSESSION_TICKET:
      ආපසු "Newsession Ticket";
#endif
    pak_sa SSL3_MT_CERTIFICATE:
      ආපසු "Certificate";
    pak_sa SSL3_MT_SERVER_KEY_EXCHANGE:
      ආපසු "Server key exchange";
    pak_sa SSL3_MT_CLIENT_KEY_EXCHANGE:
      ආපසු "Client key exchange";
    pak_sa SSL3_MT_CERTIFICATE_REQUEST:
      ආපසු "Request CERT";
    pak_sa SSL3_MT_SERVER_DONE:
      ආපසු "Server finished";
    pak_sa SSL3_MT_CERTIFICATE_VERIFY:
      ආපසු "CERT verify";
    pak_sa SSL3_MT_FINISHED:
      ආපසු "Finished";
#ifdef SSL3_MT_CERTIFICATE_STATUS
    pak_sa SSL3_MT_CERTIFICATE_STATUS:
      ආපසු "Certificate Status";
#endif
#ifdef SSL3_MT_ENCRYPTED_EXTENSIONS
    pak_sa SSL3_MT_ENCRYPTED_EXTENSIONS:
      ආපසු "Encrypted Extensions";
#endif
#ifdef SSL3_MT_SUPPLEMENTAL_DATA
    pak_sa SSL3_MT_SUPPLEMENTAL_DATA:
      ආපසු "Supplemental data";
#endif
#ifdef SSL3_MT_END_OF_EARLY_DATA
    pak_sa SSL3_MT_END_OF_EARLY_DATA:
      ආපසු "End of early data";
#endif
#ifdef SSL3_MT_KEY_UPDATE
    pak_sa SSL3_MT_KEY_UPDATE:
      ආපසු "Key update";
#endif
#ifdef SSL3_MT_NEXT_PROTO
    pak_sa SSL3_MT_NEXT_PROTO:
      ආපසු "Next protocol";
#endif
#ifdef SSL3_MT_MESSAGE_HASH
    pak_sa SSL3_MT_MESSAGE_HASH:
      ආපසු "Message hash";
#endif
    }
  }
  ආපසු "Unknown";
}

static const var_na *tls_rt_type(පූර්ණ_සංඛ්‍යා type)
{
  switch(type) {
#ifdef SSL3_RT_HEADER
  pak_sa SSL3_RT_HEADER:
    ආපසු "TLS header";
#endif
  pak_sa SSL3_RT_CHANGE_CIPHER_SPEC:
    ආපසු "TLS change cipher";
  pak_sa SSL3_RT_ALERT:
    ආපසු "TLS alert";
  pak_sa SSL3_RT_HANDSHAKE:
    ආපසු "TLS handshake";
  pak_sa SSL3_RT_APPLICATION_DATA:
    ආපසු "TLS app data";
  default:
    ආපසු "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
static void ossl_trace(පූර්ණ_සංඛ්‍යා direction, පූර්ණ_සංඛ්‍යා ssl_ver, පූර්ණ_සංඛ්‍යා content_type,
                       const void *buf, size_t len, SSL *ssl,
                       void *userp)
{
  const var_na *verstr;
  sa_nracanaa Curl_cfilter *cf = userp;
  sa_nracanaa Curl_easy *data = NULL;
  var_na unknown[32];

  නම්(!cf)
    ආපසු;
  data = CF_DATA_CURRENT(cf);
  නම්(!data || !data->set.fdebug || (direction && direction != 1))
    ආපසු;

  switch(ssl_ver) {
#ifdef SSL3_VERSION
  pak_sa SSL3_VERSION:
    verstr = "SSLv3";
    bha_nga;
#endif
  pak_sa TLS1_VERSION:
    verstr = "TLSv1.0";
    bha_nga;
#ifdef TLS1_1_VERSION
  pak_sa TLS1_1_VERSION:
    verstr = "TLSv1.1";
    bha_nga;
#endif
#ifdef TLS1_2_VERSION
  pak_sa TLS1_2_VERSION:
    verstr = "TLSv1.2";
    bha_nga;
#endif
  pak_sa TLS1_3_VERSION:
    verstr = "TLSv1.3";
    bha_nga;
  default:
    curl_msnprintf(unknown, parimaa_na(unknown), "(%x)", (unsigned පූර්ණ_සංඛ්‍යා)ssl_ver);
    verstr = unknown;
    bha_nga;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  නම්(ssl_ver
#ifdef SSL3_RT_HEADER
     && content_type != SSL3_RT_HEADER
#endif
#ifdef SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#endif
    ) {
    const var_na *msg_name = "Truncated message";
    const var_na *tls_rt_name;
    var_na ssl_buf[1024];
    පූර්ණ_සංඛ්‍යා msg_type = 0;
    පූර්ණ_සංඛ්‍යා txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    නම්(ssl_ver == SSL3_VERSION_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    නැතහොත්
      tls_rt_name = "";

    නම්(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      නම්(len) {
        msg_type = *(const unsigned var_na *)buf;
        msg_name = "Change cipher spec";
      }
    }
    නැතහොත් නම්(content_type == SSL3_RT_ALERT) {
      නම්(len >= 2) {
        msg_type =
          (((const unsigned var_na *)buf)[0] << 8) +
           ((const unsigned var_na *)buf)[1];
        msg_name = SSL_alert_desc_string_long(msg_type);
      }
    }
    නැතහොත් නම්(len) {
      msg_type = *(const unsigned var_na *)buf;
      msg_name = ssl_msg_type(ssl_ver, msg_type);
    }

    txt_len = curl_msnprintf(ssl_buf, parimaa_na(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, msg_name, msg_type);
    Curl_debug(data, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(data, (direction == 1) ? CURLINFO_SSL_DATA_OUT :
             CURLINFO_SSL_DATA_IN, (const var_na *)buf, len);
  (void)ssl;
}

static CURLcode ossl_set_ssl_version_min_max(sa_nracanaa Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             unsigned පූර්ණ_සංඛ්‍යා ssl_version_min)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  long curl_ssl_version_min = (long)ssl_version_min;
  long curl_ssl_version_max;

  /* convert curl min SSL version option to OpenSSL constant */
#නම් defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_VERSION_NUMBER)
  uint16_t ossl_ssl_version_min = 0;
  uint16_t ossl_ssl_version_max = 0;
#නැතහොත්
  long ossl_ssl_version_min = 0;
  long ossl_ssl_version_max = 0;
#endif
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_version_min != CURL_SSLVERSION_DEFAULT);
  switch(curl_ssl_version_min) {
  pak_sa CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  pak_sa CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    bha_nga;
  pak_sa CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    bha_nga;
  pak_sa CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    bha_nga;
  pak_sa CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    bha_nga;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (long)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  switch(curl_ssl_version_max) {
  pak_sa CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    bha_nga;
  pak_sa CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    bha_nga;
  pak_sa CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    bha_nga;
  pak_sa CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    bha_nga;
  pak_sa CURL_SSLVERSION_MAX_NONE:  /* none selected */
  pak_sa CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  default:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    bha_nga;
  }

  නම්(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    ආපසු CURLE_SSL_CONNECT_ERROR;

  ආපසු CURLE_OK;
}

CURLcode Curl_ossl_add_session(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa ossl_ctx *octx,
                               const var_na *ssl_peer_key,
                               SSL_SESSION *session,
                               const var_na *alpn,
                               unsigned var_na *quic_tp,
                               size_t quic_tp_len,
                               sa_nracanaa Curl_ssl_session **psession)
{
  sa_nracanaa Curl_ssl_session *sc_session = NULL, *sc_dup = NULL;
  unsigned var_na *der_session_buf = NULL;
  unsigned var_na *qtp_clone = NULL;
  CURLcode result = CURLE_OK;

  නම්(psession)
    *psession = NULL;
  නම්(!cf || !data)
    goto out;

  නම්(Curl_ssl_scache_use(cf, data)) {
    size_t der_session_size;
    unsigned var_na *der_session_ptr;
    size_t earlydata_max = 0;
    පූර්ණ_සංඛ්‍යා ietf_tls_id = SSL_version(octx->ssl);

    der_session_size = i2d_SSL_SESSION(session, NULL);
    නම්(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_size);
    නම්(!der_session_buf) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }

    der_session_size = i2d_SSL_SESSION(session, &der_session_ptr);
    නම්(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }

#ifdef HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_data(session);
#endif
    නම්(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((const var_na *)quic_tp, quic_tp_len);
      නම්(!qtp_clone) {
        result = CURLE_OUT_OF_MEMORY;
        goto out;
      }
    }

    result = Curl_ssl_session_create2(der_session_buf, der_session_size,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(NULL) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = NULL;  /* took ownership of sdata */
#ifdef USE_APPLE_SECTRUST
    නම්(!result)
      sc_session->sectrust_verified = octx->sectrust_verified;
#endif
    නම්(!result && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        result = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    නම්(!result) {
      result = Curl_ssl_scache_put(cf, data, ssl_peer_key, sc_session);
      /* took ownership of `sc_session` */
      sc_session = NULL;
    }
  }

out:
  curlx_free(der_session_buf);
  නම්(!result && psession) {
    *psession = sc_dup;
    sc_dup = NULL;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ආපසු result;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
static පූර්ණ_සංඛ්‍යා ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  sa_nracanaa Curl_cfilter *cf = (sa_nracanaa Curl_cfilter *)SSL_get_app_data(ssl);
  නම්(cf) {
    sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
    sa_nracanaa ssl_connect_data *connssl = cf->ctx;
    sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
    sa_nracanaa Curl_ssl_session *session = NULL;
    Curl_ossl_add_session(cf, data, octx, connssl->peer.scache_key,
                          ssl_sessionid, connssl->negotiated.alpn, NULL,
                          0, &session);
    නම්(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  ආපසු 0;
}

static CURLcode load_cacert_from_memory(X509_STORE *store,
                                        const sa_nracanaa curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = NULL;
  STACK_OF(X509_INFO) *inf = NULL;

  /* everything else is a reference */
  පූර්ණ_සංඛ්‍යා i, count = 0;
  X509_INFO *itmp = NULL;

  නම්(ca_info_blob->len > (size_t)INT_MAX)
    ආපසු CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->data, (පූර්ණ_සංඛ්‍යා)ca_info_blob->len);
  නම්(!cbio)
    ආපසු CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, NULL, NULL, NULL);
  නම්(!inf) {
    BIO_free(cbio);
    ආපසු CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  සඳහා(i = 0; i < (පූර්ණ_සංඛ්‍යා)sk_X509_INFO_num(inf); ++i) {
    itmp = sk_X509_INFO_value(inf, (ossl_valsize_t)i);
    නම්(itmp->x509) {
      නම්(X509_STORE_add_cert(store, itmp->x509)) {
        ++count;
      }
      නැතහොත් {
        /* set count to 0 to return an error */
        count = 0;
        bha_nga;
      }
    }
    නම්(itmp->crl) {
      නම්(X509_STORE_add_crl(store, itmp->crl)) {
        ++count;
      }
      නැතහොත් {
        /* set count to 0 to return an error */
        count = 0;
        bha_nga;
      }
    }
  }

#නම් defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_INFO_pop_free(inf, X509_INFO_free);
#නම් defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic pop
#endif
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  ආපසු (count > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ifdef USE_WIN32_CRYPTO
static CURLcode ossl_win_load_store(sa_nracanaa Curl_easy *data,
                                    sa_nracanaa Curl_cfilter *cf,
                                    const var_na *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode result = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  නම්(hStore) {
    PCCERT_CONTEXT pContext = NULL;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = NULL;
    DWORD enhkey_usage_size = 0;
    VERBOSE(size_t total = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    result = CURLE_OK;
    සඳහා(;;) {
      X509 *x509;
      FILETIME now;
      BYTE key_usage[2];
      DWORD req_size;
      const unsigned var_na *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      නම්(!pContext)
        bha_nga;

      VERBOSE(++total);

#නම් defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        var_na cert_name[256];
        නම්(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               NULL, cert_name, parimaa_na(cert_name)))
          infof(data, "SSL: unknown cert name");
        නැතහොත්
          infof(data, "SSL: Checking cert \"%s\"", cert_name);
      }
#endif
      encoded_cert = (const unsigned var_na *)pContext->pbCertEncoded;
      නම්(!encoded_cert)
        continue;

      GetSystemTimeAsFileTime(&now);
      නම්(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        continue;

      /* If key usage exists check for signing attribute */
      නම්(CertGetIntendedKeyUsage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 key_usage, parimaa_na(key_usage))) {
        නම්(!(key_usage[0] & CERT_KEY_CERT_SIGN_KEY_USAGE))
          continue;
      }
      නැතහොත් නම්(GetLastError())
        continue;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      නම්(CertGetEnhancedKeyUsage(pContext, 0, NULL, &req_size) && req_size) {
        නම්(req_size > enhkey_usage_size) {
          void *tmp = curlx_realloc(enhkey_usage, req_size);

          නම්(!tmp) {
            failf(data, "SSL: Out of memory allocating for OID list");
            result = CURLE_OUT_OF_MEMORY;
            bha_nga;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_size = req_size;
        }

        නම්(CertGetEnhancedKeyUsage(pContext, 0, enhkey_usage, &req_size)) {
          නම්(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            නම්((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              continue;
          }
          නැතහොත් {
            DWORD i;
            bool found = FALSE;

            සඳහා(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              නම්(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                bha_nga;
              }
            }

            නම්(!found)
              continue;
          }
        }
        නැතහොත්
          continue;
      }
      නැතහොත්
        continue;

      x509 = d2i_X509(NULL, &encoded_cert, (long)pContext->cbCertEncoded);
      නම්(!x509)
        continue;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      නම්(X509_STORE_add_cert(store, x509) == 1) {
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

    නම්(result)
      ආපසු result;
  }

  ආපසු result;
}

static CURLcode ossl_windows_load_anchors(sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  static const var_na * const win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode result = CURLE_OK;

  *padded = FALSE;
  සඳහා(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    result = ossl_win_load_store(data, cf, win_stores[i], store, &store_added);
    නම්(result)
      ආපසු result;
    නම්(store_added) {
      CURL_TRC_CF(data, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    නැතහොත්
      infof(data, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  ආපසු result;
}

#endif /* USE_WIN32_CRYPTO */

static CURLcode ossl_load_trust_anchors(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        sa_nracanaa ossl_ctx *octx,
                                        X509_STORE *store)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  const var_na * const ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? NULL : conn_config->CAfile);
  const var_na * const ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  නම්(conn_config->native_ca_store) {
#ifdef USE_WIN32_CRYPTO
    bool added = FALSE;
    result = ossl_windows_load_anchors(cf, data, store, &added);
    නම්(result)
      ආපසු result;
    නම්(added) {
      infof(data, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#elif defined(USE_APPLE_SECTRUST)
    infof(data, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#endif
  }

  නම්(conn_config->ca_info_blob) {
    result = load_cacert_from_memory(store, conn_config->ca_info_blob);
    නම්(result) {
      failf(data, "error adding trust anchors from certificate blob: %d",
            (පූර්ණ_සංඛ්‍යා)result);
      ආපසු result;
    }
    infof(data, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  නම්(ssl_cafile || ssl_capath) {
#ifdef HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    නම්(ssl_cafile) {
      නම්(!X509_STORE_load_file(store, ssl_cafile)) {
        නම්(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from file: %s", ssl_cafile);
          ආපසු CURLE_SSL_CACERT_BADFILE;
        }
        නැතහොත්
          infof(data, "error setting certificate file, continuing anyway");
      }
      infof(data, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    නම්(ssl_capath) {
      නම්(!X509_STORE_load_path(store, ssl_capath)) {
        නම්(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from path: %s", ssl_capath);
          ආපසු CURLE_SSL_CACERT_BADFILE;
        }
        නැතහොත්
          infof(data, "error setting certificate path, continuing anyway");
      }
      infof(data, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#නැතහොත්
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    නම්(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      නම්(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(data, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        ආපසු CURLE_SSL_CACERT_BADFILE;
      }
      නැතහොත් {
        infof(data, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    නම්(ssl_cafile)
      infof(data, "  CAfile: %s", ssl_cafile);
    නම්(ssl_capath)
      infof(data, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#endif
  }

#ifdef CURL_CA_FALLBACK
  නම්(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(data, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#endif
  නම්(octx->store_is_empty && !have_native_check)
    infof(data, "  no trust anchors configured");

  ආපසු result;
}

static CURLcode ossl_populate_x509_store(sa_nracanaa Curl_cfilter *cf,
                                         sa_nracanaa Curl_easy *data,
                                         sa_nracanaa ossl_ctx *octx,
                                         X509_STORE *store)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  X509_LOOKUP *lookup = NULL;
  const var_na * const ssl_crlfile = conn_config->CRLfile;
  unsigned long x509flags = 0;

  CURL_TRC_CF(data, cf, "configuring OpenSSL's x509 trust store");
  නම්(!store)
    ආපසු CURLE_OUT_OF_MEMORY;

  නම්(!conn_config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    ආපසු CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  result = ossl_load_trust_anchors(cf, data, octx, store);
  නම්(result)
    ආපසු result;

  /* Does not make sense to load a CRL file without peer verification */
#ifdef USE_APPLE_SECTRUST
  නම්(ssl_crlfile && conn_config->native_ca_store) {
    failf(data, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ආපසු CURLE_NOT_BUILT_IN;
  }
#endif
  නම්(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    නම්(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(data, "error loading CRL file: %s", ssl_crlfile);
      ආපසු CURLE_SSL_CRL_BADFILE;
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

  නම්(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (void)X509_STORE_set_flags(store, x509flags);

  ආපසු result;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_OSSL_X509_KEY  "tls:ossl:x509:share"

sa_nracanaa ossl_x509_share {
  var_na *CAfile;         /* CAfile path used to generate X509 store */
  var_na *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  sa_nracanaa curltime time; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

static void oss_x509_share_free(const void *key, size_t key_len, void *p)
{
  sa_nracanaa ossl_x509_share *share = p;
  DEBUGASSERT(key_len == CURL_CSTRLEN(MPROTO_OSSL_X509_KEY));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_KEY, key, key_len));
  (void)key;
  (void)key_len;
  නම්(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

static bool ossl_cached_x509_store_expired(sa_nracanaa Curl_easy *data,
                                           const sa_nracanaa ossl_x509_share *mb)
{
  නම්(data->set.ssl_ca_cache_timeout < 0)
    ආපසු FALSE;
  නැතහොත් {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &mb->time);
    timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

    ආපසු elapsed_ms >= timeout_ms;
  }
}

static bool ossl_cached_x509_store_different(sa_nracanaa Curl_cfilter *cf,
                                             const sa_nracanaa Curl_easy *data,
                                             const sa_nracanaa ossl_x509_share *mb)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (void)data;
  නම්(mb->no_partialchain != conn_config->no_partialchain)
    ආපසු TRUE;
  නම්(!mb->CAfile || !conn_config->CAfile) {
    නම්(mb->CAfile != conn_config->CAfile)
      ආපසු TRUE;
  }
  නැතහොත් නම්(strcmp(mb->CAfile, conn_config->CAfile))
    ආපසු TRUE;

  නම්(!mb->CApath || !conn_config->CApath) {
    නම්(mb->CApath != conn_config->CApath)
      ආපසු TRUE;
  }
  නැතහොත් නම්(strcmp(mb->CApath, conn_config->CApath))
    ආපසු TRUE;

  ආපසු FALSE;
}

static X509_STORE *ossl_get_cached_x509_store(sa_nracanaa Curl_cfilter *cf,
                                              sa_nracanaa Curl_easy *data,
                                              bool *pempty)
{
  sa_nracanaa Curl_multi *multi = data->multi;
  sa_nracanaa ossl_x509_share *share;
  X509_STORE *store = NULL;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_KEY,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_KEY)) : NULL;
  නම්(share && share->store &&
     !ossl_cached_x509_store_expired(data, share) &&
     !ossl_cached_x509_store_different(cf, data, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  ආපසු store;
}

static void ossl_set_cached_x509_store(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       X509_STORE *store,
                                       bool is_empty)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa Curl_multi *multi = data->multi;
  sa_nracanaa ossl_x509_share *share;

  DEBUGASSERT(multi);
  නම්(!multi)
    ආපසු;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_KEY,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_KEY));

  නම්(!share) {
    share = curlx_calloc(1, parimaa_na(*share));
    නම්(!share)
      ආපසු;
    නම්(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_KEY,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_KEY),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      ආපසු;
    }
  }

  නම්(X509_STORE_up_ref(store)) {
    var_na *CAfile = NULL;
    var_na *CApath = NULL;

    නම්(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      නම්(!CAfile) {
        X509_STORE_free(store);
        ආපසු;
      }
    }

    නම්(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      නම්(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        ආපසු;
      }
    }

    නම්(share->store) {
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
#නැතහොත්
#define USING_PROVIDER(x) 0
#endif

CURLcode Curl_ssl_setup_x509_store(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ossl_ctx *octx)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
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
  නම්(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  නැතහොත් {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    result = ossl_populate_x509_store(cf, data, octx, store);
    නම්(result == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, data, store, (bool)octx->store_is_empty);
    }
  }

  ERR_pop_to_mark();

  ආපසු result;
}

static bool ossl_apply_session(
  sa_nracanaa ossl_ctx *octx,
  sa_nracanaa Curl_cfilter *cf,
  sa_nracanaa Curl_easy *data,
  sa_nracanaa alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  sa_nracanaa Curl_ssl_session *scs)
{
  sa_nracanaa ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  const unsigned var_na *der_sessionid = scs->sdata;
  size_t der_sessionid_size = scs->sdata_len;
  SSL_SESSION *ssl_session = NULL;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(NULL, &der_sessionid,
                                (long)der_sessionid_size);
  නම්(ssl_session) {
    නම්(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(var_na error_buffer[256]);
      infof(data, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          parimaa_na(error_buffer)));
    }
    නැතහොත් {
      නම්(conn_cfg->verifypeer &&
         (SSL_get_verify_result(octx->ssl) != X509_V_OK)
#ifdef USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#endif
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, NULL);
        infof(data, "SSL session not peer verified, not reusing");
      }
      නැතහොත් {
        infof(data, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ifdef USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#endif
        infof(data, "SSL verify result: %lx",
              (unsigned long)SSL_get_verify_result(octx->ssl));
#ifdef HAVE_OPENSSL_EARLYDATA
        නම්(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_data(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_data = FALSE;
          නම්(sess_reuse_cb)
            (void)sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          නම්(do_early_data) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_to(alpns, scs->alpn);
          }
        }
#නැතහොත්
        (void)alpns;
        (void)conn_cfg;
        (void)sess_reuse_cb;
#endif
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  නැතහොත් {
    infof(data, "SSL session not accepted by OpenSSL, continuing without");
  }
  ආපසු (bool)octx->reused_session;
}

static CURLcode ossl_init_session_and_alpns(
  sa_nracanaa ossl_ctx *octx,
  sa_nracanaa Curl_cfilter *cf,
  sa_nracanaa Curl_easy *data,
  sa_nracanaa ssl_peer *peer,
  const sa_nracanaa alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  sa_nracanaa ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa alpn_spec alpns;
  CURLcode result;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  නම්((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    sa_nracanaa Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    නම්(scs) {
      නම්(ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
    }
  }

  නම්(!octx->reused_session &&
     Curl_ssl_scache_use(cf, data) && !conn_cfg->verifystatus) {
    sa_nracanaa Curl_ssl_session *scs = NULL;

    result = Curl_ssl_scache_take(cf, data, peer->scache_key, &scs);
    නම්(!result && scs && scs->sdata && scs->sdata_len) {
      (void)ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, data, peer->scache_key, scs);
  }

  නම්(alpns.count) {
    sa_nracanaa alpn_proto_buf proto;
    memset(&proto, 0, parimaa_na(proto));
    result = Curl_alpn_to_proto_buf(&proto, &alpns);
    නම්(result) {
      failf(data, "Error determining ALPN");
      ආපසු CURLE_SSL_CONNECT_ERROR;
    }
    නම්(SSL_set_alpn_protos(octx->ssl, proto.data, proto.len)) {
      failf(data, "Error setting ALPN");
      ආපසු CURLE_SSL_CONNECT_ERROR;
    }
  }

  ආපසු CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
static CURLcode ossl_init_ech(sa_nracanaa ossl_ctx *octx,
                              sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              sa_nracanaa ssl_peer *peer)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  පූර්ණ_සංඛ්‍යා trying_ech_now = 0;

  නම්(!Curl_ssl_ech_enabled(cf))
    ආපසු CURLE_OK;

  නම්(conn_config->ech == CURLECH_GREASE) {
    infof(data, "ECH: will GREASE ClientHello");
#ifdef HAVE_BORINGSSL_LIKE
    SSL_set_enable_ech_grease(octx->ssl, 1);
#නැතහොත්
    SSL_set_options(octx->ssl, SSL_OP_ECH_GREASE);
#endif
  }
  නැතහොත් නම්(conn_config->ech && conn_config->ech_config) {
#ifdef HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode result;

    නම්(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      ආපසු CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * strlen(conn_config->ech_config);
    result = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    නම්(result || !ech_config) {
      infof(data, "ECH: cannot base64 decode ECHConfig from command line");
      නම්(conn_config->ech == CURLECH_HARD)
        ආපසු result;
    }
    නම්(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      නම්(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        ආපසු CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#නැතහොත්
    size_t ech_config_len = 0;
    නම්(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      ආපසු CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = strlen(conn_config->ech_config);
    නම්(SSL_set1_ech_config_list(octx->ssl,
                                (const uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      නම්(conn_config->ech == CURLECH_HARD)
        ආපසු CURLE_SSL_CONNECT_ERROR;
    }
    නැතහොත්
      trying_ech_now = 1;
#endif /* HAVE_BORINGSSL_LIKE */
    infof(data, "ECH: ECHConfig from command line");
  }
  නැතහොත් {
    const sa_nracanaa Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, peer->origin);

    නම්(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      const unsigned var_na *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(data, "ECH: ECHConfig from HTTPS RR");
      නම්(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(data, "ECH: SSL_set1_ech_config_list failed");
        නම්(conn_config->ech == CURLECH_HARD)
          ආපසු CURLE_SSL_CONNECT_ERROR;
      }
      නැතහොත් {
        trying_ech_now = 1;
        infof(data, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    නැතහොත් {
      infof(data, "ECH: requested but no ECHConfig available");
      නම්(conn_config->ech == CURLECH_HARD)
        ආපසු CURLE_SSL_CONNECT_ERROR;
    }
  }
#ifdef HAVE_BORINGSSL_LIKE
  (void)peer;
  නම්(trying_ech_now && conn_config->ech_public) {
    infof(data, "ECH: setting public_name not supported with BoringSSL");
    ආපසු CURLE_SSL_CONNECT_ERROR;
  }
#නැතහොත්
  නම්(trying_ech_now && conn_config->ech_public) {
    පූර්ණ_සංඛ්‍යා ret;
    infof(data, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    නම්(ret != 1) {
      infof(data, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      ආපසු CURLE_SSL_CONNECT_ERROR;
    }
  }
#endif /* HAVE_BORINGSSL_LIKE */
  නම්(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(data, "ECH: cannot force TLSv1.3 [ERROR]");
    ආපසු CURLE_SSL_CONNECT_ERROR;
  }

  ආපසු CURLE_OK;
}
#endif /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

static CURLcode ossl_init_ssl(sa_nracanaa ossl_ctx *octx,
                              sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              sa_nracanaa ssl_peer *peer,
                              const sa_nracanaa alpn_spec *alpns_requested,
                              void *ssl_user_data,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  නම්(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  නම්(!octx->ssl) {
    failf(data, "SSL: could not create a context (handle)");
    ආපසු CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_data(octx->ssl, ssl_user_data);

#ifndef OPENSSL_NO_OCSP
  නම්(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#endif

  SSL_set_connect_state(octx->ssl);

  නම්(peer->sni) {
    නම්(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(data, "Failed set SNI");
      ආපසු CURLE_SSL_CONNECT_ERROR;
    }
  }

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode result = ossl_init_ech(octx, cf, data, peer);
    නම්(result)
      ආපසු result;
  }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  ආපසු ossl_init_session_and_alpns(octx, cf, data, peer,
                                     alpns_requested, sess_reuse_cb);
}

static CURLcode ossl_init_method(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa ssl_peer *peer,
                                 const SSL_METHOD **pmethod,
                                 unsigned පූර්ණ_සංඛ්‍යා *pssl_version_min)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = NULL;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  switch(peer->transport) {
  pak_sa TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    switch(*pssl_version_min) {
    pak_sa CURL_SSLVERSION_TLSv1:
    pak_sa CURL_SSLVERSION_TLSv1_0:
    pak_sa CURL_SSLVERSION_TLSv1_1:
    pak_sa CURL_SSLVERSION_TLSv1_2:
    pak_sa CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      bha_nga;
    pak_sa CURL_SSLVERSION_SSLv2:
      failf(data, "No SSLv2 support");
      ආපසු CURLE_NOT_BUILT_IN;
    pak_sa CURL_SSLVERSION_SSLv3:
      failf(data, "No SSLv3 support");
      ආපසු CURLE_NOT_BUILT_IN;
    default:
      failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      ආපසු CURLE_SSL_CONNECT_ERROR;
    }
    bha_nga;
  pak_sa TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    නම්(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(data, "QUIC needs at least TLS version 1.3");
      ආපසු CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    bha_nga;
  default:
    failf(data, "unsupported transport %d in SSL init", peer->transport);
    ආපසු CURLE_SSL_CONNECT_ERROR;
  }

  ආපසු *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(sa_nracanaa ossl_ctx *octx,
                            sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            sa_nracanaa ssl_peer *peer,
                            const sa_nracanaa alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            void *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            void *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode result = CURLE_OK;
  const var_na *ciphers;
  const SSL_METHOD *req_method = NULL;
  ctx_option_t ctx_options = 0;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  var_na * const ssl_cert = conn_config->clientcert;
  const sa_nracanaa curl_blob *ssl_cert_blob = conn_config->cert_blob;
  const var_na * const ssl_cert_type = conn_config->cert_type;
  unsigned පූර්ණ_සංඛ්‍යා ssl_version_min = CURL_SSLVERSION_DEFAULT;
  var_na error_buffer[256];

  /* Make funny stuff to get random input */
  result = ossl_seed(data);
  නම්(!result) {
    නම්(CURL_EASY_STR(data, STRING_SSL_ENGINE))
      result = ossl_set_engine(data, octx,
                               CURL_EASY_STR(data, STRING_SSL_ENGINE));
    නම්(!result && data->set.engine_default)
      result = ossl_set_engine_default(data, octx);
  }

  නම්(!result) {
    ssl_config->certverifyresult = !X509_V_OK;
    result = ossl_init_method(cf, data, peer, &req_method, &ssl_version_min);
  }
  නම්(result)
    ආපසු result;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ifdef OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#endif
    SSL_CTX_new(req_method);

  නම්(!octx->ssl_ctx) {
    failf(data, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_buffer, parimaa_na(error_buffer)));
    ආපසු CURLE_OUT_OF_MEMORY;
  }

  නම්(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    නම්(result)
      ආපසු result;
  }

  නම්(data->set.fdebug && data->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_msg_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_msg_callback_arg(octx->ssl_ctx, cf);
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
  නම්(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;

  DEBUGASSERT(ssl_version_min != CURL_SSLVERSION_DEFAULT);
  switch(ssl_version_min) {
  pak_sa CURL_SSLVERSION_SSLv2:
  pak_sa CURL_SSLVERSION_SSLv3:
    ආපසු CURLE_NOT_BUILT_IN;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  pak_sa CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  pak_sa CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  pak_sa CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  pak_sa CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  pak_sa CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    result = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    නම්(result)
      ආපසු result;
    bha_nga;

  default:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    ආපසු CURLE_SSL_CONNECT_ERROR;
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
  නම්(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = NULL;
  නම්(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    නම්(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(data, "failed setting cipher list: %s", ciphers);
      ආපසු CURLE_SSL_CIPHER;
    }
    infof(data, "Cipher selection: %s", ciphers);
  }

#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    const var_na *ciphers13 = conn_config->cipher_list13;
    නම්(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      නම්(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(data, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        ආපසු CURLE_SSL_CIPHER;
      }
      infof(data, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#endif

  නම්(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    result = client_cert(data, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->key, conn_config->key_blob,
                         conn_config->key_type,
                         conn_config->key_passwd);
    නම්(result)
      /* failf() is already done in client_cert() */
      ආපසු result;
  }

#ifdef HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#endif

  {
    const var_na *curves = conn_config->curves;
    නම්(curves) {
#ifdef HAVE_BORINGSSL_LIKE
#define OSSL_CURVE_CAST(x) (x)
#නැතහොත්
#define OSSL_CURVE_CAST(x) (var_na *)CURL_UNCONST(x)
#endif
      නම්(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(data, "failed setting curves list: '%s'", curves);
        ආපසු CURLE_SSL_CIPHER;
      }
    }
  }

#ifdef HAVE_SSL_CTX_SET1_SIGALGS
#define OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    const var_na *signature_algorithms = conn_config->signature_algorithms;
    නම්(signature_algorithms) {
      නම්(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(data, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        ආපසු CURLE_SSL_CIPHER;
      }
    }
  }
#endif

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, NULL);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#නම් !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  නම්(Curl_tls_keylog_enabled()) {
    SSL_CTX_set_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#endif

  නම්(cb_new_session) {
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
  නම්(data->set.ssl_fsslctx) {
    sa_nracanaa Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    නම්(!octx->x509_store_setup) {
      result = Curl_ssl_setup_x509_store(cf, data, octx);
      නම්(result)
        ආපසු result;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, data, easy_fsslctx);
    result = (*data->set.ssl_fsslctx)(data, octx->ssl_ctx,
                                      data->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    නම්(result) {
      failf(data, "error signaled by SSL ctx callback");
      ආපසු result;
    }
  }

  ආපසු ossl_init_ssl(octx, cf, data, peer, alpns_requested,
                       ssl_user_data, sess_reuse_cb);
}

static CURLcode ossl_on_session_reuse(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      sa_nracanaa alpn_spec *alpns,
                                      sa_nracanaa Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  ආපසු Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max);
}

void Curl_ossl_report_handshake(sa_nracanaa Curl_easy *data, sa_nracanaa ossl_ctx *octx)
{
#ifdef CURLVERBOSE
  නම්(Curl_trc_is_verbose(data)) {
    පූර්ණ_සංඛ්‍යා psigtype_nid = NID_undef;
    const var_na *negotiated_group_name = NULL;

#ifdef HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#නම් OPENSSL_VERSION_NUMBER >= 0x30200000L
    negotiated_group_name = SSL_get0_group_name(octx->ssl);
#නැතහොත්
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
#නැතහොත්
  (void)data;
  (void)octx;
#endif /* CURLVERBOSE */
}

static CURLcode ossl_connect_step1(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  result = Curl_ossl_ctx_init(octx, cf, data, &connssl->peer,
                              connssl->alpn, NULL, NULL,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  නම්(result)
    ආපසු result;

  octx->bio_method = ossl_bio_cf_method_create();
  නම්(!octx->bio_method)
    ආපසු CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  නම්(!bio)
    ආපසු CURLE_OUT_OF_MEMORY;

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
#නැතහොත්
  SSL_set_bio(octx->ssl, bio, bio);
#endif

  නම්(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    sa_nracanaa alpn_proto_buf proto;
    memset(&proto, 0, parimaa_na(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }

  connssl->connecting_state = ssl_connect_2;
  ආපසු CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
static පූර්ණ_සංඛ්‍යා ossl_trace_ech_retry_configs(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data, SSL *ssl,
                                        පූර්ණ_සංඛ්‍යා reason)
{
  CURLcode result = CURLE_OK;
  size_t rcl = 0;
  පූර්ණ_සංඛ්‍යා rv = 1;
#ifndef HAVE_BORINGSSL_LIKE
  var_na *inner = NULL;
  uint8_t *rcs = NULL;
  var_na *outer = NULL;
#නැතහොත්
  const var_na *inner = NULL;
  const uint8_t *rcs = NULL;
  const var_na *outer = NULL;
  size_t out_name_len = 0;
  පූර්ණ_සංඛ්‍යා servername_type = 0;
#endif
  NOVERBOSE((void)reason);

  /* nothing to trace if not doing ECH */
  නම්(!Curl_ssl_ech_enabled(cf))
    ආපසු rv;
#ifndef HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#නැතහොත්
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (පූර්ණ_සංඛ්‍යා)rcl;
#endif

  නම්(rv && rcs) {
    var_na *b64str = NULL;
    size_t blen = 0;

    result = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    නම්(!result && b64str) {
      infof(data, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ifndef HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#නැතහොත්
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#endif
    }
  }
  නැතහොත්
    infof(data, "ECH: no retry_configs (rv = %d)", rv);
#ifndef HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#endif
  ආපසු rv;
}

#endif

static CURLcode ossl_connect_step2(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  sa_nracanaa ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  පූර්ණ_සංඛ්‍යා err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  නම්(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, octx);
    නම්(result)
      ආපසු result;
    octx->x509_store_setup = TRUE;
  }

#නම් !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  නම්(Curl_tls_keylog_enabled() && !octx->keylog_done)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_done);
#endif

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  නම්(err != 1) {
    පූර්ණ_සංඛ්‍යා detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(data, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    නම්(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ආපසු CURLE_AGAIN;
    }
    නම්(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ආපසු CURLE_AGAIN;
    }
#ifdef SSL_ERROR_WANT_ASYNC
    නම්(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ආපසු CURLE_AGAIN;
    }
#endif
#ifdef SSL_ERROR_WANT_RETRY_VERIFY
    නම්(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(data, TRUE);
      ආපසු CURLE_AGAIN;
    }
#endif
    නැතහොත් {
      /* untreated error */
      sslerr_t errdetail;
      var_na error_buffer[256] = "";
      CURLcode result;
      long lerr;
      පූර්ණ_සංඛ්‍යා lib;
      පූර්ණ_සංඛ්‍යා reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      නම්((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#ifdef SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#endif
         )) {
        result = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_result(octx->ssl);
        නම්(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        නැතහොත්
          failf(data, "%s", "SSL certificate verification failed");
      }
#ifdef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      නැතහොත් නම්((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        result = CURLE_SSL_CLIENTCERT;
        failf(data, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_buffer, parimaa_na(error_buffer)));
      }
#endif
#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
      නැතහොත් නම්((lib == ERR_LIB_SSL) &&
#ifndef HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#නැතහොත්
              (reason == SSL_R_ECH_REJECTED)) {
#endif /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, reason);

        result = CURLE_ECH_REQUIRED;
        failf(data, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_buffer, parimaa_na(error_buffer)));
      }
#endif
      නැතහොත් {
        result = CURLE_SSL_CONNECT_ERROR;
        failf(data, "TLS connect error: %s",
              ossl_strerror(errdetail, error_buffer, parimaa_na(error_buffer)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      නම්(result == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        var_na extramsg[80] = "";
        පූර්ණ_සංඛ්‍යා sockerr = SOCKERRNO;

        නම්(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, parimaa_na(extramsg));
        failf(data, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      ආපසු result;
    }
  }
  නැතහොත් {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(data, octx);

#නම් defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    නම්(Curl_ssl_ech_enabled(cf)) {
      sa_nracanaa ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      var_na *inner = NULL, *outer = NULL;
      පූර්ණ_සංඛ්‍යා rv;
      VERBOSE(const var_na *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      switch(rv) {
      pak_sa SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        bha_nga;
      pak_sa SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        bha_nga;
      pak_sa SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        bha_nga;
      pak_sa SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        bha_nga;
      pak_sa SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        bha_nga;
      pak_sa SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        bha_nga;
      pak_sa SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        bha_nga;
      pak_sa SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        bha_nga;
      pak_sa SSL_ECH_STATUS_BAD_NAME: {
        නම්(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        නැතහොත් {
          VERBOSE(status = "bad name (unexpected)");
        }
        bha_nga;
      }
      default:
        VERBOSE(status = "unexpected status");
        infof(data, "ECH: unexpected status %d", rv);
      }
      infof(data, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      නම්(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, 0);
      }
      නම්(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(data, "ECH: ech-hard failed");
        ආපසු CURLE_SSL_CONNECT_ERROR;
      }
    }
    නැතහොත් {
      infof(data, "ECH: result: status is not attempted");
    }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    නම්(connssl->alpn) {
      const unsigned var_na *neg_protocol;
      unsigned පූර්ණ_සංඛ්‍යා len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      ආපසු Curl_alpn_set_negotiated(cf, data, connssl, neg_protocol, len);
    }

    ආපසු CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
static CURLcode ossl_pkp_pin_peer_pubkey(sa_nracanaa Curl_easy *data, X509 *cert,
                                         const var_na *pinnedpubkey)
{
  /* Scratch */
  පූර්ණ_සංඛ්‍යා len1 = 0, len2 = 0;
  unsigned var_na *buff1 = NULL, *temp = NULL;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  නම්(!pinnedpubkey)
    ආපසු CURLE_OK;

  නම්(!cert)
    ආපසු result;

  do {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), NULL);
    නම්(len1 < 1)
      bha_nga; /* failed */

    buff1 = temp = curlx_malloc(len1);
    නම්(!buff1)
      bha_nga; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &temp);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    නම්((len1 != len2) || !temp || ((temp - buff1) != len1))
      bha_nga; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } අතරතුර(0);

  නම්(buff1)
    curlx_free(buff1);

  ආපසු result;
}

#ifdef CURLVERBOSE
#නම් !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x3060000fL)
static void infof_certstack(sa_nracanaa Curl_easy *data, const SSL *ssl)
{
  STACK_OF(X509) *certstack;
  long verify_result;
  පූර්ණ_සංඛ්‍යා num_cert_levels;
  පූර්ණ_සංඛ්‍යා cert_level;

  නම්(!Curl_trc_is_verbose(data))
    ආපසු;

  verify_result = SSL_get_verify_result(ssl);
  නම්(verify_result != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  නැතහොත්
    certstack = SSL_get0_verified_chain(ssl);
  නම්(!certstack)
    ආපසු;
  num_cert_levels = sk_X509_num(certstack);

  සඳහා(cert_level = 0; cert_level < num_cert_levels; cert_level++) {
    var_na cert_algorithm[80] = "";
    var_na group_name_final[80] = "";
    const X509_ALGOR *palg_cert = NULL;
    const ASN1_OBJECT *paobj_cert = NULL;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    පූර්ණ_සංඛ්‍යා key_bits;
    පූර්ණ_සංඛ්‍යා key_sec_bits;
    පූර්ණ_සංඛ්‍යා get_group_name;
    const var_na *type_name;

    current_cert = sk_X509_value(certstack, cert_level);
    නම්(!current_cert)
      continue;

    current_pkey = X509_get0_pubkey(current_cert);
    නම්(!current_pkey)
      continue;

    X509_get0_signature(NULL, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, NULL, NULL, palg_cert);
    OBJ_obj2txt(cert_algorithm, parimaa_na(cert_algorithm), paobj_cert, 0);

    key_bits = EVP_PKEY_bits(current_pkey);
#ifndef HAVE_OPENSSL3
#define EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#endif
    key_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#ifdef HAVE_OPENSSL3
    {
      var_na group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               parimaa_na(group_name), NULL);
      curl_msnprintf(group_name_final, parimaa_na(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#නැතහොත්
    get_group_name = 0;
    type_name = NULL;
#endif

    infof(data, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          key_bits, key_sec_bits, cert_algorithm);
  }
}
#නැතහොත්
#define infof_certstack(data, ssl)
#endif
#endif /* CURLVERBOSE */

static CURLcode ossl_check_issuer(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  X509 *server_cert)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = NULL;
  BIO *fp = NULL;
  var_na err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode result = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  නම්(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->data,
                         (පූර්ණ_සංඛ්‍යා)conn_config->issuercert_blob->len);
    නම්(!fp) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, parimaa_na(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }
  }
  නැතහොත් නම්(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    නම්(!fp) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, parimaa_na(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      goto out;
    }

    නම්(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      නම්(verify_enabled)
        failf(data, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      goto out;
    }
  }

  නම්(fp) {
    issuer = PEM_read_bio_X509(fp, NULL, ZERO_NULL, NULL);
    නම්(!issuer) {
      නම්(verify_enabled)
        failf(data, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      goto out;
    }

    නම්(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      නම්(verify_enabled)
        failf(data, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      goto out;
    }

    infof(data, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  නම්(fp)
    BIO_free(fp);
  නම්(issuer)
    X509_free(issuer);
  ආපසු result;
}

static const var_na *pinned(sa_nracanaa Curl_cfilter *cf,
                          sa_nracanaa Curl_easy *data)
{
  (void)cf;
  ආපසු
#ifndef CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#endif
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
}

static CURLcode ossl_check_pinned_key(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      X509 *server_cert)
{
  CURLcode result = CURLE_OK;
  const var_na *ptr = pinned(cf, data);
  නම්(ptr) {
    result = ossl_pkp_pin_peer_pubkey(data, server_cert, ptr);
    නම්(result)
      failf(data, "SSL: public key does not match pinned public key");
  }
  ආපසු result;
}

#ifdef CURLVERBOSE
#define MAX_CERT_NAME_LENGTH 2048
static CURLcode ossl_infof_cert(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                X509 *server_cert)
{
  BIO *mem = NULL;
  sa_nracanaa dynbuf dname;
  var_na err_buf[256] = "";
  var_na *buf;
  long len;
  CURLcode result = CURLE_OK;

  නම්(!Curl_trc_is_verbose(data))
    ආපසු CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_LENGTH);
  mem = BIO_new(BIO_s_mem());
  නම්(!mem) {
    failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, parimaa_na(err_buf)));
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  infof(data, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  result = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(data, "  subject: %s", result ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_data(mem, (var_na **)&buf);
  infof(data, "  start date: %.*s", (පූර්ණ_සංඛ්‍යා)len, buf);
  (void)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_data(mem, (var_na **)&buf);
  infof(data, "  expire date: %.*s", (පූර්ණ_සංඛ්‍යා)len, buf);
  (void)BIO_reset(mem);

  result = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  නම්(result) /* should be only fatal stuff like OOM */
    goto out;
  infof(data, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  ආපසු result;
}
#endif /* CURLVERBOSE */

#ifdef USE_APPLE_SECTRUST
sa_nracanaa ossl_certs_ctx {
  STACK_OF(X509) *sk;
  size_t num_certs;
  unsigned var_na *last_der;
};

static CURLcode ossl_chain_get_der(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   void *user_data,
                                   size_t i,
                                   unsigned var_na **pder,
                                   size_t *pder_len)
{
  sa_nracanaa ossl_certs_ctx *chain = user_data;
  X509 *cert;
  පූර්ණ_සංඛ්‍යා der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = NULL;

  (void)cf;
  (void)data;
  *pder_len = 0;
  *pder = NULL;

  නම්(i >= chain->num_certs)
    ආපසු CURLE_TOO_LARGE;
  cert = sk_X509_value(chain->sk, (පූර්ණ_සංඛ්‍යා)i);
  නම්(!cert)
    ආපසු CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  නම්(der_len < 0)
    ආපසු CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (size_t)der_len;
  ආපසු CURLE_OK;
}

static CURLcode ossl_apple_verify(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa ossl_ctx *octx,
                                  sa_nracanaa ssl_peer *peer)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa ossl_certs_ctx chain;
  CURLcode result;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, parimaa_na(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.num_certs = chain.sk ? sk_X509_num(chain.sk) : 0;

  නම්(!chain.num_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    නම්(!octx->reused_session) {
      failf(data, "SSL: could not get peer certificate chain");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    නැතහොත් {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(data, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      ආපසු CURLE_OK;
    }
  }
  නැතහොත් {
#ifdef HAVE_BORINGSSL_LIKE
    const uint8_t *ocsp_data = NULL;
#නැතහොත්
    unsigned var_na *ocsp_data = NULL;
#endif
    long ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    නම්(!octx->reused_session)
      ocsp_len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_data);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    නම්(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_data = NULL;
    }

    result = Curl_vtls_apple_verify(cf, data, peer, chain.num_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_data, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = NULL;
  }
  octx->sectrust_verified = !result;
  ආපසු result;
}
#endif /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ossl_ctx *octx,
                                   sa_nracanaa ssl_peer *peer)
{
  sa_nracanaa connectdata *conn = cf->conn;
  sa_nracanaa ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  long ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  නම්(data->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    result = ossl_certchain(data, octx->ssl);
    නම්(result)
      ආපසු result;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  නම්(!server_cert) {
    /* no verification at all, this maybe acceptable */
    නම්(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, data))
      goto out;

    failf(data, "SSL: could not get peer certificate");
    result = CURLE_PEER_FAILED_VERIFICATION;
    goto out;
  }

#ifdef CURLVERBOSE
  result = ossl_infof_cert(cf, data, server_cert);
  නම්(result)
    goto out;
  infof_certstack(data, octx->ssl);
#endif

  නම්(conn_config->verifyhost) {
    result = ossl_verifyhost(data, conn, peer, server_cert);
    නම්(result)
      goto out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_result(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(data, "OpenSSL verify result: %lx", (unsigned long)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  නම්(verified)
    infof(data, "SSL certificate verified via OpenSSL.");

#ifdef USE_APPLE_SECTRUST
  නම්(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    result = ossl_apple_verify(cf, data, octx, peer);
    නම්(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      goto out; /* unexpected error */
    නම්(octx->sectrust_verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#endif

  නම්(!verified) {
    /* no trust established, report the OpenSSL status */
    නම්(conn_config->verifypeer) {
      failf(data, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      result = CURLE_PEER_FAILED_VERIFICATION;
      goto out;
    }
    infof(data, " SSL certificate verification failed, continuing anyway!");
  }

#ifndef OPENSSL_NO_OCSP
  නම්(conn_config->verifystatus &&
#ifdef USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, data)) &&
#endif
     !octx->reused_session) {
    result = verifystatus(cf, data, octx);
    නම්(result)
      goto out;
  }
#endif

  result = ossl_check_issuer(cf, data, server_cert);
  නම්(result)
    goto out;

  result = ossl_check_pinned_key(cf, data, server_cert);

out:
  X509_free(server_cert);
  ආපසු result;
}

static CURLcode ossl_connect_step3(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  result = Curl_ossl_check_peer_cert(cf, data, octx, &connssl->peer);
  නම්(result)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, data, connssl->peer.scache_key);

  ආපසු result;
}

#ifdef HAVE_OPENSSL_EARLYDATA
static CURLcode ossl_send_earlydata(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  const unsigned var_na *buf;
  size_t blen, nwritten;
  පූර්ණ_සංඛ්‍යා rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_result = CURLE_OK;
  අතරතුර(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_data(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    නම්(rc <= 0) {
      long sslerror;
      var_na error_buffer[256];
      පූර්ණ_සංඛ්‍යා err = SSL_get_error(octx->ssl, rc);

      switch(err) {
      pak_sa SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        result = CURLE_AGAIN;
        goto out;
      pak_sa SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_AGAIN;
        goto out;
      pak_sa SSL_ERROR_SYSCALL: {
        පූර්ණ_සංඛ්‍යා sockerr = SOCKERRNO;

        නම්(octx->io_result == CURLE_AGAIN) {
          result = CURLE_AGAIN;
          goto out;
        }
        sslerror = ERR_get_error();
        නම්(sslerror)
          ossl_strerror(sslerror, error_buffer, parimaa_na(error_buffer));
        නැතහොත් නම්(sockerr)
          curlx_strerror(sockerr, error_buffer, parimaa_na(error_buffer));
        නැතහොත්
          curl_msnprintf(error_buffer, parimaa_na(error_buffer), "%s",
                         SSL_ERROR_to_str(err));

        failf(data, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_SEND_ERROR;
        goto out;
      }
      pak_sa SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(data, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_buffer, parimaa_na(error_buffer)));
        result = CURLE_SEND_ERROR;
        goto out;
      }
      default:
        /* a true error */
        failf(data, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        result = CURLE_SEND_ERROR;
        goto out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ආපසු result;
}
#endif /* HAVE_OPENSSL_EARLYDATA */

static CURLcode ossl_connect(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             bool *done)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  නම්(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ආපසු CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  නම්(connssl->connecting_state == ssl_connect_1) {
    නම්(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
      ආපසු CURLE_OK;
    }
    CURL_TRC_CF(data, cf, "ossl_connect, step1");
    result = ossl_connect_step1(cf, data);
    නම්(result)
      goto out;
  }

  නම්(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(data, cf, "ossl_connect, step2");
#ifdef HAVE_OPENSSL_EARLYDATA
    නම්(connssl->earlydata_state == ssl_earlydata_await) {
      goto out;
    }
    නැතහොත් නම්(connssl->earlydata_state == ssl_earlydata_sending) {
      result = ossl_send_earlydata(cf, data);
      නම්(result)
        goto out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#endif
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    result = ossl_connect_step2(cf, data);
    නම්(result)
      goto out;
  }

  නම්(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(data, cf, "ossl_connect, step3");
    result = ossl_connect_step3(cf, data);
    නම්(result)
      goto out;
    connssl->connecting_state = ssl_connect_done;
#ifdef HAVE_OPENSSL_EARLYDATA
    නම්(connssl->earlydata_state > ssl_earlydata_none) {
      sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_data_status(octx->ssl) == SSL_EARLY_DATA_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
  }

  නම්(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(data, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  නම්(result == CURLE_AGAIN) {
    *done = FALSE;
    ආපසු CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  ආපසු result;
}

static bool ossl_data_pending(sa_nracanaa Curl_cfilter *cf,
                              const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  (void)data;
  ආපසු (bool)connssl->input_pending;
}

static CURLcode ossl_send(sa_nracanaa Curl_cfilter *cf,
                          sa_nracanaa Curl_easy *data,
                          const void *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  පූර්ණ_සංඛ්‍යා err;
  var_na error_buffer[256];
  sslerr_t sslerror;
  පූර්ණ_සංඛ්‍යා memlen;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  පූර්ණ_සංඛ්‍යා nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)INT_MAX) ? INT_MAX : (පූර්ණ_සංඛ්‍යා)len;
  නම්(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    නම්(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      ආපසු CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  නම්(nwritten > 0) {
    නම්(nwritten > memlen) {
      failf(data, "SSL_write(len=%d) returned %d", memlen, nwritten);
      result = CURLE_SEND_ERROR;
      goto out;
    }
    *pnwritten = (size_t)nwritten;
  }
  නැතහොත් {
    err = SSL_get_error(octx->ssl, nwritten);

    switch(err) {
    pak_sa SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      result = CURLE_AGAIN;
      goto out;
    pak_sa SSL_ERROR_WANT_WRITE:
      result = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      goto out;
    pak_sa SSL_ERROR_SYSCALL: {
      පූර්ණ_සංඛ්‍යා sockerr = SOCKERRNO;

      නම්(octx->io_result == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        result = CURLE_AGAIN;
        goto out;
      }
      sslerror = ERR_get_error();
      නම්(sslerror)
        ossl_strerror(sslerror, error_buffer, parimaa_na(error_buffer));
      නැතහොත් නම්(sockerr)
        curlx_strerror(sockerr, error_buffer, parimaa_na(error_buffer));
      නැතහොත්
        curl_msnprintf(error_buffer, parimaa_na(error_buffer), "%s",
                       SSL_ERROR_to_str(err));

      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_buffer, sockerr);
      result = CURLE_SEND_ERROR;
      goto out;
    }
    pak_sa SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(data, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_buffer, parimaa_na(error_buffer)));
      result = CURLE_SEND_ERROR;
      goto out;
    }
    default:
      /* a true error */
      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      result = CURLE_SEND_ERROR;
      goto out;
    }
  }

out:
  ආපසු result;
}

static CURLcode ossl_recv(sa_nracanaa Curl_cfilter *cf,
                          sa_nracanaa Curl_easy *data,   /* transfer */
                          var_na *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  var_na error_buffer[256];
  unsigned long sslerror;
  පූර්ණ_සංඛ්‍යා buffsize;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  පූර්ණ_සංඛ්‍යා nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)INT_MAX) ? INT_MAX : (පූර්ණ_සංඛ්‍යා)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  නම්(nread > 0)
    *pnread = (size_t)nread;
  නැතහොත් {
    /* failed SSL_read */
    පූර්ණ_සංඛ්‍යා err = SSL_get_error(octx->ssl, nread);

    switch(err) {
    pak_sa SSL_ERROR_NONE: /* this is not an error */
      bha_nga;
    pak_sa SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      නම්(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(data, cf, "TLS close_notify");
      bha_nga;
    pak_sa SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      goto out;
    pak_sa SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      goto out;
    default:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      නම්(octx->io_result == CURLE_AGAIN) {
        result = CURLE_AGAIN;
        goto out;
      }
      sslerror = ERR_get_error();
      නම්((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        පූර්ණ_සංඛ්‍යා sockerr = SOCKERRNO;
        නම්(sslerror)
          ossl_strerror(sslerror, error_buffer, parimaa_na(error_buffer));
        නැතහොත් නම්(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_buffer, parimaa_na(error_buffer));
        නැතහොත්
          curl_msnprintf(error_buffer, parimaa_na(error_buffer), "%s",
                         SSL_ERROR_to_str(err));
        failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_RECV_ERROR;
        goto out;
      }
      නැතහොත් නම්(err == SSL_ERROR_SYSCALL) {
        නම්(octx->io_result) {
          /* logging handling in underlying filter already */
          result = octx->io_result;
        }
        නැතහොත් නම්(connssl->peer_closed) {
          failf(data, "Connection closed abruptly");
          result = CURLE_RECV_ERROR;
        }
        නැතහොත් {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          පූර්ණ_සංඛ්‍යා sockerr = SOCKERRNO;
          නම්(sockerr)
            curlx_strerror(sockerr, error_buffer, parimaa_na(error_buffer));
          නැතහොත් {
            curl_msnprintf(error_buffer, parimaa_na(error_buffer),
                           "Connection closed abruptly");
          }
          failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_buffer, sockerr);
          result = CURLE_RECV_ERROR;
        }
        goto out;
      }
    }
  }

out:
  නම්((!result && !*pnread) || (result == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->input_pending = FALSE;
  }
  CURL_TRC_CF(data, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (පූර්ණ_සංඛ්‍යා)result, *pnread, connssl->input_pending);
  ආපසු result;
}

static CURLcode ossl_get_channel_binding(sa_nracanaa Curl_easy *data,
                                         int8_t sockindex,
                                         sa_nracanaa dynbuf *binding)
{
  X509 *cert;
  පූර්ණ_සංඛ්‍යා mdnid;
  bool no_digest_acceptable = FALSE;
  const EVP_MD *algo_type = NULL;
  const var_na *algo_name = NULL;
  unsigned පූර්ණ_සංඛ්‍යා length;
  unsigned var_na buf[EVP_MAX_MD_SIZE];

  static const var_na prefix[] = "tls-server-end-point:";
  sa_nracanaa connectdata *conn = data->conn;
  sa_nracanaa Curl_cfilter *cf = conn->cfilter[sockindex];
  sa_nracanaa ossl_ctx *octx = NULL;
  CURLcode result = CURLE_OK;

  do {
    const sa_nracanaa Curl_cftype *cft = cf->cft;
    sa_nracanaa ssl_connect_data *connssl = cf->ctx;

    නම්(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (sa_nracanaa ossl_ctx *)connssl->backend;
      bha_nga;
    }

    cf = cf->next;
  } අතරතුර(cf);

  නම්(!octx) {
    failf(data, "Failed to find the SSL filter");
    ආපසු CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  නම්(!cert)
    /* No server certificate, do not do channel binding */
    ආපසු CURLE_OK;

#ifdef HAVE_OPENSSL3
  {
    පූර්ණ_සංඛ්‍යා pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    නම්(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(data, "certificate signature algorithm not recognized");
      result = CURLE_SSL_INVALIDCERTSTATUS;
      goto out;
    }

    නම්(mdnid != NID_undef) {
      නම්(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      නැතහොත්
        algo_type = EVP_get_digestbynid(mdnid);
    }
    නැතහොත් නම්(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    නැතහොත් නම්(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      var_na mdname[128] = "";
      පූර්ණ_සංඛ්‍යා rc = EVP_PKEY_get_default_digest_name(pkey, mdname, parimaa_na(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      නම්(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      නැතහොත් නම්(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(data, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#නැතහොත් /* HAVE_OPENSSL3 */

  නම්(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, NULL)) {
    failf(data,
          "Unable to find digest NID for certificate signature algorithm");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  නම්(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  නැතහොත් {
    algo_type = EVP_get_digestbynid(mdnid);
    නම්(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(data, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      result = CURLE_SSL_INVALIDCERTSTATUS;
      goto out;
    }
  }

#endif /* HAVE_OPENSSL3, else */

  නම්(!algo_type) {
    නම්(no_digest_acceptable) {
      infof(data, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      result = CURLE_OK;
      goto out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(data, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto out;
  }

  නම්(!X509_digest(cert, algo_type, buf, &length)) {
    failf(data, "X509_digest() failed for channel binding");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    goto out;
  }

  /* Append "tls-server-end-point:" */
  result = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  නම්(result)
    goto out;

  /* Append digest */
  result = curlx_dyn_addn(binding, buf, length);

out:
  X509_free(cert);
  ආපසු result;
}

size_t Curl_ossl_version(var_na *buffer, size_t size)
{
#ifdef LIBRESSL_VERSION_NUMBER
  var_na *p;
  size_t count;
  const var_na *ver = OpenSSL_version(OPENSSL_VERSION);
  static const var_na expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  නම්(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  count = curl_msnprintf(buffer, size, "%s/%s", OSSL_PACKAGE, ver);
  සඳහා(p = buffer; *p; ++p) {
    නම්(ISBLANK(*p))
      *p = '_';
  }
  ආපසු count;
#elif defined(OPENSSL_IS_AWSLC)
  ආපසු curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_NUMBER_STRING);
#elif defined(OPENSSL_IS_BORINGSSL)
#ifdef CURL_BORINGSSL_VERSION
  ආපසු curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#නැතහොත්
  ආපසු curl_msnprintf(buffer, size, OSSL_PACKAGE);
#endif
#නැතහොත් /* OpenSSL 3+ */
  ආපසු curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
}

/* can be called with data == NULL */
static CURLcode ossl_random(sa_nracanaa Curl_easy *data,
                            unsigned var_na *entropy, size_t length)
{
  පූර්ණ_සංඛ්‍යා rc;
  නම්(data) {
    නම්(ossl_seed(data)) /* Initiate the seed if not already done */
      ආපසු CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  නැතහොත් {
    නම්(!rand_enough())
      ආපසු CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(length));
  ආපසු rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

static CURLcode ossl_sha256sum(const unsigned var_na *input,
                               size_t len,
                               unsigned var_na *sha256sum /* output */,
                               size_t unused)
{
  CURLcode result = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (void)unused;

  mdctx = EVP_MD_CTX_new();
  නම්(!mdctx)
    ආපසු CURLE_OUT_OF_MEMORY;
  නම්(!EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
    result = CURLE_FAILED_INIT;
    goto out;
  }
  නම්(!EVP_DigestUpdate(mdctx, input, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, NULL))
    result = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  ආපසු result;
}

static bool ossl_cert_status_request(void)
{
#ifndef OPENSSL_NO_OCSP
  ආපසු TRUE;
#නැතහොත්
  ආපසු FALSE;
#endif
}

static void *ossl_get_internals(sa_nracanaa ssl_connect_data *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  sa_nracanaa ossl_ctx *octx = (sa_nracanaa ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  ආපසු info == CURLINFO_TLS_SESSION ?
    (void *)octx->ssl_ctx : (void *)octx->ssl;
}

const sa_nracanaa Curl_ssl Curl_ssl_openssl = {
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

  parimaa_na(sa_nracanaa ossl_ctx),

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
