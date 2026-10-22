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

#ifdef USE_GNUTLS

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
#אם 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
ساكن فراغ tls_log_func(صحيح level, ثابت حرف *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#אם !defined(GNUTLS_VERSION_NUMBER) || (GNUTLS_VERSION_NUMBER < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#אם GNUTLS_VERSION_NUMBER >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#ضمن <gnutls/ocsp.h>

بنية gtls_ssl_backend_data {
  بنية gtls_ctx gtls;
};

ساكن ssize_t gtls_push(فراغ *s, ثابت فراغ *buf, size_t blen)
{
  بنية Curl_cfilter *cf = s;
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (صحيح)result, nwritten);
  backend->gtls.io_result = result;
  אם(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    החזר -1;
  }
  החזר (ssize_t)nwritten;
}

ساكن ssize_t gtls_pull(فراغ *s, فراغ *buf, size_t blen)
{
  بنية Curl_cfilter *cf = s;
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  אם(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    אם(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      החזר -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (صحيح)result,
              nread);
  backend->gtls.io_result = result;
  אם(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    החזר -1;
  }
  אחרת אם(nread == 0)
    connssl->peer_closed = TRUE;
  החזר (ssize_t)nread;
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
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  החזר ret;
}

ساكن فراغ gtls_cleanup(فراغ)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
ساكن فراغ showtime(بنية Curl_easy *data, ثابت حرف *text, time_t stamp)
{
  بنية tm buffer;
  ثابت بنية tm *tm = &buffer;
  حرف str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  אם(result)
    החזר;

  curl_msnprintf(str,
                 حجم(str),
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

ساكن gnutls_datum_t load_file(ثابت حرف *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { عدم, 0 };
  long filelen;
  فراغ *ptr;

  f = curlx_fopen(file, "rb");
  אם(!f)
    החזר loaded_file;
  אם(fseek(f, 0, SEEK_END))
    اذهب out;
  filelen = ftell(f);
  אם(filelen < 0 || filelen > CURL_MAX_INPUT_LENGTH)
    اذهب out;
  אם(fseek(f, 0, SEEK_SET))
    اذهب out;
  ptr = curlx_malloc((size_t)filelen);
  אם(!ptr)
    اذهب out;
  אם(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    اذهب out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned صحيح)filelen;
out:
  curlx_fclose(f);
  החזר loaded_file;
}

ساكن فراغ unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
ساكن CURLcode cf_gtls_handshake(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  صحيح rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  אם(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    אם(result)
      החזר result;
  }

  אם((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    החזר CURLE_AGAIN;
  }
  אחרת אם((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    ثابت حرف *strerr = عدم;

    אם(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    אם(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    החזר CURLE_AGAIN;
  }
  אחרת אם((rc < 0) && backend->gtls.io_result) {
    החזר backend->gtls.io_result;
  }
  אחרת אם(rc < 0) {
    ثابت حرف *strerr = عدم;

    אם(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    אם(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    החזר CURLE_SSL_CONNECT_ERROR;
  }

  החזר CURLE_OK;
}

ساكن gnutls_x509_crt_fmt_t gnutls_do_file_type(ثابت حرف *type)
{
  אם(!type || !type[0])
    החזר GNUTLS_X509_FMT_PEM;
  אם(curl_strequal(type, "PEM"))
    החזר GNUTLS_X509_FMT_PEM;
  אם(curl_strequal(type, "DER"))
    החזר GNUTLS_X509_FMT_DER;
  החזר GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ساكن CURLcode gnutls_set_ssl_version_min_max(
  بنية Curl_easy *data,
  بنية ssl_peer *peer,
  بنية ssl_filter_config *conn_config,
  ثابت حرف **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  אם(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  אם((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  אם(peer->transport == TRNSPRT_QUIC) {
    אם(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      החזר CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    החזר CURLE_OK;
  }

  بدل(ssl_version | ssl_version_max) {
  حالة CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    החזר CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    החזר CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    החזר CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    החזר CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    החזר CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    החזר CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    החזר CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    החזר CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    החזר CURLE_OK;
  حالة CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    החזר CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  החזר CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(بنية Curl_easy *data,
                                       بنية gtls_shared_creds **pcreds)
{
  بنية gtls_shared_creds *shared;
  صحيح rc;

  *pcreds = عدم;
  shared = curlx_calloc(1, حجم(*shared));
  אם(!shared)
    החזר CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  אם(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    החזר CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  החזר CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(بنية gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  אם(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    החזר CURLE_OK;
  }
  החזר CURLE_BAD_FUNCTION_ARGUMENT;
}

فراغ Curl_gtls_shared_creds_free(بنية gtls_shared_creds **pcreds)
{
  بنية gtls_shared_creds *shared = *pcreds;
  *pcreds = عدم;
  אם(shared) {
    --shared->refcount;
    אם(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

ساكن CURLcode gtls_populate_creds(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  بنية ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  صحيح rc;

  אם(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    החזר CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  אם(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#אחרת
    rc = gnutls_certificate_set_x509_system_trust(creds);
    אם(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    אחרת {
      infof(data, "  Native: %d certificates from system trust", rc);
      אם(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  אם(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    אם(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      החזר CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned صحيح)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    אם(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      אם(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        החזר CURLE_SSL_CACERT_BADFILE;
      }
    }
    אחרת
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  אחרת אם(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    אם(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      אם(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        החזר CURLE_SSL_CACERT_BADFILE;
      }
    }
    אחרת
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  אם(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    אם(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      אם(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        החזר CURLE_SSL_CACERT_BADFILE;
      }
    }
    אחרת
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  אם(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  אם(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    החזר CURLE_NOT_BUILT_IN;
  }
#endif
  אם(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    אם(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      החזר CURLE_SSL_CRL_BADFILE;
    }
    אחרת
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  החזר CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_KEY   "tls:gtls:x509:share"

ساكن bool gtls_shared_creds_expired(بنية Curl_easy *data,
                                      ثابت بنية gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  אם(timeout_ms < 0)
    החזר FALSE;

  החזר elapsed_ms >= timeout_ms;
}

ساكن bool gtls_shared_creds_different(بنية Curl_cfilter *cf,
                                        ثابت بنية gtls_shared_creds *sc)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  אם(!sc->CAfile || !conn_config->CAfile)
    החזר sc->CAfile != conn_config->CAfile;

  החזר strcmp(sc->CAfile, conn_config->CAfile);
}

ساكن بنية gtls_shared_creds *gtls_get_cached_creds(بنية Curl_cfilter *cf,
                                                       بنية Curl_easy *data)
{
  بنية gtls_shared_creds *shared_creds;

  אם(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_KEY,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_KEY));
    אם(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      החזר shared_creds;
    }
  }
  החזר عدم;
}

ساكن فراغ gtls_shared_creds_hash_free(ثابت فراغ *key,
                                        size_t key_len, فراغ *p)
{
  بنية gtls_shared_creds *sc = p;
  DEBUGASSERT(key_len == CURL_CSTRLEN(MPROTO_GTLS_X509_KEY));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_KEY, key, key_len));
  (فراغ)key;
  (فراغ)key_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

ساكن فراغ gtls_set_cached_creds(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  بنية gtls_shared_creds *sc)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  אם(!data->multi)
    החזר;

  אם(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    אם(!sc->CAfile)
      החזר;
  }

  אם(Curl_gtls_shared_creds_up_ref(sc))
    החזר;

  אם(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_KEY,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_KEY),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    החזר;
  }
}

CURLcode Curl_gtls_client_trust_setup(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      بنية gtls_ctx *gtls)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية gtls_shared_creds *cached_creds = عدم;
  bool cache_criteria_met;
  CURLcode result;
  صحيح rc;

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

  אם(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  אם(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    אם(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      החזר CURLE_SSL_CONNECT_ERROR;
    }
  }
  אחרת {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    אם(result)
      החזר result;
    gtls->shared_creds->trust_setup = TRUE;
    אם(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  החזר CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ساكن صحيح gtls_get_ietf_proto(gnutls_session_t session)
{
  بدل(gnutls_protocol_get_version(session)) {
  حالة GNUTLS_SSL3:
    החזר CURL_IETF_PROTO_SSL3;
  حالة GNUTLS_TLS1_0:
    החזר CURL_IETF_PROTO_TLS1;
  حالة GNUTLS_TLS1_1:
    החזר CURL_IETF_PROTO_TLS1_1;
  حالة GNUTLS_TLS1_2:
    החזר CURL_IETF_PROTO_TLS1_2;
  حالة GNUTLS_TLS1_3:
    החזר CURL_IETF_PROTO_TLS1_3;
  افتراضي:
    החזר CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 ثابت حرف *ssl_peer_key,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ثابت حرف *alpn,
                                 unsigned حرف *quic_tp,
                                 size_t quic_tp_len,
                                 بنية Curl_ssl_session **psession)
{
  بنية Curl_ssl_session *sc_session = عدم, *sc_dup = عدم;
  unsigned حرف *sdata, *qtp_clone = عدم;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  אם(psession)
    *psession = عدم;
  אם(!Curl_ssl_scache_use(cf, data))
    החזר CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, عدم, &sdata_len);
  אם(!sdata_len) /* GnuTLS does this for some version combinations */
    החזר CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  אם(!sdata)
    החזר CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  אם(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ثابت حرف *)quic_tp, quic_tp_len);
    אם(!qtp_clone) {
      curlx_free(sdata);
      החזר CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  אם(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  אם(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_key, sc_session);
    /* took ownership of `sc_session` */
    sc_session = عدم;
  }
  אם(!result && psession) {
    *psession = sc_dup;
    sc_dup = عدم;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  החזר result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
ساكن CURLcode cf_gtls_update_session_id(بنية Curl_cfilter *cf,
                                          بنية Curl_easy *data,
                                          gnutls_session_t session)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية Curl_ssl_session *scs = عدم;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_key,
                                   session, 0, connssl->negotiated.alpn,
                                   عدم, 0, &scs);
  אם(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  החזר result;
}

ساكن صحيح gtls_handshake_cb(gnutls_session_t session, unsigned صحيح htype,
                             unsigned when, unsigned صحيح incoming,
                             ثابت gnutls_datum_t *msg)
{
  بنية Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (فراغ)msg;
  (فراغ)incoming;
  אם(when) { /* after message has been processed */
    بنية Curl_easy *data = CF_DATA_CURRENT(cf);
    אם(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      بدل(htype) {
      حالة GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        اكسر;
      }
      افتراضي:
        اكسر;
      }
    }
  }
  החזר 0;
}
#endif

ساكن CURLcode gtls_set_priority(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  بنية gtls_ctx *gtls,
                                  ثابت حرف *priority)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية dynbuf buf;
  ثابت حرف *err = عدم;
  CURLcode result = CURLE_OK;
  صحيح rc;

  curlx_dyn_init(&buf, 4096);

  אם(conn_config->cipher_list) {
    אם((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      אם(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        אם(result)
          اذهب out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      אם(result)
        اذهب out;
      priority = curlx_dyn_ptr(&buf);
    }
    אחרת /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  אם(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  החזר result;
}

ساكن CURLcode gtls_client_init(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية ssl_peer *peer,
                                 size_t earlydata_max,
                                 بنية gtls_ctx *gtls)
{
  بنية ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned صحيح init_flags;
  صحيح rc;
  ثابت حرف *prioritylist;
  bool tls13support;
  CURLcode result;

  אם(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    החזר CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  אם(result)
    החזר result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  אם(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  אחרת אם(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  אם(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  אם(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    החזר CURLE_SSL_CONNECT_ERROR;
  }

  אם(peer->sni) {
    אם(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      החזר CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  אם(rc != GNUTLS_E_SUCCESS)
    החזר CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  אם(config->version == CURL_SSLVERSION_TLSv1_3) {
    אם(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      החזר CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  אם(result)
    החזר result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  אם(result)
    החזר result;

  אם(config->clientcert) {
    אם(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      אם(result)
        החזר result;
    }
    אם(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->key_passwd ? config->key_passwd : "");
      אם(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        החזר CURLE_SSL_CONNECT_ERROR;
      }
    }
    אחרת {
      ثابت unsigned صحيح supported_key_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_key_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->key ? config->key : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->key_passwd,
           supported_key_encryption_algorithms);
      אם(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->key_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        החזר CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  אם(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    החזר CURLE_SSL_CONNECT_ERROR;
  }

  אם(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  عدم, 0, عدم);
    אם(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      החזר CURLE_SSL_CONNECT_ERROR;
    }
  }

  החזר CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ساكن صحيح keylog_callback(gnutls_session_t session, ثابت حرف *label,
                           ثابت gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  אם(crandom.size != 32) {
    החזר -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  החזר 0;
}

ساكن CURLcode gtls_on_session_reuse(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      بنية alpn_spec *alpns,
                                      بنية Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  החזר Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

ساكن CURLcode gtls_apply_session(
  بنية gtls_ctx *gctx,
  بنية Curl_cfilter *cf,
  بنية Curl_easy *data,
  بنية ssl_peer *peer,
  بنية alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  بنية Curl_ssl_session *scs,
  bool *pwas_setup)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  صحيح rc;

  אם(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    אם(result)
      اذهب out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    אם(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    אחרת {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      אם(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        אם(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          אם(result)
            اذهب out;
        }
        אם(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  החזר result;
}

CURLcode Curl_gtls_ctx_init(بنية gtls_ctx *gctx,
                            بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            بنية ssl_peer *peer,
                            ثابت بنية alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            فراغ *cb_user_data,
                            فراغ *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_session *scs = عدم;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_count = 0;
  bool gtls_session_setup = FALSE;
  بنية alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  אם((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    אם(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = عدم;
      אם(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  אם(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_key, &scs);
    אם(result)
      اذهب out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  אם(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    אם(result)
      اذهب out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  אם(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    אם(result)
      اذهب out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  אם(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  אם(!gtls_alpns_count && alpns.count) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.count);
    עבור(i = 0; i < alpns.count; ++i) {
      gtls_alpns[i].data = (unsigned حرف *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned صحيح)strlen(alpns.entries[i]);
    }
    gtls_alpns_count = alpns.count;
  }

  אם(gtls_alpns_count &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned صحيح)gtls_alpns_count,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_key, scs);
  החזר result;
}

ساكن CURLcode gtls_connect_step1(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  אם(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    החזר CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, عدم, عدم, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#אחרת
                              عدم
#endif
                              );

  אם(result)
    החזר result;

  אם(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    بنية alpn_proto_buf proto;
    memset(&proto, 0, حجم(proto));
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
  gnutls_transport_set_push_function(backend->gtls.session, gtls_push);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  החזר CURLE_OK;
}

ساكن CURLcode pkp_pin_peer_pubkey(بنية Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    ثابت حرف *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned حرف *buff1 = عدم;

  gnutls_pubkey_t key = عدم;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  אם(!pinnedpubkey)
    החזר CURLE_OK;

  אם(!cert)
    החזר result;

  עשה {
    صحيح ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&key);
    אם(ret < 0)
      اكسر; /* failed */

    ret = gnutls_pubkey_import_x509(key, cert, 0);
    אם(ret < 0)
      اكسر; /* failed */

    ret = gnutls_pubkey_export(key, GNUTLS_X509_FMT_DER, عدم, &len1);
    אם(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      اكسر; /* failed */

    buff1 = curlx_malloc(len1);
    אם(!buff1)
      اكسر; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(key, GNUTLS_X509_FMT_DER, buff1, &len2);
    אם(ret < 0 || len1 != len2)
      اكسر; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } כלעוד(0);

  אם(key)
    gnutls_pubkey_deinit(key);

  curlx_safefree(buff1);

  החזר result;
}

فراغ Curl_gtls_report_handshake(بنية Curl_easy *data, بنية gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  אם(Curl_trc_is_verbose(data)) {
    ثابت حرف *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#אחרת
  (فراغ)data;
  (فراغ)gctx;
#endif
}

ساكن فراغ gtls_msg_verify_result(بنية Curl_easy *data,
                                   بنية ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  حرف certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = حجم(certname);
  صحيح rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  אם(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  אם(!was_verified) {
    אם(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    אחרת
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  אחרת
    infof(data, "  common name: %s (matched)", certname);
}

ساكن فراغ gtls_infof_cert(بنية Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  אם(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    صحيح rc, algo;
    time_t tstamp;
    unsigned صحيح bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    אם(rc)
      infof(data, "Failed to get certificate name");
    אחרת {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    אם(rc)
      infof(data, "Failed to get certificate issuer");
    אחרת {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#אחרת
  (فراغ)data;
  (فراغ)x509_cert;
#endif
}

ساكن CURLcode gtls_verify_ocsp_status(بنية Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = عدم;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  صحيح rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  אם(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    اذهب out;
  }
  אחרת אם(rc < 0) {
    failf(data, "Invalid OCSP response received");
    اذهب out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  אם(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    اذهب out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  אם(rc < 0) {
    failf(data, "failed to parse OCSP response");
    اذهب out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  אם(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    اذهب out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  עבור(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    אם(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      اكسر;
  }
  אם(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    اذهب out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, عدم, عدم, عدم, عدم,
                                   &status, عدم, عدم, عدم, عدم);
  אם(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    اذهب out;
  }
  بدل(status) {
  حالة GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    اكسر;
  حالة GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    اكسر;
  افتراضي:
    failf(data, "peer certificate OCSP status is not good: %d", (صحيح)status);
    اكسر;
  }

out:
  אם(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  החזר result;
}

بنية gtls_cert_chain {
  ثابت gnutls_datum_t *certs;
  unsigned صحيح num_certs;
};

#ifdef USE_APPLE_SECTRUST
ساكن CURLcode gtls_chain_get_der(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   فراغ *user_data,
                                   size_t i,
                                   unsigned حرف **pder,
                                   size_t *pder_len)
{
  بنية gtls_cert_chain *chain = user_data;

  (فراغ)cf;
  (فراغ)data;
  *pder_len = 0;
  *pder = عدم;

  אם(i >= chain->num_certs)
    החזר CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  החזר CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ساكن CURLcode gtls_verify_cert(بنية Curl_easy *data,
                                 بنية ssl_filter_config *config,
                                 بنية ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 بنية Curl_cfilter *cf,
                                 بنية ssl_peer *peer,
                                 بنية gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned صحيح verify_status = 0;
  long * ثابت certverifyresult = &ssl_config->certverifyresult;
  صحيح rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  אם(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    החזר CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  אם(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  אם(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    אם(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = عدم;
      ocsp_raw.size = 0;
    }
    אם(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      החזר CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->num_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    אם(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      החזר result; /* no further failure analysis needed */
    verified = !result;
    אם(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#אחרת
  (فراغ)cf;
  (فراغ)peer;
  (فراغ)chain;
#endif

  אם(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ثابت حرف *cause = "certificate error, no details available";
    אם(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    אחרת אם(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    אחרת אם(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    אחרת אם(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    החזר result;
  }
  החזר CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                gnutls_session_t session,
                                بنية ssl_filter_config *config,
                                بنية ssl_easy_config *ssl_config,
                                بنية ssl_peer *peer,
                                ثابت حرف *pinned_key)
{
  بنية gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = عدم, x509_issuer = عدم;
  time_t certclock;
  bool ocsp_checked = FALSE;
  صحيح rc;
  CURLcode result = CURLE_OK;
  long * ثابت certverifyresult = &ssl_config->certverifyresult;

  (فراغ)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.num_certs);
  אם(!chain.certs) {
    אם(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      اذهب out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  אם(data->set.ssl.certinfo && chain.certs) {
    אם(chain.num_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.num_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    אחרת {
      unsigned صحيح i;

      result = Curl_ssl_init_certinfo(data, (صحيح)chain.num_certs);
      אם(result)
        اذهب out;

      עבור(i = 0; i < chain.num_certs; i++) {
        ثابت uint8_t *beg = chain.certs[i].data;
        ثابت uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (صحيح)i, beg, end);
        אם(result)
          اذهب out;
      }
    }
  }

  אם(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    אם(result)
      اذهب out;
  }
  אחרת
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  אם(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    اذهب out;
  }

  אם(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    אם(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  אם(certclock == (time_t)-1) {
    אם(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    אחרת
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  אחרת {
    אם(certclock < time(عدم)) {
      אם(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        اذهب out;
      }
      אחרת
        infof(data, "  SSL certificate expiration date FAILED");
    }
    אחרת
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  אם(certclock == (time_t)-1) {
    אם(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    אחרת
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  אחרת {
    אם(certclock > time(عدم)) {
      אם(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        اذهب out;
      }
      אחרת
        infof(data, "  SSL certificate activation date FAILED");
    }
    אחרת
      infof(data, "  SSL certificate activation date OK");
  }

  אם(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    אם(result)
      اذهب out;
  }
  אחרת
    infof(data, "  SSL certificate status verification SKIPPED");

  אם(config->issuercert) {
    gnutls_datum_t issuerp;
    אם(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      اذهب out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    אם(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      اذهب out;
    }
    rc = (صحيح)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    אם(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      اذهب out;
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
  rc = (صحيح)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_msg_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  אם(result)
    اذهب out;

  אם(pinned_key) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_key);
    אם(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      اذهب out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  אם(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  אם(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  החזר result;
}

ساكن CURLcode gtls_verifyserver(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  gnutls_session_t session)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  ثابت حرف *pinned_key = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#אחרת
  ثابت حرف *pinned_key = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_key);
  אם(result)
    اذهب out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  אם(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  החזר result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ساكن CURLcode gtls_send_earlydata(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ثابت unsigned حرف *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  כלעוד(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    אם(n < 0) {
      אם(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      אחרת
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      اذهب out;
    }
    אחרת אם(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  החזר result;
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
ساكن CURLcode gtls_connect_common(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    bool *done)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  אם(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    החזר CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  אם(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    אם(result)
      اذهب out;
    connssl->connecting_state = ssl_connect_2;
  }

  אם(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    אם(connssl->earlydata_state == ssl_earlydata_await) {
      اذهب out;
    }
    אחרת אם(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      אם(result)
        اذهب out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    אם(result)
      اذهب out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  אם(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    صحيح rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    אם(result)
      اذهب out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    אם(rc) {  /* No ALPN from server */
      proto.data = عدم;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    אם(result)
      اذهب out;

#ifdef CURL_GNUTLS_EARLY_DATA
    אם(connssl->earlydata_state > ssl_earlydata_none) {
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

  אם(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  אם(result == CURLE_AGAIN) {
    *done = FALSE;
    החזר CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (صحيح)result,
              *done);
  החזר result;
}

ساكن CURLcode gtls_connect(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  بنية ssl_connect_data *connssl = cf->ctx;
  אם((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    החזר CURLE_OK;
  }
#endif
  החזר gtls_connect_common(cf, data, done);
}

ساكن bool gtls_data_pending(بنية Curl_cfilter *cf,
                              ثابت بنية Curl_easy *data)
{
  بنية ssl_connect_data *ctx = cf->ctx;
  بنية gtls_ssl_backend_data *backend;

  (فراغ)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية gtls_ssl_backend_data *)ctx->backend;
  אם(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    החזר TRUE;
  החזר FALSE;
}

ساكن CURLcode gtls_send(بنية Curl_cfilter *cf,
                          بنية Curl_easy *data,
                          ثابت فراغ *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  כלעוד(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    אם(nwritten >= 0) {
      אם((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        החזר CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (حرف *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    אחרת {
      אם(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        اذهب out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      اذهب out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (صحيح)result, *pnwritten);
  החזר result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ساكن CURLcode gtls_shutdown(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  حرف buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  אם(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    اذهب out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  אם(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    אם(send_shutdown) {
      صحيح ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      אם((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        اذهب out;
      }
      אם(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((صحيح)ret), (صحيح)ret);
        result = CURLE_RECV_ERROR;
        اذهب out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  עבור(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, حجم(buf));
    אם(nread <= 0)
      اكسر;
  }
  אם(nread > 0) {
    /* still data coming in? */
  }
  אחרת אם(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  אחרת אם((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  אחרת {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((صحيح)nread), (صحيح)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  החזר result;
}

ساكن فراغ gtls_close(بنية Curl_cfilter *cf,
                       بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  אם(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = عدم;
  }
  אם(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ساكن CURLcode gtls_recv(بنية Curl_cfilter *cf,
                          بنية Curl_easy *data,
                          حرف *buf, size_t blen,
                          size_t *pnread)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  אם(nread >= 0)
    *pnread = (size_t)nread;
  אחרת {
    אם((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      اذهب out;
    }
    אחרת אם(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      אם(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      اذهب out;
    }
    אחרת {
      failf(data, "GnuTLS recv error (%d): %s",
            (صحيح)nread, gnutls_strerror((صحيح)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      اذهب out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (صحيح)result, nread);
  החזר result;
}

size_t Curl_gtls_version(حرف *buffer, size_t size)
{
  החזר curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(عدم));
}

/* data might be NULL! */
ساكن CURLcode gtls_random(بنية Curl_easy *data,
                            unsigned حرف *entropy, size_t length)
{
  صحيح rc;
  (فراغ)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  החזר rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ساكن CURLcode gtls_sha256sum(ثابت unsigned حرف *input,
                               size_t len,
                               unsigned حرف *sha256sum, /* output */
                               size_t sha256len)
{
  بنية sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  עשה {
    unsigned صحيح ilen = (unsigned صحيح)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, input);
    len -= ilen;
    input += ilen;
  } כלעוד(len);
#אם NETTLE_VERSION_MAJOR >= 4
  (فراغ)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#אחרת
  sha256_digest(&SHA256pw, (unsigned صحيح)sha256len, sha256sum);
#endif
  החזר CURLE_OK;
}

ساكن bool gtls_cert_status_request(فراغ)
{
  החזר TRUE;
}

ساكن فراغ *gtls_get_internals(بنية ssl_connect_data *connssl,
                                CURLINFO info)
{
  بنية gtls_ssl_backend_data *backend =
    (بنية gtls_ssl_backend_data *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  החזר backend->gtls.session;
}

ثابت بنية Curl_ssl Curl_ssl_gnutls = {
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

  حجم(بنية gtls_ssl_backend_data),

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
  عدم,                          /* close_all */
  عدم,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  عدم,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
