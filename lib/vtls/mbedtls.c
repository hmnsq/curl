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
#समावेश "curl_setup.h"

#अगर_घोषित USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#समावेश <mbedtls/version.h>
#अगर MBEDTLS_VERSION_संख्या < 0x03020000
#त्रुटि "mbedTLS 3.2.0 or greater required"
#पूर्ण_अगर
#समावेश <psa/crypto_config.h>
#समावेश <mbedtls/net_sockets.h>
#समावेश <mbedtls/ssl.h>
#समावेश <mbedtls/x509.h>
#समावेश <mbedtls/psa_util.h>

#अगर MBEDTLS_VERSION_संख्या < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#त्रुटि "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#पूर्ण_अगर

#समावेश <mbedtls/error.h>
#अगर MBEDTLS_VERSION_संख्या < 0x04000000
#समावेश <mbedtls/entropy.h>
#समावेश <mbedtls/ctr_drbg.h>
#पूर्ण_अगर
#अगर_घोषित MBEDTLS_DEBUG
#समावेश <mbedtls/debug.h>
#पूर्ण_अगर

#समावेश "vtls/cipher_suite.h"
#समावेश "urldata.h"
#समावेश "curl_trc.h"
#समावेश "vtls/mbedtls.h"
#समावेश "vtls/vtls.h"
#समावेश "vtls/vtls_int.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "vtls/x509asn1.h"
#समावेश "connect.h" /* for the connect timeout */
#समावेश "curlx/strdup.h"
#समावेश "curl_sha256.h"

/* ALPN for http2 */
#अगर defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#घोषणा HAS_ALPN_MBEDTLS
#पूर्ण_अगर

काष्ठा mbed_ssl_backend_तथ्य {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#अगर_घोषित MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#पूर्ण_अगर
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#अगर_घोषित HAS_ALPN_MBEDTLS
  स्थिर अक्षर *protocols[3];
#पूर्ण_अगर
  पूर्णांक *ciphersuites;
  माप_प्रकार send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#अगर MBEDTLS_VERSION_संख्या < 0x04000000
काष्ठा rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

अटल काष्ठा rng_context_t rng;
#पूर्ण_अगर

#अगर_अघोषित MBEDTLS_ERROR_C
#घोषणा mbedtls_strerror(a, b, c) b[0] = 0
#पूर्ण_अगर

#अगर_घोषित MBEDTLS_DEBUG
अटल व्योम mbed_debug(व्योम *context, पूर्णांक level, स्थिर अक्षर *f_name,
                       पूर्णांक पंक्ति_nb, स्थिर अक्षर *पंक्ति)
{
  काष्ठा Curl_easy *तथ्य = (काष्ठा Curl_easy *)context;
  (व्योम)level;
  (व्योम)पंक्ति_nb;
  (व्योम)f_name;

  अगर(तथ्य) {
    माप_प्रकार len = म_माप(पंक्ति);
    अगर(len && (पंक्ति[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(तथ्य, "%.*s", (पूर्णांक)len, पंक्ति);
  }
}
#पूर्ण_अगर

अटल पूर्णांक mbedtls_bio_cf_write(व्योम *bio,
                                स्थिर अचिन्हित अक्षर *buf, माप_प्रकार blen)
{
  काष्ठा Curl_cfilter *cf = bio;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य);
  अगर(!तथ्य)
    वापस 0;

  परिणाम = Curl_conn_cf_send(cf->अग्र, तथ्य, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(तथ्य, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (पूर्णांक)परिणाम, nwritten);
  अगर(परिणाम == CURLE_AGAIN)
    वापस MBEDTLS_ERR_SSL_WANT_WRITE;
  वापस परिणाम ? -1 : (पूर्णांक)nwritten;
}

अटल पूर्णांक mbedtls_bio_cf_read(व्योम *bio, अचिन्हित अक्षर *buf, माप_प्रकार blen)
{
  काष्ठा Curl_cfilter *cf = bio;
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);
  माप_प्रकार nread = 0;
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य);
  अगर(!तथ्य)
    वापस 0;
  /* OpenSSL catches this case, so should we. */
  अगर(!buf)
    वापस 0;

  परिणाम = Curl_conn_cf_recv(cf->अग्र, तथ्य, (अक्षर *)buf, blen, &nread);
  CURL_TRC_CF(तथ्य, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (पूर्णांक)परिणाम, nread);
  अगर(परिणाम == CURLE_AGAIN)
    वापस MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  वापस परिणाम ? -1 : (पूर्णांक)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#घोषणा RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_आकार))
#घोषणा ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#घोषणा PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

अटल CURLcode mbed_set_ssl_version_min_max(
  काष्ठा Curl_easy *तथ्य,
  काष्ठा mbed_ssl_backend_तथ्य *backend,
  काष्ठा ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#अन्यथा
    MBEDTLS_SSL_VERSION_TLS1_3
#पूर्ण_अगर
    ;
  mbedtls_ssl_protocol_version ver_max =
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#अन्यथा
    MBEDTLS_SSL_VERSION_TLS1_2
#पूर्ण_अगर
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  चयन(conn_config->version) {
  हाल CURL_SSLVERSION_TLSv1:
  हाल CURL_SSLVERSION_TLSv1_0:
  हाल CURL_SSLVERSION_TLSv1_1:
  हाल CURL_SSLVERSION_TLSv1_2:
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    अवरोध;
#पूर्ण_अगर
  हाल CURL_SSLVERSION_TLSv1_3:
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    अवरोध;
#पूर्ण_अगर
  शेष:
    failf(तथ्य, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  चयन(conn_config->version_max) {
  हाल CURL_SSLVERSION_MAX_DEFAULT:
  हाल CURL_SSLVERSION_MAX_NONE:
  हाल CURL_SSLVERSION_MAX_TLSv1_3:
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    अवरोध;
#पूर्ण_अगर
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_2
  हाल CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    अवरोध;
#पूर्ण_अगर
  हाल CURL_SSLVERSION_MAX_TLSv1_1:
  हाल CURL_SSLVERSION_MAX_TLSv1_0:
  शेष:
    failf(तथ्य, "mbedTLS: unsupported maximum TLS version value");
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  वापस CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#अगर_घोषित MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
अटल पूर्णांक mbed_cipher_suite_get_str(uint16_t id, अक्षर *buf, माप_प्रकार buf_आकार,
                                     bool prefer_rfc)
{
  अगर(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_आकार, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  अन्यथा
    वापस Curl_cipher_suite_get_str(id, buf, buf_आकार, prefer_rfc);
  वापस 0;
}

अटल uint16_t mbed_cipher_suite_walk_str(स्थिर अक्षर **str, स्थिर अक्षर **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  माप_प्रकार len = *end - *str;
  अटल स्थिर अक्षर ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  अगर(!id) {
    अगर((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  वापस id;
}
#अन्यथा
#घोषणा mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#घोषणा mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#पूर्ण_अगर

अटल CURLcode mbed_set_selected_ciphers(
  काष्ठा Curl_easy *तथ्य,
  काष्ठा mbed_ssl_backend_तथ्य *backend,
  स्थिर अक्षर *ciphers12,
  स्थिर अक्षर *ciphers13)
{
  स्थिर अक्षर *ciphers = ciphers12;
  स्थिर पूर्णांक *supported;
  पूर्णांक *selected;
  माप_प्रकार supported_len, गिनती = 0, default13_गिनती = 0, i, j;
  स्थिर अक्षर *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  क्रम(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(माप(पूर्णांक) * (supported_len + 1));
  अगर(!selected)
    वापस CURLE_OUT_OF_MEMORY;

#अगर_अघोषित MBEDTLS_SSL_PROTO_TLS1_3
  (व्योम)ciphers13, (व्योम)j;
#अन्यथा
  अगर(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    क्रम(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      अगर(म_भेदन(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        जारी;

      selected[गिनती++] = id;
    }

    default13_गिनती = गिनती;
  }
  अन्यथा
    ciphers = ciphers13;

add_ciphers:
#पूर्ण_अगर
  क्रम(ptr = ciphers; ptr[0] != '\0' && गिनती < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    अगर(id) {
      क्रम(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      अगर(i == supported_len)
        id = 0;
    }
    अगर(!id) {
      अगर(ptr[0] != '\0')
        infof(तथ्य, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (पूर्णांक)(end - ptr), ptr);
      जारी;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    क्रम(i = 0; i < गिनती && selected[i] != id; i++)
      ;
    अगर(i < गिनती) {
      अगर(i >= default13_गिनती)
        infof(तथ्य, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (पूर्णांक)(end - ptr), ptr);
      जारी;
    }

    selected[गिनती++] = id;
  }

#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_3
  अगर(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    जाओ add_ciphers;
  }

  अगर(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    क्रम(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      अगर(!म_भेदन(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        जारी;

      /* No duplicates allowed (so selected cannot overflow) */
      क्रम(i = 0; i < गिनती && selected[i] != id; i++)
        ;
      अगर(i < गिनती)
        जारी;

      selected[गिनती++] = id;
    }
  }
#पूर्ण_अगर

  selected[गिनती] = 0;

  अगर(गिनती == 0) {
    curlx_free(selected);
    failf(तथ्य, "mbedTLS: no supported cipher in list");
    वापस CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  वापस CURLE_OK;
}

अटल व्योम mbed_dump_cert_info(काष्ठा Curl_easy *तथ्य,
                                स्थिर mbedtls_x509_crt *crt)
{
#अगर !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (व्योम)तथ्य, (व्योम)crt;
#अन्यथा
  स्थिर माप_प्रकार bufsize = 16384;
  अक्षर *p, *बफर = curlx_malloc(bufsize);

  अगर(बफर && mbedtls_x509_crt_info(बफर, bufsize, " ", crt) > 0) {
    infof(तथ्य, "Server certificate:");
    क्रम(p = बफर; *p; p += *p != '\0') {
      माप_प्रकार s = म_खोज(p, "\n");
      infof(तथ्य, "%.*s", (पूर्णांक)s, p);
      p += s;
    }
  }
  अन्यथा
    infof(तथ्य, "Unable to dump certificate information");

  curlx_free(बफर);
#पूर्ण_अगर
}

अटल व्योम mbed_extract_certinfo(काष्ठा Curl_easy *तथ्य,
                                  स्थिर mbedtls_x509_crt *crt)
{
  CURLcode परिणाम;
  स्थिर mbedtls_x509_crt *cur;
  पूर्णांक cert_गिनती = 0;
  पूर्णांक i;

  क्रम(cur = crt; cur && cert_गिनती <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->अग्र)
    cert_गिनती++;

  अगर(cert_गिनती > MAX_ALLOWED_CERT_AMOUNT) {
    infof(तथ्य, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    वापस;
  }

  परिणाम = Curl_ssl_init_certinfo(तथ्य, cert_गिनती);

  क्रम(i = 0, cur = crt; परिणाम == CURLE_OK && cur; ++i, cur = cur->अग्र) {
    परिणाम = Curl_extract_certinfo(तथ्य, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

अटल पूर्णांक mbed_verify_cb(व्योम *ptr, mbedtls_x509_crt *crt,
                          पूर्णांक depth, uint32_t *flags)
{
  काष्ठा Curl_cfilter *cf = (काष्ठा Curl_cfilter *)ptr;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा Curl_easy *तथ्य = CF_तथ्य_CURRENT(cf);

  अगर(depth == 0) {
    अगर(तथ्य->set.verbose)
      mbed_dump_cert_info(तथ्य, crt);
    अगर(तथ्य->set.ssl.certinfo)
      mbed_extract_certinfo(तथ्य, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  अगर(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  अगर(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  अगर(*flags) {
#अगर_अघोषित MBEDTLS_X509_REMOVE_INFO
    अक्षर buf[128];
    mbedtls_x509_crt_verify_info(buf, माप(buf), "", *flags);
    failf(तथ्य, "mbedTLS: %s", buf);
#अन्यथा
    failf(तथ्य, "mbedTLS: certificate verification error 0x%08x",
          (अचिन्हित पूर्णांक)*flags);
#पूर्ण_अगर
  }

  वापस 0;
}

अटल CURLcode mbed_load_cacert(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  स्थिर काष्ठा curl_blob *ca_info_blob = conn_config->ca_info_blob;
  स्थिर अक्षर * स्थिर ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? शून्य : conn_config->CAfile);
  स्थिर bool verifypeer = conn_config->verifypeer;
  स्थिर अक्षर * स्थिर ssl_capath = conn_config->CApath;
#अगर_घोषित MBEDTLS_PEM_PARSE_C
  स्थिर अक्षर * स्थिर ssl_cert_type = conn_config->cert_type;
#पूर्ण_अगर
  पूर्णांक ret = -1;
  अक्षर errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  अगर(ca_info_blob && verifypeer) {
#अगर_घोषित MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    अगर((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((अक्षर *)(ca_info_blob->तथ्य))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->तथ्य,
                                   ca_info_blob->len);
    }
    अन्यथा { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      अचिन्हित अक्षर *newblob = curlx_memdup0(ca_info_blob->तथ्य,
                                             ca_info_blob->len);
      अगर(!newblob)
        वापस CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#अन्यथा
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->तथ्य,
                                     ca_info_blob->len);
#पूर्ण_अगर

    अगर(ret < 0) {
      mbedtls_strerror(ret, errorbuf, माप(errorbuf));
      failf(तथ्य, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (अचिन्हित पूर्णांक)-ret, errorbuf);
      वापस CURLE_SSL_CERTPROBLEM;
    }
  }

  अगर(ssl_cafile && verifypeer) {
#अगर_घोषित MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    अगर(ret < 0) {
      mbedtls_strerror(ret, errorbuf, माप(errorbuf));
      failf(तथ्य, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (अचिन्हित पूर्णांक)-ret, errorbuf);
      वापस CURLE_SSL_CACERT_BADFILE;
    }
#अन्यथा
    failf(तथ्य, "mbedTLS: functions that use the file system not built in");
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  }

  अगर(ssl_capath) {
#अगर_घोषित MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    अगर(ret < 0) {
      mbedtls_strerror(ret, errorbuf, माप(errorbuf));
      failf(तथ्य, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (अचिन्हित पूर्णांक)-ret, errorbuf);

      अगर(verifypeer)
        वापस CURLE_SSL_CACERT_BADFILE;
    }
#अन्यथा
    failf(तथ्य, "mbedTLS: functions that use the file system not built in");
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  }

  वापस CURLE_OK;
}

अटल CURLcode mbed_load_clicert(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  अक्षर * स्थिर ssl_cert = conn_config->clientcert;
  स्थिर काष्ठा curl_blob *ssl_cert_blob = conn_config->cert_blob;
#अगर_घोषित MBEDTLS_PEM_PARSE_C
  स्थिर अक्षर * स्थिर ssl_cert_type = conn_config->cert_type;
#पूर्ण_अगर
  पूर्णांक ret = -1;
  अक्षर errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  अगर(ssl_cert) {
#अगर_घोषित MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    अगर(ret) {
      mbedtls_strerror(ret, errorbuf, माप(errorbuf));
      failf(तथ्य, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (अचिन्हित पूर्णांक)-ret, errorbuf);

      वापस CURLE_SSL_CERTPROBLEM;
    }
#अन्यथा
    failf(तथ्य, "mbedTLS: functions that use the file system not built in");
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  }

  अगर(ssl_cert_blob) {
#अगर_घोषित MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    अगर((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((अक्षर *)(ssl_cert_blob->तथ्य))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->तथ्य,
                                   ssl_cert_blob->len);
    }
    अन्यथा { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      अचिन्हित अक्षर *newblob = curlx_memdup0(ssl_cert_blob->तथ्य,
                                             ssl_cert_blob->len);
      अगर(!newblob)
        वापस CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#अन्यथा
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->तथ्य,
                                     ssl_cert_blob->len);
#पूर्ण_अगर

    अगर(ret) {
      mbedtls_strerror(ret, errorbuf, माप(errorbuf));
      failf(तथ्य, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (अचिन्हित पूर्णांक)-ret, errorbuf);
      वापस CURLE_SSL_CERTPROBLEM;
    }
  }

  वापस CURLE_OK;
}

अटल CURLcode mbed_load_privkey(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णांक ret = -1;
  अक्षर errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  अगर(conn_config->कुंजी || conn_config->कुंजी_blob) {
    अगर(conn_config->कुंजी) {
#अगर_घोषित MBEDTLS_FS_IO
#अगर MBEDTLS_VERSION_संख्या >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->कुंजी,
                                     conn_config->कुंजी_passwd);
      अगर(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_कुंजी_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_कुंजी_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#अन्यथा
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->कुंजी,
                                     conn_config->कुंजी_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      अगर(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#पूर्ण_अगर

      अगर(ret) {
        mbedtls_strerror(ret, errorbuf, माप(errorbuf));
        failf(तथ्य, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->कुंजी, (अचिन्हित पूर्णांक)-ret, errorbuf);
        वापस CURLE_SSL_CERTPROBLEM;
      }
#अन्यथा
      failf(तथ्य, "mbedTLS: functions that use the file system not built in");
      वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
    }
    अन्यथा {
      स्थिर काष्ठा curl_blob *ssl_कुंजी_blob = conn_config->कुंजी_blob;
      स्थिर अक्षर *passwd = conn_config->कुंजी_passwd;
      स्थिर bool is_der = conn_config->कुंजी_type &&
                          curl_strequal(conn_config->कुंजी_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      अचिन्हित अक्षर *newblob = curlx_memdup0(ssl_कुंजी_blob->तथ्य,
                                             ssl_कुंजी_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      माप_प्रकार keylen = is_der ? ssl_कुंजी_blob->len : ssl_कुंजी_blob->len + 1;
      अगर(!newblob)
        वापस CURLE_OUT_OF_MEMORY;

#अगर MBEDTLS_VERSION_संख्या >= 0x04000000
      ret = mbedtls_pk_parse_कुंजी(&backend->pk, newblob, keylen,
                                 (स्थिर अचिन्हित अक्षर *)passwd,
                                 passwd ? म_माप(passwd) : 0);
      अगर(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_कुंजी_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_कुंजी_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#अन्यथा
      ret = mbedtls_pk_parse_कुंजी(&backend->pk, newblob, keylen,
                                 (स्थिर अचिन्हित अक्षर *)passwd,
                                 passwd ? म_माप(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      अगर(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#पूर्ण_अगर
      curlx_free(newblob);

      अगर(ret) {
        mbedtls_strerror(ret, errorbuf, माप(errorbuf));
        failf(तथ्य, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (अचिन्हित पूर्णांक)-ret, errorbuf);
        वापस CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  वापस CURLE_OK;
}

अटल CURLcode mbed_load_crl(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  स्थिर अक्षर * स्थिर ssl_crlfile = conn_config->CRLfile;

#अगर_घोषित MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  अगर(ssl_crlfile) {
    अक्षर errorbuf[128];
#अगर_घोषित MBEDTLS_FS_IO
    पूर्णांक ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    अगर(ret) {
      mbedtls_strerror(ret, errorbuf, माप(errorbuf));
      failf(तथ्य, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (अचिन्हित पूर्णांक)-ret, errorbuf);

      वापस CURLE_SSL_CRL_BADFILE;
    }
#अन्यथा
    (व्योम)errorbuf;
    failf(तथ्य, "mbedTLS: functions that use the file system not built in");
    वापस CURLE_NOT_BUILT_IN;
#पूर्ण_अगर
  }
#अन्यथा
  (व्योम)backend;
  अगर(ssl_crlfile) {
    failf(तथ्य, "mbedTLS: CRL support not built in");
    वापस CURLE_NOT_BUILT_IN;
  }
#पूर्ण_अगर

  वापस CURLE_OK;
}

अटल bool mbed_apply_session(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा Curl_ssl_session *sc_session)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;

  अगर(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    पूर्णांक ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    अगर(ret) {
      failf(तथ्य, "SSL session error loading: -0x%x", (अचिन्हित पूर्णांक)-ret);
    }
    अन्यथा {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      अगर(ret)
        failf(तथ्य, "SSL session error setting: -0x%x", (अचिन्हित पूर्णांक)-ret);
    }
    mbedtls_ssl_session_free(&session);
    अगर(!ret) {
      infof(तथ्य, "SSL reusing session ID");
      वापस TRUE;
    }
  }
  वापस FALSE;
}

अटल CURLcode mbed_configure_ssl(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  पूर्णांक ret;
  bool session_applied = FALSE;
  CURLcode परिणाम;
  अक्षर errorbuf[128];

  infof(तथ्य, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  अगर(ret) {
    failf(तथ्य, "mbedTLS: ssl_config failed");
    वापस CURLE_SSL_CONNECT_ERROR;
  }

#अगर_घोषित MBEDTLS_DEBUG
  /* In order to make that work in mbedtls MBEDTLS_DEBUG_C must be defined. */
  mbedtls_ssl_conf_dbg(&backend->config, mbed_debug, तथ्य);
  /* - 0 No debug
   * - 1 Error
   * - 2 State change
   * - 3 Informational
   * - 4 Verbose
   */
  mbedtls_debug_set_threshold(4);
#पूर्ण_अगर

#अगर defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_संख्या >= 0x03060100 &&   \
  MBEDTLS_VERSION_संख्या < 0x04000000
  /* New in mbedTLS 3.6.1, need to enable, default is now disabled. 4.0.0
     enabled it by default for TLSv1.3. */
  mbedtls_ssl_conf_tls13_enable_signal_new_session_tickets(
    &backend->config, MBEDTLS_SSL_TLS1_3_SIGNAL_NEW_SESSION_TICKETS_ENABLED);
#पूर्ण_अगर

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

  परिणाम = mbed_set_ssl_version_min_max(तथ्य, backend, conn_config);
  अगर(परिणाम)
    वापस परिणाम;

#अगर MBEDTLS_VERSION_संख्या < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#पूर्ण_अगर

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  अगर(ret) {
    mbedtls_strerror(ret, errorbuf, माप(errorbuf));
    failf(तथ्य, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (अचिन्हित पूर्णांक)-ret, errorbuf);
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      शून्य /*  rev_timeout() */);

#अगर_अघोषित MBEDTLS_SSL_PROTO_TLS1_3
  अगर(conn_config->cipher_list) {
    परिणाम = mbed_set_selected_ciphers(तथ्य, backend,
                                       conn_config->cipher_list, शून्य);
#अन्यथा
  अगर(conn_config->cipher_list || conn_config->cipher_list13) {
    परिणाम = mbed_set_selected_ciphers(तथ्य, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#पूर्ण_अगर
    अगर(परिणाम) {
      failf(तथ्य, "mbedTLS: failed to set cipher suites");
      वापस परिणाम;
    }
  }
  अन्यथा {
    mbedtls_ssl_conf_ciphersuites(&backend->config,
                                  mbedtls_ssl_list_ciphersuites());
  }

#अगर_घोषित MBEDTLS_SSL_RENEGOTIATION
  mbedtls_ssl_conf_renegotiation(&backend->config,
                                 MBEDTLS_SSL_RENEGOTIATION_ENABLED);
#पूर्ण_अगर

#अगर_घोषित MBEDTLS_SSL_SESSION_TICKETS
  mbedtls_ssl_conf_session_tickets(&backend->config,
                                   MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#पूर्ण_अगर

  अगर((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    काष्ठा Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(तथ्य, cf->cft, FIRSTSOCKET);
    अगर(scs) {
      अगर(mbed_apply_session(cf, तथ्य, scs)) {
        CURL_TRC_CF(तथ्य, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  अगर(!session_applied && Curl_ssl_scache_use(cf, तथ्य)) {
    काष्ठा Curl_ssl_session *sc_session = शून्य;
    CURLcode sresult = Curl_ssl_scache_take(cf, तथ्य, connssl->peer.scache_कुंजी,
                                            &sc_session);
    अगर(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      अगर(ret) {
        failf(तथ्य, "SSL session error loading: -0x%x", (अचिन्हित पूर्णांक)-ret);
      }
      अन्यथा {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        अगर(ret)
          failf(तथ्य, "SSL session error setting: -0x%x", (अचिन्हित पूर्णांक)-ret);
        अन्यथा
          infof(तथ्य, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, तथ्य, connssl->peer.scache_कुंजी,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#अगर_घोषित MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#अन्यथा
                            शून्य
#पूर्ण_अगर
    );

  अगर(conn_config->कुंजी || conn_config->कुंजी_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  अगर(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(तथ्य, "Failed to set SNI");
    वापस CURLE_SSL_CONNECT_ERROR;
  }

#अगर_घोषित HAS_ALPN_MBEDTLS
  अगर(connssl->alpn) {
    काष्ठा alpn_proto_buf proto;
    माप_प्रकार i;

    क्रम(i = 0; i < connssl->alpn->गिनती; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    अगर(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(तथ्य, "Failed setting ALPN protocols");
      वापस CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(तथ्य, VTLS_INFOF_ALPN_OFFER_1STR, proto.तथ्य);
  }
#पूर्ण_अगर

  /* give application a chance to interfere with mbedTLS set up. */
  अगर(तथ्य->set.ssl_fsslctx) {
    परिणाम = (*तथ्य->set.ssl_fsslctx)(तथ्य, &backend->config,
                                      तथ्य->set.ssl_fsslctxp);
    अगर(परिणाम)
      failf(तथ्य, "error signaled by SSL ctx callback");
  }

  वापस परिणाम;
}

अटल CURLcode mbed_connect_step1(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम;

  अगर((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(तथ्य, "Not supported SSL version");
    वापस CURLE_NOT_BUILT_IN;
  }

  परिणाम = mbed_load_cacert(cf, तथ्य);
  अगर(!परिणाम)
    परिणाम = mbed_load_clicert(cf, तथ्य);
  अगर(!परिणाम)
    परिणाम = mbed_load_privkey(cf, तथ्य);
  अगर(!परिणाम)
    परिणाम = mbed_load_crl(cf, तथ्य);
  अगर(!परिणाम)
    परिणाम = mbed_configure_ssl(cf, तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  connssl->connecting_state = ssl_connect_2;
  वापस CURLE_OK;
}

#अगर defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#घोषणा HAVE_PINNED_PUBKEY
#पूर्ण_अगर

अटल CURLcode mbed_connect_step2(काष्ठा Curl_cfilter *cf,
                                   काष्ठा Curl_easy *तथ्य)
{
#अगर defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode परिणाम;
#पूर्ण_अगर
  पूर्णांक ret;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
#अगर_घोषित HAVE_PINNED_PUBKEY
#अगर_अघोषित CURL_DISABLE_PROXY
  स्थिर अक्षर * स्थिर pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY);
#अन्यथा
  स्थिर अक्षर * स्थिर pinnedpubkey =
    CURL_EASY_STR(तथ्य, STRING_SSL_PINNEDPUBLICKEY);
#पूर्ण_अगर
#पूर्ण_अगर

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  अगर(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    वापस CURLE_OK;
  }
  अन्यथा अगर(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    वापस CURLE_OK;
  }
  अन्यथा अगर(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(तथ्य, "peer certificate could not be verified");
    वापस CURLE_PEER_FAILED_VERIFICATION;
  }
  अन्यथा अगर(ret) {
    अक्षर errorbuf[128];
    CURL_TRC_CF(तथ्य, cf, "TLS version %04X",
                mbedtls_ssl_get_version_संख्या(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, माप(errorbuf));
    failf(तथ्य, "ssl_handshake returned: (-0x%04X) %s",
          (अचिन्हित पूर्णांक)-ret, errorbuf);
    वापस CURLE_SSL_CONNECT_ERROR;
  }

  {
    अक्षर cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, माप(cipher_str), TRUE);
    infof(तथ्य, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#अगर_घोषित HAVE_PINNED_PUBKEY
  अगर(pinnedpubkey) {
    पूर्णांक आकार;
    स्थिर mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = शून्य;
    अचिन्हित अक्षर *pubkey = शून्य;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    अगर(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(तथ्य, "Failed due to missing peer certificate");
      वापस CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, माप(*p));

    अगर(!p)
      वापस CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    अगर(!pubkey) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    अगर(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(तथ्य, "Failed copying peer certificate");
      परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      जाओ pinnedpubkey_error;
    }

    आकार = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    अगर(आकार <= 0) {
      failf(तथ्य, "Failed copying public key from peer certificate");
      परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      जाओ pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    परिणाम = Curl_pin_peer_pubkey(तथ्य,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - आकार], आकार);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    अगर(परिणाम)
      वापस परिणाम;
  }
#पूर्ण_अगर

#अगर_घोषित HAS_ALPN_MBEDTLS
  अगर(connssl->alpn) {
    स्थिर अक्षर *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    परिणाम = Curl_alpn_set_negotiated(cf, तथ्य, connssl,
                                      (स्थिर अचिन्हित अक्षर *)proto,
                                      proto ? म_माप(proto) : 0);
    अगर(परिणाम)
      वापस परिणाम;
  }
#पूर्ण_अगर

  connssl->connecting_state = ssl_connect_3;
  infof(तथ्य, "SSL connected");

  वापस CURLE_OK;
}

अटल CURLcode mbed_new_session(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  काष्ठा Curl_ssl_session *sc_session = शून्य;
  अचिन्हित अक्षर *sdata = शून्य;
  माप_प्रकार slen = 0;
  पूर्णांक ietf_tls_id;
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक ret;

  DEBUGASSERT(backend);
  अगर(!Curl_ssl_scache_use(cf, तथ्य))
    वापस CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  अगर(ret) {
    failf(तथ्य, "mbedtls_ssl_get_session returned -0x%x", (अचिन्हित पूर्णांक)-ret);
    परिणाम = CURLE_SSL_CONNECT_ERROR;
    जाओ out;
  }

  mbedtls_ssl_session_save(&session, शून्य, 0, &slen);
  अगर(!slen) {
    failf(तथ्य, "failed to serialize session: length is 0");
    जाओ out;
  }

  sdata = curlx_malloc(slen);
  अगर(!sdata) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  अगर(ret) {
    failf(तथ्य, "failed to serialize session: -0x%x", (अचिन्हित पूर्णांक)-ret);
    जाओ out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_संख्या(&backend->ssl);
  परिणाम = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = शून्य;  /* call took ownership */
  अगर(!परिणाम && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    परिणाम = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  अगर(!परिणाम) {
    परिणाम = Curl_ssl_scache_put(cf, तथ्य, connssl->peer.scache_कुंजी,
                                 sc_session);
    sc_session = शून्य;
  }

out:
  अगर(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  वापस परिणाम;
}

अटल CURLcode mbed_send(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                          स्थिर व्योम *mem, माप_प्रकार len, माप_प्रकार *pnwritten)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक nwritten;

  DEBUGASSERT(backend);
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_3
do_send:
#पूर्ण_अगर
  *pnwritten = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  /* mbedTLS is picky when a mbedtls_ssl_write() was previously blocked.
   * It requires to be called with the same amount of bytes again, or it
   * loses bytes, e.g. reporting all was sent but they were not.
   * Remember the blocked length and use that when set. */
  अगर(backend->send_blocked) {
    अगर(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(तथ्य, "re-send data shorter than previously");
      परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
      जाओ out;
    }
    CURL_TRC_CF(तथ्य, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (स्थिर अचिन्हित अक्षर *)mem, len);

  अगर(nwritten >= 0) {
    *pnwritten = (माप_प्रकार)nwritten;
    backend->send_blocked = FALSE;
  }
  अन्यथा {
    CURL_TRC_CF(तथ्य, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (अचिन्हित पूर्णांक)-nwritten);
    चयन(nwritten) {
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_3
    हाल MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      परिणाम = mbed_new_session(cf, तथ्य);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      अगर(!परिणाम)
        जाओ do_send;
      अवरोध;
#पूर्ण_अगर
    हाल MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      परिणाम = CURLE_AGAIN;
      अवरोध;
    हाल MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      परिणाम = CURLE_AGAIN;
      अवरोध;
    शेष:
      परिणाम = CURLE_SEND_ERROR;
      अवरोध;
    }
    अगर((परिणाम == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(तथ्य, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (पूर्णांक)परिणाम, *pnwritten);
  वापस परिणाम;
}

अटल CURLcode mbedtls_shutdown(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 bool send_shutdown, bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  अचिन्हित अक्षर buf[1024];
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक ret = 0;
  माप_प्रकार i;

  DEBUGASSERT(backend);

  अगर(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  अगर(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    अगर(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      चयन(ret) {
      हाल 0: /* we sent it, receive from the server */
        अवरोध;
      हाल MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        जाओ out;
      हाल MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        जाओ out;
      हाल MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        जाओ out;
      शेष:
        CURL_TRC_CF(तथ्य, cf, "mbedtls_shutdown error -0x%04X",
                    (अचिन्हित पूर्णांक)-ret);
        परिणाम = CURLE_RECV_ERROR;
        जाओ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  क्रम(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, माप(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    अगर(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, माप(buf));
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_3
    अगर(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      जारी;
#पूर्ण_अगर
    अगर(ret <= 0)
      अवरोध;
  }

  अगर(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(तथ्य, cf, "mbedtls_shutdown, still getting data");
  }
  अन्यथा अगर(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(तथ्य, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  अन्यथा अगर(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(तथ्य, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  अन्यथा अगर(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(तथ्य, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  अन्यथा {
    CURL_TRC_CF(तथ्य, cf, "mbedtls_shutdown error -0x%04X",
                (अचिन्हित पूर्णांक)-ret);
    परिणाम = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (परिणाम || *done);
  वापस परिणाम;
}

अटल व्योम mbedtls_close(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;

  (व्योम)तथ्य;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#अगर_घोषित MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#पूर्ण_अगर
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  अगर(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

अटल CURLcode mbed_recv(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                          अक्षर *buf, माप_प्रकार buffersize, माप_प्रकार *pnread)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  CURLcode परिणाम = CURLE_OK;
  पूर्णांक nread;

  DEBUGASSERT(backend);
#अगर_घोषित MBEDTLS_SSL_SESSION_TICKETS
do_read:
#पूर्ण_अगर
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (अचिन्हित अक्षर *)buf, buffersize);
  अगर(nread > 0)
    *pnread = (माप_प्रकार)nread;
  अन्यथा {
    अक्षर errorbuf[128];
    CURL_TRC_CF(तथ्य, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (अचिन्हित पूर्णांक)-nread);
    चयन(nread) {
#अगर_घोषित MBEDTLS_SSL_SESSION_TICKETS
    हाल MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      परिणाम = mbed_new_session(cf, तथ्य);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      अगर(!परिणाम)
        जाओ do_read;
      अवरोध;
#पूर्ण_अगर
    हाल MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      परिणाम = CURLE_AGAIN;
      अवरोध;
    हाल MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      परिणाम = CURLE_AGAIN;
      अवरोध;
    हाल MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      परिणाम = CURLE_OK;
      अवरोध;
    शेष:
      mbedtls_strerror(nread, errorbuf, माप(errorbuf));
      failf(तथ्य, "ssl_read returned: (-0x%04X) %s", (अचिन्हित पूर्णांक)-nread,
            errorbuf);
      परिणाम = CURLE_RECV_ERROR;
      अवरोध;
    }
  }
  वापस परिणाम;
}

अटल माप_प्रकार mbedtls_version(अक्षर *बफर, माप_प्रकार आकार)
{
#अगर_घोषित MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  अचिन्हित पूर्णांक version = mbedtls_version_get_संख्या();
  वापस curl_msnprintf(बफर, आकार, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#अन्यथा
  वापस curl_msnprintf(बफर, आकार, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#पूर्ण_अगर
}

/* 'data' might be NULL */
अटल CURLcode mbedtls_random(काष्ठा Curl_easy *तथ्य,
                               अचिन्हित अक्षर *entropy, माप_प्रकार लंबाई)
{
  psa_status_t status;
  (व्योम)तथ्य;

  status = psa_generate_random(entropy, लंबाई);

  वापस status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

अटल CURLcode mbedtls_connect(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                bool *done)
{
  CURLcode परिणाम;
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;

  /* check if the connection has already been established */
  अगर(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  अगर(connssl->connecting_state == ssl_connect_1) {
    परिणाम = mbed_connect_step1(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(connssl->connecting_state == ssl_connect_2) {
    परिणाम = mbed_connect_step2(cf, तथ्य);
    अगर(परिणाम)
      वापस परिणाम;
  }

  अगर(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    काष्ठा ssl_connect_तथ्य *ctx = cf->ctx;
    काष्ठा mbed_ssl_backend_तथ्य *backend =
      (काष्ठा mbed_ssl_backend_तथ्य *)ctx->backend;

    अगर(mbedtls_ssl_get_version_संख्या(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      परिणाम = mbed_new_session(cf, तथ्य);
      अगर(परिणाम)
        वापस परिणाम;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  अगर(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  वापस CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
अटल पूर्णांक mbedtls_init(व्योम)
{
#अगर MBEDTLS_VERSION_संख्या < 0x04000000
  पूर्णांक ret = 0;
#पूर्ण_अगर
  psa_status_t status;
  status = psa_crypto_init();

  अगर(status != PSA_SUCCESS)
    वापस 0;

#अगर MBEDTLS_VERSION_संख्या < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              शून्य, 0);

  अगर(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    वापस 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#पूर्ण_अगर
  वापस 1;
}

अटल व्योम mbedtls_cleanup(व्योम)
{
  mbedtls_psa_crypto_free();

#अगर MBEDTLS_VERSION_संख्या < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#पूर्ण_अगर
}

अटल bool mbedtls_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                                 स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *ctx = cf->ctx;
  काष्ठा mbed_ssl_backend_तथ्य *backend;

  (व्योम)तथ्य;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (काष्ठा mbed_ssl_backend_तथ्य *)ctx->backend;
  वापस mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

अटल CURLcode mbedtls_sha256sum(स्थिर अचिन्हित अक्षर *निवेश,
                                  माप_प्रकार inputlen,
                                  अचिन्हित अक्षर *sha256sum,
                                  माप_प्रकार sha256len)
{
#अगर defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  माप_प्रकार sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, निवेश, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  अगर(status != PSA_SUCCESS)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  वापस CURLE_OK;
#अन्यथा
  (व्योम)sha256len;
  वापस Curl_sha256it(sha256sum, निवेश, inputlen);
#पूर्ण_अगर
}

अटल व्योम *mbedtls_get_internals(काष्ठा ssl_connect_तथ्य *connssl,
                                   CURLINFO info)
{
  काष्ठा mbed_ssl_backend_तथ्य *backend =
    (काष्ठा mbed_ssl_backend_तथ्य *)connssl->backend;
  (व्योम)info;
  DEBUGASSERT(backend);
  वापस &backend->ssl;
}

स्थिर काष्ठा Curl_ssl Curl_ssl_mbedtls = {
  { CURLSSLBACKEND_MBEDTLS, "mbedtls" }, /* info */

  SSLSUPP_CA_PATH |
  SSLSUPP_CAINFO_BLOB |
  SSLSUPP_CERTINFO |
#अगर_घोषित HAVE_PINNED_PUBKEY
  SSLSUPP_PINNEDPUBKEY |
#पूर्ण_अगर
  SSLSUPP_SSL_CTX |
#अगर_घोषित MBEDTLS_SSL_PROTO_TLS1_3  /* requires mbedTLS 3.6.0+ */
  SSLSUPP_TLS13_CIPHERSUITES |
#पूर्ण_अगर
  SSLSUPP_HTTPS_PROXY |
  SSLSUPP_CIPHER_LIST |
#अगर_घोषित MBEDTLS_X509_CRL_PARSE_C
  SSLSUPP_CRLFILE |
#पूर्ण_अगर
  0,

  माप(काष्ठा mbed_ssl_backend_तथ्य),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_तथ्य_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  शून्य,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  शून्य,                             /* close_all */
  शून्य,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  शून्य,                             /* get_channel_binding */
};

#पूर्ण_अगर /* USE_MBEDTLS */
