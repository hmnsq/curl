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

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h" /* generic SSL protos etc */
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/keylog.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/openssl.h"        /* OpenSSL versions */
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/gtls.h"           /* GnuTLS versions */
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/wolfssl.h"        /* wolfSSL versions */
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/schannel.h"       /* Schannel SSPI version */
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/mbedtls.h"        /* mbedTLS versions */
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/rustls.h"         /* Rustls versions */

#ਅਨ੍ਤਰ੍ਭਾਵ "slist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/win32-fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha256.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/base64.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "setopt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/cf-dns.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strcopy.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APPLE_SECTRUST
#ਅਨ੍ਤਰ੍ਭਾਵ <Security/Security.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
#ਜੇ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ਸਥਿਰ ਅਚਲ ਬਣਤਰ alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
ਸਥਿਰ ਅਚਲ ਬਣਤਰ alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP2
ਸਥਿਰ ਅਚਲ ਬਣਤਰ alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
ਸਥਿਰ ਅਚਲ ਬਣਤਰ alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
ਸਥਿਰ ਅਚਲ ਬਣਤਰ alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#ਜੇ_ਅੰਤ /* USE_HTTP2 */

ਸਥਿਰ ਅਚਲ ਬਣਤਰ alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  ਜੇ(!use_alpn)
    ਵਾਪਸ ਨਿਰਾ;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  ਜੇ(only_http_10 && (wanted & CURL_HTTP_V1x))
    ਵਾਪਸ &ALPN_SPEC_H10_H11;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP2
  ਜੇ(wanted & CURL_HTTP_V2x) {
    ਜੇ(wanted & CURL_HTTP_V1x)
      ਵਾਪਸ (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    ਵਾਪਸ &ALPN_SPEC_H2;
  }
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)wanted;
  (ਖਾਲੀ)preferred;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ &ALPN_SPEC_H11;
}
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#ਜੇ_ਅੰਤ /* USE_SSL */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
ਸਥਿਰ ਪੂਰਨ multissl_ਤਿਆਰੀ(ਅਚਲ ਬਣਤਰ Curl_ssl *backend);
#ਜੇ_ਅੰਤ

curl_sslbackend Curl_ssl_backend(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
  multissl_ਤਿਆਰੀ(ਨਿਰਾ);
  ਵਾਪਸ Curl_ssl->info.id;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ CURLSSLBACKEND_NONE;
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL

/* "global" init done? */
ਸਥਿਰ bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ਪੂਰਨ Curl_ssl_init(ਖਾਲੀ)
{
  /* make sure this is only done once */
  ਜੇ(init_ssl)
    ਵਾਪਸ 1;
  init_ssl = TRUE; /* never again */

  ਜੇ(Curl_ssl->init)
    ਵਾਪਸ Curl_ssl->init();
  ਵਾਪਸ 1;
}

ਸਥਿਰ bool ssl_prefs_check(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  ਜੇ(conn_config->ਸੰਸਕਰਣ_2 >= CURL_SSLVERSION_ਆਖਰੀ) {
    failf(ਅੰਕੜੇ, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    ਵਾਪਸ FALSE;
  }

  ਚੋਣ(conn_config->ਸੰਸਕਰਣ_max) {
  ਹਾਲਤ CURL_SSLVERSION_MAX_NONE:
  ਹਾਲਤ CURL_SSLVERSION_MAX_DEFAULT:
    ਰੋਕੋ;

  ਮੂਲ:
    ਜੇ((conn_config->ਸੰਸਕਰਣ_max >> 16) < conn_config->ਸੰਸਕਰਣ_2) {
      failf(ਅੰਕੜੇ, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      ਵਾਪਸ FALSE;
    }
  }

  ਵਾਪਸ TRUE;
}

ਸਥਿਰ ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *cf_ctx_new(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                           ਅਚਲ ਬਣਤਰ alpn_spec *alpn)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *ctx;

  (ਖਾਲੀ)ਅੰਕੜੇ;
  ctx = curlx_calloc(1, ਆਕਾਰ(*ctx));
  ਜੇ(!ctx)
    ਵਾਪਸ ਨਿਰਾ;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_ਅੰਕੜੇ);
  ਜੇ(!ctx->backend) {
    curlx_free(ctx);
    ਵਾਪਸ ਨਿਰਾ;
  }
  ਵਾਪਸ ctx;
}

ਸਥਿਰ ਖਾਲੀ cf_ctx_free(ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *ctx)
{
  ਜੇ(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                                      ਬਣਤਰ dynbuf *binding)
{
  ਜੇ(Curl_ssl->get_channel_binding)
    ਵਾਪਸ Curl_ssl->get_channel_binding(ਅੰਕੜੇ, sockindex, binding);
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_ssl_close_all(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(Curl_ssl->close_all)
    Curl_ssl->close_all(ਅੰਕੜੇ);
}

CURLcode Curl_ssl_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;

  ਜੇ(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->ਅਗਲਾ, ਅੰਕੜੇ);
    CURLcode ਨਤੀਜਾ = CURLE_OK;
    ਜੇ(sock != CURL_SOCKET_BAD) {
      ਜੇ(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਬਾਹਰਲਾ_only(ਅੰਕੜੇ, ps, sock);
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = Curl_pollset_ਸੈੱਟ_ਵਿੱਚ_only(ਅੰਕੜੇ, ps, sock);
        CURL_TRC_CF(ਅੰਕੜੇ, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
ਬਣਤਰ curl_slist *Curl_ssl_engines_list(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(Curl_ssl->engines_list)
    ਵਾਪਸ Curl_ssl->engines_list(ਅੰਕੜੇ);
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ size_t multissl_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ);

ਖਾਲੀ Curl_ssl_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_WITH_MULTI_SSL
  (ਖਾਲੀ)multissl_ਸੰਸਕਰਣ(ਬਫਰ, ਮਾਪ);
#ਨਹੀਂਤਾਂ
  (ਖਾਲੀ)Curl_ssl->ਸੰਸਕਰਣ_2(ਬਫਰ, ਮਾਪ);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ Curl_ssl_free_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ curl_certinfo *ci = &ਅੰਕੜੇ->info.certs;

  ਜੇ(ci->ਅੰਕ_ਦਾ_certs) {
    /* free all individual lists used */
    ਪੂਰਨ i;
    ਲਈ(i = 0; i < ci->ਅੰਕ_ਦਾ_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = ਨਿਰਾ;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->ਅੰਕ_ਦਾ_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ ਅੰਕ)
{
  ਬਣਤਰ curl_certinfo *ci = &ਅੰਕੜੇ->info.certs;
  ਬਣਤਰ curl_slist **ਸਾਰਣੀ;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(ਅੰਕੜੇ);

  /* Allocate the required certificate information structures */
  ਸਾਰਣੀ = curlx_calloc((size_t)ਅੰਕ, ਆਕਾਰ(ਬਣਤਰ curl_slist *));
  ਜੇ(!ਸਾਰਣੀ)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ci->ਅੰਕ_ਦਾ_certs = ਅੰਕ;
  ci->certinfo = ਸਾਰਣੀ;

  ਵਾਪਸ CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ਧੱਕੋ_certinfo_len(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਪੂਰਨ certnum,
                                    ਅਚਲ ਅੱਖਰ *label,
                                    ਅਚਲ ਅੱਖਰ *ਮੁੱਲ,
                                    size_t valuelen)
{
  ਬਣਤਰ curl_certinfo *ci = &ਅੰਕੜੇ->info.certs;
  ਬਣਤਰ curl_slist *nl;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਬਣਤਰ dynbuf build;

  DEBUGASSERT(certnum < ci->ਅੰਕ_ਦਾ_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  ਜੇ(curlx_dyn_ਜੋੜੋ(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, ਮੁੱਲ, valuelen))
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  ਜੇ(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ci->certinfo[certnum] = nl;
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਫਰ, size_t ਲੰਮਾਈ)
{
  DEBUGASSERT(ਲੰਮਾਈ == ਆਕਾਰ(ਪੂਰਨ));
  ਜੇ(Curl_ssl->random)
    ਵਾਪਸ Curl_ssl->random(ਅੰਕੜੇ, ਬਫਰ, ਲੰਮਾਈ);
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_NOT_BUILT_ਵਿੱਚ;
}

/*
 * Public key pem to der conversion
 */

ਸਥਿਰ CURLcode pubkey_pem_ਮੰਜ਼ਿਲ_der(ਅਚਲ ਅੱਖਰ *pem,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ **der, size_t *der_len)
{
  ਅਚਲ ਅੱਖਰ *begin_pos, *ਸਿਰਾ_pos;
  size_t pem_ਗਿਣਤੀ, pem_len;
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ dynbuf pbuf;

  /* if no pem, exit. */
  ਜੇ(!pem)
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_ਮਾਪ);

  begin_pos = ਲੜੀ_ਖੋਜ(pem, "-----BEGIN PUBLIC KEY-----");
  ਜੇ(!begin_pos)
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  pem_ਗਿਣਤੀ = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  ਜੇ(pem_ਗਿਣਤੀ && '\n' != pem[pem_ਗਿਣਤੀ - 1])
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_ਗਿਣਤੀ += 26;

  /* Invalid if not directly following \n */
  ਸਿਰਾ_pos = ਲੜੀ_ਖੋਜ(pem + pem_ਗਿਣਤੀ, "\n-----END PUBLIC KEY-----");
  ਜੇ(!ਸਿਰਾ_pos)
    ਵਾਪਸ CURLE_BAD_CONTENT_ENCODING;

  pem_len = ਸਿਰਾ_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  ਜਦੋਂ(pem_ਗਿਣਤੀ < pem_len) {
    ਜੇ('\n' != pem[pem_ਗਿਣਤੀ] && '\r' != pem[pem_ਗਿਣਤੀ]) {
      ਨਤੀਜਾ = curlx_dyn_addn(&pbuf, &pem[pem_ਗਿਣਤੀ], 1);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ++pem_ਗਿਣਤੀ;
  }

  ਜੇ(curlx_dyn_len(&pbuf)) {
    ਨਤੀਜਾ = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = CURLE_BAD_CONTENT_ENCODING;

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *pinnedpubkey,
                              ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pubkey, size_t pubkeylen)
{
  CURLcode ਨਤੀਜਾ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ਜੇ(!pinnedpubkey)
    ਵਾਪਸ CURLE_OK;
  ਜੇ(!pubkey || !pubkeylen)
    ਵਾਪਸ ਨਤੀਜਾ;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    ਅੱਖਰ *cert_hash = ਨਿਰਾ;
    ਅਚਲ ਅੱਖਰ *pinned_hash, *ਸਿਰਾ_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *sha256sumdigest;

    ਜੇ(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      ਵਾਪਸ ਨਤੀਜਾ;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_ਲੰਮਾਈ);
    ਜੇ(!sha256sumdigest)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_ਲੰਮਾਈ);

    ਜੇ(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_ਲੰਮਾਈ,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    ਜੇ(encode)
      ਵਾਪਸ encode;

    infof(ਅੰਕੜੇ, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    ਜਦੋਂ(pinned_hash &&
          !ਲੜੀ_ਤੁਲਨਾ(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      ਸਿਰਾ_pos = strchr(pinned_hash, ';');
      pinned_hash_len = ਸਿਰਾ_pos ?
                        (size_t)(ਸਿਰਾ_pos - pinned_hash) : ਲੜੀ_ਲੰਬਾਈ(pinned_hash);

      /* compare base64 sha256 digests */
      ਜੇ(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(ਅੰਕੜੇ, "public key hash matches pinned value"));
        ਨਤੀਜਾ = CURLE_OK;
        ਰੋਕੋ;
      }

      DEBUGF(infof(ਅੰਕੜੇ, "public key hash does not match 'sha256//%.*s'",
                   (ਪੂਰਨ)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = ਸਿਰਾ_pos ? (ਸਿਰਾ_pos + 1) : ਨਿਰਾ;
    }
    curlx_safefree(cert_hash);
  }
  ਨਹੀਂਤਾਂ {
    ਲੰਮਾ filesize;
    size_t ਮਾਪ, pem_len;
    CURLcode pem_read;
    ਬਣਤਰ dynbuf buf;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pem_ptr = ਨਿਰਾ;
    size_t ਖੱਬਾ;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    ਜੇ(!fp)
      ਵਾਪਸ ਨਤੀਜਾ;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_ਮਾਪ);

    /* Determine the file's size */
    ਜੇ(fseek(fp, 0, SEEK_END))
      ਜਾਓ ਸਿਰਾ;
    filesize = ftell(fp);
    ਜੇ(fseek(fp, 0, SEEK_SET))
      ਜਾਓ ਸਿਰਾ;
    ਜੇ(filesize < 0 || filesize > MAX_PINNED_PUBKEY_ਮਾਪ)
      ਜਾਓ ਸਿਰਾ;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    ਮਾਪ = curlx_sotouz((curl_off_t)filesize);
    ਜੇ(pubkeylen > ਮਾਪ)
      ਜਾਓ ਸਿਰਾ;

    /*
     * Read the file into the dynbuf
     */
    ਖੱਬਾ = ਮਾਪ;
    ਕਰੋ {
      ਅੱਖਰ ਬਫਰ[1024];
      size_t want = ਖੱਬਾ > ਆਕਾਰ(ਬਫਰ) ? ਆਕਾਰ(ਬਫਰ) : ਖੱਬਾ;
      ਜੇ(fread(ਬਫਰ, 1, want, fp) != want)
        ਜਾਓ ਸਿਰਾ;
      ਜੇ(curlx_dyn_addn(&buf, ਬਫਰ, want))
        ਜਾਓ ਸਿਰਾ;
      ਖੱਬਾ -= want;
    } ਜਦੋਂ(ਖੱਬਾ);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    ਜੇ(pubkeylen == ਮਾਪ) {
      ਜੇ(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        ਨਤੀਜਾ = CURLE_OK;
      ਜਾਓ ਸਿਰਾ;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_ਮੰਜ਼ਿਲ_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    ਜੇ(pem_read)
      ਜਾਓ ਸਿਰਾ;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    ਜੇ(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      ਨਤੀਜਾ = CURLE_OK;
ਸਿਰਾ:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_ਸਥਿਤੀ_request(ਖਾਲੀ)
{
  ਜੇ(Curl_ssl->cert_ਸਥਿਤੀ_request)
    ਵਾਪਸ Curl_ssl->cert_ਸਥਿਤੀ_request();
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਪੂਰਨ multissl_init(ਖਾਲੀ)
{
  ਜੇ(multissl_ਤਿਆਰੀ(ਨਿਰਾ))
    ਵਾਪਸ 1;
  ਜੇ(Curl_ssl->init)
    ਵਾਪਸ Curl_ssl->init();
  ਵਾਪਸ 1;
}

ਸਥਿਰ CURLcode multissl_random(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *entropy, size_t ਲੰਮਾਈ)
{
  ਜੇ(multissl_ਤਿਆਰੀ(ਨਿਰਾ))
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_ssl->random(ਅੰਕੜੇ, entropy, ਲੰਮਾਈ);
}

ਸਥਿਰ CURLcode multissl_connect(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਜੇ(multissl_ਤਿਆਰੀ(ਨਿਰਾ))
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_ssl->do_connect(cf, ਅੰਕੜੇ, ਪੂਰਾ);
}

ਸਥਿਰ CURLcode multissl_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ easy_pollset *ps)
{
  ਜੇ(multissl_ਤਿਆਰੀ(ਨਿਰਾ))
    ਵਾਪਸ CURLE_OK;
  ਵਾਪਸ Curl_ssl->adjust_pollset(cf, ਅੰਕੜੇ, ps);
}

ਸਥਿਰ ਖਾਲੀ *multissl_get_internals(ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl,
                                    CURLINFO info)
{
  ਜੇ(multissl_ਤਿਆਰੀ(ਨਿਰਾ))
    ਵਾਪਸ ਨਿਰਾ;
  ਵਾਪਸ Curl_ssl->get_internals(connssl, info);
}

ਸਥਿਰ ਖਾਲੀ multissl_close(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(multissl_ਤਿਆਰੀ(ਨਿਰਾ))
    ਵਾਪਸ;
  Curl_ssl->close(cf, ਅੰਕੜੇ);
}

ਸਥਿਰ CURLcode multissl_recv_plain(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਅੱਖਰ *buf, size_t len, size_t *pnread)
{
  ਜੇ(multissl_ਤਿਆਰੀ(ਨਿਰਾ))
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_ssl->recv_plain(cf, ਅੰਕੜੇ, buf, len, pnread);
}

ਸਥਿਰ CURLcode multissl_send_plain(ਬਣਤਰ Curl_cfilter *cf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਅਚਲ ਖਾਲੀ *mem, size_t len,
                                    size_t *pnwritten)
{
  ਜੇ(multissl_ਤਿਆਰੀ(ਨਿਰਾ))
    ਵਾਪਸ CURLE_FAILED_INIT;
  ਵਾਪਸ Curl_ssl->send_plain(cf, ਅੰਕੜੇ, mem, len, pnwritten);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  ਨਿਰਾ,                              /* cleanup */
  multissl_ਸੰਸਕਰਣ,                  /*ਸੰਸਕਰਣ*/
  ਨਿਰਾ,                              /* shutdown */
  ਨਿਰਾ,                              /* data_pending */
  multissl_random,                   /* random */
  ਨਿਰਾ,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  ਨਿਰਾ,                              /* close_all */
  ਨਿਰਾ,                              /* engines_list */
  ਨਿਰਾ,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  ਨਿਰਾ,                              /* get_channel_binding */
};

ਅਚਲ ਬਣਤਰ Curl_ssl *Curl_ssl =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#ਜਾਂਜੇ defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#ਜਾਂਜੇ defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#ਜਾਂਜੇ defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#ਜਾਂਜੇ defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#ਜਾਂਜੇ defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#ਜਾਂਜੇ defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "Missing struct Curl_ssl for selected SSL backend"
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_ssl *ਉਪਲਬਧ_backends[] = {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WOLFSSL
  &Curl_ssl_wolfssl,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GNUTLS
  &Curl_ssl_gnutls,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MBEDTLS
  &Curl_ssl_mbedtls,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_OPENSSL
  &Curl_ssl_openssl,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SCHANNEL
  &Curl_ssl_schannel,
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_RUSTLS
  &Curl_ssl_rustls,
#ਜੇ_ਅੰਤ
  ਨਿਰਾ
};

/* Global cleanup */
ਖਾਲੀ Curl_ssl_cleanup(ਖਾਲੀ)
{
  ਜੇ(init_ssl) {
    /* only cleanup if we did a previous init */
    ਜੇ(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#ਜੇ_ਅੰਤ
    init_ssl = FALSE;
  }
}

ਸਥਿਰ size_t multissl_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ)
{
  ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_ssl *selected;
  ਸਥਿਰ ਅੱਖਰ backends[200];
  ਸਥਿਰ size_t backends_len;
  ਅਚਲ ਬਣਤਰ Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? ਉਪਲਬਧ_backends[0] : Curl_ssl;

  ਜੇ(current != selected) {
    ਅੱਖਰ *p = backends;
    ਅਚਲ ਅੱਖਰ *ਸਿਰਾ = backends + ਆਕਾਰ(backends);
    ਪੂਰਨ i;

    selected = current;

    backends[0] = '\0';

    ਲਈ(i = 0; ਉਪਲਬਧ_backends[i]; ++i) {
      ਅੱਖਰ vb[200];
      bool paren = (selected != ਉਪਲਬਧ_backends[i]);

      ਜੇ(ਉਪਲਬਧ_backends[i]->ਸੰਸਕਰਣ_2(vb, ਆਕਾਰ(vb))) {
        p += curl_msnprintf(p, ਸਿਰਾ - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  ਜੇ(ਮਾਪ) {
    curlx_strcopy(ਬਫਰ, ਮਾਪ, backends, backends_len);
  }
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ multissl_ਤਿਆਰੀ(ਅਚਲ ਬਣਤਰ Curl_ssl *backend)
{
  ਪੂਰਨ i;
  ਅੱਖਰ *env;

  ਜੇ(Curl_ssl != &Curl_ssl_multi)
    ਵਾਪਸ 1;

  ਜੇ(backend) {
    Curl_ssl = backend;
    ਵਾਪਸ 0;
  }

  ਜੇ(!ਉਪਲਬਧ_backends[0])
    ਵਾਪਸ 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  ਜੇ(env) {
    ਲਈ(i = 0; ਉਪਲਬਧ_backends[i]; i++) {
      ਜੇ(curl_strequal(env, ਉਪਲਬਧ_backends[i]->info.name)) {
        Curl_ssl = ਉਪਲਬਧ_backends[i];
        curlx_free(env);
        ਵਾਪਸ 0;
      }
    }
  }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DEFAULT_SSL_BACKEND
  ਲਈ(i = 0; ਉਪਲਬਧ_backends[i]; i++) {
    ਜੇ(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     ਉਪਲਬਧ_backends[i]->info.name)) {
      Curl_ssl = ਉਪਲਬਧ_backends[i];
      curlx_free(env);
      ਵਾਪਸ 0;
    }
  }
#ਜੇ_ਅੰਤ

  /* Fall back to first available backend */
  Curl_ssl = ਉਪਲਬਧ_backends[0];
  curlx_free(env);
  ਵਾਪਸ 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ਅਚਲ ਅੱਖਰ *name,
                                   ਅਚਲ curl_ssl_backend ***avail)
{
  ਪੂਰਨ i;

  ਜੇ(avail)
    *avail = (ਅਚਲ curl_ssl_backend **)&ਉਪਲਬਧ_backends;

  ਜੇ(Curl_ssl != &Curl_ssl_multi)
    ਵਾਪਸ id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#ਨਹੀਂਤਾਂ
           CURLSSLSET_UNKNOWN_BACKEND;
#ਜੇ_ਅੰਤ

  ਲਈ(i = 0; ਉਪਲਬਧ_backends[i]; i++) {
    ਜੇ(ਉਪਲਬਧ_backends[i]->info.id == id ||
       (name && curl_strequal(ਉਪਲਬਧ_backends[i]->info.name, name))) {
      multissl_ਤਿਆਰੀ(ਉਪਲਬਧ_backends[i]);
      ਵਾਪਸ CURLSSLSET_OK;
    }
  }

  ਵਾਪਸ CURLSSLSET_UNKNOWN_BACKEND;
}

#ਨਹੀਂਤਾਂ /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ਅਚਲ ਅੱਖਰ *name,
                                   ਅਚਲ curl_ssl_backend ***avail)
{
  (ਖਾਲੀ)id;
  (ਖਾਲੀ)name;
  (ਖਾਲੀ)avail;
  ਵਾਪਸ CURLSSLSET_NO_BACKENDS;
}

#ਜੇ_ਅੰਤ /* !USE_SSL */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL

ਖਾਲੀ Curl_ssl_peer_cleanup(ਬਣਤਰ ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_ਕੁੰਜੀ);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

ਸਥਿਰ ssl_peer_type get_peer_type(ਅਚਲ ਅੱਖਰ *hostname)
{
  ਜੇ(hostname && hostname[0]) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਬਣਤਰ in6_addr addr;
#ਨਹੀਂਤਾਂ
    ਬਣਤਰ in_addr addr;
#ਜੇ_ਅੰਤ
    ਜੇ(curlx_inet_pton(AF_INET, hostname, &addr))
      ਵਾਪਸ CURL_SSL_PEER_IPV4;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
    ਨਹੀਂਤਾਂ ਜੇ(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      ਵਾਪਸ CURL_SSL_PEER_IPV6;
    }
#ਜੇ_ਅੰਤ
  }
  ਵਾਪਸ CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(ਬਣਤਰ ssl_peer *ssl_peer,
                            ਬਣਤਰ Curl_peer *origin,
                            ਬਣਤਰ Curl_peer *peer,
                            ਬਣਤਰ ssl_filter_config *sslc,
                            ਅਚਲ ਅੱਖਰ *tls_id,
                            uint8_t transport)
{
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  /* We expect a clean struct, e.g. called only ONCE */
  ਜੇ(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  ਜੇ(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = ਲੜੀ_ਲੰਬਾਈ(origin->hostname);
    ਜੇ(len && (origin->hostname[len - 1] == '.'))
      len--;
    ਜੇ(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      ਜੇ(!ssl_peer->sni)
        ਜਾਓ ਬਾਹਰਲਾ;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  ਨਤੀਜਾ = Curl_ssl_peer_ਕੁੰਜੀ_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_ਕੁੰਜੀ);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    Curl_ssl_peer_cleanup(ssl_peer);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ ssl_cf_destroy(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਜੇ(connssl) {
    connssl->ssl_impl->close(cf, ਅੰਕੜੇ);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = ਨਿਰਾ;
  }
  cf->connected = FALSE;
}

ਸਥਿਰ CURLcode ssl_cf_connect(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(!cf->ਅਗਲਾ || !connssl->peer.origin) {
    *ਪੂਰਾ = FALSE;
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  ਜੇ(!cf->ਅਗਲਾ->connected) {
    ਨਤੀਜਾ = cf->ਅਗਲਾ->cft->do_connect(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || !*ਪੂਰਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *ਪੂਰਾ = FALSE;

  ਜੇ(!connssl->prefs_checked) {
    ਜੇ(!ssl_prefs_check(cf, ਅੰਕੜੇ)) {
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    connssl->prefs_checked = TRUE;
  }

  ਨਤੀਜਾ = connssl->ssl_impl->do_connect(cf, ਅੰਕੜੇ, ਪੂਰਾ);

  ਜੇ(!ਨਤੀਜਾ && *ਪੂਰਾ) {
    cf->connected = TRUE;
    ਜੇ(connssl->state == ssl_connection_complete) {
      connssl->handshake_ਪੂਰਾ = *Curl_pgrs_now(ਅੰਕੜੇ);
    }
    ਜੇ(Curl_tls_keylog_enabled()) {
      infof(ਅੰਕੜੇ, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LIBRESSL_ਸੰਸਕਰਣ_ਸੰਖਿਆ
      infof(ਅੰਕੜੇ, "Note LibreSSL only supports SSLKEYLOGFILE for TLS <= 1.2");
#ਜੇ_ਅੰਤ
    }
    /* Connection can be deferred when sending early data */
    DEBUGASSERT(connssl->state == ssl_connection_complete ||
                connssl->state == ssl_connection_deferred);
    DEBUGASSERT(connssl->state != ssl_connection_deferred ||
                connssl->earlydata_state > ssl_earlydata_none);
  }
ਬਾਹਰਲਾ:
  CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_connect() -> %d, done=%d", (ਪੂਰਨ)ਨਤੀਜਾ, *ਪੂਰਾ);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssl_cf_ਸੈੱਟ_earlydata(ਬਣਤਰ Curl_cfilter *cf,
                                     ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਅਚਲ ਖਾਲੀ *buf, size_t blen)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_ਖਾਲੀ_ਥਾਵਾਂ(&connssl->earlydata));
  ਜੇ(blen) {
    ਜੇ(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    ਨਤੀਜਾ = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssl_cf_connect_deferred(ਬਣਤਰ Curl_cfilter *cf,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਅਚਲ ਖਾਲੀ *buf, size_t blen,
                                        bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *ਪੂਰਾ = FALSE;
  ਜੇ(connssl->earlydata_state == ssl_earlydata_await) {
    ਨਤੀਜਾ = ssl_cf_ਸੈੱਟ_earlydata(cf, ਅੰਕੜੇ, buf, blen);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  ਨਤੀਜਾ = ssl_cf_connect(cf, ਅੰਕੜੇ, ਪੂਰਾ);

  ਜੇ(!ਨਤੀਜਾ && *ਪੂਰਾ) {
    ਜੇ(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_APPCONNECT, connssl->handshake_ਪੂਰਾ);
      connssl->stats_reported = TRUE;
    }
    ਚੋਣ(connssl->earlydata_state) {
    ਹਾਲਤ ssl_earlydata_none:
      ਰੋਕੋ;
    ਹਾਲਤ ssl_earlydata_accepted:
      ਜੇ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_ਅੰਕੜੇ(ਅੰਕੜੇ, (curl_off_t)connssl->earlydata_skip);
      infof(ਅੰਕੜੇ, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      ਰੋਕੋ;
    ਹਾਲਤ ssl_earlydata_rejected:
      ਜੇ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_ਅੰਕੜੇ(ਅੰਕੜੇ, -(curl_off_t)connssl->earlydata_skip);
      infof(ਅੰਕੜੇ, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      ਰੋਕੋ;
    ਮੂਲ:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(ਨਿਰਾ);
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ bool ssl_cf_ਅੰਕੜੇ_pending(ਬਣਤਰ Curl_cfilter *cf,
                                ਅਚਲ ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  bool pending;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  ਜੇ(connssl->ssl_impl->ਅੰਕੜੇ_pending &&
     connssl->ssl_impl->ਅੰਕੜੇ_pending(cf, ਅੰਕੜੇ))
    pending = TRUE;
  ਨਹੀਂਤਾਂ
    pending = cf->ਅਗਲਾ->cft->has_ਅੰਕੜੇ_pending(cf->ਅਗਲਾ, ਅੰਕੜੇ);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ pending;
}

ਸਥਿਰ CURLcode ssl_cf_send(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  (ਖਾਲੀ)eos;
  *pnwritten = 0;
  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);

  ਜੇ(connssl->state == ssl_connection_deferred) {
    bool ਪੂਰਾ = FALSE;
    ਨਤੀਜਾ = ssl_cf_connect_deferred(cf, ਅੰਕੜੇ, buf, blen, &ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਨਹੀਂਤਾਂ ਜੇ(!ਪੂਰਾ) {
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ਜੇ(connssl->earlydata_skip) {
    ਜੇ(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      ਨਤੀਜਾ = CURLE_OK;
      *pnwritten = blen;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  ਜੇ(blen > 0) {
    size_t nwritten;
    ਨਤੀਜਾ = connssl->ssl_impl->send_plain(cf, ਅੰਕੜੇ, buf, blen, &nwritten);
    ਜੇ(!ਨਤੀਜਾ)
      *pnwritten += nwritten;
  }

ਬਾਹਰਲਾ:
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssl_cf_recv(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅੱਖਰ *buf, size_t len,
                            size_t *pnread)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  *pnread = 0;
  ਜੇ(connssl->state == ssl_connection_deferred) {
    bool ਪੂਰਾ = FALSE;
    ਨਤੀਜਾ = ssl_cf_connect_deferred(cf, ਅੰਕੜੇ, ਨਿਰਾ, 0, &ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਨਹੀਂਤਾਂ ਜੇ(!ਪੂਰਾ) {
      ਨਤੀਜਾ = CURLE_AGAIN;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ਨਤੀਜਾ = connssl->ssl_impl->recv_plain(cf, ਅੰਕੜੇ, buf, len, pnread);

ਬਾਹਰਲਾ:
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssl_cf_shutdown(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *ਪੂਰਾ = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  ਜੇ(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_ਥੱਲੇ) {
    ਬਣਤਰ cf_call_ਅੰਕੜੇ save;

    CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
    ਨਤੀਜਾ = connssl->ssl_impl->shut_ਥੱਲੇ(cf, ਅੰਕੜੇ, TRUE, ਪੂਰਾ);
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "cf_shutdown -> %d, done=%d", (ਪੂਰਨ)ਨਤੀਜਾ, *ਪੂਰਾ);
    CF_ਅੰਕੜੇ_RESTORE(cf, save);
    cf->shutdown = (ਨਤੀਜਾ || *ਪੂਰਾ);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssl_cf_adjust_pollset(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ easy_pollset *ps)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ;

  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
  ਨਤੀਜਾ = connssl->ssl_impl->adjust_pollset(cf, ਅੰਕੜੇ, ps);
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode ssl_cf_query(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਪੂਰਨ query, ਪੂਰਨ *pres1, ਖਾਲੀ *pres2)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;

  ਚੋਣ(query) {
  ਹਾਲਤ CF_QUERY_SSL_INFO:
  ਹਾਲਤ CF_QUERY_SSL_CTX_INFO:
    ਜੇ(!Curl_ssl_cf_is_proxy(cf)) {
      ਬਣਤਰ curl_tlssessioninfo *info = pres2;
      ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
      CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_ਅੰਕੜੇ_RESTORE(cf, save);
      ਵਾਪਸ CURLE_OK;
    }
    ਰੋਕੋ;
  ਹਾਲਤ CF_QUERY_ALPN_NEGOTIATED: {
    ਅਚਲ ਅੱਖਰ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "query ALPN: returning '%s'", *palpn);
    ਵਾਪਸ CURLE_OK;
  }
  ਮੂਲ:
    ਰੋਕੋ;
  }
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->query(cf->ਅਗਲਾ, ਅੰਕੜੇ, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ਸਥਿਰ CURLcode ssl_cf_cntrl(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਪੂਰਨ event, ਪੂਰਨ arg1, ਖਾਲੀ *arg2)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;

  (ਖਾਲੀ)arg1;
  (ਖਾਲੀ)arg2;
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਚੋਣ(event) {
  ਹਾਲਤ CF_CTRL_CONN_INFO_UPDATE:
    ਜੇ(connssl->negotiated.alpn && !cf->sockindex) {
      ਜੇ(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_ਵੇਖਿਆ = 11;
      ਨਹੀਂਤਾਂ ਜੇ(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_ਵੇਖਿਆ = 20;
      ਨਹੀਂਤਾਂ ਜੇ(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_ਵੇਖਿਆ = 30;
    }
    ਰੋਕੋ;
  ਹਾਲਤ CF_CTRL_REPORT_STATS:
    ਜੇ(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_ਪੂਰਾ.tv_sec || connssl->handshake_ਪੂਰਾ.tv_usec)) {
      Curl_pgrs_ਸਮਾਂ_Was(ਅੰਕੜੇ, TIMER_APPCONNECT, connssl->handshake_ਪੂਰਾ);
      connssl->stats_reported = TRUE;
    }
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ bool cf_ssl_is_alive(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            bool *ਨਿਵੇਸ਼_pending)
{
  /*
   * This function tries to determine connection status.
   */
  ਵਾਪਸ cf->ਅਗਲਾ ?
    cf->ਅਗਲਾ->cft->is_alive(cf->ਅਗਲਾ, ਅੰਕੜੇ, ਨਿਵੇਸ਼_pending) :
    FALSE; /* pessimistic in absence of data */
}

ਬਣਤਰ Curl_cftype Curl_cft_ssl = {
  "SSL",
  CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_ਅੰਕੜੇ_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

ਬਣਤਰ Curl_cftype Curl_cft_ssl_proxy = {
  "SSL-PROXY",
  CF_TYPE_SSL | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_ਅੰਕੜੇ_pending,
  ssl_cf_send,
  ssl_cf_recv,
  Curl_cf_def_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */

ਸਥਿਰ CURLcode cf_ssl_create(ਬਣਤਰ Curl_cfilter **pcf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *ctx;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(ਅੰਕੜੇ->conn);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  (ਖਾਲੀ)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->ਬਿੱਟ.tls_enable_alpn);
  ctx = cf_ctx_new(ਅੰਕੜੇ, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
  ctx = cf_ctx_new(ਅੰਕੜੇ, alpn_get_spec(ਅੰਕੜੇ->state.http_neg.wanted,
                                       ਅੰਕੜੇ->state.http_neg.preferred,
                                       (bool)ਅੰਕੜੇ->state.http_neg.only_10,
                                       (bool)conn->ਬਿੱਟ.tls_enable_alpn));
#ਜੇ_ਅੰਤ
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    cf_ctx_free(ctx);
  *pcf = ਨਤੀਜਾ ? ਨਿਰਾ : cf;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ssl_peer_init(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_peer *origin,
                                 ਬਣਤਰ Curl_peer *peer,
                                 ਬਣਤਰ ssl_filter_config *sslc)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਅੱਖਰ tls_id[80];
  connssl->ssl_impl->ਸੰਸਕਰਣ_2(tls_id, ਆਕਾਰ(tls_id) - 1);
  ਵਾਪਸ Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_peer *origin,
                              ਬਣਤਰ connectdata *conn,
                              int8_t sockindex)
{
  ਬਣਤਰ Curl_cfilter *cf;
  ਬਣਤਰ Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->ਰਾਹੀਂ_peer2 : conn->ਰਾਹੀਂ_peer;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = cf_ssl_create(&cf, ਅੰਕੜੇ, conn);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  ਜੇ(!ਨਤੀਜਾ)
    Curl_conn_cf_ਜੋੜੋ(ਅੰਕੜੇ, conn, sockindex, cf);
  ਨਹੀਂਤਾਂ ਜੇ(cf)
    Curl_conn_cf_discard_chain(&cf, ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_ssl_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ Curl_peer *origin,
                                  ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = cf_ssl_create(&cf, ਅੰਕੜੇ, cf_at->conn);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  ਜੇ(!ਨਤੀਜਾ) {
    Curl_conn_cf_ਪਾਓ_after(cf_at, cf);
#ਜੇ defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    ਜੇ((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      ਨਤੀਜਾ = Curl_conn_dns_ਜੋੜੋ_https_resolve(ਅੰਕੜੇ, cf->conn, cf->sockindex,
                                               origin);
    }
#ਜੇ_ਅੰਤ /* USE_HTTPSRR && USE_ECH */
  }
  ਨਹੀਂਤਾਂ ਜੇ(cf)
    Curl_conn_cf_discard_chain(&cf, ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY

ਸਥਿਰ CURLcode cf_ssl_proxy_create(ਬਣਤਰ Curl_cfilter **pcf,
                                    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                    ਬਣਤਰ connectdata *conn)
{
  ਬਣਤਰ Curl_cfilter *cf = ਨਿਰਾ;
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *ctx;
  CURLcode ਨਤੀਜਾ;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)ਅੰਕੜੇ->ਸੈੱਟ.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (ਖਾਲੀ)conn;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP2
  ਜੇ(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#ਜੇ_ਅੰਤ

  ctx = cf_ctx_new(ਅੰਕੜੇ, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  ਜੇ(!ctx) {
    ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    cf_ctx_free(ctx);
  *pcf = ਨਤੀਜਾ ? ਨਿਰਾ : cf;
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_cf_ssl_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ Curl_peer *peer)
{
  ਬਣਤਰ Curl_cfilter *cf;
  CURLcode ਨਤੀਜਾ;

  ਨਤੀਜਾ = cf_ssl_proxy_create(&cf, ਅੰਕੜੇ, cf_at->conn);
  ਜੇ(!ਨਤੀਜਾ)
    ਨਤੀਜਾ = cf_ssl_peer_init(cf, peer, ਨਿਰਾ, &cf_at->conn->proxy_ssl_config);
  ਜੇ(!ਨਤੀਜਾ)
    Curl_conn_cf_ਪਾਓ_after(cf_at, cf);
  ਨਹੀਂਤਾਂ ਜੇ(cf)
    Curl_conn_cf_discard_chain(&cf, ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ssl_option)
{
  (ਖਾਲੀ)ਅੰਕੜੇ;
  ਵਾਪਸ (Curl_ssl->supports & ssl_option);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
bool Curl_ssl_ech_enabled(ਬਣਤਰ Curl_cfilter *cf)
{
  ਜੇ(Curl_ssl->supports & SSLSUPP_ECH) {
    ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ਵਾਪਸ !!conn_config->ech;
  }
  ਵਾਪਸ FALSE;
}

bool Curl_ssl_need_httpsrr(ਬਣਤਰ Curl_cfilter *cf)
{
  ਜੇ(Curl_ssl->supports & SSLSUPP_ECH) {
    ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ਜੇ(!conn_config->ech)
      ਵਾਪਸ FALSE;
    ਜੇ((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLcode vtls_shutdown_blocking(ਬਣਤਰ Curl_cfilter *cf,
                                       ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                       bool send_shutdown, bool *ਪੂਰਾ)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  ਬਣਤਰ cf_call_ਅੰਕੜੇ save;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  timediff_t timeout_ms;
  ਪੂਰਨ what, loop = 10;

  ਜੇ(cf->shutdown) {
    *ਪੂਰਾ = TRUE;
    ਵਾਪਸ CURLE_OK;
  }
  CF_ਅੰਕੜੇ_SAVE(save, cf, ਅੰਕੜੇ);

  *ਪੂਰਾ = FALSE;
  ਜਦੋਂ(!ਨਤੀਜਾ && !*ਪੂਰਾ && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(ਅੰਕੜੇ, cf->conn, cf->sockindex);

    ਜੇ(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(ਅੰਕੜੇ, "SSL shutdown timeout");
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਜਾਓ ਬਾਹਰਲਾ;
    }

    ਨਤੀਜਾ = connssl->ssl_impl->shut_ਥੱਲੇ(cf, ਅੰਕੜੇ, send_shutdown, ਪੂਰਾ);
    ਜੇ(ਨਤੀਜਾ || *ਪੂਰਾ)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਜੇ(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, ਅੰਕੜੇ, timeout_ms);
      ਜੇ(what < 0) {
        /* fatal error */
        failf(ਅੰਕੜੇ, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        ਨਤੀਜਾ = CURLE_RECV_ERROR;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(what == 0) {
        /* timeout */
        failf(ਅੰਕੜੇ, "SSL shutdown timeout");
        ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
      /* socket is readable or writable */
    }
  }
ਬਾਹਰਲਾ:
  CF_ਅੰਕੜੇ_RESTORE(cf, save);
  cf->shutdown = (ਨਤੀਜਾ || *ਪੂਰਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_ssl_cfilter_remove(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 int8_t sockindex, bool send_shutdown)
{
  ਬਣਤਰ Curl_cfilter *cf, *ਸਿਰ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਸਿਰ = ਅੰਕੜੇ->conn ? ਅੰਕੜੇ->conn->cfilter[sockindex] : ਨਿਰਾ;
  ਲਈ(cf = ਸਿਰ; cf; cf = cf->ਅਗਲਾ) {
    ਜੇ(cf->cft == &Curl_cft_ssl) {
      bool ਪੂਰਾ;
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_ਸ਼ੁਰੂ_timer(ਅੰਕੜੇ, sockindex, 0);
      ਨਤੀਜਾ = vtls_shutdown_blocking(cf, ਅੰਕੜੇ, send_shutdown, &ਪੂਰਾ);
      Curl_cshutdn_clear_timer(ਅੰਕੜੇ, sockindex);
      ਜੇ(!ਨਤੀਜਾ && !ਪੂਰਾ) /* blocking failed? */
        ਨਤੀਜਾ = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, ਅੰਕੜੇ);
      CURL_TRC_CF(ਅੰਕੜੇ, cf, "shutdown and remove SSL, done -> %d",
                  (ਪੂਰਨ)ਨਤੀਜਾ);
      ਰੋਕੋ;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

bool Curl_ssl_cf_is_proxy(ਬਣਤਰ Curl_cfilter *cf)
{
  ਵਾਪਸ (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

ਬਣਤਰ ssl_easy_config *
Curl_ssl_cf_get_easy_config(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  (ਖਾਲੀ)cf;
  ਵਾਪਸ &ਅੰਕੜੇ->ਸੈੱਟ.ssl;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ Curl_ssl_cf_is_proxy(cf) ? &ਅੰਕੜੇ->ਸੈੱਟ.proxy_ssl : &ਅੰਕੜੇ->ਸੈੱਟ.ssl;
#ਜੇ_ਅੰਤ
}

ਬਣਤਰ ssl_filter_config *
Curl_ssl_cf_get_filter_config(ਬਣਤਰ Curl_cfilter *cf)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
  ਵਾਪਸ &cf->conn->ssl_config;
#ਨਹੀਂਤਾਂ
  ਵਾਪਸ Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#ਜੇ_ਅੰਤ
}

CURLcode Curl_alpn_ਮੰਜ਼ਿਲ_proto_buf(ਬਣਤਰ alpn_proto_buf *buf,
                                ਅਚਲ ਬਣਤਰ alpn_spec *spec)
{
  size_t i, len;
  ਪੂਰਨ off = 0;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ blen;

  memset(buf, 0, ਆਕਾਰ(*buf));
  ਲਈ(i = 0; spec && i < spec->ਗਿਣਤੀ_2; ++i) {
    len = ਲੜੀ_ਲੰਬਾਈ(spec->entries[i]);
    ਜੇ(len >= ALPN_NAME_MAX)
      ਵਾਪਸ CURLE_FAILED_INIT;
    blen = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)len;
    ਜੇ(off + blen + 1 >= (ਪੂਰਨ)ਆਕਾਰ(buf->ਅੰਕੜੇ))
      ਵਾਪਸ CURLE_FAILED_INIT;
    buf->ਅੰਕੜੇ[off++] = blen;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf->ਅੰਕੜੇ + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  ਵਾਪਸ CURLE_OK;
}

CURLcode Curl_alpn_ਮੰਜ਼ਿਲ_proto_str(ਬਣਤਰ alpn_proto_buf *buf,
                                ਅਚਲ ਬਣਤਰ alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, ਆਕਾਰ(*buf));
  ਲਈ(i = 0; spec && i < spec->ਗਿਣਤੀ_2; ++i) {
    len = ਲੜੀ_ਲੰਬਾਈ(spec->entries[i]);
    ਜੇ(len >= ALPN_NAME_MAX)
      ਵਾਪਸ CURLE_FAILED_INIT;
    ਜੇ(off + len + 2 >= ਆਕਾਰ(buf->ਅੰਕੜੇ))
      ਵਾਪਸ CURLE_FAILED_INIT;
    ਜੇ(off)
      buf->ਅੰਕੜੇ[off++] = ',';
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(buf->ਅੰਕੜੇ + off, spec->entries[i], len);
    off += len;
  }
  buf->ਅੰਕੜੇ[off] = '\0';
  buf->len = (ਪੂਰਨ)off;
  ਵਾਪਸ CURLE_OK;
}

bool Curl_alpn_ਸ਼ਾਮਲ_ਹੈ_proto(ਅਚਲ ਬਣਤਰ alpn_spec *spec,
                              ਅਚਲ ਅੱਖਰ *proto)
{
  size_t i, plen = proto ? ਲੜੀ_ਲੰਬਾਈ(proto) : 0;
  ਲਈ(i = 0; spec && plen && i < spec->ਗਿਣਤੀ_2; ++i) {
    size_t slen = ਲੜੀ_ਲੰਬਾਈ(spec->entries[i]);
    ਜੇ((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

ਖਾਲੀ Curl_alpn_restrict_ਮੰਜ਼ਿਲ(ਬਣਤਰ alpn_spec *spec, ਅਚਲ ਅੱਖਰ *proto)
{
  size_t plen = ਲੜੀ_ਲੰਬਾਈ(proto);
  DEBUGASSERT(plen < ਆਕਾਰ(spec->entries[0]));
  ਜੇ(plen < ਆਕਾਰ(spec->entries[0])) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(spec->entries[0], proto, plen + 1);
    spec->ਗਿਣਤੀ_2 = 1;
  }
}

ਖਾਲੀ Curl_alpn_copy(ਬਣਤਰ alpn_spec *dest, ਅਚਲ ਬਣਤਰ alpn_spec *src)
{
  ਜੇ(src)
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(dest, src, ਆਕਾਰ(*dest));
  ਨਹੀਂਤਾਂ
    memset(dest, 0, ਆਕਾਰ(*dest));
}

CURLcode Curl_alpn_ਸੈੱਟ_negotiated(ਬਣਤਰ Curl_cfilter *cf,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl,
                                  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *proto,
                                  size_t proto_len)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  (ਖਾਲੀ)cf;

  ਜੇ(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    ਜੇ(!proto_len) {
      failf(ਅੰਕੜੇ, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ((ਲੜੀ_ਲੰਬਾਈ(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(ਅੰਕੜੇ, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (ਪੂਰਨ)proto_len, proto);
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(ਅੰਕੜੇ, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(proto && proto_len) {
    ਜੇ(memchr(proto, '\0', proto_len)) {
      failf(ਅੰਕੜੇ, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      ਨਤੀਜਾ = CURLE_SSL_CONNECT_ERROR;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    connssl->negotiated.alpn = curlx_memdup0((ਅਚਲ ਅੱਖਰ *)proto, proto_len);
    ਜੇ(!connssl->negotiated.alpn)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਜੇ(proto && proto_len) {
    ਜੇ(connssl->state == ssl_connection_deferred)
      infof(ਅੰਕੜੇ, VTLS_INFOF_ALPN_DEFERRED, (ਪੂਰਨ)proto_len, proto);
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, VTLS_INFOF_ALPN_ACCEPTED, (ਪੂਰਨ)proto_len, proto);
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(connssl->state == ssl_connection_deferred)
      infof(ਅੰਕੜੇ, VTLS_INFOF_NO_ALPN_DEFERRED);
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, VTLS_INFOF_NO_ALPN);
  }

ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_on_session_reuse(ਬਣਤਰ Curl_cfilter *cf,
                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਬਣਤਰ alpn_spec *alpns,
                               ਬਣਤਰ Curl_ssl_session *scs,
                               bool *do_early_ਅੰਕੜੇ, bool early_ਅੰਕੜੇ_allowed)
{
  ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf->ctx;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *do_early_ਅੰਕੜੇ = FALSE;

  ਜੇ(!early_ਅੰਕੜੇ_allowed) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL session does not allow earlydata");
  }
  ਨਹੀਂਤਾਂ ਜੇ(!Curl_alpn_ਸ਼ਾਮਲ_ਹੈ_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(ਅੰਕੜੇ, cf, "SSL session has different ALPN, no early data");
  }
  ਨਹੀਂਤਾਂ {
    infof(ਅੰਕੜੇ, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    ਨਤੀਜਾ = Curl_alpn_ਸੈੱਟ_negotiated(cf, ਅੰਕੜੇ, connssl,
                                      (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)scs->alpn,
                                      scs->alpn ? ਲੜੀ_ਲੰਬਾਈ(scs->alpn) : 0);
    *do_early_ਅੰਕੜੇ = !ਨਤੀਜਾ;
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਬਣਤਰ Curl_ssl_session *Curl_ssl_get_cf_session(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                                 ਅਚਲ ਬਣਤਰ Curl_cftype *cft,
                                                 int8_t sockindex)
{
  ਜੇ(ਅੰਕੜੇ->conn &&
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#ਨਹੀਂਤਾਂ
     (cft == &Curl_cft_ssl)) {
#ਜੇ_ਅੰਤ
    ਬਣਤਰ Curl_cfilter *cf1 = ਅੰਕੜੇ->conn->cfilter[sockindex];
    ਲਈ(; cf1; cf1 = cf1->ਅਗਲਾ) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      ਜੇ(cf1->cft == cft)
        ਰੋਕੋ;
    }
    ਜੇ(cf1) {
      ਬਣਤਰ ssl_connect_ਅੰਕੜੇ *connssl = cf1->ctx;
      ਜੇ(connssl)
        ਵਾਪਸ connssl->session;
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ /* USE_SSL */
