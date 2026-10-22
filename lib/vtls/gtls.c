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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_GNUTLS

#অন্তর্ভাব <gnutls/abstract.h>
#অন্তর্ভাব <gnutls/gnutls.h>
#অন্তর্ভাব <gnutls/x509.h>
#অন্তর্ভাব <gnutls/crypto.h>
#অন্তর্ভাব <nettle/sha2.h>
#অন্তর্ভাব <nettle/version.h>

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "vtls/keylog.h"
#অন্তর্ভাব "vtls/gtls.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vtls/apple.h"
#অন্তর্ভাব "vauth/vauth.h"
#অন্তর্ভাব "parsedate.h"
#অন্তর্ভাব "connect.h" /* for the connect timeout */
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#যদি 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
স্থির শূন্য tls_log_func(পূর্ণসংখ্যা level, ধ্রুবক অক্ষর *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#যদি !defined(GNUTLS_VERSION_সংখ্যা) || (GNUTLS_VERSION_সংখ্যা < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_তথ্য
#যদি GNUTLS_VERSION_সংখ্যা >= 0x030702
#define CURL_GNUTLS_EARLY_তথ্য
#endif

#অন্তর্ভাব <gnutls/ocsp.h>

কাঠামো gtls_ssl_backend_তথ্য {
  কাঠামো gtls_ctx gtls;
};

স্থির ssize_t gtls_ঠেলো(শূন্য *s, ধ্রুবক শূন্য *buf, size_t blen)
{
  কাঠামো Curl_cfilter *cf = s;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  size_t nwritten;
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য);
  ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(তথ্য, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (পূর্ণসংখ্যা)ফলাফল, nwritten);
  backend->gtls.io_ফলাফল = ফলাফল;
  যদি(ফলাফল) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (ফলাফল == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ফেরত -1;
  }
  ফেরত (ssize_t)nwritten;
}

স্থির ssize_t gtls_pull(শূন্য *s, শূন্য *buf, size_t blen)
{
  কাঠামো Curl_cfilter *cf = s;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  size_t nread;
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য);
  যদি(!backend->gtls.shared_creds->trust_setup) {
    ফলাফল = Curl_gtls_client_trust_setup(cf, তথ্য, &backend->gtls);
    যদি(ফলাফল) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_ফলাফল = ফলাফল;
      ফেরত -1;
    }
  }

  ফলাফল = Curl_conn_cf_recv(cf->পরবর্তী, তথ্য, buf, blen, &nread);
  CURL_TRC_CF(তথ্য, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (পূর্ণসংখ্যা)ফলাফল,
              nread);
  backend->gtls.io_ফলাফল = ফলাফল;
  যদি(ফলাফল) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (ফলাফল == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ফেরত -1;
  }
  অন্যথা যদি(nread == 0)
    connssl->peer_closed = TRUE;
  ফেরত (ssize_t)nread;
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
স্থির পূর্ণসংখ্যা gtls_init(শূন্য)
{
  পূর্ণসংখ্যা ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  ফেরত ret;
}

স্থির শূন্য gtls_cleanup(শূন্য)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
স্থির শূন্য showtime(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *text, time_t stamp)
{
  কাঠামো tm বাফার;
  ধ্রুবক কাঠামো tm *tm = &বাফার;
  অক্ষর str[96];
  CURLcode ফলাফল = curlx_gmtime(stamp, &বাফার);
  যদি(ফলাফল)
    ফেরত;

  curl_msnprintf(str,
                 আকার(str),
                 "  %s: %s, %02d %s %4d %02d:%02d:%02d GMT",
                 text,
                 Curl_wkday[tm->tm_wday ? tm->tm_wday-1 : 6],
                 tm->tm_mday,
                 Curl_month[tm->tm_mon],
                 tm->tm_year + 1900,
                 tm->tm_hour,
                 tm->tm_min,
                 tm->tm_sec);
  infof(তথ্য, "%s", str);
}
#endif

স্থির gnutls_datum_t load_file(ধ্রুবক অক্ষর *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { ফাঁকা, 0 };
  long filelen;
  শূন্য *ptr;

  f = curlx_fopen(file, "rb");
  যদি(!f)
    ফেরত loaded_file;
  যদি(fseek(f, 0, SEEK_END))
    যাও out;
  filelen = ftell(f);
  যদি(filelen < 0 || filelen > CURL_MAX_প্রবেশ_দৈর্ঘ্য)
    যাও out;
  যদি(fseek(f, 0, SEEK_SET))
    যাও out;
  ptr = curlx_malloc((size_t)filelen);
  যদি(!ptr)
    যাও out;
  যদি(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    যাও out;
  }

  loaded_file.তথ্য = ptr;
  loaded_file.আকার_2 = (unsigned পূর্ণসংখ্যা)filelen;
out:
  curlx_fclose(f);
  ফেরত loaded_file;
}

স্থির শূন্য unload_file(gnutls_datum_t তথ্য)
{
  curlx_free(তথ্য.তথ্য);
}

/* this function does an SSL/TLS (re-)handshake */
স্থির CURLcode cf_gtls_handshake(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  gnutls_session_t session;
  পূর্ণসংখ্যা rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_ফলাফল = CURLE_OK;
  rc = gnutls_handshake(session);

  যদি(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode ফলাফল = Curl_gtls_client_trust_setup(cf, তথ্য, &backend->gtls);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    ফেরত CURLE_AGAIN;
  }
  অন্যথা যদি((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    ধ্রুবক অক্ষর *strerr = ফাঁকা;

    যদি(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    যদি(!strerr)
      strerr = gnutls_strerror(rc);

    infof(তথ্য, "gnutls_handshake() warning: %s", strerr);
#endif
    ফেরত CURLE_AGAIN;
  }
  অন্যথা যদি((rc < 0) && backend->gtls.io_ফলাফল) {
    ফেরত backend->gtls.io_ফলাফল;
  }
  অন্যথা যদি(rc < 0) {
    ধ্রুবক অক্ষর *strerr = ফাঁকা;

    যদি(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    যদি(!strerr)
      strerr = gnutls_strerror(rc);

    failf(তথ্য, "GnuTLS, handshake failed: %s", strerr);
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  ফেরত CURLE_OK;
}

স্থির gnutls_x509_crt_fmt_t gnutls_do_file_type(ধ্রুবক অক্ষর *type)
{
  যদি(!type || !type[0])
    ফেরত GNUTLS_X509_FMT_PEM;
  যদি(curl_strequal(type, "PEM"))
    ফেরত GNUTLS_X509_FMT_PEM;
  যদি(curl_strequal(type, "DER"))
    ফেরত GNUTLS_X509_FMT_DER;
  ফেরত GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

স্থির CURLcode gnutls_set_ssl_version_min_max(
  কাঠামো Curl_easy *তথ্য,
  কাঠামো ssl_peer *peer,
  কাঠামো ssl_filter_config *conn_config,
  ধ্রুবক অক্ষর **prioritylist,
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
      failf(তথ্য, "QUIC needs at least TLS version 1.3");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    ফেরত CURLE_OK;
  }

  নির্বাচন(ssl_version | ssl_version_max) {
  ক্ষেত্র CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    ফেরত CURLE_OK;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    ফেরত CURLE_OK;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    ফেরত CURLE_OK;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    ফেরত CURLE_OK;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    ফেরত CURLE_OK;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    ফেরত CURLE_OK;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    ফেরত CURLE_OK;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    ফেরত CURLE_OK;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    ফেরত CURLE_OK;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    ফেরত CURLE_OK;
  }

  failf(তথ্য, "GnuTLS: cannot set TLS protocol");
  ফেরত CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(কাঠামো Curl_easy *তথ্য,
                                       কাঠামো gtls_shared_creds **pcreds)
{
  কাঠামো gtls_shared_creds *shared;
  পূর্ণসংখ্যা rc;

  *pcreds = ফাঁকা;
  shared = curlx_calloc(1, আকার(*shared));
  যদি(!shared)
    ফেরত CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  যদি(rc != GNUTLS_E_SUCCESS) {
    failf(তথ্য, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(তথ্য);
  *pcreds = shared;
  ফেরত CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(কাঠামো gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  যদি(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    ফেরত CURLE_OK;
  }
  ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
}

শূন্য Curl_gtls_shared_creds_free(কাঠামো gtls_shared_creds **pcreds)
{
  কাঠামো gtls_shared_creds *shared = *pcreds;
  *pcreds = ফাঁকা;
  যদি(shared) {
    --shared->refcount;
    যদি(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

স্থির CURLcode gtls_populate_creds(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    gnutls_certificate_credentials_t creds)
{
  কাঠামো ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, তথ্য);
  bool creds_are_empty = TRUE;
  পূর্ণসংখ্যা rc;

  যদি(!config->verifypeer) {
    infof(তথ্য, "SSL Trust: peer verification disabled");
    ফেরত CURLE_OK;
  }

  infof(তথ্য, "SSL Trust Anchors:");
  যদি(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(তথ্য, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#অন্যথা
    rc = gnutls_certificate_set_x509_system_trust(creds);
    যদি(rc < 0)
      infof(তথ্য, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    অন্যথা {
      infof(তথ্য, "  Native: %d certificates from system trust", rc);
      যদি(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  যদি(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    যদি(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(তথ্য, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      ফেরত CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.তথ্য = config->ca_info_blob->তথ্য;
    ca_info_datum.আকার_2 = (unsigned পূর্ণসংখ্যা)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    যদি(rc < 0) {
      infof(তথ্য, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      যদি(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ফেরত CURLE_SSL_CACERT_BADFILE;
      }
    }
    অন্যথা
      infof(তথ্য, "  CA Blob: %d certificates", rc);
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
      infof(তথ্য, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      যদি(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ফেরত CURLE_SSL_CACERT_BADFILE;
      }
    }
    অন্যথা
      infof(তথ্য, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  যদি(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    যদি(rc < 0) {
      infof(তথ্য, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      যদি(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ফেরত CURLE_SSL_CACERT_BADFILE;
      }
    }
    অন্যথা
      infof(তথ্য, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  যদি(creds_are_empty)
    infof(তথ্য, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  যদি(config->CRLfile && config->native_ca_store) {
    failf(তথ্য, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ফেরত CURLE_NOT_BUILT_IN;
  }
#endif
  যদি(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    যদি(rc < 0) {
      failf(তথ্য, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      ফেরত CURLE_SSL_CRL_BADFILE;
    }
    অন্যথা
      infof(তথ্য, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  ফেরত CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_চাবি   "tls:gtls:x509:share"

স্থির bool gtls_shared_creds_expired(কাঠামো Curl_easy *তথ্য,
                                      ধ্রুবক কাঠামো gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(তথ্য), &sc->time);
  timediff_t timeout_ms = তথ্য->set.ssl_ca_cache_timeout * (timediff_t)1000;

  যদি(timeout_ms < 0)
    ফেরত FALSE;

  ফেরত elapsed_ms >= timeout_ms;
}

স্থির bool gtls_shared_creds_different(কাঠামো Curl_cfilter *cf,
                                        ধ্রুবক কাঠামো gtls_shared_creds *sc)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  যদি(!sc->CAfile || !conn_config->CAfile)
    ফেরত sc->CAfile != conn_config->CAfile;

  ফেরত strcmp(sc->CAfile, conn_config->CAfile);
}

স্থির কাঠামো gtls_shared_creds *gtls_get_cached_creds(কাঠামো Curl_cfilter *cf,
                                                       কাঠামো Curl_easy *তথ্য)
{
  কাঠামো gtls_shared_creds *shared_creds;

  যদি(তথ্য->multi) {
    shared_creds = Curl_hash_pick(&তথ্য->multi->proto_hash,
                                  MPROTO_GTLS_X509_চাবি,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_চাবি));
    যদি(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(তথ্য, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      ফেরত shared_creds;
    }
  }
  ফেরত ফাঁকা;
}

স্থির শূন্য gtls_shared_creds_hash_free(ধ্রুবক শূন্য *চাবি,
                                        size_t চাবি_len, শূন্য *p)
{
  কাঠামো gtls_shared_creds *sc = p;
  DEBUGASSERT(চাবি_len == CURL_CSTRLEN(MPROTO_GTLS_X509_চাবি));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_চাবি, চাবি, চাবি_len));
  (শূন্য)চাবি;
  (শূন্য)চাবি_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

স্থির শূন্য gtls_set_cached_creds(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো gtls_shared_creds *sc)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  যদি(!তথ্য->multi)
    ফেরত;

  যদি(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    যদি(!sc->CAfile)
      ফেরত;
  }

  যদি(Curl_gtls_shared_creds_up_ref(sc))
    ফেরত;

  যদি(!Curl_hash_add2(&তথ্য->multi->proto_hash,
                     MPROTO_GTLS_X509_চাবি,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_চাবি),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    ফেরত;
  }
}

CURLcode Curl_gtls_client_trust_setup(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো gtls_ctx *gtls)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো gtls_shared_creds *cached_creds = ফাঁকা;
  bool cache_criteria_met;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা rc;

  /* Consider the X509 store cacheable if it comes exclusively from a CAfile,
     or no source is provided and we are falling back to OpenSSL's built-in
     default. */
  cache_criteria_met = (তথ্য->set.ssl_ca_cache_timeout != 0) &&
    conn_config->verifypeer &&
    !conn_config->CApath &&
    !conn_config->ca_info_blob &&
    !conn_config->CRLfile &&
    !conn_config->native_ca_store &&
    !conn_config->clientcert; /* GnuTLS adds client cert to its credentials! */

  যদি(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, তথ্য);

  যদি(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(তথ্য, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    যদি(rc != GNUTLS_E_SUCCESS) {
      failf(তথ্য, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }
  অন্যথা {
    CURL_TRC_CF(তথ্য, cf, "loading trust anchors and CRLs");
    ফলাফল = gtls_populate_creds(cf, তথ্য, gtls->shared_creds->creds);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    gtls->shared_creds->trust_setup = TRUE;
    যদি(cache_criteria_met)
      gtls_set_cached_creds(cf, তথ্য, gtls->shared_creds);
  }
  ফেরত CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_তথ্য
স্থির পূর্ণসংখ্যা gtls_get_ietf_proto(gnutls_session_t session)
{
  নির্বাচন(gnutls_protocol_get_version(session)) {
  ক্ষেত্র GNUTLS_SSL3:
    ফেরত CURL_IETF_PROTO_SSL3;
  ক্ষেত্র GNUTLS_TLS1_0:
    ফেরত CURL_IETF_PROTO_TLS1;
  ক্ষেত্র GNUTLS_TLS1_1:
    ফেরত CURL_IETF_PROTO_TLS1_1;
  ক্ষেত্র GNUTLS_TLS1_2:
    ফেরত CURL_IETF_PROTO_TLS1_2;
  ক্ষেত্র GNUTLS_TLS1_3:
    ফেরত CURL_IETF_PROTO_TLS1_3;
  পূর্বনির্ধারিত:
    ফেরত CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক অক্ষর *ssl_peer_চাবি,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ধ্রুবক অক্ষর *alpn,
                                 unsigned অক্ষর *quic_tp,
                                 size_t quic_tp_len,
                                 কাঠামো Curl_ssl_session **psession)
{
  কাঠামো Curl_ssl_session *sc_session = ফাঁকা, *sc_dup = ফাঁকা;
  unsigned অক্ষর *sdata, *qtp_clone = ফাঁকা;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode ফলাফল = CURLE_OK;

  যদি(psession)
    *psession = ফাঁকা;
  যদি(!Curl_ssl_scache_use(cf, তথ্য))
    ফেরত CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_তথ্য(session, ফাঁকা, &sdata_len);
  যদি(!sdata_len) /* GnuTLS does this for some version combinations */
    ফেরত CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  যদি(!sdata)
    ফেরত CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_তথ্য(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_তথ্য_আকার(session);

  CURL_TRC_CF(তথ্য, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  যদি(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ধ্রুবক অক্ষর *)quic_tp, quic_tp_len);
    যদি(!qtp_clone) {
      curlx_free(sdata);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
  }

  ফলাফল = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  যদি(!ফলাফল && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    ফলাফল = Curl_ssl_session_dup(sc_session, &sc_dup);
  যদি(!ফলাফল) {
    ফলাফল = Curl_ssl_scache_put(cf, তথ্য, ssl_peer_চাবি, sc_session);
    /* took ownership of `sc_session` */
    sc_session = ফাঁকা;
  }
  যদি(!ফলাফল && psession) {
    *psession = sc_dup;
    sc_dup = ফাঁকা;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ফেরত ফলাফল;
}
#endif

#ifdef CURL_GNUTLS_EARLY_তথ্য
স্থির CURLcode cf_gtls_update_session_id(কাঠামো Curl_cfilter *cf,
                                          কাঠামো Curl_easy *তথ্য,
                                          gnutls_session_t session)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো Curl_ssl_session *scs = ফাঁকা;
  CURLcode ফলাফল;

  ফলাফল = Curl_gtls_cache_session(cf, তথ্য, connssl->peer.scache_চাবি,
                                   session, 0, connssl->negotiated.alpn,
                                   ফাঁকা, 0, &scs);
  যদি(!ফলাফল) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা gtls_handshake_cb(gnutls_session_t session, unsigned পূর্ণসংখ্যা htype,
                             unsigned when, unsigned পূর্ণসংখ্যা incoming,
                             ধ্রুবক gnutls_datum_t *বার্তা)
{
  কাঠামো Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (শূন্য)বার্তা;
  (শূন্য)incoming;
  যদি(when) { /* after message has been processed */
    কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
    যদি(তথ্য) {
      CURL_TRC_CF(তথ্য, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      নির্বাচন(htype) {
      ক্ষেত্র GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, তথ্য, session);
        বিরতি;
      }
      পূর্বনির্ধারিত:
        বিরতি;
      }
    }
  }
  ফেরত 0;
}
#endif

স্থির CURLcode gtls_set_priority(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো gtls_ctx *gtls,
                                  ধ্রুবক অক্ষর *priority)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো dynbuf buf;
  ধ্রুবক অক্ষর *err = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা rc;

  curlx_dyn_init(&buf, 4096);

  যদি(conn_config->cipher_list) {
    যদি((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      যদি(!curlx_dyn_len(&buf)) {  /* not added yet */
        ফলাফল = curlx_dyn_add(&buf, priority);
        যদি(ফলাফল)
          যাও out;
      }
      ফলাফল = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      যদি(ফলাফল)
        যাও out;
      priority = curlx_dyn_ptr(&buf);
    }
    অন্যথা /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(তথ্য, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  যদি(rc != GNUTLS_E_SUCCESS) {
    failf(তথ্য, "Error %d setting GnuTLS priority: %s", rc, err);
    ফলাফল = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  ফেরত ফলাফল;
}

স্থির CURLcode gtls_client_init(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ssl_peer *peer,
                                 size_t earlydata_max,
                                 কাঠামো gtls_ctx *gtls)
{
  কাঠামো ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, তথ্য);
  unsigned পূর্ণসংখ্যা init_flags;
  পূর্ণসংখ্যা rc;
  ধ্রুবক অক্ষর *prioritylist;
  bool tls13support;
  CURLcode ফলাফল;

  যদি(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(তথ্য, "GnuTLS does not support SSLv2 or SSLv3");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  ফলাফল = Curl_gtls_shared_creds_create(তথ্য, &gtls->shared_creds);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_তথ্য
  যদি(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_তথ্য | GNUTLS_NO_END_OF_EARLY_তথ্য;
  অন্যথা যদি(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
    /* See https://gitlab.com/gnutls/gnutls/-/issues/1619
     * We cannot differentiate between a session announcing no earldata
     * and one announcing 0xFFFFFFFFUL. On TCP+TLS, this is unlikely, but
     * on QUIC this is common. */
    init_flags |= GNUTLS_ENABLE_EARLY_তথ্য;
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

  CURL_TRC_CF(তথ্য, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  যদি(rc != GNUTLS_E_SUCCESS) {
    failf(তথ্য, "gnutls_init() failed: %d", rc);
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  যদি(peer->sni) {
    যদি(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(তথ্য, "Failed to set SNI");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  যদি(rc != GNUTLS_E_SUCCESS)
    ফেরত CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  যদি(config->version == CURL_SSLVERSION_TLSv1_3) {
    যদি(!tls13support) {
      failf(তথ্য, "This GnuTLS installation does not support TLS 1.3");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  ফলাফল = gnutls_set_ssl_version_min_max(তথ্য, peer,
                                          config, &prioritylist, tls13support);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = gtls_set_priority(cf, তথ্য, gtls, prioritylist);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(config->clientcert) {
    যদি(!gtls->shared_creds->trust_setup) {
      ফলাফল = Curl_gtls_client_trust_setup(cf, তথ্য, gtls);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    যদি(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->চাবি_passwd ? config->চাবি_passwd : "");
      যদি(rc != GNUTLS_E_SUCCESS) {
        failf(তথ্য,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        ফেরত CURLE_SSL_CONNECT_ERROR;
      }
    }
    অন্যথা {
      ধ্রুবক unsigned পূর্ণসংখ্যা supported_চাবি_encryption_algorithms =
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
        failf(তথ্য,
              "error reading X.509 %skey file: %s",
              config->চাবি_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        ফেরত CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  যদি(rc != GNUTLS_E_SUCCESS) {
    failf(তথ্য, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  যদি(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  ফাঁকা, 0, ফাঁকা);
    যদি(rc != GNUTLS_E_SUCCESS) {
      failf(তথ্য, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
  }

  ফেরত CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_তথ্য
স্থির পূর্ণসংখ্যা keylog_callback(gnutls_session_t session, ধ্রুবক অক্ষর *label,
                           ধ্রুবক gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  যদি(crandom.আকার_2 != 32) {
    ফেরত -1;
  }

  Curl_tls_keylog_write(label, crandom.তথ্য, crandom.আকার_2,
                        secret->তথ্য, secret->আকার_2);
  ফেরত 0;
}

স্থির CURLcode gtls_on_session_reuse(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো alpn_spec *alpns,
                                      কাঠামো Curl_ssl_session *scs,
                                      bool *do_early_তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_তথ্য_আকার(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  ফেরত Curl_on_session_reuse(cf, তথ্য, alpns, scs, do_early_তথ্য,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

স্থির CURLcode gtls_apply_session(
  কাঠামো gtls_ctx *gctx,
  কাঠামো Curl_cfilter *cf,
  কাঠামো Curl_easy *তথ্য,
  কাঠামো ssl_peer *peer,
  কাঠামো alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  কাঠামো Curl_ssl_session *scs,
  bool *pwas_setup)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা rc;

  যদি(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    ফলাফল = gtls_client_init(cf, তথ্য, peer, scs->earlydata_max, gctx);
    যদি(ফলাফল)
      যাও out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_তথ্য(gctx->session, scs->sdata, scs->sdata_len);
    যদি(rc < 0)
      infof(তথ্য, "SSL session not accepted by GnuTLS, continuing without");
    অন্যথা {
      infof(তথ্য, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      যদি(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_তথ্য = FALSE;
        যদি(sess_reuse_cb) {
          ফলাফল = sess_reuse_cb(cf, তথ্য, alpns, scs, &do_early_তথ্য);
          যদি(ফলাফল)
            যাও out;
        }
        যদি(do_early_তথ্য) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  ফেরত ফলাফল;
}

CURLcode Curl_gtls_ctx_init(কাঠামো gtls_ctx *gctx,
                            কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            কাঠামো ssl_peer *peer,
                            ধ্রুবক কাঠামো alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            শূন্য *cb_user_তথ্য,
                            শূন্য *ssl_user_তথ্য,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো Curl_ssl_session *scs = ফাঁকা;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_গুনতি = 0;
  bool gtls_session_setup = FALSE;
  কাঠামো alpn_spec alpns;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  যদি((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(তথ্য, cf->cft, FIRSTSOCKET);
    যদি(scs) {
      ফলাফল = gtls_apply_session(gctx, cf, তথ্য, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = ফাঁকা;
      যদি(!ফলাফল && gtls_session_setup) {
        CURL_TRC_CF(তথ্য, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  যদি(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    ফলাফল = Curl_ssl_scache_take(cf, তথ্য, peer->scache_চাবি, &scs);
    যদি(ফলাফল)
      যাও out;
    ফলাফল = gtls_apply_session(gctx, cf, তথ্য, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  যদি(!gtls_session_setup) {
    ফলাফল = gtls_client_init(cf, তথ্য, peer, 0, gctx);
    যদি(ফলাফল)
      যাও out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_তথ্য);

  যদি(cb_setup) {
    ফলাফল = cb_setup(cf, তথ্য, cb_user_তথ্য);
    যদি(ফলাফল)
      যাও out;
  }

#ifdef CURL_GNUTLS_EARLY_তথ্য
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
    জন্য(i = 0; i < alpns.গুনতি; ++i) {
      gtls_alpns[i].তথ্য = (unsigned অক্ষর *)alpns.entries[i];
      gtls_alpns[i].আকার_2 = (unsigned পূর্ণসংখ্যা)strlen(alpns.entries[i]);
    }
    gtls_alpns_গুনতি = alpns.গুনতি;
  }

  যদি(gtls_alpns_গুনতি &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned পূর্ণসংখ্যা)gtls_alpns_গুনতি,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(তথ্য, "failed setting ALPN");
    ফলাফল = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, তথ্য, peer->scache_চাবি, scs);
  ফেরত ফলাফল;
}

স্থির CURLcode gtls_connect_step1(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল;

  DEBUGASSERT(backend);

  যদি(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    ফেরত CURLE_OK;

  ফলাফল = Curl_gtls_ctx_init(&backend->gtls, cf, তথ্য, &connssl->peer,
                              connssl->alpn, ফাঁকা, ফাঁকা, cf,
#ifdef CURL_GNUTLS_EARLY_তথ্য
                              gtls_on_session_reuse
#অন্যথা
                              ফাঁকা
#endif
                              );

  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    কাঠামো alpn_proto_buf proto;
    memset(&proto, 0, আকার(proto));
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(তথ্য, VTLS_INFOF_ALPN_OFFER_1STR, proto.তথ্য);
  }

#ifdef CURL_GNUTLS_EARLY_তথ্য
  gnutls_handshake_set_hook_function(backend->gtls.session,
                                     GNUTLS_HANDSHAKE_ANY, GNUTLS_HOOK_POST,
                                     gtls_handshake_cb);
#endif

  /* register callback functions and handle to send and receive data. */
  gnutls_transport_set_ptr(backend->gtls.session, cf);
  gnutls_transport_set_ঠেলো_function(backend->gtls.session, gtls_ঠেলো);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  ফেরত CURLE_OK;
}

স্থির CURLcode pkp_pin_peer_pubkey(কাঠামো Curl_easy *তথ্য,
                                    gnutls_x509_crt_t cert,
                                    ধ্রুবক অক্ষর *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned অক্ষর *buff1 = ফাঁকা;

  gnutls_pubkey_t চাবি = ফাঁকা;

  /* Result is returned to caller */
  CURLcode ফলাফল = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  যদি(!pinnedpubkey)
    ফেরত CURLE_OK;

  যদি(!cert)
    ফেরত ফলাফল;

  করো {
    পূর্ণসংখ্যা ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&চাবি);
    যদি(ret < 0)
      বিরতি; /* failed */

    ret = gnutls_pubkey_import_x509(চাবি, cert, 0);
    যদি(ret < 0)
      বিরতি; /* failed */

    ret = gnutls_pubkey_export(চাবি, GNUTLS_X509_FMT_DER, ফাঁকা, &len1);
    যদি(ret != GNUTLS_E_SHORT_MEMORY_বাফার || len1 == 0)
      বিরতি; /* failed */

    buff1 = curlx_malloc(len1);
    যদি(!buff1)
      বিরতি; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(চাবি, GNUTLS_X509_FMT_DER, buff1, &len2);
    যদি(ret < 0 || len1 != len2)
      বিরতি; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    ফলাফল = Curl_pin_peer_pubkey(তথ্য, pinnedpubkey, buff1, len1);
  } যখন(0);

  যদি(চাবি)
    gnutls_pubkey_deinit(চাবি);

  curlx_safefree(buff1);

  ফেরত ফলাফল;
}

শূন্য Curl_gtls_report_handshake(কাঠামো Curl_easy *তথ্য, কাঠামো gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  যদি(Curl_trc_is_verbose(তথ্য)) {
    ধ্রুবক অক্ষর *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(তথ্য, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)gctx;
#endif
}

স্থির শূন্য gtls_বার্তা_verify_ফলাফল(কাঠামো Curl_easy *তথ্য,
                                   কাঠামো ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  অক্ষর certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t আকার_2 = আকার(certname);
  পূর্ণসংখ্যা rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &আকার_2);
  যদি(rc) {
    infof(তথ্য, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  যদি(!was_verified) {
    যদি(needs_verified) {
      failf(তথ্য, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    অন্যথা
      infof(তথ্য, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  অন্যথা
    infof(তথ্য, "  common name: %s (matched)", certname);
}

স্থির শূন্য gtls_infof_cert(কাঠামো Curl_easy *তথ্য,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  যদি(Curl_trc_is_verbose(তথ্য)) {
    gnutls_datum_t certfields;
    পূর্ণসংখ্যা rc, algo;
    time_t tstamp;
    unsigned পূর্ণসংখ্যা bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(তথ্য, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(তথ্য, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    যদি(rc)
      infof(তথ্য, "Failed to get certificate name");
    অন্যথা {
      infof(তথ্য, "  subject: %s", certfields.তথ্য);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(তথ্য, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(তথ্য, "expire date", tstamp);

      gnutls_free(certfields.তথ্য);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    যদি(rc)
      infof(তথ্য, "Failed to get certificate issuer");
    অন্যথা {
      infof(তথ্য, "  issuer: %s", certfields.তথ্য);

      gnutls_free(certfields.তথ্য);
    }
  }
#অন্যথা
  (শূন্য)তথ্য;
  (শূন্য)x509_cert;
#endif
}

স্থির CURLcode gtls_verify_ocsp_status(কাঠামো Curl_easy *তথ্য,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = ফাঁকা;
  CURLcode ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
  পূর্ণসংখ্যা rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  যদি(rc == GNUTLS_E_REQUESTED_তথ্য_NOT_AVAILABLE) {
    failf(তথ্য, "No OCSP response received");
    যাও out;
  }
  অন্যথা যদি(rc < 0) {
    failf(তথ্য, "Invalid OCSP response received");
    যাও out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  যদি(rc < 0) {
    failf(তথ্য, "failed to init OCSP response parsing");
    যাও out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  যদি(rc < 0) {
    failf(তথ্য, "failed to parse OCSP response");
    যাও out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  যদি(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(তথ্য, "OCSP response basic part is not successful");
    যাও out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  জন্য(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    যদি(!rc || (rc == GNUTLS_E_REQUESTED_তথ্য_NOT_AVAILABLE))
      বিরতি;
  }
  যদি(rc < 0) {
    failf(তথ্য, "OCSP does not carry status for peer cert");
    যাও out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা,
                                   &status, ফাঁকা, ফাঁকা, ফাঁকা, ফাঁকা);
  যদি(rc < 0) {
    failf(তথ্য, "failed to parse OCSP response for peer cert");
    যাও out;
  }
  নির্বাচন(status) {
  ক্ষেত্র GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    ফলাফল = CURLE_OK;
    বিরতি;
  ক্ষেত্র GNUTLS_OCSP_CERT_REVOKED:
    failf(তথ্য, "peer certificate is revoked");
    বিরতি;
  পূর্বনির্ধারিত:
    failf(তথ্য, "peer certificate OCSP status is not good: %d", (পূর্ণসংখ্যা)status);
    বিরতি;
  }

out:
  যদি(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  ফেরত ফলাফল;
}

কাঠামো gtls_cert_chain {
  ধ্রুবক gnutls_datum_t *certs;
  unsigned পূর্ণসংখ্যা অঙ্ক_certs;
};

#ifdef USE_APPLE_SECTRUST
স্থির CURLcode gtls_chain_get_der(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   শূন্য *user_তথ্য,
                                   size_t i,
                                   unsigned অক্ষর **pder,
                                   size_t *pder_len)
{
  কাঠামো gtls_cert_chain *chain = user_তথ্য;

  (শূন্য)cf;
  (শূন্য)তথ্য;
  *pder_len = 0;
  *pder = ফাঁকা;

  যদি(i >= chain->অঙ্ক_certs)
    ফেরত CURLE_TOO_LARGE;
  *pder = chain->certs[i].তথ্য;
  *pder_len = (size_t)chain->certs[i].আকার_2;
  ফেরত CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
স্থির CURLcode gtls_verify_cert(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো ssl_filter_config *config,
                                 কাঠামো ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 কাঠামো Curl_cfilter *cf,
                                 কাঠামো ssl_peer *peer,
                                 কাঠামো gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned পূর্ণসংখ্যা verify_status = 0;
  long * ধ্রুবক certverifyresult = &ssl_config->certverifyresult;
  পূর্ণসংখ্যা rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  যদি(rc < 0) {
    failf(তথ্য, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  যদি(verified)
    infof(তথ্য, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  যদি(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode ফলাফল;

    যদি(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.তথ্য = ফাঁকা;
      ocsp_raw.আকার_2 = 0;
    }
    যদি(config->verifystatus && !ocsp_raw.আকার_2) {
      failf(তথ্য, "No OCSP response received");
      ফেরত CURLE_SSL_INVALIDCERTSTATUS;
    }
    ফলাফল = Curl_vtls_apple_verify(cf, তথ্য, peer, chain->অঙ্ক_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.তথ্য, ocsp_raw.আকার_2,
                                    config->verifystatus);
    যদি(ফলাফল && (ফলাফল != CURLE_PEER_FAILED_VERIFICATION))
      ফেরত ফলাফল; /* no further failure analysis needed */
    verified = !ফলাফল;
    যদি(verified) {
      infof(তথ্য, "SSL certificate verified via Apple SecTrust.");
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
    CURLcode ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ধ্রুবক অক্ষর *cause = "certificate error, no details available";
    যদি(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    অন্যথা যদি(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    অন্যথা যদি(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    অন্যথা যদি(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      ফলাফল = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(তথ্য, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    ফেরত ফলাফল;
  }
  ফেরত CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                gnutls_session_t session,
                                কাঠামো ssl_filter_config *config,
                                কাঠামো ssl_easy_config *ssl_config,
                                কাঠামো ssl_peer *peer,
                                ধ্রুবক অক্ষর *pinned_চাবি)
{
  কাঠামো gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = ফাঁকা, x509_issuer = ফাঁকা;
  time_t certclock;
  bool ocsp_checked = FALSE;
  পূর্ণসংখ্যা rc;
  CURLcode ফলাফল = CURLE_OK;
  long * ধ্রুবক certverifyresult = &ssl_config->certverifyresult;

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
      failf(তথ্য, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
      যাও out;
    }
    infof(তথ্য, " common name: WARNING could not obtain");
  }

  যদি(তথ্য->set.ssl.certinfo && chain.certs) {
    যদি(chain.অঙ্ক_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(তথ্য, "%u certificates is more than allowed (%d)",
            chain.অঙ্ক_certs, MAX_ALLOWED_CERT_AMOUNT);
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও out;
    }
    অন্যথা {
      unsigned পূর্ণসংখ্যা i;

      ফলাফল = Curl_ssl_init_certinfo(তথ্য, (পূর্ণসংখ্যা)chain.অঙ্ক_certs);
      যদি(ফলাফল)
        যাও out;

      জন্য(i = 0; i < chain.অঙ্ক_certs; i++) {
        ধ্রুবক uint8_t *beg = chain.certs[i].তথ্য;
        ধ্রুবক uint8_t *end = beg + chain.certs[i].আকার_2;

        ফলাফল = Curl_extract_certinfo(তথ্য, (পূর্ণসংখ্যা)i, beg, end);
        যদি(ফলাফল)
          যাও out;
      }
    }
  }

  যদি(config->verifypeer) {
    ফলাফল = gtls_verify_cert(তথ্য, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    যদি(ফলাফল)
      যাও out;
  }
  অন্যথা
    infof(তথ্য, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  যদি(gnutls_x509_crt_init(&x509_cert)) {
    failf(তথ্য, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    ফলাফল = CURLE_SSL_CONNECT_ERROR;
    যাও out;
  }

  যদি(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    যদি(rc) {
      failf(তথ্য, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  যদি(certclock == (time_t)-1) {
    যদি(config->verifypeer) {
      failf(তথ্য, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও out;
    }
    অন্যথা
      infof(তথ্য, "  SSL certificate expiration date verify FAILED");
  }
  অন্যথা {
    যদি(certclock < time(ফাঁকা)) {
      যদি(config->verifypeer) {
        failf(তথ্য, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
        যাও out;
      }
      অন্যথা
        infof(তথ্য, "  SSL certificate expiration date FAILED");
    }
    অন্যথা
      infof(তথ্য, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  যদি(certclock == (time_t)-1) {
    যদি(config->verifypeer) {
      failf(তথ্য, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও out;
    }
    অন্যথা
      infof(তথ্য, "  SSL certificate activation date verify FAILED");
  }
  অন্যথা {
    যদি(certclock > time(ফাঁকা)) {
      যদি(config->verifypeer) {
        failf(তথ্য, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        ফলাফল = CURLE_PEER_FAILED_VERIFICATION;
        যাও out;
      }
      অন্যথা
        infof(তথ্য, "  SSL certificate activation date FAILED");
    }
    অন্যথা
      infof(তথ্য, "  SSL certificate activation date OK");
  }

  যদি(config->verifystatus && !ocsp_checked) {
    ফলাফল = gtls_verify_ocsp_status(তথ্য, session, x509_cert);
    যদি(ফলাফল)
      যাও out;
  }
  অন্যথা
    infof(তথ্য, "  SSL certificate status verification SKIPPED");

  যদি(config->issuercert) {
    gnutls_datum_t issuerp;
    যদি(gnutls_x509_crt_init(&x509_issuer)) {
      failf(তথ্য, "failed to init GnuTLS x509_crt for issuer");
      ফলাফল = CURLE_SSL_ISSUER_ERROR;
      যাও out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    যদি(rc) {
      failf(তথ্য, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      ফলাফল = CURLE_SSL_ISSUER_ERROR;
      যাও out;
    }
    rc = (পূর্ণসংখ্যা)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    যদি(rc <= 0) {
      failf(তথ্য, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      ফলাফল = CURLE_SSL_ISSUER_ERROR;
      যাও out;
    }
    infof(তথ্য, "  SSL certificate issuer check OK (Issuer Cert: %s)",
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
  rc = (পূর্ণসংখ্যা)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  ফলাফল = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_বার্তা_verify_ফলাফল(তথ্য, peer, x509_cert, rc, config->verifyhost);
  যদি(ফলাফল)
    যাও out;

  যদি(pinned_চাবি) {
    ফলাফল = pkp_pin_peer_pubkey(তথ্য, x509_cert, pinned_চাবি);
    যদি(ফলাফল != CURLE_OK) {
      failf(তথ্য, "SSL: public key does not match pinned public key");
      যাও out;
    }
  }

  gtls_infof_cert(তথ্য, x509_cert);

out:
  যদি(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  যদি(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  ফেরত ফলাফল;
}

স্থির CURLcode gtls_verifyserver(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  gnutls_session_t session)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, তথ্য);
#ifndef CURL_DISABLE_PROXY
  ধ্রুবক অক্ষর *pinned_চাবি = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY);
#অন্যথা
  ধ্রুবক অক্ষর *pinned_চাবি = CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode ফলাফল;

  ফলাফল = Curl_gtls_verifyserver(cf, তথ্য, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_চাবি);
  যদি(ফলাফল)
    যাও out;

#ifdef CURL_GNUTLS_EARLY_তথ্য
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  যদি(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    ফলাফল = cf_gtls_update_session_id(cf, তথ্য, session);
#endif

out:
  ফেরত ফলাফল;
}

#ifdef CURL_GNUTLS_EARLY_তথ্য
স্থির CURLcode gtls_send_earlydata(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক unsigned অক্ষর *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_ফলাফল = CURLE_OK;
  যখন(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_তথ্য(backend->gtls.session, buf, blen);
    CURL_TRC_CF(তথ্য, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    যদি(n < 0) {
      যদি(n == GNUTLS_E_AGAIN)
        ফলাফল = CURLE_AGAIN;
      অন্যথা
        ফলাফল = backend->gtls.io_ফলাফল ?
                 backend->gtls.io_ফলাফল : CURLE_SEND_ERROR;
      যাও out;
    }
    অন্যথা যদি(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(তথ্য, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ফেরত ফলাফল;
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
স্থির CURLcode gtls_connect_common(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  যদি(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  যদি(connssl->connecting_state == ssl_connect_1) {
    ফলাফল = gtls_connect_step1(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
    connssl->connecting_state = ssl_connect_2;
  }

  যদি(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_তথ্য
    যদি(connssl->earlydata_state == ssl_earlydata_await) {
      যাও out;
    }
    অন্যথা যদি(connssl->earlydata_state == ssl_earlydata_sending) {
      ফলাফল = gtls_send_earlydata(cf, তথ্য);
      যদি(ফলাফল)
        যাও out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    ফলাফল = cf_gtls_handshake(cf, তথ্য);
    যদি(ফলাফল)
      যাও out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  যদি(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    পূর্ণসংখ্যা rc;

    Curl_gtls_report_handshake(তথ্য, &backend->gtls);

    ফলাফল = gtls_verifyserver(cf, তথ্য, backend->gtls.session);
    যদি(ফলাফল)
      যাও out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    যদি(rc) {  /* No ALPN from server */
      proto.তথ্য = ফাঁকা;
      proto.আকার_2 = 0;
    }

    ফলাফল = Curl_alpn_set_negotiated(cf, তথ্য, connssl,
                                      proto.তথ্য, proto.আকার_2);
    যদি(ফলাফল)
      যাও out;

#ifdef CURL_GNUTLS_EARLY_তথ্য
    যদি(connssl->earlydata_state > ssl_earlydata_none) {
      /* We should be in this state by now */
      DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sent);
      connssl->earlydata_state =
        (gnutls_session_get_flags(backend->gtls.session) &
         GNUTLS_SFLAGS_EARLY_তথ্য) ?
        ssl_earlydata_accepted : ssl_earlydata_rejected;
    }
#endif
    connssl->connecting_state = ssl_connect_done;
  }

  যদি(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  যদি(ফলাফল == CURLE_AGAIN) {
    *done = FALSE;
    ফেরত CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(তথ্য, cf, "gtls_connect_common() -> %d, done=%d", (পূর্ণসংখ্যা)ফলাফল,
              *done);
  ফেরত ফলাফল;
}

স্থির CURLcode gtls_connect(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_তথ্য
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  যদি((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    ফেরত CURLE_OK;
  }
#endif
  ফেরত gtls_connect_common(cf, তথ্য, done);
}

স্থির bool gtls_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                              ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *ctx = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend;

  (শূন্য)তথ্য;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (কাঠামো gtls_ssl_backend_তথ্য *)ctx->backend;
  যদি(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    ফেরত TRUE;
  ফেরত FALSE;
}

স্থির CURLcode gtls_send(কাঠামো Curl_cfilter *cf,
                          কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক শূন্য *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  যখন(remain) {
    backend->gtls.io_ফলাফল = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    যদি(nwritten >= 0) {
      যদি((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(তথ্য, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        ফেরত CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (অক্ষর *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    অন্যথা {
      যদি(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        ফলাফল = CURLE_OK;
        যাও out;
      }
      ফলাফল = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_ফলাফল ? backend->gtls.io_ফলাফল : CURLE_SEND_ERROR);
      যাও out;
    }
  }

out:
  CURL_TRC_CF(তথ্য, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
  ফেরত ফলাফল;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
স্থির CURLcode gtls_shutdown(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              bool send_shutdown, bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  অক্ষর buf[1024];
  CURLcode ফলাফল = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  যদি(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    যাও out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  যদি(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    যদি(send_shutdown) {
      পূর্ণসংখ্যা ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      যদি((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(তথ্য, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        ফলাফল = CURLE_OK;
        যাও out;
      }
      যদি(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(তথ্য, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((পূর্ণসংখ্যা)ret), (পূর্ণসংখ্যা)ret);
        ফলাফল = CURLE_RECV_ERROR;
        যাও out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  জন্য(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, আকার(buf));
    যদি(nread <= 0)
      বিরতি;
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
    CURL_TRC_CF(তথ্য, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((পূর্ণসংখ্যা)nread), (পূর্ণসংখ্যা)nread);
    ফলাফল = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (ফলাফল || *done);
  ফেরত ফলাফল;
}

স্থির শূন্য gtls_close(কাঠামো Curl_cfilter *cf,
                       কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(তথ্য, cf, "close");
  যদি(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = ফাঁকা;
  }
  যদি(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

স্থির CURLcode gtls_recv(কাঠামো Curl_cfilter *cf,
                          কাঠামো Curl_easy *তথ্য,
                          অক্ষর *buf, size_t blen,
                          size_t *pnread)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  যদি(nread >= 0)
    *pnread = (size_t)nread;
  অন্যথা {
    যদি((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      ফলাফল = CURLE_AGAIN;
      যাও out;
    }
    অন্যথা যদি(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      ফলাফল = cf_gtls_handshake(cf, তথ্য);
      যদি(!ফলাফল)
        ফলাফল = CURLE_AGAIN; /* make us get called again. */
      যাও out;
    }
    অন্যথা {
      failf(তথ্য, "GnuTLS recv error (%d): %s",
            (পূর্ণসংখ্যা)nread, gnutls_strerror((পূর্ণসংখ্যা)nread));
      ফলাফল = backend->gtls.io_ফলাফল ?
        backend->gtls.io_ফলাফল : CURLE_RECV_ERROR;
      যাও out;
    }
  }

out:
  CURL_TRC_CF(তথ্য, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (পূর্ণসংখ্যা)ফলাফল, nread);
  ফেরত ফলাফল;
}

size_t Curl_gtls_version(অক্ষর *বাফার, size_t আকার_2)
{
  ফেরত curl_msnprintf(বাফার, আকার_2, "GnuTLS/%s", gnutls_check_version(ফাঁকা));
}

/* data might be NULL! */
স্থির CURLcode gtls_random(কাঠামো Curl_easy *তথ্য,
                            unsigned অক্ষর *entropy, size_t দৈর্ঘ্য)
{
  পূর্ণসংখ্যা rc;
  (শূন্য)তথ্য;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, দৈর্ঘ্য);
  ফেরত rc ? CURLE_FAILED_INIT : CURLE_OK;
}

স্থির CURLcode gtls_sha256sum(ধ্রুবক unsigned অক্ষর *প্রবেশ,
                               size_t len,
                               unsigned অক্ষর *sha256sum, /* output */
                               size_t sha256len)
{
  কাঠামো sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  করো {
    unsigned পূর্ণসংখ্যা ilen = (unsigned পূর্ণসংখ্যা)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, প্রবেশ);
    len -= ilen;
    প্রবেশ += ilen;
  } যখন(len);
#যদি NETTLE_VERSION_MAJOR >= 4
  (শূন্য)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#অন্যথা
  sha256_digest(&SHA256pw, (unsigned পূর্ণসংখ্যা)sha256len, sha256sum);
#endif
  ফেরত CURLE_OK;
}

স্থির bool gtls_cert_status_request(শূন্য)
{
  ফেরত TRUE;
}

স্থির শূন্য *gtls_get_internals(কাঠামো ssl_connect_তথ্য *connssl,
                                CURLINFO info)
{
  কাঠামো gtls_ssl_backend_তথ্য *backend =
    (কাঠামো gtls_ssl_backend_তথ্য *)connssl->backend;
  (শূন্য)info;
  DEBUGASSERT(backend);
  ফেরত backend->gtls.session;
}

ধ্রুবক কাঠামো Curl_ssl Curl_ssl_gnutls = {
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

  আকার(কাঠামো gtls_ssl_backend_তথ্য),

  gtls_init,                     /* init */
  gtls_cleanup,                  /* cleanup */
  Curl_gtls_version,             /* version */
  gtls_shutdown,                 /* shutdown */
  gtls_তথ্য_pending,             /* data_pending */
  gtls_random,                   /* random */
  gtls_cert_status_request,      /* cert_status_request */
  gtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,       /* adjust_pollset */
  gtls_get_internals,            /* get_internals */
  gtls_close,                    /* close_one */
  ফাঁকা,                          /* close_all */
  ফাঁকা,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  ফাঁকা,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
