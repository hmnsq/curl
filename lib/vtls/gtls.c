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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_GNUTLS

#अन्तर्भाव <gnutls/abstract.h>
#अन्तर्भाव <gnutls/gnutls.h>
#अन्तर्भाव <gnutls/x509.h>
#अन्तर्भाव <gnutls/crypto.h>
#अन्तर्भाव <nettle/sha2.h>
#अन्तर्भाव <nettle/version.h>

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vtls/keylog.h"
#अन्तर्भाव "vtls/gtls.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_int.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vtls/apple.h"
#अन्तर्भाव "vauth/vauth.h"
#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "connect.h" /* for the connect timeout */
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#जइ 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
स्थिर शून्य tls_log_func(पूर्णाङ्क level, ध्रुव वर्ण *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#जइ !defined(GNUTLS_VERSION_सङ्ख्या) || (GNUTLS_VERSION_सङ्ख्या < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#जइ GNUTLS_VERSION_सङ्ख्या >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#अन्तर्भाव <gnutls/ocsp.h>

संरचना gtls_ssl_backend_data {
  संरचना gtls_ctx gtls;
};

स्थिर ssize_t gtls_प्रक्षिप(शून्य *s, ध्रुव शून्य *buf, size_t blen)
{
  संरचना Curl_cfilter *cf = s;
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (पूर्णाङ्क)result, nwritten);
  backend->gtls.io_result = result;
  जइ(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    प्रत्यागम -1;
  }
  प्रत्यागम (ssize_t)nwritten;
}

स्थिर ssize_t gtls_pull(शून्य *s, शून्य *buf, size_t blen)
{
  संरचना Curl_cfilter *cf = s;
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  जइ(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    जइ(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      प्रत्यागम -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (पूर्णाङ्क)result,
              nread);
  backend->gtls.io_result = result;
  जइ(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    प्रत्यागम -1;
  }
  अण्णहा जइ(nread == 0)
    connssl->peer_closed = TRUE;
  प्रत्यागम (ssize_t)nread;
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
स्थिर पूर्णाङ्क gtls_init(शून्य)
{
  पूर्णाङ्क ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  प्रत्यागम ret;
}

स्थिर शून्य gtls_cleanup(शून्य)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
स्थिर शून्य showtime(संरचना Curl_easy *data, ध्रुव वर्ण *text, time_t stamp)
{
  संरचना tm buffer;
  ध्रुव संरचना tm *tm = &buffer;
  वर्ण str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  जइ(result)
    प्रत्यागम;

  curl_msnprintf(str,
                 परिमाण(str),
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

स्थिर gnutls_datum_t load_file(ध्रुव वर्ण *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { रिक्त, 0 };
  long filelen;
  शून्य *ptr;

  f = curlx_fopen(file, "rb");
  जइ(!f)
    प्रत्यागम loaded_file;
  जइ(fseek(f, 0, SEEK_END))
    गच्छ out;
  filelen = ftell(f);
  जइ(filelen < 0 || filelen > CURL_MAX_निवेशः_LENGTH)
    गच्छ out;
  जइ(fseek(f, 0, SEEK_SET))
    गच्छ out;
  ptr = curlx_malloc((size_t)filelen);
  जइ(!ptr)
    गच्छ out;
  जइ(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    गच्छ out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned पूर्णाङ्क)filelen;
out:
  curlx_fclose(f);
  प्रत्यागम loaded_file;
}

स्थिर शून्य unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
स्थिर CURLcode cf_gtls_handshake(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  पूर्णाङ्क rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  जइ(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    जइ(result)
      प्रत्यागम result;
  }

  जइ((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    प्रत्यागम CURLE_AGAIN;
  }
  अण्णहा जइ((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    ध्रुव वर्ण *strerr = रिक्त;

    जइ(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    जइ(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    प्रत्यागम CURLE_AGAIN;
  }
  अण्णहा जइ((rc < 0) && backend->gtls.io_result) {
    प्रत्यागम backend->gtls.io_result;
  }
  अण्णहा जइ(rc < 0) {
    ध्रुव वर्ण *strerr = रिक्त;

    जइ(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    जइ(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर gnutls_x509_crt_fmt_t gnutls_do_file_type(ध्रुव वर्ण *type)
{
  जइ(!type || !type[0])
    प्रत्यागम GNUTLS_X509_FMT_PEM;
  जइ(curl_strequal(type, "PEM"))
    प्रत्यागम GNUTLS_X509_FMT_PEM;
  जइ(curl_strequal(type, "DER"))
    प्रत्यागम GNUTLS_X509_FMT_DER;
  प्रत्यागम GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

स्थिर CURLcode gnutls_set_ssl_version_min_max(
  संरचना Curl_easy *data,
  संरचना ssl_peer *peer,
  संरचना ssl_filter_config *conn_config,
  ध्रुव वर्ण **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  जइ(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  जइ((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  जइ(peer->transport == TRNSPRT_QUIC) {
    जइ(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    प्रत्यागम CURLE_OK;
  }

  विकल्प(ssl_version | ssl_version_max) {
  पक्ष CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    प्रत्यागम CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    प्रत्यागम CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    प्रत्यागम CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    प्रत्यागम CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    प्रत्यागम CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    प्रत्यागम CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    प्रत्यागम CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    प्रत्यागम CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    प्रत्यागम CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    प्रत्यागम CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  प्रत्यागम CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(संरचना Curl_easy *data,
                                       संरचना gtls_shared_creds **pcreds)
{
  संरचना gtls_shared_creds *shared;
  पूर्णाङ्क rc;

  *pcreds = रिक्त;
  shared = curlx_calloc(1, परिमाण(*shared));
  जइ(!shared)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  जइ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(संरचना gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  जइ(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    प्रत्यागम CURLE_OK;
  }
  प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
}

शून्य Curl_gtls_shared_creds_free(संरचना gtls_shared_creds **pcreds)
{
  संरचना gtls_shared_creds *shared = *pcreds;
  *pcreds = रिक्त;
  जइ(shared) {
    --shared->refcount;
    जइ(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

स्थिर CURLcode gtls_populate_creds(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  संरचना ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  पूर्णाङ्क rc;

  जइ(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    प्रत्यागम CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  जइ(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#अण्णहा
    rc = gnutls_certificate_set_x509_system_trust(creds);
    जइ(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    अण्णहा {
      infof(data, "  Native: %d certificates from system trust", rc);
      जइ(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  जइ(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    जइ(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      प्रत्यागम CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned पूर्णाङ्क)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    जइ(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      जइ(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        प्रत्यागम CURLE_SSL_CACERT_BADFILE;
      }
    }
    अण्णहा
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  अण्णहा जइ(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    जइ(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      जइ(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        प्रत्यागम CURLE_SSL_CACERT_BADFILE;
      }
    }
    अण्णहा
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  जइ(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    जइ(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      जइ(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        प्रत्यागम CURLE_SSL_CACERT_BADFILE;
      }
    }
    अण्णहा
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  जइ(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  जइ(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    प्रत्यागम CURLE_NOT_BUILT_IN;
  }
#endif
  जइ(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    जइ(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      प्रत्यागम CURLE_SSL_CRL_BADFILE;
    }
    अण्णहा
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  प्रत्यागम CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_कुञ्चिका   "tls:gtls:x509:share"

स्थिर bool gtls_shared_creds_expired(संरचना Curl_easy *data,
                                      ध्रुव संरचना gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  जइ(timeout_ms < 0)
    प्रत्यागम FALSE;

  प्रत्यागम elapsed_ms >= timeout_ms;
}

स्थिर bool gtls_shared_creds_different(संरचना Curl_cfilter *cf,
                                        ध्रुव संरचना gtls_shared_creds *sc)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  जइ(!sc->CAfile || !conn_config->CAfile)
    प्रत्यागम sc->CAfile != conn_config->CAfile;

  प्रत्यागम strcmp(sc->CAfile, conn_config->CAfile);
}

स्थिर संरचना gtls_shared_creds *gtls_get_cached_creds(संरचना Curl_cfilter *cf,
                                                       संरचना Curl_easy *data)
{
  संरचना gtls_shared_creds *shared_creds;

  जइ(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_कुञ्चिका,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_कुञ्चिका));
    जइ(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      प्रत्यागम shared_creds;
    }
  }
  प्रत्यागम रिक्त;
}

स्थिर शून्य gtls_shared_creds_hash_free(ध्रुव शून्य *कुञ्चिका,
                                        size_t कुञ्चिका_len, शून्य *p)
{
  संरचना gtls_shared_creds *sc = p;
  DEBUGASSERT(कुञ्चिका_len == CURL_CSTRLEN(MPROTO_GTLS_X509_कुञ्चिका));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_कुञ्चिका, कुञ्चिका, कुञ्चिका_len));
  (शून्य)कुञ्चिका;
  (शून्य)कुञ्चिका_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

स्थिर शून्य gtls_set_cached_creds(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना gtls_shared_creds *sc)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  जइ(!data->multi)
    प्रत्यागम;

  जइ(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    जइ(!sc->CAfile)
      प्रत्यागम;
  }

  जइ(Curl_gtls_shared_creds_up_ref(sc))
    प्रत्यागम;

  जइ(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_कुञ्चिका,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_कुञ्चिका),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    प्रत्यागम;
  }
}

CURLcode Curl_gtls_client_trust_setup(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      संरचना gtls_ctx *gtls)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना gtls_shared_creds *cached_creds = रिक्त;
  bool cache_criteria_met;
  CURLcode result;
  पूर्णाङ्क rc;

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

  जइ(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  जइ(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    जइ(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }
  अण्णहा {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    जइ(result)
      प्रत्यागम result;
    gtls->shared_creds->trust_setup = TRUE;
    जइ(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  प्रत्यागम CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
स्थिर पूर्णाङ्क gtls_get_ietf_proto(gnutls_session_t session)
{
  विकल्प(gnutls_protocol_get_version(session)) {
  पक्ष GNUTLS_SSL3:
    प्रत्यागम CURL_IETF_PROTO_SSL3;
  पक्ष GNUTLS_TLS1_0:
    प्रत्यागम CURL_IETF_PROTO_TLS1;
  पक्ष GNUTLS_TLS1_1:
    प्रत्यागम CURL_IETF_PROTO_TLS1_1;
  पक्ष GNUTLS_TLS1_2:
    प्रत्यागम CURL_IETF_PROTO_TLS1_2;
  पक्ष GNUTLS_TLS1_3:
    प्रत्यागम CURL_IETF_PROTO_TLS1_3;
  स्वभाव:
    प्रत्यागम CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ध्रुव वर्ण *alpn,
                                 unsigned वर्ण *quic_tp,
                                 size_t quic_tp_len,
                                 संरचना Curl_ssl_session **psession)
{
  संरचना Curl_ssl_session *sc_session = रिक्त, *sc_dup = रिक्त;
  unsigned वर्ण *sdata, *qtp_clone = रिक्त;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  जइ(psession)
    *psession = रिक्त;
  जइ(!Curl_ssl_scache_use(cf, data))
    प्रत्यागम CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, रिक्त, &sdata_len);
  जइ(!sdata_len) /* GnuTLS does this for some version combinations */
    प्रत्यागम CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  जइ(!sdata)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  जइ(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ध्रुव वर्ण *)quic_tp, quic_tp_len);
    जइ(!qtp_clone) {
      curlx_free(sdata);
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  जइ(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  जइ(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_कुञ्चिका, sc_session);
    /* took ownership of `sc_session` */
    sc_session = रिक्त;
  }
  जइ(!result && psession) {
    *psession = sc_dup;
    sc_dup = रिक्त;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  प्रत्यागम result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
स्थिर CURLcode cf_gtls_update_session_id(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *data,
                                          gnutls_session_t session)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना Curl_ssl_session *scs = रिक्त;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_कुञ्चिका,
                                   session, 0, connssl->negotiated.alpn,
                                   रिक्त, 0, &scs);
  जइ(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  प्रत्यागम result;
}

स्थिर पूर्णाङ्क gtls_handshake_cb(gnutls_session_t session, unsigned पूर्णाङ्क htype,
                             unsigned when, unsigned पूर्णाङ्क incoming,
                             ध्रुव gnutls_datum_t *सन्देशः)
{
  संरचना Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (शून्य)सन्देशः;
  (शून्य)incoming;
  जइ(when) { /* after message has been processed */
    संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
    जइ(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      विकल्प(htype) {
      पक्ष GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        भङ्ग;
      }
      स्वभाव:
        भङ्ग;
      }
    }
  }
  प्रत्यागम 0;
}
#endif

स्थिर CURLcode gtls_set_priority(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना gtls_ctx *gtls,
                                  ध्रुव वर्ण *priority)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना dynbuf buf;
  ध्रुव वर्ण *err = रिक्त;
  CURLcode result = CURLE_OK;
  पूर्णाङ्क rc;

  curlx_dyn_init(&buf, 4096);

  जइ(conn_config->cipher_list) {
    जइ((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      जइ(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        जइ(result)
          गच्छ out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      जइ(result)
        गच्छ out;
      priority = curlx_dyn_ptr(&buf);
    }
    अण्णहा /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  जइ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  प्रत्यागम result;
}

स्थिर CURLcode gtls_client_init(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना ssl_peer *peer,
                                 size_t earlydata_max,
                                 संरचना gtls_ctx *gtls)
{
  संरचना ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned पूर्णाङ्क init_flags;
  पूर्णाङ्क rc;
  ध्रुव वर्ण *prioritylist;
  bool tls13support;
  CURLcode result;

  जइ(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  जइ(result)
    प्रत्यागम result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  जइ(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  अण्णहा जइ(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  जइ(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  जइ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  जइ(peer->sni) {
    जइ(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  जइ(rc != GNUTLS_E_SUCCESS)
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  जइ(config->version == CURL_SSLVERSION_TLSv1_3) {
    जइ(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  जइ(result)
    प्रत्यागम result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  जइ(result)
    प्रत्यागम result;

  जइ(config->clientcert) {
    जइ(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      जइ(result)
        प्रत्यागम result;
    }
    जइ(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->कुञ्चिका_passwd ? config->कुञ्चिका_passwd : "");
      जइ(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
    }
    अण्णहा {
      ध्रुव unsigned पूर्णाङ्क supported_कुञ्चिका_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_कुञ्चिका_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->कुञ्चिका ? config->कुञ्चिका : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->कुञ्चिका_passwd,
           supported_कुञ्चिका_encryption_algorithms);
      जइ(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->कुञ्चिका_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  जइ(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  जइ(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  रिक्त, 0, रिक्त);
    जइ(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

  प्रत्यागम CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
स्थिर पूर्णाङ्क keylog_callback(gnutls_session_t session, ध्रुव वर्ण *label,
                           ध्रुव gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  जइ(crandom.size != 32) {
    प्रत्यागम -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  प्रत्यागम 0;
}

स्थिर CURLcode gtls_on_session_reuse(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      संरचना alpn_spec *alpns,
                                      संरचना Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  प्रत्यागम Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

स्थिर CURLcode gtls_apply_session(
  संरचना gtls_ctx *gctx,
  संरचना Curl_cfilter *cf,
  संरचना Curl_easy *data,
  संरचना ssl_peer *peer,
  संरचना alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  संरचना Curl_ssl_session *scs,
  bool *pwas_setup)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  पूर्णाङ्क rc;

  जइ(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    जइ(result)
      गच्छ out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    जइ(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    अण्णहा {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      जइ(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        जइ(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          जइ(result)
            गच्छ out;
        }
        जइ(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  प्रत्यागम result;
}

CURLcode Curl_gtls_ctx_init(संरचना gtls_ctx *gctx,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            संरचना ssl_peer *peer,
                            ध्रुव संरचना alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            शून्य *cb_user_data,
                            शून्य *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_session *scs = रिक्त;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_गणनम् = 0;
  bool gtls_session_setup = FALSE;
  संरचना alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  जइ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    जइ(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = रिक्त;
      जइ(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  जइ(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_कुञ्चिका, &scs);
    जइ(result)
      गच्छ out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  जइ(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    जइ(result)
      गच्छ out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  जइ(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    जइ(result)
      गच्छ out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  जइ(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  जइ(!gtls_alpns_गणनम् && alpns.गणनम्) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.गणनम्);
    प्रति(i = 0; i < alpns.गणनम्; ++i) {
      gtls_alpns[i].data = (unsigned वर्ण *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned पूर्णाङ्क)strlen(alpns.entries[i]);
    }
    gtls_alpns_गणनम् = alpns.गणनम्;
  }

  जइ(gtls_alpns_गणनम् &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned पूर्णाङ्क)gtls_alpns_गणनम्,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_कुञ्चिका, scs);
  प्रत्यागम result;
}

स्थिर CURLcode gtls_connect_step1(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  जइ(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    प्रत्यागम CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, रिक्त, रिक्त, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#अण्णहा
                              रिक्त
#endif
                              );

  जइ(result)
    प्रत्यागम result;

  जइ(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    संरचना alpn_proto_buf proto;
    memset(&proto, 0, परिमाण(proto));
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
  gnutls_transport_set_प्रक्षिप_function(backend->gtls.session, gtls_प्रक्षिप);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode pkp_pin_peer_pubkey(संरचना Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    ध्रुव वर्ण *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned वर्ण *buff1 = रिक्त;

  gnutls_pubkey_t कुञ्चिका = रिक्त;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  जइ(!pinnedpubkey)
    प्रत्यागम CURLE_OK;

  जइ(!cert)
    प्रत्यागम result;

  करेहि {
    पूर्णाङ्क ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&कुञ्चिका);
    जइ(ret < 0)
      भङ्ग; /* failed */

    ret = gnutls_pubkey_import_x509(कुञ्चिका, cert, 0);
    जइ(ret < 0)
      भङ्ग; /* failed */

    ret = gnutls_pubkey_export(कुञ्चिका, GNUTLS_X509_FMT_DER, रिक्त, &len1);
    जइ(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      भङ्ग; /* failed */

    buff1 = curlx_malloc(len1);
    जइ(!buff1)
      भङ्ग; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(कुञ्चिका, GNUTLS_X509_FMT_DER, buff1, &len2);
    जइ(ret < 0 || len1 != len2)
      भङ्ग; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } जाव(0);

  जइ(कुञ्चिका)
    gnutls_pubkey_deinit(कुञ्चिका);

  curlx_safefree(buff1);

  प्रत्यागम result;
}

शून्य Curl_gtls_report_handshake(संरचना Curl_easy *data, संरचना gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  जइ(Curl_trc_is_verbose(data)) {
    ध्रुव वर्ण *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#अण्णहा
  (शून्य)data;
  (शून्य)gctx;
#endif
}

स्थिर शून्य gtls_सन्देशः_verify_result(संरचना Curl_easy *data,
                                   संरचना ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  वर्ण certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = परिमाण(certname);
  पूर्णाङ्क rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  जइ(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  जइ(!was_verified) {
    जइ(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    अण्णहा
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  अण्णहा
    infof(data, "  common name: %s (matched)", certname);
}

स्थिर शून्य gtls_infof_cert(संरचना Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  जइ(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    पूर्णाङ्क rc, algo;
    time_t tstamp;
    unsigned पूर्णाङ्क bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    जइ(rc)
      infof(data, "Failed to get certificate name");
    अण्णहा {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    जइ(rc)
      infof(data, "Failed to get certificate issuer");
    अण्णहा {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#अण्णहा
  (शून्य)data;
  (शून्य)x509_cert;
#endif
}

स्थिर CURLcode gtls_verify_ocsp_status(संरचना Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = रिक्त;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  पूर्णाङ्क rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  जइ(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    गच्छ out;
  }
  अण्णहा जइ(rc < 0) {
    failf(data, "Invalid OCSP response received");
    गच्छ out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  जइ(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    गच्छ out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  जइ(rc < 0) {
    failf(data, "failed to parse OCSP response");
    गच्छ out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  जइ(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    गच्छ out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  प्रति(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    जइ(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      भङ्ग;
  }
  जइ(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    गच्छ out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, रिक्त, रिक्त, रिक्त, रिक्त,
                                   &status, रिक्त, रिक्त, रिक्त, रिक्त);
  जइ(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    गच्छ out;
  }
  विकल्प(status) {
  पक्ष GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    भङ्ग;
  पक्ष GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    भङ्ग;
  स्वभाव:
    failf(data, "peer certificate OCSP status is not good: %d", (पूर्णाङ्क)status);
    भङ्ग;
  }

out:
  जइ(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  प्रत्यागम result;
}

संरचना gtls_cert_chain {
  ध्रुव gnutls_datum_t *certs;
  unsigned पूर्णाङ्क अङ्कः_certs;
};

#ifdef USE_APPLE_SECTRUST
स्थिर CURLcode gtls_chain_get_der(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   शून्य *user_data,
                                   size_t i,
                                   unsigned वर्ण **pder,
                                   size_t *pder_len)
{
  संरचना gtls_cert_chain *chain = user_data;

  (शून्य)cf;
  (शून्य)data;
  *pder_len = 0;
  *pder = रिक्त;

  जइ(i >= chain->अङ्कः_certs)
    प्रत्यागम CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  प्रत्यागम CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
स्थिर CURLcode gtls_verify_cert(संरचना Curl_easy *data,
                                 संरचना ssl_filter_config *config,
                                 संरचना ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 संरचना Curl_cfilter *cf,
                                 संरचना ssl_peer *peer,
                                 संरचना gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned पूर्णाङ्क verify_status = 0;
  long * ध्रुव certverifyresult = &ssl_config->certverifyresult;
  पूर्णाङ्क rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  जइ(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  जइ(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  जइ(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    जइ(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = रिक्त;
      ocsp_raw.size = 0;
    }
    जइ(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      प्रत्यागम CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->अङ्कः_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    जइ(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      प्रत्यागम result; /* no further failure analysis needed */
    verified = !result;
    जइ(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#अण्णहा
  (शून्य)cf;
  (शून्य)peer;
  (शून्य)chain;
#endif

  जइ(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ध्रुव वर्ण *cause = "certificate error, no details available";
    जइ(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    अण्णहा जइ(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    अण्णहा जइ(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    अण्णहा जइ(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    प्रत्यागम result;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                gnutls_session_t session,
                                संरचना ssl_filter_config *config,
                                संरचना ssl_easy_config *ssl_config,
                                संरचना ssl_peer *peer,
                                ध्रुव वर्ण *pinned_कुञ्चिका)
{
  संरचना gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = रिक्त, x509_issuer = रिक्त;
  time_t certclock;
  bool ocsp_checked = FALSE;
  पूर्णाङ्क rc;
  CURLcode result = CURLE_OK;
  long * ध्रुव certverifyresult = &ssl_config->certverifyresult;

  (शून्य)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.अङ्कः_certs);
  जइ(!chain.certs) {
    जइ(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      गच्छ out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  जइ(data->set.ssl.certinfo && chain.certs) {
    जइ(chain.अङ्कः_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.अङ्कः_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    अण्णहा {
      unsigned पूर्णाङ्क i;

      result = Curl_ssl_init_certinfo(data, (पूर्णाङ्क)chain.अङ्कः_certs);
      जइ(result)
        गच्छ out;

      प्रति(i = 0; i < chain.अङ्कः_certs; i++) {
        ध्रुव uint8_t *beg = chain.certs[i].data;
        ध्रुव uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (पूर्णाङ्क)i, beg, end);
        जइ(result)
          गच्छ out;
      }
    }
  }

  जइ(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    जइ(result)
      गच्छ out;
  }
  अण्णहा
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  जइ(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    गच्छ out;
  }

  जइ(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    जइ(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  जइ(certclock == (time_t)-1) {
    जइ(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    अण्णहा
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  अण्णहा {
    जइ(certclock < time(रिक्त)) {
      जइ(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        गच्छ out;
      }
      अण्णहा
        infof(data, "  SSL certificate expiration date FAILED");
    }
    अण्णहा
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  जइ(certclock == (time_t)-1) {
    जइ(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    अण्णहा
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  अण्णहा {
    जइ(certclock > time(रिक्त)) {
      जइ(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        गच्छ out;
      }
      अण्णहा
        infof(data, "  SSL certificate activation date FAILED");
    }
    अण्णहा
      infof(data, "  SSL certificate activation date OK");
  }

  जइ(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    जइ(result)
      गच्छ out;
  }
  अण्णहा
    infof(data, "  SSL certificate status verification SKIPPED");

  जइ(config->issuercert) {
    gnutls_datum_t issuerp;
    जइ(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      गच्छ out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    जइ(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      गच्छ out;
    }
    rc = (पूर्णाङ्क)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    जइ(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      गच्छ out;
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
  rc = (पूर्णाङ्क)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_सन्देशः_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  जइ(result)
    गच्छ out;

  जइ(pinned_कुञ्चिका) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_कुञ्चिका);
    जइ(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      गच्छ out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  जइ(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  जइ(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  प्रत्यागम result;
}

स्थिर CURLcode gtls_verifyserver(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  gnutls_session_t session)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  ध्रुव वर्ण *pinned_कुञ्चिका = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#अण्णहा
  ध्रुव वर्ण *pinned_कुञ्चिका = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_कुञ्चिका);
  जइ(result)
    गच्छ out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  जइ(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  प्रत्यागम result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
स्थिर CURLcode gtls_send_earlydata(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ध्रुव unsigned वर्ण *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  जाव(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    जइ(n < 0) {
      जइ(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      अण्णहा
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      गच्छ out;
    }
    अण्णहा जइ(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  प्रत्यागम result;
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
स्थिर CURLcode gtls_connect_common(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  जइ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  जइ(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    जइ(result)
      गच्छ out;
    connssl->connecting_state = ssl_connect_2;
  }

  जइ(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    जइ(connssl->earlydata_state == ssl_earlydata_await) {
      गच्छ out;
    }
    अण्णहा जइ(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      जइ(result)
        गच्छ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    जइ(result)
      गच्छ out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  जइ(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    पूर्णाङ्क rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    जइ(result)
      गच्छ out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    जइ(rc) {  /* No ALPN from server */
      proto.data = रिक्त;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    जइ(result)
      गच्छ out;

#ifdef CURL_GNUTLS_EARLY_DATA
    जइ(connssl->earlydata_state > ssl_earlydata_none) {
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

  जइ(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  जइ(result == CURLE_AGAIN) {
    *done = FALSE;
    प्रत्यागम CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (पूर्णाङ्क)result,
              *done);
  प्रत्यागम result;
}

स्थिर CURLcode gtls_connect(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  संरचना ssl_connect_data *connssl = cf->ctx;
  जइ((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }
#endif
  प्रत्यागम gtls_connect_common(cf, data, done);
}

स्थिर bool gtls_data_pending(संरचना Curl_cfilter *cf,
                              ध्रुव संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *ctx = cf->ctx;
  संरचना gtls_ssl_backend_data *backend;

  (शून्य)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (संरचना gtls_ssl_backend_data *)ctx->backend;
  जइ(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

स्थिर CURLcode gtls_send(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *data,
                          ध्रुव शून्य *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  जाव(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    जइ(nwritten >= 0) {
      जइ((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        प्रत्यागम CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (वर्ण *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    अण्णहा {
      जइ(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        गच्छ out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      गच्छ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (पूर्णाङ्क)result, *pnwritten);
  प्रत्यागम result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
स्थिर CURLcode gtls_shutdown(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  वर्ण buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  जइ(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    गच्छ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  जइ(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    जइ(send_shutdown) {
      पूर्णाङ्क ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      जइ((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        गच्छ out;
      }
      जइ(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((पूर्णाङ्क)ret), (पूर्णाङ्क)ret);
        result = CURLE_RECV_ERROR;
        गच्छ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  प्रति(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, परिमाण(buf));
    जइ(nread <= 0)
      भङ्ग;
  }
  जइ(nread > 0) {
    /* still data coming in? */
  }
  अण्णहा जइ(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  अण्णहा जइ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  अण्णहा {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((पूर्णाङ्क)nread), (पूर्णाङ्क)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  प्रत्यागम result;
}

स्थिर शून्य gtls_close(संरचना Curl_cfilter *cf,
                       संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  जइ(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = रिक्त;
  }
  जइ(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

स्थिर CURLcode gtls_recv(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *data,
                          वर्ण *buf, size_t blen,
                          size_t *pnread)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  जइ(nread >= 0)
    *pnread = (size_t)nread;
  अण्णहा {
    जइ((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      गच्छ out;
    }
    अण्णहा जइ(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      जइ(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      गच्छ out;
    }
    अण्णहा {
      failf(data, "GnuTLS recv error (%d): %s",
            (पूर्णाङ्क)nread, gnutls_strerror((पूर्णाङ्क)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      गच्छ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (पूर्णाङ्क)result, nread);
  प्रत्यागम result;
}

size_t Curl_gtls_version(वर्ण *buffer, size_t size)
{
  प्रत्यागम curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(रिक्त));
}

/* data might be NULL! */
स्थिर CURLcode gtls_random(संरचना Curl_easy *data,
                            unsigned वर्ण *entropy, size_t length)
{
  पूर्णाङ्क rc;
  (शून्य)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  प्रत्यागम rc ? CURLE_FAILED_INIT : CURLE_OK;
}

स्थिर CURLcode gtls_sha256sum(ध्रुव unsigned वर्ण *निवेशः,
                               size_t len,
                               unsigned वर्ण *sha256sum, /* output */
                               size_t sha256len)
{
  संरचना sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  करेहि {
    unsigned पूर्णाङ्क ilen = (unsigned पूर्णाङ्क)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, निवेशः);
    len -= ilen;
    निवेशः += ilen;
  } जाव(len);
#जइ NETTLE_VERSION_MAJOR >= 4
  (शून्य)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#अण्णहा
  sha256_digest(&SHA256pw, (unsigned पूर्णाङ्क)sha256len, sha256sum);
#endif
  प्रत्यागम CURLE_OK;
}

स्थिर bool gtls_cert_status_request(शून्य)
{
  प्रत्यागम TRUE;
}

स्थिर शून्य *gtls_get_internals(संरचना ssl_connect_data *connssl,
                                CURLINFO info)
{
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  प्रत्यागम backend->gtls.session;
}

ध्रुव संरचना Curl_ssl Curl_ssl_gnutls = {
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

  परिमाण(संरचना gtls_ssl_backend_data),

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
  रिक्त,                          /* close_all */
  रिक्त,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  रिक्त,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
