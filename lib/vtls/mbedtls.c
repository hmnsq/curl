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
#ضمن "curl_setup.h"

#جيڪڏهن_وصف_ٿيل USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#ضمن <mbedtls/version.h>
#إذا MBEDTLS_نسخو_عدد < 0x03020000
#غلطي "mbedTLS 3.2.0 or greater required"
#جيڪڏهن_پورو
#ضمن <psa/crypto_config.h>
#ضمن <mbedtls/net_sockets.h>
#ضمن <mbedtls/ssl.h>
#ضمن <mbedtls/x509.h>
#ضمن <mbedtls/psa_util.h>

#إذا MBEDTLS_نسخو_عدد < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#غلطي "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#جيڪڏهن_پورو

#ضمن <mbedtls/error.h>
#إذا MBEDTLS_نسخو_عدد < 0x04000000
#ضمن <mbedtls/entropy.h>
#ضمن <mbedtls/ctr_drbg.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MBEDTLS_DEBUG
#ضمن <mbedtls/debug.h>
#جيڪڏهن_پورو

#ضمن "vtls/cipher_suite.h"
#ضمن "urldata.h"
#ضمن "curl_trc.h"
#ضمن "vtls/mbedtls.h"
#ضمن "vtls/vtls.h"
#ضمن "vtls/vtls_int.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vtls/x509asn1.h"
#ضمن "connect.h" /* for the connect timeout */
#ضمن "curlx/strdup.h"
#ضمن "curl_sha256.h"

/* ALPN for http2 */
#إذا defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#وصف_ڪريو HAS_ALPN_MBEDTLS
#جيڪڏهن_پورو

بنية mbed_ssl_backend_معلومات {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#جيڪڏهن_وصف_ٿيل MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#جيڪڏهن_پورو
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#جيڪڏهن_وصف_ٿيل HAS_ALPN_MBEDTLS
  ثابت حرف *protocols[3];
#جيڪڏهن_پورو
  صحيح *ciphersuites;
  size_t send_blocked_len;
  ٻٽو_انگ(initialized); /* mbedtls_ssl_context is initialized */
  ٻٽو_انگ(موڪليل_shutdown);
  ٻٽو_انگ(send_blocked);
};

/** A context for random number generation (RNG).
 */
#إذا MBEDTLS_نسخو_عدد < 0x04000000
بنية rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ساكن بنية rng_context_t rng;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل MBEDTLS_ERROR_C
#وصف_ڪريو mbedtls_strerror(a, b, c) b[0] = 0
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MBEDTLS_DEBUG
ساكن فراغ mbed_debug(فراغ *context, صحيح level, ثابت حرف *f_name,
                       صحيح سٽ_nb, ثابت حرف *سٽ)
{
  بنية Curl_easy *معلومات = (بنية Curl_easy *)context;
  (فراغ)level;
  (فراغ)سٽ_nb;
  (فراغ)f_name;

  إذا(معلومات) {
    size_t len = لڙي_ڊيگهه(سٽ);
    إذا(len && (سٽ[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(معلومات, "%.*s", (صحيح)len, سٽ);
  }
}
#جيڪڏهن_پورو

ساكن صحيح mbedtls_bio_cf_write(فراغ *bio,
                                ثابت بنانشان حرف *buf, size_t blen)
{
  بنية Curl_cfilter *cf = bio;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  size_t nwritten;
  CURLcode نتيجو;

  DEBUGASSERT(معلومات);
  إذا(!معلومات)
    أرجع 0;

  نتيجو = Curl_conn_cf_send(cf->ايندڙ, معلومات, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(معلومات, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (صحيح)نتيجو, nwritten);
  إذا(نتيجو == CURLE_AGAIN)
    أرجع MBEDTLS_ERR_SSL_WANT_WRITE;
  أرجع نتيجو ? -1 : (صحيح)nwritten;
}

ساكن صحيح mbedtls_bio_cf_read(فراغ *bio, بنانشان حرف *buf, size_t blen)
{
  بنية Curl_cfilter *cf = bio;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  size_t nread = 0;
  CURLcode نتيجو;

  DEBUGASSERT(معلومات);
  إذا(!معلومات)
    أرجع 0;
  /* OpenSSL catches this case, so should we. */
  إذا(!buf)
    أرجع 0;

  نتيجو = Curl_conn_cf_recv(cf->ايندڙ, معلومات, (حرف *)buf, blen, &nread);
  CURL_TRC_CF(معلومات, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (صحيح)نتيجو, nread);
  إذا(نتيجو == CURLE_AGAIN)
    أرجع MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  أرجع نتيجو ? -1 : (صحيح)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#وصف_ڪريو RSA_PUB_DER_MAX_بائيٽون   (38 + (2 * MBEDTLS_MPI_MAX_سائيز))
#وصف_ڪريو ECP_PUB_DER_MAX_بائيٽون   (30 + (2 * MBEDTLS_ECP_MAX_بائيٽون))

#وصف_ڪريو PUB_DER_MAX_بائيٽون   (RSA_PUB_DER_MAX_بائيٽون > ECP_PUB_DER_MAX_بائيٽون ? \
                             RSA_PUB_DER_MAX_بائيٽون : ECP_PUB_DER_MAX_بائيٽون)

ساكن CURLcode mbed_سيٽ_ssl_نسخو_min_max(
  بنية Curl_easy *معلومات,
  بنية mbed_ssl_backend_معلومات *backend,
  بنية ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_نسخو ver_min =
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_نسخو_TLS1_2
#وإلا
    MBEDTLS_SSL_نسخو_TLS1_3
#جيڪڏهن_پورو
    ;
  mbedtls_ssl_protocol_نسخو ver_max =
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_نسخو_TLS1_3
#وإلا
    MBEDTLS_SSL_نسخو_TLS1_2
#جيڪڏهن_پورو
    ;

  DEBUGASSERT(conn_config->نسخو_2 != CURL_SSLVERSION_DEFAULT);
  بدل(conn_config->نسخو_2) {
  حالة CURL_SSLVERSION_TLSv1:
  حالة CURL_SSLVERSION_TLSv1_0:
  حالة CURL_SSLVERSION_TLSv1_1:
  حالة CURL_SSLVERSION_TLSv1_2:
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_نسخو_TLS1_2;
    اكسر;
#جيڪڏهن_پورو
  حالة CURL_SSLVERSION_TLSv1_3:
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_نسخو_TLS1_3;
    اكسر;
#جيڪڏهن_پورو
  افتراضي:
    failf(معلومات, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->نسخو_2);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  بدل(conn_config->نسخو_max) {
  حالة CURL_SSLVERSION_MAX_DEFAULT:
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_TLSv1_3:
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_نسخو_TLS1_3;
    اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_2
  حالة CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_نسخو_TLS1_2;
    اكسر;
#جيڪڏهن_پورو
  حالة CURL_SSLVERSION_MAX_TLSv1_1:
  حالة CURL_SSLVERSION_MAX_TLSv1_0:
  افتراضي:
    failf(معلومات, "mbedTLS: unsupported maximum TLS version value");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_نسخو(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_نسخو(&backend->config, ver_max);

  أرجع CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#جيڪڏهن_وصف_ٿيل MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ساكن صحيح mbed_cipher_suite_get_str(uint16_t id, حرف *buf, size_t buf_سائيز,
                                     bool prefer_rfc)
{
  إذا(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_سائيز, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  وإلا
    أرجع Curl_cipher_suite_get_str(id, buf, buf_سائيز, prefer_rfc);
  أرجع 0;
}

ساكن uint16_t mbed_cipher_suite_walk_str(ثابت حرف **str, ثابت حرف **پڇاڙي)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, پڇاڙي);
  size_t len = *پڇاڙي - *str;
  ساكن ثابت حرف ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  إذا(!id) {
    إذا((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  أرجع id;
}
#وإلا
#وصف_ڪريو mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#وصف_ڪريو mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#جيڪڏهن_پورو

ساكن CURLcode mbed_سيٽ_selected_ciphers(
  بنية Curl_easy *معلومات,
  بنية mbed_ssl_backend_معلومات *backend,
  ثابت حرف *ciphers12,
  ثابت حرف *ciphers13)
{
  ثابت حرف *ciphers = ciphers12;
  ثابت صحيح *supported;
  صحيح *selected;
  size_t supported_len, ڳڻپ = 0, default13_ڳڻپ = 0, i, j;
  ثابت حرف *ptr, *پڇاڙي;

  supported = mbedtls_ssl_list_ciphersuites();
  لكل(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(حجم(صحيح) * (supported_len + 1));
  إذا(!selected)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

#جيڪڏهن_وصف_نه_ٿيل MBEDTLS_SSL_PROTO_TLS1_3
  (فراغ)ciphers13, (فراغ)j;
#وإلا
  إذا(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    لكل(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      إذا(لڙي_ڀيٽ(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        استمر;

      selected[ڳڻپ++] = id;
    }

    default13_ڳڻپ = ڳڻپ;
  }
  وإلا
    ciphers = ciphers13;

جوڙيو_ciphers:
#جيڪڏهن_پورو
  لكل(ptr = ciphers; ptr[0] != '\0' && ڳڻپ < supported_len; ptr = پڇاڙي) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &پڇاڙي);

    /* Check if cipher is supported */
    إذا(id) {
      لكل(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      إذا(i == supported_len)
        id = 0;
    }
    إذا(!id) {
      إذا(ptr[0] != '\0')
        infof(معلومات, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (صحيح)(پڇاڙي - ptr), ptr);
      استمر;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    لكل(i = 0; i < ڳڻپ && selected[i] != id; i++)
      ;
    إذا(i < ڳڻپ) {
      إذا(i >= default13_ڳڻپ)
        infof(معلومات, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (صحيح)(پڇاڙي - ptr), ptr);
      استمر;
    }

    selected[ڳڻپ++] = id;
  }

#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_3
  إذا(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    اذهب جوڙيو_ciphers;
  }

  إذا(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    لكل(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      إذا(!لڙي_ڀيٽ(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        استمر;

      /* No duplicates allowed (so selected cannot overflow) */
      لكل(i = 0; i < ڳڻپ && selected[i] != id; i++)
        ;
      إذا(i < ڳڻپ)
        استمر;

      selected[ڳڻپ++] = id;
    }
  }
#جيڪڏهن_پورو

  selected[ڳڻپ] = 0;

  إذا(ڳڻپ == 0) {
    curlx_free(selected);
    failf(معلومات, "mbedTLS: no supported cipher in list");
    أرجع CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  أرجع CURLE_OK;
}

ساكن فراغ mbed_dump_cert_info(بنية Curl_easy *معلومات,
                                ثابت mbedtls_x509_crt *crt)
{
#إذا !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (فراغ)معلومات, (فراغ)crt;
#وإلا
  ثابت size_t bufsize = 16384;
  حرف *p, *بفر = curlx_malloc(bufsize);

  إذا(بفر && mbedtls_x509_crt_info(بفر, bufsize, " ", crt) > 0) {
    infof(معلومات, "Server certificate:");
    لكل(p = بفر; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(معلومات, "%.*s", (صحيح)s, p);
      p += s;
    }
  }
  وإلا
    infof(معلومات, "Unable to dump certificate information");

  curlx_free(بفر);
#جيڪڏهن_پورو
}

ساكن فراغ mbed_ڪڍو_certinfo(بنية Curl_easy *معلومات,
                                  ثابت mbedtls_x509_crt *crt)
{
  CURLcode نتيجو;
  ثابت mbedtls_x509_crt *cur;
  صحيح cert_ڳڻپ = 0;
  صحيح i;

  لكل(cur = crt; cur && cert_ڳڻپ <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->ايندڙ)
    cert_ڳڻپ++;

  إذا(cert_ڳڻپ > MAX_ALLOWED_CERT_AMOUNT) {
    infof(معلومات, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    أرجع;
  }

  نتيجو = Curl_ssl_init_certinfo(معلومات, cert_ڳڻپ);

  لكل(i = 0, cur = crt; نتيجو == CURLE_OK && cur; ++i, cur = cur->ايندڙ) {
    نتيجو = Curl_ڪڍو_certinfo(معلومات, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ساكن صحيح mbed_verify_cb(فراغ *ptr, mbedtls_x509_crt *crt,
                          صحيح اونهائي, uint32_t *flags)
{
  بنية Curl_cfilter *cf = (بنية Curl_cfilter *)ptr;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);

  إذا(اونهائي == 0) {
    إذا(معلومات->سيٽ.verbose)
      mbed_dump_cert_info(معلومات, crt);
    إذا(معلومات->سيٽ.ssl.certinfo)
      mbed_ڪڍو_certinfo(معلومات, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  إذا(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  إذا(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  إذا(*flags) {
#جيڪڏهن_وصف_نه_ٿيل MBEDTLS_X509_REMOVE_INFO
    حرف buf[128];
    mbedtls_x509_crt_verify_info(buf, حجم(buf), "", *flags);
    failf(معلومات, "mbedTLS: %s", buf);
#وإلا
    failf(معلومات, "mbedTLS: certificate verification error 0x%08x",
          (بنانشان صحيح)*flags);
#جيڪڏهن_پورو
  }

  أرجع 0;
}

ساكن CURLcode mbed_ڀريو_cacert(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? عدم : conn_config->CAfile);
  ثابت bool verifypeer = conn_config->verifypeer;
  ثابت حرف * ثابت ssl_capath = conn_config->CApath;
#جيڪڏهن_وصف_ٿيل MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#جيڪڏهن_پورو
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  إذا(ca_info_blob && verifypeer) {
#جيڪڏهن_وصف_ٿيل MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    إذا((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((حرف *)(ca_info_blob->معلومات))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->معلومات,
                                   ca_info_blob->len);
    }
    وإلا { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      بنانشان حرف *newblob = curlx_memdup0(ca_info_blob->معلومات,
                                             ca_info_blob->len);
      إذا(!newblob)
        أرجع CURLE_ٻاهريون_جو_يادگيري;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#وإلا
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->معلومات,
                                     ca_info_blob->len);
#جيڪڏهن_پورو

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (بنانشان صحيح)-ret, errorbuf);
      أرجع CURLE_SSL_CERTPROBLEM;
    }
  }

  إذا(ssl_cafile && verifypeer) {
#جيڪڏهن_وصف_ٿيل MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (بنانشان صحيح)-ret, errorbuf);
      أرجع CURLE_SSL_CACERT_BADFILE;
    }
#وإلا
    failf(معلومات, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
  }

  إذا(ssl_capath) {
#جيڪڏهن_وصف_ٿيل MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_رستو(&backend->cacert, ssl_capath);

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (بنانشان صحيح)-ret, errorbuf);

      إذا(verifypeer)
        أرجع CURLE_SSL_CACERT_BADFILE;
    }
#وإلا
    failf(معلومات, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
  }

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_ڀريو_clicert(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  حرف * ثابت ssl_cert = conn_config->clientcert;
  ثابت بنية curl_blob *ssl_cert_blob = conn_config->cert_blob;
#جيڪڏهن_وصف_ٿيل MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#جيڪڏهن_پورو
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  إذا(ssl_cert) {
#جيڪڏهن_وصف_ٿيل MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (بنانشان صحيح)-ret, errorbuf);

      أرجع CURLE_SSL_CERTPROBLEM;
    }
#وإلا
    failf(معلومات, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
  }

  إذا(ssl_cert_blob) {
#جيڪڏهن_وصف_ٿيل MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    إذا((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((حرف *)(ssl_cert_blob->معلومات))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->معلومات,
                                   ssl_cert_blob->len);
    }
    وإلا { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      بنانشان حرف *newblob = curlx_memdup0(ssl_cert_blob->معلومات,
                                             ssl_cert_blob->len);
      إذا(!newblob)
        أرجع CURLE_ٻاهريون_جو_يادگيري;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#وإلا
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->معلومات,
                                     ssl_cert_blob->len);
#جيڪڏهن_پورو

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (بنانشان صحيح)-ret, errorbuf);
      أرجع CURLE_SSL_CERTPROBLEM;
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_ڀريو_privkey(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  إذا(conn_config->ڪنجي || conn_config->ڪنجي_blob) {
    إذا(conn_config->ڪنجي) {
#جيڪڏهن_وصف_ٿيل MBEDTLS_FS_IO
#إذا MBEDTLS_نسخو_عدد >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ڪنجي,
                                     conn_config->ڪنجي_passwd);
      إذا(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ڪنجي_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ڪنجي_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وإلا
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ڪنجي,
                                     conn_config->ڪنجي_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      إذا(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#جيڪڏهن_پورو

      إذا(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(معلومات, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->ڪنجي, (بنانشان صحيح)-ret, errorbuf);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
#وإلا
      failf(معلومات, "mbedTLS: functions that use the file system not built in");
      أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
    }
    وإلا {
      ثابت بنية curl_blob *ssl_ڪنجي_blob = conn_config->ڪنجي_blob;
      ثابت حرف *passwd = conn_config->ڪنجي_passwd;
      ثابت bool is_der = conn_config->ڪنجي_type &&
                          curl_strequal(conn_config->ڪنجي_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      بنانشان حرف *newblob = curlx_memdup0(ssl_ڪنجي_blob->معلومات,
                                             ssl_ڪنجي_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_ڪنجي_blob->len : ssl_ڪنجي_blob->len + 1;
      إذا(!newblob)
        أرجع CURLE_ٻاهريون_جو_يادگيري;

#إذا MBEDTLS_نسخو_عدد >= 0x04000000
      ret = mbedtls_pk_parse_ڪنجي(&backend->pk, newblob, keylen,
                                 (ثابت بنانشان حرف *)passwd,
                                 passwd ? لڙي_ڊيگهه(passwd) : 0);
      إذا(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ڪنجي_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ڪنجي_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وإلا
      ret = mbedtls_pk_parse_ڪنجي(&backend->pk, newblob, keylen,
                                 (ثابت بنانشان حرف *)passwd,
                                 passwd ? لڙي_ڊيگهه(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      إذا(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#جيڪڏهن_پورو
      curlx_free(newblob);

      إذا(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(معلومات, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (بنانشان صحيح)-ret, errorbuf);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_ڀريو_crl(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت حرف * ثابت ssl_crlfile = conn_config->CRLfile;

#جيڪڏهن_وصف_ٿيل MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  إذا(ssl_crlfile) {
    حرف errorbuf[128];
#جيڪڏهن_وصف_ٿيل MBEDTLS_FS_IO
    صحيح ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (بنانشان صحيح)-ret, errorbuf);

      أرجع CURLE_SSL_CRL_BADFILE;
    }
#وإلا
    (فراغ)errorbuf;
    failf(معلومات, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_اندريون;
#جيڪڏهن_پورو
  }
#وإلا
  (فراغ)backend;
  إذا(ssl_crlfile) {
    failf(معلومات, "mbedTLS: CRL support not built in");
    أرجع CURLE_NOT_BUILT_اندريون;
  }
#جيڪڏهن_پورو

  أرجع CURLE_OK;
}

ساكن bool mbed_apply_session(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية Curl_ssl_session *sc_session)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;

  إذا(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    صحيح ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_ڀريو(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    إذا(ret) {
      failf(معلومات, "SSL session error loading: -0x%x", (بنانشان صحيح)-ret);
    }
    وإلا {
      ret = mbedtls_ssl_سيٽ_session(&backend->ssl, &session);
      إذا(ret)
        failf(معلومات, "SSL session error setting: -0x%x", (بنانشان صحيح)-ret);
    }
    mbedtls_ssl_session_free(&session);
    إذا(!ret) {
      infof(معلومات, "SSL reusing session ID");
      أرجع TRUE;
    }
  }
  أرجع FALSE;
}

ساكن CURLcode mbed_configure_ssl(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح ret;
  bool session_applied = FALSE;
  CURLcode نتيجو;
  حرف errorbuf[128];

  infof(معلومات, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->پورٽ);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_وهڪرو,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  إذا(ret) {
    failf(معلومات, "mbedTLS: ssl_config failed");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

#جيڪڏهن_وصف_ٿيل MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, معلومات);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_سيٽ_حد(4);
#جيڪڏهن_پورو

#إذا defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_نسخو_عدد >= 0x03060100 &&   \
  MBEDTLS_نسخو_عدد < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#جيڪڏهن_پورو

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_ايندڙ);

  نتيجو = mbed_سيٽ_ssl_نسخو_min_max(معلومات, backend, conn_config);
  إذا(نتيجو)
    أرجع نتيجو;

#إذا MBEDTLS_نسخو_عدد < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#جيڪڏهن_پورو

  ret = mbedtls_ssl_تياري(&backend->ssl,
                          &backend->config);
  إذا(ret) {
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(معلومات, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (بنانشان صحيح)-ret, errorbuf);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_سيٽ_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      عدم /*  rev_timeout() */);

#جيڪڏهن_وصف_نه_ٿيل MBEDTLS_SSL_PROTO_TLS1_3
  إذا(conn_config->cipher_list) {
    نتيجو = mbed_سيٽ_selected_ciphers(معلومات, backend,
                                       conn_config->cipher_list, عدم);
#وإلا
  إذا(conn_config->cipher_list || conn_config->cipher_list13) {
    نتيجو = mbed_سيٽ_selected_ciphers(معلومات, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#جيڪڏهن_پورو
    إذا(نتيجو) {
      failf(معلومات, "mbedTLS: failed to set cipher suites");
      أرجع نتيجو;
    }
  }
  وإلا {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#جيڪڏهن_پورو

  إذا((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    بنية Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(معلومات, cf->cft, FIRSTSOCKET);
    إذا(scs) {
      إذا(mbed_apply_session(cf, معلومات, scs)) {
        CURL_TRC_CF(معلومات, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  إذا(!session_applied && Curl_ssl_scache_use(cf, معلومات)) {
    بنية Curl_ssl_session *sc_session = عدم;
    CURLcode sresult = Curl_ssl_scache_take(cf, معلومات, connssl->peer.scache_ڪنجي,
                                            &sc_session);
    إذا(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_ڀريو(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      إذا(ret) {
        failf(معلومات, "SSL session error loading: -0x%x", (بنانشان صحيح)-ret);
      }
      وإلا {
        ret = mbedtls_ssl_سيٽ_session(&backend->ssl, &session);
        إذا(ret)
          failf(معلومات, "SSL session error setting: -0x%x", (بنانشان صحيح)-ret);
        وإلا
          infof(معلومات, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, معلومات, connssl->peer.scache_ڪنجي,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#جيڪڏهن_وصف_ٿيل MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#وإلا
                            عدم
#جيڪڏهن_پورو
    );

  إذا(conn_config->ڪنجي || conn_config->ڪنجي_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  إذا(mbedtls_ssl_سيٽ_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(معلومات, "Failed to set SNI");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

#جيڪڏهن_وصف_ٿيل HAS_ALPN_MBEDTLS
  إذا(connssl->alpn) {
    بنية alpn_proto_buf proto;
    size_t i;

    لكل(i = 0; i < connssl->alpn->ڳڻپ; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    إذا(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(معلومات, "Failed setting ALPN protocols");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_منزل_proto_str(&proto, connssl->alpn);
    infof(معلومات, VTLS_INFOF_ALPN_OFFER_1STR, proto.معلومات);
  }
#جيڪڏهن_پورو

  /* give application a chance to interfere with mbedTLS set up. */
  إذا(معلومات->سيٽ.ssl_fsslctx) {
    نتيجو = (*معلومات->سيٽ.ssl_fsslctx)(معلومات, &backend->config,
                                      معلومات->سيٽ.ssl_fsslctxp);
    إذا(نتيجو)
      failf(معلومات, "error signaled by SSL ctx callback");
  }

  أرجع نتيجو;
}

ساكن CURLcode mbed_connect_step1(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتيجو;

  إذا((conn_config->نسخو_2 == CURL_SSLVERSION_SSLv2) ||
     (conn_config->نسخو_2 == CURL_SSLVERSION_SSLv3)) {
    failf(معلومات, "Not supported SSL version");
    أرجع CURLE_NOT_BUILT_اندريون;
  }

  نتيجو = mbed_ڀريو_cacert(cf, معلومات);
  إذا(!نتيجو)
    نتيجو = mbed_ڀريو_clicert(cf, معلومات);
  إذا(!نتيجو)
    نتيجو = mbed_ڀريو_privkey(cf, معلومات);
  إذا(!نتيجو)
    نتيجو = mbed_ڀريو_crl(cf, معلومات);
  إذا(!نتيجو)
    نتيجو = mbed_configure_ssl(cf, معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  connssl->connecting_state = ssl_connect_2;
  أرجع CURLE_OK;
}

#إذا defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#وصف_ڪريو HAVE_PINNED_PUBKEY
#جيڪڏهن_پورو

ساكن CURLcode mbed_connect_step2(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
#إذا defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode نتيجو;
#جيڪڏهن_پورو
  صحيح ret;
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
#جيڪڏهن_وصف_ٿيل HAVE_PINNED_PUBKEY
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
  ثابت حرف * ثابت pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY);
#وإلا
  ثابت حرف * ثابت pinnedpubkey =
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY);
#جيڪڏهن_پورو
#جيڪڏهن_پورو

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  إذا(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    أرجع CURLE_OK;
  }
  وإلا إذا(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    أرجع CURLE_OK;
  }
  وإلا إذا(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(معلومات, "peer certificate could not be verified");
    أرجع CURLE_PEER_FAILED_VERIFICATION;
  }
  وإلا إذا(ret) {
    حرف errorbuf[128];
    CURL_TRC_CF(معلومات, cf, "TLS version %04X",
                mbedtls_ssl_get_نسخو_عدد(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(معلومات, "ssl_handshake returned: (-0x%04X) %s",
          (بنانشان صحيح)-ret, errorbuf);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  {
    حرف cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, حجم(cipher_str), TRUE);
    infof(معلومات, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_نسخو(&backend->ssl), cipher_str);
  }

#جيڪڏهن_وصف_ٿيل HAVE_PINNED_PUBKEY
  إذا(pinnedpubkey) {
    صحيح سائيز;
    ثابت mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = عدم;
    بنانشان حرف *pubkey = عدم;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    إذا(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(معلومات, "Failed due to missing peer certificate");
      أرجع CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, حجم(*p));

    إذا(!p)
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    pubkey = curlx_malloc(PUB_DER_MAX_بائيٽون);

    إذا(!pubkey) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    إذا(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(معلومات, "Failed copying peer certificate");
      نتيجو = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      اذهب pinnedpubkey_error;
    }

    سائيز = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_بائيٽون);

    إذا(سائيز <= 0) {
      failf(معلومات, "Failed copying public key from peer certificate");
      نتيجو = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      اذهب pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    نتيجو = Curl_pin_peer_pubkey(معلومات,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_بائيٽون - سائيز], سائيز);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    إذا(نتيجو)
      أرجع نتيجو;
  }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAS_ALPN_MBEDTLS
  إذا(connssl->alpn) {
    ثابت حرف *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    نتيجو = Curl_alpn_سيٽ_negotiated(cf, معلومات, connssl,
                                      (ثابت بنانشان حرف *)proto,
                                      proto ? لڙي_ڊيگهه(proto) : 0);
    إذا(نتيجو)
      أرجع نتيجو;
  }
#جيڪڏهن_پورو

  connssl->connecting_state = ssl_connect_3;
  infof(معلومات, "SSL connected");

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_new_session(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  بنية Curl_ssl_session *sc_session = عدم;
  بنانشان حرف *sdata = عدم;
  size_t slen = 0;
  صحيح ietf_tls_id;
  CURLcode نتيجو = CURLE_OK;
  صحيح ret;

  DEBUGASSERT(backend);
  إذا(!Curl_ssl_scache_use(cf, معلومات))
    أرجع CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  إذا(ret) {
    failf(معلومات, "mbedtls_ssl_get_session returned -0x%x", (بنانشان صحيح)-ret);
    نتيجو = CURLE_SSL_CONNECT_ERROR;
    اذهب ٻاهريون;
  }

  mbedtls_ssl_session_save(&session, عدم, 0, &slen);
  إذا(!slen) {
    failf(معلومات, "failed to serialize session: length is 0");
    اذهب ٻاهريون;
  }

  sdata = curlx_malloc(slen);
  إذا(!sdata) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  إذا(ret) {
    failf(معلومات, "failed to serialize session: -0x%x", (بنانشان صحيح)-ret);
    اذهب ٻاهريون;
  }

  ietf_tls_id = mbedtls_ssl_get_نسخو_عدد(&backend->ssl);
  نتيجو = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = عدم;  /* call took ownership */
  إذا(!نتيجو && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    نتيجو = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  إذا(!نتيجو) {
    نتيجو = Curl_ssl_scache_put(cf, معلومات, connssl->peer.scache_ڪنجي,
                                 sc_session);
    sc_session = عدم;
  }

ٻاهريون:
  إذا(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  أرجع نتيجو;
}

ساكن CURLcode mbed_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          ثابت فراغ *mem, size_t len, size_t *pnwritten)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  CURLcode نتيجو = CURLE_OK;
  صحيح nwritten;

  DEBUGASSERT(backend);
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#جيڪڏهن_پورو
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  إذا(backend->send_blocked) {
    إذا(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(معلومات, "re-send data shorter than previously");
      نتيجو = CURLE_BAD_FUNCTION_دليل;
      اذهب ٻاهريون;
    }
    CURL_TRC_CF(معلومات, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ثابت بنانشان حرف *)mem, len);

  إذا(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (بنانشان صحيح)-nwritten);
    بدل(nwritten) {
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_3
    حالة MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      نتيجو = mbed_new_session(cf, معلومات);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      إذا(!نتيجو)
        اذهب do_send;
      اكسر;
#جيڪڏهن_پورو
    حالة MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      نتيجو = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتيجو = CURLE_AGAIN;
      اكسر;
    افتراضي:
      نتيجو = CURLE_SEND_ERROR;
      اكسر;
    }
    إذا((نتيجو == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

ٻاهريون:
  CURL_TRC_CF(معلومات, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (صحيح)نتيجو, *pnwritten);
  أرجع نتيجو;
}

ساكن CURLcode mbedtls_shutdown(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool send_shutdown, bool *پورو)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بنانشان حرف buf[1024];
  CURLcode نتيجو = CURLE_OK;
  صحيح ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  إذا(!backend->initialized || cf->shutdown) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *پورو = FALSE;

  إذا(!backend->موڪليل_shutdown) {
    /* do this only once */
    backend->موڪليل_shutdown = TRUE;
    إذا(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      بدل(ret) {
      حالة 0: /* we sent it, receive from the server */
        اكسر;
      حالة MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *پورو = TRUE;
        اذهب ٻاهريون;
      حالة MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        اذهب ٻاهريون;
      حالة MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        اذهب ٻاهريون;
      افتراضي:
        CURL_TRC_CF(معلومات, cf, "mbedtls_shutdown error -0x%04X",
                    (بنانشان صحيح)-ret);
        نتيجو = CURLE_RECV_ERROR;
        اذهب ٻاهريون;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  لكل(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, حجم(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    إذا(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, حجم(buf));
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_3
    إذا(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      استمر;
#جيڪڏهن_پورو
    إذا(ret <= 0)
      اكسر;
  }

  إذا(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(معلومات, cf, "mbedtls_shutdown, still getting data");
  }
  وإلا إذا(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(معلومات, cf, "mbedtls_shutdown done");
    *پورو = TRUE;
  }
  وإلا إذا(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(معلومات, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  وإلا إذا(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(معلومات, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "mbedtls_shutdown error -0x%04X",
                (بنانشان صحيح)-ret);
    نتيجو = CURLE_RECV_ERROR;
  }

ٻاهريون:
  cf->shutdown = (نتيجو || *پورو);
  أرجع نتيجو;
}

ساكن فراغ mbedtls_close(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;

  (فراغ)معلومات;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#جيڪڏهن_وصف_ٿيل MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#جيڪڏهن_پورو
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  إذا(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

ساكن CURLcode mbed_recv(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          حرف *buf, size_t buffersize, size_t *pnread)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  CURLcode نتيجو = CURLE_OK;
  صحيح nread;

  DEBUGASSERT(backend);
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_SESSION_TICKETS
do_read:
#جيڪڏهن_پورو
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (بنانشان حرف *)buf, buffersize);
  إذا(nread > 0)
    *pnread = (size_t)nread;
  وإلا {
    حرف errorbuf[128];
    CURL_TRC_CF(معلومات, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (بنانشان صحيح)-nread);
    بدل(nread) {
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_SESSION_TICKETS
    حالة MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      نتيجو = mbed_new_session(cf, معلومات);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      إذا(!نتيجو)
        اذهب do_read;
      اكسر;
#جيڪڏهن_پورو
    حالة MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      نتيجو = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتيجو = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      نتيجو = CURLE_OK;
      اكسر;
    افتراضي:
      mbedtls_strerror(nread, errorbuf, حجم(errorbuf));
      failf(معلومات, "ssl_read returned: (-0x%04X) %s", (بنانشان صحيح)-nread,
            errorbuf);
      نتيجو = CURLE_RECV_ERROR;
      اكسر;
    }
  }
  أرجع نتيجو;
}

ساكن size_t mbedtls_نسخو(حرف *بفر, size_t سائيز)
{
#جيڪڏهن_وصف_ٿيل MBEDTLS_نسخو_C
  /* if mbedtls_version_get_number() is available it is better */
  بنانشان صحيح نسخو_2 = mbedtls_نسخو_get_عدد();
  أرجع curl_msnprintf(بفر, سائيز, "mbedTLS/%u.%u.%u", نسخو_2 >> 24,
                        (نسخو_2 >> 16) & 0xff, (نسخو_2 >> 8) & 0xff);
#وإلا
  أرجع curl_msnprintf(بفر, سائيز, "mbedTLS/%s", MBEDTLS_نسخو_STRING);
#جيڪڏهن_پورو
}

/* 'data' might be NULL */
ساكن CURLcode mbedtls_random(بنية Curl_easy *معلومات,
                               بنانشان حرف *entropy, size_t ڊيگهائي)
{
  psa_حالت_t حالت;
  (فراغ)معلومات;

  حالت = psa_generate_random(entropy, ڊيگهائي);

  أرجع حالت == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ساكن CURLcode mbedtls_connect(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                bool *پورو)
{
  CURLcode نتيجو;
  بنية ssl_connect_معلومات *connssl = cf->ctx;

  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  *پورو = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  إذا(connssl->connecting_state == ssl_connect_1) {
    نتيجو = mbed_connect_step1(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
    نتيجو = mbed_connect_step2(cf, معلومات);
    إذا(نتيجو)
      أرجع نتيجو;
  }

  إذا(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    بنية ssl_connect_معلومات *ctx = cf->ctx;
    بنية mbed_ssl_backend_معلومات *backend =
      (بنية mbed_ssl_backend_معلومات *)ctx->backend;

    إذا(mbedtls_ssl_get_نسخو_عدد(&backend->ssl) <=
       MBEDTLS_SSL_نسخو_TLS1_2) {
      نتيجو = mbed_new_session(cf, معلومات);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    connssl->connecting_state = ssl_connect_پورو;
  }

  إذا(connssl->connecting_state == ssl_connect_پورو) {
    connssl->state = ssl_connection_complete;
    *پورو = TRUE;
  }

  أرجع CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ساكن صحيح mbedtls_init(فراغ)
{
#إذا MBEDTLS_نسخو_عدد < 0x04000000
  صحيح ret = 0;
#جيڪڏهن_پورو
  psa_حالت_t حالت;
  حالت = psa_crypto_init();

  إذا(حالت != PSA_SUCCESS)
    أرجع 0;

#إذا MBEDTLS_نسخو_عدد < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_ٻج(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              عدم, 0);

  إذا(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    أرجع 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_سيٽ_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#جيڪڏهن_پورو
  أرجع 1;
}

ساكن فراغ mbedtls_cleanup(فراغ)
{
  mbedtls_psa_crypto_free();

#إذا MBEDTLS_نسخو_عدد < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#جيڪڏهن_پورو
}

ساكن bool mbedtls_معلومات_pending(بنية Curl_cfilter *cf,
                                 ثابت بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *ctx = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend;

  (فراغ)معلومات;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية mbed_ssl_backend_معلومات *)ctx->backend;
  أرجع mbedtls_ssl_get_بائيٽون_avail(&backend->ssl) != 0;
}

ساكن CURLcode mbedtls_sha256sum(ثابت بنانشان حرف *داخلا,
                                  size_t inputlen,
                                  بنانشان حرف *sha256sum,
                                  size_t sha256len)
{
#إذا defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_حالت_t حالت;
  size_t sha256len_actual;
  حالت = psa_hash_compute(PSA_ALG_SHA_256, داخلا, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  إذا(حالت != PSA_SUCCESS)
    أرجع CURLE_BAD_FUNCTION_دليل;
  أرجع CURLE_OK;
#وإلا
  (فراغ)sha256len;
  أرجع Curl_sha256it(sha256sum, داخلا, inputlen);
#جيڪڏهن_پورو
}

ساكن فراغ *mbedtls_get_internals(بنية ssl_connect_معلومات *connssl,
                                   CURLINFO info)
{
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع &backend->ssl;
}

ثابت بنية Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_رستو |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#جيڪڏهن_وصف_ٿيل HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#جيڪڏهن_پورو
  SSLSUPP_SSL_CTX |
#جيڪڏهن_وصف_ٿيل MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#جيڪڏهن_پورو
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#جيڪڏهن_وصف_ٿيل MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#جيڪڏهن_پورو
  0,

  حجم(بنية mbed_ssl_backend_معلومات),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_نسخو,                  /*نسخو*/
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_معلومات_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  عدم,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  عدم,                             /* close_all */
  عدم,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  عدم,                             /* get_channel_binding */
};

#جيڪڏهن_پورو /* USE_MBEDTLS */
