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
#जर MBEDTLS_VERSION_संख्या < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#अन्तर्भाव <psa/crypto_config.h>
#अन्तर्भाव <mbedtls/net_sockets.h>
#अन्तर्भाव <mbedtls/ssl.h>
#अन्तर्भाव <mbedtls/x509.h>
#अन्तर्भाव <mbedtls/psa_util.h>

#जर MBEDTLS_VERSION_संख्या < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#अन्तर्भाव <mbedtls/error.h>
#जर MBEDTLS_VERSION_संख्या < 0x04000000
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
#जर defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
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
  स्थिरांक अक्षर *protocols[3];
#endif
  पूर्णांक *ciphersuites;
  माप_प्रकार send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#जर MBEDTLS_VERSION_संख्या < 0x04000000
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
स्थिर शून्य mbed_debug(शून्य *context, पूर्णांक level, स्थिरांक अक्षर *f_name,
                       पूर्णांक ओळ_nb, स्थिरांक अक्षर *ओळ)
{
  संरचना Curl_easy *data = (संरचना Curl_easy *)context;
  (शून्य)level;
  (शून्य)ओळ_nb;
  (शून्य)f_name;

  जर(data) {
    माप_प्रकार len = म_माप(ओळ);
    जर(len && (ओळ[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (पूर्णांक)len, ओळ);
  }
}
#endif

स्थिर पूर्णांक mbedtls_bio_cf_write(शून्य *bio,
                                स्थिरांक अचिन्हित अक्षर *buf, माप_प्रकार blen)
{
  संरचना Curl_cfilter *cf = bio;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  माप_प्रकार nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  जर(!data)
    परत 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (पूर्णांक)result, nwritten);
  जर(result == CURLE_AGAIN)
    परत MBEDTLS_ERR_SSL_WANT_WRITE;
  परत result ? -1 : (पूर्णांक)nwritten;
}

स्थिर पूर्णांक mbedtls_bio_cf_read(शून्य *bio, अचिन्हित अक्षर *buf, माप_प्रकार blen)
{
  संरचना Curl_cfilter *cf = bio;
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);
  माप_प्रकार nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  जर(!data)
    परत 0;
  /* OpenSSL catches this case, so should we. */
  जर(!buf)
    परत 0;

  result = Curl_conn_cf_recv(cf->next, data, (अक्षर *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (पूर्णांक)result, nread);
  जर(result == CURLE_AGAIN)
    परत MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  परत result ? -1 : (पूर्णांक)nread;
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
#अन्यथा
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#अन्यथा
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  निवड(conn_config->version) {
  स्थिती CURL_SSLVERSION_TLSv1:
  स्थिती CURL_SSLVERSION_TLSv1_0:
  स्थिती CURL_SSLVERSION_TLSv1_1:
  स्थिती CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    थांबा;
#endif
  स्थिती CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    थांबा;
#endif
  पूर्वनिर्धारित:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    परत CURLE_SSL_CONNECT_ERROR;
  }

  निवड(conn_config->version_max) {
  स्थिती CURL_SSLVERSION_MAX_DEFAULT:
  स्थिती CURL_SSLVERSION_MAX_NONE:
  स्थिती CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    थांबा;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  स्थिती CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    थांबा;
#endif
  स्थिती CURL_SSLVERSION_MAX_TLSv1_1:
  स्थिती CURL_SSLVERSION_MAX_TLSv1_0:
  पूर्वनिर्धारित:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    परत CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  परत CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
स्थिर पूर्णांक mbed_cipher_suite_get_str(uint16_t id, अक्षर *buf, माप_प्रकार buf_size,
                                     bool prefer_rfc)
{
  जर(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  अन्यथा
    परत Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  परत 0;
}

स्थिर uint16_t mbed_cipher_suite_walk_str(स्थिरांक अक्षर **str, स्थिरांक अक्षर **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  माप_प्रकार len = *end - *str;
  स्थिर स्थिरांक अक्षर ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  जर(!id) {
    जर((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  परत id;
}
#अन्यथा
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

स्थिर CURLcode mbed_set_selected_ciphers(
  संरचना Curl_easy *data,
  संरचना mbed_ssl_backend_data *backend,
  स्थिरांक अक्षर *ciphers12,
  स्थिरांक अक्षर *ciphers13)
{
  स्थिरांक अक्षर *ciphers = ciphers12;
  स्थिरांक पूर्णांक *supported;
  पूर्णांक *selected;
  माप_प्रकार supported_len, मोजणी = 0, default13_मोजणी = 0, i, j;
  स्थिरांक अक्षर *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  साठी(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(आकार(पूर्णांक) * (supported_len + 1));
  जर(!selected)
    परत CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (शून्य)ciphers13, (शून्य)j;
#अन्यथा
  जर(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    साठी(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      जर(म_भेदन(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        सुरूठेवा;

      selected[मोजणी++] = id;
    }

    default13_मोजणी = मोजणी;
  }
  अन्यथा
    ciphers = ciphers13;

add_ciphers:
#endif
  साठी(ptr = ciphers; ptr[0] != '\0' && मोजणी < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    जर(id) {
      साठी(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      जर(i == supported_len)
        id = 0;
    }
    जर(!id) {
      जर(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (पूर्णांक)(end - ptr), ptr);
      सुरूठेवा;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    साठी(i = 0; i < मोजणी && selected[i] != id; i++)
      ;
    जर(i < मोजणी) {
      जर(i >= default13_मोजणी)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (पूर्णांक)(end - ptr), ptr);
      सुरूठेवा;
    }

    selected[मोजणी++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  जर(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    जा add_ciphers;
  }

  जर(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    साठी(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      जर(!म_भेदन(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        सुरूठेवा;

      /* No duplicates allowed (so selected cannot overflow) */
      साठी(i = 0; i < मोजणी && selected[i] != id; i++)
        ;
      जर(i < मोजणी)
        सुरूठेवा;

      selected[मोजणी++] = id;
    }
  }
#endif

  selected[मोजणी] = 0;

  जर(मोजणी == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    परत CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  परत CURLE_OK;
}

स्थिर शून्य mbed_dump_cert_info(संरचना Curl_easy *data,
                                स्थिरांक mbedtls_x509_crt *crt)
{
#जर !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (शून्य)data, (शून्य)crt;
#अन्यथा
  स्थिरांक माप_प्रकार bufsize = 16384;
  अक्षर *p, *buffer = curlx_malloc(bufsize);

  जर(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    साठी(p = buffer; *p; p += *p != '\0') {
      माप_प्रकार s = म_खोज(p, "\n");
      infof(data, "%.*s", (पूर्णांक)s, p);
      p += s;
    }
  }
  अन्यथा
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

स्थिर शून्य mbed_extract_certinfo(संरचना Curl_easy *data,
                                  स्थिरांक mbedtls_x509_crt *crt)
{
  CURLcode result;
  स्थिरांक mbedtls_x509_crt *cur;
  पूर्णांक cert_मोजणी = 0;
  पूर्णांक i;

  साठी(cur = crt; cur && cert_मोजणी <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_मोजणी++;

  जर(cert_मोजणी > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    परत;
  }

  result = Curl_ssl_init_certinfo(data, cert_मोजणी);

  साठी(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

स्थिर पूर्णांक mbed_verify_cb(शून्य *ptr, mbedtls_x509_crt *crt,
                          पूर्णांक depth, uint32_t *flags)
{
  संरचना Curl_cfilter *cf = (संरचना Curl_cfilter *)ptr;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_easy *data = CF_DATA_CURRENT(cf);

  जर(depth == 0) {
    जर(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    जर(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  जर(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  जर(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  जर(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    अक्षर buf[128];
    mbedtls_x509_crt_verify_info(buf, आकार(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#अन्यथा
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (अचिन्हित पूर्णांक)*flags);
#endif
  }

  परत 0;
}

स्थिर CURLcode mbed_load_cacert(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  स्थिरांक संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
  स्थिरांक अक्षर * स्थिरांक ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? रिक्त : conn_config->CAfile);
  स्थिरांक bool verifypeer = conn_config->verifypeer;
  स्थिरांक अक्षर * स्थिरांक ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  स्थिरांक अक्षर * स्थिरांक ssl_cert_type = conn_config->cert_type;
#endif
  पूर्णांक ret = -1;
  अक्षर errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  जर(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    जर((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((अक्षर *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    अन्यथा { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      अचिन्हित अक्षर *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      जर(!newblob)
        परत CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#अन्यथा
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    जर(ret < 0) {
      mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (अचिन्हित पूर्णांक)-ret, errorbuf);
      परत CURLE_SSL_CERTPROBLEM;
    }
  }

  जर(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    जर(ret < 0) {
      mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (अचिन्हित पूर्णांक)-ret, errorbuf);
      परत CURLE_SSL_CACERT_BADFILE;
    }
#अन्यथा
    failf(data, "mbedTLS: functions that use the file system not built in");
    परत CURLE_NOT_BUILT_IN;
#endif
  }

  जर(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    जर(ret < 0) {
      mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (अचिन्हित पूर्णांक)-ret, errorbuf);

      जर(verifypeer)
        परत CURLE_SSL_CACERT_BADFILE;
    }
#अन्यथा
    failf(data, "mbedTLS: functions that use the file system not built in");
    परत CURLE_NOT_BUILT_IN;
#endif
  }

  परत CURLE_OK;
}

स्थिर CURLcode mbed_load_clicert(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  अक्षर * स्थिरांक ssl_cert = conn_config->clientcert;
  स्थिरांक संरचना curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  स्थिरांक अक्षर * स्थिरांक ssl_cert_type = conn_config->cert_type;
#endif
  पूर्णांक ret = -1;
  अक्षर errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  जर(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    जर(ret) {
      mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (अचिन्हित पूर्णांक)-ret, errorbuf);

      परत CURLE_SSL_CERTPROBLEM;
    }
#अन्यथा
    failf(data, "mbedTLS: functions that use the file system not built in");
    परत CURLE_NOT_BUILT_IN;
#endif
  }

  जर(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    जर((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((अक्षर *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    अन्यथा { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      अचिन्हित अक्षर *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      जर(!newblob)
        परत CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#अन्यथा
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    जर(ret) {
      mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (अचिन्हित पूर्णांक)-ret, errorbuf);
      परत CURLE_SSL_CERTPROBLEM;
    }
  }

  परत CURLE_OK;
}

स्थिर CURLcode mbed_load_privkey(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णांक ret = -1;
  अक्षर errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  जर(conn_config->किल्ली || conn_config->किल्ली_blob) {
    जर(conn_config->किल्ली) {
#ifdef MBEDTLS_FS_IO
#जर MBEDTLS_VERSION_संख्या >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->किल्ली,
                                     conn_config->किल्ली_passwd);
      जर(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_किल्ली_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_किल्ली_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#अन्यथा
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->किल्ली,
                                     conn_config->किल्ली_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      जर(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      जर(ret) {
        mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->किल्ली, (अचिन्हित पूर्णांक)-ret, errorbuf);
        परत CURLE_SSL_CERTPROBLEM;
      }
#अन्यथा
      failf(data, "mbedTLS: functions that use the file system not built in");
      परत CURLE_NOT_BUILT_IN;
#endif
    }
    अन्यथा {
      स्थिरांक संरचना curl_blob *ssl_किल्ली_blob = conn_config->किल्ली_blob;
      स्थिरांक अक्षर *passwd = conn_config->किल्ली_passwd;
      स्थिरांक bool is_der = conn_config->किल्ली_type &&
                          curl_strequal(conn_config->किल्ली_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      अचिन्हित अक्षर *newblob = curlx_memdup0(ssl_किल्ली_blob->data,
                                             ssl_किल्ली_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      माप_प्रकार keylen = is_der ? ssl_किल्ली_blob->len : ssl_किल्ली_blob->len + 1;
      जर(!newblob)
        परत CURLE_OUT_OF_MEMORY;

#जर MBEDTLS_VERSION_संख्या >= 0x04000000
      ret = mbedtls_pk_parse_किल्ली(&backend->pk, newblob, keylen,
                                 (स्थिरांक अचिन्हित अक्षर *)passwd,
                                 passwd ? म_माप(passwd) : 0);
      जर(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_किल्ली_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_किल्ली_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#अन्यथा
      ret = mbedtls_pk_parse_किल्ली(&backend->pk, newblob, keylen,
                                 (स्थिरांक अचिन्हित अक्षर *)passwd,
                                 passwd ? म_माप(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      जर(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      जर(ret) {
        mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (अचिन्हित पूर्णांक)-ret, errorbuf);
        परत CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  परत CURLE_OK;
}

स्थिर CURLcode mbed_load_crl(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  स्थिरांक अक्षर * स्थिरांक ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  जर(ssl_crlfile) {
    अक्षर errorbuf[128];
#ifdef MBEDTLS_FS_IO
    पूर्णांक ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    जर(ret) {
      mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (अचिन्हित पूर्णांक)-ret, errorbuf);

      परत CURLE_SSL_CRL_BADFILE;
    }
#अन्यथा
    (शून्य)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    परत CURLE_NOT_BUILT_IN;
#endif
  }
#अन्यथा
  (शून्य)backend;
  जर(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    परत CURLE_NOT_BUILT_IN;
  }
#endif

  परत CURLE_OK;
}

स्थिर bool mbed_apply_session(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना Curl_ssl_session *sc_session)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;

  जर(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    पूर्णांक ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    जर(ret) {
      failf(data, "SSL session error loading: -0x%x", (अचिन्हित पूर्णांक)-ret);
    }
    अन्यथा {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      जर(ret)
        failf(data, "SSL session error setting: -0x%x", (अचिन्हित पूर्णांक)-ret);
    }
    mbedtls_ssl_session_free(&session);
    जर(!ret) {
      infof(data, "SSL reusing session ID");
      परत TRUE;
    }
  }
  परत FALSE;
}

स्थिर CURLcode mbed_configure_ssl(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णांक ret;
  bool session_applied = FALSE;
  CURLcode result;
  अक्षर errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  जर(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    परत CURLE_SSL_CONNECT_ERROR;
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

#जर defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_संख्या >= 0x03060100 &&   \
  MBEDTLS_VERSION_संख्या < 0x04000000
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
  जर(result)
    परत result;

#जर MBEDTLS_VERSION_संख्या < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  जर(ret) {
    mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (अचिन्हित पूर्णांक)-ret, errorbuf);
    परत CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      रिक्त /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  जर(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, रिक्त);
#अन्यथा
  जर(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    जर(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      परत result;
    }
  }
  अन्यथा {
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

  जर((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    संरचना Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    जर(scs) {
      जर(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  जर(!session_applied && Curl_ssl_scache_use(cf, data)) {
    संरचना Curl_ssl_session *sc_session = रिक्त;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_किल्ली,
                                            &sc_session);
    जर(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      जर(ret) {
        failf(data, "SSL session error loading: -0x%x", (अचिन्हित पूर्णांक)-ret);
      }
      अन्यथा {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        जर(ret)
          failf(data, "SSL session error setting: -0x%x", (अचिन्हित पूर्णांक)-ret);
        अन्यथा
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_किल्ली,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#अन्यथा
                            रिक्त
#endif
    );

  जर(conn_config->किल्ली || conn_config->किल्ली_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  जर(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    परत CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  जर(connssl->alpn) {
    संरचना alpn_proto_buf proto;
    माप_प्रकार i;

    साठी(i = 0; i < connssl->alpn->मोजणी; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    जर(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      परत CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  जर(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    जर(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  परत result;
}

स्थिर CURLcode mbed_connect_step1(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  जर((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    परत CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  जर(!result)
    result = mbed_load_clicert(cf, data);
  जर(!result)
    result = mbed_load_privkey(cf, data);
  जर(!result)
    result = mbed_load_crl(cf, data);
  जर(!result)
    result = mbed_configure_ssl(cf, data);
  जर(result)
    परत result;

  connssl->connecting_state = ssl_connect_2;
  परत CURLE_OK;
}

#जर defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

स्थिर CURLcode mbed_connect_step2(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *data)
{
#जर defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  पूर्णांक ret;
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  स्थिरांक अक्षर * स्थिरांक pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#अन्यथा
  स्थिरांक अक्षर * स्थिरांक pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  जर(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    परत CURLE_OK;
  }
  अन्यथा जर(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    परत CURLE_OK;
  }
  अन्यथा जर(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    परत CURLE_PEER_FAILED_VERIFICATION;
  }
  अन्यथा जर(ret) {
    अक्षर errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_संख्या(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, आकार(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (अचिन्हित पूर्णांक)-ret, errorbuf);
    परत CURLE_SSL_CONNECT_ERROR;
  }

  {
    अक्षर cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, आकार(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  जर(pinnedpubkey) {
    पूर्णांक size;
    स्थिरांक mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = रिक्त;
    अचिन्हित अक्षर *pubkey = रिक्त;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    जर(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      परत CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, आकार(*p));

    जर(!p)
      परत CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    जर(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      जा pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    जर(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      जा pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    जर(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      जा pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    जर(result)
      परत result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  जर(connssl->alpn) {
    स्थिरांक अक्षर *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (स्थिरांक अचिन्हित अक्षर *)proto,
                                      proto ? म_माप(proto) : 0);
    जर(result)
      परत result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  परत CURLE_OK;
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
  अचिन्हित अक्षर *sdata = रिक्त;
  माप_प्रकार slen = 0;
  पूर्णांक ietf_tls_id;
  CURLcode result = CURLE_OK;
  पूर्णांक ret;

  DEBUGASSERT(backend);
  जर(!Curl_ssl_scache_use(cf, data))
    परत CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  जर(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (अचिन्हित पूर्णांक)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    जा out;
  }

  mbedtls_ssl_session_save(&session, रिक्त, 0, &slen);
  जर(!slen) {
    failf(data, "failed to serialize session: length is 0");
    जा out;
  }

  sdata = curlx_malloc(slen);
  जर(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  जर(ret) {
    failf(data, "failed to serialize session: -0x%x", (अचिन्हित पूर्णांक)-ret);
    जा out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_संख्या(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = रिक्त;  /* call took ownership */
  जर(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  जर(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_किल्ली,
                                 sc_session);
    sc_session = रिक्त;
  }

out:
  जर(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  परत result;
}

स्थिर CURLcode mbed_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          स्थिरांक शून्य *mem, माप_प्रकार len, माप_प्रकार *pnwritten)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  पूर्णांक nwritten;

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
  जर(backend->send_blocked) {
    जर(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      जा out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (स्थिरांक अचिन्हित अक्षर *)mem, len);

  जर(nwritten >= 0) {
    *pnwritten = (माप_प्रकार)nwritten;
    backend->send_blocked = FALSE;
  }
  अन्यथा {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (अचिन्हित पूर्णांक)-nwritten);
    निवड(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    स्थिती MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      जर(!result)
        जा do_send;
      थांबा;
#endif
    स्थिती MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      थांबा;
    स्थिती MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      थांबा;
    पूर्वनिर्धारित:
      result = CURLE_SEND_ERROR;
      थांबा;
    }
    जर((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (पूर्णांक)result, *pnwritten);
  परत result;
}

स्थिर CURLcode mbedtls_shutdown(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  अचिन्हित अक्षर buf[1024];
  CURLcode result = CURLE_OK;
  पूर्णांक ret = 0;
  माप_प्रकार i;

  DEBUGASSERT(backend);

  जर(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    परत CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  जर(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    जर(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      निवड(ret) {
      स्थिती 0: /* we sent it, receive from the server */
        थांबा;
      स्थिती MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        जा out;
      स्थिती MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        जा out;
      स्थिती MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        जा out;
      पूर्वनिर्धारित:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (अचिन्हित पूर्णांक)-ret);
        result = CURLE_RECV_ERROR;
        जा out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  साठी(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, आकार(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    जर(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, आकार(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    जर(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      सुरूठेवा;
#endif
    जर(ret <= 0)
      थांबा;
  }

  जर(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  अन्यथा जर(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  अन्यथा जर(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  अन्यथा जर(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  अन्यथा {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (अचिन्हित पूर्णांक)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  परत result;
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
  जर(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

स्थिर CURLcode mbed_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                          अक्षर *buf, माप_प्रकार buffersize, माप_प्रकार *pnread)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  पूर्णांक nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (अचिन्हित अक्षर *)buf, buffersize);
  जर(nread > 0)
    *pnread = (माप_प्रकार)nread;
  अन्यथा {
    अक्षर errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (अचिन्हित पूर्णांक)-nread);
    निवड(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    स्थिती MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      जर(!result)
        जा do_read;
      थांबा;
#endif
    स्थिती MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      थांबा;
    स्थिती MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      थांबा;
    स्थिती MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      थांबा;
    पूर्वनिर्धारित:
      mbedtls_strerror(nread, errorbuf, आकार(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (अचिन्हित पूर्णांक)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      थांबा;
    }
  }
  परत result;
}

स्थिर माप_प्रकार mbedtls_version(अक्षर *buffer, माप_प्रकार size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  अचिन्हित पूर्णांक version = mbedtls_version_get_संख्या();
  परत curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#अन्यथा
  परत curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
स्थिर CURLcode mbedtls_random(संरचना Curl_easy *data,
                               अचिन्हित अक्षर *entropy, माप_प्रकार length)
{
  psa_status_t status;
  (शून्य)data;

  status = psa_generate_random(entropy, length);

  परत status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

स्थिर CURLcode mbedtls_connect(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  संरचना ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  जर(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    परत CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  जर(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    जर(result)
      परत result;
  }

  जर(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    जर(result)
      परत result;
  }

  जर(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    संरचना ssl_connect_data *ctx = cf->ctx;
    संरचना mbed_ssl_backend_data *backend =
      (संरचना mbed_ssl_backend_data *)ctx->backend;

    जर(mbedtls_ssl_get_version_संख्या(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      जर(result)
        परत result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  जर(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  परत CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
स्थिर पूर्णांक mbedtls_init(शून्य)
{
#जर MBEDTLS_VERSION_संख्या < 0x04000000
  पूर्णांक ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  जर(status != PSA_SUCCESS)
    परत 0;

#जर MBEDTLS_VERSION_संख्या < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              रिक्त, 0);

  जर(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    परत 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  परत 1;
}

स्थिर शून्य mbedtls_cleanup(शून्य)
{
  mbedtls_psa_crypto_free();

#जर MBEDTLS_VERSION_संख्या < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

स्थिर bool mbedtls_data_pending(संरचना Curl_cfilter *cf,
                                 स्थिरांक संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *ctx = cf->ctx;
  संरचना mbed_ssl_backend_data *backend;

  (शून्य)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (संरचना mbed_ssl_backend_data *)ctx->backend;
  परत mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

स्थिर CURLcode mbedtls_sha256sum(स्थिरांक अचिन्हित अक्षर *आदान,
                                  माप_प्रकार inputlen,
                                  अचिन्हित अक्षर *sha256sum,
                                  माप_प्रकार sha256len)
{
#जर defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  माप_प्रकार sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, आदान, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  जर(status != PSA_SUCCESS)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  परत CURLE_OK;
#अन्यथा
  (शून्य)sha256len;
  परत Curl_sha256it(sha256sum, आदान, inputlen);
#endif
}

स्थिर शून्य *mbedtls_get_internals(संरचना ssl_connect_data *connssl,
                                   CURLINFO info)
{
  संरचना mbed_ssl_backend_data *backend =
    (संरचना mbed_ssl_backend_data *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  परत &backend->ssl;
}

स्थिरांक संरचना Curl_ssl Curl_ssl_mbedtls = {
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

  आकार(संरचना mbed_ssl_backend_data),

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
