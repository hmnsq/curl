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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#అన్తర్భావ <mbedtls/version.h>
#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x03020000
#దోషం "mbedTLS 3.2.0 or greater required"
#అయితే_ముగింపు
#అన్తర్భావ <psa/crypto_config.h>
#అన్తర్భావ <mbedtls/net_sockets.h>
#అన్తర్భావ <mbedtls/ssl.h>
#అన్తర్భావ <mbedtls/x509.h>
#అన్తర్భావ <mbedtls/psa_util.h>

#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#దోషం "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#అయితే_ముగింపు

#అన్తర్భావ <mbedtls/error.h>
#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x04000000
#అన్తర్భావ <mbedtls/entropy.h>
#అన్తర్భావ <mbedtls/ctr_drbg.h>
#అయితే_ముగింపు
#నిర్వచితమైతే MBEDTLS_DEBUG
#అన్తర్భావ <mbedtls/debug.h>
#అయితే_ముగింపు

#అన్తర్భావ "vtls/cipher_suite.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "vtls/mbedtls.h"
#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "vtls/vtls_int.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "vtls/x509asn1.h"
#అన్తర్భావ "connect.h" /* for the connect timeout */
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curl_sha256.h"

/* ALPN for http2 */
#ఒకవేళ defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#నిర్వచించు HAS_ALPN_MBEDTLS
#అయితే_ముగింపు

నిర్మాణం mbed_ssl_backend_దత్తాంశం {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#నిర్వచితమైతే MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#అయితే_ముగింపు
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#నిర్వచితమైతే HAS_ALPN_MBEDTLS
  స్థిరరాశి అక్షరం *protocols[3];
#అయితే_ముగింపు
  పూర్ణాంకం *ciphersuites;
  size_t send_blocked_len;
  బిట్టు(initialized); /* mbedtls_ssl_context is initialized */
  బిట్టు(పంపిన_shutdown);
  బిట్టు(send_blocked);
};

/** A context for random number generation (RNG).
 */
#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x04000000
నిర్మాణం rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

స్థిర నిర్మాణం rng_context_t rng;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే MBEDTLS_ERROR_C
#నిర్వచించు mbedtls_strerror(a, b, c) b[0] = 0
#అయితే_ముగింపు

#నిర్వచితమైతే MBEDTLS_DEBUG
స్థిర శూన్య mbed_debug(శూన్య *context, పూర్ణాంకం level, స్థిరరాశి అక్షరం *f_name,
                       పూర్ణాంకం పంక్తి_nb, స్థిరరాశి అక్షరం *పంక్తి)
{
  నిర్మాణం Curl_easy *దత్తాంశం = (నిర్మాణం Curl_easy *)context;
  (శూన్య)level;
  (శూన్య)పంక్తి_nb;
  (శూన్య)f_name;

  ఒకవేళ(దత్తాంశం) {
    size_t len = అక్షరశ్రేణి_పొడవు(పంక్తి);
    ఒకవేళ(len && (పంక్తి[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(దత్తాంశం, "%.*s", (పూర్ణాంకం)len, పంక్తి);
  }
}
#అయితే_ముగింపు

స్థిర పూర్ణాంకం mbedtls_bio_cf_write(శూన్య *bio,
                                స్థిరరాశి గుర్తులేని అక్షరం *buf, size_t blen)
{
  నిర్మాణం Curl_cfilter *cf = bio;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  size_t nwritten;
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(!దత్తాంశం)
    తిరిగి 0;

  ఫలితం = Curl_conn_cf_send(cf->తదుపరి, దత్తాంశం, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (పూర్ణాంకం)ఫలితం, nwritten);
  ఒకవేళ(ఫలితం == CURLE_AGAIN)
    తిరిగి MBEDTLS_ERR_SSL_WANT_WRITE;
  తిరిగి ఫలితం ? -1 : (పూర్ణాంకం)nwritten;
}

స్థిర పూర్ణాంకం mbedtls_bio_cf_read(శూన్య *bio, గుర్తులేని అక్షరం *buf, size_t blen)
{
  నిర్మాణం Curl_cfilter *cf = bio;
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);
  size_t nread = 0;
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం);
  ఒకవేళ(!దత్తాంశం)
    తిరిగి 0;
  /* OpenSSL catches this case, so should we. */
  ఒకవేళ(!buf)
    తిరిగి 0;

  ఫలితం = Curl_conn_cf_recv(cf->తదుపరి, దత్తాంశం, (అక్షరం *)buf, blen, &nread);
  CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (పూర్ణాంకం)ఫలితం, nread);
  ఒకవేళ(ఫలితం == CURLE_AGAIN)
    తిరిగి MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  తిరిగి ఫలితం ? -1 : (పూర్ణాంకం)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#నిర్వచించు RSA_PUB_DER_MAX_బైట్లు   (38 + (2 * MBEDTLS_MPI_MAX_కొలత))
#నిర్వచించు ECP_PUB_DER_MAX_బైట్లు   (30 + (2 * MBEDTLS_ECP_MAX_బైట్లు))

#నిర్వచించు PUB_DER_MAX_బైట్లు   (RSA_PUB_DER_MAX_బైట్లు > ECP_PUB_DER_MAX_బైట్లు ? \
                             RSA_PUB_DER_MAX_బైట్లు : ECP_PUB_DER_MAX_బైట్లు)

స్థిర CURLcode mbed_సమితి_ssl_సంస్కరణ_min_max(
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend,
  నిర్మాణం ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_సంస్కరణ ver_min =
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_సంస్కరణ_TLS1_2
#లేకపోతే
    MBEDTLS_SSL_సంస్కరణ_TLS1_3
#అయితే_ముగింపు
    ;
  mbedtls_ssl_protocol_సంస్కరణ ver_max =
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_సంస్కరణ_TLS1_3
#లేకపోతే
    MBEDTLS_SSL_సంస్కరణ_TLS1_2
#అయితే_ముగింపు
    ;

  DEBUGASSERT(conn_config->సంస్కరణ_2 != CURL_SSLVERSION_DEFAULT);
  ఎంపిక(conn_config->సంస్కరణ_2) {
  స్థితి CURL_SSLVERSION_TLSv1:
  స్థితి CURL_SSLVERSION_TLSv1_0:
  స్థితి CURL_SSLVERSION_TLSv1_1:
  స్థితి CURL_SSLVERSION_TLSv1_2:
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_సంస్కరణ_TLS1_2;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURL_SSLVERSION_TLSv1_3:
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_సంస్కరణ_TLS1_3;
    ఆపు;
#అయితే_ముగింపు
  అప్రమేయ:
    failf(దత్తాంశం, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->సంస్కరణ_2);
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  ఎంపిక(conn_config->సంస్కరణ_max) {
  స్థితి CURL_SSLVERSION_MAX_DEFAULT:
  స్థితి CURL_SSLVERSION_MAX_NONE:
  స్థితి CURL_SSLVERSION_MAX_TLSv1_3:
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_సంస్కరణ_TLS1_3;
    ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_2
  స్థితి CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_సంస్కరణ_TLS1_2;
    ఆపు;
#అయితే_ముగింపు
  స్థితి CURL_SSLVERSION_MAX_TLSv1_1:
  స్థితి CURL_SSLVERSION_MAX_TLSv1_0:
  అప్రమేయ:
    failf(దత్తాంశం, "mbedTLS: unsupported maximum TLS version value");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_సంస్కరణ(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_సంస్కరణ(&backend->config, ver_max);

  తిరిగి CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#నిర్వచితమైతే MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
స్థిర పూర్ణాంకం mbed_cipher_suite_get_str(uint16_t id, అక్షరం *buf, size_t buf_కొలత,
                                     bool prefer_rfc)
{
  ఒకవేళ(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_కొలత, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  లేకపోతే
    తిరిగి Curl_cipher_suite_get_str(id, buf, buf_కొలత, prefer_rfc);
  తిరిగి 0;
}

స్థిర uint16_t mbed_cipher_suite_walk_str(స్థిరరాశి అక్షరం **str, స్థిరరాశి అక్షరం **చివర)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, చివర);
  size_t len = *చివర - *str;
  స్థిర స్థిరరాశి అక్షరం ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  ఒకవేళ(!id) {
    ఒకవేళ((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  తిరిగి id;
}
#లేకపోతే
#నిర్వచించు mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#నిర్వచించు mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#అయితే_ముగింపు

స్థిర CURLcode mbed_సమితి_selected_ciphers(
  నిర్మాణం Curl_easy *దత్తాంశం,
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend,
  స్థిరరాశి అక్షరం *ciphers12,
  స్థిరరాశి అక్షరం *ciphers13)
{
  స్థిరరాశి అక్షరం *ciphers = ciphers12;
  స్థిరరాశి పూర్ణాంకం *supported;
  పూర్ణాంకం *selected;
  size_t supported_len, లెక్క = 0, default13_లెక్క = 0, i, j;
  స్థిరరాశి అక్షరం *ptr, *చివర;

  supported = mbedtls_ssl_list_ciphersuites();
  కొరకు(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(పరిమాణం(పూర్ణాంకం) * (supported_len + 1));
  ఒకవేళ(!selected)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

#నిర్వచితం_కాకపోతే MBEDTLS_SSL_PROTO_TLS1_3
  (శూన్య)ciphers13, (శూన్య)j;
#లేకపోతే
  ఒకవేళ(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    కొరకు(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ఒకవేళ(అక్షరశ్రేణి_పోలిక(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        కొనసాగు;

      selected[లెక్క++] = id;
    }

    default13_లెక్క = లెక్క;
  }
  లేకపోతే
    ciphers = ciphers13;

కూడు_ciphers:
#అయితే_ముగింపు
  కొరకు(ptr = ciphers; ptr[0] != '\0' && లెక్క < supported_len; ptr = చివర) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &చివర);

    /* Check if cipher is supported */
    ఒకవేళ(id) {
      కొరకు(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      ఒకవేళ(i == supported_len)
        id = 0;
    }
    ఒకవేళ(!id) {
      ఒకవేళ(ptr[0] != '\0')
        infof(దత్తాంశం, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (పూర్ణాంకం)(చివర - ptr), ptr);
      కొనసాగు;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    కొరకు(i = 0; i < లెక్క && selected[i] != id; i++)
      ;
    ఒకవేళ(i < లెక్క) {
      ఒకవేళ(i >= default13_లెక్క)
        infof(దత్తాంశం, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (పూర్ణాంకం)(చివర - ptr), ptr);
      కొనసాగు;
    }

    selected[లెక్క++] = id;
  }

#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_3
  ఒకవేళ(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    వెళ్ళు కూడు_ciphers;
  }

  ఒకవేళ(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    కొరకు(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      ఒకవేళ(!అక్షరశ్రేణి_పోలిక(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        కొనసాగు;

      /* No duplicates allowed (so selected cannot overflow) */
      కొరకు(i = 0; i < లెక్క && selected[i] != id; i++)
        ;
      ఒకవేళ(i < లెక్క)
        కొనసాగు;

      selected[లెక్క++] = id;
    }
  }
#అయితే_ముగింపు

  selected[లెక్క] = 0;

  ఒకవేళ(లెక్క == 0) {
    curlx_free(selected);
    failf(దత్తాంశం, "mbedTLS: no supported cipher in list");
    తిరిగి CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  తిరిగి CURLE_OK;
}

స్థిర శూన్య mbed_dump_cert_info(నిర్మాణం Curl_easy *దత్తాంశం,
                                స్థిరరాశి mbedtls_x509_crt *crt)
{
#ఒకవేళ !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (శూన్య)దత్తాంశం, (శూన్య)crt;
#లేకపోతే
  స్థిరరాశి size_t bufsize = 16384;
  అక్షరం *p, *బఫరు = curlx_malloc(bufsize);

  ఒకవేళ(బఫరు && mbedtls_x509_crt_info(బఫరు, bufsize, " ", crt) > 0) {
    infof(దత్తాంశం, "Server certificate:");
    కొరకు(p = బఫరు; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(దత్తాంశం, "%.*s", (పూర్ణాంకం)s, p);
      p += s;
    }
  }
  లేకపోతే
    infof(దత్తాంశం, "Unable to dump certificate information");

  curlx_free(బఫరు);
#అయితే_ముగింపు
}

స్థిర శూన్య mbed_వెలికితీయి_certinfo(నిర్మాణం Curl_easy *దత్తాంశం,
                                  స్థిరరాశి mbedtls_x509_crt *crt)
{
  CURLcode ఫలితం;
  స్థిరరాశి mbedtls_x509_crt *cur;
  పూర్ణాంకం cert_లెక్క = 0;
  పూర్ణాంకం i;

  కొరకు(cur = crt; cur && cert_లెక్క <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->తదుపరి)
    cert_లెక్క++;

  ఒకవేళ(cert_లెక్క > MAX_ALLOWED_CERT_AMOUNT) {
    infof(దత్తాంశం, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    తిరిగి;
  }

  ఫలితం = Curl_ssl_init_certinfo(దత్తాంశం, cert_లెక్క);

  కొరకు(i = 0, cur = crt; ఫలితం == CURLE_OK && cur; ++i, cur = cur->తదుపరి) {
    ఫలితం = Curl_వెలికితీయి_certinfo(దత్తాంశం, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

స్థిర పూర్ణాంకం mbed_verify_cb(శూన్య *ptr, mbedtls_x509_crt *crt,
                          పూర్ణాంకం లోతు, uint32_t *flags)
{
  నిర్మాణం Curl_cfilter *cf = (నిర్మాణం Curl_cfilter *)ptr;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం Curl_easy *దత్తాంశం = CF_దత్తాంశం_CURRENT(cf);

  ఒకవేళ(లోతు == 0) {
    ఒకవేళ(దత్తాంశం->సమితి.verbose)
      mbed_dump_cert_info(దత్తాంశం, crt);
    ఒకవేళ(దత్తాంశం->సమితి.ssl.certinfo)
      mbed_వెలికితీయి_certinfo(దత్తాంశం, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  ఒకవేళ(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  ఒకవేళ(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  ఒకవేళ(*flags) {
#నిర్వచితం_కాకపోతే MBEDTLS_X509_REMOVE_INFO
    అక్షరం buf[128];
    mbedtls_x509_crt_verify_info(buf, పరిమాణం(buf), "", *flags);
    failf(దత్తాంశం, "mbedTLS: %s", buf);
#లేకపోతే
    failf(దత్తాంశం, "mbedTLS: certificate verification error 0x%08x",
          (గుర్తులేని పూర్ణాంకం)*flags);
#అయితే_ముగింపు
  }

  తిరిగి 0;
}

స్థిర CURLcode mbed_ఎక్కించు_cacert(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  స్థిరరాశి నిర్మాణం curl_blob *ca_info_blob = conn_config->ca_info_blob;
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ఖాళీ : conn_config->CAfile);
  స్థిరరాశి bool verifypeer = conn_config->verifypeer;
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_capath = conn_config->CApath;
#నిర్వచితమైతే MBEDTLS_PEM_PARSE_C
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_cert_type = conn_config->cert_type;
#అయితే_ముగింపు
  పూర్ణాంకం ret = -1;
  అక్షరం errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  ఒకవేళ(ca_info_blob && verifypeer) {
#నిర్వచితమైతే MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ఒకవేళ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((అక్షరం *)(ca_info_blob->దత్తాంశం))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->దత్తాంశం,
                                   ca_info_blob->len);
    }
    లేకపోతే { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      గుర్తులేని అక్షరం *newblob = curlx_memdup0(ca_info_blob->దత్తాంశం,
                                             ca_info_blob->len);
      ఒకవేళ(!newblob)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#లేకపోతే
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->దత్తాంశం,
                                     ca_info_blob->len);
#అయితే_ముగింపు

    ఒకవేళ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
      failf(దత్తాంశం, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);
      తిరిగి CURLE_SSL_CERTPROBLEM;
    }
  }

  ఒకవేళ(ssl_cafile && verifypeer) {
#నిర్వచితమైతే MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    ఒకవేళ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
      failf(దత్తాంశం, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);
      తిరిగి CURLE_SSL_CACERT_BADFILE;
    }
#లేకపోతే
    failf(దత్తాంశం, "mbedTLS: functions that use the file system not built in");
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  }

  ఒకవేళ(ssl_capath) {
#నిర్వచితమైతే MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_మార్గం(&backend->cacert, ssl_capath);

    ఒకవేళ(ret < 0) {
      mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
      failf(దత్తాంశం, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);

      ఒకవేళ(verifypeer)
        తిరిగి CURLE_SSL_CACERT_BADFILE;
    }
#లేకపోతే
    failf(దత్తాంశం, "mbedTLS: functions that use the file system not built in");
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode mbed_ఎక్కించు_clicert(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  అక్షరం * స్థిరరాశి ssl_cert = conn_config->clientcert;
  స్థిరరాశి నిర్మాణం curl_blob *ssl_cert_blob = conn_config->cert_blob;
#నిర్వచితమైతే MBEDTLS_PEM_PARSE_C
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_cert_type = conn_config->cert_type;
#అయితే_ముగింపు
  పూర్ణాంకం ret = -1;
  అక్షరం errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  ఒకవేళ(ssl_cert) {
#నిర్వచితమైతే MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    ఒకవేళ(ret) {
      mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
      failf(దత్తాంశం, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);

      తిరిగి CURLE_SSL_CERTPROBLEM;
    }
#లేకపోతే
    failf(దత్తాంశం, "mbedTLS: functions that use the file system not built in");
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  }

  ఒకవేళ(ssl_cert_blob) {
#నిర్వచితమైతే MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    ఒకవేళ((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((అక్షరం *)(ssl_cert_blob->దత్తాంశం))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->దత్తాంశం,
                                   ssl_cert_blob->len);
    }
    లేకపోతే { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      గుర్తులేని అక్షరం *newblob = curlx_memdup0(ssl_cert_blob->దత్తాంశం,
                                             ssl_cert_blob->len);
      ఒకవేళ(!newblob)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#లేకపోతే
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->దత్తాంశం,
                                     ssl_cert_blob->len);
#అయితే_ముగింపు

    ఒకవేళ(ret) {
      mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
      failf(దత్తాంశం, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);
      తిరిగి CURLE_SSL_CERTPROBLEM;
    }
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode mbed_ఎక్కించు_privkey(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  పూర్ణాంకం ret = -1;
  అక్షరం errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  ఒకవేళ(conn_config->కీలకం || conn_config->కీలకం_blob) {
    ఒకవేళ(conn_config->కీలకం) {
#నిర్వచితమైతే MBEDTLS_FS_IO
#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->కీలకం,
                                     conn_config->కీలకం_passwd);
      ఒకవేళ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_కీలకం_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_కీలకం_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#లేకపోతే
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->కీలకం,
                                     conn_config->కీలకం_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      ఒకవేళ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#అయితే_ముగింపు

      ఒకవేళ(ret) {
        mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
        failf(దత్తాంశం, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->కీలకం, (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }
#లేకపోతే
      failf(దత్తాంశం, "mbedTLS: functions that use the file system not built in");
      తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
    }
    లేకపోతే {
      స్థిరరాశి నిర్మాణం curl_blob *ssl_కీలకం_blob = conn_config->కీలకం_blob;
      స్థిరరాశి అక్షరం *passwd = conn_config->కీలకం_passwd;
      స్థిరరాశి bool is_der = conn_config->కీలకం_type &&
                          curl_strequal(conn_config->కీలకం_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      గుర్తులేని అక్షరం *newblob = curlx_memdup0(ssl_కీలకం_blob->దత్తాంశం,
                                             ssl_కీలకం_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_కీలకం_blob->len : ssl_కీలకం_blob->len + 1;
      ఒకవేళ(!newblob)
        తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య >= 0x04000000
      ret = mbedtls_pk_parse_కీలకం(&backend->pk, newblob, keylen,
                                 (స్థిరరాశి గుర్తులేని అక్షరం *)passwd,
                                 passwd ? అక్షరశ్రేణి_పొడవు(passwd) : 0);
      ఒకవేళ(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_కీలకం_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_కీలకం_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#లేకపోతే
      ret = mbedtls_pk_parse_కీలకం(&backend->pk, newblob, keylen,
                                 (స్థిరరాశి గుర్తులేని అక్షరం *)passwd,
                                 passwd ? అక్షరశ్రేణి_పొడవు(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      ఒకవేళ(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#అయితే_ముగింపు
      curlx_free(newblob);

      ఒకవేళ(ret) {
        mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
        failf(దత్తాంశం, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);
        తిరిగి CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  తిరిగి CURLE_OK;
}

స్థిర CURLcode mbed_ఎక్కించు_crl(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  స్థిరరాశి అక్షరం * స్థిరరాశి ssl_crlfile = conn_config->CRLfile;

#నిర్వచితమైతే MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  ఒకవేళ(ssl_crlfile) {
    అక్షరం errorbuf[128];
#నిర్వచితమైతే MBEDTLS_FS_IO
    పూర్ణాంకం ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    ఒకవేళ(ret) {
      mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
      failf(దత్తాంశం, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);

      తిరిగి CURLE_SSL_CRL_BADFILE;
    }
#లేకపోతే
    (శూన్య)errorbuf;
    failf(దత్తాంశం, "mbedTLS: functions that use the file system not built in");
    తిరిగి CURLE_NOT_BUILT_లోపల;
#అయితే_ముగింపు
  }
#లేకపోతే
  (శూన్య)backend;
  ఒకవేళ(ssl_crlfile) {
    failf(దత్తాంశం, "mbedTLS: CRL support not built in");
    తిరిగి CURLE_NOT_BUILT_లోపల;
  }
#అయితే_ముగింపు

  తిరిగి CURLE_OK;
}

స్థిర bool mbed_apply_session(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం Curl_ssl_session *sc_session)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;

  ఒకవేళ(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    పూర్ణాంకం ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_ఎక్కించు(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    ఒకవేళ(ret) {
      failf(దత్తాంశం, "SSL session error loading: -0x%x", (గుర్తులేని పూర్ణాంకం)-ret);
    }
    లేకపోతే {
      ret = mbedtls_ssl_సమితి_session(&backend->ssl, &session);
      ఒకవేళ(ret)
        failf(దత్తాంశం, "SSL session error setting: -0x%x", (గుర్తులేని పూర్ణాంకం)-ret);
    }
    mbedtls_ssl_session_free(&session);
    ఒకవేళ(!ret) {
      infof(దత్తాంశం, "SSL reusing session ID");
      తిరిగి TRUE;
    }
  }
  తిరిగి FALSE;
}

స్థిర CURLcode mbed_configure_ssl(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  పూర్ణాంకం ret;
  bool session_applied = FALSE;
  CURLcode ఫలితం;
  అక్షరం errorbuf[128];

  infof(దత్తాంశం, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->పోర్టు);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_ప్రవాహం,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  ఒకవేళ(ret) {
    failf(దత్తాంశం, "mbedTLS: ssl_config failed");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

#నిర్వచితమైతే MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, దత్తాంశం);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_సమితి_పరిమితి(4);
#అయితే_ముగింపు

#ఒకవేళ defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_సంస్కరణ_సంఖ్య >= 0x03060100 &&   \
  MBEDTLS_సంస్కరణ_సంఖ్య < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#అయితే_ముగింపు

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_తదుపరి);

  ఫలితం = mbed_సమితి_ssl_సంస్కరణ_min_max(దత్తాంశం, backend, conn_config);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#అయితే_ముగింపు

  ret = mbedtls_ssl_అమరిక(&backend->ssl,
                          &backend->config);
  ఒకవేళ(ret) {
    mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
    failf(దత్తాంశం, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_సమితి_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      ఖాళీ /*  rev_timeout() */);

#నిర్వచితం_కాకపోతే MBEDTLS_SSL_PROTO_TLS1_3
  ఒకవేళ(conn_config->cipher_list) {
    ఫలితం = mbed_సమితి_selected_ciphers(దత్తాంశం, backend,
                                       conn_config->cipher_list, ఖాళీ);
#లేకపోతే
  ఒకవేళ(conn_config->cipher_list || conn_config->cipher_list13) {
    ఫలితం = mbed_సమితి_selected_ciphers(దత్తాంశం, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#అయితే_ముగింపు
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "mbedTLS: failed to set cipher suites");
      తిరిగి ఫలితం;
    }
  }
  లేకపోతే {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#నిర్వచితమైతే MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#అయితే_ముగింపు

#నిర్వచితమైతే MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#అయితే_ముగింపు

  ఒకవేళ((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    నిర్మాణం Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(దత్తాంశం, cf->cft, FIRSTSOCKET);
    ఒకవేళ(scs) {
      ఒకవేళ(mbed_apply_session(cf, దత్తాంశం, scs)) {
        CURL_TRC_CF(దత్తాంశం, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  ఒకవేళ(!session_applied && Curl_ssl_scache_use(cf, దత్తాంశం)) {
    నిర్మాణం Curl_ssl_session *sc_session = ఖాళీ;
    CURLcode sresult = Curl_ssl_scache_take(cf, దత్తాంశం, connssl->peer.scache_కీలకం,
                                            &sc_session);
    ఒకవేళ(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_ఎక్కించు(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      ఒకవేళ(ret) {
        failf(దత్తాంశం, "SSL session error loading: -0x%x", (గుర్తులేని పూర్ణాంకం)-ret);
      }
      లేకపోతే {
        ret = mbedtls_ssl_సమితి_session(&backend->ssl, &session);
        ఒకవేళ(ret)
          failf(దత్తాంశం, "SSL session error setting: -0x%x", (గుర్తులేని పూర్ణాంకం)-ret);
        లేకపోతే
          infof(దత్తాంశం, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, దత్తాంశం, connssl->peer.scache_కీలకం,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#నిర్వచితమైతే MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#లేకపోతే
                            ఖాళీ
#అయితే_ముగింపు
    );

  ఒకవేళ(conn_config->కీలకం || conn_config->కీలకం_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  ఒకవేళ(mbedtls_ssl_సమితి_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(దత్తాంశం, "Failed to set SNI");
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

#నిర్వచితమైతే HAS_ALPN_MBEDTLS
  ఒకవేళ(connssl->alpn) {
    నిర్మాణం alpn_proto_buf proto;
    size_t i;

    కొరకు(i = 0; i < connssl->alpn->లెక్క; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    ఒకవేళ(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(దత్తాంశం, "Failed setting ALPN protocols");
      తిరిగి CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_గమ్యం_proto_str(&proto, connssl->alpn);
    infof(దత్తాంశం, VTLS_INFOF_ALPN_OFFER_1STR, proto.దత్తాంశం);
  }
#అయితే_ముగింపు

  /* give application a chance to interfere with mbedTLS set up. */
  ఒకవేళ(దత్తాంశం->సమితి.ssl_fsslctx) {
    ఫలితం = (*దత్తాంశం->సమితి.ssl_fsslctx)(దత్తాంశం, &backend->config,
                                      దత్తాంశం->సమితి.ssl_fsslctxp);
    ఒకవేళ(ఫలితం)
      failf(దత్తాంశం, "error signaled by SSL ctx callback");
  }

  తిరిగి ఫలితం;
}

స్థిర CURLcode mbed_connect_step1(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ఫలితం;

  ఒకవేళ((conn_config->సంస్కరణ_2 == CURL_SSLVERSION_SSLv2) ||
     (conn_config->సంస్కరణ_2 == CURL_SSLVERSION_SSLv3)) {
    failf(దత్తాంశం, "Not supported SSL version");
    తిరిగి CURLE_NOT_BUILT_లోపల;
  }

  ఫలితం = mbed_ఎక్కించు_cacert(cf, దత్తాంశం);
  ఒకవేళ(!ఫలితం)
    ఫలితం = mbed_ఎక్కించు_clicert(cf, దత్తాంశం);
  ఒకవేళ(!ఫలితం)
    ఫలితం = mbed_ఎక్కించు_privkey(cf, దత్తాంశం);
  ఒకవేళ(!ఫలితం)
    ఫలితం = mbed_ఎక్కించు_crl(cf, దత్తాంశం);
  ఒకవేళ(!ఫలితం)
    ఫలితం = mbed_configure_ssl(cf, దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  connssl->connecting_state = ssl_connect_2;
  తిరిగి CURLE_OK;
}

#ఒకవేళ defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#నిర్వచించు HAVE_PINNED_PUBKEY
#అయితే_ముగింపు

స్థిర CURLcode mbed_connect_step2(నిర్మాణం Curl_cfilter *cf,
                                   నిర్మాణం Curl_easy *దత్తాంశం)
{
#ఒకవేళ defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode ఫలితం;
#అయితే_ముగింపు
  పూర్ణాంకం ret;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
#నిర్వచితమైతే HAVE_PINNED_PUBKEY
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
  స్థిరరాశి అక్షరం * స్థిరరాశి pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY);
#లేకపోతే
  స్థిరరాశి అక్షరం * స్థిరరాశి pinnedpubkey =
    CURL_EASY_STR(దత్తాంశం, STRING_SSL_PINNEDPUBLICKEY);
#అయితే_ముగింపు
#అయితే_ముగింపు

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  ఒకవేళ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    తిరిగి CURLE_OK;
  }
  లేకపోతే ఒకవేళ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    తిరిగి CURLE_OK;
  }
  లేకపోతే ఒకవేళ(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(దత్తాంశం, "peer certificate could not be verified");
    తిరిగి CURLE_PEER_FAILED_VERIFICATION;
  }
  లేకపోతే ఒకవేళ(ret) {
    అక్షరం errorbuf[128];
    CURL_TRC_CF(దత్తాంశం, cf, "TLS version %04X",
                mbedtls_ssl_get_సంస్కరణ_సంఖ్య(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, పరిమాణం(errorbuf));
    failf(దత్తాంశం, "ssl_handshake returned: (-0x%04X) %s",
          (గుర్తులేని పూర్ణాంకం)-ret, errorbuf);
    తిరిగి CURLE_SSL_CONNECT_ERROR;
  }

  {
    అక్షరం cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, పరిమాణం(cipher_str), TRUE);
    infof(దత్తాంశం, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_సంస్కరణ(&backend->ssl), cipher_str);
  }

#నిర్వచితమైతే HAVE_PINNED_PUBKEY
  ఒకవేళ(pinnedpubkey) {
    పూర్ణాంకం కొలత;
    స్థిరరాశి mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = ఖాళీ;
    గుర్తులేని అక్షరం *pubkey = ఖాళీ;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    ఒకవేళ(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(దత్తాంశం, "Failed due to missing peer certificate");
      తిరిగి CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, పరిమాణం(*p));

    ఒకవేళ(!p)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

    pubkey = curlx_malloc(PUB_DER_MAX_బైట్లు);

    ఒకవేళ(!pubkey) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    ఒకవేళ(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(దత్తాంశం, "Failed copying peer certificate");
      ఫలితం = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      వెళ్ళు pinnedpubkey_error;
    }

    కొలత = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_బైట్లు);

    ఒకవేళ(కొలత <= 0) {
      failf(దత్తాంశం, "Failed copying public key from peer certificate");
      ఫలితం = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      వెళ్ళు pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    ఫలితం = Curl_pin_peer_pubkey(దత్తాంశం,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_బైట్లు - కొలత], కొలత);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
#అయితే_ముగింపు

#నిర్వచితమైతే HAS_ALPN_MBEDTLS
  ఒకవేళ(connssl->alpn) {
    స్థిరరాశి అక్షరం *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    ఫలితం = Curl_alpn_సమితి_negotiated(cf, దత్తాంశం, connssl,
                                      (స్థిరరాశి గుర్తులేని అక్షరం *)proto,
                                      proto ? అక్షరశ్రేణి_పొడవు(proto) : 0);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
#అయితే_ముగింపు

  connssl->connecting_state = ssl_connect_3;
  infof(దత్తాంశం, "SSL connected");

  తిరిగి CURLE_OK;
}

స్థిర CURLcode mbed_new_session(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  నిర్మాణం Curl_ssl_session *sc_session = ఖాళీ;
  గుర్తులేని అక్షరం *sdata = ఖాళీ;
  size_t slen = 0;
  పూర్ణాంకం ietf_tls_id;
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం ret;

  DEBUGASSERT(backend);
  ఒకవేళ(!Curl_ssl_scache_use(cf, దత్తాంశం))
    తిరిగి CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  ఒకవేళ(ret) {
    failf(దత్తాంశం, "mbedtls_ssl_get_session returned -0x%x", (గుర్తులేని పూర్ణాంకం)-ret);
    ఫలితం = CURLE_SSL_CONNECT_ERROR;
    వెళ్ళు బహిర్గతం;
  }

  mbedtls_ssl_session_save(&session, ఖాళీ, 0, &slen);
  ఒకవేళ(!slen) {
    failf(దత్తాంశం, "failed to serialize session: length is 0");
    వెళ్ళు బహిర్గతం;
  }

  sdata = curlx_malloc(slen);
  ఒకవేళ(!sdata) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  ఒకవేళ(ret) {
    failf(దత్తాంశం, "failed to serialize session: -0x%x", (గుర్తులేని పూర్ణాంకం)-ret);
    వెళ్ళు బహిర్గతం;
  }

  ietf_tls_id = mbedtls_ssl_get_సంస్కరణ_సంఖ్య(&backend->ssl);
  ఫలితం = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = ఖాళీ;  /* call took ownership */
  ఒకవేళ(!ఫలితం && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    ఫలితం = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  ఒకవేళ(!ఫలితం) {
    ఫలితం = Curl_ssl_scache_put(cf, దత్తాంశం, connssl->peer.scache_కీలకం,
                                 sc_session);
    sc_session = ఖాళీ;
  }

బహిర్గతం:
  ఒకవేళ(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  తిరిగి ఫలితం;
}

స్థిర CURLcode mbed_send(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                          స్థిరరాశి శూన్య *mem, size_t len, size_t *pnwritten)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం nwritten;

  DEBUGASSERT(backend);
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#అయితే_ముగింపు
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  ఒకవేళ(backend->send_blocked) {
    ఒకవేళ(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(దత్తాంశం, "re-send data shorter than previously");
      ఫలితం = CURLE_BAD_FUNCTION_వాదం;
      వెళ్ళు బహిర్గతం;
    }
    CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (స్థిరరాశి గుర్తులేని అక్షరం *)mem, len);

  ఒకవేళ(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  లేకపోతే {
    CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (గుర్తులేని పూర్ణాంకం)-nwritten);
    ఎంపిక(nwritten) {
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_3
    స్థితి MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      ఫలితం = mbed_new_session(cf, దత్తాంశం);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      ఒకవేళ(!ఫలితం)
        వెళ్ళు do_send;
      ఆపు;
#అయితే_ముగింపు
    స్థితి MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ఫలితం = CURLE_AGAIN;
      ఆపు;
    స్థితి MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ఫలితం = CURLE_AGAIN;
      ఆపు;
    అప్రమేయ:
      ఫలితం = CURLE_SEND_ERROR;
      ఆపు;
    }
    ఒకవేళ((ఫలితం == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (పూర్ణాంకం)ఫలితం, *pnwritten);
  తిరిగి ఫలితం;
}

స్థిర CURLcode mbedtls_shutdown(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 bool send_shutdown, bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  గుర్తులేని అక్షరం buf[1024];
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  ఒకవేళ(!backend->initialized || cf->shutdown) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *పూర్తి = FALSE;

  ఒకవేళ(!backend->పంపిన_shutdown) {
    /* do this only once */
    backend->పంపిన_shutdown = TRUE;
    ఒకవేళ(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      ఎంపిక(ret) {
      స్థితి 0: /* we sent it, receive from the server */
        ఆపు;
      స్థితి MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *పూర్తి = TRUE;
        వెళ్ళు బహిర్గతం;
      స్థితి MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        వెళ్ళు బహిర్గతం;
      స్థితి MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        వెళ్ళు బహిర్గతం;
      అప్రమేయ:
        CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_shutdown error -0x%04X",
                    (గుర్తులేని పూర్ణాంకం)-ret);
        ఫలితం = CURLE_RECV_ERROR;
        వెళ్ళు బహిర్గతం;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  కొరకు(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, పరిమాణం(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    ఒకవేళ(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, పరిమాణం(buf));
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_3
    ఒకవేళ(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      కొనసాగు;
#అయితే_ముగింపు
    ఒకవేళ(ret <= 0)
      ఆపు;
  }

  ఒకవేళ(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_shutdown, still getting data");
  }
  లేకపోతే ఒకవేళ(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_shutdown done");
    *పూర్తి = TRUE;
  }
  లేకపోతే ఒకవేళ(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  లేకపోతే ఒకవేళ(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  లేకపోతే {
    CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_shutdown error -0x%04X",
                (గుర్తులేని పూర్ణాంకం)-ret);
    ఫలితం = CURLE_RECV_ERROR;
  }

బహిర్గతం:
  cf->shutdown = (ఫలితం || *పూర్తి);
  తిరిగి ఫలితం;
}

స్థిర శూన్య mbedtls_close(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#నిర్వచితమైతే MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#అయితే_ముగింపు
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  ఒకవేళ(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

స్థిర CURLcode mbed_recv(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                          అక్షరం *buf, size_t buffersize, size_t *pnread)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  CURLcode ఫలితం = CURLE_OK;
  పూర్ణాంకం nread;

  DEBUGASSERT(backend);
#నిర్వచితమైతే MBEDTLS_SSL_SESSION_TICKETS
do_read:
#అయితే_ముగింపు
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (గుర్తులేని అక్షరం *)buf, buffersize);
  ఒకవేళ(nread > 0)
    *pnread = (size_t)nread;
  లేకపోతే {
    అక్షరం errorbuf[128];
    CURL_TRC_CF(దత్తాంశం, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (గుర్తులేని పూర్ణాంకం)-nread);
    ఎంపిక(nread) {
#నిర్వచితమైతే MBEDTLS_SSL_SESSION_TICKETS
    స్థితి MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      ఫలితం = mbed_new_session(cf, దత్తాంశం);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      ఒకవేళ(!ఫలితం)
        వెళ్ళు do_read;
      ఆపు;
#అయితే_ముగింపు
    స్థితి MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      ఫలితం = CURLE_AGAIN;
      ఆపు;
    స్థితి MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      ఫలితం = CURLE_AGAIN;
      ఆపు;
    స్థితి MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      ఫలితం = CURLE_OK;
      ఆపు;
    అప్రమేయ:
      mbedtls_strerror(nread, errorbuf, పరిమాణం(errorbuf));
      failf(దత్తాంశం, "ssl_read returned: (-0x%04X) %s", (గుర్తులేని పూర్ణాంకం)-nread,
            errorbuf);
      ఫలితం = CURLE_RECV_ERROR;
      ఆపు;
    }
  }
  తిరిగి ఫలితం;
}

స్థిర size_t mbedtls_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత)
{
#నిర్వచితమైతే MBEDTLS_సంస్కరణ_C
  /* if mbedtls_version_get_number() is available it is better */
  గుర్తులేని పూర్ణాంకం సంస్కరణ_2 = mbedtls_సంస్కరణ_get_సంఖ్య();
  తిరిగి curl_msnprintf(బఫరు, కొలత, "mbedTLS/%u.%u.%u", సంస్కరణ_2 >> 24,
                        (సంస్కరణ_2 >> 16) & 0xff, (సంస్కరణ_2 >> 8) & 0xff);
#లేకపోతే
  తిరిగి curl_msnprintf(బఫరు, కొలత, "mbedTLS/%s", MBEDTLS_సంస్కరణ_STRING);
#అయితే_ముగింపు
}

/* 'data' might be NULL */
స్థిర CURLcode mbedtls_random(నిర్మాణం Curl_easy *దత్తాంశం,
                               గుర్తులేని అక్షరం *entropy, size_t నిడివి)
{
  psa_స్థితి_t స్థితి_2;
  (శూన్య)దత్తాంశం;

  స్థితి_2 = psa_generate_random(entropy, నిడివి);

  తిరిగి స్థితి_2 == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

స్థిర CURLcode mbedtls_connect(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                bool *పూర్తి)
{
  CURLcode ఫలితం;
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;

  /* check if the connection has already been established */
  ఒకవేళ(ssl_connection_complete == connssl->state) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  *పూర్తి = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  ఒకవేళ(connssl->connecting_state == ssl_connect_1) {
    ఫలితం = mbed_connect_step1(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_2) {
    ఫలితం = mbed_connect_step2(cf, దత్తాంశం);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    నిర్మాణం ssl_connect_దత్తాంశం *ctx = cf->ctx;
    నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
      (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)ctx->backend;

    ఒకవేళ(mbedtls_ssl_get_సంస్కరణ_సంఖ్య(&backend->ssl) <=
       MBEDTLS_SSL_సంస్కరణ_TLS1_2) {
      ఫలితం = mbed_new_session(cf, దత్తాంశం);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    connssl->connecting_state = ssl_connect_పూర్తి;
  }

  ఒకవేళ(connssl->connecting_state == ssl_connect_పూర్తి) {
    connssl->state = ssl_connection_complete;
    *పూర్తి = TRUE;
  }

  తిరిగి CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
స్థిర పూర్ణాంకం mbedtls_init(శూన్య)
{
#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x04000000
  పూర్ణాంకం ret = 0;
#అయితే_ముగింపు
  psa_స్థితి_t స్థితి_2;
  స్థితి_2 = psa_crypto_init();

  ఒకవేళ(స్థితి_2 != PSA_SUCCESS)
    తిరిగి 0;

#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_విత్తనం(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              ఖాళీ, 0);

  ఒకవేళ(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    తిరిగి 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_సమితి_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#అయితే_ముగింపు
  తిరిగి 1;
}

స్థిర శూన్య mbedtls_cleanup(శూన్య)
{
  mbedtls_psa_crypto_free();

#ఒకవేళ MBEDTLS_సంస్కరణ_సంఖ్య < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#అయితే_ముగింపు
}

స్థిర bool mbedtls_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                 స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *ctx = cf->ctx;
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend;

  (శూన్య)దత్తాంశం;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)ctx->backend;
  తిరిగి mbedtls_ssl_get_బైట్లు_avail(&backend->ssl) != 0;
}

స్థిర CURLcode mbedtls_sha256sum(స్థిరరాశి గుర్తులేని అక్షరం *ఆదానం,
                                  size_t inputlen,
                                  గుర్తులేని అక్షరం *sha256sum,
                                  size_t sha256len)
{
#ఒకవేళ defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_స్థితి_t స్థితి_2;
  size_t sha256len_actual;
  స్థితి_2 = psa_hash_compute(PSA_ALG_SHA_256, ఆదానం, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  ఒకవేళ(స్థితి_2 != PSA_SUCCESS)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  తిరిగి CURLE_OK;
#లేకపోతే
  (శూన్య)sha256len;
  తిరిగి Curl_sha256it(sha256sum, ఆదానం, inputlen);
#అయితే_ముగింపు
}

స్థిర శూన్య *mbedtls_get_internals(నిర్మాణం ssl_connect_దత్తాంశం *connssl,
                                   CURLINFO info)
{
  నిర్మాణం mbed_ssl_backend_దత్తాంశం *backend =
    (నిర్మాణం mbed_ssl_backend_దత్తాంశం *)connssl->backend;
  (శూన్య)info;
  DEBUGASSERT(backend);
  తిరిగి &backend->ssl;
}

స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_మార్గం |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#నిర్వచితమైతే HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#అయితే_ముగింపు
  SSLSUPP_SSL_CTX |
#నిర్వచితమైతే MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#అయితే_ముగింపు
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#నిర్వచితమైతే MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#అయితే_ముగింపు
  0,

  పరిమాణం(నిర్మాణం mbed_ssl_backend_దత్తాంశం),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_సంస్కరణ,                  /*సంస్కరణ*/
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_దత్తాంశం_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  ఖాళీ,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  ఖాళీ,                             /* close_all */
  ఖాళీ,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  ఖాళీ,                             /* get_channel_binding */
};

#అయితే_ముగింపు /* USE_MBEDTLS */
