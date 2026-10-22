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
/*
 * Source file for all GnuTLS-specific code for the TLS/SSL layer. No code
 * but vtls.c should ever call or use these functions.
 *
 * Note: do not use the GnuTLS' *_t variable type names in this source code,
 * since they were not present in 1.0.x.
 */
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_GNUTLS

#അന്തര്ഭാവ <gnutls/abstract.h>
#അന്തര്ഭാവ <gnutls/gnutls.h>
#അന്തര്ഭാവ <gnutls/x509.h>
#അന്തര്ഭാവ <gnutls/crypto.h>
#അന്തര്ഭാവ <nettle/sha2.h>
#അന്തര്ഭാവ <nettle/version.h>

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "vtls/keylog.h"
#അന്തര്ഭാവ "vtls/gtls.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/vtls_int.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "vtls/apple.h"
#അന്തര്ഭാവ "vauth/vauth.h"
#അന്തര്ഭാവ "parsedate.h"
#അന്തര്ഭാവ "connect.h" /* for the connect timeout */
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#എങ്കിൽ 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
സ്ഥിതം ശൂന്യം tls_log_func(പൂർണ്ണസംഖ്യ level, സ്ഥിരം അക്ഷരം *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#എങ്കിൽ !defined(GNUTLS_VERSION_സംഖ്യ) || (GNUTLS_VERSION_സംഖ്യ < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#എങ്കിൽ GNUTLS_VERSION_സംഖ്യ >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#അന്തര്ഭാവ <gnutls/ocsp.h>

ഘടന gtls_ssl_backend_data {
  ഘടന gtls_ctx gtls;
};

സ്ഥിതം ssize_t gtls_തള്ളുക(ശൂന്യം *s, സ്ഥിരം ശൂന്യം *buf, size_t blen)
{
  ഘടന Curl_cfilter *cf = s;
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (പൂർണ്ണസംഖ്യ)result, nwritten);
  backend->gtls.io_result = result;
  എങ്കിൽ(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    തിരികെ -1;
  }
  തിരികെ (ssize_t)nwritten;
}

സ്ഥിതം ssize_t gtls_pull(ശൂന്യം *s, ശൂന്യം *buf, size_t blen)
{
  ഘടന Curl_cfilter *cf = s;
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  എങ്കിൽ(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    എങ്കിൽ(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      തിരികെ -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (പൂർണ്ണസംഖ്യ)result,
              nread);
  backend->gtls.io_result = result;
  എങ്കിൽ(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    തിരികെ -1;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(nread == 0)
    connssl->peer_closed = TRUE;
  തിരികെ (ssize_t)nread;
}

/**
 * gtls_init()
 *
 * Global GnuTLS init, called from Curl_ssl_init(). This calls functions that
 * are not thread-safe (It is thread-safe since GnuTLS 3.3.0) and thus this
 * function itself is not thread-safe and must only be called from within
 * curl_global_init() to keep the thread situation under control!
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ gtls_init(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  തിരികെ ret;
}

സ്ഥിതം ശൂന്യം gtls_cleanup(ശൂന്യം)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
സ്ഥിതം ശൂന്യം showtime(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *text, time_t stamp)
{
  ഘടന tm buffer;
  സ്ഥിരം ഘടന tm *tm = &buffer;
  അക്ഷരം str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  എങ്കിൽ(result)
    തിരികെ;

  curl_msnprintf(str,
                 വലുപ്പം(str),
                 "  %s: %s, %02d %s %4d %02d:%02d:%02d GMT",
                 text,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);
  infof(data, "%s", str);
}
#endif

സ്ഥിതം gnutls_datum_t load_file(സ്ഥിരം അക്ഷരം *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { ശൂന്യ, 0 };
  long filelen;
  ശൂന്യം *ptr;

  f = curlx_fopen(file, "rb");
  എങ്കിൽ(!f)
    തിരികെ loaded_file;
  എങ്കിൽ(fseek(f, 0, SEEK_END))
    പോകുക out;
  filelen = ftell(f);
  എങ്കിൽ(filelen < 0 || filelen > CURL_MAX_നിവേശം_LENGTH)
    പോകുക out;
  എങ്കിൽ(fseek(f, 0, SEEK_SET))
    പോകുക out;
  ptr = curlx_malloc((size_t)filelen);
  എങ്കിൽ(!ptr)
    പോകുക out;
  എങ്കിൽ(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    പോകുക out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned പൂർണ്ണസംഖ്യ)filelen;
out:
  curlx_fclose(f);
  തിരികെ loaded_file;
}

സ്ഥിതം ശൂന്യം unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
സ്ഥിതം CURLcode cf_gtls_handshake(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  പൂർണ്ണസംഖ്യ rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  എങ്കിൽ(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  എങ്കിൽ((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    തിരികെ CURLE_AGAIN;
  }
  അല്ലെങ്കിൽ എങ്കിൽ((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    സ്ഥിരം അക്ഷരം *strerr = ശൂന്യ;

    എങ്കിൽ(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    എങ്കിൽ(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    തിരികെ CURLE_AGAIN;
  }
  അല്ലെങ്കിൽ എങ്കിൽ((rc < 0) && backend->gtls.io_result) {
    തിരികെ backend->gtls.io_result;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rc < 0) {
    സ്ഥിരം അക്ഷരം *strerr = ശൂന്യ;

    എങ്കിൽ(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    എങ്കിൽ(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം gnutls_x509_crt_fmt_t gnutls_do_file_type(സ്ഥിരം അക്ഷരം *type)
{
  എങ്കിൽ(!type || !type[0])
    തിരികെ GNUTLS_X509_FMT_PEM;
  എങ്കിൽ(curl_strequal(type, "PEM"))
    തിരികെ GNUTLS_X509_FMT_PEM;
  എങ്കിൽ(curl_strequal(type, "DER"))
    തിരികെ GNUTLS_X509_FMT_DER;
  തിരികെ GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

സ്ഥിതം CURLcode gnutls_set_ssl_version_min_max(
  ഘടന Curl_easy *data,
  ഘടന ssl_peer *peer,
  ഘടന ssl_filter_config *conn_config,
  സ്ഥിരം അക്ഷരം **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  എങ്കിൽ(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  എങ്കിൽ((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  എങ്കിൽ(peer->transport == TRNSPRT_QUIC) {
    എങ്കിൽ(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    തിരികെ CURLE_OK;
  }

  തിരഞ്ഞെടുപ്പ്(ssl_version | ssl_version_max) {
  അവസ്ഥ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    തിരികെ CURLE_OK;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    തിരികെ CURLE_OK;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    തിരികെ CURLE_OK;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    തിരികെ CURLE_OK;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    തിരികെ CURLE_OK;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    തിരികെ CURLE_OK;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    തിരികെ CURLE_OK;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    തിരികെ CURLE_OK;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    തിരികെ CURLE_OK;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    തിരികെ CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  തിരികെ CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(ഘടന Curl_easy *data,
                                       ഘടന gtls_shared_creds **pcreds)
{
  ഘടന gtls_shared_creds *shared;
  പൂർണ്ണസംഖ്യ rc;

  *pcreds = ശൂന്യ;
  shared = curlx_calloc(1, വലുപ്പം(*shared));
  എങ്കിൽ(!shared)
    തിരികെ CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  എങ്കിൽ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  തിരികെ CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(ഘടന gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  എങ്കിൽ(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    തിരികെ CURLE_OK;
  }
  തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
}

ശൂന്യം Curl_gtls_shared_creds_free(ഘടന gtls_shared_creds **pcreds)
{
  ഘടന gtls_shared_creds *shared = *pcreds;
  *pcreds = ശൂന്യ;
  എങ്കിൽ(shared) {
    --shared->refcount;
    എങ്കിൽ(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

സ്ഥിതം CURLcode gtls_populate_creds(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  ഘടന ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    തിരികെ CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  എങ്കിൽ(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#അല്ലെങ്കിൽ
    rc = gnutls_certificate_set_x509_system_trust(creds);
    എങ്കിൽ(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    അല്ലെങ്കിൽ {
      infof(data, "  Native: %d certificates from system trust", rc);
      എങ്കിൽ(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  എങ്കിൽ(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    എങ്കിൽ(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      തിരികെ CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned പൂർണ്ണസംഖ്യ)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    എങ്കിൽ(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      എങ്കിൽ(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        തിരികെ CURLE_SSL_CACERT_BADFILE;
      }
    }
    അല്ലെങ്കിൽ
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  അല്ലെങ്കിൽ എങ്കിൽ(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    എങ്കിൽ(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      എങ്കിൽ(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        തിരികെ CURLE_SSL_CACERT_BADFILE;
      }
    }
    അല്ലെങ്കിൽ
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  എങ്കിൽ(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    എങ്കിൽ(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      എങ്കിൽ(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        തിരികെ CURLE_SSL_CACERT_BADFILE;
      }
    }
    അല്ലെങ്കിൽ
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  എങ്കിൽ(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  എങ്കിൽ(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    തിരികെ CURLE_NOT_BUILT_IN;
  }
#endif
  എങ്കിൽ(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    എങ്കിൽ(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      തിരികെ CURLE_SSL_CRL_BADFILE;
    }
    അല്ലെങ്കിൽ
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  തിരികെ CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_താക്കോൽ   "tls:gtls:x509:share"

സ്ഥിതം bool gtls_shared_creds_expired(ഘടന Curl_easy *data,
                                      സ്ഥിരം ഘടന gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  എങ്കിൽ(timeout_ms < 0)
    തിരികെ FALSE;

  തിരികെ elapsed_ms >= timeout_ms;
}

സ്ഥിതം bool gtls_shared_creds_different(ഘടന Curl_cfilter *cf,
                                        സ്ഥിരം ഘടന gtls_shared_creds *sc)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  എങ്കിൽ(!sc->CAfile || !conn_config->CAfile)
    തിരികെ sc->CAfile != conn_config->CAfile;

  തിരികെ strcmp(sc->CAfile, conn_config->CAfile);
}

സ്ഥിതം ഘടന gtls_shared_creds *gtls_get_cached_creds(ഘടന Curl_cfilter *cf,
                                                       ഘടന Curl_easy *data)
{
  ഘടന gtls_shared_creds *shared_creds;

  എങ്കിൽ(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_താക്കോൽ,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_താക്കോൽ));
    എങ്കിൽ(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      തിരികെ shared_creds;
    }
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം ശൂന്യം gtls_shared_creds_hash_free(സ്ഥിരം ശൂന്യം *താക്കോൽ,
                                        size_t താക്കോൽ_len, ശൂന്യം *p)
{
  ഘടന gtls_shared_creds *sc = p;
  DEBUGASSERT(താക്കോൽ_len == CURL_CSTRLEN(MPROTO_GTLS_X509_താക്കോൽ));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_താക്കോൽ, താക്കോൽ, താക്കോൽ_len));
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)താക്കോൽ_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

സ്ഥിതം ശൂന്യം gtls_set_cached_creds(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  ഘടന gtls_shared_creds *sc)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  എങ്കിൽ(!data->multi)
    തിരികെ;

  എങ്കിൽ(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    എങ്കിൽ(!sc->CAfile)
      തിരികെ;
  }

  എങ്കിൽ(Curl_gtls_shared_creds_up_ref(sc))
    തിരികെ;

  എങ്കിൽ(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_താക്കോൽ,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_താക്കോൽ),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    തിരികെ;
  }
}

CURLcode Curl_gtls_client_trust_setup(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      ഘടന gtls_ctx *gtls)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന gtls_shared_creds *cached_creds = ശൂന്യ;
  bool cache_criteria_met;
  CURLcode result;
  പൂർണ്ണസംഖ്യ rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (data->set.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  എങ്കിൽ(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  എങ്കിൽ(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    എങ്കിൽ(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    എങ്കിൽ(result)
      തിരികെ result;
    gtls->shared_creds->trust_setup = TRUE;
    എങ്കിൽ(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  തിരികെ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
സ്ഥിതം പൂർണ്ണസംഖ്യ gtls_get_ietf_proto(gnutls_session_t session)
{
  തിരഞ്ഞെടുപ്പ്(gnutls_protocol_get_version(session)) {
  അവസ്ഥ GNUTLS_SSL3:
    തിരികെ CURL_IETF_PROTO_SSL3;
  അവസ്ഥ GNUTLS_TLS1_0:
    തിരികെ CURL_IETF_PROTO_TLS1;
  അവസ്ഥ GNUTLS_TLS1_1:
    തിരികെ CURL_IETF_PROTO_TLS1_1;
  അവസ്ഥ GNUTLS_TLS1_2:
    തിരികെ CURL_IETF_PROTO_TLS1_2;
  അവസ്ഥ GNUTLS_TLS1_3:
    തിരികെ CURL_IETF_PROTO_TLS1_3;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 സ്ഥിരം അക്ഷരം *alpn,
                                 unsigned അക്ഷരം *quic_tp,
                                 size_t quic_tp_len,
                                 ഘടന Curl_ssl_session **psession)
{
  ഘടന Curl_ssl_session *sc_session = ശൂന്യ, *sc_dup = ശൂന്യ;
  unsigned അക്ഷരം *sdata, *qtp_clone = ശൂന്യ;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(psession)
    *psession = ശൂന്യ;
  എങ്കിൽ(!Curl_ssl_scache_use(cf, data))
    തിരികെ CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, ശൂന്യ, &sdata_len);
  എങ്കിൽ(!sdata_len) /* GnuTLS does this for some version combinations */
    തിരികെ CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  എങ്കിൽ(!sdata)
    തിരികെ CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  എങ്കിൽ(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((സ്ഥിരം അക്ഷരം *)quic_tp, quic_tp_len);
    എങ്കിൽ(!qtp_clone) {
      curlx_free(sdata);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  എങ്കിൽ(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  എങ്കിൽ(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_താക്കോൽ, sc_session);
    /* took ownership of `sc_session` */
    sc_session = ശൂന്യ;
  }
  എങ്കിൽ(!result && psession) {
    *psession = sc_dup;
    sc_dup = ശൂന്യ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  തിരികെ result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
സ്ഥിതം CURLcode cf_gtls_update_session_id(ഘടന Curl_cfilter *cf,
                                          ഘടന Curl_easy *data,
                                          gnutls_session_t session)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന Curl_ssl_session *scs = ശൂന്യ;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_താക്കോൽ,
                                   session, 0, connssl->negotiated.alpn,
                                   ശൂന്യ, 0, &scs);
  എങ്കിൽ(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ gtls_handshake_cb(gnutls_session_t session, unsigned പൂർണ്ണസംഖ്യ htype,
                             unsigned when, unsigned പൂർണ്ണസംഖ്യ incoming,
                             സ്ഥിരം gnutls_datum_t *സന്ദേശം)
{
  ഘടന Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (ശൂന്യം)സന്ദേശം;
  (ശൂന്യം)incoming;
  എങ്കിൽ(when) { /* after message has been processed */
    ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
    എങ്കിൽ(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      തിരഞ്ഞെടുപ്പ്(htype) {
      അവസ്ഥ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        നിർത്തുക;
      }
      സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
      }
    }
  }
  തിരികെ 0;
}
#endif

സ്ഥിതം CURLcode gtls_set_priority(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  ഘടന gtls_ctx *gtls,
                                  സ്ഥിരം അക്ഷരം *priority)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന dynbuf buf;
  സ്ഥിരം അക്ഷരം *err = ശൂന്യ;
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ rc;

  curlx_dyn_init(&buf, 4096);

  എങ്കിൽ(conn_config->cipher_list) {
    എങ്കിൽ((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      എങ്കിൽ(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        എങ്കിൽ(result)
          പോകുക out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      എങ്കിൽ(result)
        പോകുക out;
      priority = curlx_dyn_ptr(&buf);
    }
    അല്ലെങ്കിൽ /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  എങ്കിൽ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  തിരികെ result;
}

സ്ഥിതം CURLcode gtls_client_init(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന ssl_peer *peer,
                                 size_t earlydata_max,
                                 ഘടന gtls_ctx *gtls)
{
  ഘടന ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned പൂർണ്ണസംഖ്യ init_flags;
  പൂർണ്ണസംഖ്യ rc;
  സ്ഥിരം അക്ഷരം *prioritylist;
  bool tls13support;
  CURLcode result;

  എങ്കിൽ(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  എങ്കിൽ(result)
    തിരികെ result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  എങ്കിൽ(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  അല്ലെങ്കിൽ എങ്കിൽ(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_DATA;
#endif

#ifdef GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#endif

#ifdef GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#endif

#ifdef GNUTLS_NO_STATUS_REQUEST
  എങ്കിൽ(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  എങ്കിൽ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  എങ്കിൽ(peer->sni) {
    എങ്കിൽ(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  എങ്കിൽ(rc != GNUTLS_E_SUCCESS)
    തിരികെ CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  എങ്കിൽ(config->version == CURL_SSLVERSION_TLSv1_3) {
    എങ്കിൽ(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  എങ്കിൽ(result)
    തിരികെ result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(config->clientcert) {
    എങ്കിൽ(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    എങ്കിൽ(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->താക്കോൽ_passwd ? config->താക്കോൽ_passwd : "");
      എങ്കിൽ(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        തിരികെ CURLE_SSL_CONNECT_ERROR;
      }
    }
    അല്ലെങ്കിൽ {
      സ്ഥിരം unsigned പൂർണ്ണസംഖ്യ supported_താക്കോൽ_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_താക്കോൽ_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->താക്കോൽ ? config->താക്കോൽ : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->താക്കോൽ_passwd,
           supported_താക്കോൽ_encryption_algorithms);
      എങ്കിൽ(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->താക്കോൽ_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        തിരികെ CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  എങ്കിൽ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  എങ്കിൽ(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  ശൂന്യ, 0, ശൂന്യ);
    എങ്കിൽ(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
  }

  തിരികെ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
സ്ഥിതം പൂർണ്ണസംഖ്യ keylog_callback(gnutls_session_t session, സ്ഥിരം അക്ഷരം *label,
                           സ്ഥിരം gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  എങ്കിൽ(crandom.size != 32) {
    തിരികെ -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  തിരികെ 0;
}

സ്ഥിതം CURLcode gtls_on_session_reuse(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      ഘടന alpn_spec *alpns,
                                      ഘടന Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  തിരികെ Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

സ്ഥിതം CURLcode gtls_apply_session(
  ഘടന gtls_ctx *gctx,
  ഘടന Curl_cfilter *cf,
  ഘടന Curl_easy *data,
  ഘടന ssl_peer *peer,
  ഘടന alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  ഘടന Curl_ssl_session *scs,
  bool *pwas_setup)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ rc;

  എങ്കിൽ(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    എങ്കിൽ(result)
      പോകുക out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    എങ്കിൽ(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    അല്ലെങ്കിൽ {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      എങ്കിൽ(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        എങ്കിൽ(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          എങ്കിൽ(result)
            പോകുക out;
        }
        എങ്കിൽ(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  തിരികെ result;
}

CURLcode Curl_gtls_ctx_init(ഘടന gtls_ctx *gctx,
                            ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            ഘടന ssl_peer *peer,
                            സ്ഥിരം ഘടന alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            ശൂന്യം *cb_user_data,
                            ശൂന്യം *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന Curl_ssl_session *scs = ശൂന്യ;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_എണ്ണം = 0;
  bool gtls_session_setup = FALSE;
  ഘടന alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  എങ്കിൽ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    എങ്കിൽ(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = ശൂന്യ;
      എങ്കിൽ(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  എങ്കിൽ(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_താക്കോൽ, &scs);
    എങ്കിൽ(result)
      പോകുക out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  എങ്കിൽ(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    എങ്കിൽ(result)
      പോകുക out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  എങ്കിൽ(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    എങ്കിൽ(result)
      പോകുക out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  എങ്കിൽ(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  എങ്കിൽ(!gtls_alpns_എണ്ണം && alpns.എണ്ണം) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.എണ്ണം);
    വേണ്ടി(i = 0; i < alpns.എണ്ണം; ++i) {
      gtls_alpns[i].data = (unsigned അക്ഷരം *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned പൂർണ്ണസംഖ്യ)strlen(alpns.entries[i]);
    }
    gtls_alpns_എണ്ണം = alpns.എണ്ണം;
  }

  എങ്കിൽ(gtls_alpns_എണ്ണം &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned പൂർണ്ണസംഖ്യ)gtls_alpns_എണ്ണം,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_താക്കോൽ, scs);
  തിരികെ result;
}

സ്ഥിതം CURLcode gtls_connect_step1(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  എങ്കിൽ(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    തിരികെ CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, ശൂന്യ, ശൂന്യ, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#അല്ലെങ്കിൽ
                              ശൂന്യ
#endif
                              );

  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ഘടന alpn_proto_buf proto;
    memset(&proto, 0, വലുപ്പം(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  gnutls_handshake_set_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#endif

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_set_ptr(backend->gtls.session, cf);
  gnutls_transport_set_തള്ളുക_function(backend->gtls.session, gtls_തള്ളുക);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode pkp_pin_peer_pubkey(ഘടന Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    സ്ഥിരം അക്ഷരം *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned അക്ഷരം *buff1 = ശൂന്യ;

  gnutls_pubkey_t താക്കോൽ = ശൂന്യ;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  എങ്കിൽ(!pinnedpubkey)
    തിരികെ CURLE_OK;

  എങ്കിൽ(!cert)
    തിരികെ result;

  ചെയ്യുക {
    പൂർണ്ണസംഖ്യ ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&താക്കോൽ);
    എങ്കിൽ(ret < 0)
      നിർത്തുക; /* failed */

    ret = gnutls_pubkey_import_x509(താക്കോൽ, cert, 0);
    എങ്കിൽ(ret < 0)
      നിർത്തുക; /* failed */

    ret = gnutls_pubkey_export(താക്കോൽ, GNUTLS_X509_FMT_DER, ശൂന്യ, &len1);
    എങ്കിൽ(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      നിർത്തുക; /* failed */

    buff1 = curlx_malloc(len1);
    എങ്കിൽ(!buff1)
      നിർത്തുക; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(താക്കോൽ, GNUTLS_X509_FMT_DER, buff1, &len2);
    എങ്കിൽ(ret < 0 || len1 != len2)
      നിർത്തുക; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } ആകുമ്പോൾ(0);

  എങ്കിൽ(താക്കോൽ)
    gnutls_pubkey_deinit(താക്കോൽ);

  curlx_safefree(buff1);

  തിരികെ result;
}

ശൂന്യം Curl_gtls_report_handshake(ഘടന Curl_easy *data, ഘടന gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    സ്ഥിരം അക്ഷരം *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)gctx;
#endif
}

സ്ഥിതം ശൂന്യം gtls_സന്ദേശം_verify_result(ഘടന Curl_easy *data,
                                   ഘടന ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  അക്ഷരം certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = വലുപ്പം(certname);
  പൂർണ്ണസംഖ്യ rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  എങ്കിൽ(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  എങ്കിൽ(!was_verified) {
    എങ്കിൽ(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    അല്ലെങ്കിൽ
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  അല്ലെങ്കിൽ
    infof(data, "  common name: %s (matched)", certname);
}

സ്ഥിതം ശൂന്യം gtls_infof_cert(ഘടന Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  എങ്കിൽ(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    പൂർണ്ണസംഖ്യ rc, algo;
    time_t tstamp;
    unsigned പൂർണ്ണസംഖ്യ bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    എങ്കിൽ(rc)
      infof(data, "Failed to get certificate name");
    അല്ലെങ്കിൽ {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    എങ്കിൽ(rc)
      infof(data, "Failed to get certificate issuer");
    അല്ലെങ്കിൽ {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)data;
  (ശൂന്യം)x509_cert;
#endif
}

സ്ഥിതം CURLcode gtls_verify_ocsp_status(ഘടന Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = ശൂന്യ;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  പൂർണ്ണസംഖ്യ rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  എങ്കിൽ(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(rc < 0) {
    failf(data, "Invalid OCSP response received");
    പോകുക out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  എങ്കിൽ(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    പോകുക out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  എങ്കിൽ(rc < 0) {
    failf(data, "failed to parse OCSP response");
    പോകുക out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  എങ്കിൽ(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    പോകുക out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  വേണ്ടി(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    എങ്കിൽ(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      നിർത്തുക;
  }
  എങ്കിൽ(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    പോകുക out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ,
                                   &status, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ);
  എങ്കിൽ(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    പോകുക out;
  }
  തിരഞ്ഞെടുപ്പ്(status) {
  അവസ്ഥ GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    നിർത്തുക;
  അവസ്ഥ GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    failf(data, "peer certificate OCSP status is not good: %d", (പൂർണ്ണസംഖ്യ)status);
    നിർത്തുക;
  }

out:
  എങ്കിൽ(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  തിരികെ result;
}

ഘടന gtls_cert_chain {
  സ്ഥിരം gnutls_datum_t *certs;
  unsigned പൂർണ്ണസംഖ്യ അക്കം_certs;
};

#ifdef USE_APPLE_SECTRUST
സ്ഥിതം CURLcode gtls_chain_get_der(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   ശൂന്യം *user_data,
                                   size_t i,
                                   unsigned അക്ഷരം **pder,
                                   size_t *pder_len)
{
  ഘടന gtls_cert_chain *chain = user_data;

  (ശൂന്യം)cf;
  (ശൂന്യം)data;
  *pder_len = 0;
  *pder = ശൂന്യ;

  എങ്കിൽ(i >= chain->അക്കം_certs)
    തിരികെ CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  തിരികെ CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
സ്ഥിതം CURLcode gtls_verify_cert(ഘടന Curl_easy *data,
                                 ഘടന ssl_filter_config *config,
                                 ഘടന ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 ഘടന Curl_cfilter *cf,
                                 ഘടന ssl_peer *peer,
                                 ഘടന gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned പൂർണ്ണസംഖ്യ verify_status = 0;
  long * സ്ഥിരം certverifyresult = &ssl_config->certverifyresult;
  പൂർണ്ണസംഖ്യ rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  എങ്കിൽ(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  എങ്കിൽ(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  എങ്കിൽ(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    എങ്കിൽ(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = ശൂന്യ;
      ocsp_raw.size = 0;
    }
    എങ്കിൽ(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      തിരികെ CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->അക്കം_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    എങ്കിൽ(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      തിരികെ result; /* no further failure analysis needed */
    verified = !result;
    എങ്കിൽ(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)cf;
  (ശൂന്യം)peer;
  (ശൂന്യം)chain;
#endif

  എങ്കിൽ(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    സ്ഥിരം അക്ഷരം *cause = "certificate error, no details available";
    എങ്കിൽ(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    അല്ലെങ്കിൽ എങ്കിൽ(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    അല്ലെങ്കിൽ എങ്കിൽ(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    അല്ലെങ്കിൽ എങ്കിൽ(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    തിരികെ result;
  }
  തിരികെ CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                gnutls_session_t session,
                                ഘടന ssl_filter_config *config,
                                ഘടന ssl_easy_config *ssl_config,
                                ഘടന ssl_peer *peer,
                                സ്ഥിരം അക്ഷരം *pinned_താക്കോൽ)
{
  ഘടന gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = ശൂന്യ, x509_issuer = ശൂന്യ;
  time_t certclock;
  bool ocsp_checked = FALSE;
  പൂർണ്ണസംഖ്യ rc;
  CURLcode result = CURLE_OK;
  long * സ്ഥിരം certverifyresult = &ssl_config->certverifyresult;

  (ശൂന്യം)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.അക്കം_certs);
  എങ്കിൽ(!chain.certs) {
    എങ്കിൽ(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      പോകുക out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  എങ്കിൽ(data->set.ssl.certinfo && chain.certs) {
    എങ്കിൽ(chain.അക്കം_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.അക്കം_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ {
      unsigned പൂർണ്ണസംഖ്യ i;

      result = Curl_ssl_init_certinfo(data, (പൂർണ്ണസംഖ്യ)chain.അക്കം_certs);
      എങ്കിൽ(result)
        പോകുക out;

      വേണ്ടി(i = 0; i < chain.അക്കം_certs; i++) {
        സ്ഥിരം uint8_t *beg = chain.certs[i].data;
        സ്ഥിരം uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (പൂർണ്ണസംഖ്യ)i, beg, end);
        എങ്കിൽ(result)
          പോകുക out;
      }
    }
  }

  എങ്കിൽ(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    എങ്കിൽ(result)
      പോകുക out;
  }
  അല്ലെങ്കിൽ
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  എങ്കിൽ(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    പോകുക out;
  }

  എങ്കിൽ(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    എങ്കിൽ(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  എങ്കിൽ(certclock == (time_t)-1) {
    എങ്കിൽ(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(certclock < time(ശൂന്യ)) {
      എങ്കിൽ(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        പോകുക out;
      }
      അല്ലെങ്കിൽ
        infof(data, "  SSL certificate expiration date FAILED");
    }
    അല്ലെങ്കിൽ
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  എങ്കിൽ(certclock == (time_t)-1) {
    എങ്കിൽ(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(certclock > time(ശൂന്യ)) {
      എങ്കിൽ(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        പോകുക out;
      }
      അല്ലെങ്കിൽ
        infof(data, "  SSL certificate activation date FAILED");
    }
    അല്ലെങ്കിൽ
      infof(data, "  SSL certificate activation date OK");
  }

  എങ്കിൽ(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    എങ്കിൽ(result)
      പോകുക out;
  }
  അല്ലെങ്കിൽ
    infof(data, "  SSL certificate status verification SKIPPED");

  എങ്കിൽ(config->issuercert) {
    gnutls_datum_t issuerp;
    എങ്കിൽ(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      പോകുക out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    എങ്കിൽ(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      പോകുക out;
    }
    rc = (പൂർണ്ണസംഖ്യ)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    എങ്കിൽ(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      പോകുക out;
    }
    infof(data, "  SSL certificate issuer check OK (Issuer Cert: %s)",
          config->issuercert);
  }

  /* This function checks if the given certificate's subject matches the
     given hostname. This is a basic implementation of the matching described
     in RFC2818 (HTTPS), which takes into account wildcards, and the subject
     alternative name PKIX extension. Returns non zero on success, and zero on
     failure. */

  /* This function does not handle trailing dots, so if we have an SNI name
     use that and fallback to the hostname only if there is no SNI (like for
     IP addresses) */
  rc = (പൂർണ്ണസംഖ്യ)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_സന്ദേശം_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(pinned_താക്കോൽ) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_താക്കോൽ);
    എങ്കിൽ(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      പോകുക out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  എങ്കിൽ(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  എങ്കിൽ(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  തിരികെ result;
}

സ്ഥിതം CURLcode gtls_verifyserver(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  gnutls_session_t session)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  സ്ഥിരം അക്ഷരം *pinned_താക്കോൽ = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#അല്ലെങ്കിൽ
  സ്ഥിരം അക്ഷരം *pinned_താക്കോൽ = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_താക്കോൽ);
  എങ്കിൽ(result)
    പോകുക out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  എങ്കിൽ(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  തിരികെ result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
സ്ഥിതം CURLcode gtls_send_earlydata(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  സ്ഥിരം unsigned അക്ഷരം *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  ആകുമ്പോൾ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    എങ്കിൽ(n < 0) {
      എങ്കിൽ(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      അല്ലെങ്കിൽ
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  തിരികെ result;
}
#endif

/*
 * This function is called after the TCP connect has completed. Setup the TLS
 * layer and do all necessary magic.
 */
/* We use connssl->connecting_state to keep track of the connection status;
   there are three states: 'ssl_connect_1' (not started yet or complete),
   'ssl_connect_2' (doing handshake with the server), and
   'ssl_connect_3' (verifying and getting stats).
 */
സ്ഥിതം CURLcode gtls_connect_common(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    bool *done)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  എങ്കിൽ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  എങ്കിൽ(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
    connssl->connecting_state = ssl_connect_2;
  }

  എങ്കിൽ(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    എങ്കിൽ(connssl->earlydata_state == ssl_earlydata_await) {
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      എങ്കിൽ(result)
        പോകുക out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    എങ്കിൽ(result)
      പോകുക out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  എങ്കിൽ(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    പൂർണ്ണസംഖ്യ rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    എങ്കിൽ(result)
      പോകുക out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    എങ്കിൽ(rc) {  /* No ALPN from server */
      proto.data = ശൂന്യ;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    എങ്കിൽ(result)
      പോകുക out;

#ifdef CURL_GNUTLS_EARLY_DATA
    എങ്കിൽ(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_DATA) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
    connssl->connecting_state = ssl_connect_done;
  }

  എങ്കിൽ(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  എങ്കിൽ(result == CURLE_AGAIN) {
    *done = FALSE;
    തിരികെ CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (പൂർണ്ണസംഖ്യ)result,
              *done);
  തിരികെ result;
}

സ്ഥിതം CURLcode gtls_connect(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  ഘടന ssl_connect_data *connssl = cf->ctx;
  എങ്കിൽ((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    തിരികെ CURLE_OK;
  }
#endif
  തിരികെ gtls_connect_common(cf, data, done);
}

സ്ഥിതം bool gtls_data_pending(ഘടന Curl_cfilter *cf,
                              സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *ctx = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend;

  (ശൂന്യം)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ഘടന gtls_ssl_backend_data *)ctx->backend;
  എങ്കിൽ(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    തിരികെ TRUE;
  തിരികെ FALSE;
}

സ്ഥിതം CURLcode gtls_send(ഘടന Curl_cfilter *cf,
                          ഘടന Curl_easy *data,
                          സ്ഥിരം ശൂന്യം *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  ആകുമ്പോൾ(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    എങ്കിൽ(nwritten >= 0) {
      എങ്കിൽ((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        തിരികെ CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (അക്ഷരം *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        പോകുക out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      പോകുക out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
  തിരികെ result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
സ്ഥിതം CURLcode gtls_shutdown(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  അക്ഷരം buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  എങ്കിൽ(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    പോകുക out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  എങ്കിൽ(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    എങ്കിൽ(send_shutdown) {
      പൂർണ്ണസംഖ്യ ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      എങ്കിൽ((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        പോകുക out;
      }
      എങ്കിൽ(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((പൂർണ്ണസംഖ്യ)ret), (പൂർണ്ണസംഖ്യ)ret);
        result = CURLE_RECV_ERROR;
        പോകുക out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  വേണ്ടി(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, വലുപ്പം(buf));
    എങ്കിൽ(nread <= 0)
      നിർത്തുക;
  }
  എങ്കിൽ(nread > 0) {
    /* still data coming in? */
  }
  അല്ലെങ്കിൽ എങ്കിൽ(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((പൂർണ്ണസംഖ്യ)nread), (പൂർണ്ണസംഖ്യ)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം gtls_close(ഘടന Curl_cfilter *cf,
                       ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  എങ്കിൽ(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = ശൂന്യ;
  }
  എങ്കിൽ(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

സ്ഥിതം CURLcode gtls_recv(ഘടന Curl_cfilter *cf,
                          ഘടന Curl_easy *data,
                          അക്ഷരം *buf, size_t blen,
                          size_t *pnread)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  എങ്കിൽ(nread >= 0)
    *pnread = (size_t)nread;
  അല്ലെങ്കിൽ {
    എങ്കിൽ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      എങ്കിൽ(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      പോകുക out;
    }
    അല്ലെങ്കിൽ {
      failf(data, "GnuTLS recv error (%d): %s",
            (പൂർണ്ണസംഖ്യ)nread, gnutls_strerror((പൂർണ്ണസംഖ്യ)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      പോകുക out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (പൂർണ്ണസംഖ്യ)result, nread);
  തിരികെ result;
}

size_t Curl_gtls_version(അക്ഷരം *buffer, size_t size)
{
  തിരികെ curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(ശൂന്യ));
}

/* data might be NULL! */
സ്ഥിതം CURLcode gtls_random(ഘടന Curl_easy *data,
                            unsigned അക്ഷരം *entropy, size_t length)
{
  പൂർണ്ണസംഖ്യ rc;
  (ശൂന്യം)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  തിരികെ rc ? CURLE_FAILED_INIT : CURLE_OK;
}

സ്ഥിതം CURLcode gtls_sha256sum(സ്ഥിരം unsigned അക്ഷരം *നിവേശം,
                               size_t len,
                               unsigned അക്ഷരം *sha256sum, /* output */
                               size_t sha256len)
{
  ഘടന sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  ചെയ്യുക {
    unsigned പൂർണ്ണസംഖ്യ ilen = (unsigned പൂർണ്ണസംഖ്യ)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, നിവേശം);
    len -= ilen;
    നിവേശം += ilen;
  } ആകുമ്പോൾ(len);
#എങ്കിൽ NETTLE_VERSION_MAJOR >= 4
  (ശൂന്യം)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#അല്ലെങ്കിൽ
  sha256_digest(&SHA256pw, (unsigned പൂർണ്ണസംഖ്യ)sha256len, sha256sum);
#endif
  തിരികെ CURLE_OK;
}

സ്ഥിതം bool gtls_cert_status_request(ശൂന്യം)
{
  തിരികെ TRUE;
}

സ്ഥിതം ശൂന്യം *gtls_get_internals(ഘടന ssl_connect_data *connssl,
                                CURLINFO info)
{
  ഘടന gtls_ssl_backend_data *backend =
    (ഘടന gtls_ssl_backend_data *)connssl->backend;
  (ശൂന്യം)info;
  DEBUGASSERT(backend);
  തിരികെ backend->gtls.session;
}

സ്ഥിരം ഘടന Curl_ssl Curl_ssl_gnutls = {
  { CURLSSLBACKEND_GNUTLS, "gnutls" }, /* info */

  SSLSUPP_CA_PATH  |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_CA_CACHE |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_CRLFILE,

  വലുപ്പം(ഘടന gtls_ssl_backend_data),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_version,             /* version */
  gtls_shutdown,                 /* shutdown */
  gtls_data_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_status_request,      /* cert_status_request */
  gtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,       /* adjust_pollset */
  gtls_get_internals,            /* get_internals */
  gtls_close,                    /* close_one */
  ശൂന്യ,                          /* close_all */
  ശൂന്യ,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  ശൂന്യ,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
