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
#ضمن "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#ضمن <mbedtls/version.h>
#إذا MBEDTLS_VERSION_العدد < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#ضمن <psa/crypto_config.h>
#ضمن <mbedtls/net_sockets.h>
#ضمن <mbedtls/ssl.h>
#ضمن <mbedtls/x509.h>
#ضمن <mbedtls/psa_util.h>

#إذا MBEDTLS_VERSION_العدد < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#ضمن <mbedtls/error.h>
#إذا MBEDTLS_VERSION_العدد < 0x04000000
#ضمن <mbedtls/entropy.h>
#ضمن <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#ضمن <mbedtls/debug.h>
#endif

#ضمن "vtls/cipher_suite.h"
#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "vtls/mbedtls.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_int.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vtls/x509asn1.h"
#ضمن "connect.h" /* for the connect timeout */
#ضمن "curlx/strdup.h"
#ضمن "curl_sha256.h"

/* ALPN for http2 */
#إذا defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

بنية mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  ثابت حرف *protocols[3];
#endif
  صحيح *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#إذا MBEDTLS_VERSION_العدد < 0x04000000
بنية rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ساكن بنية rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
ساكن فراغ mbed_debug(فراغ *context, صحيح level, ثابت حرف *f_name,
                       صحيح سطر_nb, ثابت حرف *سطر)
{
  بنية Curl_easy *data = (بنية Curl_easy *)context;
  (فراغ)level;
  (فراغ)سطر_nb;
  (فراغ)f_name;

  إذا(data) {
    size_t len = strlen(سطر);
    إذا(len && (سطر[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (صحيح)len, سطر);
  }
}
#endif

ساكن صحيح mbedtls_bio_cf_write(فراغ *bio,
                                ثابت unsigned حرف *buf, size_t blen)
{
  بنية Curl_cfilter *cf = bio;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  إذا(!data)
    إرجاع 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (صحيح)result, nwritten);
  إذا(result == CURLE_AGAIN)
    إرجاع MBEDTLS_ERR_SSL_WANT_WRITE;
  إرجاع result ? -1 : (صحيح)nwritten;
}

ساكن صحيح mbedtls_bio_cf_read(فراغ *bio, unsigned حرف *buf, size_t blen)
{
  بنية Curl_cfilter *cf = bio;
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  إذا(!data)
    إرجاع 0;
  /* OpenSSL catches this case, so should we. */
  إذا(!buf)
    إرجاع 0;

  result = Curl_conn_cf_recv(cf->next, data, (حرف *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (صحيح)result, nread);
  إذا(result == CURLE_AGAIN)
    إرجاع MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  إرجاع result ? -1 : (صحيح)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

ساكن CURLcode mbed_set_ssl_version_min_max(
  بنية Curl_easy *data,
  بنية mbed_ssl_backend_data *backend,
  بنية ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#وإلا
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#وإلا
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  بدل(conn_config->version) {
  حالة CURL_SSLVERSION_TLSv1:
  حالة CURL_SSLVERSION_TLSv1_0:
  حالة CURL_SSLVERSION_TLSv1_1:
  حالة CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    اكسر;
#endif
  حالة CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    اكسر;
#endif
  افتراضي:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    إرجاع CURLE_SSL_CONNECT_ERROR;
  }

  بدل(conn_config->version_max) {
  حالة CURL_SSLVERSION_MAX_DEFAULT:
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    اكسر;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  حالة CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    اكسر;
#endif
  حالة CURL_SSLVERSION_MAX_TLSv1_1:
  حالة CURL_SSLVERSION_MAX_TLSv1_0:
  افتراضي:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    إرجاع CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  إرجاع CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ساكن صحيح mbed_cipher_suite_get_str(uint16_t id, حرف *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  إذا(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  وإلا
    إرجاع Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  إرجاع 0;
}

ساكن uint16_t mbed_cipher_suite_walk_str(ثابت حرف **str, ثابت حرف **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  ساكن ثابت حرف ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  إذا(!id) {
    إذا((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  إرجاع id;
}
#وإلا
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

ساكن CURLcode mbed_set_selected_ciphers(
  بنية Curl_easy *data,
  بنية mbed_ssl_backend_data *backend,
  ثابت حرف *ciphers12,
  ثابت حرف *ciphers13)
{
  ثابت حرف *ciphers = ciphers12;
  ثابت صحيح *supported;
  صحيح *selected;
  size_t supported_len, تعداد_2 = 0, default13_تعداد = 0, i, j;
  ثابت حرف *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  لكل(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(حجم(صحيح) * (supported_len + 1));
  إذا(!selected)
    إرجاع CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (فراغ)ciphers13, (فراغ)j;
#وإلا
  إذا(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    لكل(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      إذا(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        استمر;

      selected[تعداد_2++] = id;
    }

    default13_تعداد = تعداد_2;
  }
  وإلا
    ciphers = ciphers13;

add_ciphers:
#endif
  لكل(ptr = ciphers; ptr[0] != '\0' && تعداد_2 < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    إذا(id) {
      لكل(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      إذا(i == supported_len)
        id = 0;
    }
    إذا(!id) {
      إذا(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (صحيح)(end - ptr), ptr);
      استمر;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    لكل(i = 0; i < تعداد_2 && selected[i] != id; i++)
      ;
    إذا(i < تعداد_2) {
      إذا(i >= default13_تعداد)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (صحيح)(end - ptr), ptr);
      استمر;
    }

    selected[تعداد_2++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  إذا(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    اذهب add_ciphers;
  }

  إذا(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    لكل(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      إذا(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        استمر;

      /* No duplicates allowed (so selected cannot overflow) */
      لكل(i = 0; i < تعداد_2 && selected[i] != id; i++)
        ;
      إذا(i < تعداد_2)
        استمر;

      selected[تعداد_2++] = id;
    }
  }
#endif

  selected[تعداد_2] = 0;

  إذا(تعداد_2 == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    إرجاع CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  إرجاع CURLE_OK;
}

ساكن فراغ mbed_dump_cert_info(بنية Curl_easy *data,
                                ثابت mbedtls_x509_crt *crt)
{
#إذا !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (فراغ)data, (فراغ)crt;
#وإلا
  ثابت size_t bufsize = 16384;
  حرف *p, *buffer = curlx_malloc(bufsize);

  إذا(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    لكل(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (صحيح)s, p);
      p += s;
    }
  }
  وإلا
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

ساكن فراغ mbed_extract_certinfo(بنية Curl_easy *data,
                                  ثابت mbedtls_x509_crt *crt)
{
  CURLcode result;
  ثابت mbedtls_x509_crt *cur;
  صحيح cert_تعداد = 0;
  صحيح i;

  لكل(cur = crt; cur && cert_تعداد <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_تعداد++;

  إذا(cert_تعداد > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    إرجاع;
  }

  result = Curl_ssl_init_certinfo(data, cert_تعداد);

  لكل(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ساكن صحيح mbed_verify_cb(فراغ *ptr, mbedtls_x509_crt *crt,
                          صحيح depth, uint32_t *flags)
{
  بنية Curl_cfilter *cf = (بنية Curl_cfilter *)ptr;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_easy *data = CF_DATA_CURRENT(cf);

  إذا(depth == 0) {
    إذا(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    إذا(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  إذا(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  إذا(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  إذا(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    حرف buf[128];
    mbedtls_x509_crt_verify_info(buf, حجم(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#وإلا
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned صحيح)*flags);
#endif
  }

  إرجاع 0;
}

ساكن CURLcode mbed_load_cacert(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? عدم : conn_config->CAfile);
  ثابت bool verifypeer = conn_config->verifypeer;
  ثابت حرف * ثابت ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#endif
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  إذا(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    إذا((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((حرف *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    وإلا { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned حرف *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      إذا(!newblob)
        إرجاع CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#وإلا
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned صحيح)-ret, errorbuf);
      إرجاع CURLE_SSL_CERTPROBLEM;
    }
  }

  إذا(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned صحيح)-ret, errorbuf);
      إرجاع CURLE_SSL_CACERT_BADFILE;
    }
#وإلا
    failf(data, "mbedTLS: functions that use the file system not built in");
    إرجاع CURLE_NOT_BUILT_IN;
#endif
  }

  إذا(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned صحيح)-ret, errorbuf);

      إذا(verifypeer)
        إرجاع CURLE_SSL_CACERT_BADFILE;
    }
#وإلا
    failf(data, "mbedTLS: functions that use the file system not built in");
    إرجاع CURLE_NOT_BUILT_IN;
#endif
  }

  إرجاع CURLE_OK;
}

ساكن CURLcode mbed_load_clicert(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  حرف * ثابت ssl_cert = conn_config->clientcert;
  ثابت بنية curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#endif
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  إذا(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned صحيح)-ret, errorbuf);

      إرجاع CURLE_SSL_CERTPROBLEM;
    }
#وإلا
    failf(data, "mbedTLS: functions that use the file system not built in");
    إرجاع CURLE_NOT_BUILT_IN;
#endif
  }

  إذا(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    إذا((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((حرف *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    وإلا { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned حرف *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      إذا(!newblob)
        إرجاع CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#وإلا
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned صحيح)-ret, errorbuf);
      إرجاع CURLE_SSL_CERTPROBLEM;
    }
  }

  إرجاع CURLE_OK;
}

ساكن CURLcode mbed_load_privkey(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  إذا(conn_config->مفتاح || conn_config->مفتاح_blob) {
    إذا(conn_config->مفتاح) {
#ifdef MBEDTLS_FS_IO
#إذا MBEDTLS_VERSION_العدد >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->مفتاح,
                                     conn_config->مفتاح_passwd);
      إذا(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_مفتاح_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_مفتاح_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وإلا
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->مفتاح,
                                     conn_config->مفتاح_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      إذا(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      إذا(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->مفتاح, (unsigned صحيح)-ret, errorbuf);
        إرجاع CURLE_SSL_CERTPROBLEM;
      }
#وإلا
      failf(data, "mbedTLS: functions that use the file system not built in");
      إرجاع CURLE_NOT_BUILT_IN;
#endif
    }
    وإلا {
      ثابت بنية curl_blob *ssl_مفتاح_blob = conn_config->مفتاح_blob;
      ثابت حرف *passwd = conn_config->مفتاح_passwd;
      ثابت bool is_der = conn_config->مفتاح_type &&
                          curl_strequal(conn_config->مفتاح_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned حرف *newblob = curlx_memdup0(ssl_مفتاح_blob->data,
                                             ssl_مفتاح_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_مفتاح_blob->len : ssl_مفتاح_blob->len + 1;
      إذا(!newblob)
        إرجاع CURLE_OUT_OF_MEMORY;

#إذا MBEDTLS_VERSION_العدد >= 0x04000000
      ret = mbedtls_pk_parse_مفتاح(&backend->pk, newblob, keylen,
                                 (ثابت unsigned حرف *)passwd,
                                 passwd ? strlen(passwd) : 0);
      إذا(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_مفتاح_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_مفتاح_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وإلا
      ret = mbedtls_pk_parse_مفتاح(&backend->pk, newblob, keylen,
                                 (ثابت unsigned حرف *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      إذا(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      إذا(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned صحيح)-ret, errorbuf);
        إرجاع CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  إرجاع CURLE_OK;
}

ساكن CURLcode mbed_load_crl(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت حرف * ثابت ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  إذا(ssl_crlfile) {
    حرف errorbuf[128];
#ifdef MBEDTLS_FS_IO
    صحيح ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned صحيح)-ret, errorbuf);

      إرجاع CURLE_SSL_CRL_BADFILE;
    }
#وإلا
    (فراغ)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    إرجاع CURLE_NOT_BUILT_IN;
#endif
  }
#وإلا
  (فراغ)backend;
  إذا(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    إرجاع CURLE_NOT_BUILT_IN;
  }
#endif

  إرجاع CURLE_OK;
}

ساكن bool mbed_apply_session(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية Curl_ssl_session *sc_session)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;

  إذا(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    صحيح ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    إذا(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned صحيح)-ret);
    }
    وإلا {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      إذا(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned صحيح)-ret);
    }
    mbedtls_ssl_session_free(&session);
    إذا(!ret) {
      infof(data, "SSL reusing session ID");
      إرجاع TRUE;
    }
  }
  إرجاع FALSE;
}

ساكن CURLcode mbed_configure_ssl(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح ret;
  bool session_applied = FALSE;
  CURLcode result;
  حرف errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  إذا(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    إرجاع CURLE_SSL_CONNECT_ERROR;
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

#إذا defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_العدد >= 0x03060100 &&   \
  MBEDTLS_VERSION_العدد < 0x04000000
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
  إذا(result)
    إرجاع result;

#إذا MBEDTLS_VERSION_العدد < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  إذا(ret) {
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned صحيح)-ret, errorbuf);
    إرجاع CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      عدم /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  إذا(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, عدم);
#وإلا
  إذا(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    إذا(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      إرجاع result;
    }
  }
  وإلا {
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

  إذا((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    بنية Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    إذا(scs) {
      إذا(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  إذا(!session_applied && Curl_ssl_scache_use(cf, data)) {
    بنية Curl_ssl_session *sc_session = عدم;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_مفتاح,
                                            &sc_session);
    إذا(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      إذا(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned صحيح)-ret);
      }
      وإلا {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        إذا(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned صحيح)-ret);
        وإلا
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_مفتاح,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#وإلا
                            عدم
#endif
    );

  إذا(conn_config->مفتاح || conn_config->مفتاح_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  إذا(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    إرجاع CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  إذا(connssl->alpn) {
    بنية alpn_proto_buf proto;
    size_t i;

    لكل(i = 0; i < connssl->alpn->تعداد_2; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    إذا(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      إرجاع CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  إذا(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    إذا(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  إرجاع result;
}

ساكن CURLcode mbed_connect_step1(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  إذا((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    إرجاع CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  إذا(!result)
    result = mbed_load_clicert(cf, data);
  إذا(!result)
    result = mbed_load_privkey(cf, data);
  إذا(!result)
    result = mbed_load_crl(cf, data);
  إذا(!result)
    result = mbed_configure_ssl(cf, data);
  إذا(result)
    إرجاع result;

  connssl->connecting_state = ssl_connect_2;
  إرجاع CURLE_OK;
}

#إذا defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

ساكن CURLcode mbed_connect_step2(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *data)
{
#إذا defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  صحيح ret;
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  ثابت حرف * ثابت pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#وإلا
  ثابت حرف * ثابت pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  إذا(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    إرجاع CURLE_OK;
  }
  وإلا إذا(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    إرجاع CURLE_OK;
  }
  وإلا إذا(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    إرجاع CURLE_PEER_FAILED_VERIFICATION;
  }
  وإلا إذا(ret) {
    حرف errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_العدد(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned صحيح)-ret, errorbuf);
    إرجاع CURLE_SSL_CONNECT_ERROR;
  }

  {
    حرف cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, حجم(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  إذا(pinnedpubkey) {
    صحيح size;
    ثابت mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = عدم;
    unsigned حرف *pubkey = عدم;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    إذا(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      إرجاع CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, حجم(*p));

    إذا(!p)
      إرجاع CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    إذا(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      اذهب pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    إذا(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      اذهب pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    إذا(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      اذهب pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    إذا(result)
      إرجاع result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  إذا(connssl->alpn) {
    ثابت حرف *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ثابت unsigned حرف *)proto,
                                      proto ? strlen(proto) : 0);
    إذا(result)
      إرجاع result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  إرجاع CURLE_OK;
}

ساكن CURLcode mbed_new_session(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  بنية Curl_ssl_session *sc_session = عدم;
  unsigned حرف *sdata = عدم;
  size_t slen = 0;
  صحيح ietf_tls_id;
  CURLcode result = CURLE_OK;
  صحيح ret;

  DEBUGASSERT(backend);
  إذا(!Curl_ssl_scache_use(cf, data))
    إرجاع CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  إذا(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned صحيح)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    اذهب out;
  }

  mbedtls_ssl_session_save(&session, عدم, 0, &slen);
  إذا(!slen) {
    failf(data, "failed to serialize session: length is 0");
    اذهب out;
  }

  sdata = curlx_malloc(slen);
  إذا(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  إذا(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned صحيح)-ret);
    اذهب out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_العدد(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = عدم;  /* call took ownership */
  إذا(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  إذا(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_مفتاح,
                                 sc_session);
    sc_session = عدم;
  }

out:
  إذا(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  إرجاع result;
}

ساكن CURLcode mbed_send(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                          ثابت فراغ *mem, size_t len, size_t *pnwritten)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  صحيح nwritten;

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
  إذا(backend->send_blocked) {
    إذا(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      اذهب out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ثابت unsigned حرف *)mem, len);

  إذا(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  وإلا {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned صحيح)-nwritten);
    بدل(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    حالة MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      إذا(!result)
        اذهب do_send;
      اكسر;
#endif
    حالة MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      اكسر;
    افتراضي:
      result = CURLE_SEND_ERROR;
      اكسر;
    }
    إذا((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (صحيح)result, *pnwritten);
  إرجاع result;
}

ساكن CURLcode mbedtls_shutdown(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  unsigned حرف buf[1024];
  CURLcode result = CURLE_OK;
  صحيح ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  إذا(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    إرجاع CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  إذا(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    إذا(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      بدل(ret) {
      حالة 0: /* we sent it, receive from the server */
        اكسر;
      حالة MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        اذهب out;
      حالة MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        اذهب out;
      حالة MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        اذهب out;
      افتراضي:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned صحيح)-ret);
        result = CURLE_RECV_ERROR;
        اذهب out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  لكل(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, حجم(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    إذا(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, حجم(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    إذا(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      استمر;
#endif
    إذا(ret <= 0)
      اكسر;
  }

  إذا(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  وإلا إذا(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  وإلا إذا(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  وإلا إذا(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  وإلا {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned صحيح)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  إرجاع result;
}

ساكن فراغ mbedtls_close(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;

  (فراغ)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  إذا(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

ساكن CURLcode mbed_recv(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                          حرف *buf, size_t buffersize, size_t *pnread)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  صحيح nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned حرف *)buf, buffersize);
  إذا(nread > 0)
    *pnread = (size_t)nread;
  وإلا {
    حرف errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned صحيح)-nread);
    بدل(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    حالة MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      إذا(!result)
        اذهب do_read;
      اكسر;
#endif
    حالة MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      اكسر;
    افتراضي:
      mbedtls_strerror(nread, errorbuf, حجم(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned صحيح)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      اكسر;
    }
  }
  إرجاع result;
}

ساكن size_t mbedtls_version(حرف *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned صحيح version = mbedtls_version_get_العدد();
  إرجاع curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#وإلا
  إرجاع curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
ساكن CURLcode mbedtls_random(بنية Curl_easy *data,
                               unsigned حرف *entropy, size_t length)
{
  psa_status_t status;
  (فراغ)data;

  status = psa_generate_random(entropy, length);

  إرجاع status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ساكن CURLcode mbedtls_connect(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  بنية ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    إرجاع CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  إذا(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    إذا(result)
      إرجاع result;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    إذا(result)
      إرجاع result;
  }

  إذا(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    بنية ssl_connect_data *ctx = cf->ctx;
    بنية mbed_ssl_backend_data *backend =
      (بنية mbed_ssl_backend_data *)ctx->backend;

    إذا(mbedtls_ssl_get_version_العدد(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      إذا(result)
        إرجاع result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  إذا(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  إرجاع CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ساكن صحيح mbedtls_init(فراغ)
{
#إذا MBEDTLS_VERSION_العدد < 0x04000000
  صحيح ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  إذا(status != PSA_SUCCESS)
    إرجاع 0;

#إذا MBEDTLS_VERSION_العدد < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              عدم, 0);

  إذا(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    إرجاع 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  إرجاع 1;
}

ساكن فراغ mbedtls_cleanup(فراغ)
{
  mbedtls_psa_crypto_free();

#إذا MBEDTLS_VERSION_العدد < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

ساكن bool mbedtls_data_pending(بنية Curl_cfilter *cf,
                                 ثابت بنية Curl_easy *data)
{
  بنية ssl_connect_data *ctx = cf->ctx;
  بنية mbed_ssl_backend_data *backend;

  (فراغ)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية mbed_ssl_backend_data *)ctx->backend;
  إرجاع mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

ساكن CURLcode mbedtls_sha256sum(ثابت unsigned حرف *إدخال,
                                  size_t inputlen,
                                  unsigned حرف *sha256sum,
                                  size_t sha256len)
{
#إذا defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, إدخال, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  إذا(status != PSA_SUCCESS)
    إرجاع CURLE_BAD_FUNCTION_ARGUMENT;
  إرجاع CURLE_OK;
#وإلا
  (فراغ)sha256len;
  إرجاع Curl_sha256it(sha256sum, إدخال, inputlen);
#endif
}

ساكن فراغ *mbedtls_get_internals(بنية ssl_connect_data *connssl,
                                   CURLINFO info)
{
  بنية mbed_ssl_backend_data *backend =
    (بنية mbed_ssl_backend_data *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  إرجاع &backend->ssl;
}

ثابت بنية Curl_ssl Curl_ssl_mbedtls = {
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

  حجم(بنية mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  عدم,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  عدم,                             /* close_all */
  عدم,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  عدم,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
