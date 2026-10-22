#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SSLUSE_H
#నిర్వచించు శీర్షిక_CURL_SSLUSE_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_OPENSSL

#నిర్వచితమైతే USE_WIN32_CRYPTO
#అన్తర్భావ <wincrypt.h>
/* If <wincrypt.h> is included directly, or indirectly via <schannel.h>,
 * <winldap.h>, <iphlpapi.h>, or something else, <wincrypt.h> does this:
 *   #define X509_NAME  ((LPCSTR)7)
 *
 * In AWS-LC/BoringSSL's <openssl/base.h> there is:
 *  typedef struct X509_name_st X509_NAME;
 *  etc.
 *
 * The redefined symbols break these OpenSSL headers when included after
 * <wincrypt.h>.
 * The workaround is to undefine those defines here (and only here).
 *
 * For unity builds it may need to be repeated elsewhere too, e.g. in ldap.c,
 * to apply to other sources using OpenSSL includes. Each compilation unit
 * needs undefine them between the first <wincrypt.h> include and the first
 * OpenSSL include.
 *
 * OpenSSL does this in <openssl/ssl.h> and <openssl/x509v3.h>, but it
 * also does the #undef by including <openssl/ossl_typ.h>. <3.1.0 only does
 * it on the first include.
 *
 * LibreSSL automatically undefines these symbols before using them.
 */
#నిర్వచనం_రద్దు X509_NAME
#నిర్వచనం_రద్దు X509_EXTENSIONS
#నిర్వచనం_రద్దు PKCS7_ISSUER_AND_SERIAL
#నిర్వచనం_రద్దు PKCS7_SIGNER_INFO
#నిర్వచనం_రద్దు OCSP_REQUEST
#నిర్వచనం_రద్దు OCSP_RESPONSE
#అయితే_ముగింపు /* USE_WIN32_CRYPTO */

/*
 * This header should only be needed to get included by vtls.c, openssl.c
 * and ngtcp2.c
 */
#అన్తర్భావ <openssl/opensslv.h>
#అన్తర్భావ <openssl/ossl_typ.h>
#అన్తర్భావ <openssl/ssl.h>

#అన్తర్భావ "urldata.h"

#ఒకవేళ OPENSSL_సంస్కరణ_సంఖ్య >= 0x30000000L
#నిర్వచించు HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#అయితే_ముగింపు

#ఒకవేళ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#నిర్వచించు HAVE_BORINGSSL_LIKE
#అయితే_ముగింపు

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#ఒకవేళ OPENSSL_సంస్కరణ_సంఖ్య >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#నిర్వచించు HAVE_KEYLOG_UPSTREAM
#అయితే_ముగింపు

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#నిర్వచితం_కాకపోతే LIBRESSL_సంస్కరణ_సంఖ్య
#నిర్వచించు HAVE_KEYLOG_CALLBACK
#అయితే_ముగింపు

/* Check for OpenSSL 1.1.1 which has early data support. */
#నిర్వచనం_రద్దు HAVE_OPENSSL_EARLYDATA
#ఒకవేళ defined(TLS1_3_సంస్కరణ) && !defined(HAVE_BORINGSSL_LIKE)
#నిర్వచించు HAVE_OPENSSL_EARLYDATA
#అయితే_ముగింపు

#నిర్వచితమైతే LIBRESSL_సంస్కరణ_సంఖ్య
రకనిర్వచనం దీర్ఘ ctx_option_t;
#లేదావేళ defined(HAVE_BORINGSSL_LIKE)
రకనిర్వచనం uint32_t ctx_option_t;
#లేకపోతే
రకనిర్వచనం uint64_t ctx_option_t;
#అయితే_ముగింపు

నిర్మాణం alpn_spec;
నిర్మాణం ssl_peer;
నిర్మాణం Curl_ssl_session;

నిర్మాణం provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  శూన్య *engine;
  /* void instead of OSSL_PROVIDER */
  శూన్య *provider;
  శూన్య *baseprov;
  శూన్య *libctx;
  అక్షరం *propq; /* for a provider */
  బిట్టు(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
నిర్మాణం ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_పద్ధతి *bio_పద్ధతి;
  నిర్మాణం provider p;
  CURLcode io_ఫలితం;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  పూర్ణాంకం blocked_ssl_write_len;
#ఒకవేళ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* Set to true once a valid keylog entry has been created to avoid dupes.
     This is a bool and not a bitfield because it is passed by address. */
  bool keylog_పూర్తి;
#అయితే_ముగింపు
  బిట్టు(x509_store_అమరిక);            /* x509 store has been set up */
  బిట్టు(store_is_ఖాళీలు);              /* no certs/paths/blobs in x509 store */
  బిట్టు(reused_session);              /* session-ID was reused for this */
#నిర్వచితమైతే USE_APPLE_SECTRUST
  బిట్టు(sectrust_verified);           /* peer was verified by sectrust */
  బిట్టు(sectrust_session);            /* session from sectrust verified peer */
#అయితే_ముగింపు
};

size_t Curl_ossl_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత);

రకనిర్వచనం CURLcode Curl_ossl_ctx_అమరిక_cb(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        శూన్య *user_దత్తాంశం);

రకనిర్వచనం పూర్ణాంకం Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
రకనిర్వచనం CURLcode Curl_ossl_init_session_reuse_cb(నిర్మాణం Curl_cfilter *cf,
                                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                                 నిర్మాణం alpn_spec *alpns,
                                                 నిర్మాణం Curl_ssl_session *scs,
                                                 bool *do_early_దత్తాంశం);

CURLcode Curl_ossl_ctx_init(నిర్మాణం ossl_ctx *octx,
                            నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ssl_peer *peer,
                            స్థిరరాశి నిర్మాణం alpn_spec *alpns_requested,
                            Curl_ossl_ctx_అమరిక_cb *cb_అమరిక,
                            శూన్య *cb_user_దత్తాంశం,
                            Curl_ossl_new_session_cb *cb_new_session,
                            శూన్య *ssl_user_దత్తాంశం,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#నిర్వచితం_కాకపోతే HAVE_OPENSSL3
#నిర్వచించు SSL_get1_peer_certificate SSL_get_peer_certificate
#అయితే_ముగింపు

బాహ్య స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_అమరిక_x509_store(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_కూడు_session(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం ossl_ctx *octx,
                               స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                               SSL_SESSION *session,
                               స్థిరరాశి అక్షరం *alpn,
                               గుర్తులేని అక్షరం *quic_tp,
                               size_t quic_tp_len,
                               నిర్మాణం Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ossl_ctx *octx,
                                   నిర్మాణం ssl_peer *peer);

/* Report properties of a successful handshake */
శూన్య Curl_ossl_report_handshake(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం ossl_ctx *octx);

శూన్య Curl_osslctx_cleanup(నిర్మాణం ossl_ctx *octx);

#అయితే_ముగింపు /* USE_OPENSSL */
#అయితే_ముగింపు /* HEADER_CURL_SSLUSE_H */
