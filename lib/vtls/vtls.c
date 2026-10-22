/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
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

/* This file is for implementing all "generic" SSL functions that all libcurl
   internals should use. It is then responsible for calling the proper
   "backend" function.

   SSL-functions in libcurl should call functions in this source file, and not
   to any specific SSL-layer.

   Curl_ssl_ - prefix for generic ones

   Note that this source code uses the functions of the configured SSL
   backend via the global Curl_ssl instance.

   "SSL/TLS Strong Encryption: An Introduction"
   https://httpd.apache.org/docs/2.0/ssl/ssl_intro.html
 */

#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"

#ಅನ್ತರ್ಭಾವ "vtls/vtls.h" /* generic SSL protos etc */
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vtls/keylog.h"

#ಅನ್ತರ್ಭಾವ "vtls/openssl.h"        /* OpenSSL versions */
#ಅನ್ತರ್ಭಾವ "vtls/gtls.h"           /* GnuTLS versions */
#ಅನ್ತರ್ಭಾವ "vtls/wolfssl.h"        /* wolfSSL versions */
#ಅನ್ತರ್ಭಾವ "vtls/schannel.h"       /* Schannel SSPI version */
#ಅನ್ತರ್ಭಾವ "vtls/mbedtls.h"        /* mbedTLS versions */
#ಅನ್ತರ್ಭಾವ "vtls/rustls.h"         /* Rustls versions */

#ಅನ್ತರ್ಭಾವ "slist.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "curlx/win32-fopen.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"
#ಅನ್ತರ್ಭಾವ "curlx/base64.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "setopt.h"
#ಅನ್ತರ್ಭಾವ "vdns/cf-dns.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

#ifdef USE_APPLE_SECTRUST
#ಅನ್ತರ್ಭಾವ <Security/Security.h>
#endif

#ifdef USE_SSL
#ಯದಿ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  ಯದಿ(!use_alpn)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  ಯದಿ(only_http_10 && (wanted & CURL_HTTP_V1x))
    ಪ್ರತ್ಯಾಗಮ &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  ಯದಿ(wanted & CURL_HTTP_V2x) {
    ಯದಿ(wanted & CURL_HTTP_V1x)
      ಪ್ರತ್ಯಾಗಮ (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    ಪ್ರತ್ಯಾಗಮ &ALPN_SPEC_H2;
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)wanted;
  (ಶೂನ್ಯ)preferred;
#endif
  ಪ್ರತ್ಯಾಗಮ &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ multissl_setup(ಧ್ರುವ ಸಂರಚನಾ Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(ಶೂನ್ಯ)
{
#ifdef USE_SSL
  multissl_setup(ರಿಕ್ತ);
  ಪ್ರತ್ಯಾಗಮ Curl_ssl->info.id;
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
ಸ್ಥಿರ bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ಪೂರ್ಣಾಙ್ಕ Curl_ssl_init(ಶೂನ್ಯ)
{
  /* make sure this is only done once */
  ಯದಿ(init_ssl)
    ಪ್ರತ್ಯಾಗಮ 1;
  init_ssl = TRUE; /* never again */

  ಯದಿ(Curl_ssl->init)
    ಪ್ರತ್ಯಾಗಮ Curl_ssl->init();
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ bool ssl_prefs_check(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  ಯದಿ(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  ವಿಕಲ್ಪ(conn_config->version_max) {
  ಪಕ್ಷ CURL_SSLVERSION_MAX_NONE:
  ಪಕ್ಷ CURL_SSLVERSION_MAX_DEFAULT:
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    ಯದಿ((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      ಪ್ರತ್ಯಾಗಮ FALSE;
    }
  }

  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ ಸಂರಚನಾ ssl_connect_data *cf_ctx_new(ಸಂರಚನಾ Curl_easy *data,
                                           ಧ್ರುವ ಸಂರಚನಾ alpn_spec *alpn)
{
  ಸಂರಚನಾ ssl_connect_data *ctx;

  (ಶೂನ್ಯ)data;
  ctx = curlx_calloc(1, ಪರಿಮಾಣ(*ctx));
  ಯದಿ(!ctx)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  ಯದಿ(!ctx->backend) {
    curlx_free(ctx);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ ctx;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ctx_free(ಸಂರಚನಾ ssl_connect_data *ctx)
{
  ಯದಿ(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(ಸಂರಚನಾ Curl_easy *data, int8_t sockindex,
                                      ಸಂರಚನಾ dynbuf *binding)
{
  ಯದಿ(Curl_ssl->get_channel_binding)
    ಪ್ರತ್ಯಾಗಮ Curl_ssl->get_channel_binding(data, sockindex, binding);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssl_close_all(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;

  ಯದಿ(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    ಯದಿ(sock != CURL_SOCKET_BAD) {
      ಯದಿ(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      ಅನ್ಯಥಾ {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
ಸಂರಚನಾ curl_slist *Curl_ssl_engines_list(ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(Curl_ssl->engines_list)
    ಪ್ರತ್ಯಾಗಮ Curl_ssl->engines_list(data);
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ size_t multissl_version(ವರ್ಣ *buffer, size_t size);

ಶೂನ್ಯ Curl_ssl_version(ವರ್ಣ *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (ಶೂನ್ಯ)multissl_version(buffer, size);
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)Curl_ssl->version(buffer, size);
#endif
}

ಶೂನ್ಯ Curl_ssl_free_certinfo(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ curl_certinfo *ci = &data->info.certs;

  ಯದಿ(ci->ಅಂಕೆ_of_certs) {
    /* free all individual lists used */
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ(i = 0; i < ci->ಅಂಕೆ_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = ರಿಕ್ತ;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->ಅಂಕೆ_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ)
{
  ಸಂರಚನಾ curl_certinfo *ci = &data->info.certs;
  ಸಂರಚನಾ curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)ಅಂಕೆ, ಪರಿಮಾಣ(ಸಂರಚನಾ curl_slist *));
  ಯದಿ(!table)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  ci->ಅಂಕೆ_of_certs = ಅಂಕೆ;
  ci->certinfo = table;

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ತಳ್ಳು_certinfo_len(ಸಂರಚನಾ Curl_easy *data,
                                    ಪೂರ್ಣಾಙ್ಕ certnum,
                                    ಧ್ರುವ ವರ್ಣ *label,
                                    ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ,
                                    size_t valuelen)
{
  ಸಂರಚನಾ curl_certinfo *ci = &data->info.certs;
  ಸಂರಚನಾ curl_slist *nl;
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ dynbuf build;

  DEBUGASSERT(certnum < ci->ಅಂಕೆ_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  ಯದಿ(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, ಮೌಲ್ಯ, valuelen))
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  ಯದಿ(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  ಪ್ರತ್ಯಾಗಮ result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(ಸಂರಚನಾ Curl_easy *data,
                         unsigned ವರ್ಣ *buffer, size_t length)
{
  DEBUGASSERT(length == ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
  ಯದಿ(Curl_ssl->random)
    ಪ್ರತ್ಯಾಗಮ Curl_ssl->random(data, buffer, length);
  ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

ಸ್ಥಿರ CURLcode pubkey_pem_to_der(ಧ್ರುವ ವರ್ಣ *pem,
                                  unsigned ವರ್ಣ **der, size_t *der_len)
{
  ಧ್ರುವ ವರ್ಣ *begin_pos, *end_pos;
  size_t pem_ಎಣಿಕೆ, pem_len;
  CURLcode result;
  ಸಂರಚನಾ dynbuf pbuf;

  /* if no pem, exit. */
  ಯದಿ(!pem)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  ಯದಿ(!begin_pos)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  pem_ಎಣಿಕೆ = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  ಯದಿ(pem_ಎಣಿಕೆ && '\n' != pem[pem_ಎಣಿಕೆ - 1])
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_ಎಣಿಕೆ += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_ಎಣಿಕೆ, "\n-----END PUBLIC KEY-----");
  ಯದಿ(!end_pos)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  ಯಾವತ್(pem_ಎಣಿಕೆ < pem_len) {
    ಯದಿ('\n' != pem[pem_ಎಣಿಕೆ] && '\r' != pem[pem_ಎಣಿಕೆ]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_ಎಣಿಕೆ], 1);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ result;
    }
    ++pem_ಎಣಿಕೆ;
  }

  ಯದಿ(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  ಅನ್ಯಥಾ
    result = CURLE_BAD_CONTENT_ENCODING;

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *pinnedpubkey,
                              ಧ್ರುವ unsigned ವರ್ಣ *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ಯದಿ(!pinnedpubkey)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಯದಿ(!pubkey || !pubkeylen)
    ಪ್ರತ್ಯಾಗಮ result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  ಯದಿ(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    ವರ್ಣ *cert_hash = ರಿಕ್ತ;
    ಧ್ರುವ ವರ್ಣ *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned ವರ್ಣ *sha256sumdigest;

    ಯದಿ(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      ಪ್ರತ್ಯಾಗಮ result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    ಯದಿ(!sha256sumdigest)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    ಯದಿ(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    ಯದಿ(encode)
      ಪ್ರತ್ಯಾಗಮ encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    ಯಾವತ್(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      ಯದಿ(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        ಭಙ್ಗ;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (ಪೂರ್ಣಾಙ್ಕ)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : ರಿಕ್ತ;
    }
    curlx_safefree(cert_hash);
  }
  ಅನ್ಯಥಾ {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    ಸಂರಚನಾ dynbuf buf;
    unsigned ವರ್ಣ *pem_ptr = ರಿಕ್ತ;
    size_t ಎಡ;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    ಯದಿ(!fp)
      ಪ್ರತ್ಯಾಗಮ result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    ಯದಿ(fseek(fp, 0, SEEK_END))
      ಗಚ್ಛ end;
    filesize = ftell(fp);
    ಯದಿ(fseek(fp, 0, SEEK_SET))
      ಗಚ್ಛ end;
    ಯದಿ(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      ಗಚ್ಛ end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    ಯದಿ(pubkeylen > size)
      ಗಚ್ಛ end;

    /*
     * Read the file into the dynbuf
     */
    ಎಡ = size;
    ಕುರು {
      ವರ್ಣ buffer[1024];
      size_t want = ಎಡ > ಪರಿಮಾಣ(buffer) ? ಪರಿಮಾಣ(buffer) : ಎಡ;
      ಯದಿ(fread(buffer, 1, want, fp) != want)
        ಗಚ್ಛ end;
      ಯದಿ(curlx_dyn_addn(&buf, buffer, want))
        ಗಚ್ಛ end;
      ಎಡ -= want;
    } ಯಾವತ್(ಎಡ);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    ಯದಿ(pubkeylen == size) {
      ಯದಿ(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      ಗಚ್ಛ end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    ಯದಿ(pem_read)
      ಗಚ್ಛ end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    ಯದಿ(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(ಶೂನ್ಯ)
{
  ಯದಿ(Curl_ssl->cert_status_request)
    ಪ್ರತ್ಯಾಗಮ Curl_ssl->cert_status_request();
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ multissl_init(ಶೂನ್ಯ)
{
  ಯದಿ(multissl_setup(ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ 1;
  ಯದಿ(Curl_ssl->init)
    ಪ್ರತ್ಯಾಗಮ Curl_ssl->init();
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ CURLcode multissl_random(ಸಂರಚನಾ Curl_easy *data,
                                unsigned ವರ್ಣ *entropy, size_t length)
{
  ಯದಿ(multissl_setup(ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಪ್ರತ್ಯಾಗಮ Curl_ssl->random(data, entropy, length);
}

ಸ್ಥಿರ CURLcode multissl_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ಯದಿ(multissl_setup(ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಪ್ರತ್ಯಾಗಮ Curl_ssl->do_connect(cf, data, done);
}

ಸ್ಥಿರ CURLcode multissl_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ easy_pollset *ps)
{
  ಯದಿ(multissl_setup(ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪ್ರತ್ಯಾಗಮ Curl_ssl->adjust_pollset(cf, data, ps);
}

ಸ್ಥಿರ ಶೂನ್ಯ *multissl_get_internals(ಸಂರಚನಾ ssl_connect_data *connssl,
                                    CURLINFO info)
{
  ಯದಿ(multissl_setup(ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ Curl_ssl->get_internals(connssl, info);
}

ಸ್ಥಿರ ಶೂನ್ಯ multissl_close(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(multissl_setup(ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ;
  Curl_ssl->close(cf, data);
}

ಸ್ಥಿರ CURLcode multissl_recv_plain(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ವರ್ಣ *buf, size_t len, size_t *pnread)
{
  ಯದಿ(multissl_setup(ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಪ್ರತ್ಯಾಗಮ Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

ಸ್ಥಿರ CURLcode multissl_send_plain(ಸಂರಚನಾ Curl_cfilter *cf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಧ್ರುವ ಶೂನ್ಯ *mem, size_t len,
                                    size_t *pnwritten)
{
  ಯದಿ(multissl_setup(ರಿಕ್ತ))
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  ಪ್ರತ್ಯಾಗಮ Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  ರಿಕ್ತ,                              /* cleanup */
  multissl_version,                  /* version */
  ರಿಕ್ತ,                              /* shutdown */
  ರಿಕ್ತ,                              /* data_pending */
  multissl_random,                   /* random */
  ರಿಕ್ತ,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  ರಿಕ್ತ,                              /* close_all */
  ರಿಕ್ತ,                              /* engines_list */
  ರಿಕ್ತ,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  ರಿಕ್ತ,                              /* get_channel_binding */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#ನೋಚೇದ್ಯದಿ defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#ನೋಚೇದ್ಯದಿ defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#ನೋಚೇದ್ಯದಿ defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#ನೋಚೇದ್ಯದಿ defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#ನೋಚೇದ್ಯದಿ defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#ನೋಚೇದ್ಯದಿ defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#ಅನ್ಯಥಾ
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_ssl *available_backends[] = {
#ifdef USE_WOLFSSL
  &Curl_ssl_wolfssl,
#endif
#ifdef USE_GNUTLS
  &Curl_ssl_gnutls,
#endif
#ifdef USE_MBEDTLS
  &Curl_ssl_mbedtls,
#endif
#ifdef USE_OPENSSL
  &Curl_ssl_openssl,
#endif
#ifdef USE_SCHANNEL
  &Curl_ssl_schannel,
#endif
#ifdef USE_RUSTLS
  &Curl_ssl_rustls,
#endif
  ರಿಕ್ತ
};

/* Global cleanup */
ಶೂನ್ಯ Curl_ssl_cleanup(ಶೂನ್ಯ)
{
  ಯದಿ(init_ssl) {
    /* only cleanup if we did a previous init */
    ಯದಿ(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

ಸ್ಥಿರ size_t multissl_version(ವರ್ಣ *buffer, size_t size)
{
  ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_ssl *selected;
  ಸ್ಥಿರ ವರ್ಣ backends[200];
  ಸ್ಥಿರ size_t backends_len;
  ಧ್ರುವ ಸಂರಚನಾ Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  ಯದಿ(current != selected) {
    ವರ್ಣ *p = backends;
    ಧ್ರುವ ವರ್ಣ *end = backends + ಪರಿಮಾಣ(backends);
    ಪೂರ್ಣಾಙ್ಕ i;

    selected = current;

    backends[0] = '\0';

    ಪ್ರತಿ(i = 0; available_backends[i]; ++i) {
      ವರ್ಣ vb[200];
      bool paren = (selected != available_backends[i]);

      ಯದಿ(available_backends[i]->version(vb, ಪರಿಮಾಣ(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  ಯದಿ(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ multissl_setup(ಧ್ರುವ ಸಂರಚನಾ Curl_ssl *backend)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  ವರ್ಣ *env;

  ಯದಿ(Curl_ssl != &Curl_ssl_multi)
    ಪ್ರತ್ಯಾಗಮ 1;

  ಯದಿ(backend) {
    Curl_ssl = backend;
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(!available_backends[0])
    ಪ್ರತ್ಯಾಗಮ 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  ಯದಿ(env) {
    ಪ್ರತಿ(i = 0; available_backends[i]; i++) {
      ಯದಿ(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        ಪ್ರತ್ಯಾಗಮ 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  ಪ್ರತಿ(i = 0; available_backends[i]; i++) {
    ಯದಿ(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ಧ್ರುವ ವರ್ಣ *name,
                                   ಧ್ರುವ curl_ssl_backend ***avail)
{
  ಪೂರ್ಣಾಙ್ಕ i;

  ಯದಿ(avail)
    *avail = (ಧ್ರುವ curl_ssl_backend **)&available_backends;

  ಯದಿ(Curl_ssl != &Curl_ssl_multi)
    ಪ್ರತ್ಯಾಗಮ id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#ಅನ್ಯಥಾ
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  ಪ್ರತಿ(i = 0; available_backends[i]; i++) {
    ಯದಿ(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      ಪ್ರತ್ಯಾಗಮ CURLSSLSET_OK;
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLSSLSET_UNKNOWN_BACKEND;
}

#ಅನ್ಯಥಾ /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ಧ್ರುವ ವರ್ಣ *name,
                                   ಧ್ರುವ curl_ssl_backend ***avail)
{
  (ಶೂನ್ಯ)id;
  (ಶೂನ್ಯ)name;
  (ಶೂನ್ಯ)avail;
  ಪ್ರತ್ಯಾಗಮ CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

ಶೂನ್ಯ Curl_ssl_peer_cleanup(ಸಂರಚನಾ ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_ಕೀಲಿ);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

ಸ್ಥಿರ ssl_peer_type get_peer_type(ಧ್ರುವ ವರ್ಣ *hostname)
{
  ಯದಿ(hostname && hostname[0]) {
#ifdef USE_IPV6
    ಸಂರಚನಾ in6_addr addr;
#ಅನ್ಯಥಾ
    ಸಂರಚನಾ in_addr addr;
#endif
    ಯದಿ(curlx_inet_pton(AF_INET, hostname, &addr))
      ಪ್ರತ್ಯಾಗಮ CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    ಅನ್ಯಥಾ ಯದಿ(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      ಪ್ರತ್ಯಾಗಮ CURL_SSL_PEER_IPV6;
    }
#endif
  }
  ಪ್ರತ್ಯಾಗಮ CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(ಸಂರಚನಾ ssl_peer *ssl_peer,
                            ಸಂರಚನಾ Curl_peer *origin,
                            ಸಂರಚನಾ Curl_peer *peer,
                            ಸಂರಚನಾ ssl_filter_config *sslc,
                            ಧ್ರುವ ವರ್ಣ *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  ಯದಿ(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  ಯದಿ(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    ಯದಿ(len && (origin->hostname[len - 1] == '.'))
      len--;
    ಯದಿ(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      ಯದಿ(!ssl_peer->sni)
        ಗಚ್ಛ out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_ಕೀಲಿ_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_ಕೀಲಿ);

out:
  ಯದಿ(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ ssl_cf_destroy(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಯದಿ(connssl) {
    connssl->ssl_impl->close(cf, data);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = ರಿಕ್ತ;
  }
  cf->connected = FALSE;
}

ಸ್ಥಿರ CURLcode ssl_cf_connect(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               bool *done)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result;

  ಯದಿ(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಯದಿ(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  ಯದಿ(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ಯದಿ(result || !*done)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  ಯದಿ(!connssl->prefs_checked) {
    ಯದಿ(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  ಯದಿ(!result && *done) {
    cf->connected = TRUE;
    ಯದಿ(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    ಯದಿ(Curl_tls_keylog_enabled()) {
      infof(data, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_ಸಂಖ್ಯೆ
      infof(data, "Note LibreSSL only supports SSLKEYLOGFILE for TLS <= 1.2");
#endif
    }
    /* Connection can be deferred when sending early data */
    DEBUGASSERT(connssl->state == ssl_connection_complete ||
                connssl->state == ssl_connection_deferred);
    DEBUGASSERT(connssl->state != ssl_connection_deferred ||
                connssl->earlydata_state > ssl_earlydata_none);
  }
out:
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (ಪೂರ್ಣಾಙ್ಕ)result, *done);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssl_cf_set_earlydata(ಸಂರಚನಾ Curl_cfilter *cf,
                                     ಸಂರಚನಾ Curl_easy *data,
                                     ಧ್ರುವ ಶೂನ್ಯ *buf, size_t blen)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  ಯದಿ(blen) {
    ಯದಿ(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssl_cf_connect_deferred(ಸಂರಚನಾ Curl_cfilter *cf,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಧ್ರುವ ಶೂನ್ಯ *buf, size_t blen,
                                        bool *done)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  ಯದಿ(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  ಯದಿ(!result && *done) {
    ಯದಿ(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    ವಿಕಲ್ಪ(connssl->earlydata_state) {
    ಪಕ್ಷ ssl_earlydata_none:
      ಭಙ್ಗ;
    ಪಕ್ಷ ssl_earlydata_accepted:
      ಯದಿ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      ಭಙ್ಗ;
    ಪಕ್ಷ ssl_earlydata_rejected:
      ಯದಿ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(ರಿಕ್ತ);
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ bool ssl_cf_data_pending(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  ಯದಿ(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  ಅನ್ಯಥಾ
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ pending;
}

ಸ್ಥಿರ CURLcode ssl_cf_send(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result = CURLE_OK;

  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  ಯದಿ(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ಅನ್ಯಥಾ ಯದಿ(!done) {
      result = CURLE_AGAIN;
      ಗಚ್ಛ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ಯದಿ(connssl->earlydata_skip) {
    ಯದಿ(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  ಯದಿ(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    ಯದಿ(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssl_cf_recv(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data, ವರ್ಣ *buf, size_t len,
                            size_t *pnread)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  ಯದಿ(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, ರಿಕ್ತ, 0, &done);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ಅನ್ಯಥಾ ಯದಿ(!done) {
      result = CURLE_AGAIN;
      ಗಚ್ಛ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssl_cf_shutdown(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                bool *done)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  ಯದಿ(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    ಸಂರಚನಾ cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (ಪೂರ್ಣಾಙ್ಕ)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssl_cf_adjust_pollset(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ easy_pollset *ps)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode ssl_cf_query(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಪೂರ್ಣಾಙ್ಕ query, ಪೂರ್ಣಾಙ್ಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;

  ವಿಕಲ್ಪ(query) {
  ಪಕ್ಷ CF_QUERY_SSL_INFO:
  ಪಕ್ಷ CF_QUERY_SSL_CTX_INFO:
    ಯದಿ(!Curl_ssl_cf_is_proxy(cf)) {
      ಸಂರಚನಾ curl_tlssessioninfo *info = pres2;
      ಸಂರಚನಾ cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_QUERY_ALPN_NEGOTIATED: {
    ಧ್ರುವ ವರ್ಣ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  ಸ್ವಭಾವ:
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode ssl_cf_cntrl(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಪೂರ್ಣಾಙ್ಕ event, ಪೂರ್ಣಾಙ್ಕ arg1, ಶೂನ್ಯ *arg2)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  (ಶೂನ್ಯ)data;
  ವಿಕಲ್ಪ(event) {
  ಪಕ್ಷ CF_CTRL_CONN_INFO_UPDATE:
    ಯದಿ(connssl->negotiated.alpn && !cf->sockindex) {
      ಯದಿ(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      ಅನ್ಯಥಾ ಯದಿ(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      ಅನ್ಯಥಾ ಯದಿ(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CF_CTRL_REPORT_STATS:
    ಯದಿ(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool cf_ssl_is_alive(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data,
                            bool *ಒಳಹರಿವು_pending)
{
  /*
   * This function tries to determine connection status.
   */
  ಪ್ರತ್ಯಾಗಮ cf->next ?
    cf->next->cft->is_alive(cf->next, data, ಒಳಹರಿವು_pending) :
    FALSE; /* pessimistic in absence of data */
}

ಸಂರಚನಾ Curl_cftype Curl_cft_ssl = {
  "SSL",
  CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_data_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#ifndef CURL_DISABLE_PROXY

ಸಂರಚನಾ Curl_cftype Curl_cft_ssl_proxy = {
  "SSL-PROXY",
  CF_TYPE_SSL | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_data_pending,
  ssl_cf_send,
  ssl_cf_recv,
  Curl_cf_def_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#endif /* !CURL_DISABLE_PROXY */

ಸ್ಥಿರ CURLcode cf_ssl_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (ಶೂನ್ಯ)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, ರಿಕ್ತ);
#ಅನ್ಯಥಾ
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  ಯದಿ(result)
    cf_ctx_free(ctx);
  *pcf = result ? ರಿಕ್ತ : cf;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_ssl_peer_init(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_peer *origin,
                                 ಸಂರಚನಾ Curl_peer *peer,
                                 ಸಂರಚನಾ ssl_filter_config *sslc)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ವರ್ಣ tls_id[80];
  connssl->ssl_impl->version(tls_id, ಪರಿಮಾಣ(tls_id) - 1);
  ಪ್ರತ್ಯಾಗಮ Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_peer *origin,
                              ಸಂರಚನಾ connectdata *conn,
                              int8_t sockindex)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  ಸಂರಚನಾ Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  ಯದಿ(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  ಯದಿ(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  ಅನ್ಯಥಾ ಯದಿ(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_ssl_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ Curl_peer *origin,
                                  ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  ಯದಿ(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  ಯದಿ(!result) {
    Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
#ಯದಿ defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    ಯದಿ((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  ಅನ್ಯಥಾ ಯದಿ(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ಪ್ರತ್ಯಾಗಮ result;
}

#ifndef CURL_DISABLE_PROXY

ಸ್ಥಿರ CURLcode cf_ssl_proxy_create(ಸಂರಚನಾ Curl_cfilter **pcf,
                                    ಸಂರಚನಾ Curl_easy *data,
                                    ಸಂರಚನಾ connectdata *conn)
{
  ಸಂರಚನಾ Curl_cfilter *cf = ರಿಕ್ತ;
  ಸಂರಚನಾ ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (ಶೂನ್ಯ)conn;
#ifdef USE_HTTP2
  ಯದಿ(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  ಯದಿ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  ಯದಿ(result)
    cf_ctx_free(ctx);
  *pcf = result ? ರಿಕ್ತ : cf;
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_cf_ssl_proxy_ಸೇರಿಸು_after(ಸಂರಚನಾ Curl_cfilter *cf_at,
                                        ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ Curl_peer *peer)
{
  ಸಂರಚನಾ Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  ಯದಿ(!result)
    result = cf_ssl_peer_init(cf, peer, ರಿಕ್ತ, &cf_at->conn->proxy_ssl_config);
  ಯದಿ(!result)
    Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
  ಅನ್ಯಥಾ ಯದಿ(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(ಸಂರಚನಾ Curl_easy *data, unsigned ಪೂರ್ಣಾಙ್ಕ ssl_option)
{
  (ಶೂನ್ಯ)data;
  ಪ್ರತ್ಯಾಗಮ (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(ಸಂರಚನಾ Curl_cfilter *cf)
{
  ಯದಿ(Curl_ssl->supports & SSLSUPP_ECH) {
    ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ಪ್ರತ್ಯಾಗಮ !!conn_config->ech;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

bool Curl_ssl_need_httpsrr(ಸಂರಚನಾ Curl_cfilter *cf)
{
  ಯದಿ(Curl_ssl->supports & SSLSUPP_ECH) {
    ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ಯದಿ(!conn_config->ech)
      ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

ಸ್ಥಿರ CURLcode vtls_shutdown_blocking(ಸಂರಚನಾ Curl_cfilter *cf,
                                       ಸಂರಚನಾ Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  ಸಂರಚನಾ cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  ಪೂರ್ಣಾಙ್ಕ what, loop = 10;

  ಯದಿ(cf->shutdown) {
    *done = TRUE;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  ಯಾವತ್(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    ಯದಿ(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      ಗಚ್ಛ out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    ಯದಿ(result || *done)
      ಗಚ್ಛ out;

    ಯದಿ(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      ಯದಿ(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        ಗಚ್ಛ out;
      }
      ಅನ್ಯಥಾ ಯದಿ(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        ಗಚ್ಛ out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_ssl_cfilter_remove(ಸಂರಚನಾ Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  ಸಂರಚನಾ Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : ರಿಕ್ತ;
  ಪ್ರತಿ(cf = head; cf; cf = cf->next) {
    ಯದಿ(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      ಯದಿ(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (ಪೂರ್ಣಾಙ್ಕ)result);
      ಭಙ್ಗ;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

bool Curl_ssl_cf_is_proxy(ಸಂರಚನಾ Curl_cfilter *cf)
{
  ಪ್ರತ್ಯಾಗಮ (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

ಸಂರಚನಾ ssl_easy_config *
Curl_ssl_cf_get_easy_config(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (ಶೂನ್ಯ)cf;
  ಪ್ರತ್ಯಾಗಮ &data->set.ssl;
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

ಸಂರಚನಾ ssl_filter_config *
Curl_ssl_cf_get_filter_config(ಸಂರಚನಾ Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  ಪ್ರತ್ಯಾಗಮ &cf->conn->ssl_config;
#ಅನ್ಯಥಾ
  ಪ್ರತ್ಯಾಗಮ Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(ಸಂರಚನಾ alpn_proto_buf *buf,
                                ಧ್ರುವ ಸಂರಚನಾ alpn_spec *spec)
{
  size_t i, len;
  ಪೂರ್ಣಾಙ್ಕ off = 0;
  unsigned ವರ್ಣ blen;

  memset(buf, 0, ಪರಿಮಾಣ(*buf));
  ಪ್ರತಿ(i = 0; spec && i < spec->ಎಣಿಕೆ; ++i) {
    len = strlen(spec->entries[i]);
    ಯದಿ(len >= ALPN_NAME_MAX)
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    blen = (unsigned ವರ್ಣ)len;
    ಯದಿ(off + blen + 1 >= (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(buf->data))
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(ಸಂರಚನಾ alpn_proto_buf *buf,
                                ಧ್ರುವ ಸಂರಚನಾ alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, ಪರಿಮಾಣ(*buf));
  ಪ್ರತಿ(i = 0; spec && i < spec->ಎಣಿಕೆ; ++i) {
    len = strlen(spec->entries[i]);
    ಯದಿ(len >= ALPN_NAME_MAX)
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    ಯದಿ(off + len + 2 >= ಪರಿಮಾಣ(buf->data))
      ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
    ಯದಿ(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (ಪೂರ್ಣಾಙ್ಕ)off;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

bool Curl_alpn_contains_proto(ಧ್ರುವ ಸಂರಚನಾ alpn_spec *spec,
                              ಧ್ರುವ ವರ್ಣ *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  ಪ್ರತಿ(i = 0; spec && plen && i < spec->ಎಣಿಕೆ; ++i) {
    size_t slen = strlen(spec->entries[i]);
    ಯದಿ((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಶೂನ್ಯ Curl_alpn_restrict_to(ಸಂರಚನಾ alpn_spec *spec, ಧ್ರುವ ವರ್ಣ *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < ಪರಿಮಾಣ(spec->entries[0]));
  ಯದಿ(plen < ಪರಿಮಾಣ(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->ಎಣಿಕೆ = 1;
  }
}

ಶೂನ್ಯ Curl_alpn_copy(ಸಂರಚನಾ alpn_spec *dest, ಧ್ರುವ ಸಂರಚನಾ alpn_spec *src)
{
  ಯದಿ(src)
    memcpy(dest, src, ಪರಿಮಾಣ(*dest));
  ಅನ್ಯಥಾ
    memset(dest, 0, ಪರಿಮಾಣ(*dest));
}

CURLcode Curl_alpn_set_negotiated(ಸಂರಚನಾ Curl_cfilter *cf,
                                  ಸಂರಚನಾ Curl_easy *data,
                                  ಸಂರಚನಾ ssl_connect_data *connssl,
                                  ಧ್ರುವ unsigned ವರ್ಣ *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (ಶೂನ್ಯ)cf;

  ಯದಿ(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    ಯದಿ(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (ಪೂರ್ಣಾಙ್ಕ)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    ಗಚ್ಛ out;
  }

  ಯದಿ(proto && proto_len) {
    ಯದಿ(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      ಗಚ್ಛ out;
    }
    connssl->negotiated.alpn = curlx_memdup0((ಧ್ರುವ ವರ್ಣ *)proto, proto_len);
    ಯದಿ(!connssl->negotiated.alpn)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  ಯದಿ(proto && proto_len) {
    ಯದಿ(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (ಪೂರ್ಣಾಙ್ಕ)proto_len, proto);
    ಅನ್ಯಥಾ
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (ಪೂರ್ಣಾಙ್ಕ)proto_len, proto);
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    ಅನ್ಯಥಾ
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_on_session_reuse(ಸಂರಚನಾ Curl_cfilter *cf,
                               ಸಂರಚನಾ Curl_easy *data,
                               ಸಂರಚನಾ alpn_spec *alpns,
                               ಸಂರಚನಾ Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  ಸಂರಚನಾ ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  ಯದಿ(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  ಅನ್ಯಥಾ ಯದಿ(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  ಅನ್ಯಥಾ {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ಧ್ರುವ unsigned ವರ್ಣ *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸಂರಚನಾ Curl_ssl_session *Curl_ssl_get_cf_session(ಸಂರಚನಾ Curl_easy *data,
                                                 ಧ್ರುವ ಸಂರಚನಾ Curl_cftype *cft,
                                                 int8_t sockindex)
{
  ಯದಿ(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#ಅನ್ಯಥಾ
     (cft == &Curl_cft_ssl)) {
#endif
    ಸಂರಚನಾ Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    ಪ್ರತಿ(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      ಯದಿ(cf1->cft == cft)
        ಭಙ್ಗ;
    }
    ಯದಿ(cf1) {
      ಸಂರಚನಾ ssl_connect_data *connssl = cf1->ctx;
      ಯದಿ(connssl)
        ಪ್ರತ್ಯಾಗಮ connssl->session;
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#endif /* USE_SSL */
