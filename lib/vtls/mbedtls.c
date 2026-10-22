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

#اگر_تعریف_شده USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#ضمن <mbedtls/version.h>
#إذا MBEDTLS_نسخه_عدد < 0x03020000
#خطا "mbedTLS 3.2.0 or greater required"
#پایان_اگر
#ضمن <psa/crypto_config.h>
#ضمن <mbedtls/net_sockets.h>
#ضمن <mbedtls/ssl.h>
#ضمن <mbedtls/x509.h>
#ضمن <mbedtls/psa_util.h>

#إذا MBEDTLS_نسخه_عدد < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#خطا "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#پایان_اگر

#ضمن <mbedtls/error.h>
#إذا MBEDTLS_نسخه_عدد < 0x04000000
#ضمن <mbedtls/entropy.h>
#ضمن <mbedtls/ctr_drbg.h>
#پایان_اگر
#اگر_تعریف_شده MBEDTLS_DEBUG
#ضمن <mbedtls/debug.h>
#پایان_اگر

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
#تعریف_کن HAS_ALPN_MBEDTLS
#پایان_اگر

بنية mbed_ssl_backend_داده {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#اگر_تعریف_شده MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#پایان_اگر
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#اگر_تعریف_شده HAS_ALPN_MBEDTLS
  ثابت حرف *protocols[3];
#پایان_اگر
  صحيح *ciphersuites;
  size_t send_blocked_len;
  بیت(initialized); /* mbedtls_ssl_context is initialized */
  بیت(فرستاده_shutdown);
  بیت(send_blocked);
};

/** A context for random number generation (RNG).
 */
#إذا MBEDTLS_نسخه_عدد < 0x04000000
بنية rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ساكن بنية rng_context_t rng;
#پایان_اگر

#اگر_تعریف_نشده MBEDTLS_ERROR_C
#تعریف_کن mbedtls_strerror(a, b, c) b[0] = 0
#پایان_اگر

#اگر_تعریف_شده MBEDTLS_DEBUG
ساكن فراغ mbed_debug(فراغ *context, صحيح level, ثابت حرف *f_name,
                       صحيح سطر_nb, ثابت حرف *سطر)
{
  بنية Curl_easy *داده = (بنية Curl_easy *)context;
  (فراغ)level;
  (فراغ)سطر_nb;
  (فراغ)f_name;

  إذا(داده) {
    size_t len = طول_رشته(سطر);
    إذا(len && (سطر[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(داده, "%.*s", (صحيح)len, سطر);
  }
}
#پایان_اگر

ساكن صحيح mbedtls_bio_cf_write(فراغ *bio,
                                ثابت بیعلامت حرف *buf, size_t blen)
{
  بنية Curl_cfilter *cf = bio;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  size_t nwritten;
  CURLcode نتیجه;

  DEBUGASSERT(داده);
  إذا(!داده)
    أرجع 0;

  نتیجه = Curl_conn_cf_send(cf->بعدی, داده, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(داده, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (صحيح)نتیجه, nwritten);
  إذا(نتیجه == CURLE_AGAIN)
    أرجع MBEDTLS_ERR_SSL_WANT_WRITE;
  أرجع نتیجه ? -1 : (صحيح)nwritten;
}

ساكن صحيح mbedtls_bio_cf_read(فراغ *bio, بیعلامت حرف *buf, size_t blen)
{
  بنية Curl_cfilter *cf = bio;
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);
  size_t nread = 0;
  CURLcode نتیجه;

  DEBUGASSERT(داده);
  إذا(!داده)
    أرجع 0;
  /* OpenSSL catches this case, so should we. */
  إذا(!buf)
    أرجع 0;

  نتیجه = Curl_conn_cf_recv(cf->بعدی, داده, (حرف *)buf, blen, &nread);
  CURL_TRC_CF(داده, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (صحيح)نتیجه, nread);
  إذا(نتیجه == CURLE_AGAIN)
    أرجع MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  أرجع نتیجه ? -1 : (صحيح)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#تعریف_کن RSA_PUB_DER_MAX_بایتها   (38 + (2 * MBEDTLS_MPI_MAX_اندازه))
#تعریف_کن ECP_PUB_DER_MAX_بایتها   (30 + (2 * MBEDTLS_ECP_MAX_بایتها))

#تعریف_کن PUB_DER_MAX_بایتها   (RSA_PUB_DER_MAX_بایتها > ECP_PUB_DER_MAX_بایتها ? \
                             RSA_PUB_DER_MAX_بایتها : ECP_PUB_DER_MAX_بایتها)

ساكن CURLcode mbed_مجموعه_ssl_نسخه_min_max(
  بنية Curl_easy *داده,
  بنية mbed_ssl_backend_داده *backend,
  بنية ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_نسخه ver_min =
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_نسخه_TLS1_2
#وإلا
    MBEDTLS_SSL_نسخه_TLS1_3
#پایان_اگر
    ;
  mbedtls_ssl_protocol_نسخه ver_max =
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_نسخه_TLS1_3
#وإلا
    MBEDTLS_SSL_نسخه_TLS1_2
#پایان_اگر
    ;

  DEBUGASSERT(conn_config->نسخه_2 != CURL_SSLVERSION_DEFAULT);
  بدل(conn_config->نسخه_2) {
  حالة CURL_SSLVERSION_TLSv1:
  حالة CURL_SSLVERSION_TLSv1_0:
  حالة CURL_SSLVERSION_TLSv1_1:
  حالة CURL_SSLVERSION_TLSv1_2:
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_نسخه_TLS1_2;
    اكسر;
#پایان_اگر
  حالة CURL_SSLVERSION_TLSv1_3:
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_نسخه_TLS1_3;
    اكسر;
#پایان_اگر
  افتراضي:
    failf(داده, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->نسخه_2);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  بدل(conn_config->نسخه_max) {
  حالة CURL_SSLVERSION_MAX_DEFAULT:
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_TLSv1_3:
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_نسخه_TLS1_3;
    اكسر;
#پایان_اگر
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_2
  حالة CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_نسخه_TLS1_2;
    اكسر;
#پایان_اگر
  حالة CURL_SSLVERSION_MAX_TLSv1_1:
  حالة CURL_SSLVERSION_MAX_TLSv1_0:
  افتراضي:
    failf(داده, "mbedTLS: unsupported maximum TLS version value");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_نسخه(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_نسخه(&backend->config, ver_max);

  أرجع CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#اگر_تعریف_شده MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ساكن صحيح mbed_cipher_suite_get_str(uint16_t id, حرف *buf, size_t buf_اندازه,
                                     bool prefer_rfc)
{
  إذا(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_اندازه, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  وإلا
    أرجع Curl_cipher_suite_get_str(id, buf, buf_اندازه, prefer_rfc);
  أرجع 0;
}

ساكن uint16_t mbed_cipher_suite_walk_str(ثابت حرف **str, ثابت حرف **انتها)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, انتها);
  size_t len = *انتها - *str;
  ساكن ثابت حرف ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  إذا(!id) {
    إذا((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  أرجع id;
}
#وإلا
#تعریف_کن mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#تعریف_کن mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#پایان_اگر

ساكن CURLcode mbed_مجموعه_selected_ciphers(
  بنية Curl_easy *داده,
  بنية mbed_ssl_backend_داده *backend,
  ثابت حرف *ciphers12,
  ثابت حرف *ciphers13)
{
  ثابت حرف *ciphers = ciphers12;
  ثابت صحيح *supported;
  صحيح *selected;
  size_t supported_len, شمار = 0, default13_شمار = 0, i, j;
  ثابت حرف *ptr, *انتها;

  supported = mbedtls_ssl_list_ciphersuites();
  لكل(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(حجم(صحيح) * (supported_len + 1));
  إذا(!selected)
    أرجع CURLE_بیرون_از_حافظه;

#اگر_تعریف_نشده MBEDTLS_SSL_PROTO_TLS1_3
  (فراغ)ciphers13, (فراغ)j;
#وإلا
  إذا(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    لكل(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      إذا(مقایسه_رشته(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        استمر;

      selected[شمار++] = id;
    }

    default13_شمار = شمار;
  }
  وإلا
    ciphers = ciphers13;

جمع_کن_ciphers:
#پایان_اگر
  لكل(ptr = ciphers; ptr[0] != '\0' && شمار < supported_len; ptr = انتها) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &انتها);

    /* Check if cipher is supported */
    إذا(id) {
      لكل(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      إذا(i == supported_len)
        id = 0;
    }
    إذا(!id) {
      إذا(ptr[0] != '\0')
        infof(داده, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (صحيح)(انتها - ptr), ptr);
      استمر;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    لكل(i = 0; i < شمار && selected[i] != id; i++)
      ;
    إذا(i < شمار) {
      إذا(i >= default13_شمار)
        infof(داده, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (صحيح)(انتها - ptr), ptr);
      استمر;
    }

    selected[شمار++] = id;
  }

#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_3
  إذا(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    اذهب جمع_کن_ciphers;
  }

  إذا(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    لكل(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      إذا(!مقایسه_رشته(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        استمر;

      /* No duplicates allowed (so selected cannot overflow) */
      لكل(i = 0; i < شمار && selected[i] != id; i++)
        ;
      إذا(i < شمار)
        استمر;

      selected[شمار++] = id;
    }
  }
#پایان_اگر

  selected[شمار] = 0;

  إذا(شمار == 0) {
    curlx_free(selected);
    failf(داده, "mbedTLS: no supported cipher in list");
    أرجع CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  أرجع CURLE_OK;
}

ساكن فراغ mbed_dump_cert_info(بنية Curl_easy *داده,
                                ثابت mbedtls_x509_crt *crt)
{
#إذا !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (فراغ)داده, (فراغ)crt;
#وإلا
  ثابت size_t bufsize = 16384;
  حرف *p, *میانگیر = curlx_malloc(bufsize);

  إذا(میانگیر && mbedtls_x509_crt_info(میانگیر, bufsize, " ", crt) > 0) {
    infof(داده, "Server certificate:");
    لكل(p = میانگیر; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(داده, "%.*s", (صحيح)s, p);
      p += s;
    }
  }
  وإلا
    infof(داده, "Unable to dump certificate information");

  curlx_free(میانگیر);
#پایان_اگر
}

ساكن فراغ mbed_استخراج_certinfo(بنية Curl_easy *داده,
                                  ثابت mbedtls_x509_crt *crt)
{
  CURLcode نتیجه;
  ثابت mbedtls_x509_crt *cur;
  صحيح cert_شمار = 0;
  صحيح i;

  لكل(cur = crt; cur && cert_شمار <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->بعدی)
    cert_شمار++;

  إذا(cert_شمار > MAX_ALLOWED_CERT_AMOUNT) {
    infof(داده, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    أرجع;
  }

  نتیجه = Curl_ssl_init_certinfo(داده, cert_شمار);

  لكل(i = 0, cur = crt; نتیجه == CURLE_OK && cur; ++i, cur = cur->بعدی) {
    نتیجه = Curl_استخراج_certinfo(داده, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ساكن صحيح mbed_verify_cb(فراغ *ptr, mbedtls_x509_crt *crt,
                          صحيح عمق, uint32_t *flags)
{
  بنية Curl_cfilter *cf = (بنية Curl_cfilter *)ptr;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_easy *داده = CF_داده_CURRENT(cf);

  إذا(عمق == 0) {
    إذا(داده->مجموعه.verbose)
      mbed_dump_cert_info(داده, crt);
    إذا(داده->مجموعه.ssl.certinfo)
      mbed_استخراج_certinfo(داده, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  إذا(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  إذا(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  إذا(*flags) {
#اگر_تعریف_نشده MBEDTLS_X509_REMOVE_INFO
    حرف buf[128];
    mbedtls_x509_crt_verify_info(buf, حجم(buf), "", *flags);
    failf(داده, "mbedTLS: %s", buf);
#وإلا
    failf(داده, "mbedTLS: certificate verification error 0x%08x",
          (بیعلامت صحيح)*flags);
#پایان_اگر
  }

  أرجع 0;
}

ساكن CURLcode mbed_بارگیری_cacert(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت بنية curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ثابت حرف * ثابت ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? عدم : conn_config->CAfile);
  ثابت bool verifypeer = conn_config->verifypeer;
  ثابت حرف * ثابت ssl_capath = conn_config->CApath;
#اگر_تعریف_شده MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#پایان_اگر
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  إذا(ca_info_blob && verifypeer) {
#اگر_تعریف_شده MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    إذا((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((حرف *)(ca_info_blob->داده))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->داده,
                                   ca_info_blob->len);
    }
    وإلا { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      بیعلامت حرف *newblob = curlx_memdup0(ca_info_blob->داده,
                                             ca_info_blob->len);
      إذا(!newblob)
        أرجع CURLE_بیرون_از_حافظه;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#وإلا
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->داده,
                                     ca_info_blob->len);
#پایان_اگر

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(داده, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (بیعلامت صحيح)-ret, errorbuf);
      أرجع CURLE_SSL_CERTPROBLEM;
    }
  }

  إذا(ssl_cafile && verifypeer) {
#اگر_تعریف_شده MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(داده, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (بیعلامت صحيح)-ret, errorbuf);
      أرجع CURLE_SSL_CACERT_BADFILE;
    }
#وإلا
    failf(داده, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
  }

  إذا(ssl_capath) {
#اگر_تعریف_شده MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_مسیر(&backend->cacert, ssl_capath);

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(داده, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (بیعلامت صحيح)-ret, errorbuf);

      إذا(verifypeer)
        أرجع CURLE_SSL_CACERT_BADFILE;
    }
#وإلا
    failf(داده, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
  }

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_بارگیری_clicert(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  حرف * ثابت ssl_cert = conn_config->clientcert;
  ثابت بنية curl_blob *ssl_cert_blob = conn_config->cert_blob;
#اگر_تعریف_شده MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#پایان_اگر
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  إذا(ssl_cert) {
#اگر_تعریف_شده MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(داده, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (بیعلامت صحيح)-ret, errorbuf);

      أرجع CURLE_SSL_CERTPROBLEM;
    }
#وإلا
    failf(داده, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
  }

  إذا(ssl_cert_blob) {
#اگر_تعریف_شده MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    إذا((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((حرف *)(ssl_cert_blob->داده))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->داده,
                                   ssl_cert_blob->len);
    }
    وإلا { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      بیعلامت حرف *newblob = curlx_memdup0(ssl_cert_blob->داده,
                                             ssl_cert_blob->len);
      إذا(!newblob)
        أرجع CURLE_بیرون_از_حافظه;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#وإلا
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->داده,
                                     ssl_cert_blob->len);
#پایان_اگر

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(داده, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (بیعلامت صحيح)-ret, errorbuf);
      أرجع CURLE_SSL_CERTPROBLEM;
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_بارگیری_privkey(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  إذا(conn_config->کلید || conn_config->کلید_blob) {
    إذا(conn_config->کلید) {
#اگر_تعریف_شده MBEDTLS_FS_IO
#إذا MBEDTLS_نسخه_عدد >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->کلید,
                                     conn_config->کلید_passwd);
      إذا(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وإلا
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->کلید,
                                     conn_config->کلید_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      إذا(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#پایان_اگر

      إذا(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(داده, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->کلید, (بیعلامت صحيح)-ret, errorbuf);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
#وإلا
      failf(داده, "mbedTLS: functions that use the file system not built in");
      أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
    }
    وإلا {
      ثابت بنية curl_blob *ssl_کلید_blob = conn_config->کلید_blob;
      ثابت حرف *passwd = conn_config->کلید_passwd;
      ثابت bool is_der = conn_config->کلید_type &&
                          curl_strequal(conn_config->کلید_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      بیعلامت حرف *newblob = curlx_memdup0(ssl_کلید_blob->داده,
                                             ssl_کلید_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_کلید_blob->len : ssl_کلید_blob->len + 1;
      إذا(!newblob)
        أرجع CURLE_بیرون_از_حافظه;

#إذا MBEDTLS_نسخه_عدد >= 0x04000000
      ret = mbedtls_pk_parse_کلید(&backend->pk, newblob, keylen,
                                 (ثابت بیعلامت حرف *)passwd,
                                 passwd ? طول_رشته(passwd) : 0);
      إذا(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_کلید_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وإلا
      ret = mbedtls_pk_parse_کلید(&backend->pk, newblob, keylen,
                                 (ثابت بیعلامت حرف *)passwd,
                                 passwd ? طول_رشته(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      إذا(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#پایان_اگر
      curlx_free(newblob);

      إذا(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(داده, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (بیعلامت صحيح)-ret, errorbuf);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_بارگیری_crl(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت حرف * ثابت ssl_crlfile = conn_config->CRLfile;

#اگر_تعریف_شده MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  إذا(ssl_crlfile) {
    حرف errorbuf[128];
#اگر_تعریف_شده MBEDTLS_FS_IO
    صحيح ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(داده, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (بیعلامت صحيح)-ret, errorbuf);

      أرجع CURLE_SSL_CRL_BADFILE;
    }
#وإلا
    (فراغ)errorbuf;
    failf(داده, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_درون;
#پایان_اگر
  }
#وإلا
  (فراغ)backend;
  إذا(ssl_crlfile) {
    failf(داده, "mbedTLS: CRL support not built in");
    أرجع CURLE_NOT_BUILT_درون;
  }
#پایان_اگر

  أرجع CURLE_OK;
}

ساكن bool mbed_apply_session(بنية Curl_cfilter *cf,
                               بنية Curl_easy *داده,
                               بنية Curl_ssl_session *sc_session)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;

  إذا(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    صحيح ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_بارگیری(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    إذا(ret) {
      failf(داده, "SSL session error loading: -0x%x", (بیعلامت صحيح)-ret);
    }
    وإلا {
      ret = mbedtls_ssl_مجموعه_session(&backend->ssl, &session);
      إذا(ret)
        failf(داده, "SSL session error setting: -0x%x", (بیعلامت صحيح)-ret);
    }
    mbedtls_ssl_session_free(&session);
    إذا(!ret) {
      infof(داده, "SSL reusing session ID");
      أرجع TRUE;
    }
  }
  أرجع FALSE;
}

ساكن CURLcode mbed_configure_ssl(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح ret;
  bool session_applied = FALSE;
  CURLcode نتیجه;
  حرف errorbuf[128];

  infof(داده, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->پورت);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_جریان,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  إذا(ret) {
    failf(داده, "mbedTLS: ssl_config failed");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

#اگر_تعریف_شده MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, داده);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_مجموعه_آستانه(4);
#پایان_اگر

#إذا defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_نسخه_عدد >= 0x03060100 &&   \
  MBEDTLS_نسخه_عدد < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#پایان_اگر

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_بعدی);

  نتیجه = mbed_مجموعه_ssl_نسخه_min_max(داده, backend, conn_config);
  إذا(نتیجه)
    أرجع نتیجه;

#إذا MBEDTLS_نسخه_عدد < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#پایان_اگر

  ret = mbedtls_ssl_راهاندازی(&backend->ssl,
                          &backend->config);
  إذا(ret) {
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(داده, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (بیعلامت صحيح)-ret, errorbuf);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_مجموعه_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      عدم /*  rev_timeout() */);

#اگر_تعریف_نشده MBEDTLS_SSL_PROTO_TLS1_3
  إذا(conn_config->cipher_list) {
    نتیجه = mbed_مجموعه_selected_ciphers(داده, backend,
                                       conn_config->cipher_list, عدم);
#وإلا
  إذا(conn_config->cipher_list || conn_config->cipher_list13) {
    نتیجه = mbed_مجموعه_selected_ciphers(داده, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#پایان_اگر
    إذا(نتیجه) {
      failf(داده, "mbedTLS: failed to set cipher suites");
      أرجع نتیجه;
    }
  }
  وإلا {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#اگر_تعریف_شده MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#پایان_اگر

#اگر_تعریف_شده MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#پایان_اگر

  إذا((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    بنية Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(داده, cf->cft, FIRSTSOCKET);
    إذا(scs) {
      إذا(mbed_apply_session(cf, داده, scs)) {
        CURL_TRC_CF(داده, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  إذا(!session_applied && Curl_ssl_scache_use(cf, داده)) {
    بنية Curl_ssl_session *sc_session = عدم;
    CURLcode sresult = Curl_ssl_scache_take(cf, داده, connssl->peer.scache_کلید,
                                            &sc_session);
    إذا(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_بارگیری(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      إذا(ret) {
        failf(داده, "SSL session error loading: -0x%x", (بیعلامت صحيح)-ret);
      }
      وإلا {
        ret = mbedtls_ssl_مجموعه_session(&backend->ssl, &session);
        إذا(ret)
          failf(داده, "SSL session error setting: -0x%x", (بیعلامت صحيح)-ret);
        وإلا
          infof(داده, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, داده, connssl->peer.scache_کلید,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#اگر_تعریف_شده MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#وإلا
                            عدم
#پایان_اگر
    );

  إذا(conn_config->کلید || conn_config->کلید_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  إذا(mbedtls_ssl_مجموعه_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(داده, "Failed to set SNI");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

#اگر_تعریف_شده HAS_ALPN_MBEDTLS
  إذا(connssl->alpn) {
    بنية alpn_proto_buf proto;
    size_t i;

    لكل(i = 0; i < connssl->alpn->شمار; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    إذا(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(داده, "Failed setting ALPN protocols");
      أرجع CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_مقصد_proto_str(&proto, connssl->alpn);
    infof(داده, VTLS_INFOF_ALPN_OFFER_1STR, proto.داده);
  }
#پایان_اگر

  /* give application a chance to interfere with mbedTLS set up. */
  إذا(داده->مجموعه.ssl_fsslctx) {
    نتیجه = (*داده->مجموعه.ssl_fsslctx)(داده, &backend->config,
                                      داده->مجموعه.ssl_fsslctxp);
    إذا(نتیجه)
      failf(داده, "error signaled by SSL ctx callback");
  }

  أرجع نتیجه;
}

ساكن CURLcode mbed_connect_step1(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجه;

  إذا((conn_config->نسخه_2 == CURL_SSLVERSION_SSLv2) ||
     (conn_config->نسخه_2 == CURL_SSLVERSION_SSLv3)) {
    failf(داده, "Not supported SSL version");
    أرجع CURLE_NOT_BUILT_درون;
  }

  نتیجه = mbed_بارگیری_cacert(cf, داده);
  إذا(!نتیجه)
    نتیجه = mbed_بارگیری_clicert(cf, داده);
  إذا(!نتیجه)
    نتیجه = mbed_بارگیری_privkey(cf, داده);
  إذا(!نتیجه)
    نتیجه = mbed_بارگیری_crl(cf, داده);
  إذا(!نتیجه)
    نتیجه = mbed_configure_ssl(cf, داده);
  إذا(نتیجه)
    أرجع نتیجه;

  connssl->connecting_state = ssl_connect_2;
  أرجع CURLE_OK;
}

#إذا defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#تعریف_کن HAVE_PINNED_PUBKEY
#پایان_اگر

ساكن CURLcode mbed_connect_step2(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *داده)
{
#إذا defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode نتیجه;
#پایان_اگر
  صحيح ret;
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
#اگر_تعریف_شده HAVE_PINNED_PUBKEY
#اگر_تعریف_نشده CURL_DISABLE_PROXY
  ثابت حرف * ثابت pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY);
#وإلا
  ثابت حرف * ثابت pinnedpubkey =
    CURL_EASY_STR(داده, STRING_SSL_PINNEDPUBLICKEY);
#پایان_اگر
#پایان_اگر

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
    failf(داده, "peer certificate could not be verified");
    أرجع CURLE_PEER_FAILED_VERIFICATION;
  }
  وإلا إذا(ret) {
    حرف errorbuf[128];
    CURL_TRC_CF(داده, cf, "TLS version %04X",
                mbedtls_ssl_get_نسخه_عدد(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(داده, "ssl_handshake returned: (-0x%04X) %s",
          (بیعلامت صحيح)-ret, errorbuf);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  {
    حرف cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, حجم(cipher_str), TRUE);
    infof(داده, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_نسخه(&backend->ssl), cipher_str);
  }

#اگر_تعریف_شده HAVE_PINNED_PUBKEY
  إذا(pinnedpubkey) {
    صحيح اندازه;
    ثابت mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = عدم;
    بیعلامت حرف *pubkey = عدم;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    إذا(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(داده, "Failed due to missing peer certificate");
      أرجع CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, حجم(*p));

    إذا(!p)
      أرجع CURLE_بیرون_از_حافظه;

    pubkey = curlx_malloc(PUB_DER_MAX_بایتها);

    إذا(!pubkey) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    إذا(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(داده, "Failed copying peer certificate");
      نتیجه = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      اذهب pinnedpubkey_error;
    }

    اندازه = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_بایتها);

    إذا(اندازه <= 0) {
      failf(داده, "Failed copying public key from peer certificate");
      نتیجه = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      اذهب pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    نتیجه = Curl_pin_peer_pubkey(داده,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_بایتها - اندازه], اندازه);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    إذا(نتیجه)
      أرجع نتیجه;
  }
#پایان_اگر

#اگر_تعریف_شده HAS_ALPN_MBEDTLS
  إذا(connssl->alpn) {
    ثابت حرف *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    نتیجه = Curl_alpn_مجموعه_negotiated(cf, داده, connssl,
                                      (ثابت بیعلامت حرف *)proto,
                                      proto ? طول_رشته(proto) : 0);
    إذا(نتیجه)
      أرجع نتیجه;
  }
#پایان_اگر

  connssl->connecting_state = ssl_connect_3;
  infof(داده, "SSL connected");

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_new_session(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  بنية Curl_ssl_session *sc_session = عدم;
  بیعلامت حرف *sdata = عدم;
  size_t slen = 0;
  صحيح ietf_tls_id;
  CURLcode نتیجه = CURLE_OK;
  صحيح ret;

  DEBUGASSERT(backend);
  إذا(!Curl_ssl_scache_use(cf, داده))
    أرجع CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  إذا(ret) {
    failf(داده, "mbedtls_ssl_get_session returned -0x%x", (بیعلامت صحيح)-ret);
    نتیجه = CURLE_SSL_CONNECT_ERROR;
    اذهب بیرون;
  }

  mbedtls_ssl_session_save(&session, عدم, 0, &slen);
  إذا(!slen) {
    failf(داده, "failed to serialize session: length is 0");
    اذهب بیرون;
  }

  sdata = curlx_malloc(slen);
  إذا(!sdata) {
    نتیجه = CURLE_بیرون_از_حافظه;
    اذهب بیرون;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  إذا(ret) {
    failf(داده, "failed to serialize session: -0x%x", (بیعلامت صحيح)-ret);
    اذهب بیرون;
  }

  ietf_tls_id = mbedtls_ssl_get_نسخه_عدد(&backend->ssl);
  نتیجه = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = عدم;  /* call took ownership */
  إذا(!نتیجه && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    نتیجه = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  إذا(!نتیجه) {
    نتیجه = Curl_ssl_scache_put(cf, داده, connssl->peer.scache_کلید,
                                 sc_session);
    sc_session = عدم;
  }

بیرون:
  إذا(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  أرجع نتیجه;
}

ساكن CURLcode mbed_send(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                          ثابت فراغ *mem, size_t len, size_t *pnwritten)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;
  صحيح nwritten;

  DEBUGASSERT(backend);
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#پایان_اگر
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  إذا(backend->send_blocked) {
    إذا(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(داده, "re-send data shorter than previously");
      نتیجه = CURLE_BAD_FUNCTION_آرگومان;
      اذهب بیرون;
    }
    CURL_TRC_CF(داده, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ثابت بیعلامت حرف *)mem, len);

  إذا(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  وإلا {
    CURL_TRC_CF(داده, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (بیعلامت صحيح)-nwritten);
    بدل(nwritten) {
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_3
    حالة MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      نتیجه = mbed_new_session(cf, داده);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      إذا(!نتیجه)
        اذهب do_send;
      اكسر;
#پایان_اگر
    حالة MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      نتیجه = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتیجه = CURLE_AGAIN;
      اكسر;
    افتراضي:
      نتیجه = CURLE_SEND_ERROR;
      اكسر;
    }
    إذا((نتیجه == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

بیرون:
  CURL_TRC_CF(داده, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (صحيح)نتیجه, *pnwritten);
  أرجع نتیجه;
}

ساكن CURLcode mbedtls_shutdown(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 bool send_shutdown, bool *انجامشده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  بیعلامت حرف buf[1024];
  CURLcode نتیجه = CURLE_OK;
  صحيح ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  إذا(!backend->initialized || cf->shutdown) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *انجامشده = FALSE;

  إذا(!backend->فرستاده_shutdown) {
    /* do this only once */
    backend->فرستاده_shutdown = TRUE;
    إذا(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      بدل(ret) {
      حالة 0: /* we sent it, receive from the server */
        اكسر;
      حالة MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *انجامشده = TRUE;
        اذهب بیرون;
      حالة MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        اذهب بیرون;
      حالة MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        اذهب بیرون;
      افتراضي:
        CURL_TRC_CF(داده, cf, "mbedtls_shutdown error -0x%04X",
                    (بیعلامت صحيح)-ret);
        نتیجه = CURLE_RECV_ERROR;
        اذهب بیرون;
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
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_3
    إذا(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      استمر;
#پایان_اگر
    إذا(ret <= 0)
      اكسر;
  }

  إذا(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(داده, cf, "mbedtls_shutdown, still getting data");
  }
  وإلا إذا(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(داده, cf, "mbedtls_shutdown done");
    *انجامشده = TRUE;
  }
  وإلا إذا(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(داده, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  وإلا إذا(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(داده, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  وإلا {
    CURL_TRC_CF(داده, cf, "mbedtls_shutdown error -0x%04X",
                (بیعلامت صحيح)-ret);
    نتیجه = CURLE_RECV_ERROR;
  }

بیرون:
  cf->shutdown = (نتیجه || *انجامشده);
  أرجع نتیجه;
}

ساكن فراغ mbedtls_close(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;

  (فراغ)داده;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#اگر_تعریف_شده MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#پایان_اگر
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  إذا(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

ساكن CURLcode mbed_recv(بنية Curl_cfilter *cf, بنية Curl_easy *داده,
                          حرف *buf, size_t buffersize, size_t *pnread)
{
  بنية ssl_connect_داده *connssl = cf->ctx;
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  CURLcode نتیجه = CURLE_OK;
  صحيح nread;

  DEBUGASSERT(backend);
#اگر_تعریف_شده MBEDTLS_SSL_SESSION_TICKETS
do_read:
#پایان_اگر
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (بیعلامت حرف *)buf, buffersize);
  إذا(nread > 0)
    *pnread = (size_t)nread;
  وإلا {
    حرف errorbuf[128];
    CURL_TRC_CF(داده, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (بیعلامت صحيح)-nread);
    بدل(nread) {
#اگر_تعریف_شده MBEDTLS_SSL_SESSION_TICKETS
    حالة MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      نتیجه = mbed_new_session(cf, داده);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      إذا(!نتیجه)
        اذهب do_read;
      اكسر;
#پایان_اگر
    حالة MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      نتیجه = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      نتیجه = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      نتیجه = CURLE_OK;
      اكسر;
    افتراضي:
      mbedtls_strerror(nread, errorbuf, حجم(errorbuf));
      failf(داده, "ssl_read returned: (-0x%04X) %s", (بیعلامت صحيح)-nread,
            errorbuf);
      نتیجه = CURLE_RECV_ERROR;
      اكسر;
    }
  }
  أرجع نتیجه;
}

ساكن size_t mbedtls_نسخه(حرف *میانگیر, size_t اندازه)
{
#اگر_تعریف_شده MBEDTLS_نسخه_C
  /* if mbedtls_version_get_number() is available it is better */
  بیعلامت صحيح نسخه_2 = mbedtls_نسخه_get_عدد();
  أرجع curl_msnprintf(میانگیر, اندازه, "mbedTLS/%u.%u.%u", نسخه_2 >> 24,
                        (نسخه_2 >> 16) & 0xff, (نسخه_2 >> 8) & 0xff);
#وإلا
  أرجع curl_msnprintf(میانگیر, اندازه, "mbedTLS/%s", MBEDTLS_نسخه_STRING);
#پایان_اگر
}

/* 'data' might be NULL */
ساكن CURLcode mbedtls_random(بنية Curl_easy *داده,
                               بیعلامت حرف *entropy, size_t درازی)
{
  psa_وضعیت_t وضعیت;
  (فراغ)داده;

  وضعیت = psa_generate_random(entropy, درازی);

  أرجع وضعیت == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ساكن CURLcode mbedtls_connect(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                bool *انجامشده)
{
  CURLcode نتیجه;
  بنية ssl_connect_داده *connssl = cf->ctx;

  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *انجامشده = TRUE;
    أرجع CURLE_OK;
  }

  *انجامشده = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  إذا(connssl->connecting_state == ssl_connect_1) {
    نتیجه = mbed_connect_step1(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
    نتیجه = mbed_connect_step2(cf, داده);
    إذا(نتیجه)
      أرجع نتیجه;
  }

  إذا(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    بنية ssl_connect_داده *ctx = cf->ctx;
    بنية mbed_ssl_backend_داده *backend =
      (بنية mbed_ssl_backend_داده *)ctx->backend;

    إذا(mbedtls_ssl_get_نسخه_عدد(&backend->ssl) <=
       MBEDTLS_SSL_نسخه_TLS1_2) {
      نتیجه = mbed_new_session(cf, داده);
      إذا(نتیجه)
        أرجع نتیجه;
    }
    connssl->connecting_state = ssl_connect_انجامشده;
  }

  إذا(connssl->connecting_state == ssl_connect_انجامشده) {
    connssl->state = ssl_connection_complete;
    *انجامشده = TRUE;
  }

  أرجع CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ساكن صحيح mbedtls_init(فراغ)
{
#إذا MBEDTLS_نسخه_عدد < 0x04000000
  صحيح ret = 0;
#پایان_اگر
  psa_وضعیت_t وضعیت;
  وضعیت = psa_crypto_init();

  إذا(وضعیت != PSA_SUCCESS)
    أرجع 0;

#إذا MBEDTLS_نسخه_عدد < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_دانه(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              عدم, 0);

  إذا(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    أرجع 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_مجموعه_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#پایان_اگر
  أرجع 1;
}

ساكن فراغ mbedtls_cleanup(فراغ)
{
  mbedtls_psa_crypto_free();

#إذا MBEDTLS_نسخه_عدد < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#پایان_اگر
}

ساكن bool mbedtls_داده_pending(بنية Curl_cfilter *cf,
                                 ثابت بنية Curl_easy *داده)
{
  بنية ssl_connect_داده *ctx = cf->ctx;
  بنية mbed_ssl_backend_داده *backend;

  (فراغ)داده;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية mbed_ssl_backend_داده *)ctx->backend;
  أرجع mbedtls_ssl_get_بایتها_avail(&backend->ssl) != 0;
}

ساكن CURLcode mbedtls_sha256sum(ثابت بیعلامت حرف *درون_داد,
                                  size_t inputlen,
                                  بیعلامت حرف *sha256sum,
                                  size_t sha256len)
{
#إذا defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_وضعیت_t وضعیت;
  size_t sha256len_actual;
  وضعیت = psa_hash_compute(PSA_ALG_SHA_256, درون_داد, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  إذا(وضعیت != PSA_SUCCESS)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  أرجع CURLE_OK;
#وإلا
  (فراغ)sha256len;
  أرجع Curl_sha256it(sha256sum, درون_داد, inputlen);
#پایان_اگر
}

ساكن فراغ *mbedtls_get_internals(بنية ssl_connect_داده *connssl,
                                   CURLINFO info)
{
  بنية mbed_ssl_backend_داده *backend =
    (بنية mbed_ssl_backend_داده *)connssl->backend;
  (فراغ)info;
  DEBUGASSERT(backend);
  أرجع &backend->ssl;
}

ثابت بنية Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_مسیر |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#اگر_تعریف_شده HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#پایان_اگر
  SSLSUPP_SSL_CTX |
#اگر_تعریف_شده MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#پایان_اگر
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#اگر_تعریف_شده MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#پایان_اگر
  0,

  حجم(بنية mbed_ssl_backend_داده),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_نسخه,                  /*نسخه*/
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_داده_pending,             /* data_pending */
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

#پایان_اگر /* USE_MBEDTLS */
