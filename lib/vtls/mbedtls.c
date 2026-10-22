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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#ಅನ್ತರ್ಭಾವ <mbedtls/version.h>
#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x03020000
#ದೋಷ "mbedTLS 3.2.0 or greater required"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <psa/crypto_config.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/net_sockets.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/ssl.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/x509.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/psa_util.h>

#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#ದೋಷ "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <mbedtls/error.h>
#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x04000000
#ಅನ್ತರ್ಭಾವ <mbedtls/entropy.h>
#ಅನ್ತರ್ಭಾವ <mbedtls/ctr_drbg.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_DEBUG
#ಅನ್ತರ್ಭಾವ <mbedtls/debug.h>
#ಆದರೆ_ಅಂತ್ಯ

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
#ಆದರೆ defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#ವ್ಯಾಖ್ಯಾನಿಸು HAS_ALPN_MBEDTLS
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#ಆದರೆ_ಅಂತ್ಯ
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ALPN_MBEDTLS
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *protocols[3];
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ *ciphersuites;
  size_t send_blocked_len;
  ಬಿಟ್(initialized); /* mbedtls_ssl_context is initialized */
  ಬಿಟ್(ಕಳುಹಿಸಿದ_shutdown);
  ಬಿಟ್(send_blocked);
};

/** A context for random number generation (RNG).
 */
#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x04000000
ರಚನೆ rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ಸ್ಥಿರ ರಚನೆ rng_context_t rng;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MBEDTLS_ERROR_C
#ವ್ಯಾಖ್ಯಾನಿಸು mbedtls_strerror(a, b, c) b[0] = 0
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_DEBUG
ಸ್ಥಿರ ಶೂನ್ಯ mbed_debug(ಶೂನ್ಯ *context, ಪೂರ್ಣಾಂಕ level, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f_name,
                       ಪೂರ್ಣಾಂಕ ಸಾಲು_nb, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = (ರಚನೆ Curl_easy *)context;
  (ಶೂನ್ಯ)level;
  (ಶೂನ್ಯ)ಸಾಲು_nb;
  (ಶೂನ್ಯ)f_name;

  ಆದರೆ(ದತ್ತಾಂಶ) {
    size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಾಲು);
    ಆದರೆ(len && (ಸಾಲು[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(ದತ್ತಾಂಶ, "%.*s", (ಪೂರ್ಣಾಂಕ)len, ಸಾಲು);
  }
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mbedtls_bio_cf_write(ಶೂನ್ಯ *bio,
                                ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t blen)
{
  ರಚನೆ Curl_cfilter *cf = bio;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ 0;

  ಫಲಿತಾಂಶ = Curl_conn_cf_send(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
    ಹಿಂತಿರುಗಿ MBEDTLS_ERR_SSL_WANT_WRITE;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? -1 : (ಪೂರ್ಣಾಂಕ)nwritten;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mbedtls_bio_cf_read(ಶೂನ್ಯ *bio, ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *buf, size_t blen)
{
  ರಚನೆ Curl_cfilter *cf = bio;
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);
  size_t nread = 0;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(!ದತ್ತಾಂಶ)
    ಹಿಂತಿರುಗಿ 0;
  /* OpenSSL catches this case, so should we. */
  ಆದರೆ(!buf)
    ಹಿಂತಿರುಗಿ 0;

  ಫಲಿತಾಂಶ = Curl_conn_cf_recv(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, (ಅಕ್ಷರ *)buf, blen, &nread);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, nread);
  ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
    ಹಿಂತಿರುಗಿ MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? -1 : (ಪೂರ್ಣಾಂಕ)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#ವ್ಯಾಖ್ಯಾನಿಸು RSA_PUB_DER_MAX_ಬೈಟುಗಳು   (38 + (2 * MBEDTLS_MPI_MAX_ಗಾತ್ರ))
#ವ್ಯಾಖ್ಯಾನಿಸು ECP_PUB_DER_MAX_ಬೈಟುಗಳು   (30 + (2 * MBEDTLS_ECP_MAX_ಬೈಟುಗಳು))

#ವ್ಯಾಖ್ಯಾನಿಸು PUB_DER_MAX_ಬೈಟುಗಳು   (RSA_PUB_DER_MAX_ಬೈಟುಗಳು > ECP_PUB_DER_MAX_ಬೈಟುಗಳು ? \
                             RSA_PUB_DER_MAX_ಬೈಟುಗಳು : ECP_PUB_DER_MAX_ಬೈಟುಗಳು)

ಸ್ಥಿರ CURLcode mbed_ಗಣ_ssl_ಆವೃತ್ತಿ_min_max(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend,
  ರಚನೆ ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_ಆವೃತ್ತಿ ver_min =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_ಆವೃತ್ತಿ_TLS1_2
#ಇಲ್ಲದಿದ್ದರೆ
    MBEDTLS_SSL_ಆವೃತ್ತಿ_TLS1_3
#ಆದರೆ_ಅಂತ್ಯ
    ;
  mbedtls_ssl_protocol_ಆವೃತ್ತಿ ver_max =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_ಆವೃತ್ತಿ_TLS1_3
#ಇಲ್ಲದಿದ್ದರೆ
    MBEDTLS_SSL_ಆವೃತ್ತಿ_TLS1_2
#ಆದರೆ_ಅಂತ್ಯ
    ;

  DEBUGASSERT(conn_config->ಆವೃತ್ತಿ_2 != CURL_SSLVERSION_DEFAULT);
  ಆಯ್ಕೆ(conn_config->ಆವೃತ್ತಿ_2) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_0:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_1:
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_2:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_ಆವೃತ್ತಿ_TLS1_2;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURL_SSLVERSION_TLSv1_3:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_ಆವೃತ್ತಿ_TLS1_3;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->ಆವೃತ್ತಿ_2);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  ಆಯ್ಕೆ(conn_config->ಆವೃತ್ತಿ_max) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_DEFAULT:
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_NONE:
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_3:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_ಆವೃತ್ತಿ_TLS1_3;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_2
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_ಆವೃತ್ತಿ_TLS1_2;
    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_1:
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_TLSv1_0:
  ಪೂರ್ವನಿಯೋಜಿತ:
    failf(ದತ್ತಾಂಶ, "mbedTLS: unsupported maximum TLS version value");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_ಆವೃತ್ತಿ(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_ಆವೃತ್ತಿ(&backend->config, ver_max);

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mbed_cipher_suite_get_str(uint16_t id, ಅಕ್ಷರ *buf, size_t buf_ಗಾತ್ರ,
                                     bool prefer_rfc)
{
  ಆದರೆ(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_ಗಾತ್ರ, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ Curl_cipher_suite_get_str(id, buf, buf_ಗಾತ್ರ, prefer_rfc);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ uint16_t mbed_cipher_suite_walk_str(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **str, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಕೊನೆ)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, ಕೊನೆ);
  size_t len = *ಕೊನೆ - *str;
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  ಆದರೆ(!id) {
    ಆದರೆ((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  ಹಿಂತಿರುಗಿ id;
}
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#ವ್ಯಾಖ್ಯಾನಿಸು mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode mbed_ಗಣ_selected_ciphers(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend,
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ciphers12,
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ciphers13)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ciphers = ciphers12;
  ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ *supported;
  ಪೂರ್ಣಾಂಕ *selected;
  size_t supported_len, ಎಣಿಕೆ_2 = 0, default13_ಎಣಿಕೆ = 0, i, j;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, *ಕೊನೆ;

  supported = mbedtls_ssl_list_ciphersuites();
  ಗಾಗಿ(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * (supported_len + 1));
  ಆದರೆ(!selected)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MBEDTLS_SSL_PROTO_TLS1_3
  (ಶೂನ್ಯ)ciphers13, (ಶೂನ್ಯ)j;
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    ಗಾಗಿ(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ಮುಂದುವರಿ;

      selected[ಎಣಿಕೆ_2++] = id;
    }

    default13_ಎಣಿಕೆ = ಎಣಿಕೆ_2;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ciphers = ciphers13;

ಕೂಡಿಸು_ciphers:
#ಆದರೆ_ಅಂತ್ಯ
  ಗಾಗಿ(ptr = ciphers; ptr[0] != '\0' && ಎಣಿಕೆ_2 < supported_len; ptr = ಕೊನೆ) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &ಕೊನೆ);

    /* Check if cipher is supported */
    ಆದರೆ(id) {
      ಗಾಗಿ(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      ಆದರೆ(i == supported_len)
        id = 0;
    }
    ಆದರೆ(!id) {
      ಆದರೆ(ptr[0] != '\0')
        infof(ದತ್ತಾಂಶ, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - ptr), ptr);
      ಮುಂದುವರಿ;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    ಗಾಗಿ(i = 0; i < ಎಣಿಕೆ_2 && selected[i] != id; i++)
      ;
    ಆದರೆ(i < ಎಣಿಕೆ_2) {
      ಆದರೆ(i >= default13_ಎಣಿಕೆ)
        infof(ದತ್ತಾಂಶ, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (ಪೂರ್ಣಾಂಕ)(ಕೊನೆ - ptr), ptr);
      ಮುಂದುವರಿ;
    }

    selected[ಎಣಿಕೆ_2++] = id;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_3
  ಆದರೆ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    ಹೋಗು ಕೂಡಿಸು_ciphers;
  }

  ಆದರೆ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    ಗಾಗಿ(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ಮುಂದುವರಿ;

      /* No duplicates allowed (so selected cannot overflow) */
      ಗಾಗಿ(i = 0; i < ಎಣಿಕೆ_2 && selected[i] != id; i++)
        ;
      ಆದರೆ(i < ಎಣಿಕೆ_2)
        ಮುಂದುವರಿ;

      selected[ಎಣಿಕೆ_2++] = id;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  selected[ಎಣಿಕೆ_2] = 0;

  ಆದರೆ(ಎಣಿಕೆ_2 == 0) {
    curlx_free(selected);
    failf(ದತ್ತಾಂಶ, "mbedTLS: no supported cipher in list");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ ಶೂನ್ಯ mbed_dump_cert_info(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಸ್ಥಿರಾಂಕ mbedtls_x509_crt *crt)
{
#ಆದರೆ !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ, (ಶೂನ್ಯ)crt;
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ size_t bufsize = 16384;
  ಅಕ್ಷರ *p, *ಬಫರ್ = curlx_malloc(bufsize);

  ಆದರೆ(ಬಫರ್ && mbedtls_x509_crt_info(ಬಫರ್, bufsize, " ", crt) > 0) {
    infof(ದತ್ತಾಂಶ, "Server certificate:");
    ಗಾಗಿ(p = ಬಫರ್; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(ದತ್ತಾಂಶ, "%.*s", (ಪೂರ್ಣಾಂಕ)s, p);
      p += s;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ
    infof(ದತ್ತಾಂಶ, "Unable to dump certificate information");

  curlx_free(ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ mbed_ಹೊರತೆಗೆ_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ಸ್ಥಿರಾಂಕ mbedtls_x509_crt *crt)
{
  CURLcode ಫಲಿತಾಂಶ;
  ಸ್ಥಿರಾಂಕ mbedtls_x509_crt *cur;
  ಪೂರ್ಣಾಂಕ cert_ಎಣಿಕೆ = 0;
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ(cur = crt; cur && cert_ಎಣಿಕೆ <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->ಮುಂದಿನ)
    cert_ಎಣಿಕೆ++;

  ಆದರೆ(cert_ಎಣಿಕೆ > MAX_ALLOWED_CERT_AMOUNT) {
    infof(ದತ್ತಾಂಶ, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    ಹಿಂತಿರುಗಿ;
  }

  ಫಲಿತಾಂಶ = Curl_ssl_init_certinfo(ದತ್ತಾಂಶ, cert_ಎಣಿಕೆ);

  ಗಾಗಿ(i = 0, cur = crt; ಫಲಿತಾಂಶ == CURLE_OK && cur; ++i, cur = cur->ಮುಂದಿನ) {
    ಫಲಿತಾಂಶ = Curl_ಹೊರತೆಗೆ_certinfo(ದತ್ತಾಂಶ, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mbed_verify_cb(ಶೂನ್ಯ *ptr, mbedtls_x509_crt *crt,
                          ಪೂರ್ಣಾಂಕ ಆಳ, uint32_t *flags)
{
  ರಚನೆ Curl_cfilter *cf = (ರಚನೆ Curl_cfilter *)ptr;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = CF_ದತ್ತಾಂಶ_CURRENT(cf);

  ಆದರೆ(ಆಳ == 0) {
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.verbose)
      mbed_dump_cert_info(ದತ್ತಾಂಶ, crt);
    ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl.certinfo)
      mbed_ಹೊರತೆಗೆ_certinfo(ದತ್ತಾಂಶ, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  ಆದರೆ(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  ಆದರೆ(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  ಆದರೆ(*flags) {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MBEDTLS_X509_REMOVE_INFO
    ಅಕ್ಷರ buf[128];
    mbedtls_x509_crt_verify_info(buf, ಗಾತ್ರ(buf), "", *flags);
    failf(ದತ್ತಾಂಶ, "mbedTLS: %s", buf);
#ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "mbedTLS: certificate verification error 0x%08x",
          (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)*flags);
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ CURLcode mbed_ತುಂಬು_cacert(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ಖಾಲಿ : conn_config->CAfile);
  ಸ್ಥಿರಾಂಕ bool verifypeer = conn_config->verifypeer;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_capath = conn_config->CApath;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_PEM_PARSE_C
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_cert_type = conn_config->cert_type;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ ret = -1;
  ಅಕ್ಷರ errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  ಆದರೆ(ca_info_blob && verifypeer) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ಆದರೆ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((ಅಕ್ಷರ *)(ca_info_blob->ದತ್ತಾಂಶ))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->ದತ್ತಾಂಶ,
                                   ca_info_blob->len);
    }
    ಇಲ್ಲದಿದ್ದರೆ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *newblob = curlx_memdup0(ca_info_blob->ದತ್ತಾಂಶ,
                                             ca_info_blob->len);
      ಆದರೆ(!newblob)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->ದತ್ತಾಂಶ,
                                     ca_info_blob->len);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
      failf(ದತ್ತಾಂಶ, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
    }
  }

  ಆದರೆ(ssl_cafile && verifypeer) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    ಆದರೆ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
      failf(ದತ್ತಾಂಶ, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "mbedTLS: functions that use the file system not built in");
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆದರೆ(ssl_capath) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_ಮಾರ್ಗ(&backend->cacert, ssl_capath);

    ಆದರೆ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
      failf(ದತ್ತಾಂಶ, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);

      ಆದರೆ(verifypeer)
        ಹಿಂತಿರುಗಿ CURLE_SSL_CACERT_BADFILE;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "mbedTLS: functions that use the file system not built in");
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mbed_ತುಂಬು_clicert(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_cert = conn_config->clientcert;
  ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_PEM_PARSE_C
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_cert_type = conn_config->cert_type;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ ret = -1;
  ಅಕ್ಷರ errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  ಆದರೆ(ssl_cert) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    ಆದರೆ(ret) {
      mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
      failf(ದತ್ತಾಂಶ, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);

      ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    failf(ದತ್ತಾಂಶ, "mbedTLS: functions that use the file system not built in");
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  }

  ಆದರೆ(ssl_cert_blob) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ಆದರೆ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((ಅಕ್ಷರ *)(ssl_cert_blob->ದತ್ತಾಂಶ))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->ದತ್ತಾಂಶ,
                                   ssl_cert_blob->len);
    }
    ಇಲ್ಲದಿದ್ದರೆ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *newblob = curlx_memdup0(ssl_cert_blob->ದತ್ತಾಂಶ,
                                             ssl_cert_blob->len);
      ಆದರೆ(!newblob)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->ದತ್ತಾಂಶ,
                                     ssl_cert_blob->len);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(ret) {
      mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
      failf(ದತ್ತಾಂಶ, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);
      ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mbed_ತುಂಬು_privkey(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಪೂರ್ಣಾಂಕ ret = -1;
  ಅಕ್ಷರ errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  ಆದರೆ(conn_config->ಕೀಲಿ || conn_config->ಕೀಲಿ_blob) {
    ಆದರೆ(conn_config->ಕೀಲಿ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_FS_IO
#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ಕೀಲಿ,
                                     conn_config->ಕೀಲಿ_passwd);
      ಆದರೆ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ಕೀಲಿ_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ಕೀಲಿ_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ಇಲ್ಲದಿದ್ದರೆ
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ಕೀಲಿ,
                                     conn_config->ಕೀಲಿ_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      ಆದರೆ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ಆದರೆ_ಅಂತ್ಯ

      ಆದರೆ(ret) {
        mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
        failf(ದತ್ತಾಂಶ, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->ಕೀಲಿ, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }
#ಇಲ್ಲದಿದ್ದರೆ
      failf(ದತ್ತಾಂಶ, "mbedTLS: functions that use the file system not built in");
      ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರಾಂಕ ರಚನೆ curl_blob *ssl_ಕೀಲಿ_blob = conn_config->ಕೀಲಿ_blob;
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *passwd = conn_config->ಕೀಲಿ_passwd;
      ಸ್ಥಿರಾಂಕ bool is_der = conn_config->ಕೀಲಿ_type &&
                          curl_strequal(conn_config->ಕೀಲಿ_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *newblob = curlx_memdup0(ssl_ಕೀಲಿ_blob->ದತ್ತಾಂಶ,
                                             ssl_ಕೀಲಿ_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_ಕೀಲಿ_blob->len : ssl_ಕೀಲಿ_blob->len + 1;
      ಆದರೆ(!newblob)
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x04000000
      ret = mbedtls_pk_parse_ಕೀಲಿ(&backend->pk, newblob, keylen,
                                 (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)passwd,
                                 passwd ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(passwd) : 0);
      ಆದರೆ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ಕೀಲಿ_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ಕೀಲಿ_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ಇಲ್ಲದಿದ್ದರೆ
      ret = mbedtls_pk_parse_ಕೀಲಿ(&backend->pk, newblob, keylen,
                                 (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)passwd,
                                 passwd ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      ಆದರೆ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ಆದರೆ_ಅಂತ್ಯ
      curlx_free(newblob);

      ಆದರೆ(ret) {
        mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
        failf(ದತ್ತಾಂಶ, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);
        ಹಿಂತಿರುಗಿ CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mbed_ತುಂಬು_crl(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ ssl_crlfile = conn_config->CRLfile;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  ಆದರೆ(ssl_crlfile) {
    ಅಕ್ಷರ errorbuf[128];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_FS_IO
    ಪೂರ್ಣಾಂಕ ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    ಆದರೆ(ret) {
      mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
      failf(ದತ್ತಾಂಶ, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);

      ಹಿಂತಿರುಗಿ CURLE_SSL_CRL_BADFILE;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    (ಶೂನ್ಯ)errorbuf;
    failf(ದತ್ತಾಂಶ, "mbedTLS: functions that use the file system not built in");
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
#ಆದರೆ_ಅಂತ್ಯ
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)backend;
  ಆದರೆ(ssl_crlfile) {
    failf(ದತ್ತಾಂಶ, "mbedTLS: CRL support not built in");
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool mbed_apply_session(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ Curl_ssl_session *sc_session)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;

  ಆದರೆ(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    ಪೂರ್ಣಾಂಕ ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_ತುಂಬು(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    ಆದರೆ(ret) {
      failf(ದತ್ತಾಂಶ, "SSL session error loading: -0x%x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ret = mbedtls_ssl_ಗಣ_session(&backend->ssl, &session);
      ಆದರೆ(ret)
        failf(ದತ್ತಾಂಶ, "SSL session error setting: -0x%x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret);
    }
    mbedtls_ssl_session_free(&session);
    ಆದರೆ(!ret) {
      infof(ದತ್ತಾಂಶ, "SSL reusing session ID");
      ಹಿಂತಿರುಗಿ TRUE;
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLcode mbed_configure_ssl(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಪೂರ್ಣಾಂಕ ret;
  bool session_applied = FALSE;
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ errorbuf[128];

  infof(ದತ್ತಾಂಶ, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->ಪೋರ್ಟ್);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_ಹರಿವು,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  ಆದರೆ(ret) {
    failf(ದತ್ತಾಂಶ, "mbedTLS: ssl_config failed");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, ದತ್ತಾಂಶ);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_ಗಣ_ಮಿತಿ(4);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ >= 0x03060100 &&   \
  MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#ಆದರೆ_ಅಂತ್ಯ

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_ಮುಂದಿನ);

  ಫಲಿತಾಂಶ = mbed_ಗಣ_ssl_ಆವೃತ್ತಿ_min_max(ದತ್ತಾಂಶ, backend, conn_config);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#ಆದರೆ_ಅಂತ್ಯ

  ret = mbedtls_ssl_ಸಿದ್ಧತೆ(&backend->ssl,
                          &backend->config);
  ಆದರೆ(ret) {
    mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
    failf(ದತ್ತಾಂಶ, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_ಗಣ_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      ಖಾಲಿ /*  rev_timeout() */);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MBEDTLS_SSL_PROTO_TLS1_3
  ಆದರೆ(conn_config->cipher_list) {
    ಫಲಿತಾಂಶ = mbed_ಗಣ_selected_ciphers(ದತ್ತಾಂಶ, backend,
                                       conn_config->cipher_list, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
  ಆದರೆ(conn_config->cipher_list || conn_config->cipher_list13) {
    ಫಲಿತಾಂಶ = mbed_ಗಣ_selected_ciphers(ದತ್ತಾಂಶ, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "mbedTLS: failed to set cipher suites");
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ರಚನೆ Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(ದತ್ತಾಂಶ, cf->cft, FIRSTSOCKET);
    ಆದರೆ(scs) {
      ಆದರೆ(mbed_apply_session(cf, ದತ್ತಾಂಶ, scs)) {
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  ಆದರೆ(!session_applied && Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ)) {
    ರಚನೆ Curl_ssl_session *sc_session = ಖಾಲಿ;
    CURLcode sresult = Curl_ssl_scache_take(cf, ದತ್ತಾಂಶ, connssl->peer.scache_ಕೀಲಿ,
                                            &sc_session);
    ಆದರೆ(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_ತುಂಬು(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      ಆದರೆ(ret) {
        failf(ದತ್ತಾಂಶ, "SSL session error loading: -0x%x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ret = mbedtls_ssl_ಗಣ_session(&backend->ssl, &session);
        ಆದರೆ(ret)
          failf(ದತ್ತಾಂಶ, "SSL session error setting: -0x%x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret);
        ಇಲ್ಲದಿದ್ದರೆ
          infof(ದತ್ತಾಂಶ, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, ದತ್ತಾಂಶ, connssl->peer.scache_ಕೀಲಿ,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#ಇಲ್ಲದಿದ್ದರೆ
                            ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
    );

  ಆದರೆ(conn_config->ಕೀಲಿ || conn_config->ಕೀಲಿ_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  ಆದರೆ(mbedtls_ssl_ಗಣ_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(ದತ್ತಾಂಶ, "Failed to set SNI");
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ALPN_MBEDTLS
  ಆದರೆ(connssl->alpn) {
    ರಚನೆ alpn_proto_buf proto;
    size_t i;

    ಗಾಗಿ(i = 0; i < connssl->alpn->ಎಣಿಕೆ_2; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    ಆದರೆ(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(ದತ್ತಾಂಶ, "Failed setting ALPN protocols");
      ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_ಗಮ್ಯ_proto_str(&proto, connssl->alpn);
    infof(ದತ್ತಾಂಶ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ದತ್ತಾಂಶ);
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* give application a chance to interfere with mbedTLS set up. */
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctx) {
    ಫಲಿತಾಂಶ = (*ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctx)(ದತ್ತಾಂಶ, &backend->config,
                                      ದತ್ತಾಂಶ->ಗಣ.ssl_fsslctxp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      failf(ದತ್ತಾಂಶ, "error signaled by SSL ctx callback");
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mbed_connect_step1(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ((conn_config->ಆವೃತ್ತಿ_2 == CURL_SSLVERSION_SSLv2) ||
     (conn_config->ಆವೃತ್ತಿ_2 == CURL_SSLVERSION_SSLv3)) {
    failf(ದತ್ತಾಂಶ, "Not supported SSL version");
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
  }

  ಫಲಿತಾಂಶ = mbed_ತುಂಬು_cacert(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = mbed_ತುಂಬು_clicert(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = mbed_ತುಂಬು_privkey(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = mbed_ತುಂಬು_crl(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = mbed_configure_ssl(cf, ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  connssl->connecting_state = ssl_connect_2;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಆದರೆ defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_PINNED_PUBKEY
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode mbed_connect_step2(ರಚನೆ Curl_cfilter *cf,
                                   ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
#ಆದರೆ defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode ಫಲಿತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
  ಪೂರ್ಣಾಂಕ ret;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PINNED_PUBKEY
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY);
#ಇಲ್ಲದಿದ್ದರೆ
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ pinnedpubkey =
    CURL_EASY_STR(ದತ್ತಾಂಶ, STRING_SSL_PINNEDPUBLICKEY);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  ಆದರೆ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(ದತ್ತಾಂಶ, "peer certificate could not be verified");
    ಹಿಂತಿರುಗಿ CURLE_PEER_FAILED_VERIFICATION;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ret) {
    ಅಕ್ಷರ errorbuf[128];
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "TLS version %04X",
                mbedtls_ssl_get_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, ಗಾತ್ರ(errorbuf));
    failf(ದತ್ತಾಂಶ, "ssl_handshake returned: (-0x%04X) %s",
          (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret, errorbuf);
    ಹಿಂತಿರುಗಿ CURLE_SSL_CONNECT_ERROR;
  }

  {
    ಅಕ್ಷರ cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, ಗಾತ್ರ(cipher_str), TRUE);
    infof(ದತ್ತಾಂಶ, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_ಆವೃತ್ತಿ(&backend->ssl), cipher_str);
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PINNED_PUBKEY
  ಆದರೆ(pinnedpubkey) {
    ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2;
    ಸ್ಥಿರಾಂಕ mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = ಖಾಲಿ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pubkey = ಖಾಲಿ;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    ಆದರೆ(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(ದತ್ತಾಂಶ, "Failed due to missing peer certificate");
      ಹಿಂತಿರುಗಿ CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, ಗಾತ್ರ(*p));

    ಆದರೆ(!p)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

    pubkey = curlx_malloc(PUB_DER_MAX_ಬೈಟುಗಳು);

    ಆದರೆ(!pubkey) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    ಆದರೆ(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(ದತ್ತಾಂಶ, "Failed copying peer certificate");
      ಫಲಿತಾಂಶ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      ಹೋಗು pinnedpubkey_error;
    }

    ಗಾತ್ರ_2 = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_ಬೈಟುಗಳು);

    ಆದರೆ(ಗಾತ್ರ_2 <= 0) {
      failf(ದತ್ತಾಂಶ, "Failed copying public key from peer certificate");
      ಫಲಿತಾಂಶ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      ಹೋಗು pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    ಫಲಿತಾಂಶ = Curl_pin_peer_pubkey(ದತ್ತಾಂಶ,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_ಬೈಟುಗಳು - ಗಾತ್ರ_2], ಗಾತ್ರ_2);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ALPN_MBEDTLS
  ಆದರೆ(connssl->alpn) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    ಫಲಿತಾಂಶ = Curl_alpn_ಗಣ_negotiated(cf, ದತ್ತಾಂಶ, connssl,
                                      (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)proto,
                                      proto ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(proto) : 0);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
#ಆದರೆ_ಅಂತ್ಯ

  connssl->connecting_state = ssl_connect_3;
  infof(ದತ್ತಾಂಶ, "SSL connected");

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode mbed_new_session(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  ರಚನೆ Curl_ssl_session *sc_session = ಖಾಲಿ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sdata = ಖಾಲಿ;
  size_t slen = 0;
  ಪೂರ್ಣಾಂಕ ietf_tls_id;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ ret;

  DEBUGASSERT(backend);
  ಆದರೆ(!Curl_ssl_scache_use(cf, ದತ್ತಾಂಶ))
    ಹಿಂತಿರುಗಿ CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  ಆದರೆ(ret) {
    failf(ದತ್ತಾಂಶ, "mbedtls_ssl_get_session returned -0x%x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret);
    ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  mbedtls_ssl_session_save(&session, ಖಾಲಿ, 0, &slen);
  ಆದರೆ(!slen) {
    failf(ದತ್ತಾಂಶ, "failed to serialize session: length is 0");
    ಹೋಗು ಹೊರಹೋಗು;
  }

  sdata = curlx_malloc(slen);
  ಆದರೆ(!sdata) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  ಆದರೆ(ret) {
    failf(ದತ್ತಾಂಶ, "failed to serialize session: -0x%x", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ietf_tls_id = mbedtls_ssl_get_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ(&backend->ssl);
  ಫಲಿತಾಂಶ = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = ಖಾಲಿ;  /* call took ownership */
  ಆದರೆ(!ಫಲಿತಾಂಶ && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    ಫಲಿತಾಂಶ = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = Curl_ssl_scache_put(cf, ದತ್ತಾಂಶ, connssl->peer.scache_ಕೀಲಿ,
                                 sc_session);
    sc_session = ಖಾಲಿ;
  }

ಹೊರಹೋಗು:
  ಆದರೆ(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mbed_send(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *mem, size_t len, size_t *pnwritten)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ nwritten;

  DEBUGASSERT(backend);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#ಆದರೆ_ಅಂತ್ಯ
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  ಆದರೆ(backend->send_blocked) {
    ಆದರೆ(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(ದತ್ತಾಂಶ, "re-send data shorter than previously");
      ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)mem, len);

  ಆದರೆ(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-nwritten);
    ಆಯ್ಕೆ(nwritten) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_3
    ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      ಫಲಿತಾಂಶ = mbed_new_session(cf, ದತ್ತಾಂಶ);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಹೋಗು do_send;
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ((ಫಲಿತಾಂಶ == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *pnwritten);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode mbedtls_shutdown(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 bool send_shutdown, bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[1024];
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  ಆದರೆ(!backend->initialized || cf->shutdown) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *ಮುಗಿದ = FALSE;

  ಆದರೆ(!backend->ಕಳುಹಿಸಿದ_shutdown) {
    /* do this only once */
    backend->ಕಳುಹಿಸಿದ_shutdown = TRUE;
    ಆದರೆ(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      ಆಯ್ಕೆ(ret) {
      ಸ್ಥಿತಿ 0: /* we sent it, receive from the server */
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *ಮುಗಿದ = TRUE;
        ಹೋಗು ಹೊರಹೋಗು;
      ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        ಹೋಗು ಹೊರಹೋಗು;
      ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ಹೋಗು ಹೊರಹೋಗು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_shutdown error -0x%04X",
                    (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret);
        ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ಗಾಗಿ(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, ಗಾತ್ರ(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    ಆದರೆ(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, ಗಾತ್ರ(buf));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_3
    ಆದರೆ(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      ಮುಂದುವರಿ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(ret <= 0)
      ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_shutdown, still getting data");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_shutdown done");
    *ಮುಗಿದ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_shutdown error -0x%04X",
                (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-ret);
    ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
  }

ಹೊರಹೋಗು:
  cf->shutdown = (ಫಲಿತಾಂಶ || *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ mbedtls_close(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#ಆದರೆ_ಅಂತ್ಯ
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  ಆದರೆ(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

ಸ್ಥಿರ CURLcode mbed_recv(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ಅಕ್ಷರ *buf, size_t buffersize, size_t *pnread)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಪೂರ್ಣಾಂಕ nread;

  DEBUGASSERT(backend);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_SESSION_TICKETS
do_read:
#ಆದರೆ_ಅಂತ್ಯ
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)buf, buffersize);
  ಆದರೆ(nread > 0)
    *pnread = (size_t)nread;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಅಕ್ಷರ errorbuf[128];
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-nread);
    ಆಯ್ಕೆ(nread) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_SESSION_TICKETS
    ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      ಫಲಿತಾಂಶ = mbed_new_session(cf, ದತ್ತಾಂಶ);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      ಆದರೆ(!ಫಲಿತಾಂಶ)
        ಹೋಗು do_read;
      ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      ಫಲಿತಾಂಶ = CURLE_OK;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      mbedtls_strerror(nread, errorbuf, ಗಾತ್ರ(errorbuf));
      failf(ದತ್ತಾಂಶ, "ssl_read returned: (-0x%04X) %s", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)-nread,
            errorbuf);
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ size_t mbedtls_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_ಆವೃತ್ತಿ_C
  /* if mbedtls_version_get_number() is available it is better */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಆವೃತ್ತಿ_2 = mbedtls_ಆವೃತ್ತಿ_get_ಸಂಖ್ಯೆ();
  ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, "mbedTLS/%u.%u.%u", ಆವೃತ್ತಿ_2 >> 24,
                        (ಆವೃತ್ತಿ_2 >> 16) & 0xff, (ಆವೃತ್ತಿ_2 >> 8) & 0xff);
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ curl_msnprintf(ಬಫರ್, ಗಾತ್ರ_2, "mbedTLS/%s", MBEDTLS_ಆವೃತ್ತಿ_STRING);
#ಆದರೆ_ಅಂತ್ಯ
}

/* 'data' might be NULL */
ಸ್ಥಿರ CURLcode mbedtls_random(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *entropy, size_t ಉದ್ದಳತೆ)
{
  psa_ಸ್ಥಿತಿ_t ಸ್ಥಿತಿ_2;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;

  ಸ್ಥಿತಿ_2 = psa_generate_random(entropy, ಉದ್ದಳತೆ);

  ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2 == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ಸ್ಥಿರ CURLcode mbedtls_connect(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;

  /* check if the connection has already been established */
  ಆದರೆ(ssl_connection_complete == connssl->state) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  *ಮುಗಿದ = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ಆದರೆ(connssl->connecting_state == ssl_connect_1) {
    ಫಲಿತಾಂಶ = mbed_connect_step1(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_2) {
    ಫಲಿತಾಂಶ = mbed_connect_step2(cf, ದತ್ತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *ctx = cf->ctx;
    ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
      (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)ctx->backend;

    ಆದರೆ(mbedtls_ssl_get_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ(&backend->ssl) <=
       MBEDTLS_SSL_ಆವೃತ್ತಿ_TLS1_2) {
      ಫಲಿತಾಂಶ = mbed_new_session(cf, ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    connssl->connecting_state = ssl_connect_ಮುಗಿದ;
  }

  ಆದರೆ(connssl->connecting_state == ssl_connect_ಮುಗಿದ) {
    connssl->state = ssl_connection_complete;
    *ಮುಗಿದ = TRUE;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mbedtls_init(ಶೂನ್ಯ)
{
#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x04000000
  ಪೂರ್ಣಾಂಕ ret = 0;
#ಆದರೆ_ಅಂತ್ಯ
  psa_ಸ್ಥಿತಿ_t ಸ್ಥಿತಿ_2;
  ಸ್ಥಿತಿ_2 = psa_crypto_init();

  ಆದರೆ(ಸ್ಥಿತಿ_2 != PSA_SUCCESS)
    ಹಿಂತಿರುಗಿ 0;

#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_ಬೀಜ(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              ಖಾಲಿ, 0);

  ಆದರೆ(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    ಹಿಂತಿರುಗಿ 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_ಗಣ_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ mbedtls_cleanup(ಶೂನ್ಯ)
{
  mbedtls_psa_crypto_free();

#ಆದರೆ MBEDTLS_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ bool mbedtls_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                 ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *ctx = cf->ctx;
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)ctx->backend;
  ಹಿಂತಿರುಗಿ mbedtls_ssl_get_ಬೈಟುಗಳು_avail(&backend->ssl) != 0;
}

ಸ್ಥಿರ CURLcode mbedtls_sha256sum(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಒಳಹರಿವು,
                                  size_t inputlen,
                                  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha256sum,
                                  size_t sha256len)
{
#ಆದರೆ defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_ಸ್ಥಿತಿ_t ಸ್ಥಿತಿ_2;
  size_t sha256len_actual;
  ಸ್ಥಿತಿ_2 = psa_hash_compute(PSA_ALG_SHA_256, ಒಳಹರಿವು, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  ಆದರೆ(ಸ್ಥಿತಿ_2 != PSA_SUCCESS)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  ಹಿಂತಿರುಗಿ CURLE_OK;
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)sha256len;
  ಹಿಂತಿರುಗಿ Curl_sha256it(sha256sum, ಒಳಹರಿವು, inputlen);
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಶೂನ್ಯ *mbedtls_get_internals(ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl,
                                   CURLINFO info)
{
  ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *backend =
    (ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ *)connssl->backend;
  (ಶೂನ್ಯ)info;
  DEBUGASSERT(backend);
  ಹಿಂತಿರುಗಿ &backend->ssl;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_ಮಾರ್ಗ |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#ಆದರೆ_ಅಂತ್ಯ
  SSLSUPP_SSL_CTX |
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#ಆದರೆ_ಅಂತ್ಯ
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#ಆದರೆ_ಅಂತ್ಯ
  0,

  ಗಾತ್ರ(ರಚನೆ mbed_ssl_backend_ದತ್ತಾಂಶ),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_ಆವೃತ್ತಿ,                  /*ಆವೃತ್ತಿ*/
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_ದತ್ತಾಂಶ_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  ಖಾಲಿ,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  ಖಾಲಿ,                             /* close_all */
  ಖಾಲಿ,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  ಖಾಲಿ,                             /* get_channel_binding */
};

#ಆದರೆ_ಅಂತ್ಯ /* USE_MBEDTLS */
