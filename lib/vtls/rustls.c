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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_RUSTLS

#అన్తర్భావ <rustls.h>

#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "curlx/strerr.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "vdns/httpsrr.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_int.h"
#అన్తర్భావ "vtls/rustls.h"
#అన్తర్భావ "vtls/keylog.h"
#అన్తర్భావ "vtls/cipher_suite.h"
#అన్తర్భావ "vtls/x509asn1.h"
#నిర్వచితమైతే USE_ECH
#అన్తర్భావ "curlx/base64.h"
#అయితే_ముగింపు

#ఒకవేళ EAGAIN != EWOULDBLOCK
#నిర్వచించు RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#లేకపోతే
#నిర్వచించు RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#అయితే_ముగింపు

నిర్మాణం rustls_ssl_backend_దత్తాంశం {
  స్థిరరాశి నిర్మాణం rustls_client_config *config;
  నిర్మాణం rustls_connection *conn;
  size_t plain_బహిర్గతం_buffered;
  బిట్టు(దత్తాంశం_లోపల_pending);
  బిట్టు(పంపిన_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
స్థిర CURLcode map_error(స్థిరరాశి rustls_ఫలితం r)
{
  ఒకవేళ(rustls_ఫలితం_is_cert_error(r)) {
    తిరిగి CURLE_PEER_FAILED_VERIFICATION;
  }
  ఎంపిక(r) {
  స్థితి RUSTLS_ఫలితం_OK:
    తిరిగి CURLE_OK;
  స్థితి RUSTLS_ఫలితం_NULL_PARAMETER:
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  అప్రమేయ:
    తిరిగి CURLE_RECV_ERROR;
  }
}

స్థిర శూన్య rustls_failf(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి rustls_ఫలితం rr,
                         స్థిరరాశి అక్షరం *సందేశం)
{
  అక్షరం errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, పరిమాణం(errorbuf), &errorlen);
  failf(దత్తాంశం, "%s: %.*s", సందేశం, (పూర్ణాంకం)errorlen, errorbuf);
}

స్థిర bool cr_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                            స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి నిర్మాణం ssl_connect_దత్తాంశం *ctx = cf->ctx;
  నిర్మాణం rustls_ssl_backend_దత్తాంశం *backend;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (నిర్మాణం rustls_ssl_backend_దత్తాంశం *)ctx->backend;
  తిరిగి (bool)backend->దత్తాంశం_లోపల_pending;
}

నిర్మాణం io_ctx {
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం Curl_easy *దత్తాంశం;
};

స్థిర పూర్ణాంకం read_cb(శూన్య *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *బహిర్గతం_n)
{
  స్థిరరాశి నిర్మాణం io_ctx *io_ctx = userdata;
  నిర్మాణం ssl_connect_దత్తాంశం * స్థిరరాశి connssl = io_ctx->cf->ctx;
  CURLcode ఫలితం;
  పూర్ణాంకం ret = 0;
  size_t nread;

  ఫలితం = Curl_conn_cf_recv(io_ctx->cf->తదుపరి, io_ctx->దత్తాంశం,
                             (అక్షరం *)buf, len, &nread);
  ఒకవేళ(ఫలితం) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ret = EAGAIN;
    లేకపోతే
      ret = EINVAL;
  }
  లేకపోతే ఒకవేళ(nread == 0)
    connssl->peer_closed = TRUE;
  *బహిర్గతం_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->దత్తాంశం, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (పూర్ణాంకం)ఫలితం, nread);
  తిరిగి ret;
}

స్థిర పూర్ణాంకం write_cb(శూన్య *userdata, స్థిరరాశి uint8_t *buf, uintptr_t len,
                    uintptr_t *బహిర్గతం_n)
{
  స్థిరరాశి నిర్మాణం io_ctx *io_ctx = userdata;
  CURLcode ఫలితం;
  పూర్ణాంకం ret = 0;
  size_t nwritten;

  ఫలితం = Curl_conn_cf_send(io_ctx->cf->తదుపరి, io_ctx->దత్తాంశం,
                             buf, len, FALSE, &nwritten);
  ఒకవేళ(ఫలితం) {
    nwritten = 0;
    ఒకవేళ(ఫలితం == CURLE_AGAIN)
      ret = EAGAIN;
    లేకపోతే
      ret = EINVAL;
  }
  *బహిర్గతం_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->దత్తాంశం, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (పూర్ణాంకం)ఫలితం, nwritten);
  తిరిగి ret;
}

స్థిర ssize_t tls_recv_మరిన్ని(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం, CURLcode *err)
{
  స్థిరరాశి నిర్మాణం ssl_connect_దత్తాంశం * స్థిరరాశి connssl = cf->ctx;
  నిర్మాణం rustls_ssl_backend_దత్తాంశం * స్థిరరాశి backend =
    (నిర్మాణం rustls_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం io_ctx io_ctx;
  size_t tls_బైట్లు_read = 0;
  rustls_io_ఫలితం io_error;
  rustls_ఫలితం rresult = 0;

  io_ctx.cf = cf;
  io_ctx.దత్తాంశం = దత్తాంశం;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_బైట్లు_read);
  ఒకవేళ(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    తిరిగి -1;
  }
  లేకపోతే ఒకవేళ(io_error) {
    అక్షరం బఫరు[STRERROR_LEN];
    failf(దత్తాంశం, "reading from socket: %s",
          curlx_strerror(io_error, బఫరు, పరిమాణం(బఫరు)));
    *err = CURLE_RECV_ERROR;
    తిరిగి -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  ఒకవేళ(rresult != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    తిరిగి -1;
  }

  backend->దత్తాంశం_లోపల_pending = TRUE;
  *err = CURLE_OK;
  తిరిగి (ssize_t)tls_బైట్లు_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
స్థిర CURLcode cr_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                        అక్షరం *plainbuf, size_t plainlen, size_t *pnread)
{
  స్థిరరాశి నిర్మాణం ssl_connect_దత్తాంశం * స్థిరరాశి connssl = cf->ctx;
  నిర్మాణం rustls_ssl_backend_దత్తాంశం * స్థిరరాశి backend =
    (నిర్మాణం rustls_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం rustls_connection *rconn = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  size_t n = 0;
  rustls_ఫలితం rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  అయితే(*pnread < plainlen) {
    ఒకవేళ(!backend->దత్తాంశం_లోపల_pending) {
      ఒకవేళ(tls_recv_మరిన్ని(cf, దత్తాంశం, &ఫలితం) < 0) {
        ఒకవేళ(ఫలితం != CURLE_AGAIN) {
          వెళ్ళు బహిర్గతం;
        }
        ఫలితం = CURLE_OK;
        ఆపు;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    ఒకవేళ(rresult == RUSTLS_ఫలితం_PLAINTEXT_ఖాళీలు) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->దత్తాంశం_లోపల_pending = FALSE;
      ఆపు;
    }
    లేకపోతే ఒకవేళ(rresult == RUSTLS_ఫలితం_UNEXPECTED_EOF) {
      failf(దత్తాంశం, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      ఫలితం = CURLE_RECV_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(rresult != RUSTLS_ఫలితం_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(దత్తాంశం, rresult, "rustls_connection_read");
      ఫలితం = CURLE_RECV_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      ఆపు;
    }
    లేకపోతే {
      *pnread += n;
    }
  }

  ఒకవేళ(!eof && !*pnread) {
    ఫలితం = CURLE_AGAIN;
  }

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (పూర్ణాంకం)ఫలితం, *pnread);
  తిరిగి ఫలితం;
}

స్థిర CURLcode cr_flush_బహిర్గతం(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం rustls_connection *rconn)
{
  నిర్మాణం io_ctx io_ctx;
  rustls_io_ఫలితం io_error;
  size_t tlswritten = 0;
  size_t tlswritten_మొత్తం = 0;

  io_ctx.cf = cf;
  io_ctx.దత్తాంశం = దత్తాంశం;

  అయితే(rustls_connection_కోరుతుంది_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    ఒకవేళ(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(దత్తాంశం, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_మొత్తం);
      తిరిగి CURLE_AGAIN;
    }
    లేకపోతే ఒకవేళ(io_error) {
      అక్షరం బఫరు[STRERROR_LEN];
      failf(దత్తాంశం, "writing to socket: %s",
            curlx_strerror(io_error, బఫరు, పరిమాణం(బఫరు)));
      తిరిగి CURLE_SEND_ERROR;
    }
    ఒకవేళ(tlswritten == 0) {
      failf(దత్తాంశం, "EOF in swrite");
      తిరిగి CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(దత్తాంశం, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_మొత్తం += tlswritten;
  }
  తిరిగి CURLE_OK;
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
స్థిర CURLcode cr_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి శూన్య *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  స్థిరరాశి నిర్మాణం ssl_connect_దత్తాంశం * స్థిరరాశి connssl = cf->ctx;
  నిర్మాణం rustls_ssl_backend_దత్తాంశం * స్థిరరాశి backend =
    (నిర్మాణం rustls_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం rustls_connection *rconn = ఖాళీ;
  size_t plainwritten = 0;
  స్థిరరాశి గుర్తులేని అక్షరం *buf = plainbuf;
  CURLcode ఫలితం = CURLE_OK;
  size_t blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(దత్తాంశం, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  ఒకవేళ(backend->plain_బహిర్గతం_buffered) {
    ఫలితం = cr_flush_బహిర్గతం(cf, దత్తాంశం, rconn);
    CURL_TRC_CF(దత్తాంశం, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_బహిర్గతం_buffered, (పూర్ణాంకం)ఫలితం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    ఒకవేళ(blen > backend->plain_బహిర్గతం_buffered) {
      blen -= backend->plain_బహిర్గతం_buffered;
      buf += backend->plain_బహిర్గతం_buffered;
    }
    లేకపోతే
      blen = 0;
    *pnwritten += backend->plain_బహిర్గతం_buffered;
    backend->plain_బహిర్గతం_buffered = 0;
  }

  ఒకవేళ(blen > 0) {
    rustls_ఫలితం rresult;
    CURL_TRC_CF(దత్తాంశం, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    ఒకవేళ(rresult != RUSTLS_ఫలితం_OK) {
      rustls_failf(దత్తాంశం, rresult, "rustls_connection_write");
      ఫలితం = CURLE_WRITE_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(plainwritten == 0) {
      failf(దత్తాంశం, "rustls_connection_write: EOF");
      ఫలితం = CURLE_WRITE_ERROR;
      వెళ్ళు బహిర్గతం;
    }
  }

  ఫలితం = cr_flush_బహిర్గతం(cf, దత్తాంశం, rconn);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_బహిర్గతం_buffered = plainwritten;
      ఒకవేళ(*pnwritten) {
        ఫలితం = CURLE_OK;
      }
    }
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే
    *pnwritten += plainwritten;

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (పూర్ణాంకం)ఫలితం, *pnwritten);
  తిరిగి ఫలితం;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
స్థిర uint32_t cr_verify_none(శూన్య *userdata,
                               స్థిరరాశి rustls_verify_server_cert_params *params)
{
  (శూన్య)userdata;
  (శూన్య)params;
  తిరిగి RUSTLS_ఫలితం_OK;
}

స్థిర పూర్ణాంకం read_file_into(స్థిరరాశి అక్షరం *filename, నిర్మాణం dynbuf *బహిర్గతం)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  ఒకవేళ(!f) {
    తిరిగి 0;
  }

  కొరకు(;;) {
    uint8_t buf[256];
    స్థిరరాశి size_t rr = fread(buf, 1, పరిమాణం(buf), f);
    ఒకవేళ((!rr && !feof(f)) ||
       curlx_dyn_addn(బహిర్గతం, buf, rr)) {
      curlx_fclose(f);
      తిరిగి 0;
    }
    ఒకవేళ(rr < పరిమాణం(buf))
      ఆపు;
  }

  తిరిగి curlx_fclose(f) == 0;
}

స్థిర శూన్య
cr_get_selected_ciphers(నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *ciphers12,
                        స్థిరరాశి అక్షరం *ciphers13,
                        స్థిరరాశి నిర్మాణం rustls_supported_ciphersuite **selected,
                        size_t *selected_కొలత)
{
  స్థిరరాశి size_t supported_len = *selected_కొలత;
  స్థిరరాశి size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  స్థిరరాశి నిర్మాణం rustls_supported_ciphersuite *నమోదు_2 = ఖాళీ;
  స్థిరరాశి అక్షరం *ciphers = ciphers12;
  size_t లెక్క = 0, default13_లెక్క = 0, i, j;
  స్థిరరాశి అక్షరం *ptr, *చివర;

  DEBUGASSERT(default_len <= supported_len);

  ఒకవేళ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    కొరకు(j = 0; j < default_len; j++) {
      నమోదు_2 = rustls_default_crypto_provider_ciphersuites_get(j);
      ఒకవేళ(rustls_supported_ciphersuite_protocol_సంస్కరణ(నమోదు_2) !=
         RUSTLS_TLS_సంస్కరణ_TLSV1_3)
        కొనసాగు;

      selected[లెక్క++] = నమోదు_2;
    }

    default13_లెక్క = లెక్క;

    ఒకవేళ(!ciphers)
      ciphers = "";
  }
  లేకపోతే
    ciphers = ciphers13;

కూడు_ciphers:
  కొరకు(ptr = ciphers; ptr[0] != '\0' && లెక్క < supported_len; ptr = చివర) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &చివర);

    /* Check if cipher is supported */
    ఒకవేళ(id) {
      కొరకు(i = 0; i < supported_len; i++) {
        నమోదు_2 = rustls_default_crypto_provider_ciphersuites_get(i);
        ఒకవేళ(rustls_supported_ciphersuite_get_suite(నమోదు_2) == id)
          ఆపు;
      }
      ఒకవేళ(i == supported_len)
        id = 0;
    }
    ఒకవేళ(!id) {
      ఒకవేళ(ptr[0] != '\0')
        infof(దత్తాంశం, "rustls: unknown cipher in list: \"%.*s\"",
              (పూర్ణాంకం)(చివర - ptr), ptr);
      కొనసాగు;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    కొరకు(i = 0; i < లెక్క && selected[i] != నమోదు_2; i++)
      ;
    ఒకవేళ(i < లెక్క) {
      ఒకవేళ(i >= default13_లెక్క)
        infof(దత్తాంశం, "rustls: duplicate cipher in list: \"%.*s\"",
              (పూర్ణాంకం)(చివర - ptr), ptr);
      కొనసాగు;
    }

    selected[లెక్క++] = నమోదు_2;
  }

  ఒకవేళ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    వెళ్ళు కూడు_ciphers;
  }

  ఒకవేళ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    కొరకు(j = 0; j < default_len; j++) {
      నమోదు_2 = rustls_default_crypto_provider_ciphersuites_get(j);
      ఒకవేళ(rustls_supported_ciphersuite_protocol_సంస్కరణ(నమోదు_2) ==
         RUSTLS_TLS_సంస్కరణ_TLSV1_3)
        కొనసాగు;

      /* No duplicates allowed (so selected cannot overflow) */
      కొరకు(i = 0; i < లెక్క && selected[i] != నమోదు_2; i++)
        ;
      ఒకవేళ(i < లెక్క)
        కొనసాగు;

      selected[లెక్క++] = నమోదు_2;
    }
  }

  *selected_కొలత = లెక్క;
}

స్థిర శూన్య cr_keylog_log_cb(నిర్మాణం rustls_str label,
                             స్థిరరాశి uint8_t *client_random,
                             size_t client_random_len, స్థిరరాశి uint8_t *secret,
                             size_t secret_len)
{
  అక్షరం clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_కొలత);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, పరిమాణం(clabel), "%.*s", (పూర్ణాంకం)label.len, label.దత్తాంశం);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

స్థిర CURLcode
init_config_builder(నిర్మాణం Curl_cfilter *cf,
                    నిర్మాణం Curl_easy *దత్తాంశం,
                    స్థిరరాశి నిర్మాణం ssl_filter_config *conn_config,
                    నిర్మాణం rustls_client_config_builder **config_builder)
{
  స్థిరరాశి నిర్మాణం rustls_supported_ciphersuite **cipher_suites = ఖాళీ;
  నిర్మాణం rustls_crypto_provider_builder *custom_provider_builder = ఖాళీ;
  స్థిరరాశి నిర్మాణం rustls_crypto_provider *custom_provider = ఖాళీ;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_సంస్కరణ_TLSV1_2,
    RUSTLS_TLS_సంస్కరణ_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode ఫలితం = CURLE_OK;
  rustls_ఫలితం rr;

  DEBUGASSERT(conn_config->సంస్కరణ_2 != CURL_SSLVERSION_DEFAULT);
  ఎంపిక(conn_config->సంస్కరణ_2) {
  స్థితి CURL_SSLVERSION_TLSv1:
  స్థితి CURL_SSLVERSION_TLSv1_0:
  స్థితి CURL_SSLVERSION_TLSv1_1:
  స్థితి CURL_SSLVERSION_TLSv1_2:
    ఆపు;
  స్థితి CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_సంస్కరణ_TLSV1_3;
    tls_versions_len = 1;
    ఆపు;
  అప్రమేయ:
    failf(దత్తాంశం, "rustls: unsupported minimum TLS version value");
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు cleanup;
  }

  ఎంపిక(conn_config->సంస్కరణ_max) {
  స్థితి CURL_SSLVERSION_MAX_DEFAULT:
  స్థితి CURL_SSLVERSION_MAX_NONE:
  స్థితి CURL_SSLVERSION_MAX_TLSv1_3:
    ఆపు;
  స్థితి CURL_SSLVERSION_MAX_TLSv1_2:
    ఒకవేళ(tls_versions[0] == RUSTLS_TLS_సంస్కరణ_TLSV1_2) {
      tls_versions_len = 1;
      ఆపు;
    }
    FALLTHROUGH();
  స్థితి CURL_SSLVERSION_MAX_TLSv1_1:
  స్థితి CURL_SSLVERSION_MAX_TLSv1_0:
  అప్రమేయ:
    failf(దత్తాంశం, "rustls: unsupported maximum TLS version value");
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు cleanup;
  }

#నిర్వచితమైతే USE_ECH
  ఒకవేళ(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_సంస్కరణ_TLSV1_3;
    tls_versions_len = 1;
    infof(దత్తాంశం, "rustls: ECH enabled, forcing TLSv1.3");
  }
#లేకపోతే
  (శూన్య)cf;
#అయితే_ముగింపు /* USE_ECH */

  cipher_suites = curlx_malloc(పరిమాణం(*cipher_suites) * cipher_suites_len);
  ఒకవేళ(!cipher_suites) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు cleanup;
  }

  cr_get_selected_ciphers(దత్తాంశం,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  ఒకవేళ(cipher_suites_len == 0) {
    failf(దత్తాంశం, "rustls: no supported cipher in list");
    ఫలితం = CURLE_SSL_CIPHER;
    వెళ్ళు cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr,
                 "failed to create crypto provider builder from default");
    ఫలితం = CURLE_SSL_CIPHER;
    వెళ్ళు cleanup;
  }

  rr =
    rustls_crypto_provider_builder_సమితి_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr,
                 "failed to set ciphersuites for crypto provider builder");
    ఫలితం = CURLE_SSL_CIPHER;
    వెళ్ళు cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "failed to build custom crypto provider");
    ఫలితం = CURLE_SSL_CIPHER;
    వెళ్ళు cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "failed to create client config builder");
    ఫలితం = CURLE_SSL_CIPHER;
    వెళ్ళు cleanup;
  }

cleanup:
  ఒకవేళ(cipher_suites) {
    curlx_free(cipher_suites);
  }
  ఒకవేళ(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  ఒకవేళ(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య
init_config_builder_alpn(నిర్మాణం Curl_easy *దత్తాంశం,
                         స్థిరరాశి నిర్మాణం ssl_connect_దత్తాంశం *connssl,
                         నిర్మాణం rustls_client_config_builder *config_builder)
{
  నిర్మాణం alpn_proto_buf proto;
  rustls_ముక్క_బైట్లు alpn[ALPN_ENTRIES_MAX];
  size_t i;

  కొరకు(i = 0; i < connssl->alpn->లెక్క; ++i) {
    alpn[i].దత్తాంశం = (స్థిరరాశి uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = అక్షరశ్రేణి_పొడవు(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_సమితి_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->లెక్క);
  Curl_alpn_గమ్యం_proto_str(&proto, connssl->alpn);
  infof(దత్తాంశం, VTLS_INFOF_ALPN_OFFER_1STR, proto.దత్తాంశం);
}

స్థిర CURLcode init_config_builder_verifier_crl(
  నిర్మాణం Curl_easy *దత్తాంశం,
  స్థిరరాశి నిర్మాణం ssl_filter_config *conn_config,
  నిర్మాణం rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం dynbuf crl_contents;
  rustls_ఫలితం rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_కొలత);
  ఒకవేళ(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(దత్తాంశం, "rustls: failed to read revocation list file");
    ఫలితం = CURLE_SSL_CRL_BADFILE;
    వెళ్ళు cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_కూడు_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "failed to parse revocation list");
    ఫలితం = CURLE_SSL_CRL_BADFILE;
    వెళ్ళు cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  తిరిగి ఫలితం;
}

స్థిర CURLcode
init_config_builder_verifier(నిర్మాణం Curl_easy *దత్తాంశం,
                             నిర్మాణం rustls_client_config_builder *builder,
                             స్థిరరాశి నిర్మాణం ssl_filter_config *conn_config,
                             స్థిరరాశి నిర్మాణం curl_blob *ca_info_blob,
                             స్థిరరాశి అక్షరం * స్థిరరాశి ssl_cafile)
{
  స్థిరరాశి నిర్మాణం rustls_మూలం_cert_store *roots = ఖాళీ;
  నిర్మాణం rustls_మూలం_cert_store_builder *roots_builder = ఖాళీ;
  నిర్మాణం rustls_web_pki_server_cert_verifier_builder *verifier_builder = ఖాళీ;
  నిర్మాణం rustls_server_cert_verifier *server_cert_verifier = ఖాళీ;
  rustls_ఫలితం rr = RUSTLS_ఫలితం_OK;
  CURLcode ఫలితం = CURLE_OK;

  roots_builder = rustls_మూలం_cert_store_builder_new();
  ఒకవేళ(ca_info_blob) {
    rr = rustls_మూలం_cert_store_builder_కూడు_pem(roots_builder,
                                                ca_info_blob->దత్తాంశం,
                                                ca_info_blob->len,
                                                1);
    ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
      rustls_failf(దత్తాంశం, rr, "failed to parse trusted certificates from blob");
      ఫలితం = CURLE_SSL_CACERT_BADFILE;
      వెళ్ళు cleanup;
    }
  }
  లేకపోతే ఒకవేళ(ssl_cafile) {
    rr = rustls_మూలం_cert_store_builder_ఎక్కించు_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
      rustls_failf(దత్తాంశం, rr, "failed to load trusted certificates");
      ఫలితం = CURLE_SSL_CACERT_BADFILE;
      వెళ్ళు cleanup;
    }
  }

  rr = rustls_మూలం_cert_store_builder_build(roots_builder, &roots);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "failed to build trusted root certificate store");
    ఫలితం = CURLE_SSL_CACERT_BADFILE;
    వెళ్ళు cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  ఒకవేళ(!verifier_builder) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు cleanup;
  }

  ఒకవేళ(conn_config->CRLfile) {
    ఫలితం = init_config_builder_verifier_crl(దత్తాంశం,
                                              conn_config,
                                              verifier_builder);
    ఒకవేళ(ఫలితం) {
      వెళ్ళు cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "failed to build certificate verifier");
    ఫలితం = CURLE_SSL_CACERT_BADFILE;
    వెళ్ళు cleanup;
  }

  rustls_client_config_builder_సమితి_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  ఒకవేళ(roots_builder) {
    rustls_మూలం_cert_store_builder_free(roots_builder);
  }
  ఒకవేళ(roots) {
    rustls_మూలం_cert_store_free(roots);
  }
  ఒకవేళ(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  ఒకవేళ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode init_config_builder_platform_verifier(
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం rustls_client_config_builder *builder)
{
  నిర్మాణం rustls_server_cert_verifier *server_cert_verifier = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  rustls_ఫలితం rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "failed to create platform certificate verifier");
    ఫలితం = CURLE_SSL_CACERT_BADFILE;
    వెళ్ళు cleanup;
  }

  rustls_client_config_builder_సమితి_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  ఒకవేళ(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode
init_config_builder_keylog(నిర్మాణం Curl_easy *దత్తాంశం,
                           నిర్మాణం rustls_client_config_builder *builder)
{
  rustls_ఫలితం rr;

  Curl_tls_keylog_open();
  ఒకవేళ(!Curl_tls_keylog_enabled()) {
    తిరిగి CURLE_OK;
  }

  rr = rustls_client_config_builder_సమితి_కీలకం_log(builder,
                                                cr_keylog_log_cb,
                                                ఖాళీ);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    తిరిగి map_error(rr);
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode
init_config_builder_client_auth(నిర్మాణం Curl_easy *దత్తాంశం,
                                స్థిరరాశి నిర్మాణం ssl_filter_config *conn_config,
                                నిర్మాణం rustls_client_config_builder *builder)
{
  నిర్మాణం dynbuf cert_contents;
  నిర్మాణం dynbuf కీలకం_contents;
  rustls_ఫలితం rr;
  స్థిరరాశి నిర్మాణం rustls_certified_కీలకం *certified_కీలకం = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  స్థిరరాశి bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  స్థిరరాశి bool have_కీలకం = conn_config->కీలకం || conn_config->కీలకం_blob;

  ఒకవేళ(have_cert && !have_కీలకం) {
    failf(దత్తాంశం, "rustls: must provide key with certificate");
    తిరిగి CURLE_SSL_CERTPROBLEM;
  }
  లేకపోతే ఒకవేళ(!have_cert && have_కీలకం) {
    failf(దత్తాంశం, "rustls: must provide certificate with key");
    తిరిగి CURLE_SSL_CERTPROBLEM;
  }

  ఒకవేళ(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(దత్తాంశం, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    తిరిగి CURLE_SSL_CERTPROBLEM;
  }
  ఒకవేళ(conn_config->కీలకం_type && !curl_strequal(conn_config->కీలకం_type, "PEM")) {
    failf(దత్తాంశం, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->కీలకం_type);
    తిరిగి CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_కొలత);
  curlx_dyn_init(&కీలకం_contents, DYN_KEYFILE_కొలత);

  ఒకవేళ(conn_config->cert_blob) {
    ఫలితం = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->దత్తాంశం,
                            conn_config->cert_blob->len);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "rustls: failed to copy client certificate blob (%s)",
            ఫలితం == CURLE_TOO_LARGE ? "too large" : "out of memory");
      వెళ్ళు cleanup;
    }
  }
  లేకపోతే ఒకవేళ(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(దత్తాంశం, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    ఫలితం = CURLE_SSL_CERTPROBLEM;
    వెళ్ళు cleanup;
  }

  ఒకవేళ(conn_config->కీలకం_blob) {
    ఫలితం = curlx_dyn_addn(&కీలకం_contents,
                            conn_config->కీలకం_blob->దత్తాంశం,
                            conn_config->కీలకం_blob->len);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "rustls: failed to copy key blob (%s)",
            ఫలితం == CURLE_TOO_LARGE ? "too large" : "out of memory");
      వెళ్ళు cleanup;
    }
  }
  లేకపోతే ఒకవేళ(!read_file_into(conn_config->కీలకం, &కీలకం_contents)) {
    failf(దత్తాంశం, "rustls: failed to read key file: '%s'",
          conn_config->కీలకం);
    ఫలితం = CURLE_SSL_CERTPROBLEM;
    వెళ్ళు cleanup;
  }

  rr = rustls_certified_కీలకం_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&కీలకం_contents),
                                  curlx_dyn_len(&కీలకం_contents),
                                  &certified_కీలకం);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "rustls: failed to build certified key");
    ఫలితం = CURLE_SSL_CERTPROBLEM;
    వెళ్ళు cleanup;
  }

  rr = rustls_certified_కీలకం_కీలకాలు_సరిపోలిక(certified_కీలకం);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr,
                 "rustls: client certificate and keypair files do not match:");
    ఫలితం = CURLE_SSL_CERTPROBLEM;
    వెళ్ళు cleanup;
  }

  rr = rustls_client_config_builder_సమితి_certified_కీలకం(builder,
                                                      &certified_కీలకం,
                                                      1);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "rustls: failed to set certified key");
    ఫలితం = CURLE_SSL_CERTPROBLEM;
    వెళ్ళు cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&కీలకం_contents);
  ఒకవేళ(certified_కీలకం) {
    rustls_certified_కీలకం_free(certified_కీలకం);
  }
  తిరిగి ఫలితం;
}

#నిర్వచితమైతే USE_ECH

స్థిర CURLcode
init_config_builder_ech(నిర్మాణం Curl_easy *దత్తాంశం,
                        నిర్మాణం Curl_cfilter *cf,
                        నిర్మాణం rustls_client_config_builder *builder)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  స్థిరరాశి rustls_hpke *hpke = rustls_supported_hpke();
  గుర్తులేని అక్షరం *ech_config = ఖాళీ;
  size_t ech_config_len = 0;
  నిర్మాణం Curl_dns_నమోదు *dns = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;
  rustls_ఫలితం rr;

  ఒకవేళ(!hpke) {
    failf(దత్తాంశం,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    ఫలితం = CURLE_SSL_CONNECT_ERROR;
    వెళ్ళు cleanup;
  }

  ఒకవేళ(conn_config->ech_ప్రజా) {
    failf(దత్తాంశం, "rustls: ECH outername not supported");
    ఫలితం = CURLE_SSL_CONNECT_ERROR;
    వెళ్ళు cleanup;
  }

  ఒకవేళ(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
      rustls_failf(దత్తాంశం, rr, "rustls: failed to configure ECH GREASE");
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు cleanup;
    }
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(conn_config->ech && conn_config->ech_config) {
    స్థిరరాశి అక్షరం *b64 = conn_config->ech_config;
    size_t decode_ఫలితం;
    ఒకవేళ(!b64[0]) {
      infof(దత్తాంశం, "rustls: ECHConfig from command line empty");
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_ఫలితం = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    ఒకవేళ(decode_ఫలితం || !ech_config) {
      infof(దత్తాంశం, "rustls: cannot base64 decode ECHConfig from command line");
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు cleanup;
    }
  }
  లేకపోతే {
    స్థిరరాశి నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
    స్థిరరాశి నిర్మాణం Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(దత్తాంశం, cf->sockindex, connssl->peer.origin);

    ఒకవేళ(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(దత్తాంశం, "rustls: ECH requested but no ECHConfig available");
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "rustls: failed to configure ECH");
    ఫలితం = CURLE_SSL_CONNECT_ERROR;
    వెళ్ళు cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  ఒకవేళ(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  ఒకవేళ(dns) {
    Curl_dns_నమోదు_unlink(దత్తాంశం, &dns);
  }
  తిరిగి ఫలితం;
}
#అయితే_ముగింపు /* USE_ECH */

స్థిర CURLcode cr_init_backend(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం rustls_ssl_backend_దత్తాంశం * స్థిరరాశి backend)
{
  స్థిరరాశి నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  స్థిరరాశి నిర్మాణం ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం rustls_connection *rconn = ఖాళీ;
  నిర్మాణం rustls_client_config_builder *config_builder = ఖాళీ;

  స్థిరరాశి నిర్మాణం curl_blob *ca_info_blob = conn_config->ca_info_blob;
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ఖాళీ : conn_config->CAfile);
  CURLcode ఫలితం = CURLE_OK;
  rustls_ఫలితం rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  ఫలితం = init_config_builder(cf, దత్తాంశం, conn_config, &config_builder);
  ఒకవేళ(ఫలితం != CURLE_OK) {
    తిరిగి ఫలితం;
  }

  ఒకవేళ(connssl->alpn) {
    init_config_builder_alpn(దత్తాంశం, connssl, config_builder);
  }

  ఒకవేళ(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_సమితి_certificate_verifier(
      config_builder, cr_verify_none);
  }
  లేకపోతే ఒకవేళ(conn_config->native_ca_store) {
    ఒకవేళ(conn_config->CRLfile) {
      failf(దత్తాంశం, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      తిరిగి CURLE_NOT_BUILT_లోపల;
    }
    ఫలితం = init_config_builder_platform_verifier(దత్తాంశం, config_builder);
    ఒకవేళ(ఫలితం != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      తిరిగి ఫలితం;
    }
  }
  లేకపోతే ఒకవేళ(ca_info_blob || ssl_cafile) {
    ఫలితం = init_config_builder_verifier(దత్తాంశం,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    ఒకవేళ(ఫలితం != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      తిరిగి ఫలితం;
    }
  }

  ఒకవేళ(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->కీలకం || conn_config->కీలకం_blob) {
    ఫలితం = init_config_builder_client_auth(దత్తాంశం,
                                             conn_config,
                                             config_builder);
    ఒకవేళ(ఫలితం != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      తిరిగి ఫలితం;
    }
  }

#నిర్వచితమైతే USE_ECH
  ఒకవేళ(Curl_ssl_ech_enabled(cf)) {
    ఫలితం = init_config_builder_ech(దత్తాంశం, cf, config_builder);
    ఒకవేళ((ఫలితం != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      తిరిగి ఫలితం;
    }
  }
#అయితే_ముగింపు /* USE_ECH */

  ఫలితం = init_config_builder_keylog(దత్తాంశం, config_builder);
  ఒకవేళ(ఫలితం != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    తిరిగి ఫలితం;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "failed to build client config");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  ఒకవేళ(rr != RUSTLS_ఫలితం_OK) {
    rustls_failf(దత్తాంశం, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = ఖాళీ;
    తిరిగి CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_సమితి_userdata(rconn, backend);
  backend->conn = rconn;

  తిరిగి ఫలితం;
}

స్థిర శూన్య cr_సమితి_negotiated_alpn(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం,
                                   స్థిరరాశి నిర్మాణం rustls_connection *rconn)
{
  నిర్మాణం ssl_connect_దత్తాంశం * స్థిరరాశి connssl = cf->ctx;
  స్థిరరాశి uint8_t *protocol = ఖాళీ;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_సమితి_negotiated(cf, దత్తాంశం, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
స్థిర CURLcode cr_connect(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                           bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం * స్థిరరాశి connssl = cf->ctx;
  స్థిరరాశి నిర్మాణం rustls_ssl_backend_దత్తాంశం * స్థిరరాశి backend =
    (నిర్మాణం rustls_ssl_backend_దత్తాంశం *)connssl->backend;
  స్థిరరాశి నిర్మాణం rustls_connection *rconn = ఖాళీ;
  CURLcode tmperr = CURLE_OK;
  CURLcode ఫలితం;
  bool కోరుతుంది_read;
  bool కోరుతుంది_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(దత్తాంశం, cf, "cr_connect, state=%d", (పూర్ణాంకం)connssl->state);
  *పూర్తి = FALSE;

#నిర్వచితమైతే USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    ఒకవేళ(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(దత్తాంశం, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(దత్తాంశం, cf, "need HTTPS-RR for ECH, delaying connect");
      తిరిగి CURLE_OK;
    }
#అయితే_ముగింపు /* USE_ECH */

  ఒకవేళ(!backend->conn) {
    ఫలితం =
      cr_init_backend(cf, దత్తాంశం,
                      (నిర్మాణం rustls_ssl_backend_దత్తాంశం *)connssl->backend);
    CURL_TRC_CF(దత్తాంశం, cf, "cr_connect, init backend -> %d", (పూర్ణాంకం)ఫలితం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  కొరకు(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    ఒకవేళ(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_సమితి_negotiated_alpn(cf, దత్తాంశం, rconn);
      tmperr = cr_send(cf, దత్తాంశం, ఖాళీ, 0, &nwritten);
      ఒకవేళ(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        తిరిగి CURLE_OK;
      }
      లేకపోతే ఒకవేళ(tmperr != CURLE_OK) {
        తిరిగి tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#నిర్వచితమైతే CURLVERBOSE
        స్థిరరాశి uint16_t proto = rustls_connection_get_protocol_సంస్కరణ(rconn);
        స్థిరరాశి rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        స్థిరరాశి rustls_str kex_సమూహం_name =
          rustls_connection_get_negotiated_కీలకం_exchange_సమూహం_name(rconn);
        స్థిరరాశి అక్షరం *ver = "TLS version unknown";
        ఒకవేళ(proto == RUSTLS_TLS_సంస్కరణ_TLSV1_3)
          ver = "TLSv1.3";
        ఒకవేళ(proto == RUSTLS_TLS_సంస్కరణ_TLSV1_2)
          ver = "TLSv1.2";
#అయితే_ముగింపు
        infof(దత్తాంశం,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (పూర్ణాంకం)ciphersuite_name.len,
              ciphersuite_name.దత్తాంశం,
              (పూర్ణాంకం)kex_సమూహం_name.len,
              kex_సమూహం_name.దత్తాంశం);
      }
      ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo) {
        size_t అంకె_certs = 0;
        size_t i;
        అయితే(rustls_connection_get_peer_certificate(rconn, అంకె_certs)) {
          అంకె_certs++;
          ఒకవేళ(అంకె_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(దత్తాంశం, "%zu certificates is more than allowed (%d)",
                  అంకె_certs, MAX_ALLOWED_CERT_AMOUNT);
            తిరిగి CURLE_SSL_CONNECT_ERROR;
          }
        }
        ఫలితం = Curl_ssl_init_certinfo(దత్తాంశం, (పూర్ణాంకం)అంకె_certs);
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
        కొరకు(i = 0; i < అంకె_certs; i++) {
          స్థిరరాశి rustls_certificate *cert;
          స్థిరరాశి గుర్తులేని అక్షరం *der_దత్తాంశం;
          size_t der_len;
          rustls_ఫలితం rresult = RUSTLS_ఫలితం_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_దత్తాంశం, &der_len);
          ఒకవేళ(rresult != RUSTLS_ఫలితం_OK) {
            అక్షరం errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, పరిమాణం(errorbuf), &errorlen);
            failf(దత్తాంశం,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (పూర్ణాంకం)errorlen, errorbuf);
            తిరిగి map_error(rresult);
          }
          ఫలితం = Curl_వెలికితీయి_certinfo(దత్తాంశం, (పూర్ణాంకం)i,
                                         der_దత్తాంశం, der_దత్తాంశం + der_len);
          ఒకవేళ(ఫలితం)
            తిరిగి ఫలితం;
        }
      }

      connssl->state = ssl_connection_complete;
      *పూర్తి = TRUE;
      తిరిగి CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    కోరుతుంది_read = rustls_connection_కోరుతుంది_read(rconn);
    కోరుతుంది_write = rustls_connection_కోరుతుంది_write(rconn) ||
                  backend->plain_బహిర్గతం_buffered;
    DEBUGASSERT(కోరుతుంది_read || కోరుతుంది_write);

    ఒకవేళ(కోరుతుంది_write) {
      size_t nwritten;
      CURL_TRC_CF(దత్తాంశం, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, దత్తాంశం, ఖాళీ, 0, &nwritten);
      ఒకవేళ(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(దత్తాంశం, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        తిరిగి CURLE_OK;
      }
      లేకపోతే ఒకవేళ(tmperr != CURLE_OK) {
        తిరిగి tmperr;
      }
    }

    ఒకవేళ(కోరుతుంది_read) {
      CURL_TRC_CF(దత్తాంశం, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_మరిన్ని(cf, దత్తాంశం, &tmperr);
      ఒకవేళ(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(దత్తాంశం, "TLS connect error: Connection closed abruptly");
        తిరిగి CURLE_SSL_CONNECT_ERROR;
      }
      ఒకవేళ(nread < 0) {
        ఒకవేళ(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(దత్తాంశం, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          తిరిగి CURLE_OK;
        }
        లేకపోతే ఒకవేళ(tmperr == CURLE_RECV_ERROR) {
          తిరిగి CURLE_SSL_CONNECT_ERROR;
        }
        లేకపోతే {
          తిరిగి tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

స్థిర శూన్య *cr_get_internals(నిర్మాణం ssl_connect_దత్తాంశం *connssl, CURLINFO info)
{
  నిర్మాణం rustls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం rustls_ssl_backend_దత్తాంశం *)connssl->backend;
  (శూన్య)info;
  DEBUGASSERT(backend);
  తిరిగి backend->conn;
}

స్థిర CURLcode cr_shutdown(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి bool send_shutdown, bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం rustls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం rustls_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  ఒకవేళ(!backend->conn || cf->shutdown) {
    *పూర్తి = TRUE;
    వెళ్ళు బహిర్గతం;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *పూర్తి = FALSE;

  ఒకవేళ(!backend->పంపిన_shutdown) {
    /* do this only once */
    backend->పంపిన_shutdown = TRUE;
    ఒకవేళ(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  ఫలితం = cr_send(cf, దత్తాంశం, ఖాళీ, 0, &nwritten);
  ఒకవేళ(ఫలితం) {
    ఒకవేళ(ఫలితం == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ఫలితం = CURLE_OK;
      వెళ్ళు బహిర్గతం;
    }
    DEBUGASSERT(ఫలితం);
    CURL_TRC_CF(దత్తాంశం, cf, "shutdown send failed: %d", (పూర్ణాంకం)ఫలితం);
    వెళ్ళు బహిర్గతం;
  }

  కొరకు(i = 0; i < 10; ++i) {
    అక్షరం buf[1024];
    ఫలితం = cr_recv(cf, దత్తాంశం, buf, (పూర్ణాంకం)పరిమాణం(buf), &nread);
    ఒకవేళ(ఫలితం)
      ఆపు;
  }

  ఒకవేళ(ఫలితం == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ఫలితం = CURLE_OK;
  }
  లేకపోతే ఒకవేళ(ఫలితం) {
    DEBUGASSERT(ఫలితం);
    CURL_TRC_CF(దత్తాంశం, cf, "shutdown, error: %d", (పూర్ణాంకం)ఫలితం);
  }
  లేకపోతే ఒకవేళ(nread == 0) {
    /* We got the close notify alert and are done. */
    *పూర్తి = TRUE;
  }

బహిర్గతం:
  cf->shutdown = (ఫలితం || *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర శూన్య cr_close(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  స్థిరరాశి నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం rustls_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం rustls_ssl_backend_దత్తాంశం *)connssl->backend;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(backend);
  ఒకవేళ(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = ఖాళీ;
  }
  ఒకవేళ(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = ఖాళీ;
  }
}

స్థిర size_t cr_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత)
{
  స్థిరరాశి నిర్మాణం rustls_str ver = rustls_సంస్కరణ();
  తిరిగి curl_msnprintf(బఫరు, కొలత, "%.*s", (పూర్ణాంకం)ver.len, ver.దత్తాంశం);
}

స్థిర CURLcode cr_random(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని అక్షరం *entropy,
                          size_t నిడివి)
{
  rustls_ఫలితం rresult = 0;
  (శూన్య)దత్తాంశం;
  rresult = rustls_default_crypto_provider_random(entropy, నిడివి);
  తిరిగి map_error(rresult);
}

స్థిర శూన్య cr_cleanup(శూన్య)
{
  Curl_tls_keylog_close();
}

స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  పరిమాణం(నిర్మాణం rustls_ssl_backend_దత్తాంశం),

  ఖాళీ,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_సంస్కరణ,                      /*సంస్కరణ*/
  cr_shutdown,                     /* shutdown */
  cr_దత్తాంశం_pending,                 /* data_pending */
  cr_random,                       /* random */
  ఖాళీ,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  ఖాళీ,                            /* close_all */
  ఖాళీ,                            /* engines_list */
  ఖాళీ,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  ఖాళీ,                            /* get_channel_binding */
};

#అయితే_ముగింపు /* USE_RUSTLS */
