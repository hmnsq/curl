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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_GNUTLS

#அந்தர்பாவ <gnutls/abstract.h>
#அந்தர்பாவ <gnutls/gnutls.h>
#அந்தர்பாவ <gnutls/x509.h>
#அந்தர்பாவ <gnutls/crypto.h>
#அந்தர்பாவ <nettle/sha2.h>
#அந்தர்பாவ <nettle/version.h>

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "vtls/keylog.h"
#அந்தர்பாவ "vtls/gtls.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_int.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "vtls/apple.h"
#அந்தர்பாவ "vauth/vauth.h"
#அந்தர்பாவ "parsedate.h"
#அந்தர்பாவ "connect.h" /* for the connect timeout */
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#எனில் 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
நிலையான வெறுமை tls_log_func(முழு level, மாறிலி எழுத்து *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#எனில் !defined(GNUTLS_VERSION_எண்) || (GNUTLS_VERSION_எண் < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#எனில் GNUTLS_VERSION_எண் >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#அந்தர்பாவ <gnutls/ocsp.h>

கட்டமைப்பு gtls_ssl_backend_data {
  கட்டமைப்பு gtls_ctx gtls;
};

நிலையான ssize_t gtls_தள்ளு(வெறுமை *s, மாறிலி வெறுமை *buf, size_t blen)
{
  கட்டமைப்பு Curl_cfilter *cf = s;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (முழு)result, nwritten);
  backend->gtls.io_result = result;
  எனில்(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    திரும்பு -1;
  }
  திரும்பு (ssize_t)nwritten;
}

நிலையான ssize_t gtls_pull(வெறுமை *s, வெறுமை *buf, size_t blen)
{
  கட்டமைப்பு Curl_cfilter *cf = s;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  எனில்(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    எனில்(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      திரும்பு -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (முழு)result,
              nread);
  backend->gtls.io_result = result;
  எனில்(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    திரும்பு -1;
  }
  இல்லையெனில் எனில்(nread == 0)
    connssl->peer_closed = TRUE;
  திரும்பு (ssize_t)nread;
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
நிலையான முழு gtls_init(வெறுமை)
{
  முழு ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  திரும்பு ret;
}

நிலையான வெறுமை gtls_cleanup(வெறுமை)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
நிலையான வெறுமை showtime(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *text, time_t stamp)
{
  கட்டமைப்பு tm buffer;
  மாறிலி கட்டமைப்பு tm *tm = &buffer;
  எழுத்து str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  எனில்(result)
    திரும்பு;

  curl_msnprintf(str,
                 அளவு(str),
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

நிலையான gnutls_datum_t load_file(மாறிலி எழுத்து *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { வெற்று, 0 };
  long filelen;
  வெறுமை *ptr;

  f = curlx_fopen(file, "rb");
  எனில்(!f)
    திரும்பு loaded_file;
  எனில்(fseek(f, 0, SEEK_END))
    செல் out;
  filelen = ftell(f);
  எனில்(filelen < 0 || filelen > CURL_MAX_உள்ளீடு_LENGTH)
    செல் out;
  எனில்(fseek(f, 0, SEEK_SET))
    செல் out;
  ptr = curlx_malloc((size_t)filelen);
  எனில்(!ptr)
    செல் out;
  எனில்(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    செல் out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned முழு)filelen;
out:
  curlx_fclose(f);
  திரும்பு loaded_file;
}

நிலையான வெறுமை unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
நிலையான CURLcode cf_gtls_handshake(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  முழு rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  எனில்(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    எனில்(result)
      திரும்பு result;
  }

  எனில்((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    திரும்பு CURLE_AGAIN;
  }
  இல்லையெனில் எனில்((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    மாறிலி எழுத்து *strerr = வெற்று;

    எனில்(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    எனில்(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    திரும்பு CURLE_AGAIN;
  }
  இல்லையெனில் எனில்((rc < 0) && backend->gtls.io_result) {
    திரும்பு backend->gtls.io_result;
  }
  இல்லையெனில் எனில்(rc < 0) {
    மாறிலி எழுத்து *strerr = வெற்று;

    எனில்(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    எனில்(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  திரும்பு CURLE_OK;
}

நிலையான gnutls_x509_crt_fmt_t gnutls_do_file_type(மாறிலி எழுத்து *type)
{
  எனில்(!type || !type[0])
    திரும்பு GNUTLS_X509_FMT_PEM;
  எனில்(curl_strequal(type, "PEM"))
    திரும்பு GNUTLS_X509_FMT_PEM;
  எனில்(curl_strequal(type, "DER"))
    திரும்பு GNUTLS_X509_FMT_DER;
  திரும்பு GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

நிலையான CURLcode gnutls_set_ssl_version_min_max(
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு ssl_peer *peer,
  கட்டமைப்பு ssl_filter_config *conn_config,
  மாறிலி எழுத்து **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  எனில்(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  எனில்((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  எனில்(peer->transport == TRNSPRT_QUIC) {
    எனில்(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    திரும்பு CURLE_OK;
  }

  தேர்வு(ssl_version | ssl_version_max) {
  நிலை CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    திரும்பு CURLE_OK;
  நிலை CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    திரும்பு CURLE_OK;
  நிலை CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    திரும்பு CURLE_OK;
  நிலை CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    திரும்பு CURLE_OK;
  நிலை CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    திரும்பு CURLE_OK;
  நிலை CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    திரும்பு CURLE_OK;
  நிலை CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    திரும்பு CURLE_OK;
  நிலை CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    திரும்பு CURLE_OK;
  நிலை CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    திரும்பு CURLE_OK;
  நிலை CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    திரும்பு CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  திரும்பு CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(கட்டமைப்பு Curl_easy *data,
                                       கட்டமைப்பு gtls_shared_creds **pcreds)
{
  கட்டமைப்பு gtls_shared_creds *shared;
  முழு rc;

  *pcreds = வெற்று;
  shared = curlx_calloc(1, அளவு(*shared));
  எனில்(!shared)
    திரும்பு CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  எனில்(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  திரும்பு CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(கட்டமைப்பு gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  எனில்(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    திரும்பு CURLE_OK;
  }
  திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
}

வெறுமை Curl_gtls_shared_creds_free(கட்டமைப்பு gtls_shared_creds **pcreds)
{
  கட்டமைப்பு gtls_shared_creds *shared = *pcreds;
  *pcreds = வெற்று;
  எனில்(shared) {
    --shared->refcount;
    எனில்(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

நிலையான CURLcode gtls_populate_creds(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  கட்டமைப்பு ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  முழு rc;

  எனில்(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    திரும்பு CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  எனில்(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#இல்லையெனில்
    rc = gnutls_certificate_set_x509_system_trust(creds);
    எனில்(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    இல்லையெனில் {
      infof(data, "  Native: %d certificates from system trust", rc);
      எனில்(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  எனில்(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    எனில்(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      திரும்பு CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned முழு)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    எனில்(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      எனில்(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        திரும்பு CURLE_SSL_CACERT_BADFILE;
      }
    }
    இல்லையெனில்
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  இல்லையெனில் எனில்(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    எனில்(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      எனில்(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        திரும்பு CURLE_SSL_CACERT_BADFILE;
      }
    }
    இல்லையெனில்
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  எனில்(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    எனில்(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      எனில்(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        திரும்பு CURLE_SSL_CACERT_BADFILE;
      }
    }
    இல்லையெனில்
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  எனில்(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  எனில்(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    திரும்பு CURLE_NOT_BUILT_IN;
  }
#endif
  எனில்(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    எனில்(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      திரும்பு CURLE_SSL_CRL_BADFILE;
    }
    இல்லையெனில்
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  திரும்பு CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_சாவி   "tls:gtls:x509:share"

நிலையான bool gtls_shared_creds_expired(கட்டமைப்பு Curl_easy *data,
                                      மாறிலி கட்டமைப்பு gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  எனில்(timeout_ms < 0)
    திரும்பு FALSE;

  திரும்பு elapsed_ms >= timeout_ms;
}

நிலையான bool gtls_shared_creds_different(கட்டமைப்பு Curl_cfilter *cf,
                                        மாறிலி கட்டமைப்பு gtls_shared_creds *sc)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  எனில்(!sc->CAfile || !conn_config->CAfile)
    திரும்பு sc->CAfile != conn_config->CAfile;

  திரும்பு strcmp(sc->CAfile, conn_config->CAfile);
}

நிலையான கட்டமைப்பு gtls_shared_creds *gtls_get_cached_creds(கட்டமைப்பு Curl_cfilter *cf,
                                                       கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு gtls_shared_creds *shared_creds;

  எனில்(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_சாவி,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_சாவி));
    எனில்(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      திரும்பு shared_creds;
    }
  }
  திரும்பு வெற்று;
}

நிலையான வெறுமை gtls_shared_creds_hash_free(மாறிலி வெறுமை *சாவி,
                                        size_t சாவி_len, வெறுமை *p)
{
  கட்டமைப்பு gtls_shared_creds *sc = p;
  DEBUGASSERT(சாவி_len == CURL_CSTRLEN(MPROTO_GTLS_X509_சாவி));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_சாவி, சாவி, சாவி_len));
  (வெறுமை)சாவி;
  (வெறுமை)சாவி_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

நிலையான வெறுமை gtls_set_cached_creds(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு gtls_shared_creds *sc)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  எனில்(!data->multi)
    திரும்பு;

  எனில்(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    எனில்(!sc->CAfile)
      திரும்பு;
  }

  எனில்(Curl_gtls_shared_creds_up_ref(sc))
    திரும்பு;

  எனில்(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_சாவி,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_சாவி),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    திரும்பு;
  }
}

CURLcode Curl_gtls_client_trust_setup(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு gtls_ctx *gtls)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு gtls_shared_creds *cached_creds = வெற்று;
  bool cache_criteria_met;
  CURLcode result;
  முழு rc;

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

  எனில்(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  எனில்(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    எனில்(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }
  இல்லையெனில் {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    எனில்(result)
      திரும்பு result;
    gtls->shared_creds->trust_setup = TRUE;
    எனில்(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  திரும்பு CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
நிலையான முழு gtls_get_ietf_proto(gnutls_session_t session)
{
  தேர்வு(gnutls_protocol_get_version(session)) {
  நிலை GNUTLS_SSL3:
    திரும்பு CURL_IETF_PROTO_SSL3;
  நிலை GNUTLS_TLS1_0:
    திரும்பு CURL_IETF_PROTO_TLS1;
  நிலை GNUTLS_TLS1_1:
    திரும்பு CURL_IETF_PROTO_TLS1_1;
  நிலை GNUTLS_TLS1_2:
    திரும்பு CURL_IETF_PROTO_TLS1_2;
  நிலை GNUTLS_TLS1_3:
    திரும்பு CURL_IETF_PROTO_TLS1_3;
  இயல்பு:
    திரும்பு CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 மாறிலி எழுத்து *ssl_peer_சாவி,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 மாறிலி எழுத்து *alpn,
                                 unsigned எழுத்து *quic_tp,
                                 size_t quic_tp_len,
                                 கட்டமைப்பு Curl_ssl_session **psession)
{
  கட்டமைப்பு Curl_ssl_session *sc_session = வெற்று, *sc_dup = வெற்று;
  unsigned எழுத்து *sdata, *qtp_clone = வெற்று;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  எனில்(psession)
    *psession = வெற்று;
  எனில்(!Curl_ssl_scache_use(cf, data))
    திரும்பு CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, வெற்று, &sdata_len);
  எனில்(!sdata_len) /* GnuTLS does this for some version combinations */
    திரும்பு CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  எனில்(!sdata)
    திரும்பு CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  எனில்(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((மாறிலி எழுத்து *)quic_tp, quic_tp_len);
    எனில்(!qtp_clone) {
      curlx_free(sdata);
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  எனில்(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  எனில்(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_சாவி, sc_session);
    /* took ownership of `sc_session` */
    sc_session = வெற்று;
  }
  எனில்(!result && psession) {
    *psession = sc_dup;
    sc_dup = வெற்று;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  திரும்பு result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
நிலையான CURLcode cf_gtls_update_session_id(கட்டமைப்பு Curl_cfilter *cf,
                                          கட்டமைப்பு Curl_easy *data,
                                          gnutls_session_t session)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு Curl_ssl_session *scs = வெற்று;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_சாவி,
                                   session, 0, connssl->negotiated.alpn,
                                   வெற்று, 0, &scs);
  எனில்(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  திரும்பு result;
}

நிலையான முழு gtls_handshake_cb(gnutls_session_t session, unsigned முழு htype,
                             unsigned when, unsigned முழு incoming,
                             மாறிலி gnutls_datum_t *செய்தி)
{
  கட்டமைப்பு Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (வெறுமை)செய்தி;
  (வெறுமை)incoming;
  எனில்(when) { /* after message has been processed */
    கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
    எனில்(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      தேர்வு(htype) {
      நிலை GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        நிறுத்து;
      }
      இயல்பு:
        நிறுத்து;
      }
    }
  }
  திரும்பு 0;
}
#endif

நிலையான CURLcode gtls_set_priority(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு gtls_ctx *gtls,
                                  மாறிலி எழுத்து *priority)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு dynbuf buf;
  மாறிலி எழுத்து *err = வெற்று;
  CURLcode result = CURLE_OK;
  முழு rc;

  curlx_dyn_init(&buf, 4096);

  எனில்(conn_config->cipher_list) {
    எனில்((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      எனில்(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        எனில்(result)
          செல் out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      எனில்(result)
        செல் out;
      priority = curlx_dyn_ptr(&buf);
    }
    இல்லையெனில் /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  எனில்(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  திரும்பு result;
}

நிலையான CURLcode gtls_client_init(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு ssl_peer *peer,
                                 size_t earlydata_max,
                                 கட்டமைப்பு gtls_ctx *gtls)
{
  கட்டமைப்பு ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned முழு init_flags;
  முழு rc;
  மாறிலி எழுத்து *prioritylist;
  bool tls13support;
  CURLcode result;

  எனில்(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  எனில்(result)
    திரும்பு result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  எனில்(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  இல்லையெனில் எனில்(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  எனில்(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  எனில்(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  எனில்(peer->sni) {
    எனில்(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  எனில்(rc != GNUTLS_E_SUCCESS)
    திரும்பு CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  எனில்(config->version == CURL_SSLVERSION_TLSv1_3) {
    எனில்(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  எனில்(result)
    திரும்பு result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  எனில்(result)
    திரும்பு result;

  எனில்(config->clientcert) {
    எனில்(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      எனில்(result)
        திரும்பு result;
    }
    எனில்(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->சாவி_passwd ? config->சாவி_passwd : "");
      எனில்(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        திரும்பு CURLE_SSL_CONNECT_ERROR;
      }
    }
    இல்லையெனில் {
      மாறிலி unsigned முழு supported_சாவி_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_சாவி_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->சாவி ? config->சாவி : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->சாவி_passwd,
           supported_சாவி_encryption_algorithms);
      எனில்(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->சாவி_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        திரும்பு CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  எனில்(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  எனில்(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  வெற்று, 0, வெற்று);
    எனில்(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
  }

  திரும்பு CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
நிலையான முழு keylog_callback(gnutls_session_t session, மாறிலி எழுத்து *label,
                           மாறிலி gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  எனில்(crandom.size != 32) {
    திரும்பு -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  திரும்பு 0;
}

நிலையான CURLcode gtls_on_session_reuse(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு alpn_spec *alpns,
                                      கட்டமைப்பு Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  திரும்பு Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

நிலையான CURLcode gtls_apply_session(
  கட்டமைப்பு gtls_ctx *gctx,
  கட்டமைப்பு Curl_cfilter *cf,
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு ssl_peer *peer,
  கட்டமைப்பு alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  கட்டமைப்பு Curl_ssl_session *scs,
  bool *pwas_setup)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  முழு rc;

  எனில்(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    எனில்(result)
      செல் out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    எனில்(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    இல்லையெனில் {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      எனில்(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        எனில்(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          எனில்(result)
            செல் out;
        }
        எனில்(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  திரும்பு result;
}

CURLcode Curl_gtls_ctx_init(கட்டமைப்பு gtls_ctx *gctx,
                            கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            கட்டமைப்பு ssl_peer *peer,
                            மாறிலி கட்டமைப்பு alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            வெறுமை *cb_user_data,
                            வெறுமை *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு Curl_ssl_session *scs = வெற்று;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_மொத்தம் = 0;
  bool gtls_session_setup = FALSE;
  கட்டமைப்பு alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  எனில்((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    எனில்(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = வெற்று;
      எனில்(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  எனில்(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_சாவி, &scs);
    எனில்(result)
      செல் out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  எனில்(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    எனில்(result)
      செல் out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  எனில்(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    எனில்(result)
      செல் out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  எனில்(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  எனில்(!gtls_alpns_மொத்தம் && alpns.மொத்தம்) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.மொத்தம்);
    க்கு(i = 0; i < alpns.மொத்தம்; ++i) {
      gtls_alpns[i].data = (unsigned எழுத்து *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned முழு)strlen(alpns.entries[i]);
    }
    gtls_alpns_மொத்தம் = alpns.மொத்தம்;
  }

  எனில்(gtls_alpns_மொத்தம் &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned முழு)gtls_alpns_மொத்தம்,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_சாவி, scs);
  திரும்பு result;
}

நிலையான CURLcode gtls_connect_step1(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  எனில்(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    திரும்பு CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, வெற்று, வெற்று, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#இல்லையெனில்
                              வெற்று
#endif
                              );

  எனில்(result)
    திரும்பு result;

  எனில்(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    கட்டமைப்பு alpn_proto_buf proto;
    memset(&proto, 0, அளவு(proto));
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
  gnutls_transport_set_தள்ளு_function(backend->gtls.session, gtls_தள்ளு);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  திரும்பு CURLE_OK;
}

நிலையான CURLcode pkp_pin_peer_pubkey(கட்டமைப்பு Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    மாறிலி எழுத்து *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned எழுத்து *buff1 = வெற்று;

  gnutls_pubkey_t சாவி = வெற்று;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  எனில்(!pinnedpubkey)
    திரும்பு CURLE_OK;

  எனில்(!cert)
    திரும்பு result;

  செய் {
    முழு ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&சாவி);
    எனில்(ret < 0)
      நிறுத்து; /* failed */

    ret = gnutls_pubkey_import_x509(சாவி, cert, 0);
    எனில்(ret < 0)
      நிறுத்து; /* failed */

    ret = gnutls_pubkey_export(சாவி, GNUTLS_X509_FMT_DER, வெற்று, &len1);
    எனில்(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      நிறுத்து; /* failed */

    buff1 = curlx_malloc(len1);
    எனில்(!buff1)
      நிறுத்து; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(சாவி, GNUTLS_X509_FMT_DER, buff1, &len2);
    எனில்(ret < 0 || len1 != len2)
      நிறுத்து; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } போது(0);

  எனில்(சாவி)
    gnutls_pubkey_deinit(சாவி);

  curlx_safefree(buff1);

  திரும்பு result;
}

வெறுமை Curl_gtls_report_handshake(கட்டமைப்பு Curl_easy *data, கட்டமைப்பு gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  எனில்(Curl_trc_is_verbose(data)) {
    மாறிலி எழுத்து *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)gctx;
#endif
}

நிலையான வெறுமை gtls_செய்தி_verify_result(கட்டமைப்பு Curl_easy *data,
                                   கட்டமைப்பு ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  எழுத்து certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = அளவு(certname);
  முழு rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  எனில்(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  எனில்(!was_verified) {
    எனில்(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    இல்லையெனில்
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  இல்லையெனில்
    infof(data, "  common name: %s (matched)", certname);
}

நிலையான வெறுமை gtls_infof_cert(கட்டமைப்பு Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  எனில்(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    முழு rc, algo;
    time_t tstamp;
    unsigned முழு bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    எனில்(rc)
      infof(data, "Failed to get certificate name");
    இல்லையெனில் {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    எனில்(rc)
      infof(data, "Failed to get certificate issuer");
    இல்லையெனில் {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#இல்லையெனில்
  (வெறுமை)data;
  (வெறுமை)x509_cert;
#endif
}

நிலையான CURLcode gtls_verify_ocsp_status(கட்டமைப்பு Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = வெற்று;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  முழு rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  எனில்(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    செல் out;
  }
  இல்லையெனில் எனில்(rc < 0) {
    failf(data, "Invalid OCSP response received");
    செல் out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  எனில்(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    செல் out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  எனில்(rc < 0) {
    failf(data, "failed to parse OCSP response");
    செல் out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  எனில்(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    செல் out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  க்கு(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    எனில்(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      நிறுத்து;
  }
  எனில்(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    செல் out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, வெற்று, வெற்று, வெற்று, வெற்று,
                                   &status, வெற்று, வெற்று, வெற்று, வெற்று);
  எனில்(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    செல் out;
  }
  தேர்வு(status) {
  நிலை GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    நிறுத்து;
  நிலை GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    நிறுத்து;
  இயல்பு:
    failf(data, "peer certificate OCSP status is not good: %d", (முழு)status);
    நிறுத்து;
  }

out:
  எனில்(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  திரும்பு result;
}

கட்டமைப்பு gtls_cert_chain {
  மாறிலி gnutls_datum_t *certs;
  unsigned முழு இலக்கம்_certs;
};

#ifdef USE_APPLE_SECTRUST
நிலையான CURLcode gtls_chain_get_der(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   வெறுமை *user_data,
                                   size_t i,
                                   unsigned எழுத்து **pder,
                                   size_t *pder_len)
{
  கட்டமைப்பு gtls_cert_chain *chain = user_data;

  (வெறுமை)cf;
  (வெறுமை)data;
  *pder_len = 0;
  *pder = வெற்று;

  எனில்(i >= chain->இலக்கம்_certs)
    திரும்பு CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  திரும்பு CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
நிலையான CURLcode gtls_verify_cert(கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு ssl_filter_config *config,
                                 கட்டமைப்பு ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு ssl_peer *peer,
                                 கட்டமைப்பு gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned முழு verify_status = 0;
  long * மாறிலி certverifyresult = &ssl_config->certverifyresult;
  முழு rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  எனில்(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  எனில்(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  எனில்(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    எனில்(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = வெற்று;
      ocsp_raw.size = 0;
    }
    எனில்(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      திரும்பு CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->இலக்கம்_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    எனில்(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      திரும்பு result; /* no further failure analysis needed */
    verified = !result;
    எனில்(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#இல்லையெனில்
  (வெறுமை)cf;
  (வெறுமை)peer;
  (வெறுமை)chain;
#endif

  எனில்(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    மாறிலி எழுத்து *cause = "certificate error, no details available";
    எனில்(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    இல்லையெனில் எனில்(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    இல்லையெனில் எனில்(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    இல்லையெனில் எனில்(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    திரும்பு result;
  }
  திரும்பு CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                gnutls_session_t session,
                                கட்டமைப்பு ssl_filter_config *config,
                                கட்டமைப்பு ssl_easy_config *ssl_config,
                                கட்டமைப்பு ssl_peer *peer,
                                மாறிலி எழுத்து *pinned_சாவி)
{
  கட்டமைப்பு gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = வெற்று, x509_issuer = வெற்று;
  time_t certclock;
  bool ocsp_checked = FALSE;
  முழு rc;
  CURLcode result = CURLE_OK;
  long * மாறிலி certverifyresult = &ssl_config->certverifyresult;

  (வெறுமை)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.இலக்கம்_certs);
  எனில்(!chain.certs) {
    எனில்(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      செல் out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  எனில்(data->set.ssl.certinfo && chain.certs) {
    எனில்(chain.இலக்கம்_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.இலக்கம்_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      செல் out;
    }
    இல்லையெனில் {
      unsigned முழு i;

      result = Curl_ssl_init_certinfo(data, (முழு)chain.இலக்கம்_certs);
      எனில்(result)
        செல் out;

      க்கு(i = 0; i < chain.இலக்கம்_certs; i++) {
        மாறிலி uint8_t *beg = chain.certs[i].data;
        மாறிலி uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (முழு)i, beg, end);
        எனில்(result)
          செல் out;
      }
    }
  }

  எனில்(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    எனில்(result)
      செல் out;
  }
  இல்லையெனில்
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  எனில்(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    செல் out;
  }

  எனில்(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    எனில்(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      செல் out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  எனில்(certclock == (time_t)-1) {
    எனில்(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      செல் out;
    }
    இல்லையெனில்
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  இல்லையெனில் {
    எனில்(certclock < time(வெற்று)) {
      எனில்(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        செல் out;
      }
      இல்லையெனில்
        infof(data, "  SSL certificate expiration date FAILED");
    }
    இல்லையெனில்
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  எனில்(certclock == (time_t)-1) {
    எனில்(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      செல் out;
    }
    இல்லையெனில்
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  இல்லையெனில் {
    எனில்(certclock > time(வெற்று)) {
      எனில்(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        செல் out;
      }
      இல்லையெனில்
        infof(data, "  SSL certificate activation date FAILED");
    }
    இல்லையெனில்
      infof(data, "  SSL certificate activation date OK");
  }

  எனில்(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    எனில்(result)
      செல் out;
  }
  இல்லையெனில்
    infof(data, "  SSL certificate status verification SKIPPED");

  எனில்(config->issuercert) {
    gnutls_datum_t issuerp;
    எனில்(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      செல் out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    எனில்(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      செல் out;
    }
    rc = (முழு)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    எனில்(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      செல் out;
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
  rc = (முழு)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_செய்தி_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  எனில்(result)
    செல் out;

  எனில்(pinned_சாவி) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_சாவி);
    எனில்(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      செல் out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  எனில்(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  எனில்(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  திரும்பு result;
}

நிலையான CURLcode gtls_verifyserver(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  gnutls_session_t session)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  மாறிலி எழுத்து *pinned_சாவி = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#இல்லையெனில்
  மாறிலி எழுத்து *pinned_சாவி = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_சாவி);
  எனில்(result)
    செல் out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  எனில்(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  திரும்பு result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
நிலையான CURLcode gtls_send_earlydata(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  மாறிலி unsigned எழுத்து *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  போது(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    எனில்(n < 0) {
      எனில்(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      இல்லையெனில்
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      செல் out;
    }
    இல்லையெனில் எனில்(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  திரும்பு result;
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
நிலையான CURLcode gtls_connect_common(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  எனில்(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  எனில்(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    எனில்(result)
      செல் out;
    connssl->connecting_state = ssl_connect_2;
  }

  எனில்(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    எனில்(connssl->earlydata_state == ssl_earlydata_await) {
      செல் out;
    }
    இல்லையெனில் எனில்(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      எனில்(result)
        செல் out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    எனில்(result)
      செல் out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  எனில்(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    முழு rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    எனில்(result)
      செல் out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    எனில்(rc) {  /* No ALPN from server */
      proto.data = வெற்று;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    எனில்(result)
      செல் out;

#ifdef CURL_GNUTLS_EARLY_DATA
    எனில்(connssl->earlydata_state > ssl_earlydata_none) {
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

  எனில்(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  எனில்(result == CURLE_AGAIN) {
    *done = FALSE;
    திரும்பு CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (முழு)result,
              *done);
  திரும்பு result;
}

நிலையான CURLcode gtls_connect(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  எனில்((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    திரும்பு CURLE_OK;
  }
#endif
  திரும்பு gtls_connect_common(cf, data, done);
}

நிலையான bool gtls_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                              மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *ctx = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend;

  (வெறுமை)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (கட்டமைப்பு gtls_ssl_backend_data *)ctx->backend;
  எனில்(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    திரும்பு TRUE;
  திரும்பு FALSE;
}

நிலையான CURLcode gtls_send(கட்டமைப்பு Curl_cfilter *cf,
                          கட்டமைப்பு Curl_easy *data,
                          மாறிலி வெறுமை *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  போது(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    எனில்(nwritten >= 0) {
      எனில்((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        திரும்பு CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (எழுத்து *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    இல்லையெனில் {
      எனில்(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        செல் out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      செல் out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (முழு)result, *pnwritten);
  திரும்பு result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
நிலையான CURLcode gtls_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  எழுத்து buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  எனில்(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    செல் out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  எனில்(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    எனில்(send_shutdown) {
      முழு ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      எனில்((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        செல் out;
      }
      எனில்(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((முழு)ret), (முழு)ret);
        result = CURLE_RECV_ERROR;
        செல் out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  க்கு(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, அளவு(buf));
    எனில்(nread <= 0)
      நிறுத்து;
  }
  எனில்(nread > 0) {
    /* still data coming in? */
  }
  இல்லையெனில் எனில்(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  இல்லையெனில் எனில்((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  இல்லையெனில் {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((முழு)nread), (முழு)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  திரும்பு result;
}

நிலையான வெறுமை gtls_close(கட்டமைப்பு Curl_cfilter *cf,
                       கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  எனில்(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = வெற்று;
  }
  எனில்(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

நிலையான CURLcode gtls_recv(கட்டமைப்பு Curl_cfilter *cf,
                          கட்டமைப்பு Curl_easy *data,
                          எழுத்து *buf, size_t blen,
                          size_t *pnread)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  எனில்(nread >= 0)
    *pnread = (size_t)nread;
  இல்லையெனில் {
    எனில்((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      செல் out;
    }
    இல்லையெனில் எனில்(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      எனில்(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      செல் out;
    }
    இல்லையெனில் {
      failf(data, "GnuTLS recv error (%d): %s",
            (முழு)nread, gnutls_strerror((முழு)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      செல் out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (முழு)result, nread);
  திரும்பு result;
}

size_t Curl_gtls_version(எழுத்து *buffer, size_t size)
{
  திரும்பு curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(வெற்று));
}

/* data might be NULL! */
நிலையான CURLcode gtls_random(கட்டமைப்பு Curl_easy *data,
                            unsigned எழுத்து *entropy, size_t length)
{
  முழு rc;
  (வெறுமை)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  திரும்பு rc ? CURLE_FAILED_INIT : CURLE_OK;
}

நிலையான CURLcode gtls_sha256sum(மாறிலி unsigned எழுத்து *உள்ளீடு,
                               size_t len,
                               unsigned எழுத்து *sha256sum, /* output */
                               size_t sha256len)
{
  கட்டமைப்பு sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  செய் {
    unsigned முழு ilen = (unsigned முழு)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, உள்ளீடு);
    len -= ilen;
    உள்ளீடு += ilen;
  } போது(len);
#எனில் NETTLE_VERSION_MAJOR >= 4
  (வெறுமை)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#இல்லையெனில்
  sha256_digest(&SHA256pw, (unsigned முழு)sha256len, sha256sum);
#endif
  திரும்பு CURLE_OK;
}

நிலையான bool gtls_cert_status_request(வெறுமை)
{
  திரும்பு TRUE;
}

நிலையான வெறுமை *gtls_get_internals(கட்டமைப்பு ssl_connect_data *connssl,
                                CURLINFO info)
{
  கட்டமைப்பு gtls_ssl_backend_data *backend =
    (கட்டமைப்பு gtls_ssl_backend_data *)connssl->backend;
  (வெறுமை)info;
  DEBUGASSERT(backend);
  திரும்பு backend->gtls.session;
}

மாறிலி கட்டமைப்பு Curl_ssl Curl_ssl_gnutls = {
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

  அளவு(கட்டமைப்பு gtls_ssl_backend_data),

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
  வெற்று,                          /* close_all */
  வெற்று,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  வெற்று,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
