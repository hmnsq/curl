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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#ಅನ್ತರ್ಭಾವ <mbedtls/version.h>
#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#ಅನ್ತರ್ಭಾವ <psa/crypto_config.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/net_sockets.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/ssl.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/x509.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/psa_util.h>

#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#ಅನ್ತರ್ಭಾವ <mbedtls/error.h>
#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ < 0x04000000
#ಅನ್ತರ್ಭಾವ <mbedtls/entropy.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#ಅನ್ತರ್ಭಾವ <mbedtls/debug.h>
#endif

#ಅನ್ತರ್ಭಾವ "vtls/cipher_suite.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "vtls/mbedtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vtls/x509asn1.h"
#ಅನ್ತರ್ಭಾವ "connect.h" /* for the connect timeout */
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"

/* ALPN for http2 */
#ಯದಿ defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

ಸಂರಚನಾ mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  ಧ್ರುವ ವರ್ಣ *protocols[3];
#endif
  ಪೂರ್ಣಾಙ್ಕ *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ < 0x04000000
ಸಂರಚನಾ rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ಸ್ಥಿರ ಸಂರಚನಾ rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
ಸ್ಥಿರ ಶೂನ್ಯ mbed_debug(ಶೂನ್ಯ *context, ಪೂರ್ಣಾಙ್ಕ level, ಧ್ರುವ ವರ್ಣ *f_name,
                       ಪೂರ್ಣಾಙ್ಕ ಸಾಲು_nb, ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
  ಸಂರಚನಾ Curl_easy *data = (ಸಂರಚನಾ Curl_easy *)context;
  (ಶೂನ್ಯ)level;
  (ಶೂನ್ಯ)ಸಾಲು_nb;
  (ಶೂನ್ಯ)f_name;

  ಯದಿ(data) {
    size_t len = strlen(ಸಾಲು);
    ಯದಿ(len && (ಸಾಲು[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (ಪೂರ್ಣಾಙ್ಕ)len, ಸಾಲು);
  }
}
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mbedtls_bio_cf_write(ಶೂನ್ಯ *bio,
                                ಧ್ರುವ unsigned ವರ್ಣ *buf, size_t blen)
{
  ಸಂರಚನಾ Curl_cfilter *cf = bio;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಙ್ಕ)result, nwritten);
  ಯದಿ(result == CURLE_AGAIN)
    ಪ್ರತ್ಯಾಗಮ MBEDTLS_ERR_SSL_WANT_WRITE;
  ಪ್ರತ್ಯಾಗಮ result ? -1 : (ಪೂರ್ಣಾಙ್ಕ)nwritten;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mbedtls_bio_cf_read(ಶೂನ್ಯ *bio, unsigned ವರ್ಣ *buf, size_t blen)
{
  ಸಂರಚನಾ Curl_cfilter *cf = bio;
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  ಯದಿ(!data)
    ಪ್ರತ್ಯಾಗಮ 0;
  /* OpenSSL catches this case, so should we. */
  ಯದಿ(!buf)
    ಪ್ರತ್ಯಾಗಮ 0;

  result = Curl_conn_cf_recv(cf->next, data, (ವರ್ಣ *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಙ್ಕ)result, nread);
  ಯದಿ(result == CURLE_AGAIN)
    ಪ್ರತ್ಯಾಗಮ MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  ಪ್ರತ್ಯಾಗಮ result ? -1 : (ಪೂರ್ಣಾಙ್ಕ)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

ಸ್ಥಿರ CURLcode mbed_set_ssl_version_min_max(
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ mbed_ssl_backend_data *backend,
  ಸಂರಚನಾ ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#ಅನ್ಯಥಾ
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#ಅನ್ಯಥಾ
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  ವಿಕಲ್ಪ(conn_config->version) {
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1:
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_0:
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_1:
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    ಭಙ್ಗ;
#endif
  ಸ್ವಭಾವ:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  ವಿಕಲ್ಪ(conn_config->version_max) {
  ಪಕ್ಷ CURL_SSLVERSION_MAX_DEFAULT:
  ಪಕ್ಷ CURL_SSLVERSION_MAX_NONE:
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    ಭಙ್ಗ;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    ಭಙ್ಗ;
#endif
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_1:
  ಪಕ್ಷ CURL_SSLVERSION_MAX_TLSv1_0:
  ಸ್ವಭಾವ:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mbed_cipher_suite_get_str(uint16_t id, ವರ್ಣ *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  ಯದಿ(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ uint16_t mbed_cipher_suite_walk_str(ಧ್ರುವ ವರ್ಣ **str, ಧ್ರುವ ವರ್ಣ **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  ಯದಿ(!id) {
    ಯದಿ((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  ಪ್ರತ್ಯಾಗಮ id;
}
#ಅನ್ಯಥಾ
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

ಸ್ಥಿರ CURLcode mbed_set_selected_ciphers(
  ಸಂರಚನಾ Curl_easy *data,
  ಸಂರಚನಾ mbed_ssl_backend_data *backend,
  ಧ್ರುವ ವರ್ಣ *ciphers12,
  ಧ್ರುವ ವರ್ಣ *ciphers13)
{
  ಧ್ರುವ ವರ್ಣ *ciphers = ciphers12;
  ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ *supported;
  ಪೂರ್ಣಾಙ್ಕ *selected;
  size_t supported_len, ಎಣಿಕೆ = 0, default13_ಎಣಿಕೆ = 0, i, j;
  ಧ್ರುವ ವರ್ಣ *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  ಪ್ರತಿ(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * (supported_len + 1));
  ಯದಿ(!selected)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (ಶೂನ್ಯ)ciphers13, (ಶೂನ್ಯ)j;
#ಅನ್ಯಥಾ
  ಯದಿ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    ಪ್ರತಿ(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ಯದಿ(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ಅನುವರ್ತ;

      selected[ಎಣಿಕೆ++] = id;
    }

    default13_ಎಣಿಕೆ = ಎಣಿಕೆ;
  }
  ಅನ್ಯಥಾ
    ciphers = ciphers13;

add_ciphers:
#endif
  ಪ್ರತಿ(ptr = ciphers; ptr[0] != '\0' && ಎಣಿಕೆ < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    ಯದಿ(id) {
      ಪ್ರತಿ(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      ಯದಿ(i == supported_len)
        id = 0;
    }
    ಯದಿ(!id) {
      ಯದಿ(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (ಪೂರ್ಣಾಙ್ಕ)(end - ptr), ptr);
      ಅನುವರ್ತ;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    ಪ್ರತಿ(i = 0; i < ಎಣಿಕೆ && selected[i] != id; i++)
      ;
    ಯದಿ(i < ಎಣಿಕೆ) {
      ಯದಿ(i >= default13_ಎಣಿಕೆ)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (ಪೂರ್ಣಾಙ್ಕ)(end - ptr), ptr);
      ಅನುವರ್ತ;
    }

    selected[ಎಣಿಕೆ++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  ಯದಿ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    ಗಚ್ಛ add_ciphers;
  }

  ಯದಿ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    ಪ್ರತಿ(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ಯದಿ(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ಅನುವರ್ತ;

      /* No duplicates allowed (so selected cannot overflow) */
      ಪ್ರತಿ(i = 0; i < ಎಣಿಕೆ && selected[i] != id; i++)
        ;
      ಯದಿ(i < ಎಣಿಕೆ)
        ಅನುವರ್ತ;

      selected[ಎಣಿಕೆ++] = id;
    }
  }
#endif

  selected[ಎಣಿಕೆ] = 0;

  ಯದಿ(ಎಣಿಕೆ == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ mbed_dump_cert_info(ಸಂರಚನಾ Curl_easy *data,
                                ಧ್ರುವ mbedtls_x509_crt *crt)
{
#ಯದಿ !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (ಶೂನ್ಯ)data, (ಶೂನ್ಯ)crt;
#ಅನ್ಯಥಾ
  ಧ್ರುವ size_t bufsize = 16384;
  ವರ್ಣ *p, *buffer = curlx_malloc(bufsize);

  ಯದಿ(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    ಪ್ರತಿ(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (ಪೂರ್ಣಾಙ್ಕ)s, p);
      p += s;
    }
  }
  ಅನ್ಯಥಾ
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ mbed_extract_certinfo(ಸಂರಚನಾ Curl_easy *data,
                                  ಧ್ರುವ mbedtls_x509_crt *crt)
{
  CURLcode result;
  ಧ್ರುವ mbedtls_x509_crt *cur;
  ಪೂರ್ಣಾಙ್ಕ cert_ಎಣಿಕೆ = 0;
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ(cur = crt; cur && cert_ಎಣಿಕೆ <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_ಎಣಿಕೆ++;

  ಯದಿ(cert_ಎಣಿಕೆ > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    ಪ್ರತ್ಯಾಗಮ;
  }

  result = Curl_ssl_init_certinfo(data, cert_ಎಣಿಕೆ);

  ಪ್ರತಿ(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mbed_verify_cb(ಶೂನ್ಯ *ptr, mbedtls_x509_crt *crt,
                          ಪೂರ್ಣಾಙ್ಕ depth, uint32_t *flags)
{
  ಸಂರಚನಾ Curl_cfilter *cf = (ಸಂರಚನಾ Curl_cfilter *)ptr;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ Curl_easy *data = CF_DATA_CURRENT(cf);

  ಯದಿ(depth == 0) {
    ಯದಿ(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    ಯದಿ(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  ಯದಿ(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  ಯದಿ(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  ಯದಿ(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    ವರ್ಣ buf[128];
    mbedtls_x509_crt_verify_info(buf, ಪರಿಮಾಣ(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#ಅನ್ಯಥಾ
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned ಪೂರ್ಣಾಙ್ಕ)*flags);
#endif
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ CURLcode mbed_load_cacert(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಧ್ರುವ ಸಂರಚನಾ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ರಿಕ್ತ : conn_config->CAfile);
  ಧ್ರುವ bool verifypeer = conn_config->verifypeer;
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_cert_type = conn_config->cert_type;
#endif
  ಪೂರ್ಣಾಙ್ಕ ret = -1;
  ವರ್ಣ errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  ಯದಿ(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ಯದಿ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((ವರ್ಣ *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    ಅನ್ಯಥಾ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned ವರ್ಣ *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      ಯದಿ(!newblob)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#ಅನ್ಯಥಾ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    ಯದಿ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
    }
  }

  ಯದಿ(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    ಯದಿ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
    }
#ಅನ್ಯಥಾ
    failf(data, "mbedTLS: functions that use the file system not built in");
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  }

  ಯದಿ(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    ಯದಿ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);

      ಯದಿ(verifypeer)
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CACERT_BADFILE;
    }
#ಅನ್ಯಥಾ
    failf(data, "mbedTLS: functions that use the file system not built in");
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mbed_load_clicert(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ವರ್ಣ * ಧ್ರುವ ssl_cert = conn_config->clientcert;
  ಧ್ರುವ ಸಂರಚನಾ curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_cert_type = conn_config->cert_type;
#endif
  ಪೂರ್ಣಾಙ್ಕ ret = -1;
  ವರ್ಣ errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  ಯದಿ(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    ಯದಿ(ret) {
      mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);

      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
    }
#ಅನ್ಯಥಾ
    failf(data, "mbedTLS: functions that use the file system not built in");
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  }

  ಯದಿ(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ಯದಿ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((ವರ್ಣ *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    ಅನ್ಯಥಾ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned ವರ್ಣ *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      ಯದಿ(!newblob)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#ಅನ್ಯಥಾ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    ಯದಿ(ret) {
      mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mbed_load_privkey(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಪೂರ್ಣಾಙ್ಕ ret = -1;
  ವರ್ಣ errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  ಯದಿ(conn_config->ಕೀಲಿ || conn_config->ಕೀಲಿ_blob) {
    ಯದಿ(conn_config->ಕೀಲಿ) {
#ifdef MBEDTLS_FS_IO
#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ಕೀಲಿ,
                                     conn_config->ಕೀಲಿ_passwd);
      ಯದಿ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ಕೀಲಿ_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ಕೀಲಿ_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ಅನ್ಯಥಾ
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ಕೀಲಿ,
                                     conn_config->ಕೀಲಿ_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      ಯದಿ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      ಯದಿ(ret) {
        mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->ಕೀಲಿ, (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      }
#ಅನ್ಯಥಾ
      failf(data, "mbedTLS: functions that use the file system not built in");
      ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
    }
    ಅನ್ಯಥಾ {
      ಧ್ರುವ ಸಂರಚನಾ curl_blob *ssl_ಕೀಲಿ_blob = conn_config->ಕೀಲಿ_blob;
      ಧ್ರುವ ವರ್ಣ *passwd = conn_config->ಕೀಲಿ_passwd;
      ಧ್ರುವ bool is_der = conn_config->ಕೀಲಿ_type &&
                          curl_strequal(conn_config->ಕೀಲಿ_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned ವರ್ಣ *newblob = curlx_memdup0(ssl_ಕೀಲಿ_blob->data,
                                             ssl_ಕೀಲಿ_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_ಕೀಲಿ_blob->len : ssl_ಕೀಲಿ_blob->len + 1;
      ಯದಿ(!newblob)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ >= 0x04000000
      ret = mbedtls_pk_parse_ಕೀಲಿ(&backend->pk, newblob, keylen,
                                 (ಧ್ರುವ unsigned ವರ್ಣ *)passwd,
                                 passwd ? strlen(passwd) : 0);
      ಯದಿ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ಕೀಲಿ_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ಕೀಲಿ_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ಅನ್ಯಥಾ
      ret = mbedtls_pk_parse_ಕೀಲಿ(&backend->pk, newblob, keylen,
                                 (ಧ್ರುವ unsigned ವರ್ಣ *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      ಯದಿ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      ಯದಿ(ret) {
        mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);
        ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mbed_load_crl(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  ಯದಿ(ssl_crlfile) {
    ವರ್ಣ errorbuf[128];
#ifdef MBEDTLS_FS_IO
    ಪೂರ್ಣಾಙ್ಕ ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    ಯದಿ(ret) {
      mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);

      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CRL_BADFILE;
    }
#ಅನ್ಯಥಾ
    (ಶೂನ್ಯ)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
#endif
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)backend;
  ಯದಿ(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  }
#endif

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool mbed_apply_session(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ Curl_ssl_session *sc_session)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;

  ಯದಿ(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    ಪೂರ್ಣಾಙ್ಕ ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    ಯದಿ(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret);
    }
    ಅನ್ಯಥಾ {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      ಯದಿ(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret);
    }
    mbedtls_ssl_session_free(&session);
    ಯದಿ(!ret) {
      infof(data, "SSL reusing session ID");
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLcode mbed_configure_ssl(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಪೂರ್ಣಾಙ್ಕ ret;
  bool session_applied = FALSE;
  CURLcode result;
  ವರ್ಣ errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  ಯದಿ(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
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

#ಯದಿ defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_ಸಂಖ್ಯೆ >= 0x03060100 &&   \
  MBEDTLS_VERSION_ಸಂಖ್ಯೆ < 0x04000000
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
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  ಯದಿ(ret) {
    mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      ರಿಕ್ತ /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  ಯದಿ(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, ರಿಕ್ತ);
#ಅನ್ಯಥಾ
  ಯದಿ(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    ಯದಿ(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      ಪ್ರತ್ಯಾಗಮ result;
    }
  }
  ಅನ್ಯಥಾ {
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

  ಯದಿ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ಸಂರಚನಾ Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    ಯದಿ(scs) {
      ಯದಿ(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  ಯದಿ(!session_applied && Curl_ssl_scache_use(cf, data)) {
    ಸಂರಚನಾ Curl_ssl_session *sc_session = ರಿಕ್ತ;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_ಕೀಲಿ,
                                            &sc_session);
    ಯದಿ(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      ಯದಿ(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret);
      }
      ಅನ್ಯಥಾ {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        ಯದಿ(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret);
        ಅನ್ಯಥಾ
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_ಕೀಲಿ,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#ಅನ್ಯಥಾ
                            ರಿಕ್ತ
#endif
    );

  ಯದಿ(conn_config->ಕೀಲಿ || conn_config->ಕೀಲಿ_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  ಯದಿ(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  ಯದಿ(connssl->alpn) {
    ಸಂರಚನಾ alpn_proto_buf proto;
    size_t i;

    ಪ್ರತಿ(i = 0; i < connssl->alpn->ಎಣಿಕೆ; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    ಯದಿ(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  ಯದಿ(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    ಯದಿ(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mbed_connect_step1(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  ಯದಿ((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  ಯದಿ(!result)
    result = mbed_load_clicert(cf, data);
  ಯದಿ(!result)
    result = mbed_load_privkey(cf, data);
  ಯದಿ(!result)
    result = mbed_load_crl(cf, data);
  ಯದಿ(!result)
    result = mbed_configure_ssl(cf, data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  connssl->connecting_state = ssl_connect_2;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

#ಯದಿ defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

ಸ್ಥಿರ CURLcode mbed_connect_step2(ಸಂರಚನಾ Curl_cfilter *cf,
                                   ಸಂರಚನಾ Curl_easy *data)
{
#ಯದಿ defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  ಪೂರ್ಣಾಙ್ಕ ret;
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#ಅನ್ಯಥಾ
  ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  ಯದಿ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    ಪ್ರತ್ಯಾಗಮ CURLE_PEER_FAILED_VERIFICATION;
  }
  ಅನ್ಯಥಾ ಯದಿ(ret) {
    ವರ್ಣ errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_ಸಂಖ್ಯೆ(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, ಪರಿಮಾಣ(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret, errorbuf);
    ಪ್ರತ್ಯಾಗಮ CURLE_SSL_CONNECT_ERROR;
  }

  {
    ವರ್ಣ cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, ಪರಿಮಾಣ(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  ಯದಿ(pinnedpubkey) {
    ಪೂರ್ಣಾಙ್ಕ size;
    ಧ್ರುವ mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = ರಿಕ್ತ;
    unsigned ವರ್ಣ *pubkey = ರಿಕ್ತ;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    ಯದಿ(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      ಪ್ರತ್ಯಾಗಮ CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, ಪರಿಮಾಣ(*p));

    ಯದಿ(!p)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    ಯದಿ(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    ಯದಿ(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      ಗಚ್ಛ pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    ಯದಿ(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      ಗಚ್ಛ pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  ಯದಿ(connssl->alpn) {
    ಧ್ರುವ ವರ್ಣ *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ಧ್ರುವ unsigned ವರ್ಣ *)proto,
                                      proto ? strlen(proto) : 0);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mbed_new_session(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  ಸಂರಚನಾ Curl_ssl_session *sc_session = ರಿಕ್ತ;
  unsigned ವರ್ಣ *sdata = ರಿಕ್ತ;
  size_t slen = 0;
  ಪೂರ್ಣಾಙ್ಕ ietf_tls_id;
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ ret;

  DEBUGASSERT(backend);
  ಯದಿ(!Curl_ssl_scache_use(cf, data))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  ಯದಿ(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    ಗಚ್ಛ out;
  }

  mbedtls_ssl_session_save(&session, ರಿಕ್ತ, 0, &slen);
  ಯದಿ(!slen) {
    failf(data, "failed to serialize session: length is 0");
    ಗಚ್ಛ out;
  }

  sdata = curlx_malloc(slen);
  ಯದಿ(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  ಯದಿ(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret);
    ಗಚ್ಛ out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_ಸಂಖ್ಯೆ(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = ರಿಕ್ತ;  /* call took ownership */
  ಯದಿ(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  ಯದಿ(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_ಕೀಲಿ,
                                 sc_session);
    sc_session = ರಿಕ್ತ;
  }

out:
  ಯದಿ(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mbed_send(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                          ಧ್ರುವ ಶೂನ್ಯ *mem, size_t len, size_t *pnwritten)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ nwritten;

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
  ಯದಿ(backend->send_blocked) {
    ಯದಿ(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ಗಚ್ಛ out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ಧ್ರುವ unsigned ವರ್ಣ *)mem, len);

  ಯದಿ(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned ಪೂರ್ಣಾಙ್ಕ)-nwritten);
    ವಿಕಲ್ಪ(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ಪಕ್ಷ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      ಯದಿ(!result)
        ಗಚ್ಛ do_send;
      ಭಙ್ಗ;
#endif
    ಪಕ್ಷ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      ಭಙ್ಗ;
    ಪಕ್ಷ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      result = CURLE_SEND_ERROR;
      ಭಙ್ಗ;
    }
    ಯದಿ((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (ಪೂರ್ಣಾಙ್ಕ)result, *pnwritten);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode mbedtls_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  unsigned ವರ್ಣ buf[1024];
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  ಯದಿ(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  ಯದಿ(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    ಯದಿ(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      ವಿಕಲ್ಪ(ret) {
      ಪಕ್ಷ 0: /* we sent it, receive from the server */
        ಭಙ್ಗ;
      ಪಕ್ಷ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        ಗಚ್ಛ out;
      ಪಕ್ಷ MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        ಗಚ್ಛ out;
      ಪಕ್ಷ MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ಗಚ್ಛ out;
      ಸ್ವಭಾವ:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret);
        result = CURLE_RECV_ERROR;
        ಗಚ್ಛ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ಪ್ರತಿ(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, ಪರಿಮಾಣ(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    ಯದಿ(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, ಪರಿಮಾಣ(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ಯದಿ(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      ಅನುವರ್ತ;
#endif
    ಯದಿ(ret <= 0)
      ಭಙ್ಗ;
  }

  ಯದಿ(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  ಅನ್ಯಥಾ ಯದಿ(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  ಅನ್ಯಥಾ ಯದಿ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned ಪೂರ್ಣಾಙ್ಕ)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ mbedtls_close(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  ಯದಿ(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

ಸ್ಥಿರ CURLcode mbed_recv(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                          ವರ್ಣ *buf, size_t buffersize, size_t *pnread)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ಪೂರ್ಣಾಙ್ಕ nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned ವರ್ಣ *)buf, buffersize);
  ಯದಿ(nread > 0)
    *pnread = (size_t)nread;
  ಅನ್ಯಥಾ {
    ವರ್ಣ errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned ಪೂರ್ಣಾಙ್ಕ)-nread);
    ವಿಕಲ್ಪ(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    ಪಕ್ಷ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      ಯದಿ(!result)
        ಗಚ್ಛ do_read;
      ಭಙ್ಗ;
#endif
    ಪಕ್ಷ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      ಭಙ್ಗ;
    ಪಕ್ಷ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      ಭಙ್ಗ;
    ಪಕ್ಷ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      mbedtls_strerror(nread, errorbuf, ಪರಿಮಾಣ(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned ಪೂರ್ಣಾಙ್ಕ)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ size_t mbedtls_version(ವರ್ಣ *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned ಪೂರ್ಣಾಙ್ಕ version = mbedtls_version_get_ಸಂಖ್ಯೆ();
  ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
ಸ್ಥಿರ CURLcode mbedtls_random(ಸಂರಚನಾ Curl_easy *data,
                               unsigned ವರ್ಣ *entropy, size_t length)
{
  psa_status_t status;
  (ಶೂನ್ಯ)data;

  status = psa_generate_random(entropy, length);

  ಪ್ರತ್ಯಾಗಮ status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ಸ್ಥಿರ CURLcode mbedtls_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  ಯದಿ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ಯದಿ(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    ಸಂರಚನಾ ssl_connect_data *ctx = cf->ctx;
    ಸಂರಚನಾ mbed_ssl_backend_data *backend =
      (ಸಂರಚನಾ mbed_ssl_backend_data *)ctx->backend;

    ಯದಿ(mbedtls_ssl_get_version_ಸಂಖ್ಯೆ(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  ಯದಿ(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mbedtls_init(ಶೂನ್ಯ)
{
#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ < 0x04000000
  ಪೂರ್ಣಾಙ್ಕ ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  ಯದಿ(status != PSA_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ 0;

#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              ರಿಕ್ತ, 0);

  ಯದಿ(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    ಪ್ರತ್ಯಾಗಮ 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ mbedtls_cleanup(ಶೂನ್ಯ)
{
  mbedtls_psa_crypto_free();

#ಯದಿ MBEDTLS_VERSION_ಸಂಖ್ಯೆ < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

ಸ್ಥಿರ bool mbedtls_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *ctx = cf->ctx;
  ಸಂರಚನಾ mbed_ssl_backend_data *backend;

  (ಶೂನ್ಯ)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ಸಂರಚನಾ mbed_ssl_backend_data *)ctx->backend;
  ಪ್ರತ್ಯಾಗಮ mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

ಸ್ಥಿರ CURLcode mbedtls_sha256sum(ಧ್ರುವ unsigned ವರ್ಣ *ಒಳಹರಿವು,
                                  size_t inputlen,
                                  unsigned ವರ್ಣ *sha256sum,
                                  size_t sha256len)
{
#ಯದಿ defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, ಒಳಹರಿವು, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  ಯದಿ(status != PSA_SUCCESS)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)sha256len;
  ಪ್ರತ್ಯಾಗಮ Curl_sha256it(sha256sum, ಒಳಹರಿವು, inputlen);
#endif
}

ಸ್ಥಿರ ಶೂನ್ಯ *mbedtls_get_internals(ಸಂರಚನಾ ssl_connect_data *connssl,
                                   CURLINFO info)
{
  ಸಂರಚನಾ mbed_ssl_backend_data *backend =
    (ಸಂರಚನಾ mbed_ssl_backend_data *)connssl->backend;
  (ಶೂನ್ಯ)info;
  DEBUGASSERT(backend);
  ಪ್ರತ್ಯಾಗಮ &backend->ssl;
}

ಧ್ರುವ ಸಂರಚನಾ Curl_ssl Curl_ssl_mbedtls = {
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

  ಪರಿಮಾಣ(ಸಂರಚನಾ mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  ರಿಕ್ತ,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  ರಿಕ್ತ,                             /* close_all */
  ರಿಕ್ತ,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  ರಿಕ್ತ,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
