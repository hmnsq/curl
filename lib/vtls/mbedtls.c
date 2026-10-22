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
#अन्तर्भाव "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#अन्तर्भाव <mbedtls/version.h>
#जइ MBEDTLS_VERSION_सङ्ख्या < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#अन्तर्भाव <psa/crypto_config.h>
#अन्तर्भाव <mbedtls/net_sockets.h>
#अन्तर्भाव <mbedtls/ssl.h>
#अन्तर्भाव <mbedtls/x509.h>
#अन्तर्भाव <mbedtls/psa_util.h>

#जइ MBEDTLS_VERSION_सङ्ख्या < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#अन्तर्भाव <mbedtls/error.h>
#जइ MBEDTLS_VERSION_सङ्ख्या < 0x04000000
#अन्तर्भाव <mbedtls/entropy.h>
#अन्तर्भाव <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#अन्तर्भाव <mbedtls/debug.h>
#endif

#अन्तर्भाव "vtls/cipher_suite.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vtls/mbedtls.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_int.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vtls/x509asn1.h"
#अन्तर्भाव "connect.h" /* for the connect timeout */
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curl_sha256.h"

/* ALPN for http2 */
#जइ defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

संरचना mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  ध्रुव वर्ण *protocols[3];
#endif
  पूर्णाङ्क *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#जइ MBEDTLS_VERSION_सङ्ख्या < 0x04000000
संरचना rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

स्थिर संरचना rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
स्थिर शून्य mbed_debug(शून्य *context, पूर्णाङ्क level, ध्रुव वर्ण *f_name,
                       पूर्णाङ्क पङ्क्तिः_nb, ध्रुव वर्ण *पङ्क्तिः)
{
  संरचना Curl_easy *data = (संरचना Curl_easy *)context;
  (शून्य)level;
  (शून्य)पङ्क्तिः_nb;
  (शून्य)f_name;

  जइ(data) {
    size_t len = strlen(पङ्क्तिः);
    जइ(len && (पङ्क्तिः[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (पूर्णाङ्क)len, पङ्क्तिः);
  }
}
#endif

स्थिर पूर्णाङ्क mbedtls_bio_cf_write(शून्य *bio,
                                ध्रुव unsigned वर्ण *buf, size_t blen)
{
  संरचना Curl_cfilter *cf = bio;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  जइ(!data)
    प्रत्यागम 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (पूर्णाङ्क)result, nwritten);
  जइ(result == CURLE_AGAIN)
    प्रत्यागम MBEDTLS_ERR_SSL_WANT_WRITE;
  प्रत्यागम result ? -1 : (पूर्णाङ्क)nwritten;
}

स्थिर पूर्णाङ्क mbedtls_bio_cf_read(शून्य *bio, unsigned वर्ण *buf, size_t blen)
{
  संरचना Curl_cfilter *cf = bio;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  जइ(!data)
    प्रत्यागम 0;
  /* OpenSSL catches this case, so should we. */
  जइ(!buf)
    प्रत्यागम 0;

  result = Curl_conn_cf_recv(cf->next, data, (वर्ण *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (पूर्णाङ्क)result, nread);
  जइ(result == CURLE_AGAIN)
    प्रत्यागम MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  प्रत्यागम result ? -1 : (पूर्णाङ्क)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

स्थिर CURLcode mbed_set_ssl_version_min_max(
  संरचना Curl_easy *data,
  संरचना mbed_ssl_backend_data *backend,
  संरचना ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#अण्णहा
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#अण्णहा
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  विकल्प(conn_config->version) {
  पक्ष CURL_SSLVERSION_TLSv1:
  पक्ष CURL_SSLVERSION_TLSv1_0:
  पक्ष CURL_SSLVERSION_TLSv1_1:
  पक्ष CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    भङ्ग;
#endif
  पक्ष CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    भङ्ग;
#endif
  स्वभाव:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  विकल्प(conn_config->version_max) {
  पक्ष CURL_SSLVERSION_MAX_DEFAULT:
  पक्ष CURL_SSLVERSION_MAX_NONE:
  पक्ष CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    भङ्ग;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  पक्ष CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    भङ्ग;
#endif
  पक्ष CURL_SSLVERSION_MAX_TLSv1_1:
  पक्ष CURL_SSLVERSION_MAX_TLSv1_0:
  स्वभाव:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  प्रत्यागम CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
स्थिर पूर्णाङ्क mbed_cipher_suite_get_str(uint16_t id, वर्ण *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  जइ(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  अण्णहा
    प्रत्यागम Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  प्रत्यागम 0;
}

स्थिर uint16_t mbed_cipher_suite_walk_str(ध्रुव वर्ण **str, ध्रुव वर्ण **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  स्थिर ध्रुव वर्ण ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  जइ(!id) {
    जइ((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  प्रत्यागम id;
}
#अण्णहा
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

स्थिर CURLcode mbed_set_selected_ciphers(
  संरचना Curl_easy *data,
  संरचना mbed_ssl_backend_data *backend,
  ध्रुव वर्ण *ciphers12,
  ध्रुव वर्ण *ciphers13)
{
  ध्रुव वर्ण *ciphers = ciphers12;
  ध्रुव पूर्णाङ्क *supported;
  पूर्णाङ्क *selected;
  size_t supported_len, गणनम् = 0, default13_गणनम् = 0, i, j;
  ध्रुव वर्ण *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  प्रति(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(परिमाण(पूर्णाङ्क) * (supported_len + 1));
  जइ(!selected)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (शून्य)ciphers13, (शून्य)j;
#अण्णहा
  जइ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    प्रति(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      जइ(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        अनुवर्त;

      selected[गणनम्++] = id;
    }

    default13_गणनम् = गणनम्;
  }
  अण्णहा
    ciphers = ciphers13;

add_ciphers:
#endif
  प्रति(ptr = ciphers; ptr[0] != '\0' && गणनम् < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    जइ(id) {
      प्रति(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      जइ(i == supported_len)
        id = 0;
    }
    जइ(!id) {
      जइ(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (पूर्णाङ्क)(end - ptr), ptr);
      अनुवर्त;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    प्रति(i = 0; i < गणनम् && selected[i] != id; i++)
      ;
    जइ(i < गणनम्) {
      जइ(i >= default13_गणनम्)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (पूर्णाङ्क)(end - ptr), ptr);
      अनुवर्त;
    }

    selected[गणनम्++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  जइ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    गच्छ add_ciphers;
  }

  जइ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    प्रति(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      जइ(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        अनुवर्त;

      /* No duplicates allowed (so selected cannot overflow) */
      प्रति(i = 0; i < गणनम् && selected[i] != id; i++)
        ;
      जइ(i < गणनम्)
        अनुवर्त;

      selected[गणनम्++] = id;
    }
  }
#endif

  selected[गणनम्] = 0;

  जइ(गणनम् == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    प्रत्यागम CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य mbed_dump_cert_info(संरचना Curl_easy *data,
                                ध्रुव mbedtls_x509_crt *crt)
{
#जइ !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (शून्य)data, (शून्य)crt;
#अण्णहा
  ध्रुव size_t bufsize = 16384;
  वर्ण *p, *buffer = curlx_malloc(bufsize);

  जइ(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    प्रति(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (पूर्णाङ्क)s, p);
      p += s;
    }
  }
  अण्णहा
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

स्थिर शून्य mbed_extract_certinfo(संरचना Curl_easy *data,
                                  ध्रुव mbedtls_x509_crt *crt)
{
  CURLcode result;
  ध्रुव mbedtls_x509_crt *cur;
  पूर्णाङ्क cert_गणनम् = 0;
  पूर्णाङ्क i;

  प्रति(cur = crt; cur && cert_गणनम् <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_गणनम्++;

  जइ(cert_गणनम् > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    प्रत्यागम;
  }

  result = Curl_ssl_init_certinfo(data, cert_गणनम्);

  प्रति(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

स्थिर पूर्णाङ्क mbed_verify_cb(शून्य *ptr, mbedtls_x509_crt *crt,
                          पूर्णाङ्क depth, uint32_t *flags)
{
  संरचना Curl_cfilter *cf = (संरचना Curl_cfilter *)ptr;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);

  जइ(depth == 0) {
    जइ(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    जइ(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  जइ(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  जइ(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  जइ(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    वर्ण buf[128];
    mbedtls_x509_crt_verify_info(buf, परिमाण(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#अण्णहा
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned पूर्णाङ्क)*flags);
#endif
  }

  प्रत्यागम 0;
}

स्थिर CURLcode mbed_load_cacert(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ध्रुव संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ध्रुव वर्ण * ध्रुव ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? रिक्त : conn_config->CAfile);
  ध्रुव bool verifypeer = conn_config->verifypeer;
  ध्रुव वर्ण * ध्रुव ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  ध्रुव वर्ण * ध्रुव ssl_cert_type = conn_config->cert_type;
#endif
  पूर्णाङ्क ret = -1;
  वर्ण errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  जइ(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    जइ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((वर्ण *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    अण्णहा { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned वर्ण *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      जइ(!newblob)
        प्रत्यागम CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#अण्णहा
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    जइ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned पूर्णाङ्क)-ret, errorbuf);
      प्रत्यागम CURLE_SSL_CERTPROBLEM;
    }
  }

  जइ(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    जइ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned पूर्णाङ्क)-ret, errorbuf);
      प्रत्यागम CURLE_SSL_CACERT_BADFILE;
    }
#अण्णहा
    failf(data, "mbedTLS: functions that use the file system not built in");
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
  }

  जइ(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    जइ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned पूर्णाङ्क)-ret, errorbuf);

      जइ(verifypeer)
        प्रत्यागम CURLE_SSL_CACERT_BADFILE;
    }
#अण्णहा
    failf(data, "mbedTLS: functions that use the file system not built in");
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mbed_load_clicert(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  वर्ण * ध्रुव ssl_cert = conn_config->clientcert;
  ध्रुव संरचना curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  ध्रुव वर्ण * ध्रुव ssl_cert_type = conn_config->cert_type;
#endif
  पूर्णाङ्क ret = -1;
  वर्ण errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  जइ(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    जइ(ret) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned पूर्णाङ्क)-ret, errorbuf);

      प्रत्यागम CURLE_SSL_CERTPROBLEM;
    }
#अण्णहा
    failf(data, "mbedTLS: functions that use the file system not built in");
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
  }

  जइ(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    जइ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((वर्ण *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    अण्णहा { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned वर्ण *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      जइ(!newblob)
        प्रत्यागम CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#अण्णहा
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    जइ(ret) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned पूर्णाङ्क)-ret, errorbuf);
      प्रत्यागम CURLE_SSL_CERTPROBLEM;
    }
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mbed_load_privkey(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णाङ्क ret = -1;
  वर्ण errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  जइ(conn_config->कुञ्चिका || conn_config->कुञ्चिका_blob) {
    जइ(conn_config->कुञ्चिका) {
#ifdef MBEDTLS_FS_IO
#जइ MBEDTLS_VERSION_सङ्ख्या >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->कुञ्चिका,
                                     conn_config->कुञ्चिका_passwd);
      जइ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_कुञ्चिका_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_कुञ्चिका_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#अण्णहा
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->कुञ्चिका,
                                     conn_config->कुञ्चिका_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      जइ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      जइ(ret) {
        mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->कुञ्चिका, (unsigned पूर्णाङ्क)-ret, errorbuf);
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
#अण्णहा
      failf(data, "mbedTLS: functions that use the file system not built in");
      प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
    }
    अण्णहा {
      ध्रुव संरचना curl_blob *ssl_कुञ्चिका_blob = conn_config->कुञ्चिका_blob;
      ध्रुव वर्ण *passwd = conn_config->कुञ्चिका_passwd;
      ध्रुव bool is_der = conn_config->कुञ्चिका_type &&
                          curl_strequal(conn_config->कुञ्चिका_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned वर्ण *newblob = curlx_memdup0(ssl_कुञ्चिका_blob->data,
                                             ssl_कुञ्चिका_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_कुञ्चिका_blob->len : ssl_कुञ्चिका_blob->len + 1;
      जइ(!newblob)
        प्रत्यागम CURLE_OUT_OF_MEMORY;

#जइ MBEDTLS_VERSION_सङ्ख्या >= 0x04000000
      ret = mbedtls_pk_parse_कुञ्चिका(&backend->pk, newblob, keylen,
                                 (ध्रुव unsigned वर्ण *)passwd,
                                 passwd ? strlen(passwd) : 0);
      जइ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_कुञ्चिका_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_कुञ्चिका_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#अण्णहा
      ret = mbedtls_pk_parse_कुञ्चिका(&backend->pk, newblob, keylen,
                                 (ध्रुव unsigned वर्ण *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      जइ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      जइ(ret) {
        mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned पूर्णाङ्क)-ret, errorbuf);
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mbed_load_crl(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ध्रुव वर्ण * ध्रुव ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  जइ(ssl_crlfile) {
    वर्ण errorbuf[128];
#ifdef MBEDTLS_FS_IO
    पूर्णाङ्क ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    जइ(ret) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned पूर्णाङ्क)-ret, errorbuf);

      प्रत्यागम CURLE_SSL_CRL_BADFILE;
    }
#अण्णहा
    (शून्य)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    प्रत्यागम CURLE_NOT_BUILT_IN;
#endif
  }
#अण्णहा
  (शून्य)backend;
  जइ(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    प्रत्यागम CURLE_NOT_BUILT_IN;
  }
#endif

  प्रत्यागम CURLE_OK;
}

स्थिर bool mbed_apply_session(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना Curl_ssl_session *sc_session)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;

  जइ(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    पूर्णाङ्क ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    जइ(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned पूर्णाङ्क)-ret);
    }
    अण्णहा {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      जइ(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned पूर्णाङ्क)-ret);
    }
    mbedtls_ssl_session_free(&session);
    जइ(!ret) {
      infof(data, "SSL reusing session ID");
      प्रत्यागम TRUE;
    }
  }
  प्रत्यागम FALSE;
}

स्थिर CURLcode mbed_configure_ssl(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णाङ्क ret;
  bool session_applied = FALSE;
  CURLcode result;
  वर्ण errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  जइ(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
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

#जइ defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_सङ्ख्या >= 0x03060100 &&   \
  MBEDTLS_VERSION_सङ्ख्या < 0x04000000
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
  जइ(result)
    प्रत्यागम result;

#जइ MBEDTLS_VERSION_सङ्ख्या < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  जइ(ret) {
    mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned पूर्णाङ्क)-ret, errorbuf);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      रिक्त /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  जइ(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, रिक्त);
#अण्णहा
  जइ(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    जइ(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      प्रत्यागम result;
    }
  }
  अण्णहा {
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

  जइ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    संरचना Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    जइ(scs) {
      जइ(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  जइ(!session_applied && Curl_ssl_scache_use(cf, data)) {
    संरचना Curl_ssl_session *sc_session = रिक्त;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_कुञ्चिका,
                                            &sc_session);
    जइ(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      जइ(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned पूर्णाङ्क)-ret);
      }
      अण्णहा {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        जइ(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned पूर्णाङ्क)-ret);
        अण्णहा
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_कुञ्चिका,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#अण्णहा
                            रिक्त
#endif
    );

  जइ(conn_config->कुञ्चिका || conn_config->कुञ्चिका_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  जइ(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  जइ(connssl->alpn) {
    संरचना alpn_proto_buf proto;
    size_t i;

    प्रति(i = 0; i < connssl->alpn->गणनम्; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    जइ(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  जइ(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    जइ(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  प्रत्यागम result;
}

स्थिर CURLcode mbed_connect_step1(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  जइ((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    प्रत्यागम CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  जइ(!result)
    result = mbed_load_clicert(cf, data);
  जइ(!result)
    result = mbed_load_privkey(cf, data);
  जइ(!result)
    result = mbed_load_crl(cf, data);
  जइ(!result)
    result = mbed_configure_ssl(cf, data);
  जइ(result)
    प्रत्यागम result;

  connssl->connecting_state = ssl_connect_2;
  प्रत्यागम CURLE_OK;
}

#जइ defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

स्थिर CURLcode mbed_connect_step2(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
#जइ defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  पूर्णाङ्क ret;
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  ध्रुव वर्ण * ध्रुव pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#अण्णहा
  ध्रुव वर्ण * ध्रुव pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  जइ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    प्रत्यागम CURLE_OK;
  }
  अण्णहा जइ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    प्रत्यागम CURLE_OK;
  }
  अण्णहा जइ(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
  }
  अण्णहा जइ(ret) {
    वर्ण errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_सङ्ख्या(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned पूर्णाङ्क)-ret, errorbuf);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  {
    वर्ण cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, परिमाण(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  जइ(pinnedpubkey) {
    पूर्णाङ्क size;
    ध्रुव mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = रिक्त;
    unsigned वर्ण *pubkey = रिक्त;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    जइ(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      प्रत्यागम CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, परिमाण(*p));

    जइ(!p)
      प्रत्यागम CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    जइ(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    जइ(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      गच्छ pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    जइ(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      गच्छ pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    जइ(result)
      प्रत्यागम result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  जइ(connssl->alpn) {
    ध्रुव वर्ण *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ध्रुव unsigned वर्ण *)proto,
                                      proto ? strlen(proto) : 0);
    जइ(result)
      प्रत्यागम result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mbed_new_session(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  संरचना Curl_ssl_session *sc_session = रिक्त;
  unsigned वर्ण *sdata = रिक्त;
  size_t slen = 0;
  पूर्णाङ्क ietf_tls_id;
  CURLcode result = CURLE_OK;
  पूर्णाङ्क ret;

  DEBUGASSERT(backend);
  जइ(!Curl_ssl_scache_use(cf, data))
    प्रत्यागम CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  जइ(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned पूर्णाङ्क)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    गच्छ out;
  }

  mbedtls_ssl_session_save(&session, रिक्त, 0, &slen);
  जइ(!slen) {
    failf(data, "failed to serialize session: length is 0");
    गच्छ out;
  }

  sdata = curlx_malloc(slen);
  जइ(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  जइ(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned पूर्णाङ्क)-ret);
    गच्छ out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_सङ्ख्या(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = रिक्त;  /* call took ownership */
  जइ(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  जइ(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_कुञ्चिका,
                                 sc_session);
    sc_session = रिक्त;
  }

out:
  जइ(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  प्रत्यागम result;
}

स्थिर CURLcode mbed_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          ध्रुव शून्य *mem, size_t len, size_t *pnwritten)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  पूर्णाङ्क nwritten;

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
  जइ(backend->send_blocked) {
    जइ(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      गच्छ out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ध्रुव unsigned वर्ण *)mem, len);

  जइ(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  अण्णहा {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned पूर्णाङ्क)-nwritten);
    विकल्प(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    पक्ष MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      जइ(!result)
        गच्छ do_send;
      भङ्ग;
#endif
    पक्ष MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      भङ्ग;
    पक्ष MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      भङ्ग;
    स्वभाव:
      result = CURLE_SEND_ERROR;
      भङ्ग;
    }
    जइ((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (पूर्णाङ्क)result, *pnwritten);
  प्रत्यागम result;
}

स्थिर CURLcode mbedtls_shutdown(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  unsigned वर्ण buf[1024];
  CURLcode result = CURLE_OK;
  पूर्णाङ्क ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  जइ(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  जइ(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    जइ(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      विकल्प(ret) {
      पक्ष 0: /* we sent it, receive from the server */
        भङ्ग;
      पक्ष MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        गच्छ out;
      पक्ष MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        गच्छ out;
      पक्ष MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        गच्छ out;
      स्वभाव:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned पूर्णाङ्क)-ret);
        result = CURLE_RECV_ERROR;
        गच्छ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  प्रति(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, परिमाण(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    जइ(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, परिमाण(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    जइ(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      अनुवर्त;
#endif
    जइ(ret <= 0)
      भङ्ग;
  }

  जइ(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  अण्णहा जइ(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  अण्णहा जइ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  अण्णहा जइ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  अण्णहा {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned पूर्णाङ्क)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  प्रत्यागम result;
}

स्थिर शून्य mbedtls_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;

  (शून्य)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  जइ(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

स्थिर CURLcode mbed_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          वर्ण *buf, size_t buffersize, size_t *pnread)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  पूर्णाङ्क nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned वर्ण *)buf, buffersize);
  जइ(nread > 0)
    *pnread = (size_t)nread;
  अण्णहा {
    वर्ण errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned पूर्णाङ्क)-nread);
    विकल्प(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    पक्ष MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      जइ(!result)
        गच्छ do_read;
      भङ्ग;
#endif
    पक्ष MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      भङ्ग;
    पक्ष MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      भङ्ग;
    पक्ष MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      भङ्ग;
    स्वभाव:
      mbedtls_strerror(nread, errorbuf, परिमाण(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned पूर्णाङ्क)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      भङ्ग;
    }
  }
  प्रत्यागम result;
}

स्थिर size_t mbedtls_version(वर्ण *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned पूर्णाङ्क version = mbedtls_version_get_सङ्ख्या();
  प्रत्यागम curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#अण्णहा
  प्रत्यागम curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
स्थिर CURLcode mbedtls_random(संरचना Curl_easy *data,
                               unsigned वर्ण *entropy, size_t length)
{
  psa_status_t status;
  (शून्य)data;

  status = psa_generate_random(entropy, length);

  प्रत्यागम status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

स्थिर CURLcode mbedtls_connect(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  संरचना ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  जइ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  जइ(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    जइ(result)
      प्रत्यागम result;
  }

  जइ(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    जइ(result)
      प्रत्यागम result;
  }

  जइ(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    संरचना ssl_connect_data *ctx = cf->ctx;
    संरचना mbed_ssl_backend_data *backend =
      (संरचना mbed_ssl_backend_data *)ctx->backend;

    जइ(mbedtls_ssl_get_version_सङ्ख्या(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      जइ(result)
        प्रत्यागम result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  जइ(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  प्रत्यागम CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
स्थिर पूर्णाङ्क mbedtls_init(शून्य)
{
#जइ MBEDTLS_VERSION_सङ्ख्या < 0x04000000
  पूर्णाङ्क ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  जइ(status != PSA_SUCCESS)
    प्रत्यागम 0;

#जइ MBEDTLS_VERSION_सङ्ख्या < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              रिक्त, 0);

  जइ(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    प्रत्यागम 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  प्रत्यागम 1;
}

स्थिर शून्य mbedtls_cleanup(शून्य)
{
  mbedtls_psa_crypto_free();

#जइ MBEDTLS_VERSION_सङ्ख्या < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

स्थिर bool mbedtls_data_pending(संरचना Curl_cfilter *cf,
                                 ध्रुव संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *ctx = cf->ctx;
  संरचना mbed_ssl_backend_data *backend;

  (शून्य)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (संरचना mbed_ssl_backend_data *)ctx->backend;
  प्रत्यागम mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

स्थिर CURLcode mbedtls_sha256sum(ध्रुव unsigned वर्ण *निवेशः,
                                  size_t inputlen,
                                  unsigned वर्ण *sha256sum,
                                  size_t sha256len)
{
#जइ defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, निवेशः, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  जइ(status != PSA_SUCCESS)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  प्रत्यागम CURLE_OK;
#अण्णहा
  (शून्य)sha256len;
  प्रत्यागम Curl_sha256it(sha256sum, निवेशः, inputlen);
#endif
}

स्थिर शून्य *mbedtls_get_internals(संरचना ssl_connect_data *connssl,
                                   CURLINFO info)
{
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  प्रत्यागम &backend->ssl;
}

ध्रुव संरचना Curl_ssl Curl_ssl_mbedtls = {
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

  परिमाण(संरचना mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  रिक्त,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  रिक्त,                             /* close_all */
  रिक्त,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  रिक्त,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
