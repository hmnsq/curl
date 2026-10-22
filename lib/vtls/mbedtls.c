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
#شامل "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#شامل <mbedtls/version.h>
#اگر MBEDTLS_VERSION_عدد < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#شامل <psa/crypto_config.h>
#شامل <mbedtls/net_sockets.h>
#شامل <mbedtls/ssl.h>
#شامل <mbedtls/x509.h>
#شامل <mbedtls/psa_util.h>

#اگر MBEDTLS_VERSION_عدد < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#شامل <mbedtls/error.h>
#اگر MBEDTLS_VERSION_عدد < 0x04000000
#شامل <mbedtls/entropy.h>
#شامل <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#شامل <mbedtls/debug.h>
#endif

#شامل "vtls/cipher_suite.h"
#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "vtls/mbedtls.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_int.h"
#شامل "vtls/vtls_scache.h"
#شامل "vtls/x509asn1.h"
#شامل "connect.h" /* for the connect timeout */
#شامل "curlx/strdup.h"
#شامل "curl_sha256.h"

/* ALPN for http2 */
#اگر defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

ساختار mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  ثابت نویسه *protocols[3];
#endif
  صحیح *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#اگر MBEDTLS_VERSION_عدد < 0x04000000
ساختار rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ایستا ساختار rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
ایستا تهی mbed_debug(تهی *context, صحیح level, ثابت نویسه *f_name,
                       صحیح سطر_nb, ثابت نویسه *سطر)
{
  ساختار Curl_easy *data = (ساختار Curl_easy *)context;
  (تهی)level;
  (تهی)سطر_nb;
  (تهی)f_name;

  اگر(data) {
    size_t len = strlen(سطر);
    اگر(len && (سطر[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (صحیح)len, سطر);
  }
}
#endif

ایستا صحیح mbedtls_bio_cf_write(تهی *bio,
                                ثابت unsigned نویسه *buf, size_t blen)
{
  ساختار Curl_cfilter *cf = bio;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  اگر(!data)
    بازگشت 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (صحیح)result, nwritten);
  اگر(result == CURLE_AGAIN)
    بازگشت MBEDTLS_ERR_SSL_WANT_WRITE;
  بازگشت result ? -1 : (صحیح)nwritten;
}

ایستا صحیح mbedtls_bio_cf_read(تهی *bio, unsigned نویسه *buf, size_t blen)
{
  ساختار Curl_cfilter *cf = bio;
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  اگر(!data)
    بازگشت 0;
  /* OpenSSL catches this case, so should we. */
  اگر(!buf)
    بازگشت 0;

  result = Curl_conn_cf_recv(cf->next, data, (نویسه *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (صحیح)result, nread);
  اگر(result == CURLE_AGAIN)
    بازگشت MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  بازگشت result ? -1 : (صحیح)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

ایستا CURLcode mbed_set_ssl_version_min_max(
  ساختار Curl_easy *data,
  ساختار mbed_ssl_backend_data *backend,
  ساختار ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#وگرنه
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#وگرنه
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  انتخاب(conn_config->version) {
  حالت CURL_SSLVERSION_TLSv1:
  حالت CURL_SSLVERSION_TLSv1_0:
  حالت CURL_SSLVERSION_TLSv1_1:
  حالت CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    بشکن;
#endif
  حالت CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    بشکن;
#endif
  پیشفرض:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  انتخاب(conn_config->version_max) {
  حالت CURL_SSLVERSION_MAX_DEFAULT:
  حالت CURL_SSLVERSION_MAX_NONE:
  حالت CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    بشکن;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  حالت CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    بشکن;
#endif
  حالت CURL_SSLVERSION_MAX_TLSv1_1:
  حالت CURL_SSLVERSION_MAX_TLSv1_0:
  پیشفرض:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  بازگشت CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ایستا صحیح mbed_cipher_suite_get_str(uint16_t id, نویسه *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  اگر(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  وگرنه
    بازگشت Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  بازگشت 0;
}

ایستا uint16_t mbed_cipher_suite_walk_str(ثابت نویسه **str, ثابت نویسه **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  ایستا ثابت نویسه ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  اگر(!id) {
    اگر((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  بازگشت id;
}
#وگرنه
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

ایستا CURLcode mbed_set_selected_ciphers(
  ساختار Curl_easy *data,
  ساختار mbed_ssl_backend_data *backend,
  ثابت نویسه *ciphers12,
  ثابت نویسه *ciphers13)
{
  ثابت نویسه *ciphers = ciphers12;
  ثابت صحیح *supported;
  صحیح *selected;
  size_t supported_len, شمار = 0, default13_شمار = 0, i, j;
  ثابت نویسه *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  برای(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(اندازه(صحیح) * (supported_len + 1));
  اگر(!selected)
    بازگشت CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (تهی)ciphers13, (تهی)j;
#وگرنه
  اگر(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    برای(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      اگر(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ادامه;

      selected[شمار++] = id;
    }

    default13_شمار = شمار;
  }
  وگرنه
    ciphers = ciphers13;

add_ciphers:
#endif
  برای(ptr = ciphers; ptr[0] != '\0' && شمار < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    اگر(id) {
      برای(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      اگر(i == supported_len)
        id = 0;
    }
    اگر(!id) {
      اگر(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (صحیح)(end - ptr), ptr);
      ادامه;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    برای(i = 0; i < شمار && selected[i] != id; i++)
      ;
    اگر(i < شمار) {
      اگر(i >= default13_شمار)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (صحیح)(end - ptr), ptr);
      ادامه;
    }

    selected[شمار++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  اگر(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    برو add_ciphers;
  }

  اگر(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    برای(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      اگر(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ادامه;

      /* No duplicates allowed (so selected cannot overflow) */
      برای(i = 0; i < شمار && selected[i] != id; i++)
        ;
      اگر(i < شمار)
        ادامه;

      selected[شمار++] = id;
    }
  }
#endif

  selected[شمار] = 0;

  اگر(شمار == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    بازگشت CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  بازگشت CURLE_OK;
}

ایستا تهی mbed_dump_cert_info(ساختار Curl_easy *data,
                                ثابت mbedtls_x509_crt *crt)
{
#اگر !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (تهی)data, (تهی)crt;
#وگرنه
  ثابت size_t bufsize = 16384;
  نویسه *p, *buffer = curlx_malloc(bufsize);

  اگر(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    برای(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (صحیح)s, p);
      p += s;
    }
  }
  وگرنه
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

ایستا تهی mbed_extract_certinfo(ساختار Curl_easy *data,
                                  ثابت mbedtls_x509_crt *crt)
{
  CURLcode result;
  ثابت mbedtls_x509_crt *cur;
  صحیح cert_شمار = 0;
  صحیح i;

  برای(cur = crt; cur && cert_شمار <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_شمار++;

  اگر(cert_شمار > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    بازگشت;
  }

  result = Curl_ssl_init_certinfo(data, cert_شمار);

  برای(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ایستا صحیح mbed_verify_cb(تهی *ptr, mbedtls_x509_crt *crt,
                          صحیح depth, uint32_t *flags)
{
  ساختار Curl_cfilter *cf = (ساختار Curl_cfilter *)ptr;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار Curl_easy *data = CF_DATA_CURRENT(cf);

  اگر(depth == 0) {
    اگر(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    اگر(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  اگر(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  اگر(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  اگر(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    نویسه buf[128];
    mbedtls_x509_crt_verify_info(buf, اندازه(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#وگرنه
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned صحیح)*flags);
#endif
  }

  بازگشت 0;
}

ایستا CURLcode mbed_load_cacert(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت ساختار curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت نویسه * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? پوچ : conn_config->CAfile);
  ثابت bool verifypeer = conn_config->verifypeer;
  ثابت نویسه * ثابت ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  ثابت نویسه * ثابت ssl_cert_type = conn_config->cert_type;
#endif
  صحیح ret = -1;
  نویسه errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  اگر(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    اگر((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((نویسه *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    وگرنه { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned نویسه *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      اگر(!newblob)
        بازگشت CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#وگرنه
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    اگر(ret < 0) {
      mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned صحیح)-ret, errorbuf);
      بازگشت CURLE_SSL_CERTPROBLEM;
    }
  }

  اگر(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    اگر(ret < 0) {
      mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned صحیح)-ret, errorbuf);
      بازگشت CURLE_SSL_CACERT_BADFILE;
    }
#وگرنه
    failf(data, "mbedTLS: functions that use the file system not built in");
    بازگشت CURLE_NOT_BUILT_IN;
#endif
  }

  اگر(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    اگر(ret < 0) {
      mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned صحیح)-ret, errorbuf);

      اگر(verifypeer)
        بازگشت CURLE_SSL_CACERT_BADFILE;
    }
#وگرنه
    failf(data, "mbedTLS: functions that use the file system not built in");
    بازگشت CURLE_NOT_BUILT_IN;
#endif
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode mbed_load_clicert(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  نویسه * ثابت ssl_cert = conn_config->clientcert;
  ثابت ساختار curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  ثابت نویسه * ثابت ssl_cert_type = conn_config->cert_type;
#endif
  صحیح ret = -1;
  نویسه errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  اگر(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    اگر(ret) {
      mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned صحیح)-ret, errorbuf);

      بازگشت CURLE_SSL_CERTPROBLEM;
    }
#وگرنه
    failf(data, "mbedTLS: functions that use the file system not built in");
    بازگشت CURLE_NOT_BUILT_IN;
#endif
  }

  اگر(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    اگر((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((نویسه *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    وگرنه { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned نویسه *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      اگر(!newblob)
        بازگشت CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#وگرنه
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    اگر(ret) {
      mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned صحیح)-ret, errorbuf);
      بازگشت CURLE_SSL_CERTPROBLEM;
    }
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode mbed_load_privkey(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحیح ret = -1;
  نویسه errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  اگر(conn_config->کلید || conn_config->کلید_blob) {
    اگر(conn_config->کلید) {
#ifdef MBEDTLS_FS_IO
#اگر MBEDTLS_VERSION_عدد >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->کلید,
                                     conn_config->کلید_passwd);
      اگر(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وگرنه
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->کلید,
                                     conn_config->کلید_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      اگر(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      اگر(ret) {
        mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->کلید, (unsigned صحیح)-ret, errorbuf);
        بازگشت CURLE_SSL_CERTPROBLEM;
      }
#وگرنه
      failf(data, "mbedTLS: functions that use the file system not built in");
      بازگشت CURLE_NOT_BUILT_IN;
#endif
    }
    وگرنه {
      ثابت ساختار curl_blob *ssl_کلید_blob = conn_config->کلید_blob;
      ثابت نویسه *passwd = conn_config->کلید_passwd;
      ثابت bool is_der = conn_config->کلید_type &&
                          curl_strequal(conn_config->کلید_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned نویسه *newblob = curlx_memdup0(ssl_کلید_blob->data,
                                             ssl_کلید_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_کلید_blob->len : ssl_کلید_blob->len + 1;
      اگر(!newblob)
        بازگشت CURLE_OUT_OF_MEMORY;

#اگر MBEDTLS_VERSION_عدد >= 0x04000000
      ret = mbedtls_pk_parse_کلید(&backend->pk, newblob, keylen,
                                 (ثابت unsigned نویسه *)passwd,
                                 passwd ? strlen(passwd) : 0);
      اگر(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وگرنه
      ret = mbedtls_pk_parse_کلید(&backend->pk, newblob, keylen,
                                 (ثابت unsigned نویسه *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      اگر(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      اگر(ret) {
        mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned صحیح)-ret, errorbuf);
        بازگشت CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode mbed_load_crl(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت نویسه * ثابت ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  اگر(ssl_crlfile) {
    نویسه errorbuf[128];
#ifdef MBEDTLS_FS_IO
    صحیح ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    اگر(ret) {
      mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned صحیح)-ret, errorbuf);

      بازگشت CURLE_SSL_CRL_BADFILE;
    }
#وگرنه
    (تهی)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    بازگشت CURLE_NOT_BUILT_IN;
#endif
  }
#وگرنه
  (تهی)backend;
  اگر(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    بازگشت CURLE_NOT_BUILT_IN;
  }
#endif

  بازگشت CURLE_OK;
}

ایستا bool mbed_apply_session(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار Curl_ssl_session *sc_session)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;

  اگر(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    صحیح ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    اگر(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned صحیح)-ret);
    }
    وگرنه {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      اگر(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned صحیح)-ret);
    }
    mbedtls_ssl_session_free(&session);
    اگر(!ret) {
      infof(data, "SSL reusing session ID");
      بازگشت TRUE;
    }
  }
  بازگشت FALSE;
}

ایستا CURLcode mbed_configure_ssl(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحیح ret;
  bool session_applied = FALSE;
  CURLcode result;
  نویسه errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  اگر(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    بازگشت CURLE_SSL_CONNECT_ERROR;
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

#اگر defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_عدد >= 0x03060100 &&   \
  MBEDTLS_VERSION_عدد < 0x04000000
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
  اگر(result)
    بازگشت result;

#اگر MBEDTLS_VERSION_عدد < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  اگر(ret) {
    mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned صحیح)-ret, errorbuf);
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      پوچ /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  اگر(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, پوچ);
#وگرنه
  اگر(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    اگر(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      بازگشت result;
    }
  }
  وگرنه {
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

  اگر((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ساختار Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    اگر(scs) {
      اگر(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  اگر(!session_applied && Curl_ssl_scache_use(cf, data)) {
    ساختار Curl_ssl_session *sc_session = پوچ;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_کلید,
                                            &sc_session);
    اگر(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      اگر(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned صحیح)-ret);
      }
      وگرنه {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        اگر(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned صحیح)-ret);
        وگرنه
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_کلید,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#وگرنه
                            پوچ
#endif
    );

  اگر(conn_config->کلید || conn_config->کلید_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  اگر(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  اگر(connssl->alpn) {
    ساختار alpn_proto_buf proto;
    size_t i;

    برای(i = 0; i < connssl->alpn->شمار; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    اگر(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      بازگشت CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  اگر(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    اگر(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  بازگشت result;
}

ایستا CURLcode mbed_connect_step1(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  اگر((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    بازگشت CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  اگر(!result)
    result = mbed_load_clicert(cf, data);
  اگر(!result)
    result = mbed_load_privkey(cf, data);
  اگر(!result)
    result = mbed_load_crl(cf, data);
  اگر(!result)
    result = mbed_configure_ssl(cf, data);
  اگر(result)
    بازگشت result;

  connssl->connecting_state = ssl_connect_2;
  بازگشت CURLE_OK;
}

#اگر defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

ایستا CURLcode mbed_connect_step2(ساختار Curl_cfilter *cf,
                                   ساختار Curl_easy *data)
{
#اگر defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  صحیح ret;
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  ثابت نویسه * ثابت pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#وگرنه
  ثابت نویسه * ثابت pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  اگر(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    بازگشت CURLE_OK;
  }
  وگرنه اگر(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    بازگشت CURLE_OK;
  }
  وگرنه اگر(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    بازگشت CURLE_PEER_FAILED_VERIFICATION;
  }
  وگرنه اگر(ret) {
    نویسه errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_عدد(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, اندازه(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned صحیح)-ret, errorbuf);
    بازگشت CURLE_SSL_CONNECT_ERROR;
  }

  {
    نویسه cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, اندازه(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  اگر(pinnedpubkey) {
    صحیح size;
    ثابت mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = پوچ;
    unsigned نویسه *pubkey = پوچ;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    اگر(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      بازگشت CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, اندازه(*p));

    اگر(!p)
      بازگشت CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    اگر(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      برو pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    اگر(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      برو pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    اگر(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      برو pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    اگر(result)
      بازگشت result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  اگر(connssl->alpn) {
    ثابت نویسه *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ثابت unsigned نویسه *)proto,
                                      proto ? strlen(proto) : 0);
    اگر(result)
      بازگشت result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  بازگشت CURLE_OK;
}

ایستا CURLcode mbed_new_session(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  ساختار Curl_ssl_session *sc_session = پوچ;
  unsigned نویسه *sdata = پوچ;
  size_t slen = 0;
  صحیح ietf_tls_id;
  CURLcode result = CURLE_OK;
  صحیح ret;

  DEBUGASSERT(backend);
  اگر(!Curl_ssl_scache_use(cf, data))
    بازگشت CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  اگر(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned صحیح)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    برو out;
  }

  mbedtls_ssl_session_save(&session, پوچ, 0, &slen);
  اگر(!slen) {
    failf(data, "failed to serialize session: length is 0");
    برو out;
  }

  sdata = curlx_malloc(slen);
  اگر(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  اگر(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned صحیح)-ret);
    برو out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_عدد(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = پوچ;  /* call took ownership */
  اگر(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  اگر(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_کلید,
                                 sc_session);
    sc_session = پوچ;
  }

out:
  اگر(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  بازگشت result;
}

ایستا CURLcode mbed_send(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                          ثابت تهی *mem, size_t len, size_t *pnwritten)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  صحیح nwritten;

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
  اگر(backend->send_blocked) {
    اگر(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      برو out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ثابت unsigned نویسه *)mem, len);

  اگر(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  وگرنه {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned صحیح)-nwritten);
    انتخاب(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    حالت MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      اگر(!result)
        برو do_send;
      بشکن;
#endif
    حالت MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      بشکن;
    حالت MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      بشکن;
    پیشفرض:
      result = CURLE_SEND_ERROR;
      بشکن;
    }
    اگر((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (صحیح)result, *pnwritten);
  بازگشت result;
}

ایستا CURLcode mbedtls_shutdown(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  unsigned نویسه buf[1024];
  CURLcode result = CURLE_OK;
  صحیح ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  اگر(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  اگر(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    اگر(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      انتخاب(ret) {
      حالت 0: /* we sent it, receive from the server */
        بشکن;
      حالت MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        برو out;
      حالت MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        برو out;
      حالت MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        برو out;
      پیشفرض:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned صحیح)-ret);
        result = CURLE_RECV_ERROR;
        برو out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  برای(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, اندازه(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    اگر(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, اندازه(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    اگر(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      ادامه;
#endif
    اگر(ret <= 0)
      بشکن;
  }

  اگر(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  وگرنه اگر(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  وگرنه اگر(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  وگرنه اگر(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  وگرنه {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned صحیح)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  بازگشت result;
}

ایستا تهی mbedtls_close(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;

  (تهی)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  اگر(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

ایستا CURLcode mbed_recv(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                          نویسه *buf, size_t buffersize, size_t *pnread)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  صحیح nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned نویسه *)buf, buffersize);
  اگر(nread > 0)
    *pnread = (size_t)nread;
  وگرنه {
    نویسه errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned صحیح)-nread);
    انتخاب(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    حالت MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      اگر(!result)
        برو do_read;
      بشکن;
#endif
    حالت MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      بشکن;
    حالت MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      بشکن;
    حالت MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      بشکن;
    پیشفرض:
      mbedtls_strerror(nread, errorbuf, اندازه(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned صحیح)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      بشکن;
    }
  }
  بازگشت result;
}

ایستا size_t mbedtls_version(نویسه *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned صحیح version = mbedtls_version_get_عدد();
  بازگشت curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#وگرنه
  بازگشت curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
ایستا CURLcode mbedtls_random(ساختار Curl_easy *data,
                               unsigned نویسه *entropy, size_t length)
{
  psa_status_t status;
  (تهی)data;

  status = psa_generate_random(entropy, length);

  بازگشت status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ایستا CURLcode mbedtls_connect(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  ساختار ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  اگر(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  اگر(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    اگر(result)
      بازگشت result;
  }

  اگر(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    اگر(result)
      بازگشت result;
  }

  اگر(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    ساختار ssl_connect_data *ctx = cf->ctx;
    ساختار mbed_ssl_backend_data *backend =
      (ساختار mbed_ssl_backend_data *)ctx->backend;

    اگر(mbedtls_ssl_get_version_عدد(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      اگر(result)
        بازگشت result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  اگر(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  بازگشت CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ایستا صحیح mbedtls_init(تهی)
{
#اگر MBEDTLS_VERSION_عدد < 0x04000000
  صحیح ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  اگر(status != PSA_SUCCESS)
    بازگشت 0;

#اگر MBEDTLS_VERSION_عدد < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              پوچ, 0);

  اگر(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    بازگشت 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  بازگشت 1;
}

ایستا تهی mbedtls_cleanup(تهی)
{
  mbedtls_psa_crypto_free();

#اگر MBEDTLS_VERSION_عدد < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

ایستا bool mbedtls_data_pending(ساختار Curl_cfilter *cf,
                                 ثابت ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *ctx = cf->ctx;
  ساختار mbed_ssl_backend_data *backend;

  (تهی)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ساختار mbed_ssl_backend_data *)ctx->backend;
  بازگشت mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

ایستا CURLcode mbedtls_sha256sum(ثابت unsigned نویسه *درون_داد,
                                  size_t inputlen,
                                  unsigned نویسه *sha256sum,
                                  size_t sha256len)
{
#اگر defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, درون_داد, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  اگر(status != PSA_SUCCESS)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  بازگشت CURLE_OK;
#وگرنه
  (تهی)sha256len;
  بازگشت Curl_sha256it(sha256sum, درون_داد, inputlen);
#endif
}

ایستا تهی *mbedtls_get_internals(ساختار ssl_connect_data *connssl,
                                   CURLINFO info)
{
  ساختار mbed_ssl_backend_data *backend =
    (ساختار mbed_ssl_backend_data *)connssl->backend;
  (تهی)info;
  DEBUGASSERT(backend);
  بازگشت &backend->ssl;
}

ثابت ساختار Curl_ssl Curl_ssl_mbedtls = {
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

  اندازه(ساختار mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  پوچ,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  پوچ,                             /* close_all */
  پوچ,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  پوچ,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
