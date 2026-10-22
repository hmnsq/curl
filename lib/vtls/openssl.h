#که_نه_تعریف_شوی سرلیک_CURL_SSLUSE_H
#تعریف_کړه سرلیک_CURL_SSLUSE_H
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

#که_تعریف_شوی USE_OPENSSL

#که_تعریف_شوی USE_WIN32_CRYPTO
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
#تعریف_لغوه X509_NAME
#تعریف_لغوه X509_EXTENSIONS
#تعریف_لغوه PKCS7_ISSUER_AND_SERIAL
#تعریف_لغوه PKCS7_SIGNER_INFO
#تعریف_لغوه OCSP_REQUEST
#تعریف_لغوه OCSP_RESPONSE
#د_که_پای /* USE_WIN32_CRYPTO */

/*
 * This header should only be needed to get included by vtls.c, openssl.c
 * and ngtcp2.c
 */
#ضمن <openssl/opensslv.h>
#ضمن <openssl/ossl_typ.h>
#ضمن <openssl/ssl.h>

#ضمن "urldata.h"

#إذا OPENSSL_نسخه_شمېره >= 0x30000000L
#تعریف_کړه HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#د_که_پای

#إذا defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#تعریف_کړه HAVE_BORINGSSL_LIKE
#د_که_پای

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#إذا OPENSSL_نسخه_شمېره >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#تعریف_کړه HAVE_KEYLOG_UPSTREAM
#د_که_پای

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#که_نه_تعریف_شوی LIBRESSL_نسخه_شمېره
#تعریف_کړه HAVE_KEYLOG_CALLBACK
#د_که_پای

/* Check for OpenSSL 1.1.1 which has early data support. */
#تعریف_لغوه HAVE_OPENSSL_EARLYDATA
#إذا defined(TLS1_3_نسخه) && !defined(HAVE_BORINGSSL_LIKE)
#تعریف_کړه HAVE_OPENSSL_EARLYDATA
#د_که_پای

#که_تعریف_شوی LIBRESSL_نسخه_شمېره
تعريفنوع اوږد ctx_option_t;
#وإلاإذا defined(HAVE_BORINGSSL_LIKE)
تعريفنوع uint32_t ctx_option_t;
#وإلا
تعريفنوع uint64_t ctx_option_t;
#د_که_پای

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
  بټ(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
بنية ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_لاره *bio_لاره;
  بنية provider p;
  CURLcode io_پایله;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  صحيح blocked_ssl_write_len;
#إذا !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* Set to true once a valid keylog entry has been created to avoid dupes.
     This is a bool and not a bitfield because it is passed by address. */
  bool keylog_بشپړ;
#د_که_پای
  بټ(x509_store_تیاری);            /* x509 store has been set up */
  بټ(store_is_تشه);              /* no certs/paths/blobs in x509 store */
  بټ(reused_session);              /* session-ID was reused for this */
#که_تعریف_شوی USE_APPLE_SECTRUST
  بټ(sectrust_verified);           /* peer was verified by sectrust */
  بټ(sectrust_session);            /* session from sectrust verified peer */
#د_که_پای
};

size_t Curl_ossl_نسخه(حرف *بفر, size_t اندازه);

تعريفنوع CURLcode Curl_ossl_ctx_تیاری_cb(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        فراغ *user_معلومات);

تعريفنوع صحيح Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
تعريفنوع CURLcode Curl_ossl_init_session_reuse_cb(بنية Curl_cfilter *cf,
                                                 بنية Curl_easy *معلومات,
                                                 بنية alpn_spec *alpns,
                                                 بنية Curl_ssl_session *scs,
                                                 bool *do_early_معلومات);

CURLcode Curl_ossl_ctx_init(بنية ossl_ctx *octx,
                            بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            بنية ssl_peer *peer,
                            ثابت بنية alpn_spec *alpns_requested,
                            Curl_ossl_ctx_تیاری_cb *cb_تیاری,
                            فراغ *cb_user_معلومات,
                            Curl_ossl_new_session_cb *cb_new_session,
                            فراغ *ssl_user_معلومات,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#که_نه_تعریف_شوی HAVE_OPENSSL3
#تعریف_کړه SSL_get1_peer_certificate SSL_get_peer_certificate
#د_که_پای

خارجي ثابت بنية Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_تیاری_x509_store(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_جمع_کړه_session(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية ossl_ctx *octx,
                               ثابت حرف *ssl_peer_کلی,
                               SSL_SESSION *session,
                               ثابت حرف *alpn,
                               بینښه حرف *quic_tp,
                               size_t quic_tp_len,
                               بنية Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   بنية ossl_ctx *octx,
                                   بنية ssl_peer *peer);

/* Report properties of a successful handshake */
فراغ Curl_ossl_report_handshake(بنية Curl_easy *معلومات, بنية ossl_ctx *octx);

فراغ Curl_osslctx_cleanup(بنية ossl_ctx *octx);

#د_که_پای /* USE_OPENSSL */
#د_که_پای /* HEADER_CURL_SSLUSE_H */
