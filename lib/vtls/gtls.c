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
#ضمن "curl_setup.h"

#جے_تعریف_شدہ USE_GNUTLS

#ضمن <gnutls/abstract.h>
#ضمن <gnutls/gnutls.h>
#ضمن <gnutls/x509.h>
#ضمن <gnutls/crypto.h>
#ضمن <nettle/sha2.h>
#ضمن <nettle/version.h>

#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "vtls/keylog.h"
#ضمن "vtls/gtls.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_int.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vtls/apple.h"
#ضمن "vauth/vauth.h"
#ضمن "parsedate.h"
#ضمن "connect.h" /* for the connect timeout */
#ضمن "progress.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#إذا 0
#تعریف_کرو GTLSDEBUG
#جے_انت

#جے_تعریف_شدہ GTLSDEBUG
ساكن فراغ tls_log_func(صحيح level, ثابت حرف *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#جے_انت

#إذا !defined(GNUTLS_نسخہ_نمبر) || (GNUTLS_نسخہ_نمبر < 0x030605)
#غلطی "too old GnuTLS version"
#جے_انت

#تعریف_رد CURL_GNUTLS_EARLY_انکڑے
#إذا GNUTLS_نسخہ_نمبر >= 0x030702
#تعریف_کرو CURL_GNUTLS_EARLY_انکڑے
#جے_انت

#ضمن <gnutls/ocsp.h>

بنية gtls_ssl_backend_انکڑے {
  بنية gtls_ctx gtls;
};

ساكن ssize_t gtls_ڈالو(فراغ *s, ثابت فراغ *buf, size_t blen)
{
  بنية Curl_cfilter *cf = s;
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجہ;

  DEBUGASSERT(انکڑے);
  نتیجہ = Curl_conn_cf_send(cf->اگلا, انکڑے, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(انکڑے, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (صحيح)نتیجہ, nwritten);
  backend->gtls.io_نتیجہ = نتیجہ;
  إذا(نتیجہ) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_سیٹ_errno(backend->gtls.session,
                               (نتیجہ == CURLE_AGAIN) ? EAGAIN : EINVAL);
    أرجع -1;
  }
  أرجع (ssize_t)nwritten;
}

ساكن ssize_t gtls_pull(فراغ *s, فراغ *buf, size_t blen)
{
  بنية Curl_cfilter *cf = s;
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
  size_t nread;
  CURLcode نتیجہ;

  DEBUGASSERT(انکڑے);
  إذا(!backend->gtls.shared_creds->trust_تیاری) {
    نتیجہ = Curl_gtls_client_trust_تیاری(cf, انکڑے, &backend->gtls);
    إذا(نتیجہ) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_سیٹ_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_نتیجہ = نتیجہ;
      أرجع -1;
    }
  }

  نتیجہ = Curl_conn_cf_recv(cf->اگلا, انکڑے, buf, blen, &nread);
  CURL_TRC_CF(انکڑے, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (صحيح)نتیجہ,
              nread);
  backend->gtls.io_نتیجہ = نتیجہ;
  إذا(نتیجہ) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_سیٹ_errno(backend->gtls.session,
                               (نتیجہ == CURLE_AGAIN) ? EAGAIN : EINVAL);
    أرجع -1;
  }
  وإلا إذا(nread == 0)
    connssl->peer_closed = TRUE;
  أرجع (ssize_t)nread;
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
ساكن صحيح gtls_init(فراغ)
{
  صحيح ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#جے_تعریف_شدہ GTLSDEBUG
  gnutls_global_سیٹ_log_function(tls_log_func);
  gnutls_global_سیٹ_log_level(2);
#جے_انت
  أرجع ret;
}

ساكن فراغ gtls_cleanup(فراغ)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#جے_تعریف_شدہ CURLVERBOSE
ساكن فراغ showtime(بنية Curl_easy *انکڑے, ثابت حرف *متن_3, time_t stamp)
{
  بنية tm بفر;
  ثابت بنية tm *tm = &بفر;
  حرف str[96];
  CURLcode نتیجہ = curlx_gmtime(stamp, &بفر);
  إذا(نتیجہ)
    أرجع;

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
  infof(انکڑے, "%s", str);
}
#جے_انت

ساكن gnutls_datum_t لدو_file(ثابت حرف *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { عدم, 0 };
  لما filelen;
  فراغ *ptr;

  f = curlx_fopen(file, "rb");
  إذا(!f)
    أرجع loaded_file;
  إذا(fseek(f, 0, SEEK_END))
    اذهب باہرلا;
  filelen = ftell(f);
  إذا(filelen < 0 || filelen > CURL_MAX_داخلہ_لمائی)
    اذهب باہرلا;
  إذا(fseek(f, 0, SEEK_SET))
    اذهب باہرلا;
  ptr = curlx_malloc((size_t)filelen);
  إذا(!ptr)
    اذهب باہرلا;
  إذا(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    اذهب باہرلا;
  }

  loaded_file.انکڑے = ptr;
  loaded_file.ناپ = (بنانشانی صحيح)filelen;
باہرلا:
  curlx_fclose(f);
  أرجع loaded_file;
}

ساكن فراغ unload_file(gnutls_datum_t انکڑے)
{
  curlx_free(انکڑے.انکڑے);
}

/* this function does an SSL/TLS (re-)handshake */
ساكن CURLcode cf_gtls_handshake(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  gnutls_session_t session;
  صحيح rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_نتیجہ = CURLE_OK;
  rc = gnutls_handshake(session);

  إذا(!backend->gtls.shared_creds->trust_تیاری) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode نتیجہ = Curl_gtls_client_trust_تیاری(cf, انکڑے, &backend->gtls);
    إذا(نتیجہ)
      أرجع نتیجہ;
  }

  إذا((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_ریکارڈ_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    أرجع CURLE_AGAIN;
  }
  وإلا إذا((rc < 0) && !gnutls_error_is_fatal(rc)) {
#جے_تعریف_شدہ CURLVERBOSE
    ثابت حرف *strerr = عدم;

    إذا(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    إذا(!strerr)
      strerr = gnutls_strerror(rc);

    infof(انکڑے, "gnutls_handshake() warning: %s", strerr);
#جے_انت
    أرجع CURLE_AGAIN;
  }
  وإلا إذا((rc < 0) && backend->gtls.io_نتیجہ) {
    أرجع backend->gtls.io_نتیجہ;
  }
  وإلا إذا(rc < 0) {
    ثابت حرف *strerr = عدم;

    إذا(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    إذا(!strerr)
      strerr = gnutls_strerror(rc);

    failf(انکڑے, "GnuTLS, handshake failed: %s", strerr);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  أرجع CURLE_OK;
}

ساكن gnutls_x509_crt_fmt_t gnutls_do_file_type(ثابت حرف *type)
{
  إذا(!type || !type[0])
    أرجع GNUTLS_X509_FMT_PEM;
  إذا(curl_strequal(type, "PEM"))
    أرجع GNUTLS_X509_FMT_PEM;
  إذا(curl_strequal(type, "DER"))
    أرجع GNUTLS_X509_FMT_DER;
  أرجع GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#تعریف_کرو GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#تعریف_کرو QUIC_ترجیح \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ساكن CURLcode gnutls_سیٹ_ssl_نسخہ_min_max(
  بنية Curl_easy *انکڑے,
  بنية ssl_peer *peer,
  بنية ssl_filter_config *conn_config,
  ثابت حرف **prioritylist,
  bool tls13support)
{
  لما ssl_نسخہ = conn_config->نسخہ_2;
  لما ssl_نسخہ_max = conn_config->نسخہ_max;

  DEBUGASSERT(ssl_نسخہ != CURL_SSLVERSION_DEFAULT);
  إذا(ssl_نسخہ <= CURL_SSLVERSION_TLSv1)
    ssl_نسخہ = CURL_SSLVERSION_TLSv1_0;
  إذا((ssl_نسخہ_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_نسخہ_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_نسخہ_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  إذا(peer->transport == TRNSPRT_QUIC) {
    إذا(ssl_نسخہ_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(انکڑے, "QUIC needs at least TLS version 1.3");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_ترجیح;
    أرجع CURLE_OK;
  }

  بدل(ssl_نسخہ | ssl_نسخہ_max) {
  حالة CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    أرجع CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    أرجع CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    أرجع CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    أرجع CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    أرجع CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    أرجع CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    أرجع CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    أرجع CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    أرجع CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    أرجع CURLE_OK;
  }

  failf(انکڑے, "GnuTLS: cannot set TLS protocol");
  أرجع CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(بنية Curl_easy *انکڑے,
                                       بنية gtls_shared_creds **pcreds)
{
  بنية gtls_shared_creds *shared;
  صحيح rc;

  *pcreds = عدم;
  shared = curlx_calloc(1, حجم(*shared));
  إذا(!shared)
    أرجع CURLE_باہرلا_دا_حافظہ;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(انکڑے, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->ویلا = *Curl_pgrs_now(انکڑے);
  *pcreds = shared;
  أرجع CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_اتے_ref(بنية gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  إذا(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    أرجع CURLE_OK;
  }
  أرجع CURLE_BAD_FUNCTION_دلیل;
}

فراغ Curl_gtls_shared_creds_free(بنية gtls_shared_creds **pcreds)
{
  بنية gtls_shared_creds *shared = *pcreds;
  *pcreds = عدم;
  إذا(shared) {
    --shared->refcount;
    إذا(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

ساكن CURLcode gtls_populate_creds(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    gnutls_certificate_credentials_t creds)
{
  بنية ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, انکڑے);
  bool creds_are_خالی_تھاواں = TRUE;
  صحيح rc;

  إذا(!config->verifypeer) {
    infof(انکڑے, "SSL Trust: peer verification disabled");
    أرجع CURLE_OK;
  }

  infof(انکڑے, "SSL Trust Anchors:");
  إذا(config->native_ca_store) {
#جے_تعریف_شدہ USE_APPLE_SECTRUST
    infof(انکڑے, "  Native: Apple SecTrust");
    creds_are_خالی_تھاواں = FALSE;
#وإلا
    rc = gnutls_certificate_سیٹ_x509_system_trust(creds);
    إذا(rc < 0)
      infof(انکڑے, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    وإلا {
      infof(انکڑے, "  Native: %d certificates from system trust", rc);
      إذا(rc > 0)
        creds_are_خالی_تھاواں = FALSE;
    }
#جے_انت
  }

  إذا(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    إذا(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(انکڑے, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      أرجع CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.انکڑے = config->ca_info_blob->انکڑے;
    ca_info_datum.ناپ = (بنانشانی صحيح)config->ca_info_blob->len;
    rc = gnutls_certificate_سیٹ_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_خالی_تھاواں = creds_are_خالی_تھاواں && (rc <= 0);
    إذا(rc < 0) {
      infof(انکڑے, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_خالی_تھاواں ? "" : ", continuing anyway"));
      إذا(creds_are_خالی_تھاواں) {
        ssl_config->certverifyresult = rc;
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
    }
    وإلا
      infof(انکڑے, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  وإلا إذا(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_سیٹ_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_سیٹ_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_خالی_تھاواں = creds_are_خالی_تھاواں && (rc <= 0);
    إذا(rc < 0) {
      infof(انکڑے, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_خالی_تھاواں ? "" : ", continuing anyway"));
      إذا(creds_are_خالی_تھاواں) {
        ssl_config->certverifyresult = rc;
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
    }
    وإلا
      infof(انکڑے, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  إذا(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_سیٹ_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_خالی_تھاواں = creds_are_خالی_تھاواں && (rc <= 0);
    إذا(rc < 0) {
      infof(انکڑے, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_خالی_تھاواں ? "" : ", continuing anyway"));
      إذا(creds_are_خالی_تھاواں) {
        ssl_config->certverifyresult = rc;
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
    }
    وإلا
      infof(انکڑے, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  إذا(creds_are_خالی_تھاواں)
    infof(انکڑے, "  no trust anchors configured");

#جے_تعریف_شدہ USE_APPLE_SECTRUST
  إذا(config->CRLfile && config->native_ca_store) {
    failf(انکڑے, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    أرجع CURLE_NOT_BUILT_وچ;
  }
#جے_انت
  إذا(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_سیٹ_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    إذا(rc < 0) {
      failf(انکڑے, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      أرجع CURLE_SSL_CRL_BADFILE;
    }
    وإلا
      infof(انکڑے, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  أرجع CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#تعریف_کرو MPROTO_GTLS_X509_کلید   "tls:gtls:x509:share"

ساكن bool gtls_shared_creds_expired(بنية Curl_easy *انکڑے,
                                      ثابت بنية gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(انکڑے), &sc->ویلا);
  timediff_t timeout_ms = انکڑے->سیٹ.ssl_ca_cache_timeout * (timediff_t)1000;

  إذا(timeout_ms < 0)
    أرجع FALSE;

  أرجع elapsed_ms >= timeout_ms;
}

ساكن bool gtls_shared_creds_different(بنية Curl_cfilter *cf,
                                        ثابت بنية gtls_shared_creds *sc)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  إذا(!sc->CAfile || !conn_config->CAfile)
    أرجع sc->CAfile != conn_config->CAfile;

  أرجع strcmp(sc->CAfile, conn_config->CAfile);
}

ساكن بنية gtls_shared_creds *gtls_get_cached_creds(بنية Curl_cfilter *cf,
                                                       بنية Curl_easy *انکڑے)
{
  بنية gtls_shared_creds *shared_creds;

  إذا(انکڑے->multi) {
    shared_creds = Curl_hash_pick(&انکڑے->multi->proto_hash,
                                  MPROTO_GTLS_X509_کلید,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_کلید));
    إذا(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(انکڑے, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      أرجع shared_creds;
    }
  }
  أرجع عدم;
}

ساكن فراغ gtls_shared_creds_hash_free(ثابت فراغ *کلید,
                                        size_t کلید_len, فراغ *p)
{
  بنية gtls_shared_creds *sc = p;
  DEBUGASSERT(کلید_len == CURL_CSTRLEN(MPROTO_GTLS_X509_کلید));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_کلید, کلید, کلید_len));
  (فراغ)کلید;
  (فراغ)کلید_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

ساكن فراغ gtls_سیٹ_cached_creds(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  بنية gtls_shared_creds *sc)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  إذا(!انکڑے->multi)
    أرجع;

  إذا(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    إذا(!sc->CAfile)
      أرجع;
  }

  إذا(Curl_gtls_shared_creds_اتے_ref(sc))
    أرجع;

  إذا(!Curl_hash_add2(&انکڑے->multi->proto_hash,
                     MPROTO_GTLS_X509_کلید,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_کلید),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    أرجع;
  }
}

CURLcode Curl_gtls_client_trust_تیاری(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے,
                                      بنية gtls_ctx *gtls)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية gtls_shared_creds *cached_creds = عدم;
  bool cache_criteria_met;
  CURLcode نتیجہ;
  صحيح rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (انکڑے->سیٹ.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  إذا(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, انکڑے);

  إذا(cached_creds && !Curl_gtls_shared_creds_اتے_ref(cached_creds)) {
    CURL_TRC_CF(انکڑے, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_سیٹ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    إذا(rc != GNUTLS_E_SUCCESS) {
      failf(انکڑے, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }
  وإلا {
    CURL_TRC_CF(انکڑے, cf, "loading trust anchors and CRLs");
    نتیجہ = gtls_populate_creds(cf, انکڑے, gtls->shared_creds->creds);
    إذا(نتیجہ)
      أرجع نتیجہ;
    gtls->shared_creds->trust_تیاری = TRUE;
    إذا(cache_criteria_met)
      gtls_سیٹ_cached_creds(cf, انکڑے, gtls->shared_creds);
  }
  أرجع CURLE_OK;
}

#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
ساكن صحيح gtls_get_ietf_proto(gnutls_session_t session)
{
  بدل(gnutls_protocol_get_نسخہ(session)) {
  حالة GNUTLS_SSL3:
    أرجع CURL_IETF_PROTO_SSL3;
  حالة GNUTLS_TLS1_0:
    أرجع CURL_IETF_PROTO_TLS1;
  حالة GNUTLS_TLS1_1:
    أرجع CURL_IETF_PROTO_TLS1_1;
  حالة GNUTLS_TLS1_2:
    أرجع CURL_IETF_PROTO_TLS1_2;
  حالة GNUTLS_TLS1_3:
    أرجع CURL_IETF_PROTO_TLS1_3;
  افتراضي:
    أرجع CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 ثابت حرف *ssl_peer_کلید,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ثابت حرف *alpn,
                                 بنانشانی حرف *quic_tp,
                                 size_t quic_tp_len,
                                 بنية Curl_ssl_session **psession)
{
  بنية Curl_ssl_session *sc_session = عدم, *sc_dup = عدم;
  بنانشانی حرف *sdata, *qtp_clone = عدم;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode نتیجہ = CURLE_OK;

  إذا(psession)
    *psession = عدم;
  إذا(!Curl_ssl_scache_use(cf, انکڑے))
    أرجع CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_انکڑے(session, عدم, &sdata_len);
  إذا(!sdata_len) /* GnuTLS does this for some version combinations */
    أرجع CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  إذا(!sdata)
    أرجع CURLE_باہرلا_دا_حافظہ;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_انکڑے(session, sdata, &sdata_len);
  earlydata_max = gnutls_ریکارڈ_get_max_early_انکڑے_ناپ(session);

  CURL_TRC_CF(انکڑے, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  إذا(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ثابت حرف *)quic_tp, quic_tp_len);
    إذا(!qtp_clone) {
      curlx_free(sdata);
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }

  نتیجہ = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  إذا(!نتیجہ && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    نتیجہ = Curl_ssl_session_dup(sc_session, &sc_dup);
  إذا(!نتیجہ) {
    نتیجہ = Curl_ssl_scache_put(cf, انکڑے, ssl_peer_کلید, sc_session);
    /* took ownership of `sc_session` */
    sc_session = عدم;
  }
  إذا(!نتیجہ && psession) {
    *psession = sc_dup;
    sc_dup = عدم;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  أرجع نتیجہ;
}
#جے_انت

#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
ساكن CURLcode cf_gtls_update_session_id(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *انکڑے,
                                          gnutls_session_t session)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية Curl_ssl_session *scs = عدم;
  CURLcode نتیجہ;

  نتیجہ = Curl_gtls_cache_session(cf, انکڑے, connssl->peer.scache_کلید,
                                   session, 0, connssl->negotiated.alpn,
                                   عدم, 0, &scs);
  إذا(!نتیجہ) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  أرجع نتیجہ;
}

ساكن صحيح gtls_handshake_cb(gnutls_session_t session, بنانشانی صحيح htype,
                             بنانشانی when, بنانشانی صحيح incoming,
                             ثابت gnutls_datum_t *پیغام)
{
  بنية Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (فراغ)پیغام;
  (فراغ)incoming;
  إذا(when) { /* after message has been processed */
    بنية Curl_easy *انکڑے = CF_انکڑے_CURRENT(cf);
    إذا(انکڑے) {
      CURL_TRC_CF(انکڑے, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      بدل(htype) {
      حالة GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, انکڑے, session);
        اكسر;
      }
      افتراضي:
        اكسر;
      }
    }
  }
  أرجع 0;
}
#جے_انت

ساكن CURLcode gtls_سیٹ_ترجیح(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  بنية gtls_ctx *gtls,
                                  ثابت حرف *ترجیح)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية dynbuf buf;
  ثابت حرف *err = عدم;
  CURLcode نتیجہ = CURLE_OK;
  صحيح rc;

  curlx_dyn_init(&buf, 4096);

  إذا(conn_config->cipher_list) {
    إذا((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      إذا(!curlx_dyn_len(&buf)) {  /* not added yet */
        نتیجہ = curlx_dyn_جوڑو(&buf, ترجیح);
        إذا(نتیجہ)
          اذهب باہرلا;
      }
      نتیجہ = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      إذا(نتیجہ)
        اذهب باہرلا;
      ترجیح = curlx_dyn_ptr(&buf);
    }
    وإلا /* replace our own completely */
      ترجیح = conn_config->cipher_list;
  }

  infof(انکڑے, "GnuTLS priority: %s", ترجیح);
  rc = gnutls_ترجیح_سیٹ_سدھا(gtls->session, ترجیح, &err);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(انکڑے, "Error %d setting GnuTLS priority: %s", rc, err);
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
  }

باہرلا:
  curlx_dyn_free(&buf);
  أرجع نتیجہ;
}

ساكن CURLcode gtls_client_init(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 بنية ssl_peer *peer,
                                 size_t earlydata_max,
                                 بنية gtls_ctx *gtls)
{
  بنية ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, انکڑے);
  بنانشانی صحيح init_flags;
  صحيح rc;
  ثابت حرف *prioritylist;
  bool tls13support;
  CURLcode نتیجہ;

  إذا(config->نسخہ_2 == CURL_SSLVERSION_SSLv2 ||
     config->نسخہ_2 == CURL_SSLVERSION_SSLv3) {
    failf(انکڑے, "GnuTLS does not support SSLv2 or SSLv3");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  نتیجہ = Curl_gtls_shared_creds_create(انکڑے, &gtls->shared_creds);
  إذا(نتیجہ)
    أرجع نتیجہ;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
  إذا(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_انکڑے | GNUTLS_NO_سرا_دا_EARLY_انکڑے;
  وإلا إذا(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_انکڑے;
#جے_انت

#جے_تعریف_شدہ GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#جے_انت

#جے_تعریف_شدہ GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#جے_انت

#جے_تعریف_شدہ GNUTLS_NO_حالت_REQUEST
  إذا(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_حالت_REQUEST;
#جے_انت

  CURL_TRC_CF(انکڑے, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(انکڑے, "gnutls_init() failed: %d", rc);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(peer->sni) {
    إذا(gnutls_server_name_سیٹ(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, لڑی_لمبائی(peer->sni)) < 0) {
      failf(انکڑے, "Failed to set SNI");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_سیٹ_default_ترجیح(gtls->session);
  إذا(rc != GNUTLS_E_SUCCESS)
    أرجع CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_نسخہ("3.6.5");

  إذا(config->نسخہ_2 == CURL_SSLVERSION_TLSv1_3) {
    إذا(!tls13support) {
      failf(انکڑے, "This GnuTLS installation does not support TLS 1.3");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  نتیجہ = gnutls_سیٹ_ssl_نسخہ_min_max(انکڑے, peer,
                                          config, &prioritylist, tls13support);
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = gtls_سیٹ_ترجیح(cf, انکڑے, gtls, prioritylist);
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(config->clientcert) {
    إذا(!gtls->shared_creds->trust_تیاری) {
      نتیجہ = Curl_gtls_client_trust_تیاری(cf, انکڑے, gtls);
      إذا(نتیجہ)
        أرجع نتیجہ;
    }
    إذا(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_سیٹ_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->کلید_passwd ? config->کلید_passwd : "");
      إذا(rc != GNUTLS_E_SUCCESS) {
        failf(انکڑے,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
    وإلا {
      ثابت بنانشانی صحيح supported_کلید_encryption_algorithms =
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
      إذا(rc != GNUTLS_E_SUCCESS) {
        failf(انکڑے,
              "error reading X.509 %skey file: %s",
              config->کلید_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_سیٹ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(انکڑے, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(config->verifystatus) {
    rc = gnutls_ocsp_حالت_request_enable_client(gtls->session,
                                                  عدم, 0, عدم);
    إذا(rc != GNUTLS_E_SUCCESS) {
      failf(انکڑے, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  أرجع CURLE_OK;
}

#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
ساكن صحيح keylog_callback(gnutls_session_t session, ثابت حرف *label,
                           ثابت gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  إذا(crandom.ناپ != 32) {
    أرجع -1;
  }

  Curl_tls_keylog_write(label, crandom.انکڑے, crandom.ناپ,
                        secret->انکڑے, secret->ناپ);
  أرجع 0;
}

ساكن CURLcode gtls_on_session_reuse(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے,
                                      بنية alpn_spec *alpns,
                                      بنية Curl_ssl_session *scs,
                                      bool *do_early_انکڑے)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;

  connssl->earlydata_max =
    gnutls_ریکارڈ_get_max_early_انکڑے_ناپ(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  أرجع Curl_on_session_reuse(cf, انکڑے, alpns, scs, do_early_انکڑے,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#جے_انت

ساكن CURLcode gtls_apply_session(
  بنية gtls_ctx *gctx,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *انکڑے,
  بنية ssl_peer *peer,
  بنية alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  بنية Curl_ssl_session *scs,
  bool *pwas_تیاری)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_OK;
  صحيح rc;

  إذا(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_شامل_اے_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    نتیجہ = gtls_client_init(cf, انکڑے, peer, scs->earlydata_max, gctx);
    إذا(نتیجہ)
      اذهب باہرلا;
    *pwas_تیاری = TRUE;

    rc = gnutls_session_سیٹ_انکڑے(gctx->session, scs->sdata, scs->sdata_len);
    إذا(rc < 0)
      infof(انکڑے, "SSL session not accepted by GnuTLS, continuing without");
    وإلا {
      infof(انکڑے, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      إذا(conn_config->earlydata && scs->alpn &&
         !cf->conn->بٹ.connect_only) {
        bool do_early_انکڑے = FALSE;
        إذا(sess_reuse_cb) {
          نتیجہ = sess_reuse_cb(cf, انکڑے, alpns, scs, &do_early_انکڑے);
          إذا(نتیجہ)
            اذهب باہرلا;
        }
        إذا(do_early_انکڑے) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_منزل(alpns, scs->alpn);
        }
      }
    }
  }
باہرلا:
  أرجع نتیجہ;
}

CURLcode Curl_gtls_ctx_init(بنية gtls_ctx *gctx,
                            بنية Curl_cfilter *cf,
                            بنية Curl_easy *انکڑے,
                            بنية ssl_peer *peer,
                            ثابت بنية alpn_spec *alpns_requested,
                            Curl_gtls_ctx_تیاری_cb *cb_تیاری,
                            فراغ *cb_user_انکڑے,
                            فراغ *ssl_user_انکڑے,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_session *scs = عدم;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_گنتی = 0;
  bool gtls_session_تیاری = FALSE;
  بنية alpn_spec alpns;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  إذا((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(انکڑے, cf->cft, FIRSTSOCKET);
    إذا(scs) {
      نتیجہ = gtls_apply_session(gctx, cf, انکڑے, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_تیاری);
      scs = عدم;
      إذا(!نتیجہ && gtls_session_تیاری) {
        CURL_TRC_CF(انکڑے, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  إذا(!gtls_session_تیاری && conn_config->cache_session &&
     !conn_config->verifystatus) {
    نتیجہ = Curl_ssl_scache_take(cf, انکڑے, peer->scache_کلید, &scs);
    إذا(نتیجہ)
      اذهب باہرلا;
    نتیجہ = gtls_apply_session(gctx, cf, انکڑے, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_تیاری);
  }

  إذا(!gtls_session_تیاری) {
    نتیجہ = gtls_client_init(cf, انکڑے, peer, 0, gctx);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  gnutls_session_سیٹ_ptr(gctx->session, ssl_user_انکڑے);

  إذا(cb_تیاری) {
    نتیجہ = cb_تیاری(cf, انکڑے, cb_user_انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  إذا(Curl_tls_keylog_enabled()) {
    gnutls_session_سیٹ_keylog_function(gctx->session, keylog_callback);
  }
#جے_انت

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  إذا(!gtls_alpns_گنتی && alpns.گنتی) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.گنتی);
    لكل(i = 0; i < alpns.گنتی; ++i) {
      gtls_alpns[i].انکڑے = (بنانشانی حرف *)alpns.entries[i];
      gtls_alpns[i].ناپ = (بنانشانی صحيح)لڑی_لمبائی(alpns.entries[i]);
    }
    gtls_alpns_گنتی = alpns.گنتی;
  }

  إذا(gtls_alpns_گنتی &&
     gnutls_alpn_سیٹ_protocols(gctx->session,
                               gtls_alpns, (بنانشانی صحيح)gtls_alpns_گنتی,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(انکڑے, "failed setting ALPN");
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
  }

باہرلا:
  Curl_ssl_scache_return(cf, انکڑے, peer->scache_کلید, scs);
  أرجع نتیجہ;
}

ساكن CURLcode gtls_connect_step1(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  CURLcode نتیجہ;

  DEBUGASSERT(backend);

  إذا(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    أرجع CURLE_OK;

  نتیجہ = Curl_gtls_ctx_init(&backend->gtls, cf, انکڑے, &connssl->peer,
                              connssl->alpn, عدم, عدم, cf,
#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
                              gtls_on_session_reuse
#وإلا
                              عدم
#جے_انت
                              );

  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    بنية alpn_proto_buf proto;
    memset(&proto, 0, حجم(proto));
    Curl_alpn_منزل_proto_str(&proto, connssl->alpn);
    infof(انکڑے, VTLS_INFOF_ALPN_OFFER_1STR, proto.انکڑے);
  }

#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
  gnutls_handshake_سیٹ_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#جے_انت

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_سیٹ_ptr(backend->gtls.session, cf);
  gnutls_transport_سیٹ_ڈالو_function(backend->gtls.session, gtls_ڈالو);
  gnutls_transport_سیٹ_pull_function(backend->gtls.session, gtls_pull);

  أرجع CURLE_OK;
}

ساكن CURLcode pkp_pin_peer_pubkey(بنية Curl_easy *انکڑے,
                                    gnutls_x509_crt_t cert,
                                    ثابت حرف *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  بنانشانی حرف *buff1 = عدم;

  gnutls_pubkey_t کلید = عدم;

  /* Result is returned to caller */
  CURLcode نتیجہ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  إذا(!pinnedpubkey)
    أرجع CURLE_OK;

  إذا(!cert)
    أرجع نتیجہ;

  افعل {
    صحيح ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&کلید);
    إذا(ret < 0)
      اكسر; /* failed */

    ret = gnutls_pubkey_import_x509(کلید, cert, 0);
    إذا(ret < 0)
      اكسر; /* failed */

    ret = gnutls_pubkey_export(کلید, GNUTLS_X509_FMT_DER, عدم, &len1);
    إذا(ret != GNUTLS_E_SHORT_حافظہ_بفر || len1 == 0)
      اكسر; /* failed */

    buff1 = curlx_malloc(len1);
    إذا(!buff1)
      اكسر; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(کلید, GNUTLS_X509_FMT_DER, buff1, &len2);
    إذا(ret < 0 || len1 != len2)
      اكسر; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    نتیجہ = Curl_pin_peer_pubkey(انکڑے, pinnedpubkey, buff1, len1);
  } طالما(0);

  إذا(کلید)
    gnutls_pubkey_deinit(کلید);

  curlx_safefree(buff1);

  أرجع نتیجہ;
}

فراغ Curl_gtls_report_handshake(بنية Curl_easy *انکڑے, بنية gtls_ctx *gctx)
{
#جے_تعریف_شدہ CURLVERBOSE
  إذا(Curl_trc_is_verbose(انکڑے)) {
    ثابت حرف *ptr;
    gnutls_protocol_t نسخہ_2 = gnutls_protocol_get_نسخہ(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(انکڑے, "SSL connection using %s / %s",
          gnutls_protocol_get_name(نسخہ_2), ptr);
  }
#وإلا
  (فراغ)انکڑے;
  (فراغ)gctx;
#جے_انت
}

ساكن فراغ gtls_پیغام_verify_نتیجہ(بنية Curl_easy *انکڑے,
                                   بنية ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  حرف certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t ناپ = حجم(certname);
  صحيح rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &ناپ);
  إذا(rc) {
    infof(انکڑے, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  إذا(!was_verified) {
    إذا(needs_verified) {
      failf(انکڑے, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    وإلا
      infof(انکڑے, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  وإلا
    infof(انکڑے, "  common name: %s (matched)", certname);
}

ساكن فراغ gtls_infof_cert(بنية Curl_easy *انکڑے,
                            gnutls_x509_crt_t x509_cert)
{
#جے_تعریف_شدہ CURLVERBOSE
  إذا(Curl_trc_is_verbose(انکڑے)) {
    gnutls_datum_t certfields;
    صحيح rc, algo;
    time_t tstamp;
    بنانشانی صحيح بٹ;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &بٹ);
    infof(انکڑے, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(انکڑے, "  certificate version: #%d",
          gnutls_x509_crt_get_نسخہ(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    إذا(rc)
      infof(انکڑے, "Failed to get certificate name");
    وإلا {
      infof(انکڑے, "  subject: %s", certfields.انکڑے);

      tstamp = gnutls_x509_crt_get_activation_ویلا(x509_cert);
      showtime(انکڑے, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_ویلا(x509_cert);
      showtime(انکڑے, "expire date", tstamp);

      gnutls_free(certfields.انکڑے);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    إذا(rc)
      infof(انکڑے, "Failed to get certificate issuer");
    وإلا {
      infof(انکڑے, "  issuer: %s", certfields.انکڑے);

      gnutls_free(certfields.انکڑے);
    }
  }
#وإلا
  (فراغ)انکڑے;
  (فراغ)x509_cert;
#جے_انت
}

ساكن CURLcode gtls_verify_ocsp_حالت(بنية Curl_easy *انکڑے,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = عدم;
  CURLcode نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
  صحيح rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_حالت_t حالت = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_حالت_request_get(session, &ocsp_raw);
  إذا(rc == GNUTLS_E_REQUESTED_انکڑے_NOT_دستیاب) {
    failf(انکڑے, "No OCSP response received");
    اذهب باہرلا;
  }
  وإلا إذا(rc < 0) {
    failf(انکڑے, "Invalid OCSP response received");
    اذهب باہرلا;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  إذا(rc < 0) {
    failf(انکڑے, "failed to init OCSP response parsing");
    اذهب باہرلا;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  إذا(rc < 0) {
    failf(انکڑے, "failed to parse OCSP response");
    اذهب باہرلا;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  إذا(gnutls_ocsp_resp_get_حالت(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(انکڑے, "OCSP response basic part is not successful");
    اذهب باہرلا;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  لكل(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    إذا(!rc || (rc == GNUTLS_E_REQUESTED_انکڑے_NOT_دستیاب))
      اكسر;
  }
  إذا(rc < 0) {
    failf(انکڑے, "OCSP does not carry status for peer cert");
    اذهب باہرلا;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, عدم, عدم, عدم, عدم,
                                   &حالت, عدم, عدم, عدم, عدم);
  إذا(rc < 0) {
    failf(انکڑے, "failed to parse OCSP response for peer cert");
    اذهب باہرلا;
  }
  بدل(حالت) {
  حالة GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    نتیجہ = CURLE_OK;
    اكسر;
  حالة GNUTLS_OCSP_CERT_REVOKED:
    failf(انکڑے, "peer certificate is revoked");
    اكسر;
  افتراضي:
    failf(انکڑے, "peer certificate OCSP status is not good: %d", (صحيح)حالت);
    اكسر;
  }

باہرلا:
  إذا(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  أرجع نتیجہ;
}

بنية gtls_cert_chain {
  ثابت gnutls_datum_t *certs;
  بنانشانی صحيح عدد_certs;
};

#جے_تعریف_شدہ USE_APPLE_SECTRUST
ساكن CURLcode gtls_chain_get_der(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   فراغ *user_انکڑے,
                                   size_t i,
                                   بنانشانی حرف **pder,
                                   size_t *pder_len)
{
  بنية gtls_cert_chain *chain = user_انکڑے;

  (فراغ)cf;
  (فراغ)انکڑے;
  *pder_len = 0;
  *pder = عدم;

  إذا(i >= chain->عدد_certs)
    أرجع CURLE_TOO_LARGE;
  *pder = chain->certs[i].انکڑے;
  *pder_len = (size_t)chain->certs[i].ناپ;
  أرجع CURLE_OK;
}
#جے_انت /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ساكن CURLcode gtls_verify_cert(بنية Curl_easy *انکڑے,
                                 بنية ssl_filter_config *config,
                                 بنية ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 بنية Curl_cfilter *cf,
                                 بنية ssl_peer *peer,
                                 بنية gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  بنانشانی صحيح verify_حالت = 0;
  لما * ثابت certverifyresult = &ssl_config->certverifyresult;
  صحيح rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_حالت);
  إذا(rc < 0) {
    failf(انکڑے, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    أرجع CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_حالت;
  verified = !(verify_حالت & GNUTLS_CERT_INVALID);
  إذا(verified)
    infof(انکڑے, "  SSL certificate verified by GnuTLS");

#جے_تعریف_شدہ USE_APPLE_SECTRUST
  إذا(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode نتیجہ;

    إذا(gnutls_ocsp_حالت_request_get(session, &ocsp_raw)) {
      ocsp_raw.انکڑے = عدم;
      ocsp_raw.ناپ = 0;
    }
    إذا(config->verifystatus && !ocsp_raw.ناپ) {
      failf(انکڑے, "No OCSP response received");
      أرجع CURLE_SSL_INVALIDCERTSTATUS;
    }
    نتیجہ = Curl_vtls_apple_verify(cf, انکڑے, peer, chain->عدد_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.انکڑے, ocsp_raw.ناپ,
                                    config->verifystatus);
    إذا(نتیجہ && (نتیجہ != CURLE_PEER_FAILED_VERIFICATION))
      أرجع نتیجہ; /* no further failure analysis needed */
    verified = !نتیجہ;
    إذا(verified) {
      infof(انکڑے, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#وإلا
  (فراغ)cf;
  (فراغ)peer;
  (فراغ)chain;
#جے_انت

  إذا(!verified) {
    CURLcode نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ثابت حرف *cause = "certificate error, no details available";
    إذا(verify_حالت & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    وإلا إذا(verify_حالت & GNUTLS_CERT_SIGNER_NOT_لبھیا)
      cause = "certificate signer not trusted";
    وإلا إذا(verify_حالت & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    وإلا إذا(verify_حالت & GNUTLS_CERT_INVALID_OCSP_حالت) {
      cause = "OCSP status response is invalid";
      نتیجہ = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(انکڑے, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    أرجع نتیجہ;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                gnutls_session_t session,
                                بنية ssl_filter_config *config,
                                بنية ssl_easy_config *ssl_config,
                                بنية ssl_peer *peer,
                                ثابت حرف *pinned_کلید)
{
  بنية gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = عدم, x509_issuer = عدم;
  time_t certclock;
  bool ocsp_checked = FALSE;
  صحيح rc;
  CURLcode نتیجہ = CURLE_OK;
  لما * ثابت certverifyresult = &ssl_config->certverifyresult;

  (فراغ)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.عدد_certs);
  إذا(!chain.certs) {
    إذا(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(انکڑے, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_لبھیا;
      نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
      اذهب باہرلا;
    }
    infof(انکڑے, " common name: WARNING could not obtain");
  }

  إذا(انکڑے->سیٹ.ssl.certinfo && chain.certs) {
    إذا(chain.عدد_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(انکڑے, "%u certificates is more than allowed (%d)",
            chain.عدد_certs, MAX_ALLOWED_CERT_AMOUNT);
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      اذهب باہرلا;
    }
    وإلا {
      بنانشانی صحيح i;

      نتیجہ = Curl_ssl_init_certinfo(انکڑے, (صحيح)chain.عدد_certs);
      إذا(نتیجہ)
        اذهب باہرلا;

      لكل(i = 0; i < chain.عدد_certs; i++) {
        ثابت uint8_t *beg = chain.certs[i].انکڑے;
        ثابت uint8_t *سرا = beg + chain.certs[i].ناپ;

        نتیجہ = Curl_کڈھو_certinfo(انکڑے, (صحيح)i, beg, سرا);
        إذا(نتیجہ)
          اذهب باہرلا;
      }
    }
  }

  إذا(config->verifypeer) {
    نتیجہ = gtls_verify_cert(انکڑے, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  وإلا
    infof(انکڑے, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  إذا(gnutls_x509_crt_init(&x509_cert)) {
    failf(انکڑے, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_لبھیا;
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
    اذهب باہرلا;
  }

  إذا(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    إذا(rc) {
      failf(انکڑے, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_لبھیا;
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      اذهب باہرلا;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_ویلا(x509_cert);

  إذا(certclock == (time_t)-1) {
    إذا(config->verifypeer) {
      failf(انکڑے, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      اذهب باہرلا;
    }
    وإلا
      infof(انکڑے, "  SSL certificate expiration date verify FAILED");
  }
  وإلا {
    إذا(certclock < ویلا(عدم)) {
      إذا(config->verifypeer) {
        failf(انکڑے, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
        اذهب باہرلا;
      }
      وإلا
        infof(انکڑے, "  SSL certificate expiration date FAILED");
    }
    وإلا
      infof(انکڑے, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_ویلا(x509_cert);

  إذا(certclock == (time_t)-1) {
    إذا(config->verifypeer) {
      failf(انکڑے, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      اذهب باہرلا;
    }
    وإلا
      infof(انکڑے, "  SSL certificate activation date verify FAILED");
  }
  وإلا {
    إذا(certclock > ویلا(عدم)) {
      إذا(config->verifypeer) {
        failf(انکڑے, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        نتیجہ = CURLE_PEER_FAILED_VERIFICATION;
        اذهب باہرلا;
      }
      وإلا
        infof(انکڑے, "  SSL certificate activation date FAILED");
    }
    وإلا
      infof(انکڑے, "  SSL certificate activation date OK");
  }

  إذا(config->verifystatus && !ocsp_checked) {
    نتیجہ = gtls_verify_ocsp_حالت(انکڑے, session, x509_cert);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  وإلا
    infof(انکڑے, "  SSL certificate status verification SKIPPED");

  إذا(config->issuercert) {
    gnutls_datum_t issuerp;
    إذا(gnutls_x509_crt_init(&x509_issuer)) {
      failf(انکڑے, "failed to init GnuTLS x509_crt for issuer");
      نتیجہ = CURLE_SSL_ISSUER_ERROR;
      اذهب باہرلا;
    }
    issuerp = لدو_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    إذا(rc) {
      failf(انکڑے, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      نتیجہ = CURLE_SSL_ISSUER_ERROR;
      اذهب باہرلا;
    }
    rc = (صحيح)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    إذا(rc <= 0) {
      failf(انکڑے, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      نتیجہ = CURLE_SSL_ISSUER_ERROR;
      اذهب باہرلا;
    }
    infof(انکڑے, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  rc = (صحيح)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  نتیجہ = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_پیغام_verify_نتیجہ(انکڑے, peer, x509_cert, rc, config->verifyhost);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(pinned_کلید) {
    نتیجہ = pkp_pin_peer_pubkey(انکڑے, x509_cert, pinned_کلید);
    إذا(نتیجہ != CURLE_OK) {
      failf(انکڑے, "SSL: public key does not match pinned public key");
      اذهب باہرلا;
    }
  }

  gtls_infof_cert(انکڑے, x509_cert);

باہرلا:
  إذا(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  إذا(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  أرجع نتیجہ;
}

ساكن CURLcode gtls_verifyserver(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *انکڑے,
                                  gnutls_session_t session)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, انکڑے);
#جے_بنا_تعریف CURL_DISABLE_PROXY
  ثابت حرف *pinned_کلید = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(انکڑے, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(انکڑے, STRING_SSL_PINNEDPUBLICKEY);
#وإلا
  ثابت حرف *pinned_کلید = CURL_EASY_STR(انکڑے, STRING_SSL_PINNEDPUBLICKEY);
#جے_انت
  CURLcode نتیجہ;

  نتیجہ = Curl_gtls_verifyserver(cf, انکڑے, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_کلید);
  إذا(نتیجہ)
    اذهب باہرلا;

#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  إذا(gnutls_protocol_get_نسخہ(session) < GNUTLS_TLS1_3)
    نتیجہ = cf_gtls_update_session_id(cf, انکڑے, session);
#جے_انت

باہرلا:
  أرجع نتیجہ;
}

#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
ساكن CURLcode gtls_send_earlydata(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  ثابت بنانشانی حرف *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_نتیجہ = CURLE_OK;
  طالما(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_ریکارڈ_send_early_انکڑے(backend->gtls.session, buf, blen);
    CURL_TRC_CF(انکڑے, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    إذا(n < 0) {
      إذا(n == GNUTLS_E_AGAIN)
        نتیجہ = CURLE_AGAIN;
      وإلا
        نتیجہ = backend->gtls.io_نتیجہ ?
                 backend->gtls.io_نتیجہ : CURLE_SEND_ERROR;
      اذهب باہرلا;
    }
    وإلا إذا(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(انکڑے, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
باہرلا:
  أرجع نتیجہ;
}
#جے_انت

/*
 * This function is called after the TCP connect has completed. Setup the TLS
 * layer and do all necessary magic.
 */
/* We use connssl->connecting_state to keep track of the connection status;
   there are three states: 'ssl_connect_1' (not started yet or complete),
   'ssl_connect_2' (doing handshake with the server), and
   'ssl_connect_3' (verifying and getting stats).
 */
ساكن CURLcode gtls_connect_common(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *انکڑے,
                                    bool *پورا)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *پورا = TRUE;
    أرجع CURLE_OK;
  }

  *پورا = FALSE;

  /* Initiate the connection, if not already done */
  إذا(connssl->connecting_state == ssl_connect_1) {
    نتیجہ = gtls_connect_step1(cf, انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
    connssl->connecting_state = ssl_connect_2;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
    إذا(connssl->earlydata_state == ssl_earlydata_await) {
      اذهب باہرلا;
    }
    وإلا إذا(connssl->earlydata_state == ssl_earlydata_sending) {
      نتیجہ = gtls_send_earlydata(cf, انکڑے);
      إذا(نتیجہ)
        اذهب باہرلا;
      connssl->earlydata_state = ssl_earlydata_گھلیا;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_گھلیا));
#جے_انت
    نتیجہ = cf_gtls_handshake(cf, انکڑے);
    إذا(نتیجہ)
      اذهب باہرلا;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  إذا(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    صحيح rc;

    Curl_gtls_report_handshake(انکڑے, &backend->gtls);

    نتیجہ = gtls_verifyserver(cf, انکڑے, backend->gtls.session);
    إذا(نتیجہ)
      اذهب باہرلا;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    إذا(rc) {  /* No ALPN from server */
      proto.انکڑے = عدم;
      proto.ناپ = 0;
    }

    نتیجہ = Curl_alpn_سیٹ_negotiated(cf, انکڑے, connssl,
                                      proto.انکڑے, proto.ناپ);
    إذا(نتیجہ)
      اذهب باہرلا;

#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
    إذا(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_گھلیا);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_انکڑے) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#جے_انت
    connssl->connecting_state = ssl_connect_پورا;
  }

  إذا(connssl->connecting_state == ssl_connect_پورا)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

باہرلا:
  إذا(نتیجہ == CURLE_AGAIN) {
    *پورا = FALSE;
    أرجع CURLE_OK;
  }
  *پورا = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(انکڑے, cf, "gtls_connect_common() -> %d, done=%d", (صحيح)نتیجہ,
              *پورا);
  أرجع نتیجہ;
}

ساكن CURLcode gtls_connect(بنية Curl_cfilter *cf,
                             بنية Curl_easy *انکڑے,
                             bool *پورا)
{
#جے_تعریف_شدہ CURL_GNUTLS_EARLY_انکڑے
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  إذا((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *پورا = TRUE;
    أرجع CURLE_OK;
  }
#جے_انت
  أرجع gtls_connect_common(cf, انکڑے, پورا);
}

ساكن bool gtls_انکڑے_pending(بنية Curl_cfilter *cf,
                              ثابت بنية Curl_easy *انکڑے)
{
  بنية ssl_connect_انکڑے *ctx = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend;

  (فراغ)انکڑے;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية gtls_ssl_backend_انکڑے *)ctx->backend;
  إذا(backend->gtls.session &&
     gnutls_ریکارڈ_check_pending(backend->gtls.session) != 0)
    أرجع TRUE;
  أرجع FALSE;
}

ساكن CURLcode gtls_send(بنية Curl_cfilter *cf,
                          بنية Curl_easy *انکڑے,
                          ثابت فراغ *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  طالما(remain) {
    backend->gtls.io_نتیجہ = CURLE_OK;
    nwritten = gnutls_ریکارڈ_send(backend->gtls.session, buf, remain);

    إذا(nwritten >= 0) {
      إذا((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(انکڑے, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        أرجع CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (حرف *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    وإلا {
      إذا(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        نتیجہ = CURLE_OK;
        اذهب باہرلا;
      }
      نتیجہ = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_نتیجہ ? backend->gtls.io_نتیجہ : CURLE_SEND_ERROR);
      اذهب باہرلا;
    }
  }

باہرلا:
  CURL_TRC_CF(انکڑے, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (صحيح)نتیجہ, *pnwritten);
  أرجع نتیجہ;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ساكن CURLcode gtls_shutdown(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے,
                              bool send_shutdown, bool *پورا)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  حرف buf[1024];
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  إذا(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *پورا = TRUE;
    اذهب باہرلا;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *پورا = FALSE;

  إذا(!backend->gtls.گھلیا_shutdown) {
    /* do this only once */
    backend->gtls.گھلیا_shutdown = TRUE;
    إذا(send_shutdown) {
      صحيح ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      إذا((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(انکڑے, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_ریکارڈ_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.گھلیا_shutdown = FALSE;
        نتیجہ = CURLE_OK;
        اذهب باہرلا;
      }
      إذا(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(انکڑے, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((صحيح)ret), (صحيح)ret);
        نتیجہ = CURLE_RECV_ERROR;
        اذهب باہرلا;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  لكل(i = 0; i < 10; ++i) {
    nread = gnutls_ریکارڈ_recv(backend->gtls.session, buf, حجم(buf));
    إذا(nread <= 0)
      اكسر;
  }
  إذا(nread > 0) {
    /* still data coming in? */
  }
  وإلا إذا(nread == 0) {
    /* We got the close notify alert and are done. */
    *پورا = TRUE;
  }
  وإلا إذا((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_ریکارڈ_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  وإلا {
    CURL_TRC_CF(انکڑے, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((صحيح)nread), (صحيح)nread);
    نتیجہ = CURLE_RECV_ERROR;
  }

باہرلا:
  cf->shutdown = (نتیجہ || *پورا);
  أرجع نتیجہ;
}

ساكن فراغ gtls_close(بنية Curl_cfilter *cf,
                       بنية Curl_easy *انکڑے)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(انکڑے, cf, "close");
  إذا(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = عدم;
  }
  إذا(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ساكن CURLcode gtls_recv(بنية Curl_cfilter *cf,
                          بنية Curl_easy *انکڑے,
                          حرف *buf, size_t blen,
                          size_t *pnread)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_ریکارڈ_recv(backend->gtls.session, buf, blen);

  إذا(nread >= 0)
    *pnread = (size_t)nread;
  وإلا {
    إذا((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      نتیجہ = CURLE_AGAIN;
      اذهب باہرلا;
    }
    وإلا إذا(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      نتیجہ = cf_gtls_handshake(cf, انکڑے);
      إذا(!نتیجہ)
        نتیجہ = CURLE_AGAIN; /* make us get called again. */
      اذهب باہرلا;
    }
    وإلا {
      failf(انکڑے, "GnuTLS recv error (%d): %s",
            (صحيح)nread, gnutls_strerror((صحيح)nread));
      نتیجہ = backend->gtls.io_نتیجہ ?
        backend->gtls.io_نتیجہ : CURLE_RECV_ERROR;
      اذهب باہرلا;
    }
  }

باہرلا:
  CURL_TRC_CF(انکڑے, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (صحيح)نتیجہ, nread);
  أرجع نتیجہ;
}

size_t Curl_gtls_نسخہ(حرف *بفر, size_t ناپ)
{
  أرجع curl_msnprintf(بفر, ناپ, "GnuTLS/%s", gnutls_check_نسخہ(عدم));
}

/* data might be NULL! */
ساكن CURLcode gtls_random(بنية Curl_easy *انکڑے,
                            بنانشانی حرف *entropy, size_t لمائی)
{
  صحيح rc;
  (فراغ)انکڑے;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, لمائی);
  أرجع rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ساكن CURLcode gtls_sha256sum(ثابت بنانشانی حرف *داخلہ,
                               size_t len,
                               بنانشانی حرف *sha256sum, /* output */
                               size_t sha256len)
{
  بنية sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  افعل {
    بنانشانی صحيح ilen = (بنانشانی صحيح)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, داخلہ);
    len -= ilen;
    داخلہ += ilen;
  } طالما(len);
#إذا NETTLE_نسخہ_MAJOR >= 4
  (فراغ)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#وإلا
  sha256_digest(&SHA256pw, (بنانشانی صحيح)sha256len, sha256sum);
#جے_انت
  أرجع CURLE_OK;
}

ساكن bool gtls_cert_حالت_request(فراغ)
{
  أرجع TRUE;
}

ساكن فراغ *gtls_get_internals(بنية ssl_connect_انکڑے *connssl,
                                CURLINFO info)
{
  بنية gtls_ssl_backend_انکڑے *backend =
    (بنية gtls_ssl_backend_انکڑے *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع backend->gtls.session;
}

ثابت بنية Curl_ssl Curl_ssl_gnutls = {
  { CURLSSLBACKEND_GNUTLS, "gnutls" }, /* info */

  SSLSUPP_CA_رستہ  |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_CA_CACHE |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_CRLFILE,

  حجم(بنية gtls_ssl_backend_انکڑے),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_نسخہ,             /*نسخہ*/
  gtls_shutdown,                 /* shutdown */
  gtls_انکڑے_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_حالت_request,      /* cert_status_request */
  gtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,       /* adjust_pollset */
  gtls_get_internals,            /* get_internals */
  gtls_close,                    /* close_one */
  عدم,                          /* close_all */
  عدم,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  عدم,                          /* get_channel_binding */
};

#جے_انت /* USE_GNUTLS */
