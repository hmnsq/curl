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

#که_تعریف_شوی USE_RUSTLS

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
#که_تعریف_شوی USE_ECH
#ضمن "curlx/base64.h"
#د_که_پای

#إذا EAGAIN != EWOULDBLOCK
#تعریف_کړه RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#وإلا
#تعریف_کړه RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#د_که_پای

بنية rustls_ssl_backend_معلومات {
  ثابت بنية rustls_client_config *config;
  بنية rustls_connection *conn;
  size_t plain_بهر_buffered;
  بټ(معلومات_دننه_pending);
  بټ(لېږل_شوی_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
ساكن CURLcode map_error(ثابت rustls_پایله r)
{
  إذا(rustls_پایله_is_cert_error(r)) {
    أرجع CURLE_PEER_FAILED_VERIFICATION;
  }
  بدل(r) {
  حالة RUSTLS_پایله_OK:
    أرجع CURLE_OK;
  حالة RUSTLS_پایله_NULL_PARAMETER:
    أرجع CURLE_BAD_FUNCTION_دلیل;
  افتراضي:
    أرجع CURLE_RECV_ERROR;
  }
}

ساكن فراغ rustls_failf(بنية Curl_easy *معلومات, ثابت rustls_پایله rr,
                         ثابت حرف *پیغام)
{
  حرف errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, حجم(errorbuf), &errorlen);
  failf(معلومات, "%s: %.*s", پیغام, (صحيح)errorlen, errorbuf);
}

ساكن bool cr_معلومات_pending(بنية Curl_cfilter *cf,
                            ثابت بنية Curl_easy *معلومات)
{
  ثابت بنية ssl_connect_معلومات *ctx = cf->ctx;
  بنية rustls_ssl_backend_معلومات *backend;

  (فراغ)معلومات;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية rustls_ssl_backend_معلومات *)ctx->backend;
  أرجع (bool)backend->معلومات_دننه_pending;
}

بنية io_ctx {
  بنية Curl_cfilter *cf;
  بنية Curl_easy *معلومات;
};

ساكن صحيح read_cb(فراغ *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *بهر_n)
{
  ثابت بنية io_ctx *io_ctx = userdata;
  بنية ssl_connect_معلومات * ثابت connssl = io_ctx->cf->ctx;
  CURLcode پایله;
  صحيح ret = 0;
  size_t nread;

  پایله = Curl_conn_cf_recv(io_ctx->cf->بل, io_ctx->معلومات,
                             (حرف *)buf, len, &nread);
  إذا(پایله) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    إذا(پایله == CURLE_AGAIN)
      ret = EAGAIN;
    وإلا
      ret = EINVAL;
  }
  وإلا إذا(nread == 0)
    connssl->peer_closed = TRUE;
  *بهر_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->معلومات, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (صحيح)پایله, nread);
  أرجع ret;
}

ساكن صحيح write_cb(فراغ *userdata, ثابت uint8_t *buf, uintptr_t len,
                    uintptr_t *بهر_n)
{
  ثابت بنية io_ctx *io_ctx = userdata;
  CURLcode پایله;
  صحيح ret = 0;
  size_t nwritten;

  پایله = Curl_conn_cf_send(io_ctx->cf->بل, io_ctx->معلومات,
                             buf, len, FALSE, &nwritten);
  إذا(پایله) {
    nwritten = 0;
    إذا(پایله == CURLE_AGAIN)
      ret = EAGAIN;
    وإلا
      ret = EINVAL;
  }
  *بهر_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->معلومات, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (صحيح)پایله, nwritten);
  أرجع ret;
}

ساكن ssize_t tls_recv_نور(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات, CURLcode *err)
{
  ثابت بنية ssl_connect_معلومات * ثابت connssl = cf->ctx;
  بنية rustls_ssl_backend_معلومات * ثابت backend =
    (بنية rustls_ssl_backend_معلومات *)connssl->backend;
  بنية io_ctx io_ctx;
  size_t tls_بایټونه_read = 0;
  rustls_io_پایله io_error;
  rustls_پایله rresult = 0;

  io_ctx.cf = cf;
  io_ctx.معلومات = معلومات;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_بایټونه_read);
  إذا(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    أرجع -1;
  }
  وإلا إذا(io_error) {
    حرف بفر[STRERROR_LEN];
    failf(معلومات, "reading from socket: %s",
          curlx_strerror(io_error, بفر, حجم(بفر)));
    *err = CURLE_RECV_ERROR;
    أرجع -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  إذا(rresult != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    أرجع -1;
  }

  backend->معلومات_دننه_pending = TRUE;
  *err = CURLE_OK;
  أرجع (ssize_t)tls_بایټونه_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
ساكن CURLcode cr_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                        حرف *plainbuf, size_t plainlen, size_t *pnread)
{
  ثابت بنية ssl_connect_معلومات * ثابت connssl = cf->ctx;
  بنية rustls_ssl_backend_معلومات * ثابت backend =
    (بنية rustls_ssl_backend_معلومات *)connssl->backend;
  بنية rustls_connection *rconn = عدم;
  CURLcode پایله = CURLE_OK;
  size_t n = 0;
  rustls_پایله rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  طالما(*pnread < plainlen) {
    إذا(!backend->معلومات_دننه_pending) {
      إذا(tls_recv_نور(cf, معلومات, &پایله) < 0) {
        إذا(پایله != CURLE_AGAIN) {
          اذهب بهر;
        }
        پایله = CURLE_OK;
        اكسر;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    إذا(rresult == RUSTLS_پایله_PLAINTEXT_تشه) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->معلومات_دننه_pending = FALSE;
      اكسر;
    }
    وإلا إذا(rresult == RUSTLS_پایله_UNEXPECTED_EOF) {
      failf(معلومات, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      پایله = CURLE_RECV_ERROR;
      اذهب بهر;
    }
    وإلا إذا(rresult != RUSTLS_پایله_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(معلومات, rresult, "rustls_connection_read");
      پایله = CURLE_RECV_ERROR;
      اذهب بهر;
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
    پایله = CURLE_AGAIN;
  }

بهر:
  CURL_TRC_CF(معلومات, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (صحيح)پایله, *pnread);
  أرجع پایله;
}

ساكن CURLcode cr_flush_بهر(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                             بنية rustls_connection *rconn)
{
  بنية io_ctx io_ctx;
  rustls_io_پایله io_error;
  size_t tlswritten = 0;
  size_t tlswritten_ټول = 0;

  io_ctx.cf = cf;
  io_ctx.معلومات = معلومات;

  طالما(rustls_connection_غواړي_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    إذا(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(معلومات, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_ټول);
      أرجع CURLE_AGAIN;
    }
    وإلا إذا(io_error) {
      حرف بفر[STRERROR_LEN];
      failf(معلومات, "writing to socket: %s",
            curlx_strerror(io_error, بفر, حجم(بفر)));
      أرجع CURLE_SEND_ERROR;
    }
    إذا(tlswritten == 0) {
      failf(معلومات, "EOF in swrite");
      أرجع CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(معلومات, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_ټول += tlswritten;
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
ساكن CURLcode cr_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                        ثابت فراغ *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ثابت بنية ssl_connect_معلومات * ثابت connssl = cf->ctx;
  بنية rustls_ssl_backend_معلومات * ثابت backend =
    (بنية rustls_ssl_backend_معلومات *)connssl->backend;
  بنية rustls_connection *rconn = عدم;
  size_t plainwritten = 0;
  ثابت بینښه حرف *buf = plainbuf;
  CURLcode پایله = CURLE_OK;
  size_t blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(معلومات, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  إذا(backend->plain_بهر_buffered) {
    پایله = cr_flush_بهر(cf, معلومات, rconn);
    CURL_TRC_CF(معلومات, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_بهر_buffered, (صحيح)پایله);
    إذا(پایله)
      أرجع پایله;
    إذا(blen > backend->plain_بهر_buffered) {
      blen -= backend->plain_بهر_buffered;
      buf += backend->plain_بهر_buffered;
    }
    وإلا
      blen = 0;
    *pnwritten += backend->plain_بهر_buffered;
    backend->plain_بهر_buffered = 0;
  }

  إذا(blen > 0) {
    rustls_پایله rresult;
    CURL_TRC_CF(معلومات, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    إذا(rresult != RUSTLS_پایله_OK) {
      rustls_failf(معلومات, rresult, "rustls_connection_write");
      پایله = CURLE_WRITE_ERROR;
      اذهب بهر;
    }
    وإلا إذا(plainwritten == 0) {
      failf(معلومات, "rustls_connection_write: EOF");
      پایله = CURLE_WRITE_ERROR;
      اذهب بهر;
    }
  }

  پایله = cr_flush_بهر(cf, معلومات, rconn);
  إذا(پایله) {
    إذا(پایله == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_بهر_buffered = plainwritten;
      إذا(*pnwritten) {
        پایله = CURLE_OK;
      }
    }
    اذهب بهر;
  }
  وإلا
    *pnwritten += plainwritten;

بهر:
  CURL_TRC_CF(معلومات, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (صحيح)پایله, *pnwritten);
  أرجع پایله;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
ساكن uint32_t cr_verify_none(فراغ *userdata,
                               ثابت rustls_verify_server_cert_params *params)
{
  (فراغ)userdata;
  (فراغ)params;
  أرجع RUSTLS_پایله_OK;
}

ساكن صحيح read_file_into(ثابت حرف *filename, بنية dynbuf *بهر)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  إذا(!f) {
    أرجع 0;
  }

  لكل(;;) {
    uint8_t buf[256];
    ثابت size_t rr = fread(buf, 1, حجم(buf), f);
    إذا((!rr && !feof(f)) ||
       curlx_dyn_addn(بهر, buf, rr)) {
      curlx_fclose(f);
      أرجع 0;
    }
    إذا(rr < حجم(buf))
      اكسر;
  }

  أرجع curlx_fclose(f) == 0;
}

ساكن فراغ
cr_get_selected_ciphers(بنية Curl_easy *معلومات,
                        ثابت حرف *ciphers12,
                        ثابت حرف *ciphers13,
                        ثابت بنية rustls_supported_ciphersuite **selected,
                        size_t *selected_اندازه)
{
  ثابت size_t supported_len = *selected_اندازه;
  ثابت size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ثابت بنية rustls_supported_ciphersuite *ثبت = عدم;
  ثابت حرف *ciphers = ciphers12;
  size_t شمېر = 0, default13_شمېر = 0, i, j;
  ثابت حرف *ptr, *پای_ټکی;

  DEBUGASSERT(default_len <= supported_len);

  إذا(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    لكل(j = 0; j < default_len; j++) {
      ثبت = rustls_default_crypto_provider_ciphersuites_get(j);
      إذا(rustls_supported_ciphersuite_protocol_نسخه(ثبت) !=
         RUSTLS_TLS_نسخه_TLSV1_3)
        استمر;

      selected[شمېر++] = ثبت;
    }

    default13_شمېر = شمېر;

    إذا(!ciphers)
      ciphers = "";
  }
  وإلا
    ciphers = ciphers13;

جمع_کړه_ciphers:
  لكل(ptr = ciphers; ptr[0] != '\0' && شمېر < supported_len; ptr = پای_ټکی) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &پای_ټکی);

    /* Check if cipher is supported */
    إذا(id) {
      لكل(i = 0; i < supported_len; i++) {
        ثبت = rustls_default_crypto_provider_ciphersuites_get(i);
        إذا(rustls_supported_ciphersuite_get_suite(ثبت) == id)
          اكسر;
      }
      إذا(i == supported_len)
        id = 0;
    }
    إذا(!id) {
      إذا(ptr[0] != '\0')
        infof(معلومات, "rustls: unknown cipher in list: \"%.*s\"",
              (صحيح)(پای_ټکی - ptr), ptr);
      استمر;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    لكل(i = 0; i < شمېر && selected[i] != ثبت; i++)
      ;
    إذا(i < شمېر) {
      إذا(i >= default13_شمېر)
        infof(معلومات, "rustls: duplicate cipher in list: \"%.*s\"",
              (صحيح)(پای_ټکی - ptr), ptr);
      استمر;
    }

    selected[شمېر++] = ثبت;
  }

  إذا(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    اذهب جمع_کړه_ciphers;
  }

  إذا(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    لكل(j = 0; j < default_len; j++) {
      ثبت = rustls_default_crypto_provider_ciphersuites_get(j);
      إذا(rustls_supported_ciphersuite_protocol_نسخه(ثبت) ==
         RUSTLS_TLS_نسخه_TLSV1_3)
        استمر;

      /* No duplicates allowed (so selected cannot overflow) */
      لكل(i = 0; i < شمېر && selected[i] != ثبت; i++)
        ;
      إذا(i < شمېر)
        استمر;

      selected[شمېر++] = ثبت;
    }
  }

  *selected_اندازه = شمېر;
}

ساكن فراغ cr_keylog_log_cb(بنية rustls_str label,
                             ثابت uint8_t *client_random,
                             size_t client_random_len, ثابت uint8_t *secret,
                             size_t secret_len)
{
  حرف clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_اندازه);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, حجم(clabel), "%.*s", (صحيح)label.len, label.معلومات);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

ساكن CURLcode
init_config_builder(بنية Curl_cfilter *cf,
                    بنية Curl_easy *معلومات,
                    ثابت بنية ssl_filter_config *conn_config,
                    بنية rustls_client_config_builder **config_builder)
{
  ثابت بنية rustls_supported_ciphersuite **cipher_suites = عدم;
  بنية rustls_crypto_provider_builder *custom_provider_builder = عدم;
  ثابت بنية rustls_crypto_provider *custom_provider = عدم;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_نسخه_TLSV1_2,
    RUSTLS_TLS_نسخه_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode پایله = CURLE_OK;
  rustls_پایله rr;

  DEBUGASSERT(conn_config->نسخه_2 != CURL_SSLVERSION_DEFAULT);
  بدل(conn_config->نسخه_2) {
  حالة CURL_SSLVERSION_TLSv1:
  حالة CURL_SSLVERSION_TLSv1_0:
  حالة CURL_SSLVERSION_TLSv1_1:
  حالة CURL_SSLVERSION_TLSv1_2:
    اكسر;
  حالة CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_نسخه_TLSV1_3;
    tls_versions_len = 1;
    اكسر;
  افتراضي:
    failf(معلومات, "rustls: unsupported minimum TLS version value");
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب cleanup;
  }

  بدل(conn_config->نسخه_max) {
  حالة CURL_SSLVERSION_MAX_DEFAULT:
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_TLSv1_3:
    اكسر;
  حالة CURL_SSLVERSION_MAX_TLSv1_2:
    إذا(tls_versions[0] == RUSTLS_TLS_نسخه_TLSV1_2) {
      tls_versions_len = 1;
      اكسر;
    }
    FALLTHROUGH();
  حالة CURL_SSLVERSION_MAX_TLSv1_1:
  حالة CURL_SSLVERSION_MAX_TLSv1_0:
  افتراضي:
    failf(معلومات, "rustls: unsupported maximum TLS version value");
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب cleanup;
  }

#که_تعریف_شوی USE_ECH
  إذا(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_نسخه_TLSV1_3;
    tls_versions_len = 1;
    infof(معلومات, "rustls: ECH enabled, forcing TLSv1.3");
  }
#وإلا
  (فراغ)cf;
#د_که_پای /* USE_ECH */

  cipher_suites = curlx_malloc(حجم(*cipher_suites) * cipher_suites_len);
  إذا(!cipher_suites) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب cleanup;
  }

  cr_get_selected_ciphers(معلومات,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  إذا(cipher_suites_len == 0) {
    failf(معلومات, "rustls: no supported cipher in list");
    پایله = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr,
                 "failed to create crypto provider builder from default");
    پایله = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr =
    rustls_crypto_provider_builder_سېټ_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr,
                 "failed to set ciphersuites for crypto provider builder");
    پایله = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "failed to build custom crypto provider");
    پایله = CURLE_SSL_CIPHER;
    اذهب cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "failed to create client config builder");
    پایله = CURLE_SSL_CIPHER;
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
  أرجع پایله;
}

ساكن فراغ
init_config_builder_alpn(بنية Curl_easy *معلومات,
                         ثابت بنية ssl_connect_معلومات *connssl,
                         بنية rustls_client_config_builder *config_builder)
{
  بنية alpn_proto_buf proto;
  rustls_ټوټه_بایټونه alpn[ALPN_ENTRIES_MAX];
  size_t i;

  لكل(i = 0; i < connssl->alpn->شمېر; ++i) {
    alpn[i].معلومات = (ثابت uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = مزي_اوږدوالی(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_سېټ_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->شمېر);
  Curl_alpn_منزل_proto_str(&proto, connssl->alpn);
  infof(معلومات, VTLS_INFOF_ALPN_OFFER_1STR, proto.معلومات);
}

ساكن CURLcode init_config_builder_verifier_crl(
  بنية Curl_easy *معلومات,
  ثابت بنية ssl_filter_config *conn_config,
  بنية rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode پایله = CURLE_OK;
  بنية dynbuf crl_contents;
  rustls_پایله rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_اندازه);
  إذا(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(معلومات, "rustls: failed to read revocation list file");
    پایله = CURLE_SSL_CRL_BADFILE;
    اذهب cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_جمع_کړه_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "failed to parse revocation list");
    پایله = CURLE_SSL_CRL_BADFILE;
    اذهب cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  أرجع پایله;
}

ساكن CURLcode
init_config_builder_verifier(بنية Curl_easy *معلومات,
                             بنية rustls_client_config_builder *builder,
                             ثابت بنية ssl_filter_config *conn_config,
                             ثابت بنية curl_blob *ca_info_blob,
                             ثابت حرف * ثابت ssl_cafile)
{
  ثابت بنية rustls_ریښه_cert_store *roots = عدم;
  بنية rustls_ریښه_cert_store_builder *roots_builder = عدم;
  بنية rustls_web_pki_server_cert_verifier_builder *verifier_builder = عدم;
  بنية rustls_server_cert_verifier *server_cert_verifier = عدم;
  rustls_پایله rr = RUSTLS_پایله_OK;
  CURLcode پایله = CURLE_OK;

  roots_builder = rustls_ریښه_cert_store_builder_new();
  إذا(ca_info_blob) {
    rr = rustls_ریښه_cert_store_builder_جمع_کړه_pem(roots_builder,
                                                ca_info_blob->معلومات,
                                                ca_info_blob->len,
                                                1);
    إذا(rr != RUSTLS_پایله_OK) {
      rustls_failf(معلومات, rr, "failed to parse trusted certificates from blob");
      پایله = CURLE_SSL_CACERT_BADFILE;
      اذهب cleanup;
    }
  }
  وإلا إذا(ssl_cafile) {
    rr = rustls_ریښه_cert_store_builder_بارول_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    إذا(rr != RUSTLS_پایله_OK) {
      rustls_failf(معلومات, rr, "failed to load trusted certificates");
      پایله = CURLE_SSL_CACERT_BADFILE;
      اذهب cleanup;
    }
  }

  rr = rustls_ریښه_cert_store_builder_build(roots_builder, &roots);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "failed to build trusted root certificate store");
    پایله = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  إذا(!verifier_builder) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب cleanup;
  }

  إذا(conn_config->CRLfile) {
    پایله = init_config_builder_verifier_crl(معلومات,
                                              conn_config,
                                              verifier_builder);
    إذا(پایله) {
      اذهب cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "failed to build certificate verifier");
    پایله = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  rustls_client_config_builder_سېټ_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  إذا(roots_builder) {
    rustls_ریښه_cert_store_builder_free(roots_builder);
  }
  إذا(roots) {
    rustls_ریښه_cert_store_free(roots);
  }
  إذا(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  إذا(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  أرجع پایله;
}

ساكن CURLcode init_config_builder_platform_verifier(
  بنية Curl_easy *معلومات,
  بنية rustls_client_config_builder *builder)
{
  بنية rustls_server_cert_verifier *server_cert_verifier = عدم;
  CURLcode پایله = CURLE_OK;
  rustls_پایله rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "failed to create platform certificate verifier");
    پایله = CURLE_SSL_CACERT_BADFILE;
    اذهب cleanup;
  }

  rustls_client_config_builder_سېټ_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  إذا(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  أرجع پایله;
}

ساكن CURLcode
init_config_builder_keylog(بنية Curl_easy *معلومات,
                           بنية rustls_client_config_builder *builder)
{
  rustls_پایله rr;

  Curl_tls_keylog_open();
  إذا(!Curl_tls_keylog_enabled()) {
    أرجع CURLE_OK;
  }

  rr = rustls_client_config_builder_سېټ_کلی_log(builder,
                                                cr_keylog_log_cb,
                                                عدم);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    أرجع map_error(rr);
  }

  أرجع CURLE_OK;
}

ساكن CURLcode
init_config_builder_client_auth(بنية Curl_easy *معلومات,
                                ثابت بنية ssl_filter_config *conn_config,
                                بنية rustls_client_config_builder *builder)
{
  بنية dynbuf cert_contents;
  بنية dynbuf کلی_contents;
  rustls_پایله rr;
  ثابت بنية rustls_certified_کلی *certified_کلی = عدم;
  CURLcode پایله = CURLE_OK;
  ثابت bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ثابت bool have_کلی = conn_config->کلی || conn_config->کلی_blob;

  إذا(have_cert && !have_کلی) {
    failf(معلومات, "rustls: must provide key with certificate");
    أرجع CURLE_SSL_CERTPROBLEM;
  }
  وإلا إذا(!have_cert && have_کلی) {
    failf(معلومات, "rustls: must provide certificate with key");
    أرجع CURLE_SSL_CERTPROBLEM;
  }

  إذا(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(معلومات, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    أرجع CURLE_SSL_CERTPROBLEM;
  }
  إذا(conn_config->کلی_type && !curl_strequal(conn_config->کلی_type, "PEM")) {
    failf(معلومات, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->کلی_type);
    أرجع CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_اندازه);
  curlx_dyn_init(&کلی_contents, DYN_KEYFILE_اندازه);

  إذا(conn_config->cert_blob) {
    پایله = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->معلومات,
                            conn_config->cert_blob->len);
    إذا(پایله) {
      failf(معلومات, "rustls: failed to copy client certificate blob (%s)",
            پایله == CURLE_TOO_LARGE ? "too large" : "out of memory");
      اذهب cleanup;
    }
  }
  وإلا إذا(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(معلومات, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    پایله = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  إذا(conn_config->کلی_blob) {
    پایله = curlx_dyn_addn(&کلی_contents,
                            conn_config->کلی_blob->معلومات,
                            conn_config->کلی_blob->len);
    إذا(پایله) {
      failf(معلومات, "rustls: failed to copy key blob (%s)",
            پایله == CURLE_TOO_LARGE ? "too large" : "out of memory");
      اذهب cleanup;
    }
  }
  وإلا إذا(!read_file_into(conn_config->کلی, &کلی_contents)) {
    failf(معلومات, "rustls: failed to read key file: '%s'",
          conn_config->کلی);
    پایله = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  rr = rustls_certified_کلی_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&کلی_contents),
                                  curlx_dyn_len(&کلی_contents),
                                  &certified_کلی);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "rustls: failed to build certified key");
    پایله = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  rr = rustls_certified_کلی_کلیګانې_سمون(certified_کلی);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr,
                 "rustls: client certificate and keypair files do not match:");
    پایله = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

  rr = rustls_client_config_builder_سېټ_certified_کلی(builder,
                                                      &certified_کلی,
                                                      1);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "rustls: failed to set certified key");
    پایله = CURLE_SSL_CERTPROBLEM;
    اذهب cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&کلی_contents);
  إذا(certified_کلی) {
    rustls_certified_کلی_free(certified_کلی);
  }
  أرجع پایله;
}

#که_تعریف_شوی USE_ECH

ساكن CURLcode
init_config_builder_ech(بنية Curl_easy *معلومات,
                        بنية Curl_cfilter *cf,
                        بنية rustls_client_config_builder *builder)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت rustls_hpke *hpke = rustls_supported_hpke();
  بینښه حرف *ech_config = عدم;
  size_t ech_config_len = 0;
  بنية Curl_dns_ثبت *dns = عدم;
  CURLcode پایله = CURLE_OK;
  rustls_پایله rr;

  إذا(!hpke) {
    failf(معلومات,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    پایله = CURLE_SSL_CONNECT_ERROR;
    اذهب cleanup;
  }

  إذا(conn_config->ech_عامه) {
    failf(معلومات, "rustls: ECH outername not supported");
    پایله = CURLE_SSL_CONNECT_ERROR;
    اذهب cleanup;
  }

  إذا(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    إذا(rr != RUSTLS_پایله_OK) {
      rustls_failf(معلومات, rr, "rustls: failed to configure ECH GREASE");
      پایله = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
    أرجع CURLE_OK;
  }

  إذا(conn_config->ech && conn_config->ech_config) {
    ثابت حرف *b64 = conn_config->ech_config;
    size_t decode_پایله;
    إذا(!b64[0]) {
      infof(معلومات, "rustls: ECHConfig from command line empty");
      پایله = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_پایله = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    إذا(decode_پایله || !ech_config) {
      infof(معلومات, "rustls: cannot base64 decode ECHConfig from command line");
      پایله = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
  }
  وإلا {
    ثابت بنية ssl_connect_معلومات *connssl = cf->ctx;
    ثابت بنية Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(معلومات, cf->sockindex, connssl->peer.origin);

    إذا(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(معلومات, "rustls: ECH requested but no ECHConfig available");
      پایله = CURLE_SSL_CONNECT_ERROR;
      اذهب cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "rustls: failed to configure ECH");
    پایله = CURLE_SSL_CONNECT_ERROR;
    اذهب cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  إذا(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  إذا(dns) {
    Curl_dns_ثبت_unlink(معلومات, &dns);
  }
  أرجع پایله;
}
#د_که_پای /* USE_ECH */

ساكن CURLcode cr_init_backend(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                بنية rustls_ssl_backend_معلومات * ثابت backend)
{
  ثابت بنية ssl_connect_معلومات *connssl = cf->ctx;
  ثابت بنية ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  بنية rustls_connection *rconn = عدم;
  بنية rustls_client_config_builder *config_builder = عدم;

  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? عدم : conn_config->CAfile);
  CURLcode پایله = CURLE_OK;
  rustls_پایله rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  پایله = init_config_builder(cf, معلومات, conn_config, &config_builder);
  إذا(پایله != CURLE_OK) {
    أرجع پایله;
  }

  إذا(connssl->alpn) {
    init_config_builder_alpn(معلومات, connssl, config_builder);
  }

  إذا(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_سېټ_certificate_verifier(
      config_builder, cr_verify_none);
  }
  وإلا إذا(conn_config->native_ca_store) {
    إذا(conn_config->CRLfile) {
      failf(معلومات, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      أرجع CURLE_NOT_BUILT_دننه;
    }
    پایله = init_config_builder_platform_verifier(معلومات, config_builder);
    إذا(پایله != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      أرجع پایله;
    }
  }
  وإلا إذا(ca_info_blob || ssl_cafile) {
    پایله = init_config_builder_verifier(معلومات,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    إذا(پایله != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      أرجع پایله;
    }
  }

  إذا(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->کلی || conn_config->کلی_blob) {
    پایله = init_config_builder_client_auth(معلومات,
                                             conn_config,
                                             config_builder);
    إذا(پایله != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      أرجع پایله;
    }
  }

#که_تعریف_شوی USE_ECH
  إذا(Curl_ssl_ech_enabled(cf)) {
    پایله = init_config_builder_ech(معلومات, cf, config_builder);
    إذا((پایله != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      أرجع پایله;
    }
  }
#د_که_پای /* USE_ECH */

  پایله = init_config_builder_keylog(معلومات, config_builder);
  إذا(پایله != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    أرجع پایله;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "failed to build client config");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  إذا(rr != RUSTLS_پایله_OK) {
    rustls_failf(معلومات, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = عدم;
    أرجع CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_سېټ_userdata(rconn, backend);
  backend->conn = rconn;

  أرجع پایله;
}

ساكن فراغ cr_سېټ_negotiated_alpn(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات,
                                   ثابت بنية rustls_connection *rconn)
{
  بنية ssl_connect_معلومات * ثابت connssl = cf->ctx;
  ثابت uint8_t *protocol = عدم;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_سېټ_negotiated(cf, معلومات, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
ساكن CURLcode cr_connect(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                           bool *بشپړ)
{
  بنية ssl_connect_معلومات * ثابت connssl = cf->ctx;
  ثابت بنية rustls_ssl_backend_معلومات * ثابت backend =
    (بنية rustls_ssl_backend_معلومات *)connssl->backend;
  ثابت بنية rustls_connection *rconn = عدم;
  CURLcode tmperr = CURLE_OK;
  CURLcode پایله;
  bool غواړي_read;
  bool غواړي_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(معلومات, cf, "cr_connect, state=%d", (صحيح)connssl->state);
  *بشپړ = FALSE;

#که_تعریف_شوی USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    إذا(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(معلومات, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(معلومات, cf, "need HTTPS-RR for ECH, delaying connect");
      أرجع CURLE_OK;
    }
#د_که_پای /* USE_ECH */

  إذا(!backend->conn) {
    پایله =
      cr_init_backend(cf, معلومات,
                      (بنية rustls_ssl_backend_معلومات *)connssl->backend);
    CURL_TRC_CF(معلومات, cf, "cr_connect, init backend -> %d", (صحيح)پایله);
    إذا(پایله)
      أرجع پایله;
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
      cr_سېټ_negotiated_alpn(cf, معلومات, rconn);
      tmperr = cr_send(cf, معلومات, عدم, 0, &nwritten);
      إذا(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        أرجع CURLE_OK;
      }
      وإلا إذا(tmperr != CURLE_OK) {
        أرجع tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#که_تعریف_شوی CURLVERBOSE
        ثابت uint16_t proto = rustls_connection_get_protocol_نسخه(rconn);
        ثابت rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ثابت rustls_str kex_ډله_name =
          rustls_connection_get_negotiated_کلی_exchange_ډله_name(rconn);
        ثابت حرف *ver = "TLS version unknown";
        إذا(proto == RUSTLS_TLS_نسخه_TLSV1_3)
          ver = "TLSv1.3";
        إذا(proto == RUSTLS_TLS_نسخه_TLSV1_2)
          ver = "TLSv1.2";
#د_که_پای
        infof(معلومات,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (صحيح)ciphersuite_name.len,
              ciphersuite_name.معلومات,
              (صحيح)kex_ډله_name.len,
              kex_ډله_name.معلومات);
      }
      إذا(معلومات->سېټ.ssl.certinfo) {
        size_t عدد_certs = 0;
        size_t i;
        طالما(rustls_connection_get_peer_certificate(rconn, عدد_certs)) {
          عدد_certs++;
          إذا(عدد_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(معلومات, "%zu certificates is more than allowed (%d)",
                  عدد_certs, MAX_ALLOWED_CERT_AMOUNT);
            أرجع CURLE_SSL_CONNECT_ERROR;
          }
        }
        پایله = Curl_ssl_init_certinfo(معلومات, (صحيح)عدد_certs);
        إذا(پایله)
          أرجع پایله;
        لكل(i = 0; i < عدد_certs; i++) {
          ثابت rustls_certificate *cert;
          ثابت بینښه حرف *der_معلومات;
          size_t der_len;
          rustls_پایله rresult = RUSTLS_پایله_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_معلومات, &der_len);
          إذا(rresult != RUSTLS_پایله_OK) {
            حرف errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, حجم(errorbuf), &errorlen);
            failf(معلومات,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (صحيح)errorlen, errorbuf);
            أرجع map_error(rresult);
          }
          پایله = Curl_ایستل_certinfo(معلومات, (صحيح)i,
                                         der_معلومات, der_معلومات + der_len);
          إذا(پایله)
            أرجع پایله;
        }
      }

      connssl->state = ssl_connection_complete;
      *بشپړ = TRUE;
      أرجع CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    غواړي_read = rustls_connection_غواړي_read(rconn);
    غواړي_write = rustls_connection_غواړي_write(rconn) ||
                  backend->plain_بهر_buffered;
    DEBUGASSERT(غواړي_read || غواړي_write);

    إذا(غواړي_write) {
      size_t nwritten;
      CURL_TRC_CF(معلومات, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, معلومات, عدم, 0, &nwritten);
      إذا(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(معلومات, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        أرجع CURLE_OK;
      }
      وإلا إذا(tmperr != CURLE_OK) {
        أرجع tmperr;
      }
    }

    إذا(غواړي_read) {
      CURL_TRC_CF(معلومات, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_نور(cf, معلومات, &tmperr);
      إذا(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(معلومات, "TLS connect error: Connection closed abruptly");
        أرجع CURLE_SSL_CONNECT_ERROR;
      }
      إذا(nread < 0) {
        إذا(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(معلومات, cf, "reading would block");
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

ساكن فراغ *cr_get_internals(بنية ssl_connect_معلومات *connssl, CURLINFO info)
{
  بنية rustls_ssl_backend_معلومات *backend =
    (بنية rustls_ssl_backend_معلومات *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع backend->conn;
}

ساكن CURLcode cr_shutdown(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                            ثابت bool send_shutdown, bool *بشپړ)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية rustls_ssl_backend_معلومات *backend =
    (بنية rustls_ssl_backend_معلومات *)connssl->backend;
  CURLcode پایله = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  إذا(!backend->conn || cf->shutdown) {
    *بشپړ = TRUE;
    اذهب بهر;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *بشپړ = FALSE;

  إذا(!backend->لېږل_شوی_shutdown) {
    /* do this only once */
    backend->لېږل_شوی_shutdown = TRUE;
    إذا(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  پایله = cr_send(cf, معلومات, عدم, 0, &nwritten);
  إذا(پایله) {
    إذا(پایله == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      پایله = CURLE_OK;
      اذهب بهر;
    }
    DEBUGASSERT(پایله);
    CURL_TRC_CF(معلومات, cf, "shutdown send failed: %d", (صحيح)پایله);
    اذهب بهر;
  }

  لكل(i = 0; i < 10; ++i) {
    حرف buf[1024];
    پایله = cr_recv(cf, معلومات, buf, (صحيح)حجم(buf), &nread);
    إذا(پایله)
      اكسر;
  }

  إذا(پایله == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    پایله = CURLE_OK;
  }
  وإلا إذا(پایله) {
    DEBUGASSERT(پایله);
    CURL_TRC_CF(معلومات, cf, "shutdown, error: %d", (صحيح)پایله);
  }
  وإلا إذا(nread == 0) {
    /* We got the close notify alert and are done. */
    *بشپړ = TRUE;
  }

بهر:
  cf->shutdown = (پایله || *بشپړ);
  أرجع پایله;
}

ساكن فراغ cr_close(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  ثابت بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية rustls_ssl_backend_معلومات *backend =
    (بنية rustls_ssl_backend_معلومات *)connssl->backend;

  (فراغ)معلومات;
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

ساكن size_t cr_نسخه(حرف *بفر, size_t اندازه)
{
  ثابت بنية rustls_str ver = rustls_نسخه();
  أرجع curl_msnprintf(بفر, اندازه, "%.*s", (صحيح)ver.len, ver.معلومات);
}

ساكن CURLcode cr_random(بنية Curl_easy *معلومات, بینښه حرف *entropy,
                          size_t اوږدتیا)
{
  rustls_پایله rresult = 0;
  (فراغ)معلومات;
  rresult = rustls_default_crypto_provider_random(entropy, اوږدتیا);
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
  حجم(بنية rustls_ssl_backend_معلومات),

  عدم,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_نسخه,                      /*نسخه*/
  cr_shutdown,                     /* shutdown */
  cr_معلومات_pending,                 /* data_pending */
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

#د_که_پای /* USE_RUSTLS */
