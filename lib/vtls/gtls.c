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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GNUTLS

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
#ಆದರೆ 0
#ವ್ಯಾಖ್ಯಾನಿಸು GTLSDEBUG
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GTLSDEBUG
ಸ್ಥಿರ ಶೂನ್ಯ tls_log_func(ಪೂರ್ಣಾಂಕ level, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(GNUTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ) || (GNUTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x030605)
#ದೋಷ "too old GnuTLS version"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
#ಆದರೆ GNUTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x030702
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <gnutls/ocsp.h>

ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ {
  ರಚನೆ gtls_ctx gtls;
};

ಸ್ಥಿರ ssize_t gtls_ತಳ್ಳು(ಶೂನ್ಯ *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t blen)
{
  ರಚನೆ Curl_cfilter *cf = s;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nwritten);
  backend->gtls.io_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_ಗಣ_errno(backend->gtls.session,
                               (ಫಲಿತಾಂಶ == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ಹಿಂತಿರುಗಿ -1;
  }
  ಹಿಂತಿರುಗಿ (ssize_t)nwritten;
}

ಸ್ಥಿರ ssize_t gtls_pull(ಶೂನ್ಯ *s, ಶೂನ್ಯ *buf, size_t blen)
{
  ರಚನೆ Curl_cfilter *cf = s;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  size_t nread;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(!backend->gtls.shared_creds->trust_ಸಿದ್ಧತೆ) {
    ಫಲಿತಾಂಶ = Curl_gtls_client_trust_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, &backend->gtls);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_ಗಣ_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
      ಹಿಂತಿರುಗಿ -1;
    }
  }

  ಫಲಿತಾಂಶ = Curl_conn_cf_recv(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, blen, &nread);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
              nread);
  backend->gtls.io_ಫಲಿತಾಂಶ = ಫಲಿತಾಂಶ;
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_ಗಣ_errno(backend->gtls.session,
                               (ಫಲಿತಾಂಶ == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ಹಿಂತಿರುಗಿ -1;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == 0)
    connssl->peer_closed = TRUE;
  ಹಿಂತಿರುಗಿ (ssize_t)nread;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gtls_init(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GTLSDEBUG
  gnutls_global_ಗಣ_log_function(tls_log_func);
  gnutls_global_ಗಣ_log_level(2);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_cleanup(ಶೂನ್ಯ)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
ಸ್ಥಿರ ಶೂನ್ಯ showtime(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ_3, time_t stamp)
{
  ರಚನೆ tm ಬಫರ್;
  ಸ್ಥಿರಾಂಕ ರಚನೆ tm *tm = &ಬಫರ್;
  ಅಕ್ಷರ str[96];
  CURLcode ಫಲಿತಾಂಶ = curlx_gmtime(stamp, &ಬಫರ್);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ;

  curl_msnprintf(str,
                 ಗಾತ್ರ(str),
                 "  %s: %s, %02d %s %4d %02d:%02d:%02d GMT",
                 ಪಠ್ಯ_3,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);
  infof(ದತ್ತಾಂಶ, "%s", str);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ gnutls_datum_t ತುಂಬು_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { ಖಾಲಿ, 0 };
  ದೀರ್ಘ filelen;
  ಶೂನ್ಯ *ptr;

  f = curlx_fopen(file, "rb");
  ಆದರೆ(!f)
    ಹಿಂತಿರುಗಿ loaded_file;
  ಆದರೆ(fseek(f, 0, SEEK_END))
    ಹೋಗು ಹೊರಹೋಗು;
  filelen = ftell(f);
  ಆದರೆ(filelen < 0 || filelen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(fseek(f, 0, SEEK_SET))
    ಹೋಗು ಹೊರಹೋಗು;
  ptr = curlx_malloc((size_t)filelen);
  ಆದರೆ(!ptr)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  loaded_file.ದತ್ತಾಂಶ = ptr;
  loaded_file.ಗಾತ್ರ_2 = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)filelen;
ಹೊರಹೋಗು:
  curlx_fclose(f);
  ಹಿಂತಿರುಗಿ loaded_file;
}

ಸ್ಥಿರ ಶೂನ್ಯ unload_file(gnutls_datum_t ದತ್ತಾಂಶ)
{
  curlx_free(ದತ್ತಾಂಶ.ದತ್ತಾಂಶ);
}

/* this function does an SSL/TLS (re-)handshake */
ಸ್ಥಿರ CURLcode cf_gtls_handshake(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  gnutls_session_t session;
  ಪೂರ್ಣಾಂಕ rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_ಫಲಿತಾಂಶ = CURLE_OK;
  rc = gnutls_handshake(session);

  ಆದರೆ(!backend->gtls.shared_creds->trust_ಸಿದ್ಧತೆ) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode ಫಲಿತಾಂಶ = Curl_gtls_client_trust_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, &backend->gtls);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_ದಾಖಲೆ_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *strerr = ಖಾಲಿ;

    ಆದರೆ(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ಆದರೆ(!strerr)
      strerr = gnutls_strerror(rc);

    infof(ದತ್ತಾಂಶ, "gnutls_handshake() warning: %s", strerr);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ CURLE_AGAIN;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((rc < 0) && backend->gtls.io_ಫಲಿತಾಂಶ) {
    ಹಿಂತಿರುಗಿ backend->gtls.io_ಫಲಿತಾಂಶ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc < 0) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *strerr = ಖಾಲಿ;

    ಆದರೆ(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ಆದರೆ(!strerr)
      strerr = gnutls_strerror(rc);

    failf(ದತ್ತಾಂಶ, "GnuTLS, handshake failed: %s", strerr);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ gnutls_x509_crt_fmt_t gnutls_do_file_type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type)
{
  ಆದರೆ(!type || !type[0])
    ಹಿಂತಿರುಗಿ GNUTLS_X509_FMT_PEM;
  ಆದರೆ(curl_strequal(type, "PEM"))
    ಹಿಂತಿರುಗಿ GNUTLS_X509_FMT_PEM;
  ಆದರೆ(curl_strequal(type, "DER"))
    ಹಿಂತಿರುಗಿ GNUTLS_X509_FMT_DER;
  ಹಿಂತಿರುಗಿ GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#ವ್ಯಾಖ್ಯಾನಿಸು GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#ವ್ಯಾಖ್ಯಾನಿಸು QUIC_ಆದ್ಯತೆ \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ಸ್ಥಿರ CURLcode gnutls_ಗಣ_ssl_ಆವೃತ್ತಿ_min_max(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ ssl_peer *peer,
  ರಚನೆ ssl_filter_config *conn_config,
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **prioritylist,
  bool tls13support)
{
  ದೀರ್ಘ ssl_ಆವೃತ್ತಿ = conn_config->ಆವೃತ್ತಿ_2;
  ದೀರ್ಘ ssl_ಆವೃತ್ತಿ_max = conn_config->ಆವೃತ್ತಿ_max;

  DEBUGASSERT(ssl_ಆವೃತ್ತಿ != CURL_SSLVERSION_DEFAULT);
  ಆದರೆ(ssl_ಆವೃತ್ತಿ <= CURL_SSLVERSION_TLSv1)
    ssl_ಆವೃತ್ತಿ = CURL_SSLVERSION_TLSv1_0;
  ಆದರೆ((ssl_ಆವೃತ್ತಿ_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_ಆವೃತ್ತಿ_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_ಆವೃತ್ತಿ_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  ಆದರೆ(peer->transport == TRNSPRT_QUIC) {
    ಆದರೆ(ssl_ಆವೃತ್ತಿ_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(ದತ್ತಾಂಶ, "QUIC needs at least TLS version 1.3");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_ಆದ್ಯತೆ;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆಯ್ಕೆ(ssl_ಆವೃತ್ತಿ | ssl_ಆವೃತ್ತಿ_max) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  failf(ದತ್ತಾಂಶ, "GnuTLS: cannot set TLS protocol");
  ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       ರಚನೆ gtls_shared_creds **pcreds)
{
  ರಚನೆ gtls_shared_creds *shared;
  ಪೂರ್ಣಾಂಕ rc;

  *pcreds = ಖಾಲಿ;
  shared = curlx_calloc(1, ಗಾತ್ರ(*shared));
  ಆದರೆ(!shared)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  ಆದರೆ(rc != GNUTLS_E_SUCCESS) {
    failf(ದತ್ತಾಂಶ, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->ಸಮಯ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
  *pcreds = shared;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_ಮೇಲೆ_ref(ರಚನೆ gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  ಆದರೆ(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
}

ಶೂನ್ಯ Curl_gtls_shared_creds_free(ರಚನೆ gtls_shared_creds **pcreds)
{
  ರಚನೆ gtls_shared_creds *shared = *pcreds;
  *pcreds = ಖಾಲಿ;
  ಆದರೆ(shared) {
    --shared->refcount;
    ಆದರೆ(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

ಸ್ಥಿರ CURLcode gtls_populate_creds(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    gnutls_certificate_credentials_t creds)
{
  ರಚನೆ ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ದತ್ತಾಂಶ);
  bool creds_are_ಖಾಲಿಗಳು = TRUE;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(!config->verifypeer) {
    infof(ದತ್ತಾಂಶ, "SSL Trust: peer verification disabled");
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  infof(ದತ್ತಾಂಶ, "SSL Trust Anchors:");
  ಆದರೆ(config->native_ca_store) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
    infof(ದತ್ತಾಂಶ, "  Native: Apple SecTrust");
    creds_are_ಖಾಲಿಗಳು = FALSE;
#ಇಲ್ಲದಿದ್ದರೆ
    rc = gnutls_certificate_ಗಣ_x509_system_trust(creds);
    ಆದರೆ(rc < 0)
      infof(ದತ್ತಾಂಶ, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, "  Native: %d certificates from system trust", rc);
      ಆದರೆ(rc > 0)
        creds_are_ಖಾಲಿಗಳು = FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆದರೆ(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    ಆದರೆ(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(ದತ್ತಾಂಶ, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.ದತ್ತಾಂಶ = config->ca_info_blob->ದತ್ತಾಂಶ;
    ca_info_datum.ಗಾತ್ರ_2 = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)config->ca_info_blob->len;
    rc = gnutls_certificate_ಗಣ_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_ಖಾಲಿಗಳು = creds_are_ಖಾಲಿಗಳು && (rc <= 0);
    ಆದರೆ(rc < 0) {
      infof(ದತ್ತಾಂಶ, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_ಖಾಲಿಗಳು ? "" : ", continuing anyway"));
      ಆದರೆ(creds_are_ಖಾಲಿಗಳು) {
        ssl_config->certverifyresult = rc;
        ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_ಗಣ_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_ಗಣ_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_ಖಾಲಿಗಳು = creds_are_ಖಾಲಿಗಳು && (rc <= 0);
    ಆದರೆ(rc < 0) {
      infof(ದತ್ತಾಂಶ, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_ಖಾಲಿಗಳು ? "" : ", continuing anyway"));
      ಆದರೆ(creds_are_ಖಾಲಿಗಳು) {
        ssl_config->certverifyresult = rc;
        ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  ಆದರೆ(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_ಗಣ_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_ಖಾಲಿಗಳು = creds_are_ಖಾಲಿಗಳು && (rc <= 0);
    ಆದರೆ(rc < 0) {
      infof(ದತ್ತಾಂಶ, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_ಖಾಲಿಗಳು ? "" : ", continuing anyway"));
      ಆದರೆ(creds_are_ಖಾಲಿಗಳು) {
        ssl_config->certverifyresult = rc;
        ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  ಆದರೆ(creds_are_ಖಾಲಿಗಳು)
    infof(ದತ್ತಾಂಶ, "  no trust anchors configured");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
  ಆದರೆ(config->CRLfile && config->native_ca_store) {
    failf(ದತ್ತಾಂಶ, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_ಗಣ_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    ಆದರೆ(rc < 0) {
      failf(ದತ್ತಾಂಶ, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      ಹಿಂತಿರುಗಿ CURLE_SSL_CRL_BADFILE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#ವ್ಯಾಖ್ಯಾನಿಸು MPROTO_GTLS_X509_ಕೀಲಿ   "tls:gtls:x509:share"

ಸ್ಥಿರ bool gtls_shared_creds_expired(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ಸ್ಥಿರಾಂಕ ರಚನೆ gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(ದತ್ತಾಂಶ), &sc->ಸಮಯ);
  timediff_t timeout_ms = ದತ್ತಾಂಶ->ಗಣ.ssl_ca_cache_timeout * (timediff_t)1000;

  ಆದರೆ(timeout_ms < 0)
    ಹಿಂತಿರುಗಿ FALSE;

  ಹಿಂತಿರುಗಿ elapsed_ms >= timeout_ms;
}

ಸ್ಥಿರ bool gtls_shared_creds_different(ರಚನೆ Curl_cfilter *cf,
                                        ಸ್ಥಿರಾಂಕ ರಚನೆ gtls_shared_creds *sc)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಆದರೆ(!sc->CAfile || !conn_config->CAfile)
    ಹಿಂತಿರುಗಿ sc->CAfile != conn_config->CAfile;

  ಹಿಂತಿರುಗಿ strcmp(sc->CAfile, conn_config->CAfile);
}

ಸ್ಥಿರ ರಚನೆ gtls_shared_creds *gtls_get_cached_creds(ರಚನೆ Curl_cfilter *cf,
                                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ gtls_shared_creds *shared_creds;

  ಆದರೆ(ದತ್ತಾಂಶ->multi) {
    shared_creds = Curl_hash_pick(&ದತ್ತಾಂಶ->multi->proto_hash,
                                  MPROTO_GTLS_X509_ಕೀಲಿ,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_ಕೀಲಿ));
    ಆದರೆ(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(ದತ್ತಾಂಶ, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      ಹಿಂತಿರುಗಿ shared_creds;
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_shared_creds_hash_free(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ,
                                        size_t ಕೀಲಿ_len, ಶೂನ್ಯ *p)
{
  ರಚನೆ gtls_shared_creds *sc = p;
  DEBUGASSERT(ಕೀಲಿ_len == CURL_CSTRLEN(MPROTO_GTLS_X509_ಕೀಲಿ));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_ಕೀಲಿ, ಕೀಲಿ, ಕೀಲಿ_len));
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)ಕೀಲಿ_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_ಗಣ_cached_creds(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ gtls_shared_creds *sc)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  ಆದರೆ(!ದತ್ತಾಂಶ->multi)
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    ಆದರೆ(!sc->CAfile)
      ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(Curl_gtls_shared_creds_ಮೇಲೆ_ref(sc))
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(!Curl_hash_add2(&ದತ್ತಾಂಶ->multi->proto_hash,
                     MPROTO_GTLS_X509_ಕೀಲಿ,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_ಕೀಲಿ),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    ಹಿಂತಿರುಗಿ;
  }
}

CURLcode Curl_gtls_client_trust_ಸಿದ್ಧತೆ(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ gtls_ctx *gtls)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ gtls_shared_creds *cached_creds = ಖಾಲಿ;
  bool cache_criteria_met;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (ದತ್ತಾಂಶ->ಗಣ.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  ಆದರೆ(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, ದತ್ತಾಂಶ);

  ಆದರೆ(cached_creds && !Curl_gtls_shared_creds_ಮೇಲೆ_ref(cached_creds)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_ಗಣ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    ಆದರೆ(rc != GNUTLS_E_SUCCESS) {
      failf(ದತ್ತಾಂಶ, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "loading trust anchors and CRLs");
    ಫಲಿತಾಂಶ = gtls_populate_creds(cf, ದತ್ತಾಂಶ, gtls->shared_creds->creds);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    gtls->shared_creds->trust_ಸಿದ್ಧತೆ = TRUE;
    ಆದರೆ(cache_criteria_met)
      gtls_ಗಣ_cached_creds(cf, ದತ್ತಾಂಶ, gtls->shared_creds);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gtls_get_ietf_proto(gnutls_session_t session)
{
  ಆಯ್ಕೆ(gnutls_protocol_get_ಆವೃತ್ತಿ(session)) {
  ಸ್ಥಿತಿ GNUTLS_SSL3:
    ಹಿಂತಿರುಗಿ CURL_IETF_PROTO_SSL3;
  ಸ್ಥಿತಿ GNUTLS_TLS1_0:
    ಹಿಂತಿರುಗಿ CURL_IETF_PROTO_TLS1;
  ಸ್ಥಿತಿ GNUTLS_TLS1_1:
    ಹಿಂತಿರುಗಿ CURL_IETF_PROTO_TLS1_1;
  ಸ್ಥಿತಿ GNUTLS_TLS1_2:
    ಹಿಂತಿರುಗಿ CURL_IETF_PROTO_TLS1_2;
  ಸ್ಥಿತಿ GNUTLS_TLS1_3:
    ಹಿಂತಿರುಗಿ CURL_IETF_PROTO_TLS1_3;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn,
                                 ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *quic_tp,
                                 size_t quic_tp_len,
                                 ರಚನೆ Curl_ssl_session **psession)
{
  ರಚನೆ Curl_ssl_session *sc_session = ಖಾಲಿ, *sc_dup = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sdata, *qtp_clone = ಖಾಲಿ;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(psession)
    *psession = ಖಾಲಿ;
  ಆದರೆ(!Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_ದತ್ತಾಂಶ(session, ಖಾಲಿ, &sdata_len);
  ಆದರೆ(!sdata_len) /* GnuTLS does this for some version combinations */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  ಆದರೆ(!sdata)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_ದತ್ತಾಂಶ(session, sdata, &sdata_len);
  earlydata_max = gnutls_ದಾಖಲೆ_get_max_early_ದತ್ತಾಂಶ_ಗಾತ್ರ(session);

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  ಆದರೆ(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)quic_tp, quic_tp_len);
    ಆದರೆ(!qtp_clone) {
      curlx_free(sdata);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  ಫಲಿತಾಂಶ = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  ಆದರೆ(!ಫಲಿತಾಂಶ && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    ಫಲಿತಾಂಶ = Curl_ssl_session_dup(sc_session, &sc_dup);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = Curl_ssl_scache_put(cf, ದತ್ತಾಂಶ, ssl_peer_ಕೀಲಿ, sc_session);
    /* took ownership of `sc_session` */
    sc_session = ಖಾಲಿ;
  }
  ಆದರೆ(!ಫಲಿತಾಂಶ && psession) {
    *psession = sc_dup;
    sc_dup = ಖಾಲಿ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
ಸ್ಥಿರ CURLcode cf_gtls_update_session_id(ರಚನೆ Curl_cfilter *cf,
                                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                          gnutls_session_t session)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ Curl_ssl_session *scs = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_gtls_cache_session(cf, ದತ್ತಾಂಶ, connssl->peer.scache_ಕೀಲಿ,
                                   session, 0, connssl->negotiated.alpn,
                                   ಖಾಲಿ, 0, &scs);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gtls_handshake_cb(gnutls_session_t session, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ htype,
                             ಚಿಹ್ನೆರಹಿತ when, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ incoming,
                             ಸ್ಥಿರಾಂಕ gnutls_datum_t *ಸಂದೇಶ)
{
  ರಚನೆ Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (ಶೂನ್ಯ)ಸಂದೇಶ;
  (ಶೂನ್ಯ)incoming;
  ಆದರೆ(when) { /* after message has been processed */
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
    ಆದರೆ(ದತ್ತಾಂಶ) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      ಆಯ್ಕೆ(htype) {
      ಸ್ಥಿತಿ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, ದತ್ತಾಂಶ, session);
        ನಿಲ್ಲಿಸು;
      }
      ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
      }
    }
  }
  ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode gtls_ಗಣ_ಆದ್ಯತೆ(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ gtls_ctx *gtls,
                                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಆದ್ಯತೆ)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ dynbuf buf;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *err = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ rc;

  curlx_dyn_init(&buf, 4096);

  ಆದರೆ(conn_config->cipher_list) {
    ಆದರೆ((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      ಆದರೆ(!curlx_dyn_len(&buf)) {  /* not added yet */
        ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&buf, ಆದ್ಯತೆ);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
      }
      ಫಲಿತಾಂಶ = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      ಆದ್ಯತೆ = curlx_dyn_ptr(&buf);
    }
    ಇಲ್ಲದಿದ್ದರೆ /* replace our own completely */
      ಆದ್ಯತೆ = conn_config->cipher_list;
  }

  infof(ದತ್ತಾಂಶ, "GnuTLS priority: %s", ಆದ್ಯತೆ);
  rc = gnutls_ಆದ್ಯತೆ_ಗಣ_ನೇರ(gtls->session, ಆದ್ಯತೆ, &err);
  ಆದರೆ(rc != GNUTLS_E_SUCCESS) {
    failf(ದತ್ತಾಂಶ, "Error %d setting GnuTLS priority: %s", rc, err);
    ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
  }

ಹೊರಹೋಗು:
  curlx_dyn_free(&buf);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode gtls_client_init(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ssl_peer *peer,
                                 size_t earlydata_max,
                                 ರಚನೆ gtls_ctx *gtls)
{
  ರಚನೆ ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ದತ್ತಾಂಶ);
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ init_flags;
  ಪೂರ್ಣಾಂಕ rc;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prioritylist;
  bool tls13support;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(config->ಆವೃತ್ತಿ_2 == CURL_SSLVERSION_SSLv2 ||
     config->ಆವೃತ್ತಿ_2 == CURL_SSLVERSION_SSLv3) {
    failf(ದತ್ತಾಂಶ, "GnuTLS does not support SSLv2 or SSLv3");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  ಫಲಿತಾಂಶ = Curl_gtls_shared_creds_create(ದತ್ತಾಂಶ, &gtls->shared_creds);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
  ಆದರೆ(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_ದತ್ತಾಂಶ | GNUTLS_NO_ಕೊನೆ_ದ_EARLY_ದತ್ತಾಂಶ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GNUTLS_NO_ಸ್ಥಿತಿ_REQUEST
  ಆದರೆ(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_ಸ್ಥಿತಿ_REQUEST;
#ಆದರೆ_ಅಂತ್ಯ

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  ಆದರೆ(rc != GNUTLS_E_SUCCESS) {
    failf(ದತ್ತಾಂಶ, "gnutls_init() failed: %d", rc);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಆದರೆ(peer->sni) {
    ಆದರೆ(gnutls_server_name_ಗಣ(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(peer->sni)) < 0) {
      failf(ದತ್ತಾಂಶ, "Failed to set SNI");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_ಗಣ_default_ಆದ್ಯತೆ(gtls->session);
  ಆದರೆ(rc != GNUTLS_E_SUCCESS)
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_ಆವೃತ್ತಿ("3.6.5");

  ಆದರೆ(config->ಆವೃತ್ತಿ_2 == CURL_SSLVERSION_TLSv1_3) {
    ಆದರೆ(!tls13support) {
      failf(ದತ್ತಾಂಶ, "This GnuTLS installation does not support TLS 1.3");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  ಫಲಿತಾಂಶ = gnutls_ಗಣ_ssl_ಆವೃತ್ತಿ_min_max(ದತ್ತಾಂಶ, peer,
                                          config, &prioritylist, tls13support);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = gtls_ಗಣ_ಆದ್ಯತೆ(cf, ದತ್ತಾಂಶ, gtls, prioritylist);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(config->clientcert) {
    ಆದರೆ(!gtls->shared_creds->trust_ಸಿದ್ಧತೆ) {
      ಫಲಿತಾಂಶ = Curl_gtls_client_trust_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, gtls);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಆದರೆ(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_ಗಣ_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->ಕೀಲಿ_passwd ? config->ಕೀಲಿ_passwd : "");
      ಆದರೆ(rc != GNUTLS_E_SUCCESS) {
        failf(ದತ್ತಾಂಶ,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ supported_ಕೀಲಿ_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_ಗಣ_x509_ಕೀಲಿ_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->ಕೀಲಿ ? config->ಕೀಲಿ : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->ಕೀಲಿ_passwd,
           supported_ಕೀಲಿ_encryption_algorithms);
      ಆದರೆ(rc != GNUTLS_E_SUCCESS) {
        failf(ದತ್ತಾಂಶ,
              "error reading X.509 %skey file: %s",
              config->ಕೀಲಿ_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_ಗಣ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  ಆದರೆ(rc != GNUTLS_E_SUCCESS) {
    failf(ದತ್ತಾಂಶ, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಆದರೆ(config->verifystatus) {
    rc = gnutls_ocsp_ಸ್ಥಿತಿ_request_enable_client(gtls->session,
                                                  ಖಾಲಿ, 0, ಖಾಲಿ);
    ಆದರೆ(rc != GNUTLS_E_SUCCESS) {
      failf(ದತ್ತಾಂಶ, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ keylog_callback(gnutls_session_t session, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label,
                           ಸ್ಥಿರಾಂಕ gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  ಆದರೆ(crandom.ಗಾತ್ರ_2 != 32) {
    ಹಿಂತಿರುಗಿ -1;
  }

  Curl_tls_keylog_write(label, crandom.ದತ್ತಾಂಶ, crandom.ಗಾತ್ರ_2,
                        secret->ದತ್ತಾಂಶ, secret->ಗಾತ್ರ_2);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ CURLcode gtls_on_session_reuse(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ alpn_spec *alpns,
                                      ರಚನೆ Curl_ssl_session *scs,
                                      bool *do_early_ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;

  connssl->earlydata_max =
    gnutls_ದಾಖಲೆ_get_max_early_ದತ್ತಾಂಶ_ಗಾತ್ರ(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  ಹಿಂತಿರುಗಿ Curl_on_session_reuse(cf, ದತ್ತಾಂಶ, alpns, scs, do_early_ದತ್ತಾಂಶ,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode gtls_apply_session(
  ರಚನೆ gtls_ctx *gctx,
  ರಚನೆ Curl_cfilter *cf,
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ ssl_peer *peer,
  ರಚನೆ alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  ರಚನೆ Curl_ssl_session *scs,
  bool *pwas_ಸಿದ್ಧತೆ)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ rc;

  ಆದರೆ(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_ಒಳಗೊಂಡಿದೆ_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    ಫಲಿತಾಂಶ = gtls_client_init(cf, ದತ್ತಾಂಶ, peer, scs->earlydata_max, gctx);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    *pwas_ಸಿದ್ಧತೆ = TRUE;

    rc = gnutls_session_ಗಣ_ದತ್ತಾಂಶ(gctx->session, scs->sdata, scs->sdata_len);
    ಆದರೆ(rc < 0)
      infof(ದತ್ತಾಂಶ, "SSL session not accepted by GnuTLS, continuing without");
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      ಆದರೆ(conn_config->earlydata && scs->alpn &&
         !cf->conn->ಬಿಟ್ಟುಗಳು.connect_only) {
        bool do_early_ದತ್ತಾಂಶ = FALSE;
        ಆದರೆ(sess_reuse_cb) {
          ಫಲಿತಾಂಶ = sess_reuse_cb(cf, ದತ್ತಾಂಶ, alpns, scs, &do_early_ದತ್ತಾಂಶ);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹೋಗು ಹೊರಹೋಗು;
        }
        ಆದರೆ(do_early_ದತ್ತಾಂಶ) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_ಗಮ್ಯ(alpns, scs->alpn);
        }
      }
    }
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_gtls_ctx_init(ರಚನೆ gtls_ctx *gctx,
                            ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ರಚನೆ ssl_peer *peer,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpns_requested,
                            Curl_gtls_ctx_ಸಿದ್ಧತೆ_cb *cb_ಸಿದ್ಧತೆ,
                            ಶೂನ್ಯ *cb_user_ದತ್ತಾಂಶ,
                            ಶೂನ್ಯ *ssl_user_ದತ್ತಾಂಶ,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ Curl_ssl_session *scs = ಖಾಲಿ;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_ಎಣಿಕೆ = 0;
  bool gtls_session_ಸಿದ್ಧತೆ = FALSE;
  ರಚನೆ alpn_spec alpns;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  ಆದರೆ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(ದತ್ತಾಂಶ, cf->cft, FIRSTSOCKET);
    ಆದರೆ(scs) {
      ಫಲಿತಾಂಶ = gtls_apply_session(gctx, cf, ದತ್ತಾಂಶ, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_ಸಿದ್ಧತೆ);
      scs = ಖಾಲಿ;
      ಆದರೆ(!ಫಲಿತಾಂಶ && gtls_session_ಸಿದ್ಧತೆ) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  ಆದರೆ(!gtls_session_ಸಿದ್ಧತೆ && conn_config->cache_session &&
     !conn_config->verifystatus) {
    ಫಲಿತಾಂಶ = Curl_ssl_scache_take(cf, ದತ್ತಾಂಶ, peer->scache_ಕೀಲಿ, &scs);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಫಲಿತಾಂಶ = gtls_apply_session(gctx, cf, ದತ್ತಾಂಶ, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_ಸಿದ್ಧತೆ);
  }

  ಆದರೆ(!gtls_session_ಸಿದ್ಧತೆ) {
    ಫಲಿತಾಂಶ = gtls_client_init(cf, ದತ್ತಾಂಶ, peer, 0, gctx);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  gnutls_session_ಗಣ_ptr(gctx->session, ssl_user_ದತ್ತಾಂಶ);

  ಆದರೆ(cb_ಸಿದ್ಧತೆ) {
    ಫಲಿತಾಂಶ = cb_ಸಿದ್ಧತೆ(cf, ದತ್ತಾಂಶ, cb_user_ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  ಆದರೆ(Curl_tls_keylog_enabled()) {
    gnutls_session_ಗಣ_keylog_function(gctx->session, keylog_callback);
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  ಆದರೆ(!gtls_alpns_ಎಣಿಕೆ && alpns.ಎಣಿಕೆ_2) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.ಎಣಿಕೆ_2);
    ಗಾಗಿ(i = 0; i < alpns.ಎಣಿಕೆ_2; ++i) {
      gtls_alpns[i].ದತ್ತಾಂಶ = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)alpns.entries[i];
      gtls_alpns[i].ಗಾತ್ರ_2 = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(alpns.entries[i]);
    }
    gtls_alpns_ಎಣಿಕೆ = alpns.ಎಣಿಕೆ_2;
  }

  ಆದರೆ(gtls_alpns_ಎಣಿಕೆ &&
     gnutls_alpn_ಗಣ_protocols(gctx->session,
                               gtls_alpns, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)gtls_alpns_ಎಣಿಕೆ,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(ದತ್ತಾಂಶ, "failed setting ALPN");
    ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
  }

ಹೊರಹೋಗು:
  Curl_ssl_scache_return(cf, ದತ್ತಾಂಶ, peer->scache_ಕೀಲಿ, scs);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode gtls_connect_step1(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(backend);

  ಆದರೆ(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = Curl_gtls_ctx_init(&backend->gtls, cf, ದತ್ತಾಂಶ, &connssl->peer,
                              connssl->alpn, ಖಾಲಿ, ಖಾಲಿ, cf,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
                              gtls_on_session_reuse
#ಇಲ್ಲದಿದ್ದರೆ
                              ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
                              );

  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ರಚನೆ alpn_proto_buf proto;
    memset(&proto, 0, ಗಾತ್ರ(proto));
    Curl_alpn_ಗಮ್ಯ_proto_str(&proto, connssl->alpn);
    infof(ದತ್ತಾಂಶ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ದತ್ತಾಂಶ);
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
  gnutls_handshake_ಗಣ_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#ಆದರೆ_ಅಂತ್ಯ

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_ಗಣ_ptr(backend->gtls.session, cf);
  gnutls_transport_ಗಣ_ತಳ್ಳು_function(backend->gtls.session, gtls_ತಳ್ಳು);
  gnutls_transport_ಗಣ_pull_function(backend->gtls.session, gtls_pull);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode pkp_pin_peer_pubkey(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    gnutls_x509_crt_t cert,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buff1 = ಖಾಲಿ;

  gnutls_pubkey_t ಕೀಲಿ = ಖಾಲಿ;

  /* Result is returned to caller */
  CURLcode ಫಲಿತಾಂಶ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ಆದರೆ(!pinnedpubkey)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಆದರೆ(!cert)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಮಾಡು {
    ಪೂರ್ಣಾಂಕ ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&ಕೀಲಿ);
    ಆದರೆ(ret < 0)
      ನಿಲ್ಲಿಸು; /* failed */

    ret = gnutls_pubkey_import_x509(ಕೀಲಿ, cert, 0);
    ಆದರೆ(ret < 0)
      ನಿಲ್ಲಿಸು; /* failed */

    ret = gnutls_pubkey_export(ಕೀಲಿ, GNUTLS_X509_FMT_DER, ಖಾಲಿ, &len1);
    ಆದರೆ(ret != GNUTLS_E_SHORT_ಸ್ಮರಣೆ_ಬಫರ್ || len1 == 0)
      ನಿಲ್ಲಿಸು; /* failed */

    buff1 = curlx_malloc(len1);
    ಆದರೆ(!buff1)
      ನಿಲ್ಲಿಸು; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(ಕೀಲಿ, GNUTLS_X509_FMT_DER, buff1, &len2);
    ಆದರೆ(ret < 0 || len1 != len2)
      ನಿಲ್ಲಿಸು; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    ಫಲಿತಾಂಶ = Curl_pin_peer_pubkey(ದತ್ತಾಂಶ, pinnedpubkey, buff1, len1);
  } ಆಗ(0);

  ಆದರೆ(ಕೀಲಿ)
    gnutls_pubkey_deinit(ಕೀಲಿ);

  curlx_safefree(buff1);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_gtls_report_handshake(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ gtls_ctx *gctx)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr;
    gnutls_protocol_t ಆವೃತ್ತಿ_2 = gnutls_protocol_get_ಆವೃತ್ತಿ(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(ದತ್ತಾಂಶ, "SSL connection using %s / %s",
          gnutls_protocol_get_name(ಆವೃತ್ತಿ_2), ptr);
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)gctx;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_ಸಂದೇಶ_verify_ಫಲಿತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ರಚನೆ ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  ಅಕ್ಷರ certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t ಗಾತ್ರ_2 = ಗಾತ್ರ(certname);
  ಪೂರ್ಣಾಂಕ rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &ಗಾತ್ರ_2);
  ಆದರೆ(rc) {
    infof(ದತ್ತಾಂಶ, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  ಆದರೆ(!was_verified) {
    ಆದರೆ(needs_verified) {
      failf(ದತ್ತಾಂಶ, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    infof(ದತ್ತಾಂಶ, "  common name: %s (matched)", certname);
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_infof_cert(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            gnutls_x509_crt_t x509_cert)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  ಆದರೆ(Curl_trc_is_verbose(ದತ್ತಾಂಶ)) {
    gnutls_datum_t certfields;
    ಪೂರ್ಣಾಂಕ rc, algo;
    time_t tstamp;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &ಬಿಟ್ಟುಗಳು);
    infof(ದತ್ತಾಂಶ, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(ದತ್ತಾಂಶ, "  certificate version: #%d",
          gnutls_x509_crt_get_ಆವೃತ್ತಿ(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    ಆದರೆ(rc)
      infof(ದತ್ತಾಂಶ, "Failed to get certificate name");
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, "  subject: %s", certfields.ದತ್ತಾಂಶ);

      tstamp = gnutls_x509_crt_get_activation_ಸಮಯ(x509_cert);
      showtime(ದತ್ತಾಂಶ, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_ಸಮಯ(x509_cert);
      showtime(ದತ್ತಾಂಶ, "expire date", tstamp);

      gnutls_free(certfields.ದತ್ತಾಂಶ);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    ಆದರೆ(rc)
      infof(ದತ್ತಾಂಶ, "Failed to get certificate issuer");
    ಇಲ್ಲದಿದ್ದರೆ {
      infof(ದತ್ತಾಂಶ, "  issuer: %s", certfields.ದತ್ತಾಂಶ);

      gnutls_free(certfields.ದತ್ತಾಂಶ);
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  (ಶೂನ್ಯ)x509_cert;
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ CURLcode gtls_verify_ocsp_ಸ್ಥಿತಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
  ಪೂರ್ಣಾಂಕ rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_ಸ್ಥಿತಿ_t ಸ್ಥಿತಿ_2 = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_ಸ್ಥಿತಿ_request_get(session, &ocsp_raw);
  ಆದರೆ(rc == GNUTLS_E_REQUESTED_ದತ್ತಾಂಶ_NOT_ಲಭ್ಯ) {
    failf(ದತ್ತಾಂಶ, "No OCSP response received");
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rc < 0) {
    failf(ದತ್ತಾಂಶ, "Invalid OCSP response received");
    ಹೋಗು ಹೊರಹೋಗು;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  ಆದರೆ(rc < 0) {
    failf(ದತ್ತಾಂಶ, "failed to init OCSP response parsing");
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  ಆದರೆ(rc < 0) {
    failf(ದತ್ತಾಂಶ, "failed to parse OCSP response");
    ಹೋಗು ಹೊರಹೋಗು;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  ಆದರೆ(gnutls_ocsp_resp_get_ಸ್ಥಿತಿ(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(ದತ್ತಾಂಶ, "OCSP response basic part is not successful");
    ಹೋಗು ಹೊರಹೋಗು;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  ಗಾಗಿ(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    ಆದರೆ(!rc || (rc == GNUTLS_E_REQUESTED_ದತ್ತಾಂಶ_NOT_ಲಭ್ಯ))
      ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ(rc < 0) {
    failf(ದತ್ತಾಂಶ, "OCSP does not carry status for peer cert");
    ಹೋಗು ಹೊರಹೋಗು;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
                                   &ಸ್ಥಿತಿ_2, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
  ಆದರೆ(rc < 0) {
    failf(ದತ್ತಾಂಶ, "failed to parse OCSP response for peer cert");
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆಯ್ಕೆ(ಸ್ಥಿತಿ_2) {
  ಸ್ಥಿತಿ GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    ಫಲಿತಾಂಶ = CURLE_OK;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ GNUTLS_OCSP_CERT_REVOKED:
    failf(ದತ್ತಾಂಶ, "peer certificate is revoked");
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "peer certificate OCSP status is not good: %d", (ಪೂರ್ಣಾಂಕ)ಸ್ಥಿತಿ_2);
    ನಿಲ್ಲಿಸು;
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ gtls_cert_chain {
  ಸ್ಥಿರಾಂಕ gnutls_datum_t *certs;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಅಂಕೆ_certs;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
ಸ್ಥಿರ CURLcode gtls_chain_get_der(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಶೂನ್ಯ *user_ದತ್ತಾಂಶ,
                                   size_t i,
                                   ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **pder,
                                   size_t *pder_len)
{
  ರಚನೆ gtls_cert_chain *chain = user_ದತ್ತಾಂಶ;

  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  *pder_len = 0;
  *pder = ಖಾಲಿ;

  ಆದರೆ(i >= chain->ಅಂಕೆ_certs)
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  *pder = chain->certs[i].ದತ್ತಾಂಶ;
  *pder_len = (size_t)chain->certs[i].ಗಾತ್ರ_2;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ಸ್ಥಿರ CURLcode gtls_verify_cert(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ ssl_filter_config *config,
                                 ರಚನೆ ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ ssl_peer *peer,
                                 ರಚನೆ gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ verify_ಸ್ಥಿತಿ = 0;
  ದೀರ್ಘ * ಸ್ಥಿರಾಂಕ certverifyresult = &ssl_config->certverifyresult;
  ಪೂರ್ಣಾಂಕ rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_ಸ್ಥಿತಿ);
  ಆದರೆ(rc < 0) {
    failf(ದತ್ತಾಂಶ, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_ಸ್ಥಿತಿ;
  verified = !(verify_ಸ್ಥಿತಿ & GNUTLS_CERT_INVALID);
  ಆದರೆ(verified)
    infof(ದತ್ತಾಂಶ, "  SSL certificate verified by GnuTLS");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
  ಆದರೆ(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode ಫಲಿತಾಂಶ;

    ಆದರೆ(gnutls_ocsp_ಸ್ಥಿತಿ_request_get(session, &ocsp_raw)) {
      ocsp_raw.ದತ್ತಾಂಶ = ಖಾಲಿ;
      ocsp_raw.ಗಾತ್ರ_2 = 0;
    }
    ಆದರೆ(config->verifystatus && !ocsp_raw.ಗಾತ್ರ_2) {
      failf(ದತ್ತಾಂಶ, "No OCSP response received");
      ಹಿಂತಿರುಗಿ CURLE_SSL_INVALIDCERTSTATUS;
    }
    ಫಲಿತಾಂಶ = Curl_vtls_apple_verify(cf, ದತ್ತಾಂಶ, peer, chain->ಅಂಕೆ_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.ದತ್ತಾಂಶ, ocsp_raw.ಗಾತ್ರ_2,
                                    config->verifystatus);
    ಆದರೆ(ಫಲಿತಾಂಶ && (ಫಲಿತಾಂಶ != CURLE_PEER_FAILED_VERIFICATION))
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; /* no further failure analysis needed */
    verified = !ಫಲಿತಾಂಶ;
    ಆದರೆ(verified) {
      infof(ದತ್ತಾಂಶ, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)cf;
  (ಶೂನ್ಯ)peer;
  (ಶೂನ್ಯ)chain;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!verified) {
    CURLcode ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cause = "certificate error, no details available";
    ಆದರೆ(verify_ಸ್ಥಿತಿ & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(verify_ಸ್ಥಿತಿ & GNUTLS_CERT_SIGNER_NOT_ಸಿಕ್ಕಿತು)
      cause = "certificate signer not trusted";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(verify_ಸ್ಥಿತಿ & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(verify_ಸ್ಥಿತಿ & GNUTLS_CERT_INVALID_OCSP_ಸ್ಥಿತಿ) {
      cause = "OCSP status response is invalid";
      ಫಲಿತಾಂಶ = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(ದತ್ತಾಂಶ, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                gnutls_session_t session,
                                ರಚನೆ ssl_filter_config *config,
                                ರಚನೆ ssl_easy_config *ssl_config,
                                ರಚನೆ ssl_peer *peer,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinned_ಕೀಲಿ)
{
  ರಚನೆ gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = ಖಾಲಿ, x509_issuer = ಖಾಲಿ;
  time_t certclock;
  bool ocsp_checked = FALSE;
  ಪೂರ್ಣಾಂಕ rc;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ದೀರ್ಘ * ಸ್ಥಿರಾಂಕ certverifyresult = &ssl_config->certverifyresult;

  (ಶೂನ್ಯ)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.ಅಂಕೆ_certs);
  ಆದರೆ(!chain.certs) {
    ಆದರೆ(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(ದತ್ತಾಂಶ, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_ಸಿಕ್ಕಿತು;
      ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    infof(ದತ್ತಾಂಶ, " common name: WARNING could not obtain");
  }

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo && chain.certs) {
    ಆದರೆ(chain.ಅಂಕೆ_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(ದತ್ತಾಂಶ, "%u certificates is more than allowed (%d)",
            chain.ಅಂಕೆ_certs, MAX_ALLOWED_CERT_AMOUNT);
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;

      ಫಲಿತಾಂಶ = Curl_ssl_init_certinfo(ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)chain.ಅಂಕೆ_certs);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;

      ಗಾಗಿ(i = 0; i < chain.ಅಂಕೆ_certs; i++) {
        ಸ್ಥಿರಾಂಕ uint8_t *beg = chain.certs[i].ದತ್ತಾಂಶ;
        ಸ್ಥಿರಾಂಕ uint8_t *ಕೊನೆ = beg + chain.certs[i].ಗಾತ್ರ_2;

        ಫಲಿತಾಂಶ = Curl_ಹೊರತೆಗೆ_certinfo(ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)i, beg, ಕೊನೆ);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
      }
    }
  }

  ಆದರೆ(config->verifypeer) {
    ಫಲಿತಾಂಶ = gtls_verify_cert(ದತ್ತಾಂಶ, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    infof(ದತ್ತಾಂಶ, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  ಆದರೆ(gnutls_x509_crt_init(&x509_cert)) {
    failf(ದತ್ತಾಂಶ, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_ಸಿಕ್ಕಿತು;
    ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    ಆದರೆ(rc) {
      failf(ದತ್ತಾಂಶ, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_ಸಿಕ್ಕಿತು;
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_ಸಮಯ(x509_cert);

  ಆದರೆ(certclock == (time_t)-1) {
    ಆದರೆ(config->verifypeer) {
      failf(ದತ್ತಾಂಶ, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "  SSL certificate expiration date verify FAILED");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(certclock < ಸಮಯ(ಖಾಲಿ)) {
      ಆದರೆ(config->verifypeer) {
        failf(ದತ್ತಾಂಶ, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        infof(ದತ್ತಾಂಶ, "  SSL certificate expiration date FAILED");
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_ಸಮಯ(x509_cert);

  ಆದರೆ(certclock == (time_t)-1) {
    ಆದರೆ(config->verifypeer) {
      failf(ದತ್ತಾಂಶ, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "  SSL certificate activation date verify FAILED");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(certclock > ಸಮಯ(ಖಾಲಿ)) {
      ಆದರೆ(config->verifypeer) {
        failf(ದತ್ತಾಂಶ, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        ಫಲಿತಾಂಶ = CURLE_PEER_FAILED_VERIFICATION;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        infof(ದತ್ತಾಂಶ, "  SSL certificate activation date FAILED");
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "  SSL certificate activation date OK");
  }

  ಆದರೆ(config->verifystatus && !ocsp_checked) {
    ಫಲಿತಾಂಶ = gtls_verify_ocsp_ಸ್ಥಿತಿ(ದತ್ತಾಂಶ, session, x509_cert);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    infof(ದತ್ತಾಂಶ, "  SSL certificate status verification SKIPPED");

  ಆದರೆ(config->issuercert) {
    gnutls_datum_t issuerp;
    ಆದರೆ(gnutls_x509_crt_init(&x509_issuer)) {
      failf(ದತ್ತಾಂಶ, "failed to init GnuTLS x509_crt for issuer");
      ಫಲಿತಾಂಶ = CURLE_SSL_ISSUER_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    issuerp = ತುಂಬು_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    ಆದರೆ(rc) {
      failf(ದತ್ತಾಂಶ, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      ಫಲಿತಾಂಶ = CURLE_SSL_ISSUER_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    rc = (ಪೂರ್ಣಾಂಕ)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    ಆದರೆ(rc <= 0) {
      failf(ದತ್ತಾಂಶ, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      ಫಲಿತಾಂಶ = CURLE_SSL_ISSUER_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    infof(ದತ್ತಾಂಶ, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  rc = (ಪೂರ್ಣಾಂಕ)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  ಫಲಿತಾಂಶ = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_ಸಂದೇಶ_verify_ಫಲಿತಾಂಶ(ದತ್ತಾಂಶ, peer, x509_cert, rc, config->verifyhost);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(pinned_ಕೀಲಿ) {
    ಫಲಿತಾಂಶ = pkp_pin_peer_pubkey(ದತ್ತಾಂಶ, x509_cert, pinned_ಕೀಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_OK) {
      failf(ದತ್ತಾಂಶ, "SSL: public key does not match pinned public key");
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  gtls_infof_cert(ದತ್ತಾಂಶ, x509_cert);

ಹೊರಹೋಗು:
  ಆದರೆ(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  ಆದರೆ(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode gtls_verifyserver(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  gnutls_session_t session)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ದತ್ತಾಂಶ);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinned_ಕೀಲಿ = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY);
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinned_ಕೀಲಿ = CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY);
#ಆದರೆ_ಅಂತ್ಯ
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_gtls_verifyserver(cf, ದತ್ತಾಂಶ, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_ಕೀಲಿ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  ಆದರೆ(gnutls_protocol_get_ಆವೃತ್ತಿ(session) < GNUTLS_TLS1_3)
    ಫಲಿತಾಂಶ = cf_gtls_update_session_id(cf, ದತ್ತಾಂಶ, session);
#ಆದರೆ_ಅಂತ್ಯ

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
ಸ್ಥಿರ CURLcode gtls_send_earlydata(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_ಫಲಿತಾಂಶ = CURLE_OK;
  ಆಗ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_ದಾಖಲೆ_send_early_ದತ್ತಾಂಶ(backend->gtls.session, buf, blen);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    ಆದರೆ(n < 0) {
      ಆದರೆ(n == GNUTLS_E_AGAIN)
        ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = backend->gtls.io_ಫಲಿತಾಂಶ ?
                 backend->gtls.io_ಫಲಿತಾಂಶ : CURLE_SEND_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(ದತ್ತಾಂಶ, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * This function is called after the TCP connect has completed. Setup the TLS
 * layer and do all necessary magic.
 */
/* We use connssl->connecting_state to keep track of the connection status;
   there are three states: 'ssl_connect_1' (not started yet or complete),
   'ssl_connect_2' (doing handshake with the server), and
   'ssl_connect_3' (verifying and getting stats).
 */
ಸ್ಥಿರ CURLcode gtls_connect_common(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  ಆದರೆ(ssl_connection_complete == connssl->state) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;

  /* Initiate the connection, if not already done */
  ಆದರೆ(connssl->connecting_state == ssl_connect_1) {
    ಫಲಿತಾಂಶ = gtls_connect_step1(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    connssl->connecting_state = ssl_connect_2;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_2) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
    ಆದರೆ(connssl->earlydata_state == ssl_earlydata_await) {
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(connssl->earlydata_state == ssl_earlydata_sending) {
      ಫಲಿತಾಂಶ = gtls_send_earlydata(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      connssl->earlydata_state = ssl_earlydata_ಕಳುಹಿಸಿದ;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_ಕಳುಹಿಸಿದ));
#ಆದರೆ_ಅಂತ್ಯ
    ಫಲಿತಾಂಶ = cf_gtls_handshake(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  ಆದರೆ(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    ಪೂರ್ಣಾಂಕ rc;

    Curl_gtls_report_handshake(ದತ್ತಾಂಶ, &backend->gtls);

    ಫಲಿತಾಂಶ = gtls_verifyserver(cf, ದತ್ತಾಂಶ, backend->gtls.session);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    ಆದರೆ(rc) {  /* No ALPN from server */
      proto.ದತ್ತಾಂಶ = ಖಾಲಿ;
      proto.ಗಾತ್ರ_2 = 0;
    }

    ಫಲಿತಾಂಶ = Curl_alpn_ಗಣ_negotiated(cf, ದತ್ತಾಂಶ, connssl,
                                      proto.ದತ್ತಾಂಶ, proto.ಗಾತ್ರ_2);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
    ಆದರೆ(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_ಕಳುಹಿಸಿದ);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_ದತ್ತಾಂಶ) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#ಆದರೆ_ಅಂತ್ಯ
    connssl->connecting_state = ssl_connect_ಮುಗಿದ;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_ಮುಗಿದ)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    *ಮುಗಿದ = FALSE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  *ಮುಗಿದ = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "gtls_connect_common() -> %d, done=%d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ,
              *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode gtls_connect(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             bool *ಮುಗಿದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_GNUTLS_EARLY_ದತ್ತಾಂಶ
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ಆದರೆ((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ gtls_connect_common(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
}

ಸ್ಥಿರ bool gtls_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                              ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *ctx = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)ctx->backend;
  ಆದರೆ(backend->gtls.session &&
     gnutls_ದಾಖಲೆ_check_pending(backend->gtls.session) != 0)
    ಹಿಂತಿರುಗಿ TRUE;
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLcode gtls_send(ರಚನೆ Curl_cfilter *cf,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  ಆಗ(remain) {
    backend->gtls.io_ಫಲಿತಾಂಶ = CURLE_OK;
    nwritten = gnutls_ದಾಖಲೆ_send(backend->gtls.session, buf, remain);

    ಆದರೆ(nwritten >= 0) {
      ಆದರೆ((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(ದತ್ತಾಂಶ, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (ಅಕ್ಷರ *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        ಫಲಿತಾಂಶ = CURLE_OK;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಫಲಿತಾಂಶ = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_ಫಲಿತಾಂಶ ? backend->gtls.io_ಫಲಿತಾಂಶ : CURLE_SEND_ERROR);
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ಸ್ಥಿರ CURLcode gtls_shutdown(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              bool send_shutdown, bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ಅಕ್ಷರ buf[1024];
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  ಆದರೆ(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *ಮುಗಿದ = TRUE;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *ಮುಗಿದ = FALSE;

  ಆದರೆ(!backend->gtls.ಕಳುಹಿಸಿದ_shutdown) {
    /* do this only once */
    backend->gtls.ಕಳುಹಿಸಿದ_shutdown = TRUE;
    ಆದರೆ(send_shutdown) {
      ಪೂರ್ಣಾಂಕ ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      ಆದರೆ((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_ದಾಖಲೆ_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.ಕಳುಹಿಸಿದ_shutdown = FALSE;
        ಫಲಿತಾಂಶ = CURLE_OK;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((ಪೂರ್ಣಾಂಕ)ret), (ಪೂರ್ಣಾಂಕ)ret);
        ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ಗಾಗಿ(i = 0; i < 10; ++i) {
    nread = gnutls_ದಾಖಲೆ_recv(backend->gtls.session, buf, ಗಾತ್ರ(buf));
    ಆದರೆ(nread <= 0)
      ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ(nread > 0) {
    /* still data coming in? */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == 0) {
    /* We got the close notify alert and are done. */
    *ಮುಗಿದ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_ದಾಖಲೆ_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((ಪೂರ್ಣಾಂಕ)nread), (ಪೂರ್ಣಾಂಕ)nread);
    ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
  }

ಹೊರಹೋಗು:
  cf->shutdown = (ಫಲಿತಾಂಶ || *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ gtls_close(ರಚನೆ Curl_cfilter *cf,
                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "close");
  ಆದರೆ(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = ಖಾಲಿ;
  }
  ಆದರೆ(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ಸ್ಥಿರ CURLcode gtls_recv(ರಚನೆ Curl_cfilter *cf,
                          ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಅಕ್ಷರ *buf, size_t blen,
                          size_t *pnread)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_ದಾಖಲೆ_recv(backend->gtls.session, buf, blen);

  ಆದರೆ(nread >= 0)
    *pnread = (size_t)nread;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      ಫಲಿತಾಂಶ = cf_gtls_handshake(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಫಲಿತಾಂಶ = CURLE_AGAIN; /* make us get called again. */
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "GnuTLS recv error (%d): %s",
            (ಪೂರ್ಣಾಂಕ)nread, gnutls_strerror((ಪೂರ್ಣಾಂಕ)nread));
      ಫಲಿತಾಂಶ = backend->gtls.io_ಫಲಿತಾಂಶ ?
        backend->gtls.io_ಫಲಿತಾಂಶ : CURLE_RECV_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nread);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

size_t Curl_gtls_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2)
{
  ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, "GnuTLS/%s", gnutls_check_ಆವೃತ್ತಿ(ಖಾಲಿ));
}

/* data might be NULL! */
ಸ್ಥಿರ CURLcode gtls_random(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *entropy, size_t ಉದ್ದಳತೆ)
{
  ಪೂರ್ಣಾಂಕ rc;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, ಉದ್ದಳತೆ);
  ಹಿಂತಿರುಗಿ rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ಸ್ಥಿರ CURLcode gtls_sha256sum(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು,
                               size_t len,
                               ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha256sum, /* output */
                               size_t sha256len)
{
  ರಚನೆ sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  ಮಾಡು {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ilen = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, ಒಳಹರಿವು);
    len -= ilen;
    ಒಳಹರಿವು += ilen;
  } ಆಗ(len);
#ಆದರೆ NETTLE_ಆವೃತ್ತಿ_MAJOR >= 4
  (ಶೂನ್ಯ)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#ಇಲ್ಲದಿದ್ದರೆ
  sha256_digest(&SHA256pw, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)sha256len, sha256sum);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool gtls_cert_ಸ್ಥಿತಿ_request(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ ಶೂನ್ಯ *gtls_get_internals(ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl,
                                CURLINFO info)
{
  ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  (ಶೂನ್ಯ)info;
  DEBUGASSERT(backend);
  ಹಿಂತಿರುಗಿ backend->gtls.session;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_gnutls = {
  { CURLSSLBACKEND_GNUTLS, "gnutls" }, /* info */

  SSLSUPP_CA_ಮಾರ್ಗ  |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_CA_CACHE |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_CRLFILE,

  ಗಾತ್ರ(ರಚನೆ gtls_ssl_backend_ದತ್ತಾಂಶ),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_ಆವೃತ್ತಿ,             /*ಆವೃತ್ತಿ*/
  gtls_shutdown,                 /* shutdown */
  gtls_ದತ್ತಾಂಶ_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_ಸ್ಥಿತಿ_request,      /* cert_status_request */
  gtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,       /* adjust_pollset */
  gtls_get_internals,            /* get_internals */
  gtls_close,                    /* close_one */
  ಖಾಲಿ,                          /* close_all */
  ಖಾಲಿ,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  ಖಾಲಿ,                          /* get_channel_binding */
};

#ಆದರೆ_ಅಂತ್ಯ /* USE_GNUTLS */
