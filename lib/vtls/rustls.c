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

#جے_تعریف_شدہ USE_RUSTLS

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
#جے_تعریف_شدہ USE_ECH
#ضمن "curlx/base64.h"
#جے_انت

#إذا EAGAIN != EWOULDBLOCK
#تعریف_کرو RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#وإلا
#تعریف_کرو RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#جے_انت

بنية rustls_ssl_backend_انکڑے {
  ثابت بنية rustls_client_config *config;
  بنية rustls_connection *conn;
  size_t plain_باہرلا_buffered;
  ثنائی_ہندسہ(انکڑے_وچ_pending);
  ثنائی_ہندسہ(گھلیا_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
ساكن CURLcode map_error(ثابت rustls_نتیجہ r)
{
  إذا(rustls_نتیجہ_is_cert_error(r)) {
    أرجع CURLE_PEER_FAILED_VERIFICATION;
  }
  بدل(r) {
  حالة RUSTLS_نتیجہ_OK:
    أرجع CURLE_OK;
  حالة RUSTLS_نتیجہ_NULL_PARAMETER:
    أرجع CURLE_BAD_FUNCTION_دلیل;
  افتراضي:
    أرجع CURLE_RECV_ERROR;
  }
}

ساكن فراغ rustls_failf(بنية Curl_easy *انکڑے, ثابت rustls_نتیجہ rr,
                         ثابت حرف *پیغام)
{
  حرف errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, حجم(errorbuf), &errorlen);
  failf(انکڑے, "%s: %.*s", پیغام, (صحيح)errorlen, errorbuf);
}

ساكن bool cr_انکڑے_pending(بنية Curl_cfilter *cf,
                            ثابت بنية Curl_easy *انکڑے)
{
  ثابت بنية ssl_connect_انکڑے *ctx = cf->ctx;
  بنية rustls_ssl_backend_انکڑے *backend;

  (فراغ)انکڑے;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية rustls_ssl_backend_انکڑے *)ctx->backend;
  أرجع (bool)backend->انکڑے_وچ_pending;
}

بنية io_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *انکڑے;
};

ساكن صحيح read_cb(فراغ *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *باہرلا_n)
{
  ثابت بنية io_ctx *io_ctx = userdata;
  بنية ssl_connect_انکڑے * ثابت connssl = io_ctx->cf->ctx;
  CURLcode نتیجہ;
  صحيح ret = 0;
  size_t nread;

  نتیجہ = Curl_conn_cf_recv(io_ctx->cf->اگلا, io_ctx->انکڑے,
                             (حرف *)buf, len, &nread);
  إذا(نتیجہ) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    إذا(نتیجہ == CURLE_AGAIN)
      ret = EAGAIN;
    وإلا
      ret = EINVAL;
  }
  وإلا إذا(nread == 0)
    connssl->peer_closed = TRUE;
  *باہرلا_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->انکڑے, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (صحيح)نتیجہ, nread);
  أرجع ret;
}

ساكن صحيح write_cb(فراغ *userdata, ثابت uint8_t *buf, uintptr_t len,
                    uintptr_t *باہرلا_n)
{
  ثابت بنية io_ctx *io_ctx = userdata;
  CURLcode نتیجہ;
  صحيح ret = 0;
  size_t nwritten;

  نتیجہ = Curl_conn_cf_send(io_ctx->cf->اگلا, io_ctx->انکڑے,
                             buf, len, FALSE, &nwritten);
  إذا(نتیجہ) {
    nwritten = 0;
    إذا(نتیجہ == CURLE_AGAIN)
      ret = EAGAIN;
    وإلا
      ret = EINVAL;
  }
  *باہرلا_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->انکڑے, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (صحيح)نتیجہ, nwritten);
  أرجع ret;
}

ساكن ssize_t tls_recv_ہور(بنية Curl_cfilter *cf,
                             بنية Curl_easy *انکڑے, CURLcode *err)
{
  ثابت بنية ssl_connect_انکڑے * ثابت connssl = cf->ctx;
  بنية rustls_ssl_backend_انکڑے * ثابت backend =
    (بنية rustls_ssl_backend_انکڑے *)connssl->backend;
  بنية io_ctx io_ctx;
  size_t tls_بائٹاں_read = 0;
  rustls_io_نتیجہ io_error;
  rustls_نتیجہ rresult = 0;

  io_ctx.cf = cf;
  io_ctx.انکڑے = انکڑے;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_بائٹاں_read);
  إذا(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    أرجع -1;
  }
  وإلا إذا(io_error) {
    حرف بفر[STRERROR_LEN];
    failf(انکڑے, "reading from socket: %s",
          curlx_strerror(io_error, بفر, حجم(بفر)));
    *err = CURLE_RECV_ERROR;
    أرجع -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  إذا(rresult != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    أرجع -1;
  }

  backend->انکڑے_وچ_pending = TRUE;
  *err = CURLE_OK;
  أرجع (ssize_t)tls_بائٹاں_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
ساكن CURLcode cr_recv(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                        حرف *plainbuf, size_t plainlen, size_t *pnread)
{
  ثابت بنية ssl_connect_انکڑے * ثابت connssl = cf->ctx;
  بنية rustls_ssl_backend_انکڑے * ثابت backend =
    (بنية rustls_ssl_backend_انکڑے *)connssl->backend;
  بنية rustls_connection *rconn = عدم;
  CURLcode نتیجہ = CURLE_OK;
  size_t n = 0;
  rustls_نتیجہ rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  طالما(*pnread < plainlen) {
    إذا(!backend->انکڑے_وچ_pending) {
      إذا(tls_recv_ہور(cf, انکڑے, &نتیجہ) < 0) {
        إذا(نتیجہ != CURLE_AGAIN) {
          اذهب باہرلا;
        }
        نتیجہ = CURLE_OK;
        اكسر;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    إذا(rresult == RUSTLS_نتیجہ_PLAINTEXT_خالی_تھاواں) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->انکڑے_وچ_pending = FALSE;
      اكسر;
    }
    وإلا إذا(rresult == RUSTLS_نتیجہ_UNEXPECTED_EOF) {
      failf(انکڑے, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      نتیجہ = CURLE_RECV_ERROR;
      اذهب باہرلا;
    }
    وإلا إذا(rresult != RUSTLS_نتیجہ_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(انکڑے, rresult, "rustls_connection_read");
      نتیجہ = CURLE_RECV_ERROR;
      اذهب باہرلا;
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
    نتیجہ = CURLE_AGAIN;
  }

باہرلا:
  CURL_TRC_CF(انکڑے, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (صحيح)نتیجہ, *pnread);
  أرجع نتیجہ;
}

ساكن CURLcode cr_flush_باہرلا(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                             بنية rustls_connection *rconn)
{
  بنية io_ctx io_ctx;
  rustls_io_نتیجہ io_error;
  size_t tlswritten = 0;
  size_t tlswritten_کل = 0;

  io_ctx.cf = cf;
  io_ctx.انکڑے = انکڑے;

  طالما(rustls_connection_چاہندا_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    إذا(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(انکڑے, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_کل);
      أرجع CURLE_AGAIN;
    }
    وإلا إذا(io_error) {
      حرف بفر[STRERROR_LEN];
      failf(انکڑے, "writing to socket: %s",
            curlx_strerror(io_error, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }
    إذا(tlswritten == 0) {
      failf(انکڑے, "EOF in swrite");
      أرجع CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(انکڑے, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_کل += tlswritten;
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
ساكن CURLcode cr_send(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                        ثابت فراغ *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ثابت بنية ssl_connect_انکڑے * ثابت connssl = cf->ctx;
  بنية rustls_ssl_backend_انکڑے * ثابت backend =
    (بنية rustls_ssl_backend_انکڑے *)connssl->backend;
  بنية rustls_connection *rconn = عدم;
  size_t plainwritten = 0;
  ثابت بنانشانی حرف *buf = plainbuf;
  CURLcode نتیجہ = CURLE_OK;
  size_t blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(انکڑے, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  إذا(backend->plain_باہرلا_buffered) {
    نتیجہ = cr_flush_باہرلا(cf, انکڑے, rconn);
    CURL_TRC_CF(انکڑے, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_باہرلا_buffered, (صحيح)نتیجہ);
    إذا(نتیجہ)
      أرجع نتیجہ;
    إذا(blen > backend->plain_باہرلا_buffered) {
      blen -= backend->plain_باہرلا_buffered;
      buf += backend->plain_باہرلا_buffered;
    }
    وإلا
      blen = 0;
    *pnwritten += backend->plain_باہرلا_buffered;
    backend->plain_باہرلا_buffered = 0;
  }

  إذا(blen > 0) {
    rustls_نتیجہ rresult;
    CURL_TRC_CF(انکڑے, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    إذا(rresult != RUSTLS_نتیجہ_OK) {
      rustls_failf(انکڑے, rresult, "rustls_connection_write");
      نتیجہ = CURLE_WRITE_ERROR;
      اذهب باہرلا;
    }
    وإلا إذا(plainwritten == 0) {
      failf(انکڑے, "rustls_connection_write: EOF");
      نتیجہ = CURLE_WRITE_ERROR;
      اذهب باہرلا;
    }
  }

  نتیجہ = cr_flush_باہرلا(cf, انکڑے, rconn);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_باہرلا_buffered = plainwritten;
      إذا(*pnwritten) {
        نتیجہ = CURLE_OK;
      }
    }
    اذهب باہرلا;
  }
  وإلا
    *pnwritten += plainwritten;

باہرلا:
  CURL_TRC_CF(انکڑے, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (صحيح)نتیجہ, *pnwritten);
  أرجع نتیجہ;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
ساكن uint32_t cr_verify_none(فراغ *userdata,
                               ثابت rustls_verify_server_cert_params *params)
{
  (فراغ)userdata;
  (فراغ)params;
  أرجع RUSTLS_نتیجہ_OK;
}

ساكن صحيح read_file_into(ثابت حرف *filename, بنية dynbuf *باہرلا)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  إذا(!f) {
    أرجع 0;
  }

  لكل(;;) {
    uint8_t buf[256];
    ثابت size_t rr = fread(buf, 1, حجم(buf), f);
    إذا((!rr && !feof(f)) ||
       curlx_dyn_addn(باہرلا, buf, rr)) {
      curlx_fclose(f);
      أرجع 0;
    }
    إذا(rr < حجم(buf))
      اكسر;
  }

  أرجع curlx_fclose(f) == 0;
}

ساكن فراغ
cr_get_selected_ciphers(بنية Curl_easy *انکڑے,
                        ثابت حرف *ciphers12,
                        ثابت حرف *ciphers13,
                        ثابت بنية rustls_supported_ciphersuite **selected,
                        size_t *selected_ناپ)
{
  ثابت size_t supported_len = *selected_ناپ;
  ثابت size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ثابت بنية rustls_supported_ciphersuite *اندراج = عدم;
  ثابت حرف *ciphers = ciphers12;
  size_t گنتی = 0, default13_گنتی = 0, i, j;
  ثابت حرف *ptr, *سرا;

  DEBUGASSERT(default_len <= supported_len);

  إذا(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    لكل(j = 0; j < default_len; j++) {
      اندراج = rustls_default_crypto_provider_ciphersuites_get(j);
      إذا(rustls_supported_ciphersuite_protocol_نسخہ(اندراج) !=
         RUSTLS_TLS_نسخہ_TLSV1_3)
        استمر;

      selected[گنتی++] = اندراج;
    }

    default13_گنتی = گنتی;

    إذا(!ciphers)
      ciphers = "";
  }
  وإلا
    ciphers = ciphers13;

جوڑو_ciphers:
  لكل(ptr = ciphers; ptr[0] != '\0' && گنتی < supported_len; ptr = سرا) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &سرا);

    /* Check if cipher is supported */
    إذا(id) {
      لكل(i = 0; i < supported_len; i++) {
        اندراج = rustls_default_crypto_provider_ciphersuites_get(i);
        إذا(rustls_supported_ciphersuite_get_suite(اندراج) == id)
          اكسر;
      }
      إذا(i == supported_len)
        id = 0;
    }
    إذا(!id) {
      إذا(ptr[0] != '\0')
        infof(انکڑے, "rustls: unknown cipher in list: \"%.*s\"",
              (صحيح)(سرا - ptr), ptr);
      استمر;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    لكل(i = 0; i < گنتی && selected[i] != اندراج; i++)
      ;
    إذا(i < گنتی) {
      إذا(i >= default13_گنتی)
        infof(انکڑے, "rustls: duplicate cipher in list: \"%.*s\"",
              (صحيح)(سرا - ptr), ptr);
      استمر;
    }

    selected[گنتی++] = اندراج;
  }

  إذا(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    اذهب جوڑو_ciphers;
  }

  إذا(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    لكل(j = 0; j < default_len; j++) {
      اندراج = rustls_default_crypto_provider_ciphersuites_get(j);
      إذا(rustls_supported_ciphersuite_protocol_نسخہ(اندراج) ==
         RUSTLS_TLS_نسخہ_TLSV1_3)
        استمر;

      /* No duplicates allowed (so selected cannot overflow) */
      لكل(i = 0; i < گنتی && selected[i] != اندراج; i++)
        ;
      إذا(i < گنتی)
        استمر;

      selected[گنتی++] = اندراج;
    }
  }

  *selected_ناپ = گنتی;
}

ساكن فراغ cr_keylog_log_cb(بنية rustls_str label,
                             ثابت uint8_t *client_random,
                             size_t client_random_len, ثابت uint8_t *secret,
                             size_t secret_len)
{
  حرف clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_ناپ);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, حجم(clabel), "%.*s", (صحيح)label.len, label.انکڑے);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

ساكن CURLcode
init_config_builder(بنية Curl_cfilter *cf,
                    بنية Curl_easy *انکڑے,
                    ثابت بنية ssl_filter_config *conn_config,
                    بنية rustls_client_config_builder **config_builder)
{
  ثابت بنية rustls_supported_ciphersuite **cipher_suites = عدم;
  بنية rustls_crypto_provider_builder *custom_provider_builder = عدم;
  ثابت بنية rustls_crypto_provider *custom_provider = عدم;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_نسخہ_TLSV1_2,
    RUSTLS_TLS_نسخہ_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode نتیجہ = CURLE_OK;
  rustls_نتیجہ rr;

  DEBUGASSERT(conn_config->نسخہ_2 != CURL_SSLVERSION_DEFAULT);
  بدل(conn_config->نسخہ_2) {
  حالة CURL_SSLVERSION_TLSv1:
  حالة CURL_SSLVERSION_TLSv1_0:
  حالة CURL_SSLVERSION_TLSv1_1:
  حالة CURL_SSLVERSION_TLSv1_2:
    اكسر;
  حالة CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_نسخہ_TLSV1_3;
    tls_versions_len = 1;
    اكسر;
  افتراضي:
    failf(انکڑے, "rustls: unsupported minimum TLS version value");
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب cleanup;
  }

  بدل(conn_config->نسخہ_max) {
  حالة CURL_SSLVERSION_MAX_DEFAULT:
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_TLSv1_3:
    اكسر;
  حالة CURL_SSLVERSION_MAX_TLSv1_2:
    إذا(tls_versions[0] == RUSTLS_TLS_نسخہ_TLSV1_2) {
      tls_versions_len = 1;
      اكسر;
    }
    FALLTHROUGH();
  حالة CURL_SSLVERSION_MAX_TLSv1_1:
  حالة CURL_SSLVERSION_MAX_TLSv1_0:
  افتراضي:
    failf(انکڑے, "rustls: unsupported maximum TLS version value");
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب cleanup;
  }

#جے_تعریف_شدہ USE_ECH
  إذا(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_نسخہ_TLSV1_3;
    tls_versions_len = 1;
    infof(انکڑے, "rustls: ECH enabled, forcing TLSv1.3");
  }
#وإلا
  (فراغ)cf;
#جے_انت /* USE_ECH */

  cipher_suites = curlx_malloc(حجم(*cipher_suites) * cipher_suites_len);
  إذا(!cipher_suites) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب cleanup;
  }

  cr_get_selected_ciphers(انکڑے,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  إذا(cipher_suites_len == 0) {
    failf(انکڑے, "rustls: no supported cipher in list");
    نتیجہ = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr,
                 "failed to create crypto provider builder from default");
    نتیجہ = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr =
    rustls_crypto_provider_builder_سیٹ_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr,
                 "failed to set ciphersuites for crypto provider builder");
    نتیجہ = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "failed to build custom crypto provider");
    نتیجہ = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "failed to create client config builder");
    نتیجہ = CURLE_SSL_CIPHER;
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
  أرجع نتیجہ;
}

ساكن فراغ
init_config_builder_alpn(بنية Curl_easy *انکڑے,
                         ثابت بنية ssl_connect_انکڑے *connssl,
                         بنية rustls_client_config_builder *config_builder)
{
  بنية alpn_proto_buf proto;
  rustls_ٹکڑا_بائٹاں alpn[ALPN_ENTRIES_MAX];
  size_t i;

  لكل(i = 0; i < connssl->alpn->گنتی; ++i) {
    alpn[i].انکڑے = (ثابت uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = لڑی_لمبائی(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_سیٹ_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->گنتی);
  Curl_alpn_منزل_proto_str(&proto, connssl->alpn);
  infof(انکڑے, VTLS_INFOF_ALPN_OFFER_1STR, proto.انکڑے);
}

ساكن CURLcode init_config_builder_verifier_crl(
  بنية Curl_easy *انکڑے,
  ثابت بنية ssl_filter_config *conn_config,
  بنية rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode نتیجہ = CURLE_OK;
  بنية dynbuf crl_contents;
  rustls_نتیجہ rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_ناپ);
  إذا(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(انکڑے, "rustls: failed to read revocation list file");
    نتیجہ = CURLE_SSL_CRL_BADFILE;
    اذهب cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_جوڑو_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "failed to parse revocation list");
    نتیجہ = CURLE_SSL_CRL_BADFILE;
    اذهب cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  أرجع نتیجہ;
}

ساكن CURLcode
init_config_builder_verifier(بنية Curl_easy *انکڑے,
                             بنية rustls_client_config_builder *builder,
                             ثابت بنية ssl_filter_config *conn_config,
                             ثابت بنية curl_blob *ca_info_blob,
                             ثابت حرف * ثابت ssl_cafile)
{
  ثابت بنية rustls_جڑ_cert_store *roots = عدم;
  بنية rustls_جڑ_cert_store_builder *roots_builder = عدم;
  بنية rustls_web_pki_server_cert_verifier_builder *verifier_builder = عدم;
  بنية rustls_server_cert_verifier *server_cert_verifier = عدم;
  rustls_نتیجہ rr = RUSTLS_نتیجہ_OK;
  CURLcode نتیجہ = CURLE_OK;

  roots_builder = rustls_جڑ_cert_store_builder_new();
  إذا(ca_info_blob) {
    rr = rustls_جڑ_cert_store_builder_جوڑو_pem(roots_builder,
                                                ca_info_blob->انکڑے,
                                                ca_info_blob->len,
                                                1);
    إذا(rr != RUSTLS_نتیجہ_OK) {
      rustls_failf(انکڑے, rr, "failed to parse trusted certificates from blob");
      نتیجہ = CURLE_SSL_CACERT_BADFILE;
      اذهب cleanup;
    }
  }
  وإلا إذا(ssl_cafile) {
    rr = rustls_جڑ_cert_store_builder_لدو_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    إذا(rr != RUSTLS_نتیجہ_OK) {
      rustls_failf(انکڑے, rr, "failed to load trusted certificates");
      نتیجہ = CURLE_SSL_CACERT_BADFILE;
      اذهب cleanup;
    }
  }

  rr = rustls_جڑ_cert_store_builder_build(roots_builder, &roots);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "failed to build trusted root certificate store");
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  إذا(!verifier_builder) {
    نتیجہ = CURLE_باہرلا_دا_حافظہ;
    اذهب cleanup;
  }

  إذا(conn_config->CRLfile) {
    نتیجہ = init_config_builder_verifier_crl(انکڑے,
                                              conn_config,
                                              verifier_builder);
    إذا(نتیجہ) {
      اذهب cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "failed to build certificate verifier");
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  rustls_client_config_builder_سیٹ_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  إذا(roots_builder) {
    rustls_جڑ_cert_store_builder_free(roots_builder);
  }
  إذا(roots) {
    rustls_جڑ_cert_store_free(roots);
  }
  إذا(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  إذا(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  أرجع نتیجہ;
}

ساكن CURLcode init_config_builder_platform_verifier(
  بنية Curl_easy *انکڑے,
  بنية rustls_client_config_builder *builder)
{
  بنية rustls_server_cert_verifier *server_cert_verifier = عدم;
  CURLcode نتیجہ = CURLE_OK;
  rustls_نتیجہ rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "failed to create platform certificate verifier");
    نتیجہ = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  rustls_client_config_builder_سیٹ_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  إذا(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  أرجع نتیجہ;
}

ساكن CURLcode
init_config_builder_keylog(بنية Curl_easy *انکڑے,
                           بنية rustls_client_config_builder *builder)
{
  rustls_نتیجہ rr;

  Curl_tls_keylog_open();
  إذا(!Curl_tls_keylog_enabled()) {
    أرجع CURLE_OK;
  }

  rr = rustls_client_config_builder_سیٹ_کلید_log(builder,
                                                cr_keylog_log_cb,
                                                عدم);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    أرجع map_error(rr);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode
init_config_builder_client_auth(بنية Curl_easy *انکڑے,
                                ثابت بنية ssl_filter_config *conn_config,
                                بنية rustls_client_config_builder *builder)
{
  بنية dynbuf cert_contents;
  بنية dynbuf کلید_contents;
  rustls_نتیجہ rr;
  ثابت بنية rustls_certified_کلید *certified_کلید = عدم;
  CURLcode نتیجہ = CURLE_OK;
  ثابت bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ثابت bool have_کلید = conn_config->کلید || conn_config->کلید_blob;

  إذا(have_cert && !have_کلید) {
    failf(انکڑے, "rustls: must provide key with certificate");
    أرجع CURLE_SSL_CERTPROBLEM;
  }
  وإلا إذا(!have_cert && have_کلید) {
    failf(انکڑے, "rustls: must provide certificate with key");
    أرجع CURLE_SSL_CERTPROBLEM;
  }

  إذا(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(انکڑے, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    أرجع CURLE_SSL_CERTPROBLEM;
  }
  إذا(conn_config->کلید_type && !curl_strequal(conn_config->کلید_type, "PEM")) {
    failf(انکڑے, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->کلید_type);
    أرجع CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_ناپ);
  curlx_dyn_init(&کلید_contents, DYN_KEYFILE_ناپ);

  إذا(conn_config->cert_blob) {
    نتیجہ = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->انکڑے,
                            conn_config->cert_blob->len);
    إذا(نتیجہ) {
      failf(انکڑے, "rustls: failed to copy client certificate blob (%s)",
            نتیجہ == CURLE_TOO_LARGE ? "too large" : "out of memory");
      اذهب cleanup;
    }
  }
  وإلا إذا(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(انکڑے, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  إذا(conn_config->کلید_blob) {
    نتیجہ = curlx_dyn_addn(&کلید_contents,
                            conn_config->کلید_blob->انکڑے,
                            conn_config->کلید_blob->len);
    إذا(نتیجہ) {
      failf(انکڑے, "rustls: failed to copy key blob (%s)",
            نتیجہ == CURLE_TOO_LARGE ? "too large" : "out of memory");
      اذهب cleanup;
    }
  }
  وإلا إذا(!read_file_into(conn_config->کلید, &کلید_contents)) {
    failf(انکڑے, "rustls: failed to read key file: '%s'",
          conn_config->کلید);
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  rr = rustls_certified_کلید_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&کلید_contents),
                                  curlx_dyn_len(&کلید_contents),
                                  &certified_کلید);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "rustls: failed to build certified key");
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  rr = rustls_certified_کلید_کلیدیں_میل(certified_کلید);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr,
                 "rustls: client certificate and keypair files do not match:");
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  rr = rustls_client_config_builder_سیٹ_certified_کلید(builder,
                                                      &certified_کلید,
                                                      1);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "rustls: failed to set certified key");
    نتیجہ = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&کلید_contents);
  إذا(certified_کلید) {
    rustls_certified_کلید_free(certified_کلید);
  }
  أرجع نتیجہ;
}

#جے_تعریف_شدہ USE_ECH

ساكن CURLcode
init_config_builder_ech(بنية Curl_easy *انکڑے,
                        بنية Curl_cfilter *cf,
                        بنية rustls_client_config_builder *builder)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت rustls_hpke *hpke = rustls_supported_hpke();
  بنانشانی حرف *ech_config = عدم;
  size_t ech_config_len = 0;
  بنية Curl_dns_اندراج *dns = عدم;
  CURLcode نتیجہ = CURLE_OK;
  rustls_نتیجہ rr;

  إذا(!hpke) {
    failf(انکڑے,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
    اذهب cleanup;
  }

  إذا(conn_config->ech_عوامی) {
    failf(انکڑے, "rustls: ECH outername not supported");
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
    اذهب cleanup;
  }

  إذا(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    إذا(rr != RUSTLS_نتیجہ_OK) {
      rustls_failf(انکڑے, rr, "rustls: failed to configure ECH GREASE");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
    أرجع CURLE_OK;
  }

  إذا(conn_config->ech && conn_config->ech_config) {
    ثابت حرف *b64 = conn_config->ech_config;
    size_t decode_نتیجہ;
    إذا(!b64[0]) {
      infof(انکڑے, "rustls: ECHConfig from command line empty");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_نتیجہ = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    إذا(decode_نتیجہ || !ech_config) {
      infof(انکڑے, "rustls: cannot base64 decode ECHConfig from command line");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
  }
  وإلا {
    ثابت بنية ssl_connect_انکڑے *connssl = cf->ctx;
    ثابت بنية Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(انکڑے, cf->sockindex, connssl->peer.origin);

    إذا(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(انکڑے, "rustls: ECH requested but no ECHConfig available");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "rustls: failed to configure ECH");
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
    اذهب cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  إذا(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  إذا(dns) {
    Curl_dns_اندراج_unlink(انکڑے, &dns);
  }
  أرجع نتیجہ;
}
#جے_انت /* USE_ECH */

ساكن CURLcode cr_init_backend(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                بنية rustls_ssl_backend_انکڑے * ثابت backend)
{
  ثابت بنية ssl_connect_انکڑے *connssl = cf->ctx;
  ثابت بنية ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  بنية rustls_connection *rconn = عدم;
  بنية rustls_client_config_builder *config_builder = عدم;

  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? عدم : conn_config->CAfile);
  CURLcode نتیجہ = CURLE_OK;
  rustls_نتیجہ rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  نتیجہ = init_config_builder(cf, انکڑے, conn_config, &config_builder);
  إذا(نتیجہ != CURLE_OK) {
    أرجع نتیجہ;
  }

  إذا(connssl->alpn) {
    init_config_builder_alpn(انکڑے, connssl, config_builder);
  }

  إذا(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_سیٹ_certificate_verifier(
      config_builder, cr_verify_none);
  }
  وإلا إذا(conn_config->native_ca_store) {
    إذا(conn_config->CRLfile) {
      failf(انکڑے, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      أرجع CURLE_NOT_BUILT_وچ;
    }
    نتیجہ = init_config_builder_platform_verifier(انکڑے, config_builder);
    إذا(نتیجہ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      أرجع نتیجہ;
    }
  }
  وإلا إذا(ca_info_blob || ssl_cafile) {
    نتیجہ = init_config_builder_verifier(انکڑے,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    إذا(نتیجہ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      أرجع نتیجہ;
    }
  }

  إذا(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->کلید || conn_config->کلید_blob) {
    نتیجہ = init_config_builder_client_auth(انکڑے,
                                             conn_config,
                                             config_builder);
    إذا(نتیجہ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      أرجع نتیجہ;
    }
  }

#جے_تعریف_شدہ USE_ECH
  إذا(Curl_ssl_ech_enabled(cf)) {
    نتیجہ = init_config_builder_ech(انکڑے, cf, config_builder);
    إذا((نتیجہ != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      أرجع نتیجہ;
    }
  }
#جے_انت /* USE_ECH */

  نتیجہ = init_config_builder_keylog(انکڑے, config_builder);
  إذا(نتیجہ != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    أرجع نتیجہ;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "failed to build client config");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  إذا(rr != RUSTLS_نتیجہ_OK) {
    rustls_failf(انکڑے, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = عدم;
    أرجع CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_سیٹ_userdata(rconn, backend);
  backend->conn = rconn;

  أرجع نتیجہ;
}

ساكن فراغ cr_سیٹ_negotiated_alpn(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *انکڑے,
                                   ثابت بنية rustls_connection *rconn)
{
  بنية ssl_connect_انکڑے * ثابت connssl = cf->ctx;
  ثابت uint8_t *protocol = عدم;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_سیٹ_negotiated(cf, انکڑے, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
ساكن CURLcode cr_connect(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                           bool *پورا)
{
  بنية ssl_connect_انکڑے * ثابت connssl = cf->ctx;
  ثابت بنية rustls_ssl_backend_انکڑے * ثابت backend =
    (بنية rustls_ssl_backend_انکڑے *)connssl->backend;
  ثابت بنية rustls_connection *rconn = عدم;
  CURLcode tmperr = CURLE_OK;
  CURLcode نتیجہ;
  bool چاہندا_read;
  bool چاہندا_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(انکڑے, cf, "cr_connect, state=%d", (صحيح)connssl->state);
  *پورا = FALSE;

#جے_تعریف_شدہ USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    إذا(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(انکڑے, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(انکڑے, cf, "need HTTPS-RR for ECH, delaying connect");
      أرجع CURLE_OK;
    }
#جے_انت /* USE_ECH */

  إذا(!backend->conn) {
    نتیجہ =
      cr_init_backend(cf, انکڑے,
                      (بنية rustls_ssl_backend_انکڑے *)connssl->backend);
    CURL_TRC_CF(انکڑے, cf, "cr_connect, init backend -> %d", (صحيح)نتیجہ);
    إذا(نتیجہ)
      أرجع نتیجہ;
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
      cr_سیٹ_negotiated_alpn(cf, انکڑے, rconn);
      tmperr = cr_send(cf, انکڑے, عدم, 0, &nwritten);
      إذا(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        أرجع CURLE_OK;
      }
      وإلا إذا(tmperr != CURLE_OK) {
        أرجع tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#جے_تعریف_شدہ CURLVERBOSE
        ثابت uint16_t proto = rustls_connection_get_protocol_نسخہ(rconn);
        ثابت rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ثابت rustls_str kex_گروہ_name =
          rustls_connection_get_negotiated_کلید_exchange_گروہ_name(rconn);
        ثابت حرف *ver = "TLS version unknown";
        إذا(proto == RUSTLS_TLS_نسخہ_TLSV1_3)
          ver = "TLSv1.3";
        إذا(proto == RUSTLS_TLS_نسخہ_TLSV1_2)
          ver = "TLSv1.2";
#جے_انت
        infof(انکڑے,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (صحيح)ciphersuite_name.len,
              ciphersuite_name.انکڑے,
              (صحيح)kex_گروہ_name.len,
              kex_گروہ_name.انکڑے);
      }
      إذا(انکڑے->سیٹ.ssl.certinfo) {
        size_t عدد_certs = 0;
        size_t i;
        طالما(rustls_connection_get_peer_certificate(rconn, عدد_certs)) {
          عدد_certs++;
          إذا(عدد_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(انکڑے, "%zu certificates is more than allowed (%d)",
                  عدد_certs, MAX_ALLOWED_CERT_AMOUNT);
            أرجع CURLE_SSL_CONNECT_ERROR;
          }
        }
        نتیجہ = Curl_ssl_init_certinfo(انکڑے, (صحيح)عدد_certs);
        إذا(نتیجہ)
          أرجع نتیجہ;
        لكل(i = 0; i < عدد_certs; i++) {
          ثابت rustls_certificate *cert;
          ثابت بنانشانی حرف *der_انکڑے;
          size_t der_len;
          rustls_نتیجہ rresult = RUSTLS_نتیجہ_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_انکڑے, &der_len);
          إذا(rresult != RUSTLS_نتیجہ_OK) {
            حرف errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, حجم(errorbuf), &errorlen);
            failf(انکڑے,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (صحيح)errorlen, errorbuf);
            أرجع map_error(rresult);
          }
          نتیجہ = Curl_کڈھو_certinfo(انکڑے, (صحيح)i,
                                         der_انکڑے, der_انکڑے + der_len);
          إذا(نتیجہ)
            أرجع نتیجہ;
        }
      }

      connssl->state = ssl_connection_complete;
      *پورا = TRUE;
      أرجع CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    چاہندا_read = rustls_connection_چاہندا_read(rconn);
    چاہندا_write = rustls_connection_چاہندا_write(rconn) ||
                  backend->plain_باہرلا_buffered;
    DEBUGASSERT(چاہندا_read || چاہندا_write);

    إذا(چاہندا_write) {
      size_t nwritten;
      CURL_TRC_CF(انکڑے, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, انکڑے, عدم, 0, &nwritten);
      إذا(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(انکڑے, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        أرجع CURLE_OK;
      }
      وإلا إذا(tmperr != CURLE_OK) {
        أرجع tmperr;
      }
    }

    إذا(چاہندا_read) {
      CURL_TRC_CF(انکڑے, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_ہور(cf, انکڑے, &tmperr);
      إذا(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(انکڑے, "TLS connect error: Connection closed abruptly");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
      إذا(nread < 0) {
        إذا(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(انکڑے, cf, "reading would block");
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

ساكن فراغ *cr_get_internals(بنية ssl_connect_انکڑے *connssl, CURLINFO info)
{
  بنية rustls_ssl_backend_انکڑے *backend =
    (بنية rustls_ssl_backend_انکڑے *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع backend->conn;
}

ساكن CURLcode cr_shutdown(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے,
                            ثابت bool send_shutdown, bool *پورا)
{
  بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية rustls_ssl_backend_انکڑے *backend =
    (بنية rustls_ssl_backend_انکڑے *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  إذا(!backend->conn || cf->shutdown) {
    *پورا = TRUE;
    اذهب باہرلا;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *پورا = FALSE;

  إذا(!backend->گھلیا_shutdown) {
    /* do this only once */
    backend->گھلیا_shutdown = TRUE;
    إذا(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  نتیجہ = cr_send(cf, انکڑے, عدم, 0, &nwritten);
  إذا(نتیجہ) {
    إذا(نتیجہ == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتیجہ = CURLE_OK;
      اذهب باہرلا;
    }
    DEBUGASSERT(نتیجہ);
    CURL_TRC_CF(انکڑے, cf, "shutdown send failed: %d", (صحيح)نتیجہ);
    اذهب باہرلا;
  }

  لكل(i = 0; i < 10; ++i) {
    حرف buf[1024];
    نتیجہ = cr_recv(cf, انکڑے, buf, (صحيح)حجم(buf), &nread);
    إذا(نتیجہ)
      اكسر;
  }

  إذا(نتیجہ == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    نتیجہ = CURLE_OK;
  }
  وإلا إذا(نتیجہ) {
    DEBUGASSERT(نتیجہ);
    CURL_TRC_CF(انکڑے, cf, "shutdown, error: %d", (صحيح)نتیجہ);
  }
  وإلا إذا(nread == 0) {
    /* We got the close notify alert and are done. */
    *پورا = TRUE;
  }

باہرلا:
  cf->shutdown = (نتیجہ || *پورا);
  أرجع نتیجہ;
}

ساكن فراغ cr_close(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  ثابت بنية ssl_connect_انکڑے *connssl = cf->ctx;
  بنية rustls_ssl_backend_انکڑے *backend =
    (بنية rustls_ssl_backend_انکڑے *)connssl->backend;

  (فراغ)انکڑے;
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

ساكن size_t cr_نسخہ(حرف *بفر, size_t ناپ)
{
  ثابت بنية rustls_str ver = rustls_نسخہ();
  أرجع curl_msnprintf(بفر, ناپ, "%.*s", (صحيح)ver.len, ver.انکڑے);
}

ساكن CURLcode cr_random(بنية Curl_easy *انکڑے, بنانشانی حرف *entropy,
                          size_t لمائی)
{
  rustls_نتیجہ rresult = 0;
  (فراغ)انکڑے;
  rresult = rustls_default_crypto_provider_random(entropy, لمائی);
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
  حجم(بنية rustls_ssl_backend_انکڑے),

  عدم,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_نسخہ,                      /*نسخہ*/
  cr_shutdown,                     /* shutdown */
  cr_انکڑے_pending,                 /* data_pending */
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

#جے_انت /* USE_RUSTLS */
