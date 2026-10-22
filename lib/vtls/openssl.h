#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SSLUSE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SSLUSE_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WIN32_CRYPTO
#ಅನ್ತರ್ಭಾವ <wincrypt.h>
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
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು X509_NAME
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು X509_EXTENSIONS
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು PKCS7_ISSUER_AND_SERIAL
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು PKCS7_SIGNER_INFO
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು OCSP_REQUEST
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು OCSP_RESPONSE
#ಆದರೆ_ಅಂತ್ಯ /* USE_WIN32_CRYPTO */

/*
 * This header should only be needed to get included by vtls.c, openssl.c
 * and ngtcp2.c
 */
#ಅನ್ತರ್ಭಾವ <openssl/opensslv.h>
#ಅನ್ತರ್ಭಾವ <openssl/ossl_typ.h>
#ಅನ್ತರ್ಭಾವ <openssl/ssl.h>

#ಅನ್ತರ್ಭಾವ "urldata.h"

#ಆದರೆ OPENSSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x30000000L
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_BORINGSSL_LIKE
#ಆದರೆ_ಅಂತ್ಯ

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#ಆದರೆ OPENSSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_KEYLOG_UPSTREAM
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_KEYLOG_CALLBACK
#ಆದರೆ_ಅಂತ್ಯ

/* Check for OpenSSL 1.1.1 which has early data support. */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು HAVE_OPENSSL_EARLYDATA
#ಆದರೆ defined(TLS1_3_ಆವೃತ್ತಿ) && !defined(HAVE_BORINGSSL_LIKE)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_OPENSSL_EARLYDATA
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ ctx_option_t;
#ಅಥವಾಆದರೆ defined(HAVE_BORINGSSL_LIKE)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t ctx_option_t;
#ಇಲ್ಲದಿದ್ದರೆ
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64_t ctx_option_t;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ alpn_spec;
ರಚನೆ ssl_peer;
ರಚನೆ Curl_ssl_session;

ರಚನೆ provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  ಶೂನ್ಯ *engine;
  /* void instead of OSSL_PROVIDER */
  ಶೂನ್ಯ *provider;
  ಶೂನ್ಯ *baseprov;
  ಶೂನ್ಯ *libctx;
  ಅಕ್ಷರ *propq; /* for a provider */
  ಬಿಟ್(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
ರಚನೆ ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_ವಿಧಾನ *bio_ವಿಧಾನ;
  ರಚನೆ provider p;
  CURLcode io_ಫಲಿತಾಂಶ;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  ಪೂರ್ಣಾಂಕ blocked_ssl_write_len;
#ಆದರೆ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* Set to true once a valid keylog entry has been created to avoid dupes.
     This is a bool and not a bitfield because it is passed by address. */
  bool keylog_ಮುಗಿದ;
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(x509_store_ಸಿದ್ಧತೆ);            /* x509 store has been set up */
  ಬಿಟ್(store_is_ಖಾಲಿಗಳು);              /* no certs/paths/blobs in x509 store */
  ಬಿಟ್(reused_session);              /* session-ID was reused for this */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
  ಬಿಟ್(sectrust_verified);           /* peer was verified by sectrust */
  ಬಿಟ್(sectrust_session);            /* session from sectrust verified peer */
#ಆದರೆ_ಅಂತ್ಯ
};

size_t Curl_ossl_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_ossl_ctx_ಸಿದ್ಧತೆ_cb(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ಶೂನ್ಯ *user_ದತ್ತಾಂಶ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ CURLcode Curl_ossl_init_session_reuse_cb(ರಚನೆ Curl_cfilter *cf,
                                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                 ರಚನೆ alpn_spec *alpns,
                                                 ರಚನೆ Curl_ssl_session *scs,
                                                 bool *do_early_ದತ್ತಾಂಶ);

CURLcode Curl_ossl_ctx_init(ರಚನೆ ossl_ctx *octx,
                            ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ssl_peer *peer,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpns_requested,
                            Curl_ossl_ctx_ಸಿದ್ಧತೆ_cb *cb_ಸಿದ್ಧತೆ,
                            ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ,
                            Curl_ossl_new_session_cb *cb_new_session,
                            ಶೂನ್ಯ *ssl_user_ದತ್ತಾಂಶ,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_OPENSSL3
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_get1_peer_certificate SSL_get_peer_certificate
#ಆದರೆ_ಅಂತ್ಯ

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_ಸಿದ್ಧತೆ_x509_store(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_ಕೂಡಿಸು_session(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ ossl_ctx *octx,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                               SSL_SESSION *session,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn,
                               ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *quic_tp,
                               size_t quic_tp_len,
                               ರಚನೆ Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ossl_ctx *octx,
                                   ರಚನೆ ssl_peer *peer);

/* Report properties of a successful handshake */
ಶೂನ್ಯ Curl_ossl_report_handshake(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ ossl_ctx *octx);

ಶೂನ್ಯ Curl_osslctx_cleanup(ರಚನೆ ossl_ctx *octx);

#ಆದರೆ_ಅಂತ್ಯ /* USE_OPENSSL */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SSLUSE_H */
