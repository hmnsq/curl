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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#شامل <mbedtls/version.h>
#اگر MBEDTLS_نسخہ_نمبر < 0x03020000
#غلطی "mbedTLS 3.2.0 or greater required"
#اگر_ختم
#شامل <psa/crypto_config.h>
#شامل <mbedtls/net_sockets.h>
#شامل <mbedtls/ssl.h>
#شامل <mbedtls/x509.h>
#شامل <mbedtls/psa_util.h>

#اگر MBEDTLS_نسخہ_نمبر < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#غلطی "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#اگر_ختم

#شامل <mbedtls/error.h>
#اگر MBEDTLS_نسخہ_نمبر < 0x04000000
#شامل <mbedtls/entropy.h>
#شامل <mbedtls/ctr_drbg.h>
#اگر_ختم
#اگر_تعریف_شدہ MBEDTLS_DEBUG
#شامل <mbedtls/debug.h>
#اگر_ختم

#شامل "vtls/cipher_suite.h"
#شامل "urldata.h"
#شامل "curl_trc.h"
#شامل "vtls/mbedtls.h"
#شامل "vtls/vtls.h"
#شامل "vtls/vtls_int.h"
#شامل "vtls/vtls_scache.h"
#شامل "vtls/x509asn1.h"
#شامل "connect.h" /* for the connect timeout */
#شامل "curlx/strdup.h"
#شامل "curl_sha256.h"

/* ALPN for http2 */
#اگر defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#تعریف_کرو HAS_ALPN_MBEDTLS
#اگر_ختم

ساخت mbed_ssl_backend_ڈیٹا {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#اگر_تعریف_شدہ MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#اگر_ختم
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#اگر_تعریف_شدہ HAS_ALPN_MBEDTLS
  ثابت حرف *protocols[3];
#اگر_ختم
  صحیح *ciphersuites;
  size_t send_blocked_len;
  ثنائی_ہندسہ(initialized); /* mbedtls_ssl_context is initialized */
  ثنائی_ہندسہ(بھیجا_shutdown);
  ثنائی_ہندسہ(send_blocked);
};

/** A context for random number generation (RNG).
 */
#اگر MBEDTLS_نسخہ_نمبر < 0x04000000
ساخت rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ساکن ساخت rng_context_t rng;
#اگر_ختم

#اگر_غیر_تعریف_شدہ MBEDTLS_ERROR_C
#تعریف_کرو mbedtls_strerror(a, b, c) b[0] = 0
#اگر_ختم

#اگر_تعریف_شدہ MBEDTLS_DEBUG
ساکن خالی mbed_debug(خالی *context, صحیح level, ثابت حرف *f_name,
                       صحیح سطر_nb, ثابت حرف *سطر)
{
  ساخت Curl_easy *ڈیٹا = (ساخت Curl_easy *)context;
  (خالی)level;
  (خالی)سطر_nb;
  (خالی)f_name;

  اگر(ڈیٹا) {
    size_t len = لڑی_لمبائی(سطر);
    اگر(len && (سطر[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(ڈیٹا, "%.*s", (صحیح)len, سطر);
  }
}
#اگر_ختم

ساکن صحیح mbedtls_bio_cf_write(خالی *bio,
                                ثابت غیرعلامتی حرف *buf, size_t blen)
{
  ساخت Curl_cfilter *cf = bio;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا);
  اگر(!ڈیٹا)
    واپس 0;

  نتیجہ = Curl_conn_cf_send(cf->اگلا, ڈیٹا, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(ڈیٹا, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (صحیح)نتیجہ, nwritten);
  اگر(نتیجہ == CURLE_AGAIN)
    واپس MBEDTLS_ERR_SSL_WANT_WRITE;
  واپس نتیجہ ? -1 : (صحیح)nwritten;
}

ساکن صحیح mbedtls_bio_cf_read(خالی *bio, غیرعلامتی حرف *buf, size_t blen)
{
  ساخت Curl_cfilter *cf = bio;
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);
  size_t nread = 0;
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا);
  اگر(!ڈیٹا)
    واپس 0;
  /* OpenSSL catches this case, so should we. */
  اگر(!buf)
    واپس 0;

  نتیجہ = Curl_conn_cf_recv(cf->اگلا, ڈیٹا, (حرف *)buf, blen, &nread);
  CURL_TRC_CF(ڈیٹا, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (صحیح)نتیجہ, nread);
  اگر(نتیجہ == CURLE_AGAIN)
    واپس MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  واپس نتیجہ ? -1 : (صحیح)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#تعریف_کرو RSA_PUB_DER_MAX_بائٹیں   (38 + (2 * MBEDTLS_MPI_MAX_سائز))
#تعریف_کرو ECP_PUB_DER_MAX_بائٹیں   (30 + (2 * MBEDTLS_ECP_MAX_بائٹیں))

#تعریف_کرو PUB_DER_MAX_بائٹیں   (RSA_PUB_DER_MAX_بائٹیں > ECP_PUB_DER_MAX_بائٹیں ? \
                             RSA_PUB_DER_MAX_بائٹیں : ECP_PUB_DER_MAX_بائٹیں)

ساکن CURLcode mbed_سیٹ_ssl_نسخہ_min_max(
  ساخت Curl_easy *ڈیٹا,
  ساخت mbed_ssl_backend_ڈیٹا *backend,
  ساخت ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_نسخہ ver_min =
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_نسخہ_TLS1_2
#ورنہ
    MBEDTLS_SSL_نسخہ_TLS1_3
#اگر_ختم
    ;
  mbedtls_ssl_protocol_نسخہ ver_max =
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_نسخہ_TLS1_3
#ورنہ
    MBEDTLS_SSL_نسخہ_TLS1_2
#اگر_ختم
    ;

  DEBUGASSERT(conn_config->نسخہ_2 != CURL_SSLVERSION_DEFAULT);
  انتخاب(conn_config->نسخہ_2) {
  حالت CURL_SSLVERSION_TLSv1:
  حالت CURL_SSLVERSION_TLSv1_0:
  حالت CURL_SSLVERSION_TLSv1_1:
  حالت CURL_SSLVERSION_TLSv1_2:
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_نسخہ_TLS1_2;
    توڑو;
#اگر_ختم
  حالت CURL_SSLVERSION_TLSv1_3:
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_نسخہ_TLS1_3;
    توڑو;
#اگر_ختم
  طےشدہ:
    failf(ڈیٹا, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->نسخہ_2);
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  انتخاب(conn_config->نسخہ_max) {
  حالت CURL_SSLVERSION_MAX_DEFAULT:
  حالت CURL_SSLVERSION_MAX_NONE:
  حالت CURL_SSLVERSION_MAX_TLSv1_3:
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_نسخہ_TLS1_3;
    توڑو;
#اگر_ختم
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_2
  حالت CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_نسخہ_TLS1_2;
    توڑو;
#اگر_ختم
  حالت CURL_SSLVERSION_MAX_TLSv1_1:
  حالت CURL_SSLVERSION_MAX_TLSv1_0:
  طےشدہ:
    failf(ڈیٹا, "mbedTLS: unsupported maximum TLS version value");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_نسخہ(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_نسخہ(&backend->config, ver_max);

  واپس CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#اگر_تعریف_شدہ MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ساکن صحیح mbed_cipher_suite_get_str(uint16_t id, حرف *buf, size_t buf_سائز,
                                     bool prefer_rfc)
{
  اگر(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_سائز, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  ورنہ
    واپس Curl_cipher_suite_get_str(id, buf, buf_سائز, prefer_rfc);
  واپس 0;
}

ساکن uint16_t mbed_cipher_suite_walk_str(ثابت حرف **str, ثابت حرف **انتہا)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, انتہا);
  size_t len = *انتہا - *str;
  ساکن ثابت حرف ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  اگر(!id) {
    اگر((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  واپس id;
}
#ورنہ
#تعریف_کرو mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#تعریف_کرو mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#اگر_ختم

ساکن CURLcode mbed_سیٹ_selected_ciphers(
  ساخت Curl_easy *ڈیٹا,
  ساخت mbed_ssl_backend_ڈیٹا *backend,
  ثابت حرف *ciphers12,
  ثابت حرف *ciphers13)
{
  ثابت حرف *ciphers = ciphers12;
  ثابت صحیح *supported;
  صحیح *selected;
  size_t supported_len, گنتی = 0, default13_گنتی = 0, i, j;
  ثابت حرف *ptr, *انتہا;

  supported = mbedtls_ssl_list_ciphersuites();
  برائے(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(حجم(صحیح) * (supported_len + 1));
  اگر(!selected)
    واپس CURLE_برآمد_کا_حافظہ;

#اگر_غیر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3
  (خالی)ciphers13, (خالی)j;
#ورنہ
  اگر(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    برائے(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      اگر(لڑی_موازنہ(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        جاری;

      selected[گنتی++] = id;
    }

    default13_گنتی = گنتی;
  }
  ورنہ
    ciphers = ciphers13;

جوڑو_ciphers:
#اگر_ختم
  برائے(ptr = ciphers; ptr[0] != '\0' && گنتی < supported_len; ptr = انتہا) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &انتہا);

    /* Check if cipher is supported */
    اگر(id) {
      برائے(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      اگر(i == supported_len)
        id = 0;
    }
    اگر(!id) {
      اگر(ptr[0] != '\0')
        infof(ڈیٹا, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (صحیح)(انتہا - ptr), ptr);
      جاری;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    برائے(i = 0; i < گنتی && selected[i] != id; i++)
      ;
    اگر(i < گنتی) {
      اگر(i >= default13_گنتی)
        infof(ڈیٹا, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (صحیح)(انتہا - ptr), ptr);
      جاری;
    }

    selected[گنتی++] = id;
  }

#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3
  اگر(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    جاؤ جوڑو_ciphers;
  }

  اگر(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    برائے(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      اگر(!لڑی_موازنہ(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        جاری;

      /* No duplicates allowed (so selected cannot overflow) */
      برائے(i = 0; i < گنتی && selected[i] != id; i++)
        ;
      اگر(i < گنتی)
        جاری;

      selected[گنتی++] = id;
    }
  }
#اگر_ختم

  selected[گنتی] = 0;

  اگر(گنتی == 0) {
    curlx_free(selected);
    failf(ڈیٹا, "mbedTLS: no supported cipher in list");
    واپس CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  واپس CURLE_OK;
}

ساکن خالی mbed_dump_cert_info(ساخت Curl_easy *ڈیٹا,
                                ثابت mbedtls_x509_crt *crt)
{
#اگر !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (خالی)ڈیٹا, (خالی)crt;
#ورنہ
  ثابت size_t bufsize = 16384;
  حرف *p, *بفر = curlx_malloc(bufsize);

  اگر(بفر && mbedtls_x509_crt_info(بفر, bufsize, " ", crt) > 0) {
    infof(ڈیٹا, "Server certificate:");
    برائے(p = بفر; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(ڈیٹا, "%.*s", (صحیح)s, p);
      p += s;
    }
  }
  ورنہ
    infof(ڈیٹا, "Unable to dump certificate information");

  curlx_free(بفر);
#اگر_ختم
}

ساکن خالی mbed_نکالو_certinfo(ساخت Curl_easy *ڈیٹا,
                                  ثابت mbedtls_x509_crt *crt)
{
  CURLcode نتیجہ;
  ثابت mbedtls_x509_crt *cur;
  صحیح cert_گنتی = 0;
  صحیح i;

  برائے(cur = crt; cur && cert_گنتی <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->اگلا)
    cert_گنتی++;

  اگر(cert_گنتی > MAX_ALLOWED_CERT_AMOUNT) {
    infof(ڈیٹا, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    واپس;
  }

  نتیجہ = Curl_ssl_init_certinfo(ڈیٹا, cert_گنتی);

  برائے(i = 0, cur = crt; نتیجہ == CURLE_OK && cur; ++i, cur = cur->اگلا) {
    نتیجہ = Curl_نکالو_certinfo(ڈیٹا, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ساکن صحیح mbed_verify_cb(خالی *ptr, mbedtls_x509_crt *crt,
                          صحیح گہرائی, uint32_t *flags)
{
  ساخت Curl_cfilter *cf = (ساخت Curl_cfilter *)ptr;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت Curl_easy *ڈیٹا = CF_ڈیٹا_CURRENT(cf);

  اگر(گہرائی == 0) {
    اگر(ڈیٹا->سیٹ.verbose)
      mbed_dump_cert_info(ڈیٹا, crt);
    اگر(ڈیٹا->سیٹ.ssl.certinfo)
      mbed_نکالو_certinfo(ڈیٹا, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  اگر(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  اگر(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  اگر(*flags) {
#اگر_غیر_تعریف_شدہ MBEDTLS_X509_REMOVE_INFO
    حرف buf[128];
    mbedtls_x509_crt_verify_info(buf, حجم(buf), "", *flags);
    failf(ڈیٹا, "mbedTLS: %s", buf);
#ورنہ
    failf(ڈیٹا, "mbedTLS: certificate verification error 0x%08x",
          (غیرعلامتی صحیح)*flags);
#اگر_ختم
  }

  واپس 0;
}

ساکن CURLcode mbed_بھرو_cacert(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت ساخت curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? تہی : conn_config->CAfile);
  ثابت bool verifypeer = conn_config->verifypeer;
  ثابت حرف * ثابت ssl_capath = conn_config->CApath;
#اگر_تعریف_شدہ MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#اگر_ختم
  صحیح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  اگر(ca_info_blob && verifypeer) {
#اگر_تعریف_شدہ MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    اگر((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((حرف *)(ca_info_blob->ڈیٹا))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->ڈیٹا,
                                   ca_info_blob->len);
    }
    ورنہ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      غیرعلامتی حرف *newblob = curlx_memdup0(ca_info_blob->ڈیٹا,
                                             ca_info_blob->len);
      اگر(!newblob)
        واپس CURLE_برآمد_کا_حافظہ;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#ورنہ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->ڈیٹا,
                                     ca_info_blob->len);
#اگر_ختم

    اگر(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(ڈیٹا, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (غیرعلامتی صحیح)-ret, errorbuf);
      واپس CURLE_SSL_CERTPROBLEM;
    }
  }

  اگر(ssl_cafile && verifypeer) {
#اگر_تعریف_شدہ MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    اگر(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(ڈیٹا, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (غیرعلامتی صحیح)-ret, errorbuf);
      واپس CURLE_SSL_CACERT_BADFILE;
    }
#ورنہ
    failf(ڈیٹا, "mbedTLS: functions that use the file system not built in");
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  }

  اگر(ssl_capath) {
#اگر_تعریف_شدہ MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_راستہ(&backend->cacert, ssl_capath);

    اگر(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(ڈیٹا, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (غیرعلامتی صحیح)-ret, errorbuf);

      اگر(verifypeer)
        واپس CURLE_SSL_CACERT_BADFILE;
    }
#ورنہ
    failf(ڈیٹا, "mbedTLS: functions that use the file system not built in");
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  }

  واپس CURLE_OK;
}

ساکن CURLcode mbed_بھرو_clicert(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  حرف * ثابت ssl_cert = conn_config->clientcert;
  ثابت ساخت curl_blob *ssl_cert_blob = conn_config->cert_blob;
#اگر_تعریف_شدہ MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#اگر_ختم
  صحیح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  اگر(ssl_cert) {
#اگر_تعریف_شدہ MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    اگر(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(ڈیٹا, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (غیرعلامتی صحیح)-ret, errorbuf);

      واپس CURLE_SSL_CERTPROBLEM;
    }
#ورنہ
    failf(ڈیٹا, "mbedTLS: functions that use the file system not built in");
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  }

  اگر(ssl_cert_blob) {
#اگر_تعریف_شدہ MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    اگر((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((حرف *)(ssl_cert_blob->ڈیٹا))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->ڈیٹا,
                                   ssl_cert_blob->len);
    }
    ورنہ { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      غیرعلامتی حرف *newblob = curlx_memdup0(ssl_cert_blob->ڈیٹا,
                                             ssl_cert_blob->len);
      اگر(!newblob)
        واپس CURLE_برآمد_کا_حافظہ;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#ورنہ
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->ڈیٹا,
                                     ssl_cert_blob->len);
#اگر_ختم

    اگر(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(ڈیٹا, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (غیرعلامتی صحیح)-ret, errorbuf);
      واپس CURLE_SSL_CERTPROBLEM;
    }
  }

  واپس CURLE_OK;
}

ساکن CURLcode mbed_بھرو_privkey(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحیح ret = -1;
  حرف errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  اگر(conn_config->کلید || conn_config->کلید_blob) {
    اگر(conn_config->کلید) {
#اگر_تعریف_شدہ MBEDTLS_FS_IO
#اگر MBEDTLS_نسخہ_نمبر >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->کلید,
                                     conn_config->کلید_passwd);
      اگر(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ورنہ
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->کلید,
                                     conn_config->کلید_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      اگر(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#اگر_ختم

      اگر(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(ڈیٹا, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->کلید, (غیرعلامتی صحیح)-ret, errorbuf);
        واپس CURLE_SSL_CERTPROBLEM;
      }
#ورنہ
      failf(ڈیٹا, "mbedTLS: functions that use the file system not built in");
      واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
    }
    ورنہ {
      ثابت ساخت curl_blob *ssl_کلید_blob = conn_config->کلید_blob;
      ثابت حرف *passwd = conn_config->کلید_passwd;
      ثابت bool is_der = conn_config->کلید_type &&
                          curl_strequal(conn_config->کلید_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      غیرعلامتی حرف *newblob = curlx_memdup0(ssl_کلید_blob->ڈیٹا,
                                             ssl_کلید_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_کلید_blob->len : ssl_کلید_blob->len + 1;
      اگر(!newblob)
        واپس CURLE_برآمد_کا_حافظہ;

#اگر MBEDTLS_نسخہ_نمبر >= 0x04000000
      ret = mbedtls_pk_parse_کلید(&backend->pk, newblob, keylen,
                                 (ثابت غیرعلامتی حرف *)passwd,
                                 passwd ? لڑی_لمبائی(passwd) : 0);
      اگر(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#ورنہ
      ret = mbedtls_pk_parse_کلید(&backend->pk, newblob, keylen,
                                 (ثابت غیرعلامتی حرف *)passwd,
                                 passwd ? لڑی_لمبائی(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      اگر(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#اگر_ختم
      curlx_free(newblob);

      اگر(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(ڈیٹا, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (غیرعلامتی صحیح)-ret, errorbuf);
        واپس CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  واپس CURLE_OK;
}

ساکن CURLcode mbed_بھرو_crl(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت حرف * ثابت ssl_crlfile = conn_config->CRLfile;

#اگر_تعریف_شدہ MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  اگر(ssl_crlfile) {
    حرف errorbuf[128];
#اگر_تعریف_شدہ MBEDTLS_FS_IO
    صحیح ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    اگر(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(ڈیٹا, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (غیرعلامتی صحیح)-ret, errorbuf);

      واپس CURLE_SSL_CRL_BADFILE;
    }
#ورنہ
    (خالی)errorbuf;
    failf(ڈیٹا, "mbedTLS: functions that use the file system not built in");
    واپس CURLE_NOT_BUILT_اندر;
#اگر_ختم
  }
#ورنہ
  (خالی)backend;
  اگر(ssl_crlfile) {
    failf(ڈیٹا, "mbedTLS: CRL support not built in");
    واپس CURLE_NOT_BUILT_اندر;
  }
#اگر_ختم

  واپس CURLE_OK;
}

ساکن bool mbed_apply_session(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت Curl_ssl_session *sc_session)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;

  اگر(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    صحیح ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_بھرو(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    اگر(ret) {
      failf(ڈیٹا, "SSL session error loading: -0x%x", (غیرعلامتی صحیح)-ret);
    }
    ورنہ {
      ret = mbedtls_ssl_سیٹ_session(&backend->ssl, &session);
      اگر(ret)
        failf(ڈیٹا, "SSL session error setting: -0x%x", (غیرعلامتی صحیح)-ret);
    }
    mbedtls_ssl_session_free(&session);
    اگر(!ret) {
      infof(ڈیٹا, "SSL reusing session ID");
      واپس TRUE;
    }
  }
  واپس FALSE;
}

ساکن CURLcode mbed_configure_ssl(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحیح ret;
  bool session_applied = FALSE;
  CURLcode نتیجہ;
  حرف errorbuf[128];

  infof(ڈیٹا, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->پورٹ);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_دھارا,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  اگر(ret) {
    failf(ڈیٹا, "mbedTLS: ssl_config failed");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

#اگر_تعریف_شدہ MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, ڈیٹا);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_سیٹ_حد(4);
#اگر_ختم

#اگر defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_نسخہ_نمبر >= 0x03060100 &&   \
  MBEDTLS_نسخہ_نمبر < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#اگر_ختم

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_اگلا);

  نتیجہ = mbed_سیٹ_ssl_نسخہ_min_max(ڈیٹا, backend, conn_config);
  اگر(نتیجہ)
    واپس نتیجہ;

#اگر MBEDTLS_نسخہ_نمبر < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#اگر_ختم

  ret = mbedtls_ssl_تیاری(&backend->ssl,
                          &backend->config);
  اگر(ret) {
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(ڈیٹا, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (غیرعلامتی صحیح)-ret, errorbuf);
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_سیٹ_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      تہی /*  rev_timeout() */);

#اگر_غیر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3
  اگر(conn_config->cipher_list) {
    نتیجہ = mbed_سیٹ_selected_ciphers(ڈیٹا, backend,
                                       conn_config->cipher_list, تہی);
#ورنہ
  اگر(conn_config->cipher_list || conn_config->cipher_list13) {
    نتیجہ = mbed_سیٹ_selected_ciphers(ڈیٹا, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#اگر_ختم
    اگر(نتیجہ) {
      failf(ڈیٹا, "mbedTLS: failed to set cipher suites");
      واپس نتیجہ;
    }
  }
  ورنہ {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#اگر_تعریف_شدہ MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#اگر_ختم

#اگر_تعریف_شدہ MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#اگر_ختم

  اگر((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    ساخت Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(ڈیٹا, cf->cft, FIRSTSOCKET);
    اگر(scs) {
      اگر(mbed_apply_session(cf, ڈیٹا, scs)) {
        CURL_TRC_CF(ڈیٹا, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  اگر(!session_applied && Curl_ssl_scache_use(cf, ڈیٹا)) {
    ساخت Curl_ssl_session *sc_session = تہی;
    CURLcode sresult = Curl_ssl_scache_take(cf, ڈیٹا, connssl->peer.scache_کلید,
                                            &sc_session);
    اگر(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_بھرو(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      اگر(ret) {
        failf(ڈیٹا, "SSL session error loading: -0x%x", (غیرعلامتی صحیح)-ret);
      }
      ورنہ {
        ret = mbedtls_ssl_سیٹ_session(&backend->ssl, &session);
        اگر(ret)
          failf(ڈیٹا, "SSL session error setting: -0x%x", (غیرعلامتی صحیح)-ret);
        ورنہ
          infof(ڈیٹا, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, ڈیٹا, connssl->peer.scache_کلید,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#اگر_تعریف_شدہ MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#ورنہ
                            تہی
#اگر_ختم
    );

  اگر(conn_config->کلید || conn_config->کلید_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  اگر(mbedtls_ssl_سیٹ_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(ڈیٹا, "Failed to set SNI");
    واپس CURLE_SSL_CONNECT_ERROR;
  }

#اگر_تعریف_شدہ HAS_ALPN_MBEDTLS
  اگر(connssl->alpn) {
    ساخت alpn_proto_buf proto;
    size_t i;

    برائے(i = 0; i < connssl->alpn->گنتی; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    اگر(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(ڈیٹا, "Failed setting ALPN protocols");
      واپس CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_مقصد_proto_str(&proto, connssl->alpn);
    infof(ڈیٹا, VTLS_INFOF_ALPN_OFFER_1STR, proto.ڈیٹا);
  }
#اگر_ختم

  /* give application a chance to interfere with mbedTLS set up. */
  اگر(ڈیٹا->سیٹ.ssl_fsslctx) {
    نتیجہ = (*ڈیٹا->سیٹ.ssl_fsslctx)(ڈیٹا, &backend->config,
                                      ڈیٹا->سیٹ.ssl_fsslctxp);
    اگر(نتیجہ)
      failf(ڈیٹا, "error signaled by SSL ctx callback");
  }

  واپس نتیجہ;
}

ساکن CURLcode mbed_connect_step1(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ;

  اگر((conn_config->نسخہ_2 == CURL_SSLVERSION_SSLv2) ||
     (conn_config->نسخہ_2 == CURL_SSLVERSION_SSLv3)) {
    failf(ڈیٹا, "Not supported SSL version");
    واپس CURLE_NOT_BUILT_اندر;
  }

  نتیجہ = mbed_بھرو_cacert(cf, ڈیٹا);
  اگر(!نتیجہ)
    نتیجہ = mbed_بھرو_clicert(cf, ڈیٹا);
  اگر(!نتیجہ)
    نتیجہ = mbed_بھرو_privkey(cf, ڈیٹا);
  اگر(!نتیجہ)
    نتیجہ = mbed_بھرو_crl(cf, ڈیٹا);
  اگر(!نتیجہ)
    نتیجہ = mbed_configure_ssl(cf, ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  connssl->connecting_state = ssl_connect_2;
  واپس CURLE_OK;
}

#اگر defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#تعریف_کرو HAVE_PINNED_PUBKEY
#اگر_ختم

ساکن CURLcode mbed_connect_step2(ساخت Curl_cfilter *cf,
                                   ساخت Curl_easy *ڈیٹا)
{
#اگر defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode نتیجہ;
#اگر_ختم
  صحیح ret;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
#اگر_تعریف_شدہ HAVE_PINNED_PUBKEY
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
  ثابت حرف * ثابت pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY);
#ورنہ
  ثابت حرف * ثابت pinnedpubkey =
    CURL_EASY_STR(ڈیٹا, STRING_SSL_PINNEDPUBLICKEY);
#اگر_ختم
#اگر_ختم

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  اگر(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    واپس CURLE_OK;
  }
  ورنہ اگر(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    واپس CURLE_OK;
  }
  ورنہ اگر(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(ڈیٹا, "peer certificate could not be verified");
    واپس CURLE_PEER_FAILED_VERIFICATION;
  }
  ورنہ اگر(ret) {
    حرف errorbuf[128];
    CURL_TRC_CF(ڈیٹا, cf, "TLS version %04X",
                mbedtls_ssl_get_نسخہ_نمبر(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(ڈیٹا, "ssl_handshake returned: (-0x%04X) %s",
          (غیرعلامتی صحیح)-ret, errorbuf);
    واپس CURLE_SSL_CONNECT_ERROR;
  }

  {
    حرف cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, حجم(cipher_str), TRUE);
    infof(ڈیٹا, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_نسخہ(&backend->ssl), cipher_str);
  }

#اگر_تعریف_شدہ HAVE_PINNED_PUBKEY
  اگر(pinnedpubkey) {
    صحیح سائز;
    ثابت mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = تہی;
    غیرعلامتی حرف *pubkey = تہی;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    اگر(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(ڈیٹا, "Failed due to missing peer certificate");
      واپس CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, حجم(*p));

    اگر(!p)
      واپس CURLE_برآمد_کا_حافظہ;

    pubkey = curlx_malloc(PUB_DER_MAX_بائٹیں);

    اگر(!pubkey) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    اگر(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(ڈیٹا, "Failed copying peer certificate");
      نتیجہ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      جاؤ pinnedpubkey_error;
    }

    سائز = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_بائٹیں);

    اگر(سائز <= 0) {
      failf(ڈیٹا, "Failed copying public key from peer certificate");
      نتیجہ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      جاؤ pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    نتیجہ = Curl_pin_peer_pubkey(ڈیٹا,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_بائٹیں - سائز], سائز);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
#اگر_ختم

#اگر_تعریف_شدہ HAS_ALPN_MBEDTLS
  اگر(connssl->alpn) {
    ثابت حرف *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    نتیجہ = Curl_alpn_سیٹ_negotiated(cf, ڈیٹا, connssl,
                                      (ثابت غیرعلامتی حرف *)proto,
                                      proto ? لڑی_لمبائی(proto) : 0);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
#اگر_ختم

  connssl->connecting_state = ssl_connect_3;
  infof(ڈیٹا, "SSL connected");

  واپس CURLE_OK;
}

ساکن CURLcode mbed_new_session(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  ساخت Curl_ssl_session *sc_session = تہی;
  غیرعلامتی حرف *sdata = تہی;
  size_t slen = 0;
  صحیح ietf_tls_id;
  CURLcode نتیجہ = CURLE_OK;
  صحیح ret;

  DEBUGASSERT(backend);
  اگر(!Curl_ssl_scache_use(cf, ڈیٹا))
    واپس CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  اگر(ret) {
    failf(ڈیٹا, "mbedtls_ssl_get_session returned -0x%x", (غیرعلامتی صحیح)-ret);
    نتیجہ = CURLE_SSL_CONNECT_ERROR;
    جاؤ برآمد;
  }

  mbedtls_ssl_session_save(&session, تہی, 0, &slen);
  اگر(!slen) {
    failf(ڈیٹا, "failed to serialize session: length is 0");
    جاؤ برآمد;
  }

  sdata = curlx_malloc(slen);
  اگر(!sdata) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  اگر(ret) {
    failf(ڈیٹا, "failed to serialize session: -0x%x", (غیرعلامتی صحیح)-ret);
    جاؤ برآمد;
  }

  ietf_tls_id = mbedtls_ssl_get_نسخہ_نمبر(&backend->ssl);
  نتیجہ = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = تہی;  /* call took ownership */
  اگر(!نتیجہ && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    نتیجہ = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  اگر(!نتیجہ) {
    نتیجہ = Curl_ssl_scache_put(cf, ڈیٹا, connssl->peer.scache_کلید,
                                 sc_session);
    sc_session = تہی;
  }

برآمد:
  اگر(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  واپس نتیجہ;
}

ساکن CURLcode mbed_send(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                          ثابت خالی *mem, size_t len, size_t *pnwritten)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  صحیح nwritten;

  DEBUGASSERT(backend);
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#اگر_ختم
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  اگر(backend->send_blocked) {
    اگر(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(ڈیٹا, "re-send data shorter than previously");
      نتیجہ = CURLE_BAD_FUNCTION_دلیل;
      جاؤ برآمد;
    }
    CURL_TRC_CF(ڈیٹا, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ثابت غیرعلامتی حرف *)mem, len);

  اگر(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  ورنہ {
    CURL_TRC_CF(ڈیٹا, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (غیرعلامتی صحیح)-nwritten);
    انتخاب(nwritten) {
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3
    حالت MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      نتیجہ = mbed_new_session(cf, ڈیٹا);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      اگر(!نتیجہ)
        جاؤ do_send;
      توڑو;
#اگر_ختم
    حالت MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      نتیجہ = CURLE_AGAIN;
      توڑو;
    حالت MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتیجہ = CURLE_AGAIN;
      توڑو;
    طےشدہ:
      نتیجہ = CURLE_SEND_ERROR;
      توڑو;
    }
    اگر((نتیجہ == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (صحیح)نتیجہ, *pnwritten);
  واپس نتیجہ;
}

ساکن CURLcode mbedtls_shutdown(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 bool send_shutdown, bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  غیرعلامتی حرف buf[1024];
  CURLcode نتیجہ = CURLE_OK;
  صحیح ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  اگر(!backend->initialized || cf->shutdown) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *مکمل = FALSE;

  اگر(!backend->بھیجا_shutdown) {
    /* do this only once */
    backend->بھیجا_shutdown = TRUE;
    اگر(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      انتخاب(ret) {
      حالت 0: /* we sent it, receive from the server */
        توڑو;
      حالت MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *مکمل = TRUE;
        جاؤ برآمد;
      حالت MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        جاؤ برآمد;
      حالت MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        جاؤ برآمد;
      طےشدہ:
        CURL_TRC_CF(ڈیٹا, cf, "mbedtls_shutdown error -0x%04X",
                    (غیرعلامتی صحیح)-ret);
        نتیجہ = CURLE_RECV_ERROR;
        جاؤ برآمد;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  برائے(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, حجم(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    اگر(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, حجم(buf));
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3
    اگر(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      جاری;
#اگر_ختم
    اگر(ret <= 0)
      توڑو;
  }

  اگر(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(ڈیٹا, cf, "mbedtls_shutdown, still getting data");
  }
  ورنہ اگر(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(ڈیٹا, cf, "mbedtls_shutdown done");
    *مکمل = TRUE;
  }
  ورنہ اگر(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(ڈیٹا, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  ورنہ اگر(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(ڈیٹا, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  ورنہ {
    CURL_TRC_CF(ڈیٹا, cf, "mbedtls_shutdown error -0x%04X",
                (غیرعلامتی صحیح)-ret);
    نتیجہ = CURLE_RECV_ERROR;
  }

برآمد:
  cf->shutdown = (نتیجہ || *مکمل);
  واپس نتیجہ;
}

ساکن خالی mbedtls_close(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;

  (خالی)ڈیٹا;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#اگر_تعریف_شدہ MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#اگر_ختم
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  اگر(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

ساکن CURLcode mbed_recv(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                          حرف *buf, size_t buffersize, size_t *pnread)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  CURLcode نتیجہ = CURLE_OK;
  صحیح nread;

  DEBUGASSERT(backend);
#اگر_تعریف_شدہ MBEDTLS_SSL_SESSION_TICKETS
do_read:
#اگر_ختم
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (غیرعلامتی حرف *)buf, buffersize);
  اگر(nread > 0)
    *pnread = (size_t)nread;
  ورنہ {
    حرف errorbuf[128];
    CURL_TRC_CF(ڈیٹا, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (غیرعلامتی صحیح)-nread);
    انتخاب(nread) {
#اگر_تعریف_شدہ MBEDTLS_SSL_SESSION_TICKETS
    حالت MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      نتیجہ = mbed_new_session(cf, ڈیٹا);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      اگر(!نتیجہ)
        جاؤ do_read;
      توڑو;
#اگر_ختم
    حالت MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      نتیجہ = CURLE_AGAIN;
      توڑو;
    حالت MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتیجہ = CURLE_AGAIN;
      توڑو;
    حالت MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      نتیجہ = CURLE_OK;
      توڑو;
    طےشدہ:
      mbedtls_strerror(nread, errorbuf, حجم(errorbuf));
      failf(ڈیٹا, "ssl_read returned: (-0x%04X) %s", (غیرعلامتی صحیح)-nread,
            errorbuf);
      نتیجہ = CURLE_RECV_ERROR;
      توڑو;
    }
  }
  واپس نتیجہ;
}

ساکن size_t mbedtls_نسخہ(حرف *بفر, size_t سائز)
{
#اگر_تعریف_شدہ MBEDTLS_نسخہ_C
  /* if mbedtls_version_get_number() is available it is better */
  غیرعلامتی صحیح نسخہ_2 = mbedtls_نسخہ_get_نمبر();
  واپس curl_msnprintf(بفر, سائز, "mbedTLS/%u.%u.%u", نسخہ_2 >> 24,
                        (نسخہ_2 >> 16) & 0xff, (نسخہ_2 >> 8) & 0xff);
#ورنہ
  واپس curl_msnprintf(بفر, سائز, "mbedTLS/%s", MBEDTLS_نسخہ_STRING);
#اگر_ختم
}

/* 'data' might be NULL */
ساکن CURLcode mbedtls_random(ساخت Curl_easy *ڈیٹا,
                               غیرعلامتی حرف *entropy, size_t لمبائی)
{
  psa_حالت_t حالت_2;
  (خالی)ڈیٹا;

  حالت_2 = psa_generate_random(entropy, لمبائی);

  واپس حالت_2 == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ساکن CURLcode mbedtls_connect(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                bool *مکمل)
{
  CURLcode نتیجہ;
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;

  /* check if the connection has already been established */
  اگر(ssl_connection_complete == connssl->state) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  *مکمل = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  اگر(connssl->connecting_state == ssl_connect_1) {
    نتیجہ = mbed_connect_step1(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(connssl->connecting_state == ssl_connect_2) {
    نتیجہ = mbed_connect_step2(cf, ڈیٹا);
    اگر(نتیجہ)
      واپس نتیجہ;
  }

  اگر(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    ساخت ssl_connect_ڈیٹا *ctx = cf->ctx;
    ساخت mbed_ssl_backend_ڈیٹا *backend =
      (ساخت mbed_ssl_backend_ڈیٹا *)ctx->backend;

    اگر(mbedtls_ssl_get_نسخہ_نمبر(&backend->ssl) <=
       MBEDTLS_SSL_نسخہ_TLS1_2) {
      نتیجہ = mbed_new_session(cf, ڈیٹا);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    connssl->connecting_state = ssl_connect_مکمل;
  }

  اگر(connssl->connecting_state == ssl_connect_مکمل) {
    connssl->state = ssl_connection_complete;
    *مکمل = TRUE;
  }

  واپس CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ساکن صحیح mbedtls_init(خالی)
{
#اگر MBEDTLS_نسخہ_نمبر < 0x04000000
  صحیح ret = 0;
#اگر_ختم
  psa_حالت_t حالت_2;
  حالت_2 = psa_crypto_init();

  اگر(حالت_2 != PSA_SUCCESS)
    واپس 0;

#اگر MBEDTLS_نسخہ_نمبر < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_بیج(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              تہی, 0);

  اگر(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    واپس 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_سیٹ_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#اگر_ختم
  واپس 1;
}

ساکن خالی mbedtls_cleanup(خالی)
{
  mbedtls_psa_crypto_free();

#اگر MBEDTLS_نسخہ_نمبر < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#اگر_ختم
}

ساکن bool mbedtls_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                 ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *ctx = cf->ctx;
  ساخت mbed_ssl_backend_ڈیٹا *backend;

  (خالی)ڈیٹا;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (ساخت mbed_ssl_backend_ڈیٹا *)ctx->backend;
  واپس mbedtls_ssl_get_بائٹیں_avail(&backend->ssl) != 0;
}

ساکن CURLcode mbedtls_sha256sum(ثابت غیرعلامتی حرف *داخلہ,
                                  size_t inputlen,
                                  غیرعلامتی حرف *sha256sum,
                                  size_t sha256len)
{
#اگر defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_حالت_t حالت_2;
  size_t sha256len_actual;
  حالت_2 = psa_hash_compute(PSA_ALG_SHA_256, داخلہ, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  اگر(حالت_2 != PSA_SUCCESS)
    واپس CURLE_BAD_FUNCTION_دلیل;
  واپس CURLE_OK;
#ورنہ
  (خالی)sha256len;
  واپس Curl_sha256it(sha256sum, داخلہ, inputlen);
#اگر_ختم
}

ساکن خالی *mbedtls_get_internals(ساخت ssl_connect_ڈیٹا *connssl,
                                   CURLINFO info)
{
  ساخت mbed_ssl_backend_ڈیٹا *backend =
    (ساخت mbed_ssl_backend_ڈیٹا *)connssl->backend;
  (خالی)info;
  DEBUGASSERT(backend);
  واپس &backend->ssl;
}

ثابت ساخت Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_راستہ |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#اگر_تعریف_شدہ HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#اگر_ختم
  SSLSUPP_SSL_CTX |
#اگر_تعریف_شدہ MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#اگر_ختم
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#اگر_تعریف_شدہ MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#اگر_ختم
  0,

  حجم(ساخت mbed_ssl_backend_ڈیٹا),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_نسخہ,                  /*نسخہ*/
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_ڈیٹا_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  تہی,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  تہی,                             /* close_all */
  تہی,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  تہی,                             /* get_channel_binding */
};

#اگر_ختم /* USE_MBEDTLS */
