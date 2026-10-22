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

কাঠামো rustls_ssl_backend_তথ্য {
  ধ্রুবক কাঠামো rustls_client_config *config;
  কাঠামো rustls_connection *conn;
  size_t plain_out_buffered;
  BIT(তথ্য_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
স্থির CURLcode map_error(ধ্রুবক rustls_ফলাফল r)
{
  যদি(rustls_ফলাফল_is_cert_error(r)) {
    ফেরত CURLE_PEER_FAILED_VERIFICATION;
  }
  নির্বাচন(r) {
  ক্ষেত্র RUSTLS_ফলাফল_OK:
    ফেরত CURLE_OK;
  ক্ষেত্র RUSTLS_ফলাফল_NULL_PARAMETER:
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  পূর্বনির্ধারিত:
    ফেরত CURLE_RECV_ERROR;
  }
}

স্থির শূন্য rustls_failf(কাঠামো Curl_easy *তথ্য, ধ্রুবক rustls_ফলাফল rr,
                         ধ্রুবক অক্ষর *বার্তা)
{
  অক্ষর errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, আকার(errorbuf), &errorlen);
  failf(তথ্য, "%s: %.*s", বার্তা, (পূর্ণসংখ্যা)errorlen, errorbuf);
}

স্থির bool cr_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                            ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক কাঠামো ssl_connect_তথ্য *ctx = cf->ctx;
  কাঠামো rustls_ssl_backend_তথ্য *backend;

  (শূন্য)তথ্য;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (কাঠামো rustls_ssl_backend_তথ্য *)ctx->backend;
  ফেরত (bool)backend->তথ্য_in_pending;
}

কাঠামো io_ctx {
  কাঠামো Curl_cfilter *cf;
  কাঠামো Curl_easy *তথ্য;
};

স্থির পূর্ণসংখ্যা read_cb(শূন্য *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  ধ্রুবক কাঠামো io_ctx *io_ctx = userdata;
  কাঠামো ssl_connect_তথ্য * ধ্রুবক connssl = io_ctx->cf->ctx;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা ret = 0;
  size_t nread;

  ফলাফল = Curl_conn_cf_recv(io_ctx->cf->পরবর্তী, io_ctx->তথ্য,
                             (অক্ষর *)buf, len, &nread);
  যদি(ফলাফল) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    যদি(ফলাফল == CURLE_AGAIN)
      ret = EAGAIN;
    অন্যথা
      ret = EINVAL;
  }
  অন্যথা যদি(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->তথ্য, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (পূর্ণসংখ্যা)ফলাফল, nread);
  ফেরত ret;
}

স্থির পূর্ণসংখ্যা write_cb(শূন্য *userdata, ধ্রুবক uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  ধ্রুবক কাঠামো io_ctx *io_ctx = userdata;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা ret = 0;
  size_t nwritten;

  ফলাফল = Curl_conn_cf_send(io_ctx->cf->পরবর্তী, io_ctx->তথ্য,
                             buf, len, FALSE, &nwritten);
  যদি(ফলাফল) {
    nwritten = 0;
    যদি(ফলাফল == CURLE_AGAIN)
      ret = EAGAIN;
    অন্যথা
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->তথ্য, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (পূর্ণসংখ্যা)ফলাফল, nwritten);
  ফেরত ret;
}

স্থির ssize_t tls_recv_more(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য, CURLcode *err)
{
  ধ্রুবক কাঠামো ssl_connect_তথ্য * ধ্রুবক connssl = cf->ctx;
  কাঠামো rustls_ssl_backend_তথ্য * ধ্রুবক backend =
    (কাঠামো rustls_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো io_ctx io_ctx;
  size_t tls_bytes_read = 0;
  rustls_io_ফলাফল io_error;
  rustls_ফলাফল rresult = 0;

  io_ctx.cf = cf;
  io_ctx.তথ্য = তথ্য;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  যদি(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    ফেরত -1;
  }
  অন্যথা যদি(io_error) {
    অক্ষর বাফার[STRERROR_LEN];
    failf(তথ্য, "reading from socket: %s",
          curlx_strerror(io_error, বাফার, আকার(বাফার)));
    *err = CURLE_RECV_ERROR;
    ফেরত -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  যদি(rresult != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    ফেরত -1;
  }

  backend->তথ্য_in_pending = TRUE;
  *err = CURLE_OK;
  ফেরত (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
স্থির CURLcode cr_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                        অক্ষর *plainbuf, size_t plainlen, size_t *pnread)
{
  ধ্রুবক কাঠামো ssl_connect_তথ্য * ধ্রুবক connssl = cf->ctx;
  কাঠামো rustls_ssl_backend_তথ্য * ধ্রুবক backend =
    (কাঠামো rustls_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো rustls_connection *rconn = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  size_t n = 0;
  rustls_ফলাফল rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  যখন(*pnread < plainlen) {
    যদি(!backend->তথ্য_in_pending) {
      যদি(tls_recv_more(cf, তথ্য, &ফলাফল) < 0) {
        যদি(ফলাফল != CURLE_AGAIN) {
          যাও out;
        }
        ফলাফল = CURLE_OK;
        বিরতি;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    যদি(rresult == RUSTLS_ফলাফল_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->তথ্য_in_pending = FALSE;
      বিরতি;
    }
    অন্যথা যদি(rresult == RUSTLS_ফলাফল_UNEXPECTED_EOF) {
      failf(তথ্য, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      ফলাফল = CURLE_RECV_ERROR;
      যাও out;
    }
    অন্যথা যদি(rresult != RUSTLS_ফলাফল_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(তথ্য, rresult, "rustls_connection_read");
      ফলাফল = CURLE_RECV_ERROR;
      যাও out;
    }
    অন্যথা যদি(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      বিরতি;
    }
    অন্যথা {
      *pnread += n;
    }
  }

  যদি(!eof && !*pnread) {
    ফলাফল = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(তথ্য, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (পূর্ণসংখ্যা)ফলাফল, *pnread);
  ফেরত ফলাফল;
}

স্থির CURLcode cr_flush_out(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                             কাঠামো rustls_connection *rconn)
{
  কাঠামো io_ctx io_ctx;
  rustls_io_ফলাফল io_error;
  size_t tlswritten = 0;
  size_t tlswritten_যোগ = 0;

  io_ctx.cf = cf;
  io_ctx.তথ্য = তথ্য;

  যখন(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    যদি(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(তথ্য, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_যোগ);
      ফেরত CURLE_AGAIN;
    }
    অন্যথা যদি(io_error) {
      অক্ষর বাফার[STRERROR_LEN];
      failf(তথ্য, "writing to socket: %s",
            curlx_strerror(io_error, বাফার, আকার(বাফার)));
      ফেরত CURLE_SEND_ERROR;
    }
    যদি(tlswritten == 0) {
      failf(তথ্য, "EOF in swrite");
      ফেরত CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(তথ্য, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_যোগ += tlswritten;
  }
  ফেরত CURLE_OK;
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
স্থির CURLcode cr_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক শূন্য *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ধ্রুবক কাঠামো ssl_connect_তথ্য * ধ্রুবক connssl = cf->ctx;
  কাঠামো rustls_ssl_backend_তথ্য * ধ্রুবক backend =
    (কাঠামো rustls_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো rustls_connection *rconn = ফাঁকা;
  size_t plainwritten = 0;
  ধ্রুবক unsigned অক্ষর *buf = plainbuf;
  CURLcode ফলাফল = CURLE_OK;
  size_t blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(তথ্য, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  যদি(backend->plain_out_buffered) {
    ফলাফল = cr_flush_out(cf, তথ্য, rconn);
    CURL_TRC_CF(তথ্য, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (পূর্ণসংখ্যা)ফলাফল);
    যদি(ফলাফল)
      ফেরত ফলাফল;
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
    rustls_ফলাফল rresult;
    CURL_TRC_CF(তথ্য, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    যদি(rresult != RUSTLS_ফলাফল_OK) {
      rustls_failf(তথ্য, rresult, "rustls_connection_write");
      ফলাফল = CURLE_WRITE_ERROR;
      যাও out;
    }
    অন্যথা যদি(plainwritten == 0) {
      failf(তথ্য, "rustls_connection_write: EOF");
      ফলাফল = CURLE_WRITE_ERROR;
      যাও out;
    }
  }

  ফলাফল = cr_flush_out(cf, তথ্য, rconn);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      যদি(*pnwritten) {
        ফলাফল = CURLE_OK;
      }
    }
    যাও out;
  }
  অন্যথা
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(তথ্য, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
  ফেরত ফলাফল;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
স্থির uint32_t cr_verify_none(শূন্য *userdata,
                               ধ্রুবক rustls_verify_server_cert_params *params)
{
  (শূন্য)userdata;
  (শূন্য)params;
  ফেরত RUSTLS_ফলাফল_OK;
}

স্থির পূর্ণসংখ্যা read_file_into(ধ্রুবক অক্ষর *filename, কাঠামো dynbuf *out)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  যদি(!f) {
    ফেরত 0;
  }

  জন্য(;;) {
    uint8_t buf[256];
    ধ্রুবক size_t rr = fread(buf, 1, আকার(buf), f);
    যদি((!rr && !feof(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      ফেরত 0;
    }
    যদি(rr < আকার(buf))
      বিরতি;
  }

  ফেরত curlx_fclose(f) == 0;
}

স্থির শূন্য
cr_get_selected_ciphers(কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক অক্ষর *ciphers12,
                        ধ্রুবক অক্ষর *ciphers13,
                        ধ্রুবক কাঠামো rustls_supported_ciphersuite **selected,
                        size_t *selected_আকার)
{
  ধ্রুবক size_t supported_len = *selected_আকার;
  ধ্রুবক size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ধ্রুবক কাঠামো rustls_supported_ciphersuite *entry = ফাঁকা;
  ধ্রুবক অক্ষর *ciphers = ciphers12;
  size_t গুনতি = 0, default13_গুনতি = 0, i, j;
  ধ্রুবক অক্ষর *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  যদি(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    জন্য(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      যদি(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        চালু;

      selected[গুনতি++] = entry;
    }

    default13_গুনতি = গুনতি;

    যদি(!ciphers)
      ciphers = "";
  }
  অন্যথা
    ciphers = ciphers13;

add_ciphers:
  জন্য(ptr = ciphers; ptr[0] != '\0' && গুনতি < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    যদি(id) {
      জন্য(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        যদি(rustls_supported_ciphersuite_get_suite(entry) == id)
          বিরতি;
      }
      যদি(i == supported_len)
        id = 0;
    }
    যদি(!id) {
      যদি(ptr[0] != '\0')
        infof(তথ্য, "rustls: unknown cipher in list: \"%.*s\"",
              (পূর্ণসংখ্যা)(end - ptr), ptr);
      চালু;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    জন্য(i = 0; i < গুনতি && selected[i] != entry; i++)
      ;
    যদি(i < গুনতি) {
      যদি(i >= default13_গুনতি)
        infof(তথ্য, "rustls: duplicate cipher in list: \"%.*s\"",
              (পূর্ণসংখ্যা)(end - ptr), ptr);
      চালু;
    }

    selected[গুনতি++] = entry;
  }

  যদি(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    যাও add_ciphers;
  }

  যদি(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    জন্য(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      যদি(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        চালু;

      /* No duplicates allowed (so selected cannot overflow) */
      জন্য(i = 0; i < গুনতি && selected[i] != entry; i++)
        ;
      যদি(i < গুনতি)
        চালু;

      selected[গুনতি++] = entry;
    }
  }

  *selected_আকার = গুনতি;
}

স্থির শূন্য cr_keylog_log_cb(কাঠামো rustls_str label,
                             ধ্রুবক uint8_t *client_random,
                             size_t client_random_len, ধ্রুবক uint8_t *secret,
                             size_t secret_len)
{
  অক্ষর clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_আকার);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, আকার(clabel), "%.*s", (পূর্ণসংখ্যা)label.len, label.তথ্য);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

স্থির CURLcode
init_config_builder(কাঠামো Curl_cfilter *cf,
                    কাঠামো Curl_easy *তথ্য,
                    ধ্রুবক কাঠামো ssl_filter_config *conn_config,
                    কাঠামো rustls_client_config_builder **config_builder)
{
  ধ্রুবক কাঠামো rustls_supported_ciphersuite **cipher_suites = ফাঁকা;
  কাঠামো rustls_crypto_provider_builder *custom_provider_builder = ফাঁকা;
  ধ্রুবক কাঠামো rustls_crypto_provider *custom_provider = ফাঁকা;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode ফলাফল = CURLE_OK;
  rustls_ফলাফল rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  নির্বাচন(conn_config->version) {
  ক্ষেত্র CURL_SSLVERSION_TLSv1:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_0:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_1:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_2:
    বিরতি;
  ক্ষেত্র CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    বিরতি;
  পূর্বনির্ধারিত:
    failf(তথ্য, "rustls: unsupported minimum TLS version value");
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও cleanup;
  }

  নির্বাচন(conn_config->version_max) {
  ক্ষেত্র CURL_SSLVERSION_MAX_DEFAULT:
  ক্ষেত্র CURL_SSLVERSION_MAX_NONE:
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_3:
    বিরতি;
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_2:
    যদি(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      বিরতি;
    }
    FALLTHROUGH();
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_1:
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_0:
  পূর্বনির্ধারিত:
    failf(তথ্য, "rustls: unsupported maximum TLS version value");
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও cleanup;
  }

#ifdef USE_ECH
  যদি(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(তথ্য, "rustls: ECH enabled, forcing TLSv1.3");
  }
#অন্যথা
  (শূন্য)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(আকার(*cipher_suites) * cipher_suites_len);
  যদি(!cipher_suites) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও cleanup;
  }

  cr_get_selected_ciphers(তথ্য,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  যদি(cipher_suites_len == 0) {
    failf(তথ্য, "rustls: no supported cipher in list");
    ফলাফল = CURLE_SSL_CIPHER;
    যাও cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr,
                 "failed to create crypto provider builder from default");
    ফলাফল = CURLE_SSL_CIPHER;
    যাও cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr,
                 "failed to set ciphersuites for crypto provider builder");
    ফলাফল = CURLE_SSL_CIPHER;
    যাও cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "failed to build custom crypto provider");
    ফলাফল = CURLE_SSL_CIPHER;
    যাও cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "failed to create client config builder");
    ফলাফল = CURLE_SSL_CIPHER;
    যাও cleanup;
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
  ফেরত ফলাফল;
}

স্থির শূন্য
init_config_builder_alpn(কাঠামো Curl_easy *তথ্য,
                         ধ্রুবক কাঠামো ssl_connect_তথ্য *connssl,
                         কাঠামো rustls_client_config_builder *config_builder)
{
  কাঠামো alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  size_t i;

  জন্য(i = 0; i < connssl->alpn->গুনতি; ++i) {
    alpn[i].তথ্য = (ধ্রুবক uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = strlen(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->গুনতি);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(তথ্য, VTLS_INFOF_ALPN_OFFER_1STR, proto.তথ্য);
}

স্থির CURLcode init_config_builder_verifier_crl(
  কাঠামো Curl_easy *তথ্য,
  ধ্রুবক কাঠামো ssl_filter_config *conn_config,
  কাঠামো rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো dynbuf crl_contents;
  rustls_ফলাফল rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_আকার);
  যদি(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(তথ্য, "rustls: failed to read revocation list file");
    ফলাফল = CURLE_SSL_CRL_BADFILE;
    যাও cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "failed to parse revocation list");
    ফলাফল = CURLE_SSL_CRL_BADFILE;
    যাও cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  ফেরত ফলাফল;
}

স্থির CURLcode
init_config_builder_verifier(কাঠামো Curl_easy *তথ্য,
                             কাঠামো rustls_client_config_builder *builder,
                             ধ্রুবক কাঠামো ssl_filter_config *conn_config,
                             ধ্রুবক কাঠামো curl_blob *ca_info_blob,
                             ধ্রুবক অক্ষর * ধ্রুবক ssl_cafile)
{
  ধ্রুবক কাঠামো rustls_মূল_cert_store *roots = ফাঁকা;
  কাঠামো rustls_মূল_cert_store_builder *roots_builder = ফাঁকা;
  কাঠামো rustls_web_pki_server_cert_verifier_builder *verifier_builder = ফাঁকা;
  কাঠামো rustls_server_cert_verifier *server_cert_verifier = ফাঁকা;
  rustls_ফলাফল rr = RUSTLS_ফলাফল_OK;
  CURLcode ফলাফল = CURLE_OK;

  roots_builder = rustls_মূল_cert_store_builder_new();
  যদি(ca_info_blob) {
    rr = rustls_মূল_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->তথ্য,
                                                ca_info_blob->len,
                                                1);
    যদি(rr != RUSTLS_ফলাফল_OK) {
      rustls_failf(তথ্য, rr, "failed to parse trusted certificates from blob");
      ফলাফল = CURLE_SSL_CACERT_BADFILE;
      যাও cleanup;
    }
  }
  অন্যথা যদি(ssl_cafile) {
    rr = rustls_মূল_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    যদি(rr != RUSTLS_ফলাফল_OK) {
      rustls_failf(তথ্য, rr, "failed to load trusted certificates");
      ফলাফল = CURLE_SSL_CACERT_BADFILE;
      যাও cleanup;
    }
  }

  rr = rustls_মূল_cert_store_builder_build(roots_builder, &roots);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "failed to build trusted root certificate store");
    ফলাফল = CURLE_SSL_CACERT_BADFILE;
    যাও cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  যদি(!verifier_builder) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও cleanup;
  }

  যদি(conn_config->CRLfile) {
    ফলাফল = init_config_builder_verifier_crl(তথ্য,
                                              conn_config,
                                              verifier_builder);
    যদি(ফলাফল) {
      যাও cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "failed to build certificate verifier");
    ফলাফল = CURLE_SSL_CACERT_BADFILE;
    যাও cleanup;
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

  ফেরত ফলাফল;
}

স্থির CURLcode init_config_builder_platform_verifier(
  কাঠামো Curl_easy *তথ্য,
  কাঠামো rustls_client_config_builder *builder)
{
  কাঠামো rustls_server_cert_verifier *server_cert_verifier = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  rustls_ফলাফল rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "failed to create platform certificate verifier");
    ফলাফল = CURLE_SSL_CACERT_BADFILE;
    যাও cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  যদি(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode
init_config_builder_keylog(কাঠামো Curl_easy *তথ্য,
                           কাঠামো rustls_client_config_builder *builder)
{
  rustls_ফলাফল rr;

  Curl_tls_keylog_open();
  যদি(!Curl_tls_keylog_enabled()) {
    ফেরত CURLE_OK;
  }

  rr = rustls_client_config_builder_set_চাবি_log(builder,
                                                cr_keylog_log_cb,
                                                ফাঁকা);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    ফেরত map_error(rr);
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode
init_config_builder_client_auth(কাঠামো Curl_easy *তথ্য,
                                ধ্রুবক কাঠামো ssl_filter_config *conn_config,
                                কাঠামো rustls_client_config_builder *builder)
{
  কাঠামো dynbuf cert_contents;
  কাঠামো dynbuf চাবি_contents;
  rustls_ফলাফল rr;
  ধ্রুবক কাঠামো rustls_certified_চাবি *certified_চাবি = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  ধ্রুবক bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ধ্রুবক bool have_চাবি = conn_config->চাবি || conn_config->চাবি_blob;

  যদি(have_cert && !have_চাবি) {
    failf(তথ্য, "rustls: must provide key with certificate");
    ফেরত CURLE_SSL_CERTPROBLEM;
  }
  অন্যথা যদি(!have_cert && have_চাবি) {
    failf(তথ্য, "rustls: must provide certificate with key");
    ফেরত CURLE_SSL_CERTPROBLEM;
  }

  যদি(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(তথ্য, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    ফেরত CURLE_SSL_CERTPROBLEM;
  }
  যদি(conn_config->চাবি_type && !curl_strequal(conn_config->চাবি_type, "PEM")) {
    failf(তথ্য, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->চাবি_type);
    ফেরত CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_আকার);
  curlx_dyn_init(&চাবি_contents, DYN_KEYFILE_আকার);

  যদি(conn_config->cert_blob) {
    ফলাফল = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->তথ্য,
                            conn_config->cert_blob->len);
    যদি(ফলাফল) {
      failf(তথ্য, "rustls: failed to copy client certificate blob (%s)",
            ফলাফল == CURLE_TOO_LARGE ? "too large" : "out of memory");
      যাও cleanup;
    }
  }
  অন্যথা যদি(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(তথ্য, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    ফলাফল = CURLE_SSL_CERTPROBLEM;
    যাও cleanup;
  }

  যদি(conn_config->চাবি_blob) {
    ফলাফল = curlx_dyn_addn(&চাবি_contents,
                            conn_config->চাবি_blob->তথ্য,
                            conn_config->চাবি_blob->len);
    যদি(ফলাফল) {
      failf(তথ্য, "rustls: failed to copy key blob (%s)",
            ফলাফল == CURLE_TOO_LARGE ? "too large" : "out of memory");
      যাও cleanup;
    }
  }
  অন্যথা যদি(!read_file_into(conn_config->চাবি, &চাবি_contents)) {
    failf(তথ্য, "rustls: failed to read key file: '%s'",
          conn_config->চাবি);
    ফলাফল = CURLE_SSL_CERTPROBLEM;
    যাও cleanup;
  }

  rr = rustls_certified_চাবি_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&চাবি_contents),
                                  curlx_dyn_len(&চাবি_contents),
                                  &certified_চাবি);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "rustls: failed to build certified key");
    ফলাফল = CURLE_SSL_CERTPROBLEM;
    যাও cleanup;
  }

  rr = rustls_certified_চাবি_চাবিগুলি_match(certified_চাবি);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr,
                 "rustls: client certificate and keypair files do not match:");
    ফলাফল = CURLE_SSL_CERTPROBLEM;
    যাও cleanup;
  }

  rr = rustls_client_config_builder_set_certified_চাবি(builder,
                                                      &certified_চাবি,
                                                      1);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "rustls: failed to set certified key");
    ফলাফল = CURLE_SSL_CERTPROBLEM;
    যাও cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&চাবি_contents);
  যদি(certified_চাবি) {
    rustls_certified_চাবি_free(certified_চাবি);
  }
  ফেরত ফলাফল;
}

#ifdef USE_ECH

স্থির CURLcode
init_config_builder_ech(কাঠামো Curl_easy *তথ্য,
                        কাঠামো Curl_cfilter *cf,
                        কাঠামো rustls_client_config_builder *builder)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ধ্রুবক rustls_hpke *hpke = rustls_supported_hpke();
  unsigned অক্ষর *ech_config = ফাঁকা;
  size_t ech_config_len = 0;
  কাঠামো Curl_dns_entry *dns = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;
  rustls_ফলাফল rr;

  যদি(!hpke) {
    failf(তথ্য,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    ফলাফল = CURLE_SSL_CONNECT_ERROR;
    যাও cleanup;
  }

  যদি(conn_config->ech_public) {
    failf(তথ্য, "rustls: ECH outername not supported");
    ফলাফল = CURLE_SSL_CONNECT_ERROR;
    যাও cleanup;
  }

  যদি(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    যদি(rr != RUSTLS_ফলাফল_OK) {
      rustls_failf(তথ্য, rr, "rustls: failed to configure ECH GREASE");
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও cleanup;
    }
    ফেরত CURLE_OK;
  }

  যদি(conn_config->ech && conn_config->ech_config) {
    ধ্রুবক অক্ষর *b64 = conn_config->ech_config;
    size_t decode_ফলাফল;
    যদি(!b64[0]) {
      infof(তথ্য, "rustls: ECHConfig from command line empty");
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_ফলাফল = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    যদি(decode_ফলাফল || !ech_config) {
      infof(তথ্য, "rustls: cannot base64 decode ECHConfig from command line");
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও cleanup;
    }
  }
  অন্যথা {
    ধ্রুবক কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
    ধ্রুবক কাঠামো Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(তথ্য, cf->sockindex, connssl->peer.origin);

    যদি(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(তথ্য, "rustls: ECH requested but no ECHConfig available");
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "rustls: failed to configure ECH");
    ফলাফল = CURLE_SSL_CONNECT_ERROR;
    যাও cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  যদি(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  যদি(dns) {
    Curl_dns_entry_unlink(তথ্য, &dns);
  }
  ফেরত ফলাফল;
}
#endif /* USE_ECH */

স্থির CURLcode cr_init_backend(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                কাঠামো rustls_ssl_backend_তথ্য * ধ্রুবক backend)
{
  ধ্রুবক কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  ধ্রুবক কাঠামো ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  কাঠামো rustls_connection *rconn = ফাঁকা;
  কাঠামো rustls_client_config_builder *config_builder = ফাঁকা;

  ধ্রুবক কাঠামো curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ধ্রুবক অক্ষর * ধ্রুবক ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ফাঁকা : conn_config->CAfile);
  CURLcode ফলাফল = CURLE_OK;
  rustls_ফলাফল rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  ফলাফল = init_config_builder(cf, তথ্য, conn_config, &config_builder);
  যদি(ফলাফল != CURLE_OK) {
    ফেরত ফলাফল;
  }

  যদি(connssl->alpn) {
    init_config_builder_alpn(তথ্য, connssl, config_builder);
  }

  যদি(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  অন্যথা যদি(conn_config->native_ca_store) {
    যদি(conn_config->CRLfile) {
      failf(তথ্য, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      ফেরত CURLE_NOT_BUILT_IN;
    }
    ফলাফল = init_config_builder_platform_verifier(তথ্য, config_builder);
    যদি(ফলাফল != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ফেরত ফলাফল;
    }
  }
  অন্যথা যদি(ca_info_blob || ssl_cafile) {
    ফলাফল = init_config_builder_verifier(তথ্য,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    যদি(ফলাফল != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ফেরত ফলাফল;
    }
  }

  যদি(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->চাবি || conn_config->চাবি_blob) {
    ফলাফল = init_config_builder_client_auth(তথ্য,
                                             conn_config,
                                             config_builder);
    যদি(ফলাফল != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      ফেরত ফলাফল;
    }
  }

#ifdef USE_ECH
  যদি(Curl_ssl_ech_enabled(cf)) {
    ফলাফল = init_config_builder_ech(তথ্য, cf, config_builder);
    যদি((ফলাফল != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      ফেরত ফলাফল;
    }
  }
#endif /* USE_ECH */

  ফলাফল = init_config_builder_keylog(তথ্য, config_builder);
  যদি(ফলাফল != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    ফেরত ফলাফল;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "failed to build client config");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  যদি(rr != RUSTLS_ফলাফল_OK) {
    rustls_failf(তথ্য, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = ফাঁকা;
    ফেরত CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  ফেরত ফলাফল;
}

স্থির শূন্য cr_set_negotiated_alpn(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য,
                                   ধ্রুবক কাঠামো rustls_connection *rconn)
{
  কাঠামো ssl_connect_তথ্য * ধ্রুবক connssl = cf->ctx;
  ধ্রুবক uint8_t *protocol = ফাঁকা;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, তথ্য, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
স্থির CURLcode cr_connect(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                           bool *done)
{
  কাঠামো ssl_connect_তথ্য * ধ্রুবক connssl = cf->ctx;
  ধ্রুবক কাঠামো rustls_ssl_backend_তথ্য * ধ্রুবক backend =
    (কাঠামো rustls_ssl_backend_তথ্য *)connssl->backend;
  ধ্রুবক কাঠামো rustls_connection *rconn = ফাঁকা;
  CURLcode tmperr = CURLE_OK;
  CURLcode ফলাফল;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(তথ্য, cf, "cr_connect, state=%d", (পূর্ণসংখ্যা)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    যদি(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(তথ্য, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(তথ্য, cf, "need HTTPS-RR for ECH, delaying connect");
      ফেরত CURLE_OK;
    }
#endif /* USE_ECH */

  যদি(!backend->conn) {
    ফলাফল =
      cr_init_backend(cf, তথ্য,
                      (কাঠামো rustls_ssl_backend_তথ্য *)connssl->backend);
    CURL_TRC_CF(তথ্য, cf, "cr_connect, init backend -> %d", (পূর্ণসংখ্যা)ফলাফল);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  জন্য(;;) {
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
      cr_set_negotiated_alpn(cf, তথ্য, rconn);
      tmperr = cr_send(cf, তথ্য, ফাঁকা, 0, &nwritten);
      যদি(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ফেরত CURLE_OK;
      }
      অন্যথা যদি(tmperr != CURLE_OK) {
        ফেরত tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        ধ্রুবক uint16_t proto = rustls_connection_get_protocol_version(rconn);
        ধ্রুবক rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ধ্রুবক rustls_str kex_group_name =
          rustls_connection_get_negotiated_চাবি_exchange_group_name(rconn);
        ধ্রুবক অক্ষর *ver = "TLS version unknown";
        যদি(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        যদি(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(তথ্য,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (পূর্ণসংখ্যা)ciphersuite_name.len,
              ciphersuite_name.তথ্য,
              (পূর্ণসংখ্যা)kex_group_name.len,
              kex_group_name.তথ্য);
      }
      যদি(তথ্য->set.ssl.certinfo) {
        size_t অঙ্ক_certs = 0;
        size_t i;
        যখন(rustls_connection_get_peer_certificate(rconn, অঙ্ক_certs)) {
          অঙ্ক_certs++;
          যদি(অঙ্ক_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(তথ্য, "%zu certificates is more than allowed (%d)",
                  অঙ্ক_certs, MAX_ALLOWED_CERT_AMOUNT);
            ফেরত CURLE_SSL_CONNECT_ERROR;
          }
        }
        ফলাফল = Curl_ssl_init_certinfo(তথ্য, (পূর্ণসংখ্যা)অঙ্ক_certs);
        যদি(ফলাফল)
          ফেরত ফলাফল;
        জন্য(i = 0; i < অঙ্ক_certs; i++) {
          ধ্রুবক rustls_certificate *cert;
          ধ্রুবক unsigned অক্ষর *der_তথ্য;
          size_t der_len;
          rustls_ফলাফল rresult = RUSTLS_ফলাফল_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_তথ্য, &der_len);
          যদি(rresult != RUSTLS_ফলাফল_OK) {
            অক্ষর errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, আকার(errorbuf), &errorlen);
            failf(তথ্য,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (পূর্ণসংখ্যা)errorlen, errorbuf);
            ফেরত map_error(rresult);
          }
          ফলাফল = Curl_extract_certinfo(তথ্য, (পূর্ণসংখ্যা)i,
                                         der_তথ্য, der_তথ্য + der_len);
          যদি(ফলাফল)
            ফেরত ফলাফল;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      ফেরত CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    যদি(wants_write) {
      size_t nwritten;
      CURL_TRC_CF(তথ্য, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, তথ্য, ফাঁকা, 0, &nwritten);
      যদি(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(তথ্য, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ফেরত CURLE_OK;
      }
      অন্যথা যদি(tmperr != CURLE_OK) {
        ফেরত tmperr;
      }
    }

    যদি(wants_read) {
      CURL_TRC_CF(তথ্য, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, তথ্য, &tmperr);
      যদি(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(তথ্য, "TLS connect error: Connection closed abruptly");
        ফেরত CURLE_SSL_CONNECT_ERROR;
      }
      যদি(nread < 0) {
        যদি(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(তথ্য, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          ফেরত CURLE_OK;
        }
        অন্যথা যদি(tmperr == CURLE_RECV_ERROR) {
          ফেরত CURLE_SSL_CONNECT_ERROR;
        }
        অন্যথা {
          ফেরত tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

স্থির শূন্য *cr_get_internals(কাঠামো ssl_connect_তথ্য *connssl, CURLINFO info)
{
  কাঠামো rustls_ssl_backend_তথ্য *backend =
    (কাঠামো rustls_ssl_backend_তথ্য *)connssl->backend;
  (শূন্য)info;
  DEBUGASSERT(backend);
  ফেরত backend->conn;
}

স্থির CURLcode cr_shutdown(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক bool send_shutdown, bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো rustls_ssl_backend_তথ্য *backend =
    (কাঠামো rustls_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  যদি(!backend->conn || cf->shutdown) {
    *done = TRUE;
    যাও out;
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

  ফলাফল = cr_send(cf, তথ্য, ফাঁকা, 0, &nwritten);
  যদি(ফলাফল) {
    যদি(ফলাফল == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ফলাফল = CURLE_OK;
      যাও out;
    }
    DEBUGASSERT(ফলাফল);
    CURL_TRC_CF(তথ্য, cf, "shutdown send failed: %d", (পূর্ণসংখ্যা)ফলাফল);
    যাও out;
  }

  জন্য(i = 0; i < 10; ++i) {
    অক্ষর buf[1024];
    ফলাফল = cr_recv(cf, তথ্য, buf, (পূর্ণসংখ্যা)আকার(buf), &nread);
    যদি(ফলাফল)
      বিরতি;
  }

  যদি(ফলাফল == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ফলাফল = CURLE_OK;
  }
  অন্যথা যদি(ফলাফল) {
    DEBUGASSERT(ফলাফল);
    CURL_TRC_CF(তথ্য, cf, "shutdown, error: %d", (পূর্ণসংখ্যা)ফলাফল);
  }
  অন্যথা যদি(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (ফলাফল || *done);
  ফেরত ফলাফল;
}

স্থির শূন্য cr_close(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  ধ্রুবক কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো rustls_ssl_backend_তথ্য *backend =
    (কাঠামো rustls_ssl_backend_তথ্য *)connssl->backend;

  (শূন্য)তথ্য;
  DEBUGASSERT(backend);
  যদি(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = ফাঁকা;
  }
  যদি(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = ফাঁকা;
  }
}

স্থির size_t cr_version(অক্ষর *বাফার, size_t আকার_2)
{
  ধ্রুবক কাঠামো rustls_str ver = rustls_version();
  ফেরত curl_msnprintf(বাফার, আকার_2, "%.*s", (পূর্ণসংখ্যা)ver.len, ver.তথ্য);
}

স্থির CURLcode cr_random(কাঠামো Curl_easy *তথ্য, unsigned অক্ষর *entropy,
                          size_t দৈর্ঘ্য)
{
  rustls_ফলাফল rresult = 0;
  (শূন্য)তথ্য;
  rresult = rustls_default_crypto_provider_random(entropy, দৈর্ঘ্য);
  ফেরত map_error(rresult);
}

স্থির শূন্য cr_cleanup(শূন্য)
{
  Curl_tls_keylog_close();
}

ধ্রুবক কাঠামো Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  আকার(কাঠামো rustls_ssl_backend_তথ্য),

  ফাঁকা,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_তথ্য_pending,                 /* data_pending */
  cr_random,                       /* random */
  ফাঁকা,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  ফাঁকা,                            /* close_all */
  ফাঁকা,                            /* engines_list */
  ফাঁকা,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  ফাঁকা,                            /* get_channel_binding */
};

#endif /* USE_RUSTLS */
