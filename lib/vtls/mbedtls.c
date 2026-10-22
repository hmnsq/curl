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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/version.h>
#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x03020000
#ਗਲਤੀ "mbedTLS 3.2.0 or greater required"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <psa/crypto_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/net_sockets.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/ssl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/x509.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/psa_util.h>

#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#ਗਲਤੀ "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/error.h>
#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x04000000
#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/entropy.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/ctr_drbg.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ <mbedtls/debug.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/cipher_suite.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/mbedtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/x509asn1.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h" /* for the connect timeout */
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha256.h"

/* ALPN for http2 */
#ਜੇ defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAS_ALPN_MBEDTLS
#ਜੇ_ਅੰਤ

ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#ਜੇ_ਅੰਤ
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ALPN_MBEDTLS
  ਅਚਲ ਅੱਖਰ *protocols[3];
#ਜੇ_ਅੰਤ
  ਪੂਰਨ *ciphersuites;
  size_t send_blocked_len;
  ਦੋਆਧਾਰੀ_ਅੰਕ(initialized); /* mbedtls_ssl_context is initialized */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਭੇਜਿਆ_shutdown);
  ਦੋਆਧਾਰੀ_ਅੰਕ(send_blocked);
};

/** A context for random number generation (RNG).
 */
#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x04000000
ਬਣਤਰ rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ਸਥਿਰ ਬਣਤਰ rng_context_t rng;
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MBEDTLS_ERROR_C
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mbedtls_strerror(a, b, c) b[0] = 0
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_DEBUG
ਸਥਿਰ ਖਾਲੀ mbed_debug(ਖਾਲੀ *context, ਪੂਰਨ level, ਅਚਲ ਅੱਖਰ *f_name,
                       ਪੂਰਨ ਸਤਰ_nb, ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = (ਬਣਤਰ Curl_easy *)context;
  (ਖਾਲੀ)level;
  (ਖਾਲੀ)ਸਤਰ_nb;
  (ਖਾਲੀ)f_name;

  ਜੇ(ਅੰਕੜੇ) {
    size_t len = ਲੜੀ_ਲੰਬਾਈ(ਸਤਰ);
    ਜੇ(len && (ਸਤਰ[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(ਅੰਕੜੇ, "%.*s", (ਪੂਰਨ)len, ਸਤਰ);
  }
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ mbedtls_bio_cf_write(ਖਾਲੀ *bio,
                                ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t blen)
{
  ਬਣਤਰ Curl_cfilter *cf = bio;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ 0;

  ਨਤੀਜਾ = Curl_conn_cf_send(cf->ਅਗਲਾ, ਅੰਕੜੇ, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (ਪੂਰਨ)ਨਤੀਜਾ, nwritten);
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
    ਵਾਪਸ MBEDTLS_ERR_SSL_WANT_WRITE;
  ਵਾਪਸ ਨਤੀਜਾ ? -1 : (ਪੂਰਨ)nwritten;
}

ਸਥਿਰ ਪੂਰਨ mbedtls_bio_cf_read(ਖਾਲੀ *bio, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *buf, size_t blen)
{
  ਬਣਤਰ Curl_cfilter *cf = bio;
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);
  size_t nread = 0;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(!ਅੰਕੜੇ)
    ਵਾਪਸ 0;
  /* OpenSSL catches this case, so should we. */
  ਜੇ(!buf)
    ਵਾਪਸ 0;

  ਨਤੀਜਾ = Curl_conn_cf_recv(cf->ਅਗਲਾ, ਅੰਕੜੇ, (ਅੱਖਰ *)buf, blen, &nread);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (ਪੂਰਨ)ਨਤੀਜਾ, nread);
  ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
    ਵਾਪਸ MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  ਵਾਪਸ ਨਤੀਜਾ ? -1 : (ਪੂਰਨ)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RSA_PUB_DER_MAX_ਬਾਈਟਾਂ   (38 + (2 * MBEDTLS_MPI_MAX_ਮਾਪ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ECP_PUB_DER_MAX_ਬਾਈਟਾਂ   (30 + (2 * MBEDTLS_ECP_MAX_ਬਾਈਟਾਂ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PUB_DER_MAX_ਬਾਈਟਾਂ   (RSA_PUB_DER_MAX_ਬਾਈਟਾਂ > ECP_PUB_DER_MAX_ਬਾਈਟਾਂ ? \
                             RSA_PUB_DER_MAX_ਬਾਈਟਾਂ : ECP_PUB_DER_MAX_ਬਾਈਟਾਂ)

ਸਥਿਰ CURLcode mbed_ਸੈੱਟ_ssl_ਸੰਸਕਰਣ_min_max(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend,
  ਬਣਤਰ ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_ਸੰਸਕਰਣ ver_min =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_ਸੰਸਕਰਣ_TLS1_2
#ਨਹੀਂਤਾਂ
    MBEDTLS_SSL_ਸੰਸਕਰਣ_TLS1_3
#ਜੇ_ਅੰਤ
    ;
  mbedtls_ssl_protocol_ਸੰਸਕਰਣ ver_max =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_ਸੰਸਕਰਣ_TLS1_3
#ਨਹੀਂਤਾਂ
    MBEDTLS_SSL_ਸੰਸਕਰਣ_TLS1_2
#ਜੇ_ਅੰਤ
    ;

  DEBUGASSERT(conn_config->ਸੰਸਕਰਣ_2 != CURL_SSLVERSION_DEFAULT);
  ਚੋਣ(conn_config->ਸੰਸਕਰਣ_2) {
  ਹਾਲਤ CURL_SSLVERSION_TLSv1:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_0:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_1:
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_2:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_ਸੰਸਕਰਣ_TLS1_2;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURL_SSLVERSION_TLSv1_3:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_ਸੰਸਕਰਣ_TLS1_3;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->ਸੰਸਕਰਣ_2);
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  ਚੋਣ(conn_config->ਸੰਸਕਰਣ_max) {
  ਹਾਲਤ CURL_SSLVERSION_MAX_DEFAULT:
  ਹਾਲਤ CURL_SSLVERSION_MAX_NONE:
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_3:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_ਸੰਸਕਰਣ_TLS1_3;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_2
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_ਸੰਸਕਰਣ_TLS1_2;
    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_1:
  ਹਾਲਤ CURL_SSLVERSION_MAX_TLSv1_0:
  ਮੂਲ:
    failf(ਅੰਕੜੇ, "mbedTLS: unsupported maximum TLS version value");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_ਸੰਸਕਰਣ(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_ਸੰਸਕਰਣ(&backend->config, ver_max);

  ਵਾਪਸ CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ਸਥਿਰ ਪੂਰਨ mbed_cipher_suite_get_str(uint16_t id, ਅੱਖਰ *buf, size_t buf_ਮਾਪ,
                                     bool prefer_rfc)
{
  ਜੇ(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_ਮਾਪ, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ Curl_cipher_suite_get_str(id, buf, buf_ਮਾਪ, prefer_rfc);
  ਵਾਪਸ 0;
}

ਸਥਿਰ uint16_t mbed_cipher_suite_walk_str(ਅਚਲ ਅੱਖਰ **str, ਅਚਲ ਅੱਖਰ **ਸਿਰਾ)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, ਸਿਰਾ);
  size_t len = *ਸਿਰਾ - *str;
  ਸਥਿਰ ਅਚਲ ਅੱਖਰ ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  ਜੇ(!id) {
    ਜੇ((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  ਵਾਪਸ id;
}
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode mbed_ਸੈੱਟ_selected_ciphers(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend,
  ਅਚਲ ਅੱਖਰ *ciphers12,
  ਅਚਲ ਅੱਖਰ *ciphers13)
{
  ਅਚਲ ਅੱਖਰ *ciphers = ciphers12;
  ਅਚਲ ਪੂਰਨ *supported;
  ਪੂਰਨ *selected;
  size_t supported_len, ਗਿਣਤੀ_2 = 0, default13_ਗਿਣਤੀ = 0, i, j;
  ਅਚਲ ਅੱਖਰ *ptr, *ਸਿਰਾ;

  supported = mbedtls_ssl_list_ciphersuites();
  ਲਈ(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(ਆਕਾਰ(ਪੂਰਨ) * (supported_len + 1));
  ਜੇ(!selected)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3
  (ਖਾਲੀ)ciphers13, (ਖਾਲੀ)j;
#ਨਹੀਂਤਾਂ
  ਜੇ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    ਲਈ(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ਜੇ(ਲੜੀ_ਤੁਲਨਾ(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ਜਾਰੀ;

      selected[ਗਿਣਤੀ_2++] = id;
    }

    default13_ਗਿਣਤੀ = ਗਿਣਤੀ_2;
  }
  ਨਹੀਂਤਾਂ
    ciphers = ciphers13;

ਜੋੜੋ_ciphers:
#ਜੇ_ਅੰਤ
  ਲਈ(ptr = ciphers; ptr[0] != '\0' && ਗਿਣਤੀ_2 < supported_len; ptr = ਸਿਰਾ) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &ਸਿਰਾ);

    /* Check if cipher is supported */
    ਜੇ(id) {
      ਲਈ(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      ਜੇ(i == supported_len)
        id = 0;
    }
    ਜੇ(!id) {
      ਜੇ(ptr[0] != '\0')
        infof(ਅੰਕੜੇ, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (ਪੂਰਨ)(ਸਿਰਾ - ptr), ptr);
      ਜਾਰੀ;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    ਲਈ(i = 0; i < ਗਿਣਤੀ_2 && selected[i] != id; i++)
      ;
    ਜੇ(i < ਗਿਣਤੀ_2) {
      ਜੇ(i >= default13_ਗਿਣਤੀ)
        infof(ਅੰਕੜੇ, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (ਪੂਰਨ)(ਸਿਰਾ - ptr), ptr);
      ਜਾਰੀ;
    }

    selected[ਗਿਣਤੀ_2++] = id;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3
  ਜੇ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    ਜਾਓ ਜੋੜੋ_ciphers;
  }

  ਜੇ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    ਲਈ(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ਜਾਰੀ;

      /* No duplicates allowed (so selected cannot overflow) */
      ਲਈ(i = 0; i < ਗਿਣਤੀ_2 && selected[i] != id; i++)
        ;
      ਜੇ(i < ਗਿਣਤੀ_2)
        ਜਾਰੀ;

      selected[ਗਿਣਤੀ_2++] = id;
    }
  }
#ਜੇ_ਅੰਤ

  selected[ਗਿਣਤੀ_2] = 0;

  ਜੇ(ਗਿਣਤੀ_2 == 0) {
    curlx_free(selected);
    failf(ਅੰਕੜੇ, "mbedTLS: no supported cipher in list");
    ਵਾਪਸ CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ ਖਾਲੀ mbed_dump_cert_info(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਅਚਲ mbedtls_x509_crt *crt)
{
#ਜੇ !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (ਖਾਲੀ)ਅੰਕੜੇ, (ਖਾਲੀ)crt;
#ਨਹੀਂਤਾਂ
  ਅਚਲ size_t bufsize = 16384;
  ਅੱਖਰ *p, *ਬਫਰ = curlx_malloc(bufsize);

  ਜੇ(ਬਫਰ && mbedtls_x509_crt_info(ਬਫਰ, bufsize, " ", crt) > 0) {
    infof(ਅੰਕੜੇ, "Server certificate:");
    ਲਈ(p = ਬਫਰ; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(ਅੰਕੜੇ, "%.*s", (ਪੂਰਨ)s, p);
      p += s;
    }
  }
  ਨਹੀਂਤਾਂ
    infof(ਅੰਕੜੇ, "Unable to dump certificate information");

  curlx_free(ਬਫਰ);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ mbed_ਕੱਢੋ_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਅਚਲ mbedtls_x509_crt *crt)
{
  CURLcode ਨਤੀਜਾ;
  ਅਚਲ mbedtls_x509_crt *cur;
  ਪੂਰਨ cert_ਗਿਣਤੀ = 0;
  ਪੂਰਨ i;

  ਲਈ(cur = crt; cur && cert_ਗਿਣਤੀ <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->ਅਗਲਾ)
    cert_ਗਿਣਤੀ++;

  ਜੇ(cert_ਗਿਣਤੀ > MAX_ALLOWED_CERT_AMOUNT) {
    infof(ਅੰਕੜੇ, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    ਵਾਪਸ;
  }

  ਨਤੀਜਾ = Curl_ssl_init_certinfo(ਅੰਕੜੇ, cert_ਗਿਣਤੀ);

  ਲਈ(i = 0, cur = crt; ਨਤੀਜਾ == CURLE_OK && cur; ++i, cur = cur->ਅਗਲਾ) {
    ਨਤੀਜਾ = Curl_ਕੱਢੋ_certinfo(ਅੰਕੜੇ, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ਸਥਿਰ ਪੂਰਨ mbed_verify_cb(ਖਾਲੀ *ptr, mbedtls_x509_crt *crt,
                          ਪੂਰਨ ਡੂੰਘਾਈ, uint32_t *flags)
{
  ਬਣਤਰ Curl_cfilter *cf = (ਬਣਤਰ Curl_cfilter *)ptr;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = CF_ਅੰਕੜੇ_CURRENT(cf);

  ਜੇ(ਡੂੰਘਾਈ == 0) {
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.verbose)
      mbed_dump_cert_info(ਅੰਕੜੇ, crt);
    ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl.certinfo)
      mbed_ਕੱਢੋ_certinfo(ਅੰਕੜੇ, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  ਜੇ(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  ਜੇ(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  ਜੇ(*flags) {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MBEDTLS_X509_REMOVE_INFO
    ਅੱਖਰ buf[128];
    mbedtls_x509_crt_verify_info(buf, ਆਕਾਰ(buf), "", *flags);
    failf(ਅੰਕੜੇ, "mbedTLS: %s", buf);
#ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "mbedTLS: certificate verification error 0x%08x",
          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)*flags);
#ਜੇ_ਅੰਤ
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ CURLcode mbed_ਲੱਦੋ_cacert(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਅਚਲ ਬਣਤਰ curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ਨਿਰਾ : conn_config->CAfile);
  ਅਚਲ bool verifypeer = conn_config->verifypeer;
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_capath = conn_config->CApath;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_PEM_PARSE_C
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_cert_type = conn_config->cert_type;
#ਜੇ_ਅੰਤ
  ਪੂਰਨ ret = -1;
  ਅੱਖਰ errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  ਜੇ(ca_info_blob && verifypeer) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ਜੇ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((ਅੱਖਰ *)(ca_info_blob->ਅੰਕੜੇ))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->ਅੰਕੜੇ,
                                   ca_info_blob->len);
    }
    ਨਹੀਂਤਾਂ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *newblob = curlx_memdup0(ca_info_blob->ਅੰਕੜੇ,
                                             ca_info_blob->len);
      ਜੇ(!newblob)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#ਨਹੀਂਤਾਂ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->ਅੰਕੜੇ,
                                     ca_info_blob->len);
#ਜੇ_ਅੰਤ

    ਜੇ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
      failf(ਅੰਕੜੇ, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);
      ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
    }
  }

  ਜੇ(ssl_cafile && verifypeer) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    ਜੇ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
      failf(ਅੰਕੜੇ, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);
      ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
    }
#ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "mbedTLS: functions that use the file system not built in");
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  }

  ਜੇ(ssl_capath) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_ਰਸਤਾ(&backend->cacert, ssl_capath);

    ਜੇ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
      failf(ਅੰਕੜੇ, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);

      ਜੇ(verifypeer)
        ਵਾਪਸ CURLE_SSL_CACERT_BADFILE;
    }
#ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "mbedTLS: functions that use the file system not built in");
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode mbed_ਲੱਦੋ_clicert(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਅੱਖਰ * ਅਚਲ ssl_cert = conn_config->clientcert;
  ਅਚਲ ਬਣਤਰ curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_PEM_PARSE_C
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_cert_type = conn_config->cert_type;
#ਜੇ_ਅੰਤ
  ਪੂਰਨ ret = -1;
  ਅੱਖਰ errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  ਜੇ(ssl_cert) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    ਜੇ(ret) {
      mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
      failf(ਅੰਕੜੇ, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);

      ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
    }
#ਨਹੀਂਤਾਂ
    failf(ਅੰਕੜੇ, "mbedTLS: functions that use the file system not built in");
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  }

  ਜੇ(ssl_cert_blob) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ਜੇ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((ਅੱਖਰ *)(ssl_cert_blob->ਅੰਕੜੇ))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->ਅੰਕੜੇ,
                                   ssl_cert_blob->len);
    }
    ਨਹੀਂਤਾਂ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *newblob = curlx_memdup0(ssl_cert_blob->ਅੰਕੜੇ,
                                             ssl_cert_blob->len);
      ਜੇ(!newblob)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#ਨਹੀਂਤਾਂ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->ਅੰਕੜੇ,
                                     ssl_cert_blob->len);
#ਜੇ_ਅੰਤ

    ਜੇ(ret) {
      mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
      failf(ਅੰਕੜੇ, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);
      ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
    }
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode mbed_ਲੱਦੋ_privkey(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਪੂਰਨ ret = -1;
  ਅੱਖਰ errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  ਜੇ(conn_config->ਕੁੰਜੀ || conn_config->ਕੁੰਜੀ_blob) {
    ਜੇ(conn_config->ਕੁੰਜੀ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_FS_IO
#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ਕੁੰਜੀ,
                                     conn_config->ਕੁੰਜੀ_passwd);
      ਜੇ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ਕੁੰਜੀ_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ਕੁੰਜੀ_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ਨਹੀਂਤਾਂ
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ਕੁੰਜੀ,
                                     conn_config->ਕੁੰਜੀ_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      ਜੇ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ਜੇ_ਅੰਤ

      ਜੇ(ret) {
        mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
        failf(ਅੰਕੜੇ, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->ਕੁੰਜੀ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }
#ਨਹੀਂਤਾਂ
      failf(ਅੰਕੜੇ, "mbedTLS: functions that use the file system not built in");
      ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ {
      ਅਚਲ ਬਣਤਰ curl_blob *ssl_ਕੁੰਜੀ_blob = conn_config->ਕੁੰਜੀ_blob;
      ਅਚਲ ਅੱਖਰ *passwd = conn_config->ਕੁੰਜੀ_passwd;
      ਅਚਲ bool is_der = conn_config->ਕੁੰਜੀ_type &&
                          curl_strequal(conn_config->ਕੁੰਜੀ_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *newblob = curlx_memdup0(ssl_ਕੁੰਜੀ_blob->ਅੰਕੜੇ,
                                             ssl_ਕੁੰਜੀ_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_ਕੁੰਜੀ_blob->len : ssl_ਕੁੰਜੀ_blob->len + 1;
      ਜੇ(!newblob)
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x04000000
      ret = mbedtls_pk_parse_ਕੁੰਜੀ(&backend->pk, newblob, keylen,
                                 (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)passwd,
                                 passwd ? ਲੜੀ_ਲੰਬਾਈ(passwd) : 0);
      ਜੇ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ਕੁੰਜੀ_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ਕੁੰਜੀ_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ਨਹੀਂਤਾਂ
      ret = mbedtls_pk_parse_ਕੁੰਜੀ(&backend->pk, newblob, keylen,
                                 (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)passwd,
                                 passwd ? ਲੜੀ_ਲੰਬਾਈ(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      ਜੇ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ਜੇ_ਅੰਤ
      curlx_free(newblob);

      ਜੇ(ret) {
        mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
        failf(ਅੰਕੜੇ, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);
        ਵਾਪਸ CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode mbed_ਲੱਦੋ_crl(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਅਚਲ ਅੱਖਰ * ਅਚਲ ssl_crlfile = conn_config->CRLfile;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  ਜੇ(ssl_crlfile) {
    ਅੱਖਰ errorbuf[128];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_FS_IO
    ਪੂਰਨ ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    ਜੇ(ret) {
      mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
      failf(ਅੰਕੜੇ, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);

      ਵਾਪਸ CURLE_SSL_CRL_BADFILE;
    }
#ਨਹੀਂਤਾਂ
    (ਖਾਲੀ)errorbuf;
    failf(ਅੰਕੜੇ, "mbedTLS: functions that use the file system not built in");
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
#ਜੇ_ਅੰਤ
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)backend;
  ਜੇ(ssl_crlfile) {
    failf(ਅੰਕੜੇ, "mbedTLS: CRL support not built in");
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool mbed_apply_session(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ Curl_ssl_session *sc_session)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;

  ਜੇ(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    ਪੂਰਨ ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_ਲੱਦੋ(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    ਜੇ(ret) {
      failf(ਅੰਕੜੇ, "SSL session error loading: -0x%x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret);
    }
    ਨਹੀਂਤਾਂ {
      ret = mbedtls_ssl_ਸੈੱਟ_session(&backend->ssl, &session);
      ਜੇ(ret)
        failf(ਅੰਕੜੇ, "SSL session error setting: -0x%x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret);
    }
    mbedtls_ssl_session_free(&session);
    ਜੇ(!ret) {
      infof(ਅੰਕੜੇ, "SSL reusing session ID");
      ਵਾਪਸ TRUE;
    }
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLcode mbed_configure_ssl(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਪੂਰਨ ret;
  bool session_applied = FALSE;
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ errorbuf[128];

  infof(ਅੰਕੜੇ, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->ਪੋਰਟ);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_ਧਾਰਾ,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  ਜੇ(ret) {
    failf(ਅੰਕੜੇ, "mbedTLS: ssl_config failed");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, ਅੰਕੜੇ);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_ਸੈੱਟ_ਹੱਦ(4);
#ਜੇ_ਅੰਤ

#ਜੇ defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ >= 0x03060100 &&   \
  MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#ਜੇ_ਅੰਤ

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_ਅਗਲਾ);

  ਨਤੀਜਾ = mbed_ਸੈੱਟ_ssl_ਸੰਸਕਰਣ_min_max(ਅੰਕੜੇ, backend, conn_config);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#ਜੇ_ਅੰਤ

  ret = mbedtls_ssl_ਤਿਆਰੀ(&backend->ssl,
                          &backend->config);
  ਜੇ(ret) {
    mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
    failf(ਅੰਕੜੇ, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_ਸੈੱਟ_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      ਨਿਰਾ /*  rev_timeout() */);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3
  ਜੇ(conn_config->cipher_list) {
    ਨਤੀਜਾ = mbed_ਸੈੱਟ_selected_ciphers(ਅੰਕੜੇ, backend,
                                       conn_config->cipher_list, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
  ਜੇ(conn_config->cipher_list || conn_config->cipher_list13) {
    ਨਤੀਜਾ = mbed_ਸੈੱਟ_selected_ciphers(ਅੰਕੜੇ, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#ਜੇ_ਅੰਤ
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "mbedTLS: failed to set cipher suites");
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਨਹੀਂਤਾਂ {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#ਜੇ_ਅੰਤ

  ਜੇ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ਬਣਤਰ Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(ਅੰਕੜੇ, cf->cft, FIRSTSOCKET);
    ਜੇ(scs) {
      ਜੇ(mbed_apply_session(cf, ਅੰਕੜੇ, scs)) {
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  ਜੇ(!session_applied && Curl_ssl_scache_use(cf, ਅੰਕੜੇ)) {
    ਬਣਤਰ Curl_ssl_session *sc_session = ਨਿਰਾ;
    CURLcode sresult = Curl_ssl_scache_take(cf, ਅੰਕੜੇ, connssl->peer.scache_ਕੁੰਜੀ,
                                            &sc_session);
    ਜੇ(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_ਲੱਦੋ(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      ਜੇ(ret) {
        failf(ਅੰਕੜੇ, "SSL session error loading: -0x%x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret);
      }
      ਨਹੀਂਤਾਂ {
        ret = mbedtls_ssl_ਸੈੱਟ_session(&backend->ssl, &session);
        ਜੇ(ret)
          failf(ਅੰਕੜੇ, "SSL session error setting: -0x%x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret);
        ਨਹੀਂਤਾਂ
          infof(ਅੰਕੜੇ, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, ਅੰਕੜੇ, connssl->peer.scache_ਕੁੰਜੀ,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#ਨਹੀਂਤਾਂ
                            ਨਿਰਾ
#ਜੇ_ਅੰਤ
    );

  ਜੇ(conn_config->ਕੁੰਜੀ || conn_config->ਕੁੰਜੀ_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  ਜੇ(mbedtls_ssl_ਸੈੱਟ_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(ਅੰਕੜੇ, "Failed to set SNI");
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ALPN_MBEDTLS
  ਜੇ(connssl->alpn) {
    ਬਣਤਰ alpn_proto_buf proto;
    size_t i;

    ਲਈ(i = 0; i < connssl->alpn->ਗਿਣਤੀ_2; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    ਜੇ(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(ਅੰਕੜੇ, "Failed setting ALPN protocols");
      ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_ਮੰਜ਼ਿਲ_proto_str(&proto, connssl->alpn);
    infof(ਅੰਕੜੇ, VTLS_INFOF_ALPN_OFFER_1STR, proto.ਅੰਕੜੇ);
  }
#ਜੇ_ਅੰਤ

  /* give application a chance to interfere with mbedTLS set up. */
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctx) {
    ਨਤੀਜਾ = (*ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctx)(ਅੰਕੜੇ, &backend->config,
                                      ਅੰਕੜੇ->ਸੈੱਟ.ssl_fsslctxp);
    ਜੇ(ਨਤੀਜਾ)
      failf(ਅੰਕੜੇ, "error signaled by SSL ctx callback");
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mbed_connect_step1(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ਨਤੀਜਾ;

  ਜੇ((conn_config->ਸੰਸਕਰਣ_2 == CURL_SSLVERSION_SSLv2) ||
     (conn_config->ਸੰਸਕਰਣ_2 == CURL_SSLVERSION_SSLv3)) {
    failf(ਅੰਕੜੇ, "Not supported SSL version");
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
  }

  ਨਤੀਜਾ = mbed_ਲੱਦੋ_cacert(cf, ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = mbed_ਲੱਦੋ_clicert(cf, ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = mbed_ਲੱਦੋ_privkey(cf, ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = mbed_ਲੱਦੋ_crl(cf, ਅੰਕੜੇ);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = mbed_configure_ssl(cf, ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  connssl->connecting_state = ssl_connect_2;
  ਵਾਪਸ CURLE_OK;
}

#ਜੇ defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_PINNED_PUBKEY
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode mbed_connect_step2(ਬਣਤਰ Curl_cfilter *cf,
                                   ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
#ਜੇ defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode ਨਤੀਜਾ;
#ਜੇ_ਅੰਤ
  ਪੂਰਨ ret;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PINNED_PUBKEY
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਅਚਲ ਅੱਖਰ * ਅਚਲ pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY);
#ਨਹੀਂਤਾਂ
  ਅਚਲ ਅੱਖਰ * ਅਚਲ pinnedpubkey =
    CURL_EASY_STR(ਅੰਕੜੇ, STRING_SSL_PINNEDPUBLICKEY);
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  ਜੇ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ਵਾਪਸ CURLE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(ਅੰਕੜੇ, "peer certificate could not be verified");
    ਵਾਪਸ CURLE_PEER_FAILED_VERIFICATION;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ret) {
    ਅੱਖਰ errorbuf[128];
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "TLS version %04X",
                mbedtls_ssl_get_ਸੰਸਕਰਣ_ਸੰਖਿਆ(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, ਆਕਾਰ(errorbuf));
    failf(ਅੰਕੜੇ, "ssl_handshake returned: (-0x%04X) %s",
          (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret, errorbuf);
    ਵਾਪਸ CURLE_SSL_CONNECT_ERROR;
  }

  {
    ਅੱਖਰ cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, ਆਕਾਰ(cipher_str), TRUE);
    infof(ਅੰਕੜੇ, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_ਸੰਸਕਰਣ(&backend->ssl), cipher_str);
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PINNED_PUBKEY
  ਜੇ(pinnedpubkey) {
    ਪੂਰਨ ਮਾਪ;
    ਅਚਲ mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = ਨਿਰਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pubkey = ਨਿਰਾ;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    ਜੇ(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(ਅੰਕੜੇ, "Failed due to missing peer certificate");
      ਵਾਪਸ CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, ਆਕਾਰ(*p));

    ਜੇ(!p)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

    pubkey = curlx_malloc(PUB_DER_MAX_ਬਾਈਟਾਂ);

    ਜੇ(!pubkey) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    ਜੇ(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(ਅੰਕੜੇ, "Failed copying peer certificate");
      ਨਤੀਜਾ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      ਜਾਓ pinnedpubkey_error;
    }

    ਮਾਪ = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_ਬਾਈਟਾਂ);

    ਜੇ(ਮਾਪ <= 0) {
      failf(ਅੰਕੜੇ, "Failed copying public key from peer certificate");
      ਨਤੀਜਾ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      ਜਾਓ pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    ਨਤੀਜਾ = Curl_pin_peer_pubkey(ਅੰਕੜੇ,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_ਬਾਈਟਾਂ - ਮਾਪ], ਮਾਪ);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAS_ALPN_MBEDTLS
  ਜੇ(connssl->alpn) {
    ਅਚਲ ਅੱਖਰ *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    ਨਤੀਜਾ = Curl_alpn_ਸੈੱਟ_negotiated(cf, ਅੰਕੜੇ, connssl,
                                      (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)proto,
                                      proto ? ਲੜੀ_ਲੰਬਾਈ(proto) : 0);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
#ਜੇ_ਅੰਤ

  connssl->connecting_state = ssl_connect_3;
  infof(ਅੰਕੜੇ, "SSL connected");

  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode mbed_new_session(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  ਬਣਤਰ Curl_ssl_session *sc_session = ਨਿਰਾ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sdata = ਨਿਰਾ;
  size_t slen = 0;
  ਪੂਰਨ ietf_tls_id;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ ret;

  DEBUGASSERT(backend);
  ਜੇ(!Curl_ssl_scache_use(cf, ਅੰਕੜੇ))
    ਵਾਪਸ CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  ਜੇ(ret) {
    failf(ਅੰਕੜੇ, "mbedtls_ssl_get_session returned -0x%x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret);
    ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  mbedtls_ssl_session_save(&session, ਨਿਰਾ, 0, &slen);
  ਜੇ(!slen) {
    failf(ਅੰਕੜੇ, "failed to serialize session: length is 0");
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  sdata = curlx_malloc(slen);
  ਜੇ(!sdata) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  ਜੇ(ret) {
    failf(ਅੰਕੜੇ, "failed to serialize session: -0x%x", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ietf_tls_id = mbedtls_ssl_get_ਸੰਸਕਰਣ_ਸੰਖਿਆ(&backend->ssl);
  ਨਤੀਜਾ = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = ਨਿਰਾ;  /* call took ownership */
  ਜੇ(!ਨਤੀਜਾ && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    ਨਤੀਜਾ = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  ਜੇ(!ਨਤੀਜਾ) {
    ਨਤੀਜਾ = Curl_ssl_scache_put(cf, ਅੰਕੜੇ, connssl->peer.scache_ਕੁੰਜੀ,
                                 sc_session);
    sc_session = ਨਿਰਾ;
  }

ਬਾਹਰਲਾ:
  ਜੇ(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mbed_send(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅਚਲ ਖਾਲੀ *mem, size_t len, size_t *pnwritten)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ nwritten;

  DEBUGASSERT(backend);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#ਜੇ_ਅੰਤ
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  ਜੇ(backend->send_blocked) {
    ਜੇ(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(ਅੰਕੜੇ, "re-send data shorter than previously");
      ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)mem, len);

  ਜੇ(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-nwritten);
    ਚੋਣ(nwritten) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3
    ਹਾਲਤ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      ਨਤੀਜਾ = mbed_new_session(cf, ਅੰਕੜੇ);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      ਜੇ(!ਨਤੀਜਾ)
        ਜਾਓ do_send;
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਹਾਲਤ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਰੋਕੋ;
    ਹਾਲਤ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਰੋਕੋ;
    ਮੂਲ:
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਰੋਕੋ;
    }
    ਜੇ((ਨਤੀਜਾ == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (ਪੂਰਨ)ਨਤੀਜਾ, *pnwritten);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode mbedtls_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 bool send_shutdown, bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[1024];
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  ਜੇ(!backend->initialized || cf->shutdown) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *ਪੂਰਾ = FALSE;

  ਜੇ(!backend->ਭੇਜਿਆ_shutdown) {
    /* do this only once */
    backend->ਭੇਜਿਆ_shutdown = TRUE;
    ਜੇ(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      ਚੋਣ(ret) {
      ਹਾਲਤ 0: /* we sent it, receive from the server */
        ਰੋਕੋ;
      ਹਾਲਤ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *ਪੂਰਾ = TRUE;
        ਜਾਓ ਬਾਹਰਲਾ;
      ਹਾਲਤ MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        ਜਾਓ ਬਾਹਰਲਾ;
      ਹਾਲਤ MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ਜਾਓ ਬਾਹਰਲਾ;
      ਮੂਲ:
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_shutdown error -0x%04X",
                    (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret);
        ਨਤੀਜਾ = CURLE_RECV_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ਲਈ(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, ਆਕਾਰ(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    ਜੇ(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, ਆਕਾਰ(buf));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3
    ਜੇ(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      ਜਾਰੀ;
#ਜੇ_ਅੰਤ
    ਜੇ(ret <= 0)
      ਰੋਕੋ;
  }

  ਜੇ(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_shutdown, still getting data");
  }
  ਨਹੀਂਤਾਂ ਜੇ(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_shutdown done");
    *ਪੂਰਾ = TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_shutdown error -0x%04X",
                (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-ret);
    ਨਤੀਜਾ = CURLE_RECV_ERROR;
  }

ਬਾਹਰਲਾ:
  cf->shutdown = (ਨਤੀਜਾ || *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ mbedtls_close(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#ਜੇ_ਅੰਤ
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  ਜੇ(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

ਸਥਿਰ CURLcode mbed_recv(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਅੱਖਰ *buf, size_t buffersize, size_t *pnread)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਪੂਰਨ nread;

  DEBUGASSERT(backend);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_SESSION_TICKETS
do_read:
#ਜੇ_ਅੰਤ
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)buf, buffersize);
  ਜੇ(nread > 0)
    *pnread = (size_t)nread;
  ਨਹੀਂਤਾਂ {
    ਅੱਖਰ errorbuf[128];
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-nread);
    ਚੋਣ(nread) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_SESSION_TICKETS
    ਹਾਲਤ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      ਨਤੀਜਾ = mbed_new_session(cf, ਅੰਕੜੇ);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      ਜੇ(!ਨਤੀਜਾ)
        ਜਾਓ do_read;
      ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਹਾਲਤ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਰੋਕੋ;
    ਹਾਲਤ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਰੋਕੋ;
    ਹਾਲਤ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      ਨਤੀਜਾ = CURLE_OK;
      ਰੋਕੋ;
    ਮੂਲ:
      mbedtls_strerror(nread, errorbuf, ਆਕਾਰ(errorbuf));
      failf(ਅੰਕੜੇ, "ssl_read returned: (-0x%04X) %s", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)-nread,
            errorbuf);
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ size_t mbedtls_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_ਸੰਸਕਰਣ_C
  /* if mbedtls_version_get_number() is available it is better */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸੰਸਕਰਣ_2 = mbedtls_ਸੰਸਕਰਣ_get_ਸੰਖਿਆ();
  ਵਾਪਸ curl_msnprintf(ਬਫਰ, ਮਾਪ, "mbedTLS/%u.%u.%u", ਸੰਸਕਰਣ_2 >> 24,
                        (ਸੰਸਕਰਣ_2 >> 16) & 0xff, (ਸੰਸਕਰਣ_2 >> 8) & 0xff);
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ curl_msnprintf(ਬਫਰ, ਮਾਪ, "mbedTLS/%s", MBEDTLS_ਸੰਸਕਰਣ_STRING);
#ਜੇ_ਅੰਤ
}

/* 'data' might be NULL */
ਸਥਿਰ CURLcode mbedtls_random(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *entropy, size_t ਲੰਮਾਈ)
{
  psa_ਸਥਿਤੀ_t ਸਥਿਤੀ;
  (ਖਾਲੀ)ਅੰਕੜੇ;

  ਸਥਿਤੀ = psa_generate_random(entropy, ਲੰਮਾਈ);

  ਵਾਪਸ ਸਥਿਤੀ == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ਸਥਿਰ CURLcode mbedtls_connect(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;

  /* check if the connection has already been established */
  ਜੇ(ssl_connection_complete == connssl->state) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  *ਪੂਰਾ = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ਜੇ(connssl->connecting_state == ssl_connect_1) {
    ਨਤੀਜਾ = mbed_connect_step1(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_2) {
    ਨਤੀਜਾ = mbed_connect_step2(cf, ਅੰਕੜੇ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *ctx = cf->ctx;
    ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
      (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)ctx->backend;

    ਜੇ(mbedtls_ssl_get_ਸੰਸਕਰਣ_ਸੰਖਿਆ(&backend->ssl) <=
       MBEDTLS_SSL_ਸੰਸਕਰਣ_TLS1_2) {
      ਨਤੀਜਾ = mbed_new_session(cf, ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    connssl->connecting_state = ssl_connect_ਪੂਰਾ;
  }

  ਜੇ(connssl->connecting_state == ssl_connect_ਪੂਰਾ) {
    connssl->state = ssl_connection_complete;
    *ਪੂਰਾ = TRUE;
  }

  ਵਾਪਸ CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ਸਥਿਰ ਪੂਰਨ mbedtls_init(ਖਾਲੀ)
{
#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x04000000
  ਪੂਰਨ ret = 0;
#ਜੇ_ਅੰਤ
  psa_ਸਥਿਤੀ_t ਸਥਿਤੀ;
  ਸਥਿਤੀ = psa_crypto_init();

  ਜੇ(ਸਥਿਤੀ != PSA_SUCCESS)
    ਵਾਪਸ 0;

#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_ਬੀਜ(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              ਨਿਰਾ, 0);

  ਜੇ(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    ਵਾਪਸ 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_ਸੈੱਟ_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#ਜੇ_ਅੰਤ
  ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ mbedtls_cleanup(ਖਾਲੀ)
{
  mbedtls_psa_crypto_free();

#ਜੇ MBEDTLS_ਸੰਸਕਰਣ_ਸੰਖਿਆ < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ bool mbedtls_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                                 ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *ctx = cf->ctx;
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)ctx->backend;
  ਵਾਪਸ mbedtls_ssl_get_ਬਾਈਟਾਂ_avail(&backend->ssl) != 0;
}

ਸਥਿਰ CURLcode mbedtls_sha256sum(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਨਿਵੇਸ਼,
                                  size_t inputlen,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha256sum,
                                  size_t sha256len)
{
#ਜੇ defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_ਸਥਿਤੀ_t ਸਥਿਤੀ;
  size_t sha256len_actual;
  ਸਥਿਤੀ = psa_hash_compute(PSA_ALG_SHA_256, ਨਿਵੇਸ਼, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  ਜੇ(ਸਥਿਤੀ != PSA_SUCCESS)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  ਵਾਪਸ CURLE_OK;
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)sha256len;
  ਵਾਪਸ Curl_sha256it(sha256sum, ਨਿਵੇਸ਼, inputlen);
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਖਾਲੀ *mbedtls_get_internals(ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl,
                                   CURLINFO info)
{
  ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *backend =
    (ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ *)connssl->backend;
  (ਖਾਲੀ)info;
  DEBUGASSERT(backend);
  ਵਾਪਸ &backend->ssl;
}

ਅਚਲ ਬਣਤਰ Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_ਰਸਤਾ |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#ਜੇ_ਅੰਤ
  SSLSUPP_SSL_CTX |
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#ਜੇ_ਅੰਤ
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#ਜੇ_ਅੰਤ
  0,

  ਆਕਾਰ(ਬਣਤਰ mbed_ssl_backend_ਅੰਕੜੇ),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_ਸੰਸਕਰਣ,                  /*ਸੰਸਕਰਣ*/
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_ਅੰਕੜੇ_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  ਨਿਰਾ,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  ਨਿਰਾ,                             /* close_all */
  ਨਿਰਾ,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  ਨਿਰਾ,                             /* get_channel_binding */
};

#ਜੇ_ਅੰਤ /* USE_MBEDTLS */
