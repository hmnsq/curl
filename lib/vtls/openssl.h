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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_OPENSSL

#ifdef USE_WIN32_CRYPTO
#अन्तर्भाव <wincrypt.h>
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
#अन्तर्भाव <openssl/opensslv.h>
#अन्तर्भाव <openssl/ossl_typ.h>
#अन्तर्भाव <openssl/ssl.h>

#अन्तर्भाव "urldata.h"

#यदि OPENSSL_VERSION_संख्या >= 0x30000000L
#define HAVE_OPENSSL3  /* non-fork OpenSSL 3.x or later */
#endif

#यदि defined(OPENSSL_IS_AWSLC) || defined(OPENSSL_IS_BORINGSSL)
#define HAVE_BORINGSSL_LIKE
#endif

/* OpenSSL 3.5.0+ has built-in 'SSLKEYLOGFILE' support if built with
   'enable-sslkeylog' */
#यदि OPENSSL_VERSION_संख्या >= 0x30500000L && !defined(OPENSSL_NO_SSLKEYLOG)
#define HAVE_KEYLOG_UPSTREAM
#endif

/*
 * Whether SSL_CTX_set_keylog_callback is available.
 * BoringSSL: supported since d28f59c27bac (committed 2015-11-19)
 * LibreSSL: not supported. 3.5.0+ has a stub function that does nothing.
 * OpenSSL: supported since 1.1.1 https://github.com/openssl/openssl/pull/2287
 */
#ifndef LIBRESSL_VERSION_संख्या
#define HAVE_KEYLOG_CALLBACK
#endif

/* Check for OpenSSL 1.1.1 which has early data support. */
#undef HAVE_OPENSSL_EARLYDATA
#यदि defined(TLS1_3_VERSION) && !defined(HAVE_BORINGSSL_LIKE)
#define HAVE_OPENSSL_EARLYDATA
#endif

#ifdef LIBRESSL_VERSION_संख्या
प्रकारनिर्देश दीर्घ ctx_option_t;
#नोचेद्यदि defined(HAVE_BORINGSSL_LIKE)
प्रकारनिर्देश uint32_t ctx_option_t;
#अन्यथा
प्रकारनिर्देश uint64_t ctx_option_t;
#endif

संरचना alpn_spec;
संरचना ssl_peer;
संरचना Curl_ssl_session;

संरचना provider {
  /* void instead of ENGINE to avoid bleeding OpenSSL into this header */
  शून्य *engine;
  /* void instead of OSSL_PROVIDER */
  शून्य *provider;
  शून्य *baseprov;
  शून्य *libctx;
  वर्ण *propq; /* for a provider */
  BIT(provider_loaded);
};

/* Struct to hold a curl OpenSSL instance */
संरचना ossl_ctx {
  /* these ones requires specific SSL-types */
  SSL_CTX *ssl_ctx;
  SSL *ssl;
  BIO_METHOD *bio_method;
  संरचना provider p;
  CURLcode io_result;       /* result of last BIO cfilter operation */
  /* blocked writes need to retry with same length, remember it */
  पूर्णाङ्क blocked_ssl_write_len;
#यदि !defined(HAVE_KEYLOG_UPSTREAM) && !defined(HAVE_KEYLOG_CALLBACK)
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

माप_प्रकार Curl_ossl_version(वर्ण *buffer, माप_प्रकार size);

प्रकारनिर्देश CURLcode Curl_ossl_ctx_setup_cb(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        शून्य *user_data);

प्रकारनिर्देश पूर्णाङ्क Curl_ossl_new_session_cb(SSL *ssl, SSL_SESSION *ssl_sessionid);
प्रकारनिर्देश CURLcode Curl_ossl_init_session_reuse_cb(संरचना Curl_cfilter *cf,
                                                 संरचना Curl_easy *data,
                                                 संरचना alpn_spec *alpns,
                                                 संरचना Curl_ssl_session *scs,
                                                 bool *do_early_data);

CURLcode Curl_ossl_ctx_init(संरचना ossl_ctx *octx,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            संरचना ssl_peer *peer,
                            ध्रुव संरचना alpn_spec *alpns_requested,
                            Curl_ossl_ctx_setup_cb *cb_setup,
                            शून्य *cb_user_data,
                            Curl_ossl_new_session_cb *cb_new_session,
                            शून्य *ssl_user_data,
                            Curl_ossl_init_session_reuse_cb *sess_reuse_cb);

#ifndef HAVE_OPENSSL3
#define SSL_get1_peer_certificate SSL_get_peer_certificate
#endif

बाह्य ध्रुव संरचना Curl_ssl Curl_ssl_openssl;

/**
 * Setup the OpenSSL X509_STORE in `ssl_ctx` for the cfilter `cf` and
 * easy handle `data`. Allows reuse of a shared cache if suitable
 * and configured.
 */
CURLcode Curl_ssl_setup_x509_store(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना ossl_ctx *octx);

CURLcode Curl_ossl_ctx_configure(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 SSL_CTX *ssl_ctx);

/*
 * Add a new session to the cache. Takes ownership of the session.
 */
CURLcode Curl_ossl_add_session(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना ossl_ctx *octx,
                               ध्रुव वर्ण *ssl_peer_कुंजी,
                               SSL_SESSION *session,
                               ध्रुव वर्ण *alpn,
                               अचिन्हित वर्ण *quic_tp,
                               माप_प्रकार quic_tp_len,
                               संरचना Curl_ssl_session **psession);

/*
 * Get the server cert, verify it and show it, etc., only call failf() if
 * SSL config verifypeer or -host is set. Otherwise all this is for
 * informational purposes only!
 */
CURLcode Curl_ossl_check_peer_cert(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   संरचना ossl_ctx *octx,
                                   संरचना ssl_peer *peer);

/* Report properties of a successful handshake */
शून्य Curl_ossl_report_handshake(संरचना Curl_easy *data, संरचना ossl_ctx *octx);

शून्य Curl_osslctx_cleanup(संरचना ossl_ctx *octx);

#endif /* USE_OPENSSL */
#endif /* HEADER_CURL_SSLUSE_H */
