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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_OPENSSL

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "formdata.h" /* for the boundary function */
#અન્તર્ભાવ "url.h" /* for the SSL config check function */
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "vtls/openssl.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/vtls_int.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "vtls/keylog.h"
#અન્તર્ભાવ "vtls/hostcheck.h"
#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "multiif.h"
#અન્તર્ભાવ "curlx/strerr.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curlx/strcopy.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ "vtls/apple.h"
#ifdef USE_ECH
#અન્તર્ભાવ "curlx/base64.h"
#endif

#અન્તર્ભાવ <openssl/rand.h>
#અન્તર્ભાવ <openssl/x509v3.h>
#ifndef OPENSSL_NO_DSA
#અન્તર્ભાવ <openssl/dsa.h>
#endif
#અન્તર્ભાવ <openssl/dh.h>
#અન્તર્ભાવ <openssl/err.h>
#અન્તર્ભાવ <openssl/conf.h>
#અન્તર્ભાવ <openssl/bn.h>
#અન્તર્ભાવ <openssl/rsa.h>
#અન્તર્ભાવ <openssl/bio.h>
#અન્તર્ભાવ <openssl/pkcs12.h>
#અન્તર્ભાવ <openssl/tls1.h>
#અન્તર્ભાવ <openssl/evp.h>

#જો defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#અન્તર્ભાવ <openssl/ech.h>
#endif

#ifndef OPENSSL_NO_OCSP
#અન્તર્ભાવ <openssl/ocsp.h>
#endif

#જો !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#define USE_OPENSSL_ENGINE
#અન્તર્ભાવ <openssl/engine.h>
#endif

#ifdef LIBRESSL_VERSION_સંખ્યા
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  જો LIBRESSL_VERSION_સંખ્યા < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#અથવાજો !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#endif

#જો defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#અન્તર્ભાવ <openssl/provider.h>
#અન્તર્ભાવ <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#define OPENSSL_HAS_PROVIDERS
#endif

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#જો !defined(LIBRESSL_VERSION_સંખ્યા) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_ચાવી_EXPLICIT_PARAMS))
#define HAVE_SSL_CTX_SET_DEFAULT_READ_BUFFER_LEN 1
#endif

#જો defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#અન્તર્ભાવ <openssl/ui.h>
#endif

#ifdef HAVE_OPENSSL3
#define HAVE_EVP_PKEY_GET_PARAMS 1
#endif

#ifdef HAVE_EVP_PKEY_GET_PARAMS
#અન્તર્ભાવ <openssl/core_names.h>
#define DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = ખાલી
#define FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#નહીંતર
#define DECLARE_PKEY_PARAM_BIGNUM(name) અચલ BIGNUM *name
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
#ifndef LIBRESSL_VERSION_સંખ્યા
#define HAVE_SSL_CTX_SET1_SIGALGS
#endif

#ifdef LIBRESSL_VERSION_સંખ્યા
#define OSSL_PACKAGE "LibreSSL"
#અથવાજો defined(OPENSSL_IS_AWSLC)
#define OSSL_PACKAGE "AWS-LC"
#અથવાજો defined(OPENSSL_IS_BORINGSSL)
#define OSSL_PACKAGE "BoringSSL"
#અથવાજો defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#define OSSL_PACKAGE "quictls"
#નહીંતર
#define OSSL_PACKAGE "OpenSSL"
#endif

#ifdef HAVE_BORINGSSL_LIKE
પ્રકારવ્યાખ્યા size_t numcert_t;
પ્રકારવ્યાખ્યા uint32_t sslerr_t;
#નહીંતર
પ્રકારવ્યાખ્યા પૂર્ણાંક numcert_t;
પ્રકારવ્યાખ્યા unsigned long sslerr_t;
#endif
#define ossl_valsize_t numcert_t

#જો OPENSSL_VERSION_સંખ્યા >= 0x40100000L
#define ossl_ASN1_STRING_get_length ASN1_STRING_get_length
#નહીંતર
સ્થિર size_t ossl_ASN1_STRING_get_length(અચલ ASN1_STRING *str)
{
   અચલ પૂર્ણાંક length = ASN1_STRING_length(str);
   પાછા length >= 0 ? (size_t)length : 0;
}
#endif

સ્થિર CURLcode ધકેલો_certinfo(સંરચના Curl_easy *data,
                              BIO *mem, અચલ અક્ષર *label, પૂર્ણાંક અંક)
  WARN_UNUSED_RESULT;

સ્થિર CURLcode ધકેલો_certinfo(સંરચના Curl_easy *data,
                              BIO *mem, અચલ અક્ષર *label, પૂર્ણાંક અંક)
{
  અક્ષર *ptr;
  long len = BIO_get_mem_data(mem, &ptr);
  CURLcode result = Curl_ssl_ધકેલો_certinfo_len(data, અંક, label, ptr, len);
  (શૂન્ય)BIO_reset(mem);
  પાછા result;
}

સ્થિર CURLcode pubkey_show(સંરચના Curl_easy *data,
                            BIO *mem,
                            પૂર્ણાંક અંક,
                            અચલ અક્ષર *type,
                            અચલ અક્ષર *name,
                            અચલ BIGNUM *bn) WARN_UNUSED_RESULT;

સ્થિર CURLcode pubkey_show(સંરચના Curl_easy *data,
                            BIO *mem,
                            પૂર્ણાંક અંક,
                            અચલ અક્ષર *type,
                            અચલ અક્ષર *name,
                            અચલ BIGNUM *bn)
{
  અક્ષર namebuf[32];

  curl_msnprintf(namebuf, કદ(namebuf), "%s(%s)", type, name);

  જો(bn)
    BN_print(mem, bn);
  પાછા ધકેલો_certinfo(data, mem, namebuf, અંક);
}

#define print_pubkey_BN(_type, _name, અંક_2)           \
  pubkey_show(data, mem, અંક_2, #_type, #_name, _name)

સ્થિર પૂર્ણાંક asn1_object_dump(અચલ ASN1_OBJECT *a, અક્ષર *buf, size_t len)
{
  પૂર્ણાંક i = i2t_ASN1_OBJECT(buf, (પૂર્ણાંક)len, a);
  પાછા (i >= (પૂર્ણાંક)len);  /* buffer too small */
}

સ્થિર CURLcode X509V3_ext(સંરચના Curl_easy *data,
                           પૂર્ણાંક certnum,
                           અચલ ઢગલો_OF(X509_EXTENSION) *extsarg)
{
  પૂર્ણાંક i;
  CURLcode result = CURLE_OK;
#ifdef LIBRESSL_VERSION_સંખ્યા
  ઢગલો_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#નહીંતર
  અચલ ઢગલો_OF(X509_EXTENSION) *exts = extsarg;
#endif

  જો((પૂર્ણાંક)sk_X509_EXTENSION_અંક(exts) <= 0)
    /* no extensions, bail out */
    પાછા result;

  માટે(i = 0; i < (પૂર્ણાંક)sk_X509_EXTENSION_અંક(exts); i++) {
    અચલ ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_કિંમત(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    અક્ષર namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    જો(!bio_out)
      પાછા result;

    obj = X509_EXTENSION_get_object(ext);

    જો(asn1_object_dump(obj, namebuf, કદ(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    જો(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (અચલ ASN1_STRING *)X509_EXTENSION_get_data(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    result = Curl_ssl_ધકેલો_certinfo_len(data, certnum, namebuf, biomem->data,
                                        biomem->length);
    BIO_free(bio_out);
    જો(result)
      તોડો;
  }
  પાછા result;
}

સ્થિર CURLcode get_pkey_rsa(સંરચના Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, પૂર્ણાંક i)
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
#નહીંતર
  RSA_get0_ચાવી(rsa, &n, &e, ખાલી);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (પૂર્ણાંક)(n ? BN_અંક_bits(n) : 0));
  result = ધકેલો_certinfo(data, mem, "RSA Public Key", i);
  જો(!result) {
    result = print_pubkey_BN(rsa, n, i);
    જો(!result)
      result = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  પાછા result;
}

#ifndef OPENSSL_NO_DSA
સ્થિર CURLcode get_pkey_dsa(સંરચના Curl_easy *data,
                             EVP_PKEY *pubkey, BIO *mem, પૂર્ણાંક i)
{
  CURLcode result = CURLE_OK;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ચાવી);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ચાવી, &pub_ચાવી);
#નહીંતર
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_ચાવી(dsa, &pub_ચાવી, ખાલી);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dsa, p, i);
  જો(!result)
    result = print_pubkey_BN(dsa, q, i);
  જો(!result)
    result = print_pubkey_BN(dsa, g, i);
  જો(!result)
    result = print_pubkey_BN(dsa, pub_ચાવી, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ચાવી);
  પાછા result;
}
#endif /* !OPENSSL_NO_DSA */

સ્થિર CURLcode get_pkey_dh(સંરચના Curl_easy *data,
                            EVP_PKEY *pubkey, BIO *mem, પૂર્ણાંક i)
{
  CURLcode result;
#ifndef HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#endif /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_ચાવી);
#ifdef HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_ચાવી, &pub_ચાવી);
#નહીંતર
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_ચાવી(dh, &pub_ચાવી, ખાલી);
#endif /* HAVE_EVP_PKEY_GET_PARAMS */
  result = print_pubkey_BN(dh, p, i);
  જો(!result)
    result = print_pubkey_BN(dh, q, i);
  જો(!result)
    result = print_pubkey_BN(dh, g, i);
  જો(!result)
    result = print_pubkey_BN(dh, pub_ચાવી, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_ચાવી);
  પાછા result;
}

#ifdef HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
પ્રકારવ્યાખ્યા અચલ X509_PUBKEY pubkeytype_t;
#નહીંતર
પ્રકારવ્યાખ્યા X509_PUBKEY pubkeytype_t;
#endif

સ્થિર CURLcode ossl_certchain(સંરચના Curl_easy *data, SSL *ssl)
{
  CURLcode result;
  ઢગલો_OF(X509) *sk;
  પૂર્ણાંક i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  જો(!sk)
    પાછા CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_અંક(sk);
  જો(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(data, "%d certificates is more than allowed (%d)", (પૂર્ણાંક)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  result = Curl_ssl_init_certinfo(data, (પૂર્ણાંક)numcerts);
  જો(result)
    પાછા result;

  mem = BIO_new(BIO_s_mem());
  જો(!mem)
    result = CURLE_OUT_OF_MEMORY;

  માટે(i = 0; !result && (i < (પૂર્ણાંક)numcerts); i++) {
    X509 *x = sk_X509_કિંમત(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = ખાલી;
    size_t j;
    અચલ ASN1_BIT_STRING *psig = ખાલી;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    result = ધકેલો_certinfo(data, mem, "Subject", i);
    જો(result)
      તોડો;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    result = ધકેલો_certinfo(data, mem, "Issuer", i);
    જો(result)
      તોડો;

    BIO_printf(mem, "%lx", (unsigned long)X509_get_version(x));
    result = ધકેલો_certinfo(data, mem, "Version", i);
    જો(result)
      તોડો;

    {
      ASN1_INTEGER *અંક = X509_get_serial_સંખ્યા(x);
      અચલ unsigned અક્ષર *numdata = ASN1_STRING_get0_data(અંક);
      અચલ size_t numlen = ossl_ASN1_STRING_get_length(અંક);
      જો(ASN1_STRING_type(અંક) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      માટે(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    result = ધકેલો_certinfo(data, mem, "Serial Number", i);
    જો(result)
      તોડો;

    {
      અચલ X509_ALGOR *sigalg = ખાલી;
      pubkeytype_t *xpubkey = ખાલી;
      ASN1_OBJECT *pubkeyoid = ખાલી;

      X509_get0_signature(&psig, &sigalg, x);
      જો(sigalg) {
        અચલ ASN1_OBJECT *sigalgoid = ખાલી;
        X509_ALGOR_get0(&sigalgoid, ખાલી, ખાલી, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        result = ધકેલો_certinfo(data, mem, "Signature Algorithm", i);
        જો(result)
          તોડો;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      જો(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, ખાલી, ખાલી, ખાલી, xpubkey);
        જો(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          result = ધકેલો_certinfo(data, mem, "Public Key Algorithm", i);
          જો(result)
            તોડો;
        }
      }

      result = X509V3_ext(data, i, X509_get0_extensions(x));
      જો(result)
        તોડો;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    result = ધકેલો_certinfo(data, mem, "Start date", i);
    જો(result)
      તોડો;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    result = ધકેલો_certinfo(data, mem, "Expire date", i);
    જો(result)
      તોડો;

    pubkey = X509_get_pubkey(x);
    જો(!pubkey)
      infof(data, "   Unable to load public key");
    નહીંતર {
      પસંદગી(EVP_PKEY_id(pubkey)) {
      સ્થિતિ EVP_PKEY_RSA:
        result = get_pkey_rsa(data, pubkey, mem, i);
        તોડો;

#ifndef OPENSSL_NO_DSA
      સ્થિતિ EVP_PKEY_DSA:
        result = get_pkey_dsa(data, pubkey, mem, i);
        તોડો;
#endif

      સ્થિતિ EVP_PKEY_DH:
        result = get_pkey_dh(data, pubkey, mem, i);
        તોડો;
      }
      EVP_PKEY_free(pubkey);
    }

    જો(!result && psig) {
      અચલ unsigned અક્ષર *psigdata = ASN1_STRING_get0_data(psig);
      અચલ size_t psiglen = ossl_ASN1_STRING_get_length(psig);
      માટે(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      result = ધકેલો_certinfo(data, mem, "Signature", i);
    }

    જો(!result) {
      PEM_write_bio_X509(mem, x);
      result = ધકેલો_certinfo(data, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  જો(result)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(data);

  પાછા result;
}

સ્થિર પૂર્ણાંક ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_data(bio, ખાલી);
  પાછા 1;
}

સ્થિર પૂર્ણાંક ossl_bio_cf_destroy(BIO *bio)
{
  જો(!bio)
    પાછા 0;
  પાછા 1;
}

સ્થિર long ossl_bio_cf_ctrl(BIO *bio, પૂર્ણાંક cmd, long અંક, શૂન્ય *ptr)
{
  સંરચના Curl_cfilter *cf = BIO_get_data(bio);
  long ret = 1;

  (શૂન્ય)cf;
  (શૂન્ય)ptr;
  પસંદગી(cmd) {
  સ્થિતિ BIO_CTRL_GET_CLOSE:
    ret = (long)BIO_get_shutdown(bio);
    તોડો;
  સ્થિતિ BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (પૂર્ણાંક)અંક);
    તોડો;
  સ્થિતિ BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    તોડો;
  સ્થિતિ BIO_CTRL_DUP:
    ret = 1;
    તોડો;
  સ્થિતિ BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    સંરચના ssl_connect_data *connssl = cf->ctx;
    પાછા connssl->peer_closed;
  }
  મૂળભૂત:
    ret = 0;
    તોડો;
  }
  પાછા ret;
}

સ્થિર પૂર્ણાંક ossl_bio_cf_out_write(BIO *bio, અચલ અક્ષર *buf, પૂર્ણાંક blen)
{
  સંરચના Curl_cfilter *cf = BIO_get_data(bio);
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  જો(blen < 0)
    પાછા 0;

  result = Curl_conn_cf_send(cf->next, data,
                             (અચલ uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (પૂર્ણાંક)result, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  જો(result) {
    જો(result == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    પાછા -1;
  }
  પાછા (પૂર્ણાંક)nwritten;
}

સ્થિર પૂર્ણાંક ossl_bio_cf_in_read(BIO *bio, અક્ષર *buf, પૂર્ણાંક blen)
{
  સંરચના Curl_cfilter *cf = BIO_get_data(bio);
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result, r2;

  DEBUGASSERT(data);
  /* OpenSSL catches this case, so should we. */
  જો(!buf)
    પાછા 0;
  જો(blen < 0)
    પાછા 0;

  result = Curl_conn_cf_recv(cf->next, data, buf, (size_t)blen, &nread);
  CURL_TRC_CF(data, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (પૂર્ણાંક)result, nread);
  BIO_clear_retry_flags(bio);
  octx->io_result = result;
  જો(result) {
    જો(result == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  નહીંતર {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->આગત_pending = TRUE;
    જો(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  જો(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, data, octx);
    જો(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_result = r2;
      પાછા -1;
    }
    octx->x509_store_setup = TRUE;
  }
  પાછા result ? -1 : (પૂર્ણાંક)nread;
}

સ્થિર BIO_METHOD *ossl_bio_cf_method_create(શૂન્ય)
{
  BIO_METHOD *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  જો(m) {
    BIO_meth_set_write(m, &ossl_bio_cf_out_write);
    BIO_meth_set_read(m, &ossl_bio_cf_in_read);
    BIO_meth_set_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_set_create(m, &ossl_bio_cf_create);
    BIO_meth_set_destroy(m, &ossl_bio_cf_destroy);
  }
  પાછા m;
}

સ્થિર શૂન્ય ossl_bio_cf_method_free(BIO_METHOD *m)
{
  જો(m)
    BIO_meth_free(m);
}

#ifndef HAVE_KEYLOG_UPSTREAM
#ifdef HAVE_KEYLOG_CALLBACK
સ્થિર શૂન્ય ossl_keylog_callback(અચલ SSL *ssl, અચલ અક્ષર *લીટી)
{
  (શૂન્ય)ssl;

  Curl_tls_keylog_write_લીટી(લીટી);
}
#નહીંતર
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
સ્થિર શૂન્ય ossl_log_tls12_secret(અચલ SSL *ssl, bool *keylog_done)
{
  અચલ SSL_SESSION *session;
  unsigned અક્ષર client_random[SSL3_RANDOM_SIZE];
  unsigned અક્ષર master_ચાવી[SSL_MAX_MASTER_ચાવી_LENGTH];
  પૂર્ણાંક master_ચાવી_length = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  જો(!session || *keylog_done) {
    ERR_કાઢો_to_mark();
    પાછા;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_SIZE);
  master_ચાવી_length = (પૂર્ણાંક)
    SSL_SESSION_get_master_ચાવી(session, master_ચાવી, SSL_MAX_MASTER_ચાવી_LENGTH);

  ERR_કાઢો_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  જો(master_ચાવી_length <= 0)
    પાછા;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        કદ(client_random),
                        master_ચાવી, master_ચાવી_length);
}
#endif /* !HAVE_KEYLOG_CALLBACK */
#endif /* HAVE_KEYLOG_UPSTREAM */

સ્થિર અચલ અક્ષર *SSL_ERROR_to_str(પૂર્ણાંક err)
{
  પસંદગી(err) {
  સ્થિતિ SSL_ERROR_NONE:
    પાછા "SSL_ERROR_NONE";
  સ્થિતિ SSL_ERROR_SSL:
    પાછા "SSL_ERROR_SSL";
  સ્થિતિ SSL_ERROR_WANT_READ:
    પાછા "SSL_ERROR_WANT_READ";
  સ્થિતિ SSL_ERROR_WANT_WRITE:
    પાછા "SSL_ERROR_WANT_WRITE";
  સ્થિતિ SSL_ERROR_WANT_X509_LOOKUP:
    પાછા "SSL_ERROR_WANT_X509_LOOKUP";
  સ્થિતિ SSL_ERROR_SYSCALL:
    પાછા "SSL_ERROR_SYSCALL";
  સ્થિતિ SSL_ERROR_ZERO_RETURN:
    પાછા "SSL_ERROR_ZERO_RETURN";
  સ્થિતિ SSL_ERROR_WANT_CONNECT:
    પાછા "SSL_ERROR_WANT_CONNECT";
  સ્થિતિ SSL_ERROR_WANT_ACCEPT:
    પાછા "SSL_ERROR_WANT_ACCEPT";
#ifdef SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  સ્થિતિ SSL_ERROR_WANT_ASYNC:
    પાછા "SSL_ERROR_WANT_ASYNC";
#endif
#ifdef SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  સ્થિતિ SSL_ERROR_WANT_ASYNC_JOB:
    પાછા "SSL_ERROR_WANT_ASYNC_JOB";
#endif
#ifdef SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  સ્થિતિ SSL_ERROR_WANT_CLIENT_HELLO_CB:
    પાછા "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#endif
  મૂળભૂત:
    પાછા "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
સ્થિર અક્ષર *ossl_strerror(unsigned long error, અક્ષર *buf, size_t size)
{
  size_t len;
  DEBUGASSERT(size);
  *buf = '\0';

  len = Curl_ossl_version(buf, size);
  DEBUGASSERT(len < (size - 2));
  જો(len < (size - 2)) {
    buf += len;
    size -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#ifdef HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, size);
#નહીંતર
  ERR_error_string_n(error, buf, size);
#endif

  જો(!*buf) {
    અચલ અક્ષર *સંદેશ = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, size, સંદેશ, strlen(સંદેશ));
  }

  પાછા buf;
}

સ્થિર પૂર્ણાંક passwd_callback(અક્ષર *buf, પૂર્ણાંક અંક, પૂર્ણાંક encrypting, શૂન્ય *password)
{
  DEBUGASSERT(encrypting == 0);

  જો(!encrypting && અંક >= 0 && password) {
    પૂર્ણાંક klen = curlx_uztosi(strlen((અક્ષર *)password));
    જો(અંક > klen) {
      memcpy(buf, password, klen + 1);
      પાછા klen;
    }
  }
  પાછા 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
સ્થિર bool rand_enough(શૂન્ય)
{
  પાછા RAND_status() != 0;
}

સ્થિર CURLcode ossl_seed(સંરચના Curl_easy *data)
{
  /* This might get called before it has been added to a multi handle */
  જો(data->multi && data->multi->ssl_seeded)
    પાછા CURLE_OK;

  જો(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    જો(data->multi)
      data->multi->ssl_seeded = TRUE;
    પાછા CURLE_OK;
  }
  failf(data, "Insufficient randomness");
  પાછા CURLE_SSL_CONNECT_ERROR;
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
સ્થિર પૂર્ણાંક ossl_do_file_type(અચલ અક્ષર *type)
{
  જો(!type || !type[0])
    પાછા SSL_FILETYPE_PEM;
  જો(curl_strequal(type, "PEM"))
    પાછા SSL_FILETYPE_PEM;
  જો(curl_strequal(type, "DER"))
    પાછા SSL_FILETYPE_ASN1;
  જો(curl_strequal(type, "PROV"))
    પાછા SSL_FILETYPE_PROVIDER;
  જો(curl_strequal(type, "ENG"))
    પાછા SSL_FILETYPE_ENGINE;
  જો(curl_strequal(type, "P12"))
    પાછા SSL_FILETYPE_PKCS12;
  પાછા -1;
}

#જો defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
સ્થિર પૂર્ણાંક ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  અચલ અક્ષર *password;
  પસંદગી(UI_get_string_type(uis)) {
  સ્થિતિ UIT_PROMPT:
  સ્થિતિ UIT_VERIFY:
    password = (અચલ અક્ષર *)UI_get0_user_data(ui);
    જો(password && (UI_get_આગત_flags(uis) & UI_આગત_FLAG_DEFAULT_PWD)) {
      UI_set_result(ui, uis, password);
      પાછા 1;
    }
    FALLTHROUGH();
  મૂળભૂત:
    તોડો;
  }
  પાછા UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
સ્થિર પૂર્ણાંક ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  પસંદગી(UI_get_string_type(uis)) {
  સ્થિતિ UIT_PROMPT:
  સ્થિતિ UIT_VERIFY:
    જો(UI_get0_user_data(ui) &&
       (UI_get_આગત_flags(uis) & UI_આગત_FLAG_DEFAULT_PWD)) {
      પાછા 1;
    }
    FALLTHROUGH();
  મૂળભૂત:
    તોડો;
  }
  પાછા UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
સ્થિર bool is_pkcs11_uri(અચલ અક્ષર *string)
{
  પાછા string && curl_strnequal(string, "pkcs11:", 7);
}

#endif

સ્થિર CURLcode ossl_set_engine(સંરચના Curl_easy *data, સંરચના ossl_ctx *octx,
                                અચલ અક્ષર *name);
#ifdef OPENSSL_HAS_PROVIDERS
સ્થિર CURLcode ossl_set_provider(સંરચના Curl_easy *data,
                                  સંરચના ossl_ctx *octx,
                                  અચલ અક્ષર *iname);
#endif

સ્થિર પૂર્ણાંક use_certificate_blob(SSL_CTX *ctx, અચલ સંરચના curl_blob *blob,
                                પૂર્ણાંક type, અચલ અક્ષર *ચાવી_passwd)
{
  પૂર્ણાંક ret = 0;
  X509 *x = ખાલી;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->data, (પૂર્ણાંક)blob->len);
  જો(!in)
    પાછા CURLE_OUT_OF_MEMORY;

  જો(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, ખાલી);
  }
  નહીંતર જો(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, ખાલી, passwd_callback, CURL_UNCONST(ચાવી_passwd));
  }
  નહીંતર {
    ret = 0;
    જાઓ end;
  }

  જો(!x) {
    ret = 0;
    જાઓ end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  પાછા ret;
}

સ્થિર પૂર્ણાંક use_privatekey_blob(SSL_CTX *ctx, અચલ સંરચના curl_blob *blob,
                               પૂર્ણાંક type, અચલ અક્ષર *ચાવી_passwd)
{
  પૂર્ણાંક ret = 0;
  EVP_PKEY *pkey = ખાલી;
  BIO *in = BIO_new_mem_buf(blob->data, (પૂર્ણાંક)blob->len);
  જો(!in)
    પાછા CURLE_OUT_OF_MEMORY;

  જો(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_Private_ચાવી(in, ખાલી, passwd_callback,
                                   CURL_UNCONST(ચાવી_passwd));
  નહીંતર જો(type == SSL_FILETYPE_ASN1)
    pkey = d2i_Private_ચાવી_bio(in, ખાલી);
  નહીંતર
    જાઓ end;

  જો(!pkey)
    જાઓ end;

  ret = SSL_CTX_use_Private_ચાવી(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  પાછા ret;
}

સ્થિર પૂર્ણાંક use_certificate_chain_blob(SSL_CTX *ctx,
                                      અચલ સંરચના curl_blob *blob,
                                      અચલ અક્ષર *ચાવી_passwd)
{
  પૂર્ણાંક ret = 0;
  X509 *x = ખાલી;
  BIO *in = BIO_new_mem_buf(blob->data, (પૂર્ણાંક)blob->len);
  જો(!in)
    પાછા CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, ખાલી,
                            passwd_callback, CURL_UNCONST(ચાવી_passwd));
  જો(!x)
    જાઓ end;

  ret = SSL_CTX_use_certificate(ctx, x);

  જો(ERR_peek_error() != 0)
    ret = 0;

  જો(ret) {
    X509 *ca;
    sslerr_t err;

    જો(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      જાઓ end;
    }

    જ્યારે((ca = PEM_read_bio_X509(in, ખાલી, passwd_callback,
                                  CURL_UNCONST(ચાવી_passwd))) != ખાલી) {

      જો(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        જાઓ end;
      }
    }

    err = ERR_peek_last_error();
    જો((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_લીટી))
      ERR_clear_error();
    નહીંતર
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  પાછા ret;
}

સ્થિર પૂર્ણાંક enginecheck(સંરચના Curl_easy *data,
                       સંરચના ossl_ctx *octx,
                       અચલ અક્ષર *ચાવી_file,
                       અચલ અક્ષર *ચાવી_passwd)
{
#ifdef USE_OPENSSL_ENGINE
  EVP_PKEY *priv_ચાવી = ખાલી;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  જો(!octx->p.engine && is_pkcs11_uri(ચાવી_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    પાછા 0;

  જો(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    જો(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      પાછા 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);
    priv_ચાવી = ENGINE_load_private_ચાવી(octx->p.engine, ચાવી_file,
                                       ui_method,
                                       CURL_UNCONST(ચાવી_passwd));
    UI_destroy_method(ui_method);
    જો(!priv_ચાવી) {
      failf(data, "failed to load private key from crypto engine");
      પાછા 0;
    }
    જો(SSL_CTX_use_Private_ચાવી(ctx, priv_ચાવી) != 1) {
      failf(data, "unable to set private key");
      EVP_PKEY_free(priv_ચાવી);
      પાછા 0;
    }
    EVP_PKEY_free(priv_ચાવી);  /* we do not need the handle any more... */
  }
  નહીંતર {
    failf(data, "crypto engine not set, cannot load private key");
    પાછા 0;
  }
  પાછા 1;
#નહીંતર
  (શૂન્ય)octx;
  (શૂન્ય)ચાવી_file;
  (શૂન્ય)ચાવી_passwd;
  failf(data, "SSL_FILETYPE_ENGINE not supported for private key");
  પાછા 0;
#endif
}

સ્થિર પૂર્ણાંક providercheck(સંરચના Curl_easy *data,
                         સંરચના ossl_ctx *octx,
                         અચલ અક્ષર *ચાવી_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  અક્ષર error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  જો(!octx->p.provider_loaded && is_pkcs11_uri(ચાવી_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK) {
    પાછા 0;
  }

  જો(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_ચાવી = ખાલી;
    OSSL_STORE_CTX *store = ખાલી;
    OSSL_STORE_INFO *info = ખાલી;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    જો(!ui_method) {
      failf(data, "unable to create " OSSL_PACKAGE " user-interface method");
      પાછા 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(ચાવી_file, octx->p.libctx,
                               octx->p.propq, ui_method, ખાલી, ખાલી,
                               ખાલી, ખાલી);
    જો(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      UI_destroy_method(ui_method);
      પાછા 0;
    }
    જો(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    જો(info) {
      પૂર્ણાંક ossl_type = OSSL_STORE_INFO_get_type(info);

      જો(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_ચાવી = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    જો(!priv_ચાવી) {
      failf(data, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      પાછા 0;
    }

    જો(SSL_CTX_use_Private_ચાવી(ctx, priv_ચાવી) != 1) {
      failf(data, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      EVP_PKEY_free(priv_ચાવી);
      પાછા 0;
    }
    EVP_PKEY_free(priv_ચાવી); /* we do not need the handle any more... */
  }
  નહીંતર {
    failf(data, "crypto provider not set, cannot load private key");
    પાછા 0;
  }
  પાછા 1;
#નહીંતર
  (શૂન્ય)octx;
  (શૂન્ય)ચાવી_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for private key");
  પાછા 0;
#endif
}

સ્થિર પૂર્ણાંક engineload(સંરચના Curl_easy *data,
                      સંરચના ossl_ctx *octx,
                      અચલ અક્ષર *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#જો defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  અક્ષર error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  જો(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(data, octx, "pkcs11") != CURLE_OK)
    પાછા 0;

  જો(octx->p.engine) {
    સ્થિર અચલ અક્ષર cmd_name[] = "LOAD_CERT_CTRL";
    સંરચના {
      અચલ અક્ષર *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = ખાલી;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    જો(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), ખાલી)) {
      failf(data, "SSL engine does not support loading certificates");
      પાછા 0;
    }

    /* Load the certificate from the engine */
    જો(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, ખાલી, 1)) {
      failf(data, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      પાછા 0;
    }

    જો(!params.cert) {
      failf(data, "SSL engine did not initialize the certificate properly.");
      પાછા 0;
    }

    જો(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      X509_free(params.cert);
      પાછા 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  નહીંતર {
    failf(data, "crypto engine not set, cannot load certificate");
    પાછા 0;
  }
  પાછા 1;
#નહીંતર
  (શૂન્ય)octx;
  (શૂન્ય)cert_file;
  failf(data, "SSL_FILETYPE_ENGINE not supported for certificate");
  પાછા 0;
#endif
}

સ્થિર પૂર્ણાંક providerload(સંરચના Curl_easy *data,
                        સંરચના ossl_ctx *octx,
                        અચલ અક્ષર *cert_file)
{
#ifdef OPENSSL_HAS_PROVIDERS
  અક્ષર error_buffer[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  જો(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(data, octx, "pkcs11") != CURLE_OK)
    પાછા 0;

  જો(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = ખાલી;
    X509 *cert = ખાલી;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         ખાલી, ખાલી, ખાલી, ખાલી, ખાલી, ખાલી);
    પૂર્ણાંક rc;

    જો(!store) {
      failf(data, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      પાછા 0;
    }
    જો(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(data, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
    }

    info = OSSL_STORE_load(store);
    જો(info) {
      પૂર્ણાંક ossl_type = OSSL_STORE_INFO_get_type(info);

      જો(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    જો(!cert) {
      failf(data, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      પાછા 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    જો(rc != 1) {
      failf(data, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      પાછા 0;
    }
  }
  નહીંતર {
    failf(data, "crypto provider not set, cannot load certificate");
    પાછા 0;
  }
  પાછા 1;
#નહીંતર
  (શૂન્ય)octx;
  (શૂન્ય)cert_file;
  failf(data, "SSL_FILETYPE_PROVIDER not supported for certificate");
  પાછા 0;
#endif
}

સ્થિર પૂર્ણાંક pkcs12load(સંરચના Curl_easy *data,
                      SSL_CTX* ctx,
                      અચલ સંરચના curl_blob *cert_blob,
                      અચલ અક્ષર *cert_file,
                      અચલ અક્ષર *ચાવી_passwd)
{
  અક્ષર error_buffer[256];
  BIO *cert_bio = ખાલી;
  PKCS12 *p12 = ખાલી;
  EVP_PKEY *pri;
  X509 *x509;
  પૂર્ણાંક cert_done = 0;
  ઢગલો_OF(X509) *ca = ખાલી;
  જો(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->data, (પૂર્ણાંક)cert_blob->len);
    જો(!cert_bio) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      પાછા 0;
    }
  }
  નહીંતર {
    cert_bio = BIO_new(BIO_s_file());
    જો(!cert_bio) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
      પાછા 0;
    }

    જો(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(data, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      પાછા 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, ખાલી);
  BIO_free(cert_bio);

  જો(!p12) {
    failf(data, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    પાછા 0;
  }

  જો(!PKCS12_parse(p12, ચાવી_passwd, &pri, &x509, &ca)) {
    failf(data, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, કદ(error_buffer)));
    PKCS12_free(p12);
    પાછા 0;
  }

  PKCS12_free(p12);

  જો(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(data, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_buffer, કદ(error_buffer)));
    જાઓ fail;
  }

  જો(SSL_CTX_use_Private_ચાવી(ctx, pri) != 1) {
    failf(data, "unable to use private key from PKCS12 file '%s'", cert_file);
    જાઓ fail;
  }

  જો(!SSL_CTX_check_private_ચાવી(ctx)) {
    failf(data, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    જાઓ fail;
  }
  /* Set Certificate Verification chain */
  જો(ca) {
    જ્યારે(sk_X509_અંક(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_કાઢો(ca);
      જો(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to client CA list");
        જાઓ fail;
      }
      જો(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(data, "cannot add certificate to certificate chain");
        જાઓ fail;
      }
    }
  }

  cert_done = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_કાઢો_free(ca, X509_free);
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic કાઢો
#endif
  જો(!cert_done)
    પાછા 0; /* failure! */
  પાછા 1;
}

સ્થિર CURLcode client_cert(સંરચના Curl_easy *data,
                            સંરચના ossl_ctx *octx,
                            અક્ષર *cert_file,
                            અચલ સંરચના curl_blob *cert_blob,
                            અચલ અક્ષર *cert_type,
                            અક્ષર *ચાવી_file,
                            અચલ સંરચના curl_blob *ચાવી_blob,
                            અચલ અક્ષર *ચાવી_type,
                            અક્ષર *ચાવી_passwd)
{
  અક્ષર error_buffer[256];
  bool check_privkey = TRUE;
  પૂર્ણાંક file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  જો(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    પૂર્ણાંક cert_use_result;

    જો(ચાવી_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, ચાવી_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    પસંદગી(file_type) {
    સ્થિતિ SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_result = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, ચાવી_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      જો(cert_use_result != 1) {
        failf(data,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            કદ(error_buffer)));
        પાછા CURLE_SSL_CERTPROBLEM;
      }
      તોડો;

    સ્થિતિ SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_result = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, ચાવી_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      જો(cert_use_result != 1) {
        failf(data,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_buffer,
                            કદ(error_buffer)));
        પાછા CURLE_SSL_CERTPROBLEM;
      }
      તોડો;

    સ્થિતિ SSL_FILETYPE_ENGINE:
      જો(!cert_file || !engineload(data, octx, cert_file))
        પાછા CURLE_SSL_CERTPROBLEM;
      તોડો;

    સ્થિતિ SSL_FILETYPE_PROVIDER:
      જો(!cert_file || !providerload(data, octx, cert_file))
        પાછા CURLE_SSL_CERTPROBLEM;
      તોડો;

    સ્થિતિ SSL_FILETYPE_PKCS12:
      જો(!pkcs12load(data, ctx, cert_blob, cert_file, ચાવી_passwd))
        પાછા CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      તોડો;

    મૂળભૂત:
      failf(data, "not supported file type '%s' for certificate", cert_type);
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }

    જો(!ચાવી_file && !ચાવી_blob) {
      ચાવી_file = cert_file;
      ચાવી_blob = cert_blob;
    }
    નહીંતર
      file_type = ossl_do_file_type(ચાવી_type);

    પસંદગી(file_type) {
    સ્થિતિ SSL_FILETYPE_PEM:
    સ્થિતિ SSL_FILETYPE_ASN1:
      cert_use_result = ચાવી_blob ?
        use_privatekey_blob(ctx, ચાવી_blob, file_type, ચાવી_passwd) :
      SSL_CTX_use_Private_ચાવી_file(ctx, ચાવી_file, file_type);
      જો(cert_use_result != 1) {
        failf(data, "unable to set private key file: '%s' type %s",
              ચાવી_file ? ચાવી_file : "(memory blob)",
              ચાવી_type ? ચાવી_type : "PEM");
        પાછા CURLE_BAD_FUNCTION_ARGUMENT;
      }
      તોડો;
    સ્થિતિ SSL_FILETYPE_ENGINE:
      જો(!enginecheck(data, octx, ચાવી_file, ચાવી_passwd))
        પાછા CURLE_SSL_CERTPROBLEM;
      તોડો;

    સ્થિતિ SSL_FILETYPE_PROVIDER:
      જો(!providercheck(data, octx, ચાવી_file))
        પાછા CURLE_SSL_CERTPROBLEM;
      તોડો;

    સ્થિતિ SSL_FILETYPE_PKCS12:
      જો(!pcks12_done) {
        failf(data, "file type P12 for private key not supported");
        પાછા CURLE_SSL_CERTPROBLEM;
      }
      તોડો;
    મૂળભૂત:
      failf(data, "not supported file type for private key");
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    જો(!ssl) {
      failf(data, "unable to create an SSL structure");
      પાછા CURLE_OUT_OF_MEMORY;
    }

    x509 = SSL_get_certificate(ssl);

    જો(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#જો !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_ચાવી = SSL_get_privatekey(ssl);
      જો(EVP_PKEY_id(priv_ચાવી) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_ચાવી);
        જો(RSA_flags(rsa) & RSA_METHOD_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#endif

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    જો(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      જો(!SSL_CTX_check_private_ચાવી(ctx)) {
        failf(data, "Private key does not match the certificate public key");
        પાછા CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  પાછા CURLE_OK;
}

#ifdef CURLVERBOSE
/* returns non-zero on failure */
સ્થિર CURLcode x509_name_oneline(અચલ X509_NAME *a, સંરચના dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  પૂર્ણાંક rc;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  જો(bio_out) {
    unsigned long flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    જો(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      result = curlx_dyn_addn(d, biomem->data, biomem->length);
    }
    BIO_free(bio_out);
  }
  પાછા result;
}
#endif

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
સ્થિર પૂર્ણાંક ossl_init(શૂન્ય)
{
  અચલ uint64_t flags =
#ifdef OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#endif
#ifdef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    OPENSSL_INIT_NO_LOAD_CONFIG |
#નહીંતર
    OPENSSL_INIT_LOAD_CONFIG |
#endif
    0;
  OPENSSL_init_ssl(flags, ખાલી);

#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#endif

  પાછા 1;
}

/* Global cleanup */
સ્થિર શૂન્ય ossl_cleanup(શૂન્ય)
{
#ifndef HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#endif
}

/* Selects an OpenSSL crypto engine or provider.
 */
સ્થિર CURLcode ossl_set_engine(સંરચના Curl_easy *data, સંરચના ossl_ctx *octx,
                                અચલ અક્ષર *name)
{
#ifdef USE_OPENSSL_ENGINE
  CURLcode result = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  જો(e) {

    જો(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = ખાલી;
    }
    જો(!ENGINE_init(e)) {
      અક્ષર buf[256];

      ENGINE_free(e);
      failf(data, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, કદ(buf)));
      result = CURLE_SSL_ENGINE_INITFAILED;
      e = ખાલી;
    }
    નહીંતર {
      result = CURLE_OK;
    }
    octx->p.engine = e;
    પાછા result;
  }
#endif
#ifdef OPENSSL_HAS_PROVIDERS
  પાછા ossl_set_provider(data, octx, name);
#નહીંતર
  (શૂન્ય)octx;
  (શૂન્ય)name;
  failf(data, "OpenSSL engine not found");
  પાછા CURLE_SSL_ENGINE_NOTFOUND;
#endif
}

/* Sets engine as default for all SSL operations
 */
સ્થિર CURLcode ossl_set_engine_default(સંરચના Curl_easy *data,
                                        સંરચના ossl_ctx *octx)
{
#ifdef USE_OPENSSL_ENGINE
  જો(octx->p.engine) {
    જો(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(data, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    નહીંતર {
      failf(data, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      પાછા CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#નહીંતર
  (શૂન્ય)data;
  (શૂન્ય)octx;
#endif
  પાછા CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
સ્થિર સંરચના curl_slist *ossl_engines_list(સંરચના Curl_easy *data)
{
  સંરચના curl_slist *list = ખાલી;
#ifdef USE_OPENSSL_ENGINE
  સંરચના curl_slist *beg;
  ENGINE *e;

  માટે(e = ENGINE_get_first(); e; e = ENGINE_get_next(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    જો(!beg) {
      curl_slist_free_all(list);
      પાછા ખાલી;
    }
    list = beg;
  }
#endif
  (શૂન્ય)data;
  પાછા list;
}

#ifdef OPENSSL_HAS_PROVIDERS

સ્થિર શૂન્ય ossl_provider_cleanup(સંરચના ossl_ctx *octx)
{
  જો(!octx)
    પાછા;
  જો(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = ખાલી;
  }
  જો(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = ખાલી;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = ખાલી;
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
સ્થિર CURLcode ossl_set_provider(સંરચના Curl_easy *data,
                                  સંરચના ossl_ctx *octx,
                                  અચલ અક્ષર *iname)
{
  અક્ષર name[MAX_PROVIDER_LEN + 1];
  સંરચના Curl_str prov;
  અચલ અક્ષર *propq = ખાલી;

  જો(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    પાછા CURLE_OK;
  }
  જો(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  memcpy(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  જો(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    જો(!libctx)
      પાછા CURLE_OUT_OF_MEMORY;
    જો(propq) {
      octx->p.propq = curlx_strdup(propq);
      જો(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        પાછા CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#ifndef CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  જો(!OSSL_LIB_CTX_load_config(octx->p.libctx, ખાલી)) {
    infof(data, "Failed to load default openssl config. Proceeding.");
  }
#endif

  જો(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    પાછા CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  જો(!octx->p.provider) {
    અક્ષર error_buffer[256];
    failf(data, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_buffer, કદ(error_buffer)));
    ossl_provider_cleanup(octx);
    પાછા CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  જો(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(data, "Failed to load base");
    પાછા CURLE_SSL_ENGINE_NOTFOUND;
  }
  નહીંતર
    octx->p.provider_loaded = TRUE;
  પાછા CURLE_OK;
}
#નહીંતર /* OPENSSL_HAS_PROVIDERS */
#define ossl_provider_cleanup(x)
#endif

સ્થિર CURLcode ossl_shutdown(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  અક્ષર buf[1024];
  પૂર્ણાંક nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  જો(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    જાઓ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  જો(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    માટે(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (પૂર્ણાંક)કદ(buf));
      CURL_TRC_CF(data, cf, "SSL shutdown not sent, read -> %d", nread);
      જો(nread <= 0)
        તોડો;
    }
    err = SSL_get_error(octx->ssl, nread);
    જો(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool આગત_pending;
      /* Yes, it did. */
      જો(!send_shutdown) {
        CURL_TRC_CF(data, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        જાઓ out;
      }
      નહીંતર જો(!cf->next->cft->is_alive(cf->next, data, &આગત_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(data, cf, "peer closed connection");
        *done = TRUE;
        જાઓ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  જો(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    પૂર્ણાંક rc;
    ERR_clear_error();
    CURL_TRC_CF(data, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    જો(rc == 1) {
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
      *done = TRUE;
      જાઓ out;
    }
    જો(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      જાઓ out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  માટે(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (પૂર્ણાંક)કદ(buf));
    CURL_TRC_CF(data, cf, "SSL shutdown read -> %d", nread);
    જો(nread <= 0)
      તોડો;
  }
  err = SSL_get_error(octx->ssl, nread);
  પસંદગી(err) {
  સ્થિતિ SSL_ERROR_ZERO_RETURN: /* no more data */
    જો(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(data, cf, "SSL shutdown finished");
    નહીંતર
      CURL_TRC_CF(data, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    તોડો;
  સ્થિતિ SSL_ERROR_NONE: /* did not get anything */
  સ્થિતિ SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(data, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    તોડો;
  સ્થિતિ SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(data, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    તોડો;
  મૂળભૂત:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(unsigned long sslerr = ERR_get_error());
      CURL_TRC_CF(data, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, કદ(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    result = CURLE_OK;
    તોડો;
  }

out:
  cf->shutdown = (result || *done);
  જો(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->આગત_pending = FALSE;
  પાછા result;
}

/* also used by quic-tls */
શૂન્ય Curl_osslctx_cleanup(સંરચના ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  જો(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = ખાલી;
  }
  જો(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = ખાલી;
    octx->x509_store_setup = FALSE;
  }
  જો(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = ખાલી;
  }
#ifdef USE_OPENSSL_ENGINE
  જો(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = ખાલી;
  }
#endif
  ossl_provider_cleanup(octx);
}

સ્થિર શૂન્ય ossl_close(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  (શૂન્ય)data;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
સ્થિર શૂન્ય ossl_close_all(સંરચના Curl_easy *data)
{
  (શૂન્ય)data;
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
સ્થિર CURLcode ossl_verifyhost(સંરચના Curl_easy *data,
                                સંરચના connectdata *conn,
                                સંરચના ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  પૂર્ણાંક target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  ઢગલો_OF(GENERAL_NAME) *altnames;
#ifdef USE_IPV6
  સંરચના in6_addr addr;
#નહીંતર
  સંરચના in_addr addr;
#endif
  CURLcode result = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = strlen(peer->origin->hostname);

  (શૂન્ય)conn;
  પસંદગી(peer->type) {
  સ્થિતિ CURL_SSL_PEER_IPV4:
    જો(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      પાછા CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = કદ(સંરચના in_addr);
    તોડો;
#ifdef USE_IPV6
  સ્થિતિ CURL_SSL_PEER_IPV6:
    જો(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      પાછા CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = કદ(સંરચના in6_addr);
    તોડો;
#endif
  સ્થિતિ CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    તોડો;
  મૂળભૂત:
    DEBUGASSERT(0);
    failf(data, "unexpected SSL peer type: %d", (પૂર્ણાંક)peer->type);
    પાછા CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, ખાલી, ખાલી);

  જો(altnames) {
#ifdef HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#નહીંતર
    પૂર્ણાંક numalts;
    પૂર્ણાંક i;
#endif

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_અંક(altnames);

    /* loop through all alternatives - until a dnsmatch */
    માટે(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      અચલ GENERAL_NAME *check = sk_GENERAL_NAME_કિંમત(altnames, i);

      જો(check->type == GEN_DNS)
        dNSName = TRUE;
      નહીંતર જો(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      જો(check->type == target) {
        /* get data and length */
        અચલ અક્ષર *altptr = (અચલ અક્ષર *)ASN1_STRING_get0_data(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_length(check->d.ia5);

        પસંદગી(target) {
        સ્થિતિ GEN_DNS: /* name/pattern comparison */
          જો(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (પૂર્ણાંક)altlen, altptr);
          }
          તોડો;

        સ્થિતિ GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          જો((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(data, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          તોડો;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  જો(matched)
    /* an alternative name matched */
    ;
  નહીંતર જો(dNSName || iPAddress) {
    અચલ અક્ષર *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(data, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(data, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    result = CURLE_PEER_FAILED_VERIFICATION;
  }
  નહીંતર {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    પૂર્ણાંક i = -1;
    unsigned અક્ષર *cn = ખાલી;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    અચલ X509_NAME *name = X509_get_subject_name(server_cert);
    જો(name) {
      પૂર્ણાંક j;
      જ્યારે((j = X509_NAME_get_index_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    જો(i >= 0) {
      અચલ ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      જો(tmp) {
        જો(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_length(tmp);
          cn = (unsigned અક્ષર *)CURL_UNCONST(ASN1_STRING_get0_data(tmp));
        }
        નહીંતર { /* not a UTF8 name */
          અચલ પૂર્ણાંક utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          જો(utf8len < 0)
            cnlen = 0;
          નહીંતર
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        જો((cnlen == 0) || !cn)
          result = CURLE_OUT_OF_MEMORY;
        નહીંતર જો(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(data, "SSL: illegal cert name field");
          result = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    જો(result)
      /* error already detected, pass through */
      ;
    નહીંતર જો(!cn) {
      failf(data, "SSL: unable to obtain common name from peer certificate");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    નહીંતર જો(!Curl_cert_hostcheck((અચલ અક્ષર *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(data, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (પૂર્ણાંક)cnlen, cn,
            peer->origin->user_hostname);
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    નહીંતર {
      infof(data, " common name: %.*s (matched)", (પૂર્ણાંક)cnlen, cn);
    }
    જો(free_cn)
      OPENSSL_free(cn);
  }

  પાછા result;
}

#ifndef OPENSSL_NO_OCSP
સ્થિર CURLcode verifystatus(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             સંરચના ossl_ctx *octx)
{
  પૂર્ણાંક i, ocsp_status;
#ifdef HAVE_BORINGSSL_LIKE
  અચલ uint8_t *status;
#નહીંતર
  unsigned અક્ષર *status;
#endif
  અચલ unsigned અક્ષર *p;
  CURLcode result = CURLE_OK;
  OCSP_RESPONSE *rsp = ખાલી;
  OCSP_BASICRESP *br = ખાલી;
  X509_STORE     *st = ખાલી;
  ઢગલો_OF(X509) *ch = ખાલી;
  X509 *cert;
  OCSP_CERTID *id = ખાલી;
  પૂર્ણાંક cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  પૂર્ણાંક ret;
  long len;

  (શૂન્ય)cf;
  DEBUGASSERT(octx);

  len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  જો(!status) {
    failf(data, "No OCSP response received");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(ખાલી, &p, len);
  જો(!rsp) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }

  ocsp_status = OCSP_response_status(rsp);
  જો(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(data, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }

  br = OCSP_response_get1_basic(rsp);
  જો(!br) {
    failf(data, "Invalid OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }

#જો !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_સંખ્યા) && LIBRESSL_VERSION_સંખ્યા < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#નહીંતર
  ch = SSL_get_peer_cert_chain(octx->ssl);
#endif
  જો(!ch) {
    failf(data, "Could not get peer certificate chain");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  જો(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(data, "OCSP response verification failed");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  જો(!cert) {
    failf(data, "Error getting peer certificate");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }

  માટે(i = 0; i < (પૂર્ણાંક)sk_X509_અંક(ch); i++) {
    X509 *issuer = sk_X509_કિંમત(ch, (ossl_valsize_t)i);
    જો(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      તોડો;
    }
  }
  X509_free(cert);

  જો(!id) {
    failf(data, "Error computing OCSP ID");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  જો(ret != 1) {
    failf(data, "Could not find certificate ID in OCSP response");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  જો(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(data, "OCSP response has expired");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }

  infof(data, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  પસંદગી(cert_status) {
  સ્થિતિ V_OCSP_CERTSTATUS_GOOD:
    તોડો;

  સ્થિતિ V_OCSP_CERTSTATUS_REVOKED:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    failf(data, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    જાઓ end;

  સ્થિતિ V_OCSP_CERTSTATUS_UNKNOWN:
  મૂળભૂત:
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ end;
  }

end:
  જો(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  પાછા result;
}
#endif

સ્થિર અચલ અક્ષર *ssl_સંદેશ_type(પૂર્ણાંક ssl_ver, પૂર્ણાંક સંદેશ)
{
  જો(ssl_ver == SSL3_VERSION_MAJOR) {
    પસંદગી(સંદેશ) {
    સ્થિતિ SSL3_MT_HELLO_REQUEST:
      પાછા "Hello request";
    સ્થિતિ SSL3_MT_CLIENT_HELLO:
      પાછા "Client hello";
    સ્થિતિ SSL3_MT_SERVER_HELLO:
      પાછા "Server hello";
#ifdef SSL3_MT_NEWSESSION_TICKET
    સ્થિતિ SSL3_MT_NEWSESSION_TICKET:
      પાછા "Newsession Ticket";
#endif
    સ્થિતિ SSL3_MT_CERTIFICATE:
      પાછા "Certificate";
    સ્થિતિ SSL3_MT_SERVER_ચાવી_EXCHANGE:
      પાછા "Server key exchange";
    સ્થિતિ SSL3_MT_CLIENT_ચાવી_EXCHANGE:
      પાછા "Client key exchange";
    સ્થિતિ SSL3_MT_CERTIFICATE_REQUEST:
      પાછા "Request CERT";
    સ્થિતિ SSL3_MT_SERVER_DONE:
      પાછા "Server finished";
    સ્થિતિ SSL3_MT_CERTIFICATE_VERIFY:
      પાછા "CERT verify";
    સ્થિતિ SSL3_MT_FINISHED:
      પાછા "Finished";
#ifdef SSL3_MT_CERTIFICATE_STATUS
    સ્થિતિ SSL3_MT_CERTIFICATE_STATUS:
      પાછા "Certificate Status";
#endif
#ifdef SSL3_MT_ENCRYPTED_EXTENSIONS
    સ્થિતિ SSL3_MT_ENCRYPTED_EXTENSIONS:
      પાછા "Encrypted Extensions";
#endif
#ifdef SSL3_MT_SUPPLEMENTAL_DATA
    સ્થિતિ SSL3_MT_SUPPLEMENTAL_DATA:
      પાછા "Supplemental data";
#endif
#ifdef SSL3_MT_END_OF_EARLY_DATA
    સ્થિતિ SSL3_MT_END_OF_EARLY_DATA:
      પાછા "End of early data";
#endif
#ifdef SSL3_MT_ચાવી_UPDATE
    સ્થિતિ SSL3_MT_ચાવી_UPDATE:
      પાછા "Key update";
#endif
#ifdef SSL3_MT_NEXT_PROTO
    સ્થિતિ SSL3_MT_NEXT_PROTO:
      પાછા "Next protocol";
#endif
#ifdef SSL3_MT_MESSAGE_HASH
    સ્થિતિ SSL3_MT_MESSAGE_HASH:
      પાછા "Message hash";
#endif
    }
  }
  પાછા "Unknown";
}

સ્થિર અચલ અક્ષર *tls_rt_type(પૂર્ણાંક type)
{
  પસંદગી(type) {
#ifdef SSL3_RT_HEADER
  સ્થિતિ SSL3_RT_HEADER:
    પાછા "TLS header";
#endif
  સ્થિતિ SSL3_RT_CHANGE_CIPHER_SPEC:
    પાછા "TLS change cipher";
  સ્થિતિ SSL3_RT_ALERT:
    પાછા "TLS alert";
  સ્થિતિ SSL3_RT_HANDSHAKE:
    પાછા "TLS handshake";
  સ્થિતિ SSL3_RT_APPLICATION_DATA:
    પાછા "TLS app data";
  મૂળભૂત:
    પાછા "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
સ્થિર શૂન્ય ossl_trace(પૂર્ણાંક direction, પૂર્ણાંક ssl_ver, પૂર્ણાંક content_type,
                       અચલ શૂન્ય *buf, size_t len, SSL *ssl,
                       શૂન્ય *userp)
{
  અચલ અક્ષર *verstr;
  સંરચના Curl_cfilter *cf = userp;
  સંરચના Curl_easy *data = ખાલી;
  અક્ષર unknown[32];

  જો(!cf)
    પાછા;
  data = CF_DATA_CURRENT(cf);
  જો(!data || !data->set.fdebug || (direction && direction != 1))
    પાછા;

  પસંદગી(ssl_ver) {
#ifdef SSL3_VERSION
  સ્થિતિ SSL3_VERSION:
    verstr = "SSLv3";
    તોડો;
#endif
  સ્થિતિ TLS1_VERSION:
    verstr = "TLSv1.0";
    તોડો;
#ifdef TLS1_1_VERSION
  સ્થિતિ TLS1_1_VERSION:
    verstr = "TLSv1.1";
    તોડો;
#endif
#ifdef TLS1_2_VERSION
  સ્થિતિ TLS1_2_VERSION:
    verstr = "TLSv1.2";
    તોડો;
#endif
  સ્થિતિ TLS1_3_VERSION:
    verstr = "TLSv1.3";
    તોડો;
  મૂળભૂત:
    curl_msnprintf(unknown, કદ(unknown), "(%x)", (unsigned પૂર્ણાંક)ssl_ver);
    verstr = unknown;
    તોડો;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  જો(ssl_ver
#ifdef SSL3_RT_HEADER
     && content_type != SSL3_RT_HEADER
#endif
#ifdef SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#endif
    ) {
    અચલ અક્ષર *સંદેશ_name = "Truncated message";
    અચલ અક્ષર *tls_rt_name;
    અક્ષર ssl_buf[1024];
    પૂર્ણાંક સંદેશ_type = 0;
    પૂર્ણાંક txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    જો(ssl_ver == SSL3_VERSION_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    નહીંતર
      tls_rt_name = "";

    જો(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      જો(len) {
        સંદેશ_type = *(અચલ unsigned અક્ષર *)buf;
        સંદેશ_name = "Change cipher spec";
      }
    }
    નહીંતર જો(content_type == SSL3_RT_ALERT) {
      જો(len >= 2) {
        સંદેશ_type =
          (((અચલ unsigned અક્ષર *)buf)[0] << 8) +
           ((અચલ unsigned અક્ષર *)buf)[1];
        સંદેશ_name = SSL_alert_desc_string_long(સંદેશ_type);
      }
    }
    નહીંતર જો(len) {
      સંદેશ_type = *(અચલ unsigned અક્ષર *)buf;
      સંદેશ_name = ssl_સંદેશ_type(ssl_ver, સંદેશ_type);
    }

    txt_len = curl_msnprintf(ssl_buf, કદ(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, સંદેશ_name, સંદેશ_type);
    Curl_debug(data, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(data, (direction == 1) ? CURLINFO_SSL_DATA_OUT :
             CURLINFO_SSL_DATA_IN, (અચલ અક્ષર *)buf, len);
  (શૂન્ય)ssl;
}

સ્થિર CURLcode ossl_set_ssl_version_min_max(સંરચના Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             unsigned પૂર્ણાંક ssl_version_min)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  long curl_ssl_version_min = (long)ssl_version_min;
  long curl_ssl_version_max;

  /* convert curl min SSL version option to OpenSSL constant */
#જો defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_VERSION_સંખ્યા)
  uint16_t ossl_ssl_version_min = 0;
  uint16_t ossl_ssl_version_max = 0;
#નહીંતર
  long ossl_ssl_version_min = 0;
  long ossl_ssl_version_max = 0;
#endif
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_version_min != CURL_SSLVERSION_DEFAULT);
  પસંદગી(curl_ssl_version_min) {
  સ્થિતિ CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  સ્થિતિ CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    તોડો;
  સ્થિતિ CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    તોડો;
  સ્થિતિ CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    તોડો;
  સ્થિતિ CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    તોડો;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (long)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  પસંદગી(curl_ssl_version_max) {
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    તોડો;
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    તોડો;
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    તોડો;
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    તોડો;
  સ્થિતિ CURL_SSLVERSION_MAX_NONE:  /* none selected */
  સ્થિતિ CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  મૂળભૂત:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    તોડો;
  }

  જો(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    પાછા CURLE_SSL_CONNECT_ERROR;

  પાછા CURLE_OK;
}

CURLcode Curl_ossl_add_session(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના ossl_ctx *octx,
                               અચલ અક્ષર *ssl_peer_ચાવી,
                               SSL_SESSION *session,
                               અચલ અક્ષર *alpn,
                               unsigned અક્ષર *quic_tp,
                               size_t quic_tp_len,
                               સંરચના Curl_ssl_session **psession)
{
  સંરચના Curl_ssl_session *sc_session = ખાલી, *sc_dup = ખાલી;
  unsigned અક્ષર *der_session_buf = ખાલી;
  unsigned અક્ષર *qtp_clone = ખાલી;
  CURLcode result = CURLE_OK;

  જો(psession)
    *psession = ખાલી;
  જો(!cf || !data)
    જાઓ out;

  જો(Curl_ssl_scache_use(cf, data)) {
    size_t der_session_size;
    unsigned અક્ષર *der_session_ptr;
    size_t earlydata_max = 0;
    પૂર્ણાંક ietf_tls_id = SSL_version(octx->ssl);

    der_session_size = i2d_SSL_SESSION(session, ખાલી);
    જો(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_size);
    જો(!der_session_buf) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }

    der_session_size = i2d_SSL_SESSION(session, &der_session_ptr);
    જો(der_session_size == 0) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }

#ifdef HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_data(session);
#endif
    જો(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((અચલ અક્ષર *)quic_tp, quic_tp_len);
      જો(!qtp_clone) {
        result = CURLE_OUT_OF_MEMORY;
        જાઓ out;
      }
    }

    result = Curl_ssl_session_create2(der_session_buf, der_session_size,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(ખાલી) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = ખાલી;  /* took ownership of sdata */
#ifdef USE_APPLE_SECTRUST
    જો(!result)
      sc_session->sectrust_verified = octx->sectrust_verified;
#endif
    જો(!result && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        result = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    જો(!result) {
      result = Curl_ssl_scache_put(cf, data, ssl_peer_ચાવી, sc_session);
      /* took ownership of `sc_session` */
      sc_session = ખાલી;
    }
  }

out:
  curlx_free(der_session_buf);
  જો(!result && psession) {
    *psession = sc_dup;
    sc_dup = ખાલી;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  પાછા result;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
સ્થિર પૂર્ણાંક ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  સંરચના Curl_cfilter *cf = (સંરચના Curl_cfilter *)SSL_get_app_data(ssl);
  જો(cf) {
    સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
    સંરચના ssl_connect_data *connssl = cf->ctx;
    સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
    સંરચના Curl_ssl_session *session = ખાલી;
    Curl_ossl_add_session(cf, data, octx, connssl->peer.scache_ચાવી,
                          ssl_sessionid, connssl->negotiated.alpn, ખાલી,
                          0, &session);
    જો(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  પાછા 0;
}

સ્થિર CURLcode load_cacert_from_memory(X509_STORE *store,
                                        અચલ સંરચના curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = ખાલી;
  ઢગલો_OF(X509_INFO) *inf = ખાલી;

  /* everything else is a reference */
  પૂર્ણાંક i, ગણતરી = 0;
  X509_INFO *itmp = ખાલી;

  જો(ca_info_blob->len > (size_t)INT_MAX)
    પાછા CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->data, (પૂર્ણાંક)ca_info_blob->len);
  જો(!cbio)
    પાછા CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, ખાલી, ખાલી, ખાલી);
  જો(!inf) {
    BIO_free(cbio);
    પાછા CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  માટે(i = 0; i < (પૂર્ણાંક)sk_X509_INFO_અંક(inf); ++i) {
    itmp = sk_X509_INFO_કિંમત(inf, (ossl_valsize_t)i);
    જો(itmp->x509) {
      જો(X509_STORE_add_cert(store, itmp->x509)) {
        ++ગણતરી;
      }
      નહીંતર {
        /* set count to 0 to return an error */
        ગણતરી = 0;
        તોડો;
      }
    }
    જો(itmp->crl) {
      જો(X509_STORE_add_crl(store, itmp->crl)) {
        ++ગણતરી;
      }
      નહીંતર {
        /* set count to 0 to return an error */
        ગણતરી = 0;
        તોડો;
      }
    }
  }

#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic ધકેલો
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif
  sk_X509_INFO_કાઢો_free(inf, X509_INFO_free);
#જો defined(__clang__) && __clang_major__ >= 16
#pragma clang diagnostic કાઢો
#endif
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  પાછા (ગણતરી > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#ifdef USE_WIN32_CRYPTO
સ્થિર CURLcode ossl_win_load_store(સંરચના Curl_easy *data,
                                    સંરચના Curl_cfilter *cf,
                                    અચલ અક્ષર *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode result = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  જો(hStore) {
    PCCERT_CONTEXT pContext = ખાલી;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = ખાલી;
    DWORD enhkey_usage_size = 0;
    VERBOSE(size_t total = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    result = CURLE_OK;
    માટે(;;) {
      X509 *x509;
      FILETIME now;
      BYTE ચાવી_usage[2];
      DWORD req_size;
      અચલ unsigned અક્ષર *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      જો(!pContext)
        તોડો;

      VERBOSE(++total);

#જો defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        અક્ષર cert_name[256];
        જો(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               ખાલી, cert_name, કદ(cert_name)))
          infof(data, "SSL: unknown cert name");
        નહીંતર
          infof(data, "SSL: Checking cert \"%s\"", cert_name);
      }
#endif
      encoded_cert = (અચલ unsigned અક્ષર *)pContext->pbCertEncoded;
      જો(!encoded_cert)
        ચાલુ;

      GetSystemTimeAsFileTime(&now);
      જો(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        ચાલુ;

      /* If key usage exists check for signing attribute */
      જો(Cert_Get_Intended_ચાવી_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 ચાવી_usage, કદ(ચાવી_usage))) {
        જો(!(ચાવી_usage[0] & CERT_ચાવી_CERT_SIGN_ચાવી_USAGE))
          ચાલુ;
      }
      નહીંતર જો(GetLastError())
        ચાલુ;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      જો(Cert_Get_Enhanced_ચાવી_Usage(pContext, 0, ખાલી, &req_size) && req_size) {
        જો(req_size > enhkey_usage_size) {
          શૂન્ય *tmp = curlx_realloc(enhkey_usage, req_size);

          જો(!tmp) {
            failf(data, "SSL: Out of memory allocating for OID list");
            result = CURLE_OUT_OF_MEMORY;
            તોડો;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_size = req_size;
        }

        જો(Cert_Get_Enhanced_ચાવી_Usage(pContext, 0, enhkey_usage, &req_size)) {
          જો(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            જો((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              ચાલુ;
          }
          નહીંતર {
            DWORD i;
            bool found = FALSE;

            માટે(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              જો(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                તોડો;
              }
            }

            જો(!found)
              ચાલુ;
          }
        }
        નહીંતર
          ચાલુ;
      }
      નહીંતર
        ચાલુ;

      x509 = d2i_X509(ખાલી, &encoded_cert, (long)pContext->cbCertEncoded);
      જો(!x509)
        ચાલુ;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      જો(X509_STORE_add_cert(store, x509) == 1) {
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

    જો(result)
      પાછા result;
  }

  પાછા result;
}

સ્થિર CURLcode ossl_windows_load_anchors(સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  સ્થિર અચલ અક્ષર * અચલ win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode result = CURLE_OK;

  *padded = FALSE;
  માટે(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    result = ossl_win_load_store(data, cf, win_stores[i], store, &store_added);
    જો(result)
      પાછા result;
    જો(store_added) {
      CURL_TRC_CF(data, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    નહીંતર
      infof(data, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  પાછા result;
}

#endif /* USE_WIN32_CRYPTO */

સ્થિર CURLcode ossl_load_trust_anchors(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના ossl_ctx *octx,
                                        X509_STORE *store)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  અચલ અક્ષર * અચલ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? ખાલી : conn_config->CAfile);
  અચલ અક્ષર * અચલ ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  જો(conn_config->native_ca_store) {
#ifdef USE_WIN32_CRYPTO
    bool added = FALSE;
    result = ossl_windows_load_anchors(cf, data, store, &added);
    જો(result)
      પાછા result;
    જો(added) {
      infof(data, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#અથવાજો defined(USE_APPLE_SECTRUST)
    infof(data, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#endif
  }

  જો(conn_config->ca_info_blob) {
    result = load_cacert_from_memory(store, conn_config->ca_info_blob);
    જો(result) {
      failf(data, "error adding trust anchors from certificate blob: %d",
            (પૂર્ણાંક)result);
      પાછા result;
    }
    infof(data, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  જો(ssl_cafile || ssl_capath) {
#ifdef HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    જો(ssl_cafile) {
      જો(!X509_STORE_load_file(store, ssl_cafile)) {
        જો(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from file: %s", ssl_cafile);
          પાછા CURLE_SSL_CACERT_BADFILE;
        }
        નહીંતર
          infof(data, "error setting certificate file, continuing anyway");
      }
      infof(data, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    જો(ssl_capath) {
      જો(!X509_STORE_load_path(store, ssl_capath)) {
        જો(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(data, "error adding trust anchors from path: %s", ssl_capath);
          પાછા CURLE_SSL_CACERT_BADFILE;
        }
        નહીંતર
          infof(data, "error setting certificate path, continuing anyway");
      }
      infof(data, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#નહીંતર
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    જો(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      જો(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(data, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        પાછા CURLE_SSL_CACERT_BADFILE;
      }
      નહીંતર {
        infof(data, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    જો(ssl_cafile)
      infof(data, "  CAfile: %s", ssl_cafile);
    જો(ssl_capath)
      infof(data, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#endif
  }

#ifdef CURL_CA_FALLBACK
  જો(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(data, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#endif
  જો(octx->store_is_empty && !have_native_check)
    infof(data, "  no trust anchors configured");

  પાછા result;
}

સ્થિર CURLcode ossl_populate_x509_store(સંરચના Curl_cfilter *cf,
                                         સંરચના Curl_easy *data,
                                         સંરચના ossl_ctx *octx,
                                         X509_STORE *store)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  X509_LOOKUP *lookup = ખાલી;
  અચલ અક્ષર * અચલ ssl_crlfile = conn_config->CRLfile;
  unsigned long x509flags = 0;

  CURL_TRC_CF(data, cf, "configuring OpenSSL's x509 trust store");
  જો(!store)
    પાછા CURLE_OUT_OF_MEMORY;

  જો(!conn_config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    પાછા CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  result = ossl_load_trust_anchors(cf, data, octx, store);
  જો(result)
    પાછા result;

  /* Does not make sense to load a CRL file without peer verification */
#ifdef USE_APPLE_SECTRUST
  જો(ssl_crlfile && conn_config->native_ca_store) {
    failf(data, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    પાછા CURLE_NOT_BUILT_IN;
  }
#endif
  જો(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    જો(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(data, "error loading CRL file: %s", ssl_crlfile);
      પાછા CURLE_SSL_CRL_BADFILE;
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

  જો(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (શૂન્ય)X509_STORE_set_flags(store, x509flags);

  પાછા result;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_OSSL_X509_ચાવી  "tls:ossl:x509:share"

સંરચના ossl_x509_share {
  અક્ષર *CAfile;         /* CAfile path used to generate X509 store */
  અક્ષર *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  સંરચના curltime time; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

સ્થિર શૂન્ય oss_x509_share_free(અચલ શૂન્ય *ચાવી, size_t ચાવી_len, શૂન્ય *p)
{
  સંરચના ossl_x509_share *share = p;
  DEBUGASSERT(ચાવી_len == CURL_CSTRLEN(MPROTO_OSSL_X509_ચાવી));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_ચાવી, ચાવી, ચાવી_len));
  (શૂન્ય)ચાવી;
  (શૂન્ય)ચાવી_len;
  જો(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

સ્થિર bool ossl_cached_x509_store_expired(સંરચના Curl_easy *data,
                                           અચલ સંરચના ossl_x509_share *mb)
{
  જો(data->set.ssl_ca_cache_timeout < 0)
    પાછા FALSE;
  નહીંતર {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &mb->time);
    timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

    પાછા elapsed_ms >= timeout_ms;
  }
}

સ્થિર bool ossl_cached_x509_store_different(સંરચના Curl_cfilter *cf,
                                             અચલ સંરચના Curl_easy *data,
                                             અચલ સંરચના ossl_x509_share *mb)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (શૂન્ય)data;
  જો(mb->no_partialchain != conn_config->no_partialchain)
    પાછા TRUE;
  જો(!mb->CAfile || !conn_config->CAfile) {
    જો(mb->CAfile != conn_config->CAfile)
      પાછા TRUE;
  }
  નહીંતર જો(strcmp(mb->CAfile, conn_config->CAfile))
    પાછા TRUE;

  જો(!mb->CApath || !conn_config->CApath) {
    જો(mb->CApath != conn_config->CApath)
      પાછા TRUE;
  }
  નહીંતર જો(strcmp(mb->CApath, conn_config->CApath))
    પાછા TRUE;

  પાછા FALSE;
}

સ્થિર X509_STORE *ossl_get_cached_x509_store(સંરચના Curl_cfilter *cf,
                                              સંરચના Curl_easy *data,
                                              bool *pempty)
{
  સંરચના Curl_multi *multi = data->multi;
  સંરચના ossl_x509_share *share;
  X509_STORE *store = ખાલી;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_ચાવી,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_ચાવી)) : ખાલી;
  જો(share && share->store &&
     !ossl_cached_x509_store_expired(data, share) &&
     !ossl_cached_x509_store_different(cf, data, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  પાછા store;
}

સ્થિર શૂન્ય ossl_set_cached_x509_store(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       X509_STORE *store,
                                       bool is_empty)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના Curl_multi *multi = data->multi;
  સંરચના ossl_x509_share *share;

  DEBUGASSERT(multi);
  જો(!multi)
    પાછા;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_ચાવી,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_ચાવી));

  જો(!share) {
    share = curlx_calloc(1, કદ(*share));
    જો(!share)
      પાછા;
    જો(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_ચાવી,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_ચાવી),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      પાછા;
    }
  }

  જો(X509_STORE_up_ref(store)) {
    અક્ષર *CAfile = ખાલી;
    અક્ષર *CApath = ખાલી;

    જો(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      જો(!CAfile) {
        X509_STORE_free(store);
        પાછા;
      }
    }

    જો(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      જો(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        પાછા;
      }
    }

    જો(share->store) {
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
#નહીંતર
#define USING_PROVIDER(x) 0
#endif

CURLcode Curl_ssl_setup_x509_store(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના ossl_ctx *octx)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
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
  જો(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  નહીંતર {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    result = ossl_populate_x509_store(cf, data, octx, store);
    જો(result == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, data, store, (bool)octx->store_is_empty);
    }
  }

  ERR_કાઢો_to_mark();

  પાછા result;
}

સ્થિર bool ossl_apply_session(
  સંરચના ossl_ctx *octx,
  સંરચના Curl_cfilter *cf,
  સંરચના Curl_easy *data,
  સંરચના alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  સંરચના Curl_ssl_session *scs)
{
  સંરચના ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  અચલ unsigned અક્ષર *der_sessionid = scs->sdata;
  size_t der_sessionid_size = scs->sdata_len;
  SSL_SESSION *ssl_session = ખાલી;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(ખાલી, &der_sessionid,
                                (long)der_sessionid_size);
  જો(ssl_session) {
    જો(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(અક્ષર error_buffer[256]);
      infof(data, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_buffer,
                          કદ(error_buffer)));
    }
    નહીંતર {
      જો(conn_cfg->verifypeer &&
         (SSL_get_verify_result(octx->ssl) != X509_V_OK)
#ifdef USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#endif
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, ખાલી);
        infof(data, "SSL session not peer verified, not reusing");
      }
      નહીંતર {
        infof(data, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#ifdef USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#endif
        infof(data, "SSL verify result: %lx",
              (unsigned long)SSL_get_verify_result(octx->ssl));
#ifdef HAVE_OPENSSL_EARLYDATA
        જો(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_data(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_data = FALSE;
          જો(sess_reuse_cb)
            (શૂન્ય)sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          જો(do_early_data) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_to(alpns, scs->alpn);
          }
        }
#નહીંતર
        (શૂન્ય)alpns;
        (શૂન્ય)conn_cfg;
        (શૂન્ય)sess_reuse_cb;
#endif
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  નહીંતર {
    infof(data, "SSL session not accepted by OpenSSL, continuing without");
  }
  પાછા (bool)octx->reused_session;
}

સ્થિર CURLcode ossl_init_session_and_alpns(
  સંરચના ossl_ctx *octx,
  સંરચના Curl_cfilter *cf,
  સંરચના Curl_easy *data,
  સંરચના ssl_peer *peer,
  અચલ સંરચના alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  સંરચના ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  સંરચના alpn_spec alpns;
  CURLcode result;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  જો((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    સંરચના Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    જો(scs) {
      જો(ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
    }
  }

  જો(!octx->reused_session &&
     Curl_ssl_scache_use(cf, data) && !conn_cfg->verifystatus) {
    સંરચના Curl_ssl_session *scs = ખાલી;

    result = Curl_ssl_scache_take(cf, data, peer->scache_ચાવી, &scs);
    જો(!result && scs && scs->sdata && scs->sdata_len) {
      (શૂન્ય)ossl_apply_session(octx, cf, data, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, data, peer->scache_ચાવી, scs);
  }

  જો(alpns.ગણતરી) {
    સંરચના alpn_proto_buf proto;
    memset(&proto, 0, કદ(proto));
    result = Curl_alpn_to_proto_buf(&proto, &alpns);
    જો(result) {
      failf(data, "Error determining ALPN");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
    જો(SSL_set_alpn_protos(octx->ssl, proto.data, proto.len)) {
      failf(data, "Error setting ALPN");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
  }

  પાછા CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
સ્થિર CURLcode ossl_init_ech(સંરચના ossl_ctx *octx,
                              સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              સંરચના ssl_peer *peer)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  પૂર્ણાંક trying_ech_now = 0;

  જો(!Curl_ssl_ech_enabled(cf))
    પાછા CURLE_OK;

  જો(conn_config->ech == CURLECH_GREASE) {
    infof(data, "ECH: will GREASE ClientHello");
#ifdef HAVE_BORINGSSL_LIKE
    SSL_set_enable_ech_grease(octx->ssl, 1);
#નહીંતર
    SSL_set_options(octx->ssl, SSL_OP_ECH_GREASE);
#endif
  }
  નહીંતર જો(conn_config->ech && conn_config->ech_config) {
#ifdef HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode result;

    જો(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * strlen(conn_config->ech_config);
    result = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    જો(result || !ech_config) {
      infof(data, "ECH: cannot base64 decode ECHConfig from command line");
      જો(conn_config->ech == CURLECH_HARD)
        પાછા result;
    }
    જો(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      જો(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        પાછા CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#નહીંતર
    size_t ech_config_len = 0;
    જો(!conn_config->ech_config[0]) {
      infof(data, "ECH: ECHConfig from command line empty");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = strlen(conn_config->ech_config);
    જો(SSL_set1_ech_config_list(octx->ssl,
                                (અચલ uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(data, "ECH: SSL_ECH_set1_ech_config_list failed");
      જો(conn_config->ech == CURLECH_HARD)
        પાછા CURLE_SSL_CONNECT_ERROR;
    }
    નહીંતર
      trying_ech_now = 1;
#endif /* HAVE_BORINGSSL_LIKE */
    infof(data, "ECH: ECHConfig from command line");
  }
  નહીંતર {
    અચલ સંરચના Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, peer->origin);

    જો(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      અચલ unsigned અક્ષર *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(data, "ECH: ECHConfig from HTTPS RR");
      જો(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(data, "ECH: SSL_set1_ech_config_list failed");
        જો(conn_config->ech == CURLECH_HARD)
          પાછા CURLE_SSL_CONNECT_ERROR;
      }
      નહીંતર {
        trying_ech_now = 1;
        infof(data, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    નહીંતર {
      infof(data, "ECH: requested but no ECHConfig available");
      જો(conn_config->ech == CURLECH_HARD)
        પાછા CURLE_SSL_CONNECT_ERROR;
    }
  }
#ifdef HAVE_BORINGSSL_LIKE
  (શૂન્ય)peer;
  જો(trying_ech_now && conn_config->ech_public) {
    infof(data, "ECH: setting public_name not supported with BoringSSL");
    પાછા CURLE_SSL_CONNECT_ERROR;
  }
#નહીંતર
  જો(trying_ech_now && conn_config->ech_public) {
    પૂર્ણાંક ret;
    infof(data, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    જો(ret != 1) {
      infof(data, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
  }
#endif /* HAVE_BORINGSSL_LIKE */
  જો(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(data, "ECH: cannot force TLSv1.3 [ERROR]");
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  પાછા CURLE_OK;
}
#endif /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

સ્થિર CURLcode ossl_init_ssl(સંરચના ossl_ctx *octx,
                              સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              સંરચના ssl_peer *peer,
                              અચલ સંરચના alpn_spec *alpns_requested,
                              શૂન્ય *ssl_user_data,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  જો(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  જો(!octx->ssl) {
    failf(data, "SSL: could not create a context (handle)");
    પાછા CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_data(octx->ssl, ssl_user_data);

#ifndef OPENSSL_NO_OCSP
  જો(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#endif

  SSL_set_connect_state(octx->ssl);

  જો(peer->sni) {
    જો(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(data, "Failed set SNI");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
  }

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode result = ossl_init_ech(octx, cf, data, peer);
    જો(result)
      પાછા result;
  }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  પાછા ossl_init_session_and_alpns(octx, cf, data, peer,
                                     alpns_requested, sess_reuse_cb);
}

સ્થિર CURLcode ossl_init_method(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના ssl_peer *peer,
                                 અચલ SSL_METHOD **pmethod,
                                 unsigned પૂર્ણાંક *pssl_version_min)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = ખાલી;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  પસંદગી(peer->transport) {
  સ્થિતિ TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    પસંદગી(*pssl_version_min) {
    સ્થિતિ CURL_SSLVERSION_TLSv1:
    સ્થિતિ CURL_SSLVERSION_TLSv1_0:
    સ્થિતિ CURL_SSLVERSION_TLSv1_1:
    સ્થિતિ CURL_SSLVERSION_TLSv1_2:
    સ્થિતિ CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      તોડો;
    સ્થિતિ CURL_SSLVERSION_SSLv2:
      failf(data, "No SSLv2 support");
      પાછા CURLE_NOT_BUILT_IN;
    સ્થિતિ CURL_SSLVERSION_SSLv3:
      failf(data, "No SSLv3 support");
      પાછા CURLE_NOT_BUILT_IN;
    મૂળભૂત:
      failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
    તોડો;
  સ્થિતિ TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    જો(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(data, "QUIC needs at least TLS version 1.3");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    તોડો;
  મૂળભૂત:
    failf(data, "unsupported transport %d in SSL init", peer->transport);
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  પાછા *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(સંરચના ossl_ctx *octx,
                            સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            સંરચના ssl_peer *peer,
                            અચલ સંરચના alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            શૂન્ય *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            શૂન્ય *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *ciphers;
  અચલ SSL_METHOD *req_method = ખાલી;
  ctx_option_t ctx_options = 0;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  અક્ષર * અચલ ssl_cert = conn_config->clientcert;
  અચલ સંરચના curl_blob *ssl_cert_blob = conn_config->cert_blob;
  અચલ અક્ષર * અચલ ssl_cert_type = conn_config->cert_type;
  unsigned પૂર્ણાંક ssl_version_min = CURL_SSLVERSION_DEFAULT;
  અક્ષર error_buffer[256];

  /* Make funny stuff to get random input */
  result = ossl_seed(data);
  જો(!result) {
    જો(CURL_EASY_STR(data, STRING_SSL_ENGINE))
      result = ossl_set_engine(data, octx,
                               CURL_EASY_STR(data, STRING_SSL_ENGINE));
    જો(!result && data->set.engine_default)
      result = ossl_set_engine_default(data, octx);
  }

  જો(!result) {
    ssl_config->certverifyresult = !X509_V_OK;
    result = ossl_init_method(cf, data, peer, &req_method, &ssl_version_min);
  }
  જો(result)
    પાછા result;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#ifdef OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#endif
    SSL_CTX_new(req_method);

  જો(!octx->ssl_ctx) {
    failf(data, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_buffer, કદ(error_buffer)));
    પાછા CURLE_OUT_OF_MEMORY;
  }

  જો(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    જો(result)
      પાછા result;
  }

  જો(data->set.fdebug && data->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_સંદેશ_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_સંદેશ_callback_arg(octx->ssl_ctx, cf);
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
  જો(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_ઉમેરો_EMPTY_FRAGMENTS;

  DEBUGASSERT(ssl_version_min != CURL_SSLVERSION_DEFAULT);
  પસંદગી(ssl_version_min) {
  સ્થિતિ CURL_SSLVERSION_SSLv2:
  સ્થિતિ CURL_SSLVERSION_SSLv3:
    પાછા CURLE_NOT_BUILT_IN;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  સ્થિતિ CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  સ્થિતિ CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  સ્થિતિ CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  સ્થિતિ CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  સ્થિતિ CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    result = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    જો(result)
      પાછા result;
    તોડો;

  મૂળભૂત:
    failf(data, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    પાછા CURLE_SSL_CONNECT_ERROR;
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
  જો(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = ખાલી;
  જો(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    જો(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(data, "failed setting cipher list: %s", ciphers);
      પાછા CURLE_SSL_CIPHER;
    }
    infof(data, "Cipher selection: %s", ciphers);
  }

#ifdef HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    અચલ અક્ષર *ciphers13 = conn_config->cipher_list13;
    જો(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      જો(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(data, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        પાછા CURLE_SSL_CIPHER;
      }
      infof(data, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#endif

  જો(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    result = client_cert(data, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->ચાવી, conn_config->ચાવી_blob,
                         conn_config->ચાવી_type,
                         conn_config->ચાવી_passwd);
    જો(result)
      /* failf() is already done in client_cert() */
      પાછા result;
  }

#ifdef HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#endif

  {
    અચલ અક્ષર *curves = conn_config->curves;
    જો(curves) {
#ifdef HAVE_BORINGSSL_LIKE
#define OSSL_CURVE_CAST(x) (x)
#નહીંતર
#define OSSL_CURVE_CAST(x) (અક્ષર *)CURL_UNCONST(x)
#endif
      જો(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(data, "failed setting curves list: '%s'", curves);
        પાછા CURLE_SSL_CIPHER;
      }
    }
  }

#ifdef HAVE_SSL_CTX_SET1_SIGALGS
#define OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    અચલ અક્ષર *signature_algorithms = conn_config->signature_algorithms;
    જો(signature_algorithms) {
      જો(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(data, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        પાછા CURLE_SSL_CIPHER;
      }
    }
  }
#endif

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, ખાલી);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#જો !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  જો(Curl_tls_keylog_enabled()) {
    SSL_CTX_set_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#endif

  જો(cb_new_session) {
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
  જો(data->set.ssl_fsslctx) {
    સંરચના Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    જો(!octx->x509_store_setup) {
      result = Curl_ssl_setup_x509_store(cf, data, octx);
      જો(result)
        પાછા result;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, data, easy_fsslctx);
    result = (*data->set.ssl_fsslctx)(data, octx->ssl_ctx,
                                      data->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    જો(result) {
      failf(data, "error signaled by SSL ctx callback");
      પાછા result;
    }
  }

  પાછા ossl_init_ssl(octx, cf, data, peer, alpns_requested,
                       ssl_user_data, sess_reuse_cb);
}

સ્થિર CURLcode ossl_on_session_reuse(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      સંરચના alpn_spec *alpns,
                                      સંરચના Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  પાછા Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max);
}

શૂન્ય Curl_ossl_report_handshake(સંરચના Curl_easy *data, સંરચના ossl_ctx *octx)
{
#ifdef CURLVERBOSE
  જો(Curl_trc_is_verbose(data)) {
    પૂર્ણાંક psigtype_nid = NID_undef;
    અચલ અક્ષર *negotiated_group_name = ખાલી;

#ifdef HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#જો OPENSSL_VERSION_સંખ્યા >= 0x30200000L
    negotiated_group_name = SSL_get0_group_name(octx->ssl);
#નહીંતર
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
#નહીંતર
  (શૂન્ય)data;
  (શૂન્ય)octx;
#endif /* CURLVERBOSE */
}

સ્થિર CURLcode ossl_connect_step1(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode result;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  result = Curl_ossl_ctx_init(octx, cf, data, &connssl->peer,
                              connssl->alpn, ખાલી, ખાલી,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  જો(result)
    પાછા result;

  octx->bio_method = ossl_bio_cf_method_create();
  જો(!octx->bio_method)
    પાછા CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  જો(!bio)
    પાછા CURLE_OUT_OF_MEMORY;

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
#નહીંતર
  SSL_set_bio(octx->ssl, bio, bio);
#endif

  જો(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    સંરચના alpn_proto_buf proto;
    memset(&proto, 0, કદ(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }

  connssl->connecting_state = ssl_connect_2;
  પાછા CURLE_OK;
}

#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
સ્થિર પૂર્ણાંક ossl_trace_ech_retry_configs(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data, SSL *ssl,
                                        પૂર્ણાંક reason)
{
  CURLcode result = CURLE_OK;
  size_t rcl = 0;
  પૂર્ણાંક rv = 1;
#ifndef HAVE_BORINGSSL_LIKE
  અક્ષર *inner = ખાલી;
  uint8_t *rcs = ખાલી;
  અક્ષર *outer = ખાલી;
#નહીંતર
  અચલ અક્ષર *inner = ખાલી;
  અચલ uint8_t *rcs = ખાલી;
  અચલ અક્ષર *outer = ખાલી;
  size_t out_name_len = 0;
  પૂર્ણાંક servername_type = 0;
#endif
  NOVERBOSE((શૂન્ય)reason);

  /* nothing to trace if not doing ECH */
  જો(!Curl_ssl_ech_enabled(cf))
    પાછા rv;
#ifndef HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#નહીંતર
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (પૂર્ણાંક)rcl;
#endif

  જો(rv && rcs) {
    અક્ષર *b64str = ખાલી;
    size_t blen = 0;

    result = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    જો(!result && b64str) {
      infof(data, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#ifndef HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#નહીંતર
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(data, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#endif
    }
  }
  નહીંતર
    infof(data, "ECH: no retry_configs (rv = %d)", rv);
#ifndef HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#endif
  પાછા rv;
}

#endif

સ્થિર CURLcode ossl_connect_step2(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  સંરચના ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  પૂર્ણાંક err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  જો(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_ssl_setup_x509_store(cf, data, octx);
    જો(result)
      પાછા result;
    octx->x509_store_setup = TRUE;
  }

#જો !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  જો(Curl_tls_keylog_enabled() && !octx->keylog_done)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_done);
#endif

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  જો(err != 1) {
    પૂર્ણાંક detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(data, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    જો(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      પાછા CURLE_AGAIN;
    }
    જો(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      પાછા CURLE_AGAIN;
    }
#ifdef SSL_ERROR_WANT_ASYNC
    જો(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      પાછા CURLE_AGAIN;
    }
#endif
#ifdef SSL_ERROR_WANT_RETRY_VERIFY
    જો(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(data, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(data, TRUE);
      પાછા CURLE_AGAIN;
    }
#endif
    નહીંતર {
      /* untreated error */
      sslerr_t errdetail;
      અક્ષર error_buffer[256] = "";
      CURLcode result;
      long lerr;
      પૂર્ણાંક lib;
      પૂર્ણાંક reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      જો((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#ifdef SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#endif
         )) {
        result = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_result(octx->ssl);
        જો(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(data, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        નહીંતર
          failf(data, "%s", "SSL certificate verification failed");
      }
#ifdef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      નહીંતર જો((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        result = CURLE_SSL_CLIENTCERT;
        failf(data, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_buffer, કદ(error_buffer)));
      }
#endif
#ifdef HAVE_SSL_SET1_ECH_CONFIG_LIST
      નહીંતર જો((lib == ERR_LIB_SSL) &&
#ifndef HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#નહીંતર
              (reason == SSL_R_ECH_REJECTED)) {
#endif /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, reason);

        result = CURLE_ECH_REQUIRED;
        failf(data, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_buffer, કદ(error_buffer)));
      }
#endif
      નહીંતર {
        result = CURLE_SSL_CONNECT_ERROR;
        failf(data, "TLS connect error: %s",
              ossl_strerror(errdetail, error_buffer, કદ(error_buffer)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      જો(result == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        અક્ષર extramsg[80] = "";
        પૂર્ણાંક sockerr = SOCKERRNO;

        જો(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, કદ(extramsg));
        failf(data, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      પાછા result;
    }
  }
  નહીંતર {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(data, octx);

#જો defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    જો(Curl_ssl_ech_enabled(cf)) {
      સંરચના ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      અક્ષર *inner = ખાલી, *outer = ખાલી;
      પૂર્ણાંક rv;
      VERBOSE(અચલ અક્ષર *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      પસંદગી(rv) {
      સ્થિતિ SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        તોડો;
      સ્થિતિ SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        તોડો;
      સ્થિતિ SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        તોડો;
      સ્થિતિ SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        તોડો;
      સ્થિતિ SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        તોડો;
      સ્થિતિ SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        તોડો;
      સ્થિતિ SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        તોડો;
      સ્થિતિ SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        તોડો;
      સ્થિતિ SSL_ECH_STATUS_BAD_NAME: {
        જો(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        નહીંતર {
          VERBOSE(status = "bad name (unexpected)");
        }
        તોડો;
      }
      મૂળભૂત:
        VERBOSE(status = "unexpected status");
        infof(data, "ECH: unexpected status %d", rv);
      }
      infof(data, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      જો(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, data, octx->ssl, 0);
      }
      જો(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(data, "ECH: ech-hard failed");
        પાછા CURLE_SSL_CONNECT_ERROR;
      }
    }
    નહીંતર {
      infof(data, "ECH: result: status is not attempted");
    }
#endif /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    જો(connssl->alpn) {
      અચલ unsigned અક્ષર *neg_protocol;
      unsigned પૂર્ણાંક len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      પાછા Curl_alpn_set_negotiated(cf, data, connssl, neg_protocol, len);
    }

    પાછા CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
સ્થિર CURLcode ossl_pkp_pin_peer_pubkey(સંરચના Curl_easy *data, X509 *cert,
                                         અચલ અક્ષર *pinnedpubkey)
{
  /* Scratch */
  પૂર્ણાંક len1 = 0, len2 = 0;
  unsigned અક્ષર *buff1 = ખાલી, *temp = ખાલી;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  જો(!pinnedpubkey)
    પાછા CURLE_OK;

  જો(!cert)
    પાછા result;

  કરો {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), ખાલી);
    જો(len1 < 1)
      તોડો; /* failed */

    buff1 = temp = curlx_malloc(len1);
    જો(!buff1)
      તોડો; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &temp);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    જો((len1 != len2) || !temp || ((temp - buff1) != len1))
      તોડો; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } જ્યારે(0);

  જો(buff1)
    curlx_free(buff1);

  પાછા result;
}

#ifdef CURLVERBOSE
#જો !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_સંખ્યા) && LIBRESSL_VERSION_સંખ્યા < 0x3060000fL)
સ્થિર શૂન્ય infof_certstack(સંરચના Curl_easy *data, અચલ SSL *ssl)
{
  ઢગલો_OF(X509) *certstack;
  long verify_result;
  પૂર્ણાંક અંક_cert_levels;
  પૂર્ણાંક cert_level;

  જો(!Curl_trc_is_verbose(data))
    પાછા;

  verify_result = SSL_get_verify_result(ssl);
  જો(verify_result != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  નહીંતર
    certstack = SSL_get0_verified_chain(ssl);
  જો(!certstack)
    પાછા;
  અંક_cert_levels = sk_X509_અંક(certstack);

  માટે(cert_level = 0; cert_level < અંક_cert_levels; cert_level++) {
    અક્ષર cert_algorithm[80] = "";
    અક્ષર group_name_final[80] = "";
    અચલ X509_ALGOR *palg_cert = ખાલી;
    અચલ ASN1_OBJECT *paobj_cert = ખાલી;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    પૂર્ણાંક ચાવી_bits;
    પૂર્ણાંક ચાવી_sec_bits;
    પૂર્ણાંક get_group_name;
    અચલ અક્ષર *type_name;

    current_cert = sk_X509_કિંમત(certstack, cert_level);
    જો(!current_cert)
      ચાલુ;

    current_pkey = X509_get0_pubkey(current_cert);
    જો(!current_pkey)
      ચાલુ;

    X509_get0_signature(ખાલી, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, ખાલી, ખાલી, palg_cert);
    OBJ_obj2txt(cert_algorithm, કદ(cert_algorithm), paobj_cert, 0);

    ચાવી_bits = EVP_PKEY_bits(current_pkey);
#ifndef HAVE_OPENSSL3
#define EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#endif
    ચાવી_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#ifdef HAVE_OPENSSL3
    {
      અક્ષર group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               કદ(group_name), ખાલી);
      curl_msnprintf(group_name_final, કદ(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#નહીંતર
    get_group_name = 0;
    type_name = ખાલી;
#endif

    infof(data, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          ચાવી_bits, ચાવી_sec_bits, cert_algorithm);
  }
}
#નહીંતર
#define infof_certstack(data, ssl)
#endif
#endif /* CURLVERBOSE */

સ્થિર CURLcode ossl_check_issuer(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  X509 *server_cert)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = ખાલી;
  BIO *fp = ખાલી;
  અક્ષર err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode result = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  જો(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->data,
                         (પૂર્ણાંક)conn_config->issuercert_blob->len);
    જો(!fp) {
      failf(data, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, કદ(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }
  }
  નહીંતર જો(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    જો(!fp) {
      failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, કદ(err_buf)));
      result = CURLE_OUT_OF_MEMORY;
      જાઓ out;
    }

    જો(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      જો(verify_enabled)
        failf(data, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      જાઓ out;
    }
  }

  જો(fp) {
    issuer = PEM_read_bio_X509(fp, ખાલી, ZERO_NULL, ખાલી);
    જો(!issuer) {
      જો(verify_enabled)
        failf(data, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      જાઓ out;
    }

    જો(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      જો(verify_enabled)
        failf(data, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      જાઓ out;
    }

    infof(data, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  જો(fp)
    BIO_free(fp);
  જો(issuer)
    X509_free(issuer);
  પાછા result;
}

સ્થિર અચલ અક્ષર *pinned(સંરચના Curl_cfilter *cf,
                          સંરચના Curl_easy *data)
{
  (શૂન્ય)cf;
  પાછા
#ifndef CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#endif
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
}

સ્થિર CURLcode ossl_check_pinned_ચાવી(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      X509 *server_cert)
{
  CURLcode result = CURLE_OK;
  અચલ અક્ષર *ptr = pinned(cf, data);
  જો(ptr) {
    result = ossl_pkp_pin_peer_pubkey(data, server_cert, ptr);
    જો(result)
      failf(data, "SSL: public key does not match pinned public key");
  }
  પાછા result;
}

#ifdef CURLVERBOSE
#define MAX_CERT_NAME_LENGTH 2048
સ્થિર CURLcode ossl_infof_cert(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                X509 *server_cert)
{
  BIO *mem = ખાલી;
  સંરચના dynbuf dname;
  અક્ષર err_buf[256] = "";
  અક્ષર *buf;
  long len;
  CURLcode result = CURLE_OK;

  જો(!Curl_trc_is_verbose(data))
    પાછા CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_LENGTH);
  mem = BIO_new(BIO_s_mem());
  જો(!mem) {
    failf(data, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, કદ(err_buf)));
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  infof(data, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  result = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(data, "  subject: %s", result ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_data(mem, (અક્ષર **)&buf);
  infof(data, "  start date: %.*s", (પૂર્ણાંક)len, buf);
  (શૂન્ય)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_data(mem, (અક્ષર **)&buf);
  infof(data, "  expire date: %.*s", (પૂર્ણાંક)len, buf);
  (શૂન્ય)BIO_reset(mem);

  result = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  જો(result) /* should be only fatal stuff like OOM */
    જાઓ out;
  infof(data, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  પાછા result;
}
#endif /* CURLVERBOSE */

#ifdef USE_APPLE_SECTRUST
સંરચના ossl_certs_ctx {
  ઢગલો_OF(X509) *sk;
  size_t અંક_certs;
  unsigned અક્ષર *last_der;
};

સ્થિર CURLcode ossl_chain_get_der(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   શૂન્ય *user_data,
                                   size_t i,
                                   unsigned અક્ષર **pder,
                                   size_t *pder_len)
{
  સંરચના ossl_certs_ctx *chain = user_data;
  X509 *cert;
  પૂર્ણાંક der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = ખાલી;

  (શૂન્ય)cf;
  (શૂન્ય)data;
  *pder_len = 0;
  *pder = ખાલી;

  જો(i >= chain->અંક_certs)
    પાછા CURLE_TOO_LARGE;
  cert = sk_X509_કિંમત(chain->sk, (પૂર્ણાંક)i);
  જો(!cert)
    પાછા CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  જો(der_len < 0)
    પાછા CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (size_t)der_len;
  પાછા CURLE_OK;
}

સ્થિર CURLcode ossl_apple_verify(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  સંરચના ossl_ctx *octx,
                                  સંરચના ssl_peer *peer)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના ossl_certs_ctx chain;
  CURLcode result;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, કદ(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.અંક_certs = chain.sk ? sk_X509_અંક(chain.sk) : 0;

  જો(!chain.અંક_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    જો(!octx->reused_session) {
      failf(data, "SSL: could not get peer certificate chain");
      result = CURLE_PEER_FAILED_VERIFICATION;
    }
    નહીંતર {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(data, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      પાછા CURLE_OK;
    }
  }
  નહીંતર {
#ifdef HAVE_BORINGSSL_LIKE
    અચલ uint8_t *ocsp_data = ખાલી;
#નહીંતર
    unsigned અક્ષર *ocsp_data = ખાલી;
#endif
    long ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    જો(!octx->reused_session)
      ocsp_len = (long)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_data);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    જો(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_data = ખાલી;
    }

    result = Curl_vtls_apple_verify(cf, data, peer, chain.અંક_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_data, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = ખાલી;
  }
  octx->sectrust_verified = !result;
  પાછા result;
}
#endif /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના ossl_ctx *octx,
                                   સંરચના ssl_peer *peer)
{
  સંરચના connectdata *conn = cf->conn;
  સંરચના ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  long ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  જો(data->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    result = ossl_certchain(data, octx->ssl);
    જો(result)
      પાછા result;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  જો(!server_cert) {
    /* no verification at all, this maybe acceptable */
    જો(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, data))
      જાઓ out;

    failf(data, "SSL: could not get peer certificate");
    result = CURLE_PEER_FAILED_VERIFICATION;
    જાઓ out;
  }

#ifdef CURLVERBOSE
  result = ossl_infof_cert(cf, data, server_cert);
  જો(result)
    જાઓ out;
  infof_certstack(data, octx->ssl);
#endif

  જો(conn_config->verifyhost) {
    result = ossl_verifyhost(data, conn, peer, server_cert);
    જો(result)
      જાઓ out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_result(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(data, "OpenSSL verify result: %lx", (unsigned long)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  જો(verified)
    infof(data, "SSL certificate verified via OpenSSL.");

#ifdef USE_APPLE_SECTRUST
  જો(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    result = ossl_apple_verify(cf, data, octx, peer);
    જો(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      જાઓ out; /* unexpected error */
    જો(octx->sectrust_verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#endif

  જો(!verified) {
    /* no trust established, report the OpenSSL status */
    જો(conn_config->verifypeer) {
      failf(data, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      result = CURLE_PEER_FAILED_VERIFICATION;
      જાઓ out;
    }
    infof(data, " SSL certificate verification failed, continuing anyway!");
  }

#ifndef OPENSSL_NO_OCSP
  જો(conn_config->verifystatus &&
#ifdef USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, data)) &&
#endif
     !octx->reused_session) {
    result = verifystatus(cf, data, octx);
    જો(result)
      જાઓ out;
  }
#endif

  result = ossl_check_issuer(cf, data, server_cert);
  જો(result)
    જાઓ out;

  result = ossl_check_pinned_ચાવી(cf, data, server_cert);

out:
  X509_free(server_cert);
  પાછા result;
}

સ્થિર CURLcode ossl_connect_step3(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  result = Curl_ossl_check_peer_cert(cf, data, octx, &connssl->peer);
  જો(result)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, data, connssl->peer.scache_ચાવી);

  પાછા result;
}

#ifdef HAVE_OPENSSL_EARLYDATA
સ્થિર CURLcode ossl_send_earlydata(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  અચલ unsigned અક્ષર *buf;
  size_t blen, nwritten;
  પૂર્ણાંક rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_result = CURLE_OK;
  જ્યારે(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_data(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    જો(rc <= 0) {
      long sslerror;
      અક્ષર error_buffer[256];
      પૂર્ણાંક err = SSL_get_error(octx->ssl, rc);

      પસંદગી(err) {
      સ્થિતિ SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        result = CURLE_AGAIN;
        જાઓ out;
      સ્થિતિ SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        result = CURLE_AGAIN;
        જાઓ out;
      સ્થિતિ SSL_ERROR_SYSCALL: {
        પૂર્ણાંક sockerr = SOCKERRNO;

        જો(octx->io_result == CURLE_AGAIN) {
          result = CURLE_AGAIN;
          જાઓ out;
        }
        sslerror = ERR_get_error();
        જો(sslerror)
          ossl_strerror(sslerror, error_buffer, કદ(error_buffer));
        નહીંતર જો(sockerr)
          curlx_strerror(sockerr, error_buffer, કદ(error_buffer));
        નહીંતર
          curl_msnprintf(error_buffer, કદ(error_buffer), "%s",
                         SSL_ERROR_to_str(err));

        failf(data, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_SEND_ERROR;
        જાઓ out;
      }
      સ્થિતિ SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(data, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_buffer, કદ(error_buffer)));
        result = CURLE_SEND_ERROR;
        જાઓ out;
      }
      મૂળભૂત:
        /* a true error */
        failf(data, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        result = CURLE_SEND_ERROR;
        જાઓ out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  પાછા result;
}
#endif /* HAVE_OPENSSL_EARLYDATA */

સ્થિર CURLcode ossl_connect(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             bool *done)
{
  CURLcode result = CURLE_OK;
  સંરચના ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  જો(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  જો(connssl->connecting_state == ssl_connect_1) {
    જો(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR, delaying connect");
      પાછા CURLE_OK;
    }
    CURL_TRC_CF(data, cf, "ossl_connect, step1");
    result = ossl_connect_step1(cf, data);
    જો(result)
      જાઓ out;
  }

  જો(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(data, cf, "ossl_connect, step2");
#ifdef HAVE_OPENSSL_EARLYDATA
    જો(connssl->earlydata_state == ssl_earlydata_await) {
      જાઓ out;
    }
    નહીંતર જો(connssl->earlydata_state == ssl_earlydata_sending) {
      result = ossl_send_earlydata(cf, data);
      જો(result)
        જાઓ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#endif
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    result = ossl_connect_step2(cf, data);
    જો(result)
      જાઓ out;
  }

  જો(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(data, cf, "ossl_connect, step3");
    result = ossl_connect_step3(cf, data);
    જો(result)
      જાઓ out;
    connssl->connecting_state = ssl_connect_done;
#ifdef HAVE_OPENSSL_EARLYDATA
    જો(connssl->earlydata_state > ssl_earlydata_none) {
      સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_data_status(octx->ssl) == SSL_EARLY_DATA_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
  }

  જો(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(data, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  જો(result == CURLE_AGAIN) {
    *done = FALSE;
    પાછા CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  પાછા result;
}

સ્થિર bool ossl_data_pending(સંરચના Curl_cfilter *cf,
                              અચલ સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  (શૂન્ય)data;
  પાછા (bool)connssl->આગત_pending;
}

સ્થિર CURLcode ossl_send(સંરચના Curl_cfilter *cf,
                          સંરચના Curl_easy *data,
                          અચલ શૂન્ય *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  પૂર્ણાંક err;
  અક્ષર error_buffer[256];
  sslerr_t sslerror;
  પૂર્ણાંક memlen;
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  પૂર્ણાંક nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)INT_MAX) ? INT_MAX : (પૂર્ણાંક)len;
  જો(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    જો(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      પાછા CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  જો(nwritten > 0) {
    જો(nwritten > memlen) {
      failf(data, "SSL_write(len=%d) returned %d", memlen, nwritten);
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
    *pnwritten = (size_t)nwritten;
  }
  નહીંતર {
    err = SSL_get_error(octx->ssl, nwritten);

    પસંદગી(err) {
    સ્થિતિ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      result = CURLE_AGAIN;
      જાઓ out;
    સ્થિતિ SSL_ERROR_WANT_WRITE:
      result = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      જાઓ out;
    સ્થિતિ SSL_ERROR_SYSCALL: {
      પૂર્ણાંક sockerr = SOCKERRNO;

      જો(octx->io_result == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        result = CURLE_AGAIN;
        જાઓ out;
      }
      sslerror = ERR_get_error();
      જો(sslerror)
        ossl_strerror(sslerror, error_buffer, કદ(error_buffer));
      નહીંતર જો(sockerr)
        curlx_strerror(sockerr, error_buffer, કદ(error_buffer));
      નહીંતર
        curl_msnprintf(error_buffer, કદ(error_buffer), "%s",
                       SSL_ERROR_to_str(err));

      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_buffer, sockerr);
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
    સ્થિતિ SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(data, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_buffer, કદ(error_buffer)));
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
    મૂળભૂત:
      /* a true error */
      failf(data, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      result = CURLE_SEND_ERROR;
      જાઓ out;
    }
  }

out:
  પાછા result;
}

સ્થિર CURLcode ossl_recv(સંરચના Curl_cfilter *cf,
                          સંરચના Curl_easy *data,   /* transfer */
                          અક્ષર *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  અક્ષર error_buffer[256];
  unsigned long sslerror;
  પૂર્ણાંક buffsize;
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  CURLcode result = CURLE_OK;
  પૂર્ણાંક nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)INT_MAX) ? INT_MAX : (પૂર્ણાંક)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  જો(nread > 0)
    *pnread = (size_t)nread;
  નહીંતર {
    /* failed SSL_read */
    પૂર્ણાંક err = SSL_get_error(octx->ssl, nread);

    પસંદગી(err) {
    સ્થિતિ SSL_ERROR_NONE: /* this is not an error */
      તોડો;
    સ્થિતિ SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      જો(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(data, cf, "TLS close_notify");
      તોડો;
    સ્થિતિ SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      જાઓ out;
    સ્થિતિ SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      જાઓ out;
    મૂળભૂત:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      જો(octx->io_result == CURLE_AGAIN) {
        result = CURLE_AGAIN;
        જાઓ out;
      }
      sslerror = ERR_get_error();
      જો((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        પૂર્ણાંક sockerr = SOCKERRNO;
        જો(sslerror)
          ossl_strerror(sslerror, error_buffer, કદ(error_buffer));
        નહીંતર જો(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_buffer, કદ(error_buffer));
        નહીંતર
          curl_msnprintf(error_buffer, કદ(error_buffer), "%s",
                         SSL_ERROR_to_str(err));
        failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_buffer, sockerr);
        result = CURLE_RECV_ERROR;
        જાઓ out;
      }
      નહીંતર જો(err == SSL_ERROR_SYSCALL) {
        જો(octx->io_result) {
          /* logging handling in underlying filter already */
          result = octx->io_result;
        }
        નહીંતર જો(connssl->peer_closed) {
          failf(data, "Connection closed abruptly");
          result = CURLE_RECV_ERROR;
        }
        નહીંતર {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          પૂર્ણાંક sockerr = SOCKERRNO;
          જો(sockerr)
            curlx_strerror(sockerr, error_buffer, કદ(error_buffer));
          નહીંતર {
            curl_msnprintf(error_buffer, કદ(error_buffer),
                           "Connection closed abruptly");
          }
          failf(data, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_buffer, sockerr);
          result = CURLE_RECV_ERROR;
        }
        જાઓ out;
      }
    }
  }

out:
  જો((!result && !*pnread) || (result == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->આગત_pending = FALSE;
  }
  CURL_TRC_CF(data, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (પૂર્ણાંક)result, *pnread, connssl->આગત_pending);
  પાછા result;
}

સ્થિર CURLcode ossl_get_channel_binding(સંરચના Curl_easy *data,
                                         int8_t sockindex,
                                         સંરચના dynbuf *binding)
{
  X509 *cert;
  પૂર્ણાંક mdnid;
  bool no_digest_acceptable = FALSE;
  અચલ EVP_MD *algo_type = ખાલી;
  અચલ અક્ષર *algo_name = ખાલી;
  unsigned પૂર્ણાંક length;
  unsigned અક્ષર buf[EVP_MAX_MD_SIZE];

  સ્થિર અચલ અક્ષર prefix[] = "tls-server-end-point:";
  સંરચના connectdata *conn = data->conn;
  સંરચના Curl_cfilter *cf = conn->cfilter[sockindex];
  સંરચના ossl_ctx *octx = ખાલી;
  CURLcode result = CURLE_OK;

  કરો {
    અચલ સંરચના Curl_cftype *cft = cf->cft;
    સંરચના ssl_connect_data *connssl = cf->ctx;

    જો(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (સંરચના ossl_ctx *)connssl->backend;
      તોડો;
    }

    cf = cf->next;
  } જ્યારે(cf);

  જો(!octx) {
    failf(data, "Failed to find the SSL filter");
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  જો(!cert)
    /* No server certificate, do not do channel binding */
    પાછા CURLE_OK;

#ifdef HAVE_OPENSSL3
  {
    પૂર્ણાંક pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    જો(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(data, "certificate signature algorithm not recognized");
      result = CURLE_SSL_INVALIDCERTSTATUS;
      જાઓ out;
    }

    જો(mdnid != NID_undef) {
      જો(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      નહીંતર
        algo_type = EVP_get_digestbynid(mdnid);
    }
    નહીંતર જો(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    નહીંતર જો(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      અક્ષર mdname[128] = "";
      પૂર્ણાંક rc = EVP_PKEY_get_default_digest_name(pkey, mdname, કદ(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      જો(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      નહીંતર જો(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(data, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#નહીંતર /* HAVE_OPENSSL3 */

  જો(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, ખાલી)) {
    failf(data,
          "Unable to find digest NID for certificate signature algorithm");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  જો(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  નહીંતર {
    algo_type = EVP_get_digestbynid(mdnid);
    જો(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(data, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      result = CURLE_SSL_INVALIDCERTSTATUS;
      જાઓ out;
    }
  }

#endif /* HAVE_OPENSSL3, else */

  જો(!algo_type) {
    જો(no_digest_acceptable) {
      infof(data, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      result = CURLE_OK;
      જાઓ out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(data, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ out;
  }

  જો(!X509_digest(cert, algo_type, buf, &length)) {
    failf(data, "X509_digest() failed for channel binding");
    result = CURLE_SSL_INVALIDCERTSTATUS;
    જાઓ out;
  }

  /* Append "tls-server-end-point:" */
  result = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  જો(result)
    જાઓ out;

  /* Append digest */
  result = curlx_dyn_addn(binding, buf, length);

out:
  X509_free(cert);
  પાછા result;
}

size_t Curl_ossl_version(અક્ષર *buffer, size_t size)
{
#ifdef LIBRESSL_VERSION_સંખ્યા
  અક્ષર *p;
  size_t ગણતરી;
  અચલ અક્ષર *ver = OpenSSL_version(OPENSSL_VERSION);
  સ્થિર અચલ અક્ષર expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  જો(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  ગણતરી = curl_msnprintf(buffer, size, "%s/%s", OSSL_PACKAGE, ver);
  માટે(p = buffer; *p; ++p) {
    જો(ISBLANK(*p))
      *p = '_';
  }
  પાછા ગણતરી;
#અથવાજો defined(OPENSSL_IS_AWSLC)
  પાછા curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_સંખ્યા_STRING);
#અથવાજો defined(OPENSSL_IS_BORINGSSL)
#ifdef CURL_BORINGSSL_VERSION
  પાછા curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#નહીંતર
  પાછા curl_msnprintf(buffer, size, OSSL_PACKAGE);
#endif
#નહીંતર /* OpenSSL 3+ */
  પાછા curl_msnprintf(buffer, size, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#endif
}

/* can be called with data == NULL */
સ્થિર CURLcode ossl_random(સંરચના Curl_easy *data,
                            unsigned અક્ષર *entropy, size_t length)
{
  પૂર્ણાંક rc;
  જો(data) {
    જો(ossl_seed(data)) /* Initiate the seed if not already done */
      પાછા CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  નહીંતર {
    જો(!rand_enough())
      પાછા CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(length));
  પાછા rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

સ્થિર CURLcode ossl_sha256sum(અચલ unsigned અક્ષર *આગત,
                               size_t len,
                               unsigned અક્ષર *sha256sum /* output */,
                               size_t unused)
{
  CURLcode result = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (શૂન્ય)unused;

  mdctx = EVP_MD_CTX_new();
  જો(!mdctx)
    પાછા CURLE_OUT_OF_MEMORY;
  જો(!EVP_DigestInit_ex(mdctx, EVP_sha256(), ખાલી)) {
    result = CURLE_FAILED_INIT;
    જાઓ out;
  }
  જો(!EVP_DigestUpdate(mdctx, આગત, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, ખાલી))
    result = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  પાછા result;
}

સ્થિર bool ossl_cert_status_request(શૂન્ય)
{
#ifndef OPENSSL_NO_OCSP
  પાછા TRUE;
#નહીંતર
  પાછા FALSE;
#endif
}

સ્થિર શૂન્ય *ossl_get_internals(સંરચના ssl_connect_data *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  સંરચના ossl_ctx *octx = (સંરચના ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  પાછા info == CURLINFO_TLS_SESSION ?
    (શૂન્ય *)octx->ssl_ctx : (શૂન્ય *)octx->ssl;
}

અચલ સંરચના Curl_ssl Curl_ssl_openssl = {
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

  કદ(સંરચના ossl_ctx),

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
