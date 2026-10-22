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
#यदि 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
स्थिर शून्य tls_log_func(पूर्णाङ्क level, अचर वर्ण *str)
{
  curl_mfprintf(मानक_त्रुटि, "|<%d>| %s", level, str);
}
#endif

#यदि !defined(GNUTLS_VERSION_सङ्ख्या) || (GNUTLS_VERSION_सङ्ख्या < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#यदि GNUTLS_VERSION_सङ्ख्या >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#अन्तर्भाव <gnutls/ocsp.h>

संरचना gtls_ssl_backend_data {
  संरचना gtls_ctx gtls;
};

स्थिर ssize_t gtls_थप(शून्य *s, अचर शून्य *buf, माप_प्रकार blen)
{
  संरचना Curl_cfilter *cf = s;
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  माप_प्रकार nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (पूर्णाङ्क)result, nwritten);
  backend->gtls.io_result = result;
  यदि(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    फर्काऊ -1;
  }
  फर्काऊ (ssize_t)nwritten;
}

स्थिर ssize_t gtls_pull(शून्य *s, शून्य *buf, माप_प्रकार blen)
{
  संरचना Curl_cfilter *cf = s;
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  माप_प्रकार nread;
  CURLcode result;

  DEBUGASSERT(data);
  यदि(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    यदि(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      फर्काऊ -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (पूर्णाङ्क)result,
              nread);
  backend->gtls.io_result = result;
  यदि(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    फर्काऊ -1;
  }
  अन्यथा यदि(nread == 0)
    connssl->peer_closed = TRUE;
  फर्काऊ (ssize_t)nread;
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
  फर्काऊ ret;
}

स्थिर शून्य gtls_cleanup(शून्य)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
स्थिर शून्य showtime(संरचना Curl_easy *data, अचर वर्ण *text, समय_प्रकार stamp)
{
  संरचना पंचांग buffer;
  अचर संरचना पंचांग *पंचांग = &buffer;
  वर्ण str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  यदि(result)
    फर्काऊ;

  curl_msnprintf(str,
                 परिमाण(str),
                 "  %s: %s, %02d %s %4d %02d:%02d:%02d GMT",
                 text,
                 Curl_wkday[पंचांग->tm_wday ? पंचांग->tm_wday-1 : 6],
                 पंचांग->tm_mday,
                 Curl_month[पंचांग->tm_mon],
                 पंचांग->tm_year + 1900,
                 पंचांग->tm_hour,
                 पंचांग->tm_min,
                 पंचांग->tm_sec);
  infof(data, "%s", str);
}
#endif

स्थिर gnutls_datum_t load_file(अचर वर्ण *file)
{
  खाता *f;
  gnutls_datum_t loaded_file = { रिक्त, 0 };
  दीर्घ filelen;
  शून्य *ptr;

  f = curlx_fopen(file, "rb");
  यदि(!f)
    फर्काऊ loaded_file;
  यदि(ख_जाओ(f, 0, अंत_से))
    गच्छ out;
  filelen = ख_बताओ(f);
  यदि(filelen < 0 || filelen > CURL_MAX_आगत_LENGTH)
    गच्छ out;
  यदि(ख_जाओ(f, 0, शुरू_से))
    गच्छ out;
  ptr = curlx_malloc((माप_प्रकार)filelen);
  यदि(!ptr)
    गच्छ out;
  यदि(ख_पढ़ो(ptr, 1, (माप_प्रकार)filelen, f) < (माप_प्रकार)filelen) {
    curlx_free(ptr);
    गच्छ out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (अचिन्हित पूर्णाङ्क)filelen;
out:
  curlx_fclose(f);
  फर्काऊ loaded_file;
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

  यदि(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    यदि(result)
      फर्काऊ result;
  }

  यदि((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    फर्काऊ CURLE_AGAIN;
  }
  अन्यथा यदि((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    अचर वर्ण *strerr = रिक्त;

    यदि(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    यदि(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    फर्काऊ CURLE_AGAIN;
  }
  अन्यथा यदि((rc < 0) && backend->gtls.io_result) {
    फर्काऊ backend->gtls.io_result;
  }
  अन्यथा यदि(rc < 0) {
    अचर वर्ण *strerr = रिक्त;

    यदि(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    यदि(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    फर्काऊ CURLE_SSL_CONNECT_ERROR;
  }

  फर्काऊ CURLE_OK;
}

स्थिर gnutls_x509_crt_fmt_t gnutls_do_file_type(अचर वर्ण *type)
{
  यदि(!type || !type[0])
    फर्काऊ GNUTLS_X509_FMT_PEM;
  यदि(curl_strequal(type, "PEM"))
    फर्काऊ GNUTLS_X509_FMT_PEM;
  यदि(curl_strequal(type, "DER"))
    फर्काऊ GNUTLS_X509_FMT_DER;
  फर्काऊ GNUTLS_X509_FMT_PEM; /* default to PEM */
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
  अचर वर्ण **prioritylist,
  bool tls13support)
{
  दीर्घ ssl_version = conn_config->version;
  दीर्घ ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  यदि(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  यदि((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  यदि(peer->transport == TRNSPRT_QUIC) {
    यदि(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      फर्काऊ CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    फर्काऊ CURLE_OK;
  }

  विकल्प(ssl_version | ssl_version_max) {
  पक्ष CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    फर्काऊ CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    फर्काऊ CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    फर्काऊ CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    फर्काऊ CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    फर्काऊ CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    फर्काऊ CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    फर्काऊ CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    फर्काऊ CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    फर्काऊ CURLE_OK;
  पक्ष CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    फर्काऊ CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  फर्काऊ CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(संरचना Curl_easy *data,
                                       संरचना gtls_shared_creds **pcreds)
{
  संरचना gtls_shared_creds *shared;
  पूर्णाङ्क rc;

  *pcreds = रिक्त;
  shared = curlx_calloc(1, परिमाण(*shared));
  यदि(!shared)
    फर्काऊ CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  यदि(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    फर्काऊ CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->समय = *Curl_pgrs_now(data);
  *pcreds = shared;
  फर्काऊ CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(संरचना gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  यदि(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    फर्काऊ CURLE_OK;
  }
  फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
}

शून्य Curl_gtls_shared_creds_free(संरचना gtls_shared_creds **pcreds)
{
  संरचना gtls_shared_creds *shared = *pcreds;
  *pcreds = रिक्त;
  यदि(shared) {
    --shared->refcount;
    यदि(!shared->refcount) {
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

  यदि(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    फर्काऊ CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  यदि(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#अन्यथा
    rc = gnutls_certificate_set_x509_system_trust(creds);
    यदि(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    अन्यथा {
      infof(data, "  Native: %d certificates from system trust", rc);
      यदि(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  यदि(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    यदि(config->ca_info_blob->len > (माप_प्रकार)अच_पूर्णांक_उच्च) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      फर्काऊ CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (अचिन्हित पूर्णाङ्क)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    यदि(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      यदि(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        फर्काऊ CURLE_SSL_CACERT_BADFILE;
      }
    }
    अन्यथा
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  अन्यथा यदि(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    यदि(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      यदि(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        फर्काऊ CURLE_SSL_CACERT_BADFILE;
      }
    }
    अन्यथा
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  यदि(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    यदि(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      यदि(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        फर्काऊ CURLE_SSL_CACERT_BADFILE;
      }
    }
    अन्यथा
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  यदि(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  यदि(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    फर्काऊ CURLE_NOT_BUILT_IN;
  }
#endif
  यदि(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    यदि(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      फर्काऊ CURLE_SSL_CRL_BADFILE;
    }
    अन्यथा
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  फर्काऊ CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_साँचो   "tls:gtls:x509:share"

स्थिर bool gtls_shared_creds_expired(संरचना Curl_easy *data,
                                      अचर संरचना gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->समय);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  यदि(timeout_ms < 0)
    फर्काऊ FALSE;

  फर्काऊ elapsed_ms >= timeout_ms;
}

स्थिर bool gtls_shared_creds_different(संरचना Curl_cfilter *cf,
                                        अचर संरचना gtls_shared_creds *sc)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  यदि(!sc->CAfile || !conn_config->CAfile)
    फर्काऊ sc->CAfile != conn_config->CAfile;

  फर्काऊ म_भेद(sc->CAfile, conn_config->CAfile);
}

स्थिर संरचना gtls_shared_creds *gtls_get_cached_creds(संरचना Curl_cfilter *cf,
                                                       संरचना Curl_easy *data)
{
  संरचना gtls_shared_creds *shared_creds;

  यदि(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_साँचो,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_साँचो));
    यदि(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      फर्काऊ shared_creds;
    }
  }
  फर्काऊ रिक्त;
}

स्थिर शून्य gtls_shared_creds_hash_free(अचर शून्य *साँचो,
                                        माप_प्रकार साँचो_len, शून्य *p)
{
  संरचना gtls_shared_creds *sc = p;
  DEBUGASSERT(साँचो_len == CURL_CSTRLEN(MPROTO_GTLS_X509_साँचो));
  DEBUGASSERT(!स_भेद(MPROTO_GTLS_X509_साँचो, साँचो, साँचो_len));
  (शून्य)साँचो;
  (शून्य)साँचो_len;
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
  यदि(!data->multi)
    फर्काऊ;

  यदि(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    यदि(!sc->CAfile)
      फर्काऊ;
  }

  यदि(Curl_gtls_shared_creds_up_ref(sc))
    फर्काऊ;

  यदि(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_साँचो,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_साँचो),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    फर्काऊ;
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

  यदि(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  यदि(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    यदि(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      फर्काऊ CURLE_SSL_CONNECT_ERROR;
    }
  }
  अन्यथा {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    यदि(result)
      फर्काऊ result;
    gtls->shared_creds->trust_setup = TRUE;
    यदि(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  फर्काऊ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
स्थिर पूर्णाङ्क gtls_get_ietf_proto(gnutls_session_t session)
{
  विकल्प(gnutls_protocol_get_version(session)) {
  पक्ष GNUTLS_SSL3:
    फर्काऊ CURL_IETF_PROTO_SSL3;
  पक्ष GNUTLS_TLS1_0:
    फर्काऊ CURL_IETF_PROTO_TLS1;
  पक्ष GNUTLS_TLS1_1:
    फर्काऊ CURL_IETF_PROTO_TLS1_1;
  पक्ष GNUTLS_TLS1_2:
    फर्काऊ CURL_IETF_PROTO_TLS1_2;
  पक्ष GNUTLS_TLS1_3:
    फर्काऊ CURL_IETF_PROTO_TLS1_3;
  स्वभाव:
    फर्काऊ CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 अचर वर्ण *ssl_peer_साँचो,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 अचर वर्ण *alpn,
                                 अचिन्हित वर्ण *quic_tp,
                                 माप_प्रकार quic_tp_len,
                                 संरचना Curl_ssl_session **psession)
{
  संरचना Curl_ssl_session *sc_session = रिक्त, *sc_dup = रिक्त;
  अचिन्हित वर्ण *sdata, *qtp_clone = रिक्त;
  माप_प्रकार sdata_len = 0;
  माप_प्रकार earlydata_max = 0;
  CURLcode result = CURLE_OK;

  यदि(psession)
    *psession = रिक्त;
  यदि(!Curl_ssl_scache_use(cf, data))
    फर्काऊ CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, रिक्त, &sdata_len);
  यदि(!sdata_len) /* GnuTLS does this for some version combinations */
    फर्काऊ CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  यदि(!sdata)
    फर्काऊ CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  यदि(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((अचर वर्ण *)quic_tp, quic_tp_len);
    यदि(!qtp_clone) {
      curlx_free(sdata);
      फर्काऊ CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  यदि(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  यदि(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_साँचो, sc_session);
    /* took ownership of `sc_session` */
    sc_session = रिक्त;
  }
  यदि(!result && psession) {
    *psession = sc_dup;
    sc_dup = रिक्त;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  फर्काऊ result;
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

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_साँचो,
                                   session, 0, connssl->negotiated.alpn,
                                   रिक्त, 0, &scs);
  यदि(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  फर्काऊ result;
}

स्थिर पूर्णाङ्क gtls_handshake_cb(gnutls_session_t session, अचिन्हित पूर्णाङ्क htype,
                             अचिन्हित when, अचिन्हित पूर्णाङ्क incoming,
                             अचर gnutls_datum_t *सन्देश)
{
  संरचना Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (शून्य)सन्देश;
  (शून्य)incoming;
  यदि(when) { /* after message has been processed */
    संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
    यदि(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      विकल्प(htype) {
      पक्ष GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        रोक;
      }
      स्वभाव:
        रोक;
      }
    }
  }
  फर्काऊ 0;
}
#endif

स्थिर CURLcode gtls_set_priority(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना gtls_ctx *gtls,
                                  अचर वर्ण *priority)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना dynbuf buf;
  अचर वर्ण *err = रिक्त;
  CURLcode result = CURLE_OK;
  पूर्णाङ्क rc;

  curlx_dyn_init(&buf, 4096);

  यदि(conn_config->cipher_list) {
    यदि((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      यदि(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        यदि(result)
          गच्छ out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      यदि(result)
        गच्छ out;
      priority = curlx_dyn_ptr(&buf);
    }
    अन्यथा /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  यदि(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  फर्काऊ result;
}

स्थिर CURLcode gtls_client_init(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना ssl_peer *peer,
                                 माप_प्रकार earlydata_max,
                                 संरचना gtls_ctx *gtls)
{
  संरचना ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  अचिन्हित पूर्णाङ्क init_flags;
  पूर्णाङ्क rc;
  अचर वर्ण *prioritylist;
  bool tls13support;
  CURLcode result;

  यदि(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    फर्काऊ CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  यदि(result)
    फर्काऊ result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  यदि(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  अन्यथा यदि(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  यदि(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  यदि(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    फर्काऊ CURLE_SSL_CONNECT_ERROR;
  }

  यदि(peer->sni) {
    यदि(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, म_माप(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      फर्काऊ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  यदि(rc != GNUTLS_E_SUCCESS)
    फर्काऊ CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  यदि(config->version == CURL_SSLVERSION_TLSv1_3) {
    यदि(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      फर्काऊ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  यदि(result)
    फर्काऊ result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  यदि(result)
    फर्काऊ result;

  यदि(config->clientcert) {
    यदि(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      यदि(result)
        फर्काऊ result;
    }
    यदि(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->साँचो_passwd ? config->साँचो_passwd : "");
      यदि(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        फर्काऊ CURLE_SSL_CONNECT_ERROR;
      }
    }
    अन्यथा {
      अचर अचिन्हित पूर्णाङ्क supported_साँचो_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_साँचो_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->साँचो ? config->साँचो : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->साँचो_passwd,
           supported_साँचो_encryption_algorithms);
      यदि(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->साँचो_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        फर्काऊ CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  यदि(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    फर्काऊ CURLE_SSL_CONNECT_ERROR;
  }

  यदि(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  रिक्त, 0, रिक्त);
    यदि(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      फर्काऊ CURLE_SSL_CONNECT_ERROR;
    }
  }

  फर्काऊ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
स्थिर पूर्णाङ्क keylog_callback(gnutls_session_t session, अचर वर्ण *label,
                           अचर gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  यदि(crandom.size != 32) {
    फर्काऊ -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  फर्काऊ 0;
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
  फर्काऊ Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
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

  यदि(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    यदि(result)
      गच्छ out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    यदि(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    अन्यथा {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      यदि(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        यदि(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          यदि(result)
            गच्छ out;
        }
        यदि(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  फर्काऊ result;
}

CURLcode Curl_gtls_ctx_init(संरचना gtls_ctx *gctx,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            संरचना ssl_peer *peer,
                            अचर संरचना alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            शून्य *cb_user_data,
                            शून्य *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_session *scs = रिक्त;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  माप_प्रकार gtls_alpns_गन्ती = 0;
  bool gtls_session_setup = FALSE;
  संरचना alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  यदि((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    यदि(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = रिक्त;
      यदि(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  यदि(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_साँचो, &scs);
    यदि(result)
      गच्छ out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  यदि(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    यदि(result)
      गच्छ out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  यदि(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    यदि(result)
      गच्छ out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  यदि(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  यदि(!gtls_alpns_गन्ती && alpns.गन्ती) {
    माप_प्रकार i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.गन्ती);
    लागि(i = 0; i < alpns.गन्ती; ++i) {
      gtls_alpns[i].data = (अचिन्हित वर्ण *)alpns.entries[i];
      gtls_alpns[i].size = (अचिन्हित पूर्णाङ्क)म_माप(alpns.entries[i]);
    }
    gtls_alpns_गन्ती = alpns.गन्ती;
  }

  यदि(gtls_alpns_गन्ती &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (अचिन्हित पूर्णाङ्क)gtls_alpns_गन्ती,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_साँचो, scs);
  फर्काऊ result;
}

स्थिर CURLcode gtls_connect_step1(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  यदि(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    फर्काऊ CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, रिक्त, रिक्त, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#अन्यथा
                              रिक्त
#endif
                              );

  यदि(result)
    फर्काऊ result;

  यदि(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    संरचना alpn_proto_buf proto;
    स_रखो(&proto, 0, परिमाण(proto));
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
  gnutls_transport_set_थप_function(backend->gtls.session, gtls_थप);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  फर्काऊ CURLE_OK;
}

स्थिर CURLcode pkp_pin_peer_pubkey(संरचना Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    अचर वर्ण *pinnedpubkey)
{
  /* Scratch */
  माप_प्रकार len1 = 0, len2 = 0;
  अचिन्हित वर्ण *buff1 = रिक्त;

  gnutls_pubkey_t साँचो = रिक्त;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  यदि(!pinnedpubkey)
    फर्काऊ CURLE_OK;

  यदि(!cert)
    फर्काऊ result;

  गर {
    पूर्णाङ्क ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&साँचो);
    यदि(ret < 0)
      रोक; /* failed */

    ret = gnutls_pubkey_import_x509(साँचो, cert, 0);
    यदि(ret < 0)
      रोक; /* failed */

    ret = gnutls_pubkey_export(साँचो, GNUTLS_X509_FMT_DER, रिक्त, &len1);
    यदि(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      रोक; /* failed */

    buff1 = curlx_malloc(len1);
    यदि(!buff1)
      रोक; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(साँचो, GNUTLS_X509_FMT_DER, buff1, &len2);
    यदि(ret < 0 || len1 != len2)
      रोक; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } जबसम्म(0);

  यदि(साँचो)
    gnutls_pubkey_deinit(साँचो);

  curlx_safefree(buff1);

  फर्काऊ result;
}

शून्य Curl_gtls_report_handshake(संरचना Curl_easy *data, संरचना gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  यदि(Curl_trc_is_verbose(data)) {
    अचर वर्ण *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#अन्यथा
  (शून्य)data;
  (शून्य)gctx;
#endif
}

स्थिर शून्य gtls_सन्देश_verify_result(संरचना Curl_easy *data,
                                   संरचना ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  वर्ण certname[65] = ""; /* limited to 64 chars by ASN.1 */
  माप_प्रकार size = परिमाण(certname);
  पूर्णाङ्क rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  यदि(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  यदि(!was_verified) {
    यदि(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    अन्यथा
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  अन्यथा
    infof(data, "  common name: %s (matched)", certname);
}

स्थिर शून्य gtls_infof_cert(संरचना Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  यदि(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    पूर्णाङ्क rc, algo;
    समय_प्रकार tstamp;
    अचिन्हित पूर्णाङ्क bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    यदि(rc)
      infof(data, "Failed to get certificate name");
    अन्यथा {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    यदि(rc)
      infof(data, "Failed to get certificate issuer");
    अन्यथा {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#अन्यथा
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
  यदि(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    गच्छ out;
  }
  अन्यथा यदि(rc < 0) {
    failf(data, "Invalid OCSP response received");
    गच्छ out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  यदि(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    गच्छ out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  यदि(rc < 0) {
    failf(data, "failed to parse OCSP response");
    गच्छ out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  यदि(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    गच्छ out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  लागि(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    यदि(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      रोक;
  }
  यदि(rc < 0) {
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
  यदि(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    गच्छ out;
  }
  विकल्प(status) {
  पक्ष GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    रोक;
  पक्ष GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    रोक;
  स्वभाव:
    failf(data, "peer certificate OCSP status is not good: %d", (पूर्णाङ्क)status);
    रोक;
  }

out:
  यदि(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  फर्काऊ result;
}

संरचना gtls_cert_chain {
  अचर gnutls_datum_t *certs;
  अचिन्हित पूर्णाङ्क अङ्क_certs;
};

#ifdef USE_APPLE_SECTRUST
स्थिर CURLcode gtls_chain_get_der(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   शून्य *user_data,
                                   माप_प्रकार i,
                                   अचिन्हित वर्ण **pder,
                                   माप_प्रकार *pder_len)
{
  संरचना gtls_cert_chain *chain = user_data;

  (शून्य)cf;
  (शून्य)data;
  *pder_len = 0;
  *pder = रिक्त;

  यदि(i >= chain->अङ्क_certs)
    फर्काऊ CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (माप_प्रकार)chain->certs[i].size;
  फर्काऊ CURLE_OK;
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
  अचिन्हित पूर्णाङ्क verify_status = 0;
  दीर्घ * अचर certverifyresult = &ssl_config->certverifyresult;
  पूर्णाङ्क rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  यदि(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    फर्काऊ CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  यदि(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  यदि(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    यदि(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = रिक्त;
      ocsp_raw.size = 0;
    }
    यदि(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      फर्काऊ CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->अङ्क_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    यदि(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      फर्काऊ result; /* no further failure analysis needed */
    verified = !result;
    यदि(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#अन्यथा
  (शून्य)cf;
  (शून्य)peer;
  (शून्य)chain;
#endif

  यदि(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    अचर वर्ण *cause = "certificate error, no details available";
    यदि(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    अन्यथा यदि(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    अन्यथा यदि(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    अन्यथा यदि(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    फर्काऊ result;
  }
  फर्काऊ CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                gnutls_session_t session,
                                संरचना ssl_filter_config *config,
                                संरचना ssl_easy_config *ssl_config,
                                संरचना ssl_peer *peer,
                                अचर वर्ण *pinned_साँचो)
{
  संरचना gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = रिक्त, x509_issuer = रिक्त;
  समय_प्रकार certclock;
  bool ocsp_checked = FALSE;
  पूर्णाङ्क rc;
  CURLcode result = CURLE_OK;
  दीर्घ * अचर certverifyresult = &ssl_config->certverifyresult;

  (शून्य)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.अङ्क_certs);
  यदि(!chain.certs) {
    यदि(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      गच्छ out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  यदि(data->set.ssl.certinfo && chain.certs) {
    यदि(chain.अङ्क_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.अङ्क_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    अन्यथा {
      अचिन्हित पूर्णाङ्क i;

      result = Curl_ssl_init_certinfo(data, (पूर्णाङ्क)chain.अङ्क_certs);
      यदि(result)
        गच्छ out;

      लागि(i = 0; i < chain.अङ्क_certs; i++) {
        अचर uint8_t *beg = chain.certs[i].data;
        अचर uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (पूर्णाङ्क)i, beg, end);
        यदि(result)
          गच्छ out;
      }
    }
  }

  यदि(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    यदि(result)
      गच्छ out;
  }
  अन्यथा
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  यदि(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    गच्छ out;
  }

  यदि(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    यदि(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  यदि(certclock == (समय_प्रकार)-1) {
    यदि(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    अन्यथा
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  अन्यथा {
    यदि(certclock < समय(रिक्त)) {
      यदि(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        गच्छ out;
      }
      अन्यथा
        infof(data, "  SSL certificate expiration date FAILED");
    }
    अन्यथा
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  यदि(certclock == (समय_प्रकार)-1) {
    यदि(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    अन्यथा
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  अन्यथा {
    यदि(certclock > समय(रिक्त)) {
      यदि(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        गच्छ out;
      }
      अन्यथा
        infof(data, "  SSL certificate activation date FAILED");
    }
    अन्यथा
      infof(data, "  SSL certificate activation date OK");
  }

  यदि(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    यदि(result)
      गच्छ out;
  }
  अन्यथा
    infof(data, "  SSL certificate status verification SKIPPED");

  यदि(config->issuercert) {
    gnutls_datum_t issuerp;
    यदि(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      गच्छ out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    यदि(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      गच्छ out;
    }
    rc = (पूर्णाङ्क)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    यदि(rc <= 0) {
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
  gtls_सन्देश_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  यदि(result)
    गच्छ out;

  यदि(pinned_साँचो) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_साँचो);
    यदि(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      गच्छ out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  यदि(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  यदि(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  फर्काऊ result;
}

स्थिर CURLcode gtls_verifyserver(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  gnutls_session_t session)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  अचर वर्ण *pinned_साँचो = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#अन्यथा
  अचर वर्ण *pinned_साँचो = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_साँचो);
  यदि(result)
    गच्छ out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  यदि(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  फर्काऊ result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
स्थिर CURLcode gtls_send_earlydata(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  अचर अचिन्हित वर्ण *buf;
  माप_प्रकार blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  जबसम्म(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    यदि(n < 0) {
      यदि(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      अन्यथा
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      गच्छ out;
    }
    अन्यथा यदि(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (माप_प्रकार)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  फर्काऊ result;
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
  यदि(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    फर्काऊ CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  यदि(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    यदि(result)
      गच्छ out;
    connssl->connecting_state = ssl_connect_2;
  }

  यदि(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    यदि(connssl->earlydata_state == ssl_earlydata_await) {
      गच्छ out;
    }
    अन्यथा यदि(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      यदि(result)
        गच्छ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    यदि(result)
      गच्छ out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  यदि(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    पूर्णाङ्क rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    यदि(result)
      गच्छ out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    यदि(rc) {  /* No ALPN from server */
      proto.data = रिक्त;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    यदि(result)
      गच्छ out;

#ifdef CURL_GNUTLS_EARLY_DATA
    यदि(connssl->earlydata_state > ssl_earlydata_none) {
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

  यदि(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  यदि(result == CURLE_AGAIN) {
    *done = FALSE;
    फर्काऊ CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (पूर्णाङ्क)result,
              *done);
  फर्काऊ result;
}

स्थिर CURLcode gtls_connect(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  संरचना ssl_connect_data *connssl = cf->ctx;
  यदि((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    फर्काऊ CURLE_OK;
  }
#endif
  फर्काऊ gtls_connect_common(cf, data, done);
}

स्थिर bool gtls_data_pending(संरचना Curl_cfilter *cf,
                              अचर संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *ctx = cf->ctx;
  संरचना gtls_ssl_backend_data *backend;

  (शून्य)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (संरचना gtls_ssl_backend_data *)ctx->backend;
  यदि(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    फर्काऊ TRUE;
  फर्काऊ FALSE;
}

स्थिर CURLcode gtls_send(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *data,
                          अचर शून्य *buf,
                          माप_प्रकार blen,
                          माप_प्रकार *pnwritten)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  माप_प्रकार remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  जबसम्म(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    यदि(nwritten >= 0) {
      यदि((माप_प्रकार)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        फर्काऊ CURLE_SEND_ERROR;
      }
      *pnwritten += (माप_प्रकार)nwritten;
      buf = (वर्ण *)CURL_UNCONST(buf) + (माप_प्रकार)nwritten;
      remain -= (माप_प्रकार)nwritten;
    }
    अन्यथा {
      यदि(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
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
  फर्काऊ result;
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
  माप_प्रकार i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  यदि(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    गच्छ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  यदि(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    यदि(send_shutdown) {
      पूर्णाङ्क ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      यदि((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        गच्छ out;
      }
      यदि(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((पूर्णाङ्क)ret), (पूर्णाङ्क)ret);
        result = CURLE_RECV_ERROR;
        गच्छ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  लागि(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, परिमाण(buf));
    यदि(nread <= 0)
      रोक;
  }
  यदि(nread > 0) {
    /* still data coming in? */
  }
  अन्यथा यदि(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  अन्यथा यदि((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  अन्यथा {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((पूर्णाङ्क)nread), (पूर्णाङ्क)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  फर्काऊ result;
}

स्थिर शून्य gtls_close(संरचना Curl_cfilter *cf,
                       संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  यदि(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = रिक्त;
  }
  यदि(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

स्थिर CURLcode gtls_recv(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *data,
                          वर्ण *buf, माप_प्रकार blen,
                          माप_प्रकार *pnread)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  यदि(nread >= 0)
    *pnread = (माप_प्रकार)nread;
  अन्यथा {
    यदि((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      गच्छ out;
    }
    अन्यथा यदि(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      यदि(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      गच्छ out;
    }
    अन्यथा {
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
  फर्काऊ result;
}

माप_प्रकार Curl_gtls_version(वर्ण *buffer, माप_प्रकार size)
{
  फर्काऊ curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(रिक्त));
}

/* data might be NULL! */
स्थिर CURLcode gtls_random(संरचना Curl_easy *data,
                            अचिन्हित वर्ण *entropy, माप_प्रकार length)
{
  पूर्णाङ्क rc;
  (शून्य)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  फर्काऊ rc ? CURLE_FAILED_INIT : CURLE_OK;
}

स्थिर CURLcode gtls_sha256sum(अचर अचिन्हित वर्ण *आगत,
                               माप_प्रकार len,
                               अचिन्हित वर्ण *sha256sum, /* output */
                               माप_प्रकार sha256len)
{
  संरचना sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  गर {
    अचिन्हित पूर्णाङ्क ilen = (अचिन्हित पूर्णाङ्क)CURLMIN(len, अच_पूर्णांक_उच्च);
    sha256_update(&SHA256pw, ilen, आगत);
    len -= ilen;
    आगत += ilen;
  } जबसम्म(len);
#यदि NETTLE_VERSION_MAJOR >= 4
  (शून्य)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#अन्यथा
  sha256_digest(&SHA256pw, (अचिन्हित पूर्णाङ्क)sha256len, sha256sum);
#endif
  फर्काऊ CURLE_OK;
}

स्थिर bool gtls_cert_status_request(शून्य)
{
  फर्काऊ TRUE;
}

स्थिर शून्य *gtls_get_internals(संरचना ssl_connect_data *connssl,
                                CURLINFO info)
{
  संरचना gtls_ssl_backend_data *backend =
    (संरचना gtls_ssl_backend_data *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  फर्काऊ backend->gtls.session;
}

अचर संरचना Curl_ssl Curl_ssl_gnutls = {
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
