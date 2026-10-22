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
#شامل "curl_setup.h"

#ifdef USE_RUSTLS

#شامل <rustls.h>

#شامل "curlx/win32-fopen.h"
#شامل "curlx/strerr.h"
#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "vdns/cf-dns.h"
#شامل "vdns/httpsrr.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_int.h"
#شامل "vtls/rustls.h"
#شامل "vtls/keylog.h"
#شامل "vtls/cipher_suite.h"
#شامل "vtls/x509asn1.h"
#ifdef USE_ECH
#شامل "curlx/base64.h"
#endif

#اگر EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#وگرنه
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

ساختار rustls_ssl_backend_data {
  ثابت ساختار rustls_client_config *config;
  ساختار rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
ایستا CURLcode map_error(ثابت rustls_result r)
{
  اگر(rustls_result_is_cert_error(r)) {
    بازگشت CURLE_PEER_FAILED_VERIFICATION;
  }
  انتخاب(r) {
  حالت RUSTLS_RESULT_OK:
    بازگشت CURLE_OK;
  حالت RUSTLS_RESULT_NULL_PARAMETER:
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  پیشفرض:
    بازگشت CURLE_RECV_ERROR;
  }
}

ایستا تهی rustls_failf(ساختار Curl_easy *data, ثابت rustls_result rr,
                         ثابت نویسه *پیام)
{
  نویسه errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, اندازه(errorbuf), &errorlen);
  failf(data, "%s: %.*s", پیام, (صحیح)errorlen, errorbuf);
}

ایستا bool cr_data_pending(ساختار Curl_cfilter *cf,
                            ثابت ساختار Curl_easy *data)
{
  ثابت ساختار ssl_connect_data *ctx = cf->ctx;
  ساختار rustls_ssl_backend_data *backend;

  (تهی)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ساختار rustls_ssl_backend_data *)ctx->backend;
  بازگشت (bool)backend->data_in_pending;
}

ساختار io_ctx {
  ساختار Curl_cfilter *cf;
  ساختار Curl_easy *data;
};

ایستا صحیح read_cb(تهی *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  ثابت ساختار io_ctx *io_ctx = userdata;
  ساختار ssl_connect_data * ثابت connssl = io_ctx->cf->ctx;
  CURLcode result;
  صحیح ret = 0;
  size_t nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (نویسه *)buf, len, &nread);
  اگر(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    اگر(result == CURLE_AGAIN)
      ret = EAGAIN;
    وگرنه
      ret = EINVAL;
  }
  وگرنه اگر(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (صحیح)result, nread);
  بازگشت ret;
}

ایستا صحیح write_cb(تهی *userdata, ثابت uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  ثابت ساختار io_ctx *io_ctx = userdata;
  CURLcode result;
  صحیح ret = 0;
  size_t nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  اگر(result) {
    nwritten = 0;
    اگر(result == CURLE_AGAIN)
      ret = EAGAIN;
    وگرنه
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (صحیح)result, nwritten);
  بازگشت ret;
}

ایستا ssize_t tls_recv_more(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data, CURLcode *err)
{
  ثابت ساختار ssl_connect_data * ثابت connssl = cf->ctx;
  ساختار rustls_ssl_backend_data * ثابت backend =
    (ساختار rustls_ssl_backend_data *)connssl->backend;
  ساختار io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  اگر(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    بازگشت -1;
  }
  وگرنه اگر(io_error) {
    نویسه buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, اندازه(buffer)));
    *err = CURLE_RECV_ERROR;
    بازگشت -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  اگر(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    بازگشت -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  بازگشت (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
ایستا CURLcode cr_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                        نویسه *plainbuf, size_t plainlen, size_t *pnread)
{
  ثابت ساختار ssl_connect_data * ثابت connssl = cf->ctx;
  ساختار rustls_ssl_backend_data * ثابت backend =
    (ساختار rustls_ssl_backend_data *)connssl->backend;
  ساختار rustls_connection *rconn = پوچ;
  CURLcode result = CURLE_OK;
  size_t n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  وقتی(*pnread < plainlen) {
    اگر(!backend->data_in_pending) {
      اگر(tls_recv_more(cf, data, &result) < 0) {
        اگر(result != CURLE_AGAIN) {
          برو out;
        }
        result = CURLE_OK;
        بشکن;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    اگر(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      بشکن;
    }
    وگرنه اگر(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      برو out;
    }
    وگرنه اگر(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      برو out;
    }
    وگرنه اگر(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      بشکن;
    }
    وگرنه {
      *pnread += n;
    }
  }

  اگر(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (صحیح)result, *pnread);
  بازگشت result;
}

ایستا CURLcode cr_flush_out(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                             ساختار rustls_connection *rconn)
{
  ساختار io_ctx io_ctx;
  rustls_io_result io_error;
  size_t tlswritten = 0;
  size_t tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  وقتی(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    اگر(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      بازگشت CURLE_AGAIN;
    }
    وگرنه اگر(io_error) {
      نویسه buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, اندازه(buffer)));
      بازگشت CURLE_SEND_ERROR;
    }
    اگر(tlswritten == 0) {
      failf(data, "EOF in swrite");
      بازگشت CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  بازگشت CURLE_OK;
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
ایستا CURLcode cr_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                        ثابت تهی *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ثابت ساختار ssl_connect_data * ثابت connssl = cf->ctx;
  ساختار rustls_ssl_backend_data * ثابت backend =
    (ساختار rustls_ssl_backend_data *)connssl->backend;
  ساختار rustls_connection *rconn = پوچ;
  size_t plainwritten = 0;
  ثابت unsigned نویسه *buf = plainbuf;
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
  اگر(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (صحیح)result);
    اگر(result)
      بازگشت result;
    اگر(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    وگرنه
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  اگر(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    اگر(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      برو out;
    }
    وگرنه اگر(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      برو out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  اگر(result) {
    اگر(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      اگر(*pnwritten) {
        result = CURLE_OK;
      }
    }
    برو out;
  }
  وگرنه
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (صحیح)result, *pnwritten);
  بازگشت result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
ایستا uint32_t cr_verify_none(تهی *userdata,
                               ثابت rustls_verify_server_cert_params *params)
{
  (تهی)userdata;
  (تهی)params;
  بازگشت RUSTLS_RESULT_OK;
}

ایستا صحیح read_file_into(ثابت نویسه *filename, ساختار dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  اگر(!f) {
    بازگشت 0;
  }

  برای(;;) {
    uint8_t buf[256];
    ثابت size_t rr = fread(buf, 1, اندازه(buf), f);
    اگر((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      بازگشت 0;
    }
    اگر(rr < اندازه(buf))
      بشکن;
  }

  بازگشت curlx_fclose(f) == 0;
}

ایستا تهی
cr_get_selected_ciphers(ساختار Curl_easy *data,
                        ثابت نویسه *ciphers12,
                        ثابت نویسه *ciphers13,
                        ثابت ساختار rustls_supported_ciphersuite **selected,
                        size_t *selected_size)
{
  ثابت size_t supported_len = *selected_size;
  ثابت size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ثابت ساختار rustls_supported_ciphersuite *entry = پوچ;
  ثابت نویسه *ciphers = ciphers12;
  size_t شمار = 0, default13_شمار = 0, i, j;
  ثابت نویسه *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  اگر(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    برای(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      اگر(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        ادامه;

      selected[شمار++] = entry;
    }

    default13_شمار = شمار;

    اگر(!ciphers)
      ciphers = "";
  }
  وگرنه
    ciphers = ciphers13;

add_ciphers:
  برای(ptr = ciphers; ptr[0] != '\0' && شمار < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    اگر(id) {
      برای(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        اگر(rustls_supported_ciphersuite_get_suite(entry) == id)
          بشکن;
      }
      اگر(i == supported_len)
        id = 0;
    }
    اگر(!id) {
      اگر(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (صحیح)(end - ptr), ptr);
      ادامه;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    برای(i = 0; i < شمار && selected[i] != entry; i++)
      ;
    اگر(i < شمار) {
      اگر(i >= default13_شمار)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (صحیح)(end - ptr), ptr);
      ادامه;
    }

    selected[شمار++] = entry;
  }

  اگر(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    برو add_ciphers;
  }

  اگر(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    برای(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      اگر(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        ادامه;

      /* No duplicates allowed (so selected cannot overflow) */
      برای(i = 0; i < شمار && selected[i] != entry; i++)
        ;
      اگر(i < شمار)
        ادامه;

      selected[شمار++] = entry;
    }
  }

  *selected_size = شمار;
}

ایستا تهی cr_keylog_log_cb(ساختار rustls_str label,
                             ثابت uint8_t *client_random,
                             size_t client_random_len, ثابت uint8_t *secret,
                             size_t secret_len)
{
  نویسه clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, اندازه(clabel), "%.*s", (صحیح)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

ایستا CURLcode
init_config_builder(ساختار Curl_cfilter *cf,
                    ساختار Curl_easy *data,
                    ثابت ساختار ssl_filter_config *conn_config,
                    ساختار rustls_client_config_builder **config_builder)
{
  ثابت ساختار rustls_supported_ciphersuite **cipher_suites = پوچ;
  ساختار rustls_crypto_provider_builder *custom_provider_builder = پوچ;
  ثابت ساختار rustls_crypto_provider *custom_provider = پوچ;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  انتخاب(conn_config->version) {
  حالت CURL_SSLVERSION_TLSv1:
  حالت CURL_SSLVERSION_TLSv1_0:
  حالت CURL_SSLVERSION_TLSv1_1:
  حالت CURL_SSLVERSION_TLSv1_2:
    بشکن;
  حالت CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    بشکن;
  پیشفرض:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو cleanup;
  }

  انتخاب(conn_config->version_max) {
  حالت CURL_SSLVERSION_MAX_DEFAULT:
  حالت CURL_SSLVERSION_MAX_NONE:
  حالت CURL_SSLVERSION_MAX_TLSv1_3:
    بشکن;
  حالت CURL_SSLVERSION_MAX_TLSv1_2:
    اگر(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      بشکن;
    }
    FALLTHROUGH();
  حالت CURL_SSLVERSION_MAX_TLSv1_1:
  حالت CURL_SSLVERSION_MAX_TLSv1_0:
  پیشفرض:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو cleanup;
  }

#ifdef USE_ECH
  اگر(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#وگرنه
  (تهی)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(اندازه(*cipher_suites) * cipher_suites_len);
  اگر(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    برو cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  اگر(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    برو cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    برو cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    برو cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    برو cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
    برو cleanup;
  }

cleanup:
  اگر(cipher_suites) {
    curlx_free(cipher_suites);
  }
  اگر(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  اگر(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  بازگشت result;
}

ایستا تهی
init_config_builder_alpn(ساختار Curl_easy *data,
                         ثابت ساختار ssl_connect_data *connssl,
                         ساختار rustls_client_config_builder *config_builder)
{
  ساختار alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  برای(i = 0; i < connssl->alpn->شمار; ++i) {
    alpn[i].data = (ثابت uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->شمار);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

ایستا CURLcode init_config_builder_verifier_crl(
  ساختار Curl_easy *data,
  ثابت ساختار ssl_filter_config *conn_config,
  ساختار rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  ساختار dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  اگر(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    برو cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    برو cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  بازگشت result;
}

ایستا CURLcode
init_config_builder_verifier(ساختار Curl_easy *data,
                             ساختار rustls_client_config_builder *builder,
                             ثابت ساختار ssl_filter_config *conn_config,
                             ثابت ساختار curl_blob *ca_info_blob,
                             ثابت نویسه * ثابت ssl_cafile)
{
  ثابت ساختار rustls_ریشه_cert_store *roots = پوچ;
  ساختار rustls_ریشه_cert_store_builder *roots_builder = پوچ;
  ساختار rustls_web_pki_server_cert_verifier_builder *verifier_builder = پوچ;
  ساختار rustls_server_cert_verifier *server_cert_verifier = پوچ;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_ریشه_cert_store_builder_new();
  اگر(ca_info_blob) {
    rr = rustls_ریشه_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    اگر(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      برو cleanup;
    }
  }
  وگرنه اگر(ssl_cafile) {
    rr = rustls_ریشه_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    اگر(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      برو cleanup;
    }
  }

  rr = rustls_ریشه_cert_store_builder_build(roots_builder, &roots);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    برو cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  اگر(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    برو cleanup;
  }

  اگر(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    اگر(result) {
      برو cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    برو cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  اگر(roots_builder) {
    rustls_ریشه_cert_store_builder_free(roots_builder);
  }
  اگر(roots) {
    rustls_ریشه_cert_store_free(roots);
  }
  اگر(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  اگر(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  بازگشت result;
}

ایستا CURLcode init_config_builder_platform_verifier(
  ساختار Curl_easy *data,
  ساختار rustls_client_config_builder *builder)
{
  ساختار rustls_server_cert_verifier *server_cert_verifier = پوچ;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    برو cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  اگر(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  بازگشت result;
}

ایستا CURLcode
init_config_builder_keylog(ساختار Curl_easy *data,
                           ساختار rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  اگر(!Curl_tls_keylog_enabled()) {
    بازگشت CURLE_OK;
  }

  rr = rustls_client_config_builder_set_کلید_log(builder,
                                                cr_keylog_log_cb,
                                                پوچ);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    بازگشت map_error(rr);
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode
init_config_builder_client_auth(ساختار Curl_easy *data,
                                ثابت ساختار ssl_filter_config *conn_config,
                                ساختار rustls_client_config_builder *builder)
{
  ساختار dynbuf cert_contents;
  ساختار dynbuf کلید_contents;
  rustls_result rr;
  ثابت ساختار rustls_certified_کلید *certified_کلید = پوچ;
  CURLcode result = CURLE_OK;
  ثابت bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ثابت bool have_کلید = conn_config->کلید || conn_config->کلید_blob;

  اگر(have_cert && !have_کلید) {
    failf(data, "rustls: must provide key with certificate");
    بازگشت CURLE_SSL_CERTPROBLEM;
  }
  وگرنه اگر(!have_cert && have_کلید) {
    failf(data, "rustls: must provide certificate with key");
    بازگشت CURLE_SSL_CERTPROBLEM;
  }

  اگر(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    بازگشت CURLE_SSL_CERTPROBLEM;
  }
  اگر(conn_config->کلید_type && !curl_strequal(conn_config->کلید_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->کلید_type);
    بازگشت CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&کلید_contents, DYN_KEYFILE_SIZE);

  اگر(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    اگر(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      برو cleanup;
    }
  }
  وگرنه اگر(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    برو cleanup;
  }

  اگر(conn_config->کلید_blob) {
    result = curlx_dyn_addn(&کلید_contents,
                            conn_config->کلید_blob->data,
                            conn_config->کلید_blob->len);
    اگر(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      برو cleanup;
    }
  }
  وگرنه اگر(!read_file_into(conn_config->کلید, &کلید_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->کلید);
    result = CURLE_SSL_CERTPROBLEM;
    برو cleanup;
  }

  rr = rustls_certified_کلید_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&کلید_contents),
                                  curlx_dyn_len(&کلید_contents),
                                  &certified_کلید);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    برو cleanup;
  }

  rr = rustls_certified_کلید_کلیدها_match(certified_کلید);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    برو cleanup;
  }

  rr = rustls_client_config_builder_set_certified_کلید(builder,
                                                      &certified_کلید,
                                                      1);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    برو cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&کلید_contents);
  اگر(certified_کلید) {
    rustls_certified_کلید_free(certified_کلید);
  }
  بازگشت result;
}

#ifdef USE_ECH

ایستا CURLcode
init_config_builder_ech(ساختار Curl_easy *data,
                        ساختار Curl_cfilter *cf,
                        ساختار rustls_client_config_builder *builder)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت rustls_hpke *hpke = rustls_supported_hpke();
  unsigned نویسه *ech_config = پوچ;
  size_t ech_config_len = 0;
  ساختار Curl_dns_entry *dns = پوچ;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  اگر(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    برو cleanup;
  }

  اگر(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    برو cleanup;
  }

  اگر(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    اگر(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      برو cleanup;
    }
    بازگشت CURLE_OK;
  }

  اگر(conn_config->ech && conn_config->ech_config) {
    ثابت نویسه *b64 = conn_config->ech_config;
    size_t decode_result;
    اگر(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      برو cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    اگر(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      برو cleanup;
    }
  }
  وگرنه {
    ثابت ساختار ssl_connect_data *connssl = cf->ctx;
    ثابت ساختار Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    اگر(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      برو cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    برو cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  اگر(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  اگر(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  بازگشت result;
}
#endif /* USE_ECH */

ایستا CURLcode cr_init_backend(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                ساختار rustls_ssl_backend_data * ثابت backend)
{
  ثابت ساختار ssl_connect_data *connssl = cf->ctx;
  ثابت ساختار ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  ساختار rustls_connection *rconn = پوچ;
  ساختار rustls_client_config_builder *config_builder = پوچ;

  ثابت ساختار curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت نویسه * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? پوچ : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  اگر(result != CURLE_OK) {
    بازگشت result;
  }

  اگر(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  اگر(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  وگرنه اگر(conn_config->native_ca_store) {
    اگر(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      بازگشت CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    اگر(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      بازگشت result;
    }
  }
  وگرنه اگر(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    اگر(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      بازگشت result;
    }
  }

  اگر(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->کلید || conn_config->کلید_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    اگر(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      بازگشت result;
    }
  }

#ifdef USE_ECH
  اگر(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    اگر((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      بازگشت result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  اگر(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    بازگشت result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  اگر(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = پوچ;
    بازگشت CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  بازگشت result;
}

ایستا تهی cr_set_negotiated_alpn(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data,
                                   ثابت ساختار rustls_connection *rconn)
{
  ساختار ssl_connect_data * ثابت connssl = cf->ctx;
  ثابت uint8_t *protocol = پوچ;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
ایستا CURLcode cr_connect(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                           bool *done)
{
  ساختار ssl_connect_data * ثابت connssl = cf->ctx;
  ثابت ساختار rustls_ssl_backend_data * ثابت backend =
    (ساختار rustls_ssl_backend_data *)connssl->backend;
  ثابت ساختار rustls_connection *rconn = پوچ;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (صحیح)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    اگر(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      بازگشت CURLE_OK;
    }
#endif /* USE_ECH */

  اگر(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (ساختار rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (صحیح)result);
    اگر(result)
      بازگشت result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  برای(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    اگر(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, پوچ, 0, &nwritten);
      اگر(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        بازگشت CURLE_OK;
      }
      وگرنه اگر(tmperr != CURLE_OK) {
        بازگشت tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        ثابت uint16_t proto = rustls_connection_get_protocol_version(rconn);
        ثابت rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ثابت rustls_str kex_group_name =
          rustls_connection_get_negotiated_کلید_exchange_group_name(rconn);
        ثابت نویسه *ver = "TLS version unknown";
        اگر(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        اگر(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (صحیح)ciphersuite_name.len,
              ciphersuite_name.data,
              (صحیح)kex_group_name.len,
              kex_group_name.data);
      }
      اگر(data->set.ssl.certinfo) {
        size_t رقم_certs = 0;
        size_t i;
        وقتی(rustls_connection_get_peer_certificate(rconn, رقم_certs)) {
          رقم_certs++;
          اگر(رقم_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  رقم_certs, MAX_ALLOWED_CERT_AMOUNT);
            بازگشت CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (صحیح)رقم_certs);
        اگر(result)
          بازگشت result;
        برای(i = 0; i < رقم_certs; i++) {
          ثابت rustls_certificate *cert;
          ثابت unsigned نویسه *der_data;
          size_t der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          اگر(rresult != RUSTLS_RESULT_OK) {
            نویسه errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, اندازه(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (صحیح)errorlen, errorbuf);
            بازگشت map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (صحیح)i,
                                         der_data, der_data + der_len);
          اگر(result)
            بازگشت result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      بازگشت CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    اگر(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, پوچ, 0, &nwritten);
      اگر(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        بازگشت CURLE_OK;
      }
      وگرنه اگر(tmperr != CURLE_OK) {
        بازگشت tmperr;
      }
    }

    اگر(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      اگر(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        بازگشت CURLE_SSL_CONNECT_ERROR;
      }
      اگر(nread < 0) {
        اگر(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          بازگشت CURLE_OK;
        }
        وگرنه اگر(tmperr == CURLE_RECV_ERROR) {
          بازگشت CURLE_SSL_CONNECT_ERROR;
        }
        وگرنه {
          بازگشت tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

ایستا تهی *cr_get_internals(ساختار ssl_connect_data *connssl, CURLINFO info)
{
  ساختار rustls_ssl_backend_data *backend =
    (ساختار rustls_ssl_backend_data *)connssl->backend;
  (تهی)info;
  DEBUGASSERT(backend);
  بازگشت backend->conn;
}

ایستا CURLcode cr_shutdown(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                            ثابت bool send_shutdown, bool *done)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار rustls_ssl_backend_data *backend =
    (ساختار rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  اگر(!backend->conn || cf->shutdown) {
    *done = TRUE;
    برو out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  اگر(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    اگر(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, پوچ, 0, &nwritten);
  اگر(result) {
    اگر(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      برو out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (صحیح)result);
    برو out;
  }

  برای(i = 0; i < 10; ++i) {
    نویسه buf[1024];
    result = cr_recv(cf, data, buf, (صحیح)اندازه(buf), &nread);
    اگر(result)
      بشکن;
  }

  اگر(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  وگرنه اگر(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (صحیح)result);
  }
  وگرنه اگر(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  بازگشت result;
}

ایستا تهی cr_close(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ثابت ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار rustls_ssl_backend_data *backend =
    (ساختار rustls_ssl_backend_data *)connssl->backend;

  (تهی)data;
  DEBUGASSERT(backend);
  اگر(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = پوچ;
  }
  اگر(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = پوچ;
  }
}

ایستا size_t cr_version(نویسه *buffer, size_t size)
{
  ثابت ساختار rustls_str ver = rustls_version();
  بازگشت curl_msnprintf(buffer, size, "%.*s", (صحیح)ver.len, ver.data);
}

ایستا CURLcode cr_random(ساختار Curl_easy *data, unsigned نویسه *entropy,
                          size_t length)
{
  rustls_result rresult = 0;
  (تهی)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  بازگشت map_error(rresult);
}

ایستا تهی cr_cleanup(تهی)
{
  Curl_tls_keylog_close();
}

ثابت ساختار Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  اندازه(ساختار rustls_ssl_backend_data),

  پوچ,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
  cr_random,                       /* random */
  پوچ,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  پوچ,                            /* close_all */
  پوچ,                            /* engines_list */
  پوچ,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  پوچ,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
