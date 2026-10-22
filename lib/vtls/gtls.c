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
#समावेश "curl_setup.h"

#अगर_घोषित USE_GNUTLS

#समावेश <gnutls/abstract.h>
#समावेश <gnutls/gnutls.h>
#समावेश <gnutls/x509.h>
#समावेश <gnutls/crypto.h>
#समावेश <nettle/sha2.h>
#समावेश <nettle/version.h>

#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "vtls/keylog.h"
#समावेश "vtls/gtls.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_int.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "vtls/apple.h"
#समावेश "vauth/vauth.h"
#समावेश "parsedate.h"
#समावेश "connect.h" /* for the connect timeout */
#समावेश "progress.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/win32-fopen.h"
#समावेश "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#अगर 0
#घोषणा GTLSDEBUG
#पूर्ण_अगर

#अगर_घोषित GTLSDEBUG
अटल व्योम tls_log_func(पूर्णांक level, स्थिर अक्षर *str)
{
  curl_mfprintf(मानक_त्रुटि, "|<%d>| %s", level, str);
}
#पूर्ण_अगर

#अगर !defined(GNUTLS_VERSION_संख्या) || (GNUTLS_VERSION_संख्या < 0x030605)
#त्रुटि "too old GnuTLS version"
#पूर्ण_अगर

#अघोषित CURL_GNUTLS_EARLY_तथ्य
#अगर GNUTLS_VERSION_संख्या >= 0x030702
#घोषणा CURL_GNUTLS_EARLY_तथ्य
#पूर्ण_अगर

#समावेश <gnutls/ocsp.h>

काष्ठा gtls_ssl_backend_तथ्य {
  काष्ठा gtls_ctx gtls;
};

अटल ssize_t gtls_धकेलो(व्योम *s, स्थिर व्योम *buf, माप_प्रकार blen)
{
  काष्ठा Curl_cfilter *cf = s;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य);
  परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(तथ्य, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (पूर्णांक)परिणाम, nwritten);
  backend->gtls.io_परिणाम = परिणाम;
  अगर(परिणाम) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (परिणाम == CURLE_AGAIN) ? EAGAIN : EINVAL);
    वापस -1;
  }
  वापस (ssize_t)nwritten;
}

अटल ssize_t gtls_pull(व्योम *s, व्योम *buf, माप_प्रकार blen)
{
  काष्ठा Curl_cfilter *cf = s;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  माप_प्रकार nread;
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य);
  अगर(!backend->gtls.shared_creds->trust_setup) {
    परिणाम = Curl_gtls_client_trust_setup(cf, तथ्य, &backend->gtls);
    अगर(परिणाम) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_परिणाम = परिणाम;
      वापस -1;
    }
  }

  परिणाम = Curl_conn_cf_recv(cf->अग्र, तथ्य, buf, blen, &nread);
  CURL_TRC_CF(तथ्य, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (पूर्णांक)परिणाम,
              nread);
  backend->gtls.io_परिणाम = परिणाम;
  अगर(परिणाम) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (परिणाम == CURLE_AGAIN) ? EAGAIN : EINVAL);
    वापस -1;
  }
  अन्यथा अगर(nread == 0)
    connssl->peer_closed = TRUE;
  वापस (ssize_t)nread;
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
अटल पूर्णांक gtls_init(व्योम)
{
  पूर्णांक ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#अगर_घोषित GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#पूर्ण_अगर
  वापस ret;
}

अटल व्योम gtls_cleanup(व्योम)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#अगर_घोषित CURLVERBOSE
अटल व्योम showtime(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *text, समय_प्रकार stamp)
{
  काष्ठा पंचांग बफर;
  स्थिर काष्ठा पंचांग *पंचांग = &बफर;
  अक्षर str[96];
  CURLcode परिणाम = curlx_gmtime(stamp, &बफर);
  अगर(परिणाम)
    वापस;

  curl_msnprintf(str,
                 माप(str),
                 "  %s: %s, %02d %s %4d %02d:%02d:%02d GMT",
                 text,
                 Curl_wkday[पंचांग->tm_wday ? पंचांग->tm_wday-1 : 6],
                 पंचांग->tm_mday,
                 Curl_month[पंचांग->tm_mon],
                 पंचांग->tm_year + 1900,
                 पंचांग->tm_hour,
                 पंचांग->tm_min,
                 पंचांग->tm_sec);
  infof(तथ्य, "%s", str);
}
#पूर्ण_अगर

अटल gnutls_datum_t load_file(स्थिर अक्षर *file)
{
  खाता *f;
  gnutls_datum_t loaded_file = { शून्य, 0 };
  दीर्घ filelen;
  व्योम *ptr;

  f = curlx_fopen(file, "rb");
  अगर(!f)
    वापस loaded_file;
  अगर(ख_जाओ(f, 0, अंत_से))
    जाओ out;
  filelen = ख_बताओ(f);
  अगर(filelen < 0 || filelen > CURL_MAX_निवेश_लंबाई)
    जाओ out;
  अगर(ख_जाओ(f, 0, शुरू_से))
    जाओ out;
  ptr = curlx_malloc((माप_प्रकार)filelen);
  अगर(!ptr)
    जाओ out;
  अगर(ख_पढ़ो(ptr, 1, (माप_प्रकार)filelen, f) < (माप_प्रकार)filelen) {
    curlx_free(ptr);
    जाओ out;
  }

  loaded_file.तथ्य = ptr;
  loaded_file.आकार = (अचिन्हित पूर्णांक)filelen;
out:
  curlx_fclose(f);
  वापस loaded_file;
}

अटल व्योम unload_file(gnutls_datum_t तथ्य)
{
  curlx_free(तथ्य.तथ्य);
}

/* this function does an SSL/TLS (re-)handshake */
अटल CURLcode cf_gtls_handshake(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  gnutls_session_t session;
  पूर्णांक rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_परिणाम = CURLE_OK;
  rc = gnutls_handshake(session);

  अगर(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode परिणाम = Curl_gtls_client_trust_setup(cf, तथ्य, &backend->gtls);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    वापस CURLE_AGAIN;
  }
  अन्यथा अगर((rc < 0) && !gnutls_error_is_fatal(rc)) {
#अगर_घोषित CURLVERBOSE
    स्थिर अक्षर *strerr = शून्य;

    अगर(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    अगर(!strerr)
      strerr = gnutls_strerror(rc);

    infof(तथ्य, "gnutls_handshake() warning: %s", strerr);
#पूर्ण_अगर
    वापस CURLE_AGAIN;
  }
  अन्यथा अगर((rc < 0) && backend->gtls.io_परिणाम) {
    वापस backend->gtls.io_परिणाम;
  }
  अन्यथा अगर(rc < 0) {
    स्थिर अक्षर *strerr = शून्य;

    अगर(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    अगर(!strerr)
      strerr = gnutls_strerror(rc);

    failf(तथ्य, "GnuTLS, handshake failed: %s", strerr);
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  वापस CURLE_OK;
}

अटल gnutls_x509_crt_fmt_t gnutls_do_file_type(स्थिर अक्षर *type)
{
  अगर(!type || !type[0])
    वापस GNUTLS_X509_FMT_PEM;
  अगर(curl_strequal(type, "PEM"))
    वापस GNUTLS_X509_FMT_PEM;
  अगर(curl_strequal(type, "DER"))
    वापस GNUTLS_X509_FMT_DER;
  वापस GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#घोषणा GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#घोषणा QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

अटल CURLcode gnutls_set_ssl_version_min_max(
  काष्ठा Curl_easy *तथ्य,
  काष्ठा ssl_peer *peer,
  काष्ठा ssl_filter_config *conn_config,
  स्थिर अक्षर **prioritylist,
  bool tls13support)
{
  दीर्घ ssl_version = conn_config->version;
  दीर्घ ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  अगर(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  अगर((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  अगर(peer->transport == TRNSPRT_QUIC) {
    अगर(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(तथ्य, "QUIC needs at least TLS version 1.3");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    वापस CURLE_OK;
  }

  चयन(ssl_version | ssl_version_max) {
  हाल CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    वापस CURLE_OK;
  हाल CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    वापस CURLE_OK;
  हाल CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    वापस CURLE_OK;
  हाल CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    वापस CURLE_OK;
  हाल CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    वापस CURLE_OK;
  हाल CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    वापस CURLE_OK;
  हाल CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    वापस CURLE_OK;
  हाल CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    वापस CURLE_OK;
  हाल CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    वापस CURLE_OK;
  हाल CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    वापस CURLE_OK;
  }

  failf(तथ्य, "GnuTLS: cannot set TLS protocol");
  वापस CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(काष्ठा Curl_easy *तथ्य,
                                       काष्ठा gtls_shared_creds **pcreds)
{
  काष्ठा gtls_shared_creds *shared;
  पूर्णांक rc;

  *pcreds = शून्य;
  shared = curlx_calloc(1, माप(*shared));
  अगर(!shared)
    वापस CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  अगर(rc != GNUTLS_E_SUCCESS) {
    failf(तथ्य, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->समय = *Curl_pgrs_now(तथ्य);
  *pcreds = shared;
  वापस CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(काष्ठा gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  अगर(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    वापस CURLE_OK;
  }
  वापस CURLE_BAD_FUNCTION_ARGUMENT;
}

व्योम Curl_gtls_shared_creds_free(काष्ठा gtls_shared_creds **pcreds)
{
  काष्ठा gtls_shared_creds *shared = *pcreds;
  *pcreds = शून्य;
  अगर(shared) {
    --shared->refcount;
    अगर(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

अटल CURLcode gtls_populate_creds(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    gnutls_certificate_credentials_t creds)
{
  काष्ठा ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, तथ्य);
  bool creds_are_empty = TRUE;
  पूर्णांक rc;

  अगर(!config->verifypeer) {
    infof(तथ्य, "SSL Trust: peer verification disabled");
    वापस CURLE_OK;
  }

  infof(तथ्य, "SSL Trust Anchors:");
  अगर(config->native_ca_store) {
#अगर_घोषित USE_APPLE_SECTRUST
    infof(तथ्य, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#अन्यथा
    rc = gnutls_certificate_set_x509_system_trust(creds);
    अगर(rc < 0)
      infof(तथ्य, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    अन्यथा {
      infof(तथ्य, "  Native: %d certificates from system trust", rc);
      अगर(rc > 0)
        creds_are_empty = FALSE;
    }
#पूर्ण_अगर
  }

  अगर(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    अगर(config->ca_info_blob->len > (माप_प्रकार)अच_पूर्णांक_उच्च) {
      failf(तथ्य, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      वापस CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.तथ्य = config->ca_info_blob->तथ्य;
    ca_info_datum.आकार = (अचिन्हित पूर्णांक)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    अगर(rc < 0) {
      infof(तथ्य, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      अगर(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        वापस CURLE_SSL_CACERT_BADFILE;
      }
    }
    अन्यथा
      infof(तथ्य, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  अन्यथा अगर(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    अगर(rc < 0) {
      infof(तथ्य, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      अगर(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        वापस CURLE_SSL_CACERT_BADFILE;
      }
    }
    अन्यथा
      infof(तथ्य, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  अगर(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    अगर(rc < 0) {
      infof(तथ्य, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      अगर(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        वापस CURLE_SSL_CACERT_BADFILE;
      }
    }
    अन्यथा
      infof(तथ्य, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  अगर(creds_are_empty)
    infof(तथ्य, "  no trust anchors configured");

#अगर_घोषित USE_APPLE_SECTRUST
  अगर(config->CRLfile && config->native_ca_store) {
    failf(तथ्य, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    वापस CURLE_NOT_BUILT_IN;
  }
#पूर्ण_अगर
  अगर(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    अगर(rc < 0) {
      failf(तथ्य, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      वापस CURLE_SSL_CRL_BADFILE;
    }
    अन्यथा
      infof(तथ्य, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  वापस CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#घोषणा MPROTO_GTLS_X509_कुंजी   "tls:gtls:x509:share"

अटल bool gtls_shared_creds_expired(काष्ठा Curl_easy *तथ्य,
                                      स्थिर काष्ठा gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(तथ्य), &sc->समय);
  timediff_t timeout_ms = तथ्य->set.ssl_ca_cache_timeout * (timediff_t)1000;

  अगर(timeout_ms < 0)
    वापस FALSE;

  वापस elapsed_ms >= timeout_ms;
}

अटल bool gtls_shared_creds_different(काष्ठा Curl_cfilter *cf,
                                        स्थिर काष्ठा gtls_shared_creds *sc)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  अगर(!sc->CAfile || !conn_config->CAfile)
    वापस sc->CAfile != conn_config->CAfile;

  वापस म_भेद(sc->CAfile, conn_config->CAfile);
}

अटल काष्ठा gtls_shared_creds *gtls_get_cached_creds(काष्ठा Curl_cfilter *cf,
                                                       काष्ठा Curl_easy *तथ्य)
{
  काष्ठा gtls_shared_creds *shared_creds;

  अगर(तथ्य->multi) {
    shared_creds = Curl_hash_pick(&तथ्य->multi->proto_hash,
                                  MPROTO_GTLS_X509_कुंजी,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_कुंजी));
    अगर(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(तथ्य, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      वापस shared_creds;
    }
  }
  वापस शून्य;
}

अटल व्योम gtls_shared_creds_hash_free(स्थिर व्योम *कुंजी,
                                        माप_प्रकार कुंजी_len, व्योम *p)
{
  काष्ठा gtls_shared_creds *sc = p;
  DEBUGASSERT(कुंजी_len == CURL_CSTRLEN(MPROTO_GTLS_X509_कुंजी));
  DEBUGASSERT(!स_भेद(MPROTO_GTLS_X509_कुंजी, कुंजी, कुंजी_len));
  (व्योम)कुंजी;
  (व्योम)कुंजी_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

अटल व्योम gtls_set_cached_creds(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा gtls_shared_creds *sc)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  अगर(!तथ्य->multi)
    वापस;

  अगर(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    अगर(!sc->CAfile)
      वापस;
  }

  अगर(Curl_gtls_shared_creds_up_ref(sc))
    वापस;

  अगर(!Curl_hash_add2(&तथ्य->multi->proto_hash,
                     MPROTO_GTLS_X509_कुंजी,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_कुंजी),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    वापस;
  }
}

CURLcode Curl_gtls_client_trust_setup(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा gtls_ctx *gtls)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा gtls_shared_creds *cached_creds = शून्य;
  bool cache_criteria_met;
  CURLcode परिणाम;
  पूर्णांक rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (तथ्य->set.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  अगर(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, तथ्य);

  अगर(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(तथ्य, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    अगर(rc != GNUTLS_E_SUCCESS) {
      failf(तथ्य, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      वापस CURLE_SSL_CONNECT_ERROR;
    }
  }
  अन्यथा {
    CURL_TRC_CF(तथ्य, cf, "loading trust anchors and CRLs");
    परिणाम = gtls_populate_creds(cf, तथ्य, gtls->shared_creds->creds);
    अगर(परिणाम)
      वापस परिणाम;
    gtls->shared_creds->trust_setup = TRUE;
    अगर(cache_criteria_met)
      gtls_set_cached_creds(cf, तथ्य, gtls->shared_creds);
  }
  वापस CURLE_OK;
}

#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
अटल पूर्णांक gtls_get_ietf_proto(gnutls_session_t session)
{
  चयन(gnutls_protocol_get_version(session)) {
  हाल GNUTLS_SSL3:
    वापस CURL_IETF_PROTO_SSL3;
  हाल GNUTLS_TLS1_0:
    वापस CURL_IETF_PROTO_TLS1;
  हाल GNUTLS_TLS1_1:
    वापस CURL_IETF_PROTO_TLS1_1;
  हाल GNUTLS_TLS1_2:
    वापस CURL_IETF_PROTO_TLS1_2;
  हाल GNUTLS_TLS1_3:
    वापस CURL_IETF_PROTO_TLS1_3;
  शेष:
    वापस CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *ssl_peer_कुंजी,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 स्थिर अक्षर *alpn,
                                 अचिन्हित अक्षर *quic_tp,
                                 माप_प्रकार quic_tp_len,
                                 काष्ठा Curl_ssl_session **psession)
{
  काष्ठा Curl_ssl_session *sc_session = शून्य, *sc_dup = शून्य;
  अचिन्हित अक्षर *sdata, *qtp_clone = शून्य;
  माप_प्रकार sdata_len = 0;
  माप_प्रकार earlydata_max = 0;
  CURLcode परिणाम = CURLE_OK;

  अगर(psession)
    *psession = शून्य;
  अगर(!Curl_ssl_scache_use(cf, तथ्य))
    वापस CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_तथ्य(session, शून्य, &sdata_len);
  अगर(!sdata_len) /* GnuTLS does this for some version combinations */
    वापस CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  अगर(!sdata)
    वापस CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_तथ्य(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_तथ्य_आकार(session);

  CURL_TRC_CF(तथ्य, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  अगर(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((स्थिर अक्षर *)quic_tp, quic_tp_len);
    अगर(!qtp_clone) {
      curlx_free(sdata);
      वापस CURLE_OUT_OF_MEMORY;
    }
  }

  परिणाम = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  अगर(!परिणाम && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    परिणाम = Curl_ssl_session_dup(sc_session, &sc_dup);
  अगर(!परिणाम) {
    परिणाम = Curl_ssl_scache_put(cf, तथ्य, ssl_peer_कुंजी, sc_session);
    /* took ownership of `sc_session` */
    sc_session = शून्य;
  }
  अगर(!परिणाम && psession) {
    *psession = sc_dup;
    sc_dup = शून्य;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  वापस परिणाम;
}
#पूर्ण_अगर

#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
अटल CURLcode cf_gtls_update_session_id(काष्ठा Curl_cfilter *cf,
                                          काष्ठा Curl_easy *तथ्य,
                                          gnutls_session_t session)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा Curl_ssl_session *scs = शून्य;
  CURLcode परिणाम;

  परिणाम = Curl_gtls_cache_session(cf, तथ्य, connssl->peer.scache_कुंजी,
                                   session, 0, connssl->negotiated.alpn,
                                   शून्य, 0, &scs);
  अगर(!परिणाम) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  वापस परिणाम;
}

अटल पूर्णांक gtls_handshake_cb(gnutls_session_t session, अचिन्हित पूर्णांक htype,
                             अचिन्हित when, अचिन्हित पूर्णांक incoming,
                             स्थिर gnutls_datum_t *संदेश)
{
  काष्ठा Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (व्योम)संदेश;
  (व्योम)incoming;
  अगर(when) { /* after message has been processed */
    काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
    अगर(तथ्य) {
      CURL_TRC_CF(तथ्य, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      चयन(htype) {
      हाल GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, तथ्य, session);
        अवरोध;
      }
      शेष:
        अवरोध;
      }
    }
  }
  वापस 0;
}
#पूर्ण_अगर

अटल CURLcode gtls_set_priority(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा gtls_ctx *gtls,
                                  स्थिर अक्षर *priority)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा dynbuf buf;
  स्थिर अक्षर *err = शून्य;
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक rc;

  curlx_dyn_init(&buf, 4096);

  अगर(conn_config->cipher_list) {
    अगर((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      अगर(!curlx_dyn_len(&buf)) {  /* not added yet */
        परिणाम = curlx_dyn_add(&buf, priority);
        अगर(परिणाम)
          जाओ out;
      }
      परिणाम = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      अगर(परिणाम)
        जाओ out;
      priority = curlx_dyn_ptr(&buf);
    }
    अन्यथा /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(तथ्य, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  अगर(rc != GNUTLS_E_SUCCESS) {
    failf(तथ्य, "Error %d setting GnuTLS priority: %s", rc, err);
    परिणाम = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  वापस परिणाम;
}

अटल CURLcode gtls_client_init(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ssl_peer *peer,
                                 माप_प्रकार earlydata_max,
                                 काष्ठा gtls_ctx *gtls)
{
  काष्ठा ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, तथ्य);
  अचिन्हित पूर्णांक init_flags;
  पूर्णांक rc;
  स्थिर अक्षर *prioritylist;
  bool tls13support;
  CURLcode परिणाम;

  अगर(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(तथ्य, "GnuTLS does not support SSLv2 or SSLv3");
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  परिणाम = Curl_gtls_shared_creds_create(तथ्य, &gtls->shared_creds);
  अगर(परिणाम)
    वापस परिणाम;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
  अगर(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_तथ्य | GNUTLS_NO_END_OF_EARLY_तथ्य;
  अन्यथा अगर(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_तथ्य;
#पूर्ण_अगर

#अगर_घोषित GNUTLS_FORCE_CLIENT_CERT
  init_flags |= GNUTLS_FORCE_CLIENT_CERT;
#पूर्ण_अगर

#अगर_घोषित GNUTLS_NO_TICKETS_TLS12
  init_flags |= GNUTLS_NO_TICKETS_TLS12;
#पूर्ण_अगर

#अगर_घोषित GNUTLS_NO_STATUS_REQUEST
  अगर(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#पूर्ण_अगर

  CURL_TRC_CF(तथ्य, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  अगर(rc != GNUTLS_E_SUCCESS) {
    failf(तथ्य, "gnutls_init() failed: %d", rc);
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  अगर(peer->sni) {
    अगर(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, म_माप(peer->sni)) < 0) {
      failf(तथ्य, "Failed to set SNI");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  अगर(rc != GNUTLS_E_SUCCESS)
    वापस CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  अगर(config->version == CURL_SSLVERSION_TLSv1_3) {
    अगर(!tls13support) {
      failf(तथ्य, "This GnuTLS installation does not support TLS 1.3");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  परिणाम = gnutls_set_ssl_version_min_max(तथ्य, peer,
                                          config, &prioritylist, tls13support);
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = gtls_set_priority(cf, तथ्य, gtls, prioritylist);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(config->clientcert) {
    अगर(!gtls->shared_creds->trust_setup) {
      परिणाम = Curl_gtls_client_trust_setup(cf, तथ्य, gtls);
      अगर(परिणाम)
        वापस परिणाम;
    }
    अगर(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->कुंजी_passwd ? config->कुंजी_passwd : "");
      अगर(rc != GNUTLS_E_SUCCESS) {
        failf(तथ्य,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        वापस CURLE_SSL_CONNECT_ERROR;
      }
    }
    अन्यथा {
      स्थिर अचिन्हित पूर्णांक supported_कुंजी_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_कुंजी_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->कुंजी ? config->कुंजी : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->कुंजी_passwd,
           supported_कुंजी_encryption_algorithms);
      अगर(rc != GNUTLS_E_SUCCESS) {
        failf(तथ्य,
              "error reading X.509 %skey file: %s",
              config->कुंजी_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        वापस CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  अगर(rc != GNUTLS_E_SUCCESS) {
    failf(तथ्य, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  अगर(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  शून्य, 0, शून्य);
    अगर(rc != GNUTLS_E_SUCCESS) {
      failf(तथ्य, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      वापस CURLE_SSL_CONNECT_ERROR;
    }
  }

  वापस CURLE_OK;
}

#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
अटल पूर्णांक keylog_callback(gnutls_session_t session, स्थिर अक्षर *label,
                           स्थिर gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  अगर(crandom.आकार != 32) {
    वापस -1;
  }

  Curl_tls_keylog_write(label, crandom.तथ्य, crandom.आकार,
                        secret->तथ्य, secret->आकार);
  वापस 0;
}

अटल CURLcode gtls_on_session_reuse(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा alpn_spec *alpns,
                                      काष्ठा Curl_ssl_session *scs,
                                      bool *do_early_तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_तथ्य_आकार(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  वापस Curl_on_session_reuse(cf, तथ्य, alpns, scs, do_early_तथ्य,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#पूर्ण_अगर

अटल CURLcode gtls_apply_session(
  काष्ठा gtls_ctx *gctx,
  काष्ठा Curl_cfilter *cf,
  काष्ठा Curl_easy *तथ्य,
  काष्ठा ssl_peer *peer,
  काष्ठा alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  काष्ठा Curl_ssl_session *scs,
  bool *pwas_setup)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक rc;

  अगर(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    परिणाम = gtls_client_init(cf, तथ्य, peer, scs->earlydata_max, gctx);
    अगर(परिणाम)
      जाओ out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_तथ्य(gctx->session, scs->sdata, scs->sdata_len);
    अगर(rc < 0)
      infof(तथ्य, "SSL session not accepted by GnuTLS, continuing without");
    अन्यथा {
      infof(तथ्य, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      अगर(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_तथ्य = FALSE;
        अगर(sess_reuse_cb) {
          परिणाम = sess_reuse_cb(cf, तथ्य, alpns, scs, &do_early_तथ्य);
          अगर(परिणाम)
            जाओ out;
        }
        अगर(do_early_तथ्य) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  वापस परिणाम;
}

CURLcode Curl_gtls_ctx_init(काष्ठा gtls_ctx *gctx,
                            काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            काष्ठा ssl_peer *peer,
                            स्थिर काष्ठा alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            व्योम *cb_user_तथ्य,
                            व्योम *ssl_user_तथ्य,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा Curl_ssl_session *scs = शून्य;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  माप_प्रकार gtls_alpns_गिनती = 0;
  bool gtls_session_setup = FALSE;
  काष्ठा alpn_spec alpns;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  अगर((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(तथ्य, cf->cft, FIRSTSOCKET);
    अगर(scs) {
      परिणाम = gtls_apply_session(gctx, cf, तथ्य, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = शून्य;
      अगर(!परिणाम && gtls_session_setup) {
        CURL_TRC_CF(तथ्य, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  अगर(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    परिणाम = Curl_ssl_scache_take(cf, तथ्य, peer->scache_कुंजी, &scs);
    अगर(परिणाम)
      जाओ out;
    परिणाम = gtls_apply_session(gctx, cf, तथ्य, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  अगर(!gtls_session_setup) {
    परिणाम = gtls_client_init(cf, तथ्य, peer, 0, gctx);
    अगर(परिणाम)
      जाओ out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_तथ्य);

  अगर(cb_setup) {
    परिणाम = cb_setup(cf, तथ्य, cb_user_तथ्य);
    अगर(परिणाम)
      जाओ out;
  }

#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  अगर(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#पूर्ण_अगर

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  अगर(!gtls_alpns_गिनती && alpns.गिनती) {
    माप_प्रकार i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.गिनती);
    क्रम(i = 0; i < alpns.गिनती; ++i) {
      gtls_alpns[i].तथ्य = (अचिन्हित अक्षर *)alpns.entries[i];
      gtls_alpns[i].आकार = (अचिन्हित पूर्णांक)म_माप(alpns.entries[i]);
    }
    gtls_alpns_गिनती = alpns.गिनती;
  }

  अगर(gtls_alpns_गिनती &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (अचिन्हित पूर्णांक)gtls_alpns_गिनती,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(तथ्य, "failed setting ALPN");
    परिणाम = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, तथ्य, peer->scache_कुंजी, scs);
  वापस परिणाम;
}

अटल CURLcode gtls_connect_step1(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम;

  DEBUGASSERT(backend);

  अगर(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    वापस CURLE_OK;

  परिणाम = Curl_gtls_ctx_init(&backend->gtls, cf, तथ्य, &connssl->peer,
                              connssl->alpn, शून्य, शून्य, cf,
#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
                              gtls_on_session_reuse
#अन्यथा
                              शून्य
#पूर्ण_अगर
                              );

  अगर(परिणाम)
    वापस परिणाम;

  अगर(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    काष्ठा alpn_proto_buf proto;
    स_रखो(&proto, 0, माप(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(तथ्य, VTLS_INFOF_ALPN_OFFER_1STR, proto.तथ्य);
  }

#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
  gnutls_handshake_set_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#पूर्ण_अगर

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_set_ptr(backend->gtls.session, cf);
  gnutls_transport_set_धकेलो_function(backend->gtls.session, gtls_धकेलो);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  वापस CURLE_OK;
}

अटल CURLcode pkp_pin_peer_pubkey(काष्ठा Curl_easy *तथ्य,
                                    gnutls_x509_crt_t cert,
                                    स्थिर अक्षर *pinnedpubkey)
{
  /* Scratch */
  माप_प्रकार len1 = 0, len2 = 0;
  अचिन्हित अक्षर *buff1 = शून्य;

  gnutls_pubkey_t कुंजी = शून्य;

  /* Result is returned to caller */
  CURLcode परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  अगर(!pinnedpubkey)
    वापस CURLE_OK;

  अगर(!cert)
    वापस परिणाम;

  करो {
    पूर्णांक ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&कुंजी);
    अगर(ret < 0)
      अवरोध; /* failed */

    ret = gnutls_pubkey_import_x509(कुंजी, cert, 0);
    अगर(ret < 0)
      अवरोध; /* failed */

    ret = gnutls_pubkey_export(कुंजी, GNUTLS_X509_FMT_DER, शून्य, &len1);
    अगर(ret != GNUTLS_E_SHORT_MEMORY_बफर || len1 == 0)
      अवरोध; /* failed */

    buff1 = curlx_malloc(len1);
    अगर(!buff1)
      अवरोध; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(कुंजी, GNUTLS_X509_FMT_DER, buff1, &len2);
    अगर(ret < 0 || len1 != len2)
      अवरोध; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    परिणाम = Curl_pin_peer_pubkey(तथ्य, pinnedpubkey, buff1, len1);
  } जबतक(0);

  अगर(कुंजी)
    gnutls_pubkey_deinit(कुंजी);

  curlx_safefree(buff1);

  वापस परिणाम;
}

व्योम Curl_gtls_report_handshake(काष्ठा Curl_easy *तथ्य, काष्ठा gtls_ctx *gctx)
{
#अगर_घोषित CURLVERBOSE
  अगर(Curl_trc_is_verbose(तथ्य)) {
    स्थिर अक्षर *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(तथ्य, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)gctx;
#पूर्ण_अगर
}

अटल व्योम gtls_संदेश_verify_परिणाम(काष्ठा Curl_easy *तथ्य,
                                   काष्ठा ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  अक्षर certname[65] = ""; /* limited to 64 chars by ASN.1 */
  माप_प्रकार आकार = माप(certname);
  पूर्णांक rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &आकार);
  अगर(rc) {
    infof(तथ्य, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  अगर(!was_verified) {
    अगर(needs_verified) {
      failf(तथ्य, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    अन्यथा
      infof(तथ्य, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  अन्यथा
    infof(तथ्य, "  common name: %s (matched)", certname);
}

अटल व्योम gtls_infof_cert(काष्ठा Curl_easy *तथ्य,
                            gnutls_x509_crt_t x509_cert)
{
#अगर_घोषित CURLVERBOSE
  अगर(Curl_trc_is_verbose(तथ्य)) {
    gnutls_datum_t certfields;
    पूर्णांक rc, algo;
    समय_प्रकार tstamp;
    अचिन्हित पूर्णांक bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(तथ्य, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(तथ्य, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    अगर(rc)
      infof(तथ्य, "Failed to get certificate name");
    अन्यथा {
      infof(तथ्य, "  subject: %s", certfields.तथ्य);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(तथ्य, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(तथ्य, "expire date", tstamp);

      gnutls_free(certfields.तथ्य);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    अगर(rc)
      infof(तथ्य, "Failed to get certificate issuer");
    अन्यथा {
      infof(तथ्य, "  issuer: %s", certfields.तथ्य);

      gnutls_free(certfields.तथ्य);
    }
  }
#अन्यथा
  (व्योम)तथ्य;
  (व्योम)x509_cert;
#पूर्ण_अगर
}

अटल CURLcode gtls_verify_ocsp_status(काष्ठा Curl_easy *तथ्य,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = शून्य;
  CURLcode परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
  पूर्णांक rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  अगर(rc == GNUTLS_E_REQUESTED_तथ्य_NOT_AVAILABLE) {
    failf(तथ्य, "No OCSP response received");
    जाओ out;
  }
  अन्यथा अगर(rc < 0) {
    failf(तथ्य, "Invalid OCSP response received");
    जाओ out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  अगर(rc < 0) {
    failf(तथ्य, "failed to init OCSP response parsing");
    जाओ out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  अगर(rc < 0) {
    failf(तथ्य, "failed to parse OCSP response");
    जाओ out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  अगर(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(तथ्य, "OCSP response basic part is not successful");
    जाओ out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  क्रम(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    अगर(!rc || (rc == GNUTLS_E_REQUESTED_तथ्य_NOT_AVAILABLE))
      अवरोध;
  }
  अगर(rc < 0) {
    failf(तथ्य, "OCSP does not carry status for peer cert");
    जाओ out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, शून्य, शून्य, शून्य, शून्य,
                                   &status, शून्य, शून्य, शून्य, शून्य);
  अगर(rc < 0) {
    failf(तथ्य, "failed to parse OCSP response for peer cert");
    जाओ out;
  }
  चयन(status) {
  हाल GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    परिणाम = CURLE_OK;
    अवरोध;
  हाल GNUTLS_OCSP_CERT_REVOKED:
    failf(तथ्य, "peer certificate is revoked");
    अवरोध;
  शेष:
    failf(तथ्य, "peer certificate OCSP status is not good: %d", (पूर्णांक)status);
    अवरोध;
  }

out:
  अगर(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  वापस परिणाम;
}

काष्ठा gtls_cert_chain {
  स्थिर gnutls_datum_t *certs;
  अचिन्हित पूर्णांक अंक_certs;
};

#अगर_घोषित USE_APPLE_SECTRUST
अटल CURLcode gtls_chain_get_der(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   व्योम *user_तथ्य,
                                   माप_प्रकार i,
                                   अचिन्हित अक्षर **pder,
                                   माप_प्रकार *pder_len)
{
  काष्ठा gtls_cert_chain *chain = user_तथ्य;

  (व्योम)cf;
  (व्योम)तथ्य;
  *pder_len = 0;
  *pder = शून्य;

  अगर(i >= chain->अंक_certs)
    वापस CURLE_TOO_LARGE;
  *pder = chain->certs[i].तथ्य;
  *pder_len = (माप_प्रकार)chain->certs[i].आकार;
  वापस CURLE_OK;
}
#पूर्ण_अगर /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
अटल CURLcode gtls_verify_cert(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा ssl_filter_config *config,
                                 काष्ठा ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 काष्ठा Curl_cfilter *cf,
                                 काष्ठा ssl_peer *peer,
                                 काष्ठा gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  अचिन्हित पूर्णांक verify_status = 0;
  दीर्घ * स्थिर certverifyresult = &ssl_config->certverifyresult;
  पूर्णांक rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  अगर(rc < 0) {
    failf(तथ्य, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    वापस CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  अगर(verified)
    infof(तथ्य, "  SSL certificate verified by GnuTLS");

#अगर_घोषित USE_APPLE_SECTRUST
  अगर(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode परिणाम;

    अगर(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.तथ्य = शून्य;
      ocsp_raw.आकार = 0;
    }
    अगर(config->verifystatus && !ocsp_raw.आकार) {
      failf(तथ्य, "No OCSP response received");
      वापस CURLE_SSL_INVALIDCERTSTATUS;
    }
    परिणाम = Curl_vtls_apple_verify(cf, तथ्य, peer, chain->अंक_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.तथ्य, ocsp_raw.आकार,
                                    config->verifystatus);
    अगर(परिणाम && (परिणाम != CURLE_PEER_FAILED_VERIFICATION))
      वापस परिणाम; /* no further failure analysis needed */
    verified = !परिणाम;
    अगर(verified) {
      infof(तथ्य, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#अन्यथा
  (व्योम)cf;
  (व्योम)peer;
  (व्योम)chain;
#पूर्ण_अगर

  अगर(!verified) {
    CURLcode परिणाम = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    स्थिर अक्षर *cause = "certificate error, no details available";
    अगर(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    अन्यथा अगर(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    अन्यथा अगर(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    अन्यथा अगर(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      परिणाम = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(तथ्य, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    वापस परिणाम;
  }
  वापस CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                gnutls_session_t session,
                                काष्ठा ssl_filter_config *config,
                                काष्ठा ssl_easy_config *ssl_config,
                                काष्ठा ssl_peer *peer,
                                स्थिर अक्षर *pinned_कुंजी)
{
  काष्ठा gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = शून्य, x509_issuer = शून्य;
  समय_प्रकार certclock;
  bool ocsp_checked = FALSE;
  पूर्णांक rc;
  CURLcode परिणाम = CURLE_OK;
  दीर्घ * स्थिर certverifyresult = &ssl_config->certverifyresult;

  (व्योम)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.अंक_certs);
  अगर(!chain.certs) {
    अगर(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(तथ्य, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      परिणाम = CURLE_PEER_FAILED_VERIFICATION;
      जाओ out;
    }
    infof(तथ्य, " common name: WARNING could not obtain");
  }

  अगर(तथ्य->set.ssl.certinfo && chain.certs) {
    अगर(chain.अंक_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(तथ्य, "%u certificates is more than allowed (%d)",
            chain.अंक_certs, MAX_ALLOWED_CERT_AMOUNT);
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ out;
    }
    अन्यथा {
      अचिन्हित पूर्णांक i;

      परिणाम = Curl_ssl_init_certinfo(तथ्य, (पूर्णांक)chain.अंक_certs);
      अगर(परिणाम)
        जाओ out;

      क्रम(i = 0; i < chain.अंक_certs; i++) {
        स्थिर uint8_t *beg = chain.certs[i].तथ्य;
        स्थिर uint8_t *end = beg + chain.certs[i].आकार;

        परिणाम = Curl_extract_certinfo(तथ्य, (पूर्णांक)i, beg, end);
        अगर(परिणाम)
          जाओ out;
      }
    }
  }

  अगर(config->verifypeer) {
    परिणाम = gtls_verify_cert(तथ्य, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    अगर(परिणाम)
      जाओ out;
  }
  अन्यथा
    infof(तथ्य, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  अगर(gnutls_x509_crt_init(&x509_cert)) {
    failf(तथ्य, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    जाओ out;
  }

  अगर(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    अगर(rc) {
      failf(तथ्य, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  अगर(certclock == (समय_प्रकार)-1) {
    अगर(config->verifypeer) {
      failf(तथ्य, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ out;
    }
    अन्यथा
      infof(तथ्य, "  SSL certificate expiration date verify FAILED");
  }
  अन्यथा {
    अगर(certclock < समय(शून्य)) {
      अगर(config->verifypeer) {
        failf(तथ्य, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;
        जाओ out;
      }
      अन्यथा
        infof(तथ्य, "  SSL certificate expiration date FAILED");
    }
    अन्यथा
      infof(तथ्य, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  अगर(certclock == (समय_प्रकार)-1) {
    अगर(config->verifypeer) {
      failf(तथ्य, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ out;
    }
    अन्यथा
      infof(तथ्य, "  SSL certificate activation date verify FAILED");
  }
  अन्यथा {
    अगर(certclock > समय(शून्य)) {
      अगर(config->verifypeer) {
        failf(तथ्य, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        परिणाम = CURLE_PEER_FAILED_VERIFICATION;
        जाओ out;
      }
      अन्यथा
        infof(तथ्य, "  SSL certificate activation date FAILED");
    }
    अन्यथा
      infof(तथ्य, "  SSL certificate activation date OK");
  }

  अगर(config->verifystatus && !ocsp_checked) {
    परिणाम = gtls_verify_ocsp_status(तथ्य, session, x509_cert);
    अगर(परिणाम)
      जाओ out;
  }
  अन्यथा
    infof(तथ्य, "  SSL certificate status verification SKIPPED");

  अगर(config->issuercert) {
    gnutls_datum_t issuerp;
    अगर(gnutls_x509_crt_init(&x509_issuer)) {
      failf(तथ्य, "failed to init GnuTLS x509_crt for issuer");
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      जाओ out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    अगर(rc) {
      failf(तथ्य, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      जाओ out;
    }
    rc = (पूर्णांक)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    अगर(rc <= 0) {
      failf(तथ्य, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      परिणाम = CURLE_SSL_ISSUER_ERROR;
      जाओ out;
    }
    infof(तथ्य, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  rc = (पूर्णांक)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  परिणाम = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_संदेश_verify_परिणाम(तथ्य, peer, x509_cert, rc, config->verifyhost);
  अगर(परिणाम)
    जाओ out;

  अगर(pinned_कुंजी) {
    परिणाम = pkp_pin_peer_pubkey(तथ्य, x509_cert, pinned_कुंजी);
    अगर(परिणाम != CURLE_OK) {
      failf(तथ्य, "SSL: public key does not match pinned public key");
      जाओ out;
    }
  }

  gtls_infof_cert(तथ्य, x509_cert);

out:
  अगर(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  अगर(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  वापस परिणाम;
}

अटल CURLcode gtls_verifyserver(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  gnutls_session_t session)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, तथ्य);
#अगर_अघोषित CURL_DISABLE_PROXY
  स्थिर अक्षर *pinned_कुंजी = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY);
#अन्यथा
  स्थिर अक्षर *pinned_कुंजी = CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY);
#पूर्ण_अगर
  CURLcode परिणाम;

  परिणाम = Curl_gtls_verifyserver(cf, तथ्य, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_कुंजी);
  अगर(परिणाम)
    जाओ out;

#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  अगर(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    परिणाम = cf_gtls_update_session_id(cf, तथ्य, session);
#पूर्ण_अगर

out:
  वापस परिणाम;
}

#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
अटल CURLcode gtls_send_earlydata(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  स्थिर अचिन्हित अक्षर *buf;
  माप_प्रकार blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_परिणाम = CURLE_OK;
  जबतक(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_तथ्य(backend->gtls.session, buf, blen);
    CURL_TRC_CF(तथ्य, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    अगर(n < 0) {
      अगर(n == GNUTLS_E_AGAIN)
        परिणाम = CURLE_AGAIN;
      अन्यथा
        परिणाम = backend->gtls.io_परिणाम ?
                 backend->gtls.io_परिणाम : CURLE_SEND_ERROR;
      जाओ out;
    }
    अन्यथा अगर(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (माप_प्रकार)n);
  }
  /* sent everything there was */
  infof(तथ्य, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  वापस परिणाम;
}
#पूर्ण_अगर

/*
 * This function is called after the TCP connect has completed. Setup the TLS
 * layer and do all necessary magic.
 */
/* We use connssl->connecting_state to keep track of the connection status;
   there are three states: 'ssl_connect_1' (not started yet or complete),
   'ssl_connect_2' (doing handshake with the server), and
   'ssl_connect_3' (verifying and getting stats).
 */
अटल CURLcode gtls_connect_common(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  अगर(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  अगर(connssl->connecting_state == ssl_connect_1) {
    परिणाम = gtls_connect_step1(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
    connssl->connecting_state = ssl_connect_2;
  }

  अगर(connssl->connecting_state == ssl_connect_2) {
#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
    अगर(connssl->earlydata_state == ssl_earlydata_await) {
      जाओ out;
    }
    अन्यथा अगर(connssl->earlydata_state == ssl_earlydata_sending) {
      परिणाम = gtls_send_earlydata(cf, तथ्य);
      अगर(परिणाम)
        जाओ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#पूर्ण_अगर
    परिणाम = cf_gtls_handshake(cf, तथ्य);
    अगर(परिणाम)
      जाओ out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  अगर(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    पूर्णांक rc;

    Curl_gtls_report_handshake(तथ्य, &backend->gtls);

    परिणाम = gtls_verifyserver(cf, तथ्य, backend->gtls.session);
    अगर(परिणाम)
      जाओ out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    अगर(rc) {  /* No ALPN from server */
      proto.तथ्य = शून्य;
      proto.आकार = 0;
    }

    परिणाम = Curl_alpn_set_negotiated(cf, तथ्य, connssl,
                                      proto.तथ्य, proto.आकार);
    अगर(परिणाम)
      जाओ out;

#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
    अगर(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_तथ्य) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#पूर्ण_अगर
    connssl->connecting_state = ssl_connect_done;
  }

  अगर(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  अगर(परिणाम == CURLE_AGAIN) {
    *done = FALSE;
    वापस CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(तथ्य, cf, "gtls_connect_common() -> %d, done=%d", (पूर्णांक)परिणाम,
              *done);
  वापस परिणाम;
}

अटल CURLcode gtls_connect(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             bool *done)
{
#अगर_घोषित CURL_GNUTLS_EARLY_तथ्य
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  अगर((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    वापस CURLE_OK;
  }
#पूर्ण_अगर
  वापस gtls_connect_common(cf, तथ्य, done);
}

अटल bool gtls_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                              स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *ctx = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend;

  (व्योम)तथ्य;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (काष्ठा gtls_ssl_backend_तथ्य *)ctx->backend;
  अगर(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    वापस TRUE;
  वापस FALSE;
}

अटल CURLcode gtls_send(काष्ठा Curl_cfilter *cf,
                          काष्ठा Curl_easy *तथ्य,
                          स्थिर व्योम *buf,
                          माप_प्रकार blen,
                          माप_प्रकार *pnwritten)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  ssize_t nwritten;
  माप_प्रकार remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  जबतक(remain) {
    backend->gtls.io_परिणाम = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    अगर(nwritten >= 0) {
      अगर((माप_प्रकार)nwritten > remain) {
        DEBUGASSERT(0);
        failf(तथ्य, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        वापस CURLE_SEND_ERROR;
      }
      *pnwritten += (माप_प्रकार)nwritten;
      buf = (अक्षर *)CURL_UNCONST(buf) + (माप_प्रकार)nwritten;
      remain -= (माप_प्रकार)nwritten;
    }
    अन्यथा {
      अगर(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        परिणाम = CURLE_OK;
        जाओ out;
      }
      परिणाम = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_परिणाम ? backend->gtls.io_परिणाम : CURLE_SEND_ERROR);
      जाओ out;
    }
  }

out:
  CURL_TRC_CF(तथ्य, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (पूर्णांक)परिणाम, *pnwritten);
  वापस परिणाम;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
अटल CURLcode gtls_shutdown(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              bool send_shutdown, bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  अक्षर buf[1024];
  CURLcode परिणाम = CURLE_OK;
  ssize_t nread = 0;
  माप_प्रकार i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  अगर(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    जाओ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  अगर(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    अगर(send_shutdown) {
      पूर्णांक ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      अगर((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(तथ्य, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        परिणाम = CURLE_OK;
        जाओ out;
      }
      अगर(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(तथ्य, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((पूर्णांक)ret), (पूर्णांक)ret);
        परिणाम = CURLE_RECV_ERROR;
        जाओ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  क्रम(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, माप(buf));
    अगर(nread <= 0)
      अवरोध;
  }
  अगर(nread > 0) {
    /* still data coming in? */
  }
  अन्यथा अगर(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  अन्यथा अगर((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  अन्यथा {
    CURL_TRC_CF(तथ्य, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((पूर्णांक)nread), (पूर्णांक)nread);
    परिणाम = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (परिणाम || *done);
  वापस परिणाम;
}

अटल व्योम gtls_close(काष्ठा Curl_cfilter *cf,
                       काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(तथ्य, cf, "close");
  अगर(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = शून्य;
  }
  अगर(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

अटल CURLcode gtls_recv(काष्ठा Curl_cfilter *cf,
                          काष्ठा Curl_easy *तथ्य,
                          अक्षर *buf, माप_प्रकार blen,
                          माप_प्रकार *pnread)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  अगर(nread >= 0)
    *pnread = (माप_प्रकार)nread;
  अन्यथा {
    अगर((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      परिणाम = CURLE_AGAIN;
      जाओ out;
    }
    अन्यथा अगर(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      परिणाम = cf_gtls_handshake(cf, तथ्य);
      अगर(!परिणाम)
        परिणाम = CURLE_AGAIN; /* make us get called again. */
      जाओ out;
    }
    अन्यथा {
      failf(तथ्य, "GnuTLS recv error (%d): %s",
            (पूर्णांक)nread, gnutls_strerror((पूर्णांक)nread));
      परिणाम = backend->gtls.io_परिणाम ?
        backend->gtls.io_परिणाम : CURLE_RECV_ERROR;
      जाओ out;
    }
  }

out:
  CURL_TRC_CF(तथ्य, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (पूर्णांक)परिणाम, nread);
  वापस परिणाम;
}

माप_प्रकार Curl_gtls_version(अक्षर *बफर, माप_प्रकार आकार)
{
  वापस curl_msnprintf(बफर, आकार, "GnuTLS/%s", gnutls_check_version(शून्य));
}

/* data might be NULL! */
अटल CURLcode gtls_random(काष्ठा Curl_easy *तथ्य,
                            अचिन्हित अक्षर *entropy, माप_प्रकार लंबाई)
{
  पूर्णांक rc;
  (व्योम)तथ्य;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, लंबाई);
  वापस rc ? CURLE_FAILED_INIT : CURLE_OK;
}

अटल CURLcode gtls_sha256sum(स्थिर अचिन्हित अक्षर *निवेश,
                               माप_प्रकार len,
                               अचिन्हित अक्षर *sha256sum, /* output */
                               माप_प्रकार sha256len)
{
  काष्ठा sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  करो {
    अचिन्हित पूर्णांक ilen = (अचिन्हित पूर्णांक)CURLMIN(len, अच_पूर्णांक_उच्च);
    sha256_update(&SHA256pw, ilen, निवेश);
    len -= ilen;
    निवेश += ilen;
  } जबतक(len);
#अगर NETTLE_VERSION_MAJOR >= 4
  (व्योम)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#अन्यथा
  sha256_digest(&SHA256pw, (अचिन्हित पूर्णांक)sha256len, sha256sum);
#पूर्ण_अगर
  वापस CURLE_OK;
}

अटल bool gtls_cert_status_request(व्योम)
{
  वापस TRUE;
}

अटल व्योम *gtls_get_internals(काष्ठा ssl_connect_तथ्य *connssl,
                                CURLINFO info)
{
  काष्ठा gtls_ssl_backend_तथ्य *backend =
    (काष्ठा gtls_ssl_backend_तथ्य *)connssl->backend;
  (व्योम)info;
  DEBUGASSERT(backend);
  वापस backend->gtls.session;
}

स्थिर काष्ठा Curl_ssl Curl_ssl_gnutls = {
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

  माप(काष्ठा gtls_ssl_backend_तथ्य),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_version,             /* version */
  gtls_shutdown,                 /* shutdown */
  gtls_तथ्य_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_status_request,      /* cert_status_request */
  gtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,       /* adjust_pollset */
  gtls_get_internals,            /* get_internals */
  gtls_close,                    /* close_one */
  शून्य,                          /* close_all */
  शून्य,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  शून्य,                          /* get_channel_binding */
};

#पूर्ण_अगर /* USE_GNUTLS */
