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

#ضمن "curl_setup.h"

#ضمن "urldata.h"
#ضمن "cfilters.h"

#ضمن "vtls/vtls.h" /* generic SSL protos etc */
#ضمن "vtls/vtls_int.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vtls/keylog.h"

#ضمن "vtls/openssl.h"        /* OpenSSL versions */
#ضمن "vtls/gtls.h"           /* GnuTLS versions */
#ضمن "vtls/wolfssl.h"        /* wolfSSL versions */
#ضمن "vtls/schannel.h"       /* Schannel SSPI version */
#ضمن "vtls/mbedtls.h"        /* mbedTLS versions */
#ضمن "vtls/rustls.h"         /* Rustls versions */

#ضمن "slist.h"
#ضمن "curl_trc.h"
#ضمن "strcase.h"
#ضمن "url.h"
#ضمن "progress.h"
#ضمن "curlx/win32-fopen.h"
#ضمن "curl_sha256.h"
#ضمن "curlx/base64.h"
#ضمن "curlx/inet_pton.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "setopt.h"
#ضمن "vdns/cf-dns.h"
#ضمن "curlx/strdup.h"
#ضمن "curlx/strcopy.h"

#جيڪڏهن_وصف_ٿيل USE_APPLE_SECTRUST
#ضمن <Security/Security.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_SSL
#إذا !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ساكن ثابت بنية alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
ساكن ثابت بنية alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#جيڪڏهن_وصف_ٿيل USE_HTTP2
ساكن ثابت بنية alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
ساكن ثابت بنية alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
ساكن ثابت بنية alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#جيڪڏهن_پورو /* USE_HTTP2 */

ساكن ثابت بنية alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  إذا(!use_alpn)
    أرجع عدم;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  إذا(only_http_10 && (wanted & CURL_HTTP_V1x))
    أرجع &ALPN_SPEC_H10_H11;
#جيڪڏهن_وصف_ٿيل USE_HTTP2
  إذا(wanted & CURL_HTTP_V2x) {
    إذا(wanted & CURL_HTTP_V1x)
      أرجع (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    أرجع &ALPN_SPEC_H2;
  }
#وإلا
  (فراغ)wanted;
  (فراغ)preferred;
#جيڪڏهن_پورو
  أرجع &ALPN_SPEC_H11;
}
#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#جيڪڏهن_پورو /* USE_SSL */

#جيڪڏهن_وصف_ٿيل USE_SSL
ساكن صحيح multissl_تياري(ثابت بنية Curl_ssl *backend);
#جيڪڏهن_پورو

curl_sslbackend Curl_ssl_backend(فراغ)
{
#جيڪڏهن_وصف_ٿيل USE_SSL
  multissl_تياري(عدم);
  أرجع Curl_ssl->info.id;
#وإلا
  أرجع CURLSSLBACKEND_NONE;
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل USE_SSL

/* "global" init done? */
ساكن bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
صحيح Curl_ssl_init(فراغ)
{
  /* make sure this is only done once */
  إذا(init_ssl)
    أرجع 1;
  init_ssl = TRUE; /* never again */

  إذا(Curl_ssl->init)
    أرجع Curl_ssl->init();
  أرجع 1;
}

ساكن bool ssl_prefs_check(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  إذا(conn_config->نسخو_2 >= CURL_SSLVERSION_آخري) {
    failf(معلومات, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    أرجع FALSE;
  }

  بدل(conn_config->نسخو_max) {
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_DEFAULT:
    اكسر;

  افتراضي:
    إذا((conn_config->نسخو_max >> 16) < conn_config->نسخو_2) {
      failf(معلومات, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      أرجع FALSE;
    }
  }

  أرجع TRUE;
}

ساكن بنية ssl_connect_معلومات *cf_ctx_new(بنية Curl_easy *معلومات,
                                           ثابت بنية alpn_spec *alpn)
{
  بنية ssl_connect_معلومات *ctx;

  (فراغ)معلومات;
  ctx = curlx_calloc(1, حجم(*ctx));
  إذا(!ctx)
    أرجع عدم;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_معلومات);
  إذا(!ctx->backend) {
    curlx_free(ctx);
    أرجع عدم;
  }
  أرجع ctx;
}

ساكن فراغ cf_ctx_free(بنية ssl_connect_معلومات *ctx)
{
  إذا(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(بنية Curl_easy *معلومات, int8_t sockindex,
                                      بنية dynbuf *binding)
{
  إذا(Curl_ssl->get_channel_binding)
    أرجع Curl_ssl->get_channel_binding(معلومات, sockindex, binding);
  أرجع CURLE_OK;
}

فراغ Curl_ssl_close_all(بنية Curl_easy *معلومات)
{
  إذا(Curl_ssl->close_all)
    Curl_ssl->close_all(معلومات);
}

CURLcode Curl_ssl_adjust_pollset(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 بنية easy_pollset *ps)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;

  إذا(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->ايندڙ, معلومات);
    CURLcode نتيجو = CURLE_OK;
    إذا(sock != CURL_SOCKET_BAD) {
      إذا(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        نتيجو = Curl_pollset_سيٽ_ٻاهريون_only(معلومات, ps, sock);
        CURL_TRC_CF(معلومات, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      وإلا {
        نتيجو = Curl_pollset_سيٽ_اندريون_only(معلومات, ps, sock);
        CURL_TRC_CF(معلومات, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    أرجع نتيجو;
  }
  أرجع CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
بنية curl_slist *Curl_ssl_engines_list(بنية Curl_easy *معلومات)
{
  إذا(Curl_ssl->engines_list)
    أرجع Curl_ssl->engines_list(معلومات);
  أرجع عدم;
}

ساكن size_t multissl_نسخو(حرف *بفر, size_t سائيز);

فراغ Curl_ssl_نسخو(حرف *بفر, size_t سائيز)
{
#جيڪڏهن_وصف_ٿيل CURL_WITH_MULTI_SSL
  (فراغ)multissl_نسخو(بفر, سائيز);
#وإلا
  (فراغ)Curl_ssl->نسخو_2(بفر, سائيز);
#جيڪڏهن_پورو
}

فراغ Curl_ssl_free_certinfo(بنية Curl_easy *معلومات)
{
  بنية curl_certinfo *ci = &معلومات->info.certs;

  إذا(ci->انگ_جو_certs) {
    /* free all individual lists used */
    صحيح i;
    لكل(i = 0; i < ci->انگ_جو_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = عدم;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->انگ_جو_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(بنية Curl_easy *معلومات, صحيح انگ)
{
  بنية curl_certinfo *ci = &معلومات->info.certs;
  بنية curl_slist **جدول;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(معلومات);

  /* Allocate the required certificate information structures */
  جدول = curlx_calloc((size_t)انگ, حجم(بنية curl_slist *));
  إذا(!جدول)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  ci->انگ_جو_certs = انگ;
  ci->certinfo = جدول;

  أرجع CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ڌڪو_certinfo_len(بنية Curl_easy *معلومات,
                                    صحيح certnum,
                                    ثابت حرف *label,
                                    ثابت حرف *قدر,
                                    size_t valuelen)
{
  بنية curl_certinfo *ci = &معلومات->info.certs;
  بنية curl_slist *nl;
  CURLcode نتيجو = CURLE_OK;
  بنية dynbuf build;

  DEBUGASSERT(certnum < ci->انگ_جو_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  إذا(curlx_dyn_جوڙيو(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, قدر, valuelen))
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  إذا(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  }

  ci->certinfo[certnum] = nl;
  أرجع نتيجو;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(بنية Curl_easy *معلومات,
                         بنانشان حرف *بفر, size_t ڊيگهائي)
{
  DEBUGASSERT(ڊيگهائي == حجم(صحيح));
  إذا(Curl_ssl->random)
    أرجع Curl_ssl->random(معلومات, بفر, ڊيگهائي);
  وإلا
    أرجع CURLE_NOT_BUILT_اندريون;
}

/*
 * Public key pem to der conversion
 */

ساكن CURLcode pubkey_pem_منزل_der(ثابت حرف *pem,
                                  بنانشان حرف **der, size_t *der_len)
{
  ثابت حرف *begin_pos, *پڇاڙي_pos;
  size_t pem_ڳڻپ, pem_len;
  CURLcode نتيجو;
  بنية dynbuf pbuf;

  /* if no pem, exit. */
  إذا(!pem)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_سائيز);

  begin_pos = لڙي_ڳولا(pem, "-----BEGIN PUBLIC KEY-----");
  إذا(!begin_pos)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  pem_ڳڻپ = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  إذا(pem_ڳڻپ && '\n' != pem[pem_ڳڻپ - 1])
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_ڳڻپ += 26;

  /* Invalid if not directly following \n */
  پڇاڙي_pos = لڙي_ڳولا(pem + pem_ڳڻپ, "\n-----END PUBLIC KEY-----");
  إذا(!پڇاڙي_pos)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  pem_len = پڇاڙي_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  طالما(pem_ڳڻپ < pem_len) {
    إذا('\n' != pem[pem_ڳڻپ] && '\r' != pem[pem_ڳڻپ]) {
      نتيجو = curlx_dyn_addn(&pbuf, &pem[pem_ڳڻپ], 1);
      إذا(نتيجو)
        أرجع نتيجو;
    }
    ++pem_ڳڻپ;
  }

  إذا(curlx_dyn_len(&pbuf)) {
    نتيجو = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  وإلا
    نتيجو = CURLE_BAD_CONTENT_ENCODING;

  أرجع نتيجو;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(بنية Curl_easy *معلومات,
                              ثابت حرف *pinnedpubkey,
                              ثابت بنانشان حرف *pubkey, size_t pubkeylen)
{
  CURLcode نتيجو = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  إذا(!pinnedpubkey)
    أرجع CURLE_OK;
  إذا(!pubkey || !pubkeylen)
    أرجع نتيجو;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  إذا(!لڙي_ڀيٽ(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    حرف *cert_hash = عدم;
    ثابت حرف *pinned_hash, *پڇاڙي_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    بنانشان حرف *sha256sumdigest;

    إذا(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      أرجع نتيجو;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_ڊيگهائي);
    إذا(!sha256sumdigest)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_ڊيگهائي);

    إذا(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_ڊيگهائي,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    إذا(encode)
      أرجع encode;

    infof(معلومات, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    طالما(pinned_hash &&
          !لڙي_ڀيٽ(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      پڇاڙي_pos = strchr(pinned_hash, ';');
      pinned_hash_len = پڇاڙي_pos ?
                        (size_t)(پڇاڙي_pos - pinned_hash) : لڙي_ڊيگهه(pinned_hash);

      /* compare base64 sha256 digests */
      إذا(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(معلومات, "public key hash matches pinned value"));
        نتيجو = CURLE_OK;
        اكسر;
      }

      DEBUGF(infof(معلومات, "public key hash does not match 'sha256//%.*s'",
                   (صحيح)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = پڇاڙي_pos ? (پڇاڙي_pos + 1) : عدم;
    }
    curlx_safefree(cert_hash);
  }
  وإلا {
    ڊگهو filesize;
    size_t سائيز, pem_len;
    CURLcode pem_read;
    بنية dynbuf buf;
    بنانشان حرف *pem_ptr = عدم;
    size_t کاٻو;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    إذا(!fp)
      أرجع نتيجو;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_سائيز);

    /* Determine the file's size */
    إذا(fseek(fp, 0, SEEK_END))
      اذهب پڇاڙي;
    filesize = ftell(fp);
    إذا(fseek(fp, 0, SEEK_SET))
      اذهب پڇاڙي;
    إذا(filesize < 0 || filesize > MAX_PINNED_PUBKEY_سائيز)
      اذهب پڇاڙي;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    سائيز = curlx_sotouz((curl_off_t)filesize);
    إذا(pubkeylen > سائيز)
      اذهب پڇاڙي;

    /*
     * Read the file into the dynbuf
     */
    کاٻو = سائيز;
    افعل {
      حرف بفر[1024];
      size_t want = کاٻو > حجم(بفر) ? حجم(بفر) : کاٻو;
      إذا(fread(بفر, 1, want, fp) != want)
        اذهب پڇاڙي;
      إذا(curlx_dyn_addn(&buf, بفر, want))
        اذهب پڇاڙي;
      کاٻو -= want;
    } طالما(کاٻو);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    إذا(pubkeylen == سائيز) {
      إذا(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        نتيجو = CURLE_OK;
      اذهب پڇاڙي;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_منزل_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    إذا(pem_read)
      اذهب پڇاڙي;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    إذا(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      نتيجو = CURLE_OK;
پڇاڙي:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  أرجع نتيجو;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_حالت_request(فراغ)
{
  إذا(Curl_ssl->cert_حالت_request)
    أرجع Curl_ssl->cert_حالت_request();
  أرجع FALSE;
}

ساكن صحيح multissl_init(فراغ)
{
  إذا(multissl_تياري(عدم))
    أرجع 1;
  إذا(Curl_ssl->init)
    أرجع Curl_ssl->init();
  أرجع 1;
}

ساكن CURLcode multissl_random(بنية Curl_easy *معلومات,
                                بنانشان حرف *entropy, size_t ڊيگهائي)
{
  إذا(multissl_تياري(عدم))
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_ssl->random(معلومات, entropy, ڊيگهائي);
}

ساكن CURLcode multissl_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات, bool *پورو)
{
  إذا(multissl_تياري(عدم))
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_ssl->do_connect(cf, معلومات, پورو);
}

ساكن CURLcode multissl_adjust_pollset(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        بنية easy_pollset *ps)
{
  إذا(multissl_تياري(عدم))
    أرجع CURLE_OK;
  أرجع Curl_ssl->adjust_pollset(cf, معلومات, ps);
}

ساكن فراغ *multissl_get_internals(بنية ssl_connect_معلومات *connssl,
                                    CURLINFO info)
{
  إذا(multissl_تياري(عدم))
    أرجع عدم;
  أرجع Curl_ssl->get_internals(connssl, info);
}

ساكن فراغ multissl_close(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  إذا(multissl_تياري(عدم))
    أرجع;
  Curl_ssl->close(cf, معلومات);
}

ساكن CURLcode multissl_recv_plain(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    حرف *buf, size_t len, size_t *pnread)
{
  إذا(multissl_تياري(عدم))
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_ssl->recv_plain(cf, معلومات, buf, len, pnread);
}

ساكن CURLcode multissl_send_plain(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *معلومات,
                                    ثابت فراغ *mem, size_t len,
                                    size_t *pnwritten)
{
  إذا(multissl_تياري(عدم))
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_ssl->send_plain(cf, معلومات, mem, len, pnwritten);
}

ساكن ثابت بنية Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  عدم,                              /* cleanup */
  multissl_نسخو,                  /*نسخو*/
  عدم,                              /* shutdown */
  عدم,                              /* data_pending */
  multissl_random,                   /* random */
  عدم,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  عدم,                              /* close_all */
  عدم,                              /* engines_list */
  عدم,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  عدم,                              /* get_channel_binding */
};

ثابت بنية Curl_ssl *Curl_ssl =
#جيڪڏهن_وصف_ٿيل CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#وإلاإذا defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#وإلاإذا defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#وإلاإذا defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#وإلاإذا defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#وإلاإذا defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#وإلاإذا defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#وإلا
#غلطي "Missing struct Curl_ssl for selected SSL backend"
#جيڪڏهن_پورو

ساكن ثابت بنية Curl_ssl *موجود_backends[] = {
#جيڪڏهن_وصف_ٿيل USE_WOLFSSL
  &Curl_ssl_wolfssl,
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_GNUTLS
  &Curl_ssl_gnutls,
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_MBEDTLS
  &Curl_ssl_mbedtls,
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_OPENSSL
  &Curl_ssl_openssl,
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SCHANNEL
  &Curl_ssl_schannel,
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_RUSTLS
  &Curl_ssl_rustls,
#جيڪڏهن_پورو
  عدم
};

/* Global cleanup */
فراغ Curl_ssl_cleanup(فراغ)
{
  إذا(init_ssl) {
    /* only cleanup if we did a previous init */
    إذا(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#جيڪڏهن_وصف_ٿيل CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#جيڪڏهن_پورو
    init_ssl = FALSE;
  }
}

ساكن size_t multissl_نسخو(حرف *بفر, size_t سائيز)
{
  ساكن ثابت بنية Curl_ssl *selected;
  ساكن حرف backends[200];
  ساكن size_t backends_len;
  ثابت بنية Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? موجود_backends[0] : Curl_ssl;

  إذا(current != selected) {
    حرف *p = backends;
    ثابت حرف *پڇاڙي = backends + حجم(backends);
    صحيح i;

    selected = current;

    backends[0] = '\0';

    لكل(i = 0; موجود_backends[i]; ++i) {
      حرف vb[200];
      bool paren = (selected != موجود_backends[i]);

      إذا(موجود_backends[i]->نسخو_2(vb, حجم(vb))) {
        p += curl_msnprintf(p, پڇاڙي - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  إذا(سائيز) {
    curlx_strcopy(بفر, سائيز, backends, backends_len);
  }
  أرجع 0;
}

ساكن صحيح multissl_تياري(ثابت بنية Curl_ssl *backend)
{
  صحيح i;
  حرف *env;

  إذا(Curl_ssl != &Curl_ssl_multi)
    أرجع 1;

  إذا(backend) {
    Curl_ssl = backend;
    أرجع 0;
  }

  إذا(!موجود_backends[0])
    أرجع 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  إذا(env) {
    لكل(i = 0; موجود_backends[i]; i++) {
      إذا(curl_strequal(env, موجود_backends[i]->info.name)) {
        Curl_ssl = موجود_backends[i];
        curlx_free(env);
        أرجع 0;
      }
    }
  }

#جيڪڏهن_وصف_ٿيل CURL_DEFAULT_SSL_BACKEND
  لكل(i = 0; موجود_backends[i]; i++) {
    إذا(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     موجود_backends[i]->info.name)) {
      Curl_ssl = موجود_backends[i];
      curlx_free(env);
      أرجع 0;
    }
  }
#جيڪڏهن_پورو

  /* Fall back to first available backend */
  Curl_ssl = موجود_backends[0];
  curlx_free(env);
  أرجع 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت حرف *name,
                                   ثابت curl_ssl_backend ***avail)
{
  صحيح i;

  إذا(avail)
    *avail = (ثابت curl_ssl_backend **)&موجود_backends;

  إذا(Curl_ssl != &Curl_ssl_multi)
    أرجع id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#جيڪڏهن_وصف_ٿيل CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#وإلا
           CURLSSLSET_UNKNOWN_BACKEND;
#جيڪڏهن_پورو

  لكل(i = 0; موجود_backends[i]; i++) {
    إذا(موجود_backends[i]->info.id == id ||
       (name && curl_strequal(موجود_backends[i]->info.name, name))) {
      multissl_تياري(موجود_backends[i]);
      أرجع CURLSSLSET_OK;
    }
  }

  أرجع CURLSSLSET_UNKNOWN_BACKEND;
}

#وإلا /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت حرف *name,
                                   ثابت curl_ssl_backend ***avail)
{
  (فراغ)id;
  (فراغ)name;
  (فراغ)avail;
  أرجع CURLSSLSET_NO_BACKENDS;
}

#جيڪڏهن_پورو /* !USE_SSL */

#جيڪڏهن_وصف_ٿيل USE_SSL

فراغ Curl_ssl_peer_cleanup(بنية ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_ڪنجي);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

ساكن ssl_peer_type get_peer_type(ثابت حرف *hostname)
{
  إذا(hostname && hostname[0]) {
#جيڪڏهن_وصف_ٿيل USE_IPV6
    بنية in6_addr addr;
#وإلا
    بنية in_addr addr;
#جيڪڏهن_پورو
    إذا(curlx_inet_pton(AF_INET, hostname, &addr))
      أرجع CURL_SSL_PEER_IPV4;
#جيڪڏهن_وصف_ٿيل USE_IPV6
    وإلا إذا(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      أرجع CURL_SSL_PEER_IPV6;
    }
#جيڪڏهن_پورو
  }
  أرجع CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(بنية ssl_peer *ssl_peer,
                            بنية Curl_peer *origin,
                            بنية Curl_peer *peer,
                            بنية ssl_filter_config *sslc,
                            ثابت حرف *tls_id,
                            uint8_t transport)
{
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  /* We expect a clean struct, e.g. called only ONCE */
  إذا(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    أرجع CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  إذا(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = لڙي_ڊيگهه(origin->hostname);
    إذا(len && (origin->hostname[len - 1] == '.'))
      len--;
    إذا(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      إذا(!ssl_peer->sni)
        اذهب ٻاهريون;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  نتيجو = Curl_ssl_peer_ڪنجي_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_ڪنجي);

ٻاهريون:
  إذا(نتيجو)
    Curl_ssl_peer_cleanup(ssl_peer);
  أرجع نتيجو;
}

ساكن فراغ ssl_cf_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  إذا(connssl) {
    connssl->ssl_impl->close(cf, معلومات);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = عدم;
  }
  cf->connected = FALSE;
}

ساكن CURLcode ssl_cf_connect(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               bool *پورو)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو;

  إذا(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }

  إذا(!cf->ايندڙ || !connssl->peer.origin) {
    *پورو = FALSE;
    أرجع CURLE_FAILED_INIT;
  }

  إذا(!cf->ايندڙ->connected) {
    نتيجو = cf->ايندڙ->cft->do_connect(cf->ايندڙ, معلومات, پورو);
    إذا(نتيجو || !*پورو)
      أرجع نتيجو;
  }

  CF_معلومات_SAVE(save, cf, معلومات);
  CURL_TRC_CF(معلومات, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *پورو = FALSE;

  إذا(!connssl->prefs_checked) {
    إذا(!ssl_prefs_check(cf, معلومات)) {
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اذهب ٻاهريون;
    }
    connssl->prefs_checked = TRUE;
  }

  نتيجو = connssl->ssl_impl->do_connect(cf, معلومات, پورو);

  إذا(!نتيجو && *پورو) {
    cf->connected = TRUE;
    إذا(connssl->state == ssl_connection_complete) {
      connssl->handshake_پورو = *Curl_pgrs_now(معلومات);
    }
    إذا(Curl_tls_keylog_enabled()) {
      infof(معلومات, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#جيڪڏهن_وصف_ٿيل LIBRESSL_نسخو_عدد
      infof(معلومات, "Note LibreSSL only supports SSLKEYLOGFILE for TLS <= 1.2");
#جيڪڏهن_پورو
    }
    /* Connection can be deferred when sending early data */
    DEBUGASSERT(connssl->state == ssl_connection_complete ||
                connssl->state == ssl_connection_deferred);
    DEBUGASSERT(connssl->state != ssl_connection_deferred ||
                connssl->earlydata_state > ssl_earlydata_none);
  }
ٻاهريون:
  CURL_TRC_CF(معلومات, cf, "cf_connect() -> %d, done=%d", (صحيح)نتيجو, *پورو);
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode ssl_cf_سيٽ_earlydata(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *معلومات,
                                     ثابت فراغ *buf, size_t blen)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_خالي_جايون(&connssl->earlydata));
  إذا(blen) {
    إذا(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    نتيجو = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(معلومات, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssl_cf_connect_deferred(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *معلومات,
                                        ثابت فراغ *buf, size_t blen,
                                        bool *پورو)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *پورو = FALSE;
  إذا(connssl->earlydata_state == ssl_earlydata_await) {
    نتيجو = ssl_cf_سيٽ_earlydata(cf, معلومات, buf, blen);
    إذا(نتيجو)
      أرجع نتيجو;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  نتيجو = ssl_cf_connect(cf, معلومات, پورو);

  إذا(!نتيجو && *پورو) {
    إذا(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrs_وقت_Was(معلومات, TIMER_APPCONNECT, connssl->handshake_پورو);
      connssl->stats_reported = TRUE;
    }
    بدل(connssl->earlydata_state) {
    حالة ssl_earlydata_none:
      اكسر;
    حالة ssl_earlydata_accepted:
      إذا(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_معلومات(معلومات, (curl_off_t)connssl->earlydata_skip);
      infof(معلومات, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      اكسر;
    حالة ssl_earlydata_rejected:
      إذا(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_معلومات(معلومات, -(curl_off_t)connssl->earlydata_skip);
      infof(معلومات, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      اكسر;
    افتراضي:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(عدم);
      اكسر;
    }
  }
  أرجع نتيجو;
}

ساكن bool ssl_cf_معلومات_pending(بنية Curl_cfilter *cf,
                                ثابت بنية Curl_easy *معلومات)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية cf_call_معلومات save;
  bool pending;

  CF_معلومات_SAVE(save, cf, معلومات);
  إذا(connssl->ssl_impl->معلومات_pending &&
     connssl->ssl_impl->معلومات_pending(cf, معلومات))
    pending = TRUE;
  وإلا
    pending = cf->ايندڙ->cft->has_معلومات_pending(cf->ايندڙ, معلومات);
  CF_معلومات_RESTORE(cf, save);
  أرجع pending;
}

ساكن CURLcode ssl_cf_send(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            ثابت uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو = CURLE_OK;

  (فراغ)eos;
  *pnwritten = 0;
  CF_معلومات_SAVE(save, cf, معلومات);

  إذا(connssl->state == ssl_connection_deferred) {
    bool پورو = FALSE;
    نتيجو = ssl_cf_connect_deferred(cf, معلومات, buf, blen, &پورو);
    إذا(نتيجو)
      اذهب ٻاهريون;
    وإلا إذا(!پورو) {
      نتيجو = CURLE_AGAIN;
      اذهب ٻاهريون;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  إذا(connssl->earlydata_skip) {
    إذا(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      نتيجو = CURLE_OK;
      *pnwritten = blen;
      اذهب ٻاهريون;
    }
    وإلا {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  إذا(blen > 0) {
    size_t nwritten;
    نتيجو = connssl->ssl_impl->send_plain(cf, معلومات, buf, blen, &nwritten);
    إذا(!نتيجو)
      *pnwritten += nwritten;
  }

ٻاهريون:
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode ssl_cf_recv(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات, حرف *buf, size_t len,
                            size_t *pnread)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو = CURLE_OK;

  CF_معلومات_SAVE(save, cf, معلومات);
  *pnread = 0;
  إذا(connssl->state == ssl_connection_deferred) {
    bool پورو = FALSE;
    نتيجو = ssl_cf_connect_deferred(cf, معلومات, عدم, 0, &پورو);
    إذا(نتيجو)
      اذهب ٻاهريون;
    وإلا إذا(!پورو) {
      نتيجو = CURLE_AGAIN;
      اذهب ٻاهريون;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  نتيجو = connssl->ssl_impl->recv_plain(cf, معلومات, buf, len, pnread);

ٻاهريون:
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode ssl_cf_shutdown(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                bool *پورو)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  *پورو = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  إذا(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_هيٺ_ڏانهن) {
    بنية cf_call_معلومات save;

    CF_معلومات_SAVE(save, cf, معلومات);
    نتيجو = connssl->ssl_impl->shut_هيٺ_ڏانهن(cf, معلومات, TRUE, پورو);
    CURL_TRC_CF(معلومات, cf, "cf_shutdown -> %d, done=%d", (صحيح)نتيجو, *پورو);
    CF_معلومات_RESTORE(cf, save);
    cf->shutdown = (نتيجو || *پورو);
  }
  أرجع نتيجو;
}

ساكن CURLcode ssl_cf_adjust_pollset(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      بنية easy_pollset *ps)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو;

  CF_معلومات_SAVE(save, cf, معلومات);
  نتيجو = connssl->ssl_impl->adjust_pollset(cf, معلومات, ps);
  CF_معلومات_RESTORE(cf, save);
  أرجع نتيجو;
}

ساكن CURLcode ssl_cf_query(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_SSL_INFO:
  حالة CF_QUERY_SSL_CTX_INFO:
    إذا(!Curl_ssl_cf_is_proxy(cf)) {
      بنية curl_tlssessioninfo *info = pres2;
      بنية cf_call_معلومات save;
      CF_معلومات_SAVE(save, cf, معلومات);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_معلومات_RESTORE(cf, save);
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(معلومات, cf, "query ALPN: returning '%s'", *palpn);
    أرجع CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->query(cf->ايندڙ, معلومات, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode ssl_cf_cntrl(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;

  (فراغ)arg1;
  (فراغ)arg2;
  (فراغ)معلومات;
  بدل(event) {
  حالة CF_CTRL_CONN_INFO_UPDATE:
    إذا(connssl->negotiated.alpn && !cf->sockindex) {
      إذا(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_ڏٺل = 11;
      وإلا إذا(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_ڏٺل = 20;
      وإلا إذا(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_ڏٺل = 30;
    }
    اكسر;
  حالة CF_CTRL_REPORT_STATS:
    إذا(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_پورو.tv_sec || connssl->handshake_پورو.tv_usec)) {
      Curl_pgrs_وقت_Was(معلومات, TIMER_APPCONNECT, connssl->handshake_پورو);
      connssl->stats_reported = TRUE;
    }
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن bool cf_ssl_is_alive(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات,
                            bool *داخلا_pending)
{
  /*
   * This function tries to determine connection status.
   */
  أرجع cf->ايندڙ ?
    cf->ايندڙ->cft->is_alive(cf->ايندڙ, معلومات, داخلا_pending) :
    FALSE; /* pessimistic in absence of data */
}

بنية Curl_cftype Curl_cft_ssl = {
  "SSL",
  CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_معلومات_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY

بنية Curl_cftype Curl_cft_ssl_proxy = {
  "SSL-PROXY",
  CF_TYPE_SSL | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_معلومات_pending,
  ssl_cf_send,
  ssl_cf_recv,
  Curl_cf_def_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY */

ساكن CURLcode cf_ssl_create(بنية Curl_cfilter **pcf,
                              بنية Curl_easy *معلومات,
                              بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = عدم;
  بنية ssl_connect_معلومات *ctx;
  CURLcode نتيجو;

  DEBUGASSERT(معلومات->conn);

#جيڪڏهن_وصف_ٿيل CURL_DISABLE_HTTP
  (فراغ)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->بٽ.tls_enable_alpn);
  ctx = cf_ctx_new(معلومات, عدم);
#وإلا
  ctx = cf_ctx_new(معلومات, alpn_get_spec(معلومات->state.http_neg.wanted,
                                       معلومات->state.http_neg.preferred,
                                       (bool)معلومات->state.http_neg.only_10,
                                       (bool)conn->بٽ.tls_enable_alpn));
#جيڪڏهن_پورو
  إذا(!ctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }

  نتيجو = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

ٻاهريون:
  إذا(نتيجو)
    cf_ctx_free(ctx);
  *pcf = نتيجو ? عدم : cf;
  أرجع نتيجو;
}

ساكن CURLcode cf_ssl_peer_init(بنية Curl_cfilter *cf,
                                 بنية Curl_peer *origin,
                                 بنية Curl_peer *peer,
                                 بنية ssl_filter_config *sslc)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  حرف tls_id[80];
  connssl->ssl_impl->نسخو_2(tls_id, حجم(tls_id) - 1);
  أرجع Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_جوڙيو(بنية Curl_easy *معلومات,
                              بنية Curl_peer *origin,
                              بنية connectdata *conn,
                              int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  بنية Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->ذريعي_peer2 : conn->ذريعي_peer;
  CURLcode نتيجو;

  نتيجو = cf_ssl_create(&cf, معلومات, conn);
  إذا(!نتيجو)
    نتيجو = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  إذا(!نتيجو)
    Curl_conn_cf_جوڙيو(معلومات, conn, sockindex, cf);
  وإلا إذا(cf)
    Curl_conn_cf_discard_chain(&cf, معلومات);
  أرجع نتيجو;
}

CURLcode Curl_cf_ssl_وجهو_after(بنية Curl_cfilter *cf_at,
                                  بنية Curl_easy *معلومات,
                                  بنية Curl_peer *origin,
                                  بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو;

  نتيجو = cf_ssl_create(&cf, معلومات, cf_at->conn);
  إذا(!نتيجو)
    نتيجو = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  إذا(!نتيجو) {
    Curl_conn_cf_وجهو_after(cf_at, cf);
#إذا defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    إذا((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      نتيجو = Curl_conn_dns_جوڙيو_https_resolve(معلومات, cf->conn, cf->sockindex,
                                               origin);
    }
#جيڪڏهن_پورو /* USE_HTTPSRR && USE_ECH */
  }
  وإلا إذا(cf)
    Curl_conn_cf_discard_chain(&cf, معلومات);
  أرجع نتيجو;
}

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY

ساكن CURLcode cf_ssl_proxy_create(بنية Curl_cfilter **pcf,
                                    بنية Curl_easy *معلومات,
                                    بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = عدم;
  بنية ssl_connect_معلومات *ctx;
  CURLcode نتيجو;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)معلومات->سيٽ.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (فراغ)conn;
#جيڪڏهن_وصف_ٿيل USE_HTTP2
  إذا(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#جيڪڏهن_پورو

  ctx = cf_ctx_new(معلومات, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  إذا(!ctx) {
    نتيجو = CURLE_ٻاهريون_جو_يادگيري;
    اذهب ٻاهريون;
  }
  نتيجو = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

ٻاهريون:
  إذا(نتيجو)
    cf_ctx_free(ctx);
  *pcf = نتيجو ? عدم : cf;
  أرجع نتيجو;
}

CURLcode Curl_cf_ssl_proxy_وجهو_after(بنية Curl_cfilter *cf_at,
                                        بنية Curl_easy *معلومات,
                                        بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf;
  CURLcode نتيجو;

  نتيجو = cf_ssl_proxy_create(&cf, معلومات, cf_at->conn);
  إذا(!نتيجو)
    نتيجو = cf_ssl_peer_init(cf, peer, عدم, &cf_at->conn->proxy_ssl_config);
  إذا(!نتيجو)
    Curl_conn_cf_وجهو_after(cf_at, cf);
  وإلا إذا(cf)
    Curl_conn_cf_discard_chain(&cf, معلومات);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(بنية Curl_easy *معلومات, بنانشان صحيح ssl_option)
{
  (فراغ)معلومات;
  أرجع (Curl_ssl->supports & ssl_option);
}

#جيڪڏهن_وصف_ٿيل USE_ECH
bool Curl_ssl_ech_enabled(بنية Curl_cfilter *cf)
{
  إذا(Curl_ssl->supports & SSLSUPP_ECH) {
    بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    أرجع !!conn_config->ech;
  }
  أرجع FALSE;
}

bool Curl_ssl_need_httpsrr(بنية Curl_cfilter *cf)
{
  إذا(Curl_ssl->supports & SSLSUPP_ECH) {
    بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    إذا(!conn_config->ech)
      أرجع FALSE;
    إذا((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      أرجع FALSE;
    أرجع TRUE;
  }
  أرجع FALSE;
}
#جيڪڏهن_پورو

ساكن CURLcode vtls_shutdown_blocking(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *معلومات,
                                       bool send_shutdown, bool *پورو)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  بنية cf_call_معلومات save;
  CURLcode نتيجو = CURLE_OK;
  timediff_t timeout_ms;
  صحيح what, loop = 10;

  إذا(cf->shutdown) {
    *پورو = TRUE;
    أرجع CURLE_OK;
  }
  CF_معلومات_SAVE(save, cf, معلومات);

  *پورو = FALSE;
  طالما(!نتيجو && !*پورو && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(معلومات, cf->conn, cf->sockindex);

    إذا(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(معلومات, "SSL shutdown timeout");
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      اذهب ٻاهريون;
    }

    نتيجو = connssl->ssl_impl->shut_هيٺ_ڏانهن(cf, معلومات, send_shutdown, پورو);
    إذا(نتيجو || *پورو)
      اذهب ٻاهريون;

    إذا(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, معلومات, timeout_ms);
      إذا(what < 0) {
        /* fatal error */
        failf(معلومات, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        نتيجو = CURLE_RECV_ERROR;
        اذهب ٻاهريون;
      }
      وإلا إذا(what == 0) {
        /* timeout */
        failf(معلومات, "SSL shutdown timeout");
        نتيجو = CURLE_OPERATION_TIMEDOUT;
        اذهب ٻاهريون;
      }
      /* socket is readable or writable */
    }
  }
ٻاهريون:
  CF_معلومات_RESTORE(cf, save);
  cf->shutdown = (نتيجو || *پورو);
  أرجع نتيجو;
}

CURLcode Curl_ssl_cfilter_remove(بنية Curl_easy *معلومات,
                                 int8_t sockindex, bool send_shutdown)
{
  بنية Curl_cfilter *cf, *سر;
  CURLcode نتيجو = CURLE_OK;

  سر = معلومات->conn ? معلومات->conn->cfilter[sockindex] : عدم;
  لكل(cf = سر; cf; cf = cf->ايندڙ) {
    إذا(cf->cft == &Curl_cft_ssl) {
      bool پورو;
      CURL_TRC_CF(معلومات, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_شروعات_timer(معلومات, sockindex, 0);
      نتيجو = vtls_shutdown_blocking(cf, معلومات, send_shutdown, &پورو);
      Curl_cshutdn_clear_timer(معلومات, sockindex);
      إذا(!نتيجو && !پورو) /* blocking failed? */
        نتيجو = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, معلومات);
      CURL_TRC_CF(معلومات, cf, "shutdown and remove SSL, done -> %d",
                  (صحيح)نتيجو);
      اكسر;
    }
  }
  أرجع نتيجو;
}

bool Curl_ssl_cf_is_proxy(بنية Curl_cfilter *cf)
{
  أرجع (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

بنية ssl_easy_config *
Curl_ssl_cf_get_easy_config(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات)
{
#جيڪڏهن_وصف_ٿيل CURL_DISABLE_PROXY
  (فراغ)cf;
  أرجع &معلومات->سيٽ.ssl;
#وإلا
  أرجع Curl_ssl_cf_is_proxy(cf) ? &معلومات->سيٽ.proxy_ssl : &معلومات->سيٽ.ssl;
#جيڪڏهن_پورو
}

بنية ssl_filter_config *
Curl_ssl_cf_get_filter_config(بنية Curl_cfilter *cf)
{
#جيڪڏهن_وصف_ٿيل CURL_DISABLE_PROXY
  أرجع &cf->conn->ssl_config;
#وإلا
  أرجع Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#جيڪڏهن_پورو
}

CURLcode Curl_alpn_منزل_proto_buf(بنية alpn_proto_buf *buf,
                                ثابت بنية alpn_spec *spec)
{
  size_t i, len;
  صحيح off = 0;
  بنانشان حرف blen;

  memset(buf, 0, حجم(*buf));
  لكل(i = 0; spec && i < spec->ڳڻپ; ++i) {
    len = لڙي_ڊيگهه(spec->entries[i]);
    إذا(len >= ALPN_NAME_MAX)
      أرجع CURLE_FAILED_INIT;
    blen = (بنانشان حرف)len;
    إذا(off + blen + 1 >= (صحيح)حجم(buf->معلومات))
      أرجع CURLE_FAILED_INIT;
    buf->معلومات[off++] = blen;
    يادگيري_نقل(buf->معلومات + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  أرجع CURLE_OK;
}

CURLcode Curl_alpn_منزل_proto_str(بنية alpn_proto_buf *buf,
                                ثابت بنية alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, حجم(*buf));
  لكل(i = 0; spec && i < spec->ڳڻپ; ++i) {
    len = لڙي_ڊيگهه(spec->entries[i]);
    إذا(len >= ALPN_NAME_MAX)
      أرجع CURLE_FAILED_INIT;
    إذا(off + len + 2 >= حجم(buf->معلومات))
      أرجع CURLE_FAILED_INIT;
    إذا(off)
      buf->معلومات[off++] = ',';
    يادگيري_نقل(buf->معلومات + off, spec->entries[i], len);
    off += len;
  }
  buf->معلومات[off] = '\0';
  buf->len = (صحيح)off;
  أرجع CURLE_OK;
}

bool Curl_alpn_شامل_آهي_proto(ثابت بنية alpn_spec *spec,
                              ثابت حرف *proto)
{
  size_t i, plen = proto ? لڙي_ڊيگهه(proto) : 0;
  لكل(i = 0; spec && plen && i < spec->ڳڻپ; ++i) {
    size_t slen = لڙي_ڊيگهه(spec->entries[i]);
    إذا((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      أرجع TRUE;
  }
  أرجع FALSE;
}

فراغ Curl_alpn_restrict_منزل(بنية alpn_spec *spec, ثابت حرف *proto)
{
  size_t plen = لڙي_ڊيگهه(proto);
  DEBUGASSERT(plen < حجم(spec->entries[0]));
  إذا(plen < حجم(spec->entries[0])) {
    يادگيري_نقل(spec->entries[0], proto, plen + 1);
    spec->ڳڻپ = 1;
  }
}

فراغ Curl_alpn_copy(بنية alpn_spec *dest, ثابت بنية alpn_spec *src)
{
  إذا(src)
    يادگيري_نقل(dest, src, حجم(*dest));
  وإلا
    memset(dest, 0, حجم(*dest));
}

CURLcode Curl_alpn_سيٽ_negotiated(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *معلومات,
                                  بنية ssl_connect_معلومات *connssl,
                                  ثابت بنانشان حرف *proto,
                                  size_t proto_len)
{
  CURLcode نتيجو = CURLE_OK;
  (فراغ)cf;

  إذا(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    إذا(!proto_len) {
      failf(معلومات, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اذهب ٻاهريون;
    }
    وإلا إذا(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اذهب ٻاهريون;
    }
    وإلا إذا((لڙي_ڊيگهه(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(معلومات, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (صحيح)proto_len, proto);
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اذهب ٻاهريون;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(معلومات, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    اذهب ٻاهريون;
  }

  إذا(proto && proto_len) {
    إذا(memchr(proto, '\0', proto_len)) {
      failf(معلومات, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      نتيجو = CURLE_SSL_CONNECT_ERROR;
      اذهب ٻاهريون;
    }
    connssl->negotiated.alpn = curlx_memdup0((ثابت حرف *)proto, proto_len);
    إذا(!connssl->negotiated.alpn)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  إذا(proto && proto_len) {
    إذا(connssl->state == ssl_connection_deferred)
      infof(معلومات, VTLS_INFOF_ALPN_DEFERRED, (صحيح)proto_len, proto);
    وإلا
      infof(معلومات, VTLS_INFOF_ALPN_ACCEPTED, (صحيح)proto_len, proto);
  }
  وإلا {
    إذا(connssl->state == ssl_connection_deferred)
      infof(معلومات, VTLS_INFOF_NO_ALPN_DEFERRED);
    وإلا
      infof(معلومات, VTLS_INFOF_NO_ALPN);
  }

ٻاهريون:
  أرجع نتيجو;
}

CURLcode Curl_on_session_reuse(بنية Curl_cfilter *cf,
                               بنية Curl_easy *معلومات,
                               بنية alpn_spec *alpns,
                               بنية Curl_ssl_session *scs,
                               bool *do_early_معلومات, bool early_معلومات_allowed)
{
  بنية ssl_connect_معلومات *connssl = cf->ctx;
  CURLcode نتيجو = CURLE_OK;

  *do_early_معلومات = FALSE;

  إذا(!early_معلومات_allowed) {
    CURL_TRC_CF(معلومات, cf, "SSL session does not allow earlydata");
  }
  وإلا إذا(!Curl_alpn_شامل_آهي_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(معلومات, cf, "SSL session has different ALPN, no early data");
  }
  وإلا {
    infof(معلومات, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    نتيجو = Curl_alpn_سيٽ_negotiated(cf, معلومات, connssl,
                                      (ثابت بنانشان حرف *)scs->alpn,
                                      scs->alpn ? لڙي_ڊيگهه(scs->alpn) : 0);
    *do_early_معلومات = !نتيجو;
  }
  أرجع نتيجو;
}

بنية Curl_ssl_session *Curl_ssl_get_cf_session(بنية Curl_easy *معلومات,
                                                 ثابت بنية Curl_cftype *cft,
                                                 int8_t sockindex)
{
  إذا(معلومات->conn &&
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#وإلا
     (cft == &Curl_cft_ssl)) {
#جيڪڏهن_پورو
    بنية Curl_cfilter *cf1 = معلومات->conn->cfilter[sockindex];
    لكل(; cf1; cf1 = cf1->ايندڙ) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      إذا(cf1->cft == cft)
        اكسر;
    }
    إذا(cf1) {
      بنية ssl_connect_معلومات *connssl = cf1->ctx;
      إذا(connssl)
        أرجع connssl->session;
    }
  }
  أرجع عدم;
}

#جيڪڏهن_پورو /* USE_SSL */
