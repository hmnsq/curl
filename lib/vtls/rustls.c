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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_RUSTLS

#அந்தர்பாவ <rustls.h>

#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "curlx/strerr.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "vdns/httpsrr.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_int.h"
#அந்தர்பாவ "vtls/rustls.h"
#அந்தர்பாவ "vtls/keylog.h"
#அந்தர்பாவ "vtls/cipher_suite.h"
#அந்தர்பாவ "vtls/x509asn1.h"
#ifdef USE_ECH
#அந்தர்பாவ "curlx/base64.h"
#endif

#எனில் EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#இல்லையெனில்
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

கட்டமைப்பு rustls_ssl_backend_data {
  மாறிலி கட்டமைப்பு rustls_client_config *config;
  கட்டமைப்பு rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
நிலையான CURLcode map_error(மாறிலி rustls_result r)
{
  எனில்(rustls_result_is_cert_error(r)) {
    திரும்பு CURLE_PEER_FAILED_VERIFICATION;
  }
  தேர்வு(r) {
  நிலை RUSTLS_RESULT_OK:
    திரும்பு CURLE_OK;
  நிலை RUSTLS_RESULT_NULL_PARAMETER:
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  இயல்பு:
    திரும்பு CURLE_RECV_ERROR;
  }
}

நிலையான வெறுமை rustls_failf(கட்டமைப்பு Curl_easy *data, மாறிலி rustls_result rr,
                         மாறிலி எழுத்து *செய்தி)
{
  எழுத்து errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, அளவு(errorbuf), &errorlen);
  failf(data, "%s: %.*s", செய்தி, (முழு)errorlen, errorbuf);
}

நிலையான bool cr_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                            மாறிலி கட்டமைப்பு Curl_easy *data)
{
  மாறிலி கட்டமைப்பு ssl_connect_data *ctx = cf->ctx;
  கட்டமைப்பு rustls_ssl_backend_data *backend;

  (வெறுமை)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (கட்டமைப்பு rustls_ssl_backend_data *)ctx->backend;
  திரும்பு (bool)backend->data_in_pending;
}

கட்டமைப்பு io_ctx {
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு Curl_easy *data;
};

நிலையான முழு read_cb(வெறுமை *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  மாறிலி கட்டமைப்பு io_ctx *io_ctx = userdata;
  கட்டமைப்பு ssl_connect_data * மாறிலி connssl = io_ctx->cf->ctx;
  CURLcode result;
  முழு ret = 0;
  size_t nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (எழுத்து *)buf, len, &nread);
  எனில்(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    எனில்(result == CURLE_AGAIN)
      ret = EAGAIN;
    இல்லையெனில்
      ret = EINVAL;
  }
  இல்லையெனில் எனில்(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (முழு)result, nread);
  திரும்பு ret;
}

நிலையான முழு write_cb(வெறுமை *userdata, மாறிலி uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  மாறிலி கட்டமைப்பு io_ctx *io_ctx = userdata;
  CURLcode result;
  முழு ret = 0;
  size_t nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  எனில்(result) {
    nwritten = 0;
    எனில்(result == CURLE_AGAIN)
      ret = EAGAIN;
    இல்லையெனில்
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (முழு)result, nwritten);
  திரும்பு ret;
}

நிலையான ssize_t tls_recv_more(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data, CURLcode *err)
{
  மாறிலி கட்டமைப்பு ssl_connect_data * மாறிலி connssl = cf->ctx;
  கட்டமைப்பு rustls_ssl_backend_data * மாறிலி backend =
    (கட்டமைப்பு rustls_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  எனில்(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    திரும்பு -1;
  }
  இல்லையெனில் எனில்(io_error) {
    எழுத்து buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, அளவு(buffer)));
    *err = CURLE_RECV_ERROR;
    திரும்பு -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  எனில்(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    திரும்பு -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  திரும்பு (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
நிலையான CURLcode cr_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                        எழுத்து *plainbuf, size_t plainlen, size_t *pnread)
{
  மாறிலி கட்டமைப்பு ssl_connect_data * மாறிலி connssl = cf->ctx;
  கட்டமைப்பு rustls_ssl_backend_data * மாறிலி backend =
    (கட்டமைப்பு rustls_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு rustls_connection *rconn = வெற்று;
  CURLcode result = CURLE_OK;
  size_t n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  போது(*pnread < plainlen) {
    எனில்(!backend->data_in_pending) {
      எனில்(tls_recv_more(cf, data, &result) < 0) {
        எனில்(result != CURLE_AGAIN) {
          செல் out;
        }
        result = CURLE_OK;
        நிறுத்து;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    எனில்(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      நிறுத்து;
    }
    இல்லையெனில் எனில்(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      செல் out;
    }
    இல்லையெனில் எனில்(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      செல் out;
    }
    இல்லையெனில் எனில்(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      நிறுத்து;
    }
    இல்லையெனில் {
      *pnread += n;
    }
  }

  எனில்(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (முழு)result, *pnread);
  திரும்பு result;
}

நிலையான CURLcode cr_flush_out(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு rustls_connection *rconn)
{
  கட்டமைப்பு io_ctx io_ctx;
  rustls_io_result io_error;
  size_t tlswritten = 0;
  size_t tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  போது(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    எனில்(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      திரும்பு CURLE_AGAIN;
    }
    இல்லையெனில் எனில்(io_error) {
      எழுத்து buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, அளவு(buffer)));
      திரும்பு CURLE_SEND_ERROR;
    }
    எனில்(tlswritten == 0) {
      failf(data, "EOF in swrite");
      திரும்பு CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  திரும்பு CURLE_OK;
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
நிலையான CURLcode cr_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                        மாறிலி வெறுமை *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  மாறிலி கட்டமைப்பு ssl_connect_data * மாறிலி connssl = cf->ctx;
  கட்டமைப்பு rustls_ssl_backend_data * மாறிலி backend =
    (கட்டமைப்பு rustls_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு rustls_connection *rconn = வெற்று;
  size_t plainwritten = 0;
  மாறிலி unsigned எழுத்து *buf = plainbuf;
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
  எனில்(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (முழு)result);
    எனில்(result)
      திரும்பு result;
    எனில்(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    இல்லையெனில்
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  எனில்(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    எனில்(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      செல் out;
    }
    இல்லையெனில் எனில்(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      செல் out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  எனில்(result) {
    எனில்(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      எனில்(*pnwritten) {
        result = CURLE_OK;
      }
    }
    செல் out;
  }
  இல்லையெனில்
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (முழு)result, *pnwritten);
  திரும்பு result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
நிலையான uint32_t cr_verify_none(வெறுமை *userdata,
                               மாறிலி rustls_verify_server_cert_params *params)
{
  (வெறுமை)userdata;
  (வெறுமை)params;
  திரும்பு RUSTLS_RESULT_OK;
}

நிலையான முழு read_file_into(மாறிலி எழுத்து *filename, கட்டமைப்பு dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  எனில்(!f) {
    திரும்பு 0;
  }

  க்கு(;;) {
    uint8_t buf[256];
    மாறிலி size_t rr = fread(buf, 1, அளவு(buf), f);
    எனில்((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      திரும்பு 0;
    }
    எனில்(rr < அளவு(buf))
      நிறுத்து;
  }

  திரும்பு curlx_fclose(f) == 0;
}

நிலையான வெறுமை
cr_get_selected_ciphers(கட்டமைப்பு Curl_easy *data,
                        மாறிலி எழுத்து *ciphers12,
                        மாறிலி எழுத்து *ciphers13,
                        மாறிலி கட்டமைப்பு rustls_supported_ciphersuite **selected,
                        size_t *selected_size)
{
  மாறிலி size_t supported_len = *selected_size;
  மாறிலி size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  மாறிலி கட்டமைப்பு rustls_supported_ciphersuite *entry = வெற்று;
  மாறிலி எழுத்து *ciphers = ciphers12;
  size_t மொத்தம் = 0, default13_மொத்தம் = 0, i, j;
  மாறிலி எழுத்து *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  எனில்(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    க்கு(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      எனில்(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        தொடர்;

      selected[மொத்தம்++] = entry;
    }

    default13_மொத்தம் = மொத்தம்;

    எனில்(!ciphers)
      ciphers = "";
  }
  இல்லையெனில்
    ciphers = ciphers13;

add_ciphers:
  க்கு(ptr = ciphers; ptr[0] != '\0' && மொத்தம் < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    எனில்(id) {
      க்கு(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        எனில்(rustls_supported_ciphersuite_get_suite(entry) == id)
          நிறுத்து;
      }
      எனில்(i == supported_len)
        id = 0;
    }
    எனில்(!id) {
      எனில்(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (முழு)(end - ptr), ptr);
      தொடர்;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    க்கு(i = 0; i < மொத்தம் && selected[i] != entry; i++)
      ;
    எனில்(i < மொத்தம்) {
      எனில்(i >= default13_மொத்தம்)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (முழு)(end - ptr), ptr);
      தொடர்;
    }

    selected[மொத்தம்++] = entry;
  }

  எனில்(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    செல் add_ciphers;
  }

  எனில்(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    க்கு(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      எனில்(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        தொடர்;

      /* No duplicates allowed (so selected cannot overflow) */
      க்கு(i = 0; i < மொத்தம் && selected[i] != entry; i++)
        ;
      எனில்(i < மொத்தம்)
        தொடர்;

      selected[மொத்தம்++] = entry;
    }
  }

  *selected_size = மொத்தம்;
}

நிலையான வெறுமை cr_keylog_log_cb(கட்டமைப்பு rustls_str label,
                             மாறிலி uint8_t *client_random,
                             size_t client_random_len, மாறிலி uint8_t *secret,
                             size_t secret_len)
{
  எழுத்து clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, அளவு(clabel), "%.*s", (முழு)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

நிலையான CURLcode
init_config_builder(கட்டமைப்பு Curl_cfilter *cf,
                    கட்டமைப்பு Curl_easy *data,
                    மாறிலி கட்டமைப்பு ssl_filter_config *conn_config,
                    கட்டமைப்பு rustls_client_config_builder **config_builder)
{
  மாறிலி கட்டமைப்பு rustls_supported_ciphersuite **cipher_suites = வெற்று;
  கட்டமைப்பு rustls_crypto_provider_builder *custom_provider_builder = வெற்று;
  மாறிலி கட்டமைப்பு rustls_crypto_provider *custom_provider = வெற்று;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  தேர்வு(conn_config->version) {
  நிலை CURL_SSLVERSION_TLSv1:
  நிலை CURL_SSLVERSION_TLSv1_0:
  நிலை CURL_SSLVERSION_TLSv1_1:
  நிலை CURL_SSLVERSION_TLSv1_2:
    நிறுத்து;
  நிலை CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    நிறுத்து;
  இயல்பு:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் cleanup;
  }

  தேர்வு(conn_config->version_max) {
  நிலை CURL_SSLVERSION_MAX_DEFAULT:
  நிலை CURL_SSLVERSION_MAX_NONE:
  நிலை CURL_SSLVERSION_MAX_TLSv1_3:
    நிறுத்து;
  நிலை CURL_SSLVERSION_MAX_TLSv1_2:
    எனில்(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      நிறுத்து;
    }
    FALLTHROUGH();
  நிலை CURL_SSLVERSION_MAX_TLSv1_1:
  நிலை CURL_SSLVERSION_MAX_TLSv1_0:
  இயல்பு:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் cleanup;
  }

#ifdef USE_ECH
  எனில்(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#இல்லையெனில்
  (வெறுமை)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(அளவு(*cipher_suites) * cipher_suites_len);
  எனில்(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    செல் cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  எனில்(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    செல் cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    செல் cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    செல் cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    செல் cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
    செல் cleanup;
  }

cleanup:
  எனில்(cipher_suites) {
    curlx_free(cipher_suites);
  }
  எனில்(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  எனில்(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  திரும்பு result;
}

நிலையான வெறுமை
init_config_builder_alpn(கட்டமைப்பு Curl_easy *data,
                         மாறிலி கட்டமைப்பு ssl_connect_data *connssl,
                         கட்டமைப்பு rustls_client_config_builder *config_builder)
{
  கட்டமைப்பு alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  க்கு(i = 0; i < connssl->alpn->மொத்தம்; ++i) {
    alpn[i].data = (மாறிலி uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->மொத்தம்);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

நிலையான CURLcode init_config_builder_verifier_crl(
  கட்டமைப்பு Curl_easy *data,
  மாறிலி கட்டமைப்பு ssl_filter_config *conn_config,
  கட்டமைப்பு rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  கட்டமைப்பு dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  எனில்(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    செல் cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    செல் cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  திரும்பு result;
}

நிலையான CURLcode
init_config_builder_verifier(கட்டமைப்பு Curl_easy *data,
                             கட்டமைப்பு rustls_client_config_builder *builder,
                             மாறிலி கட்டமைப்பு ssl_filter_config *conn_config,
                             மாறிலி கட்டமைப்பு curl_blob *ca_info_blob,
                             மாறிலி எழுத்து * மாறிலி ssl_cafile)
{
  மாறிலி கட்டமைப்பு rustls_வேர்_cert_store *roots = வெற்று;
  கட்டமைப்பு rustls_வேர்_cert_store_builder *roots_builder = வெற்று;
  கட்டமைப்பு rustls_web_pki_server_cert_verifier_builder *verifier_builder = வெற்று;
  கட்டமைப்பு rustls_server_cert_verifier *server_cert_verifier = வெற்று;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_வேர்_cert_store_builder_new();
  எனில்(ca_info_blob) {
    rr = rustls_வேர்_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    எனில்(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      செல் cleanup;
    }
  }
  இல்லையெனில் எனில்(ssl_cafile) {
    rr = rustls_வேர்_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    எனில்(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      செல் cleanup;
    }
  }

  rr = rustls_வேர்_cert_store_builder_build(roots_builder, &roots);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    செல் cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  எனில்(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    செல் cleanup;
  }

  எனில்(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    எனில்(result) {
      செல் cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    செல் cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  எனில்(roots_builder) {
    rustls_வேர்_cert_store_builder_free(roots_builder);
  }
  எனில்(roots) {
    rustls_வேர்_cert_store_free(roots);
  }
  எனில்(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  எனில்(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  திரும்பு result;
}

நிலையான CURLcode init_config_builder_platform_verifier(
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு rustls_client_config_builder *builder)
{
  கட்டமைப்பு rustls_server_cert_verifier *server_cert_verifier = வெற்று;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    செல் cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  எனில்(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  திரும்பு result;
}

நிலையான CURLcode
init_config_builder_keylog(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  எனில்(!Curl_tls_keylog_enabled()) {
    திரும்பு CURLE_OK;
  }

  rr = rustls_client_config_builder_set_சாவி_log(builder,
                                                cr_keylog_log_cb,
                                                வெற்று);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    திரும்பு map_error(rr);
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode
init_config_builder_client_auth(கட்டமைப்பு Curl_easy *data,
                                மாறிலி கட்டமைப்பு ssl_filter_config *conn_config,
                                கட்டமைப்பு rustls_client_config_builder *builder)
{
  கட்டமைப்பு dynbuf cert_contents;
  கட்டமைப்பு dynbuf சாவி_contents;
  rustls_result rr;
  மாறிலி கட்டமைப்பு rustls_certified_சாவி *certified_சாவி = வெற்று;
  CURLcode result = CURLE_OK;
  மாறிலி bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  மாறிலி bool have_சாவி = conn_config->சாவி || conn_config->சாவி_blob;

  எனில்(have_cert && !have_சாவி) {
    failf(data, "rustls: must provide key with certificate");
    திரும்பு CURLE_SSL_CERTPROBLEM;
  }
  இல்லையெனில் எனில்(!have_cert && have_சாவி) {
    failf(data, "rustls: must provide certificate with key");
    திரும்பு CURLE_SSL_CERTPROBLEM;
  }

  எனில்(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    திரும்பு CURLE_SSL_CERTPROBLEM;
  }
  எனில்(conn_config->சாவி_type && !curl_strequal(conn_config->சாவி_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->சாவி_type);
    திரும்பு CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&சாவி_contents, DYN_KEYFILE_SIZE);

  எனில்(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    எனில்(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      செல் cleanup;
    }
  }
  இல்லையெனில் எனில்(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    செல் cleanup;
  }

  எனில்(conn_config->சாவி_blob) {
    result = curlx_dyn_addn(&சாவி_contents,
                            conn_config->சாவி_blob->data,
                            conn_config->சாவி_blob->len);
    எனில்(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      செல் cleanup;
    }
  }
  இல்லையெனில் எனில்(!read_file_into(conn_config->சாவி, &சாவி_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->சாவி);
    result = CURLE_SSL_CERTPROBLEM;
    செல் cleanup;
  }

  rr = rustls_certified_சாவி_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&சாவி_contents),
                                  curlx_dyn_len(&சாவி_contents),
                                  &certified_சாவி);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    செல் cleanup;
  }

  rr = rustls_certified_சாவி_சாவிகள்_match(certified_சாவி);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    செல் cleanup;
  }

  rr = rustls_client_config_builder_set_certified_சாவி(builder,
                                                      &certified_சாவி,
                                                      1);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    செல் cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&சாவி_contents);
  எனில்(certified_சாவி) {
    rustls_certified_சாவி_free(certified_சாவி);
  }
  திரும்பு result;
}

#ifdef USE_ECH

நிலையான CURLcode
init_config_builder_ech(கட்டமைப்பு Curl_easy *data,
                        கட்டமைப்பு Curl_cfilter *cf,
                        கட்டமைப்பு rustls_client_config_builder *builder)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  மாறிலி rustls_hpke *hpke = rustls_supported_hpke();
  unsigned எழுத்து *ech_config = வெற்று;
  size_t ech_config_len = 0;
  கட்டமைப்பு Curl_dns_entry *dns = வெற்று;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  எனில்(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    செல் cleanup;
  }

  எனில்(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    செல் cleanup;
  }

  எனில்(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    எனில்(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      செல் cleanup;
    }
    திரும்பு CURLE_OK;
  }

  எனில்(conn_config->ech && conn_config->ech_config) {
    மாறிலி எழுத்து *b64 = conn_config->ech_config;
    size_t decode_result;
    எனில்(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      செல் cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    எனில்(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      செல் cleanup;
    }
  }
  இல்லையெனில் {
    மாறிலி கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
    மாறிலி கட்டமைப்பு Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    எனில்(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      செல் cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    செல் cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  எனில்(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  எனில்(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  திரும்பு result;
}
#endif /* USE_ECH */

நிலையான CURLcode cr_init_backend(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு rustls_ssl_backend_data * மாறிலி backend)
{
  மாறிலி கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  மாறிலி கட்டமைப்பு ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு rustls_connection *rconn = வெற்று;
  கட்டமைப்பு rustls_client_config_builder *config_builder = வெற்று;

  மாறிலி கட்டமைப்பு curl_blob *ca_info_blob = conn_config->ca_info_blob;
  மாறிலி எழுத்து * மாறிலி ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? வெற்று : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  எனில்(result != CURLE_OK) {
    திரும்பு result;
  }

  எனில்(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  எனில்(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  இல்லையெனில் எனில்(conn_config->native_ca_store) {
    எனில்(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      திரும்பு CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    எனில்(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      திரும்பு result;
    }
  }
  இல்லையெனில் எனில்(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    எனில்(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      திரும்பு result;
    }
  }

  எனில்(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->சாவி || conn_config->சாவி_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    எனில்(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      திரும்பு result;
    }
  }

#ifdef USE_ECH
  எனில்(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    எனில்((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      திரும்பு result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  எனில்(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    திரும்பு result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  எனில்(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = வெற்று;
    திரும்பு CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  திரும்பு result;
}

நிலையான வெறுமை cr_set_negotiated_alpn(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data,
                                   மாறிலி கட்டமைப்பு rustls_connection *rconn)
{
  கட்டமைப்பு ssl_connect_data * மாறிலி connssl = cf->ctx;
  மாறிலி uint8_t *protocol = வெற்று;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
நிலையான CURLcode cr_connect(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                           bool *done)
{
  கட்டமைப்பு ssl_connect_data * மாறிலி connssl = cf->ctx;
  மாறிலி கட்டமைப்பு rustls_ssl_backend_data * மாறிலி backend =
    (கட்டமைப்பு rustls_ssl_backend_data *)connssl->backend;
  மாறிலி கட்டமைப்பு rustls_connection *rconn = வெற்று;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (முழு)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    எனில்(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      திரும்பு CURLE_OK;
    }
#endif /* USE_ECH */

  எனில்(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (கட்டமைப்பு rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (முழு)result);
    எனில்(result)
      திரும்பு result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  க்கு(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    எனில்(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, வெற்று, 0, &nwritten);
      எனில்(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        திரும்பு CURLE_OK;
      }
      இல்லையெனில் எனில்(tmperr != CURLE_OK) {
        திரும்பு tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        மாறிலி uint16_t proto = rustls_connection_get_protocol_version(rconn);
        மாறிலி rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        மாறிலி rustls_str kex_group_name =
          rustls_connection_get_negotiated_சாவி_exchange_group_name(rconn);
        மாறிலி எழுத்து *ver = "TLS version unknown";
        எனில்(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        எனில்(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (முழு)ciphersuite_name.len,
              ciphersuite_name.data,
              (முழு)kex_group_name.len,
              kex_group_name.data);
      }
      எனில்(data->set.ssl.certinfo) {
        size_t இலக்கம்_certs = 0;
        size_t i;
        போது(rustls_connection_get_peer_certificate(rconn, இலக்கம்_certs)) {
          இலக்கம்_certs++;
          எனில்(இலக்கம்_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  இலக்கம்_certs, MAX_ALLOWED_CERT_AMOUNT);
            திரும்பு CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (முழு)இலக்கம்_certs);
        எனில்(result)
          திரும்பு result;
        க்கு(i = 0; i < இலக்கம்_certs; i++) {
          மாறிலி rustls_certificate *cert;
          மாறிலி unsigned எழுத்து *der_data;
          size_t der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          எனில்(rresult != RUSTLS_RESULT_OK) {
            எழுத்து errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, அளவு(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (முழு)errorlen, errorbuf);
            திரும்பு map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (முழு)i,
                                         der_data, der_data + der_len);
          எனில்(result)
            திரும்பு result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      திரும்பு CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    எனில்(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, வெற்று, 0, &nwritten);
      எனில்(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        திரும்பு CURLE_OK;
      }
      இல்லையெனில் எனில்(tmperr != CURLE_OK) {
        திரும்பு tmperr;
      }
    }

    எனில்(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      எனில்(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        திரும்பு CURLE_SSL_CONNECT_ERROR;
      }
      எனில்(nread < 0) {
        எனில்(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          திரும்பு CURLE_OK;
        }
        இல்லையெனில் எனில்(tmperr == CURLE_RECV_ERROR) {
          திரும்பு CURLE_SSL_CONNECT_ERROR;
        }
        இல்லையெனில் {
          திரும்பு tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

நிலையான வெறுமை *cr_get_internals(கட்டமைப்பு ssl_connect_data *connssl, CURLINFO info)
{
  கட்டமைப்பு rustls_ssl_backend_data *backend =
    (கட்டமைப்பு rustls_ssl_backend_data *)connssl->backend;
  (வெறுமை)info;
  DEBUGASSERT(backend);
  திரும்பு backend->conn;
}

நிலையான CURLcode cr_shutdown(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                            மாறிலி bool send_shutdown, bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு rustls_ssl_backend_data *backend =
    (கட்டமைப்பு rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  எனில்(!backend->conn || cf->shutdown) {
    *done = TRUE;
    செல் out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  எனில்(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    எனில்(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, வெற்று, 0, &nwritten);
  எனில்(result) {
    எனில்(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      செல் out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (முழு)result);
    செல் out;
  }

  க்கு(i = 0; i < 10; ++i) {
    எழுத்து buf[1024];
    result = cr_recv(cf, data, buf, (முழு)அளவு(buf), &nread);
    எனில்(result)
      நிறுத்து;
  }

  எனில்(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  இல்லையெனில் எனில்(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (முழு)result);
  }
  இல்லையெனில் எனில்(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  திரும்பு result;
}

நிலையான வெறுமை cr_close(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  மாறிலி கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு rustls_ssl_backend_data *backend =
    (கட்டமைப்பு rustls_ssl_backend_data *)connssl->backend;

  (வெறுமை)data;
  DEBUGASSERT(backend);
  எனில்(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = வெற்று;
  }
  எனில்(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = வெற்று;
  }
}

நிலையான size_t cr_version(எழுத்து *buffer, size_t size)
{
  மாறிலி கட்டமைப்பு rustls_str ver = rustls_version();
  திரும்பு curl_msnprintf(buffer, size, "%.*s", (முழு)ver.len, ver.data);
}

நிலையான CURLcode cr_random(கட்டமைப்பு Curl_easy *data, unsigned எழுத்து *entropy,
                          size_t length)
{
  rustls_result rresult = 0;
  (வெறுமை)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  திரும்பு map_error(rresult);
}

நிலையான வெறுமை cr_cleanup(வெறுமை)
{
  Curl_tls_keylog_close();
}

மாறிலி கட்டமைப்பு Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  அளவு(கட்டமைப்பு rustls_ssl_backend_data),

  வெற்று,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
  cr_random,                       /* random */
  வெற்று,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  வெற்று,                            /* close_all */
  வெற்று,                            /* engines_list */
  வெற்று,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  வெற்று,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
