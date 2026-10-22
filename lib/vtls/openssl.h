#اگر_غیر_تعریف_شدہ سرخی_CURL_SSLUSE_H
#تعریف_کرو سرخی_CURL_SSLUSE_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_OPENSSL

#اگر_تعریف_شدہ USE_WIN32_CRYPTO
#شامل <wincrypt.h>
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
#تعریف_منسوخ X509_NAME
#تعریف_منسوخ X509_EXTENSIONS
#تعریف_منسوخ PKCS7_ISSUER_AND_SERIAL
#تعریف_منسوخ PKCS7_SIGNER_INFO
#تعریف_منسوخ OCSP_REQUEST
#تعریف_منسوخ OCSP_RESPONSE
#اگر_ختم /* USE_WIN32_CRYPTO */

/*
 * This header should only be needed to get included by vtls.c, openssl.c
 * and ngtcp2.c
 */
#شامل <openssl/opensslv.h>
#شامل <openssl/ossl_typ.h>
#شامل <openssl/ssl.h>

#شامل "urldata.h"

#اگر OPENSSL_نسخہ_نمبر >= 0x30000000L
#تعریف_کرو HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#اگر_ختم

#اگر defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#تعریف_کرو HAVE_BORINGSSL_LIKE
#اگر_ختم

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#اگر OPENSSL_نسخہ_نمبر >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#تعریف_کرو HAVE_KEYLOG_UPSTREAM
#اگر_ختم

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#اگر_غیر_تعریف_شدہ LIBRESSL_نسخہ_نمبر
#تعریف_کرو HAVE_KEYLOG_CALLBACK
#اگر_ختم

/* Check for OpenSSL 1.1.1 which has early data support. */
#تعریف_منسوخ HAVE_OPENSSL_EARLYDATA
#اگر defined(TLS1_3_نسخہ) && !defined(HAVE_BORINGSSL_LIKE)
#تعریف_کرو HAVE_OPENSSL_EARLYDATA
#اگر_ختم

#اگر_تعریف_شدہ LIBRESSL_نسخہ_نمبر
قسمتعریف طویل ctx_option_t;
#وگرنہ_اگر defined(HAVE_BORINGSSL_LIKE)
قسمتعریف uint32_t ctx_option_t;
#ورنہ
قسمتعریف uint64_t ctx_option_t;
#اگر_ختم

ساخت alpn_spec;
ساخت ssl_peer;
ساخت Curl_ssl_session;

ساخت provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  خالی *engine;
  /* void instead of OSSL_PROVIDER */
  خالی *provider;
  خالی *baseprov;
  خالی *libctx;
  حرف *propq; /* for a provider */
  ثنائی_ہندسہ(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
ساخت ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_طریقہ *bio_طریقہ;
  ساخت provider p;
  CURLcode io_نتیجہ;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  صحیح blocked_ssl_write_len;
#اگر !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* Set to true once a valid keylog entry has been created to avoid dupes.
     This is a bool and not a bitfield because it is passed by address. */
  bool keylog_مکمل;
#اگر_ختم
  ثنائی_ہندسہ(x509_store_تیاری);            /* x509 store has been set up */
  ثنائی_ہندسہ(store_is_خالی_خانے);              /* no certs/paths/blobs in x509 store */
  ثنائی_ہندسہ(reused_session);              /* session-ID was reused for this */
#اگر_تعریف_شدہ USE_APPLE_SECTRUST
  ثنائی_ہندسہ(sectrust_verified);           /* peer was verified by sectrust */
  ثنائی_ہندسہ(sectrust_session);            /* session from sectrust verified peer */
#اگر_ختم
};

size_t Curl_ossl_نسخہ(حرف *بفر, size_t سائز);

قسمتعریف CURLcode Curl_ossl_ctx_تیاری_cb(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        خالی *user_ڈیٹا);

قسمتعریف صحیح Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
قسمتعریف CURLcode Curl_ossl_init_session_reuse_cb(ساخت Curl_cfilter *cf,
                                                 ساخت Curl_easy *ڈیٹا,
                                                 ساخت alpn_spec *alpns,
                                                 ساخت Curl_ssl_session *scs,
                                                 bool *do_early_ڈیٹا);

CURLcode Curl_ossl_ctx_init(ساخت ossl_ctx *octx,
                            ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت ssl_peer *peer,
                            ثابت ساخت alpn_spec *alpns_requested,
                            Curl_ossl_ctx_تیاری_cb *cb_تیاری,
                            خالی *cb_user_ڈیٹا,
                            Curl_ossl_new_session_cb *cb_new_session,
                            خالی *ssl_user_ڈیٹا,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#اگر_غیر_تعریف_شدہ HAVE_OPENSSL3
#تعریف_کرو SSL_get1_peer_certificate SSL_get_peer_certificate
#اگر_ختم

خارجی ثابت ساخت Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_تیاری_x509_store(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_جوڑو_session(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت ossl_ctx *octx,
                               ثابت حرف *ssl_peer_کلید,
                               SSL_SESSION *session,
                               ثابت حرف *alpn,
                               غیرعلامتی حرف *quic_tp,
                               size_t quic_tp_len,
                               ساخت Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ساخت ossl_ctx *octx,
                                   ساخت ssl_peer *peer);

/* Report properties of a successful handshake */
خالی Curl_ossl_report_handshake(ساخت Curl_easy *ڈیٹا, ساخت ossl_ctx *octx);

خالی Curl_osslctx_cleanup(ساخت ossl_ctx *octx);

#اگر_ختم /* USE_OPENSSL */
#اگر_ختم /* HEADER_CURL_SSLUSE_H */
