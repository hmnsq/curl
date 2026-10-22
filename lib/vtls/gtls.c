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

#ifdef USE_GNUTLS

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
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
ایستا تهی tls_log_func(صحیح level, ثابت نویسه *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#اگر !defined(GNUTLS_VERSION_عدد) || (GNUTLS_VERSION_عدد < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#اگر GNUTLS_VERSION_عدد >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#شامل <gnutls/ocsp.h>

ساختار gtls_ssl_backend_data {
  ساختار gtls_ctx gtls;
};

ایستا ssize_t gtls_افزودن(تهی *s, ثابت تهی *buf, size_t blen)
{
  ساختار Curl_cfilter *cf = s;
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (صحیح)result, nwritten);
  backend->gtls.io_result = result;
  اگر(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    بازگشت -1;
  }
  بازگشت (ssize_t)nwritten;
}

ایستا ssize_t gtls_pull(تهی *s, تهی *buf, size_t blen)
{
  ساختار Curl_cfilter *cf = s;
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  اگر(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    اگر(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      بازگشت -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (صحیح)result,
              nread);
  backend->gtls.io_result = result;
  اگر(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    بازگشت -1;
  }
  وگرنه اگر(nread == 0)
    connssl->peer_closed = TRUE;
  بازگشت (ssize_t)nread;
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
ایستا صحیح gtls_init(تهی)
{
  صحیح ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  بازگشت ret;
}

ایستا تهی gtls_cleanup(تهی)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
ایستا تهی showtime(ساختار Curl_easy *data, ثابت نویسه *text, time_t stamp)
{
  ساختار tm buffer;
  ثابت ساختار tm *tm = &buffer;
  نویسه str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  اگر(result)
    بازگشت;

  curl_msnprintf(str,
                 اندازه(str),
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

ایستا gnutls_datum_t load_file(ثابت نویسه *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { پوچ, 0 };
  long filelen;
  تهی *ptr;

  f = curlx_fopen(file, "rb");
  اگر(!f)
    بازگشت loaded_file;
  اگر(fseek(f, 0, SEEK_END))
    برو out;
  filelen = ftell(f);
  اگر(filelen < 0 || filelen > CURL_MAX_درون_داد_LENGTH)
    برو out;
  اگر(fseek(f, 0, SEEK_SET))
    برو out;
  ptr = curlx_malloc((size_t)filelen);
  اگر(!ptr)
    برو out;
  اگر(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    برو out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned صحیح)filelen;
out:
  curlx_fclose(f);
  بازگشت loaded_file;
}

ایستا تهی unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
ایستا CURLcode cf_gtls_handshake(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  صحیح rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  اگر(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    اگر(result)
      بازگشت result;
  }

  اگر((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    بازگشت CURLE_AGAIN;
  }
  وگرنه اگر((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    ثابت نویسه *strerr = پوچ;

    اگر(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    اگر(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    بازگشت CURLE_AGAIN;
  }
  وگرنه اگر((rc < 0) && backend->gtls.io_result) {
    بازگشت backend->gtls.io_result;
  }
  وگرنه اگر(rc < 0) {
    ثابت نویسه *strerr = پوچ;

    اگر(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    اگر(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  بازگشت CURLE_OK;
}

ایستا gnutls_x509_crt_fmt_t gnutls_do_file_type(ثابت نویسه *type)
{
  اگر(!type || !type[0])
    بازگشت GNUTLS_X509_FMT_PEM;
  اگر(curl_strequal(type, "PEM"))
    بازگشت GNUTLS_X509_FMT_PEM;
  اگر(curl_strequal(type, "DER"))
    بازگشت GNUTLS_X509_FMT_DER;
  بازگشت GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ایستا CURLcode gnutls_set_ssl_version_min_max(
  ساختار Curl_easy *data,
  ساختار ssl_peer *peer,
  ساختار ssl_filter_config *conn_config,
  ثابت نویسه **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  اگر(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  اگر((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  اگر(peer->transport == TRNSPRT_QUIC) {
    اگر(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    بازگشت CURLE_OK;
  }

  انتخاب(ssl_version | ssl_version_max) {
  حالت CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    بازگشت CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    بازگشت CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    بازگشت CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    بازگشت CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    بازگشت CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    بازگشت CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    بازگشت CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    بازگشت CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    بازگشت CURLE_OK;
  حالت CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    بازگشت CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  بازگشت CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(ساختار Curl_easy *data,
                                       ساختار gtls_shared_creds **pcreds)
{
  ساختار gtls_shared_creds *shared;
  صحیح rc;

  *pcreds = پوچ;
  shared = curlx_calloc(1, اندازه(*shared));
  اگر(!shared)
    بازگشت CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  اگر(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  بازگشت CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(ساختار gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  اگر(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    بازگشت CURLE_OK;
  }
  بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
}

تهی Curl_gtls_shared_creds_free(ساختار gtls_shared_creds **pcreds)
{
  ساختار gtls_shared_creds *shared = *pcreds;
  *pcreds = پوچ;
  اگر(shared) {
    --shared->refcount;
    اگر(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

ایستا CURLcode gtls_populate_creds(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  ساختار ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ساختار ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  صحیح rc;

  اگر(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    بازگشت CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  اگر(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#وگرنه
    rc = gnutls_certificate_set_x509_system_trust(creds);
    اگر(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    وگرنه {
      infof(data, "  Native: %d certificates from system trust", rc);
      اگر(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  اگر(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    اگر(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      بازگشت CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned صحیح)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    اگر(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      اگر(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        بازگشت CURLE_SSL_CACERT_BADFILE;
      }
    }
    وگرنه
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  وگرنه اگر(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    اگر(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      اگر(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        بازگشت CURLE_SSL_CACERT_BADFILE;
      }
    }
    وگرنه
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  اگر(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    اگر(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      اگر(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        بازگشت CURLE_SSL_CACERT_BADFILE;
      }
    }
    وگرنه
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  اگر(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  اگر(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    بازگشت CURLE_NOT_BUILT_IN;
  }
#endif
  اگر(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    اگر(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      بازگشت CURLE_SSL_CRL_BADFILE;
    }
    وگرنه
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  بازگشت CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_کلید   "tls:gtls:x509:share"

ایستا bool gtls_shared_creds_expired(ساختار Curl_easy *data,
                                      ثابت ساختار gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  اگر(timeout_ms < 0)
    بازگشت FALSE;

  بازگشت elapsed_ms >= timeout_ms;
}

ایستا bool gtls_shared_creds_different(ساختار Curl_cfilter *cf,
                                        ثابت ساختار gtls_shared_creds *sc)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  اگر(!sc->CAfile || !conn_config->CAfile)
    بازگشت sc->CAfile != conn_config->CAfile;

  بازگشت strcmp(sc->CAfile, conn_config->CAfile);
}

ایستا ساختار gtls_shared_creds *gtls_get_cached_creds(ساختار Curl_cfilter *cf,
                                                       ساختار Curl_easy *data)
{
  ساختار gtls_shared_creds *shared_creds;

  اگر(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_کلید,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_کلید));
    اگر(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      بازگشت shared_creds;
    }
  }
  بازگشت پوچ;
}

ایستا تهی gtls_shared_creds_hash_free(ثابت تهی *کلید,
                                        size_t کلید_len, تهی *p)
{
  ساختار gtls_shared_creds *sc = p;
  DEBUGASSERT(کلید_len == CURL_CSTRLEN(MPROTO_GTLS_X509_کلید));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_کلید, کلید, کلید_len));
  (تهی)کلید;
  (تهی)کلید_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

ایستا تهی gtls_set_cached_creds(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ساختار gtls_shared_creds *sc)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  اگر(!data->multi)
    بازگشت;

  اگر(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    اگر(!sc->CAfile)
      بازگشت;
  }

  اگر(Curl_gtls_shared_creds_up_ref(sc))
    بازگشت;

  اگر(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_کلید,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_کلید),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    بازگشت;
  }
}

CURLcode Curl_gtls_client_trust_setup(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      ساختار gtls_ctx *gtls)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار gtls_shared_creds *cached_creds = پوچ;
  bool cache_criteria_met;
  CURLcode result;
  صحیح rc;

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

  اگر(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  اگر(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    اگر(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
  }
  وگرنه {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    اگر(result)
      بازگشت result;
    gtls->shared_creds->trust_setup = TRUE;
    اگر(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  بازگشت CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ایستا صحیح gtls_get_ietf_proto(gnutls_session_t session)
{
  انتخاب(gnutls_protocol_get_version(session)) {
  حالت GNUTLS_SSL3:
    بازگشت CURL_IETF_PROTO_SSL3;
  حالت GNUTLS_TLS1_0:
    بازگشت CURL_IETF_PROTO_TLS1;
  حالت GNUTLS_TLS1_1:
    بازگشت CURL_IETF_PROTO_TLS1_1;
  حالت GNUTLS_TLS1_2:
    بازگشت CURL_IETF_PROTO_TLS1_2;
  حالت GNUTLS_TLS1_3:
    بازگشت CURL_IETF_PROTO_TLS1_3;
  پیشفرض:
    بازگشت CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ثابت نویسه *ssl_peer_کلید,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ثابت نویسه *alpn,
                                 unsigned نویسه *quic_tp,
                                 size_t quic_tp_len,
                                 ساختار Curl_ssl_session **psession)
{
  ساختار Curl_ssl_session *sc_session = پوچ, *sc_dup = پوچ;
  unsigned نویسه *sdata, *qtp_clone = پوچ;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  اگر(psession)
    *psession = پوچ;
  اگر(!Curl_ssl_scache_use(cf, data))
    بازگشت CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, پوچ, &sdata_len);
  اگر(!sdata_len) /* GnuTLS does this for some version combinations */
    بازگشت CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  اگر(!sdata)
    بازگشت CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  اگر(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ثابت نویسه *)quic_tp, quic_tp_len);
    اگر(!qtp_clone) {
      curlx_free(sdata);
      بازگشت CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  اگر(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  اگر(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_کلید, sc_session);
    /* took ownership of `sc_session` */
    sc_session = پوچ;
  }
  اگر(!result && psession) {
    *psession = sc_dup;
    sc_dup = پوچ;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  بازگشت result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
ایستا CURLcode cf_gtls_update_session_id(ساختار Curl_cfilter *cf,
                                          ساختار Curl_easy *data,
                                          gnutls_session_t session)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار Curl_ssl_session *scs = پوچ;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_کلید,
                                   session, 0, connssl->negotiated.alpn,
                                   پوچ, 0, &scs);
  اگر(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  بازگشت result;
}

ایستا صحیح gtls_handshake_cb(gnutls_session_t session, unsigned صحیح htype,
                             unsigned when, unsigned صحیح incoming,
                             ثابت gnutls_datum_t *پیام)
{
  ساختار Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (تهی)پیام;
  (تهی)incoming;
  اگر(when) { /* after message has been processed */
    ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
    اگر(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      انتخاب(htype) {
      حالت GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        بشکن;
      }
      پیشفرض:
        بشکن;
      }
    }
  }
  بازگشت 0;
}
#endif

ایستا CURLcode gtls_set_priority(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ساختار gtls_ctx *gtls,
                                  ثابت نویسه *priority)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار dynbuf buf;
  ثابت نویسه *err = پوچ;
  CURLcode result = CURLE_OK;
  صحیح rc;

  curlx_dyn_init(&buf, 4096);

  اگر(conn_config->cipher_list) {
    اگر((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      اگر(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        اگر(result)
          برو out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      اگر(result)
        برو out;
      priority = curlx_dyn_ptr(&buf);
    }
    وگرنه /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  اگر(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  بازگشت result;
}

ایستا CURLcode gtls_client_init(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار ssl_peer *peer,
                                 size_t earlydata_max,
                                 ساختار gtls_ctx *gtls)
{
  ساختار ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ساختار ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned صحیح init_flags;
  صحیح rc;
  ثابت نویسه *prioritylist;
  bool tls13support;
  CURLcode result;

  اگر(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  اگر(result)
    بازگشت result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  اگر(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  وگرنه اگر(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  اگر(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  اگر(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  اگر(peer->sni) {
    اگر(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  اگر(rc != GNUTLS_E_SUCCESS)
    بازگشت CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  اگر(config->version == CURL_SSLVERSION_TLSv1_3) {
    اگر(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  اگر(result)
    بازگشت result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  اگر(result)
    بازگشت result;

  اگر(config->clientcert) {
    اگر(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      اگر(result)
        بازگشت result;
    }
    اگر(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->کلید_passwd ? config->کلید_passwd : "");
      اگر(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        بازگشت CURLE_SSL_CONNECT_ERROR;
      }
    }
    وگرنه {
      ثابت unsigned صحیح supported_کلید_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_کلید_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->کلید ? config->کلید : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->کلید_passwd,
           supported_کلید_encryption_algorithms);
      اگر(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->کلید_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        بازگشت CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  اگر(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  اگر(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  پوچ, 0, پوچ);
    اگر(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
  }

  بازگشت CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ایستا صحیح keylog_callback(gnutls_session_t session, ثابت نویسه *label,
                           ثابت gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  اگر(crandom.size != 32) {
    بازگشت -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  بازگشت 0;
}

ایستا CURLcode gtls_on_session_reuse(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      ساختار alpn_spec *alpns,
                                      ساختار Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  بازگشت Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

ایستا CURLcode gtls_apply_session(
  ساختار gtls_ctx *gctx,
  ساختار Curl_cfilter *cf,
  ساختار Curl_easy *data,
  ساختار ssl_peer *peer,
  ساختار alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  ساختار Curl_ssl_session *scs,
  bool *pwas_setup)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  صحیح rc;

  اگر(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    اگر(result)
      برو out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    اگر(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    وگرنه {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      اگر(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        اگر(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          اگر(result)
            برو out;
        }
        اگر(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  بازگشت result;
}

CURLcode Curl_gtls_ctx_init(ساختار gtls_ctx *gctx,
                            ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            ساختار ssl_peer *peer,
                            ثابت ساختار alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            تهی *cb_user_data,
                            تهی *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار Curl_ssl_session *scs = پوچ;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_شمار = 0;
  bool gtls_session_setup = FALSE;
  ساختار alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  اگر((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    اگر(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = پوچ;
      اگر(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  اگر(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_کلید, &scs);
    اگر(result)
      برو out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  اگر(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    اگر(result)
      برو out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  اگر(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    اگر(result)
      برو out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  اگر(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  اگر(!gtls_alpns_شمار && alpns.شمار) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.شمار);
    برای(i = 0; i < alpns.شمار; ++i) {
      gtls_alpns[i].data = (unsigned نویسه *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned صحیح)strlen(alpns.entries[i]);
    }
    gtls_alpns_شمار = alpns.شمار;
  }

  اگر(gtls_alpns_شمار &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned صحیح)gtls_alpns_شمار,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_کلید, scs);
  بازگشت result;
}

ایستا CURLcode gtls_connect_step1(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  اگر(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    بازگشت CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, پوچ, پوچ, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#وگرنه
                              پوچ
#endif
                              );

  اگر(result)
    بازگشت result;

  اگر(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ساختار alpn_proto_buf proto;
    memset(&proto, 0, اندازه(proto));
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
  gnutls_transport_set_افزودن_function(backend->gtls.session, gtls_افزودن);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  بازگشت CURLE_OK;
}

ایستا CURLcode pkp_pin_peer_pubkey(ساختار Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    ثابت نویسه *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned نویسه *buff1 = پوچ;

  gnutls_pubkey_t کلید = پوچ;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  اگر(!pinnedpubkey)
    بازگشت CURLE_OK;

  اگر(!cert)
    بازگشت result;

  انجامبده {
    صحیح ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&کلید);
    اگر(ret < 0)
      بشکن; /* failed */

    ret = gnutls_pubkey_import_x509(کلید, cert, 0);
    اگر(ret < 0)
      بشکن; /* failed */

    ret = gnutls_pubkey_export(کلید, GNUTLS_X509_FMT_DER, پوچ, &len1);
    اگر(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      بشکن; /* failed */

    buff1 = curlx_malloc(len1);
    اگر(!buff1)
      بشکن; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(کلید, GNUTLS_X509_FMT_DER, buff1, &len2);
    اگر(ret < 0 || len1 != len2)
      بشکن; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } وقتی(0);

  اگر(کلید)
    gnutls_pubkey_deinit(کلید);

  curlx_safefree(buff1);

  بازگشت result;
}

تهی Curl_gtls_report_handshake(ساختار Curl_easy *data, ساختار gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  اگر(Curl_trc_is_verbose(data)) {
    ثابت نویسه *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#وگرنه
  (تهی)data;
  (تهی)gctx;
#endif
}

ایستا تهی gtls_پیام_verify_result(ساختار Curl_easy *data,
                                   ساختار ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  نویسه certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = اندازه(certname);
  صحیح rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  اگر(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  اگر(!was_verified) {
    اگر(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    وگرنه
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  وگرنه
    infof(data, "  common name: %s (matched)", certname);
}

ایستا تهی gtls_infof_cert(ساختار Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  اگر(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    صحیح rc, algo;
    time_t tstamp;
    unsigned صحیح bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    اگر(rc)
      infof(data, "Failed to get certificate name");
    وگرنه {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    اگر(rc)
      infof(data, "Failed to get certificate issuer");
    وگرنه {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#وگرنه
  (تهی)data;
  (تهی)x509_cert;
#endif
}

ایستا CURLcode gtls_verify_ocsp_status(ساختار Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = پوچ;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  صحیح rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  اگر(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    برو out;
  }
  وگرنه اگر(rc < 0) {
    failf(data, "Invalid OCSP response received");
    برو out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  اگر(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    برو out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  اگر(rc < 0) {
    failf(data, "failed to parse OCSP response");
    برو out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  اگر(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    برو out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  برای(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    اگر(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      بشکن;
  }
  اگر(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    برو out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, پوچ, پوچ, پوچ, پوچ,
                                   &status, پوچ, پوچ, پوچ, پوچ);
  اگر(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    برو out;
  }
  انتخاب(status) {
  حالت GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    بشکن;
  حالت GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    بشکن;
  پیشفرض:
    failf(data, "peer certificate OCSP status is not good: %d", (صحیح)status);
    بشکن;
  }

out:
  اگر(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  بازگشت result;
}

ساختار gtls_cert_chain {
  ثابت gnutls_datum_t *certs;
  unsigned صحیح رقم_certs;
};

#ifdef USE_APPLE_SECTRUST
ایستا CURLcode gtls_chain_get_der(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   تهی *user_data,
                                   size_t i,
                                   unsigned نویسه **pder,
                                   size_t *pder_len)
{
  ساختار gtls_cert_chain *chain = user_data;

  (تهی)cf;
  (تهی)data;
  *pder_len = 0;
  *pder = پوچ;

  اگر(i >= chain->رقم_certs)
    بازگشت CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  بازگشت CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ایستا CURLcode gtls_verify_cert(ساختار Curl_easy *data,
                                 ساختار ssl_filter_config *config,
                                 ساختار ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 ساختار Curl_cfilter *cf,
                                 ساختار ssl_peer *peer,
                                 ساختار gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned صحیح verify_status = 0;
  long * ثابت certverifyresult = &ssl_config->certverifyresult;
  صحیح rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  اگر(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  اگر(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  اگر(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    اگر(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = پوچ;
      ocsp_raw.size = 0;
    }
    اگر(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      بازگشت CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->رقم_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    اگر(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      بازگشت result; /* no further failure analysis needed */
    verified = !result;
    اگر(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#وگرنه
  (تهی)cf;
  (تهی)peer;
  (تهی)chain;
#endif

  اگر(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ثابت نویسه *cause = "certificate error, no details available";
    اگر(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    وگرنه اگر(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    وگرنه اگر(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    وگرنه اگر(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    بازگشت result;
  }
  بازگشت CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                gnutls_session_t session,
                                ساختار ssl_filter_config *config,
                                ساختار ssl_easy_config *ssl_config,
                                ساختار ssl_peer *peer,
                                ثابت نویسه *pinned_کلید)
{
  ساختار gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = پوچ, x509_issuer = پوچ;
  time_t certclock;
  bool ocsp_checked = FALSE;
  صحیح rc;
  CURLcode result = CURLE_OK;
  long * ثابت certverifyresult = &ssl_config->certverifyresult;

  (تهی)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.رقم_certs);
  اگر(!chain.certs) {
    اگر(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      برو out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  اگر(data->set.ssl.certinfo && chain.certs) {
    اگر(chain.رقم_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.رقم_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      برو out;
    }
    وگرنه {
      unsigned صحیح i;

      result = Curl_ssl_init_certinfo(data, (صحیح)chain.رقم_certs);
      اگر(result)
        برو out;

      برای(i = 0; i < chain.رقم_certs; i++) {
        ثابت uint8_t *beg = chain.certs[i].data;
        ثابت uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (صحیح)i, beg, end);
        اگر(result)
          برو out;
      }
    }
  }

  اگر(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    اگر(result)
      برو out;
  }
  وگرنه
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  اگر(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    برو out;
  }

  اگر(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    اگر(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      برو out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  اگر(certclock == (time_t)-1) {
    اگر(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      برو out;
    }
    وگرنه
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  وگرنه {
    اگر(certclock < time(پوچ)) {
      اگر(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        برو out;
      }
      وگرنه
        infof(data, "  SSL certificate expiration date FAILED");
    }
    وگرنه
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  اگر(certclock == (time_t)-1) {
    اگر(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      برو out;
    }
    وگرنه
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  وگرنه {
    اگر(certclock > time(پوچ)) {
      اگر(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        برو out;
      }
      وگرنه
        infof(data, "  SSL certificate activation date FAILED");
    }
    وگرنه
      infof(data, "  SSL certificate activation date OK");
  }

  اگر(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    اگر(result)
      برو out;
  }
  وگرنه
    infof(data, "  SSL certificate status verification SKIPPED");

  اگر(config->issuercert) {
    gnutls_datum_t issuerp;
    اگر(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      برو out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    اگر(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      برو out;
    }
    rc = (صحیح)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    اگر(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      برو out;
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
  rc = (صحیح)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_پیام_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  اگر(result)
    برو out;

  اگر(pinned_کلید) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_کلید);
    اگر(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      برو out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  اگر(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  اگر(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  بازگشت result;
}

ایستا CURLcode gtls_verifyserver(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  gnutls_session_t session)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  ثابت نویسه *pinned_کلید = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#وگرنه
  ثابت نویسه *pinned_کلید = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_کلید);
  اگر(result)
    برو out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  اگر(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  بازگشت result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ایستا CURLcode gtls_send_earlydata(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ثابت unsigned نویسه *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  وقتی(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    اگر(n < 0) {
      اگر(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      وگرنه
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      برو out;
    }
    وگرنه اگر(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  بازگشت result;
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
ایستا CURLcode gtls_connect_common(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    bool *done)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  اگر(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  اگر(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    اگر(result)
      برو out;
    connssl->connecting_state = ssl_connect_2;
  }

  اگر(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    اگر(connssl->earlydata_state == ssl_earlydata_await) {
      برو out;
    }
    وگرنه اگر(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      اگر(result)
        برو out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    اگر(result)
      برو out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  اگر(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    صحیح rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    اگر(result)
      برو out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    اگر(rc) {  /* No ALPN from server */
      proto.data = پوچ;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    اگر(result)
      برو out;

#ifdef CURL_GNUTLS_EARLY_DATA
    اگر(connssl->earlydata_state > ssl_earlydata_none) {
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

  اگر(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  اگر(result == CURLE_AGAIN) {
    *done = FALSE;
    بازگشت CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (صحیح)result,
              *done);
  بازگشت result;
}

ایستا CURLcode gtls_connect(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  ساختار ssl_connect_data *connssl = cf->ctx;
  اگر((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    بازگشت CURLE_OK;
  }
#endif
  بازگشت gtls_connect_common(cf, data, done);
}

ایستا bool gtls_data_pending(ساختار Curl_cfilter *cf,
                              ثابت ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *ctx = cf->ctx;
  ساختار gtls_ssl_backend_data *backend;

  (تهی)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ساختار gtls_ssl_backend_data *)ctx->backend;
  اگر(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    بازگشت TRUE;
  بازگشت FALSE;
}

ایستا CURLcode gtls_send(ساختار Curl_cfilter *cf,
                          ساختار Curl_easy *data,
                          ثابت تهی *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  وقتی(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    اگر(nwritten >= 0) {
      اگر((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        بازگشت CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (نویسه *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    وگرنه {
      اگر(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        برو out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      برو out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (صحیح)result, *pnwritten);
  بازگشت result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ایستا CURLcode gtls_shutdown(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  نویسه buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  اگر(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    برو out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  اگر(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    اگر(send_shutdown) {
      صحیح ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      اگر((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        برو out;
      }
      اگر(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((صحیح)ret), (صحیح)ret);
        result = CURLE_RECV_ERROR;
        برو out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  برای(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, اندازه(buf));
    اگر(nread <= 0)
      بشکن;
  }
  اگر(nread > 0) {
    /* still data coming in? */
  }
  وگرنه اگر(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  وگرنه اگر((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  وگرنه {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((صحیح)nread), (صحیح)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  بازگشت result;
}

ایستا تهی gtls_close(ساختار Curl_cfilter *cf,
                       ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  اگر(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = پوچ;
  }
  اگر(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ایستا CURLcode gtls_recv(ساختار Curl_cfilter *cf,
                          ساختار Curl_easy *data,
                          نویسه *buf, size_t blen,
                          size_t *pnread)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  اگر(nread >= 0)
    *pnread = (size_t)nread;
  وگرنه {
    اگر((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      برو out;
    }
    وگرنه اگر(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      اگر(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      برو out;
    }
    وگرنه {
      failf(data, "GnuTLS recv error (%d): %s",
            (صحیح)nread, gnutls_strerror((صحیح)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      برو out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (صحیح)result, nread);
  بازگشت result;
}

size_t Curl_gtls_version(نویسه *buffer, size_t size)
{
  بازگشت curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(پوچ));
}

/* data might be NULL! */
ایستا CURLcode gtls_random(ساختار Curl_easy *data,
                            unsigned نویسه *entropy, size_t length)
{
  صحیح rc;
  (تهی)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  بازگشت rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ایستا CURLcode gtls_sha256sum(ثابت unsigned نویسه *درون_داد,
                               size_t len,
                               unsigned نویسه *sha256sum, /* output */
                               size_t sha256len)
{
  ساختار sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  انجامبده {
    unsigned صحیح ilen = (unsigned صحیح)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, درون_داد);
    len -= ilen;
    درون_داد += ilen;
  } وقتی(len);
#اگر NETTLE_VERSION_MAJOR >= 4
  (تهی)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#وگرنه
  sha256_digest(&SHA256pw, (unsigned صحیح)sha256len, sha256sum);
#endif
  بازگشت CURLE_OK;
}

ایستا bool gtls_cert_status_request(تهی)
{
  بازگشت TRUE;
}

ایستا تهی *gtls_get_internals(ساختار ssl_connect_data *connssl,
                                CURLINFO info)
{
  ساختار gtls_ssl_backend_data *backend =
    (ساختار gtls_ssl_backend_data *)connssl->backend;
  (تهی)info;
  DEBUGASSERT(backend);
  بازگشت backend->gtls.session;
}

ثابت ساختار Curl_ssl Curl_ssl_gnutls = {
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

  اندازه(ساختار gtls_ssl_backend_data),

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
  پوچ,                          /* close_all */
  پوچ,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  پوچ,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
