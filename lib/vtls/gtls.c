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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_GNUTLS

#ଅନ୍ତର୍ଭାଵ <gnutls/abstract.h>
#ଅନ୍ତର୍ଭାଵ <gnutls/gnutls.h>
#ଅନ୍ତର୍ଭାଵ <gnutls/x509.h>
#ଅନ୍ତର୍ଭାଵ <gnutls/crypto.h>
#ଅନ୍ତର୍ଭାଵ <nettle/sha2.h>
#ଅନ୍ତର୍ଭାଵ <nettle/version.h>

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "vtls/keylog.h"
#ଅନ୍ତର୍ଭାଵ "vtls/gtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_int.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "vtls/apple.h"
#ଅନ୍ତର୍ଭାଵ "vauth/vauth.h"
#ଅନ୍ତର୍ଭାଵ "parsedate.h"
#ଅନ୍ତର୍ଭାଵ "connect.h" /* for the connect timeout */
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"
#ଅନ୍ତର୍ଭାଵ "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#ଯଦି 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
ସ୍ଥିର ଶୂନ୍ୟ tls_log_func(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅଚଳ ଅକ୍ଷର *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#ଯଦି !defined(GNUTLS_VERSION_ସଂଖ୍ୟା) || (GNUTLS_VERSION_ସଂଖ୍ୟା < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#ଯଦି GNUTLS_VERSION_ସଂଖ୍ୟା >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#ଅନ୍ତର୍ଭାଵ <gnutls/ocsp.h>

ସଂରଚନା gtls_ssl_backend_data {
  ସଂରଚନା gtls_ctx gtls;
};

ସ୍ଥିର ssize_t gtls_ଠେଲ(ଶୂନ୍ୟ *s, ଅଚଳ ଶୂନ୍ୟ *buf, size_t blen)
{
  ସଂରଚନା Curl_cfilter *cf = s;
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nwritten);
  backend->gtls.io_result = result;
  ଯଦି(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ଫେରନ୍ତୁ -1;
  }
  ଫେରନ୍ତୁ (ssize_t)nwritten;
}

ସ୍ଥିର ssize_t gtls_pull(ଶୂନ୍ୟ *s, ଶୂନ୍ୟ *buf, size_t blen)
{
  ସଂରଚନା Curl_cfilter *cf = s;
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  ଯଦି(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    ଯଦି(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      ଫେରନ୍ତୁ -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
              nread);
  backend->gtls.io_result = result;
  ଯଦି(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ଫେରନ୍ତୁ -1;
  }
  ଅନ୍ୟଥା ଯଦି(nread == 0)
    connssl->peer_closed = TRUE;
  ଫେରନ୍ତୁ (ssize_t)nread;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gtls_init(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ଶୂନ୍ୟ gtls_cleanup(ଶୂନ୍ୟ)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
ସ୍ଥିର ଶୂନ୍ୟ showtime(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *text, time_t stamp)
{
  ସଂରଚନା tm buffer;
  ଅଚଳ ସଂରଚନା tm *tm = &buffer;
  ଅକ୍ଷର str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  ଯଦି(result)
    ଫେରନ୍ତୁ;

  curl_msnprintf(str,
                 ଆକାର(str),
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

ସ୍ଥିର gnutls_datum_t load_file(ଅଚଳ ଅକ୍ଷର *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { ଖାଲି, 0 };
  long filelen;
  ଶୂନ୍ୟ *ptr;

  f = curlx_fopen(file, "rb");
  ଯଦି(!f)
    ଫେରନ୍ତୁ loaded_file;
  ଯଦି(fseek(f, 0, SEEK_END))
    ଯାଅ out;
  filelen = ftell(f);
  ଯଦି(filelen < 0 || filelen > CURL_MAX_ନିବେଶ_LENGTH)
    ଯାଅ out;
  ଯଦି(fseek(f, 0, SEEK_SET))
    ଯାଅ out;
  ptr = curlx_malloc((size_t)filelen);
  ଯଦି(!ptr)
    ଯାଅ out;
  ଯଦି(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    ଯାଅ out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)filelen;
out:
  curlx_fclose(f);
  ଫେରନ୍ତୁ loaded_file;
}

ସ୍ଥିର ଶୂନ୍ୟ unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
ସ୍ଥିର CURLcode cf_gtls_handshake(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  ଯଦି(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    ଫେରନ୍ତୁ CURLE_AGAIN;
  }
  ଅନ୍ୟଥା ଯଦି((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    ଅଚଳ ଅକ୍ଷର *strerr = ଖାଲି;

    ଯଦି(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ଯଦି(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    ଫେରନ୍ତୁ CURLE_AGAIN;
  }
  ଅନ୍ୟଥା ଯଦି((rc < 0) && backend->gtls.io_result) {
    ଫେରନ୍ତୁ backend->gtls.io_result;
  }
  ଅନ୍ୟଥା ଯଦି(rc < 0) {
    ଅଚଳ ଅକ୍ଷର *strerr = ଖାଲି;

    ଯଦି(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ଯଦି(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର gnutls_x509_crt_fmt_t gnutls_do_file_type(ଅଚଳ ଅକ୍ଷର *type)
{
  ଯଦି(!type || !type[0])
    ଫେରନ୍ତୁ GNUTLS_X509_FMT_PEM;
  ଯଦି(curl_strequal(type, "PEM"))
    ଫେରନ୍ତୁ GNUTLS_X509_FMT_PEM;
  ଯଦି(curl_strequal(type, "DER"))
    ଫେରନ୍ତୁ GNUTLS_X509_FMT_DER;
  ଫେରନ୍ତୁ GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

ସ୍ଥିର CURLcode gnutls_set_ssl_version_min_max(
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା ssl_peer *peer,
  ସଂରଚନା ssl_filter_config *conn_config,
  ଅଚଳ ଅକ୍ଷର **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  ଯଦି(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  ଯଦି((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  ଯଦି(peer->transport == TRNSPRT_QUIC) {
    ଯଦି(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ବାଛ(ssl_version | ssl_version_max) {
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    ଫେରନ୍ତୁ CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(ସଂରଚନା Curl_easy *data,
                                       ସଂରଚନା gtls_shared_creds **pcreds)
{
  ସଂରଚନା gtls_shared_creds *shared;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  *pcreds = ଖାଲି;
  shared = curlx_calloc(1, ଆକାର(*shared));
  ଯଦି(!shared)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  ଯଦି(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(ସଂରଚନା gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  ଯଦି(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
}

ଶୂନ୍ୟ Curl_gtls_shared_creds_free(ସଂରଚନା gtls_shared_creds **pcreds)
{
  ସଂରଚନା gtls_shared_creds *shared = *pcreds;
  *pcreds = ଖାଲି;
  ଯଦି(shared) {
    --shared->refcount;
    ଯଦି(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

ସ୍ଥିର CURLcode gtls_populate_creds(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  ସଂରଚନା ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    ଫେରନ୍ତୁ CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  ଯଦି(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#ଅନ୍ୟଥା
    rc = gnutls_certificate_set_x509_system_trust(creds);
    ଯଦି(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    ଅନ୍ୟଥା {
      infof(data, "  Native: %d certificates from system trust", rc);
      ଯଦି(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  ଯଦି(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    ଯଦି(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    ଯଦି(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      ଯଦି(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ଅନ୍ୟଥା
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  ଅନ୍ୟଥା ଯଦି(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    ଯଦି(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      ଯଦି(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ଅନ୍ୟଥା
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  ଯଦି(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    ଯଦି(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      ଯଦି(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ଅନ୍ୟଥା
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  ଯଦି(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  ଯଦି(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  }
#endif
  ଯଦି(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    ଯଦି(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      ଫେରନ୍ତୁ CURLE_SSL_CRL_BADFILE;
    }
    ଅନ୍ୟଥା
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_ଚାବି   "tls:gtls:x509:share"

ସ୍ଥିର bool gtls_shared_creds_expired(ସଂରଚନା Curl_easy *data,
                                      ଅଚଳ ସଂରଚନା gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  ଯଦି(timeout_ms < 0)
    ଫେରନ୍ତୁ FALSE;

  ଫେରନ୍ତୁ elapsed_ms >= timeout_ms;
}

ସ୍ଥିର bool gtls_shared_creds_different(ସଂରଚନା Curl_cfilter *cf,
                                        ଅଚଳ ସଂରଚନା gtls_shared_creds *sc)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ଯଦି(!sc->CAfile || !conn_config->CAfile)
    ଫେରନ୍ତୁ sc->CAfile != conn_config->CAfile;

  ଫେରନ୍ତୁ strcmp(sc->CAfile, conn_config->CAfile);
}

ସ୍ଥିର ସଂରଚନା gtls_shared_creds *gtls_get_cached_creds(ସଂରଚନା Curl_cfilter *cf,
                                                       ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା gtls_shared_creds *shared_creds;

  ଯଦି(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_ଚାବି,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_ଚାବି));
    ଯଦି(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      ଫେରନ୍ତୁ shared_creds;
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଶୂନ୍ୟ gtls_shared_creds_hash_free(ଅଚଳ ଶୂନ୍ୟ *ଚାବି,
                                        size_t ଚାବି_len, ଶୂନ୍ୟ *p)
{
  ସଂରଚନା gtls_shared_creds *sc = p;
  DEBUGASSERT(ଚାବି_len == CURL_CSTRLEN(MPROTO_GTLS_X509_ଚାବି));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_ଚାବି, ଚାବି, ଚାବି_len));
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)ଚାବି_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

ସ୍ଥିର ଶୂନ୍ୟ gtls_set_cached_creds(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା gtls_shared_creds *sc)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  ଯଦି(!data->multi)
    ଫେରନ୍ତୁ;

  ଯଦି(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    ଯଦି(!sc->CAfile)
      ଫେରନ୍ତୁ;
  }

  ଯଦି(Curl_gtls_shared_creds_up_ref(sc))
    ଫେରନ୍ତୁ;

  ଯଦି(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_ଚାବି,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_ଚାବି),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    ଫେରନ୍ତୁ;
  }
}

CURLcode Curl_gtls_client_trust_setup(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା gtls_ctx *gtls)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା gtls_shared_creds *cached_creds = ଖାଲି;
  bool cache_criteria_met;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

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

  ଯଦି(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  ଯଦି(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    ଯଦି(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
  }
  ଅନ୍ୟଥା {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    gtls->shared_creds->trust_setup = TRUE;
    ଯଦି(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gtls_get_ietf_proto(gnutls_session_t session)
{
  ବାଛ(gnutls_protocol_get_version(session)) {
  ସ୍ଥିତି GNUTLS_SSL3:
    ଫେରନ୍ତୁ CURL_IETF_PROTO_SSL3;
  ସ୍ଥିତି GNUTLS_TLS1_0:
    ଫେରନ୍ତୁ CURL_IETF_PROTO_TLS1;
  ସ୍ଥିତି GNUTLS_TLS1_1:
    ଫେରନ୍ତୁ CURL_IETF_PROTO_TLS1_1;
  ସ୍ଥିତି GNUTLS_TLS1_2:
    ଫେରନ୍ତୁ CURL_IETF_PROTO_TLS1_2;
  ସ୍ଥିତି GNUTLS_TLS1_3:
    ଫେରନ୍ତୁ CURL_IETF_PROTO_TLS1_3;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 ଅଚଳ ଅକ୍ଷର *alpn,
                                 unsigned ଅକ୍ଷର *quic_tp,
                                 size_t quic_tp_len,
                                 ସଂରଚନା Curl_ssl_session **psession)
{
  ସଂରଚନା Curl_ssl_session *sc_session = ଖାଲି, *sc_dup = ଖାଲି;
  unsigned ଅକ୍ଷର *sdata, *qtp_clone = ଖାଲି;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  ଯଦି(psession)
    *psession = ଖାଲି;
  ଯଦି(!Curl_ssl_scache_use(cf, data))
    ଫେରନ୍ତୁ CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, ଖାଲି, &sdata_len);
  ଯଦି(!sdata_len) /* GnuTLS does this for some version combinations */
    ଫେରନ୍ତୁ CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  ଯଦି(!sdata)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  ଯଦି(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((ଅଚଳ ଅକ୍ଷର *)quic_tp, quic_tp_len);
    ଯଦି(!qtp_clone) {
      curlx_free(sdata);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  ଯଦି(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  ଯଦି(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_ଚାବି, sc_session);
    /* took ownership of `sc_session` */
    sc_session = ଖାଲି;
  }
  ଯଦି(!result && psession) {
    *psession = sc_dup;
    sc_dup = ଖାଲି;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ଫେରନ୍ତୁ result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
ସ୍ଥିର CURLcode cf_gtls_update_session_id(ସଂରଚନା Curl_cfilter *cf,
                                          ସଂରଚନା Curl_easy *data,
                                          gnutls_session_t session)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା Curl_ssl_session *scs = ଖାଲି;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_ଚାବି,
                                   session, 0, connssl->negotiated.alpn,
                                   ଖାଲି, 0, &scs);
  ଯଦି(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gtls_handshake_cb(gnutls_session_t session, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା htype,
                             unsigned when, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା incoming,
                             ଅଚଳ gnutls_datum_t *ସନ୍ଦେଶ)
{
  ସଂରଚନା Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (ଶୂନ୍ୟ)ସନ୍ଦେଶ;
  (ଶୂନ୍ୟ)incoming;
  ଯଦି(when) { /* after message has been processed */
    ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
    ଯଦି(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      ବାଛ(htype) {
      ସ୍ଥିତି GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        ଭାଙ୍ଗ;
      }
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
      }
    }
  }
  ଫେରନ୍ତୁ 0;
}
#endif

ସ୍ଥିର CURLcode gtls_set_priority(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା gtls_ctx *gtls,
                                  ଅଚଳ ଅକ୍ଷର *priority)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା dynbuf buf;
  ଅଚଳ ଅକ୍ଷର *err = ଖାଲି;
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  curlx_dyn_init(&buf, 4096);

  ଯଦି(conn_config->cipher_list) {
    ଯଦି((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      ଯଦି(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        ଯଦି(result)
          ଯାଅ out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      ଯଦି(result)
        ଯାଅ out;
      priority = curlx_dyn_ptr(&buf);
    }
    ଅନ୍ୟଥା /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  ଯଦି(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode gtls_client_init(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ssl_peer *peer,
                                 size_t earlydata_max,
                                 ସଂରଚନା gtls_ctx *gtls)
{
  ସଂରଚନା ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_flags;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ଅଚଳ ଅକ୍ଷର *prioritylist;
  bool tls13support;
  CURLcode result;

  ଯଦି(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  ଯଦି(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  ଅନ୍ୟଥା ଯଦି(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  ଯଦି(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  ଯଦି(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  ଯଦି(peer->sni) {
    ଯଦି(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  ଯଦି(rc != GNUTLS_E_SUCCESS)
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  ଯଦି(config->version == CURL_SSLVERSION_TLSv1_3) {
    ଯଦି(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(config->clientcert) {
    ଯଦି(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ଯଦି(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->ଚାବି_passwd ? config->ଚାବି_passwd : "");
      ଯଦି(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
      }
    }
    ଅନ୍ୟଥା {
      ଅଚଳ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା supported_ଚାବି_encryption_algorithms =
        GNUTLS_PKCS_USE_PKCS12_3DES | GNUTLS_PKCS_USE_PKCS12_ARCFOUR |
        GNUTLS_PKCS_USE_PKCS12_RC2_40 | GNUTLS_PKCS_USE_PBES2_3DES |
        GNUTLS_PKCS_USE_PBES2_AES_128 | GNUTLS_PKCS_USE_PBES2_AES_192 |
        GNUTLS_PKCS_USE_PBES2_AES_256;
      rc = gnutls_certificate_set_x509_ଚାବି_file2(
           gtls->shared_creds->creds,
           config->clientcert,
           config->ଚାବି ? config->ଚାବି : config->clientcert,
           gnutls_do_file_type(config->cert_type),
           config->ଚାବି_passwd,
           supported_ଚାବି_encryption_algorithms);
      ଯଦି(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->ଚାବି_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  ଯଦି(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  ଯଦି(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  ଖାଲି, 0, ଖାଲି);
    ଯଦି(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା keylog_callback(gnutls_session_t session, ଅଚଳ ଅକ୍ଷର *label,
                           ଅଚଳ gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  ଯଦି(crandom.size != 32) {
    ଫେରନ୍ତୁ -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର CURLcode gtls_on_session_reuse(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା alpn_spec *alpns,
                                      ସଂରଚନା Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  ଫେରନ୍ତୁ Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

ସ୍ଥିର CURLcode gtls_apply_session(
  ସଂରଚନା gtls_ctx *gctx,
  ସଂରଚନା Curl_cfilter *cf,
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା ssl_peer *peer,
  ସଂରଚନା alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  ସଂରଚନା Curl_ssl_session *scs,
  bool *pwas_setup)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  ଯଦି(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    ଯଦି(result)
      ଯାଅ out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    ଯଦି(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    ଅନ୍ୟଥା {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      ଯଦି(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        ଯଦି(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          ଯଦି(result)
            ଯାଅ out;
        }
        ଯଦି(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_gtls_ctx_init(ସଂରଚନା gtls_ctx *gctx,
                            ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା ssl_peer *peer,
                            ଅଚଳ ସଂରଚନା alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            ଶୂନ୍ୟ *cb_user_data,
                            ଶୂନ୍ୟ *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା Curl_ssl_session *scs = ଖାଲି;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_ଗଣନା = 0;
  bool gtls_session_setup = FALSE;
  ସଂରଚନା alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  ଯଦି((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    ଯଦି(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = ଖାଲି;
      ଯଦି(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  ଯଦି(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_ଚାବି, &scs);
    ଯଦି(result)
      ଯାଅ out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  ଯଦି(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    ଯଦି(result)
      ଯାଅ out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  ଯଦି(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    ଯଦି(result)
      ଯାଅ out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  ଯଦି(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  ଯଦି(!gtls_alpns_ଗଣନା && alpns.ଗଣନା_2) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.ଗଣନା_2);
    ପାଇଁ(i = 0; i < alpns.ଗଣନା_2; ++i) {
      gtls_alpns[i].data = (unsigned ଅକ୍ଷର *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)strlen(alpns.entries[i]);
    }
    gtls_alpns_ଗଣନା = alpns.ଗଣନା_2;
  }

  ଯଦି(gtls_alpns_ଗଣନା &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)gtls_alpns_ଗଣନା,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_ଚାବି, scs);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode gtls_connect_step1(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  ଯଦି(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    ଫେରନ୍ତୁ CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, ଖାଲି, ଖାଲି, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#ଅନ୍ୟଥା
                              ଖାଲି
#endif
                              );

  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    ସଂରଚନା alpn_proto_buf proto;
    memset(&proto, 0, ଆକାର(proto));
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
  gnutls_transport_set_ଠେଲ_function(backend->gtls.session, gtls_ଠେଲ);
  gnutls_transport_set_pull_function(backend->gtls.session, gtls_pull);

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode pkp_pin_peer_pubkey(ସଂରଚନା Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    ଅଚଳ ଅକ୍ଷର *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned ଅକ୍ଷର *buff1 = ଖାଲି;

  gnutls_pubkey_t ଚାବି = ଖାଲି;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ଯଦି(!pinnedpubkey)
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(!cert)
    ଫେରନ୍ତୁ result;

  କର {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&ଚାବି);
    ଯଦି(ret < 0)
      ଭାଙ୍ଗ; /* failed */

    ret = gnutls_pubkey_import_x509(ଚାବି, cert, 0);
    ଯଦି(ret < 0)
      ଭାଙ୍ଗ; /* failed */

    ret = gnutls_pubkey_export(ଚାବି, GNUTLS_X509_FMT_DER, ଖାଲି, &len1);
    ଯଦି(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      ଭାଙ୍ଗ; /* failed */

    buff1 = curlx_malloc(len1);
    ଯଦି(!buff1)
      ଭାଙ୍ଗ; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(ଚାବି, GNUTLS_X509_FMT_DER, buff1, &len2);
    ଯଦି(ret < 0 || len1 != len2)
      ଭାଙ୍ଗ; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } ଯେତେବେଳେ(0);

  ଯଦି(ଚାବି)
    gnutls_pubkey_deinit(ଚାବି);

  curlx_safefree(buff1);

  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_gtls_report_handshake(ସଂରଚନା Curl_easy *data, ସଂରଚନା gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  ଯଦି(Curl_trc_is_verbose(data)) {
    ଅଚଳ ଅକ୍ଷର *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)gctx;
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ gtls_ସନ୍ଦେଶ_verify_result(ସଂରଚନା Curl_easy *data,
                                   ସଂରଚନା ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  ଅକ୍ଷର certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = ଆକାର(certname);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  ଯଦି(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  ଯଦି(!was_verified) {
    ଯଦି(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    ଅନ୍ୟଥା
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  ଅନ୍ୟଥା
    infof(data, "  common name: %s (matched)", certname);
}

ସ୍ଥିର ଶୂନ୍ୟ gtls_infof_cert(ସଂରଚନା Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  ଯଦି(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, algo;
    time_t tstamp;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    ଯଦି(rc)
      infof(data, "Failed to get certificate name");
    ଅନ୍ୟଥା {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    ଯଦି(rc)
      infof(data, "Failed to get certificate issuer");
    ଅନ୍ୟଥା {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)x509_cert;
#endif
}

ସ୍ଥିର CURLcode gtls_verify_ocsp_status(ସଂରଚନା Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = ଖାଲି;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  ଯଦି(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    ଯାଅ out;
  }
  ଅନ୍ୟଥା ଯଦି(rc < 0) {
    failf(data, "Invalid OCSP response received");
    ଯାଅ out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  ଯଦି(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    ଯାଅ out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  ଯଦି(rc < 0) {
    failf(data, "failed to parse OCSP response");
    ଯାଅ out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  ଯଦି(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    ଯାଅ out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  ପାଇଁ(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    ଯଦି(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      ଭାଙ୍ଗ;
  }
  ଯଦି(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    ଯାଅ out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି,
                                   &status, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି);
  ଯଦି(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    ଯାଅ out;
  }
  ବାଛ(status) {
  ସ୍ଥିତି GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "peer certificate OCSP status is not good: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)status);
    ଭାଙ୍ଗ;
  }

out:
  ଯଦି(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  ଫେରନ୍ତୁ result;
}

ସଂରଚନା gtls_cert_chain {
  ଅଚଳ gnutls_datum_t *certs;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_certs;
};

#ifdef USE_APPLE_SECTRUST
ସ୍ଥିର CURLcode gtls_chain_get_der(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ଶୂନ୍ୟ *user_data,
                                   size_t i,
                                   unsigned ଅକ୍ଷର **pder,
                                   size_t *pder_len)
{
  ସଂରଚନା gtls_cert_chain *chain = user_data;

  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)data;
  *pder_len = 0;
  *pder = ଖାଲି;

  ଯଦି(i >= chain->ଅଙ୍କ_certs)
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  ଫେରନ୍ତୁ CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
ସ୍ଥିର CURLcode gtls_verify_cert(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା ssl_filter_config *config,
                                 ସଂରଚନା ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା ssl_peer *peer,
                                 ସଂରଚନା gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_status = 0;
  long * ଅଚଳ certverifyresult = &ssl_config->certverifyresult;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  ଯଦି(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  ଯଦି(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  ଯଦି(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    ଯଦି(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = ଖାଲି;
      ocsp_raw.size = 0;
    }
    ଯଦି(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      ଫେରନ୍ତୁ CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->ଅଙ୍କ_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    ଯଦି(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      ଫେରନ୍ତୁ result; /* no further failure analysis needed */
    verified = !result;
    ଯଦି(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)cf;
  (ଶୂନ୍ୟ)peer;
  (ଶୂନ୍ୟ)chain;
#endif

  ଯଦି(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    ଅଚଳ ଅକ୍ଷର *cause = "certificate error, no details available";
    ଯଦି(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    ଅନ୍ୟଥା ଯଦି(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    ଅନ୍ୟଥା ଯଦି(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    ଅନ୍ୟଥା ଯଦି(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                gnutls_session_t session,
                                ସଂରଚନା ssl_filter_config *config,
                                ସଂରଚନା ssl_easy_config *ssl_config,
                                ସଂରଚନା ssl_peer *peer,
                                ଅଚଳ ଅକ୍ଷର *pinned_ଚାବି)
{
  ସଂରଚନା gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = ଖାଲି, x509_issuer = ଖାଲି;
  time_t certclock;
  bool ocsp_checked = FALSE;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  CURLcode result = CURLE_OK;
  long * ଅଚଳ certverifyresult = &ssl_config->certverifyresult;

  (ଶୂନ୍ୟ)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.ଅଙ୍କ_certs);
  ଯଦି(!chain.certs) {
    ଯଦି(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      ଯାଅ out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  ଯଦି(data->set.ssl.certinfo && chain.certs) {
    ଯଦି(chain.ଅଙ୍କ_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.ଅଙ୍କ_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା {
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

      result = Curl_ssl_init_certinfo(data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)chain.ଅଙ୍କ_certs);
      ଯଦି(result)
        ଯାଅ out;

      ପାଇଁ(i = 0; i < chain.ଅଙ୍କ_certs; i++) {
        ଅଚଳ uint8_t *beg = chain.certs[i].data;
        ଅଚଳ uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)i, beg, end);
        ଯଦି(result)
          ଯାଅ out;
      }
    }
  }

  ଯଦି(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଅନ୍ୟଥା
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  ଯଦି(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    ଯାଅ out;
  }

  ଯଦି(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    ଯଦି(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  ଯଦି(certclock == (time_t)-1) {
    ଯଦି(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  ଅନ୍ୟଥା {
    ଯଦି(certclock < time(ଖାଲି)) {
      ଯଦି(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        ଯାଅ out;
      }
      ଅନ୍ୟଥା
        infof(data, "  SSL certificate expiration date FAILED");
    }
    ଅନ୍ୟଥା
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  ଯଦି(certclock == (time_t)-1) {
    ଯଦି(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  ଅନ୍ୟଥା {
    ଯଦି(certclock > time(ଖାଲି)) {
      ଯଦି(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        ଯାଅ out;
      }
      ଅନ୍ୟଥା
        infof(data, "  SSL certificate activation date FAILED");
    }
    ଅନ୍ୟଥା
      infof(data, "  SSL certificate activation date OK");
  }

  ଯଦି(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଅନ୍ୟଥା
    infof(data, "  SSL certificate status verification SKIPPED");

  ଯଦି(config->issuercert) {
    gnutls_datum_t issuerp;
    ଯଦି(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      ଯାଅ out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    ଯଦି(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ଯାଅ out;
    }
    rc = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    ଯଦି(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      ଯାଅ out;
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
  rc = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_ସନ୍ଦେଶ_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(pinned_ଚାବି) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_ଚାବି);
    ଯଦି(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      ଯାଅ out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  ଯଦି(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  ଯଦି(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode gtls_verifyserver(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  gnutls_session_t session)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  ଅଚଳ ଅକ୍ଷର *pinned_ଚାବି = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#ଅନ୍ୟଥା
  ଅଚଳ ଅକ୍ଷର *pinned_ଚାବି = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_ଚାବି);
  ଯଦି(result)
    ଯାଅ out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  ଯଦି(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  ଫେରନ୍ତୁ result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
ସ୍ଥିର CURLcode gtls_send_earlydata(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ଅଚଳ unsigned ଅକ୍ଷର *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  ଯେତେବେଳେ(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    ଯଦି(n < 0) {
      ଯଦି(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      ଅନ୍ୟଥା
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ଫେରନ୍ତୁ result;
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
ସ୍ଥିର CURLcode gtls_connect_common(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    bool *done)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  ଯଦି(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  ଯଦି(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    ଯଦି(result)
      ଯାଅ out;
    connssl->connecting_state = ssl_connect_2;
  }

  ଯଦି(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    ଯଦି(connssl->earlydata_state == ssl_earlydata_await) {
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      ଯଦି(result)
        ଯାଅ out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    ଯଦି(result)
      ଯାଅ out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  ଯଦି(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    ଯଦି(result)
      ଯାଅ out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    ଯଦି(rc) {  /* No ALPN from server */
      proto.data = ଖାଲି;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    ଯଦି(result)
      ଯାଅ out;

#ifdef CURL_GNUTLS_EARLY_DATA
    ଯଦି(connssl->earlydata_state > ssl_earlydata_none) {
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

  ଯଦି(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  ଯଦି(result == CURLE_AGAIN) {
    *done = FALSE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result,
              *done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode gtls_connect(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ଯଦି((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
#endif
  ଫେରନ୍ତୁ gtls_connect_common(cf, data, done);
}

ସ୍ଥିର bool gtls_data_pending(ସଂରଚନା Curl_cfilter *cf,
                              ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *ctx = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ସଂରଚନା gtls_ssl_backend_data *)ctx->backend;
  ଯଦି(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLcode gtls_send(ସଂରଚନା Curl_cfilter *cf,
                          ସଂରଚନା Curl_easy *data,
                          ଅଚଳ ଶୂନ୍ୟ *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  ଯେତେବେଳେ(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    ଯଦି(nwritten >= 0) {
      ଯଦି((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        ଫେରନ୍ତୁ CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (ଅକ୍ଷର *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    ଅନ୍ୟଥା {
      ଯଦି(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        ଯାଅ out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      ଯାଅ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
  ଫେରନ୍ତୁ result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
ସ୍ଥିର CURLcode gtls_shutdown(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  ଅକ୍ଷର buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  ଯଦି(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    ଯାଅ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  ଯଦି(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    ଯଦି(send_shutdown) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      ଯଦି((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        ଯାଅ out;
      }
      ଯଦି(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ret), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ret);
        result = CURLE_RECV_ERROR;
        ଯାଅ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ପାଇଁ(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, ଆକାର(buf));
    ଯଦି(nread <= 0)
      ଭାଙ୍ଗ;
  }
  ଯଦି(nread > 0) {
    /* still data coming in? */
  }
  ଅନ୍ୟଥା ଯଦି(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  ଅନ୍ୟଥା ଯଦି((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  ଅନ୍ୟଥା {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nread), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ gtls_close(ସଂରଚନା Curl_cfilter *cf,
                       ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  ଯଦି(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = ଖାଲି;
  }
  ଯଦି(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

ସ୍ଥିର CURLcode gtls_recv(ସଂରଚନା Curl_cfilter *cf,
                          ସଂରଚନା Curl_easy *data,
                          ଅକ୍ଷର *buf, size_t blen,
                          size_t *pnread)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  ଯଦି(nread >= 0)
    *pnread = (size_t)nread;
  ଅନ୍ୟଥା {
    ଯଦି((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      ଯଦି(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      ଯାଅ out;
    }
    ଅନ୍ୟଥା {
      failf(data, "GnuTLS recv error (%d): %s",
            (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nread, gnutls_strerror((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      ଯାଅ out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nread);
  ଫେରନ୍ତୁ result;
}

size_t Curl_gtls_version(ଅକ୍ଷର *buffer, size_t size)
{
  ଫେରନ୍ତୁ curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(ଖାଲି));
}

/* data might be NULL! */
ସ୍ଥିର CURLcode gtls_random(ସଂରଚନା Curl_easy *data,
                            unsigned ଅକ୍ଷର *entropy, size_t length)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  (ଶୂନ୍ୟ)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  ଫେରନ୍ତୁ rc ? CURLE_FAILED_INIT : CURLE_OK;
}

ସ୍ଥିର CURLcode gtls_sha256sum(ଅଚଳ unsigned ଅକ୍ଷର *ନିବେଶ,
                               size_t len,
                               unsigned ଅକ୍ଷର *sha256sum, /* output */
                               size_t sha256len)
{
  ସଂରଚନା sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  କର {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ilen = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, ନିବେଶ);
    len -= ilen;
    ନିବେଶ += ilen;
  } ଯେତେବେଳେ(len);
#ଯଦି NETTLE_VERSION_MAJOR >= 4
  (ଶୂନ୍ୟ)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#ଅନ୍ୟଥା
  sha256_digest(&SHA256pw, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)sha256len, sha256sum);
#endif
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool gtls_cert_status_request(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର ଶୂନ୍ୟ *gtls_get_internals(ସଂରଚନା ssl_connect_data *connssl,
                                CURLINFO info)
{
  ସଂରଚନା gtls_ssl_backend_data *backend =
    (ସଂରଚନା gtls_ssl_backend_data *)connssl->backend;
  (ଶୂନ୍ୟ)info;
  DEBUGASSERT(backend);
  ଫେରନ୍ତୁ backend->gtls.session;
}

ଅଚଳ ସଂରଚନା Curl_ssl Curl_ssl_gnutls = {
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

  ଆକାର(ସଂରଚନା gtls_ssl_backend_data),

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
  ଖାଲି,                          /* close_all */
  ଖାଲି,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  ଖାଲି,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
