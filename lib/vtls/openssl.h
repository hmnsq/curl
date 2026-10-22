#अगर_अघोषित HEADER_CURL_SSLUSE_H
#घोषणा HEADER_CURL_SSLUSE_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_OPENSSL

#अगर_घोषित USE_WIN32_CRYPTO
#समावेश <wincrypt.h>
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
#अघोषित X509_NAME
#अघोषित X509_EXTENSIONS
#अघोषित PKCS7_ISSUER_AND_SERIAL
#अघोषित PKCS7_SIGNER_INFO
#अघोषित OCSP_REQUEST
#अघोषित OCSP_RESPONSE
#पूर्ण_अगर /* USE_WIN32_CRYPTO */

/*
 * This header should only be needed to get included by vtls.c, openssl.c
 * and ngtcp2.c
 */
#समावेश <openssl/opensslv.h>
#समावेश <openssl/ossl_typ.h>
#समावेश <openssl/ssl.h>

#समावेश "urldata.h"

#अगर OPENSSL_VERSION_संख्या >= 0x30000000L
#घोषणा HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#पूर्ण_अगर

#अगर defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#घोषणा HAVE_BORINGSSL_LIKE
#पूर्ण_अगर

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#अगर OPENSSL_VERSION_संख्या >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#घोषणा HAVE_KEYLOG_UPSTREAM
#पूर्ण_अगर

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#अगर_अघोषित LIBRESSL_VERSION_संख्या
#घोषणा HAVE_KEYLOG_CALLBACK
#पूर्ण_अगर

/* Check for OpenSSL 1.1.1 which has early data support. */
#अघोषित HAVE_OPENSSL_EARLYDATA
#अगर defined(TLS1_3_VERSION) && !defined(HAVE_BORINGSSL_LIKE)
#घोषणा HAVE_OPENSSL_EARLYDATA
#पूर्ण_अगर

#अगर_घोषित LIBRESSL_VERSION_संख्या
प्रकार दीर्घ ctx_option_t;
#या_अगर defined(HAVE_BORINGSSL_LIKE)
प्रकार uint32_t ctx_option_t;
#अन्यथा
प्रकार uint64_t ctx_option_t;
#पूर्ण_अगर

काष्ठा alpn_spec;
काष्ठा ssl_peer;
काष्ठा Curl_ssl_session;

काष्ठा provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  व्योम *engine;
  /* void instead of OSSL_PROVIDER */
  व्योम *provider;
  व्योम *baseprov;
  व्योम *libctx;
  अक्षर *propq; /* for a provider */
  BIT(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
काष्ठा ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_METHOD *bio_method;
  काष्ठा provider p;
  CURLcode io_परिणाम;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  पूर्णांक blocked_ssl_write_len;
#अगर !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
  /* Set to true once a valid keylog entry has been created to avoid dupes.
     This is a bool and not a bitfield because it is passed by address. */
  bool keylog_done;
#पूर्ण_अगर
  BIT(x509_store_setup);            /* x509 store has been set up */
  BIT(store_is_empty);              /* no certs/paths/blobs in x509 store */
  BIT(reused_session);              /* session-ID was reused for this */
#अगर_घोषित USE_APPLE_SECTRUST
  BIT(sectrust_verified);           /* peer was verified by sectrust */
  BIT(sectrust_session);            /* session from sectrust verified peer */
#पूर्ण_अगर
};

माप_प्रकार Curl_ossl_version(अक्षर *बफर, माप_प्रकार आकार);

प्रकार CURLcode Curl_ossl_ctx_setup_cb(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        व्योम *user_तथ्य);

प्रकार पूर्णांक Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
प्रकार CURLcode Curl_ossl_init_session_reuse_cb(काष्ठा Curl_cfilter *cf,
                                                 काष्ठा Curl_easy *तथ्य,
                                                 काष्ठा alpn_spec *alpns,
                                                 काष्ठा Curl_ssl_session *scs,
                                                 bool *do_early_तथ्य);

CURLcode Curl_ossl_ctx_init(काष्ठा ossl_ctx *octx,
                            काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा ssl_peer *peer,
                            स्थिर काष्ठा alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            व्योम *cb_user_तथ्य,
                            Curl_ossl_new_session_cb *cb_new_session,
                            व्योम *ssl_user_तथ्य,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#अगर_अघोषित HAVE_OPENSSL3
#घोषणा SSL_get1_peer_certificate SSL_get_peer_certificate
#पूर्ण_अगर

बाह्य स्थिर काष्ठा Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_setup_x509_store(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_add_session(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा ossl_ctx *octx,
                               स्थिर अक्षर *ssl_peer_कुंजी,
                               SSL_SESSION *session,
                               स्थिर अक्षर *alpn,
                               अचिन्हित अक्षर *quic_tp,
                               माप_प्रकार quic_tp_len,
                               काष्ठा Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ossl_ctx *octx,
                                   काष्ठा ssl_peer *peer);

/* Report properties of a successful handshake */
व्योम Curl_ossl_report_handshake(काष्ठा Curl_easy *तथ्य, काष्ठा ossl_ctx *octx);

व्योम Curl_osslctx_cleanup(काष्ठा ossl_ctx *octx);

#पूर्ण_अगर /* USE_OPENSSL */
#पूर्ण_अगर /* HEADER_CURL_SSLUSE_H */
