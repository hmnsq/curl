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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_OPENSSL

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "formdata.h" /* for the boundary function */
#अन्तर्भाव "url.h" /* for the SSL config check function */
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "vtls/openssl.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_int.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "vtls/keylog.h"
#अन्तर्भाव "vtls/hostcheck.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "multiif.h"
#अन्तर्भाव "curlx/strerr.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/strcopy.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "vtls/apple.h"
#यदि_परिभाषितम् USE_ECH
#अन्तर्भाव "curlx/base64.h"
#यदि_अन्तः

#अन्तर्भाव <openssl/rand.h>
#अन्तर्भाव <openssl/x509v3.h>
#यदि_अपरिभाषितम् OPENSSL_NO_DSA
#अन्तर्भाव <openssl/dsa.h>
#यदि_अन्तः
#अन्तर्भाव <openssl/dh.h>
#अन्तर्भाव <openssl/err.h>
#अन्तर्भाव <openssl/conf.h>
#अन्तर्भाव <openssl/bn.h>
#अन्तर्भाव <openssl/rsa.h>
#अन्तर्भाव <openssl/bio.h>
#अन्तर्भाव <openssl/pkcs12.h>
#अन्तर्भाव <openssl/tls1.h>
#अन्तर्भाव <openssl/evp.h>

#यदि defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#अन्तर्भाव <openssl/ech.h>
#यदि_अन्तः

#यदि_अपरिभाषितम् OPENSSL_NO_OCSP
#अन्तर्भाव <openssl/ocsp.h>
#यदि_अन्तः

#यदि !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#परिभाषय USE_OPENSSL_ENGINE
#अन्तर्भाव <openssl/engine.h>
#यदि_अन्तः

#यदि_परिभाषितम् LIBRESSL_संस्करणम्_सङ्ख्या
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  यदि LIBRESSL_संस्करणम्_सङ्ख्या < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#नोचेद्यदि !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#यदि_अन्तः

#यदि defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#अन्तर्भाव <openssl/provider.h>
#अन्तर्भाव <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#परिभाषय OPENSSL_HAS_PROVIDERS
#यदि_अन्तः

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#यदि !defined(LIBRESSL_संस्करणम्_सङ्ख्या) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_कुञ्चिका_EXPLICIT_PARAMS))
#परिभाषय HAVE_SSL_CTX_समुच्चयः_DEFAULT_READ_बफर_LEN 1
#यदि_अन्तः

#यदि defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#अन्तर्भाव <openssl/ui.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_OPENSSL3
#परिभाषय HAVE_EVP_PKEY_GET_PARAMS 1
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_EVP_PKEY_GET_PARAMS
#अन्तर्भाव <openssl/core_names.h>
#परिभाषय DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = रिक्त
#परिभाषय FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#अन्यथा
#परिभाषय DECLARE_PKEY_PARAM_BIGNUM(name) ध्रुव BIGNUM *name
#परिभाषय FREE_PKEY_PARAM_BIGNUM(name)
#यदि_अन्तः

/* Whether SSL_CTX_set_ciphersuites is available.
 * BoringSSL: no
 * LibreSSL: supported since 3.4.1 (released 2021-10-14)
 * OpenSSL: supported since 1.1.1 (commit a53b5be6a05)
 */
#यदि_अपरिभाषितम् OPENSSL_IS_BORINGSSL
#  define HAVE_SSL_CTX_समुच्चयः_CIPHERSUITES
#  ifndef OPENSSL_IS_AWSLC
#    define HAVE_SSL_CTX_समुच्चयः_POST_HANDSHAKE_AUTH
#  endif
#यदि_अन्तः

/* Whether SSL_CTX_set1_sigalgs_list is available
 * BoringSSL: supported since 0.20240913.0 (commit 826ce15)
 * LibreSSL: no
 * OpenSSL: supported since 1.0.2 (commit 0b362de5f575)
 */
#यदि_अपरिभाषितम् LIBRESSL_संस्करणम्_सङ्ख्या
#परिभाषय HAVE_SSL_CTX_SET1_SIGALGS
#यदि_अन्तः

#यदि_परिभाषितम् LIBRESSL_संस्करणम्_सङ्ख्या
#परिभाषय OSSL_PACKAGE "LibreSSL"
#नोचेद्यदि defined(OPENSSL_IS_AWSLC)
#परिभाषय OSSL_PACKAGE "AWS-LC"
#नोचेद्यदि defined(OPENSSL_IS_BORINGSSL)
#परिभाषय OSSL_PACKAGE "BoringSSL"
#नोचेद्यदि defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#परिभाषय OSSL_PACKAGE "quictls"
#अन्यथा
#परिभाषय OSSL_PACKAGE "OpenSSL"
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_BORINGSSL_LIKE
प्रकारनिर्देश size_t numcert_t;
प्रकारनिर्देश uint32_t sslerr_t;
#अन्यथा
प्रकारनिर्देश पूर्णाङ्क numcert_t;
प्रकारनिर्देश अचिह्नित दीर्घ sslerr_t;
#यदि_अन्तः
#परिभाषय ossl_valsize_t numcert_t

#यदि OPENSSL_संस्करणम्_सङ्ख्या >= 0x40100000L
#परिभाषय ossl_ASN1_STRING_get_दीर्घता ASN1_STRING_get_दीर्घता
#अन्यथा
स्थिर size_t ossl_ASN1_STRING_get_दीर्घता(ध्रुव ASN1_STRING *str)
{
   ध्रुव पूर्णाङ्क दीर्घता = ASN1_STRING_दीर्घता(str);
   प्रत्यागम दीर्घता >= 0 ? (size_t)दीर्घता : 0;
}
#यदि_अन्तः

स्थिर CURLcode प्रक्षिप_certinfo(संरचना Curl_easy *दत्त,
                              BIO *mem, ध्रुव वर्ण *label, पूर्णाङ्क अङ्कः)
  WARN_अप्रयुक्तम्_परिणाम;

स्थिर CURLcode प्रक्षिप_certinfo(संरचना Curl_easy *दत्त,
                              BIO *mem, ध्रुव वर्ण *label, पूर्णाङ्क अङ्कः)
{
  वर्ण *ptr;
  दीर्घ len = BIO_get_mem_दत्त(mem, &ptr);
  CURLcode परिणाम = Curl_ssl_प्रक्षिप_certinfo_len(दत्त, अङ्कः, label, ptr, len);
  (शून्य)BIO_reset(mem);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode pubkey_show(संरचना Curl_easy *दत्त,
                            BIO *mem,
                            पूर्णाङ्क अङ्कः,
                            ध्रुव वर्ण *type,
                            ध्रुव वर्ण *name,
                            ध्रुव BIGNUM *bn) WARN_अप्रयुक्तम्_परिणाम;

स्थिर CURLcode pubkey_show(संरचना Curl_easy *दत्त,
                            BIO *mem,
                            पूर्णाङ्क अङ्कः,
                            ध्रुव वर्ण *type,
                            ध्रुव वर्ण *name,
                            ध्रुव BIGNUM *bn)
{
  वर्ण namebuf[32];

  curl_msnprintf(namebuf, परिमाण(namebuf), "%s(%s)", type, name);

  यदि(bn)
    BN_print(mem, bn);
  प्रत्यागम प्रक्षिप_certinfo(दत्त, mem, namebuf, अङ्कः);
}

#परिभाषय print_pubkey_BN(_type, _name, अङ्कः_2)           \
  pubkey_show(दत्त, mem, अङ्कः_2, #_type, #_name, _name)

स्थिर पूर्णाङ्क asn1_object_dump(ध्रुव ASN1_OBJECT *a, वर्ण *buf, size_t len)
{
  पूर्णाङ्क i = i2t_ASN1_OBJECT(buf, (पूर्णाङ्क)len, a);
  प्रत्यागम (i >= (पूर्णाङ्क)len);  /* buffer too small */
}

स्थिर CURLcode X509V3_ext(संरचना Curl_easy *दत्त,
                           पूर्णाङ्क certnum,
                           ध्रुव राशिः_सम्बन्धि(X509_EXTENSION) *extsarg)
{
  पूर्णाङ्क i;
  CURLcode परिणाम = CURLE_OK;
#यदि_परिभाषितम् LIBRESSL_संस्करणम्_सङ्ख्या
  राशिः_सम्बन्धि(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#अन्यथा
  ध्रुव राशिः_सम्बन्धि(X509_EXTENSION) *exts = extsarg;
#यदि_अन्तः

  यदि((पूर्णाङ्क)sk_X509_EXTENSION_अङ्कः(exts) <= 0)
    /* no extensions, bail out */
    प्रत्यागम परिणाम;

  प्रति(i = 0; i < (पूर्णाङ्क)sk_X509_EXTENSION_अङ्कः(exts); i++) {
    ध्रुव ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_मानम्(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    वर्ण namebuf[128];
    BIO *bio_बहिर्गमनम् = BIO_new(BIO_s_mem());

    यदि(!bio_बहिर्गमनम्)
      प्रत्यागम परिणाम;

    obj = X509_EXTENSION_get_object(ext);

    यदि(asn1_object_dump(obj, namebuf, परिमाण(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    यदि(!X509V3_EXT_print(bio_बहिर्गमनम्, ext, 0, 0))
      ASN1_STRING_print(bio_बहिर्गमनम्,
                        (ध्रुव ASN1_STRING *)X509_EXTENSION_get_दत्त(ext));

    BIO_get_mem_ptr(bio_बहिर्गमनम्, &biomem);
    परिणाम = Curl_ssl_प्रक्षिप_certinfo_len(दत्त, certnum, namebuf, biomem->दत्त,
                                        biomem->दीर्घता);
    BIO_free(bio_बहिर्गमनम्);
    यदि(परिणाम)
      भङ्ग;
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode get_pkey_rsa(संरचना Curl_easy *दत्त,
                             EVP_PKEY *pubkey, BIO *mem, पूर्णाङ्क i)
{
  CURLcode परिणाम = CURLE_OK;
#यदि_अपरिभाषितम् HAVE_EVP_PKEY_GET_PARAMS
  RSA *rsa = EVP_PKEY_get0_RSA(pubkey);
#यदि_अन्तः /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(n);
  DECLARE_PKEY_PARAM_BIGNUM(e);
#यदि_परिभाषितम् HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_N, &n);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_E, &e);
#अन्यथा
  RSA_get0_कुञ्चिका(rsa, &n, &e, रिक्त);
#यदि_अन्तः /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (पूर्णाङ्क)(n ? BN_अङ्कः_द्वयङ्काः(n) : 0));
  परिणाम = प्रक्षिप_certinfo(दत्त, mem, "RSA Public Key", i);
  यदि(!परिणाम) {
    परिणाम = print_pubkey_BN(rsa, n, i);
    यदि(!परिणाम)
      परिणाम = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् OPENSSL_NO_DSA
स्थिर CURLcode get_pkey_dsa(संरचना Curl_easy *दत्त,
                             EVP_PKEY *pubkey, BIO *mem, पूर्णाङ्क i)
{
  CURLcode परिणाम = CURLE_OK;
#यदि_अपरिभाषितम् HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#यदि_अन्तः /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_कुञ्चिका);
#यदि_परिभाषितम् HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_कुञ्चिका, &pub_कुञ्चिका);
#अन्यथा
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_कुञ्चिका(dsa, &pub_कुञ्चिका, रिक्त);
#यदि_अन्तः /* HAVE_EVP_PKEY_GET_PARAMS */
  परिणाम = print_pubkey_BN(dsa, p, i);
  यदि(!परिणाम)
    परिणाम = print_pubkey_BN(dsa, q, i);
  यदि(!परिणाम)
    परिणाम = print_pubkey_BN(dsa, g, i);
  यदि(!परिणाम)
    परिणाम = print_pubkey_BN(dsa, pub_कुञ्चिका, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_कुञ्चिका);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः /* !OPENSSL_NO_DSA */

स्थिर CURLcode get_pkey_dh(संरचना Curl_easy *दत्त,
                            EVP_PKEY *pubkey, BIO *mem, पूर्णाङ्क i)
{
  CURLcode परिणाम;
#यदि_अपरिभाषितम् HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#यदि_अन्तः /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_कुञ्चिका);
#यदि_परिभाषितम् HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_कुञ्चिका, &pub_कुञ्चिका);
#अन्यथा
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_कुञ्चिका(dh, &pub_कुञ्चिका, रिक्त);
#यदि_अन्तः /* HAVE_EVP_PKEY_GET_PARAMS */
  परिणाम = print_pubkey_BN(dh, p, i);
  यदि(!परिणाम)
    परिणाम = print_pubkey_BN(dh, q, i);
  यदि(!परिणाम)
    परिणाम = print_pubkey_BN(dh, g, i);
  यदि(!परिणाम)
    परिणाम = print_pubkey_BN(dh, pub_कुञ्चिका, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_कुञ्चिका);
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
प्रकारनिर्देश ध्रुव X509_PUBKEY pubkeytype_t;
#अन्यथा
प्रकारनिर्देश X509_PUBKEY pubkeytype_t;
#यदि_अन्तः

स्थिर CURLcode ossl_certchain(संरचना Curl_easy *दत्त, SSL *ssl)
{
  CURLcode परिणाम;
  राशिः_सम्बन्धि(X509) *sk;
  पूर्णाङ्क i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  यदि(!sk)
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_अङ्कः(sk);
  यदि(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(दत्त, "%d certificates is more than allowed (%d)", (पूर्णाङ्क)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  परिणाम = Curl_ssl_init_certinfo(दत्त, (पूर्णाङ्क)numcerts);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  mem = BIO_new(BIO_s_mem());
  यदि(!mem)
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रति(i = 0; !परिणाम && (i < (पूर्णाङ्क)numcerts); i++) {
    X509 *x = sk_X509_मानम्(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = रिक्त;
    size_t j;
    ध्रुव ASN1_BIT_STRING *psig = रिक्त;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Subject", i);
    यदि(परिणाम)
      भङ्ग;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Issuer", i);
    यदि(परिणाम)
      भङ्ग;

    BIO_printf(mem, "%lx", (अचिह्नित दीर्घ)X509_get_संस्करणम्(x));
    परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Version", i);
    यदि(परिणाम)
      भङ्ग;

    {
      ASN1_INTEGER *अङ्कः = X509_get_serial_सङ्ख्या(x);
      ध्रुव अचिह्नित वर्ण *numdata = ASN1_STRING_get0_दत्त(अङ्कः);
      ध्रुव size_t numlen = ossl_ASN1_STRING_get_दीर्घता(अङ्कः);
      यदि(ASN1_STRING_type(अङ्कः) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      प्रति(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Serial Number", i);
    यदि(परिणाम)
      भङ्ग;

    {
      ध्रुव X509_ALGOR *sigalg = रिक्त;
      pubkeytype_t *xpubkey = रिक्त;
      ASN1_OBJECT *pubkeyoid = रिक्त;

      X509_get0_signature(&psig, &sigalg, x);
      यदि(sigalg) {
        ध्रुव ASN1_OBJECT *sigalgoid = रिक्त;
        X509_ALGOR_get0(&sigalgoid, रिक्त, रिक्त, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Signature Algorithm", i);
        यदि(परिणाम)
          भङ्ग;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      यदि(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, रिक्त, रिक्त, रिक्त, xpubkey);
        यदि(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Public Key Algorithm", i);
          यदि(परिणाम)
            भङ्ग;
        }
      }

      परिणाम = X509V3_ext(दत्त, i, X509_get0_extensions(x));
      यदि(परिणाम)
        भङ्ग;
    }

    ASN1_TIME_print(mem, X509_get0_notBefore(x));
    परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Start date", i);
    यदि(परिणाम)
      भङ्ग;

    ASN1_TIME_print(mem, X509_get0_notAfter(x));
    परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Expire date", i);
    यदि(परिणाम)
      भङ्ग;

    pubkey = X509_get_pubkey(x);
    यदि(!pubkey)
      infof(दत्त, "   Unable to load public key");
    अन्यथा {
      विकल्प(EVP_PKEY_id(pubkey)) {
      पक्ष EVP_PKEY_RSA:
        परिणाम = get_pkey_rsa(दत्त, pubkey, mem, i);
        भङ्ग;

#यदि_अपरिभाषितम् OPENSSL_NO_DSA
      पक्ष EVP_PKEY_DSA:
        परिणाम = get_pkey_dsa(दत्त, pubkey, mem, i);
        भङ्ग;
#यदि_अन्तः

      पक्ष EVP_PKEY_DH:
        परिणाम = get_pkey_dh(दत्त, pubkey, mem, i);
        भङ्ग;
      }
      EVP_PKEY_free(pubkey);
    }

    यदि(!परिणाम && psig) {
      ध्रुव अचिह्नित वर्ण *psigdata = ASN1_STRING_get0_दत्त(psig);
      ध्रुव size_t psiglen = ossl_ASN1_STRING_get_दीर्घता(psig);
      प्रति(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Signature", i);
    }

    यदि(!परिणाम) {
      PEM_write_bio_X509(mem, x);
      परिणाम = प्रक्षिप_certinfo(दत्त, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  यदि(परिणाम)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(दत्त);

  प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क ossl_bio_cf_create(BIO *bio)
{
  BIO_समुच्चयः_shutdown(bio, 1);
  BIO_समुच्चयः_init(bio, 1);
  BIO_समुच्चयः_दत्त(bio, रिक्त);
  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क ossl_bio_cf_destroy(BIO *bio)
{
  यदि(!bio)
    प्रत्यागम 0;
  प्रत्यागम 1;
}

स्थिर दीर्घ ossl_bio_cf_ctrl(BIO *bio, पूर्णाङ्क cmd, दीर्घ अङ्कः, शून्य *ptr)
{
  संरचना Curl_cfilter *cf = BIO_get_दत्त(bio);
  दीर्घ ret = 1;

  (शून्य)cf;
  (शून्य)ptr;
  विकल्प(cmd) {
  पक्ष BIO_CTRL_GET_CLOSE:
    ret = (दीर्घ)BIO_get_shutdown(bio);
    भङ्ग;
  पक्ष BIO_CTRL_समुच्चयः_CLOSE:
    BIO_समुच्चयः_shutdown(bio, (पूर्णाङ्क)अङ्कः);
    भङ्ग;
  पक्ष BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    भङ्ग;
  पक्ष BIO_CTRL_DUP:
    ret = 1;
    भङ्ग;
  पक्ष BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    संरचना ssl_connect_दत्त *connssl = cf->ctx;
    प्रत्यागम connssl->peer_closed;
  }
  स्वभाव:
    ret = 0;
    भङ्ग;
  }
  प्रत्यागम ret;
}

स्थिर पूर्णाङ्क ossl_bio_cf_बहिर्गमनम्_write(BIO *bio, ध्रुव वर्ण *buf, पूर्णाङ्क blen)
{
  संरचना Curl_cfilter *cf = BIO_get_दत्त(bio);
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  size_t nwritten;
  CURLcode परिणाम;

  DEBUGASSERT(दत्त);
  यदि(blen < 0)
    प्रत्यागम 0;

  परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त,
                             (ध्रुव uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(दत्त, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (पूर्णाङ्क)परिणाम, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_परिणाम = परिणाम;
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN)
      BIO_समुच्चयः_retry_write(bio);
    प्रत्यागम -1;
  }
  प्रत्यागम (पूर्णाङ्क)nwritten;
}

स्थिर पूर्णाङ्क ossl_bio_cf_अन्तर्_read(BIO *bio, वर्ण *buf, पूर्णाङ्क blen)
{
  संरचना Curl_cfilter *cf = BIO_get_दत्त(bio);
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  size_t nread;
  CURLcode परिणाम, r2;

  DEBUGASSERT(दत्त);
  /* OpenSSL catches this case, so should we. */
  यदि(!buf)
    प्रत्यागम 0;
  यदि(blen < 0)
    प्रत्यागम 0;

  परिणाम = Curl_conn_cf_recv(cf->अग्र, दत्त, buf, (size_t)blen, &nread);
  CURL_TRC_CF(दत्त, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (पूर्णाङ्क)परिणाम, nread);
  BIO_clear_retry_flags(bio);
  octx->io_परिणाम = परिणाम;
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN)
      BIO_समुच्चयः_retry_read(bio);
  }
  अन्यथा {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->निवेशः_pending = TRUE;
    यदि(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  यदि(!octx->x509_store_सज्जता) {
    r2 = Curl_ssl_सज्जता_x509_store(cf, दत्त, octx);
    यदि(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_परिणाम = r2;
      प्रत्यागम -1;
    }
    octx->x509_store_सज्जता = TRUE;
  }
  प्रत्यागम परिणाम ? -1 : (पूर्णाङ्क)nread;
}

स्थिर BIO_विधानम् *ossl_bio_cf_विधानम्_create(शून्य)
{
  BIO_विधानम् *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  यदि(m) {
    BIO_meth_समुच्चयः_write(m, &ossl_bio_cf_बहिर्गमनम्_write);
    BIO_meth_समुच्चयः_read(m, &ossl_bio_cf_अन्तर्_read);
    BIO_meth_समुच्चयः_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_समुच्चयः_create(m, &ossl_bio_cf_create);
    BIO_meth_समुच्चयः_destroy(m, &ossl_bio_cf_destroy);
  }
  प्रत्यागम m;
}

स्थिर शून्य ossl_bio_cf_विधानम्_free(BIO_विधानम् *m)
{
  यदि(m)
    BIO_meth_free(m);
}

#यदि_अपरिभाषितम् HAVE_KEYLOG_UPSTREAM
#यदि_परिभाषितम् HAVE_KEYLOG_CALLBACK
स्थिर शून्य ossl_keylog_callback(ध्रुव SSL *ssl, ध्रुव वर्ण *पङ्क्तिः)
{
  (शून्य)ssl;

  Curl_tls_keylog_write_पङ्क्तिः(पङ्क्तिः);
}
#अन्यथा
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
स्थिर शून्य ossl_log_tls12_secret(ध्रुव SSL *ssl, bool *keylog_समाप्तम्)
{
  ध्रुव SSL_SESSION *session;
  अचिह्नित वर्ण client_random[SSL3_RANDOM_आकार];
  अचिह्नित वर्ण master_कुञ्चिका[SSL_MAX_MASTER_कुञ्चिका_दीर्घता];
  पूर्णाङ्क master_कुञ्चिका_दीर्घता = 0;

  ERR_समुच्चयः_mark();

  session = SSL_get_session(ssl);

  यदि(!session || *keylog_समाप्तम्) {
    ERR_निष्कासय_गन्तव्यम्_mark();
    प्रत्यागम;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_आकार);
  master_कुञ्चिका_दीर्घता = (पूर्णाङ्क)
    SSL_SESSION_get_master_कुञ्चिका(session, master_कुञ्चिका, SSL_MAX_MASTER_कुञ्चिका_दीर्घता);

  ERR_निष्कासय_गन्तव्यम्_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  यदि(master_कुञ्चिका_दीर्घता <= 0)
    प्रत्यागम;

  *keylog_समाप्तम् = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        परिमाण(client_random),
                        master_कुञ्चिका, master_कुञ्चिका_दीर्घता);
}
#यदि_अन्तः /* !HAVE_KEYLOG_CALLBACK */
#यदि_अन्तः /* HAVE_KEYLOG_UPSTREAM */

स्थिर ध्रुव वर्ण *SSL_ERROR_गन्तव्यम्_str(पूर्णाङ्क err)
{
  विकल्प(err) {
  पक्ष SSL_ERROR_NONE:
    प्रत्यागम "SSL_ERROR_NONE";
  पक्ष SSL_ERROR_SSL:
    प्रत्यागम "SSL_ERROR_SSL";
  पक्ष SSL_ERROR_WANT_READ:
    प्रत्यागम "SSL_ERROR_WANT_READ";
  पक्ष SSL_ERROR_WANT_WRITE:
    प्रत्यागम "SSL_ERROR_WANT_WRITE";
  पक्ष SSL_ERROR_WANT_X509_LOOKUP:
    प्रत्यागम "SSL_ERROR_WANT_X509_LOOKUP";
  पक्ष SSL_ERROR_SYSCALL:
    प्रत्यागम "SSL_ERROR_SYSCALL";
  पक्ष SSL_ERROR_ZERO_RETURN:
    प्रत्यागम "SSL_ERROR_ZERO_RETURN";
  पक्ष SSL_ERROR_WANT_CONNECT:
    प्रत्यागम "SSL_ERROR_WANT_CONNECT";
  पक्ष SSL_ERROR_WANT_ACCEPT:
    प्रत्यागम "SSL_ERROR_WANT_ACCEPT";
#यदि_परिभाषितम् SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  पक्ष SSL_ERROR_WANT_ASYNC:
    प्रत्यागम "SSL_ERROR_WANT_ASYNC";
#यदि_अन्तः
#यदि_परिभाषितम् SSL_ERROR_WANT_ASYNC_JOB  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  पक्ष SSL_ERROR_WANT_ASYNC_JOB:
    प्रत्यागम "SSL_ERROR_WANT_ASYNC_JOB";
#यदि_अन्तः
#यदि_परिभाषितम् SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  पक्ष SSL_ERROR_WANT_CLIENT_HELLO_CB:
    प्रत्यागम "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#यदि_अन्तः
  स्वभाव:
    प्रत्यागम "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
स्थिर वर्ण *ossl_strerror(अचिह्नित दीर्घ error, वर्ण *buf, size_t आकार)
{
  size_t len;
  DEBUGASSERT(आकार);
  *buf = '\0';

  len = Curl_ossl_संस्करणम्(buf, आकार);
  DEBUGASSERT(len < (आकार - 2));
  यदि(len < (आकार - 2)) {
    buf += len;
    आकार -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#यदि_परिभाषितम् HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, आकार);
#अन्यथा
  ERR_error_string_n(error, buf, आकार);
#यदि_अन्तः

  यदि(!*buf) {
    ध्रुव वर्ण *सन्देशः = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, आकार, सन्देशः, वाक्यदैर्घ्यम्(सन्देशः));
  }

  प्रत्यागम buf;
}

स्थिर पूर्णाङ्क passwd_callback(वर्ण *buf, पूर्णाङ्क अङ्कः, पूर्णाङ्क encrypting, शून्य *password)
{
  DEBUGASSERT(encrypting == 0);

  यदि(!encrypting && अङ्कः >= 0 && password) {
    पूर्णाङ्क klen = curlx_uztosi(वाक्यदैर्घ्यम्((वर्ण *)password));
    यदि(अङ्कः > klen) {
      स्मृतिं_प्रतिलिख(buf, password, klen + 1);
      प्रत्यागम klen;
    }
  }
  प्रत्यागम 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
स्थिर bool rand_enough(शून्य)
{
  प्रत्यागम RAND_स्थितिः() != 0;
}

स्थिर CURLcode ossl_बीजम्(संरचना Curl_easy *दत्त)
{
  /* This might get called before it has been added to a multi handle */
  यदि(दत्त->multi && दत्त->multi->ssl_seeded)
    प्रत्यागम CURLE_OK;

  यदि(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    यदि(दत्त->multi)
      दत्त->multi->ssl_seeded = TRUE;
    प्रत्यागम CURLE_OK;
  }
  failf(दत्त, "Insufficient randomness");
  प्रत्यागम CURLE_SSL_CONNECT_ERROR;
}

#यदि_अपरिभाषितम् SSL_FILETYPE_ENGINE
#परिभाषय SSL_FILETYPE_ENGINE 42
#यदि_अन्तः
#यदि_अपरिभाषितम् SSL_FILETYPE_PKCS12
#परिभाषय SSL_FILETYPE_PKCS12 43
#यदि_अन्तः
#यदि_अपरिभाषितम् SSL_FILETYPE_PROVIDER
#परिभाषय SSL_FILETYPE_PROVIDER 44
#यदि_अन्तः
स्थिर पूर्णाङ्क ossl_do_file_type(ध्रुव वर्ण *type)
{
  यदि(!type || !type[0])
    प्रत्यागम SSL_FILETYPE_PEM;
  यदि(curl_strequal(type, "PEM"))
    प्रत्यागम SSL_FILETYPE_PEM;
  यदि(curl_strequal(type, "DER"))
    प्रत्यागम SSL_FILETYPE_ASN1;
  यदि(curl_strequal(type, "PROV"))
    प्रत्यागम SSL_FILETYPE_PROVIDER;
  यदि(curl_strequal(type, "ENG"))
    प्रत्यागम SSL_FILETYPE_ENGINE;
  यदि(curl_strequal(type, "P12"))
    प्रत्यागम SSL_FILETYPE_PKCS12;
  प्रत्यागम -1;
}

#यदि defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
स्थिर पूर्णाङ्क ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  ध्रुव वर्ण *password;
  विकल्प(UI_get_string_type(uis)) {
  पक्ष UIT_PROMPT:
  पक्ष UIT_VERIFY:
    password = (ध्रुव वर्ण *)UI_get0_user_दत्त(ui);
    यदि(password && (UI_get_निवेशः_flags(uis) & UI_निवेशः_FLAG_DEFAULT_PWD)) {
      UI_समुच्चयः_परिणाम(ui, uis, password);
      प्रत्यागम 1;
    }
    FALLTHROUGH();
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम UI_विधानम्_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
स्थिर पूर्णाङ्क ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  विकल्प(UI_get_string_type(uis)) {
  पक्ष UIT_PROMPT:
  पक्ष UIT_VERIFY:
    यदि(UI_get0_user_दत्त(ui) &&
       (UI_get_निवेशः_flags(uis) & UI_निवेशः_FLAG_DEFAULT_PWD)) {
      प्रत्यागम 1;
    }
    FALLTHROUGH();
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम UI_विधानम्_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
स्थिर bool is_pkcs11_uri(ध्रुव वर्ण *string)
{
  प्रत्यागम string && curl_strnequal(string, "pkcs11:", 7);
}

#यदि_अन्तः

स्थिर CURLcode ossl_समुच्चयः_engine(संरचना Curl_easy *दत्त, संरचना ossl_ctx *octx,
                                ध्रुव वर्ण *name);
#यदि_परिभाषितम् OPENSSL_HAS_PROVIDERS
स्थिर CURLcode ossl_समुच्चयः_provider(संरचना Curl_easy *दत्त,
                                  संरचना ossl_ctx *octx,
                                  ध्रुव वर्ण *iname);
#यदि_अन्तः

स्थिर पूर्णाङ्क use_certificate_blob(SSL_CTX *ctx, ध्रुव संरचना curl_blob *blob,
                                पूर्णाङ्क type, ध्रुव वर्ण *कुञ्चिका_passwd)
{
  पूर्णाङ्क ret = 0;
  X509 *x = रिक्त;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *अन्तर् = BIO_new_mem_buf(blob->दत्त, (पूर्णाङ्क)blob->len);
  यदि(!अन्तर्)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(अन्तर्, रिक्त);
  }
  अन्यथा यदि(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(अन्तर्, रिक्त, passwd_callback, CURL_UNCONST(कुञ्चिका_passwd));
  }
  अन्यथा {
    ret = 0;
    गच्छ अन्तः;
  }

  यदि(!x) {
    ret = 0;
    गच्छ अन्तः;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
अन्तः:
  X509_free(x);
  BIO_free(अन्तर्);
  प्रत्यागम ret;
}

स्थिर पूर्णाङ्क use_privatekey_blob(SSL_CTX *ctx, ध्रुव संरचना curl_blob *blob,
                               पूर्णाङ्क type, ध्रुव वर्ण *कुञ्चिका_passwd)
{
  पूर्णाङ्क ret = 0;
  EVP_PKEY *pkey = रिक्त;
  BIO *अन्तर् = BIO_new_mem_buf(blob->दत्त, (पूर्णाङ्क)blob->len);
  यदि(!अन्तर्)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_वैयक्तिकम्_कुञ्चिका(अन्तर्, रिक्त, passwd_callback,
                                   CURL_UNCONST(कुञ्चिका_passwd));
  अन्यथा यदि(type == SSL_FILETYPE_ASN1)
    pkey = d2i_वैयक्तिकम्_कुञ्चिका_bio(अन्तर्, रिक्त);
  अन्यथा
    गच्छ अन्तः;

  यदि(!pkey)
    गच्छ अन्तः;

  ret = SSL_CTX_use_वैयक्तिकम्_कुञ्चिका(ctx, pkey);
  EVP_PKEY_free(pkey);
अन्तः:
  BIO_free(अन्तर्);
  प्रत्यागम ret;
}

स्थिर पूर्णाङ्क use_certificate_chain_blob(SSL_CTX *ctx,
                                      ध्रुव संरचना curl_blob *blob,
                                      ध्रुव वर्ण *कुञ्चिका_passwd)
{
  पूर्णाङ्क ret = 0;
  X509 *x = रिक्त;
  BIO *अन्तर् = BIO_new_mem_buf(blob->दत्त, (पूर्णाङ्क)blob->len);
  यदि(!अन्तर्)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(अन्तर्, रिक्त,
                            passwd_callback, CURL_UNCONST(कुञ्चिका_passwd));
  यदि(!x)
    गच्छ अन्तः;

  ret = SSL_CTX_use_certificate(ctx, x);

  यदि(ERR_peek_error() != 0)
    ret = 0;

  यदि(ret) {
    X509 *ca;
    sslerr_t err;

    यदि(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      गच्छ अन्तः;
    }

    यावत्((ca = PEM_read_bio_X509(अन्तर्, रिक्त, passwd_callback,
                                  CURL_UNCONST(कुञ्चिका_passwd))) != रिक्त) {

      यदि(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        गच्छ अन्तः;
      }
    }

    err = ERR_peek_चरमः_error();
    यदि((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_START_पङ्क्तिः))
      ERR_clear_error();
    अन्यथा
      ret = 0;
  }

अन्तः:
  X509_free(x);
  BIO_free(अन्तर्);
  प्रत्यागम ret;
}

स्थिर पूर्णाङ्क enginecheck(संरचना Curl_easy *दत्त,
                       संरचना ossl_ctx *octx,
                       ध्रुव वर्ण *कुञ्चिका_file,
                       ध्रुव वर्ण *कुञ्चिका_passwd)
{
#यदि_परिभाषितम् USE_OPENSSL_ENGINE
  EVP_PKEY *priv_कुञ्चिका = रिक्त;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  यदि(!octx->p.engine && is_pkcs11_uri(कुञ्चिका_file) &&
     ossl_समुच्चयः_engine(दत्त, octx, "pkcs11") != CURLE_OK)
    प्रत्यागम 0;

  यदि(octx->p.engine) {
    UI_विधानम् *ui_विधानम् = UI_create_विधानम्("curl user interface");
    यदि(!ui_विधानम्) {
      failf(दत्त, "unable to create " OSSL_PACKAGE " user-interface method");
      प्रत्यागम 0;
    }
    UI_विधानम्_समुच्चयः_opener(ui_विधानम्, UI_विधानम्_get_opener(UI_OpenSSL()));
    UI_विधानम्_समुच्चयः_closer(ui_विधानम्, UI_विधानम्_get_closer(UI_OpenSSL()));
    UI_विधानम्_समुच्चयः_reader(ui_विधानम्, ssl_ui_reader);
    UI_विधानम्_समुच्चयः_writer(ui_विधानम्, ssl_ui_writer);
    priv_कुञ्चिका = ENGINE_आरोपय_वैयक्तिकम्_कुञ्चिका(octx->p.engine, कुञ्चिका_file,
                                       ui_विधानम्,
                                       CURL_UNCONST(कुञ्चिका_passwd));
    UI_destroy_विधानम्(ui_विधानम्);
    यदि(!priv_कुञ्चिका) {
      failf(दत्त, "failed to load private key from crypto engine");
      प्रत्यागम 0;
    }
    यदि(SSL_CTX_use_वैयक्तिकम्_कुञ्चिका(ctx, priv_कुञ्चिका) != 1) {
      failf(दत्त, "unable to set private key");
      EVP_PKEY_free(priv_कुञ्चिका);
      प्रत्यागम 0;
    }
    EVP_PKEY_free(priv_कुञ्चिका);  /* we do not need the handle any more... */
  }
  अन्यथा {
    failf(दत्त, "crypto engine not set, cannot load private key");
    प्रत्यागम 0;
  }
  प्रत्यागम 1;
#अन्यथा
  (शून्य)octx;
  (शून्य)कुञ्चिका_file;
  (शून्य)कुञ्चिका_passwd;
  failf(दत्त, "SSL_FILETYPE_ENGINE not supported for private key");
  प्रत्यागम 0;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क providercheck(संरचना Curl_easy *दत्त,
                         संरचना ossl_ctx *octx,
                         ध्रुव वर्ण *कुञ्चिका_file)
{
#यदि_परिभाषितम् OPENSSL_HAS_PROVIDERS
  वर्ण error_बफर[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  यदि(!octx->p.provider_loaded && is_pkcs11_uri(कुञ्चिका_file) &&
     ossl_समुच्चयः_provider(दत्त, octx, "pkcs11") != CURLE_OK) {
    प्रत्यागम 0;
  }

  यदि(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_कुञ्चिका = रिक्त;
    OSSL_STORE_CTX *store = रिक्त;
    OSSL_STORE_INFO *info = रिक्त;
    UI_विधानम् *ui_विधानम् = UI_create_विधानम्("curl user interface");
    यदि(!ui_विधानम्) {
      failf(दत्त, "unable to create " OSSL_PACKAGE " user-interface method");
      प्रत्यागम 0;
    }
    UI_विधानम्_समुच्चयः_opener(ui_विधानम्, UI_विधानम्_get_opener(UI_OpenSSL()));
    UI_विधानम्_समुच्चयः_closer(ui_विधानम्, UI_विधानम्_get_closer(UI_OpenSSL()));
    UI_विधानम्_समुच्चयः_reader(ui_विधानम्, ssl_ui_reader);
    UI_विधानम्_समुच्चयः_writer(ui_विधानम्, ssl_ui_writer);

    store = OSSL_STORE_open_ex(कुञ्चिका_file, octx->p.libctx,
                               octx->p.propq, ui_विधानम्, रिक्त, रिक्त,
                               रिक्त, रिक्त);
    यदि(!store) {
      failf(दत्त, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      UI_destroy_विधानम्(ui_विधानम्);
      प्रत्यागम 0;
    }
    यदि(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(दत्त, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
    }

    info = OSSL_STORE_आरोपय(store);
    यदि(info) {
      पूर्णाङ्क ossl_type = OSSL_STORE_INFO_get_type(info);

      यदि(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_कुञ्चिका = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_विधानम्(ui_विधानम्);
    यदि(!priv_कुञ्चिका) {
      failf(दत्त, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      प्रत्यागम 0;
    }

    यदि(SSL_CTX_use_वैयक्तिकम्_कुञ्चिका(ctx, priv_कुञ्चिका) != 1) {
      failf(दत्त, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      EVP_PKEY_free(priv_कुञ्चिका);
      प्रत्यागम 0;
    }
    EVP_PKEY_free(priv_कुञ्चिका); /* we do not need the handle any more... */
  }
  अन्यथा {
    failf(दत्त, "crypto provider not set, cannot load private key");
    प्रत्यागम 0;
  }
  प्रत्यागम 1;
#अन्यथा
  (शून्य)octx;
  (शून्य)कुञ्चिका_file;
  failf(दत्त, "SSL_FILETYPE_PROVIDER not supported for private key");
  प्रत्यागम 0;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क engineload(संरचना Curl_easy *दत्त,
                      संरचना ossl_ctx *octx,
                      ध्रुव वर्ण *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#यदि defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  वर्ण error_बफर[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  यदि(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_समुच्चयः_engine(दत्त, octx, "pkcs11") != CURLE_OK)
    प्रत्यागम 0;

  यदि(octx->p.engine) {
    स्थिर ध्रुव वर्ण cmd_name[] = "LOAD_CERT_CTRL";
    संरचना {
      ध्रुव वर्ण *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = रिक्त;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    यदि(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), रिक्त)) {
      failf(दत्त, "SSL engine does not support loading certificates");
      प्रत्यागम 0;
    }

    /* Load the certificate from the engine */
    यदि(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, रिक्त, 1)) {
      failf(दत्त, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      प्रत्यागम 0;
    }

    यदि(!params.cert) {
      failf(दत्त, "SSL engine did not initialize the certificate properly.");
      प्रत्यागम 0;
    }

    यदि(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(दत्त, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      X509_free(params.cert);
      प्रत्यागम 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  अन्यथा {
    failf(दत्त, "crypto engine not set, cannot load certificate");
    प्रत्यागम 0;
  }
  प्रत्यागम 1;
#अन्यथा
  (शून्य)octx;
  (शून्य)cert_file;
  failf(दत्त, "SSL_FILETYPE_ENGINE not supported for certificate");
  प्रत्यागम 0;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क providerload(संरचना Curl_easy *दत्त,
                        संरचना ossl_ctx *octx,
                        ध्रुव वर्ण *cert_file)
{
#यदि_परिभाषितम् OPENSSL_HAS_PROVIDERS
  वर्ण error_बफर[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  यदि(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_समुच्चयः_provider(दत्त, octx, "pkcs11") != CURLE_OK)
    प्रत्यागम 0;

  यदि(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = रिक्त;
    X509 *cert = रिक्त;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त);
    पूर्णाङ्क rc;

    यदि(!store) {
      failf(दत्त, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      प्रत्यागम 0;
    }
    यदि(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(दत्त, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
    }

    info = OSSL_STORE_आरोपय(store);
    यदि(info) {
      पूर्णाङ्क ossl_type = OSSL_STORE_INFO_get_type(info);

      यदि(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    यदि(!cert) {
      failf(दत्त, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      प्रत्यागम 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    यदि(rc != 1) {
      failf(दत्त, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      प्रत्यागम 0;
    }
  }
  अन्यथा {
    failf(दत्त, "crypto provider not set, cannot load certificate");
    प्रत्यागम 0;
  }
  प्रत्यागम 1;
#अन्यथा
  (शून्य)octx;
  (शून्य)cert_file;
  failf(दत्त, "SSL_FILETYPE_PROVIDER not supported for certificate");
  प्रत्यागम 0;
#यदि_अन्तः
}

स्थिर पूर्णाङ्क pkcs12load(संरचना Curl_easy *दत्त,
                      SSL_CTX* ctx,
                      ध्रुव संरचना curl_blob *cert_blob,
                      ध्रुव वर्ण *cert_file,
                      ध्रुव वर्ण *कुञ्चिका_passwd)
{
  वर्ण error_बफर[256];
  BIO *cert_bio = रिक्त;
  PKCS12 *p12 = रिक्त;
  EVP_PKEY *pri;
  X509 *x509;
  पूर्णाङ्क cert_समाप्तम् = 0;
  राशिः_सम्बन्धि(X509) *ca = रिक्त;
  यदि(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->दत्त, (पूर्णाङ्क)cert_blob->len);
    यदि(!cert_bio) {
      failf(दत्त, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      प्रत्यागम 0;
    }
  }
  अन्यथा {
    cert_bio = BIO_new(BIO_s_file());
    यदि(!cert_bio) {
      failf(दत्त, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
      प्रत्यागम 0;
    }

    यदि(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(दत्त, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      प्रत्यागम 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, रिक्त);
  BIO_free(cert_bio);

  यदि(!p12) {
    failf(दत्त, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    प्रत्यागम 0;
  }

  यदि(!PKCS12_parse(p12, कुञ्चिका_passwd, &pri, &x509, &ca)) {
    failf(दत्त, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_बफर, परिमाण(error_बफर)));
    PKCS12_free(p12);
    प्रत्यागम 0;
  }

  PKCS12_free(p12);

  यदि(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(दत्त, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_बफर, परिमाण(error_बफर)));
    गच्छ fail;
  }

  यदि(SSL_CTX_use_वैयक्तिकम्_कुञ्चिका(ctx, pri) != 1) {
    failf(दत्त, "unable to use private key from PKCS12 file '%s'", cert_file);
    गच्छ fail;
  }

  यदि(!SSL_CTX_check_वैयक्तिकम्_कुञ्चिका(ctx)) {
    failf(दत्त, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    गच्छ fail;
  }
  /* Set Certificate Verification chain */
  यदि(ca) {
    यावत्(sk_X509_अङ्कः(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_निष्कासय(ca);
      यदि(!SSL_CTX_सङ्कलय_client_CA(ctx, x)) {
        X509_free(x);
        failf(दत्त, "cannot add certificate to client CA list");
        गच्छ fail;
      }
      यदि(!SSL_CTX_सङ्कलय_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(दत्त, "cannot add certificate to certificate chain");
        गच्छ fail;
      }
    }
  }

  cert_समाप्तम् = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wcast-function-type-strict"
#यदि_अन्तः
  sk_X509_निष्कासय_free(ca, X509_free);
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
  यदि(!cert_समाप्तम्)
    प्रत्यागम 0; /* failure! */
  प्रत्यागम 1;
}

स्थिर CURLcode client_cert(संरचना Curl_easy *दत्त,
                            संरचना ossl_ctx *octx,
                            वर्ण *cert_file,
                            ध्रुव संरचना curl_blob *cert_blob,
                            ध्रुव वर्ण *cert_type,
                            वर्ण *कुञ्चिका_file,
                            ध्रुव संरचना curl_blob *कुञ्चिका_blob,
                            ध्रुव वर्ण *कुञ्चिका_type,
                            वर्ण *कुञ्चिका_passwd)
{
  वर्ण error_बफर[256];
  bool check_privkey = TRUE;
  पूर्णाङ्क file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  यदि(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_समाप्तम् = FALSE;
    पूर्णाङ्क cert_use_परिणाम;

    यदि(कुञ्चिका_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_समुच्चयः_default_passwd_cb_userdata(ctx, कुञ्चिका_passwd);
      /* Set passwd callback: */
      SSL_CTX_समुच्चयः_default_passwd_cb(ctx, passwd_callback);
    }

    विकल्प(file_type) {
    पक्ष SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_परिणाम = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, कुञ्चिका_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      यदि(cert_use_परिणाम != 1) {
        failf(दत्त,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_बफर,
                            परिमाण(error_बफर)));
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
      भङ्ग;

    पक्ष SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_परिणाम = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, कुञ्चिका_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      यदि(cert_use_परिणाम != 1) {
        failf(दत्त,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_बफर,
                            परिमाण(error_बफर)));
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
      भङ्ग;

    पक्ष SSL_FILETYPE_ENGINE:
      यदि(!cert_file || !engineload(दत्त, octx, cert_file))
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      भङ्ग;

    पक्ष SSL_FILETYPE_PROVIDER:
      यदि(!cert_file || !providerload(दत्त, octx, cert_file))
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      भङ्ग;

    पक्ष SSL_FILETYPE_PKCS12:
      यदि(!pkcs12load(दत्त, ctx, cert_blob, cert_file, कुञ्चिका_passwd))
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      pcks12_समाप्तम् = TRUE;
      भङ्ग;

    स्वभाव:
      failf(दत्त, "not supported file type '%s' for certificate", cert_type);
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }

    यदि(!कुञ्चिका_file && !कुञ्चिका_blob) {
      कुञ्चिका_file = cert_file;
      कुञ्चिका_blob = cert_blob;
    }
    अन्यथा
      file_type = ossl_do_file_type(कुञ्चिका_type);

    विकल्प(file_type) {
    पक्ष SSL_FILETYPE_PEM:
    पक्ष SSL_FILETYPE_ASN1:
      cert_use_परिणाम = कुञ्चिका_blob ?
        use_privatekey_blob(ctx, कुञ्चिका_blob, file_type, कुञ्चिका_passwd) :
      SSL_CTX_use_वैयक्तिकम्_कुञ्चिका_file(ctx, कुञ्चिका_file, file_type);
      यदि(cert_use_परिणाम != 1) {
        failf(दत्त, "unable to set private key file: '%s' type %s",
              कुञ्चिका_file ? कुञ्चिका_file : "(memory blob)",
              कुञ्चिका_type ? कुञ्चिका_type : "PEM");
        प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
      }
      भङ्ग;
    पक्ष SSL_FILETYPE_ENGINE:
      यदि(!enginecheck(दत्त, octx, कुञ्चिका_file, कुञ्चिका_passwd))
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      भङ्ग;

    पक्ष SSL_FILETYPE_PROVIDER:
      यदि(!providercheck(दत्त, octx, कुञ्चिका_file))
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      भङ्ग;

    पक्ष SSL_FILETYPE_PKCS12:
      यदि(!pcks12_समाप्तम्) {
        failf(दत्त, "file type P12 for private key not supported");
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
      भङ्ग;
    स्वभाव:
      failf(दत्त, "not supported file type for private key");
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }

    ssl = SSL_new(ctx);
    यदि(!ssl) {
      failf(दत्त, "unable to create an SSL structure");
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }

    x509 = SSL_get_certificate(ssl);

    यदि(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#यदि !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_कुञ्चिका = SSL_get_privatekey(ssl);
      यदि(EVP_PKEY_id(priv_कुञ्चिका) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_कुञ्चिका);
        यदि(RSA_flags(rsa) & RSA_विधानम्_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#यदि_अन्तः

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    यदि(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      यदि(!SSL_CTX_check_वैयक्तिकम्_कुञ्चिका(ctx)) {
        failf(दत्त, "Private key does not match the certificate public key");
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् CURLVERBOSE
/* returns non-zero on failure */
स्थिर CURLcode x509_name_oneline(ध्रुव X509_NAME *a, संरचना dynbuf *d)
{
  BIO *bio_बहिर्गमनम् = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  पूर्णाङ्क rc;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(bio_बहिर्गमनम्) {
    अचिह्नित दीर्घ flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_बहिर्गमनम्, a, 0, flags);
    यदि(rc != -1) {
      BIO_get_mem_ptr(bio_बहिर्गमनम्, &biomem);
      परिणाम = curlx_dyn_addn(d, biomem->दत्त, biomem->दीर्घता);
    }
    BIO_free(bio_बहिर्गमनम्);
  }
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
स्थिर पूर्णाङ्क ossl_init(शून्य)
{
  ध्रुव uint64_t flags =
#यदि_परिभाषितम् OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#यदि_अन्तः
#यदि_परिभाषितम् CURL_DISABLE_OPENSSL_AUTO_आरोपय_CONFIG
    OPENSSL_INIT_NO_आरोपय_CONFIG |
#अन्यथा
    OPENSSL_INIT_आरोपय_CONFIG |
#यदि_अन्तः
    0;
  OPENSSL_init_ssl(flags, रिक्त);

#यदि_अपरिभाषितम् HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#यदि_अन्तः

  प्रत्यागम 1;
}

/* Global cleanup */
स्थिर शून्य ossl_cleanup(शून्य)
{
#यदि_अपरिभाषितम् HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#यदि_अन्तः
}

/* Selects an OpenSSL crypto engine or provider.
 */
स्थिर CURLcode ossl_समुच्चयः_engine(संरचना Curl_easy *दत्त, संरचना ossl_ctx *octx,
                                ध्रुव वर्ण *name)
{
#यदि_परिभाषितम् USE_OPENSSL_ENGINE
  CURLcode परिणाम = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  यदि(e) {

    यदि(octx->p.engine) {
      ENGINE_finish(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = रिक्त;
    }
    यदि(!ENGINE_init(e)) {
      वर्ण buf[256];

      ENGINE_free(e);
      failf(दत्त, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, परिमाण(buf)));
      परिणाम = CURLE_SSL_ENGINE_INITFAILED;
      e = रिक्त;
    }
    अन्यथा {
      परिणाम = CURLE_OK;
    }
    octx->p.engine = e;
    प्रत्यागम परिणाम;
  }
#यदि_अन्तः
#यदि_परिभाषितम् OPENSSL_HAS_PROVIDERS
  प्रत्यागम ossl_समुच्चयः_provider(दत्त, octx, name);
#अन्यथा
  (शून्य)octx;
  (शून्य)name;
  failf(दत्त, "OpenSSL engine not found");
  प्रत्यागम CURLE_SSL_ENGINE_NOTFOUND;
#यदि_अन्तः
}

/* Sets engine as default for all SSL operations
 */
स्थिर CURLcode ossl_समुच्चयः_engine_default(संरचना Curl_easy *दत्त,
                                        संरचना ossl_ctx *octx)
{
#यदि_परिभाषितम् USE_OPENSSL_ENGINE
  यदि(octx->p.engine) {
    यदि(ENGINE_समुच्चयः_default(octx->p.engine, ENGINE_विधानम्_ALL) > 0) {
      infof(दत्त, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    अन्यथा {
      failf(दत्त, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      प्रत्यागम CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#अन्यथा
  (शून्य)दत्त;
  (शून्य)octx;
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
स्थिर संरचना curl_slist *ossl_engines_list(संरचना Curl_easy *दत्त)
{
  संरचना curl_slist *list = रिक्त;
#यदि_परिभाषितम् USE_OPENSSL_ENGINE
  संरचना curl_slist *beg;
  ENGINE *e;

  प्रति(e = ENGINE_get_first(); e; e = ENGINE_get_अग्र(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    यदि(!beg) {
      curl_slist_free_all(list);
      प्रत्यागम रिक्त;
    }
    list = beg;
  }
#यदि_अन्तः
  (शून्य)दत्त;
  प्रत्यागम list;
}

#यदि_परिभाषितम् OPENSSL_HAS_PROVIDERS

स्थिर शून्य ossl_provider_cleanup(संरचना ossl_ctx *octx)
{
  यदि(!octx)
    प्रत्यागम;
  यदि(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = रिक्त;
  }
  यदि(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = रिक्त;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = रिक्त;
  curlx_safefree(octx->p.propq);
  octx->p.provider_loaded = FALSE;
}

#परिभाषय MAX_PROVIDER_LEN 128 /* reasonable */

/* Selects an OpenSSL crypto provider.
 *
 * A provider might need an associated property, a string passed on to
 * OpenSSL. Specify this as [PROVIDER][:PROPERTY]: separate the name and the
 * property with a colon. No colon means no property is set.
 *
 * An example provider + property looks like "tpm2:?provider=tpm2".
 */
स्थिर CURLcode ossl_समुच्चयः_provider(संरचना Curl_easy *दत्त,
                                  संरचना ossl_ctx *octx,
                                  ध्रुव वर्ण *iname)
{
  वर्ण name[MAX_PROVIDER_LEN + 1];
  संरचना Curl_str prov;
  ध्रुव वर्ण *propq = रिक्त;

  यदि(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    प्रत्यागम CURLE_OK;
  }
  यदि(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  यदि(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  स्मृतिं_प्रतिलिख(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  यदि(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    यदि(!libctx)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    यदि(propq) {
      octx->p.propq = curlx_strdup(propq);
      यदि(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      }
    }
    octx->p.libctx = libctx;
  }

#यदि_अपरिभाषितम् CURL_DISABLE_OPENSSL_AUTO_आरोपय_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  यदि(!OSSL_LIB_CTX_आरोपय_config(octx->p.libctx, रिक्त)) {
    infof(दत्त, "Failed to load default openssl config. Proceeding.");
  }
#यदि_अन्तः

  यदि(OSSL_PROVIDER_उपलब्धम्(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    प्रत्यागम CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_आरोपय(octx->p.libctx, name, 1);
  यदि(!octx->p.provider) {
    वर्ण error_बफर[256];
    failf(दत्त, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_बफर, परिमाण(error_बफर)));
    ossl_provider_cleanup(octx);
    प्रत्यागम CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_आरोपय(octx->p.libctx, "base", 1);
  यदि(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(दत्त, "Failed to load base");
    प्रत्यागम CURLE_SSL_ENGINE_NOTFOUND;
  }
  अन्यथा
    octx->p.provider_loaded = TRUE;
  प्रत्यागम CURLE_OK;
}
#अन्यथा /* OPENSSL_HAS_PROVIDERS */
#परिभाषय ossl_provider_cleanup(x)
#यदि_अन्तः

स्थिर CURLcode ossl_shutdown(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              bool send_shutdown, bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  वर्ण buf[1024];
  पूर्णाङ्क nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  यदि(!octx->ssl || cf->shutdown) {
    *समाप्तम् = TRUE;
    गच्छ बहिर्गमनम्;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *समाप्तम् = FALSE;
  यदि(!(SSL_get_shutdown(octx->ssl) & SSL_प्रेषितम्_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    प्रति(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (पूर्णाङ्क)परिमाण(buf));
      CURL_TRC_CF(दत्त, cf, "SSL shutdown not sent, read -> %d", nread);
      यदि(nread <= 0)
        भङ्ग;
    }
    err = SSL_get_error(octx->ssl, nread);
    यदि(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool निवेशः_pending;
      /* Yes, it did. */
      यदि(!send_shutdown) {
        CURL_TRC_CF(दत्त, cf, "SSL shutdown received, not sending");
        *समाप्तम् = TRUE;
        गच्छ बहिर्गमनम्;
      }
      अन्यथा यदि(!cf->अग्र->cft->is_alive(cf->अग्र, दत्त, &निवेशः_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(दत्त, cf, "peer closed connection");
        *समाप्तम् = TRUE;
        गच्छ बहिर्गमनम्;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  यदि(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_प्रेषितम्_SHUTDOWN)) {
    पूर्णाङ्क rc;
    ERR_clear_error();
    CURL_TRC_CF(दत्त, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    यदि(rc == 1) {
      CURL_TRC_CF(दत्त, cf, "SSL shutdown finished");
      *समाप्तम् = TRUE;
      गच्छ बहिर्गमनम्;
    }
    यदि(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(दत्त, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      गच्छ बहिर्गमनम्;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  प्रति(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (पूर्णाङ्क)परिमाण(buf));
    CURL_TRC_CF(दत्त, cf, "SSL shutdown read -> %d", nread);
    यदि(nread <= 0)
      भङ्ग;
  }
  err = SSL_get_error(octx->ssl, nread);
  विकल्प(err) {
  पक्ष SSL_ERROR_ZERO_RETURN: /* no more data */
    यदि(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(दत्त, cf, "SSL shutdown finished");
    अन्यथा
      CURL_TRC_CF(दत्त, cf, "SSL shutdown not received, but closed");
    *समाप्तम् = TRUE;
    भङ्ग;
  पक्ष SSL_ERROR_NONE: /* did not get anything */
  पक्ष SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(दत्त, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    भङ्ग;
  पक्ष SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(दत्त, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    भङ्ग;
  स्वभाव:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(अचिह्नित दीर्घ sslerr = ERR_get_error());
      CURL_TRC_CF(दत्त, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, परिमाण(buf)) :
                   SSL_ERROR_गन्तव्यम्_str(err)),
                  SOCKERRNO);
    }
    *समाप्तम् = TRUE;
    परिणाम = CURLE_OK;
    भङ्ग;
  }

बहिर्गमनम्:
  cf->shutdown = (परिणाम || *समाप्तम्);
  यदि(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->निवेशः_pending = FALSE;
  प्रत्यागम परिणाम;
}

/* also used by quic-tls */
शून्य Curl_osslctx_cleanup(संरचना ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  यदि(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = रिक्त;
  }
  यदि(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = रिक्त;
    octx->x509_store_सज्जता = FALSE;
  }
  यदि(octx->bio_विधानम्) {
    ossl_bio_cf_विधानम्_free(octx->bio_विधानम्);
    octx->bio_विधानम् = रिक्त;
  }
#यदि_परिभाषितम् USE_OPENSSL_ENGINE
  यदि(octx->p.engine) {
    ENGINE_finish(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = रिक्त;
  }
#यदि_अन्तः
  ossl_provider_cleanup(octx);
}

स्थिर शून्य ossl_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  (शून्य)दत्त;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
स्थिर शून्य ossl_close_all(संरचना Curl_easy *दत्त)
{
  (शून्य)दत्त;
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
स्थिर CURLcode ossl_verifyhost(संरचना Curl_easy *दत्त,
                                संरचना connectdata *conn,
                                संरचना ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  पूर्णाङ्क target; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  राशिः_सम्बन्धि(GENERAL_NAME) *altnames;
#यदि_परिभाषितम् USE_IPV6
  संरचना in6_addr addr;
#अन्यथा
  संरचना in_addr addr;
#यदि_अन्तः
  CURLcode परिणाम = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = वाक्यदैर्घ्यम्(peer->origin->hostname);

  (शून्य)conn;
  विकल्प(peer->type) {
  पक्ष CURL_SSL_PEER_IPV4:
    यदि(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = परिमाण(संरचना in_addr);
    भङ्ग;
#यदि_परिभाषितम् USE_IPV6
  पक्ष CURL_SSL_PEER_IPV6:
    यदि(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
    target = GEN_IPADD;
    addrlen = परिमाण(संरचना in6_addr);
    भङ्ग;
#यदि_अन्तः
  पक्ष CURL_SSL_PEER_DNS:
    target = GEN_DNS;
    भङ्ग;
  स्वभाव:
    DEBUGASSERT(0);
    failf(दत्त, "unexpected SSL peer type: %d", (पूर्णाङ्क)peer->type);
    प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, रिक्त, रिक्त);

  यदि(altnames) {
#यदि_परिभाषितम् HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#अन्यथा
    पूर्णाङ्क numalts;
    पूर्णाङ्क i;
#यदि_अन्तः

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_अङ्कः(altnames);

    /* loop through all alternatives - until a dnsmatch */
    प्रति(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      ध्रुव GENERAL_NAME *check = sk_GENERAL_NAME_मानम्(altnames, i);

      यदि(check->type == GEN_DNS)
        dNSName = TRUE;
      अन्यथा यदि(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      यदि(check->type == target) {
        /* get data and length */
        ध्रुव वर्ण *altptr = (ध्रुव वर्ण *)ASN1_STRING_get0_दत्त(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_दीर्घता(check->d.ia5);

        विकल्प(target) {
        पक्ष GEN_DNS: /* name/pattern comparison */
          यदि(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(दत्त, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (पूर्णाङ्क)altlen, altptr);
          }
          भङ्ग;

        पक्ष GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          यदि((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(दत्त, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          भङ्ग;
        }
      }
    }
    GENERAL_नामानि_free(altnames);
  }

  यदि(matched)
    /* an alternative name matched */
    ;
  अन्यथा यदि(dNSName || iPAddress) {
    ध्रुव वर्ण *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(दत्त, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(दत्त, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
  }
  अन्यथा {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    पूर्णाङ्क i = -1;
    अचिह्नित वर्ण *cn = रिक्त;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    ध्रुव X509_NAME *name = X509_get_subject_name(server_cert);
    यदि(name) {
      पूर्णाङ्क j;
      यावत्((j = X509_NAME_get_सूचकांक_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    यदि(i >= 0) {
      ध्रुव ASN1_STRING *tmp =
        X509_NAME_प्रविष्टिः_get_दत्त(X509_NAME_get_प्रविष्टिः(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      यदि(tmp) {
        यदि(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_दीर्घता(tmp);
          cn = (अचिह्नित वर्ण *)CURL_UNCONST(ASN1_STRING_get0_दत्त(tmp));
        }
        अन्यथा { /* not a UTF8 name */
          ध्रुव पूर्णाङ्क utf8len = ASN1_STRING_गन्तव्यम्_UTF8(&cn, tmp);
          यदि(utf8len < 0)
            cnlen = 0;
          अन्यथा
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        यदि((cnlen == 0) || !cn)
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        अन्यथा यदि(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(दत्त, "SSL: illegal cert name field");
          परिणाम = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    यदि(परिणाम)
      /* error already detected, pass through */
      ;
    अन्यथा यदि(!cn) {
      failf(दत्त, "SSL: unable to obtain common name from peer certificate");
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
    अन्यथा यदि(!Curl_cert_hostcheck((ध्रुव वर्ण *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(दत्त, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (पूर्णाङ्क)cnlen, cn,
            peer->origin->user_hostname);
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
    अन्यथा {
      infof(दत्त, " common name: %.*s (matched)", (पूर्णाङ्क)cnlen, cn);
    }
    यदि(free_cn)
      OPENSSL_free(cn);
  }

  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् OPENSSL_NO_OCSP
स्थिर CURLcode verifystatus(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             संरचना ossl_ctx *octx)
{
  पूर्णाङ्क i, ocsp_स्थितिः;
#यदि_परिभाषितम् HAVE_BORINGSSL_LIKE
  ध्रुव uint8_t *स्थितिः;
#अन्यथा
  अचिह्नित वर्ण *स्थितिः;
#यदि_अन्तः
  ध्रुव अचिह्नित वर्ण *p;
  CURLcode परिणाम = CURLE_OK;
  OCSP_RESPONSE *rsp = रिक्त;
  OCSP_BASICRESP *br = रिक्त;
  X509_STORE     *st = रिक्त;
  राशिः_सम्बन्धि(X509) *ch = रिक्त;
  X509 *cert;
  OCSP_CERTID *id = रिक्त;
  पूर्णाङ्क cert_स्थितिः, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  पूर्णाङ्क ret;
  दीर्घ len;

  (शून्य)cf;
  DEBUGASSERT(octx);

  len = (दीर्घ)SSL_get_tlsext_स्थितिः_ocsp_resp(octx->ssl, &स्थितिः);

  यदि(!स्थितिः) {
    failf(दत्त, "No OCSP response received");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }
  p = स्थितिः;
  rsp = d2i_OCSP_RESPONSE(रिक्त, &p, len);
  यदि(!rsp) {
    failf(दत्त, "Invalid OCSP response");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }

  ocsp_स्थितिः = OCSP_response_स्थितिः(rsp);
  यदि(ocsp_स्थितिः != OCSP_RESPONSE_स्थितिः_SUCCESSFUL) {
    failf(दत्त, "Invalid OCSP response status: %s (%d)",
          OCSP_response_स्थितिः_str(ocsp_स्थितिः), ocsp_स्थितिः);
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }

  br = OCSP_response_get1_basic(rsp);
  यदि(!br) {
    failf(दत्त, "Invalid OCSP response");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }

#यदि !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_संस्करणम्_सङ्ख्या) && LIBRESSL_संस्करणम्_सङ्ख्या < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#अन्यथा
  ch = SSL_get_peer_cert_chain(octx->ssl);
#यदि_अन्तः
  यदि(!ch) {
    failf(दत्त, "Could not get peer certificate chain");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  यदि(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(दत्त, "OCSP response verification failed");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  यदि(!cert) {
    failf(दत्त, "Error getting peer certificate");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }

  प्रति(i = 0; i < (पूर्णाङ्क)sk_X509_अङ्कः(ch); i++) {
    X509 *issuer = sk_X509_मानम्(ch, (ossl_valsize_t)i);
    यदि(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_गन्तव्यम्_id(EVP_sha1(), cert, issuer);
      भङ्ग;
    }
  }
  X509_free(cert);

  यदि(!id) {
    failf(दत्त, "Error computing OCSP ID");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_find_स्थितिः(br, id, &cert_स्थितिः, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  यदि(ret != 1) {
    failf(दत्त, "Could not find certificate ID in OCSP response");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  यदि(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(दत्त, "OCSP response has expired");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }

  infof(दत्त, "SSL certificate status: %s (%d)",
        OCSP_cert_स्थितिः_str(cert_स्थितिः), cert_स्थितिः);

  विकल्प(cert_स्थितिः) {
  पक्ष V_OCSP_CERTSTATUS_GOOD:
    भङ्ग;

  पक्ष V_OCSP_CERTSTATUS_REVOKED:
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    failf(दत्त, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    गच्छ अन्तः;

  पक्ष V_OCSP_CERTSTATUS_UNKNOWN:
  स्वभाव:
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ अन्तः;
  }

अन्तः:
  यदि(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  प्रत्यागम परिणाम;
}
#यदि_अन्तः

स्थिर ध्रुव वर्ण *ssl_सन्देशः_type(पूर्णाङ्क ssl_ver, पूर्णाङ्क सन्देशः)
{
  यदि(ssl_ver == SSL3_संस्करणम्_MAJOR) {
    विकल्प(सन्देशः) {
    पक्ष SSL3_MT_HELLO_REQUEST:
      प्रत्यागम "Hello request";
    पक्ष SSL3_MT_CLIENT_HELLO:
      प्रत्यागम "Client hello";
    पक्ष SSL3_MT_SERVER_HELLO:
      प्रत्यागम "Server hello";
#यदि_परिभाषितम् SSL3_MT_NEWSESSION_TICKET
    पक्ष SSL3_MT_NEWSESSION_TICKET:
      प्रत्यागम "Newsession Ticket";
#यदि_अन्तः
    पक्ष SSL3_MT_CERTIFICATE:
      प्रत्यागम "Certificate";
    पक्ष SSL3_MT_SERVER_कुञ्चिका_EXCHANGE:
      प्रत्यागम "Server key exchange";
    पक्ष SSL3_MT_CLIENT_कुञ्चिका_EXCHANGE:
      प्रत्यागम "Client key exchange";
    पक्ष SSL3_MT_CERTIFICATE_REQUEST:
      प्रत्यागम "Request CERT";
    पक्ष SSL3_MT_SERVER_समाप्तम्:
      प्रत्यागम "Server finished";
    पक्ष SSL3_MT_CERTIFICATE_VERIFY:
      प्रत्यागम "CERT verify";
    पक्ष SSL3_MT_परिसमाप्तम्:
      प्रत्यागम "Finished";
#यदि_परिभाषितम् SSL3_MT_CERTIFICATE_स्थितिः
    पक्ष SSL3_MT_CERTIFICATE_स्थितिः:
      प्रत्यागम "Certificate Status";
#यदि_अन्तः
#यदि_परिभाषितम् SSL3_MT_ENCRYPTED_EXTENSIONS
    पक्ष SSL3_MT_ENCRYPTED_EXTENSIONS:
      प्रत्यागम "Encrypted Extensions";
#यदि_अन्तः
#यदि_परिभाषितम् SSL3_MT_SUPPLEMENTAL_दत्त
    पक्ष SSL3_MT_SUPPLEMENTAL_दत्त:
      प्रत्यागम "Supplemental data";
#यदि_अन्तः
#यदि_परिभाषितम् SSL3_MT_अन्तः_सम्बन्धि_EARLY_दत्त
    पक्ष SSL3_MT_अन्तः_सम्बन्धि_EARLY_दत्त:
      प्रत्यागम "End of early data";
#यदि_अन्तः
#यदि_परिभाषितम् SSL3_MT_कुञ्चिका_UPDATE
    पक्ष SSL3_MT_कुञ्चिका_UPDATE:
      प्रत्यागम "Key update";
#यदि_अन्तः
#यदि_परिभाषितम् SSL3_MT_अग्र_PROTO
    पक्ष SSL3_MT_अग्र_PROTO:
      प्रत्यागम "Next protocol";
#यदि_अन्तः
#यदि_परिभाषितम् SSL3_MT_सन्देशः_HASH
    पक्ष SSL3_MT_सन्देशः_HASH:
      प्रत्यागम "Message hash";
#यदि_अन्तः
    }
  }
  प्रत्यागम "Unknown";
}

स्थिर ध्रुव वर्ण *tls_rt_type(पूर्णाङ्क type)
{
  विकल्प(type) {
#यदि_परिभाषितम् SSL3_RT_शीर्षकम्
  पक्ष SSL3_RT_शीर्षकम्:
    प्रत्यागम "TLS header";
#यदि_अन्तः
  पक्ष SSL3_RT_CHANGE_CIPHER_SPEC:
    प्रत्यागम "TLS change cipher";
  पक्ष SSL3_RT_ALERT:
    प्रत्यागम "TLS alert";
  पक्ष SSL3_RT_HANDSHAKE:
    प्रत्यागम "TLS handshake";
  पक्ष SSL3_RT_APPLICATION_दत्त:
    प्रत्यागम "TLS app data";
  स्वभाव:
    प्रत्यागम "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
स्थिर शून्य ossl_trace(पूर्णाङ्क direction, पूर्णाङ्क ssl_ver, पूर्णाङ्क content_type,
                       ध्रुव शून्य *buf, size_t len, SSL *ssl,
                       शून्य *userp)
{
  ध्रुव वर्ण *verstr;
  संरचना Curl_cfilter *cf = userp;
  संरचना Curl_easy *दत्त = रिक्त;
  वर्ण unknown[32];

  यदि(!cf)
    प्रत्यागम;
  दत्त = CF_दत्त_CURRENT(cf);
  यदि(!दत्त || !दत्त->समुच्चयः.fdebug || (direction && direction != 1))
    प्रत्यागम;

  विकल्प(ssl_ver) {
#यदि_परिभाषितम् SSL3_संस्करणम्
  पक्ष SSL3_संस्करणम्:
    verstr = "SSLv3";
    भङ्ग;
#यदि_अन्तः
  पक्ष TLS1_संस्करणम्:
    verstr = "TLSv1.0";
    भङ्ग;
#यदि_परिभाषितम् TLS1_1_संस्करणम्
  पक्ष TLS1_1_संस्करणम्:
    verstr = "TLSv1.1";
    भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् TLS1_2_संस्करणम्
  पक्ष TLS1_2_संस्करणम्:
    verstr = "TLSv1.2";
    भङ्ग;
#यदि_अन्तः
  पक्ष TLS1_3_संस्करणम्:
    verstr = "TLSv1.3";
    भङ्ग;
  स्वभाव:
    curl_msnprintf(unknown, परिमाण(unknown), "(%x)", (अचिह्नित पूर्णाङ्क)ssl_ver);
    verstr = unknown;
    भङ्ग;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  यदि(ssl_ver
#यदि_परिभाषितम् SSL3_RT_शीर्षकम्
     && content_type != SSL3_RT_शीर्षकम्
#यदि_अन्तः
#यदि_परिभाषितम् SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#यदि_अन्तः
    ) {
    ध्रुव वर्ण *सन्देशः_name = "Truncated message";
    ध्रुव वर्ण *tls_rt_name;
    वर्ण ssl_buf[1024];
    पूर्णाङ्क सन्देशः_type = 0;
    पूर्णाङ्क txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    यदि(ssl_ver == SSL3_संस्करणम्_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    अन्यथा
      tls_rt_name = "";

    यदि(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      यदि(len) {
        सन्देशः_type = *(ध्रुव अचिह्नित वर्ण *)buf;
        सन्देशः_name = "Change cipher spec";
      }
    }
    अन्यथा यदि(content_type == SSL3_RT_ALERT) {
      यदि(len >= 2) {
        सन्देशः_type =
          (((ध्रुव अचिह्नित वर्ण *)buf)[0] << 8) +
           ((ध्रुव अचिह्नित वर्ण *)buf)[1];
        सन्देशः_name = SSL_alert_desc_string_long(सन्देशः_type);
      }
    }
    अन्यथा यदि(len) {
      सन्देशः_type = *(ध्रुव अचिह्नित वर्ण *)buf;
      सन्देशः_name = ssl_सन्देशः_type(ssl_ver, सन्देशः_type);
    }

    txt_len = curl_msnprintf(ssl_buf, परिमाण(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, सन्देशः_name, सन्देशः_type);
    Curl_debug(दत्त, CURLINFO_TEXT, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(दत्त, (direction == 1) ? CURLINFO_SSL_दत्त_बहिर्गमनम् :
             CURLINFO_SSL_दत्त_अन्तर्, (ध्रुव वर्ण *)buf, len);
  (शून्य)ssl;
}

स्थिर CURLcode ossl_समुच्चयः_ssl_संस्करणम्_min_max(संरचना Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             अचिह्नित पूर्णाङ्क ssl_संस्करणम्_min)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  दीर्घ curl_ssl_संस्करणम्_min = (दीर्घ)ssl_संस्करणम्_min;
  दीर्घ curl_ssl_संस्करणम्_max;

  /* convert curl min SSL version option to OpenSSL constant */
#यदि defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_संस्करणम्_सङ्ख्या)
  uint16_t ossl_ssl_संस्करणम्_min = 0;
  uint16_t ossl_ssl_संस्करणम्_max = 0;
#अन्यथा
  दीर्घ ossl_ssl_संस्करणम्_min = 0;
  दीर्घ ossl_ssl_संस्करणम्_max = 0;
#यदि_अन्तः
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_संस्करणम्_min != CURL_SSLVERSION_DEFAULT);
  विकल्प(curl_ssl_संस्करणम्_min) {
  पक्ष CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  पक्ष CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_संस्करणम्_min = TLS1_संस्करणम्;
    भङ्ग;
  पक्ष CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_संस्करणम्_min = TLS1_1_संस्करणम्;
    भङ्ग;
  पक्ष CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_संस्करणम्_min = TLS1_2_संस्करणम्;
    भङ्ग;
  पक्ष CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_संस्करणम्_min = TLS1_3_संस्करणम्;
    भङ्ग;
  }

  /* ... then, TLS max version */
  curl_ssl_संस्करणम्_max = (दीर्घ)conn_config->संस्करणम्_max;

  /* convert curl max SSL version option to OpenSSL constant */
  विकल्प(curl_ssl_संस्करणम्_max) {
  पक्ष CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_संस्करणम्_max = TLS1_संस्करणम्;
    भङ्ग;
  पक्ष CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_संस्करणम्_max = TLS1_1_संस्करणम्;
    भङ्ग;
  पक्ष CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_संस्करणम्_max = TLS1_2_संस्करणम्;
    भङ्ग;
  पक्ष CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_संस्करणम्_max = TLS1_3_संस्करणम्;
    भङ्ग;
  पक्ष CURL_SSLVERSION_MAX_NONE:  /* none selected */
  पक्ष CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  स्वभाव:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_संस्करणम्_max = 0;
    भङ्ग;
  }

  यदि(!SSL_CTX_समुच्चयः_min_proto_संस्करणम्(ctx, ossl_ssl_संस्करणम्_min) ||
     !SSL_CTX_समुच्चयः_max_proto_संस्करणम्(ctx, ossl_ssl_संस्करणम्_max))
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;

  प्रत्यागम CURLE_OK;
}

CURLcode Curl_ossl_सङ्कलय_session(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना ossl_ctx *octx,
                               ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                               SSL_SESSION *session,
                               ध्रुव वर्ण *alpn,
                               अचिह्नित वर्ण *quic_tp,
                               size_t quic_tp_len,
                               संरचना Curl_ssl_session **psession)
{
  संरचना Curl_ssl_session *sc_session = रिक्त, *sc_dup = रिक्त;
  अचिह्नित वर्ण *der_session_buf = रिक्त;
  अचिह्नित वर्ण *qtp_clone = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  यदि(psession)
    *psession = रिक्त;
  यदि(!cf || !दत्त)
    गच्छ बहिर्गमनम्;

  यदि(Curl_ssl_scache_use(cf, दत्त)) {
    size_t der_session_आकार;
    अचिह्नित वर्ण *der_session_ptr;
    size_t earlydata_max = 0;
    पूर्णाङ्क ietf_tls_id = SSL_संस्करणम्(octx->ssl);

    der_session_आकार = i2d_SSL_SESSION(session, रिक्त);
    यदि(der_session_आकार == 0) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_आकार);
    यदि(!der_session_buf) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }

    der_session_आकार = i2d_SSL_SESSION(session, &der_session_ptr);
    यदि(der_session_आकार == 0) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }

#यदि_परिभाषितम् HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_दत्त(session);
#यदि_अन्तः
    यदि(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((ध्रुव वर्ण *)quic_tp, quic_tp_len);
      यदि(!qtp_clone) {
        परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
        गच्छ बहिर्गमनम्;
      }
    }

    परिणाम = Curl_ssl_session_create2(der_session_buf, der_session_आकार,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)time(रिक्त) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = रिक्त;  /* took ownership of sdata */
#यदि_परिभाषितम् USE_APPLE_SECTRUST
    यदि(!परिणाम)
      sc_session->sectrust_verified = octx->sectrust_verified;
#यदि_अन्तः
    यदि(!परिणाम && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        परिणाम = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    यदि(!परिणाम) {
      परिणाम = Curl_ssl_scache_put(cf, दत्त, ssl_peer_कुञ्चिका, sc_session);
      /* took ownership of `sc_session` */
      sc_session = रिक्त;
    }
  }

बहिर्गमनम्:
  curlx_free(der_session_buf);
  यदि(!परिणाम && psession) {
    *psession = sc_dup;
    sc_dup = रिक्त;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  प्रत्यागम परिणाम;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
स्थिर पूर्णाङ्क ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  संरचना Curl_cfilter *cf = (संरचना Curl_cfilter *)SSL_get_app_दत्त(ssl);
  यदि(cf) {
    संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
    संरचना ssl_connect_दत्त *connssl = cf->ctx;
    संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
    संरचना Curl_ssl_session *session = रिक्त;
    Curl_ossl_सङ्कलय_session(cf, दत्त, octx, connssl->peer.scache_कुञ्चिका,
                          ssl_sessionid, connssl->negotiated.alpn, रिक्त,
                          0, &session);
    यदि(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  प्रत्यागम 0;
}

स्थिर CURLcode आरोपय_cacert_from_स्मृतिः(X509_STORE *store,
                                        ध्रुव संरचना curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = रिक्त;
  राशिः_सम्बन्धि(X509_INFO) *inf = रिक्त;

  /* everything else is a reference */
  पूर्णाङ्क i, गणनम् = 0;
  X509_INFO *itmp = रिक्त;

  यदि(ca_info_blob->len > (size_t)अधिकतम_पूर्णाङ्कः)
    प्रत्यागम CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->दत्त, (पूर्णाङ्क)ca_info_blob->len);
  यदि(!cbio)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  inf = PEM_X509_INFO_read_bio(cbio, रिक्त, रिक्त, रिक्त);
  यदि(!inf) {
    BIO_free(cbio);
    प्रत्यागम CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  प्रति(i = 0; i < (पूर्णाङ्क)sk_X509_INFO_अङ्कः(inf); ++i) {
    itmp = sk_X509_INFO_मानम्(inf, (ossl_valsize_t)i);
    यदि(itmp->x509) {
      यदि(X509_STORE_सङ्कलय_cert(store, itmp->x509)) {
        ++गणनम्;
      }
      अन्यथा {
        /* set count to 0 to return an error */
        गणनम् = 0;
        भङ्ग;
      }
    }
    यदि(itmp->crl) {
      यदि(X509_STORE_सङ्कलय_crl(store, itmp->crl)) {
        ++गणनम्;
      }
      अन्यथा {
        /* set count to 0 to return an error */
        गणनम् = 0;
        भङ्ग;
      }
    }
  }

#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic प्रक्षिप
#निर्देशः clang diagnostic ignored "-Wcast-function-type-strict"
#यदि_अन्तः
  sk_X509_INFO_निष्कासय_free(inf, X509_INFO_free);
#यदि defined(__clang__) && __clang_major__ >= 16
#निर्देशः clang diagnostic निष्कासय
#यदि_अन्तः
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  प्रत्यागम (गणनम् > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#यदि_परिभाषितम् USE_WIN32_CRYPTO
स्थिर CURLcode ossl_win_आरोपय_store(संरचना Curl_easy *दत्त,
                                    संरचना Curl_cfilter *cf,
                                    ध्रुव वर्ण *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode परिणाम = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  यदि(hStore) {
    PCCERT_CONTEXT pContext = रिक्त;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = रिक्त;
    DWORD enhkey_usage_आकार = 0;
    VERBOSE(size_t योग = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    परिणाम = CURLE_OK;
    प्रति(;;) {
      X509 *x509;
      FILETIME now;
      BYTE कुञ्चिका_usage[2];
      DWORD req_आकार;
      ध्रुव अचिह्नित वर्ण *encoded_cert;
      pContext = Cert_Enum_Certificates_अन्तर्_Store(hStore, pContext);
      यदि(!pContext)
        भङ्ग;

      VERBOSE(++योग);

#यदि defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        वर्ण cert_name[256];
        यदि(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               रिक्त, cert_name, परिमाण(cert_name)))
          infof(दत्त, "SSL: unknown cert name");
        अन्यथा
          infof(दत्त, "SSL: Checking cert \"%s\"", cert_name);
      }
#यदि_अन्तः
      encoded_cert = (ध्रुव अचिह्नित वर्ण *)pContext->pbCertEncoded;
      यदि(!encoded_cert)
        अनुवर्त;

      GetSystemTimeAsFileTime(&now);
      यदि(CompareFileTime(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         CompareFileTime(&now, &pContext->pCertInfo->NotAfter) > 0)
        अनुवर्त;

      /* If key usage exists check for signing attribute */
      यदि(Cert_Get_Intended_कुञ्चिका_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 कुञ्चिका_usage, परिमाण(कुञ्चिका_usage))) {
        यदि(!(कुञ्चिका_usage[0] & CERT_कुञ्चिका_CERT_SIGN_कुञ्चिका_USAGE))
          अनुवर्त;
      }
      अन्यथा यदि(Get_चरमः_Error())
        अनुवर्त;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      यदि(Cert_Get_Enhanced_कुञ्चिका_Usage(pContext, 0, रिक्त, &req_आकार) && req_आकार) {
        यदि(req_आकार > enhkey_usage_आकार) {
          शून्य *tmp = curlx_realloc(enhkey_usage, req_आकार);

          यदि(!tmp) {
            failf(दत्त, "SSL: Out of memory allocating for OID list");
            परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
            भङ्ग;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_आकार = req_आकार;
        }

        यदि(Cert_Get_Enhanced_कुञ्चिका_Usage(pContext, 0, enhkey_usage, &req_आकार)) {
          यदि(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            यदि((HRESULT)Get_चरमः_Error() != CRYPT_E_NOT_प्राप्तम्)
              अनुवर्त;
          }
          अन्यथा {
            DWORD i;
            bool प्राप्तम् = FALSE;

            प्रति(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              यदि(!वाक्यं_तुलय("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                प्राप्तम् = TRUE;
                भङ्ग;
              }
            }

            यदि(!प्राप्तम्)
              अनुवर्त;
          }
        }
        अन्यथा
          अनुवर्त;
      }
      अन्यथा
        अनुवर्त;

      x509 = d2i_X509(रिक्त, &encoded_cert, (दीर्घ)pContext->cbCertEncoded);
      यदि(!x509)
        अनुवर्त;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      यदि(X509_STORE_सङ्कलय_cert(store, x509) == 1) {
        VERBOSE(++imported);
#यदि_परिभाषितम् DEBUGBUILD
        infof(दत्त, "SSL: Imported cert");
#यदि_अन्तः
        *padded = TRUE;
      }
      X509_free(x509);
    }

    curlx_free(enhkey_usage);
    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    CURL_TRC_CF(दत्त, cf,
                "ossl_win_load_store() found: %zu imported: %zu certs in %s.",
                योग, imported, win_store);

    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ossl_windows_आरोपय_anchors(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *दत्त,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  स्थिर ध्रुव वर्ण * ध्रुव win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode परिणाम = CURLE_OK;

  *padded = FALSE;
  प्रति(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    परिणाम = ossl_win_आरोपय_store(दत्त, cf, win_stores[i], store, &store_added);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    यदि(store_added) {
      CURL_TRC_CF(दत्त, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    अन्यथा
      infof(दत्त, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* USE_WIN32_CRYPTO */

स्थिर CURLcode ossl_आरोपय_trust_anchors(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        संरचना ossl_ctx *octx,
                                        X509_STORE *store)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण * ध्रुव ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? रिक्त : conn_config->CAfile);
  ध्रुव वर्ण * ध्रुव ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_रिक्तानि = TRUE;
  यदि(conn_config->native_ca_store) {
#यदि_परिभाषितम् USE_WIN32_CRYPTO
    bool added = FALSE;
    परिणाम = ossl_windows_आरोपय_anchors(cf, दत्त, store, &added);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    यदि(added) {
      infof(दत्त, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_रिक्तानि = FALSE;
    }
#नोचेद्यदि defined(USE_APPLE_SECTRUST)
    infof(दत्त, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#यदि_अन्तः
  }

  यदि(conn_config->ca_info_blob) {
    परिणाम = आरोपय_cacert_from_स्मृतिः(store, conn_config->ca_info_blob);
    यदि(परिणाम) {
      failf(दत्त, "error adding trust anchors from certificate blob: %d",
            (पूर्णाङ्क)परिणाम);
      प्रत्यागम परिणाम;
    }
    infof(दत्त, "  CA Blob from configuration");
    octx->store_is_रिक्तानि = FALSE;
  }

  यदि(ssl_cafile || ssl_capath) {
#यदि_परिभाषितम् HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    यदि(ssl_cafile) {
      यदि(!X509_STORE_आरोपय_file(store, ssl_cafile)) {
        यदि(octx->store_is_रिक्तानि && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(दत्त, "error adding trust anchors from file: %s", ssl_cafile);
          प्रत्यागम CURLE_SSL_CACERT_BADFILE;
        }
        अन्यथा
          infof(दत्त, "error setting certificate file, continuing anyway");
      }
      infof(दत्त, "  CAfile: %s", ssl_cafile);
      octx->store_is_रिक्तानि = FALSE;
    }
    यदि(ssl_capath) {
      यदि(!X509_STORE_आरोपय_मार्गः(store, ssl_capath)) {
        यदि(octx->store_is_रिक्तानि && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(दत्त, "error adding trust anchors from path: %s", ssl_capath);
          प्रत्यागम CURLE_SSL_CACERT_BADFILE;
        }
        अन्यथा
          infof(दत्त, "error setting certificate path, continuing anyway");
      }
      infof(दत्त, "  CApath: %s", ssl_capath);
      octx->store_is_रिक्तानि = FALSE;
    }
#अन्यथा
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    यदि(!X509_STORE_आरोपय_locations(store, ssl_cafile, ssl_capath)) {
      यदि(octx->store_is_रिक्तानि && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(दत्त, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        प्रत्यागम CURLE_SSL_CACERT_BADFILE;
      }
      अन्यथा {
        infof(दत्त, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    यदि(ssl_cafile)
      infof(दत्त, "  CAfile: %s", ssl_cafile);
    यदि(ssl_capath)
      infof(दत्त, "  CApath: %s", ssl_capath);
    octx->store_is_रिक्तानि = FALSE;
#यदि_अन्तः
  }

#यदि_परिभाषितम् CURL_CA_FALLBACK
  यदि(octx->store_is_रिक्तानि) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_समुच्चयः_default_paths(store);
    infof(दत्त, "  OpenSSL default paths (fallback)");
    octx->store_is_रिक्तानि = FALSE;
  }
#यदि_अन्तः
  यदि(octx->store_is_रिक्तानि && !have_native_check)
    infof(दत्त, "  no trust anchors configured");

  प्रत्यागम परिणाम;
}

स्थिर CURLcode ossl_populate_x509_store(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त,
                                         संरचना ossl_ctx *octx,
                                         X509_STORE *store)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  X509_LOOKUP *lookup = रिक्त;
  ध्रुव वर्ण * ध्रुव ssl_crlfile = conn_config->CRLfile;
  अचिह्नित दीर्घ x509flags = 0;

  CURL_TRC_CF(दत्त, cf, "configuring OpenSSL's x509 trust store");
  यदि(!store)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(!conn_config->verifypeer) {
    infof(दत्त, "SSL Trust: peer verification disabled");
    प्रत्यागम CURLE_OK;
  }

  infof(दत्त, "SSL Trust Anchors:");
  परिणाम = ossl_आरोपय_trust_anchors(cf, दत्त, octx, store);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  /* Does not make sense to load a CRL file without peer verification */
#यदि_परिभाषितम् USE_APPLE_SECTRUST
  यदि(ssl_crlfile && conn_config->native_ca_store) {
    failf(दत्त, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
  }
#यदि_अन्तः
  यदि(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_सङ्कलय_lookup(store, X509_LOOKUP_file());
    यदि(!lookup ||
       (!X509_आरोपय_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(दत्त, "error loading CRL file: %s", ssl_crlfile);
      प्रत्यागम CURLE_SSL_CRL_BADFILE;
    }
    x509flags = X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL;
    infof(दत्त, " CRLfile: %s", ssl_crlfile);
  }

  /* Try building a chain using issuers in the trusted store first to avoid
     problems with server-sent legacy intermediates. Newer versions of
     OpenSSL do alternate chain checking by default but we do not know how to
     determine that in a reliable manner.
     https://web.archive.org/web/20190422050538/rt.openssl.org/Ticket/Display.html?id=3621
   */
  x509flags |= X509_V_FLAG_TRUSTED_FIRST;

  यदि(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (शून्य)X509_STORE_समुच्चयः_flags(store, x509flags);

  प्रत्यागम परिणाम;
}

/* key to use at `multi->proto_hash` */
#परिभाषय MPROTO_OSSL_X509_कुञ्चिका  "tls:ossl:x509:share"

संरचना ossl_x509_share {
  वर्ण *CAfile;         /* CAfile path used to generate X509 store */
  वर्ण *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  संरचना curltime time; /* when the cached store was created */
  BIT(store_is_रिक्तानि);  /* no certs/paths/blobs are in the store */
  BIT(no_partialchain); /* keep partial chain state */
};

स्थिर शून्य oss_x509_share_free(ध्रुव शून्य *कुञ्चिका, size_t कुञ्चिका_len, शून्य *p)
{
  संरचना ossl_x509_share *share = p;
  DEBUGASSERT(कुञ्चिका_len == CURL_CSTRLEN(MPROTO_OSSL_X509_कुञ्चिका));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_कुञ्चिका, कुञ्चिका, कुञ्चिका_len));
  (शून्य)कुञ्चिका;
  (शून्य)कुञ्चिका_len;
  यदि(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

स्थिर bool ossl_cached_x509_store_expired(संरचना Curl_easy *दत्त,
                                           ध्रुव संरचना ossl_x509_share *mb)
{
  यदि(दत्त->समुच्चयः.ssl_ca_cache_timeout < 0)
    प्रत्यागम FALSE;
  अन्यथा {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &mb->time);
    timediff_t timeout_ms = दत्त->समुच्चयः.ssl_ca_cache_timeout * (timediff_t)1000;

    प्रत्यागम elapsed_ms >= timeout_ms;
  }
}

स्थिर bool ossl_cached_x509_store_different(संरचना Curl_cfilter *cf,
                                             ध्रुव संरचना Curl_easy *दत्त,
                                             ध्रुव संरचना ossl_x509_share *mb)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (शून्य)दत्त;
  यदि(mb->no_partialchain != conn_config->no_partialchain)
    प्रत्यागम TRUE;
  यदि(!mb->CAfile || !conn_config->CAfile) {
    यदि(mb->CAfile != conn_config->CAfile)
      प्रत्यागम TRUE;
  }
  अन्यथा यदि(वाक्यं_तुलय(mb->CAfile, conn_config->CAfile))
    प्रत्यागम TRUE;

  यदि(!mb->CApath || !conn_config->CApath) {
    यदि(mb->CApath != conn_config->CApath)
      प्रत्यागम TRUE;
  }
  अन्यथा यदि(वाक्यं_तुलय(mb->CApath, conn_config->CApath))
    प्रत्यागम TRUE;

  प्रत्यागम FALSE;
}

स्थिर X509_STORE *ossl_get_cached_x509_store(संरचना Curl_cfilter *cf,
                                              संरचना Curl_easy *दत्त,
                                              bool *pempty)
{
  संरचना Curl_multi *multi = दत्त->multi;
  संरचना ossl_x509_share *share;
  X509_STORE *store = रिक्त;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_कुञ्चिका,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_कुञ्चिका)) : रिक्त;
  यदि(share && share->store &&
     !ossl_cached_x509_store_expired(दत्त, share) &&
     !ossl_cached_x509_store_different(cf, दत्त, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_रिक्तानि;
  }

  प्रत्यागम store;
}

स्थिर शून्य ossl_समुच्चयः_cached_x509_store(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       X509_STORE *store,
                                       bool is_रिक्तानि)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_multi *multi = दत्त->multi;
  संरचना ossl_x509_share *share;

  DEBUGASSERT(multi);
  यदि(!multi)
    प्रत्यागम;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_कुञ्चिका,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_कुञ्चिका));

  यदि(!share) {
    share = curlx_calloc(1, परिमाण(*share));
    यदि(!share)
      प्रत्यागम;
    यदि(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_कुञ्चिका,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_कुञ्चिका),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      प्रत्यागम;
    }
  }

  यदि(X509_STORE_ऊर्ध्वम्_ref(store)) {
    वर्ण *CAfile = रिक्त;
    वर्ण *CApath = रिक्त;

    यदि(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      यदि(!CAfile) {
        X509_STORE_free(store);
        प्रत्यागम;
      }
    }

    यदि(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      यदि(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        प्रत्यागम;
      }
    }

    यदि(share->store) {
      X509_STORE_free(share->store);
      curlx_free(share->CAfile);
      curlx_free(share->CApath);
    }

    share->time = *Curl_pgrs_now(दत्त);
    share->store = store;
    share->store_is_रिक्तानि = is_रिक्तानि;
    share->CAfile = CAfile;
    share->CApath = CApath;
    share->no_partialchain = conn_config->no_partialchain;
  }
}

#यदि_परिभाषितम् OPENSSL_HAS_PROVIDERS
/* CApath certificates loaded under a provider libctx can outlive that libctx
   when the store is shared with the multi handle. */
#परिभाषय USING_PROVIDER(octx) octx->p.libctx
#अन्यथा
#परिभाषय USING_PROVIDER(x) 0
#यदि_अन्तः

CURLcode Curl_ssl_सज्जता_x509_store(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना ossl_ctx *octx)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  X509_STORE *cached_store;
  bool cache_criteria_met, is_रिक्तानि;

  /* Consider the X509 store cacheable if it comes from CAfile/CApath,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (दत्त->समुच्चयः.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !USING_PROVIDER(octx);

  ERR_समुच्चयः_mark();

  cached_store = ossl_get_cached_x509_store(cf, दत्त, &is_रिक्तानि);
  यदि(cached_store && cache_criteria_met && X509_STORE_ऊर्ध्वम्_ref(cached_store)) {
    SSL_CTX_समुच्चयः_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_रिक्तानि = is_रिक्तानि;
  }
  अन्यथा {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    परिणाम = ossl_populate_x509_store(cf, दत्त, octx, store);
    यदि(परिणाम == CURLE_OK && cache_criteria_met) {
      ossl_समुच्चयः_cached_x509_store(cf, दत्त, store, (bool)octx->store_is_रिक्तानि);
    }
  }

  ERR_निष्कासय_गन्तव्यम्_mark();

  प्रत्यागम परिणाम;
}

स्थिर bool ossl_apply_session(
  संरचना ossl_ctx *octx,
  संरचना Curl_cfilter *cf,
  संरचना Curl_easy *दत्त,
  संरचना alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  संरचना Curl_ssl_session *scs)
{
  संरचना ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  ध्रुव अचिह्नित वर्ण *der_sessionid = scs->sdata;
  size_t der_sessionid_आकार = scs->sdata_len;
  SSL_SESSION *ssl_session = रिक्त;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(रिक्त, &der_sessionid,
                                (दीर्घ)der_sessionid_आकार);
  यदि(ssl_session) {
    यदि(!SSL_समुच्चयः_session(octx->ssl, ssl_session)) {
      VERBOSE(वर्ण error_बफर[256]);
      infof(दत्त, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_बफर,
                          परिमाण(error_बफर)));
    }
    अन्यथा {
      यदि(conn_cfg->verifypeer &&
         (SSL_get_verify_परिणाम(octx->ssl) != X509_V_OK)
#यदि_परिभाषितम् USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#यदि_अन्तः
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_समुच्चयः_session(octx->ssl, रिक्त);
        infof(दत्त, "SSL session not peer verified, not reusing");
      }
      अन्यथा {
        infof(दत्त, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#यदि_परिभाषितम् USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#यदि_अन्तः
        infof(दत्त, "SSL verify result: %lx",
              (अचिह्नित दीर्घ)SSL_get_verify_परिणाम(octx->ssl));
#यदि_परिभाषितम् HAVE_OPENSSL_EARLYDATA
        यदि(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_दत्त(ssl_session) &&
           !cf->conn->द्वयङ्काः.connect_only &&
           (SSL_संस्करणम्(octx->ssl) == TLS1_3_संस्करणम्)) {
          bool do_early_दत्त = FALSE;
          यदि(sess_reuse_cb)
            (शून्य)sess_reuse_cb(cf, दत्त, alpns, scs, &do_early_दत्त);
          यदि(do_early_दत्त) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_गन्तव्यम्(alpns, scs->alpn);
          }
        }
#अन्यथा
        (शून्य)alpns;
        (शून्य)conn_cfg;
        (शून्य)sess_reuse_cb;
#यदि_अन्तः
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  अन्यथा {
    infof(दत्त, "SSL session not accepted by OpenSSL, continuing without");
  }
  प्रत्यागम (bool)octx->reused_session;
}

स्थिर CURLcode ossl_init_session_and_alpns(
  संरचना ossl_ctx *octx,
  संरचना Curl_cfilter *cf,
  संरचना Curl_easy *दत्त,
  संरचना ssl_peer *peer,
  ध्रुव संरचना alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  संरचना ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  संरचना alpn_spec alpns;
  CURLcode परिणाम;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  यदि((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    संरचना Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(दत्त, cf->cft, FIRSTSOCKET);
    यदि(scs) {
      यदि(ossl_apply_session(octx, cf, दत्त, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(दत्त, cf, "applied SSL session from control connection");
    }
  }

  यदि(!octx->reused_session &&
     Curl_ssl_scache_use(cf, दत्त) && !conn_cfg->verifystatus) {
    संरचना Curl_ssl_session *scs = रिक्त;

    परिणाम = Curl_ssl_scache_take(cf, दत्त, peer->scache_कुञ्चिका, &scs);
    यदि(!परिणाम && scs && scs->sdata && scs->sdata_len) {
      (शून्य)ossl_apply_session(octx, cf, दत्त, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, दत्त, peer->scache_कुञ्चिका, scs);
  }

  यदि(alpns.गणनम्) {
    संरचना alpn_proto_buf proto;
    स्मृतिं_पूरय(&proto, 0, परिमाण(proto));
    परिणाम = Curl_alpn_गन्तव्यम्_proto_buf(&proto, &alpns);
    यदि(परिणाम) {
      failf(दत्त, "Error determining ALPN");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    यदि(SSL_समुच्चयः_alpn_protos(octx->ssl, proto.दत्त, proto.len)) {
      failf(दत्त, "Error setting ALPN");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् HAVE_SSL_SET1_ECH_CONFIG_LIST
स्थिर CURLcode ossl_init_ech(संरचना ossl_ctx *octx,
                              संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              संरचना ssl_peer *peer)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णाङ्क trying_ech_now = 0;

  यदि(!Curl_ssl_ech_enabled(cf))
    प्रत्यागम CURLE_OK;

  यदि(conn_config->ech == CURLECH_GREASE) {
    infof(दत्त, "ECH: will GREASE ClientHello");
#यदि_परिभाषितम् HAVE_BORINGSSL_LIKE
    SSL_समुच्चयः_enable_ech_grease(octx->ssl, 1);
#अन्यथा
    SSL_समुच्चयः_options(octx->ssl, SSL_OP_ECH_GREASE);
#यदि_अन्तः
  }
  अन्यथा यदि(conn_config->ech && conn_config->ech_config) {
#यदि_परिभाषितम् HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode परिणाम;

    यदि(!conn_config->ech_config[0]) {
      infof(दत्त, "ECH: ECHConfig from command line empty");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * वाक्यदैर्घ्यम्(conn_config->ech_config);
    परिणाम = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    यदि(परिणाम || !ech_config) {
      infof(दत्त, "ECH: cannot base64 decode ECHConfig from command line");
      यदि(conn_config->ech == CURLECH_HARD)
        प्रत्यागम परिणाम;
    }
    यदि(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(दत्त, "ECH: SSL_ECH_set1_ech_config_list failed");
      यदि(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#अन्यथा
    size_t ech_config_len = 0;
    यदि(!conn_config->ech_config[0]) {
      infof(दत्त, "ECH: ECHConfig from command line empty");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = वाक्यदैर्घ्यम्(conn_config->ech_config);
    यदि(SSL_set1_ech_config_list(octx->ssl,
                                (ध्रुव uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(दत्त, "ECH: SSL_ECH_set1_ech_config_list failed");
      यदि(conn_config->ech == CURLECH_HARD)
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    अन्यथा
      trying_ech_now = 1;
#यदि_अन्तः /* HAVE_BORINGSSL_LIKE */
    infof(दत्त, "ECH: ECHConfig from command line");
  }
  अन्यथा {
    ध्रुव संरचना Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(दत्त, cf->sockindex, peer->origin);

    यदि(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      ध्रुव अचिह्नित वर्ण *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(दत्त, "ECH: ECHConfig from HTTPS RR");
      यदि(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(दत्त, "ECH: SSL_set1_ech_config_list failed");
        यदि(conn_config->ech == CURLECH_HARD)
          प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
      अन्यथा {
        trying_ech_now = 1;
        infof(दत्त, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    अन्यथा {
      infof(दत्त, "ECH: requested but no ECHConfig available");
      यदि(conn_config->ech == CURLECH_HARD)
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }
#यदि_परिभाषितम् HAVE_BORINGSSL_LIKE
  (शून्य)peer;
  यदि(trying_ech_now && conn_config->ech_लौकिकम्) {
    infof(दत्त, "ECH: setting public_name not supported with BoringSSL");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }
#अन्यथा
  यदि(trying_ech_now && conn_config->ech_लौकिकम्) {
    पूर्णाङ्क ret;
    infof(दत्त, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_लौकिकम्);
    ret = SSL_ech_set1_server_नामानि(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_लौकिकम्,
                                    0 /* do send outer */);
    यदि(ret != 1) {
      infof(दत्त, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }
#यदि_अन्तः /* HAVE_BORINGSSL_LIKE */
  यदि(trying_ech_now &&
     SSL_समुच्चयः_min_proto_संस्करणम्(octx->ssl, TLS1_3_संस्करणम्) != 1) {
    infof(दत्त, "ECH: cannot force TLSv1.3 [ERROR]");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

स्थिर CURLcode ossl_init_ssl(संरचना ossl_ctx *octx,
                              संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              संरचना ssl_peer *peer,
                              ध्रुव संरचना alpn_spec *alpns_requested,
                              शून्य *ssl_user_दत्त,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  यदि(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  यदि(!octx->ssl) {
    failf(दत्त, "SSL: could not create a context (handle)");
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  SSL_समुच्चयः_app_दत्त(octx->ssl, ssl_user_दत्त);

#यदि_अपरिभाषितम् OPENSSL_NO_OCSP
  यदि(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_समुच्चयः_tlsext_स्थितिः_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#यदि_अन्तः

  SSL_समुच्चयः_connect_state(octx->ssl);

  यदि(peer->sni) {
    यदि(!SSL_समुच्चयः_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(दत्त, "Failed set SNI");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

#यदि_परिभाषितम् HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode परिणाम = ossl_init_ech(octx, cf, दत्त, peer);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
#यदि_अन्तः /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  प्रत्यागम ossl_init_session_and_alpns(octx, cf, दत्त, peer,
                                     alpns_requested, sess_reuse_cb);
}

स्थिर CURLcode ossl_init_विधानम्(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना ssl_peer *peer,
                                 ध्रुव SSL_विधानम् **pmethod,
                                 अचिह्नित पूर्णाङ्क *pssl_संस्करणम्_min)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = रिक्त;
  *pssl_संस्करणम्_min = conn_config->संस्करणम्_2;
  DEBUGASSERT(conn_config->संस्करणम्_2 != CURL_SSLVERSION_DEFAULT);
  विकल्प(peer->transport) {
  पक्ष TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    विकल्प(*pssl_संस्करणम्_min) {
    पक्ष CURL_SSLVERSION_TLSv1:
    पक्ष CURL_SSLVERSION_TLSv1_0:
    पक्ष CURL_SSLVERSION_TLSv1_1:
    पक्ष CURL_SSLVERSION_TLSv1_2:
    पक्ष CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_विधानम्();
      भङ्ग;
    पक्ष CURL_SSLVERSION_SSLv2:
      failf(दत्त, "No SSLv2 support");
      प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
    पक्ष CURL_SSLVERSION_SSLv3:
      failf(दत्त, "No SSLv3 support");
      प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
    स्वभाव:
      failf(दत्त, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    भङ्ग;
  पक्ष TRNSPRT_QUIC:
    *pssl_संस्करणम्_min = CURL_SSLVERSION_TLSv1_3;
    यदि(conn_config->संस्करणम्_max &&
       (conn_config->संस्करणम्_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->संस्करणम्_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(दत्त, "QUIC needs at least TLS version 1.3");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_विधानम्();
    भङ्ग;
  स्वभाव:
    failf(दत्त, "unsupported transport %d in SSL init", peer->transport);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  प्रत्यागम *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(संरचना ossl_ctx *octx,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            संरचना ssl_peer *peer,
                            ध्रुव संरचना alpn_spec *alpns_requested,
                            Curl_ossl_ctx_सज्जता_cb *cb_सज्जता,
                            शून्य *cb_user_दत्त,
                            Curl_ossl_new_session_cb *cb_new_session,
                            शून्य *ssl_user_दत्त,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *ciphers;
  ध्रुव SSL_विधानम् *req_विधानम् = रिक्त;
  ctx_option_t ctx_options = 0;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, दत्त);
  वर्ण * ध्रुव ssl_cert = conn_config->clientcert;
  ध्रुव संरचना curl_blob *ssl_cert_blob = conn_config->cert_blob;
  ध्रुव वर्ण * ध्रुव ssl_cert_type = conn_config->cert_type;
  अचिह्नित पूर्णाङ्क ssl_संस्करणम्_min = CURL_SSLVERSION_DEFAULT;
  वर्ण error_बफर[256];

  /* Make funny stuff to get random input */
  परिणाम = ossl_बीजम्(दत्त);
  यदि(!परिणाम) {
    यदि(CURL_EASY_STR(दत्त, STRING_SSL_ENGINE))
      परिणाम = ossl_समुच्चयः_engine(दत्त, octx,
                               CURL_EASY_STR(दत्त, STRING_SSL_ENGINE));
    यदि(!परिणाम && दत्त->समुच्चयः.engine_default)
      परिणाम = ossl_समुच्चयः_engine_default(दत्त, octx);
  }

  यदि(!परिणाम) {
    ssl_config->certverifyresult = !X509_V_OK;
    परिणाम = ossl_init_विधानम्(cf, दत्त, peer, &req_विधानम्, &ssl_संस्करणम्_min);
  }
  यदि(परिणाम)
    प्रत्यागम परिणाम;
  DEBUGASSERT(req_विधानम्);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#यदि_परिभाषितम् OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_विधानम्):
#यदि_अन्तः
    SSL_CTX_new(req_विधानम्);

  यदि(!octx->ssl_ctx) {
    failf(दत्त, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_बफर, परिमाण(error_बफर)));
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(cb_सज्जता) {
    परिणाम = cb_सज्जता(cf, दत्त, cb_user_दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(दत्त->समुच्चयः.fdebug && दत्त->समुच्चयः.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_समुच्चयः_सन्देशः_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_समुच्चयः_सन्देशः_callback_arg(octx->ssl_ctx, cf);
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
  यदि(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_निवेशय_रिक्तानि_FRAGMENTS;

  DEBUGASSERT(ssl_संस्करणम्_min != CURL_SSLVERSION_DEFAULT);
  विकल्प(ssl_संस्करणम्_min) {
  पक्ष CURL_SSLVERSION_SSLv2:
  पक्ष CURL_SSLVERSION_SSLv3:
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  पक्ष CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  पक्ष CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  पक्ष CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  पक्ष CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  पक्ष CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    परिणाम = ossl_समुच्चयः_ssl_संस्करणम्_min_max(cf, octx->ssl_ctx, ssl_संस्करणम्_min);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    भङ्ग;

  स्वभाव:
    failf(दत्त, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  SSL_CTX_समुच्चयः_options(octx->ssl_ctx, ctx_options);
  SSL_CTX_समुच्चयः_read_ahead(octx->ssl_ctx, 1);

  /* Max TLS1.2 record size 0x4000 + 0x800.
     OpenSSL supports processing "jumbo TLS record" (8 TLS records) in one go
     for some algorithms, so match that here.
     Experimentation shows that a slightly larger buffer is needed
     to avoid short reads.

     However using a large buffer (8 packets) actually decreases performance.
     4 packets is better.
   */
#यदि_परिभाषितम् HAVE_SSL_CTX_समुच्चयः_DEFAULT_READ_बफर_LEN
  SSL_CTX_समुच्चयः_default_read_बफर_len(octx->ssl_ctx, 0x401e * 4);
#यदि_अन्तः

  /* We do retry writes sometimes from another buffer address */
  SSL_CTX_समुच्चयः_बहुलकम्(octx->ssl_ctx, SSL_बहुलकम्_ACCEPT_MOVING_WRITE_बफर);

  ciphers = conn_config->cipher_list;
  यदि(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = रिक्त;
  यदि(ciphers && (ssl_संस्करणम्_min < CURL_SSLVERSION_TLSv1_3)) {
    यदि(!SSL_CTX_समुच्चयः_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(दत्त, "failed setting cipher list: %s", ciphers);
      प्रत्यागम CURLE_SSL_CIPHER;
    }
    infof(दत्त, "Cipher selection: %s", ciphers);
  }

#यदि_परिभाषितम् HAVE_SSL_CTX_समुच्चयः_CIPHERSUITES
  {
    ध्रुव वर्ण *ciphers13 = conn_config->cipher_list13;
    यदि(ciphers13 &&
       (!conn_config->संस्करणम्_max ||
        (conn_config->संस्करणम्_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->संस्करणम्_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      यदि(!SSL_CTX_समुच्चयः_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(दत्त, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        प्रत्यागम CURLE_SSL_CIPHER;
      }
      infof(दत्त, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#यदि_अन्तः

  यदि(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    परिणाम = client_cert(दत्त, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->कुञ्चिका, conn_config->कुञ्चिका_blob,
                         conn_config->कुञ्चिका_type,
                         conn_config->कुञ्चिका_passwd);
    यदि(परिणाम)
      /* failf() is already done in client_cert() */
      प्रत्यागम परिणाम;
  }

#यदि_परिभाषितम् HAVE_SSL_CTX_समुच्चयः_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_समुच्चयः_post_handshake_auth(octx->ssl_ctx, 1);
#यदि_अन्तः

  {
    ध्रुव वर्ण *curves = conn_config->curves;
    यदि(curves) {
#यदि_परिभाषितम् HAVE_BORINGSSL_LIKE
#परिभाषय OSSL_वक्रम्_CAST(x) (x)
#अन्यथा
#परिभाषय OSSL_वक्रम्_CAST(x) (वर्ण *)CURL_UNCONST(x)
#यदि_अन्तः
      यदि(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_वक्रम्_CAST(curves))) {
        failf(दत्त, "failed setting curves list: '%s'", curves);
        प्रत्यागम CURLE_SSL_CIPHER;
      }
    }
  }

#यदि_परिभाषितम् HAVE_SSL_CTX_SET1_SIGALGS
#परिभाषय OSSL_SIGALG_CAST(x) OSSL_वक्रम्_CAST(x)
  {
    ध्रुव वर्ण *signature_algorithms = conn_config->signature_algorithms;
    यदि(signature_algorithms) {
      यदि(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(दत्त, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        प्रत्यागम CURLE_SSL_CIPHER;
      }
    }
  }
#यदि_अन्तः

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_समुच्चयः_verify(octx->ssl_ctx, SSL_VERIFY_NONE, रिक्त);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#यदि !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  यदि(Curl_tls_keylog_enabled()) {
    SSL_CTX_समुच्चयः_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#यदि_अन्तः

  यदि(cb_new_session) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_समुच्चयः_session_cache_बहुलकम्(octx->ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_आन्तरिकम्);
    SSL_CTX_sess_समुच्चयः_new_cb(octx->ssl_ctx, cb_new_session);
  }

  /* give application a chance to interfere with SSL set up. */
  यदि(दत्त->समुच्चयः.ssl_fsslctx) {
    संरचना Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    यदि(!octx->x509_store_सज्जता) {
      परिणाम = Curl_ssl_सज्जता_x509_store(cf, दत्त, octx);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
      octx->x509_store_सज्जता = TRUE;
    }
    CURL_CBAPI_START(&guard, दत्त, easy_fsslctx);
    परिणाम = (*दत्त->समुच्चयः.ssl_fsslctx)(दत्त, octx->ssl_ctx,
                                      दत्त->समुच्चयः.ssl_fsslctxp);
    CURL_CBAPI_अन्तः(&guard);
    यदि(परिणाम) {
      failf(दत्त, "error signaled by SSL ctx callback");
      प्रत्यागम परिणाम;
    }
  }

  प्रत्यागम ossl_init_ssl(octx, cf, दत्त, peer, alpns_requested,
                       ssl_user_दत्त, sess_reuse_cb);
}

स्थिर CURLcode ossl_on_session_reuse(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      संरचना alpn_spec *alpns,
                                      संरचना Curl_ssl_session *scs,
                                      bool *do_early_दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  प्रत्यागम Curl_on_session_reuse(cf, दत्त, alpns, scs, do_early_दत्त,
                               connssl->earlydata_max);
}

शून्य Curl_ossl_report_handshake(संरचना Curl_easy *दत्त, संरचना ossl_ctx *octx)
{
#यदि_परिभाषितम् CURLVERBOSE
  यदि(Curl_trc_is_verbose(दत्त)) {
    पूर्णाङ्क psigtype_nid = NID_undef;
    ध्रुव वर्ण *negotiated_गणः_name = रिक्त;

#यदि_परिभाषितम् HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#यदि OPENSSL_संस्करणम्_सङ्ख्या >= 0x30200000L
    negotiated_गणः_name = SSL_get0_गणः_name(octx->ssl);
#अन्यथा
    negotiated_गणः_name =
      OBJ_nid2sn(SSL_get_negotiated_गणः(octx->ssl) & 0x0000FFFF);
#यदि_अन्तः
#यदि_अन्तः

    /* Informational message */
    infof(दत्त, "SSL connection using %s / %s / %s / %s",
          SSL_get_संस्करणम्(octx->ssl),
          SSL_get_cipher(octx->ssl),
          negotiated_गणः_name ? negotiated_गणः_name : "[blank]",
          OBJ_nid2sn(psigtype_nid));
  }
#अन्यथा
  (शून्य)दत्त;
  (शून्य)octx;
#यदि_अन्तः /* CURLVERBOSE */
}

स्थिर CURLcode ossl_connect_step1(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode परिणाम;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  परिणाम = Curl_ossl_ctx_init(octx, cf, दत्त, &connssl->peer,
                              connssl->alpn, रिक्त, रिक्त,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  octx->bio_विधानम् = ossl_bio_cf_विधानम्_create();
  यदि(!octx->bio_विधानम्)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  bio = BIO_new(octx->bio_विधानम्);
  यदि(!bio)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  BIO_समुच्चयः_दत्त(bio, cf);
#यदि_परिभाषितम् HAVE_SSL_SET0_WBIO
  /* with OpenSSL v1.1.1 we get an alternative to SSL_set_bio() that works
   * without backward compat quirks. Every call takes one reference, so we
   * up it and pass. SSL* then owns and frees it.
   * We check on the function in configure, since LibreSSL and friends
   * each have their own versions to add support for this. */
  BIO_ऊर्ध्वम्_ref(bio);
  SSL_set0_rbio(octx->ssl, bio);
  SSL_set0_wbio(octx->ssl, bio);
#अन्यथा
  SSL_समुच्चयः_bio(octx->ssl, bio, bio);
#यदि_अन्तः

  यदि(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    संरचना alpn_proto_buf proto;
    स्मृतिं_पूरय(&proto, 0, परिमाण(proto));
    Curl_alpn_गन्तव्यम्_proto_str(&proto, connssl->alpn);
    infof(दत्त, VTLS_INFOF_ALPN_OFFER_1STR, proto.दत्त);
  }

  connssl->connecting_state = ssl_connect_2;
  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
स्थिर पूर्णाङ्क ossl_trace_ech_retry_configs(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त, SSL *ssl,
                                        पूर्णाङ्क reason)
{
  CURLcode परिणाम = CURLE_OK;
  size_t rcl = 0;
  पूर्णाङ्क rv = 1;
#यदि_अपरिभाषितम् HAVE_BORINGSSL_LIKE
  वर्ण *inner = रिक्त;
  uint8_t *rcs = रिक्त;
  वर्ण *outer = रिक्त;
#अन्यथा
  ध्रुव वर्ण *inner = रिक्त;
  ध्रुव uint8_t *rcs = रिक्त;
  ध्रुव वर्ण *outer = रिक्त;
  size_t बहिर्गमनम्_name_len = 0;
  पूर्णाङ्क servername_type = 0;
#यदि_अन्तः
  NOVERBOSE((शून्य)reason);

  /* nothing to trace if not doing ECH */
  यदि(!Curl_ssl_ech_enabled(cf))
    प्रत्यागम rv;
#यदि_अपरिभाषितम् HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#अन्यथा
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (पूर्णाङ्क)rcl;
#यदि_अन्तः

  यदि(rv && rcs) {
    वर्ण *b64str = रिक्त;
    size_t blen = 0;

    परिणाम = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    यदि(!परिणाम && b64str) {
      infof(दत्त, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#यदि_अपरिभाषितम् HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_स्थितिः(ssl, &inner, &outer);
      infof(दत्त, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#अन्यथा
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &बहिर्गमनम्_name_len);
      infof(दत्त, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#यदि_अन्तः
    }
  }
  अन्यथा
    infof(दत्त, "ECH: no retry_configs (rv = %d)", rv);
#यदि_अपरिभाषितम् HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#यदि_अन्तः
  प्रत्यागम rv;
}

#यदि_अन्तः

स्थिर CURLcode ossl_connect_step2(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, दत्त);
  पूर्णाङ्क err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  यदि(!octx->x509_store_सज्जता) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode परिणाम = Curl_ssl_सज्जता_x509_store(cf, दत्त, octx);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    octx->x509_store_सज्जता = TRUE;
  }

#यदि !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  यदि(Curl_tls_keylog_enabled() && !octx->keylog_समाप्तम्)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_समाप्तम्);
#यदि_अन्तः

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  यदि(err != 1) {
    पूर्णाङ्क detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(दत्त, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    यदि(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(दत्त, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      प्रत्यागम CURLE_AGAIN;
    }
    यदि(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(दत्त, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      प्रत्यागम CURLE_AGAIN;
    }
#यदि_परिभाषितम् SSL_ERROR_WANT_ASYNC
    यदि(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(दत्त, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      प्रत्यागम CURLE_AGAIN;
    }
#यदि_अन्तः
#यदि_परिभाषितम् SSL_ERROR_WANT_RETRY_VERIFY
    यदि(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(दत्त, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(दत्त, TRUE);
      प्रत्यागम CURLE_AGAIN;
    }
#यदि_अन्तः
    अन्यथा {
      /* untreated error */
      sslerr_t errdetail;
      वर्ण error_बफर[256] = "";
      CURLcode परिणाम;
      दीर्घ lerr;
      पूर्णाङ्क lib;
      पूर्णाङ्क reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      यदि((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#यदि_परिभाषितम् SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#यदि_अन्तः
         )) {
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_परिणाम(octx->ssl);
        यदि(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(दत्त, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        अन्यथा
          failf(दत्त, "%s", "SSL certificate verification failed");
      }
#यदि_परिभाषितम् SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      अन्यथा यदि((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        परिणाम = CURLE_SSL_CLIENTCERT;
        failf(दत्त, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_बफर, परिमाण(error_बफर)));
      }
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SSL_SET1_ECH_CONFIG_LIST
      अन्यथा यदि((lib == ERR_LIB_SSL) &&
#यदि_अपरिभाषितम् HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#अन्यथा
              (reason == SSL_R_ECH_REJECTED)) {
#यदि_अन्तः /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, दत्त, octx->ssl, reason);

        परिणाम = CURLE_ECH_REQUIRED;
        failf(दत्त, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_बफर, परिमाण(error_बफर)));
      }
#यदि_अन्तः
      अन्यथा {
        परिणाम = CURLE_SSL_CONNECT_ERROR;
        failf(दत्त, "TLS connect error: %s",
              ossl_strerror(errdetail, error_बफर, परिमाण(error_बफर)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      यदि(परिणाम == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        वर्ण extramsg[80] = "";
        पूर्णाङ्क sockerr = SOCKERRNO;

        यदि(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, परिमाण(extramsg));
        failf(दत्त, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_गन्तव्यम्_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->पत्तनम्);
      }

      प्रत्यागम परिणाम;
    }
  }
  अन्यथा {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(दत्त, octx);

#यदि defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    यदि(Curl_ssl_ech_enabled(cf)) {
      संरचना ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      वर्ण *inner = रिक्त, *outer = रिक्त;
      पूर्णाङ्क rv;
      VERBOSE(ध्रुव वर्ण *स्थितिः);

      rv = SSL_ech_get1_स्थितिः(octx->ssl, &inner, &outer);
      विकल्प(rv) {
      पक्ष SSL_ECH_स्थितिः_SUCCESS:
        VERBOSE(स्थितिः = "succeeded");
        भङ्ग;
      पक्ष SSL_ECH_स्थितिः_GREASE_ECH:
        VERBOSE(स्थितिः = "sent GREASE, got retry-configs");
        भङ्ग;
      पक्ष SSL_ECH_स्थितिः_GREASE:
        VERBOSE(स्थितिः = "sent GREASE");
        भङ्ग;
      पक्ष SSL_ECH_स्थितिः_NOT_TRIED:
        VERBOSE(स्थितिः = "not attempted");
        भङ्ग;
      पक्ष SSL_ECH_स्थितिः_NOT_CONFIGURED:
        VERBOSE(स्थितिः = "not configured");
        भङ्ग;
      पक्ष SSL_ECH_स्थितिः_BACKEND:
        VERBOSE(स्थितिः = "backend (unexpected)");
        भङ्ग;
      पक्ष SSL_ECH_स्थितिः_FAILED:
        VERBOSE(स्थितिः = "failed");
        भङ्ग;
      पक्ष SSL_ECH_स्थितिः_BAD_CALL:
        VERBOSE(स्थितिः = "bad call (unexpected)");
        भङ्ग;
      पक्ष SSL_ECH_स्थितिः_BAD_NAME: {
        यदि(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !वाक्यं_तुलय(inner, connssl->peer.origin->hostname)) {
          VERBOSE(स्थितिः = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_स्थितिः_SUCCESS;
        }
        अन्यथा {
          VERBOSE(स्थितिः = "bad name (unexpected)");
        }
        भङ्ग;
      }
      स्वभाव:
        VERBOSE(स्थितिः = "unexpected status");
        infof(दत्त, "ECH: unexpected status %d", rv);
      }
      infof(दत्त, "ECH: result '%s' (inner=%s, outer=%s)",
            (स्थितिः ? स्थितिः : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      यदि(rv == SSL_ECH_स्थितिः_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, दत्त, octx->ssl, 0);
      }
      यदि(rv != SSL_ECH_स्थितिः_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(दत्त, "ECH: ech-hard failed");
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
    }
    अन्यथा {
      infof(दत्त, "ECH: result: status is not attempted");
    }
#यदि_अन्तः /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    यदि(connssl->alpn) {
      ध्रुव अचिह्नित वर्ण *neg_protocol;
      अचिह्नित पूर्णाङ्क len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      प्रत्यागम Curl_alpn_समुच्चयः_negotiated(cf, दत्त, connssl, neg_protocol, len);
    }

    प्रत्यागम CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
स्थिर CURLcode ossl_pkp_pin_peer_pubkey(संरचना Curl_easy *दत्त, X509 *cert,
                                         ध्रुव वर्ण *pinnedpubkey)
{
  /* Scratch */
  पूर्णाङ्क len1 = 0, len2 = 0;
  अचिह्नित वर्ण *buff1 = रिक्त, *अस्थायी = रिक्त;

  /* Result is returned to caller */
  CURLcode परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  यदि(!pinnedpubkey)
    प्रत्यागम CURLE_OK;

  यदि(!cert)
    प्रत्यागम परिणाम;

  कुरु {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), रिक्त);
    यदि(len1 < 1)
      भङ्ग; /* failed */

    buff1 = अस्थायी = curlx_malloc(len1);
    यदि(!buff1)
      भङ्ग; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &अस्थायी);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    यदि((len1 != len2) || !अस्थायी || ((अस्थायी - buff1) != len1))
      भङ्ग; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    परिणाम = Curl_pin_peer_pubkey(दत्त, pinnedpubkey, buff1, len1);
  } यावत्(0);

  यदि(buff1)
    curlx_free(buff1);

  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् CURLVERBOSE
#यदि !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_संस्करणम्_सङ्ख्या) && LIBRESSL_संस्करणम्_सङ्ख्या < 0x3060000fL)
स्थिर शून्य infof_certstack(संरचना Curl_easy *दत्त, ध्रुव SSL *ssl)
{
  राशिः_सम्बन्धि(X509) *certstack;
  दीर्घ verify_परिणाम;
  पूर्णाङ्क अङ्कः_cert_स्तराः;
  पूर्णाङ्क cert_level;

  यदि(!Curl_trc_is_verbose(दत्त))
    प्रत्यागम;

  verify_परिणाम = SSL_get_verify_परिणाम(ssl);
  यदि(verify_परिणाम != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  अन्यथा
    certstack = SSL_get0_verified_chain(ssl);
  यदि(!certstack)
    प्रत्यागम;
  अङ्कः_cert_स्तराः = sk_X509_अङ्कः(certstack);

  प्रति(cert_level = 0; cert_level < अङ्कः_cert_स्तराः; cert_level++) {
    वर्ण cert_algorithm[80] = "";
    वर्ण गणः_name_final[80] = "";
    ध्रुव X509_ALGOR *palg_cert = रिक्त;
    ध्रुव ASN1_OBJECT *paobj_cert = रिक्त;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    पूर्णाङ्क कुञ्चिका_द्वयङ्काः;
    पूर्णाङ्क कुञ्चिका_sec_द्वयङ्काः;
    पूर्णाङ्क get_गणः_name;
    ध्रुव वर्ण *type_name;

    current_cert = sk_X509_मानम्(certstack, cert_level);
    यदि(!current_cert)
      अनुवर्त;

    current_pkey = X509_get0_pubkey(current_cert);
    यदि(!current_pkey)
      अनुवर्त;

    X509_get0_signature(रिक्त, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, रिक्त, रिक्त, palg_cert);
    OBJ_obj2txt(cert_algorithm, परिमाण(cert_algorithm), paobj_cert, 0);

    कुञ्चिका_द्वयङ्काः = EVP_PKEY_द्वयङ्काः(current_pkey);
#यदि_अपरिभाषितम् HAVE_OPENSSL3
#परिभाषय EVP_PKEY_get_security_द्वयङ्काः EVP_PKEY_security_द्वयङ्काः
#यदि_अन्तः
    कुञ्चिका_sec_द्वयङ्काः = EVP_PKEY_get_security_द्वयङ्काः(current_pkey);
#यदि_परिभाषितम् HAVE_OPENSSL3
    {
      वर्ण गणः_name[80] = "";
      get_गणः_name = EVP_PKEY_get_गणः_name(current_pkey, गणः_name,
                                               परिमाण(गणः_name), रिक्त);
      curl_msnprintf(गणः_name_final, परिमाण(गणः_name_final), "/%s",
                     गणः_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#अन्यथा
    get_गणः_name = 0;
    type_name = रिक्त;
#यदि_अन्तः

    infof(दत्त, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_गणः_name == 0 ? "" : गणः_name_final,
          कुञ्चिका_द्वयङ्काः, कुञ्चिका_sec_द्वयङ्काः, cert_algorithm);
  }
}
#अन्यथा
#परिभाषय infof_certstack(दत्त, ssl)
#यदि_अन्तः
#यदि_अन्तः /* CURLVERBOSE */

स्थिर CURLcode ossl_check_issuer(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  X509 *server_cert)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = रिक्त;
  BIO *fp = रिक्त;
  वर्ण err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode परिणाम = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  यदि(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->दत्त,
                         (पूर्णाङ्क)conn_config->issuercert_blob->len);
    यदि(!fp) {
      failf(दत्त, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, परिमाण(err_buf)));
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }
  }
  अन्यथा यदि(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    यदि(!fp) {
      failf(दत्त, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, परिमाण(err_buf)));
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ बहिर्गमनम्;
    }

    यदि(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      यदि(verify_enabled)
        failf(दत्त, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

  यदि(fp) {
    issuer = PEM_read_bio_X509(fp, रिक्त, ZERO_NULL, रिक्त);
    यदि(!issuer) {
      यदि(verify_enabled)
        failf(दत्त, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      गच्छ बहिर्गमनम्;
    }

    यदि(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      यदि(verify_enabled)
        failf(दत्त, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      गच्छ बहिर्गमनम्;
    }

    infof(दत्त, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

बहिर्गमनम्:
  यदि(fp)
    BIO_free(fp);
  यदि(issuer)
    X509_free(issuer);
  प्रत्यागम परिणाम;
}

स्थिर ध्रुव वर्ण *pinned(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *दत्त)
{
  (शून्य)cf;
  प्रत्यागम
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#यदि_अन्तः
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY);
}

स्थिर CURLcode ossl_check_pinned_कुञ्चिका(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      X509 *server_cert)
{
  CURLcode परिणाम = CURLE_OK;
  ध्रुव वर्ण *ptr = pinned(cf, दत्त);
  यदि(ptr) {
    परिणाम = ossl_pkp_pin_peer_pubkey(दत्त, server_cert, ptr);
    यदि(परिणाम)
      failf(दत्त, "SSL: public key does not match pinned public key");
  }
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् CURLVERBOSE
#परिभाषय MAX_CERT_NAME_दीर्घता 2048
स्थिर CURLcode ossl_infof_cert(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                X509 *server_cert)
{
  BIO *mem = रिक्त;
  संरचना dynbuf dname;
  वर्ण err_buf[256] = "";
  वर्ण *buf;
  दीर्घ len;
  CURLcode परिणाम = CURLE_OK;

  यदि(!Curl_trc_is_verbose(दत्त))
    प्रत्यागम CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_दीर्घता);
  mem = BIO_new(BIO_s_mem());
  यदि(!mem) {
    failf(दत्त, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, परिमाण(err_buf)));
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  infof(दत्त, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  परिणाम = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(दत्त, "  subject: %s", परिणाम ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_TIME_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_दत्त(mem, (वर्ण **)&buf);
  infof(दत्त, "  start date: %.*s", (पूर्णाङ्क)len, buf);
  (शून्य)BIO_reset(mem);

  ASN1_TIME_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_दत्त(mem, (वर्ण **)&buf);
  infof(दत्त, "  expire date: %.*s", (पूर्णाङ्क)len, buf);
  (शून्य)BIO_reset(mem);

  परिणाम = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  यदि(परिणाम) /* should be only fatal stuff like OOM */
    गच्छ बहिर्गमनम्;
  infof(दत्त, "  issuer: %s", curlx_dyn_ptr(&dname));

बहिर्गमनम्:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः /* CURLVERBOSE */

#यदि_परिभाषितम् USE_APPLE_SECTRUST
संरचना ossl_certs_ctx {
  राशिः_सम्बन्धि(X509) *sk;
  size_t अङ्कः_certs;
  अचिह्नित वर्ण *चरमः_der;
};

स्थिर CURLcode ossl_chain_get_der(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   शून्य *user_दत्त,
                                   size_t i,
                                   अचिह्नित वर्ण **pder,
                                   size_t *pder_len)
{
  संरचना ossl_certs_ctx *chain = user_दत्त;
  X509 *cert;
  पूर्णाङ्क der_len;

  OPENSSL_free(chain->चरमः_der);
  chain->चरमः_der = रिक्त;

  (शून्य)cf;
  (शून्य)दत्त;
  *pder_len = 0;
  *pder = रिक्त;

  यदि(i >= chain->अङ्कः_certs)
    प्रत्यागम CURLE_TOO_LARGE;
  cert = sk_X509_मानम्(chain->sk, (पूर्णाङ्क)i);
  यदि(!cert)
    प्रत्यागम CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  यदि(der_len < 0)
    प्रत्यागम CURLE_FAILED_INIT;
  chain->चरमः_der = *pder;
  *pder_len = (size_t)der_len;
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ossl_apple_verify(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना ossl_ctx *octx,
                                  संरचना ssl_peer *peer)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ossl_certs_ctx chain;
  CURLcode परिणाम;

  octx->sectrust_verified = FALSE;
  स्मृतिं_पूरय(&chain, 0, परिमाण(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.अङ्कः_certs = chain.sk ? sk_X509_अङ्कः(chain.sk) : 0;

  यदि(!chain.अङ्कः_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    यदि(!octx->reused_session) {
      failf(दत्त, "SSL: could not get peer certificate chain");
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    }
    अन्यथा {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(दत्त, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      प्रत्यागम CURLE_OK;
    }
  }
  अन्यथा {
#यदि_परिभाषितम् HAVE_BORINGSSL_LIKE
    ध्रुव uint8_t *ocsp_दत्त = रिक्त;
#अन्यथा
    अचिह्नित वर्ण *ocsp_दत्त = रिक्त;
#यदि_अन्तः
    दीर्घ ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    यदि(!octx->reused_session)
      ocsp_len = (दीर्घ)SSL_get_tlsext_स्थितिः_ocsp_resp(octx->ssl, &ocsp_दत्त);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    यदि(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_दत्त = रिक्त;
    }

    परिणाम = Curl_vtls_apple_verify(cf, दत्त, peer, chain.अङ्कः_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_दत्त, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.चरमः_der);
    chain.चरमः_der = रिक्त;
  }
  octx->sectrust_verified = !परिणाम;
  प्रत्यागम परिणाम;
}
#यदि_अन्तः /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   संरचना ossl_ctx *octx,
                                   संरचना ssl_peer *peer)
{
  संरचना connectdata *conn = cf->conn;
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, दत्त);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  दीर्घ ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  यदि(दत्त->समुच्चयः.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    परिणाम = ossl_certchain(दत्त, octx->ssl);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  यदि(!server_cert) {
    /* no verification at all, this maybe acceptable */
    यदि(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, दत्त))
      गच्छ बहिर्गमनम्;

    failf(दत्त, "SSL: could not get peer certificate");
    परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    गच्छ बहिर्गमनम्;
  }

#यदि_परिभाषितम् CURLVERBOSE
  परिणाम = ossl_infof_cert(cf, दत्त, server_cert);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  infof_certstack(दत्त, octx->ssl);
#यदि_अन्तः

  यदि(conn_config->verifyhost) {
    परिणाम = ossl_verifyhost(दत्त, conn, peer, server_cert);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_परिणाम(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(दत्त, "OpenSSL verify result: %lx", (अचिह्नित दीर्घ)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  यदि(verified)
    infof(दत्त, "SSL certificate verified via OpenSSL.");

#यदि_परिभाषितम् USE_APPLE_SECTRUST
  यदि(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    परिणाम = ossl_apple_verify(cf, दत्त, octx, peer);
    यदि(परिणाम && (परिणाम != CURLE_PEER_FAILED_VERIFICATION))
      गच्छ बहिर्गमनम्; /* unexpected error */
    यदि(octx->sectrust_verified) {
      infof(दत्त, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#यदि_अन्तः

  यदि(!verified) {
    /* no trust established, report the OpenSSL status */
    यदि(conn_config->verifypeer) {
      failf(दत्त, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
      गच्छ बहिर्गमनम्;
    }
    infof(दत्त, " SSL certificate verification failed, continuing anyway!");
  }

#यदि_अपरिभाषितम् OPENSSL_NO_OCSP
  यदि(conn_config->verifystatus &&
#यदि_परिभाषितम् USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, दत्त)) &&
#यदि_अन्तः
     !octx->reused_session) {
    परिणाम = verifystatus(cf, दत्त, octx);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
#यदि_अन्तः

  परिणाम = ossl_check_issuer(cf, दत्त, server_cert);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = ossl_check_pinned_कुञ्चिका(cf, दत्त, server_cert);

बहिर्गमनम्:
  X509_free(server_cert);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ossl_connect_step3(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  परिणाम = Curl_ossl_check_peer_cert(cf, दत्त, octx, &connssl->peer);
  यदि(परिणाम)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, दत्त, connssl->peer.scache_कुञ्चिका);

  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् HAVE_OPENSSL_EARLYDATA
स्थिर CURLcode ossl_send_earlydata(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  ध्रुव अचिह्नित वर्ण *buf;
  size_t blen, nwritten;
  पूर्णाङ्क rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_परिणाम = CURLE_OK;
  यावत्(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_दत्त(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(दत्त, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    यदि(rc <= 0) {
      दीर्घ sslerror;
      वर्ण error_बफर[256];
      पूर्णाङ्क err = SSL_get_error(octx->ssl, rc);

      विकल्प(err) {
      पक्ष SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        परिणाम = CURLE_AGAIN;
        गच्छ बहिर्गमनम्;
      पक्ष SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        परिणाम = CURLE_AGAIN;
        गच्छ बहिर्गमनम्;
      पक्ष SSL_ERROR_SYSCALL: {
        पूर्णाङ्क sockerr = SOCKERRNO;

        यदि(octx->io_परिणाम == CURLE_AGAIN) {
          परिणाम = CURLE_AGAIN;
          गच्छ बहिर्गमनम्;
        }
        sslerror = ERR_get_error();
        यदि(sslerror)
          ossl_strerror(sslerror, error_बफर, परिमाण(error_बफर));
        अन्यथा यदि(sockerr)
          curlx_strerror(sockerr, error_बफर, परिमाण(error_बफर));
        अन्यथा
          curl_msnprintf(error_बफर, परिमाण(error_बफर), "%s",
                         SSL_ERROR_गन्तव्यम्_str(err));

        failf(दत्त, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_बफर, sockerr);
        परिणाम = CURLE_SEND_ERROR;
        गच्छ बहिर्गमनम्;
      }
      पक्ष SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(दत्त, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_बफर, परिमाण(error_बफर)));
        परिणाम = CURLE_SEND_ERROR;
        गच्छ बहिर्गमनम्;
      }
      स्वभाव:
        /* a true error */
        failf(दत्त, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_गन्तव्यम्_str(err), SOCKERRNO);
        परिणाम = CURLE_SEND_ERROR;
        गच्छ बहिर्गमनम्;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(दत्त, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}
#यदि_अन्तः /* HAVE_OPENSSL_EARLYDATA */

स्थिर CURLcode ossl_connect(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             bool *समाप्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;

  /* check if the connection has already been established */
  यदि(ssl_connection_complete == connssl->state) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  यदि(connssl->connecting_state == ssl_connect_1) {
    यदि(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(दत्त, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(दत्त, cf, "need HTTPS-RR, delaying connect");
      प्रत्यागम CURLE_OK;
    }
    CURL_TRC_CF(दत्त, cf, "ossl_connect, step1");
    परिणाम = ossl_connect_step1(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(दत्त, cf, "ossl_connect, step2");
#यदि_परिभाषितम् HAVE_OPENSSL_EARLYDATA
    यदि(connssl->earlydata_state == ssl_earlydata_await) {
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(connssl->earlydata_state == ssl_earlydata_sending) {
      परिणाम = ossl_send_earlydata(cf, दत्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      connssl->earlydata_state = ssl_earlydata_प्रेषितम्;
    }
#यदि_अन्तः
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_प्रेषितम्));

    परिणाम = ossl_connect_step2(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  यदि(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(दत्त, cf, "ossl_connect, step3");
    परिणाम = ossl_connect_step3(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    connssl->connecting_state = ssl_connect_समाप्तम्;
#यदि_परिभाषितम् HAVE_OPENSSL_EARLYDATA
    यदि(connssl->earlydata_state > ssl_earlydata_none) {
      संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_प्रेषितम्);
      connssl->earlydata_state =
        (SSL_get_early_दत्त_स्थितिः(octx->ssl) == SSL_EARLY_दत्त_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#यदि_अन्तः
  }

  यदि(connssl->connecting_state == ssl_connect_समाप्तम्) {
    CURL_TRC_CF(दत्त, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

बहिर्गमनम्:
  यदि(परिणाम == CURLE_AGAIN) {
    *समाप्तम् = FALSE;
    प्रत्यागम CURLE_OK;
  }
  *समाप्तम् = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  प्रत्यागम परिणाम;
}

स्थिर bool ossl_दत्त_pending(संरचना Curl_cfilter *cf,
                              ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  (शून्य)दत्त;
  प्रत्यागम (bool)connssl->निवेशः_pending;
}

स्थिर CURLcode ossl_send(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *दत्त,
                          ध्रुव शून्य *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  पूर्णाङ्क err;
  वर्ण error_बफर[256];
  sslerr_t sslerror;
  पूर्णाङ्क memlen;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)अधिकतम_पूर्णाङ्कः) ? अधिकतम_पूर्णाङ्कः : (पूर्णाङ्क)len;
  यदि(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    यदि(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  यदि(nwritten > 0) {
    यदि(nwritten > memlen) {
      failf(दत्त, "SSL_write(len=%d) returned %d", memlen, nwritten);
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    *pnwritten = (size_t)nwritten;
  }
  अन्यथा {
    err = SSL_get_error(octx->ssl, nwritten);

    विकल्प(err) {
    पक्ष SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      परिणाम = CURLE_AGAIN;
      गच्छ बहिर्गमनम्;
    पक्ष SSL_ERROR_WANT_WRITE:
      परिणाम = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      गच्छ बहिर्गमनम्;
    पक्ष SSL_ERROR_SYSCALL: {
      पूर्णाङ्क sockerr = SOCKERRNO;

      यदि(octx->io_परिणाम == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        परिणाम = CURLE_AGAIN;
        गच्छ बहिर्गमनम्;
      }
      sslerror = ERR_get_error();
      यदि(sslerror)
        ossl_strerror(sslerror, error_बफर, परिमाण(error_बफर));
      अन्यथा यदि(sockerr)
        curlx_strerror(sockerr, error_बफर, परिमाण(error_बफर));
      अन्यथा
        curl_msnprintf(error_बफर, परिमाण(error_बफर), "%s",
                       SSL_ERROR_गन्तव्यम्_str(err));

      failf(दत्त, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_बफर, sockerr);
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    पक्ष SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(दत्त, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_बफर, परिमाण(error_बफर)));
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    स्वभाव:
      /* a true error */
      failf(दत्त, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_गन्तव्यम्_str(err), SOCKERRNO);
      परिणाम = CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ossl_recv(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *दत्त,   /* transfer */
                          वर्ण *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  वर्ण error_बफर[256];
  अचिह्नित दीर्घ sslerror;
  पूर्णाङ्क buffsize;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)अधिकतम_पूर्णाङ्कः) ? अधिकतम_पूर्णाङ्कः : (पूर्णाङ्क)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  यदि(nread > 0)
    *pnread = (size_t)nread;
  अन्यथा {
    /* failed SSL_read */
    पूर्णाङ्क err = SSL_get_error(octx->ssl, nread);

    विकल्प(err) {
    पक्ष SSL_ERROR_NONE: /* this is not an error */
      भङ्ग;
    पक्ष SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      यदि(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(दत्त, cf, "TLS close_notify");
      भङ्ग;
    पक्ष SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      परिणाम = CURLE_AGAIN;
      गच्छ बहिर्गमनम्;
    पक्ष SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      परिणाम = CURLE_AGAIN;
      गच्छ बहिर्गमनम्;
    स्वभाव:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      यदि(octx->io_परिणाम == CURLE_AGAIN) {
        परिणाम = CURLE_AGAIN;
        गच्छ बहिर्गमनम्;
      }
      sslerror = ERR_get_error();
      यदि((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
           queue */
        पूर्णाङ्क sockerr = SOCKERRNO;
        यदि(sslerror)
          ossl_strerror(sslerror, error_बफर, परिमाण(error_बफर));
        अन्यथा यदि(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_बफर, परिमाण(error_बफर));
        अन्यथा
          curl_msnprintf(error_बफर, परिमाण(error_बफर), "%s",
                         SSL_ERROR_गन्तव्यम्_str(err));
        failf(दत्त, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_बफर, sockerr);
        परिणाम = CURLE_RECV_ERROR;
        गच्छ बहिर्गमनम्;
      }
      अन्यथा यदि(err == SSL_ERROR_SYSCALL) {
        यदि(octx->io_परिणाम) {
          /* logging handling in underlying filter already */
          परिणाम = octx->io_परिणाम;
        }
        अन्यथा यदि(connssl->peer_closed) {
          failf(दत्त, "Connection closed abruptly");
          परिणाम = CURLE_RECV_ERROR;
        }
        अन्यथा {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          पूर्णाङ्क sockerr = SOCKERRNO;
          यदि(sockerr)
            curlx_strerror(sockerr, error_बफर, परिमाण(error_बफर));
          अन्यथा {
            curl_msnprintf(error_बफर, परिमाण(error_बफर),
                           "Connection closed abruptly");
          }
          failf(दत्त, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_बफर, sockerr);
          परिणाम = CURLE_RECV_ERROR;
        }
        गच्छ बहिर्गमनम्;
      }
    }
  }

बहिर्गमनम्:
  यदि((!परिणाम && !*pnread) || (परिणाम == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->निवेशः_pending = FALSE;
  }
  CURL_TRC_CF(दत्त, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (पूर्णाङ्क)परिणाम, *pnread, connssl->निवेशः_pending);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ossl_get_channel_binding(संरचना Curl_easy *दत्त,
                                         int8_t sockindex,
                                         संरचना dynbuf *binding)
{
  X509 *cert;
  पूर्णाङ्क mdnid;
  bool no_digest_acceptable = FALSE;
  ध्रुव EVP_MD *algo_type = रिक्त;
  ध्रुव वर्ण *algo_name = रिक्त;
  अचिह्नित पूर्णाङ्क दीर्घता;
  अचिह्नित वर्ण buf[EVP_MAX_MD_आकार];

  स्थिर ध्रुव वर्ण उपसर्गः[] = "tls-server-end-point:";
  संरचना connectdata *conn = दत्त->conn;
  संरचना Curl_cfilter *cf = conn->cfilter[sockindex];
  संरचना ossl_ctx *octx = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  कुरु {
    ध्रुव संरचना Curl_cftype *cft = cf->cft;
    संरचना ssl_connect_दत्त *connssl = cf->ctx;

    यदि(cft->name && !वाक्यं_तुलय(cft->name, "SSL")) {
      octx = (संरचना ossl_ctx *)connssl->backend;
      भङ्ग;
    }

    cf = cf->अग्र;
  } यावत्(cf);

  यदि(!octx) {
    failf(दत्त, "Failed to find the SSL filter");
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  यदि(!cert)
    /* No server certificate, do not do channel binding */
    प्रत्यागम CURLE_OK;

#यदि_परिभाषितम् HAVE_OPENSSL3
  {
    पूर्णाङ्क pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    यदि(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(दत्त, "certificate signature algorithm not recognized");
      परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
      गच्छ बहिर्गमनम्;
    }

    यदि(mdnid != NID_undef) {
      यदि(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      अन्यथा
        algo_type = EVP_get_digestbynid(mdnid);
    }
    अन्यथा यदि(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    अन्यथा यदि(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      वर्ण mdname[128] = "";
      पूर्णाङ्क rc = EVP_PKEY_get_default_digest_name(pkey, mdname, परिमाण(mdname));
      bool md_is_undef = !वाक्यं_तुलय(mdname, "UNDEF");

      यदि(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      अन्यथा यदि(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(दत्त, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#अन्यथा /* HAVE_OPENSSL3 */

  यदि(!OBJ_find_sigid_algs(X509_get_signature_nid(cert), &mdnid, रिक्त)) {
    failf(दत्त,
          "Unable to find digest NID for certificate signature algorithm");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ बहिर्गमनम्;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  यदि(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  अन्यथा {
    algo_type = EVP_get_digestbynid(mdnid);
    यदि(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(दत्त, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
      गच्छ बहिर्गमनम्;
    }
  }

#यदि_अन्तः /* HAVE_OPENSSL3, else */

  यदि(!algo_type) {
    यदि(no_digest_acceptable) {
      infof(दत्त, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      परिणाम = CURLE_OK;
      गच्छ बहिर्गमनम्;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(दत्त, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ बहिर्गमनम्;
  }

  यदि(!X509_digest(cert, algo_type, buf, &दीर्घता)) {
    failf(दत्त, "X509_digest() failed for channel binding");
    परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    गच्छ बहिर्गमनम्;
  }

  /* Append "tls-server-end-point:" */
  परिणाम = curlx_dyn_addn(binding, उपसर्गः, CURL_CSTRLEN(उपसर्गः));
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  /* Append digest */
  परिणाम = curlx_dyn_addn(binding, buf, दीर्घता);

बहिर्गमनम्:
  X509_free(cert);
  प्रत्यागम परिणाम;
}

size_t Curl_ossl_संस्करणम्(वर्ण *बफर, size_t आकार)
{
#यदि_परिभाषितम् LIBRESSL_संस्करणम्_सङ्ख्या
  वर्ण *p;
  size_t गणनम्;
  ध्रुव वर्ण *ver = Open_SSL_संस्करणम्(OPENSSL_संस्करणम्);
  स्थिर ध्रुव वर्ण expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  यदि(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  गणनम् = curl_msnprintf(बफर, आकार, "%s/%s", OSSL_PACKAGE, ver);
  प्रति(p = बफर; *p; ++p) {
    यदि(ISBLANK(*p))
      *p = '_';
  }
  प्रत्यागम गणनम्;
#नोचेद्यदि defined(OPENSSL_IS_AWSLC)
  प्रत्यागम curl_msnprintf(बफर, आकार, "%s/%s",
                        OSSL_PACKAGE, AWSLC_संस्करणम्_सङ्ख्या_STRING);
#नोचेद्यदि defined(OPENSSL_IS_BORINGSSL)
#यदि_परिभाषितम् CURL_BORINGSSL_संस्करणम्
  प्रत्यागम curl_msnprintf(बफर, आकार, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_संस्करणम्);
#अन्यथा
  प्रत्यागम curl_msnprintf(बफर, आकार, OSSL_PACKAGE);
#यदि_अन्तः
#अन्यथा /* OpenSSL 3+ */
  प्रत्यागम curl_msnprintf(बफर, आकार, "%s/%s",
                        OSSL_PACKAGE, Open_SSL_संस्करणम्(OPENSSL_संस्करणम्_STRING));
#यदि_अन्तः
}

/* can be called with data == NULL */
स्थिर CURLcode ossl_random(संरचना Curl_easy *दत्त,
                            अचिह्नित वर्ण *entropy, size_t दीर्घता)
{
  पूर्णाङ्क rc;
  यदि(दत्त) {
    यदि(ossl_बीजम्(दत्त)) /* Initiate the seed if not already done */
      प्रत्यागम CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  अन्यथा {
    यदि(!rand_enough())
      प्रत्यागम CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_अष्टकानि(entropy, (ossl_valsize_t)curlx_uztosi(दीर्घता));
  प्रत्यागम rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

स्थिर CURLcode ossl_sha256sum(ध्रुव अचिह्नित वर्ण *निवेशः,
                               size_t len,
                               अचिह्नित वर्ण *sha256sum /* output */,
                               size_t अप्रयुक्तम्)
{
  CURLcode परिणाम = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (शून्य)अप्रयुक्तम्;

  mdctx = EVP_MD_CTX_new();
  यदि(!mdctx)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  यदि(!EVP_DigestInit_ex(mdctx, EVP_sha256(), रिक्त)) {
    परिणाम = CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }
  यदि(!EVP_DigestUpdate(mdctx, निवेशः, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, रिक्त))
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
बहिर्गमनम्:
  EVP_MD_CTX_free(mdctx);
  प्रत्यागम परिणाम;
}

स्थिर bool ossl_cert_स्थितिः_request(शून्य)
{
#यदि_अपरिभाषितम् OPENSSL_NO_OCSP
  प्रत्यागम TRUE;
#अन्यथा
  प्रत्यागम FALSE;
#यदि_अन्तः
}

स्थिर शून्य *ossl_get_internals(संरचना ssl_connect_दत्त *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  संरचना ossl_ctx *octx = (संरचना ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  प्रत्यागम info == CURLINFO_TLS_SESSION ?
    (शून्य *)octx->ssl_ctx : (शून्य *)octx->ssl;
}

ध्रुव संरचना Curl_ssl Curl_ssl_openssl = {
  { CURLSSLBACKEND_OPENSSL, "openssl" }, /* info */

  SSLSUPP_CA_मार्गः |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_SSL_CTX |
#यदि_परिभाषितम् HAVE_SSL_CTX_समुच्चयः_CIPHERSUITES
  SSLSUPP_TLS13_CIPHERSUITES |
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SSL_CTX_SET1_SIGALGS
  SSLSUPP_SIGNATURE_ALGORITHMS |
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SSL_SET1_ECH_CONFIG_LIST
  SSLSUPP_ECH |
#यदि_अन्तः
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_ISSUERCERT_BLOB |
  SSLSUPP_SSL_EC_CURVES |
  SSLSUPP_CRLFILE,

  परिमाण(संरचना ossl_ctx),

  ossl_init,                /* init */
  ossl_cleanup,             /* cleanup */
  Curl_ossl_संस्करणम्,        /*संस्करणम्*/
  ossl_shutdown,            /* shutdown */
  ossl_दत्त_pending,        /* data_pending */
  ossl_random,              /* random */
  ossl_cert_स्थितिः_request, /* cert_status_request */
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

#यदि_अन्तः /* USE_OPENSSL */
