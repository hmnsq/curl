/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 * Copyright (C) Hoi-Ho Chan, <hoiho.chan@gmail.com>
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
/*
 * Source file for all mbedTLS-specific code for the TLS/SSL layer. No code
 * but vtls.c should ever call or use these functions.
 *
 */
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#অন্তর্ভাব <mbedtls/version.h>
#যদি MBEDTLS_VERSION_সংখ্যা < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#অন্তর্ভাব <psa/crypto_config.h>
#অন্তর্ভাব <mbedtls/net_sockets.h>
#অন্তর্ভাব <mbedtls/ssl.h>
#অন্তর্ভাব <mbedtls/x509.h>
#অন্তর্ভাব <mbedtls/psa_util.h>

#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#অন্তর্ভাব <mbedtls/error.h>
#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
#অন্তর্ভাব <mbedtls/entropy.h>
#অন্তর্ভাব <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#অন্তর্ভাব <mbedtls/debug.h>
#endif

#অন্তর্ভাব "vtls/cipher_suite.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "vtls/mbedtls.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vtls/x509asn1.h"
#অন্তর্ভাব "connect.h" /* for the connect timeout */
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curl_sha256.h"

/* ALPN for http2 */
#যদি defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

সংরচনা mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  ধ্রুব বর্ণ *protocols[3];
#endif
  পূর্ণাঙ্ক *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
সংরচনা rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

স্থির সংরচনা rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
স্থির শূন্য mbed_debug(শূন্য *context, পূর্ণাঙ্ক level, ধ্রুব বর্ণ *f_name,
                       পূর্ণাঙ্ক সারি_nb, ধ্রুব বর্ণ *সারি)
{
  সংরচনা Curl_easy *data = (সংরচনা Curl_easy *)context;
  (শূন্য)level;
  (শূন্য)সারি_nb;
  (শূন্য)f_name;

  যদি(data) {
    size_t len = strlen(সারি);
    যদি(len && (সারি[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (পূর্ণাঙ্ক)len, সারি);
  }
}
#endif

স্থির পূর্ণাঙ্ক mbedtls_bio_cf_write(শূন্য *bio,
                                ধ্রুব unsigned বর্ণ *buf, size_t blen)
{
  সংরচনা Curl_cfilter *cf = bio;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  যদি(!data)
    প্রত্যাগম 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (পূর্ণাঙ্ক)result, nwritten);
  যদি(result == CURLE_AGAIN)
    প্রত্যাগম MBEDTLS_ERR_SSL_WANT_WRITE;
  প্রত্যাগম result ? -1 : (পূর্ণাঙ্ক)nwritten;
}

স্থির পূর্ণাঙ্ক mbedtls_bio_cf_read(শূন্য *bio, unsigned বর্ণ *buf, size_t blen)
{
  সংরচনা Curl_cfilter *cf = bio;
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  যদি(!data)
    প্রত্যাগম 0;
  /* OpenSSL catches this case, so should we. */
  যদি(!buf)
    প্রত্যাগম 0;

  result = Curl_conn_cf_recv(cf->next, data, (বর্ণ *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (পূর্ণাঙ্ক)result, nread);
  যদি(result == CURLE_AGAIN)
    প্রত্যাগম MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  প্রত্যাগম result ? -1 : (পূর্ণাঙ্ক)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

স্থির CURLcode mbed_set_ssl_version_min_max(
  সংরচনা Curl_easy *data,
  সংরচনা mbed_ssl_backend_data *backend,
  সংরচনা ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#অন্যথা
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#অন্যথা
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  বিকল্প(conn_config->version) {
  পক্ষ CURL_SSLVERSION_TLSv1:
  পক্ষ CURL_SSLVERSION_TLSv1_0:
  পক্ষ CURL_SSLVERSION_TLSv1_1:
  পক্ষ CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    ভঙ্গ;
#endif
  পক্ষ CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    ভঙ্গ;
#endif
  স্বভাব:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

  বিকল্প(conn_config->version_max) {
  পক্ষ CURL_SSLVERSION_MAX_DEFAULT:
  পক্ষ CURL_SSLVERSION_MAX_NONE:
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    ভঙ্গ;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    ভঙ্গ;
#endif
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_1:
  পক্ষ CURL_SSLVERSION_MAX_TLSv1_0:
  স্বভাব:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  প্রত্যাগম CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
স্থির পূর্ণাঙ্ক mbed_cipher_suite_get_str(uint16_t id, বর্ণ *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  যদি(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  অন্যথা
    প্রত্যাগম Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  প্রত্যাগম 0;
}

স্থির uint16_t mbed_cipher_suite_walk_str(ধ্রুব বর্ণ **str, ধ্রুব বর্ণ **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  স্থির ধ্রুব বর্ণ ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  যদি(!id) {
    যদি((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  প্রত্যাগম id;
}
#অন্যথা
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

স্থির CURLcode mbed_set_selected_ciphers(
  সংরচনা Curl_easy *data,
  সংরচনা mbed_ssl_backend_data *backend,
  ধ্রুব বর্ণ *ciphers12,
  ধ্রুব বর্ণ *ciphers13)
{
  ধ্রুব বর্ণ *ciphers = ciphers12;
  ধ্রুব পূর্ণাঙ্ক *supported;
  পূর্ণাঙ্ক *selected;
  size_t supported_len, গুনতি = 0, default13_গুনতি = 0, i, j;
  ধ্রুব বর্ণ *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  প্রতি(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(পরিমাণ(পূর্ণাঙ্ক) * (supported_len + 1));
  যদি(!selected)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (শূন্য)ciphers13, (শূন্য)j;
#অন্যথা
  যদি(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    প্রতি(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      যদি(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        অনুবর্ত;

      selected[গুনতি++] = id;
    }

    default13_গুনতি = গুনতি;
  }
  অন্যথা
    ciphers = ciphers13;

add_ciphers:
#endif
  প্রতি(ptr = ciphers; ptr[0] != '\0' && গুনতি < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    যদি(id) {
      প্রতি(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      যদি(i == supported_len)
        id = 0;
    }
    যদি(!id) {
      যদি(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (পূর্ণাঙ্ক)(end - ptr), ptr);
      অনুবর্ত;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    প্রতি(i = 0; i < গুনতি && selected[i] != id; i++)
      ;
    যদি(i < গুনতি) {
      যদি(i >= default13_গুনতি)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (পূর্ণাঙ্ক)(end - ptr), ptr);
      অনুবর্ত;
    }

    selected[গুনতি++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  যদি(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    গচ্ছ add_ciphers;
  }

  যদি(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    প্রতি(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      যদি(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        অনুবর্ত;

      /* No duplicates allowed (so selected cannot overflow) */
      প্রতি(i = 0; i < গুনতি && selected[i] != id; i++)
        ;
      যদি(i < গুনতি)
        অনুবর্ত;

      selected[গুনতি++] = id;
    }
  }
#endif

  selected[গুনতি] = 0;

  যদি(গুনতি == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    প্রত্যাগম CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  প্রত্যাগম CURLE_OK;
}

স্থির শূন্য mbed_dump_cert_info(সংরচনা Curl_easy *data,
                                ধ্রুব mbedtls_x509_crt *crt)
{
#যদি !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (শূন্য)data, (শূন্য)crt;
#অন্যথা
  ধ্রুব size_t bufsize = 16384;
  বর্ণ *p, *buffer = curlx_malloc(bufsize);

  যদি(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    প্রতি(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (পূর্ণাঙ্ক)s, p);
      p += s;
    }
  }
  অন্যথা
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

স্থির শূন্য mbed_extract_certinfo(সংরচনা Curl_easy *data,
                                  ধ্রুব mbedtls_x509_crt *crt)
{
  CURLcode result;
  ধ্রুব mbedtls_x509_crt *cur;
  পূর্ণাঙ্ক cert_গুনতি = 0;
  পূর্ণাঙ্ক i;

  প্রতি(cur = crt; cur && cert_গুনতি <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_গুনতি++;

  যদি(cert_গুনতি > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    প্রত্যাগম;
  }

  result = Curl_ssl_init_certinfo(data, cert_গুনতি);

  প্রতি(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

স্থির পূর্ণাঙ্ক mbed_verify_cb(শূন্য *ptr, mbedtls_x509_crt *crt,
                          পূর্ণাঙ্ক depth, uint32_t *flags)
{
  সংরচনা Curl_cfilter *cf = (সংরচনা Curl_cfilter *)ptr;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা Curl_easy *data = CF_DATA_CURRENT(cf);

  যদি(depth == 0) {
    যদি(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    যদি(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  যদি(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  যদি(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  যদি(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    বর্ণ buf[128];
    mbedtls_x509_crt_verify_info(buf, পরিমাণ(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#অন্যথা
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned পূর্ণাঙ্ক)*flags);
#endif
  }

  প্রত্যাগম 0;
}

স্থির CURLcode mbed_load_cacert(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ধ্রুব সংরচনা curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ধ্রুব বর্ণ * ধ্রুব ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? রিক্ত : conn_config->CAfile);
  ধ্রুব bool verifypeer = conn_config->verifypeer;
  ধ্রুব বর্ণ * ধ্রুব ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  ধ্রুব বর্ণ * ধ্রুব ssl_cert_type = conn_config->cert_type;
#endif
  পূর্ণাঙ্ক ret = -1;
  বর্ণ errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  যদি(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    যদি((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((বর্ণ *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    অন্যথা { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned বর্ণ *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      যদি(!newblob)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#অন্যথা
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    যদি(ret < 0) {
      mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned পূর্ণাঙ্ক)-ret, errorbuf);
      প্রত্যাগম CURLE_SSL_CERTPROBLEM;
    }
  }

  যদি(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    যদি(ret < 0) {
      mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned পূর্ণাঙ্ক)-ret, errorbuf);
      প্রত্যাগম CURLE_SSL_CACERT_BADFILE;
    }
#অন্যথা
    failf(data, "mbedTLS: functions that use the file system not built in");
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
  }

  যদি(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    যদি(ret < 0) {
      mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned পূর্ণাঙ্ক)-ret, errorbuf);

      যদি(verifypeer)
        প্রত্যাগম CURLE_SSL_CACERT_BADFILE;
    }
#অন্যথা
    failf(data, "mbedTLS: functions that use the file system not built in");
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode mbed_load_clicert(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  বর্ণ * ধ্রুব ssl_cert = conn_config->clientcert;
  ধ্রুব সংরচনা curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  ধ্রুব বর্ণ * ধ্রুব ssl_cert_type = conn_config->cert_type;
#endif
  পূর্ণাঙ্ক ret = -1;
  বর্ণ errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  যদি(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    যদি(ret) {
      mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned পূর্ণাঙ্ক)-ret, errorbuf);

      প্রত্যাগম CURLE_SSL_CERTPROBLEM;
    }
#অন্যথা
    failf(data, "mbedTLS: functions that use the file system not built in");
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
  }

  যদি(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    যদি((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((বর্ণ *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    অন্যথা { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned বর্ণ *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      যদি(!newblob)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#অন্যথা
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    যদি(ret) {
      mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned পূর্ণাঙ্ক)-ret, errorbuf);
      প্রত্যাগম CURLE_SSL_CERTPROBLEM;
    }
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode mbed_load_privkey(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  পূর্ণাঙ্ক ret = -1;
  বর্ণ errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  যদি(conn_config->চাবি || conn_config->চাবি_blob) {
    যদি(conn_config->চাবি) {
#ifdef MBEDTLS_FS_IO
#যদি MBEDTLS_VERSION_সংখ্যা >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->চাবি,
                                     conn_config->চাবি_passwd);
      যদি(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_চাবি_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_চাবি_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#অন্যথা
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->চাবি,
                                     conn_config->চাবি_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      যদি(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      যদি(ret) {
        mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->চাবি, (unsigned পূর্ণাঙ্ক)-ret, errorbuf);
        প্রত্যাগম CURLE_SSL_CERTPROBLEM;
      }
#অন্যথা
      failf(data, "mbedTLS: functions that use the file system not built in");
      প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
    }
    অন্যথা {
      ধ্রুব সংরচনা curl_blob *ssl_চাবি_blob = conn_config->চাবি_blob;
      ধ্রুব বর্ণ *passwd = conn_config->চাবি_passwd;
      ধ্রুব bool is_der = conn_config->চাবি_type &&
                          curl_strequal(conn_config->চাবি_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned বর্ণ *newblob = curlx_memdup0(ssl_চাবি_blob->data,
                                             ssl_চাবি_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_চাবি_blob->len : ssl_চাবি_blob->len + 1;
      যদি(!newblob)
        প্রত্যাগম CURLE_OUT_OF_MEMORY;

#যদি MBEDTLS_VERSION_সংখ্যা >= 0x04000000
      ret = mbedtls_pk_parse_চাবি(&backend->pk, newblob, keylen,
                                 (ধ্রুব unsigned বর্ণ *)passwd,
                                 passwd ? strlen(passwd) : 0);
      যদি(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_চাবি_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_চাবি_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#অন্যথা
      ret = mbedtls_pk_parse_চাবি(&backend->pk, newblob, keylen,
                                 (ধ্রুব unsigned বর্ণ *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      যদি(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      যদি(ret) {
        mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned পূর্ণাঙ্ক)-ret, errorbuf);
        প্রত্যাগম CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode mbed_load_crl(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ধ্রুব বর্ণ * ধ্রুব ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  যদি(ssl_crlfile) {
    বর্ণ errorbuf[128];
#ifdef MBEDTLS_FS_IO
    পূর্ণাঙ্ক ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    যদি(ret) {
      mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned পূর্ণাঙ্ক)-ret, errorbuf);

      প্রত্যাগম CURLE_SSL_CRL_BADFILE;
    }
#অন্যথা
    (শূন্য)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    প্রত্যাগম CURLE_NOT_BUILT_IN;
#endif
  }
#অন্যথা
  (শূন্য)backend;
  যদি(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  }
#endif

  প্রত্যাগম CURLE_OK;
}

স্থির bool mbed_apply_session(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা Curl_ssl_session *sc_session)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;

  যদি(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    পূর্ণাঙ্ক ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    যদি(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned পূর্ণাঙ্ক)-ret);
    }
    অন্যথা {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      যদি(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned পূর্ণাঙ্ক)-ret);
    }
    mbedtls_ssl_session_free(&session);
    যদি(!ret) {
      infof(data, "SSL reusing session ID");
      প্রত্যাগম TRUE;
    }
  }
  প্রত্যাগম FALSE;
}

স্থির CURLcode mbed_configure_ssl(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  পূর্ণাঙ্ক ret;
  bool session_applied = FALSE;
  CURLcode result;
  বর্ণ errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  যদি(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

#ifdef MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, data);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_set_threshold(4);
#endif

#যদি defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_সংখ্যা >= 0x03060100 &&   \
  MBEDTLS_VERSION_সংখ্যা < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#endif

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_next);

  result = mbed_set_ssl_version_min_max(data, backend, conn_config);
  যদি(result)
    প্রত্যাগম result;

#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  যদি(ret) {
    mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned পূর্ণাঙ্ক)-ret, errorbuf);
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      রিক্ত /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  যদি(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, রিক্ত);
#অন্যথা
  যদি(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    যদি(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      প্রত্যাগম result;
    }
  }
  অন্যথা {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#ifdef MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#endif

#ifdef MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#endif

  যদি((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    সংরচনা Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    যদি(scs) {
      যদি(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  যদি(!session_applied && Curl_ssl_scache_use(cf, data)) {
    সংরচনা Curl_ssl_session *sc_session = রিক্ত;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_চাবি,
                                            &sc_session);
    যদি(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      যদি(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned পূর্ণাঙ্ক)-ret);
      }
      অন্যথা {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        যদি(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned পূর্ণাঙ্ক)-ret);
        অন্যথা
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_চাবি,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#অন্যথা
                            রিক্ত
#endif
    );

  যদি(conn_config->চাবি || conn_config->চাবি_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  যদি(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  যদি(connssl->alpn) {
    সংরচনা alpn_proto_buf proto;
    size_t i;

    প্রতি(i = 0; i < connssl->alpn->গুনতি; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    যদি(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  যদি(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    যদি(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  প্রত্যাগম result;
}

স্থির CURLcode mbed_connect_step1(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  যদি((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    প্রত্যাগম CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  যদি(!result)
    result = mbed_load_clicert(cf, data);
  যদি(!result)
    result = mbed_load_privkey(cf, data);
  যদি(!result)
    result = mbed_load_crl(cf, data);
  যদি(!result)
    result = mbed_configure_ssl(cf, data);
  যদি(result)
    প্রত্যাগম result;

  connssl->connecting_state = ssl_connect_2;
  প্রত্যাগম CURLE_OK;
}

#যদি defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

স্থির CURLcode mbed_connect_step2(সংরচনা Curl_cfilter *cf,
                                   সংরচনা Curl_easy *data)
{
#যদি defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  পূর্ণাঙ্ক ret;
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  ধ্রুব বর্ণ * ধ্রুব pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#অন্যথা
  ধ্রুব বর্ণ * ধ্রুব pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  যদি(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    প্রত্যাগম CURLE_OK;
  }
  অন্যথা যদি(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    প্রত্যাগম CURLE_OK;
  }
  অন্যথা যদি(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    প্রত্যাগম CURLE_PEER_FAILED_VERIFICATION;
  }
  অন্যথা যদি(ret) {
    বর্ণ errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_সংখ্যা(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, পরিমাণ(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned পূর্ণাঙ্ক)-ret, errorbuf);
    প্রত্যাগম CURLE_SSL_CONNECT_ERROR;
  }

  {
    বর্ণ cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, পরিমাণ(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  যদি(pinnedpubkey) {
    পূর্ণাঙ্ক size;
    ধ্রুব mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = রিক্ত;
    unsigned বর্ণ *pubkey = রিক্ত;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    যদি(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      প্রত্যাগম CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, পরিমাণ(*p));

    যদি(!p)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    যদি(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    যদি(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      গচ্ছ pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    যদি(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      গচ্ছ pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    যদি(result)
      প্রত্যাগম result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  যদি(connssl->alpn) {
    ধ্রুব বর্ণ *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ধ্রুব unsigned বর্ণ *)proto,
                                      proto ? strlen(proto) : 0);
    যদি(result)
      প্রত্যাগম result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode mbed_new_session(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  সংরচনা Curl_ssl_session *sc_session = রিক্ত;
  unsigned বর্ণ *sdata = রিক্ত;
  size_t slen = 0;
  পূর্ণাঙ্ক ietf_tls_id;
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক ret;

  DEBUGASSERT(backend);
  যদি(!Curl_ssl_scache_use(cf, data))
    প্রত্যাগম CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  যদি(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned পূর্ণাঙ্ক)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    গচ্ছ out;
  }

  mbedtls_ssl_session_save(&session, রিক্ত, 0, &slen);
  যদি(!slen) {
    failf(data, "failed to serialize session: length is 0");
    গচ্ছ out;
  }

  sdata = curlx_malloc(slen);
  যদি(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  যদি(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned পূর্ণাঙ্ক)-ret);
    গচ্ছ out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_সংখ্যা(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = রিক্ত;  /* call took ownership */
  যদি(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  যদি(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_চাবি,
                                 sc_session);
    sc_session = রিক্ত;
  }

out:
  যদি(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  প্রত্যাগম result;
}

স্থির CURLcode mbed_send(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                          ধ্রুব শূন্য *mem, size_t len, size_t *pnwritten)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক nwritten;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#endif
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  যদি(backend->send_blocked) {
    যদি(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      গচ্ছ out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ধ্রুব unsigned বর্ণ *)mem, len);

  যদি(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  অন্যথা {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned পূর্ণাঙ্ক)-nwritten);
    বিকল্প(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    পক্ষ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      যদি(!result)
        গচ্ছ do_send;
      ভঙ্গ;
#endif
    পক্ষ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      ভঙ্গ;
    পক্ষ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      ভঙ্গ;
    স্বভাব:
      result = CURLE_SEND_ERROR;
      ভঙ্গ;
    }
    যদি((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (পূর্ণাঙ্ক)result, *pnwritten);
  প্রত্যাগম result;
}

স্থির CURLcode mbedtls_shutdown(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  unsigned বর্ণ buf[1024];
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  যদি(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  যদি(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    যদি(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      বিকল্প(ret) {
      পক্ষ 0: /* we sent it, receive from the server */
        ভঙ্গ;
      পক্ষ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        গচ্ছ out;
      পক্ষ MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        গচ্ছ out;
      পক্ষ MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        গচ্ছ out;
      স্বভাব:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned পূর্ণাঙ্ক)-ret);
        result = CURLE_RECV_ERROR;
        গচ্ছ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  প্রতি(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, পরিমাণ(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    যদি(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, পরিমাণ(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    যদি(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      অনুবর্ত;
#endif
    যদি(ret <= 0)
      ভঙ্গ;
  }

  যদি(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  অন্যথা যদি(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  অন্যথা যদি(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  অন্যথা যদি(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  অন্যথা {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned পূর্ণাঙ্ক)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  প্রত্যাগম result;
}

স্থির শূন্য mbedtls_close(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;

  (শূন্য)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  যদি(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

স্থির CURLcode mbed_recv(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                          বর্ণ *buf, size_t buffersize, size_t *pnread)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  পূর্ণাঙ্ক nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned বর্ণ *)buf, buffersize);
  যদি(nread > 0)
    *pnread = (size_t)nread;
  অন্যথা {
    বর্ণ errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned পূর্ণাঙ্ক)-nread);
    বিকল্প(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    পক্ষ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      যদি(!result)
        গচ্ছ do_read;
      ভঙ্গ;
#endif
    পক্ষ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      ভঙ্গ;
    পক্ষ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      ভঙ্গ;
    পক্ষ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      ভঙ্গ;
    স্বভাব:
      mbedtls_strerror(nread, errorbuf, পরিমাণ(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned পূর্ণাঙ্ক)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      ভঙ্গ;
    }
  }
  প্রত্যাগম result;
}

স্থির size_t mbedtls_version(বর্ণ *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned পূর্ণাঙ্ক version = mbedtls_version_get_সংখ্যা();
  প্রত্যাগম curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#অন্যথা
  প্রত্যাগম curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
স্থির CURLcode mbedtls_random(সংরচনা Curl_easy *data,
                               unsigned বর্ণ *entropy, size_t length)
{
  psa_status_t status;
  (শূন্য)data;

  status = psa_generate_random(entropy, length);

  প্রত্যাগম status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

স্থির CURLcode mbedtls_connect(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  সংরচনা ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  যদি(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  যদি(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    যদি(result)
      প্রত্যাগম result;
  }

  যদি(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    যদি(result)
      প্রত্যাগম result;
  }

  যদি(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    সংরচনা ssl_connect_data *ctx = cf->ctx;
    সংরচনা mbed_ssl_backend_data *backend =
      (সংরচনা mbed_ssl_backend_data *)ctx->backend;

    যদি(mbedtls_ssl_get_version_সংখ্যা(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      যদি(result)
        প্রত্যাগম result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  যদি(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  প্রত্যাগম CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
স্থির পূর্ণাঙ্ক mbedtls_init(শূন্য)
{
#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
  পূর্ণাঙ্ক ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  যদি(status != PSA_SUCCESS)
    প্রত্যাগম 0;

#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              রিক্ত, 0);

  যদি(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    প্রত্যাগম 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  প্রত্যাগম 1;
}

স্থির শূন্য mbedtls_cleanup(শূন্য)
{
  mbedtls_psa_crypto_free();

#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

স্থির bool mbedtls_data_pending(সংরচনা Curl_cfilter *cf,
                                 ধ্রুব সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *ctx = cf->ctx;
  সংরচনা mbed_ssl_backend_data *backend;

  (শূন্য)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (সংরচনা mbed_ssl_backend_data *)ctx->backend;
  প্রত্যাগম mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

স্থির CURLcode mbedtls_sha256sum(ধ্রুব unsigned বর্ণ *প্রবেশ,
                                  size_t inputlen,
                                  unsigned বর্ণ *sha256sum,
                                  size_t sha256len)
{
#যদি defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, প্রবেশ, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  যদি(status != PSA_SUCCESS)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  প্রত্যাগম CURLE_OK;
#অন্যথা
  (শূন্য)sha256len;
  প্রত্যাগম Curl_sha256it(sha256sum, প্রবেশ, inputlen);
#endif
}

স্থির শূন্য *mbedtls_get_internals(সংরচনা ssl_connect_data *connssl,
                                   CURLINFO info)
{
  সংরচনা mbed_ssl_backend_data *backend =
    (সংরচনা mbed_ssl_backend_data *)connssl->backend;
  (শূন্য)info;
  DEBUGASSERT(backend);
  প্রত্যাগম &backend->ssl;
}

ধ্রুব সংরচনা Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_PATH |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#ifdef HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#endif
  SSLSUPP_SSL_CTX |
#ifdef MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#endif
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#ifdef MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#endif
  0,

  পরিমাণ(সংরচনা mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  রিক্ত,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  রিক্ত,                             /* close_all */
  রিক্ত,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  রিক্ত,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
