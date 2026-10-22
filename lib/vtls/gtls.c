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
#якщо 0
#define GTLSDEBUG
#endif

#ifdef GTLSDEBUG
static void tls_log_func(ціле level, const char *str)
{
  curl_mfprintf(stderr, "|<%d>| %s", level, str);
}
#endif

#якщо !defined(GNUTLS_VERSION_NUMBER) || (GNUTLS_VERSION_NUMBER < 0x030605)
#error "too old GnuTLS version"
#endif

#undef CURL_GNUTLS_EARLY_DATA
#якщо GNUTLS_VERSION_NUMBER >= 0x030702
#define CURL_GNUTLS_EARLY_DATA
#endif

#include <gnutls/ocsp.h>

struct gtls_ssl_backend_data {
  struct gtls_ctx gtls;
};

static ssize_t gtls_push(void *s, const void *buf, size_t blen)
{
  struct Curl_cfilter *cf = s;
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  struct Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "gtls_push(len=%zu) -> %d, %zu",
              blen, (ціле)result, nwritten);
  backend->gtls.io_result = result;
  якщо(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    повернути -1;
  }
  повернути (ssize_t)nwritten;
}

static ssize_t gtls_pull(void *s, void *buf, size_t blen)
{
  struct Curl_cfilter *cf = s;
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  struct Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread;
  CURLcode result;

  DEBUGASSERT(data);
  якщо(!backend->gtls.shared_creds->trust_setup) {
    result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    якщо(result) {
      /* !checksrc! disable ERRNOVAR 1 */
      gnutls_transport_set_errno(backend->gtls.session, EINVAL);
      backend->gtls.io_result = result;
      повернути -1;
    }
  }

  result = Curl_conn_cf_recv(cf->next, data, buf, blen, &nread);
  CURL_TRC_CF(data, cf, "gtls_pull(len=%zu) -> %d, %zu", blen, (ціле)result,
              nread);
  backend->gtls.io_result = result;
  якщо(result) {
    /* !checksrc! disable ERRNOVAR 1 */
    gnutls_transport_set_errno(backend->gtls.session,
                               (result == CURLE_AGAIN) ? EAGAIN : EINVAL);
    повернути -1;
  }
  інакше якщо(nread == 0)
    connssl->peer_closed = TRUE;
  повернути (ssize_t)nread;
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
static ціле gtls_init(void)
{
  ціле ret = 1;
  ret = gnutls_global_init() ? 0 : 1;
#ifdef GTLSDEBUG
  gnutls_global_set_log_function(tls_log_func);
  gnutls_global_set_log_level(2);
#endif
  повернути ret;
}

static void gtls_cleanup(void)
{
  gnutls_global_deinit();
  Curl_tls_keylog_close();
}

#ifdef CURLVERBOSE
static void showtime(struct Curl_easy *data, const char *text, time_t stamp)
{
  struct tm buffer;
  const struct tm *tm = &buffer;
  char str[96];
  CURLcode result = curlx_gmtime(stamp, &buffer);
  якщо(result)
    повернути;

  curl_msnprintf(str,
                 sizeof(str),
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

static gnutls_datum_t load_file(const char *file)
{
  FILE *f;
  gnutls_datum_t loaded_file = { NULL, 0 };
  long filelen;
  void *ptr;

  f = curlx_fopen(file, "rb");
  якщо(!f)
    повернути loaded_file;
  якщо(fseek(f, 0, SEEK_END))
    goto out;
  filelen = ftell(f);
  якщо(filelen < 0 || filelen > CURL_MAX_INPUT_LENGTH)
    goto out;
  якщо(fseek(f, 0, SEEK_SET))
    goto out;
  ptr = curlx_malloc((size_t)filelen);
  якщо(!ptr)
    goto out;
  якщо(fread(ptr, 1, (size_t)filelen, f) < (size_t)filelen) {
    curlx_free(ptr);
    goto out;
  }

  loaded_file.data = ptr;
  loaded_file.size = (unsigned ціле)filelen;
out:
  curlx_fclose(f);
  повернути loaded_file;
}

static void unload_file(gnutls_datum_t data)
{
  curlx_free(data.data);
}

/* this function does an SSL/TLS (re-)handshake */
static CURLcode cf_gtls_handshake(struct Curl_cfilter *cf,
                                  struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  gnutls_session_t session;
  ціле rc;

  DEBUGASSERT(backend);
  session = backend->gtls.session;

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  backend->gtls.io_result = CURLE_OK;
  rc = gnutls_handshake(session);

  якщо(!backend->gtls.shared_creds->trust_setup) {
    /* After having send off the ClientHello, we prepare the trust
     * store to verify the coming certificate from the server */
    CURLcode result = Curl_gtls_client_trust_setup(cf, data, &backend->gtls);
    якщо(result)
      повернути result;
  }

  якщо((rc == GNUTLS_E_AGAIN) || (rc == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need =
      gnutls_record_get_direction(session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
    повернути CURLE_AGAIN;
  }
  інакше якщо((rc < 0) && !gnutls_error_is_fatal(rc)) {
#ifdef CURLVERBOSE
    const char *strerr = NULL;

    якщо(rc == GNUTLS_E_WARNING_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    якщо(!strerr)
      strerr = gnutls_strerror(rc);

    infof(data, "gnutls_handshake() warning: %s", strerr);
#endif
    повернути CURLE_AGAIN;
  }
  інакше якщо((rc < 0) && backend->gtls.io_result) {
    повернути backend->gtls.io_result;
  }
  інакше якщо(rc < 0) {
    const char *strerr = NULL;

    якщо(rc == GNUTLS_E_FATAL_ALERT_RECEIVED) {
      gnutls_alert_description_t alert = gnutls_alert_get(session);
      strerr = gnutls_alert_get_name(alert);
    }

    якщо(!strerr)
      strerr = gnutls_strerror(rc);

    failf(data, "GnuTLS, handshake failed: %s", strerr);
    повернути CURLE_SSL_CONNECT_ERROR;
  }

  повернути CURLE_OK;
}

static gnutls_x509_crt_fmt_t gnutls_do_file_type(const char *type)
{
  якщо(!type || !type[0])
    повернути GNUTLS_X509_FMT_PEM;
  якщо(curl_strequal(type, "PEM"))
    повернути GNUTLS_X509_FMT_PEM;
  якщо(curl_strequal(type, "DER"))
    повернути GNUTLS_X509_FMT_DER;
  повернути GNUTLS_X509_FMT_PEM; /* default to PEM */
}

#define GNUTLS_CIPHERS "NORMAL:%PROFILE_MEDIUM:-ARCFOUR-128:" \
  "-CTYPE-ALL:+CTYPE-X509"

#define QUIC_PRIORITY \
  "NORMAL:%PROFILE_MEDIUM:-VERS-ALL:+VERS-TLS1.3:-CIPHER-ALL:+AES-128-GCM:" \
  "+AES-256-GCM:+CHACHA20-POLY1305:+AES-128-CCM:-GROUP-ALL:" \
  "+GROUP-SECP256R1:+GROUP-X25519:+GROUP-SECP384R1:+GROUP-SECP521R1:" \
  "%DISABLE_TLS13_COMPAT_MODE"

static CURLcode gnutls_set_ssl_version_min_max(
  struct Curl_easy *data,
  struct ssl_peer *peer,
  struct ssl_filter_config *conn_config,
  const char **prioritylist,
  bool tls13support)
{
  long ssl_version = conn_config->version;
  long ssl_version_max = conn_config->version_max;

  DEBUGASSERT(ssl_version != CURL_SSLVERSION_DEFAULT);
  якщо(ssl_version <= CURL_SSLVERSION_TLSv1)
    ssl_version = CURL_SSLVERSION_TLSv1_0;
  якщо((ssl_version_max == CURL_SSLVERSION_MAX_NONE) ||
     (ssl_version_max == CURL_SSLVERSION_MAX_DEFAULT))
    ssl_version_max = tls13support ?
      CURL_SSLVERSION_MAX_TLSv1_3 : CURL_SSLVERSION_MAX_TLSv1_2;

  якщо(peer->transport == TRNSPRT_QUIC) {
    якщо(ssl_version_max < CURL_SSLVERSION_MAX_TLSv1_3) {
      failf(data, "QUIC needs at least TLS version 1.3");
      повернути CURLE_SSL_CONNECT_ERROR;
    }
    *prioritylist = QUIC_PRIORITY;
    повернути CURLE_OK;
  }

  switch(ssl_version | ssl_version_max) {
  case CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_0:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.0";
    повернути CURLE_OK;
  case CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1:+VERS-TLS1.0";
    повернути CURLE_OK;
  case CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1:+VERS-TLS1.0";
    повернути CURLE_OK;
  case CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_1:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.1";
    повернути CURLE_OK;
  case CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2:+VERS-TLS1.1";
    повернути CURLE_OK;
  case CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_2:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.2";
    повернути CURLE_OK;
  case CURL_SSLVERSION_TLSv1_3 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3";
    повернути CURLE_OK;
  case CURL_SSLVERSION_TLSv1_0 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0";
    повернути CURLE_OK;
  case CURL_SSLVERSION_TLSv1_1 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2:+VERS-TLS1.1";
    повернути CURLE_OK;
  case CURL_SSLVERSION_TLSv1_2 | CURL_SSLVERSION_MAX_TLSv1_3:
    *prioritylist = GNUTLS_CIPHERS ":-VERS-SSL3.0:-VERS-TLS-ALL:"
      "+VERS-TLS1.3:+VERS-TLS1.2";
    повернути CURLE_OK;
  }

  failf(data, "GnuTLS: cannot set TLS protocol");
  повернути CURLE_SSL_CONNECT_ERROR;
}

CURLcode Curl_gtls_shared_creds_create(struct Curl_easy *data,
                                       struct gtls_shared_creds **pcreds)
{
  struct gtls_shared_creds *shared;
  ціле rc;

  *pcreds = NULL;
  shared = curlx_calloc(1, sizeof(*shared));
  якщо(!shared)
    повернути CURLE_OUT_OF_MEMORY;

  rc = gnutls_certificate_allocate_credentials(&shared->creds);
  якщо(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_certificate_allocate_credentials() failed: %s",
          gnutls_strerror(rc));
    curlx_free(shared);
    повернути CURLE_SSL_CONNECT_ERROR;
  }

  shared->refcount = 1;
  shared->time = *Curl_pgrs_now(data);
  *pcreds = shared;
  повернути CURLE_OK;
}

CURLcode Curl_gtls_shared_creds_up_ref(struct gtls_shared_creds *creds)
{
  DEBUGASSERT(creds);
  якщо(creds->refcount < SIZE_MAX) {
    ++creds->refcount;
    повернути CURLE_OK;
  }
  повернути CURLE_BAD_FUNCTION_ARGUMENT;
}

void Curl_gtls_shared_creds_free(struct gtls_shared_creds **pcreds)
{
  struct gtls_shared_creds *shared = *pcreds;
  *pcreds = NULL;
  якщо(shared) {
    --shared->refcount;
    якщо(!shared->refcount) {
      gnutls_certificate_free_credentials(shared->creds);
      curlx_free(shared->CAfile);
      curlx_free(shared);
    }
  }
}

static CURLcode gtls_populate_creds(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    gnutls_certificate_credentials_t creds)
{
  struct ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  struct ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  bool creds_are_empty = TRUE;
  ціле rc;

  якщо(!config->verifypeer) {
    infof(data, "SSL Trust: peer verification disabled");
    повернути CURLE_OK;
  }

  infof(data, "SSL Trust Anchors:");
  якщо(config->native_ca_store) {
#ifdef USE_APPLE_SECTRUST
    infof(data, "  Native: Apple SecTrust");
    creds_are_empty = FALSE;
#інакше
    rc = gnutls_certificate_set_x509_system_trust(creds);
    якщо(rc < 0)
      infof(data, "error reading native CA store (%s), continuing anyway",
            gnutls_strerror(rc));
    інакше {
      infof(data, "  Native: %d certificates from system trust", rc);
      якщо(rc > 0)
        creds_are_empty = FALSE;
    }
#endif
  }

  якщо(config->ca_info_blob) {
    gnutls_datum_t ca_info_datum;
    якщо(config->ca_info_blob->len > (size_t)UINT_MAX) {
      failf(data, "certificate blob too long: %zu bytes",
            config->ca_info_blob->len);
      повернути CURLE_SSL_CACERT_BADFILE;
    }
    ca_info_datum.data = config->ca_info_blob->data;
    ca_info_datum.size = (unsigned ціле)config->ca_info_blob->len;
    rc = gnutls_certificate_set_x509_trust_mem(creds, &ca_info_datum,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    якщо(rc < 0) {
      infof(data, "error reading CA cert blob (%s)%s", gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      якщо(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        повернути CURLE_SSL_CACERT_BADFILE;
      }
    }
    інакше
      infof(data, "  CA Blob: %d certificates", rc);
  }
  /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
  інакше якщо(config->CAfile) {
    /* set the trusted CA cert bundle file */
    gnutls_certificate_set_verify_flags(creds,
                                        GNUTLS_VERIFY_ALLOW_X509_V1_CA_CRT);

    rc = gnutls_certificate_set_x509_trust_file(creds,
                                                config->CAfile,
                                                GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    якщо(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CAfile, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      якщо(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        повернути CURLE_SSL_CACERT_BADFILE;
      }
    }
    інакше
      infof(data, "  CAfile: %d certificates in %s", rc, config->CAfile);
  }

  якщо(config->CApath) {
    /* set the trusted CA cert directory */
    rc = gnutls_certificate_set_x509_trust_dir(creds, config->CApath,
                                               GNUTLS_X509_FMT_PEM);
    creds_are_empty = creds_are_empty && (rc <= 0);
    якщо(rc < 0) {
      infof(data, "error reading CA cert file %s (%s)%s",
            config->CApath, gnutls_strerror(rc),
            (creds_are_empty ? "" : ", continuing anyway"));
      якщо(creds_are_empty) {
        ssl_config->certverifyresult = rc;
        повернути CURLE_SSL_CACERT_BADFILE;
      }
    }
    інакше
      infof(data, "  CApath: %d certificates in %s", rc, config->CApath);
  }

  якщо(creds_are_empty)
    infof(data, "  no trust anchors configured");

#ifdef USE_APPLE_SECTRUST
  якщо(config->CRLfile && config->native_ca_store) {
    failf(data, "gnutls: CRL file not supported with native CA store; "
          "the platform verifier has no CRL attachment API");
    повернути CURLE_NOT_BUILT_IN;
  }
#endif
  якщо(config->CRLfile) {
    /* set the CRL list file */
    rc = gnutls_certificate_set_x509_crl_file(creds, config->CRLfile,
                                              GNUTLS_X509_FMT_PEM);
    якщо(rc < 0) {
      failf(data, "error reading CRL file %s (%s)",
            config->CRLfile, gnutls_strerror(rc));
      повернути CURLE_SSL_CRL_BADFILE;
    }
    інакше
      infof(data, "  CRLfile: %d CRL in %s", rc, config->CRLfile);
  }

  повернути CURLE_OK;
}

/* key to use at `multi->proto_hash` */
#define MPROTO_GTLS_X509_KEY   "tls:gtls:x509:share"

static bool gtls_shared_creds_expired(struct Curl_easy *data,
                                      const struct gtls_shared_creds *sc)
{
  timediff_t elapsed_ms = curlx_ptimediff_ms(Curl_pgrs_now(data), &sc->time);
  timediff_t timeout_ms = data->set.ssl_ca_cache_timeout * (timediff_t)1000;

  якщо(timeout_ms < 0)
    повернути FALSE;

  повернути elapsed_ms >= timeout_ms;
}

static bool gtls_shared_creds_different(struct Curl_cfilter *cf,
                                        const struct gtls_shared_creds *sc)
{
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  якщо(!sc->CAfile || !conn_config->CAfile)
    повернути sc->CAfile != conn_config->CAfile;

  повернути strcmp(sc->CAfile, conn_config->CAfile);
}

static struct gtls_shared_creds *gtls_get_cached_creds(struct Curl_cfilter *cf,
                                                       struct Curl_easy *data)
{
  struct gtls_shared_creds *shared_creds;

  якщо(data->multi) {
    shared_creds = Curl_hash_pick(&data->multi->proto_hash,
                                  MPROTO_GTLS_X509_KEY,
                                  CURL_CSTRLEN(MPROTO_GTLS_X509_KEY));
    якщо(shared_creds && shared_creds->creds &&
       !gtls_shared_creds_expired(data, shared_creds) &&
       !gtls_shared_creds_different(cf, shared_creds)) {
      повернути shared_creds;
    }
  }
  повернути NULL;
}

static void gtls_shared_creds_hash_free(const void *key,
                                        size_t key_len, void *p)
{
  struct gtls_shared_creds *sc = p;
  DEBUGASSERT(key_len == CURL_CSTRLEN(MPROTO_GTLS_X509_KEY));
  DEBUGASSERT(!memcmp(MPROTO_GTLS_X509_KEY, key, key_len));
  (void)key;
  (void)key_len;
  Curl_gtls_shared_creds_free(&sc); /* down reference */
}

static void gtls_set_cached_creds(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  struct gtls_shared_creds *sc)
{
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);

  DEBUGASSERT(sc);
  DEBUGASSERT(sc->creds);
  DEBUGASSERT(!sc->CAfile);
  DEBUGASSERT(sc->refcount == 1);
  якщо(!data->multi)
    повернути;

  якщо(conn_config->CAfile) {
    sc->CAfile = curlx_strdup(conn_config->CAfile);
    якщо(!sc->CAfile)
      повернути;
  }

  якщо(Curl_gtls_shared_creds_up_ref(sc))
    повернути;

  якщо(!Curl_hash_add2(&data->multi->proto_hash,
                     MPROTO_GTLS_X509_KEY,
                     CURL_CSTRLEN(MPROTO_GTLS_X509_KEY),
                     sc, gtls_shared_creds_hash_free)) {
    Curl_gtls_shared_creds_free(&sc); /* down reference again */
    повернути;
  }
}

CURLcode Curl_gtls_client_trust_setup(struct Curl_cfilter *cf,
                                      struct Curl_easy *data,
                                      struct gtls_ctx *gtls)
{
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  struct gtls_shared_creds *cached_creds = NULL;
  bool cache_criteria_met;
  CURLcode result;
  ціле rc;

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

  якщо(cache_criteria_met)
    cached_creds = gtls_get_cached_creds(cf, data);

  якщо(cached_creds && !Curl_gtls_shared_creds_up_ref(cached_creds)) {
    CURL_TRC_CF(data, cf, "using shared trust anchors and CRLs");
    Curl_gtls_shared_creds_free(&gtls->shared_creds);
    gtls->shared_creds = cached_creds;
    rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                                gtls->shared_creds->creds);
    якщо(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
      повернути CURLE_SSL_CONNECT_ERROR;
    }
  }
  інакше {
    CURL_TRC_CF(data, cf, "loading trust anchors and CRLs");
    result = gtls_populate_creds(cf, data, gtls->shared_creds->creds);
    якщо(result)
      повернути result;
    gtls->shared_creds->trust_setup = TRUE;
    якщо(cache_criteria_met)
      gtls_set_cached_creds(cf, data, gtls->shared_creds);
  }
  повернути CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
static ціле gtls_get_ietf_proto(gnutls_session_t session)
{
  switch(gnutls_protocol_get_version(session)) {
  case GNUTLS_SSL3:
    повернути CURL_IETF_PROTO_SSL3;
  case GNUTLS_TLS1_0:
    повернути CURL_IETF_PROTO_TLS1;
  case GNUTLS_TLS1_1:
    повернути CURL_IETF_PROTO_TLS1_1;
  case GNUTLS_TLS1_2:
    повернути CURL_IETF_PROTO_TLS1_2;
  case GNUTLS_TLS1_3:
    повернути CURL_IETF_PROTO_TLS1_3;
  default:
    повернути CURL_IETF_PROTO_UNKNOWN;
  }
}

CURLcode Curl_gtls_cache_session(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 const char *ssl_peer_key,
                                 gnutls_session_t session,
                                 curl_off_t valid_until,
                                 const char *alpn,
                                 unsigned char *quic_tp,
                                 size_t quic_tp_len,
                                 struct Curl_ssl_session **psession)
{
  struct Curl_ssl_session *sc_session = NULL, *sc_dup = NULL;
  unsigned char *sdata, *qtp_clone = NULL;
  size_t sdata_len = 0;
  size_t earlydata_max = 0;
  CURLcode result = CURLE_OK;

  якщо(psession)
    *psession = NULL;
  якщо(!Curl_ssl_scache_use(cf, data))
    повернути CURLE_OK;

  /* we always unconditionally get the session id here, as even if we
     already got it from the cache and asked to use it in the connection, it
     might have been rejected and then a new one is in use now and we need to
     detect that. */

  /* get the session ID data size */
  gnutls_session_get_data(session, NULL, &sdata_len);
  якщо(!sdata_len) /* GnuTLS does this for some version combinations */
    повернути CURLE_OK;

  sdata = curlx_malloc(sdata_len); /* get a buffer for it */
  якщо(!sdata)
    повернути CURLE_OUT_OF_MEMORY;

  /* extract session ID to the allocated buffer */
  gnutls_session_get_data(session, sdata, &sdata_len);
  earlydata_max = gnutls_record_get_max_early_data_size(session);

  CURL_TRC_CF(data, cf, "get session id (len=%zu, alpn=%s, earlymax=%zu) "
              "and store in cache", sdata_len, alpn ? alpn : "-",
              earlydata_max);
  якщо(quic_tp && quic_tp_len) {
    qtp_clone = curlx_memdup0((const char *)quic_tp, quic_tp_len);
    якщо(!qtp_clone) {
      curlx_free(sdata);
      повернути CURLE_OUT_OF_MEMORY;
    }
  }

  result = Curl_ssl_session_create2(sdata, sdata_len,
                                    gtls_get_ietf_proto(session),
                                    alpn, valid_until, earlydata_max,
                                    qtp_clone, quic_tp_len,
                                    &sc_session);
  /* call took ownership of `sdata` and `qtp_clone` */
  якщо(!result && psession &&  /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP))
    result = Curl_ssl_session_dup(sc_session, &sc_dup);
  якщо(!result) {
    result = Curl_ssl_scache_put(cf, data, ssl_peer_key, sc_session);
    /* took ownership of `sc_session` */
    sc_session = NULL;
  }
  якщо(!result && psession) {
    *psession = sc_dup;
    sc_dup = NULL;
  }
  Curl_ssl_session_destroy(sc_session);
  Curl_ssl_session_destroy(sc_dup);
  повернути result;
}
#endif

#ifdef CURL_GNUTLS_EARLY_DATA
static CURLcode cf_gtls_update_session_id(struct Curl_cfilter *cf,
                                          struct Curl_easy *data,
                                          gnutls_session_t session)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct Curl_ssl_session *scs = NULL;
  CURLcode result;

  result = Curl_gtls_cache_session(cf, data, connssl->peer.scache_key,
                                   session, 0, connssl->negotiated.alpn,
                                   NULL, 0, &scs);
  якщо(!result) {
    Curl_ssl_session_destroy(connssl->session);
    connssl->session = scs;
  }
  повернути result;
}

static ціле gtls_handshake_cb(gnutls_session_t session, unsigned ціле htype,
                             unsigned when, unsigned ціле incoming,
                             const gnutls_datum_t *msg)
{
  struct Curl_cfilter *cf = gnutls_session_get_ptr(session);

  (void)msg;
  (void)incoming;
  якщо(when) { /* after message has been processed */
    struct Curl_easy *data = CF_DATA_CURRENT(cf);
    якщо(data) {
      CURL_TRC_CF(data, cf, "handshake: %s message type %u",
                  incoming ? "incoming" : "outgoing", htype);
      switch(htype) {
      case GNUTLS_HANDSHAKE_NEW_SESSION_TICKET: {
        cf_gtls_update_session_id(cf, data, session);
        break;
      }
      default:
        break;
      }
    }
  }
  повернути 0;
}
#endif

static CURLcode gtls_set_priority(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  struct gtls_ctx *gtls,
                                  const char *priority)
{
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  struct dynbuf buf;
  const char *err = NULL;
  CURLcode result = CURLE_OK;
  ціле rc;

  curlx_dyn_init(&buf, 4096);

  якщо(conn_config->cipher_list) {
    якщо((conn_config->cipher_list[0] == '+') ||
       (conn_config->cipher_list[0] == '-') ||
       (conn_config->cipher_list[0] == '!')) {
      /* add it to our own */
      якщо(!curlx_dyn_len(&buf)) {  /* not added yet */
        result = curlx_dyn_add(&buf, priority);
        якщо(result)
          goto out;
      }
      result = curlx_dyn_addf(&buf, ":%s", conn_config->cipher_list);
      якщо(result)
        goto out;
      priority = curlx_dyn_ptr(&buf);
    }
    інакше /* replace our own completely */
      priority = conn_config->cipher_list;
  }

  infof(data, "GnuTLS priority: %s", priority);
  rc = gnutls_priority_set_direct(gtls->session, priority, &err);
  якщо(rc != GNUTLS_E_SUCCESS) {
    failf(data, "Error %d setting GnuTLS priority: %s", rc, err);
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  curlx_dyn_free(&buf);
  повернути result;
}

static CURLcode gtls_client_init(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 struct ssl_peer *peer,
                                 size_t earlydata_max,
                                 struct gtls_ctx *gtls)
{
  struct ssl_filter_config *config = Curl_ssl_cf_get_filter_config(cf);
  struct ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
  unsigned ціле init_flags;
  ціле rc;
  const char *prioritylist;
  bool tls13support;
  CURLcode result;

  якщо(config->version == CURL_SSLVERSION_SSLv2 ||
     config->version == CURL_SSLVERSION_SSLv3) {
    failf(data, "GnuTLS does not support SSLv2 or SSLv3");
    повернути CURLE_SSL_CONNECT_ERROR;
  }

  /* allocate a shared creds struct */
  result = Curl_gtls_shared_creds_create(data, &gtls->shared_creds);
  якщо(result)
    повернути result;

  ssl_config->certverifyresult = 0;

  /* Initialize TLS session as a client */
  init_flags = GNUTLS_CLIENT;
#ifdef CURL_GNUTLS_EARLY_DATA
  якщо(peer->transport == TRNSPRT_QUIC && earlydata_max > 0)
    init_flags |= GNUTLS_ENABLE_EARLY_DATA | GNUTLS_NO_END_OF_EARLY_DATA;
  інакше якщо(earlydata_max > 0 && earlydata_max != 0xFFFFFFFFUL)
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
  якщо(!config->verifystatus)
    /* Disable the "status_request" TLS extension, enabled by default since
       GnuTLS 3.8.0. */
    init_flags |= GNUTLS_NO_STATUS_REQUEST;
#endif

  CURL_TRC_CF(data, cf, "gnutls_init(flags=%x), earlydata=%zu",
              init_flags, earlydata_max);
  rc = gnutls_init(&gtls->session, init_flags);
  якщо(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_init() failed: %d", rc);
    повернути CURLE_SSL_CONNECT_ERROR;
  }

  якщо(peer->sni) {
    якщо(gnutls_server_name_set(gtls->session, GNUTLS_NAME_DNS,
                              peer->sni, strlen(peer->sni)) < 0) {
      failf(data, "Failed to set SNI");
      повернути CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* Use default priorities */
  rc = gnutls_set_default_priority(gtls->session);
  якщо(rc != GNUTLS_E_SUCCESS)
    повернути CURLE_SSL_CONNECT_ERROR;

  /* "In GnuTLS 3.6.5, TLS 1.3 is enabled by default" */
  tls13support = !!gnutls_check_version("3.6.5");

  якщо(config->version == CURL_SSLVERSION_TLSv1_3) {
    якщо(!tls13support) {
      failf(data, "This GnuTLS installation does not support TLS 1.3");
      повернути CURLE_SSL_CONNECT_ERROR;
    }
  }

  /* At this point we know we have a supported TLS version, so set it */
  result = gnutls_set_ssl_version_min_max(data, peer,
                                          config, &prioritylist, tls13support);
  якщо(result)
    повернути result;

  result = gtls_set_priority(cf, data, gtls, prioritylist);
  якщо(result)
    повернути result;

  якщо(config->clientcert) {
    якщо(!gtls->shared_creds->trust_setup) {
      result = Curl_gtls_client_trust_setup(cf, data, gtls);
      якщо(result)
        повернути result;
    }
    якщо(config->cert_type &&
       curl_strequal(config->cert_type, "P12")) {
      rc = gnutls_certificate_set_x509_simple_pkcs12_file(
        gtls->shared_creds->creds, config->clientcert, GNUTLS_X509_FMT_DER,
        config->key_passwd ? config->key_passwd : "");
      якщо(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 potentially-encrypted key or certificate "
              "file: %s",
              gnutls_strerror(rc));
        повернути CURLE_SSL_CONNECT_ERROR;
      }
    }
    інакше {
      const unsigned ціле supported_key_encryption_algorithms =
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
      якщо(rc != GNUTLS_E_SUCCESS) {
        failf(data,
              "error reading X.509 %skey file: %s",
              config->key_passwd ? "potentially-encrypted " : "",
              gnutls_strerror(rc));
        повернути CURLE_SSL_CONNECT_ERROR;
      }
    }
  }

  rc = gnutls_credentials_set(gtls->session, GNUTLS_CRD_CERTIFICATE,
                              gtls->shared_creds->creds);
  якщо(rc != GNUTLS_E_SUCCESS) {
    failf(data, "gnutls_credentials_set() failed: %s", gnutls_strerror(rc));
    повернути CURLE_SSL_CONNECT_ERROR;
  }

  якщо(config->verifystatus) {
    rc = gnutls_ocsp_status_request_enable_client(gtls->session,
                                                  NULL, 0, NULL);
    якщо(rc != GNUTLS_E_SUCCESS) {
      failf(data, "gnutls_ocsp_status_request_enable_client() failed: %d", rc);
      повернути CURLE_SSL_CONNECT_ERROR;
    }
  }

  повернути CURLE_OK;
}

#ifdef CURL_GNUTLS_EARLY_DATA
static ціле keylog_callback(gnutls_session_t session, const char *label,
                           const gnutls_datum_t *secret)
{
  gnutls_datum_t crandom;
  gnutls_datum_t srandom;

  gnutls_session_get_random(session, &crandom, &srandom);
  якщо(crandom.size != 32) {
    повернути -1;
  }

  Curl_tls_keylog_write(label, crandom.data, crandom.size,
                        secret->data, secret->size);
  повернути 0;
}

static CURLcode gtls_on_session_reuse(struct Curl_cfilter *cf,
                                      struct Curl_easy *data,
                                      struct alpn_spec *alpns,
                                      struct Curl_ssl_session *scs,
                                      bool *do_early_data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;

  connssl->earlydata_max =
    gnutls_record_get_max_early_data_size(backend->gtls.session);

  /* Seems to be no GnuTLS way to signal no EarlyData in session */
  повернути Curl_on_session_reuse(cf, data, alpns, scs, do_early_data,
                               connssl->earlydata_max &&
                               connssl->earlydata_max != 0xFFFFFFFFUL);
}
#endif

static CURLcode gtls_apply_session(
  struct gtls_ctx *gctx,
  struct Curl_cfilter *cf,
  struct Curl_easy *data,
  struct ssl_peer *peer,
  struct alpn_spec *alpns,
  Curl_gtls_init_session_reuse_cb *sess_reuse_cb,
  struct Curl_ssl_session *scs,
  bool *pwas_setup)
{
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OK;
  ціле rc;

  якщо(scs && scs->sdata && scs->sdata_len &&
     (!scs->alpn || Curl_alpn_contains_proto(alpns, scs->alpn))) {
    /* we got a cached session, use it! */

    result = gtls_client_init(cf, data, peer, scs->earlydata_max, gctx);
    якщо(result)
      goto out;
    *pwas_setup = TRUE;

    rc = gnutls_session_set_data(gctx->session, scs->sdata, scs->sdata_len);
    якщо(rc < 0)
      infof(data, "SSL session not accepted by GnuTLS, continuing without");
    інакше {
      infof(data, "SSL reusing session with ALPN '%s'",
            scs->alpn ? scs->alpn : "-");
      якщо(conn_config->earlydata && scs->alpn &&
         !cf->conn->bits.connect_only) {
        bool do_early_data = FALSE;
        якщо(sess_reuse_cb) {
          result = sess_reuse_cb(cf, data, alpns, scs, &do_early_data);
          якщо(result)
            goto out;
        }
        якщо(do_early_data) {
          /* We only try the ALPN protocol the session used before,
           * otherwise we might send early data for the wrong protocol */
          Curl_alpn_restrict_to(alpns, scs->alpn);
        }
      }
    }
  }
out:
  повернути result;
}

CURLcode Curl_gtls_ctx_init(struct gtls_ctx *gctx,
                            struct Curl_cfilter *cf,
                            struct Curl_easy *data,
                            struct ssl_peer *peer,
                            const struct alpn_spec *alpns_requested,
                            Curl_gtls_ctx_setup_cb *cb_setup,
                            void *cb_user_data,
                            void *ssl_user_data,
                            Curl_gtls_init_session_reuse_cb *sess_reuse_cb)
{
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  struct Curl_ssl_session *scs = NULL;
  gnutls_datum_t gtls_alpns[ALPN_ENTRIES_MAX];
  size_t gtls_alpns_count = 0;
  bool gtls_session_setup = FALSE;
  struct alpn_spec alpns;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(gctx);
  Curl_alpn_copy(&alpns, alpns_requested);

  якщо((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    scs = Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    якщо(scs) {
      result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                  sess_reuse_cb, scs, &gtls_session_setup);
      scs = NULL;
      якщо(!result && gtls_session_setup) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
      }
    }
  }
  /* This might be a reconnect, so we check for a session ID in the cache
     to speed up things. We need to do this before constructing the GnuTLS
     session since we need to set flags depending on the kind of reuse. */
  якщо(!gtls_session_setup && conn_config->cache_session &&
     !conn_config->verifystatus) {
    result = Curl_ssl_scache_take(cf, data, peer->scache_key, &scs);
    якщо(result)
      goto out;
    result = gtls_apply_session(gctx, cf, data, peer, &alpns,
                                sess_reuse_cb, scs, &gtls_session_setup);
  }

  якщо(!gtls_session_setup) {
    result = gtls_client_init(cf, data, peer, 0, gctx);
    якщо(result)
      goto out;
  }

  gnutls_session_set_ptr(gctx->session, ssl_user_data);

  якщо(cb_setup) {
    result = cb_setup(cf, data, cb_user_data);
    якщо(result)
      goto out;
  }

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Open the file if a TLS or QUIC backend has not done this before. */
  Curl_tls_keylog_open();
  якщо(Curl_tls_keylog_enabled()) {
    gnutls_session_set_keylog_function(gctx->session, keylog_callback);
  }
#endif

  /* convert the ALPN string from our arguments to a list of strings that
   * GnuTLS wants and does convert internally back to this string for sending
   * to the server. nice. */
  якщо(!gtls_alpns_count && alpns.count) {
    size_t i;
    DEBUGASSERT(CURL_ARRAYSIZE(gtls_alpns) >= alpns.count);
    для(i = 0; i < alpns.count; ++i) {
      gtls_alpns[i].data = (unsigned char *)alpns.entries[i];
      gtls_alpns[i].size = (unsigned ціле)strlen(alpns.entries[i]);
    }
    gtls_alpns_count = alpns.count;
  }

  якщо(gtls_alpns_count &&
     gnutls_alpn_set_protocols(gctx->session,
                               gtls_alpns, (unsigned ціле)gtls_alpns_count,
                               GNUTLS_ALPN_MANDATORY)) {
    failf(data, "failed setting ALPN");
    result = CURLE_SSL_CONNECT_ERROR;
  }

out:
  Curl_ssl_scache_return(cf, data, peer->scache_key, scs);
  повернути result;
}

static CURLcode gtls_connect_step1(struct Curl_cfilter *cf,
                                   struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  CURLcode result;

  DEBUGASSERT(backend);

  якщо(connssl->state == ssl_connection_complete)
    /* to make us tolerant against being called more than once for the
       same connection */
    повернути CURLE_OK;

  result = Curl_gtls_ctx_init(&backend->gtls, cf, data, &connssl->peer,
                              connssl->alpn, NULL, NULL, cf,
#ifdef CURL_GNUTLS_EARLY_DATA
                              gtls_on_session_reuse
#інакше
                              NULL
#endif
                              );

  якщо(result)
    повернути result;

  якщо(connssl->alpn && (connssl->state != ssl_connection_deferred)) {
    struct alpn_proto_buf proto;
    memset(&proto, 0, sizeof(proto));
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

  повернути CURLE_OK;
}

static CURLcode pkp_pin_peer_pubkey(struct Curl_easy *data,
                                    gnutls_x509_crt_t cert,
                                    const char *pinnedpubkey)
{
  /* Scratch */
  size_t len1 = 0, len2 = 0;
  unsigned char *buff1 = NULL;

  gnutls_pubkey_t key = NULL;

  /* Result is returned to caller */
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  якщо(!pinnedpubkey)
    повернути CURLE_OK;

  якщо(!cert)
    повернути result;

  do {
    ціле ret;

    /* Begin Gyrations to get the public key */
    ret = gnutls_pubkey_init(&key);
    якщо(ret < 0)
      break; /* failed */

    ret = gnutls_pubkey_import_x509(key, cert, 0);
    якщо(ret < 0)
      break; /* failed */

    ret = gnutls_pubkey_export(key, GNUTLS_X509_FMT_DER, NULL, &len1);
    якщо(ret != GNUTLS_E_SHORT_MEMORY_BUFFER || len1 == 0)
      break; /* failed */

    buff1 = curlx_malloc(len1);
    якщо(!buff1)
      break; /* failed */

    len2 = len1;

    ret = gnutls_pubkey_export(key, GNUTLS_X509_FMT_DER, buff1, &len2);
    якщо(ret < 0 || len1 != len2)
      break; /* failed */

    /* End Gyrations */

    /* The one good exit point */
    result = Curl_pin_peer_pubkey(data, pinnedpubkey, buff1, len1);
  } поки(0);

  якщо(key)
    gnutls_pubkey_deinit(key);

  curlx_safefree(buff1);

  повернути result;
}

void Curl_gtls_report_handshake(struct Curl_easy *data, struct gtls_ctx *gctx)
{
#ifdef CURLVERBOSE
  якщо(Curl_trc_is_verbose(data)) {
    const char *ptr;
    gnutls_protocol_t version = gnutls_protocol_get_version(gctx->session);

    /* the name of the cipher suite used, e.g. ECDHE_RSA_AES_256_GCM_SHA384. */
    ptr = gnutls_cipher_suite_get_name(gnutls_kx_get(gctx->session),
                                       gnutls_cipher_get(gctx->session),
                                       gnutls_mac_get(gctx->session));

    infof(data, "SSL connection using %s / %s",
          gnutls_protocol_get_name(version), ptr);
  }
#інакше
  (void)data;
  (void)gctx;
#endif
}

static void gtls_msg_verify_result(struct Curl_easy *data,
                                   struct ssl_peer *peer,
                                   gnutls_x509_crt_t x509_cert,
                                   bool was_verified,
                                   bool needs_verified)
{
  char certname[65] = ""; /* limited to 64 chars by ASN.1 */
  size_t size = sizeof(certname);
  ціле rc;

  rc = gnutls_x509_crt_get_dn_by_oid(x509_cert, GNUTLS_OID_X520_COMMON_NAME,
                                     0, /* the first and only one */
                                     FALSE, certname, &size);
  якщо(rc) {
    infof(data, "error fetching CN from cert:%s", gnutls_strerror(rc));
    certname[0] = 0;
  }

  якщо(!was_verified) {
    якщо(needs_verified) {
      failf(data, "SSL: certificate subject name (%s) does not match "
            "target hostname '%s'", certname,
            peer->origin->user_hostname);
    }
    інакше
      infof(data, "  common name: %s (does not match '%s')",
            certname, peer->origin->user_hostname);
  }
  інакше
    infof(data, "  common name: %s (matched)", certname);
}

static void gtls_infof_cert(struct Curl_easy *data,
                            gnutls_x509_crt_t x509_cert)
{
#ifdef CURLVERBOSE
  якщо(Curl_trc_is_verbose(data)) {
    gnutls_datum_t certfields;
    ціле rc, algo;
    time_t tstamp;
    unsigned ціле bits;

    /* public key algorithm's parameters */
    algo = gnutls_x509_crt_get_pk_algorithm(x509_cert, &bits);
    infof(data, "  certificate public key: %s",
          gnutls_pk_algorithm_get_name((gnutls_pk_algorithm_t)algo));

    /* version of the X.509 certificate. */
    infof(data, "  certificate version: #%d",
          gnutls_x509_crt_get_version(x509_cert));

    rc = gnutls_x509_crt_get_dn2(x509_cert, &certfields);
    якщо(rc)
      infof(data, "Failed to get certificate name");
    інакше {
      infof(data, "  subject: %s", certfields.data);

      tstamp = gnutls_x509_crt_get_activation_time(x509_cert);
      showtime(data, "start date", tstamp);

      tstamp = gnutls_x509_crt_get_expiration_time(x509_cert);
      showtime(data, "expire date", tstamp);

      gnutls_free(certfields.data);
    }

    rc = gnutls_x509_crt_get_issuer_dn2(x509_cert, &certfields);
    якщо(rc)
      infof(data, "Failed to get certificate issuer");
    інакше {
      infof(data, "  issuer: %s", certfields.data);

      gnutls_free(certfields.data);
    }
  }
#інакше
  (void)data;
  (void)x509_cert;
#endif
}

static CURLcode gtls_verify_ocsp_status(struct Curl_easy *data,
                                        gnutls_session_t session,
                                        gnutls_x509_crt_t cert)
{
  gnutls_datum_t ocsp_raw;
  gnutls_ocsp_resp_t ocsp_resp = NULL;
  CURLcode result = CURLE_SSL_INVALIDCERTSTATUS;
  ціле rc, idx;
  bool ocsp_init = FALSE;
  gnutls_ocsp_cert_status_t status = GNUTLS_OCSP_CERT_UNKNOWN;

  /* Get the OCSP stapled response. We require it to be there. */
  rc = gnutls_ocsp_status_request_get(session, &ocsp_raw);
  якщо(rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE) {
    failf(data, "No OCSP response received");
    goto out;
  }
  інакше якщо(rc < 0) {
    failf(data, "Invalid OCSP response received");
    goto out;
  }
  /* Parse the OCSP response, which needs to succeed. */
  rc = gnutls_ocsp_resp_init(&ocsp_resp);
  якщо(rc < 0) {
    failf(data, "failed to init OCSP response parsing");
    goto out;
  }
  ocsp_init = TRUE;
  rc = gnutls_ocsp_resp_import(ocsp_resp, &ocsp_raw);
  якщо(rc < 0) {
    failf(data, "failed to parse OCSP response");
    goto out;
  }
  /* OCSP responses carry a basic, overall status and then individual
   * parts for different certificates. The overall status must be good. */
  якщо(gnutls_ocsp_resp_get_status(ocsp_resp) != GNUTLS_OCSP_RESP_SUCCESSFUL) {
    failf(data, "OCSP response basic part is not successful");
    goto out;
  }
  /* Find the OCSP sub-response for the peer certificate. */
  для(idx = 0;; idx++) {
    rc = gnutls_ocsp_resp_check_crt(ocsp_resp, idx, cert);
    якщо(!rc || (rc == GNUTLS_E_REQUESTED_DATA_NOT_AVAILABLE))
      break;
  }
  якщо(rc < 0) {
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
  якщо(rc < 0) {
    failf(data, "failed to parse OCSP response for peer cert");
    goto out;
  }
  switch(status) {
  case GNUTLS_OCSP_CERT_GOOD:
    /* We only regard this as OK and fail all other status codes. */
    result = CURLE_OK;
    break;
  case GNUTLS_OCSP_CERT_REVOKED:
    failf(data, "peer certificate is revoked");
    break;
  default:
    failf(data, "peer certificate OCSP status is not good: %d", (ціле)status);
    break;
  }

out:
  якщо(ocsp_init)
    gnutls_ocsp_resp_deinit(ocsp_resp);
  повернути result;
}

struct gtls_cert_chain {
  const gnutls_datum_t *certs;
  unsigned ціле num_certs;
};

#ifdef USE_APPLE_SECTRUST
static CURLcode gtls_chain_get_der(struct Curl_cfilter *cf,
                                   struct Curl_easy *data,
                                   void *user_data,
                                   size_t i,
                                   unsigned char **pder,
                                   size_t *pder_len)
{
  struct gtls_cert_chain *chain = user_data;

  (void)cf;
  (void)data;
  *pder_len = 0;
  *pder = NULL;

  якщо(i >= chain->num_certs)
    повернути CURLE_TOO_LARGE;
  *pder = chain->certs[i].data;
  *pder_len = (size_t)chain->certs[i].size;
  повернути CURLE_OK;
}
#endif /* USE_APPLE_SECTRUST */

/* This function verifies the peer's certificate and returns CURLE_OK on
   success or an appropriate CURLcode on error. The certificate verification
   status bitmask (trusted, invalid etc.) is stored in
   ssl_config->certverifyresult as one or more gnutls_certificate_status_t
   enumerated elements bitwise or'd. */
static CURLcode gtls_verify_cert(struct Curl_easy *data,
                                 struct ssl_filter_config *config,
                                 struct ssl_easy_config *ssl_config,
                                 gnutls_session_t session,
                                 struct Curl_cfilter *cf,
                                 struct ssl_peer *peer,
                                 struct gtls_cert_chain *chain,
                                 bool *pocsp_checked)
{
  bool verified = FALSE;
  unsigned ціле verify_status = 0;
  long * const certverifyresult = &ssl_config->certverifyresult;
  ціле rc;

  *pocsp_checked = FALSE;
  rc = gnutls_certificate_verify_peers2(session, &verify_status);
  якщо(rc < 0) {
    failf(data, "server cert verify failed: %d", rc);
    *certverifyresult = rc;
    повернути CURLE_SSL_CONNECT_ERROR;
  }
  *certverifyresult = verify_status;
  verified = !(verify_status & GNUTLS_CERT_INVALID);
  якщо(verified)
    infof(data, "  SSL certificate verified by GnuTLS");

#ifdef USE_APPLE_SECTRUST
  якщо(!verified && config->native_ca_store) {
    gnutls_datum_t ocsp_raw;
    CURLcode result;

    якщо(gnutls_ocsp_status_request_get(session, &ocsp_raw)) {
      ocsp_raw.data = NULL;
      ocsp_raw.size = 0;
    }
    якщо(config->verifystatus && !ocsp_raw.size) {
      failf(data, "No OCSP response received");
      повернути CURLE_SSL_INVALIDCERTSTATUS;
    }
    result = Curl_vtls_apple_verify(cf, data, peer, chain->num_certs,
                                    gtls_chain_get_der, chain,
                                    ocsp_raw.data, ocsp_raw.size,
                                    config->verifystatus);
    якщо(result && (result != CURLE_PEER_FAILED_VERIFICATION))
      повернути result; /* no further failure analysis needed */
    verified = !result;
    якщо(verified) {
      infof(data, "SSL certificate verified via Apple SecTrust.");
      *certverifyresult = 0;
      *pocsp_checked = TRUE;
    }
  }
#інакше
  (void)cf;
  (void)peer;
  (void)chain;
#endif

  якщо(!verified) {
    CURLcode result = CURLE_PEER_FAILED_VERIFICATION;
    /* verify_status is a bitmask of gnutls_certificate_status bits */
    const char *cause = "certificate error, no details available";
    якщо(verify_status & GNUTLS_CERT_EXPIRED)
      cause = "certificate has expired";
    інакше якщо(verify_status & GNUTLS_CERT_SIGNER_NOT_FOUND)
      cause = "certificate signer not trusted";
    інакше якщо(verify_status & GNUTLS_CERT_INSECURE_ALGORITHM)
      cause = "certificate uses insecure algorithm";
    інакше якщо(verify_status & GNUTLS_CERT_INVALID_OCSP_STATUS) {
      cause = "OCSP status response is invalid";
      result = CURLE_SSL_INVALIDCERTSTATUS;
    }
    failf(data, "SSL certificate verification failed: %s. (CAfile: %s "
          "CRLfile: %s)", cause,
          config->CAfile ? config->CAfile : "none",
          config->CRLfile ?
          config->CRLfile : "none");

    повернути result;
  }
  повернути CURLE_OK;
}

CURLcode Curl_gtls_verifyserver(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                gnutls_session_t session,
                                struct ssl_filter_config *config,
                                struct ssl_easy_config *ssl_config,
                                struct ssl_peer *peer,
                                const char *pinned_key)
{
  struct gtls_cert_chain chain;
  gnutls_x509_crt_t x509_cert = NULL, x509_issuer = NULL;
  time_t certclock;
  bool ocsp_checked = FALSE;
  ціле rc;
  CURLcode result = CURLE_OK;
  long * const certverifyresult = &ssl_config->certverifyresult;

  (void)cf;
  /* This function returns the peer's raw certificate (chain) as sent by
     the peer. These certificates are in raw format (DER encoded for
     X.509). In case of a X.509 then a certificate list may be present. The
     first certificate in the list is the peer's certificate, following the
     issuer's certificate, then the issuer's issuer etc. */

  chain.certs = gnutls_certificate_get_peers(session, &chain.num_certs);
  якщо(!chain.certs) {
    якщо(config->verifypeer ||
       config->verifyhost ||
       config->issuercert) {
      failf(data, "failed to get server cert");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_PEER_FAILED_VERIFICATION;
      goto out;
    }
    infof(data, " common name: WARNING could not obtain");
  }

  якщо(data->set.ssl.certinfo && chain.certs) {
    якщо(chain.num_certs > MAX_ALLOWED_CERT_AMOUNT) {
      failf(data, "%u certificates is more than allowed (%d)",
            chain.num_certs, MAX_ALLOWED_CERT_AMOUNT);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    інакше {
      unsigned ціле i;

      result = Curl_ssl_init_certinfo(data, (ціле)chain.num_certs);
      якщо(result)
        goto out;

      для(i = 0; i < chain.num_certs; i++) {
        const uint8_t *beg = chain.certs[i].data;
        const uint8_t *end = beg + chain.certs[i].size;

        result = Curl_extract_certinfo(data, (ціле)i, beg, end);
        якщо(result)
          goto out;
      }
    }
  }

  якщо(config->verifypeer) {
    result = gtls_verify_cert(data, config, ssl_config, session,
                              cf, peer, &chain, &ocsp_checked);
    якщо(result)
      goto out;
  }
  інакше
    infof(data, "  SSL certificate verification SKIPPED");

  /* initialize an X.509 certificate structure. */
  якщо(gnutls_x509_crt_init(&x509_cert)) {
    failf(data, "failed to init GnuTLS x509_crt");
    *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
    result = CURLE_SSL_CONNECT_ERROR;
    goto out;
  }

  якщо(chain.certs) {
    /* convert the given DER or PEM encoded Certificate to the native
       gnutls_x509_crt_t format */
    rc = gnutls_x509_crt_import(x509_cert, chain.certs, GNUTLS_X509_FMT_DER);
    якщо(rc) {
      failf(data, "error parsing server's certificate chain");
      *certverifyresult = GNUTLS_E_NO_CERTIFICATE_FOUND;
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
  }

  /* Check for time-based validity */
  certclock = gnutls_x509_crt_get_expiration_time(x509_cert);

  якщо(certclock == (time_t)-1) {
    якщо(config->verifypeer) {
      failf(data, "server cert expiration date verify failed");
      *certverifyresult = GNUTLS_CERT_EXPIRED;
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    інакше
      infof(data, "  SSL certificate expiration date verify FAILED");
  }
  інакше {
    якщо(certclock < time(NULL)) {
      якщо(config->verifypeer) {
        failf(data, "server certificate expiration date has passed.");
        *certverifyresult = GNUTLS_CERT_EXPIRED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        goto out;
      }
      інакше
        infof(data, "  SSL certificate expiration date FAILED");
    }
    інакше
      infof(data, "  SSL certificate expiration date OK");
  }

  certclock = gnutls_x509_crt_get_activation_time(x509_cert);

  якщо(certclock == (time_t)-1) {
    якщо(config->verifypeer) {
      failf(data, "server cert activation date verify failed");
      *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    інакше
      infof(data, "  SSL certificate activation date verify FAILED");
  }
  інакше {
    якщо(certclock > time(NULL)) {
      якщо(config->verifypeer) {
        failf(data, "server certificate not activated yet.");
        *certverifyresult = GNUTLS_CERT_NOT_ACTIVATED;
        result = CURLE_PEER_FAILED_VERIFICATION;
        goto out;
      }
      інакше
        infof(data, "  SSL certificate activation date FAILED");
    }
    інакше
      infof(data, "  SSL certificate activation date OK");
  }

  якщо(config->verifystatus && !ocsp_checked) {
    result = gtls_verify_ocsp_status(data, session, x509_cert);
    якщо(result)
      goto out;
  }
  інакше
    infof(data, "  SSL certificate status verification SKIPPED");

  якщо(config->issuercert) {
    gnutls_datum_t issuerp;
    якщо(gnutls_x509_crt_init(&x509_issuer)) {
      failf(data, "failed to init GnuTLS x509_crt for issuer");
      result = CURLE_SSL_ISSUER_ERROR;
      goto out;
    }
    issuerp = load_file(config->issuercert);
    rc = gnutls_x509_crt_import(x509_issuer, &issuerp, GNUTLS_X509_FMT_PEM);
    unload_file(issuerp);
    якщо(rc) {
      failf(data, "failed to import issuer certificate (%s) (Issuer Cert: %s)",
            gnutls_strerror(rc), config->issuercert);
      result = CURLE_SSL_ISSUER_ERROR;
      goto out;
    }
    rc = (ціле)gnutls_x509_crt_check_issuer(x509_cert, x509_issuer);
    якщо(rc <= 0) {
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
  rc = (ціле)gnutls_x509_crt_check_hostname(x509_cert,
                                           peer->sni ? peer->sni :
                                           peer->origin->hostname);
  result = (!rc && config->verifyhost) ?
    CURLE_PEER_FAILED_VERIFICATION : CURLE_OK;
  gtls_msg_verify_result(data, peer, x509_cert, rc, config->verifyhost);
  якщо(result)
    goto out;

  якщо(pinned_key) {
    result = pkp_pin_peer_pubkey(data, x509_cert, pinned_key);
    якщо(result != CURLE_OK) {
      failf(data, "SSL: public key does not match pinned public key");
      goto out;
    }
  }

  gtls_infof_cert(data, x509_cert);

out:
  якщо(x509_issuer)
    gnutls_x509_crt_deinit(x509_issuer);
  якщо(x509_cert)
    gnutls_x509_crt_deinit(x509_cert);
  повернути result;
}

static CURLcode gtls_verifyserver(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  gnutls_session_t session)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  struct ssl_easy_config *ssl_config = Curl_ssl_cf_get_easy_config(cf, data);
#ifndef CURL_DISABLE_PROXY
  const char *pinned_key = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#інакше
  const char *pinned_key = CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
  CURLcode result;

  result = Curl_gtls_verifyserver(cf, data, session, conn_config, ssl_config,
                                  &connssl->peer, pinned_key);
  якщо(result)
    goto out;

#ifdef CURL_GNUTLS_EARLY_DATA
  /* Only on TLSv1.2 or lower do we have the session id now. For
   * TLSv1.3 we get it via a SESSION_TICKET message that arrives later. */
  якщо(gnutls_protocol_get_version(session) < GNUTLS_TLS1_3)
    result = cf_gtls_update_session_id(cf, data, session);
#endif

out:
  повернути result;
}

#ifdef CURL_GNUTLS_EARLY_DATA
static CURLcode gtls_send_earlydata(struct Curl_cfilter *cf,
                                    struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  const unsigned char *buf;
  size_t blen;
  ssize_t n;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_sending);
  backend->gtls.io_result = CURLE_OK;
  поки(Curl_bufq_peek(&connssl->earlydata, &buf, &blen)) {
    n = gnutls_record_send_early_data(backend->gtls.session, buf, blen);
    CURL_TRC_CF(data, cf, "gtls_send_earlydata(len=%zu) -> %zd", blen, n);
    якщо(n < 0) {
      якщо(n == GNUTLS_E_AGAIN)
        result = CURLE_AGAIN;
      інакше
        result = backend->gtls.io_result ?
                 backend->gtls.io_result : CURLE_SEND_ERROR;
      goto out;
    }
    інакше якщо(!n) {
      /* GnuTLS is buggy, it *SHOULD* return the amount of bytes it took in.
       * Instead it returns 0 if everything was written. */
      n = (ssize_t)blen;
    }

    Curl_bufq_skip(&connssl->earlydata, (size_t)n);
  }
  /* sent everything there was */
  infof(data, "SSL sending %zu bytes of early data", connssl->earlydata_skip);
out:
  повернути result;
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
static CURLcode gtls_connect_common(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    bool *done)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(backend);
  /* check if the connection has already been established */
  якщо(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    повернути CURLE_OK;
  }

  *done = FALSE;

  /* Initiate the connection, if not already done */
  якщо(connssl->connecting_state == ssl_connect_1) {
    result = gtls_connect_step1(cf, data);
    якщо(result)
      goto out;
    connssl->connecting_state = ssl_connect_2;
  }

  якщо(connssl->connecting_state == ssl_connect_2) {
#ifdef CURL_GNUTLS_EARLY_DATA
    якщо(connssl->earlydata_state == ssl_earlydata_await) {
      goto out;
    }
    інакше якщо(connssl->earlydata_state == ssl_earlydata_sending) {
      result = gtls_send_earlydata(cf, data);
      якщо(result)
        goto out;
      connssl->earlydata_state = ssl_earlydata_sent;
    }
    DEBUGASSERT((connssl->earlydata_state == ssl_earlydata_none) ||
                (connssl->earlydata_state == ssl_earlydata_sent));
#endif
    result = cf_gtls_handshake(cf, data);
    якщо(result)
      goto out;
    connssl->connecting_state = ssl_connect_3;
  }

  /* Finish connecting once the handshake is done */
  якщо(connssl->connecting_state == ssl_connect_3) {
    gnutls_datum_t proto;
    ціле rc;

    Curl_gtls_report_handshake(data, &backend->gtls);

    result = gtls_verifyserver(cf, data, backend->gtls.session);
    якщо(result)
      goto out;

    connssl->state = ssl_connection_complete;

    rc = gnutls_alpn_get_selected_protocol(backend->gtls.session, &proto);
    якщо(rc) {  /* No ALPN from server */
      proto.data = NULL;
      proto.size = 0;
    }

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      proto.data, proto.size);
    якщо(result)
      goto out;

#ifdef CURL_GNUTLS_EARLY_DATA
    якщо(connssl->earlydata_state > ssl_earlydata_none) {
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

  якщо(connssl->connecting_state == ssl_connect_done)
    DEBUGASSERT(connssl->state == ssl_connection_complete);

out:
  якщо(result == CURLE_AGAIN) {
    *done = FALSE;
    повернути CURLE_OK;
  }
  *done = ((connssl->state == ssl_connection_complete) ||
           (connssl->state == ssl_connection_deferred));
  CURL_TRC_CF(data, cf, "gtls_connect_common() -> %d, done=%d", (ціле)result,
              *done);
  повернути result;
}

static CURLcode gtls_connect(struct Curl_cfilter *cf,
                             struct Curl_easy *data,
                             bool *done)
{
#ifdef CURL_GNUTLS_EARLY_DATA
  struct ssl_connect_data *connssl = cf->ctx;
  якщо((connssl->state == ssl_connection_deferred) &&
     (connssl->earlydata_state == ssl_earlydata_await)) {
    /* We refuse to be pushed, we are waiting for someone to send/recv. */
    *done = TRUE;
    повернути CURLE_OK;
  }
#endif
  повернути gtls_connect_common(cf, data, done);
}

static bool gtls_data_pending(struct Curl_cfilter *cf,
                              const struct Curl_easy *data)
{
  struct ssl_connect_data *ctx = cf->ctx;
  struct gtls_ssl_backend_data *backend;

  (void)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (struct gtls_ssl_backend_data *)ctx->backend;
  якщо(backend->gtls.session &&
     gnutls_record_check_pending(backend->gtls.session) != 0)
    повернути TRUE;
  повернути FALSE;
}

static CURLcode gtls_send(struct Curl_cfilter *cf,
                          struct Curl_easy *data,
                          const void *buf,
                          size_t blen,
                          size_t *pnwritten)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nwritten;
  size_t remain = blen;

  DEBUGASSERT(backend);
  *pnwritten = 0;

  поки(remain) {
    backend->gtls.io_result = CURLE_OK;
    nwritten = gnutls_record_send(backend->gtls.session, buf, remain);

    якщо(nwritten >= 0) {
      якщо((size_t)nwritten > remain) {
        DEBUGASSERT(0);
        failf(data, "gnutls_record_send(len=%zu) returned %zd",
              remain, nwritten);
        повернути CURLE_SEND_ERROR;
      }
      *pnwritten += (size_t)nwritten;
      buf = (char *)CURL_UNCONST(buf) + (size_t)nwritten;
      remain -= (size_t)nwritten;
    }
    інакше {
      якщо(*pnwritten && (nwritten == GNUTLS_E_AGAIN)) {
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
              blen, (ціле)result, *pnwritten);
  повернути result;
}

/*
 * This function is called to shut down the SSL layer but keep the
 * socket open (CCC - Clear Command Channel)
 */
static CURLcode gtls_shutdown(struct Curl_cfilter *cf,
                              struct Curl_easy *data,
                              bool send_shutdown, bool *done)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  char buf[1024];
  CURLcode result = CURLE_OK;
  ssize_t nread = 0;
  size_t i;

  DEBUGASSERT(backend);
  /* If we have no handshaked connection or already shut down */
  якщо(!backend->gtls.session || cf->shutdown ||
     connssl->state != ssl_connection_complete) {
    *done = TRUE;
    goto out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  якщо(!backend->gtls.sent_shutdown) {
    /* do this only once */
    backend->gtls.sent_shutdown = TRUE;
    якщо(send_shutdown) {
      ціле ret = gnutls_bye(backend->gtls.session, GNUTLS_SHUT_RDWR);
      якщо((ret == GNUTLS_E_AGAIN) || (ret == GNUTLS_E_INTERRUPTED)) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye EAGAIN");
        connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
          CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
        backend->gtls.sent_shutdown = FALSE;
        result = CURLE_OK;
        goto out;
      }
      якщо(ret != GNUTLS_E_SUCCESS) {
        CURL_TRC_CF(data, cf, "SSL shutdown, gnutls_bye error: '%s'(%d)",
                    gnutls_strerror((ціле)ret), (ціле)ret);
        result = CURLE_RECV_ERROR;
        goto out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  для(i = 0; i < 10; ++i) {
    nread = gnutls_record_recv(backend->gtls.session, buf, sizeof(buf));
    якщо(nread <= 0)
      break;
  }
  якщо(nread > 0) {
    /* still data coming in? */
  }
  інакше якщо(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }
  інакше якщо((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
    connssl->io_need = gnutls_record_get_direction(backend->gtls.session) ?
      CURL_SSL_IO_NEED_SEND : CURL_SSL_IO_NEED_RECV;
  }
  інакше {
    CURL_TRC_CF(data, cf, "SSL shutdown, error: '%s'(%d)",
                gnutls_strerror((ціле)nread), (ціле)nread);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  повернути result;
}

static void gtls_close(struct Curl_cfilter *cf,
                       struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;

  DEBUGASSERT(backend);
  CURL_TRC_CF(data, cf, "close");
  якщо(backend->gtls.session) {
    gnutls_deinit(backend->gtls.session);
    backend->gtls.session = NULL;
  }
  якщо(backend->gtls.shared_creds) {
    Curl_gtls_shared_creds_free(&backend->gtls.shared_creds);
  }
}

static CURLcode gtls_recv(struct Curl_cfilter *cf,
                          struct Curl_easy *data,
                          char *buf, size_t blen,
                          size_t *pnread)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ssize_t nread;

  DEBUGASSERT(backend);

  nread = gnutls_record_recv(backend->gtls.session, buf, blen);

  якщо(nread >= 0)
    *pnread = (size_t)nread;
  інакше {
    якщо((nread == GNUTLS_E_AGAIN) || (nread == GNUTLS_E_INTERRUPTED)) {
      result = CURLE_AGAIN;
      goto out;
    }
    інакше якщо(nread == GNUTLS_E_REHANDSHAKE) {
      /* Either TLSv1.2 renegotiate or a TLSv1.3 session key update. */
      result = cf_gtls_handshake(cf, data);
      якщо(!result)
        result = CURLE_AGAIN; /* make us get called again. */
      goto out;
    }
    інакше {
      failf(data, "GnuTLS recv error (%d): %s",
            (ціле)nread, gnutls_strerror((ціле)nread));
      result = backend->gtls.io_result ?
        backend->gtls.io_result : CURLE_RECV_ERROR;
      goto out;
    }
  }

out:
  CURL_TRC_CF(data, cf, "gtls_recv(len=%zu) -> %d, %zd", blen,
              (ціле)result, nread);
  повернути result;
}

size_t Curl_gtls_version(char *buffer, size_t size)
{
  повернути curl_msnprintf(buffer, size, "GnuTLS/%s", gnutls_check_version(NULL));
}

/* data might be NULL! */
static CURLcode gtls_random(struct Curl_easy *data,
                            unsigned char *entropy, size_t length)
{
  ціле rc;
  (void)data;
  rc = gnutls_rnd(GNUTLS_RND_RANDOM, entropy, length);
  повернути rc ? CURLE_FAILED_INIT : CURLE_OK;
}

static CURLcode gtls_sha256sum(const unsigned char *input,
                               size_t len,
                               unsigned char *sha256sum, /* output */
                               size_t sha256len)
{
  struct sha256_ctx SHA256pw;
  sha256_init(&SHA256pw);
  do {
    unsigned ціле ilen = (unsigned ціле)CURLMIN(len, UINT_MAX);
    sha256_update(&SHA256pw, ilen, input);
    len -= ilen;
    input += ilen;
  } поки(len);
#якщо NETTLE_VERSION_MAJOR >= 4
  (void)sha256len;
  sha256_digest(&SHA256pw, sha256sum);
#інакше
  sha256_digest(&SHA256pw, (unsigned ціле)sha256len, sha256sum);
#endif
  повернути CURLE_OK;
}

static bool gtls_cert_status_request(void)
{
  повернути TRUE;
}

static void *gtls_get_internals(struct ssl_connect_data *connssl,
                                CURLINFO info)
{
  struct gtls_ssl_backend_data *backend =
    (struct gtls_ssl_backend_data *)connssl->backend;
  (void)info;
  DEBUGASSERT(backend);
  повернути backend->gtls.session;
}

const struct Curl_ssl Curl_ssl_gnutls = {
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

  sizeof(struct gtls_ssl_backend_data),

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
