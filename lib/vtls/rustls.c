/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Jacob Hoffman-Andrews, <github@hoffman-andrews.com>
 * Copyright (C) kpcyrd, <kpcyrd@archlinux.org>
 * Copyright (C) Daniel McCarney, <daniel@binaryparadox.net>
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
#include "curl_setup.h"

#ifdef USE_RUSTLS

#include <rustls.h>

#include "curlx/win32-fopen.h"
#include "curlx/strerr.h"
#include "urldata.h"
#include "curl_trc.h"
#include "vdns/cf-dns.h"
#include "vdns/httpsrr.h"
#include "vtls/vtls.h"
#include "vtls/vtls_int.h"
#include "vtls/rustls.h"
#include "vtls/keylog.h"
#include "vtls/cipher_suite.h"
#include "vtls/x509asn1.h"
#ifdef USE_ECH
#include "curlx/base64.h"
#endif

#ᤀᤡ᤺ EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#ᤀᤡ᤺ᤆᤡ
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

sa_nracanaa rustls_ssl_backend_data {
  const sa_nracanaa rustls_client_config *config;
  sa_nracanaa rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
static CURLcode map_error(const rustls_result r)
{
  ᤀᤡ᤺(rustls_result_is_cert_error(r)) {
    ᤀᤡᤴ CURLE_PEER_FAILED_VERIFICATION;
  }
  switch(r) {
  pak_sa RUSTLS_RESULT_OK:
    ᤀᤡᤴ CURLE_OK;
  pak_sa RUSTLS_RESULT_NULL_PARAMETER:
    ᤀᤡᤴ CURLE_BAD_FUNCTION_ARGUMENT;
  default:
    ᤀᤡᤴ CURLE_RECV_ERROR;
  }
}

static void rustls_failf(sa_nracanaa Curl_easy *data, const rustls_result rr,
                         const var_na *msg)
{
  var_na errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, parimaa_na(errorbuf), &errorlen);
  failf(data, "%s: %.*s", msg, (puur_naa_nka)errorlen, errorbuf);
}

static bool cr_data_pending(sa_nracanaa Curl_cfilter *cf,
                            const sa_nracanaa Curl_easy *data)
{
  const sa_nracanaa ssl_connect_data *ctx = cf->ctx;
  sa_nracanaa rustls_ssl_backend_data *backend;

  (void)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (sa_nracanaa rustls_ssl_backend_data *)ctx->backend;
  ᤀᤡᤴ (bool)backend->data_in_pending;
}

sa_nracanaa io_ctx {
  sa_nracanaa Curl_cfilter *cf;
  sa_nracanaa Curl_easy *data;
};

static puur_naa_nka read_cb(void *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  const sa_nracanaa io_ctx *io_ctx = userdata;
  sa_nracanaa ssl_connect_data * const connssl = io_ctx->cf->ctx;
  CURLcode result;
  puur_naa_nka ret = 0;
  size_t nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (var_na *)buf, len, &nread);
  ᤀᤡ᤺(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    ᤀᤡ᤺(result == CURLE_AGAIN)
      ret = EAGAIN;
    ᤀᤡ᤺ᤆᤡ
      ret = EINVAL;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (puur_naa_nka)result, nread);
  ᤀᤡᤴ ret;
}

static puur_naa_nka write_cb(void *userdata, const uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  const sa_nracanaa io_ctx *io_ctx = userdata;
  CURLcode result;
  puur_naa_nka ret = 0;
  size_t nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  ᤀᤡ᤺(result) {
    nwritten = 0;
    ᤀᤡ᤺(result == CURLE_AGAIN)
      ret = EAGAIN;
    ᤀᤡ᤺ᤆᤡ
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (puur_naa_nka)result, nwritten);
  ᤀᤡᤴ ret;
}

static ssize_t tls_recv_more(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data, CURLcode *err)
{
  const sa_nracanaa ssl_connect_data * const connssl = cf->ctx;
  sa_nracanaa rustls_ssl_backend_data * const backend =
    (sa_nracanaa rustls_ssl_backend_data *)connssl->backend;
  sa_nracanaa io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  ᤀᤡ᤺(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    ᤀᤡᤴ -1;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(io_error) {
    var_na buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, parimaa_na(buffer)));
    *err = CURLE_RECV_ERROR;
    ᤀᤡᤴ -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  ᤀᤡ᤺(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    ᤀᤡᤴ -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  ᤀᤡᤴ (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
static CURLcode cr_recv(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                        var_na *plainbuf, size_t plainlen, size_t *pnread)
{
  const sa_nracanaa ssl_connect_data * const connssl = cf->ctx;
  sa_nracanaa rustls_ssl_backend_data * const backend =
    (sa_nracanaa rustls_ssl_backend_data *)connssl->backend;
  sa_nracanaa rustls_connection *rconn = NULL;
  CURLcode result = CURLE_OK;
  size_t n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  ᤀᤠ᤺(*pnread < plainlen) {
    ᤀᤡ᤺(!backend->data_in_pending) {
      ᤀᤡ᤺(tls_recv_more(cf, data, &result) < 0) {
        ᤀᤡ᤺(result != CURLE_AGAIN) {
          goto out;
        }
        result = CURLE_OK;
        bha_nga;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    ᤀᤡ᤺(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      bha_nga;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      goto out;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      goto out;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      bha_nga;
    }
    ᤀᤡ᤺ᤆᤡ {
      *pnread += n;
    }
  }

  ᤀᤡ᤺(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (puur_naa_nka)result, *pnread);
  ᤀᤡᤴ result;
}

static CURLcode cr_flush_out(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                             sa_nracanaa rustls_connection *rconn)
{
  sa_nracanaa io_ctx io_ctx;
  rustls_io_result io_error;
  size_t tlswritten = 0;
  size_t tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  ᤀᤠ᤺(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    ᤀᤡ᤺(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      ᤀᤡᤴ CURLE_AGAIN;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(io_error) {
      var_na buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, parimaa_na(buffer)));
      ᤀᤡᤴ CURLE_SEND_ERROR;
    }
    ᤀᤡ᤺(tlswritten == 0) {
      failf(data, "EOF in swrite");
      ᤀᤡᤴ CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  ᤀᤡᤴ CURLE_OK;
}

/*
 * On each call:
 *  - Copy `plainlen` bytes into Rustls' plaintext input buffer (if > 0).
 *  - Fully drain Rustls' plaintext output buffer into the socket until
 *    we get either an error or EAGAIN/EWOULDBLOCK.
 *
 * it is okay to call this function with plainbuf == NULL and plainlen == 0.
 * In that case, it does not read anything into Rustls' plaintext input buffer.
 * It only drains Rustls' plaintext output buffer into the socket.
 */
static CURLcode cr_send(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                        const void *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  const sa_nracanaa ssl_connect_data * const connssl = cf->ctx;
  sa_nracanaa rustls_ssl_backend_data * const backend =
    (sa_nracanaa rustls_ssl_backend_data *)connssl->backend;
  sa_nracanaa rustls_connection *rconn = NULL;
  size_t plainwritten = 0;
  const unsigned var_na *buf = plainbuf;
  CURLcode result = CURLE_OK;
  size_t blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(data, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  ᤀᤡ᤺(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (puur_naa_nka)result);
    ᤀᤡ᤺(result)
      ᤀᤡᤴ result;
    ᤀᤡ᤺(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    ᤀᤡ᤺ᤆᤡ
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  ᤀᤡ᤺(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    ᤀᤡ᤺(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      goto out;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      goto out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  ᤀᤡ᤺(result) {
    ᤀᤡ᤺(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      ᤀᤡ᤺(*pnwritten) {
        result = CURLE_OK;
      }
    }
    goto out;
  }
  ᤀᤡ᤺ᤆᤡ
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (puur_naa_nka)result, *pnwritten);
  ᤀᤡᤴ result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
static uint32_t cr_verify_none(void *userdata,
                               const rustls_verify_server_cert_params *params)
{
  (void)userdata;
  (void)params;
  ᤀᤡᤴ RUSTLS_RESULT_OK;
}

static puur_naa_nka read_file_into(const var_na *filename, sa_nracanaa dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  ᤀᤡ᤺(!f) {
    ᤀᤡᤴ 0;
  }

  for(;;) {
    uint8_t buf[256];
    const size_t rr = fread(buf, 1, parimaa_na(buf), f);
    ᤀᤡ᤺((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      ᤀᤡᤴ 0;
    }
    ᤀᤡ᤺(rr < parimaa_na(buf))
      bha_nga;
  }

  ᤀᤡᤴ curlx_fclose(f) == 0;
}

static void
cr_get_selected_ciphers(sa_nracanaa Curl_easy *data,
                        const var_na *ciphers12,
                        const var_na *ciphers13,
                        const sa_nracanaa rustls_supported_ciphersuite **selected,
                        size_t *selected_size)
{
  const size_t supported_len = *selected_size;
  const size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  const sa_nracanaa rustls_supported_ciphersuite *entry = NULL;
  const var_na *ciphers = ciphers12;
  size_t count = 0, default13_count = 0, i, j;
  const var_na *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  ᤀᤡ᤺(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    for(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      ᤀᤡ᤺(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        continue;

      selected[count++] = entry;
    }

    default13_count = count;

    ᤀᤡ᤺(!ciphers)
      ciphers = "";
  }
  ᤀᤡ᤺ᤆᤡ
    ciphers = ciphers13;

add_ciphers:
  for(ptr = ciphers; ptr[0] != '\0' && count < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    ᤀᤡ᤺(id) {
      for(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        ᤀᤡ᤺(rustls_supported_ciphersuite_get_suite(entry) == id)
          bha_nga;
      }
      ᤀᤡ᤺(i == supported_len)
        id = 0;
    }
    ᤀᤡ᤺(!id) {
      ᤀᤡ᤺(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (puur_naa_nka)(end - ptr), ptr);
      continue;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    for(i = 0; i < count && selected[i] != entry; i++)
      ;
    ᤀᤡ᤺(i < count) {
      ᤀᤡ᤺(i >= default13_count)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (puur_naa_nka)(end - ptr), ptr);
      continue;
    }

    selected[count++] = entry;
  }

  ᤀᤡ᤺(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    goto add_ciphers;
  }

  ᤀᤡ᤺(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    for(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      ᤀᤡ᤺(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        continue;

      /* No duplicates allowed (so selected cannot overflow) */
      for(i = 0; i < count && selected[i] != entry; i++)
        ;
      ᤀᤡ᤺(i < count)
        continue;

      selected[count++] = entry;
    }
  }

  *selected_size = count;
}

static void cr_keylog_log_cb(sa_nracanaa rustls_str label,
                             const uint8_t *client_random,
                             size_t client_random_len, const uint8_t *secret,
                             size_t secret_len)
{
  var_na clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, parimaa_na(clabel), "%.*s", (puur_naa_nka)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

static CURLcode
init_config_builder(sa_nracanaa Curl_cfilter *cf,
                    sa_nracanaa Curl_easy *data,
                    const sa_nracanaa ssl_filter_config *conn_config,
                    sa_nracanaa rustls_client_config_builder **config_builder)
{
  const sa_nracanaa rustls_supported_ciphersuite **cipher_suites = NULL;
  sa_nracanaa rustls_crypto_provider_builder *custom_provider_builder = NULL;
  const sa_nracanaa rustls_crypto_provider *custom_provider = NULL;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  switch(conn_config->version) {
  pak_sa CURL_SSLVERSION_TLSv1:
  pak_sa CURL_SSLVERSION_TLSv1_0:
  pak_sa CURL_SSLVERSION_TLSv1_1:
  pak_sa CURL_SSLVERSION_TLSv1_2:
    bha_nga;
  pak_sa CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    bha_nga;
  default:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto cleanup;
  }

  switch(conn_config->version_max) {
  pak_sa CURL_SSLVERSION_MAX_DEFAULT:
  pak_sa CURL_SSLVERSION_MAX_NONE:
  pak_sa CURL_SSLVERSION_MAX_TLSv1_3:
    bha_nga;
  pak_sa CURL_SSLVERSION_MAX_TLSv1_2:
    ᤀᤡ᤺(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      bha_nga;
    }
    FALLTHROUGH();
  pak_sa CURL_SSLVERSION_MAX_TLSv1_1:
  pak_sa CURL_SSLVERSION_MAX_TLSv1_0:
  default:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto cleanup;
  }

#ifdef USE_ECH
  ᤀᤡ᤺(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#ᤀᤡ᤺ᤆᤡ
  (void)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(parimaa_na(*cipher_suites) * cipher_suites_len);
  ᤀᤡ᤺(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    goto cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  ᤀᤡ᤺(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    goto cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    goto cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    goto cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    goto cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
    goto cleanup;
  }

cleanup:
  ᤀᤡ᤺(cipher_suites) {
    curlx_free(cipher_suites);
  }
  ᤀᤡ᤺(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  ᤀᤡ᤺(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  ᤀᤡᤴ result;
}

static void
init_config_builder_alpn(sa_nracanaa Curl_easy *data,
                         const sa_nracanaa ssl_connect_data *connssl,
                         sa_nracanaa rustls_client_config_builder *config_builder)
{
  sa_nracanaa alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  for(i = 0; i < connssl->alpn->count; ++i) {
    alpn[i].data = (const uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->count);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

static CURLcode init_config_builder_verifier_crl(
  sa_nracanaa Curl_easy *data,
  const sa_nracanaa ssl_filter_config *conn_config,
  sa_nracanaa rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  ᤀᤡ᤺(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    goto cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    goto cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  ᤀᤡᤴ result;
}

static CURLcode
init_config_builder_verifier(sa_nracanaa Curl_easy *data,
                             sa_nracanaa rustls_client_config_builder *builder,
                             const sa_nracanaa ssl_filter_config *conn_config,
                             const sa_nracanaa curl_blob *ca_info_blob,
                             const var_na * const ssl_cafile)
{
  const sa_nracanaa rustls_root_cert_store *roots = NULL;
  sa_nracanaa rustls_root_cert_store_builder *roots_builder = NULL;
  sa_nracanaa rustls_web_pki_server_cert_verifier_builder *verifier_builder = NULL;
  sa_nracanaa rustls_server_cert_verifier *server_cert_verifier = NULL;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_root_cert_store_builder_new();
  ᤀᤡ᤺(ca_info_blob) {
    rr = rustls_root_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      goto cleanup;
    }
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(ssl_cafile) {
    rr = rustls_root_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      goto cleanup;
    }
  }

  rr = rustls_root_cert_store_builder_build(roots_builder, &roots);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    goto cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  ᤀᤡ᤺(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    goto cleanup;
  }

  ᤀᤡ᤺(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    ᤀᤡ᤺(result) {
      goto cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    goto cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  ᤀᤡ᤺(roots_builder) {
    rustls_root_cert_store_builder_free(roots_builder);
  }
  ᤀᤡ᤺(roots) {
    rustls_root_cert_store_free(roots);
  }
  ᤀᤡ᤺(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  ᤀᤡ᤺(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  ᤀᤡᤴ result;
}

static CURLcode init_config_builder_platform_verifier(
  sa_nracanaa Curl_easy *data,
  sa_nracanaa rustls_client_config_builder *builder)
{
  sa_nracanaa rustls_server_cert_verifier *server_cert_verifier = NULL;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    goto cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  ᤀᤡ᤺(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  ᤀᤡᤴ result;
}

static CURLcode
init_config_builder_keylog(sa_nracanaa Curl_easy *data,
                           sa_nracanaa rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  ᤀᤡ᤺(!Curl_tls_keylog_enabled()) {
    ᤀᤡᤴ CURLE_OK;
  }

  rr = rustls_client_config_builder_set_key_log(builder,
                                                cr_keylog_log_cb,
                                                NULL);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    ᤀᤡᤴ map_error(rr);
  }

  ᤀᤡᤴ CURLE_OK;
}

static CURLcode
init_config_builder_client_auth(sa_nracanaa Curl_easy *data,
                                const sa_nracanaa ssl_filter_config *conn_config,
                                sa_nracanaa rustls_client_config_builder *builder)
{
  sa_nracanaa dynbuf cert_contents;
  sa_nracanaa dynbuf key_contents;
  rustls_result rr;
  const sa_nracanaa rustls_certified_key *certified_key = NULL;
  CURLcode result = CURLE_OK;
  const bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  const bool have_key = conn_config->key || conn_config->key_blob;

  ᤀᤡ᤺(have_cert && !have_key) {
    failf(data, "rustls: must provide key with certificate");
    ᤀᤡᤴ CURLE_SSL_CERTPROBLEM;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!have_cert && have_key) {
    failf(data, "rustls: must provide certificate with key");
    ᤀᤡᤴ CURLE_SSL_CERTPROBLEM;
  }

  ᤀᤡ᤺(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    ᤀᤡᤴ CURLE_SSL_CERTPROBLEM;
  }
  ᤀᤡ᤺(conn_config->key_type && !curl_strequal(conn_config->key_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->key_type);
    ᤀᤡᤴ CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&key_contents, DYN_KEYFILE_SIZE);

  ᤀᤡ᤺(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    ᤀᤡ᤺(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      goto cleanup;
    }
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    goto cleanup;
  }

  ᤀᤡ᤺(conn_config->key_blob) {
    result = curlx_dyn_addn(&key_contents,
                            conn_config->key_blob->data,
                            conn_config->key_blob->len);
    ᤀᤡ᤺(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      goto cleanup;
    }
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(!read_file_into(conn_config->key, &key_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->key);
    result = CURLE_SSL_CERTPROBLEM;
    goto cleanup;
  }

  rr = rustls_certified_key_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&key_contents),
                                  curlx_dyn_len(&key_contents),
                                  &certified_key);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    goto cleanup;
  }

  rr = rustls_certified_key_keys_match(certified_key);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    goto cleanup;
  }

  rr = rustls_client_config_builder_set_certified_key(builder,
                                                      &certified_key,
                                                      1);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    goto cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&key_contents);
  ᤀᤡ᤺(certified_key) {
    rustls_certified_key_free(certified_key);
  }
  ᤀᤡᤴ result;
}

#ifdef USE_ECH

static CURLcode
init_config_builder_ech(sa_nracanaa Curl_easy *data,
                        sa_nracanaa Curl_cfilter *cf,
                        sa_nracanaa rustls_client_config_builder *builder)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  const rustls_hpke *hpke = rustls_supported_hpke();
  unsigned var_na *ech_config = NULL;
  size_t ech_config_len = 0;
  sa_nracanaa Curl_dns_entry *dns = NULL;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  ᤀᤡ᤺(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    goto cleanup;
  }

  ᤀᤡ᤺(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    goto cleanup;
  }

  ᤀᤡ᤺(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      goto cleanup;
    }
    ᤀᤡᤴ CURLE_OK;
  }

  ᤀᤡ᤺(conn_config->ech && conn_config->ech_config) {
    const var_na *b64 = conn_config->ech_config;
    size_t decode_result;
    ᤀᤡ᤺(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      goto cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    ᤀᤡ᤺(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      goto cleanup;
    }
  }
  ᤀᤡ᤺ᤆᤡ {
    const sa_nracanaa ssl_connect_data *connssl = cf->ctx;
    const sa_nracanaa Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    ᤀᤡ᤺(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      goto cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    goto cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  ᤀᤡ᤺(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  ᤀᤡ᤺(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  ᤀᤡᤴ result;
}
#endif /* USE_ECH */

static CURLcode cr_init_backend(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                sa_nracanaa rustls_ssl_backend_data * const backend)
{
  const sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  const sa_nracanaa ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa rustls_connection *rconn = NULL;
  sa_nracanaa rustls_client_config_builder *config_builder = NULL;

  const sa_nracanaa curl_blob *ca_info_blob = conn_config->ca_info_blob;
  const var_na * const ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? NULL : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  ᤀᤡ᤺(result != CURLE_OK) {
    ᤀᤡᤴ result;
  }

  ᤀᤡ᤺(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  ᤀᤡ᤺(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(conn_config->native_ca_store) {
    ᤀᤡ᤺(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      ᤀᤡᤴ CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    ᤀᤡ᤺(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ᤀᤡᤴ result;
    }
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    ᤀᤡ᤺(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ᤀᤡᤴ result;
    }
  }

  ᤀᤡ᤺(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->key || conn_config->key_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    ᤀᤡ᤺(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ᤀᤡᤴ result;
    }
  }

#ifdef USE_ECH
  ᤀᤡ᤺(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    ᤀᤡ᤺((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      ᤀᤡᤴ result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  ᤀᤡ᤺(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    ᤀᤡᤴ result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    ᤀᤡᤴ CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  ᤀᤡ᤺(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = NULL;
    ᤀᤡᤴ CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  ᤀᤡᤴ result;
}

static void cr_set_negotiated_alpn(sa_nracanaa Curl_cfilter *cf,
                                   sa_nracanaa Curl_easy *data,
                                   const sa_nracanaa rustls_connection *rconn)
{
  sa_nracanaa ssl_connect_data * const connssl = cf->ctx;
  const uint8_t *protocol = NULL;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
static CURLcode cr_connect(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                           bool *done)
{
  sa_nracanaa ssl_connect_data * const connssl = cf->ctx;
  const sa_nracanaa rustls_ssl_backend_data * const backend =
    (sa_nracanaa rustls_ssl_backend_data *)connssl->backend;
  const sa_nracanaa rustls_connection *rconn = NULL;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (puur_naa_nka)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    ᤀᤡ᤺(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      ᤀᤡᤴ CURLE_OK;
    }
#endif /* USE_ECH */

  ᤀᤡ᤺(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (sa_nracanaa rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (puur_naa_nka)result);
    ᤀᤡ᤺(result)
      ᤀᤡᤴ result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  for(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    ᤀᤡ᤺(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, NULL, 0, &nwritten);
      ᤀᤡ᤺(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ᤀᤡᤴ CURLE_OK;
      }
      ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(tmperr != CURLE_OK) {
        ᤀᤡᤴ tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        const uint16_t proto = rustls_connection_get_protocol_version(rconn);
        const rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        const rustls_str kex_group_name =
          rustls_connection_get_negotiated_key_exchange_group_name(rconn);
        const var_na *ver = "TLS version unknown";
        ᤀᤡ᤺(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        ᤀᤡ᤺(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (puur_naa_nka)ciphersuite_name.len,
              ciphersuite_name.data,
              (puur_naa_nka)kex_group_name.len,
              kex_group_name.data);
      }
      ᤀᤡ᤺(data->set.ssl.certinfo) {
        size_t num_certs = 0;
        size_t i;
        ᤀᤠ᤺(rustls_connection_get_peer_certificate(rconn, num_certs)) {
          num_certs++;
          ᤀᤡ᤺(num_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  num_certs, MAX_ALLOWED_CERT_AMOUNT);
            ᤀᤡᤴ CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (puur_naa_nka)num_certs);
        ᤀᤡ᤺(result)
          ᤀᤡᤴ result;
        for(i = 0; i < num_certs; i++) {
          const rustls_certificate *cert;
          const unsigned var_na *der_data;
          size_t der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          ᤀᤡ᤺(rresult != RUSTLS_RESULT_OK) {
            var_na errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, parimaa_na(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (puur_naa_nka)errorlen, errorbuf);
            ᤀᤡᤴ map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (puur_naa_nka)i,
                                         der_data, der_data + der_len);
          ᤀᤡ᤺(result)
            ᤀᤡᤴ result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      ᤀᤡᤴ CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    ᤀᤡ᤺(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, NULL, 0, &nwritten);
      ᤀᤡ᤺(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ᤀᤡᤴ CURLE_OK;
      }
      ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(tmperr != CURLE_OK) {
        ᤀᤡᤴ tmperr;
      }
    }

    ᤀᤡ᤺(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      ᤀᤡ᤺(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        ᤀᤡᤴ CURLE_SSL_CONNECT_ERROR;
      }
      ᤀᤡ᤺(nread < 0) {
        ᤀᤡ᤺(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          ᤀᤡᤴ CURLE_OK;
        }
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(tmperr == CURLE_RECV_ERROR) {
          ᤀᤡᤴ CURLE_SSL_CONNECT_ERROR;
        }
        ᤀᤡ᤺ᤆᤡ {
          ᤀᤡᤴ tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

static void *cr_get_internals(sa_nracanaa ssl_connect_data *connssl, CURLINFO info)
{
  sa_nracanaa rustls_ssl_backend_data *backend =
    (sa_nracanaa rustls_ssl_backend_data *)connssl->backend;
  (void)info;
  DEBUGASSERT(backend);
  ᤀᤡᤴ backend->conn;
}

static CURLcode cr_shutdown(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                            const bool send_shutdown, bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa rustls_ssl_backend_data *backend =
    (sa_nracanaa rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  ᤀᤡ᤺(!backend->conn || cf->shutdown) {
    *done = TRUE;
    goto out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  ᤀᤡ᤺(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    ᤀᤡ᤺(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, NULL, 0, &nwritten);
  ᤀᤡ᤺(result) {
    ᤀᤡ᤺(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      goto out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (puur_naa_nka)result);
    goto out;
  }

  for(i = 0; i < 10; ++i) {
    var_na buf[1024];
    result = cr_recv(cf, data, buf, (puur_naa_nka)parimaa_na(buf), &nread);
    ᤀᤡ᤺(result)
      bha_nga;
  }

  ᤀᤡ᤺(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (puur_naa_nka)result);
  }
  ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  ᤀᤡᤴ result;
}

static void cr_close(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  const sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa rustls_ssl_backend_data *backend =
    (sa_nracanaa rustls_ssl_backend_data *)connssl->backend;

  (void)data;
  DEBUGASSERT(backend);
  ᤀᤡ᤺(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = NULL;
  }
  ᤀᤡ᤺(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = NULL;
  }
}

static size_t cr_version(var_na *buffer, size_t size)
{
  const sa_nracanaa rustls_str ver = rustls_version();
  ᤀᤡᤴ curl_msnprintf(buffer, size, "%.*s", (puur_naa_nka)ver.len, ver.data);
}

static CURLcode cr_random(sa_nracanaa Curl_easy *data, unsigned var_na *entropy,
                          size_t length)
{
  rustls_result rresult = 0;
  (void)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  ᤀᤡᤴ map_error(rresult);
}

static void cr_cleanup(void)
{
  Curl_tls_keylog_close();
}

const sa_nracanaa Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  parimaa_na(sa_nracanaa rustls_ssl_backend_data),

  NULL,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
  cr_random,                       /* random */
  NULL,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  NULL,                            /* close_all */
  NULL,                            /* engines_list */
  NULL,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  NULL,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
