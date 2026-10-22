#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SSLUSE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SSLUSE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WIN32_CRYPTO
#ਅਨ੍ਤਰ੍ਭਾਵ <wincrypt.h>
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
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ X509_NAME
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ X509_EXTENSIONS
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ PKCS7_ISSUER_AND_SERIAL
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ PKCS7_SIGNER_INFO
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ OCSP_REQUEST
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ OCSP_RESPONSE
#ਜੇ_ਅੰਤ /* USE_WIN32_CRYPTO */

/*
 * This header should only be needed to get included by vtls.c, openssl.c
 * and ngtcp2.c
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/opensslv.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/ossl_typ.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <openssl/ssl.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"

#ਜੇ OPENSSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x30000000L
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#ਜੇ_ਅੰਤ

#ਜੇ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_BORINGSSL_LIKE
#ਜੇ_ਅੰਤ

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#ਜੇ OPENSSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_KEYLOG_UPSTREAM
#ਜੇ_ਅੰਤ

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_KEYLOG_CALLBACK
#ਜੇ_ਅੰਤ

/* Check for OpenSSL 1.1.1 which has early data support. */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ HAVE_OPENSSL_EARLYDATA
#ਜੇ defined(TLS1_3_ਸੰਸਕਰਣ) && !defined(HAVE_BORINGSSL_LIKE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_OPENSSL_EARLYDATA
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ ctx_option_t;
#ਜਾਂਜੇ defined(HAVE_BORINGSSL_LIKE)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t ctx_option_t;
#ਨਹੀਂਤਾਂ
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64_t ctx_option_t;
#ਜੇ_ਅੰਤ

ਬਣਤਰ alpn_spec;
ਬਣਤਰ ssl_peer;
ਬਣਤਰ Curl_ssl_session;

ਬਣਤਰ provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  ਖਾਲੀ *engine;
  /* void instead of OSSL_PROVIDER */
  ਖਾਲੀ *provider;
  ਖਾਲੀ *baseprov;
  ਖਾਲੀ *libctx;
  ਅੱਖਰ *propq; /* for a provider */
  ਦੋਆਧਾਰੀ_ਅੰਕ(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
ਬਣਤਰ ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_ਵਿਧੀ *bio_ਵਿਧੀ;
  ਬਣਤਰ provider p;
  CURLcode io_ਨਤੀਜਾ;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  ਪੂਰਨ blocked_ssl_write_len;
#ਜੇ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* Set to true once a valid keylog entry has been created to avoid dupes.
     This is a bool and not a bitfield because it is passed by address. */
  bool keylog_ਪੂਰਾ;
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(x509_store_ਤਿਆਰੀ);            /* x509 store has been set up */
  ਦੋਆਧਾਰੀ_ਅੰਕ(store_is_ਖਾਲੀ_ਥਾਵਾਂ);              /* no certs/paths/blobs in x509 store */
  ਦੋਆਧਾਰੀ_ਅੰਕ(reused_session);              /* session-ID was reused for this */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
  ਦੋਆਧਾਰੀ_ਅੰਕ(sectrust_verified);           /* peer was verified by sectrust */
  ਦੋਆਧਾਰੀ_ਅੰਕ(sectrust_session);            /* session from sectrust verified peer */
#ਜੇ_ਅੰਤ
};

size_t Curl_ossl_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_ossl_ctx_ਤਿਆਰੀ_cb(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਖਾਲੀ *user_ਅੰਕੜੇ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ CURLcode Curl_ossl_init_session_reuse_cb(ਬਣਤਰ Curl_cfilter *cf,
                                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                 ਬਣਤਰ alpn_spec *alpns,
                                                 ਬਣਤਰ Curl_ssl_session *scs,
                                                 bool *do_early_ਅੰਕੜੇ);

CURLcode Curl_ossl_ctx_init(ਬਣਤਰ ossl_ctx *octx,
                            ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ssl_peer *peer,
                            ਅਚਲ ਬਣਤਰ alpn_spec *alpns_requested,
                            Curl_ossl_ctx_ਤਿਆਰੀ_cb *cb_ਤਿਆਰੀ,
                            ਖਾਲੀ *cb_user_ਅੰਕੜੇ,
                            Curl_ossl_new_session_cb *cb_new_session,
                            ਖਾਲੀ *ssl_user_ਅੰਕੜੇ,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_OPENSSL3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_get1_peer_certificate SSL_get_peer_certificate
#ਜੇ_ਅੰਤ

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_ਤਿਆਰੀ_x509_store(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_ਜੋੜੋ_session(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ ossl_ctx *octx,
                               ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                               SSL_SESSION *session,
                               ਅਚਲ ਅੱਖਰ *alpn,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *quic_tp,
                               size_t quic_tp_len,
                               ਬਣਤਰ Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ossl_ctx *octx,
                                   ਬਣਤਰ ssl_peer *peer);

/* Report properties of a successful handshake */
ਖਾਲੀ Curl_ossl_report_handshake(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ ossl_ctx *octx);

ਖਾਲੀ Curl_osslctx_cleanup(ਬਣਤਰ ossl_ctx *octx);

#ਜੇ_ਅੰਤ /* USE_OPENSSL */
#ਜੇ_ਅੰਤ /* HEADER_CURL_SSLUSE_H */
