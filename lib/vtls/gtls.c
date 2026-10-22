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

#اگر_تعریف_شده USE_GNUTLS

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
#تعریف_کن GTLSDEBUG
#پایان_اگر

#اگر_تعریف_شده GTLSDEBUG
ساكن فراغ tls_log_func(صحيح level, ثابت حرف *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#پایان_اگر

#إذا !defined(GNUTLS_نسخه_عدد) || (GNUTLS_نسخه_عدد < 0x030605)
#خطا "too old GnuTLS version"
#پایان_اگر

#لغو_تعریف CURL_GNUTLS_EARLY_داده
#إذا GNUTLS_نسخه_عدد >= 0x030702
#تعریف_کن CURL_GNUTLS_EARLY_داده
#پایان_اگر

#ضمن <gnutls/ocsp.h>

بنية gtls_ssl_backend_داده {
  بنية gtls_ctx gtls;
};

ساكن ssize_t gtls_افزودن(فراغ *s, ثابت فراغ *buf, size_t blen)
{
  بنية Curl_cfilter *cf = s;
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجه;

  DEBUGASSERT(داده);
  نتیجه = Curl_conn_cf_send(cf->بعدی, داده, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(داده, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (صحيح)نتیجه, nwritten);
  backend->gtls.io_نتیجه = نتیجه;
  إذا(نتیجه) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_مجموعه_errno(backend->gtls.session,
                               (نتیجه == CURLE_AGAIN) ? EAGAIN : EINVAL);
    أرجع -1;
  }
  أرجع (ssize_t)nwritten;
}

ساكن ssize_t gtls_pull(فراغ *s, فراغ *buf, size_t blen)
{
  بنية Curl_cfilter *cf = s;
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  size_t nread;
  CURLcode نتیجه;

  DEBUGASSERT(داده);
  إذا(!backend->gtls.shared_creds->trust_راهاندازی) {
    نتیجه = Curl_gtls_client_trust_راهاندازی(cf, داده, &backend->gtls);
    إذا(نتیجه) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_مجموعه_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_نتیجه = نتیجه;
      أرجع -1;
    }
  }

  نتیجه = Curl_conn_cf_recv(cf->بعدی, داده, buf, blen, &nread);
  CURL_TRC_CF(داده, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (صحيح)نتیجه,
              nread);
  backend->gtls.io_نتیجه = نتیجه;
  إذا(نتیجه) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_مجموعه_errno(backend->gtls.session,
                               (نتیجه == CURLE_AGAIN) ? EAGAIN : EINVAL);
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
#اگر_تعریف_شده GTLSDEBUG
  gnutls_global_مجموعه_log_function(tls_log_func);
  gnutls_global_مجموعه_log_level(2);
#پایان_اگر
  أرجع ret;
}

ساكن فراغ gtls_cleanup(فراغ)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#اگر_تعریف_شده CURLVERBOSE
ساكن فراغ showtime(بنية Curl_easy *داده, ثابت حرف *متن_3, time_t stamp)
{
  بنية tm میانگیر;
  ثابت بنية tm *tm = &میانگیر;
  حرف str[96];
  CURLcode نتیجه = curlx_gmtime(stamp, &میانگیر);
  إذا(نتیجه)
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
  infof(داده, "%s", str);
}
#پایان_اگر

ساكن gnutls_datum_t بارگیری_file(ثابت حرف *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { عدم, 0 };
  دراز filelen;
  فراغ *ptr;

  f = curlx_fopen(file, "rb");
  إذا(!f)
    أرجع loaded_file;
  إذا(fseek(f, 0, SEEK_END))
    اذهب بیرون;
  filelen = ftell(f);
  إذا(filelen < 0 || filelen > CURL_MAX_درون_داد_درازی)
    اذهب بیرون;
  إذا(fseek(f, 0, SEEK_SET))
    اذهب بیرون;
  ptr = curlx_malloc((size_t)filelen);
  إذا(!ptr)
    اذهب بیرون;
  إذا(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    اذهب بیرون;
  }

  loaded_file.داده = ptr;
  loaded_file.اندازه = (بیعلامت صحيح)filelen;
بیرون:
  curlx_fclose(f);
  أرجع loaded_file;
}

ساكن فراغ unload_file(gnutls_datum_t داده)
{
  curlx_free(داده.داده);
}

/* this function does an SSL/TLS (re-)handshake */
ساكن CURLcode cf_gtls_handshake(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  gnutls_session_t session;
  صحيح rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_نتیجه = CURLE_OK;
  rc = gnutls_handshake(session);

  إذا(!backend->gtls.shared_creds->trust_راهاندازی) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode نتیجه = Curl_gtls_client_trust_راهاندازی(cf, داده, &backend->gtls);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_رکورد_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    أرجع CURLE_AGAIN;
  }
  وإلا إذا((rc < 0) && !gnutls_error_is_fatal(rc)) {
#اگر_تعریف_شده CURLVERBOSE
    ثابت حرف *strerr = عدم;

    إذا(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    إذا(!strerr)
      strerr = gnutls_strerror(rc);

    infof(داده, "gnutls_handshake() warning: %s", strerr);
#پایان_اگر
    أرجع CURLE_AGAIN;
  }
  وإلا إذا((rc < 0) && backend->gtls.io_نتیجه) {
    أرجع backend->gtls.io_نتیجه;
  }
  وإلا إذا(rc < 0) {
    ثابت حرف *strerr = عدم;

    إذا(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    إذا(!strerr)
      strerr = gnutls_strerror(rc);

    failf(داده, "GnuTLS, handshake failed: %s", strerr);
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

#تعریف_کن GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#تعریف_کن QUIC_اولویت \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ساكن CURLcode gnutls_مجموعه_ssl_نسخه_min_max(
  بنية Curl_easy *داده,
  بنية ssl_peer *peer,
  بنية ssl_filter_config *conn_config,
  ثابت حرف **prioritylist,
  bool tls13support)
{
  دراز ssl_نسخه = conn_config->نسخه_2;
  دراز ssl_نسخه_max = conn_config->نسخه_max;

  DEBUGASSERT(ssl_نسخه != CURL_SSLVERSION_DEFAULT);
  إذا(ssl_نسخه <= CURL_SSLVERSION_TLSv1)
    ssl_نسخه = CURL_SSLVERSION_TLSv1_0;
  إذا((ssl_نسخه_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_نسخه_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_نسخه_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  إذا(peer->transport == TRNSPRT_QUIC) {
    إذا(ssl_نسخه_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(داده, "QUIC needs at least TLS version 1.3");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_اولویت;
    أرجع CURLE_OK;
  }

  بدل(ssl_نسخه | ssl_نسخه_max) {
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

  failf(داده, "GnuTLS: cannot set TLS protocol");
  أرجع CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(بنية Curl_easy *داده,
                                       بنية gtls_shared_creds **pcreds)
{
  بنية gtls_shared_creds *shared;
  صحيح rc;

  *pcreds = عدم;
  shared = curlx_calloc(1, حجم(*shared));
  إذا(!shared)
    أرجع CURLE_بیرون_از_حافظه;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(داده, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->زمان = *Curl_pgrs_now(داده);
  *pcreds = shared;
  أرجع CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_به_بالا_ref(بنية gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  إذا(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    أرجع CURLE_OK;
  }
  أرجع CURLE_BAD_FUNCTION_آرگومان;
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
                                    بنية Curl_easy *داده,
                                    gnutls_certificate_credentials_t creds)
{
  بنية ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, داده);
  bool creds_are_خالی = TRUE;
  صحيح rc;

  إذا(!config->verifypeer) {
    infof(داده, "SSL Trust: peer verification disabled");
    أرجع CURLE_OK;
  }

  infof(داده, "SSL Trust Anchors:");
  إذا(config->native_ca_store) {
#اگر_تعریف_شده USE_APPLE_SECTRUST
    infof(داده, "  Native: Apple SecTrust");
    creds_are_خالی = FALSE;
#وإلا
    rc = gnutls_certificate_مجموعه_x509_system_trust(creds);
    إذا(rc < 0)
      infof(داده, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    وإلا {
      infof(داده, "  Native: %d certificates from system trust", rc);
      إذا(rc > 0)
        creds_are_خالی = FALSE;
    }
#پایان_اگر
  }

  إذا(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    إذا(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(داده, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      أرجع CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.داده = config->ca_info_blob->داده;
    ca_info_datum.اندازه = (بیعلامت صحيح)config->ca_info_blob->len;
    rc = gnutls_certificate_مجموعه_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_خالی = creds_are_خالی && (rc <= 0);
    إذا(rc < 0) {
      infof(داده, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_خالی ? "" : ", continuing anyway"));
      إذا(creds_are_خالی) {
        ssl_config->certverifyresult = rc;
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
    }
    وإلا
      infof(داده, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  وإلا إذا(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_مجموعه_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_مجموعه_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_خالی = creds_are_خالی && (rc <= 0);
    إذا(rc < 0) {
      infof(داده, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_خالی ? "" : ", continuing anyway"));
      إذا(creds_are_خالی) {
        ssl_config->certverifyresult = rc;
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
    }
    وإلا
      infof(داده, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  إذا(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_مجموعه_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_خالی = creds_are_خالی && (rc <= 0);
    إذا(rc < 0) {
      infof(داده, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_خالی ? "" : ", continuing anyway"));
      إذا(creds_are_خالی) {
        ssl_config->certverifyresult = rc;
        أرجع CURLE_SSL_CACERT_BADFILE;
      }
    }
    وإلا
      infof(داده, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  إذا(creds_are_خالی)
    infof(داده, "  no trust anchors configured");

#اگر_تعریف_شده USE_APPLE_SECTRUST
  إذا(config->CRLfile && config->native_ca_store) {
    failf(داده, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    أرجع CURLE_NOT_BUILT_درون;
  }
#پایان_اگر
  إذا(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_مجموعه_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    إذا(rc < 0) {
      failf(داده, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      أرجع CURLE_SSL_CRL_BADFILE;
    }
    وإلا
      infof(داده, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  أرجع CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#تعریف_کن MPROTO_GTLS_X509_کلید   "tls:gtls:x509:share"

ساكن bool gtls_shared_creds_expired(بنية Curl_easy *داده,
                                      ثابت بنية gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(داده), &sc->زمان);
  timediff_t timeout_ms = داده->مجموعه.ssl_ca_cache_timeout * (timediff_t)1000;

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
                                                       بنية Curl_easy *داده)
{
  بنية gtls_shared_creds *shared_creds;

  إذا(داده->multi) {
    shared_creds = Curl_hash_pick(&داده->multi->proto_hash,
                                  MPROTO_GTLS_X509_کلید,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_کلید));
    إذا(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(داده, shared_creds) &&
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

ساكن فراغ gtls_مجموعه_cached_creds(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  بنية gtls_shared_creds *sc)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  إذا(!داده->multi)
    أرجع;

  إذا(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    إذا(!sc->CAfile)
      أرجع;
  }

  إذا(Curl_gtls_shared_creds_به_بالا_ref(sc))
    أرجع;

  إذا(!Curl_hash_add2(&داده->multi->proto_hash,
                     MPROTO_GTLS_X509_کلید,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_کلید),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    أرجع;
  }
}

CURLcode Curl_gtls_client_trust_راهاندازی(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      بنية gtls_ctx *gtls)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية gtls_shared_creds *cached_creds = عدم;
  bool cache_criteria_met;
  CURLcode نتیجه;
  صحيح rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (داده->مجموعه.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  إذا(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, داده);

  إذا(cached_creds && !Curl_gtls_shared_creds_به_بالا_ref(cached_creds)) {
    CURL_TRC_CF(داده, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_مجموعه(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    إذا(rc != GNUTLS_E_SUCCESS) {
      failf(داده, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }
  وإلا {
    CURL_TRC_CF(داده, cf, "loading trust anchors and CRLs");
    نتیجه = gtls_populate_creds(cf, داده, gtls->shared_creds->creds);
    إذا(نتیجه)
      أرجع نتیجه;
    gtls->shared_creds->trust_راهاندازی = TRUE;
    إذا(cache_criteria_met)
      gtls_مجموعه_cached_creds(cf, داده, gtls->shared_creds);
  }
  أرجع CURLE_OK;
}

#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
ساكن صحيح gtls_get_ietf_proto(gnutls_session_t session)
{
  بدل(gnutls_protocol_get_نسخه(session)) {
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
                                 بنية Curl_easy *داده,
                                 ثابت حرف *ssl_peer_کلید,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ثابت حرف *alpn,
                                 بیعلامت حرف *quic_tp,
                                 size_t quic_tp_len,
                                 بنية Curl_ssl_session **psession)
{
  بنية Curl_ssl_session *sc_session = عدم, *sc_dup = عدم;
  بیعلامت حرف *sdata, *qtp_clone = عدم;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode نتیجه = CURLE_OK;

  إذا(psession)
    *psession = عدم;
  إذا(!Curl_ssl_scache_use(cf, داده))
    أرجع CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_داده(session, عدم, &sdata_len);
  إذا(!sdata_len) /* GnuTLS does this for some version combinations */
    أرجع CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  إذا(!sdata)
    أرجع CURLE_بیرون_از_حافظه;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_داده(session, sdata, &sdata_len);
  earlydata_max = gnutls_رکورد_get_max_early_داده_اندازه(session);

  CURL_TRC_CF(داده, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  إذا(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ثابت حرف *)quic_tp, quic_tp_len);
    إذا(!qtp_clone) {
      curlx_free(sdata);
      أرجع CURLE_بیرون_از_حافظه;
    }
  }

  نتیجه = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  إذا(!نتیجه && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    نتیجه = Curl_ssl_session_dup(sc_session, &sc_dup);
  إذا(!نتیجه) {
    نتیجه = Curl_ssl_scache_put(cf, داده, ssl_peer_کلید, sc_session);
    /* took ownership of `sc_session` */
    sc_session = عدم;
  }
  إذا(!نتیجه && psession) {
    *psession = sc_dup;
    sc_dup = عدم;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  أرجع نتیجه;
}
#پایان_اگر

#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
ساكن CURLcode cf_gtls_update_session_id(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *داده,
                                          gnutls_session_t session)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية Curl_ssl_session *scs = عدم;
  CURLcode نتیجه;

  نتیجه = Curl_gtls_cache_session(cf, داده, connssl->peer.scache_کلید,
                                   session, 0, connssl->negotiated.alpn,
                                   عدم, 0, &scs);
  إذا(!نتیجه) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  أرجع نتیجه;
}

ساكن صحيح gtls_handshake_cb(gnutls_session_t session, بیعلامت صحيح htype,
                             بیعلامت when, بیعلامت صحيح incoming,
                             ثابت gnutls_datum_t *پیام)
{
  بنية Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (فراغ)پیام;
  (فراغ)incoming;
  إذا(when) { /* after message has been processed */
    بنية Curl_easy *داده = CF_داده_CURRENT(cf);
    إذا(داده) {
      CURL_TRC_CF(داده, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      بدل(htype) {
      حالة GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, داده, session);
        اكسر;
      }
      افتراضي:
        اكسر;
      }
    }
  }
  أرجع 0;
}
#پایان_اگر

ساكن CURLcode gtls_مجموعه_اولویت(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  بنية gtls_ctx *gtls,
                                  ثابت حرف *اولویت)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية dynbuf buf;
  ثابت حرف *err = عدم;
  CURLcode نتیجه = CURLE_OK;
  صحيح rc;

  curlx_dyn_init(&buf, 4096);

  إذا(conn_config->cipher_list) {
    إذا((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      إذا(!curlx_dyn_len(&buf)) {  /* not added yet */
        نتیجه = curlx_dyn_جمع_کن(&buf, اولویت);
        إذا(نتیجه)
          اذهب بیرون;
      }
      نتیجه = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      إذا(نتیجه)
        اذهب بیرون;
      اولویت = curlx_dyn_ptr(&buf);
    }
    وإلا /* replace our own completely */
      اولویت = conn_config->cipher_list;
  }

  infof(داده, "GnuTLS priority: %s", اولویت);
  rc = gnutls_اولویت_مجموعه_مستقیم(gtls->session, اولویت, &err);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(داده, "Error %d setting GnuTLS priority: %s", rc, err);
    نتیجه = CURLE_SSL_CONNECT_ERROR;
  }

بیرون:
  curlx_dyn_free(&buf);
  أرجع نتیجه;
}

ساكن CURLcode gtls_client_init(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 بنية ssl_peer *peer,
                                 size_t earlydata_max,
                                 بنية gtls_ctx *gtls)
{
  بنية ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, داده);
  بیعلامت صحيح init_flags;
  صحيح rc;
  ثابت حرف *prioritylist;
  bool tls13support;
  CURLcode نتیجه;

  إذا(config->نسخه_2 == CURL_SSLVERSION_SSLv2 ||
     config->نسخه_2 == CURL_SSLVERSION_SSLv3) {
    failf(داده, "GnuTLS does not support SSLv2 or SSLv3");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  نتیجه = Curl_gtls_shared_creds_create(داده, &gtls->shared_creds);
  إذا(نتیجه)
    أرجع نتیجه;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
  إذا(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_داده | GNUTLS_NO_انتها_از_EARLY_داده;
  وإلا إذا(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_داده;
#پایان_اگر

#اگر_تعریف_شده GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#پایان_اگر

#اگر_تعریف_شده GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#پایان_اگر

#اگر_تعریف_شده GNUTLS_NO_وضعیت_REQUEST
  إذا(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_وضعیت_REQUEST;
#پایان_اگر

  CURL_TRC_CF(داده, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(داده, "gnutls_init() failed: %d", rc);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(peer->sni) {
    إذا(gnutls_server_name_مجموعه(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, طول_رشته(peer->sni)) < 0) {
      failf(داده, "Failed to set SNI");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_مجموعه_default_اولویت(gtls->session);
  إذا(rc != GNUTLS_E_SUCCESS)
    أرجع CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_نسخه("3.6.5");

  إذا(config->نسخه_2 == CURL_SSLVERSION_TLSv1_3) {
    إذا(!tls13support) {
      failf(داده, "This GnuTLS installation does not support TLS 1.3");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  نتیجه = gnutls_مجموعه_ssl_نسخه_min_max(داده, peer,
                                          config, &prioritylist, tls13support);
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = gtls_مجموعه_اولویت(cf, داده, gtls, prioritylist);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(config->clientcert) {
    إذا(!gtls->shared_creds->trust_راهاندازی) {
      نتیجه = Curl_gtls_client_trust_راهاندازی(cf, داده, gtls);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    إذا(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_مجموعه_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->کلید_passwd ? config->کلید_passwd : "");
      إذا(rc != GNUTLS_E_SUCCESS) {
        failf(داده,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
    وإلا {
      ثابت بیعلامت صحيح supported_کلید_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_مجموعه_x509_کلید_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->کلید ? config->کلید : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->کلید_passwd,
           supported_کلید_encryption_algorithms);
      إذا(rc != GNUTLS_E_SUCCESS) {
        failf(داده,
              "error reading X.509 %skey file: %s",
              config->کلید_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_مجموعه(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  إذا(rc != GNUTLS_E_SUCCESS) {
    failf(داده, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  إذا(config->verifystatus) {
    rc = gnutls_ocsp_وضعیت_request_enable_client(gtls->session,
                                                  عدم, 0, عدم);
    إذا(rc != GNUTLS_E_SUCCESS) {
      failf(داده, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
  }

  أرجع CURLE_OK;
}

#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
ساكن صحيح keylog_callback(gnutls_session_t session, ثابت حرف *label,
                           ثابت gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  إذا(crandom.اندازه != 32) {
    أرجع -1;
  }

  Curl_tls_keylog_write(label, crandom.داده, crandom.اندازه,
                        secret->داده, secret->اندازه);
  أرجع 0;
}

ساكن CURLcode gtls_on_session_reuse(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      بنية alpn_spec *alpns,
                                      بنية Curl_ssl_session *scs,
                                      bool *do_early_داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;

  connssl->earlydata_max =
    gnutls_رکورد_get_max_early_داده_اندازه(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  أرجع Curl_on_session_reuse(cf, داده, alpns, scs, do_early_داده,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#پایان_اگر

ساكن CURLcode gtls_apply_session(
  بنية gtls_ctx *gctx,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *داده,
  بنية ssl_peer *peer,
  بنية alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  بنية Curl_ssl_session *scs,
  bool *pwas_راهاندازی)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجه = CURLE_OK;
  صحيح rc;

  إذا(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_شامل_است_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    نتیجه = gtls_client_init(cf, داده, peer, scs->earlydata_max, gctx);
    إذا(نتیجه)
      اذهب بیرون;
    *pwas_راهاندازی = TRUE;

    rc = gnutls_session_مجموعه_داده(gctx->session, scs->sdata, scs->sdata_len);
    إذا(rc < 0)
      infof(داده, "SSL session not accepted by GnuTLS, continuing without");
    وإلا {
      infof(داده, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      إذا(conn_config->earlydata && scs->alpn &&
         !cf->conn->بیتها.connect_only) {
        bool do_early_داده = FALSE;
        إذا(sess_reuse_cb) {
          نتیجه = sess_reuse_cb(cf, داده, alpns, scs, &do_early_داده);
          إذا(نتیجه)
            اذهب بیرون;
        }
        إذا(do_early_داده) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_مقصد(alpns, scs->alpn);
        }
      }
    }
  }
بیرون:
  أرجع نتیجه;
}

CURLcode Curl_gtls_ctx_init(بنية gtls_ctx *gctx,
                            بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده,
                            بنية ssl_peer *peer,
                            ثابت بنية alpn_spec *alpns_requested,
                            Curl_gtls_ctx_راهاندازی_cb *cb_راهاندازی,
                            فراغ *cb_user_داده,
                            فراغ *ssl_user_داده,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_session *scs = عدم;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_شمار = 0;
  bool gtls_session_راهاندازی = FALSE;
  بنية alpn_spec alpns;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  إذا((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(داده, cf->cft, FIRSTSOCKET);
    إذا(scs) {
      نتیجه = gtls_apply_session(gctx, cf, داده, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_راهاندازی);
      scs = عدم;
      إذا(!نتیجه && gtls_session_راهاندازی) {
        CURL_TRC_CF(داده, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  إذا(!gtls_session_راهاندازی && conn_config->cache_session &&
     !conn_config->verifystatus) {
    نتیجه = Curl_ssl_scache_take(cf, داده, peer->scache_کلید, &scs);
    إذا(نتیجه)
      اذهب بیرون;
    نتیجه = gtls_apply_session(gctx, cf, داده, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_راهاندازی);
  }

  إذا(!gtls_session_راهاندازی) {
    نتیجه = gtls_client_init(cf, داده, peer, 0, gctx);
    إذا(نتیجه)
      اذهب بیرون;
  }

  gnutls_session_مجموعه_ptr(gctx->session, ssl_user_داده);

  إذا(cb_راهاندازی) {
    نتیجه = cb_راهاندازی(cf, داده, cb_user_داده);
    إذا(نتیجه)
      اذهب بیرون;
  }

#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  إذا(Curl_tls_keylog_enabled()) {
    gnutls_session_مجموعه_keylog_function(gctx->session, keylog_callback);
  }
#پایان_اگر

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  إذا(!gtls_alpns_شمار && alpns.شمار) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.شمار);
    لكل(i = 0; i < alpns.شمار; ++i) {
      gtls_alpns[i].داده = (بیعلامت حرف *)alpns.entries[i];
      gtls_alpns[i].اندازه = (بیعلامت صحيح)طول_رشته(alpns.entries[i]);
    }
    gtls_alpns_شمار = alpns.شمار;
  }

  إذا(gtls_alpns_شمار &&
     gnutls_alpn_مجموعه_protocols(gctx->session,
                               gtls_alpns, (بیعلامت صحيح)gtls_alpns_شمار,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(داده, "failed setting ALPN");
    نتیجه = CURLE_SSL_CONNECT_ERROR;
  }

بیرون:
  Curl_ssl_scache_return(cf, داده, peer->scache_کلید, scs);
  أرجع نتیجه;
}

ساكن CURLcode gtls_connect_step1(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  CURLcode نتیجه;

  DEBUGASSERT(backend);

  إذا(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    أرجع CURLE_OK;

  نتیجه = Curl_gtls_ctx_init(&backend->gtls, cf, داده, &connssl->peer,
                              connssl->alpn, عدم, عدم, cf,
#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
                              gtls_on_session_reuse
#وإلا
                              عدم
#پایان_اگر
                              );

  إذا(نتیجه)
    أرجع نتیجه;

  إذا(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    بنية alpn_proto_buf proto;
    memset(&proto, 0, حجم(proto));
    Curl_alpn_مقصد_proto_str(&proto, connssl->alpn);
    infof(داده, VTLS_INFOF_ALPN_OFFER_1STR, proto.داده);
  }

#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
  gnutls_handshake_مجموعه_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#پایان_اگر

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_مجموعه_ptr(backend->gtls.session, cf);
  gnutls_transport_مجموعه_افزودن_function(backend->gtls.session, gtls_افزودن);
  gnutls_transport_مجموعه_pull_function(backend->gtls.session, gtls_pull);

  أرجع CURLE_OK;
}

ساكن CURLcode pkp_pin_peer_pubkey(بنية Curl_easy *داده,
                                    gnutls_x509_crt_t cert,
                                    ثابت حرف *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  بیعلامت حرف *buff1 = عدم;

  gnutls_pubkey_t کلید = عدم;

  /* Result is returned to caller */
  CURLcode نتیجه = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  إذا(!pinnedpubkey)
    أرجع CURLE_OK;

  إذا(!cert)
    أرجع نتیجه;

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
    إذا(ret != GNUTLS_E_SHORT_حافظه_میانگیر || len1 == 0)
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
    نتیجه = Curl_pin_peer_pubkey(داده, pinnedpubkey, buff1, len1);
  } طالما(0);

  إذا(کلید)
    gnutls_pubkey_deinit(کلید);

  curlx_safefree(buff1);

  أرجع نتیجه;
}

فراغ Curl_gtls_report_handshake(بنية Curl_easy *داده, بنية gtls_ctx *gctx)
{
#اگر_تعریف_شده CURLVERBOSE
  إذا(Curl_trc_is_verbose(داده)) {
    ثابت حرف *ptr;
    gnutls_protocol_t نسخه_2 = gnutls_protocol_get_نسخه(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(داده, "SSL connection using %s / %s",
          gnutls_protocol_get_name(نسخه_2), ptr);
  }
#وإلا
  (فراغ)داده;
  (فراغ)gctx;
#پایان_اگر
}

ساكن فراغ gtls_پیام_verify_نتیجه(بنية Curl_easy *داده,
                                   بنية ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  حرف certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t اندازه = حجم(certname);
  صحيح rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &اندازه);
  إذا(rc) {
    infof(داده, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  إذا(!was_verified) {
    إذا(needs_verified) {
      failf(داده, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    وإلا
      infof(داده, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  وإلا
    infof(داده, "  common name: %s (matched)", certname);
}

ساكن فراغ gtls_infof_cert(بنية Curl_easy *داده,
                            gnutls_x509_crt_t x509_cert)
{
#اگر_تعریف_شده CURLVERBOSE
  إذا(Curl_trc_is_verbose(داده)) {
    gnutls_datum_t certfields;
    صحيح rc, algo;
    time_t tstamp;
    بیعلامت صحيح بیتها;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &بیتها);
    infof(داده, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(داده, "  certificate version: #%d",
          gnutls_x509_crt_get_نسخه(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    إذا(rc)
      infof(داده, "Failed to get certificate name");
    وإلا {
      infof(داده, "  subject: %s", certfields.داده);

      tstamp = gnutls_x509_crt_get_activation_زمان(x509_cert);
      showtime(داده, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_زمان(x509_cert);
      showtime(داده, "expire date", tstamp);

      gnutls_free(certfields.داده);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    إذا(rc)
      infof(داده, "Failed to get certificate issuer");
    وإلا {
      infof(داده, "  issuer: %s", certfields.داده);

      gnutls_free(certfields.داده);
    }
  }
#وإلا
  (فراغ)داده;
  (فراغ)x509_cert;
#پایان_اگر
}

ساكن CURLcode gtls_verify_ocsp_وضعیت(بنية Curl_easy *داده,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = عدم;
  CURLcode نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
  صحيح rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_وضعیت_t وضعیت = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_وضعیت_request_get(session, &ocsp_raw);
  إذا(rc == GNUTLS_E_REQUESTED_داده_NOT_موجود) {
    failf(داده, "No OCSP response received");
    اذهب بیرون;
  }
  وإلا إذا(rc < 0) {
    failf(داده, "Invalid OCSP response received");
    اذهب بیرون;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  إذا(rc < 0) {
    failf(داده, "failed to init OCSP response parsing");
    اذهب بیرون;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  إذا(rc < 0) {
    failf(داده, "failed to parse OCSP response");
    اذهب بیرون;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  إذا(gnutls_ocsp_resp_get_وضعیت(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(داده, "OCSP response basic part is not successful");
    اذهب بیرون;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  لكل(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    إذا(!rc || (rc == GNUTLS_E_REQUESTED_داده_NOT_موجود))
      اكسر;
  }
  إذا(rc < 0) {
    failf(داده, "OCSP does not carry status for peer cert");
    اذهب بیرون;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, عدم, عدم, عدم, عدم,
                                   &وضعیت, عدم, عدم, عدم, عدم);
  إذا(rc < 0) {
    failf(داده, "failed to parse OCSP response for peer cert");
    اذهب بیرون;
  }
  بدل(وضعیت) {
  حالة GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    نتیجه = CURLE_OK;
    اكسر;
  حالة GNUTLS_OCSP_CERT_REVOKED:
    failf(داده, "peer certificate is revoked");
    اكسر;
  افتراضي:
    failf(داده, "peer certificate OCSP status is not good: %d", (صحيح)وضعیت);
    اكسر;
  }

بیرون:
  إذا(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  أرجع نتیجه;
}

بنية gtls_cert_chain {
  ثابت gnutls_datum_t *certs;
  بیعلامت صحيح رقم_certs;
};

#اگر_تعریف_شده USE_APPLE_SECTRUST
ساكن CURLcode gtls_chain_get_der(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده,
                                   فراغ *user_داده,
                                   size_t i,
                                   بیعلامت حرف **pder,
                                   size_t *pder_len)
{
  بنية gtls_cert_chain *chain = user_داده;

  (فراغ)cf;
  (فراغ)داده;
  *pder_len = 0;
  *pder = عدم;

  إذا(i >= chain->رقم_certs)
    أرجع CURLE_TOO_LARGE;
  *pder = chain->certs[i].داده;
  *pder_len = (size_t)chain->certs[i].اندازه;
  أرجع CURLE_OK;
}
#پایان_اگر /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ساكن CURLcode gtls_verify_cert(بنية Curl_easy *داده,
                                 بنية ssl_filter_config *config,
                                 بنية ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 بنية Curl_cfilter *cf,
                                 بنية ssl_peer *peer,
                                 بنية gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  بیعلامت صحيح verify_وضعیت = 0;
  دراز * ثابت certverifyresult = &ssl_config->certverifyresult;
  صحيح rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_وضعیت);
  إذا(rc < 0) {
    failf(داده, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    أرجع CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_وضعیت;
  verified = !(verify_وضعیت & GNUTLS_CERT_INVALID);
  إذا(verified)
    infof(داده, "  SSL certificate verified by GnuTLS");

#اگر_تعریف_شده USE_APPLE_SECTRUST
  إذا(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode نتیجه;

    إذا(gnutls_ocsp_وضعیت_request_get(session, &ocsp_raw)) {
      ocsp_raw.داده = عدم;
      ocsp_raw.اندازه = 0;
    }
    إذا(config->verifystatus && !ocsp_raw.اندازه) {
      failf(داده, "No OCSP response received");
      أرجع CURLE_SSL_INVALIDCERTSTATUS;
    }
    نتیجه = Curl_vtls_apple_verify(cf, داده, peer, chain->رقم_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.داده, ocsp_raw.اندازه,
                                    config->verifystatus);
    إذا(نتیجه && (نتیجه != CURLE_PEER_FAILED_VERIFICATION))
      أرجع نتیجه; /* no further failure analysis needed */
    verified = !نتیجه;
    إذا(verified) {
      infof(داده, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#وإلا
  (فراغ)cf;
  (فراغ)peer;
  (فراغ)chain;
#پایان_اگر

  إذا(!verified) {
    CURLcode نتیجه = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ثابت حرف *cause = "certificate error, no details available";
    إذا(verify_وضعیت & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    وإلا إذا(verify_وضعیت & GNUTLS_CERT_SIGNER_NOT_یافت)
      cause = "certificate signer not trusted";
    وإلا إذا(verify_وضعیت & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    وإلا إذا(verify_وضعیت & GNUTLS_CERT_INVALID_OCSP_وضعیت) {
      cause = "OCSP status response is invalid";
      نتیجه = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(داده, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    أرجع نتیجه;
  }
  أرجع CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
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
  CURLcode نتیجه = CURLE_OK;
  دراز * ثابت certverifyresult = &ssl_config->certverifyresult;

  (فراغ)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.رقم_certs);
  إذا(!chain.certs) {
    إذا(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(داده, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_یافت;
      نتیجه = CURLE_PEER_FAILED_VERIFICATION;
      اذهب بیرون;
    }
    infof(داده, " common name: WARNING could not obtain");
  }

  إذا(داده->مجموعه.ssl.certinfo && chain.certs) {
    إذا(chain.رقم_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(داده, "%u certificates is more than allowed (%d)",
            chain.رقم_certs, MAX_ALLOWED_CERT_AMOUNT);
      نتیجه = CURLE_SSL_CONNECT_ERROR;
      اذهب بیرون;
    }
    وإلا {
      بیعلامت صحيح i;

      نتیجه = Curl_ssl_init_certinfo(داده, (صحيح)chain.رقم_certs);
      إذا(نتیجه)
        اذهب بیرون;

      لكل(i = 0; i < chain.رقم_certs; i++) {
        ثابت uint8_t *beg = chain.certs[i].داده;
        ثابت uint8_t *انتها = beg + chain.certs[i].اندازه;

        نتیجه = Curl_استخراج_certinfo(داده, (صحيح)i, beg, انتها);
        إذا(نتیجه)
          اذهب بیرون;
      }
    }
  }

  إذا(config->verifypeer) {
    نتیجه = gtls_verify_cert(داده, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    إذا(نتیجه)
      اذهب بیرون;
  }
  وإلا
    infof(داده, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  إذا(gnutls_x509_crt_init(&x509_cert)) {
    failf(داده, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_یافت;
    نتیجه = CURLE_SSL_CONNECT_ERROR;
    اذهب بیرون;
  }

  إذا(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    إذا(rc) {
      failf(داده, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_یافت;
      نتیجه = CURLE_SSL_CONNECT_ERROR;
      اذهب بیرون;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_زمان(x509_cert);

  إذا(certclock == (time_t)-1) {
    إذا(config->verifypeer) {
      failf(داده, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      نتیجه = CURLE_SSL_CONNECT_ERROR;
      اذهب بیرون;
    }
    وإلا
      infof(داده, "  SSL certificate expiration date verify FAILED");
  }
  وإلا {
    إذا(certclock < زمان(عدم)) {
      إذا(config->verifypeer) {
        failf(داده, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        نتیجه = CURLE_PEER_FAILED_VERIFICATION;
        اذهب بیرون;
      }
      وإلا
        infof(داده, "  SSL certificate expiration date FAILED");
    }
    وإلا
      infof(داده, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_زمان(x509_cert);

  إذا(certclock == (time_t)-1) {
    إذا(config->verifypeer) {
      failf(داده, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      نتیجه = CURLE_SSL_CONNECT_ERROR;
      اذهب بیرون;
    }
    وإلا
      infof(داده, "  SSL certificate activation date verify FAILED");
  }
  وإلا {
    إذا(certclock > زمان(عدم)) {
      إذا(config->verifypeer) {
        failf(داده, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        نتیجه = CURLE_PEER_FAILED_VERIFICATION;
        اذهب بیرون;
      }
      وإلا
        infof(داده, "  SSL certificate activation date FAILED");
    }
    وإلا
      infof(داده, "  SSL certificate activation date OK");
  }

  إذا(config->verifystatus && !ocsp_checked) {
    نتیجه = gtls_verify_ocsp_وضعیت(داده, session, x509_cert);
    إذا(نتیجه)
      اذهب بیرون;
  }
  وإلا
    infof(داده, "  SSL certificate status verification SKIPPED");

  إذا(config->issuercert) {
    gnutls_datum_t issuerp;
    إذا(gnutls_x509_crt_init(&x509_issuer)) {
      failf(داده, "failed to init GnuTLS x509_crt for issuer");
      نتیجه = CURLE_SSL_ISSUER_ERROR;
      اذهب بیرون;
    }
    issuerp = بارگیری_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    إذا(rc) {
      failf(داده, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      نتیجه = CURLE_SSL_ISSUER_ERROR;
      اذهب بیرون;
    }
    rc = (صحيح)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    إذا(rc <= 0) {
      failf(داده, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      نتیجه = CURLE_SSL_ISSUER_ERROR;
      اذهب بیرون;
    }
    infof(داده, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  نتیجه = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_پیام_verify_نتیجه(داده, peer, x509_cert, rc, config->verifyhost);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(pinned_کلید) {
    نتیجه = pkp_pin_peer_pubkey(داده, x509_cert, pinned_کلید);
    إذا(نتیجه != CURLE_OK) {
      failf(داده, "SSL: public key does not match pinned public key");
      اذهب بیرون;
    }
  }

  gtls_infof_cert(داده, x509_cert);

بیرون:
  إذا(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  إذا(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  أرجع نتیجه;
}

ساكن CURLcode gtls_verifyserver(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده,
                                  gnutls_session_t session)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, داده);
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  ثابت حرف *pinned_کلید = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY);
#وإلا
  ثابت حرف *pinned_کلید = CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY);
#پایان_اگر
  CURLcode نتیجه;

  نتیجه = Curl_gtls_verifyserver(cf, داده, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_کلید);
  إذا(نتیجه)
    اذهب بیرون;

#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  إذا(gnutls_protocol_get_نسخه(session) < GNUTLS_TLS1_3)
    نتیجه = cf_gtls_update_session_id(cf, داده, session);
#پایان_اگر

بیرون:
  أرجع نتیجه;
}

#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
ساكن CURLcode gtls_send_earlydata(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;
  ثابت بیعلامت حرف *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_نتیجه = CURLE_OK;
  طالما(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_رکورد_send_early_داده(backend->gtls.session, buf, blen);
    CURL_TRC_CF(داده, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    إذا(n < 0) {
      إذا(n == GNUTLS_E_AGAIN)
        نتیجه = CURLE_AGAIN;
      وإلا
        نتیجه = backend->gtls.io_نتیجه ?
                 backend->gtls.io_نتیجه : CURLE_SEND_ERROR;
      اذهب بیرون;
    }
    وإلا إذا(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(داده, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
بیرون:
  أرجع نتیجه;
}
#پایان_اگر

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
                                    بنية Curl_easy *داده,
                                    bool *انجامشده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;

  /* Initiate the connection, if not already done */
  إذا(connssl->connecting_state == ssl_connect_1) {
    نتیجه = gtls_connect_step1(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
    connssl->connecting_state = ssl_connect_2;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
    إذا(connssl->earlydata_state == ssl_earlydata_await) {
      اذهب بیرون;
    }
    وإلا إذا(connssl->earlydata_state == ssl_earlydata_sending) {
      نتیجه = gtls_send_earlydata(cf, داده);
      إذا(نتیجه)
        اذهب بیرون;
      connssl->earlydata_state = ssl_earlydata_فرستاده;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_فرستاده));
#پایان_اگر
    نتیجه = cf_gtls_handshake(cf, داده);
    إذا(نتیجه)
      اذهب بیرون;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  إذا(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    صحيح rc;

    Curl_gtls_report_handshake(داده, &backend->gtls);

    نتیجه = gtls_verifyserver(cf, داده, backend->gtls.session);
    إذا(نتیجه)
      اذهب بیرون;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    إذا(rc) {  /* No ALPN from server */
      proto.داده = عدم;
      proto.اندازه = 0;
    }

    نتیجه = Curl_alpn_مجموعه_negotiated(cf, داده, connssl,
                                      proto.داده, proto.اندازه);
    إذا(نتیجه)
      اذهب بیرون;

#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
    إذا(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_فرستاده);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_داده) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#پایان_اگر
    connssl->connecting_state = ssl_connect_انجامشده;
  }

  إذا(connssl->connecting_state == ssl_connect_انجامشده)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

بیرون:
  إذا(نتیجه == CURLE_AGAIN) {
    *انجامشده = FALSE;
    أرجع CURLE_OK;
  }
  *انجامشده = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(داده, cf, "gtls_connect_common() -> %d, done=%d", (صحيح)نتیجه,
              *انجامشده);
  أرجع نتیجه;
}

ساكن CURLcode gtls_connect(بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده,
                             bool *انجامشده)
{
#اگر_تعریف_شده CURL_GNUTLS_EARLY_داده
  بنية ssl_connect_داده *connssl = cf->ctx;
  إذا((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }
#پایان_اگر
  أرجع gtls_connect_common(cf, داده, انجامشده);
}

ساكن bool gtls_داده_pending(بنية Curl_cfilter *cf,
                              ثابت بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *ctx = cf->ctx;
  بنية gtls_ssl_backend_داده *backend;

  (فراغ)داده;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية gtls_ssl_backend_داده *)ctx->backend;
  إذا(backend->gtls.session &&
     gnutls_رکورد_check_pending(backend->gtls.session) != 0)
    أرجع TRUE;
  أرجع FALSE;
}

ساكن CURLcode gtls_send(بنية Curl_cfilter *cf,
                          بنية Curl_easy *داده,
                          ثابت فراغ *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  طالما(remain) {
    backend->gtls.io_نتیجه = CURLE_OK;
    nwritten = gnutls_رکورد_send(backend->gtls.session, buf, remain);

    إذا(nwritten >= 0) {
      إذا((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(داده, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        أرجع CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (حرف *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    وإلا {
      إذا(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        نتیجه = CURLE_OK;
        اذهب بیرون;
      }
      نتیجه = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_نتیجه ? backend->gtls.io_نتیجه : CURLE_SEND_ERROR);
      اذهب بیرون;
    }
  }

بیرون:
  CURL_TRC_CF(داده, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (صحيح)نتیجه, *pnwritten);
  أرجع نتیجه;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ساكن CURLcode gtls_shutdown(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              bool send_shutdown, bool *انجامشده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  حرف buf[1024];
  CURLcode نتیجه = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  إذا(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *انجامشده = TRUE;
    اذهب بیرون;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *انجامشده = FALSE;

  إذا(!backend->gtls.فرستاده_shutdown) {
    /* do this only once */
    backend->gtls.فرستاده_shutdown = TRUE;
    إذا(send_shutdown) {
      صحيح ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      إذا((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(داده, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_رکورد_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.فرستاده_shutdown = FALSE;
        نتیجه = CURLE_OK;
        اذهب بیرون;
      }
      إذا(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(داده, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((صحيح)ret), (صحيح)ret);
        نتیجه = CURLE_RECV_ERROR;
        اذهب بیرون;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  لكل(i = 0; i < 10; ++i) {
    nread = gnutls_رکورد_recv(backend->gtls.session, buf, حجم(buf));
    إذا(nread <= 0)
      اكسر;
  }
  إذا(nread > 0) {
    /* still data coming in? */
  }
  وإلا إذا(nread == 0) {
    /* We got the close notify alert and are done. */
    *انجامشده = TRUE;
  }
  وإلا إذا((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_رکورد_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  وإلا {
    CURL_TRC_CF(داده, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((صحيح)nread), (صحيح)nread);
    نتیجه = CURLE_RECV_ERROR;
  }

بیرون:
  cf->shutdown = (نتیجه || *انجامشده);
  أرجع نتیجه;
}

ساكن فراغ gtls_close(بنية Curl_cfilter *cf,
                       بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(داده, cf, "close");
  إذا(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = عدم;
  }
  إذا(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ساكن CURLcode gtls_recv(بنية Curl_cfilter *cf,
                          بنية Curl_easy *داده,
                          حرف *buf, size_t blen,
                          size_t *pnread)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_رکورد_recv(backend->gtls.session, buf, blen);

  إذا(nread >= 0)
    *pnread = (size_t)nread;
  وإلا {
    إذا((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      نتیجه = CURLE_AGAIN;
      اذهب بیرون;
    }
    وإلا إذا(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      نتیجه = cf_gtls_handshake(cf, داده);
      إذا(!نتیجه)
        نتیجه = CURLE_AGAIN; /* make us get called again. */
      اذهب بیرون;
    }
    وإلا {
      failf(داده, "GnuTLS recv error (%d): %s",
            (صحيح)nread, gnutls_strerror((صحيح)nread));
      نتیجه = backend->gtls.io_نتیجه ?
        backend->gtls.io_نتیجه : CURLE_RECV_ERROR;
      اذهب بیرون;
    }
  }

بیرون:
  CURL_TRC_CF(داده, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (صحيح)نتیجه, nread);
  أرجع نتیجه;
}

size_t Curl_gtls_نسخه(حرف *میانگیر, size_t اندازه)
{
  أرجع curl_msnprintf(میانگیر, اندازه, "GnuTLS/%s", gnutls_check_نسخه(عدم));
}

/* data might be NULL! */
ساكن CURLcode gtls_random(بنية Curl_easy *داده,
                            بیعلامت حرف *entropy, size_t درازی)
{
  صحيح rc;
  (فراغ)داده;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, درازی);
  أرجع rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ساكن CURLcode gtls_sha256sum(ثابت بیعلامت حرف *درون_داد,
                               size_t len,
                               بیعلامت حرف *sha256sum, /* output */
                               size_t sha256len)
{
  بنية sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  افعل {
    بیعلامت صحيح ilen = (بیعلامت صحيح)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, درون_داد);
    len -= ilen;
    درون_داد += ilen;
  } طالما(len);
#إذا NETTLE_نسخه_MAJOR >= 4
  (فراغ)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#وإلا
  sha256_digest(&SHA256pw, (بیعلامت صحيح)sha256len, sha256sum);
#پایان_اگر
  أرجع CURLE_OK;
}

ساكن bool gtls_cert_وضعیت_request(فراغ)
{
  أرجع TRUE;
}

ساكن فراغ *gtls_get_internals(بنية ssl_connect_داده *connssl,
                                CURLINFO info)
{
  بنية gtls_ssl_backend_داده *backend =
    (بنية gtls_ssl_backend_داده *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع backend->gtls.session;
}

ثابت بنية Curl_ssl Curl_ssl_gnutls = {
  { CURLSSLBACKEND_GNUTLS, "gnutls" }, /* info */

  SSLSUPP_CA_مسیر  |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_CA_CACHE |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_CRLFILE,

  حجم(بنية gtls_ssl_backend_داده),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_نسخه,             /*نسخه*/
  gtls_shutdown,                 /* shutdown */
  gtls_داده_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_وضعیت_request,      /* cert_status_request */
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

#پایان_اگر /* USE_GNUTLS */
