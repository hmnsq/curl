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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_RUSTLS

#अन्तर्भाव <rustls.h>

#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curlx/strerr.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "vdns/httpsrr.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_int.h"
#अन्तर्भाव "vtls/rustls.h"
#अन्तर्भाव "vtls/keylog.h"
#अन्तर्भाव "vtls/cipher_suite.h"
#अन्तर्भाव "vtls/x509asn1.h"
#यदि_परिभाषितम् USE_ECH
#अन्तर्भाव "curlx/base64.h"
#यदि_अन्तः

#यदि EAGAIN != EWOULDBLOCK
#परिभाषय RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#अन्यथा
#परिभाषय RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#यदि_अन्तः

संरचना rustls_ssl_backend_दत्त {
  ध्रुव संरचना rustls_client_config *config;
  संरचना rustls_connection *conn;
  size_t plain_बहिर्गमनम्_buffered;
  BIT(दत्त_अन्तर्_pending);
  BIT(प्रेषितम्_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
स्थिर CURLcode map_error(ध्रुव rustls_परिणाम r)
{
  यदि(rustls_परिणाम_is_cert_error(r)) {
    प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
  }
  विकल्प(r) {
  पक्ष RUSTLS_परिणाम_OK:
    प्रत्यागम CURLE_OK;
  पक्ष RUSTLS_परिणाम_NULL_PARAMETER:
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  स्वभाव:
    प्रत्यागम CURLE_RECV_ERROR;
  }
}

स्थिर शून्य rustls_failf(संरचना Curl_easy *दत्त, ध्रुव rustls_परिणाम rr,
                         ध्रुव वर्ण *सन्देशः)
{
  वर्ण errorbuf[STRERROR_LEN];
  size_t errorlen;
  rustls_error(rr, errorbuf, परिमाण(errorbuf), &errorlen);
  failf(दत्त, "%s: %.*s", सन्देशः, (पूर्णाङ्क)errorlen, errorbuf);
}

स्थिर bool cr_दत्त_pending(संरचना Curl_cfilter *cf,
                            ध्रुव संरचना Curl_easy *दत्त)
{
  ध्रुव संरचना ssl_connect_दत्त *ctx = cf->ctx;
  संरचना rustls_ssl_backend_दत्त *backend;

  (शून्य)दत्त;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (संरचना rustls_ssl_backend_दत्त *)ctx->backend;
  प्रत्यागम (bool)backend->दत्त_अन्तर्_pending;
}

संरचना io_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_easy *दत्त;
};

स्थिर पूर्णाङ्क read_cb(शून्य *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *बहिर्गमनम्_n)
{
  ध्रुव संरचना io_ctx *io_ctx = userdata;
  संरचना ssl_connect_दत्त * ध्रुव connssl = io_ctx->cf->ctx;
  CURLcode परिणाम;
  पूर्णाङ्क ret = 0;
  size_t nread;

  परिणाम = Curl_conn_cf_recv(io_ctx->cf->अग्र, io_ctx->दत्त,
                             (वर्ण *)buf, len, &nread);
  यदि(परिणाम) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    यदि(परिणाम == CURLE_AGAIN)
      ret = EAGAIN;
    अन्यथा
      ret = EINVAL;
  }
  अन्यथा यदि(nread == 0)
    connssl->peer_closed = TRUE;
  *बहिर्गमनम्_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->दत्त, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (size_t)len, (पूर्णाङ्क)परिणाम, nread);
  प्रत्यागम ret;
}

स्थिर पूर्णाङ्क write_cb(शून्य *userdata, ध्रुव uint8_t *buf, uintptr_t len,
                    uintptr_t *बहिर्गमनम्_n)
{
  ध्रुव संरचना io_ctx *io_ctx = userdata;
  CURLcode परिणाम;
  पूर्णाङ्क ret = 0;
  size_t nwritten;

  परिणाम = Curl_conn_cf_send(io_ctx->cf->अग्र, io_ctx->दत्त,
                             buf, len, FALSE, &nwritten);
  यदि(परिणाम) {
    nwritten = 0;
    यदि(परिणाम == CURLE_AGAIN)
      ret = EAGAIN;
    अन्यथा
      ret = EINVAL;
  }
  *बहिर्गमनम्_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->दत्त, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (पूर्णाङ्क)परिणाम, nwritten);
  प्रत्यागम ret;
}

स्थिर ssize_t tls_recv_अधिकम्(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त, CURLcode *err)
{
  ध्रुव संरचना ssl_connect_दत्त * ध्रुव connssl = cf->ctx;
  संरचना rustls_ssl_backend_दत्त * ध्रुव backend =
    (संरचना rustls_ssl_backend_दत्त *)connssl->backend;
  संरचना io_ctx io_ctx;
  size_t tls_अष्टकानि_read = 0;
  rustls_io_परिणाम io_error;
  rustls_परिणाम rresult = 0;

  io_ctx.cf = cf;
  io_ctx.दत्त = दत्त;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_अष्टकानि_read);
  यदि(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    प्रत्यागम -1;
  }
  अन्यथा यदि(io_error) {
    वर्ण बफर[STRERROR_LEN];
    failf(दत्त, "reading from socket: %s",
          curlx_strerror(io_error, बफर, परिमाण(बफर)));
    *err = CURLE_RECV_ERROR;
    प्रत्यागम -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  यदि(rresult != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    प्रत्यागम -1;
  }

  backend->दत्त_अन्तर्_pending = TRUE;
  *err = CURLE_OK;
  प्रत्यागम (ssize_t)tls_अष्टकानि_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
स्थिर CURLcode cr_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                        वर्ण *plainbuf, size_t plainlen, size_t *pnread)
{
  ध्रुव संरचना ssl_connect_दत्त * ध्रुव connssl = cf->ctx;
  संरचना rustls_ssl_backend_दत्त * ध्रुव backend =
    (संरचना rustls_ssl_backend_दत्त *)connssl->backend;
  संरचना rustls_connection *rconn = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  size_t n = 0;
  rustls_परिणाम rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  यावत्(*pnread < plainlen) {
    यदि(!backend->दत्त_अन्तर्_pending) {
      यदि(tls_recv_अधिकम्(cf, दत्त, &परिणाम) < 0) {
        यदि(परिणाम != CURLE_AGAIN) {
          गच्छ बहिर्गमनम्;
        }
        परिणाम = CURLE_OK;
        भङ्ग;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    यदि(rresult == RUSTLS_परिणाम_PLAINTEXT_रिक्तानि) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->दत्त_अन्तर्_pending = FALSE;
      भङ्ग;
    }
    अन्यथा यदि(rresult == RUSTLS_परिणाम_UNEXPECTED_EOF) {
      failf(दत्त, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      परिणाम = CURLE_RECV_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(rresult != RUSTLS_परिणाम_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(दत्त, rresult, "rustls_connection_read");
      परिणाम = CURLE_RECV_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      भङ्ग;
    }
    अन्यथा {
      *pnread += n;
    }
  }

  यदि(!eof && !*pnread) {
    परिणाम = CURLE_AGAIN;
  }

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (पूर्णाङ्क)परिणाम, *pnread);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cr_flush_बहिर्गमनम्(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                             संरचना rustls_connection *rconn)
{
  संरचना io_ctx io_ctx;
  rustls_io_परिणाम io_error;
  size_t tlswritten = 0;
  size_t tlswritten_योग = 0;

  io_ctx.cf = cf;
  io_ctx.दत्त = दत्त;

  यावत्(rustls_connection_इच्छति_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    यदि(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(दत्त, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_योग);
      प्रत्यागम CURLE_AGAIN;
    }
    अन्यथा यदि(io_error) {
      वर्ण बफर[STRERROR_LEN];
      failf(दत्त, "writing to socket: %s",
            curlx_strerror(io_error, बफर, परिमाण(बफर)));
      प्रत्यागम CURLE_SEND_ERROR;
    }
    यदि(tlswritten == 0) {
      failf(दत्त, "EOF in swrite");
      प्रत्यागम CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(दत्त, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_योग += tlswritten;
  }
  प्रत्यागम CURLE_OK;
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
स्थिर CURLcode cr_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                        ध्रुव शून्य *plainbuf, size_t plainlen,
                        size_t *pnwritten)
{
  ध्रुव संरचना ssl_connect_दत्त * ध्रुव connssl = cf->ctx;
  संरचना rustls_ssl_backend_दत्त * ध्रुव backend =
    (संरचना rustls_ssl_backend_दत्त *)connssl->backend;
  संरचना rustls_connection *rconn = रिक्त;
  size_t plainwritten = 0;
  ध्रुव अचिह्नित वर्ण *buf = plainbuf;
  CURLcode परिणाम = CURLE_OK;
  size_t blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(दत्त, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  यदि(backend->plain_बहिर्गमनम्_buffered) {
    परिणाम = cr_flush_बहिर्गमनम्(cf, दत्त, rconn);
    CURL_TRC_CF(दत्त, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_बहिर्गमनम्_buffered, (पूर्णाङ्क)परिणाम);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    यदि(blen > backend->plain_बहिर्गमनम्_buffered) {
      blen -= backend->plain_बहिर्गमनम्_buffered;
      buf += backend->plain_बहिर्गमनम्_buffered;
    }
    अन्यथा
      blen = 0;
    *pnwritten += backend->plain_बहिर्गमनम्_buffered;
    backend->plain_बहिर्गमनम्_buffered = 0;
  }

  यदि(blen > 0) {
    rustls_परिणाम rresult;
    CURL_TRC_CF(दत्त, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    यदि(rresult != RUSTLS_परिणाम_OK) {
      rustls_failf(दत्त, rresult, "rustls_connection_write");
      परिणाम = CURLE_WRITE_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(plainwritten == 0) {
      failf(दत्त, "rustls_connection_write: EOF");
      परिणाम = CURLE_WRITE_ERROR;
      गच्छ बहिर्गमनम्;
    }
  }

  परिणाम = cr_flush_बहिर्गमनम्(cf, दत्त, rconn);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_बहिर्गमनम्_buffered = plainwritten;
      यदि(*pnwritten) {
        परिणाम = CURLE_OK;
      }
    }
    गच्छ बहिर्गमनम्;
  }
  अन्यथा
    *pnwritten += plainwritten;

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (पूर्णाङ्क)परिणाम, *pnwritten);
  प्रत्यागम परिणाम;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
स्थिर uint32_t cr_verify_none(शून्य *userdata,
                               ध्रुव rustls_verify_server_cert_params *params)
{
  (शून्य)userdata;
  (शून्य)params;
  प्रत्यागम RUSTLS_परिणाम_OK;
}

स्थिर पूर्णाङ्क read_file_into(ध्रुव वर्ण *filename, संरचना dynbuf *बहिर्गमनम्)
{
  FILE *f = curlx_fopen(filename, FOPEN_READTEXT);
  यदि(!f) {
    प्रत्यागम 0;
  }

  प्रति(;;) {
    uint8_t buf[256];
    ध्रुव size_t rr = fread(buf, 1, परिमाण(buf), f);
    यदि((!rr && !feof(f)) ||
       curlx_dyn_addn(बहिर्गमनम्, buf, rr)) {
      curlx_fclose(f);
      प्रत्यागम 0;
    }
    यदि(rr < परिमाण(buf))
      भङ्ग;
  }

  प्रत्यागम curlx_fclose(f) == 0;
}

स्थिर शून्य
cr_get_selected_ciphers(संरचना Curl_easy *दत्त,
                        ध्रुव वर्ण *ciphers12,
                        ध्रुव वर्ण *ciphers13,
                        ध्रुव संरचना rustls_supported_ciphersuite **selected,
                        size_t *selected_आकार)
{
  ध्रुव size_t supported_len = *selected_आकार;
  ध्रुव size_t default_len = rustls_default_crypto_provider_ciphersuites_len();
  ध्रुव संरचना rustls_supported_ciphersuite *प्रविष्टिः = रिक्त;
  ध्रुव वर्ण *ciphers = ciphers12;
  size_t गणनम् = 0, default13_गणनम् = 0, i, j;
  ध्रुव वर्ण *ptr, *अन्तः;

  DEBUGASSERT(default_len <= supported_len);

  यदि(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    प्रति(j = 0; j < default_len; j++) {
      प्रविष्टिः = rustls_default_crypto_provider_ciphersuites_get(j);
      यदि(rustls_supported_ciphersuite_protocol_संस्करणम्(प्रविष्टिः) !=
         RUSTLS_TLS_संस्करणम्_TLSV1_3)
        अनुवर्त;

      selected[गणनम्++] = प्रविष्टिः;
    }

    default13_गणनम् = गणनम्;

    यदि(!ciphers)
      ciphers = "";
  }
  अन्यथा
    ciphers = ciphers13;

सङ्कलय_ciphers:
  प्रति(ptr = ciphers; ptr[0] != '\0' && गणनम् < supported_len; ptr = अन्तः) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &अन्तः);

    /* Check if cipher is supported */
    यदि(id) {
      प्रति(i = 0; i < supported_len; i++) {
        प्रविष्टिः = rustls_default_crypto_provider_ciphersuites_get(i);
        यदि(rustls_supported_ciphersuite_get_suite(प्रविष्टिः) == id)
          भङ्ग;
      }
      यदि(i == supported_len)
        id = 0;
    }
    यदि(!id) {
      यदि(ptr[0] != '\0')
        infof(दत्त, "rustls: unknown cipher in list: \"%.*s\"",
              (पूर्णाङ्क)(अन्तः - ptr), ptr);
      अनुवर्त;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    प्रति(i = 0; i < गणनम् && selected[i] != प्रविष्टिः; i++)
      ;
    यदि(i < गणनम्) {
      यदि(i >= default13_गणनम्)
        infof(दत्त, "rustls: duplicate cipher in list: \"%.*s\"",
              (पूर्णाङ्क)(अन्तः - ptr), ptr);
      अनुवर्त;
    }

    selected[गणनम्++] = प्रविष्टिः;
  }

  यदि(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    गच्छ सङ्कलय_ciphers;
  }

  यदि(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    प्रति(j = 0; j < default_len; j++) {
      प्रविष्टिः = rustls_default_crypto_provider_ciphersuites_get(j);
      यदि(rustls_supported_ciphersuite_protocol_संस्करणम्(प्रविष्टिः) ==
         RUSTLS_TLS_संस्करणम्_TLSV1_3)
        अनुवर्त;

      /* No duplicates allowed (so selected cannot overflow) */
      प्रति(i = 0; i < गणनम् && selected[i] != प्रविष्टिः; i++)
        ;
      यदि(i < गणनम्)
        अनुवर्त;

      selected[गणनम्++] = प्रविष्टिः;
    }
  }

  *selected_आकार = गणनम्;
}

स्थिर शून्य cr_keylog_log_cb(संरचना rustls_str label,
                             ध्रुव uint8_t *client_random,
                             size_t client_random_len, ध्रुव uint8_t *secret,
                             size_t secret_len)
{
  वर्ण clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_आकार);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, परिमाण(clabel), "%.*s", (पूर्णाङ्क)label.len, label.दत्त);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

स्थिर CURLcode
init_config_builder(संरचना Curl_cfilter *cf,
                    संरचना Curl_easy *दत्त,
                    ध्रुव संरचना ssl_filter_config *conn_config,
                    संरचना rustls_client_config_builder **config_builder)
{
  ध्रुव संरचना rustls_supported_ciphersuite **cipher_suites = रिक्त;
  संरचना rustls_crypto_provider_builder *custom_provider_builder = रिक्त;
  ध्रुव संरचना rustls_crypto_provider *custom_provider = रिक्त;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_संस्करणम्_TLSV1_2,
    RUSTLS_TLS_संस्करणम्_TLSV1_3,
  };
  size_t tls_versions_len = 2;
  size_t cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode परिणाम = CURLE_OK;
  rustls_परिणाम rr;

  DEBUGASSERT(conn_config->संस्करणम्_2 != CURL_SSLVERSION_DEFAULT);
  विकल्प(conn_config->संस्करणम्_2) {
  पक्ष CURL_SSLVERSION_TLSv1:
  पक्ष CURL_SSLVERSION_TLSv1_0:
  पक्ष CURL_SSLVERSION_TLSv1_1:
  पक्ष CURL_SSLVERSION_TLSv1_2:
    भङ्ग;
  पक्ष CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_संस्करणम्_TLSV1_3;
    tls_versions_len = 1;
    भङ्ग;
  स्वभाव:
    failf(दत्त, "rustls: unsupported minimum TLS version value");
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ cleanup;
  }

  विकल्प(conn_config->संस्करणम्_max) {
  पक्ष CURL_SSLVERSION_MAX_DEFAULT:
  पक्ष CURL_SSLVERSION_MAX_NONE:
  पक्ष CURL_SSLVERSION_MAX_TLSv1_3:
    भङ्ग;
  पक्ष CURL_SSLVERSION_MAX_TLSv1_2:
    यदि(tls_versions[0] == RUSTLS_TLS_संस्करणम्_TLSV1_2) {
      tls_versions_len = 1;
      भङ्ग;
    }
    FALLTHROUGH();
  पक्ष CURL_SSLVERSION_MAX_TLSv1_1:
  पक्ष CURL_SSLVERSION_MAX_TLSv1_0:
  स्वभाव:
    failf(दत्त, "rustls: unsupported maximum TLS version value");
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ cleanup;
  }

#यदि_परिभाषितम् USE_ECH
  यदि(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_संस्करणम्_TLSV1_3;
    tls_versions_len = 1;
    infof(दत्त, "rustls: ECH enabled, forcing TLSv1.3");
  }
#अन्यथा
  (शून्य)cf;
#यदि_अन्तः /* USE_ECH */

  cipher_suites = curlx_malloc(परिमाण(*cipher_suites) * cipher_suites_len);
  यदि(!cipher_suites) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ cleanup;
  }

  cr_get_selected_ciphers(दत्त,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  यदि(cipher_suites_len == 0) {
    failf(दत्त, "rustls: no supported cipher in list");
    परिणाम = CURLE_SSL_CIPHER;
    गच्छ cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr,
                 "failed to create crypto provider builder from default");
    परिणाम = CURLE_SSL_CIPHER;
    गच्छ cleanup;
  }

  rr =
    rustls_crypto_provider_builder_समुच्चयः_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr,
                 "failed to set ciphersuites for crypto provider builder");
    परिणाम = CURLE_SSL_CIPHER;
    गच्छ cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "failed to build custom crypto provider");
    परिणाम = CURLE_SSL_CIPHER;
    गच्छ cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "failed to create client config builder");
    परिणाम = CURLE_SSL_CIPHER;
    गच्छ cleanup;
  }

cleanup:
  यदि(cipher_suites) {
    curlx_free(cipher_suites);
  }
  यदि(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  यदि(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य
init_config_builder_alpn(संरचना Curl_easy *दत्त,
                         ध्रुव संरचना ssl_connect_दत्त *connssl,
                         संरचना rustls_client_config_builder *config_builder)
{
  संरचना alpn_proto_buf proto;
  rustls_खण्डिका_अष्टकानि alpn[ALPN_ENTRIES_MAX];
  size_t i;

  प्रति(i = 0; i < connssl->alpn->गणनम्; ++i) {
    alpn[i].दत्त = (ध्रुव uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = वाक्यदैर्घ्यम्(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_समुच्चयः_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->गणनम्);
  Curl_alpn_गन्तव्यम्_proto_str(&proto, connssl->alpn);
  infof(दत्त, VTLS_INFOF_ALPN_OFFER_1STR, proto.दत्त);
}

स्थिर CURLcode init_config_builder_verifier_crl(
  संरचना Curl_easy *दत्त,
  ध्रुव संरचना ssl_filter_config *conn_config,
  संरचना rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode परिणाम = CURLE_OK;
  संरचना dynbuf crl_contents;
  rustls_परिणाम rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_आकार);
  यदि(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(दत्त, "rustls: failed to read revocation list file");
    परिणाम = CURLE_SSL_CRL_BADFILE;
    गच्छ cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_सङ्कलय_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "failed to parse revocation list");
    परिणाम = CURLE_SSL_CRL_BADFILE;
    गच्छ cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode
init_config_builder_verifier(संरचना Curl_easy *दत्त,
                             संरचना rustls_client_config_builder *builder,
                             ध्रुव संरचना ssl_filter_config *conn_config,
                             ध्रुव संरचना curl_blob *ca_info_blob,
                             ध्रुव वर्ण * ध्रुव ssl_cafile)
{
  ध्रुव संरचना rustls_मूलम्_cert_store *roots = रिक्त;
  संरचना rustls_मूलम्_cert_store_builder *roots_builder = रिक्त;
  संरचना rustls_web_pki_server_cert_verifier_builder *verifier_builder = रिक्त;
  संरचना rustls_server_cert_verifier *server_cert_verifier = रिक्त;
  rustls_परिणाम rr = RUSTLS_परिणाम_OK;
  CURLcode परिणाम = CURLE_OK;

  roots_builder = rustls_मूलम्_cert_store_builder_new();
  यदि(ca_info_blob) {
    rr = rustls_मूलम्_cert_store_builder_सङ्कलय_pem(roots_builder,
                                                ca_info_blob->दत्त,
                                                ca_info_blob->len,
                                                1);
    यदि(rr != RUSTLS_परिणाम_OK) {
      rustls_failf(दत्त, rr, "failed to parse trusted certificates from blob");
      परिणाम = CURLE_SSL_CACERT_BADFILE;
      गच्छ cleanup;
    }
  }
  अन्यथा यदि(ssl_cafile) {
    rr = rustls_मूलम्_cert_store_builder_आरोपय_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    यदि(rr != RUSTLS_परिणाम_OK) {
      rustls_failf(दत्त, rr, "failed to load trusted certificates");
      परिणाम = CURLE_SSL_CACERT_BADFILE;
      गच्छ cleanup;
    }
  }

  rr = rustls_मूलम्_cert_store_builder_build(roots_builder, &roots);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "failed to build trusted root certificate store");
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  यदि(!verifier_builder) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ cleanup;
  }

  यदि(conn_config->CRLfile) {
    परिणाम = init_config_builder_verifier_crl(दत्त,
                                              conn_config,
                                              verifier_builder);
    यदि(परिणाम) {
      गच्छ cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "failed to build certificate verifier");
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  rustls_client_config_builder_समुच्चयः_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  यदि(roots_builder) {
    rustls_मूलम्_cert_store_builder_free(roots_builder);
  }
  यदि(roots) {
    rustls_मूलम्_cert_store_free(roots);
  }
  यदि(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  यदि(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode init_config_builder_platform_verifier(
  संरचना Curl_easy *दत्त,
  संरचना rustls_client_config_builder *builder)
{
  संरचना rustls_server_cert_verifier *server_cert_verifier = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  rustls_परिणाम rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "failed to create platform certificate verifier");
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  rustls_client_config_builder_समुच्चयः_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  यदि(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode
init_config_builder_keylog(संरचना Curl_easy *दत्त,
                           संरचना rustls_client_config_builder *builder)
{
  rustls_परिणाम rr;

  Curl_tls_keylog_open();
  यदि(!Curl_tls_keylog_enabled()) {
    प्रत्यागम CURLE_OK;
  }

  rr = rustls_client_config_builder_समुच्चयः_कुञ्चिका_log(builder,
                                                cr_keylog_log_cb,
                                                रिक्त);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    प्रत्यागम map_error(rr);
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode
init_config_builder_client_auth(संरचना Curl_easy *दत्त,
                                ध्रुव संरचना ssl_filter_config *conn_config,
                                संरचना rustls_client_config_builder *builder)
{
  संरचना dynbuf cert_contents;
  संरचना dynbuf कुञ्चिका_contents;
  rustls_परिणाम rr;
  ध्रुव संरचना rustls_certified_कुञ्चिका *certified_कुञ्चिका = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  ध्रुव bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  ध्रुव bool have_कुञ्चिका = conn_config->कुञ्चिका || conn_config->कुञ्चिका_blob;

  यदि(have_cert && !have_कुञ्चिका) {
    failf(दत्त, "rustls: must provide key with certificate");
    प्रत्यागम CURLE_SSL_CERTPROBLEM;
  }
  अन्यथा यदि(!have_cert && have_कुञ्चिका) {
    failf(दत्त, "rustls: must provide certificate with key");
    प्रत्यागम CURLE_SSL_CERTPROBLEM;
  }

  यदि(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(दत्त, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    प्रत्यागम CURLE_SSL_CERTPROBLEM;
  }
  यदि(conn_config->कुञ्चिका_type && !curl_strequal(conn_config->कुञ्चिका_type, "PEM")) {
    failf(दत्त, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->कुञ्चिका_type);
    प्रत्यागम CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_आकार);
  curlx_dyn_init(&कुञ्चिका_contents, DYN_KEYFILE_आकार);

  यदि(conn_config->cert_blob) {
    परिणाम = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->दत्त,
                            conn_config->cert_blob->len);
    यदि(परिणाम) {
      failf(दत्त, "rustls: failed to copy client certificate blob (%s)",
            परिणाम == CURLE_TOO_LARGE ? "too large" : "out of memory");
      गच्छ cleanup;
    }
  }
  अन्यथा यदि(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(दत्त, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    परिणाम = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

  यदि(conn_config->कुञ्चिका_blob) {
    परिणाम = curlx_dyn_addn(&कुञ्चिका_contents,
                            conn_config->कुञ्चिका_blob->दत्त,
                            conn_config->कुञ्चिका_blob->len);
    यदि(परिणाम) {
      failf(दत्त, "rustls: failed to copy key blob (%s)",
            परिणाम == CURLE_TOO_LARGE ? "too large" : "out of memory");
      गच्छ cleanup;
    }
  }
  अन्यथा यदि(!read_file_into(conn_config->कुञ्चिका, &कुञ्चिका_contents)) {
    failf(दत्त, "rustls: failed to read key file: '%s'",
          conn_config->कुञ्चिका);
    परिणाम = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

  rr = rustls_certified_कुञ्चिका_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&कुञ्चिका_contents),
                                  curlx_dyn_len(&कुञ्चिका_contents),
                                  &certified_कुञ्चिका);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "rustls: failed to build certified key");
    परिणाम = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

  rr = rustls_certified_कुञ्चिका_कुञ्चिकाः_मेलनम्(certified_कुञ्चिका);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr,
                 "rustls: client certificate and keypair files do not match:");
    परिणाम = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

  rr = rustls_client_config_builder_समुच्चयः_certified_कुञ्चिका(builder,
                                                      &certified_कुञ्चिका,
                                                      1);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "rustls: failed to set certified key");
    परिणाम = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&कुञ्चिका_contents);
  यदि(certified_कुञ्चिका) {
    rustls_certified_कुञ्चिका_free(certified_कुञ्चिका);
  }
  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् USE_ECH

स्थिर CURLcode
init_config_builder_ech(संरचना Curl_easy *दत्त,
                        संरचना Curl_cfilter *cf,
                        संरचना rustls_client_config_builder *builder)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ध्रुव rustls_hpke *hpke = rustls_supported_hpke();
  अचिह्नित वर्ण *ech_config = रिक्त;
  size_t ech_config_len = 0;
  संरचना Curl_dns_प्रविष्टिः *dns = रिक्त;
  CURLcode परिणाम = CURLE_OK;
  rustls_परिणाम rr;

  यदि(!hpke) {
    failf(दत्त,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    गच्छ cleanup;
  }

  यदि(conn_config->ech_लौकिकम्) {
    failf(दत्त, "rustls: ECH outername not supported");
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    गच्छ cleanup;
  }

  यदि(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    यदि(rr != RUSTLS_परिणाम_OK) {
      rustls_failf(दत्त, rr, "rustls: failed to configure ECH GREASE");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ cleanup;
    }
    प्रत्यागम CURLE_OK;
  }

  यदि(conn_config->ech && conn_config->ech_config) {
    ध्रुव वर्ण *b64 = conn_config->ech_config;
    size_t decode_परिणाम;
    यदि(!b64[0]) {
      infof(दत्त, "rustls: ECHConfig from command line empty");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_परिणाम = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    यदि(decode_परिणाम || !ech_config) {
      infof(दत्त, "rustls: cannot base64 decode ECHConfig from command line");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ cleanup;
    }
  }
  अन्यथा {
    ध्रुव संरचना ssl_connect_दत्त *connssl = cf->ctx;
    ध्रुव संरचना Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(दत्त, cf->sockindex, connssl->peer.origin);

    यदि(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(दत्त, "rustls: ECH requested but no ECHConfig available");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "rustls: failed to configure ECH");
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    गच्छ cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  यदि(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  यदि(dns) {
    Curl_dns_प्रविष्टिः_unlink(दत्त, &dns);
  }
  प्रत्यागम परिणाम;
}
#यदि_अन्तः /* USE_ECH */

स्थिर CURLcode cr_init_backend(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                संरचना rustls_ssl_backend_दत्त * ध्रुव backend)
{
  ध्रुव संरचना ssl_connect_दत्त *connssl = cf->ctx;
  ध्रुव संरचना ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  संरचना rustls_connection *rconn = रिक्त;
  संरचना rustls_client_config_builder *config_builder = रिक्त;

  ध्रुव संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ध्रुव वर्ण * ध्रुव ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? रिक्त : conn_config->CAfile);
  CURLcode परिणाम = CURLE_OK;
  rustls_परिणाम rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  परिणाम = init_config_builder(cf, दत्त, conn_config, &config_builder);
  यदि(परिणाम != CURLE_OK) {
    प्रत्यागम परिणाम;
  }

  यदि(connssl->alpn) {
    init_config_builder_alpn(दत्त, connssl, config_builder);
  }

  यदि(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_समुच्चयः_certificate_verifier(
      config_builder, cr_verify_none);
  }
  अन्यथा यदि(conn_config->native_ca_store) {
    यदि(conn_config->CRLfile) {
      failf(दत्त, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
    }
    परिणाम = init_config_builder_platform_verifier(दत्त, config_builder);
    यदि(परिणाम != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      प्रत्यागम परिणाम;
    }
  }
  अन्यथा यदि(ca_info_blob || ssl_cafile) {
    परिणाम = init_config_builder_verifier(दत्त,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    यदि(परिणाम != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      प्रत्यागम परिणाम;
    }
  }

  यदि(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->कुञ्चिका || conn_config->कुञ्चिका_blob) {
    परिणाम = init_config_builder_client_auth(दत्त,
                                             conn_config,
                                             config_builder);
    यदि(परिणाम != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      प्रत्यागम परिणाम;
    }
  }

#यदि_परिभाषितम् USE_ECH
  यदि(Curl_ssl_ech_enabled(cf)) {
    परिणाम = init_config_builder_ech(दत्त, cf, config_builder);
    यदि((परिणाम != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      प्रत्यागम परिणाम;
    }
  }
#यदि_अन्तः /* USE_ECH */

  परिणाम = init_config_builder_keylog(दत्त, config_builder);
  यदि(परिणाम != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    प्रत्यागम परिणाम;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "failed to build client config");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  यदि(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(दत्त, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = रिक्त;
    प्रत्यागम CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_समुच्चयः_userdata(rconn, backend);
  backend->conn = rconn;

  प्रत्यागम परिणाम;
}

स्थिर शून्य cr_समुच्चयः_negotiated_alpn(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त,
                                   ध्रुव संरचना rustls_connection *rconn)
{
  संरचना ssl_connect_दत्त * ध्रुव connssl = cf->ctx;
  ध्रुव uint8_t *protocol = रिक्त;
  size_t len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_समुच्चयः_negotiated(cf, दत्त, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
स्थिर CURLcode cr_connect(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                           bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त * ध्रुव connssl = cf->ctx;
  ध्रुव संरचना rustls_ssl_backend_दत्त * ध्रुव backend =
    (संरचना rustls_ssl_backend_दत्त *)connssl->backend;
  ध्रुव संरचना rustls_connection *rconn = रिक्त;
  CURLcode tmperr = CURLE_OK;
  CURLcode परिणाम;
  bool इच्छति_read;
  bool इच्छति_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(दत्त, cf, "cr_connect, state=%d", (पूर्णाङ्क)connssl->state);
  *समाप्तम् = FALSE;

#यदि_परिभाषितम् USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    यदि(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(दत्त, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(दत्त, cf, "need HTTPS-RR for ECH, delaying connect");
      प्रत्यागम CURLE_OK;
    }
#यदि_अन्तः /* USE_ECH */

  यदि(!backend->conn) {
    परिणाम =
      cr_init_backend(cf, दत्त,
                      (संरचना rustls_ssl_backend_दत्त *)connssl->backend);
    CURL_TRC_CF(दत्त, cf, "cr_connect, init backend -> %d", (पूर्णाङ्क)परिणाम);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  प्रति(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    यदि(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      size_t nwritten;
      cr_समुच्चयः_negotiated_alpn(cf, दत्त, rconn);
      tmperr = cr_send(cf, दत्त, रिक्त, 0, &nwritten);
      यदि(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        प्रत्यागम CURLE_OK;
      }
      अन्यथा यदि(tmperr != CURLE_OK) {
        प्रत्यागम tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#यदि_परिभाषितम् CURLVERBOSE
        ध्रुव uint16_t proto = rustls_connection_get_protocol_संस्करणम्(rconn);
        ध्रुव rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        ध्रुव rustls_str kex_गणः_name =
          rustls_connection_get_negotiated_कुञ्चिका_exchange_गणः_name(rconn);
        ध्रुव वर्ण *ver = "TLS version unknown";
        यदि(proto == RUSTLS_TLS_संस्करणम्_TLSV1_3)
          ver = "TLSv1.3";
        यदि(proto == RUSTLS_TLS_संस्करणम्_TLSV1_2)
          ver = "TLSv1.2";
#यदि_अन्तः
        infof(दत्त,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (पूर्णाङ्क)ciphersuite_name.len,
              ciphersuite_name.दत्त,
              (पूर्णाङ्क)kex_गणः_name.len,
              kex_गणः_name.दत्त);
      }
      यदि(दत्त->समुच्चयः.ssl.certinfo) {
        size_t अङ्कः_certs = 0;
        size_t i;
        यावत्(rustls_connection_get_peer_certificate(rconn, अङ्कः_certs)) {
          अङ्कः_certs++;
          यदि(अङ्कः_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(दत्त, "%zu certificates is more than allowed (%d)",
                  अङ्कः_certs, MAX_ALLOWED_CERT_AMOUNT);
            प्रत्यागम CURLE_SSL_CONNECT_ERROR;
          }
        }
        परिणाम = Curl_ssl_init_certinfo(दत्त, (पूर्णाङ्क)अङ्कः_certs);
        यदि(परिणाम)
          प्रत्यागम परिणाम;
        प्रति(i = 0; i < अङ्कः_certs; i++) {
          ध्रुव rustls_certificate *cert;
          ध्रुव अचिह्नित वर्ण *der_दत्त;
          size_t der_len;
          rustls_परिणाम rresult = RUSTLS_परिणाम_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_दत्त, &der_len);
          यदि(rresult != RUSTLS_परिणाम_OK) {
            वर्ण errorbuf[255];
            size_t errorlen;
            rustls_error(rresult, errorbuf, परिमाण(errorbuf), &errorlen);
            failf(दत्त,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (पूर्णाङ्क)errorlen, errorbuf);
            प्रत्यागम map_error(rresult);
          }
          परिणाम = Curl_extract_certinfo(दत्त, (पूर्णाङ्क)i,
                                         der_दत्त, der_दत्त + der_len);
          यदि(परिणाम)
            प्रत्यागम परिणाम;
        }
      }

      connssl->state = ssl_connection_complete;
      *समाप्तम् = TRUE;
      प्रत्यागम CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    इच्छति_read = rustls_connection_इच्छति_read(rconn);
    इच्छति_write = rustls_connection_इच्छति_write(rconn) ||
                  backend->plain_बहिर्गमनम्_buffered;
    DEBUGASSERT(इच्छति_read || इच्छति_write);

    यदि(इच्छति_write) {
      size_t nwritten;
      CURL_TRC_CF(दत्त, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, दत्त, रिक्त, 0, &nwritten);
      यदि(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(दत्त, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        प्रत्यागम CURLE_OK;
      }
      अन्यथा यदि(tmperr != CURLE_OK) {
        प्रत्यागम tmperr;
      }
    }

    यदि(इच्छति_read) {
      CURL_TRC_CF(दत्त, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_अधिकम्(cf, दत्त, &tmperr);
      यदि(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(दत्त, "TLS connect error: Connection closed abruptly");
        प्रत्यागम CURLE_SSL_CONNECT_ERROR;
      }
      यदि(nread < 0) {
        यदि(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(दत्त, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          प्रत्यागम CURLE_OK;
        }
        अन्यथा यदि(tmperr == CURLE_RECV_ERROR) {
          प्रत्यागम CURLE_SSL_CONNECT_ERROR;
        }
        अन्यथा {
          प्रत्यागम tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

स्थिर शून्य *cr_get_internals(संरचना ssl_connect_दत्त *connssl, CURLINFO info)
{
  संरचना rustls_ssl_backend_दत्त *backend =
    (संरचना rustls_ssl_backend_दत्त *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  प्रत्यागम backend->conn;
}

स्थिर CURLcode cr_shutdown(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                            ध्रुव bool send_shutdown, bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना rustls_ssl_backend_दत्त *backend =
    (संरचना rustls_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  size_t i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  यदि(!backend->conn || cf->shutdown) {
    *समाप्तम् = TRUE;
    गच्छ बहिर्गमनम्;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *समाप्तम् = FALSE;

  यदि(!backend->प्रेषितम्_shutdown) {
    /* do this only once */
    backend->प्रेषितम्_shutdown = TRUE;
    यदि(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  परिणाम = cr_send(cf, दत्त, रिक्त, 0, &nwritten);
  यदि(परिणाम) {
    यदि(परिणाम == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      परिणाम = CURLE_OK;
      गच्छ बहिर्गमनम्;
    }
    DEBUGASSERT(परिणाम);
    CURL_TRC_CF(दत्त, cf, "shutdown send failed: %d", (पूर्णाङ्क)परिणाम);
    गच्छ बहिर्गमनम्;
  }

  प्रति(i = 0; i < 10; ++i) {
    वर्ण buf[1024];
    परिणाम = cr_recv(cf, दत्त, buf, (पूर्णाङ्क)परिमाण(buf), &nread);
    यदि(परिणाम)
      भङ्ग;
  }

  यदि(परिणाम == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    परिणाम = CURLE_OK;
  }
  अन्यथा यदि(परिणाम) {
    DEBUGASSERT(परिणाम);
    CURL_TRC_CF(दत्त, cf, "shutdown, error: %d", (पूर्णाङ्क)परिणाम);
  }
  अन्यथा यदि(nread == 0) {
    /* We got the close notify alert and are done. */
    *समाप्तम् = TRUE;
  }

बहिर्गमनम्:
  cf->shutdown = (परिणाम || *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर शून्य cr_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  ध्रुव संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना rustls_ssl_backend_दत्त *backend =
    (संरचना rustls_ssl_backend_दत्त *)connssl->backend;

  (शून्य)दत्त;
  DEBUGASSERT(backend);
  यदि(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = रिक्त;
  }
  यदि(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = रिक्त;
  }
}

स्थिर size_t cr_संस्करणम्(वर्ण *बफर, size_t आकार)
{
  ध्रुव संरचना rustls_str ver = rustls_संस्करणम्();
  प्रत्यागम curl_msnprintf(बफर, आकार, "%.*s", (पूर्णाङ्क)ver.len, ver.दत्त);
}

स्थिर CURLcode cr_random(संरचना Curl_easy *दत्त, अचिह्नित वर्ण *entropy,
                          size_t दीर्घता)
{
  rustls_परिणाम rresult = 0;
  (शून्य)दत्त;
  rresult = rustls_default_crypto_provider_random(entropy, दीर्घता);
  प्रत्यागम map_error(rresult);
}

स्थिर शून्य cr_cleanup(शून्य)
{
  Curl_tls_keylog_close();
}

ध्रुव संरचना Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  परिमाण(संरचना rustls_ssl_backend_दत्त),

  रिक्त,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_संस्करणम्,                      /*संस्करणम्*/
  cr_shutdown,                     /* shutdown */
  cr_दत्त_pending,                 /* data_pending */
  cr_random,                       /* random */
  रिक्त,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  रिक्त,                            /* close_all */
  रिक्त,                            /* engines_list */
  रिक्त,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  रिक्त,                            /* get_channel_binding */
};

#यदि_अन्तः /* USE_RUSTLS */
