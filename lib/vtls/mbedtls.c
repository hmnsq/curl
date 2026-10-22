/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 * Copyright (C) Hoi-Ho Chan, <hoiho.chan@gmail.com>
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
 * Source file for all mbedTLS-specific code for the TLS/SSL layer. No code
 * but vtls.c should ever call or use these functions.
 *
 */
#include "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#include <mbedtls/version.h>
#만약 MBEDTLS_VERSION_NUMBER < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#include <psa/crypto_config.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509.h>
#include <mbedtls/psa_util.h>

#만약 MBEDTLS_VERSION_NUMBER < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#include <mbedtls/error.h>
#만약 MBEDTLS_VERSION_NUMBER < 0x04000000
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#include <mbedtls/debug.h>
#endif

#include "vtls/cipher_suite.h"
#include "urldata.h"
#include "curl_trc.h"
#include "vtls/mbedtls.h"
#include "vtls/vtls.h"
#include "vtls/vtls_int.h"
#include "vtls/vtls_scache.h"
#include "vtls/x509asn1.h"
#include "connect.h" /* for the connect timeout */
#include "curlx/strdup.h"
#include "curl_sha256.h"

/* ALPN for http2 */
#만약 defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

struct mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  const char *protocols[3];
#endif
  정수 *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#만약 MBEDTLS_VERSION_NUMBER < 0x04000000
struct rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

static struct rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
static void mbed_debug(void *context, 정수 level, const char *f_name,
                       정수 line_nb, const char *line)
{
  struct Curl_easy *data = (struct Curl_easy *)context;
  (void)level;
  (void)line_nb;
  (void)f_name;

  만약(data) {
    size_t len = strlen(line);
    만약(len && (line[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (정수)len, line);
  }
}
#endif

static 정수 mbedtls_bio_cf_write(void *bio,
                                const unsigned char *buf, size_t blen)
{
  struct Curl_cfilter *cf = bio;
  struct Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  만약(!data)
    반환 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (정수)result, nwritten);
  만약(result == CURLE_AGAIN)
    반환 MBEDTLS_ERR_SSL_WANT_WRITE;
  반환 result ? -1 : (정수)nwritten;
}

static 정수 mbedtls_bio_cf_read(void *bio, unsigned char *buf, size_t blen)
{
  struct Curl_cfilter *cf = bio;
  struct Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  만약(!data)
    반환 0;
  /* OpenSSL catches this case, so should we. */
  만약(!buf)
    반환 0;

  result = Curl_conn_cf_recv(cf->next, data, (char *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (정수)result, nread);
  만약(result == CURLE_AGAIN)
    반환 MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  반환 result ? -1 : (정수)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

static CURLcode mbed_set_ssl_version_min_max(
  struct Curl_easy *data,
  struct mbed_ssl_backend_data *backend,
  struct ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#아니면
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#아니면
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  switch(conn_config->version) {
  case CURL_SSLVERSION_TLSv1:
  case CURL_SSLVERSION_TLSv1_0:
  case CURL_SSLVERSION_TLSv1_1:
  case CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    break;
#endif
  case CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    break;
#endif
  default:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    반환 CURLE_SSL_CONNECT_ERROR;
  }

  switch(conn_config->version_max) {
  case CURL_SSLVERSION_MAX_DEFAULT:
  case CURL_SSLVERSION_MAX_NONE:
  case CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    break;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  case CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    break;
#endif
  case CURL_SSLVERSION_MAX_TLSv1_1:
  case CURL_SSLVERSION_MAX_TLSv1_0:
  default:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    반환 CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  반환 CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
static 정수 mbed_cipher_suite_get_str(uint16_t id, char *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  만약(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  아니면
    반환 Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  반환 0;
}

static uint16_t mbed_cipher_suite_walk_str(const char **str, const char **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  static const char ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  만약(!id) {
    만약((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  반환 id;
}
#아니면
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

static CURLcode mbed_set_selected_ciphers(
  struct Curl_easy *data,
  struct mbed_ssl_backend_data *backend,
  const char *ciphers12,
  const char *ciphers13)
{
  const char *ciphers = ciphers12;
  const 정수 *supported;
  정수 *selected;
  size_t supported_len, count = 0, default13_count = 0, i, j;
  const char *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  위해(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(sizeof(정수) * (supported_len + 1));
  만약(!selected)
    반환 CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (void)ciphers13, (void)j;
#아니면
  만약(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    위해(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      만약(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        continue;

      selected[count++] = id;
    }

    default13_count = count;
  }
  아니면
    ciphers = ciphers13;

add_ciphers:
#endif
  위해(ptr = ciphers; ptr[0] != '\0' && count < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    만약(id) {
      위해(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      만약(i == supported_len)
        id = 0;
    }
    만약(!id) {
      만약(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (정수)(end - ptr), ptr);
      continue;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    위해(i = 0; i < count && selected[i] != id; i++)
      ;
    만약(i < count) {
      만약(i >= default13_count)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (정수)(end - ptr), ptr);
      continue;
    }

    selected[count++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  만약(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    goto add_ciphers;
  }

  만약(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    위해(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      만약(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        continue;

      /* No duplicates allowed (so selected cannot overflow) */
      위해(i = 0; i < count && selected[i] != id; i++)
        ;
      만약(i < count)
        continue;

      selected[count++] = id;
    }
  }
#endif

  selected[count] = 0;

  만약(count == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    반환 CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  반환 CURLE_OK;
}

static void mbed_dump_cert_info(struct Curl_easy *data,
                                const mbedtls_x509_crt *crt)
{
#만약 !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (void)data, (void)crt;
#아니면
  const size_t bufsize = 16384;
  char *p, *buffer = curlx_malloc(bufsize);

  만약(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    위해(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (정수)s, p);
      p += s;
    }
  }
  아니면
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

static void mbed_extract_certinfo(struct Curl_easy *data,
                                  const mbedtls_x509_crt *crt)
{
  CURLcode result;
  const mbedtls_x509_crt *cur;
  정수 cert_count = 0;
  정수 i;

  위해(cur = crt; cur && cert_count <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_count++;

  만약(cert_count > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    반환;
  }

  result = Curl_ssl_init_certinfo(data, cert_count);

  위해(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

static 정수 mbed_verify_cb(void *ptr, mbedtls_x509_crt *crt,
                          정수 depth, uint32_t *flags)
{
  struct Curl_cfilter *cf = (struct Curl_cfilter *)ptr;
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  struct Curl_easy *data = CF_DATA_CURRENT(cf);

  만약(depth == 0) {
    만약(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    만약(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  만약(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  만약(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  만약(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    char buf[128];
    mbedtls_x509_crt_verify_info(buf, sizeof(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#아니면
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned 정수)*flags);
#endif
  }

  반환 0;
}

static CURLcode mbed_load_cacert(struct Curl_cfilter *cf,
                                 struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  const struct curl_blob *ca_info_blob = conn_config->ca_info_blob;
  const char * const ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? NULL : conn_config->CAfile);
  const bool verifypeer = conn_config->verifypeer;
  const char * const ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  const char * const ssl_cert_type = conn_config->cert_type;
#endif
  정수 ret = -1;
  char errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  만약(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    만약((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((char *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    아니면 { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned char *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      만약(!newblob)
        반환 CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#아니면
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    만약(ret < 0) {
      mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned 정수)-ret, errorbuf);
      반환 CURLE_SSL_CERTPROBLEM;
    }
  }

  만약(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    만약(ret < 0) {
      mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned 정수)-ret, errorbuf);
      반환 CURLE_SSL_CACERT_BADFILE;
    }
#아니면
    failf(data, "mbedTLS: functions that use the file system not built in");
    반환 CURLE_NOT_BUILT_IN;
#endif
  }

  만약(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    만약(ret < 0) {
      mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned 정수)-ret, errorbuf);

      만약(verifypeer)
        반환 CURLE_SSL_CACERT_BADFILE;
    }
#아니면
    failf(data, "mbedTLS: functions that use the file system not built in");
    반환 CURLE_NOT_BUILT_IN;
#endif
  }

  반환 CURLE_OK;
}

static CURLcode mbed_load_clicert(struct Curl_cfilter *cf,
                                  struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  char * const ssl_cert = conn_config->clientcert;
  const struct curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  const char * const ssl_cert_type = conn_config->cert_type;
#endif
  정수 ret = -1;
  char errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  만약(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    만약(ret) {
      mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned 정수)-ret, errorbuf);

      반환 CURLE_SSL_CERTPROBLEM;
    }
#아니면
    failf(data, "mbedTLS: functions that use the file system not built in");
    반환 CURLE_NOT_BUILT_IN;
#endif
  }

  만약(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    만약((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((char *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    아니면 { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned char *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      만약(!newblob)
        반환 CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#아니면
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    만약(ret) {
      mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned 정수)-ret, errorbuf);
      반환 CURLE_SSL_CERTPROBLEM;
    }
  }

  반환 CURLE_OK;
}

static CURLcode mbed_load_privkey(struct Curl_cfilter *cf,
                                  struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  정수 ret = -1;
  char errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  만약(conn_config->key || conn_config->key_blob) {
    만약(conn_config->key) {
#ifdef MBEDTLS_FS_IO
#만약 MBEDTLS_VERSION_NUMBER >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->key,
                                     conn_config->key_passwd);
      만약(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_KEY_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_KEY_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#아니면
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->key,
                                     conn_config->key_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      만약(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      만약(ret) {
        mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->key, (unsigned 정수)-ret, errorbuf);
        반환 CURLE_SSL_CERTPROBLEM;
      }
#아니면
      failf(data, "mbedTLS: functions that use the file system not built in");
      반환 CURLE_NOT_BUILT_IN;
#endif
    }
    아니면 {
      const struct curl_blob *ssl_key_blob = conn_config->key_blob;
      const char *passwd = conn_config->key_passwd;
      const bool is_der = conn_config->key_type &&
                          curl_strequal(conn_config->key_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned char *newblob = curlx_memdup0(ssl_key_blob->data,
                                             ssl_key_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_key_blob->len : ssl_key_blob->len + 1;
      만약(!newblob)
        반환 CURLE_OUT_OF_MEMORY;

#만약 MBEDTLS_VERSION_NUMBER >= 0x04000000
      ret = mbedtls_pk_parse_key(&backend->pk, newblob, keylen,
                                 (const unsigned char *)passwd,
                                 passwd ? strlen(passwd) : 0);
      만약(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_KEY_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_KEY_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#아니면
      ret = mbedtls_pk_parse_key(&backend->pk, newblob, keylen,
                                 (const unsigned char *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      만약(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      만약(ret) {
        mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned 정수)-ret, errorbuf);
        반환 CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  반환 CURLE_OK;
}

static CURLcode mbed_load_crl(struct Curl_cfilter *cf,
                              struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  const char * const ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  만약(ssl_crlfile) {
    char errorbuf[128];
#ifdef MBEDTLS_FS_IO
    정수 ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    만약(ret) {
      mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned 정수)-ret, errorbuf);

      반환 CURLE_SSL_CRL_BADFILE;
    }
#아니면
    (void)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    반환 CURLE_NOT_BUILT_IN;
#endif
  }
#아니면
  (void)backend;
  만약(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    반환 CURLE_NOT_BUILT_IN;
  }
#endif

  반환 CURLE_OK;
}

static bool mbed_apply_session(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               struct Curl_ssl_session *sc_session)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;

  만약(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    정수 ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    만약(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned 정수)-ret);
    }
    아니면 {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      만약(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned 정수)-ret);
    }
    mbedtls_ssl_session_free(&session);
    만약(!ret) {
      infof(data, "SSL reusing session ID");
      반환 TRUE;
    }
  }
  반환 FALSE;
}

static CURLcode mbed_configure_ssl(struct Curl_cfilter *cf,
                                   struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  정수 ret;
  bool session_applied = FALSE;
  CURLcode result;
  char errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  만약(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    반환 CURLE_SSL_CONNECT_ERROR;
  }

#ifdef MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, data);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_set_threshold(4);
#endif

#만약 defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_NUMBER >= 0x03060100 &&   \
  MBEDTLS_VERSION_NUMBER < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#endif

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_next);

  result = mbed_set_ssl_version_min_max(data, backend, conn_config);
  만약(result)
    반환 result;

#만약 MBEDTLS_VERSION_NUMBER < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  만약(ret) {
    mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned 정수)-ret, errorbuf);
    반환 CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      NULL /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  만약(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, NULL);
#아니면
  만약(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    만약(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      반환 result;
    }
  }
  아니면 {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#ifdef MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#endif

#ifdef MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#endif

  만약((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    struct Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    만약(scs) {
      만약(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  만약(!session_applied && Curl_ssl_scache_use(cf, data)) {
    struct Curl_ssl_session *sc_session = NULL;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_key,
                                            &sc_session);
    만약(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      만약(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned 정수)-ret);
      }
      아니면 {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        만약(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned 정수)-ret);
        아니면
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_key,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#아니면
                            NULL
#endif
    );

  만약(conn_config->key || conn_config->key_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  만약(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    반환 CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  만약(connssl->alpn) {
    struct alpn_proto_buf proto;
    size_t i;

    위해(i = 0; i < connssl->alpn->count; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    만약(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      반환 CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  만약(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    만약(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  반환 result;
}

static CURLcode mbed_connect_step1(struct Curl_cfilter *cf,
                                   struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  만약((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    반환 CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  만약(!result)
    result = mbed_load_clicert(cf, data);
  만약(!result)
    result = mbed_load_privkey(cf, data);
  만약(!result)
    result = mbed_load_crl(cf, data);
  만약(!result)
    result = mbed_configure_ssl(cf, data);
  만약(result)
    반환 result;

  connssl->connecting_state = ssl_connect_2;
  반환 CURLE_OK;
}

#만약 defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

static CURLcode mbed_connect_step2(struct Curl_cfilter *cf,
                                   struct Curl_easy *data)
{
#만약 defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  정수 ret;
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  const char * const pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#아니면
  const char * const pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  만약(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    반환 CURLE_OK;
  }
  아니면 만약(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    반환 CURLE_OK;
  }
  아니면 만약(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    반환 CURLE_PEER_FAILED_VERIFICATION;
  }
  아니면 만약(ret) {
    char errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_number(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, sizeof(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned 정수)-ret, errorbuf);
    반환 CURLE_SSL_CONNECT_ERROR;
  }

  {
    char cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, sizeof(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  만약(pinnedpubkey) {
    정수 size;
    const mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = NULL;
    unsigned char *pubkey = NULL;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    만약(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      반환 CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, sizeof(*p));

    만약(!p)
      반환 CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    만약(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      goto pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    만약(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      goto pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    만약(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      goto pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    만약(result)
      반환 result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  만약(connssl->alpn) {
    const char *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (const unsigned char *)proto,
                                      proto ? strlen(proto) : 0);
    만약(result)
      반환 result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  반환 CURLE_OK;
}

static CURLcode mbed_new_session(struct Curl_cfilter *cf,
                                 struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  struct Curl_ssl_session *sc_session = NULL;
  unsigned char *sdata = NULL;
  size_t slen = 0;
  정수 ietf_tls_id;
  CURLcode result = CURLE_OK;
  정수 ret;

  DEBUGASSERT(backend);
  만약(!Curl_ssl_scache_use(cf, data))
    반환 CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  만약(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned 정수)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    goto out;
  }

  mbedtls_ssl_session_save(&session, NULL, 0, &slen);
  만약(!slen) {
    failf(data, "failed to serialize session: length is 0");
    goto out;
  }

  sdata = curlx_malloc(slen);
  만약(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  만약(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned 정수)-ret);
    goto out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_number(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = NULL;  /* call took ownership */
  만약(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  만약(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_key,
                                 sc_session);
    sc_session = NULL;
  }

out:
  만약(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  반환 result;
}

static CURLcode mbed_send(struct Curl_cfilter *cf, struct Curl_easy *data,
                          const void *mem, size_t len, size_t *pnwritten)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  정수 nwritten;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#endif
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  만약(backend->send_blocked) {
    만약(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      goto out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (const unsigned char *)mem, len);

  만약(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  아니면 {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned 정수)-nwritten);
    switch(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    case MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      만약(!result)
        goto do_send;
      break;
#endif
    case MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      break;
    case MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      break;
    default:
      result = CURLE_SEND_ERROR;
      break;
    }
    만약((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (정수)result, *pnwritten);
  반환 result;
}

static CURLcode mbedtls_shutdown(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  unsigned char buf[1024];
  CURLcode result = CURLE_OK;
  정수 ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  만약(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    반환 CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  만약(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    만약(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      switch(ret) {
      case 0: /* we sent it, receive from the server */
        break;
      case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        goto out;
      case MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        goto out;
      case MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        goto out;
      default:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned 정수)-ret);
        result = CURLE_RECV_ERROR;
        goto out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  위해(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, sizeof(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    만약(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, sizeof(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    만약(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      continue;
#endif
    만약(ret <= 0)
      break;
  }

  만약(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  아니면 만약(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  아니면 만약(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  아니면 만약(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  아니면 {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned 정수)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  반환 result;
}

static void mbedtls_close(struct Curl_cfilter *cf, struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;

  (void)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  만약(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

static CURLcode mbed_recv(struct Curl_cfilter *cf, struct Curl_easy *data,
                          char *buf, size_t buffersize, size_t *pnread)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  정수 nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned char *)buf, buffersize);
  만약(nread > 0)
    *pnread = (size_t)nread;
  아니면 {
    char errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned 정수)-nread);
    switch(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    case MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      만약(!result)
        goto do_read;
      break;
#endif
    case MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      break;
    case MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      break;
    case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      break;
    default:
      mbedtls_strerror(nread, errorbuf, sizeof(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned 정수)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      break;
    }
  }
  반환 result;
}

static size_t mbedtls_version(char *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned 정수 version = mbedtls_version_get_number();
  반환 curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#아니면
  반환 curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
static CURLcode mbedtls_random(struct Curl_easy *data,
                               unsigned char *entropy, size_t length)
{
  psa_status_t status;
  (void)data;

  status = psa_generate_random(entropy, length);

  반환 status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

static CURLcode mbedtls_connect(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  struct ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  만약(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    반환 CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  만약(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    만약(result)
      반환 result;
  }

  만약(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    만약(result)
      반환 result;
  }

  만약(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    struct ssl_connect_data *ctx = cf->ctx;
    struct mbed_ssl_backend_data *backend =
      (struct mbed_ssl_backend_data *)ctx->backend;

    만약(mbedtls_ssl_get_version_number(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      만약(result)
        반환 result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  만약(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  반환 CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
static 정수 mbedtls_init(void)
{
#만약 MBEDTLS_VERSION_NUMBER < 0x04000000
  정수 ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  만약(status != PSA_SUCCESS)
    반환 0;

#만약 MBEDTLS_VERSION_NUMBER < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              NULL, 0);

  만약(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    반환 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  반환 1;
}

static void mbedtls_cleanup(void)
{
  mbedtls_psa_crypto_free();

#만약 MBEDTLS_VERSION_NUMBER < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

static bool mbedtls_data_pending(struct Curl_cfilter *cf,
                                 const struct Curl_easy *data)
{
  struct ssl_connect_data *ctx = cf->ctx;
  struct mbed_ssl_backend_data *backend;

  (void)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (struct mbed_ssl_backend_data *)ctx->backend;
  반환 mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

static CURLcode mbedtls_sha256sum(const unsigned char *input,
                                  size_t inputlen,
                                  unsigned char *sha256sum,
                                  size_t sha256len)
{
#만약 defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, input, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  만약(status != PSA_SUCCESS)
    반환 CURLE_BAD_FUNCTION_ARGUMENT;
  반환 CURLE_OK;
#아니면
  (void)sha256len;
  반환 Curl_sha256it(sha256sum, input, inputlen);
#endif
}

static void *mbedtls_get_internals(struct ssl_connect_data *connssl,
                                   CURLINFO info)
{
  struct mbed_ssl_backend_data *backend =
    (struct mbed_ssl_backend_data *)connssl->backend;
  (void)info;
  DEBUGASSERT(backend);
  반환 &backend->ssl;
}

const struct Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_PATH |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#ifdef HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#endif
  SSLSUPP_SSL_CTX |
#ifdef MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#endif
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#ifdef MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#endif
  0,

  sizeof(struct mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  NULL,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  NULL,                             /* close_all */
  NULL,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  NULL,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
