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

#यदि_परिभाषितम् USE_GNUTLS

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
#परिभाषय GTLSDEBUG
#यदि_अन्तः

#यदि_परिभाषितम् GTLSDEBUG
स्थिर शून्य tls_log_func(पूर्णाङ्क level, ध्रुव वर्ण *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#यदि_अन्तः

#यदि !defined(GNUTLS_संस्करणम्_सङ्ख्या) || (GNUTLS_संस्करणम्_सङ्ख्या < 0x030605)
#दोषः "too old GnuTLS version"
#यदि_अन्तः

#अपरिभाषय CURL_GNUTLS_EARLY_दत्त
#यदि GNUTLS_संस्करणम्_सङ्ख्या >= 0x030702
#परिभाषय CURL_GNUTLS_EARLY_दत्त
#यदि_अन्तः

#अन्तर्भाव <gnutls/ocsp.h>

संरचना gtls_ssl_backend_दत्त {
  संरचना gtls_ctx gtls;
};

स्थिर ssize_t gtls_प्रक्षिप(शून्य *s, ध्रुव शून्य *buf, size_t blen)
{
  संरचना Curl_cfilter *cf = s;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  size_t nwritten;
  CURLcode परिणाम;

  DEBUGASSERT(दत्त);
  परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(दत्त, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (पूर्णाङ्क)परिणाम, nwritten);
  backend->gtls.io_परिणाम = परिणाम;
  यदि(परिणाम) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_समुच्चयः_errno(backend->gtls.session,
                               (परिणाम == CURLE_AGAIN) ? EAGAIN : EINVAL);
    प्रत्यागम -1;
  }
  प्रत्यागम (ssize_t)nwritten;
}

स्थिर ssize_t gtls_pull(शून्य *s, शून्य *buf, size_t blen)
{
  संरचना Curl_cfilter *cf = s;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  size_t nread;
  CURLcode परिणाम;

  DEBUGASSERT(दत्त);
  यदि(!backend->gtls.shared_creds->trust_सज्जता) {
    परिणाम = Curl_gtls_client_trust_सज्जता(cf, दत्त, &backend->gtls);
    यदि(परिणाम) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_समुच्चयः_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_परिणाम = परिणाम;
      प्रत्यागम -1;
    }
  }

  परिणाम = Curl_conn_cf_recv(cf->अग्र, दत्त, buf, blen, &nread);
  CURL_TRC_CF(दत्त, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (पूर्णाङ्क)परिणाम,
              nread);
  backend->gtls.io_परिणाम = परिणाम;
  यदि(परिणाम) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_समुच्चयः_errno(backend->gtls.session,
                               (परिणाम == CURLE_AGAIN) ? EAGAIN : EINVAL);
    प्रत्यागम -1;
  }
  अन्यथा यदि(nread == 0)
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
#यदि_परिभाषितम् GTLSDEBUG
  gnutls_global_समुच्चयः_log_function(tls_log_func);
  gnutls_global_समुच्चयः_log_level(2);
#यदि_अन्तः
  प्रत्यागम ret;
}

स्थिर शून्य gtls_cleanup(शून्य)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#यदि_परिभाषितम् CURLVERBOSE
स्थिर शून्य showtime(संरचना Curl_easy *दत्त, ध्रुव वर्ण *text, time_t stamp)
{
  संरचना tm बफर;
  ध्रुव संरचना tm *tm = &बफर;
  वर्ण str[96];
  CURLcode परिणाम = curlx_gmtime(stamp, &बफर);
  यदि(परिणाम)
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
  infof(दत्त, "%s", str);
}
#यदि_अन्तः

स्थिर gnutls_datum_t आरोपय_file(ध्रुव वर्ण *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { रिक्त, 0 };
  दीर्घ filelen;
  शून्य *ptr;

  f = curlx_fopen(file, "rb");
  यदि(!f)
    प्रत्यागम loaded_file;
  यदि(fseek(f, 0, SEEK_END))
    गच्छ बहिर्गमनम्;
  filelen = ftell(f);
  यदि(filelen < 0 || filelen > CURL_MAX_निवेशः_दीर्घता)
    गच्छ बहिर्गमनम्;
  यदि(fseek(f, 0, SEEK_SET))
    गच्छ बहिर्गमनम्;
  ptr = curlx_malloc((size_t)filelen);
  यदि(!ptr)
    गच्छ बहिर्गमनम्;
  यदि(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    गच्छ बहिर्गमनम्;
  }

  loaded_file.दत्त = ptr;
  loaded_file.आकार = (अचिह्नित पूर्णाङ्क)filelen;
बहिर्गमनम्:
  curlx_fclose(f);
  प्रत्यागम loaded_file;
}

स्थिर शून्य unload_file(gnutls_datum_t दत्त)
{
  curlx_free(दत्त.दत्त);
}

/* this function does an SSL/TLS (re-)handshake */
स्थिर CURLcode cf_gtls_handshake(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  gnutls_session_t session;
  पूर्णाङ्क rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_परिणाम = CURLE_OK;
  rc = gnutls_handshake(session);

  यदि(!backend->gtls.shared_creds->trust_सज्जता) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode परिणाम = Curl_gtls_client_trust_सज्जता(cf, दत्त, &backend->gtls);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_लेखः_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    प्रत्यागम CURLE_AGAIN;
  }
  अन्यथा यदि((rc < 0) && !gnutls_error_is_fatal(rc)) {
#यदि_परिभाषितम् CURLVERBOSE
    ध्रुव वर्ण *strerr = रिक्त;

    यदि(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    यदि(!strerr)
      strerr = gnutls_strerror(rc);

    infof(दत्त, "gnutls_handshake() warning: %s", strerr);
#यदि_अन्तः
    प्रत्यागम CURLE_AGAIN;
  }
  अन्यथा यदि((rc < 0) && backend->gtls.io_परिणाम) {
    प्रत्यागम backend->gtls.io_परिणाम;
  }
  अन्यथा यदि(rc < 0) {
    ध्रुव वर्ण *strerr = रिक्त;

    यदि(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    यदि(!strerr)
      strerr = gnutls_strerror(rc);

    failf(दत्त, "GnuTLS, handshake failed: %s", strerr);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  प्रत्यागम CURLE_OK;
}

स्थिर gnutls_x509_crt_fmt_t gnutls_do_file_type(ध्रुव वर्ण *type)
{
  यदि(!type || !type[0])
    प्रत्यागम GNUTLS_X509_FMT_PEM;
  यदि(curl_strequal(type, "PEM"))
    प्रत्यागम GNUTLS_X509_FMT_PEM;
  यदि(curl_strequal(type, "DER"))
    प्रत्यागम GNUTLS_X509_FMT_DER;
  प्रत्यागम GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#परिभाषय GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#परिभाषय QUIC_प्राथमिकता \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

स्थिर CURLcode gnutls_समुच्चयः_ssl_संस्करणम्_min_max(
  संरचना Curl_easy *दत्त,
  संरचना ssl_peer *peer,
  संरचना ssl_filter_config *conn_config,
  ध्रुव वर्ण **prioritylist,
  bool tls13support)
{
  दीर्घ ssl_संस्करणम् = conn_config->संस्करणम्_2;
  दीर्घ ssl_संस्करणम्_max = conn_config->संस्करणम्_max;

  DEBUGASSERT(ssl_संस्करणम् != CURL_SSLVERSION_DEFAULT);
  यदि(ssl_संस्करणम् <= CURL_SSLVERSION_TLSv1)
    ssl_संस्करणम् = CURL_SSLVERSION_TLSv1_0;
  यदि((ssl_संस्करणम्_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_संस्करणम्_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_संस्करणम्_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  यदि(peer->transport == TRNSPRT_QUIC) {
    यदि(ssl_संस्करणम्_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(दत्त, "QUIC needs at least TLS version 1.3");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_प्राथमिकता;
    प्रत्यागम CURLE_OK;
  }

  विकल्प(ssl_संस्करणम् | ssl_संस्करणम्_max) {
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

  failf(दत्त, "GnuTLS: cannot set TLS protocol");
  प्रत्यागम CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(संरचना Curl_easy *दत्त,
                                       संरचना gtls_shared_creds **pcreds)
{
  संरचना gtls_shared_creds *shared;
  पूर्णाङ्क rc;

  *pcreds = रिक्त;
  shared = curlx_calloc(1, परिमाण(*shared));
  यदि(!shared)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  यदि(rc != GNUTLS_E_SUCCESS) {
    failf(दत्त, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(दत्त);
  *pcreds = shared;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_ऊर्ध्वम्_ref(संरचना gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  यदि(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    प्रत्यागम CURLE_OK;
  }
  प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
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
                                    संरचना Curl_easy *दत्त,
                                    gnutls_certificate_credentials_t creds)
{
  संरचना ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, दत्त);
  bool creds_are_रिक्तानि = TRUE;
  पूर्णाङ्क rc;

  यदि(!config->verifypeer) {
    infof(दत्त, "SSL Trust: peer verification disabled");
    प्रत्यागम CURLE_OK;
  }

  infof(दत्त, "SSL Trust Anchors:");
  यदि(config->native_ca_store) {
#यदि_परिभाषितम् USE_APPLE_SECTRUST
    infof(दत्त, "  Native: Apple SecTrust");
    creds_are_रिक्तानि = FALSE;
#अन्यथा
    rc = gnutls_certificate_समुच्चयः_x509_system_trust(creds);
    यदि(rc < 0)
      infof(दत्त, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    अन्यथा {
      infof(दत्त, "  Native: %d certificates from system trust", rc);
      यदि(rc > 0)
        creds_are_रिक्तानि = FALSE;
    }
#यदि_अन्तः
  }

  यदि(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    यदि(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(दत्त, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      प्रत्यागम CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.दत्त = config->ca_info_blob->दत्त;
    ca_info_datum.आकार = (अचिह्नित पूर्णाङ्क)config->ca_info_blob->len;
    rc = gnutls_certificate_समुच्चयः_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_रिक्तानि = creds_are_रिक्तानि && (rc <= 0);
    यदि(rc < 0) {
      infof(दत्त, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_रिक्तानि ? "" : ", continuing anyway"));
      यदि(creds_are_रिक्तानि) {
        ssl_config->certverifyresult = rc;
        प्रत्यागम CURLE_SSL_CACERT_BADFILE;
      }
    }
    अन्यथा
      infof(दत्त, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  अन्यथा यदि(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_समुच्चयः_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_समुच्चयः_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_रिक्तानि = creds_are_रिक्तानि && (rc <= 0);
    यदि(rc < 0) {
      infof(दत्त, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_रिक्तानि ? "" : ", continuing anyway"));
      यदि(creds_are_रिक्तानि) {
        ssl_config->certverifyresult = rc;
        प्रत्यागम CURLE_SSL_CACERT_BADFILE;
      }
    }
    अन्यथा
      infof(दत्त, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  यदि(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_समुच्चयः_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_रिक्तानि = creds_are_रिक्तानि && (rc <= 0);
    यदि(rc < 0) {
      infof(दत्त, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_रिक्तानि ? "" : ", continuing anyway"));
      यदि(creds_are_रिक्तानि) {
        ssl_config->certverifyresult = rc;
        प्रत्यागम CURLE_SSL_CACERT_BADFILE;
      }
    }
    अन्यथा
      infof(दत्त, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  यदि(creds_are_रिक्तानि)
    infof(दत्त, "  no trust anchors configured");

#यदि_परिभाषितम् USE_APPLE_SECTRUST
  यदि(config->CRLfile && config->native_ca_store) {
    failf(दत्त, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
  }
#यदि_अन्तः
  यदि(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_समुच्चयः_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    यदि(rc < 0) {
      failf(दत्त, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      प्रत्यागम CURLE_SSL_CRL_BADFILE;
    }
    अन्यथा
      infof(दत्त, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  प्रत्यागम CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#परिभाषय MPROTO_GTLS_X509_कुञ्चिका   "tls:gtls:x509:share"

स्थिर bool gtls_shared_creds_expired(संरचना Curl_easy *दत्त,
                                      ध्रुव संरचना gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(दत्त), &sc->time);
  timediff_t timeout_ms = दत्त->समुच्चयः.ssl_ca_cache_timeout * (timediff_t)1000;

  यदि(timeout_ms < 0)
    प्रत्यागम FALSE;

  प्रत्यागम elapsed_ms >= timeout_ms;
}

स्थिर bool gtls_shared_creds_different(संरचना Curl_cfilter *cf,
                                        ध्रुव संरचना gtls_shared_creds *sc)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  यदि(!sc->CAfile || !conn_config->CAfile)
    प्रत्यागम sc->CAfile != conn_config->CAfile;

  प्रत्यागम वाक्यं_तुलय(sc->CAfile, conn_config->CAfile);
}

स्थिर संरचना gtls_shared_creds *gtls_get_cached_creds(संरचना Curl_cfilter *cf,
                                                       संरचना Curl_easy *दत्त)
{
  संरचना gtls_shared_creds *shared_creds;

  यदि(दत्त->multi) {
    shared_creds = Curl_hash_pick(&दत्त->multi->proto_hash,
                                  MPROTO_GTLS_X509_कुञ्चिका,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_कुञ्चिका));
    यदि(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(दत्त, shared_creds) &&
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

स्थिर शून्य gtls_समुच्चयः_cached_creds(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना gtls_shared_creds *sc)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  यदि(!दत्त->multi)
    प्रत्यागम;

  यदि(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    यदि(!sc->CAfile)
      प्रत्यागम;
  }

  यदि(Curl_gtls_shared_creds_ऊर्ध्वम्_ref(sc))
    प्रत्यागम;

  यदि(!Curl_hash_add2(&दत्त->multi->proto_hash,
                     MPROTO_GTLS_X509_कुञ्चिका,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_कुञ्चिका),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    प्रत्यागम;
  }
}

CURLcode Curl_gtls_client_trust_सज्जता(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      संरचना gtls_ctx *gtls)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना gtls_shared_creds *cached_creds = रिक्त;
  bool cache_criteria_met;
  CURLcode परिणाम;
  पूर्णाङ्क rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (दत्त->समुच्चयः.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  यदि(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, दत्त);

  यदि(cached_creds && !Curl_gtls_shared_creds_ऊर्ध्वम्_ref(cached_creds)) {
    CURL_TRC_CF(दत्त, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_समुच्चयः(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    यदि(rc != GNUTLS_E_SUCCESS) {
      failf(दत्त, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }
  अन्यथा {
    CURL_TRC_CF(दत्त, cf, "loading trust anchors and CRLs");
    परिणाम = gtls_populate_creds(cf, दत्त, gtls->shared_creds->creds);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    gtls->shared_creds->trust_सज्जता = TRUE;
    यदि(cache_criteria_met)
      gtls_समुच्चयः_cached_creds(cf, दत्त, gtls->shared_creds);
  }
  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
स्थिर पूर्णाङ्क gtls_get_ietf_proto(gnutls_session_t session)
{
  विकल्प(gnutls_protocol_get_संस्करणम्(session)) {
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
                                 संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ध्रुव वर्ण *alpn,
                                 अचिह्नित वर्ण *quic_tp,
                                 size_t quic_tp_len,
                                 संरचना Curl_ssl_session **psession)
{
  संरचना Curl_ssl_session *sc_session = रिक्त, *sc_dup = रिक्त;
  अचिह्नित वर्ण *sdata, *qtp_clone = रिक्त;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode परिणाम = CURLE_OK;

  यदि(psession)
    *psession = रिक्त;
  यदि(!Curl_ssl_scache_use(cf, दत्त))
    प्रत्यागम CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_दत्त(session, रिक्त, &sdata_len);
  यदि(!sdata_len) /* GnuTLS does this for some version combinations */
    प्रत्यागम CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  यदि(!sdata)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_दत्त(session, sdata, &sdata_len);
  earlydata_max = gnutls_लेखः_get_max_early_दत्त_आकार(session);

  CURL_TRC_CF(दत्त, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  यदि(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ध्रुव वर्ण *)quic_tp, quic_tp_len);
    यदि(!qtp_clone) {
      curlx_free(sdata);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }

  परिणाम = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  यदि(!परिणाम && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    परिणाम = Curl_ssl_session_dup(sc_session, &sc_dup);
  यदि(!परिणाम) {
    परिणाम = Curl_ssl_scache_put(cf, दत्त, ssl_peer_कुञ्चिका, sc_session);
    /* took ownership of `sc_session` */
    sc_session = रिक्त;
  }
  यदि(!परिणाम && psession) {
    *psession = sc_dup;
    sc_dup = रिक्त;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
स्थिर CURLcode cf_gtls_update_session_id(संरचना Curl_cfilter *cf,
                                          संरचना Curl_easy *दत्त,
                                          gnutls_session_t session)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना Curl_ssl_session *scs = रिक्त;
  CURLcode परिणाम;

  परिणाम = Curl_gtls_cache_session(cf, दत्त, connssl->peer.scache_कुञ्चिका,
                                   session, 0, connssl->negotiated.alpn,
                                   रिक्त, 0, &scs);
  यदि(!परिणाम) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क gtls_handshake_cb(gnutls_session_t session, अचिह्नित पूर्णाङ्क htype,
                             अचिह्नित when, अचिह्नित पूर्णाङ्क incoming,
                             ध्रुव gnutls_datum_t *सन्देशः)
{
  संरचना Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (शून्य)सन्देशः;
  (शून्य)incoming;
  यदि(when) { /* after message has been processed */
    संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
    यदि(दत्त) {
      CURL_TRC_CF(दत्त, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      विकल्प(htype) {
      पक्ष GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, दत्त, session);
        भङ्ग;
      }
      स्वभाव:
        भङ्ग;
      }
    }
  }
  प्रत्यागम 0;
}
#यदि_अन्तः

स्थिर CURLcode gtls_समुच्चयः_प्राथमिकता(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना gtls_ctx *gtls,
                                  ध्रुव वर्ण *प्राथमिकता)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना dynbuf buf;
  ध्रुव वर्ण *err = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क rc;

  curlx_dyn_init(&buf, 4096);

  यदि(conn_config->cipher_list) {
    यदि((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      यदि(!curlx_dyn_len(&buf)) {  /* not added yet */
        परिणाम = curlx_dyn_सङ्कलय(&buf, प्राथमिकता);
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
      }
      परिणाम = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      प्राथमिकता = curlx_dyn_ptr(&buf);
    }
    अन्यथा /* replace our own completely */
      प्राथमिकता = conn_config->cipher_list;
  }

  infof(दत्त, "GnuTLS priority: %s", प्राथमिकता);
  rc = gnutls_प्राथमिकता_समुच्चयः_प्रत्यक्षम्(gtls->session, प्राथमिकता, &err);
  यदि(rc != GNUTLS_E_SUCCESS) {
    failf(दत्त, "Error %d setting GnuTLS priority: %s", rc, err);
    परिणाम = CURLE_SSL_CONNECT_ERROR;
  }

बहिर्गमनम्:
  curlx_dyn_free(&buf);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode gtls_client_init(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना ssl_peer *peer,
                                 size_t earlydata_max,
                                 संरचना gtls_ctx *gtls)
{
  संरचना ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, दत्त);
  अचिह्नित पूर्णाङ्क init_flags;
  पूर्णाङ्क rc;
  ध्रुव वर्ण *prioritylist;
  bool tls13support;
  CURLcode परिणाम;

  यदि(config->संस्करणम्_2 == CURL_SSLVERSION_SSLv2 ||
     config->संस्करणम्_2 == CURL_SSLVERSION_SSLv3) {
    failf(दत्त, "GnuTLS does not support SSLv2 or SSLv3");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  परिणाम = Curl_gtls_shared_creds_create(दत्त, &gtls->shared_creds);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
  यदि(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_दत्त | GNUTLS_NO_अन्तः_सम्बन्धि_EARLY_दत्त;
  अन्यथा यदि(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_दत्त;
#यदि_अन्तः

#यदि_परिभाषितम् GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#यदि_अन्तः

#यदि_परिभाषितम् GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#यदि_अन्तः

#यदि_परिभाषितम् GNUTLS_NO_स्थितिः_REQUEST
  यदि(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_स्थितिः_REQUEST;
#यदि_अन्तः

  CURL_TRC_CF(दत्त, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  यदि(rc != GNUTLS_E_SUCCESS) {
    failf(दत्त, "gnutls_init() failed: %d", rc);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  यदि(peer->sni) {
    यदि(gnutls_server_name_समुच्चयः(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, वाक्यदैर्घ्यम्(peer->sni)) < 0) {
      failf(दत्त, "Failed to set SNI");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_समुच्चयः_default_प्राथमिकता(gtls->session);
  यदि(rc != GNUTLS_E_SUCCESS)
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_संस्करणम्("3.6.5");

  यदि(config->संस्करणम्_2 == CURL_SSLVERSION_TLSv1_3) {
    यदि(!tls13support) {
      failf(दत्त, "This GnuTLS installation does not support TLS 1.3");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  परिणाम = gnutls_समुच्चयः_ssl_संस्करणम्_min_max(दत्त, peer,
                                          config, &prioritylist, tls13support);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = gtls_समुच्चयः_प्राथमिकता(cf, दत्त, gtls, prioritylist);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(config->clientcert) {
    यदि(!gtls->shared_creds->trust_सज्जता) {
      परिणाम = Curl_gtls_client_trust_सज्जता(cf, दत्त, gtls);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    यदि(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_समुच्चयः_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->कुञ्चिका_passwd ? config->कुञ्चिका_passwd : "");
      यदि(rc != GNUTLS_E_SUCCESS) {
        failf(दत्त,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
    }
    अन्यथा {
      ध्रुव अचिह्नित पूर्णाङ्क supported_कुञ्चिका_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_समुच्चयः_x509_कुञ्चिका_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->कुञ्चिका ? config->कुञ्चिका : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->कुञ्चिका_passwd,
           supported_कुञ्चिका_encryption_algorithms);
      यदि(rc != GNUTLS_E_SUCCESS) {
        failf(दत्त,
              "error reading X.509 %skey file: %s",
              config->कुञ्चिका_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_समुच्चयः(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  यदि(rc != GNUTLS_E_SUCCESS) {
    failf(दत्त, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  यदि(config->verifystatus) {
    rc = gnutls_ocsp_स्थितिः_request_enable_client(gtls->session,
                                                  रिक्त, 0, रिक्त);
    यदि(rc != GNUTLS_E_SUCCESS) {
      failf(दत्त, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
  }

  प्रत्यागम CURLE_OK;
}

#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
स्थिर पूर्णाङ्क keylog_callback(gnutls_session_t session, ध्रुव वर्ण *label,
                           ध्रुव gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  यदि(crandom.आकार != 32) {
    प्रत्यागम -1;
  }

  Curl_tls_keylog_write(label, crandom.दत्त, crandom.आकार,
                        secret->दत्त, secret->आकार);
  प्रत्यागम 0;
}

स्थिर CURLcode gtls_on_session_reuse(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      संरचना alpn_spec *alpns,
                                      संरचना Curl_ssl_session *scs,
                                      bool *do_early_दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;

  connssl->earlydata_max =
    gnutls_लेखः_get_max_early_दत्त_आकार(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  प्रत्यागम Curl_on_session_reuse(cf, दत्त, alpns, scs, do_early_दत्त,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#यदि_अन्तः

स्थिर CURLcode gtls_apply_session(
  संरचना gtls_ctx *gctx,
  संरचना Curl_cfilter *cf,
  संरचना Curl_easy *दत्त,
  संरचना ssl_peer *peer,
  संरचना alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  संरचना Curl_ssl_session *scs,
  bool *pwas_सज्जता)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क rc;

  यदि(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_अन्तर्भवति_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    परिणाम = gtls_client_init(cf, दत्त, peer, scs->earlydata_max, gctx);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    *pwas_सज्जता = TRUE;

    rc = gnutls_session_समुच्चयः_दत्त(gctx->session, scs->sdata, scs->sdata_len);
    यदि(rc < 0)
      infof(दत्त, "SSL session not accepted by GnuTLS, continuing without");
    अन्यथा {
      infof(दत्त, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      यदि(conn_config->earlydata && scs->alpn &&
         !cf->conn->द्वयङ्काः.connect_only) {
        bool do_early_दत्त = FALSE;
        यदि(sess_reuse_cb) {
          परिणाम = sess_reuse_cb(cf, दत्त, alpns, scs, &do_early_दत्त);
          यदि(परिणाम)
            गच्छ बहिर्गमनम्;
        }
        यदि(do_early_दत्त) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_गन्तव्यम्(alpns, scs->alpn);
        }
      }
    }
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

CURLcode Curl_gtls_ctx_init(संरचना gtls_ctx *gctx,
                            संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            संरचना ssl_peer *peer,
                            ध्रुव संरचना alpn_spec *alpns_requested,
                            Curl_gtls_ctx_सज्जता_cb *cb_सज्जता,
                            शून्य *cb_user_दत्त,
                            शून्य *ssl_user_दत्त,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_session *scs = रिक्त;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_गणनम् = 0;
  bool gtls_session_सज्जता = FALSE;
  संरचना alpn_spec alpns;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  यदि((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(दत्त, cf->cft, FIRSTSOCKET);
    यदि(scs) {
      परिणाम = gtls_apply_session(gctx, cf, दत्त, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_सज्जता);
      scs = रिक्त;
      यदि(!परिणाम && gtls_session_सज्जता) {
        CURL_TRC_CF(दत्त, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  यदि(!gtls_session_सज्जता && conn_config->cache_session &&
     !conn_config->verifystatus) {
    परिणाम = Curl_ssl_scache_take(cf, दत्त, peer->scache_कुञ्चिका, &scs);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    परिणाम = gtls_apply_session(gctx, cf, दत्त, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_सज्जता);
  }

  यदि(!gtls_session_सज्जता) {
    परिणाम = gtls_client_init(cf, दत्त, peer, 0, gctx);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  gnutls_session_समुच्चयः_ptr(gctx->session, ssl_user_दत्त);

  यदि(cb_सज्जता) {
    परिणाम = cb_सज्जता(cf, दत्त, cb_user_दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  यदि(Curl_tls_keylog_enabled()) {
    gnutls_session_समुच्चयः_keylog_function(gctx->session, keylog_callback);
  }
#यदि_अन्तः

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  यदि(!gtls_alpns_गणनम् && alpns.गणनम्) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.गणनम्);
    प्रति(i = 0; i < alpns.गणनम्; ++i) {
      gtls_alpns[i].दत्त = (अचिह्नित वर्ण *)alpns.entries[i];
      gtls_alpns[i].आकार = (अचिह्नित पूर्णाङ्क)वाक्यदैर्घ्यम्(alpns.entries[i]);
    }
    gtls_alpns_गणनम् = alpns.गणनम्;
  }

  यदि(gtls_alpns_गणनम् &&
     gnutls_alpn_समुच्चयः_protocols(gctx->session,
                               gtls_alpns, (अचिह्नित पूर्णाङ्क)gtls_alpns_गणनम्,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(दत्त, "failed setting ALPN");
    परिणाम = CURLE_SSL_CONNECT_ERROR;
  }

बहिर्गमनम्:
  Curl_ssl_scache_return(cf, दत्त, peer->scache_कुञ्चिका, scs);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode gtls_connect_step1(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम;

  DEBUGASSERT(backend);

  यदि(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    प्रत्यागम CURLE_OK;

  परिणाम = Curl_gtls_ctx_init(&backend->gtls, cf, दत्त, &connssl->peer,
                              connssl->alpn, रिक्त, रिक्त, cf,
#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
                              gtls_on_session_reuse
#अन्यथा
                              रिक्त
#यदि_अन्तः
                              );

  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    संरचना alpn_proto_buf proto;
    स्मृतिं_पूरय(&proto, 0, परिमाण(proto));
    Curl_alpn_गन्तव्यम्_proto_str(&proto, connssl->alpn);
    infof(दत्त, VTLS_INFOF_ALPN_OFFER_1STR, proto.दत्त);
  }

#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
  gnutls_handshake_समुच्चयः_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#यदि_अन्तः

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_समुच्चयः_ptr(backend->gtls.session, cf);
  gnutls_transport_समुच्चयः_प्रक्षिप_function(backend->gtls.session, gtls_प्रक्षिप);
  gnutls_transport_समुच्चयः_pull_function(backend->gtls.session, gtls_pull);

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode pkp_pin_peer_pubkey(संरचना Curl_easy *दत्त,
                                    gnutls_x509_crt_t cert,
                                    ध्रुव वर्ण *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  अचिह्नित वर्ण *buff1 = रिक्त;

  gnutls_pubkey_t कुञ्चिका = रिक्त;

  /* Result is returned to caller */
  CURLcode परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  यदि(!pinnedpubkey)
    प्रत्यागम CURLE_OK;

  यदि(!cert)
    प्रत्यागम परिणाम;

  कुरु {
    पूर्णाङ्क ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&कुञ्चिका);
    यदि(ret < 0)
      भङ्ग; /* failed */

    ret = gnutls_pubkey_import_x509(कुञ्चिका, cert, 0);
    यदि(ret < 0)
      भङ्ग; /* failed */

    ret = gnutls_pubkey_export(कुञ्चिका, GNUTLS_X509_FMT_DER, रिक्त, &len1);
    यदि(ret != GNUTLS_E_SHORT_स्मृतिः_बफर || len1 == 0)
      भङ्ग; /* failed */

    buff1 = curlx_malloc(len1);
    यदि(!buff1)
      भङ्ग; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(कुञ्चिका, GNUTLS_X509_FMT_DER, buff1, &len2);
    यदि(ret < 0 || len1 != len2)
      भङ्ग; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    परिणाम = Curl_pin_peer_pubkey(दत्त, pinnedpubkey, buff1, len1);
  } यावत्(0);

  यदि(कुञ्चिका)
    gnutls_pubkey_deinit(कुञ्चिका);

  curlx_safefree(buff1);

  प्रत्यागम परिणाम;
}

शून्य Curl_gtls_report_handshake(संरचना Curl_easy *दत्त, संरचना gtls_ctx *gctx)
{
#यदि_परिभाषितम् CURLVERBOSE
  यदि(Curl_trc_is_verbose(दत्त)) {
    ध्रुव वर्ण *ptr;
    gnutls_protocol_t संस्करणम्_2 = gnutls_protocol_get_संस्करणम्(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(दत्त, "SSL connection using %s / %s",
          gnutls_protocol_get_name(संस्करणम्_2), ptr);
  }
#अन्यथा
  (शून्य)दत्त;
  (शून्य)gctx;
#यदि_अन्तः
}

स्थिर शून्य gtls_सन्देशः_verify_परिणाम(संरचना Curl_easy *दत्त,
                                   संरचना ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  वर्ण certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t आकार = परिमाण(certname);
  पूर्णाङ्क rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &आकार);
  यदि(rc) {
    infof(दत्त, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  यदि(!was_verified) {
    यदि(needs_verified) {
      failf(दत्त, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    अन्यथा
      infof(दत्त, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  अन्यथा
    infof(दत्त, "  common name: %s (matched)", certname);
}

स्थिर शून्य gtls_infof_cert(संरचना Curl_easy *दत्त,
                            gnutls_x509_crt_t x509_cert)
{
#यदि_परिभाषितम् CURLVERBOSE
  यदि(Curl_trc_is_verbose(दत्त)) {
    gnutls_datum_t certfields;
    पूर्णाङ्क rc, algo;
    time_t tstamp;
    अचिह्नित पूर्णाङ्क द्वयङ्काः;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &द्वयङ्काः);
    infof(दत्त, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(दत्त, "  certificate version: #%d",
          gnutls_x509_crt_get_संस्करणम्(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    यदि(rc)
      infof(दत्त, "Failed to get certificate name");
    अन्यथा {
      infof(दत्त, "  subject: %s", certfields.दत्त);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(दत्त, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(दत्त, "expire date", tstamp);

      gnutls_free(certfields.दत्त);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    यदि(rc)
      infof(दत्त, "Failed to get certificate issuer");
    अन्यथा {
      infof(दत्त, "  issuer: %s", certfields.दत्त);

      gnutls_free(certfields.दत्त);
    }
  }
#अन्यथा
  (शून्य)दत्त;
  (शून्य)x509_cert;
#यदि_अन्तः
}

स्थिर CURLcode gtls_verify_ocsp_स्थितिः(संरचना Curl_easy *दत्त,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = रिक्त;
  CURLcode परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
  पूर्णाङ्क rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_स्थितिः_t स्थितिः = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_स्थितिः_request_get(session, &ocsp_raw);
  यदि(rc == GNUTLS_E_REQUESTED_दत्त_NOT_उपलब्धम्) {
    failf(दत्त, "No OCSP response received");
    गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(rc < 0) {
    failf(दत्त, "Invalid OCSP response received");
    गच्छ बहिर्गमनम्;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  यदि(rc < 0) {
    failf(दत्त, "failed to init OCSP response parsing");
    गच्छ बहिर्गमनम्;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  यदि(rc < 0) {
    failf(दत्त, "failed to parse OCSP response");
    गच्छ बहिर्गमनम्;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  यदि(gnutls_ocsp_resp_get_स्थितिः(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(दत्त, "OCSP response basic part is not successful");
    गच्छ बहिर्गमनम्;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  प्रति(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    यदि(!rc || (rc == GNUTLS_E_REQUESTED_दत्त_NOT_उपलब्धम्))
      भङ्ग;
  }
  यदि(rc < 0) {
    failf(दत्त, "OCSP does not carry status for peer cert");
    गच्छ बहिर्गमनम्;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, रिक्त, रिक्त, रिक्त, रिक्त,
                                   &स्थितिः, रिक्त, रिक्त, रिक्त, रिक्त);
  यदि(rc < 0) {
    failf(दत्त, "failed to parse OCSP response for peer cert");
    गच्छ बहिर्गमनम्;
  }
  विकल्प(स्थितिः) {
  पक्ष GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    परिणाम = CURLE_OK;
    भङ्ग;
  पक्ष GNUTLS_OCSP_CERT_REVOKED:
    failf(दत्त, "peer certificate is revoked");
    भङ्ग;
  स्वभाव:
    failf(दत्त, "peer certificate OCSP status is not good: %d", (पूर्णाङ्क)स्थितिः);
    भङ्ग;
  }

बहिर्गमनम्:
  यदि(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  प्रत्यागम परिणाम;
}

संरचना gtls_cert_chain {
  ध्रुव gnutls_datum_t *certs;
  अचिह्नित पूर्णाङ्क अङ्कः_certs;
};

#यदि_परिभाषितम् USE_APPLE_SECTRUST
स्थिर CURLcode gtls_chain_get_der(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   शून्य *user_दत्त,
                                   size_t i,
                                   अचिह्नित वर्ण **pder,
                                   size_t *pder_len)
{
  संरचना gtls_cert_chain *chain = user_दत्त;

  (शून्य)cf;
  (शून्य)दत्त;
  *pder_len = 0;
  *pder = रिक्त;

  यदि(i >= chain->अङ्कः_certs)
    प्रत्यागम CURLE_TOO_LARGE;
  *pder = chain->certs[i].दत्त;
  *pder_len = (size_t)chain->certs[i].आकार;
  प्रत्यागम CURLE_OK;
}
#यदि_अन्तः /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
स्थिर CURLcode gtls_verify_cert(संरचना Curl_easy *दत्त,
                                 संरचना ssl_filter_config *config,
                                 संरचना ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 संरचना Curl_cfilter *cf,
                                 संरचना ssl_peer *peer,
                                 संरचना gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  अचिह्नित पूर्णाङ्क verify_स्थितिः = 0;
  दीर्घ * ध्रुव certverifyresult = &ssl_config->certverifyresult;
  पूर्णाङ्क rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_स्थितिः);
  यदि(rc < 0) {
    failf(दत्त, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_स्थितिः;
  verified = !(verify_स्थितिः & GNUTLS_CERT_INVALID);
  यदि(verified)
    infof(दत्त, "  SSL certificate verified by GnuTLS");

#यदि_परिभाषितम् USE_APPLE_SECTRUST
  यदि(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode परिणाम;

    यदि(gnutls_ocsp_स्थितिः_request_get(session, &ocsp_raw)) {
      ocsp_raw.दत्त = रिक्त;
      ocsp_raw.आकार = 0;
    }
    यदि(config->verifystatus && !ocsp_raw.आकार) {
      failf(दत्त, "No OCSP response received");
      प्रत्यागम CURLE_SSL_INVALIDCERTSTATUS;
    }
    परिणाम = Curl_vtls_apple_verify(cf, दत्त, peer, chain->अङ्कः_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.दत्त, ocsp_raw.आकार,
                                    config->verifystatus);
    यदि(परिणाम && (परिणाम != CURLE_PEER_FAILED_VERIFICATION))
      प्रत्यागम परिणाम; /* no further failure analysis needed */
    verified = !परिणाम;
    यदि(verified) {
      infof(दत्त, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#अन्यथा
  (शून्य)cf;
  (शून्य)peer;
  (शून्य)chain;
#यदि_अन्तः

  यदि(!verified) {
    CURLcode परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ध्रुव वर्ण *cause = "certificate error, no details available";
    यदि(verify_स्थितिः & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    अन्यथा यदि(verify_स्थितिः & GNUTLS_CERT_SIGNER_NOT_प्राप्तम्)
      cause = "certificate signer not trusted";
    अन्यथा यदि(verify_स्थितिः & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    अन्यथा यदि(verify_स्थितिः & GNUTLS_CERT_INVALID_OCSP_स्थितिः) {
      cause = "OCSP status response is invalid";
      परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(दत्त, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    प्रत्यागम परिणाम;
  }
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
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
  CURLcode परिणाम = CURLE_OK;
  दीर्घ * ध्रुव certverifyresult = &ssl_config->certverifyresult;

  (शून्य)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.अङ्कः_certs);
  यदि(!chain.certs) {
    यदि(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(दत्त, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_प्राप्तम्;
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
      गच्छ बहिर्गमनम्;
    }
    infof(दत्त, " common name: WARNING could not obtain");
  }

  यदि(दत्त->समुच्चयः.ssl.certinfo && chain.certs) {
    यदि(chain.अङ्कः_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(दत्त, "%u certificates is more than allowed (%d)",
            chain.अङ्कः_certs, MAX_ALLOWED_CERT_AMOUNT);
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा {
      अचिह्नित पूर्णाङ्क i;

      परिणाम = Curl_ssl_init_certinfo(दत्त, (पूर्णाङ्क)chain.अङ्कः_certs);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;

      प्रति(i = 0; i < chain.अङ्कः_certs; i++) {
        ध्रुव uint8_t *beg = chain.certs[i].दत्त;
        ध्रुव uint8_t *अन्तः = beg + chain.certs[i].आकार;

        परिणाम = Curl_extract_certinfo(दत्त, (पूर्णाङ्क)i, beg, अन्तः);
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
      }
    }
  }

  यदि(config->verifypeer) {
    परिणाम = gtls_verify_cert(दत्त, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  अन्यथा
    infof(दत्त, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  यदि(gnutls_x509_crt_init(&x509_cert)) {
    failf(दत्त, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_प्राप्तम्;
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    गच्छ बहिर्गमनम्;
  }

  यदि(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    यदि(rc) {
      failf(दत्त, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_प्राप्तम्;
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  यदि(certclock == (time_t)-1) {
    यदि(config->verifypeer) {
      failf(दत्त, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा
      infof(दत्त, "  SSL certificate expiration date verify FAILED");
  }
  अन्यथा {
    यदि(certclock < time(रिक्त)) {
      यदि(config->verifypeer) {
        failf(दत्त, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;
        गच्छ बहिर्गमनम्;
      }
      अन्यथा
        infof(दत्त, "  SSL certificate expiration date FAILED");
    }
    अन्यथा
      infof(दत्त, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  यदि(certclock == (time_t)-1) {
    यदि(config->verifypeer) {
      failf(दत्त, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा
      infof(दत्त, "  SSL certificate activation date verify FAILED");
  }
  अन्यथा {
    यदि(certclock > time(रिक्त)) {
      यदि(config->verifypeer) {
        failf(दत्त, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;
        गच्छ बहिर्गमनम्;
      }
      अन्यथा
        infof(दत्त, "  SSL certificate activation date FAILED");
    }
    अन्यथा
      infof(दत्त, "  SSL certificate activation date OK");
  }

  यदि(config->verifystatus && !ocsp_checked) {
    परिणाम = gtls_verify_ocsp_स्थितिः(दत्त, session, x509_cert);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  अन्यथा
    infof(दत्त, "  SSL certificate status verification SKIPPED");

  यदि(config->issuercert) {
    gnutls_datum_t issuerp;
    यदि(gnutls_x509_crt_init(&x509_issuer)) {
      failf(दत्त, "failed to init GnuTLS x509_crt for issuer");
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      गच्छ बहिर्गमनम्;
    }
    issuerp = आरोपय_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    यदि(rc) {
      failf(दत्त, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      गच्छ बहिर्गमनम्;
    }
    rc = (पूर्णाङ्क)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    यदि(rc <= 0) {
      failf(दत्त, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      गच्छ बहिर्गमनम्;
    }
    infof(दत्त, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  परिणाम = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_सन्देशः_verify_परिणाम(दत्त, peer, x509_cert, rc, config->verifyhost);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(pinned_कुञ्चिका) {
    परिणाम = pkp_pin_peer_pubkey(दत्त, x509_cert, pinned_कुञ्चिका);
    यदि(परिणाम != CURLE_OK) {
      failf(दत्त, "SSL: public key does not match pinned public key");
      गच्छ बहिर्गमनम्;
    }
  }

  gtls_infof_cert(दत्त, x509_cert);

बहिर्गमनम्:
  यदि(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  यदि(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode gtls_verifyserver(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  gnutls_session_t session)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, दत्त);
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  ध्रुव वर्ण *pinned_कुञ्चिका = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY);
#अन्यथा
  ध्रुव वर्ण *pinned_कुञ्चिका = CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY);
#यदि_अन्तः
  CURLcode परिणाम;

  परिणाम = Curl_gtls_verifyserver(cf, दत्त, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_कुञ्चिका);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  यदि(gnutls_protocol_get_संस्करणम्(session) < GNUTLS_TLS1_3)
    परिणाम = cf_gtls_update_session_id(cf, दत्त, session);
#यदि_अन्तः

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
स्थिर CURLcode gtls_send_earlydata(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  ध्रुव अचिह्नित वर्ण *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_परिणाम = CURLE_OK;
  यावत्(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_लेखः_send_early_दत्त(backend->gtls.session, buf, blen);
    CURL_TRC_CF(दत्त, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    यदि(n < 0) {
      यदि(n == GNUTLS_E_AGAIN)
        परिणाम = CURLE_AGAIN;
      अन्यथा
        परिणाम = backend->gtls.io_परिणाम ?
                 backend->gtls.io_परिणाम : CURLE_SEND_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(दत्त, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}
#यदि_अन्तः

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
                                    संरचना Curl_easy *दत्त,
                                    bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  यदि(ssl_connection_complete == connssl->state) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;

  /* Initiate the connection, if not already done */
  यदि(connssl->connecting_state == ssl_connect_1) {
    परिणाम = gtls_connect_step1(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    connssl->connecting_state = ssl_connect_2;
  }

  यदि(connssl->connecting_state == ssl_connect_2) {
#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
    यदि(connssl->earlydata_state == ssl_earlydata_await) {
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(connssl->earlydata_state == ssl_earlydata_sending) {
      परिणाम = gtls_send_earlydata(cf, दत्त);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      connssl->earlydata_state = ssl_earlydata_प्रेषितम्;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_प्रेषितम्));
#यदि_अन्तः
    परिणाम = cf_gtls_handshake(cf, दत्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  यदि(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    पूर्णाङ्क rc;

    Curl_gtls_report_handshake(दत्त, &backend->gtls);

    परिणाम = gtls_verifyserver(cf, दत्त, backend->gtls.session);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    यदि(rc) {  /* No ALPN from server */
      proto.दत्त = रिक्त;
      proto.आकार = 0;
    }

    परिणाम = Curl_alpn_समुच्चयः_negotiated(cf, दत्त, connssl,
                                      proto.दत्त, proto.आकार);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;

#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
    यदि(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_प्रेषितम्);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_दत्त) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#यदि_अन्तः
    connssl->connecting_state = ssl_connect_समाप्तम्;
  }

  यदि(connssl->connecting_state == ssl_connect_समाप्तम्)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

बहिर्गमनम्:
  यदि(परिणाम == CURLE_AGAIN) {
    *समाप्तम् = FALSE;
    प्रत्यागम CURLE_OK;
  }
  *समाप्तम् = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(दत्त, cf, "gtls_connect_common() -> %d, done=%d", (पूर्णाङ्क)परिणाम,
              *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode gtls_connect(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             bool *समाप्तम्)
{
#यदि_परिभाषितम् CURL_GNUTLS_EARLY_दत्त
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  यदि((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }
#यदि_अन्तः
  प्रत्यागम gtls_connect_common(cf, दत्त, समाप्तम्);
}

स्थिर bool gtls_दत्त_pending(संरचना Curl_cfilter *cf,
                              ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *ctx = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend;

  (शून्य)दत्त;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (संरचना gtls_ssl_backend_दत्त *)ctx->backend;
  यदि(backend->gtls.session &&
     gnutls_लेखः_check_pending(backend->gtls.session) != 0)
    प्रत्यागम TRUE;
  प्रत्यागम FALSE;
}

स्थिर CURLcode gtls_send(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *दत्त,
                          ध्रुव शून्य *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  यावत्(remain) {
    backend->gtls.io_परिणाम = CURLE_OK;
    nwritten = gnutls_लेखः_send(backend->gtls.session, buf, remain);

    यदि(nwritten >= 0) {
      यदि((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(दत्त, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        प्रत्यागम CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (वर्ण *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    अन्यथा {
      यदि(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        परिणाम = CURLE_OK;
        गच्छ बहिर्गमनम्;
      }
      परिणाम = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_परिणाम ? backend->gtls.io_परिणाम : CURLE_SEND_ERROR);
      गच्छ बहिर्गमनम्;
    }
  }

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (पूर्णाङ्क)परिणाम, *pnwritten);
  प्रत्यागम परिणाम;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
स्थिर CURLcode gtls_shutdown(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              bool send_shutdown, bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  वर्ण buf[1024];
  CURLcode परिणाम = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  यदि(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *समाप्तम् = TRUE;
    गच्छ बहिर्गमनम्;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *समाप्तम् = FALSE;

  यदि(!backend->gtls.प्रेषितम्_shutdown) {
    /* do this only once */
    backend->gtls.प्रेषितम्_shutdown = TRUE;
    यदि(send_shutdown) {
      पूर्णाङ्क ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      यदि((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(दत्त, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_लेखः_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.प्रेषितम्_shutdown = FALSE;
        परिणाम = CURLE_OK;
        गच्छ बहिर्गमनम्;
      }
      यदि(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(दत्त, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((पूर्णाङ्क)ret), (पूर्णाङ्क)ret);
        परिणाम = CURLE_RECV_ERROR;
        गच्छ बहिर्गमनम्;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  प्रति(i = 0; i < 10; ++i) {
    nread = gnutls_लेखः_recv(backend->gtls.session, buf, परिमाण(buf));
    यदि(nread <= 0)
      भङ्ग;
  }
  यदि(nread > 0) {
    /* still data coming in? */
  }
  अन्यथा यदि(nread == 0) {
    /* We got the close notify alert and are done. */
    *समाप्तम् = TRUE;
  }
  अन्यथा यदि((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_लेखः_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  अन्यथा {
    CURL_TRC_CF(दत्त, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((पूर्णाङ्क)nread), (पूर्णाङ्क)nread);
    परिणाम = CURLE_RECV_ERROR;
  }

बहिर्गमनम्:
  cf->shutdown = (परिणाम || *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर शून्य gtls_close(संरचना Curl_cfilter *cf,
                       संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(दत्त, cf, "close");
  यदि(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = रिक्त;
  }
  यदि(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

स्थिर CURLcode gtls_recv(संरचना Curl_cfilter *cf,
                          संरचना Curl_easy *दत्त,
                          वर्ण *buf, size_t blen,
                          size_t *pnread)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_लेखः_recv(backend->gtls.session, buf, blen);

  यदि(nread >= 0)
    *pnread = (size_t)nread;
  अन्यथा {
    यदि((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      परिणाम = CURLE_AGAIN;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      परिणाम = cf_gtls_handshake(cf, दत्त);
      यदि(!परिणाम)
        परिणाम = CURLE_AGAIN; /* make us get called again. */
      गच्छ बहिर्गमनम्;
    }
    अन्यथा {
      failf(दत्त, "GnuTLS recv error (%d): %s",
            (पूर्णाङ्क)nread, gnutls_strerror((पूर्णाङ्क)nread));
      परिणाम = backend->gtls.io_परिणाम ?
        backend->gtls.io_परिणाम : CURLE_RECV_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (पूर्णाङ्क)परिणाम, nread);
  प्रत्यागम परिणाम;
}

size_t Curl_gtls_संस्करणम्(वर्ण *बफर, size_t आकार)
{
  प्रत्यागम curl_msnprintf(बफर, आकार, "GnuTLS/%s", gnutls_check_संस्करणम्(रिक्त));
}

/* data might be NULL! */
स्थिर CURLcode gtls_random(संरचना Curl_easy *दत्त,
                            अचिह्नित वर्ण *entropy, size_t दीर्घता)
{
  पूर्णाङ्क rc;
  (शून्य)दत्त;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, दीर्घता);
  प्रत्यागम rc ? CURLE_FAILED_INIT : CURLE_OK;
}

स्थिर CURLcode gtls_sha256sum(ध्रुव अचिह्नित वर्ण *निवेशः,
                               size_t len,
                               अचिह्नित वर्ण *sha256sum, /* output */
                               size_t sha256len)
{
  संरचना sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  कुरु {
    अचिह्नित पूर्णाङ्क ilen = (अचिह्नित पूर्णाङ्क)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, निवेशः);
    len -= ilen;
    निवेशः += ilen;
  } यावत्(len);
#यदि NETTLE_संस्करणम्_MAJOR >= 4
  (शून्य)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#अन्यथा
  sha256_digest(&SHA256pw, (अचिह्नित पूर्णाङ्क)sha256len, sha256sum);
#यदि_अन्तः
  प्रत्यागम CURLE_OK;
}

स्थिर bool gtls_cert_स्थितिः_request(शून्य)
{
  प्रत्यागम TRUE;
}

स्थिर शून्य *gtls_get_internals(संरचना ssl_connect_दत्त *connssl,
                                CURLINFO info)
{
  संरचना gtls_ssl_backend_दत्त *backend =
    (संरचना gtls_ssl_backend_दत्त *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  प्रत्यागम backend->gtls.session;
}

ध्रुव संरचना Curl_ssl Curl_ssl_gnutls = {
  { CURLSSLBACKEND_GNUTLS, "gnutls" }, /* info */

  SSLSUPP_CA_मार्गः  |
  SSLSUPP_CERTINFO |
  SSLSUPP_PINNEDPUBKEY |
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_CA_CACHE |
  SSLSUPP_ISSUERCERT |
  SSLSUPP_CRLFILE,

  परिमाण(संरचना gtls_ssl_backend_दत्त),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_संस्करणम्,             /*संस्करणम्*/
  gtls_shutdown,                 /* shutdown */
  gtls_दत्त_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_स्थितिः_request,      /* cert_status_request */
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

#यदि_अन्तः /* USE_GNUTLS */
