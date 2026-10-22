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

#اگر_تعریف_شدہ USE_RUSTLS

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
#اگر_تعریف_شدہ USE_ECH
#شامل "curlx/base64.h"
#اگر_ختم

#اگر EAGAIN != EWOULDBLOCK
#تعریف_کرو RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#ورنہ
#تعریف_کرو RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#اگر_ختم

ساخت rustls_ssl_backend_ڈیٹا {
  ثابت ساخت rustls_client_config *config;
  ساخت rustls_connection *conn;
  size_t plain_برآمد_buffered;
  ثنائی_ہندسہ(ڈیٹا_اندر_pending);
  ثنائی_ہندسہ(بھیجا_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
ساکن CURLcode map_error(ثابت rustls_نتیجہ r)
{
  اگر(rustls_نتیجہ_is_cert_error(r)) {
    واپس CURLE_PEER_FAILED_VERIFICATION;
  }
  انتخاب(r) {
  حالت RUSTLS_نتیجہ_OK:
    واپس CURLE_OK;
  حالت RUSTLS_نتیجہ_NULL_PARAMETER:
    واپس CURLE_BAD_FUNCTION_دلیل;
  طےشدہ:
    واپس CURLE_RECV_ERROR;
  }
}

ساکن خالی rustls_failf(ساخت Curl_easy *ڈیٹا, ثابت rustls_نتیجہ rr,
                         ثابت حرف *پیغام)
{
  حرف errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, حجم(errorbuf), &errorlen);
  failf(ڈیٹا, "%s: %.*s", پیغام, (صحیح)errorlen, errorbuf);
}

ساکن bool cr_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                            ثابت ساخت Curl_easy *ڈیٹا)
{
  ثابت ساخت ssl_connect_ڈیٹا *ctx = cf->ctx;
  ساخت rustls_ssl_backend_ڈیٹا *backend;

  (خالی)ڈیٹا;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ساخت rustls_ssl_backend_ڈیٹا *)ctx->backend;
  واپس (bool)backend->ڈیٹا_اندر_pending;
}

ساخت io_ctx {
  ساخت Curl_cfilter *cf;
  ساخت Curl_easy *ڈیٹا;
};

ساکن صحیح read_cb(خالی *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *برآمد_n)
{
  ثابت ساخت io_ctx *io_ctx = userdata;
  ساخت ssl_connect_ڈیٹا * ثابت connssl = io_ctx->cf->ctx;
  CURLcode نتیجہ;
  صحیح ret = 0;
  size_t nread;

  نتیجہ = Curl_conn_cf_recv(io_ctx->cf->اگلا, io_ctx->ڈیٹا,
                             (حرف *)buf, len, &nread);
  اگر(نتیجہ) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    اگر(نتیجہ == CURLE_AGAIN)
      ret = EAGAIN;
    ورنہ
      ret = EINVAL;
  }
  ورنہ اگر(nread == 0)
    connssl->peer_closed = TRUE;
  *برآمد_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->ڈیٹا, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (صحیح)نتیجہ, nread);
  واپس ret;
}

ساکن صحیح write_cb(خالی *userdata, ثابت uint8_t *buf, uintptr_t len,
                    uintptr_t *برآمد_n)
{
  ثابت ساخت io_ctx *io_ctx = userdata;
  CURLcode نتیجہ;
  صحیح ret = 0;
  size_t nwritten;

  نتیجہ = Curl_conn_cf_send(io_ctx->cf->اگلا, io_ctx->ڈیٹا,
                             buf, len, FALSE, &nwritten);
  اگر(نتیجہ) {
    nwritten = 0;
    اگر(نتیجہ == CURLE_AGAIN)
      ret = EAGAIN;
    ورنہ
      ret = EINVAL;
  }
  *برآمد_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->ڈیٹا, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (صحیح)نتیجہ, nwritten);
  واپس ret;
}

ساکن ssize_t tls_recv_مزید(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا, CURLcode *err)
{
  ثابت ساخت ssl_connect_ڈیٹا * ثابت connssl = cf->ctx;
  ساخت rustls_ssl_backend_ڈیٹا * ثابت backend =
    (ساخت rustls_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت io_ctx io_ctx;
  size_t tls_بائٹیں_read = 0;
  rustls_io_نتیجہ io_error;
  rustls_نتیجہ rresult = 0;

  io_ctx.cf = cf;
  io_ctx.ڈیٹا = ڈیٹا;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_بائٹیں_read);
  اگر(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    واپس -1;
  }
  ورنہ اگر(io_error) {
    حرف بفر[STRERROR_LEN];
    failf(ڈیٹا, "reading from socket: %s",
          curlx_strerror(io_error, بفر, حجم(بفر)));
    *err = CURLE_RECV_ERROR;
    واپس -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  اگر(rresult != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    واپس -1;
  }

  backend->ڈیٹا_اندر_pending = TRUE;
  *err = CURLE_OK;
  واپس (ssize_t)tls_بائٹیں_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
ساکن CURLcode cr_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                        حرف *plainbuf, size_t plainlen, size_t *pnread)
{
  ثابت ساخت ssl_connect_ڈیٹا * ثابت connssl = cf->ctx;
  ساخت rustls_ssl_backend_ڈیٹا * ثابت backend =
    (ساخت rustls_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت rustls_connection *rconn = تہی;
  CURLcode نتیجہ = CURLE_OK;
  size_t n = 0;
  rustls_نتیجہ rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  جب(*pnread < plainlen) {
    اگر(!backend->ڈیٹا_اندر_pending) {
      اگر(tls_recv_مزید(cf, ڈیٹا, &نتیجہ) < 0) {
        اگر(نتیجہ != CURLE_AGAIN) {
          جاؤ برآمد;
        }
        نتیجہ = CURLE_OK;
        توڑو;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    اگر(rresult == RUSTLS_نتیجہ_PLAINTEXT_خالی_خانے) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->ڈیٹا_اندر_pending = FALSE;
      توڑو;
    }
    ورنہ اگر(rresult == RUSTLS_نتیجہ_UNEXPECTED_EOF) {
      failf(ڈیٹا, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      نتیجہ = CURLE_RECV_ERROR;
      جاؤ برآمد;
    }
    ورنہ اگر(rresult != RUSTLS_نتیجہ_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(ڈیٹا, rresult, "rustls_connection_read");
      نتیجہ = CURLE_RECV_ERROR;
      جاؤ برآمد;
    }
    ورنہ اگر(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      توڑو;
    }
    ورنہ {
      *pnread += n;
    }
  }

  اگر(!eof && !*pnread) {
    نتیجہ = CURLE_AGAIN;
  }

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (صحیح)نتیجہ, *pnread);
  واپس نتیجہ;
}

ساکن CURLcode cr_flush_برآمد(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                             ساخت rustls_connection *rconn)
{
  ساخت io_ctx io_ctx;
  rustls_io_نتیجہ io_error;
  size_t tlswritten = 0;
  size_t tlswritten_کل = 0;

  io_ctx.cf = cf;
  io_ctx.ڈیٹا = ڈیٹا;

  جب(rustls_connection_چاہتا_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    اگر(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(ڈیٹا, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_کل);
      واپس CURLE_AGAIN;
    }
    ورنہ اگر(io_error) {
      حرف بفر[STRERROR_LEN];
      failf(ڈیٹا, "writing to socket: %s",
            curlx_strerror(io_error, بفر, حجم(بفر)));
      واپس CURLE_SEND_ERROR;
    }
    اگر(tlswritten == 0) {
      failf(ڈیٹا, "EOF in swrite");
      واپس CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(ڈیٹا, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_کل += tlswritten;
  }
  واپس CURLE_OK;
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
ساکن CURLcode cr_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                        ثابت خالی *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ثابت ساخت ssl_connect_ڈیٹا * ثابت connssl = cf->ctx;
  ساخت rustls_ssl_backend_ڈیٹا * ثابت backend =
    (ساخت rustls_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت rustls_connection *rconn = تہی;
  size_t plainwritten = 0;
  ثابت غیرعلامتی حرف *buf = plainbuf;
  CURLcode نتیجہ = CURLE_OK;
  size_t blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(ڈیٹا, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  اگر(backend->plain_برآمد_buffered) {
    نتیجہ = cr_flush_برآمد(cf, ڈیٹا, rconn);
    CURL_TRC_CF(ڈیٹا, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_برآمد_buffered, (صحیح)نتیجہ);
    اگر(نتیجہ)
      واپس نتیجہ;
    اگر(blen > backend->plain_برآمد_buffered) {
      blen -= backend->plain_برآمد_buffered;
      buf += backend->plain_برآمد_buffered;
    }
    ورنہ
      blen = 0;
    *pnwritten += backend->plain_برآمد_buffered;
    backend->plain_برآمد_buffered = 0;
  }

  اگر(blen > 0) {
    rustls_نتیجہ rresult;
    CURL_TRC_CF(ڈیٹا, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    اگر(rresult != RUSTLS_نتیجہ_OK) {
      rustls_failf(ڈیٹا, rresult, "rustls_connection_write");
      نتیجہ = CURLE_WRITE_ERROR;
      جاؤ برآمد;
    }
    ورنہ اگر(plainwritten == 0) {
      failf(ڈیٹا, "rustls_connection_write: EOF");
      نتیجہ = CURLE_WRITE_ERROR;
      جاؤ برآمد;
    }
  }

  نتیجہ = cr_flush_برآمد(cf, ڈیٹا, rconn);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_برآمد_buffered = plainwritten;
      اگر(*pnwritten) {
        نتیجہ = CURLE_OK;
      }
    }
    جاؤ برآمد;
  }
  ورنہ
    *pnwritten += plainwritten;

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (صحیح)نتیجہ, *pnwritten);
  واپس نتیجہ;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
ساکن uint32_t cr_verify_none(خالی *userdata,
                               ثابت rustls_verify_server_cert_params *params)
{
  (خالی)userdata;
  (خالی)params;
  واپس RUSTLS_نتیجہ_OK;
}

ساکن صحیح read_file_into(ثابت حرف *filename, ساخت dynbuf *برآمد)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  اگر(!f) {
    واپس 0;
  }

  برائے(;;) {
    uint8_t buf[256];
    ثابت size_t rr = fread(buf, 1, حجم(buf), f);
    اگر((!rr && !feof(f)) ||
       curlx_dyn_addn(برآمد, buf, rr)) {
      curlx_fclose(f);
      واپس 0;
    }
    اگر(rr < حجم(buf))
      توڑو;
  }

  واپس curlx_fclose(f) == 0;
}

ساکن خالی
cr_get_selected_ciphers(ساخت Curl_easy *ڈیٹا,
                        ثابت حرف *ciphers12,
                        ثابت حرف *ciphers13,
                        ثابت ساخت rustls_supported_ciphersuite **selected,
                        size_t *selected_سائز)
{
  ثابت size_t supported_len = *selected_سائز;
  ثابت size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ثابت ساخت rustls_supported_ciphersuite *اندراج_2 = تہی;
  ثابت حرف *ciphers = ciphers12;
  size_t گنتی = 0, default13_گنتی = 0, i, j;
  ثابت حرف *ptr, *انتہا;

  DEBUGASSERT(default_len <= supported_len);

  اگر(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    برائے(j = 0; j < default_len; j++) {
      اندراج_2 = rustls_default_crypto_provider_ciphersuites_get(j);
      اگر(rustls_supported_ciphersuite_protocol_نسخہ(اندراج_2) !=
         RUSTLS_TLS_نسخہ_TLSV1_3)
        جاری;

      selected[گنتی++] = اندراج_2;
    }

    default13_گنتی = گنتی;

    اگر(!ciphers)
      ciphers = "";
  }
  ورنہ
    ciphers = ciphers13;

جوڑو_ciphers:
  برائے(ptr = ciphers; ptr[0] != '\0' && گنتی < supported_len; ptr = انتہا) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &انتہا);

    /* Check if cipher is supported */
    اگر(id) {
      برائے(i = 0; i < supported_len; i++) {
        اندراج_2 = rustls_default_crypto_provider_ciphersuites_get(i);
        اگر(rustls_supported_ciphersuite_get_suite(اندراج_2) == id)
          توڑو;
      }
      اگر(i == supported_len)
        id = 0;
    }
    اگر(!id) {
      اگر(ptr[0] != '\0')
        infof(ڈیٹا, "rustls: unknown cipher in list: \"%.*s\"",
              (صحیح)(انتہا - ptr), ptr);
      جاری;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    برائے(i = 0; i < گنتی && selected[i] != اندراج_2; i++)
      ;
    اگر(i < گنتی) {
      اگر(i >= default13_گنتی)
        infof(ڈیٹا, "rustls: duplicate cipher in list: \"%.*s\"",
              (صحیح)(انتہا - ptr), ptr);
      جاری;
    }

    selected[گنتی++] = اندراج_2;
  }

  اگر(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    جاؤ جوڑو_ciphers;
  }

  اگر(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    برائے(j = 0; j < default_len; j++) {
      اندراج_2 = rustls_default_crypto_provider_ciphersuites_get(j);
      اگر(rustls_supported_ciphersuite_protocol_نسخہ(اندراج_2) ==
         RUSTLS_TLS_نسخہ_TLSV1_3)
        جاری;

      /* No duplicates allowed (so selected cannot overflow) */
      برائے(i = 0; i < گنتی && selected[i] != اندراج_2; i++)
        ;
      اگر(i < گنتی)
        جاری;

      selected[گنتی++] = اندراج_2;
    }
  }

  *selected_سائز = گنتی;
}

ساکن خالی cr_keylog_log_cb(ساخت rustls_str label,
                             ثابت uint8_t *client_random,
                             size_t client_random_len, ثابت uint8_t *secret,
                             size_t secret_len)
{
  حرف clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_سائز);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, حجم(clabel), "%.*s", (صحیح)label.len, label.ڈیٹا);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

ساکن CURLcode
init_config_builder(ساخت Curl_cfilter *cf,
                    ساخت Curl_easy *ڈیٹا,
                    ثابت ساخت ssl_filter_config *conn_config,
                    ساخت rustls_client_config_builder **config_builder)
{
  ثابت ساخت rustls_supported_ciphersuite **cipher_suites = تہی;
  ساخت rustls_crypto_provider_builder *custom_provider_builder = تہی;
  ثابت ساخت rustls_crypto_provider *custom_provider = تہی;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_نسخہ_TLSV1_2,
    RUSTLS_TLS_نسخہ_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode نتیجہ = CURLE_OK;
  rustls_نتیجہ rr;

  DEBUGASSERT(conn_config->نسخہ_2 != CURL_SSLVERSION_DEFAULT);
  انتخاب(conn_config->نسخہ_2) {
  حالت CURL_SSLVERSION_TLSv1:
  حالت CURL_SSLVERSION_TLSv1_0:
  حالت CURL_SSLVERSION_TLSv1_1:
  حالت CURL_SSLVERSION_TLSv1_2:
    توڑو;
  حالت CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_نسخہ_TLSV1_3;
    tls_versions_len = 1;
    توڑو;
  طےشدہ:
    failf(ڈیٹا, "rustls: unsupported minimum TLS version value");
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ cleanup;
  }

  انتخاب(conn_config->نسخہ_max) {
  حالت CURL_SSLVERSION_MAX_DEFAULT:
  حالت CURL_SSLVERSION_MAX_NONE:
  حالت CURL_SSLVERSION_MAX_TLSv1_3:
    توڑو;
  حالت CURL_SSLVERSION_MAX_TLSv1_2:
    اگر(tls_versions[0] == RUSTLS_TLS_نسخہ_TLSV1_2) {
      tls_versions_len = 1;
      توڑو;
    }
    FALLTHROUGH();
  حالت CURL_SSLVERSION_MAX_TLSv1_1:
  حالت CURL_SSLVERSION_MAX_TLSv1_0:
  طےشدہ:
    failf(ڈیٹا, "rustls: unsupported maximum TLS version value");
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ cleanup;
  }

#اگر_تعریف_شدہ USE_ECH
  اگر(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_نسخہ_TLSV1_3;
    tls_versions_len = 1;
    infof(ڈیٹا, "rustls: ECH enabled, forcing TLSv1.3");
  }
#ورنہ
  (خالی)cf;
#اگر_ختم /* USE_ECH */

  cipher_suites = curlx_malloc(حجم(*cipher_suites) * cipher_suites_len);
  اگر(!cipher_suites) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ cleanup;
  }

  cr_get_selected_ciphers(ڈیٹا,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  اگر(cipher_suites_len == 0) {
    failf(ڈیٹا, "rustls: no supported cipher in list");
    نتیجہ = CURLE_SSL_CIPHER;
    جاؤ cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr,
                 "failed to create crypto provider builder from default");
    نتیجہ = CURLE_SSL_CIPHER;
    جاؤ cleanup;
  }

  rr =
    rustls_crypto_provider_builder_سیٹ_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr,
                 "failed to set ciphersuites for crypto provider builder");
    نتیجہ = CURLE_SSL_CIPHER;
    جاؤ cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "failed to build custom crypto provider");
    نتیجہ = CURLE_SSL_CIPHER;
    جاؤ cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "failed to create client config builder");
    نتیجہ = CURLE_SSL_CIPHER;
    جاؤ cleanup;
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
  واپس نتیجہ;
}

ساکن خالی
init_config_builder_alpn(ساخت Curl_easy *ڈیٹا,
                         ثابت ساخت ssl_connect_ڈیٹا *connssl,
                         ساخت rustls_client_config_builder *config_builder)
{
  ساخت alpn_proto_buf proto;
  rustls_قاش_بائٹیں alpn[ALPN_ENTRIES_MAX];
  size_t i;

  برائے(i = 0; i < connssl->alpn->گنتی; ++i) {
    alpn[i].ڈیٹا = (ثابت uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = لڑی_لمبائی(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_سیٹ_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->گنتی);
  Curl_alpn_مقصد_proto_str(&proto, connssl->alpn);
  infof(ڈیٹا, VTLS_INFOF_ALPN_OFFER_1STR, proto.ڈیٹا);
}

ساکن CURLcode init_config_builder_verifier_crl(
  ساخت Curl_easy *ڈیٹا,
  ثابت ساخت ssl_filter_config *conn_config,
  ساخت rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode نتیجہ = CURLE_OK;
  ساخت dynbuf crl_contents;
  rustls_نتیجہ rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_سائز);
  اگر(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(ڈیٹا, "rustls: failed to read revocation list file");
    نتیجہ = CURLE_SSL_CRL_BADFILE;
    جاؤ cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_جوڑو_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "failed to parse revocation list");
    نتیجہ = CURLE_SSL_CRL_BADFILE;
    جاؤ cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  واپس نتیجہ;
}

ساکن CURLcode
init_config_builder_verifier(ساخت Curl_easy *ڈیٹا,
                             ساخت rustls_client_config_builder *builder,
                             ثابت ساخت ssl_filter_config *conn_config,
                             ثابت ساخت curl_blob *ca_info_blob,
                             ثابت حرف * ثابت ssl_cafile)
{
  ثابت ساخت rustls_جڑ_cert_store *roots = تہی;
  ساخت rustls_جڑ_cert_store_builder *roots_builder = تہی;
  ساخت rustls_web_pki_server_cert_verifier_builder *verifier_builder = تہی;
  ساخت rustls_server_cert_verifier *server_cert_verifier = تہی;
  rustls_نتیجہ rr = RUSTLS_نتیجہ_OK;
  CURLcode نتیجہ = CURLE_OK;

  roots_builder = rustls_جڑ_cert_store_builder_new();
  اگر(ca_info_blob) {
    rr = rustls_جڑ_cert_store_builder_جوڑو_pem(roots_builder,
                                                ca_info_blob->ڈیٹا,
                                                ca_info_blob->len,
                                                1);
    اگر(rr != RUSTLS_نتیجہ_OK) {
      rustls_failf(ڈیٹا, rr, "failed to parse trusted certificates from blob");
      نتیجہ = CURLE_SSL_CACERT_BADFILE;
      جاؤ cleanup;
    }
  }
  ورنہ اگر(ssl_cafile) {
    rr = rustls_جڑ_cert_store_builder_بھرو_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    اگر(rr != RUSTLS_نتیجہ_OK) {
      rustls_failf(ڈیٹا, rr, "failed to load trusted certificates");
      نتیجہ = CURLE_SSL_CACERT_BADFILE;
      جاؤ cleanup;
    }
  }

  rr = rustls_جڑ_cert_store_builder_build(roots_builder, &roots);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "failed to build trusted root certificate store");
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    جاؤ cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  اگر(!verifier_builder) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ cleanup;
  }

  اگر(conn_config->CRLfile) {
    نتیجہ = init_config_builder_verifier_crl(ڈیٹا,
                                              conn_config,
                                              verifier_builder);
    اگر(نتیجہ) {
      جاؤ cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "failed to build certificate verifier");
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    جاؤ cleanup;
  }

  rustls_client_config_builder_سیٹ_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  اگر(roots_builder) {
    rustls_جڑ_cert_store_builder_free(roots_builder);
  }
  اگر(roots) {
    rustls_جڑ_cert_store_free(roots);
  }
  اگر(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  اگر(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  واپس نتیجہ;
}

ساکن CURLcode init_config_builder_platform_verifier(
  ساخت Curl_easy *ڈیٹا,
  ساخت rustls_client_config_builder *builder)
{
  ساخت rustls_server_cert_verifier *server_cert_verifier = تہی;
  CURLcode نتیجہ = CURLE_OK;
  rustls_نتیجہ rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "failed to create platform certificate verifier");
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    جاؤ cleanup;
  }

  rustls_client_config_builder_سیٹ_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  اگر(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  واپس نتیجہ;
}

ساکن CURLcode
init_config_builder_keylog(ساخت Curl_easy *ڈیٹا,
                           ساخت rustls_client_config_builder *builder)
{
  rustls_نتیجہ rr;

  Curl_tls_keylog_open();
  اگر(!Curl_tls_keylog_enabled()) {
    واپس CURLE_OK;
  }

  rr = rustls_client_config_builder_سیٹ_کلید_log(builder,
                                                cr_keylog_log_cb,
                                                تہی);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    واپس map_error(rr);
  }

  واپس CURLE_OK;
}

ساکن CURLcode
init_config_builder_client_auth(ساخت Curl_easy *ڈیٹا,
                                ثابت ساخت ssl_filter_config *conn_config,
                                ساخت rustls_client_config_builder *builder)
{
  ساخت dynbuf cert_contents;
  ساخت dynbuf کلید_contents;
  rustls_نتیجہ rr;
  ثابت ساخت rustls_certified_کلید *certified_کلید = تہی;
  CURLcode نتیجہ = CURLE_OK;
  ثابت bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ثابت bool have_کلید = conn_config->کلید || conn_config->کلید_blob;

  اگر(have_cert && !have_کلید) {
    failf(ڈیٹا, "rustls: must provide key with certificate");
    واپس CURLE_SSL_CERTPROBLEM;
  }
  ورنہ اگر(!have_cert && have_کلید) {
    failf(ڈیٹا, "rustls: must provide certificate with key");
    واپس CURLE_SSL_CERTPROBLEM;
  }

  اگر(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(ڈیٹا, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    واپس CURLE_SSL_CERTPROBLEM;
  }
  اگر(conn_config->کلید_type && !curl_strequal(conn_config->کلید_type, "PEM")) {
    failf(ڈیٹا, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->کلید_type);
    واپس CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_سائز);
  curlx_dyn_init(&کلید_contents, DYN_KEYFILE_سائز);

  اگر(conn_config->cert_blob) {
    نتیجہ = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->ڈیٹا,
                            conn_config->cert_blob->len);
    اگر(نتیجہ) {
      failf(ڈیٹا, "rustls: failed to copy client certificate blob (%s)",
            نتیجہ == CURLE_TOO_LARGE ? "too large" : "out of memory");
      جاؤ cleanup;
    }
  }
  ورنہ اگر(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(ڈیٹا, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    جاؤ cleanup;
  }

  اگر(conn_config->کلید_blob) {
    نتیجہ = curlx_dyn_addn(&کلید_contents,
                            conn_config->کلید_blob->ڈیٹا,
                            conn_config->کلید_blob->len);
    اگر(نتیجہ) {
      failf(ڈیٹا, "rustls: failed to copy key blob (%s)",
            نتیجہ == CURLE_TOO_LARGE ? "too large" : "out of memory");
      جاؤ cleanup;
    }
  }
  ورنہ اگر(!read_file_into(conn_config->کلید, &کلید_contents)) {
    failf(ڈیٹا, "rustls: failed to read key file: '%s'",
          conn_config->کلید);
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    جاؤ cleanup;
  }

  rr = rustls_certified_کلید_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&کلید_contents),
                                  curlx_dyn_len(&کلید_contents),
                                  &certified_کلید);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "rustls: failed to build certified key");
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    جاؤ cleanup;
  }

  rr = rustls_certified_کلید_کلیدیں_مطابقت(certified_کلید);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr,
                 "rustls: client certificate and keypair files do not match:");
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    جاؤ cleanup;
  }

  rr = rustls_client_config_builder_سیٹ_certified_کلید(builder,
                                                      &certified_کلید,
                                                      1);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "rustls: failed to set certified key");
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    جاؤ cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&کلید_contents);
  اگر(certified_کلید) {
    rustls_certified_کلید_free(certified_کلید);
  }
  واپس نتیجہ;
}

#اگر_تعریف_شدہ USE_ECH

ساکن CURLcode
init_config_builder_ech(ساخت Curl_easy *ڈیٹا,
                        ساخت Curl_cfilter *cf,
                        ساخت rustls_client_config_builder *builder)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت rustls_hpke *hpke = rustls_supported_hpke();
  غیرعلامتی حرف *ech_config = تہی;
  size_t ech_config_len = 0;
  ساخت Curl_dns_اندراج *dns = تہی;
  CURLcode نتیجہ = CURLE_OK;
  rustls_نتیجہ rr;

  اگر(!hpke) {
    failf(ڈیٹا,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
    جاؤ cleanup;
  }

  اگر(conn_config->ech_عوامی) {
    failf(ڈیٹا, "rustls: ECH outername not supported");
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
    جاؤ cleanup;
  }

  اگر(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    اگر(rr != RUSTLS_نتیجہ_OK) {
      rustls_failf(ڈیٹا, rr, "rustls: failed to configure ECH GREASE");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ cleanup;
    }
    واپس CURLE_OK;
  }

  اگر(conn_config->ech && conn_config->ech_config) {
    ثابت حرف *b64 = conn_config->ech_config;
    size_t decode_نتیجہ;
    اگر(!b64[0]) {
      infof(ڈیٹا, "rustls: ECHConfig from command line empty");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_نتیجہ = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    اگر(decode_نتیجہ || !ech_config) {
      infof(ڈیٹا, "rustls: cannot base64 decode ECHConfig from command line");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ cleanup;
    }
  }
  ورنہ {
    ثابت ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
    ثابت ساخت Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(ڈیٹا, cf->sockindex, connssl->peer.origin);

    اگر(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(ڈیٹا, "rustls: ECH requested but no ECHConfig available");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "rustls: failed to configure ECH");
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
    جاؤ cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  اگر(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  اگر(dns) {
    Curl_dns_اندراج_unlink(ڈیٹا, &dns);
  }
  واپس نتیجہ;
}
#اگر_ختم /* USE_ECH */

ساکن CURLcode cr_init_backend(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                ساخت rustls_ssl_backend_ڈیٹا * ثابت backend)
{
  ثابت ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ثابت ساخت ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  ساخت rustls_connection *rconn = تہی;
  ساخت rustls_client_config_builder *config_builder = تہی;

  ثابت ساخت curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? تہی : conn_config->CAfile);
  CURLcode نتیجہ = CURLE_OK;
  rustls_نتیجہ rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  نتیجہ = init_config_builder(cf, ڈیٹا, conn_config, &config_builder);
  اگر(نتیجہ != CURLE_OK) {
    واپس نتیجہ;
  }

  اگر(connssl->alpn) {
    init_config_builder_alpn(ڈیٹا, connssl, config_builder);
  }

  اگر(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_سیٹ_certificate_verifier(
      config_builder, cr_verify_none);
  }
  ورنہ اگر(conn_config->native_ca_store) {
    اگر(conn_config->CRLfile) {
      failf(ڈیٹا, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      واپس CURLE_NOT_BUILT_اندر;
    }
    نتیجہ = init_config_builder_platform_verifier(ڈیٹا, config_builder);
    اگر(نتیجہ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      واپس نتیجہ;
    }
  }
  ورنہ اگر(ca_info_blob || ssl_cafile) {
    نتیجہ = init_config_builder_verifier(ڈیٹا,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    اگر(نتیجہ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      واپس نتیجہ;
    }
  }

  اگر(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->کلید || conn_config->کلید_blob) {
    نتیجہ = init_config_builder_client_auth(ڈیٹا,
                                             conn_config,
                                             config_builder);
    اگر(نتیجہ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      واپس نتیجہ;
    }
  }

#اگر_تعریف_شدہ USE_ECH
  اگر(Curl_ssl_ech_enabled(cf)) {
    نتیجہ = init_config_builder_ech(ڈیٹا, cf, config_builder);
    اگر((نتیجہ != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      واپس نتیجہ;
    }
  }
#اگر_ختم /* USE_ECH */

  نتیجہ = init_config_builder_keylog(ڈیٹا, config_builder);
  اگر(نتیجہ != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    واپس نتیجہ;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "failed to build client config");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  اگر(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(ڈیٹا, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = تہی;
    واپس CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_سیٹ_userdata(rconn, backend);
  backend->conn = rconn;

  واپس نتیجہ;
}

ساکن خالی cr_سیٹ_negotiated_alpn(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا,
                                   ثابت ساخت rustls_connection *rconn)
{
  ساخت ssl_connect_ڈیٹا * ثابت connssl = cf->ctx;
  ثابت uint8_t *protocol = تہی;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_سیٹ_negotiated(cf, ڈیٹا, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
ساکن CURLcode cr_connect(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                           bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا * ثابت connssl = cf->ctx;
  ثابت ساخت rustls_ssl_backend_ڈیٹا * ثابت backend =
    (ساخت rustls_ssl_backend_ڈیٹا *)connssl->backend;
  ثابت ساخت rustls_connection *rconn = تہی;
  CURLcode tmperr = CURLE_OK;
  CURLcode نتیجہ;
  bool چاہتا_read;
  bool چاہتا_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(ڈیٹا, cf, "cr_connect, state=%d", (صحیح)connssl->state);
  *مکمل = FALSE;

#اگر_تعریف_شدہ USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    اگر(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(ڈیٹا, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(ڈیٹا, cf, "need HTTPS-RR for ECH, delaying connect");
      واپس CURLE_OK;
    }
#اگر_ختم /* USE_ECH */

  اگر(!backend->conn) {
    نتیجہ =
      cr_init_backend(cf, ڈیٹا,
                      (ساخت rustls_ssl_backend_ڈیٹا *)connssl->backend);
    CURL_TRC_CF(ڈیٹا, cf, "cr_connect, init backend -> %d", (صحیح)نتیجہ);
    اگر(نتیجہ)
      واپس نتیجہ;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  برائے(;;) {
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
      cr_سیٹ_negotiated_alpn(cf, ڈیٹا, rconn);
      tmperr = cr_send(cf, ڈیٹا, تہی, 0, &nwritten);
      اگر(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        واپس CURLE_OK;
      }
      ورنہ اگر(tmperr != CURLE_OK) {
        واپس tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#اگر_تعریف_شدہ CURLVERBOSE
        ثابت uint16_t proto = rustls_connection_get_protocol_نسخہ(rconn);
        ثابت rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ثابت rustls_str kex_گروہ_name =
          rustls_connection_get_negotiated_کلید_exchange_گروہ_name(rconn);
        ثابت حرف *ver = "TLS version unknown";
        اگر(proto == RUSTLS_TLS_نسخہ_TLSV1_3)
          ver = "TLSv1.3";
        اگر(proto == RUSTLS_TLS_نسخہ_TLSV1_2)
          ver = "TLSv1.2";
#اگر_ختم
        infof(ڈیٹا,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (صحیح)ciphersuite_name.len,
              ciphersuite_name.ڈیٹا,
              (صحیح)kex_گروہ_name.len,
              kex_گروہ_name.ڈیٹا);
      }
      اگر(ڈیٹا->سیٹ.ssl.certinfo) {
        size_t عدد_certs = 0;
        size_t i;
        جب(rustls_connection_get_peer_certificate(rconn, عدد_certs)) {
          عدد_certs++;
          اگر(عدد_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(ڈیٹا, "%zu certificates is more than allowed (%d)",
                  عدد_certs, MAX_ALLOWED_CERT_AMOUNT);
            واپس CURLE_SSL_CONNECT_ERROR;
          }
        }
        نتیجہ = Curl_ssl_init_certinfo(ڈیٹا, (صحیح)عدد_certs);
        اگر(نتیجہ)
          واپس نتیجہ;
        برائے(i = 0; i < عدد_certs; i++) {
          ثابت rustls_certificate *cert;
          ثابت غیرعلامتی حرف *der_ڈیٹا;
          size_t der_len;
          rustls_نتیجہ rresult = RUSTLS_نتیجہ_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_ڈیٹا, &der_len);
          اگر(rresult != RUSTLS_نتیجہ_OK) {
            حرف errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, حجم(errorbuf), &errorlen);
            failf(ڈیٹا,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (صحیح)errorlen, errorbuf);
            واپس map_error(rresult);
          }
          نتیجہ = Curl_نکالو_certinfo(ڈیٹا, (صحیح)i,
                                         der_ڈیٹا, der_ڈیٹا + der_len);
          اگر(نتیجہ)
            واپس نتیجہ;
        }
      }

      connssl->state = ssl_connection_complete;
      *مکمل = TRUE;
      واپس CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    چاہتا_read = rustls_connection_چاہتا_read(rconn);
    چاہتا_write = rustls_connection_چاہتا_write(rconn) ||
                  backend->plain_برآمد_buffered;
    DEBUGASSERT(چاہتا_read || چاہتا_write);

    اگر(چاہتا_write) {
      size_t nwritten;
      CURL_TRC_CF(ڈیٹا, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, ڈیٹا, تہی, 0, &nwritten);
      اگر(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(ڈیٹا, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        واپس CURLE_OK;
      }
      ورنہ اگر(tmperr != CURLE_OK) {
        واپس tmperr;
      }
    }

    اگر(چاہتا_read) {
      CURL_TRC_CF(ڈیٹا, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_مزید(cf, ڈیٹا, &tmperr);
      اگر(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(ڈیٹا, "TLS connect error: Connection closed abruptly");
        واپس CURLE_SSL_CONNECT_ERROR;
      }
      اگر(nread < 0) {
        اگر(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(ڈیٹا, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          واپس CURLE_OK;
        }
        ورنہ اگر(tmperr == CURLE_RECV_ERROR) {
          واپس CURLE_SSL_CONNECT_ERROR;
        }
        ورنہ {
          واپس tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

ساکن خالی *cr_get_internals(ساخت ssl_connect_ڈیٹا *connssl, CURLINFO info)
{
  ساخت rustls_ssl_backend_ڈیٹا *backend =
    (ساخت rustls_ssl_backend_ڈیٹا *)connssl->backend;
  (خالی)info;
  DEBUGASSERT(backend);
  واپس backend->conn;
}

ساکن CURLcode cr_shutdown(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                            ثابت bool send_shutdown, bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت rustls_ssl_backend_ڈیٹا *backend =
    (ساخت rustls_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  اگر(!backend->conn || cf->shutdown) {
    *مکمل = TRUE;
    جاؤ برآمد;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *مکمل = FALSE;

  اگر(!backend->بھیجا_shutdown) {
    /* do this only once */
    backend->بھیجا_shutdown = TRUE;
    اگر(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  نتیجہ = cr_send(cf, ڈیٹا, تہی, 0, &nwritten);
  اگر(نتیجہ) {
    اگر(نتیجہ == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتیجہ = CURLE_OK;
      جاؤ برآمد;
    }
    DEBUGASSERT(نتیجہ);
    CURL_TRC_CF(ڈیٹا, cf, "shutdown send failed: %d", (صحیح)نتیجہ);
    جاؤ برآمد;
  }

  برائے(i = 0; i < 10; ++i) {
    حرف buf[1024];
    نتیجہ = cr_recv(cf, ڈیٹا, buf, (صحیح)حجم(buf), &nread);
    اگر(نتیجہ)
      توڑو;
  }

  اگر(نتیجہ == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    نتیجہ = CURLE_OK;
  }
  ورنہ اگر(نتیجہ) {
    DEBUGASSERT(نتیجہ);
    CURL_TRC_CF(ڈیٹا, cf, "shutdown, error: %d", (صحیح)نتیجہ);
  }
  ورنہ اگر(nread == 0) {
    /* We got the close notify alert and are done. */
    *مکمل = TRUE;
  }

برآمد:
  cf->shutdown = (نتیجہ || *مکمل);
  واپس نتیجہ;
}

ساکن خالی cr_close(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ثابت ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت rustls_ssl_backend_ڈیٹا *backend =
    (ساخت rustls_ssl_backend_ڈیٹا *)connssl->backend;

  (خالی)ڈیٹا;
  DEBUGASSERT(backend);
  اگر(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = تہی;
  }
  اگر(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = تہی;
  }
}

ساکن size_t cr_نسخہ(حرف *بفر, size_t سائز)
{
  ثابت ساخت rustls_str ver = rustls_نسخہ();
  واپس curl_msnprintf(بفر, سائز, "%.*s", (صحیح)ver.len, ver.ڈیٹا);
}

ساکن CURLcode cr_random(ساخت Curl_easy *ڈیٹا, غیرعلامتی حرف *entropy,
                          size_t لمبائی)
{
  rustls_نتیجہ rresult = 0;
  (خالی)ڈیٹا;
  rresult = rustls_default_crypto_provider_random(entropy, لمبائی);
  واپس map_error(rresult);
}

ساکن خالی cr_cleanup(خالی)
{
  Curl_tls_keylog_close();
}

ثابت ساخت Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  حجم(ساخت rustls_ssl_backend_ڈیٹا),

  تہی,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_نسخہ,                      /*نسخہ*/
  cr_shutdown,                     /* shutdown */
  cr_ڈیٹا_pending,                 /* data_pending */
  cr_random,                       /* random */
  تہی,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  تہی,                            /* close_all */
  تہی,                            /* engines_list */
  تہی,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  تہی,                            /* get_channel_binding */
};

#اگر_ختم /* USE_RUSTLS */
