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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#അന്തര്ഭാവ <mbedtls/version.h>
#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#അന്തര്ഭാവ <psa/crypto_config.h>
#അന്തര്ഭാവ <mbedtls/net_sockets.h>
#അന്തര്ഭാവ <mbedtls/ssl.h>
#അന്തര്ഭാവ <mbedtls/x509.h>
#അന്തര്ഭാവ <mbedtls/psa_util.h>

#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#അന്തര്ഭാവ <mbedtls/error.h>
#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x04000000
#അന്തര്ഭാവ <mbedtls/entropy.h>
#അന്തര്ഭാവ <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#അന്തര്ഭാവ <mbedtls/debug.h>
#endif

#അന്തര്ഭാവ "vtls/cipher_suite.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "vtls/mbedtls.h"
#അന്തര്ഭാവ "vtls/vtls.h"
#അന്തര്ഭാവ "vtls/vtls_int.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "vtls/x509asn1.h"
#അന്തര്ഭാവ "connect.h" /* for the connect timeout */
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curl_sha256.h"

/* ALPN for http2 */
#എങ്കിൽ defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

ഘടന mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  സ്ഥിരം അക്ഷരം *protocols[3];
#endif
  പൂർണ്ണസംഖ്യ *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x04000000
ഘടന rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

സ്ഥിതം ഘടന rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
സ്ഥിതം ശൂന്യം mbed_debug(ശൂന്യം *context, പൂർണ്ണസംഖ്യ level, സ്ഥിരം അക്ഷരം *f_name,
                       പൂർണ്ണസംഖ്യ വരി_nb, സ്ഥിരം അക്ഷരം *വരി)
{
  ഘടന Curl_easy *data = (ഘടന Curl_easy *)context;
  (ശൂന്യം)level;
  (ശൂന്യം)വരി_nb;
  (ശൂന്യം)f_name;

  എങ്കിൽ(data) {
    size_t len = strlen(വരി);
    എങ്കിൽ(len && (വരി[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (പൂർണ്ണസംഖ്യ)len, വരി);
  }
}
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ mbedtls_bio_cf_write(ശൂന്യം *bio,
                                സ്ഥിരം unsigned അക്ഷരം *buf, size_t blen)
{
  ഘടന Curl_cfilter *cf = bio;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  എങ്കിൽ(!data)
    തിരികെ 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (പൂർണ്ണസംഖ്യ)result, nwritten);
  എങ്കിൽ(result == CURLE_AGAIN)
    തിരികെ MBEDTLS_ERR_SSL_WANT_WRITE;
  തിരികെ result ? -1 : (പൂർണ്ണസംഖ്യ)nwritten;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mbedtls_bio_cf_read(ശൂന്യം *bio, unsigned അക്ഷരം *buf, size_t blen)
{
  ഘടന Curl_cfilter *cf = bio;
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  എങ്കിൽ(!data)
    തിരികെ 0;
  /* OpenSSL catches this case, so should we. */
  എങ്കിൽ(!buf)
    തിരികെ 0;

  result = Curl_conn_cf_recv(cf->next, data, (അക്ഷരം *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (പൂർണ്ണസംഖ്യ)result, nread);
  എങ്കിൽ(result == CURLE_AGAIN)
    തിരികെ MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  തിരികെ result ? -1 : (പൂർണ്ണസംഖ്യ)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

സ്ഥിതം CURLcode mbed_set_ssl_version_min_max(
  ഘടന Curl_easy *data,
  ഘടന mbed_ssl_backend_data *backend,
  ഘടന ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#അല്ലെങ്കിൽ
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#അല്ലെങ്കിൽ
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  തിരഞ്ഞെടുപ്പ്(conn_config->version) {
  അവസ്ഥ CURL_SSLVERSION_TLSv1:
  അവസ്ഥ CURL_SSLVERSION_TLSv1_0:
  അവസ്ഥ CURL_SSLVERSION_TLSv1_1:
  അവസ്ഥ CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    നിർത്തുക;
#endif
  അവസ്ഥ CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    നിർത്തുക;
#endif
  സ്ഥിരസ്ഥിതി:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  തിരഞ്ഞെടുപ്പ്(conn_config->version_max) {
  അവസ്ഥ CURL_SSLVERSION_MAX_DEFAULT:
  അവസ്ഥ CURL_SSLVERSION_MAX_NONE:
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    നിർത്തുക;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    നിർത്തുക;
#endif
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_1:
  അവസ്ഥ CURL_SSLVERSION_MAX_TLSv1_0:
  സ്ഥിരസ്ഥിതി:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  തിരികെ CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
സ്ഥിതം പൂർണ്ണസംഖ്യ mbed_cipher_suite_get_str(uint16_t id, അക്ഷരം *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  എങ്കിൽ(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  അല്ലെങ്കിൽ
    തിരികെ Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  തിരികെ 0;
}

സ്ഥിതം uint16_t mbed_cipher_suite_walk_str(സ്ഥിരം അക്ഷരം **str, സ്ഥിരം അക്ഷരം **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  സ്ഥിതം സ്ഥിരം അക്ഷരം ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  എങ്കിൽ(!id) {
    എങ്കിൽ((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  തിരികെ id;
}
#അല്ലെങ്കിൽ
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

സ്ഥിതം CURLcode mbed_set_selected_ciphers(
  ഘടന Curl_easy *data,
  ഘടന mbed_ssl_backend_data *backend,
  സ്ഥിരം അക്ഷരം *ciphers12,
  സ്ഥിരം അക്ഷരം *ciphers13)
{
  സ്ഥിരം അക്ഷരം *ciphers = ciphers12;
  സ്ഥിരം പൂർണ്ണസംഖ്യ *supported;
  പൂർണ്ണസംഖ്യ *selected;
  size_t supported_len, എണ്ണം = 0, default13_എണ്ണം = 0, i, j;
  സ്ഥിരം അക്ഷരം *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  വേണ്ടി(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(വലുപ്പം(പൂർണ്ണസംഖ്യ) * (supported_len + 1));
  എങ്കിൽ(!selected)
    തിരികെ CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (ശൂന്യം)ciphers13, (ശൂന്യം)j;
#അല്ലെങ്കിൽ
  എങ്കിൽ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    വേണ്ടി(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      എങ്കിൽ(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        തുടരുക;

      selected[എണ്ണം++] = id;
    }

    default13_എണ്ണം = എണ്ണം;
  }
  അല്ലെങ്കിൽ
    ciphers = ciphers13;

add_ciphers:
#endif
  വേണ്ടി(ptr = ciphers; ptr[0] != '\0' && എണ്ണം < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    എങ്കിൽ(id) {
      വേണ്ടി(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      എങ്കിൽ(i == supported_len)
        id = 0;
    }
    എങ്കിൽ(!id) {
      എങ്കിൽ(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (പൂർണ്ണസംഖ്യ)(end - ptr), ptr);
      തുടരുക;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    വേണ്ടി(i = 0; i < എണ്ണം && selected[i] != id; i++)
      ;
    എങ്കിൽ(i < എണ്ണം) {
      എങ്കിൽ(i >= default13_എണ്ണം)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (പൂർണ്ണസംഖ്യ)(end - ptr), ptr);
      തുടരുക;
    }

    selected[എണ്ണം++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  എങ്കിൽ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    പോകുക add_ciphers;
  }

  എങ്കിൽ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    വേണ്ടി(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      എങ്കിൽ(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        തുടരുക;

      /* No duplicates allowed (so selected cannot overflow) */
      വേണ്ടി(i = 0; i < എണ്ണം && selected[i] != id; i++)
        ;
      എങ്കിൽ(i < എണ്ണം)
        തുടരുക;

      selected[എണ്ണം++] = id;
    }
  }
#endif

  selected[എണ്ണം] = 0;

  എങ്കിൽ(എണ്ണം == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    തിരികെ CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  തിരികെ CURLE_OK;
}

സ്ഥിതം ശൂന്യം mbed_dump_cert_info(ഘടന Curl_easy *data,
                                സ്ഥിരം mbedtls_x509_crt *crt)
{
#എങ്കിൽ !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (ശൂന്യം)data, (ശൂന്യം)crt;
#അല്ലെങ്കിൽ
  സ്ഥിരം size_t bufsize = 16384;
  അക്ഷരം *p, *buffer = curlx_malloc(bufsize);

  എങ്കിൽ(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    വേണ്ടി(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (പൂർണ്ണസംഖ്യ)s, p);
      p += s;
    }
  }
  അല്ലെങ്കിൽ
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

സ്ഥിതം ശൂന്യം mbed_extract_certinfo(ഘടന Curl_easy *data,
                                  സ്ഥിരം mbedtls_x509_crt *crt)
{
  CURLcode result;
  സ്ഥിരം mbedtls_x509_crt *cur;
  പൂർണ്ണസംഖ്യ cert_എണ്ണം = 0;
  പൂർണ്ണസംഖ്യ i;

  വേണ്ടി(cur = crt; cur && cert_എണ്ണം <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_എണ്ണം++;

  എങ്കിൽ(cert_എണ്ണം > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    തിരികെ;
  }

  result = Curl_ssl_init_certinfo(data, cert_എണ്ണം);

  വേണ്ടി(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mbed_verify_cb(ശൂന്യം *ptr, mbedtls_x509_crt *crt,
                          പൂർണ്ണസംഖ്യ depth, uint32_t *flags)
{
  ഘടന Curl_cfilter *cf = (ഘടന Curl_cfilter *)ptr;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന Curl_easy *data = CF_DATA_CURRENT(cf);

  എങ്കിൽ(depth == 0) {
    എങ്കിൽ(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    എങ്കിൽ(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  എങ്കിൽ(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  എങ്കിൽ(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  എങ്കിൽ(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    അക്ഷരം buf[128];
    mbedtls_x509_crt_verify_info(buf, വലുപ്പം(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#അല്ലെങ്കിൽ
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned പൂർണ്ണസംഖ്യ)*flags);
#endif
  }

  തിരികെ 0;
}

സ്ഥിതം CURLcode mbed_load_cacert(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  സ്ഥിരം ഘടന curl_blob *ca_info_blob = conn_config->ca_info_blob;
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ശൂന്യ : conn_config->CAfile);
  സ്ഥിരം bool verifypeer = conn_config->verifypeer;
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_cert_type = conn_config->cert_type;
#endif
  പൂർണ്ണസംഖ്യ ret = -1;
  അക്ഷരം errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  എങ്കിൽ(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    എങ്കിൽ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((അക്ഷരം *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    അല്ലെങ്കിൽ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned അക്ഷരം *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      എങ്കിൽ(!newblob)
        തിരികെ CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#അല്ലെങ്കിൽ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    എങ്കിൽ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);
      തിരികെ CURLE_SSL_CERTPROBLEM;
    }
  }

  എങ്കിൽ(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    എങ്കിൽ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);
      തിരികെ CURLE_SSL_CACERT_BADFILE;
    }
#അല്ലെങ്കിൽ
    failf(data, "mbedTLS: functions that use the file system not built in");
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
  }

  എങ്കിൽ(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    എങ്കിൽ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);

      എങ്കിൽ(verifypeer)
        തിരികെ CURLE_SSL_CACERT_BADFILE;
    }
#അല്ലെങ്കിൽ
    failf(data, "mbedTLS: functions that use the file system not built in");
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode mbed_load_clicert(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  അക്ഷരം * സ്ഥിരം ssl_cert = conn_config->clientcert;
  സ്ഥിരം ഘടന curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_cert_type = conn_config->cert_type;
#endif
  പൂർണ്ണസംഖ്യ ret = -1;
  അക്ഷരം errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  എങ്കിൽ(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    എങ്കിൽ(ret) {
      mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);

      തിരികെ CURLE_SSL_CERTPROBLEM;
    }
#അല്ലെങ്കിൽ
    failf(data, "mbedTLS: functions that use the file system not built in");
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
  }

  എങ്കിൽ(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    എങ്കിൽ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((അക്ഷരം *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    അല്ലെങ്കിൽ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned അക്ഷരം *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      എങ്കിൽ(!newblob)
        തിരികെ CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#അല്ലെങ്കിൽ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    എങ്കിൽ(ret) {
      mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);
      തിരികെ CURLE_SSL_CERTPROBLEM;
    }
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode mbed_load_privkey(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  പൂർണ്ണസംഖ്യ ret = -1;
  അക്ഷരം errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  എങ്കിൽ(conn_config->താക്കോൽ || conn_config->താക്കോൽ_blob) {
    എങ്കിൽ(conn_config->താക്കോൽ) {
#ifdef MBEDTLS_FS_IO
#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->താക്കോൽ,
                                     conn_config->താക്കോൽ_passwd);
      എങ്കിൽ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_താക്കോൽ_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_താക്കോൽ_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#അല്ലെങ്കിൽ
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->താക്കോൽ,
                                     conn_config->താക്കോൽ_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      എങ്കിൽ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      എങ്കിൽ(ret) {
        mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->താക്കോൽ, (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);
        തിരികെ CURLE_SSL_CERTPROBLEM;
      }
#അല്ലെങ്കിൽ
      failf(data, "mbedTLS: functions that use the file system not built in");
      തിരികെ CURLE_NOT_BUILT_IN;
#endif
    }
    അല്ലെങ്കിൽ {
      സ്ഥിരം ഘടന curl_blob *ssl_താക്കോൽ_blob = conn_config->താക്കോൽ_blob;
      സ്ഥിരം അക്ഷരം *passwd = conn_config->താക്കോൽ_passwd;
      സ്ഥിരം bool is_der = conn_config->താക്കോൽ_type &&
                          curl_strequal(conn_config->താക്കോൽ_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned അക്ഷരം *newblob = curlx_memdup0(ssl_താക്കോൽ_blob->data,
                                             ssl_താക്കോൽ_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_താക്കോൽ_blob->len : ssl_താക്കോൽ_blob->len + 1;
      എങ്കിൽ(!newblob)
        തിരികെ CURLE_OUT_OF_MEMORY;

#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ >= 0x04000000
      ret = mbedtls_pk_parse_താക്കോൽ(&backend->pk, newblob, keylen,
                                 (സ്ഥിരം unsigned അക്ഷരം *)passwd,
                                 passwd ? strlen(passwd) : 0);
      എങ്കിൽ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_താക്കോൽ_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_താക്കോൽ_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#അല്ലെങ്കിൽ
      ret = mbedtls_pk_parse_താക്കോൽ(&backend->pk, newblob, keylen,
                                 (സ്ഥിരം unsigned അക്ഷരം *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      എങ്കിൽ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      എങ്കിൽ(ret) {
        mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);
        തിരികെ CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode mbed_load_crl(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  സ്ഥിരം അക്ഷരം * സ്ഥിരം ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  എങ്കിൽ(ssl_crlfile) {
    അക്ഷരം errorbuf[128];
#ifdef MBEDTLS_FS_IO
    പൂർണ്ണസംഖ്യ ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    എങ്കിൽ(ret) {
      mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);

      തിരികെ CURLE_SSL_CRL_BADFILE;
    }
#അല്ലെങ്കിൽ
    (ശൂന്യം)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    തിരികെ CURLE_NOT_BUILT_IN;
#endif
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)backend;
  എങ്കിൽ(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    തിരികെ CURLE_NOT_BUILT_IN;
  }
#endif

  തിരികെ CURLE_OK;
}

സ്ഥിതം bool mbed_apply_session(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന Curl_ssl_session *sc_session)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;

  എങ്കിൽ(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    പൂർണ്ണസംഖ്യ ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    എങ്കിൽ(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned പൂർണ്ണസംഖ്യ)-ret);
    }
    അല്ലെങ്കിൽ {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      എങ്കിൽ(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned പൂർണ്ണസംഖ്യ)-ret);
    }
    mbedtls_ssl_session_free(&session);
    എങ്കിൽ(!ret) {
      infof(data, "SSL reusing session ID");
      തിരികെ TRUE;
    }
  }
  തിരികെ FALSE;
}

സ്ഥിതം CURLcode mbed_configure_ssl(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  പൂർണ്ണസംഖ്യ ret;
  bool session_applied = FALSE;
  CURLcode result;
  അക്ഷരം errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  എങ്കിൽ(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    തിരികെ CURLE_SSL_CONNECT_ERROR;
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

#എങ്കിൽ defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_സംഖ്യ >= 0x03060100 &&   \
  MBEDTLS_VERSION_സംഖ്യ < 0x04000000
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
  എങ്കിൽ(result)
    തിരികെ result;

#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  എങ്കിൽ(ret) {
    mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      ശൂന്യ /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  എങ്കിൽ(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, ശൂന്യ);
#അല്ലെങ്കിൽ
  എങ്കിൽ(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    എങ്കിൽ(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      തിരികെ result;
    }
  }
  അല്ലെങ്കിൽ {
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

  എങ്കിൽ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ഘടന Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    എങ്കിൽ(scs) {
      എങ്കിൽ(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  എങ്കിൽ(!session_applied && Curl_ssl_scache_use(cf, data)) {
    ഘടന Curl_ssl_session *sc_session = ശൂന്യ;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_താക്കോൽ,
                                            &sc_session);
    എങ്കിൽ(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      എങ്കിൽ(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned പൂർണ്ണസംഖ്യ)-ret);
      }
      അല്ലെങ്കിൽ {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        എങ്കിൽ(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned പൂർണ്ണസംഖ്യ)-ret);
        അല്ലെങ്കിൽ
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_താക്കോൽ,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#അല്ലെങ്കിൽ
                            ശൂന്യ
#endif
    );

  എങ്കിൽ(conn_config->താക്കോൽ || conn_config->താക്കോൽ_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  എങ്കിൽ(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  എങ്കിൽ(connssl->alpn) {
    ഘടന alpn_proto_buf proto;
    size_t i;

    വേണ്ടി(i = 0; i < connssl->alpn->എണ്ണം; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    എങ്കിൽ(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      തിരികെ CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  എങ്കിൽ(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    എങ്കിൽ(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  തിരികെ result;
}

സ്ഥിതം CURLcode mbed_connect_step1(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  എങ്കിൽ((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    തിരികെ CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  എങ്കിൽ(!result)
    result = mbed_load_clicert(cf, data);
  എങ്കിൽ(!result)
    result = mbed_load_privkey(cf, data);
  എങ്കിൽ(!result)
    result = mbed_load_crl(cf, data);
  എങ്കിൽ(!result)
    result = mbed_configure_ssl(cf, data);
  എങ്കിൽ(result)
    തിരികെ result;

  connssl->connecting_state = ssl_connect_2;
  തിരികെ CURLE_OK;
}

#എങ്കിൽ defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

സ്ഥിതം CURLcode mbed_connect_step2(ഘടന Curl_cfilter *cf,
                                   ഘടന Curl_easy *data)
{
#എങ്കിൽ defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  പൂർണ്ണസംഖ്യ ret;
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  സ്ഥിരം അക്ഷരം * സ്ഥിരം pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#അല്ലെങ്കിൽ
  സ്ഥിരം അക്ഷരം * സ്ഥിരം pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  എങ്കിൽ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    തിരികെ CURLE_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    തിരികെ CURLE_PEER_FAILED_VERIFICATION;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ret) {
    അക്ഷരം errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_സംഖ്യ(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, വലുപ്പം(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned പൂർണ്ണസംഖ്യ)-ret, errorbuf);
    തിരികെ CURLE_SSL_CONNECT_ERROR;
  }

  {
    അക്ഷരം cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, വലുപ്പം(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  എങ്കിൽ(pinnedpubkey) {
    പൂർണ്ണസംഖ്യ size;
    സ്ഥിരം mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = ശൂന്യ;
    unsigned അക്ഷരം *pubkey = ശൂന്യ;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    എങ്കിൽ(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      തിരികെ CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, വലുപ്പം(*p));

    എങ്കിൽ(!p)
      തിരികെ CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    എങ്കിൽ(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    എങ്കിൽ(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      പോകുക pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    എങ്കിൽ(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      പോകുക pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    എങ്കിൽ(result)
      തിരികെ result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  എങ്കിൽ(connssl->alpn) {
    സ്ഥിരം അക്ഷരം *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (സ്ഥിരം unsigned അക്ഷരം *)proto,
                                      proto ? strlen(proto) : 0);
    എങ്കിൽ(result)
      തിരികെ result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode mbed_new_session(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  ഘടന Curl_ssl_session *sc_session = ശൂന്യ;
  unsigned അക്ഷരം *sdata = ശൂന്യ;
  size_t slen = 0;
  പൂർണ്ണസംഖ്യ ietf_tls_id;
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ ret;

  DEBUGASSERT(backend);
  എങ്കിൽ(!Curl_ssl_scache_use(cf, data))
    തിരികെ CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  എങ്കിൽ(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned പൂർണ്ണസംഖ്യ)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    പോകുക out;
  }

  mbedtls_ssl_session_save(&session, ശൂന്യ, 0, &slen);
  എങ്കിൽ(!slen) {
    failf(data, "failed to serialize session: length is 0");
    പോകുക out;
  }

  sdata = curlx_malloc(slen);
  എങ്കിൽ(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  എങ്കിൽ(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned പൂർണ്ണസംഖ്യ)-ret);
    പോകുക out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_സംഖ്യ(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = ശൂന്യ;  /* call took ownership */
  എങ്കിൽ(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  എങ്കിൽ(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_താക്കോൽ,
                                 sc_session);
    sc_session = ശൂന്യ;
  }

out:
  എങ്കിൽ(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  തിരികെ result;
}

സ്ഥിതം CURLcode mbed_send(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                          സ്ഥിരം ശൂന്യം *mem, size_t len, size_t *pnwritten)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ nwritten;

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
  എങ്കിൽ(backend->send_blocked) {
    എങ്കിൽ(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      പോകുക out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (സ്ഥിരം unsigned അക്ഷരം *)mem, len);

  എങ്കിൽ(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned പൂർണ്ണസംഖ്യ)-nwritten);
    തിരഞ്ഞെടുപ്പ്(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    അവസ്ഥ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      എങ്കിൽ(!result)
        പോകുക do_send;
      നിർത്തുക;
#endif
    അവസ്ഥ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      നിർത്തുക;
    അവസ്ഥ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      result = CURLE_SEND_ERROR;
      നിർത്തുക;
    }
    എങ്കിൽ((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (പൂർണ്ണസംഖ്യ)result, *pnwritten);
  തിരികെ result;
}

സ്ഥിതം CURLcode mbedtls_shutdown(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  unsigned അക്ഷരം buf[1024];
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  എങ്കിൽ(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  എങ്കിൽ(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    എങ്കിൽ(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      തിരഞ്ഞെടുപ്പ്(ret) {
      അവസ്ഥ 0: /* we sent it, receive from the server */
        നിർത്തുക;
      അവസ്ഥ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        പോകുക out;
      അവസ്ഥ MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        പോകുക out;
      അവസ്ഥ MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        പോകുക out;
      സ്ഥിരസ്ഥിതി:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned പൂർണ്ണസംഖ്യ)-ret);
        result = CURLE_RECV_ERROR;
        പോകുക out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  വേണ്ടി(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, വലുപ്പം(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    എങ്കിൽ(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, വലുപ്പം(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    എങ്കിൽ(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      തുടരുക;
#endif
    എങ്കിൽ(ret <= 0)
      നിർത്തുക;
  }

  എങ്കിൽ(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned പൂർണ്ണസംഖ്യ)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം mbedtls_close(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;

  (ശൂന്യം)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  എങ്കിൽ(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

സ്ഥിതം CURLcode mbed_recv(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                          അക്ഷരം *buf, size_t buffersize, size_t *pnread)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  പൂർണ്ണസംഖ്യ nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned അക്ഷരം *)buf, buffersize);
  എങ്കിൽ(nread > 0)
    *pnread = (size_t)nread;
  അല്ലെങ്കിൽ {
    അക്ഷരം errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned പൂർണ്ണസംഖ്യ)-nread);
    തിരഞ്ഞെടുപ്പ്(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    അവസ്ഥ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      എങ്കിൽ(!result)
        പോകുക do_read;
      നിർത്തുക;
#endif
    അവസ്ഥ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      നിർത്തുക;
    അവസ്ഥ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      നിർത്തുക;
    അവസ്ഥ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      mbedtls_strerror(nread, errorbuf, വലുപ്പം(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned പൂർണ്ണസംഖ്യ)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      നിർത്തുക;
    }
  }
  തിരികെ result;
}

സ്ഥിതം size_t mbedtls_version(അക്ഷരം *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned പൂർണ്ണസംഖ്യ version = mbedtls_version_get_സംഖ്യ();
  തിരികെ curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#അല്ലെങ്കിൽ
  തിരികെ curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
സ്ഥിതം CURLcode mbedtls_random(ഘടന Curl_easy *data,
                               unsigned അക്ഷരം *entropy, size_t length)
{
  psa_status_t status;
  (ശൂന്യം)data;

  status = psa_generate_random(entropy, length);

  തിരികെ status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

സ്ഥിതം CURLcode mbedtls_connect(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  ഘടന ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  എങ്കിൽ(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  എങ്കിൽ(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  എങ്കിൽ(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    എങ്കിൽ(result)
      തിരികെ result;
  }

  എങ്കിൽ(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    ഘടന ssl_connect_data *ctx = cf->ctx;
    ഘടന mbed_ssl_backend_data *backend =
      (ഘടന mbed_ssl_backend_data *)ctx->backend;

    എങ്കിൽ(mbedtls_ssl_get_version_സംഖ്യ(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  എങ്കിൽ(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  തിരികെ CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ mbedtls_init(ശൂന്യം)
{
#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x04000000
  പൂർണ്ണസംഖ്യ ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  എങ്കിൽ(status != PSA_SUCCESS)
    തിരികെ 0;

#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              ശൂന്യ, 0);

  എങ്കിൽ(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    തിരികെ 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  തിരികെ 1;
}

സ്ഥിതം ശൂന്യം mbedtls_cleanup(ശൂന്യം)
{
  mbedtls_psa_crypto_free();

#എങ്കിൽ MBEDTLS_VERSION_സംഖ്യ < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

സ്ഥിതം bool mbedtls_data_pending(ഘടന Curl_cfilter *cf,
                                 സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *ctx = cf->ctx;
  ഘടന mbed_ssl_backend_data *backend;

  (ശൂന്യം)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ഘടന mbed_ssl_backend_data *)ctx->backend;
  തിരികെ mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

സ്ഥിതം CURLcode mbedtls_sha256sum(സ്ഥിരം unsigned അക്ഷരം *നിവേശം,
                                  size_t inputlen,
                                  unsigned അക്ഷരം *sha256sum,
                                  size_t sha256len)
{
#എങ്കിൽ defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, നിവേശം, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  എങ്കിൽ(status != PSA_SUCCESS)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  തിരികെ CURLE_OK;
#അല്ലെങ്കിൽ
  (ശൂന്യം)sha256len;
  തിരികെ Curl_sha256it(sha256sum, നിവേശം, inputlen);
#endif
}

സ്ഥിതം ശൂന്യം *mbedtls_get_internals(ഘടന ssl_connect_data *connssl,
                                   CURLINFO info)
{
  ഘടന mbed_ssl_backend_data *backend =
    (ഘടന mbed_ssl_backend_data *)connssl->backend;
  (ശൂന്യം)info;
  DEBUGASSERT(backend);
  തിരികെ &backend->ssl;
}

സ്ഥിരം ഘടന Curl_ssl Curl_ssl_mbedtls = {
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

  വലുപ്പം(ഘടന mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  ശൂന്യ,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  ശൂന്യ,                             /* close_all */
  ശൂന്യ,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  ശൂന്യ,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
