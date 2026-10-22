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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RUSTLS

#ಅನ್ತರ್ಭಾವ <rustls.h>

#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "curlx/strerr.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "vdns/httpsrr.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"
#ಅನ್ತರ್ಭಾವ "vtls/rustls.h"
#ಅನ್ತರ್ಭಾವ "vtls/keylog.h"
#ಅನ್ತರ್ಭಾವ "vtls/cipher_suite.h"
#ಅನ್ತರ್ಭಾವ "vtls/x509asn1.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ EAGAIN != EWOULDBLOCK
#ವ್ಯಾಖ್ಯಾನಿಸು RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ {
  ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_client_config *config;
  ರಚನೆ rustls_connection *conn;
  size_t plain_ಹೊರಹೋಗು_buffered;
  ಬಿಟ್(ದತ್ತಾಂಶ_ಒಳ_pending);
  ಬಿಟ್(ಕಳುಹಿಸಿದ_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
ಸ್ಥಿರ CURLcode map_error(ಸ್ಥಿರಾಂಕ rustls_ಫಲಿತಾಂಶ r)
{
  ಆದರೆ(rustls_ಫಲಿತಾಂಶ_is_cert_error(r)) {
    ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
  }
  ಆಯ್ಕೆ(r) {
  ಸ್ಥಿತಿ RUSTLS_ಫಲಿತಾಂಶ_OK:
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ RUSTLS_ಫಲಿತಾಂಶ_NULL_PARAMETER:
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rustls_failf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ rustls_ಫಲಿತಾಂಶ rr,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ)
{
  ಅಕ್ಷರ errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, ಗಾತ್ರ(errorbuf), &errorlen);
  failf(ದತ್ತಾಂಶ, "%s: %.*s", ಸಂದೇಶ, (ಪೂರ್ಣಾಂಕ)errorlen, errorbuf);
}

ಸ್ಥಿರ bool cr_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                            ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *ctx = cf->ctx;
  ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *backend;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *)ctx->backend;
  ಹಿಂತಿರುಗಿ (bool)backend->ದತ್ತಾಂಶ_ಒಳ_pending;
}

ರಚನೆ io_ctx {
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_cb(ಶೂನ್ಯ *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *ಹೊರಹೋಗು_n)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ io_ctx *io_ctx = userdata;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ connssl = io_ctx->cf->ctx;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ ret = 0;
  size_t nread;

  ಫಲಿತಾಂಶ = Curl_conn_cf_recv(io_ctx->cf->ಮುಂದಿನ, io_ctx->ದತ್ತಾಂಶ,
                             (ಅಕ್ಷರ *)buf, len, &nread);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ret = EAGAIN;
    ಇಲ್ಲದಿದ್ದರೆ
      ret = EINVAL;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == 0)
    connssl->peer_closed = TRUE;
  *ಹೊರಹೋಗು_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->ದತ್ತಾಂಶ, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nread);
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ write_cb(ಶೂನ್ಯ *userdata, ಸ್ಥಿರಾಂಕ uint8_t *buf, uintptr_t len,
                    uintptr_t *ಹೊರಹೋಗು_n)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ io_ctx *io_ctx = userdata;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ ret = 0;
  size_t nwritten;

  ಫಲಿತಾಂಶ = Curl_conn_cf_send(io_ctx->cf->ಮುಂದಿನ, io_ctx->ದತ್ತಾಂಶ,
                             buf, len, FALSE, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    nwritten = 0;
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
      ret = EAGAIN;
    ಇಲ್ಲದಿದ್ದರೆ
      ret = EINVAL;
  }
  *ಹೊರಹೋಗು_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->ದತ್ತಾಂಶ, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nwritten);
  ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ssize_t tls_recv_ಇನ್ನಷ್ಟು(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, CURLcode *err)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ connssl = cf->ctx;
  ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ backend =
    (ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ io_ctx io_ctx;
  size_t tls_ಬೈಟುಗಳು_read = 0;
  rustls_io_ಫಲಿತಾಂಶ io_error;
  rustls_ಫಲಿತಾಂಶ rresult = 0;

  io_ctx.cf = cf;
  io_ctx.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_ಬೈಟುಗಳು_read);
  ಆದರೆ(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    ಹಿಂತಿರುಗಿ -1;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(io_error) {
    ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
    failf(ದತ್ತಾಂಶ, "reading from socket: %s",
          curlx_strerror(io_error, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
    *err = CURLE_RECV_ERROR;
    ಹಿಂತಿರುಗಿ -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  ಆದರೆ(rresult != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    ಹಿಂತಿರುಗಿ -1;
  }

  backend->ದತ್ತಾಂಶ_ಒಳ_pending = TRUE;
  *err = CURLE_OK;
  ಹಿಂತಿರುಗಿ (ssize_t)tls_ಬೈಟುಗಳು_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
ಸ್ಥಿರ CURLcode cr_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಅಕ್ಷರ *plainbuf, size_t plainlen, size_t *pnread)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ connssl = cf->ctx;
  ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ backend =
    (ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ rustls_connection *rconn = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t n = 0;
  rustls_ಫಲಿತಾಂಶ rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  ಆಗ(*pnread < plainlen) {
    ಆದರೆ(!backend->ದತ್ತಾಂಶ_ಒಳ_pending) {
      ಆದರೆ(tls_recv_ಇನ್ನಷ್ಟು(cf, ದತ್ತಾಂಶ, &ಫಲಿತಾಂಶ) < 0) {
        ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_AGAIN) {
          ಹೋಗು ಹೊರಹೋಗು;
        }
        ಫಲಿತಾಂಶ = CURLE_OK;
        ನಿಲ್ಲಿಸು;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    ಆದರೆ(rresult == RUSTLS_ಫಲಿತಾಂಶ_PLAINTEXT_ಖಾಲಿಗಳು) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->ದತ್ತಾಂಶ_ಒಳ_pending = FALSE;
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rresult == RUSTLS_ಫಲಿತಾಂಶ_UNEXPECTED_EOF) {
      failf(ದತ್ತಾಂಶ, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rresult != RUSTLS_ಫಲಿತಾಂಶ_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(ದತ್ತಾಂಶ, rresult, "rustls_connection_read");
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      *pnread += n;
    }
  }

  ಆದರೆ(!eof && !*pnread) {
    ಫಲಿತಾಂಶ = CURLE_AGAIN;
  }

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnread);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cr_flush_ಹೊರಹೋಗು(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ rustls_connection *rconn)
{
  ರಚನೆ io_ctx io_ctx;
  rustls_io_ಫಲಿತಾಂಶ io_error;
  size_t tlswritten = 0;
  size_t tlswritten_ಒಟ್ಟು = 0;

  io_ctx.cf = cf;
  io_ctx.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ;

  ಆಗ(rustls_connection_ಬಯಸುತ್ತದೆ_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    ಆದರೆ(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_ಒಟ್ಟು);
      ಹಿಂತಿರುಗಿ CURLE_AGAIN;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(io_error) {
      ಅಕ್ಷರ ಬಫರ್[STRERROR_LEN];
      failf(ದತ್ತಾಂಶ, "writing to socket: %s",
            curlx_strerror(io_error, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್)));
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    ಆದರೆ(tlswritten == 0) {
      failf(ದತ್ತಾಂಶ, "EOF in swrite");
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_ಒಟ್ಟು += tlswritten;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
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
ಸ್ಥಿರ CURLcode cr_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ connssl = cf->ctx;
  ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ backend =
    (ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ rustls_connection *rconn = ಖಾಲಿ;
  size_t plainwritten = 0;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf = plainbuf;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  ಆದರೆ(backend->plain_ಹೊರಹೋಗು_buffered) {
    ಫಲಿತಾಂಶ = cr_flush_ಹೊರಹೋಗು(cf, ದತ್ತಾಂಶ, rconn);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_ಹೊರಹೋಗು_buffered, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    ಆದರೆ(blen > backend->plain_ಹೊರಹೋಗು_buffered) {
      blen -= backend->plain_ಹೊರಹೋಗು_buffered;
      buf += backend->plain_ಹೊರಹೋಗು_buffered;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      blen = 0;
    *pnwritten += backend->plain_ಹೊರಹೋಗು_buffered;
    backend->plain_ಹೊರಹೋಗು_buffered = 0;
  }

  ಆದರೆ(blen > 0) {
    rustls_ಫಲಿತಾಂಶ rresult;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    ಆದರೆ(rresult != RUSTLS_ಫಲಿತಾಂಶ_OK) {
      rustls_failf(ದತ್ತಾಂಶ, rresult, "rustls_connection_write");
      ಫಲಿತಾಂಶ = CURLE_WRITE_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(plainwritten == 0) {
      failf(ದತ್ತಾಂಶ, "rustls_connection_write: EOF");
      ಫಲಿತಾಂಶ = CURLE_WRITE_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }

  ಫಲಿತಾಂಶ = cr_flush_ಹೊರಹೋಗು(cf, ದತ್ತಾಂಶ, rconn);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_ಹೊರಹೋಗು_buffered = plainwritten;
      ಆದರೆ(*pnwritten) {
        ಫಲಿತಾಂಶ = CURLE_OK;
      }
    }
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    *pnwritten += plainwritten;

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
ಸ್ಥಿರ uint32_t cr_verify_none(ಶೂನ್ಯ *userdata,
                               ಸ್ಥಿರಾಂಕ rustls_verify_server_cert_params *params)
{
  (ಶೂನ್ಯ)userdata;
  (ಶೂನ್ಯ)params;
  ಹಿಂತಿರುಗಿ RUSTLS_ಫಲಿತಾಂಶ_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ read_file_into(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ರಚನೆ dynbuf *ಹೊರಹೋಗು)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  ಆದರೆ(!f) {
    ಹಿಂತಿರುಗಿ 0;
  }

  ಗಾಗಿ(;;) {
    uint8_t buf[256];
    ಸ್ಥಿರಾಂಕ size_t rr = fread(buf, 1, ಗಾತ್ರ(buf), f);
    ಆದರೆ((!rr && !feof(f)) ||
       curlx_dyn_addn(ಹೊರಹೋಗು, buf, rr)) {
      curlx_fclose(f);
      ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(rr < ಗಾತ್ರ(buf))
      ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ curlx_fclose(f) == 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ
cr_get_selected_ciphers(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ciphers12,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ciphers13,
                        ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_supported_ciphersuite **selected,
                        size_t *selected_ಗಾತ್ರ)
{
  ಸ್ಥಿರಾಂಕ size_t supported_len = *selected_ಗಾತ್ರ;
  ಸ್ಥಿರಾಂಕ size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_supported_ciphersuite *ನಮೂದು = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ciphers = ciphers12;
  size_t ಎಣಿಕೆ_2 = 0, default13_ಎಣಿಕೆ = 0, i, j;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, *ಕೊನೆ;

  DEBUGASSERT(default_len <= supported_len);

  ಆದರೆ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    ಗಾಗಿ(j = 0; j < default_len; j++) {
      ನಮೂದು = rustls_default_crypto_provider_ciphersuites_get(j);
      ಆದರೆ(rustls_supported_ciphersuite_protocol_ಆವೃತ್ತಿ(ನಮೂದು) !=
         RUSTLS_TLS_ಆವೃತ್ತಿ_TLSV1_3)
        ಮುಂದುವರಿ;

      selected[ಎಣಿಕೆ_2++] = ನಮೂದು;
    }

    default13_ಎಣಿಕೆ = ಎಣಿಕೆ_2;

    ಆದರೆ(!ciphers)
      ciphers = "";
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ciphers = ciphers13;

ಕೂಡಿಸು_ciphers:
  ಗಾಗಿ(ptr = ciphers; ptr[0] != '\0' && ಎಣಿಕೆ_2 < supported_len; ptr = ಕೊನೆ) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &ಕೊನೆ);

    /* Check if cipher is supported */
    ಆದರೆ(id) {
      ಗಾಗಿ(i = 0; i < supported_len; i++) {
        ನಮೂದು = rustls_default_crypto_provider_ciphersuites_get(i);
        ಆದರೆ(rustls_supported_ciphersuite_get_suite(ನಮೂದು) == id)
          ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(i == supported_len)
        id = 0;
    }
    ಆದರೆ(!id) {
      ಆದರೆ(ptr[0] != '\0')
        infof(ದತ್ತಾಂಶ, "rustls: unknown cipher in list: \"%.*s\"",
              (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - ptr), ptr);
      ಮುಂದುವರಿ;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    ಗಾಗಿ(i = 0; i < ಎಣಿಕೆ_2 && selected[i] != ನಮೂದು; i++)
      ;
    ಆದರೆ(i < ಎಣಿಕೆ_2) {
      ಆದರೆ(i >= default13_ಎಣಿಕೆ)
        infof(ದತ್ತಾಂಶ, "rustls: duplicate cipher in list: \"%.*s\"",
              (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - ptr), ptr);
      ಮುಂದುವರಿ;
    }

    selected[ಎಣಿಕೆ_2++] = ನಮೂದು;
  }

  ಆದರೆ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    ಹೋಗು ಕೂಡಿಸು_ciphers;
  }

  ಆದರೆ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    ಗಾಗಿ(j = 0; j < default_len; j++) {
      ನಮೂದು = rustls_default_crypto_provider_ciphersuites_get(j);
      ಆದರೆ(rustls_supported_ciphersuite_protocol_ಆವೃತ್ತಿ(ನಮೂದು) ==
         RUSTLS_TLS_ಆವೃತ್ತಿ_TLSV1_3)
        ಮುಂದುವರಿ;

      /* No duplicates allowed (so selected cannot overflow) */
      ಗಾಗಿ(i = 0; i < ಎಣಿಕೆ_2 && selected[i] != ನಮೂದು; i++)
        ;
      ಆದರೆ(i < ಎಣಿಕೆ_2)
        ಮುಂದುವರಿ;

      selected[ಎಣಿಕೆ_2++] = ನಮೂದು;
    }
  }

  *selected_ಗಾತ್ರ = ಎಣಿಕೆ_2;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_keylog_log_cb(ರಚನೆ rustls_str label,
                             ಸ್ಥಿರಾಂಕ uint8_t *client_random,
                             size_t client_random_len, ಸ್ಥಿರಾಂಕ uint8_t *secret,
                             size_t secret_len)
{
  ಅಕ್ಷರ clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_ಗಾತ್ರ);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, ಗಾತ್ರ(clabel), "%.*s", (ಪೂರ್ಣಾಂಕ)label.len, label.ದತ್ತಾಂಶ);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

ಸ್ಥಿರ CURLcode
init_config_builder(ರಚನೆ Curl_cfilter *cf,
                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                    ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_filter_config *conn_config,
                    ರಚನೆ rustls_client_config_builder **config_builder)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_supported_ciphersuite **cipher_suites = ಖಾಲಿ;
  ರಚನೆ rustls_crypto_provider_builder *custom_provider_builder = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_crypto_provider *custom_provider = ಖಾಲಿ;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_ಆವೃತ್ತಿ_TLSV1_2,
    RUSTLS_TLS_ಆವೃತ್ತಿ_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  rustls_ಫಲಿತಾಂಶ rr;

  DEBUGASSERT(conn_config->ಆವೃತ್ತಿ_2 != CURL_SSLVERSION_DEFAULT);
  ಆಯ್ಕೆ(conn_config->ಆವೃತ್ತಿ_2) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_2:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_ಆವೃತ್ತಿ_TLSV1_3;
    tls_versions_len = 1;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "rustls: unsupported minimum TLS version value");
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು cleanup;
  }

  ಆಯ್ಕೆ(conn_config->ಆವೃತ್ತಿ_max) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_DEFAULT:
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_NONE:
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_3:
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_2:
    ಆದರೆ(tls_versions[0] == RUSTLS_TLS_ಆವೃತ್ತಿ_TLSV1_2) {
      tls_versions_len = 1;
      ನಿಲ್ಲಿಸು;
    }
    FALLTHROUGH();
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_1:
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_0:
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "rustls: unsupported maximum TLS version value");
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು cleanup;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
  ಆದರೆ(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_ಆವೃತ್ತಿ_TLSV1_3;
    tls_versions_len = 1;
    infof(ದತ್ತಾಂಶ, "rustls: ECH enabled, forcing TLSv1.3");
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)cf;
#ಆದರೆ_ಅಂತ್ಯ /* USE_ECH */

  cipher_suites = curlx_malloc(ಗಾತ್ರ(*cipher_suites) * cipher_suites_len);
  ಆದರೆ(!cipher_suites) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು cleanup;
  }

  cr_get_selected_ciphers(ದತ್ತಾಂಶ,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  ಆದರೆ(cipher_suites_len == 0) {
    failf(ದತ್ತಾಂಶ, "rustls: no supported cipher in list");
    ಫಲಿತಾಂಶ = CURLE_SSL_CIPHER;
    ಹೋಗು cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr,
                 "failed to create crypto provider builder from default");
    ಫಲಿತಾಂಶ = CURLE_SSL_CIPHER;
    ಹೋಗು cleanup;
  }

  rr =
    rustls_crypto_provider_builder_ಗಣ_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr,
                 "failed to set ciphersuites for crypto provider builder");
    ಫಲಿತಾಂಶ = CURLE_SSL_CIPHER;
    ಹೋಗು cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "failed to build custom crypto provider");
    ಫಲಿತಾಂಶ = CURLE_SSL_CIPHER;
    ಹೋಗು cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "failed to create client config builder");
    ಫಲಿತಾಂಶ = CURLE_SSL_CIPHER;
    ಹೋಗು cleanup;
  }

cleanup:
  ಆದರೆ(cipher_suites) {
    curlx_free(cipher_suites);
  }
  ಆದರೆ(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  ಆದರೆ(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ
init_config_builder_alpn(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl,
                         ರಚನೆ rustls_client_config_builder *config_builder)
{
  ರಚನೆ alpn_proto_buf proto;
  rustls_ತುಂಡು_ಬೈಟುಗಳು alpn[ALPN_ENTRIES_MAX];
  size_t i;

  ಗಾಗಿ(i = 0; i < connssl->alpn->ಎಣಿಕೆ_2; ++i) {
    alpn[i].ದತ್ತಾಂಶ = (ಸ್ಥಿರಾಂಕ uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_ಗಣ_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->ಎಣಿಕೆ_2);
  Curl_alpn_ಗಮ್ಯ_proto_str(&proto, connssl->alpn);
  infof(ದತ್ತಾಂಶ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode init_config_builder_verifier_crl(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_filter_config *conn_config,
  ರಚನೆ rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ dynbuf crl_contents;
  rustls_ಫಲಿತಾಂಶ rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_ಗಾತ್ರ);
  ಆದರೆ(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(ದತ್ತಾಂಶ, "rustls: failed to read revocation list file");
    ಫಲಿತಾಂಶ = CURLE_SSL_CRL_BADFILE;
    ಹೋಗು cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_ಕೂಡಿಸು_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "failed to parse revocation list");
    ಫಲಿತಾಂಶ = CURLE_SSL_CRL_BADFILE;
    ಹೋಗು cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode
init_config_builder_verifier(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ರಚನೆ rustls_client_config_builder *builder,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_filter_config *conn_config,
                             ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ca_info_blob,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_cafile)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_ಬೇರು_cert_store *roots = ಖಾಲಿ;
  ರಚನೆ rustls_ಬೇರು_cert_store_builder *roots_builder = ಖಾಲಿ;
  ರಚನೆ rustls_web_pki_server_cert_verifier_builder *verifier_builder = ಖಾಲಿ;
  ರಚನೆ rustls_server_cert_verifier *server_cert_verifier = ಖಾಲಿ;
  rustls_ಫಲಿತಾಂಶ rr = RUSTLS_ಫಲಿತಾಂಶ_OK;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  roots_builder = rustls_ಬೇರು_cert_store_builder_new();
  ಆದರೆ(ca_info_blob) {
    rr = rustls_ಬೇರು_cert_store_builder_ಕೂಡಿಸು_pem(roots_builder,
                                                ca_info_blob->ದತ್ತಾಂಶ,
                                                ca_info_blob->len,
                                                1);
    ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
      rustls_failf(ದತ್ತಾಂಶ, rr, "failed to parse trusted certificates from blob");
      ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
      ಹೋಗು cleanup;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ssl_cafile) {
    rr = rustls_ಬೇರು_cert_store_builder_ತುಂಬು_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
      rustls_failf(ದತ್ತಾಂಶ, rr, "failed to load trusted certificates");
      ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
      ಹೋಗು cleanup;
    }
  }

  rr = rustls_ಬೇರು_cert_store_builder_build(roots_builder, &roots);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "failed to build trusted root certificate store");
    ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
    ಹೋಗು cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  ಆದರೆ(!verifier_builder) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು cleanup;
  }

  ಆದರೆ(conn_config->CRLfile) {
    ಫಲಿತಾಂಶ = init_config_builder_verifier_crl(ದತ್ತಾಂಶ,
                                              conn_config,
                                              verifier_builder);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      ಹೋಗು cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "failed to build certificate verifier");
    ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
    ಹೋಗು cleanup;
  }

  rustls_client_config_builder_ಗಣ_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  ಆದರೆ(roots_builder) {
    rustls_ಬೇರು_cert_store_builder_free(roots_builder);
  }
  ಆದರೆ(roots) {
    rustls_ಬೇರು_cert_store_free(roots);
  }
  ಆದರೆ(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  ಆದರೆ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode init_config_builder_platform_verifier(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ rustls_client_config_builder *builder)
{
  ರಚನೆ rustls_server_cert_verifier *server_cert_verifier = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  rustls_ಫಲಿತಾಂಶ rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "failed to create platform certificate verifier");
    ಫಲಿತಾಂಶ = CURLE_SSL_CACERT_BADFILE;
    ಹೋಗು cleanup;
  }

  rustls_client_config_builder_ಗಣ_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  ಆದರೆ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode
init_config_builder_keylog(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ rustls_client_config_builder *builder)
{
  rustls_ಫಲಿತಾಂಶ rr;

  Curl_tls_keylog_open();
  ಆದರೆ(!Curl_tls_keylog_enabled()) {
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  rr = rustls_client_config_builder_ಗಣ_ಕೀಲಿ_log(builder,
                                                cr_keylog_log_cb,
                                                ಖಾಲಿ);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    ಹಿಂತಿರುಗಿ map_error(rr);
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode
init_config_builder_client_auth(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_filter_config *conn_config,
                                ರಚನೆ rustls_client_config_builder *builder)
{
  ರಚನೆ dynbuf cert_contents;
  ರಚನೆ dynbuf ಕೀಲಿ_contents;
  rustls_ಫಲಿತಾಂಶ rr;
  ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_certified_ಕೀಲಿ *certified_ಕೀಲಿ = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಸ್ಥಿರಾಂಕ bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ಸ್ಥಿರಾಂಕ bool have_ಕೀಲಿ = conn_config->ಕೀಲಿ || conn_config->ಕೀಲಿ_blob;

  ಆದರೆ(have_cert && !have_ಕೀಲಿ) {
    failf(ದತ್ತಾಂಶ, "rustls: must provide key with certificate");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!have_cert && have_ಕೀಲಿ) {
    failf(ದತ್ತಾಂಶ, "rustls: must provide certificate with key");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
  }

  ಆದರೆ(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(ದತ್ತಾಂಶ, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
  }
  ಆದರೆ(conn_config->ಕೀಲಿ_type && !curl_strequal(conn_config->ಕೀಲಿ_type, "PEM")) {
    failf(ದತ್ತಾಂಶ, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->ಕೀಲಿ_type);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_ಗಾತ್ರ);
  curlx_dyn_init(&ಕೀಲಿ_contents, DYN_KEYFILE_ಗಾತ್ರ);

  ಆದರೆ(conn_config->cert_blob) {
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->ದತ್ತಾಂಶ,
                            conn_config->cert_blob->len);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "rustls: failed to copy client certificate blob (%s)",
            ಫಲಿತಾಂಶ == CURLE_TOO_LARGE ? "too large" : "out of memory");
      ಹೋಗು cleanup;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(ದತ್ತಾಂಶ, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
    ಹೋಗು cleanup;
  }

  ಆದರೆ(conn_config->ಕೀಲಿ_blob) {
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಕೀಲಿ_contents,
                            conn_config->ಕೀಲಿ_blob->ದತ್ತಾಂಶ,
                            conn_config->ಕೀಲಿ_blob->len);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "rustls: failed to copy key blob (%s)",
            ಫಲಿತಾಂಶ == CURLE_TOO_LARGE ? "too large" : "out of memory");
      ಹೋಗು cleanup;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!read_file_into(conn_config->ಕೀಲಿ, &ಕೀಲಿ_contents)) {
    failf(ದತ್ತಾಂಶ, "rustls: failed to read key file: '%s'",
          conn_config->ಕೀಲಿ);
    ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
    ಹೋಗು cleanup;
  }

  rr = rustls_certified_ಕೀಲಿ_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&ಕೀಲಿ_contents),
                                  curlx_dyn_len(&ಕೀಲಿ_contents),
                                  &certified_ಕೀಲಿ);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "rustls: failed to build certified key");
    ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
    ಹೋಗು cleanup;
  }

  rr = rustls_certified_ಕೀಲಿ_ಕೀಲಿಗಳು_ಹೊಂದಿಕೆ(certified_ಕೀಲಿ);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr,
                 "rustls: client certificate and keypair files do not match:");
    ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
    ಹೋಗು cleanup;
  }

  rr = rustls_client_config_builder_ಗಣ_certified_ಕೀಲಿ(builder,
                                                      &certified_ಕೀಲಿ,
                                                      1);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "rustls: failed to set certified key");
    ಫಲಿತಾಂಶ = CURLE_SSL_CERTPROBLEM;
    ಹೋಗು cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&ಕೀಲಿ_contents);
  ಆದರೆ(certified_ಕೀಲಿ) {
    rustls_certified_ಕೀಲಿ_free(certified_ಕೀಲಿ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH

ಸ್ಥಿರ CURLcode
init_config_builder_ech(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ರಚನೆ Curl_cfilter *cf,
                        ರಚನೆ rustls_client_config_builder *builder)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸ್ಥಿರಾಂಕ rustls_hpke *hpke = rustls_supported_hpke();
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ech_config = ಖಾಲಿ;
  size_t ech_config_len = 0;
  ರಚನೆ Curl_dns_ನಮೂದು *dns = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  rustls_ಫಲಿತಾಂಶ rr;

  ಆದರೆ(!hpke) {
    failf(ದತ್ತಾಂಶ,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
    ಹೋಗು cleanup;
  }

  ಆದರೆ(conn_config->ech_ಬಹಿರಂಗ) {
    failf(ದತ್ತಾಂಶ, "rustls: ECH outername not supported");
    ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
    ಹೋಗು cleanup;
  }

  ಆದರೆ(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
      rustls_failf(ದತ್ತಾಂಶ, rr, "rustls: failed to configure ECH GREASE");
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು cleanup;
    }
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(conn_config->ech && conn_config->ech_config) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b64 = conn_config->ech_config;
    size_t decode_ಫಲಿತಾಂಶ;
    ಆದರೆ(!b64[0]) {
      infof(ದತ್ತಾಂಶ, "rustls: ECHConfig from command line empty");
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_ಫಲಿತಾಂಶ = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    ಆದರೆ(decode_ಫಲಿತಾಂಶ || !ech_config) {
      infof(ದತ್ತಾಂಶ, "rustls: cannot base64 decode ECHConfig from command line");
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು cleanup;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(ದತ್ತಾಂಶ, cf->sockindex, connssl->peer.origin);

    ಆದರೆ(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(ದತ್ತಾಂಶ, "rustls: ECH requested but no ECHConfig available");
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "rustls: failed to configure ECH");
    ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
    ಹೋಗು cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  ಆದರೆ(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  ಆದರೆ(dns) {
    Curl_dns_ನಮೂದು_unlink(ದತ್ತಾಂಶ, &dns);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
#ಆದರೆ_ಅಂತ್ಯ /* USE_ECH */

ಸ್ಥಿರ CURLcode cr_init_backend(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ backend)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ rustls_connection *rconn = ಖಾಲಿ;
  ರಚನೆ rustls_client_config_builder *config_builder = ಖಾಲಿ;

  ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ಖಾಲಿ : conn_config->CAfile);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  rustls_ಫಲಿತಾಂಶ rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  ಫಲಿತಾಂಶ = init_config_builder(cf, ದತ್ತಾಂಶ, conn_config, &config_builder);
  ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_OK) {
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(connssl->alpn) {
    init_config_builder_alpn(ದತ್ತಾಂಶ, connssl, config_builder);
  }

  ಆದರೆ(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_ಗಣ_certificate_verifier(
      config_builder, cr_verify_none);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(conn_config->native_ca_store) {
    ಆದರೆ(conn_config->CRLfile) {
      failf(ದತ್ತಾಂಶ, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
    }
    ಫಲಿತಾಂಶ = init_config_builder_platform_verifier(ದತ್ತಾಂಶ, config_builder);
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ca_info_blob || ssl_cafile) {
    ಫಲಿತಾಂಶ = init_config_builder_verifier(ದತ್ತಾಂಶ,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

  ಆದರೆ(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->ಕೀಲಿ || conn_config->ಕೀಲಿ_blob) {
    ಫಲಿತಾಂಶ = init_config_builder_client_auth(ದತ್ತಾಂಶ,
                                             conn_config,
                                             config_builder);
    ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
  ಆದರೆ(Curl_ssl_ech_enabled(cf)) {
    ಫಲಿತಾಂಶ = init_config_builder_ech(ದತ್ತಾಂಶ, cf, config_builder);
    ಆದರೆ((ಫಲಿತಾಂಶ != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ /* USE_ECH */

  ಫಲಿತಾಂಶ = init_config_builder_keylog(ದತ್ತಾಂಶ, config_builder);
  ಆದರೆ(ಫಲಿತಾಂಶ != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "failed to build client config");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  ಆದರೆ(rr != RUSTLS_ಫಲಿತಾಂಶ_OK) {
    rustls_failf(ದತ್ತಾಂಶ, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_ಗಣ_userdata(rconn, backend);
  backend->conn = rconn;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_ಗಣ_negotiated_alpn(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_connection *rconn)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ connssl = cf->ctx;
  ಸ್ಥಿರಾಂಕ uint8_t *protocol = ಖಾಲಿ;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_ಗಣ_negotiated(cf, ದತ್ತಾಂಶ, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
ಸ್ಥಿರ CURLcode cr_connect(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ connssl = cf->ctx;
  ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ * ಸ್ಥಿರಾಂಕ backend =
    (ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_connection *rconn = ಖಾಲಿ;
  CURLcode tmperr = CURLE_OK;
  CURLcode ಫಲಿತಾಂಶ;
  bool ಬಯಸುತ್ತದೆ_read;
  bool ಬಯಸುತ್ತದೆ_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cr_connect, state=%d", (ಪೂರ್ಣಾಂಕ)connssl->state);
  *ಮುಗಿದ = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    ಆದರೆ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(ದತ್ತಾಂಶ, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "need HTTPS-RR for ECH, delaying connect");
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
#ಆದರೆ_ಅಂತ್ಯ /* USE_ECH */

  ಆದರೆ(!backend->conn) {
    ಫಲಿತಾಂಶ =
      cr_init_backend(cf, ದತ್ತಾಂಶ,
                      (ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cr_connect, init backend -> %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  ಗಾಗಿ(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    ಆದರೆ(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_ಗಣ_negotiated_alpn(cf, ದತ್ತಾಂಶ, rconn);
      tmperr = cr_send(cf, ದತ್ತಾಂಶ, ಖಾಲಿ, 0, &nwritten);
      ಆದರೆ(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(tmperr != CURLE_OK) {
        ಹಿಂತಿರುಗಿ tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
        ಸ್ಥಿರಾಂಕ uint16_t proto = rustls_connection_get_protocol_ಆವೃತ್ತಿ(rconn);
        ಸ್ಥಿರಾಂಕ rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ಸ್ಥಿರಾಂಕ rustls_str kex_ಗುಂಪು_name =
          rustls_connection_get_negotiated_ಕೀಲಿ_exchange_ಗುಂಪು_name(rconn);
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ver = "TLS version unknown";
        ಆದರೆ(proto == RUSTLS_TLS_ಆವೃತ್ತಿ_TLSV1_3)
          ver = "TLSv1.3";
        ಆದರೆ(proto == RUSTLS_TLS_ಆವೃತ್ತಿ_TLSV1_2)
          ver = "TLSv1.2";
#ಆದರೆ_ಅಂತ್ಯ
        infof(ದತ್ತಾಂಶ,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (ಪೂರ್ಣಾಂಕ)ciphersuite_name.len,
              ciphersuite_name.ದತ್ತಾಂಶ,
              (ಪೂರ್ಣಾಂಕ)kex_ಗುಂಪು_name.len,
              kex_ಗುಂಪು_name.ದತ್ತಾಂಶ);
      }
      ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo) {
        size_t ಅಂಕೆ_certs = 0;
        size_t i;
        ಆಗ(rustls_connection_get_peer_certificate(rconn, ಅಂಕೆ_certs)) {
          ಅಂಕೆ_certs++;
          ಆದರೆ(ಅಂಕೆ_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(ದತ್ತಾಂಶ, "%zu certificates is more than allowed (%d)",
                  ಅಂಕೆ_certs, MAX_ALLOWED_CERT_AMOUNT);
            ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
          }
        }
        ಫಲಿತಾಂಶ = Curl_ssl_init_certinfo(ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)ಅಂಕೆ_certs);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        ಗಾಗಿ(i = 0; i < ಅಂಕೆ_certs; i++) {
          ಸ್ಥಿರಾಂಕ rustls_certificate *cert;
          ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *der_ದತ್ತಾಂಶ;
          size_t der_len;
          rustls_ಫಲಿತಾಂಶ rresult = RUSTLS_ಫಲಿತಾಂಶ_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_ದತ್ತಾಂಶ, &der_len);
          ಆದರೆ(rresult != RUSTLS_ಫಲಿತಾಂಶ_OK) {
            ಅಕ್ಷರ errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, ಗಾತ್ರ(errorbuf), &errorlen);
            failf(ದತ್ತಾಂಶ,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (ಪೂರ್ಣಾಂಕ)errorlen, errorbuf);
            ಹಿಂತಿರುಗಿ map_error(rresult);
          }
          ಫಲಿತಾಂಶ = Curl_ಹೊರತೆಗೆ_certinfo(ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)i,
                                         der_ದತ್ತಾಂಶ, der_ದತ್ತಾಂಶ + der_len);
          ಆದರೆ(ಫಲಿತಾಂಶ)
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
      }

      connssl->state = ssl_connection_complete;
      *ಮುಗಿದ = TRUE;
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    ಬಯಸುತ್ತದೆ_read = rustls_connection_ಬಯಸುತ್ತದೆ_read(rconn);
    ಬಯಸುತ್ತದೆ_write = rustls_connection_ಬಯಸುತ್ತದೆ_write(rconn) ||
                  backend->plain_ಹೊರಹೋಗು_buffered;
    DEBUGASSERT(ಬಯಸುತ್ತದೆ_read || ಬಯಸುತ್ತದೆ_write);

    ಆದರೆ(ಬಯಸುತ್ತದೆ_write) {
      size_t nwritten;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, ದತ್ತಾಂಶ, ಖಾಲಿ, 0, &nwritten);
      ಆದರೆ(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ಹಿಂತಿರುಗಿ CURLE_OK;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(tmperr != CURLE_OK) {
        ಹಿಂತಿರುಗಿ tmperr;
      }
    }

    ಆದರೆ(ಬಯಸುತ್ತದೆ_read) {
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_ಇನ್ನಷ್ಟು(cf, ದತ್ತಾಂಶ, &tmperr);
      ಆದರೆ(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(ದತ್ತಾಂಶ, "TLS connect error: Connection closed abruptly");
        ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
      }
      ಆದರೆ(nread < 0) {
        ಆದರೆ(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          ಹಿಂತಿರುಗಿ CURLE_OK;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(tmperr == CURLE_RECV_ERROR) {
          ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಹಿಂತಿರುಗಿ tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

ಸ್ಥಿರ ಶೂನ್ಯ *cr_get_internals(ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl, CURLINFO info)
{
  ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  (ಶೂನ್ಯ)info;
  DEBUGASSERT(backend);
  ಹಿಂತಿರುಗಿ backend->conn;
}

ಸ್ಥಿರ CURLcode cr_shutdown(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ bool send_shutdown, bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  ಆದರೆ(!backend->conn || cf->shutdown) {
    *ಮುಗಿದ = TRUE;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *ಮುಗಿದ = FALSE;

  ಆದರೆ(!backend->ಕಳುಹಿಸಿದ_shutdown) {
    /* do this only once */
    backend->ಕಳುಹಿಸಿದ_shutdown = TRUE;
    ಆದರೆ(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  ಫಲಿತಾಂಶ = cr_send(cf, ದತ್ತಾಂಶ, ಖಾಲಿ, 0, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ಫಲಿತಾಂಶ = CURLE_OK;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    DEBUGASSERT(ಫಲಿತಾಂಶ);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown send failed: %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಗಾಗಿ(i = 0; i < 10; ++i) {
    ಅಕ್ಷರ buf[1024];
    ಫಲಿತಾಂಶ = cr_recv(cf, ದತ್ತಾಂಶ, buf, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf), &nread);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ಫಲಿತಾಂಶ = CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
    DEBUGASSERT(ಫಲಿತಾಂಶ);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown, error: %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nread == 0) {
    /* We got the close notify alert and are done. */
    *ಮುಗಿದ = TRUE;
  }

ಹೊರಹೋಗು:
  cf->shutdown = (ಫಲಿತಾಂಶ || *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_close(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(backend);
  ಆದರೆ(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = ಖಾಲಿ;
  }
  ಆದರೆ(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = ಖಾಲಿ;
  }
}

ಸ್ಥಿರ size_t cr_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ rustls_str ver = rustls_ಆವೃತ್ತಿ();
  ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, "%.*s", (ಪೂರ್ಣಾಂಕ)ver.len, ver.ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode cr_random(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *entropy,
                          size_t ಉದ್ದಳತೆ)
{
  rustls_ಫಲಿತಾಂಶ rresult = 0;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  rresult = rustls_default_crypto_provider_random(entropy, ಉದ್ದಳತೆ);
  ಹಿಂತಿರುಗಿ map_error(rresult);
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_cleanup(ಶೂನ್ಯ)
{
  Curl_tls_keylog_close();
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  ಗಾತ್ರ(ರಚನೆ rustls_ssl_backend_ದತ್ತಾಂಶ),

  ಖಾಲಿ,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_ಆವೃತ್ತಿ,                      /*ಆವೃತ್ತಿ*/
  cr_shutdown,                     /* shutdown */
  cr_ದತ್ತಾಂಶ_pending,                 /* data_pending */
  cr_random,                       /* random */
  ಖಾಲಿ,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  ಖಾಲಿ,                            /* close_all */
  ಖಾಲಿ,                            /* engines_list */
  ಖಾಲಿ,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  ಖಾಲಿ,                            /* get_channel_binding */
};

#ಆದರೆ_ಅಂತ್ಯ /* USE_RUSTLS */
