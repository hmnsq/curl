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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_RUSTLS

#અન્તર્ભાવ <rustls.h>

#અન્તર્ભાવ "curlx/win32-fopen.h"
#અન્તર્ભાવ "curlx/strerr.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "vdns/httpsrr.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/vtls_int.h"
#અન્તર્ભાવ "vtls/rustls.h"
#અન્તર્ભાવ "vtls/keylog.h"
#અન્તર્ભાવ "vtls/cipher_suite.h"
#અન્તર્ભાવ "vtls/x509asn1.h"
#ifdef USE_ECH
#અન્તર્ભાવ "curlx/base64.h"
#endif

#જો EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#નહીંતર
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

સંરચના rustls_ssl_backend_data {
  અચલ સંરચના rustls_client_config *config;
  સંરચના rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
સ્થિર CURLcode map_error(અચલ rustls_result r)
{
  જો(rustls_result_is_cert_error(r)) {
    પાછા CURLE_PEER_FAILED_VERIFICATION;
  }
  પસંદગી(r) {
  સ્થિતિ RUSTLS_RESULT_OK:
    પાછા CURLE_OK;
  સ્થિતિ RUSTLS_RESULT_NULL_PARAMETER:
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  મૂળભૂત:
    પાછા CURLE_RECV_ERROR;
  }
}

સ્થિર શૂન્ય rustls_failf(સંરચના Curl_easy *data, અચલ rustls_result rr,
                         અચલ અક્ષર *સંદેશ)
{
  અક્ષર errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, કદ(errorbuf), &errorlen);
  failf(data, "%s: %.*s", સંદેશ, (પૂર્ણાંક)errorlen, errorbuf);
}

સ્થિર bool cr_data_pending(સંરચના Curl_cfilter *cf,
                            અચલ સંરચના Curl_easy *data)
{
  અચલ સંરચના ssl_connect_data *ctx = cf->ctx;
  સંરચના rustls_ssl_backend_data *backend;

  (શૂન્ય)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (સંરચના rustls_ssl_backend_data *)ctx->backend;
  પાછા (bool)backend->data_in_pending;
}

સંરચના io_ctx {
  સંરચના Curl_cfilter *cf;
  સંરચના Curl_easy *data;
};

સ્થિર પૂર્ણાંક read_cb(શૂન્ય *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  અચલ સંરચના io_ctx *io_ctx = userdata;
  સંરચના ssl_connect_data * અચલ connssl = io_ctx->cf->ctx;
  CURLcode result;
  પૂર્ણાંક ret = 0;
  size_t nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (અક્ષર *)buf, len, &nread);
  જો(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    જો(result == CURLE_AGAIN)
      ret = EAGAIN;
    નહીંતર
      ret = EINVAL;
  }
  નહીંતર જો(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (પૂર્ણાંક)result, nread);
  પાછા ret;
}

સ્થિર પૂર્ણાંક write_cb(શૂન્ય *userdata, અચલ uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  અચલ સંરચના io_ctx *io_ctx = userdata;
  CURLcode result;
  પૂર્ણાંક ret = 0;
  size_t nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  જો(result) {
    nwritten = 0;
    જો(result == CURLE_AGAIN)
      ret = EAGAIN;
    નહીંતર
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (પૂર્ણાંક)result, nwritten);
  પાછા ret;
}

સ્થિર ssize_t tls_recv_more(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data, CURLcode *err)
{
  અચલ સંરચના ssl_connect_data * અચલ connssl = cf->ctx;
  સંરચના rustls_ssl_backend_data * અચલ backend =
    (સંરચના rustls_ssl_backend_data *)connssl->backend;
  સંરચના io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  જો(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    પાછા -1;
  }
  નહીંતર જો(io_error) {
    અક્ષર buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, કદ(buffer)));
    *err = CURLE_RECV_ERROR;
    પાછા -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  જો(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    પાછા -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  પાછા (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
સ્થિર CURLcode cr_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                        અક્ષર *plainbuf, size_t plainlen, size_t *pnread)
{
  અચલ સંરચના ssl_connect_data * અચલ connssl = cf->ctx;
  સંરચના rustls_ssl_backend_data * અચલ backend =
    (સંરચના rustls_ssl_backend_data *)connssl->backend;
  સંરચના rustls_connection *rconn = ખાલી;
  CURLcode result = CURLE_OK;
  size_t n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  જ્યારે(*pnread < plainlen) {
    જો(!backend->data_in_pending) {
      જો(tls_recv_more(cf, data, &result) < 0) {
        જો(result != CURLE_AGAIN) {
          જાઓ out;
        }
        result = CURLE_OK;
        તોડો;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    જો(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      તોડો;
    }
    નહીંતર જો(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      જાઓ out;
    }
    નહીંતર જો(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      જાઓ out;
    }
    નહીંતર જો(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      તોડો;
    }
    નહીંતર {
      *pnread += n;
    }
  }

  જો(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (પૂર્ણાંક)result, *pnread);
  પાછા result;
}

સ્થિર CURLcode cr_flush_out(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                             સંરચના rustls_connection *rconn)
{
  સંરચના io_ctx io_ctx;
  rustls_io_result io_error;
  size_t tlswritten = 0;
  size_t tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  જ્યારે(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    જો(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      પાછા CURLE_AGAIN;
    }
    નહીંતર જો(io_error) {
      અક્ષર buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, કદ(buffer)));
      પાછા CURLE_SEND_ERROR;
    }
    જો(tlswritten == 0) {
      failf(data, "EOF in swrite");
      પાછા CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  પાછા CURLE_OK;
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
સ્થિર CURLcode cr_send(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                        અચલ શૂન્ય *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  અચલ સંરચના ssl_connect_data * અચલ connssl = cf->ctx;
  સંરચના rustls_ssl_backend_data * અચલ backend =
    (સંરચના rustls_ssl_backend_data *)connssl->backend;
  સંરચના rustls_connection *rconn = ખાલી;
  size_t plainwritten = 0;
  અચલ unsigned અક્ષર *buf = plainbuf;
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
  જો(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (પૂર્ણાંક)result);
    જો(result)
      પાછા result;
    જો(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    નહીંતર
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  જો(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    જો(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      જાઓ out;
    }
    નહીંતર જો(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      જાઓ out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  જો(result) {
    જો(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      જો(*pnwritten) {
        result = CURLE_OK;
      }
    }
    જાઓ out;
  }
  નહીંતર
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (પૂર્ણાંક)result, *pnwritten);
  પાછા result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
સ્થિર uint32_t cr_verify_none(શૂન્ય *userdata,
                               અચલ rustls_verify_server_cert_params *params)
{
  (શૂન્ય)userdata;
  (શૂન્ય)params;
  પાછા RUSTLS_RESULT_OK;
}

સ્થિર પૂર્ણાંક read_file_into(અચલ અક્ષર *filename, સંરચના dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  જો(!f) {
    પાછા 0;
  }

  માટે(;;) {
    uint8_t buf[256];
    અચલ size_t rr = fread(buf, 1, કદ(buf), f);
    જો((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      પાછા 0;
    }
    જો(rr < કદ(buf))
      તોડો;
  }

  પાછા curlx_fclose(f) == 0;
}

સ્થિર શૂન્ય
cr_get_selected_ciphers(સંરચના Curl_easy *data,
                        અચલ અક્ષર *ciphers12,
                        અચલ અક્ષર *ciphers13,
                        અચલ સંરચના rustls_supported_ciphersuite **selected,
                        size_t *selected_size)
{
  અચલ size_t supported_len = *selected_size;
  અચલ size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  અચલ સંરચના rustls_supported_ciphersuite *entry = ખાલી;
  અચલ અક્ષર *ciphers = ciphers12;
  size_t ગણતરી = 0, default13_ગણતરી = 0, i, j;
  અચલ અક્ષર *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  જો(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    માટે(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      જો(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        ચાલુ;

      selected[ગણતરી++] = entry;
    }

    default13_ગણતરી = ગણતરી;

    જો(!ciphers)
      ciphers = "";
  }
  નહીંતર
    ciphers = ciphers13;

add_ciphers:
  માટે(ptr = ciphers; ptr[0] != '\0' && ગણતરી < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    જો(id) {
      માટે(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        જો(rustls_supported_ciphersuite_get_suite(entry) == id)
          તોડો;
      }
      જો(i == supported_len)
        id = 0;
    }
    જો(!id) {
      જો(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (પૂર્ણાંક)(end - ptr), ptr);
      ચાલુ;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    માટે(i = 0; i < ગણતરી && selected[i] != entry; i++)
      ;
    જો(i < ગણતરી) {
      જો(i >= default13_ગણતરી)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (પૂર્ણાંક)(end - ptr), ptr);
      ચાલુ;
    }

    selected[ગણતરી++] = entry;
  }

  જો(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    જાઓ add_ciphers;
  }

  જો(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    માટે(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      જો(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        ચાલુ;

      /* No duplicates allowed (so selected cannot overflow) */
      માટે(i = 0; i < ગણતરી && selected[i] != entry; i++)
        ;
      જો(i < ગણતરી)
        ચાલુ;

      selected[ગણતરી++] = entry;
    }
  }

  *selected_size = ગણતરી;
}

સ્થિર શૂન્ય cr_keylog_log_cb(સંરચના rustls_str label,
                             અચલ uint8_t *client_random,
                             size_t client_random_len, અચલ uint8_t *secret,
                             size_t secret_len)
{
  અક્ષર clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, કદ(clabel), "%.*s", (પૂર્ણાંક)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

સ્થિર CURLcode
init_config_builder(સંરચના Curl_cfilter *cf,
                    સંરચના Curl_easy *data,
                    અચલ સંરચના ssl_filter_config *conn_config,
                    સંરચના rustls_client_config_builder **config_builder)
{
  અચલ સંરચના rustls_supported_ciphersuite **cipher_suites = ખાલી;
  સંરચના rustls_crypto_provider_builder *custom_provider_builder = ખાલી;
  અચલ સંરચના rustls_crypto_provider *custom_provider = ખાલી;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  પસંદગી(conn_config->version) {
  સ્થિતિ CURL_SSLVERSION_TLSv1:
  સ્થિતિ CURL_SSLVERSION_TLSv1_0:
  સ્થિતિ CURL_SSLVERSION_TLSv1_1:
  સ્થિતિ CURL_SSLVERSION_TLSv1_2:
    તોડો;
  સ્થિતિ CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    તોડો;
  મૂળભૂત:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ cleanup;
  }

  પસંદગી(conn_config->version_max) {
  સ્થિતિ CURL_SSLVERSION_MAX_DEFAULT:
  સ્થિતિ CURL_SSLVERSION_MAX_NONE:
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_3:
    તોડો;
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_2:
    જો(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      તોડો;
    }
    FALLTHROUGH();
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_1:
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_0:
  મૂળભૂત:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ cleanup;
  }

#ifdef USE_ECH
  જો(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#નહીંતર
  (શૂન્ય)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(કદ(*cipher_suites) * cipher_suites_len);
  જો(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  જો(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    જાઓ cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    જાઓ cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    જાઓ cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    જાઓ cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
    જાઓ cleanup;
  }

cleanup:
  જો(cipher_suites) {
    curlx_free(cipher_suites);
  }
  જો(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  જો(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  પાછા result;
}

સ્થિર શૂન્ય
init_config_builder_alpn(સંરચના Curl_easy *data,
                         અચલ સંરચના ssl_connect_data *connssl,
                         સંરચના rustls_client_config_builder *config_builder)
{
  સંરચના alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  માટે(i = 0; i < connssl->alpn->ગણતરી; ++i) {
    alpn[i].data = (અચલ uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->ગણતરી);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

સ્થિર CURLcode init_config_builder_verifier_crl(
  સંરચના Curl_easy *data,
  અચલ સંરચના ssl_filter_config *conn_config,
  સંરચના rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  સંરચના dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  જો(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    જાઓ cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    જાઓ cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  પાછા result;
}

સ્થિર CURLcode
init_config_builder_verifier(સંરચના Curl_easy *data,
                             સંરચના rustls_client_config_builder *builder,
                             અચલ સંરચના ssl_filter_config *conn_config,
                             અચલ સંરચના curl_blob *ca_info_blob,
                             અચલ અક્ષર * અચલ ssl_cafile)
{
  અચલ સંરચના rustls_મૂળ_cert_store *roots = ખાલી;
  સંરચના rustls_મૂળ_cert_store_builder *roots_builder = ખાલી;
  સંરચના rustls_web_pki_server_cert_verifier_builder *verifier_builder = ખાલી;
  સંરચના rustls_server_cert_verifier *server_cert_verifier = ખાલી;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_મૂળ_cert_store_builder_new();
  જો(ca_info_blob) {
    rr = rustls_મૂળ_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    જો(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      જાઓ cleanup;
    }
  }
  નહીંતર જો(ssl_cafile) {
    rr = rustls_મૂળ_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    જો(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      જાઓ cleanup;
    }
  }

  rr = rustls_મૂળ_cert_store_builder_build(roots_builder, &roots);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    જાઓ cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  જો(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ cleanup;
  }

  જો(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    જો(result) {
      જાઓ cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    જાઓ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  જો(roots_builder) {
    rustls_મૂળ_cert_store_builder_free(roots_builder);
  }
  જો(roots) {
    rustls_મૂળ_cert_store_free(roots);
  }
  જો(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  જો(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  પાછા result;
}

સ્થિર CURLcode init_config_builder_platform_verifier(
  સંરચના Curl_easy *data,
  સંરચના rustls_client_config_builder *builder)
{
  સંરચના rustls_server_cert_verifier *server_cert_verifier = ખાલી;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    જાઓ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  જો(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  પાછા result;
}

સ્થિર CURLcode
init_config_builder_keylog(સંરચના Curl_easy *data,
                           સંરચના rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  જો(!Curl_tls_keylog_enabled()) {
    પાછા CURLE_OK;
  }

  rr = rustls_client_config_builder_set_ચાવી_log(builder,
                                                cr_keylog_log_cb,
                                                ખાલી);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    પાછા map_error(rr);
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode
init_config_builder_client_auth(સંરચના Curl_easy *data,
                                અચલ સંરચના ssl_filter_config *conn_config,
                                સંરચના rustls_client_config_builder *builder)
{
  સંરચના dynbuf cert_contents;
  સંરચના dynbuf ચાવી_contents;
  rustls_result rr;
  અચલ સંરચના rustls_certified_ચાવી *certified_ચાવી = ખાલી;
  CURLcode result = CURLE_OK;
  અચલ bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  અચલ bool have_ચાવી = conn_config->ચાવી || conn_config->ચાવી_blob;

  જો(have_cert && !have_ચાવી) {
    failf(data, "rustls: must provide key with certificate");
    પાછા CURLE_SSL_CERTPROBLEM;
  }
  નહીંતર જો(!have_cert && have_ચાવી) {
    failf(data, "rustls: must provide certificate with key");
    પાછા CURLE_SSL_CERTPROBLEM;
  }

  જો(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    પાછા CURLE_SSL_CERTPROBLEM;
  }
  જો(conn_config->ચાવી_type && !curl_strequal(conn_config->ચાવી_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->ચાવી_type);
    પાછા CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&ચાવી_contents, DYN_KEYFILE_SIZE);

  જો(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    જો(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      જાઓ cleanup;
    }
  }
  નહીંતર જો(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    જાઓ cleanup;
  }

  જો(conn_config->ચાવી_blob) {
    result = curlx_dyn_addn(&ચાવી_contents,
                            conn_config->ચાવી_blob->data,
                            conn_config->ચાવી_blob->len);
    જો(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      જાઓ cleanup;
    }
  }
  નહીંતર જો(!read_file_into(conn_config->ચાવી, &ચાવી_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->ચાવી);
    result = CURLE_SSL_CERTPROBLEM;
    જાઓ cleanup;
  }

  rr = rustls_certified_ચાવી_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&ચાવી_contents),
                                  curlx_dyn_len(&ચાવી_contents),
                                  &certified_ચાવી);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    જાઓ cleanup;
  }

  rr = rustls_certified_ચાવી_ચાવીઓ_match(certified_ચાવી);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    જાઓ cleanup;
  }

  rr = rustls_client_config_builder_set_certified_ચાવી(builder,
                                                      &certified_ચાવી,
                                                      1);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    જાઓ cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&ચાવી_contents);
  જો(certified_ચાવી) {
    rustls_certified_ચાવી_free(certified_ચાવી);
  }
  પાછા result;
}

#ifdef USE_ECH

સ્થિર CURLcode
init_config_builder_ech(સંરચના Curl_easy *data,
                        સંરચના Curl_cfilter *cf,
                        સંરચના rustls_client_config_builder *builder)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  અચલ rustls_hpke *hpke = rustls_supported_hpke();
  unsigned અક્ષર *ech_config = ખાલી;
  size_t ech_config_len = 0;
  સંરચના Curl_dns_entry *dns = ખાલી;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  જો(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    જાઓ cleanup;
  }

  જો(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    જાઓ cleanup;
  }

  જો(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    જો(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ cleanup;
    }
    પાછા CURLE_OK;
  }

  જો(conn_config->ech && conn_config->ech_config) {
    અચલ અક્ષર *b64 = conn_config->ech_config;
    size_t decode_result;
    જો(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    જો(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ cleanup;
    }
  }
  નહીંતર {
    અચલ સંરચના ssl_connect_data *connssl = cf->ctx;
    અચલ સંરચના Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    જો(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    જાઓ cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  જો(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  જો(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  પાછા result;
}
#endif /* USE_ECH */

સ્થિર CURLcode cr_init_backend(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                સંરચના rustls_ssl_backend_data * અચલ backend)
{
  અચલ સંરચના ssl_connect_data *connssl = cf->ctx;
  અચલ સંરચના ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  સંરચના rustls_connection *rconn = ખાલી;
  સંરચના rustls_client_config_builder *config_builder = ખાલી;

  અચલ સંરચના curl_blob *ca_info_blob = conn_config->ca_info_blob;
  અચલ અક્ષર * અચલ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ખાલી : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  જો(result != CURLE_OK) {
    પાછા result;
  }

  જો(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  જો(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  નહીંતર જો(conn_config->native_ca_store) {
    જો(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      પાછા CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    જો(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      પાછા result;
    }
  }
  નહીંતર જો(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    જો(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      પાછા result;
    }
  }

  જો(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->ચાવી || conn_config->ચાવી_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    જો(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      પાછા result;
    }
  }

#ifdef USE_ECH
  જો(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    જો((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      પાછા result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  જો(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    પાછા result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  જો(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = ખાલી;
    પાછા CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  પાછા result;
}

સ્થિર શૂન્ય cr_set_negotiated_alpn(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data,
                                   અચલ સંરચના rustls_connection *rconn)
{
  સંરચના ssl_connect_data * અચલ connssl = cf->ctx;
  અચલ uint8_t *protocol = ખાલી;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
સ્થિર CURLcode cr_connect(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                           bool *done)
{
  સંરચના ssl_connect_data * અચલ connssl = cf->ctx;
  અચલ સંરચના rustls_ssl_backend_data * અચલ backend =
    (સંરચના rustls_ssl_backend_data *)connssl->backend;
  અચલ સંરચના rustls_connection *rconn = ખાલી;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (પૂર્ણાંક)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    જો(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      પાછા CURLE_OK;
    }
#endif /* USE_ECH */

  જો(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (સંરચના rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (પૂર્ણાંક)result);
    જો(result)
      પાછા result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  માટે(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    જો(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, ખાલી, 0, &nwritten);
      જો(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        પાછા CURLE_OK;
      }
      નહીંતર જો(tmperr != CURLE_OK) {
        પાછા tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        અચલ uint16_t proto = rustls_connection_get_protocol_version(rconn);
        અચલ rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        અચલ rustls_str kex_group_name =
          rustls_connection_get_negotiated_ચાવી_exchange_group_name(rconn);
        અચલ અક્ષર *ver = "TLS version unknown";
        જો(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        જો(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (પૂર્ણાંક)ciphersuite_name.len,
              ciphersuite_name.data,
              (પૂર્ણાંક)kex_group_name.len,
              kex_group_name.data);
      }
      જો(data->set.ssl.certinfo) {
        size_t અંક_certs = 0;
        size_t i;
        જ્યારે(rustls_connection_get_peer_certificate(rconn, અંક_certs)) {
          અંક_certs++;
          જો(અંક_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  અંક_certs, MAX_ALLOWED_CERT_AMOUNT);
            પાછા CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (પૂર્ણાંક)અંક_certs);
        જો(result)
          પાછા result;
        માટે(i = 0; i < અંક_certs; i++) {
          અચલ rustls_certificate *cert;
          અચલ unsigned અક્ષર *der_data;
          size_t der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          જો(rresult != RUSTLS_RESULT_OK) {
            અક્ષર errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, કદ(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (પૂર્ણાંક)errorlen, errorbuf);
            પાછા map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (પૂર્ણાંક)i,
                                         der_data, der_data + der_len);
          જો(result)
            પાછા result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      પાછા CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    જો(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, ખાલી, 0, &nwritten);
      જો(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        પાછા CURLE_OK;
      }
      નહીંતર જો(tmperr != CURLE_OK) {
        પાછા tmperr;
      }
    }

    જો(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      જો(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        પાછા CURLE_SSL_CONNECT_ERROR;
      }
      જો(nread < 0) {
        જો(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          પાછા CURLE_OK;
        }
        નહીંતર જો(tmperr == CURLE_RECV_ERROR) {
          પાછા CURLE_SSL_CONNECT_ERROR;
        }
        નહીંતર {
          પાછા tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

સ્થિર શૂન્ય *cr_get_internals(સંરચના ssl_connect_data *connssl, CURLINFO info)
{
  સંરચના rustls_ssl_backend_data *backend =
    (સંરચના rustls_ssl_backend_data *)connssl->backend;
  (શૂન્ય)info;
  DEBUGASSERT(backend);
  પાછા backend->conn;
}

સ્થિર CURLcode cr_shutdown(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                            અચલ bool send_shutdown, bool *done)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના rustls_ssl_backend_data *backend =
    (સંરચના rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  જો(!backend->conn || cf->shutdown) {
    *done = TRUE;
    જાઓ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  જો(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    જો(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, ખાલી, 0, &nwritten);
  જો(result) {
    જો(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      જાઓ out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (પૂર્ણાંક)result);
    જાઓ out;
  }

  માટે(i = 0; i < 10; ++i) {
    અક્ષર buf[1024];
    result = cr_recv(cf, data, buf, (પૂર્ણાંક)કદ(buf), &nread);
    જો(result)
      તોડો;
  }

  જો(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  નહીંતર જો(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (પૂર્ણાંક)result);
  }
  નહીંતર જો(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  પાછા result;
}

સ્થિર શૂન્ય cr_close(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  અચલ સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના rustls_ssl_backend_data *backend =
    (સંરચના rustls_ssl_backend_data *)connssl->backend;

  (શૂન્ય)data;
  DEBUGASSERT(backend);
  જો(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = ખાલી;
  }
  જો(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = ખાલી;
  }
}

સ્થિર size_t cr_version(અક્ષર *buffer, size_t size)
{
  અચલ સંરચના rustls_str ver = rustls_version();
  પાછા curl_msnprintf(buffer, size, "%.*s", (પૂર્ણાંક)ver.len, ver.data);
}

સ્થિર CURLcode cr_random(સંરચના Curl_easy *data, unsigned અક્ષર *entropy,
                          size_t length)
{
  rustls_result rresult = 0;
  (શૂન્ય)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  પાછા map_error(rresult);
}

સ્થિર શૂન્ય cr_cleanup(શૂન્ય)
{
  Curl_tls_keylog_close();
}

અચલ સંરચના Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  કદ(સંરચના rustls_ssl_backend_data),

  ખાલી,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
  cr_random,                       /* random */
  ખાલી,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  ખાલી,                            /* close_all */
  ખાલી,                            /* engines_list */
  ખાલી,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  ખાલી,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
