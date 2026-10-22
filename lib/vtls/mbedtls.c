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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_MBEDTLS

/* Define this to enable lots of debugging for mbedTLS */
/* #define MBEDTLS_DEBUG */

#અન્તર્ભાવ <mbedtls/version.h>
#જો MBEDTLS_VERSION_સંખ્યા < 0x03020000
#error "mbedTLS 3.2.0 or greater required"
#endif
#અન્તર્ભાવ <psa/crypto_config.h>
#અન્તર્ભાવ <mbedtls/net_sockets.h>
#અન્તર્ભાવ <mbedtls/ssl.h>
#અન્તર્ભાવ <mbedtls/x509.h>
#અન્તર્ભાવ <mbedtls/psa_util.h>

#જો MBEDTLS_VERSION_સંખ્યા < 0x04000000 && !defined(MBEDTLS_CTR_DRBG_C)
#error "MBEDTLS_CTR_DRBG_C is required for mbedTLS 3.x."
#endif

#અન્તર્ભાવ <mbedtls/error.h>
#જો MBEDTLS_VERSION_સંખ્યા < 0x04000000
#અન્તર્ભાવ <mbedtls/entropy.h>
#અન્તર્ભાવ <mbedtls/ctr_drbg.h>
#endif
#ifdef MBEDTLS_DEBUG
#અન્તર્ભાવ <mbedtls/debug.h>
#endif

#અન્તર્ભાવ "vtls/cipher_suite.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "vtls/mbedtls.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "vtls/vtls_int.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "vtls/x509asn1.h"
#અન્તર્ભાવ "connect.h" /* for the connect timeout */
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curl_sha256.h"

/* ALPN for http2 */
#જો defined(USE_HTTP2) && defined(MBEDTLS_SSL_ALPN)
#define HAS_ALPN_MBEDTLS
#endif

સંરચના mbed_ssl_backend_data {
  mbedtls_ssl_context ssl;
  mbedtls_x509_crt cacert;
  mbedtls_x509_crt clicert;
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl crl;
#endif
  mbedtls_pk_context pk;
  mbedtls_ssl_config config;
#ifdef HAS_ALPN_MBEDTLS
  અચલ અક્ષર *protocols[3];
#endif
  પૂર્ણાંક *ciphersuites;
  size_t send_blocked_len;
  BIT(initialized); /* mbedtls_ssl_context is initialized */
  BIT(sent_shutdown);
  BIT(send_blocked);
};

/** A context for random number generation (RNG).
 */
#જો MBEDTLS_VERSION_સંખ્યા < 0x04000000
સંરચના rng_context_t {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context drbg;
};

સ્થિર સંરચના rng_context_t rng;
#endif

#ifndef MBEDTLS_ERROR_C
#define mbedtls_strerror(a, b, c) b[0] = 0
#endif

#ifdef MBEDTLS_DEBUG
સ્થિર શૂન્ય mbed_debug(શૂન્ય *context, પૂર્ણાંક level, અચલ અક્ષર *f_name,
                       પૂર્ણાંક લીટી_nb, અચલ અક્ષર *લીટી)
{
  સંરચના Curl_easy *data = (સંરચના Curl_easy *)context;
  (શૂન્ય)level;
  (શૂન્ય)લીટી_nb;
  (શૂન્ય)f_name;

  જો(data) {
    size_t len = strlen(લીટી);
    જો(len && (લીટી[len - 1] == '\n'))
      /* discount any trailing newline */
      len--;
    infof(data, "%.*s", (પૂર્ણાંક)len, લીટી);
  }
}
#endif

સ્થિર પૂર્ણાંક mbedtls_bio_cf_write(શૂન્ય *bio,
                                અચલ unsigned અક્ષર *buf, size_t blen)
{
  સંરચના Curl_cfilter *cf = bio;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nwritten;
  CURLcode result;

  DEBUGASSERT(data);
  જો(!data)
    પાછા 0;

  result = Curl_conn_cf_send(cf->next, data, buf, blen, FALSE, &nwritten);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_out_write(len=%zu) -> %d, %zu",
              blen, (પૂર્ણાંક)result, nwritten);
  જો(result == CURLE_AGAIN)
    પાછા MBEDTLS_ERR_SSL_WANT_WRITE;
  પાછા result ? -1 : (પૂર્ણાંક)nwritten;
}

સ્થિર પૂર્ણાંક mbedtls_bio_cf_read(શૂન્ય *bio, unsigned અક્ષર *buf, size_t blen)
{
  સંરચના Curl_cfilter *cf = bio;
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);
  size_t nread = 0;
  CURLcode result;

  DEBUGASSERT(data);
  જો(!data)
    પાછા 0;
  /* OpenSSL catches this case, so should we. */
  જો(!buf)
    પાછા 0;

  result = Curl_conn_cf_recv(cf->next, data, (અક્ષર *)buf, blen, &nread);
  CURL_TRC_CF(data, cf, "mbedtls_bio_cf_in_read(len=%zu) -> %d, %zu",
              blen, (પૂર્ણાંક)result, nread);
  જો(result == CURLE_AGAIN)
    પાછા MBEDTLS_ERR_SSL_WANT_READ;
  /* nread is never larger than int here */
  પાછા result ? -1 : (પૂર્ણાંક)nread;
}

/* See:
 * https://web.archive.org/web/20200921194007/tls.mbed.org/discussions/generic/howto-determine-exact-buffer-len-for-mbedtls_pk_write_pubkey_der
 */
#define RSA_PUB_DER_MAX_BYTES   (38 + (2 * MBEDTLS_MPI_MAX_SIZE))
#define ECP_PUB_DER_MAX_BYTES   (30 + (2 * MBEDTLS_ECP_MAX_BYTES))

#define PUB_DER_MAX_BYTES   (RSA_PUB_DER_MAX_BYTES > ECP_PUB_DER_MAX_BYTES ? \
                             RSA_PUB_DER_MAX_BYTES : ECP_PUB_DER_MAX_BYTES)

સ્થિર CURLcode mbed_set_ssl_version_min_max(
  સંરચના Curl_easy *data,
  સંરચના mbed_ssl_backend_data *backend,
  સંરચના ssl_filter_config *conn_config)
{
  mbedtls_ssl_protocol_version ver_min =
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    MBEDTLS_SSL_VERSION_TLS1_2
#નહીંતર
    MBEDTLS_SSL_VERSION_TLS1_3
#endif
    ;
  mbedtls_ssl_protocol_version ver_max =
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    MBEDTLS_SSL_VERSION_TLS1_3
#નહીંતર
    MBEDTLS_SSL_VERSION_TLS1_2
#endif
    ;

  DEBUGASSERT(conn_config->version != CURL_SSLVERSION_DEFAULT);
  પસંદગી(conn_config->version) {
  સ્થિતિ CURL_SSLVERSION_TLSv1:
  સ્થિતિ CURL_SSLVERSION_TLSv1_0:
  સ્થિતિ CURL_SSLVERSION_TLSv1_1:
  સ્થિતિ CURL_SSLVERSION_TLSv1_2:
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
    ver_min = MBEDTLS_SSL_VERSION_TLS1_2;
    તોડો;
#endif
  સ્થિતિ CURL_SSLVERSION_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_min = MBEDTLS_SSL_VERSION_TLS1_3;
    તોડો;
#endif
  મૂળભૂત:
    failf(data, "mbedTLS: unsupported minimum TLS version value: %x",
          conn_config->version);
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  પસંદગી(conn_config->version_max) {
  સ્થિતિ CURL_SSLVERSION_MAX_DEFAULT:
  સ્થિતિ CURL_SSLVERSION_MAX_NONE:
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_3:
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ver_max = MBEDTLS_SSL_VERSION_TLS1_3;
    તોડો;
#endif
#ifdef MBEDTLS_SSL_PROTO_TLS1_2
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_2:
    ver_max = MBEDTLS_SSL_VERSION_TLS1_2;
    તોડો;
#endif
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_1:
  સ્થિતિ CURL_SSLVERSION_MAX_TLSv1_0:
  મૂળભૂત:
    failf(data, "mbedTLS: unsupported maximum TLS version value");
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_conf_min_tls_version(&backend->config, ver_min);
  mbedtls_ssl_conf_max_tls_version(&backend->config, ver_max);

  પાછા CURLE_OK;
}

/* TLS_ECJPAKE_WITH_AES_128_CCM_8 (0xC0FF) is marked experimental in mbedTLS.
   The number is not reserved by IANA nor is the cipher suite present in other
   SSL implementations. Provide provisional support for specifying the cipher
   suite here. */
#ifdef MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
સ્થિર પૂર્ણાંક mbed_cipher_suite_get_str(uint16_t id, અક્ષર *buf, size_t buf_size,
                                     bool prefer_rfc)
{
  જો(id == MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8)
    curl_msnprintf(buf, buf_size, "%s", "TLS_ECJPAKE_WITH_AES_128_CCM_8");
  નહીંતર
    પાછા Curl_cipher_suite_get_str(id, buf, buf_size, prefer_rfc);
  પાછા 0;
}

સ્થિર uint16_t mbed_cipher_suite_walk_str(અચલ અક્ષર **str, અચલ અક્ષર **end)
{
  uint16_t id = Curl_cipher_suite_walk_str(str, end);
  size_t len = *end - *str;
  સ્થિર અચલ અક્ષર ecjpake_suite[] = "TLS_ECJPAKE_WITH_AES_128_CCM_8";

  જો(!id) {
    જો((len == CURL_CSTRLEN(ecjpake_suite)) &&
       curl_strnequal(ecjpake_suite, *str, len))
      id = MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8;
  }
  પાછા id;
}
#નહીંતર
#define mbed_cipher_suite_get_str  Curl_cipher_suite_get_str
#define mbed_cipher_suite_walk_str Curl_cipher_suite_walk_str
#endif

સ્થિર CURLcode mbed_set_selected_ciphers(
  સંરચના Curl_easy *data,
  સંરચના mbed_ssl_backend_data *backend,
  અચલ અક્ષર *ciphers12,
  અચલ અક્ષર *ciphers13)
{
  અચલ અક્ષર *ciphers = ciphers12;
  અચલ પૂર્ણાંક *supported;
  પૂર્ણાંક *selected;
  size_t supported_len, ગણતરી = 0, default13_ગણતરી = 0, i, j;
  અચલ અક્ષર *ptr, *end;

  supported = mbedtls_ssl_list_ciphersuites();
  માટે(i = 0; supported[i] != 0; i++)
    ;
  supported_len = i;

  selected = curlx_malloc(કદ(પૂર્ણાંક) * (supported_len + 1));
  જો(!selected)
    પાછા CURLE_OUT_OF_MEMORY;

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  (શૂન્ય)ciphers13, (શૂન્ય)j;
#નહીંતર
  જો(!ciphers13) {
    /* Add default TLSv1.3 ciphers to selection */
    માટે(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      જો(strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ચાલુ;

      selected[ગણતરી++] = id;
    }

    default13_ગણતરી = ગણતરી;
  }
  નહીંતર
    ciphers = ciphers13;

add_ciphers:
#endif
  માટે(ptr = ciphers; ptr[0] != '\0' && ગણતરી < supported_len; ptr = end) {
    uint16_t id = mbed_cipher_suite_walk_str(&ptr, &end);

    /* Check if cipher is supported */
    જો(id) {
      માટે(i = 0; i < supported_len && supported[i] != id; i++)
        ;
      જો(i == supported_len)
        id = 0;
    }
    જો(!id) {
      જો(ptr[0] != '\0')
        infof(data, "mbedTLS: unknown cipher in list: \"%.*s\"",
              (પૂર્ણાંક)(end - ptr), ptr);
      ચાલુ;
    }

    /* No duplicates allowed (so selected cannot overflow) */
    માટે(i = 0; i < ગણતરી && selected[i] != id; i++)
      ;
    જો(i < ગણતરી) {
      જો(i >= default13_ગણતરી)
        infof(data, "mbedTLS: duplicate cipher in list: \"%.*s\"",
              (પૂર્ણાંક)(end - ptr), ptr);
      ચાલુ;
    }

    selected[ગણતરી++] = id;
  }

#ifdef MBEDTLS_SSL_PROTO_TLS1_3
  જો(ciphers == ciphers13 && ciphers12) {
    ciphers = ciphers12;
    જાઓ add_ciphers;
  }

  જો(!ciphers12) {
    /* Add default TLSv1.2 ciphers to selection */
    માટે(j = 0; j < supported_len; j++) {
      uint16_t id = (uint16_t)supported[j];
      જો(!strncmp(mbedtls_ssl_get_ciphersuite_name(id), "TLS1-3", 6))
        ચાલુ;

      /* No duplicates allowed (so selected cannot overflow) */
      માટે(i = 0; i < ગણતરી && selected[i] != id; i++)
        ;
      જો(i < ગણતરી)
        ચાલુ;

      selected[ગણતરી++] = id;
    }
  }
#endif

  selected[ગણતરી] = 0;

  જો(ગણતરી == 0) {
    curlx_free(selected);
    failf(data, "mbedTLS: no supported cipher in list");
    પાછા CURLE_SSL_CIPHER;
  }

  /* mbedtls_ssl_conf_ciphersuites(): The ciphersuites array is not copied.
     It must remain valid for the lifetime of the SSL configuration */
  backend->ciphersuites = selected;
  mbedtls_ssl_conf_ciphersuites(&backend->config, backend->ciphersuites);
  પાછા CURLE_OK;
}

સ્થિર શૂન્ય mbed_dump_cert_info(સંરચના Curl_easy *data,
                                અચલ mbedtls_x509_crt *crt)
{
#જો !defined(CURLVERBOSE) || defined(MBEDTLS_X509_REMOVE_INFO)
  (શૂન્ય)data, (શૂન્ય)crt;
#નહીંતર
  અચલ size_t bufsize = 16384;
  અક્ષર *p, *buffer = curlx_malloc(bufsize);

  જો(buffer && mbedtls_x509_crt_info(buffer, bufsize, " ", crt) > 0) {
    infof(data, "Server certificate:");
    માટે(p = buffer; *p; p += *p != '\0') {
      size_t s = strcspn(p, "\n");
      infof(data, "%.*s", (પૂર્ણાંક)s, p);
      p += s;
    }
  }
  નહીંતર
    infof(data, "Unable to dump certificate information");

  curlx_free(buffer);
#endif
}

સ્થિર શૂન્ય mbed_extract_certinfo(સંરચના Curl_easy *data,
                                  અચલ mbedtls_x509_crt *crt)
{
  CURLcode result;
  અચલ mbedtls_x509_crt *cur;
  પૂર્ણાંક cert_ગણતરી = 0;
  પૂર્ણાંક i;

  માટે(cur = crt; cur && cert_ગણતરી <= MAX_ALLOWED_CERT_AMOUNT; cur = cur->next)
    cert_ગણતરી++;

  જો(cert_ગણતરી > MAX_ALLOWED_CERT_AMOUNT) {
    infof(data, "More certificates than allowed (%d), skipping certinfo",
          MAX_ALLOWED_CERT_AMOUNT);
    પાછા;
  }

  result = Curl_ssl_init_certinfo(data, cert_ગણતરી);

  માટે(i = 0, cur = crt; result == CURLE_OK && cur; ++i, cur = cur->next) {
    result = Curl_extract_certinfo(data, i, cur->raw.p,
                                   cur->raw.p + cur->raw.len);
  }
}

સ્થિર પૂર્ણાંક mbed_verify_cb(શૂન્ય *ptr, mbedtls_x509_crt *crt,
                          પૂર્ણાંક depth, uint32_t *flags)
{
  સંરચના Curl_cfilter *cf = (સંરચના Curl_cfilter *)ptr;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના Curl_easy *data = CF_DATA_CURRENT(cf);

  જો(depth == 0) {
    જો(data->set.verbose)
      mbed_dump_cert_info(data, crt);
    જો(data->set.ssl.certinfo)
      mbed_extract_certinfo(data, crt);
  }

  /* `verifypeer` and `verifyhost` are independent, so clear the flags of a
     disabled check only. The name mismatch belongs to `verifyhost`. */
  જો(!conn_config->verifypeer)
    *flags &= MBEDTLS_X509_BADCERT_CN_MISMATCH;
  જો(!conn_config->verifyhost)
    *flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;

  જો(*flags) {
#ifndef MBEDTLS_X509_REMOVE_INFO
    અક્ષર buf[128];
    mbedtls_x509_crt_verify_info(buf, કદ(buf), "", *flags);
    failf(data, "mbedTLS: %s", buf);
#નહીંતર
    failf(data, "mbedTLS: certificate verification error 0x%08x",
          (unsigned પૂર્ણાંક)*flags);
#endif
  }

  પાછા 0;
}

સ્થિર CURLcode mbed_load_cacert(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  અચલ સંરચના curl_blob *ca_info_blob = conn_config->ca_info_blob;
  અચલ અક્ષર * અચલ ssl_cafile =
    /* CURLOPT_CAINFO_BLOB overrides CURLOPT_CAINFO */
    (ca_info_blob ? ખાલી : conn_config->CAfile);
  અચલ bool verifypeer = conn_config->verifypeer;
  અચલ અક્ષર * અચલ ssl_capath = conn_config->CApath;
#ifdef MBEDTLS_PEM_PARSE_C
  અચલ અક્ષર * અચલ ssl_cert_type = conn_config->cert_type;
#endif
  પૂર્ણાંક ret = -1;
  અક્ષર errorbuf[128];

  mbedtls_x509_crt_init(&backend->cacert);

  જો(ca_info_blob && verifypeer) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    જો((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((અક્ષર *)(ca_info_blob->data))[ca_info_blob->len - 1] == '\0') {
      ret = mbedtls_x509_crt_parse(&backend->cacert, ca_info_blob->data,
                                   ca_info_blob->len);
    }
    નહીંતર { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned અક્ષર *newblob = curlx_memdup0(ca_info_blob->data,
                                             ca_info_blob->len);
      જો(!newblob)
        પાછા CURLE_OUT_OF_MEMORY;

      ret = mbedtls_x509_crt_parse(&backend->cacert, newblob,
                                   ca_info_blob->len + 1);
      curlx_free(newblob);
    }
#નહીંતર
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->cacert, ca_info_blob->data,
                                     ca_info_blob->len);
#endif

    જો(ret < 0) {
      mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
      failf(data, "mbedTLS: error importing CA cert blob: (-0x%04X) %s",
            (unsigned પૂર્ણાંક)-ret, errorbuf);
      પાછા CURLE_SSL_CERTPROBLEM;
    }
  }

  જો(ssl_cafile && verifypeer) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->cacert, ssl_cafile);

    જો(ret < 0) {
      mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
      failf(data, "mbedTLS: error reading CA cert file %s: (-0x%04X) %s",
            ssl_cafile, (unsigned પૂર્ણાંક)-ret, errorbuf);
      પાછા CURLE_SSL_CACERT_BADFILE;
    }
#નહીંતર
    failf(data, "mbedTLS: functions that use the file system not built in");
    પાછા CURLE_NOT_BUILT_IN;
#endif
  }

  જો(ssl_capath) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_path(&backend->cacert, ssl_capath);

    જો(ret < 0) {
      mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
      failf(data, "mbedTLS: error reading CA cert path %s: (-0x%04X) %s",
            ssl_capath, (unsigned પૂર્ણાંક)-ret, errorbuf);

      જો(verifypeer)
        પાછા CURLE_SSL_CACERT_BADFILE;
    }
#નહીંતર
    failf(data, "mbedTLS: functions that use the file system not built in");
    પાછા CURLE_NOT_BUILT_IN;
#endif
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode mbed_load_clicert(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  અક્ષર * અચલ ssl_cert = conn_config->clientcert;
  અચલ સંરચના curl_blob *ssl_cert_blob = conn_config->cert_blob;
#ifdef MBEDTLS_PEM_PARSE_C
  અચલ અક્ષર * અચલ ssl_cert_type = conn_config->cert_type;
#endif
  પૂર્ણાંક ret = -1;
  અક્ષર errorbuf[128];

  mbedtls_x509_crt_init(&backend->clicert);

  જો(ssl_cert) {
#ifdef MBEDTLS_FS_IO
    ret = mbedtls_x509_crt_parse_file(&backend->clicert, ssl_cert);

    જો(ret) {
      mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
      failf(data, "mbedTLS: error reading client cert file %s: (-0x%04X) %s",
            ssl_cert, (unsigned પૂર્ણાંક)-ret, errorbuf);

      પાછા CURLE_SSL_CERTPROBLEM;
    }
#નહીંતર
    failf(data, "mbedTLS: functions that use the file system not built in");
    પાછા CURLE_NOT_BUILT_IN;
#endif
  }

  જો(ssl_cert_blob) {
#ifdef MBEDTLS_PEM_PARSE_C
    /* if DER or a null-terminated PEM process using
       mbedtls_x509_crt_parse(). */
    જો((ssl_cert_type && curl_strequal(ssl_cert_type, "DER")) ||
       ((અક્ષર *)(ssl_cert_blob->data))[ssl_cert_blob->len - 1] == '\0') {

      ret = mbedtls_x509_crt_parse(&backend->clicert,
                                   ssl_cert_blob->data,
                                   ssl_cert_blob->len);
    }
    નહીંતર { /* they say it is PEM and it is not null-terminated */

      /* Unfortunately, mbedtls_x509_crt_parse() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). The function accepts PEM or DER formats, but we cannot
         assume if the user passed in a PEM format cert that it is
         null-terminated. */
      unsigned અક્ષર *newblob = curlx_memdup0(ssl_cert_blob->data,
                                             ssl_cert_blob->len);
      જો(!newblob)
        પાછા CURLE_OUT_OF_MEMORY;
      ret = mbedtls_x509_crt_parse(&backend->clicert, newblob,
                                   ssl_cert_blob->len + 1);
      curlx_free(newblob);
    }
#નહીંતર
    /* DER encoded certs do not need to be null-terminated because it is a
       binary format. Thus, if we are not compiling with PEM_PARSE we can avoid
       the extra memory copies altogether. */
    ret = mbedtls_x509_crt_parse_der(&backend->clicert, ssl_cert_blob->data,
                                     ssl_cert_blob->len);
#endif

    જો(ret) {
      mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
      failf(data, "mbedTLS: error reading client cert blob: (-0x%04X) %s",
            (unsigned પૂર્ણાંક)-ret, errorbuf);
      પાછા CURLE_SSL_CERTPROBLEM;
    }
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode mbed_load_privkey(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  પૂર્ણાંક ret = -1;
  અક્ષર errorbuf[128];

  mbedtls_pk_init(&backend->pk);

  જો(conn_config->ચાવી || conn_config->ચાવી_blob) {
    જો(conn_config->ચાવી) {
#ifdef MBEDTLS_FS_IO
#જો MBEDTLS_VERSION_સંખ્યા >= 0x04000000
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ચાવી,
                                     conn_config->ચાવી_passwd);
      જો(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ચાવી_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ચાવી_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#નહીંતર
      ret = mbedtls_pk_parse_keyfile(&backend->pk, conn_config->ચાવી,
                                     conn_config->ચાવી_passwd,
                                     mbedtls_ctr_drbg_random,
                                     &rng.drbg);
      જો(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif

      જો(ret) {
        mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
        failf(data, "mbedTLS: error reading private key %s: (-0x%04X) %s",
              conn_config->ચાવી, (unsigned પૂર્ણાંક)-ret, errorbuf);
        પાછા CURLE_SSL_CERTPROBLEM;
      }
#નહીંતર
      failf(data, "mbedTLS: functions that use the file system not built in");
      પાછા CURLE_NOT_BUILT_IN;
#endif
    }
    નહીંતર {
      અચલ સંરચના curl_blob *ssl_ચાવી_blob = conn_config->ચાવી_blob;
      અચલ અક્ષર *passwd = conn_config->ચાવી_passwd;
      અચલ bool is_der = conn_config->ચાવી_type &&
                          curl_strequal(conn_config->ચાવી_type, "DER");
      /* Unfortunately, mbedtls_pk_parse_key() requires the data to be
         null-terminated if the data is PEM encoded (even when provided the
         exact length). */
      unsigned અક્ષર *newblob = curlx_memdup0(ssl_ચાવી_blob->data,
                                             ssl_ચાવી_blob->len);
      /* mbedtls_pk_parse_key() requires the terminator to be counted in the
         length for PEM input, while the DER parsers require the exact
         length: a DER key with the terminator counted in is rejected with
         MBEDTLS_ERR_ASN1_LENGTH_MISMATCH. */
      size_t keylen = is_der ? ssl_ચાવી_blob->len : ssl_ચાવી_blob->len + 1;
      જો(!newblob)
        પાછા CURLE_OUT_OF_MEMORY;

#જો MBEDTLS_VERSION_સંખ્યા >= 0x04000000
      ret = mbedtls_pk_parse_ચાવી(&backend->pk, newblob, keylen,
                                 (અચલ unsigned અક્ષર *)passwd,
                                 passwd ? strlen(passwd) : 0);
      જો(ret == 0 &&
         !(mbedtls_pk_can_do_psa(&backend->pk,
                                 PSA_ALG_RSA_PKCS1V15_SIGN(PSA_ALG_ANY_HASH),
                                 PSA_ચાવી_USAGE_SIGN_HASH) ||
           mbedtls_pk_can_do_psa(&backend->pk,
                                 MBEDTLS_PK_ALG_ECDSA(PSA_ALG_ANY_HASH),
                                 PSA_ચાવી_USAGE_SIGN_HASH)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#નહીંતર
      ret = mbedtls_pk_parse_ચાવી(&backend->pk, newblob, keylen,
                                 (અચલ unsigned અક્ષર *)passwd,
                                 passwd ? strlen(passwd) : 0,
                                 mbedtls_ctr_drbg_random,
                                 &rng.drbg);
      જો(ret == 0 && !(mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_RSA) ||
                       mbedtls_pk_can_do(&backend->pk, MBEDTLS_PK_ECKEY)))
        ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;
#endif
      curlx_free(newblob);

      જો(ret) {
        mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
        failf(data, "mbedTLS: error parsing private key: (-0x%04X) %s",
              (unsigned પૂર્ણાંક)-ret, errorbuf);
        પાછા CURLE_SSL_CERTPROBLEM;
      }
    }
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode mbed_load_crl(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  અચલ અક્ષર * અચલ ssl_crlfile = conn_config->CRLfile;

#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_init(&backend->crl);

  જો(ssl_crlfile) {
    અક્ષર errorbuf[128];
#ifdef MBEDTLS_FS_IO
    પૂર્ણાંક ret = mbedtls_x509_crl_parse_file(&backend->crl, ssl_crlfile);

    જો(ret) {
      mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
      failf(data, "mbedTLS: error reading CRL file %s: (-0x%04X) %s",
            ssl_crlfile, (unsigned પૂર્ણાંક)-ret, errorbuf);

      પાછા CURLE_SSL_CRL_BADFILE;
    }
#નહીંતર
    (શૂન્ય)errorbuf;
    failf(data, "mbedTLS: functions that use the file system not built in");
    પાછા CURLE_NOT_BUILT_IN;
#endif
  }
#નહીંતર
  (શૂન્ય)backend;
  જો(ssl_crlfile) {
    failf(data, "mbedTLS: CRL support not built in");
    પાછા CURLE_NOT_BUILT_IN;
  }
#endif

  પાછા CURLE_OK;
}

સ્થિર bool mbed_apply_session(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના Curl_ssl_session *sc_session)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;

  જો(sc_session && sc_session->sdata && sc_session->sdata_len) {
    mbedtls_ssl_session session;
    પૂર્ણાંક ret;

    mbedtls_ssl_session_init(&session);
    ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                   sc_session->sdata_len);
    જો(ret) {
      failf(data, "SSL session error loading: -0x%x", (unsigned પૂર્ણાંક)-ret);
    }
    નહીંતર {
      ret = mbedtls_ssl_set_session(&backend->ssl, &session);
      જો(ret)
        failf(data, "SSL session error setting: -0x%x", (unsigned પૂર્ણાંક)-ret);
    }
    mbedtls_ssl_session_free(&session);
    જો(!ret) {
      infof(data, "SSL reusing session ID");
      પાછા TRUE;
    }
  }
  પાછા FALSE;
}

સ્થિર CURLcode mbed_configure_ssl(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  પૂર્ણાંક ret;
  bool session_applied = FALSE;
  CURLcode result;
  અક્ષર errorbuf[128];

  infof(data, "mbedTLS: Connecting to %s:%d",
        connssl->peer.origin->hostname, connssl->peer.origin->port);

  mbedtls_ssl_config_init(&backend->config);
  ret = mbedtls_ssl_config_defaults(&backend->config,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT);
  જો(ret) {
    failf(data, "mbedTLS: ssl_config failed");
    પાછા CURLE_SSL_CONNECT_ERROR;
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

#જો defined(MBEDTLS_SSL_SESSION_TICKETS) && \
  defined(MBEDTLS_SSL_PROTO_TLS1_3) &&      \
  MBEDTLS_VERSION_સંખ્યા >= 0x03060100 &&   \
  MBEDTLS_VERSION_સંખ્યા < 0x04000000
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
  જો(result)
    પાછા result;

#જો MBEDTLS_VERSION_સંખ્યા < 0x04000000
  mbedtls_ssl_conf_rng(&backend->config, mbedtls_ctr_drbg_random,
                       &rng.drbg);
#endif

  ret = mbedtls_ssl_setup(&backend->ssl,
                          &backend->config);
  જો(ret) {
    mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
    failf(data, "mbedTLS: ssl_setup failed: "
          "(-0x%04X) %s", (unsigned પૂર્ણાંક)-ret, errorbuf);
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  mbedtls_ssl_set_bio(&backend->ssl, cf,
                      mbedtls_bio_cf_write,
                      mbedtls_bio_cf_read,
                      ખાલી /*  rev_timeout() */);

#ifndef MBEDTLS_SSL_PROTO_TLS1_3
  જો(conn_config->cipher_list) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list, ખાલી);
#નહીંતર
  જો(conn_config->cipher_list || conn_config->cipher_list13) {
    result = mbed_set_selected_ciphers(data, backend,
                                       conn_config->cipher_list,
                                       conn_config->cipher_list13);
#endif
    જો(result) {
      failf(data, "mbedTLS: failed to set cipher suites");
      પાછા result;
    }
  }
  નહીંતર {
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

  જો((cf->sockindex == SECONDARYSOCKET) && !(cf->cft->flags & CF_TYPE_PROXY)) {
    /* FTP is a bitch. On TLS secured transfers, it is a common server
     * option to require the client to use the SAME TLS session as on
     * the control connection or it fails the request. See #22225. */
    સંરચના Curl_ssl_session *scs =
      Curl_ssl_get_cf_session(data, cf->cft, FIRSTSOCKET);
    જો(scs) {
      જો(mbed_apply_session(cf, data, scs)) {
        CURL_TRC_CF(data, cf, "applied SSL session from control connection");
        session_applied = TRUE;
      }
    }
  }

  /* Check if there is a cached ID we can/should use here! */
  જો(!session_applied && Curl_ssl_scache_use(cf, data)) {
    સંરચના Curl_ssl_session *sc_session = ખાલી;
    CURLcode sresult = Curl_ssl_scache_take(cf, data, connssl->peer.scache_ચાવી,
                                            &sc_session);
    જો(!sresult && sc_session && sc_session->sdata && sc_session->sdata_len) {
      mbedtls_ssl_session session;

      mbedtls_ssl_session_init(&session);
      ret = mbedtls_ssl_session_load(&session, sc_session->sdata,
                                     sc_session->sdata_len);
      જો(ret) {
        failf(data, "SSL session error loading: -0x%x", (unsigned પૂર્ણાંક)-ret);
      }
      નહીંતર {
        ret = mbedtls_ssl_set_session(&backend->ssl, &session);
        જો(ret)
          failf(data, "SSL session error setting: -0x%x", (unsigned પૂર્ણાંક)-ret);
        નહીંતર
          infof(data, "SSL reusing session ID");
      }
      mbedtls_ssl_session_free(&session);
    }
    Curl_ssl_scache_return(cf, data, connssl->peer.scache_ચાવી,
                           sc_session);
  }

  mbedtls_ssl_conf_ca_chain(&backend->config, &backend->cacert,
#ifdef MBEDTLS_X509_CRL_PARSE_C
                            &backend->crl
#નહીંતર
                            ખાલી
#endif
    );

  જો(conn_config->ચાવી || conn_config->ચાવી_blob) {
    mbedtls_ssl_conf_own_cert(&backend->config, &backend->clicert,
                              &backend->pk);
  }

  જો(mbedtls_ssl_set_hostname(&backend->ssl, connssl->peer.sni ?
                              connssl->peer.sni :
                              connssl->peer.origin->hostname)) {
    /* mbedtls_ssl_set_hostname() sets the name to use in CN/SAN checks and
       the name to set in the SNI extension. Thus even if curl connects to
       a host specified as an IP address, this function must be used. */
    failf(data, "Failed to set SNI");
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

#ifdef HAS_ALPN_MBEDTLS
  જો(connssl->alpn) {
    સંરચના alpn_proto_buf proto;
    size_t i;

    માટે(i = 0; i < connssl->alpn->ગણતરી; ++i) {
      backend->protocols[i] = connssl->alpn->entries[i];
    }
    /* this function does not clone the protocols array, which is why we need
       to keep it around */
    જો(mbedtls_ssl_conf_alpn_protocols(&backend->config,
                                       &backend->protocols[0])) {
      failf(data, "Failed setting ALPN protocols");
      પાછા CURLE_SSL_CONNECT_ERROR;
    }
    Curl_alpn_to_proto_str(&proto, connssl->alpn);
    infof(data, VTLS_INFOF_ALPN_OFFER_1STR, proto.data);
  }
#endif

  /* give application a chance to interfere with mbedTLS set up. */
  જો(data->set.ssl_fsslctx) {
    result = (*data->set.ssl_fsslctx)(data, &backend->config,
                                      data->set.ssl_fsslctxp);
    જો(result)
      failf(data, "error signaled by SSL ctx callback");
  }

  પાછા result;
}

સ્થિર CURLcode mbed_connect_step1(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;

  જો((conn_config->version == CURL_SSLVERSION_SSLv2) ||
     (conn_config->version == CURL_SSLVERSION_SSLv3)) {
    failf(data, "Not supported SSL version");
    પાછા CURLE_NOT_BUILT_IN;
  }

  result = mbed_load_cacert(cf, data);
  જો(!result)
    result = mbed_load_clicert(cf, data);
  જો(!result)
    result = mbed_load_privkey(cf, data);
  જો(!result)
    result = mbed_load_crl(cf, data);
  જો(!result)
    result = mbed_configure_ssl(cf, data);
  જો(result)
    પાછા result;

  connssl->connecting_state = ssl_connect_2;
  પાછા CURLE_OK;
}

#જો defined(MBEDTLS_PK_WRITE_C) && defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
#define HAVE_PINNED_PUBKEY
#endif

સ્થિર CURLcode mbed_connect_step2(સંરચના Curl_cfilter *cf,
                                   સંરચના Curl_easy *data)
{
#જો defined(HAVE_PINNED_PUBKEY) || defined(HAS_ALPN_MBEDTLS)
  CURLcode result;
#endif
  પૂર્ણાંક ret;
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
#ifdef HAVE_PINNED_PUBKEY
#ifndef CURL_DISABLE_PROXY
  અચલ અક્ષર * અચલ pinnedpubkey = Curl_ssl_cf_is_proxy(cf) ?
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY_PROXY) :
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#નહીંતર
  અચલ અક્ષર * અચલ pinnedpubkey =
    CURL_EASY_STR(data, STRING_SSL_PINNEDPUBLICKEY);
#endif
#endif

  DEBUGASSERT(backend);

  ret = mbedtls_ssl_handshake(&backend->ssl);

  જો(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
    પાછા CURLE_OK;
  }
  નહીંતર જો(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
    પાછા CURLE_OK;
  }
  નહીંતર જો(ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
    failf(data, "peer certificate could not be verified");
    પાછા CURLE_PEER_FAILED_VERIFICATION;
  }
  નહીંતર જો(ret) {
    અક્ષર errorbuf[128];
    CURL_TRC_CF(data, cf, "TLS version %04X",
                mbedtls_ssl_get_version_સંખ્યા(&backend->ssl));
    mbedtls_strerror(ret, errorbuf, કદ(errorbuf));
    failf(data, "ssl_handshake returned: (-0x%04X) %s",
          (unsigned પૂર્ણાંક)-ret, errorbuf);
    પાછા CURLE_SSL_CONNECT_ERROR;
  }

  {
    અક્ષર cipher_str[64];
    uint16_t cipher_id;
    cipher_id =
      (uint16_t)mbedtls_ssl_get_ciphersuite_id_from_ssl(&backend->ssl);
    mbed_cipher_suite_get_str(cipher_id, cipher_str, કદ(cipher_str), TRUE);
    infof(data, "mbedTLS: %s Handshake complete, cipher is %s",
          mbedtls_ssl_get_version(&backend->ssl), cipher_str);
  }

#ifdef HAVE_PINNED_PUBKEY
  જો(pinnedpubkey) {
    પૂર્ણાંક size;
    અચલ mbedtls_x509_crt *peercert;
    mbedtls_x509_crt *p = ખાલી;
    unsigned અક્ષર *pubkey = ખાલી;

    peercert = mbedtls_ssl_get_peer_cert(&backend->ssl);
    જો(!peercert || !peercert->raw.p || !peercert->raw.len) {
      failf(data, "Failed due to missing peer certificate");
      પાછા CURLE_SSL_PINNEDPUBKEYNOTMATCH;
    }

    p = curlx_calloc(1, કદ(*p));

    જો(!p)
      પાછા CURLE_OUT_OF_MEMORY;

    pubkey = curlx_malloc(PUB_DER_MAX_BYTES);

    જો(!pubkey) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ pinnedpubkey_error;
    }

    mbedtls_x509_crt_init(p);

    /* Make a copy of our const peercert because mbedtls_pk_write_pubkey_der
       needs a non-const key, for now.
       https://github.com/Mbed-TLS/mbedtls/issues/396 */
    જો(mbedtls_x509_crt_parse_der(p, peercert->raw.p, peercert->raw.len)) {
      failf(data, "Failed copying peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      જાઓ pinnedpubkey_error;
    }

    size = mbedtls_pk_write_pubkey_der(&p->pk, pubkey, PUB_DER_MAX_BYTES);

    જો(size <= 0) {
      failf(data, "Failed copying public key from peer certificate");
      result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;
      જાઓ pinnedpubkey_error;
    }

    /* mbedtls_pk_write_pubkey_der writes data at the end of the buffer. */
    result = Curl_pin_peer_pubkey(data,
                                  pinnedpubkey,
                                  &pubkey[PUB_DER_MAX_BYTES - size], size);
pinnedpubkey_error:
    mbedtls_x509_crt_free(p);
    curlx_free(p);
    curlx_free(pubkey);
    જો(result)
      પાછા result;
  }
#endif

#ifdef HAS_ALPN_MBEDTLS
  જો(connssl->alpn) {
    અચલ અક્ષર *proto = mbedtls_ssl_get_alpn_protocol(&backend->ssl);

    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (અચલ unsigned અક્ષર *)proto,
                                      proto ? strlen(proto) : 0);
    જો(result)
      પાછા result;
  }
#endif

  connssl->connecting_state = ssl_connect_3;
  infof(data, "SSL connected");

  પાછા CURLE_OK;
}

સ્થિર CURLcode mbed_new_session(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  mbedtls_ssl_session session;
  bool msession_alloced = FALSE;
  સંરચના Curl_ssl_session *sc_session = ખાલી;
  unsigned અક્ષર *sdata = ખાલી;
  size_t slen = 0;
  પૂર્ણાંક ietf_tls_id;
  CURLcode result = CURLE_OK;
  પૂર્ણાંક ret;

  DEBUGASSERT(backend);
  જો(!Curl_ssl_scache_use(cf, data))
    પાછા CURLE_OK;

  mbedtls_ssl_session_init(&session);
  ret = mbedtls_ssl_get_session(&backend->ssl, &session);
  msession_alloced = (ret != MBEDTLS_ERR_SSL_ALLOC_FAILED);
  જો(ret) {
    failf(data, "mbedtls_ssl_get_session returned -0x%x", (unsigned પૂર્ણાંક)-ret);
    result = CURLE_SSL_CONNECT_ERROR;
    જાઓ out;
  }

  mbedtls_ssl_session_save(&session, ખાલી, 0, &slen);
  જો(!slen) {
    failf(data, "failed to serialize session: length is 0");
    જાઓ out;
  }

  sdata = curlx_malloc(slen);
  જો(!sdata) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  ret = mbedtls_ssl_session_save(&session, sdata, slen, &slen);
  જો(ret) {
    failf(data, "failed to serialize session: -0x%x", (unsigned પૂર્ણાંક)-ret);
    જાઓ out;
  }

  ietf_tls_id = mbedtls_ssl_get_version_સંખ્યા(&backend->ssl);
  result = Curl_ssl_session_create(sdata, slen,
                                   ietf_tls_id,
                                   connssl->negotiated.alpn, 0, 0,
                                   &sc_session);
  sdata = ખાલી;  /* call took ownership */
  જો(!result && /* return a duplicate if asked for and FTP */
     (cf->conn->scheme->family == CURLPROTO_FTP)) {
    Curl_ssl_session_destroy(connssl->session);
    result = Curl_ssl_session_dup(sc_session, &connssl->session);
  }

  જો(!result) {
    result = Curl_ssl_scache_put(cf, data, connssl->peer.scache_ચાવી,
                                 sc_session);
    sc_session = ખાલી;
  }

out:
  જો(msession_alloced)
    mbedtls_ssl_session_free(&session);
  Curl_ssl_session_destroy(sc_session);
  curlx_free(sdata);
  પાછા result;
}

સ્થિર CURLcode mbed_send(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                          અચલ શૂન્ય *mem, size_t len, size_t *pnwritten)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  પૂર્ણાંક nwritten;

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
  જો(backend->send_blocked) {
    જો(backend->send_blocked_len > len) {
      DEBUGASSERT(0);
      failf(data, "re-send data shorter than previously");
      result = CURLE_BAD_FUNCTION_ARGUMENT;
      જાઓ out;
    }
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> previously blocked "
                "on %zu bytes", len, backend->send_blocked_len);
    len = backend->send_blocked_len;
  }

  nwritten = mbedtls_ssl_write(&backend->ssl, (અચલ unsigned અક્ષર *)mem, len);

  જો(nwritten >= 0) {
    *pnwritten = (size_t)nwritten;
    backend->send_blocked = FALSE;
  }
  નહીંતર {
    CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> -0x%04X",
                len, (unsigned પૂર્ણાંક)-nwritten);
    પસંદગી(nwritten) {
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    સ્થિતિ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This return code is not blocking. Having treated the new
       * ticket, resume sending until we get a "real" result. */
      જો(!result)
        જાઓ do_send;
      તોડો;
#endif
    સ્થિતિ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      તોડો;
    સ્થિતિ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      તોડો;
    મૂળભૂત:
      result = CURLE_SEND_ERROR;
      તોડો;
    }
    જો((result == CURLE_AGAIN) && !backend->send_blocked) {
      backend->send_blocked = TRUE;
      backend->send_blocked_len = len;
    }
  }

out:
  CURL_TRC_CF(data, cf, "mbedtls_ssl_write(len=%zu) -> %d, %zu",
              len, (પૂર્ણાંક)result, *pnwritten);
  પાછા result;
}

સ્થિર CURLcode mbedtls_shutdown(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 bool send_shutdown, bool *done)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  unsigned અક્ષર buf[1024];
  CURLcode result = CURLE_OK;
  પૂર્ણાંક ret = 0;
  size_t i;

  DEBUGASSERT(backend);

  જો(!backend->initialized || cf->shutdown) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  connssl->io_need = CURL_SSL_IO_NEED_NONE;
  *done = FALSE;

  જો(!backend->sent_shutdown) {
    /* do this only once */
    backend->sent_shutdown = TRUE;
    જો(send_shutdown) {
      ret = mbedtls_ssl_close_notify(&backend->ssl);
      પસંદગી(ret) {
      સ્થિતિ 0: /* we sent it, receive from the server */
        તોડો;
      સ્થિતિ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY: /* server also closed */
        *done = TRUE;
        જાઓ out;
      સ્થિતિ MBEDTLS_ERR_SSL_WANT_READ:
        connssl->io_need = CURL_SSL_IO_NEED_RECV;
        જાઓ out;
      સ્થિતિ MBEDTLS_ERR_SSL_WANT_WRITE:
        connssl->io_need = CURL_SSL_IO_NEED_SEND;
        જાઓ out;
      મૂળભૂત:
        CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                    (unsigned પૂર્ણાંક)-ret);
        result = CURLE_RECV_ERROR;
        જાઓ out;
      }
    }
  }

  /* SSL should now have started the shutdown from our side. Since it
   * was not complete, we are lacking the close notify from the server. */
  માટે(i = 0; i < 10; ++i) {
    ret = mbedtls_ssl_read(&backend->ssl, buf, કદ(buf));
    /* This seems to be a bug in mbedTLS TLSv1.3 where it reports
     * WANT_READ, but has not encountered an EAGAIN. */
    જો(ret == MBEDTLS_ERR_SSL_WANT_READ)
      ret = mbedtls_ssl_read(&backend->ssl, buf, કદ(buf));
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    જો(ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET)
      ચાલુ;
#endif
    જો(ret <= 0)
      તોડો;
  }

  જો(ret > 0) {
    /* still data coming in? */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, still getting data");
  }
  નહીંતર જો(ret == 0 || (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)) {
    /* We got the close notify alert and are done. */
    CURL_TRC_CF(data, cf, "mbedtls_shutdown done");
    *done = TRUE;
  }
  નહીંતર જો(ret == MBEDTLS_ERR_SSL_WANT_READ) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need RECV");
    connssl->io_need = CURL_SSL_IO_NEED_RECV;
  }
  નહીંતર જો(ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown, need SEND");
    connssl->io_need = CURL_SSL_IO_NEED_SEND;
  }
  નહીંતર {
    CURL_TRC_CF(data, cf, "mbedtls_shutdown error -0x%04X",
                (unsigned પૂર્ણાંક)-ret);
    result = CURLE_RECV_ERROR;
  }

out:
  cf->shutdown = (result || *done);
  પાછા result;
}

સ્થિર શૂન્ય mbedtls_close(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;

  (શૂન્ય)data;
  DEBUGASSERT(backend);
  mbedtls_pk_free(&backend->pk);
  mbedtls_x509_crt_free(&backend->clicert);
  mbedtls_x509_crt_free(&backend->cacert);
#ifdef MBEDTLS_X509_CRL_PARSE_C
  mbedtls_x509_crl_free(&backend->crl);
#endif
  curlx_safefree(backend->ciphersuites);
  mbedtls_ssl_config_free(&backend->config);
  જો(backend->initialized) {
    mbedtls_ssl_free(&backend->ssl);
    backend->initialized = FALSE;
  }
}

સ્થિર CURLcode mbed_recv(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                          અક્ષર *buf, size_t buffersize, size_t *pnread)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  CURLcode result = CURLE_OK;
  પૂર્ણાંક nread;

  DEBUGASSERT(backend);
#ifdef MBEDTLS_SSL_SESSION_TICKETS
do_read:
#endif
  *pnread = 0;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  nread = mbedtls_ssl_read(&backend->ssl, (unsigned અક્ષર *)buf, buffersize);
  જો(nread > 0)
    *pnread = (size_t)nread;
  નહીંતર {
    અક્ષર errorbuf[128];
    CURL_TRC_CF(data, cf, "mbedtls_ssl_read(len=%zu) -> -0x%04X",
                buffersize, (unsigned પૂર્ણાંક)-nread);
    પસંદગી(nread) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
    સ્થિતિ MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
      result = mbed_new_session(cf, data);
      /* This is not blocking anything. We can try again until a
       * "real" result comes. */
      જો(!result)
        જાઓ do_read;
      તોડો;
#endif
    સ્થિતિ MBEDTLS_ERR_SSL_WANT_READ:
      connssl->io_need = CURL_SSL_IO_NEED_RECV;
      result = CURLE_AGAIN;
      તોડો;
    સ્થિતિ MBEDTLS_ERR_SSL_WANT_WRITE:
      connssl->io_need = CURL_SSL_IO_NEED_SEND;
      result = CURLE_AGAIN;
      તોડો;
    સ્થિતિ MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
      result = CURLE_OK;
      તોડો;
    મૂળભૂત:
      mbedtls_strerror(nread, errorbuf, કદ(errorbuf));
      failf(data, "ssl_read returned: (-0x%04X) %s", (unsigned પૂર્ણાંક)-nread,
            errorbuf);
      result = CURLE_RECV_ERROR;
      તોડો;
    }
  }
  પાછા result;
}

સ્થિર size_t mbedtls_version(અક્ષર *buffer, size_t size)
{
#ifdef MBEDTLS_VERSION_C
  /* if mbedtls_version_get_number() is available it is better */
  unsigned પૂર્ણાંક version = mbedtls_version_get_સંખ્યા();
  પાછા curl_msnprintf(buffer, size, "mbedTLS/%u.%u.%u", version >> 24,
                        (version >> 16) & 0xff, (version >> 8) & 0xff);
#નહીંતર
  પાછા curl_msnprintf(buffer, size, "mbedTLS/%s", MBEDTLS_VERSION_STRING);
#endif
}

/* 'data' might be NULL */
સ્થિર CURLcode mbedtls_random(સંરચના Curl_easy *data,
                               unsigned અક્ષર *entropy, size_t length)
{
  psa_status_t status;
  (શૂન્ય)data;

  status = psa_generate_random(entropy, length);

  પાછા status == PSA_SUCCESS ? CURLE_OK : CURLE_FAILED_INIT;
}

સ્થિર CURLcode mbedtls_connect(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                bool *done)
{
  CURLcode result;
  સંરચના ssl_connect_data *connssl = cf->ctx;

  /* check if the connection has already been established */
  જો(ssl_connection_complete == connssl->state) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  *done = FALSE;
  connssl->io_need = CURL_SSL_IO_NEED_NONE;

  જો(connssl->connecting_state == ssl_connect_1) {
    result = mbed_connect_step1(cf, data);
    જો(result)
      પાછા result;
  }

  જો(connssl->connecting_state == ssl_connect_2) {
    result = mbed_connect_step2(cf, data);
    જો(result)
      પાછા result;
  }

  જો(connssl->connecting_state == ssl_connect_3) {
    /* For tls1.3 we get notified about new sessions */
    સંરચના ssl_connect_data *ctx = cf->ctx;
    સંરચના mbed_ssl_backend_data *backend =
      (સંરચના mbed_ssl_backend_data *)ctx->backend;

    જો(mbedtls_ssl_get_version_સંખ્યા(&backend->ssl) <=
       MBEDTLS_SSL_VERSION_TLS1_2) {
      result = mbed_new_session(cf, data);
      જો(result)
        પાછા result;
    }
    connssl->connecting_state = ssl_connect_done;
  }

  જો(connssl->connecting_state == ssl_connect_done) {
    connssl->state = ssl_connection_complete;
    *done = TRUE;
  }

  પાછા CURLE_OK;
}

/*
 * return 0 error initializing SSL
 * return 1 SSL initialized successfully
 */
સ્થિર પૂર્ણાંક mbedtls_init(શૂન્ય)
{
#જો MBEDTLS_VERSION_સંખ્યા < 0x04000000
  પૂર્ણાંક ret = 0;
#endif
  psa_status_t status;
  status = psa_crypto_init();

  જો(status != PSA_SUCCESS)
    પાછા 0;

#જો MBEDTLS_VERSION_સંખ્યા < 0x04000000
  mbedtls_ctr_drbg_init(&rng.drbg);
  mbedtls_entropy_init(&rng.entropy);

  ret = mbedtls_ctr_drbg_seed(&rng.drbg, mbedtls_entropy_func, &rng.entropy,
                              ખાલી, 0);

  જો(ret)
    /* mbedtls_ctr_drbg_seed returned error */
    પાછા 0;

  /* To prevent an adversary from reading your random data,
     you can enable prediction resistance.

     Entropy is gathered before each mbedtls_ctr_drbg_random() call.
     Only use this if you have ample supply of good entropy.*/
  mbedtls_ctr_drbg_set_prediction_resistance(&rng.drbg,
                                             MBEDTLS_CTR_DRBG_PR_ON);
#endif
  પાછા 1;
}

સ્થિર શૂન્ય mbedtls_cleanup(શૂન્ય)
{
  mbedtls_psa_crypto_free();

#જો MBEDTLS_VERSION_સંખ્યા < 0x04000000
  mbedtls_ctr_drbg_free(&rng.drbg);
  mbedtls_entropy_free(&rng.entropy);
#endif
}

સ્થિર bool mbedtls_data_pending(સંરચના Curl_cfilter *cf,
                                 અચલ સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *ctx = cf->ctx;
  સંરચના mbed_ssl_backend_data *backend;

  (શૂન્ય)data;
  DEBUGASSERT(ctx && ctx->backend);
  backend = (સંરચના mbed_ssl_backend_data *)ctx->backend;
  પાછા mbedtls_ssl_get_bytes_avail(&backend->ssl) != 0;
}

સ્થિર CURLcode mbedtls_sha256sum(અચલ unsigned અક્ષર *આગત,
                                  size_t inputlen,
                                  unsigned અક્ષર *sha256sum,
                                  size_t sha256len)
{
#જો defined(PSA_WANT_ALG_SHA_256) && PSA_WANT_ALG_SHA_256
  psa_status_t status;
  size_t sha256len_actual;
  status = psa_hash_compute(PSA_ALG_SHA_256, આગત, inputlen,
                            sha256sum, sha256len,
                            &sha256len_actual);
  જો(status != PSA_SUCCESS)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  પાછા CURLE_OK;
#નહીંતર
  (શૂન્ય)sha256len;
  પાછા Curl_sha256it(sha256sum, આગત, inputlen);
#endif
}

સ્થિર શૂન્ય *mbedtls_get_internals(સંરચના ssl_connect_data *connssl,
                                   CURLINFO info)
{
  સંરચના mbed_ssl_backend_data *backend =
    (સંરચના mbed_ssl_backend_data *)connssl->backend;
  (શૂન્ય)info;
  DEBUGASSERT(backend);
  પાછા &backend->ssl;
}

અચલ સંરચના Curl_ssl Curl_ssl_mbedtls = {
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

  કદ(સંરચના mbed_ssl_backend_data),

  mbedtls_init,                     /* init */
  mbedtls_cleanup,                  /* cleanup */
  mbedtls_version,                  /* version */
  mbedtls_shutdown,                 /* shutdown */
  mbedtls_data_pending,             /* data_pending */
  mbedtls_random,                   /* random */
  ખાલી,                             /* cert_status_request */
  mbedtls_connect,                  /* connect */
  Curl_ssl_adjust_pollset,          /* adjust_pollset */
  mbedtls_get_internals,            /* get_internals */
  mbedtls_close,                    /* close_one */
  ખાલી,                             /* close_all */
  ખાલી,                             /* engines_list */
  mbedtls_sha256sum,                /* sha256sum */
  mbed_recv,                        /* recv decrypted data */
  mbed_send,                        /* send data to encrypt */
  ખાલી,                             /* get_channel_binding */
};

#endif /* USE_MBEDTLS */
