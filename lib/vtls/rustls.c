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
#ضمن "curl_setup.h"

#ifdef USE_RUSTLS

#ضمن <rustls.h>

#ضمن "curlx/win32-fopen.h"
#ضمن "curlx/strerr.h"
#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "vdns/cf-dns.h"
#ضمن "vdns/httpsrr.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_int.h"
#ضمن "vtls/rustls.h"
#ضمن "vtls/keylog.h"
#ضمن "vtls/cipher_suite.h"
#ضمن "vtls/x509asn1.h"
#ifdef USE_ECH
#ضمن "curlx/base64.h"
#endif

#إذا EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#وإلا
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

بنية rustls_ssl_backend_data {
  ثابت بنية rustls_client_config *config;
  بنية rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
ساكن CURLcode map_error(ثابت rustls_result r)
{
  إذا(rustls_result_is_cert_error(r)) {
    أرجع CURLE_PEER_FAILED_VERIFICATION;
  }
  بدل(r) {
  حالة RUSTLS_RESULT_OK:
    أرجع CURLE_OK;
  حالة RUSTLS_RESULT_NULL_PARAMETER:
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;
  افتراضي:
    أرجع CURLE_RECV_ERROR;
  }
}

ساكن فراغ rustls_failf(بنية Curl_easy *data, ثابت rustls_result rr,
                         ثابت حرف *ڤسنن)
{
  حرف errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, حجم(errorbuf), &errorlen);
  failf(data, "%s: %.*s", ڤسنن, (صحيح)errorlen, errorbuf);
}

ساكن bool cr_data_pending(بنية Curl_cfilter *cf,
                            ثابت بنية Curl_easy *data)
{
  ثابت بنية ssl_connect_data *ctx = cf->ctx;
  بنية rustls_ssl_backend_data *backend;

  (فراغ)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية rustls_ssl_backend_data *)ctx->backend;
  أرجع (bool)backend->data_in_pending;
}

بنية io_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *data;
};

ساكن صحيح read_cb(فراغ *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  ثابت بنية io_ctx *io_ctx = userdata;
  بنية ssl_connect_data * ثابت connssl = io_ctx->cf->ctx;
  CURLcode result;
  صحيح ret = 0;
  size_t nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (حرف *)buf, len, &nread);
  إذا(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    إذا(result == CURLE_AGAIN)
      ret = EAGAIN;
    وإلا
      ret = EINVAL;
  }
  وإلا إذا(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (صحيح)result, nread);
  أرجع ret;
}

ساكن صحيح write_cb(فراغ *userdata, ثابت uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  ثابت بنية io_ctx *io_ctx = userdata;
  CURLcode result;
  صحيح ret = 0;
  size_t nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  إذا(result) {
    nwritten = 0;
    إذا(result == CURLE_AGAIN)
      ret = EAGAIN;
    وإلا
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (صحيح)result, nwritten);
  أرجع ret;
}

ساكن ssize_t tls_recv_more(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data, CURLcode *err)
{
  ثابت بنية ssl_connect_data * ثابت connssl = cf->ctx;
  بنية rustls_ssl_backend_data * ثابت backend =
    (بنية rustls_ssl_backend_data *)connssl->backend;
  بنية io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  إذا(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    أرجع -1;
  }
  وإلا إذا(io_error) {
    حرف buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, حجم(buffer)));
    *err = CURLE_RECV_ERROR;
    أرجع -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  إذا(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    أرجع -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  أرجع (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
ساكن CURLcode cr_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                        حرف *plainbuf, size_t plainlen, size_t *pnread)
{
  ثابت بنية ssl_connect_data * ثابت connssl = cf->ctx;
  بنية rustls_ssl_backend_data * ثابت backend =
    (بنية rustls_ssl_backend_data *)connssl->backend;
  بنية rustls_connection *rconn = عدم;
  CURLcode result = CURLE_OK;
  size_t n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  طالما(*pnread < plainlen) {
    إذا(!backend->data_in_pending) {
      إذا(tls_recv_more(cf, data, &result) < 0) {
        إذا(result != CURLE_AGAIN) {
          اذهب out;
        }
        result = CURLE_OK;
        اكسر;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    إذا(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      اكسر;
    }
    وإلا إذا(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      اذهب out;
    }
    وإلا إذا(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      اذهب out;
    }
    وإلا إذا(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      اكسر;
    }
    وإلا {
      *pnread += n;
    }
  }

  إذا(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (صحيح)result, *pnread);
  أرجع result;
}

ساكن CURLcode cr_flush_out(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                             بنية rustls_connection *rconn)
{
  بنية io_ctx io_ctx;
  rustls_io_result io_error;
  size_t tlswritten = 0;
  size_t tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  طالما(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    إذا(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      أرجع CURLE_AGAIN;
    }
    وإلا إذا(io_error) {
      حرف buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, حجم(buffer)));
      أرجع CURLE_SEND_ERROR;
    }
    إذا(tlswritten == 0) {
      failf(data, "EOF in swrite");
      أرجع CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  أرجع CURLE_OK;
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
ساكن CURLcode cr_send(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                        ثابت فراغ *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ثابت بنية ssl_connect_data * ثابت connssl = cf->ctx;
  بنية rustls_ssl_backend_data * ثابت backend =
    (بنية rustls_ssl_backend_data *)connssl->backend;
  بنية rustls_connection *rconn = عدم;
  size_t plainwritten = 0;
  ثابت unsigned حرف *buf = plainbuf;
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
  إذا(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (صحيح)result);
    إذا(result)
      أرجع result;
    إذا(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    وإلا
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  إذا(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    إذا(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      اذهب out;
    }
    وإلا إذا(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      اذهب out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  إذا(result) {
    إذا(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      إذا(*pnwritten) {
        result = CURLE_OK;
      }
    }
    اذهب out;
  }
  وإلا
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (صحيح)result, *pnwritten);
  أرجع result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
ساكن uint32_t cr_verify_none(فراغ *userdata,
                               ثابت rustls_verify_server_cert_params *params)
{
  (فراغ)userdata;
  (فراغ)params;
  أرجع RUSTLS_RESULT_OK;
}

ساكن صحيح read_file_into(ثابت حرف *filename, بنية dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  إذا(!f) {
    أرجع 0;
  }

  لكل(;;) {
    uint8_t buf[256];
    ثابت size_t rr = fread(buf, 1, حجم(buf), f);
    إذا((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      أرجع 0;
    }
    إذا(rr < حجم(buf))
      اكسر;
  }

  أرجع curlx_fclose(f) == 0;
}

ساكن فراغ
cr_get_selected_ciphers(بنية Curl_easy *data,
                        ثابت حرف *ciphers12,
                        ثابت حرف *ciphers13,
                        ثابت بنية rustls_supported_ciphersuite **selected,
                        size_t *selected_size)
{
  ثابت size_t supported_len = *selected_size;
  ثابت size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ثابت بنية rustls_supported_ciphersuite *entry = عدم;
  ثابت حرف *ciphers = ciphers12;
  size_t کيراءن = 0, default13_کيراءن = 0, i, j;
  ثابت حرف *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  إذا(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    لكل(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      إذا(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        استمر;

      selected[کيراءن++] = entry;
    }

    default13_کيراءن = کيراءن;

    إذا(!ciphers)
      ciphers = "";
  }
  وإلا
    ciphers = ciphers13;

add_ciphers:
  لكل(ptr = ciphers; ptr[0] != '\0' && کيراءن < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    إذا(id) {
      لكل(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        إذا(rustls_supported_ciphersuite_get_suite(entry) == id)
          اكسر;
      }
      إذا(i == supported_len)
        id = 0;
    }
    إذا(!id) {
      إذا(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (صحيح)(end - ptr), ptr);
      استمر;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    لكل(i = 0; i < کيراءن && selected[i] != entry; i++)
      ;
    إذا(i < کيراءن) {
      إذا(i >= default13_کيراءن)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (صحيح)(end - ptr), ptr);
      استمر;
    }

    selected[کيراءن++] = entry;
  }

  إذا(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    اذهب add_ciphers;
  }

  إذا(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    لكل(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      إذا(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        استمر;

      /* No duplicates allowed (so selected cannot overflow) */
      لكل(i = 0; i < کيراءن && selected[i] != entry; i++)
        ;
      إذا(i < کيراءن)
        استمر;

      selected[کيراءن++] = entry;
    }
  }

  *selected_size = کيراءن;
}

ساكن فراغ cr_keylog_log_cb(بنية rustls_str label,
                             ثابت uint8_t *client_random,
                             size_t client_random_len, ثابت uint8_t *secret,
                             size_t secret_len)
{
  حرف clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, حجم(clabel), "%.*s", (صحيح)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

ساكن CURLcode
init_config_builder(بنية Curl_cfilter *cf,
                    بنية Curl_easy *data,
                    ثابت بنية ssl_filter_config *conn_config,
                    بنية rustls_client_config_builder **config_builder)
{
  ثابت بنية rustls_supported_ciphersuite **cipher_suites = عدم;
  بنية rustls_crypto_provider_builder *custom_provider_builder = عدم;
  ثابت بنية rustls_crypto_provider *custom_provider = عدم;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  بدل(conn_config->version) {
  حالة CURL_SSLVERSION_TLSv1:
  حالة CURL_SSLVERSION_TLSv1_0:
  حالة CURL_SSLVERSION_TLSv1_1:
  حالة CURL_SSLVERSION_TLSv1_2:
    اكسر;
  حالة CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    اكسر;
  افتراضي:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب cleanup;
  }

  بدل(conn_config->version_max) {
  حالة CURL_SSLVERSION_MAX_DEFAULT:
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_TLSv1_3:
    اكسر;
  حالة CURL_SSLVERSION_MAX_TLSv1_2:
    إذا(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      اكسر;
    }
    FALLTHROUGH();
  حالة CURL_SSLVERSION_MAX_TLSv1_1:
  حالة CURL_SSLVERSION_MAX_TLSv1_0:
  افتراضي:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب cleanup;
  }

#ifdef USE_ECH
  إذا(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#وإلا
  (فراغ)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(حجم(*cipher_suites) * cipher_suites_len);
  إذا(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  إذا(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

cleanup:
  إذا(cipher_suites) {
    curlx_free(cipher_suites);
  }
  إذا(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  إذا(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  أرجع result;
}

ساكن فراغ
init_config_builder_alpn(بنية Curl_easy *data,
                         ثابت بنية ssl_connect_data *connssl,
                         بنية rustls_client_config_builder *config_builder)
{
  بنية alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  لكل(i = 0; i < connssl->alpn->کيراءن; ++i) {
    alpn[i].data = (ثابت uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->کيراءن);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

ساكن CURLcode init_config_builder_verifier_crl(
  بنية Curl_easy *data,
  ثابت بنية ssl_filter_config *conn_config,
  بنية rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  بنية dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  إذا(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    اذهب cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    اذهب cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  أرجع result;
}

ساكن CURLcode
init_config_builder_verifier(بنية Curl_easy *data,
                             بنية rustls_client_config_builder *builder,
                             ثابت بنية ssl_filter_config *conn_config,
                             ثابت بنية curl_blob *ca_info_blob,
                             ثابت حرف * ثابت ssl_cafile)
{
  ثابت بنية rustls_اکر_cert_store *roots = عدم;
  بنية rustls_اکر_cert_store_builder *roots_builder = عدم;
  بنية rustls_web_pki_server_cert_verifier_builder *verifier_builder = عدم;
  بنية rustls_server_cert_verifier *server_cert_verifier = عدم;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_اکر_cert_store_builder_new();
  إذا(ca_info_blob) {
    rr = rustls_اکر_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    إذا(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      اذهب cleanup;
    }
  }
  وإلا إذا(ssl_cafile) {
    rr = rustls_اکر_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    إذا(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      اذهب cleanup;
    }
  }

  rr = rustls_اکر_cert_store_builder_build(roots_builder, &roots);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  إذا(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب cleanup;
  }

  إذا(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    إذا(result) {
      اذهب cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  إذا(roots_builder) {
    rustls_اکر_cert_store_builder_free(roots_builder);
  }
  إذا(roots) {
    rustls_اکر_cert_store_free(roots);
  }
  إذا(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  إذا(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  أرجع result;
}

ساكن CURLcode init_config_builder_platform_verifier(
  بنية Curl_easy *data,
  بنية rustls_client_config_builder *builder)
{
  بنية rustls_server_cert_verifier *server_cert_verifier = عدم;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  إذا(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  أرجع result;
}

ساكن CURLcode
init_config_builder_keylog(بنية Curl_easy *data,
                           بنية rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  إذا(!Curl_tls_keylog_enabled()) {
    أرجع CURLE_OK;
  }

  rr = rustls_client_config_builder_set_کونچي_log(builder,
                                                cr_keylog_log_cb,
                                                عدم);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    أرجع map_error(rr);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode
init_config_builder_client_auth(بنية Curl_easy *data,
                                ثابت بنية ssl_filter_config *conn_config,
                                بنية rustls_client_config_builder *builder)
{
  بنية dynbuf cert_contents;
  بنية dynbuf کونچي_contents;
  rustls_result rr;
  ثابت بنية rustls_certified_کونچي *certified_کونچي = عدم;
  CURLcode result = CURLE_OK;
  ثابت bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ثابت bool have_کونچي = conn_config->کونچي || conn_config->کونچي_blob;

  إذا(have_cert && !have_کونچي) {
    failf(data, "rustls: must provide key with certificate");
    أرجع CURLE_SSL_CERTPROBLEM;
  }
  وإلا إذا(!have_cert && have_کونچي) {
    failf(data, "rustls: must provide certificate with key");
    أرجع CURLE_SSL_CERTPROBLEM;
  }

  إذا(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    أرجع CURLE_SSL_CERTPROBLEM;
  }
  إذا(conn_config->کونچي_type && !curl_strequal(conn_config->کونچي_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->کونچي_type);
    أرجع CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&کونچي_contents, DYN_KEYFILE_SIZE);

  إذا(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    إذا(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      اذهب cleanup;
    }
  }
  وإلا إذا(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  إذا(conn_config->کونچي_blob) {
    result = curlx_dyn_addn(&کونچي_contents,
                            conn_config->کونچي_blob->data,
                            conn_config->کونچي_blob->len);
    إذا(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      اذهب cleanup;
    }
  }
  وإلا إذا(!read_file_into(conn_config->کونچي, &کونچي_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->کونچي);
    result = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  rr = rustls_certified_کونچي_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&کونچي_contents),
                                  curlx_dyn_len(&کونچي_contents),
                                  &certified_کونچي);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  rr = rustls_certified_کونچي_کونچي_سموا_match(certified_کونچي);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  rr = rustls_client_config_builder_set_certified_کونچي(builder,
                                                      &certified_کونچي,
                                                      1);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&کونچي_contents);
  إذا(certified_کونچي) {
    rustls_certified_کونچي_free(certified_کونچي);
  }
  أرجع result;
}

#ifdef USE_ECH

ساكن CURLcode
init_config_builder_ech(بنية Curl_easy *data,
                        بنية Curl_cfilter *cf,
                        بنية rustls_client_config_builder *builder)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت rustls_hpke *hpke = rustls_supported_hpke();
  unsigned حرف *ech_config = عدم;
  size_t ech_config_len = 0;
  بنية Curl_dns_entry *dns = عدم;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  إذا(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    اذهب cleanup;
  }

  إذا(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    اذهب cleanup;
  }

  إذا(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    إذا(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
    أرجع CURLE_OK;
  }

  إذا(conn_config->ech && conn_config->ech_config) {
    ثابت حرف *b64 = conn_config->ech_config;
    size_t decode_result;
    إذا(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    إذا(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
  }
  وإلا {
    ثابت بنية ssl_connect_data *connssl = cf->ctx;
    ثابت بنية Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    إذا(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    اذهب cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  إذا(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  إذا(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  أرجع result;
}
#endif /* USE_ECH */

ساكن CURLcode cr_init_backend(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                بنية rustls_ssl_backend_data * ثابت backend)
{
  ثابت بنية ssl_connect_data *connssl = cf->ctx;
  ثابت بنية ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  بنية rustls_connection *rconn = عدم;
  بنية rustls_client_config_builder *config_builder = عدم;

  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? عدم : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  إذا(result != CURLE_OK) {
    أرجع result;
  }

  إذا(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  إذا(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  وإلا إذا(conn_config->native_ca_store) {
    إذا(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      أرجع CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    إذا(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      أرجع result;
    }
  }
  وإلا إذا(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    إذا(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      أرجع result;
    }
  }

  إذا(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->کونچي || conn_config->کونچي_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    إذا(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      أرجع result;
    }
  }

#ifdef USE_ECH
  إذا(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    إذا((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      أرجع result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  إذا(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    أرجع result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  إذا(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = عدم;
    أرجع CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  أرجع result;
}

ساكن فراغ cr_set_negotiated_alpn(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data,
                                   ثابت بنية rustls_connection *rconn)
{
  بنية ssl_connect_data * ثابت connssl = cf->ctx;
  ثابت uint8_t *protocol = عدم;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
ساكن CURLcode cr_connect(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                           bool *done)
{
  بنية ssl_connect_data * ثابت connssl = cf->ctx;
  ثابت بنية rustls_ssl_backend_data * ثابت backend =
    (بنية rustls_ssl_backend_data *)connssl->backend;
  ثابت بنية rustls_connection *rconn = عدم;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (صحيح)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    إذا(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      أرجع CURLE_OK;
    }
#endif /* USE_ECH */

  إذا(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (بنية rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (صحيح)result);
    إذا(result)
      أرجع result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  لكل(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    إذا(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, عدم, 0, &nwritten);
      إذا(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        أرجع CURLE_OK;
      }
      وإلا إذا(tmperr != CURLE_OK) {
        أرجع tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        ثابت uint16_t proto = rustls_connection_get_protocol_version(rconn);
        ثابت rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ثابت rustls_str kex_group_name =
          rustls_connection_get_negotiated_کونچي_exchange_group_name(rconn);
        ثابت حرف *ver = "TLS version unknown";
        إذا(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        إذا(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (صحيح)ciphersuite_name.len,
              ciphersuite_name.data,
              (صحيح)kex_group_name.len,
              kex_group_name.data);
      }
      إذا(data->set.ssl.certinfo) {
        size_t اڠک_certs = 0;
        size_t i;
        طالما(rustls_connection_get_peer_certificate(rconn, اڠک_certs)) {
          اڠک_certs++;
          إذا(اڠک_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  اڠک_certs, MAX_ALLOWED_CERT_AMOUNT);
            أرجع CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (صحيح)اڠک_certs);
        إذا(result)
          أرجع result;
        لكل(i = 0; i < اڠک_certs; i++) {
          ثابت rustls_certificate *cert;
          ثابت unsigned حرف *der_data;
          size_t der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          إذا(rresult != RUSTLS_RESULT_OK) {
            حرف errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, حجم(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (صحيح)errorlen, errorbuf);
            أرجع map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (صحيح)i,
                                         der_data, der_data + der_len);
          إذا(result)
            أرجع result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      أرجع CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    إذا(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, عدم, 0, &nwritten);
      إذا(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        أرجع CURLE_OK;
      }
      وإلا إذا(tmperr != CURLE_OK) {
        أرجع tmperr;
      }
    }

    إذا(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      إذا(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
      إذا(nread < 0) {
        إذا(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          أرجع CURLE_OK;
        }
        وإلا إذا(tmperr == CURLE_RECV_ERROR) {
          أرجع CURLE_SSL_CONNECT_ERROR;
        }
        وإلا {
          أرجع tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

ساكن فراغ *cr_get_internals(بنية ssl_connect_data *connssl, CURLINFO info)
{
  بنية rustls_ssl_backend_data *backend =
    (بنية rustls_ssl_backend_data *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع backend->conn;
}

ساكن CURLcode cr_shutdown(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                            ثابت bool send_shutdown, bool *done)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية rustls_ssl_backend_data *backend =
    (بنية rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  إذا(!backend->conn || cf->shutdown) {
    *done = TRUE;
    اذهب out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  إذا(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    إذا(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, عدم, 0, &nwritten);
  إذا(result) {
    إذا(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      اذهب out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (صحيح)result);
    اذهب out;
  }

  لكل(i = 0; i < 10; ++i) {
    حرف buf[1024];
    result = cr_recv(cf, data, buf, (صحيح)حجم(buf), &nread);
    إذا(result)
      اكسر;
  }

  إذا(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  وإلا إذا(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (صحيح)result);
  }
  وإلا إذا(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  أرجع result;
}

ساكن فراغ cr_close(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  ثابت بنية ssl_connect_data *connssl = cf->ctx;
  بنية rustls_ssl_backend_data *backend =
    (بنية rustls_ssl_backend_data *)connssl->backend;

  (فراغ)data;
  DEBUGASSERT(backend);
  إذا(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = عدم;
  }
  إذا(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = عدم;
  }
}

ساكن size_t cr_version(حرف *buffer, size_t size)
{
  ثابت بنية rustls_str ver = rustls_version();
  أرجع curl_msnprintf(buffer, size, "%.*s", (صحيح)ver.len, ver.data);
}

ساكن CURLcode cr_random(بنية Curl_easy *data, unsigned حرف *entropy,
                          size_t length)
{
  rustls_result rresult = 0;
  (فراغ)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  أرجع map_error(rresult);
}

ساكن فراغ cr_cleanup(فراغ)
{
  Curl_tls_keylog_close();
}

ثابت بنية Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  حجم(بنية rustls_ssl_backend_data),

  عدم,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
  cr_random,                       /* random */
  عدم,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  عدم,                            /* close_all */
  عدم,                            /* engines_list */
  عدم,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  عدم,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
