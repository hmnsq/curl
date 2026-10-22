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

#جيڪڏهن_وصف_ٿيل USE_GNUTLS

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
#وصف_ڪريو GTLSDEBUG
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل GTLSDEBUG
ساكن فراغ tls_log_func(صحيح level, ثابت حرف *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#جيڪڏهن_پورو

#إذا !defined(GNUTLS_نسخو_عدد) || (GNUTLS_نسخو_عدد < 0x030605)
#غلطي "too old GnuTLS version"
#جيڪڏهن_پورو

#وصف_رد CURL_GNUTLS_EARLY_معلومات
#إذا GNUTLS_نسخو_عدد >= 0x030702
#وصف_ڪريو CURL_GNUTLS_EARLY_معلومات
#جيڪڏهن_پورو

#ضمن <gnutls/ocsp.h>

بنية gtls_ssl_backend_معلومات {
  بنية gtls_ctx gtls;
};

ساكن ssize_t gtls_ڌڪو(فراغ *s, ثابت فراغ *buf, size_t blen)
{
  بنية Curl_cfilter *cf = s;
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  size_t nwritten;
  CURLcode نتيجو;

  DEBUGASSERT(معلومات);
  نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(معلومات, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (صحيح)نتيجو, nwritten);
  backend->gtls.io_نتيجو = نتيجو;
  إذا(نتيجو) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_سيٽ_errno(backend->gtls.session,
                               (نتيجو == CURLE_AGAIN) ? EAGAIN : EINVAL);
    أرجع -1;
  }
  أرجع (ssize_t)nwritten;
}

ساكن ssize_t gtls_pull(فراغ *s, فراغ *buf, size_t blen)
{
  بنية Curl_cfilter *cf = s;
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  size_t nread;
  CURLcode نتيجو;

  DEBUGASSERT(معلومات);
  إذا(!backend->gtls.shared_creds->trust_تياري) {
    نتيجو = Curl_gtls_client_trust_تياري(cf, معلومات, &backend->gtls);
    إذا(نتيجو) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_سيٽ_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_نتيجو = نتيجو;
      أرجع -1;
    }
  }

  نتيجو = Curl_conn_cf_recv(cf->ايندڙ, معلومات, buf, blen, &nread);
  CURL_TRC_CF(معلومات, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (صحيح)نتيجو,
              nread);
  backend->gtls.io_نتيجو = نتيجو;
  إذا(نتيجو) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_سيٽ_errno(backend->gtls.session,
                               (نتيجو == CURLE_AGAIN) ? EAGAIN : EINVAL);
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
#جيڪڏهن_وصف_ٿيل GTLSDEBUG
  gnutls_global_سيٽ_log_function(tls_log_func);
  gnutls_global_سيٽ_log_level(2);
#جيڪڏهن_پورو
  أرجع ret;
}

ساكن فراغ gtls_cleanup(فراغ)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
ساكن فراغ showtime(بنية Curl_easy *معلومات, ثابت حرف *متن_3, time_t stamp)
{
  بنية tm بفر;
  ثابت بنية tm *tm = &بفر;
  حرف str[96];
  CURLcode نتيجو = curlx_gmtime(stamp, &بفر);
  إذا(نتيجو)
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
  infof(معلومات, "%s", str);
}
#جيڪڏهن_پورو

ساكن gnutls_datum_t ڀريو_file(ثابت حرف *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { عدم, 0 };
  ڊگهو filelen;
  فراغ *ptr;

  f = curlx_fopen(file, "rb");
  إذا(!f)
    أرجع loaded_file;
  إذا(fseek(f, 0, SEEK_END))
    اذهب ٻاهريون;
  filelen = ftell(f);
  إذا(filelen < 0 || filelen > CURL_MAX_داخلا_ڊيگهائي)
    اذهب ٻاهريون;
  إذا(fseek(f, 0, SEEK_SET))
    اذهب ٻاهريون;
  ptr = curlx_malloc((size_t)filelen);
  إذا(!ptr)
    اذهب ٻاهريون;
  إذا(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    اذهب ٻاهريون;
  }

  loaded_file.معلومات = ptr;
  loaded_file.سائيز = (بنانشان صحيح)filelen;
ٻاهريون:
  curlx_fclose(f);
  أرجع loaded_file;
}

ساكن فراغ unload_file(gnutls_datum_t معلومات)
{
  curlx_free(معلومات.معلومات);
}

/* this function does an SSL/TLS (re-)handshake */
ساكن CURLcode cf_gtls_handshake(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  gnutls_session_t session;
  صحيح rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_نتيجو = CURLE_OK;
  rc = gnutls_handshake(session);

  إذا(!backend->gtls.shared_creds->trust_تياري) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode نتيجو = Curl_gtls_client_trust_تياري(cf, معلومات, &backend->gtls);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_رڪارڊ_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    أرجع CURLE_AGAIN;
  }
  وإلا إذا((rc < 0) && !gnutls_error_is_fatal(rc)) {
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
    ثابت حرف *strerr = عدم;

    إذا(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    إذا(!strerr)
      strerr = gnutls_strerror(rc);

    infof(معلومات, "gnutls_handshake() warning: %s", strerr);
#جيڪڏهن_پورو
    أرجع CURLE_AGAIN;
  }
  وإلا إذا((rc < 0) && backend->gtls.io_نتيجو) {
    أرجع backend->gtls.io_نتيجو;
  }
  وإلا إذا(rc < 0) {
    ثابت حرف *strerr = عدم;

    إذا(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    إذا(!strerr)
      strerr = gnutls_strerror(rc);

    failf(معلومات, "GnuTLS, handshake failed: %s", strerr);
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

#وصف_ڪريو GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#وصف_ڪريو QUIC_ترجيح \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ساكن CURLcode gnutls_سيٽ_ssl_نسخو_min_max(
  بنية Curl_easy *معلومات,
  بنية ssl_peer *peer,
  بنية ssl_filter_config *conn_config,
  ثابت حرف **prioritylist,
  bool tls13support)
{
  ڊگهو ssl_نسخو = conn_config->نسخو_2;
  ڊگهو ssl_نسخو_max = conn_config->نسخو_max;

  DEBUGASSERT(ssl_نسخو != CURL_SSLVERSION_DEFAULT);
  إذا(ssl_نسخو <= CURL_SSLVERSION_TLSv1)
    ssl_نسخو = CURL_SSLVERSION_TLSv1_0;
  إذا((ssl_نسخو_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_نسخو_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_نسخو_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  إذا(peer->transport == TRNSPRT_QUIC) {
    إذا(ssl_نسخو_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(معلومات, "QUIC needs at least TLS version 1.3");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_ترجيح;
    أرجع CURLE_OK;
  }

  بدل(ssl_نسخو | ssl_نسخو_max) {
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

  failf(معلومات, "GnuTLS: cannot set TLS protocol");
  أرجع CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(بنية Curl_easy *معلومات,
                                       بنية gtls_shared_creds **pcreds)
{
  بنية gtls_shared_creds *shared;
  صحيح rc;

  *pcreds = عدم;
  shared = curlx_calloc(1, حجم(*shared));
  إذا(!shared)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(معلومات, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->وقت = *Curl_pgrs_now(معلومات);
  *pcreds = shared;
  أرجع CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_مٿي_ref(بنية gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  إذا(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    أرجع CURLE_OK;
  }
  أرجع CURLE_BAD_FUNCTION_دليل;
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
                                    بنية Curl_easy *معلومات,
                                    gnutls_certificate_credentials_t creds)
{
  بنية ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, معلومات);
  bool creds_are_خالي_جايون = TRUE;
  صحيح rc;

  إذا(!config->verifypeer) {
    infof(معلومات, "SSL Trust: peer verification disabled");
    أرجع CURLE_OK;
  }

  infof(معلومات, "SSL Trust Anchors:");
  إذا(config->native_ca_store) {
#جيڪڏهن_وصف_ٿيل USE_APPLE_SECTRUST
    infof(معلومات, "  Native: Apple SecTrust");
    creds_are_خالي_جايون = FALSE;
#وإلا
    rc = gnutls_certificate_سيٽ_x509_system_trust(creds);
    إذا(rc < 0)
      infof(معلومات, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    وإلا {
      infof(معلومات, "  Native: %d certificates from system trust", rc);
      إذا(rc > 0)
        creds_are_خالي_جايون = FALSE;
    }
#جيڪڏهن_پورو
  }

  إذا(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    إذا(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(معلومات, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      أرجع CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.معلومات = config->ca_info_blob->معلومات;
    ca_info_datum.سائيز = (بنانشان صحيح)config->ca_info_blob->len;
    rc = gnutls_certificate_سيٽ_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_خالي_جايون = creds_are_خالي_جايون && (rc <= 0);
    إذا(rc < 0) {
      infof(معلومات, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_خالي_جايون ? "" : ", continuing anyway"));
      إذا(creds_are_خالي_جايون) {
        ssl_config->certverifyresult = rc;
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
    }
    وإلا
      infof(معلومات, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  وإلا إذا(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_سيٽ_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_سيٽ_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_خالي_جايون = creds_are_خالي_جايون && (rc <= 0);
    إذا(rc < 0) {
      infof(معلومات, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_خالي_جايون ? "" : ", continuing anyway"));
      إذا(creds_are_خالي_جايون) {
        ssl_config->certverifyresult = rc;
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
    }
    وإلا
      infof(معلومات, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  إذا(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_سيٽ_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_خالي_جايون = creds_are_خالي_جايون && (rc <= 0);
    إذا(rc < 0) {
      infof(معلومات, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_خالي_جايون ? "" : ", continuing anyway"));
      إذا(creds_are_خالي_جايون) {
        ssl_config->certverifyresult = rc;
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
    }
    وإلا
      infof(معلومات, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  إذا(creds_are_خالي_جايون)
    infof(معلومات, "  no trust anchors configured");

#جيڪڏهن_وصف_ٿيل USE_APPLE_SECTRUST
  إذا(config->CRLfile && config->native_ca_store) {
    failf(معلومات, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    أرجع CURLE_NOT_BUILT_اندريون;
  }
#جيڪڏهن_پورو
  إذا(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_سيٽ_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    إذا(rc < 0) {
      failf(معلومات, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      أرجع CURLE_SSL_CRL_BADFILE;
    }
    وإلا
      infof(معلومات, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  أرجع CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#وصف_ڪريو MPROTO_GTLS_X509_ڪنجي   "tls:gtls:x509:share"

ساكن bool gtls_shared_creds_expired(بنية Curl_easy *معلومات,
                                      ثابت بنية gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(معلومات), &sc->وقت);
  timediff_t timeout_ms = معلومات->سيٽ.ssl_ca_cache_timeout * (timediff_t)1000;

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
                                                       بنية Curl_easy *معلومات)
{
  بنية gtls_shared_creds *shared_creds;

  إذا(معلومات->multi) {
    shared_creds = Curl_hash_pick(&معلومات->multi->proto_hash,
                                  MPROTO_GTLS_X509_ڪنجي,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_ڪنجي));
    إذا(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(معلومات, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      أرجع shared_creds;
    }
  }
  أرجع عدم;
}

ساكن فراغ gtls_shared_creds_hash_free(ثابت فراغ *ڪنجي,
                                        size_t ڪنجي_len, فراغ *p)
{
  بنية gtls_shared_creds *sc = p;
  DEBUGASSERT(ڪنجي_len == CURL_CSTRLEN(MPROTO_GTLS_X509_ڪنجي));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_ڪنجي, ڪنجي, ڪنجي_len));
  (فراغ)ڪنجي;
  (فراغ)ڪنجي_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

ساكن فراغ gtls_سيٽ_cached_creds(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية gtls_shared_creds *sc)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  إذا(!معلومات->multi)
    أرجع;

  إذا(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    إذا(!sc->CAfile)
      أرجع;
  }

  إذا(Curl_gtls_shared_creds_مٿي_ref(sc))
    أرجع;

  إذا(!Curl_hash_add2(&معلومات->multi->proto_hash,
                     MPROTO_GTLS_X509_ڪنجي,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_ڪنجي),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    أرجع;
  }
}

CURLcode Curl_gtls_client_trust_تياري(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      بنية gtls_ctx *gtls)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية gtls_shared_creds *cached_creds = عدم;
  bool cache_criteria_met;
  CURLcode نتيجو;
  صحيح rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (معلومات->سيٽ.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  إذا(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, معلومات);

  إذا(cached_creds && !Curl_gtls_shared_creds_مٿي_ref(cached_creds)) {
    CURL_TRC_CF(معلومات, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_سيٽ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    إذا(rc != GNUTLS_E_SUCCESS) {
      failf(معلومات, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "loading trust anchors and CRLs");
    نتيجو = gtls_populate_creds(cf, معلومات, gtls->shared_creds->creds);
    إذا(نتيجو)
      أرجع نتيجو;
    gtls->shared_creds->trust_تياري = TRUE;
    إذا(cache_criteria_met)
      gtls_سيٽ_cached_creds(cf, معلومات, gtls->shared_creds);
  }
  أرجع CURLE_OK;
}

#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
ساكن صحيح gtls_get_ietf_proto(gnutls_session_t session)
{
  بدل(gnutls_protocol_get_نسخو(session)) {
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
                                 بنية Curl_easy *معلومات,
                                 ثابت حرف *ssl_peer_ڪنجي,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ثابت حرف *alpn,
                                 بنانشان حرف *quic_tp,
                                 size_t quic_tp_len,
                                 بنية Curl_ssl_session **psession)
{
  بنية Curl_ssl_session *sc_session = عدم, *sc_dup = عدم;
  بنانشان حرف *sdata, *qtp_clone = عدم;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode نتيجو = CURLE_OK;

  إذا(psession)
    *psession = عدم;
  إذا(!Curl_ssl_scache_use(cf, معلومات))
    أرجع CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_معلومات(session, عدم, &sdata_len);
  إذا(!sdata_len) /* GnuTLS does this for some version combinations */
    أرجع CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  إذا(!sdata)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_معلومات(session, sdata, &sdata_len);
  earlydata_max = gnutls_رڪارڊ_get_max_early_معلومات_سائيز(session);

  CURL_TRC_CF(معلومات, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  إذا(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ثابت حرف *)quic_tp, quic_tp_len);
    إذا(!qtp_clone) {
      curlx_free(sdata);
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }

  نتيجو = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  إذا(!نتيجو && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    نتيجو = Curl_ssl_session_dup(sc_session, &sc_dup);
  إذا(!نتيجو) {
    نتيجو = Curl_ssl_scache_put(cf, معلومات, ssl_peer_ڪنجي, sc_session);
    /* took ownership of `sc_session` */
    sc_session = عدم;
  }
  إذا(!نتيجو && psession) {
    *psession = sc_dup;
    sc_dup = عدم;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  أرجع نتيجو;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
ساكن CURLcode cf_gtls_update_session_id(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *معلومات,
                                          gnutls_session_t session)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية Curl_ssl_session *scs = عدم;
  CURLcode نتيجو;

  نتيجو = Curl_gtls_cache_session(cf, معلومات, connssl->peer.scache_ڪنجي,
                                   session, 0, connssl->negotiated.alpn,
                                   عدم, 0, &scs);
  إذا(!نتيجو) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  أرجع نتيجو;
}

ساكن صحيح gtls_handshake_cb(gnutls_session_t session, بنانشان صحيح htype,
                             بنانشان when, بنانشان صحيح incoming,
                             ثابت gnutls_datum_t *نياپو)
{
  بنية Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (فراغ)نياپو;
  (فراغ)incoming;
  إذا(when) { /* after message has been processed */
    بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
    إذا(معلومات) {
      CURL_TRC_CF(معلومات, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      بدل(htype) {
      حالة GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, معلومات, session);
        اكسر;
      }
      افتراضي:
        اكسر;
      }
    }
  }
  أرجع 0;
}
#جيڪڏهن_پورو

ساكن CURLcode gtls_سيٽ_ترجيح(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية gtls_ctx *gtls,
                                  ثابت حرف *ترجيح)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية dynbuf buf;
  ثابت حرف *err = عدم;
  CURLcode نتيجو = CURLE_OK;
  صحيح rc;

  curlx_dyn_init(&buf, 4096);

  إذا(conn_config->cipher_list) {
    إذا((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      إذا(!curlx_dyn_len(&buf)) {  /* not added yet */
        نتيجو = curlx_dyn_جوڙيو(&buf, ترجيح);
        إذا(نتيجو)
          اذهب ٻاهريون;
      }
      نتيجو = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      إذا(نتيجو)
        اذهب ٻاهريون;
      ترجيح = curlx_dyn_ptr(&buf);
    }
    وإلا /* replace our own completely */
      ترجيح = conn_config->cipher_list;
  }

  infof(معلومات, "GnuTLS priority: %s", ترجيح);
  rc = gnutls_ترجيح_سيٽ_سڌو(gtls->session, ترجيح, &err);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(معلومات, "Error %d setting GnuTLS priority: %s", rc, err);
    نتيجو = CURLE_SSL_CONNECT_ERROR;
  }

ٻاهريون:
  curlx_dyn_free(&buf);
  أرجع نتيجو;
}

ساكن CURLcode gtls_client_init(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية ssl_peer *peer,
                                 size_t earlydata_max,
                                 بنية gtls_ctx *gtls)
{
  بنية ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, معلومات);
  بنانشان صحيح init_flags;
  صحيح rc;
  ثابت حرف *prioritylist;
  bool tls13support;
  CURLcode نتيجو;

  إذا(config->نسخو_2 == CURL_SSLVERSION_SSLv2 ||
     config->نسخو_2 == CURL_SSLVERSION_SSLv3) {
    failf(معلومات, "GnuTLS does not support SSLv2 or SSLv3");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  نتيجو = Curl_gtls_shared_creds_create(معلومات, &gtls->shared_creds);
  إذا(نتيجو)
    أرجع نتيجو;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
  إذا(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_معلومات | GNUTLS_NO_پڇاڙي_جو_EARLY_معلومات;
  وإلا إذا(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_معلومات;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل GNUTLS_NO_حالت_REQUEST
  إذا(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_حالت_REQUEST;
#جيڪڏهن_پورو

  CURL_TRC_CF(معلومات, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(معلومات, "gnutls_init() failed: %d", rc);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(peer->sni) {
    إذا(gnutls_server_name_سيٽ(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, لڙي_ڊيگهه(peer->sni)) < 0) {
      failf(معلومات, "Failed to set SNI");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_سيٽ_default_ترجيح(gtls->session);
  إذا(rc != GNUTLS_E_SUCCESS)
    أرجع CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_نسخو("3.6.5");

  إذا(config->نسخو_2 == CURL_SSLVERSION_TLSv1_3) {
    إذا(!tls13support) {
      failf(معلومات, "This GnuTLS installation does not support TLS 1.3");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  نتيجو = gnutls_سيٽ_ssl_نسخو_min_max(معلومات, peer,
                                          config, &prioritylist, tls13support);
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = gtls_سيٽ_ترجيح(cf, معلومات, gtls, prioritylist);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(config->clientcert) {
    إذا(!gtls->shared_creds->trust_تياري) {
      نتيجو = Curl_gtls_client_trust_تياري(cf, معلومات, gtls);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    إذا(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_سيٽ_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->ڪنجي_passwd ? config->ڪنجي_passwd : "");
      إذا(rc != GNUTLS_E_SUCCESS) {
        failf(معلومات,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
    وإلا {
      ثابت بنانشان صحيح supported_ڪنجي_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_سيٽ_x509_ڪنجي_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->ڪنجي ? config->ڪنجي : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->ڪنجي_passwd,
           supported_ڪنجي_encryption_algorithms);
      إذا(rc != GNUTLS_E_SUCCESS) {
        failf(معلومات,
              "error reading X.509 %skey file: %s",
              config->ڪنجي_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_سيٽ(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(معلومات, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(config->verifystatus) {
    rc = gnutls_ocsp_حالت_request_enable_client(gtls->session,
                                                  عدم, 0, عدم);
    إذا(rc != GNUTLS_E_SUCCESS) {
      failf(معلومات, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  أرجع CURLE_OK;
}

#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
ساكن صحيح keylog_callback(gnutls_session_t session, ثابت حرف *label,
                           ثابت gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  إذا(crandom.سائيز != 32) {
    أرجع -1;
  }

  Curl_tls_keylog_write(label, crandom.معلومات, crandom.سائيز,
                        secret->معلومات, secret->سائيز);
  أرجع 0;
}

ساكن CURLcode gtls_on_session_reuse(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      بنية alpn_spec *alpns,
                                      بنية Curl_ssl_session *scs,
                                      bool *do_early_معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;

  connssl->earlydata_max =
    gnutls_رڪارڊ_get_max_early_معلومات_سائيز(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  أرجع Curl_on_session_reuse(cf, معلومات, alpns, scs, do_early_معلومات,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#جيڪڏهن_پورو

ساكن CURLcode gtls_apply_session(
  بنية gtls_ctx *gctx,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *معلومات,
  بنية ssl_peer *peer,
  بنية alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  بنية Curl_ssl_session *scs,
  bool *pwas_تياري)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتيجو = CURLE_OK;
  صحيح rc;

  إذا(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_شامل_آهي_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    نتيجو = gtls_client_init(cf, معلومات, peer, scs->earlydata_max, gctx);
    إذا(نتيجو)
      اذهب ٻاهريون;
    *pwas_تياري = TRUE;

    rc = gnutls_session_سيٽ_معلومات(gctx->session, scs->sdata, scs->sdata_len);
    إذا(rc < 0)
      infof(معلومات, "SSL session not accepted by GnuTLS, continuing without");
    وإلا {
      infof(معلومات, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      إذا(conn_config->earlydata && scs->alpn &&
         !cf->conn->بٽ.connect_only) {
        bool do_early_معلومات = FALSE;
        إذا(sess_reuse_cb) {
          نتيجو = sess_reuse_cb(cf, معلومات, alpns, scs, &do_early_معلومات);
          إذا(نتيجو)
            اذهب ٻاهريون;
        }
        إذا(do_early_معلومات) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_منزل(alpns, scs->alpn);
        }
      }
    }
  }
ٻاهريون:
  أرجع نتيجو;
}

CURLcode Curl_gtls_ctx_init(بنية gtls_ctx *gctx,
                            بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            بنية ssl_peer *peer,
                            ثابت بنية alpn_spec *alpns_requested,
                            Curl_gtls_ctx_تياري_cb *cb_تياري,
                            فراغ *cb_user_معلومات,
                            فراغ *ssl_user_معلومات,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_session *scs = عدم;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_ڳڻپ = 0;
  bool gtls_session_تياري = FALSE;
  بنية alpn_spec alpns;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  إذا((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(معلومات, cf->cft, FIRSTSOCKET);
    إذا(scs) {
      نتيجو = gtls_apply_session(gctx, cf, معلومات, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_تياري);
      scs = عدم;
      إذا(!نتيجو && gtls_session_تياري) {
        CURL_TRC_CF(معلومات, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  إذا(!gtls_session_تياري && conn_config->cache_session &&
     !conn_config->verifystatus) {
    نتيجو = Curl_ssl_scache_take(cf, معلومات, peer->scache_ڪنجي, &scs);
    إذا(نتيجو)
      اذهب ٻاهريون;
    نتيجو = gtls_apply_session(gctx, cf, معلومات, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_تياري);
  }

  إذا(!gtls_session_تياري) {
    نتيجو = gtls_client_init(cf, معلومات, peer, 0, gctx);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  gnutls_session_سيٽ_ptr(gctx->session, ssl_user_معلومات);

  إذا(cb_تياري) {
    نتيجو = cb_تياري(cf, معلومات, cb_user_معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  إذا(Curl_tls_keylog_enabled()) {
    gnutls_session_سيٽ_keylog_function(gctx->session, keylog_callback);
  }
#جيڪڏهن_پورو

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  إذا(!gtls_alpns_ڳڻپ && alpns.ڳڻپ) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.ڳڻپ);
    لكل(i = 0; i < alpns.ڳڻپ; ++i) {
      gtls_alpns[i].معلومات = (بنانشان حرف *)alpns.entries[i];
      gtls_alpns[i].سائيز = (بنانشان صحيح)لڙي_ڊيگهه(alpns.entries[i]);
    }
    gtls_alpns_ڳڻپ = alpns.ڳڻپ;
  }

  إذا(gtls_alpns_ڳڻپ &&
     gnutls_alpn_سيٽ_protocols(gctx->session,
                               gtls_alpns, (بنانشان صحيح)gtls_alpns_ڳڻپ,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(معلومات, "failed setting ALPN");
    نتيجو = CURLE_SSL_CONNECT_ERROR;
  }

ٻاهريون:
  Curl_ssl_scache_return(cf, معلومات, peer->scache_ڪنجي, scs);
  أرجع نتيجو;
}

ساكن CURLcode gtls_connect_step1(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  CURLcode نتيجو;

  DEBUGASSERT(backend);

  إذا(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    أرجع CURLE_OK;

  نتيجو = Curl_gtls_ctx_init(&backend->gtls, cf, معلومات, &connssl->peer,
                              connssl->alpn, عدم, عدم, cf,
#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
                              gtls_on_session_reuse
#وإلا
                              عدم
#جيڪڏهن_پورو
                              );

  إذا(نتيجو)
    أرجع نتيجو;

  إذا(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    بنية alpn_proto_buf proto;
    memset(&proto, 0, حجم(proto));
    Curl_alpn_منزل_proto_str(&proto, connssl->alpn);
    infof(معلومات, VTLS_INFOF_ALPN_OFFER_1STR, proto.معلومات);
  }

#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
  gnutls_handshake_سيٽ_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#جيڪڏهن_پورو

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_سيٽ_ptr(backend->gtls.session, cf);
  gnutls_transport_سيٽ_ڌڪو_function(backend->gtls.session, gtls_ڌڪو);
  gnutls_transport_سيٽ_pull_function(backend->gtls.session, gtls_pull);

  أرجع CURLE_OK;
}

ساكن CURLcode pkp_pin_peer_pubkey(بنية Curl_easy *معلومات,
                                    gnutls_x509_crt_t cert,
                                    ثابت حرف *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  بنانشان حرف *buff1 = عدم;

  gnutls_pubkey_t ڪنجي = عدم;

  /* Result is returned to caller */
  CURLcode نتيجو = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  إذا(!pinnedpubkey)
    أرجع CURLE_OK;

  إذا(!cert)
    أرجع نتيجو;

  افعل {
    صحيح ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&ڪنجي);
    إذا(ret < 0)
      اكسر; /* failed */

    ret = gnutls_pubkey_import_x509(ڪنجي, cert, 0);
    إذا(ret < 0)
      اكسر; /* failed */

    ret = gnutls_pubkey_export(ڪنجي, GNUTLS_X509_FMT_DER, عدم, &len1);
    إذا(ret != GNUTLS_E_SHORT_يادگيري_بفر || len1 == 0)
      اكسر; /* failed */

    buff1 = curlx_malloc(len1);
    إذا(!buff1)
      اكسر; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(ڪنجي, GNUTLS_X509_FMT_DER, buff1, &len2);
    إذا(ret < 0 || len1 != len2)
      اكسر; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    نتيجو = Curl_pin_peer_pubkey(معلومات, pinnedpubkey, buff1, len1);
  } طالما(0);

  إذا(ڪنجي)
    gnutls_pubkey_deinit(ڪنجي);

  curlx_safefree(buff1);

  أرجع نتيجو;
}

فراغ Curl_gtls_report_handshake(بنية Curl_easy *معلومات, بنية gtls_ctx *gctx)
{
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  إذا(Curl_trc_is_verbose(معلومات)) {
    ثابت حرف *ptr;
    gnutls_protocol_t نسخو_2 = gnutls_protocol_get_نسخو(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(معلومات, "SSL connection using %s / %s",
          gnutls_protocol_get_name(نسخو_2), ptr);
  }
#وإلا
  (فراغ)معلومات;
  (فراغ)gctx;
#جيڪڏهن_پورو
}

ساكن فراغ gtls_نياپو_verify_نتيجو(بنية Curl_easy *معلومات,
                                   بنية ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  حرف certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t سائيز = حجم(certname);
  صحيح rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &سائيز);
  إذا(rc) {
    infof(معلومات, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  إذا(!was_verified) {
    إذا(needs_verified) {
      failf(معلومات, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    وإلا
      infof(معلومات, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  وإلا
    infof(معلومات, "  common name: %s (matched)", certname);
}

ساكن فراغ gtls_infof_cert(بنية Curl_easy *معلومات,
                            gnutls_x509_crt_t x509_cert)
{
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  إذا(Curl_trc_is_verbose(معلومات)) {
    gnutls_datum_t certfields;
    صحيح rc, algo;
    time_t tstamp;
    بنانشان صحيح بٽ;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &بٽ);
    infof(معلومات, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(معلومات, "  certificate version: #%d",
          gnutls_x509_crt_get_نسخو(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    إذا(rc)
      infof(معلومات, "Failed to get certificate name");
    وإلا {
      infof(معلومات, "  subject: %s", certfields.معلومات);

      tstamp = gnutls_x509_crt_get_activation_وقت(x509_cert);
      showtime(معلومات, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_وقت(x509_cert);
      showtime(معلومات, "expire date", tstamp);

      gnutls_free(certfields.معلومات);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    إذا(rc)
      infof(معلومات, "Failed to get certificate issuer");
    وإلا {
      infof(معلومات, "  issuer: %s", certfields.معلومات);

      gnutls_free(certfields.معلومات);
    }
  }
#وإلا
  (فراغ)معلومات;
  (فراغ)x509_cert;
#جيڪڏهن_پورو
}

ساكن CURLcode gtls_verify_ocsp_حالت(بنية Curl_easy *معلومات,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = عدم;
  CURLcode نتيجو = CURLE_SSL_INVALIDCERTSTATUS;
  صحيح rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_حالت_t حالت = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_حالت_request_get(session, &ocsp_raw);
  إذا(rc == GNUTLS_E_REQUESTED_معلومات_NOT_موجود) {
    failf(معلومات, "No OCSP response received");
    اذهب ٻاهريون;
  }
  وإلا إذا(rc < 0) {
    failf(معلومات, "Invalid OCSP response received");
    اذهب ٻاهريون;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  إذا(rc < 0) {
    failf(معلومات, "failed to init OCSP response parsing");
    اذهب ٻاهريون;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  إذا(rc < 0) {
    failf(معلومات, "failed to parse OCSP response");
    اذهب ٻاهريون;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  إذا(gnutls_ocsp_resp_get_حالت(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(معلومات, "OCSP response basic part is not successful");
    اذهب ٻاهريون;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  لكل(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    إذا(!rc || (rc == GNUTLS_E_REQUESTED_معلومات_NOT_موجود))
      اكسر;
  }
  إذا(rc < 0) {
    failf(معلومات, "OCSP does not carry status for peer cert");
    اذهب ٻاهريون;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, عدم, عدم, عدم, عدم,
                                   &حالت, عدم, عدم, عدم, عدم);
  إذا(rc < 0) {
    failf(معلومات, "failed to parse OCSP response for peer cert");
    اذهب ٻاهريون;
  }
  بدل(حالت) {
  حالة GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    نتيجو = CURLE_OK;
    اكسر;
  حالة GNUTLS_OCSP_CERT_REVOKED:
    failf(معلومات, "peer certificate is revoked");
    اكسر;
  افتراضي:
    failf(معلومات, "peer certificate OCSP status is not good: %d", (صحيح)حالت);
    اكسر;
  }

ٻاهريون:
  إذا(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  أرجع نتيجو;
}

بنية gtls_cert_chain {
  ثابت gnutls_datum_t *certs;
  بنانشان صحيح انگ_certs;
};

#جيڪڏهن_وصف_ٿيل USE_APPLE_SECTRUST
ساكن CURLcode gtls_chain_get_der(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   فراغ *user_معلومات,
                                   size_t i,
                                   بنانشان حرف **pder,
                                   size_t *pder_len)
{
  بنية gtls_cert_chain *chain = user_معلومات;

  (فراغ)cf;
  (فراغ)معلومات;
  *pder_len = 0;
  *pder = عدم;

  إذا(i >= chain->انگ_certs)
    أرجع CURLE_TOO_LARGE;
  *pder = chain->certs[i].معلومات;
  *pder_len = (size_t)chain->certs[i].سائيز;
  أرجع CURLE_OK;
}
#جيڪڏهن_پورو /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ساكن CURLcode gtls_verify_cert(بنية Curl_easy *معلومات,
                                 بنية ssl_filter_config *config,
                                 بنية ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 بنية Curl_cfilter *cf,
                                 بنية ssl_peer *peer,
                                 بنية gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  بنانشان صحيح verify_حالت = 0;
  ڊگهو * ثابت certverifyresult = &ssl_config->certverifyresult;
  صحيح rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_حالت);
  إذا(rc < 0) {
    failf(معلومات, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    أرجع CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_حالت;
  verified = !(verify_حالت & GNUTLS_CERT_INVALID);
  إذا(verified)
    infof(معلومات, "  SSL certificate verified by GnuTLS");

#جيڪڏهن_وصف_ٿيل USE_APPLE_SECTRUST
  إذا(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode نتيجو;

    إذا(gnutls_ocsp_حالت_request_get(session, &ocsp_raw)) {
      ocsp_raw.معلومات = عدم;
      ocsp_raw.سائيز = 0;
    }
    إذا(config->verifystatus && !ocsp_raw.سائيز) {
      failf(معلومات, "No OCSP response received");
      أرجع CURLE_SSL_INVALIDCERTSTATUS;
    }
    نتيجو = Curl_vtls_apple_verify(cf, معلومات, peer, chain->انگ_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.معلومات, ocsp_raw.سائيز,
                                    config->verifystatus);
    إذا(نتيجو && (نتيجو != CURLE_PEER_FAILED_VERIFICATION))
      أرجع نتيجو; /* no further failure analysis needed */
    verified = !نتيجو;
    إذا(verified) {
      infof(معلومات, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#وإلا
  (فراغ)cf;
  (فراغ)peer;
  (فراغ)chain;
#جيڪڏهن_پورو

  إذا(!verified) {
    CURLcode نتيجو = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ثابت حرف *cause = "certificate error, no details available";
    إذا(verify_حالت & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    وإلا إذا(verify_حالت & GNUTLS_CERT_SIGNER_NOT_مليو)
      cause = "certificate signer not trusted";
    وإلا إذا(verify_حالت & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    وإلا إذا(verify_حالت & GNUTLS_CERT_INVALID_OCSP_حالت) {
      cause = "OCSP status response is invalid";
      نتيجو = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(معلومات, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    أرجع نتيجو;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                gnutls_session_t session,
                                بنية ssl_filter_config *config,
                                بنية ssl_easy_config *ssl_config,
                                بنية ssl_peer *peer,
                                ثابت حرف *pinned_ڪنجي)
{
  بنية gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = عدم, x509_issuer = عدم;
  time_t certclock;
  bool ocsp_checked = FALSE;
  صحيح rc;
  CURLcode نتيجو = CURLE_OK;
  ڊگهو * ثابت certverifyresult = &ssl_config->certverifyresult;

  (فراغ)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.انگ_certs);
  إذا(!chain.certs) {
    إذا(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(معلومات, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_مليو;
      نتيجو = CURLE_PEER_FAILED_VERIFICATION;
      اذهب ٻاهريون;
    }
    infof(معلومات, " common name: WARNING could not obtain");
  }

  إذا(معلومات->سيٽ.ssl.certinfo && chain.certs) {
    إذا(chain.انگ_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(معلومات, "%u certificates is more than allowed (%d)",
            chain.انگ_certs, MAX_ALLOWED_CERT_AMOUNT);
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اذهب ٻاهريون;
    }
    وإلا {
      بنانشان صحيح i;

      نتيجو = Curl_ssl_init_certinfo(معلومات, (صحيح)chain.انگ_certs);
      إذا(نتيجو)
        اذهب ٻاهريون;

      لكل(i = 0; i < chain.انگ_certs; i++) {
        ثابت uint8_t *beg = chain.certs[i].معلومات;
        ثابت uint8_t *پڇاڙي = beg + chain.certs[i].سائيز;

        نتيجو = Curl_ڪڍو_certinfo(معلومات, (صحيح)i, beg, پڇاڙي);
        إذا(نتيجو)
          اذهب ٻاهريون;
      }
    }
  }

  إذا(config->verifypeer) {
    نتيجو = gtls_verify_cert(معلومات, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  وإلا
    infof(معلومات, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  إذا(gnutls_x509_crt_init(&x509_cert)) {
    failf(معلومات, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_مليو;
    نتيجو = CURLE_SSL_CONNECT_ERROR;
    اذهب ٻاهريون;
  }

  إذا(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    إذا(rc) {
      failf(معلومات, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_مليو;
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اذهب ٻاهريون;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_وقت(x509_cert);

  إذا(certclock == (time_t)-1) {
    إذا(config->verifypeer) {
      failf(معلومات, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اذهب ٻاهريون;
    }
    وإلا
      infof(معلومات, "  SSL certificate expiration date verify FAILED");
  }
  وإلا {
    إذا(certclock < وقت(عدم)) {
      إذا(config->verifypeer) {
        failf(معلومات, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        نتيجو = CURLE_PEER_FAILED_VERIFICATION;
        اذهب ٻاهريون;
      }
      وإلا
        infof(معلومات, "  SSL certificate expiration date FAILED");
    }
    وإلا
      infof(معلومات, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_وقت(x509_cert);

  إذا(certclock == (time_t)-1) {
    إذا(config->verifypeer) {
      failf(معلومات, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اذهب ٻاهريون;
    }
    وإلا
      infof(معلومات, "  SSL certificate activation date verify FAILED");
  }
  وإلا {
    إذا(certclock > وقت(عدم)) {
      إذا(config->verifypeer) {
        failf(معلومات, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        نتيجو = CURLE_PEER_FAILED_VERIFICATION;
        اذهب ٻاهريون;
      }
      وإلا
        infof(معلومات, "  SSL certificate activation date FAILED");
    }
    وإلا
      infof(معلومات, "  SSL certificate activation date OK");
  }

  إذا(config->verifystatus && !ocsp_checked) {
    نتيجو = gtls_verify_ocsp_حالت(معلومات, session, x509_cert);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  وإلا
    infof(معلومات, "  SSL certificate status verification SKIPPED");

  إذا(config->issuercert) {
    gnutls_datum_t issuerp;
    إذا(gnutls_x509_crt_init(&x509_issuer)) {
      failf(معلومات, "failed to init GnuTLS x509_crt for issuer");
      نتيجو = CURLE_SSL_ISSUER_ERROR;
      اذهب ٻاهريون;
    }
    issuerp = ڀريو_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    إذا(rc) {
      failf(معلومات, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      نتيجو = CURLE_SSL_ISSUER_ERROR;
      اذهب ٻاهريون;
    }
    rc = (صحيح)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    إذا(rc <= 0) {
      failf(معلومات, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      نتيجو = CURLE_SSL_ISSUER_ERROR;
      اذهب ٻاهريون;
    }
    infof(معلومات, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  نتيجو = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_نياپو_verify_نتيجو(معلومات, peer, x509_cert, rc, config->verifyhost);
  إذا(نتيجو)
    اذهب ٻاهريون;

  إذا(pinned_ڪنجي) {
    نتيجو = pkp_pin_peer_pubkey(معلومات, x509_cert, pinned_ڪنجي);
    إذا(نتيجو != CURLE_OK) {
      failf(معلومات, "SSL: public key does not match pinned public key");
      اذهب ٻاهريون;
    }
  }

  gtls_infof_cert(معلومات, x509_cert);

ٻاهريون:
  إذا(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  إذا(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  أرجع نتيجو;
}

ساكن CURLcode gtls_verifyserver(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  gnutls_session_t session)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, معلومات);
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  ثابت حرف *pinned_ڪنجي = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY);
#وإلا
  ثابت حرف *pinned_ڪنجي = CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY);
#جيڪڏهن_پورو
  CURLcode نتيجو;

  نتيجو = Curl_gtls_verifyserver(cf, معلومات, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_ڪنجي);
  إذا(نتيجو)
    اذهب ٻاهريون;

#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  إذا(gnutls_protocol_get_نسخو(session) < GNUTLS_TLS1_3)
    نتيجو = cf_gtls_update_session_id(cf, معلومات, session);
#جيڪڏهن_پورو

ٻاهريون:
  أرجع نتيجو;
}

#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
ساكن CURLcode gtls_send_earlydata(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  CURLcode نتيجو = CURLE_OK;
  ثابت بنانشان حرف *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_نتيجو = CURLE_OK;
  طالما(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_رڪارڊ_send_early_معلومات(backend->gtls.session, buf, blen);
    CURL_TRC_CF(معلومات, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    إذا(n < 0) {
      إذا(n == GNUTLS_E_AGAIN)
        نتيجو = CURLE_AGAIN;
      وإلا
        نتيجو = backend->gtls.io_نتيجو ?
                 backend->gtls.io_نتيجو : CURLE_SEND_ERROR;
      اذهب ٻاهريون;
    }
    وإلا إذا(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(معلومات, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
ٻاهريون:
  أرجع نتيجو;
}
#جيڪڏهن_پورو

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
                                    بنية Curl_easy *معلومات,
                                    bool *پورو)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  *پورو = FALSE;

  /* Initiate the connection, if not already done */
  إذا(connssl->connecting_state == ssl_connect_1) {
    نتيجو = gtls_connect_step1(cf, معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
    connssl->connecting_state = ssl_connect_2;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
    إذا(connssl->earlydata_state == ssl_earlydata_await) {
      اذهب ٻاهريون;
    }
    وإلا إذا(connssl->earlydata_state == ssl_earlydata_sending) {
      نتيجو = gtls_send_earlydata(cf, معلومات);
      إذا(نتيجو)
        اذهب ٻاهريون;
      connssl->earlydata_state = ssl_earlydata_موڪليل;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_موڪليل));
#جيڪڏهن_پورو
    نتيجو = cf_gtls_handshake(cf, معلومات);
    إذا(نتيجو)
      اذهب ٻاهريون;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  إذا(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    صحيح rc;

    Curl_gtls_report_handshake(معلومات, &backend->gtls);

    نتيجو = gtls_verifyserver(cf, معلومات, backend->gtls.session);
    إذا(نتيجو)
      اذهب ٻاهريون;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    إذا(rc) {  /* No ALPN from server */
      proto.معلومات = عدم;
      proto.سائيز = 0;
    }

    نتيجو = Curl_alpn_سيٽ_negotiated(cf, معلومات, connssl,
                                      proto.معلومات, proto.سائيز);
    إذا(نتيجو)
      اذهب ٻاهريون;

#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
    إذا(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_موڪليل);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_معلومات) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#جيڪڏهن_پورو
    connssl->connecting_state = ssl_connect_پورو;
  }

  إذا(connssl->connecting_state == ssl_connect_پورو)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

ٻاهريون:
  إذا(نتيجو == CURLE_AGAIN) {
    *پورو = FALSE;
    أرجع CURLE_OK;
  }
  *پورو = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(معلومات, cf, "gtls_connect_common() -> %d, done=%d", (صحيح)نتيجو,
              *پورو);
  أرجع نتيجو;
}

ساكن CURLcode gtls_connect(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             bool *پورو)
{
#جيڪڏهن_وصف_ٿيل CURL_GNUTLS_EARLY_معلومات
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  إذا((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *پورو = TRUE;
    أرجع CURLE_OK;
  }
#جيڪڏهن_پورو
  أرجع gtls_connect_common(cf, معلومات, پورو);
}

ساكن bool gtls_معلومات_pending(بنية Curl_cfilter *cf,
                              ثابت بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *ctx = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend;

  (فراغ)معلومات;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية gtls_ssl_backend_معلومات *)ctx->backend;
  إذا(backend->gtls.session &&
     gnutls_رڪارڊ_check_pending(backend->gtls.session) != 0)
    أرجع TRUE;
  أرجع FALSE;
}

ساكن CURLcode gtls_send(بنية Curl_cfilter *cf,
                          بنية Curl_easy *معلومات,
                          ثابت فراغ *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  CURLcode نتيجو = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  طالما(remain) {
    backend->gtls.io_نتيجو = CURLE_OK;
    nwritten = gnutls_رڪارڊ_send(backend->gtls.session, buf, remain);

    إذا(nwritten >= 0) {
      إذا((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(معلومات, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        أرجع CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (حرف *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    وإلا {
      إذا(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        نتيجو = CURLE_OK;
        اذهب ٻاهريون;
      }
      نتيجو = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_نتيجو ? backend->gtls.io_نتيجو : CURLE_SEND_ERROR);
      اذهب ٻاهريون;
    }
  }

ٻاهريون:
  CURL_TRC_CF(معلومات, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (صحيح)نتيجو, *pnwritten);
  أرجع نتيجو;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ساكن CURLcode gtls_shutdown(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              bool send_shutdown, bool *پورو)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  حرف buf[1024];
  CURLcode نتيجو = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  إذا(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *پورو = TRUE;
    اذهب ٻاهريون;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *پورو = FALSE;

  إذا(!backend->gtls.موڪليل_shutdown) {
    /* do this only once */
    backend->gtls.موڪليل_shutdown = TRUE;
    إذا(send_shutdown) {
      صحيح ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      إذا((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(معلومات, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_رڪارڊ_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.موڪليل_shutdown = FALSE;
        نتيجو = CURLE_OK;
        اذهب ٻاهريون;
      }
      إذا(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(معلومات, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((صحيح)ret), (صحيح)ret);
        نتيجو = CURLE_RECV_ERROR;
        اذهب ٻاهريون;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  لكل(i = 0; i < 10; ++i) {
    nread = gnutls_رڪارڊ_recv(backend->gtls.session, buf, حجم(buf));
    إذا(nread <= 0)
      اكسر;
  }
  إذا(nread > 0) {
    /* still data coming in? */
  }
  وإلا إذا(nread == 0) {
    /* We got the close notify alert and are done. */
    *پورو = TRUE;
  }
  وإلا إذا((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_رڪارڊ_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((صحيح)nread), (صحيح)nread);
    نتيجو = CURLE_RECV_ERROR;
  }

ٻاهريون:
  cf->shutdown = (نتيجو || *پورو);
  أرجع نتيجو;
}

ساكن فراغ gtls_close(بنية Curl_cfilter *cf,
                       بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(معلومات, cf, "close");
  إذا(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = عدم;
  }
  إذا(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ساكن CURLcode gtls_recv(بنية Curl_cfilter *cf,
                          بنية Curl_easy *معلومات,
                          حرف *buf, size_t blen,
                          size_t *pnread)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  CURLcode نتيجو = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_رڪارڊ_recv(backend->gtls.session, buf, blen);

  إذا(nread >= 0)
    *pnread = (size_t)nread;
  وإلا {
    إذا((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      نتيجو = CURLE_AGAIN;
      اذهب ٻاهريون;
    }
    وإلا إذا(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      نتيجو = cf_gtls_handshake(cf, معلومات);
      إذا(!نتيجو)
        نتيجو = CURLE_AGAIN; /* make us get called again. */
      اذهب ٻاهريون;
    }
    وإلا {
      failf(معلومات, "GnuTLS recv error (%d): %s",
            (صحيح)nread, gnutls_strerror((صحيح)nread));
      نتيجو = backend->gtls.io_نتيجو ?
        backend->gtls.io_نتيجو : CURLE_RECV_ERROR;
      اذهب ٻاهريون;
    }
  }

ٻاهريون:
  CURL_TRC_CF(معلومات, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (صحيح)نتيجو, nread);
  أرجع نتيجو;
}

size_t Curl_gtls_نسخو(حرف *بفر, size_t سائيز)
{
  أرجع curl_msnprintf(بفر, سائيز, "GnuTLS/%s", gnutls_check_نسخو(عدم));
}

/* data might be NULL! */
ساكن CURLcode gtls_random(بنية Curl_easy *معلومات,
                            بنانشان حرف *entropy, size_t ڊيگهائي)
{
  صحيح rc;
  (فراغ)معلومات;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, ڊيگهائي);
  أرجع rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ساكن CURLcode gtls_sha256sum(ثابت بنانشان حرف *داخلا,
                               size_t len,
                               بنانشان حرف *sha256sum, /* output */
                               size_t sha256len)
{
  بنية sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  افعل {
    بنانشان صحيح ilen = (بنانشان صحيح)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, داخلا);
    len -= ilen;
    داخلا += ilen;
  } طالما(len);
#إذا NETTLE_نسخو_MAJOR >= 4
  (فراغ)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#وإلا
  sha256_digest(&SHA256pw, (بنانشان صحيح)sha256len, sha256sum);
#جيڪڏهن_پورو
  أرجع CURLE_OK;
}

ساكن bool gtls_cert_حالت_request(فراغ)
{
  أرجع TRUE;
}

ساكن فراغ *gtls_get_internals(بنية ssl_connect_معلومات *connssl,
                                CURLINFO info)
{
  بنية gtls_ssl_backend_معلومات *backend =
    (بنية gtls_ssl_backend_معلومات *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع backend->gtls.session;
}

ثابت بنية Curl_ssl Curl_ssl_gnutls = {
  { CURLSSLBACKEND_GNUTLS, "gnutls" }, /* info */

  SSLSUPP_CA_رستو  |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_CA_CACHE |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_CRLFILE,

  حجم(بنية gtls_ssl_backend_معلومات),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_نسخو,             /*نسخو*/
  gtls_shutdown,                 /* shutdown */
  gtls_معلومات_pending,             /* data_pending */
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

#جيڪڏهن_پورو /* USE_GNUTLS */
