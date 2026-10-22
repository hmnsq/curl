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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_RUSTLS

#ଅନ୍ତର୍ଭାଵ <rustls.h>

#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strerr.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "vdns/httpsrr.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_int.h"
#ଅନ୍ତର୍ଭାଵ "vtls/rustls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/keylog.h"
#ଅନ୍ତର୍ଭାଵ "vtls/cipher_suite.h"
#ଅନ୍ତର୍ଭାଵ "vtls/x509asn1.h"
#ifdef USE_ECH
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#endif

#ଯଦି EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#ଅନ୍ୟଥା
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

ସଂରଚନା rustls_ssl_backend_data {
  ଅଚଳ ସଂରଚନା rustls_client_config *config;
  ସଂରଚନା rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
ସ୍ଥିର CURLcode map_error(ଅଚଳ rustls_result r)
{
  ଯଦି(rustls_result_is_cert_error(r)) {
    ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
  }
  ବାଛ(r) {
  ସ୍ଥିତି RUSTLS_RESULT_OK:
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି RUSTLS_RESULT_NULL_PARAMETER:
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_RECV_ERROR;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ rustls_failf(ସଂରଚନା Curl_easy *data, ଅଚଳ rustls_result rr,
                         ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ)
{
  ଅକ୍ଷର errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, ଆକାର(errorbuf), &errorlen);
  failf(data, "%s: %.*s", ସନ୍ଦେଶ, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)errorlen, errorbuf);
}

ସ୍ଥିର bool cr_data_pending(ସଂରଚନା Curl_cfilter *cf,
                            ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ ସଂରଚନା ssl_connect_data *ctx = cf->ctx;
  ସଂରଚନା rustls_ssl_backend_data *backend;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ସଂରଚନା rustls_ssl_backend_data *)ctx->backend;
  ଫେରନ୍ତୁ (bool)backend->data_in_pending;
}

ସଂରଚନା io_ctx {
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା Curl_easy *data;
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_cb(ଶୂନ୍ୟ *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  ଅଚଳ ସଂରଚନା io_ctx *io_ctx = userdata;
  ସଂରଚନା ssl_connect_data * ଅଚଳ connssl = io_ctx->cf->ctx;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
  size_t nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (ଅକ୍ଷର *)buf, len, &nread);
  ଯଦି(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    ଯଦି(result == CURLE_AGAIN)
      ret = EAGAIN;
    ଅନ୍ୟଥା
      ret = EINVAL;
  }
  ଅନ୍ୟଥା ଯଦି(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nread);
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା write_cb(ଶୂନ୍ୟ *userdata, ଅଚଳ uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  ଅଚଳ ସଂରଚନା io_ctx *io_ctx = userdata;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
  size_t nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  ଯଦି(result) {
    nwritten = 0;
    ଯଦି(result == CURLE_AGAIN)
      ret = EAGAIN;
    ଅନ୍ୟଥା
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nwritten);
  ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ssize_t tls_recv_more(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data, CURLcode *err)
{
  ଅଚଳ ସଂରଚନା ssl_connect_data * ଅଚଳ connssl = cf->ctx;
  ସଂରଚନା rustls_ssl_backend_data * ଅଚଳ backend =
    (ସଂରଚନା rustls_ssl_backend_data *)connssl->backend;
  ସଂରଚନା io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  ଯଦି(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    ଫେରନ୍ତୁ -1;
  }
  ଅନ୍ୟଥା ଯଦି(io_error) {
    ଅକ୍ଷର buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, ଆକାର(buffer)));
    *err = CURLE_RECV_ERROR;
    ଫେରନ୍ତୁ -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  ଯଦି(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    ଫେରନ୍ତୁ -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  ଫେରନ୍ତୁ (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
ସ୍ଥିର CURLcode cr_recv(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                        ଅକ୍ଷର *plainbuf, size_t plainlen, size_t *pnread)
{
  ଅଚଳ ସଂରଚନା ssl_connect_data * ଅଚଳ connssl = cf->ctx;
  ସଂରଚନା rustls_ssl_backend_data * ଅଚଳ backend =
    (ସଂରଚନା rustls_ssl_backend_data *)connssl->backend;
  ସଂରଚନା rustls_connection *rconn = ଖାଲି;
  CURLcode result = CURLE_OK;
  size_t n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  ଯେତେବେଳେ(*pnread < plainlen) {
    ଯଦି(!backend->data_in_pending) {
      ଯଦି(tls_recv_more(cf, data, &result) < 0) {
        ଯଦି(result != CURLE_AGAIN) {
          ଯାଅ out;
        }
        result = CURLE_OK;
        ଭାଙ୍ଗ;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    ଯଦି(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା ଯଦି(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା {
      *pnread += n;
    }
  }

  ଯଦି(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnread);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cr_flush_out(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା rustls_connection *rconn)
{
  ସଂରଚନା io_ctx io_ctx;
  rustls_io_result io_error;
  size_t tlswritten = 0;
  size_t tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  ଯେତେବେଳେ(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    ଯଦି(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      ଫେରନ୍ତୁ CURLE_AGAIN;
    }
    ଅନ୍ୟଥା ଯଦି(io_error) {
      ଅକ୍ଷର buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, ଆକାର(buffer)));
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    ଯଦି(tlswritten == 0) {
      failf(data, "EOF in swrite");
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  ଫେରନ୍ତୁ CURLE_OK;
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
ସ୍ଥିର CURLcode cr_send(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଶୂନ୍ୟ *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ଅଚଳ ସଂରଚନା ssl_connect_data * ଅଚଳ connssl = cf->ctx;
  ସଂରଚନା rustls_ssl_backend_data * ଅଚଳ backend =
    (ସଂରଚନା rustls_ssl_backend_data *)connssl->backend;
  ସଂରଚନା rustls_connection *rconn = ଖାଲି;
  size_t plainwritten = 0;
  ଅଚଳ unsigned ଅକ୍ଷର *buf = plainbuf;
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
  ଯଦି(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    ଯଦି(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    ଅନ୍ୟଥା
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  ଯଦି(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    ଯଦି(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      ଯାଅ out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      ଯଦି(*pnwritten) {
        result = CURLE_OK;
      }
    }
    ଯାଅ out;
  }
  ଅନ୍ୟଥା
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
  ଫେରନ୍ତୁ result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
ସ୍ଥିର uint32_t cr_verify_none(ଶୂନ୍ୟ *userdata,
                               ଅଚଳ rustls_verify_server_cert_params *params)
{
  (ଶୂନ୍ୟ)userdata;
  (ଶୂନ୍ୟ)params;
  ଫେରନ୍ତୁ RUSTLS_RESULT_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_file_into(ଅଚଳ ଅକ୍ଷର *filename, ସଂରଚନା dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  ଯଦି(!f) {
    ଫେରନ୍ତୁ 0;
  }

  ପାଇଁ(;;) {
    uint8_t buf[256];
    ଅଚଳ size_t rr = fread(buf, 1, ଆକାର(buf), f);
    ଯଦି((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      ଫେରନ୍ତୁ 0;
    }
    ଯଦି(rr < ଆକାର(buf))
      ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ curlx_fclose(f) == 0;
}

ସ୍ଥିର ଶୂନ୍ୟ
cr_get_selected_ciphers(ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଅକ୍ଷର *ciphers12,
                        ଅଚଳ ଅକ୍ଷର *ciphers13,
                        ଅଚଳ ସଂରଚନା rustls_supported_ciphersuite **selected,
                        size_t *selected_size)
{
  ଅଚଳ size_t supported_len = *selected_size;
  ଅଚଳ size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ଅଚଳ ସଂରଚନା rustls_supported_ciphersuite *entry = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *ciphers = ciphers12;
  size_t ଗଣନା_2 = 0, default13_ଗଣନା = 0, i, j;
  ଅଚଳ ଅକ୍ଷର *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  ଯଦି(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    ପାଇଁ(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      ଯଦି(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        ଜାରି;

      selected[ଗଣନା_2++] = entry;
    }

    default13_ଗଣନା = ଗଣନା_2;

    ଯଦି(!ciphers)
      ciphers = "";
  }
  ଅନ୍ୟଥା
    ciphers = ciphers13;

add_ciphers:
  ପାଇଁ(ptr = ciphers; ptr[0] != '\0' && ଗଣନା_2 < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    ଯଦି(id) {
      ପାଇଁ(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        ଯଦି(rustls_supported_ciphersuite_get_suite(entry) == id)
          ଭାଙ୍ଗ;
      }
      ଯଦି(i == supported_len)
        id = 0;
    }
    ଯଦି(!id) {
      ଯଦି(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - ptr), ptr);
      ଜାରି;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    ପାଇଁ(i = 0; i < ଗଣନା_2 && selected[i] != entry; i++)
      ;
    ଯଦି(i < ଗଣନା_2) {
      ଯଦି(i >= default13_ଗଣନା)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - ptr), ptr);
      ଜାରି;
    }

    selected[ଗଣନା_2++] = entry;
  }

  ଯଦି(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    ଯାଅ add_ciphers;
  }

  ଯଦି(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    ପାଇଁ(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      ଯଦି(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        ଜାରି;

      /* No duplicates allowed (so selected cannot overflow) */
      ପାଇଁ(i = 0; i < ଗଣନା_2 && selected[i] != entry; i++)
        ;
      ଯଦି(i < ଗଣନା_2)
        ଜାରି;

      selected[ଗଣନା_2++] = entry;
    }
  }

  *selected_size = ଗଣନା_2;
}

ସ୍ଥିର ଶୂନ୍ୟ cr_keylog_log_cb(ସଂରଚନା rustls_str label,
                             ଅଚଳ uint8_t *client_random,
                             size_t client_random_len, ଅଚଳ uint8_t *secret,
                             size_t secret_len)
{
  ଅକ୍ଷର clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, ଆକାର(clabel), "%.*s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

ସ୍ଥିର CURLcode
init_config_builder(ସଂରଚନା Curl_cfilter *cf,
                    ସଂରଚନା Curl_easy *data,
                    ଅଚଳ ସଂରଚନା ssl_filter_config *conn_config,
                    ସଂରଚନା rustls_client_config_builder **config_builder)
{
  ଅଚଳ ସଂରଚନା rustls_supported_ciphersuite **cipher_suites = ଖାଲି;
  ସଂରଚନା rustls_crypto_provider_builder *custom_provider_builder = ଖାଲି;
  ଅଚଳ ସଂରଚନା rustls_crypto_provider *custom_provider = ଖାଲି;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  ବାଛ(conn_config->version) {
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1:
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_0:
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_1:
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_2:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ cleanup;
  }

  ବାଛ(conn_config->version_max) {
  ସ୍ଥିତି CURL_SSLVERSION_MAX_DEFAULT:
  ସ୍ଥିତି CURL_SSLVERSION_MAX_NONE:
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_3:
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_2:
    ଯଦି(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      ଭାଙ୍ଗ;
    }
    FALLTHROUGH();
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_1:
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_0:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ cleanup;
  }

#ifdef USE_ECH
  ଯଦି(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(ଆକାର(*cipher_suites) * cipher_suites_len);
  ଯଦି(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  ଯଦି(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    ଯାଅ cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    ଯାଅ cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    ଯାଅ cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    ଯାଅ cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
    ଯାଅ cleanup;
  }

cleanup:
  ଯଦି(cipher_suites) {
    curlx_free(cipher_suites);
  }
  ଯଦି(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  ଯଦି(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ
init_config_builder_alpn(ସଂରଚନା Curl_easy *data,
                         ଅଚଳ ସଂରଚନା ssl_connect_data *connssl,
                         ସଂରଚନା rustls_client_config_builder *config_builder)
{
  ସଂରଚନା alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  ପାଇଁ(i = 0; i < connssl->alpn->ଗଣନା_2; ++i) {
    alpn[i].data = (ଅଚଳ uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->ଗଣନା_2);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

ସ୍ଥିର CURLcode init_config_builder_verifier_crl(
  ସଂରଚନା Curl_easy *data,
  ଅଚଳ ସଂରଚନା ssl_filter_config *conn_config,
  ସଂରଚନା rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  ଯଦି(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    ଯାଅ cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    ଯାଅ cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode
init_config_builder_verifier(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା rustls_client_config_builder *builder,
                             ଅଚଳ ସଂରଚନା ssl_filter_config *conn_config,
                             ଅଚଳ ସଂରଚନା curl_blob *ca_info_blob,
                             ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_cafile)
{
  ଅଚଳ ସଂରଚନା rustls_ମୂଳ_cert_store *roots = ଖାଲି;
  ସଂରଚନା rustls_ମୂଳ_cert_store_builder *roots_builder = ଖାଲି;
  ସଂରଚନା rustls_web_pki_server_cert_verifier_builder *verifier_builder = ଖାଲି;
  ସଂରଚନା rustls_server_cert_verifier *server_cert_verifier = ଖାଲି;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_ମୂଳ_cert_store_builder_new();
  ଯଦି(ca_info_blob) {
    rr = rustls_ମୂଳ_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    ଯଦି(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      ଯାଅ cleanup;
    }
  }
  ଅନ୍ୟଥା ଯଦି(ssl_cafile) {
    rr = rustls_ମୂଳ_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    ଯଦି(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      ଯାଅ cleanup;
    }
  }

  rr = rustls_ମୂଳ_cert_store_builder_build(roots_builder, &roots);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    ଯାଅ cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  ଯଦି(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ cleanup;
  }

  ଯଦି(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    ଯଦି(result) {
      ଯାଅ cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    ଯାଅ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  ଯଦି(roots_builder) {
    rustls_ମୂଳ_cert_store_builder_free(roots_builder);
  }
  ଯଦି(roots) {
    rustls_ମୂଳ_cert_store_free(roots);
  }
  ଯଦି(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  ଯଦି(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode init_config_builder_platform_verifier(
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା rustls_client_config_builder *builder)
{
  ସଂରଚନା rustls_server_cert_verifier *server_cert_verifier = ଖାଲି;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    ଯାଅ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  ଯଦି(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode
init_config_builder_keylog(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  ଯଦି(!Curl_tls_keylog_enabled()) {
    ଫେରନ୍ତୁ CURLE_OK;
  }

  rr = rustls_client_config_builder_set_ଚାବି_log(builder,
                                                cr_keylog_log_cb,
                                                ଖାଲି);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    ଫେରନ୍ତୁ map_error(rr);
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode
init_config_builder_client_auth(ସଂରଚନା Curl_easy *data,
                                ଅଚଳ ସଂରଚନା ssl_filter_config *conn_config,
                                ସଂରଚନା rustls_client_config_builder *builder)
{
  ସଂରଚନା dynbuf cert_contents;
  ସଂରଚନା dynbuf ଚାବି_contents;
  rustls_result rr;
  ଅଚଳ ସଂରଚନା rustls_certified_ଚାବି *certified_ଚାବି = ଖାଲି;
  CURLcode result = CURLE_OK;
  ଅଚଳ bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ଅଚଳ bool have_ଚାବି = conn_config->ଚାବି || conn_config->ଚାବି_blob;

  ଯଦି(have_cert && !have_ଚାବି) {
    failf(data, "rustls: must provide key with certificate");
    ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
  }
  ଅନ୍ୟଥା ଯଦି(!have_cert && have_ଚାବି) {
    failf(data, "rustls: must provide certificate with key");
    ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
  }

  ଯଦି(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
  }
  ଯଦି(conn_config->ଚାବି_type && !curl_strequal(conn_config->ଚାବି_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->ଚାବି_type);
    ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&ଚାବି_contents, DYN_KEYFILE_SIZE);

  ଯଦି(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    ଯଦି(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      ଯାଅ cleanup;
    }
  }
  ଅନ୍ୟଥା ଯଦି(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    ଯାଅ cleanup;
  }

  ଯଦି(conn_config->ଚାବି_blob) {
    result = curlx_dyn_addn(&ଚାବି_contents,
                            conn_config->ଚାବି_blob->data,
                            conn_config->ଚାବି_blob->len);
    ଯଦି(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      ଯାଅ cleanup;
    }
  }
  ଅନ୍ୟଥା ଯଦି(!read_file_into(conn_config->ଚାବି, &ଚାବି_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->ଚାବି);
    result = CURLE_SSL_CERTPROBLEM;
    ଯାଅ cleanup;
  }

  rr = rustls_certified_ଚାବି_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&ଚାବି_contents),
                                  curlx_dyn_len(&ଚାବି_contents),
                                  &certified_ଚାବି);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    ଯାଅ cleanup;
  }

  rr = rustls_certified_ଚାବି_ଚାବିଗୁଡ଼ିକ_match(certified_ଚାବି);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    ଯାଅ cleanup;
  }

  rr = rustls_client_config_builder_set_certified_ଚାବି(builder,
                                                      &certified_ଚାବି,
                                                      1);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    ଯାଅ cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&ଚାବି_contents);
  ଯଦି(certified_ଚାବି) {
    rustls_certified_ଚାବି_free(certified_ଚାବି);
  }
  ଫେରନ୍ତୁ result;
}

#ifdef USE_ECH

ସ୍ଥିର CURLcode
init_config_builder_ech(ସଂରଚନା Curl_easy *data,
                        ସଂରଚନା Curl_cfilter *cf,
                        ସଂରଚନା rustls_client_config_builder *builder)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ଅଚଳ rustls_hpke *hpke = rustls_supported_hpke();
  unsigned ଅକ୍ଷର *ech_config = ଖାଲି;
  size_t ech_config_len = 0;
  ସଂରଚନା Curl_dns_entry *dns = ଖାଲି;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  ଯଦି(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    ଯାଅ cleanup;
  }

  ଯଦି(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    ଯାଅ cleanup;
  }

  ଯଦି(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    ଯଦି(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ cleanup;
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(conn_config->ech && conn_config->ech_config) {
    ଅଚଳ ଅକ୍ଷର *b64 = conn_config->ech_config;
    size_t decode_result;
    ଯଦି(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    ଯଦି(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ cleanup;
    }
  }
  ଅନ୍ୟଥା {
    ଅଚଳ ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
    ଅଚଳ ସଂରଚନା Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    ଯଦି(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    ଯାଅ cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  ଯଦି(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  ଯଦି(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  ଫେରନ୍ତୁ result;
}
#endif /* USE_ECH */

ସ୍ଥିର CURLcode cr_init_backend(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା rustls_ssl_backend_data * ଅଚଳ backend)
{
  ଅଚଳ ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ଅଚଳ ସଂରଚନା ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା rustls_connection *rconn = ଖାଲି;
  ସଂରଚନା rustls_client_config_builder *config_builder = ଖାଲି;

  ଅଚଳ ସଂରଚନା curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ଖାଲି : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  ଯଦି(result != CURLE_OK) {
    ଫେରନ୍ତୁ result;
  }

  ଯଦି(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  ଯଦି(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  ଅନ୍ୟଥା ଯଦି(conn_config->native_ca_store) {
    ଯଦି(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    ଯଦି(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ଫେରନ୍ତୁ result;
    }
  }
  ଅନ୍ୟଥା ଯଦି(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    ଯଦି(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ଫେରନ୍ତୁ result;
    }
  }

  ଯଦି(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->ଚାବି || conn_config->ଚାବି_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    ଯଦି(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ଫେରନ୍ତୁ result;
    }
  }

#ifdef USE_ECH
  ଯଦି(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    ଯଦି((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      ଫେରନ୍ତୁ result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  ଯଦି(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    ଫେରନ୍ତୁ result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  ଯଦି(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = ଖାଲି;
    ଫେରନ୍ତୁ CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cr_set_negotiated_alpn(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data,
                                   ଅଚଳ ସଂରଚନା rustls_connection *rconn)
{
  ସଂରଚନା ssl_connect_data * ଅଚଳ connssl = cf->ctx;
  ଅଚଳ uint8_t *protocol = ଖାଲି;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
ସ୍ଥିର CURLcode cr_connect(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                           bool *done)
{
  ସଂରଚନା ssl_connect_data * ଅଚଳ connssl = cf->ctx;
  ଅଚଳ ସଂରଚନା rustls_ssl_backend_data * ଅଚଳ backend =
    (ସଂରଚନା rustls_ssl_backend_data *)connssl->backend;
  ଅଚଳ ସଂରଚନା rustls_connection *rconn = ଖାଲି;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    ଯଦି(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      ଫେରନ୍ତୁ CURLE_OK;
    }
#endif /* USE_ECH */

  ଯଦି(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (ସଂରଚନା rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  ପାଇଁ(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    ଯଦି(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, ଖାଲି, 0, &nwritten);
      ଯଦି(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଅନ୍ୟଥା ଯଦି(tmperr != CURLE_OK) {
        ଫେରନ୍ତୁ tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        ଅଚଳ uint16_t proto = rustls_connection_get_protocol_version(rconn);
        ଅଚଳ rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ଅଚଳ rustls_str kex_group_name =
          rustls_connection_get_negotiated_ଚାବି_exchange_group_name(rconn);
        ଅଚଳ ଅକ୍ଷର *ver = "TLS version unknown";
        ଯଦି(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        ଯଦି(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ciphersuite_name.len,
              ciphersuite_name.data,
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)kex_group_name.len,
              kex_group_name.data);
      }
      ଯଦି(data->set.ssl.certinfo) {
        size_t ଅଙ୍କ_certs = 0;
        size_t i;
        ଯେତେବେଳେ(rustls_connection_get_peer_certificate(rconn, ଅଙ୍କ_certs)) {
          ଅଙ୍କ_certs++;
          ଯଦି(ଅଙ୍କ_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  ଅଙ୍କ_certs, MAX_ALLOWED_CERT_AMOUNT);
            ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ_certs);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
        ପାଇଁ(i = 0; i < ଅଙ୍କ_certs; i++) {
          ଅଚଳ rustls_certificate *cert;
          ଅଚଳ unsigned ଅକ୍ଷର *der_data;
          size_t der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          ଯଦି(rresult != RUSTLS_RESULT_OK) {
            ଅକ୍ଷର errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, ଆକାର(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)errorlen, errorbuf);
            ଫେରନ୍ତୁ map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)i,
                                         der_data, der_data + der_len);
          ଯଦି(result)
            ଫେରନ୍ତୁ result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      ଫେରନ୍ତୁ CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    ଯଦି(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, ଖାଲି, 0, &nwritten);
      ଯଦି(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ଫେରନ୍ତୁ CURLE_OK;
      }
      ଅନ୍ୟଥା ଯଦି(tmperr != CURLE_OK) {
        ଫେରନ୍ତୁ tmperr;
      }
    }

    ଯଦି(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      ଯଦି(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
      }
      ଯଦି(nread < 0) {
        ଯଦି(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          ଫେରନ୍ତୁ CURLE_OK;
        }
        ଅନ୍ୟଥା ଯଦି(tmperr == CURLE_RECV_ERROR) {
          ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
        }
        ଅନ୍ୟଥା {
          ଫେରନ୍ତୁ tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

ସ୍ଥିର ଶୂନ୍ୟ *cr_get_internals(ସଂରଚନା ssl_connect_data *connssl, CURLINFO info)
{
  ସଂରଚନା rustls_ssl_backend_data *backend =
    (ସଂରଚନା rustls_ssl_backend_data *)connssl->backend;
  (ଶୂନ୍ୟ)info;
  DEBUGASSERT(backend);
  ଫେରନ୍ତୁ backend->conn;
}

ସ୍ଥିର CURLcode cr_shutdown(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                            ଅଚଳ bool send_shutdown, bool *done)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା rustls_ssl_backend_data *backend =
    (ସଂରଚନା rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  ଯଦି(!backend->conn || cf->shutdown) {
    *done = TRUE;
    ଯାଅ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  ଯଦି(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    ଯଦି(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, ଖାଲି, 0, &nwritten);
  ଯଦି(result) {
    ଯଦି(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      ଯାଅ out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯାଅ out;
  }

  ପାଇଁ(i = 0; i < 10; ++i) {
    ଅକ୍ଷର buf[1024];
    result = cr_recv(cf, data, buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(buf), &nread);
    ଯଦି(result)
      ଭାଙ୍ଗ;
  }

  ଯଦି(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  ଅନ୍ୟଥା ଯଦି(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  }
  ଅନ୍ୟଥା ଯଦି(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cr_close(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ଅଚଳ ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା rustls_ssl_backend_data *backend =
    (ସଂରଚନା rustls_ssl_backend_data *)connssl->backend;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(backend);
  ଯଦି(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = ଖାଲି;
  }
  ଯଦି(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = ଖାଲି;
  }
}

ସ୍ଥିର size_t cr_version(ଅକ୍ଷର *buffer, size_t size)
{
  ଅଚଳ ସଂରଚନା rustls_str ver = rustls_version();
  ଫେରନ୍ତୁ curl_msnprintf(buffer, size, "%.*s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ver.len, ver.data);
}

ସ୍ଥିର CURLcode cr_random(ସଂରଚନା Curl_easy *data, unsigned ଅକ୍ଷର *entropy,
                          size_t length)
{
  rustls_result rresult = 0;
  (ଶୂନ୍ୟ)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  ଫେରନ୍ତୁ map_error(rresult);
}

ସ୍ଥିର ଶୂନ୍ୟ cr_cleanup(ଶୂନ୍ୟ)
{
  Curl_tls_keylog_close();
}

ଅଚଳ ସଂରଚନା Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  ଆକାର(ସଂରଚନା rustls_ssl_backend_data),

  ଖାଲି,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
  cr_random,                       /* random */
  ଖାଲି,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  ଖାଲି,                            /* close_all */
  ଖାଲି,                            /* engines_list */
  ଖାଲି,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  ଖାଲି,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
