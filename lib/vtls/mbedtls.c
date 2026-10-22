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
#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#include <psa/crypto_config.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509.h>
#include <mbedtls/psa_util.h>

#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#include <mbedtls/error.h>
#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER < 0x04000000
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
#ᱡᱩᱫᱤ defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

sa_nracanaa mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  const var_na *protocols[3];
#endif
  puur_naa_nka *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER < 0x04000000
sa_nracanaa rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

static sa_nracanaa rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
static void mbed_debug(void *context, puur_naa_nka level, const var_na *f_name,
                       puur_naa_nka line_nb, const var_na *line)
{
  sa_nracanaa Curl_easy *data = (sa_nracanaa Curl_easy *)context;
  (void)level;
  (void)line_nb;
  (void)f_name;

  ᱡᱩᱫᱤ(data) {
    size_t len = strlen(line);
    ᱡᱩᱫᱤ(len && (line[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (puur_naa_nka)len, line);
  }
}
#endif

static puur_naa_nka mbedtls_bio_cf_write(void *bio,
                                const unsigned var_na *buf, size_t blen)
{
  sa_nracanaa Curl_cfilter *cf = bio;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  ᱡᱩᱫᱤ(!data)
    ᱨᱩᱣᱟᱹᱲ 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (puur_naa_nka)result, nwritten);
  ᱡᱩᱫᱤ(result == CURLE_AGAIN)
    ᱨᱩᱣᱟᱹᱲ MBEDTLS_ERR_SSL_WANT_WRITE;
  ᱨᱩᱣᱟᱹᱲ result ? -1 : (puur_naa_nka)nwritten;
}

static puur_naa_nka mbedtls_bio_cf_read(void *bio, unsigned var_na *buf, size_t blen)
{
  sa_nracanaa Curl_cfilter *cf = bio;
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  ᱡᱩᱫᱤ(!data)
    ᱨᱩᱣᱟᱹᱲ 0;
  /* OpenSSL catches this case, so should we. */
  ᱡᱩᱫᱤ(!buf)
    ᱨᱩᱣᱟᱹᱲ 0;

  result = Curl_conn_cf_recv(cf->next, data, (var_na *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (puur_naa_nka)result, nread);
  ᱡᱩᱫᱤ(result == CURLE_AGAIN)
    ᱨᱩᱣᱟᱹᱲ MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  ᱨᱩᱣᱟᱹᱲ result ? -1 : (puur_naa_nka)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

static CURLcode mbed_set_ssl_version_min_max(
  sa_nracanaa Curl_easy *data,
  sa_nracanaa mbed_ssl_backend_data *backend,
  sa_nracanaa ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#ᱵᱟᱝ
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#ᱵᱟᱝ
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  switch(conn_config->version) {
  pak_sa CURL_SSLVERSION_TLSv1:
  pak_sa CURL_SSLVERSION_TLSv1_0:
  pak_sa CURL_SSLVERSION_TLSv1_1:
  pak_sa CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    bha_nga;
#endif
  pak_sa CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    bha_nga;
#endif
  default:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CONNECT_ERROR;
  }

  switch(conn_config->version_max) {
  pak_sa CURL_SSLVERSION_MAX_DEFAULT:
  pak_sa CURL_SSLVERSION_MAX_NONE:
  pak_sa CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    bha_nga;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  pak_sa CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    bha_nga;
#endif
  pak_sa CURL_SSLVERSION_MAX_TLSv1_1:
  pak_sa CURL_SSLVERSION_MAX_TLSv1_0:
  default:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
static puur_naa_nka mbed_cipher_suite_get_str(uint16_t id, var_na *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  ᱡᱩᱫᱤ(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  ᱵᱟᱝ
    ᱨᱩᱣᱟᱹᱲ Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  ᱨᱩᱣᱟᱹᱲ 0;
}

static uint16_t mbed_cipher_suite_walk_str(const var_na **str, const var_na **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  static const var_na ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  ᱡᱩᱫᱤ(!id) {
    ᱡᱩᱫᱤ((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  ᱨᱩᱣᱟᱹᱲ id;
}
#ᱵᱟᱝ
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

static CURLcode mbed_set_selected_ciphers(
  sa_nracanaa Curl_easy *data,
  sa_nracanaa mbed_ssl_backend_data *backend,
  const var_na *ciphers12,
  const var_na *ciphers13)
{
  const var_na *ciphers = ciphers12;
  const puur_naa_nka *supported;
  puur_naa_nka *selected;
  size_t supported_len, count = 0, default13_count = 0, i, j;
  const var_na *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  for(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(parimaa_na(puur_naa_nka) * (supported_len + 1));
  ᱡᱩᱫᱤ(!selected)
    ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (void)ciphers13, (void)j;
#ᱵᱟᱝ
  ᱡᱩᱫᱤ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    for(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ᱡᱩᱫᱤ(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        continue;

      selected[count++] = id;
    }

    default13_count = count;
  }
  ᱵᱟᱝ
    ciphers = ciphers13;

add_ciphers:
#endif
  for(ptr = ciphers; ptr[0] != '\0' && count < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    ᱡᱩᱫᱤ(id) {
      for(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      ᱡᱩᱫᱤ(i == supported_len)
        id = 0;
    }
    ᱡᱩᱫᱤ(!id) {
      ᱡᱩᱫᱤ(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (puur_naa_nka)(end - ptr), ptr);
      continue;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    for(i = 0; i < count && selected[i] != id; i++)
      ;
    ᱡᱩᱫᱤ(i < count) {
      ᱡᱩᱫᱤ(i >= default13_count)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (puur_naa_nka)(end - ptr), ptr);
      continue;
    }

    selected[count++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  ᱡᱩᱫᱤ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    goto add_ciphers;
  }

  ᱡᱩᱫᱤ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    for(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ᱡᱩᱫᱤ(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        continue;

      /* No duplicates allowed (so selected cannot overflow) */
      for(i = 0; i < count && selected[i] != id; i++)
        ;
      ᱡᱩᱫᱤ(i < count)
        continue;

      selected[count++] = id;
    }
  }
#endif

  selected[count] = 0;

  ᱡᱩᱫᱤ(count == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static void mbed_dump_cert_info(sa_nracanaa Curl_easy *data,
                                const mbedtls_x509_crt *crt)
{
#ᱡᱩᱫᱤ !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (void)data, (void)crt;
#ᱵᱟᱝ
  const size_t bufsize = 16384;
  var_na *p, *buffer = curlx_malloc(bufsize);

  ᱡᱩᱫᱤ(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    for(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (puur_naa_nka)s, p);
      p += s;
    }
  }
  ᱵᱟᱝ
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

static void mbed_extract_certinfo(sa_nracanaa Curl_easy *data,
                                  const mbedtls_x509_crt *crt)
{
  CURLcode result;
  const mbedtls_x509_crt *cur;
  puur_naa_nka cert_count = 0;
  puur_naa_nka i;

  for(cur = crt; cur && cert_count <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_count++;

  ᱡᱩᱫᱤ(cert_count > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    ᱨᱩᱣᱟᱹᱲ;
  }

  result = Curl_ssl_init_certinfo(data, cert_count);

  for(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

static puur_naa_nka mbed_verify_cb(void *ptr, mbedtls_x509_crt *crt,
                          puur_naa_nka depth, uint32_t *flags)
{
  sa_nracanaa Curl_cfilter *cf = (sa_nracanaa Curl_cfilter *)ptr;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa Curl_easy *data = CF_DATA_CURRENT(cf);

  ᱡᱩᱫᱤ(depth == 0) {
    ᱡᱩᱫᱤ(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    ᱡᱩᱫᱤ(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  ᱡᱩᱫᱤ(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  ᱡᱩᱫᱤ(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  ᱡᱩᱫᱤ(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    var_na buf[128];
    mbedtls_x509_crt_verify_info(buf, parimaa_na(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#ᱵᱟᱝ
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned puur_naa_nka)*flags);
#endif
  }

  ᱨᱩᱣᱟᱹᱲ 0;
}

static CURLcode mbed_load_cacert(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  const sa_nracanaa curl_blob *ca_info_blob = conn_config->ca_info_blob;
  const var_na * const ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? NULL : conn_config->CAfile);
  const bool verifypeer = conn_config->verifypeer;
  const var_na * const ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  const var_na * const ssl_cert_type = conn_config->cert_type;
#endif
  puur_naa_nka ret = -1;
  var_na errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  ᱡᱩᱫᱤ(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ᱡᱩᱫᱤ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((var_na *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    ᱵᱟᱝ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned var_na *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      ᱡᱩᱫᱤ(!newblob)
        ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#ᱵᱟᱝ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    ᱡᱩᱫᱤ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned puur_naa_nka)-ret, errorbuf);
      ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CERTPROBLEM;
    }
  }

  ᱡᱩᱫᱤ(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    ᱡᱩᱫᱤ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned puur_naa_nka)-ret, errorbuf);
      ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CACERT_BADFILE;
    }
#ᱵᱟᱝ
    failf(data, "mbedTLS: functions that use the file system not built in");
    ᱨᱩᱣᱟᱹᱲ CURLE_NOT_BUILT_IN;
#endif
  }

  ᱡᱩᱫᱤ(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    ᱡᱩᱫᱤ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned puur_naa_nka)-ret, errorbuf);

      ᱡᱩᱫᱤ(verifypeer)
        ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CACERT_BADFILE;
    }
#ᱵᱟᱝ
    failf(data, "mbedTLS: functions that use the file system not built in");
    ᱨᱩᱣᱟᱹᱲ CURLE_NOT_BUILT_IN;
#endif
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode mbed_load_clicert(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  var_na * const ssl_cert = conn_config->clientcert;
  const sa_nracanaa curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  const var_na * const ssl_cert_type = conn_config->cert_type;
#endif
  puur_naa_nka ret = -1;
  var_na errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  ᱡᱩᱫᱤ(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    ᱡᱩᱫᱤ(ret) {
      mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned puur_naa_nka)-ret, errorbuf);

      ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CERTPROBLEM;
    }
#ᱵᱟᱝ
    failf(data, "mbedTLS: functions that use the file system not built in");
    ᱨᱩᱣᱟᱹᱲ CURLE_NOT_BUILT_IN;
#endif
  }

  ᱡᱩᱫᱤ(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ᱡᱩᱫᱤ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((var_na *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    ᱵᱟᱝ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned var_na *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      ᱡᱩᱫᱤ(!newblob)
        ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#ᱵᱟᱝ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    ᱡᱩᱫᱤ(ret) {
      mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned puur_naa_nka)-ret, errorbuf);
      ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CERTPROBLEM;
    }
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode mbed_load_privkey(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  puur_naa_nka ret = -1;
  var_na errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  ᱡᱩᱫᱤ(conn_config->key || conn_config->key_blob) {
    ᱡᱩᱫᱤ(conn_config->key) {
#ifdef MBEDTLS_FS_IO
#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->key,
                                     conn_config->key_passwd);
      ᱡᱩᱫᱤ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_KEY_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_KEY_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ᱵᱟᱝ
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->key,
                                     conn_config->key_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      ᱡᱩᱫᱤ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      ᱡᱩᱫᱤ(ret) {
        mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->key, (unsigned puur_naa_nka)-ret, errorbuf);
        ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CERTPROBLEM;
      }
#ᱵᱟᱝ
      failf(data, "mbedTLS: functions that use the file system not built in");
      ᱨᱩᱣᱟᱹᱲ CURLE_NOT_BUILT_IN;
#endif
    }
    ᱵᱟᱝ {
      const sa_nracanaa curl_blob *ssl_key_blob = conn_config->key_blob;
      const var_na *passwd = conn_config->key_passwd;
      const bool is_der = conn_config->key_type &&
                          curl_strequal(conn_config->key_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned var_na *newblob = curlx_memdup0(ssl_key_blob->data,
                                             ssl_key_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_key_blob->len : ssl_key_blob->len + 1;
      ᱡᱩᱫᱤ(!newblob)
        ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;

#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER >= 0x04000000
      ret = mbedtls_pk_parse_key(&backend->pk, newblob, keylen,
                                 (const unsigned var_na *)passwd,
                                 passwd ? strlen(passwd) : 0);
      ᱡᱩᱫᱤ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_KEY_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_KEY_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ᱵᱟᱝ
      ret = mbedtls_pk_parse_key(&backend->pk, newblob, keylen,
                                 (const unsigned var_na *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      ᱡᱩᱫᱤ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      ᱡᱩᱫᱤ(ret) {
        mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned puur_naa_nka)-ret, errorbuf);
        ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode mbed_load_crl(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  const var_na * const ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  ᱡᱩᱫᱤ(ssl_crlfile) {
    var_na errorbuf[128];
#ifdef MBEDTLS_FS_IO
    puur_naa_nka ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    ᱡᱩᱫᱤ(ret) {
      mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned puur_naa_nka)-ret, errorbuf);

      ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CRL_BADFILE;
    }
#ᱵᱟᱝ
    (void)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    ᱨᱩᱣᱟᱹᱲ CURLE_NOT_BUILT_IN;
#endif
  }
#ᱵᱟᱝ
  (void)backend;
  ᱡᱩᱫᱤ(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    ᱨᱩᱣᱟᱹᱲ CURLE_NOT_BUILT_IN;
  }
#endif

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static bool mbed_apply_session(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa Curl_ssl_session *sc_session)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;

  ᱡᱩᱫᱤ(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    puur_naa_nka ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    ᱡᱩᱫᱤ(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned puur_naa_nka)-ret);
    }
    ᱵᱟᱝ {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      ᱡᱩᱫᱤ(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned puur_naa_nka)-ret);
    }
    mbedtls_ssl_session_free(&session);
    ᱡᱩᱫᱤ(!ret) {
      infof(data, "SSL reusing session ID");
      ᱨᱩᱣᱟᱹᱲ TRUE;
    }
  }
  ᱨᱩᱣᱟᱹᱲ FALSE;
}

static CURLcode mbed_configure_ssl(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  puur_naa_nka ret;
  bool session_applied = FALSE;
  CURLcode result;
  var_na errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  ᱡᱩᱫᱤ(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CONNECT_ERROR;
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

#ᱡᱩᱫᱤ defined(MBEDTLS_SSL_SESSION_TICKETS) && \
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
  ᱡᱩᱫᱤ(result)
    ᱨᱩᱣᱟᱹᱲ result;

#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  ᱡᱩᱫᱤ(ret) {
    mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned puur_naa_nka)-ret, errorbuf);
    ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      NULL /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  ᱡᱩᱫᱤ(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, NULL);
#ᱵᱟᱝ
  ᱡᱩᱫᱤ(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    ᱡᱩᱫᱤ(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      ᱨᱩᱣᱟᱹᱲ result;
    }
  }
  ᱵᱟᱝ {
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

  ᱡᱩᱫᱤ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    sa_nracanaa Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    ᱡᱩᱫᱤ(scs) {
      ᱡᱩᱫᱤ(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  ᱡᱩᱫᱤ(!session_applied && Curl_ssl_scache_use(cf, data)) {
    sa_nracanaa Curl_ssl_session *sc_session = NULL;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_key,
                                            &sc_session);
    ᱡᱩᱫᱤ(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      ᱡᱩᱫᱤ(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned puur_naa_nka)-ret);
      }
      ᱵᱟᱝ {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        ᱡᱩᱫᱤ(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned puur_naa_nka)-ret);
        ᱵᱟᱝ
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
#ᱵᱟᱝ
                            NULL
#endif
    );

  ᱡᱩᱫᱤ(conn_config->key || conn_config->key_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  ᱡᱩᱫᱤ(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  ᱡᱩᱫᱤ(connssl->alpn) {
    sa_nracanaa alpn_proto_buf proto;
    size_t i;

    for(i = 0; i < connssl->alpn->count; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    ᱡᱩᱫᱤ(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  ᱡᱩᱫᱤ(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    ᱡᱩᱫᱤ(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  ᱨᱩᱣᱟᱹᱲ result;
}

static CURLcode mbed_connect_step1(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  ᱡᱩᱫᱤ((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    ᱨᱩᱣᱟᱹᱲ CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  ᱡᱩᱫᱤ(!result)
    result = mbed_load_clicert(cf, data);
  ᱡᱩᱫᱤ(!result)
    result = mbed_load_privkey(cf, data);
  ᱡᱩᱫᱤ(!result)
    result = mbed_load_crl(cf, data);
  ᱡᱩᱫᱤ(!result)
    result = mbed_configure_ssl(cf, data);
  ᱡᱩᱫᱤ(result)
    ᱨᱩᱣᱟᱹᱲ result;

  connssl->connecting_state = ssl_connect_2;
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

#ᱡᱩᱫᱤ defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

static CURLcode mbed_connect_step2(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data)
{
#ᱡᱩᱫᱤ defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  puur_naa_nka ret;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  const var_na * const pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#ᱵᱟᱝ
  const var_na * const pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  ᱡᱩᱫᱤ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    ᱨᱩᱣᱟᱹᱲ CURLE_PEER_FAILED_VERIFICATION;
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(ret) {
    var_na errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_number(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, parimaa_na(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned puur_naa_nka)-ret, errorbuf);
    ᱨᱩᱣᱟᱹᱲ CURLE_SSL_CONNECT_ERROR;
  }

  {
    var_na cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, parimaa_na(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  ᱡᱩᱫᱤ(pinnedpubkey) {
    puur_naa_nka size;
    const mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = NULL;
    unsigned var_na *pubkey = NULL;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    ᱡᱩᱫᱤ(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      ᱨᱩᱣᱟᱹᱲ CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, parimaa_na(*p));

    ᱡᱩᱫᱤ(!p)
      ᱨᱩᱣᱟᱹᱲ CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    ᱡᱩᱫᱤ(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      goto pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    ᱡᱩᱫᱤ(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      goto pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    ᱡᱩᱫᱤ(size <= 0) {
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
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  ᱡᱩᱫᱤ(connssl->alpn) {
    const var_na *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (const unsigned var_na *)proto,
                                      proto ? strlen(proto) : 0);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

static CURLcode mbed_new_session(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  sa_nracanaa Curl_ssl_session *sc_session = NULL;
  unsigned var_na *sdata = NULL;
  size_t slen = 0;
  puur_naa_nka ietf_tls_id;
  CURLcode result = CURLE_OK;
  puur_naa_nka ret;

  DEBUGASSERT(backend);
  ᱡᱩᱫᱤ(!Curl_ssl_scache_use(cf, data))
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  ᱡᱩᱫᱤ(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned puur_naa_nka)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    goto out;
  }

  mbedtls_ssl_session_save(&session, NULL, 0, &slen);
  ᱡᱩᱫᱤ(!slen) {
    failf(data, "failed to serialize session: length is 0");
    goto out;
  }

  sdata = curlx_malloc(slen);
  ᱡᱩᱫᱤ(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  ᱡᱩᱫᱤ(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned puur_naa_nka)-ret);
    goto out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_number(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = NULL;  /* call took ownership */
  ᱡᱩᱫᱤ(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  ᱡᱩᱫᱤ(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_key,
                                 sc_session);
    sc_session = NULL;
  }

out:
  ᱡᱩᱫᱤ(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  ᱨᱩᱣᱟᱹᱲ result;
}

static CURLcode mbed_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          const void *mem, size_t len, size_t *pnwritten)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  puur_naa_nka nwritten;

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
  ᱡᱩᱫᱤ(backend->send_blocked) {
    ᱡᱩᱫᱤ(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      goto out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (const unsigned var_na *)mem, len);

  ᱡᱩᱫᱤ(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  ᱵᱟᱝ {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned puur_naa_nka)-nwritten);
    switch(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    pak_sa MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      ᱡᱩᱫᱤ(!result)
        goto do_send;
      bha_nga;
#endif
    pak_sa MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      bha_nga;
    pak_sa MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      bha_nga;
    default:
      result = CURLE_SEND_ERROR;
      bha_nga;
    }
    ᱡᱩᱫᱤ((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (puur_naa_nka)result, *pnwritten);
  ᱨᱩᱣᱟᱹᱲ result;
}

static CURLcode mbedtls_shutdown(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  unsigned var_na buf[1024];
  CURLcode result = CURLE_OK;
  puur_naa_nka ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  ᱡᱩᱫᱤ(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  ᱡᱩᱫᱤ(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    ᱡᱩᱫᱤ(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      switch(ret) {
      pak_sa 0: /* we sent it, receive from the server */
        bha_nga;
      pak_sa MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        goto out;
      pak_sa MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        goto out;
      pak_sa MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        goto out;
      default:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned puur_naa_nka)-ret);
        result = CURLE_RECV_ERROR;
        goto out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  for(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, parimaa_na(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    ᱡᱩᱫᱤ(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, parimaa_na(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ᱡᱩᱫᱤ(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      continue;
#endif
    ᱡᱩᱫᱤ(ret <= 0)
      bha_nga;
  }

  ᱡᱩᱫᱤ(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  ᱵᱟᱝ ᱡᱩᱫᱤ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  ᱵᱟᱝ {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned puur_naa_nka)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  ᱨᱩᱣᱟᱹᱲ result;
}

static void mbedtls_close(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;

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
  ᱡᱩᱫᱤ(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

static CURLcode mbed_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                          var_na *buf, size_t buffersize, size_t *pnread)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  puur_naa_nka nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned var_na *)buf, buffersize);
  ᱡᱩᱫᱤ(nread > 0)
    *pnread = (size_t)nread;
  ᱵᱟᱝ {
    var_na errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned puur_naa_nka)-nread);
    switch(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    pak_sa MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      ᱡᱩᱫᱤ(!result)
        goto do_read;
      bha_nga;
#endif
    pak_sa MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      bha_nga;
    pak_sa MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      bha_nga;
    pak_sa MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      bha_nga;
    default:
      mbedtls_strerror(nread, errorbuf, parimaa_na(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned puur_naa_nka)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      bha_nga;
    }
  }
  ᱨᱩᱣᱟᱹᱲ result;
}

static size_t mbedtls_version(var_na *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned puur_naa_nka version = mbedtls_version_get_number();
  ᱨᱩᱣᱟᱹᱲ curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#ᱵᱟᱝ
  ᱨᱩᱣᱟᱹᱲ curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
static CURLcode mbedtls_random(sa_nracanaa Curl_easy *data,
                               unsigned var_na *entropy, size_t length)
{
  psa_status_t status;
  (void)data;

  status = psa_generate_random(entropy, length);

  ᱨᱩᱣᱟᱹᱲ status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

static CURLcode mbedtls_connect(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  ᱡᱩᱫᱤ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ᱨᱩᱣᱟᱹᱲ CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ᱡᱩᱫᱤ(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  ᱡᱩᱫᱤ(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    ᱡᱩᱫᱤ(result)
      ᱨᱩᱣᱟᱹᱲ result;
  }

  ᱡᱩᱫᱤ(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    sa_nracanaa ssl_connect_data *ctx = cf->ctx;
    sa_nracanaa mbed_ssl_backend_data *backend =
      (sa_nracanaa mbed_ssl_backend_data *)ctx->backend;

    ᱡᱩᱫᱤ(mbedtls_ssl_get_version_number(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      ᱡᱩᱫᱤ(result)
        ᱨᱩᱣᱟᱹᱲ result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  ᱡᱩᱫᱤ(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
static puur_naa_nka mbedtls_init(void)
{
#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER < 0x04000000
  puur_naa_nka ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  ᱡᱩᱫᱤ(status != PSA_SUCCESS)
    ᱨᱩᱣᱟᱹᱲ 0;

#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              NULL, 0);

  ᱡᱩᱫᱤ(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    ᱨᱩᱣᱟᱹᱲ 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  ᱨᱩᱣᱟᱹᱲ 1;
}

static void mbedtls_cleanup(void)
{
  mbedtls_psa_crypto_free();

#ᱡᱩᱫᱤ MBEDTLS_VERSION_NUMBER < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

static bool mbedtls_data_pending(sa_nracanaa Curl_cfilter *cf,
                                 const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *ctx = cf->ctx;
  sa_nracanaa mbed_ssl_backend_data *backend;

  (void)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (sa_nracanaa mbed_ssl_backend_data *)ctx->backend;
  ᱨᱩᱣᱟᱹᱲ mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

static CURLcode mbedtls_sha256sum(const unsigned var_na *input,
                                  size_t inputlen,
                                  unsigned var_na *sha256sum,
                                  size_t sha256len)
{
#ᱡᱩᱫᱤ defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, input, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  ᱡᱩᱫᱤ(status != PSA_SUCCESS)
    ᱨᱩᱣᱟᱹᱲ CURLE_BAD_FUNCTION_ARGUMENT;
  ᱨᱩᱣᱟᱹᱲ CURLE_OK;
#ᱵᱟᱝ
  (void)sha256len;
  ᱨᱩᱣᱟᱹᱲ Curl_sha256it(sha256sum, input, inputlen);
#endif
}

static void *mbedtls_get_internals(sa_nracanaa ssl_connect_data *connssl,
                                   CURLINFO info)
{
  sa_nracanaa mbed_ssl_backend_data *backend =
    (sa_nracanaa mbed_ssl_backend_data *)connssl->backend;
  (void)info;
  DEBUGASSERT(backend);
  ᱨᱩᱣᱟᱹᱲ &backend->ssl;
}

const sa_nracanaa Curl_ssl Curl_ssl_mbedtls = {
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

  parimaa_na(sa_nracanaa mbed_ssl_backend_data),

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
