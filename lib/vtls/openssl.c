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
#समावेश "curl_setup.h"

#अगर_घोषित USE_OPENSSL

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "formdata.h" /* for the boundary function */
#समावेश "url.h" /* for the SSL config check function */
#समावेश "curlx/inet_pton.h"
#समावेश "vtls/openssl.h"
#समावेश "connect.h"
#समावेश "progress.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_int.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "vauth/vauth.h"
#समावेश "vtls/keylog.h"
#समावेश "vtls/hostcheck.h"
#समावेश "transfer.h"
#समावेश "multiif.h"
#समावेश "curlx/strerr.h"
#समावेश "curlx/strparse.h"
#समावेश "curlx/strcopy.h"
#समावेश "curlx/strdup.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vdns/httpsrr.h"
#समावेश "vtls/apple.h"
#अगर_घोषित USE_ECH
#समावेश "curlx/base64.h"
#पूर्ण_अगर

#समावेश <openssl/rand.h>
#समावेश <openssl/x509v3.h>
#अगर_अघोषित OPENSSL_NO_DSA
#समावेश <openssl/dsa.h>
#पूर्ण_अगर
#समावेश <openssl/dh.h>
#समावेश <openssl/err.h>
#समावेश <openssl/conf.h>
#समावेश <openssl/bn.h>
#समावेश <openssl/rsa.h>
#समावेश <openssl/bio.h>
#समावेश <openssl/pkcs12.h>
#समावेश <openssl/tls1.h>
#समावेश <openssl/evp.h>

#अगर defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#समावेश <openssl/ech.h>
#पूर्ण_अगर

#अगर_अघोषित OPENSSL_NO_OCSP
#समावेश <openssl/ocsp.h>
#पूर्ण_अगर

#अगर !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#घोषणा USE_OPENSSL_ENGINE
#समावेश <openssl/engine.h>
#पूर्ण_अगर

#अगर_घोषित LIBRESSL_VERSION_संख्या
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  अगर LIBRESSL_VERSION_संख्या < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#या_अगर !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#पूर्ण_अगर

#अगर defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#समावेश <openssl/provider.h>
#समावेश <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#घोषणा OPENSSL_HAS_PROVIDERS
#पूर्ण_अगर

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#अगर !defined(LIBRESSL_VERSION_संख्या) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_कुंजी_EXPLICIT_PARAMS))
#घोषणा HAVE_SSL_CTX_SET_DEFAULT_READ_बफर_LEN 1
#पूर्ण_अगर

#अगर defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#समावेश <openssl/ui.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_OPENSSL3
#घोषणा HAVE_EVP_PKEY_GET_PARAMS 1
#पूर्ण_अगर

#अगर_घोषित HAVE_EVP_PKEY_GET_PARAMS
#समावेश <openssl/core_names.h>
#घोषणा DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = शून्य
#घोषणा FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#अन्यथा
#घोषणा DECLARE_PKEY_PARAM_BIGNUM(name) स्थिर BIGNUM *name
#घोषणा FREE_PKEY_PARAM_BIGNUM(name)
#पूर्ण_अगर

/* Whether SSL_CTX_set_ciphersuites is available.
 * BoringSSL: no
 * LibreSSL: supported since 3.4.1 (released 2021-10-14)
 * OpenSSL: supported since 1.1.1 (commit a53b5be6a05)
 */
#अगर_अघोषित OPENSSL_IS_BORINGSSL
#  define HAVE_SSL_CTX_SET_CIPHERSUITES
#  ifndef OPENSSL_IS_AWSLC
#    define HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
#  endif
#पूर्ण_अगर

/* Whether SSL_CTX_set1_sigalgs_list is available
 * BoringSSL: supported since 0.20240913.0 (commit 826ce15)
 * LibreSSL: no
 * OpenSSL: supported since 1.0.2 (commit 0b362de5f575)
 */
#अगर_अघोषित LIBRESSL_VERSION_संख्या
#घोषणा HAVE_SSL_CTX_SET1_SIGALGS
#पूर्ण_अगर

#अगर_घोषित LIBRESSL_VERSION_संख्या
#घोषणा OSSL_PACKAGE "LibreSSL"
#या_अगर defined(OPENSSL_IS_AWSLC)
#घोषणा OSSL_PACKAGE "AWS-LC"
#या_अगर defined(OPENSSL_IS_BORINGSSL)
#घोषणा OSSL_PACKAGE "BoringSSL"
#या_अगर defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#घोषणा OSSL_PACKAGE "quictls"
#अन्यथा
#घोषणा OSSL_PACKAGE "OpenSSL"
#पूर्ण_अगर

#अगर_घोषित HAVE_BORINGSSL_LIKE
प्रकार माप_प्रकार numcert_t;
प्रकार uint32_t sslerr_t;
#अन्यथा
प्रकार पूर्णांक numcert_t;
प्रकार अचिन्हित दीर्घ sslerr_t;
#पूर्ण_अगर
#घोषणा ossl_valsize_t numcert_t

#अगर OPENSSL_VERSION_संख्या >= 0x40100000L
#घोषणा ossl_ASN1_STRING_get_लंबाई ASN1_STRING_get_लंबाई
#अन्यथा
अटल माप_प्रकार ossl_ASN1_STRING_get_लंबाई(स्थिर ASN1_STRING *str)
{
   स्थिर पूर्णांक लंबाई = ASN1_STRING_लंबाई(str);
   वापस लंबाई >= 0 ? (माप_प्रकार)लंबाई : 0;
}
#पूर्ण_अगर

अटल CURLcode धकेलो_certinfo(काष्ठा Curl_easy *तथ्य,
                              BIO *mem, स्थिर अक्षर *label, पूर्णांक अंक)
  WARN_UNUSED_परिणाम;

अटल CURLcode धकेलो_certinfo(काष्ठा Curl_easy *तथ्य,
                              BIO *mem, स्थिर अक्षर *label, पूर्णांक अंक)
{
  अक्षर *ptr;
  दीर्घ len = BIO_get_mem_तथ्य(mem, &ptr);
  CURLcode परिणाम = Curl_ssl_धकेलो_certinfo_len(तथ्य, अंक, label, ptr, len);
  (व्योम)BIO_reset(mem);
  वापस परिणाम;
}

अटल CURLcode pubkey_show(काष्ठा Curl_easy *तथ्य,
                            BIO *mem,
                            पूर्णांक अंक,
                            स्थिर अक्षर *type,
                            स्थिर अक्षर *name,
                            स्थिर BIGNUM *bn) WARN_UNUSED_परिणाम;

अटल CURLcode pubkey_show(काष्ठा Curl_easy *तथ्य,
                            BIO *mem,
                            पूर्णांक अंक,
                            स्थिर अक्षर *type,
                            स्थिर अक्षर *name,
                            स्थिर BIGNUM *bn)
{
  अक्षर namebuf[32];

  curl_msnprintf(namebuf, माप(namebuf), "%s(%s)", type, name);

  अगर(bn)
    BN_print(mem, bn);
  वापस धकेलो_certinfo(तथ्य, mem, namebuf, अंक);
}

#घोषणा print_pubkey_BN(_type, _name, अंक_2)           \
  pubkey_show(तथ्य, mem, अंक_2, #_type, #_name, _name)

अटल पूर्णांक asn1_object_dump(स्थिर ASN1_OBJECT *a, अक्षर *buf, माप_प्रकार len)
{
  पूर्णांक i = i2t_ASN1_OBJECT(buf, (पूर्णांक)len, a);
  वापस (i >= (पूर्णांक)len);  /* buffer too small */
}

अटल CURLcode X509V3_ext(काष्ठा Curl_easy *तथ्य,
                           पूर्णांक certnum,
                           स्थिर ढेर_OF(X509_EXTENSION) *extsarg)
{
  पूर्णांक i;
  CURLcode परिणाम = CURLE_OK;
#अगर_घोषित LIBRESSL_VERSION_संख्या
  ढेर_OF(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#अन्यथा
  स्थिर ढेर_OF(X509_EXTENSION) *exts = extsarg;
#पूर्ण_अगर

  अगर((पूर्णांक)sk_X509_EXTENSION_अंक(exts) <= 0)
    /* no extensions, bail out */
    वापस परिणाम;

  क्रम(i = 0; i < (पूर्णांक)sk_X509_EXTENSION_अंक(exts); i++) {
    स्थिर ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_मूल्य(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    अक्षर namebuf[128];
    BIO *bio_out = BIO_new(BIO_s_mem());

    अगर(!bio_out)
      वापस परिणाम;

    obj = X509_EXTENSION_get_object(ext);

    अगर(asn1_object_dump(obj, namebuf, माप(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    अगर(!X509V3_EXT_print(bio_out, ext, 0, 0))
      ASN1_STRING_print(bio_out,
                        (स्थिर ASN1_STRING *)X509_EXTENSION_get_तथ्य(ext));

    BIO_get_mem_ptr(bio_out, &biomem);
    परिणाम = Curl_ssl_धकेलो_certinfo_len(तथ्य, certnum, namebuf, biomem->तथ्य,
                                        biomem->लंबाई);
    BIO_free(bio_out);
    अगर(परिणाम)
      अवरोध;
  }
  वापस परिणाम;
}

अटल CURLcode get_pkey_rsa(काष्ठा Curl_easy *तथ्य,
                             EVP_PKEY *pubkey, BIO *mem, पूर्णांक i)
{
  CURLcode परिणाम = CURLE_OK;
#अगर_अघोषित HAVE_EVP_PKEY_GET_PARAMS
  RSA *rsa = EVP_PKEY_get0_RSA(pubkey);
#पूर्ण_अगर /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(n);
  DECLARE_PKEY_PARAM_BIGNUM(e);
#अगर_घोषित HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_N, &n);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_E, &e);
#अन्यथा
  RSA_get0_कुंजी(rsa, &n, &e, शून्य);
#पूर्ण_अगर /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (पूर्णांक)(n ? BN_अंक_bits(n) : 0));
  परिणाम = धकेलो_certinfo(तथ्य, mem, "RSA Public Key", i);
  अगर(!परिणाम) {
    परिणाम = print_pubkey_BN(rsa, n, i);
    अगर(!परिणाम)
      परिणाम = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  वापस परिणाम;
}

#अगर_अघोषित OPENSSL_NO_DSA
अटल CURLcode get_pkey_dsa(काष्ठा Curl_easy *तथ्य,
                             EVP_PKEY *pubkey, BIO *mem, पूर्णांक i)
{
  CURLcode परिणाम = CURLE_OK;
#अगर_अघोषित HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#पूर्ण_अगर /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_कुंजी);
#अगर_घोषित HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_कुंजी, &pub_कुंजी);
#अन्यथा
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_कुंजी(dsa, &pub_कुंजी, शून्य);
#पूर्ण_अगर /* HAVE_EVP_PKEY_GET_PARAMS */
  परिणाम = print_pubkey_BN(dsa, p, i);
  अगर(!परिणाम)
    परिणाम = print_pubkey_BN(dsa, q, i);
  अगर(!परिणाम)
    परिणाम = print_pubkey_BN(dsa, g, i);
  अगर(!परिणाम)
    परिणाम = print_pubkey_BN(dsa, pub_कुंजी, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_कुंजी);
  वापस परिणाम;
}
#पूर्ण_अगर /* !OPENSSL_NO_DSA */

अटल CURLcode get_pkey_dh(काष्ठा Curl_easy *तथ्य,
                            EVP_PKEY *pubkey, BIO *mem, पूर्णांक i)
{
  CURLcode परिणाम;
#अगर_अघोषित HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#पूर्ण_अगर /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_कुंजी);
#अगर_घोषित HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_कुंजी, &pub_कुंजी);
#अन्यथा
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_कुंजी(dh, &pub_कुंजी, शून्य);
#पूर्ण_अगर /* HAVE_EVP_PKEY_GET_PARAMS */
  परिणाम = print_pubkey_BN(dh, p, i);
  अगर(!परिणाम)
    परिणाम = print_pubkey_BN(dh, q, i);
  अगर(!परिणाम)
    परिणाम = print_pubkey_BN(dh, g, i);
  अगर(!परिणाम)
    परिणाम = print_pubkey_BN(dh, pub_कुंजी, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_कुंजी);
  वापस परिणाम;
}

#अगर_घोषित HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
प्रकार स्थिर X509_PUBKEY pubkeytype_t;
#अन्यथा
प्रकार X509_PUBKEY pubkeytype_t;
#पूर्ण_अगर

अटल CURLcode ossl_certchain(काष्ठा Curl_easy *तथ्य, SSL *ssl)
{
  CURLcode परिणाम;
  ढेर_OF(X509) *sk;
  पूर्णांक i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  अगर(!sk)
    वापस CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_अंक(sk);
  अगर(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(तथ्य, "%d certificates is more than allowed (%d)", (पूर्णांक)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  परिणाम = Curl_ssl_init_certinfo(तथ्य, (पूर्णांक)numcerts);
  अगर(परिणाम)
    वापस परिणाम;

  mem = BIO_new(BIO_s_mem());
  अगर(!mem)
    परिणाम = CURLE_OUT_OF_MEMORY;

  क्रम(i = 0; !परिणाम && (i < (पूर्णांक)numcerts); i++) {
    X509 *x = sk_X509_मूल्य(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = शून्य;
    माप_प्रकार j;
    स्थिर ASN1_BIT_STRING *psig = शून्य;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    परिणाम = धकेलो_certinfo(तथ्य, mem, "Subject", i);
    अगर(परिणाम)
      अवरोध;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    परिणाम = धकेलो_certinfo(तथ्य, mem, "Issuer", i);
    अगर(परिणाम)
      अवरोध;

    BIO_printf(mem, "%lx", (अचिन्हित दीर्घ)X509_get_version(x));
    परिणाम = धकेलो_certinfo(तथ्य, mem, "Version", i);
    अगर(परिणाम)
      अवरोध;

    {
      ASN1_INTEGER *अंक = X509_get_serial_संख्या(x);
      स्थिर अचिन्हित अक्षर *numdata = ASN1_STRING_get0_तथ्य(अंक);
      स्थिर माप_प्रकार numlen = ossl_ASN1_STRING_get_लंबाई(अंक);
      अगर(ASN1_STRING_type(अंक) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      क्रम(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    परिणाम = धकेलो_certinfo(तथ्य, mem, "Serial Number", i);
    अगर(परिणाम)
      अवरोध;

    {
      स्थिर X509_ALGOR *sigalg = शून्य;
      pubkeytype_t *xpubkey = शून्य;
      ASN1_OBJECT *pubkeyoid = शून्य;

      X509_get0_signature(&psig, &sigalg, x);
      अगर(sigalg) {
        स्थिर ASN1_OBJECT *sigalgoid = शून्य;
        X509_ALGOR_get0(&sigalgoid, शून्य, शून्य, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        परिणाम = धकेलो_certinfo(तथ्य, mem, "Signature Algorithm", i);
        अगर(परिणाम)
          अवरोध;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      अगर(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, शून्य, शून्य, शून्य, xpubkey);
        अगर(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          परिणाम = धकेलो_certinfo(तथ्य, mem, "Public Key Algorithm", i);
          अगर(परिणाम)
            अवरोध;
        }
      }

      परिणाम = X509V3_ext(तथ्य, i, X509_get0_extensions(x));
      अगर(परिणाम)
        अवरोध;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    परिणाम = धकेलो_certinfo(तथ्य, mem, "Start date", i);
    अगर(परिणाम)
      अवरोध;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    परिणाम = धकेलो_certinfo(तथ्य, mem, "Expire date", i);
    अगर(परिणाम)
      अवरोध;

    pubkey = X509_get_pubkey(x);
    अगर(!pubkey)
      infof(तथ्य, "   Unable to load public key");
    अन्यथा {
      चयन(EVP_PKEY_id(pubkey)) {
      हाल EVP_PKEY_RSA:
        परिणाम = get_pkey_rsa(तथ्य, pubkey, mem, i);
        अवरोध;

#अगर_अघोषित OPENSSL_NO_DSA
      हाल EVP_PKEY_DSA:
        परिणाम = get_pkey_dsa(तथ्य, pubkey, mem, i);
        अवरोध;
#पूर्ण_अगर

      हाल EVP_PKEY_DH:
        परिणाम = get_pkey_dh(तथ्य, pubkey, mem, i);
        अवरोध;
      }
      EVP_PKEY_free(pubkey);
    }

    अगर(!परिणाम && psig) {
      स्थिर अचिन्हित अक्षर *psigdata = ASN1_STRING_get0_तथ्य(psig);
      स्थिर माप_प्रकार psiglen = ossl_ASN1_STRING_get_लंबाई(psig);
      क्रम(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      परिणाम = धकेलो_certinfo(तथ्य, mem, "Signature", i);
    }

    अगर(!परिणाम) {
      PEM_write_bio_X509(mem, x);
      परिणाम = धकेलो_certinfo(तथ्य, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  अगर(परिणाम)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(तथ्य);

  वापस परिणाम;
}

अटल पूर्णांक ossl_bio_cf_create(BIO *bio)
{
  BIO_set_shutdown(bio, 1);
  BIO_set_init(bio, 1);
  BIO_set_तथ्य(bio, शून्य);
  वापस 1;
}

अटल पूर्णांक ossl_bio_cf_destroy(BIO *bio)
{
  अगर(!bio)
    वापस 0;
  वापस 1;
}

अटल दीर्घ ossl_bio_cf_ctrl(BIO *bio, पूर्णांक cmd, दीर्घ अंक, व्योम *ptr)
{
  काष्ठा Curl_cfilter *cf = BIO_get_तथ्य(bio);
  दीर्घ ret = 1;

  (व्योम)cf;
  (व्योम)ptr;
  चयन(cmd) {
  हाल BIO_CTRL_GET_CLOSE:
    ret = (दीर्घ)BIO_get_shutdown(bio);
    अवरोध;
  हाल BIO_CTRL_SET_CLOSE:
    BIO_set_shutdown(bio, (पूर्णांक)अंक);
    अवरोध;
  हाल BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    अवरोध;
  हाल BIO_CTRL_DUP:
    ret = 1;
    अवरोध;
  हाल BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
    वापस connssl->peer_closed;
  }
  शेष:
    ret = 0;
    अवरोध;
  }
  वापस ret;
}

अटल पूर्णांक ossl_bio_cf_out_write(BIO *bio, स्थिर अक्षर *buf, पूर्णांक blen)
{
  काष्ठा Curl_cfilter *cf = BIO_get_तथ्य(bio);
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य);
  अगर(blen < 0)
    वापस 0;

  परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य,
                             (स्थिर uint8_t *)buf, (माप_प्रकार)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(तथ्य, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (पूर्णांक)परिणाम, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_परिणाम = परिणाम;
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN)
      BIO_set_retry_write(bio);
    वापस -1;
  }
  वापस (पूर्णांक)nwritten;
}

अटल पूर्णांक ossl_bio_cf_in_read(BIO *bio, अक्षर *buf, पूर्णांक blen)
{
  काष्ठा Curl_cfilter *cf = BIO_get_तथ्य(bio);
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  माप_प्रकार nread;
  CURLcode परिणाम, r2;

  DEBUGASSERT(तथ्य);
  /* OpenSSL catches this case, so should we. */
  अगर(!buf)
    वापस 0;
  अगर(blen < 0)
    वापस 0;

  परिणाम = Curl_conn_cf_recv(cf->अग्र, तथ्य, buf, (माप_प्रकार)blen, &nread);
  CURL_TRC_CF(तथ्य, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (पूर्णांक)परिणाम, nread);
  BIO_clear_retry_flags(bio);
  octx->io_परिणाम = परिणाम;
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN)
      BIO_set_retry_read(bio);
  }
  अन्यथा {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->निवेश_pending = TRUE;
    अगर(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  अगर(!octx->x509_store_setup) {
    r2 = Curl_ssl_setup_x509_store(cf, तथ्य, octx);
    अगर(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_परिणाम = r2;
      वापस -1;
    }
    octx->x509_store_setup = TRUE;
  }
  वापस परिणाम ? -1 : (पूर्णांक)nread;
}

अटल BIO_METHOD *ossl_bio_cf_method_create(व्योम)
{
  BIO_METHOD *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  अगर(m) {
    BIO_meth_set_write(m, &ossl_bio_cf_out_write);
    BIO_meth_set_read(m, &ossl_bio_cf_in_read);
    BIO_meth_set_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_set_create(m, &ossl_bio_cf_create);
    BIO_meth_set_destroy(m, &ossl_bio_cf_destroy);
  }
  वापस m;
}

अटल व्योम ossl_bio_cf_method_free(BIO_METHOD *m)
{
  अगर(m)
    BIO_meth_free(m);
}

#अगर_अघोषित HAVE_KEYLOG_UPSTREAM
#अगर_घोषित HAVE_KEYLOG_CALLBACK
अटल व्योम ossl_keylog_callback(स्थिर SSL *ssl, स्थिर अक्षर *पंक्ति)
{
  (व्योम)ssl;

  Curl_tls_keylog_write_पंक्ति(पंक्ति);
}
#अन्यथा
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
अटल व्योम ossl_log_tls12_secret(स्थिर SSL *ssl, bool *keylog_done)
{
  स्थिर SSL_SESSION *session;
  अचिन्हित अक्षर client_random[SSL3_RANDOM_आकार];
  अचिन्हित अक्षर master_कुंजी[SSL_MAX_MASTER_कुंजी_लंबाई];
  पूर्णांक master_कुंजी_लंबाई = 0;

  ERR_set_mark();

  session = SSL_get_session(ssl);

  अगर(!session || *keylog_done) {
    ERR_निकालो_to_mark();
    वापस;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_आकार);
  master_कुंजी_लंबाई = (पूर्णांक)
    SSL_SESSION_get_master_कुंजी(session, master_कुंजी, SSL_MAX_MASTER_कुंजी_लंबाई);

  ERR_निकालो_to_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  अगर(master_कुंजी_लंबाई <= 0)
    वापस;

  *keylog_done = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        माप(client_random),
                        master_कुंजी, master_कुंजी_लंबाई);
}
#पूर्ण_अगर /* !HAVE_KEYLOG_CALLBACK */
#पूर्ण_अगर /* HAVE_KEYLOG_UPSTREAM */

अटल स्थिर अक्षर *SSL_ERROR_to_str(पूर्णांक err)
{
  चयन(err) {
  हाल SSL_ERROR_NONE:
    वापस "SSL_ERROR_NONE";
  हाल SSL_ERROR_SSL:
    वापस "SSL_ERROR_SSL";
  हाल SSL_ERROR_WANT_READ:
    वापस "SSL_ERROR_WANT_READ";
  हाल SSL_ERROR_WANT_WRITE:
    वापस "SSL_ERROR_WANT_WRITE";
  हाल SSL_ERROR_WANT_X509_LOOKUP:
    वापस "SSL_ERROR_WANT_X509_LOOKUP";
  हाल SSL_ERROR_SYSCALL:
    वापस "SSL_ERROR_SYSCALL";
  हाल SSL_ERROR_ZERO_RETURN:
    वापस "SSL_ERROR_ZERO_RETURN";
  हाल SSL_ERROR_WANT_CONNECT:
    वापस "SSL_ERROR_WANT_CONNECT";
  हाल SSL_ERROR_WANT_ACCEPT:
    वापस "SSL_ERROR_WANT_ACCEPT";
#अगर_घोषित SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  हाल SSL_ERROR_WANT_ASYNC:
    वापस "SSL_ERROR_WANT_ASYNC";
#पूर्ण_अगर
#अगर_घोषित SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  हाल SSL_ERROR_WANT_ASYNC_JOB:
    वापस "SSL_ERROR_WANT_ASYNC_JOB";
#पूर्ण_अगर
#अगर_घोषित SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  हाल SSL_ERROR_WANT_CLIENT_HELLO_CB:
    वापस "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#पूर्ण_अगर
  शेष:
    वापस "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
अटल अक्षर *ossl_strerror(अचिन्हित दीर्घ error, अक्षर *buf, माप_प्रकार आकार)
{
  माप_प्रकार len;
  DEBUGASSERT(आकार);
  *buf = '\0';

  len = Curl_ossl_version(buf, आकार);
  DEBUGASSERT(len < (आकार - 2));
  अगर(len < (आकार - 2)) {
    buf += len;
    आकार -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#अगर_घोषित HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, आकार);
#अन्यथा
  ERR_error_string_n(error, buf, आकार);
#पूर्ण_अगर

  अगर(!*buf) {
    स्थिर अक्षर *संदेश = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, आकार, संदेश, म_माप(संदेश));
  }

  वापस buf;
}

अटल पूर्णांक passwd_callback(अक्षर *buf, पूर्णांक अंक, पूर्णांक encrypting, व्योम *password)
{
  DEBUGASSERT(encrypting == 0);

  अगर(!encrypting && अंक >= 0 && password) {
    पूर्णांक klen = curlx_uztosi(म_माप((अक्षर *)password));
    अगर(अंक > klen) {
      स_नकल(buf, password, klen + 1);
      वापस klen;
    }
  }
  वापस 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
अटल bool rand_enough(व्योम)
{
  वापस RAND_status() != 0;
}

अटल CURLcode ossl_seed(काष्ठा Curl_easy *तथ्य)
{
  /* This might get called before it has been added to a multi handle */
  अगर(तथ्य->multi && तथ्य->multi->ssl_seeded)
    वापस CURLE_OK;

  अगर(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    अगर(तथ्य->multi)
      तथ्य->multi->ssl_seeded = TRUE;
    वापस CURLE_OK;
  }
  failf(तथ्य, "Insufficient randomness");
  वापस CURLE_SSL_CONNECT_ERROR;
}

#अगर_अघोषित SSL_FILETYPE_ENGINE
#घोषणा SSL_FILETYPE_ENGINE 42
#पूर्ण_अगर
#अगर_अघोषित SSL_FILETYPE_PKCS12
#घोषणा SSL_FILETYPE_PKCS12 43
#पूर्ण_अगर
#अगर_अघोषित SSL_FILETYPE_PROVIDER
#घोषणा SSL_FILETYPE_PROVIDER 44
#पूर्ण_अगर
अटल पूर्णांक ossl_do_file_type(स्थिर अक्षर *type)
{
  अगर(!type || !type[0])
    वापस SSL_FILETYPE_PEM;
  अगर(curl_strequal(type, "PEM"))
    वापस SSL_FILETYPE_PEM;
  अगर(curl_strequal(type, "DER"))
    वापस SSL_FILETYPE_ASN1;
  अगर(curl_strequal(type, "PROV"))
    वापस SSL_FILETYPE_PROVIDER;
  अगर(curl_strequal(type, "ENG"))
    वापस SSL_FILETYPE_ENGINE;
  अगर(curl_strequal(type, "P12"))
    वापस SSL_FILETYPE_PKCS12;
  वापस -1;
}

#अगर defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
अटल पूर्णांक ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  स्थिर अक्षर *password;
  चयन(UI_get_string_type(uis)) {
  हाल UIT_PROMPT:
  हाल UIT_VERIFY:
    password = (स्थिर अक्षर *)UI_get0_user_तथ्य(ui);
    अगर(password && (UI_get_निवेश_flags(uis) & UI_निवेश_FLAG_DEFAULT_PWD)) {
      UI_set_परिणाम(ui, uis, password);
      वापस 1;
    }
    FALLTHROUGH();
  शेष:
    अवरोध;
  }
  वापस UI_method_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
अटल पूर्णांक ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  चयन(UI_get_string_type(uis)) {
  हाल UIT_PROMPT:
  हाल UIT_VERIFY:
    अगर(UI_get0_user_तथ्य(ui) &&
       (UI_get_निवेश_flags(uis) & UI_निवेश_FLAG_DEFAULT_PWD)) {
      वापस 1;
    }
    FALLTHROUGH();
  शेष:
    अवरोध;
  }
  वापस UI_method_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
अटल bool is_pkcs11_uri(स्थिर अक्षर *string)
{
  वापस string && curl_strnequal(string, "pkcs11:", 7);
}

#पूर्ण_अगर

अटल CURLcode ossl_set_engine(काष्ठा Curl_easy *तथ्य, काष्ठा ossl_ctx *octx,
                                स्थिर अक्षर *name);
#अगर_घोषित OPENSSL_HAS_PROVIDERS
अटल CURLcode ossl_set_provider(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ossl_ctx *octx,
                                  स्थिर अक्षर *iname);
#पूर्ण_अगर

अटल पूर्णांक use_certificate_blob(SSL_CTX *ctx, स्थिर काष्ठा curl_blob *blob,
                                पूर्णांक type, स्थिर अक्षर *कुंजी_passwd)
{
  पूर्णांक ret = 0;
  X509 *x = शून्य;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *in = BIO_new_mem_buf(blob->तथ्य, (पूर्णांक)blob->len);
  अगर(!in)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(in, शून्य);
  }
  अन्यथा अगर(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(in, शून्य, passwd_callback, CURL_UNCONST(कुंजी_passwd));
  }
  अन्यथा {
    ret = 0;
    जाओ end;
  }

  अगर(!x) {
    ret = 0;
    जाओ end;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
end:
  X509_free(x);
  BIO_free(in);
  वापस ret;
}

अटल पूर्णांक use_privatekey_blob(SSL_CTX *ctx, स्थिर काष्ठा curl_blob *blob,
                               पूर्णांक type, स्थिर अक्षर *कुंजी_passwd)
{
  पूर्णांक ret = 0;
  EVP_PKEY *pkey = शून्य;
  BIO *in = BIO_new_mem_buf(blob->तथ्य, (पूर्णांक)blob->len);
  अगर(!in)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_Private_कुंजी(in, शून्य, passwd_callback,
                                   CURL_UNCONST(कुंजी_passwd));
  अन्यथा अगर(type == SSL_FILETYPE_ASN1)
    pkey = d2i_Private_कुंजी_bio(in, शून्य);
  अन्यथा
    जाओ end;

  अगर(!pkey)
    जाओ end;

  ret = SSL_CTX_use_Private_कुंजी(ctx, pkey);
  EVP_PKEY_free(pkey);
end:
  BIO_free(in);
  वापस ret;
}

अटल पूर्णांक use_certificate_chain_blob(SSL_CTX *ctx,
                                      स्थिर काष्ठा curl_blob *blob,
                                      स्थिर अक्षर *कुंजी_passwd)
{
  पूर्णांक ret = 0;
  X509 *x = शून्य;
  BIO *in = BIO_new_mem_buf(blob->तथ्य, (पूर्णांक)blob->len);
  अगर(!in)
    वापस CURLE_OUT_OF_MEMORY;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(in, शून्य,
                            passwd_callback, CURL_UNCONST(कुंजी_passwd));
  अगर(!x)
    जाओ end;

  ret = SSL_CTX_use_certificate(ctx, x);

  अगर(ERR_peek_error() != 0)
    ret = 0;

  अगर(ret) {
    X509 *ca;
    sslerr_t err;

    अगर(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      जाओ end;
    }

    जबतक((ca = PEM_read_bio_X509(in, शून्य, passwd_callback,
                                  CURL_UNCONST(कुंजी_passwd))) != शून्य) {

      अगर(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        जाओ end;
      }
    }

    err = ERR_peek_last_error();
    अगर((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_पंक्ति))
      ERR_clear_error();
    अन्यथा
      ret = 0;
  }

end:
  X509_free(x);
  BIO_free(in);
  वापस ret;
}

अटल पूर्णांक enginecheck(काष्ठा Curl_easy *तथ्य,
                       काष्ठा ossl_ctx *octx,
                       स्थिर अक्षर *कुंजी_file,
                       स्थिर अक्षर *कुंजी_passwd)
{
#अगर_घोषित USE_OPENSSL_ENGINE
  EVP_PKEY *priv_कुंजी = शून्य;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  अगर(!octx->p.engine && is_pkcs11_uri(कुंजी_file) &&
     ossl_set_engine(तथ्य, octx, "pkcs11") != CURLE_OK)
    वापस 0;

  अगर(octx->p.engine) {
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    अगर(!ui_method) {
      failf(तथ्य, "unable to create " OSSL_PACKAGE " user-interface method");
      वापस 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);
    priv_कुंजी = ENGINE_load_private_कुंजी(octx->p.engine, कुंजी_file,
                                       ui_method,
                                       CURL_UNCONST(कुंजी_passwd));
    UI_destroy_method(ui_method);
    अगर(!priv_कुंजी) {
      failf(तथ्य, "failed to load private key from crypto engine");
      वापस 0;
    }
    अगर(SSL_CTX_use_Private_कुंजी(ctx, priv_कुंजी) != 1) {
      failf(तथ्य, "unable to set private key");
      EVP_PKEY_free(priv_कुंजी);
      वापस 0;
    }
    EVP_PKEY_free(priv_कुंजी);  /* we do not need the handle any more... */
  }
  अन्यथा {
    failf(तथ्य, "crypto engine not set, cannot load private key");
    वापस 0;
  }
  वापस 1;
#अन्यथा
  (व्योम)octx;
  (व्योम)कुंजी_file;
  (व्योम)कुंजी_passwd;
  failf(तथ्य, "SSL_FILETYPE_ENGINE not supported for private key");
  वापस 0;
#पूर्ण_अगर
}

अटल पूर्णांक providercheck(काष्ठा Curl_easy *तथ्य,
                         काष्ठा ossl_ctx *octx,
                         स्थिर अक्षर *कुंजी_file)
{
#अगर_घोषित OPENSSL_HAS_PROVIDERS
  अक्षर error_बफर[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  अगर(!octx->p.provider_loaded && is_pkcs11_uri(कुंजी_file) &&
     ossl_set_provider(तथ्य, octx, "pkcs11") != CURLE_OK) {
    वापस 0;
  }

  अगर(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_कुंजी = शून्य;
    OSSL_STORE_CTX *store = शून्य;
    OSSL_STORE_INFO *info = शून्य;
    UI_METHOD *ui_method = UI_create_method("curl user interface");
    अगर(!ui_method) {
      failf(तथ्य, "unable to create " OSSL_PACKAGE " user-interface method");
      वापस 0;
    }
    UI_method_set_opener(ui_method, UI_method_get_opener(UI_OpenSSL()));
    UI_method_set_closer(ui_method, UI_method_get_closer(UI_OpenSSL()));
    UI_method_set_reader(ui_method, ssl_ui_reader);
    UI_method_set_writer(ui_method, ssl_ui_writer);

    store = OSSL_STORE_open_ex(कुंजी_file, octx->p.libctx,
                               octx->p.propq, ui_method, शून्य, शून्य,
                               शून्य, शून्य);
    अगर(!store) {
      failf(तथ्य, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      UI_destroy_method(ui_method);
      वापस 0;
    }
    अगर(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(तथ्य, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
    }

    info = OSSL_STORE_load(store);
    अगर(info) {
      पूर्णांक ossl_type = OSSL_STORE_INFO_get_type(info);

      अगर(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_कुंजी = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_method(ui_method);
    अगर(!priv_कुंजी) {
      failf(तथ्य, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      वापस 0;
    }

    अगर(SSL_CTX_use_Private_कुंजी(ctx, priv_कुंजी) != 1) {
      failf(तथ्य, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      EVP_PKEY_free(priv_कुंजी);
      वापस 0;
    }
    EVP_PKEY_free(priv_कुंजी); /* we do not need the handle any more... */
  }
  अन्यथा {
    failf(तथ्य, "crypto provider not set, cannot load private key");
    वापस 0;
  }
  वापस 1;
#अन्यथा
  (व्योम)octx;
  (व्योम)कुंजी_file;
  failf(तथ्य, "SSL_FILETYPE_PROVIDER not supported for private key");
  वापस 0;
#पूर्ण_अगर
}

अटल पूर्णांक engineload(काष्ठा Curl_easy *तथ्य,
                      काष्ठा ossl_ctx *octx,
                      स्थिर अक्षर *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#अगर defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  अक्षर error_बफर[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  अगर(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_set_engine(तथ्य, octx, "pkcs11") != CURLE_OK)
    वापस 0;

  अगर(octx->p.engine) {
    अटल स्थिर अक्षर cmd_name[] = "LOAD_CERT_CTRL";
    काष्ठा {
      स्थिर अक्षर *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = शून्य;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    अगर(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), शून्य)) {
      failf(तथ्य, "SSL engine does not support loading certificates");
      वापस 0;
    }

    /* Load the certificate from the engine */
    अगर(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, शून्य, 1)) {
      failf(तथ्य, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      वापस 0;
    }

    अगर(!params.cert) {
      failf(तथ्य, "SSL engine did not initialize the certificate properly.");
      वापस 0;
    }

    अगर(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(तथ्य, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      X509_free(params.cert);
      वापस 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  अन्यथा {
    failf(तथ्य, "crypto engine not set, cannot load certificate");
    वापस 0;
  }
  वापस 1;
#अन्यथा
  (व्योम)octx;
  (व्योम)cert_file;
  failf(तथ्य, "SSL_FILETYPE_ENGINE not supported for certificate");
  वापस 0;
#पूर्ण_अगर
}

अटल पूर्णांक providerload(काष्ठा Curl_easy *तथ्य,
                        काष्ठा ossl_ctx *octx,
                        स्थिर अक्षर *cert_file)
{
#अगर_घोषित OPENSSL_HAS_PROVIDERS
  अक्षर error_बफर[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  अगर(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_set_provider(तथ्य, octx, "pkcs11") != CURLE_OK)
    वापस 0;

  अगर(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = शून्य;
    X509 *cert = शून्य;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         शून्य, शून्य, शून्य, शून्य, शून्य, शून्य);
    पूर्णांक rc;

    अगर(!store) {
      failf(तथ्य, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      वापस 0;
    }
    अगर(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(तथ्य, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
    }

    info = OSSL_STORE_load(store);
    अगर(info) {
      पूर्णांक ossl_type = OSSL_STORE_INFO_get_type(info);

      अगर(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    अगर(!cert) {
      failf(तथ्य, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      वापस 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    अगर(rc != 1) {
      failf(तथ्य, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      वापस 0;
    }
  }
  अन्यथा {
    failf(तथ्य, "crypto provider not set, cannot load certificate");
    वापस 0;
  }
  वापस 1;
#अन्यथा
  (व्योम)octx;
  (व्योम)cert_file;
  failf(तथ्य, "SSL_FILETYPE_PROVIDER not supported for certificate");
  वापस 0;
#पूर्ण_अगर
}

अटल पूर्णांक pkcs12load(काष्ठा Curl_easy *तथ्य,
                      SSL_CTX* ctx,
                      स्थिर काष्ठा curl_blob *cert_blob,
                      स्थिर अक्षर *cert_file,
                      स्थिर अक्षर *कुंजी_passwd)
{
  अक्षर error_बफर[256];
  BIO *cert_bio = शून्य;
  PKCS12 *p12 = शून्य;
  EVP_PKEY *pri;
  X509 *x509;
  पूर्णांक cert_done = 0;
  ढेर_OF(X509) *ca = शून्य;
  अगर(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->तथ्य, (पूर्णांक)cert_blob->len);
    अगर(!cert_bio) {
      failf(तथ्य, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      वापस 0;
    }
  }
  अन्यथा {
    cert_bio = BIO_new(BIO_s_file());
    अगर(!cert_bio) {
      failf(तथ्य, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
      वापस 0;
    }

    अगर(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(तथ्य, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      वापस 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, शून्य);
  BIO_free(cert_bio);

  अगर(!p12) {
    failf(तथ्य, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    वापस 0;
  }

  अगर(!PKCS12_parse(p12, कुंजी_passwd, &pri, &x509, &ca)) {
    failf(तथ्य, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_बफर, माप(error_बफर)));
    PKCS12_free(p12);
    वापस 0;
  }

  PKCS12_free(p12);

  अगर(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(तथ्य, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_बफर, माप(error_बफर)));
    जाओ fail;
  }

  अगर(SSL_CTX_use_Private_कुंजी(ctx, pri) != 1) {
    failf(तथ्य, "unable to use private key from PKCS12 file '%s'", cert_file);
    जाओ fail;
  }

  अगर(!SSL_CTX_check_private_कुंजी(ctx)) {
    failf(तथ्य, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    जाओ fail;
  }
  /* Set Certificate Verification chain */
  अगर(ca) {
    जबतक(sk_X509_अंक(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_निकालो(ca);
      अगर(!SSL_CTX_add_client_CA(ctx, x)) {
        X509_free(x);
        failf(तथ्य, "cannot add certificate to client CA list");
        जाओ fail;
      }
      अगर(!SSL_CTX_add_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(तथ्य, "cannot add certificate to certificate chain");
        जाओ fail;
      }
    }
  }

  cert_done = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wcast-function-type-strict"
#पूर्ण_अगर
  sk_X509_निकालो_free(ca, X509_free);
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic निकालो
#पूर्ण_अगर
  अगर(!cert_done)
    वापस 0; /* failure! */
  वापस 1;
}

अटल CURLcode client_cert(काष्ठा Curl_easy *तथ्य,
                            काष्ठा ossl_ctx *octx,
                            अक्षर *cert_file,
                            स्थिर काष्ठा curl_blob *cert_blob,
                            स्थिर अक्षर *cert_type,
                            अक्षर *कुंजी_file,
                            स्थिर काष्ठा curl_blob *कुंजी_blob,
                            स्थिर अक्षर *कुंजी_type,
                            अक्षर *कुंजी_passwd)
{
  अक्षर error_बफर[256];
  bool check_privkey = TRUE;
  पूर्णांक file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  अगर(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_done = FALSE;
    पूर्णांक cert_use_परिणाम;

    अगर(कुंजी_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_set_default_passwd_cb_userdata(ctx, कुंजी_passwd);
      /* Set passwd callback: */
      SSL_CTX_set_default_passwd_cb(ctx, passwd_callback);
    }

    चयन(file_type) {
    हाल SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_परिणाम = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, कुंजी_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      अगर(cert_use_परिणाम != 1) {
        failf(तथ्य,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_बफर,
                            माप(error_बफर)));
        वापस CURLE_SSL_CERTPROBLEM;
      }
      अवरोध;

    हाल SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_परिणाम = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, कुंजी_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      अगर(cert_use_परिणाम != 1) {
        failf(तथ्य,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_बफर,
                            माप(error_बफर)));
        वापस CURLE_SSL_CERTPROBLEM;
      }
      अवरोध;

    हाल SSL_FILETYPE_ENGINE:
      अगर(!cert_file || !engineload(तथ्य, octx, cert_file))
        वापस CURLE_SSL_CERTPROBLEM;
      अवरोध;

    हाल SSL_FILETYPE_PROVIDER:
      अगर(!cert_file || !providerload(तथ्य, octx, cert_file))
        वापस CURLE_SSL_CERTPROBLEM;
      अवरोध;

    हाल SSL_FILETYPE_PKCS12:
      अगर(!pkcs12load(तथ्य, ctx, cert_blob, cert_file, कुंजी_passwd))
        वापस CURLE_SSL_CERTPROBLEM;
      pcks12_done = TRUE;
      अवरोध;

    शेष:
      failf(तथ्य, "not supported file type '%s' for certificate", cert_type);
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }

    अगर(!कुंजी_file && !कुंजी_blob) {
      कुंजी_file = cert_file;
      कुंजी_blob = cert_blob;
    }
    अन्यथा
      file_type = ossl_do_file_type(कुंजी_type);

    चयन(file_type) {
    हाल SSL_FILETYPE_PEM:
    हाल SSL_FILETYPE_ASN1:
      cert_use_परिणाम = कुंजी_blob ?
        use_privatekey_blob(ctx, कुंजी_blob, file_type, कुंजी_passwd) :
      SSL_CTX_use_Private_कुंजी_file(ctx, कुंजी_file, file_type);
      अगर(cert_use_परिणाम != 1) {
        failf(तथ्य, "unable to set private key file: '%s' type %s",
              कुंजी_file ? कुंजी_file : "(memory blob)",
              कुंजी_type ? कुंजी_type : "PEM");
        वापस CURLE_BAD_FUNCTION_ARGUMENT;
      }
      अवरोध;
    हाल SSL_FILETYPE_ENGINE:
      अगर(!enginecheck(तथ्य, octx, कुंजी_file, कुंजी_passwd))
        वापस CURLE_SSL_CERTPROBLEM;
      अवरोध;

    हाल SSL_FILETYPE_PROVIDER:
      अगर(!providercheck(तथ्य, octx, कुंजी_file))
        वापस CURLE_SSL_CERTPROBLEM;
      अवरोध;

    हाल SSL_FILETYPE_PKCS12:
      अगर(!pcks12_done) {
        failf(तथ्य, "file type P12 for private key not supported");
        वापस CURLE_SSL_CERTPROBLEM;
      }
      अवरोध;
    शेष:
      failf(तथ्य, "not supported file type for private key");
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }

    ssl = SSL_new(ctx);
    अगर(!ssl) {
      failf(तथ्य, "unable to create an SSL structure");
      वापस CURLE_OUT_OF_MEMORY;
    }

    x509 = SSL_get_certificate(ssl);

    अगर(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#अगर !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_कुंजी = SSL_get_privatekey(ssl);
      अगर(EVP_PKEY_id(priv_कुंजी) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_कुंजी);
        अगर(RSA_flags(rsa) & RSA_METHOD_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#पूर्ण_अगर

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    अगर(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      अगर(!SSL_CTX_check_private_कुंजी(ctx)) {
        failf(तथ्य, "Private key does not match the certificate public key");
        वापस CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  वापस CURLE_OK;
}

#अगर_घोषित CURLVERBOSE
/* returns non-zero on failure */
अटल CURLcode x509_name_oneline(स्थिर X509_NAME *a, काष्ठा dynbuf *d)
{
  BIO *bio_out = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  पूर्णांक rc;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  अगर(bio_out) {
    अचिन्हित दीर्घ flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_out, a, 0, flags);
    अगर(rc != -1) {
      BIO_get_mem_ptr(bio_out, &biomem);
      परिणाम = curlx_dyn_addn(d, biomem->तथ्य, biomem->लंबाई);
    }
    BIO_free(bio_out);
  }
  वापस परिणाम;
}
#पूर्ण_अगर

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
अटल पूर्णांक ossl_init(व्योम)
{
  स्थिर uint64_t flags =
#अगर_घोषित OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#पूर्ण_अगर
#अगर_घोषित CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
    OPENSSL_INIT_NO_LOAD_CONFIG |
#अन्यथा
    OPENSSL_INIT_LOAD_CONFIG |
#पूर्ण_अगर
    0;
  OPENSSL_init_ssl(flags, शून्य);

#अगर_अघोषित HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#पूर्ण_अगर

  वापस 1;
}

/* Global cleanup */
अटल व्योम ossl_cleanup(व्योम)
{
#अगर_अघोषित HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#पूर्ण_अगर
}

/* Selects an OpenSSL crypto engine or provider.
 */
अटल CURLcode ossl_set_engine(काष्ठा Curl_easy *तथ्य, काष्ठा ossl_ctx *octx,
                                स्थिर अक्षर *name)
{
#अगर_घोषित USE_OPENSSL_ENGINE
  CURLcode परिणाम = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  अगर(e) {

    अगर(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = शून्य;
    }
    अगर(!ENGINE_init(e)) {
      अक्षर buf[256];

      ENGINE_free(e);
      failf(तथ्य, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, माप(buf)));
      परिणाम = CURLE_SSL_ENGINE_INITFAILED;
      e = शून्य;
    }
    अन्यथा {
      परिणाम = CURLE_OK;
    }
    octx->p.engine = e;
    वापस परिणाम;
  }
#पूर्ण_अगर
#अगर_घोषित OPENSSL_HAS_PROVIDERS
  वापस ossl_set_provider(तथ्य, octx, name);
#अन्यथा
  (व्योम)octx;
  (व्योम)name;
  failf(तथ्य, "OpenSSL engine not found");
  वापस CURLE_SSL_ENGINE_NOTFOUND;
#पूर्ण_अगर
}

/* Sets engine as default for all SSL operations
 */
अटल CURLcode ossl_set_engine_default(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ossl_ctx *octx)
{
#अगर_घोषित USE_OPENSSL_ENGINE
  अगर(octx->p.engine) {
    अगर(ENGINE_set_default(octx->p.engine, ENGINE_METHOD_ALL) > 0) {
      infof(तथ्य, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    अन्यथा {
      failf(तथ्य, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      वापस CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)octx;
#पूर्ण_अगर
  वापस CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
अटल काष्ठा curl_slist *ossl_engines_list(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा curl_slist *list = शून्य;
#अगर_घोषित USE_OPENSSL_ENGINE
  काष्ठा curl_slist *beg;
  ENGINE *e;

  क्रम(e = ENGINE_get_first(); e; e = ENGINE_get_अग्र(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    अगर(!beg) {
      curl_slist_free_all(list);
      वापस शून्य;
    }
    list = beg;
  }
#पूर्ण_अगर
  (व्योम)तथ्य;
  वापस list;
}

#अगर_घोषित OPENSSL_HAS_PROVIDERS

अटल व्योम ossl_provider_cleanup(काष्ठा ossl_ctx *octx)
{
  अगर(!octx)
    वापस;
  अगर(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = शून्य;
  }
  अगर(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = शून्य;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = शून्य;
  curlx_safefree(octx->p.propq);
  octx->p.provider_loaded = FALSE;
}

#घोषणा MAX_PROVIDER_LEN 128 /* reasonable */

/* Selects an OpenSSL crypto provider.
 *
 * A provider might need an associated property, a string passed on to
 * OpenSSL. Specify this as [PROVIDER][:PROPERTY]: separate the name and the
 * property with a colon. No colon means no property is set.
 *
 * An example provider + property looks like "tpm2:?provider=tpm2".
 */
अटल CURLcode ossl_set_provider(काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ossl_ctx *octx,
                                  स्थिर अक्षर *iname)
{
  अक्षर name[MAX_PROVIDER_LEN + 1];
  काष्ठा Curl_str prov;
  स्थिर अक्षर *propq = शून्य;

  अगर(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    वापस CURLE_OK;
  }
  अगर(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  स_नकल(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  अगर(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    अगर(!libctx)
      वापस CURLE_OUT_OF_MEMORY;
    अगर(propq) {
      octx->p.propq = curlx_strdup(propq);
      अगर(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        वापस CURLE_OUT_OF_MEMORY;
      }
    }
    octx->p.libctx = libctx;
  }

#अगर_अघोषित CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  अगर(!OSSL_LIB_CTX_load_config(octx->p.libctx, शून्य)) {
    infof(तथ्य, "Failed to load default openssl config. Proceeding.");
  }
#पूर्ण_अगर

  अगर(OSSL_PROVIDER_available(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    वापस CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_load(octx->p.libctx, name, 1);
  अगर(!octx->p.provider) {
    अक्षर error_बफर[256];
    failf(तथ्य, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_बफर, माप(error_बफर)));
    ossl_provider_cleanup(octx);
    वापस CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_load(octx->p.libctx, "base", 1);
  अगर(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(तथ्य, "Failed to load base");
    वापस CURLE_SSL_ENGINE_NOTFOUND;
  }
  अन्यथा
    octx->p.provider_loaded = TRUE;
  वापस CURLE_OK;
}
#अन्यथा /* OPENSSL_HAS_PROVIDERS */
#घोषणा ossl_provider_cleanup(x)
#पूर्ण_अगर

अटल CURLcode ossl_shutdown(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              bool send_shutdown, bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  अक्षर buf[1024];
  पूर्णांक nread = -1, err;
  माप_प्रकार i;

  DEBUGASSERT(octx);
  अगर(!octx->ssl || cf->shutdown) {
    *done = TRUE;
    जाओ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;
  अगर(!(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    क्रम(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (पूर्णांक)माप(buf));
      CURL_TRC_CF(तथ्य, cf, "SSL shutdown not sent, read -> %d", nread);
      अगर(nread <= 0)
        अवरोध;
    }
    err = SSL_get_error(octx->ssl, nread);
    अगर(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool निवेश_pending;
      /* Yes, it did. */
      अगर(!send_shutdown) {
        CURL_TRC_CF(तथ्य, cf, "SSL shutdown received, not sending");
        *done = TRUE;
        जाओ out;
      }
      अन्यथा अगर(!cf->अग्र->cft->is_alive(cf->अग्र, तथ्य, &निवेश_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(तथ्य, cf, "peer closed connection");
        *done = TRUE;
        जाओ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  अगर(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_SENT_SHUTDOWN)) {
    पूर्णांक rc;
    ERR_clear_error();
    CURL_TRC_CF(तथ्य, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    अगर(rc == 1) {
      CURL_TRC_CF(तथ्य, cf, "SSL shutdown finished");
      *done = TRUE;
      जाओ out;
    }
    अगर(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(तथ्य, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      जाओ out;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  क्रम(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (पूर्णांक)माप(buf));
    CURL_TRC_CF(तथ्य, cf, "SSL shutdown read -> %d", nread);
    अगर(nread <= 0)
      अवरोध;
  }
  err = SSL_get_error(octx->ssl, nread);
  चयन(err) {
  हाल SSL_ERROR_ZERO_RETURN: /* no more data */
    अगर(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(तथ्य, cf, "SSL shutdown finished");
    अन्यथा
      CURL_TRC_CF(तथ्य, cf, "SSL shutdown not received, but closed");
    *done = TRUE;
    अवरोध;
  हाल SSL_ERROR_NONE: /* did not get anything */
  हाल SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(तथ्य, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    अवरोध;
  हाल SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(तथ्य, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    अवरोध;
  शेष:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(अचिन्हित दीर्घ sslerr = ERR_get_error());
      CURL_TRC_CF(तथ्य, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, माप(buf)) :
                   SSL_ERROR_to_str(err)),
                  SOCKERRNO);
    }
    *done = TRUE;
    परिणाम = CURLE_OK;
    अवरोध;
  }

out:
  cf->shutdown = (परिणाम || *done);
  अगर(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->निवेश_pending = FALSE;
  वापस परिणाम;
}

/* also used by quic-tls */
व्योम Curl_osslctx_cleanup(काष्ठा ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  अगर(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = शून्य;
  }
  अगर(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = शून्य;
    octx->x509_store_setup = FALSE;
  }
  अगर(octx->bio_method) {
    ossl_bio_cf_method_free(octx->bio_method);
    octx->bio_method = शून्य;
  }
#अगर_घोषित USE_OPENSSL_ENGINE
  अगर(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = शून्य;
  }
#पूर्ण_अगर
  ossl_provider_cleanup(octx);
}

अटल व्योम ossl_close(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  (व्योम)तथ्य;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
अटल व्योम ossl_close_all(काष्ठा Curl_easy *तथ्य)
{
  (व्योम)तथ्य;
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
अटल CURLcode ossl_verifyhost(काष्ठा Curl_easy *तथ्य,
                                काष्ठा connectdata *conn,
                                काष्ठा ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  पूर्णांक target; /* target type, GEN_DNS or GEN_IPADD */
  माप_प्रकार addrlen = 0;
  ढेर_OF(GENERAL_NAME) *altnames;
#अगर_घोषित USE_IPV6
  काष्ठा in6_addr addr;
#अन्यथा
  काष्ठा in_addr addr;
#पूर्ण_अगर
  CURLcode परिणाम = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  माप_प्रकार hostlen = म_माप(peer->origin->hostname);

  (व्योम)conn;
  चयन(peer->type) {
  हाल CURL_SSL_PEER_IPV4:
    अगर(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      वापस CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = माप(काष्ठा in_addr);
    अवरोध;
#अगर_घोषित USE_IPV6
  हाल CURL_SSL_PEER_IPV6:
    अगर(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      वापस CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = माप(काष्ठा in6_addr);
    अवरोध;
#पूर्ण_अगर
  हाल CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    अवरोध;
  शेष:
    DEBUGASSERT(0);
    failf(तथ्य, "unexpected SSL peer type: %d", (पूर्णांक)peer->type);
    वापस CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, शून्य, शून्य);

  अगर(altnames) {
#अगर_घोषित HAVE_BORINGSSL_LIKE
    माप_प्रकार numalts;
    माप_प्रकार i;
#अन्यथा
    पूर्णांक numalts;
    पूर्णांक i;
#पूर्ण_अगर

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_अंक(altnames);

    /* loop through all alternatives - until a dnsmatch */
    क्रम(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      स्थिर GENERAL_NAME *check = sk_GENERAL_NAME_मूल्य(altnames, i);

      अगर(check->type == GEN_DNS)
        dNSName = TRUE;
      अन्यथा अगर(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      अगर(check->type == target) {
        /* get data and length */
        स्थिर अक्षर *altptr = (स्थिर अक्षर *)ASN1_STRING_get0_तथ्य(check->d.ia5);
        माप_प्रकार altlen = ossl_ASN1_STRING_get_लंबाई(check->d.ia5);

        चयन(target) {
        हाल GEN_DNS: /* name/pattern comparison */
          अगर(!स_प्रथम(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(तथ्य, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (पूर्णांक)altlen, altptr);
          }
          अवरोध;

        हाल GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          अगर((altlen == addrlen) && !स_भेद(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(तथ्य, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          अवरोध;
        }
      }
    }
    GENERAL_NAMES_free(altnames);
  }

  अगर(matched)
    /* an alternative name matched */
    ;
  अन्यथा अगर(dNSName || iPAddress) {
    स्थिर अक्षर *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(तथ्य, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(तथ्य, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
  }
  अन्यथा {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    पूर्णांक i = -1;
    अचिन्हित अक्षर *cn = शून्य;
    माप_प्रकार cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    स्थिर X509_NAME *name = X509_get_subject_name(server_cert);
    अगर(name) {
      पूर्णांक j;
      जबतक((j = X509_NAME_get_सूचकांक_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    अगर(i >= 0) {
      स्थिर ASN1_STRING *tmp =
        X509_NAME_ENTRY_get_तथ्य(X509_NAME_get_entry(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      अगर(tmp) {
        अगर(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_लंबाई(tmp);
          cn = (अचिन्हित अक्षर *)CURL_UNCONST(ASN1_STRING_get0_तथ्य(tmp));
        }
        अन्यथा { /* not a UTF8 name */
          स्थिर पूर्णांक utf8len = ASN1_STRING_to_UTF8(&cn, tmp);
          अगर(utf8len < 0)
            cnlen = 0;
          अन्यथा
            cnlen = (माप_प्रकार)utf8len;
          free_cn = TRUE;
        }

        अगर((cnlen == 0) || !cn)
          परिणाम = CURLE_OUT_OF_MEMORY;
        अन्यथा अगर(स_प्रथम(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(तथ्य, "SSL: illegal cert name field");
          परिणाम = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    अगर(परिणाम)
      /* error already detected, pass through */
      ;
    अन्यथा अगर(!cn) {
      failf(तथ्य, "SSL: unable to obtain common name from peer certificate");
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
    अन्यथा अगर(!Curl_cert_hostcheck((स्थिर अक्षर *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(तथ्य, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (पूर्णांक)cnlen, cn,
            peer->origin->user_hostname);
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
    अन्यथा {
      infof(तथ्य, " common name: %.*s (matched)", (पूर्णांक)cnlen, cn);
    }
    अगर(free_cn)
      OPENSSL_free(cn);
  }

  वापस परिणाम;
}

#अगर_अघोषित OPENSSL_NO_OCSP
अटल CURLcode verifystatus(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             काष्ठा ossl_ctx *octx)
{
  पूर्णांक i, ocsp_status;
#अगर_घोषित HAVE_BORINGSSL_LIKE
  स्थिर uint8_t *status;
#अन्यथा
  अचिन्हित अक्षर *status;
#पूर्ण_अगर
  स्थिर अचिन्हित अक्षर *p;
  CURLcode परिणाम = CURLE_OK;
  OCSP_RESPONSE *rsp = शून्य;
  OCSP_BASICRESP *br = शून्य;
  X509_STORE     *st = शून्य;
  ढेर_OF(X509) *ch = शून्य;
  X509 *cert;
  OCSP_CERTID *id = शून्य;
  पूर्णांक cert_status, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  पूर्णांक ret;
  दीर्घ len;

  (व्योम)cf;
  DEBUGASSERT(octx);

  len = (दीर्घ)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &status);

  अगर(!status) {
    failf(तथ्य, "No OCSP response received");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }
  p = status;
  rsp = d2i_OCSP_RESPONSE(शून्य, &p, len);
  अगर(!rsp) {
    failf(तथ्य, "Invalid OCSP response");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }

  ocsp_status = OCSP_response_status(rsp);
  अगर(ocsp_status != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
    failf(तथ्य, "Invalid OCSP response status: %s (%d)",
          OCSP_response_status_str(ocsp_status), ocsp_status);
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }

  br = OCSP_response_get1_basic(rsp);
  अगर(!br) {
    failf(तथ्य, "Invalid OCSP response");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }

#अगर !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_संख्या) && LIBRESSL_VERSION_संख्या < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#अन्यथा
  ch = SSL_get_peer_cert_chain(octx->ssl);
#पूर्ण_अगर
  अगर(!ch) {
    failf(तथ्य, "Could not get peer certificate chain");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  अगर(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(तथ्य, "OCSP response verification failed");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  अगर(!cert) {
    failf(तथ्य, "Error getting peer certificate");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }

  क्रम(i = 0; i < (पूर्णांक)sk_X509_अंक(ch); i++) {
    X509 *issuer = sk_X509_मूल्य(ch, (ossl_valsize_t)i);
    अगर(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_to_id(EVP_sha1(), cert, issuer);
      अवरोध;
    }
  }
  X509_free(cert);

  अगर(!id) {
    failf(तथ्य, "Error computing OCSP ID");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_status(br, id, &cert_status, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  अगर(ret != 1) {
    failf(तथ्य, "Could not find certificate ID in OCSP response");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  अगर(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(तथ्य, "OCSP response has expired");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }

  infof(तथ्य, "SSL certificate status: %s (%d)",
        OCSP_cert_status_str(cert_status), cert_status);

  चयन(cert_status) {
  हाल V_OCSP_CERTSTATUS_GOOD:
    अवरोध;

  हाल V_OCSP_CERTSTATUS_REVOKED:
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    failf(तथ्य, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    जाओ end;

  हाल V_OCSP_CERTSTATUS_UNKNOWN:
  शेष:
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ end;
  }

end:
  अगर(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  वापस परिणाम;
}
#पूर्ण_अगर

अटल स्थिर अक्षर *ssl_संदेश_type(पूर्णांक ssl_ver, पूर्णांक संदेश)
{
  अगर(ssl_ver == SSL3_VERSION_MAJOR) {
    चयन(संदेश) {
    हाल SSL3_MT_HELLO_REQUEST:
      वापस "Hello request";
    हाल SSL3_MT_CLIENT_HELLO:
      वापस "Client hello";
    हाल SSL3_MT_SERVER_HELLO:
      वापस "Server hello";
#अगर_घोषित SSL3_MT_NEWSESSION_TICKET
    हाल SSL3_MT_NEWSESSION_TICKET:
      वापस "Newsession Ticket";
#पूर्ण_अगर
    हाल SSL3_MT_CERTIFICATE:
      वापस "Certificate";
    हाल SSL3_MT_SERVER_कुंजी_EXCHANGE:
      वापस "Server key exchange";
    हाल SSL3_MT_CLIENT_कुंजी_EXCHANGE:
      वापस "Client key exchange";
    हाल SSL3_MT_CERTIFICATE_REQUEST:
      वापस "Request CERT";
    हाल SSL3_MT_SERVER_DONE:
      वापस "Server finished";
    हाल SSL3_MT_CERTIFICATE_VERIFY:
      वापस "CERT verify";
    हाल SSL3_MT_FINISHED:
      वापस "Finished";
#अगर_घोषित SSL3_MT_CERTIFICATE_STATUS
    हाल SSL3_MT_CERTIFICATE_STATUS:
      वापस "Certificate Status";
#पूर्ण_अगर
#अगर_घोषित SSL3_MT_ENCRYPTED_EXTENSIONS
    हाल SSL3_MT_ENCRYPTED_EXTENSIONS:
      वापस "Encrypted Extensions";
#पूर्ण_अगर
#अगर_घोषित SSL3_MT_SUPPLEMENTAL_तथ्य
    हाल SSL3_MT_SUPPLEMENTAL_तथ्य:
      वापस "Supplemental data";
#पूर्ण_अगर
#अगर_घोषित SSL3_MT_END_OF_EARLY_तथ्य
    हाल SSL3_MT_END_OF_EARLY_तथ्य:
      वापस "End of early data";
#पूर्ण_अगर
#अगर_घोषित SSL3_MT_कुंजी_UPDATE
    हाल SSL3_MT_कुंजी_UPDATE:
      वापस "Key update";
#पूर्ण_अगर
#अगर_घोषित SSL3_MT_अग्र_PROTO
    हाल SSL3_MT_अग्र_PROTO:
      वापस "Next protocol";
#पूर्ण_अगर
#अगर_घोषित SSL3_MT_MESSAGE_HASH
    हाल SSL3_MT_MESSAGE_HASH:
      वापस "Message hash";
#पूर्ण_अगर
    }
  }
  वापस "Unknown";
}

अटल स्थिर अक्षर *tls_rt_type(पूर्णांक type)
{
  चयन(type) {
#अगर_घोषित SSL3_RT_HEADER
  हाल SSL3_RT_HEADER:
    वापस "TLS header";
#पूर्ण_अगर
  हाल SSL3_RT_CHANGE_CIPHER_SPEC:
    वापस "TLS change cipher";
  हाल SSL3_RT_ALERT:
    वापस "TLS alert";
  हाल SSL3_RT_HANDSHAKE:
    वापस "TLS handshake";
  हाल SSL3_RT_APPLICATION_तथ्य:
    वापस "TLS app data";
  शेष:
    वापस "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
अटल व्योम ossl_trace(पूर्णांक direction, पूर्णांक ssl_ver, पूर्णांक content_type,
                       स्थिर व्योम *buf, माप_प्रकार len, SSL *ssl,
                       व्योम *userp)
{
  स्थिर अक्षर *verstr;
  काष्ठा Curl_cfilter *cf = userp;
  काष्ठा Curl_easy *तथ्य = शून्य;
  अक्षर unknown[32];

  अगर(!cf)
    वापस;
  तथ्य = CF_तथ्य_CURRENT(cf);
  अगर(!तथ्य || !तथ्य->set.fdebug || (direction && direction != 1))
    वापस;

  चयन(ssl_ver) {
#अगर_घोषित SSL3_VERSION
  हाल SSL3_VERSION:
    verstr = "SSLv3";
    अवरोध;
#पूर्ण_अगर
  हाल TLS1_VERSION:
    verstr = "TLSv1.0";
    अवरोध;
#अगर_घोषित TLS1_1_VERSION
  हाल TLS1_1_VERSION:
    verstr = "TLSv1.1";
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित TLS1_2_VERSION
  हाल TLS1_2_VERSION:
    verstr = "TLSv1.2";
    अवरोध;
#पूर्ण_अगर
  हाल TLS1_3_VERSION:
    verstr = "TLSv1.3";
    अवरोध;
  शेष:
    curl_msnprintf(unknown, माप(unknown), "(%x)", (अचिन्हित पूर्णांक)ssl_ver);
    verstr = unknown;
    अवरोध;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  अगर(ssl_ver
#अगर_घोषित SSL3_RT_HEADER
     && content_type != SSL3_RT_HEADER
#पूर्ण_अगर
#अगर_घोषित SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#पूर्ण_अगर
    ) {
    स्थिर अक्षर *संदेश_name = "Truncated message";
    स्थिर अक्षर *tls_rt_name;
    अक्षर ssl_buf[1024];
    पूर्णांक संदेश_type = 0;
    पूर्णांक txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    अगर(ssl_ver == SSL3_VERSION_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    अन्यथा
      tls_rt_name = "";

    अगर(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      अगर(len) {
        संदेश_type = *(स्थिर अचिन्हित अक्षर *)buf;
        संदेश_name = "Change cipher spec";
      }
    }
    अन्यथा अगर(content_type == SSL3_RT_ALERT) {
      अगर(len >= 2) {
        संदेश_type =
          (((स्थिर अचिन्हित अक्षर *)buf)[0] << 8) +
           ((स्थिर अचिन्हित अक्षर *)buf)[1];
        संदेश_name = SSL_alert_desc_string_long(संदेश_type);
      }
    }
    अन्यथा अगर(len) {
      संदेश_type = *(स्थिर अचिन्हित अक्षर *)buf;
      संदेश_name = ssl_संदेश_type(ssl_ver, संदेश_type);
    }

    txt_len = curl_msnprintf(ssl_buf, माप(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, संदेश_name, संदेश_type);
    Curl_debug(तथ्य, CURLINFO_TEXT, ssl_buf, (माप_प्रकार)txt_len);
  }

  Curl_debug(तथ्य, (direction == 1) ? CURLINFO_SSL_तथ्य_OUT :
             CURLINFO_SSL_तथ्य_IN, (स्थिर अक्षर *)buf, len);
  (व्योम)ssl;
}

अटल CURLcode ossl_set_ssl_version_min_max(काष्ठा Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             अचिन्हित पूर्णांक ssl_version_min)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  दीर्घ curl_ssl_version_min = (दीर्घ)ssl_version_min;
  दीर्घ curl_ssl_version_max;

  /* convert curl min SSL version option to OpenSSL constant */
#अगर defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_VERSION_संख्या)
  uint16_t ossl_ssl_version_min = 0;
  uint16_t ossl_ssl_version_max = 0;
#अन्यथा
  दीर्घ ossl_ssl_version_min = 0;
  दीर्घ ossl_ssl_version_max = 0;
#पूर्ण_अगर
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_version_min != CURL_SSLVERSION_DEFAULT);
  चयन(curl_ssl_version_min) {
  हाल CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  हाल CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_version_min = TLS1_VERSION;
    अवरोध;
  हाल CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_version_min = TLS1_1_VERSION;
    अवरोध;
  हाल CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_version_min = TLS1_2_VERSION;
    अवरोध;
  हाल CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_version_min = TLS1_3_VERSION;
    अवरोध;
  }

  /* ... then, TLS max version */
  curl_ssl_version_max = (दीर्घ)conn_config->version_max;

  /* convert curl max SSL version option to OpenSSL constant */
  चयन(curl_ssl_version_max) {
  हाल CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_version_max = TLS1_VERSION;
    अवरोध;
  हाल CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_version_max = TLS1_1_VERSION;
    अवरोध;
  हाल CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_version_max = TLS1_2_VERSION;
    अवरोध;
  हाल CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_version_max = TLS1_3_VERSION;
    अवरोध;
  हाल CURL_SSLVERSION_MAX_NONE:  /* none selected */
  हाल CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  शेष:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_version_max = 0;
    अवरोध;
  }

  अगर(!SSL_CTX_set_min_proto_version(ctx, ossl_ssl_version_min) ||
     !SSL_CTX_set_max_proto_version(ctx, ossl_ssl_version_max))
    वापस CURLE_SSL_CONNECT_ERROR;

  वापस CURLE_OK;
}

CURLcode Curl_ossl_add_session(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा ossl_ctx *octx,
                               स्थिर अक्षर *ssl_peer_कुंजी,
                               SSL_SESSION *session,
                               स्थिर अक्षर *alpn,
                               अचिन्हित अक्षर *quic_tp,
                               माप_प्रकार quic_tp_len,
                               काष्ठा Curl_ssl_session **psession)
{
  काष्ठा Curl_ssl_session *sc_session = शून्य, *sc_dup = शून्य;
  अचिन्हित अक्षर *der_session_buf = शून्य;
  अचिन्हित अक्षर *qtp_clone = शून्य;
  CURLcode परिणाम = CURLE_OK;

  अगर(psession)
    *psession = शून्य;
  अगर(!cf || !तथ्य)
    जाओ out;

  अगर(Curl_ssl_scache_use(cf, तथ्य)) {
    माप_प्रकार der_session_आकार;
    अचिन्हित अक्षर *der_session_ptr;
    माप_प्रकार earlydata_max = 0;
    पूर्णांक ietf_tls_id = SSL_version(octx->ssl);

    der_session_आकार = i2d_SSL_SESSION(session, शून्य);
    अगर(der_session_आकार == 0) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_आकार);
    अगर(!der_session_buf) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }

    der_session_आकार = i2d_SSL_SESSION(session, &der_session_ptr);
    अगर(der_session_आकार == 0) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }

#अगर_घोषित HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_तथ्य(session);
#पूर्ण_अगर
    अगर(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((स्थिर अक्षर *)quic_tp, quic_tp_len);
      अगर(!qtp_clone) {
        परिणाम = CURLE_OUT_OF_MEMORY;
        जाओ out;
      }
    }

    परिणाम = Curl_ssl_session_create2(der_session_buf, der_session_आकार,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)समय(शून्य) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = शून्य;  /* took ownership of sdata */
#अगर_घोषित USE_APPLE_SECTRUST
    अगर(!परिणाम)
      sc_session->sectrust_verified = octx->sectrust_verified;
#पूर्ण_अगर
    अगर(!परिणाम && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        परिणाम = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    अगर(!परिणाम) {
      परिणाम = Curl_ssl_scache_put(cf, तथ्य, ssl_peer_कुंजी, sc_session);
      /* took ownership of `sc_session` */
      sc_session = शून्य;
    }
  }

out:
  curlx_free(der_session_buf);
  अगर(!परिणाम && psession) {
    *psession = sc_dup;
    sc_dup = शून्य;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  वापस परिणाम;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
अटल पूर्णांक ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  काष्ठा Curl_cfilter *cf = (काष्ठा Curl_cfilter *)SSL_get_app_तथ्य(ssl);
  अगर(cf) {
    काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
    काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
    काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
    काष्ठा Curl_ssl_session *session = शून्य;
    Curl_ossl_add_session(cf, तथ्य, octx, connssl->peer.scache_कुंजी,
                          ssl_sessionid, connssl->negotiated.alpn, शून्य,
                          0, &session);
    अगर(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  वापस 0;
}

अटल CURLcode load_cacert_from_memory(X509_STORE *store,
                                        स्थिर काष्ठा curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = शून्य;
  ढेर_OF(X509_INFO) *inf = शून्य;

  /* everything else is a reference */
  पूर्णांक i, गिनती = 0;
  X509_INFO *itmp = शून्य;

  अगर(ca_info_blob->len > (माप_प्रकार)पूर्णांक_उच्च)
    वापस CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->तथ्य, (पूर्णांक)ca_info_blob->len);
  अगर(!cbio)
    वापस CURLE_OUT_OF_MEMORY;

  inf = PEM_X509_INFO_read_bio(cbio, शून्य, शून्य, शून्य);
  अगर(!inf) {
    BIO_free(cbio);
    वापस CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  क्रम(i = 0; i < (पूर्णांक)sk_X509_INFO_अंक(inf); ++i) {
    itmp = sk_X509_INFO_मूल्य(inf, (ossl_valsize_t)i);
    अगर(itmp->x509) {
      अगर(X509_STORE_add_cert(store, itmp->x509)) {
        ++गिनती;
      }
      अन्यथा {
        /* set count to 0 to return an error */
        गिनती = 0;
        अवरोध;
      }
    }
    अगर(itmp->crl) {
      अगर(X509_STORE_add_crl(store, itmp->crl)) {
        ++गिनती;
      }
      अन्यथा {
        /* set count to 0 to return an error */
        गिनती = 0;
        अवरोध;
      }
    }
  }

#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic धकेलो
#आशय clang diagnostic ignored "-Wcast-function-type-strict"
#पूर्ण_अगर
  sk_X509_INFO_निकालो_free(inf, X509_INFO_free);
#अगर defined(__clang__) && __clang_major__ >= 16
#आशय clang diagnostic निकालो
#पूर्ण_अगर
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  वापस (गिनती > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#अगर_घोषित USE_WIN32_CRYPTO
अटल CURLcode ossl_win_load_store(काष्ठा Curl_easy *तथ्य,
                                    काष्ठा Curl_cfilter *cf,
                                    स्थिर अक्षर *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode परिणाम = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  अगर(hStore) {
    PCCERT_CONTEXT pContext = शून्य;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = शून्य;
    DWORD enhkey_usage_आकार = 0;
    VERBOSE(माप_प्रकार योग = 0);
    VERBOSE(माप_प्रकार imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    परिणाम = CURLE_OK;
    क्रम(;;) {
      X509 *x509;
      FILETIME now;
      BYTE कुंजी_usage[2];
      DWORD req_आकार;
      स्थिर अचिन्हित अक्षर *encoded_cert;
      pContext = CertEnumCertificatesInStore(hStore, pContext);
      अगर(!pContext)
        अवरोध;

      VERBOSE(++योग);

#अगर defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        अक्षर cert_name[256];
        अगर(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               शून्य, cert_name, माप(cert_name)))
          infof(तथ्य, "SSL: unknown cert name");
        अन्यथा
          infof(तथ्य, "SSL: Checking cert \"%s\"", cert_name);
      }
#पूर्ण_अगर
      encoded_cert = (स्थिर अचिन्हित अक्षर *)pContext->pbCertEncoded;
      अगर(!encoded_cert)
        जारी;

      GetSystemTimeAsFileTime(&now);
      अगर(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        जारी;

      /* If key usage exists check for signing attribute */
      अगर(Cert_Get_Intended_कुंजी_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 कुंजी_usage, माप(कुंजी_usage))) {
        अगर(!(कुंजी_usage[0] & CERT_कुंजी_CERT_SIGN_कुंजी_USAGE))
          जारी;
      }
      अन्यथा अगर(GetLastError())
        जारी;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      अगर(Cert_Get_Enhanced_कुंजी_Usage(pContext, 0, शून्य, &req_आकार) && req_आकार) {
        अगर(req_आकार > enhkey_usage_आकार) {
          व्योम *tmp = curlx_realloc(enhkey_usage, req_आकार);

          अगर(!tmp) {
            failf(तथ्य, "SSL: Out of memory allocating for OID list");
            परिणाम = CURLE_OUT_OF_MEMORY;
            अवरोध;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_आकार = req_आकार;
        }

        अगर(Cert_Get_Enhanced_कुंजी_Usage(pContext, 0, enhkey_usage, &req_आकार)) {
          अगर(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            अगर((HRESULT)GetLastError() != CRYPT_E_NOT_FOUND)
              जारी;
          }
          अन्यथा {
            DWORD i;
            bool found = FALSE;

            क्रम(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              अगर(!म_भेद("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                found = TRUE;
                अवरोध;
              }
            }

            अगर(!found)
              जारी;
          }
        }
        अन्यथा
          जारी;
      }
      अन्यथा
        जारी;

      x509 = d2i_X509(शून्य, &encoded_cert, (दीर्घ)pContext->cbCertEncoded);
      अगर(!x509)
        जारी;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      अगर(X509_STORE_add_cert(store, x509) == 1) {
        VERBOSE(++imported);
#अगर_घोषित DEBUGBUILD
        infof(तथ्य, "SSL: Imported cert");
#पूर्ण_अगर
        *padded = TRUE;
      }
      X509_free(x509);
    }

    curlx_free(enhkey_usage);
    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    CURL_TRC_CF(तथ्य, cf,
                "ossl_win_load_store() found: %zu imported: %zu certs in %s.",
                योग, imported, win_store);

    अगर(परिणाम)
      वापस परिणाम;
  }

  वापस परिणाम;
}

अटल CURLcode ossl_windows_load_anchors(काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  अटल स्थिर अक्षर * स्थिर win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  माप_प्रकार i;
  CURLcode परिणाम = CURLE_OK;

  *padded = FALSE;
  क्रम(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    परिणाम = ossl_win_load_store(तथ्य, cf, win_stores[i], store, &store_added);
    अगर(परिणाम)
      वापस परिणाम;
    अगर(store_added) {
      CURL_TRC_CF(तथ्य, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    अन्यथा
      infof(तथ्य, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  वापस परिणाम;
}

#पूर्ण_अगर /* USE_WIN32_CRYPTO */

अटल CURLcode ossl_load_trust_anchors(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा ossl_ctx *octx,
                                        X509_STORE *store)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर * स्थिर ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? शून्य : conn_config->CAfile);
  स्थिर अक्षर * स्थिर ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_empty = TRUE;
  अगर(conn_config->native_ca_store) {
#अगर_घोषित USE_WIN32_CRYPTO
    bool added = FALSE;
    परिणाम = ossl_windows_load_anchors(cf, तथ्य, store, &added);
    अगर(परिणाम)
      वापस परिणाम;
    अगर(added) {
      infof(तथ्य, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_empty = FALSE;
    }
#या_अगर defined(USE_APPLE_SECTRUST)
    infof(तथ्य, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#पूर्ण_अगर
  }

  अगर(conn_config->ca_info_blob) {
    परिणाम = load_cacert_from_memory(store, conn_config->ca_info_blob);
    अगर(परिणाम) {
      failf(तथ्य, "error adding trust anchors from certificate blob: %d",
            (पूर्णांक)परिणाम);
      वापस परिणाम;
    }
    infof(तथ्य, "  CA Blob from configuration");
    octx->store_is_empty = FALSE;
  }

  अगर(ssl_cafile || ssl_capath) {
#अगर_घोषित HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    अगर(ssl_cafile) {
      अगर(!X509_STORE_load_file(store, ssl_cafile)) {
        अगर(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(तथ्य, "error adding trust anchors from file: %s", ssl_cafile);
          वापस CURLE_SSL_CACERT_BADFILE;
        }
        अन्यथा
          infof(तथ्य, "error setting certificate file, continuing anyway");
      }
      infof(तथ्य, "  CAfile: %s", ssl_cafile);
      octx->store_is_empty = FALSE;
    }
    अगर(ssl_capath) {
      अगर(!X509_STORE_load_path(store, ssl_capath)) {
        अगर(octx->store_is_empty && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(तथ्य, "error adding trust anchors from path: %s", ssl_capath);
          वापस CURLE_SSL_CACERT_BADFILE;
        }
        अन्यथा
          infof(तथ्य, "error setting certificate path, continuing anyway");
      }
      infof(तथ्य, "  CApath: %s", ssl_capath);
      octx->store_is_empty = FALSE;
    }
#अन्यथा
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    अगर(!X509_STORE_load_locations(store, ssl_cafile, ssl_capath)) {
      अगर(octx->store_is_empty && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(तथ्य, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        वापस CURLE_SSL_CACERT_BADFILE;
      }
      अन्यथा {
        infof(तथ्य, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    अगर(ssl_cafile)
      infof(तथ्य, "  CAfile: %s", ssl_cafile);
    अगर(ssl_capath)
      infof(तथ्य, "  CApath: %s", ssl_capath);
    octx->store_is_empty = FALSE;
#पूर्ण_अगर
  }

#अगर_घोषित CURL_CA_FALLBACK
  अगर(octx->store_is_empty) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_set_default_paths(store);
    infof(तथ्य, "  OpenSSL default paths (fallback)");
    octx->store_is_empty = FALSE;
  }
#पूर्ण_अगर
  अगर(octx->store_is_empty && !have_native_check)
    infof(तथ्य, "  no trust anchors configured");

  वापस परिणाम;
}

अटल CURLcode ossl_populate_x509_store(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य,
                                         काष्ठा ossl_ctx *octx,
                                         X509_STORE *store)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  X509_LOOKUP *lookup = शून्य;
  स्थिर अक्षर * स्थिर ssl_crlfile = conn_config->CRLfile;
  अचिन्हित दीर्घ x509flags = 0;

  CURL_TRC_CF(तथ्य, cf, "configuring OpenSSL's x509 trust store");
  अगर(!store)
    वापस CURLE_OUT_OF_MEMORY;

  अगर(!conn_config->verifypeer) {
    infof(तथ्य, "SSL Trust: peer verification disabled");
    वापस CURLE_OK;
  }

  infof(तथ्य, "SSL Trust Anchors:");
  परिणाम = ossl_load_trust_anchors(cf, तथ्य, octx, store);
  अगर(परिणाम)
    वापस परिणाम;

  /* Does not make sense to load a CRL file without peer verification */
#अगर_घोषित USE_APPLE_SECTRUST
  अगर(ssl_crlfile && conn_config->native_ca_store) {
    failf(तथ्य, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    वापस CURLE_NOT_BUILT_IN;
  }
#पूर्ण_अगर
  अगर(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file());
    अगर(!lookup ||
       (!X509_load_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(तथ्य, "error loading CRL file: %s", ssl_crlfile);
      वापस CURLE_SSL_CRL_BADFILE;
    }
    x509flags = X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL;
    infof(तथ्य, " CRLfile: %s", ssl_crlfile);
  }

  /* Try building a chain using issuers in the trusted store first to avoid
     problems with server-sent legacy intermediates. Newer versions of
     OpenSSL do alternate chain checking by default but we do not know how to
     determine that in a reliable manner.
     https://web.archive.org/web/20190422050538/rt.openssl.org/Ticket/Display.html?id=3621
   */
  x509flags |= X509_V_FLAG_TRUSTED_FIRST;

  अगर(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (व्योम)X509_STORE_set_flags(store, x509flags);

  वापस परिणाम;
}

/* key to use at `multi->proto_hash` */
#घोषणा MPROTO_OSSL_X509_कुंजी  "tls:ossl:x509:share"

काष्ठा ossl_x509_share {
  अक्षर *CAfile;         /* CAfile path used to generate X509 store */
  अक्षर *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  काष्ठा curltime समय; /* when the cached store was created */
  BIT(store_is_empty);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

अटल व्योम oss_x509_share_free(स्थिर व्योम *कुंजी, माप_प्रकार कुंजी_len, व्योम *p)
{
  काष्ठा ossl_x509_share *share = p;
  DEBUGASSERT(कुंजी_len == CURL_CSTRLEN(MPROTO_OSSL_X509_कुंजी));
  DEBUGASSERT(!स_भेद(MPROTO_OSSL_X509_कुंजी, कुंजी, कुंजी_len));
  (व्योम)कुंजी;
  (व्योम)कुंजी_len;
  अगर(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

अटल bool ossl_cached_x509_store_expired(काष्ठा Curl_easy *तथ्य,
                                           स्थिर काष्ठा ossl_x509_share *mb)
{
  अगर(तथ्य->set.ssl_ca_cache_timeout < 0)
    वापस FALSE;
  अन्यथा {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &mb->समय);
    timediff_t timeout_ms = तथ्य->set.ssl_ca_cache_timeout * (timediff_t)1000;

    वापस elapsed_ms >= timeout_ms;
  }
}

अटल bool ossl_cached_x509_store_different(काष्ठा Curl_cfilter *cf,
                                             स्थिर काष्ठा Curl_easy *तथ्य,
                                             स्थिर काष्ठा ossl_x509_share *mb)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (व्योम)तथ्य;
  अगर(mb->no_partialchain != conn_config->no_partialchain)
    वापस TRUE;
  अगर(!mb->CAfile || !conn_config->CAfile) {
    अगर(mb->CAfile != conn_config->CAfile)
      वापस TRUE;
  }
  अन्यथा अगर(म_भेद(mb->CAfile, conn_config->CAfile))
    वापस TRUE;

  अगर(!mb->CApath || !conn_config->CApath) {
    अगर(mb->CApath != conn_config->CApath)
      वापस TRUE;
  }
  अन्यथा अगर(म_भेद(mb->CApath, conn_config->CApath))
    वापस TRUE;

  वापस FALSE;
}

अटल X509_STORE *ossl_get_cached_x509_store(काष्ठा Curl_cfilter *cf,
                                              काष्ठा Curl_easy *तथ्य,
                                              bool *pempty)
{
  काष्ठा Curl_multi *multi = तथ्य->multi;
  काष्ठा ossl_x509_share *share;
  X509_STORE *store = शून्य;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_कुंजी,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_कुंजी)) : शून्य;
  अगर(share && share->store &&
     !ossl_cached_x509_store_expired(तथ्य, share) &&
     !ossl_cached_x509_store_different(cf, तथ्य, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_empty;
  }

  वापस store;
}

अटल व्योम ossl_set_cached_x509_store(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       X509_STORE *store,
                                       bool is_empty)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा Curl_multi *multi = तथ्य->multi;
  काष्ठा ossl_x509_share *share;

  DEBUGASSERT(multi);
  अगर(!multi)
    वापस;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_कुंजी,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_कुंजी));

  अगर(!share) {
    share = curlx_calloc(1, माप(*share));
    अगर(!share)
      वापस;
    अगर(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_कुंजी,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_कुंजी),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      वापस;
    }
  }

  अगर(X509_STORE_up_ref(store)) {
    अक्षर *CAfile = शून्य;
    अक्षर *CApath = शून्य;

    अगर(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      अगर(!CAfile) {
        X509_STORE_free(store);
        वापस;
      }
    }

    अगर(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      अगर(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        वापस;
      }
    }

    अगर(share->store) {
      X509_STORE_free(share->store);
      curlx_free(share->CAfile);
      curlx_free(share->CApath);
    }

    share->समय = *Curl_pgrs_now(तथ्य);
    share->store = store;
    share->store_is_empty = is_empty;
    share->CAfile = CAfile;
    share->CApath = CApath;
    share->no_partialchain = conn_config->no_partialchain;
  }
}

#अगर_घोषित OPENSSL_HAS_PROVIDERS
/* CApath certificates loaded under a provider libctx can outlive that libctx
   when the store is shared with the multi handle. */
#घोषणा USING_PROVIDER(octx) octx->p.libctx
#अन्यथा
#घोषणा USING_PROVIDER(x) 0
#पूर्ण_अगर

CURLcode Curl_ssl_setup_x509_store(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ossl_ctx *octx)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  X509_STORE *cached_store;
  bool cache_criteria_met, is_empty;

  /* Consider the X509 store cacheable if it comes from CAfile/CApath,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (तथ्य->set.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !USING_PROVIDER(octx);

  ERR_set_mark();

  cached_store = ossl_get_cached_x509_store(cf, तथ्य, &is_empty);
  अगर(cached_store && cache_criteria_met && X509_STORE_up_ref(cached_store)) {
    SSL_CTX_set_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_empty = is_empty;
  }
  अन्यथा {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    परिणाम = ossl_populate_x509_store(cf, तथ्य, octx, store);
    अगर(परिणाम == CURLE_OK && cache_criteria_met) {
      ossl_set_cached_x509_store(cf, तथ्य, store, (bool)octx->store_is_empty);
    }
  }

  ERR_निकालो_to_mark();

  वापस परिणाम;
}

अटल bool ossl_apply_session(
  काष्ठा ossl_ctx *octx,
  काष्ठा Curl_cfilter *cf,
  काष्ठा Curl_easy *तथ्य,
  काष्ठा alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  काष्ठा Curl_ssl_session *scs)
{
  काष्ठा ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  स्थिर अचिन्हित अक्षर *der_sessionid = scs->sdata;
  माप_प्रकार der_sessionid_आकार = scs->sdata_len;
  SSL_SESSION *ssl_session = शून्य;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(शून्य, &der_sessionid,
                                (दीर्घ)der_sessionid_आकार);
  अगर(ssl_session) {
    अगर(!SSL_set_session(octx->ssl, ssl_session)) {
      VERBOSE(अक्षर error_बफर[256]);
      infof(तथ्य, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          माप(error_बफर)));
    }
    अन्यथा {
      अगर(conn_cfg->verifypeer &&
         (SSL_get_verify_परिणाम(octx->ssl) != X509_V_OK)
#अगर_घोषित USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#पूर्ण_अगर
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_set_session(octx->ssl, शून्य);
        infof(तथ्य, "SSL session not peer verified, not reusing");
      }
      अन्यथा {
        infof(तथ्य, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#अगर_घोषित USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#पूर्ण_अगर
        infof(तथ्य, "SSL verify result: %lx",
              (अचिन्हित दीर्घ)SSL_get_verify_परिणाम(octx->ssl));
#अगर_घोषित HAVE_OPENSSL_EARLYDATA
        अगर(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_तथ्य(ssl_session) &&
           !cf->conn->bits.connect_only &&
           (SSL_version(octx->ssl) == TLS1_3_VERSION)) {
          bool do_early_तथ्य = FALSE;
          अगर(sess_reuse_cb)
            (व्योम)sess_reuse_cb(cf, तथ्य, alpns, scs, &do_early_तथ्य);
          अगर(do_early_तथ्य) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_to(alpns, scs->alpn);
          }
        }
#अन्यथा
        (व्योम)alpns;
        (व्योम)conn_cfg;
        (व्योम)sess_reuse_cb;
#पूर्ण_अगर
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  अन्यथा {
    infof(तथ्य, "SSL session not accepted by OpenSSL, continuing without");
  }
  वापस (bool)octx->reused_session;
}

अटल CURLcode ossl_init_session_and_alpns(
  काष्ठा ossl_ctx *octx,
  काष्ठा Curl_cfilter *cf,
  काष्ठा Curl_easy *तथ्य,
  काष्ठा ssl_peer *peer,
  स्थिर काष्ठा alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  काष्ठा ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा alpn_spec alpns;
  CURLcode परिणाम;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  अगर((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    काष्ठा Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(तथ्य, cf->cft, FIRSTSOCKET);
    अगर(scs) {
      अगर(ossl_apply_session(octx, cf, तथ्य, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(तथ्य, cf, "applied SSL session from control connection");
    }
  }

  अगर(!octx->reused_session &&
     Curl_ssl_scache_use(cf, तथ्य) && !conn_cfg->verifystatus) {
    काष्ठा Curl_ssl_session *scs = शून्य;

    परिणाम = Curl_ssl_scache_take(cf, तथ्य, peer->scache_कुंजी, &scs);
    अगर(!परिणाम && scs && scs->sdata && scs->sdata_len) {
      (व्योम)ossl_apply_session(octx, cf, तथ्य, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, तथ्य, peer->scache_कुंजी, scs);
  }

  अगर(alpns.गिनती) {
    काष्ठा alpn_proto_buf proto;
    स_रखो(&proto, 0, माप(proto));
    परिणाम = Curl_alpn_to_proto_buf(&proto, &alpns);
    अगर(परिणाम) {
      failf(तथ्य, "Error determining ALPN");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
    अगर(SSL_set_alpn_protos(octx->ssl, proto.तथ्य, proto.len)) {
      failf(तथ्य, "Error setting ALPN");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
  }

  वापस CURLE_OK;
}

#अगर_घोषित HAVE_SSL_SET1_ECH_CONFIG_LIST
अटल CURLcode ossl_init_ech(काष्ठा ossl_ctx *octx,
                              काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              काष्ठा ssl_peer *peer)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णांक trying_ech_now = 0;

  अगर(!Curl_ssl_ech_enabled(cf))
    वापस CURLE_OK;

  अगर(conn_config->ech == CURLECH_GREASE) {
    infof(तथ्य, "ECH: will GREASE ClientHello");
#अगर_घोषित HAVE_BORINGSSL_LIKE
    SSL_set_enable_ech_grease(octx->ssl, 1);
#अन्यथा
    SSL_set_options(octx->ssl, SSL_OP_ECH_GREASE);
#पूर्ण_अगर
  }
  अन्यथा अगर(conn_config->ech && conn_config->ech_config) {
#अगर_घोषित HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    माप_प्रकार ech_config_len = 0;
    CURLcode परिणाम;

    अगर(!conn_config->ech_config[0]) {
      infof(तथ्य, "ECH: ECHConfig from command line empty");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * म_माप(conn_config->ech_config);
    परिणाम = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    अगर(परिणाम || !ech_config) {
      infof(तथ्य, "ECH: cannot base64 decode ECHConfig from command line");
      अगर(conn_config->ech == CURLECH_HARD)
        वापस परिणाम;
    }
    अगर(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(तथ्य, "ECH: SSL_ECH_set1_ech_config_list failed");
      अगर(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        वापस CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#अन्यथा
    माप_प्रकार ech_config_len = 0;
    अगर(!conn_config->ech_config[0]) {
      infof(तथ्य, "ECH: ECHConfig from command line empty");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = म_माप(conn_config->ech_config);
    अगर(SSL_set1_ech_config_list(octx->ssl,
                                (स्थिर uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(तथ्य, "ECH: SSL_ECH_set1_ech_config_list failed");
      अगर(conn_config->ech == CURLECH_HARD)
        वापस CURLE_SSL_CONNECT_ERROR;
    }
    अन्यथा
      trying_ech_now = 1;
#पूर्ण_अगर /* HAVE_BORINGSSL_LIKE */
    infof(तथ्य, "ECH: ECHConfig from command line");
  }
  अन्यथा {
    स्थिर काष्ठा Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(तथ्य, cf->sockindex, peer->origin);

    अगर(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      स्थिर अचिन्हित अक्षर *ecl = rinfo->echconfiglist;
      माप_प्रकार elen = rinfo->echconfiglist_len;

      infof(तथ्य, "ECH: ECHConfig from HTTPS RR");
      अगर(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(तथ्य, "ECH: SSL_set1_ech_config_list failed");
        अगर(conn_config->ech == CURLECH_HARD)
          वापस CURLE_SSL_CONNECT_ERROR;
      }
      अन्यथा {
        trying_ech_now = 1;
        infof(तथ्य, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    अन्यथा {
      infof(तथ्य, "ECH: requested but no ECHConfig available");
      अगर(conn_config->ech == CURLECH_HARD)
        वापस CURLE_SSL_CONNECT_ERROR;
    }
  }
#अगर_घोषित HAVE_BORINGSSL_LIKE
  (व्योम)peer;
  अगर(trying_ech_now && conn_config->ech_public) {
    infof(तथ्य, "ECH: setting public_name not supported with BoringSSL");
    वापस CURLE_SSL_CONNECT_ERROR;
  }
#अन्यथा
  अगर(trying_ech_now && conn_config->ech_public) {
    पूर्णांक ret;
    infof(तथ्य, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_public);
    ret = SSL_ech_set1_server_names(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_public,
                                    0 /* do send outer */);
    अगर(ret != 1) {
      infof(तथ्य, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      वापस CURLE_SSL_CONNECT_ERROR;
    }
  }
#पूर्ण_अगर /* HAVE_BORINGSSL_LIKE */
  अगर(trying_ech_now &&
     SSL_set_min_proto_version(octx->ssl, TLS1_3_VERSION) != 1) {
    infof(तथ्य, "ECH: cannot force TLSv1.3 [ERROR]");
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  वापस CURLE_OK;
}
#पूर्ण_अगर /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

अटल CURLcode ossl_init_ssl(काष्ठा ossl_ctx *octx,
                              काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              काष्ठा ssl_peer *peer,
                              स्थिर काष्ठा alpn_spec *alpns_requested,
                              व्योम *ssl_user_तथ्य,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  अगर(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  अगर(!octx->ssl) {
    failf(तथ्य, "SSL: could not create a context (handle)");
    वापस CURLE_OUT_OF_MEMORY;
  }

  SSL_set_app_तथ्य(octx->ssl, ssl_user_तथ्य);

#अगर_अघोषित OPENSSL_NO_OCSP
  अगर(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_set_tlsext_status_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#पूर्ण_अगर

  SSL_set_connect_state(octx->ssl);

  अगर(peer->sni) {
    अगर(!SSL_set_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(तथ्य, "Failed set SNI");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
  }

#अगर_घोषित HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode परिणाम = ossl_init_ech(octx, cf, तथ्य, peer);
    अगर(परिणाम)
      वापस परिणाम;
  }
#पूर्ण_अगर /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  वापस ossl_init_session_and_alpns(octx, cf, तथ्य, peer,
                                     alpns_requested, sess_reuse_cb);
}

अटल CURLcode ossl_init_method(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ssl_peer *peer,
                                 स्थिर SSL_METHOD **pmethod,
                                 अचिन्हित पूर्णांक *pssl_version_min)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = शून्य;
  *pssl_version_min = conn_config->version;
  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  चयन(peer->transport) {
  हाल TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    चयन(*pssl_version_min) {
    हाल CURL_SSLVERSION_TLSv1:
    हाल CURL_SSLVERSION_TLSv1_0:
    हाल CURL_SSLVERSION_TLSv1_1:
    हाल CURL_SSLVERSION_TLSv1_2:
    हाल CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_method();
      अवरोध;
    हाल CURL_SSLVERSION_SSLv2:
      failf(तथ्य, "No SSLv2 support");
      वापस CURLE_NOT_BUILT_IN;
    हाल CURL_SSLVERSION_SSLv3:
      failf(तथ्य, "No SSLv3 support");
      वापस CURLE_NOT_BUILT_IN;
    शेष:
      failf(तथ्य, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
    अवरोध;
  हाल TRNSPRT_QUIC:
    *pssl_version_min = CURL_SSLVERSION_TLSv1_3;
    अगर(conn_config->version_max &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->version_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(तथ्य, "QUIC needs at least TLS version 1.3");
      वापस CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_method();
    अवरोध;
  शेष:
    failf(तथ्य, "unsupported transport %d in SSL init", peer->transport);
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  वापस *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(काष्ठा ossl_ctx *octx,
                            काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा ssl_peer *peer,
                            स्थिर काष्ठा alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            व्योम *cb_user_तथ्य,
                            Curl_ossl_new_session_cb *cb_new_session,
                            व्योम *ssl_user_तथ्य,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *ciphers;
  स्थिर SSL_METHOD *req_method = शून्य;
  ctx_option_t ctx_options = 0;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, तथ्य);
  अक्षर * स्थिर ssl_cert = conn_config->clientcert;
  स्थिर काष्ठा curl_blob *ssl_cert_blob = conn_config->cert_blob;
  स्थिर अक्षर * स्थिर ssl_cert_type = conn_config->cert_type;
  अचिन्हित पूर्णांक ssl_version_min = CURL_SSLVERSION_DEFAULT;
  अक्षर error_बफर[256];

  /* Make funny stuff to get random input */
  परिणाम = ossl_seed(तथ्य);
  अगर(!परिणाम) {
    अगर(CURL_EASY_STR(तथ्य, STRING_SSL_ENGINE))
      परिणाम = ossl_set_engine(तथ्य, octx,
                               CURL_EASY_STR(तथ्य, STRING_SSL_ENGINE));
    अगर(!परिणाम && तथ्य->set.engine_default)
      परिणाम = ossl_set_engine_default(तथ्य, octx);
  }

  अगर(!परिणाम) {
    ssl_config->certverifyresult = !X509_V_OK;
    परिणाम = ossl_init_method(cf, तथ्य, peer, &req_method, &ssl_version_min);
  }
  अगर(परिणाम)
    वापस परिणाम;
  DEBUGASSERT(req_method);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#अगर_घोषित OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_method):
#पूर्ण_अगर
    SSL_CTX_new(req_method);

  अगर(!octx->ssl_ctx) {
    failf(तथ्य, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_बफर, माप(error_बफर)));
    वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(cb_setup) {
    परिणाम = cb_setup(cf, तथ्य, cb_user_तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(तथ्य->set.fdebug && तथ्य->set.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_set_संदेश_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_set_संदेश_callback_arg(octx->ssl_ctx, cf);
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
  अगर(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_जोड़ो_EMPTY_FRAGMENTS;

  DEBUGASSERT(ssl_version_min != CURL_SSLVERSION_DEFAULT);
  चयन(ssl_version_min) {
  हाल CURL_SSLVERSION_SSLv2:
  हाल CURL_SSLVERSION_SSLv3:
    वापस CURLE_NOT_BUILT_IN;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  हाल CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  हाल CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  हाल CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  हाल CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  हाल CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    परिणाम = ossl_set_ssl_version_min_max(cf, octx->ssl_ctx, ssl_version_min);
    अगर(परिणाम)
      वापस परिणाम;
    अवरोध;

  शेष:
    failf(तथ्य, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    वापस CURLE_SSL_CONNECT_ERROR;
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
#अगर_घोषित HAVE_SSL_CTX_SET_DEFAULT_READ_बफर_LEN
  SSL_CTX_set_default_read_बफर_len(octx->ssl_ctx, 0x401e * 4);
#पूर्ण_अगर

  /* We do retry writes sometimes from another buffer address */
  SSL_CTX_set_mode(octx->ssl_ctx, SSL_MODE_ACCEPT_MOVING_WRITE_बफर);

  ciphers = conn_config->cipher_list;
  अगर(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = शून्य;
  अगर(ciphers && (ssl_version_min < CURL_SSLVERSION_TLSv1_3)) {
    अगर(!SSL_CTX_set_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(तथ्य, "failed setting cipher list: %s", ciphers);
      वापस CURLE_SSL_CIPHER;
    }
    infof(तथ्य, "Cipher selection: %s", ciphers);
  }

#अगर_घोषित HAVE_SSL_CTX_SET_CIPHERSUITES
  {
    स्थिर अक्षर *ciphers13 = conn_config->cipher_list13;
    अगर(ciphers13 &&
       (!conn_config->version_max ||
        (conn_config->version_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->version_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      अगर(!SSL_CTX_set_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(तथ्य, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        वापस CURLE_SSL_CIPHER;
      }
      infof(तथ्य, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#पूर्ण_अगर

  अगर(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    परिणाम = client_cert(तथ्य, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->कुंजी, conn_config->कुंजी_blob,
                         conn_config->कुंजी_type,
                         conn_config->कुंजी_passwd);
    अगर(परिणाम)
      /* failf() is already done in client_cert() */
      वापस परिणाम;
  }

#अगर_घोषित HAVE_SSL_CTX_SET_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_set_post_handshake_auth(octx->ssl_ctx, 1);
#पूर्ण_अगर

  {
    स्थिर अक्षर *curves = conn_config->curves;
    अगर(curves) {
#अगर_घोषित HAVE_BORINGSSL_LIKE
#घोषणा OSSL_CURVE_CAST(x) (x)
#अन्यथा
#घोषणा OSSL_CURVE_CAST(x) (अक्षर *)CURL_UNCONST(x)
#पूर्ण_अगर
      अगर(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_CURVE_CAST(curves))) {
        failf(तथ्य, "failed setting curves list: '%s'", curves);
        वापस CURLE_SSL_CIPHER;
      }
    }
  }

#अगर_घोषित HAVE_SSL_CTX_SET1_SIGALGS
#घोषणा OSSL_SIGALG_CAST(x) OSSL_CURVE_CAST(x)
  {
    स्थिर अक्षर *signature_algorithms = conn_config->signature_algorithms;
    अगर(signature_algorithms) {
      अगर(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(तथ्य, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        वापस CURLE_SSL_CIPHER;
      }
    }
  }
#पूर्ण_अगर

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_set_verify(octx->ssl_ctx, SSL_VERIFY_NONE, शून्य);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#अगर !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  अगर(Curl_tls_keylog_enabled()) {
    SSL_CTX_set_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#पूर्ण_अगर

  अगर(cb_new_session) {
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
  अगर(तथ्य->set.ssl_fsslctx) {
    काष्ठा Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    अगर(!octx->x509_store_setup) {
      परिणाम = Curl_ssl_setup_x509_store(cf, तथ्य, octx);
      अगर(परिणाम)
        वापस परिणाम;
      octx->x509_store_setup = TRUE;
    }
    CURL_CBAPI_START(&guard, तथ्य, easy_fsslctx);
    परिणाम = (*तथ्य->set.ssl_fsslctx)(तथ्य, octx->ssl_ctx,
                                      तथ्य->set.ssl_fsslctxp);
    CURL_CBAPI_END(&guard);
    अगर(परिणाम) {
      failf(तथ्य, "error signaled by SSL ctx callback");
      वापस परिणाम;
    }
  }

  वापस ossl_init_ssl(octx, cf, तथ्य, peer, alpns_requested,
                       ssl_user_तथ्य, sess_reuse_cb);
}

अटल CURLcode ossl_on_session_reuse(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा alpn_spec *alpns,
                                      काष्ठा Curl_ssl_session *scs,
                                      bool *do_early_तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  वापस Curl_on_session_reuse(cf, तथ्य, alpns, scs, do_early_तथ्य,
                               connssl->earlydata_max);
}

व्योम Curl_ossl_report_handshake(काष्ठा Curl_easy *तथ्य, काष्ठा ossl_ctx *octx)
{
#अगर_घोषित CURLVERBOSE
  अगर(Curl_trc_is_verbose(तथ्य)) {
    पूर्णांक psigtype_nid = NID_undef;
    स्थिर अक्षर *negotiated_group_name = शून्य;

#अगर_घोषित HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#अगर OPENSSL_VERSION_संख्या >= 0x30200000L
    negotiated_group_name = SSL_get0_group_name(octx->ssl);
#अन्यथा
    negotiated_group_name =
      OBJ_nid2sn(SSL_get_negotiated_group(octx->ssl) & 0x0000FFFF);
#पूर्ण_अगर
#पूर्ण_अगर

    /* Informational message */
    infof(तथ्य, "SSL connection using %s / %s / %s / %s",
          SSL_get_version(octx->ssl),
          SSL_get_cipher(octx->ssl),
          negotiated_group_name ? negotiated_group_name : "[blank]",
          OBJ_nid2sn(psigtype_nid));
  }
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)octx;
#पूर्ण_अगर /* CURLVERBOSE */
}

अटल CURLcode ossl_connect_step1(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode परिणाम;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  परिणाम = Curl_ossl_ctx_init(octx, cf, तथ्य, &connssl->peer,
                              connssl->alpn, शून्य, शून्य,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  अगर(परिणाम)
    वापस परिणाम;

  octx->bio_method = ossl_bio_cf_method_create();
  अगर(!octx->bio_method)
    वापस CURLE_OUT_OF_MEMORY;
  bio = BIO_new(octx->bio_method);
  अगर(!bio)
    वापस CURLE_OUT_OF_MEMORY;

  BIO_set_तथ्य(bio, cf);
#अगर_घोषित HAVE_SSL_SET0_WBIO
  /* with OpenSSL v1.1.1 we get an alternative to SSL_set_bio() that works
   * without backward compat quirks. Every call takes one reference, so we
   * up it and pass. SSL* then owns and frees it.
   * We check on the function in configure, since LibreSSL and friends
   * each have their own versions to add support for this. */
  BIO_up_ref(bio);
  SSL_set0_rbio(octx->ssl, bio);
  SSL_set0_wbio(octx->ssl, bio);
#अन्यथा
  SSL_set_bio(octx->ssl, bio, bio);
#पूर्ण_अगर

  अगर(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    काष्ठा alpn_proto_buf proto;
    स_रखो(&proto, 0, माप(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(तथ्य, VTLS_INFOF_ALPN_OFFER_1STR, proto.तथ्य);
  }

  connssl->connecting_state = ssl_connect_2;
  वापस CURLE_OK;
}

#अगर_घोषित HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
अटल पूर्णांक ossl_trace_ech_retry_configs(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य, SSL *ssl,
                                        पूर्णांक reason)
{
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार rcl = 0;
  पूर्णांक rv = 1;
#अगर_अघोषित HAVE_BORINGSSL_LIKE
  अक्षर *inner = शून्य;
  uint8_t *rcs = शून्य;
  अक्षर *outer = शून्य;
#अन्यथा
  स्थिर अक्षर *inner = शून्य;
  स्थिर uint8_t *rcs = शून्य;
  स्थिर अक्षर *outer = शून्य;
  माप_प्रकार out_name_len = 0;
  पूर्णांक servername_type = 0;
#पूर्ण_अगर
  NOVERBOSE((व्योम)reason);

  /* nothing to trace if not doing ECH */
  अगर(!Curl_ssl_ech_enabled(cf))
    वापस rv;
#अगर_अघोषित HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#अन्यथा
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (पूर्णांक)rcl;
#पूर्ण_अगर

  अगर(rv && rcs) {
    अक्षर *b64str = शून्य;
    माप_प्रकार blen = 0;

    परिणाम = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    अगर(!परिणाम && b64str) {
      infof(तथ्य, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#अगर_अघोषित HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_status(ssl, &inner, &outer);
      infof(तथ्य, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#अन्यथा
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &out_name_len);
      infof(तथ्य, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#पूर्ण_अगर
    }
  }
  अन्यथा
    infof(तथ्य, "ECH: no retry_configs (rv = %d)", rv);
#अगर_अघोषित HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#पूर्ण_अगर
  वापस rv;
}

#पूर्ण_अगर

अटल CURLcode ossl_connect_step2(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  काष्ठा ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, तथ्य);
  पूर्णांक err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  अगर(!octx->x509_store_setup) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode परिणाम = Curl_ssl_setup_x509_store(cf, तथ्य, octx);
    अगर(परिणाम)
      वापस परिणाम;
    octx->x509_store_setup = TRUE;
  }

#अगर !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  अगर(Curl_tls_keylog_enabled() && !octx->keylog_done)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_done);
#पूर्ण_अगर

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  अगर(err != 1) {
    पूर्णांक detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(तथ्य, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    अगर(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(तथ्य, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      वापस CURLE_AGAIN;
    }
    अगर(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(तथ्य, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      वापस CURLE_AGAIN;
    }
#अगर_घोषित SSL_ERROR_WANT_ASYNC
    अगर(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(तथ्य, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      वापस CURLE_AGAIN;
    }
#पूर्ण_अगर
#अगर_घोषित SSL_ERROR_WANT_RETRY_VERIFY
    अगर(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(तथ्य, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(तथ्य, TRUE);
      वापस CURLE_AGAIN;
    }
#पूर्ण_अगर
    अन्यथा {
      /* untreated error */
      sslerr_t errdetail;
      अक्षर error_बफर[256] = "";
      CURLcode परिणाम;
      दीर्घ lerr;
      पूर्णांक lib;
      पूर्णांक reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      अगर((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#अगर_घोषित SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#पूर्ण_अगर
         )) {
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_परिणाम(octx->ssl);
        अगर(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(तथ्य, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        अन्यथा
          failf(तथ्य, "%s", "SSL certificate verification failed");
      }
#अगर_घोषित SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      अन्यथा अगर((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        परिणाम = CURLE_SSL_CLIENTCERT;
        failf(तथ्य, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_बफर, माप(error_बफर)));
      }
#पूर्ण_अगर
#अगर_घोषित HAVE_SSL_SET1_ECH_CONFIG_LIST
      अन्यथा अगर((lib == ERR_LIB_SSL) &&
#अगर_अघोषित HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#अन्यथा
              (reason == SSL_R_ECH_REJECTED)) {
#पूर्ण_अगर /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, तथ्य, octx->ssl, reason);

        परिणाम = CURLE_ECH_REQUIRED;
        failf(तथ्य, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_बफर, माप(error_बफर)));
      }
#पूर्ण_अगर
      अन्यथा {
        परिणाम = CURLE_SSL_CONNECT_ERROR;
        failf(तथ्य, "TLS connect error: %s",
              ossl_strerror(errdetail, error_बफर, माप(error_बफर)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      अगर(परिणाम == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        अक्षर extramsg[80] = "";
        पूर्णांक sockerr = SOCKERRNO;

        अगर(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, माप(extramsg));
        failf(तथ्य, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_to_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->port);
      }

      वापस परिणाम;
    }
  }
  अन्यथा {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(तथ्य, octx);

#अगर defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    अगर(Curl_ssl_ech_enabled(cf)) {
      काष्ठा ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      अक्षर *inner = शून्य, *outer = शून्य;
      पूर्णांक rv;
      VERBOSE(स्थिर अक्षर *status);

      rv = SSL_ech_get1_status(octx->ssl, &inner, &outer);
      चयन(rv) {
      हाल SSL_ECH_STATUS_SUCCESS:
        VERBOSE(status = "succeeded");
        अवरोध;
      हाल SSL_ECH_STATUS_GREASE_ECH:
        VERBOSE(status = "sent GREASE, got retry-configs");
        अवरोध;
      हाल SSL_ECH_STATUS_GREASE:
        VERBOSE(status = "sent GREASE");
        अवरोध;
      हाल SSL_ECH_STATUS_NOT_TRIED:
        VERBOSE(status = "not attempted");
        अवरोध;
      हाल SSL_ECH_STATUS_NOT_CONFIGURED:
        VERBOSE(status = "not configured");
        अवरोध;
      हाल SSL_ECH_STATUS_BACKEND:
        VERBOSE(status = "backend (unexpected)");
        अवरोध;
      हाल SSL_ECH_STATUS_FAILED:
        VERBOSE(status = "failed");
        अवरोध;
      हाल SSL_ECH_STATUS_BAD_CALL:
        VERBOSE(status = "bad call (unexpected)");
        अवरोध;
      हाल SSL_ECH_STATUS_BAD_NAME: {
        अगर(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !म_भेद(inner, connssl->peer.origin->hostname)) {
          VERBOSE(status = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_STATUS_SUCCESS;
        }
        अन्यथा {
          VERBOSE(status = "bad name (unexpected)");
        }
        अवरोध;
      }
      शेष:
        VERBOSE(status = "unexpected status");
        infof(तथ्य, "ECH: unexpected status %d", rv);
      }
      infof(तथ्य, "ECH: result '%s' (inner=%s, outer=%s)",
            (status ? status : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      अगर(rv == SSL_ECH_STATUS_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, तथ्य, octx->ssl, 0);
      }
      अगर(rv != SSL_ECH_STATUS_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(तथ्य, "ECH: ech-hard failed");
        वापस CURLE_SSL_CONNECT_ERROR;
      }
    }
    अन्यथा {
      infof(तथ्य, "ECH: result: status is not attempted");
    }
#पूर्ण_अगर /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    अगर(connssl->alpn) {
      स्थिर अचिन्हित अक्षर *neg_protocol;
      अचिन्हित पूर्णांक len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      वापस Curl_alpn_set_negotiated(cf, तथ्य, connssl, neg_protocol, len);
    }

    वापस CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
अटल CURLcode ossl_pkp_pin_peer_pubkey(काष्ठा Curl_easy *तथ्य, X509 *cert,
                                         स्थिर अक्षर *pinnedpubkey)
{
  /* Scratch */
  पूर्णांक len1 = 0, len2 = 0;
  अचिन्हित अक्षर *buff1 = शून्य, *अस्थायी = शून्य;

  /* Result is returned to caller */
  CURLcode परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  अगर(!pinnedpubkey)
    वापस CURLE_OK;

  अगर(!cert)
    वापस परिणाम;

  करो {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), शून्य);
    अगर(len1 < 1)
      अवरोध; /* failed */

    buff1 = अस्थायी = curlx_malloc(len1);
    अगर(!buff1)
      अवरोध; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &अस्थायी);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    अगर((len1 != len2) || !अस्थायी || ((अस्थायी - buff1) != len1))
      अवरोध; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    परिणाम = Curl_pin_peer_pubkey(तथ्य, pinnedpubkey, buff1, len1);
  } जबतक(0);

  अगर(buff1)
    curlx_free(buff1);

  वापस परिणाम;
}

#अगर_घोषित CURLVERBOSE
#अगर !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_VERSION_संख्या) && LIBRESSL_VERSION_संख्या < 0x3060000fL)
अटल व्योम infof_certstack(काष्ठा Curl_easy *तथ्य, स्थिर SSL *ssl)
{
  ढेर_OF(X509) *certstack;
  दीर्घ verify_परिणाम;
  पूर्णांक अंक_cert_levels;
  पूर्णांक cert_level;

  अगर(!Curl_trc_is_verbose(तथ्य))
    वापस;

  verify_परिणाम = SSL_get_verify_परिणाम(ssl);
  अगर(verify_परिणाम != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  अन्यथा
    certstack = SSL_get0_verified_chain(ssl);
  अगर(!certstack)
    वापस;
  अंक_cert_levels = sk_X509_अंक(certstack);

  क्रम(cert_level = 0; cert_level < अंक_cert_levels; cert_level++) {
    अक्षर cert_algorithm[80] = "";
    अक्षर group_name_final[80] = "";
    स्थिर X509_ALGOR *palg_cert = शून्य;
    स्थिर ASN1_OBJECT *paobj_cert = शून्य;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    पूर्णांक कुंजी_bits;
    पूर्णांक कुंजी_sec_bits;
    पूर्णांक get_group_name;
    स्थिर अक्षर *type_name;

    current_cert = sk_X509_मूल्य(certstack, cert_level);
    अगर(!current_cert)
      जारी;

    current_pkey = X509_get0_pubkey(current_cert);
    अगर(!current_pkey)
      जारी;

    X509_get0_signature(शून्य, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, शून्य, शून्य, palg_cert);
    OBJ_obj2txt(cert_algorithm, माप(cert_algorithm), paobj_cert, 0);

    कुंजी_bits = EVP_PKEY_bits(current_pkey);
#अगर_अघोषित HAVE_OPENSSL3
#घोषणा EVP_PKEY_get_security_bits EVP_PKEY_security_bits
#पूर्ण_अगर
    कुंजी_sec_bits = EVP_PKEY_get_security_bits(current_pkey);
#अगर_घोषित HAVE_OPENSSL3
    {
      अक्षर group_name[80] = "";
      get_group_name = EVP_PKEY_get_group_name(current_pkey, group_name,
                                               माप(group_name), शून्य);
      curl_msnprintf(group_name_final, माप(group_name_final), "/%s",
                     group_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#अन्यथा
    get_group_name = 0;
    type_name = शून्य;
#पूर्ण_अगर

    infof(तथ्य, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_group_name == 0 ? "" : group_name_final,
          कुंजी_bits, कुंजी_sec_bits, cert_algorithm);
  }
}
#अन्यथा
#घोषणा infof_certstack(तथ्य, ssl)
#पूर्ण_अगर
#पूर्ण_अगर /* CURLVERBOSE */

अटल CURLcode ossl_check_issuer(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  X509 *server_cert)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = शून्य;
  BIO *fp = शून्य;
  अक्षर err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode परिणाम = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  अगर(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->तथ्य,
                         (पूर्णांक)conn_config->issuercert_blob->len);
    अगर(!fp) {
      failf(तथ्य, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, माप(err_buf)));
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }
  }
  अन्यथा अगर(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    अगर(!fp) {
      failf(तथ्य, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, माप(err_buf)));
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ out;
    }

    अगर(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      अगर(verify_enabled)
        failf(तथ्य, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      जाओ out;
    }
  }

  अगर(fp) {
    issuer = PEM_read_bio_X509(fp, शून्य, ZERO_NULL, शून्य);
    अगर(!issuer) {
      अगर(verify_enabled)
        failf(तथ्य, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      जाओ out;
    }

    अगर(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      अगर(verify_enabled)
        failf(तथ्य, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      जाओ out;
    }

    infof(तथ्य, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

out:
  अगर(fp)
    BIO_free(fp);
  अगर(issuer)
    X509_free(issuer);
  वापस परिणाम;
}

अटल स्थिर अक्षर *pinned(काष्ठा Curl_cfilter *cf,
                          काष्ठा Curl_easy *तथ्य)
{
  (व्योम)cf;
  वापस
#अगर_अघोषित CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#पूर्ण_अगर
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY);
}

अटल CURLcode ossl_check_pinned_कुंजी(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      X509 *server_cert)
{
  CURLcode परिणाम = CURLE_OK;
  स्थिर अक्षर *ptr = pinned(cf, तथ्य);
  अगर(ptr) {
    परिणाम = ossl_pkp_pin_peer_pubkey(तथ्य, server_cert, ptr);
    अगर(परिणाम)
      failf(तथ्य, "SSL: public key does not match pinned public key");
  }
  वापस परिणाम;
}

#अगर_घोषित CURLVERBOSE
#घोषणा MAX_CERT_NAME_लंबाई 2048
अटल CURLcode ossl_infof_cert(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                X509 *server_cert)
{
  BIO *mem = शून्य;
  काष्ठा dynbuf dname;
  अक्षर err_buf[256] = "";
  अक्षर *buf;
  दीर्घ len;
  CURLcode परिणाम = CURLE_OK;

  अगर(!Curl_trc_is_verbose(तथ्य))
    वापस CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_लंबाई);
  mem = BIO_new(BIO_s_mem());
  अगर(!mem) {
    failf(तथ्य, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, माप(err_buf)));
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  infof(तथ्य, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  परिणाम = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(तथ्य, "  subject: %s", परिणाम ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_तथ्य(mem, (अक्षर **)&buf);
  infof(तथ्य, "  start date: %.*s", (पूर्णांक)len, buf);
  (व्योम)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_तथ्य(mem, (अक्षर **)&buf);
  infof(तथ्य, "  expire date: %.*s", (पूर्णांक)len, buf);
  (व्योम)BIO_reset(mem);

  परिणाम = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  अगर(परिणाम) /* should be only fatal stuff like OOM */
    जाओ out;
  infof(तथ्य, "  issuer: %s", curlx_dyn_ptr(&dname));

out:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  वापस परिणाम;
}
#पूर्ण_अगर /* CURLVERBOSE */

#अगर_घोषित USE_APPLE_SECTRUST
काष्ठा ossl_certs_ctx {
  ढेर_OF(X509) *sk;
  माप_प्रकार अंक_certs;
  अचिन्हित अक्षर *last_der;
};

अटल CURLcode ossl_chain_get_der(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   व्योम *user_तथ्य,
                                   माप_प्रकार i,
                                   अचिन्हित अक्षर **pder,
                                   माप_प्रकार *pder_len)
{
  काष्ठा ossl_certs_ctx *chain = user_तथ्य;
  X509 *cert;
  पूर्णांक der_len;

  OPENSSL_free(chain->last_der);
  chain->last_der = शून्य;

  (व्योम)cf;
  (व्योम)तथ्य;
  *pder_len = 0;
  *pder = शून्य;

  अगर(i >= chain->अंक_certs)
    वापस CURLE_TOO_LARGE;
  cert = sk_X509_मूल्य(chain->sk, (पूर्णांक)i);
  अगर(!cert)
    वापस CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  अगर(der_len < 0)
    वापस CURLE_FAILED_INIT;
  chain->last_der = *pder;
  *pder_len = (माप_प्रकार)der_len;
  वापस CURLE_OK;
}

अटल CURLcode ossl_apple_verify(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ossl_ctx *octx,
                                  काष्ठा ssl_peer *peer)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा ossl_certs_ctx chain;
  CURLcode परिणाम;

  octx->sectrust_verified = FALSE;
  स_रखो(&chain, 0, माप(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.अंक_certs = chain.sk ? sk_X509_अंक(chain.sk) : 0;

  अगर(!chain.अंक_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    अगर(!octx->reused_session) {
      failf(तथ्य, "SSL: could not get peer certificate chain");
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
    अन्यथा {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(तथ्य, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      वापस CURLE_OK;
    }
  }
  अन्यथा {
#अगर_घोषित HAVE_BORINGSSL_LIKE
    स्थिर uint8_t *ocsp_तथ्य = शून्य;
#अन्यथा
    अचिन्हित अक्षर *ocsp_तथ्य = शून्य;
#पूर्ण_अगर
    दीर्घ ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    अगर(!octx->reused_session)
      ocsp_len = (दीर्घ)SSL_get_tlsext_status_ocsp_resp(octx->ssl, &ocsp_तथ्य);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    अगर(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_तथ्य = शून्य;
    }

    परिणाम = Curl_vtls_apple_verify(cf, तथ्य, peer, chain.अंक_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_तथ्य, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.last_der);
    chain.last_der = शून्य;
  }
  octx->sectrust_verified = !परिणाम;
  वापस परिणाम;
}
#पूर्ण_अगर /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ossl_ctx *octx,
                                   काष्ठा ssl_peer *peer)
{
  काष्ठा connectdata *conn = cf->conn;
  काष्ठा ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, तथ्य);
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  दीर्घ ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  अगर(तथ्य->set.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    परिणाम = ossl_certchain(तथ्य, octx->ssl);
    अगर(परिणाम)
      वापस परिणाम;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  अगर(!server_cert) {
    /* no verification at all, this maybe acceptable */
    अगर(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, तथ्य))
      जाओ out;

    failf(तथ्य, "SSL: could not get peer certificate");
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    जाओ out;
  }

#अगर_घोषित CURLVERBOSE
  परिणाम = ossl_infof_cert(cf, तथ्य, server_cert);
  अगर(परिणाम)
    जाओ out;
  infof_certstack(तथ्य, octx->ssl);
#पूर्ण_अगर

  अगर(conn_config->verifyhost) {
    परिणाम = ossl_verifyhost(तथ्य, conn, peer, server_cert);
    अगर(परिणाम)
      जाओ out;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_परिणाम(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(तथ्य, "OpenSSL verify result: %lx", (अचिन्हित दीर्घ)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  अगर(verified)
    infof(तथ्य, "SSL certificate verified via OpenSSL.");

#अगर_घोषित USE_APPLE_SECTRUST
  अगर(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    परिणाम = ossl_apple_verify(cf, तथ्य, octx, peer);
    अगर(परिणाम && (परिणाम != CURLE_PEER_FAILED_VERIFICATION))
      जाओ out; /* unexpected error */
    अगर(octx->sectrust_verified) {
      infof(तथ्य, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#पूर्ण_अगर

  अगर(!verified) {
    /* no trust established, report the OpenSSL status */
    अगर(conn_config->verifypeer) {
      failf(तथ्य, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
      जाओ out;
    }
    infof(तथ्य, " SSL certificate verification failed, continuing anyway!");
  }

#अगर_अघोषित OPENSSL_NO_OCSP
  अगर(conn_config->verifystatus &&
#अगर_घोषित USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, तथ्य)) &&
#पूर्ण_अगर
     !octx->reused_session) {
    परिणाम = verifystatus(cf, तथ्य, octx);
    अगर(परिणाम)
      जाओ out;
  }
#पूर्ण_अगर

  परिणाम = ossl_check_issuer(cf, तथ्य, server_cert);
  अगर(परिणाम)
    जाओ out;

  परिणाम = ossl_check_pinned_कुंजी(cf, तथ्य, server_cert);

out:
  X509_free(server_cert);
  वापस परिणाम;
}

अटल CURLcode ossl_connect_step3(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  परिणाम = Curl_ossl_check_peer_cert(cf, तथ्य, octx, &connssl->peer);
  अगर(परिणाम)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, तथ्य, connssl->peer.scache_कुंजी);

  वापस परिणाम;
}

#अगर_घोषित HAVE_OPENSSL_EARLYDATA
अटल CURLcode ossl_send_earlydata(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  स्थिर अचिन्हित अक्षर *buf;
  माप_प्रकार blen, nwritten;
  पूर्णांक rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_परिणाम = CURLE_OK;
  जबतक(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_तथ्य(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(तथ्य, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    अगर(rc <= 0) {
      दीर्घ sslerror;
      अक्षर error_बफर[256];
      पूर्णांक err = SSL_get_error(octx->ssl, rc);

      चयन(err) {
      हाल SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        परिणाम = CURLE_AGAIN;
        जाओ out;
      हाल SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        परिणाम = CURLE_AGAIN;
        जाओ out;
      हाल SSL_ERROR_SYSCALL: {
        पूर्णांक sockerr = SOCKERRNO;

        अगर(octx->io_परिणाम == CURLE_AGAIN) {
          परिणाम = CURLE_AGAIN;
          जाओ out;
        }
        sslerror = ERR_get_error();
        अगर(sslerror)
          ossl_strerror(sslerror, error_बफर, माप(error_बफर));
        अन्यथा अगर(sockerr)
          curlx_strerror(sockerr, error_बफर, माप(error_बफर));
        अन्यथा
          curl_msnprintf(error_बफर, माप(error_बफर), "%s",
                         SSL_ERROR_to_str(err));

        failf(तथ्य, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_बफर, sockerr);
        परिणाम = CURLE_SEND_ERROR;
        जाओ out;
      }
      हाल SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(तथ्य, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_बफर, माप(error_बफर)));
        परिणाम = CURLE_SEND_ERROR;
        जाओ out;
      }
      शेष:
        /* a true error */
        failf(तथ्य, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_to_str(err), SOCKERRNO);
        परिणाम = CURLE_SEND_ERROR;
        जाओ out;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(तथ्य, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  वापस परिणाम;
}
#पूर्ण_अगर /* HAVE_OPENSSL_EARLYDATA */

अटल CURLcode ossl_connect(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             bool *done)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;

  /* check if the connection has already been established */
  अगर(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  अगर(connssl->connecting_state == ssl_connect_1) {
    अगर(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(तथ्य, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(तथ्य, cf, "need HTTPS-RR, delaying connect");
      वापस CURLE_OK;
    }
    CURL_TRC_CF(तथ्य, cf, "ossl_connect, step1");
    परिणाम = ossl_connect_step1(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
  }

  अगर(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(तथ्य, cf, "ossl_connect, step2");
#अगर_घोषित HAVE_OPENSSL_EARLYDATA
    अगर(connssl->earlydata_state == ssl_earlydata_await) {
      जाओ out;
    }
    अन्यथा अगर(connssl->earlydata_state == ssl_earlydata_sending) {
      परिणाम = ossl_send_earlydata(cf, तथ्य);
      अगर(परिणाम)
        जाओ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
#पूर्ण_अगर
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));

    परिणाम = ossl_connect_step2(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
  }

  अगर(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(तथ्य, cf, "ossl_connect, step3");
    परिणाम = ossl_connect_step3(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
    connssl->connecting_state = ssl_connect_done;
#अगर_घोषित HAVE_OPENSSL_EARLYDATA
    अगर(connssl->earlydata_state > ssl_earlydata_none) {
      काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (SSL_get_early_तथ्य_status(octx->ssl) == SSL_EARLY_तथ्य_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#पूर्ण_अगर
  }

  अगर(connssl->connecting_state == ssl_connect_done) {
    CURL_TRC_CF(तथ्य, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

out:
  अगर(परिणाम == CURLE_AGAIN) {
    *done = FALSE;
    वापस CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  वापस परिणाम;
}

अटल bool ossl_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                              स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  (व्योम)तथ्य;
  वापस (bool)connssl->निवेश_pending;
}

अटल CURLcode ossl_send(काष्ठा Curl_cfilter *cf,
                          काष्ठा Curl_easy *तथ्य,
                          स्थिर व्योम *mem,
                          माप_प्रकार len,
                          माप_प्रकार *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  पूर्णांक err;
  अक्षर error_बफर[256];
  sslerr_t sslerror;
  पूर्णांक memlen;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (माप_प्रकार)पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णांक)len;
  अगर(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    अगर(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  अगर(nwritten > 0) {
    अगर(nwritten > memlen) {
      failf(तथ्य, "SSL_write(len=%d) returned %d", memlen, nwritten);
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
    *pnwritten = (माप_प्रकार)nwritten;
  }
  अन्यथा {
    err = SSL_get_error(octx->ssl, nwritten);

    चयन(err) {
    हाल SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      परिणाम = CURLE_AGAIN;
      जाओ out;
    हाल SSL_ERROR_WANT_WRITE:
      परिणाम = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      जाओ out;
    हाल SSL_ERROR_SYSCALL: {
      पूर्णांक sockerr = SOCKERRNO;

      अगर(octx->io_परिणाम == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        परिणाम = CURLE_AGAIN;
        जाओ out;
      }
      sslerror = ERR_get_error();
      अगर(sslerror)
        ossl_strerror(sslerror, error_बफर, माप(error_बफर));
      अन्यथा अगर(sockerr)
        curlx_strerror(sockerr, error_बफर, माप(error_बफर));
      अन्यथा
        curl_msnprintf(error_बफर, माप(error_बफर), "%s",
                       SSL_ERROR_to_str(err));

      failf(तथ्य, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_बफर, sockerr);
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
    हाल SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(तथ्य, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_बफर, माप(error_बफर)));
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
    शेष:
      /* a true error */
      failf(तथ्य, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_to_str(err), SOCKERRNO);
      परिणाम = CURLE_SEND_ERROR;
      जाओ out;
    }
  }

out:
  वापस परिणाम;
}

अटल CURLcode ossl_recv(काष्ठा Curl_cfilter *cf,
                          काष्ठा Curl_easy *तथ्य,   /* transfer */
                          अक्षर *buf,                /* store read data here */
                          माप_प्रकार buffersize,        /* max amount to read */
                          माप_प्रकार *pnread)
{
  अक्षर error_बफर[256];
  अचिन्हित दीर्घ sslerror;
  पूर्णांक buffsize;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (माप_प्रकार)पूर्णांक_उच्च) ? पूर्णांक_उच्च : (पूर्णांक)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  अगर(nread > 0)
    *pnread = (माप_प्रकार)nread;
  अन्यथा {
    /* failed SSL_read */
    पूर्णांक err = SSL_get_error(octx->ssl, nread);

    चयन(err) {
    हाल SSL_ERROR_NONE: /* this is not an error */
      अवरोध;
    हाल SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      अगर(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(तथ्य, cf, "TLS close_notify");
      अवरोध;
    हाल SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      परिणाम = CURLE_AGAIN;
      जाओ out;
    हाल SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      परिणाम = CURLE_AGAIN;
      जाओ out;
    शेष:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      अगर(octx->io_परिणाम == CURLE_AGAIN) {
        परिणाम = CURLE_AGAIN;
        जाओ out;
      }
      sslerror = ERR_get_error();
      अगर((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        पूर्णांक sockerr = SOCKERRNO;
        अगर(sslerror)
          ossl_strerror(sslerror, error_बफर, माप(error_बफर));
        अन्यथा अगर(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_बफर, माप(error_बफर));
        अन्यथा
          curl_msnprintf(error_बफर, माप(error_बफर), "%s",
                         SSL_ERROR_to_str(err));
        failf(तथ्य, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_बफर, sockerr);
        परिणाम = CURLE_RECV_ERROR;
        जाओ out;
      }
      अन्यथा अगर(err == SSL_ERROR_SYSCALL) {
        अगर(octx->io_परिणाम) {
          /* logging handling in underlying filter already */
          परिणाम = octx->io_परिणाम;
        }
        अन्यथा अगर(connssl->peer_closed) {
          failf(तथ्य, "Connection closed abruptly");
          परिणाम = CURLE_RECV_ERROR;
        }
        अन्यथा {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          पूर्णांक sockerr = SOCKERRNO;
          अगर(sockerr)
            curlx_strerror(sockerr, error_बफर, माप(error_बफर));
          अन्यथा {
            curl_msnprintf(error_बफर, माप(error_बफर),
                           "Connection closed abruptly");
          }
          failf(तथ्य, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_बफर, sockerr);
          परिणाम = CURLE_RECV_ERROR;
        }
        जाओ out;
      }
    }
  }

out:
  अगर((!परिणाम && !*pnread) || (परिणाम == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->निवेश_pending = FALSE;
  }
  CURL_TRC_CF(तथ्य, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (पूर्णांक)परिणाम, *pnread, connssl->निवेश_pending);
  वापस परिणाम;
}

अटल CURLcode ossl_get_channel_binding(काष्ठा Curl_easy *तथ्य,
                                         int8_t sockindex,
                                         काष्ठा dynbuf *binding)
{
  X509 *cert;
  पूर्णांक mdnid;
  bool no_digest_acceptable = FALSE;
  स्थिर EVP_MD *algo_type = शून्य;
  स्थिर अक्षर *algo_name = शून्य;
  अचिन्हित पूर्णांक लंबाई;
  अचिन्हित अक्षर buf[EVP_MAX_MD_आकार];

  अटल स्थिर अक्षर prefix[] = "tls-server-end-point:";
  काष्ठा connectdata *conn = तथ्य->conn;
  काष्ठा Curl_cfilter *cf = conn->cfilter[sockindex];
  काष्ठा ossl_ctx *octx = शून्य;
  CURLcode परिणाम = CURLE_OK;

  करो {
    स्थिर काष्ठा Curl_cftype *cft = cf->cft;
    काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;

    अगर(cft->name && !म_भेद(cft->name, "SSL")) {
      octx = (काष्ठा ossl_ctx *)connssl->backend;
      अवरोध;
    }

    cf = cf->अग्र;
  } जबतक(cf);

  अगर(!octx) {
    failf(तथ्य, "Failed to find the SSL filter");
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  अगर(!cert)
    /* No server certificate, do not do channel binding */
    वापस CURLE_OK;

#अगर_घोषित HAVE_OPENSSL3
  {
    पूर्णांक pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    अगर(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(तथ्य, "certificate signature algorithm not recognized");
      परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
      जाओ out;
    }

    अगर(mdnid != NID_undef) {
      अगर(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      अन्यथा
        algo_type = EVP_get_digestbynid(mdnid);
    }
    अन्यथा अगर(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    अन्यथा अगर(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      अक्षर mdname[128] = "";
      पूर्णांक rc = EVP_PKEY_get_default_digest_name(pkey, mdname, माप(mdname));
      bool md_is_undef = !म_भेद(mdname, "UNDEF");

      अगर(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      अन्यथा अगर(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(तथ्य, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#अन्यथा /* HAVE_OPENSSL3 */

  अगर(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, शून्य)) {
    failf(तथ्य,
          "Unable to find digest NID for certificate signature algorithm");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ out;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  अगर(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  अन्यथा {
    algo_type = EVP_get_digestbynid(mdnid);
    अगर(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(तथ्य, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
      जाओ out;
    }
  }

#पूर्ण_अगर /* HAVE_OPENSSL3, else */

  अगर(!algo_type) {
    अगर(no_digest_acceptable) {
      infof(तथ्य, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      परिणाम = CURLE_OK;
      जाओ out;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(तथ्य, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ out;
  }

  अगर(!X509_digest(cert, algo_type, buf, &लंबाई)) {
    failf(तथ्य, "X509_digest() failed for channel binding");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    जाओ out;
  }

  /* Append "tls-server-end-point:" */
  परिणाम = curlx_dyn_addn(binding, prefix, CURL_CSTRLEN(prefix));
  अगर(परिणाम)
    जाओ out;

  /* Append digest */
  परिणाम = curlx_dyn_addn(binding, buf, लंबाई);

out:
  X509_free(cert);
  वापस परिणाम;
}

माप_प्रकार Curl_ossl_version(अक्षर *बफर, माप_प्रकार आकार)
{
#अगर_घोषित LIBRESSL_VERSION_संख्या
  अक्षर *p;
  माप_प्रकार गिनती;
  स्थिर अक्षर *ver = OpenSSL_version(OPENSSL_VERSION);
  अटल स्थिर अक्षर expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  अगर(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  गिनती = curl_msnprintf(बफर, आकार, "%s/%s", OSSL_PACKAGE, ver);
  क्रम(p = बफर; *p; ++p) {
    अगर(ISBLANK(*p))
      *p = '_';
  }
  वापस गिनती;
#या_अगर defined(OPENSSL_IS_AWSLC)
  वापस curl_msnprintf(बफर, आकार, "%s/%s",
                        OSSL_PACKAGE, AWSLC_VERSION_संख्या_STRING);
#या_अगर defined(OPENSSL_IS_BORINGSSL)
#अगर_घोषित CURL_BORINGSSL_VERSION
  वापस curl_msnprintf(बफर, आकार, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_VERSION);
#अन्यथा
  वापस curl_msnprintf(बफर, आकार, OSSL_PACKAGE);
#पूर्ण_अगर
#अन्यथा /* OpenSSL 3+ */
  वापस curl_msnprintf(बफर, आकार, "%s/%s",
                        OSSL_PACKAGE, OpenSSL_version(OPENSSL_VERSION_STRING));
#पूर्ण_अगर
}

/* can be called with data == NULL */
अटल CURLcode ossl_random(काष्ठा Curl_easy *तथ्य,
                            अचिन्हित अक्षर *entropy, माप_प्रकार लंबाई)
{
  पूर्णांक rc;
  अगर(तथ्य) {
    अगर(ossl_seed(तथ्य)) /* Initiate the seed if not already done */
      वापस CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  अन्यथा {
    अगर(!rand_enough())
      वापस CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_bytes(entropy, (ossl_valsize_t)curlx_uztosi(लंबाई));
  वापस rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

अटल CURLcode ossl_sha256sum(स्थिर अचिन्हित अक्षर *निवेश,
                               माप_प्रकार len,
                               अचिन्हित अक्षर *sha256sum /* output */,
                               माप_प्रकार unused)
{
  CURLcode परिणाम = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (व्योम)unused;

  mdctx = EVP_MD_CTX_new();
  अगर(!mdctx)
    वापस CURLE_OUT_OF_MEMORY;
  अगर(!EVP_DigestInit_ex(mdctx, EVP_sha256(), शून्य)) {
    परिणाम = CURLE_FAILED_INIT;
    जाओ out;
  }
  अगर(!EVP_DigestUpdate(mdctx, निवेश, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, शून्य))
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
out:
  EVP_MD_CTX_free(mdctx);
  वापस परिणाम;
}

अटल bool ossl_cert_status_request(व्योम)
{
#अगर_अघोषित OPENSSL_NO_OCSP
  वापस TRUE;
#अन्यथा
  वापस FALSE;
#पूर्ण_अगर
}

अटल व्योम *ossl_get_internals(काष्ठा ssl_connect_तथ्य *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  काष्ठा ossl_ctx *octx = (काष्ठा ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  वापस info == CURLINFO_TLS_SESSION ?
    (व्योम *)octx->ssl_ctx : (व्योम *)octx->ssl;
}

स्थिर काष्ठा Curl_ssl Curl_ssl_openssl = {
  { CURLSSLBACKEND_OPENSSL, "openssl" }, /* info */

  SSLSUPP_CA_PATH |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_SSL_CTX |
#अगर_घोषित HAVE_SSL_CTX_SET_CIPHERSUITES
  SSLSUPP_TLS13_CIPHERSUITES |
#पूर्ण_अगर
#अगर_घोषित HAVE_SSL_CTX_SET1_SIGALGS
  SSLSUPP_SIGNATURE_ALGORITHMS |
#पूर्ण_अगर
#अगर_घोषित HAVE_SSL_SET1_ECH_CONFIG_LIST
  SSLSUPP_ECH |
#पूर्ण_अगर
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_ISSUERCERT_BLOB |
  SSLSUPP_SSL_EC_CURVES |
  SSLSUPP_CRLFILE,

  माप(काष्ठा ossl_ctx),

  ossl_init,                /* init */
  ossl_cleanup,             /* cleanup */
  Curl_ossl_version,        /* version */
  ossl_shutdown,            /* shutdown */
  ossl_तथ्य_pending,        /* data_pending */
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

#पूर्ण_अगर /* USE_OPENSSL */
