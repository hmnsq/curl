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

#که_تعریف_شوی USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#ضمن <mbedtls/version.h>
#إذا MBEDTLS_نسخه_شمېره < 0x03020000
#تېروتنه "mbedTLS 3.2.0 or greater required"
#د_که_پای
#ضمن <psa/crypto_config.h>
#ضمن <mbedtls/net_sockets.h>
#ضمن <mbedtls/ssl.h>
#ضمن <mbedtls/x509.h>
#ضمن <mbedtls/psa_util.h>

#إذا MBEDTLS_نسخه_شمېره < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#تېروتنه "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#د_که_پای

#ضمن <mbedtls/error.h>
#إذا MBEDTLS_نسخه_شمېره < 0x04000000
#ضمن <mbedtls/entropy.h>
#ضمن <mbedtls/ctr_drbg.h>
#د_که_پای
#که_تعریف_شوی MBEDTLS_DEBUG
#ضمن <mbedtls/debug.h>
#د_که_پای

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
#تعریف_کړه HAS_ALPN_MBEDTLS
#د_که_پای

بنية mbed_ssl_backend_معلومات {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#که_تعریف_شوی MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#د_که_پای
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#که_تعریف_شوی HAS_ALPN_MBEDTLS
  ثابت حرف *protocols[3];
#د_که_پای
  صحيح *ciphersuites;
  size_t send_blocked_len;
  بټ(initialized); /* mbedtls_ssl_context is initialized */
  بټ(لېږل_شوی_shutdown);
  بټ(send_blocked);
};

/** A context for random number generation (RNG).
 */
#إذا MBEDTLS_نسخه_شمېره < 0x04000000
بنية rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

ساكن بنية rng_context_t rng;
#د_که_پای

#که_نه_تعریف_شوی MBEDTLS_ERROR_C
#تعریف_کړه mbedtls_strerror(a, b, c) b[0] = 0
#د_که_پای

#که_تعریف_شوی MBEDTLS_DEBUG
ساكن فراغ mbed_debug(فراغ *context, صحيح level, ثابت حرف *f_name,
                       صحيح کرښه_nb, ثابت حرف *کرښه)
{
  بنية Curl_easy *معلومات = (بنية Curl_easy *)context;
  (فراغ)level;
  (فراغ)کرښه_nb;
  (فراغ)f_name;

  إذا(معلومات) {
    size_t len = مزي_اوږدوالی(کرښه);
    إذا(len && (کرښه[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(معلومات, "%.*s", (صحيح)len, کرښه);
  }
}
#د_که_پای

ساكن صحيح mbedtls_bio_cf_write(فراغ *bio,
                                ثابت بینښه حرف *buf, size_t blen)
{
  بنية Curl_cfilter *cf = bio;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  size_t nwritten;
  CURLcode پایله;

  DEBUGASSERT(معلومات);
  إذا(!معلومات)
    أرجع 0;

  پایله = Curl_conn_cf_send(cf->بل, معلومات, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(معلومات, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (صحيح)پایله, nwritten);
  إذا(پایله == CURLE_AGAIN)
    أرجع MBEDTLS_ERR_SSL_WANT_WRITE;
  أرجع پایله ? -1 : (صحيح)nwritten;
}

ساكن صحيح mbedtls_bio_cf_read(فراغ *bio, بینښه حرف *buf, size_t blen)
{
  بنية Curl_cfilter *cf = bio;
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);
  size_t nread = 0;
  CURLcode پایله;

  DEBUGASSERT(معلومات);
  إذا(!معلومات)
    أرجع 0;
  /* OpenSSL catches this case, so should we. */
  إذا(!buf)
    أرجع 0;

  پایله = Curl_conn_cf_recv(cf->بل, معلومات, (حرف *)buf, blen, &nread);
  CURL_TRC_CF(معلومات, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (صحيح)پایله, nread);
  إذا(پایله == CURLE_AGAIN)
    أرجع MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  أرجع پایله ? -1 : (صحيح)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#تعریف_کړه RSA_PUB_DER_MAX_بایټونه   (38 + (2 * MBEDTLS_MPI_MAX_اندازه))
#تعریف_کړه ECP_PUB_DER_MAX_بایټونه   (30 + (2 * MBEDTLS_ECP_MAX_بایټونه))

#تعریف_کړه PUB_DER_MAX_بایټونه   (RSA_PUB_DER_MAX_بایټونه > ECP_PUB_DER_MAX_بایټونه ? \
                             RSA_PUB_DER_MAX_بایټونه : ECP_PUB_DER_MAX_بایټونه)

ساكن CURLcode mbed_سېټ_ssl_نسخه_min_max(
  بنية Curl_easy *معلومات,
  بنية mbed_ssl_backend_معلومات *backend,
  بنية ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_نسخه ver_min =
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_نسخه_TLS1_2
#وإلا
    MBEDTLS_SSL_نسخه_TLS1_3
#د_که_پای
    ;
  mbedtls_ssl_protocol_نسخه ver_max =
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_نسخه_TLS1_3
#وإلا
    MBEDTLS_SSL_نسخه_TLS1_2
#د_که_پای
    ;

  DEBUGASSERT(conn_config->نسخه_2 != CURL_SSLVERSION_DEFAULT);
  بدل(conn_config->نسخه_2) {
  حالة CURL_SSLVERSION_TLSv1:
  حالة CURL_SSLVERSION_TLSv1_0:
  حالة CURL_SSLVERSION_TLSv1_1:
  حالة CURL_SSLVERSION_TLSv1_2:
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_نسخه_TLS1_2;
    اكسر;
#د_که_پای
  حالة CURL_SSLVERSION_TLSv1_3:
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_نسخه_TLS1_3;
    اكسر;
#د_که_پای
  افتراضي:
    failf(معلومات, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->نسخه_2);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  بدل(conn_config->نسخه_max) {
  حالة CURL_SSLVERSION_MAX_DEFAULT:
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_TLSv1_3:
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_نسخه_TLS1_3;
    اكسر;
#د_که_پای
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_2
  حالة CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_نسخه_TLS1_2;
    اكسر;
#د_که_پای
  حالة CURL_SSLVERSION_MAX_TLSv1_1:
  حالة CURL_SSLVERSION_MAX_TLSv1_0:
  افتراضي:
    failf(معلومات, "mbedTLS: unsupported maximum TLS version value");
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
#که_تعریف_شوی MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
ساكن صحيح mbed_cipher_suite_get_str(uint16_t id, حرف *buf, size_t buf_اندازه,
                                     bool prefer_rfc)
{
  إذا(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_اندازه, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  وإلا
    أرجع Curl_cipher_suite_get_str(id, buf, buf_اندازه, prefer_rfc);
  أرجع 0;
}

ساكن uint16_t mbed_cipher_suite_walk_str(ثابت حرف **str, ثابت حرف **پای_ټکی)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, پای_ټکی);
  size_t len = *پای_ټکی - *str;
  ساكن ثابت حرف ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  إذا(!id) {
    إذا((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  أرجع id;
}
#وإلا
#تعریف_کړه mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#تعریف_کړه mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#د_که_پای

ساكن CURLcode mbed_سېټ_selected_ciphers(
  بنية Curl_easy *معلومات,
  بنية mbed_ssl_backend_معلومات *backend,
  ثابت حرف *ciphers12,
  ثابت حرف *ciphers13)
{
  ثابت حرف *ciphers = ciphers12;
  ثابت صحيح *supported;
  صحيح *selected;
  size_t supported_len, شمېر = 0, default13_شمېر = 0, i, j;
  ثابت حرف *ptr, *پای_ټکی;

  supported = mbedtls_ssl_list_ciphersuites();
  لكل(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(حجم(صحيح) * (supported_len + 1));
  إذا(!selected)
    أرجع CURLE_بهر_د_حافظه;

#که_نه_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3
  (فراغ)ciphers13, (فراغ)j;
#وإلا
  إذا(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    لكل(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      إذا(مزي_پرتله(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        استمر;

      selected[شمېر++] = id;
    }

    default13_شمېر = شمېر;
  }
  وإلا
    ciphers = ciphers13;

جمع_کړه_ciphers:
#د_که_پای
  لكل(ptr = ciphers; ptr[0] != '\0' && شمېر < supported_len; ptr = پای_ټکی) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &پای_ټکی);

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
              (صحيح)(پای_ټکی - ptr), ptr);
      استمر;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    لكل(i = 0; i < شمېر && selected[i] != id; i++)
      ;
    إذا(i < شمېر) {
      إذا(i >= default13_شمېر)
        infof(معلومات, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (صحيح)(پای_ټکی - ptr), ptr);
      استمر;
    }

    selected[شمېر++] = id;
  }

#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3
  إذا(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    اذهب جمع_کړه_ciphers;
  }

  إذا(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    لكل(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      إذا(!مزي_پرتله(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        استمر;

      /* No duplicates allowed (so selected cannot overflow) */
      لكل(i = 0; i < شمېر && selected[i] != id; i++)
        ;
      إذا(i < شمېر)
        استمر;

      selected[شمېر++] = id;
    }
  }
#د_که_پای

  selected[شمېر] = 0;

  إذا(شمېر == 0) {
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
#د_که_پای
}

ساكن فراغ mbed_ایستل_certinfo(بنية Curl_easy *معلومات,
                                  ثابت mbedtls_x509_crt *crt)
{
  CURLcode پایله;
  ثابت mbedtls_x509_crt *cur;
  صحيح cert_شمېر = 0;
  صحيح i;

  لكل(cur = crt; cur && cert_شمېر <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->بل)
    cert_شمېر++;

  إذا(cert_شمېر > MAX_ALLOWED_CERT_AMOUNT) {
    infof(معلومات, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    أرجع;
  }

  پایله = Curl_ssl_init_certinfo(معلومات, cert_شمېر);

  لكل(i = 0, cur = crt; پایله == CURLE_OK && cur; ++i, cur = cur->بل) {
    پایله = Curl_ایستل_certinfo(معلومات, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

ساكن صحيح mbed_verify_cb(فراغ *ptr, mbedtls_x509_crt *crt,
                          صحيح ژوروالی, uint32_t *flags)
{
  بنية Curl_cfilter *cf = (بنية Curl_cfilter *)ptr;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_easy *معلومات = CF_معلومات_CURRENT(cf);

  إذا(ژوروالی == 0) {
    إذا(معلومات->سېټ.verbose)
      mbed_dump_cert_info(معلومات, crt);
    إذا(معلومات->سېټ.ssl.certinfo)
      mbed_ایستل_certinfo(معلومات, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  إذا(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  إذا(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  إذا(*flags) {
#که_نه_تعریف_شوی MBEDTLS_X509_REMOVE_INFO
    حرف buf[128];
    mbedtls_x509_crt_verify_info(buf, حجم(buf), "", *flags);
    failf(معلومات, "mbedTLS: %s", buf);
#وإلا
    failf(معلومات, "mbedTLS: certificate verification error 0x%08x",
          (بینښه صحيح)*flags);
#د_که_پای
  }

  أرجع 0;
}

ساكن CURLcode mbed_بارول_cacert(بنية Curl_cfilter *cf,
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
#که_تعریف_شوی MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#د_که_پای
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  إذا(ca_info_blob && verifypeer) {
#که_تعریف_شوی MBEDTLS_PEM_PARSE_C
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
      بینښه حرف *newblob = curlx_memdup0(ca_info_blob->معلومات,
                                             ca_info_blob->len);
      إذا(!newblob)
        أرجع CURLE_بهر_د_حافظه;

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
#د_که_پای

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (بینښه صحيح)-ret, errorbuf);
      أرجع CURLE_SSL_CERTPROBLEM;
    }
  }

  إذا(ssl_cafile && verifypeer) {
#که_تعریف_شوی MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (بینښه صحيح)-ret, errorbuf);
      أرجع CURLE_SSL_CACERT_BADFILE;
    }
#وإلا
    failf(معلومات, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
  }

  إذا(ssl_capath) {
#که_تعریف_شوی MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_لار(&backend->cacert, ssl_capath);

    إذا(ret < 0) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (بینښه صحيح)-ret, errorbuf);

      إذا(verifypeer)
        أرجع CURLE_SSL_CACERT_BADFILE;
    }
#وإلا
    failf(معلومات, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
  }

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_بارول_clicert(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  حرف * ثابت ssl_cert = conn_config->clientcert;
  ثابت بنية curl_blob *ssl_cert_blob = conn_config->cert_blob;
#که_تعریف_شوی MBEDTLS_PEM_PARSE_C
  ثابت حرف * ثابت ssl_cert_type = conn_config->cert_type;
#د_که_پای
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  إذا(ssl_cert) {
#که_تعریف_شوی MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (بینښه صحيح)-ret, errorbuf);

      أرجع CURLE_SSL_CERTPROBLEM;
    }
#وإلا
    failf(معلومات, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
  }

  إذا(ssl_cert_blob) {
#که_تعریف_شوی MBEDTLS_PEM_PARSE_C
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
      بینښه حرف *newblob = curlx_memdup0(ssl_cert_blob->معلومات,
                                             ssl_cert_blob->len);
      إذا(!newblob)
        أرجع CURLE_بهر_د_حافظه;
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
#د_که_پای

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (بینښه صحيح)-ret, errorbuf);
      أرجع CURLE_SSL_CERTPROBLEM;
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_بارول_privkey(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  صحيح ret = -1;
  حرف errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  إذا(conn_config->کلی || conn_config->کلی_blob) {
    إذا(conn_config->کلی) {
#که_تعریف_شوی MBEDTLS_FS_IO
#إذا MBEDTLS_نسخه_شمېره >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->کلی,
                                     conn_config->کلی_passwd);
      إذا(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_کلی_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_کلی_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وإلا
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->کلی,
                                     conn_config->کلی_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      إذا(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#د_که_پای

      إذا(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(معلومات, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->کلی, (بینښه صحيح)-ret, errorbuf);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
#وإلا
      failf(معلومات, "mbedTLS: functions that use the file system not built in");
      أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
    }
    وإلا {
      ثابت بنية curl_blob *ssl_کلی_blob = conn_config->کلی_blob;
      ثابت حرف *passwd = conn_config->کلی_passwd;
      ثابت bool is_der = conn_config->کلی_type &&
                          curl_strequal(conn_config->کلی_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      بینښه حرف *newblob = curlx_memdup0(ssl_کلی_blob->معلومات,
                                             ssl_کلی_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_کلی_blob->len : ssl_کلی_blob->len + 1;
      إذا(!newblob)
        أرجع CURLE_بهر_د_حافظه;

#إذا MBEDTLS_نسخه_شمېره >= 0x04000000
      ret = mbedtls_pk_parse_کلی(&backend->pk, newblob, keylen,
                                 (ثابت بینښه حرف *)passwd,
                                 passwd ? مزي_اوږدوالی(passwd) : 0);
      إذا(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_کلی_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_کلی_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#وإلا
      ret = mbedtls_pk_parse_کلی(&backend->pk, newblob, keylen,
                                 (ثابت بینښه حرف *)passwd,
                                 passwd ? مزي_اوږدوالی(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      إذا(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#د_که_پای
      curlx_free(newblob);

      إذا(ret) {
        mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
        failf(معلومات, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (بینښه صحيح)-ret, errorbuf);
        أرجع CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  أرجع CURLE_OK;
}

ساكن CURLcode mbed_بارول_crl(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ثابت حرف * ثابت ssl_crlfile = conn_config->CRLfile;

#که_تعریف_شوی MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  إذا(ssl_crlfile) {
    حرف errorbuf[128];
#که_تعریف_شوی MBEDTLS_FS_IO
    صحيح ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    إذا(ret) {
      mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
      failf(معلومات, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (بینښه صحيح)-ret, errorbuf);

      أرجع CURLE_SSL_CRL_BADFILE;
    }
#وإلا
    (فراغ)errorbuf;
    failf(معلومات, "mbedTLS: functions that use the file system not built in");
    أرجع CURLE_NOT_BUILT_دننه;
#د_که_پای
  }
#وإلا
  (فراغ)backend;
  إذا(ssl_crlfile) {
    failf(معلومات, "mbedTLS: CRL support not built in");
    أرجع CURLE_NOT_BUILT_دننه;
  }
#د_که_پای

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
    ret = mbedtls_ssl_session_بارول(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    إذا(ret) {
      failf(معلومات, "SSL session error loading: -0x%x", (بینښه صحيح)-ret);
    }
    وإلا {
      ret = mbedtls_ssl_سېټ_session(&backend->ssl, &session);
      إذا(ret)
        failf(معلومات, "SSL session error setting: -0x%x", (بینښه صحيح)-ret);
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
  CURLcode پایله;
  حرف errorbuf[128];

  infof(معلومات, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->پورټ);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_بهیر,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  إذا(ret) {
    failf(معلومات, "mbedTLS: ssl_config failed");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

#که_تعریف_شوی MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, معلومات);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_سېټ_پوله(4);
#د_که_پای

#إذا defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_نسخه_شمېره >= 0x03060100 &&   \
  MBEDTLS_نسخه_شمېره < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#د_که_پای

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_بل);

  پایله = mbed_سېټ_ssl_نسخه_min_max(معلومات, backend, conn_config);
  إذا(پایله)
    أرجع پایله;

#إذا MBEDTLS_نسخه_شمېره < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#د_که_پای

  ret = mbedtls_ssl_تیاری(&backend->ssl,
                          &backend->config);
  إذا(ret) {
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(معلومات, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (بینښه صحيح)-ret, errorbuf);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_سېټ_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      عدم /*  rev_timeout() */);

#که_نه_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3
  إذا(conn_config->cipher_list) {
    پایله = mbed_سېټ_selected_ciphers(معلومات, backend,
                                       conn_config->cipher_list, عدم);
#وإلا
  إذا(conn_config->cipher_list || conn_config->cipher_list13) {
    پایله = mbed_سېټ_selected_ciphers(معلومات, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#د_که_پای
    إذا(پایله) {
      failf(معلومات, "mbedTLS: failed to set cipher suites");
      أرجع پایله;
    }
  }
  وإلا {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#که_تعریف_شوی MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#د_که_پای

#که_تعریف_شوی MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#د_که_پای

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
    CURLcode sresult = Curl_ssl_scache_take(cf, معلومات, connssl->peer.scache_کلی,
                                            &sc_session);
    إذا(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_بارول(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      إذا(ret) {
        failf(معلومات, "SSL session error loading: -0x%x", (بینښه صحيح)-ret);
      }
      وإلا {
        ret = mbedtls_ssl_سېټ_session(&backend->ssl, &session);
        إذا(ret)
          failf(معلومات, "SSL session error setting: -0x%x", (بینښه صحيح)-ret);
        وإلا
          infof(معلومات, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, معلومات, connssl->peer.scache_کلی,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#که_تعریف_شوی MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#وإلا
                            عدم
#د_که_پای
    );

  إذا(conn_config->کلی || conn_config->کلی_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  إذا(mbedtls_ssl_سېټ_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(معلومات, "Failed to set SNI");
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

#که_تعریف_شوی HAS_ALPN_MBEDTLS
  إذا(connssl->alpn) {
    بنية alpn_proto_buf proto;
    size_t i;

    لكل(i = 0; i < connssl->alpn->شمېر; ++i) {
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
#د_که_پای

  /* give application a chance to interfere with mbedTLS set up. */
  إذا(معلومات->سېټ.ssl_fsslctx) {
    پایله = (*معلومات->سېټ.ssl_fsslctx)(معلومات, &backend->config,
                                      معلومات->سېټ.ssl_fsslctxp);
    إذا(پایله)
      failf(معلومات, "error signaled by SSL ctx callback");
  }

  أرجع پایله;
}

ساكن CURLcode mbed_connect_step1(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode پایله;

  إذا((conn_config->نسخه_2 == CURL_SSLVERSION_SSLv2) ||
     (conn_config->نسخه_2 == CURL_SSLVERSION_SSLv3)) {
    failf(معلومات, "Not supported SSL version");
    أرجع CURLE_NOT_BUILT_دننه;
  }

  پایله = mbed_بارول_cacert(cf, معلومات);
  إذا(!پایله)
    پایله = mbed_بارول_clicert(cf, معلومات);
  إذا(!پایله)
    پایله = mbed_بارول_privkey(cf, معلومات);
  إذا(!پایله)
    پایله = mbed_بارول_crl(cf, معلومات);
  إذا(!پایله)
    پایله = mbed_configure_ssl(cf, معلومات);
  إذا(پایله)
    أرجع پایله;

  connssl->connecting_state = ssl_connect_2;
  أرجع CURLE_OK;
}

#إذا defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#تعریف_کړه HAVE_PINNED_PUBKEY
#د_که_پای

ساكن CURLcode mbed_connect_step2(بنية Curl_cfilter *cf,
                                   بنية Curl_easy *معلومات)
{
#إذا defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode پایله;
#د_که_پای
  صحيح ret;
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
#که_تعریف_شوی HAVE_PINNED_PUBKEY
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
  ثابت حرف * ثابت pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY);
#وإلا
  ثابت حرف * ثابت pinnedpubkey =
    CURL_EASY_STR(معلومات, STRING_SSL_PINNEDPUBLICKEY);
#د_که_پای
#د_که_پای

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
                mbedtls_ssl_get_نسخه_شمېره(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, حجم(errorbuf));
    failf(معلومات, "ssl_handshake returned: (-0x%04X) %s",
          (بینښه صحيح)-ret, errorbuf);
    أرجع CURLE_SSL_CONNECT_ERROR;
  }

  {
    حرف cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, حجم(cipher_str), TRUE);
    infof(معلومات, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_نسخه(&backend->ssl), cipher_str);
  }

#که_تعریف_شوی HAVE_PINNED_PUBKEY
  إذا(pinnedpubkey) {
    صحيح اندازه;
    ثابت mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = عدم;
    بینښه حرف *pubkey = عدم;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    إذا(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(معلومات, "Failed due to missing peer certificate");
      أرجع CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, حجم(*p));

    إذا(!p)
      أرجع CURLE_بهر_د_حافظه;

    pubkey = curlx_malloc(PUB_DER_MAX_بایټونه);

    إذا(!pubkey) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    إذا(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(معلومات, "Failed copying peer certificate");
      پایله = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      اذهب pinnedpubkey_error;
    }

    اندازه = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_بایټونه);

    إذا(اندازه <= 0) {
      failf(معلومات, "Failed copying public key from peer certificate");
      پایله = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      اذهب pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    پایله = Curl_pin_peer_pubkey(معلومات,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_بایټونه - اندازه], اندازه);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    إذا(پایله)
      أرجع پایله;
  }
#د_که_پای

#که_تعریف_شوی HAS_ALPN_MBEDTLS
  إذا(connssl->alpn) {
    ثابت حرف *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    پایله = Curl_alpn_سېټ_negotiated(cf, معلومات, connssl,
                                      (ثابت بینښه حرف *)proto,
                                      proto ? مزي_اوږدوالی(proto) : 0);
    إذا(پایله)
      أرجع پایله;
  }
#د_که_پای

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
  بینښه حرف *sdata = عدم;
  size_t slen = 0;
  صحيح ietf_tls_id;
  CURLcode پایله = CURLE_OK;
  صحيح ret;

  DEBUGASSERT(backend);
  إذا(!Curl_ssl_scache_use(cf, معلومات))
    أرجع CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  إذا(ret) {
    failf(معلومات, "mbedtls_ssl_get_session returned -0x%x", (بینښه صحيح)-ret);
    پایله = CURLE_SSL_CONNECT_ERROR;
    اذهب بهر;
  }

  mbedtls_ssl_session_save(&session, عدم, 0, &slen);
  إذا(!slen) {
    failf(معلومات, "failed to serialize session: length is 0");
    اذهب بهر;
  }

  sdata = curlx_malloc(slen);
  إذا(!sdata) {
    پایله = CURLE_بهر_د_حافظه;
    اذهب بهر;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  إذا(ret) {
    failf(معلومات, "failed to serialize session: -0x%x", (بینښه صحيح)-ret);
    اذهب بهر;
  }

  ietf_tls_id = mbedtls_ssl_get_نسخه_شمېره(&backend->ssl);
  پایله = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = عدم;  /* call took ownership */
  إذا(!پایله && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    پایله = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  إذا(!پایله) {
    پایله = Curl_ssl_scache_put(cf, معلومات, connssl->peer.scache_کلی,
                                 sc_session);
    sc_session = عدم;
  }

بهر:
  إذا(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  أرجع پایله;
}

ساكن CURLcode mbed_send(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                          ثابت فراغ *mem, size_t len, size_t *pnwritten)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  CURLcode پایله = CURLE_OK;
  صحيح nwritten;

  DEBUGASSERT(backend);
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#د_که_پای
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
      پایله = CURLE_BAD_FUNCTION_دلیل;
      اذهب بهر;
    }
    CURL_TRC_CF(معلومات, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ثابت بینښه حرف *)mem, len);

  إذا(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  وإلا {
    CURL_TRC_CF(معلومات, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (بینښه صحيح)-nwritten);
    بدل(nwritten) {
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3
    حالة MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      پایله = mbed_new_session(cf, معلومات);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      إذا(!پایله)
        اذهب do_send;
      اكسر;
#د_که_پای
    حالة MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      پایله = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      پایله = CURLE_AGAIN;
      اكسر;
    افتراضي:
      پایله = CURLE_SEND_ERROR;
      اكسر;
    }
    إذا((پایله == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

بهر:
  CURL_TRC_CF(معلومات, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (صحيح)پایله, *pnwritten);
  أرجع پایله;
}

ساكن CURLcode mbedtls_shutdown(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 bool send_shutdown, bool *بشپړ)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend =
    (بنية mbed_ssl_backend_معلومات *)connssl->backend;
  بینښه حرف buf[1024];
  CURLcode پایله = CURLE_OK;
  صحيح ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  إذا(!backend->initialized || cf->shutdown) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *بشپړ = FALSE;

  إذا(!backend->لېږل_شوی_shutdown) {
    /* do this only once */
    backend->لېږل_شوی_shutdown = TRUE;
    إذا(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      بدل(ret) {
      حالة 0: /* we sent it, receive from the server */
        اكسر;
      حالة MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *بشپړ = TRUE;
        اذهب بهر;
      حالة MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        اذهب بهر;
      حالة MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        اذهب بهر;
      افتراضي:
        CURL_TRC_CF(معلومات, cf, "mbedtls_shutdown error -0x%04X",
                    (بینښه صحيح)-ret);
        پایله = CURLE_RECV_ERROR;
        اذهب بهر;
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
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3
    إذا(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      استمر;
#د_که_پای
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
    *بشپړ = TRUE;
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
                (بینښه صحيح)-ret);
    پایله = CURLE_RECV_ERROR;
  }

بهر:
  cf->shutdown = (پایله || *بشپړ);
  أرجع پایله;
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
#که_تعریف_شوی MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#د_که_پای
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
  CURLcode پایله = CURLE_OK;
  صحيح nread;

  DEBUGASSERT(backend);
#که_تعریف_شوی MBEDTLS_SSL_SESSION_TICKETS
do_read:
#د_که_پای
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (بینښه حرف *)buf, buffersize);
  إذا(nread > 0)
    *pnread = (size_t)nread;
  وإلا {
    حرف errorbuf[128];
    CURL_TRC_CF(معلومات, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (بینښه صحيح)-nread);
    بدل(nread) {
#که_تعریف_شوی MBEDTLS_SSL_SESSION_TICKETS
    حالة MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      پایله = mbed_new_session(cf, معلومات);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      إذا(!پایله)
        اذهب do_read;
      اكسر;
#د_که_پای
    حالة MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      پایله = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      پایله = CURLE_AGAIN;
      اكسر;
    حالة MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      پایله = CURLE_OK;
      اكسر;
    افتراضي:
      mbedtls_strerror(nread, errorbuf, حجم(errorbuf));
      failf(معلومات, "ssl_read returned: (-0x%04X) %s", (بینښه صحيح)-nread,
            errorbuf);
      پایله = CURLE_RECV_ERROR;
      اكسر;
    }
  }
  أرجع پایله;
}

ساكن size_t mbedtls_نسخه(حرف *بفر, size_t اندازه)
{
#که_تعریف_شوی MBEDTLS_نسخه_C
  /* if mbedtls_version_get_number() is available it is better */
  بینښه صحيح نسخه_2 = mbedtls_نسخه_get_شمېره();
  أرجع curl_msnprintf(بفر, اندازه, "mbedTLS/%u.%u.%u", نسخه_2 >> 24,
                        (نسخه_2 >> 16) & 0xff, (نسخه_2 >> 8) & 0xff);
#وإلا
  أرجع curl_msnprintf(بفر, اندازه, "mbedTLS/%s", MBEDTLS_نسخه_STRING);
#د_که_پای
}

/* 'data' might be NULL */
ساكن CURLcode mbedtls_random(بنية Curl_easy *معلومات,
                               بینښه حرف *entropy, size_t اوږدتیا)
{
  psa_حالت_t حالت;
  (فراغ)معلومات;

  حالت = psa_generate_random(entropy, اوږدتیا);

  أرجع حالت == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

ساكن CURLcode mbedtls_connect(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                bool *بشپړ)
{
  CURLcode پایله;
  بنية ssl_connect_معلومات *connssl = cf->ctx;

  /* check if the connection has already been established */
  إذا(ssl_connection_complete == connssl->state) {
    *بشپړ = TRUE;
    أرجع CURLE_OK;
  }

  *بشپړ = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  إذا(connssl->connecting_state == ssl_connect_1) {
    پایله = mbed_connect_step1(cf, معلومات);
    إذا(پایله)
      أرجع پایله;
  }

  إذا(connssl->connecting_state == ssl_connect_2) {
    پایله = mbed_connect_step2(cf, معلومات);
    إذا(پایله)
      أرجع پایله;
  }

  إذا(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    بنية ssl_connect_معلومات *ctx = cf->ctx;
    بنية mbed_ssl_backend_معلومات *backend =
      (بنية mbed_ssl_backend_معلومات *)ctx->backend;

    إذا(mbedtls_ssl_get_نسخه_شمېره(&backend->ssl) <=
       MBEDTLS_SSL_نسخه_TLS1_2) {
      پایله = mbed_new_session(cf, معلومات);
      إذا(پایله)
        أرجع پایله;
    }
    connssl->connecting_state = ssl_connect_بشپړ;
  }

  إذا(connssl->connecting_state == ssl_connect_بشپړ) {
    connssl->state = ssl_connection_complete;
    *بشپړ = TRUE;
  }

  أرجع CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
ساكن صحيح mbedtls_init(فراغ)
{
#إذا MBEDTLS_نسخه_شمېره < 0x04000000
  صحيح ret = 0;
#د_که_پای
  psa_حالت_t حالت;
  حالت = psa_crypto_init();

  إذا(حالت != PSA_SUCCESS)
    أرجع 0;

#إذا MBEDTLS_نسخه_شمېره < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_تخم(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              عدم, 0);

  إذا(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    أرجع 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_سېټ_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#د_که_پای
  أرجع 1;
}

ساكن فراغ mbedtls_cleanup(فراغ)
{
  mbedtls_psa_crypto_free();

#إذا MBEDTLS_نسخه_شمېره < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#د_که_پای
}

ساكن bool mbedtls_معلومات_pending(بنية Curl_cfilter *cf,
                                 ثابت بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *ctx = cf->ctx;
  بنية mbed_ssl_backend_معلومات *backend;

  (فراغ)معلومات;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (بنية mbed_ssl_backend_معلومات *)ctx->backend;
  أرجع mbedtls_ssl_get_بایټونه_avail(&backend->ssl) != 0;
}

ساكن CURLcode mbedtls_sha256sum(ثابت بینښه حرف *ننوت,
                                  size_t inputlen,
                                  بینښه حرف *sha256sum,
                                  size_t sha256len)
{
#إذا defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_حالت_t حالت;
  size_t sha256len_actual;
  حالت = psa_hash_compute(PSA_ALG_SHA_256, ننوت, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  إذا(حالت != PSA_SUCCESS)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  أرجع CURLE_OK;
#وإلا
  (فراغ)sha256len;
  أرجع Curl_sha256it(sha256sum, ننوت, inputlen);
#د_که_پای
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

  SSLSUPP_CA_لار |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#که_تعریف_شوی HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#د_که_پای
  SSLSUPP_SSL_CTX |
#که_تعریف_شوی MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#د_که_پای
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#که_تعریف_شوی MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#د_که_پای
  0,

  حجم(بنية mbed_ssl_backend_معلومات),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_نسخه,                  /*نسخه*/
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

#د_که_پای /* USE_MBEDTLS */
