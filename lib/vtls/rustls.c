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
#समावेश "curl_setup.h"

#अगर_घोषित USE_RUSTLS

#समावेश <rustls.h>

#समावेश "curlx/win32-fopen.h"
#समावेश "curlx/strerr.h"
#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "vdns/cf-dns.h"
#समावेश "vdns/httpsrr.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_int.h"
#समावेश "vtls/rustls.h"
#समावेश "vtls/keylog.h"
#समावेश "vtls/cipher_suite.h"
#समावेश "vtls/x509asn1.h"
#अगर_घोषित USE_ECH
#समावेश "curlx/base64.h"
#पूर्ण_अगर

#अगर EAGAIN != EWOULDBLOCK
#घोषणा RAW_EAGAIN(e) ((e) == EWOULDBLOCK || (e) == EAGAIN)
#अन्यथा
#घोषणा RAW_EAGAIN(e) ((e) == EWOULDBLOCK)
#पूर्ण_अगर

काष्ठा rustls_ssl_backend_तथ्य {
  स्थिर काष्ठा rustls_client_config *config;
  काष्ठा rustls_connection *conn;
  माप_प्रकार plain_out_buffered;
  BIT(तथ्य_in_pending);
  BIT(sent_shutdown);
};

/* For a given rustls_result error code, return the best-matching CURLcode. */
अटल CURLcode map_error(स्थिर rustls_परिणाम r)
{
  अगर(rustls_परिणाम_is_cert_error(r)) {
    वापस CURLE_PEER_FAILED_VERIFICATION;
  }
  चयन(r) {
  हाल RUSTLS_परिणाम_OK:
    वापस CURLE_OK;
  हाल RUSTLS_परिणाम_NULL_PARAMETER:
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  शेष:
    वापस CURLE_RECV_ERROR;
  }
}

अटल व्योम rustls_failf(काष्ठा Curl_easy *तथ्य, स्थिर rustls_परिणाम rr,
                         स्थिर अक्षर *संदेश)
{
  अक्षर errorbuf[STRERROR_LEN];
  माप_प्रकार errorlen;
  rustls_error(rr, errorbuf, माप(errorbuf), &errorlen);
  failf(तथ्य, "%s: %.*s", संदेश, (पूर्णांक)errorlen, errorbuf);
}

अटल bool cr_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                            स्थिर काष्ठा Curl_easy *तथ्य)
{
  स्थिर काष्ठा ssl_connect_तथ्य *ctx = cf->ctx;
  काष्ठा rustls_ssl_backend_तथ्य *backend;

  (व्योम)तथ्य;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (काष्ठा rustls_ssl_backend_तथ्य *)ctx->backend;
  वापस (bool)backend->तथ्य_in_pending;
}

काष्ठा io_ctx {
  काष्ठा Curl_cfilter *cf;
  काष्ठा Curl_easy *तथ्य;
};

अटल पूर्णांक read_cb(व्योम *userdata, uint8_t *buf, uintptr_t len,
                   uintptr_t *out_n)
{
  स्थिर काष्ठा io_ctx *io_ctx = userdata;
  काष्ठा ssl_connect_तथ्य * स्थिर connssl = io_ctx->cf->ctx;
  CURLcode परिणाम;
  पूर्णांक ret = 0;
  माप_प्रकार nread;

  परिणाम = Curl_conn_cf_recv(io_ctx->cf->अग्र, io_ctx->तथ्य,
                             (अक्षर *)buf, len, &nread);
  अगर(परिणाम) {
    nread = 0;
    /* !checksrc! disable ERRNOVAR 4 */
    अगर(परिणाम == CURLE_AGAIN)
      ret = EAGAIN;
    अन्यथा
      ret = EINVAL;
  }
  अन्यथा अगर(nread == 0)
    connssl->peer_closed = TRUE;
  *out_n = (uintptr_t)nread;
  CURL_TRC_CF(io_ctx->तथ्य, io_ctx->cf, "cf->next recv(len=%zu) -> %d, %zu",
              (माप_प्रकार)len, (पूर्णांक)परिणाम, nread);
  वापस ret;
}

अटल पूर्णांक write_cb(व्योम *userdata, स्थिर uint8_t *buf, uintptr_t len,
                    uintptr_t *out_n)
{
  स्थिर काष्ठा io_ctx *io_ctx = userdata;
  CURLcode परिणाम;
  पूर्णांक ret = 0;
  माप_प्रकार nwritten;

  परिणाम = Curl_conn_cf_send(io_ctx->cf->अग्र, io_ctx->तथ्य,
                             buf, len, FALSE, &nwritten);
  अगर(परिणाम) {
    nwritten = 0;
    अगर(परिणाम == CURLE_AGAIN)
      ret = EAGAIN;
    अन्यथा
      ret = EINVAL;
  }
  *out_n = (uintptr_t)nwritten;
  CURL_TRC_CF(io_ctx->तथ्य, io_ctx->cf, "cf->next send(len=%zu) -> %d, %zu",
              len, (पूर्णांक)परिणाम, nwritten);
  वापस ret;
}

अटल ssize_t tls_recv_more(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य, CURLcode *err)
{
  स्थिर काष्ठा ssl_connect_तथ्य * स्थिर connssl = cf->ctx;
  काष्ठा rustls_ssl_backend_तथ्य * स्थिर backend =
    (काष्ठा rustls_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा io_ctx io_ctx;
  माप_प्रकार tls_bytes_read = 0;
  rustls_io_परिणाम io_error;
  rustls_परिणाम rresult = 0;

  io_ctx.cf = cf;
  io_ctx.तथ्य = तथ्य;
  io_error = rustls_connection_read_tls(backend->conn, read_cb, &io_ctx,
                                        &tls_bytes_read);
  अगर(RAW_EAGAIN(io_error)) {
    *err = CURLE_AGAIN;
    वापस -1;
  }
  अन्यथा अगर(io_error) {
    अक्षर बफर[STRERROR_LEN];
    failf(तथ्य, "reading from socket: %s",
          curlx_strerror(io_error, बफर, माप(बफर)));
    *err = CURLE_RECV_ERROR;
    वापस -1;
  }

  rresult = rustls_connection_process_new_packets(backend->conn);
  अगर(rresult != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rresult, "rustls_connection_process_new_packets");
    *err = map_error(rresult);
    वापस -1;
  }

  backend->तथ्य_in_pending = TRUE;
  *err = CURLE_OK;
  वापस (ssize_t)tls_bytes_read;
}

/*
 * Filter receive method implementation. `plainbuf` and `plainlen`
 * are always not NULL/0.
 */
अटल CURLcode cr_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                        अक्षर *plainbuf, माप_प्रकार plainlen, माप_प्रकार *pnread)
{
  स्थिर काष्ठा ssl_connect_तथ्य * स्थिर connssl = cf->ctx;
  काष्ठा rustls_ssl_backend_तथ्य * स्थिर backend =
    (काष्ठा rustls_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा rustls_connection *rconn = शून्य;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार n = 0;
  rustls_परिणाम rresult = 0;
  bool eof = FALSE;

  DEBUGASSERT(backend);
  *pnread = 0;
  rconn = backend->conn;

  जबतक(*pnread < plainlen) {
    अगर(!backend->तथ्य_in_pending) {
      अगर(tls_recv_more(cf, तथ्य, &परिणाम) < 0) {
        अगर(परिणाम != CURLE_AGAIN) {
          जाओ out;
        }
        परिणाम = CURLE_OK;
        अवरोध;
      }
    }

    rresult = rustls_connection_read(rconn,
                                     (uint8_t *)plainbuf + *pnread,
                                     plainlen - *pnread,
                                     &n);
    अगर(rresult == RUSTLS_परिणाम_PLAINTEXT_EMPTY) {
      /* A TLS message that carried no plaintext. Break out of the loop
       * to prevent a server from keeping us here forever. */
      backend->तथ्य_in_pending = FALSE;
      अवरोध;
    }
    अन्यथा अगर(rresult == RUSTLS_परिणाम_UNEXPECTED_EOF) {
      failf(तथ्य, "rustls: peer closed TCP connection "
            "without first closing TLS connection");
      परिणाम = CURLE_RECV_ERROR;
      जाओ out;
    }
    अन्यथा अगर(rresult != RUSTLS_परिणाम_OK) {
      /* n always equals 0 in this case, do not need to check it */
      rustls_failf(तथ्य, rresult, "rustls_connection_read");
      परिणाम = CURLE_RECV_ERROR;
      जाओ out;
    }
    अन्यथा अगर(n == 0) {
      /* n == 0 indicates clean EOF, but we may have read some other
         plaintext bytes before we reached this. Break out of the loop
         so we can figure out whether to return success or EOF. */
      eof = TRUE;
      अवरोध;
    }
    अन्यथा {
      *pnread += n;
    }
  }

  अगर(!eof && !*pnread) {
    परिणाम = CURLE_AGAIN;
  }

out:
  CURL_TRC_CF(तथ्य, cf, "rustls_recv(len=%zu) -> %d, %zu",
              plainlen, (पूर्णांक)परिणाम, *pnread);
  वापस परिणाम;
}

अटल CURLcode cr_flush_out(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                             काष्ठा rustls_connection *rconn)
{
  काष्ठा io_ctx io_ctx;
  rustls_io_परिणाम io_error;
  माप_प्रकार tlswritten = 0;
  माप_प्रकार tlswritten_योग = 0;

  io_ctx.cf = cf;
  io_ctx.तथ्य = तथ्य;

  जबतक(rustls_connection_wants_write(rconn)) {
    io_error = rustls_connection_write_tls(rconn, write_cb, &io_ctx,
                                           &tlswritten);
    अगर(RAW_EAGAIN(io_error)) {
      CURL_TRC_CF(तथ्य, cf, "cf_send: EAGAIN after %zu bytes",
                  tlswritten_योग);
      वापस CURLE_AGAIN;
    }
    अन्यथा अगर(io_error) {
      अक्षर बफर[STRERROR_LEN];
      failf(तथ्य, "writing to socket: %s",
            curlx_strerror(io_error, बफर, माप(बफर)));
      वापस CURLE_SEND_ERROR;
    }
    अगर(tlswritten == 0) {
      failf(तथ्य, "EOF in swrite");
      वापस CURLE_SEND_ERROR;
    }
    CURL_TRC_CF(तथ्य, cf, "cf_send: wrote %zu TLS bytes", tlswritten);
    tlswritten_योग += tlswritten;
  }
  वापस CURLE_OK;
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
अटल CURLcode cr_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                        स्थिर व्योम *plainbuf, माप_प्रकार plainlen,
                        माप_प्रकार *pnwritten)
{
  स्थिर काष्ठा ssl_connect_तथ्य * स्थिर connssl = cf->ctx;
  काष्ठा rustls_ssl_backend_तथ्य * स्थिर backend =
    (काष्ठा rustls_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा rustls_connection *rconn = शून्य;
  माप_प्रकार plainwritten = 0;
  स्थिर अचिन्हित अक्षर *buf = plainbuf;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार blen = plainlen;

  DEBUGASSERT(backend);
  *pnwritten = 0;
  rconn = backend->conn;
  DEBUGASSERT(rconn);

  CURL_TRC_CF(तथ्य, cf, "cf_send(len=%zu)", plainlen);

  /* If a previous send blocked, we already added its plain bytes
   * to rustsls and must not do that again. Flush the TLS bytes and,
   * if successful, deduct the previous plain bytes from the current
   * send. */
  अगर(backend->plain_out_buffered) {
    परिणाम = cr_flush_out(cf, तथ्य, rconn);
    CURL_TRC_CF(तथ्य, cf, "cf_send: flushing %zu previously added bytes -> %d",
                backend->plain_out_buffered, (पूर्णांक)परिणाम);
    अगर(परिणाम)
      वापस परिणाम;
    अगर(blen > backend->plain_out_buffered) {
      blen -= backend->plain_out_buffered;
      buf += backend->plain_out_buffered;
    }
    अन्यथा
      blen = 0;
    *pnwritten += backend->plain_out_buffered;
    backend->plain_out_buffered = 0;
  }

  अगर(blen > 0) {
    rustls_परिणाम rresult;
    CURL_TRC_CF(तथ्य, cf, "cf_send: adding %zu plain bytes to Rustls", blen);
    rresult = rustls_connection_write(rconn, buf, blen, &plainwritten);
    अगर(rresult != RUSTLS_परिणाम_OK) {
      rustls_failf(तथ्य, rresult, "rustls_connection_write");
      परिणाम = CURLE_WRITE_ERROR;
      जाओ out;
    }
    अन्यथा अगर(plainwritten == 0) {
      failf(तथ्य, "rustls_connection_write: EOF");
      परिणाम = CURLE_WRITE_ERROR;
      जाओ out;
    }
  }

  परिणाम = cr_flush_out(cf, तथ्य, rconn);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN) {
      /* The TLS bytes may have been partially written, but we fail the
       * complete send() and remember how much we already added to Rustls. */
      backend->plain_out_buffered = plainwritten;
      अगर(*pnwritten) {
        परिणाम = CURLE_OK;
      }
    }
    जाओ out;
  }
  अन्यथा
    *pnwritten += plainwritten;

out:
  CURL_TRC_CF(तथ्य, cf, "rustls_send(len=%zu) -> %d, %zu",
              plainlen, (पूर्णांक)परिणाम, *pnwritten);
  वापस परिणाम;
}

/* A server certificate verify callback for Rustls that always returns
   RUSTLS_RESULT_OK, or in other words disable certificate verification. */
अटल uint32_t cr_verify_none(व्योम *userdata,
                               स्थिर rustls_verify_server_cert_params *params)
{
  (व्योम)userdata;
  (व्योम)params;
  वापस RUSTLS_परिणाम_OK;
}

अटल पूर्णांक read_file_into(स्थिर अक्षर *filename, काष्ठा dynbuf *out)
{
  खाता *f = curlx_fopen(filename, FOPEN_READTEXT);
  अगर(!f) {
    वापस 0;
  }

  क्रम(;;) {
    uint8_t buf[256];
    स्थिर माप_प्रकार rr = ख_पढ़ो(buf, 1, माप(buf), f);
    अगर((!rr && !ख_पूर्ण(f)) ||
       curlx_dyn_addn(out, buf, rr)) {
      curlx_fclose(f);
      वापस 0;
    }
    अगर(rr < माप(buf))
      अवरोध;
  }

  वापस curlx_fclose(f) == 0;
}

अटल व्योम
cr_get_selected_ciphers(काष्ठा Curl_easy *तथ्य,
                        स्थिर अक्षर *ciphers12,
                        स्थिर अक्षर *ciphers13,
                        स्थिर काष्ठा rustls_supported_ciphersuite **selected,
                        माप_प्रकार *selected_आकार)
{
  स्थिर माप_प्रकार supported_len = *selected_आकार;
  स्थिर माप_प्रकार default_len = rustls_default_crypto_provider_ciphersuites_len();
  स्थिर काष्ठा rustls_supported_ciphersuite *entry = शून्य;
  स्थिर अक्षर *ciphers = ciphers12;
  माप_प्रकार गिनती = 0, default13_गिनती = 0, i, j;
  स्थिर अक्षर *ptr, *end;

  DEBUGASSERT(default_len <= supported_len);

  अगर(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    क्रम(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      अगर(rustls_supported_ciphersuite_protocol_version(entry) !=
         RUSTLS_TLS_VERSION_TLSV1_3)
        जारी;

      selected[गिनती++] = entry;
    }

    default13_गिनती = गिनती;

    अगर(!ciphers)
      ciphers = "";
  }
  अन्यथा
    ciphers = ciphers13;

add_ciphers:
  क्रम(ptr = ciphers; ptr[0] != '\0' && गिनती < supported_len; ptr = end) {
    uint16_t id = Curl_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    अगर(id) {
      क्रम(i = 0; i < supported_len; i++) {
        entry = rustls_default_crypto_provider_ciphersuites_get(i);
        अगर(rustls_supported_ciphersuite_get_suite(entry) == id)
          अवरोध;
      }
      अगर(i == supported_len)
        id = 0;
    }
    अगर(!id) {
      अगर(ptr[0] != '\0')
        infof(तथ्य, "rustls: unknown cipher in list: \"%.*s\"",
              (पूर्णांक)(end - ptr), ptr);
      जारी;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    क्रम(i = 0; i < गिनती && selected[i] != entry; i++)
      ;
    अगर(i < गिनती) {
      अगर(i >= default13_गिनती)
        infof(तथ्य, "rustls: duplicate cipher in list: \"%.*s\"",
              (पूर्णांक)(end - ptr), ptr);
      जारी;
    }

    selected[गिनती++] = entry;
  }

  अगर(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    जाओ add_ciphers;
  }

  अगर(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    क्रम(j = 0; j < default_len; j++) {
      entry = rustls_default_crypto_provider_ciphersuites_get(j);
      अगर(rustls_supported_ciphersuite_protocol_version(entry) ==
         RUSTLS_TLS_VERSION_TLSV1_3)
        जारी;

      /* No duplicates allowed (so selected cannot overflow) */
      क्रम(i = 0; i < गिनती && selected[i] != entry; i++)
        ;
      अगर(i < गिनती)
        जारी;

      selected[गिनती++] = entry;
    }
  }

  *selected_आकार = गिनती;
}

अटल व्योम cr_keylog_log_cb(काष्ठा rustls_str label,
                             स्थिर uint8_t *client_random,
                             माप_प्रकार client_random_len, स्थिर uint8_t *secret,
                             माप_प्रकार secret_len)
{
  अक्षर clabel[KEYLOG_LABEL_MAXLEN];
  DEBUGASSERT(client_random_len == CLIENT_RANDOM_आकार);
  /* Turning a "rustls_str" into a null delimited "c" string */
  curl_msnprintf(clabel, माप(clabel), "%.*s", (पूर्णांक)label.len, label.तथ्य);
  Curl_tls_keylog_write(clabel, client_random, client_random_len,
                        secret, secret_len);
}

अटल CURLcode
init_config_builder(काष्ठा Curl_cfilter *cf,
                    काष्ठा Curl_easy *तथ्य,
                    स्थिर काष्ठा ssl_filter_config *conn_config,
                    काष्ठा rustls_client_config_builder **config_builder)
{
  स्थिर काष्ठा rustls_supported_ciphersuite **cipher_suites = शून्य;
  काष्ठा rustls_crypto_provider_builder *custom_provider_builder = शून्य;
  स्थिर काष्ठा rustls_crypto_provider *custom_provider = शून्य;

  uint16_t tls_versions[2] = {
    RUSTLS_TLS_VERSION_TLSV1_2,
    RUSTLS_TLS_VERSION_TLSV1_3,
  };
  माप_प्रकार tls_versions_len = 2;
  माप_प्रकार cipher_suites_len = rustls_default_crypto_provider_ciphersuites_len();

  CURLcode परिणाम = CURLE_OK;
  rustls_परिणाम rr;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  चयन(conn_config->version) {
  हाल CURL_SSLVERSION_TLSv1:
  हाल CURL_SSLVERSION_TLSv1_0:
  हाल CURL_SSLVERSION_TLSv1_1:
  हाल CURL_SSLVERSION_TLSv1_2:
    अवरोध;
  हाल CURL_SSLVERSION_TLSv1_3:
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    अवरोध;
  शेष:
    failf(तथ्य, "rustls: unsupported minimum TLS version value");
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ cleanup;
  }

  चयन(conn_config->version_max) {
  हाल CURL_SSLVERSION_MAX_DEFAULT:
  हाल CURL_SSLVERSION_MAX_NONE:
  हाल CURL_SSLVERSION_MAX_TLSv1_3:
    अवरोध;
  हाल CURL_SSLVERSION_MAX_TLSv1_2:
    अगर(tls_versions[0] == RUSTLS_TLS_VERSION_TLSV1_2) {
      tls_versions_len = 1;
      अवरोध;
    }
    FALLTHROUGH();
  हाल CURL_SSLVERSION_MAX_TLSv1_1:
  हाल CURL_SSLVERSION_MAX_TLSv1_0:
  शेष:
    failf(तथ्य, "rustls: unsupported maximum TLS version value");
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ cleanup;
  }

#अगर_घोषित USE_ECH
  अगर(Curl_ssl_ech_enabled(cf)) {
    tls_versions[0] = RUSTLS_TLS_VERSION_TLSV1_3;
    tls_versions_len = 1;
    infof(तथ्य, "rustls: ECH enabled, forcing TLSv1.3");
  }
#अन्यथा
  (व्योम)cf;
#पूर्ण_अगर /* USE_ECH */

  cipher_suites = curlx_malloc(माप(*cipher_suites) * cipher_suites_len);
  अगर(!cipher_suites) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ cleanup;
  }

  cr_get_selected_ciphers(तथ्य,
                          conn_config->cipher_list,
                          conn_config->cipher_list13,
                          cipher_suites, &cipher_suites_len);
  अगर(cipher_suites_len == 0) {
    failf(तथ्य, "rustls: no supported cipher in list");
    परिणाम = CURLE_SSL_CIPHER;
    जाओ cleanup;
  }

  rr = rustls_crypto_provider_builder_new_from_default(
    &custom_provider_builder);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr,
                 "failed to create crypto provider builder from default");
    परिणाम = CURLE_SSL_CIPHER;
    जाओ cleanup;
  }

  rr =
    rustls_crypto_provider_builder_set_cipher_suites(
      custom_provider_builder,
      cipher_suites,
      cipher_suites_len);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr,
                 "failed to set ciphersuites for crypto provider builder");
    परिणाम = CURLE_SSL_CIPHER;
    जाओ cleanup;
  }

  rr = rustls_crypto_provider_builder_build(custom_provider_builder,
                                            &custom_provider);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "failed to build custom crypto provider");
    परिणाम = CURLE_SSL_CIPHER;
    जाओ cleanup;
  }

  rr = rustls_client_config_builder_new_custom(custom_provider,
                                               tls_versions,
                                               tls_versions_len,
                                               config_builder);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "failed to create client config builder");
    परिणाम = CURLE_SSL_CIPHER;
    जाओ cleanup;
  }

cleanup:
  अगर(cipher_suites) {
    curlx_free(cipher_suites);
  }
  अगर(custom_provider_builder) {
    rustls_crypto_provider_builder_free(custom_provider_builder);
  }
  अगर(custom_provider) {
    rustls_crypto_provider_free(custom_provider);
  }
  वापस परिणाम;
}

अटल व्योम
init_config_builder_alpn(काष्ठा Curl_easy *तथ्य,
                         स्थिर काष्ठा ssl_connect_तथ्य *connssl,
                         काष्ठा rustls_client_config_builder *config_builder)
{
  काष्ठा alpn_proto_buf proto;
  rustls_slice_bytes alpn[ALPN_ENTRIES_MAX];
  माप_प्रकार i;

  क्रम(i = 0; i < connssl->alpn->गिनती; ++i) {
    alpn[i].तथ्य = (स्थिर uint8_t *)connssl->alpn->entries[i];
    alpn[i].len = म_माप(connssl->alpn->entries[i]);
  }
  rustls_client_config_builder_set_alpn_protocols(config_builder, alpn,
                                                  connssl->alpn->गिनती);
  Curl_alpn_to_proto_str(&proto, connssl->alpn);
  infof(तथ्य, VTLS_INFOF_ALPN_OFFER_1STR, proto.तथ्य);
}

अटल CURLcode init_config_builder_verifier_crl(
  काष्ठा Curl_easy *तथ्य,
  स्थिर काष्ठा ssl_filter_config *conn_config,
  काष्ठा rustls_web_pki_server_cert_verifier_builder *builder)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा dynbuf crl_contents;
  rustls_परिणाम rr;

  curlx_dyn_init(&crl_contents, DYN_CRLFILE_आकार);
  अगर(!read_file_into(conn_config->CRLfile, &crl_contents)) {
    failf(तथ्य, "rustls: failed to read revocation list file");
    परिणाम = CURLE_SSL_CRL_BADFILE;
    जाओ cleanup;
  }

  rr = rustls_web_pki_server_cert_verifier_builder_add_crl(
    builder,
    curlx_dyn_uptr(&crl_contents),
    curlx_dyn_len(&crl_contents));
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "failed to parse revocation list");
    परिणाम = CURLE_SSL_CRL_BADFILE;
    जाओ cleanup;
  }

cleanup:
  curlx_dyn_free(&crl_contents);
  वापस परिणाम;
}

अटल CURLcode
init_config_builder_verifier(काष्ठा Curl_easy *तथ्य,
                             काष्ठा rustls_client_config_builder *builder,
                             स्थिर काष्ठा ssl_filter_config *conn_config,
                             स्थिर काष्ठा curl_blob *ca_info_blob,
                             स्थिर अक्षर * स्थिर ssl_cafile)
{
  स्थिर काष्ठा rustls_मूल_cert_store *roots = शून्य;
  काष्ठा rustls_मूल_cert_store_builder *roots_builder = शून्य;
  काष्ठा rustls_web_pki_server_cert_verifier_builder *verifier_builder = शून्य;
  काष्ठा rustls_server_cert_verifier *server_cert_verifier = शून्य;
  rustls_परिणाम rr = RUSTLS_परिणाम_OK;
  CURLcode परिणाम = CURLE_OK;

  roots_builder = rustls_मूल_cert_store_builder_new();
  अगर(ca_info_blob) {
    rr = rustls_मूल_cert_store_builder_add_pem(roots_builder,
                                                ca_info_blob->तथ्य,
                                                ca_info_blob->len,
                                                1);
    अगर(rr != RUSTLS_परिणाम_OK) {
      rustls_failf(तथ्य, rr, "failed to parse trusted certificates from blob");
      परिणाम = CURLE_SSL_CACERT_BADFILE;
      जाओ cleanup;
    }
  }
  अन्यथा अगर(ssl_cafile) {
    rr = rustls_मूल_cert_store_builder_load_roots_from_file(roots_builder,
                                                             ssl_cafile,
                                                             1);
    अगर(rr != RUSTLS_परिणाम_OK) {
      rustls_failf(तथ्य, rr, "failed to load trusted certificates");
      परिणाम = CURLE_SSL_CACERT_BADFILE;
      जाओ cleanup;
    }
  }

  rr = rustls_मूल_cert_store_builder_build(roots_builder, &roots);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "failed to build trusted root certificate store");
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    जाओ cleanup;
  }

  verifier_builder = rustls_web_pki_server_cert_verifier_builder_new(roots);
  अगर(!verifier_builder) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ cleanup;
  }

  अगर(conn_config->CRLfile) {
    परिणाम = init_config_builder_verifier_crl(तथ्य,
                                              conn_config,
                                              verifier_builder);
    अगर(परिणाम) {
      जाओ cleanup;
    }
  }

  rr = rustls_web_pki_server_cert_verifier_builder_build(
    verifier_builder, &server_cert_verifier);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "failed to build certificate verifier");
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    जाओ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);
cleanup:
  अगर(roots_builder) {
    rustls_मूल_cert_store_builder_free(roots_builder);
  }
  अगर(roots) {
    rustls_मूल_cert_store_free(roots);
  }
  अगर(verifier_builder) {
    rustls_web_pki_server_cert_verifier_builder_free(verifier_builder);
  }
  अगर(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }

  वापस परिणाम;
}

अटल CURLcode init_config_builder_platform_verifier(
  काष्ठा Curl_easy *तथ्य,
  काष्ठा rustls_client_config_builder *builder)
{
  काष्ठा rustls_server_cert_verifier *server_cert_verifier = शून्य;
  CURLcode परिणाम = CURLE_OK;
  rustls_परिणाम rr;

  rr = rustls_platform_server_cert_verifier(&server_cert_verifier);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "failed to create platform certificate verifier");
    परिणाम = CURLE_SSL_CACERT_BADFILE;
    जाओ cleanup;
  }

  rustls_client_config_builder_set_server_verifier(builder,
                                                   server_cert_verifier);

cleanup:
  अगर(server_cert_verifier) {
    rustls_server_cert_verifier_free(server_cert_verifier);
  }
  वापस परिणाम;
}

अटल CURLcode
init_config_builder_keylog(काष्ठा Curl_easy *तथ्य,
                           काष्ठा rustls_client_config_builder *builder)
{
  rustls_परिणाम rr;

  Curl_tls_keylog_open();
  अगर(!Curl_tls_keylog_enabled()) {
    वापस CURLE_OK;
  }

  rr = rustls_client_config_builder_set_कुंजी_log(builder,
                                                cr_keylog_log_cb,
                                                शून्य);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "rustls_client_config_builder_set_key_log");
    Curl_tls_keylog_close();
    वापस map_error(rr);
  }

  वापस CURLE_OK;
}

अटल CURLcode
init_config_builder_client_auth(काष्ठा Curl_easy *तथ्य,
                                स्थिर काष्ठा ssl_filter_config *conn_config,
                                काष्ठा rustls_client_config_builder *builder)
{
  काष्ठा dynbuf cert_contents;
  काष्ठा dynbuf कुंजी_contents;
  rustls_परिणाम rr;
  स्थिर काष्ठा rustls_certified_कुंजी *certified_कुंजी = शून्य;
  CURLcode परिणाम = CURLE_OK;
  स्थिर bool have_cert = conn_config->clientcert || conn_config->cert_blob;
  स्थिर bool have_कुंजी = conn_config->कुंजी || conn_config->कुंजी_blob;

  अगर(have_cert && !have_कुंजी) {
    failf(तथ्य, "rustls: must provide key with certificate");
    वापस CURLE_SSL_CERTPROBLEM;
  }
  अन्यथा अगर(!have_cert && have_कुंजी) {
    failf(तथ्य, "rustls: must provide certificate with key");
    वापस CURLE_SSL_CERTPROBLEM;
  }

  अगर(conn_config->cert_type && !curl_strequal(conn_config->cert_type, "PEM")) {
    failf(तथ्य, "rustls: unsupported certificate type '%s', only 'PEM' is "
          "supported", conn_config->cert_type);
    वापस CURLE_SSL_CERTPROBLEM;
  }
  अगर(conn_config->कुंजी_type && !curl_strequal(conn_config->कुंजी_type, "PEM")) {
    failf(तथ्य, "rustls: unsupported key type '%s', only 'PEM' is supported",
          conn_config->कुंजी_type);
    वापस CURLE_SSL_CERTPROBLEM;
  }

  curlx_dyn_init(&cert_contents, DYN_CERTFILE_आकार);
  curlx_dyn_init(&कुंजी_contents, DYN_KEYFILE_आकार);

  अगर(conn_config->cert_blob) {
    परिणाम = curlx_dyn_addn(&cert_contents,
                            conn_config->cert_blob->तथ्य,
                            conn_config->cert_blob->len);
    अगर(परिणाम) {
      failf(तथ्य, "rustls: failed to copy client certificate blob (%s)",
            परिणाम == CURLE_TOO_LARGE ? "too large" : "out of memory");
      जाओ cleanup;
    }
  }
  अन्यथा अगर(!read_file_into(conn_config->clientcert, &cert_contents)) {
    failf(तथ्य, "rustls: failed to read client certificate file: '%s'",
          conn_config->clientcert);
    परिणाम = CURLE_SSL_CERTPROBLEM;
    जाओ cleanup;
  }

  अगर(conn_config->कुंजी_blob) {
    परिणाम = curlx_dyn_addn(&कुंजी_contents,
                            conn_config->कुंजी_blob->तथ्य,
                            conn_config->कुंजी_blob->len);
    अगर(परिणाम) {
      failf(तथ्य, "rustls: failed to copy key blob (%s)",
            परिणाम == CURLE_TOO_LARGE ? "too large" : "out of memory");
      जाओ cleanup;
    }
  }
  अन्यथा अगर(!read_file_into(conn_config->कुंजी, &कुंजी_contents)) {
    failf(तथ्य, "rustls: failed to read key file: '%s'",
          conn_config->कुंजी);
    परिणाम = CURLE_SSL_CERTPROBLEM;
    जाओ cleanup;
  }

  rr = rustls_certified_कुंजी_build(curlx_dyn_uptr(&cert_contents),
                                  curlx_dyn_len(&cert_contents),
                                  curlx_dyn_uptr(&कुंजी_contents),
                                  curlx_dyn_len(&कुंजी_contents),
                                  &certified_कुंजी);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "rustls: failed to build certified key");
    परिणाम = CURLE_SSL_CERTPROBLEM;
    जाओ cleanup;
  }

  rr = rustls_certified_कुंजी_कुंजियाँ_match(certified_कुंजी);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr,
                 "rustls: client certificate and keypair files do not match:");
    परिणाम = CURLE_SSL_CERTPROBLEM;
    जाओ cleanup;
  }

  rr = rustls_client_config_builder_set_certified_कुंजी(builder,
                                                      &certified_कुंजी,
                                                      1);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "rustls: failed to set certified key");
    परिणाम = CURLE_SSL_CERTPROBLEM;
    जाओ cleanup;
  }

cleanup:
  curlx_dyn_free(&cert_contents);
  curlx_dyn_free(&कुंजी_contents);
  अगर(certified_कुंजी) {
    rustls_certified_कुंजी_free(certified_कुंजी);
  }
  वापस परिणाम;
}

#अगर_घोषित USE_ECH

अटल CURLcode
init_config_builder_ech(काष्ठा Curl_easy *तथ्य,
                        काष्ठा Curl_cfilter *cf,
                        काष्ठा rustls_client_config_builder *builder)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  स्थिर rustls_hpke *hpke = rustls_supported_hpke();
  अचिन्हित अक्षर *ech_config = शून्य;
  माप_प्रकार ech_config_len = 0;
  काष्ठा Curl_dns_entry *dns = शून्य;
  CURLcode परिणाम = CURLE_OK;
  rustls_परिणाम rr;

  अगर(!hpke) {
    failf(तथ्य,
          "rustls: ECH unavailable, rustls-ffi built without "
          "HPKE compatible crypto provider");
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    जाओ cleanup;
  }

  अगर(conn_config->ech_public) {
    failf(तथ्य, "rustls: ECH outername not supported");
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    जाओ cleanup;
  }

  अगर(conn_config->ech == CURLECH_GREASE) {
    rr = rustls_client_config_builder_enable_ech_grease(builder, hpke);
    अगर(rr != RUSTLS_परिणाम_OK) {
      rustls_failf(तथ्य, rr, "rustls: failed to configure ECH GREASE");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ cleanup;
    }
    वापस CURLE_OK;
  }

  अगर(conn_config->ech && conn_config->ech_config) {
    स्थिर अक्षर *b64 = conn_config->ech_config;
    माप_प्रकार decode_परिणाम;
    अगर(!b64[0]) {
      infof(तथ्य, "rustls: ECHConfig from command line empty");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ cleanup;
    }
    /* rustls-ffi expects the raw TLS encoded ECHConfigList bytes */
    decode_परिणाम = curlx_base64_decode(b64, &ech_config, &ech_config_len);
    अगर(decode_परिणाम || !ech_config) {
      infof(तथ्य, "rustls: cannot base64 decode ECHConfig from command line");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ cleanup;
    }
  }
  अन्यथा {
    स्थिर काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
    स्थिर काष्ठा Curl_https_rrinfo *rinfo =
      Curl_conn_dns_get_https(तथ्य, cf->sockindex, connssl->peer.origin);

    अगर(!Curl_httpsrr_is_for_peer(connssl->peer.origin, rinfo) ||
       !rinfo->echconfiglist) {
      failf(तथ्य, "rustls: ECH requested but no ECHConfig available");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ cleanup;
    }
    ech_config = rinfo->echconfiglist;
    ech_config_len = rinfo->echconfiglist_len;
  }

  rr = rustls_client_config_builder_enable_ech(builder,
                                               ech_config,
                                               ech_config_len,
                                               hpke);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "rustls: failed to configure ECH");
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    जाओ cleanup;
  }
cleanup:
  /* if we base64 decoded, we can free now */
  अगर(conn_config->ech && conn_config->ech_config) {
    curlx_free(ech_config);
  }
  अगर(dns) {
    Curl_dns_entry_unlink(तथ्य, &dns);
  }
  वापस परिणाम;
}
#पूर्ण_अगर /* USE_ECH */

अटल CURLcode cr_init_backend(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                काष्ठा rustls_ssl_backend_तथ्य * स्थिर backend)
{
  स्थिर काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  स्थिर काष्ठा ssl_filter_config *conn_config =
    Curl_ssl_cf_get_filter_config(cf);
  काष्ठा rustls_connection *rconn = शून्य;
  काष्ठा rustls_client_config_builder *config_builder = शून्य;

  स्थिर काष्ठा curl_blob *ca_info_blob = conn_config->ca_info_blob;
  स्थिर अक्षर * स्थिर ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? शून्य : conn_config->CAfile);
  CURLcode परिणाम = CURLE_OK;
  rustls_परिणाम rr;

  DEBUGASSERT(backend);
  rconn = backend->conn;

  परिणाम = init_config_builder(cf, तथ्य, conn_config, &config_builder);
  अगर(परिणाम != CURLE_OK) {
    वापस परिणाम;
  }

  अगर(connssl->alpn) {
    init_config_builder_alpn(तथ्य, connssl, config_builder);
  }

  अगर(!conn_config->verifypeer) {
    rustls_client_config_builder_dangerous_set_certificate_verifier(
      config_builder, cr_verify_none);
  }
  अन्यथा अगर(conn_config->native_ca_store) {
    अगर(conn_config->CRLfile) {
      failf(तथ्य, "rustls: CRL file not supported with native CA store; "
            "the platform verifier has no CRL attachment API");
      rustls_client_config_builder_free(config_builder);
      वापस CURLE_NOT_BUILT_IN;
    }
    परिणाम = init_config_builder_platform_verifier(तथ्य, config_builder);
    अगर(परिणाम != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      वापस परिणाम;
    }
  }
  अन्यथा अगर(ca_info_blob || ssl_cafile) {
    परिणाम = init_config_builder_verifier(तथ्य,
                                          config_builder,
                                          conn_config,
                                          ca_info_blob,
                                          ssl_cafile);
    अगर(परिणाम != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      वापस परिणाम;
    }
  }

  अगर(conn_config->clientcert || conn_config->cert_blob ||
     conn_config->कुंजी || conn_config->कुंजी_blob) {
    परिणाम = init_config_builder_client_auth(तथ्य,
                                             conn_config,
                                             config_builder);
    अगर(परिणाम != CURLE_OK) {
      rustls_client_config_builder_free(config_builder);
      वापस परिणाम;
    }
  }

#अगर_घोषित USE_ECH
  अगर(Curl_ssl_ech_enabled(cf)) {
    परिणाम = init_config_builder_ech(तथ्य, cf, config_builder);
    अगर((परिणाम != CURLE_OK) && (conn_config->ech == CURLECH_HARD)) {
      rustls_client_config_builder_free(config_builder);
      वापस परिणाम;
    }
  }
#पूर्ण_अगर /* USE_ECH */

  परिणाम = init_config_builder_keylog(तथ्य, config_builder);
  अगर(परिणाम != CURLE_OK) {
    rustls_client_config_builder_free(config_builder);
    वापस परिणाम;
  }

  rr = rustls_client_config_builder_build(config_builder, &backend->config);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "failed to build client config");
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  DEBUGASSERT(!rconn);
  rr = rustls_client_connection_new(backend->config,
                                    connssl->peer.origin->hostname,
                                    &rconn);
  अगर(rr != RUSTLS_परिणाम_OK) {
    rustls_failf(तथ्य, rr, "rustls_client_connection_new");
    rustls_client_config_free(backend->config);
    backend->config = शून्य;
    वापस CURLE_COULDNT_CONNECT;
  }
  DEBUGASSERT(rconn);
  rustls_connection_set_userdata(rconn, backend);
  backend->conn = rconn;

  वापस परिणाम;
}

अटल व्योम cr_set_negotiated_alpn(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य,
                                   स्थिर काष्ठा rustls_connection *rconn)
{
  काष्ठा ssl_connect_तथ्य * स्थिर connssl = cf->ctx;
  स्थिर uint8_t *protocol = शून्य;
  माप_प्रकार len = 0;

  rustls_connection_get_alpn_protocol(rconn, &protocol, &len);
  Curl_alpn_set_negotiated(cf, तथ्य, connssl, protocol, len);
}

/* Given an established network connection, do a TLS handshake.
 *
 * This function sets `*done` to true once the handshake is complete.
 * This function never reads the value of `*done*`.
 */
अटल CURLcode cr_connect(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                           bool *done)
{
  काष्ठा ssl_connect_तथ्य * स्थिर connssl = cf->ctx;
  स्थिर काष्ठा rustls_ssl_backend_तथ्य * स्थिर backend =
    (काष्ठा rustls_ssl_backend_तथ्य *)connssl->backend;
  स्थिर काष्ठा rustls_connection *rconn = शून्य;
  CURLcode tmperr = CURLE_OK;
  CURLcode परिणाम;
  bool wants_read;
  bool wants_write;
  ssize_t nread;

  DEBUGASSERT(backend);

  CURL_TRC_CF(तथ्य, cf, "cr_connect, state=%d", (पूर्णांक)connssl->state);
  *done = FALSE;

#अगर_घोषित USE_ECH
    /* if we do ECH and need the HTTPS-RR information for it,
     * we delay the connect until it arrives or DNS resolve fails. */
    अगर(Curl_ssl_need_httpsrr(cf) &&
       !Curl_conn_dns_resolved_https(तथ्य, cf->sockindex,
                                     connssl->peer.peer)) {
      CURL_TRC_CF(तथ्य, cf, "need HTTPS-RR for ECH, delaying connect");
      वापस CURLE_OK;
    }
#पूर्ण_अगर /* USE_ECH */

  अगर(!backend->conn) {
    परिणाम =
      cr_init_backend(cf, तथ्य,
                      (काष्ठा rustls_ssl_backend_तथ्य *)connssl->backend);
    CURL_TRC_CF(तथ्य, cf, "cr_connect, init backend -> %d", (पूर्णांक)परिणाम);
    अगर(परिणाम)
      वापस परिणाम;
    connssl->state = ssl_connection_negotiating;
  }
  rconn = backend->conn;

  /* Read/write data until the handshake is done or the socket would block. */
  क्रम(;;) {
    /*
     * Connection has been established according to Rustls. Set send/recv
     * handlers, and update the state machine.
     */
    connssl->io_need = CURL_SSL_IO_NEED_NONE;
    अगर(!rustls_connection_is_handshaking(rconn)) {
      /* Rustls claims it is no longer handshaking *before* it has
       * send its FINISHED message off. We attempt to let it write
       * one more time. Oh my.
       */
      माप_प्रकार nwritten;
      cr_set_negotiated_alpn(cf, तथ्य, rconn);
      tmperr = cr_send(cf, तथ्य, शून्य, 0, &nwritten);
      अगर(tmperr == CURLE_AGAIN) {
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        वापस CURLE_OK;
      }
      अन्यथा अगर(tmperr != CURLE_OK) {
        वापस tmperr;
      }
      /* REALLY Done with the handshake. */
      {
#अगर_घोषित CURLVERBOSE
        स्थिर uint16_t proto = rustls_connection_get_protocol_version(rconn);
        स्थिर rustls_str ciphersuite_name =
          rustls_connection_get_negotiated_ciphersuite_name(rconn);
        स्थिर rustls_str kex_group_name =
          rustls_connection_get_negotiated_कुंजी_exchange_group_name(rconn);
        स्थिर अक्षर *ver = "TLS version unknown";
        अगर(proto == RUSTLS_TLS_VERSION_TLSV1_3)
          ver = "TLSv1.3";
        अगर(proto == RUSTLS_TLS_VERSION_TLSV1_2)
          ver = "TLSv1.2";
#पूर्ण_अगर
        infof(तथ्य,
              "rustls: handshake complete, %s, ciphersuite: %.*s, "
              "key exchange group: %.*s",
              ver,
              (पूर्णांक)ciphersuite_name.len,
              ciphersuite_name.तथ्य,
              (पूर्णांक)kex_group_name.len,
              kex_group_name.तथ्य);
      }
      अगर(तथ्य->set.ssl.certinfo) {
        माप_प्रकार अंक_certs = 0;
        माप_प्रकार i;
        जबतक(rustls_connection_get_peer_certificate(rconn, अंक_certs)) {
          अंक_certs++;
          अगर(अंक_certs > MAX_ALLOWED_CERT_AMOUNT) {
            failf(तथ्य, "%zu certificates is more than allowed (%d)",
                  अंक_certs, MAX_ALLOWED_CERT_AMOUNT);
            वापस CURLE_SSL_CONNECT_ERROR;
          }
        }
        परिणाम = Curl_ssl_init_certinfo(तथ्य, (पूर्णांक)अंक_certs);
        अगर(परिणाम)
          वापस परिणाम;
        क्रम(i = 0; i < अंक_certs; i++) {
          स्थिर rustls_certificate *cert;
          स्थिर अचिन्हित अक्षर *der_तथ्य;
          माप_प्रकार der_len;
          rustls_परिणाम rresult = RUSTLS_परिणाम_OK;
          cert = rustls_connection_get_peer_certificate(rconn, i);
          DEBUGASSERT(cert); /* Should exist since we counted already */
          rresult = rustls_certificate_get_der(cert, &der_तथ्य, &der_len);
          अगर(rresult != RUSTLS_परिणाम_OK) {
            अक्षर errorbuf[255];
            माप_प्रकार errorlen;
            rustls_error(rresult, errorbuf, माप(errorbuf), &errorlen);
            failf(तथ्य,
                  "Failed getting DER of server certificate #%zu: %.*s", i,
                  (पूर्णांक)errorlen, errorbuf);
            वापस map_error(rresult);
          }
          परिणाम = Curl_extract_certinfo(तथ्य, (पूर्णांक)i,
                                         der_तथ्य, der_तथ्य + der_len);
          अगर(परिणाम)
            वापस परिणाम;
        }
      }

      connssl->state = ssl_connection_complete;
      *done = TRUE;
      वापस CURLE_OK;
    }

    connssl->connecting_state = ssl_connect_2;
    wants_read = rustls_connection_wants_read(rconn);
    wants_write = rustls_connection_wants_write(rconn) ||
                  backend->plain_out_buffered;
    DEBUGASSERT(wants_read || wants_write);

    अगर(wants_write) {
      माप_प्रकार nwritten;
      CURL_TRC_CF(तथ्य, cf, "rustls_connection wants us to write_tls.");
      tmperr = cr_send(cf, तथ्य, शून्य, 0, &nwritten);
      अगर(tmperr == CURLE_AGAIN) {
        CURL_TRC_CF(तथ्य, cf, "writing would block");
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        वापस CURLE_OK;
      }
      अन्यथा अगर(tmperr != CURLE_OK) {
        वापस tmperr;
      }
    }

    अगर(wants_read) {
      CURL_TRC_CF(तथ्य, cf, "rustls_connection wants us to read_tls.");
      nread = tls_recv_more(cf, तथ्य, &tmperr);
      अगर(nread == 0) {
        connssl->peer_closed = TRUE;
        failf(तथ्य, "TLS connect error: Connection closed abruptly");
        वापस CURLE_SSL_CONNECT_ERROR;
      }
      अगर(nread < 0) {
        अगर(tmperr == CURLE_AGAIN) {
          CURL_TRC_CF(तथ्य, cf, "reading would block");
          connssl->io_need = CURL_SSL_IO_NEED_RECV;
          वापस CURLE_OK;
        }
        अन्यथा अगर(tmperr == CURLE_RECV_ERROR) {
          वापस CURLE_SSL_CONNECT_ERROR;
        }
        अन्यथा {
          वापस tmperr;
        }
      }
    }
  }

  /* We should never fall through the loop. We should return either because
     the handshake is done or because we cannot read/write without blocking. */
  DEBUGASSERT(FALSE);
}

अटल व्योम *cr_get_internals(काष्ठा ssl_connect_तथ्य *connssl, CURLINFO info)
{
  काष्ठा rustls_ssl_backend_तथ्य *backend =
    (काष्ठा rustls_ssl_backend_तथ्य *)connssl->backend;
  (व्योम)info;
  DEBUGASSERT(backend);
  वापस backend->conn;
}

अटल CURLcode cr_shutdown(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                            स्थिर bool send_shutdown, bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा rustls_ssl_backend_तथ्य *backend =
    (काष्ठा rustls_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार i, nread = 0, nwritten;

  DEBUGASSERT(backend);
  अगर(!backend->conn || cf->shutdown) {
    *done = TRUE;
    जाओ out;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  अगर(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    अगर(send_shutdown) {
      rustls_connection_send_close_notify(backend->conn);
    }
  }

  परिणाम = cr_send(cf, तथ्य, शून्य, 0, &nwritten);
  अगर(परिणाम) {
    अगर(परिणाम == CURLE_AGAIN) {
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      परिणाम = CURLE_OK;
      जाओ out;
    }
    DEBUGASSERT(परिणाम);
    CURL_TRC_CF(तथ्य, cf, "shutdown send failed: %d", (पूर्णांक)परिणाम);
    जाओ out;
  }

  क्रम(i = 0; i < 10; ++i) {
    अक्षर buf[1024];
    परिणाम = cr_recv(cf, तथ्य, buf, (पूर्णांक)माप(buf), &nread);
    अगर(परिणाम)
      अवरोध;
  }

  अगर(परिणाम == CURLE_AGAIN) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    परिणाम = CURLE_OK;
  }
  अन्यथा अगर(परिणाम) {
    DEBUGASSERT(परिणाम);
    CURL_TRC_CF(तथ्य, cf, "shutdown, error: %d", (पूर्णांक)परिणाम);
  }
  अन्यथा अगर(nread == 0) {
    /* We got the close notify alert and are done. */
    *done = TRUE;
  }

out:
  cf->shutdown = (परिणाम || *done);
  वापस परिणाम;
}

अटल व्योम cr_close(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  स्थिर काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा rustls_ssl_backend_तथ्य *backend =
    (काष्ठा rustls_ssl_backend_तथ्य *)connssl->backend;

  (व्योम)तथ्य;
  DEBUGASSERT(backend);
  अगर(backend->conn) {
    rustls_connection_free(backend->conn);
    backend->conn = शून्य;
  }
  अगर(backend->config) {
    rustls_client_config_free(backend->config);
    backend->config = शून्य;
  }
}

अटल माप_प्रकार cr_version(अक्षर *बफर, माप_प्रकार आकार)
{
  स्थिर काष्ठा rustls_str ver = rustls_version();
  वापस curl_msnprintf(बफर, आकार, "%.*s", (पूर्णांक)ver.len, ver.तथ्य);
}

अटल CURLcode cr_random(काष्ठा Curl_easy *तथ्य, अचिन्हित अक्षर *entropy,
                          माप_प्रकार लंबाई)
{
  rustls_परिणाम rresult = 0;
  (व्योम)तथ्य;
  rresult = rustls_default_crypto_provider_random(entropy, लंबाई);
  वापस map_error(rresult);
}

अटल व्योम cr_cleanup(व्योम)
{
  Curl_tls_keylog_close();
}

स्थिर काष्ठा Curl_ssl Curl_ssl_rustls = {
  { CURLSSLBACKEND_RUSTLS, "rustls" },
  SSLSUPP_CAINFO_BLOB |            /* supports */
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
  SSLSUPP_TLS13_CIPHERSUITES |
  SSLSUPP_CERTINFO |
  SSLSUPP_ECH |
  SSLSUPP_CRLFILE,
  माप(काष्ठा rustls_ssl_backend_तथ्य),

  शून्य,                            /* init */
  cr_cleanup,                      /* cleanup */
  cr_version,                      /* version */
  cr_shutdown,                     /* shutdown */
  cr_तथ्य_pending,                 /* data_pending */
  cr_random,                       /* random */
  शून्य,                            /* cert_status_request */
  cr_connect,                      /* connect */
  Curl_ssl_adjust_pollset,         /* adjust_pollset */
  cr_get_internals,                /* get_internals */
  cr_close,                        /* close_one */
  शून्य,                            /* close_all */
  शून्य,                            /* engines_list */
  शून्य,                            /* sha256sum */
  cr_recv,                         /* recv decrypted data */
  cr_send,                         /* send data to encrypt */
  शून्य,                            /* get_channel_binding */
};

#पूर्ण_अगर /* USE_RUSTLS */
