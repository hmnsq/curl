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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_RUSTLS

#അന്തര്ഭാവ <rustls.h>

#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "curlx/strerr.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "vdns/httpsrr.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/vtls_int.h"
#അന്തര്ഭാവ "vtls/rustls.h"
#അന്തര്ഭാവ "vtls/keylog.h"
#അന്തര്ഭാവ "vtls/cipher_suite.h"
#അന്തര്ഭാവ "vtls/x509asn1.h"
#ifdef USE_ECH
#അന്തര്ഭാവ "curlx/base64.h"
#endif

#എങ്കിൽ EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#അല്ലെങ്കിൽ
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

ഘടന rustls_ssl_backend_data {
  സ്ഥിരം ഘടന rustls_client_config *config;
  ഘടന rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
സ്ഥിതം CURLcode map_error(സ്ഥിരം rustls_result r)
{
  എങ്കിൽ(rustls_result_is_cert_error(r)) {
    തിരികെ CURLE_PEER_FAILED_VERIFICATION;
  }
  തിരഞ്ഞെടുപ്പ്(r) {
  അവസ്ഥ RUSTLS_RESULT_OK:
    തിരികെ CURLE_OK;
  അവസ്ഥ RUSTLS_RESULT_NULL_PARAMETER:
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLE_RECV_ERROR;
  }
}

സ്ഥിതം ശൂന്യം rustls_failf(ഘടന Curl_easy *data, സ്ഥിരം rustls_result rr,
                         സ്ഥിരം അക്ഷരം *സന്ദേശം)
{
  അക്ഷരം errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, വലുപ്പം(errorbuf), &errorlen);
  failf(data, "%s: %.*s", സന്ദേശം, (പൂർണ്ണസംഖ്യ)errorlen, errorbuf);
}

സ്ഥിതം bool cr_data_pending(ഘടന Curl_cfilter *cf,
                            സ്ഥിരം ഘടന Curl_easy *data)
{
  സ്ഥിരം ഘടന ssl_connect_data *ctx = cf->ctx;
  ഘടന rustls_ssl_backend_data *backend;

  (ശൂന്യം)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ഘടന rustls_ssl_backend_data *)ctx->backend;
  തിരികെ (bool)backend->data_in_pending;
}

ഘടന io_ctx {
  ഘടന Curl_cfilter *cf;
  ഘടന Curl_easy *data;
};

സ്ഥിതം പൂർണ്ണസംഖ്യ read_cb(ശൂന്യം *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  സ്ഥിരം ഘടന io_ctx *io_ctx = userdata;
  ഘടന ssl_connect_data * സ്ഥിരം connssl = io_ctx->cf->ctx;
  CURLcode result;
  പൂർണ്ണസംഖ്യ ret = 0;
  size_t nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (അക്ഷരം *)buf, len, &nread);
  എങ്കിൽ(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    എങ്കിൽ(result == CURLE_AGAIN)
      ret = EAGAIN;
    അല്ലെങ്കിൽ
      ret = EINVAL;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (പൂർണ്ണസംഖ്യ)result, nread);
  തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ write_cb(ശൂന്യം *userdata, സ്ഥിരം uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  സ്ഥിരം ഘടന io_ctx *io_ctx = userdata;
  CURLcode result;
  പൂർണ്ണസംഖ്യ ret = 0;
  size_t nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  എങ്കിൽ(result) {
    nwritten = 0;
    എങ്കിൽ(result == CURLE_AGAIN)
      ret = EAGAIN;
    അല്ലെങ്കിൽ
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (പൂർണ്ണസംഖ്യ)result, nwritten);
  തിരികെ ret;
}

സ്ഥിതം ssize_t tls_recv_more(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data, CURLcode *err)
{
  സ്ഥിരം ഘടന ssl_connect_data * സ്ഥിരം connssl = cf->ctx;
  ഘടന rustls_ssl_backend_data * സ്ഥിരം backend =
    (ഘടന rustls_ssl_backend_data *)connssl->backend;
  ഘടന io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  എങ്കിൽ(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    തിരികെ -1;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(io_error) {
    അക്ഷരം buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, വലുപ്പം(buffer)));
    *err = CURLE_RECV_ERROR;
    തിരികെ -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  എങ്കിൽ(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    തിരികെ -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  തിരികെ (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
സ്ഥിതം CURLcode cr_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                        അക്ഷരം *plainbuf, size_t plainlen, size_t *pnread)
{
  സ്ഥിരം ഘടന ssl_connect_data * സ്ഥിരം connssl = cf->ctx;
  ഘടന rustls_ssl_backend_data * സ്ഥിരം backend =
    (ഘടന rustls_ssl_backend_data *)connssl->backend;
  ഘടന rustls_connection *rconn = ശൂന്യ;
  CURLcode result = CURLE_OK;
  size_t n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  ആകുമ്പോൾ(*pnread < plainlen) {
    എങ്കിൽ(!backend->data_in_pending) {
      എങ്കിൽ(tls_recv_more(cf, data, &result) < 0) {
        എങ്കിൽ(result != CURLE_AGAIN) {
          പോകുക out;
        }
        result = CURLE_OK;
        നിർത്തുക;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    എങ്കിൽ(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ {
      *pnread += n;
    }
  }

  എങ്കിൽ(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (പൂർണ്ണസംഖ്യ)result, *pnread);
  തിരികെ result;
}

സ്ഥിതം CURLcode cr_flush_out(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                             ഘടന rustls_connection *rconn)
{
  ഘടന io_ctx io_ctx;
  rustls_io_result io_error;
  size_t tlswritten = 0;
  size_t tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  ആകുമ്പോൾ(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    എങ്കിൽ(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      തിരികെ CURLE_AGAIN;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(io_error) {
      അക്ഷരം buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, വലുപ്പം(buffer)));
      തിരികെ CURLE_SEND_ERROR;
    }
    എങ്കിൽ(tlswritten == 0) {
      failf(data, "EOF in swrite");
      തിരികെ CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  തിരികെ CURLE_OK;
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
സ്ഥിതം CURLcode cr_send(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                        സ്ഥിരം ശൂന്യം *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  സ്ഥിരം ഘടന ssl_connect_data * സ്ഥിരം connssl = cf->ctx;
  ഘടന rustls_ssl_backend_data * സ്ഥിരം backend =
    (ഘടന rustls_ssl_backend_data *)connssl->backend;
  ഘടന rustls_connection *rconn = ശൂന്യ;
  size_t plainwritten = 0;
  സ്ഥിരം unsigned അക്ഷരം *buf = plainbuf;
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
  എങ്കിൽ(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (പൂർണ്ണസംഖ്യ)result);
    എങ്കിൽ(result)
      തിരികെ result;
    എങ്കിൽ(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    അല്ലെങ്കിൽ
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  എങ്കിൽ(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    എങ്കിൽ(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      പോകുക out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      എങ്കിൽ(*pnwritten) {
        result = CURLE_OK;
      }
    }
    പോകുക out;
  }
  അല്ലെങ്കിൽ
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
  തിരികെ result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
സ്ഥിതം uint32_t cr_verify_none(ശൂന്യം *userdata,
                               സ്ഥിരം rustls_verify_server_cert_params *params)
{
  (ശൂന്യം)userdata;
  (ശൂന്യം)params;
  തിരികെ RUSTLS_RESULT_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ read_file_into(സ്ഥിരം അക്ഷരം *filename, ഘടന dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  എങ്കിൽ(!f) {
    തിരികെ 0;
  }

  വേണ്ടി(;;) {
    uint8_t buf[256];
    സ്ഥിരം size_t rr = fread(buf, 1, വലുപ്പം(buf), f);
    എങ്കിൽ((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      തിരികെ 0;
    }
    എങ്കിൽ(rr < വലുപ്പം(buf))
      നിർത്തുക;
  }

  തിരികെ curlx_fclose(f) == 0;
}

സ്ഥിതം ശൂന്യം
cr_get_selected_ciphers(ഘടന Curl_easy *data,
                        സ്ഥിരം അക്ഷരം *ciphers12,
                        സ്ഥിരം അക്ഷരം *ciphers13,
                        സ്ഥിരം ഘടന rustls_supported_ciphersuite **selected,
                        size_t *selected_size)
{
  സ്ഥിരം size_t supported_len = *selected_size;
  സ്ഥിരം size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  സ്ഥിരം ഘടന rustls_supported_ciphersuite *entry = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *ciphers = ciphers12;
  size_t എണ്ണം = 0, default13_എണ്ണം = 0, i, j;
  സ്ഥിരം അക്ഷരം *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  എങ്കിൽ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    വേണ്ടി(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      എങ്കിൽ(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        തുടരുക;

      selected[എണ്ണം++] = entry;
    }

    default13_എണ്ണം = എണ്ണം;

    എങ്കിൽ(!ciphers)
      ciphers = "";
  }
  അല്ലെങ്കിൽ
    ciphers = ciphers13;

add_ciphers:
  വേണ്ടി(ptr = ciphers; ptr[0] != '\0' && എണ്ണം < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    എങ്കിൽ(id) {
      വേണ്ടി(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        എങ്കിൽ(rustls_supported_ciphersuite_get_suite(entry) == id)
          നിർത്തുക;
      }
      എങ്കിൽ(i == supported_len)
        id = 0;
    }
    എങ്കിൽ(!id) {
      എങ്കിൽ(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (പൂർണ്ണസംഖ്യ)(end - ptr), ptr);
      തുടരുക;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    വേണ്ടി(i = 0; i < എണ്ണം && selected[i] != entry; i++)
      ;
    എങ്കിൽ(i < എണ്ണം) {
      എങ്കിൽ(i >= default13_എണ്ണം)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (പൂർണ്ണസംഖ്യ)(end - ptr), ptr);
      തുടരുക;
    }

    selected[എണ്ണം++] = entry;
  }

  എങ്കിൽ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    പോകുക add_ciphers;
  }

  എങ്കിൽ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    വേണ്ടി(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      എങ്കിൽ(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        തുടരുക;

      /* No duplicates allowed (so selected cannot overflow) */
      വേണ്ടി(i = 0; i < എണ്ണം && selected[i] != entry; i++)
        ;
      എങ്കിൽ(i < എണ്ണം)
        തുടരുക;

      selected[എണ്ണം++] = entry;
    }
  }

  *selected_size = എണ്ണം;
}

സ്ഥിതം ശൂന്യം cr_keylog_log_cb(ഘടന rustls_str label,
                             സ്ഥിരം uint8_t *client_random,
                             size_t client_random_len, സ്ഥിരം uint8_t *secret,
                             size_t secret_len)
{
  അക്ഷരം clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, വലുപ്പം(clabel), "%.*s", (പൂർണ്ണസംഖ്യ)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

സ്ഥിതം CURLcode
init_config_builder(ഘടന Curl_cfilter *cf,
                    ഘടന Curl_easy *data,
                    സ്ഥിരം ഘടന ssl_filter_config *conn_config,
                    ഘടന rustls_client_config_builder **config_builder)
{
  സ്ഥിരം ഘടന rustls_supported_ciphersuite **cipher_suites = ശൂന്യ;
  ഘടന rustls_crypto_provider_builder *custom_provider_builder = ശൂന്യ;
  സ്ഥിരം ഘടന rustls_crypto_provider *custom_provider = ശൂന്യ;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  തിരഞ്ഞെടുപ്പ്(conn_config->version) {
  അവസ്ഥ CURL_SSLVERSION_TLSv1:
  അവസ്ഥ CURL_SSLVERSION_TLSv1_0:
  അവസ്ഥ CURL_SSLVERSION_TLSv1_1:
  അവസ്ഥ CURL_SSLVERSION_TLSv1_2:
    നിർത്തുക;
  അവസ്ഥ CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക cleanup;
  }

  തിരഞ്ഞെടുപ്പ്(conn_config->version_max) {
  അവസ്ഥ CURL_SSLVERSION_MAX_DEFAULT:
  അവസ്ഥ CURL_SSLVERSION_MAX_NONE:
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_3:
    നിർത്തുക;
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_2:
    എങ്കിൽ(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      നിർത്തുക;
    }
    FALLTHROUGH();
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_1:
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_0:
  സ്ഥിരസ്ഥിതി:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക cleanup;
  }

#ifdef USE_ECH
  എങ്കിൽ(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(വലുപ്പം(*cipher_suites) * cipher_suites_len);
  എങ്കിൽ(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  എങ്കിൽ(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    പോകുക cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    പോകുക cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    പോകുക cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    പോകുക cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
    പോകുക cleanup;
  }

cleanup:
  എങ്കിൽ(cipher_suites) {
    curlx_free(cipher_suites);
  }
  എങ്കിൽ(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  എങ്കിൽ(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം
init_config_builder_alpn(ഘടന Curl_easy *data,
                         സ്ഥിരം ഘടന ssl_connect_data *connssl,
                         ഘടന rustls_client_config_builder *config_builder)
{
  ഘടന alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  വേണ്ടി(i = 0; i < connssl->alpn->എണ്ണം; ++i) {
    alpn[i].data = (സ്ഥിരം uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->എണ്ണം);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

സ്ഥിതം CURLcode init_config_builder_verifier_crl(
  ഘടന Curl_easy *data,
  സ്ഥിരം ഘടന ssl_filter_config *conn_config,
  ഘടന rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  ഘടന dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  എങ്കിൽ(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    പോകുക cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    പോകുക cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  തിരികെ result;
}

സ്ഥിതം CURLcode
init_config_builder_verifier(ഘടന Curl_easy *data,
                             ഘടന rustls_client_config_builder *builder,
                             സ്ഥിരം ഘടന ssl_filter_config *conn_config,
                             സ്ഥിരം ഘടന curl_blob *ca_info_blob,
                             സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_cafile)
{
  സ്ഥിരം ഘടന rustls_വേര്_cert_store *roots = ശൂന്യ;
  ഘടന rustls_വേര്_cert_store_builder *roots_builder = ശൂന്യ;
  ഘടന rustls_web_pki_server_cert_verifier_builder *verifier_builder = ശൂന്യ;
  ഘടന rustls_server_cert_verifier *server_cert_verifier = ശൂന്യ;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_വേര്_cert_store_builder_new();
  എങ്കിൽ(ca_info_blob) {
    rr = rustls_വേര്_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      പോകുക cleanup;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ssl_cafile) {
    rr = rustls_വേര്_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      പോകുക cleanup;
    }
  }

  rr = rustls_വേര്_cert_store_builder_build(roots_builder, &roots);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    പോകുക cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  എങ്കിൽ(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക cleanup;
  }

  എങ്കിൽ(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    എങ്കിൽ(result) {
      പോകുക cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    പോകുക cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  എങ്കിൽ(roots_builder) {
    rustls_വേര്_cert_store_builder_free(roots_builder);
  }
  എങ്കിൽ(roots) {
    rustls_വേര്_cert_store_free(roots);
  }
  എങ്കിൽ(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  എങ്കിൽ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode init_config_builder_platform_verifier(
  ഘടന Curl_easy *data,
  ഘടന rustls_client_config_builder *builder)
{
  ഘടന rustls_server_cert_verifier *server_cert_verifier = ശൂന്യ;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    പോകുക cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  എങ്കിൽ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode
init_config_builder_keylog(ഘടന Curl_easy *data,
                           ഘടന rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  എങ്കിൽ(!Curl_tls_keylog_enabled()) {
    തിരികെ CURLE_OK;
  }

  rr = rustls_client_config_builder_set_താക്കോൽ_log(builder,
                                                cr_keylog_log_cb,
                                                ശൂന്യ);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    തിരികെ map_error(rr);
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode
init_config_builder_client_auth(ഘടന Curl_easy *data,
                                സ്ഥിരം ഘടന ssl_filter_config *conn_config,
                                ഘടന rustls_client_config_builder *builder)
{
  ഘടന dynbuf cert_contents;
  ഘടന dynbuf താക്കോൽ_contents;
  rustls_result rr;
  സ്ഥിരം ഘടന rustls_certified_താക്കോൽ *certified_താക്കോൽ = ശൂന്യ;
  CURLcode result = CURLE_OK;
  സ്ഥിരം bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  സ്ഥിരം bool have_താക്കോൽ = conn_config->താക്കോൽ || conn_config->താക്കോൽ_blob;

  എങ്കിൽ(have_cert && !have_താക്കോൽ) {
    failf(data, "rustls: must provide key with certificate");
    തിരികെ CURLE_SSL_CERTPROBLEM;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!have_cert && have_താക്കോൽ) {
    failf(data, "rustls: must provide certificate with key");
    തിരികെ CURLE_SSL_CERTPROBLEM;
  }

  എങ്കിൽ(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    തിരികെ CURLE_SSL_CERTPROBLEM;
  }
  എങ്കിൽ(conn_config->താക്കോൽ_type && !curl_strequal(conn_config->താക്കോൽ_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->താക്കോൽ_type);
    തിരികെ CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&താക്കോൽ_contents, DYN_KEYFILE_SIZE);

  എങ്കിൽ(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    എങ്കിൽ(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      പോകുക cleanup;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    പോകുക cleanup;
  }

  എങ്കിൽ(conn_config->താക്കോൽ_blob) {
    result = curlx_dyn_addn(&താക്കോൽ_contents,
                            conn_config->താക്കോൽ_blob->data,
                            conn_config->താക്കോൽ_blob->len);
    എങ്കിൽ(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      പോകുക cleanup;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!read_file_into(conn_config->താക്കോൽ, &താക്കോൽ_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->താക്കോൽ);
    result = CURLE_SSL_CERTPROBLEM;
    പോകുക cleanup;
  }

  rr = rustls_certified_താക്കോൽ_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&താക്കോൽ_contents),
                                  curlx_dyn_len(&താക്കോൽ_contents),
                                  &certified_താക്കോൽ);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    പോകുക cleanup;
  }

  rr = rustls_certified_താക്കോൽ_താക്കോലുകൾ_match(certified_താക്കോൽ);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    പോകുക cleanup;
  }

  rr = rustls_client_config_builder_set_certified_താക്കോൽ(builder,
                                                      &certified_താക്കോൽ,
                                                      1);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    പോകുക cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&താക്കോൽ_contents);
  എങ്കിൽ(certified_താക്കോൽ) {
    rustls_certified_താക്കോൽ_free(certified_താക്കോൽ);
  }
  തിരികെ result;
}

#ifdef USE_ECH

സ്ഥിതം CURLcode
init_config_builder_ech(ഘടന Curl_easy *data,
                        ഘടന Curl_cfilter *cf,
                        ഘടന rustls_client_config_builder *builder)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  സ്ഥിരം rustls_hpke *hpke = rustls_supported_hpke();
  unsigned അക്ഷരം *ech_config = ശൂന്യ;
  size_t ech_config_len = 0;
  ഘടന Curl_dns_entry *dns = ശൂന്യ;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  എങ്കിൽ(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    പോകുക cleanup;
  }

  എങ്കിൽ(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    പോകുക cleanup;
  }

  എങ്കിൽ(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക cleanup;
    }
    തിരികെ CURLE_OK;
  }

  എങ്കിൽ(conn_config->ech && conn_config->ech_config) {
    സ്ഥിരം അക്ഷരം *b64 = conn_config->ech_config;
    size_t decode_result;
    എങ്കിൽ(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    എങ്കിൽ(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക cleanup;
    }
  }
  അല്ലെങ്കിൽ {
    സ്ഥിരം ഘടന ssl_connect_data *connssl = cf->ctx;
    സ്ഥിരം ഘടന Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    എങ്കിൽ(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    പോകുക cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  എങ്കിൽ(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  എങ്കിൽ(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  തിരികെ result;
}
#endif /* USE_ECH */

സ്ഥിതം CURLcode cr_init_backend(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                ഘടന rustls_ssl_backend_data * സ്ഥിരം backend)
{
  സ്ഥിരം ഘടന ssl_connect_data *connssl = cf->ctx;
  സ്ഥിരം ഘടന ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  ഘടന rustls_connection *rconn = ശൂന്യ;
  ഘടന rustls_client_config_builder *config_builder = ശൂന്യ;

  സ്ഥിരം ഘടന curl_blob *ca_info_blob = conn_config->ca_info_blob;
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ശൂന്യ : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  എങ്കിൽ(result != CURLE_OK) {
    തിരികെ result;
  }

  എങ്കിൽ(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  എങ്കിൽ(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(conn_config->native_ca_store) {
    എങ്കിൽ(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      തിരികെ CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    എങ്കിൽ(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      തിരികെ result;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    എങ്കിൽ(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      തിരികെ result;
    }
  }

  എങ്കിൽ(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->താക്കോൽ || conn_config->താക്കോൽ_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    എങ്കിൽ(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      തിരികെ result;
    }
  }

#ifdef USE_ECH
  എങ്കിൽ(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    എങ്കിൽ((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      തിരികെ result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  എങ്കിൽ(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    തിരികെ result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  എങ്കിൽ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = ശൂന്യ;
    തിരികെ CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cr_set_negotiated_alpn(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data,
                                   സ്ഥിരം ഘടന rustls_connection *rconn)
{
  ഘടന ssl_connect_data * സ്ഥിരം connssl = cf->ctx;
  സ്ഥിരം uint8_t *protocol = ശൂന്യ;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
സ്ഥിതം CURLcode cr_connect(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                           bool *done)
{
  ഘടന ssl_connect_data * സ്ഥിരം connssl = cf->ctx;
  സ്ഥിരം ഘടന rustls_ssl_backend_data * സ്ഥിരം backend =
    (ഘടന rustls_ssl_backend_data *)connssl->backend;
  സ്ഥിരം ഘടന rustls_connection *rconn = ശൂന്യ;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (പൂർണ്ണസംഖ്യ)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    എങ്കിൽ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      തിരികെ CURLE_OK;
    }
#endif /* USE_ECH */

  എങ്കിൽ(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (ഘടന rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (പൂർണ്ണസംഖ്യ)result);
    എങ്കിൽ(result)
      തിരികെ result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  വേണ്ടി(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    എങ്കിൽ(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, ശൂന്യ, 0, &nwritten);
      എങ്കിൽ(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        തിരികെ CURLE_OK;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(tmperr != CURLE_OK) {
        തിരികെ tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        സ്ഥിരം uint16_t proto = rustls_connection_get_protocol_version(rconn);
        സ്ഥിരം rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        സ്ഥിരം rustls_str kex_group_name =
          rustls_connection_get_negotiated_താക്കോൽ_exchange_group_name(rconn);
        സ്ഥിരം അക്ഷരം *ver = "TLS version unknown";
        എങ്കിൽ(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        എങ്കിൽ(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (പൂർണ്ണസംഖ്യ)ciphersuite_name.len,
              ciphersuite_name.data,
              (പൂർണ്ണസംഖ്യ)kex_group_name.len,
              kex_group_name.data);
      }
      എങ്കിൽ(data->set.ssl.certinfo) {
        size_t അക്കം_certs = 0;
        size_t i;
        ആകുമ്പോൾ(rustls_connection_get_peer_certificate(rconn, അക്കം_certs)) {
          അക്കം_certs++;
          എങ്കിൽ(അക്കം_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  അക്കം_certs, MAX_ALLOWED_CERT_AMOUNT);
            തിരികെ CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (പൂർണ്ണസംഖ്യ)അക്കം_certs);
        എങ്കിൽ(result)
          തിരികെ result;
        വേണ്ടി(i = 0; i < അക്കം_certs; i++) {
          സ്ഥിരം rustls_certificate *cert;
          സ്ഥിരം unsigned അക്ഷരം *der_data;
          size_t der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          എങ്കിൽ(rresult != RUSTLS_RESULT_OK) {
            അക്ഷരം errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, വലുപ്പം(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (പൂർണ്ണസംഖ്യ)errorlen, errorbuf);
            തിരികെ map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (പൂർണ്ണസംഖ്യ)i,
                                         der_data, der_data + der_len);
          എങ്കിൽ(result)
            തിരികെ result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      തിരികെ CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    എങ്കിൽ(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, ശൂന്യ, 0, &nwritten);
      എങ്കിൽ(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        തിരികെ CURLE_OK;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(tmperr != CURLE_OK) {
        തിരികെ tmperr;
      }
    }

    എങ്കിൽ(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      എങ്കിൽ(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        തിരികെ CURLE_SSL_CONNECT_ERROR;
      }
      എങ്കിൽ(nread < 0) {
        എങ്കിൽ(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          തിരികെ CURLE_OK;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(tmperr == CURLE_RECV_ERROR) {
          തിരികെ CURLE_SSL_CONNECT_ERROR;
        }
        അല്ലെങ്കിൽ {
          തിരികെ tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

സ്ഥിതം ശൂന്യം *cr_get_internals(ഘടന ssl_connect_data *connssl, CURLINFO info)
{
  ഘടന rustls_ssl_backend_data *backend =
    (ഘടന rustls_ssl_backend_data *)connssl->backend;
  (ശൂന്യം)info;
  DEBUGASSERT(backend);
  തിരികെ backend->conn;
}

സ്ഥിതം CURLcode cr_shutdown(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                            സ്ഥിരം bool send_shutdown, bool *done)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന rustls_ssl_backend_data *backend =
    (ഘടന rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  എങ്കിൽ(!backend->conn || cf->shutdown) {
    *done = TRUE;
    പോകുക out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  എങ്കിൽ(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    എങ്കിൽ(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, ശൂന്യ, 0, &nwritten);
  എങ്കിൽ(result) {
    എങ്കിൽ(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      പോകുക out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (പൂർണ്ണസംഖ്യ)result);
    പോകുക out;
  }

  വേണ്ടി(i = 0; i < 10; ++i) {
    അക്ഷരം buf[1024];
    result = cr_recv(cf, data, buf, (പൂർണ്ണസംഖ്യ)വലുപ്പം(buf), &nread);
    എങ്കിൽ(result)
      നിർത്തുക;
  }

  എങ്കിൽ(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (പൂർണ്ണസംഖ്യ)result);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cr_close(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  സ്ഥിരം ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന rustls_ssl_backend_data *backend =
    (ഘടന rustls_ssl_backend_data *)connssl->backend;

  (ശൂന്യം)data;
  DEBUGASSERT(backend);
  എങ്കിൽ(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = ശൂന്യ;
  }
  എങ്കിൽ(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = ശൂന്യ;
  }
}

സ്ഥിതം size_t cr_version(അക്ഷരം *buffer, size_t size)
{
  സ്ഥിരം ഘടന rustls_str ver = rustls_version();
  തിരികെ curl_msnprintf(buffer, size, "%.*s", (പൂർണ്ണസംഖ്യ)ver.len, ver.data);
}

സ്ഥിതം CURLcode cr_random(ഘടന Curl_easy *data, unsigned അക്ഷരം *entropy,
                          size_t length)
{
  rustls_result rresult = 0;
  (ശൂന്യം)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  തിരികെ map_error(rresult);
}

സ്ഥിതം ശൂന്യം cr_cleanup(ശൂന്യം)
{
  Curl_tls_keylog_close();
}

സ്ഥിരം ഘടന Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  വലുപ്പം(ഘടന rustls_ssl_backend_data),

  ശൂന്യ,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
  cr_random,                       /* random */
  ശൂന്യ,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  ശൂന്യ,                            /* close_all */
  ശൂന്യ,                            /* engines_list */
  ശൂന്യ,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  ശൂന്യ,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
