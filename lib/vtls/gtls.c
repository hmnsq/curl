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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_GNUTLS

#અન્તર્ભાવ <gnutls/abstract.h>
#અન્તર્ભાવ <gnutls/gnutls.h>
#અન્તર્ભાવ <gnutls/x509.h>
#અન્તર્ભાવ <gnutls/crypto.h>
#અન્તર્ભાવ <nettle/sha2.h>
#અન્તર્ભાવ <nettle/version.h>

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "vtls/keylog.h"
#અન્તર્ભાવ "vtls/gtls.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/vtls_int.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "vtls/apple.h"
#અન્તર્ભાવ "vauth/vauth.h"
#અન્તર્ભાવ "parsedate.h"
#અન્તર્ભાવ "connect.h" /* for the connect timeout */
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"
#અન્તર્ભાવ "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#જો 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
સ્થિર શૂન્ય tls_log_func(પૂર્ણાંક level, અચલ અક્ષર *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#જો !defined(GNUTLS_VERSION_સંખ્યા) || (GNUTLS_VERSION_સંખ્યા < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#જો GNUTLS_VERSION_સંખ્યા >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#અન્તર્ભાવ <gnutls/ocsp.h>

સંરચના gtls_ssl_backend_data {
  સંરચના gtls_ctx gtls;
};

સ્થિર ssize_t gtls_ધકેલો(શૂન્ય *s, અચલ શૂન્ય *buf, size_t blen)
{
  સંરચના Curl_cfilter *cf = s;
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (પૂર્ણાંક)result, nwritten);
  backend->gtls.io_result = result;
  જો(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    પાછા -1;
  }
  પાછા (ssize_t)nwritten;
}

સ્થિર ssize_t gtls_pull(શૂન્ય *s, શૂન્ય *buf, size_t blen)
{
  સંરચના Curl_cfilter *cf = s;
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  જો(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    જો(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      પાછા -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (પૂર્ણાંક)result,
              nread);
  backend->gtls.io_result = result;
  જો(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    પાછા -1;
  }
  નહીંતર જો(nread == 0)
    connssl->peer_closed = TRUE;
  પાછા (ssize_t)nread;
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
સ્થિર પૂર્ણાંક gtls_init(શૂન્ય)
{
  પૂર્ણાંક ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  પાછા ret;
}

સ્થિર શૂન્ય gtls_cleanup(શૂન્ય)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
સ્થિર શૂન્ય showtime(સંરચના Curl_easy *data, અચલ અક્ષર *text, time_t stamp)
{
  સંરચના tm buffer;
  અચલ સંરચના tm *tm = &buffer;
  અક્ષર str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  જો(result)
    પાછા;

  curl_msnprintf(str,
                 કદ(str),
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

સ્થિર gnutls_datum_t load_file(અચલ અક્ષર *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { ખાલી, 0 };
  long filelen;
  શૂન્ય *ptr;

  f = curlx_fopen(file, "rb");
  જો(!f)
    પાછા loaded_file;
  જો(fseek(f, 0, SEEK_END))
    જાઓ out;
  filelen = ftell(f);
  જો(filelen < 0 || filelen > CURL_MAX_આગત_LENGTH)
    જાઓ out;
  જો(fseek(f, 0, SEEK_SET))
    જાઓ out;
  ptr = curlx_malloc((size_t)filelen);
  જો(!ptr)
    જાઓ out;
  જો(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    જાઓ out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned પૂર્ણાંક)filelen;
out:
  curlx_fclose(f);
  પાછા loaded_file;
}

સ્થિર શૂન્ય unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
સ્થિર CURLcode cf_gtls_handshake(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  પૂર્ણાંક rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  જો(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    જો(result)
      પાછા result;
  }

  જો((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    પાછા CURLE_AGAIN;
  }
  નહીંતર જો((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    અચલ અક્ષર *strerr = ખાલી;

    જો(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    જો(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    પાછા CURLE_AGAIN;
  }
  નહીંતર જો((rc < 0) && backend->gtls.io_result) {
    પાછા backend->gtls.io_result;
  }
  નહીંતર જો(rc < 0) {
    અચલ અક્ષર *strerr = ખાલી;

    જો(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    જો(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  પાછા CURLE_OK;
}

સ્થિર gnutls_x509_crt_fmt_t gnutls_do_file_type(અચલ અક્ષર *type)
{
  જો(!type || !type[0])
    પાછા GNUTLS_X509_FMT_PEM;
  જો(curl_strequal(type, "PEM"))
    પાછા GNUTLS_X509_FMT_PEM;
  જો(curl_strequal(type, "DER"))
    પાછા GNUTLS_X509_FMT_DER;
  પાછા GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

સ્થિર CURLcode gnutls_set_ssl_version_min_max(
  સંરચના Curl_easy *data,
  સંરચના ssl_peer *peer,
  સંરચના ssl_filter_config *conn_config,
  અચલ અક્ષર **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  જો(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  જો((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  જો(peer->transport == TRNSPRT_QUIC) {
    જો(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    પાછા CURLE_OK;
  }

  પસંદગી(ssl_version | ssl_version_max) {
  સ્થિતિ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    પાછા CURLE_OK;
  સ્થિતિ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    પાછા CURLE_OK;
  સ્થિતિ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    પાછા CURLE_OK;
  સ્થિતિ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    પાછા CURLE_OK;
  સ્થિતિ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    પાછા CURLE_OK;
  સ્થિતિ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    પાછા CURLE_OK;
  સ્થિતિ CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    પાછા CURLE_OK;
  સ્થિતિ CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    પાછા CURLE_OK;
  સ્થિતિ CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    પાછા CURLE_OK;
  સ્થિતિ CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    પાછા CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  પાછા CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(સંરચના Curl_easy *data,
                                       સંરચના gtls_shared_creds **pcreds)
{
  સંરચના gtls_shared_creds *shared;
  પૂર્ણાંક rc;

  *pcreds = ખાલી;
  shared = curlx_calloc(1, કદ(*shared));
  જો(!shared)
    પાછા CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  જો(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  પાછા CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(સંરચના gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  જો(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    પાછા CURLE_OK;
  }
  પાછા CURLE_BAD_FUNCTION_ARGUMENT;
}

શૂન્ય Curl_gtls_shared_creds_free(સંરચના gtls_shared_creds **pcreds)
{
  સંરચના gtls_shared_creds *shared = *pcreds;
  *pcreds = ખાલી;
  જો(shared) {
    --shared->refcount;
    જો(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

સ્થિર CURLcode gtls_populate_creds(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  સંરચના ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  પૂર્ણાંક rc;

  જો(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    પાછા CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  જો(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#નહીંતર
    rc = gnutls_certificate_set_x509_system_trust(creds);
    જો(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    નહીંતર {
      infof(data, "  Native: %d certificates from system trust", rc);
      જો(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  જો(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    જો(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      પાછા CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned પૂર્ણાંક)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    જો(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      જો(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        પાછા CURLE_SSL_CACERT_BADFILE;
      }
    }
    નહીંતર
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  નહીંતર જો(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    જો(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      જો(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        પાછા CURLE_SSL_CACERT_BADFILE;
      }
    }
    નહીંતર
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  જો(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    જો(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      જો(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        પાછા CURLE_SSL_CACERT_BADFILE;
      }
    }
    નહીંતર
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  જો(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  જો(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    પાછા CURLE_NOT_BUILT_IN;
  }
#endif
  જો(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    જો(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      પાછા CURLE_SSL_CRL_BADFILE;
    }
    નહીંતર
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  પાછા CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_ચાવી   "tls:gtls:x509:share"

સ્થિર bool gtls_shared_creds_expired(સંરચના Curl_easy *data,
                                      અચલ સંરચના gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  જો(timeout_ms < 0)
    પાછા FALSE;

  પાછા elapsed_ms >= timeout_ms;
}

સ્થિર bool gtls_shared_creds_different(સંરચના Curl_cfilter *cf,
                                        અચલ સંરચના gtls_shared_creds *sc)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  જો(!sc->CAfile || !conn_config->CAfile)
    પાછા sc->CAfile != conn_config->CAfile;

  પાછા strcmp(sc->CAfile, conn_config->CAfile);
}

સ્થિર સંરચના gtls_shared_creds *gtls_get_cached_creds(સંરચના Curl_cfilter *cf,
                                                       સંરચના Curl_easy *data)
{
  સંરચના gtls_shared_creds *shared_creds;

  જો(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_ચાવી,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_ચાવી));
    જો(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      પાછા shared_creds;
    }
  }
  પાછા ખાલી;
}

સ્થિર શૂન્ય gtls_shared_creds_hash_free(અચલ શૂન્ય *ચાવી,
                                        size_t ચાવી_len, શૂન્ય *p)
{
  સંરચના gtls_shared_creds *sc = p;
  DEBUGASSERT(ચાવી_len == CURL_CSTRLEN(MPROTO_GTLS_X509_ચાવી));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_ચાવી, ચાવી, ચાવી_len));
  (શૂન્ય)ચાવી;
  (શૂન્ય)ચાવી_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

સ્થિર શૂન્ય gtls_set_cached_creds(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  સંરચના gtls_shared_creds *sc)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  જો(!data->multi)
    પાછા;

  જો(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    જો(!sc->CAfile)
      પાછા;
  }

  જો(Curl_gtls_shared_creds_up_ref(sc))
    પાછા;

  જો(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_ચાવી,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_ચાવી),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    પાછા;
  }
}

CURLcode Curl_gtls_client_trust_setup(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      સંરચના gtls_ctx *gtls)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના gtls_shared_creds *cached_creds = ખાલી;
  bool cache_criteria_met;
  CURLcode result;
  પૂર્ણાંક rc;

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

  જો(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  જો(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    જો(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
  }
  નહીંતર {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    જો(result)
      પાછા result;
    gtls->shared_creds->trust_setup = TRUE;
    જો(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  પાછા CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
સ્થિર પૂર્ણાંક gtls_get_ietf_proto(gnutls_session_t session)
{
  પસંદગી(gnutls_protocol_get_version(session)) {
  સ્થિતિ GNUTLS_SSL3:
    પાછા CURL_IETF_PROTO_SSL3;
  સ્થિતિ GNUTLS_TLS1_0:
    પાછા CURL_IETF_PROTO_TLS1;
  સ્થિતિ GNUTLS_TLS1_1:
    પાછા CURL_IETF_PROTO_TLS1_1;
  સ્થિતિ GNUTLS_TLS1_2:
    પાછા CURL_IETF_PROTO_TLS1_2;
  સ્થિતિ GNUTLS_TLS1_3:
    પાછા CURL_IETF_PROTO_TLS1_3;
  મૂળભૂત:
    પાછા CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 અચલ અક્ષર *ssl_peer_ચાવી,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 અચલ અક્ષર *alpn,
                                 unsigned અક્ષર *quic_tp,
                                 size_t quic_tp_len,
                                 સંરચના Curl_ssl_session **psession)
{
  સંરચના Curl_ssl_session *sc_session = ખાલી, *sc_dup = ખાલી;
  unsigned અક્ષર *sdata, *qtp_clone = ખાલી;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  જો(psession)
    *psession = ખાલી;
  જો(!Curl_ssl_scache_use(cf, data))
    પાછા CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, ખાલી, &sdata_len);
  જો(!sdata_len) /* GnuTLS does this for some version combinations */
    પાછા CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  જો(!sdata)
    પાછા CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  જો(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((અચલ અક્ષર *)quic_tp, quic_tp_len);
    જો(!qtp_clone) {
      curlx_free(sdata);
      પાછા CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  જો(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  જો(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_ચાવી, sc_session);
    /* took ownership of `sc_session` */
    sc_session = ખાલી;
  }
  જો(!result && psession) {
    *psession = sc_dup;
    sc_dup = ખાલી;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  પાછા result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
સ્થિર CURLcode cf_gtls_update_session_id(સંરચના Curl_cfilter *cf,
                                          સંરચના Curl_easy *data,
                                          gnutls_session_t session)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના Curl_ssl_session *scs = ખાલી;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_ચાવી,
                                   session, 0, connssl->negotiated.alpn,
                                   ખાલી, 0, &scs);
  જો(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  પાછા result;
}

સ્થિર પૂર્ણાંક gtls_handshake_cb(gnutls_session_t session, unsigned પૂર્ણાંક htype,
                             unsigned when, unsigned પૂર્ણાંક incoming,
                             અચલ gnutls_datum_t *સંદેશ)
{
  સંરચના Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (શૂન્ય)સંદેશ;
  (શૂન્ય)incoming;
  જો(when) { /* after message has been processed */
    સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
    જો(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      પસંદગી(htype) {
      સ્થિતિ GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        તોડો;
      }
      મૂળભૂત:
        તોડો;
      }
    }
  }
  પાછા 0;
}
#endif

સ્થિર CURLcode gtls_set_priority(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  સંરચના gtls_ctx *gtls,
                                  અચલ અક્ષર *priority)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના dynbuf buf;
  અચલ અક્ષર *err = ખાલી;
  CURLcode result = CURLE_OK;
  પૂર્ણાંક rc;

  curlx_dyn_init(&buf, 4096);

  જો(conn_config->cipher_list) {
    જો((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      જો(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        જો(result)
          જાઓ out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      જો(result)
        જાઓ out;
      priority = curlx_dyn_ptr(&buf);
    }
    નહીંતર /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  જો(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  પાછા result;
}

સ્થિર CURLcode gtls_client_init(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના ssl_peer *peer,
                                 size_t earlydata_max,
                                 સંરચના gtls_ctx *gtls)
{
  સંરચના ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned પૂર્ણાંક init_flags;
  પૂર્ણાંક rc;
  અચલ અક્ષર *prioritylist;
  bool tls13support;
  CURLcode result;

  જો(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  જો(result)
    પાછા result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  જો(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  નહીંતર જો(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  જો(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  જો(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  જો(peer->sni) {
    જો(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  જો(rc != GNUTLS_E_SUCCESS)
    પાછા CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  જો(config->version == CURL_SSLVERSION_TLSv1_3) {
    જો(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  જો(result)
    પાછા result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  જો(result)
    પાછા result;

  જો(config->clientcert) {
    જો(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      જો(result)
        પાછા result;
    }
    જો(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->ચાવી_passwd ? config->ચાવી_passwd : "");
      જો(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        પાછા CURLE_SSL_CONNECT_ERROR;
      }
    }
    નહીંતર {
      અચલ unsigned પૂર્ણાંક supported_ચાવી_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_ચાવી_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->ચાવી ? config->ચાવી : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->ચાવી_passwd,
           supported_ચાવી_encryption_algorithms);
      જો(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->ચાવી_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        પાછા CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  જો(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  જો(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  ખાલી, 0, ખાલી);
    જો(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
  }

  પાછા CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
સ્થિર પૂર્ણાંક keylog_callback(gnutls_session_t session, અચલ અક્ષર *label,
                           અચલ gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  જો(crandom.size != 32) {
    પાછા -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  પાછા 0;
}

સ્થિર CURLcode gtls_on_session_reuse(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      સંરચના alpn_spec *alpns,
                                      સંરચના Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  પાછા Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

સ્થિર CURLcode gtls_apply_session(
  સંરચના gtls_ctx *gctx,
  સંરચના Curl_cfilter *cf,
  સંરચના Curl_easy *data,
  સંરચના ssl_peer *peer,
  સંરચના alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  સંરચના Curl_ssl_session *scs,
  bool *pwas_setup)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  પૂર્ણાંક rc;

  જો(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    જો(result)
      જાઓ out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    જો(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    નહીંતર {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      જો(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        જો(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          જો(result)
            જાઓ out;
        }
        જો(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  પાછા result;
}

CURLcode Curl_gtls_ctx_init(સંરચના gtls_ctx *gctx,
                            સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            સંરચના ssl_peer *peer,
                            અચલ સંરચના alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            શૂન્ય *cb_user_data,
                            શૂન્ય *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના Curl_ssl_session *scs = ખાલી;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_ગણતરી = 0;
  bool gtls_session_setup = FALSE;
  સંરચના alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  જો((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    જો(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = ખાલી;
      જો(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  જો(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_ચાવી, &scs);
    જો(result)
      જાઓ out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  જો(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    જો(result)
      જાઓ out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  જો(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    જો(result)
      જાઓ out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  જો(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  જો(!gtls_alpns_ગણતરી && alpns.ગણતરી) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.ગણતરી);
    માટે(i = 0; i < alpns.ગણતરી; ++i) {
      gtls_alpns[i].data = (unsigned અક્ષર *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned પૂર્ણાંક)strlen(alpns.entries[i]);
    }
    gtls_alpns_ગણતરી = alpns.ગણતરી;
  }

  જો(gtls_alpns_ગણતરી &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned પૂર્ણાંક)gtls_alpns_ગણતરી,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_ચાવી, scs);
  પાછા result;
}

સ્થિર CURLcode gtls_connect_step1(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  જો(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    પાછા CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, ખાલી, ખાલી, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#નહીંતર
                              ખાલી
#endif
                              );

  જો(result)
    પાછા result;

  જો(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    સંરચના alpn_proto_buf proto;
    memset(&proto, 0, કદ(proto));
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
  gnutls_transport_set_ધકેલો_function(backend->gtls.session, gtls_ધકેલો);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  પાછા CURLE_OK;
}

સ્થિર CURLcode pkp_pin_peer_pubkey(સંરચના Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    અચલ અક્ષર *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned અક્ષર *buff1 = ખાલી;

  gnutls_pubkey_t ચાવી = ખાલી;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  જો(!pinnedpubkey)
    પાછા CURLE_OK;

  જો(!cert)
    પાછા result;

  કરો {
    પૂર્ણાંક ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&ચાવી);
    જો(ret < 0)
      તોડો; /* failed */

    ret = gnutls_pubkey_import_x509(ચાવી, cert, 0);
    જો(ret < 0)
      તોડો; /* failed */

    ret = gnutls_pubkey_export(ચાવી, GNUTLS_X509_FMT_DER, ખાલી, &len1);
    જો(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      તોડો; /* failed */

    buff1 = curlx_malloc(len1);
    જો(!buff1)
      તોડો; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(ચાવી, GNUTLS_X509_FMT_DER, buff1, &len2);
    જો(ret < 0 || len1 != len2)
      તોડો; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } જ્યારે(0);

  જો(ચાવી)
    gnutls_pubkey_deinit(ચાવી);

  curlx_safefree(buff1);

  પાછા result;
}

શૂન્ય Curl_gtls_report_handshake(સંરચના Curl_easy *data, સંરચના gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  જો(Curl_trc_is_verbose(data)) {
    અચલ અક્ષર *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#નહીંતર
  (શૂન્ય)data;
  (શૂન્ય)gctx;
#endif
}

સ્થિર શૂન્ય gtls_સંદેશ_verify_result(સંરચના Curl_easy *data,
                                   સંરચના ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  અક્ષર certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = કદ(certname);
  પૂર્ણાંક rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  જો(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  જો(!was_verified) {
    જો(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    નહીંતર
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  નહીંતર
    infof(data, "  common name: %s (matched)", certname);
}

સ્થિર શૂન્ય gtls_infof_cert(સંરચના Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  જો(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    પૂર્ણાંક rc, algo;
    time_t tstamp;
    unsigned પૂર્ણાંક bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    જો(rc)
      infof(data, "Failed to get certificate name");
    નહીંતર {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    જો(rc)
      infof(data, "Failed to get certificate issuer");
    નહીંતર {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#નહીંતર
  (શૂન્ય)data;
  (શૂન્ય)x509_cert;
#endif
}

સ્થિર CURLcode gtls_verify_ocsp_status(સંરચના Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = ખાલી;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  પૂર્ણાંક rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  જો(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    જાઓ out;
  }
  નહીંતર જો(rc < 0) {
    failf(data, "Invalid OCSP response received");
    જાઓ out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  જો(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    જાઓ out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  જો(rc < 0) {
    failf(data, "failed to parse OCSP response");
    જાઓ out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  જો(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    જાઓ out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  માટે(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    જો(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      તોડો;
  }
  જો(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    જાઓ out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, ખાલી, ખાલી, ખાલી, ખાલી,
                                   &status, ખાલી, ખાલી, ખાલી, ખાલી);
  જો(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    જાઓ out;
  }
  પસંદગી(status) {
  સ્થિતિ GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    તોડો;
  સ્થિતિ GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    તોડો;
  મૂળભૂત:
    failf(data, "peer certificate OCSP status is not good: %d", (પૂર્ણાંક)status);
    તોડો;
  }

out:
  જો(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  પાછા result;
}

સંરચના gtls_cert_chain {
  અચલ gnutls_datum_t *certs;
  unsigned પૂર્ણાંક અંક_certs;
};

#ifdef USE_APPLE_SECTRUST
સ્થિર CURLcode gtls_chain_get_der(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   શૂન્ય *user_data,
                                   size_t i,
                                   unsigned અક્ષર **pder,
                                   size_t *pder_len)
{
  સંરચના gtls_cert_chain *chain = user_data;

  (શૂન્ય)cf;
  (શૂન્ય)data;
  *pder_len = 0;
  *pder = ખાલી;

  જો(i >= chain->અંક_certs)
    પાછા CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  પાછા CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
સ્થિર CURLcode gtls_verify_cert(સંરચના Curl_easy *data,
                                 સંરચના ssl_filter_config *config,
                                 સંરચના ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 સંરચના Curl_cfilter *cf,
                                 સંરચના ssl_peer *peer,
                                 સંરચના gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned પૂર્ણાંક verify_status = 0;
  long * અચલ certverifyresult = &ssl_config->certverifyresult;
  પૂર્ણાંક rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  જો(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    પાછા CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  જો(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  જો(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    જો(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = ખાલી;
      ocsp_raw.size = 0;
    }
    જો(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      પાછા CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->અંક_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    જો(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      પાછા result; /* no further failure analysis needed */
    verified = !result;
    જો(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#નહીંતર
  (શૂન્ય)cf;
  (શૂન્ય)peer;
  (શૂન્ય)chain;
#endif

  જો(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    અચલ અક્ષર *cause = "certificate error, no details available";
    જો(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    નહીંતર જો(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    નહીંતર જો(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    નહીંતર જો(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    પાછા result;
  }
  પાછા CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                gnutls_session_t session,
                                સંરચના ssl_filter_config *config,
                                સંરચના ssl_easy_config *ssl_config,
                                સંરચના ssl_peer *peer,
                                અચલ અક્ષર *pinned_ચાવી)
{
  સંરચના gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = ખાલી, x509_issuer = ખાલી;
  time_t certclock;
  bool ocsp_checked = FALSE;
  પૂર્ણાંક rc;
  CURLcode result = CURLE_OK;
  long * અચલ certverifyresult = &ssl_config->certverifyresult;

  (શૂન્ય)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.અંક_certs);
  જો(!chain.certs) {
    જો(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      જાઓ out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  જો(data->set.ssl.certinfo && chain.certs) {
    જો(chain.અંક_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.અંક_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ out;
    }
    નહીંતર {
      unsigned પૂર્ણાંક i;

      result = Curl_ssl_init_certinfo(data, (પૂર્ણાંક)chain.અંક_certs);
      જો(result)
        જાઓ out;

      માટે(i = 0; i < chain.અંક_certs; i++) {
        અચલ uint8_t *beg = chain.certs[i].data;
        અચલ uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (પૂર્ણાંક)i, beg, end);
        જો(result)
          જાઓ out;
      }
    }
  }

  જો(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    જો(result)
      જાઓ out;
  }
  નહીંતર
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  જો(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    જાઓ out;
  }

  જો(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    જો(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  જો(certclock == (time_t)-1) {
    જો(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ out;
    }
    નહીંતર
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  નહીંતર {
    જો(certclock < time(ખાલી)) {
      જો(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        જાઓ out;
      }
      નહીંતર
        infof(data, "  SSL certificate expiration date FAILED");
    }
    નહીંતર
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  જો(certclock == (time_t)-1) {
    જો(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ out;
    }
    નહીંતર
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  નહીંતર {
    જો(certclock > time(ખાલી)) {
      જો(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        જાઓ out;
      }
      નહીંતર
        infof(data, "  SSL certificate activation date FAILED");
    }
    નહીંતર
      infof(data, "  SSL certificate activation date OK");
  }

  જો(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    જો(result)
      જાઓ out;
  }
  નહીંતર
    infof(data, "  SSL certificate status verification SKIPPED");

  જો(config->issuercert) {
    gnutls_datum_t issuerp;
    જો(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      જાઓ out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    જો(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      જાઓ out;
    }
    rc = (પૂર્ણાંક)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    જો(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      જાઓ out;
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
  rc = (પૂર્ણાંક)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_સંદેશ_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  જો(result)
    જાઓ out;

  જો(pinned_ચાવી) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_ચાવી);
    જો(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      જાઓ out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  જો(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  જો(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  પાછા result;
}

સ્થિર CURLcode gtls_verifyserver(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  gnutls_session_t session)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  અચલ અક્ષર *pinned_ચાવી = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#નહીંતર
  અચલ અક્ષર *pinned_ચાવી = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_ચાવી);
  જો(result)
    જાઓ out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  જો(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  પાછા result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
સ્થિર CURLcode gtls_send_earlydata(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  અચલ unsigned અક્ષર *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  જ્યારે(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    જો(n < 0) {
      જો(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      નહીંતર
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      જાઓ out;
    }
    નહીંતર જો(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  પાછા result;
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
સ્થિર CURLcode gtls_connect_common(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    bool *done)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  જો(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  જો(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    જો(result)
      જાઓ out;
    connssl->connecting_state = ssl_connect_2;
  }

  જો(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    જો(connssl->earlydata_state == ssl_earlydata_await) {
      જાઓ out;
    }
    નહીંતર જો(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      જો(result)
        જાઓ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    જો(result)
      જાઓ out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  જો(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    પૂર્ણાંક rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    જો(result)
      જાઓ out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    જો(rc) {  /* No ALPN from server */
      proto.data = ખાલી;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    જો(result)
      જાઓ out;

#ifdef CURL_GNUTLS_EARLY_DATA
    જો(connssl->earlydata_state > ssl_earlydata_none) {
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

  જો(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  જો(result == CURLE_AGAIN) {
    *done = FALSE;
    પાછા CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (પૂર્ણાંક)result,
              *done);
  પાછા result;
}

સ્થિર CURLcode gtls_connect(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  સંરચના ssl_connect_data *connssl = cf->ctx;
  જો((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    પાછા CURLE_OK;
  }
#endif
  પાછા gtls_connect_common(cf, data, done);
}

સ્થિર bool gtls_data_pending(સંરચના Curl_cfilter *cf,
                              અચલ સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *ctx = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend;

  (શૂન્ય)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (સંરચના gtls_ssl_backend_data *)ctx->backend;
  જો(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    પાછા TRUE;
  પાછા FALSE;
}

સ્થિર CURLcode gtls_send(સંરચના Curl_cfilter *cf,
                          સંરચના Curl_easy *data,
                          અચલ શૂન્ય *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  જ્યારે(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    જો(nwritten >= 0) {
      જો((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        પાછા CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (અક્ષર *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    નહીંતર {
      જો(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        જાઓ out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      જાઓ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (પૂર્ણાંક)result, *pnwritten);
  પાછા result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
સ્થિર CURLcode gtls_shutdown(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  અક્ષર buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  જો(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    જાઓ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  જો(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    જો(send_shutdown) {
      પૂર્ણાંક ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      જો((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        જાઓ out;
      }
      જો(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((પૂર્ણાંક)ret), (પૂર્ણાંક)ret);
        result = CURLE_RECV_ERROR;
        જાઓ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  માટે(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, કદ(buf));
    જો(nread <= 0)
      તોડો;
  }
  જો(nread > 0) {
    /* still data coming in? */
  }
  નહીંતર જો(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  નહીંતર જો((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  નહીંતર {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((પૂર્ણાંક)nread), (પૂર્ણાંક)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  પાછા result;
}

સ્થિર શૂન્ય gtls_close(સંરચના Curl_cfilter *cf,
                       સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  જો(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = ખાલી;
  }
  જો(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

સ્થિર CURLcode gtls_recv(સંરચના Curl_cfilter *cf,
                          સંરચના Curl_easy *data,
                          અક્ષર *buf, size_t blen,
                          size_t *pnread)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  જો(nread >= 0)
    *pnread = (size_t)nread;
  નહીંતર {
    જો((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      જાઓ out;
    }
    નહીંતર જો(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      જો(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      જાઓ out;
    }
    નહીંતર {
      failf(data, "GnuTLS recv error (%d): %s",
            (પૂર્ણાંક)nread, gnutls_strerror((પૂર્ણાંક)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      જાઓ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (પૂર્ણાંક)result, nread);
  પાછા result;
}

size_t Curl_gtls_version(અક્ષર *buffer, size_t size)
{
  પાછા curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(ખાલી));
}

/* data might be NULL! */
સ્થિર CURLcode gtls_random(સંરચના Curl_easy *data,
                            unsigned અક્ષર *entropy, size_t length)
{
  પૂર્ણાંક rc;
  (શૂન્ય)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  પાછા rc ? CURLE_FAILED_INIT : CURLE_OK;
}

સ્થિર CURLcode gtls_sha256sum(અચલ unsigned અક્ષર *આગત,
                               size_t len,
                               unsigned અક્ષર *sha256sum, /* output */
                               size_t sha256len)
{
  સંરચના sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  કરો {
    unsigned પૂર્ણાંક ilen = (unsigned પૂર્ણાંક)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, આગત);
    len -= ilen;
    આગત += ilen;
  } જ્યારે(len);
#જો NETTLE_VERSION_MAJOR >= 4
  (શૂન્ય)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#નહીંતર
  sha256_digest(&SHA256pw, (unsigned પૂર્ણાંક)sha256len, sha256sum);
#endif
  પાછા CURLE_OK;
}

સ્થિર bool gtls_cert_status_request(શૂન્ય)
{
  પાછા TRUE;
}

સ્થિર શૂન્ય *gtls_get_internals(સંરચના ssl_connect_data *connssl,
                                CURLINFO info)
{
  સંરચના gtls_ssl_backend_data *backend =
    (સંરચના gtls_ssl_backend_data *)connssl->backend;
  (શૂન્ય)info;
  DEBUGASSERT(backend);
  પાછા backend->gtls.session;
}

અચલ સંરચના Curl_ssl Curl_ssl_gnutls = {
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

  કદ(સંરચના gtls_ssl_backend_data),

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
  ખાલી,                          /* close_all */
  ખાલી,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  ખાલી,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
