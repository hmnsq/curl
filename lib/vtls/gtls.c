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
#include "curl_setup.h"

#ifdef USE_GNUTLS

#include <gnutls/abstract.h>
#include <gnutls/gnutls.h>
#include <gnutls/x509.h>
#include <gnutls/crypto.h>
#include <nettle/sha2.h>
#include <nettle/version.h>

#include "urldata.h"
#include "curl_trc.h"
#include "vtls/keylog.h"
#include "vtls/gtls.h"
#include "vtls/vtls.h"
#include "vtls/vtls_int.h"
#include "vtls/vtls_scache.h"
#include "vtls/apple.h"
#include "vauth/vauth.h"
#include "parsedate.h"
#include "connect.h" /* for the connect timeout */
#include "progress.h"
#include "curlx/strdup.h"
#include "curlx/win32-fopen.h"
#include "vtls/x509asn1.h"

/* Enable GnuTLS debugging by defining GTLSDEBUG */
#ပေႃး 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
static void tls_log_func(puur_naa_nka level, const var_na *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#ပေႃး !defined(GNUTLS_VERSION_NUMBER) || (GNUTLS_VERSION_NUMBER < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#ပေႃး GNUTLS_VERSION_NUMBER >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#include <gnutls/ocsp.h>

sa_nracanaa gtls_ssl_backend_data {
  sa_nracanaa gtls_ctx gtls;
};

static ssize_t gtls_push(void *s, const void *buf, size_t blen)
{
  sa_nracanaa Curl_cfilter *cf = s;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (puur_naa_nka)result, nwritten);
  backend->gtls.io_result = result;
  ပေႃး(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ပွၵ်ႈ -1;
  }
  ပွၵ်ႈ (ssize_t)nwritten;
}

static ssize_t gtls_pull(void *s, void *buf, size_t blen)
{
  sa_nracanaa Curl_cfilter *cf = s;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  ပေႃး(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    ပေႃး(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      ပွၵ်ႈ -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (puur_naa_nka)result,
              nread);
  backend->gtls.io_result = result;
  ပေႃး(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    ပွၵ်ႈ -1;
  }
  ပေႃးဢမ်ႇ ပေႃး(nread == 0)
    connssl->peer_closed = TRUE;
  ပွၵ်ႈ (ssize_t)nread;
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
static puur_naa_nka gtls_init(void)
{
  puur_naa_nka ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  ပွၵ်ႈ ret;
}

static void gtls_cleanup(void)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
static void showtime(sa_nracanaa Curl_easy *data, const var_na *text, time_t stamp)
{
  sa_nracanaa tm buffer;
  const sa_nracanaa tm *tm = &buffer;
  var_na str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  ပေႃး(result)
    ပွၵ်ႈ;

  curl_msnprintf(str,
                 parimaa_na(str),
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

static gnutls_datum_t load_file(const var_na *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { NULL, 0 };
  long filelen;
  void *ptr;

  f = curlx_fopen(file, "rb");
  ပေႃး(!f)
    ပွၵ်ႈ loaded_file;
  ပေႃး(fseek(f, 0, SEEK_END))
    goto out;
  filelen = ftell(f);
  ပေႃး(filelen < 0 || filelen > CURL_MAX_INPUT_LENGTH)
    goto out;
  ပေႃး(fseek(f, 0, SEEK_SET))
    goto out;
  ptr = curlx_malloc((size_t)filelen);
  ပေႃး(!ptr)
    goto out;
  ပေႃး(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    goto out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned puur_naa_nka)filelen;
out:
  curlx_fclose(f);
  ပွၵ်ႈ loaded_file;
}

static void unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
static CURLcode cf_gtls_handshake(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  puur_naa_nka rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  ပေႃး(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    ပေႃး(result)
      ပွၵ်ႈ result;
  }

  ပေႃး((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    ပွၵ်ႈ CURLE_AGAIN;
  }
  ပေႃးဢမ်ႇ ပေႃး((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    const var_na *strerr = NULL;

    ပေႃး(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ပေႃး(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    ပွၵ်ႈ CURLE_AGAIN;
  }
  ပေႃးဢမ်ႇ ပေႃး((rc < 0) && backend->gtls.io_result) {
    ပွၵ်ႈ backend->gtls.io_result;
  }
  ပေႃးဢမ်ႇ ပေႃး(rc < 0) {
    const var_na *strerr = NULL;

    ပေႃး(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    ပေႃး(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
  }

  ပွၵ်ႈ CURLE_OK;
}

static gnutls_x509_crt_fmt_t gnutls_do_file_type(const var_na *type)
{
  ပေႃး(!type || !type[0])
    ပွၵ်ႈ GNUTLS_X509_FMT_PEM;
  ပေႃး(curl_strequal(type, "PEM"))
    ပွၵ်ႈ GNUTLS_X509_FMT_PEM;
  ပေႃး(curl_strequal(type, "DER"))
    ပွၵ်ႈ GNUTLS_X509_FMT_DER;
  ပွၵ်ႈ GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

static CURLcode gnutls_set_ssl_version_min_max(
  sa_nracanaa Curl_easy *data,
  sa_nracanaa ssl_peer *peer,
  sa_nracanaa ssl_filter_config *conn_config,
  const var_na **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  ပေႃး(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  ပေႃး((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  ပေႃး(peer->transport == TRNSPRT_QUIC) {
    ပေႃး(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    ပွၵ်ႈ CURLE_OK;
  }

  switch(ssl_version | ssl_version_max) {
  pak_sa CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    ပွၵ်ႈ CURLE_OK;
  pak_sa CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    ပွၵ်ႈ CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(sa_nracanaa Curl_easy *data,
                                       sa_nracanaa gtls_shared_creds **pcreds)
{
  sa_nracanaa gtls_shared_creds *shared;
  puur_naa_nka rc;

  *pcreds = NULL;
  shared = curlx_calloc(1, parimaa_na(*shared));
  ပေႃး(!shared)
    ပွၵ်ႈ CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  ပေႃး(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  ပွၵ်ႈ CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(sa_nracanaa gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  ပေႃး(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    ပွၵ်ႈ CURLE_OK;
  }
  ပွၵ်ႈ CURLE_BAD_FUNCTION_ARGUMENT;
}

void Curl_gtls_shared_creds_free(sa_nracanaa gtls_shared_creds **pcreds)
{
  sa_nracanaa gtls_shared_creds *shared = *pcreds;
  *pcreds = NULL;
  ပေႃး(shared) {
    --shared->refcount;
    ပေႃး(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

static CURLcode gtls_populate_creds(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  sa_nracanaa ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  puur_naa_nka rc;

  ပေႃး(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    ပွၵ်ႈ CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  ပေႃး(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#ပေႃးဢမ်ႇ
    rc = gnutls_certificate_set_x509_system_trust(creds);
    ပေႃး(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    ပေႃးဢမ်ႇ {
      infof(data, "  Native: %d certificates from system trust", rc);
      ပေႃး(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  ပေႃး(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    ပေႃး(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      ပွၵ်ႈ CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned puur_naa_nka)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    ပေႃး(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      ပေႃး(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ပွၵ်ႈ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ပေႃးဢမ်ႇ
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  ပေႃးဢမ်ႇ ပေႃး(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    ပေႃး(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      ပေႃး(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ပွၵ်ႈ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ပေႃးဢမ်ႇ
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  ပေႃး(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    ပေႃး(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      ပေႃး(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        ပွၵ်ႈ CURLE_SSL_CACERT_BADFILE;
      }
    }
    ပေႃးဢမ်ႇ
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  ပေႃး(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  ပေႃး(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    ပွၵ်ႈ CURLE_NOT_BUILT_IN;
  }
#endif
  ပေႃး(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    ပေႃး(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      ပွၵ်ႈ CURLE_SSL_CRL_BADFILE;
    }
    ပေႃးဢမ်ႇ
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  ပွၵ်ႈ CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_KEY   "tls:gtls:x509:share"

static bool gtls_shared_creds_expired(sa_nracanaa Curl_easy *data,
                                      const sa_nracanaa gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  ပေႃး(timeout_ms < 0)
    ပွၵ်ႈ FALSE;

  ပွၵ်ႈ elapsed_ms >= timeout_ms;
}

static bool gtls_shared_creds_different(sa_nracanaa Curl_cfilter *cf,
                                        const sa_nracanaa gtls_shared_creds *sc)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ပေႃး(!sc->CAfile || !conn_config->CAfile)
    ပွၵ်ႈ sc->CAfile != conn_config->CAfile;

  ပွၵ်ႈ strcmp(sc->CAfile, conn_config->CAfile);
}

static sa_nracanaa gtls_shared_creds *gtls_get_cached_creds(sa_nracanaa Curl_cfilter *cf,
                                                       sa_nracanaa Curl_easy *data)
{
  sa_nracanaa gtls_shared_creds *shared_creds;

  ပေႃး(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_KEY,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_KEY));
    ပေႃး(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      ပွၵ်ႈ shared_creds;
    }
  }
  ပွၵ်ႈ NULL;
}

static void gtls_shared_creds_hash_free(const void *key,
                                        size_t key_len, void *p)
{
  sa_nracanaa gtls_shared_creds *sc = p;
  DEBUGASSERT(key_len == CURL_CSTRLEN(MPROTO_GTLS_X509_KEY));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_KEY, key, key_len));
  (void)key;
  (void)key_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

static void gtls_set_cached_creds(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa gtls_shared_creds *sc)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  ပေႃး(!data->multi)
    ပွၵ်ႈ;

  ပေႃး(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    ပေႃး(!sc->CAfile)
      ပွၵ်ႈ;
  }

  ပေႃး(Curl_gtls_shared_creds_up_ref(sc))
    ပွၵ်ႈ;

  ပေႃး(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_KEY,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_KEY),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    ပွၵ်ႈ;
  }
}

CURLcode Curl_gtls_client_trust_setup(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      sa_nracanaa gtls_ctx *gtls)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa gtls_shared_creds *cached_creds = NULL;
  bool cache_criteria_met;
  CURLcode result;
  puur_naa_nka rc;

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

  ပေႃး(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  ပေႃး(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    ပေႃး(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
    }
  }
  ပေႃးဢမ်ႇ {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    ပေႃး(result)
      ပွၵ်ႈ result;
    gtls->shared_creds->trust_setup = TRUE;
    ပေႃး(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  ပွၵ်ႈ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
static puur_naa_nka gtls_get_ietf_proto(gnutls_session_t session)
{
  switch(gnutls_protocol_get_version(session)) {
  pak_sa GNUTLS_SSL3:
    ပွၵ်ႈ CURL_IETF_PROTO_SSL3;
  pak_sa GNUTLS_TLS1_0:
    ပွၵ်ႈ CURL_IETF_PROTO_TLS1;
  pak_sa GNUTLS_TLS1_1:
    ပွၵ်ႈ CURL_IETF_PROTO_TLS1_1;
  pak_sa GNUTLS_TLS1_2:
    ပွၵ်ႈ CURL_IETF_PROTO_TLS1_2;
  pak_sa GNUTLS_TLS1_3:
    ပွၵ်ႈ CURL_IETF_PROTO_TLS1_3;
  default:
    ပွၵ်ႈ CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 const var_na *ssl_peer_key,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 const var_na *alpn,
                                 unsigned var_na *quic_tp,
                                 size_t quic_tp_len,
                                 sa_nracanaa Curl_ssl_session **psession)
{
  sa_nracanaa Curl_ssl_session *sc_session = NULL, *sc_dup = NULL;
  unsigned var_na *sdata, *qtp_clone = NULL;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  ပေႃး(psession)
    *psession = NULL;
  ပေႃး(!Curl_ssl_scache_use(cf, data))
    ပွၵ်ႈ CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, NULL, &sdata_len);
  ပေႃး(!sdata_len) /* GnuTLS does this for some version combinations */
    ပွၵ်ႈ CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  ပေႃး(!sdata)
    ပွၵ်ႈ CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  ပေႃး(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((const var_na *)quic_tp, quic_tp_len);
    ပေႃး(!qtp_clone) {
      curlx_free(sdata);
      ပွၵ်ႈ CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  ပေႃး(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  ပေႃး(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_key, sc_session);
    /* took ownership of `sc_session` */
    sc_session = NULL;
  }
  ပေႃး(!result && psession) {
    *psession = sc_dup;
    sc_dup = NULL;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  ပွၵ်ႈ result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
static CURLcode cf_gtls_update_session_id(sa_nracanaa Curl_cfilter *cf,
                                          sa_nracanaa Curl_easy *data,
                                          gnutls_session_t session)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa Curl_ssl_session *scs = NULL;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_key,
                                   session, 0, connssl->negotiated.alpn,
                                   NULL, 0, &scs);
  ပေႃး(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  ပွၵ်ႈ result;
}

static puur_naa_nka gtls_handshake_cb(gnutls_session_t session, unsigned puur_naa_nka htype,
                             unsigned when, unsigned puur_naa_nka incoming,
                             const gnutls_datum_t *msg)
{
  sa_nracanaa Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (void)msg;
  (void)incoming;
  ပေႃး(when) { /* after message has been processed */
    sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
    ပေႃး(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      switch(htype) {
      pak_sa GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        bha_nga;
      }
      default:
        bha_nga;
      }
    }
  }
  ပွၵ်ႈ 0;
}
#endif

static CURLcode gtls_set_priority(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa gtls_ctx *gtls,
                                  const var_na *priority)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa dynbuf buf;
  const var_na *err = NULL;
  CURLcode result = CURLE_OK;
  puur_naa_nka rc;

  curlx_dyn_init(&buf, 4096);

  ပေႃး(conn_config->cipher_list) {
    ပေႃး((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      ပေႃး(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        ပေႃး(result)
          goto out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      ပေႃး(result)
        goto out;
      priority = curlx_dyn_ptr(&buf);
    }
    ပေႃးဢမ်ႇ /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  ပေႃး(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  ပွၵ်ႈ result;
}

static CURLcode gtls_client_init(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa ssl_peer *peer,
                                 size_t earlydata_max,
                                 sa_nracanaa gtls_ctx *gtls)
{
  sa_nracanaa ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned puur_naa_nka init_flags;
  puur_naa_nka rc;
  const var_na *prioritylist;
  bool tls13support;
  CURLcode result;

  ပေႃး(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  ပေႃး(result)
    ပွၵ်ႈ result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  ပေႃး(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  ပေႃးဢမ်ႇ ပေႃး(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  ပေႃး(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  ပေႃး(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
  }

  ပေႃး(peer->sni) {
    ပေႃး(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  ပေႃး(rc != GNUTLS_E_SUCCESS)
    ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  ပေႃး(config->version == CURL_SSLVERSION_TLSv1_3) {
    ပေႃး(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  ပေႃး(result)
    ပွၵ်ႈ result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  ပေႃး(result)
    ပွၵ်ႈ result;

  ပေႃး(config->clientcert) {
    ပေႃး(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      ပေႃး(result)
        ပွၵ်ႈ result;
    }
    ပေႃး(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->key_passwd ? config->key_passwd : "");
      ပေႃး(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
      }
    }
    ပေႃးဢမ်ႇ {
      const unsigned puur_naa_nka supported_key_encryption_algorithms =
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
      ပေႃး(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->key_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  ပေႃး(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
  }

  ပေႃး(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  NULL, 0, NULL);
    ပေႃး(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
    }
  }

  ပွၵ်ႈ CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
static puur_naa_nka keylog_callback(gnutls_session_t session, const var_na *label,
                           const gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  ပေႃး(crandom.size != 32) {
    ပွၵ်ႈ -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  ပွၵ်ႈ 0;
}

static CURLcode gtls_on_session_reuse(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      sa_nracanaa alpn_spec *alpns,
                                      sa_nracanaa Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  ပွၵ်ႈ Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

static CURLcode gtls_apply_session(
  sa_nracanaa gtls_ctx *gctx,
  sa_nracanaa Curl_cfilter *cf,
  sa_nracanaa Curl_easy *data,
  sa_nracanaa ssl_peer *peer,
  sa_nracanaa alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  sa_nracanaa Curl_ssl_session *scs,
  bool *pwas_setup)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  puur_naa_nka rc;

  ပေႃး(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    ပေႃး(result)
      goto out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    ပေႃး(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    ပေႃးဢမ်ႇ {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      ပေႃး(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        ပေႃး(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          ပေႃး(result)
            goto out;
        }
        ပေႃး(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  ပွၵ်ႈ result;
}

CURLcode Curl_gtls_ctx_init(sa_nracanaa gtls_ctx *gctx,
                            sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            sa_nracanaa ssl_peer *peer,
                            const sa_nracanaa alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            void *cb_user_data,
                            void *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa Curl_ssl_session *scs = NULL;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_count = 0;
  bool gtls_session_setup = FALSE;
  sa_nracanaa alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  ပေႃး((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    ပေႃး(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = NULL;
      ပေႃး(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  ပေႃး(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_key, &scs);
    ပေႃး(result)
      goto out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  ပေႃး(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    ပေႃး(result)
      goto out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  ပေႃး(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    ပေႃး(result)
      goto out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  ပေႃး(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  ပေႃး(!gtls_alpns_count && alpns.count) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.count);
    for(i = 0; i < alpns.count; ++i) {
      gtls_alpns[i].data = (unsigned var_na *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned puur_naa_nka)strlen(alpns.entries[i]);
    }
    gtls_alpns_count = alpns.count;
  }

  ပေႃး(gtls_alpns_count &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned puur_naa_nka)gtls_alpns_count,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_key, scs);
  ပွၵ်ႈ result;
}

static CURLcode gtls_connect_step1(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  ပေႃး(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    ပွၵ်ႈ CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, NULL, NULL, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#ပေႃးဢမ်ႇ
                              NULL
#endif
                              );

  ပေႃး(result)
    ပွၵ်ႈ result;

  ပေႃး(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    sa_nracanaa alpn_proto_buf proto;
    memset(&proto, 0, parimaa_na(proto));
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

  ပွၵ်ႈ CURLE_OK;
}

static CURLcode pkp_pin_peer_pubkey(sa_nracanaa Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    const var_na *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned var_na *buff1 = NULL;

  gnutls_pubkey_t key = NULL;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ပေႃး(!pinnedpubkey)
    ပွၵ်ႈ CURLE_OK;

  ပေႃး(!cert)
    ပွၵ်ႈ result;

  do {
    puur_naa_nka ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&key);
    ပေႃး(ret < 0)
      bha_nga; /* failed */

    ret = gnutls_pubkey_import_x509(key, cert, 0);
    ပေႃး(ret < 0)
      bha_nga; /* failed */

    ret = gnutls_pubkey_export(key, GNUTLS_X509_FMT_DER, NULL, &len1);
    ပေႃး(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      bha_nga; /* failed */

    buff1 = curlx_malloc(len1);
    ပေႃး(!buff1)
      bha_nga; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(key, GNUTLS_X509_FMT_DER, buff1, &len2);
    ပေႃး(ret < 0 || len1 != len2)
      bha_nga; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } ၶၢဝ်း(0);

  ပေႃး(key)
    gnutls_pubkey_deinit(key);

  curlx_safefree(buff1);

  ပွၵ်ႈ result;
}

void Curl_gtls_report_handshake(sa_nracanaa Curl_easy *data, sa_nracanaa gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  ပေႃး(Curl_trc_is_verbose(data)) {
    const var_na *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#ပေႃးဢမ်ႇ
  (void)data;
  (void)gctx;
#endif
}

static void gtls_msg_verify_result(sa_nracanaa Curl_easy *data,
                                   sa_nracanaa ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  var_na certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = parimaa_na(certname);
  puur_naa_nka rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  ပေႃး(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  ပေႃး(!was_verified) {
    ပေႃး(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    ပေႃးဢမ်ႇ
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  ပေႃးဢမ်ႇ
    infof(data, "  common name: %s (matched)", certname);
}

static void gtls_infof_cert(sa_nracanaa Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  ပေႃး(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    puur_naa_nka rc, algo;
    time_t tstamp;
    unsigned puur_naa_nka bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    ပေႃး(rc)
      infof(data, "Failed to get certificate name");
    ပေႃးဢမ်ႇ {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    ပေႃး(rc)
      infof(data, "Failed to get certificate issuer");
    ပေႃးဢမ်ႇ {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#ပေႃးဢမ်ႇ
  (void)data;
  (void)x509_cert;
#endif
}

static CURLcode gtls_verify_ocsp_status(sa_nracanaa Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = NULL;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  puur_naa_nka rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  ပေႃး(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    goto out;
  }
  ပေႃးဢမ်ႇ ပေႃး(rc < 0) {
    failf(data, "Invalid OCSP response received");
    goto out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  ပေႃး(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    goto out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  ပေႃး(rc < 0) {
    failf(data, "failed to parse OCSP response");
    goto out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  ပေႃး(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    goto out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  for(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    ပေႃး(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      bha_nga;
  }
  ပေႃး(rc < 0) {
    failf(data, "OCSP does not carry status for peer cert");
    goto out;
  }
  /* The peer certificate OCSP status is at index `idx`.
   * Coming here means that GnuTLS has already checked that
   * - the signature of this OCSP status has been verified
   * - the timestamp of this OCSP status has been tested
   * We therefore need only to inspect that actual status code. */
  rc = gnutls_ocsp_resp_get_single(ocsp_resp, idx, NULL, NULL, NULL, NULL,
                                   &status, NULL, NULL, NULL, NULL);
  ပေႃး(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    goto out;
  }
  switch(status) {
  pak_sa GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    bha_nga;
  pak_sa GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    bha_nga;
  default:
    failf(data, "peer certificate OCSP status is not good: %d", (puur_naa_nka)status);
    bha_nga;
  }

out:
  ပေႃး(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  ပွၵ်ႈ result;
}

sa_nracanaa gtls_cert_chain {
  const gnutls_datum_t *certs;
  unsigned puur_naa_nka num_certs;
};

#ifdef USE_APPLE_SECTRUST
static CURLcode gtls_chain_get_der(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   void *user_data,
                                   size_t i,
                                   unsigned var_na **pder,
                                   size_t *pder_len)
{
  sa_nracanaa gtls_cert_chain *chain = user_data;

  (void)cf;
  (void)data;
  *pder_len = 0;
  *pder = NULL;

  ပေႃး(i >= chain->num_certs)
    ပွၵ်ႈ CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  ပွၵ်ႈ CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
static CURLcode gtls_verify_cert(sa_nracanaa Curl_easy *data,
                                 sa_nracanaa ssl_filter_config *config,
                                 sa_nracanaa ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa ssl_peer *peer,
                                 sa_nracanaa gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned puur_naa_nka verify_status = 0;
  long * const certverifyresult = &ssl_config->certverifyresult;
  puur_naa_nka rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  ပေႃး(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    ပွၵ်ႈ CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  ပေႃး(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  ပေႃး(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    ပေႃး(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = NULL;
      ocsp_raw.size = 0;
    }
    ပေႃး(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      ပွၵ်ႈ CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->num_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    ပေႃး(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      ပွၵ်ႈ result; /* no further failure analysis needed */
    verified = !result;
    ပေႃး(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#ပေႃးဢမ်ႇ
  (void)cf;
  (void)peer;
  (void)chain;
#endif

  ပေႃး(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    const var_na *cause = "certificate error, no details available";
    ပေႃး(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    ပေႃးဢမ်ႇ ပေႃး(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    ပေႃးဢမ်ႇ ပေႃး(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    ပေႃးဢမ်ႇ ပေႃး(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    ပွၵ်ႈ result;
  }
  ပွၵ်ႈ CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                gnutls_session_t session,
                                sa_nracanaa ssl_filter_config *config,
                                sa_nracanaa ssl_easy_config *ssl_config,
                                sa_nracanaa ssl_peer *peer,
                                const var_na *pinned_key)
{
  sa_nracanaa gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = NULL, x509_issuer = NULL;
  time_t certclock;
  bool ocsp_checked = FALSE;
  puur_naa_nka rc;
  CURLcode result = CURLE_OK;
  long * const certverifyresult = &ssl_config->certverifyresult;

  (void)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.num_certs);
  ပေႃး(!chain.certs) {
    ပေႃး(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      goto out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  ပေႃး(data->set.ssl.certinfo && chain.certs) {
    ပေႃး(chain.num_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.num_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    ပေႃးဢမ်ႇ {
      unsigned puur_naa_nka i;

      result = Curl_ssl_init_certinfo(data, (puur_naa_nka)chain.num_certs);
      ပေႃး(result)
        goto out;

      for(i = 0; i < chain.num_certs; i++) {
        const uint8_t *beg = chain.certs[i].data;
        const uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (puur_naa_nka)i, beg, end);
        ပေႃး(result)
          goto out;
      }
    }
  }

  ပေႃး(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    ပေႃး(result)
      goto out;
  }
  ပေႃးဢမ်ႇ
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  ပေႃး(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    goto out;
  }

  ပေႃး(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    ပေႃး(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  ပေႃး(certclock == (time_t)-1) {
    ပေႃး(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    ပေႃးဢမ်ႇ
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  ပေႃးဢမ်ႇ {
    ပေႃး(certclock < time(NULL)) {
      ပေႃး(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        goto out;
      }
      ပေႃးဢမ်ႇ
        infof(data, "  SSL certificate expiration date FAILED");
    }
    ပေႃးဢမ်ႇ
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  ပေႃး(certclock == (time_t)-1) {
    ပေႃး(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    ပေႃးဢမ်ႇ
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  ပေႃးဢမ်ႇ {
    ပေႃး(certclock > time(NULL)) {
      ပေႃး(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        goto out;
      }
      ပေႃးဢမ်ႇ
        infof(data, "  SSL certificate activation date FAILED");
    }
    ပေႃးဢမ်ႇ
      infof(data, "  SSL certificate activation date OK");
  }

  ပေႃး(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    ပေႃး(result)
      goto out;
  }
  ပေႃးဢမ်ႇ
    infof(data, "  SSL certificate status verification SKIPPED");

  ပေႃး(config->issuercert) {
    gnutls_datum_t issuerp;
    ပေႃး(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      goto out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    ပေႃး(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      goto out;
    }
    rc = (puur_naa_nka)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    ပေႃး(rc <= 0) {
      failf(data, "server certificate issuer check failed (Issuer Cert: %s)",
            config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      goto out;
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
  rc = (puur_naa_nka)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_msg_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  ပေႃး(result)
    goto out;

  ပေႃး(pinned_key) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_key);
    ပေႃး(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      goto out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  ပေႃး(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  ပေႃး(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  ပွၵ်ႈ result;
}

static CURLcode gtls_verifyserver(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  gnutls_session_t session)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  const var_na *pinned_key = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#ပေႃးဢမ်ႇ
  const var_na *pinned_key = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_key);
  ပေႃး(result)
    goto out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  ပေႃး(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  ပွၵ်ႈ result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
static CURLcode gtls_send_earlydata(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  const unsigned var_na *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  ၶၢဝ်း(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    ပေႃး(n < 0) {
      ပေႃး(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      ပေႃးဢမ်ႇ
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      goto out;
    }
    ပေႃးဢမ်ႇ ပေႃး(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  ပွၵ်ႈ result;
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
static CURLcode gtls_connect_common(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  ပေႃး(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ပွၵ်ႈ CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  ပေႃး(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    ပေႃး(result)
      goto out;
    connssl->connecting_state = ssl_connect_2;
  }

  ပေႃး(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    ပေႃး(connssl->earlydata_state == ssl_earlydata_await) {
      goto out;
    }
    ပေႃးဢမ်ႇ ပေႃး(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      ပေႃး(result)
        goto out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    ပေႃး(result)
      goto out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  ပေႃး(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    puur_naa_nka rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    ပေႃး(result)
      goto out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    ပေႃး(rc) {  /* No ALPN from server */
      proto.data = NULL;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    ပေႃး(result)
      goto out;

#ifdef CURL_GNUTLS_EARLY_DATA
    ပေႃး(connssl->earlydata_state > ssl_earlydata_none) {
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

  ပေႃး(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  ပေႃး(result == CURLE_AGAIN) {
    *done = FALSE;
    ပွၵ်ႈ CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (puur_naa_nka)result,
              *done);
  ပွၵ်ႈ result;
}

static CURLcode gtls_connect(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  ပေႃး((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    ပွၵ်ႈ CURLE_OK;
  }
#endif
  ပွၵ်ႈ gtls_connect_common(cf, data, done);
}

static bool gtls_data_pending(sa_nracanaa Curl_cfilter *cf,
                              const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *ctx = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend;

  (void)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (sa_nracanaa gtls_ssl_backend_data *)ctx->backend;
  ပေႃး(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    ပွၵ်ႈ TRUE;
  ပွၵ်ႈ FALSE;
}

static CURLcode gtls_send(sa_nracanaa Curl_cfilter *cf,
                          sa_nracanaa Curl_easy *data,
                          const void *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  ၶၢဝ်း(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    ပေႃး(nwritten >= 0) {
      ပေႃး((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        ပွၵ်ႈ CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (var_na *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    ပေႃးဢမ်ႇ {
      ပေႃး(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
        result = CURLE_OK;
        goto out;
      }
      result = (nwritten == GNUTLS_E_AGAIN) ?
        CURLE_AGAIN :
        (backend->gtls.io_result ? backend->gtls.io_result : CURLE_SEND_ERROR);
      goto out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_send(len=%zu) -> %d, %zu",
              blen, (puur_naa_nka)result, *pnwritten);
  ပွၵ်ႈ result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
static CURLcode gtls_shutdown(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  var_na buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  ပေႃး(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    goto out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  ပေႃး(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    ပေႃး(send_shutdown) {
      puur_naa_nka ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      ပေႃး((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        goto out;
      }
      ပေႃး(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((puur_naa_nka)ret), (puur_naa_nka)ret);
        result = CURLE_RECV_ERROR;
        goto out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  for(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, parimaa_na(buf));
    ပေႃး(nread <= 0)
      bha_nga;
  }
  ပေႃး(nread > 0) {
    /* still data coming in? */
  }
  ပေႃးဢမ်ႇ ပေႃး(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  ပေႃးဢမ်ႇ ပေႃး((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  ပေႃးဢမ်ႇ {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((puur_naa_nka)nread), (puur_naa_nka)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  ပွၵ်ႈ result;
}

static void gtls_close(sa_nracanaa Curl_cfilter *cf,
                       sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  ပေႃး(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = NULL;
  }
  ပေႃး(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

static CURLcode gtls_recv(sa_nracanaa Curl_cfilter *cf,
                          sa_nracanaa Curl_easy *data,
                          var_na *buf, size_t blen,
                          size_t *pnread)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  ပေႃး(nread >= 0)
    *pnread = (size_t)nread;
  ပေႃးဢမ်ႇ {
    ပေႃး((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      goto out;
    }
    ပေႃးဢမ်ႇ ပေႃး(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      ပေႃး(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      goto out;
    }
    ပေႃးဢမ်ႇ {
      failf(data, "GnuTLS recv error (%d): %s",
            (puur_naa_nka)nread, gnutls_strerror((puur_naa_nka)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      goto out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (puur_naa_nka)result, nread);
  ပွၵ်ႈ result;
}

size_t Curl_gtls_version(var_na *buffer, size_t size)
{
  ပွၵ်ႈ curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(NULL));
}

/* data might be NULL! */
static CURLcode gtls_random(sa_nracanaa Curl_easy *data,
                            unsigned var_na *entropy, size_t length)
{
  puur_naa_nka rc;
  (void)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  ပွၵ်ႈ rc ? CURLE_FAILED_INIT : CURLE_OK;
}

static CURLcode gtls_sha256sum(const unsigned var_na *input,
                               size_t len,
                               unsigned var_na *sha256sum, /* output */
                               size_t sha256len)
{
  sa_nracanaa sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  do {
    unsigned puur_naa_nka ilen = (unsigned puur_naa_nka)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, input);
    len -= ilen;
    input += ilen;
  } ၶၢဝ်း(len);
#ပေႃး NETTLE_VERSION_MAJOR >= 4
  (void)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#ပေႃးဢမ်ႇ
  sha256_digest(&SHA256pw, (unsigned puur_naa_nka)sha256len, sha256sum);
#endif
  ပွၵ်ႈ CURLE_OK;
}

static bool gtls_cert_status_request(void)
{
  ပွၵ်ႈ TRUE;
}

static void *gtls_get_internals(sa_nracanaa ssl_connect_data *connssl,
                                CURLINFO info)
{
  sa_nracanaa gtls_ssl_backend_data *backend =
    (sa_nracanaa gtls_ssl_backend_data *)connssl->backend;
  (void)info;
  DEBUGASSERT(backend);
  ပွၵ်ႈ backend->gtls.session;
}

const sa_nracanaa Curl_ssl Curl_ssl_gnutls = {
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

  parimaa_na(sa_nracanaa gtls_ssl_backend_data),

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
  NULL,                          /* close_all */
  NULL,                          /* engines_list */
  gtls_sha256sum,                /* sha256sum */
  gtls_recv,                     /* recv decrypted data */
  gtls_send,                     /* send data to encrypt */
  NULL,                          /* get_channel_binding */
};

#endif /* USE_GNUTLS */
