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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#अन्तर्भाव <mbedtls/version.h>
#यदि MBEDTLS_संस्करणम्_सङ्ख्या < 0x03020000
#दोषः "mbedTLS 3.2.0 or greater required"
#यदि_अन्तः
#अन्तर्भाव <psa/crypto_config.h>
#अन्तर्भाव <mbedtls/net_sockets.h>
#अन्तर्भाव <mbedtls/ssl.h>
#अन्तर्भाव <mbedtls/x509.h>
#अन्तर्भाव <mbedtls/psa_util.h>

#यदि MBEDTLS_संस्करणम्_सङ्ख्या < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#दोषः "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#यदि_अन्तः

#अन्तर्भाव <mbedtls/error.h>
#यदि MBEDTLS_संस्करणम्_सङ्ख्या < 0x04000000
#अन्तर्भाव <mbedtls/entropy.h>
#अन्तर्भाव <mbedtls/ctr_drbg.h>
#यदि_अन्तः
#यदि_परिभाषितम् MBEDTLS_DEBUG
#अन्तर्भाव <mbedtls/debug.h>
#यदि_अन्तः

#अन्तर्भाव "vtls/cipher_suite.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "vtls/mbedtls.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "vtls/vtls_int.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vtls/x509asn1.h"
#अन्तर्भाव "connect.h" /* for the connect timeout */
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curl_sha256.h"

/* ALPN for http2 */
#यदि defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#परिभाषय HAS_ALPN_MBEDTLS
#यदि_अन्तः

संरचना mbed_ssl_backend_दत्त {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#यदि_परिभाषितम् MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#यदि_अन्तः
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#यदि_परिभाषितम् HAS_ALPN_MBEDTLS
  ध्रुव वर्ण *protocols[3];
#यदि_अन्तः
  पूर्णाङ्क *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(प्रेषितम्_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#यदि MBEDTLS_संस्करणम्_सङ्ख्या < 0x04000000
संरचना rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

स्थिर संरचना rng_context_t rng;
#यदि_अन्तः

#यदि_अपरिभाषितम् MBEDTLS_ERROR_C
#परिभाषय mbedtls_strerror(a, b, c) b[0] = 0
#यदि_अन्तः

#यदि_परिभाषितम् MBEDTLS_DEBUG
स्थिर शून्य mbed_debug(शून्य *context, पूर्णाङ्क level, ध्रुव वर्ण *f_name,
                       पूर्णाङ्क पङ्क्तिः_nb, ध्रुव वर्ण *पङ्क्तिः)
{
  संरचना Curl_easy *दत्त = (संरचना Curl_easy *)context;
  (शून्य)level;
  (शून्य)पङ्क्तिः_nb;
  (शून्य)f_name;

  यदि(दत्त) {
    size_t len = वाक्यदैर्घ्यम्(पङ्क्तिः);
    यदि(len && (पङ्क्तिः[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(दत्त, "%.*s", (पूर्णाङ्क)len, पङ्क्तिः);
  }
}
#यदि_अन्तः

स्थिर पूर्णाङ्क mbedtls_bio_cf_write(शून्य *bio,
                                ध्रुव अचिह्नित वर्ण *buf, size_t blen)
{
  संरचना Curl_cfilter *cf = bio;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  size_t nwritten;
  CURLcode परिणाम;

  DEBUGASSERT(दत्त);
  यदि(!दत्त)
    प्रत्यागम 0;

  परिणाम = Curl_conn_cf_send(cf->अग्र, दत्त, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(दत्त, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (पूर्णाङ्क)परिणाम, nwritten);
  यदि(परिणाम == CURLE_AGAIN)
    प्रत्यागम MBEDTLS_ERR_SSL_WANT_WRITE;
  प्रत्यागम परिणाम ? -1 : (पूर्णाङ्क)nwritten;
}

स्थिर पूर्णाङ्क mbedtls_bio_cf_read(शून्य *bio, अचिह्नित वर्ण *buf, size_t blen)
{
  संरचना Curl_cfilter *cf = bio;
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);
  size_t nread = 0;
  CURLcode परिणाम;

  DEBUGASSERT(दत्त);
  यदि(!दत्त)
    प्रत्यागम 0;
  /* OpenSSL catches this case, so should we. */
  यदि(!buf)
    प्रत्यागम 0;

  परिणाम = Curl_conn_cf_recv(cf->अग्र, दत्त, (वर्ण *)buf, blen, &nread);
  CURL_TRC_CF(दत्त, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (पूर्णाङ्क)परिणाम, nread);
  यदि(परिणाम == CURLE_AGAIN)
    प्रत्यागम MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  प्रत्यागम परिणाम ? -1 : (पूर्णाङ्क)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#परिभाषय RSA_PUB_DER_MAX_अष्टकानि   (38 + (2 * MBEDTLS_MPI_MAX_आकार))
#परिभाषय ECP_PUB_DER_MAX_अष्टकानि   (30 + (2 * MBEDTLS_ECP_MAX_अष्टकानि))

#परिभाषय PUB_DER_MAX_अष्टकानि   (RSA_PUB_DER_MAX_अष्टकानि > ECP_PUB_DER_MAX_अष्टकानि ? \
                             RSA_PUB_DER_MAX_अष्टकानि : ECP_PUB_DER_MAX_अष्टकानि)

स्थिर CURLcode mbed_समुच्चयः_ssl_संस्करणम्_min_max(
  संरचना Curl_easy *दत्त,
  संरचना mbed_ssl_backend_दत्त *backend,
  संरचना ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_संस्करणम् ver_min =
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_संस्करणम्_TLS1_2
#अन्यथा
    MBEDTLS_SSL_संस्करणम्_TLS1_3
#यदि_अन्तः
    ;
  mbedtls_ssl_protocol_संस्करणम् ver_max =
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_संस्करणम्_TLS1_3
#अन्यथा
    MBEDTLS_SSL_संस्करणम्_TLS1_2
#यदि_अन्तः
    ;

  DEBUGASSERT(conn_config->संस्करणम्_2 != CURL_SSLVERSION_DEFAULT);
  विकल्प(conn_config->संस्करणम्_2) {
  पक्ष CURL_SSLVERSION_TLSv1:
  पक्ष CURL_SSLVERSION_TLSv1_0:
  पक्ष CURL_SSLVERSION_TLSv1_1:
  पक्ष CURL_SSLVERSION_TLSv1_2:
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_संस्करणम्_TLS1_2;
    भङ्ग;
#यदि_अन्तः
  पक्ष CURL_SSLVERSION_TLSv1_3:
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_संस्करणम्_TLS1_3;
    भङ्ग;
#यदि_अन्तः
  स्वभाव:
    failf(दत्त, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->संस्करणम्_2);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  विकल्प(conn_config->संस्करणम्_max) {
  पक्ष CURL_SSLVERSION_MAX_DEFAULT:
  पक्ष CURL_SSLVERSION_MAX_NONE:
  पक्ष CURL_SSLVERSION_MAX_TLSv1_3:
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_संस्करणम्_TLS1_3;
    भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_2
  पक्ष CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_संस्करणम्_TLS1_2;
    भङ्ग;
#यदि_अन्तः
  पक्ष CURL_SSLVERSION_MAX_TLSv1_1:
  पक्ष CURL_SSLVERSION_MAX_TLSv1_0:
  स्वभाव:
    failf(दत्त, "mbedTLS: unsupported maximum TLS version value");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_संस्करणम्(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_संस्करणम्(&backend->config, ver_max);

  प्रत्यागम CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#यदि_परिभाषितम् MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
स्थिर पूर्णाङ्क mbed_cipher_suite_get_str(uint16_t id, वर्ण *buf, size_t buf_आकार,
                                     bool prefer_rfc)
{
  यदि(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_आकार, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  अन्यथा
    प्रत्यागम Curl_cipher_suite_get_str(id, buf, buf_आकार, prefer_rfc);
  प्रत्यागम 0;
}

स्थिर uint16_t mbed_cipher_suite_walk_str(ध्रुव वर्ण **str, ध्रुव वर्ण **अन्तः)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, अन्तः);
  size_t len = *अन्तः - *str;
  स्थिर ध्रुव वर्ण ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  यदि(!id) {
    यदि((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  प्रत्यागम id;
}
#अन्यथा
#परिभाषय mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#परिभाषय mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#यदि_अन्तः

स्थिर CURLcode mbed_समुच्चयः_selected_ciphers(
  संरचना Curl_easy *दत्त,
  संरचना mbed_ssl_backend_दत्त *backend,
  ध्रुव वर्ण *ciphers12,
  ध्रुव वर्ण *ciphers13)
{
  ध्रुव वर्ण *ciphers = ciphers12;
  ध्रुव पूर्णाङ्क *supported;
  पूर्णाङ्क *selected;
  size_t supported_len, गणनम् = 0, default13_गणनम् = 0, i, j;
  ध्रुव वर्ण *ptr, *अन्तः;

  supported = mbedtls_ssl_list_ciphersuites();
  प्रति(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(परिमाण(पूर्णाङ्क) * (supported_len + 1));
  यदि(!selected)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

#यदि_अपरिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3
  (शून्य)ciphers13, (शून्य)j;
#अन्यथा
  यदि(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    प्रति(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      यदि(वाक्ये_तुलय(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        अनुवर्त;

      selected[गणनम्++] = id;
    }

    default13_गणनम् = गणनम्;
  }
  अन्यथा
    ciphers = ciphers13;

सङ्कलय_ciphers:
#यदि_अन्तः
  प्रति(ptr = ciphers; ptr[0] != '\0' && गणनम् < supported_len; ptr = अन्तः) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &अन्तः);

    /* Check if cipher is supported */
    यदि(id) {
      प्रति(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      यदि(i == supported_len)
        id = 0;
    }
    यदि(!id) {
      यदि(ptr[0] != '\0')
        infof(दत्त, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (पूर्णाङ्क)(अन्तः - ptr), ptr);
      अनुवर्त;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    प्रति(i = 0; i < गणनम् && selected[i] != id; i++)
      ;
    यदि(i < गणनम्) {
      यदि(i >= default13_गणनम्)
        infof(दत्त, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (पूर्णाङ्क)(अन्तः - ptr), ptr);
      अनुवर्त;
    }

    selected[गणनम्++] = id;
  }

#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3
  यदि(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    गच्छ सङ्कलय_ciphers;
  }

  यदि(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    प्रति(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      यदि(!वाक्ये_तुलय(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        अनुवर्त;

      /* No duplicates allowed (so selected cannot overflow) */
      प्रति(i = 0; i < गणनम् && selected[i] != id; i++)
        ;
      यदि(i < गणनम्)
        अनुवर्त;

      selected[गणनम्++] = id;
    }
  }
#यदि_अन्तः

  selected[गणनम्] = 0;

  यदि(गणनम् == 0) {
    curlx_free(selected);
    failf(दत्त, "mbedTLS: no supported cipher in list");
    प्रत्यागम CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  प्रत्यागम CURLE_OK;
}

स्थिर शून्य mbed_dump_cert_info(संरचना Curl_easy *दत्त,
                                ध्रुव mbedtls_x509_crt *crt)
{
#यदि !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (शून्य)दत्त, (शून्य)crt;
#अन्यथा
  ध्रुव size_t bufsize = 16384;
  वर्ण *p, *बफर = curlx_malloc(bufsize);

  यदि(बफर && mbedtls_x509_crt_info(बफर, bufsize, " ", crt) > 0) {
    infof(दत्त, "Server certificate:");
    प्रति(p = बफर; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(दत्त, "%.*s", (पूर्णाङ्क)s, p);
      p += s;
    }
  }
  अन्यथा
    infof(दत्त, "Unable to dump certificate information");

  curlx_free(बफर);
#यदि_अन्तः
}

स्थिर शून्य mbed_extract_certinfo(संरचना Curl_easy *दत्त,
                                  ध्रुव mbedtls_x509_crt *crt)
{
  CURLcode परिणाम;
  ध्रुव mbedtls_x509_crt *cur;
  पूर्णाङ्क cert_गणनम् = 0;
  पूर्णाङ्क i;

  प्रति(cur = crt; cur && cert_गणनम् <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->अग्र)
    cert_गणनम्++;

  यदि(cert_गणनम् > MAX_ALLOWED_CERT_AMOUNT) {
    infof(दत्त, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    प्रत्यागम;
  }

  परिणाम = Curl_ssl_init_certinfo(दत्त, cert_गणनम्);

  प्रति(i = 0, cur = crt; परिणाम == CURLE_OK && cur; ++i, cur = cur->अग्र) {
    परिणाम = Curl_extract_certinfo(दत्त, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

स्थिर पूर्णाङ्क mbed_verify_cb(शून्य *ptr, mbedtls_x509_crt *crt,
                          पूर्णाङ्क गभीरता, uint32_t *flags)
{
  संरचना Curl_cfilter *cf = (संरचना Curl_cfilter *)ptr;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_easy *दत्त = CF_दत्त_CURRENT(cf);

  यदि(गभीरता == 0) {
    यदि(दत्त->समुच्चयः.verbose)
      mbed_dump_cert_info(दत्त, crt);
    यदि(दत्त->समुच्चयः.ssl.certinfo)
      mbed_extract_certinfo(दत्त, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  यदि(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  यदि(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  यदि(*flags) {
#यदि_अपरिभाषितम् MBEDTLS_X509_REMOVE_INFO
    वर्ण buf[128];
    mbedtls_x509_crt_verify_info(buf, परिमाण(buf), "", *flags);
    failf(दत्त, "mbedTLS: %s", buf);
#अन्यथा
    failf(दत्त, "mbedTLS: certificate verification error 0x%08x",
          (अचिह्नित पूर्णाङ्क)*flags);
#यदि_अन्तः
  }

  प्रत्यागम 0;
}

स्थिर CURLcode mbed_आरोपय_cacert(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ध्रुव संरचना curl_blob *ca_info_blob = conn_config->ca_info_blob;
  ध्रुव वर्ण * ध्रुव ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? रिक्त : conn_config->CAfile);
  ध्रुव bool verifypeer = conn_config->verifypeer;
  ध्रुव वर्ण * ध्रुव ssl_capath = conn_config->CApath;
#यदि_परिभाषितम् MBEDTLS_PEM_PARSE_C
  ध्रुव वर्ण * ध्रुव ssl_cert_type = conn_config->cert_type;
#यदि_अन्तः
  पूर्णाङ्क ret = -1;
  वर्ण errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  यदि(ca_info_blob && verifypeer) {
#यदि_परिभाषितम् MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    यदि((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((वर्ण *)(ca_info_blob->दत्त))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->दत्त,
                                   ca_info_blob->len);
    }
    अन्यथा { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      अचिह्नित वर्ण *newblob = curlx_memdup0(ca_info_blob->दत्त,
                                             ca_info_blob->len);
      यदि(!newblob)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#अन्यथा
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->दत्त,
                                     ca_info_blob->len);
#यदि_अन्तः

    यदि(ret < 0) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(दत्त, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (अचिह्नित पूर्णाङ्क)-ret, errorbuf);
      प्रत्यागम CURLE_SSL_CERTPROBLEM;
    }
  }

  यदि(ssl_cafile && verifypeer) {
#यदि_परिभाषितम् MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    यदि(ret < 0) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(दत्त, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (अचिह्नित पूर्णाङ्क)-ret, errorbuf);
      प्रत्यागम CURLE_SSL_CACERT_BADFILE;
    }
#अन्यथा
    failf(दत्त, "mbedTLS: functions that use the file system not built in");
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
  }

  यदि(ssl_capath) {
#यदि_परिभाषितम् MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_मार्गः(&backend->cacert, ssl_capath);

    यदि(ret < 0) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(दत्त, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (अचिह्नित पूर्णाङ्क)-ret, errorbuf);

      यदि(verifypeer)
        प्रत्यागम CURLE_SSL_CACERT_BADFILE;
    }
#अन्यथा
    failf(दत्त, "mbedTLS: functions that use the file system not built in");
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mbed_आरोपय_clicert(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  वर्ण * ध्रुव ssl_cert = conn_config->clientcert;
  ध्रुव संरचना curl_blob *ssl_cert_blob = conn_config->cert_blob;
#यदि_परिभाषितम् MBEDTLS_PEM_PARSE_C
  ध्रुव वर्ण * ध्रुव ssl_cert_type = conn_config->cert_type;
#यदि_अन्तः
  पूर्णाङ्क ret = -1;
  वर्ण errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  यदि(ssl_cert) {
#यदि_परिभाषितम् MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    यदि(ret) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(दत्त, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (अचिह्नित पूर्णाङ्क)-ret, errorbuf);

      प्रत्यागम CURLE_SSL_CERTPROBLEM;
    }
#अन्यथा
    failf(दत्त, "mbedTLS: functions that use the file system not built in");
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
  }

  यदि(ssl_cert_blob) {
#यदि_परिभाषितम् MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    यदि((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((वर्ण *)(ssl_cert_blob->दत्त))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->दत्त,
                                   ssl_cert_blob->len);
    }
    अन्यथा { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      अचिह्नित वर्ण *newblob = curlx_memdup0(ssl_cert_blob->दत्त,
                                             ssl_cert_blob->len);
      यदि(!newblob)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#अन्यथा
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->दत्त,
                                     ssl_cert_blob->len);
#यदि_अन्तः

    यदि(ret) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(दत्त, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (अचिह्नित पूर्णाङ्क)-ret, errorbuf);
      प्रत्यागम CURLE_SSL_CERTPROBLEM;
    }
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mbed_आरोपय_privkey(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णाङ्क ret = -1;
  वर्ण errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  यदि(conn_config->कुञ्चिका || conn_config->कुञ्चिका_blob) {
    यदि(conn_config->कुञ्चिका) {
#यदि_परिभाषितम् MBEDTLS_FS_IO
#यदि MBEDTLS_संस्करणम्_सङ्ख्या >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->कुञ्चिका,
                                     conn_config->कुञ्चिका_passwd);
      यदि(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_कुञ्चिका_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_कुञ्चिका_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#अन्यथा
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->कुञ्चिका,
                                     conn_config->कुञ्चिका_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      यदि(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#यदि_अन्तः

      यदि(ret) {
        mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
        failf(दत्त, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->कुञ्चिका, (अचिह्नित पूर्णाङ्क)-ret, errorbuf);
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
#अन्यथा
      failf(दत्त, "mbedTLS: functions that use the file system not built in");
      प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
    }
    अन्यथा {
      ध्रुव संरचना curl_blob *ssl_कुञ्चिका_blob = conn_config->कुञ्चिका_blob;
      ध्रुव वर्ण *passwd = conn_config->कुञ्चिका_passwd;
      ध्रुव bool is_der = conn_config->कुञ्चिका_type &&
                          curl_strequal(conn_config->कुञ्चिका_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      अचिह्नित वर्ण *newblob = curlx_memdup0(ssl_कुञ्चिका_blob->दत्त,
                                             ssl_कुञ्चिका_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_कुञ्चिका_blob->len : ssl_कुञ्चिका_blob->len + 1;
      यदि(!newblob)
        प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

#यदि MBEDTLS_संस्करणम्_सङ्ख्या >= 0x04000000
      ret = mbedtls_pk_parse_कुञ्चिका(&backend->pk, newblob, keylen,
                                 (ध्रुव अचिह्नित वर्ण *)passwd,
                                 passwd ? वाक्यदैर्घ्यम्(passwd) : 0);
      यदि(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_कुञ्चिका_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_कुञ्चिका_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#अन्यथा
      ret = mbedtls_pk_parse_कुञ्चिका(&backend->pk, newblob, keylen,
                                 (ध्रुव अचिह्नित वर्ण *)passwd,
                                 passwd ? वाक्यदैर्घ्यम्(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      यदि(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#यदि_अन्तः
      curlx_free(newblob);

      यदि(ret) {
        mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
        failf(दत्त, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (अचिह्नित पूर्णाङ्क)-ret, errorbuf);
        प्रत्यागम CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mbed_आरोपय_crl(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ध्रुव वर्ण * ध्रुव ssl_crlfile = conn_config->CRLfile;

#यदि_परिभाषितम् MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  यदि(ssl_crlfile) {
    वर्ण errorbuf[128];
#यदि_परिभाषितम् MBEDTLS_FS_IO
    पूर्णाङ्क ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    यदि(ret) {
      mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
      failf(दत्त, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (अचिह्नित पूर्णाङ्क)-ret, errorbuf);

      प्रत्यागम CURLE_SSL_CRL_BADFILE;
    }
#अन्यथा
    (शून्य)errorbuf;
    failf(दत्त, "mbedTLS: functions that use the file system not built in");
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
#यदि_अन्तः
  }
#अन्यथा
  (शून्य)backend;
  यदि(ssl_crlfile) {
    failf(दत्त, "mbedTLS: CRL support not built in");
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
  }
#यदि_अन्तः

  प्रत्यागम CURLE_OK;
}

स्थिर bool mbed_apply_session(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना Curl_ssl_session *sc_session)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;

  यदि(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    पूर्णाङ्क ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_आरोपय(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    यदि(ret) {
      failf(दत्त, "SSL session error loading: -0x%x", (अचिह्नित पूर्णाङ्क)-ret);
    }
    अन्यथा {
      ret = mbedtls_ssl_समुच्चयः_session(&backend->ssl, &session);
      यदि(ret)
        failf(दत्त, "SSL session error setting: -0x%x", (अचिह्नित पूर्णाङ्क)-ret);
    }
    mbedtls_ssl_session_free(&session);
    यदि(!ret) {
      infof(दत्त, "SSL reusing session ID");
      प्रत्यागम TRUE;
    }
  }
  प्रत्यागम FALSE;
}

स्थिर CURLcode mbed_configure_ssl(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णाङ्क ret;
  bool session_applied = FALSE;
  CURLcode परिणाम;
  वर्ण errorbuf[128];

  infof(दत्त, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->पत्तनम्);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_प्रवाहः,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  यदि(ret) {
    failf(दत्त, "mbedTLS: ssl_config failed");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

#यदि_परिभाषितम् MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, दत्त);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_समुच्चयः_देहली(4);
#यदि_अन्तः

#यदि defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_संस्करणम्_सङ्ख्या >= 0x03060100 &&   \
  MBEDTLS_संस्करणम्_सङ्ख्या < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#यदि_अन्तः

  /* Always let mbedTLS verify certificates, if verifypeer or verifyhost are
   * disabled we clear the corresponding error flags in the verify callback
   * function. That is also where we log verification errors. */
  mbedtls_ssl_conf_verify(&backend->config, mbed_verify_cb, cf);
  mbedtls_ssl_conf_authmode(&backend->config, MBEDTLS_SSL_VERIFY_REQUIRED);

  mbedtls_ssl_init(&backend->ssl);
  backend->initialized = TRUE;

  /* use the default secure profile baked into mbedTLS */
  mbedtls_ssl_conf_cert_profile(&backend->config,
                                &mbedtls_x509_crt_profile_अग्र);

  परिणाम = mbed_समुच्चयः_ssl_संस्करणम्_min_max(दत्त, backend, conn_config);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

#यदि MBEDTLS_संस्करणम्_सङ्ख्या < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#यदि_अन्तः

  ret = mbedtls_ssl_सज्जता(&backend->ssl,
                          &backend->config);
  यदि(ret) {
    mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
    failf(दत्त, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (अचिह्नित पूर्णाङ्क)-ret, errorbuf);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_समुच्चयः_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      रिक्त /*  rev_timeout() */);

#यदि_अपरिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3
  यदि(conn_config->cipher_list) {
    परिणाम = mbed_समुच्चयः_selected_ciphers(दत्त, backend,
                                       conn_config->cipher_list, रिक्त);
#अन्यथा
  यदि(conn_config->cipher_list || conn_config->cipher_list13) {
    परिणाम = mbed_समुच्चयः_selected_ciphers(दत्त, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#यदि_अन्तः
    यदि(परिणाम) {
      failf(दत्त, "mbedTLS: failed to set cipher suites");
      प्रत्यागम परिणाम;
    }
  }
  अन्यथा {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#यदि_परिभाषितम् MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#यदि_अन्तः

#यदि_परिभाषितम् MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#यदि_अन्तः

  यदि((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    संरचना Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(दत्त, cf->cft, FIRSTSOCKET);
    यदि(scs) {
      यदि(mbed_apply_session(cf, दत्त, scs)) {
        CURL_TRC_CF(दत्त, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  यदि(!session_applied && Curl_ssl_scache_use(cf, दत्त)) {
    संरचना Curl_ssl_session *sc_session = रिक्त;
    CURLcode sresult = Curl_ssl_scache_take(cf, दत्त, connssl->peer.scache_कुञ्चिका,
                                            &sc_session);
    यदि(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_आरोपय(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      यदि(ret) {
        failf(दत्त, "SSL session error loading: -0x%x", (अचिह्नित पूर्णाङ्क)-ret);
      }
      अन्यथा {
        ret = mbedtls_ssl_समुच्चयः_session(&backend->ssl, &session);
        यदि(ret)
          failf(दत्त, "SSL session error setting: -0x%x", (अचिह्नित पूर्णाङ्क)-ret);
        अन्यथा
          infof(दत्त, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, दत्त, connssl->peer.scache_कुञ्चिका,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#यदि_परिभाषितम् MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#अन्यथा
                            रिक्त
#यदि_अन्तः
    );

  यदि(conn_config->कुञ्चिका || conn_config->कुञ्चिका_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  यदि(mbedtls_ssl_समुच्चयः_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(दत्त, "Failed to set SNI");
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

#यदि_परिभाषितम् HAS_ALPN_MBEDTLS
  यदि(connssl->alpn) {
    संरचना alpn_proto_buf proto;
    size_t i;

    प्रति(i = 0; i < connssl->alpn->गणनम्; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    यदि(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(दत्त, "Failed setting ALPN protocols");
      प्रत्यागम CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_गन्तव्यम्_proto_str(&proto, connssl->alpn);
    infof(दत्त, VTLS_INFOF_ALPN_OFFER_1STR, proto.दत्त);
  }
#यदि_अन्तः

  /* give application a chance to interfere with mbedTLS set up. */
  यदि(दत्त->समुच्चयः.ssl_fsslctx) {
    परिणाम = (*दत्त->समुच्चयः.ssl_fsslctx)(दत्त, &backend->config,
                                      दत्त->समुच्चयः.ssl_fsslctxp);
    यदि(परिणाम)
      failf(दत्त, "error signaled by SSL ctx callback");
  }

  प्रत्यागम परिणाम;
}

स्थिर CURLcode mbed_connect_step1(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम;

  यदि((conn_config->संस्करणम्_2 == CURL_SSLVERSION_SSLv2) ||
     (conn_config->संस्करणम्_2 == CURL_SSLVERSION_SSLv3)) {
    failf(दत्त, "Not supported SSL version");
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
  }

  परिणाम = mbed_आरोपय_cacert(cf, दत्त);
  यदि(!परिणाम)
    परिणाम = mbed_आरोपय_clicert(cf, दत्त);
  यदि(!परिणाम)
    परिणाम = mbed_आरोपय_privkey(cf, दत्त);
  यदि(!परिणाम)
    परिणाम = mbed_आरोपय_crl(cf, दत्त);
  यदि(!परिणाम)
    परिणाम = mbed_configure_ssl(cf, दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  connssl->connecting_state = ssl_connect_2;
  प्रत्यागम CURLE_OK;
}

#यदि defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#परिभाषय HAVE_PINNED_PUBKEY
#यदि_अन्तः

स्थिर CURLcode mbed_connect_step2(संरचना Curl_cfilter *cf,
                                   संरचना Curl_easy *दत्त)
{
#यदि defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode परिणाम;
#यदि_अन्तः
  पूर्णाङ्क ret;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
#यदि_परिभाषितम् HAVE_PINNED_PUBKEY
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
  ध्रुव वर्ण * ध्रुव pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY);
#अन्यथा
  ध्रुव वर्ण * ध्रुव pinnedpubkey =
    CURL_EASY_STR(दत्त, STRING_SSL_PINNEDPUBLICKEY);
#यदि_अन्तः
#यदि_अन्तः

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  यदि(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    प्रत्यागम CURLE_OK;
  }
  अन्यथा यदि(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    प्रत्यागम CURLE_OK;
  }
  अन्यथा यदि(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(दत्त, "peer certificate could not be verified");
    प्रत्यागम CURLE_PEER_FAILED_VERIFICATION;
  }
  अन्यथा यदि(ret) {
    वर्ण errorbuf[128];
    CURL_TRC_CF(दत्त, cf, "TLS version %04X",
                mbedtls_ssl_get_संस्करणम्_सङ्ख्या(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, परिमाण(errorbuf));
    failf(दत्त, "ssl_handshake returned: (-0x%04X) %s",
          (अचिह्नित पूर्णाङ्क)-ret, errorbuf);
    प्रत्यागम CURLE_SSL_CONNECT_ERROR;
  }

  {
    वर्ण cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, परिमाण(cipher_str), TRUE);
    infof(दत्त, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_संस्करणम्(&backend->ssl), cipher_str);
  }

#यदि_परिभाषितम् HAVE_PINNED_PUBKEY
  यदि(pinnedpubkey) {
    पूर्णाङ्क आकार;
    ध्रुव mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = रिक्त;
    अचिह्नित वर्ण *pubkey = रिक्त;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    यदि(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(दत्त, "Failed due to missing peer certificate");
      प्रत्यागम CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, परिमाण(*p));

    यदि(!p)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

    pubkey = curlx_malloc(PUB_DER_MAX_अष्टकानि);

    यदि(!pubkey) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    यदि(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(दत्त, "Failed copying peer certificate");
      परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      गच्छ pinnedpubkey_error;
    }

    आकार = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_अष्टकानि);

    यदि(आकार <= 0) {
      failf(दत्त, "Failed copying public key from peer certificate");
      परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      गच्छ pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    परिणाम = Curl_pin_peer_pubkey(दत्त,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_अष्टकानि - आकार], आकार);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
#यदि_अन्तः

#यदि_परिभाषितम् HAS_ALPN_MBEDTLS
  यदि(connssl->alpn) {
    ध्रुव वर्ण *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    परिणाम = Curl_alpn_समुच्चयः_negotiated(cf, दत्त, connssl,
                                      (ध्रुव अचिह्नित वर्ण *)proto,
                                      proto ? वाक्यदैर्घ्यम्(proto) : 0);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
#यदि_अन्तः

  connssl->connecting_state = ssl_connect_3;
  infof(दत्त, "SSL connected");

  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode mbed_new_session(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  संरचना Curl_ssl_session *sc_session = रिक्त;
  अचिह्नित वर्ण *sdata = रिक्त;
  size_t slen = 0;
  पूर्णाङ्क ietf_tls_id;
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क ret;

  DEBUGASSERT(backend);
  यदि(!Curl_ssl_scache_use(cf, दत्त))
    प्रत्यागम CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  यदि(ret) {
    failf(दत्त, "mbedtls_ssl_get_session returned -0x%x", (अचिह्नित पूर्णाङ्क)-ret);
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    गच्छ बहिर्गमनम्;
  }

  mbedtls_ssl_session_save(&session, रिक्त, 0, &slen);
  यदि(!slen) {
    failf(दत्त, "failed to serialize session: length is 0");
    गच्छ बहिर्गमनम्;
  }

  sdata = curlx_malloc(slen);
  यदि(!sdata) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  यदि(ret) {
    failf(दत्त, "failed to serialize session: -0x%x", (अचिह्नित पूर्णाङ्क)-ret);
    गच्छ बहिर्गमनम्;
  }

  ietf_tls_id = mbedtls_ssl_get_संस्करणम्_सङ्ख्या(&backend->ssl);
  परिणाम = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = रिक्त;  /* call took ownership */
  यदि(!परिणाम && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    परिणाम = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  यदि(!परिणाम) {
    परिणाम = Curl_ssl_scache_put(cf, दत्त, connssl->peer.scache_कुञ्चिका,
                                 sc_session);
    sc_session = रिक्त;
  }

बहिर्गमनम्:
  यदि(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mbed_send(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                          ध्रुव शून्य *mem, size_t len, size_t *pnwritten)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क nwritten;

  DEBUGASSERT(backend);
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#यदि_अन्तः
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  यदि(backend->send_blocked) {
    यदि(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(दत्त, "re-send data shorter than previously");
      परिणाम = CURLE_BAD_FUNCTION_तर्कः;
      गच्छ बहिर्गमनम्;
    }
    CURL_TRC_CF(दत्त, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (ध्रुव अचिह्नित वर्ण *)mem, len);

  यदि(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  अन्यथा {
    CURL_TRC_CF(दत्त, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (अचिह्नित पूर्णाङ्क)-nwritten);
    विकल्प(nwritten) {
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3
    पक्ष MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      परिणाम = mbed_new_session(cf, दत्त);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      यदि(!परिणाम)
        गच्छ do_send;
      भङ्ग;
#यदि_अन्तः
    पक्ष MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      परिणाम = CURLE_AGAIN;
      भङ्ग;
    पक्ष MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      परिणाम = CURLE_AGAIN;
      भङ्ग;
    स्वभाव:
      परिणाम = CURLE_SEND_ERROR;
      भङ्ग;
    }
    यदि((परिणाम == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (पूर्णाङ्क)परिणाम, *pnwritten);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode mbedtls_shutdown(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 bool send_shutdown, bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  अचिह्नित वर्ण buf[1024];
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  यदि(!backend->initialized || cf->shutdown) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *समाप्तम् = FALSE;

  यदि(!backend->प्रेषितम्_shutdown) {
    /* do this only once */
    backend->प्रेषितम्_shutdown = TRUE;
    यदि(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      विकल्प(ret) {
      पक्ष 0: /* we sent it, receive from the server */
        भङ्ग;
      पक्ष MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *समाप्तम् = TRUE;
        गच्छ बहिर्गमनम्;
      पक्ष MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        गच्छ बहिर्गमनम्;
      पक्ष MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        गच्छ बहिर्गमनम्;
      स्वभाव:
        CURL_TRC_CF(दत्त, cf, "mbedtls_shutdown error -0x%04X",
                    (अचिह्नित पूर्णाङ्क)-ret);
        परिणाम = CURLE_RECV_ERROR;
        गच्छ बहिर्गमनम्;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  प्रति(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, परिमाण(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    यदि(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, परिमाण(buf));
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3
    यदि(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      अनुवर्त;
#यदि_अन्तः
    यदि(ret <= 0)
      भङ्ग;
  }

  यदि(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(दत्त, cf, "mbedtls_shutdown, still getting data");
  }
  अन्यथा यदि(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(दत्त, cf, "mbedtls_shutdown done");
    *समाप्तम् = TRUE;
  }
  अन्यथा यदि(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(दत्त, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  अन्यथा यदि(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(दत्त, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  अन्यथा {
    CURL_TRC_CF(दत्त, cf, "mbedtls_shutdown error -0x%04X",
                (अचिह्नित पूर्णाङ्क)-ret);
    परिणाम = CURLE_RECV_ERROR;
  }

बहिर्गमनम्:
  cf->shutdown = (परिणाम || *समाप्तम्);
  प्रत्यागम परिणाम;
}

स्थिर शून्य mbedtls_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;

  (शून्य)दत्त;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#यदि_परिभाषितम् MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#यदि_अन्तः
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  यदि(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

स्थिर CURLcode mbed_recv(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                          वर्ण *buf, size_t buffersize, size_t *pnread)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  पूर्णाङ्क nread;

  DEBUGASSERT(backend);
#यदि_परिभाषितम् MBEDTLS_SSL_SESSION_TICKETS
do_read:
#यदि_अन्तः
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (अचिह्नित वर्ण *)buf, buffersize);
  यदि(nread > 0)
    *pnread = (size_t)nread;
  अन्यथा {
    वर्ण errorbuf[128];
    CURL_TRC_CF(दत्त, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (अचिह्नित पूर्णाङ्क)-nread);
    विकल्प(nread) {
#यदि_परिभाषितम् MBEDTLS_SSL_SESSION_TICKETS
    पक्ष MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      परिणाम = mbed_new_session(cf, दत्त);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      यदि(!परिणाम)
        गच्छ do_read;
      भङ्ग;
#यदि_अन्तः
    पक्ष MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      परिणाम = CURLE_AGAIN;
      भङ्ग;
    पक्ष MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      परिणाम = CURLE_AGAIN;
      भङ्ग;
    पक्ष MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      परिणाम = CURLE_OK;
      भङ्ग;
    स्वभाव:
      mbedtls_strerror(nread, errorbuf, परिमाण(errorbuf));
      failf(दत्त, "ssl_read returned: (-0x%04X) %s", (अचिह्नित पूर्णाङ्क)-nread,
            errorbuf);
      परिणाम = CURLE_RECV_ERROR;
      भङ्ग;
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर size_t mbedtls_संस्करणम्(वर्ण *बफर, size_t आकार)
{
#यदि_परिभाषितम् MBEDTLS_संस्करणम्_C
  /* if mbedtls_version_get_number() is available it is better */
  अचिह्नित पूर्णाङ्क संस्करणम्_2 = mbedtls_संस्करणम्_get_सङ्ख्या();
  प्रत्यागम curl_msnprintf(बफर, आकार, "mbedTLS/%u.%u.%u", संस्करणम्_2 >> 24,
                        (संस्करणम्_2 >> 16) & 0xff, (संस्करणम्_2 >> 8) & 0xff);
#अन्यथा
  प्रत्यागम curl_msnprintf(बफर, आकार, "mbedTLS/%s", MBEDTLS_संस्करणम्_STRING);
#यदि_अन्तः
}

/* 'data' might be NULL */
स्थिर CURLcode mbedtls_random(संरचना Curl_easy *दत्त,
                               अचिह्नित वर्ण *entropy, size_t दीर्घता)
{
  psa_स्थितिः_t स्थितिः;
  (शून्य)दत्त;

  स्थितिः = psa_generate_random(entropy, दीर्घता);

  प्रत्यागम स्थितिः == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

स्थिर CURLcode mbedtls_connect(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                bool *समाप्तम्)
{
  CURLcode परिणाम;
  संरचना ssl_connect_दत्त *connssl = cf->ctx;

  /* check if the connection has already been established */
  यदि(ssl_connection_complete == connssl->state) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  *समाप्तम् = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  यदि(connssl->connecting_state == ssl_connect_1) {
    परिणाम = mbed_connect_step1(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(connssl->connecting_state == ssl_connect_2) {
    परिणाम = mbed_connect_step2(cf, दत्त);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }

  यदि(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    संरचना ssl_connect_दत्त *ctx = cf->ctx;
    संरचना mbed_ssl_backend_दत्त *backend =
      (संरचना mbed_ssl_backend_दत्त *)ctx->backend;

    यदि(mbedtls_ssl_get_संस्करणम्_सङ्ख्या(&backend->ssl) <=
       MBEDTLS_SSL_संस्करणम्_TLS1_2) {
      परिणाम = mbed_new_session(cf, दत्त);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    connssl->connecting_state = ssl_connect_समाप्तम्;
  }

  यदि(connssl->connecting_state == ssl_connect_समाप्तम्) {
    connssl->state = ssl_connection_complete;
    *समाप्तम् = TRUE;
  }

  प्रत्यागम CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
स्थिर पूर्णाङ्क mbedtls_init(शून्य)
{
#यदि MBEDTLS_संस्करणम्_सङ्ख्या < 0x04000000
  पूर्णाङ्क ret = 0;
#यदि_अन्तः
  psa_स्थितिः_t स्थितिः;
  स्थितिः = psa_crypto_init();

  यदि(स्थितिः != PSA_SUCCESS)
    प्रत्यागम 0;

#यदि MBEDTLS_संस्करणम्_सङ्ख्या < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_बीजम्(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              रिक्त, 0);

  यदि(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    प्रत्यागम 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_समुच्चयः_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#यदि_अन्तः
  प्रत्यागम 1;
}

स्थिर शून्य mbedtls_cleanup(शून्य)
{
  mbedtls_psa_crypto_free();

#यदि MBEDTLS_संस्करणम्_सङ्ख्या < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#यदि_अन्तः
}

स्थिर bool mbedtls_दत्त_pending(संरचना Curl_cfilter *cf,
                                 ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *ctx = cf->ctx;
  संरचना mbed_ssl_backend_दत्त *backend;

  (शून्य)दत्त;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (संरचना mbed_ssl_backend_दत्त *)ctx->backend;
  प्रत्यागम mbedtls_ssl_get_अष्टकानि_avail(&backend->ssl) != 0;
}

स्थिर CURLcode mbedtls_sha256sum(ध्रुव अचिह्नित वर्ण *निवेशः,
                                  size_t inputlen,
                                  अचिह्नित वर्ण *sha256sum,
                                  size_t sha256len)
{
#यदि defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_स्थितिः_t स्थितिः;
  size_t sha256len_actual;
  स्थितिः = psa_hash_compute(PSA_ALG_SHA_256, निवेशः, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  यदि(स्थितिः != PSA_SUCCESS)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  प्रत्यागम CURLE_OK;
#अन्यथा
  (शून्य)sha256len;
  प्रत्यागम Curl_sha256it(sha256sum, निवेशः, inputlen);
#यदि_अन्तः
}

स्थिर शून्य *mbedtls_get_internals(संरचना ssl_connect_दत्त *connssl,
                                   CURLINFO info)
{
  संरचना mbed_ssl_backend_दत्त *backend =
    (संरचना mbed_ssl_backend_दत्त *)connssl->backend;
  (शून्य)info;
  DEBUGASSERT(backend);
  प्रत्यागम &backend->ssl;
}

ध्रुव संरचना Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_मार्गः |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#यदि_परिभाषितम् HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#यदि_अन्तः
  SSLSUPP_SSL_CTX |
#यदि_परिभाषितम् MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#यदि_अन्तः
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#यदि_परिभाषितम् MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#यदि_अन्तः
  0,

  परिमाण(संरचना mbed_ssl_backend_दत्त),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_संस्करणम्,                  /*संस्करणम्*/
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_दत्त_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  रिक्त,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  रिक्त,                             /* close_all */
  रिक्त,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  रिक्त,                             /* get_channel_binding */
};

#यदि_अन्तः /* USE_MBEDTLS */
