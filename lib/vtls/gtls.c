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
#অন্তর্ভাৱ "curl_setup.h"

#ifdef USE_GNUTLS

#অন্তর্ভাৱ <gnutls/abstract.h>
#অন্তর্ভাৱ <gnutls/gnutls.h>
#অন্তর্ভাৱ <gnutls/x509.h>
#অন্তর্ভাৱ <gnutls/crypto.h>
#অন্তর্ভাৱ <nettle/sha2.h>
#অন্তর্ভাৱ <nettle/version.h>

#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "vtls/keylog.h"
#অন্তর্ভাৱ "vtls/gtls.h"
#অন্তর্ভাৱ "vtls/vtls.h"
#অন্তর্ভাৱ "vtls/vtls_int.h"
#অন্তর্ভাৱ "vtls/vtls_scache.h"
#অন্তর্ভাৱ "vtls/apple.h"
#অন্তর্ভাৱ "vauth/vauth.h"
#অন্তর্ভাৱ "parsedate.h"
#অন্তর্ভাৱ "connect.h" /* for the connect timeout */
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "curlx/strdup.h"
#অন্তর্ভাৱ "curlx/win32-fopen.h"
#অন্তর্ভাৱ "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#যদি 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
স্থির শূন্য tls_log_func(পূর্ণাঙ্ক level, ধ্রুৱ ৱর্ণ *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#যদি !defined(GNUTLS_VERSION_সংখ্যা) || (GNUTLS_VERSION_সংখ্যা < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#যদি GNUTLS_VERSION_সংখ্যা >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#অন্তর্ভাৱ <gnutls/ocsp.h>

সংরচনা gtls_ssl_backend_data {
  সংরচনা gtls_ctx gtls;
};

স্থির ssize_t gtls_ঠেলো(শূন্য *s, ধ্রুৱ শূন্য *buf, size_t blen)
{
  সংরচনা Curl_cfilter *cf = s;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (পূর্ণাঙ্ক)result, nwritten);
  backend->gtls.io_result = result;
  যদি(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ঘূৰাই -1;
  }
  ঘূৰাই (ssize_t)nwritten;
}

স্থির ssize_t gtls_pull(শূন্য *s, শূন্য *buf, size_t blen)
{
  সংরচনা Curl_cfilter *cf = s;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  যদি(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    যদি(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      ঘূৰাই -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (পূর্ণাঙ্ক)result,
              nread);
  backend->gtls.io_result = result;
  যদি(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ঘূৰাই -1;
  }
  অন্যথা যদি(nread == 0)
    connssl->peer_closed = TRUE;
  ঘূৰাই (ssize_t)nread;
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
স্থির পূর্ণাঙ্ক gtls_init(শূন্য)
{
  পূর্ণাঙ্ক ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  ঘূৰাই ret;
}

স্থির শূন্য gtls_cleanup(শূন্য)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
স্থির শূন্য showtime(সংরচনা Curl_easy *data, ধ্রুৱ ৱর্ণ *text, time_t stamp)
{
  সংরচনা tm buffer;
  ধ্রুৱ সংরচনা tm *tm = &buffer;
  ৱর্ণ str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  যদি(result)
    ঘূৰাই;

  curl_msnprintf(str,
                 পরিমাণ(str),
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

স্থির gnutls_datum_t load_file(ধ্রুৱ ৱর্ণ *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { রিক্ত, 0 };
  long filelen;
  শূন্য *ptr;

  f = curlx_fopen(file, "rb");
  যদি(!f)
    ঘূৰাই loaded_file;
  যদি(fseek(f, 0, SEEK_END))
    গচ্ছ out;
  filelen = ftell(f);
  যদি(filelen < 0 || filelen > CURL_MAX_প্ৰবেশ_LENGTH)
    গচ্ছ out;
  যদি(fseek(f, 0, SEEK_SET))
    গচ্ছ out;
  ptr = curlx_malloc((size_t)filelen);
  যদি(!ptr)
    গচ্ছ out;
  যদি(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    গচ্ছ out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned পূর্ণাঙ্ক)filelen;
out:
  curlx_fclose(f);
  ঘূৰাই loaded_file;
}

স্থির শূন্য unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
স্থির CURLcode cf_gtls_handshake(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  পূর্ণাঙ্ক rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  যদি(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    যদি(result)
      ঘূৰাই result;
  }

  যদি((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    ঘূৰাই CURLE_AGAIN;
  }
  অন্যথা যদি((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    ধ্রুৱ ৱর্ণ *strerr = রিক্ত;

    যদি(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    যদি(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    ঘূৰাই CURLE_AGAIN;
  }
  অন্যথা যদি((rc < 0) && backend->gtls.io_result) {
    ঘূৰাই backend->gtls.io_result;
  }
  অন্যথা যদি(rc < 0) {
    ধ্রুৱ ৱর্ণ *strerr = রিক্ত;

    যদি(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    যদি(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }

  ঘূৰাই CURLE_OK;
}

স্থির gnutls_x509_crt_fmt_t gnutls_do_file_type(ধ্রুৱ ৱর্ণ *type)
{
  যদি(!type || !type[0])
    ঘূৰাই GNUTLS_X509_FMT_PEM;
  যদি(curl_strequal(type, "PEM"))
    ঘূৰাই GNUTLS_X509_FMT_PEM;
  যদি(curl_strequal(type, "DER"))
    ঘূৰাই GNUTLS_X509_FMT_DER;
  ঘূৰাই GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

স্থির CURLcode gnutls_set_ssl_version_min_max(
  সংরচনা Curl_easy *data,
  সংরচনা ssl_peer *peer,
  সংরচনা ssl_filter_config *conn_config,
  ধ্রুৱ ৱর্ণ **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  যদি(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  যদি((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  যদি(peer->transport == TRNSPRT_QUIC) {
    যদি(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    ঘূৰাই CURLE_OK;
  }

  ৱিকল্প(ssl_version | ssl_version_max) {
  পক্ষ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    ঘূৰাই CURLE_OK;
  পক্ষ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    ঘূৰাই CURLE_OK;
  পক্ষ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    ঘূৰাই CURLE_OK;
  পক্ষ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    ঘূৰাই CURLE_OK;
  পক্ষ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    ঘূৰাই CURLE_OK;
  পক্ষ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    ঘূৰাই CURLE_OK;
  পক্ষ CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    ঘূৰাই CURLE_OK;
  পক্ষ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    ঘূৰাই CURLE_OK;
  পক্ষ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    ঘূৰাই CURLE_OK;
  পক্ষ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    ঘূৰাই CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  ঘূৰাই CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(সংরচনা Curl_easy *data,
                                       সংরচনা gtls_shared_creds **pcreds)
{
  সংরচনা gtls_shared_creds *shared;
  পূর্ণাঙ্ক rc;

  *pcreds = রিক্ত;
  shared = curlx_calloc(1, পরিমাণ(*shared));
  যদি(!shared)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  যদি(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(সংরচনা gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  যদি(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    ঘূৰাই CURLE_OK;
  }
  ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
}

শূন্য Curl_gtls_shared_creds_free(সংরচনা gtls_shared_creds **pcreds)
{
  সংরচনা gtls_shared_creds *shared = *pcreds;
  *pcreds = রিক্ত;
  যদি(shared) {
    --shared->refcount;
    যদি(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

স্থির CURLcode gtls_populate_creds(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  সংরচনা ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  পূর্ণাঙ্ক rc;

  যদি(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    ঘূৰাই CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  যদি(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#অন্যথা
    rc = gnutls_certificate_set_x509_system_trust(creds);
    যদি(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    অন্যথা {
      infof(data, "  Native: %d certificates from system trust", rc);
      যদি(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  যদি(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    যদি(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      ঘূৰাই CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned পূর্ণাঙ্ক)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    যদি(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      যদি(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ঘূৰাই CURLE_SSL_CACERT_BADFILE;
      }
    }
    অন্যথা
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  অন্যথা যদি(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    যদি(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      যদি(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ঘূৰাই CURLE_SSL_CACERT_BADFILE;
      }
    }
    অন্যথা
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  যদি(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    যদি(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      যদি(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ঘূৰাই CURLE_SSL_CACERT_BADFILE;
      }
    }
    অন্যথা
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  যদি(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  যদি(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ঘূৰাই CURLE_NOT_BUILT_IN;
  }
#endif
  যদি(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    যদি(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      ঘূৰাই CURLE_SSL_CRL_BADFILE;
    }
    অন্যথা
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  ঘূৰাই CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_চাবি   "tls:gtls:x509:share"

স্থির bool gtls_shared_creds_expired(সংরচনা Curl_easy *data,
                                      ধ্রুৱ সংরচনা gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  যদি(timeout_ms < 0)
    ঘূৰাই FALSE;

  ঘূৰাই elapsed_ms >= timeout_ms;
}

স্থির bool gtls_shared_creds_different(সংরচনা Curl_cfilter *cf,
                                        ধ্রুৱ সংরচনা gtls_shared_creds *sc)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  যদি(!sc->CAfile || !conn_config->CAfile)
    ঘূৰাই sc->CAfile != conn_config->CAfile;

  ঘূৰাই strcmp(sc->CAfile, conn_config->CAfile);
}

স্থির সংরচনা gtls_shared_creds *gtls_get_cached_creds(সংরচনা Curl_cfilter *cf,
                                                       সংরচনা Curl_easy *data)
{
  সংরচনা gtls_shared_creds *shared_creds;

  যদি(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_চাবি,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_চাবি));
    যদি(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      ঘূৰাই shared_creds;
    }
  }
  ঘূৰাই রিক্ত;
}

স্থির শূন্য gtls_shared_creds_hash_free(ধ্রুৱ শূন্য *চাবি,
                                        size_t চাবি_len, শূন্য *p)
{
  সংরচনা gtls_shared_creds *sc = p;
  DEBUGASSERT(চাবি_len == CURL_CSTRLEN(MPROTO_GTLS_X509_চাবি));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_চাবি, চাবি, চাবি_len));
  (শূন্য)চাবি;
  (শূন্য)চাবি_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

স্থির শূন্য gtls_set_cached_creds(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা gtls_shared_creds *sc)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  যদি(!data->multi)
    ঘূৰাই;

  যদি(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    যদি(!sc->CAfile)
      ঘূৰাই;
  }

  যদি(Curl_gtls_shared_creds_up_ref(sc))
    ঘূৰাই;

  যদি(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_চাবি,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_চাবি),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    ঘূৰাই;
  }
}

CURLcode Curl_gtls_client_trust_setup(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      সংরচনা gtls_ctx *gtls)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা gtls_shared_creds *cached_creds = রিক্ত;
  bool cache_criteria_met;
  CURLcode result;
  পূর্ণাঙ্ক rc;

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

  যদি(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  যদি(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    যদি(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
  }
  অন্যথা {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    যদি(result)
      ঘূৰাই result;
    gtls->shared_creds->trust_setup = TRUE;
    যদি(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  ঘূৰাই CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
স্থির পূর্ণাঙ্ক gtls_get_ietf_proto(gnutls_session_t session)
{
  ৱিকল্প(gnutls_protocol_get_version(session)) {
  পক্ষ GNUTLS_SSL3:
    ঘূৰাই CURL_IETF_PROTO_SSL3;
  পক্ষ GNUTLS_TLS1_0:
    ঘূৰাই CURL_IETF_PROTO_TLS1;
  পক্ষ GNUTLS_TLS1_1:
    ঘূৰাই CURL_IETF_PROTO_TLS1_1;
  পক্ষ GNUTLS_TLS1_2:
    ঘূৰাই CURL_IETF_PROTO_TLS1_2;
  পক্ষ GNUTLS_TLS1_3:
    ঘূৰাই CURL_IETF_PROTO_TLS1_3;
  স্ৱভাৱ:
    ঘূৰাই CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 ধ্রুৱ ৱর্ণ *ssl_peer_চাবি,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ধ্রুৱ ৱর্ণ *alpn,
                                 unsigned ৱর্ণ *quic_tp,
                                 size_t quic_tp_len,
                                 সংরচনা Curl_ssl_session **psession)
{
  সংরচনা Curl_ssl_session *sc_session = রিক্ত, *sc_dup = রিক্ত;
  unsigned ৱর্ণ *sdata, *qtp_clone = রিক্ত;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  যদি(psession)
    *psession = রিক্ত;
  যদি(!Curl_ssl_scache_use(cf, data))
    ঘূৰাই CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, রিক্ত, &sdata_len);
  যদি(!sdata_len) /* GnuTLS does this for some version combinations */
    ঘূৰাই CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  যদি(!sdata)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  যদি(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ধ্রুৱ ৱর্ণ *)quic_tp, quic_tp_len);
    যদি(!qtp_clone) {
      curlx_free(sdata);
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  যদি(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  যদি(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_চাবি, sc_session);
    /* took ownership of `sc_session` */
    sc_session = রিক্ত;
  }
  যদি(!result && psession) {
    *psession = sc_dup;
    sc_dup = রিক্ত;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ঘূৰাই result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
স্থির CURLcode cf_gtls_update_session_id(সংরচনা Curl_cfilter *cf,
                                          সংরচনা Curl_easy *data,
                                          gnutls_session_t session)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা Curl_ssl_session *scs = রিক্ত;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_চাবি,
                                   session, 0, connssl->negotiated.alpn,
                                   রিক্ত, 0, &scs);
  যদি(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  ঘূৰাই result;
}

স্থির পূর্ণাঙ্ক gtls_handshake_cb(gnutls_session_t session, unsigned পূর্ণাঙ্ক htype,
                             unsigned when, unsigned পূর্ণাঙ্ক incoming,
                             ধ্রুৱ gnutls_datum_t *বাৰ্তা)
{
  সংরচনা Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (শূন্য)বাৰ্তা;
  (শূন্য)incoming;
  যদি(when) { /* after message has been processed */
    সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
    যদি(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      ৱিকল্প(htype) {
      পক্ষ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        ভঙ্গ;
      }
      স্ৱভাৱ:
        ভঙ্গ;
      }
    }
  }
  ঘূৰাই 0;
}
#endif

স্থির CURLcode gtls_set_priority(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা gtls_ctx *gtls,
                                  ধ্রুৱ ৱর্ণ *priority)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা dynbuf buf;
  ধ্রুৱ ৱর্ণ *err = রিক্ত;
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক rc;

  curlx_dyn_init(&buf, 4096);

  যদি(conn_config->cipher_list) {
    যদি((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      যদি(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        যদি(result)
          গচ্ছ out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      যদি(result)
        গচ্ছ out;
      priority = curlx_dyn_ptr(&buf);
    }
    অন্যথা /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  যদি(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  ঘূৰাই result;
}

স্থির CURLcode gtls_client_init(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা ssl_peer *peer,
                                 size_t earlydata_max,
                                 সংরচনা gtls_ctx *gtls)
{
  সংরচনা ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned পূর্ণাঙ্ক init_flags;
  পূর্ণাঙ্ক rc;
  ধ্রুৱ ৱর্ণ *prioritylist;
  bool tls13support;
  CURLcode result;

  যদি(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  যদি(result)
    ঘূৰাই result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  যদি(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  অন্যথা যদি(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  যদি(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  যদি(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }

  যদি(peer->sni) {
    যদি(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  যদি(rc != GNUTLS_E_SUCCESS)
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  যদি(config->version == CURL_SSLVERSION_TLSv1_3) {
    যদি(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  যদি(result)
    ঘূৰাই result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  যদি(result)
    ঘূৰাই result;

  যদি(config->clientcert) {
    যদি(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      যদি(result)
        ঘূৰাই result;
    }
    যদি(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->চাবি_passwd ? config->চাবি_passwd : "");
      যদি(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        ঘূৰাই CURLE_SSL_CONNECT_ERROR;
      }
    }
    অন্যথা {
      ধ্রুৱ unsigned পূর্ণাঙ্ক supported_চাবি_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_চাবি_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->চাবি ? config->চাবি : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->চাবি_passwd,
           supported_চাবি_encryption_algorithms);
      যদি(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->চাবি_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        ঘূৰাই CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  যদি(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }

  যদি(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  রিক্ত, 0, রিক্ত);
    যদি(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      ঘূৰাই CURLE_SSL_CONNECT_ERROR;
    }
  }

  ঘূৰাই CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
স্থির পূর্ণাঙ্ক keylog_callback(gnutls_session_t session, ধ্রুৱ ৱর্ণ *label,
                           ধ্রুৱ gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  যদি(crandom.size != 32) {
    ঘূৰাই -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  ঘূৰাই 0;
}

স্থির CURLcode gtls_on_session_reuse(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      সংরচনা alpn_spec *alpns,
                                      সংরচনা Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  ঘূৰাই Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

স্থির CURLcode gtls_apply_session(
  সংরচনা gtls_ctx *gctx,
  সংরচনা Curl_cfilter *cf,
  সংরচনা Curl_easy *data,
  সংরচনা ssl_peer *peer,
  সংরচনা alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  সংরচনা Curl_ssl_session *scs,
  bool *pwas_setup)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক rc;

  যদি(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    যদি(result)
      গচ্ছ out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    যদি(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    অন্যথা {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      যদি(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        যদি(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          যদি(result)
            গচ্ছ out;
        }
        যদি(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  ঘূৰাই result;
}

CURLcode Curl_gtls_ctx_init(সংরচনা gtls_ctx *gctx,
                            সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            সংরচনা ssl_peer *peer,
                            ধ্রুৱ সংরচনা alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            শূন্য *cb_user_data,
                            শূন্য *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা Curl_ssl_session *scs = রিক্ত;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_গুনতি = 0;
  bool gtls_session_setup = FALSE;
  সংরচনা alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  যদি((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    যদি(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = রিক্ত;
      যদি(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  যদি(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_চাবি, &scs);
    যদি(result)
      গচ্ছ out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  যদি(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    যদি(result)
      গচ্ছ out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  যদি(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    যদি(result)
      গচ্ছ out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  যদি(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  যদি(!gtls_alpns_গুনতি && alpns.গুনতি) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.গুনতি);
    বাবে(i = 0; i < alpns.গুনতি; ++i) {
      gtls_alpns[i].data = (unsigned ৱর্ণ *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned পূর্ণাঙ্ক)strlen(alpns.entries[i]);
    }
    gtls_alpns_গুনতি = alpns.গুনতি;
  }

  যদি(gtls_alpns_গুনতি &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned পূর্ণাঙ্ক)gtls_alpns_গুনতি,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_চাবি, scs);
  ঘূৰাই result;
}

স্থির CURLcode gtls_connect_step1(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  যদি(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    ঘূৰাই CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, রিক্ত, রিক্ত, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#অন্যথা
                              রিক্ত
#endif
                              );

  যদি(result)
    ঘূৰাই result;

  যদি(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    সংরচনা alpn_proto_buf proto;
    memset(&proto, 0, পরিমাণ(proto));
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
  gnutls_transport_set_ঠেলো_function(backend->gtls.session, gtls_ঠেলো);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  ঘূৰাই CURLE_OK;
}

স্থির CURLcode pkp_pin_peer_pubkey(সংরচনা Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    ধ্রুৱ ৱর্ণ *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned ৱর্ণ *buff1 = রিক্ত;

  gnutls_pubkey_t চাবি = রিক্ত;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  যদি(!pinnedpubkey)
    ঘূৰাই CURLE_OK;

  যদি(!cert)
    ঘূৰাই result;

  কৰা {
    পূর্ণাঙ্ক ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&চাবি);
    যদি(ret < 0)
      ভঙ্গ; /* failed */

    ret = gnutls_pubkey_import_x509(চাবি, cert, 0);
    যদি(ret < 0)
      ভঙ্গ; /* failed */

    ret = gnutls_pubkey_export(চাবি, GNUTLS_X509_FMT_DER, রিক্ত, &len1);
    যদি(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      ভঙ্গ; /* failed */

    buff1 = curlx_malloc(len1);
    যদি(!buff1)
      ভঙ্গ; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(চাবি, GNUTLS_X509_FMT_DER, buff1, &len2);
    যদি(ret < 0 || len1 != len2)
      ভঙ্গ; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } যেতিয়ালৈ(0);

  যদি(চাবি)
    gnutls_pubkey_deinit(চাবি);

  curlx_safefree(buff1);

  ঘূৰাই result;
}

শূন্য Curl_gtls_report_handshake(সংরচনা Curl_easy *data, সংরচনা gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  যদি(Curl_trc_is_verbose(data)) {
    ধ্রুৱ ৱর্ণ *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#অন্যথা
  (শূন্য)data;
  (শূন্য)gctx;
#endif
}

স্থির শূন্য gtls_বাৰ্তা_verify_result(সংরচনা Curl_easy *data,
                                   সংরচনা ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  ৱর্ণ certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = পরিমাণ(certname);
  পূর্ণাঙ্ক rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  যদি(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  যদি(!was_verified) {
    যদি(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    অন্যথা
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  অন্যথা
    infof(data, "  common name: %s (matched)", certname);
}

স্থির শূন্য gtls_infof_cert(সংরচনা Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  যদি(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    পূর্ণাঙ্ক rc, algo;
    time_t tstamp;
    unsigned পূর্ণাঙ্ক bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    যদি(rc)
      infof(data, "Failed to get certificate name");
    অন্যথা {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    যদি(rc)
      infof(data, "Failed to get certificate issuer");
    অন্যথা {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#অন্যথা
  (শূন্য)data;
  (শূন্য)x509_cert;
#endif
}

স্থির CURLcode gtls_verify_ocsp_status(সংরচনা Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = রিক্ত;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  পূর্ণাঙ্ক rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  যদি(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    গচ্ছ out;
  }
  অন্যথা যদি(rc < 0) {
    failf(data, "Invalid OCSP response received");
    গচ্ছ out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  যদি(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    গচ্ছ out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  যদি(rc < 0) {
    failf(data, "failed to parse OCSP response");
    গচ্ছ out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  যদি(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    গচ্ছ out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  বাবে(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    যদি(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      ভঙ্গ;
  }
  যদি(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    গচ্ছ out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, রিক্ত, রিক্ত, রিক্ত, রিক্ত,
                                   &status, রিক্ত, রিক্ত, রিক্ত, রিক্ত);
  যদি(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    গচ্ছ out;
  }
  ৱিকল্প(status) {
  পক্ষ GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    ভঙ্গ;
  পক্ষ GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    ভঙ্গ;
  স্ৱভাৱ:
    failf(data, "peer certificate OCSP status is not good: %d", (পূর্ণাঙ্ক)status);
    ভঙ্গ;
  }

out:
  যদি(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  ঘূৰাই result;
}

সংরচনা gtls_cert_chain {
  ধ্রুৱ gnutls_datum_t *certs;
  unsigned পূর্ণাঙ্ক অঙ্ক_certs;
};

#ifdef USE_APPLE_SECTRUST
স্থির CURLcode gtls_chain_get_der(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   শূন্য *user_data,
                                   size_t i,
                                   unsigned ৱর্ণ **pder,
                                   size_t *pder_len)
{
  সংরচনা gtls_cert_chain *chain = user_data;

  (শূন্য)cf;
  (শূন্য)data;
  *pder_len = 0;
  *pder = রিক্ত;

  যদি(i >= chain->অঙ্ক_certs)
    ঘূৰাই CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  ঘূৰাই CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
স্থির CURLcode gtls_verify_cert(সংরচনা Curl_easy *data,
                                 সংরচনা ssl_filter_config *config,
                                 সংরচনা ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 সংরচনা Curl_cfilter *cf,
                                 সংরচনা ssl_peer *peer,
                                 সংরচনা gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned পূর্ণাঙ্ক verify_status = 0;
  long * ধ্রুৱ certverifyresult = &ssl_config->certverifyresult;
  পূর্ণাঙ্ক rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  যদি(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    ঘূৰাই CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  যদি(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  যদি(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    যদি(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = রিক্ত;
      ocsp_raw.size = 0;
    }
    যদি(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      ঘূৰাই CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->অঙ্ক_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    যদি(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      ঘূৰাই result; /* no further failure analysis needed */
    verified = !result;
    যদি(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#অন্যথা
  (শূন্য)cf;
  (শূন্য)peer;
  (শূন্য)chain;
#endif

  যদি(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ধ্রুৱ ৱর্ণ *cause = "certificate error, no details available";
    যদি(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    অন্যথা যদি(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    অন্যথা যদি(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    অন্যথা যদি(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    ঘূৰাই result;
  }
  ঘূৰাই CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                gnutls_session_t session,
                                সংরচনা ssl_filter_config *config,
                                সংরচনা ssl_easy_config *ssl_config,
                                সংরচনা ssl_peer *peer,
                                ধ্রুৱ ৱর্ণ *pinned_চাবি)
{
  সংরচনা gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = রিক্ত, x509_issuer = রিক্ত;
  time_t certclock;
  bool ocsp_checked = FALSE;
  পূর্ণাঙ্ক rc;
  CURLcode result = CURLE_OK;
  long * ধ্রুৱ certverifyresult = &ssl_config->certverifyresult;

  (শূন্য)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.অঙ্ক_certs);
  যদি(!chain.certs) {
    যদি(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      গচ্ছ out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  যদি(data->set.ssl.certinfo && chain.certs) {
    যদি(chain.অঙ্ক_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.অঙ্ক_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ out;
    }
    অন্যথা {
      unsigned পূর্ণাঙ্ক i;

      result = Curl_ssl_init_certinfo(data, (পূর্ণাঙ্ক)chain.অঙ্ক_certs);
      যদি(result)
        গচ্ছ out;

      বাবে(i = 0; i < chain.অঙ্ক_certs; i++) {
        ধ্রুৱ uint8_t *beg = chain.certs[i].data;
        ধ্রুৱ uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (পূর্ণাঙ্ক)i, beg, end);
        যদি(result)
          গচ্ছ out;
      }
    }
  }

  যদি(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    যদি(result)
      গচ্ছ out;
  }
  অন্যথা
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  যদি(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    গচ্ছ out;
  }

  যদি(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    যদি(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  যদি(certclock == (time_t)-1) {
    যদি(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ out;
    }
    অন্যথা
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  অন্যথা {
    যদি(certclock < time(রিক্ত)) {
      যদি(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        গচ্ছ out;
      }
      অন্যথা
        infof(data, "  SSL certificate expiration date FAILED");
    }
    অন্যথা
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  যদি(certclock == (time_t)-1) {
    যদি(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ out;
    }
    অন্যথা
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  অন্যথা {
    যদি(certclock > time(রিক্ত)) {
      যদি(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        গচ্ছ out;
      }
      অন্যথা
        infof(data, "  SSL certificate activation date FAILED");
    }
    অন্যথা
      infof(data, "  SSL certificate activation date OK");
  }

  যদি(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    যদি(result)
      গচ্ছ out;
  }
  অন্যথা
    infof(data, "  SSL certificate status verification SKIPPED");

  যদি(config->issuercert) {
    gnutls_datum_t issuerp;
    যদি(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      গচ্ছ out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    যদি(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      গচ্ছ out;
    }
    rc = (পূর্ণাঙ্ক)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    যদি(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      গচ্ছ out;
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
  rc = (পূর্ণাঙ্ক)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_বাৰ্তা_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  যদি(result)
    গচ্ছ out;

  যদি(pinned_চাবি) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_চাবি);
    যদি(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      গচ্ছ out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  যদি(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  যদি(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  ঘূৰাই result;
}

স্থির CURLcode gtls_verifyserver(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  gnutls_session_t session)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  ধ্রুৱ ৱর্ণ *pinned_চাবি = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#অন্যথা
  ধ্রুৱ ৱর্ণ *pinned_চাবি = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_চাবি);
  যদি(result)
    গচ্ছ out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  যদি(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  ঘূৰাই result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
স্থির CURLcode gtls_send_earlydata(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ধ্রুৱ unsigned ৱর্ণ *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  যেতিয়ালৈ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    যদি(n < 0) {
      যদি(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      অন্যথা
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      গচ্ছ out;
    }
    অন্যথা যদি(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ঘূৰাই result;
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
স্থির CURLcode gtls_connect_common(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  যদি(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  যদি(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    যদি(result)
      গচ্ছ out;
    connssl->connecting_state = ssl_connect_2;
  }

  যদি(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    যদি(connssl->earlydata_state == ssl_earlydata_await) {
      গচ্ছ out;
    }
    অন্যথা যদি(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      যদি(result)
        গচ্ছ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    যদি(result)
      গচ্ছ out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  যদি(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    পূর্ণাঙ্ক rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    যদি(result)
      গচ্ছ out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    যদি(rc) {  /* No ALPN from server */
      proto.data = রিক্ত;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    যদি(result)
      গচ্ছ out;

#ifdef CURL_GNUTLS_EARLY_DATA
    যদি(connssl->earlydata_state > ssl_earlydata_none) {
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

  যদি(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  যদি(result == CURLE_AGAIN) {
    *done = FALSE;
    ঘূৰাই CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (পূর্ণাঙ্ক)result,
              *done);
  ঘূৰাই result;
}

স্থির CURLcode gtls_connect(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  যদি((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    ঘূৰাই CURLE_OK;
  }
#endif
  ঘূৰাই gtls_connect_common(cf, data, done);
}

স্থির bool gtls_data_pending(সংরচনা Curl_cfilter *cf,
                              ধ্রুৱ সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *ctx = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend;

  (শূন্য)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (সংরচনা gtls_ssl_backend_data *)ctx->backend;
  যদি(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    ঘূৰাই TRUE;
  ঘূৰাই FALSE;
}

স্থির CURLcode gtls_send(সংরচনা Curl_cfilter *cf,
                          সংরচনা Curl_easy *data,
                          ধ্রুৱ শূন্য *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  যেতিয়ালৈ(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    যদি(nwritten >= 0) {
      যদি((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        ঘূৰাই CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (ৱর্ণ *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    অন্যথা {
      যদি(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        গচ্ছ out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      গচ্ছ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (পূর্ণাঙ্ক)result, *pnwritten);
  ঘূৰাই result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
স্থির CURLcode gtls_shutdown(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  ৱর্ণ buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  যদি(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    গচ্ছ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  যদি(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    যদি(send_shutdown) {
      পূর্ণাঙ্ক ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      যদি((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        গচ্ছ out;
      }
      যদি(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((পূর্ণাঙ্ক)ret), (পূর্ণাঙ্ক)ret);
        result = CURLE_RECV_ERROR;
        গচ্ছ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  বাবে(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, পরিমাণ(buf));
    যদি(nread <= 0)
      ভঙ্গ;
  }
  যদি(nread > 0) {
    /* still data coming in? */
  }
  অন্যথা যদি(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  অন্যথা যদি((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  অন্যথা {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((পূর্ণাঙ্ক)nread), (পূর্ণাঙ্ক)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  ঘূৰাই result;
}

স্থির শূন্য gtls_close(সংরচনা Curl_cfilter *cf,
                       সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  যদি(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = রিক্ত;
  }
  যদি(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

স্থির CURLcode gtls_recv(সংরচনা Curl_cfilter *cf,
                          সংরচনা Curl_easy *data,
                          ৱর্ণ *buf, size_t blen,
                          size_t *pnread)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  যদি(nread >= 0)
    *pnread = (size_t)nread;
  অন্যথা {
    যদি((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      গচ্ছ out;
    }
    অন্যথা যদি(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      যদি(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      গচ্ছ out;
    }
    অন্যথা {
      failf(data, "GnuTLS recv error (%d): %s",
            (পূর্ণাঙ্ক)nread, gnutls_strerror((পূর্ণাঙ্ক)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      গচ্ছ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (পূর্ণাঙ্ক)result, nread);
  ঘূৰাই result;
}

size_t Curl_gtls_version(ৱর্ণ *buffer, size_t size)
{
  ঘূৰাই curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(রিক্ত));
}

/* data might be NULL! */
স্থির CURLcode gtls_random(সংরচনা Curl_easy *data,
                            unsigned ৱর্ণ *entropy, size_t length)
{
  পূর্ণাঙ্ক rc;
  (শূন্য)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  ঘূৰাই rc ? CURLE_FAILED_INIT : CURLE_OK;
}

স্থির CURLcode gtls_sha256sum(ধ্রুৱ unsigned ৱর্ণ *প্ৰবেশ,
                               size_t len,
                               unsigned ৱর্ণ *sha256sum, /* output */
                               size_t sha256len)
{
  সংরচনা sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  কৰা {
    unsigned পূর্ণাঙ্ক ilen = (unsigned পূর্ণাঙ্ক)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, প্ৰবেশ);
    len -= ilen;
    প্ৰবেশ += ilen;
  } যেতিয়ালৈ(len);
#যদি NETTLE_VERSION_MAJOR >= 4
  (শূন্য)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#অন্যথা
  sha256_digest(&SHA256pw, (unsigned পূর্ণাঙ্ক)sha256len, sha256sum);
#endif
  ঘূৰাই CURLE_OK;
}

স্থির bool gtls_cert_status_request(শূন্য)
{
  ঘূৰাই TRUE;
}

স্থির শূন্য *gtls_get_internals(সংরচনা ssl_connect_data *connssl,
                                CURLINFO info)
{
  সংরচনা gtls_ssl_backend_data *backend =
    (সংরচনা gtls_ssl_backend_data *)connssl->backend;
  (শূন্য)info;
  DEBUGASSERT(backend);
  ঘূৰাই backend->gtls.session;
}

ধ্রুৱ সংরচনা Curl_ssl Curl_ssl_gnutls = {
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

  পরিমাণ(সংরচনা gtls_ssl_backend_data),

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
  রিক্ত,                          /* close_all */
  রিক্ত,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  রিক্ত,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
