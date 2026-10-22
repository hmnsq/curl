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

#ifdef USE_RUSTLS

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
#ifdef USE_ECH
#अन्तर्भाव "curlx/base64.h"
#endif

#यदि EAGAIN != EWOULDBLOCK
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#अन्यथा
#define RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#endif

संरचना rustls_ssl_backend_data {
  अचर संरचना rustls_client_config *config;
  संरचना rustls_connection *conn;
  माप_प्रकार plain_out_buffered;
  BIT(data_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
स्थिर CURLcode map_error(अचर rustls_result r)
{
  यदि(rustls_result_is_cert_error(r)) {
    फर्काऊ CURLE_PEER_FAILED_VERIFICATION;
  }
  विकल्प(r) {
  पक्ष RUSTLS_RESULT_OK:
    फर्काऊ CURLE_OK;
  पक्ष RUSTLS_RESULT_NULL_PARAMETER:
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;
  स्वभाव:
    फर्काऊ CURLE_RECV_ERROR;
  }
}

स्थिर शून्य rustls_failf(संरचना Curl_easy *data, अचर rustls_result rr,
                         अचर वर्ण *सन्देश)
{
  वर्ण errorbuf[STRERROR_LEN];
  माप_प्रकार errorlen;
  rustls_error(rr, errorbuf, परिमाण(errorbuf), &errorlen);
  failf(data, "%s: %.*s", सन्देश, (पूर्णाङ्क)errorlen, errorbuf);
}

स्थिर bool cr_data_pending(संरचना Curl_cfilter *cf,
                            अचर संरचना Curl_easy *data)
{
  अचर संरचना ssl_connect_data *ctx = cf->ctx;
  संरचना rustls_ssl_backend_data *backend;

  (शून्य)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (संरचना rustls_ssl_backend_data *)ctx->backend;
  फर्काऊ (bool)backend->data_in_pending;
}

संरचना io_ctx {
  संरचना Curl_cfilter *cf;
  संरचना Curl_easy *data;
};

स्थिर पूर्णाङ्क read_cb(शून्य *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  अचर संरचना io_ctx *io_ctx = userdata;
  संरचना ssl_connect_data * अचर connssl = io_ctx->cf->ctx;
  CURLcode result;
  पूर्णाङ्क ret = 0;
  माप_प्रकार nread;

  result = Curl_conn_cf_recv(io_ctx->cf->next, io_ctx->data,
                             (वर्ण *)buf, len, &nread);
  यदि(result) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    यदि(result == CURLE_AGAIN)
      ret = EAGAIN;
    अन्यथा
      ret = EINVAL;
  }
  अन्यथा यदि(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (माप_प्रकार)len, (पूर्णाङ्क)result, nread);
  फर्काऊ ret;
}

स्थिर पूर्णाङ्क write_cb(शून्य *userdata, अचर uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  अचर संरचना io_ctx *io_ctx = userdata;
  CURLcode result;
  पूर्णाङ्क ret = 0;
  माप_प्रकार nwritten;

  result = Curl_conn_cf_send(io_ctx->cf->next, io_ctx->data,
                             buf, len, FALSE, &nwritten);
  यदि(result) {
    nwritten = 0;
    यदि(result == CURLE_AGAIN)
      ret = EAGAIN;
    अन्यथा
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->data, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (पूर्णाङ्क)result, nwritten);
  फर्काऊ ret;
}

स्थिर ssize_t tls_recv_more(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data, CURLcode *err)
{
  अचर संरचना ssl_connect_data * अचर connssl = cf->ctx;
  संरचना rustls_ssl_backend_data * अचर backend =
    (संरचना rustls_ssl_backend_data *)connssl->backend;
  संरचना io_ctx io_ctx;
  माप_प्रकार tls_bytes_read = 0;
  rustls_io_result io_error;
  rustls_result rresult = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  यदि(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    फर्काऊ -1;
  }
  अन्यथा यदि(io_error) {
    वर्ण buffer[STRERROR_LEN];
    failf(data, "reading from socket: %s",
          curlx_strerror(io_error, buffer, परिमाण(buffer)));
    *err = CURLE_RECV_ERROR;
    फर्काऊ -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  यदि(rresult != RUSTLS_RESULT_OK) {
    rustls_failf(data, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    फर्काऊ -1;
  }

  backend->data_in_pending = TRUE;
  *err = CURLE_OK;
  फर्काऊ (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
स्थिर CURLcode cr_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                        वर्ण *plainbuf, माप_प्रकार plainlen, माप_प्रकार *pnread)
{
  अचर संरचना ssl_connect_data * अचर connssl = cf->ctx;
  संरचना rustls_ssl_backend_data * अचर backend =
    (संरचना rustls_ssl_backend_data *)connssl->backend;
  संरचना rustls_connection *rconn = रिक्त;
  CURLcode result = CURLE_OK;
  माप_प्रकार n = 0;
  rustls_result rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  जबसम्म(*pnread < plainlen) {
    यदि(!backend->data_in_pending) {
      यदि(tls_recv_more(cf, data, &result) < 0) {
        यदि(result != CURLE_AGAIN) {
          गच्छ out;
        }
        result = CURLE_OK;
        रोक;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    यदि(rresult == RUSTLS_RESULT_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->data_in_pending = FALSE;
      रोक;
    }
    अन्यथा यदि(rresult == RUSTLS_RESULT_UNEXPECTED_EOF) {
      failf(data, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      result = CURLE_RECV_ERROR;
      गच्छ out;
    }
    अन्यथा यदि(rresult != RUSTLS_RESULT_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(data, rresult, "rustls_connection_read");
      result = CURLE_RECV_ERROR;
      गच्छ out;
    }
    अन्यथा यदि(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      रोक;
    }
    अन्यथा {
      *pnread += n;
    }
  }

  यदि(!eof && !*pnread) {
    result = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(data, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (पूर्णाङ्क)result, *pnread);
  फर्काऊ result;
}

स्थिर CURLcode cr_flush_out(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                             संरचना rustls_connection *rconn)
{
  संरचना io_ctx io_ctx;
  rustls_io_result io_error;
  माप_प्रकार tlswritten = 0;
  माप_प्रकार tlswritten_total = 0;

  io_ctx.cf = cf;
  io_ctx.data = data;

  जबसम्म(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    यदि(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(data, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_total);
      फर्काऊ CURLE_AGAIN;
    }
    अन्यथा यदि(io_error) {
      वर्ण buffer[STRERROR_LEN];
      failf(data, "writing to socket: %s",
            curlx_strerror(io_error, buffer, परिमाण(buffer)));
      फर्काऊ CURLE_SEND_ERROR;
    }
    यदि(tlswritten == 0) {
      failf(data, "EOF in swrite");
      फर्काऊ CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(data, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_total += tlswritten;
  }
  फर्काऊ CURLE_OK;
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
स्थिर CURLcode cr_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                        अचर शून्य *plainbuf, माप_प्रकार plainlen,
                        माप_प्रकार *pnwritten)
{
  अचर संरचना ssl_connect_data * अचर connssl = cf->ctx;
  संरचना rustls_ssl_backend_data * अचर backend =
    (संरचना rustls_ssl_backend_data *)connssl->backend;
  संरचना rustls_connection *rconn = रिक्त;
  माप_प्रकार plainwritten = 0;
  अचर अचिन्हित वर्ण *buf = plainbuf;
  CURLcode result = CURLE_OK;
  माप_प्रकार blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(data, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  यदि(backend->plain_out_buffered) {
    result = cr_flush_out(cf, data, rconn);
    CURL_TRC_CF(data, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (पूर्णाङ्क)result);
    यदि(result)
      फर्काऊ result;
    यदि(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    अन्यथा
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  यदि(blen > 0) {
    rustls_result rresult;
    CURL_TRC_CF(data, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    यदि(rresult != RUSTLS_RESULT_OK) {
      rustls_failf(data, rresult, "rustls_connection_write");
      result = CURLE_WRITE_ERROR;
      गच्छ out;
    }
    अन्यथा यदि(plainwritten == 0) {
      failf(data, "rustls_connection_write: EOF");
      result = CURLE_WRITE_ERROR;
      गच्छ out;
    }
  }

  result = cr_flush_out(cf, data, rconn);
  यदि(result) {
    यदि(result == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      यदि(*pnwritten) {
        result = CURLE_OK;
      }
    }
    गच्छ out;
  }
  अन्यथा
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(data, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (पूर्णाङ्क)result, *pnwritten);
  फर्काऊ result;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
स्थिर uint32_t cr_verify_none(शून्य *userdata,
                               अचर rustls_verify_server_cert_params *params)
{
  (शून्य)userdata;
  (शून्य)params;
  फर्काऊ RUSTLS_RESULT_OK;
}

स्थिर पूर्णाङ्क read_file_into(अचर वर्ण *filename, संरचना dynbuf *out)
{
  खाता *f = curlx_fopen(filename, FOPEN_READTEXT);
  यदि(!f) {
    फर्काऊ 0;
  }

  लागि(;;) {
    uint8_t buf[256];
    अचर माप_प्रकार rr = ख_पढ़ो(buf, 1, परिमाण(buf), f);
    यदि((!rr && !ख_पूर्ण(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      फर्काऊ 0;
    }
    यदि(rr < परिमाण(buf))
      रोक;
  }

  फर्काऊ curlx_fclose(f) == 0;
}

स्थिर शून्य
cr_get_selected_ciphers(संरचना Curl_easy *data,
                        अचर वर्ण *ciphers12,
                        अचर वर्ण *ciphers13,
                        अचर संरचना rustls_supported_ciphersuite **selected,
                        माप_प्रकार *selected_size)
{
  अचर माप_प्रकार supported_len = *selected_size;
  अचर माप_प्रकार default_len = rustls_default_crypto_provider_ciphersuites_len();
  अचर संरचना rustls_supported_ciphersuite *entry = रिक्त;
  अचर वर्ण *ciphers = ciphers12;
  माप_प्रकार गन्ती = 0, default13_गन्ती = 0, i, j;
  अचर वर्ण *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  यदि(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    लागि(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      यदि(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        जारी;

      selected[गन्ती++] = entry;
    }

    default13_गन्ती = गन्ती;

    यदि(!ciphers)
      ciphers = "";
  }
  अन्यथा
    ciphers = ciphers13;

add_ciphers:
  लागि(ptr = ciphers; ptr[0] != '\0' && गन्ती < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    यदि(id) {
      लागि(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        यदि(rustls_supported_ciphersuite_get_suite(entry) == id)
          रोक;
      }
      यदि(i == supported_len)
        id = 0;
    }
    यदि(!id) {
      यदि(ptr[0] != '\0')
        infof(data, "rustls: unknown cipher in list: \"%.*s\"",
              (पूर्णाङ्क)(end - ptr), ptr);
      जारी;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    लागि(i = 0; i < गन्ती && selected[i] != entry; i++)
      ;
    यदि(i < गन्ती) {
      यदि(i >= default13_गन्ती)
        infof(data, "rustls: duplicate cipher in list: \"%.*s\"",
              (पूर्णाङ्क)(end - ptr), ptr);
      जारी;
    }

    selected[गन्ती++] = entry;
  }

  यदि(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    गच्छ add_ciphers;
  }

  यदि(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    लागि(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      यदि(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        जारी;

      /* No duplicates allowed (so selected cannot overflow) */
      लागि(i = 0; i < गन्ती && selected[i] != entry; i++)
        ;
      यदि(i < गन्ती)
        जारी;

      selected[गन्ती++] = entry;
    }
  }

  *selected_size = गन्ती;
}

स्थिर शून्य cr_keylog_log_cb(संरचना rustls_str label,
                             अचर uint8_t *client_random,
                             माप_प्रकार client_random_len, अचर uint8_t *secret,
                             माप_प्रकार secret_len)
{
  वर्ण clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_SIZE);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, परिमाण(clabel), "%.*s", (पूर्णाङ्क)label.len, label.data);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

स्थिर CURLcode
init_config_builder(संरचना Curl_cfilter *cf,
                    संरचना Curl_easy *data,
                    अचर संरचना ssl_filter_config *conn_config,
                    संरचना rustls_client_config_builder **config_builder)
{
  अचर संरचना rustls_supported_ciphersuite **cipher_suites = रिक्त;
  संरचना rustls_crypto_provider_builder *custom_provider_builder = रिक्त;
  अचर संरचना rustls_crypto_provider *custom_provider = रिक्त;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  माप_प्रकार tls_versions_len = 2;
  माप_प्रकार cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  विकल्प(conn_config->version) {
  पक्ष CURL_SSLVERSION_TLSv1:
  पक्ष CURL_SSLVERSION_TLSv1_0:
  पक्ष CURL_SSLVERSION_TLSv1_1:
  पक्ष CURL_SSLVERSION_TLSv1_2:
    रोक;
  पक्ष CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    रोक;
  स्वभाव:
    failf(data, "rustls: unsupported minimum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ cleanup;
  }

  विकल्प(conn_config->version_max) {
  पक्ष CURL_SSLVERSION_MAX_DEFAULT:
  पक्ष CURL_SSLVERSION_MAX_NONE:
  पक्ष CURL_SSLVERSION_MAX_TLSv1_3:
    रोक;
  पक्ष CURL_SSLVERSION_MAX_TLSv1_2:
    यदि(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      रोक;
    }
    FALLTHROUGH();
  पक्ष CURL_SSLVERSION_MAX_TLSv1_1:
  पक्ष CURL_SSLVERSION_MAX_TLSv1_0:
  स्वभाव:
    failf(data, "rustls: unsupported maximum TLS version value");
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ cleanup;
  }

#ifdef USE_ECH
  यदि(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(data, "rustls: ECH enabled, forcing TLSv1.3");
  }
#अन्यथा
  (शून्य)cf;
#endif /* USE_ECH */

  cipher_suites = curlx_malloc(परिमाण(*cipher_suites) * cipher_suites_len);
  यदि(!cipher_suites) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ cleanup;
  }

  cr_get_selected_ciphers(data,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  यदि(cipher_suites_len == 0) {
    failf(data, "rustls: no supported cipher in list");
    result = CURLE_SSL_CIPHER;
    गच्छ cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to create crypto provider builder from default");
    result = CURLE_SSL_CIPHER;
    गच्छ cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "failed to set ciphersuites for crypto provider builder");
    result = CURLE_SSL_CIPHER;
    गच्छ cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build custom crypto provider");
    result = CURLE_SSL_CIPHER;
    गच्छ cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create client config builder");
    result = CURLE_SSL_CIPHER;
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
  फर्काऊ result;
}

स्थिर शून्य
init_config_builder_alpn(संरचना Curl_easy *data,
                         अचर संरचना ssl_connect_data *connssl,
                         संरचना rustls_client_config_builder *config_builder)
{
  संरचना alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  माप_प्रकार i;

  लागि(i = 0; i < connssl->alpn->गन्ती; ++i) {
    alpn[i].data = (अचर uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = म_माप(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->गन्ती);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
}

स्थिर CURLcode init_config_builder_verifier_crl(
  संरचना Curl_easy *data,
  अचर संरचना ssl_filter_config *conn_config,
  संरचना rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode result = CURLE_OK;
  संरचना dynbuf crl_contents;
  rustls_result rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_SIZE);
  यदि(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(data, "rustls: failed to read revocation list file");
    result = CURLE_SSL_CRL_BADFILE;
    गच्छ cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to parse revocation list");
    result = CURLE_SSL_CRL_BADFILE;
    गच्छ cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  फर्काऊ result;
}

स्थिर CURLcode
init_config_builder_verifier(संरचना Curl_easy *data,
                             संरचना rustls_client_config_builder *builder,
                             अचर संरचना ssl_filter_config *conn_config,
                             अचर संरचना curl_blob *ca_info_blob,
                             अचर वर्ण * अचर ssl_cafile)
{
  अचर संरचना rustls_जरा_cert_store *roots = रिक्त;
  संरचना rustls_जरा_cert_store_builder *roots_builder = रिक्त;
  संरचना rustls_web_pki_server_cert_verifier_builder *verifier_builder = रिक्त;
  संरचना rustls_server_cert_verifier *server_cert_verifier = रिक्त;
  rustls_result rr = RUSTLS_RESULT_OK;
  CURLcode result = CURLE_OK;

  roots_builder = rustls_जरा_cert_store_builder_new();
  यदि(ca_info_blob) {
    rr = rustls_जरा_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->data,
                                                ca_info_blob->len,
                                                1);
    यदि(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to parse trusted certificates from blob");
      result = CURLE_SSL_CACERT_BADFILE;
      गच्छ cleanup;
    }
  }
  अन्यथा यदि(ssl_cafile) {
    rr = rustls_जरा_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    यदि(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "failed to load trusted certificates");
      result = CURLE_SSL_CACERT_BADFILE;
      गच्छ cleanup;
    }
  }

  rr = rustls_जरा_cert_store_builder_build(roots_builder, &roots);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build trusted root certificate store");
    result = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  यदि(!verifier_builder) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ cleanup;
  }

  यदि(conn_config->CRLfile) {
    result = init_config_builder_verifier_crl(data,
                                              conn_config,
                                              verifier_builder);
    यदि(result) {
      गच्छ cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  यदि(roots_builder) {
    rustls_जरा_cert_store_builder_free(roots_builder);
  }
  यदि(roots) {
    rustls_जरा_cert_store_free(roots);
  }
  यदि(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  यदि(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  फर्काऊ result;
}

स्थिर CURLcode init_config_builder_platform_verifier(
  संरचना Curl_easy *data,
  संरचना rustls_client_config_builder *builder)
{
  संरचना rustls_server_cert_verifier *server_cert_verifier = रिक्त;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to create platform certificate verifier");
    result = CURLE_SSL_CACERT_BADFILE;
    गच्छ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  यदि(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  फर्काऊ result;
}

स्थिर CURLcode
init_config_builder_keylog(संरचना Curl_easy *data,
                           संरचना rustls_client_config_builder *builder)
{
  rustls_result rr;

  Curl_tls_keylog_open();
  यदि(!Curl_tls_keylog_enabled()) {
    फर्काऊ CURLE_OK;
  }

  rr = rustls_client_config_builder_set_साँचो_log(builder,
                                                cr_keylog_log_cb,
                                                रिक्त);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    फर्काऊ map_error(rr);
  }

  फर्काऊ CURLE_OK;
}

स्थिर CURLcode
init_config_builder_client_auth(संरचना Curl_easy *data,
                                अचर संरचना ssl_filter_config *conn_config,
                                संरचना rustls_client_config_builder *builder)
{
  संरचना dynbuf cert_contents;
  संरचना dynbuf साँचो_contents;
  rustls_result rr;
  अचर संरचना rustls_certified_साँचो *certified_साँचो = रिक्त;
  CURLcode result = CURLE_OK;
  अचर bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  अचर bool have_साँचो = conn_config->साँचो || conn_config->साँचो_blob;

  यदि(have_cert && !have_साँचो) {
    failf(data, "rustls: must provide key with certificate");
    फर्काऊ CURLE_SSL_CERTPROBLEM;
  }
  अन्यथा यदि(!have_cert && have_साँचो) {
    failf(data, "rustls: must provide certificate with key");
    फर्काऊ CURLE_SSL_CERTPROBLEM;
  }

  यदि(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(data, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    फर्काऊ CURLE_SSL_CERTPROBLEM;
  }
  यदि(conn_config->साँचो_type && !curl_strequal(conn_config->साँचो_type, "PEM")) {
    failf(data, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->साँचो_type);
    फर्काऊ CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_SIZE);
  curlx_dyn_init(&साँचो_contents, DYN_KEYFILE_SIZE);

  यदि(conn_config->cert_blob) {
    result = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->data,
                            conn_config->cert_blob->len);
    यदि(result) {
      failf(data, "rustls: failed to copy client certificate blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      गच्छ cleanup;
    }
  }
  अन्यथा यदि(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(data, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    result = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

  यदि(conn_config->साँचो_blob) {
    result = curlx_dyn_addn(&साँचो_contents,
                            conn_config->साँचो_blob->data,
                            conn_config->साँचो_blob->len);
    यदि(result) {
      failf(data, "rustls: failed to copy key blob (%s)",
            result == CURLE_TOO_LARGE ? "too large" : "out of memory");
      गच्छ cleanup;
    }
  }
  अन्यथा यदि(!read_file_into(conn_config->साँचो, &साँचो_contents)) {
    failf(data, "rustls: failed to read key file: '%s'",
          conn_config->साँचो);
    result = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

  rr = rustls_certified_साँचो_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&साँचो_contents),
                                  curlx_dyn_len(&साँचो_contents),
                                  &certified_साँचो);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to build certified key");
    result = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

  rr = rustls_certified_साँचो_साँचोहरू_match(certified_साँचो);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr,
                 "rustls: client certificate and keypair files do not match:");
    result = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

  rr = rustls_client_config_builder_set_certified_साँचो(builder,
                                                      &certified_साँचो,
                                                      1);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to set certified key");
    result = CURLE_SSL_CERTPROBLEM;
    गच्छ cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&साँचो_contents);
  यदि(certified_साँचो) {
    rustls_certified_साँचो_free(certified_साँचो);
  }
  फर्काऊ result;
}

#ifdef USE_ECH

स्थिर CURLcode
init_config_builder_ech(संरचना Curl_easy *data,
                        संरचना Curl_cfilter *cf,
                        संरचना rustls_client_config_builder *builder)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  अचर rustls_hpke *hpke = rustls_supported_hpke();
  अचिन्हित वर्ण *ech_config = रिक्त;
  माप_प्रकार ech_config_len = 0;
  संरचना Curl_dns_entry *dns = रिक्त;
  CURLcode result = CURLE_OK;
  rustls_result rr;

  यदि(!hpke) {
    failf(data,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    result = CURLE_SSL_CONNECT_ERROR;
    गच्छ cleanup;
  }

  यदि(conn_config->ech_public) {
    failf(data, "rustls: ECH outername not supported");
    result = CURLE_SSL_CONNECT_ERROR;
    गच्छ cleanup;
  }

  यदि(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    यदि(rr != RUSTLS_RESULT_OK) {
      rustls_failf(data, rr, "rustls: failed to configure ECH GREASE");
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ cleanup;
    }
    फर्काऊ CURLE_OK;
  }

  यदि(conn_config->ech && conn_config->ech_config) {
    अचर वर्ण *b64 = conn_config->ech_config;
    माप_प्रकार decode_result;
    यदि(!b64[0]) {
      infof(data, "rustls: ECHConfig from command line empty");
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_result = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    यदि(decode_result || !ech_config) {
      infof(data, "rustls: cannot base64 decode ECHConfig from command line");
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ cleanup;
    }
  }
  अन्यथा {
    अचर संरचना ssl_connect_data *connssl = cf->ctx;
    अचर संरचना Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(data, cf->sockindex, connssl->peer.origin);

    यदि(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(data, "rustls: ECH requested but no ECHConfig available");
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls: failed to configure ECH");
    result = CURLE_SSL_CONNECT_ERROR;
    गच्छ cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  यदि(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  यदि(dns) {
    Curl_dns_entry_unlink(data, &dns);
  }
  फर्काऊ result;
}
#endif /* USE_ECH */

स्थिर CURLcode cr_init_backend(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                संरचना rustls_ssl_backend_data * अचर backend)
{
  अचर संरचना ssl_connect_data *connssl = cf->ctx;
  अचर संरचना ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  संरचना rustls_connection *rconn = रिक्त;
  संरचना rustls_client_config_builder *config_builder = रिक्त;

  अचर संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
  अचर वर्ण * अचर ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? रिक्त : conn_config->CAfile);
  CURLcode result = CURLE_OK;
  rustls_result rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  result = init_config_builder(cf, data, conn_config, &config_builder);
  यदि(result != CURLE_OK) {
    फर्काऊ result;
  }

  यदि(connssl->alpn) {
    init_config_builder_alpn(data, connssl, config_builder);
  }

  यदि(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  अन्यथा यदि(conn_config->native_ca_store) {
    यदि(conn_config->CRLfile) {
      failf(data, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      फर्काऊ CURLE_NOT_BUILT_IN;
    }
    result = init_config_builder_platform_verifier(data, config_builder);
    यदि(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      फर्काऊ result;
    }
  }
  अन्यथा यदि(ca_info_blob || ssl_cafile) {
    result = init_config_builder_verifier(data,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    यदि(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      फर्काऊ result;
    }
  }

  यदि(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->साँचो || conn_config->साँचो_blob) {
    result = init_config_builder_client_auth(data,
                                             conn_config,
                                             config_builder);
    यदि(result != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      फर्काऊ result;
    }
  }

#ifdef USE_ECH
  यदि(Curl_ssl_ech_enabled(cf)) {
    result = init_config_builder_ech(data, cf, config_builder);
    यदि((result != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      फर्काऊ result;
    }
  }
#endif /* USE_ECH */

  result = init_config_builder_keylog(data, config_builder);
  यदि(result != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    फर्काऊ result;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "failed to build client config");
    फर्काऊ CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  यदि(rr != RUSTLS_RESULT_OK) {
    rustls_failf(data, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = रिक्त;
    फर्काऊ CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  फर्काऊ result;
}

स्थिर शून्य cr_set_negotiated_alpn(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data,
                                   अचर संरचना rustls_connection *rconn)
{
  संरचना ssl_connect_data * अचर connssl = cf->ctx;
  अचर uint8_t *protocol = रिक्त;
  माप_प्रकार len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, data, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
स्थिर CURLcode cr_connect(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                           bool *done)
{
  संरचना ssl_connect_data * अचर connssl = cf->ctx;
  अचर संरचना rustls_ssl_backend_data * अचर backend =
    (संरचना rustls_ssl_backend_data *)connssl->backend;
  अचर संरचना rustls_connection *rconn = रिक्त;
  CURLcode tmperr = CURLE_OK;
  CURLcode result;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(data, cf, "cr_connect, state=%d", (पूर्णाङ्क)connssl->state);
  *done = FALSE;

#ifdef USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    यदि(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(data, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(data, cf, "need HTTPS-RR for ECH, delaying connect");
      फर्काऊ CURLE_OK;
    }
#endif /* USE_ECH */

  यदि(!backend->conn) {
    result =
      cr_init_backend(cf, data,
                      (संरचना rustls_ssl_backend_data *)connssl->backend);
    CURL_TRC_CF(data, cf, "cr_connect, init backend -> %d", (पूर्णाङ्क)result);
    यदि(result)
      फर्काऊ result;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  लागि(;;) {
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
      माप_प्रकार nwritten;
      cr_set_negotiated_alpn(cf, data, rconn);
      tmperr = cr_send(cf, data, रिक्त, 0, &nwritten);
      यदि(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        फर्काऊ CURLE_OK;
      }
      अन्यथा यदि(tmperr != CURLE_OK) {
        फर्काऊ tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#ifdef CURLVERBOSE
        अचर uint16_t proto = rustls_connection_get_protocol_version(rconn);
        अचर rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        अचर rustls_str kex_group_name =
          rustls_connection_get_negotiated_साँचो_exchange_group_name(rconn);
        अचर वर्ण *ver = "TLS version unknown";
        यदि(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        यदि(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#endif
        infof(data,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (पूर्णाङ्क)ciphersuite_name.len,
              ciphersuite_name.data,
              (पूर्णाङ्क)kex_group_name.len,
              kex_group_name.data);
      }
      यदि(data->set.ssl.certinfo) {
        माप_प्रकार अङ्क_certs = 0;
        माप_प्रकार i;
        जबसम्म(rustls_connection_get_peer_certificate(rconn, अङ्क_certs)) {
          अङ्क_certs++;
          यदि(अङ्क_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(data, "%zu certificates is more than allowed (%d)",
                  अङ्क_certs, MAX_ALLOWED_CERT_AMOUNT);
            फर्काऊ CURLE_SSL_CONNECT_ERROR;
          }
        }
        result = Curl_ssl_init_certinfo(data, (पूर्णाङ्क)अङ्क_certs);
        यदि(result)
          फर्काऊ result;
        लागि(i = 0; i < अङ्क_certs; i++) {
          अचर rustls_certificate *cert;
          अचर अचिन्हित वर्ण *der_data;
          माप_प्रकार der_len;
          rustls_result rresult = RUSTLS_RESULT_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_data, &der_len);
          यदि(rresult != RUSTLS_RESULT_OK) {
            वर्ण errorbuf[255];
            माप_प्रकार errorlen;
            rustls_error(rresult, errorbuf, परिमाण(errorbuf), &errorlen);
            failf(data,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (पूर्णाङ्क)errorlen, errorbuf);
            फर्काऊ map_error(rresult);
          }
          result = Curl_extract_certinfo(data, (पूर्णाङ्क)i,
                                         der_data, der_data + der_len);
          यदि(result)
            फर्काऊ result;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      फर्काऊ CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    यदि(wants_write) {
      माप_प्रकार nwritten;
      CURL_TRC_CF(data, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, data, रिक्त, 0, &nwritten);
      यदि(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(data, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        फर्काऊ CURLE_OK;
      }
      अन्यथा यदि(tmperr != CURLE_OK) {
        फर्काऊ tmperr;
      }
    }

    यदि(wants_read) {
      CURL_TRC_CF(data, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, data, &tmperr);
      यदि(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(data, "TLS connect error: Connection closed abruptly");
        फर्काऊ CURLE_SSL_CONNECT_ERROR;
      }
      यदि(nread < 0) {
        यदि(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(data, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          फर्काऊ CURLE_OK;
        }
        अन्यथा यदि(tmperr == CURLE_RECV_ERROR) {
          फर्काऊ CURLE_SSL_CONNECT_ERROR;
        }
        अन्यथा {
          फर्काऊ tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

स्थिर शून्य *cr_get_internals(संरचना ssl_connect_data *connssl, CURLINFO info)
{
  संरचना rustls_ssl_backend_data *backend =
    (संरचना rustls_ssl_backend_data *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  फर्काऊ backend->conn;
}

स्थिर CURLcode cr_shutdown(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                            अचर bool send_shutdown, bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना rustls_ssl_backend_data *backend =
    (संरचना rustls_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  माप_प्रकार i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  यदि(!backend->conn || cf->shutdown) {
    *done = TRUE;
    गच्छ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  यदि(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    यदि(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  result = cr_send(cf, data, रिक्त, 0, &nwritten);
  यदि(result) {
    यदि(result == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_OK;
      गच्छ out;
    }
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown send failed: %d", (पूर्णाङ्क)result);
    गच्छ out;
  }

  लागि(i = 0; i < 10; ++i) {
    वर्ण buf[1024];
    result = cr_recv(cf, data, buf, (पूर्णाङ्क)परिमाण(buf), &nread);
    यदि(result)
      रोक;
  }

  यदि(result == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    result = CURLE_OK;
  }
  अन्यथा यदि(result) {
    DEBUGASSERT(result);
    CURL_TRC_CF(data, cf, "shutdown, error: %d", (पूर्णाङ्क)result);
  }
  अन्यथा यदि(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (result || *done);
  फर्काऊ result;
}

स्थिर शून्य cr_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  अचर संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना rustls_ssl_backend_data *backend =
    (संरचना rustls_ssl_backend_data *)connssl->backend;

  (शून्य)data;
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

स्थिर माप_प्रकार cr_version(वर्ण *buffer, माप_प्रकार size)
{
  अचर संरचना rustls_str ver = rustls_version();
  फर्काऊ curl_msnprintf(buffer, size, "%.*s", (पूर्णाङ्क)ver.len, ver.data);
}

स्थिर CURLcode cr_random(संरचना Curl_easy *data, अचिन्हित वर्ण *entropy,
                          माप_प्रकार length)
{
  rustls_result rresult = 0;
  (शून्य)data;
  rresult = rustls_default_crypto_provider_random(entropy, length);
  फर्काऊ map_error(rresult);
}

स्थिर शून्य cr_cleanup(शून्य)
{
  Curl_tls_keylog_close();
}

अचर संरचना Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  परिमाण(संरचना rustls_ssl_backend_data),

  रिक्त,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_data_pending,                 /* data_pending */
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

#endif /* USE_RUSTLS */
