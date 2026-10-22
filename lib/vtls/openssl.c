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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_OPENSSL

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "formdata.h" /* for the boundary function */
#అన్తర్భావ "url.h" /* for the SSL config check function */
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "vtls/openssl.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_int.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "vtls/keylog.h"
#అన్తర్భావ "vtls/hostcheck.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "multiif.h"
#అన్తర్భావ "curlx/strerr.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curlx/strcopy.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ "vtls/apple.h"
#నిర్వచితమైతే USE_ECH
#అన్తర్భావ "curlx/base64.h"
#అయితే_ముగింపు

#అన్తర్భావ <openssl/rand.h>
#అన్తర్భావ <openssl/x509v3.h>
#నిర్వచితం_కాకపోతే OPENSSL_NO_DSA
#అన్తర్భావ <openssl/dsa.h>
#అయితే_ముగింపు
#అన్తర్భావ <openssl/dh.h>
#అన్తర్భావ <openssl/err.h>
#అన్తర్భావ <openssl/conf.h>
#అన్తర్భావ <openssl/bn.h>
#అన్తర్భావ <openssl/rsa.h>
#అన్తర్భావ <openssl/bio.h>
#అన్తర్భావ <openssl/pkcs12.h>
#అన్తర్భావ <openssl/tls1.h>
#అన్తర్భావ <openssl/evp.h>

#ఒకవేళ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
#అన్తర్భావ <openssl/ech.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే OPENSSL_NO_OCSP
#అన్తర్భావ <openssl/ocsp.h>
#అయితే_ముగింపు

#ఒకవేళ !defined(OPENSSL_NO_ENGINE) && !defined(OPENSSL_NO_UI_CONSOLE)
#నిర్వచించు USE_OPENSSL_ENGINE
#అన్తర్భావ <openssl/engine.h>
#అయితే_ముగింపు

#నిర్వచితమైతే LIBRESSL_సంస్కరణ_సంఖ్య
/* As of LibreSSL 2.0.0-4.0.0: OPENSSL_VERSION_NUMBER == 0x20000000L */
#  ఒకవేళ LIBRESSL_సంస్కరణ_సంఖ్య < 0x3040100fL /* 2021-10-14 */
#    error "LibreSSL 3.4.1 or greater required"
#  endif
#లేదావేళ !defined(HAVE_BORINGSSL_LIKE)
#  ifndef HAVE_OPENSSL3 /* 2021-09-07 */
#    error "OpenSSL 3.0.0 or greater required"
#  endif
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_OPENSSL3) && !defined(OPENSSL_NO_UI_CONSOLE)
#అన్తర్భావ <openssl/provider.h>
#అన్తర్భావ <openssl/store.h>
/* this is used in the following conditions to make them easier to read */
#నిర్వచించు OPENSSL_HAS_PROVIDERS
#అయితే_ముగింపు

/* AWS-LC fixed a bug with large buffers in v1.61.0 which also introduced
 * X509_V_ERR_EC_KEY_EXPLICIT_PARAMS. */
#ఒకవేళ !defined(LIBRESSL_సంస్కరణ_సంఖ్య) && !defined(OPENSSL_IS_BORINGSSL) && \
  (!defined(OPENSSL_IS_AWSLC) || defined(X509_V_ERR_EC_కీలకం_EXPLICIT_PARAMS))
#నిర్వచించు HAVE_SSL_CTX_సమితి_DEFAULT_READ_బఫరు_LEN 1
#అయితే_ముగింపు

#ఒకవేళ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
#అన్తర్భావ <openssl/ui.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_OPENSSL3
#నిర్వచించు HAVE_EVP_PKEY_GET_PARAMS 1
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_EVP_PKEY_GET_PARAMS
#అన్తర్భావ <openssl/core_names.h>
#నిర్వచించు DECLARE_PKEY_PARAM_BIGNUM(name) BIGNUM *name = ఖాళీ
#నిర్వచించు FREE_PKEY_PARAM_BIGNUM(name) BN_clear_free(name)
#లేకపోతే
#నిర్వచించు DECLARE_PKEY_PARAM_BIGNUM(name) స్థిరరాశి BIGNUM *name
#నిర్వచించు FREE_PKEY_PARAM_BIGNUM(name)
#అయితే_ముగింపు

/* Whether SSL_CTX_set_ciphersuites is available.
 * BoringSSL: no
 * LibreSSL: supported since 3.4.1 (released 2021-10-14)
 * OpenSSL: supported since 1.1.1 (commit a53b5be6a05)
 */
#నిర్వచితం_కాకపోతే OPENSSL_IS_BORINGSSL
#  define HAVE_SSL_CTX_సమితి_CIPHERSUITES
#  ifndef OPENSSL_IS_AWSLC
#    define HAVE_SSL_CTX_సమితి_POST_HANDSHAKE_AUTH
#  endif
#అయితే_ముగింపు

/* Whether SSL_CTX_set1_sigalgs_list is available
 * BoringSSL: supported since 0.20240913.0 (commit 826ce15)
 * LibreSSL: no
 * OpenSSL: supported since 1.0.2 (commit 0b362de5f575)
 */
#నిర్వచితం_కాకపోతే LIBRESSL_సంస్కరణ_సంఖ్య
#నిర్వచించు HAVE_SSL_CTX_SET1_SIGALGS
#అయితే_ముగింపు

#నిర్వచితమైతే LIBRESSL_సంస్కరణ_సంఖ్య
#నిర్వచించు OSSL_PACKAGE "LibreSSL"
#లేదావేళ defined(OPENSSL_IS_AWSLC)
#నిర్వచించు OSSL_PACKAGE "AWS-LC"
#లేదావేళ defined(OPENSSL_IS_BORINGSSL)
#నిర్వచించు OSSL_PACKAGE "BoringSSL"
#లేదావేళ defined(USE_NGTCP2) && defined(USE_NGHTTP3) && \
  !defined(OPENSSL_QUIC_API2)
#నిర్వచించు OSSL_PACKAGE "quictls"
#లేకపోతే
#నిర్వచించు OSSL_PACKAGE "OpenSSL"
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_BORINGSSL_LIKE
రకనిర్వచనం size_t numcert_t;
రకనిర్వచనం uint32_t sslerr_t;
#లేకపోతే
రకనిర్వచనం పూర్ణాంకం numcert_t;
రకనిర్వచనం గుర్తులేని దీర్ఘ sslerr_t;
#అయితే_ముగింపు
#నిర్వచించు ossl_valsize_t numcert_t

#ఒకవేళ OPENSSL_సంస్కరణ_సంఖ్య >= 0x40100000L
#నిర్వచించు ossl_ASN1_STRING_get_నిడివి ASN1_STRING_get_నిడివి
#లేకపోతే
స్థిర size_t ossl_ASN1_STRING_get_నిడివి(స్థిరరాశి ASN1_STRING *str)
{
   స్థిరరాశి పూర్ణాంకం నిడివి = ASN1_STRING_నిడివి(str);
   తిరిగి నిడివి >= 0 ? (size_t)నిడివి : 0;
}
#అయితే_ముగింపు

స్థిర CURLcode నెట్టు_certinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                              BIO *mem, స్థిరరాశి అక్షరం *label, పూర్ణాంకం అంకె)
  WARN_వాడని_ఫలితం;

స్థిర CURLcode నెట్టు_certinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                              BIO *mem, స్థిరరాశి అక్షరం *label, పూర్ణాంకం అంకె)
{
  అక్షరం *ptr;
  దీర్ఘ len = BIO_get_mem_దత్తాంశం(mem, &ptr);
  CURLcode ఫలితం = Curl_ssl_నెట్టు_certinfo_len(దత్తాంశం, అంకె, label, ptr, len);
  (శూన్య)BIO_reset(mem);
  తిరిగి ఫలితం;
}

స్థిర CURLcode pubkey_show(నిర్మాణం Curl_easy *దత్తాంశం,
                            BIO *mem,
                            పూర్ణాంకం అంకె,
                            స్థిరరాశి అక్షరం *type,
                            స్థిరరాశి అక్షరం *name,
                            స్థిరరాశి BIGNUM *bn) WARN_వాడని_ఫలితం;

స్థిర CURLcode pubkey_show(నిర్మాణం Curl_easy *దత్తాంశం,
                            BIO *mem,
                            పూర్ణాంకం అంకె,
                            స్థిరరాశి అక్షరం *type,
                            స్థిరరాశి అక్షరం *name,
                            స్థిరరాశి BIGNUM *bn)
{
  అక్షరం namebuf[32];

  curl_msnprintf(namebuf, పరిమాణం(namebuf), "%s(%s)", type, name);

  ఒకవేళ(bn)
    BN_print(mem, bn);
  తిరిగి నెట్టు_certinfo(దత్తాంశం, mem, namebuf, అంకె);
}

#నిర్వచించు print_pubkey_BN(_type, _name, అంకె_2)           \
  pubkey_show(దత్తాంశం, mem, అంకె_2, #_type, #_name, _name)

స్థిర పూర్ణాంకం asn1_object_dump(స్థిరరాశి ASN1_OBJECT *a, అక్షరం *buf, size_t len)
{
  పూర్ణాంకం i = i2t_ASN1_OBJECT(buf, (పూర్ణాంకం)len, a);
  తిరిగి (i >= (పూర్ణాంకం)len);  /* buffer too small */
}

స్థిర CURLcode X509V3_ext(నిర్మాణం Curl_easy *దత్తాంశం,
                           పూర్ణాంకం certnum,
                           స్థిరరాశి దొంతర_యొక్క(X509_EXTENSION) *extsarg)
{
  పూర్ణాంకం i;
  CURLcode ఫలితం = CURLE_OK;
#నిర్వచితమైతే LIBRESSL_సంస్కరణ_సంఖ్య
  దొంతర_యొక్క(X509_EXTENSION) *exts = CURL_UNCONST(extsarg);
#లేకపోతే
  స్థిరరాశి దొంతర_యొక్క(X509_EXTENSION) *exts = extsarg;
#అయితే_ముగింపు

  ఒకవేళ((పూర్ణాంకం)sk_X509_EXTENSION_అంకె(exts) <= 0)
    /* no extensions, bail out */
    తిరిగి ఫలితం;

  కొరకు(i = 0; i < (పూర్ణాంకం)sk_X509_EXTENSION_అంకె(exts); i++) {
    స్థిరరాశి ASN1_OBJECT *obj;
    X509_EXTENSION *ext = sk_X509_EXTENSION_విలువ(exts, (ossl_valsize_t)i);
    BUF_MEM *biomem;
    అక్షరం namebuf[128];
    BIO *bio_బహిర్గతం = BIO_new(BIO_s_mem());

    ఒకవేళ(!bio_బహిర్గతం)
      తిరిగి ఫలితం;

    obj = X509_EXTENSION_get_object(ext);

    ఒకవేళ(asn1_object_dump(obj, namebuf, పరిమాణం(namebuf)))
      /* make sure the name is null-terminated */
      namebuf[CURL_CSTRLEN(namebuf)] = 0;

    ఒకవేళ(!X509V3_EXT_print(bio_బహిర్గతం, ext, 0, 0))
      ASN1_STRING_print(bio_బహిర్గతం,
                        (స్థిరరాశి ASN1_STRING *)X509_EXTENSION_get_దత్తాంశం(ext));

    BIO_get_mem_ptr(bio_బహిర్గతం, &biomem);
    ఫలితం = Curl_ssl_నెట్టు_certinfo_len(దత్తాంశం, certnum, namebuf, biomem->దత్తాంశం,
                                        biomem->నిడివి);
    BIO_free(bio_బహిర్గతం);
    ఒకవేళ(ఫలితం)
      ఆపు;
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode get_pkey_rsa(నిర్మాణం Curl_easy *దత్తాంశం,
                             EVP_PKEY *pubkey, BIO *mem, పూర్ణాంకం i)
{
  CURLcode ఫలితం = CURLE_OK;
#నిర్వచితం_కాకపోతే HAVE_EVP_PKEY_GET_PARAMS
  RSA *rsa = EVP_PKEY_get0_RSA(pubkey);
#అయితే_ముగింపు /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(n);
  DECLARE_PKEY_PARAM_BIGNUM(e);
#నిర్వచితమైతే HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_N, &n);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_RSA_E, &e);
#లేకపోతే
  RSA_get0_కీలకం(rsa, &n, &e, ఖాళీ);
#అయితే_ముగింపు /* HAVE_EVP_PKEY_GET_PARAMS */
  BIO_printf(mem, "%d", (పూర్ణాంకం)(n ? BN_అంకె_బిట్లు(n) : 0));
  ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "RSA Public Key", i);
  ఒకవేళ(!ఫలితం) {
    ఫలితం = print_pubkey_BN(rsa, n, i);
    ఒకవేళ(!ఫలితం)
      ఫలితం = print_pubkey_BN(rsa, e, i);
  }
  FREE_PKEY_PARAM_BIGNUM(n);
  FREE_PKEY_PARAM_BIGNUM(e);
  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే OPENSSL_NO_DSA
స్థిర CURLcode get_pkey_dsa(నిర్మాణం Curl_easy *దత్తాంశం,
                             EVP_PKEY *pubkey, BIO *mem, పూర్ణాంకం i)
{
  CURLcode ఫలితం = CURLE_OK;
#నిర్వచితం_కాకపోతే HAVE_EVP_PKEY_GET_PARAMS
  DSA *dsa = EVP_PKEY_get0_DSA(pubkey);
#అయితే_ముగింపు /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_కీలకం);
#నిర్వచితమైతే HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_కీలకం, &pub_కీలకం);
#లేకపోతే
  DSA_get0_pqg(dsa, &p, &q, &g);
  DSA_get0_కీలకం(dsa, &pub_కీలకం, ఖాళీ);
#అయితే_ముగింపు /* HAVE_EVP_PKEY_GET_PARAMS */
  ఫలితం = print_pubkey_BN(dsa, p, i);
  ఒకవేళ(!ఫలితం)
    ఫలితం = print_pubkey_BN(dsa, q, i);
  ఒకవేళ(!ఫలితం)
    ఫలితం = print_pubkey_BN(dsa, g, i);
  ఒకవేళ(!ఫలితం)
    ఫలితం = print_pubkey_BN(dsa, pub_కీలకం, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_కీలకం);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* !OPENSSL_NO_DSA */

స్థిర CURLcode get_pkey_dh(నిర్మాణం Curl_easy *దత్తాంశం,
                            EVP_PKEY *pubkey, BIO *mem, పూర్ణాంకం i)
{
  CURLcode ఫలితం;
#నిర్వచితం_కాకపోతే HAVE_EVP_PKEY_GET_PARAMS
  DH *dh = EVP_PKEY_get0_DH(pubkey);
#అయితే_ముగింపు /* !HAVE_EVP_PKEY_GET_PARAMS */
  DECLARE_PKEY_PARAM_BIGNUM(p);
  DECLARE_PKEY_PARAM_BIGNUM(q);
  DECLARE_PKEY_PARAM_BIGNUM(g);
  DECLARE_PKEY_PARAM_BIGNUM(pub_కీలకం);
#నిర్వచితమైతే HAVE_EVP_PKEY_GET_PARAMS
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_P, &p);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_Q, &q);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_FFC_G, &g);
  EVP_PKEY_get_bn_param(pubkey, OSSL_PKEY_PARAM_PUB_కీలకం, &pub_కీలకం);
#లేకపోతే
  DH_get0_pqg(dh, &p, &q, &g);
  DH_get0_కీలకం(dh, &pub_కీలకం, ఖాళీ);
#అయితే_ముగింపు /* HAVE_EVP_PKEY_GET_PARAMS */
  ఫలితం = print_pubkey_BN(dh, p, i);
  ఒకవేళ(!ఫలితం)
    ఫలితం = print_pubkey_BN(dh, q, i);
  ఒకవేళ(!ఫలితం)
    ఫలితం = print_pubkey_BN(dh, g, i);
  ఒకవేళ(!ఫలితం)
    ఫలితం = print_pubkey_BN(dh, pub_కీలకం, i);
  FREE_PKEY_PARAM_BIGNUM(p);
  FREE_PKEY_PARAM_BIGNUM(q);
  FREE_PKEY_PARAM_BIGNUM(g);
  FREE_PKEY_PARAM_BIGNUM(pub_కీలకం);
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే HAVE_OPENSSL3
/* from OpenSSL commit fc756e594ed5a27af378 */
రకనిర్వచనం స్థిరరాశి X509_PUBKEY pubkeytype_t;
#లేకపోతే
రకనిర్వచనం X509_PUBKEY pubkeytype_t;
#అయితే_ముగింపు

స్థిర CURLcode ossl_certchain(నిర్మాణం Curl_easy *దత్తాంశం, SSL *ssl)
{
  CURLcode ఫలితం;
  దొంతర_యొక్క(X509) *sk;
  పూర్ణాంకం i;
  numcert_t numcerts;
  BIO *mem;

  DEBUGASSERT(ssl);

  sk = SSL_get_peer_cert_chain(ssl);
  ఒకవేళ(!sk)
    తిరిగి CURLE_SSL_CONNECT_ERROR;

  numcerts = sk_X509_అంకె(sk);
  ఒకవేళ(numcerts > MAX_ALLOWED_CERT_AMOUNT) {
    failf(దత్తాంశం, "%d certificates is more than allowed (%d)", (పూర్ణాంకం)numcerts,
          MAX_ALLOWED_CERT_AMOUNT);
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  ఫలితం = Curl_ssl_init_certinfo(దత్తాంశం, (పూర్ణాంకం)numcerts);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  mem = BIO_new(BIO_s_mem());
  ఒకవేళ(!mem)
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  కొరకు(i = 0; !ఫలితం && (i < (పూర్ణాంకం)numcerts); i++) {
    X509 *x = sk_X509_విలువ(sk, (ossl_valsize_t)i);
    EVP_PKEY *pubkey = ఖాళీ;
    size_t j;
    స్థిరరాశి ASN1_బిట్టు_STRING *psig = ఖాళీ;

    X509_NAME_print_ex(mem, X509_get_subject_name(x), 0, XN_FLAG_ONELINE);
    ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Subject", i);
    ఒకవేళ(ఫలితం)
      ఆపు;

    X509_NAME_print_ex(mem, X509_get_issuer_name(x), 0, XN_FLAG_ONELINE);
    ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Issuer", i);
    ఒకవేళ(ఫలితం)
      ఆపు;

    BIO_printf(mem, "%lx", (గుర్తులేని దీర్ఘ)X509_get_సంస్కరణ(x));
    ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Version", i);
    ఒకవేళ(ఫలితం)
      ఆపు;

    {
      ASN1_INTEGER *అంకె = X509_get_serial_సంఖ్య(x);
      స్థిరరాశి గుర్తులేని అక్షరం *numdata = ASN1_STRING_get0_దత్తాంశం(అంకె);
      స్థిరరాశి size_t numlen = ossl_ASN1_STRING_get_నిడివి(అంకె);
      ఒకవేళ(ASN1_STRING_type(అంకె) == V_ASN1_NEG_INTEGER)
        BIO_puts(mem, "-");
      కొరకు(j = 0; j < numlen; j++)
        BIO_printf(mem, "%02x", numdata[j]);
    }
    ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Serial Number", i);
    ఒకవేళ(ఫలితం)
      ఆపు;

    {
      స్థిరరాశి X509_ALGOR *sigalg = ఖాళీ;
      pubkeytype_t *xpubkey = ఖాళీ;
      ASN1_OBJECT *pubkeyoid = ఖాళీ;

      X509_get0_signature(&psig, &sigalg, x);
      ఒకవేళ(sigalg) {
        స్థిరరాశి ASN1_OBJECT *sigalgoid = ఖాళీ;
        X509_ALGOR_get0(&sigalgoid, ఖాళీ, ఖాళీ, sigalg);
        i2a_ASN1_OBJECT(mem, sigalgoid);
        ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Signature Algorithm", i);
        ఒకవేళ(ఫలితం)
          ఆపు;
      }

      xpubkey = X509_get_X509_PUBKEY(x);
      ఒకవేళ(xpubkey) {
        X509_PUBKEY_get0_param(&pubkeyoid, ఖాళీ, ఖాళీ, ఖాళీ, xpubkey);
        ఒకవేళ(pubkeyoid) {
          i2a_ASN1_OBJECT(mem, pubkeyoid);
          ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Public Key Algorithm", i);
          ఒకవేళ(ఫలితం)
            ఆపు;
        }
      }

      ఫలితం = X509V3_ext(దత్తాంశం, i, X509_get0_extensions(x));
      ఒకవేళ(ఫలితం)
        ఆపు;
    }

    ASN1_సమయం_print(mem, X509_get0_notBefore(x));
    ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Start date", i);
    ఒకవేళ(ఫలితం)
      ఆపు;

    ASN1_సమయం_print(mem, X509_get0_notAfter(x));
    ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Expire date", i);
    ఒకవేళ(ఫలితం)
      ఆపు;

    pubkey = X509_get_pubkey(x);
    ఒకవేళ(!pubkey)
      infof(దత్తాంశం, "   Unable to load public key");
    లేకపోతే {
      ఎంపిక(EVP_PKEY_id(pubkey)) {
      స్థితి EVP_PKEY_RSA:
        ఫలితం = get_pkey_rsa(దత్తాంశం, pubkey, mem, i);
        ఆపు;

#నిర్వచితం_కాకపోతే OPENSSL_NO_DSA
      స్థితి EVP_PKEY_DSA:
        ఫలితం = get_pkey_dsa(దత్తాంశం, pubkey, mem, i);
        ఆపు;
#అయితే_ముగింపు

      స్థితి EVP_PKEY_DH:
        ఫలితం = get_pkey_dh(దత్తాంశం, pubkey, mem, i);
        ఆపు;
      }
      EVP_PKEY_free(pubkey);
    }

    ఒకవేళ(!ఫలితం && psig) {
      స్థిరరాశి గుర్తులేని అక్షరం *psigdata = ASN1_STRING_get0_దత్తాంశం(psig);
      స్థిరరాశి size_t psiglen = ossl_ASN1_STRING_get_నిడివి(psig);
      కొరకు(j = 0; j < psiglen; j++)
        BIO_printf(mem, "%02x:", psigdata[j]);
      ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Signature", i);
    }

    ఒకవేళ(!ఫలితం) {
      PEM_write_bio_X509(mem, x);
      ఫలితం = నెట్టు_certinfo(దత్తాంశం, mem, "Cert", i);
    }
  }

  BIO_free(mem);

  ఒకవేళ(ఫలితం)
    /* cleanup all leftovers */
    Curl_ssl_free_certinfo(దత్తాంశం);

  తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం ossl_bio_cf_create(BIO *bio)
{
  BIO_సమితి_shutdown(bio, 1);
  BIO_సమితి_init(bio, 1);
  BIO_సమితి_దత్తాంశం(bio, ఖాళీ);
  తిరిగి 1;
}

స్థిర పూర్ణాంకం ossl_bio_cf_destroy(BIO *bio)
{
  ఒకవేళ(!bio)
    తిరిగి 0;
  తిరిగి 1;
}

స్థిర దీర్ఘ ossl_bio_cf_ctrl(BIO *bio, పూర్ణాంకం cmd, దీర్ఘ అంకె, శూన్య *ptr)
{
  నిర్మాణం Curl_cfilter *cf = BIO_get_దత్తాంశం(bio);
  దీర్ఘ ret = 1;

  (శూన్య)cf;
  (శూన్య)ptr;
  ఎంపిక(cmd) {
  స్థితి BIO_CTRL_GET_CLOSE:
    ret = (దీర్ఘ)BIO_get_shutdown(bio);
    ఆపు;
  స్థితి BIO_CTRL_సమితి_CLOSE:
    BIO_సమితి_shutdown(bio, (పూర్ణాంకం)అంకె);
    ఆపు;
  స్థితి BIO_CTRL_FLUSH:
    /* we do no delayed writes, but if we ever would, this
     * needs to trigger it. */
    ret = 1;
    ఆపు;
  స్థితి BIO_CTRL_DUP:
    ret = 1;
    ఆపు;
  స్థితి BIO_CTRL_EOF: {
    /* EOF has been reached on input? */
    నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
    తిరిగి connssl->peer_closed;
  }
  అప్రమేయ:
    ret = 0;
    ఆపు;
  }
  తిరిగి ret;
}

స్థిర పూర్ణాంకం ossl_bio_cf_బహిర్గతం_write(BIO *bio, స్థిరరాశి అక్షరం *buf, పూర్ణాంకం blen)
{
  నిర్మాణం Curl_cfilter *cf = BIO_get_దత్తాంశం(bio);
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  size_t nwritten;
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(blen < 0)
    తిరిగి 0;

  ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం,
                             (స్థిరరాశి uint8_t *)buf, (size_t)blen, FALSE,
                             &nwritten);
  CURL_TRC_CF(దత్తాంశం, cf, "ossl_bio_cf_out_write(len=%d) -> %d, %zu",
              blen, (పూర్ణాంకం)ఫలితం, nwritten);
  BIO_clear_retry_flags(bio);
  octx->io_ఫలితం = ఫలితం;
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      BIO_సమితి_retry_write(bio);
    తిరిగి -1;
  }
  తిరిగి (పూర్ణాంకం)nwritten;
}

స్థిర పూర్ణాంకం ossl_bio_cf_లోపల_read(BIO *bio, అక్షరం *buf, పూర్ణాంకం blen)
{
  నిర్మాణం Curl_cfilter *cf = BIO_get_దత్తాంశం(bio);
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  size_t nread;
  CURLcode ఫలితం, r2;

  DEBUGASSERT(దత్తాంశం);
  /* OpenSSL catches this case, so should we. */
  ఒకవేళ(!buf)
    తిరిగి 0;
  ఒకవేళ(blen < 0)
    తిరిగి 0;

  ఫలితం = Curl_conn_cf_recv(cf->తదుపరి, దత్తాంశం, buf, (size_t)blen, &nread);
  CURL_TRC_CF(దత్తాంశం, cf, "ossl_bio_cf_in_read(len=%d) -> %d, %zu",
              blen, (పూర్ణాంకం)ఫలితం, nread);
  BIO_clear_retry_flags(bio);
  octx->io_ఫలితం = ఫలితం;
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      BIO_సమితి_retry_read(bio);
  }
  లేకపోతే {
    /* feeding data to OpenSSL means SSL_read() might succeed */
    connssl->ఆదానం_pending = TRUE;
    ఒకవేళ(nread == 0)
      connssl->peer_closed = TRUE;
  }

  /* Before returning server replies to the SSL instance, we need
   * to have setup the x509 store or verification fails. */
  ఒకవేళ(!octx->x509_store_అమరిక) {
    r2 = Curl_ssl_అమరిక_x509_store(cf, దత్తాంశం, octx);
    ఒకవేళ(r2) {
      BIO_clear_retry_flags(bio);
      octx->io_ఫలితం = r2;
      తిరిగి -1;
    }
    octx->x509_store_అమరిక = TRUE;
  }
  తిరిగి ఫలితం ? -1 : (పూర్ణాంకం)nread;
}

స్థిర BIO_పద్ధతి *ossl_bio_cf_పద్ధతి_create(శూన్య)
{
  BIO_పద్ధతి *m = BIO_meth_new(BIO_TYPE_MEM, "OpenSSL CF BIO");
  ఒకవేళ(m) {
    BIO_meth_సమితి_write(m, &ossl_bio_cf_బహిర్గతం_write);
    BIO_meth_సమితి_read(m, &ossl_bio_cf_లోపల_read);
    BIO_meth_సమితి_ctrl(m, &ossl_bio_cf_ctrl);
    BIO_meth_సమితి_create(m, &ossl_bio_cf_create);
    BIO_meth_సమితి_destroy(m, &ossl_bio_cf_destroy);
  }
  తిరిగి m;
}

స్థిర శూన్య ossl_bio_cf_పద్ధతి_free(BIO_పద్ధతి *m)
{
  ఒకవేళ(m)
    BIO_meth_free(m);
}

#నిర్వచితం_కాకపోతే HAVE_KEYLOG_UPSTREAM
#నిర్వచితమైతే HAVE_KEYLOG_CALLBACK
స్థిర శూన్య ossl_keylog_callback(స్థిరరాశి SSL *ssl, స్థిరరాశి అక్షరం *పంక్తి)
{
  (శూన్య)ssl;

  Curl_tls_keylog_write_పంక్తి(పంక్తి);
}
#లేకపోతే
/*
 * ossl_log_tls12_secret is called by libcurl to make the CLIENT_RANDOMs if the
 * OpenSSL being used does not have native support for doing that.
 */
స్థిర శూన్య ossl_log_tls12_secret(స్థిరరాశి SSL *ssl, bool *keylog_పూర్తి)
{
  స్థిరరాశి SSL_SESSION *session;
  గుర్తులేని అక్షరం client_random[SSL3_RANDOM_కొలత];
  గుర్తులేని అక్షరం master_కీలకం[SSL_MAX_MASTER_కీలకం_నిడివి];
  పూర్ణాంకం master_కీలకం_నిడివి = 0;

  ERR_సమితి_mark();

  session = SSL_get_session(ssl);

  ఒకవేళ(!session || *keylog_పూర్తి) {
    ERR_తీయి_గమ్యం_mark();
    తిరిగి;
  }

  SSL_get_client_random(ssl, client_random, SSL3_RANDOM_కొలత);
  master_కీలకం_నిడివి = (పూర్ణాంకం)
    SSL_SESSION_get_master_కీలకం(session, master_కీలకం, SSL_MAX_MASTER_కీలకం_నిడివి);

  ERR_తీయి_గమ్యం_mark();

  /* The handshake has not progressed sufficiently yet, or this is a TLS 1.3
   * session (when curl was built with older OpenSSL headers and running with
   * newer OpenSSL runtime libraries). */
  ఒకవేళ(master_కీలకం_నిడివి <= 0)
    తిరిగి;

  *keylog_పూర్తి = TRUE;
  Curl_tls_keylog_write("CLIENT_RANDOM", client_random,
                        పరిమాణం(client_random),
                        master_కీలకం, master_కీలకం_నిడివి);
}
#అయితే_ముగింపు /* !HAVE_KEYLOG_CALLBACK */
#అయితే_ముగింపు /* HAVE_KEYLOG_UPSTREAM */

స్థిర స్థిరరాశి అక్షరం *SSL_ERROR_గమ్యం_str(పూర్ణాంకం err)
{
  ఎంపిక(err) {
  స్థితి SSL_ERROR_NONE:
    తిరిగి "SSL_ERROR_NONE";
  స్థితి SSL_ERROR_SSL:
    తిరిగి "SSL_ERROR_SSL";
  స్థితి SSL_ERROR_WANT_READ:
    తిరిగి "SSL_ERROR_WANT_READ";
  స్థితి SSL_ERROR_WANT_WRITE:
    తిరిగి "SSL_ERROR_WANT_WRITE";
  స్థితి SSL_ERROR_WANT_X509_LOOKUP:
    తిరిగి "SSL_ERROR_WANT_X509_LOOKUP";
  స్థితి SSL_ERROR_SYSCALL:
    తిరిగి "SSL_ERROR_SYSCALL";
  స్థితి SSL_ERROR_ZERO_RETURN:
    తిరిగి "SSL_ERROR_ZERO_RETURN";
  స్థితి SSL_ERROR_WANT_CONNECT:
    తిరిగి "SSL_ERROR_WANT_CONNECT";
  స్థితి SSL_ERROR_WANT_ACCEPT:
    తిరిగి "SSL_ERROR_WANT_ACCEPT";
#నిర్వచితమైతే SSL_ERROR_WANT_ASYNC  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  స్థితి SSL_ERROR_WANT_ASYNC:
    తిరిగి "SSL_ERROR_WANT_ASYNC";
#అయితే_ముగింపు
#నిర్వచితమైతే SSL_ERROR_WANT_ASYNC_పని  /* OpenSSL 1.1.0+, LibreSSL 3.6.0+ */
  స్థితి SSL_ERROR_WANT_ASYNC_పని:
    తిరిగి "SSL_ERROR_WANT_ASYNC_JOB";
#అయితే_ముగింపు
#నిర్వచితమైతే SSL_ERROR_WANT_CLIENT_HELLO_CB  /* OpenSSL 1.1.1, LibreSSL 3.6.0+ */
  స్థితి SSL_ERROR_WANT_CLIENT_HELLO_CB:
    తిరిగి "SSL_ERROR_WANT_CLIENT_HELLO_CB";
#అయితే_ముగింపు
  అప్రమేయ:
    తిరిగి "SSL_ERROR unknown";
  }
}

/* Return error string for last OpenSSL error
 */
స్థిర అక్షరం *ossl_strerror(గుర్తులేని దీర్ఘ error, అక్షరం *buf, size_t కొలత)
{
  size_t len;
  DEBUGASSERT(కొలత);
  *buf = '\0';

  len = Curl_ossl_సంస్కరణ(buf, కొలత);
  DEBUGASSERT(len < (కొలత - 2));
  ఒకవేళ(len < (కొలత - 2)) {
    buf += len;
    కొలత -= (len + 2);
    *buf++ = ':';
    *buf++ = ' ';
    *buf = '\0';
  }

#నిర్వచితమైతే HAVE_BORINGSSL_LIKE
  ERR_error_string_n((uint32_t)error, buf, కొలత);
#లేకపోతే
  ERR_error_string_n(error, buf, కొలత);
#అయితే_ముగింపు

  ఒకవేళ(!*buf) {
    స్థిరరాశి అక్షరం *సందేశం = error ? "Unknown error" : "No error";
    curlx_strcopy(buf, కొలత, సందేశం, అక్షరశ్రేణి_పొడవు(సందేశం));
  }

  తిరిగి buf;
}

స్థిర పూర్ణాంకం passwd_callback(అక్షరం *buf, పూర్ణాంకం అంకె, పూర్ణాంకం encrypting, శూన్య *password)
{
  DEBUGASSERT(encrypting == 0);

  ఒకవేళ(!encrypting && అంకె >= 0 && password) {
    పూర్ణాంకం klen = curlx_uztosi(అక్షరశ్రేణి_పొడవు((అక్షరం *)password));
    ఒకవేళ(అంకె > klen) {
      జ్ఞాపకం_నకలు(buf, password, klen + 1);
      తిరిగి klen;
    }
  }
  తిరిగి 0;
}

/*
 * rand_enough() returns TRUE if we have seeded the random engine properly.
 */
స్థిర bool rand_enough(శూన్య)
{
  తిరిగి RAND_స్థితి() != 0;
}

స్థిర CURLcode ossl_విత్తనం(నిర్మాణం Curl_easy *దత్తాంశం)
{
  /* This might get called before it has been added to a multi handle */
  ఒకవేళ(దత్తాంశం->multi && దత్తాంశం->multi->ssl_seeded)
    తిరిగి CURLE_OK;

  ఒకవేళ(rand_enough()) {
    /* OpenSSL 1.1.0+ should return here */
    ఒకవేళ(దత్తాంశం->multi)
      దత్తాంశం->multi->ssl_seeded = TRUE;
    తిరిగి CURLE_OK;
  }
  failf(దత్తాంశం, "Insufficient randomness");
  తిరిగి CURLE_SSL_CONNECT_ERROR;
}

#నిర్వచితం_కాకపోతే SSL_FILETYPE_ENGINE
#నిర్వచించు SSL_FILETYPE_ENGINE 42
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SSL_FILETYPE_PKCS12
#నిర్వచించు SSL_FILETYPE_PKCS12 43
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SSL_FILETYPE_PROVIDER
#నిర్వచించు SSL_FILETYPE_PROVIDER 44
#అయితే_ముగింపు
స్థిర పూర్ణాంకం ossl_do_file_type(స్థిరరాశి అక్షరం *type)
{
  ఒకవేళ(!type || !type[0])
    తిరిగి SSL_FILETYPE_PEM;
  ఒకవేళ(curl_strequal(type, "PEM"))
    తిరిగి SSL_FILETYPE_PEM;
  ఒకవేళ(curl_strequal(type, "DER"))
    తిరిగి SSL_FILETYPE_ASN1;
  ఒకవేళ(curl_strequal(type, "PROV"))
    తిరిగి SSL_FILETYPE_PROVIDER;
  ఒకవేళ(curl_strequal(type, "ENG"))
    తిరిగి SSL_FILETYPE_ENGINE;
  ఒకవేళ(curl_strequal(type, "P12"))
    తిరిగి SSL_FILETYPE_PKCS12;
  తిరిగి -1;
}

#ఒకవేళ defined(USE_OPENSSL_ENGINE) || defined(OPENSSL_HAS_PROVIDERS)
/*
 * Supply default password to the engine user interface conversation.
 * The password is passed by OpenSSL engine from ENGINE_load_private_key()
 * last argument to the ui and can be obtained by UI_get0_user_data(ui) here.
 */
స్థిర పూర్ణాంకం ssl_ui_reader(UI *ui, UI_STRING *uis)
{
  స్థిరరాశి అక్షరం *password;
  ఎంపిక(UI_get_string_type(uis)) {
  స్థితి UIT_PROMPT:
  స్థితి UIT_VERIFY:
    password = (స్థిరరాశి అక్షరం *)UI_get0_user_దత్తాంశం(ui);
    ఒకవేళ(password && (UI_get_ఆదానం_flags(uis) & UI_ఆదానం_FLAG_DEFAULT_PWD)) {
      UI_సమితి_ఫలితం(ui, uis, password);
      తిరిగి 1;
    }
    FALLTHROUGH();
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి UI_పద్ధతి_get_reader(UI_OpenSSL())(ui, uis);
}

/*
 * Suppress interactive request for a default password if available.
 */
స్థిర పూర్ణాంకం ssl_ui_writer(UI *ui, UI_STRING *uis)
{
  ఎంపిక(UI_get_string_type(uis)) {
  స్థితి UIT_PROMPT:
  స్థితి UIT_VERIFY:
    ఒకవేళ(UI_get0_user_దత్తాంశం(ui) &&
       (UI_get_ఆదానం_flags(uis) & UI_ఆదానం_FLAG_DEFAULT_PWD)) {
      తిరిగి 1;
    }
    FALLTHROUGH();
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి UI_పద్ధతి_get_writer(UI_OpenSSL())(ui, uis);
}

/*
 * Check if a given string is a PKCS#11 URI
 */
స్థిర bool is_pkcs11_uri(స్థిరరాశి అక్షరం *string)
{
  తిరిగి string && curl_strnequal(string, "pkcs11:", 7);
}

#అయితే_ముగింపు

స్థిర CURLcode ossl_సమితి_engine(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ossl_ctx *octx,
                                స్థిరరాశి అక్షరం *name);
#నిర్వచితమైతే OPENSSL_HAS_PROVIDERS
స్థిర CURLcode ossl_సమితి_provider(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ossl_ctx *octx,
                                  స్థిరరాశి అక్షరం *iname);
#అయితే_ముగింపు

స్థిర పూర్ణాంకం use_certificate_blob(SSL_CTX *ctx, స్థిరరాశి నిర్మాణం curl_blob *blob,
                                పూర్ణాంకం type, స్థిరరాశి అక్షరం *కీలకం_passwd)
{
  పూర్ణాంకం ret = 0;
  X509 *x = ఖాళీ;
  /* the typecast of blob->len is fine since it is guaranteed to never be
     larger than CURL_MAX_INPUT_LENGTH */
  BIO *లోపల = BIO_new_mem_buf(blob->దత్తాంశం, (పూర్ణాంకం)blob->len);
  ఒకవేళ(!లోపల)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(type == SSL_FILETYPE_ASN1) {
    /* j = ERR_R_ASN1_LIB; */
    x = d2i_X509_bio(లోపల, ఖాళీ);
  }
  లేకపోతే ఒకవేళ(type == SSL_FILETYPE_PEM) {
    /* ERR_R_PEM_LIB; */
    x = PEM_read_bio_X509(లోపల, ఖాళీ, passwd_callback, CURL_UNCONST(కీలకం_passwd));
  }
  లేకపోతే {
    ret = 0;
    వెళ్ళు చివర;
  }

  ఒకవేళ(!x) {
    ret = 0;
    వెళ్ళు చివర;
  }

  ret = SSL_CTX_use_certificate(ctx, x);
చివర:
  X509_free(x);
  BIO_free(లోపల);
  తిరిగి ret;
}

స్థిర పూర్ణాంకం use_privatekey_blob(SSL_CTX *ctx, స్థిరరాశి నిర్మాణం curl_blob *blob,
                               పూర్ణాంకం type, స్థిరరాశి అక్షరం *కీలకం_passwd)
{
  పూర్ణాంకం ret = 0;
  EVP_PKEY *pkey = ఖాళీ;
  BIO *లోపల = BIO_new_mem_buf(blob->దత్తాంశం, (పూర్ణాంకం)blob->len);
  ఒకవేళ(!లోపల)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(type == SSL_FILETYPE_PEM)
    pkey = PEM_read_bio_వ్యక్తిగత_కీలకం(లోపల, ఖాళీ, passwd_callback,
                                   CURL_UNCONST(కీలకం_passwd));
  లేకపోతే ఒకవేళ(type == SSL_FILETYPE_ASN1)
    pkey = d2i_వ్యక్తిగత_కీలకం_bio(లోపల, ఖాళీ);
  లేకపోతే
    వెళ్ళు చివర;

  ఒకవేళ(!pkey)
    వెళ్ళు చివర;

  ret = SSL_CTX_use_వ్యక్తిగత_కీలకం(ctx, pkey);
  EVP_PKEY_free(pkey);
చివర:
  BIO_free(లోపల);
  తిరిగి ret;
}

స్థిర పూర్ణాంకం use_certificate_chain_blob(SSL_CTX *ctx,
                                      స్థిరరాశి నిర్మాణం curl_blob *blob,
                                      స్థిరరాశి అక్షరం *కీలకం_passwd)
{
  పూర్ణాంకం ret = 0;
  X509 *x = ఖాళీ;
  BIO *లోపల = BIO_new_mem_buf(blob->దత్తాంశం, (పూర్ణాంకం)blob->len);
  ఒకవేళ(!లోపల)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ERR_clear_error();

  x = PEM_read_bio_X509_AUX(లోపల, ఖాళీ,
                            passwd_callback, CURL_UNCONST(కీలకం_passwd));
  ఒకవేళ(!x)
    వెళ్ళు చివర;

  ret = SSL_CTX_use_certificate(ctx, x);

  ఒకవేళ(ERR_peek_error() != 0)
    ret = 0;

  ఒకవేళ(ret) {
    X509 *ca;
    sslerr_t err;

    ఒకవేళ(!SSL_CTX_clear_chain_certs(ctx)) {
      ret = 0;
      వెళ్ళు చివర;
    }

    అయితే((ca = PEM_read_bio_X509(లోపల, ఖాళీ, passwd_callback,
                                  CURL_UNCONST(కీలకం_passwd))) != ఖాళీ) {

      ఒకవేళ(!SSL_CTX_add0_chain_cert(ctx, ca)) {
        X509_free(ca);
        ret = 0;
        వెళ్ళు చివర;
      }
    }

    err = ERR_peek_చివరి_error();
    ఒకవేళ((ERR_GET_LIB(err) == ERR_LIB_PEM) &&
       (ERR_GET_REASON(err) == PEM_R_NO_ప్రారంభం_పంక్తి))
      ERR_clear_error();
    లేకపోతే
      ret = 0;
  }

చివర:
  X509_free(x);
  BIO_free(లోపల);
  తిరిగి ret;
}

స్థిర పూర్ణాంకం enginecheck(నిర్మాణం Curl_easy *దత్తాంశం,
                       నిర్మాణం ossl_ctx *octx,
                       స్థిరరాశి అక్షరం *కీలకం_file,
                       స్థిరరాశి అక్షరం *కీలకం_passwd)
{
#నిర్వచితమైతే USE_OPENSSL_ENGINE
  EVP_PKEY *priv_కీలకం = ఖాళీ;
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the
   * key_file is a PKCS#11 URI */
  ఒకవేళ(!octx->p.engine && is_pkcs11_uri(కీలకం_file) &&
     ossl_సమితి_engine(దత్తాంశం, octx, "pkcs11") != CURLE_OK)
    తిరిగి 0;

  ఒకవేళ(octx->p.engine) {
    UI_పద్ధతి *ui_పద్ధతి = UI_create_పద్ధతి("curl user interface");
    ఒకవేళ(!ui_పద్ధతి) {
      failf(దత్తాంశం, "unable to create " OSSL_PACKAGE " user-interface method");
      తిరిగి 0;
    }
    UI_పద్ధతి_సమితి_opener(ui_పద్ధతి, UI_పద్ధతి_get_opener(UI_OpenSSL()));
    UI_పద్ధతి_సమితి_closer(ui_పద్ధతి, UI_పద్ధతి_get_closer(UI_OpenSSL()));
    UI_పద్ధతి_సమితి_reader(ui_పద్ధతి, ssl_ui_reader);
    UI_పద్ధతి_సమితి_writer(ui_పద్ధతి, ssl_ui_writer);
    priv_కీలకం = ENGINE_ఎక్కించు_వ్యక్తిగత_కీలకం(octx->p.engine, కీలకం_file,
                                       ui_పద్ధతి,
                                       CURL_UNCONST(కీలకం_passwd));
    UI_destroy_పద్ధతి(ui_పద్ధతి);
    ఒకవేళ(!priv_కీలకం) {
      failf(దత్తాంశం, "failed to load private key from crypto engine");
      తిరిగి 0;
    }
    ఒకవేళ(SSL_CTX_use_వ్యక్తిగత_కీలకం(ctx, priv_కీలకం) != 1) {
      failf(దత్తాంశం, "unable to set private key");
      EVP_PKEY_free(priv_కీలకం);
      తిరిగి 0;
    }
    EVP_PKEY_free(priv_కీలకం);  /* we do not need the handle any more... */
  }
  లేకపోతే {
    failf(దత్తాంశం, "crypto engine not set, cannot load private key");
    తిరిగి 0;
  }
  తిరిగి 1;
#లేకపోతే
  (శూన్య)octx;
  (శూన్య)కీలకం_file;
  (శూన్య)కీలకం_passwd;
  failf(దత్తాంశం, "SSL_FILETYPE_ENGINE not supported for private key");
  తిరిగి 0;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం providercheck(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం ossl_ctx *octx,
                         స్థిరరాశి అక్షరం *కీలకం_file)
{
#నిర్వచితమైతే OPENSSL_HAS_PROVIDERS
  అక్షరం error_బఫరు[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the
   * key_file is a PKCS#11 URI */
  ఒకవేళ(!octx->p.provider_loaded && is_pkcs11_uri(కీలకం_file) &&
     ossl_సమితి_provider(దత్తాంశం, octx, "pkcs11") != CURLE_OK) {
    తిరిగి 0;
  }

  ఒకవేళ(octx->p.provider_loaded) {
    /* Load the private key from the provider */
    EVP_PKEY *priv_కీలకం = ఖాళీ;
    OSSL_STORE_CTX *store = ఖాళీ;
    OSSL_STORE_INFO *info = ఖాళీ;
    UI_పద్ధతి *ui_పద్ధతి = UI_create_పద్ధతి("curl user interface");
    ఒకవేళ(!ui_పద్ధతి) {
      failf(దత్తాంశం, "unable to create " OSSL_PACKAGE " user-interface method");
      తిరిగి 0;
    }
    UI_పద్ధతి_సమితి_opener(ui_పద్ధతి, UI_పద్ధతి_get_opener(UI_OpenSSL()));
    UI_పద్ధతి_సమితి_closer(ui_పద్ధతి, UI_పద్ధతి_get_closer(UI_OpenSSL()));
    UI_పద్ధతి_సమితి_reader(ui_పద్ధతి, ssl_ui_reader);
    UI_పద్ధతి_సమితి_writer(ui_పద్ధతి, ssl_ui_writer);

    store = OSSL_STORE_open_ex(కీలకం_file, octx->p.libctx,
                               octx->p.propq, ui_పద్ధతి, ఖాళీ, ఖాళీ,
                               ఖాళీ, ఖాళీ);
    ఒకవేళ(!store) {
      failf(దత్తాంశం, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      UI_destroy_పద్ధతి(ui_పద్ధతి);
      తిరిగి 0;
    }
    ఒకవేళ(OSSL_STORE_expect(store, OSSL_STORE_INFO_PKEY) != 1) {
      failf(దత్తాంశం, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
    }

    info = OSSL_STORE_ఎక్కించు(store);
    ఒకవేళ(info) {
      పూర్ణాంకం ossl_type = OSSL_STORE_INFO_get_type(info);

      ఒకవేళ(ossl_type == OSSL_STORE_INFO_PKEY)
        priv_కీలకం = OSSL_STORE_INFO_get1_PKEY(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    UI_destroy_పద్ధతి(ui_పద్ధతి);
    ఒకవేళ(!priv_కీలకం) {
      failf(దత్తాంశం, "No private key found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      తిరిగి 0;
    }

    ఒకవేళ(SSL_CTX_use_వ్యక్తిగత_కీలకం(ctx, priv_కీలకం) != 1) {
      failf(దత్తాంశం, "unable to set private key [%s]",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      EVP_PKEY_free(priv_కీలకం);
      తిరిగి 0;
    }
    EVP_PKEY_free(priv_కీలకం); /* we do not need the handle any more... */
  }
  లేకపోతే {
    failf(దత్తాంశం, "crypto provider not set, cannot load private key");
    తిరిగి 0;
  }
  తిరిగి 1;
#లేకపోతే
  (శూన్య)octx;
  (శూన్య)కీలకం_file;
  failf(దత్తాంశం, "SSL_FILETYPE_PROVIDER not supported for private key");
  తిరిగి 0;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం engineload(నిర్మాణం Curl_easy *దత్తాంశం,
                      నిర్మాణం ossl_ctx *octx,
                      స్థిరరాశి అక్షరం *cert_file)
{
/* ENGINE_CTRL_GET_CMD_FROM_NAME supported by OpenSSL, LibreSSL <=3.8.3 */
#ఒకవేళ defined(USE_OPENSSL_ENGINE) && defined(ENGINE_CTRL_GET_CMD_FROM_NAME)
  అక్షరం error_బఫరు[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 engine if none was provided and the cert_file is a
   * PKCS#11 URI */
  ఒకవేళ(!octx->p.engine && is_pkcs11_uri(cert_file) &&
     ossl_సమితి_engine(దత్తాంశం, octx, "pkcs11") != CURLE_OK)
    తిరిగి 0;

  ఒకవేళ(octx->p.engine) {
    స్థిర స్థిరరాశి అక్షరం cmd_name[] = "LOAD_CERT_CTRL";
    నిర్మాణం {
      స్థిరరాశి అక్షరం *cert_id;
      X509 *cert;
    } params;

    params.cert_id = cert_file;
    params.cert = ఖాళీ;

    /* Does the engine supports LOAD_CERT_CTRL ? */
    ఒకవేళ(!ENGINE_ctrl(octx->p.engine, ENGINE_CTRL_GET_CMD_FROM_NAME,
                    0, CURL_UNCONST(cmd_name), ఖాళీ)) {
      failf(దత్తాంశం, "SSL engine does not support loading certificates");
      తిరిగి 0;
    }

    /* Load the certificate from the engine */
    ఒకవేళ(!ENGINE_ctrl_cmd(octx->p.engine, cmd_name, 0, &params, ఖాళీ, 1)) {
      failf(దత్తాంశం, "SSL engine cannot load client cert with id '%s' [%s]",
            cert_file,
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      తిరిగి 0;
    }

    ఒకవేళ(!params.cert) {
      failf(దత్తాంశం, "SSL engine did not initialize the certificate properly.");
      తిరిగి 0;
    }

    ఒకవేళ(SSL_CTX_use_certificate(ctx, params.cert) != 1) {
      failf(దత్తాంశం, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      X509_free(params.cert);
      తిరిగి 0;
    }
    X509_free(params.cert); /* we do not need the handle any more... */
  }
  లేకపోతే {
    failf(దత్తాంశం, "crypto engine not set, cannot load certificate");
    తిరిగి 0;
  }
  తిరిగి 1;
#లేకపోతే
  (శూన్య)octx;
  (శూన్య)cert_file;
  failf(దత్తాంశం, "SSL_FILETYPE_ENGINE not supported for certificate");
  తిరిగి 0;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం providerload(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం ossl_ctx *octx,
                        స్థిరరాశి అక్షరం *cert_file)
{
#నిర్వచితమైతే OPENSSL_HAS_PROVIDERS
  అక్షరం error_బఫరు[256];
  SSL_CTX *ctx = octx->ssl_ctx;

  /* Implicitly use pkcs11 provider if none was provided and the cert_file is
   * a PKCS#11 URI */
  ఒకవేళ(!octx->p.provider_loaded && is_pkcs11_uri(cert_file) &&
     ossl_సమితి_provider(దత్తాంశం, octx, "pkcs11") != CURLE_OK)
    తిరిగి 0;

  ఒకవేళ(octx->p.provider_loaded) {
    /* Load the certificate from the provider */
    OSSL_STORE_INFO *info = ఖాళీ;
    X509 *cert = ఖాళీ;
    OSSL_STORE_CTX *store =
      OSSL_STORE_open_ex(cert_file, octx->p.libctx,
                         ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);
    పూర్ణాంకం rc;

    ఒకవేళ(!store) {
      failf(దత్తాంశం, "Failed to open OpenSSL store: %s",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      తిరిగి 0;
    }
    ఒకవేళ(OSSL_STORE_expect(store, OSSL_STORE_INFO_CERT) != 1) {
      failf(దత్తాంశం, "Failed to set store preference. Ignoring the error: %s",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
    }

    info = OSSL_STORE_ఎక్కించు(store);
    ఒకవేళ(info) {
      పూర్ణాంకం ossl_type = OSSL_STORE_INFO_get_type(info);

      ఒకవేళ(ossl_type == OSSL_STORE_INFO_CERT)
        cert = OSSL_STORE_INFO_get1_CERT(info);
      OSSL_STORE_INFO_free(info);
    }
    OSSL_STORE_close(store);
    ఒకవేళ(!cert) {
      failf(దత్తాంశం, "No cert found in the openssl store: %s",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      తిరిగి 0;
    }

    rc = SSL_CTX_use_certificate(ctx, cert);
    X509_free(cert); /* we do not need the handle any more... */

    ఒకవేళ(rc != 1) {
      failf(దత్తాంశం, "unable to set client certificate [%s]",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      తిరిగి 0;
    }
  }
  లేకపోతే {
    failf(దత్తాంశం, "crypto provider not set, cannot load certificate");
    తిరిగి 0;
  }
  తిరిగి 1;
#లేకపోతే
  (శూన్య)octx;
  (శూన్య)cert_file;
  failf(దత్తాంశం, "SSL_FILETYPE_PROVIDER not supported for certificate");
  తిరిగి 0;
#అయితే_ముగింపు
}

స్థిర పూర్ణాంకం pkcs12load(నిర్మాణం Curl_easy *దత్తాంశం,
                      SSL_CTX* ctx,
                      స్థిరరాశి నిర్మాణం curl_blob *cert_blob,
                      స్థిరరాశి అక్షరం *cert_file,
                      స్థిరరాశి అక్షరం *కీలకం_passwd)
{
  అక్షరం error_బఫరు[256];
  BIO *cert_bio = ఖాళీ;
  PKCS12 *p12 = ఖాళీ;
  EVP_PKEY *pri;
  X509 *x509;
  పూర్ణాంకం cert_పూర్తి = 0;
  దొంతర_యొక్క(X509) *ca = ఖాళీ;
  ఒకవేళ(cert_blob) {
    cert_bio = BIO_new_mem_buf(cert_blob->దత్తాంశం, (పూర్ణాంకం)cert_blob->len);
    ఒకవేళ(!cert_bio) {
      failf(దత్తాంశం, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      తిరిగి 0;
    }
  }
  లేకపోతే {
    cert_bio = BIO_new(BIO_s_file());
    ఒకవేళ(!cert_bio) {
      failf(దత్తాంశం, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
      తిరిగి 0;
    }

    ఒకవేళ(BIO_read_filename(cert_bio, CURL_UNCONST(cert_file)) <= 0) {
      failf(దత్తాంశం, "could not open PKCS12 file '%s'", cert_file);
      BIO_free(cert_bio);
      తిరిగి 0;
    }
  }

  p12 = d2i_PKCS12_bio(cert_bio, ఖాళీ);
  BIO_free(cert_bio);

  ఒకవేళ(!p12) {
    failf(దత్తాంశం, "error reading PKCS12 file '%s'",
          cert_blob ? "(memory blob)" : cert_file);
    తిరిగి 0;
  }

  ఒకవేళ(!PKCS12_parse(p12, కీలకం_passwd, &pri, &x509, &ca)) {
    failf(దత్తాంశం, "could not parse PKCS12 file, check password, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_బఫరు, పరిమాణం(error_బఫరు)));
    PKCS12_free(p12);
    తిరిగి 0;
  }

  PKCS12_free(p12);

  ఒకవేళ(SSL_CTX_use_certificate(ctx, x509) != 1) {
    failf(దత్తాంశం, "could not load PKCS12 client certificate, " OSSL_PACKAGE
          " error %s",
          ossl_strerror(ERR_get_error(), error_బఫరు, పరిమాణం(error_బఫరు)));
    వెళ్ళు fail;
  }

  ఒకవేళ(SSL_CTX_use_వ్యక్తిగత_కీలకం(ctx, pri) != 1) {
    failf(దత్తాంశం, "unable to use private key from PKCS12 file '%s'", cert_file);
    వెళ్ళు fail;
  }

  ఒకవేళ(!SSL_CTX_check_వ్యక్తిగత_కీలకం(ctx)) {
    failf(దత్తాంశం, "private key from PKCS12 file '%s' "
          "does not match certificate in same file", cert_file);
    వెళ్ళు fail;
  }
  /* Set Certificate Verification chain */
  ఒకవేళ(ca) {
    అయితే(sk_X509_అంకె(ca)) {
      /*
       * Note that sk_X509_pop() is used below to make sure the cert is
       * removed from the stack properly before getting passed to
       * SSL_CTX_add_extra_chain_cert(), which takes ownership. Previously
       * we used sk_X509_value() instead, but then we would clean it in the
       * subsequent sk_X509_pop_free() call.
       */
      X509 *x = sk_X509_తీయి(ca);
      ఒకవేళ(!SSL_CTX_కూడు_client_CA(ctx, x)) {
        X509_free(x);
        failf(దత్తాంశం, "cannot add certificate to client CA list");
        వెళ్ళు fail;
      }
      ఒకవేళ(!SSL_CTX_కూడు_extra_chain_cert(ctx, x)) {
        X509_free(x);
        failf(దత్తాంశం, "cannot add certificate to certificate chain");
        వెళ్ళు fail;
      }
    }
  }

  cert_పూర్తి = 1;
fail:
  EVP_PKEY_free(pri);
  X509_free(x509);
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
  sk_X509_తీయి_free(ca, X509_free);
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
  ఒకవేళ(!cert_పూర్తి)
    తిరిగి 0; /* failure! */
  తిరిగి 1;
}

స్థిర CURLcode client_cert(నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ossl_ctx *octx,
                            అక్షరం *cert_file,
                            స్థిరరాశి నిర్మాణం curl_blob *cert_blob,
                            స్థిరరాశి అక్షరం *cert_type,
                            అక్షరం *కీలకం_file,
                            స్థిరరాశి నిర్మాణం curl_blob *కీలకం_blob,
                            స్థిరరాశి అక్షరం *కీలకం_type,
                            అక్షరం *కీలకం_passwd)
{
  అక్షరం error_బఫరు[256];
  bool check_privkey = TRUE;
  పూర్ణాంకం file_type = ossl_do_file_type(cert_type);
  SSL_CTX *ctx = octx->ssl_ctx;

  ఒకవేళ(cert_file || cert_blob || (file_type == SSL_FILETYPE_ENGINE) ||
     (file_type == SSL_FILETYPE_PROVIDER)) {
    SSL *ssl;
    X509 *x509;
    bool pcks12_పూర్తి = FALSE;
    పూర్ణాంకం cert_use_ఫలితం;

    ఒకవేళ(కీలకం_passwd) {
      /* set the password in the callback userdata */
      SSL_CTX_సమితి_default_passwd_cb_userdata(ctx, కీలకం_passwd);
      /* Set passwd callback: */
      SSL_CTX_సమితి_default_passwd_cb(ctx, passwd_callback);
    }

    ఎంపిక(file_type) {
    స్థితి SSL_FILETYPE_PEM:
      /* SSL_CTX_use_certificate_chain_file() only works on PEM files */
      cert_use_ఫలితం = cert_blob ?
        use_certificate_chain_blob(ctx, cert_blob, కీలకం_passwd) :
        SSL_CTX_use_certificate_chain_file(ctx, cert_file);
      ఒకవేళ(cert_use_ఫలితం != 1) {
        failf(దత్తాంశం,
              "could not load PEM client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_బఫరు,
                            పరిమాణం(error_బఫరు)));
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }
      ఆపు;

    స్థితి SSL_FILETYPE_ASN1:
      /* SSL_CTX_use_certificate_file() works with either PEM or ASN1, but
         we use the case above for PEM so this can only be performed with
         ASN1 files. */

      cert_use_ఫలితం = cert_blob ?
        use_certificate_blob(ctx, cert_blob, file_type, కీలకం_passwd) :
      SSL_CTX_use_certificate_file(ctx, cert_file, file_type);
      ఒకవేళ(cert_use_ఫలితం != 1) {
        failf(దత్తాంశం,
              "could not load ASN1 client certificate from %s, " OSSL_PACKAGE
              " error %s, "
              "(no key found, wrong passphrase, or wrong file format?)",
              (cert_blob ? "CURLOPT_SSLCERT_BLOB" : cert_file),
              ossl_strerror(ERR_get_error(), error_బఫరు,
                            పరిమాణం(error_బఫరు)));
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }
      ఆపు;

    స్థితి SSL_FILETYPE_ENGINE:
      ఒకవేళ(!cert_file || !engineload(దత్తాంశం, octx, cert_file))
        తిరిగి CURLE_SSL_CERTPROBLEM;
      ఆపు;

    స్థితి SSL_FILETYPE_PROVIDER:
      ఒకవేళ(!cert_file || !providerload(దత్తాంశం, octx, cert_file))
        తిరిగి CURLE_SSL_CERTPROBLEM;
      ఆపు;

    స్థితి SSL_FILETYPE_PKCS12:
      ఒకవేళ(!pkcs12load(దత్తాంశం, ctx, cert_blob, cert_file, కీలకం_passwd))
        తిరిగి CURLE_SSL_CERTPROBLEM;
      pcks12_పూర్తి = TRUE;
      ఆపు;

    అప్రమేయ:
      failf(దత్తాంశం, "not supported file type '%s' for certificate", cert_type);
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }

    ఒకవేళ(!కీలకం_file && !కీలకం_blob) {
      కీలకం_file = cert_file;
      కీలకం_blob = cert_blob;
    }
    లేకపోతే
      file_type = ossl_do_file_type(కీలకం_type);

    ఎంపిక(file_type) {
    స్థితి SSL_FILETYPE_PEM:
    స్థితి SSL_FILETYPE_ASN1:
      cert_use_ఫలితం = కీలకం_blob ?
        use_privatekey_blob(ctx, కీలకం_blob, file_type, కీలకం_passwd) :
      SSL_CTX_use_వ్యక్తిగత_కీలకం_file(ctx, కీలకం_file, file_type);
      ఒకవేళ(cert_use_ఫలితం != 1) {
        failf(దత్తాంశం, "unable to set private key file: '%s' type %s",
              కీలకం_file ? కీలకం_file : "(memory blob)",
              కీలకం_type ? కీలకం_type : "PEM");
        తిరిగి CURLE_BAD_FUNCTION_వాదం;
      }
      ఆపు;
    స్థితి SSL_FILETYPE_ENGINE:
      ఒకవేళ(!enginecheck(దత్తాంశం, octx, కీలకం_file, కీలకం_passwd))
        తిరిగి CURLE_SSL_CERTPROBLEM;
      ఆపు;

    స్థితి SSL_FILETYPE_PROVIDER:
      ఒకవేళ(!providercheck(దత్తాంశం, octx, కీలకం_file))
        తిరిగి CURLE_SSL_CERTPROBLEM;
      ఆపు;

    స్థితి SSL_FILETYPE_PKCS12:
      ఒకవేళ(!pcks12_పూర్తి) {
        failf(దత్తాంశం, "file type P12 for private key not supported");
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }
      ఆపు;
    అప్రమేయ:
      failf(దత్తాంశం, "not supported file type for private key");
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }

    ssl = SSL_new(ctx);
    ఒకవేళ(!ssl) {
      failf(దత్తాంశం, "unable to create an SSL structure");
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }

    x509 = SSL_get_certificate(ssl);

    ఒకవేళ(x509) {
      EVP_PKEY *pktmp = X509_get_pubkey(x509);
      EVP_PKEY_copy_parameters(pktmp, SSL_get_privatekey(ssl));
      EVP_PKEY_free(pktmp);
    }

#ఒకవేళ !defined(OPENSSL_NO_RSA) && !defined(OPENSSL_NO_DEPRECATED_3_0)
    {
      /* If RSA is used, do not check the private key if its flags indicate
       * it does not support it. */
      EVP_PKEY *priv_కీలకం = SSL_get_privatekey(ssl);
      ఒకవేళ(EVP_PKEY_id(priv_కీలకం) == EVP_PKEY_RSA) {
        RSA *rsa = EVP_PKEY_get1_RSA(priv_కీలకం);
        ఒకవేళ(RSA_flags(rsa) & RSA_పద్ధతి_FLAG_NO_CHECK)
          check_privkey = FALSE;
        RSA_free(rsa); /* Decrement reference count */
      }
    }
#అయితే_ముగింపు

    SSL_free(ssl);

    /* If we are using DSA, we can copy the parameters from
     * the private key */

    ఒకవేళ(check_privkey == TRUE) {
      /* Now we know that a key and cert have been set against
       * the SSL context */
      ఒకవేళ(!SSL_CTX_check_వ్యక్తిగత_కీలకం(ctx)) {
        failf(దత్తాంశం, "Private key does not match the certificate public key");
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }
    }
  }
  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే CURLVERBOSE
/* returns non-zero on failure */
స్థిర CURLcode x509_name_oneline(స్థిరరాశి X509_NAME *a, నిర్మాణం dynbuf *d)
{
  BIO *bio_బహిర్గతం = BIO_new(BIO_s_mem());
  BUF_MEM *biomem;
  పూర్ణాంకం rc;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(bio_బహిర్గతం) {
    గుర్తులేని దీర్ఘ flags = XN_FLAG_SEP_SPLUS_SPC |
      (XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB & ~XN_FLAG_SPC_EQ);
    curlx_dyn_reset(d);
    rc = X509_NAME_print_ex(bio_బహిర్గతం, a, 0, flags);
    ఒకవేళ(rc != -1) {
      BIO_get_mem_ptr(bio_బహిర్గతం, &biomem);
      ఫలితం = curlx_dyn_addn(d, biomem->దత్తాంశం, biomem->నిడివి);
    }
    BIO_free(bio_బహిర్గతం);
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
స్థిర పూర్ణాంకం ossl_init(శూన్య)
{
  స్థిరరాశి uint64_t flags =
#నిర్వచితమైతే OPENSSL_INIT_ENGINE_ALL_BUILTIN
    /* not present in BoringSSL */
    OPENSSL_INIT_ENGINE_ALL_BUILTIN |
#అయితే_ముగింపు
#నిర్వచితమైతే CURL_DISABLE_OPENSSL_AUTO_ఎక్కించు_CONFIG
    OPENSSL_INIT_NO_ఎక్కించు_CONFIG |
#లేకపోతే
    OPENSSL_INIT_ఎక్కించు_CONFIG |
#అయితే_ముగింపు
    0;
  OPENSSL_init_ssl(flags, ఖాళీ);

#నిర్వచితం_కాకపోతే HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_open();
#అయితే_ముగింపు

  తిరిగి 1;
}

/* Global cleanup */
స్థిర శూన్య ossl_cleanup(శూన్య)
{
#నిర్వచితం_కాకపోతే HAVE_KEYLOG_UPSTREAM
  Curl_tls_keylog_close();
#అయితే_ముగింపు
}

/* Selects an OpenSSL crypto engine or provider.
 */
స్థిర CURLcode ossl_సమితి_engine(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ossl_ctx *octx,
                                స్థిరరాశి అక్షరం *name)
{
#నిర్వచితమైతే USE_OPENSSL_ENGINE
  CURLcode ఫలితం = CURLE_SSL_ENGINE_NOTFOUND;
  ENGINE *e = ENGINE_by_id(name);

  ఒకవేళ(e) {

    ఒకవేళ(octx->p.engine) {
      ENGINE_ముగింపు(octx->p.engine);
      ENGINE_free(octx->p.engine);
      octx->p.engine = ఖాళీ;
    }
    ఒకవేళ(!ENGINE_init(e)) {
      అక్షరం buf[256];

      ENGINE_free(e);
      failf(దత్తాంశం, "Failed to initialize SSL Engine '%s': %s",
            name, ossl_strerror(ERR_get_error(), buf, పరిమాణం(buf)));
      ఫలితం = CURLE_SSL_ENGINE_INITFAILED;
      e = ఖాళీ;
    }
    లేకపోతే {
      ఫలితం = CURLE_OK;
    }
    octx->p.engine = e;
    తిరిగి ఫలితం;
  }
#అయితే_ముగింపు
#నిర్వచితమైతే OPENSSL_HAS_PROVIDERS
  తిరిగి ossl_సమితి_provider(దత్తాంశం, octx, name);
#లేకపోతే
  (శూన్య)octx;
  (శూన్య)name;
  failf(దత్తాంశం, "OpenSSL engine not found");
  తిరిగి CURLE_SSL_ENGINE_NOTFOUND;
#అయితే_ముగింపు
}

/* Sets engine as default for all SSL operations
 */
స్థిర CURLcode ossl_సమితి_engine_default(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ossl_ctx *octx)
{
#నిర్వచితమైతే USE_OPENSSL_ENGINE
  ఒకవేళ(octx->p.engine) {
    ఒకవేళ(ENGINE_సమితి_default(octx->p.engine, ENGINE_పద్ధతి_ALL) > 0) {
      infof(దత్తాంశం, "set default crypto engine '%s'",
            ENGINE_get_id(octx->p.engine));
    }
    లేకపోతే {
      failf(దత్తాంశం, "set default crypto engine '%s' failed",
            ENGINE_get_id(octx->p.engine));
      తిరిగి CURLE_SSL_ENGINE_SETFAILED;
    }
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)octx;
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

/* Return list of OpenSSL crypto engine names.
 */
స్థిర నిర్మాణం curl_slist *ossl_engines_list(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం curl_slist *list = ఖాళీ;
#నిర్వచితమైతే USE_OPENSSL_ENGINE
  నిర్మాణం curl_slist *beg;
  ENGINE *e;

  కొరకు(e = ENGINE_get_మొదటి(); e; e = ENGINE_get_తదుపరి(e)) {
    beg = curl_slist_append(list, ENGINE_get_id(e));
    ఒకవేళ(!beg) {
      curl_slist_free_all(list);
      తిరిగి ఖాళీ;
    }
    list = beg;
  }
#అయితే_ముగింపు
  (శూన్య)దత్తాంశం;
  తిరిగి list;
}

#నిర్వచితమైతే OPENSSL_HAS_PROVIDERS

స్థిర శూన్య ossl_provider_cleanup(నిర్మాణం ossl_ctx *octx)
{
  ఒకవేళ(!octx)
    తిరిగి;
  ఒకవేళ(octx->p.baseprov) {
    OSSL_PROVIDER_unload(octx->p.baseprov);
    octx->p.baseprov = ఖాళీ;
  }
  ఒకవేళ(octx->p.provider) {
    OSSL_PROVIDER_unload(octx->p.provider);
    octx->p.provider = ఖాళీ;
  }
  OSSL_LIB_CTX_free(octx->p.libctx);
  octx->p.libctx = ఖాళీ;
  curlx_safefree(octx->p.propq);
  octx->p.provider_loaded = FALSE;
}

#నిర్వచించు MAX_PROVIDER_LEN 128 /* reasonable */

/* Selects an OpenSSL crypto provider.
 *
 * A provider might need an associated property, a string passed on to
 * OpenSSL. Specify this as [PROVIDER][:PROPERTY]: separate the name and the
 * property with a colon. No colon means no property is set.
 *
 * An example provider + property looks like "tpm2:?provider=tpm2".
 */
స్థిర CURLcode ossl_సమితి_provider(నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ossl_ctx *octx,
                                  స్థిరరాశి అక్షరం *iname)
{
  అక్షరం name[MAX_PROVIDER_LEN + 1];
  నిర్మాణం Curl_str prov;
  స్థిరరాశి అక్షరం *propq = ఖాళీ;

  ఒకవేళ(!iname) {
    /* clear and cleanup provider use */
    ossl_provider_cleanup(octx);
    తిరిగి CURLE_OK;
  }
  ఒకవేళ(curlx_str_until(&iname, &prov, MAX_PROVIDER_LEN, ':'))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఒకవేళ(!curlx_str_single(&iname, ':'))
    /* there was a colon, get the propq until the end of string */
    propq = iname;

  /* we need the name in a buffer, null-terminated */
  జ్ఞాపకం_నకలు(name, curlx_str(&prov), curlx_strlen(&prov));
  name[curlx_strlen(&prov)] = 0;

  ఒకవేళ(!octx->p.libctx) {
    OSSL_LIB_CTX *libctx = OSSL_LIB_CTX_new();
    ఒకవేళ(!libctx)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    ఒకవేళ(propq) {
      octx->p.propq = curlx_strdup(propq);
      ఒకవేళ(!octx->p.propq) {
        OSSL_LIB_CTX_free(libctx);
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      }
    }
    octx->p.libctx = libctx;
  }

#నిర్వచితం_కాకపోతే CURL_DISABLE_OPENSSL_AUTO_ఎక్కించు_CONFIG
  /* load the configuration file into the library context before checking the
   * provider availability */
  ఒకవేళ(!OSSL_LIB_CTX_ఎక్కించు_config(octx->p.libctx, ఖాళీ)) {
    infof(దత్తాంశం, "Failed to load default openssl config. Proceeding.");
  }
#అయితే_ముగింపు

  ఒకవేళ(OSSL_PROVIDER_అందుబాటు(octx->p.libctx, name)) {
    /* already loaded through the configuration - no action needed */
    octx->p.provider_loaded = TRUE;
    తిరిగి CURLE_OK;
  }

  octx->p.provider = OSSL_PROVIDER_try_ఎక్కించు(octx->p.libctx, name, 1);
  ఒకవేళ(!octx->p.provider) {
    అక్షరం error_బఫరు[256];
    failf(దత్తాంశం, "Failed to initialize provider: %s",
          ossl_strerror(ERR_get_error(), error_బఫరు, పరిమాణం(error_బఫరు)));
    ossl_provider_cleanup(octx);
    తిరిగి CURLE_SSL_ENGINE_NOTFOUND;
  }

  /* load the base provider as well */
  octx->p.baseprov = OSSL_PROVIDER_try_ఎక్కించు(octx->p.libctx, "base", 1);
  ఒకవేళ(!octx->p.baseprov) {
    ossl_provider_cleanup(octx);
    failf(దత్తాంశం, "Failed to load base");
    తిరిగి CURLE_SSL_ENGINE_NOTFOUND;
  }
  లేకపోతే
    octx->p.provider_loaded = TRUE;
  తిరిగి CURLE_OK;
}
#లేకపోతే /* OPENSSL_HAS_PROVIDERS */
#నిర్వచించు ossl_provider_cleanup(x)
#అయితే_ముగింపు

స్థిర CURLcode ossl_shutdown(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              bool send_shutdown, bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  అక్షరం buf[1024];
  పూర్ణాంకం nread = -1, err;
  size_t i;

  DEBUGASSERT(octx);
  ఒకవేళ(!octx->ssl || cf->shutdown) {
    *పూర్తి = TRUE;
    వెళ్ళు బహిర్గతం;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *పూర్తి = FALSE;
  ఒకవేళ(!(SSL_get_shutdown(octx->ssl) & SSL_పంపిన_SHUTDOWN)) {
    /* We have not started the shutdown from our side yet. Check
     * if the server already sent us one. */
    ERR_clear_error();
    కొరకు(i = 0; i < 10; ++i) {
      nread = SSL_read(octx->ssl, buf, (పూర్ణాంకం)పరిమాణం(buf));
      CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown not sent, read -> %d", nread);
      ఒకవేళ(nread <= 0)
        ఆపు;
    }
    err = SSL_get_error(octx->ssl, nread);
    ఒకవేళ(!nread && err == SSL_ERROR_ZERO_RETURN) {
      bool ఆదానం_pending;
      /* Yes, it did. */
      ఒకవేళ(!send_shutdown) {
        CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown received, not sending");
        *పూర్తి = TRUE;
        వెళ్ళు బహిర్గతం;
      }
      లేకపోతే ఒకవేళ(!cf->తదుపరి->cft->is_alive(cf->తదుపరి, దత్తాంశం, &ఆదానం_pending)) {
        /* Server closed the connection after its closy notify. It
         * seems not interested to see our close notify, so do not
         * send it. We are done. */
        connssl->peer_closed = TRUE;
        CURL_TRC_CF(దత్తాంశం, cf, "peer closed connection");
        *పూర్తి = TRUE;
        వెళ్ళు బహిర్గతం;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ఒకవేళ(send_shutdown && !(SSL_get_shutdown(octx->ssl) & SSL_పంపిన_SHUTDOWN)) {
    పూర్ణాంకం rc;
    ERR_clear_error();
    CURL_TRC_CF(దత్తాంశం, cf, "send SSL close notify");
    rc = SSL_shutdown(octx->ssl);
    ఒకవేళ(rc == 1) {
      CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown finished");
      *పూర్తి = TRUE;
      వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(SSL_get_error(octx->ssl, rc) == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown still wants to send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      వెళ్ళు బహిర్గతం;
    }
    /* Having sent the close notify, we use SSL_read() to get the
     * missing close notify from the server. */
  }

  కొరకు(i = 0; i < 10; ++i) {
    ERR_clear_error();
    nread = SSL_read(octx->ssl, buf, (పూర్ణాంకం)పరిమాణం(buf));
    CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown read -> %d", nread);
    ఒకవేళ(nread <= 0)
      ఆపు;
  }
  err = SSL_get_error(octx->ssl, nread);
  ఎంపిక(err) {
  స్థితి SSL_ERROR_ZERO_RETURN: /* no more data */
    ఒకవేళ(SSL_shutdown(octx->ssl) == 1)
      CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown finished");
    లేకపోతే
      CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown not received, but closed");
    *పూర్తి = TRUE;
    ఆపు;
  స్థితి SSL_ERROR_NONE: /* did not get anything */
  స్థితి SSL_ERROR_WANT_READ:
    /* SSL has sent its notify and now wants to read the reply
     * from the server. We are not really interested in that. */
    CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown sent, want receive");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ఆపు;
  స్థితి SSL_ERROR_WANT_WRITE:
    CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown send blocked");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ఆపు;
  అప్రమేయ:
    /* Server seems to have closed the connection without sending us
     * a close notify. */
    {
      VERBOSE(గుర్తులేని దీర్ఘ sslerr = ERR_get_error());
      CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown, ignore recv error: '%s', errno %d",
                  (sslerr ?
                   ossl_strerror(sslerr, buf, పరిమాణం(buf)) :
                   SSL_ERROR_గమ్యం_str(err)),
                  SOCKERRNO);
    }
    *పూర్తి = TRUE;
    ఫలితం = CURLE_OK;
    ఆపు;
  }

బహిర్గతం:
  cf->shutdown = (ఫలితం || *పూర్తి);
  ఒకవేళ(cf->shutdown || (connssl->io_need != CURL_SSL_IO_NEED_NONE))
    connssl->ఆదానం_pending = FALSE;
  తిరిగి ఫలితం;
}

/* also used by quic-tls */
శూన్య Curl_osslctx_cleanup(నిర్మాణం ossl_ctx *octx)
{
  DEBUGASSERT(octx);
  ఒకవేళ(octx->ssl) {
    SSL_free(octx->ssl);
    octx->ssl = ఖాళీ;
  }
  ఒకవేళ(octx->ssl_ctx) {
    SSL_CTX_free(octx->ssl_ctx);
    octx->ssl_ctx = ఖాళీ;
    octx->x509_store_అమరిక = FALSE;
  }
  ఒకవేళ(octx->bio_పద్ధతి) {
    ossl_bio_cf_పద్ధతి_free(octx->bio_పద్ధతి);
    octx->bio_పద్ధతి = ఖాళీ;
  }
#నిర్వచితమైతే USE_OPENSSL_ENGINE
  ఒకవేళ(octx->p.engine) {
    ENGINE_ముగింపు(octx->p.engine);
    ENGINE_free(octx->p.engine);
    octx->p.engine = ఖాళీ;
  }
#అయితే_ముగింపు
  ossl_provider_cleanup(octx);
}

స్థిర శూన్య ossl_close(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  (శూన్య)దత్తాంశం;
  Curl_osslctx_cleanup(octx);
}

/*
 * This function is called when the 'data' struct is going away. Close
 * down everything and free all resources!
 */
స్థిర శూన్య ossl_close_all(నిర్మాణం Curl_easy *దత్తాంశం)
{
  (శూన్య)దత్తాంశం;
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
స్థిర CURLcode ossl_verifyhost(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం connectdata *conn,
                                నిర్మాణం ssl_peer *peer,
                                X509 *server_cert)
{
  bool matched = FALSE;
  పూర్ణాంకం లక్ష్యం; /* target type, GEN_DNS or GEN_IPADD */
  size_t addrlen = 0;
  దొంతర_యొక్క(GENERAL_NAME) *altnames;
#నిర్వచితమైతే USE_IPV6
  నిర్మాణం in6_addr addr;
#లేకపోతే
  నిర్మాణం in_addr addr;
#అయితే_ముగింపు
  CURLcode ఫలితం = CURLE_OK;
  bool dNSName = FALSE; /* if a dNSName field exists in the cert */
  bool iPAddress = FALSE; /* if an iPAddress field exists in the cert */
  size_t hostlen = అక్షరశ్రేణి_పొడవు(peer->origin->hostname);

  (శూన్య)conn;
  ఎంపిక(peer->type) {
  స్థితి CURL_SSL_PEER_IPV4:
    ఒకవేళ(!curlx_inet_pton(AF_INET, peer->origin->hostname, &addr))
      తిరిగి CURLE_PEER_FAILED_VERIFICATION;
    లక్ష్యం = GEN_IPADD;
    addrlen = పరిమాణం(నిర్మాణం in_addr);
    ఆపు;
#నిర్వచితమైతే USE_IPV6
  స్థితి CURL_SSL_PEER_IPV6:
    ఒకవేళ(!curlx_inet_pton(AF_INET6, peer->origin->hostname, &addr))
      తిరిగి CURLE_PEER_FAILED_VERIFICATION;
    లక్ష్యం = GEN_IPADD;
    addrlen = పరిమాణం(నిర్మాణం in6_addr);
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURL_SSL_PEER_DNS:
    లక్ష్యం = GEN_DNS;
    ఆపు;
  అప్రమేయ:
    DEBUGASSERT(0);
    failf(దత్తాంశం, "unexpected SSL peer type: %d", (పూర్ణాంకం)peer->type);
    తిరిగి CURLE_PEER_FAILED_VERIFICATION;
  }

  /* get a "list" of alternative names */
  altnames = X509_get_ext_d2i(server_cert, NID_subject_alt_name, ఖాళీ, ఖాళీ);

  ఒకవేళ(altnames) {
#నిర్వచితమైతే HAVE_BORINGSSL_LIKE
    size_t numalts;
    size_t i;
#లేకపోతే
    పూర్ణాంకం numalts;
    పూర్ణాంకం i;
#అయితే_ముగింపు

    /* get amount of alternatives, RFC2459 claims there MUST be at least
       one, but we do not depend on it... */
    numalts = sk_GENERAL_NAME_అంకె(altnames);

    /* loop through all alternatives - until a dnsmatch */
    కొరకు(i = 0; (i < numalts) && !matched; i++) {
      /* get a handle to alternative name number i */
      స్థిరరాశి GENERAL_NAME *check = sk_GENERAL_NAME_విలువ(altnames, i);

      ఒకవేళ(check->type == GEN_DNS)
        dNSName = TRUE;
      లేకపోతే ఒకవేళ(check->type == GEN_IPADD)
        iPAddress = TRUE;

      /* only check alternatives of the same type the target is */
      ఒకవేళ(check->type == లక్ష్యం) {
        /* get data and length */
        స్థిరరాశి అక్షరం *altptr = (స్థిరరాశి అక్షరం *)ASN1_STRING_get0_దత్తాంశం(check->d.ia5);
        size_t altlen = ossl_ASN1_STRING_get_నిడివి(check->d.ia5);

        ఎంపిక(లక్ష్యం) {
        స్థితి GEN_DNS: /* name/pattern comparison */
          ఒకవేళ(!memchr(altptr, '\0', altlen) &&
             Curl_cert_hostcheck(altptr, altlen,
                                 peer->origin->hostname, hostlen)) {
            matched = TRUE;
            infof(దత్తాంశం, "  subjectAltName: \"%s\" matches cert's \"%.*s\"",
                  peer->origin->user_hostname, (పూర్ణాంకం)altlen, altptr);
          }
          ఆపు;

        స్థితి GEN_IPADD: /* IP address comparison */
          /* compare alternative IP address if the data chunk is the same size
             our server IP address is */
          ఒకవేళ((altlen == addrlen) && !memcmp(altptr, &addr, altlen)) {
            matched = TRUE;
            infof(దత్తాంశం, "  subjectAltName: \"%s\" matches cert's IP address!",
                  peer->origin->user_hostname);
          }
          ఆపు;
        }
      }
    }
    GENERAL_పేర్లు_free(altnames);
  }

  ఒకవేళ(matched)
    /* an alternative name matched */
    ;
  లేకపోతే ఒకవేళ(dNSName || iPAddress) {
    స్థిరరాశి అక్షరం *tname = (peer->type == CURL_SSL_PEER_DNS) ? "hostname" :
                        (peer->type == CURL_SSL_PEER_IPV4) ?
                        "IPv4 address" : "IPv6 address";
    infof(దత్తాంశం, " subjectAltName does not match %s %s", tname,
          peer->origin->user_hostname);
    failf(దత్తాంశం, "SSL: no alternative certificate subject name matches "
          "target %s '%s'", tname, peer->origin->user_hostname);
    ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
  }
  లేకపోతే {
    /* we have to look to the last occurrence of a commonName in the
       distinguished one to get the most significant one. */
    పూర్ణాంకం i = -1;
    గుర్తులేని అక్షరం *cn = ఖాళీ;
    size_t cnlen = 0;
    bool free_cn = FALSE;

    /* The following is done because of a bug in 0.9.6b */
    స్థిరరాశి X509_NAME *name = X509_get_subject_name(server_cert);
    ఒకవేళ(name) {
      పూర్ణాంకం j;
      అయితే((j = X509_NAME_get_సూచీ_by_NID(name, NID_commonName, i)) >= 0)
        i = j;
    }

    /* we have the name entry and we now convert this to a string
       that we can use for comparison. Doing this we support BMPstring,
       UTF8, etc. */

    ఒకవేళ(i >= 0) {
      స్థిరరాశి ASN1_STRING *tmp =
        X509_NAME_నమోదు_get_దత్తాంశం(X509_NAME_get_నమోదు(name, i));

      /* In OpenSSL 0.9.7d and earlier, ASN1_STRING_to_UTF8 fails if the input
         is already UTF-8 encoded. We check for this case and copy the raw
         string manually to avoid the problem. This code can be made
         conditional in the future when OpenSSL has been fixed. */
      ఒకవేళ(tmp) {
        ఒకవేళ(ASN1_STRING_type(tmp) == V_ASN1_UTF8STRING) {
          cnlen = ossl_ASN1_STRING_get_నిడివి(tmp);
          cn = (గుర్తులేని అక్షరం *)CURL_UNCONST(ASN1_STRING_get0_దత్తాంశం(tmp));
        }
        లేకపోతే { /* not a UTF8 name */
          స్థిరరాశి పూర్ణాంకం utf8len = ASN1_STRING_గమ్యం_UTF8(&cn, tmp);
          ఒకవేళ(utf8len < 0)
            cnlen = 0;
          లేకపోతే
            cnlen = (size_t)utf8len;
          free_cn = TRUE;
        }

        ఒకవేళ((cnlen == 0) || !cn)
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        లేకపోతే ఒకవేళ(memchr(cn, '\0', cnlen)) {
          /* there was a null-terminator before the end of string, this
             cannot match and we return failure! */
          failf(దత్తాంశం, "SSL: illegal cert name field");
          ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
        }
      }
    }

    ఒకవేళ(ఫలితం)
      /* error already detected, pass through */
      ;
    లేకపోతే ఒకవేళ(!cn) {
      failf(దత్తాంశం, "SSL: unable to obtain common name from peer certificate");
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    }
    లేకపోతే ఒకవేళ(!Curl_cert_hostcheck((స్థిరరాశి అక్షరం *)cn, cnlen,
                                 peer->origin->hostname, hostlen)) {
      failf(దత్తాంశం, "SSL: certificate subject name '%.*s' does not match "
            "target hostname '%s'", (పూర్ణాంకం)cnlen, cn,
            peer->origin->user_hostname);
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    }
    లేకపోతే {
      infof(దత్తాంశం, " common name: %.*s (matched)", (పూర్ణాంకం)cnlen, cn);
    }
    ఒకవేళ(free_cn)
      OPENSSL_free(cn);
  }

  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే OPENSSL_NO_OCSP
స్థిర CURLcode verifystatus(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం ossl_ctx *octx)
{
  పూర్ణాంకం i, ocsp_స్థితి;
#నిర్వచితమైతే HAVE_BORINGSSL_LIKE
  స్థిరరాశి uint8_t *స్థితి_2;
#లేకపోతే
  గుర్తులేని అక్షరం *స్థితి_2;
#అయితే_ముగింపు
  స్థిరరాశి గుర్తులేని అక్షరం *p;
  CURLcode ఫలితం = CURLE_OK;
  OCSP_RESPONSE *rsp = ఖాళీ;
  OCSP_BASICRESP *br = ఖాళీ;
  X509_STORE     *st = ఖాళీ;
  దొంతర_యొక్క(X509) *ch = ఖాళీ;
  X509 *cert;
  OCSP_CERTID *id = ఖాళీ;
  పూర్ణాంకం cert_స్థితి, crl_reason;
  ASN1_GENERALIZEDTIME *rev, *thisupd, *nextupd;
  పూర్ణాంకం ret;
  దీర్ఘ len;

  (శూన్య)cf;
  DEBUGASSERT(octx);

  len = (దీర్ఘ)SSL_get_tlsext_స్థితి_ocsp_resp(octx->ssl, &స్థితి_2);

  ఒకవేళ(!స్థితి_2) {
    failf(దత్తాంశం, "No OCSP response received");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }
  p = స్థితి_2;
  rsp = d2i_OCSP_RESPONSE(ఖాళీ, &p, len);
  ఒకవేళ(!rsp) {
    failf(దత్తాంశం, "Invalid OCSP response");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }

  ocsp_స్థితి = OCSP_response_స్థితి(rsp);
  ఒకవేళ(ocsp_స్థితి != OCSP_RESPONSE_స్థితి_SUCCESSFUL) {
    failf(దత్తాంశం, "Invalid OCSP response status: %s (%d)",
          OCSP_response_స్థితి_str(ocsp_స్థితి), ocsp_స్థితి);
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }

  br = OCSP_response_get1_basic(rsp);
  ఒకవేళ(!br) {
    failf(దత్తాంశం, "Invalid OCSP response");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }

#ఒకవేళ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_సంస్కరణ_సంఖ్య) && LIBRESSL_సంస్కరణ_సంఖ్య < 0x3060000fL)
  ch = SSL_get0_verified_chain(octx->ssl);
#లేకపోతే
  ch = SSL_get_peer_cert_chain(octx->ssl);
#అయితే_ముగింపు
  ఒకవేళ(!ch) {
    failf(దత్తాంశం, "Could not get peer certificate chain");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }
  st = SSL_CTX_get_cert_store(octx->ssl_ctx);

  ఒకవేళ(OCSP_basic_verify(br, ch, st, 0) <= 0) {
    failf(దత్తాంశం, "OCSP response verification failed");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }

  /* Compute the certificate's ID */
  cert = SSL_get1_peer_certificate(octx->ssl);
  ఒకవేళ(!cert) {
    failf(దత్తాంశం, "Error getting peer certificate");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }

  కొరకు(i = 0; i < (పూర్ణాంకం)sk_X509_అంకె(ch); i++) {
    X509 *issuer = sk_X509_విలువ(ch, (ossl_valsize_t)i);
    ఒకవేళ(X509_check_issued(issuer, cert) == X509_V_OK) {
      /* Note to analysis tools: using SHA1 here is fine. The `id`
       * generated is used as a hash lookup key, not as a verifier
       * of the OCSP data itself. This all according to RFC 5019. */
      id = OCSP_cert_గమ్యం_id(EVP_sha1(), cert, issuer);
      ఆపు;
    }
  }
  X509_free(cert);

  ఒకవేళ(!id) {
    failf(దత్తాంశం, "Error computing OCSP ID");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }

  /* Find the single OCSP response corresponding to the certificate ID */
  ret = OCSP_resp_కనుగొను_స్థితి(br, id, &cert_స్థితి, &crl_reason, &rev,
                              &thisupd, &nextupd);
  OCSP_CERTID_free(id);
  ఒకవేళ(ret != 1) {
    failf(దత్తాంశం, "Could not find certificate ID in OCSP response");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }

  /* Validate the OCSP response issuing and update times.
   * - `thisupd` is the time the OCSP response was issued
   * - `nextupd` is the time the OCSP response should be updated
   *    (valid life time assigned by the OCSP responder)
   * - 3rd param: how many seconds of clock skew we allow between
   *   our clock and the instance that issued the OCSP response
   * - 4th param: how many seconds in the past `thisupd` may be, with
   *   -1 meaning there is no limit. */
  ఒకవేళ(!OCSP_check_validity(thisupd, nextupd, 300L, -1L)) {
    failf(దత్తాంశం, "OCSP response has expired");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }

  infof(దత్తాంశం, "SSL certificate status: %s (%d)",
        OCSP_cert_స్థితి_str(cert_స్థితి), cert_స్థితి);

  ఎంపిక(cert_స్థితి) {
  స్థితి V_OCSP_CERTSTATUS_GOOD:
    ఆపు;

  స్థితి V_OCSP_CERTSTATUS_REVOKED:
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    failf(దత్తాంశం, "SSL certificate revocation reason: %s (%d)",
          OCSP_crl_reason_str(crl_reason), crl_reason);
    వెళ్ళు చివర;

  స్థితి V_OCSP_CERTSTATUS_UNKNOWN:
  అప్రమేయ:
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు చివర;
  }

చివర:
  ఒకవేళ(br)
    OCSP_BASICRESP_free(br);
  OCSP_RESPONSE_free(rsp);

  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

స్థిర స్థిరరాశి అక్షరం *ssl_సందేశం_type(పూర్ణాంకం ssl_ver, పూర్ణాంకం సందేశం)
{
  ఒకవేళ(ssl_ver == SSL3_సంస్కరణ_MAJOR) {
    ఎంపిక(సందేశం) {
    స్థితి SSL3_MT_HELLO_REQUEST:
      తిరిగి "Hello request";
    స్థితి SSL3_MT_CLIENT_HELLO:
      తిరిగి "Client hello";
    స్థితి SSL3_MT_SERVER_HELLO:
      తిరిగి "Server hello";
#నిర్వచితమైతే SSL3_MT_NEWSESSION_TICKET
    స్థితి SSL3_MT_NEWSESSION_TICKET:
      తిరిగి "Newsession Ticket";
#అయితే_ముగింపు
    స్థితి SSL3_MT_CERTIFICATE:
      తిరిగి "Certificate";
    స్థితి SSL3_MT_SERVER_కీలకం_EXCHANGE:
      తిరిగి "Server key exchange";
    స్థితి SSL3_MT_CLIENT_కీలకం_EXCHANGE:
      తిరిగి "Client key exchange";
    స్థితి SSL3_MT_CERTIFICATE_REQUEST:
      తిరిగి "Request CERT";
    స్థితి SSL3_MT_SERVER_పూర్తి:
      తిరిగి "Server finished";
    స్థితి SSL3_MT_CERTIFICATE_VERIFY:
      తిరిగి "CERT verify";
    స్థితి SSL3_MT_ముగిసిన:
      తిరిగి "Finished";
#నిర్వచితమైతే SSL3_MT_CERTIFICATE_స్థితి
    స్థితి SSL3_MT_CERTIFICATE_స్థితి:
      తిరిగి "Certificate Status";
#అయితే_ముగింపు
#నిర్వచితమైతే SSL3_MT_ENCRYPTED_EXTENSIONS
    స్థితి SSL3_MT_ENCRYPTED_EXTENSIONS:
      తిరిగి "Encrypted Extensions";
#అయితే_ముగింపు
#నిర్వచితమైతే SSL3_MT_SUPPLEMENTAL_దత్తాంశం
    స్థితి SSL3_MT_SUPPLEMENTAL_దత్తాంశం:
      తిరిగి "Supplemental data";
#అయితే_ముగింపు
#నిర్వచితమైతే SSL3_MT_చివర_యొక్క_EARLY_దత్తాంశం
    స్థితి SSL3_MT_చివర_యొక్క_EARLY_దత్తాంశం:
      తిరిగి "End of early data";
#అయితే_ముగింపు
#నిర్వచితమైతే SSL3_MT_కీలకం_UPDATE
    స్థితి SSL3_MT_కీలకం_UPDATE:
      తిరిగి "Key update";
#అయితే_ముగింపు
#నిర్వచితమైతే SSL3_MT_తదుపరి_PROTO
    స్థితి SSL3_MT_తదుపరి_PROTO:
      తిరిగి "Next protocol";
#అయితే_ముగింపు
#నిర్వచితమైతే SSL3_MT_సందేశం_HASH
    స్థితి SSL3_MT_సందేశం_HASH:
      తిరిగి "Message hash";
#అయితే_ముగింపు
    }
  }
  తిరిగి "Unknown";
}

స్థిర స్థిరరాశి అక్షరం *tls_rt_type(పూర్ణాంకం type)
{
  ఎంపిక(type) {
#నిర్వచితమైతే SSL3_RT_శీర్షిక
  స్థితి SSL3_RT_శీర్షిక:
    తిరిగి "TLS header";
#అయితే_ముగింపు
  స్థితి SSL3_RT_CHANGE_CIPHER_SPEC:
    తిరిగి "TLS change cipher";
  స్థితి SSL3_RT_ALERT:
    తిరిగి "TLS alert";
  స్థితి SSL3_RT_HANDSHAKE:
    తిరిగి "TLS handshake";
  స్థితి SSL3_RT_APPLICATION_దత్తాంశం:
    తిరిగి "TLS app data";
  అప్రమేయ:
    తిరిగి "TLS Unknown";
  }
}

/*
 * Our callback from the SSL/TLS layers.
 */
స్థిర శూన్య ossl_trace(పూర్ణాంకం direction, పూర్ణాంకం ssl_ver, పూర్ణాంకం content_type,
                       స్థిరరాశి శూన్య *buf, size_t len, SSL *ssl,
                       శూన్య *userp)
{
  స్థిరరాశి అక్షరం *verstr;
  నిర్మాణం Curl_cfilter *cf = userp;
  నిర్మాణం Curl_easy *దత్తాంశం = ఖాళీ;
  అక్షరం unknown[32];

  ఒకవేళ(!cf)
    తిరిగి;
  దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  ఒకవేళ(!దత్తాంశం || !దత్తాంశం->సమితి.fdebug || (direction && direction != 1))
    తిరిగి;

  ఎంపిక(ssl_ver) {
#నిర్వచితమైతే SSL3_సంస్కరణ
  స్థితి SSL3_సంస్కరణ:
    verstr = "SSLv3";
    ఆపు;
#అయితే_ముగింపు
  స్థితి TLS1_సంస్కరణ:
    verstr = "TLSv1.0";
    ఆపు;
#నిర్వచితమైతే TLS1_1_సంస్కరణ
  స్థితి TLS1_1_సంస్కరణ:
    verstr = "TLSv1.1";
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే TLS1_2_సంస్కరణ
  స్థితి TLS1_2_సంస్కరణ:
    verstr = "TLSv1.2";
    ఆపు;
#అయితే_ముగింపు
  స్థితి TLS1_3_సంస్కరణ:
    verstr = "TLSv1.3";
    ఆపు;
  అప్రమేయ:
    curl_msnprintf(unknown, పరిమాణం(unknown), "(%x)", (గుర్తులేని పూర్ణాంకం)ssl_ver);
    verstr = unknown;
    ఆపు;
  }

  /* Log progress for interesting records only (like Handshake or Alert), skip
   * all raw record headers (content_type == SSL3_RT_HEADER or ssl_ver == 0).
   * For TLS 1.3, skip notification of the decrypted inner Content-Type.
   */
  ఒకవేళ(ssl_ver
#నిర్వచితమైతే SSL3_RT_శీర్షిక
     && content_type != SSL3_RT_శీర్షిక
#అయితే_ముగింపు
#నిర్వచితమైతే SSL3_RT_INNER_CONTENT_TYPE
     && content_type != SSL3_RT_INNER_CONTENT_TYPE
#అయితే_ముగింపు
    ) {
    స్థిరరాశి అక్షరం *సందేశం_name = "Truncated message";
    స్థిరరాశి అక్షరం *tls_rt_name;
    అక్షరం ssl_buf[1024];
    పూర్ణాంకం సందేశం_type = 0;
    పూర్ణాంకం txt_len;

    /* the info given when the version is zero is not that useful for us */

    ssl_ver >>= 8; /* check the upper 8 bits only below */

    /* SSLv2 does not seem to have TLS record-type headers, so OpenSSL
     * always pass-up content-type as 0, but the interesting message-type
     * is at 'buf[0]'.
     */
    ఒకవేళ(ssl_ver == SSL3_సంస్కరణ_MAJOR && content_type)
      tls_rt_name = tls_rt_type(content_type);
    లేకపోతే
      tls_rt_name = "";

    ఒకవేళ(content_type == SSL3_RT_CHANGE_CIPHER_SPEC) {
      ఒకవేళ(len) {
        సందేశం_type = *(స్థిరరాశి గుర్తులేని అక్షరం *)buf;
        సందేశం_name = "Change cipher spec";
      }
    }
    లేకపోతే ఒకవేళ(content_type == SSL3_RT_ALERT) {
      ఒకవేళ(len >= 2) {
        సందేశం_type =
          (((స్థిరరాశి గుర్తులేని అక్షరం *)buf)[0] << 8) +
           ((స్థిరరాశి గుర్తులేని అక్షరం *)buf)[1];
        సందేశం_name = SSL_alert_desc_string_long(సందేశం_type);
      }
    }
    లేకపోతే ఒకవేళ(len) {
      సందేశం_type = *(స్థిరరాశి గుర్తులేని అక్షరం *)buf;
      సందేశం_name = ssl_సందేశం_type(ssl_ver, సందేశం_type);
    }

    txt_len = curl_msnprintf(ssl_buf, పరిమాణం(ssl_buf),
                             "%s (%s), %s, %s (%d):\n",
                             verstr, direction ? "OUT" : "IN",
                             tls_rt_name, సందేశం_name, సందేశం_type);
    Curl_debug(దత్తాంశం, CURLINFO_పాఠ్యం, ssl_buf, (size_t)txt_len);
  }

  Curl_debug(దత్తాంశం, (direction == 1) ? CURLINFO_SSL_దత్తాంశం_బహిర్గతం :
             CURLINFO_SSL_దత్తాంశం_లోపల, (స్థిరరాశి అక్షరం *)buf, len);
  (శూన్య)ssl;
}

స్థిర CURLcode ossl_సమితి_ssl_సంస్కరణ_min_max(నిర్మాణం Curl_cfilter *cf,
                                             SSL_CTX *ctx,
                                             గుర్తులేని పూర్ణాంకం ssl_సంస్కరణ_min)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* first, TLS min version... */
  దీర్ఘ curl_ssl_సంస్కరణ_min = (దీర్ఘ)ssl_సంస్కరణ_min;
  దీర్ఘ curl_ssl_సంస్కరణ_max;

  /* convert curl min SSL version option to OpenSSL constant */
#ఒకవేళ defined(HAVE_BORINGSSL_LIKE) || defined(LIBRESSL_సంస్కరణ_సంఖ్య)
  uint16_t ossl_ssl_సంస్కరణ_min = 0;
  uint16_t ossl_ssl_సంస్కరణ_max = 0;
#లేకపోతే
  దీర్ఘ ossl_ssl_సంస్కరణ_min = 0;
  దీర్ఘ ossl_ssl_సంస్కరణ_max = 0;
#అయితే_ముగింపు
  /* it cannot be default here */
  DEBUGASSERT(curl_ssl_సంస్కరణ_min != CURL_SSLVERSION_DEFAULT);
  ఎంపిక(curl_ssl_సంస్కరణ_min) {
  స్థితి CURL_SSLVERSION_TLSv1: /* TLS 1.x */
  స్థితి CURL_SSLVERSION_TLSv1_0:
    ossl_ssl_సంస్కరణ_min = TLS1_సంస్కరణ;
    ఆపు;
  స్థితి CURL_SSLVERSION_TLSv1_1:
    ossl_ssl_సంస్కరణ_min = TLS1_1_సంస్కరణ;
    ఆపు;
  స్థితి CURL_SSLVERSION_TLSv1_2:
    ossl_ssl_సంస్కరణ_min = TLS1_2_సంస్కరణ;
    ఆపు;
  స్థితి CURL_SSLVERSION_TLSv1_3:
    ossl_ssl_సంస్కరణ_min = TLS1_3_సంస్కరణ;
    ఆపు;
  }

  /* ... then, TLS max version */
  curl_ssl_సంస్కరణ_max = (దీర్ఘ)conn_config->సంస్కరణ_max;

  /* convert curl max SSL version option to OpenSSL constant */
  ఎంపిక(curl_ssl_సంస్కరణ_max) {
  స్థితి CURL_SSLVERSION_MAX_TLSv1_0:
    ossl_ssl_సంస్కరణ_max = TLS1_సంస్కరణ;
    ఆపు;
  స్థితి CURL_SSLVERSION_MAX_TLSv1_1:
    ossl_ssl_సంస్కరణ_max = TLS1_1_సంస్కరణ;
    ఆపు;
  స్థితి CURL_SSLVERSION_MAX_TLSv1_2:
    ossl_ssl_సంస్కరణ_max = TLS1_2_సంస్కరణ;
    ఆపు;
  స్థితి CURL_SSLVERSION_MAX_TLSv1_3:
    ossl_ssl_సంస్కరణ_max = TLS1_3_సంస్కరణ;
    ఆపు;
  స్థితి CURL_SSLVERSION_MAX_NONE:  /* none selected */
  స్థితి CURL_SSLVERSION_MAX_DEFAULT:  /* max selected */
  అప్రమేయ:
    /* SSL_CTX_set_max_proto_version states that: setting the maximum to 0
       enables protocol versions up to the highest version supported by
       the library */
    ossl_ssl_సంస్కరణ_max = 0;
    ఆపు;
  }

  ఒకవేళ(!SSL_CTX_సమితి_min_proto_సంస్కరణ(ctx, ossl_ssl_సంస్కరణ_min) ||
     !SSL_CTX_సమితి_max_proto_సంస్కరణ(ctx, ossl_ssl_సంస్కరణ_max))
    తిరిగి CURLE_SSL_CONNECT_ERROR;

  తిరిగి CURLE_OK;
}

CURLcode Curl_ossl_కూడు_session(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ossl_ctx *octx,
                               స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                               SSL_SESSION *session,
                               స్థిరరాశి అక్షరం *alpn,
                               గుర్తులేని అక్షరం *quic_tp,
                               size_t quic_tp_len,
                               నిర్మాణం Curl_ssl_session **psession)
{
  నిర్మాణం Curl_ssl_session *sc_session = ఖాళీ, *sc_dup = ఖాళీ;
  గుర్తులేని అక్షరం *der_session_buf = ఖాళీ;
  గుర్తులేని అక్షరం *qtp_clone = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(psession)
    *psession = ఖాళీ;
  ఒకవేళ(!cf || !దత్తాంశం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(Curl_ssl_scache_use(cf, దత్తాంశం)) {
    size_t der_session_కొలత;
    గుర్తులేని అక్షరం *der_session_ptr;
    size_t earlydata_max = 0;
    పూర్ణాంకం ietf_tls_id = SSL_సంస్కరణ(octx->ssl);

    der_session_కొలత = i2d_SSL_SESSION(session, ఖాళీ);
    ఒకవేళ(der_session_కొలత == 0) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }

    der_session_buf = der_session_ptr = curlx_malloc(der_session_కొలత);
    ఒకవేళ(!der_session_buf) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }

    der_session_కొలత = i2d_SSL_SESSION(session, &der_session_ptr);
    ఒకవేళ(der_session_కొలత == 0) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }

#నిర్వచితమైతే HAVE_OPENSSL_EARLYDATA
    earlydata_max = SSL_SESSION_get_max_early_దత్తాంశం(session);
#అయితే_ముగింపు
    ఒకవేళ(quic_tp && quic_tp_len) {
      qtp_clone = curlx_memdup0((స్థిరరాశి అక్షరం *)quic_tp, quic_tp_len);
      ఒకవేళ(!qtp_clone) {
        ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
        వెళ్ళు బహిర్గతం;
      }
    }

    ఫలితం = Curl_ssl_session_create2(der_session_buf, der_session_కొలత,
                                      ietf_tls_id, alpn,
                                      (curl_off_t)సమయం(ఖాళీ) +
                                        SSL_SESSION_get_timeout(session),
                                      earlydata_max, qtp_clone, quic_tp_len,
                                      &sc_session);
    der_session_buf = ఖాళీ;  /* took ownership of sdata */
#నిర్వచితమైతే USE_APPLE_SECTRUST
    ఒకవేళ(!ఫలితం)
      sc_session->sectrust_verified = octx->sectrust_verified;
#అయితే_ముగింపు
    ఒకవేళ(!ఫలితం && psession &&  /* return a duplicate if asked for and FTP */
       (cf->conn->scheme->family == CURLPROTO_FTP)) {
        ఫలితం = Curl_ssl_session_dup(sc_session, &sc_dup);
    }
    ఒకవేళ(!ఫలితం) {
      ఫలితం = Curl_ssl_scache_put(cf, దత్తాంశం, ssl_peer_కీలకం, sc_session);
      /* took ownership of `sc_session` */
      sc_session = ఖాళీ;
    }
  }

బహిర్గతం:
  curlx_free(der_session_buf);
  ఒకవేళ(!ఫలితం && psession) {
    *psession = sc_dup;
    sc_dup = ఖాళీ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  తిరిగి ఫలితం;
}

/* The "new session" callback must return zero if the session can be removed
 * or non-zero if the session has been put into the session cache.
 */
స్థిర పూర్ణాంకం ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid)
{
  నిర్మాణం Curl_cfilter *cf = (నిర్మాణం Curl_cfilter *)SSL_get_app_దత్తాంశం(ssl);
  ఒకవేళ(cf) {
    నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
    నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
    నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
    నిర్మాణం Curl_ssl_session *session = ఖాళీ;
    Curl_ossl_కూడు_session(cf, దత్తాంశం, octx, connssl->peer.scache_కీలకం,
                          ssl_sessionid, connssl->negotiated.alpn, ఖాళీ,
                          0, &session);
    ఒకవేళ(session) { /* remember current TLS session */
      Curl_ssl_session_destroy(connssl->session);
      connssl->session = session;
    }
  }
  తిరిగి 0;
}

స్థిర CURLcode ఎక్కించు_cacert_from_జ్ఞాపకం(X509_STORE *store,
                                        స్థిరరాశి నిర్మాణం curl_blob *ca_info_blob)
{
  /* these need to be freed at the end */
  BIO *cbio = ఖాళీ;
  దొంతర_యొక్క(X509_INFO) *inf = ఖాళీ;

  /* everything else is a reference */
  పూర్ణాంకం i, లెక్క = 0;
  X509_INFO *itmp = ఖాళీ;

  ఒకవేళ(ca_info_blob->len > (size_t)గరిష్ఠ_పూర్ణాంకం)
    తిరిగి CURLE_SSL_CACERT_BADFILE;

  cbio = BIO_new_mem_buf(ca_info_blob->దత్తాంశం, (పూర్ణాంకం)ca_info_blob->len);
  ఒకవేళ(!cbio)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  inf = PEM_X509_INFO_read_bio(cbio, ఖాళీ, ఖాళీ, ఖాళీ);
  ఒకవేళ(!inf) {
    BIO_free(cbio);
    తిరిగి CURLE_SSL_CACERT_BADFILE;
  }

  /* add each entry from PEM file to x509_store */
  కొరకు(i = 0; i < (పూర్ణాంకం)sk_X509_INFO_అంకె(inf); ++i) {
    itmp = sk_X509_INFO_విలువ(inf, (ossl_valsize_t)i);
    ఒకవేళ(itmp->x509) {
      ఒకవేళ(X509_STORE_కూడు_cert(store, itmp->x509)) {
        ++లెక్క;
      }
      లేకపోతే {
        /* set count to 0 to return an error */
        లెక్క = 0;
        ఆపు;
      }
    }
    ఒకవేళ(itmp->crl) {
      ఒకవేళ(X509_STORE_కూడు_crl(store, itmp->crl)) {
        ++లెక్క;
      }
      లేకపోతే {
        /* set count to 0 to return an error */
        లెక్క = 0;
        ఆపు;
      }
    }
  }

#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic నెట్టు
#సూచన clang diagnostic ignored "-Wcast-function-type-strict"
#అయితే_ముగింపు
  sk_X509_INFO_తీయి_free(inf, X509_INFO_free);
#ఒకవేళ defined(__clang__) && __clang_major__ >= 16
#సూచన clang diagnostic తీయి
#అయితే_ముగింపు
  BIO_free(cbio);

  /* if we did not end up importing anything, treat that as an error */
  తిరిగి (లెక్క > 0) ? CURLE_OK : CURLE_SSL_CACERT_BADFILE;
}

#నిర్వచితమైతే USE_WIN32_CRYPTO
స్థిర CURLcode ossl_win_ఎక్కించు_store(నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం Curl_cfilter *cf,
                                    స్థిరరాశి అక్షరం *win_store,
                                    X509_STORE *store,
                                    bool *padded)
{
  CURLcode ఫలితం = CURLE_OK;
  HCERTSTORE hStore;

  *padded = FALSE;

  hStore = CertOpenSystemStoreA(0, win_store);
  ఒకవేళ(hStore) {
    PCCERT_CONTEXT pContext = ఖాళీ;
    /* The array of enhanced key usage OIDs varies per certificate and
       is declared outside of the loop so that rather than malloc/free each
       iteration we can grow it with realloc, when necessary. */
    CERT_ENHKEY_USAGE *enhkey_usage = ఖాళీ;
    DWORD enhkey_usage_కొలత = 0;
    VERBOSE(size_t మొత్తం = 0);
    VERBOSE(size_t imported = 0);

    /* This loop makes a best effort to import all valid certificates from
       the MS root store. If a certificate cannot be imported it is
       skipped. 'result' is used to store only hard-fail conditions (such
       as out of memory) that cause an early break. */
    ఫలితం = CURLE_OK;
    కొరకు(;;) {
      X509 *x509;
      FILETIME now;
      BYTE కీలకం_usage[2];
      DWORD req_కొలత;
      స్థిరరాశి గుర్తులేని అక్షరం *encoded_cert;
      pContext = Cert_Enum_Certificates_లోపల_Store(hStore, pContext);
      ఒకవేళ(!pContext)
        ఆపు;

      VERBOSE(++మొత్తం);

#ఒకవేళ defined(DEBUGBUILD) && defined(CURLVERBOSE)
      {
        అక్షరం cert_name[256];
        ఒకవేళ(!CertGetNameStringA(pContext, CERT_NAME_SIMPLE_DISPLAY_TYPE, 0,
                               ఖాళీ, cert_name, పరిమాణం(cert_name)))
          infof(దత్తాంశం, "SSL: unknown cert name");
        లేకపోతే
          infof(దత్తాంశం, "SSL: Checking cert \"%s\"", cert_name);
      }
#అయితే_ముగింపు
      encoded_cert = (స్థిరరాశి గుర్తులేని అక్షరం *)pContext->pbCertEncoded;
      ఒకవేళ(!encoded_cert)
        కొనసాగు;

      Get_System_సమయం_As_File_సమయం(&now);
      ఒకవేళ(Compare_File_సమయం(&pContext->pCertInfo->NotBefore, &now) > 0 ||
         Compare_File_సమయం(&now, &pContext->pCertInfo->NotAfter) > 0)
        కొనసాగు;

      /* If key usage exists check for signing attribute */
      ఒకవేళ(Cert_Get_Intended_కీలకం_Usage(pContext->dwCertEncodingType,
                                 pContext->pCertInfo,
                                 కీలకం_usage, పరిమాణం(కీలకం_usage))) {
        ఒకవేళ(!(కీలకం_usage[0] & CERT_కీలకం_CERT_SIGN_కీలకం_USAGE))
          కొనసాగు;
      }
      లేకపోతే ఒకవేళ(Get_చివరి_Error())
        కొనసాగు;

      /* If enhanced key usage exists check for server auth attribute.
       *
       * Note "In a Microsoft environment, a certificate might also have
       * EKU extended properties that specify valid uses for the
       * certificate."  The call below checks both, and behavior varies
       * depending on what is found. For more details see
       * CertGetEnhancedKeyUsage doc.
       */
      ఒకవేళ(Cert_Get_Enhanced_కీలకం_Usage(pContext, 0, ఖాళీ, &req_కొలత) && req_కొలత) {
        ఒకవేళ(req_కొలత > enhkey_usage_కొలత) {
          శూన్య *tmp = curlx_realloc(enhkey_usage, req_కొలత);

          ఒకవేళ(!tmp) {
            failf(దత్తాంశం, "SSL: Out of memory allocating for OID list");
            ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
            ఆపు;
          }

          enhkey_usage = (CERT_ENHKEY_USAGE *)tmp;
          enhkey_usage_కొలత = req_కొలత;
        }

        ఒకవేళ(Cert_Get_Enhanced_కీలకం_Usage(pContext, 0, enhkey_usage, &req_కొలత)) {
          ఒకవేళ(!enhkey_usage->cUsageIdentifier) {
            /* "If GetLastError returns CRYPT_E_NOT_FOUND, the certificate
               is good for all uses. If it returns zero, the certificate
               has no valid uses." */
            ఒకవేళ((HRESULT)Get_చివరి_Error() != CRYPT_E_NOT_దొరికింది)
              కొనసాగు;
          }
          లేకపోతే {
            DWORD i;
            bool దొరికింది = FALSE;

            కొరకు(i = 0; i < enhkey_usage->cUsageIdentifier; ++i) {
              ఒకవేళ(!strcmp("1.3.6.1.5.5.7.3.1" /* OID server auth */,
                         enhkey_usage->rgpszUsageIdentifier[i])) {
                దొరికింది = TRUE;
                ఆపు;
              }
            }

            ఒకవేళ(!దొరికింది)
              కొనసాగు;
          }
        }
        లేకపోతే
          కొనసాగు;
      }
      లేకపోతే
        కొనసాగు;

      x509 = d2i_X509(ఖాళీ, &encoded_cert, (దీర్ఘ)pContext->cbCertEncoded);
      ఒకవేళ(!x509)
        కొనసాగు;

      /* Try to import the certificate. This may fail for legitimate reasons
         such as duplicate certificate, which is allowed by MS but not
         OpenSSL. */
      ఒకవేళ(X509_STORE_కూడు_cert(store, x509) == 1) {
        VERBOSE(++imported);
#నిర్వచితమైతే DEBUGBUILD
        infof(దత్తాంశం, "SSL: Imported cert");
#అయితే_ముగింపు
        *padded = TRUE;
      }
      X509_free(x509);
    }

    curlx_free(enhkey_usage);
    CertFreeCertificateContext(pContext);
    CertCloseStore(hStore, 0);

    CURL_TRC_CF(దత్తాంశం, cf,
                "ossl_win_load_store() found: %zu imported: %zu certs in %s.",
                మొత్తం, imported, win_store);

    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode ossl_windows_ఎక్కించు_anchors(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం,
                                          X509_STORE *store,
                                          bool *padded)
{
  /* Import certificates from the Windows root certificate store if
     requested.
     https://stackoverflow.com/questions/9507184/
     https://github.com/d3x0r/SACK/blob/ff15424d3c581b86d40f818532e5a400c516d39d/src/netlib/ssl_layer.c#L1410
     https://datatracker.ietf.org/doc/html/rfc5280 */
  స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి win_stores[] = {
    "ROOT",   /* Trusted Root Certification Authorities */
    "CA"      /* Intermediate Certification Authorities */
  };
  size_t i;
  CURLcode ఫలితం = CURLE_OK;

  *padded = FALSE;
  కొరకు(i = 0; i < CURL_ARRAYSIZE(win_stores); ++i) {
    bool store_added = FALSE;
    ఫలితం = ossl_win_ఎక్కించు_store(దత్తాంశం, cf, win_stores[i], store, &store_added);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(store_added) {
      CURL_TRC_CF(దత్తాంశం, cf, "added trust anchors from Windows %s store",
                  win_stores[i]);
      *padded = TRUE;
    }
    లేకపోతే
      infof(దత్తాంశం, "error importing Windows %s store, continuing anyway",
            win_stores[i]);
  }
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* USE_WIN32_CRYPTO */

స్థిర CURLcode ossl_ఎక్కించు_trust_anchors(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం ossl_ctx *octx,
                                        X509_STORE *store)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (conn_config->ca_info_blob ? ఖాళీ : conn_config->CAfile);
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_capath = conn_config->CApath;
  bool have_native_check = FALSE;

  octx->store_is_ఖాళీలు = TRUE;
  ఒకవేళ(conn_config->native_ca_store) {
#నిర్వచితమైతే USE_WIN32_CRYPTO
    bool added = FALSE;
    ఫలితం = ossl_windows_ఎక్కించు_anchors(cf, దత్తాంశం, store, &added);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(added) {
      infof(దత్తాంశం, "  Native: Windows System Stores ROOT+CA");
      octx->store_is_ఖాళీలు = FALSE;
    }
#లేదావేళ defined(USE_APPLE_SECTRUST)
    infof(దత్తాంశం, "  Native: Apple SecTrust");
    have_native_check = TRUE;
#అయితే_ముగింపు
  }

  ఒకవేళ(conn_config->ca_info_blob) {
    ఫలితం = ఎక్కించు_cacert_from_జ్ఞాపకం(store, conn_config->ca_info_blob);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "error adding trust anchors from certificate blob: %d",
            (పూర్ణాంకం)ఫలితం);
      తిరిగి ఫలితం;
    }
    infof(దత్తాంశం, "  CA Blob from configuration");
    octx->store_is_ఖాళీలు = FALSE;
  }

  ఒకవేళ(ssl_cafile || ssl_capath) {
#నిర్వచితమైతే HAVE_OPENSSL3
    /* OpenSSL 3.0.0 has deprecated SSL_CTX_load_verify_locations */
    ఒకవేళ(ssl_cafile) {
      ఒకవేళ(!X509_STORE_ఎక్కించు_file(store, ssl_cafile)) {
        ఒకవేళ(octx->store_is_ఖాళీలు && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(దత్తాంశం, "error adding trust anchors from file: %s", ssl_cafile);
          తిరిగి CURLE_SSL_CACERT_BADFILE;
        }
        లేకపోతే
          infof(దత్తాంశం, "error setting certificate file, continuing anyway");
      }
      infof(దత్తాంశం, "  CAfile: %s", ssl_cafile);
      octx->store_is_ఖాళీలు = FALSE;
    }
    ఒకవేళ(ssl_capath) {
      ఒకవేళ(!X509_STORE_ఎక్కించు_మార్గం(store, ssl_capath)) {
        ఒకవేళ(octx->store_is_ఖాళీలు && !have_native_check) {
          /* Fail if we insist on successfully verifying the server. */
          failf(దత్తాంశం, "error adding trust anchors from path: %s", ssl_capath);
          తిరిగి CURLE_SSL_CACERT_BADFILE;
        }
        లేకపోతే
          infof(దత్తాంశం, "error setting certificate path, continuing anyway");
      }
      infof(దత్తాంశం, "  CApath: %s", ssl_capath);
      octx->store_is_ఖాళీలు = FALSE;
    }
#లేకపోతే
    /* tell OpenSSL where to find CA certificates that are used to verify the
       server's certificate. */
    ఒకవేళ(!X509_STORE_ఎక్కించు_locations(store, ssl_cafile, ssl_capath)) {
      ఒకవేళ(octx->store_is_ఖాళీలు && !have_native_check) {
        /* Fail if we insist on successfully verifying the server. */
        failf(దత్తాంశం, "error adding trust anchors from locations:"
              "  CAfile: %s CApath: %s",
              ssl_cafile ? ssl_cafile : "none",
              ssl_capath ? ssl_capath : "none");
        తిరిగి CURLE_SSL_CACERT_BADFILE;
      }
      లేకపోతే {
        infof(దత్తాంశం, "error setting certificate verify locations,"
              " continuing anyway");
      }
    }
    ఒకవేళ(ssl_cafile)
      infof(దత్తాంశం, "  CAfile: %s", ssl_cafile);
    ఒకవేళ(ssl_capath)
      infof(దత్తాంశం, "  CApath: %s", ssl_capath);
    octx->store_is_ఖాళీలు = FALSE;
#అయితే_ముగింపు
  }

#నిర్వచితమైతే CURL_CA_FALLBACK
  ఒకవేళ(octx->store_is_ఖాళీలు) {
    /* verifying the peer without any CA certificates does not
       work so use OpenSSL's built-in default as fallback */
    X509_STORE_సమితి_default_paths(store);
    infof(దత్తాంశం, "  OpenSSL default paths (fallback)");
    octx->store_is_ఖాళీలు = FALSE;
  }
#అయితే_ముగింపు
  ఒకవేళ(octx->store_is_ఖాళీలు && !have_native_check)
    infof(దత్తాంశం, "  no trust anchors configured");

  తిరిగి ఫలితం;
}

స్థిర CURLcode ossl_populate_x509_store(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         నిర్మాణం ossl_ctx *octx,
                                         X509_STORE *store)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ఫలితం = CURLE_OK;
  X509_LOOKUP *lookup = ఖాళీ;
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_crlfile = conn_config->CRLfile;
  గుర్తులేని దీర్ఘ x509flags = 0;

  CURL_TRC_CF(దత్తాంశం, cf, "configuring OpenSSL's x509 trust store");
  ఒకవేళ(!store)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(!conn_config->verifypeer) {
    infof(దత్తాంశం, "SSL Trust: peer verification disabled");
    తిరిగి CURLE_OK;
  }

  infof(దత్తాంశం, "SSL Trust Anchors:");
  ఫలితం = ossl_ఎక్కించు_trust_anchors(cf, దత్తాంశం, octx, store);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  /* Does not make sense to load a CRL file without peer verification */
#నిర్వచితమైతే USE_APPLE_SECTRUST
  ఒకవేళ(ssl_crlfile && conn_config->native_ca_store) {
    failf(దత్తాంశం, "openssl: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    తిరిగి CURLE_NOT_BUILT_లోపల;
  }
#అయితే_ముగింపు
  ఒకవేళ(ssl_crlfile) {
    /* tell OpenSSL where to find CRL file that is used to check certificate
     * revocation */
    lookup = X509_STORE_కూడు_lookup(store, X509_LOOKUP_file());
    ఒకవేళ(!lookup ||
       (!X509_ఎక్కించు_crl_file(lookup, ssl_crlfile, X509_FILETYPE_PEM))) {
      failf(దత్తాంశం, "error loading CRL file: %s", ssl_crlfile);
      తిరిగి CURLE_SSL_CRL_BADFILE;
    }
    x509flags = X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL;
    infof(దత్తాంశం, " CRLfile: %s", ssl_crlfile);
  }

  /* Try building a chain using issuers in the trusted store first to avoid
     problems with server-sent legacy intermediates. Newer versions of
     OpenSSL do alternate chain checking by default but we do not know how to
     determine that in a reliable manner.
     https://web.archive.org/web/20190422050538/rt.openssl.org/Ticket/Display.html?id=3621
   */
  x509flags |= X509_V_FLAG_TRUSTED_మొదటి;

  ఒకవేళ(!conn_config->no_partialchain && !ssl_crlfile) {
    /* Have intermediate certificates in the trust store be treated as
       trust-anchors, in the same way as self-signed root CA certificates are.
       This allows users to verify servers using the intermediate cert only,
       instead of needing the whole chain.

       Due to OpenSSL bug https://github.com/openssl/openssl/issues/5081 we
       cannot do partial chains with a CRL check. */
    x509flags |= X509_V_FLAG_PARTIAL_CHAIN;
  }
  (శూన్య)X509_STORE_సమితి_flags(store, x509flags);

  తిరిగి ఫలితం;
}

/* key to use at `multi->proto_hash` */
#నిర్వచించు MPROTO_OSSL_X509_కీలకం  "tls:ossl:x509:share"

నిర్మాణం ossl_x509_share {
  అక్షరం *CAfile;         /* CAfile path used to generate X509 store */
  అక్షరం *CApath;         /* CApath path used to generate X509 store */
  X509_STORE *store;    /* cached X509 store or NULL if none */
  నిర్మాణం curltime సమయం; /* when the cached store was created */
  బిట్టు(store_is_ఖాళీలు);  /* no certs/paths/blobs are in the store */
  బిట్టు(no_partialchain); /* keep partial chain state */
};

స్థిర శూన్య oss_x509_share_free(స్థిరరాశి శూన్య *కీలకం, size_t కీలకం_len, శూన్య *p)
{
  నిర్మాణం ossl_x509_share *share = p;
  DEBUGASSERT(కీలకం_len == CURL_CSTRLEN(MPROTO_OSSL_X509_కీలకం));
  DEBUGASSERT(!memcmp(MPROTO_OSSL_X509_కీలకం, కీలకం, కీలకం_len));
  (శూన్య)కీలకం;
  (శూన్య)కీలకం_len;
  ఒకవేళ(share->store) {
    X509_STORE_free(share->store);
  }
  curlx_free(share->CAfile);
  curlx_free(share->CApath);
  curlx_free(share);
}

స్థిర bool ossl_cached_x509_store_expired(నిర్మాణం Curl_easy *దత్తాంశం,
                                           స్థిరరాశి నిర్మాణం ossl_x509_share *mb)
{
  ఒకవేళ(దత్తాంశం->సమితి.ssl_ca_cache_timeout < 0)
    తిరిగి FALSE;
  లేకపోతే {
    timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &mb->సమయం);
    timediff_t timeout_ms = దత్తాంశం->సమితి.ssl_ca_cache_timeout * (timediff_t)1000;

    తిరిగి elapsed_ms >= timeout_ms;
  }
}

స్థిర bool ossl_cached_x509_store_different(నిర్మాణం Curl_cfilter *cf,
                                             స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం,
                                             స్థిరరాశి నిర్మాణం ossl_x509_share *mb)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  (శూన్య)దత్తాంశం;
  ఒకవేళ(mb->no_partialchain != conn_config->no_partialchain)
    తిరిగి TRUE;
  ఒకవేళ(!mb->CAfile || !conn_config->CAfile) {
    ఒకవేళ(mb->CAfile != conn_config->CAfile)
      తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(strcmp(mb->CAfile, conn_config->CAfile))
    తిరిగి TRUE;

  ఒకవేళ(!mb->CApath || !conn_config->CApath) {
    ఒకవేళ(mb->CApath != conn_config->CApath)
      తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(strcmp(mb->CApath, conn_config->CApath))
    తిరిగి TRUE;

  తిరిగి FALSE;
}

స్థిర X509_STORE *ossl_get_cached_x509_store(నిర్మాణం Curl_cfilter *cf,
                                              నిర్మాణం Curl_easy *దత్తాంశం,
                                              bool *pempty)
{
  నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;
  నిర్మాణం ossl_x509_share *share;
  X509_STORE *store = ఖాళీ;

  DEBUGASSERT(multi);
  *pempty = TRUE;
  share = multi ? Curl_hash_pick(&multi->proto_hash,
                                 MPROTO_OSSL_X509_కీలకం,
                                 CURL_CSTRLEN(MPROTO_OSSL_X509_కీలకం)) : ఖాళీ;
  ఒకవేళ(share && share->store &&
     !ossl_cached_x509_store_expired(దత్తాంశం, share) &&
     !ossl_cached_x509_store_different(cf, దత్తాంశం, share)) {
    store = share->store;
    *pempty = (bool)share->store_is_ఖాళీలు;
  }

  తిరిగి store;
}

స్థిర శూన్య ossl_సమితి_cached_x509_store(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       X509_STORE *store,
                                       bool is_ఖాళీలు)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం Curl_multi *multi = దత్తాంశం->multi;
  నిర్మాణం ossl_x509_share *share;

  DEBUGASSERT(multi);
  ఒకవేళ(!multi)
    తిరిగి;
  share = Curl_hash_pick(&multi->proto_hash,
                         MPROTO_OSSL_X509_కీలకం,
                         CURL_CSTRLEN(MPROTO_OSSL_X509_కీలకం));

  ఒకవేళ(!share) {
    share = curlx_calloc(1, పరిమాణం(*share));
    ఒకవేళ(!share)
      తిరిగి;
    ఒకవేళ(!Curl_hash_add2(&multi->proto_hash,
                       MPROTO_OSSL_X509_కీలకం,
                       CURL_CSTRLEN(MPROTO_OSSL_X509_కీలకం),
                       share, oss_x509_share_free)) {
      curlx_free(share);
      తిరిగి;
    }
  }

  ఒకవేళ(X509_STORE_పైకి_ref(store)) {
    అక్షరం *CAfile = ఖాళీ;
    అక్షరం *CApath = ఖాళీ;

    ఒకవేళ(conn_config->CAfile) {
      CAfile = curlx_strdup(conn_config->CAfile);
      ఒకవేళ(!CAfile) {
        X509_STORE_free(store);
        తిరిగి;
      }
    }

    ఒకవేళ(conn_config->CApath) {
      CApath = curlx_strdup(conn_config->CApath);
      ఒకవేళ(!CApath) {
        curlx_free(CAfile);
        X509_STORE_free(store);
        తిరిగి;
      }
    }

    ఒకవేళ(share->store) {
      X509_STORE_free(share->store);
      curlx_free(share->CAfile);
      curlx_free(share->CApath);
    }

    share->సమయం = *Curl_pgrs_now(దత్తాంశం);
    share->store = store;
    share->store_is_ఖాళీలు = is_ఖాళీలు;
    share->CAfile = CAfile;
    share->CApath = CApath;
    share->no_partialchain = conn_config->no_partialchain;
  }
}

#నిర్వచితమైతే OPENSSL_HAS_PROVIDERS
/* CApath certificates loaded under a provider libctx can outlive that libctx
   when the store is shared with the multi handle. */
#నిర్వచించు USING_PROVIDER(octx) octx->p.libctx
#లేకపోతే
#నిర్వచించు USING_PROVIDER(x) 0
#అయితే_ముగింపు

CURLcode Curl_ssl_అమరిక_x509_store(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ossl_ctx *octx)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ఫలితం = CURLE_OK;
  X509_STORE *cached_store;
  bool cache_criteria_met, is_ఖాళీలు;

  /* Consider the X509 store cacheable if it comes from CAfile/CApath,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (దత్తాంశం->సమితి.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !USING_PROVIDER(octx);

  ERR_సమితి_mark();

  cached_store = ossl_get_cached_x509_store(cf, దత్తాంశం, &is_ఖాళీలు);
  ఒకవేళ(cached_store && cache_criteria_met && X509_STORE_పైకి_ref(cached_store)) {
    SSL_CTX_సమితి_cert_store(octx->ssl_ctx, cached_store);
    octx->store_is_ఖాళీలు = is_ఖాళీలు;
  }
  లేకపోతే {
    X509_STORE *store = SSL_CTX_get_cert_store(octx->ssl_ctx);

    ఫలితం = ossl_populate_x509_store(cf, దత్తాంశం, octx, store);
    ఒకవేళ(ఫలితం == CURLE_OK && cache_criteria_met) {
      ossl_సమితి_cached_x509_store(cf, దత్తాంశం, store, (bool)octx->store_is_ఖాళీలు);
    }
  }

  ERR_తీయి_గమ్యం_mark();

  తిరిగి ఫలితం;
}

స్థిర bool ossl_apply_session(
  నిర్మాణం ossl_ctx *octx,
  నిర్మాణం Curl_cfilter *cf,
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం alpn_spec *alpns,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb,
  నిర్మాణం Curl_ssl_session *scs)
{
  నిర్మాణం ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  స్థిరరాశి గుర్తులేని అక్షరం *der_sessionid = scs->sdata;
  size_t der_sessionid_కొలత = scs->sdata_len;
  SSL_SESSION *ssl_session = ఖాళీ;

  /* If OpenSSL does not accept the session from the cache, this
   * is not an error. We continue without it. */
  ssl_session = d2i_SSL_SESSION(ఖాళీ, &der_sessionid,
                                (దీర్ఘ)der_sessionid_కొలత);
  ఒకవేళ(ssl_session) {
    ఒకవేళ(!SSL_సమితి_session(octx->ssl, ssl_session)) {
      VERBOSE(అక్షరం error_బఫరు[256]);
      infof(దత్తాంశం, "SSL: SSL_set_session not accepted, "
            "continuing without: %s",
            ossl_strerror(ERR_get_error(), error_బఫరు,
                          పరిమాణం(error_బఫరు)));
    }
    లేకపోతే {
      ఒకవేళ(conn_cfg->verifypeer &&
         (SSL_get_verify_ఫలితం(octx->ssl) != X509_V_OK)
#నిర్వచితమైతే USE_APPLE_SECTRUST
         /* if sectrust is used and verified the session before */
         && (!conn_cfg->native_ca_store || !scs->sectrust_verified)
#అయితే_ముగింపు
        ) {
        /* Session was from unverified connection, cannot reuse here */
        SSL_సమితి_session(octx->ssl, ఖాళీ);
        infof(దత్తాంశం, "SSL session not peer verified, not reusing");
      }
      లేకపోతే {
        infof(దత్తాంశం, "SSL reusing session with ALPN '%s'",
              scs->alpn ? scs->alpn : "-");
        octx->reused_session = TRUE;
#నిర్వచితమైతే USE_APPLE_SECTRUST
        octx->sectrust_session = scs->sectrust_verified;
#అయితే_ముగింపు
        infof(దత్తాంశం, "SSL verify result: %lx",
              (గుర్తులేని దీర్ఘ)SSL_get_verify_ఫలితం(octx->ssl));
#నిర్వచితమైతే HAVE_OPENSSL_EARLYDATA
        ఒకవేళ(conn_cfg->earlydata && scs->alpn &&
           SSL_SESSION_get_max_early_దత్తాంశం(ssl_session) &&
           !cf->conn->బిట్లు.connect_only &&
           (SSL_సంస్కరణ(octx->ssl) == TLS1_3_సంస్కరణ)) {
          bool do_early_దత్తాంశం = FALSE;
          ఒకవేళ(sess_reuse_cb)
            (శూన్య)sess_reuse_cb(cf, దత్తాంశం, alpns, scs, &do_early_దత్తాంశం);
          ఒకవేళ(do_early_దత్తాంశం) {
            /* We only try the ALPN protocol the session used before,
             * otherwise we might send early data for the wrong protocol */
            Curl_alpn_restrict_గమ్యం(alpns, scs->alpn);
          }
        }
#లేకపోతే
        (శూన్య)alpns;
        (శూన్య)conn_cfg;
        (శూన్య)sess_reuse_cb;
#అయితే_ముగింపు
      }
    }
    SSL_SESSION_free(ssl_session);
  }
  లేకపోతే {
    infof(దత్తాంశం, "SSL session not accepted by OpenSSL, continuing without");
  }
  తిరిగి (bool)octx->reused_session;
}

స్థిర CURLcode ossl_init_session_and_alpns(
  నిర్మాణం ossl_ctx *octx,
  నిర్మాణం Curl_cfilter *cf,
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం ssl_peer *peer,
  స్థిరరాశి నిర్మాణం alpn_spec *alpns_requested,
  Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  నిర్మాణం ssl_filter_config *conn_cfg = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం alpn_spec alpns;
  CURLcode ఫలితం;

  Curl_alpn_copy(&alpns, alpns_requested);

  octx->reused_session = FALSE;

  ఒకవేళ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    నిర్మాణం Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(దత్తాంశం, cf->cft, FIRSTSOCKET);
    ఒకవేళ(scs) {
      ఒకవేళ(ossl_apply_session(octx, cf, దత్తాంశం, &alpns, sess_reuse_cb, scs))
        CURL_TRC_CF(దత్తాంశం, cf, "applied SSL session from control connection");
    }
  }

  ఒకవేళ(!octx->reused_session &&
     Curl_ssl_scache_use(cf, దత్తాంశం) && !conn_cfg->verifystatus) {
    నిర్మాణం Curl_ssl_session *scs = ఖాళీ;

    ఫలితం = Curl_ssl_scache_take(cf, దత్తాంశం, peer->scache_కీలకం, &scs);
    ఒకవేళ(!ఫలితం && scs && scs->sdata && scs->sdata_len) {
      (శూన్య)ossl_apply_session(octx, cf, దత్తాంశం, &alpns, sess_reuse_cb, scs);
    }
    Curl_ssl_scache_return(cf, దత్తాంశం, peer->scache_కీలకం, scs);
  }

  ఒకవేళ(alpns.లెక్క) {
    నిర్మాణం alpn_proto_buf proto;
    memset(&proto, 0, పరిమాణం(proto));
    ఫలితం = Curl_alpn_గమ్యం_proto_buf(&proto, &alpns);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Error determining ALPN");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
    ఒకవేళ(SSL_సమితి_alpn_protos(octx->ssl, proto.దత్తాంశం, proto.len)) {
      failf(దత్తాంశం, "Error setting ALPN");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }

  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే HAVE_SSL_SET1_ECH_CONFIG_LIST
స్థిర CURLcode ossl_init_ech(నిర్మాణం ossl_ctx *octx,
                              నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ssl_peer *peer)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  పూర్ణాంకం trying_ech_now = 0;

  ఒకవేళ(!Curl_ssl_ech_enabled(cf))
    తిరిగి CURLE_OK;

  ఒకవేళ(conn_config->ech == CURLECH_GREASE) {
    infof(దత్తాంశం, "ECH: will GREASE ClientHello");
#నిర్వచితమైతే HAVE_BORINGSSL_LIKE
    SSL_సమితి_enable_ech_grease(octx->ssl, 1);
#లేకపోతే
    SSL_సమితి_options(octx->ssl, SSL_OP_ECH_GREASE);
#అయితే_ముగింపు
  }
  లేకపోతే ఒకవేళ(conn_config->ech && conn_config->ech_config) {
#నిర్వచితమైతే HAVE_BORINGSSL_LIKE
    uint8_t *ech_config;
    size_t ech_config_len = 0;
    CURLcode ఫలితం;

    ఒకవేళ(!conn_config->ech_config[0]) {
      infof(దత్తాంశం, "ECH: ECHConfig from command line empty");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = 2 * అక్షరశ్రేణి_పొడవు(conn_config->ech_config);
    ఫలితం = curlx_base64_decode(conn_config->ech_config,
                                 &ech_config, &ech_config_len);
    ఒకవేళ(ఫలితం || !ech_config) {
      infof(దత్తాంశం, "ECH: cannot base64 decode ECHConfig from command line");
      ఒకవేళ(conn_config->ech == CURLECH_HARD)
        తిరిగి ఫలితం;
    }
    ఒకవేళ(SSL_set1_ech_config_list(octx->ssl, ech_config, ech_config_len) != 1) {
      infof(దత్తాంశం, "ECH: SSL_ECH_set1_ech_config_list failed");
      ఒకవేళ(conn_config->ech == CURLECH_HARD) {
        curlx_free(ech_config);
        తిరిగి CURLE_SSL_CONNECT_ERROR;
      }
    }
    curlx_free(ech_config);
    trying_ech_now = 1;
#లేకపోతే
    size_t ech_config_len = 0;
    ఒకవేళ(!conn_config->ech_config[0]) {
      infof(దత్తాంశం, "ECH: ECHConfig from command line empty");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
    ech_config_len = అక్షరశ్రేణి_పొడవు(conn_config->ech_config);
    ఒకవేళ(SSL_set1_ech_config_list(octx->ssl,
                                (స్థిరరాశి uint8_t *)conn_config->ech_config,
                                ech_config_len) != 1) {
      infof(దత్తాంశం, "ECH: SSL_ECH_set1_ech_config_list failed");
      ఒకవేళ(conn_config->ech == CURLECH_HARD)
        తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
    లేకపోతే
      trying_ech_now = 1;
#అయితే_ముగింపు /* HAVE_BORINGSSL_LIKE */
    infof(దత్తాంశం, "ECH: ECHConfig from command line");
  }
  లేకపోతే {
    స్థిరరాశి నిర్మాణం Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(దత్తాంశం, cf->sockindex, peer->origin);

    ఒకవేళ(Curl_httpsrr_is_for_peer(peer->origin, rinfo) && rinfo->echconfiglist) {
      స్థిరరాశి గుర్తులేని అక్షరం *ecl = rinfo->echconfiglist;
      size_t elen = rinfo->echconfiglist_len;

      infof(దత్తాంశం, "ECH: ECHConfig from HTTPS RR");
      ఒకవేళ(SSL_set1_ech_config_list(octx->ssl, ecl, elen) != 1) {
        infof(దత్తాంశం, "ECH: SSL_set1_ech_config_list failed");
        ఒకవేళ(conn_config->ech == CURLECH_HARD)
          తిరిగి CURLE_SSL_CONNECT_ERROR;
      }
      లేకపోతే {
        trying_ech_now = 1;
        infof(దత్తాంశం, "ECH: imported ECHConfigList of length %zu", elen);
      }
    }
    లేకపోతే {
      infof(దత్తాంశం, "ECH: requested but no ECHConfig available");
      ఒకవేళ(conn_config->ech == CURLECH_HARD)
        తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }
#నిర్వచితమైతే HAVE_BORINGSSL_LIKE
  (శూన్య)peer;
  ఒకవేళ(trying_ech_now && conn_config->ech_ప్రజా) {
    infof(దత్తాంశం, "ECH: setting public_name not supported with BoringSSL");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }
#లేకపోతే
  ఒకవేళ(trying_ech_now && conn_config->ech_ప్రజా) {
    పూర్ణాంకం ret;
    infof(దత్తాంశం, "ECH: inner: '%s', outer: '%s'",
          peer->origin->hostname ? peer->origin->hostname : "NULL",
          conn_config->ech_ప్రజా);
    ret = SSL_ech_set1_server_పేర్లు(octx->ssl,
                                    peer->origin->hostname,
                                    conn_config->ech_ప్రజా,
                                    0 /* do send outer */);
    ఒకవేళ(ret != 1) {
      infof(దత్తాంశం, "ECH: rv failed to set server name(s) %d [ERROR]", ret);
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }
#అయితే_ముగింపు /* HAVE_BORINGSSL_LIKE */
  ఒకవేళ(trying_ech_now &&
     SSL_సమితి_min_proto_సంస్కరణ(octx->ssl, TLS1_3_సంస్కరణ) != 1) {
    infof(దత్తాంశం, "ECH: cannot force TLSv1.3 [ERROR]");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* else HAVE_SSL_SET1_ECH_CONFIG_LIST */

స్థిర CURLcode ossl_init_ssl(నిర్మాణం ossl_ctx *octx,
                              నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం ssl_peer *peer,
                              స్థిరరాశి నిర్మాణం alpn_spec *alpns_requested,
                              శూన్య *ssl_user_దత్తాంశం,
                              Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  /* Let's make an SSL structure */
  ఒకవేళ(octx->ssl)
    SSL_free(octx->ssl);
  octx->ssl = SSL_new(octx->ssl_ctx);
  ఒకవేళ(!octx->ssl) {
    failf(దత్తాంశం, "SSL: could not create a context (handle)");
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  SSL_సమితి_app_దత్తాంశం(octx->ssl, ssl_user_దత్తాంశం);

#నిర్వచితం_కాకపోతే OPENSSL_NO_OCSP
  ఒకవేళ(Curl_ssl_cf_get_filter_config(cf)->verifystatus)
    SSL_సమితి_tlsext_స్థితి_type(octx->ssl, TLSEXT_STATUSTYPE_ocsp);
#అయితే_ముగింపు

  SSL_సమితి_connect_state(octx->ssl);

  ఒకవేళ(peer->sni) {
    ఒకవేళ(!SSL_సమితి_tlsext_host_name(octx->ssl, peer->sni)) {
      failf(దత్తాంశం, "Failed set SNI");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }

#నిర్వచితమైతే HAVE_SSL_SET1_ECH_CONFIG_LIST
  {
    CURLcode ఫలితం = ossl_init_ech(octx, cf, దత్తాంశం, peer);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
#అయితే_ముగింపు /* HAVE_SSL_SET1_ECH_CONFIG_LIST */

  తిరిగి ossl_init_session_and_alpns(octx, cf, దత్తాంశం, peer,
                                     alpns_requested, sess_reuse_cb);
}

స్థిర CURLcode ossl_init_పద్ధతి(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ssl_peer *peer,
                                 స్థిరరాశి SSL_పద్ధతి **pmethod,
                                 గుర్తులేని పూర్ణాంకం *pssl_సంస్కరణ_min)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  *pmethod = ఖాళీ;
  *pssl_సంస్కరణ_min = conn_config->సంస్కరణ_2;
  DEBUGASSERT(conn_config->సంస్కరణ_2 != CURL_SSLVERSION_DEFAULT);
  ఎంపిక(peer->transport) {
  స్థితి TRNSPRT_TCP:
    /* check to see if we have been told to use an explicit SSL/TLS version */
    ఎంపిక(*pssl_సంస్కరణ_min) {
    స్థితి CURL_SSLVERSION_TLSv1:
    స్థితి CURL_SSLVERSION_TLSv1_0:
    స్థితి CURL_SSLVERSION_TLSv1_1:
    స్థితి CURL_SSLVERSION_TLSv1_2:
    స్థితి CURL_SSLVERSION_TLSv1_3:
      /* it is handled later with the context options */
      *pmethod = TLS_client_పద్ధతి();
      ఆపు;
    స్థితి CURL_SSLVERSION_SSLv2:
      failf(దత్తాంశం, "No SSLv2 support");
      తిరిగి CURLE_NOT_BUILT_లోపల;
    స్థితి CURL_SSLVERSION_SSLv3:
      failf(దత్తాంశం, "No SSLv3 support");
      తిరిగి CURLE_NOT_BUILT_లోపల;
    అప్రమేయ:
      failf(దత్తాంశం, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
    ఆపు;
  స్థితి TRNSPRT_QUIC:
    *pssl_సంస్కరణ_min = CURL_SSLVERSION_TLSv1_3;
    ఒకవేళ(conn_config->సంస్కరణ_max &&
       (conn_config->సంస్కరణ_max != CURL_SSLVERSION_MAX_DEFAULT) &&
       (conn_config->సంస్కరణ_max != CURL_SSLVERSION_MAX_TLSv1_3)) {
      failf(దత్తాంశం, "QUIC needs at least TLS version 1.3");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }

    *pmethod = TLS_పద్ధతి();
    ఆపు;
  అప్రమేయ:
    failf(దత్తాంశం, "unsupported transport %d in SSL init", peer->transport);
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  తిరిగి *pmethod ? CURLE_OK : CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_ossl_ctx_init(నిర్మాణం ossl_ctx *octx,
                            నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ssl_peer *peer,
                            స్థిరరాశి నిర్మాణం alpn_spec *alpns_requested,
                            Curl_ossl_ctx_అమరిక_cb *cb_అమరిక,
                            శూన్య *cb_user_దత్తాంశం,
                            Curl_ossl_new_session_cb *cb_new_session,
                            శూన్య *ssl_user_దత్తాంశం,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *ciphers;
  స్థిరరాశి SSL_పద్ధతి *req_పద్ధతి = ఖాళీ;
  ctx_option_t ctx_options = 0;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, దత్తాంశం);
  అక్షరం * స్థిరరాశి ssl_cert = conn_config->clientcert;
  స్థిరరాశి నిర్మాణం curl_blob *ssl_cert_blob = conn_config->cert_blob;
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_cert_type = conn_config->cert_type;
  గుర్తులేని పూర్ణాంకం ssl_సంస్కరణ_min = CURL_SSLVERSION_DEFAULT;
  అక్షరం error_బఫరు[256];

  /* Make funny stuff to get random input */
  ఫలితం = ossl_విత్తనం(దత్తాంశం);
  ఒకవేళ(!ఫలితం) {
    ఒకవేళ(CURL_EASY_STR(దత్తాంశం, STRING_SSL_ENGINE))
      ఫలితం = ossl_సమితి_engine(దత్తాంశం, octx,
                               CURL_EASY_STR(దత్తాంశం, STRING_SSL_ENGINE));
    ఒకవేళ(!ఫలితం && దత్తాంశం->సమితి.engine_default)
      ఫలితం = ossl_సమితి_engine_default(దత్తాంశం, octx);
  }

  ఒకవేళ(!ఫలితం) {
    ssl_config->certverifyresult = !X509_V_OK;
    ఫలితం = ossl_init_పద్ధతి(cf, దత్తాంశం, peer, &req_పద్ధతి, &ssl_సంస్కరణ_min);
  }
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;
  DEBUGASSERT(req_పద్ధతి);

  DEBUGASSERT(!octx->ssl_ctx);
  octx->ssl_ctx =
#నిర్వచితమైతే OPENSSL_HAS_PROVIDERS
    octx->p.libctx ?
    SSL_CTX_new_ex(octx->p.libctx, octx->p.propq, req_పద్ధతి):
#అయితే_ముగింపు
    SSL_CTX_new(req_పద్ధతి);

  ఒకవేళ(!octx->ssl_ctx) {
    failf(దత్తాంశం, "SSL: could not create a context: %s",
          ossl_strerror(ERR_peek_error(), error_బఫరు, పరిమాణం(error_బఫరు)));
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(cb_అమరిక) {
    ఫలితం = cb_అమరిక(cf, దత్తాంశం, cb_user_దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(దత్తాంశం->సమితి.fdebug && దత్తాంశం->సమితి.verbose &&
     (peer->transport != TRNSPRT_QUIC)) {
    /* the SSL trace callback is only used for verbose logging;
     * QUIC connections use a different TLS record format that
     * ossl_trace cannot handle */
    SSL_CTX_సమితి_సందేశం_callback(octx->ssl_ctx, ossl_trace);
    SSL_CTX_సమితి_సందేశం_callback_arg(octx->ssl_ctx, cf);
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
  ఒకవేళ(!conn_config->enable_beast)
    ctx_options &= ~(ctx_option_t)SSL_OP_DONT_చొప్పించు_ఖాళీలు_FRAGMENTS;

  DEBUGASSERT(ssl_సంస్కరణ_min != CURL_SSLVERSION_DEFAULT);
  ఎంపిక(ssl_సంస్కరణ_min) {
  స్థితి CURL_SSLVERSION_SSLv2:
  స్థితి CURL_SSLVERSION_SSLv3:
    తిరిగి CURLE_NOT_BUILT_లోపల;

    /* "--tlsv<x.y>" options mean TLS >= version <x.y> */
  స్థితి CURL_SSLVERSION_TLSv1:   /* TLS >= version 1.0 */
  స్థితి CURL_SSLVERSION_TLSv1_0: /* TLS >= version 1.0 */
  స్థితి CURL_SSLVERSION_TLSv1_1: /* TLS >= version 1.1 */
  స్థితి CURL_SSLVERSION_TLSv1_2: /* TLS >= version 1.2 */
  స్థితి CURL_SSLVERSION_TLSv1_3: /* TLS >= version 1.3 */
    /* asking for any TLS version as the minimum, means no SSL versions
       allowed */
    ctx_options |= SSL_OP_NO_SSLv2;
    ctx_options |= SSL_OP_NO_SSLv3;

    ఫలితం = ossl_సమితి_ssl_సంస్కరణ_min_max(cf, octx->ssl_ctx, ssl_సంస్కరణ_min);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఆపు;

  అప్రమేయ:
    failf(దత్తాంశం, "Unrecognized parameter passed via CURLOPT_SSLVERSION");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  SSL_CTX_సమితి_options(octx->ssl_ctx, ctx_options);
  SSL_CTX_సమితి_read_ahead(octx->ssl_ctx, 1);

  /* Max TLS1.2 record size 0x4000 + 0x800.
     OpenSSL supports processing "jumbo TLS record" (8 TLS records) in one go
     for some algorithms, so match that here.
     Experimentation shows that a slightly larger buffer is needed
     to avoid short reads.

     However using a large buffer (8 packets) actually decreases performance.
     4 packets is better.
   */
#నిర్వచితమైతే HAVE_SSL_CTX_సమితి_DEFAULT_READ_బఫరు_LEN
  SSL_CTX_సమితి_default_read_బఫరు_len(octx->ssl_ctx, 0x401e * 4);
#అయితే_ముగింపు

  /* We do retry writes sometimes from another buffer address */
  SSL_CTX_సమితి_బహుళకం(octx->ssl_ctx, SSL_బహుళకం_ACCEPT_MOVING_WRITE_బఫరు);

  ciphers = conn_config->cipher_list;
  ఒకవేళ(!ciphers && (peer->transport != TRNSPRT_QUIC))
    ciphers = ఖాళీ;
  ఒకవేళ(ciphers && (ssl_సంస్కరణ_min < CURL_SSLVERSION_TLSv1_3)) {
    ఒకవేళ(!SSL_CTX_సమితి_cipher_list(octx->ssl_ctx, ciphers)) {
      failf(దత్తాంశం, "failed setting cipher list: %s", ciphers);
      తిరిగి CURLE_SSL_CIPHER;
    }
    infof(దత్తాంశం, "Cipher selection: %s", ciphers);
  }

#నిర్వచితమైతే HAVE_SSL_CTX_సమితి_CIPHERSUITES
  {
    స్థిరరాశి అక్షరం *ciphers13 = conn_config->cipher_list13;
    ఒకవేళ(ciphers13 &&
       (!conn_config->సంస్కరణ_max ||
        (conn_config->సంస్కరణ_max == CURL_SSLVERSION_MAX_DEFAULT) ||
        (conn_config->సంస్కరణ_max >= CURL_SSLVERSION_MAX_TLSv1_3))) {
      ఒకవేళ(!SSL_CTX_సమితి_ciphersuites(octx->ssl_ctx, ciphers13)) {
        failf(దత్తాంశం, "failed setting TLS 1.3 cipher suite: %s", ciphers13);
        తిరిగి CURLE_SSL_CIPHER;
      }
      infof(దత్తాంశం, "TLS 1.3 cipher selection: %s", ciphers13);
    }
  }
#అయితే_ముగింపు

  ఒకవేళ(ssl_cert || ssl_cert_blob || ssl_cert_type) {
    ఫలితం = client_cert(దత్తాంశం, octx,
                         ssl_cert, ssl_cert_blob, ssl_cert_type,
                         conn_config->కీలకం, conn_config->కీలకం_blob,
                         conn_config->కీలకం_type,
                         conn_config->కీలకం_passwd);
    ఒకవేళ(ఫలితం)
      /* failf() is already done in client_cert() */
      తిరిగి ఫలితం;
  }

#నిర్వచితమైతే HAVE_SSL_CTX_సమితి_POST_HANDSHAKE_AUTH
  /* OpenSSL 1.1.1 requires clients to opt-in for PHA */
  SSL_CTX_సమితి_post_handshake_auth(octx->ssl_ctx, 1);
#అయితే_ముగింపు

  {
    స్థిరరాశి అక్షరం *curves = conn_config->curves;
    ఒకవేళ(curves) {
#నిర్వచితమైతే HAVE_BORINGSSL_LIKE
#నిర్వచించు OSSL_వక్రం_CAST(x) (x)
#లేకపోతే
#నిర్వచించు OSSL_వక్రం_CAST(x) (అక్షరం *)CURL_UNCONST(x)
#అయితే_ముగింపు
      ఒకవేళ(!SSL_CTX_set1_curves_list(octx->ssl_ctx, OSSL_వక్రం_CAST(curves))) {
        failf(దత్తాంశం, "failed setting curves list: '%s'", curves);
        తిరిగి CURLE_SSL_CIPHER;
      }
    }
  }

#నిర్వచితమైతే HAVE_SSL_CTX_SET1_SIGALGS
#నిర్వచించు OSSL_SIGALG_CAST(x) OSSL_వక్రం_CAST(x)
  {
    స్థిరరాశి అక్షరం *signature_algorithms = conn_config->signature_algorithms;
    ఒకవేళ(signature_algorithms) {
      ఒకవేళ(!SSL_CTX_set1_sigalgs_list(octx->ssl_ctx,
                                    OSSL_SIGALG_CAST(signature_algorithms))) {
        failf(దత్తాంశం, "failed setting signature algorithms: '%s'",
              signature_algorithms);
        తిరిగి CURLE_SSL_CIPHER;
      }
    }
  }
#అయితే_ముగింపు

  /* OpenSSL always tries to verify the peer. By setting the failure mode
   * to NONE, we allow the connect to complete, regardless of the outcome.
   * We then explicitly check the result and may try alternatives like
   * Apple's SecTrust for verification. */
  SSL_CTX_సమితి_verify(octx->ssl_ctx, SSL_VERIFY_NONE, ఖాళీ);

  /* Enable logging of secrets to the file specified in env SSLKEYLOGFILE. */
#ఒకవేళ !defined(HAVE_KEYLOG_UPSTREAM) && defined(HAVE_KEYLOG_CALLBACK)
  ఒకవేళ(Curl_tls_keylog_enabled()) {
    SSL_CTX_సమితి_keylog_callback(octx->ssl_ctx, ossl_keylog_callback);
  }
#అయితే_ముగింపు

  ఒకవేళ(cb_new_session) {
    /* Enable the session cache because it is a prerequisite for the
     * "new session" callback. Use the "external storage" mode to prevent
     * OpenSSL from creating an internal session cache.
     */
    SSL_CTX_సమితి_session_cache_బహుళకం(octx->ssl_ctx,
                                   SSL_SESS_CACHE_CLIENT |
                                   SSL_SESS_CACHE_NO_అంతర్గత);
    SSL_CTX_sess_సమితి_new_cb(octx->ssl_ctx, cb_new_session);
  }

  /* give application a chance to interfere with SSL set up. */
  ఒకవేళ(దత్తాంశం->సమితి.ssl_fsslctx) {
    నిర్మాణం Curl_mapi_guard guard;
    /* When a user callback is installed to modify the SSL_CTX,
     * we need to do the full initialization before calling it.
     * See: #11800 */
    ఒకవేళ(!octx->x509_store_అమరిక) {
      ఫలితం = Curl_ssl_అమరిక_x509_store(cf, దత్తాంశం, octx);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
      octx->x509_store_అమరిక = TRUE;
    }
    CURL_CBAPI_ప్రారంభం(&guard, దత్తాంశం, easy_fsslctx);
    ఫలితం = (*దత్తాంశం->సమితి.ssl_fsslctx)(దత్తాంశం, octx->ssl_ctx,
                                      దత్తాంశం->సమితి.ssl_fsslctxp);
    CURL_CBAPI_చివర(&guard);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "error signaled by SSL ctx callback");
      తిరిగి ఫలితం;
    }
  }

  తిరిగి ossl_init_ssl(octx, cf, దత్తాంశం, peer, alpns_requested,
                       ssl_user_దత్తాంశం, sess_reuse_cb);
}

స్థిర CURLcode ossl_on_session_reuse(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం alpn_spec *alpns,
                                      నిర్మాణం Curl_ssl_session *scs,
                                      bool *do_early_దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;

  connssl->earlydata_max = scs->earlydata_max;

  తిరిగి Curl_on_session_reuse(cf, దత్తాంశం, alpns, scs, do_early_దత్తాంశం,
                               connssl->earlydata_max);
}

శూన్య Curl_ossl_report_handshake(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ossl_ctx *octx)
{
#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    పూర్ణాంకం psigtype_nid = NID_undef;
    స్థిరరాశి అక్షరం *negotiated_సమూహం_name = ఖాళీ;

#నిర్వచితమైతే HAVE_OPENSSL3
    SSL_get_peer_signature_type_nid(octx->ssl, &psigtype_nid);
#ఒకవేళ OPENSSL_సంస్కరణ_సంఖ్య >= 0x30200000L
    negotiated_సమూహం_name = SSL_get0_సమూహం_name(octx->ssl);
#లేకపోతే
    negotiated_సమూహం_name =
      OBJ_nid2sn(SSL_get_negotiated_సమూహం(octx->ssl) & 0x0000FFFF);
#అయితే_ముగింపు
#అయితే_ముగింపు

    /* Informational message */
    infof(దత్తాంశం, "SSL connection using %s / %s / %s / %s",
          SSL_get_సంస్కరణ(octx->ssl),
          SSL_get_cipher(octx->ssl),
          negotiated_సమూహం_name ? negotiated_సమూహం_name : "[blank]",
          OBJ_nid2sn(psigtype_nid));
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)octx;
#అయితే_ముగింపు /* CURLVERBOSE */
}

స్థిర CURLcode ossl_connect_step1(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  BIO *bio;
  CURLcode ఫలితం;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_1);
  DEBUGASSERT(octx);
  DEBUGASSERT(connssl->peer.origin);

  ఫలితం = Curl_ossl_ctx_init(octx, cf, దత్తాంశం, &connssl->peer,
                              connssl->alpn, ఖాళీ, ఖాళీ,
                              ossl_new_session_cb, cf,
                              ossl_on_session_reuse);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  octx->bio_పద్ధతి = ossl_bio_cf_పద్ధతి_create();
  ఒకవేళ(!octx->bio_పద్ధతి)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  bio = BIO_new(octx->bio_పద్ధతి);
  ఒకవేళ(!bio)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  BIO_సమితి_దత్తాంశం(bio, cf);
#నిర్వచితమైతే HAVE_SSL_SET0_WBIO
  /* with OpenSSL v1.1.1 we get an alternative to SSL_set_bio() that works
   * without backward compat quirks. Every call takes one reference, so we
   * up it and pass. SSL* then owns and frees it.
   * We check on the function in configure, since LibreSSL and friends
   * each have their own versions to add support for this. */
  BIO_పైకి_ref(bio);
  SSL_set0_rbio(octx->ssl, bio);
  SSL_set0_wbio(octx->ssl, bio);
#లేకపోతే
  SSL_సమితి_bio(octx->ssl, bio, bio);
#అయితే_ముగింపు

  ఒకవేళ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    నిర్మాణం alpn_proto_buf proto;
    memset(&proto, 0, పరిమాణం(proto));
    Curl_alpn_గమ్యం_proto_str(&proto, connssl->alpn);
    infof(దత్తాంశం, VTLS_INFOF_ALPN_OFFER_1STR, proto.దత్తాంశం);
  }

  connssl->connecting_state = ssl_connect_2;
  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే HAVE_SSL_SET1_ECH_CONFIG_LIST
/* If we have retry configs, then trace those out */
స్థిర పూర్ణాంకం ossl_trace_ech_retry_configs(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం, SSL *ssl,
                                        పూర్ణాంకం reason)
{
  CURLcode ఫలితం = CURLE_OK;
  size_t rcl = 0;
  పూర్ణాంకం rv = 1;
#నిర్వచితం_కాకపోతే HAVE_BORINGSSL_LIKE
  అక్షరం *inner = ఖాళీ;
  uint8_t *rcs = ఖాళీ;
  అక్షరం *outer = ఖాళీ;
#లేకపోతే
  స్థిరరాశి అక్షరం *inner = ఖాళీ;
  స్థిరరాశి uint8_t *rcs = ఖాళీ;
  స్థిరరాశి అక్షరం *outer = ఖాళీ;
  size_t బహిర్గతం_name_len = 0;
  పూర్ణాంకం servername_type = 0;
#అయితే_ముగింపు
  NOVERBOSE((శూన్య)reason);

  /* nothing to trace if not doing ECH */
  ఒకవేళ(!Curl_ssl_ech_enabled(cf))
    తిరిగి rv;
#నిర్వచితం_కాకపోతే HAVE_BORINGSSL_LIKE
  rv = SSL_ech_get1_retry_config(ssl, &rcs, &rcl);
#లేకపోతే
  SSL_get0_ech_retry_configs(ssl, &rcs, &rcl);
  rv = (పూర్ణాంకం)rcl;
#అయితే_ముగింపు

  ఒకవేళ(rv && rcs) {
    అక్షరం *b64str = ఖాళీ;
    size_t blen = 0;

    ఫలితం = curlx_base64_encode(rcs, rcl, &b64str, &blen);
    ఒకవేళ(!ఫలితం && b64str) {
      infof(దత్తాంశం, "ECH: retry_configs %s", b64str);
      curlx_free(b64str);
#నిర్వచితం_కాకపోతే HAVE_BORINGSSL_LIKE
      rv = SSL_ech_get1_స్థితి(ssl, &inner, &outer);
      infof(దత్తాంశం, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#లేకపోతే
      rv = SSL_ech_accepted(ssl);
      servername_type = SSL_get_servername_type(ssl);
      inner = SSL_get_servername(ssl, servername_type);
      SSL_get0_ech_name_override(ssl, &outer, &బహిర్గతం_name_len);
      infof(దత్తాంశం, "ECH: retry_configs for %s from %s, %d %d",
            inner ? inner : "NULL", outer ? outer : "NULL", reason, rv);
#అయితే_ముగింపు
    }
  }
  లేకపోతే
    infof(దత్తాంశం, "ECH: no retry_configs (rv = %d)", rv);
#నిర్వచితం_కాకపోతే HAVE_BORINGSSL_LIKE
  OPENSSL_free(inner);
  OPENSSL_free(rcs);
  OPENSSL_free(outer);
#అయితే_ముగింపు
  తిరిగి rv;
}

#అయితే_ముగింపు

స్థిర CURLcode ossl_connect_step2(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  నిర్మాణం ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, దత్తాంశం);
  పూర్ణాంకం err;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_2);
  DEBUGASSERT(octx);
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  ERR_clear_error();

  err = SSL_connect(octx->ssl);

  ఒకవేళ(!octx->x509_store_అమరిక) {
    /* After having send off the ClientHello, we prepare the x509
     * store to verify the coming certificate from the server */
    CURLcode ఫలితం = Curl_ssl_అమరిక_x509_store(cf, దత్తాంశం, octx);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    octx->x509_store_అమరిక = TRUE;
  }

#ఒకవేళ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* If key logging is enabled, wait for the handshake to complete and then
   * proceed with logging secrets (for TLS 1.2 or older).
   */
  ఒకవేళ(Curl_tls_keylog_enabled() && !octx->keylog_పూర్తి)
    ossl_log_tls12_secret(octx->ssl, &octx->keylog_పూర్తి);
#అయితే_ముగింపు

  /* 1  is fine
     0  is "not successful but was shut down controlled"
     <0 is "handshake was not successful, because a fatal error occurred" */
  ఒకవేళ(err != 1) {
    పూర్ణాంకం detail = SSL_get_error(octx->ssl, err);
    CURL_TRC_CF(దత్తాంశం, cf, "SSL_connect() -> err=%d, detail=%d", err, detail);

    ఒకవేళ(detail == SSL_ERROR_WANT_READ) {
      CURL_TRC_CF(దత్తాంశం, cf, "SSL_connect() -> want recv");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      తిరిగి CURLE_AGAIN;
    }
    ఒకవేళ(detail == SSL_ERROR_WANT_WRITE) {
      CURL_TRC_CF(దత్తాంశం, cf, "SSL_connect() -> want send");
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      తిరిగి CURLE_AGAIN;
    }
#నిర్వచితమైతే SSL_ERROR_WANT_ASYNC
    ఒకవేళ(detail == SSL_ERROR_WANT_ASYNC) {
      CURL_TRC_CF(దత్తాంశం, cf, "SSL_connect() -> want async");
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      తిరిగి CURLE_AGAIN;
    }
#అయితే_ముగింపు
#నిర్వచితమైతే SSL_ERROR_WANT_RETRY_VERIFY
    ఒకవేళ(detail == SSL_ERROR_WANT_RETRY_VERIFY) {
      CURL_TRC_CF(దత్తాంశం, cf, "SSL_connect() -> want retry_verify");
      Curl_xfer_pause_recv(దత్తాంశం, TRUE);
      తిరిగి CURLE_AGAIN;
    }
#అయితే_ముగింపు
    లేకపోతే {
      /* untreated error */
      sslerr_t errdetail;
      అక్షరం error_బఫరు[256] = "";
      CURLcode ఫలితం;
      దీర్ఘ lerr;
      పూర్ణాంకం lib;
      పూర్ణాంకం reason;

      /* the connection failed, we are not waiting for anything else. */
      connssl->connecting_state = ssl_connect_2;

      /* Get the earliest error code from the thread's error queue and remove
         the entry. */
      errdetail = ERR_get_error();

      /* Extract which lib and reason */
      lib = ERR_GET_LIB(errdetail);
      reason = ERR_GET_REASON(errdetail);

      ఒకవేళ((lib == ERR_LIB_SSL) &&
         ((reason == SSL_R_CERTIFICATE_VERIFY_FAILED)
/* Missing from OpenSSL 4+ OPENSSL_NO_DEPRECATED_3_0 builds */
#నిర్వచితమైతే SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
          || (reason == SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED)
#అయితే_ముగింపు
         )) {
        ఫలితం = CURLE_PEER_FAILED_VERIFICATION;

        lerr = SSL_get_verify_ఫలితం(octx->ssl);
        ఒకవేళ(lerr != X509_V_OK) {
          ssl_config->certverifyresult = lerr;
          failf(దత్తాంశం, "SSL certificate problem: %s",
                X509_verify_cert_error_string(lerr));
        }
        లేకపోతే
          failf(దత్తాంశం, "%s", "SSL certificate verification failed");
      }
#నిర్వచితమైతే SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
      /* SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED is only available on
         OpenSSL version above v1.1.1, not AWS-LC, BoringSSL, or LibreSSL */
      లేకపోతే ఒకవేళ((lib == ERR_LIB_SSL) &&
              (reason == SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED)) {
        /* If client certificate is required, communicate the
           error to client */
        ఫలితం = CURLE_SSL_CLIENTCERT;
        failf(దత్తాంశం, "TLS cert problem: %s",
              ossl_strerror(errdetail, error_బఫరు, పరిమాణం(error_బఫరు)));
      }
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SSL_SET1_ECH_CONFIG_LIST
      లేకపోతే ఒకవేళ((lib == ERR_LIB_SSL) &&
#నిర్వచితం_కాకపోతే HAVE_BORINGSSL_LIKE
              (reason == SSL_R_ECH_REQUIRED)) {
#లేకపోతే
              (reason == SSL_R_ECH_REJECTED)) {
#అయితే_ముగింపు /* HAVE_BORINGSSL_LIKE */

        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, దత్తాంశం, octx->ssl, reason);

        ఫలితం = CURLE_ECH_REQUIRED;
        failf(దత్తాంశం, "ECH: rejected, %s",
              ossl_strerror(errdetail, error_బఫరు, పరిమాణం(error_బఫరు)));
      }
#అయితే_ముగింపు
      లేకపోతే {
        ఫలితం = CURLE_SSL_CONNECT_ERROR;
        failf(దత్తాంశం, "TLS connect error: %s",
              ossl_strerror(errdetail, error_బఫరు, పరిమాణం(error_బఫరు)));
      }

      /* detail is already set to the SSL error above */

      /* If we e.g. use SSLv2 request-method and the server does not like us
       * (RST connection, etc.), OpenSSL gives no explanation whatsoever and
       * the SO_ERROR is also lost.
       */
      ఒకవేళ(ఫలితం == CURLE_SSL_CONNECT_ERROR && errdetail == 0) {
        అక్షరం extramsg[80] = "";
        పూర్ణాంకం sockerr = SOCKERRNO;

        ఒకవేళ(sockerr && detail == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, extramsg, పరిమాణం(extramsg));
        failf(దత్తాంశం, OSSL_PACKAGE " SSL_connect: %s in connection to %s:%d ",
              extramsg[0] ? extramsg : SSL_ERROR_గమ్యం_str(detail),
              connssl->peer.origin->hostname, connssl->peer.origin->పోర్టు);
      }

      తిరిగి ఫలితం;
    }
  }
  లేకపోతే {
    /* we connected fine, we are not waiting for anything else. */
    connssl->connecting_state = ssl_connect_3;
    Curl_ossl_report_handshake(దత్తాంశం, octx);

#ఒకవేళ defined(HAVE_SSL_SET1_ECH_CONFIG_LIST) && !defined(HAVE_BORINGSSL_LIKE)
    ఒకవేళ(Curl_ssl_ech_enabled(cf)) {
      నిర్మాణం ssl_filter_config *conn_config =
        Curl_ssl_cf_get_filter_config(cf);
      అక్షరం *inner = ఖాళీ, *outer = ఖాళీ;
      పూర్ణాంకం rv;
      VERBOSE(స్థిరరాశి అక్షరం *స్థితి_2);

      rv = SSL_ech_get1_స్థితి(octx->ssl, &inner, &outer);
      ఎంపిక(rv) {
      స్థితి SSL_ECH_స్థితి_SUCCESS:
        VERBOSE(స్థితి_2 = "succeeded");
        ఆపు;
      స్థితి SSL_ECH_స్థితి_GREASE_ECH:
        VERBOSE(స్థితి_2 = "sent GREASE, got retry-configs");
        ఆపు;
      స్థితి SSL_ECH_స్థితి_GREASE:
        VERBOSE(స్థితి_2 = "sent GREASE");
        ఆపు;
      స్థితి SSL_ECH_స్థితి_NOT_TRIED:
        VERBOSE(స్థితి_2 = "not attempted");
        ఆపు;
      స్థితి SSL_ECH_స్థితి_NOT_CONFIGURED:
        VERBOSE(స్థితి_2 = "not configured");
        ఆపు;
      స్థితి SSL_ECH_స్థితి_BACKEND:
        VERBOSE(స్థితి_2 = "backend (unexpected)");
        ఆపు;
      స్థితి SSL_ECH_స్థితి_FAILED:
        VERBOSE(స్థితి_2 = "failed");
        ఆపు;
      స్థితి SSL_ECH_స్థితి_BAD_CALL:
        VERBOSE(స్థితి_2 = "bad call (unexpected)");
        ఆపు;
      స్థితి SSL_ECH_స్థితి_BAD_NAME: {
        ఒకవేళ(!conn_config->verifypeer && !conn_config->verifyhost &&
           inner && !strcmp(inner, connssl->peer.origin->hostname)) {
          VERBOSE(స్థితి_2 = "bad name (tolerated without peer verification)");
          rv = SSL_ECH_స్థితి_SUCCESS;
        }
        లేకపోతే {
          VERBOSE(స్థితి_2 = "bad name (unexpected)");
        }
        ఆపు;
      }
      అప్రమేయ:
        VERBOSE(స్థితి_2 = "unexpected status");
        infof(దత్తాంశం, "ECH: unexpected status %d", rv);
      }
      infof(దత్తాంశం, "ECH: result '%s' (inner=%s, outer=%s)",
            (స్థితి_2 ? స్థితి_2 : "NULL"),
            (inner ? inner : "NULL"),
            (outer ? outer : "NULL"));
      OPENSSL_free(inner);
      OPENSSL_free(outer);
      ఒకవేళ(rv == SSL_ECH_స్థితి_GREASE_ECH) {
        /* trace retry_configs if we got some */
        ossl_trace_ech_retry_configs(cf, దత్తాంశం, octx->ssl, 0);
      }
      ఒకవేళ(rv != SSL_ECH_స్థితి_SUCCESS && (conn_config->ech == CURLECH_HARD)) {
        infof(దత్తాంశం, "ECH: ech-hard failed");
        తిరిగి CURLE_SSL_CONNECT_ERROR;
      }
    }
    లేకపోతే {
      infof(దత్తాంశం, "ECH: result: status is not attempted");
    }
#అయితే_ముగింపు /* HAVE_SSL_SET1_ECH_CONFIG_LIST && !HAVE_BORINGSSL_LIKE */

    /* Sets data and len to negotiated protocol, len is 0 if no protocol was
     * negotiated
     */
    ఒకవేళ(connssl->alpn) {
      స్థిరరాశి గుర్తులేని అక్షరం *neg_protocol;
      గుర్తులేని పూర్ణాంకం len;
      SSL_get0_alpn_selected(octx->ssl, &neg_protocol, &len);

      తిరిగి Curl_alpn_సమితి_negotiated(cf, దత్తాంశం, connssl, neg_protocol, len);
    }

    తిరిగి CURLE_OK;
  }
}

/*
 * Heavily modified from:
 * https://www.owasp.org/index.php/Certificate_and_Public_Key_Pinning#OpenSSL
 */
స్థిర CURLcode ossl_pkp_pin_peer_pubkey(నిర్మాణం Curl_easy *దత్తాంశం, X509 *cert,
                                         స్థిరరాశి అక్షరం *pinnedpubkey)
{
  /* Scratch */
  పూర్ణాంకం len1 = 0, len2 = 0;
  గుర్తులేని అక్షరం *buff1 = ఖాళీ, *తాత్కాలిక = ఖాళీ;

  /* Result is returned to caller */
  CURLcode ఫలితం = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ఒకవేళ(!pinnedpubkey)
    తిరిగి CURLE_OK;

  ఒకవేళ(!cert)
    తిరిగి ఫలితం;

  చేయి {
    /* Get the subjectPublicKeyInfo */
    /* https://groups.google.com/group/mailing.openssl.users/browse_thread/thread/d61858dae102c6c7 */
    len1 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), ఖాళీ);
    ఒకవేళ(len1 < 1)
      ఆపు; /* failed */

    buff1 = తాత్కాలిక = curlx_malloc(len1);
    ఒకవేళ(!buff1)
      ఆపు; /* failed */

    /* https://docs.openssl.org/master/man3/d2i_X509/ */
    len2 = i2d_X509_PUBKEY(X509_get_X509_PUBKEY(cert), &తాత్కాలిక);

    /*
     * These checks are verifying we got back the same values as when we
     * sized the buffer. it is pretty weak since they should always be the
     * same, but it gives us something to test.
     */
    ఒకవేళ((len1 != len2) || !తాత్కాలిక || ((తాత్కాలిక - buff1) != len1))
      ఆపు; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    ఫలితం = Curl_pin_peer_pubkey(దత్తాంశం, pinnedpubkey, buff1, len1);
  } అయితే(0);

  ఒకవేళ(buff1)
    curlx_free(buff1);

  తిరిగి ఫలితం;
}

#నిర్వచితమైతే CURLVERBOSE
#ఒకవేళ !defined(HAVE_BORINGSSL_LIKE) && \
  !(defined(LIBRESSL_సంస్కరణ_సంఖ్య) && LIBRESSL_సంస్కరణ_సంఖ్య < 0x3060000fL)
స్థిర శూన్య infof_certstack(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి SSL *ssl)
{
  దొంతర_యొక్క(X509) *certstack;
  దీర్ఘ verify_ఫలితం;
  పూర్ణాంకం అంకె_cert_స్థాయిలు;
  పూర్ణాంకం cert_level;

  ఒకవేళ(!Curl_trc_is_verbose(దత్తాంశం))
    తిరిగి;

  verify_ఫలితం = SSL_get_verify_ఫలితం(ssl);
  ఒకవేళ(verify_ఫలితం != X509_V_OK)
    certstack = SSL_get_peer_cert_chain(ssl);
  లేకపోతే
    certstack = SSL_get0_verified_chain(ssl);
  ఒకవేళ(!certstack)
    తిరిగి;
  అంకె_cert_స్థాయిలు = sk_X509_అంకె(certstack);

  కొరకు(cert_level = 0; cert_level < అంకె_cert_స్థాయిలు; cert_level++) {
    అక్షరం cert_algorithm[80] = "";
    అక్షరం సమూహం_name_final[80] = "";
    స్థిరరాశి X509_ALGOR *palg_cert = ఖాళీ;
    స్థిరరాశి ASN1_OBJECT *paobj_cert = ఖాళీ;
    X509 *current_cert;
    EVP_PKEY *current_pkey;
    పూర్ణాంకం కీలకం_బిట్లు;
    పూర్ణాంకం కీలకం_sec_బిట్లు;
    పూర్ణాంకం get_సమూహం_name;
    స్థిరరాశి అక్షరం *type_name;

    current_cert = sk_X509_విలువ(certstack, cert_level);
    ఒకవేళ(!current_cert)
      కొనసాగు;

    current_pkey = X509_get0_pubkey(current_cert);
    ఒకవేళ(!current_pkey)
      కొనసాగు;

    X509_get0_signature(ఖాళీ, &palg_cert, current_cert);
    X509_ALGOR_get0(&paobj_cert, ఖాళీ, ఖాళీ, palg_cert);
    OBJ_obj2txt(cert_algorithm, పరిమాణం(cert_algorithm), paobj_cert, 0);

    కీలకం_బిట్లు = EVP_PKEY_బిట్లు(current_pkey);
#నిర్వచితం_కాకపోతే HAVE_OPENSSL3
#నిర్వచించు EVP_PKEY_get_security_బిట్లు EVP_PKEY_security_బిట్లు
#అయితే_ముగింపు
    కీలకం_sec_బిట్లు = EVP_PKEY_get_security_బిట్లు(current_pkey);
#నిర్వచితమైతే HAVE_OPENSSL3
    {
      అక్షరం సమూహం_name[80] = "";
      get_సమూహం_name = EVP_PKEY_get_సమూహం_name(current_pkey, సమూహం_name,
                                               పరిమాణం(సమూహం_name), ఖాళీ);
      curl_msnprintf(సమూహం_name_final, పరిమాణం(సమూహం_name_final), "/%s",
                     సమూహం_name);
    }
    type_name = EVP_PKEY_get0_type_name(current_pkey);
#లేకపోతే
    get_సమూహం_name = 0;
    type_name = ఖాళీ;
#అయితే_ముగింపు

    infof(దత్తాంశం, "  Certificate level %d: "
          "Public key type %s%s (%d/%d Bits/secBits), signed using %s",
          cert_level, type_name ? type_name : "?",
          get_సమూహం_name == 0 ? "" : సమూహం_name_final,
          కీలకం_బిట్లు, కీలకం_sec_బిట్లు, cert_algorithm);
  }
}
#లేకపోతే
#నిర్వచించు infof_certstack(దత్తాంశం, ssl)
#అయితే_ముగింపు
#అయితే_ముగింపు /* CURLVERBOSE */

స్థిర CURLcode ossl_check_issuer(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  X509 *server_cert)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  X509 *issuer = ఖాళీ;
  BIO *fp = ఖాళీ;
  అక్షరం err_buf[256] = "";
  bool verify_enabled = (conn_config->verifypeer || conn_config->verifyhost);
  CURLcode ఫలితం = CURLE_OK;

  /* e.g. match issuer name with provided issuer certificate */
  ఒకవేళ(conn_config->issuercert_blob) {
    fp = BIO_new_mem_buf(conn_config->issuercert_blob->దత్తాంశం,
                         (పూర్ణాంకం)conn_config->issuercert_blob->len);
    ఒకవేళ(!fp) {
      failf(దత్తాంశం, "BIO_new_mem_buf NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, పరిమాణం(err_buf)));
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }
  }
  లేకపోతే ఒకవేళ(conn_config->issuercert) {
    fp = BIO_new(BIO_s_file());
    ఒకవేళ(!fp) {
      failf(దత్తాంశం, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
            ossl_strerror(ERR_get_error(), err_buf, పరిమాణం(err_buf)));
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(BIO_read_filename(fp, conn_config->issuercert) <= 0) {
      ఒకవేళ(verify_enabled)
        failf(దత్తాంశం, "SSL: Unable to open issuer cert (%s)",
              conn_config->issuercert);
      ఫలితం = CURLE_SSL_ISSUER_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఒకవేళ(fp) {
    issuer = PEM_read_bio_X509(fp, ఖాళీ, ZERO_NULL, ఖాళీ);
    ఒకవేళ(!issuer) {
      ఒకవేళ(verify_enabled)
        failf(దత్తాంశం, "SSL: Unable to read issuer cert (%s)",
              conn_config->issuercert);
      ఫలితం = CURLE_SSL_ISSUER_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(X509_check_issued(issuer, server_cert) != X509_V_OK) {
      ఒకవేళ(verify_enabled)
        failf(దత్తాంశం, "SSL: Certificate issuer check failed (%s)",
              conn_config->issuercert);
      ఫలితం = CURLE_SSL_ISSUER_ERROR;
      వెళ్ళు బహిర్గతం;
    }

    infof(దత్తాంశం, " SSL certificate issuer check ok (%s)",
          conn_config->issuercert);
  }

బహిర్గతం:
  ఒకవేళ(fp)
    BIO_free(fp);
  ఒకవేళ(issuer)
    X509_free(issuer);
  తిరిగి ఫలితం;
}

స్థిర స్థిరరాశి అక్షరం *pinned(నిర్మాణం Curl_cfilter *cf,
                          నిర్మాణం Curl_easy *దత్తాంశం)
{
  (శూన్య)cf;
  తిరిగి
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
    Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
#అయితే_ముగింపు
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY);
}

స్థిర CURLcode ossl_check_pinned_కీలకం(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      X509 *server_cert)
{
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి అక్షరం *ptr = pinned(cf, దత్తాంశం);
  ఒకవేళ(ptr) {
    ఫలితం = ossl_pkp_pin_peer_pubkey(దత్తాంశం, server_cert, ptr);
    ఒకవేళ(ఫలితం)
      failf(దత్తాంశం, "SSL: public key does not match pinned public key");
  }
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే CURLVERBOSE
#నిర్వచించు MAX_CERT_NAME_నిడివి 2048
స్థిర CURLcode ossl_infof_cert(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                X509 *server_cert)
{
  BIO *mem = ఖాళీ;
  నిర్మాణం dynbuf dname;
  అక్షరం err_buf[256] = "";
  అక్షరం *buf;
  దీర్ఘ len;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!Curl_trc_is_verbose(దత్తాంశం))
    తిరిగి CURLE_OK;

  curlx_dyn_init(&dname, MAX_CERT_NAME_నిడివి);
  mem = BIO_new(BIO_s_mem());
  ఒకవేళ(!mem) {
    failf(దత్తాంశం, "BIO_new return NULL, " OSSL_PACKAGE " error %s",
          ossl_strerror(ERR_get_error(), err_buf, పరిమాణం(err_buf)));
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  infof(దత్తాంశం, "%s certificate:", Curl_ssl_cf_is_proxy(cf) ?
        "Proxy" : "Server");

  ఫలితం = x509_name_oneline(X509_get_subject_name(server_cert), &dname);
  infof(దత్తాంశం, "  subject: %s", ఫలితం ? "[NONE]" : curlx_dyn_ptr(&dname));

  ASN1_సమయం_print(mem, X509_get0_notBefore(server_cert));
  len = BIO_get_mem_దత్తాంశం(mem, (అక్షరం **)&buf);
  infof(దత్తాంశం, "  start date: %.*s", (పూర్ణాంకం)len, buf);
  (శూన్య)BIO_reset(mem);

  ASN1_సమయం_print(mem, X509_get0_notAfter(server_cert));
  len = BIO_get_mem_దత్తాంశం(mem, (అక్షరం **)&buf);
  infof(దత్తాంశం, "  expire date: %.*s", (పూర్ణాంకం)len, buf);
  (శూన్య)BIO_reset(mem);

  ఫలితం = x509_name_oneline(X509_get_issuer_name(server_cert), &dname);
  ఒకవేళ(ఫలితం) /* should be only fatal stuff like OOM */
    వెళ్ళు బహిర్గతం;
  infof(దత్తాంశం, "  issuer: %s", curlx_dyn_ptr(&dname));

బహిర్గతం:
  BIO_free(mem);
  curlx_dyn_free(&dname);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* CURLVERBOSE */

#నిర్వచితమైతే USE_APPLE_SECTRUST
నిర్మాణం ossl_certs_ctx {
  దొంతర_యొక్క(X509) *sk;
  size_t అంకె_certs;
  గుర్తులేని అక్షరం *చివరి_der;
};

స్థిర CURLcode ossl_chain_get_der(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   శూన్య *user_దత్తాంశం,
                                   size_t i,
                                   గుర్తులేని అక్షరం **pder,
                                   size_t *pder_len)
{
  నిర్మాణం ossl_certs_ctx *chain = user_దత్తాంశం;
  X509 *cert;
  పూర్ణాంకం der_len;

  OPENSSL_free(chain->చివరి_der);
  chain->చివరి_der = ఖాళీ;

  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
  *pder_len = 0;
  *pder = ఖాళీ;

  ఒకవేళ(i >= chain->అంకె_certs)
    తిరిగి CURLE_TOO_LARGE;
  cert = sk_X509_విలువ(chain->sk, (పూర్ణాంకం)i);
  ఒకవేళ(!cert)
    తిరిగి CURLE_FAILED_INIT;
  der_len = i2d_X509(cert, pder);
  ఒకవేళ(der_len < 0)
    తిరిగి CURLE_FAILED_INIT;
  chain->చివరి_der = *pder;
  *pder_len = (size_t)der_len;
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ossl_apple_verify(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ossl_ctx *octx,
                                  నిర్మాణం ssl_peer *peer)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం ossl_certs_ctx chain;
  CURLcode ఫలితం;

  octx->sectrust_verified = FALSE;
  memset(&chain, 0, పరిమాణం(chain));
  chain.sk = SSL_get_peer_cert_chain(octx->ssl);
  chain.అంకె_certs = chain.sk ? sk_X509_అంకె(chain.sk) : 0;

  ఒకవేళ(!chain.అంకె_certs &&
     (conn_config->verifypeer || conn_config->verifyhost)) {
    ఒకవేళ(!octx->reused_session) {
      failf(దత్తాంశం, "SSL: could not get peer certificate chain");
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    }
    లేకపోతే {
      /* When session was reused, there is no peer cert chain.
       * We trust it if it came from a SecTrust verified TLS. */
      CURL_TRC_CF(దత్తాంశం, cf, "session reused, sectrust_session=%d",
                  octx->sectrust_session);
      octx->sectrust_verified = (bool)octx->sectrust_session;
      తిరిగి CURLE_OK;
    }
  }
  లేకపోతే {
#నిర్వచితమైతే HAVE_BORINGSSL_LIKE
    స్థిరరాశి uint8_t *ocsp_దత్తాంశం = ఖాళీ;
#లేకపోతే
    గుర్తులేని అక్షరం *ocsp_దత్తాంశం = ఖాళీ;
#అయితే_ముగింపు
    దీర్ఘ ocsp_len = 0;
    bool ocsp_required = (!octx->reused_session && conn_config->verifystatus);
    ఒకవేళ(!octx->reused_session)
      ocsp_len = (దీర్ఘ)SSL_get_tlsext_స్థితి_ocsp_resp(octx->ssl, &ocsp_దత్తాంశం);
    /* SSL_get_tlsext_status_ocsp_resp() returns the length of the OCSP
       response data or -1 if there is no OCSP response data.
       AWS-LC breaks the API and returns 0 when there is no data. */
    ఒకవేళ(ocsp_len <= 0) {
      ocsp_len = 0; /* no data available */
      ocsp_దత్తాంశం = ఖాళీ;
    }

    ఫలితం = Curl_vtls_apple_verify(cf, దత్తాంశం, peer, chain.అంకె_certs,
                                    ossl_chain_get_der, &chain,
                                    ocsp_దత్తాంశం, ocsp_len,
                                    ocsp_required);
    OPENSSL_free(chain.చివరి_der);
    chain.చివరి_der = ఖాళీ;
  }
  octx->sectrust_verified = !ఫలితం;
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* USE_APPLE_SECTRUST */

CURLcode Curl_ossl_check_peer_cert(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ossl_ctx *octx,
                                   నిర్మాణం ssl_peer *peer)
{
  నిర్మాణం connectdata *conn = cf->conn;
  నిర్మాణం ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, దత్తాంశం);
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ఫలితం = CURLE_OK;
  దీర్ఘ ossl_verify;
  X509 *server_cert;
  bool verified = FALSE;

  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo && !octx->reused_session) {
    /* asked to gather certificate info. Reused sessions do not have cert
       chains */
    ఫలితం = ossl_certchain(దత్తాంశం, octx->ssl);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  server_cert = SSL_get1_peer_certificate(octx->ssl);
  ఒకవేళ(!server_cert) {
    /* no verification at all, this maybe acceptable */
    ఒకవేళ(!(conn_config->verifypeer || conn_config->verifyhost) &&
       !pinned(cf, దత్తాంశం))
      వెళ్ళు బహిర్గతం;

    failf(దత్తాంశం, "SSL: could not get peer certificate");
    ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    వెళ్ళు బహిర్గతం;
  }

#నిర్వచితమైతే CURLVERBOSE
  ఫలితం = ossl_infof_cert(cf, దత్తాంశం, server_cert);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  infof_certstack(దత్తాంశం, octx->ssl);
#అయితే_ముగింపు

  ఒకవేళ(conn_config->verifyhost) {
    ఫలితం = ossl_verifyhost(దత్తాంశం, conn, peer, server_cert);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  /* `verifyhost` is either OK or not requested from here on */

  ossl_verify = SSL_get_verify_ఫలితం(octx->ssl);
  ssl_config->certverifyresult = ossl_verify;
  infof(దత్తాంశం, "OpenSSL verify result: %lx", (గుర్తులేని దీర్ఘ)ossl_verify);

  verified = (ossl_verify == X509_V_OK);
  ఒకవేళ(verified)
    infof(దత్తాంశం, "SSL certificate verified via OpenSSL.");

#నిర్వచితమైతే USE_APPLE_SECTRUST
  ఒకవేళ(!verified && conn_config->verifypeer && conn_config->native_ca_store) {
    /* we verify using Apple SecTrust *unless* OpenSSL already verified.
     * This may happen if the application intercepted the OpenSSL callback
     * and installed its own. */
    ఫలితం = ossl_apple_verify(cf, దత్తాంశం, octx, peer);
    ఒకవేళ(ఫలితం && (ఫలితం != CURLE_PEER_FAILED_VERIFICATION))
      వెళ్ళు బహిర్గతం; /* unexpected error */
    ఒకవేళ(octx->sectrust_verified) {
      infof(దత్తాంశం, "SSL certificate verified via Apple SecTrust.");
      ssl_config->certverifyresult = X509_V_OK;
      verified = TRUE;
    }
  }
#అయితే_ముగింపు

  ఒకవేళ(!verified) {
    /* no trust established, report the OpenSSL status */
    ఒకవేళ(conn_config->verifypeer) {
      failf(దత్తాంశం, "SSL certificate OpenSSL verify result: %s (%ld)",
            X509_verify_cert_error_string(ossl_verify), ossl_verify);
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
      వెళ్ళు బహిర్గతం;
    }
    infof(దత్తాంశం, " SSL certificate verification failed, continuing anyway!");
  }

#నిర్వచితం_కాకపోతే OPENSSL_NO_OCSP
  ఒకవేళ(conn_config->verifystatus &&
#నిర్వచితమైతే USE_APPLE_SECTRUST
    /* When SecTrust did the verify and has the capability, no need. */
     !(octx->sectrust_verified && Curl_vtls_apple_verifies_ocsp(cf, దత్తాంశం)) &&
#అయితే_ముగింపు
     !octx->reused_session) {
    ఫలితం = verifystatus(cf, దత్తాంశం, octx);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
#అయితే_ముగింపు

  ఫలితం = ossl_check_issuer(cf, దత్తాంశం, server_cert);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = ossl_check_pinned_కీలకం(cf, దత్తాంశం, server_cert);

బహిర్గతం:
  X509_free(server_cert);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ossl_connect_step3(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;

  DEBUGASSERT(connssl->connecting_state == ssl_connect_3);

  /*
   * We check certificates to authenticate the server; otherwise we risk
   * man-in-the-middle attack; NEVERTHELESS, if we are told explicitly not to
   * verify the peer, ignore faults and failures from the server cert
   * operations.
   */

  ఫలితం = Curl_ossl_check_peer_cert(cf, దత్తాంశం, octx, &connssl->peer);
  ఒకవేళ(ఫలితం)
    /* on error, remove sessions we might have in the pool */
    Curl_ssl_scache_remove_all(cf, దత్తాంశం, connssl->peer.scache_కీలకం);

  తిరిగి ఫలితం;
}

#నిర్వచితమైతే HAVE_OPENSSL_EARLYDATA
స్థిర CURLcode ossl_send_earlydata(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి గుర్తులేని అక్షరం *buf;
  size_t blen, nwritten;
  పూర్ణాంకం rc;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  octx->io_ఫలితం = CURLE_OK;
  అయితే(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    nwritten = 0;
    rc = SSL_write_early_దత్తాంశం(octx->ssl, buf, blen, &nwritten);
    CURL_TRC_CF(దత్తాంశం, cf, "SSL_write_early_data(len=%zu) -> %d, %zu",
                blen, rc, nwritten);
    ఒకవేళ(rc <= 0) {
      దీర్ఘ sslerror;
      అక్షరం error_బఫరు[256];
      పూర్ణాంకం err = SSL_get_error(octx->ssl, rc);

      ఎంపిక(err) {
      స్థితి SSL_ERROR_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        ఫలితం = CURLE_AGAIN;
        వెళ్ళు బహిర్గతం;
      స్థితి SSL_ERROR_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ఫలితం = CURLE_AGAIN;
        వెళ్ళు బహిర్గతం;
      స్థితి SSL_ERROR_SYSCALL: {
        పూర్ణాంకం sockerr = SOCKERRNO;

        ఒకవేళ(octx->io_ఫలితం == CURLE_AGAIN) {
          ఫలితం = CURLE_AGAIN;
          వెళ్ళు బహిర్గతం;
        }
        sslerror = ERR_get_error();
        ఒకవేళ(sslerror)
          ossl_strerror(sslerror, error_బఫరు, పరిమాణం(error_బఫరు));
        లేకపోతే ఒకవేళ(sockerr)
          curlx_strerror(sockerr, error_బఫరు, పరిమాణం(error_బఫరు));
        లేకపోతే
          curl_msnprintf(error_బఫరు, పరిమాణం(error_బఫరు), "%s",
                         SSL_ERROR_గమ్యం_str(err));

        failf(దత్తాంశం, OSSL_PACKAGE " SSL_write:early_data: %s, errno %d",
              error_బఫరు, sockerr);
        ఫలితం = CURLE_SEND_ERROR;
        వెళ్ళు బహిర్గతం;
      }
      స్థితి SSL_ERROR_SSL: {
        /*  A failure in the SSL library occurred, usually a protocol error.
            The OpenSSL error queue contains more information on the error. */
        sslerror = ERR_get_error();
        failf(దత్తాంశం, "SSL_write_early_data() error: %s",
              ossl_strerror(sslerror, error_బఫరు, పరిమాణం(error_బఫరు)));
        ఫలితం = CURLE_SEND_ERROR;
        వెళ్ళు బహిర్గతం;
      }
      అప్రమేయ:
        /* a true error */
        failf(దత్తాంశం, OSSL_PACKAGE " SSL_write_early_data: %s, errno %d",
              SSL_ERROR_గమ్యం_str(err), SOCKERRNO);
        ఫలితం = CURLE_SEND_ERROR;
        వెళ్ళు బహిర్గతం;
      }
    }
    Curl_bufq_skip(&connssl->earlydata, nwritten);
  }
  /* sent everything there was */
  infof(దత్తాంశం, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
బహిర్గతం:
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* HAVE_OPENSSL_EARLYDATA */

స్థిర CURLcode ossl_connect(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             bool *పూర్తి)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;

  /* check if the connection has already been established */
  ఒకవేళ(ssl_connection_complete == connssl->state) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ఒకవేళ(connssl->connecting_state == ssl_connect_1) {
    ఒకవేళ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(దత్తాంశం, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(దత్తాంశం, cf, "need HTTPS-RR, delaying connect");
      తిరిగి CURLE_OK;
    }
    CURL_TRC_CF(దత్తాంశం, cf, "ossl_connect, step1");
    ఫలితం = ossl_connect_step1(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_2) {
    CURL_TRC_CF(దత్తాంశం, cf, "ossl_connect, step2");
#నిర్వచితమైతే HAVE_OPENSSL_EARLYDATA
    ఒకవేళ(connssl->earlydata_state == ssl_earlydata_await) {
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(connssl->earlydata_state == ssl_earlydata_sending) {
      ఫలితం = ossl_send_earlydata(cf, దత్తాంశం);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      connssl->earlydata_state = ssl_earlydata_పంపిన;
    }
#అయితే_ముగింపు
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_పంపిన));

    ఫలితం = ossl_connect_step2(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_3) {
    CURL_TRC_CF(దత్తాంశం, cf, "ossl_connect, step3");
    ఫలితం = ossl_connect_step3(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    connssl->connecting_state = ssl_connect_పూర్తి;
#నిర్వచితమైతే HAVE_OPENSSL_EARLYDATA
    ఒకవేళ(connssl->earlydata_state > ssl_earlydata_none) {
      నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_పంపిన);
      connssl->earlydata_state =
        (SSL_get_early_దత్తాంశం_స్థితి(octx->ssl) == SSL_EARLY_దత్తాంశం_ACCEPTED) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#అయితే_ముగింపు
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_పూర్తి) {
    CURL_TRC_CF(దత్తాంశం, cf, "ossl_connect, done");
    connssl->state = ssl_connection_complete;
  }

బహిర్గతం:
  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    *పూర్తి = FALSE;
    తిరిగి CURLE_OK;
  }
  *పూర్తి = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  తిరిగి ఫలితం;
}

స్థిర bool ossl_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                              స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  (శూన్య)దత్తాంశం;
  తిరిగి (bool)connssl->ఆదానం_pending;
}

స్థిర CURLcode ossl_send(నిర్మాణం Curl_cfilter *cf,
                          నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి శూన్య *mem,
                          size_t len,
                          size_t *pnwritten)
{
  /* SSL_write() is said to return 'int' while write() and send() returns
     'size_t' */
  పూర్ణాంకం err;
  అక్షరం error_బఫరు[256];
  sslerr_t sslerror;
  పూర్ణాంకం memlen;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం nwritten;

  DEBUGASSERT(octx);
  *pnwritten = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  memlen = (len > (size_t)గరిష్ఠ_పూర్ణాంకం) ? గరిష్ఠ_పూర్ణాంకం : (పూర్ణాంకం)len;
  ఒకవేళ(octx->blocked_ssl_write_len && (octx->blocked_ssl_write_len != memlen)) {
    /* The previous SSL_write() call was blocked, using that length.
     * We need to use that again or OpenSSL freaks out. A shorter
     * length should not happen and is a bug in libcurl. */
    ఒకవేళ(octx->blocked_ssl_write_len > memlen) {
      DEBUGASSERT(0);
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
    memlen = octx->blocked_ssl_write_len;
  }
  octx->blocked_ssl_write_len = 0;
  nwritten = SSL_write(octx->ssl, mem, memlen);

  ఒకవేళ(nwritten > 0) {
    ఒకవేళ(nwritten > memlen) {
      failf(దత్తాంశం, "SSL_write(len=%d) returned %d", memlen, nwritten);
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    *pnwritten = (size_t)nwritten;
  }
  లేకపోతే {
    err = SSL_get_error(octx->ssl, nwritten);

    ఎంపిక(err) {
    స్థితి SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      octx->blocked_ssl_write_len = memlen;
      ఫలితం = CURLE_AGAIN;
      వెళ్ళు బహిర్గతం;
    స్థితి SSL_ERROR_WANT_WRITE:
      ఫలితం = CURLE_AGAIN;
      octx->blocked_ssl_write_len = memlen;
      వెళ్ళు బహిర్గతం;
    స్థితి SSL_ERROR_SYSCALL: {
      పూర్ణాంకం sockerr = SOCKERRNO;

      ఒకవేళ(octx->io_ఫలితం == CURLE_AGAIN) {
        octx->blocked_ssl_write_len = memlen;
        ఫలితం = CURLE_AGAIN;
        వెళ్ళు బహిర్గతం;
      }
      sslerror = ERR_get_error();
      ఒకవేళ(sslerror)
        ossl_strerror(sslerror, error_బఫరు, పరిమాణం(error_బఫరు));
      లేకపోతే ఒకవేళ(sockerr)
        curlx_strerror(sockerr, error_బఫరు, పరిమాణం(error_బఫరు));
      లేకపోతే
        curl_msnprintf(error_బఫరు, పరిమాణం(error_బఫరు), "%s",
                       SSL_ERROR_గమ్యం_str(err));

      failf(దత్తాంశం, OSSL_PACKAGE " SSL_write: %s, errno %d",
            error_బఫరు, sockerr);
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    స్థితి SSL_ERROR_SSL: {
      /*  A failure in the SSL library occurred, usually a protocol error.
          The OpenSSL error queue contains more information on the error. */
      sslerror = ERR_get_error();
      failf(దత్తాంశం, "SSL_write() error: %s",
            ossl_strerror(sslerror, error_బఫరు, పరిమాణం(error_బఫరు)));
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    అప్రమేయ:
      /* a true error */
      failf(దత్తాంశం, OSSL_PACKAGE " SSL_write: %s, errno %d",
            SSL_ERROR_గమ్యం_str(err), SOCKERRNO);
      ఫలితం = CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

బహిర్గతం:
  తిరిగి ఫలితం;
}

స్థిర CURLcode ossl_recv(నిర్మాణం Curl_cfilter *cf,
                          నిర్మాణం Curl_easy *దత్తాంశం,   /* transfer */
                          అక్షరం *buf,                /* store read data here */
                          size_t buffersize,        /* max amount to read */
                          size_t *pnread)
{
  అక్షరం error_బఫరు[256];
  గుర్తులేని దీర్ఘ sslerror;
  పూర్ణాంకం buffsize;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం nread;

  DEBUGASSERT(octx);

  *pnread = 0;
  ERR_clear_error();

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  buffsize = (buffersize > (size_t)గరిష్ఠ_పూర్ణాంకం) ? గరిష్ఠ_పూర్ణాంకం : (పూర్ణాంకం)buffersize;
  nread = SSL_read(octx->ssl, buf, buffsize);

  ఒకవేళ(nread > 0)
    *pnread = (size_t)nread;
  లేకపోతే {
    /* failed SSL_read */
    పూర్ణాంకం err = SSL_get_error(octx->ssl, nread);

    ఎంపిక(err) {
    స్థితి SSL_ERROR_NONE: /* this is not an error */
      ఆపు;
    స్థితి SSL_ERROR_ZERO_RETURN: /* no more data */
      /* close_notify alert */
      ఒకవేళ(cf->sockindex == FIRSTSOCKET)
        /* mark the connection for close if it is indeed the control
           connection */
        CURL_TRC_CF(దత్తాంశం, cf, "TLS close_notify");
      ఆపు;
    స్థితి SSL_ERROR_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ఫలితం = CURLE_AGAIN;
      వెళ్ళు బహిర్గతం;
    స్థితి SSL_ERROR_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ఫలితం = CURLE_AGAIN;
      వెళ్ళు బహిర్గతం;
    అప్రమేయ:
      /* openssl/ssl.h for SSL_ERROR_SYSCALL says "look at error stack/return
         value/errno" */
      /* https://docs.openssl.org/master/man3/ERR_get_error/ */
      ఒకవేళ(octx->io_ఫలితం == CURLE_AGAIN) {
        ఫలితం = CURLE_AGAIN;
        వెళ్ళు బహిర్గతం;
      }
      sslerror = ERR_get_error();
      ఒకవేళ((nread < 0) || sslerror) {
        /* If the return code was negative or there actually is an error in the
వరుస*/
        పూర్ణాంకం sockerr = SOCKERRNO;
        ఒకవేళ(sslerror)
          ossl_strerror(sslerror, error_బఫరు, పరిమాణం(error_బఫరు));
        లేకపోతే ఒకవేళ(sockerr && err == SSL_ERROR_SYSCALL)
          curlx_strerror(sockerr, error_బఫరు, పరిమాణం(error_బఫరు));
        లేకపోతే
          curl_msnprintf(error_బఫరు, పరిమాణం(error_బఫరు), "%s",
                         SSL_ERROR_గమ్యం_str(err));
        failf(దత్తాంశం, OSSL_PACKAGE " SSL_read: %s, errno %d",
              error_బఫరు, sockerr);
        ఫలితం = CURLE_RECV_ERROR;
        వెళ్ళు బహిర్గతం;
      }
      లేకపోతే ఒకవేళ(err == SSL_ERROR_SYSCALL) {
        ఒకవేళ(octx->io_ఫలితం) {
          /* logging handling in underlying filter already */
          ఫలితం = octx->io_ఫలితం;
        }
        లేకపోతే ఒకవేళ(connssl->peer_closed) {
          failf(దత్తాంశం, "Connection closed abruptly");
          ఫలితం = CURLE_RECV_ERROR;
        }
        లేకపోతే {
          /* We should no longer get here nowadays, but handle
           * the error in case of some weirdness in the OSSL stack */
          పూర్ణాంకం sockerr = SOCKERRNO;
          ఒకవేళ(sockerr)
            curlx_strerror(sockerr, error_బఫరు, పరిమాణం(error_బఫరు));
          లేకపోతే {
            curl_msnprintf(error_బఫరు, పరిమాణం(error_బఫరు),
                           "Connection closed abruptly");
          }
          failf(దత్తాంశం, OSSL_PACKAGE " SSL_read: %s, errno %d",
                error_బఫరు, sockerr);
          ఫలితం = CURLE_RECV_ERROR;
        }
        వెళ్ళు బహిర్గతం;
      }
    }
  }

బహిర్గతం:
  ఒకవేళ((!ఫలితం && !*pnread) || (ఫలితం == CURLE_AGAIN)) {
    /* This happens when:
     * - we read an EOF
     * - OpenSSLs buffers are empty, there is no more data
     * - OpenSSL read is blocked on writing something first
     * - an incomplete TLS packet is buffered that cannot be read
     *   until more data arrives */
    connssl->ఆదానం_pending = FALSE;
  }
  CURL_TRC_CF(దత్తాంశం, cf, "ossl_recv(len=%zu) -> %d, %zu (in_pending=%d)",
              buffersize, (పూర్ణాంకం)ఫలితం, *pnread, connssl->ఆదానం_pending);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ossl_get_channel_binding(నిర్మాణం Curl_easy *దత్తాంశం,
                                         int8_t sockindex,
                                         నిర్మాణం dynbuf *binding)
{
  X509 *cert;
  పూర్ణాంకం mdnid;
  bool no_digest_acceptable = FALSE;
  స్థిరరాశి EVP_MD *algo_type = ఖాళీ;
  స్థిరరాశి అక్షరం *algo_name = ఖాళీ;
  గుర్తులేని పూర్ణాంకం నిడివి;
  గుర్తులేని అక్షరం buf[EVP_MAX_MD_కొలత];

  స్థిర స్థిరరాశి అక్షరం ఉపసర్గ[] = "tls-server-end-point:";
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  నిర్మాణం Curl_cfilter *cf = conn->cfilter[sockindex];
  నిర్మాణం ossl_ctx *octx = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  చేయి {
    స్థిరరాశి నిర్మాణం Curl_cftype *cft = cf->cft;
    నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;

    ఒకవేళ(cft->name && !strcmp(cft->name, "SSL")) {
      octx = (నిర్మాణం ossl_ctx *)connssl->backend;
      ఆపు;
    }

    cf = cf->తదుపరి;
  } అయితే(cf);

  ఒకవేళ(!octx) {
    failf(దత్తాంశం, "Failed to find the SSL filter");
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  cert = SSL_get1_peer_certificate(octx->ssl);
  ఒకవేళ(!cert)
    /* No server certificate, do not do channel binding */
    తిరిగి CURLE_OK;

#నిర్వచితమైతే HAVE_OPENSSL3
  {
    పూర్ణాంకం pknid, secbits;
    uint32_t flags;
    EVP_PKEY *pkey = X509_get0_pubkey(cert);

    ఒకవేళ(!X509_get_signature_info(cert, &mdnid, &pknid, &secbits, &flags)) {
      failf(దత్తాంశం, "certificate signature algorithm not recognized");
      ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
      వెళ్ళు బహిర్గతం;
    }

    ఒకవేళ(mdnid != NID_undef) {
      ఒకవేళ(mdnid == NID_md5 || mdnid == NID_sha1) {
        algo_type = EVP_sha256();
      }
      లేకపోతే
        algo_type = EVP_get_digestbynid(mdnid);
    }
    లేకపోతే ఒకవేళ(pkey && !EVP_PKEY_is_a(pkey, OBJ_nid2sn(pknid))) {
      /* The cert's pkey is different from the algorithm used to sign
       * the certificate. Since the reported `mdnid` is undefined, there
       * is no digest algorithm available here. This happens in PQC
       * and is accepted, resulting in no addition to the binding. */
      no_digest_acceptable = TRUE;
    }
    లేకపోతే ఒకవేళ(pkey) {
      /* cert's pkey type is the same as the cert signer (or same family).
       * Ask for the mandatory/advisory digest algorithm for the pkey.
       */
      అక్షరం mdname[128] = "";
      పూర్ణాంకం rc = EVP_PKEY_get_default_digest_name(pkey, mdname, పరిమాణం(mdname));
      bool md_is_undef = !strcmp(mdname, "UNDEF");

      ఒకవేళ(rc == 2 && md_is_undef) {
        /* OpenSSL declares "undef" the *mandatory* digest for this key.
         * This is some PQC shit, accept it, no addition to binding. */
        no_digest_acceptable = TRUE;
      }
      లేకపోతే ఒకవేళ(rc > 0 && mdname[0] != '\0' && !md_is_undef) {
        infof(దత్తాంశం, "Digest algorithm : %s%s (derived from public key)"
              ", but unavailable",
              mdname, rc == 2 ? " [mandatory]" : " [advisory]");
      }
    }
  }
#లేకపోతే /* HAVE_OPENSSL3 */

  ఒకవేళ(!OBJ_కనుగొను_sigid_algs(X509_get_signature_nid(cert), &mdnid, ఖాళీ)) {
    failf(దత్తాంశం,
          "Unable to find digest NID for certificate signature algorithm");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు బహిర్గతం;
  }

  /* https://datatracker.ietf.org/doc/html/rfc5929#section-4.1 */
  ఒకవేళ(mdnid == NID_md5 || mdnid == NID_sha1) {
    algo_type = EVP_sha256();
  }
  లేకపోతే {
    algo_type = EVP_get_digestbynid(mdnid);
    ఒకవేళ(!algo_type) {
      algo_name = OBJ_nid2sn(mdnid);
      failf(దత్తాంశం, "Could not find digest algorithm %s (NID %d)",
            algo_name ? algo_name : "(null)", mdnid);
      ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
      వెళ్ళు బహిర్గతం;
    }
  }

#అయితే_ముగింపు /* HAVE_OPENSSL3, else */

  ఒకవేళ(!algo_type) {
    ఒకవేళ(no_digest_acceptable) {
      infof(దత్తాంశం, "certificate exposes no signing digest algorithm, "
            "nothing to add to channel binding");
      ఫలితం = CURLE_OK;
      వెళ్ళు బహిర్గతం;
    }
    /* unacceptable, something is wrong, fail */
    algo_name = OBJ_nid2sn(mdnid);
    failf(దత్తాంశం, "Unable to find digest algorithm %s (NID %d) "
          "for channel binding", algo_name ? algo_name : "(null)", mdnid);
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(!X509_digest(cert, algo_type, buf, &నిడివి)) {
    failf(దత్తాంశం, "X509_digest() failed for channel binding");
    ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    వెళ్ళు బహిర్గతం;
  }

  /* Append "tls-server-end-point:" */
  ఫలితం = curlx_dyn_addn(binding, ఉపసర్గ, CURL_CSTRLEN(ఉపసర్గ));
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  /* Append digest */
  ఫలితం = curlx_dyn_addn(binding, buf, నిడివి);

బహిర్గతం:
  X509_free(cert);
  తిరిగి ఫలితం;
}

size_t Curl_ossl_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత)
{
#నిర్వచితమైతే LIBRESSL_సంస్కరణ_సంఖ్య
  అక్షరం *p;
  size_t లెక్క;
  స్థిరరాశి అక్షరం *ver = Open_SSL_సంస్కరణ(OPENSSL_సంస్కరణ);
  స్థిర స్థిరరాశి అక్షరం expected[] = OSSL_PACKAGE " "; /* ie "LibreSSL " */
  ఒకవేళ(curl_strnequal(ver, expected, CURL_CSTRLEN(expected))) {
    ver += CURL_CSTRLEN(expected);
  }
  లెక్క = curl_msnprintf(బఫరు, కొలత, "%s/%s", OSSL_PACKAGE, ver);
  కొరకు(p = బఫరు; *p; ++p) {
    ఒకవేళ(ISBLANK(*p))
      *p = '_';
  }
  తిరిగి లెక్క;
#లేదావేళ defined(OPENSSL_IS_AWSLC)
  తిరిగి curl_msnprintf(బఫరు, కొలత, "%s/%s",
                        OSSL_PACKAGE, AWSLC_సంస్కరణ_సంఖ్య_STRING);
#లేదావేళ defined(OPENSSL_IS_BORINGSSL)
#నిర్వచితమైతే CURL_BORINGSSL_సంస్కరణ
  తిరిగి curl_msnprintf(బఫరు, కొలత, "%s/%s",
                        OSSL_PACKAGE, CURL_BORINGSSL_సంస్కరణ);
#లేకపోతే
  తిరిగి curl_msnprintf(బఫరు, కొలత, OSSL_PACKAGE);
#అయితే_ముగింపు
#లేకపోతే /* OpenSSL 3+ */
  తిరిగి curl_msnprintf(బఫరు, కొలత, "%s/%s",
                        OSSL_PACKAGE, Open_SSL_సంస్కరణ(OPENSSL_సంస్కరణ_STRING));
#అయితే_ముగింపు
}

/* can be called with data == NULL */
స్థిర CURLcode ossl_random(నిర్మాణం Curl_easy *దత్తాంశం,
                            గుర్తులేని అక్షరం *entropy, size_t నిడివి)
{
  పూర్ణాంకం rc;
  ఒకవేళ(దత్తాంశం) {
    ఒకవేళ(ossl_విత్తనం(దత్తాంశం)) /* Initiate the seed if not already done */
      తిరిగి CURLE_FAILED_INIT; /* could not seed for some reason */
  }
  లేకపోతే {
    ఒకవేళ(!rand_enough())
      తిరిగి CURLE_FAILED_INIT;
  }
  /* RAND_bytes() returns 1 on success, 0 otherwise. */
  rc = RAND_బైట్లు(entropy, (ossl_valsize_t)curlx_uztosi(నిడివి));
  తిరిగి rc == 1 ? CURLE_OK : CURLE_FAILED_INIT;
}

స్థిర CURLcode ossl_sha256sum(స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                               size_t len,
                               గుర్తులేని అక్షరం *sha256sum /* output */,
                               size_t వాడని)
{
  CURLcode ఫలితం = CURLE_OK;
  EVP_MD_CTX *mdctx;
  (శూన్య)వాడని;

  mdctx = EVP_MD_CTX_new();
  ఒకవేళ(!mdctx)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  ఒకవేళ(!EVP_DigestInit_ex(mdctx, EVP_sha256(), ఖాళీ)) {
    ఫలితం = CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(!EVP_DigestUpdate(mdctx, ఆదానం, len) ||
     !EVP_DigestFinal_ex(mdctx, sha256sum, ఖాళీ))
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
బహిర్గతం:
  EVP_MD_CTX_free(mdctx);
  తిరిగి ఫలితం;
}

స్థిర bool ossl_cert_స్థితి_request(శూన్య)
{
#నిర్వచితం_కాకపోతే OPENSSL_NO_OCSP
  తిరిగి TRUE;
#లేకపోతే
  తిరిగి FALSE;
#అయితే_ముగింపు
}

స్థిర శూన్య *ossl_get_internals(నిర్మాణం ssl_connect_దత్తాంశం *connssl,
                                CURLINFO info)
{
  /* Legacy: CURLINFO_TLS_SESSION must return an SSL_CTX pointer. */
  నిర్మాణం ossl_ctx *octx = (నిర్మాణం ossl_ctx *)connssl->backend;
  DEBUGASSERT(octx);
  తిరిగి info == CURLINFO_TLS_SESSION ?
    (శూన్య *)octx->ssl_ctx : (శూన్య *)octx->ssl;
}

స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_openssl = {
  { CURLSSLBACKEND_OPENSSL, "openssl" }, /* info */

  SSLSUPP_CA_మార్గం |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_SSL_CTX |
#నిర్వచితమైతే HAVE_SSL_CTX_సమితి_CIPHERSUITES
  SSLSUPP_TLS13_CIPHERSUITES |
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SSL_CTX_SET1_SIGALGS
  SSLSUPP_SIGNATURE_ALGORITHMS |
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SSL_SET1_ECH_CONFIG_LIST
  SSLSUPP_ECH |
#అయితే_ముగింపు
  SSLSUPP_CA_CACHE |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_ISSUERCERT_BLOB |
  SSLSUPP_SSL_EC_CURVES |
  SSLSUPP_CRLFILE,

  పరిమాణం(నిర్మాణం ossl_ctx),

  ossl_init,                /* init */
  ossl_cleanup,             /* cleanup */
  Curl_ossl_సంస్కరణ,        /*సంస్కరణ*/
  ossl_shutdown,            /* shutdown */
  ossl_దత్తాంశం_pending,        /* data_pending */
  ossl_random,              /* random */
  ossl_cert_స్థితి_request, /* cert_status_request */
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

#అయితే_ముగింపు /* USE_OPENSSL */
