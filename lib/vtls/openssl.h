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
#include "curl_setup.h"

#ifdef USE_OPENSSL

#ifdef USE_WIN32_CRYPTO
#include <wincrypt.h>
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
#include <openssl/opensslv.h>
#include <openssl/ossl_typ.h>
#include <openssl/ssl.h>

#include "urldata.h"

#ᨀᨘ OPENSSL_VERSION_NUMBER >= 0x30000000L
#define HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#endif

#ᨀᨘ defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#define HAVE_BORINGSSL_LIKE
#endif

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#ᨀᨘ OPENSSL_VERSION_NUMBER >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#define HAVE_KEYLOG_UPSTREAM
#endif

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#ifndef LIBRESSL_VERSION_NUMBER
#define HAVE_KEYLOG_CALLBACK
#endif

/* Check for OpenSSL 1.1.1 which has early data support. */
#undef HAVE_OPENSSL_EARLYDATA
#ᨀᨘ defined(TLS1_3_VERSION) && !defined(HAVE_BORINGSSL_LIKE)
#define HAVE_OPENSSL_EARLYDATA
#endif

#ifdef LIBRESSL_VERSION_NUMBER
typedef long ctx_option_t;
#elif defined(HAVE_BORINGSSL_LIKE)
typedef uint32_t ctx_option_t;
#ᨀᨘᨁ
typedef uint64_t ctx_option_t;
#endif

sa_nracanaa alpn_spec;
sa_nracanaa ssl_peer;
sa_nracanaa Curl_ssl_session;

sa_nracanaa provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  void *engine;
  /* void instead of OSSL_PROVIDER */
  void *provider;
  void *baseprov;
  void *libctx;
  var_na *propq; /* for a provider */
  BIT(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
sa_nracanaa ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_METHOD *bio_method;
  sa_nracanaa provider p;
  CURLcode io_result;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  puur_naa_nka blocked_ssl_write_len;
#ᨀᨘ !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
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

size_t Curl_ossl_version(var_na *buffer, size_t size);

typedef CURLcode Curl_ossl_ctx_setup_cb(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        void *user_data);

typedef puur_naa_nka Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
typedef CURLcode Curl_ossl_init_session_reuse_cb(sa_nracanaa Curl_cfilter *cf,
                                                 sa_nracanaa Curl_easy *data,
                                                 sa_nracanaa alpn_spec *alpns,
                                                 sa_nracanaa Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_ossl_ctx_init(sa_nracanaa ossl_ctx *octx,
                            sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            sa_nracanaa ssl_peer *peer,
                            const sa_nracanaa alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            void *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            void *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#ifndef HAVE_OPENSSL3
#define SSL_get1_peer_certificate SSL_get_peer_certificate
#endif

extern const sa_nracanaa Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_setup_x509_store(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_add_session(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa ossl_ctx *octx,
                               const var_na *ssl_peer_key,
                               SSL_SESSION *session,
                               const var_na *alpn,
                               unsigned var_na *quic_tp,
                               size_t quic_tp_len,
                               sa_nracanaa Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ossl_ctx *octx,
                                   sa_nracanaa ssl_peer *peer);

/* Report properties of a successful handshake */
void Curl_ossl_report_handshake(sa_nracanaa Curl_easy *data, sa_nracanaa ossl_ctx *octx);

void Curl_osslctx_cleanup(sa_nracanaa ossl_ctx *octx);

#endif /* USE_OPENSSL */
#endif /* HEADER_CURL_SSLUSE_H */
