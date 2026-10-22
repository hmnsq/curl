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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#অন্তর্ভাব <mbedtls/version.h>
#যদি MBEDTLS_VERSION_সংখ্যা < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#অন্তর্ভাব <psa/crypto_config.h>
#অন্তর্ভাব <mbedtls/net_sockets.h>
#অন্তর্ভাব <mbedtls/ssl.h>
#অন্তর্ভাব <mbedtls/x509.h>
#অন্তর্ভাব <mbedtls/psa_util.h>

#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#অন্তর্ভাব <mbedtls/error.h>
#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
#অন্তর্ভাব <mbedtls/entropy.h>
#অন্তর্ভাব <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#অন্তর্ভাব <mbedtls/debug.h>
#endif

#অন্তর্ভাব "vtls/cipher_suite.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "vtls/mbedtls.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vtls/x509asn1.h"
#অন্তর্ভাব "connect.h" /* for the connect timeout */
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curl_sha256.h"

/* ALPN for http2 */
#যদি defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

কাঠামো mbed_ssl_backend_তথ্য {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  ধ্রুবক অক্ষর *protocols[3];
#endif
  পূর্ণসংখ্যা *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
কাঠামো rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

স্থির কাঠামো rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
স্থির শূন্য mbed_debug(শূন্য *context, পূর্ণসংখ্যা level, ধ্রুবক অক্ষর *f_name,
                       পূর্ণসংখ্যা সারি_nb, ধ্রুবক অক্ষর *সারি)
{
  কাঠামো Curl_easy *তথ্য = (কাঠামো Curl_easy *)context;
  (শূন্য)level;
  (শূন্য)সারি_nb;
  (শূন্য)f_name;

  যদি(তথ্য) {
    size_t len = strlen(সারি);
    যদি(len && (সারি[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(তথ্য, "%.*s", (পূর্ণসংখ্যা)len, সারি);
  }
}
#endif

স্থির পূর্ণসংখ্যা mbedtls_bio_cf_write(শূন্য *bio,
                                ধ্রুবক unsigned অক্ষর *buf, size_t blen)
{
  কাঠামো Curl_cfilter *cf = bio;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  size_t nwritten;
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য);
  যদি(!তথ্য)
    ফেরত 0;

  ফলাফল = Curl_conn_cf_send(cf->পরবর্তী, তথ্য, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(তথ্য, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (পূর্ণসংখ্যা)ফলাফল, nwritten);
  যদি(ফলাফল == CURLE_AGAIN)
    ফেরত MBEDTLS_ERR_SSL_WANT_WRITE;
  ফেরত ফলাফল ? -1 : (পূর্ণসংখ্যা)nwritten;
}

স্থির পূর্ণসংখ্যা mbedtls_bio_cf_read(শূন্য *bio, unsigned অক্ষর *buf, size_t blen)
{
  কাঠামো Curl_cfilter *cf = bio;
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);
  size_t nread = 0;
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য);
  যদি(!তথ্য)
    ফেরত 0;
  /* OpenSSL catches this case, so should we. */
  যদি(!buf)
    ফেরত 0;

  ফলাফল = Curl_conn_cf_recv(cf->পরবর্তী, তথ্য, (অক্ষর *)buf, blen, &nread);
  CURL_TRC_CF(তথ্য, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (পূর্ণসংখ্যা)ফলাফল, nread);
  যদি(ফলাফল == CURLE_AGAIN)
    ফেরত MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  ফেরত ফলাফল ? -1 : (পূর্ণসংখ্যা)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_আকার))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

স্থির CURLcode mbed_set_ssl_version_min_max(
  কাঠামো Curl_easy *তথ্য,
  কাঠামো mbed_ssl_backend_তথ্য *backend,
  কাঠামো ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#অন্যথা
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#অন্যথা
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  নির্বাচন(conn_config->version) {
  ক্ষেত্র CURL_SSLVERSION_TLSv1:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_0:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_1:
  ক্ষেত্র CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    বিরতি;
#endif
  ক্ষেত্র CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    বিরতি;
#endif
  পূর্বনির্ধারিত:
    failf(তথ্য, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  নির্বাচন(conn_config->version_max) {
  ক্ষেত্র CURL_SSLVERSION_MAX_DEFAULT:
  ক্ষেত্র CURL_SSLVERSION_MAX_NONE:
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    বিরতি;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    বিরতি;
#endif
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_1:
  ক্ষেত্র CURL_SSLVERSION_MAX_TLSv1_0:
  পূর্বনির্ধারিত:
    failf(তথ্য, "mbedTLS: unsupported maximum TLS version value");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  ফেরত CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
স্থির পূর্ণসংখ্যা mbed_cipher_suite_get_str(uint16_t id, অক্ষর *buf, size_t buf_আকার,
                                     bool prefer_rfc)
{
  যদি(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_আকার, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  অন্যথা
    ফেরত Curl_cipher_suite_get_str(id, buf, buf_আকার, prefer_rfc);
  ফেরত 0;
}

স্থির uint16_t mbed_cipher_suite_walk_str(ধ্রুবক অক্ষর **str, ধ্রুবক অক্ষর **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  স্থির ধ্রুবক অক্ষর ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  যদি(!id) {
    যদি((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  ফেরত id;
}
#অন্যথা
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

স্থির CURLcode mbed_set_selected_ciphers(
  কাঠামো Curl_easy *তথ্য,
  কাঠামো mbed_ssl_backend_তথ্য *backend,
  ধ্রুবক অক্ষর *ciphers12,
  ধ্রুবক অক্ষর *ciphers13)
{
  ধ্রুবক অক্ষর *ciphers = ciphers12;
  ধ্রুবক পূর্ণসংখ্যা *supported;
  পূর্ণসংখ্যা *selected;
  size_t supported_len, গুনতি = 0, default13_গুনতি = 0, i, j;
  ধ্রুবক অক্ষর *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  জন্য(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(আকার(পূর্ণসংখ্যা) * (supported_len + 1));
  যদি(!selected)
    ফেরত CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (শূন্য)ciphers13, (শূন্য)j;
#অন্যথা
  যদি(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    জন্য(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      যদি(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        চালু;

      selected[গুনতি++] = id;
    }

    default13_গুনতি = গুনতি;
  }
  অন্যথা
    ciphers = ciphers13;

add_ciphers:
#endif
  জন্য(ptr = ciphers; ptr[0] != '\0' && গুনতি < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    যদি(id) {
      জন্য(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      যদি(i == supported_len)
        id = 0;
    }
    যদি(!id) {
      যদি(ptr[0] != '\0')
        infof(তথ্য, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (পূর্ণসংখ্যা)(end - ptr), ptr);
      চালু;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    জন্য(i = 0; i < গুনতি && selected[i] != id; i++)
      ;
    যদি(i < গুনতি) {
      যদি(i >= default13_গুনতি)
        infof(তথ্য, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (পূর্ণসংখ্যা)(end - ptr), ptr);
      চালু;
    }

    selected[গুনতি++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  যদি(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    যাও add_ciphers;
  }

  যদি(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    জন্য(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      যদি(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        চালু;

      /* No duplicates allowed (so selected cannot overflow) */
      জন্য(i = 0; i < গুনতি && selected[i] != id; i++)
        ;
      যদি(i < গুনতি)
        চালু;

      selected[গুনতি++] = id;
    }
  }
#endif

  selected[গুনতি] = 0;

  যদি(গুনতি == 0) {
    curlx_free(selected);
    failf(তথ্য, "mbedTLS: no supported cipher in list");
    ফেরত CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  ফেরত CURLE_OK;
}

স্থির শূন্য mbed_dump_cert_info(কাঠামো Curl_easy *তথ্য,
                                ধ্রুবক mbedtls_x509_crt *crt)
{
#যদি !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (শূন্য)তথ্য, (শূন্য)crt;
#অন্যথা
  ধ্রুবক size_t bufsize = 16384;
  অক্ষর *p, *বাফার = curlx_malloc(bufsize);

  যদি(বাফার && mbedtls_x509_crt_info(বাফার, bufsize, " ", crt) > 0) {
    infof(তথ্য, "Server certificate:");
    জন্য(p = বাফার; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(তথ্য, "%.*s", (পূর্ণসংখ্যা)s, p);
      p += s;
    }
  }
  অন্যথা
    infof(তথ্য, "Unable to dump certificate information");

  curlx_free(বাফার);
#endif
}

স্থির শূন্য mbed_extract_certinfo(কাঠামো Curl_easy *তথ্য,
                                  ধ্রুবক mbedtls_x509_crt *crt)
{
  CURLcode ফলাফল;
  ধ্রুবক mbedtls_x509_crt *cur;
  পূর্ণসংখ্যা cert_গুনতি = 0;
  পূর্ণসংখ্যা i;

  জন্য(cur = crt; cur && cert_গুনতি <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->পরবর্তী)
    cert_গুনতি++;

  যদি(cert_গুনতি > MAX_ALLOWED_CERT_AMOUNT) {
    infof(তথ্য, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    ফেরত;
  }

  ফলাফল = Curl_ssl_init_certinfo(তথ্য, cert_গুনতি);

  জন্য(i = 0, cur = crt; ফলাফল == CURLE_OK && cur; ++i, cur = cur->পরবর্তী) {
    ফলাফল = Curl_extract_certinfo(তথ্য, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

স্থির পূর্ণসংখ্যা mbed_verify_cb(শূন্য *ptr, mbedtls_x509_crt *crt,
                          পূর্ণসংখ্যা depth, uint32_t *flags)
{
  কাঠামো Curl_cfilter *cf = (কাঠামো Curl_cfilter *)ptr;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো Curl_easy *তথ্য = CF_তথ্য_CURRENT(cf);

  যদি(depth == 0) {
    যদি(তথ্য->set.verbose)
      mbed_dump_cert_info(তথ্য, crt);
    যদি(তথ্য->set.ssl.certinfo)
      mbed_extract_certinfo(তথ্য, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  যদি(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  যদি(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  যদি(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    অক্ষর buf[128];
    mbedtls_x509_crt_verify_info(buf, আকার(buf), "", *flags);
    failf(তথ্য, "mbedTLS: %s", buf);
#অন্যথা
    failf(তথ্য, "mbedTLS: certificate verification error 0x%08x",
          (unsigned পূর্ণসংখ্যা)*flags);
#endif
  }

  ফেরত 0;
}

স্থির CURLcode mbed_load_cacert(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ধ্রুবক কাঠামো curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ধ্রুবক অক্ষর * ধ্রুবক ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ফাঁকা : conn_config->CAfile);
  ধ্রুবক bool verifypeer = conn_config->verifypeer;
  ধ্রুবক অক্ষর * ধ্রুবক ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  ধ্রুবক অক্ষর * ধ্রুবক ssl_cert_type = conn_config->cert_type;
#endif
  পূর্ণসংখ্যা ret = -1;
  অক্ষর errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  যদি(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    যদি((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((অক্ষর *)(ca_info_blob->তথ্য))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->তথ্য,
                                   ca_info_blob->len);
    }
    অন্যথা { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned অক্ষর *newblob = curlx_memdup0(ca_info_blob->তথ্য,
                                             ca_info_blob->len);
      যদি(!newblob)
        ফেরত CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#অন্যথা
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->তথ্য,
                                     ca_info_blob->len);
#endif

    যদি(ret < 0) {
      mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
      failf(তথ্য, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned পূর্ণসংখ্যা)-ret, errorbuf);
      ফেরত CURLE_SSL_CERTPROBLEM;
    }
  }

  যদি(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    যদি(ret < 0) {
      mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
      failf(তথ্য, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned পূর্ণসংখ্যা)-ret, errorbuf);
      ফেরত CURLE_SSL_CACERT_BADFILE;
    }
#অন্যথা
    failf(তথ্য, "mbedTLS: functions that use the file system not built in");
    ফেরত CURLE_NOT_BUILT_IN;
#endif
  }

  যদি(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    যদি(ret < 0) {
      mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
      failf(তথ্য, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned পূর্ণসংখ্যা)-ret, errorbuf);

      যদি(verifypeer)
        ফেরত CURLE_SSL_CACERT_BADFILE;
    }
#অন্যথা
    failf(তথ্য, "mbedTLS: functions that use the file system not built in");
    ফেরত CURLE_NOT_BUILT_IN;
#endif
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode mbed_load_clicert(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  অক্ষর * ধ্রুবক ssl_cert = conn_config->clientcert;
  ধ্রুবক কাঠামো curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  ধ্রুবক অক্ষর * ধ্রুবক ssl_cert_type = conn_config->cert_type;
#endif
  পূর্ণসংখ্যা ret = -1;
  অক্ষর errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  যদি(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    যদি(ret) {
      mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
      failf(তথ্য, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned পূর্ণসংখ্যা)-ret, errorbuf);

      ফেরত CURLE_SSL_CERTPROBLEM;
    }
#অন্যথা
    failf(তথ্য, "mbedTLS: functions that use the file system not built in");
    ফেরত CURLE_NOT_BUILT_IN;
#endif
  }

  যদি(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    যদি((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((অক্ষর *)(ssl_cert_blob->তথ্য))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->তথ্য,
                                   ssl_cert_blob->len);
    }
    অন্যথা { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned অক্ষর *newblob = curlx_memdup0(ssl_cert_blob->তথ্য,
                                             ssl_cert_blob->len);
      যদি(!newblob)
        ফেরত CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#অন্যথা
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->তথ্য,
                                     ssl_cert_blob->len);
#endif

    যদি(ret) {
      mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
      failf(তথ্য, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned পূর্ণসংখ্যা)-ret, errorbuf);
      ফেরত CURLE_SSL_CERTPROBLEM;
    }
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode mbed_load_privkey(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  পূর্ণসংখ্যা ret = -1;
  অক্ষর errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  যদি(conn_config->চাবি || conn_config->চাবি_blob) {
    যদি(conn_config->চাবি) {
#ifdef MBEDTLS_FS_IO
#যদি MBEDTLS_VERSION_সংখ্যা >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->চাবি,
                                     conn_config->চাবি_passwd);
      যদি(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_চাবি_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_চাবি_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#অন্যথা
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->চাবি,
                                     conn_config->চাবি_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      যদি(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      যদি(ret) {
        mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
        failf(তথ্য, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->চাবি, (unsigned পূর্ণসংখ্যা)-ret, errorbuf);
        ফেরত CURLE_SSL_CERTPROBLEM;
      }
#অন্যথা
      failf(তথ্য, "mbedTLS: functions that use the file system not built in");
      ফেরত CURLE_NOT_BUILT_IN;
#endif
    }
    অন্যথা {
      ধ্রুবক কাঠামো curl_blob *ssl_চাবি_blob = conn_config->চাবি_blob;
      ধ্রুবক অক্ষর *passwd = conn_config->চাবি_passwd;
      ধ্রুবক bool is_der = conn_config->চাবি_type &&
                          curl_strequal(conn_config->চাবি_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned অক্ষর *newblob = curlx_memdup0(ssl_চাবি_blob->তথ্য,
                                             ssl_চাবি_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_চাবি_blob->len : ssl_চাবি_blob->len + 1;
      যদি(!newblob)
        ফেরত CURLE_OUT_OF_MEMORY;

#যদি MBEDTLS_VERSION_সংখ্যা >= 0x04000000
      ret = mbedtls_pk_parse_চাবি(&backend->pk, newblob, keylen,
                                 (ধ্রুবক unsigned অক্ষর *)passwd,
                                 passwd ? strlen(passwd) : 0);
      যদি(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_চাবি_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_চাবি_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#অন্যথা
      ret = mbedtls_pk_parse_চাবি(&backend->pk, newblob, keylen,
                                 (ধ্রুবক unsigned অক্ষর *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      যদি(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      যদি(ret) {
        mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
        failf(তথ্য, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned পূর্ণসংখ্যা)-ret, errorbuf);
        ফেরত CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  ফেরত CURLE_OK;
}

স্থির CURLcode mbed_load_crl(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ধ্রুবক অক্ষর * ধ্রুবক ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  যদি(ssl_crlfile) {
    অক্ষর errorbuf[128];
#ifdef MBEDTLS_FS_IO
    পূর্ণসংখ্যা ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    যদি(ret) {
      mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
      failf(তথ্য, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned পূর্ণসংখ্যা)-ret, errorbuf);

      ফেরত CURLE_SSL_CRL_BADFILE;
    }
#অন্যথা
    (শূন্য)errorbuf;
    failf(তথ্য, "mbedTLS: functions that use the file system not built in");
    ফেরত CURLE_NOT_BUILT_IN;
#endif
  }
#অন্যথা
  (শূন্য)backend;
  যদি(ssl_crlfile) {
    failf(তথ্য, "mbedTLS: CRL support not built in");
    ফেরত CURLE_NOT_BUILT_IN;
  }
#endif

  ফেরত CURLE_OK;
}

স্থির bool mbed_apply_session(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো Curl_ssl_session *sc_session)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;

  যদি(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    পূর্ণসংখ্যা ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    যদি(ret) {
      failf(তথ্য, "SSL session error loading: -0x%x", (unsigned পূর্ণসংখ্যা)-ret);
    }
    অন্যথা {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      যদি(ret)
        failf(তথ্য, "SSL session error setting: -0x%x", (unsigned পূর্ণসংখ্যা)-ret);
    }
    mbedtls_ssl_session_free(&session);
    যদি(!ret) {
      infof(তথ্য, "SSL reusing session ID");
      ফেরত TRUE;
    }
  }
  ফেরত FALSE;
}

স্থির CURLcode mbed_configure_ssl(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  পূর্ণসংখ্যা ret;
  bool session_applied = FALSE;
  CURLcode ফলাফল;
  অক্ষর errorbuf[128];

  infof(তথ্য, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  যদি(ret) {
    failf(তথ্য, "mbedTLS: ssl_config failed");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

#ifdef MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, তথ্য);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_set_threshold(4);
#endif

#যদি defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_সংখ্যা >= 0x03060100 &&   \
  MBEDTLS_VERSION_সংখ্যা < 0x04000000
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
                                &mbedtls_x509_crt_profile_পরবর্তী);

  ফলাফল = mbed_set_ssl_version_min_max(তথ্য, backend, conn_config);
  যদি(ফলাফল)
    ফেরত ফলাফল;

#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  যদি(ret) {
    mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
    failf(তথ্য, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned পূর্ণসংখ্যা)-ret, errorbuf);
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      ফাঁকা /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  যদি(conn_config->cipher_list) {
    ফলাফল = mbed_set_selected_ciphers(তথ্য, backend,
                                       conn_config->cipher_list, ফাঁকা);
#অন্যথা
  যদি(conn_config->cipher_list || conn_config->cipher_list13) {
    ফলাফল = mbed_set_selected_ciphers(তথ্য, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    যদি(ফলাফল) {
      failf(তথ্য, "mbedTLS: failed to set cipher suites");
      ফেরত ফলাফল;
    }
  }
  অন্যথা {
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

  যদি((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    কাঠামো Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(তথ্য, cf->cft, FIRSTSOCKET);
    যদি(scs) {
      যদি(mbed_apply_session(cf, তথ্য, scs)) {
        CURL_TRC_CF(তথ্য, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  যদি(!session_applied && Curl_ssl_scache_use(cf, তথ্য)) {
    কাঠামো Curl_ssl_session *sc_session = ফাঁকা;
    CURLcode sresult = Curl_ssl_scache_take(cf, তথ্য, connssl->peer.scache_চাবি,
                                            &sc_session);
    যদি(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      যদি(ret) {
        failf(তথ্য, "SSL session error loading: -0x%x", (unsigned পূর্ণসংখ্যা)-ret);
      }
      অন্যথা {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        যদি(ret)
          failf(তথ্য, "SSL session error setting: -0x%x", (unsigned পূর্ণসংখ্যা)-ret);
        অন্যথা
          infof(তথ্য, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, তথ্য, connssl->peer.scache_চাবি,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#অন্যথা
                            ফাঁকা
#endif
    );

  যদি(conn_config->চাবি || conn_config->চাবি_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  যদি(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(তথ্য, "Failed to set SNI");
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  যদি(connssl->alpn) {
    কাঠামো alpn_proto_buf proto;
    size_t i;

    জন্য(i = 0; i < connssl->alpn->গুনতি; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    যদি(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(তথ্য, "Failed setting ALPN protocols");
      ফেরত CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(তথ্য, VTLS_INFOF_ALPN_OFFER_1STR, proto.তথ্য);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  যদি(তথ্য->set.ssl_fsslctx) {
    ফলাফল = (*তথ্য->set.ssl_fsslctx)(তথ্য, &backend->config,
                                      তথ্য->set.ssl_fsslctxp);
    যদি(ফলাফল)
      failf(তথ্য, "error signaled by SSL ctx callback");
  }

  ফেরত ফলাফল;
}

স্থির CURLcode mbed_connect_step1(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ফলাফল;

  যদি((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(তথ্য, "Not supported SSL version");
    ফেরত CURLE_NOT_BUILT_IN;
  }

  ফলাফল = mbed_load_cacert(cf, তথ্য);
  যদি(!ফলাফল)
    ফলাফল = mbed_load_clicert(cf, তথ্য);
  যদি(!ফলাফল)
    ফলাফল = mbed_load_privkey(cf, তথ্য);
  যদি(!ফলাফল)
    ফলাফল = mbed_load_crl(cf, তথ্য);
  যদি(!ফলাফল)
    ফলাফল = mbed_configure_ssl(cf, তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  connssl->connecting_state = ssl_connect_2;
  ফেরত CURLE_OK;
}

#যদি defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

স্থির CURLcode mbed_connect_step2(কাঠামো Curl_cfilter *cf,
                                   কাঠামো Curl_easy *তথ্য)
{
#যদি defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode ফলাফল;
#endif
  পূর্ণসংখ্যা ret;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  ধ্রুবক অক্ষর * ধ্রুবক pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY);
#অন্যথা
  ধ্রুবক অক্ষর * ধ্রুবক pinnedpubkey =
    CURL_EASY_STR(তথ্য, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  যদি(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    ফেরত CURLE_OK;
  }
  অন্যথা যদি(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    ফেরত CURLE_OK;
  }
  অন্যথা যদি(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(তথ্য, "peer certificate could not be verified");
    ফেরত CURLE_PEER_FAILED_VERIFICATION;
  }
  অন্যথা যদি(ret) {
    অক্ষর errorbuf[128];
    CURL_TRC_CF(তথ্য, cf, "TLS version %04X",
                mbedtls_ssl_get_version_সংখ্যা(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, আকার(errorbuf));
    failf(তথ্য, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned পূর্ণসংখ্যা)-ret, errorbuf);
    ফেরত CURLE_SSL_CONNECT_ERROR;
  }

  {
    অক্ষর cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, আকার(cipher_str), TRUE);
    infof(তথ্য, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  যদি(pinnedpubkey) {
    পূর্ণসংখ্যা আকার_2;
    ধ্রুবক mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = ফাঁকা;
    unsigned অক্ষর *pubkey = ফাঁকা;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    যদি(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(তথ্য, "Failed due to missing peer certificate");
      ফেরত CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, আকার(*p));

    যদি(!p)
      ফেরত CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    যদি(!pubkey) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    যদি(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(তথ্য, "Failed copying peer certificate");
      ফলাফল = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      যাও pinnedpubkey_error;
    }

    আকার_2 = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    যদি(আকার_2 <= 0) {
      failf(তথ্য, "Failed copying public key from peer certificate");
      ফলাফল = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      যাও pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    ফলাফল = Curl_pin_peer_pubkey(তথ্য,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - আকার_2], আকার_2);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  যদি(connssl->alpn) {
    ধ্রুবক অক্ষর *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    ফলাফল = Curl_alpn_set_negotiated(cf, তথ্য, connssl,
                                      (ধ্রুবক unsigned অক্ষর *)proto,
                                      proto ? strlen(proto) : 0);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(তথ্য, "SSL connected");

  ফেরত CURLE_OK;
}

স্থির CURLcode mbed_new_session(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  কাঠামো Curl_ssl_session *sc_session = ফাঁকা;
  unsigned অক্ষর *sdata = ফাঁকা;
  size_t slen = 0;
  পূর্ণসংখ্যা ietf_tls_id;
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা ret;

  DEBUGASSERT(backend);
  যদি(!Curl_ssl_scache_use(cf, তথ্য))
    ফেরত CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  যদি(ret) {
    failf(তথ্য, "mbedtls_ssl_get_session returned -0x%x", (unsigned পূর্ণসংখ্যা)-ret);
    ফলাফল = CURLE_SSL_CONNECT_ERROR;
    যাও out;
  }

  mbedtls_ssl_session_save(&session, ফাঁকা, 0, &slen);
  যদি(!slen) {
    failf(তথ্য, "failed to serialize session: length is 0");
    যাও out;
  }

  sdata = curlx_malloc(slen);
  যদি(!sdata) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  যদি(ret) {
    failf(তথ্য, "failed to serialize session: -0x%x", (unsigned পূর্ণসংখ্যা)-ret);
    যাও out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_সংখ্যা(&backend->ssl);
  ফলাফল = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = ফাঁকা;  /* call took ownership */
  যদি(!ফলাফল && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    ফলাফল = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  যদি(!ফলাফল) {
    ফলাফল = Curl_ssl_scache_put(cf, তথ্য, connssl->peer.scache_চাবি,
                                 sc_session);
    sc_session = ফাঁকা;
  }

out:
  যদি(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  ফেরত ফলাফল;
}

স্থির CURLcode mbed_send(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                          ধ্রুবক শূন্য *mem, size_t len, size_t *pnwritten)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা nwritten;

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
  যদি(backend->send_blocked) {
    যদি(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(তথ্য, "re-send data shorter than previously");
      ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
      যাও out;
    }
    CURL_TRC_CF(তথ্য, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ধ্রুবক unsigned অক্ষর *)mem, len);

  যদি(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  অন্যথা {
    CURL_TRC_CF(তথ্য, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned পূর্ণসংখ্যা)-nwritten);
    নির্বাচন(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ক্ষেত্র MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      ফলাফল = mbed_new_session(cf, তথ্য);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      যদি(!ফলাফল)
        যাও do_send;
      বিরতি;
#endif
    ক্ষেত্র MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ফলাফল = CURLE_AGAIN;
      বিরতি;
    ক্ষেত্র MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ফলাফল = CURLE_AGAIN;
      বিরতি;
    পূর্বনির্ধারিত:
      ফলাফল = CURLE_SEND_ERROR;
      বিরতি;
    }
    যদি((ফলাফল == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(তথ্য, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (পূর্ণসংখ্যা)ফলাফল, *pnwritten);
  ফেরত ফলাফল;
}

স্থির CURLcode mbedtls_shutdown(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 bool send_shutdown, bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  unsigned অক্ষর buf[1024];
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  যদি(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  যদি(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    যদি(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      নির্বাচন(ret) {
      ক্ষেত্র 0: /* we sent it, receive from the server */
        বিরতি;
      ক্ষেত্র MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        যাও out;
      ক্ষেত্র MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        যাও out;
      ক্ষেত্র MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        যাও out;
      পূর্বনির্ধারিত:
        CURL_TRC_CF(তথ্য, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned পূর্ণসংখ্যা)-ret);
        ফলাফল = CURLE_RECV_ERROR;
        যাও out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  জন্য(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, আকার(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    যদি(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, আকার(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    যদি(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      চালু;
#endif
    যদি(ret <= 0)
      বিরতি;
  }

  যদি(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(তথ্য, cf, "mbedtls_shutdown, still getting data");
  }
  অন্যথা যদি(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(তথ্য, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  অন্যথা যদি(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(তথ্য, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  অন্যথা যদি(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(তথ্য, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  অন্যথা {
    CURL_TRC_CF(তথ্য, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned পূর্ণসংখ্যা)-ret);
    ফলাফল = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (ফলাফল || *done);
  ফেরত ফলাফল;
}

স্থির শূন্য mbedtls_close(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;

  (শূন্য)তথ্য;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  যদি(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

স্থির CURLcode mbed_recv(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                          অক্ষর *buf, size_t buffersize, size_t *pnread)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  CURLcode ফলাফল = CURLE_OK;
  পূর্ণসংখ্যা nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned অক্ষর *)buf, buffersize);
  যদি(nread > 0)
    *pnread = (size_t)nread;
  অন্যথা {
    অক্ষর errorbuf[128];
    CURL_TRC_CF(তথ্য, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned পূর্ণসংখ্যা)-nread);
    নির্বাচন(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    ক্ষেত্র MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      ফলাফল = mbed_new_session(cf, তথ্য);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      যদি(!ফলাফল)
        যাও do_read;
      বিরতি;
#endif
    ক্ষেত্র MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ফলাফল = CURLE_AGAIN;
      বিরতি;
    ক্ষেত্র MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ফলাফল = CURLE_AGAIN;
      বিরতি;
    ক্ষেত্র MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      ফলাফল = CURLE_OK;
      বিরতি;
    পূর্বনির্ধারিত:
      mbedtls_strerror(nread, errorbuf, আকার(errorbuf));
      failf(তথ্য, "ssl_read returned: (-0x%04X) %s", (unsigned পূর্ণসংখ্যা)-nread,
            errorbuf);
      ফলাফল = CURLE_RECV_ERROR;
      বিরতি;
    }
  }
  ফেরত ফলাফল;
}

স্থির size_t mbedtls_version(অক্ষর *বাফার, size_t আকার_2)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned পূর্ণসংখ্যা version = mbedtls_version_get_সংখ্যা();
  ফেরত curl_msnprintf(বাফার, আকার_2, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#অন্যথা
  ফেরত curl_msnprintf(বাফার, আকার_2, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
স্থির CURLcode mbedtls_random(কাঠামো Curl_easy *তথ্য,
                               unsigned অক্ষর *entropy, size_t দৈর্ঘ্য)
{
  psa_status_t status;
  (শূন্য)তথ্য;

  status = psa_generate_random(entropy, দৈর্ঘ্য);

  ফেরত status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

স্থির CURLcode mbedtls_connect(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                bool *done)
{
  CURLcode ফলাফল;
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;

  /* check if the connection has already been established */
  যদি(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  যদি(connssl->connecting_state == ssl_connect_1) {
    ফলাফল = mbed_connect_step1(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(connssl->connecting_state == ssl_connect_2) {
    ফলাফল = mbed_connect_step2(cf, তথ্য);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }

  যদি(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    কাঠামো ssl_connect_তথ্য *ctx = cf->ctx;
    কাঠামো mbed_ssl_backend_তথ্য *backend =
      (কাঠামো mbed_ssl_backend_তথ্য *)ctx->backend;

    যদি(mbedtls_ssl_get_version_সংখ্যা(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      ফলাফল = mbed_new_session(cf, তথ্য);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  যদি(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  ফেরত CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
স্থির পূর্ণসংখ্যা mbedtls_init(শূন্য)
{
#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
  পূর্ণসংখ্যা ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  যদি(status != PSA_SUCCESS)
    ফেরত 0;

#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              ফাঁকা, 0);

  যদি(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    ফেরত 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  ফেরত 1;
}

স্থির শূন্য mbedtls_cleanup(শূন্য)
{
  mbedtls_psa_crypto_free();

#যদি MBEDTLS_VERSION_সংখ্যা < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

স্থির bool mbedtls_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                                 ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *ctx = cf->ctx;
  কাঠামো mbed_ssl_backend_তথ্য *backend;

  (শূন্য)তথ্য;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (কাঠামো mbed_ssl_backend_তথ্য *)ctx->backend;
  ফেরত mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

স্থির CURLcode mbedtls_sha256sum(ধ্রুবক unsigned অক্ষর *প্রবেশ,
                                  size_t inputlen,
                                  unsigned অক্ষর *sha256sum,
                                  size_t sha256len)
{
#যদি defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, প্রবেশ, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  যদি(status != PSA_SUCCESS)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  ফেরত CURLE_OK;
#অন্যথা
  (শূন্য)sha256len;
  ফেরত Curl_sha256it(sha256sum, প্রবেশ, inputlen);
#endif
}

স্থির শূন্য *mbedtls_get_internals(কাঠামো ssl_connect_তথ্য *connssl,
                                   CURLINFO info)
{
  কাঠামো mbed_ssl_backend_তথ্য *backend =
    (কাঠামো mbed_ssl_backend_তথ্য *)connssl->backend;
  (শূন্য)info;
  DEBUGASSERT(backend);
  ফেরত &backend->ssl;
}

ধ্রুবক কাঠামো Curl_ssl Curl_ssl_mbedtls = {
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

  আকার(কাঠামো mbed_ssl_backend_তথ্য),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_তথ্য_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  ফাঁকা,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  ফাঁকা,                             /* close_all */
  ফাঁকা,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  ফাঁকা,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
