#جے_بنا_تعریف سرخی_CURL_SSLUSE_H
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
#ضمن "curl_setup.h"

#جے_تعریف_شدہ USE_OPENSSL

#جے_تعریف_شدہ USE_WIN32_CRYPTO
#ضمن <wincrypt.h>
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
#تعریف_رد X509_NAME
#تعریف_رد X509_EXTENSIONS
#تعریف_رد PKCS7_ISSUER_AND_SERIAL
#تعریف_رد PKCS7_SIGNER_INFO
#تعریف_رد OCSP_REQUEST
#تعریف_رد OCSP_RESPONSE
#جے_انت /* USE_WIN32_CRYPTO */

/*
 * This header should only be needed to get included by vtls.c, openssl.c
 * and ngtcp2.c
 */
#ضمن <openssl/opensslv.h>
#ضمن <openssl/ossl_typ.h>
#ضمن <openssl/ssl.h>

#ضمن "urldata.h"

#إذا OPENSSL_نسخہ_نمبر >= 0x30000000L
#تعریف_کرو HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#جے_انت

#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#تعریف_کرو HAVE_BORINGSSL_LIKE
#جے_انت

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#إذا OPENSSL_نسخہ_نمبر >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#تعریف_کرو HAVE_KEYLOG_UPSTREAM
#جے_انت

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#جے_بنا_تعریف LIBRESSL_نسخہ_نمبر
#تعریف_کرو HAVE_KEYLOG_CALLBACK
#جے_انت

/* Check for OpenSSL 1.1.1 which has early data support. */
#تعریف_رد HAVE_OPENSSL_EARLYDATA
#إذا defined(TLS1_3_نسخہ) && !defined(HAVE_BORINGSSL_LIKE)
#تعریف_کرو HAVE_OPENSSL_EARLYDATA
#جے_انت

#جے_تعریف_شدہ LIBRESSL_نسخہ_نمبر
تعريفنوع لما ctx_option_t;
#وإلاإذا defined(HAVE_BORINGSSL_LIKE)
تعريفنوع uint32_t ctx_option_t;
#وإلا
تعريفنوع uint64_t ctx_option_t;
#جے_انت

بنية alpn_spec;
بنية ssl_peer;
بنية Curl_ssl_session;

بنية provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  فراغ *engine;
  /* void instead of OSSL_PROVIDER */
  فراغ *provider;
  فراغ *baseprov;
  فراغ *libctx;
  حرف *propq; /* for a provider */
  ثنائی_ہندسہ(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
بنية ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_طریقہ *bio_طریقہ;
  بنية provider p;
  CURLcode io_نتیجہ;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  صحيح blocked_ssl_write_len;
#إذا !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* Set to true once a valid keylog entry has been created to avoid dupes.
     This is a bool and not a bitfield because it is passed by address. */
  bool keylog_پورا;
#جے_انت
  ثنائی_ہندسہ(x509_store_تیاری);            /* x509 store has been set up */
  ثنائی_ہندسہ(store_is_خالی_تھاواں);              /* no certs/paths/blobs in x509 store */
  ثنائی_ہندسہ(reused_session);              /* session-ID was reused for this */
#جے_تعریف_شدہ USE_APPLE_SECTRUST
  ثنائی_ہندسہ(sectrust_verified);           /* peer was verified by sectrust */
  ثنائی_ہندسہ(sectrust_session);            /* session from sectrust verified peer */
#جے_انت
};

size_t Curl_ossl_نسخہ(حرف *بفر, size_t ناپ);

تعريفنوع CURLcode Curl_ossl_ctx_تیاری_cb(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *انکڑے,
                                        فراغ *user_انکڑے);

تعريفنوع صحيح Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
تعريفنوع CURLcode Curl_ossl_init_session_reuse_cb(بنية Curl_cfilter *cf,
                                                 بنية Curl_easy *انکڑے,
                                                 بنية alpn_spec *alpns,
                                                 بنية Curl_ssl_session *scs,
                                                 bool *do_early_انکڑے);

CURLcode Curl_ossl_ctx_init(بنية ossl_ctx *octx,
                            بنية Curl_cfilter *cf,
                            بنية Curl_easy *انکڑے,
                            بنية ssl_peer *peer,
                            ثابت بنية alpn_spec *alpns_requested,
                            Curl_ossl_ctx_تیاری_cb *cb_تیاری,
                            فراغ *cb_user_انکڑے,
                            Curl_ossl_new_session_cb *cb_new_session,
                            فراغ *ssl_user_انکڑے,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#جے_بنا_تعریف HAVE_OPENSSL3
#تعریف_کرو SSL_get1_peer_certificate SSL_get_peer_certificate
#جے_انت

خارجي ثابت بنية Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_تیاری_x509_store(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   بنية ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_جوڑو_session(بنية Curl_cfilter *cf,
                               بنية Curl_easy *انکڑے,
                               بنية ossl_ctx *octx,
                               ثابت حرف *ssl_peer_کلید,
                               SSL_SESSION *session,
                               ثابت حرف *alpn,
                               بنانشانی حرف *quic_tp,
                               size_t quic_tp_len,
                               بنية Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   بنية ossl_ctx *octx,
                                   بنية ssl_peer *peer);

/* Report properties of a successful handshake */
فراغ Curl_ossl_report_handshake(بنية Curl_easy *انکڑے, بنية ossl_ctx *octx);

فراغ Curl_osslctx_cleanup(بنية ossl_ctx *octx);

#جے_انت /* USE_OPENSSL */
#جے_انت /* HEADER_CURL_SSLUSE_H */
