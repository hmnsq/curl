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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_OPENSSL

#ifdef USE_WIN32_CRYPTO
#અન્તર્ભાવ <wincrypt.h>
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
#અન્તર્ભાવ <openssl/opensslv.h>
#અન્તર્ભાવ <openssl/ossl_typ.h>
#અન્તર્ભાવ <openssl/ssl.h>

#અન્તર્ભાવ "urldata.h"

#જો OPENSSL_VERSION_સંખ્યા >= 0x30000000L
#define HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#endif

#જો defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#define HAVE_BORINGSSL_LIKE
#endif

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#જો OPENSSL_VERSION_સંખ્યા >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#define HAVE_KEYLOG_UPSTREAM
#endif

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#ifndef LIBRESSL_VERSION_સંખ્યા
#define HAVE_KEYLOG_CALLBACK
#endif

/* Check for OpenSSL 1.1.1 which has early data support. */
#undef HAVE_OPENSSL_EARLYDATA
#જો defined(TLS1_3_VERSION) && !defined(HAVE_BORINGSSL_LIKE)
#define HAVE_OPENSSL_EARLYDATA
#endif

#ifdef LIBRESSL_VERSION_સંખ્યા
પ્રકારવ્યાખ્યા long ctx_option_t;
#અથવાજો defined(HAVE_BORINGSSL_LIKE)
પ્રકારવ્યાખ્યા uint32_t ctx_option_t;
#નહીંતર
પ્રકારવ્યાખ્યા uint64_t ctx_option_t;
#endif

સંરચના alpn_spec;
સંરચના ssl_peer;
સંરચના Curl_ssl_session;

સંરચના provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  શૂન્ય *engine;
  /* void instead of OSSL_PROVIDER */
  શૂન્ય *provider;
  શૂન્ય *baseprov;
  શૂન્ય *libctx;
  અક્ષર *propq; /* for a provider */
  BIT(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
સંરચના ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_METHOD *bio_method;
  સંરચના provider p;
  CURLcode io_result;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  પૂર્ણાંક blocked_ssl_write_len;
#જો !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
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

size_t Curl_ossl_version(અક્ષર *buffer, size_t size);

પ્રકારવ્યાખ્યા CURLcode Curl_ossl_ctx_setup_cb(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        શૂન્ય *user_data);

પ્રકારવ્યાખ્યા પૂર્ણાંક Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
પ્રકારવ્યાખ્યા CURLcode Curl_ossl_init_session_reuse_cb(સંરચના Curl_cfilter *cf,
                                                 સંરચના Curl_easy *data,
                                                 સંરચના alpn_spec *alpns,
                                                 સંરચના Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_ossl_ctx_init(સંરચના ossl_ctx *octx,
                            સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            સંરચના ssl_peer *peer,
                            અચલ સંરચના alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            શૂન્ય *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            શૂન્ય *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#ifndef HAVE_OPENSSL3
#define SSL_get1_peer_certificate SSL_get_peer_certificate
#endif

બાહ્ય અચલ સંરચના Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_setup_x509_store(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_add_session(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના ossl_ctx *octx,
                               અચલ અક્ષર *ssl_peer_ચાવી,
                               SSL_SESSION *session,
                               અચલ અક્ષર *alpn,
                               unsigned અક્ષર *quic_tp,
                               size_t quic_tp_len,
                               સંરચના Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   સંરચના ossl_ctx *octx,
                                   સંરચના ssl_peer *peer);

/* Report properties of a successful handshake */
શૂન્ય Curl_ossl_report_handshake(સંરચના Curl_easy *data, સંરચના ossl_ctx *octx);

શૂન્ય Curl_osslctx_cleanup(સંરચના ossl_ctx *octx);

#endif /* USE_OPENSSL */
#endif /* HEADER_CURL_SSLUSE_H */
