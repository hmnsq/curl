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

#ifdef USE_RUSTLS

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
#ifdef USE_ECH
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#endif

#ಯದಿ EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#ಅನ್ಯಥಾ
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

ಸಂರಚನಾ rustls_ssl_backend_data {
  ಧ್ರುವ ಸಂರಚನಾ rustls_client_config *config;
  ಸಂರಚನಾ rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
ಸ್ಥಿರ CURLcode map_error(ಧ್ರುವ rustls_result r)
{
  ಯದಿ(rustls_result_is_cert_error(r)) {
    ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
  }
  ವಿಕಲ್ಪ(r) {
  ಪಕ್ಷ RUSTLS_RESULT_OK:
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ RUSTLS_RESULT_NULL_PARAMETER:
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rustls_failf(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ rustls_result rr,
                         ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ)
{
  ವರ್ಣ errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, ಪರಿಮಾಣ(errorbuf), &errorlen);
  failf(data, "%s: %.*s", ಸಂದೇಶ, (ಪೂರ್ಣಾಙ್ಕ)errorlen, errorbuf);
}

ಸ್ಥಿರ bool cr_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ಸಂರಚನಾ ssl_connect_data *ctx = cf->ctx;
  ಸಂರಚನಾ rustls_ssl_backend_data *backend;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ಸಂರಚನಾ rustls_ssl_backend_data *)ctx->backend;
  ಪ್ರತ್ಯಾಗಮ (bool)backend->data_in_pending;
}

ಸಂರಚನಾ io_ctx {
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ Curl_easy *data;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_cb(ಶೂನ್ಯ *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  ಧ್ರುವ ಸಂರಚನಾ io_ctx *io_ctx = userdata;
  ಸಂರಚನಾ ssl_connect_data * ಧ್ರುವ connssl = io_ctx->cf->ctx;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ ret = 0;
  size_t nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (ವರ್ಣ *)buf, len, &nread);
  ಯದಿ(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    ಯದಿ(result == CURLE_AGAIN)
      ret = EAGAIN;
    ಅನ್ಯಥಾ
      ret = EINVAL;
  }
  ಅನ್ಯಥಾ ಯದಿ(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (ಪೂರ್ಣಾಙ್ಕ)result, nread);
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ write_cb(ಶೂನ್ಯ *userdata, ಧ್ರುವ uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  ಧ್ರುವ ಸಂರಚನಾ io_ctx *io_ctx = userdata;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ ret = 0;
  size_t nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  ಯದಿ(result) {
    nwritten = 0;
    ಯದಿ(result == CURLE_AGAIN)
      ret = EAGAIN;
    ಅನ್ಯಥಾ
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (ಪೂರ್ಣಾಙ್ಕ)result, nwritten);
  ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ssize_t tls_recv_more(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data, CURLcode *err)
{
  ಧ್ರುವ ಸಂರಚನಾ ssl_connect_data * ಧ್ರುವ connssl = cf->ctx;
  ಸಂರಚನಾ rustls_ssl_backend_data * ಧ್ರುವ backend =
    (ಸಂರಚನಾ rustls_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  ಯದಿ(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಅನ್ಯಥಾ ಯದಿ(io_error) {
    ವರ್ಣ buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, ಪರಿಮಾಣ(buffer)));
    *err = CURLE_RECV_ERROR;
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  ಯದಿ(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  ಪ್ರತ್ಯಾಗಮ (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
ಸ್ಥಿರ CURLcode cr_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                        ವರ್ಣ *plainbuf, size_t plainlen, size_t *pnread)
{
  ಧ್ರುವ ಸಂರಚನಾ ssl_connect_data * ಧ್ರುವ connssl = cf->ctx;
  ಸಂರಚನಾ rustls_ssl_backend_data * ಧ್ರುವ backend =
    (ಸಂರಚನಾ rustls_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ rustls_connection *rconn = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  size_t n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  ಯಾವತ್(*pnread < plainlen) {
    ಯದಿ(!backend->data_in_pending) {
      ಯದಿ(tls_recv_more(cf, data, &result) < 0) {
        ಯದಿ(result != CURLE_AGAIN) {
          ಗಚ್ಛ out;
        }
        result = CURLE_OK;
        ಭಙ್ಗ;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    ಯದಿ(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ ಯದಿ(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ {
      *pnread += n;
    }
  }

  ಯದಿ(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnread);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cr_flush_out(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ rustls_connection *rconn)
{
  ಸಂರಚನಾ io_ctx io_ctx;
  rustls_io_result io_error;
  size_t tlswritten = 0;
  size_t tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  ಯಾವತ್(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    ಯದಿ(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      ಪ್ರತ್ಯಾಗಮ CURLE_AGAIN;
    }
    ಅನ್ಯಥಾ ಯದಿ(io_error) {
      ವರ್ಣ buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, ಪರಿಮಾಣ(buffer)));
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
    ಯದಿ(tlswritten == 0) {
      failf(data, "EOF in swrite");
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
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
ಸ್ಥಿರ CURLcode cr_send(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                        ಧ್ರುವ ಶೂನ್ಯ *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ಧ್ರುವ ಸಂರಚನಾ ssl_connect_data * ಧ್ರುವ connssl = cf->ctx;
  ಸಂರಚನಾ rustls_ssl_backend_data * ಧ್ರುವ backend =
    (ಸಂರಚನಾ rustls_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ rustls_connection *rconn = ರಿಕ್ತ;
  size_t plainwritten = 0;
  ಧ್ರುವ unsigned ವರ್ಣ *buf = plainbuf;
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
  ಯದಿ(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (ಪೂರ್ಣಾಙ್ಕ)result);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    ಯದಿ(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    ಅನ್ಯಥಾ
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  ಯದಿ(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    ಯದಿ(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      ಗಚ್ಛ out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      ಯದಿ(*pnwritten) {
        result = CURLE_OK;
      }
    }
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
ಸ್ಥಿರ uint32_t cr_verify_none(ಶೂನ್ಯ *userdata,
                               ಧ್ರುವ rustls_verify_server_cert_params *params)
{
  (ಶೂನ್ಯ)userdata;
  (ಶೂನ್ಯ)params;
  ಪ್ರತ್ಯಾಗಮ RUSTLS_RESULT_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ read_file_into(ಧ್ರುವ ವರ್ಣ *filename, ಸಂರಚನಾ dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  ಯದಿ(!f) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಪ್ರತಿ(;;) {
    uint8_t buf[256];
    ಧ್ರುವ size_t rr = fread(buf, 1, ಪರಿಮಾಣ(buf), f);
    ಯದಿ((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(rr < ಪರಿಮಾಣ(buf))
      ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ curlx_fclose(f) == 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ
cr_get_selected_ciphers(ಸಂರಚನಾ Curl_easy *data,
                        ಧ್ರುವ ವರ್ಣ *ciphers12,
                        ಧ್ರುವ ವರ್ಣ *ciphers13,
                        ಧ್ರುವ ಸಂರಚನಾ rustls_supported_ciphersuite **selected,
                        size_t *selected_size)
{
  ಧ್ರುವ size_t supported_len = *selected_size;
  ಧ್ರುವ size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ಧ್ರುವ ಸಂರಚನಾ rustls_supported_ciphersuite *entry = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *ciphers = ciphers12;
  size_t ಎಣಿಕೆ = 0, default13_ಎಣಿಕೆ = 0, i, j;
  ಧ್ರುವ ವರ್ಣ *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  ಯದಿ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    ಪ್ರತಿ(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      ಯದಿ(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        ಅನುವರ್ತ;

      selected[ಎಣಿಕೆ++] = entry;
    }

    default13_ಎಣಿಕೆ = ಎಣಿಕೆ;

    ಯದಿ(!ciphers)
      ciphers = "";
  }
  ಅನ್ಯಥಾ
    ciphers = ciphers13;

add_ciphers:
  ಪ್ರತಿ(ptr = ciphers; ptr[0] != '\0' && ಎಣಿಕೆ < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    ಯದಿ(id) {
      ಪ್ರತಿ(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        ಯದಿ(rustls_supported_ciphersuite_get_suite(entry) == id)
          ಭಙ್ಗ;
      }
      ಯದಿ(i == supported_len)
        id = 0;
    }
    ಯದಿ(!id) {
      ಯದಿ(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (ಪೂರ್ಣಾಙ್ಕ)(end - ptr), ptr);
      ಅನುವರ್ತ;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    ಪ್ರತಿ(i = 0; i < ಎಣಿಕೆ && selected[i] != entry; i++)
      ;
    ಯದಿ(i < ಎಣಿಕೆ) {
      ಯದಿ(i >= default13_ಎಣಿಕೆ)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (ಪೂರ್ಣಾಙ್ಕ)(end - ptr), ptr);
      ಅನುವರ್ತ;
    }

    selected[ಎಣಿಕೆ++] = entry;
  }

  ಯದಿ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    ಗಚ್ಛ add_ciphers;
  }

  ಯದಿ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    ಪ್ರತಿ(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      ಯದಿ(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        ಅನುವರ್ತ;

      /* No duplicates allowed (so selected cannot overflow) */
      ಪ್ರತಿ(i = 0; i < ಎಣಿಕೆ && selected[i] != entry; i++)
        ;
      ಯದಿ(i < ಎಣಿಕೆ)
        ಅನುವರ್ತ;

      selected[ಎಣಿಕೆ++] = entry;
    }
  }

  *selected_size = ಎಣಿಕೆ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_keylog_log_cb(ಸಂರಚನಾ rustls_str label,
                             ಧ್ರುವ uint8_t *client_random,
                             size_t client_random_len, ಧ್ರುವ uint8_t *secret,
                             size_t secret_len)
{
  ವರ್ಣ clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, ಪರಿಮಾಣ(clabel), "%.*s", (ಪೂರ್ಣಾಙ್ಕ)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

ಸ್ಥಿರ CURLcode
init_config_builder(ಸಂರಚನಾ Curl_cfilter *cf,
                    ಸಂರಚನಾ Curl_easy *data,
                    ಧ್ರುವ ಸಂರಚನಾ ssl_filter_config *conn_config,
                    ಸಂರಚನಾ rustls_client_config_builder **config_builder)
{
  ಧ್ರುವ ಸಂರಚನಾ rustls_supported_ciphersuite **cipher_suites = ರಿಕ್ತ;
  ಸಂರಚನಾ rustls_crypto_provider_builder *custom_provider_builder = ರಿಕ್ತ;
  ಧ್ರುವ ಸಂರಚನಾ rustls_crypto_provider *custom_provider = ರಿಕ್ತ;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  ವಿಕಲ್ಪ(conn_config->version) {
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1:
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_0:
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_1:
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_2:
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ cleanup;
  }

  ವಿಕಲ್ಪ(conn_config->version_max) {
  ಪಕ್ಷ CURL_SSLVERSION_MAX_DEFAULT:
  ಪಕ್ಷ CURL_SSLVERSION_MAX_NONE:
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_3:
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_2:
    ಯದಿ(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      ಭಙ್ಗ;
    }
    FALLTHROUGH();
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_1:
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_0:
  ಸ್ವಭಾವ:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ cleanup;
  }

#ifdef USE_ECH
  ಯದಿ(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(ಪರಿಮಾಣ(*cipher_suites) * cipher_suites_len);
  ಯದಿ(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  ಯದಿ(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    ಗಚ್ಛ cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    ಗಚ್ಛ cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    ಗಚ್ಛ cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    ಗಚ್ಛ cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
    ಗಚ್ಛ cleanup;
  }

cleanup:
  ಯದಿ(cipher_suites) {
    curlx_free(cipher_suites);
  }
  ಯದಿ(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  ಯದಿ(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ
init_config_builder_alpn(ಸಂರಚನಾ Curl_easy *data,
                         ಧ್ರುವ ಸಂರಚನಾ ssl_connect_data *connssl,
                         ಸಂರಚನಾ rustls_client_config_builder *config_builder)
{
  ಸಂರಚನಾ alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  ಪ್ರತಿ(i = 0; i < connssl->alpn->ಎಣಿಕೆ; ++i) {
    alpn[i].data = (ಧ್ರುವ uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->ಎಣಿಕೆ);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

ಸ್ಥಿರ CURLcode init_config_builder_verifier_crl(
  ಸಂರಚನಾ Curl_easy *data,
  ಧ್ರುವ ಸಂರಚನಾ ssl_filter_config *conn_config,
  ಸಂರಚನಾ rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  ಯದಿ(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    ಗಚ್ಛ cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    ಗಚ್ಛ cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode
init_config_builder_verifier(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ rustls_client_config_builder *builder,
                             ಧ್ರುವ ಸಂರಚನಾ ssl_filter_config *conn_config,
                             ಧ್ರುವ ಸಂರಚನಾ curl_blob *ca_info_blob,
                             ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_cafile)
{
  ಧ್ರುವ ಸಂರಚನಾ rustls_ಬೇರು_cert_store *roots = ರಿಕ್ತ;
  ಸಂರಚನಾ rustls_ಬೇರು_cert_store_builder *roots_builder = ರಿಕ್ತ;
  ಸಂರಚನಾ rustls_web_pki_server_cert_verifier_builder *verifier_builder = ರಿಕ್ತ;
  ಸಂರಚನಾ rustls_server_cert_verifier *server_cert_verifier = ರಿಕ್ತ;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_ಬೇರು_cert_store_builder_new();
  ಯದಿ(ca_info_blob) {
    rr = rustls_ಬೇರು_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    ಯದಿ(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      ಗಚ್ಛ cleanup;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(ssl_cafile) {
    rr = rustls_ಬೇರು_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    ಯದಿ(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      ಗಚ್ಛ cleanup;
    }
  }

  rr = rustls_ಬೇರು_cert_store_builder_build(roots_builder, &roots);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    ಗಚ್ಛ cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  ಯದಿ(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ cleanup;
  }

  ಯದಿ(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    ಯದಿ(result) {
      ಗಚ್ಛ cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    ಗಚ್ಛ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  ಯದಿ(roots_builder) {
    rustls_ಬೇರು_cert_store_builder_free(roots_builder);
  }
  ಯದಿ(roots) {
    rustls_ಬೇರು_cert_store_free(roots);
  }
  ಯದಿ(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  ಯದಿ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode init_config_builder_platform_verifier(
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ rustls_client_config_builder *builder)
{
  ಸಂರಚನಾ rustls_server_cert_verifier *server_cert_verifier = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    ಗಚ್ಛ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  ಯದಿ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode
init_config_builder_keylog(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  ಯದಿ(!Curl_tls_keylog_enabled()) {
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  rr = rustls_client_config_builder_set_ಕೀಲಿ_log(builder,
                                                cr_keylog_log_cb,
                                                ರಿಕ್ತ);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    ಪ್ರತ್ಯಾಗಮ map_error(rr);
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode
init_config_builder_client_auth(ಸಂರಚನಾ Curl_easy *data,
                                ಧ್ರುವ ಸಂರಚನಾ ssl_filter_config *conn_config,
                                ಸಂರಚನಾ rustls_client_config_builder *builder)
{
  ಸಂರಚನಾ dynbuf cert_contents;
  ಸಂರಚನಾ dynbuf ಕೀಲಿ_contents;
  rustls_result rr;
  ಧ್ರುವ ಸಂರಚನಾ rustls_certified_ಕೀಲಿ *certified_ಕೀಲಿ = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  ಧ್ರುವ bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ಧ್ರುವ bool have_ಕೀಲಿ = conn_config->ಕೀಲಿ || conn_config->ಕೀಲಿ_blob;

  ಯದಿ(have_cert && !have_ಕೀಲಿ) {
    failf(data, "rustls: must provide key with certificate");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
  }
  ಅನ್ಯಥಾ ಯದಿ(!have_cert && have_ಕೀಲಿ) {
    failf(data, "rustls: must provide certificate with key");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
  }

  ಯದಿ(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
  }
  ಯದಿ(conn_config->ಕೀಲಿ_type && !curl_strequal(conn_config->ಕೀಲಿ_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->ಕೀಲಿ_type);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&ಕೀಲಿ_contents, DYN_KEYFILE_SIZE);

  ಯದಿ(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    ಯದಿ(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      ಗಚ್ಛ cleanup;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    ಗಚ್ಛ cleanup;
  }

  ಯದಿ(conn_config->ಕೀಲಿ_blob) {
    result = curlx_dyn_addn(&ಕೀಲಿ_contents,
                            conn_config->ಕೀಲಿ_blob->data,
                            conn_config->ಕೀಲಿ_blob->len);
    ಯದಿ(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      ಗಚ್ಛ cleanup;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(!read_file_into(conn_config->ಕೀಲಿ, &ಕೀಲಿ_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->ಕೀಲಿ);
    result = CURLE_SSL_CERTPROBLEM;
    ಗಚ್ಛ cleanup;
  }

  rr = rustls_certified_ಕೀಲಿ_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&ಕೀಲಿ_contents),
                                  curlx_dyn_len(&ಕೀಲಿ_contents),
                                  &certified_ಕೀಲಿ);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    ಗಚ್ಛ cleanup;
  }

  rr = rustls_certified_ಕೀಲಿ_ಕೀಲಿಗಳು_match(certified_ಕೀಲಿ);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    ಗಚ್ಛ cleanup;
  }

  rr = rustls_client_config_builder_set_certified_ಕೀಲಿ(builder,
                                                      &certified_ಕೀಲಿ,
                                                      1);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    ಗಚ್ಛ cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&ಕೀಲಿ_contents);
  ಯದಿ(certified_ಕೀಲಿ) {
    rustls_certified_ಕೀಲಿ_free(certified_ಕೀಲಿ);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifdef USE_ECH

ಸ್ಥಿರ CURLcode
init_config_builder_ech(ಸಂರಚನಾ Curl_easy *data,
                        ಸಂರಚನಾ Curl_cfilter *cf,
                        ಸಂರಚನಾ rustls_client_config_builder *builder)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಧ್ರುವ rustls_hpke *hpke = rustls_supported_hpke();
  unsigned ವರ್ಣ *ech_config = ರಿಕ್ತ;
  size_t ech_config_len = 0;
  ಸಂರಚನಾ Curl_dns_entry *dns = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  ಯದಿ(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    ಗಚ್ಛ cleanup;
  }

  ಯದಿ(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    ಗಚ್ಛ cleanup;
  }

  ಯದಿ(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    ಯದಿ(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ cleanup;
    }
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಯದಿ(conn_config->ech && conn_config->ech_config) {
    ಧ್ರುವ ವರ್ಣ *b64 = conn_config->ech_config;
    size_t decode_result;
    ಯದಿ(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    ಯದಿ(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ cleanup;
    }
  }
  ಅನ್ಯಥಾ {
    ಧ್ರುವ ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
    ಧ್ರುವ ಸಂರಚನಾ Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    ಯದಿ(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    ಗಚ್ಛ cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  ಯದಿ(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  ಯದಿ(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}
#endif /* USE_ECH */

ಸ್ಥಿರ CURLcode cr_init_backend(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ rustls_ssl_backend_data * ಧ್ರುವ backend)
{
  ಧ್ರುವ ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಧ್ರುವ ಸಂರಚನಾ ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ rustls_connection *rconn = ರಿಕ್ತ;
  ಸಂರಚನಾ rustls_client_config_builder *config_builder = ರಿಕ್ತ;

  ಧ್ರುವ ಸಂರಚನಾ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ರಿಕ್ತ : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  ಯದಿ(result != CURLE_OK) {
    ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  ಯದಿ(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  ಅನ್ಯಥಾ ಯದಿ(conn_config->native_ca_store) {
    ಯದಿ(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    ಯದಿ(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    ಯದಿ(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

  ಯದಿ(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->ಕೀಲಿ || conn_config->ಕೀಲಿ_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    ಯದಿ(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }

#ifdef USE_ECH
  ಯದಿ(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    ಯದಿ((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  ಯದಿ(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    ಪ್ರತ್ಯಾಗಮ result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  ಯದಿ(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_set_negotiated_alpn(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data,
                                   ಧ್ರುವ ಸಂರಚನಾ rustls_connection *rconn)
{
  ಸಂರಚನಾ ssl_connect_data * ಧ್ರುವ connssl = cf->ctx;
  ಧ್ರುವ uint8_t *protocol = ರಿಕ್ತ;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
ಸ್ಥಿರ CURLcode cr_connect(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                           bool *done)
{
  ಸಂರಚನಾ ssl_connect_data * ಧ್ರುವ connssl = cf->ctx;
  ಧ್ರುವ ಸಂರಚನಾ rustls_ssl_backend_data * ಧ್ರುವ backend =
    (ಸಂರಚನಾ rustls_ssl_backend_data *)connssl->backend;
  ಧ್ರುವ ಸಂರಚನಾ rustls_connection *rconn = ರಿಕ್ತ;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (ಪೂರ್ಣಾಙ್ಕ)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    ಯದಿ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
#endif /* USE_ECH */

  ಯದಿ(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (ಸಂರಚನಾ rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  ಪ್ರತಿ(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    ಯದಿ(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, ರಿಕ್ತ, 0, &nwritten);
      ಯದಿ(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಅನ್ಯಥಾ ಯದಿ(tmperr != CURLE_OK) {
        ಪ್ರತ್ಯಾಗಮ tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        ಧ್ರುವ uint16_t proto = rustls_connection_get_protocol_version(rconn);
        ಧ್ರುವ rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ಧ್ರುವ rustls_str kex_group_name =
          rustls_connection_get_negotiated_ಕೀಲಿ_exchange_group_name(rconn);
        ಧ್ರುವ ವರ್ಣ *ver = "TLS version unknown";
        ಯದಿ(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        ಯದಿ(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (ಪೂರ್ಣಾಙ್ಕ)ciphersuite_name.len,
              ciphersuite_name.data,
              (ಪೂರ್ಣಾಙ್ಕ)kex_group_name.len,
              kex_group_name.data);
      }
      ಯದಿ(data->set.ssl.certinfo) {
        size_t ಅಂಕೆ_certs = 0;
        size_t i;
        ಯಾವತ್(rustls_connection_get_peer_certificate(rconn, ಅಂಕೆ_certs)) {
          ಅಂಕೆ_certs++;
          ಯದಿ(ಅಂಕೆ_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  ಅಂಕೆ_certs, MAX_ALLOWED_CERT_AMOUNT);
            ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ_certs);
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
        ಪ್ರತಿ(i = 0; i < ಅಂಕೆ_certs; i++) {
          ಧ್ರುವ rustls_certificate *cert;
          ಧ್ರುವ unsigned ವರ್ಣ *der_data;
          size_t der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          ಯದಿ(rresult != RUSTLS_RESULT_OK) {
            ವರ್ಣ errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, ಪರಿಮಾಣ(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (ಪೂರ್ಣಾಙ್ಕ)errorlen, errorbuf);
            ಪ್ರತ್ಯಾಗಮ map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (ಪೂರ್ಣಾಙ್ಕ)i,
                                         der_data, der_data + der_len);
          ಯದಿ(result)
            ಪ್ರತ್ಯಾಗಮ result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    ಯದಿ(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, ರಿಕ್ತ, 0, &nwritten);
      ಯದಿ(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಅನ್ಯಥಾ ಯದಿ(tmperr != CURLE_OK) {
        ಪ್ರತ್ಯಾಗಮ tmperr;
      }
    }

    ಯದಿ(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      ಯದಿ(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
      }
      ಯದಿ(nread < 0) {
        ಯದಿ(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
        ಅನ್ಯಥಾ ಯದಿ(tmperr == CURLE_RECV_ERROR) {
          ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
        }
        ಅನ್ಯಥಾ {
          ಪ್ರತ್ಯಾಗಮ tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

ಸ್ಥಿರ ಶೂನ್ಯ *cr_get_internals(ಸಂರಚನಾ ssl_connect_data *connssl, CURLINFO info)
{
  ಸಂರಚನಾ rustls_ssl_backend_data *backend =
    (ಸಂರಚನಾ rustls_ssl_backend_data *)connssl->backend;
  (ಶೂನ್ಯ)info;
  DEBUGASSERT(backend);
  ಪ್ರತ್ಯಾಗಮ backend->conn;
}

ಸ್ಥಿರ CURLcode cr_shutdown(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ bool send_shutdown, bool *done)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ rustls_ssl_backend_data *backend =
    (ಸಂರಚನಾ rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  ಯದಿ(!backend->conn || cf->shutdown) {
    *done = TRUE;
    ಗಚ್ಛ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  ಯದಿ(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    ಯದಿ(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, ರಿಕ್ತ, 0, &nwritten);
  ಯದಿ(result) {
    ಯದಿ(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      ಗಚ್ಛ out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    ಗಚ್ಛ out;
  }

  ಪ್ರತಿ(i = 0; i < 10; ++i) {
    ವರ್ಣ buf[1024];
    result = cr_recv(cf, data, buf, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf), &nread);
    ಯದಿ(result)
      ಭಙ್ಗ;
  }

  ಯದಿ(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (ಪೂರ್ಣಾಙ್ಕ)result);
  }
  ಅನ್ಯಥಾ ಯದಿ(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_close(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಧ್ರುವ ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ rustls_ssl_backend_data *backend =
    (ಸಂರಚನಾ rustls_ssl_backend_data *)connssl->backend;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(backend);
  ಯದಿ(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = ರಿಕ್ತ;
  }
  ಯದಿ(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = ರಿಕ್ತ;
  }
}

ಸ್ಥಿರ size_t cr_version(ವರ್ಣ *buffer, size_t size)
{
  ಧ್ರುವ ಸಂರಚನಾ rustls_str ver = rustls_version();
  ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, size, "%.*s", (ಪೂರ್ಣಾಙ್ಕ)ver.len, ver.data);
}

ಸ್ಥಿರ CURLcode cr_random(ಸಂರಚನಾ Curl_easy *data, unsigned ವರ್ಣ *entropy,
                          size_t length)
{
  rustls_result rresult = 0;
  (ಶೂನ್ಯ)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  ಪ್ರತ್ಯಾಗಮ map_error(rresult);
}

ಸ್ಥಿರ ಶೂನ್ಯ cr_cleanup(ಶೂನ್ಯ)
{
  Curl_tls_keylog_close();
}

ಧ್ರುವ ಸಂರಚನಾ Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  ಪರಿಮಾಣ(ಸಂರಚನಾ rustls_ssl_backend_data),

  ರಿಕ್ತ,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
  cr_random,                       /* random */
  ರಿಕ್ತ,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  ರಿಕ್ತ,                            /* close_all */
  ರಿಕ್ತ,                            /* engines_list */
  ರಿಕ್ತ,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  ರಿಕ್ತ,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
