#ifndef HEADER_CURL_SSLUSE_H
#define HEADER_CURL_SSLUSE_H
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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_OPENSSL

#ifdef USE_WIN32_CRYPTO
#അന്തര്ഭാവ <wincrypt.h>
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
#undef X509_NAME
#undef X509_EXTENSIONS
#undef PKCS7_ISSUER_AND_SERIAL
#undef PKCS7_SIGNER_INFO
#undef OCSP_REQUEST
#undef OCSP_RESPONSE
#endif /* USE_WIN32_CRYPTO */

/*
 * This header should only be needed to get included by vtls.c, openssl.c
 * and ngtcp2.c
 */
#അന്തര്ഭാവ <openssl/opensslv.h>
#അന്തര്ഭാവ <openssl/ossl_typ.h>
#അന്തര്ഭാവ <openssl/ssl.h>

#അന്തര്ഭാവ "urldata.h"

#എങ്കിൽ OPENSSL_VERSION_സംഖ്യ >= 0x30000000L
#define HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#endif

#എങ്കിൽ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#define HAVE_BORINGSSL_LIKE
#endif

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#എങ്കിൽ OPENSSL_VERSION_സംഖ്യ >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#define HAVE_KEYLOG_UPSTREAM
#endif

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#ifndef LIBRESSL_VERSION_സംഖ്യ
#define HAVE_KEYLOG_CALLBACK
#endif

/* Check for OpenSSL 1.1.1 which has early data support. */
#undef HAVE_OPENSSL_EARLYDATA
#എങ്കിൽ defined(TLS1_3_VERSION) && !defined(HAVE_BORINGSSL_LIKE)
#define HAVE_OPENSSL_EARLYDATA
#endif

#ifdef LIBRESSL_VERSION_സംഖ്യ
തരനിർവചനം long ctx_option_t;
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_BORINGSSL_LIKE)
തരനിർവചനം uint32_t ctx_option_t;
#അല്ലെങ്കിൽ
തരനിർവചനം uint64_t ctx_option_t;
#endif

ഘടന alpn_spec;
ഘടന ssl_peer;
ഘടന Curl_ssl_session;

ഘടന provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  ശൂന്യം *engine;
  /* void instead of OSSL_PROVIDER */
  ശൂന്യം *provider;
  ശൂന്യം *baseprov;
  ശൂന്യം *libctx;
  അക്ഷരം *propq; /* for a provider */
  BIT(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
ഘടന ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_METHOD *bio_method;
  ഘടന provider p;
  CURLcode io_result;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  പൂർണ്ണസംഖ്യ blocked_ssl_write_len;
#എങ്കിൽ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* Set to true once a valid keylog entry has been created to avoid dupes.
     This is a bool and not a bitfield because it is passed by address. */
  bool keylog_done;
#endif
  BIT(x509_store_setup);            /* x509 store has been set up */
  BIT(store_is_empty);              /* no certs/paths/blobs in x509 store */
  BIT(reused_session);              /* session-ID was reused for this */
#ifdef USE_APPLE_SECTRUST
  BIT(sectrust_verified);           /* peer was verified by sectrust */
  BIT(sectrust_session);            /* session from sectrust verified peer */
#endif
};

size_t Curl_ossl_version(അക്ഷരം *buffer, size_t size);

തരനിർവചനം CURLcode Curl_ossl_ctx_setup_cb(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ശൂന്യം *user_data);

തരനിർവചനം പൂർണ്ണസംഖ്യ Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
തരനിർവചനം CURLcode Curl_ossl_init_session_reuse_cb(ഘടന Curl_cfilter *cf,
                                                 ഘടന Curl_easy *data,
                                                 ഘടന alpn_spec *alpns,
                                                 ഘടന Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_ossl_ctx_init(ഘടന ossl_ctx *octx,
                            ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            ഘടന ssl_peer *peer,
                            സ്ഥിരം ഘടന alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            ശൂന്യം *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            ശൂന്യം *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#ifndef HAVE_OPENSSL3
#define SSL_get1_peer_certificate SSL_get_peer_certificate
#endif

ബാഹ്യ സ്ഥിരം ഘടന Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_setup_x509_store(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_add_session(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന ossl_ctx *octx,
                               സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                               SSL_SESSION *session,
                               സ്ഥിരം അക്ഷരം *alpn,
                               unsigned അക്ഷരം *quic_tp,
                               size_t quic_tp_len,
                               ഘടന Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ഘടന ossl_ctx *octx,
                                   ഘടന ssl_peer *peer);

/* Report properties of a successful handshake */
ശൂന്യം Curl_ossl_report_handshake(ഘടന Curl_easy *data, ഘടന ossl_ctx *octx);

ശൂന്യം Curl_osslctx_cleanup(ഘടന ossl_ctx *octx);

#endif /* USE_OPENSSL */
#endif /* HEADER_CURL_SSLUSE_H */
