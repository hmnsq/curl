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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RUSTLS

#ਅਨ੍ਤਰ੍ਭਾਵ <rustls.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strerr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/httpsrr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/rustls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/keylog.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/cipher_suite.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/x509asn1.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਜੇ_ਅੰਤ

#ਜੇ EAGAIN != EWOULDBLOCK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#ਜੇ_ਅੰਤ

ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ {
  ਅਚਲ ਬਣਤਰ rustls_client_config *config;
  ਬਣਤਰ rustls_connection *conn;
  size_t plain_ਬਾਹਰਲਾ_buffered;
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਅੰਕੜੇ_ਵਿੱਚ_pending);
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਭੇਜਿਆ_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
ਸਥਿਰ CURLcode map_error(ਅਚਲ rustls_ਨਤੀਜਾ r)
{
  ਜੇ(rustls_ਨਤੀਜਾ_is_cert_error(r)) {
    ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
  }
  ਚੋਣ(r) {
  ਹਾਲਤ RUSTLS_ਨਤੀਜਾ_OK:
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ RUSTLS_ਨਤੀਜਾ_NULL_PARAMETER:
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਮੂਲ:
    ਵਾਪਸ CURLE_RECV_ERROR;
  }
}

ਸਥਿਰ ਖਾਲੀ rustls_failf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ rustls_ਨਤੀਜਾ rr,
                         ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ)
{
  ਅੱਖਰ errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, ਆਕਾਰ(errorbuf), &errorlen);
  failf(ਅੰਕੜੇ, "%s: %.*s", ਸੁਨੇਹਾ, (ਪੂਰਨ)errorlen, errorbuf);
}

ਸਥਿਰ bool cr_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                            ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *ctx = cf->ctx;
  ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *backend;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *)ctx->backend;
  ਵਾਪਸ (bool)backend->ਅੰਕੜੇ_ਵਿੱਚ_pending;
}

ਬਣਤਰ io_ctx {
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ;
};

ਸਥਿਰ ਪੂਰਨ read_cb(ਖਾਲੀ *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *ਬਾਹਰਲਾ_n)
{
  ਅਚਲ ਬਣਤਰ io_ctx *io_ctx = userdata;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ * ਅਚਲ connssl = io_ctx->cf->ctx;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ ret = 0;
  size_t nread;

  ਨਤੀਜਾ = Curl_conn_cf_recv(io_ctx->cf->ਅਗਲਾ, io_ctx->ਅੰਕੜੇ,
                             (ਅੱਖਰ *)buf, len, &nread);
  ਜੇ(ਨਤੀਜਾ) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ret = EAGAIN;
    ਨਹੀਂਤਾਂ
      ret = EINVAL;
  }
  ਨਹੀਂਤਾਂ ਜੇ(nread == 0)
    connssl->peer_closed = TRUE;
  *ਬਾਹਰਲਾ_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->ਅੰਕੜੇ, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (ਪੂਰਨ)ਨਤੀਜਾ, nread);
  ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ write_cb(ਖਾਲੀ *userdata, ਅਚਲ uint8_t *buf, uintptr_t len,
                    uintptr_t *ਬਾਹਰਲਾ_n)
{
  ਅਚਲ ਬਣਤਰ io_ctx *io_ctx = userdata;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ ret = 0;
  size_t nwritten;

  ਨਤੀਜਾ = Curl_conn_cf_send(io_ctx->cf->ਅਗਲਾ, io_ctx->ਅੰਕੜੇ,
                             buf, len, FALSE, &nwritten);
  ਜੇ(ਨਤੀਜਾ) {
    nwritten = 0;
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
      ret = EAGAIN;
    ਨਹੀਂਤਾਂ
      ret = EINVAL;
  }
  *ਬਾਹਰਲਾ_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->ਅੰਕੜੇ, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (ਪੂਰਨ)ਨਤੀਜਾ, nwritten);
  ਵਾਪਸ ret;
}

ਸਥਿਰ ssize_t tls_recv_ਹੋਰ(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, CURLcode *err)
{
  ਅਚਲ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ * ਅਚਲ connssl = cf->ctx;
  ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ * ਅਚਲ backend =
    (ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ io_ctx io_ctx;
  size_t tls_ਬਾਈਟਾਂ_read = 0;
  rustls_io_ਨਤੀਜਾ io_error;
  rustls_ਨਤੀਜਾ rresult = 0;

  io_ctx.cf = cf;
  io_ctx.ਅੰਕੜੇ = ਅੰਕੜੇ;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_ਬਾਈਟਾਂ_read);
  ਜੇ(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    ਵਾਪਸ -1;
  }
  ਨਹੀਂਤਾਂ ਜੇ(io_error) {
    ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
    failf(ਅੰਕੜੇ, "reading from socket: %s",
          curlx_strerror(io_error, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
    *err = CURLE_RECV_ERROR;
    ਵਾਪਸ -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  ਜੇ(rresult != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    ਵਾਪਸ -1;
  }

  backend->ਅੰਕੜੇ_ਵਿੱਚ_pending = TRUE;
  *err = CURLE_OK;
  ਵਾਪਸ (ssize_t)tls_ਬਾਈਟਾਂ_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
ਸਥਿਰ CURLcode cr_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅੱਖਰ *plainbuf, size_t plainlen, size_t *pnread)
{
  ਅਚਲ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ * ਅਚਲ connssl = cf->ctx;
  ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ * ਅਚਲ backend =
    (ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ rustls_connection *rconn = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t n = 0;
  rustls_ਨਤੀਜਾ rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  ਜਦੋਂ(*pnread < plainlen) {
    ਜੇ(!backend->ਅੰਕੜੇ_ਵਿੱਚ_pending) {
      ਜੇ(tls_recv_ਹੋਰ(cf, ਅੰਕੜੇ, &ਨਤੀਜਾ) < 0) {
        ਜੇ(ਨਤੀਜਾ != CURLE_AGAIN) {
          ਜਾਓ ਬਾਹਰਲਾ;
        }
        ਨਤੀਜਾ = CURLE_OK;
        ਰੋਕੋ;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    ਜੇ(rresult == RUSTLS_ਨਤੀਜਾ_PLAINTEXT_ਖਾਲੀ_ਥਾਵਾਂ) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->ਅੰਕੜੇ_ਵਿੱਚ_pending = FALSE;
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(rresult == RUSTLS_ਨਤੀਜਾ_UNEXPECTED_EOF) {
      failf(ਅੰਕੜੇ, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(rresult != RUSTLS_ਨਤੀਜਾ_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(ਅੰਕੜੇ, rresult, "rustls_connection_read");
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ {
      *pnread += n;
    }
  }

  ਜੇ(!eof && !*pnread) {
    ਨਤੀਜਾ = CURLE_AGAIN;
  }

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnread);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cr_flush_ਬਾਹਰਲਾ(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ rustls_connection *rconn)
{
  ਬਣਤਰ io_ctx io_ctx;
  rustls_io_ਨਤੀਜਾ io_error;
  size_t tlswritten = 0;
  size_t tlswritten_ਕੁੱਲ = 0;

  io_ctx.cf = cf;
  io_ctx.ਅੰਕੜੇ = ਅੰਕੜੇ;

  ਜਦੋਂ(rustls_connection_ਚਾਹੁੰਦਾ_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    ਜੇ(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_ਕੁੱਲ);
      ਵਾਪਸ CURLE_AGAIN;
    }
    ਨਹੀਂਤਾਂ ਜੇ(io_error) {
      ਅੱਖਰ ਬਫਰ[STRERROR_LEN];
      failf(ਅੰਕੜੇ, "writing to socket: %s",
            curlx_strerror(io_error, ਬਫਰ, ਆਕਾਰ(ਬਫਰ)));
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
    ਜੇ(tlswritten == 0) {
      failf(ਅੰਕੜੇ, "EOF in swrite");
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_ਕੁੱਲ += tlswritten;
  }
  ਵਾਪਸ CURLE_OK;
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
ਸਥਿਰ CURLcode cr_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਖਾਲੀ *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ਅਚਲ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ * ਅਚਲ connssl = cf->ctx;
  ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ * ਅਚਲ backend =
    (ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ rustls_connection *rconn = ਨਿਰਾ;
  size_t plainwritten = 0;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf = plainbuf;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  ਜੇ(backend->plain_ਬਾਹਰਲਾ_buffered) {
    ਨਤੀਜਾ = cr_flush_ਬਾਹਰਲਾ(cf, ਅੰਕੜੇ, rconn);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_ਬਾਹਰਲਾ_buffered, (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    ਜੇ(blen > backend->plain_ਬਾਹਰਲਾ_buffered) {
      blen -= backend->plain_ਬਾਹਰਲਾ_buffered;
      buf += backend->plain_ਬਾਹਰਲਾ_buffered;
    }
    ਨਹੀਂਤਾਂ
      blen = 0;
    *pnwritten += backend->plain_ਬਾਹਰਲਾ_buffered;
    backend->plain_ਬਾਹਰਲਾ_buffered = 0;
  }

  ਜੇ(blen > 0) {
    rustls_ਨਤੀਜਾ rresult;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    ਜੇ(rresult != RUSTLS_ਨਤੀਜਾ_OK) {
      rustls_failf(ਅੰਕੜੇ, rresult, "rustls_connection_write");
      ਨਤੀਜਾ = CURLE_WRITE_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(plainwritten == 0) {
      failf(ਅੰਕੜੇ, "rustls_connection_write: EOF");
      ਨਤੀਜਾ = CURLE_WRITE_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }

  ਨਤੀਜਾ = cr_flush_ਬਾਹਰਲਾ(cf, ਅੰਕੜੇ, rconn);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_ਬਾਹਰਲਾ_buffered = plainwritten;
      ਜੇ(*pnwritten) {
        ਨਤੀਜਾ = CURLE_OK;
      }
    }
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ
    *pnwritten += plainwritten;

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
ਸਥਿਰ uint32_t cr_verify_none(ਖਾਲੀ *userdata,
                               ਅਚਲ rustls_verify_server_cert_params *params)
{
  (ਖਾਲੀ)userdata;
  (ਖਾਲੀ)params;
  ਵਾਪਸ RUSTLS_ਨਤੀਜਾ_OK;
}

ਸਥਿਰ ਪੂਰਨ read_file_into(ਅਚਲ ਅੱਖਰ *filename, ਬਣਤਰ dynbuf *ਬਾਹਰਲਾ)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  ਜੇ(!f) {
    ਵਾਪਸ 0;
  }

  ਲਈ(;;) {
    uint8_t buf[256];
    ਅਚਲ size_t rr = fread(buf, 1, ਆਕਾਰ(buf), f);
    ਜੇ((!rr && !feof(f)) ||
       curlx_dyn_addn(ਬਾਹਰਲਾ, buf, rr)) {
      curlx_fclose(f);
      ਵਾਪਸ 0;
    }
    ਜੇ(rr < ਆਕਾਰ(buf))
      ਰੋਕੋ;
  }

  ਵਾਪਸ curlx_fclose(f) == 0;
}

ਸਥਿਰ ਖਾਲੀ
cr_get_selected_ciphers(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *ciphers12,
                        ਅਚਲ ਅੱਖਰ *ciphers13,
                        ਅਚਲ ਬਣਤਰ rustls_supported_ciphersuite **selected,
                        size_t *selected_ਮਾਪ)
{
  ਅਚਲ size_t supported_len = *selected_ਮਾਪ;
  ਅਚਲ size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ਅਚਲ ਬਣਤਰ rustls_supported_ciphersuite *ਇੰਦਰਾਜ = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *ciphers = ciphers12;
  size_t ਗਿਣਤੀ_2 = 0, default13_ਗਿਣਤੀ = 0, i, j;
  ਅਚਲ ਅੱਖਰ *ptr, *ਸਿਰਾ;

  DEBUGASSERT(default_len <= supported_len);

  ਜੇ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    ਲਈ(j = 0; j < default_len; j++) {
      ਇੰਦਰਾਜ = rustls_default_crypto_provider_ciphersuites_get(j);
      ਜੇ(rustls_supported_ciphersuite_protocol_ਸੰਸਕਰਣ(ਇੰਦਰਾਜ) !=
         RUSTLS_TLS_ਸੰਸਕਰਣ_TLSV1_3)
        ਜਾਰੀ;

      selected[ਗਿਣਤੀ_2++] = ਇੰਦਰਾਜ;
    }

    default13_ਗਿਣਤੀ = ਗਿਣਤੀ_2;

    ਜੇ(!ciphers)
      ciphers = "";
  }
  ਨਹੀਂਤਾਂ
    ciphers = ciphers13;

ਜੋੜੋ_ciphers:
  ਲਈ(ptr = ciphers; ptr[0] != '\0' && ਗਿਣਤੀ_2 < supported_len; ptr = ਸਿਰਾ) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &ਸਿਰਾ);

    /* Check if cipher is supported */
    ਜੇ(id) {
      ਲਈ(i = 0; i < supported_len; i++) {
        ਇੰਦਰਾਜ = rustls_default_crypto_provider_ciphersuites_get(i);
        ਜੇ(rustls_supported_ciphersuite_get_suite(ਇੰਦਰਾਜ) == id)
          ਰੋਕੋ;
      }
      ਜੇ(i == supported_len)
        id = 0;
    }
    ਜੇ(!id) {
      ਜੇ(ptr[0] != '\0')
        infof(ਅੰਕੜੇ, "rustls: unknown cipher in list: \"%.*s\"",
              (ਪੂਰਨ)(ਸਿਰਾ - ptr), ptr);
      ਜਾਰੀ;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    ਲਈ(i = 0; i < ਗਿਣਤੀ_2 && selected[i] != ਇੰਦਰਾਜ; i++)
      ;
    ਜੇ(i < ਗਿਣਤੀ_2) {
      ਜੇ(i >= default13_ਗਿਣਤੀ)
        infof(ਅੰਕੜੇ, "rustls: duplicate cipher in list: \"%.*s\"",
              (ਪੂਰਨ)(ਸਿਰਾ - ptr), ptr);
      ਜਾਰੀ;
    }

    selected[ਗਿਣਤੀ_2++] = ਇੰਦਰਾਜ;
  }

  ਜੇ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    ਜਾਓ ਜੋੜੋ_ciphers;
  }

  ਜੇ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    ਲਈ(j = 0; j < default_len; j++) {
      ਇੰਦਰਾਜ = rustls_default_crypto_provider_ciphersuites_get(j);
      ਜੇ(rustls_supported_ciphersuite_protocol_ਸੰਸਕਰਣ(ਇੰਦਰਾਜ) ==
         RUSTLS_TLS_ਸੰਸਕਰਣ_TLSV1_3)
        ਜਾਰੀ;

      /* No duplicates allowed (so selected cannot overflow) */
      ਲਈ(i = 0; i < ਗਿਣਤੀ_2 && selected[i] != ਇੰਦਰਾਜ; i++)
        ;
      ਜੇ(i < ਗਿਣਤੀ_2)
        ਜਾਰੀ;

      selected[ਗਿਣਤੀ_2++] = ਇੰਦਰਾਜ;
    }
  }

  *selected_ਮਾਪ = ਗਿਣਤੀ_2;
}

ਸਥਿਰ ਖਾਲੀ cr_keylog_log_cb(ਬਣਤਰ rustls_str label,
                             ਅਚਲ uint8_t *client_random,
                             size_t client_random_len, ਅਚਲ uint8_t *secret,
                             size_t secret_len)
{
  ਅੱਖਰ clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_ਮਾਪ);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, ਆਕਾਰ(clabel), "%.*s", (ਪੂਰਨ)label.len, label.ਅੰਕੜੇ);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

ਸਥਿਰ CURLcode
init_config_builder(ਬਣਤਰ Curl_cfilter *cf,
                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                    ਅਚਲ ਬਣਤਰ ssl_filter_config *conn_config,
                    ਬਣਤਰ rustls_client_config_builder **config_builder)
{
  ਅਚਲ ਬਣਤਰ rustls_supported_ciphersuite **cipher_suites = ਨਿਰਾ;
  ਬਣਤਰ rustls_crypto_provider_builder *custom_provider_builder = ਨਿਰਾ;
  ਅਚਲ ਬਣਤਰ rustls_crypto_provider *custom_provider = ਨਿਰਾ;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_ਸੰਸਕਰਣ_TLSV1_2,
    RUSTLS_TLS_ਸੰਸਕਰਣ_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode ਨਤੀਜਾ = CURLE_OK;
  rustls_ਨਤੀਜਾ rr;

  DEBUGASSERT(conn_config->ਸੰਸਕਰਣ_2 != CURL_SSLVERSION_DEFAULT);
  ਚੋਣ(conn_config->ਸੰਸਕਰਣ_2) {
  ਹਾਲਤ CURL_SSLVERSION_TLSv1:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_0:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_1:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_2:
    ਰੋਕੋ;
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_ਸੰਸਕਰਣ_TLSV1_3;
    tls_versions_len = 1;
    ਰੋਕੋ;
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "rustls: unsupported minimum TLS version value");
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ cleanup;
  }

  ਚੋਣ(conn_config->ਸੰਸਕਰਣ_max) {
  ਹਾਲਤ CURL_SSLVERSION_MAX_DEFAULT:
  ਹਾਲਤ CURL_SSLVERSION_MAX_NONE:
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_3:
    ਰੋਕੋ;
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_2:
    ਜੇ(tls_versions[0] == RUSTLS_TLS_ਸੰਸਕਰਣ_TLSV1_2) {
      tls_versions_len = 1;
      ਰੋਕੋ;
    }
    FALLTHROUGH();
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_1:
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_0:
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "rustls: unsupported maximum TLS version value");
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ cleanup;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
  ਜੇ(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_ਸੰਸਕਰਣ_TLSV1_3;
    tls_versions_len = 1;
    infof(ਅੰਕੜੇ, "rustls: ECH enabled, forcing TLSv1.3");
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)cf;
#ਜੇ_ਅੰਤ /* USE_ECH */

  cipher_suites = curlx_malloc(ਆਕਾਰ(*cipher_suites) * cipher_suites_len);
  ਜੇ(!cipher_suites) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ cleanup;
  }

  cr_get_selected_ciphers(ਅੰਕੜੇ,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  ਜੇ(cipher_suites_len == 0) {
    failf(ਅੰਕੜੇ, "rustls: no supported cipher in list");
    ਨਤੀਜਾ = CURLE_SSL_CIPHER;
    ਜਾਓ cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr,
                 "failed to create crypto provider builder from default");
    ਨਤੀਜਾ = CURLE_SSL_CIPHER;
    ਜਾਓ cleanup;
  }

  rr =
    rustls_crypto_provider_builder_ਸੈੱਟ_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr,
                 "failed to set ciphersuites for crypto provider builder");
    ਨਤੀਜਾ = CURLE_SSL_CIPHER;
    ਜਾਓ cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "failed to build custom crypto provider");
    ਨਤੀਜਾ = CURLE_SSL_CIPHER;
    ਜਾਓ cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "failed to create client config builder");
    ਨਤੀਜਾ = CURLE_SSL_CIPHER;
    ਜਾਓ cleanup;
  }

cleanup:
  ਜੇ(cipher_suites) {
    curlx_free(cipher_suites);
  }
  ਜੇ(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  ਜੇ(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ
init_config_builder_alpn(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਅਚਲ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl,
                         ਬਣਤਰ rustls_client_config_builder *config_builder)
{
  ਬਣਤਰ alpn_proto_buf proto;
  rustls_ਟੁਕੜਾ_ਬਾਈਟਾਂ alpn[ALPN_ENTRIES_MAX];
  size_t i;

  ਲਈ(i = 0; i < connssl->alpn->ਗਿਣਤੀ_2; ++i) {
    alpn[i].ਅੰਕੜੇ = (ਅਚਲ uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = ਲੜੀ_ਲੰਬਾਈ(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_ਸੈੱਟ_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->ਗਿਣਤੀ_2);
  Curl_alpn_ਮੰਜ਼ਿਲ_proto_str(&proto, connssl->alpn);
  infof(ਅੰਕੜੇ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode init_config_builder_verifier_crl(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਅਚਲ ਬਣਤਰ ssl_filter_config *conn_config,
  ਬਣਤਰ rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ dynbuf crl_contents;
  rustls_ਨਤੀਜਾ rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_ਮਾਪ);
  ਜੇ(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(ਅੰਕੜੇ, "rustls: failed to read revocation list file");
    ਨਤੀਜਾ = CURLE_SSL_CRL_BADFILE;
    ਜਾਓ cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_ਜੋੜੋ_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "failed to parse revocation list");
    ਨਤੀਜਾ = CURLE_SSL_CRL_BADFILE;
    ਜਾਓ cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode
init_config_builder_verifier(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਬਣਤਰ rustls_client_config_builder *builder,
                             ਅਚਲ ਬਣਤਰ ssl_filter_config *conn_config,
                             ਅਚਲ ਬਣਤਰ curl_blob *ca_info_blob,
                             ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_cafile)
{
  ਅਚਲ ਬਣਤਰ rustls_ਜੜ੍ਹ_cert_store *roots = ਨਿਰਾ;
  ਬਣਤਰ rustls_ਜੜ੍ਹ_cert_store_builder *roots_builder = ਨਿਰਾ;
  ਬਣਤਰ rustls_web_pki_server_cert_verifier_builder *verifier_builder = ਨਿਰਾ;
  ਬਣਤਰ rustls_server_cert_verifier *server_cert_verifier = ਨਿਰਾ;
  rustls_ਨਤੀਜਾ rr = RUSTLS_ਨਤੀਜਾ_OK;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  roots_builder = rustls_ਜੜ੍ਹ_cert_store_builder_new();
  ਜੇ(ca_info_blob) {
    rr = rustls_ਜੜ੍ਹ_cert_store_builder_ਜੋੜੋ_pem(roots_builder,
                                                ca_info_blob->ਅੰਕੜੇ,
                                                ca_info_blob->len,
                                                1);
    ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
      rustls_failf(ਅੰਕੜੇ, rr, "failed to parse trusted certificates from blob");
      ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
      ਜਾਓ cleanup;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(ssl_cafile) {
    rr = rustls_ਜੜ੍ਹ_cert_store_builder_ਲੱਦੋ_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
      rustls_failf(ਅੰਕੜੇ, rr, "failed to load trusted certificates");
      ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
      ਜਾਓ cleanup;
    }
  }

  rr = rustls_ਜੜ੍ਹ_cert_store_builder_build(roots_builder, &roots);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "failed to build trusted root certificate store");
    ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
    ਜਾਓ cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  ਜੇ(!verifier_builder) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ cleanup;
  }

  ਜੇ(conn_config->CRLfile) {
    ਨਤੀਜਾ = init_config_builder_verifier_crl(ਅੰਕੜੇ,
                                              conn_config,
                                              verifier_builder);
    ਜੇ(ਨਤੀਜਾ) {
      ਜਾਓ cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "failed to build certificate verifier");
    ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
    ਜਾਓ cleanup;
  }

  rustls_client_config_builder_ਸੈੱਟ_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  ਜੇ(roots_builder) {
    rustls_ਜੜ੍ਹ_cert_store_builder_free(roots_builder);
  }
  ਜੇ(roots) {
    rustls_ਜੜ੍ਹ_cert_store_free(roots);
  }
  ਜੇ(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  ਜੇ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode init_config_builder_platform_verifier(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ rustls_client_config_builder *builder)
{
  ਬਣਤਰ rustls_server_cert_verifier *server_cert_verifier = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  rustls_ਨਤੀਜਾ rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "failed to create platform certificate verifier");
    ਨਤੀਜਾ = CURLE_SSL_CACERT_BADFILE;
    ਜਾਓ cleanup;
  }

  rustls_client_config_builder_ਸੈੱਟ_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  ਜੇ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode
init_config_builder_keylog(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ rustls_client_config_builder *builder)
{
  rustls_ਨਤੀਜਾ rr;

  Curl_tls_keylog_open();
  ਜੇ(!Curl_tls_keylog_enabled()) {
    ਵਾਪਸ CURLE_OK;
  }

  rr = rustls_client_config_builder_ਸੈੱਟ_ਕੁੰਜੀ_log(builder,
                                                cr_keylog_log_cb,
                                                ਨਿਰਾ);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    ਵਾਪਸ map_error(rr);
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode
init_config_builder_client_auth(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਅਚਲ ਬਣਤਰ ssl_filter_config *conn_config,
                                ਬਣਤਰ rustls_client_config_builder *builder)
{
  ਬਣਤਰ dynbuf cert_contents;
  ਬਣਤਰ dynbuf ਕੁੰਜੀ_contents;
  rustls_ਨਤੀਜਾ rr;
  ਅਚਲ ਬਣਤਰ rustls_certified_ਕੁੰਜੀ *certified_ਕੁੰਜੀ = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਅਚਲ bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ਅਚਲ bool have_ਕੁੰਜੀ = conn_config->ਕੁੰਜੀ || conn_config->ਕੁੰਜੀ_blob;

  ਜੇ(have_cert && !have_ਕੁੰਜੀ) {
    failf(ਅੰਕੜੇ, "rustls: must provide key with certificate");
    ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!have_cert && have_ਕੁੰਜੀ) {
    failf(ਅੰਕੜੇ, "rustls: must provide certificate with key");
    ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
  }

  ਜੇ(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(ਅੰਕੜੇ, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
  }
  ਜੇ(conn_config->ਕੁੰਜੀ_type && !curl_strequal(conn_config->ਕੁੰਜੀ_type, "PEM")) {
    failf(ਅੰਕੜੇ, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->ਕੁੰਜੀ_type);
    ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_ਮਾਪ);
  curlx_dyn_init(&ਕੁੰਜੀ_contents, DYN_KEYFILE_ਮਾਪ);

  ਜੇ(conn_config->cert_blob) {
    ਨਤੀਜਾ = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->ਅੰਕੜੇ,
                            conn_config->cert_blob->len);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "rustls: failed to copy client certificate blob (%s)",
            ਨਤੀਜਾ == CURLE_TOO_LARGE ? "too large" : "out of memory");
      ਜਾਓ cleanup;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(ਅੰਕੜੇ, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
    ਜਾਓ cleanup;
  }

  ਜੇ(conn_config->ਕੁੰਜੀ_blob) {
    ਨਤੀਜਾ = curlx_dyn_addn(&ਕੁੰਜੀ_contents,
                            conn_config->ਕੁੰਜੀ_blob->ਅੰਕੜੇ,
                            conn_config->ਕੁੰਜੀ_blob->len);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "rustls: failed to copy key blob (%s)",
            ਨਤੀਜਾ == CURLE_TOO_LARGE ? "too large" : "out of memory");
      ਜਾਓ cleanup;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(!read_file_into(conn_config->ਕੁੰਜੀ, &ਕੁੰਜੀ_contents)) {
    failf(ਅੰਕੜੇ, "rustls: failed to read key file: '%s'",
          conn_config->ਕੁੰਜੀ);
    ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
    ਜਾਓ cleanup;
  }

  rr = rustls_certified_ਕੁੰਜੀ_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&ਕੁੰਜੀ_contents),
                                  curlx_dyn_len(&ਕੁੰਜੀ_contents),
                                  &certified_ਕੁੰਜੀ);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "rustls: failed to build certified key");
    ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
    ਜਾਓ cleanup;
  }

  rr = rustls_certified_ਕੁੰਜੀ_ਕੁੰਜੀਆਂ_ਮੇਲ(certified_ਕੁੰਜੀ);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr,
                 "rustls: client certificate and keypair files do not match:");
    ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
    ਜਾਓ cleanup;
  }

  rr = rustls_client_config_builder_ਸੈੱਟ_certified_ਕੁੰਜੀ(builder,
                                                      &certified_ਕੁੰਜੀ,
                                                      1);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "rustls: failed to set certified key");
    ਨਤੀਜਾ = CURLE_SSL_CERTPROBLEM;
    ਜਾਓ cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&ਕੁੰਜੀ_contents);
  ਜੇ(certified_ਕੁੰਜੀ) {
    rustls_certified_ਕੁੰਜੀ_free(certified_ਕੁੰਜੀ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH

ਸਥਿਰ CURLcode
init_config_builder_ech(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਬਣਤਰ Curl_cfilter *cf,
                        ਬਣਤਰ rustls_client_config_builder *builder)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਅਚਲ rustls_hpke *hpke = rustls_supported_hpke();
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ech_config = ਨਿਰਾ;
  size_t ech_config_len = 0;
  ਬਣਤਰ Curl_dns_ਇੰਦਰਾਜ *dns = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  rustls_ਨਤੀਜਾ rr;

  ਜੇ(!hpke) {
    failf(ਅੰਕੜੇ,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
    ਜਾਓ cleanup;
  }

  ਜੇ(conn_config->ech_ਜਨਤਕ) {
    failf(ਅੰਕੜੇ, "rustls: ECH outername not supported");
    ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
    ਜਾਓ cleanup;
  }

  ਜੇ(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
      rustls_failf(ਅੰਕੜੇ, rr, "rustls: failed to configure ECH GREASE");
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ cleanup;
    }
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(conn_config->ech && conn_config->ech_config) {
    ਅਚਲ ਅੱਖਰ *b64 = conn_config->ech_config;
    size_t decode_ਨਤੀਜਾ;
    ਜੇ(!b64[0]) {
      infof(ਅੰਕੜੇ, "rustls: ECHConfig from command line empty");
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_ਨਤੀਜਾ = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    ਜੇ(decode_ਨਤੀਜਾ || !ech_config) {
      infof(ਅੰਕੜੇ, "rustls: cannot base64 decode ECHConfig from command line");
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ cleanup;
    }
  }
  ਨਹੀਂਤਾਂ {
    ਅਚਲ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
    ਅਚਲ ਬਣਤਰ Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(ਅੰਕੜੇ, cf->sockindex, connssl->peer.origin);

    ਜੇ(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(ਅੰਕੜੇ, "rustls: ECH requested but no ECHConfig available");
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "rustls: failed to configure ECH");
    ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
    ਜਾਓ cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  ਜੇ(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  ਜੇ(dns) {
    Curl_dns_ਇੰਦਰਾਜ_unlink(ਅੰਕੜੇ, &dns);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ /* USE_ECH */

ਸਥਿਰ CURLcode cr_init_backend(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ * ਅਚਲ backend)
{
  ਅਚਲ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਅਚਲ ਬਣਤਰ ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ rustls_connection *rconn = ਨਿਰਾ;
  ਬਣਤਰ rustls_client_config_builder *config_builder = ਨਿਰਾ;

  ਅਚਲ ਬਣਤਰ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ਨਿਰਾ : conn_config->CAfile);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  rustls_ਨਤੀਜਾ rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  ਨਤੀਜਾ = init_config_builder(cf, ਅੰਕੜੇ, conn_config, &config_builder);
  ਜੇ(ਨਤੀਜਾ != CURLE_OK) {
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(connssl->alpn) {
    init_config_builder_alpn(ਅੰਕੜੇ, connssl, config_builder);
  }

  ਜੇ(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_ਸੈੱਟ_certificate_verifier(
      config_builder, cr_verify_none);
  }
  ਨਹੀਂਤਾਂ ਜੇ(conn_config->native_ca_store) {
    ਜੇ(conn_config->CRLfile) {
      failf(ਅੰਕੜੇ, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
    }
    ਨਤੀਜਾ = init_config_builder_platform_verifier(ਅੰਕੜੇ, config_builder);
    ਜੇ(ਨਤੀਜਾ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(ca_info_blob || ssl_cafile) {
    ਨਤੀਜਾ = init_config_builder_verifier(ਅੰਕੜੇ,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    ਜੇ(ਨਤੀਜਾ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

  ਜੇ(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->ਕੁੰਜੀ || conn_config->ਕੁੰਜੀ_blob) {
    ਨਤੀਜਾ = init_config_builder_client_auth(ਅੰਕੜੇ,
                                             conn_config,
                                             config_builder);
    ਜੇ(ਨਤੀਜਾ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
  ਜੇ(Curl_ssl_ech_enabled(cf)) {
    ਨਤੀਜਾ = init_config_builder_ech(ਅੰਕੜੇ, cf, config_builder);
    ਜੇ((ਨਤੀਜਾ != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
#ਜੇ_ਅੰਤ /* USE_ECH */

  ਨਤੀਜਾ = init_config_builder_keylog(ਅੰਕੜੇ, config_builder);
  ਜੇ(ਨਤੀਜਾ != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    ਵਾਪਸ ਨਤੀਜਾ;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "failed to build client config");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  ਜੇ(rr != RUSTLS_ਨਤੀਜਾ_OK) {
    rustls_failf(ਅੰਕੜੇ, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = ਨਿਰਾ;
    ਵਾਪਸ CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_ਸੈੱਟ_userdata(rconn, backend);
  backend->conn = rconn;

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cr_ਸੈੱਟ_negotiated_alpn(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਅਚਲ ਬਣਤਰ rustls_connection *rconn)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ * ਅਚਲ connssl = cf->ctx;
  ਅਚਲ uint8_t *protocol = ਨਿਰਾ;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_ਸੈੱਟ_negotiated(cf, ਅੰਕੜੇ, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
ਸਥਿਰ CURLcode cr_connect(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ * ਅਚਲ connssl = cf->ctx;
  ਅਚਲ ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ * ਅਚਲ backend =
    (ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਅਚਲ ਬਣਤਰ rustls_connection *rconn = ਨਿਰਾ;
  CURLcode tmperr = CURLE_OK;
  CURLcode ਨਤੀਜਾ;
  bool ਚਾਹੁੰਦਾ_read;
  bool ਚਾਹੁੰਦਾ_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(ਅੰਕੜੇ, cf, "cr_connect, state=%d", (ਪੂਰਨ)connssl->state);
  *ਪੂਰਾ = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    ਜੇ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(ਅੰਕੜੇ, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "need HTTPS-RR for ECH, delaying connect");
      ਵਾਪਸ CURLE_OK;
    }
#ਜੇ_ਅੰਤ /* USE_ECH */

  ਜੇ(!backend->conn) {
    ਨਤੀਜਾ =
      cr_init_backend(cf, ਅੰਕੜੇ,
                      (ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *)connssl->backend);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cr_connect, init backend -> %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  ਲਈ(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    ਜੇ(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_ਸੈੱਟ_negotiated_alpn(cf, ਅੰਕੜੇ, rconn);
      tmperr = cr_send(cf, ਅੰਕੜੇ, ਨਿਰਾ, 0, &nwritten);
      ਜੇ(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ਵਾਪਸ CURLE_OK;
      }
      ਨਹੀਂਤਾਂ ਜੇ(tmperr != CURLE_OK) {
        ਵਾਪਸ tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
        ਅਚਲ uint16_t proto = rustls_connection_get_protocol_ਸੰਸਕਰਣ(rconn);
        ਅਚਲ rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ਅਚਲ rustls_str kex_ਸਮੂਹ_name =
          rustls_connection_get_negotiated_ਕੁੰਜੀ_exchange_ਸਮੂਹ_name(rconn);
        ਅਚਲ ਅੱਖਰ *ver = "TLS version unknown";
        ਜੇ(proto == RUSTLS_TLS_ਸੰਸਕਰਣ_TLSV1_3)
          ver = "TLSv1.3";
        ਜੇ(proto == RUSTLS_TLS_ਸੰਸਕਰਣ_TLSV1_2)
          ver = "TLSv1.2";
#ਜੇ_ਅੰਤ
        infof(ਅੰਕੜੇ,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (ਪੂਰਨ)ciphersuite_name.len,
              ciphersuite_name.ਅੰਕੜੇ,
              (ਪੂਰਨ)kex_ਸਮੂਹ_name.len,
              kex_ਸਮੂਹ_name.ਅੰਕੜੇ);
      }
      ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo) {
        size_t ਅੰਕ_certs = 0;
        size_t i;
        ਜਦੋਂ(rustls_connection_get_peer_certificate(rconn, ਅੰਕ_certs)) {
          ਅੰਕ_certs++;
          ਜੇ(ਅੰਕ_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(ਅੰਕੜੇ, "%zu certificates is more than allowed (%d)",
                  ਅੰਕ_certs, MAX_ALLOWED_CERT_AMOUNT);
            ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
          }
        }
        ਨਤੀਜਾ = Curl_ssl_init_certinfo(ਅੰਕੜੇ, (ਪੂਰਨ)ਅੰਕ_certs);
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
        ਲਈ(i = 0; i < ਅੰਕ_certs; i++) {
          ਅਚਲ rustls_certificate *cert;
          ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *der_ਅੰਕੜੇ;
          size_t der_len;
          rustls_ਨਤੀਜਾ rresult = RUSTLS_ਨਤੀਜਾ_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_ਅੰਕੜੇ, &der_len);
          ਜੇ(rresult != RUSTLS_ਨਤੀਜਾ_OK) {
            ਅੱਖਰ errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, ਆਕਾਰ(errorbuf), &errorlen);
            failf(ਅੰਕੜੇ,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (ਪੂਰਨ)errorlen, errorbuf);
            ਵਾਪਸ map_error(rresult);
          }
          ਨਤੀਜਾ = Curl_ਕੱਢੋ_certinfo(ਅੰਕੜੇ, (ਪੂਰਨ)i,
                                         der_ਅੰਕੜੇ, der_ਅੰਕੜੇ + der_len);
          ਜੇ(ਨਤੀਜਾ)
            ਵਾਪਸ ਨਤੀਜਾ;
        }
      }

      connssl->state = ssl_connection_complete;
      *ਪੂਰਾ = TRUE;
      ਵਾਪਸ CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    ਚਾਹੁੰਦਾ_read = rustls_connection_ਚਾਹੁੰਦਾ_read(rconn);
    ਚਾਹੁੰਦਾ_write = rustls_connection_ਚਾਹੁੰਦਾ_write(rconn) ||
                  backend->plain_ਬਾਹਰਲਾ_buffered;
    DEBUGASSERT(ਚਾਹੁੰਦਾ_read || ਚਾਹੁੰਦਾ_write);

    ਜੇ(ਚਾਹੁੰਦਾ_write) {
      size_t nwritten;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, ਅੰਕੜੇ, ਨਿਰਾ, 0, &nwritten);
      ਜੇ(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ਵਾਪਸ CURLE_OK;
      }
      ਨਹੀਂਤਾਂ ਜੇ(tmperr != CURLE_OK) {
        ਵਾਪਸ tmperr;
      }
    }

    ਜੇ(ਚਾਹੁੰਦਾ_read) {
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_ਹੋਰ(cf, ਅੰਕੜੇ, &tmperr);
      ਜੇ(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(ਅੰਕੜੇ, "TLS connect error: Connection closed abruptly");
        ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
      }
      ਜੇ(nread < 0) {
        ਜੇ(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(ਅੰਕੜੇ, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          ਵਾਪਸ CURLE_OK;
        }
        ਨਹੀਂਤਾਂ ਜੇ(tmperr == CURLE_RECV_ERROR) {
          ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
        }
        ਨਹੀਂਤਾਂ {
          ਵਾਪਸ tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

ਸਥਿਰ ਖਾਲੀ *cr_get_internals(ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl, CURLINFO info)
{
  ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  (ਖਾਲੀ)info;
  DEBUGASSERT(backend);
  ਵਾਪਸ backend->conn;
}

ਸਥਿਰ CURLcode cr_shutdown(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ bool send_shutdown, bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  ਜੇ(!backend->conn || cf->shutdown) {
    *ਪੂਰਾ = TRUE;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *ਪੂਰਾ = FALSE;

  ਜੇ(!backend->ਭੇਜਿਆ_shutdown) {
    /* do this only once */
    backend->ਭੇਜਿਆ_shutdown = TRUE;
    ਜੇ(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  ਨਤੀਜਾ = cr_send(cf, ਅੰਕੜੇ, ਨਿਰਾ, 0, &nwritten);
  ਜੇ(ਨਤੀਜਾ) {
    ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ਨਤੀਜਾ = CURLE_OK;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    DEBUGASSERT(ਨਤੀਜਾ);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown send failed: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਲਈ(i = 0; i < 10; ++i) {
    ਅੱਖਰ buf[1024];
    ਨਤੀਜਾ = cr_recv(cf, ਅੰਕੜੇ, buf, (ਪੂਰਨ)ਆਕਾਰ(buf), &nread);
    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;
  }

  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ਨਤੀਜਾ = CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
    DEBUGASSERT(ਨਤੀਜਾ);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown, error: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(nread == 0) {
    /* We got the close notify alert and are done. */
    *ਪੂਰਾ = TRUE;
  }

ਬਾਹਰਲਾ:
  cf->shutdown = (ਨਤੀਜਾ || *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cr_close(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਅਚਲ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ *)connssl->backend;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(backend);
  ਜੇ(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = ਨਿਰਾ;
  }
  ਜੇ(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = ਨਿਰਾ;
  }
}

ਸਥਿਰ size_t cr_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ)
{
  ਅਚਲ ਬਣਤਰ rustls_str ver = rustls_ਸੰਸਕਰਣ();
  ਵਾਪਸ curl_msnprintf(ਬਫਰ, ਮਾਪ, "%.*s", (ਪੂਰਨ)ver.len, ver.ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode cr_random(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *entropy,
                          size_t ਲੰਮਾਈ)
{
  rustls_ਨਤੀਜਾ rresult = 0;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  rresult = rustls_default_crypto_provider_random(entropy, ਲੰਮਾਈ);
  ਵਾਪਸ map_error(rresult);
}

ਸਥਿਰ ਖਾਲੀ cr_cleanup(ਖਾਲੀ)
{
  Curl_tls_keylog_close();
}

ਅਚਲ ਬਣਤਰ Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  ਆਕਾਰ(ਬਣਤਰ rustls_ssl_backend_ਅੰਕੜੇ),

  ਨਿਰਾ,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_ਸੰਸਕਰਣ,                      /*ਸੰਸਕਰਣ*/
  cr_shutdown,                     /* shutdown */
  cr_ਅੰਕੜੇ_pending,                 /* data_pending */
  cr_random,                       /* random */
  ਨਿਰਾ,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  ਨਿਰਾ,                            /* close_all */
  ਨਿਰਾ,                            /* engines_list */
  ਨਿਰਾ,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  ਨਿਰਾ,                            /* get_channel_binding */
};

#ਜੇ_ਅੰਤ /* USE_RUSTLS */
