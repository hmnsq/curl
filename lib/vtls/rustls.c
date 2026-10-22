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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_RUSTLS

#অন্তর্ভাব <rustls.h>

#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curlx/strerr.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "vdns/httpsrr.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "vtls/rustls.h"
#অন্তর্ভাব "vtls/keylog.h"
#অন্তর্ভাব "vtls/cipher_suite.h"
#অন্তর্ভাব "vtls/x509asn1.h"
#ifdef USE_ECH
#অন্তর্ভাব "curlx/base64.h"
#endif

#যদি EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#অন্যথা
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

সংরচনা rustls_ssl_backend_data {
  ধ্রুব সংরচনা rustls_client_config *config;
  সংরচনা rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
স্থির CURLcode map_error(ধ্রুব rustls_result r)
{
  যদি(rustls_result_is_cert_error(r)) {
    প্রত্যাগম CURLE_PEER_FAILED_VERIFICATION;
  }
  বিকল্প(r) {
  পক্ষ RUSTLS_RESULT_OK:
    প্রত্যাগম CURLE_OK;
  পক্ষ RUSTLS_RESULT_NULL_PARAMETER:
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  স্বভাব:
    প্রত্যাগম CURLE_RECV_ERROR;
  }
}

স্থির শূন্য rustls_failf(সংরচনা Curl_easy *data, ধ্রুব rustls_result rr,
                         ধ্রুব বর্ণ *বার্তা)
{
  বর্ণ errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, পরিমাণ(errorbuf), &errorlen);
  failf(data, "%s: %.*s", বার্তা, (পূর্ণাঙ্ক)errorlen, errorbuf);
}

স্থির bool cr_data_pending(সংরচনা Curl_cfilter *cf,
                            ধ্রুব সংরচনা Curl_easy *data)
{
  ধ্রুব সংরচনা ssl_connect_data *ctx = cf->ctx;
  সংরচনা rustls_ssl_backend_data *backend;

  (শূন্য)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (সংরচনা rustls_ssl_backend_data *)ctx->backend;
  প্রত্যাগম (bool)backend->data_in_pending;
}

সংরচনা io_ctx {
  সংরচনা Curl_cfilter *cf;
  সংরচনা Curl_easy *data;
};

স্থির পূর্ণাঙ্ক read_cb(শূন্য *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  ধ্রুব সংরচনা io_ctx *io_ctx = userdata;
  সংরচনা ssl_connect_data * ধ্রুব connssl = io_ctx->cf->ctx;
  CURLcode result;
  পূর্ণাঙ্ক ret = 0;
  size_t nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (বর্ণ *)buf, len, &nread);
  যদি(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    যদি(result == CURLE_AGAIN)
      ret = EAGAIN;
    অন্যথা
      ret = EINVAL;
  }
  অন্যথা যদি(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (পূর্ণাঙ্ক)result, nread);
  প্রত্যাগম ret;
}

স্থির পূর্ণাঙ্ক write_cb(শূন্য *userdata, ধ্রুব uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  ধ্রুব সংরচনা io_ctx *io_ctx = userdata;
  CURLcode result;
  পূর্ণাঙ্ক ret = 0;
  size_t nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  যদি(result) {
    nwritten = 0;
    যদি(result == CURLE_AGAIN)
      ret = EAGAIN;
    অন্যথা
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (পূর্ণাঙ্ক)result, nwritten);
  প্রত্যাগম ret;
}

স্থির ssize_t tls_recv_more(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data, CURLcode *err)
{
  ধ্রুব সংরচনা ssl_connect_data * ধ্রুব connssl = cf->ctx;
  সংরচনা rustls_ssl_backend_data * ধ্রুব backend =
    (সংরচনা rustls_ssl_backend_data *)connssl->backend;
  সংরচনা io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  যদি(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    প্রত্যাগম -1;
  }
  অন্যথা যদি(io_error) {
    বর্ণ buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, পরিমাণ(buffer)));
    *err = CURLE_RECV_ERROR;
    প্রত্যাগম -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  যদি(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    প্রত্যাগম -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  প্রত্যাগম (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
স্থির CURLcode cr_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                        বর্ণ *plainbuf, size_t plainlen, size_t *pnread)
{
  ধ্রুব সংরচনা ssl_connect_data * ধ্রুব connssl = cf->ctx;
  সংরচনা rustls_ssl_backend_data * ধ্রুব backend =
    (সংরচনা rustls_ssl_backend_data *)connssl->backend;
  সংরচনা rustls_connection *rconn = রিক্ত;
  CURLcode result = CURLE_OK;
  size_t n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  যাবত্(*pnread < plainlen) {
    যদি(!backend->data_in_pending) {
      যদি(tls_recv_more(cf, data, &result) < 0) {
        যদি(result != CURLE_AGAIN) {
          গচ্ছ out;
        }
        result = CURLE_OK;
        ভঙ্গ;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    যদি(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      ভঙ্গ;
    }
    অন্যথা যদি(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      গচ্ছ out;
    }
    অন্যথা যদি(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      গচ্ছ out;
    }
    অন্যথা যদি(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      ভঙ্গ;
    }
    অন্যথা {
      *pnread += n;
    }
  }

  যদি(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (পূর্ণাঙ্ক)result, *pnread);
  প্রত্যাগম result;
}

স্থির CURLcode cr_flush_out(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                             সংরচনা rustls_connection *rconn)
{
  সংরচনা io_ctx io_ctx;
  rustls_io_result io_error;
  size_t tlswritten = 0;
  size_t tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  যাবত্(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    যদি(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      প্রত্যাগম CURLE_AGAIN;
    }
    অন্যথা যদি(io_error) {
      বর্ণ buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, পরিমাণ(buffer)));
      প্রত্যাগম CURLE_SEND_ERROR;
    }
    যদি(tlswritten == 0) {
      failf(data, "EOF in swrite");
      প্রত্যাগম CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  প্রত্যাগম CURLE_OK;
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
স্থির CURLcode cr_send(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                        ধ্রুব শূন্য *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ধ্রুব সংরচনা ssl_connect_data * ধ্রুব connssl = cf->ctx;
  সংরচনা rustls_ssl_backend_data * ধ্রুব backend =
    (সংরচনা rustls_ssl_backend_data *)connssl->backend;
  সংরচনা rustls_connection *rconn = রিক্ত;
  size_t plainwritten = 0;
  ধ্রুব unsigned বর্ণ *buf = plainbuf;
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
  যদি(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (পূর্ণাঙ্ক)result);
    যদি(result)
      প্রত্যাগম result;
    যদি(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    অন্যথা
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  যদি(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    যদি(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      গচ্ছ out;
    }
    অন্যথা যদি(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      গচ্ছ out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  যদি(result) {
    যদি(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      যদি(*pnwritten) {
        result = CURLE_OK;
      }
    }
    গচ্ছ out;
  }
  অন্যথা
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (পূর্ণাঙ্ক)result, *pnwritten);
  প্রত্যাগম result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
স্থির uint32_t cr_verify_none(শূন্য *userdata,
                               ধ্রুব rustls_verify_server_cert_params *params)
{
  (শূন্য)userdata;
  (শূন্য)params;
  প্রত্যাগম RUSTLS_RESULT_OK;
}

স্থির পূর্ণাঙ্ক read_file_into(ধ্রুব বর্ণ *filename, সংরচনা dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  যদি(!f) {
    প্রত্যাগম 0;
  }

  প্রতি(;;) {
    uint8_t buf[256];
    ধ্রুব size_t rr = fread(buf, 1, পরিমাণ(buf), f);
    যদি((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      প্রত্যাগম 0;
    }
    যদি(rr < পরিমাণ(buf))
      ভঙ্গ;
  }

  প্রত্যাগম curlx_fclose(f) == 0;
}

স্থির শূন্য
cr_get_selected_ciphers(সংরচনা Curl_easy *data,
                        ধ্রুব বর্ণ *ciphers12,
                        ধ্রুব বর্ণ *ciphers13,
                        ধ্রুব সংরচনা rustls_supported_ciphersuite **selected,
                        size_t *selected_size)
{
  ধ্রুব size_t supported_len = *selected_size;
  ধ্রুব size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ধ্রুব সংরচনা rustls_supported_ciphersuite *entry = রিক্ত;
  ধ্রুব বর্ণ *ciphers = ciphers12;
  size_t গুনতি = 0, default13_গুনতি = 0, i, j;
  ধ্রুব বর্ণ *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  যদি(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    প্রতি(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      যদি(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        অনুবর্ত;

      selected[গুনতি++] = entry;
    }

    default13_গুনতি = গুনতি;

    যদি(!ciphers)
      ciphers = "";
  }
  অন্যথা
    ciphers = ciphers13;

add_ciphers:
  প্রতি(ptr = ciphers; ptr[0] != '\0' && গুনতি < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    যদি(id) {
      প্রতি(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        যদি(rustls_supported_ciphersuite_get_suite(entry) == id)
          ভঙ্গ;
      }
      যদি(i == supported_len)
        id = 0;
    }
    যদি(!id) {
      যদি(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (পূর্ণাঙ্ক)(end - ptr), ptr);
      অনুবর্ত;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    প্রতি(i = 0; i < গুনতি && selected[i] != entry; i++)
      ;
    যদি(i < গুনতি) {
      যদি(i >= default13_গুনতি)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (পূর্ণাঙ্ক)(end - ptr), ptr);
      অনুবর্ত;
    }

    selected[গুনতি++] = entry;
  }

  যদি(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    গচ্ছ add_ciphers;
  }

  যদি(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    প্রতি(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      যদি(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        অনুবর্ত;

      /* No duplicates allowed (so selected cannot overflow) */
      প্রতি(i = 0; i < গুনতি && selected[i] != entry; i++)
        ;
      যদি(i < গুনতি)
        অনুবর্ত;

      selected[গুনতি++] = entry;
    }
  }

  *selected_size = গুনতি;
}

স্থির শূন্য cr_keylog_log_cb(সংরচনা rustls_str label,
                             ধ্রুব uint8_t *client_random,
                             size_t client_random_len, ধ্রুব uint8_t *secret,
                             size_t secret_len)
{
  বর্ণ clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, পরিমাণ(clabel), "%.*s", (পূর্ণাঙ্ক)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

স্থির CURLcode
init_config_builder(সংরচনা Curl_cfilter *cf,
                    সংরচনা Curl_easy *data,
                    ধ্রুব সংরচনা ssl_filter_config *conn_config,
                    সংরচনা rustls_client_config_builder **config_builder)
{
  ধ্রুব সংরচনা rustls_supported_ciphersuite **cipher_suites = রিক্ত;
  সংরচনা rustls_crypto_provider_builder *custom_provider_builder = রিক্ত;
  ধ্রুব সংরচনা rustls_crypto_provider *custom_provider = রিক্ত;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  বিকল্প(conn_config->version) {
  পক্ষ CURL_SSLVERSION_TLSv1:
  পক্ষ CURL_SSLVERSION_TLSv1_0:
  পক্ষ CURL_SSLVERSION_TLSv1_1:
  পক্ষ CURL_SSLVERSION_TLSv1_2:
    ভঙ্গ;
  পক্ষ CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    ভঙ্গ;
  স্বভাব:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ cleanup;
  }

  বিকল্প(conn_config->version_max) {
  পক্ষ CURL_SSLVERSION_MAX_DEFAULT:
  পক্ষ CURL_SSLVERSION_MAX_NONE:
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_3:
    ভঙ্গ;
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_2:
    যদি(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      ভঙ্গ;
    }
    FALLTHROUGH();
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_1:
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_0:
  স্বভাব:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ cleanup;
  }

#ifdef USE_ECH
  যদি(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#অন্যথা
  (শূন্য)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(পরিমাণ(*cipher_suites) * cipher_suites_len);
  যদি(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  যদি(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    গচ্ছ cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    গচ্ছ cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    গচ্ছ cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    গচ্ছ cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
    গচ্ছ cleanup;
  }

cleanup:
  যদি(cipher_suites) {
    curlx_free(cipher_suites);
  }
  যদি(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  যদি(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  প্রত্যাগম result;
}

স্থির শূন্য
init_config_builder_alpn(সংরচনা Curl_easy *data,
                         ধ্রুব সংরচনা ssl_connect_data *connssl,
                         সংরচনা rustls_client_config_builder *config_builder)
{
  সংরচনা alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  প্রতি(i = 0; i < connssl->alpn->গুনতি; ++i) {
    alpn[i].data = (ধ্রুব uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->গুনতি);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

স্থির CURLcode init_config_builder_verifier_crl(
  সংরচনা Curl_easy *data,
  ধ্রুব সংরচনা ssl_filter_config *conn_config,
  সংরচনা rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  সংরচনা dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  যদি(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    গচ্ছ cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    গচ্ছ cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  প্রত্যাগম result;
}

স্থির CURLcode
init_config_builder_verifier(সংরচনা Curl_easy *data,
                             সংরচনা rustls_client_config_builder *builder,
                             ধ্রুব সংরচনা ssl_filter_config *conn_config,
                             ধ্রুব সংরচনা curl_blob *ca_info_blob,
                             ধ্রুব বর্ণ * ধ্রুব ssl_cafile)
{
  ধ্রুব সংরচনা rustls_মূল_cert_store *roots = রিক্ত;
  সংরচনা rustls_মূল_cert_store_builder *roots_builder = রিক্ত;
  সংরচনা rustls_web_pki_server_cert_verifier_builder *verifier_builder = রিক্ত;
  সংরচনা rustls_server_cert_verifier *server_cert_verifier = রিক্ত;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_মূল_cert_store_builder_new();
  যদি(ca_info_blob) {
    rr = rustls_মূল_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    যদি(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      গচ্ছ cleanup;
    }
  }
  অন্যথা যদি(ssl_cafile) {
    rr = rustls_মূল_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    যদি(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      গচ্ছ cleanup;
    }
  }

  rr = rustls_মূল_cert_store_builder_build(roots_builder, &roots);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    গচ্ছ cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  যদি(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ cleanup;
  }

  যদি(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    যদি(result) {
      গচ্ছ cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    গচ্ছ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  যদি(roots_builder) {
    rustls_মূল_cert_store_builder_free(roots_builder);
  }
  যদি(roots) {
    rustls_মূল_cert_store_free(roots);
  }
  যদি(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  যদি(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  প্রত্যাগম result;
}

স্থির CURLcode init_config_builder_platform_verifier(
  সংরচনা Curl_easy *data,
  সংরচনা rustls_client_config_builder *builder)
{
  সংরচনা rustls_server_cert_verifier *server_cert_verifier = রিক্ত;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    গচ্ছ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  যদি(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  প্রত্যাগম result;
}

স্থির CURLcode
init_config_builder_keylog(সংরচনা Curl_easy *data,
                           সংরচনা rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  যদি(!Curl_tls_keylog_enabled()) {
    প্রত্যাগম CURLE_OK;
  }

  rr = rustls_client_config_builder_set_চাবি_log(builder,
                                                cr_keylog_log_cb,
                                                রিক্ত);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    প্রত্যাগম map_error(rr);
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode
init_config_builder_client_auth(সংরচনা Curl_easy *data,
                                ধ্রুব সংরচনা ssl_filter_config *conn_config,
                                সংরচনা rustls_client_config_builder *builder)
{
  সংরচনা dynbuf cert_contents;
  সংরচনা dynbuf চাবি_contents;
  rustls_result rr;
  ধ্রুব সংরচনা rustls_certified_চাবি *certified_চাবি = রিক্ত;
  CURLcode result = CURLE_OK;
  ধ্রুব bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ধ্রুব bool have_চাবি = conn_config->চাবি || conn_config->চাবি_blob;

  যদি(have_cert && !have_চাবি) {
    failf(data, "rustls: must provide key with certificate");
    প্রত্যাগম CURLE_SSL_CERTPROBLEM;
  }
  অন্যথা যদি(!have_cert && have_চাবি) {
    failf(data, "rustls: must provide certificate with key");
    প্রত্যাগম CURLE_SSL_CERTPROBLEM;
  }

  যদি(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    প্রত্যাগম CURLE_SSL_CERTPROBLEM;
  }
  যদি(conn_config->চাবি_type && !curl_strequal(conn_config->চাবি_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->চাবি_type);
    প্রত্যাগম CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&চাবি_contents, DYN_KEYFILE_SIZE);

  যদি(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    যদি(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      গচ্ছ cleanup;
    }
  }
  অন্যথা যদি(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    গচ্ছ cleanup;
  }

  যদি(conn_config->চাবি_blob) {
    result = curlx_dyn_addn(&চাবি_contents,
                            conn_config->চাবি_blob->data,
                            conn_config->চাবি_blob->len);
    যদি(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      গচ্ছ cleanup;
    }
  }
  অন্যথা যদি(!read_file_into(conn_config->চাবি, &চাবি_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->চাবি);
    result = CURLE_SSL_CERTPROBLEM;
    গচ্ছ cleanup;
  }

  rr = rustls_certified_চাবি_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&চাবি_contents),
                                  curlx_dyn_len(&চাবি_contents),
                                  &certified_চাবি);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    গচ্ছ cleanup;
  }

  rr = rustls_certified_চাবি_চাবিগুলি_match(certified_চাবি);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    গচ্ছ cleanup;
  }

  rr = rustls_client_config_builder_set_certified_চাবি(builder,
                                                      &certified_চাবি,
                                                      1);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    গচ্ছ cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&চাবি_contents);
  যদি(certified_চাবি) {
    rustls_certified_চাবি_free(certified_চাবি);
  }
  প্রত্যাগম result;
}

#ifdef USE_ECH

স্থির CURLcode
init_config_builder_ech(সংরচনা Curl_easy *data,
                        সংরচনা Curl_cfilter *cf,
                        সংরচনা rustls_client_config_builder *builder)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ধ্রুব rustls_hpke *hpke = rustls_supported_hpke();
  unsigned বর্ণ *ech_config = রিক্ত;
  size_t ech_config_len = 0;
  সংরচনা Curl_dns_entry *dns = রিক্ত;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  যদি(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    গচ্ছ cleanup;
  }

  যদি(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    গচ্ছ cleanup;
  }

  যদি(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    যদি(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ cleanup;
    }
    প্রত্যাগম CURLE_OK;
  }

  যদি(conn_config->ech && conn_config->ech_config) {
    ধ্রুব বর্ণ *b64 = conn_config->ech_config;
    size_t decode_result;
    যদি(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    যদি(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ cleanup;
    }
  }
  অন্যথা {
    ধ্রুব সংরচনা ssl_connect_data *connssl = cf->ctx;
    ধ্রুব সংরচনা Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    যদি(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    গচ্ছ cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  যদি(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  যদি(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  প্রত্যাগম result;
}
#endif /* USE_ECH */

স্থির CURLcode cr_init_backend(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                সংরচনা rustls_ssl_backend_data * ধ্রুব backend)
{
  ধ্রুব সংরচনা ssl_connect_data *connssl = cf->ctx;
  ধ্রুব সংরচনা ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  সংরচনা rustls_connection *rconn = রিক্ত;
  সংরচনা rustls_client_config_builder *config_builder = রিক্ত;

  ধ্রুব সংরচনা curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ধ্রুব বর্ণ * ধ্রুব ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? রিক্ত : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  যদি(result != CURLE_OK) {
    প্রত্যাগম result;
  }

  যদি(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  যদি(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  অন্যথা যদি(conn_config->native_ca_store) {
    যদি(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      প্রত্যাগম CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    যদি(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      প্রত্যাগম result;
    }
  }
  অন্যথা যদি(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    যদি(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      প্রত্যাগম result;
    }
  }

  যদি(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->চাবি || conn_config->চাবি_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    যদি(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      প্রত্যাগম result;
    }
  }

#ifdef USE_ECH
  যদি(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    যদি((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      প্রত্যাগম result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  যদি(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    প্রত্যাগম result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  যদি(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = রিক্ত;
    প্রত্যাগম CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  প্রত্যাগম result;
}

স্থির শূন্য cr_set_negotiated_alpn(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data,
                                   ধ্রুব সংরচনা rustls_connection *rconn)
{
  সংরচনা ssl_connect_data * ধ্রুব connssl = cf->ctx;
  ধ্রুব uint8_t *protocol = রিক্ত;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
স্থির CURLcode cr_connect(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                           bool *done)
{
  সংরচনা ssl_connect_data * ধ্রুব connssl = cf->ctx;
  ধ্রুব সংরচনা rustls_ssl_backend_data * ধ্রুব backend =
    (সংরচনা rustls_ssl_backend_data *)connssl->backend;
  ধ্রুব সংরচনা rustls_connection *rconn = রিক্ত;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (পূর্ণাঙ্ক)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    যদি(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      প্রত্যাগম CURLE_OK;
    }
#endif /* USE_ECH */

  যদি(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (সংরচনা rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (পূর্ণাঙ্ক)result);
    যদি(result)
      প্রত্যাগম result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  প্রতি(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    যদি(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, রিক্ত, 0, &nwritten);
      যদি(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        প্রত্যাগম CURLE_OK;
      }
      অন্যথা যদি(tmperr != CURLE_OK) {
        প্রত্যাগম tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        ধ্রুব uint16_t proto = rustls_connection_get_protocol_version(rconn);
        ধ্রুব rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ধ্রুব rustls_str kex_group_name =
          rustls_connection_get_negotiated_চাবি_exchange_group_name(rconn);
        ধ্রুব বর্ণ *ver = "TLS version unknown";
        যদি(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        যদি(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (পূর্ণাঙ্ক)ciphersuite_name.len,
              ciphersuite_name.data,
              (পূর্ণাঙ্ক)kex_group_name.len,
              kex_group_name.data);
      }
      যদি(data->set.ssl.certinfo) {
        size_t অঙ্ক_certs = 0;
        size_t i;
        যাবত্(rustls_connection_get_peer_certificate(rconn, অঙ্ক_certs)) {
          অঙ্ক_certs++;
          যদি(অঙ্ক_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  অঙ্ক_certs, MAX_ALLOWED_CERT_AMOUNT);
            প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (পূর্ণাঙ্ক)অঙ্ক_certs);
        যদি(result)
          প্রত্যাগম result;
        প্রতি(i = 0; i < অঙ্ক_certs; i++) {
          ধ্রুব rustls_certificate *cert;
          ধ্রুব unsigned বর্ণ *der_data;
          size_t der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          যদি(rresult != RUSTLS_RESULT_OK) {
            বর্ণ errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, পরিমাণ(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (পূর্ণাঙ্ক)errorlen, errorbuf);
            প্রত্যাগম map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (পূর্ণাঙ্ক)i,
                                         der_data, der_data + der_len);
          যদি(result)
            প্রত্যাগম result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      প্রত্যাগম CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    যদি(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, রিক্ত, 0, &nwritten);
      যদি(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        প্রত্যাগম CURLE_OK;
      }
      অন্যথা যদি(tmperr != CURLE_OK) {
        প্রত্যাগম tmperr;
      }
    }

    যদি(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      যদি(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
      }
      যদি(nread < 0) {
        যদি(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          প্রত্যাগম CURLE_OK;
        }
        অন্যথা যদি(tmperr == CURLE_RECV_ERROR) {
          প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
        }
        অন্যথা {
          প্রত্যাগম tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

স্থির শূন্য *cr_get_internals(সংরচনা ssl_connect_data *connssl, CURLINFO info)
{
  সংরচনা rustls_ssl_backend_data *backend =
    (সংরচনা rustls_ssl_backend_data *)connssl->backend;
  (শূন্য)info;
  DEBUGASSERT(backend);
  প্রত্যাগম backend->conn;
}

স্থির CURLcode cr_shutdown(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                            ধ্রুব bool send_shutdown, bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা rustls_ssl_backend_data *backend =
    (সংরচনা rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  যদি(!backend->conn || cf->shutdown) {
    *done = TRUE;
    গচ্ছ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  যদি(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    যদি(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, রিক্ত, 0, &nwritten);
  যদি(result) {
    যদি(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      গচ্ছ out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (পূর্ণাঙ্ক)result);
    গচ্ছ out;
  }

  প্রতি(i = 0; i < 10; ++i) {
    বর্ণ buf[1024];
    result = cr_recv(cf, data, buf, (পূর্ণাঙ্ক)পরিমাণ(buf), &nread);
    যদি(result)
      ভঙ্গ;
  }

  যদি(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  অন্যথা যদি(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (পূর্ণাঙ্ক)result);
  }
  অন্যথা যদি(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  প্রত্যাগম result;
}

স্থির শূন্য cr_close(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  ধ্রুব সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা rustls_ssl_backend_data *backend =
    (সংরচনা rustls_ssl_backend_data *)connssl->backend;

  (শূন্য)data;
  DEBUGASSERT(backend);
  যদি(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = রিক্ত;
  }
  যদি(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = রিক্ত;
  }
}

স্থির size_t cr_version(বর্ণ *buffer, size_t size)
{
  ধ্রুব সংরচনা rustls_str ver = rustls_version();
  প্রত্যাগম curl_msnprintf(buffer, size, "%.*s", (পূর্ণাঙ্ক)ver.len, ver.data);
}

স্থির CURLcode cr_random(সংরচনা Curl_easy *data, unsigned বর্ণ *entropy,
                          size_t length)
{
  rustls_result rresult = 0;
  (শূন্য)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  প্রত্যাগম map_error(rresult);
}

স্থির শূন্য cr_cleanup(শূন্য)
{
  Curl_tls_keylog_close();
}

ধ্রুব সংরচনা Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  পরিমাণ(সংরচনা rustls_ssl_backend_data),

  রিক্ত,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
  cr_random,                       /* random */
  রিক্ত,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  রিক্ত,                            /* close_all */
  রিক্ত,                            /* engines_list */
  রিক্ত,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  রিক্ত,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
