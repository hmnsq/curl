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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GNUTLS

#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/abstract.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/gnutls.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/x509.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/crypto.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/sha2.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nettle/version.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/keylog.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/gtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/apple.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vauth/vauth.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h" /* for the connect timeout */
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#ਜੇ 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GTLSDEBUG
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GTLSDEBUG
ਸਥਿਰ ਖਾਲੀ tls_log_func(ਪੂਰਨ level, ਅਚਲ ਅੱਖਰ *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#ਜੇ_ਅੰਤ

#ਜੇ !defined(GNUTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ) || (GNUTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x030605)
#ਗਲਤੀ "too old GnuTLS version"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
#ਜੇ GNUTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x030702
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <gnutls/ocsp.h>

ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ {
  ਬਣਤਰ gtls_ctx gtls;
};

ਸਥਿਰ ssize_t gtls_ਧੱਕੋ(ਖਾਲੀ *s, ਅਚਲ ਖਾਲੀ *buf, size_t blen)
{
  ਬਣਤਰ Curl_cfilter *cf = s;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (ਪੂਰਨ)ਨਤੀਜਾ, nwritten);
  backend->gtls.io_ਨਤੀਜਾ = ਨਤੀਜਾ;
  ਜੇ(ਨਤੀਜਾ) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_ਸੈੱਟ_errno(backend->gtls.session,
                               (ਨਤੀਜਾ == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ਵਾਪਸ -1;
  }
  ਵਾਪਸ (ssize_t)nwritten;
}

ਸਥਿਰ ssize_t gtls_pull(ਖਾਲੀ *s, ਖਾਲੀ *buf, size_t blen)
{
  ਬਣਤਰ Curl_cfilter *cf = s;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  size_t nread;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(!backend->gtls.shared_creds->trust_ਤਿਆਰੀ) {
    ਨਤੀਜਾ = Curl_gtls_client_trust_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, &backend->gtls);
    ਜੇ(ਨਤੀਜਾ) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_ਸੈੱਟ_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_ਨਤੀਜਾ = ਨਤੀਜਾ;
      ਵਾਪਸ -1;
    }
  }

  ਨਤੀਜਾ = Curl_conn_cf_recv(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, blen, &nread);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (ਪੂਰਨ)ਨਤੀਜਾ,
              nread);
  backend->gtls.io_ਨਤੀਜਾ = ਨਤੀਜਾ;
  ਜੇ(ਨਤੀਜਾ) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_ਸੈੱਟ_errno(backend->gtls.session,
                               (ਨਤੀਜਾ == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ਵਾਪਸ -1;
  }
  ਨਹੀਂਤਾਂ ਜੇ(nread == 0)
    connssl->peer_closed = TRUE;
  ਵਾਪਸ (ssize_t)nread;
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
ਸਥਿਰ ਪੂਰਨ gtls_init(ਖਾਲੀ)
{
  ਪੂਰਨ ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GTLSDEBUG
  gnutls_global_ਸੈੱਟ_log_function(tls_log_func);
  gnutls_global_ਸੈੱਟ_log_level(2);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ ret;
}

ਸਥਿਰ ਖਾਲੀ gtls_cleanup(ਖਾਲੀ)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
ਸਥਿਰ ਖਾਲੀ showtime(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਪਾਠ_3, time_t stamp)
{
  ਬਣਤਰ tm ਬਫਰ;
  ਅਚਲ ਬਣਤਰ tm *tm = &ਬਫਰ;
  ਅੱਖਰ str[96];
  CURLcode ਨਤੀਜਾ = curlx_gmtime(stamp, &ਬਫਰ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ;

  curl_msnprintf(str,
                 ਆਕਾਰ(str),
                 "  %s: %s, %02d %s %4d %02d:%02d:%02d GMT",
                 ਪਾਠ_3,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);
  infof(ਅੰਕੜੇ, "%s", str);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ gnutls_datum_t ਲੱਦੋ_file(ਅਚਲ ਅੱਖਰ *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { ਨਿਰਾ, 0 };
  ਲੰਮਾ filelen;
  ਖਾਲੀ *ptr;

  f = curlx_fopen(file, "rb");
  ਜੇ(!f)
    ਵਾਪਸ loaded_file;
  ਜੇ(fseek(f, 0, SEEK_END))
    ਜਾਓ ਬਾਹਰਲਾ;
  filelen = ftell(f);
  ਜੇ(filelen < 0 || filelen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(fseek(f, 0, SEEK_SET))
    ਜਾਓ ਬਾਹਰਲਾ;
  ptr = curlx_malloc((size_t)filelen);
  ਜੇ(!ptr)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  loaded_file.ਅੰਕੜੇ = ptr;
  loaded_file.ਮਾਪ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)filelen;
ਬਾਹਰਲਾ:
  curlx_fclose(f);
  ਵਾਪਸ loaded_file;
}

ਸਥਿਰ ਖਾਲੀ unload_file(gnutls_datum_t ਅੰਕੜੇ)
{
  curlx_free(ਅੰਕੜੇ.ਅੰਕੜੇ);
}

/* this function does an SSL/TLS (re-)handshake */
ਸਥਿਰ CURLcode cf_gtls_handshake(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  gnutls_session_t session;
  ਪੂਰਨ rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_ਨਤੀਜਾ = CURLE_OK;
  rc = gnutls_handshake(session);

  ਜੇ(!backend->gtls.shared_creds->trust_ਤਿਆਰੀ) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode ਨਤੀਜਾ = Curl_gtls_client_trust_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, &backend->gtls);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_ਰਿਕਾਰਡ_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    ਵਾਪਸ CURLE_AGAIN;
  }
  ਨਹੀਂਤਾਂ ਜੇ((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
    ਅਚਲ ਅੱਖਰ *strerr = ਨਿਰਾ;

    ਜੇ(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ਜੇ(!strerr)
      strerr = gnutls_strerror(rc);

    infof(ਅੰਕੜੇ, "gnutls_handshake() warning: %s", strerr);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ CURLE_AGAIN;
  }
  ਨਹੀਂਤਾਂ ਜੇ((rc < 0) && backend->gtls.io_ਨਤੀਜਾ) {
    ਵਾਪਸ backend->gtls.io_ਨਤੀਜਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rc < 0) {
    ਅਚਲ ਅੱਖਰ *strerr = ਨਿਰਾ;

    ਜੇ(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ਜੇ(!strerr)
      strerr = gnutls_strerror(rc);

    failf(ਅੰਕੜੇ, "GnuTLS, handshake failed: %s", strerr);
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ gnutls_x509_crt_fmt_t gnutls_do_file_type(ਅਚਲ ਅੱਖਰ *type)
{
  ਜੇ(!type || !type[0])
    ਵਾਪਸ GNUTLS_X509_FMT_PEM;
  ਜੇ(curl_strequal(type, "PEM"))
    ਵਾਪਸ GNUTLS_X509_FMT_PEM;
  ਜੇ(curl_strequal(type, "DER"))
    ਵਾਪਸ GNUTLS_X509_FMT_DER;
  ਵਾਪਸ GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ QUIC_ਤਰਜੀਹ \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ਸਥਿਰ CURLcode gnutls_ਸੈੱਟ_ssl_ਸੰਸਕਰਣ_min_max(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ ssl_peer *peer,
  ਬਣਤਰ ssl_filter_config *conn_config,
  ਅਚਲ ਅੱਖਰ **prioritylist,
  bool tls13support)
{
  ਲੰਮਾ ssl_ਸੰਸਕਰਣ = conn_config->ਸੰਸਕਰਣ_2;
  ਲੰਮਾ ssl_ਸੰਸਕਰਣ_max = conn_config->ਸੰਸਕਰਣ_max;

  DEBUGASSERT(ssl_ਸੰਸਕਰਣ != CURL_SSLVERSION_DEFAULT);
  ਜੇ(ssl_ਸੰਸਕਰਣ <= CURL_SSLVERSION_TLSv1)
    ssl_ਸੰਸਕਰਣ = CURL_SSLVERSION_TLSv1_0;
  ਜੇ((ssl_ਸੰਸਕਰਣ_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_ਸੰਸਕਰਣ_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_ਸੰਸਕਰਣ_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  ਜੇ(peer->transport == TRNSPRT_QUIC) {
    ਜੇ(ssl_ਸੰਸਕਰਣ_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(ਅੰਕੜੇ, "QUIC needs at least TLS version 1.3");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_ਤਰਜੀਹ;
    ਵਾਪਸ CURLE_OK;
  }

  ਚੋਣ(ssl_ਸੰਸਕਰਣ | ssl_ਸੰਸਕਰਣ_max) {
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    ਵਾਪਸ CURLE_OK;
  }

  failf(ਅੰਕੜੇ, "GnuTLS: cannot set TLS protocol");
  ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       ਬਣਤਰ gtls_shared_creds **pcreds)
{
  ਬਣਤਰ gtls_shared_creds *shared;
  ਪੂਰਨ rc;

  *pcreds = ਨਿਰਾ;
  shared = curlx_calloc(1, ਆਕਾਰ(*shared));
  ਜੇ(!shared)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  ਜੇ(rc != GNUTLS_E_SUCCESS) {
    failf(ਅੰਕੜੇ, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->ਸਮਾਂ = *Curl_pgrs_now(ਅੰਕੜੇ);
  *pcreds = shared;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_ਉੱਪਰ_ref(ਬਣਤਰ gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  ਜੇ(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    ਵਾਪਸ CURLE_OK;
  }
  ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
}

ਖਾਲੀ Curl_gtls_shared_creds_free(ਬਣਤਰ gtls_shared_creds **pcreds)
{
  ਬਣਤਰ gtls_shared_creds *shared = *pcreds;
  *pcreds = ਨਿਰਾ;
  ਜੇ(shared) {
    --shared->refcount;
    ਜੇ(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

ਸਥਿਰ CURLcode gtls_populate_creds(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    gnutls_certificate_credentials_t creds)
{
  ਬਣਤਰ ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ਅੰਕੜੇ);
  bool creds_are_ਖਾਲੀ_ਥਾਵਾਂ = TRUE;
  ਪੂਰਨ rc;

  ਜੇ(!config->verifypeer) {
    infof(ਅੰਕੜੇ, "SSL Trust: peer verification disabled");
    ਵਾਪਸ CURLE_OK;
  }

  infof(ਅੰਕੜੇ, "SSL Trust Anchors:");
  ਜੇ(config->native_ca_store) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
    infof(ਅੰਕੜੇ, "  Native: Apple SecTrust");
    creds_are_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
#ਨਹੀਂਤਾਂ
    rc = gnutls_certificate_ਸੈੱਟ_x509_system_trust(creds);
    ਜੇ(rc < 0)
      infof(ਅੰਕੜੇ, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, "  Native: %d certificates from system trust", rc);
      ਜੇ(rc > 0)
        creds_are_ਖਾਲੀ_ਥਾਵਾਂ = FALSE;
    }
#ਜੇ_ਅੰਤ
  }

  ਜੇ(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    ਜੇ(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(ਅੰਕੜੇ, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.ਅੰਕੜੇ = config->ca_info_blob->ਅੰਕੜੇ;
    ca_info_datum.ਮਾਪ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)config->ca_info_blob->len;
    rc = gnutls_certificate_ਸੈੱਟ_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_ਖਾਲੀ_ਥਾਵਾਂ = creds_are_ਖਾਲੀ_ਥਾਵਾਂ && (rc <= 0);
    ਜੇ(rc < 0) {
      infof(ਅੰਕੜੇ, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_ਖਾਲੀ_ਥਾਵਾਂ ? "" : ", continuing anyway"));
      ਜੇ(creds_are_ਖਾਲੀ_ਥਾਵਾਂ) {
        ssl_config->certverifyresult = rc;
        ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  ਨਹੀਂਤਾਂ ਜੇ(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_ਸੈੱਟ_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_ਸੈੱਟ_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_ਖਾਲੀ_ਥਾਵਾਂ = creds_are_ਖਾਲੀ_ਥਾਵਾਂ && (rc <= 0);
    ਜੇ(rc < 0) {
      infof(ਅੰਕੜੇ, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_ਖਾਲੀ_ਥਾਵਾਂ ? "" : ", continuing anyway"));
      ਜੇ(creds_are_ਖਾਲੀ_ਥਾਵਾਂ) {
        ssl_config->certverifyresult = rc;
        ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  ਜੇ(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_ਸੈੱਟ_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_ਖਾਲੀ_ਥਾਵਾਂ = creds_are_ਖਾਲੀ_ਥਾਵਾਂ && (rc <= 0);
    ਜੇ(rc < 0) {
      infof(ਅੰਕੜੇ, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_ਖਾਲੀ_ਥਾਵਾਂ ? "" : ", continuing anyway"));
      ਜੇ(creds_are_ਖਾਲੀ_ਥਾਵਾਂ) {
        ssl_config->certverifyresult = rc;
        ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  ਜੇ(creds_are_ਖਾਲੀ_ਥਾਵਾਂ)
    infof(ਅੰਕੜੇ, "  no trust anchors configured");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
  ਜੇ(config->CRLfile && config->native_ca_store) {
    failf(ਅੰਕੜੇ, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  }
#ਜੇ_ਅੰਤ
  ਜੇ(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_ਸੈੱਟ_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    ਜੇ(rc < 0) {
      failf(ਅੰਕੜੇ, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      ਵਾਪਸ CURLE_SSL_CRL_BADFILE;
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  ਵਾਪਸ CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPROTO_GTLS_X509_ਕੁੰਜੀ   "tls:gtls:x509:share"

ਸਥਿਰ bool gtls_shared_creds_expired(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਅਚਲ ਬਣਤਰ gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(ਅੰਕੜੇ), &sc->ਸਮਾਂ);
  timediff_t timeout_ms = ਅੰਕੜੇ->ਸੈੱਟ.ssl_ca_cache_timeout * (timediff_t)1000;

  ਜੇ(timeout_ms < 0)
    ਵਾਪਸ FALSE;

  ਵਾਪਸ elapsed_ms >= timeout_ms;
}

ਸਥਿਰ bool gtls_shared_creds_different(ਬਣਤਰ Curl_cfilter *cf,
                                        ਅਚਲ ਬਣਤਰ gtls_shared_creds *sc)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਜੇ(!sc->CAfile || !conn_config->CAfile)
    ਵਾਪਸ sc->CAfile != conn_config->CAfile;

  ਵਾਪਸ strcmp(sc->CAfile, conn_config->CAfile);
}

ਸਥਿਰ ਬਣਤਰ gtls_shared_creds *gtls_get_cached_creds(ਬਣਤਰ Curl_cfilter *cf,
                                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ gtls_shared_creds *shared_creds;

  ਜੇ(ਅੰਕੜੇ->multi) {
    shared_creds = Curl_hash_pick(&ਅੰਕੜੇ->multi->proto_hash,
                                  MPROTO_GTLS_X509_ਕੁੰਜੀ,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_ਕੁੰਜੀ));
    ਜੇ(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(ਅੰਕੜੇ, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      ਵਾਪਸ shared_creds;
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਖਾਲੀ gtls_shared_creds_hash_free(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ,
                                        size_t ਕੁੰਜੀ_len, ਖਾਲੀ *p)
{
  ਬਣਤਰ gtls_shared_creds *sc = p;
  DEBUGASSERT(ਕੁੰਜੀ_len == CURL_CSTRLEN(MPROTO_GTLS_X509_ਕੁੰਜੀ));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_ਕੁੰਜੀ, ਕੁੰਜੀ, ਕੁੰਜੀ_len));
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)ਕੁੰਜੀ_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

ਸਥਿਰ ਖਾਲੀ gtls_ਸੈੱਟ_cached_creds(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ gtls_shared_creds *sc)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  ਜੇ(!ਅੰਕੜੇ->multi)
    ਵਾਪਸ;

  ਜੇ(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    ਜੇ(!sc->CAfile)
      ਵਾਪਸ;
  }

  ਜੇ(Curl_gtls_shared_creds_ਉੱਪਰ_ref(sc))
    ਵਾਪਸ;

  ਜੇ(!Curl_hash_add2(&ਅੰਕੜੇ->multi->proto_hash,
                     MPROTO_GTLS_X509_ਕੁੰਜੀ,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_ਕੁੰਜੀ),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    ਵਾਪਸ;
  }
}

CURLcode Curl_gtls_client_trust_ਤਿਆਰੀ(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ gtls_ctx *gtls)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ gtls_shared_creds *cached_creds = ਨਿਰਾ;
  bool cache_criteria_met;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (ਅੰਕੜੇ->ਸੈੱਟ.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  ਜੇ(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, ਅੰਕੜੇ);

  ਜੇ(cached_creds && !Curl_gtls_shared_creds_ਉੱਪਰ_ref(cached_creds)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_ਸੈੱਟ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    ਜੇ(rc != GNUTLS_E_SUCCESS) {
      failf(ਅੰਕੜੇ, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "loading trust anchors and CRLs");
    ਨਤੀਜਾ = gtls_populate_creds(cf, ਅੰਕੜੇ, gtls->shared_creds->creds);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    gtls->shared_creds->trust_ਤਿਆਰੀ = TRUE;
    ਜੇ(cache_criteria_met)
      gtls_ਸੈੱਟ_cached_creds(cf, ਅੰਕੜੇ, gtls->shared_creds);
  }
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
ਸਥਿਰ ਪੂਰਨ gtls_get_ietf_proto(gnutls_session_t session)
{
  ਚੋਣ(gnutls_protocol_get_ਸੰਸਕਰਣ(session)) {
  ਹਾਲਤ GNUTLS_SSL3:
    ਵਾਪਸ CURL_IETF_PROTO_SSL3;
  ਹਾਲਤ GNUTLS_TLS1_0:
    ਵਾਪਸ CURL_IETF_PROTO_TLS1;
  ਹਾਲਤ GNUTLS_TLS1_1:
    ਵਾਪਸ CURL_IETF_PROTO_TLS1_1;
  ਹਾਲਤ GNUTLS_TLS1_2:
    ਵਾਪਸ CURL_IETF_PROTO_TLS1_2;
  ਹਾਲਤ GNUTLS_TLS1_3:
    ਵਾਪਸ CURL_IETF_PROTO_TLS1_3;
  ਮੂਲ:
    ਵਾਪਸ CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ਅਚਲ ਅੱਖਰ *alpn,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *quic_tp,
                                 size_t quic_tp_len,
                                 ਬਣਤਰ Curl_ssl_session **psession)
{
  ਬਣਤਰ Curl_ssl_session *sc_session = ਨਿਰਾ, *sc_dup = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sdata, *qtp_clone = ਨਿਰਾ;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(psession)
    *psession = ਨਿਰਾ;
  ਜੇ(!Curl_ssl_scache_use(cf, ਅੰਕੜੇ))
    ਵਾਪਸ CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_ਅੰਕੜੇ(session, ਨਿਰਾ, &sdata_len);
  ਜੇ(!sdata_len) /* GnuTLS does this for some version combinations */
    ਵਾਪਸ CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  ਜੇ(!sdata)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_ਅੰਕੜੇ(session, sdata, &sdata_len);
  earlydata_max = gnutls_ਰਿਕਾਰਡ_get_max_early_ਅੰਕੜੇ_ਮਾਪ(session);

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  ਜੇ(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ਅਚਲ ਅੱਖਰ *)quic_tp, quic_tp_len);
    ਜੇ(!qtp_clone) {
      curlx_free(sdata);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  ਨਤੀਜਾ = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  ਜੇ(!ਨਤੀਜਾ && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    ਨਤੀਜਾ = Curl_ssl_session_dup(sc_session, &sc_dup);
  ਜੇ(!ਨਤੀਜਾ) {
    ਨਤੀਜਾ = Curl_ssl_scache_put(cf, ਅੰਕੜੇ, ssl_peer_ਕੁੰਜੀ, sc_session);
    /* took ownership of `sc_session` */
    sc_session = ਨਿਰਾ;
  }
  ਜੇ(!ਨਤੀਜਾ && psession) {
    *psession = sc_dup;
    sc_dup = ਨਿਰਾ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
ਸਥਿਰ CURLcode cf_gtls_update_session_id(ਬਣਤਰ Curl_cfilter *cf,
                                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                          gnutls_session_t session)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ Curl_ssl_session *scs = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_gtls_cache_session(cf, ਅੰਕੜੇ, connssl->peer.scache_ਕੁੰਜੀ,
                                   session, 0, connssl->negotiated.alpn,
                                   ਨਿਰਾ, 0, &scs);
  ਜੇ(!ਨਤੀਜਾ) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ gtls_handshake_cb(gnutls_session_t session, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ htype,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ when, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ incoming,
                             ਅਚਲ gnutls_datum_t *ਸੁਨੇਹਾ)
{
  ਬਣਤਰ Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (ਖਾਲੀ)ਸੁਨੇਹਾ;
  (ਖਾਲੀ)incoming;
  ਜੇ(when) { /* after message has been processed */
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
    ਜੇ(ਅੰਕੜੇ) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      ਚੋਣ(htype) {
      ਹਾਲਤ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, ਅੰਕੜੇ, session);
        ਰੋਕੋ;
      }
      ਮੂਲ:
        ਰੋਕੋ;
      }
    }
  }
  ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode gtls_ਸੈੱਟ_ਤਰਜੀਹ(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ gtls_ctx *gtls,
                                  ਅਚਲ ਅੱਖਰ *ਤਰਜੀਹ)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ dynbuf buf;
  ਅਚਲ ਅੱਖਰ *err = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ rc;

  curlx_dyn_init(&buf, 4096);

  ਜੇ(conn_config->cipher_list) {
    ਜੇ((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      ਜੇ(!curlx_dyn_len(&buf)) {  /* not added yet */
        ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&buf, ਤਰਜੀਹ);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਤੀਜਾ = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      ਤਰਜੀਹ = curlx_dyn_ptr(&buf);
    }
    ਨਹੀਂਤਾਂ /* replace our own completely */
      ਤਰਜੀਹ = conn_config->cipher_list;
  }

  infof(ਅੰਕੜੇ, "GnuTLS priority: %s", ਤਰਜੀਹ);
  rc = gnutls_ਤਰਜੀਹ_ਸੈੱਟ_ਸਿੱਧਾ(gtls->session, ਤਰਜੀਹ, &err);
  ਜੇ(rc != GNUTLS_E_SUCCESS) {
    failf(ਅੰਕੜੇ, "Error %d setting GnuTLS priority: %s", rc, err);
    ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
  }

ਬਾਹਰਲਾ:
  curlx_dyn_free(&buf);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode gtls_client_init(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ssl_peer *peer,
                                 size_t earlydata_max,
                                 ਬਣਤਰ gtls_ctx *gtls)
{
  ਬਣਤਰ ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ਅੰਕੜੇ);
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ init_flags;
  ਪੂਰਨ rc;
  ਅਚਲ ਅੱਖਰ *prioritylist;
  bool tls13support;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(config->ਸੰਸਕਰਣ_2 == CURL_SSLVERSION_SSLv2 ||
     config->ਸੰਸਕਰਣ_2 == CURL_SSLVERSION_SSLv3) {
    failf(ਅੰਕੜੇ, "GnuTLS does not support SSLv2 or SSLv3");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  ਨਤੀਜਾ = Curl_gtls_shared_creds_create(ਅੰਕੜੇ, &gtls->shared_creds);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
  ਜੇ(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_ਅੰਕੜੇ | GNUTLS_NO_ਸਿਰਾ_ਦਾ_EARLY_ਅੰਕੜੇ;
  ਨਹੀਂਤਾਂ ਜੇ(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GNUTLS_NO_ਸਥਿਤੀ_REQUEST
  ਜੇ(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_ਸਥਿਤੀ_REQUEST;
#ਜੇ_ਅੰਤ

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  ਜੇ(rc != GNUTLS_E_SUCCESS) {
    failf(ਅੰਕੜੇ, "gnutls_init() failed: %d", rc);
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਜੇ(peer->sni) {
    ਜੇ(gnutls_server_name_ਸੈੱਟ(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, ਲੜੀ_ਲੰਬਾਈ(peer->sni)) < 0) {
      failf(ਅੰਕੜੇ, "Failed to set SNI");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_ਸੈੱਟ_default_ਤਰਜੀਹ(gtls->session);
  ਜੇ(rc != GNUTLS_E_SUCCESS)
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_ਸੰਸਕਰਣ("3.6.5");

  ਜੇ(config->ਸੰਸਕਰਣ_2 == CURL_SSLVERSION_TLSv1_3) {
    ਜੇ(!tls13support) {
      failf(ਅੰਕੜੇ, "This GnuTLS installation does not support TLS 1.3");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  ਨਤੀਜਾ = gnutls_ਸੈੱਟ_ssl_ਸੰਸਕਰਣ_min_max(ਅੰਕੜੇ, peer,
                                          config, &prioritylist, tls13support);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = gtls_ਸੈੱਟ_ਤਰਜੀਹ(cf, ਅੰਕੜੇ, gtls, prioritylist);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(config->clientcert) {
    ਜੇ(!gtls->shared_creds->trust_ਤਿਆਰੀ) {
      ਨਤੀਜਾ = Curl_gtls_client_trust_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, gtls);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਜੇ(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_ਸੈੱਟ_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->ਕੁੰਜੀ_passwd ? config->ਕੁੰਜੀ_passwd : "");
      ਜੇ(rc != GNUTLS_E_SUCCESS) {
        failf(ਅੰਕੜੇ,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
      }
    }
    ਨਹੀਂਤਾਂ {
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ supported_ਕੁੰਜੀ_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_ਸੈੱਟ_x509_ਕੁੰਜੀ_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->ਕੁੰਜੀ ? config->ਕੁੰਜੀ : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->ਕੁੰਜੀ_passwd,
           supported_ਕੁੰਜੀ_encryption_algorithms);
      ਜੇ(rc != GNUTLS_E_SUCCESS) {
        failf(ਅੰਕੜੇ,
              "error reading X.509 %skey file: %s",
              config->ਕੁੰਜੀ_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_ਸੈੱਟ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  ਜੇ(rc != GNUTLS_E_SUCCESS) {
    failf(ਅੰਕੜੇ, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਜੇ(config->verifystatus) {
    rc = gnutls_ocsp_ਸਥਿਤੀ_request_enable_client(gtls->session,
                                                  ਨਿਰਾ, 0, ਨਿਰਾ);
    ਜੇ(rc != GNUTLS_E_SUCCESS) {
      failf(ਅੰਕੜੇ, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ਵਾਪਸ CURLE_OK;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
ਸਥਿਰ ਪੂਰਨ keylog_callback(gnutls_session_t session, ਅਚਲ ਅੱਖਰ *label,
                           ਅਚਲ gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  ਜੇ(crandom.ਮਾਪ != 32) {
    ਵਾਪਸ -1;
  }

  Curl_tls_keylog_write(label, crandom.ਅੰਕੜੇ, crandom.ਮਾਪ,
                        secret->ਅੰਕੜੇ, secret->ਮਾਪ);
  ਵਾਪਸ 0;
}

ਸਥਿਰ CURLcode gtls_on_session_reuse(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ alpn_spec *alpns,
                                      ਬਣਤਰ Curl_ssl_session *scs,
                                      bool *do_early_ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;

  connssl->earlydata_max =
    gnutls_ਰਿਕਾਰਡ_get_max_early_ਅੰਕੜੇ_ਮਾਪ(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  ਵਾਪਸ Curl_on_session_reuse(cf, ਅੰਕੜੇ, alpns, scs, do_early_ਅੰਕੜੇ,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode gtls_apply_session(
  ਬਣਤਰ gtls_ctx *gctx,
  ਬਣਤਰ Curl_cfilter *cf,
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ ssl_peer *peer,
  ਬਣਤਰ alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  ਬਣਤਰ Curl_ssl_session *scs,
  bool *pwas_ਤਿਆਰੀ)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ rc;

  ਜੇ(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_ਸ਼ਾਮਲ_ਹੈ_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    ਨਤੀਜਾ = gtls_client_init(cf, ਅੰਕੜੇ, peer, scs->earlydata_max, gctx);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    *pwas_ਤਿਆਰੀ = TRUE;

    rc = gnutls_session_ਸੈੱਟ_ਅੰਕੜੇ(gctx->session, scs->sdata, scs->sdata_len);
    ਜੇ(rc < 0)
      infof(ਅੰਕੜੇ, "SSL session not accepted by GnuTLS, continuing without");
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      ਜੇ(conn_config->earlydata && scs->alpn &&
         !cf->conn->ਬਿੱਟ.connect_only) {
        bool do_early_ਅੰਕੜੇ = FALSE;
        ਜੇ(sess_reuse_cb) {
          ਨਤੀਜਾ = sess_reuse_cb(cf, ਅੰਕੜੇ, alpns, scs, &do_early_ਅੰਕੜੇ);
          ਜੇ(ਨਤੀਜਾ)
            ਜਾਓ ਬਾਹਰਲਾ;
        }
        ਜੇ(do_early_ਅੰਕੜੇ) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_ਮੰਜ਼ਿਲ(alpns, scs->alpn);
        }
      }
    }
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_gtls_ctx_init(ਬਣਤਰ gtls_ctx *gctx,
                            ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਣਤਰ ssl_peer *peer,
                            ਅਚਲ ਬਣਤਰ alpn_spec *alpns_requested,
                            Curl_gtls_ctx_ਤਿਆਰੀ_cb *cb_ਤਿਆਰੀ,
                            ਖਾਲੀ *cb_user_ਅੰਕੜੇ,
                            ਖਾਲੀ *ssl_user_ਅੰਕੜੇ,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ Curl_ssl_session *scs = ਨਿਰਾ;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_ਗਿਣਤੀ = 0;
  bool gtls_session_ਤਿਆਰੀ = FALSE;
  ਬਣਤਰ alpn_spec alpns;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  ਜੇ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(ਅੰਕੜੇ, cf->cft, FIRSTSOCKET);
    ਜੇ(scs) {
      ਨਤੀਜਾ = gtls_apply_session(gctx, cf, ਅੰਕੜੇ, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_ਤਿਆਰੀ);
      scs = ਨਿਰਾ;
      ਜੇ(!ਨਤੀਜਾ && gtls_session_ਤਿਆਰੀ) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  ਜੇ(!gtls_session_ਤਿਆਰੀ && conn_config->cache_session &&
     !conn_config->verifystatus) {
    ਨਤੀਜਾ = Curl_ssl_scache_take(cf, ਅੰਕੜੇ, peer->scache_ਕੁੰਜੀ, &scs);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਨਤੀਜਾ = gtls_apply_session(gctx, cf, ਅੰਕੜੇ, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_ਤਿਆਰੀ);
  }

  ਜੇ(!gtls_session_ਤਿਆਰੀ) {
    ਨਤੀਜਾ = gtls_client_init(cf, ਅੰਕੜੇ, peer, 0, gctx);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  gnutls_session_ਸੈੱਟ_ptr(gctx->session, ssl_user_ਅੰਕੜੇ);

  ਜੇ(cb_ਤਿਆਰੀ) {
    ਨਤੀਜਾ = cb_ਤਿਆਰੀ(cf, ਅੰਕੜੇ, cb_user_ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  ਜੇ(Curl_tls_keylog_enabled()) {
    gnutls_session_ਸੈੱਟ_keylog_function(gctx->session, keylog_callback);
  }
#ਜੇ_ਅੰਤ

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  ਜੇ(!gtls_alpns_ਗਿਣਤੀ && alpns.ਗਿਣਤੀ_2) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.ਗਿਣਤੀ_2);
    ਲਈ(i = 0; i < alpns.ਗਿਣਤੀ_2; ++i) {
      gtls_alpns[i].ਅੰਕੜੇ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)alpns.entries[i];
      gtls_alpns[i].ਮਾਪ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਲੜੀ_ਲੰਬਾਈ(alpns.entries[i]);
    }
    gtls_alpns_ਗਿਣਤੀ = alpns.ਗਿਣਤੀ_2;
  }

  ਜੇ(gtls_alpns_ਗਿਣਤੀ &&
     gnutls_alpn_ਸੈੱਟ_protocols(gctx->session,
                               gtls_alpns, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)gtls_alpns_ਗਿਣਤੀ,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(ਅੰਕੜੇ, "failed setting ALPN");
    ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
  }

ਬਾਹਰਲਾ:
  Curl_ssl_scache_return(cf, ਅੰਕੜੇ, peer->scache_ਕੁੰਜੀ, scs);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode gtls_connect_step1(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(backend);

  ਜੇ(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = Curl_gtls_ctx_init(&backend->gtls, cf, ਅੰਕੜੇ, &connssl->peer,
                              connssl->alpn, ਨਿਰਾ, ਨਿਰਾ, cf,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
                              gtls_on_session_reuse
#ਨਹੀਂਤਾਂ
                              ਨਿਰਾ
#ਜੇ_ਅੰਤ
                              );

  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ਬਣਤਰ alpn_proto_buf proto;
    memset(&proto, 0, ਆਕਾਰ(proto));
    Curl_alpn_ਮੰਜ਼ਿਲ_proto_str(&proto, connssl->alpn);
    infof(ਅੰਕੜੇ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ਅੰਕੜੇ);
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
  gnutls_handshake_ਸੈੱਟ_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#ਜੇ_ਅੰਤ

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_ਸੈੱਟ_ptr(backend->gtls.session, cf);
  gnutls_transport_ਸੈੱਟ_ਧੱਕੋ_function(backend->gtls.session, gtls_ਧੱਕੋ);
  gnutls_transport_ਸੈੱਟ_pull_function(backend->gtls.session, gtls_pull);

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode pkp_pin_peer_pubkey(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    gnutls_x509_crt_t cert,
                                    ਅਚਲ ਅੱਖਰ *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buff1 = ਨਿਰਾ;

  gnutls_pubkey_t ਕੁੰਜੀ = ਨਿਰਾ;

  /* Result is returned to caller */
  CURLcode ਨਤੀਜਾ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ਜੇ(!pinnedpubkey)
    ਵਾਪਸ CURLE_OK;

  ਜੇ(!cert)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਕਰੋ {
    ਪੂਰਨ ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&ਕੁੰਜੀ);
    ਜੇ(ret < 0)
      ਰੋਕੋ; /* failed */

    ret = gnutls_pubkey_import_x509(ਕੁੰਜੀ, cert, 0);
    ਜੇ(ret < 0)
      ਰੋਕੋ; /* failed */

    ret = gnutls_pubkey_export(ਕੁੰਜੀ, GNUTLS_X509_FMT_DER, ਨਿਰਾ, &len1);
    ਜੇ(ret != GNUTLS_E_SHORT_ਸਮ੍ਰਿਤੀ_ਬਫਰ || len1 == 0)
      ਰੋਕੋ; /* failed */

    buff1 = curlx_malloc(len1);
    ਜੇ(!buff1)
      ਰੋਕੋ; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(ਕੁੰਜੀ, GNUTLS_X509_FMT_DER, buff1, &len2);
    ਜੇ(ret < 0 || len1 != len2)
      ਰੋਕੋ; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    ਨਤੀਜਾ = Curl_pin_peer_pubkey(ਅੰਕੜੇ, pinnedpubkey, buff1, len1);
  } ਜਦੋਂ(0);

  ਜੇ(ਕੁੰਜੀ)
    gnutls_pubkey_deinit(ਕੁੰਜੀ);

  curlx_safefree(buff1);

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_gtls_report_handshake(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ gtls_ctx *gctx)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    ਅਚਲ ਅੱਖਰ *ptr;
    gnutls_protocol_t ਸੰਸਕਰਣ_2 = gnutls_protocol_get_ਸੰਸਕਰਣ(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(ਅੰਕੜੇ, "SSL connection using %s / %s",
          gnutls_protocol_get_name(ਸੰਸਕਰਣ_2), ptr);
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)gctx;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ gtls_ਸੁਨੇਹਾ_verify_ਨਤੀਜਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਣਤਰ ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  ਅੱਖਰ certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t ਮਾਪ = ਆਕਾਰ(certname);
  ਪੂਰਨ rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &ਮਾਪ);
  ਜੇ(rc) {
    infof(ਅੰਕੜੇ, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  ਜੇ(!was_verified) {
    ਜੇ(needs_verified) {
      failf(ਅੰਕੜੇ, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  ਨਹੀਂਤਾਂ
    infof(ਅੰਕੜੇ, "  common name: %s (matched)", certname);
}

ਸਥਿਰ ਖਾਲੀ gtls_infof_cert(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            gnutls_x509_crt_t x509_cert)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  ਜੇ(Curl_trc_is_verbose(ਅੰਕੜੇ)) {
    gnutls_datum_t certfields;
    ਪੂਰਨ rc, algo;
    time_t tstamp;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਬਿੱਟ;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &ਬਿੱਟ);
    infof(ਅੰਕੜੇ, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(ਅੰਕੜੇ, "  certificate version: #%d",
          gnutls_x509_crt_get_ਸੰਸਕਰਣ(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    ਜੇ(rc)
      infof(ਅੰਕੜੇ, "Failed to get certificate name");
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, "  subject: %s", certfields.ਅੰਕੜੇ);

      tstamp = gnutls_x509_crt_get_activation_ਸਮਾਂ(x509_cert);
      showtime(ਅੰਕੜੇ, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_ਸਮਾਂ(x509_cert);
      showtime(ਅੰਕੜੇ, "expire date", tstamp);

      gnutls_free(certfields.ਅੰਕੜੇ);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    ਜੇ(rc)
      infof(ਅੰਕੜੇ, "Failed to get certificate issuer");
    ਨਹੀਂਤਾਂ {
      infof(ਅੰਕੜੇ, "  issuer: %s", certfields.ਅੰਕੜੇ);

      gnutls_free(certfields.ਅੰਕੜੇ);
    }
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)ਅੰਕੜੇ;
  (ਖਾਲੀ)x509_cert;
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ CURLcode gtls_verify_ocsp_ਸਥਿਤੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
  ਪੂਰਨ rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_ਸਥਿਤੀ_t ਸਥਿਤੀ = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_ਸਥਿਤੀ_request_get(session, &ocsp_raw);
  ਜੇ(rc == GNUTLS_E_REQUESTED_ਅੰਕੜੇ_NOT_ਉਪਲਬਧ) {
    failf(ਅੰਕੜੇ, "No OCSP response received");
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(rc < 0) {
    failf(ਅੰਕੜੇ, "Invalid OCSP response received");
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  ਜੇ(rc < 0) {
    failf(ਅੰਕੜੇ, "failed to init OCSP response parsing");
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  ਜੇ(rc < 0) {
    failf(ਅੰਕੜੇ, "failed to parse OCSP response");
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  ਜੇ(gnutls_ocsp_resp_get_ਸਥਿਤੀ(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(ਅੰਕੜੇ, "OCSP response basic part is not successful");
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  ਲਈ(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    ਜੇ(!rc || (rc == GNUTLS_E_REQUESTED_ਅੰਕੜੇ_NOT_ਉਪਲਬਧ))
      ਰੋਕੋ;
  }
  ਜੇ(rc < 0) {
    failf(ਅੰਕੜੇ, "OCSP does not carry status for peer cert");
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
                                   &ਸਥਿਤੀ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
  ਜੇ(rc < 0) {
    failf(ਅੰਕੜੇ, "failed to parse OCSP response for peer cert");
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਚੋਣ(ਸਥਿਤੀ) {
  ਹਾਲਤ GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    ਨਤੀਜਾ = CURLE_OK;
    ਰੋਕੋ;
  ਹਾਲਤ GNUTLS_OCSP_CERT_REVOKED:
    failf(ਅੰਕੜੇ, "peer certificate is revoked");
    ਰੋਕੋ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "peer certificate OCSP status is not good: %d", (ਪੂਰਨ)ਸਥਿਤੀ);
    ਰੋਕੋ;
  }

ਬਾਹਰਲਾ:
  ਜੇ(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ gtls_cert_chain {
  ਅਚਲ gnutls_datum_t *certs;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਅੰਕ_certs;
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
ਸਥਿਰ CURLcode gtls_chain_get_der(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਖਾਲੀ *user_ਅੰਕੜੇ,
                                   size_t i,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **pder,
                                   size_t *pder_len)
{
  ਬਣਤਰ gtls_cert_chain *chain = user_ਅੰਕੜੇ;

  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  *pder_len = 0;
  *pder = ਨਿਰਾ;

  ਜੇ(i >= chain->ਅੰਕ_certs)
    ਵਾਪਸ CURLE_TOO_LARGE;
  *pder = chain->certs[i].ਅੰਕੜੇ;
  *pder_len = (size_t)chain->certs[i].ਮਾਪ;
  ਵਾਪਸ CURLE_OK;
}
#ਜੇ_ਅੰਤ /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ਸਥਿਰ CURLcode gtls_verify_cert(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ ssl_filter_config *config,
                                 ਬਣਤਰ ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ ssl_peer *peer,
                                 ਬਣਤਰ gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ verify_ਸਥਿਤੀ = 0;
  ਲੰਮਾ * ਅਚਲ certverifyresult = &ssl_config->certverifyresult;
  ਪੂਰਨ rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_ਸਥਿਤੀ);
  ਜੇ(rc < 0) {
    failf(ਅੰਕੜੇ, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_ਸਥਿਤੀ;
  verified = !(verify_ਸਥਿਤੀ & GNUTLS_CERT_INVALID);
  ਜੇ(verified)
    infof(ਅੰਕੜੇ, "  SSL certificate verified by GnuTLS");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
  ਜੇ(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode ਨਤੀਜਾ;

    ਜੇ(gnutls_ocsp_ਸਥਿਤੀ_request_get(session, &ocsp_raw)) {
      ocsp_raw.ਅੰਕੜੇ = ਨਿਰਾ;
      ocsp_raw.ਮਾਪ = 0;
    }
    ਜੇ(config->verifystatus && !ocsp_raw.ਮਾਪ) {
      failf(ਅੰਕੜੇ, "No OCSP response received");
      ਵਾਪਸ CURLE_SSL_INVALIDCERTSTATUS;
    }
    ਨਤੀਜਾ = Curl_vtls_apple_verify(cf, ਅੰਕੜੇ, peer, chain->ਅੰਕ_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.ਅੰਕੜੇ, ocsp_raw.ਮਾਪ,
                                    config->verifystatus);
    ਜੇ(ਨਤੀਜਾ && (ਨਤੀਜਾ != CURLE_PEER_FAILED_VERIFICATION))
      ਵਾਪਸ ਨਤੀਜਾ; /* no further failure analysis needed */
    verified = !ਨਤੀਜਾ;
    ਜੇ(verified) {
      infof(ਅੰਕੜੇ, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)cf;
  (ਖਾਲੀ)peer;
  (ਖਾਲੀ)chain;
#ਜੇ_ਅੰਤ

  ਜੇ(!verified) {
    CURLcode ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ਅਚਲ ਅੱਖਰ *cause = "certificate error, no details available";
    ਜੇ(verify_ਸਥਿਤੀ & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    ਨਹੀਂਤਾਂ ਜੇ(verify_ਸਥਿਤੀ & GNUTLS_CERT_SIGNER_NOT_ਮਿਲਿਆ)
      cause = "certificate signer not trusted";
    ਨਹੀਂਤਾਂ ਜੇ(verify_ਸਥਿਤੀ & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    ਨਹੀਂਤਾਂ ਜੇ(verify_ਸਥਿਤੀ & GNUTLS_CERT_INVALID_OCSP_ਸਥਿਤੀ) {
      cause = "OCSP status response is invalid";
      ਨਤੀਜਾ = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(ਅੰਕੜੇ, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                gnutls_session_t session,
                                ਬਣਤਰ ssl_filter_config *config,
                                ਬਣਤਰ ssl_easy_config *ssl_config,
                                ਬਣਤਰ ssl_peer *peer,
                                ਅਚਲ ਅੱਖਰ *pinned_ਕੁੰਜੀ)
{
  ਬਣਤਰ gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = ਨਿਰਾ, x509_issuer = ਨਿਰਾ;
  time_t certclock;
  bool ocsp_checked = FALSE;
  ਪੂਰਨ rc;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਲੰਮਾ * ਅਚਲ certverifyresult = &ssl_config->certverifyresult;

  (ਖਾਲੀ)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.ਅੰਕ_certs);
  ਜੇ(!chain.certs) {
    ਜੇ(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(ਅੰਕੜੇ, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_ਮਿਲਿਆ;
      ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    infof(ਅੰਕੜੇ, " common name: WARNING could not obtain");
  }

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo && chain.certs) {
    ਜੇ(chain.ਅੰਕ_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(ਅੰਕੜੇ, "%u certificates is more than allowed (%d)",
            chain.ਅੰਕ_certs, MAX_ALLOWED_CERT_AMOUNT);
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;

      ਨਤੀਜਾ = Curl_ssl_init_certinfo(ਅੰਕੜੇ, (ਪੂਰਨ)chain.ਅੰਕ_certs);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;

      ਲਈ(i = 0; i < chain.ਅੰਕ_certs; i++) {
        ਅਚਲ uint8_t *beg = chain.certs[i].ਅੰਕੜੇ;
        ਅਚਲ uint8_t *ਸਿਰਾ = beg + chain.certs[i].ਮਾਪ;

        ਨਤੀਜਾ = Curl_ਕੱਢੋ_certinfo(ਅੰਕੜੇ, (ਪੂਰਨ)i, beg, ਸਿਰਾ);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
  }

  ਜੇ(config->verifypeer) {
    ਨਤੀਜਾ = gtls_verify_cert(ਅੰਕੜੇ, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ
    infof(ਅੰਕੜੇ, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  ਜੇ(gnutls_x509_crt_init(&x509_cert)) {
    failf(ਅੰਕੜੇ, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_ਮਿਲਿਆ;
    ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    ਜੇ(rc) {
      failf(ਅੰਕੜੇ, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_ਮਿਲਿਆ;
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_ਸਮਾਂ(x509_cert);

  ਜੇ(certclock == (time_t)-1) {
    ਜੇ(config->verifypeer) {
      failf(ਅੰਕੜੇ, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "  SSL certificate expiration date verify FAILED");
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(certclock < ਸਮਾਂ(ਨਿਰਾ)) {
      ਜੇ(config->verifypeer) {
        failf(ਅੰਕੜੇ, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ
        infof(ਅੰਕੜੇ, "  SSL certificate expiration date FAILED");
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_ਸਮਾਂ(x509_cert);

  ਜੇ(certclock == (time_t)-1) {
    ਜੇ(config->verifypeer) {
      failf(ਅੰਕੜੇ, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "  SSL certificate activation date verify FAILED");
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(certclock > ਸਮਾਂ(ਨਿਰਾ)) {
      ਜੇ(config->verifypeer) {
        failf(ਅੰਕੜੇ, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        ਨਤੀਜਾ = CURLE_PEER_FAILED_VERIFICATION;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ
        infof(ਅੰਕੜੇ, "  SSL certificate activation date FAILED");
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "  SSL certificate activation date OK");
  }

  ਜੇ(config->verifystatus && !ocsp_checked) {
    ਨਤੀਜਾ = gtls_verify_ocsp_ਸਥਿਤੀ(ਅੰਕੜੇ, session, x509_cert);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ
    infof(ਅੰਕੜੇ, "  SSL certificate status verification SKIPPED");

  ਜੇ(config->issuercert) {
    gnutls_datum_t issuerp;
    ਜੇ(gnutls_x509_crt_init(&x509_issuer)) {
      failf(ਅੰਕੜੇ, "failed to init GnuTLS x509_crt for issuer");
      ਨਤੀਜਾ = CURLE_SSL_ISSUER_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    issuerp = ਲੱਦੋ_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    ਜੇ(rc) {
      failf(ਅੰਕੜੇ, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      ਨਤੀਜਾ = CURLE_SSL_ISSUER_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    rc = (ਪੂਰਨ)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    ਜੇ(rc <= 0) {
      failf(ਅੰਕੜੇ, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      ਨਤੀਜਾ = CURLE_SSL_ISSUER_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    infof(ਅੰਕੜੇ, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  rc = (ਪੂਰਨ)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  ਨਤੀਜਾ = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_ਸੁਨੇਹਾ_verify_ਨਤੀਜਾ(ਅੰਕੜੇ, peer, x509_cert, rc, config->verifyhost);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(pinned_ਕੁੰਜੀ) {
    ਨਤੀਜਾ = pkp_pin_peer_pubkey(ਅੰਕੜੇ, x509_cert, pinned_ਕੁੰਜੀ);
    ਜੇ(ਨਤੀਜਾ != CURLE_OK) {
      failf(ਅੰਕੜੇ, "SSL: public key does not match pinned public key");
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  gtls_infof_cert(ਅੰਕੜੇ, x509_cert);

ਬਾਹਰਲਾ:
  ਜੇ(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  ਜੇ(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode gtls_verifyserver(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  gnutls_session_t session)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ਅੰਕੜੇ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਅਚਲ ਅੱਖਰ *pinned_ਕੁੰਜੀ = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY);
#ਨਹੀਂਤਾਂ
  ਅਚਲ ਅੱਖਰ *pinned_ਕੁੰਜੀ = CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY);
#ਜੇ_ਅੰਤ
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_gtls_verifyserver(cf, ਅੰਕੜੇ, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_ਕੁੰਜੀ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  ਜੇ(gnutls_protocol_get_ਸੰਸਕਰਣ(session) < GNUTLS_TLS1_3)
    ਨਤੀਜਾ = cf_gtls_update_session_id(cf, ਅੰਕੜੇ, session);
#ਜੇ_ਅੰਤ

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
ਸਥਿਰ CURLcode gtls_send_earlydata(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_ਨਤੀਜਾ = CURLE_OK;
  ਜਦੋਂ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_ਰਿਕਾਰਡ_send_early_ਅੰਕੜੇ(backend->gtls.session, buf, blen);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    ਜੇ(n < 0) {
      ਜੇ(n == GNUTLS_E_AGAIN)
        ਨਤੀਜਾ = CURLE_AGAIN;
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = backend->gtls.io_ਨਤੀਜਾ ?
                 backend->gtls.io_ਨਤੀਜਾ : CURLE_SEND_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(ਅੰਕੜੇ, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

/*
 * This function is called after the TCP connect has completed. Setup the TLS
 * layer and do all necessary magic.
 */
/* We use connssl->connecting_state to keep track of the connection status;
   there are three states: 'ssl_connect_1' (not started yet or complete),
   'ssl_connect_2' (doing handshake with the server), and
   'ssl_connect_3' (verifying and getting stats).
 */
ਸਥਿਰ CURLcode gtls_connect_common(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  ਜੇ(ssl_connection_complete == connssl->state) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;

  /* Initiate the connection, if not already done */
  ਜੇ(connssl->connecting_state == ssl_connect_1) {
    ਨਤੀਜਾ = gtls_connect_step1(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    connssl->connecting_state = ssl_connect_2;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_2) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
    ਜੇ(connssl->earlydata_state == ssl_earlydata_await) {
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(connssl->earlydata_state == ssl_earlydata_sending) {
      ਨਤੀਜਾ = gtls_send_earlydata(cf, ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      connssl->earlydata_state = ssl_earlydata_ਭੇਜਿਆ;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_ਭੇਜਿਆ));
#ਜੇ_ਅੰਤ
    ਨਤੀਜਾ = cf_gtls_handshake(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  ਜੇ(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    ਪੂਰਨ rc;

    Curl_gtls_report_handshake(ਅੰਕੜੇ, &backend->gtls);

    ਨਤੀਜਾ = gtls_verifyserver(cf, ਅੰਕੜੇ, backend->gtls.session);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    ਜੇ(rc) {  /* No ALPN from server */
      proto.ਅੰਕੜੇ = ਨਿਰਾ;
      proto.ਮਾਪ = 0;
    }

    ਨਤੀਜਾ = Curl_alpn_ਸੈੱਟ_negotiated(cf, ਅੰਕੜੇ, connssl,
                                      proto.ਅੰਕੜੇ, proto.ਮਾਪ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
    ਜੇ(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_ਭੇਜਿਆ);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_ਅੰਕੜੇ) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#ਜੇ_ਅੰਤ
    connssl->connecting_state = ssl_connect_ਪੂਰਾ;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_ਪੂਰਾ)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    *ਪੂਰਾ = FALSE;
    ਵਾਪਸ CURLE_OK;
  }
  *ਪੂਰਾ = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "gtls_connect_common() -> %d, done=%d", (ਪੂਰਨ)ਨਤੀਜਾ,
              *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode gtls_connect(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             bool *ਪੂਰਾ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_GNUTLS_EARLY_ਅੰਕੜੇ
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਜੇ((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }
#ਜੇ_ਅੰਤ
  ਵਾਪਸ gtls_connect_common(cf, ਅੰਕੜੇ, ਪੂਰਾ);
}

ਸਥਿਰ bool gtls_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                              ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *ctx = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)ctx->backend;
  ਜੇ(backend->gtls.session &&
     gnutls_ਰਿਕਾਰਡ_check_pending(backend->gtls.session) != 0)
    ਵਾਪਸ TRUE;
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLcode gtls_send(ਬਣਤਰ Curl_cfilter *cf,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਖਾਲੀ *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  ਜਦੋਂ(remain) {
    backend->gtls.io_ਨਤੀਜਾ = CURLE_OK;
    nwritten = gnutls_ਰਿਕਾਰਡ_send(backend->gtls.session, buf, remain);

    ਜੇ(nwritten >= 0) {
      ਜੇ((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(ਅੰਕੜੇ, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        ਵਾਪਸ CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (ਅੱਖਰ *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        ਨਤੀਜਾ = CURLE_OK;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਤੀਜਾ = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_ਨਤੀਜਾ ? backend->gtls.io_ਨਤੀਜਾ : CURLE_SEND_ERROR);
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ਸਥਿਰ CURLcode gtls_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              bool send_shutdown, bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਅੱਖਰ buf[1024];
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  ਜੇ(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *ਪੂਰਾ = TRUE;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *ਪੂਰਾ = FALSE;

  ਜੇ(!backend->gtls.ਭੇਜਿਆ_shutdown) {
    /* do this only once */
    backend->gtls.ਭੇਜਿਆ_shutdown = TRUE;
    ਜੇ(send_shutdown) {
      ਪੂਰਨ ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      ਜੇ((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_ਰਿਕਾਰਡ_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.ਭੇਜਿਆ_shutdown = FALSE;
        ਨਤੀਜਾ = CURLE_OK;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((ਪੂਰਨ)ret), (ਪੂਰਨ)ret);
        ਨਤੀਜਾ = CURLE_RECV_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ਲਈ(i = 0; i < 10; ++i) {
    nread = gnutls_ਰਿਕਾਰਡ_recv(backend->gtls.session, buf, ਆਕਾਰ(buf));
    ਜੇ(nread <= 0)
      ਰੋਕੋ;
  }
  ਜੇ(nread > 0) {
    /* still data coming in? */
  }
  ਨਹੀਂਤਾਂ ਜੇ(nread == 0) {
    /* We got the close notify alert and are done. */
    *ਪੂਰਾ = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_ਰਿਕਾਰਡ_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((ਪੂਰਨ)nread), (ਪੂਰਨ)nread);
    ਨਤੀਜਾ = CURLE_RECV_ERROR;
  }

ਬਾਹਰਲਾ:
  cf->shutdown = (ਨਤੀਜਾ || *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ gtls_close(ਬਣਤਰ Curl_cfilter *cf,
                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "close");
  ਜੇ(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = ਨਿਰਾ;
  }
  ਜੇ(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ਸਥਿਰ CURLcode gtls_recv(ਬਣਤਰ Curl_cfilter *cf,
                          ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅੱਖਰ *buf, size_t blen,
                          size_t *pnread)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_ਰਿਕਾਰਡ_recv(backend->gtls.session, buf, blen);

  ਜੇ(nread >= 0)
    *pnread = (size_t)nread;
  ਨਹੀਂਤਾਂ {
    ਜੇ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      ਨਤੀਜਾ = cf_gtls_handshake(cf, ਅੰਕੜੇ);
      ਜੇ(!ਨਤੀਜਾ)
        ਨਤੀਜਾ = CURLE_AGAIN; /* make us get called again. */
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "GnuTLS recv error (%d): %s",
            (ਪੂਰਨ)nread, gnutls_strerror((ਪੂਰਨ)nread));
      ਨਤੀਜਾ = backend->gtls.io_ਨਤੀਜਾ ?
        backend->gtls.io_ਨਤੀਜਾ : CURLE_RECV_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (ਪੂਰਨ)ਨਤੀਜਾ, nread);
  ਵਾਪਸ ਨਤੀਜਾ;
}

size_t Curl_gtls_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ)
{
  ਵਾਪਸ curl_msnprintf(ਬਫਰ, ਮਾਪ, "GnuTLS/%s", gnutls_check_ਸੰਸਕਰਣ(ਨਿਰਾ));
}

/* data might be NULL! */
ਸਥਿਰ CURLcode gtls_random(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *entropy, size_t ਲੰਮਾਈ)
{
  ਪੂਰਨ rc;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, ਲੰਮਾਈ);
  ਵਾਪਸ rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ਸਥਿਰ CURLcode gtls_sha256sum(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                               size_t len,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha256sum, /* output */
                               size_t sha256len)
{
  ਬਣਤਰ sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  ਕਰੋ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ilen = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, ਨਿਵੇਸ਼);
    len -= ilen;
    ਨਿਵੇਸ਼ += ilen;
  } ਜਦੋਂ(len);
#ਜੇ NETTLE_ਸੰਸਕਰਣ_MAJOR >= 4
  (ਖਾਲੀ)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#ਨਹੀਂਤਾਂ
  sha256_digest(&SHA256pw, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)sha256len, sha256sum);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool gtls_cert_ਸਥਿਤੀ_request(ਖਾਲੀ)
{
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ ਖਾਲੀ *gtls_get_internals(ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl,
                                CURLINFO info)
{
  ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  (ਖਾਲੀ)info;
  DEBUGASSERT(backend);
  ਵਾਪਸ backend->gtls.session;
}

ਅਚਲ ਬਣਤਰ Curl_ssl Curl_ssl_gnutls = {
  { CURLSSLBACKEND_GNUTLS, "gnutls" }, /* info */

  SSLSUPP_CA_ਰਸਤਾ  |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_CA_CACHE |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_CRLFILE,

  ਆਕਾਰ(ਬਣਤਰ gtls_ssl_backend_ਅੰਕੜੇ),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_ਸੰਸਕਰਣ,             /*ਸੰਸਕਰਣ*/
  gtls_shutdown,                 /* shutdown */
  gtls_ਅੰਕੜੇ_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_ਸਥਿਤੀ_request,      /* cert_status_request */
  gtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,       /* adjust_pollset */
  gtls_get_internals,            /* get_internals */
  gtls_close,                    /* close_one */
  ਨਿਰਾ,                          /* close_all */
  ਨਿਰਾ,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  ਨਿਰਾ,                          /* get_channel_binding */
};

#ਜੇ_ਅੰਤ /* USE_GNUTLS */
