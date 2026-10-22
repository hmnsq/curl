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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_GNUTLS

#شامل <gnutls/abstract.h>
#شامل <gnutls/gnutls.h>
#شامل <gnutls/x509.h>
#شامل <gnutls/crypto.h>
#شامل <nettle/sha2.h>
#شامل <nettle/version.h>

#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "vtls/keylog.h"
#شامل "vtls/gtls.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_int.h"
#شامل "vtls/vtls_scache.h"
#شامل "vtls/apple.h"
#شامل "vauth/vauth.h"
#شامل "parsedate.h"
#شامل "connect.h" /* for the connect timeout */
#شامل "progress.h"
#شامل "curlx/strdup.h"
#شامل "curlx/win32-fopen.h"
#شامل "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#اگر 0
#تعریف_کرو GTLSDEBUG
#اگر_ختم

#اگر_تعریف_شدہ GTLSDEBUG
ساکن خالی tls_log_func(صحیح level, ثابت حرف *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#اگر_ختم

#اگر !defined(GNUTLS_نسخہ_نمبر) || (GNUTLS_نسخہ_نمبر < 0x030605)
#غلطی "too old GnuTLS version"
#اگر_ختم

#تعریف_منسوخ CURL_GNUTLS_EARLY_ڈیٹا
#اگر GNUTLS_نسخہ_نمبر >= 0x030702
#تعریف_کرو CURL_GNUTLS_EARLY_ڈیٹا
#اگر_ختم

#شامل <gnutls/ocsp.h>

ساخت gtls_ssl_backend_ڈیٹا {
  ساخت gtls_ctx gtls;
};

ساکن ssize_t gtls_ڈالو(خالی *s, ثابت خالی *buf, size_t blen)
{
  ساخت Curl_cfilter *cf = s;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا);
  نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(ڈیٹا, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (صحیح)نتیجہ, nwritten);
  backend->gtls.io_نتیجہ = نتیجہ;
  اگر(نتیجہ) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_سیٹ_errno(backend->gtls.session,
                               (نتیجہ == CURLE_AGAIN) ? EAGAIN : EINVAL);
    واپس -1;
  }
  واپس (ssize_t)nwritten;
}

ساکن ssize_t gtls_pull(خالی *s, خالی *buf, size_t blen)
{
  ساخت Curl_cfilter *cf = s;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  size_t nread;
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا);
  اگر(!backend->gtls.shared_creds->trust_تیاری) {
    نتیجہ = Curl_gtls_client_trust_تیاری(cf, ڈیٹا, &backend->gtls);
    اگر(نتیجہ) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_سیٹ_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_نتیجہ = نتیجہ;
      واپس -1;
    }
  }

  نتیجہ = Curl_conn_cf_recv(cf->اگلا, ڈیٹا, buf, blen, &nread);
  CURL_TRC_CF(ڈیٹا, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (صحیح)نتیجہ,
              nread);
  backend->gtls.io_نتیجہ = نتیجہ;
  اگر(نتیجہ) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_سیٹ_errno(backend->gtls.session,
                               (نتیجہ == CURLE_AGAIN) ? EAGAIN : EINVAL);
    واپس -1;
  }
  ورنہ اگر(nread == 0)
    connssl->peer_closed = TRUE;
  واپس (ssize_t)nread;
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
ساکن صحیح gtls_init(خالی)
{
  صحیح ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#اگر_تعریف_شدہ GTLSDEBUG
  gnutls_global_سیٹ_log_function(tls_log_func);
  gnutls_global_سیٹ_log_level(2);
#اگر_ختم
  واپس ret;
}

ساکن خالی gtls_cleanup(خالی)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#اگر_تعریف_شدہ CURLVERBOSE
ساکن خالی showtime(ساخت Curl_easy *ڈیٹا, ثابت حرف *متن_3, time_t stamp)
{
  ساخت tm بفر;
  ثابت ساخت tm *tm = &بفر;
  حرف str[96];
  CURLcode نتیجہ = curlx_gmtime(stamp, &بفر);
  اگر(نتیجہ)
    واپس;

  curl_msnprintf(str,
                 حجم(str),
                 "  %s: %s, %02d %s %4d %02d:%02d:%02d GMT",
                 متن_3,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);
  infof(ڈیٹا, "%s", str);
}
#اگر_ختم

ساکن gnutls_datum_t بھرو_file(ثابت حرف *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { تہی, 0 };
  طویل filelen;
  خالی *ptr;

  f = curlx_fopen(file, "rb");
  اگر(!f)
    واپس loaded_file;
  اگر(fseek(f, 0, SEEK_END))
    جاؤ برآمد;
  filelen = ftell(f);
  اگر(filelen < 0 || filelen > CURL_MAX_داخلہ_لمبائی)
    جاؤ برآمد;
  اگر(fseek(f, 0, SEEK_SET))
    جاؤ برآمد;
  ptr = curlx_malloc((size_t)filelen);
  اگر(!ptr)
    جاؤ برآمد;
  اگر(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    جاؤ برآمد;
  }

  loaded_file.ڈیٹا = ptr;
  loaded_file.سائز = (غیرعلامتی صحیح)filelen;
برآمد:
  curlx_fclose(f);
  واپس loaded_file;
}

ساکن خالی unload_file(gnutls_datum_t ڈیٹا)
{
  curlx_free(ڈیٹا.ڈیٹا);
}

/* this function does an SSL/TLS (re-)handshake */
ساکن CURLcode cf_gtls_handshake(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  gnutls_session_t session;
  صحیح rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_نتیجہ = CURLE_OK;
  rc = gnutls_handshake(session);

  اگر(!backend->gtls.shared_creds->trust_تیاری) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode نتیجہ = Curl_gtls_client_trust_تیاری(cf, ڈیٹا, &backend->gtls);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_ریکارڈ_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    واپس CURLE_AGAIN;
  }
  ورنہ اگر((rc < 0) && !gnutls_error_is_fatal(rc)) {
#اگر_تعریف_شدہ CURLVERBOSE
    ثابت حرف *strerr = تہی;

    اگر(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    اگر(!strerr)
      strerr = gnutls_strerror(rc);

    infof(ڈیٹا, "gnutls_handshake() warning: %s", strerr);
#اگر_ختم
    واپس CURLE_AGAIN;
  }
  ورنہ اگر((rc < 0) && backend->gtls.io_نتیجہ) {
    واپس backend->gtls.io_نتیجہ;
  }
  ورنہ اگر(rc < 0) {
    ثابت حرف *strerr = تہی;

    اگر(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    اگر(!strerr)
      strerr = gnutls_strerror(rc);

    failf(ڈیٹا, "GnuTLS, handshake failed: %s", strerr);
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  واپس CURLE_OK;
}

ساکن gnutls_x509_crt_fmt_t gnutls_do_file_type(ثابت حرف *type)
{
  اگر(!type || !type[0])
    واپس GNUTLS_X509_FMT_PEM;
  اگر(curl_strequal(type, "PEM"))
    واپس GNUTLS_X509_FMT_PEM;
  اگر(curl_strequal(type, "DER"))
    واپس GNUTLS_X509_FMT_DER;
  واپس GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#تعریف_کرو GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#تعریف_کرو QUIC_ترجیح \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ساکن CURLcode gnutls_سیٹ_ssl_نسخہ_min_max(
  ساخت Curl_easy *ڈیٹا,
  ساخت ssl_peer *peer,
  ساخت ssl_filter_config *conn_config,
  ثابت حرف **prioritylist,
  bool tls13support)
{
  طویل ssl_نسخہ = conn_config->نسخہ_2;
  طویل ssl_نسخہ_max = conn_config->نسخہ_max;

  DEBUGASSERT(ssl_نسخہ != CURL_SSLVERSION_DEFAULT);
  اگر(ssl_نسخہ <= CURL_SSLVERSION_TLSv1)
    ssl_نسخہ = CURL_SSLVERSION_TLSv1_0;
  اگر((ssl_نسخہ_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_نسخہ_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_نسخہ_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  اگر(peer->transport == TRNSPRT_QUIC) {
    اگر(ssl_نسخہ_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(ڈیٹا, "QUIC needs at least TLS version 1.3");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_ترجیح;
    واپس CURLE_OK;
  }

  انتخاب(ssl_نسخہ | ssl_نسخہ_max) {
  حالت CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    واپس CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    واپس CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    واپس CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    واپس CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    واپس CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    واپس CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    واپس CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    واپس CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    واپس CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    واپس CURLE_OK;
  }

  failf(ڈیٹا, "GnuTLS: cannot set TLS protocol");
  واپس CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(ساخت Curl_easy *ڈیٹا,
                                       ساخت gtls_shared_creds **pcreds)
{
  ساخت gtls_shared_creds *shared;
  صحیح rc;

  *pcreds = تہی;
  shared = curlx_calloc(1, حجم(*shared));
  اگر(!shared)
    واپس CURLE_برآمد_کا_حافظہ;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  اگر(rc != GNUTLS_E_SUCCESS) {
    failf(ڈیٹا, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->وقت = *Curl_pgrs_now(ڈیٹا);
  *pcreds = shared;
  واپس CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_اوپر_ref(ساخت gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  اگر(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    واپس CURLE_OK;
  }
  واپس CURLE_BAD_FUNCTION_دلیل;
}

خالی Curl_gtls_shared_creds_free(ساخت gtls_shared_creds **pcreds)
{
  ساخت gtls_shared_creds *shared = *pcreds;
  *pcreds = تہی;
  اگر(shared) {
    --shared->refcount;
    اگر(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

ساکن CURLcode gtls_populate_creds(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    gnutls_certificate_credentials_t creds)
{
  ساخت ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ساخت ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ڈیٹا);
  bool creds_are_خالی_خانے = TRUE;
  صحیح rc;

  اگر(!config->verifypeer) {
    infof(ڈیٹا, "SSL Trust: peer verification disabled");
    واپس CURLE_OK;
  }

  infof(ڈیٹا, "SSL Trust Anchors:");
  اگر(config->native_ca_store) {
#اگر_تعریف_شدہ USE_APPLE_SECTRUST
    infof(ڈیٹا, "  Native: Apple SecTrust");
    creds_are_خالی_خانے = FALSE;
#ورنہ
    rc = gnutls_certificate_سیٹ_x509_system_trust(creds);
    اگر(rc < 0)
      infof(ڈیٹا, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    ورنہ {
      infof(ڈیٹا, "  Native: %d certificates from system trust", rc);
      اگر(rc > 0)
        creds_are_خالی_خانے = FALSE;
    }
#اگر_ختم
  }

  اگر(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    اگر(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(ڈیٹا, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      واپس CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.ڈیٹا = config->ca_info_blob->ڈیٹا;
    ca_info_datum.سائز = (غیرعلامتی صحیح)config->ca_info_blob->len;
    rc = gnutls_certificate_سیٹ_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_خالی_خانے = creds_are_خالی_خانے && (rc <= 0);
    اگر(rc < 0) {
      infof(ڈیٹا, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_خالی_خانے ? "" : ", continuing anyway"));
      اگر(creds_are_خالی_خانے) {
        ssl_config->certverifyresult = rc;
        واپس CURLE_SSL_CACERT_BADFILE;
      }
    }
    ورنہ
      infof(ڈیٹا, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  ورنہ اگر(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_سیٹ_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_سیٹ_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_خالی_خانے = creds_are_خالی_خانے && (rc <= 0);
    اگر(rc < 0) {
      infof(ڈیٹا, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_خالی_خانے ? "" : ", continuing anyway"));
      اگر(creds_are_خالی_خانے) {
        ssl_config->certverifyresult = rc;
        واپس CURLE_SSL_CACERT_BADFILE;
      }
    }
    ورنہ
      infof(ڈیٹا, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  اگر(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_سیٹ_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_خالی_خانے = creds_are_خالی_خانے && (rc <= 0);
    اگر(rc < 0) {
      infof(ڈیٹا, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_خالی_خانے ? "" : ", continuing anyway"));
      اگر(creds_are_خالی_خانے) {
        ssl_config->certverifyresult = rc;
        واپس CURLE_SSL_CACERT_BADFILE;
      }
    }
    ورنہ
      infof(ڈیٹا, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  اگر(creds_are_خالی_خانے)
    infof(ڈیٹا, "  no trust anchors configured");

#اگر_تعریف_شدہ USE_APPLE_SECTRUST
  اگر(config->CRLfile && config->native_ca_store) {
    failf(ڈیٹا, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    واپس CURLE_NOT_BUILT_اندر;
  }
#اگر_ختم
  اگر(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_سیٹ_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    اگر(rc < 0) {
      failf(ڈیٹا, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      واپس CURLE_SSL_CRL_BADFILE;
    }
    ورنہ
      infof(ڈیٹا, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  واپس CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#تعریف_کرو MPROTO_GTLS_X509_کلید   "tls:gtls:x509:share"

ساکن bool gtls_shared_creds_expired(ساخت Curl_easy *ڈیٹا,
                                      ثابت ساخت gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(ڈیٹا), &sc->وقت);
  timediff_t timeout_ms = ڈیٹا->سیٹ.ssl_ca_cache_timeout * (timediff_t)1000;

  اگر(timeout_ms < 0)
    واپس FALSE;

  واپس elapsed_ms >= timeout_ms;
}

ساکن bool gtls_shared_creds_different(ساخت Curl_cfilter *cf,
                                        ثابت ساخت gtls_shared_creds *sc)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  اگر(!sc->CAfile || !conn_config->CAfile)
    واپس sc->CAfile != conn_config->CAfile;

  واپس strcmp(sc->CAfile, conn_config->CAfile);
}

ساکن ساخت gtls_shared_creds *gtls_get_cached_creds(ساخت Curl_cfilter *cf,
                                                       ساخت Curl_easy *ڈیٹا)
{
  ساخت gtls_shared_creds *shared_creds;

  اگر(ڈیٹا->multi) {
    shared_creds = Curl_hash_pick(&ڈیٹا->multi->proto_hash,
                                  MPROTO_GTLS_X509_کلید,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_کلید));
    اگر(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(ڈیٹا, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      واپس shared_creds;
    }
  }
  واپس تہی;
}

ساکن خالی gtls_shared_creds_hash_free(ثابت خالی *کلید,
                                        size_t کلید_len, خالی *p)
{
  ساخت gtls_shared_creds *sc = p;
  DEBUGASSERT(کلید_len == CURL_CSTRLEN(MPROTO_GTLS_X509_کلید));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_کلید, کلید, کلید_len));
  (خالی)کلید;
  (خالی)کلید_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

ساکن خالی gtls_سیٹ_cached_creds(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت gtls_shared_creds *sc)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  اگر(!ڈیٹا->multi)
    واپس;

  اگر(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    اگر(!sc->CAfile)
      واپس;
  }

  اگر(Curl_gtls_shared_creds_اوپر_ref(sc))
    واپس;

  اگر(!Curl_hash_add2(&ڈیٹا->multi->proto_hash,
                     MPROTO_GTLS_X509_کلید,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_کلید),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    واپس;
  }
}

CURLcode Curl_gtls_client_trust_تیاری(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت gtls_ctx *gtls)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت gtls_shared_creds *cached_creds = تہی;
  bool cache_criteria_met;
  CURLcode نتیجہ;
  صحیح rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (ڈیٹا->سیٹ.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  اگر(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, ڈیٹا);

  اگر(cached_creds && !Curl_gtls_shared_creds_اوپر_ref(cached_creds)) {
    CURL_TRC_CF(ڈیٹا, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_سیٹ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    اگر(rc != GNUTLS_E_SUCCESS) {
      failf(ڈیٹا, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      واپس CURLE_SSL_CONNECT_ERROR;
    }
  }
  ورنہ {
    CURL_TRC_CF(ڈیٹا, cf, "loading trust anchors and CRLs");
    نتیجہ = gtls_populate_creds(cf, ڈیٹا, gtls->shared_creds->creds);
    اگر(نتیجہ)
      واپس نتیجہ;
    gtls->shared_creds->trust_تیاری = TRUE;
    اگر(cache_criteria_met)
      gtls_سیٹ_cached_creds(cf, ڈیٹا, gtls->shared_creds);
  }
  واپس CURLE_OK;
}

#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
ساکن صحیح gtls_get_ietf_proto(gnutls_session_t session)
{
  انتخاب(gnutls_protocol_get_نسخہ(session)) {
  حالت GNUTLS_SSL3:
    واپس CURL_IETF_PROTO_SSL3;
  حالت GNUTLS_TLS1_0:
    واپس CURL_IETF_PROTO_TLS1;
  حالت GNUTLS_TLS1_1:
    واپس CURL_IETF_PROTO_TLS1_1;
  حالت GNUTLS_TLS1_2:
    واپس CURL_IETF_PROTO_TLS1_2;
  حالت GNUTLS_TLS1_3:
    واپس CURL_IETF_PROTO_TLS1_3;
  طےشدہ:
    واپس CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ثابت حرف *ssl_peer_کلید,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ثابت حرف *alpn,
                                 غیرعلامتی حرف *quic_tp,
                                 size_t quic_tp_len,
                                 ساخت Curl_ssl_session **psession)
{
  ساخت Curl_ssl_session *sc_session = تہی, *sc_dup = تہی;
  غیرعلامتی حرف *sdata, *qtp_clone = تہی;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode نتیجہ = CURLE_OK;

  اگر(psession)
    *psession = تہی;
  اگر(!Curl_ssl_scache_use(cf, ڈیٹا))
    واپس CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_ڈیٹا(session, تہی, &sdata_len);
  اگر(!sdata_len) /* GnuTLS does this for some version combinations */
    واپس CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  اگر(!sdata)
    واپس CURLE_برآمد_کا_حافظہ;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_ڈیٹا(session, sdata, &sdata_len);
  earlydata_max = gnutls_ریکارڈ_get_max_early_ڈیٹا_سائز(session);

  CURL_TRC_CF(ڈیٹا, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  اگر(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ثابت حرف *)quic_tp, quic_tp_len);
    اگر(!qtp_clone) {
      curlx_free(sdata);
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  نتیجہ = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  اگر(!نتیجہ && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    نتیجہ = Curl_ssl_session_dup(sc_session, &sc_dup);
  اگر(!نتیجہ) {
    نتیجہ = Curl_ssl_scache_put(cf, ڈیٹا, ssl_peer_کلید, sc_session);
    /* took ownership of `sc_session` */
    sc_session = تہی;
  }
  اگر(!نتیجہ && psession) {
    *psession = sc_dup;
    sc_dup = تہی;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  واپس نتیجہ;
}
#اگر_ختم

#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
ساکن CURLcode cf_gtls_update_session_id(ساخت Curl_cfilter *cf,
                                          ساخت Curl_easy *ڈیٹا,
                                          gnutls_session_t session)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت Curl_ssl_session *scs = تہی;
  CURLcode نتیجہ;

  نتیجہ = Curl_gtls_cache_session(cf, ڈیٹا, connssl->peer.scache_کلید,
                                   session, 0, connssl->negotiated.alpn,
                                   تہی, 0, &scs);
  اگر(!نتیجہ) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  واپس نتیجہ;
}

ساکن صحیح gtls_handshake_cb(gnutls_session_t session, غیرعلامتی صحیح htype,
                             غیرعلامتی when, غیرعلامتی صحیح incoming,
                             ثابت gnutls_datum_t *پیغام)
{
  ساخت Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (خالی)پیغام;
  (خالی)incoming;
  اگر(when) { /* after message has been processed */
    ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
    اگر(ڈیٹا) {
      CURL_TRC_CF(ڈیٹا, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      انتخاب(htype) {
      حالت GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, ڈیٹا, session);
        توڑو;
      }
      طےشدہ:
        توڑو;
      }
    }
  }
  واپس 0;
}
#اگر_ختم

ساکن CURLcode gtls_سیٹ_ترجیح(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت gtls_ctx *gtls,
                                  ثابت حرف *ترجیح)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت dynbuf buf;
  ثابت حرف *err = تہی;
  CURLcode نتیجہ = CURLE_OK;
  صحیح rc;

  curlx_dyn_init(&buf, 4096);

  اگر(conn_config->cipher_list) {
    اگر((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      اگر(!curlx_dyn_len(&buf)) {  /* not added yet */
        نتیجہ = curlx_dyn_جوڑو(&buf, ترجیح);
        اگر(نتیجہ)
          جاؤ برآمد;
      }
      نتیجہ = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      اگر(نتیجہ)
        جاؤ برآمد;
      ترجیح = curlx_dyn_ptr(&buf);
    }
    ورنہ /* replace our own completely */
      ترجیح = conn_config->cipher_list;
  }

  infof(ڈیٹا, "GnuTLS priority: %s", ترجیح);
  rc = gnutls_ترجیح_سیٹ_براہ_راست(gtls->session, ترجیح, &err);
  اگر(rc != GNUTLS_E_SUCCESS) {
    failf(ڈیٹا, "Error %d setting GnuTLS priority: %s", rc, err);
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
  }

برآمد:
  curlx_dyn_free(&buf);
  واپس نتیجہ;
}

ساکن CURLcode gtls_client_init(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت ssl_peer *peer,
                                 size_t earlydata_max,
                                 ساخت gtls_ctx *gtls)
{
  ساخت ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ساخت ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ڈیٹا);
  غیرعلامتی صحیح init_flags;
  صحیح rc;
  ثابت حرف *prioritylist;
  bool tls13support;
  CURLcode نتیجہ;

  اگر(config->نسخہ_2 == CURL_SSLVERSION_SSLv2 ||
     config->نسخہ_2 == CURL_SSLVERSION_SSLv3) {
    failf(ڈیٹا, "GnuTLS does not support SSLv2 or SSLv3");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  نتیجہ = Curl_gtls_shared_creds_create(ڈیٹا, &gtls->shared_creds);
  اگر(نتیجہ)
    واپس نتیجہ;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
  اگر(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_ڈیٹا | GNUTLS_NO_انتہا_کا_EARLY_ڈیٹا;
  ورنہ اگر(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_ڈیٹا;
#اگر_ختم

#اگر_تعریف_شدہ GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#اگر_ختم

#اگر_تعریف_شدہ GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#اگر_ختم

#اگر_تعریف_شدہ GNUTLS_NO_حالت_REQUEST
  اگر(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_حالت_REQUEST;
#اگر_ختم

  CURL_TRC_CF(ڈیٹا, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  اگر(rc != GNUTLS_E_SUCCESS) {
    failf(ڈیٹا, "gnutls_init() failed: %d", rc);
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  اگر(peer->sni) {
    اگر(gnutls_server_name_سیٹ(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, لڑی_لمبائی(peer->sni)) < 0) {
      failf(ڈیٹا, "Failed to set SNI");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_سیٹ_default_ترجیح(gtls->session);
  اگر(rc != GNUTLS_E_SUCCESS)
    واپس CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_نسخہ("3.6.5");

  اگر(config->نسخہ_2 == CURL_SSLVERSION_TLSv1_3) {
    اگر(!tls13support) {
      failf(ڈیٹا, "This GnuTLS installation does not support TLS 1.3");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  نتیجہ = gnutls_سیٹ_ssl_نسخہ_min_max(ڈیٹا, peer,
                                          config, &prioritylist, tls13support);
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = gtls_سیٹ_ترجیح(cf, ڈیٹا, gtls, prioritylist);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(config->clientcert) {
    اگر(!gtls->shared_creds->trust_تیاری) {
      نتیجہ = Curl_gtls_client_trust_تیاری(cf, ڈیٹا, gtls);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    اگر(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_سیٹ_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->کلید_passwd ? config->کلید_passwd : "");
      اگر(rc != GNUTLS_E_SUCCESS) {
        failf(ڈیٹا,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        واپس CURLE_SSL_CONNECT_ERROR;
      }
    }
    ورنہ {
      ثابت غیرعلامتی صحیح supported_کلید_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_سیٹ_x509_کلید_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->کلید ? config->کلید : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->کلید_passwd,
           supported_کلید_encryption_algorithms);
      اگر(rc != GNUTLS_E_SUCCESS) {
        failf(ڈیٹا,
              "error reading X.509 %skey file: %s",
              config->کلید_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        واپس CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_سیٹ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  اگر(rc != GNUTLS_E_SUCCESS) {
    failf(ڈیٹا, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  اگر(config->verifystatus) {
    rc = gnutls_ocsp_حالت_request_enable_client(gtls->session,
                                                  تہی, 0, تہی);
    اگر(rc != GNUTLS_E_SUCCESS) {
      failf(ڈیٹا, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      واپس CURLE_SSL_CONNECT_ERROR;
    }
  }

  واپس CURLE_OK;
}

#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
ساکن صحیح keylog_callback(gnutls_session_t session, ثابت حرف *label,
                           ثابت gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  اگر(crandom.سائز != 32) {
    واپس -1;
  }

  Curl_tls_keylog_write(label, crandom.ڈیٹا, crandom.سائز,
                        secret->ڈیٹا, secret->سائز);
  واپس 0;
}

ساکن CURLcode gtls_on_session_reuse(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت alpn_spec *alpns,
                                      ساخت Curl_ssl_session *scs,
                                      bool *do_early_ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;

  connssl->earlydata_max =
    gnutls_ریکارڈ_get_max_early_ڈیٹا_سائز(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  واپس Curl_on_session_reuse(cf, ڈیٹا, alpns, scs, do_early_ڈیٹا,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#اگر_ختم

ساکن CURLcode gtls_apply_session(
  ساخت gtls_ctx *gctx,
  ساخت Curl_cfilter *cf,
  ساخت Curl_easy *ڈیٹا,
  ساخت ssl_peer *peer,
  ساخت alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  ساخت Curl_ssl_session *scs,
  bool *pwas_تیاری)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_OK;
  صحیح rc;

  اگر(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_شامل_ہے_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    نتیجہ = gtls_client_init(cf, ڈیٹا, peer, scs->earlydata_max, gctx);
    اگر(نتیجہ)
      جاؤ برآمد;
    *pwas_تیاری = TRUE;

    rc = gnutls_session_سیٹ_ڈیٹا(gctx->session, scs->sdata, scs->sdata_len);
    اگر(rc < 0)
      infof(ڈیٹا, "SSL session not accepted by GnuTLS, continuing without");
    ورنہ {
      infof(ڈیٹا, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      اگر(conn_config->earlydata && scs->alpn &&
         !cf->conn->بٹ.connect_only) {
        bool do_early_ڈیٹا = FALSE;
        اگر(sess_reuse_cb) {
          نتیجہ = sess_reuse_cb(cf, ڈیٹا, alpns, scs, &do_early_ڈیٹا);
          اگر(نتیجہ)
            جاؤ برآمد;
        }
        اگر(do_early_ڈیٹا) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_مقصد(alpns, scs->alpn);
        }
      }
    }
  }
برآمد:
  واپس نتیجہ;
}

CURLcode Curl_gtls_ctx_init(ساخت gtls_ctx *gctx,
                            ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            ساخت ssl_peer *peer,
                            ثابت ساخت alpn_spec *alpns_requested,
                            Curl_gtls_ctx_تیاری_cb *cb_تیاری,
                            خالی *cb_user_ڈیٹا,
                            خالی *ssl_user_ڈیٹا,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت Curl_ssl_session *scs = تہی;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_گنتی = 0;
  bool gtls_session_تیاری = FALSE;
  ساخت alpn_spec alpns;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  اگر((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(ڈیٹا, cf->cft, FIRSTSOCKET);
    اگر(scs) {
      نتیجہ = gtls_apply_session(gctx, cf, ڈیٹا, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_تیاری);
      scs = تہی;
      اگر(!نتیجہ && gtls_session_تیاری) {
        CURL_TRC_CF(ڈیٹا, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  اگر(!gtls_session_تیاری && conn_config->cache_session &&
     !conn_config->verifystatus) {
    نتیجہ = Curl_ssl_scache_take(cf, ڈیٹا, peer->scache_کلید, &scs);
    اگر(نتیجہ)
      جاؤ برآمد;
    نتیجہ = gtls_apply_session(gctx, cf, ڈیٹا, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_تیاری);
  }

  اگر(!gtls_session_تیاری) {
    نتیجہ = gtls_client_init(cf, ڈیٹا, peer, 0, gctx);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  gnutls_session_سیٹ_ptr(gctx->session, ssl_user_ڈیٹا);

  اگر(cb_تیاری) {
    نتیجہ = cb_تیاری(cf, ڈیٹا, cb_user_ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  اگر(Curl_tls_keylog_enabled()) {
    gnutls_session_سیٹ_keylog_function(gctx->session, keylog_callback);
  }
#اگر_ختم

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  اگر(!gtls_alpns_گنتی && alpns.گنتی) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.گنتی);
    برائے(i = 0; i < alpns.گنتی; ++i) {
      gtls_alpns[i].ڈیٹا = (غیرعلامتی حرف *)alpns.entries[i];
      gtls_alpns[i].سائز = (غیرعلامتی صحیح)لڑی_لمبائی(alpns.entries[i]);
    }
    gtls_alpns_گنتی = alpns.گنتی;
  }

  اگر(gtls_alpns_گنتی &&
     gnutls_alpn_سیٹ_protocols(gctx->session,
                               gtls_alpns, (غیرعلامتی صحیح)gtls_alpns_گنتی,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(ڈیٹا, "failed setting ALPN");
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
  }

برآمد:
  Curl_ssl_scache_return(cf, ڈیٹا, peer->scache_کلید, scs);
  واپس نتیجہ;
}

ساکن CURLcode gtls_connect_step1(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ;

  DEBUGASSERT(backend);

  اگر(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    واپس CURLE_OK;

  نتیجہ = Curl_gtls_ctx_init(&backend->gtls, cf, ڈیٹا, &connssl->peer,
                              connssl->alpn, تہی, تہی, cf,
#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
                              gtls_on_session_reuse
#ورنہ
                              تہی
#اگر_ختم
                              );

  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ساخت alpn_proto_buf proto;
    memset(&proto, 0, حجم(proto));
    Curl_alpn_مقصد_proto_str(&proto, connssl->alpn);
    infof(ڈیٹا, VTLS_INFOF_ALPN_OFFER_1STR, proto.ڈیٹا);
  }

#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
  gnutls_handshake_سیٹ_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#اگر_ختم

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_سیٹ_ptr(backend->gtls.session, cf);
  gnutls_transport_سیٹ_ڈالو_function(backend->gtls.session, gtls_ڈالو);
  gnutls_transport_سیٹ_pull_function(backend->gtls.session, gtls_pull);

  واپس CURLE_OK;
}

ساکن CURLcode pkp_pin_peer_pubkey(ساخت Curl_easy *ڈیٹا,
                                    gnutls_x509_crt_t cert,
                                    ثابت حرف *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  غیرعلامتی حرف *buff1 = تہی;

  gnutls_pubkey_t کلید = تہی;

  /* Result is returned to caller */
  CURLcode نتیجہ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  اگر(!pinnedpubkey)
    واپس CURLE_OK;

  اگر(!cert)
    واپس نتیجہ;

  کرو {
    صحیح ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&کلید);
    اگر(ret < 0)
      توڑو; /* failed */

    ret = gnutls_pubkey_import_x509(کلید, cert, 0);
    اگر(ret < 0)
      توڑو; /* failed */

    ret = gnutls_pubkey_export(کلید, GNUTLS_X509_FMT_DER, تہی, &len1);
    اگر(ret != GNUTLS_E_SHORT_حافظہ_بفر || len1 == 0)
      توڑو; /* failed */

    buff1 = curlx_malloc(len1);
    اگر(!buff1)
      توڑو; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(کلید, GNUTLS_X509_FMT_DER, buff1, &len2);
    اگر(ret < 0 || len1 != len2)
      توڑو; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    نتیجہ = Curl_pin_peer_pubkey(ڈیٹا, pinnedpubkey, buff1, len1);
  } جب(0);

  اگر(کلید)
    gnutls_pubkey_deinit(کلید);

  curlx_safefree(buff1);

  واپس نتیجہ;
}

خالی Curl_gtls_report_handshake(ساخت Curl_easy *ڈیٹا, ساخت gtls_ctx *gctx)
{
#اگر_تعریف_شدہ CURLVERBOSE
  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    ثابت حرف *ptr;
    gnutls_protocol_t نسخہ_2 = gnutls_protocol_get_نسخہ(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(ڈیٹا, "SSL connection using %s / %s",
          gnutls_protocol_get_name(نسخہ_2), ptr);
  }
#ورنہ
  (خالی)ڈیٹا;
  (خالی)gctx;
#اگر_ختم
}

ساکن خالی gtls_پیغام_verify_نتیجہ(ساخت Curl_easy *ڈیٹا,
                                   ساخت ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  حرف certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t سائز = حجم(certname);
  صحیح rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &سائز);
  اگر(rc) {
    infof(ڈیٹا, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  اگر(!was_verified) {
    اگر(needs_verified) {
      failf(ڈیٹا, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    ورنہ
      infof(ڈیٹا, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  ورنہ
    infof(ڈیٹا, "  common name: %s (matched)", certname);
}

ساکن خالی gtls_infof_cert(ساخت Curl_easy *ڈیٹا,
                            gnutls_x509_crt_t x509_cert)
{
#اگر_تعریف_شدہ CURLVERBOSE
  اگر(Curl_trc_is_verbose(ڈیٹا)) {
    gnutls_datum_t certfields;
    صحیح rc, algo;
    time_t tstamp;
    غیرعلامتی صحیح بٹ;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &بٹ);
    infof(ڈیٹا, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(ڈیٹا, "  certificate version: #%d",
          gnutls_x509_crt_get_نسخہ(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    اگر(rc)
      infof(ڈیٹا, "Failed to get certificate name");
    ورنہ {
      infof(ڈیٹا, "  subject: %s", certfields.ڈیٹا);

      tstamp = gnutls_x509_crt_get_activation_وقت(x509_cert);
      showtime(ڈیٹا, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_وقت(x509_cert);
      showtime(ڈیٹا, "expire date", tstamp);

      gnutls_free(certfields.ڈیٹا);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    اگر(rc)
      infof(ڈیٹا, "Failed to get certificate issuer");
    ورنہ {
      infof(ڈیٹا, "  issuer: %s", certfields.ڈیٹا);

      gnutls_free(certfields.ڈیٹا);
    }
  }
#ورنہ
  (خالی)ڈیٹا;
  (خالی)x509_cert;
#اگر_ختم
}

ساکن CURLcode gtls_verify_ocsp_حالت(ساخت Curl_easy *ڈیٹا,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = تہی;
  CURLcode نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
  صحیح rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_حالت_t حالت_2 = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_حالت_request_get(session, &ocsp_raw);
  اگر(rc == GNUTLS_E_REQUESTED_ڈیٹا_NOT_دستیاب) {
    failf(ڈیٹا, "No OCSP response received");
    جاؤ برآمد;
  }
  ورنہ اگر(rc < 0) {
    failf(ڈیٹا, "Invalid OCSP response received");
    جاؤ برآمد;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  اگر(rc < 0) {
    failf(ڈیٹا, "failed to init OCSP response parsing");
    جاؤ برآمد;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  اگر(rc < 0) {
    failf(ڈیٹا, "failed to parse OCSP response");
    جاؤ برآمد;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  اگر(gnutls_ocsp_resp_get_حالت(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(ڈیٹا, "OCSP response basic part is not successful");
    جاؤ برآمد;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  برائے(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    اگر(!rc || (rc == GNUTLS_E_REQUESTED_ڈیٹا_NOT_دستیاب))
      توڑو;
  }
  اگر(rc < 0) {
    failf(ڈیٹا, "OCSP does not carry status for peer cert");
    جاؤ برآمد;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, تہی, تہی, تہی, تہی,
                                   &حالت_2, تہی, تہی, تہی, تہی);
  اگر(rc < 0) {
    failf(ڈیٹا, "failed to parse OCSP response for peer cert");
    جاؤ برآمد;
  }
  انتخاب(حالت_2) {
  حالت GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    نتیجہ = CURLE_OK;
    توڑو;
  حالت GNUTLS_OCSP_CERT_REVOKED:
    failf(ڈیٹا, "peer certificate is revoked");
    توڑو;
  طےشدہ:
    failf(ڈیٹا, "peer certificate OCSP status is not good: %d", (صحیح)حالت_2);
    توڑو;
  }

برآمد:
  اگر(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  واپس نتیجہ;
}

ساخت gtls_cert_chain {
  ثابت gnutls_datum_t *certs;
  غیرعلامتی صحیح عدد_certs;
};

#اگر_تعریف_شدہ USE_APPLE_SECTRUST
ساکن CURLcode gtls_chain_get_der(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   خالی *user_ڈیٹا,
                                   size_t i,
                                   غیرعلامتی حرف **pder,
                                   size_t *pder_len)
{
  ساخت gtls_cert_chain *chain = user_ڈیٹا;

  (خالی)cf;
  (خالی)ڈیٹا;
  *pder_len = 0;
  *pder = تہی;

  اگر(i >= chain->عدد_certs)
    واپس CURLE_TOO_LARGE;
  *pder = chain->certs[i].ڈیٹا;
  *pder_len = (size_t)chain->certs[i].سائز;
  واپس CURLE_OK;
}
#اگر_ختم /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ساکن CURLcode gtls_verify_cert(ساخت Curl_easy *ڈیٹا,
                                 ساخت ssl_filter_config *config,
                                 ساخت ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 ساخت Curl_cfilter *cf,
                                 ساخت ssl_peer *peer,
                                 ساخت gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  غیرعلامتی صحیح verify_حالت = 0;
  طویل * ثابت certverifyresult = &ssl_config->certverifyresult;
  صحیح rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_حالت);
  اگر(rc < 0) {
    failf(ڈیٹا, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    واپس CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_حالت;
  verified = !(verify_حالت & GNUTLS_CERT_INVALID);
  اگر(verified)
    infof(ڈیٹا, "  SSL certificate verified by GnuTLS");

#اگر_تعریف_شدہ USE_APPLE_SECTRUST
  اگر(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode نتیجہ;

    اگر(gnutls_ocsp_حالت_request_get(session, &ocsp_raw)) {
      ocsp_raw.ڈیٹا = تہی;
      ocsp_raw.سائز = 0;
    }
    اگر(config->verifystatus && !ocsp_raw.سائز) {
      failf(ڈیٹا, "No OCSP response received");
      واپس CURLE_SSL_INVALIDCERTSTATUS;
    }
    نتیجہ = Curl_vtls_apple_verify(cf, ڈیٹا, peer, chain->عدد_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.ڈیٹا, ocsp_raw.سائز,
                                    config->verifystatus);
    اگر(نتیجہ && (نتیجہ != CURLE_PEER_FAILED_VERIFICATION))
      واپس نتیجہ; /* no further failure analysis needed */
    verified = !نتیجہ;
    اگر(verified) {
      infof(ڈیٹا, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#ورنہ
  (خالی)cf;
  (خالی)peer;
  (خالی)chain;
#اگر_ختم

  اگر(!verified) {
    CURLcode نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ثابت حرف *cause = "certificate error, no details available";
    اگر(verify_حالت & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    ورنہ اگر(verify_حالت & GNUTLS_CERT_SIGNER_NOT_ملا)
      cause = "certificate signer not trusted";
    ورنہ اگر(verify_حالت & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    ورنہ اگر(verify_حالت & GNUTLS_CERT_INVALID_OCSP_حالت) {
      cause = "OCSP status response is invalid";
      نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(ڈیٹا, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    واپس نتیجہ;
  }
  واپس CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                gnutls_session_t session,
                                ساخت ssl_filter_config *config,
                                ساخت ssl_easy_config *ssl_config,
                                ساخت ssl_peer *peer,
                                ثابت حرف *pinned_کلید)
{
  ساخت gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = تہی, x509_issuer = تہی;
  time_t certclock;
  bool ocsp_checked = FALSE;
  صحیح rc;
  CURLcode نتیجہ = CURLE_OK;
  طویل * ثابت certverifyresult = &ssl_config->certverifyresult;

  (خالی)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.عدد_certs);
  اگر(!chain.certs) {
    اگر(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(ڈیٹا, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_ملا;
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
      جاؤ برآمد;
    }
    infof(ڈیٹا, " common name: WARNING could not obtain");
  }

  اگر(ڈیٹا->سیٹ.ssl.certinfo && chain.certs) {
    اگر(chain.عدد_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(ڈیٹا, "%u certificates is more than allowed (%d)",
            chain.عدد_certs, MAX_ALLOWED_CERT_AMOUNT);
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ برآمد;
    }
    ورنہ {
      غیرعلامتی صحیح i;

      نتیجہ = Curl_ssl_init_certinfo(ڈیٹا, (صحیح)chain.عدد_certs);
      اگر(نتیجہ)
        جاؤ برآمد;

      برائے(i = 0; i < chain.عدد_certs; i++) {
        ثابت uint8_t *beg = chain.certs[i].ڈیٹا;
        ثابت uint8_t *انتہا = beg + chain.certs[i].سائز;

        نتیجہ = Curl_نکالو_certinfo(ڈیٹا, (صحیح)i, beg, انتہا);
        اگر(نتیجہ)
          جاؤ برآمد;
      }
    }
  }

  اگر(config->verifypeer) {
    نتیجہ = gtls_verify_cert(ڈیٹا, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  ورنہ
    infof(ڈیٹا, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  اگر(gnutls_x509_crt_init(&x509_cert)) {
    failf(ڈیٹا, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_ملا;
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
    جاؤ برآمد;
  }

  اگر(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    اگر(rc) {
      failf(ڈیٹا, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_ملا;
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ برآمد;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_وقت(x509_cert);

  اگر(certclock == (time_t)-1) {
    اگر(config->verifypeer) {
      failf(ڈیٹا, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ برآمد;
    }
    ورنہ
      infof(ڈیٹا, "  SSL certificate expiration date verify FAILED");
  }
  ورنہ {
    اگر(certclock < وقت(تہی)) {
      اگر(config->verifypeer) {
        failf(ڈیٹا, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
        جاؤ برآمد;
      }
      ورنہ
        infof(ڈیٹا, "  SSL certificate expiration date FAILED");
    }
    ورنہ
      infof(ڈیٹا, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_وقت(x509_cert);

  اگر(certclock == (time_t)-1) {
    اگر(config->verifypeer) {
      failf(ڈیٹا, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ برآمد;
    }
    ورنہ
      infof(ڈیٹا, "  SSL certificate activation date verify FAILED");
  }
  ورنہ {
    اگر(certclock > وقت(تہی)) {
      اگر(config->verifypeer) {
        failf(ڈیٹا, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
        جاؤ برآمد;
      }
      ورنہ
        infof(ڈیٹا, "  SSL certificate activation date FAILED");
    }
    ورنہ
      infof(ڈیٹا, "  SSL certificate activation date OK");
  }

  اگر(config->verifystatus && !ocsp_checked) {
    نتیجہ = gtls_verify_ocsp_حالت(ڈیٹا, session, x509_cert);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  ورنہ
    infof(ڈیٹا, "  SSL certificate status verification SKIPPED");

  اگر(config->issuercert) {
    gnutls_datum_t issuerp;
    اگر(gnutls_x509_crt_init(&x509_issuer)) {
      failf(ڈیٹا, "failed to init GnuTLS x509_crt for issuer");
      نتیجہ = CURLE_SSL_ISSUER_ERROR;
      جاؤ برآمد;
    }
    issuerp = بھرو_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    اگر(rc) {
      failf(ڈیٹا, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      نتیجہ = CURLE_SSL_ISSUER_ERROR;
      جاؤ برآمد;
    }
    rc = (صحیح)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    اگر(rc <= 0) {
      failf(ڈیٹا, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      نتیجہ = CURLE_SSL_ISSUER_ERROR;
      جاؤ برآمد;
    }
    infof(ڈیٹا, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  rc = (صحیح)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  نتیجہ = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_پیغام_verify_نتیجہ(ڈیٹا, peer, x509_cert, rc, config->verifyhost);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(pinned_کلید) {
    نتیجہ = pkp_pin_peer_pubkey(ڈیٹا, x509_cert, pinned_کلید);
    اگر(نتیجہ != CURLE_OK) {
      failf(ڈیٹا, "SSL: public key does not match pinned public key");
      جاؤ برآمد;
    }
  }

  gtls_infof_cert(ڈیٹا, x509_cert);

برآمد:
  اگر(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  اگر(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  واپس نتیجہ;
}

ساکن CURLcode gtls_verifyserver(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  gnutls_session_t session)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, ڈیٹا);
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  ثابت حرف *pinned_کلید = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY);
#ورنہ
  ثابت حرف *pinned_کلید = CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY);
#اگر_ختم
  CURLcode نتیجہ;

  نتیجہ = Curl_gtls_verifyserver(cf, ڈیٹا, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_کلید);
  اگر(نتیجہ)
    جاؤ برآمد;

#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  اگر(gnutls_protocol_get_نسخہ(session) < GNUTLS_TLS1_3)
    نتیجہ = cf_gtls_update_session_id(cf, ڈیٹا, session);
#اگر_ختم

برآمد:
  واپس نتیجہ;
}

#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
ساکن CURLcode gtls_send_earlydata(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  ثابت غیرعلامتی حرف *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_نتیجہ = CURLE_OK;
  جب(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_ریکارڈ_send_early_ڈیٹا(backend->gtls.session, buf, blen);
    CURL_TRC_CF(ڈیٹا, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    اگر(n < 0) {
      اگر(n == GNUTLS_E_AGAIN)
        نتیجہ = CURLE_AGAIN;
      ورنہ
        نتیجہ = backend->gtls.io_نتیجہ ?
                 backend->gtls.io_نتیجہ : CURLE_SEND_ERROR;
      جاؤ برآمد;
    }
    ورنہ اگر(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(ڈیٹا, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
برآمد:
  واپس نتیجہ;
}
#اگر_ختم

/*
 * This function is called after the TCP connect has completed. Setup the TLS
 * layer and do all necessary magic.
 */
/* We use connssl->connecting_state to keep track of the connection status;
   there are three states: 'ssl_connect_1' (not started yet or complete),
   'ssl_connect_2' (doing handshake with the server), and
   'ssl_connect_3' (verifying and getting stats).
 */
ساکن CURLcode gtls_connect_common(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  اگر(ssl_connection_complete == connssl->state) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;

  /* Initiate the connection, if not already done */
  اگر(connssl->connecting_state == ssl_connect_1) {
    نتیجہ = gtls_connect_step1(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
    connssl->connecting_state = ssl_connect_2;
  }

  اگر(connssl->connecting_state == ssl_connect_2) {
#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
    اگر(connssl->earlydata_state == ssl_earlydata_await) {
      جاؤ برآمد;
    }
    ورنہ اگر(connssl->earlydata_state == ssl_earlydata_sending) {
      نتیجہ = gtls_send_earlydata(cf, ڈیٹا);
      اگر(نتیجہ)
        جاؤ برآمد;
      connssl->earlydata_state = ssl_earlydata_بھیجا;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_بھیجا));
#اگر_ختم
    نتیجہ = cf_gtls_handshake(cf, ڈیٹا);
    اگر(نتیجہ)
      جاؤ برآمد;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  اگر(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    صحیح rc;

    Curl_gtls_report_handshake(ڈیٹا, &backend->gtls);

    نتیجہ = gtls_verifyserver(cf, ڈیٹا, backend->gtls.session);
    اگر(نتیجہ)
      جاؤ برآمد;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    اگر(rc) {  /* No ALPN from server */
      proto.ڈیٹا = تہی;
      proto.سائز = 0;
    }

    نتیجہ = Curl_alpn_سیٹ_negotiated(cf, ڈیٹا, connssl,
                                      proto.ڈیٹا, proto.سائز);
    اگر(نتیجہ)
      جاؤ برآمد;

#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
    اگر(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_بھیجا);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_ڈیٹا) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#اگر_ختم
    connssl->connecting_state = ssl_connect_مکمل;
  }

  اگر(connssl->connecting_state == ssl_connect_مکمل)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

برآمد:
  اگر(نتیجہ == CURLE_AGAIN) {
    *مکمل = FALSE;
    واپس CURLE_OK;
  }
  *مکمل = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(ڈیٹا, cf, "gtls_connect_common() -> %d, done=%d", (صحیح)نتیجہ,
              *مکمل);
  واپس نتیجہ;
}

ساکن CURLcode gtls_connect(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             bool *مکمل)
{
#اگر_تعریف_شدہ CURL_GNUTLS_EARLY_ڈیٹا
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  اگر((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *مکمل = TRUE;
    واپس CURLE_OK;
  }
#اگر_ختم
  واپس gtls_connect_common(cf, ڈیٹا, مکمل);
}

ساکن bool gtls_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                              ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *ctx = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend;

  (خالی)ڈیٹا;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ساخت gtls_ssl_backend_ڈیٹا *)ctx->backend;
  اگر(backend->gtls.session &&
     gnutls_ریکارڈ_check_pending(backend->gtls.session) != 0)
    واپس TRUE;
  واپس FALSE;
}

ساکن CURLcode gtls_send(ساخت Curl_cfilter *cf,
                          ساخت Curl_easy *ڈیٹا,
                          ثابت خالی *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  جب(remain) {
    backend->gtls.io_نتیجہ = CURLE_OK;
    nwritten = gnutls_ریکارڈ_send(backend->gtls.session, buf, remain);

    اگر(nwritten >= 0) {
      اگر((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(ڈیٹا, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        واپس CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (حرف *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    ورنہ {
      اگر(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        نتیجہ = CURLE_OK;
        جاؤ برآمد;
      }
      نتیجہ = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_نتیجہ ? backend->gtls.io_نتیجہ : CURLE_SEND_ERROR);
      جاؤ برآمد;
    }
  }

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (صحیح)نتیجہ, *pnwritten);
  واپس نتیجہ;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ساکن CURLcode gtls_shutdown(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              bool send_shutdown, bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  حرف buf[1024];
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  اگر(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *مکمل = TRUE;
    جاؤ برآمد;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *مکمل = FALSE;

  اگر(!backend->gtls.بھیجا_shutdown) {
    /* do this only once */
    backend->gtls.بھیجا_shutdown = TRUE;
    اگر(send_shutdown) {
      صحیح ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      اگر((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_ریکارڈ_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.بھیجا_shutdown = FALSE;
        نتیجہ = CURLE_OK;
        جاؤ برآمد;
      }
      اگر(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((صحیح)ret), (صحیح)ret);
        نتیجہ = CURLE_RECV_ERROR;
        جاؤ برآمد;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  برائے(i = 0; i < 10; ++i) {
    nread = gnutls_ریکارڈ_recv(backend->gtls.session, buf, حجم(buf));
    اگر(nread <= 0)
      توڑو;
  }
  اگر(nread > 0) {
    /* still data coming in? */
  }
  ورنہ اگر(nread == 0) {
    /* We got the close notify alert and are done. */
    *مکمل = TRUE;
  }
  ورنہ اگر((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_ریکارڈ_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  ورنہ {
    CURL_TRC_CF(ڈیٹا, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((صحیح)nread), (صحیح)nread);
    نتیجہ = CURLE_RECV_ERROR;
  }

برآمد:
  cf->shutdown = (نتیجہ || *مکمل);
  واپس نتیجہ;
}

ساکن خالی gtls_close(ساخت Curl_cfilter *cf,
                       ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(ڈیٹا, cf, "close");
  اگر(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = تہی;
  }
  اگر(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ساکن CURLcode gtls_recv(ساخت Curl_cfilter *cf,
                          ساخت Curl_easy *ڈیٹا,
                          حرف *buf, size_t blen,
                          size_t *pnread)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_ریکارڈ_recv(backend->gtls.session, buf, blen);

  اگر(nread >= 0)
    *pnread = (size_t)nread;
  ورنہ {
    اگر((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      نتیجہ = CURLE_AGAIN;
      جاؤ برآمد;
    }
    ورنہ اگر(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      نتیجہ = cf_gtls_handshake(cf, ڈیٹا);
      اگر(!نتیجہ)
        نتیجہ = CURLE_AGAIN; /* make us get called again. */
      جاؤ برآمد;
    }
    ورنہ {
      failf(ڈیٹا, "GnuTLS recv error (%d): %s",
            (صحیح)nread, gnutls_strerror((صحیح)nread));
      نتیجہ = backend->gtls.io_نتیجہ ?
        backend->gtls.io_نتیجہ : CURLE_RECV_ERROR;
      جاؤ برآمد;
    }
  }

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (صحیح)نتیجہ, nread);
  واپس نتیجہ;
}

size_t Curl_gtls_نسخہ(حرف *بفر, size_t سائز)
{
  واپس curl_msnprintf(بفر, سائز, "GnuTLS/%s", gnutls_check_نسخہ(تہی));
}

/* data might be NULL! */
ساکن CURLcode gtls_random(ساخت Curl_easy *ڈیٹا,
                            غیرعلامتی حرف *entropy, size_t لمبائی)
{
  صحیح rc;
  (خالی)ڈیٹا;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, لمبائی);
  واپس rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ساکن CURLcode gtls_sha256sum(ثابت غیرعلامتی حرف *داخلہ,
                               size_t len,
                               غیرعلامتی حرف *sha256sum, /* output */
                               size_t sha256len)
{
  ساخت sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  کرو {
    غیرعلامتی صحیح ilen = (غیرعلامتی صحیح)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, داخلہ);
    len -= ilen;
    داخلہ += ilen;
  } جب(len);
#اگر NETTLE_نسخہ_MAJOR >= 4
  (خالی)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#ورنہ
  sha256_digest(&SHA256pw, (غیرعلامتی صحیح)sha256len, sha256sum);
#اگر_ختم
  واپس CURLE_OK;
}

ساکن bool gtls_cert_حالت_request(خالی)
{
  واپس TRUE;
}

ساکن خالی *gtls_get_internals(ساخت ssl_connect_ڈیٹا *connssl,
                                CURLINFO info)
{
  ساخت gtls_ssl_backend_ڈیٹا *backend =
    (ساخت gtls_ssl_backend_ڈیٹا *)connssl->backend;
  (خالی)info;
  DEBUGASSERT(backend);
  واپس backend->gtls.session;
}

ثابت ساخت Curl_ssl Curl_ssl_gnutls = {
  { CURLSSLBACKEND_GNUTLS, "gnutls" }, /* info */

  SSLSUPP_CA_راستہ  |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_CA_CACHE |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_CRLFILE,

  حجم(ساخت gtls_ssl_backend_ڈیٹا),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_نسخہ,             /*نسخہ*/
  gtls_shutdown,                 /* shutdown */
  gtls_ڈیٹا_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_حالت_request,      /* cert_status_request */
  gtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,       /* adjust_pollset */
  gtls_get_internals,            /* get_internals */
  gtls_close,                    /* close_one */
  تہی,                          /* close_all */
  تہی,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  تہی,                          /* get_channel_binding */
};

#اگر_ختم /* USE_GNUTLS */
