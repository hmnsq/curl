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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_GNUTLS

#అన్తర్భావ <gnutls/abstract.h>
#అన్తర్భావ <gnutls/gnutls.h>
#అన్తర్భావ <gnutls/x509.h>
#అన్తర్భావ <gnutls/crypto.h>
#అన్తర్భావ <nettle/sha2.h>
#అన్తర్భావ <nettle/version.h>

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "vtls/keylog.h"
#అన్తర్భావ "vtls/gtls.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_int.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "vtls/apple.h"
#అన్తర్భావ "vauth/vauth.h"
#అన్తర్భావ "parsedate.h"
#అన్తర్భావ "connect.h" /* for the connect timeout */
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#ఒకవేళ 0
#నిర్వచించు GTLSDEBUG
#అయితే_ముగింపు

#నిర్వచితమైతే GTLSDEBUG
స్థిర శూన్య tls_log_func(పూర్ణాంకం level, స్థిరరాశి అక్షరం *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#అయితే_ముగింపు

#ఒకవేళ !defined(GNUTLS_సంస్కరణ_సంఖ్య) || (GNUTLS_సంస్కరణ_సంఖ్య < 0x030605)
#దోషం "too old GnuTLS version"
#అయితే_ముగింపు

#నిర్వచనం_రద్దు CURL_GNUTLS_EARLY_దత్తాంశం
#ఒకవేళ GNUTLS_సంస్కరణ_సంఖ్య >= 0x030702
#నిర్వచించు CURL_GNUTLS_EARLY_దత్తాంశం
#అయితే_ముగింపు

#అన్తర్భావ <gnutls/ocsp.h>

నిర్మాణం gtls_ssl_backend_దత్తాంశం {
  నిర్మాణం gtls_ctx gtls;
};

స్థిర ssize_t gtls_నెట్టు(శూన్య *s, స్థిరరాశి శూన్య *buf, size_t blen)
{
  నిర్మాణం Curl_cfilter *cf = s;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  size_t nwritten;
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం);
  ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(దత్తాంశం, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (పూర్ణాంకం)ఫలితం, nwritten);
  backend->gtls.io_ఫలితం = ఫలితం;
  ఒకవేళ(ఫలితం) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_సమితి_errno(backend->gtls.session,
                               (ఫలితం == CURLE_AGAIN) ? EAGAIN : EINVAL);
    తిరిగి -1;
  }
  తిరిగి (ssize_t)nwritten;
}

స్థిర ssize_t gtls_pull(శూన్య *s, శూన్య *buf, size_t blen)
{
  నిర్మాణం Curl_cfilter *cf = s;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  size_t nread;
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(!backend->gtls.shared_creds->trust_అమరిక) {
    ఫలితం = Curl_gtls_client_trust_అమరిక(cf, దత్తాంశం, &backend->gtls);
    ఒకవేళ(ఫలితం) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_సమితి_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_ఫలితం = ఫలితం;
      తిరిగి -1;
    }
  }

  ఫలితం = Curl_conn_cf_recv(cf->తదుపరి, దత్తాంశం, buf, blen, &nread);
  CURL_TRC_CF(దత్తాంశం, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (పూర్ణాంకం)ఫలితం,
              nread);
  backend->gtls.io_ఫలితం = ఫలితం;
  ఒకవేళ(ఫలితం) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_సమితి_errno(backend->gtls.session,
                               (ఫలితం == CURLE_AGAIN) ? EAGAIN : EINVAL);
    తిరిగి -1;
  }
  లేకపోతే ఒకవేళ(nread == 0)
    connssl->peer_closed = TRUE;
  తిరిగి (ssize_t)nread;
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
స్థిర పూర్ణాంకం gtls_init(శూన్య)
{
  పూర్ణాంకం ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#నిర్వచితమైతే GTLSDEBUG
  gnutls_global_సమితి_log_function(tls_log_func);
  gnutls_global_సమితి_log_level(2);
#అయితే_ముగింపు
  తిరిగి ret;
}

స్థిర శూన్య gtls_cleanup(శూన్య)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#నిర్వచితమైతే CURLVERBOSE
స్థిర శూన్య showtime(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *పాఠ్యం_3, time_t stamp)
{
  నిర్మాణం tm బఫరు;
  స్థిరరాశి నిర్మాణం tm *tm = &బఫరు;
  అక్షరం str[96];
  CURLcode ఫలితం = curlx_gmtime(stamp, &బఫరు);
  ఒకవేళ(ఫలితం)
    తిరిగి;

  curl_msnprintf(str,
                 పరిమాణం(str),
                 "  %s: %s, %02d %s %4d %02d:%02d:%02d GMT",
                 పాఠ్యం_3,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);
  infof(దత్తాంశం, "%s", str);
}
#అయితే_ముగింపు

స్థిర gnutls_datum_t ఎక్కించు_file(స్థిరరాశి అక్షరం *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { ఖాళీ, 0 };
  దీర్ఘ filelen;
  శూన్య *ptr;

  f = curlx_fopen(file, "rb");
  ఒకవేళ(!f)
    తిరిగి loaded_file;
  ఒకవేళ(fseek(f, 0, SEEK_END))
    వెళ్ళు బహిర్గతం;
  filelen = ftell(f);
  ఒకవేళ(filelen < 0 || filelen > CURL_MAX_ఆదానం_నిడివి)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(fseek(f, 0, SEEK_SET))
    వెళ్ళు బహిర్గతం;
  ptr = curlx_malloc((size_t)filelen);
  ఒకవేళ(!ptr)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    వెళ్ళు బహిర్గతం;
  }

  loaded_file.దత్తాంశం = ptr;
  loaded_file.కొలత = (గుర్తులేని పూర్ణాంకం)filelen;
బహిర్గతం:
  curlx_fclose(f);
  తిరిగి loaded_file;
}

స్థిర శూన్య unload_file(gnutls_datum_t దత్తాంశం)
{
  curlx_free(దత్తాంశం.దత్తాంశం);
}

/* this function does an SSL/TLS (re-)handshake */
స్థిర CURLcode cf_gtls_handshake(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  gnutls_session_t session;
  పూర్ణాంకం rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_ఫలితం = CURLE_OK;
  rc = gnutls_handshake(session);

  ఒకవేళ(!backend->gtls.shared_creds->trust_అమరిక) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode ఫలితం = Curl_gtls_client_trust_అమరిక(cf, దత్తాంశం, &backend->gtls);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_రికార్డు_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    తిరిగి CURLE_AGAIN;
  }
  లేకపోతే ఒకవేళ((rc < 0) && !gnutls_error_is_fatal(rc)) {
#నిర్వచితమైతే CURLVERBOSE
    స్థిరరాశి అక్షరం *strerr = ఖాళీ;

    ఒకవేళ(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ఒకవేళ(!strerr)
      strerr = gnutls_strerror(rc);

    infof(దత్తాంశం, "gnutls_handshake() warning: %s", strerr);
#అయితే_ముగింపు
    తిరిగి CURLE_AGAIN;
  }
  లేకపోతే ఒకవేళ((rc < 0) && backend->gtls.io_ఫలితం) {
    తిరిగి backend->gtls.io_ఫలితం;
  }
  లేకపోతే ఒకవేళ(rc < 0) {
    స్థిరరాశి అక్షరం *strerr = ఖాళీ;

    ఒకవేళ(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ఒకవేళ(!strerr)
      strerr = gnutls_strerror(rc);

    failf(దత్తాంశం, "GnuTLS, handshake failed: %s", strerr);
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  తిరిగి CURLE_OK;
}

స్థిర gnutls_x509_crt_fmt_t gnutls_do_file_type(స్థిరరాశి అక్షరం *type)
{
  ఒకవేళ(!type || !type[0])
    తిరిగి GNUTLS_X509_FMT_PEM;
  ఒకవేళ(curl_strequal(type, "PEM"))
    తిరిగి GNUTLS_X509_FMT_PEM;
  ఒకవేళ(curl_strequal(type, "DER"))
    తిరిగి GNUTLS_X509_FMT_DER;
  తిరిగి GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#నిర్వచించు GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#నిర్వచించు QUIC_ప్రాధాన్యత \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

స్థిర CURLcode gnutls_సమితి_ssl_సంస్కరణ_min_max(
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం ssl_peer *peer,
  నిర్మాణం ssl_filter_config *conn_config,
  స్థిరరాశి అక్షరం **prioritylist,
  bool tls13support)
{
  దీర్ఘ ssl_సంస్కరణ = conn_config->సంస్కరణ_2;
  దీర్ఘ ssl_సంస్కరణ_max = conn_config->సంస్కరణ_max;

  DEBUGASSERT(ssl_సంస్కరణ != CURL_SSLVERSION_DEFAULT);
  ఒకవేళ(ssl_సంస్కరణ <= CURL_SSLVERSION_TLSv1)
    ssl_సంస్కరణ = CURL_SSLVERSION_TLSv1_0;
  ఒకవేళ((ssl_సంస్కరణ_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_సంస్కరణ_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_సంస్కరణ_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  ఒకవేళ(peer->transport == TRNSPRT_QUIC) {
    ఒకవేళ(ssl_సంస్కరణ_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(దత్తాంశం, "QUIC needs at least TLS version 1.3");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_ప్రాధాన్యత;
    తిరిగి CURLE_OK;
  }

  ఎంపిక(ssl_సంస్కరణ | ssl_సంస్కరణ_max) {
  స్థితి CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    తిరిగి CURLE_OK;
  స్థితి CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    తిరిగి CURLE_OK;
  స్థితి CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    తిరిగి CURLE_OK;
  స్థితి CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    తిరిగి CURLE_OK;
  స్థితి CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    తిరిగి CURLE_OK;
  స్థితి CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    తిరిగి CURLE_OK;
  స్థితి CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    తిరిగి CURLE_OK;
  స్థితి CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    తిరిగి CURLE_OK;
  స్థితి CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    తిరిగి CURLE_OK;
  స్థితి CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    తిరిగి CURLE_OK;
  }

  failf(దత్తాంశం, "GnuTLS: cannot set TLS protocol");
  తిరిగి CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(నిర్మాణం Curl_easy *దత్తాంశం,
                                       నిర్మాణం gtls_shared_creds **pcreds)
{
  నిర్మాణం gtls_shared_creds *shared;
  పూర్ణాంకం rc;

  *pcreds = ఖాళీ;
  shared = curlx_calloc(1, పరిమాణం(*shared));
  ఒకవేళ(!shared)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  ఒకవేళ(rc != GNUTLS_E_SUCCESS) {
    failf(దత్తాంశం, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->సమయం = *Curl_pgrs_now(దత్తాంశం);
  *pcreds = shared;
  తిరిగి CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_పైకి_ref(నిర్మాణం gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  ఒకవేళ(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    తిరిగి CURLE_OK;
  }
  తిరిగి CURLE_BAD_FUNCTION_వాదం;
}

శూన్య Curl_gtls_shared_creds_free(నిర్మాణం gtls_shared_creds **pcreds)
{
  నిర్మాణం gtls_shared_creds *shared = *pcreds;
  *pcreds = ఖాళీ;
  ఒకవేళ(shared) {
    --shared->refcount;
    ఒకవేళ(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

స్థిర CURLcode gtls_populate_creds(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    gnutls_certificate_credentials_t creds)
{
  నిర్మాణం ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, దత్తాంశం);
  bool creds_are_ఖాళీలు = TRUE;
  పూర్ణాంకం rc;

  ఒకవేళ(!config->verifypeer) {
    infof(దత్తాంశం, "SSL Trust: peer verification disabled");
    తిరిగి CURLE_OK;
  }

  infof(దత్తాంశం, "SSL Trust Anchors:");
  ఒకవేళ(config->native_ca_store) {
#నిర్వచితమైతే USE_APPLE_SECTRUST
    infof(దత్తాంశం, "  Native: Apple SecTrust");
    creds_are_ఖాళీలు = FALSE;
#లేకపోతే
    rc = gnutls_certificate_సమితి_x509_system_trust(creds);
    ఒకవేళ(rc < 0)
      infof(దత్తాంశం, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    లేకపోతే {
      infof(దత్తాంశం, "  Native: %d certificates from system trust", rc);
      ఒకవేళ(rc > 0)
        creds_are_ఖాళీలు = FALSE;
    }
#అయితే_ముగింపు
  }

  ఒకవేళ(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    ఒకవేళ(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(దత్తాంశం, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      తిరిగి CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.దత్తాంశం = config->ca_info_blob->దత్తాంశం;
    ca_info_datum.కొలత = (గుర్తులేని పూర్ణాంకం)config->ca_info_blob->len;
    rc = gnutls_certificate_సమితి_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_ఖాళీలు = creds_are_ఖాళీలు && (rc <= 0);
    ఒకవేళ(rc < 0) {
      infof(దత్తాంశం, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_ఖాళీలు ? "" : ", continuing anyway"));
      ఒకవేళ(creds_are_ఖాళీలు) {
        ssl_config->certverifyresult = rc;
        తిరిగి CURLE_SSL_CACERT_BADFILE;
      }
    }
    లేకపోతే
      infof(దత్తాంశం, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  లేకపోతే ఒకవేళ(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_సమితి_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_సమితి_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_ఖాళీలు = creds_are_ఖాళీలు && (rc <= 0);
    ఒకవేళ(rc < 0) {
      infof(దత్తాంశం, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_ఖాళీలు ? "" : ", continuing anyway"));
      ఒకవేళ(creds_are_ఖాళీలు) {
        ssl_config->certverifyresult = rc;
        తిరిగి CURLE_SSL_CACERT_BADFILE;
      }
    }
    లేకపోతే
      infof(దత్తాంశం, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  ఒకవేళ(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_సమితి_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_ఖాళీలు = creds_are_ఖాళీలు && (rc <= 0);
    ఒకవేళ(rc < 0) {
      infof(దత్తాంశం, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_ఖాళీలు ? "" : ", continuing anyway"));
      ఒకవేళ(creds_are_ఖాళీలు) {
        ssl_config->certverifyresult = rc;
        తిరిగి CURLE_SSL_CACERT_BADFILE;
      }
    }
    లేకపోతే
      infof(దత్తాంశం, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  ఒకవేళ(creds_are_ఖాళీలు)
    infof(దత్తాంశం, "  no trust anchors configured");

#నిర్వచితమైతే USE_APPLE_SECTRUST
  ఒకవేళ(config->CRLfile && config->native_ca_store) {
    failf(దత్తాంశం, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    తిరిగి CURLE_NOT_BUILT_లోపల;
  }
#అయితే_ముగింపు
  ఒకవేళ(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_సమితి_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    ఒకవేళ(rc < 0) {
      failf(దత్తాంశం, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      తిరిగి CURLE_SSL_CRL_BADFILE;
    }
    లేకపోతే
      infof(దత్తాంశం, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  తిరిగి CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#నిర్వచించు MPROTO_GTLS_X509_కీలకం   "tls:gtls:x509:share"

స్థిర bool gtls_shared_creds_expired(నిర్మాణం Curl_easy *దత్తాంశం,
                                      స్థిరరాశి నిర్మాణం gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(దత్తాంశం), &sc->సమయం);
  timediff_t timeout_ms = దత్తాంశం->సమితి.ssl_ca_cache_timeout * (timediff_t)1000;

  ఒకవేళ(timeout_ms < 0)
    తిరిగి FALSE;

  తిరిగి elapsed_ms >= timeout_ms;
}

స్థిర bool gtls_shared_creds_different(నిర్మాణం Curl_cfilter *cf,
                                        స్థిరరాశి నిర్మాణం gtls_shared_creds *sc)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ఒకవేళ(!sc->CAfile || !conn_config->CAfile)
    తిరిగి sc->CAfile != conn_config->CAfile;

  తిరిగి strcmp(sc->CAfile, conn_config->CAfile);
}

స్థిర నిర్మాణం gtls_shared_creds *gtls_get_cached_creds(నిర్మాణం Curl_cfilter *cf,
                                                       నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం gtls_shared_creds *shared_creds;

  ఒకవేళ(దత్తాంశం->multi) {
    shared_creds = Curl_hash_pick(&దత్తాంశం->multi->proto_hash,
                                  MPROTO_GTLS_X509_కీలకం,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_కీలకం));
    ఒకవేళ(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(దత్తాంశం, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      తిరిగి shared_creds;
    }
  }
  తిరిగి ఖాళీ;
}

స్థిర శూన్య gtls_shared_creds_hash_free(స్థిరరాశి శూన్య *కీలకం,
                                        size_t కీలకం_len, శూన్య *p)
{
  నిర్మాణం gtls_shared_creds *sc = p;
  DEBUGASSERT(కీలకం_len == CURL_CSTRLEN(MPROTO_GTLS_X509_కీలకం));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_కీలకం, కీలకం, కీలకం_len));
  (శూన్య)కీలకం;
  (శూన్య)కీలకం_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

స్థిర శూన్య gtls_సమితి_cached_creds(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం gtls_shared_creds *sc)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  ఒకవేళ(!దత్తాంశం->multi)
    తిరిగి;

  ఒకవేళ(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    ఒకవేళ(!sc->CAfile)
      తిరిగి;
  }

  ఒకవేళ(Curl_gtls_shared_creds_పైకి_ref(sc))
    తిరిగి;

  ఒకవేళ(!Curl_hash_add2(&దత్తాంశం->multi->proto_hash,
                     MPROTO_GTLS_X509_కీలకం,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_కీలకం),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    తిరిగి;
  }
}

CURLcode Curl_gtls_client_trust_అమరిక(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం gtls_ctx *gtls)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం gtls_shared_creds *cached_creds = ఖాళీ;
  bool cache_criteria_met;
  CURLcode ఫలితం;
  పూర్ణాంకం rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (దత్తాంశం->సమితి.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  ఒకవేళ(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, దత్తాంశం);

  ఒకవేళ(cached_creds && !Curl_gtls_shared_creds_పైకి_ref(cached_creds)) {
    CURL_TRC_CF(దత్తాంశం, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_సమితి(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    ఒకవేళ(rc != GNUTLS_E_SUCCESS) {
      failf(దత్తాంశం, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }
  లేకపోతే {
    CURL_TRC_CF(దత్తాంశం, cf, "loading trust anchors and CRLs");
    ఫలితం = gtls_populate_creds(cf, దత్తాంశం, gtls->shared_creds->creds);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    gtls->shared_creds->trust_అమరిక = TRUE;
    ఒకవేళ(cache_criteria_met)
      gtls_సమితి_cached_creds(cf, దత్తాంశం, gtls->shared_creds);
  }
  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
స్థిర పూర్ణాంకం gtls_get_ietf_proto(gnutls_session_t session)
{
  ఎంపిక(gnutls_protocol_get_సంస్కరణ(session)) {
  స్థితి GNUTLS_SSL3:
    తిరిగి CURL_IETF_PROTO_SSL3;
  స్థితి GNUTLS_TLS1_0:
    తిరిగి CURL_IETF_PROTO_TLS1;
  స్థితి GNUTLS_TLS1_1:
    తిరిగి CURL_IETF_PROTO_TLS1_1;
  స్థితి GNUTLS_TLS1_2:
    తిరిగి CURL_IETF_PROTO_TLS1_2;
  స్థితి GNUTLS_TLS1_3:
    తిరిగి CURL_IETF_PROTO_TLS1_3;
  అప్రమేయ:
    తిరిగి CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 స్థిరరాశి అక్షరం *alpn,
                                 గుర్తులేని అక్షరం *quic_tp,
                                 size_t quic_tp_len,
                                 నిర్మాణం Curl_ssl_session **psession)
{
  నిర్మాణం Curl_ssl_session *sc_session = ఖాళీ, *sc_dup = ఖాళీ;
  గుర్తులేని అక్షరం *sdata, *qtp_clone = ఖాళీ;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(psession)
    *psession = ఖాళీ;
  ఒకవేళ(!Curl_ssl_scache_use(cf, దత్తాంశం))
    తిరిగి CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_దత్తాంశం(session, ఖాళీ, &sdata_len);
  ఒకవేళ(!sdata_len) /* GnuTLS does this for some version combinations */
    తిరిగి CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  ఒకవేళ(!sdata)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_దత్తాంశం(session, sdata, &sdata_len);
  earlydata_max = gnutls_రికార్డు_get_max_early_దత్తాంశం_కొలత(session);

  CURL_TRC_CF(దత్తాంశం, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  ఒకవేళ(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((స్థిరరాశి అక్షరం *)quic_tp, quic_tp_len);
    ఒకవేళ(!qtp_clone) {
      curlx_free(sdata);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }

  ఫలితం = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  ఒకవేళ(!ఫలితం && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    ఫలితం = Curl_ssl_session_dup(sc_session, &sc_dup);
  ఒకవేళ(!ఫలితం) {
    ఫలితం = Curl_ssl_scache_put(cf, దత్తాంశం, ssl_peer_కీలకం, sc_session);
    /* took ownership of `sc_session` */
    sc_session = ఖాళీ;
  }
  ఒకవేళ(!ఫలితం && psession) {
    *psession = sc_dup;
    sc_dup = ఖాళీ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
స్థిర CURLcode cf_gtls_update_session_id(నిర్మాణం Curl_cfilter *cf,
                                          నిర్మాణం Curl_easy *దత్తాంశం,
                                          gnutls_session_t session)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం Curl_ssl_session *scs = ఖాళీ;
  CURLcode ఫలితం;

  ఫలితం = Curl_gtls_cache_session(cf, దత్తాంశం, connssl->peer.scache_కీలకం,
                                   session, 0, connssl->negotiated.alpn,
                                   ఖాళీ, 0, &scs);
  ఒకవేళ(!ఫలితం) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం gtls_handshake_cb(gnutls_session_t session, గుర్తులేని పూర్ణాంకం htype,
                             గుర్తులేని when, గుర్తులేని పూర్ణాంకం incoming,
                             స్థిరరాశి gnutls_datum_t *సందేశం)
{
  నిర్మాణం Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (శూన్య)సందేశం;
  (శూన్య)incoming;
  ఒకవేళ(when) { /* after message has been processed */
    నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
    ఒకవేళ(దత్తాంశం) {
      CURL_TRC_CF(దత్తాంశం, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      ఎంపిక(htype) {
      స్థితి GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, దత్తాంశం, session);
        ఆపు;
      }
      అప్రమేయ:
        ఆపు;
      }
    }
  }
  తిరిగి 0;
}
#అయితే_ముగింపు

స్థిర CURLcode gtls_సమితి_ప్రాధాన్యత(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం gtls_ctx *gtls,
                                  స్థిరరాశి అక్షరం *ప్రాధాన్యత)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం dynbuf buf;
  స్థిరరాశి అక్షరం *err = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం rc;

  curlx_dyn_init(&buf, 4096);

  ఒకవేళ(conn_config->cipher_list) {
    ఒకవేళ((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      ఒకవేళ(!curlx_dyn_len(&buf)) {  /* not added yet */
        ఫలితం = curlx_dyn_కూడు(&buf, ప్రాధాన్యత);
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
      }
      ఫలితం = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      ప్రాధాన్యత = curlx_dyn_ptr(&buf);
    }
    లేకపోతే /* replace our own completely */
      ప్రాధాన్యత = conn_config->cipher_list;
  }

  infof(దత్తాంశం, "GnuTLS priority: %s", ప్రాధాన్యత);
  rc = gnutls_ప్రాధాన్యత_సమితి_ప్రత్యక్ష(gtls->session, ప్రాధాన్యత, &err);
  ఒకవేళ(rc != GNUTLS_E_SUCCESS) {
    failf(దత్తాంశం, "Error %d setting GnuTLS priority: %s", rc, err);
    ఫలితం = CURLE_SSL_CONNECT_ERROR;
  }

బహిర్గతం:
  curlx_dyn_free(&buf);
  తిరిగి ఫలితం;
}

స్థిర CURLcode gtls_client_init(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ssl_peer *peer,
                                 size_t earlydata_max,
                                 నిర్మాణం gtls_ctx *gtls)
{
  నిర్మాణం ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, దత్తాంశం);
  గుర్తులేని పూర్ణాంకం init_flags;
  పూర్ణాంకం rc;
  స్థిరరాశి అక్షరం *prioritylist;
  bool tls13support;
  CURLcode ఫలితం;

  ఒకవేళ(config->సంస్కరణ_2 == CURL_SSLVERSION_SSLv2 ||
     config->సంస్కరణ_2 == CURL_SSLVERSION_SSLv3) {
    failf(దత్తాంశం, "GnuTLS does not support SSLv2 or SSLv3");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  ఫలితం = Curl_gtls_shared_creds_create(దత్తాంశం, &gtls->shared_creds);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
  ఒకవేళ(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_దత్తాంశం | GNUTLS_NO_చివర_యొక్క_EARLY_దత్తాంశం;
  లేకపోతే ఒకవేళ(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_దత్తాంశం;
#అయితే_ముగింపు

#నిర్వచితమైతే GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#అయితే_ముగింపు

#నిర్వచితమైతే GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#అయితే_ముగింపు

#నిర్వచితమైతే GNUTLS_NO_స్థితి_REQUEST
  ఒకవేళ(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_స్థితి_REQUEST;
#అయితే_ముగింపు

  CURL_TRC_CF(దత్తాంశం, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  ఒకవేళ(rc != GNUTLS_E_SUCCESS) {
    failf(దత్తాంశం, "gnutls_init() failed: %d", rc);
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  ఒకవేళ(peer->sni) {
    ఒకవేళ(gnutls_server_name_సమితి(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, అక్షరశ్రేణి_పొడవు(peer->sni)) < 0) {
      failf(దత్తాంశం, "Failed to set SNI");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_సమితి_default_ప్రాధాన్యత(gtls->session);
  ఒకవేళ(rc != GNUTLS_E_SUCCESS)
    తిరిగి CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_సంస్కరణ("3.6.5");

  ఒకవేళ(config->సంస్కరణ_2 == CURL_SSLVERSION_TLSv1_3) {
    ఒకవేళ(!tls13support) {
      failf(దత్తాంశం, "This GnuTLS installation does not support TLS 1.3");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  ఫలితం = gnutls_సమితి_ssl_సంస్కరణ_min_max(దత్తాంశం, peer,
                                          config, &prioritylist, tls13support);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = gtls_సమితి_ప్రాధాన్యత(cf, దత్తాంశం, gtls, prioritylist);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(config->clientcert) {
    ఒకవేళ(!gtls->shared_creds->trust_అమరిక) {
      ఫలితం = Curl_gtls_client_trust_అమరిక(cf, దత్తాంశం, gtls);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    ఒకవేళ(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_సమితి_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->కీలకం_passwd ? config->కీలకం_passwd : "");
      ఒకవేళ(rc != GNUTLS_E_SUCCESS) {
        failf(దత్తాంశం,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        తిరిగి CURLE_SSL_CONNECT_ERROR;
      }
    }
    లేకపోతే {
      స్థిరరాశి గుర్తులేని పూర్ణాంకం supported_కీలకం_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_సమితి_x509_కీలకం_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->కీలకం ? config->కీలకం : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->కీలకం_passwd,
           supported_కీలకం_encryption_algorithms);
      ఒకవేళ(rc != GNUTLS_E_SUCCESS) {
        failf(దత్తాంశం,
              "error reading X.509 %skey file: %s",
              config->కీలకం_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        తిరిగి CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_సమితి(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  ఒకవేళ(rc != GNUTLS_E_SUCCESS) {
    failf(దత్తాంశం, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  ఒకవేళ(config->verifystatus) {
    rc = gnutls_ocsp_స్థితి_request_enable_client(gtls->session,
                                                  ఖాళీ, 0, ఖాళీ);
    ఒకవేళ(rc != GNUTLS_E_SUCCESS) {
      failf(దత్తాంశం, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
  }

  తిరిగి CURLE_OK;
}

#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
స్థిర పూర్ణాంకం keylog_callback(gnutls_session_t session, స్థిరరాశి అక్షరం *label,
                           స్థిరరాశి gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  ఒకవేళ(crandom.కొలత != 32) {
    తిరిగి -1;
  }

  Curl_tls_keylog_write(label, crandom.దత్తాంశం, crandom.కొలత,
                        secret->దత్తాంశం, secret->కొలత);
  తిరిగి 0;
}

స్థిర CURLcode gtls_on_session_reuse(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం alpn_spec *alpns,
                                      నిర్మాణం Curl_ssl_session *scs,
                                      bool *do_early_దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;

  connssl->earlydata_max =
    gnutls_రికార్డు_get_max_early_దత్తాంశం_కొలత(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  తిరిగి Curl_on_session_reuse(cf, దత్తాంశం, alpns, scs, do_early_దత్తాంశం,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#అయితే_ముగింపు

స్థిర CURLcode gtls_apply_session(
  నిర్మాణం gtls_ctx *gctx,
  నిర్మాణం Curl_cfilter *cf,
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం ssl_peer *peer,
  నిర్మాణం alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  నిర్మాణం Curl_ssl_session *scs,
  bool *pwas_అమరిక)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం rc;

  ఒకవేళ(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_కలిగియుంది_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    ఫలితం = gtls_client_init(cf, దత్తాంశం, peer, scs->earlydata_max, gctx);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    *pwas_అమరిక = TRUE;

    rc = gnutls_session_సమితి_దత్తాంశం(gctx->session, scs->sdata, scs->sdata_len);
    ఒకవేళ(rc < 0)
      infof(దత్తాంశం, "SSL session not accepted by GnuTLS, continuing without");
    లేకపోతే {
      infof(దత్తాంశం, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      ఒకవేళ(conn_config->earlydata && scs->alpn &&
         !cf->conn->బిట్లు.connect_only) {
        bool do_early_దత్తాంశం = FALSE;
        ఒకవేళ(sess_reuse_cb) {
          ఫలితం = sess_reuse_cb(cf, దత్తాంశం, alpns, scs, &do_early_దత్తాంశం);
          ఒకవేళ(ఫలితం)
            వెళ్ళు బహిర్గతం;
        }
        ఒకవేళ(do_early_దత్తాంశం) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_గమ్యం(alpns, scs->alpn);
        }
      }
    }
  }
బహిర్గతం:
  తిరిగి ఫలితం;
}

CURLcode Curl_gtls_ctx_init(నిర్మాణం gtls_ctx *gctx,
                            నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            నిర్మాణం ssl_peer *peer,
                            స్థిరరాశి నిర్మాణం alpn_spec *alpns_requested,
                            Curl_gtls_ctx_అమరిక_cb *cb_అమరిక,
                            శూన్య *cb_user_దత్తాంశం,
                            శూన్య *ssl_user_దత్తాంశం,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం Curl_ssl_session *scs = ఖాళీ;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_లెక్క = 0;
  bool gtls_session_అమరిక = FALSE;
  నిర్మాణం alpn_spec alpns;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  ఒకవేళ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(దత్తాంశం, cf->cft, FIRSTSOCKET);
    ఒకవేళ(scs) {
      ఫలితం = gtls_apply_session(gctx, cf, దత్తాంశం, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_అమరిక);
      scs = ఖాళీ;
      ఒకవేళ(!ఫలితం && gtls_session_అమరిక) {
        CURL_TRC_CF(దత్తాంశం, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  ఒకవేళ(!gtls_session_అమరిక && conn_config->cache_session &&
     !conn_config->verifystatus) {
    ఫలితం = Curl_ssl_scache_take(cf, దత్తాంశం, peer->scache_కీలకం, &scs);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఫలితం = gtls_apply_session(gctx, cf, దత్తాంశం, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_అమరిక);
  }

  ఒకవేళ(!gtls_session_అమరిక) {
    ఫలితం = gtls_client_init(cf, దత్తాంశం, peer, 0, gctx);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  gnutls_session_సమితి_ptr(gctx->session, ssl_user_దత్తాంశం);

  ఒకవేళ(cb_అమరిక) {
    ఫలితం = cb_అమరిక(cf, దత్తాంశం, cb_user_దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  ఒకవేళ(Curl_tls_keylog_enabled()) {
    gnutls_session_సమితి_keylog_function(gctx->session, keylog_callback);
  }
#అయితే_ముగింపు

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  ఒకవేళ(!gtls_alpns_లెక్క && alpns.లెక్క) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.లెక్క);
    కొరకు(i = 0; i < alpns.లెక్క; ++i) {
      gtls_alpns[i].దత్తాంశం = (గుర్తులేని అక్షరం *)alpns.entries[i];
      gtls_alpns[i].కొలత = (గుర్తులేని పూర్ణాంకం)అక్షరశ్రేణి_పొడవు(alpns.entries[i]);
    }
    gtls_alpns_లెక్క = alpns.లెక్క;
  }

  ఒకవేళ(gtls_alpns_లెక్క &&
     gnutls_alpn_సమితి_protocols(gctx->session,
                               gtls_alpns, (గుర్తులేని పూర్ణాంకం)gtls_alpns_లెక్క,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(దత్తాంశం, "failed setting ALPN");
    ఫలితం = CURLE_SSL_CONNECT_ERROR;
  }

బహిర్గతం:
  Curl_ssl_scache_return(cf, దత్తాంశం, peer->scache_కీలకం, scs);
  తిరిగి ఫలితం;
}

స్థిర CURLcode gtls_connect_step1(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం;

  DEBUGASSERT(backend);

  ఒకవేళ(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    తిరిగి CURLE_OK;

  ఫలితం = Curl_gtls_ctx_init(&backend->gtls, cf, దత్తాంశం, &connssl->peer,
                              connssl->alpn, ఖాళీ, ఖాళీ, cf,
#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
                              gtls_on_session_reuse
#లేకపోతే
                              ఖాళీ
#అయితే_ముగింపు
                              );

  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    నిర్మాణం alpn_proto_buf proto;
    memset(&proto, 0, పరిమాణం(proto));
    Curl_alpn_గమ్యం_proto_str(&proto, connssl->alpn);
    infof(దత్తాంశం, VTLS_INFOF_ALPN_OFFER_1STR, proto.దత్తాంశం);
  }

#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
  gnutls_handshake_సమితి_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#అయితే_ముగింపు

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_సమితి_ptr(backend->gtls.session, cf);
  gnutls_transport_సమితి_నెట్టు_function(backend->gtls.session, gtls_నెట్టు);
  gnutls_transport_సమితి_pull_function(backend->gtls.session, gtls_pull);

  తిరిగి CURLE_OK;
}

స్థిర CURLcode pkp_pin_peer_pubkey(నిర్మాణం Curl_easy *దత్తాంశం,
                                    gnutls_x509_crt_t cert,
                                    స్థిరరాశి అక్షరం *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  గుర్తులేని అక్షరం *buff1 = ఖాళీ;

  gnutls_pubkey_t కీలకం = ఖాళీ;

  /* Result is returned to caller */
  CURLcode ఫలితం = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ఒకవేళ(!pinnedpubkey)
    తిరిగి CURLE_OK;

  ఒకవేళ(!cert)
    తిరిగి ఫలితం;

  చేయి {
    పూర్ణాంకం ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&కీలకం);
    ఒకవేళ(ret < 0)
      ఆపు; /* failed */

    ret = gnutls_pubkey_import_x509(కీలకం, cert, 0);
    ఒకవేళ(ret < 0)
      ఆపు; /* failed */

    ret = gnutls_pubkey_export(కీలకం, GNUTLS_X509_FMT_DER, ఖాళీ, &len1);
    ఒకవేళ(ret != GNUTLS_E_SHORT_జ్ఞాపకం_బఫరు || len1 == 0)
      ఆపు; /* failed */

    buff1 = curlx_malloc(len1);
    ఒకవేళ(!buff1)
      ఆపు; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(కీలకం, GNUTLS_X509_FMT_DER, buff1, &len2);
    ఒకవేళ(ret < 0 || len1 != len2)
      ఆపు; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    ఫలితం = Curl_pin_peer_pubkey(దత్తాంశం, pinnedpubkey, buff1, len1);
  } అయితే(0);

  ఒకవేళ(కీలకం)
    gnutls_pubkey_deinit(కీలకం);

  curlx_safefree(buff1);

  తిరిగి ఫలితం;
}

శూన్య Curl_gtls_report_handshake(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం gtls_ctx *gctx)
{
#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    స్థిరరాశి అక్షరం *ptr;
    gnutls_protocol_t సంస్కరణ_2 = gnutls_protocol_get_సంస్కరణ(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(దత్తాంశం, "SSL connection using %s / %s",
          gnutls_protocol_get_name(సంస్కరణ_2), ptr);
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)gctx;
#అయితే_ముగింపు
}

స్థిర శూన్య gtls_సందేశం_verify_ఫలితం(నిర్మాణం Curl_easy *దత్తాంశం,
                                   నిర్మాణం ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  అక్షరం certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t కొలత = పరిమాణం(certname);
  పూర్ణాంకం rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &కొలత);
  ఒకవేళ(rc) {
    infof(దత్తాంశం, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  ఒకవేళ(!was_verified) {
    ఒకవేళ(needs_verified) {
      failf(దత్తాంశం, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    లేకపోతే
      infof(దత్తాంశం, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  లేకపోతే
    infof(దత్తాంశం, "  common name: %s (matched)", certname);
}

స్థిర శూన్య gtls_infof_cert(నిర్మాణం Curl_easy *దత్తాంశం,
                            gnutls_x509_crt_t x509_cert)
{
#నిర్వచితమైతే CURLVERBOSE
  ఒకవేళ(Curl_trc_is_verbose(దత్తాంశం)) {
    gnutls_datum_t certfields;
    పూర్ణాంకం rc, algo;
    time_t tstamp;
    గుర్తులేని పూర్ణాంకం బిట్లు;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &బిట్లు);
    infof(దత్తాంశం, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(దత్తాంశం, "  certificate version: #%d",
          gnutls_x509_crt_get_సంస్కరణ(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    ఒకవేళ(rc)
      infof(దత్తాంశం, "Failed to get certificate name");
    లేకపోతే {
      infof(దత్తాంశం, "  subject: %s", certfields.దత్తాంశం);

      tstamp = gnutls_x509_crt_get_activation_సమయం(x509_cert);
      showtime(దత్తాంశం, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_సమయం(x509_cert);
      showtime(దత్తాంశం, "expire date", tstamp);

      gnutls_free(certfields.దత్తాంశం);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    ఒకవేళ(rc)
      infof(దత్తాంశం, "Failed to get certificate issuer");
    లేకపోతే {
      infof(దత్తాంశం, "  issuer: %s", certfields.దత్తాంశం);

      gnutls_free(certfields.దత్తాంశం);
    }
  }
#లేకపోతే
  (శూన్య)దత్తాంశం;
  (శూన్య)x509_cert;
#అయితే_ముగింపు
}

స్థిర CURLcode gtls_verify_ocsp_స్థితి(నిర్మాణం Curl_easy *దత్తాంశం,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = ఖాళీ;
  CURLcode ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
  పూర్ణాంకం rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_స్థితి_t స్థితి_2 = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_స్థితి_request_get(session, &ocsp_raw);
  ఒకవేళ(rc == GNUTLS_E_REQUESTED_దత్తాంశం_NOT_అందుబాటు) {
    failf(దత్తాంశం, "No OCSP response received");
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(rc < 0) {
    failf(దత్తాంశం, "Invalid OCSP response received");
    వెళ్ళు బహిర్గతం;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  ఒకవేళ(rc < 0) {
    failf(దత్తాంశం, "failed to init OCSP response parsing");
    వెళ్ళు బహిర్గతం;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  ఒకవేళ(rc < 0) {
    failf(దత్తాంశం, "failed to parse OCSP response");
    వెళ్ళు బహిర్గతం;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  ఒకవేళ(gnutls_ocsp_resp_get_స్థితి(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(దత్తాంశం, "OCSP response basic part is not successful");
    వెళ్ళు బహిర్గతం;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  కొరకు(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    ఒకవేళ(!rc || (rc == GNUTLS_E_REQUESTED_దత్తాంశం_NOT_అందుబాటు))
      ఆపు;
  }
  ఒకవేళ(rc < 0) {
    failf(దత్తాంశం, "OCSP does not carry status for peer cert");
    వెళ్ళు బహిర్గతం;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ,
                                   &స్థితి_2, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ);
  ఒకవేళ(rc < 0) {
    failf(దత్తాంశం, "failed to parse OCSP response for peer cert");
    వెళ్ళు బహిర్గతం;
  }
  ఎంపిక(స్థితి_2) {
  స్థితి GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    ఫలితం = CURLE_OK;
    ఆపు;
  స్థితి GNUTLS_OCSP_CERT_REVOKED:
    failf(దత్తాంశం, "peer certificate is revoked");
    ఆపు;
  అప్రమేయ:
    failf(దత్తాంశం, "peer certificate OCSP status is not good: %d", (పూర్ణాంకం)స్థితి_2);
    ఆపు;
  }

బహిర్గతం:
  ఒకవేళ(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  తిరిగి ఫలితం;
}

నిర్మాణం gtls_cert_chain {
  స్థిరరాశి gnutls_datum_t *certs;
  గుర్తులేని పూర్ణాంకం అంకె_certs;
};

#నిర్వచితమైతే USE_APPLE_SECTRUST
స్థిర CURLcode gtls_chain_get_der(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   శూన్య *user_దత్తాంశం,
                                   size_t i,
                                   గుర్తులేని అక్షరం **pder,
                                   size_t *pder_len)
{
  నిర్మాణం gtls_cert_chain *chain = user_దత్తాంశం;

  (శూన్య)cf;
  (శూన్య)దత్తాంశం;
  *pder_len = 0;
  *pder = ఖాళీ;

  ఒకవేళ(i >= chain->అంకె_certs)
    తిరిగి CURLE_TOO_LARGE;
  *pder = chain->certs[i].దత్తాంశం;
  *pder_len = (size_t)chain->certs[i].కొలత;
  తిరిగి CURLE_OK;
}
#అయితే_ముగింపు /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
స్థిర CURLcode gtls_verify_cert(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం ssl_filter_config *config,
                                 నిర్మాణం ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం ssl_peer *peer,
                                 నిర్మాణం gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  గుర్తులేని పూర్ణాంకం verify_స్థితి = 0;
  దీర్ఘ * స్థిరరాశి certverifyresult = &ssl_config->certverifyresult;
  పూర్ణాంకం rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_స్థితి);
  ఒకవేళ(rc < 0) {
    failf(దత్తాంశం, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_స్థితి;
  verified = !(verify_స్థితి & GNUTLS_CERT_INVALID);
  ఒకవేళ(verified)
    infof(దత్తాంశం, "  SSL certificate verified by GnuTLS");

#నిర్వచితమైతే USE_APPLE_SECTRUST
  ఒకవేళ(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode ఫలితం;

    ఒకవేళ(gnutls_ocsp_స్థితి_request_get(session, &ocsp_raw)) {
      ocsp_raw.దత్తాంశం = ఖాళీ;
      ocsp_raw.కొలత = 0;
    }
    ఒకవేళ(config->verifystatus && !ocsp_raw.కొలత) {
      failf(దత్తాంశం, "No OCSP response received");
      తిరిగి CURLE_SSL_INVALIDCERTSTATUS;
    }
    ఫలితం = Curl_vtls_apple_verify(cf, దత్తాంశం, peer, chain->అంకె_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.దత్తాంశం, ocsp_raw.కొలత,
                                    config->verifystatus);
    ఒకవేళ(ఫలితం && (ఫలితం != CURLE_PEER_FAILED_VERIFICATION))
      తిరిగి ఫలితం; /* no further failure analysis needed */
    verified = !ఫలితం;
    ఒకవేళ(verified) {
      infof(దత్తాంశం, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#లేకపోతే
  (శూన్య)cf;
  (శూన్య)peer;
  (శూన్య)chain;
#అయితే_ముగింపు

  ఒకవేళ(!verified) {
    CURLcode ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    స్థిరరాశి అక్షరం *cause = "certificate error, no details available";
    ఒకవేళ(verify_స్థితి & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    లేకపోతే ఒకవేళ(verify_స్థితి & GNUTLS_CERT_SIGNER_NOT_దొరికింది)
      cause = "certificate signer not trusted";
    లేకపోతే ఒకవేళ(verify_స్థితి & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    లేకపోతే ఒకవేళ(verify_స్థితి & GNUTLS_CERT_INVALID_OCSP_స్థితి) {
      cause = "OCSP status response is invalid";
      ఫలితం = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(దత్తాంశం, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    తిరిగి ఫలితం;
  }
  తిరిగి CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                gnutls_session_t session,
                                నిర్మాణం ssl_filter_config *config,
                                నిర్మాణం ssl_easy_config *ssl_config,
                                నిర్మాణం ssl_peer *peer,
                                స్థిరరాశి అక్షరం *pinned_కీలకం)
{
  నిర్మాణం gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = ఖాళీ, x509_issuer = ఖాళీ;
  time_t certclock;
  bool ocsp_checked = FALSE;
  పూర్ణాంకం rc;
  CURLcode ఫలితం = CURLE_OK;
  దీర్ఘ * స్థిరరాశి certverifyresult = &ssl_config->certverifyresult;

  (శూన్య)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.అంకె_certs);
  ఒకవేళ(!chain.certs) {
    ఒకవేళ(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(దత్తాంశం, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_దొరికింది;
      ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
      వెళ్ళు బహిర్గతం;
    }
    infof(దత్తాంశం, " common name: WARNING could not obtain");
  }

  ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo && chain.certs) {
    ఒకవేళ(chain.అంకె_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(దత్తాంశం, "%u certificates is more than allowed (%d)",
            chain.అంకె_certs, MAX_ALLOWED_CERT_AMOUNT);
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే {
      గుర్తులేని పూర్ణాంకం i;

      ఫలితం = Curl_ssl_init_certinfo(దత్తాంశం, (పూర్ణాంకం)chain.అంకె_certs);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;

      కొరకు(i = 0; i < chain.అంకె_certs; i++) {
        స్థిరరాశి uint8_t *beg = chain.certs[i].దత్తాంశం;
        స్థిరరాశి uint8_t *చివర = beg + chain.certs[i].కొలత;

        ఫలితం = Curl_వెలికితీయి_certinfo(దత్తాంశం, (పూర్ణాంకం)i, beg, చివర);
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
      }
    }
  }

  ఒకవేళ(config->verifypeer) {
    ఫలితం = gtls_verify_cert(దత్తాంశం, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  లేకపోతే
    infof(దత్తాంశం, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  ఒకవేళ(gnutls_x509_crt_init(&x509_cert)) {
    failf(దత్తాంశం, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_దొరికింది;
    ఫలితం = CURLE_SSL_CONNECT_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    ఒకవేళ(rc) {
      failf(దత్తాంశం, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_దొరికింది;
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_సమయం(x509_cert);

  ఒకవేళ(certclock == (time_t)-1) {
    ఒకవేళ(config->verifypeer) {
      failf(దత్తాంశం, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే
      infof(దత్తాంశం, "  SSL certificate expiration date verify FAILED");
  }
  లేకపోతే {
    ఒకవేళ(certclock < సమయం(ఖాళీ)) {
      ఒకవేళ(config->verifypeer) {
        failf(దత్తాంశం, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
        వెళ్ళు బహిర్గతం;
      }
      లేకపోతే
        infof(దత్తాంశం, "  SSL certificate expiration date FAILED");
    }
    లేకపోతే
      infof(దత్తాంశం, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_సమయం(x509_cert);

  ఒకవేళ(certclock == (time_t)-1) {
    ఒకవేళ(config->verifypeer) {
      failf(దత్తాంశం, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే
      infof(దత్తాంశం, "  SSL certificate activation date verify FAILED");
  }
  లేకపోతే {
    ఒకవేళ(certclock > సమయం(ఖాళీ)) {
      ఒకవేళ(config->verifypeer) {
        failf(దత్తాంశం, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        ఫలితం = CURLE_PEER_FAILED_VERIFICATION;
        వెళ్ళు బహిర్గతం;
      }
      లేకపోతే
        infof(దత్తాంశం, "  SSL certificate activation date FAILED");
    }
    లేకపోతే
      infof(దత్తాంశం, "  SSL certificate activation date OK");
  }

  ఒకవేళ(config->verifystatus && !ocsp_checked) {
    ఫలితం = gtls_verify_ocsp_స్థితి(దత్తాంశం, session, x509_cert);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  లేకపోతే
    infof(దత్తాంశం, "  SSL certificate status verification SKIPPED");

  ఒకవేళ(config->issuercert) {
    gnutls_datum_t issuerp;
    ఒకవేళ(gnutls_x509_crt_init(&x509_issuer)) {
      failf(దత్తాంశం, "failed to init GnuTLS x509_crt for issuer");
      ఫలితం = CURLE_SSL_ISSUER_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    issuerp = ఎక్కించు_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    ఒకవేళ(rc) {
      failf(దత్తాంశం, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      ఫలితం = CURLE_SSL_ISSUER_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    rc = (పూర్ణాంకం)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    ఒకవేళ(rc <= 0) {
      failf(దత్తాంశం, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      ఫలితం = CURLE_SSL_ISSUER_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    infof(దత్తాంశం, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  rc = (పూర్ణాంకం)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  ఫలితం = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_సందేశం_verify_ఫలితం(దత్తాంశం, peer, x509_cert, rc, config->verifyhost);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(pinned_కీలకం) {
    ఫలితం = pkp_pin_peer_pubkey(దత్తాంశం, x509_cert, pinned_కీలకం);
    ఒకవేళ(ఫలితం != CURLE_OK) {
      failf(దత్తాంశం, "SSL: public key does not match pinned public key");
      వెళ్ళు బహిర్గతం;
    }
  }

  gtls_infof_cert(దత్తాంశం, x509_cert);

బహిర్గతం:
  ఒకవేళ(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  ఒకవేళ(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  తిరిగి ఫలితం;
}

స్థిర CURLcode gtls_verifyserver(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  gnutls_session_t session)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, దత్తాంశం);
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థిరరాశి అక్షరం *pinned_కీలకం = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY);
#లేకపోతే
  స్థిరరాశి అక్షరం *pinned_కీలకం = CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY);
#అయితే_ముగింపు
  CURLcode ఫలితం;

  ఫలితం = Curl_gtls_verifyserver(cf, దత్తాంశం, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_కీలకం);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  ఒకవేళ(gnutls_protocol_get_సంస్కరణ(session) < GNUTLS_TLS1_3)
    ఫలితం = cf_gtls_update_session_id(cf, దత్తాంశం, session);
#అయితే_ముగింపు

బహిర్గతం:
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
స్థిర CURLcode gtls_send_earlydata(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి గుర్తులేని అక్షరం *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_ఫలితం = CURLE_OK;
  అయితే(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_రికార్డు_send_early_దత్తాంశం(backend->gtls.session, buf, blen);
    CURL_TRC_CF(దత్తాంశం, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    ఒకవేళ(n < 0) {
      ఒకవేళ(n == GNUTLS_E_AGAIN)
        ఫలితం = CURLE_AGAIN;
      లేకపోతే
        ఫలితం = backend->gtls.io_ఫలితం ?
                 backend->gtls.io_ఫలితం : CURLE_SEND_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(దత్తాంశం, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
బహిర్గతం:
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు

/*
 * This function is called after the TCP connect has completed. Setup the TLS
 * layer and do all necessary magic.
 */
/* We use connssl->connecting_state to keep track of the connection status;
   there are three states: 'ssl_connect_1' (not started yet or complete),
   'ssl_connect_2' (doing handshake with the server), and
   'ssl_connect_3' (verifying and getting stats).
 */
స్థిర CURLcode gtls_connect_common(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  ఒకవేళ(ssl_connection_complete == connssl->state) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;

  /* Initiate the connection, if not already done */
  ఒకవేళ(connssl->connecting_state == ssl_connect_1) {
    ఫలితం = gtls_connect_step1(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    connssl->connecting_state = ssl_connect_2;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_2) {
#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
    ఒకవేళ(connssl->earlydata_state == ssl_earlydata_await) {
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(connssl->earlydata_state == ssl_earlydata_sending) {
      ఫలితం = gtls_send_earlydata(cf, దత్తాంశం);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      connssl->earlydata_state = ssl_earlydata_పంపిన;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_పంపిన));
#అయితే_ముగింపు
    ఫలితం = cf_gtls_handshake(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  ఒకవేళ(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    పూర్ణాంకం rc;

    Curl_gtls_report_handshake(దత్తాంశం, &backend->gtls);

    ఫలితం = gtls_verifyserver(cf, దత్తాంశం, backend->gtls.session);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    ఒకవేళ(rc) {  /* No ALPN from server */
      proto.దత్తాంశం = ఖాళీ;
      proto.కొలత = 0;
    }

    ఫలితం = Curl_alpn_సమితి_negotiated(cf, దత్తాంశం, connssl,
                                      proto.దత్తాంశం, proto.కొలత);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;

#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
    ఒకవేళ(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_పంపిన);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_దత్తాంశం) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#అయితే_ముగింపు
    connssl->connecting_state = ssl_connect_పూర్తి;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_పూర్తి)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

బహిర్గతం:
  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    *పూర్తి = FALSE;
    తిరిగి CURLE_OK;
  }
  *పూర్తి = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(దత్తాంశం, cf, "gtls_connect_common() -> %d, done=%d", (పూర్ణాంకం)ఫలితం,
              *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర CURLcode gtls_connect(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             bool *పూర్తి)
{
#నిర్వచితమైతే CURL_GNUTLS_EARLY_దత్తాంశం
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  ఒకవేళ((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }
#అయితే_ముగింపు
  తిరిగి gtls_connect_common(cf, దత్తాంశం, పూర్తి);
}

స్థిర bool gtls_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                              స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *ctx = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)ctx->backend;
  ఒకవేళ(backend->gtls.session &&
     gnutls_రికార్డు_check_pending(backend->gtls.session) != 0)
    తిరిగి TRUE;
  తిరిగి FALSE;
}

స్థిర CURLcode gtls_send(నిర్మాణం Curl_cfilter *cf,
                          నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి శూన్య *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  అయితే(remain) {
    backend->gtls.io_ఫలితం = CURLE_OK;
    nwritten = gnutls_రికార్డు_send(backend->gtls.session, buf, remain);

    ఒకవేళ(nwritten >= 0) {
      ఒకవేళ((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(దత్తాంశం, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        తిరిగి CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (అక్షరం *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    లేకపోతే {
      ఒకవేళ(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        ఫలితం = CURLE_OK;
        వెళ్ళు బహిర్గతం;
      }
      ఫలితం = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_ఫలితం ? backend->gtls.io_ఫలితం : CURLE_SEND_ERROR);
      వెళ్ళు బహిర్గతం;
    }
  }

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (పూర్ణాంకం)ఫలితం, *pnwritten);
  తిరిగి ఫలితం;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
స్థిర CURLcode gtls_shutdown(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              bool send_shutdown, bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  అక్షరం buf[1024];
  CURLcode ఫలితం = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  ఒకవేళ(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *పూర్తి = TRUE;
    వెళ్ళు బహిర్గతం;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *పూర్తి = FALSE;

  ఒకవేళ(!backend->gtls.పంపిన_shutdown) {
    /* do this only once */
    backend->gtls.పంపిన_shutdown = TRUE;
    ఒకవేళ(send_shutdown) {
      పూర్ణాంకం ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      ఒకవేళ((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_రికార్డు_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.పంపిన_shutdown = FALSE;
        ఫలితం = CURLE_OK;
        వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((పూర్ణాంకం)ret), (పూర్ణాంకం)ret);
        ఫలితం = CURLE_RECV_ERROR;
        వెళ్ళు బహిర్గతం;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  కొరకు(i = 0; i < 10; ++i) {
    nread = gnutls_రికార్డు_recv(backend->gtls.session, buf, పరిమాణం(buf));
    ఒకవేళ(nread <= 0)
      ఆపు;
  }
  ఒకవేళ(nread > 0) {
    /* still data coming in? */
  }
  లేకపోతే ఒకవేళ(nread == 0) {
    /* We got the close notify alert and are done. */
    *పూర్తి = TRUE;
  }
  లేకపోతే ఒకవేళ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_రికార్డు_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  లేకపోతే {
    CURL_TRC_CF(దత్తాంశం, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((పూర్ణాంకం)nread), (పూర్ణాంకం)nread);
    ఫలితం = CURLE_RECV_ERROR;
  }

బహిర్గతం:
  cf->shutdown = (ఫలితం || *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర శూన్య gtls_close(నిర్మాణం Curl_cfilter *cf,
                       నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(దత్తాంశం, cf, "close");
  ఒకవేళ(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = ఖాళీ;
  }
  ఒకవేళ(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

స్థిర CURLcode gtls_recv(నిర్మాణం Curl_cfilter *cf,
                          నిర్మాణం Curl_easy *దత్తాంశం,
                          అక్షరం *buf, size_t blen,
                          size_t *pnread)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_రికార్డు_recv(backend->gtls.session, buf, blen);

  ఒకవేళ(nread >= 0)
    *pnread = (size_t)nread;
  లేకపోతే {
    ఒకవేళ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      ఫలితం = CURLE_AGAIN;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      ఫలితం = cf_gtls_handshake(cf, దత్తాంశం);
      ఒకవేళ(!ఫలితం)
        ఫలితం = CURLE_AGAIN; /* make us get called again. */
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే {
      failf(దత్తాంశం, "GnuTLS recv error (%d): %s",
            (పూర్ణాంకం)nread, gnutls_strerror((పూర్ణాంకం)nread));
      ఫలితం = backend->gtls.io_ఫలితం ?
        backend->gtls.io_ఫలితం : CURLE_RECV_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (పూర్ణాంకం)ఫలితం, nread);
  తిరిగి ఫలితం;
}

size_t Curl_gtls_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత)
{
  తిరిగి curl_msnprintf(బఫరు, కొలత, "GnuTLS/%s", gnutls_check_సంస్కరణ(ఖాళీ));
}

/* data might be NULL! */
స్థిర CURLcode gtls_random(నిర్మాణం Curl_easy *దత్తాంశం,
                            గుర్తులేని అక్షరం *entropy, size_t నిడివి)
{
  పూర్ణాంకం rc;
  (శూన్య)దత్తాంశం;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, నిడివి);
  తిరిగి rc ? CURLE_FAILED_INIT : CURLE_OK;
}

స్థిర CURLcode gtls_sha256sum(స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                               size_t len,
                               గుర్తులేని అక్షరం *sha256sum, /* output */
                               size_t sha256len)
{
  నిర్మాణం sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  చేయి {
    గుర్తులేని పూర్ణాంకం ilen = (గుర్తులేని పూర్ణాంకం)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, ఆదానం);
    len -= ilen;
    ఆదానం += ilen;
  } అయితే(len);
#ఒకవేళ NETTLE_సంస్కరణ_MAJOR >= 4
  (శూన్య)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#లేకపోతే
  sha256_digest(&SHA256pw, (గుర్తులేని పూర్ణాంకం)sha256len, sha256sum);
#అయితే_ముగింపు
  తిరిగి CURLE_OK;
}

స్థిర bool gtls_cert_స్థితి_request(శూన్య)
{
  తిరిగి TRUE;
}

స్థిర శూన్య *gtls_get_internals(నిర్మాణం ssl_connect_దత్తాంశం *connssl,
                                CURLINFO info)
{
  నిర్మాణం gtls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం gtls_ssl_backend_దత్తాంశం *)connssl->backend;
  (శూన్య)info;
  DEBUGASSERT(backend);
  తిరిగి backend->gtls.session;
}

స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_gnutls = {
  { CURLSSLBACKEND_GNUTLS, "gnutls" }, /* info */

  SSLSUPP_CA_మార్గం  |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_CA_CACHE |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_CRLFILE,

  పరిమాణం(నిర్మాణం gtls_ssl_backend_దత్తాంశం),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_సంస్కరణ,             /*సంస్కరణ*/
  gtls_shutdown,                 /* shutdown */
  gtls_దత్తాంశం_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_స్థితి_request,      /* cert_status_request */
  gtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,       /* adjust_pollset */
  gtls_get_internals,            /* get_internals */
  gtls_close,                    /* close_one */
  ఖాళీ,                          /* close_all */
  ఖాళీ,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  ఖాళీ,                          /* get_channel_binding */
};

#అయితే_ముగింపు /* USE_GNUTLS */
