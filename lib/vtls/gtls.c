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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_GNUTLS

#ಅನ್ತರ್ಭಾವ <gnutls/abstract.h>
#ಅನ್ತರ್ಭಾವ <gnutls/gnutls.h>
#ಅನ್ತರ್ಭಾವ <gnutls/x509.h>
#ಅನ್ತರ್ಭಾವ <gnutls/crypto.h>
#ಅನ್ತರ್ಭಾವ <nettle/sha2.h>
#ಅನ್ತರ್ಭಾವ <nettle/version.h>

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "vtls/keylog.h"
#ಅನ್ತರ್ಭಾವ "vtls/gtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vtls/apple.h"
#ಅನ್ತರ್ಭಾವ "vauth/vauth.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h"
#ಅನ್ತರ್ಭಾವ "connect.h" /* for the connect timeout */
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#ಯದಿ 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
ಸ್ಥಿರ ಶೂನ್ಯ tls_log_func(ಪೂರ್ಣಾಙ್ಕ level, ಧ್ರುವ ವರ್ಣ *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#ಯದಿ !defined(GNUTLS_VERSION_ಸಂಖ್ಯೆ) || (GNUTLS_VERSION_ಸಂಖ್ಯೆ < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#ಯದಿ GNUTLS_VERSION_ಸಂಖ್ಯೆ >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#ಅನ್ತರ್ಭಾವ <gnutls/ocsp.h>

ಸಂರಚನಾ gtls_ssl_backend_data {
  ಸಂರಚನಾ gtls_ctx gtls;
};

ಸ್ಥಿರ ssize_t gtls_ತಳ್ಳು(ಶೂನ್ಯ *s, ಧ್ರುವ ಶೂನ್ಯ *buf, size_t blen)
{
  ಸಂರಚನಾ Curl_cfilter *cf = s;
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಙ್ಕ)result, nwritten);
  backend->gtls.io_result = result;
  ಯದಿ(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಪ್ರತ್ಯಾಗಮ (ssize_t)nwritten;
}

ಸ್ಥಿರ ssize_t gtls_pull(ಶೂನ್ಯ *s, ಶೂನ್ಯ *buf, size_t blen)
{
  ಸಂರಚನಾ Curl_cfilter *cf = s;
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  ಯದಿ(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    ಯದಿ(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      ಪ್ರತ್ಯಾಗಮ -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (ಪೂರ್ಣಾಙ್ಕ)result,
              nread);
  backend->gtls.io_result = result;
  ಯದಿ(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಅನ್ಯಥಾ ಯದಿ(nread == 0)
    connssl->peer_closed = TRUE;
  ಪ್ರತ್ಯಾಗಮ (ssize_t)nread;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gtls_init(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_cleanup(ಶೂನ್ಯ)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
ಸ್ಥಿರ ಶೂನ್ಯ showtime(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *text, time_t stamp)
{
  ಸಂರಚನಾ tm buffer;
  ಧ್ರುವ ಸಂರಚನಾ tm *tm = &buffer;
  ವರ್ಣ str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ;

  curl_msnprintf(str,
                 ಪರಿಮಾಣ(str),
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

ಸ್ಥಿರ gnutls_datum_t load_file(ಧ್ರುವ ವರ್ಣ *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { ರಿಕ್ತ, 0 };
  long filelen;
  ಶೂನ್ಯ *ptr;

  f = curlx_fopen(file, "rb");
  ಯದಿ(!f)
    ಪ್ರತ್ಯಾಗಮ loaded_file;
  ಯದಿ(fseek(f, 0, SEEK_END))
    ಗಚ್ಛ out;
  filelen = ftell(f);
  ಯದಿ(filelen < 0 || filelen > CURL_MAX_ಒಳಹರಿವು_LENGTH)
    ಗಚ್ಛ out;
  ಯದಿ(fseek(f, 0, SEEK_SET))
    ಗಚ್ಛ out;
  ptr = curlx_malloc((size_t)filelen);
  ಯದಿ(!ptr)
    ಗಚ್ಛ out;
  ಯದಿ(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    ಗಚ್ಛ out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned ಪೂರ್ಣಾಙ್ಕ)filelen;
out:
  curlx_fclose(f);
  ಪ್ರತ್ಯಾಗಮ loaded_file;
}

ಸ್ಥಿರ ಶೂನ್ಯ unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
ಸ್ಥಿರ CURLcode cf_gtls_handshake(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  ಪೂರ್ಣಾಙ್ಕ rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  ಯದಿ(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  }
  ಅನ್ಯಥಾ ಯದಿ((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    ಧ್ರುವ ವರ್ಣ *strerr = ರಿಕ್ತ;

    ಯದಿ(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ಯದಿ(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
  }
  ಅನ್ಯಥಾ ಯದಿ((rc < 0) && backend->gtls.io_result) {
    ಪ್ರತ್ಯಾಗಮ backend->gtls.io_result;
  }
  ಅನ್ಯಥಾ ಯದಿ(rc < 0) {
    ಧ್ರುವ ವರ್ಣ *strerr = ರಿಕ್ತ;

    ಯದಿ(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ಯದಿ(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ gnutls_x509_crt_fmt_t gnutls_do_file_type(ಧ್ರುವ ವರ್ಣ *type)
{
  ಯದಿ(!type || !type[0])
    ಪ್ರತ್ಯಾಗಮ GNUTLS_X509_FMT_PEM;
  ಯದಿ(curl_strequal(type, "PEM"))
    ಪ್ರತ್ಯಾಗಮ GNUTLS_X509_FMT_PEM;
  ಯದಿ(curl_strequal(type, "DER"))
    ಪ್ರತ್ಯಾಗಮ GNUTLS_X509_FMT_DER;
  ಪ್ರತ್ಯಾಗಮ GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ಸ್ಥಿರ CURLcode gnutls_set_ssl_version_min_max(
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ ssl_peer *peer,
  ಸಂರಚನಾ ssl_filter_config *conn_config,
  ಧ್ರುವ ವರ್ಣ **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  ಯದಿ(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  ಯದಿ((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  ಯದಿ(peer->transport == TRNSPRT_QUIC) {
    ಯದಿ(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ವಿಕಲ್ಪ(ssl_version | ssl_version_max) {
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(ಸಂರಚನಾ Curl_easy *data,
                                       ಸಂರಚನಾ gtls_shared_creds **pcreds)
{
  ಸಂರಚನಾ gtls_shared_creds *shared;
  ಪೂರ್ಣಾಙ್ಕ rc;

  *pcreds = ರಿಕ್ತ;
  shared = curlx_calloc(1, ಪರಿಮಾಣ(*shared));
  ಯದಿ(!shared)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  ಯದಿ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(ಸಂರಚನಾ gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  ಯದಿ(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
}

ಶೂನ್ಯ Curl_gtls_shared_creds_free(ಸಂರಚನಾ gtls_shared_creds **pcreds)
{
  ಸಂರಚನಾ gtls_shared_creds *shared = *pcreds;
  *pcreds = ರಿಕ್ತ;
  ಯದಿ(shared) {
    --shared->refcount;
    ಯದಿ(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

ಸ್ಥಿರ CURLcode gtls_populate_creds(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  ಸಂರಚನಾ ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  ಯದಿ(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#ಅನ್ಯಥಾ
    rc = gnutls_certificate_set_x509_system_trust(creds);
    ಯದಿ(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    ಅನ್ಯಥಾ {
      infof(data, "  Native: %d certificates from system trust", rc);
      ಯದಿ(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  ಯದಿ(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    ಯದಿ(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned ಪೂರ್ಣಾಙ್ಕ)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    ಯದಿ(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      ಯದಿ(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ಅನ್ಯಥಾ
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  ಅನ್ಯಥಾ ಯದಿ(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    ಯದಿ(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      ಯದಿ(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ಅನ್ಯಥಾ
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  ಯದಿ(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    ಯದಿ(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      ಯದಿ(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ಅನ್ಯಥಾ
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  ಯದಿ(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  ಯದಿ(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  }
#endif
  ಯದಿ(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    ಯದಿ(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CRL_BADFILE;
    }
    ಅನ್ಯಥಾ
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_ಕೀಲಿ   "tls:gtls:x509:share"

ಸ್ಥಿರ bool gtls_shared_creds_expired(ಸಂರಚನಾ Curl_easy *data,
                                      ಧ್ರುವ ಸಂರಚನಾ gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  ಯದಿ(timeout_ms < 0)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಪ್ರತ್ಯಾಗಮ elapsed_ms >= timeout_ms;
}

ಸ್ಥಿರ bool gtls_shared_creds_different(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಧ್ರುವ ಸಂರಚನಾ gtls_shared_creds *sc)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಯದಿ(!sc->CAfile || !conn_config->CAfile)
    ಪ್ರತ್ಯಾಗಮ sc->CAfile != conn_config->CAfile;

  ಪ್ರತ್ಯಾಗಮ strcmp(sc->CAfile, conn_config->CAfile);
}

ಸ್ಥಿರ ಸಂರಚನಾ gtls_shared_creds *gtls_get_cached_creds(ಸಂರಚನಾ Curl_cfilter *cf,
                                                       ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ gtls_shared_creds *shared_creds;

  ಯದಿ(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_ಕೀಲಿ,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_ಕೀಲಿ));
    ಯದಿ(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      ಪ್ರತ್ಯಾಗಮ shared_creds;
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_shared_creds_hash_free(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ,
                                        size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p)
{
  ಸಂರಚನಾ gtls_shared_creds *sc = p;
  DEBUGASSERT(ಕೀಲಿ_len == CURL_CSTRLEN(MPROTO_GTLS_X509_ಕೀಲಿ));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_ಕೀಲಿ, ಕೀಲಿ, ಕೀಲಿ_len));
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)ಕೀಲಿ_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_set_cached_creds(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ gtls_shared_creds *sc)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  ಯದಿ(!data->multi)
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    ಯದಿ(!sc->CAfile)
      ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ(Curl_gtls_shared_creds_up_ref(sc))
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_ಕೀಲಿ,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_ಕೀಲಿ),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    ಪ್ರತ್ಯಾಗಮ;
  }
}

CURLcode Curl_gtls_client_trust_setup(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ gtls_ctx *gtls)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ gtls_shared_creds *cached_creds = ರಿಕ್ತ;
  bool cache_criteria_met;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ rc;

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

  ಯದಿ(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  ಯದಿ(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    ಯದಿ(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    gtls->shared_creds->trust_setup = TRUE;
    ಯದಿ(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gtls_get_ietf_proto(gnutls_session_t session)
{
  ವಿಕಲ್ಪ(gnutls_protocol_get_version(session)) {
  ಪಕ್ಷ GNUTLS_SSL3:
    ಪ್ರತ್ಯಾಗಮ CURL_IETF_PROTO_SSL3;
  ಪಕ್ಷ GNUTLS_TLS1_0:
    ಪ್ರತ್ಯಾಗಮ CURL_IETF_PROTO_TLS1;
  ಪಕ್ಷ GNUTLS_TLS1_1:
    ಪ್ರತ್ಯಾಗಮ CURL_IETF_PROTO_TLS1_1;
  ಪಕ್ಷ GNUTLS_TLS1_2:
    ಪ್ರತ್ಯಾಗಮ CURL_IETF_PROTO_TLS1_2;
  ಪಕ್ಷ GNUTLS_TLS1_3:
    ಪ್ರತ್ಯಾಗಮ CURL_IETF_PROTO_TLS1_3;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ಧ್ರುವ ವರ್ಣ *alpn,
                                 unsigned ವರ್ಣ *quic_tp,
                                 size_t quic_tp_len,
                                 ಸಂರಚನಾ Curl_ssl_session **psession)
{
  ಸಂರಚನಾ Curl_ssl_session *sc_session = ರಿಕ್ತ, *sc_dup = ರಿಕ್ತ;
  unsigned ವರ್ಣ *sdata, *qtp_clone = ರಿಕ್ತ;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  ಯದಿ(psession)
    *psession = ರಿಕ್ತ;
  ಯದಿ(!Curl_ssl_scache_use(cf, data))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, ರಿಕ್ತ, &sdata_len);
  ಯದಿ(!sdata_len) /* GnuTLS does this for some version combinations */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  ಯದಿ(!sdata)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  ಯದಿ(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ಧ್ರುವ ವರ್ಣ *)quic_tp, quic_tp_len);
    ಯದಿ(!qtp_clone) {
      curlx_free(sdata);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  ಯದಿ(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  ಯದಿ(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_ಕೀಲಿ, sc_session);
    /* took ownership of `sc_session` */
    sc_session = ರಿಕ್ತ;
  }
  ಯದಿ(!result && psession) {
    *psession = sc_dup;
    sc_dup = ರಿಕ್ತ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
ಸ್ಥಿರ CURLcode cf_gtls_update_session_id(ಸಂರಚನಾ Curl_cfilter *cf,
                                          ಸಂರಚನಾ Curl_easy *data,
                                          gnutls_session_t session)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ Curl_ssl_session *scs = ರಿಕ್ತ;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_ಕೀಲಿ,
                                   session, 0, connssl->negotiated.alpn,
                                   ರಿಕ್ತ, 0, &scs);
  ಯದಿ(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gtls_handshake_cb(gnutls_session_t session, unsigned ಪೂರ್ಣಾಙ್ಕ htype,
                             unsigned when, unsigned ಪೂರ್ಣಾಙ್ಕ incoming,
                             ಧ್ರುವ gnutls_datum_t *ಸಂದೇಶ)
{
  ಸಂರಚನಾ Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (ಶೂನ್ಯ)ಸಂದೇಶ;
  (ಶೂನ್ಯ)incoming;
  ಯದಿ(when) { /* after message has been processed */
    ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
    ಯದಿ(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      ವಿಕಲ್ಪ(htype) {
      ಪಕ್ಷ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        ಭಙ್ಗ;
      }
      ಸ್ವಭಾವ:
        ಭಙ್ಗ;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

ಸ್ಥಿರ CURLcode gtls_set_priority(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ gtls_ctx *gtls,
                                  ಧ್ರುವ ವರ್ಣ *priority)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ dynbuf buf;
  ಧ್ರುವ ವರ್ಣ *err = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ rc;

  curlx_dyn_init(&buf, 4096);

  ಯದಿ(conn_config->cipher_list) {
    ಯದಿ((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      ಯದಿ(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        ಯದಿ(result)
          ಗಚ್ಛ out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      priority = curlx_dyn_ptr(&buf);
    }
    ಅನ್ಯಥಾ /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  ಯದಿ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode gtls_client_init(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ssl_peer *peer,
                                 size_t earlydata_max,
                                 ಸಂರಚನಾ gtls_ctx *gtls)
{
  ಸಂರಚನಾ ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned ಪೂರ್ಣಾಙ್ಕ init_flags;
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಧ್ರುವ ವರ್ಣ *prioritylist;
  bool tls13support;
  CURLcode result;

  ಯದಿ(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  ಯದಿ(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  ಅನ್ಯಥಾ ಯದಿ(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  ಯದಿ(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  ಯದಿ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  ಯದಿ(peer->sni) {
    ಯದಿ(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  ಯದಿ(rc != GNUTLS_E_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  ಯದಿ(config->version == CURL_SSLVERSION_TLSv1_3) {
    ಯದಿ(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(config->clientcert) {
    ಯದಿ(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ಯದಿ(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->ಕೀಲಿ_passwd ? config->ಕೀಲಿ_passwd : "");
      ಯದಿ(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
      }
    }
    ಅನ್ಯಥಾ {
      ಧ್ರುವ unsigned ಪೂರ್ಣಾಙ್ಕ supported_ಕೀಲಿ_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_ಕೀಲಿ_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->ಕೀಲಿ ? config->ಕೀಲಿ : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->ಕೀಲಿ_passwd,
           supported_ಕೀಲಿ_encryption_algorithms);
      ಯದಿ(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->ಕೀಲಿ_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  ಯದಿ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  ಯದಿ(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  ರಿಕ್ತ, 0, ರಿಕ್ತ);
    ಯದಿ(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ keylog_callback(gnutls_session_t session, ಧ್ರುವ ವರ್ಣ *label,
                           ಧ್ರುವ gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  ಯದಿ(crandom.size != 32) {
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ CURLcode gtls_on_session_reuse(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ alpn_spec *alpns,
                                      ಸಂರಚನಾ Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  ಪ್ರತ್ಯಾಗಮ Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

ಸ್ಥಿರ CURLcode gtls_apply_session(
  ಸಂರಚನಾ gtls_ctx *gctx,
  ಸಂರಚನಾ Curl_cfilter *cf,
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ ssl_peer *peer,
  ಸಂರಚನಾ alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  ಸಂರಚನಾ Curl_ssl_session *scs,
  bool *pwas_setup)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ rc;

  ಯದಿ(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    ಯದಿ(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    ಅನ್ಯಥಾ {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      ಯದಿ(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        ಯದಿ(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          ಯದಿ(result)
            ಗಚ್ಛ out;
        }
        ಯದಿ(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_gtls_ctx_init(ಸಂರಚನಾ gtls_ctx *gctx,
                            ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಸಂರಚನಾ ssl_peer *peer,
                            ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            ಶೂನ್ಯ *cb_user_data,
                            ಶೂನ್ಯ *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ Curl_ssl_session *scs = ರಿಕ್ತ;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_ಎಣಿಕೆ = 0;
  bool gtls_session_setup = FALSE;
  ಸಂರಚನಾ alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  ಯದಿ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    ಯದಿ(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = ರಿಕ್ತ;
      ಯದಿ(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  ಯದಿ(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_ಕೀಲಿ, &scs);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  ಯದಿ(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  ಯದಿ(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  ಯದಿ(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  ಯದಿ(!gtls_alpns_ಎಣಿಕೆ && alpns.ಎಣಿಕೆ) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.ಎಣಿಕೆ);
    ಪ್ರತಿ(i = 0; i < alpns.ಎಣಿಕೆ; ++i) {
      gtls_alpns[i].data = (unsigned ವರ್ಣ *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned ಪೂರ್ಣಾಙ್ಕ)strlen(alpns.entries[i]);
    }
    gtls_alpns_ಎಣಿಕೆ = alpns.ಎಣಿಕೆ;
  }

  ಯದಿ(gtls_alpns_ಎಣಿಕೆ &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned ಪೂರ್ಣಾಙ್ಕ)gtls_alpns_ಎಣಿಕೆ,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_ಕೀಲಿ, scs);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode gtls_connect_step1(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  ಯದಿ(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, ರಿಕ್ತ, ರಿಕ್ತ, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#ಅನ್ಯಥಾ
                              ರಿಕ್ತ
#endif
                              );

  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ಸಂರಚನಾ alpn_proto_buf proto;
    memset(&proto, 0, ಪರಿಮಾಣ(proto));
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
  gnutls_transport_set_ತಳ್ಳು_function(backend->gtls.session, gtls_ತಳ್ಳು);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode pkp_pin_peer_pubkey(ಸಂರಚನಾ Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    ಧ್ರುವ ವರ್ಣ *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned ವರ್ಣ *buff1 = ರಿಕ್ತ;

  gnutls_pubkey_t ಕೀಲಿ = ರಿಕ್ತ;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ಯದಿ(!pinnedpubkey)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  ಯದಿ(!cert)
    ಪ್ರತ್ಯಾಗಮ result;

  ಕುರು {
    ಪೂರ್ಣಾಙ್ಕ ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&ಕೀಲಿ);
    ಯದಿ(ret < 0)
      ಭಙ್ಗ; /* failed */

    ret = gnutls_pubkey_import_x509(ಕೀಲಿ, cert, 0);
    ಯದಿ(ret < 0)
      ಭಙ್ಗ; /* failed */

    ret = gnutls_pubkey_export(ಕೀಲಿ, GNUTLS_X509_FMT_DER, ರಿಕ್ತ, &len1);
    ಯದಿ(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      ಭಙ್ಗ; /* failed */

    buff1 = curlx_malloc(len1);
    ಯದಿ(!buff1)
      ಭಙ್ಗ; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(ಕೀಲಿ, GNUTLS_X509_FMT_DER, buff1, &len2);
    ಯದಿ(ret < 0 || len1 != len2)
      ಭಙ್ಗ; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } ಯಾವತ್(0);

  ಯದಿ(ಕೀಲಿ)
    gnutls_pubkey_deinit(ಕೀಲಿ);

  curlx_safefree(buff1);

  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_gtls_report_handshake(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  ಯದಿ(Curl_trc_is_verbose(data)) {
    ಧ್ರುವ ವರ್ಣ *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)gctx;
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_ಸಂದೇಶ_verify_result(ಸಂರಚನಾ Curl_easy *data,
                                   ಸಂರಚನಾ ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  ವರ್ಣ certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = ಪರಿಮಾಣ(certname);
  ಪೂರ್ಣಾಙ್ಕ rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  ಯದಿ(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  ಯದಿ(!was_verified) {
    ಯದಿ(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    ಅನ್ಯಥಾ
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  ಅನ್ಯಥಾ
    infof(data, "  common name: %s (matched)", certname);
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_infof_cert(ಸಂರಚನಾ Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  ಯದಿ(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    ಪೂರ್ಣಾಙ್ಕ rc, algo;
    time_t tstamp;
    unsigned ಪೂರ್ಣಾಙ್ಕ bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    ಯದಿ(rc)
      infof(data, "Failed to get certificate name");
    ಅನ್ಯಥಾ {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    ಯದಿ(rc)
      infof(data, "Failed to get certificate issuer");
    ಅನ್ಯಥಾ {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)x509_cert;
#endif
}

ಸ್ಥಿರ CURLcode gtls_verify_ocsp_status(ಸಂರಚನಾ Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = ರಿಕ್ತ;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  ಪೂರ್ಣಾಙ್ಕ rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  ಯದಿ(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(rc < 0) {
    failf(data, "Invalid OCSP response received");
    ಗಚ್ಛ out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  ಯದಿ(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    ಗಚ್ಛ out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  ಯದಿ(rc < 0) {
    failf(data, "failed to parse OCSP response");
    ಗಚ್ಛ out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  ಯದಿ(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    ಗಚ್ಛ out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  ಪ್ರತಿ(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    ಯದಿ(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      ಭಙ್ಗ;
  }
  ಯದಿ(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    ಗಚ್ಛ out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ,
                                   &status, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
  ಯದಿ(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    ಗಚ್ಛ out;
  }
  ವಿಕಲ್ಪ(status) {
  ಪಕ್ಷ GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    ಭಙ್ಗ;
  ಪಕ್ಷ GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    failf(data, "peer certificate OCSP status is not good: %d", (ಪೂರ್ಣಾಙ್ಕ)status);
    ಭಙ್ಗ;
  }

out:
  ಯದಿ(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸಂರಚನಾ gtls_cert_chain {
  ಧ್ರುವ gnutls_datum_t *certs;
  unsigned ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_certs;
};

#ifdef USE_APPLE_SECTRUST
ಸ್ಥಿರ CURLcode gtls_chain_get_der(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಶೂನ್ಯ *user_data,
                                   size_t i,
                                   unsigned ವರ್ಣ **pder,
                                   size_t *pder_len)
{
  ಸಂರಚನಾ gtls_cert_chain *chain = user_data;

  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)data;
  *pder_len = 0;
  *pder = ರಿಕ್ತ;

  ಯದಿ(i >= chain->ಅಂಕೆ_certs)
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ಸ್ಥಿರ CURLcode gtls_verify_cert(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ ssl_filter_config *config,
                                 ಸಂರಚನಾ ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ ssl_peer *peer,
                                 ಸಂರಚನಾ gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned ಪೂರ್ಣಾಙ್ಕ verify_status = 0;
  long * ಧ್ರುವ certverifyresult = &ssl_config->certverifyresult;
  ಪೂರ್ಣಾಙ್ಕ rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  ಯದಿ(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  ಯದಿ(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  ಯದಿ(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    ಯದಿ(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = ರಿಕ್ತ;
      ocsp_raw.size = 0;
    }
    ಯದಿ(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->ಅಂಕೆ_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    ಯದಿ(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      ಪ್ರತ್ಯಾಗಮ result; /* no further failure analysis needed */
    verified = !result;
    ಯದಿ(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)peer;
  (ಶೂನ್ಯ)chain;
#endif

  ಯದಿ(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ಧ್ರುವ ವರ್ಣ *cause = "certificate error, no details available";
    ಯದಿ(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    ಅನ್ಯಥಾ ಯದಿ(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    ಅನ್ಯಥಾ ಯದಿ(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    ಅನ್ಯಥಾ ಯದಿ(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                gnutls_session_t session,
                                ಸಂರಚನಾ ssl_filter_config *config,
                                ಸಂರಚನಾ ssl_easy_config *ssl_config,
                                ಸಂರಚನಾ ssl_peer *peer,
                                ಧ್ರುವ ವರ್ಣ *pinned_ಕೀಲಿ)
{
  ಸಂರಚನಾ gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = ರಿಕ್ತ, x509_issuer = ರಿಕ್ತ;
  time_t certclock;
  bool ocsp_checked = FALSE;
  ಪೂರ್ಣಾಙ್ಕ rc;
  CURLcode result = CURLE_OK;
  long * ಧ್ರುವ certverifyresult = &ssl_config->certverifyresult;

  (ಶೂನ್ಯ)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.ಅಂಕೆ_certs);
  ಯದಿ(!chain.certs) {
    ಯದಿ(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      ಗಚ್ಛ out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  ಯದಿ(data->set.ssl.certinfo && chain.certs) {
    ಯದಿ(chain.ಅಂಕೆ_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.ಅಂಕೆ_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ {
      unsigned ಪೂರ್ಣಾಙ್ಕ i;

      result = Curl_ssl_init_certinfo(data, (ಪೂರ್ಣಾಙ್ಕ)chain.ಅಂಕೆ_certs);
      ಯದಿ(result)
        ಗಚ್ಛ out;

      ಪ್ರತಿ(i = 0; i < chain.ಅಂಕೆ_certs; i++) {
        ಧ್ರುವ uint8_t *beg = chain.certs[i].data;
        ಧ್ರುವ uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (ಪೂರ್ಣಾಙ್ಕ)i, beg, end);
        ಯದಿ(result)
          ಗಚ್ಛ out;
      }
    }
  }

  ಯದಿ(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  ಯದಿ(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    ಗಚ್ಛ out;
  }

  ಯದಿ(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    ಯದಿ(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  ಯದಿ(certclock == (time_t)-1) {
    ಯದಿ(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(certclock < time(ರಿಕ್ತ)) {
      ಯದಿ(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ
        infof(data, "  SSL certificate expiration date FAILED");
    }
    ಅನ್ಯಥಾ
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  ಯದಿ(certclock == (time_t)-1) {
    ಯದಿ(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(certclock > time(ರಿಕ್ತ)) {
      ಯದಿ(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ
        infof(data, "  SSL certificate activation date FAILED");
    }
    ಅನ್ಯಥಾ
      infof(data, "  SSL certificate activation date OK");
  }

  ಯದಿ(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ
    infof(data, "  SSL certificate status verification SKIPPED");

  ಯದಿ(config->issuercert) {
    gnutls_datum_t issuerp;
    ಯದಿ(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      ಗಚ್ಛ out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    ಯದಿ(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ಗಚ್ಛ out;
    }
    rc = (ಪೂರ್ಣಾಙ್ಕ)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    ಯದಿ(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ಗಚ್ಛ out;
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
  rc = (ಪೂರ್ಣಾಙ್ಕ)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_ಸಂದೇಶ_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(pinned_ಕೀಲಿ) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_ಕೀಲಿ);
    ಯದಿ(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      ಗಚ್ಛ out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  ಯದಿ(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  ಯದಿ(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode gtls_verifyserver(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  gnutls_session_t session)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  ಧ್ರುವ ವರ್ಣ *pinned_ಕೀಲಿ = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#ಅನ್ಯಥಾ
  ಧ್ರುವ ವರ್ಣ *pinned_ಕೀಲಿ = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_ಕೀಲಿ);
  ಯದಿ(result)
    ಗಚ್ಛ out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  ಯದಿ(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ಸ್ಥಿರ CURLcode gtls_send_earlydata(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ಧ್ರುವ unsigned ವರ್ಣ *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  ಯಾವತ್(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    ಯದಿ(n < 0) {
      ಯದಿ(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      ಅನ್ಯಥಾ
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ಪ್ರತ್ಯಾಗಮ result;
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
ಸ್ಥಿರ CURLcode gtls_connect_common(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    bool *done)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  ಯದಿ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  ಯದಿ(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    connssl->connecting_state = ssl_connect_2;
  }

  ಯದಿ(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    ಯದಿ(connssl->earlydata_state == ssl_earlydata_await) {
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  ಯದಿ(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    ಪೂರ್ಣಾಙ್ಕ rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    ಯದಿ(result)
      ಗಚ್ಛ out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    ಯದಿ(rc) {  /* No ALPN from server */
      proto.data = ರಿಕ್ತ;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    ಯದಿ(result)
      ಗಚ್ಛ out;

#ifdef CURL_GNUTLS_EARLY_DATA
    ಯದಿ(connssl->earlydata_state > ssl_earlydata_none) {
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

  ಯದಿ(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  ಯದಿ(result == CURLE_AGAIN) {
    *done = FALSE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (ಪೂರ್ಣಾಙ್ಕ)result,
              *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode gtls_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಯದಿ((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
#endif
  ಪ್ರತ್ಯಾಗಮ gtls_connect_common(cf, data, done);
}

ಸ್ಥಿರ bool gtls_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *ctx = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ಸಂರಚನಾ gtls_ssl_backend_data *)ctx->backend;
  ಯದಿ(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLcode gtls_send(ಸಂರಚನಾ Curl_cfilter *cf,
                          ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ ಶೂನ್ಯ *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  ಯಾವತ್(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    ಯದಿ(nwritten >= 0) {
      ಯದಿ((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (ವರ್ಣ *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        ಗಚ್ಛ out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      ಗಚ್ಛ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ಸ್ಥಿರ CURLcode gtls_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  ವರ್ಣ buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  ಯದಿ(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    ಗಚ್ಛ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  ಯದಿ(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    ಯದಿ(send_shutdown) {
      ಪೂರ್ಣಾಙ್ಕ ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      ಯದಿ((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        ಗಚ್ಛ out;
      }
      ಯದಿ(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((ಪೂರ್ಣಾಙ್ಕ)ret), (ಪೂರ್ಣಾಙ್ಕ)ret);
        result = CURLE_RECV_ERROR;
        ಗಚ್ಛ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ಪ್ರತಿ(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, ಪರಿಮಾಣ(buf));
    ಯದಿ(nread <= 0)
      ಭಙ್ಗ;
  }
  ಯದಿ(nread > 0) {
    /* still data coming in? */
  }
  ಅನ್ಯಥಾ ಯದಿ(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((ಪೂರ್ಣಾಙ್ಕ)nread), (ಪೂರ್ಣಾಙ್ಕ)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_close(ಸಂರಚನಾ Curl_cfilter *cf,
                       ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  ಯದಿ(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = ರಿಕ್ತ;
  }
  ಯದಿ(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ಸ್ಥಿರ CURLcode gtls_recv(ಸಂರಚನಾ Curl_cfilter *cf,
                          ಸಂರಚನಾ Curl_easy *data,
                          ವರ್ಣ *buf, size_t blen,
                          size_t *pnread)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  ಯದಿ(nread >= 0)
    *pnread = (size_t)nread;
  ಅನ್ಯಥಾ {
    ಯದಿ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      ಯದಿ(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ {
      failf(data, "GnuTLS recv error (%d): %s",
            (ಪೂರ್ಣಾಙ್ಕ)nread, gnutls_strerror((ಪೂರ್ಣಾಙ್ಕ)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      ಗಚ್ಛ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (ಪೂರ್ಣಾಙ್ಕ)result, nread);
  ಪ್ರತ್ಯಾಗಮ result;
}

size_t Curl_gtls_version(ವರ್ಣ *buffer, size_t size)
{
  ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(ರಿಕ್ತ));
}

/* data might be NULL! */
ಸ್ಥಿರ CURLcode gtls_random(ಸಂರಚನಾ Curl_easy *data,
                            unsigned ವರ್ಣ *entropy, size_t length)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  (ಶೂನ್ಯ)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  ಪ್ರತ್ಯಾಗಮ rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ಸ್ಥಿರ CURLcode gtls_sha256sum(ಧ್ರುವ unsigned ವರ್ಣ *ಒಳಹರಿವು,
                               size_t len,
                               unsigned ವರ್ಣ *sha256sum, /* output */
                               size_t sha256len)
{
  ಸಂರಚನಾ sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  ಕುರು {
    unsigned ಪೂರ್ಣಾಙ್ಕ ilen = (unsigned ಪೂರ್ಣಾಙ್ಕ)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, ಒಳಹರಿವು);
    len -= ilen;
    ಒಳಹರಿವು += ilen;
  } ಯಾವತ್(len);
#ಯದಿ NETTLE_VERSION_MAJOR >= 4
  (ಶೂನ್ಯ)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#ಅನ್ಯಥಾ
  sha256_digest(&SHA256pw, (unsigned ಪೂರ್ಣಾಙ್ಕ)sha256len, sha256sum);
#endif
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool gtls_cert_status_request(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ ಶೂನ್ಯ *gtls_get_internals(ಸಂರಚನಾ ssl_connect_data *connssl,
                                CURLINFO info)
{
  ಸಂರಚನಾ gtls_ssl_backend_data *backend =
    (ಸಂರಚನಾ gtls_ssl_backend_data *)connssl->backend;
  (ಶೂನ್ಯ)info;
  DEBUGASSERT(backend);
  ಪ್ರತ್ಯಾಗಮ backend->gtls.session;
}

ಧ್ರುವ ಸಂರಚನಾ Curl_ssl Curl_ssl_gnutls = {
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

  ಪರಿಮಾಣ(ಸಂರಚನಾ gtls_ssl_backend_data),

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
  ರಿಕ್ತ,                          /* close_all */
  ರಿಕ್ತ,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  ರಿಕ್ತ,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
