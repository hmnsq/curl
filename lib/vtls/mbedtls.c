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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#அந்தர்பாவ <mbedtls/version.h>
#எனில் MBEDTLS_VERSION_எண் < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#அந்தர்பாவ <psa/crypto_config.h>
#அந்தர்பாவ <mbedtls/net_sockets.h>
#அந்தர்பாவ <mbedtls/ssl.h>
#அந்தர்பாவ <mbedtls/x509.h>
#அந்தர்பாவ <mbedtls/psa_util.h>

#எனில் MBEDTLS_VERSION_எண் < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#அந்தர்பாவ <mbedtls/error.h>
#எனில் MBEDTLS_VERSION_எண் < 0x04000000
#அந்தர்பாவ <mbedtls/entropy.h>
#அந்தர்பாவ <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#அந்தர்பாவ <mbedtls/debug.h>
#endif

#அந்தர்பாவ "vtls/cipher_suite.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "vtls/mbedtls.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "vtls/vtls_int.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "vtls/x509asn1.h"
#அந்தர்பாவ "connect.h" /* for the connect timeout */
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curl_sha256.h"

/* ALPN for http2 */
#எனில் defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

கட்டமைப்பு mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  மாறிலி எழுத்து *protocols[3];
#endif
  முழு *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#எனில் MBEDTLS_VERSION_எண் < 0x04000000
கட்டமைப்பு rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

நிலையான கட்டமைப்பு rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
நிலையான வெறுமை mbed_debug(வெறுமை *context, முழு level, மாறிலி எழுத்து *f_name,
                       முழு வரி_nb, மாறிலி எழுத்து *வரி)
{
  கட்டமைப்பு Curl_easy *data = (கட்டமைப்பு Curl_easy *)context;
  (வெறுமை)level;
  (வெறுமை)வரி_nb;
  (வெறுமை)f_name;

  எனில்(data) {
    size_t len = strlen(வரி);
    எனில்(len && (வரி[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (முழு)len, வரி);
  }
}
#endif

நிலையான முழு mbedtls_bio_cf_write(வெறுமை *bio,
                                மாறிலி unsigned எழுத்து *buf, size_t blen)
{
  கட்டமைப்பு Curl_cfilter *cf = bio;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  எனில்(!data)
    திரும்பு 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (முழு)result, nwritten);
  எனில்(result == CURLE_AGAIN)
    திரும்பு MBEDTLS_ERR_SSL_WANT_WRITE;
  திரும்பு result ? -1 : (முழு)nwritten;
}

நிலையான முழு mbedtls_bio_cf_read(வெறுமை *bio, unsigned எழுத்து *buf, size_t blen)
{
  கட்டமைப்பு Curl_cfilter *cf = bio;
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  எனில்(!data)
    திரும்பு 0;
  /* OpenSSL catches this case, so should we. */
  எனில்(!buf)
    திரும்பு 0;

  result = Curl_conn_cf_recv(cf->next, data, (எழுத்து *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (முழு)result, nread);
  எனில்(result == CURLE_AGAIN)
    திரும்பு MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  திரும்பு result ? -1 : (முழு)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

நிலையான CURLcode mbed_set_ssl_version_min_max(
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு mbed_ssl_backend_data *backend,
  கட்டமைப்பு ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#இல்லையெனில்
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#இல்லையெனில்
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  தேர்வு(conn_config->version) {
  நிலை CURL_SSLVERSION_TLSv1:
  நிலை CURL_SSLVERSION_TLSv1_0:
  நிலை CURL_SSLVERSION_TLSv1_1:
  நிலை CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    நிறுத்து;
#endif
  நிலை CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    நிறுத்து;
#endif
  இயல்பு:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  தேர்வு(conn_config->version_max) {
  நிலை CURL_SSLVERSION_MAX_DEFAULT:
  நிலை CURL_SSLVERSION_MAX_NONE:
  நிலை CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    நிறுத்து;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  நிலை CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    நிறுத்து;
#endif
  நிலை CURL_SSLVERSION_MAX_TLSv1_1:
  நிலை CURL_SSLVERSION_MAX_TLSv1_0:
  இயல்பு:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  திரும்பு CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
நிலையான முழு mbed_cipher_suite_get_str(uint16_t id, எழுத்து *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  எனில்(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  இல்லையெனில்
    திரும்பு Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  திரும்பு 0;
}

நிலையான uint16_t mbed_cipher_suite_walk_str(மாறிலி எழுத்து **str, மாறிலி எழுத்து **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  நிலையான மாறிலி எழுத்து ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  எனில்(!id) {
    எனில்((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  திரும்பு id;
}
#இல்லையெனில்
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

நிலையான CURLcode mbed_set_selected_ciphers(
  கட்டமைப்பு Curl_easy *data,
  கட்டமைப்பு mbed_ssl_backend_data *backend,
  மாறிலி எழுத்து *ciphers12,
  மாறிலி எழுத்து *ciphers13)
{
  மாறிலி எழுத்து *ciphers = ciphers12;
  மாறிலி முழு *supported;
  முழு *selected;
  size_t supported_len, மொத்தம் = 0, default13_மொத்தம் = 0, i, j;
  மாறிலி எழுத்து *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  க்கு(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(அளவு(முழு) * (supported_len + 1));
  எனில்(!selected)
    திரும்பு CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (வெறுமை)ciphers13, (வெறுமை)j;
#இல்லையெனில்
  எனில்(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    க்கு(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      எனில்(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        தொடர்;

      selected[மொத்தம்++] = id;
    }

    default13_மொத்தம் = மொத்தம்;
  }
  இல்லையெனில்
    ciphers = ciphers13;

add_ciphers:
#endif
  க்கு(ptr = ciphers; ptr[0] != '\0' && மொத்தம் < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    எனில்(id) {
      க்கு(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      எனில்(i == supported_len)
        id = 0;
    }
    எனில்(!id) {
      எனில்(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (முழு)(end - ptr), ptr);
      தொடர்;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    க்கு(i = 0; i < மொத்தம் && selected[i] != id; i++)
      ;
    எனில்(i < மொத்தம்) {
      எனில்(i >= default13_மொத்தம்)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (முழு)(end - ptr), ptr);
      தொடர்;
    }

    selected[மொத்தம்++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  எனில்(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    செல் add_ciphers;
  }

  எனில்(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    க்கு(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      எனில்(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        தொடர்;

      /* No duplicates allowed (so selected cannot overflow) */
      க்கு(i = 0; i < மொத்தம் && selected[i] != id; i++)
        ;
      எனில்(i < மொத்தம்)
        தொடர்;

      selected[மொத்தம்++] = id;
    }
  }
#endif

  selected[மொத்தம்] = 0;

  எனில்(மொத்தம் == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    திரும்பு CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  திரும்பு CURLE_OK;
}

நிலையான வெறுமை mbed_dump_cert_info(கட்டமைப்பு Curl_easy *data,
                                மாறிலி mbedtls_x509_crt *crt)
{
#எனில் !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (வெறுமை)data, (வெறுமை)crt;
#இல்லையெனில்
  மாறிலி size_t bufsize = 16384;
  எழுத்து *p, *buffer = curlx_malloc(bufsize);

  எனில்(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    க்கு(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (முழு)s, p);
      p += s;
    }
  }
  இல்லையெனில்
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

நிலையான வெறுமை mbed_extract_certinfo(கட்டமைப்பு Curl_easy *data,
                                  மாறிலி mbedtls_x509_crt *crt)
{
  CURLcode result;
  மாறிலி mbedtls_x509_crt *cur;
  முழு cert_மொத்தம் = 0;
  முழு i;

  க்கு(cur = crt; cur && cert_மொத்தம் <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_மொத்தம்++;

  எனில்(cert_மொத்தம் > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    திரும்பு;
  }

  result = Curl_ssl_init_certinfo(data, cert_மொத்தம்);

  க்கு(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

நிலையான முழு mbed_verify_cb(வெறுமை *ptr, mbedtls_x509_crt *crt,
                          முழு depth, uint32_t *flags)
{
  கட்டமைப்பு Curl_cfilter *cf = (கட்டமைப்பு Curl_cfilter *)ptr;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு Curl_easy *data = CF_DATA_CURRENT(cf);

  எனில்(depth == 0) {
    எனில்(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    எனில்(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  எனில்(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  எனில்(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  எனில்(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    எழுத்து buf[128];
    mbedtls_x509_crt_verify_info(buf, அளவு(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#இல்லையெனில்
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned முழு)*flags);
#endif
  }

  திரும்பு 0;
}

நிலையான CURLcode mbed_load_cacert(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  மாறிலி கட்டமைப்பு curl_blob *ca_info_blob = conn_config->ca_info_blob;
  மாறிலி எழுத்து * மாறிலி ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? வெற்று : conn_config->CAfile);
  மாறிலி bool verifypeer = conn_config->verifypeer;
  மாறிலி எழுத்து * மாறிலி ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  மாறிலி எழுத்து * மாறிலி ssl_cert_type = conn_config->cert_type;
#endif
  முழு ret = -1;
  எழுத்து errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  எனில்(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    எனில்((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((எழுத்து *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    இல்லையெனில் { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned எழுத்து *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      எனில்(!newblob)
        திரும்பு CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#இல்லையெனில்
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    எனில்(ret < 0) {
      mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned முழு)-ret, errorbuf);
      திரும்பு CURLE_SSL_CERTPROBLEM;
    }
  }

  எனில்(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    எனில்(ret < 0) {
      mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned முழு)-ret, errorbuf);
      திரும்பு CURLE_SSL_CACERT_BADFILE;
    }
#இல்லையெனில்
    failf(data, "mbedTLS: functions that use the file system not built in");
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  }

  எனில்(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    எனில்(ret < 0) {
      mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned முழு)-ret, errorbuf);

      எனில்(verifypeer)
        திரும்பு CURLE_SSL_CACERT_BADFILE;
    }
#இல்லையெனில்
    failf(data, "mbedTLS: functions that use the file system not built in");
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode mbed_load_clicert(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  எழுத்து * மாறிலி ssl_cert = conn_config->clientcert;
  மாறிலி கட்டமைப்பு curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  மாறிலி எழுத்து * மாறிலி ssl_cert_type = conn_config->cert_type;
#endif
  முழு ret = -1;
  எழுத்து errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  எனில்(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    எனில்(ret) {
      mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned முழு)-ret, errorbuf);

      திரும்பு CURLE_SSL_CERTPROBLEM;
    }
#இல்லையெனில்
    failf(data, "mbedTLS: functions that use the file system not built in");
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  }

  எனில்(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    எனில்((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((எழுத்து *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    இல்லையெனில் { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned எழுத்து *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      எனில்(!newblob)
        திரும்பு CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#இல்லையெனில்
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    எனில்(ret) {
      mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned முழு)-ret, errorbuf);
      திரும்பு CURLE_SSL_CERTPROBLEM;
    }
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode mbed_load_privkey(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  முழு ret = -1;
  எழுத்து errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  எனில்(conn_config->சாவி || conn_config->சாவி_blob) {
    எனில்(conn_config->சாவி) {
#ifdef MBEDTLS_FS_IO
#எனில் MBEDTLS_VERSION_எண் >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->சாவி,
                                     conn_config->சாவி_passwd);
      எனில்(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_சாவி_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_சாவி_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#இல்லையெனில்
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->சாவி,
                                     conn_config->சாவி_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      எனில்(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      எனில்(ret) {
        mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->சாவி, (unsigned முழு)-ret, errorbuf);
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }
#இல்லையெனில்
      failf(data, "mbedTLS: functions that use the file system not built in");
      திரும்பு CURLE_NOT_BUILT_IN;
#endif
    }
    இல்லையெனில் {
      மாறிலி கட்டமைப்பு curl_blob *ssl_சாவி_blob = conn_config->சாவி_blob;
      மாறிலி எழுத்து *passwd = conn_config->சாவி_passwd;
      மாறிலி bool is_der = conn_config->சாவி_type &&
                          curl_strequal(conn_config->சாவி_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned எழுத்து *newblob = curlx_memdup0(ssl_சாவி_blob->data,
                                             ssl_சாவி_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_சாவி_blob->len : ssl_சாவி_blob->len + 1;
      எனில்(!newblob)
        திரும்பு CURLE_OUT_OF_MEMORY;

#எனில் MBEDTLS_VERSION_எண் >= 0x04000000
      ret = mbedtls_pk_parse_சாவி(&backend->pk, newblob, keylen,
                                 (மாறிலி unsigned எழுத்து *)passwd,
                                 passwd ? strlen(passwd) : 0);
      எனில்(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_சாவி_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_சாவி_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#இல்லையெனில்
      ret = mbedtls_pk_parse_சாவி(&backend->pk, newblob, keylen,
                                 (மாறிலி unsigned எழுத்து *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      எனில்(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      எனில்(ret) {
        mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned முழு)-ret, errorbuf);
        திரும்பு CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode mbed_load_crl(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  மாறிலி எழுத்து * மாறிலி ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  எனில்(ssl_crlfile) {
    எழுத்து errorbuf[128];
#ifdef MBEDTLS_FS_IO
    முழு ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    எனில்(ret) {
      mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned முழு)-ret, errorbuf);

      திரும்பு CURLE_SSL_CRL_BADFILE;
    }
#இல்லையெனில்
    (வெறுமை)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    திரும்பு CURLE_NOT_BUILT_IN;
#endif
  }
#இல்லையெனில்
  (வெறுமை)backend;
  எனில்(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    திரும்பு CURLE_NOT_BUILT_IN;
  }
#endif

  திரும்பு CURLE_OK;
}

நிலையான bool mbed_apply_session(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு Curl_ssl_session *sc_session)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;

  எனில்(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    முழு ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    எனில்(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned முழு)-ret);
    }
    இல்லையெனில் {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      எனில்(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned முழு)-ret);
    }
    mbedtls_ssl_session_free(&session);
    எனில்(!ret) {
      infof(data, "SSL reusing session ID");
      திரும்பு TRUE;
    }
  }
  திரும்பு FALSE;
}

நிலையான CURLcode mbed_configure_ssl(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  முழு ret;
  bool session_applied = FALSE;
  CURLcode result;
  எழுத்து errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  எனில்(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
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

#எனில் defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_எண் >= 0x03060100 &&   \
  MBEDTLS_VERSION_எண் < 0x04000000
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
  எனில்(result)
    திரும்பு result;

#எனில் MBEDTLS_VERSION_எண் < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  எனில்(ret) {
    mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned முழு)-ret, errorbuf);
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      வெற்று /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  எனில்(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, வெற்று);
#இல்லையெனில்
  எனில்(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    எனில்(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      திரும்பு result;
    }
  }
  இல்லையெனில் {
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

  எனில்((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    கட்டமைப்பு Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    எனில்(scs) {
      எனில்(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  எனில்(!session_applied && Curl_ssl_scache_use(cf, data)) {
    கட்டமைப்பு Curl_ssl_session *sc_session = வெற்று;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_சாவி,
                                            &sc_session);
    எனில்(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      எனில்(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned முழு)-ret);
      }
      இல்லையெனில் {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        எனில்(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned முழு)-ret);
        இல்லையெனில்
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_சாவி,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#இல்லையெனில்
                            வெற்று
#endif
    );

  எனில்(conn_config->சாவி || conn_config->சாவி_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  எனில்(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  எனில்(connssl->alpn) {
    கட்டமைப்பு alpn_proto_buf proto;
    size_t i;

    க்கு(i = 0; i < connssl->alpn->மொத்தம்; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    எனில்(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      திரும்பு CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  எனில்(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    எனில்(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  திரும்பு result;
}

நிலையான CURLcode mbed_connect_step1(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  எனில்((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    திரும்பு CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  எனில்(!result)
    result = mbed_load_clicert(cf, data);
  எனில்(!result)
    result = mbed_load_privkey(cf, data);
  எனில்(!result)
    result = mbed_load_crl(cf, data);
  எனில்(!result)
    result = mbed_configure_ssl(cf, data);
  எனில்(result)
    திரும்பு result;

  connssl->connecting_state = ssl_connect_2;
  திரும்பு CURLE_OK;
}

#எனில் defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

நிலையான CURLcode mbed_connect_step2(கட்டமைப்பு Curl_cfilter *cf,
                                   கட்டமைப்பு Curl_easy *data)
{
#எனில் defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  முழு ret;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  மாறிலி எழுத்து * மாறிலி pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#இல்லையெனில்
  மாறிலி எழுத்து * மாறிலி pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  எனில்(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் எனில்(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    திரும்பு CURLE_OK;
  }
  இல்லையெனில் எனில்(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    திரும்பு CURLE_PEER_FAILED_VERIFICATION;
  }
  இல்லையெனில் எனில்(ret) {
    எழுத்து errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_எண்(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, அளவு(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned முழு)-ret, errorbuf);
    திரும்பு CURLE_SSL_CONNECT_ERROR;
  }

  {
    எழுத்து cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, அளவு(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  எனில்(pinnedpubkey) {
    முழு size;
    மாறிலி mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = வெற்று;
    unsigned எழுத்து *pubkey = வெற்று;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    எனில்(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      திரும்பு CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, அளவு(*p));

    எனில்(!p)
      திரும்பு CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    எனில்(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      செல் pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    எனில்(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      செல் pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    எனில்(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      செல் pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    எனில்(result)
      திரும்பு result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  எனில்(connssl->alpn) {
    மாறிலி எழுத்து *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (மாறிலி unsigned எழுத்து *)proto,
                                      proto ? strlen(proto) : 0);
    எனில்(result)
      திரும்பு result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  திரும்பு CURLE_OK;
}

நிலையான CURLcode mbed_new_session(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  கட்டமைப்பு Curl_ssl_session *sc_session = வெற்று;
  unsigned எழுத்து *sdata = வெற்று;
  size_t slen = 0;
  முழு ietf_tls_id;
  CURLcode result = CURLE_OK;
  முழு ret;

  DEBUGASSERT(backend);
  எனில்(!Curl_ssl_scache_use(cf, data))
    திரும்பு CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  எனில்(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned முழு)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    செல் out;
  }

  mbedtls_ssl_session_save(&session, வெற்று, 0, &slen);
  எனில்(!slen) {
    failf(data, "failed to serialize session: length is 0");
    செல் out;
  }

  sdata = curlx_malloc(slen);
  எனில்(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  எனில்(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned முழு)-ret);
    செல் out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_எண்(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = வெற்று;  /* call took ownership */
  எனில்(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  எனில்(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_சாவி,
                                 sc_session);
    sc_session = வெற்று;
  }

out:
  எனில்(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  திரும்பு result;
}

நிலையான CURLcode mbed_send(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                          மாறிலி வெறுமை *mem, size_t len, size_t *pnwritten)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  முழு nwritten;

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
  எனில்(backend->send_blocked) {
    எனில்(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      செல் out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (மாறிலி unsigned எழுத்து *)mem, len);

  எனில்(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  இல்லையெனில் {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned முழு)-nwritten);
    தேர்வு(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    நிலை MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      எனில்(!result)
        செல் do_send;
      நிறுத்து;
#endif
    நிலை MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      நிறுத்து;
    நிலை MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      நிறுத்து;
    இயல்பு:
      result = CURLE_SEND_ERROR;
      நிறுத்து;
    }
    எனில்((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (முழு)result, *pnwritten);
  திரும்பு result;
}

நிலையான CURLcode mbedtls_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  unsigned எழுத்து buf[1024];
  CURLcode result = CURLE_OK;
  முழு ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  எனில்(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  எனில்(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    எனில்(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      தேர்வு(ret) {
      நிலை 0: /* we sent it, receive from the server */
        நிறுத்து;
      நிலை MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        செல் out;
      நிலை MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        செல் out;
      நிலை MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        செல் out;
      இயல்பு:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned முழு)-ret);
        result = CURLE_RECV_ERROR;
        செல் out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  க்கு(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, அளவு(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    எனில்(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, அளவு(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    எனில்(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      தொடர்;
#endif
    எனில்(ret <= 0)
      நிறுத்து;
  }

  எனில்(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  இல்லையெனில் எனில்(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  இல்லையெனில் எனில்(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  இல்லையெனில் எனில்(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  இல்லையெனில் {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned முழு)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  திரும்பு result;
}

நிலையான வெறுமை mbedtls_close(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;

  (வெறுமை)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  எனில்(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

நிலையான CURLcode mbed_recv(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                          எழுத்து *buf, size_t buffersize, size_t *pnread)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  முழு nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned எழுத்து *)buf, buffersize);
  எனில்(nread > 0)
    *pnread = (size_t)nread;
  இல்லையெனில் {
    எழுத்து errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned முழு)-nread);
    தேர்வு(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    நிலை MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      எனில்(!result)
        செல் do_read;
      நிறுத்து;
#endif
    நிலை MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      நிறுத்து;
    நிலை MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      நிறுத்து;
    நிலை MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      நிறுத்து;
    இயல்பு:
      mbedtls_strerror(nread, errorbuf, அளவு(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned முழு)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      நிறுத்து;
    }
  }
  திரும்பு result;
}

நிலையான size_t mbedtls_version(எழுத்து *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned முழு version = mbedtls_version_get_எண்();
  திரும்பு curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#இல்லையெனில்
  திரும்பு curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
நிலையான CURLcode mbedtls_random(கட்டமைப்பு Curl_easy *data,
                               unsigned எழுத்து *entropy, size_t length)
{
  psa_status_t status;
  (வெறுமை)data;

  status = psa_generate_random(entropy, length);

  திரும்பு status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

நிலையான CURLcode mbedtls_connect(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  எனில்(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  எனில்(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    எனில்(result)
      திரும்பு result;
  }

  எனில்(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    கட்டமைப்பு ssl_connect_data *ctx = cf->ctx;
    கட்டமைப்பு mbed_ssl_backend_data *backend =
      (கட்டமைப்பு mbed_ssl_backend_data *)ctx->backend;

    எனில்(mbedtls_ssl_get_version_எண்(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      எனில்(result)
        திரும்பு result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  எனில்(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  திரும்பு CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
நிலையான முழு mbedtls_init(வெறுமை)
{
#எனில் MBEDTLS_VERSION_எண் < 0x04000000
  முழு ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  எனில்(status != PSA_SUCCESS)
    திரும்பு 0;

#எனில் MBEDTLS_VERSION_எண் < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              வெற்று, 0);

  எனில்(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    திரும்பு 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  திரும்பு 1;
}

நிலையான வெறுமை mbedtls_cleanup(வெறுமை)
{
  mbedtls_psa_crypto_free();

#எனில் MBEDTLS_VERSION_எண் < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

நிலையான bool mbedtls_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                                 மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *ctx = cf->ctx;
  கட்டமைப்பு mbed_ssl_backend_data *backend;

  (வெறுமை)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (கட்டமைப்பு mbed_ssl_backend_data *)ctx->backend;
  திரும்பு mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

நிலையான CURLcode mbedtls_sha256sum(மாறிலி unsigned எழுத்து *உள்ளீடு,
                                  size_t inputlen,
                                  unsigned எழுத்து *sha256sum,
                                  size_t sha256len)
{
#எனில் defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, உள்ளீடு, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  எனில்(status != PSA_SUCCESS)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  திரும்பு CURLE_OK;
#இல்லையெனில்
  (வெறுமை)sha256len;
  திரும்பு Curl_sha256it(sha256sum, உள்ளீடு, inputlen);
#endif
}

நிலையான வெறுமை *mbedtls_get_internals(கட்டமைப்பு ssl_connect_data *connssl,
                                   CURLINFO info)
{
  கட்டமைப்பு mbed_ssl_backend_data *backend =
    (கட்டமைப்பு mbed_ssl_backend_data *)connssl->backend;
  (வெறுமை)info;
  DEBUGASSERT(backend);
  திரும்பு &backend->ssl;
}

மாறிலி கட்டமைப்பு Curl_ssl Curl_ssl_mbedtls = {
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

  அளவு(கட்டமைப்பு mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  வெற்று,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  வெற்று,                             /* close_all */
  வெற்று,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  வெற்று,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
