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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#ଅନ୍ତର୍ଭାଵ <mbedtls/version.h>
#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#ଅନ୍ତର୍ଭାଵ <psa/crypto_config.h>
#ଅନ୍ତର୍ଭାଵ <mbedtls/net_sockets.h>
#ଅନ୍ତର୍ଭାଵ <mbedtls/ssl.h>
#ଅନ୍ତର୍ଭାଵ <mbedtls/x509.h>
#ଅନ୍ତର୍ଭାଵ <mbedtls/psa_util.h>

#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#ଅନ୍ତର୍ଭାଵ <mbedtls/error.h>
#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x04000000
#ଅନ୍ତର୍ଭାଵ <mbedtls/entropy.h>
#ଅନ୍ତର୍ଭାଵ <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#ଅନ୍ତର୍ଭାଵ <mbedtls/debug.h>
#endif

#ଅନ୍ତର୍ଭାଵ "vtls/cipher_suite.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "vtls/mbedtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_int.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "vtls/x509asn1.h"
#ଅନ୍ତର୍ଭାଵ "connect.h" /* for the connect timeout */
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curl_sha256.h"

/* ALPN for http2 */
#ଯଦି defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

ସଂରଚନା mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  ଅଚଳ ଅକ୍ଷର *protocols[3];
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x04000000
ସଂରଚନା rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ସ୍ଥିର ସଂରଚନା rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
ସ୍ଥିର ଶୂନ୍ୟ mbed_debug(ଶୂନ୍ୟ *context, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅଚଳ ଅକ୍ଷର *f_name,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି_nb, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
  ସଂରଚନା Curl_easy *data = (ସଂରଚନା Curl_easy *)context;
  (ଶୂନ୍ୟ)level;
  (ଶୂନ୍ୟ)ଧାଡ଼ି_nb;
  (ଶୂନ୍ୟ)f_name;

  ଯଦି(data) {
    size_t len = strlen(ଧାଡ଼ି);
    ଯଦି(len && (ଧାଡ଼ି[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len, ଧାଡ଼ି);
  }
}
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbedtls_bio_cf_write(ଶୂନ୍ୟ *bio,
                                ଅଚଳ unsigned ଅକ୍ଷର *buf, size_t blen)
{
  ସଂରଚନା Curl_cfilter *cf = bio;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  ଯଦି(!data)
    ଫେରନ୍ତୁ 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nwritten);
  ଯଦି(result == CURLE_AGAIN)
    ଫେରନ୍ତୁ MBEDTLS_ERR_SSL_WANT_WRITE;
  ଫେରନ୍ତୁ result ? -1 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nwritten;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbedtls_bio_cf_read(ଶୂନ୍ୟ *bio, unsigned ଅକ୍ଷର *buf, size_t blen)
{
  ସଂରଚନା Curl_cfilter *cf = bio;
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  ଯଦି(!data)
    ଫେରନ୍ତୁ 0;
  /* OpenSSL catches this case, so should we. */
  ଯଦି(!buf)
    ଫେରନ୍ତୁ 0;

  result = Curl_conn_cf_recv(cf->next, data, (ଅକ୍ଷର *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, nread);
  ଯଦି(result == CURLE_AGAIN)
    ଫେରନ୍ତୁ MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  ଫେରନ୍ତୁ result ? -1 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

ସ୍ଥିର CURLcode mbed_set_ssl_version_min_max(
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା mbed_ssl_backend_data *backend,
  ସଂରଚନା ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#ଅନ୍ୟଥା
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#ଅନ୍ୟଥା
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  ବାଛ(conn_config->version) {
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1:
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_0:
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_1:
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    ଭାଙ୍ଗ;
#endif
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  ବାଛ(conn_config->version_max) {
  ସ୍ଥିତି CURL_SSLVERSION_MAX_DEFAULT:
  ସ୍ଥିତି CURL_SSLVERSION_MAX_NONE:
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    ଭାଙ୍ଗ;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_1:
  ସ୍ଥିତି CURL_SSLVERSION_MAX_TLSv1_0:
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  ଫେରନ୍ତୁ CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbed_cipher_suite_get_str(uint16_t id, ଅକ୍ଷର *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  ଯଦି(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର uint16_t mbed_cipher_suite_walk_str(ଅଚଳ ଅକ୍ଷର **str, ଅଚଳ ଅକ୍ଷର **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  ସ୍ଥିର ଅଚଳ ଅକ୍ଷର ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  ଯଦି(!id) {
    ଯଦି((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  ଫେରନ୍ତୁ id;
}
#ଅନ୍ୟଥା
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

ସ୍ଥିର CURLcode mbed_set_selected_ciphers(
  ସଂରଚନା Curl_easy *data,
  ସଂରଚନା mbed_ssl_backend_data *backend,
  ଅଚଳ ଅକ୍ଷର *ciphers12,
  ଅଚଳ ଅକ୍ଷର *ciphers13)
{
  ଅଚଳ ଅକ୍ଷର *ciphers = ciphers12;
  ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା *supported;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା *selected;
  size_t supported_len, ଗଣନା_2 = 0, default13_ଗଣନା = 0, i, j;
  ଅଚଳ ଅକ୍ଷର *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  ପାଇଁ(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * (supported_len + 1));
  ଯଦି(!selected)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (ଶୂନ୍ୟ)ciphers13, (ଶୂନ୍ୟ)j;
#ଅନ୍ୟଥା
  ଯଦି(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    ପାଇଁ(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ଯଦି(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ଜାରି;

      selected[ଗଣନା_2++] = id;
    }

    default13_ଗଣନା = ଗଣନା_2;
  }
  ଅନ୍ୟଥା
    ciphers = ciphers13;

add_ciphers:
#endif
  ପାଇଁ(ptr = ciphers; ptr[0] != '\0' && ଗଣନା_2 < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    ଯଦି(id) {
      ପାଇଁ(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      ଯଦି(i == supported_len)
        id = 0;
    }
    ଯଦି(!id) {
      ଯଦି(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - ptr), ptr);
      ଜାରି;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    ପାଇଁ(i = 0; i < ଗଣନା_2 && selected[i] != id; i++)
      ;
    ଯଦି(i < ଗଣନା_2) {
      ଯଦି(i >= default13_ଗଣନା)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(end - ptr), ptr);
      ଜାରି;
    }

    selected[ଗଣନା_2++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  ଯଦି(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    ଯାଅ add_ciphers;
  }

  ଯଦି(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    ପାଇଁ(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ଯଦି(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ଜାରି;

      /* No duplicates allowed (so selected cannot overflow) */
      ପାଇଁ(i = 0; i < ଗଣନା_2 && selected[i] != id; i++)
        ;
      ଯଦି(i < ଗଣନା_2)
        ଜାରି;

      selected[ଗଣନା_2++] = id;
    }
  }
#endif

  selected[ଗଣନା_2] = 0;

  ଯଦି(ଗଣନା_2 == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    ଫେରନ୍ତୁ CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର ଶୂନ୍ୟ mbed_dump_cert_info(ସଂରଚନା Curl_easy *data,
                                ଅଚଳ mbedtls_x509_crt *crt)
{
#ଯଦି !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (ଶୂନ୍ୟ)data, (ଶୂନ୍ୟ)crt;
#ଅନ୍ୟଥା
  ଅଚଳ size_t bufsize = 16384;
  ଅକ୍ଷର *p, *buffer = curlx_malloc(bufsize);

  ଯଦି(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    ପାଇଁ(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s, p);
      p += s;
    }
  }
  ଅନ୍ୟଥା
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ mbed_extract_certinfo(ସଂରଚନା Curl_easy *data,
                                  ଅଚଳ mbedtls_x509_crt *crt)
{
  CURLcode result;
  ଅଚଳ mbedtls_x509_crt *cur;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cert_ଗଣନା = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ପାଇଁ(cur = crt; cur && cert_ଗଣନା <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_ଗଣନା++;

  ଯଦି(cert_ଗଣନା > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    ଫେରନ୍ତୁ;
  }

  result = Curl_ssl_init_certinfo(data, cert_ଗଣନା);

  ପାଇଁ(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbed_verify_cb(ଶୂନ୍ୟ *ptr, mbedtls_x509_crt *crt,
                          ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth, uint32_t *flags)
{
  ସଂରଚନା Curl_cfilter *cf = (ସଂରଚନା Curl_cfilter *)ptr;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା Curl_easy *data = CF_DATA_CURRENT(cf);

  ଯଦି(depth == 0) {
    ଯଦି(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    ଯଦି(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  ଯଦି(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  ଯଦି(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  ଯଦି(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    ଅକ୍ଷର buf[128];
    mbedtls_x509_crt_verify_info(buf, ଆକାର(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#ଅନ୍ୟଥା
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)*flags);
#endif
  }

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର CURLcode mbed_load_cacert(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ଅଚଳ ସଂରଚନା curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ଖାଲି : conn_config->CAfile);
  ଅଚଳ bool verifypeer = conn_config->verifypeer;
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_cert_type = conn_config->cert_type;
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = -1;
  ଅକ୍ଷର errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  ଯଦି(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ଯଦି((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((ଅକ୍ଷର *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    ଅନ୍ୟଥା { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned ଅକ୍ଷର *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      ଯଦି(!newblob)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#ଅନ୍ୟଥା
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    ଯଦି(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);
      ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
    }
  }

  ଯଦି(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    ଯଦି(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);
      ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
    }
#ଅନ୍ୟଥା
    failf(data, "mbedTLS: functions that use the file system not built in");
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
  }

  ଯଦି(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    ଯଦି(ret < 0) {
      mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);

      ଯଦି(verifypeer)
        ଫେରନ୍ତୁ CURLE_SSL_CACERT_BADFILE;
    }
#ଅନ୍ୟଥା
    failf(data, "mbedTLS: functions that use the file system not built in");
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode mbed_load_clicert(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ଅକ୍ଷର * ଅଚଳ ssl_cert = conn_config->clientcert;
  ଅଚଳ ସଂରଚନା curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_cert_type = conn_config->cert_type;
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = -1;
  ଅକ୍ଷର errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  ଯଦି(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    ଯଦି(ret) {
      mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);

      ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
    }
#ଅନ୍ୟଥା
    failf(data, "mbedTLS: functions that use the file system not built in");
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
  }

  ଯଦି(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ଯଦି((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((ଅକ୍ଷର *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    ଅନ୍ୟଥା { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned ଅକ୍ଷର *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      ଯଦି(!newblob)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#ଅନ୍ୟଥା
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    ଯଦି(ret) {
      mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);
      ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode mbed_load_privkey(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = -1;
  ଅକ୍ଷର errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  ଯଦି(conn_config->ଚାବି || conn_config->ଚାବି_blob) {
    ଯଦି(conn_config->ଚାବି) {
#ifdef MBEDTLS_FS_IO
#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ଚାବି,
                                     conn_config->ଚାବି_passwd);
      ଯଦି(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ଚାବି_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ଚାବି_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ଅନ୍ୟଥା
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ଚାବି,
                                     conn_config->ଚାବି_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      ଯଦି(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      ଯଦି(ret) {
        mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->ଚାବି, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      }
#ଅନ୍ୟଥା
      failf(data, "mbedTLS: functions that use the file system not built in");
      ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
    }
    ଅନ୍ୟଥା {
      ଅଚଳ ସଂରଚନା curl_blob *ssl_ଚାବି_blob = conn_config->ଚାବି_blob;
      ଅଚଳ ଅକ୍ଷର *passwd = conn_config->ଚାବି_passwd;
      ଅଚଳ bool is_der = conn_config->ଚାବି_type &&
                          curl_strequal(conn_config->ଚାବି_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned ଅକ୍ଷର *newblob = curlx_memdup0(ssl_ଚାବି_blob->data,
                                             ssl_ଚାବି_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_ଚାବି_blob->len : ssl_ଚାବି_blob->len + 1;
      ଯଦି(!newblob)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା >= 0x04000000
      ret = mbedtls_pk_parse_ଚାବି(&backend->pk, newblob, keylen,
                                 (ଅଚଳ unsigned ଅକ୍ଷର *)passwd,
                                 passwd ? strlen(passwd) : 0);
      ଯଦି(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ଚାବି_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ଚାବି_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ଅନ୍ୟଥା
      ret = mbedtls_pk_parse_ଚାବି(&backend->pk, newblob, keylen,
                                 (ଅଚଳ unsigned ଅକ୍ଷର *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      ଯଦି(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      ଯଦି(ret) {
        mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);
        ଫେରନ୍ତୁ CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode mbed_load_crl(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  ଯଦି(ssl_crlfile) {
    ଅକ୍ଷର errorbuf[128];
#ifdef MBEDTLS_FS_IO
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    ଯଦି(ret) {
      mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);

      ଫେରନ୍ତୁ CURLE_SSL_CRL_BADFILE;
    }
#ଅନ୍ୟଥା
    (ଶୂନ୍ୟ)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
#endif
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)backend;
  ଯଦି(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  }
#endif

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool mbed_apply_session(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା Curl_ssl_session *sc_session)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;

  ଯଦି(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    ଯଦି(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret);
    }
    ଅନ୍ୟଥା {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      ଯଦି(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret);
    }
    mbedtls_ssl_session_free(&session);
    ଯଦି(!ret) {
      infof(data, "SSL reusing session ID");
      ଫେରନ୍ତୁ TRUE;
    }
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLcode mbed_configure_ssl(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
  bool session_applied = FALSE;
  CURLcode result;
  ଅକ୍ଷର errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  ଯଦି(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
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

#ଯଦି defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_ସଂଖ୍ୟା >= 0x03060100 &&   \
  MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x04000000
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
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  ଯଦି(ret) {
    mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      ଖାଲି /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  ଯଦି(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, ଖାଲି);
#ଅନ୍ୟଥା
  ଯଦି(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    ଯଦି(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      ଫେରନ୍ତୁ result;
    }
  }
  ଅନ୍ୟଥା {
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

  ଯଦି((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ସଂରଚନା Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    ଯଦି(scs) {
      ଯଦି(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  ଯଦି(!session_applied && Curl_ssl_scache_use(cf, data)) {
    ସଂରଚନା Curl_ssl_session *sc_session = ଖାଲି;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_ଚାବି,
                                            &sc_session);
    ଯଦି(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      ଯଦି(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret);
      }
      ଅନ୍ୟଥା {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        ଯଦି(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret);
        ଅନ୍ୟଥା
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_ଚାବି,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#ଅନ୍ୟଥା
                            ଖାଲି
#endif
    );

  ଯଦି(conn_config->ଚାବି || conn_config->ଚାବି_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  ଯଦି(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  ଯଦି(connssl->alpn) {
    ସଂରଚନା alpn_proto_buf proto;
    size_t i;

    ପାଇଁ(i = 0; i < connssl->alpn->ଗଣନା_2; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    ଯଦି(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  ଯଦି(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    ଯଦି(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode mbed_connect_step1(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  ଯଦି((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  ଯଦି(!result)
    result = mbed_load_clicert(cf, data);
  ଯଦି(!result)
    result = mbed_load_privkey(cf, data);
  ଯଦି(!result)
    result = mbed_load_crl(cf, data);
  ଯଦି(!result)
    result = mbed_configure_ssl(cf, data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  connssl->connecting_state = ssl_connect_2;
  ଫେରନ୍ତୁ CURLE_OK;
}

#ଯଦି defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

ସ୍ଥିର CURLcode mbed_connect_step2(ସଂରଚନା Curl_cfilter *cf,
                                   ସଂରଚନା Curl_easy *data)
{
#ଯଦି defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#ଅନ୍ୟଥା
  ଅଚଳ ଅକ୍ଷର * ଅଚଳ pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  ଯଦି(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା ଯଦି(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ଅନ୍ୟଥା ଯଦି(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    ଫେରନ୍ତୁ CURLE_PEER_FAILED_VERIFICATION;
  }
  ଅନ୍ୟଥା ଯଦି(ret) {
    ଅକ୍ଷର errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_ସଂଖ୍ୟା(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, ଆକାର(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret, errorbuf);
    ଫେରନ୍ତୁ CURLE_SSL_CONNECT_ERROR;
  }

  {
    ଅକ୍ଷର cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, ଆକାର(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  ଯଦି(pinnedpubkey) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା size;
    ଅଚଳ mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = ଖାଲି;
    unsigned ଅକ୍ଷର *pubkey = ଖାଲି;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    ଯଦି(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      ଫେରନ୍ତୁ CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, ଆକାର(*p));

    ଯଦି(!p)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    ଯଦି(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    ଯଦି(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      ଯାଅ pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    ଯଦି(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      ଯାଅ pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  ଯଦି(connssl->alpn) {
    ଅଚଳ ଅକ୍ଷର *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ଅଚଳ unsigned ଅକ୍ଷର *)proto,
                                      proto ? strlen(proto) : 0);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode mbed_new_session(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  ସଂରଚନା Curl_ssl_session *sc_session = ଖାଲି;
  unsigned ଅକ୍ଷର *sdata = ଖାଲି;
  size_t slen = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ietf_tls_id;
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

  DEBUGASSERT(backend);
  ଯଦି(!Curl_ssl_scache_use(cf, data))
    ଫେରନ୍ତୁ CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  ଯଦି(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    ଯାଅ out;
  }

  mbedtls_ssl_session_save(&session, ଖାଲି, 0, &slen);
  ଯଦି(!slen) {
    failf(data, "failed to serialize session: length is 0");
    ଯାଅ out;
  }

  sdata = curlx_malloc(slen);
  ଯଦି(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  ଯଦି(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret);
    ଯାଅ out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_ସଂଖ୍ୟା(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = ଖାଲି;  /* call took ownership */
  ଯଦି(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  ଯଦି(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_ଚାବି,
                                 sc_session);
    sc_session = ଖାଲି;
  }

out:
  ଯଦି(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode mbed_send(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                          ଅଚଳ ଶୂନ୍ୟ *mem, size_t len, size_t *pnwritten)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nwritten;

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
  ଯଦି(backend->send_blocked) {
    ଯଦି(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      ଯାଅ out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ଅଚଳ unsigned ଅକ୍ଷର *)mem, len);

  ଯଦି(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  ଅନ୍ୟଥା {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-nwritten);
    ବାଛ(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ସ୍ଥିତି MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      ଯଦି(!result)
        ଯାଅ do_send;
      ଭାଙ୍ଗ;
#endif
    ସ୍ଥିତି MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      result = CURLE_SEND_ERROR;
      ଭାଙ୍ଗ;
    }
    ଯଦି((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *pnwritten);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode mbedtls_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  unsigned ଅକ୍ଷର buf[1024];
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  ଯଦି(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  ଯଦି(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    ଯଦି(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      ବାଛ(ret) {
      ସ୍ଥିତି 0: /* we sent it, receive from the server */
        ଭାଙ୍ଗ;
      ସ୍ଥିତି MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        ଯାଅ out;
      ସ୍ଥିତି MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        ଯାଅ out;
      ସ୍ଥିତି MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        ଯାଅ out;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret);
        result = CURLE_RECV_ERROR;
        ଯାଅ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  ପାଇଁ(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, ଆକାର(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    ଯଦି(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, ଆକାର(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ଯଦି(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      ଜାରି;
#endif
    ଯଦି(ret <= 0)
      ଭାଙ୍ଗ;
  }

  ଯଦି(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  ଅନ୍ୟଥା ଯଦି(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  ଅନ୍ୟଥା ଯଦି(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  ଅନ୍ୟଥା {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ mbedtls_close(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  ଯଦି(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

ସ୍ଥିର CURLcode mbed_recv(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                          ଅକ୍ଷର *buf, size_t buffersize, size_t *pnread)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned ଅକ୍ଷର *)buf, buffersize);
  ଯଦି(nread > 0)
    *pnread = (size_t)nread;
  ଅନ୍ୟଥା {
    ଅକ୍ଷର errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-nread);
    ବାଛ(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    ସ୍ଥିତି MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      ଯଦି(!result)
        ଯାଅ do_read;
      ଭାଙ୍ଗ;
#endif
    ସ୍ଥିତି MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      mbedtls_strerror(nread, errorbuf, ଆକାର(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର size_t mbedtls_version(ଅକ୍ଷର *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା version = mbedtls_version_get_ସଂଖ୍ୟା();
  ଫେରନ୍ତୁ curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
ସ୍ଥିର CURLcode mbedtls_random(ସଂରଚନା Curl_easy *data,
                               unsigned ଅକ୍ଷର *entropy, size_t length)
{
  psa_status_t status;
  (ଶୂନ୍ୟ)data;

  status = psa_generate_random(entropy, length);

  ଫେରନ୍ତୁ status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ସ୍ଥିର CURLcode mbedtls_connect(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  ଯଦି(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ଯଦି(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    ସଂରଚନା ssl_connect_data *ctx = cf->ctx;
    ସଂରଚନା mbed_ssl_backend_data *backend =
      (ସଂରଚନା mbed_ssl_backend_data *)ctx->backend;

    ଯଦି(mbedtls_ssl_get_version_ସଂଖ୍ୟା(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  ଯଦି(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mbedtls_init(ଶୂନ୍ୟ)
{
#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x04000000
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  ଯଦି(status != PSA_SUCCESS)
    ଫେରନ୍ତୁ 0;

#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              ଖାଲି, 0);

  ଯଦି(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    ଫେରନ୍ତୁ 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ mbedtls_cleanup(ଶୂନ୍ୟ)
{
  mbedtls_psa_crypto_free();

#ଯଦି MBEDTLS_VERSION_ସଂଖ୍ୟା < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

ସ୍ଥିର bool mbedtls_data_pending(ସଂରଚନା Curl_cfilter *cf,
                                 ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *ctx = cf->ctx;
  ସଂରଚନା mbed_ssl_backend_data *backend;

  (ଶୂନ୍ୟ)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ସଂରଚନା mbed_ssl_backend_data *)ctx->backend;
  ଫେରନ୍ତୁ mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

ସ୍ଥିର CURLcode mbedtls_sha256sum(ଅଚଳ unsigned ଅକ୍ଷର *ନିବେଶ,
                                  size_t inputlen,
                                  unsigned ଅକ୍ଷର *sha256sum,
                                  size_t sha256len)
{
#ଯଦି defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, ନିବେଶ, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  ଯଦି(status != PSA_SUCCESS)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  ଫେରନ୍ତୁ CURLE_OK;
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)sha256len;
  ଫେରନ୍ତୁ Curl_sha256it(sha256sum, ନିବେଶ, inputlen);
#endif
}

ସ୍ଥିର ଶୂନ୍ୟ *mbedtls_get_internals(ସଂରଚନା ssl_connect_data *connssl,
                                   CURLINFO info)
{
  ସଂରଚନା mbed_ssl_backend_data *backend =
    (ସଂରଚନା mbed_ssl_backend_data *)connssl->backend;
  (ଶୂନ୍ୟ)info;
  DEBUGASSERT(backend);
  ଫେରନ୍ତୁ &backend->ssl;
}

ଅଚଳ ସଂରଚନା Curl_ssl Curl_ssl_mbedtls = {
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

  ଆକାର(ସଂରଚନା mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  ଖାଲି,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  ଖାଲି,                             /* close_all */
  ଖାଲି,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  ଖାଲି,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
