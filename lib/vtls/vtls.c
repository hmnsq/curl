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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APPLE_SECTRUST
#ಅನ್ತರ್ಭಾವ <Security/Security.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
#ಆದರೆ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP2
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTP2 */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  ಆದರೆ(!use_alpn)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  ಆದರೆ(only_http_10 && (wanted & CURL_HTTP_V1x))
    ಹಿಂತಿರುಗಿ &ALPN_SPEC_H10_H11;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP2
  ಆದರೆ(wanted & CURL_HTTP_V2x) {
    ಆದರೆ(wanted & CURL_HTTP_V1x)
      ಹಿಂತಿರುಗಿ (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    ಹಿಂತಿರುಗಿ &ALPN_SPEC_H2;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)wanted;
  (ಶೂನ್ಯ)preferred;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ &ALPN_SPEC_H11;
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ multissl_ಸಿದ್ಧತೆ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl *backend);
#ಆದರೆ_ಅಂತ್ಯ

curl_sslbackend Curl_ssl_backend(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
  multissl_ಸಿದ್ಧತೆ(ಖಾಲಿ);
  ಹಿಂತಿರುಗಿ Curl_ssl->info.id;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ CURLSSLBACKEND_NONE;
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL

/* "global" init done? */
ಸ್ಥಿರ bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ಪೂರ್ಣಾಂಕ Curl_ssl_init(ಶೂನ್ಯ)
{
  /* make sure this is only done once */
  ಆದರೆ(init_ssl)
    ಹಿಂತಿರುಗಿ 1;
  init_ssl = TRUE; /* never again */

  ಆದರೆ(Curl_ssl->init)
    ಹಿಂತಿರುಗಿ Curl_ssl->init();
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ bool ssl_prefs_check(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  ಆದರೆ(conn_config->ಆವೃತ್ತಿ_2 >= CURL_SSLVERSION_ಕೊನೆಯ) {
    failf(ದತ್ತಾಂಶ, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    ಹಿಂತಿರುಗಿ FALSE;
  }

  ಆಯ್ಕೆ(conn_config->ಆವೃತ್ತಿ_max) {
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_NONE:
  ಸ್ಥಿತಿ CURL_SSLVERSION_MAX_DEFAULT:
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ((conn_config->ಆವೃತ್ತಿ_max >> 16) < conn_config->ಆವೃತ್ತಿ_2) {
      failf(ದತ್ತಾಂಶ, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      ಹಿಂತಿರುಗಿ FALSE;
    }
  }

  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *cf_ctx_new(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                           ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *alpn)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *ctx;

  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ctx = curlx_calloc(1, ಗಾತ್ರ(*ctx));
  ಆದರೆ(!ctx)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_ದತ್ತಾಂಶ);
  ಆದರೆ(!ctx->backend) {
    curlx_free(ctx);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ ctx;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ctx_free(ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *ctx)
{
  ಆದರೆ(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                                      ರಚನೆ dynbuf *binding)
{
  ಆದರೆ(Curl_ssl->get_channel_binding)
    ಹಿಂತಿರುಗಿ Curl_ssl->get_channel_binding(ದತ್ತಾಂಶ, sockindex, binding);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssl_close_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(Curl_ssl->close_all)
    Curl_ssl->close_all(ದತ್ತಾಂಶ);
}

CURLcode Curl_ssl_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;

  ಆದರೆ(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ);
    CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
    ಆದರೆ(sock != CURL_SOCKET_BAD) {
      ಆದರೆ(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಹೊರಹೋಗು_only(ದತ್ತಾಂಶ, ps, sock);
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = Curl_pollset_ಗಣ_ಒಳ_only(ದತ್ತಾಂಶ, ps, sock);
        CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
ರಚನೆ curl_slist *Curl_ssl_engines_list(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(Curl_ssl->engines_list)
    ಹಿಂತಿರುಗಿ Curl_ssl->engines_list(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ size_t multissl_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2);

ಶೂನ್ಯ Curl_ssl_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_WITH_MULTI_SSL
  (ಶೂನ್ಯ)multissl_ಆವೃತ್ತಿ(ಬಫರ್, ಗಾತ್ರ_2);
#ಇಲ್ಲದಿದ್ದರೆ
  (ಶೂನ್ಯ)Curl_ssl->ಆವೃತ್ತಿ_2(ಬಫರ್, ಗಾತ್ರ_2);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ Curl_ssl_free_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ curl_certinfo *ci = &ದತ್ತಾಂಶ->info.certs;

  ಆದರೆ(ci->ಅಂಕೆ_ದ_certs) {
    /* free all individual lists used */
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ(i = 0; i < ci->ಅಂಕೆ_ದ_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = ಖಾಲಿ;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->ಅಂಕೆ_ದ_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಅಂಕೆ)
{
  ರಚನೆ curl_certinfo *ci = &ದತ್ತಾಂಶ->info.certs;
  ರಚನೆ curl_slist **ಕೋಷ್ಟಕ;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(ದತ್ತಾಂಶ);

  /* Allocate the required certificate information structures */
  ಕೋಷ್ಟಕ = curlx_calloc((size_t)ಅಂಕೆ, ಗಾತ್ರ(ರಚನೆ curl_slist *));
  ಆದರೆ(!ಕೋಷ್ಟಕ)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ci->ಅಂಕೆ_ದ_certs = ಅಂಕೆ;
  ci->certinfo = ಕೋಷ್ಟಕ;

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ತಳ್ಳು_certinfo_len(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಪೂರ್ಣಾಂಕ certnum,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ,
                                    size_t valuelen)
{
  ರಚನೆ curl_certinfo *ci = &ದತ್ತಾಂಶ->info.certs;
  ರಚನೆ curl_slist *nl;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ರಚನೆ dynbuf build;

  DEBUGASSERT(certnum < ci->ಅಂಕೆ_ದ_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, ಮೌಲ್ಯ, valuelen))
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  ಆದರೆ(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ci->certinfo[certnum] = nl;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಬಫರ್, size_t ಉದ್ದಳತೆ)
{
  DEBUGASSERT(ಉದ್ದಳತೆ == ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
  ಆದರೆ(Curl_ssl->random)
    ಹಿಂತಿರುಗಿ Curl_ssl->random(ದತ್ತಾಂಶ, ಬಫರ್, ಉದ್ದಳತೆ);
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_NOT_BUILT_ಒಳ;
}

/*
 * Public key pem to der conversion
 */

ಸ್ಥಿರ CURLcode pubkey_pem_ಗಮ್ಯ_der(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pem,
                                  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ **der, size_t *der_len)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *begin_pos, *ಕೊನೆ_pos;
  size_t pem_ಎಣಿಕೆ, pem_len;
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ dynbuf pbuf;

  /* if no pem, exit. */
  ಆದರೆ(!pem)
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_ಗಾತ್ರ);

  begin_pos = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(pem, "-----BEGIN PUBLIC KEY-----");
  ಆದರೆ(!begin_pos)
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  pem_ಎಣಿಕೆ = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  ಆದರೆ(pem_ಎಣಿಕೆ && '\n' != pem[pem_ಎಣಿಕೆ - 1])
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_ಎಣಿಕೆ += 26;

  /* Invalid if not directly following \n */
  ಕೊನೆ_pos = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(pem + pem_ಎಣಿಕೆ, "\n-----END PUBLIC KEY-----");
  ಆದರೆ(!ಕೊನೆ_pos)
    ಹಿಂತಿರುಗಿ CURLE_BAD_CONTENT_ENCODING;

  pem_len = ಕೊನೆ_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  ಆಗ(pem_ಎಣಿಕೆ < pem_len) {
    ಆದರೆ('\n' != pem[pem_ಎಣಿಕೆ] && '\r' != pem[pem_ಎಣಿಕೆ]) {
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&pbuf, &pem[pem_ಎಣಿಕೆ], 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ++pem_ಎಣಿಕೆ;
  }

  ಆದರೆ(curlx_dyn_len(&pbuf)) {
    ಫಲಿತಾಂಶ = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = CURLE_BAD_CONTENT_ENCODING;

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinnedpubkey,
                              ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pubkey, size_t pubkeylen)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ಆದರೆ(!pinnedpubkey)
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಆದರೆ(!pubkey || !pubkeylen)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    ಅಕ್ಷರ *cert_hash = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinned_hash, *ಕೊನೆ_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *sha256sumdigest;

    ಆದರೆ(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_ಉದ್ದಳತೆ);
    ಆದರೆ(!sha256sumdigest)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_ಉದ್ದಳತೆ);

    ಆದರೆ(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_ಉದ್ದಳತೆ,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    ಆದರೆ(encode)
      ಹಿಂತಿರುಗಿ encode;

    infof(ದತ್ತಾಂಶ, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    ಆಗ(pinned_hash &&
          !ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      ಕೊನೆ_pos = strchr(pinned_hash, ';');
      pinned_hash_len = ಕೊನೆ_pos ?
                        (size_t)(ಕೊನೆ_pos - pinned_hash) : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(pinned_hash);

      /* compare base64 sha256 digests */
      ಆದರೆ(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(ದತ್ತಾಂಶ, "public key hash matches pinned value"));
        ಫಲಿತಾಂಶ = CURLE_OK;
        ನಿಲ್ಲಿಸು;
      }

      DEBUGF(infof(ದತ್ತಾಂಶ, "public key hash does not match 'sha256//%.*s'",
                   (ಪೂರ್ಣಾಂಕ)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = ಕೊನೆ_pos ? (ಕೊನೆ_pos + 1) : ಖಾಲಿ;
    }
    curlx_safefree(cert_hash);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ದೀರ್ಘ filesize;
    size_t ಗಾತ್ರ_2, pem_len;
    CURLcode pem_read;
    ರಚನೆ dynbuf buf;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pem_ptr = ಖಾಲಿ;
    size_t ಎಡ;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    ಆದರೆ(!fp)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_ಗಾತ್ರ);

    /* Determine the file's size */
    ಆದರೆ(fseek(fp, 0, SEEK_END))
      ಹೋಗು ಕೊನೆ;
    filesize = ftell(fp);
    ಆದರೆ(fseek(fp, 0, SEEK_SET))
      ಹೋಗು ಕೊನೆ;
    ಆದರೆ(filesize < 0 || filesize > MAX_PINNED_PUBKEY_ಗಾತ್ರ)
      ಹೋಗು ಕೊನೆ;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    ಗಾತ್ರ_2 = curlx_sotouz((curl_off_t)filesize);
    ಆದರೆ(pubkeylen > ಗಾತ್ರ_2)
      ಹೋಗು ಕೊನೆ;

    /*
     * Read the file into the dynbuf
     */
    ಎಡ = ಗಾತ್ರ_2;
    ಮಾಡು {
      ಅಕ್ಷರ ಬಫರ್[1024];
      size_t want = ಎಡ > ಗಾತ್ರ(ಬಫರ್) ? ಗಾತ್ರ(ಬಫರ್) : ಎಡ;
      ಆದರೆ(fread(ಬಫರ್, 1, want, fp) != want)
        ಹೋಗು ಕೊನೆ;
      ಆದರೆ(curlx_dyn_addn(&buf, ಬಫರ್, want))
        ಹೋಗು ಕೊನೆ;
      ಎಡ -= want;
    } ಆಗ(ಎಡ);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    ಆದರೆ(pubkeylen == ಗಾತ್ರ_2) {
      ಆದರೆ(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        ಫಲಿತಾಂಶ = CURLE_OK;
      ಹೋಗು ಕೊನೆ;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_ಗಮ್ಯ_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    ಆದರೆ(pem_read)
      ಹೋಗು ಕೊನೆ;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    ಆದರೆ(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      ಫಲಿತಾಂಶ = CURLE_OK;
ಕೊನೆ:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_ಸ್ಥಿತಿ_request(ಶೂನ್ಯ)
{
  ಆದರೆ(Curl_ssl->cert_ಸ್ಥಿತಿ_request)
    ಹಿಂತಿರುಗಿ Curl_ssl->cert_ಸ್ಥಿತಿ_request();
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ multissl_init(ಶೂನ್ಯ)
{
  ಆದರೆ(multissl_ಸಿದ್ಧತೆ(ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ(Curl_ssl->init)
    ಹಿಂತಿರುಗಿ Curl_ssl->init();
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ CURLcode multissl_random(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *entropy, size_t ಉದ್ದಳತೆ)
{
  ಆದರೆ(multissl_ಸಿದ್ಧತೆ(ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_ssl->random(ದತ್ತಾಂಶ, entropy, ಉದ್ದಳತೆ);
}

ಸ್ಥಿರ CURLcode multissl_connect(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ಆದರೆ(multissl_ಸಿದ್ಧತೆ(ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_ssl->do_connect(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);
}

ಸ್ಥಿರ CURLcode multissl_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ easy_pollset *ps)
{
  ಆದರೆ(multissl_ಸಿದ್ಧತೆ(ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಹಿಂತಿರುಗಿ Curl_ssl->adjust_pollset(cf, ದತ್ತಾಂಶ, ps);
}

ಸ್ಥಿರ ಶೂನ್ಯ *multissl_get_internals(ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl,
                                    CURLINFO info)
{
  ಆದರೆ(multissl_ಸಿದ್ಧತೆ(ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ Curl_ssl->get_internals(connssl, info);
}

ಸ್ಥಿರ ಶೂನ್ಯ multissl_close(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(multissl_ಸಿದ್ಧತೆ(ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ;
  Curl_ssl->close(cf, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರ CURLcode multissl_recv_plain(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಅಕ್ಷರ *buf, size_t len, size_t *pnread)
{
  ಆದರೆ(multissl_ಸಿದ್ಧತೆ(ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_ssl->recv_plain(cf, ದತ್ತಾಂಶ, buf, len, pnread);
}

ಸ್ಥಿರ CURLcode multissl_send_plain(ರಚನೆ Curl_cfilter *cf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *mem, size_t len,
                                    size_t *pnwritten)
{
  ಆದರೆ(multissl_ಸಿದ್ಧತೆ(ಖಾಲಿ))
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  ಹಿಂತಿರುಗಿ Curl_ssl->send_plain(cf, ದತ್ತಾಂಶ, mem, len, pnwritten);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  ಖಾಲಿ,                              /* cleanup */
  multissl_ಆವೃತ್ತಿ,                  /*ಆವೃತ್ತಿ*/
  ಖಾಲಿ,                              /* shutdown */
  ಖಾಲಿ,                              /* data_pending */
  multissl_random,                   /* random */
  ಖಾಲಿ,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  ಖಾಲಿ,                              /* close_all */
  ಖಾಲಿ,                              /* engines_list */
  ಖಾಲಿ,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  ಖಾಲಿ,                              /* get_channel_binding */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl *Curl_ssl =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#ಅಥವಾಆದರೆ defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#ಅಥವಾಆದರೆ defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#ಅಥವಾಆದರೆ defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#ಅಥವಾಆದರೆ defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#ಅಥವಾಆದರೆ defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#ಅಥವಾಆದರೆ defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "Missing struct Curl_ssl for selected SSL backend"
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl *ಲಭ್ಯ_backends[] = {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WOLFSSL
  &Curl_ssl_wolfssl,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GNUTLS
  &Curl_ssl_gnutls,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MBEDTLS
  &Curl_ssl_mbedtls,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_OPENSSL
  &Curl_ssl_openssl,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SCHANNEL
  &Curl_ssl_schannel,
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_RUSTLS
  &Curl_ssl_rustls,
#ಆದರೆ_ಅಂತ್ಯ
  ಖಾಲಿ
};

/* Global cleanup */
ಶೂನ್ಯ Curl_ssl_cleanup(ಶೂನ್ಯ)
{
  ಆದರೆ(init_ssl) {
    /* only cleanup if we did a previous init */
    ಆದರೆ(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#ಆದರೆ_ಅಂತ್ಯ
    init_ssl = FALSE;
  }
}

ಸ್ಥಿರ size_t multissl_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2)
{
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl *selected;
  ಸ್ಥಿರ ಅಕ್ಷರ backends[200];
  ಸ್ಥಿರ size_t backends_len;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? ಲಭ್ಯ_backends[0] : Curl_ssl;

  ಆದರೆ(current != selected) {
    ಅಕ್ಷರ *p = backends;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = backends + ಗಾತ್ರ(backends);
    ಪೂರ್ಣಾಂಕ i;

    selected = current;

    backends[0] = '\0';

    ಗಾಗಿ(i = 0; ಲಭ್ಯ_backends[i]; ++i) {
      ಅಕ್ಷರ vb[200];
      bool paren = (selected != ಲಭ್ಯ_backends[i]);

      ಆದರೆ(ಲಭ್ಯ_backends[i]->ಆವೃತ್ತಿ_2(vb, ಗಾತ್ರ(vb))) {
        p += curl_msnprintf(p, ಕೊನೆ - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  ಆದರೆ(ಗಾತ್ರ_2) {
    curlx_strcopy(ಬಫರ್, ಗಾತ್ರ_2, backends, backends_len);
  }
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ multissl_ಸಿದ್ಧತೆ(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ssl *backend)
{
  ಪೂರ್ಣಾಂಕ i;
  ಅಕ್ಷರ *env;

  ಆದರೆ(Curl_ssl != &Curl_ssl_multi)
    ಹಿಂತಿರುಗಿ 1;

  ಆದರೆ(backend) {
    Curl_ssl = backend;
    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(!ಲಭ್ಯ_backends[0])
    ಹಿಂತಿರುಗಿ 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  ಆದರೆ(env) {
    ಗಾಗಿ(i = 0; ಲಭ್ಯ_backends[i]; i++) {
      ಆದರೆ(curl_strequal(env, ಲಭ್ಯ_backends[i]->info.name)) {
        Curl_ssl = ಲಭ್ಯ_backends[i];
        curlx_free(env);
        ಹಿಂತಿರುಗಿ 0;
      }
    }
  }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DEFAULT_SSL_BACKEND
  ಗಾಗಿ(i = 0; ಲಭ್ಯ_backends[i]; i++) {
    ಆದರೆ(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     ಲಭ್ಯ_backends[i]->info.name)) {
      Curl_ssl = ಲಭ್ಯ_backends[i];
      curlx_free(env);
      ಹಿಂತಿರುಗಿ 0;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* Fall back to first available backend */
  Curl_ssl = ಲಭ್ಯ_backends[0];
  curlx_free(env);
  ಹಿಂತಿರುಗಿ 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                   ಸ್ಥಿರಾಂಕ curl_ssl_backend ***avail)
{
  ಪೂರ್ಣಾಂಕ i;

  ಆದರೆ(avail)
    *avail = (ಸ್ಥಿರಾಂಕ curl_ssl_backend **)&ಲಭ್ಯ_backends;

  ಆದರೆ(Curl_ssl != &Curl_ssl_multi)
    ಹಿಂತಿರುಗಿ id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#ಇಲ್ಲದಿದ್ದರೆ
           CURLSSLSET_UNKNOWN_BACKEND;
#ಆದರೆ_ಅಂತ್ಯ

  ಗಾಗಿ(i = 0; ಲಭ್ಯ_backends[i]; i++) {
    ಆದರೆ(ಲಭ್ಯ_backends[i]->info.id == id ||
       (name && curl_strequal(ಲಭ್ಯ_backends[i]->info.name, name))) {
      multissl_ಸಿದ್ಧತೆ(ಲಭ್ಯ_backends[i]);
      ಹಿಂತಿರುಗಿ CURLSSLSET_OK;
    }
  }

  ಹಿಂತಿರುಗಿ CURLSSLSET_UNKNOWN_BACKEND;
}

#ಇಲ್ಲದಿದ್ದರೆ /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                   ಸ್ಥಿರಾಂಕ curl_ssl_backend ***avail)
{
  (ಶೂನ್ಯ)id;
  (ಶೂನ್ಯ)name;
  (ಶೂನ್ಯ)avail;
  ಹಿಂತಿರುಗಿ CURLSSLSET_NO_BACKENDS;
}

#ಆದರೆ_ಅಂತ್ಯ /* !USE_SSL */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL

ಶೂನ್ಯ Curl_ssl_peer_cleanup(ರಚನೆ ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_ಕೀಲಿ);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

ಸ್ಥಿರ ssl_peer_type get_peer_type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname)
{
  ಆದರೆ(hostname && hostname[0]) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ರಚನೆ in6_addr addr;
#ಇಲ್ಲದಿದ್ದರೆ
    ರಚನೆ in_addr addr;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(curlx_inet_pton(AF_INET, hostname, &addr))
      ಹಿಂತಿರುಗಿ CURL_SSL_PEER_IPV4;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      ಹಿಂತಿರುಗಿ CURL_SSL_PEER_IPV6;
    }
#ಆದರೆ_ಅಂತ್ಯ
  }
  ಹಿಂತಿರುಗಿ CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(ರಚನೆ ssl_peer *ssl_peer,
                            ರಚನೆ Curl_peer *origin,
                            ರಚನೆ Curl_peer *peer,
                            ರಚನೆ ssl_filter_config *sslc,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tls_id,
                            uint8_t transport)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  /* We expect a clean struct, e.g. called only ONCE */
  ಆದರೆ(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  ಆದರೆ(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(origin->hostname);
    ಆದರೆ(len && (origin->hostname[len - 1] == '.'))
      len--;
    ಆದರೆ(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      ಆದರೆ(!ssl_peer->sni)
        ಹೋಗು ಹೊರಹೋಗು;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  ಫಲಿತಾಂಶ = Curl_ssl_peer_ಕೀಲಿ_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_ಕೀಲಿ);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    Curl_ssl_peer_cleanup(ssl_peer);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ssl_cf_destroy(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ಆದರೆ(connssl) {
    connssl->ssl_impl->close(cf, ದತ್ತಾಂಶ);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = ಖಾಲಿ;
  }
  cf->connected = FALSE;
}

ಸ್ಥಿರ CURLcode ssl_cf_connect(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(!cf->ಮುಂದಿನ || !connssl->peer.origin) {
    *ಮುಗಿದ = FALSE;
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  ಆದರೆ(!cf->ಮುಂದಿನ->connected) {
    ಫಲಿತಾಂಶ = cf->ಮುಂದಿನ->cft->do_connect(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || !*ಮುಗಿದ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *ಮುಗಿದ = FALSE;

  ಆದರೆ(!connssl->prefs_checked) {
    ಆದರೆ(!ssl_prefs_check(cf, ದತ್ತಾಂಶ)) {
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    connssl->prefs_checked = TRUE;
  }

  ಫಲಿತಾಂಶ = connssl->ssl_impl->do_connect(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಆದರೆ(!ಫಲಿತಾಂಶ && *ಮುಗಿದ) {
    cf->connected = TRUE;
    ಆದರೆ(connssl->state == ssl_connection_complete) {
      connssl->handshake_ಮುಗಿದ = *Curl_pgrs_now(ದತ್ತಾಂಶ);
    }
    ಆದರೆ(Curl_tls_keylog_enabled()) {
      infof(ದತ್ತಾಂಶ, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LIBRESSL_ಆವೃತ್ತಿ_ಸಂಖ್ಯೆ
      infof(ದತ್ತಾಂಶ, "Note LibreSSL only supports SSLKEYLOGFILE for TLS <= 1.2");
#ಆದರೆ_ಅಂತ್ಯ
    }
    /* Connection can be deferred when sending early data */
    DEBUGASSERT(connssl->state == ssl_connection_complete ||
                connssl->state == ssl_connection_deferred);
    DEBUGASSERT(connssl->state != ssl_connection_deferred ||
                connssl->earlydata_state > ssl_earlydata_none);
  }
ಹೊರಹೋಗು:
  CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_connect() -> %d, done=%d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಮುಗಿದ);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssl_cf_ಗಣ_earlydata(ರಚನೆ Curl_cfilter *cf,
                                     ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t blen)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_ಖಾಲಿಗಳು(&connssl->earlydata));
  ಆದರೆ(blen) {
    ಆದರೆ(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    ಫಲಿತಾಂಶ = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssl_cf_connect_deferred(ರಚನೆ Curl_cfilter *cf,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t blen,
                                        bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *ಮುಗಿದ = FALSE;
  ಆದರೆ(connssl->earlydata_state == ssl_earlydata_await) {
    ಫಲಿತಾಂಶ = ssl_cf_ಗಣ_earlydata(cf, ದತ್ತಾಂಶ, buf, blen);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  ಫಲಿತಾಂಶ = ssl_cf_connect(cf, ದತ್ತಾಂಶ, ಮುಗಿದ);

  ಆದರೆ(!ಫಲಿತಾಂಶ && *ಮುಗಿದ) {
    ಆದರೆ(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_APPCONNECT, connssl->handshake_ಮುಗಿದ);
      connssl->stats_reported = TRUE;
    }
    ಆಯ್ಕೆ(connssl->earlydata_state) {
    ಸ್ಥಿತಿ ssl_earlydata_none:
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ssl_earlydata_accepted:
      ಆದರೆ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ, (curl_off_t)connssl->earlydata_skip);
      infof(ದತ್ತಾಂಶ, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ ssl_earlydata_rejected:
      ಆದರೆ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ, -(curl_off_t)connssl->earlydata_skip);
      infof(ದತ್ತಾಂಶ, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(ಖಾಲಿ);
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ bool ssl_cf_ದತ್ತಾಂಶ_pending(ರಚನೆ Curl_cfilter *cf,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  bool pending;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  ಆದರೆ(connssl->ssl_impl->ದತ್ತಾಂಶ_pending &&
     connssl->ssl_impl->ದತ್ತಾಂಶ_pending(cf, ದತ್ತಾಂಶ))
    pending = TRUE;
  ಇಲ್ಲದಿದ್ದರೆ
    pending = cf->ಮುಂದಿನ->cft->has_ದತ್ತಾಂಶ_pending(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ pending;
}

ಸ್ಥಿರ CURLcode ssl_cf_send(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  (ಶೂನ್ಯ)eos;
  *pnwritten = 0;
  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);

  ಆದರೆ(connssl->state == ssl_connection_deferred) {
    bool ಮುಗಿದ = FALSE;
    ಫಲಿತಾಂಶ = ssl_cf_connect_deferred(cf, ದತ್ತಾಂಶ, buf, blen, &ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮುಗಿದ) {
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ಆದರೆ(connssl->earlydata_skip) {
    ಆದರೆ(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      ಫಲಿತಾಂಶ = CURLE_OK;
      *pnwritten = blen;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  ಆದರೆ(blen > 0) {
    size_t nwritten;
    ಫಲಿತಾಂಶ = connssl->ssl_impl->send_plain(cf, ದತ್ತಾಂಶ, buf, blen, &nwritten);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      *pnwritten += nwritten;
  }

ಹೊರಹೋಗು:
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssl_cf_recv(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಅಕ್ಷರ *buf, size_t len,
                            size_t *pnread)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  *pnread = 0;
  ಆದರೆ(connssl->state == ssl_connection_deferred) {
    bool ಮುಗಿದ = FALSE;
    ಫಲಿತಾಂಶ = ssl_cf_connect_deferred(cf, ದತ್ತಾಂಶ, ಖಾಲಿ, 0, &ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!ಮುಗಿದ) {
      ಫಲಿತಾಂಶ = CURLE_AGAIN;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ಫಲಿತಾಂಶ = connssl->ssl_impl->recv_plain(cf, ದತ್ತಾಂಶ, buf, len, pnread);

ಹೊರಹೋಗು:
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssl_cf_shutdown(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *ಮುಗಿದ = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  ಆದರೆ(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_ಕೆಳಕ್ಕೆ) {
    ರಚನೆ cf_call_ದತ್ತಾಂಶ save;

    CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
    ಫಲಿತಾಂಶ = connssl->ssl_impl->shut_ಕೆಳಕ್ಕೆ(cf, ದತ್ತಾಂಶ, TRUE, ಮುಗಿದ);
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "cf_shutdown -> %d, done=%d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ, *ಮುಗಿದ);
    CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
    cf->shutdown = (ಫಲಿತಾಂಶ || *ಮುಗಿದ);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssl_cf_adjust_pollset(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ easy_pollset *ps)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ;

  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = connssl->ssl_impl->adjust_pollset(cf, ದತ್ತಾಂಶ, ps);
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode ssl_cf_query(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಪೂರ್ಣಾಂಕ query, ಪೂರ್ಣಾಂಕ *pres1, ಶೂನ್ಯ *pres2)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;

  ಆಯ್ಕೆ(query) {
  ಸ್ಥಿತಿ CF_QUERY_SSL_INFO:
  ಸ್ಥಿತಿ CF_QUERY_SSL_CTX_INFO:
    ಆದರೆ(!Curl_ssl_cf_is_proxy(cf)) {
      ರಚನೆ curl_tlssessioninfo *info = pres2;
      ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
      CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_QUERY_ALPN_NEGOTIATED: {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "query ALPN: returning '%s'", *palpn);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->query(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ಸ್ಥಿರ CURLcode ssl_cf_cntrl(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಪೂರ್ಣಾಂಕ event, ಪೂರ್ಣಾಂಕ arg1, ಶೂನ್ಯ *arg2)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;

  (ಶೂನ್ಯ)arg1;
  (ಶೂನ್ಯ)arg2;
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಆಯ್ಕೆ(event) {
  ಸ್ಥಿತಿ CF_CTRL_CONN_INFO_UPDATE:
    ಆದರೆ(connssl->negotiated.alpn && !cf->sockindex) {
      ಆದರೆ(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_ನೋಡಿದ = 11;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_ನೋಡಿದ = 20;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_ನೋಡಿದ = 30;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CF_CTRL_REPORT_STATS:
    ಆದರೆ(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_ಮುಗಿದ.tv_sec || connssl->handshake_ಮುಗಿದ.tv_usec)) {
      Curl_pgrs_ಸಮಯ_Was(ದತ್ತಾಂಶ, TIMER_APPCONNECT, connssl->handshake_ಮುಗಿದ);
      connssl->stats_reported = TRUE;
    }
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ bool cf_ssl_is_alive(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            bool *ಒಳಹರಿವು_pending)
{
  /*
   * This function tries to determine connection status.
   */
  ಹಿಂತಿರುಗಿ cf->ಮುಂದಿನ ?
    cf->ಮುಂದಿನ->cft->is_alive(cf->ಮುಂದಿನ, ದತ್ತಾಂಶ, ಒಳಹರಿವು_pending) :
    FALSE; /* pessimistic in absence of data */
}

ರಚನೆ Curl_cftype Curl_cft_ssl = {
  "SSL",
  CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_ದತ್ತಾಂಶ_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY

ರಚನೆ Curl_cftype Curl_cft_ssl_proxy = {
  "SSL-PROXY",
  CF_TYPE_SSL | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_ದತ್ತಾಂಶ_pending,
  ssl_cf_send,
  ssl_cf_recv,
  Curl_cf_def_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */

ಸ್ಥಿರ CURLcode cf_ssl_create(ರಚನೆ Curl_cfilter **pcf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ connectdata *conn)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *ctx;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(ದತ್ತಾಂಶ->conn);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_HTTP
  (ಶೂನ್ಯ)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->ಬಿಟ್ಟುಗಳು.tls_enable_alpn);
  ctx = cf_ctx_new(ದತ್ತಾಂಶ, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
  ctx = cf_ctx_new(ದತ್ತಾಂಶ, alpn_get_spec(ದತ್ತಾಂಶ->state.http_neg.wanted,
                                       ದತ್ತಾಂಶ->state.http_neg.preferred,
                                       (bool)ದತ್ತಾಂಶ->state.http_neg.only_10,
                                       (bool)conn->ಬಿಟ್ಟುಗಳು.tls_enable_alpn));
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    cf_ctx_free(ctx);
  *pcf = ಫಲಿತಾಂಶ ? ಖಾಲಿ : cf;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ssl_peer_init(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_peer *origin,
                                 ರಚನೆ Curl_peer *peer,
                                 ರಚನೆ ssl_filter_config *sslc)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ಅಕ್ಷರ tls_id[80];
  connssl->ssl_impl->ಆವೃತ್ತಿ_2(tls_id, ಗಾತ್ರ(tls_id) - 1);
  ಹಿಂತಿರುಗಿ Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_peer *origin,
                              ರಚನೆ connectdata *conn,
                              int8_t sockindex)
{
  ರಚನೆ Curl_cfilter *cf;
  ರಚನೆ Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->ಮೂಲಕ_peer2 : conn->ಮೂಲಕ_peer;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = cf_ssl_create(&cf, ದತ್ತಾಂಶ, conn);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    Curl_conn_cf_ಕೂಡಿಸು(ದತ್ತಾಂಶ, conn, sockindex, cf);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cf)
    Curl_conn_cf_discard_chain(&cf, ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_ssl_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ Curl_peer *origin,
                                  ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = cf_ssl_create(&cf, ದತ್ತಾಂಶ, cf_at->conn);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
#ಆದರೆ defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    ಆದರೆ((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      ಫಲಿತಾಂಶ = Curl_conn_dns_ಕೂಡಿಸು_https_resolve(ದತ್ತಾಂಶ, cf->conn, cf->sockindex,
                                               origin);
    }
#ಆದರೆ_ಅಂತ್ಯ /* USE_HTTPSRR && USE_ECH */
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cf)
    Curl_conn_cf_discard_chain(&cf, ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY

ಸ್ಥಿರ CURLcode cf_ssl_proxy_create(ರಚನೆ Curl_cfilter **pcf,
                                    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                    ರಚನೆ connectdata *conn)
{
  ರಚನೆ Curl_cfilter *cf = ಖಾಲಿ;
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *ctx;
  CURLcode ಫಲಿತಾಂಶ;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)ದತ್ತಾಂಶ->ಗಣ.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (ಶೂನ್ಯ)conn;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP2
  ಆದರೆ(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#ಆದರೆ_ಅಂತ್ಯ

  ctx = cf_ctx_new(ದತ್ತಾಂಶ, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  ಆದರೆ(!ctx) {
    ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    cf_ctx_free(ctx);
  *pcf = ಫಲಿತಾಂಶ ? ಖಾಲಿ : cf;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_cf_ssl_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ Curl_peer *peer)
{
  ರಚನೆ Curl_cfilter *cf;
  CURLcode ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = cf_ssl_proxy_create(&cf, ದತ್ತಾಂಶ, cf_at->conn);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    ಫಲಿತಾಂಶ = cf_ssl_peer_init(cf, peer, ಖಾಲಿ, &cf_at->conn->proxy_ssl_config);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    Curl_conn_cf_ಸೇರಿಸು_after(cf_at, cf);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cf)
    Curl_conn_cf_discard_chain(&cf, ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ssl_option)
{
  (ಶೂನ್ಯ)ದತ್ತಾಂಶ;
  ಹಿಂತಿರುಗಿ (Curl_ssl->supports & ssl_option);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
bool Curl_ssl_ech_enabled(ರಚನೆ Curl_cfilter *cf)
{
  ಆದರೆ(Curl_ssl->supports & SSLSUPP_ECH) {
    ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ಹಿಂತಿರುಗಿ !!conn_config->ech;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

bool Curl_ssl_need_httpsrr(ರಚನೆ Curl_cfilter *cf)
{
  ಆದರೆ(Curl_ssl->supports & SSLSUPP_ECH) {
    ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ಆದರೆ(!conn_config->ech)
      ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLcode vtls_shutdown_blocking(ರಚನೆ Curl_cfilter *cf,
                                       ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                       bool send_shutdown, bool *ಮುಗಿದ)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  ರಚನೆ cf_call_ದತ್ತಾಂಶ save;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  timediff_t timeout_ms;
  ಪೂರ್ಣಾಂಕ what, loop = 10;

  ಆದರೆ(cf->shutdown) {
    *ಮುಗಿದ = TRUE;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }
  CF_ದತ್ತಾಂಶ_SAVE(save, cf, ದತ್ತಾಂಶ);

  *ಮುಗಿದ = FALSE;
  ಆಗ(!ಫಲಿತಾಂಶ && !*ಮುಗಿದ && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(ದತ್ತಾಂಶ, cf->conn, cf->sockindex);

    ಆದರೆ(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(ದತ್ತಾಂಶ, "SSL shutdown timeout");
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ಹೋಗು ಹೊರಹೋಗು;
    }

    ಫಲಿತಾಂಶ = connssl->ssl_impl->shut_ಕೆಳಕ್ಕೆ(cf, ದತ್ತಾಂಶ, send_shutdown, ಮುಗಿದ);
    ಆದರೆ(ಫಲಿತಾಂಶ || *ಮುಗಿದ)
      ಹೋಗು ಹೊರಹೋಗು;

    ಆದರೆ(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, ದತ್ತಾಂಶ, timeout_ms);
      ಆದರೆ(what < 0) {
        /* fatal error */
        failf(ದತ್ತಾಂಶ, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(what == 0) {
        /* timeout */
        failf(ದತ್ತಾಂಶ, "SSL shutdown timeout");
        ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
        ಹೋಗು ಹೊರಹೋಗು;
      }
      /* socket is readable or writable */
    }
  }
ಹೊರಹೋಗು:
  CF_ದತ್ತಾಂಶ_RESTORE(cf, save);
  cf->shutdown = (ಫಲಿತಾಂಶ || *ಮುಗಿದ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_ssl_cfilter_remove(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 int8_t sockindex, bool send_shutdown)
{
  ರಚನೆ Curl_cfilter *cf, *ತಲೆ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ತಲೆ = ದತ್ತಾಂಶ->conn ? ದತ್ತಾಂಶ->conn->cfilter[sockindex] : ಖಾಲಿ;
  ಗಾಗಿ(cf = ತಲೆ; cf; cf = cf->ಮುಂದಿನ) {
    ಆದರೆ(cf->cft == &Curl_cft_ssl) {
      bool ಮುಗಿದ;
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_ಆರಂಭ_timer(ದತ್ತಾಂಶ, sockindex, 0);
      ಫಲಿತಾಂಶ = vtls_shutdown_blocking(cf, ದತ್ತಾಂಶ, send_shutdown, &ಮುಗಿದ);
      Curl_cshutdn_clear_timer(ದತ್ತಾಂಶ, sockindex);
      ಆದರೆ(!ಫಲಿತಾಂಶ && !ಮುಗಿದ) /* blocking failed? */
        ಫಲಿತಾಂಶ = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, ದತ್ತಾಂಶ);
      CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "shutdown and remove SSL, done -> %d",
                  (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
      ನಿಲ್ಲಿಸು;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

bool Curl_ssl_cf_is_proxy(ರಚನೆ Curl_cfilter *cf)
{
  ಹಿಂತಿರುಗಿ (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

ರಚನೆ ssl_easy_config *
Curl_ssl_cf_get_easy_config(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_PROXY
  (ಶೂನ್ಯ)cf;
  ಹಿಂತಿರುಗಿ &ದತ್ತಾಂಶ->ಗಣ.ssl;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ Curl_ssl_cf_is_proxy(cf) ? &ದತ್ತಾಂಶ->ಗಣ.proxy_ssl : &ದತ್ತಾಂಶ->ಗಣ.ssl;
#ಆದರೆ_ಅಂತ್ಯ
}

ರಚನೆ ssl_filter_config *
Curl_ssl_cf_get_filter_config(ರಚನೆ Curl_cfilter *cf)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_PROXY
  ಹಿಂತಿರುಗಿ &cf->conn->ssl_config;
#ಇಲ್ಲದಿದ್ದರೆ
  ಹಿಂತಿರುಗಿ Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#ಆದರೆ_ಅಂತ್ಯ
}

CURLcode Curl_alpn_ಗಮ್ಯ_proto_buf(ರಚನೆ alpn_proto_buf *buf,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *spec)
{
  size_t i, len;
  ಪೂರ್ಣಾಂಕ off = 0;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ blen;

  memset(buf, 0, ಗಾತ್ರ(*buf));
  ಗಾಗಿ(i = 0; spec && i < spec->ಎಣಿಕೆ_2; ++i) {
    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(spec->entries[i]);
    ಆದರೆ(len >= ALPN_NAME_MAX)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    blen = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)len;
    ಆದರೆ(off + blen + 1 >= (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf->ದತ್ತಾಂಶ))
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    buf->ದತ್ತಾಂಶ[off++] = blen;
    ಸ್ಮರಣೆ_ನಕಲು(buf->ದತ್ತಾಂಶ + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

CURLcode Curl_alpn_ಗಮ್ಯ_proto_str(ರಚನೆ alpn_proto_buf *buf,
                                ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, ಗಾತ್ರ(*buf));
  ಗಾಗಿ(i = 0; spec && i < spec->ಎಣಿಕೆ_2; ++i) {
    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(spec->entries[i]);
    ಆದರೆ(len >= ALPN_NAME_MAX)
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    ಆದರೆ(off + len + 2 >= ಗಾತ್ರ(buf->ದತ್ತಾಂಶ))
      ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
    ಆದರೆ(off)
      buf->ದತ್ತಾಂಶ[off++] = ',';
    ಸ್ಮರಣೆ_ನಕಲು(buf->ದತ್ತಾಂಶ + off, spec->entries[i], len);
    off += len;
  }
  buf->ದತ್ತಾಂಶ[off] = '\0';
  buf->len = (ಪೂರ್ಣಾಂಕ)off;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

bool Curl_alpn_ಒಳಗೊಂಡಿದೆ_proto(ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *spec,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proto)
{
  size_t i, plen = proto ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(proto) : 0;
  ಗಾಗಿ(i = 0; spec && plen && i < spec->ಎಣಿಕೆ_2; ++i) {
    size_t slen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(spec->entries[i]);
    ಆದರೆ((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಶೂನ್ಯ Curl_alpn_restrict_ಗಮ್ಯ(ರಚನೆ alpn_spec *spec, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proto)
{
  size_t plen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(proto);
  DEBUGASSERT(plen < ಗಾತ್ರ(spec->entries[0]));
  ಆದರೆ(plen < ಗಾತ್ರ(spec->entries[0])) {
    ಸ್ಮರಣೆ_ನಕಲು(spec->entries[0], proto, plen + 1);
    spec->ಎಣಿಕೆ_2 = 1;
  }
}

ಶೂನ್ಯ Curl_alpn_copy(ರಚನೆ alpn_spec *dest, ಸ್ಥಿರಾಂಕ ರಚನೆ alpn_spec *src)
{
  ಆದರೆ(src)
    ಸ್ಮರಣೆ_ನಕಲು(dest, src, ಗಾತ್ರ(*dest));
  ಇಲ್ಲದಿದ್ದರೆ
    memset(dest, 0, ಗಾತ್ರ(*dest));
}

CURLcode Curl_alpn_ಗಣ_negotiated(ರಚನೆ Curl_cfilter *cf,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl,
                                  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *proto,
                                  size_t proto_len)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  (ಶೂನ್ಯ)cf;

  ಆದರೆ(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    ಆದರೆ(!proto_len) {
      failf(ದತ್ತಾಂಶ, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(ದತ್ತಾಂಶ, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (ಪೂರ್ಣಾಂಕ)proto_len, proto);
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(ದತ್ತಾಂಶ, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(proto && proto_len) {
    ಆದರೆ(memchr(proto, '\0', proto_len)) {
      failf(ದತ್ತಾಂಶ, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      ಫಲಿತಾಂಶ = CURLE_SSL_CONNECT_ERROR;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    connssl->negotiated.alpn = curlx_memdup0((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)proto, proto_len);
    ಆದರೆ(!connssl->negotiated.alpn)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಆದರೆ(proto && proto_len) {
    ಆದರೆ(connssl->state == ssl_connection_deferred)
      infof(ದತ್ತಾಂಶ, VTLS_INFOF_ALPN_DEFERRED, (ಪೂರ್ಣಾಂಕ)proto_len, proto);
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, VTLS_INFOF_ALPN_ACCEPTED, (ಪೂರ್ಣಾಂಕ)proto_len, proto);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(connssl->state == ssl_connection_deferred)
      infof(ದತ್ತಾಂಶ, VTLS_INFOF_NO_ALPN_DEFERRED);
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, VTLS_INFOF_NO_ALPN);
  }

ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_on_session_reuse(ರಚನೆ Curl_cfilter *cf,
                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ರಚನೆ alpn_spec *alpns,
                               ರಚನೆ Curl_ssl_session *scs,
                               bool *do_early_ದತ್ತಾಂಶ, bool early_ದತ್ತಾಂಶ_allowed)
{
  ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf->ctx;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *do_early_ದತ್ತಾಂಶ = FALSE;

  ಆದರೆ(!early_ದತ್ತಾಂಶ_allowed) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL session does not allow earlydata");
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_alpn_ಒಳಗೊಂಡಿದೆ_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(ದತ್ತಾಂಶ, cf, "SSL session has different ALPN, no early data");
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    infof(ದತ್ತಾಂಶ, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    ಫಲಿತಾಂಶ = Curl_alpn_ಗಣ_negotiated(cf, ದತ್ತಾಂಶ, connssl,
                                      (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)scs->alpn,
                                      scs->alpn ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(scs->alpn) : 0);
    *do_early_ದತ್ತಾಂಶ = !ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ರಚನೆ Curl_ssl_session *Curl_ssl_get_cf_session(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                                 ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cftype *cft,
                                                 int8_t sockindex)
{
  ಆದರೆ(ದತ್ತಾಂಶ->conn &&
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#ಇಲ್ಲದಿದ್ದರೆ
     (cft == &Curl_cft_ssl)) {
#ಆದರೆ_ಅಂತ್ಯ
    ರಚನೆ Curl_cfilter *cf1 = ದತ್ತಾಂಶ->conn->cfilter[sockindex];
    ಗಾಗಿ(; cf1; cf1 = cf1->ಮುಂದಿನ) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      ಆದರೆ(cf1->cft == cft)
        ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(cf1) {
      ರಚನೆ ssl_connect_ದತ್ತಾಂಶ *connssl = cf1->ctx;
      ಆದರೆ(connssl)
        ಹಿಂತಿರುಗಿ connssl->session;
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */
