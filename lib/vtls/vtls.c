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

#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "cfilters.h"

#شامل "vtls/vtls.h" /* generic SSL protos etc */
#شامل "vtls/vtls_int.h"
#شامل "vtls/vtls_scache.h"
#شامل "vtls/keylog.h"

#شامل "vtls/openssl.h"        /* OpenSSL versions */
#شامل "vtls/gtls.h"           /* GnuTLS versions */
#شامل "vtls/wolfssl.h"        /* wolfSSL versions */
#شامل "vtls/schannel.h"       /* Schannel SSPI version */
#شامل "vtls/mbedtls.h"        /* mbedTLS versions */
#شامل "vtls/rustls.h"         /* Rustls versions */

#شامل "slist.h"
#شامل "curl_trc.h"
#شامل "strcase.h"
#شامل "url.h"
#شامل "progress.h"
#شامل "curlx/win32-fopen.h"
#شامل "curl_sha256.h"
#شامل "curlx/base64.h"
#شامل "curlx/inet_pton.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "setopt.h"
#شامل "vdns/cf-dns.h"
#شامل "curlx/strdup.h"
#شامل "curlx/strcopy.h"

#اگر_تعریف_شدہ USE_APPLE_SECTRUST
#شامل <Security/Security.h>
#اگر_ختم

#اگر_تعریف_شدہ USE_SSL
#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ساکن ثابت ساخت alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
ساکن ثابت ساخت alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#اگر_تعریف_شدہ USE_HTTP2
ساکن ثابت ساخت alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
ساکن ثابت ساخت alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
ساکن ثابت ساخت alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#اگر_ختم /* USE_HTTP2 */

ساکن ثابت ساخت alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  اگر(!use_alpn)
    واپس تہی;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  اگر(only_http_10 && (wanted & CURL_HTTP_V1x))
    واپس &ALPN_SPEC_H10_H11;
#اگر_تعریف_شدہ USE_HTTP2
  اگر(wanted & CURL_HTTP_V2x) {
    اگر(wanted & CURL_HTTP_V1x)
      واپس (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    واپس &ALPN_SPEC_H2;
  }
#ورنہ
  (خالی)wanted;
  (خالی)preferred;
#اگر_ختم
  واپس &ALPN_SPEC_H11;
}
#اگر_ختم /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#اگر_ختم /* USE_SSL */

#اگر_تعریف_شدہ USE_SSL
ساکن صحیح multissl_تیاری(ثابت ساخت Curl_ssl *backend);
#اگر_ختم

curl_sslbackend Curl_ssl_backend(خالی)
{
#اگر_تعریف_شدہ USE_SSL
  multissl_تیاری(تہی);
  واپس Curl_ssl->info.id;
#ورنہ
  واپس CURLSSLBACKEND_NONE;
#اگر_ختم
}

#اگر_تعریف_شدہ USE_SSL

/* "global" init done? */
ساکن bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
صحیح Curl_ssl_init(خالی)
{
  /* make sure this is only done once */
  اگر(init_ssl)
    واپس 1;
  init_ssl = TRUE; /* never again */

  اگر(Curl_ssl->init)
    واپس Curl_ssl->init();
  واپس 1;
}

ساکن bool ssl_prefs_check(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  اگر(conn_config->نسخہ_2 >= CURL_SSLVERSION_آخری) {
    failf(ڈیٹا, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    واپس FALSE;
  }

  انتخاب(conn_config->نسخہ_max) {
  حالت CURL_SSLVERSION_MAX_NONE:
  حالت CURL_SSLVERSION_MAX_DEFAULT:
    توڑو;

  طےشدہ:
    اگر((conn_config->نسخہ_max >> 16) < conn_config->نسخہ_2) {
      failf(ڈیٹا, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      واپس FALSE;
    }
  }

  واپس TRUE;
}

ساکن ساخت ssl_connect_ڈیٹا *cf_ctx_new(ساخت Curl_easy *ڈیٹا,
                                           ثابت ساخت alpn_spec *alpn)
{
  ساخت ssl_connect_ڈیٹا *ctx;

  (خالی)ڈیٹا;
  ctx = curlx_calloc(1, حجم(*ctx));
  اگر(!ctx)
    واپس تہی;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_ڈیٹا);
  اگر(!ctx->backend) {
    curlx_free(ctx);
    واپس تہی;
  }
  واپس ctx;
}

ساکن خالی cf_ctx_free(ساخت ssl_connect_ڈیٹا *ctx)
{
  اگر(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(ساخت Curl_easy *ڈیٹا, int8_t sockindex,
                                      ساخت dynbuf *binding)
{
  اگر(Curl_ssl->get_channel_binding)
    واپس Curl_ssl->get_channel_binding(ڈیٹا, sockindex, binding);
  واپس CURLE_OK;
}

خالی Curl_ssl_close_all(ساخت Curl_easy *ڈیٹا)
{
  اگر(Curl_ssl->close_all)
    Curl_ssl->close_all(ڈیٹا);
}

CURLcode Curl_ssl_adjust_pollset(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ساخت easy_pollset *ps)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;

  اگر(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->اگلا, ڈیٹا);
    CURLcode نتیجہ = CURLE_OK;
    اگر(sock != CURL_SOCKET_BAD) {
      اگر(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        نتیجہ = Curl_pollset_سیٹ_برآمد_only(ڈیٹا, ps, sock);
        CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      ورنہ {
        نتیجہ = Curl_pollset_سیٹ_اندر_only(ڈیٹا, ps, sock);
        CURL_TRC_CF(ڈیٹا, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    واپس نتیجہ;
  }
  واپس CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
ساخت curl_slist *Curl_ssl_engines_list(ساخت Curl_easy *ڈیٹا)
{
  اگر(Curl_ssl->engines_list)
    واپس Curl_ssl->engines_list(ڈیٹا);
  واپس تہی;
}

ساکن size_t multissl_نسخہ(حرف *بفر, size_t سائز);

خالی Curl_ssl_نسخہ(حرف *بفر, size_t سائز)
{
#اگر_تعریف_شدہ CURL_WITH_MULTI_SSL
  (خالی)multissl_نسخہ(بفر, سائز);
#ورنہ
  (خالی)Curl_ssl->نسخہ_2(بفر, سائز);
#اگر_ختم
}

خالی Curl_ssl_free_certinfo(ساخت Curl_easy *ڈیٹا)
{
  ساخت curl_certinfo *ci = &ڈیٹا->info.certs;

  اگر(ci->عدد_کا_certs) {
    /* free all individual lists used */
    صحیح i;
    برائے(i = 0; i < ci->عدد_کا_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = تہی;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->عدد_کا_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(ساخت Curl_easy *ڈیٹا, صحیح عدد)
{
  ساخت curl_certinfo *ci = &ڈیٹا->info.certs;
  ساخت curl_slist **جدول;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(ڈیٹا);

  /* Allocate the required certificate information structures */
  جدول = curlx_calloc((size_t)عدد, حجم(ساخت curl_slist *));
  اگر(!جدول)
    واپس CURLE_برآمد_کا_حافظہ;

  ci->عدد_کا_certs = عدد;
  ci->certinfo = جدول;

  واپس CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ڈالو_certinfo_len(ساخت Curl_easy *ڈیٹا,
                                    صحیح certnum,
                                    ثابت حرف *label,
                                    ثابت حرف *قدر,
                                    size_t valuelen)
{
  ساخت curl_certinfo *ci = &ڈیٹا->info.certs;
  ساخت curl_slist *nl;
  CURLcode نتیجہ = CURLE_OK;
  ساخت dynbuf build;

  DEBUGASSERT(certnum < ci->عدد_کا_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  اگر(curlx_dyn_جوڑو(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, قدر, valuelen))
    واپس CURLE_برآمد_کا_حافظہ;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  اگر(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    نتیجہ = CURLE_برآمد_کا_حافظہ;
  }

  ci->certinfo[certnum] = nl;
  واپس نتیجہ;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(ساخت Curl_easy *ڈیٹا,
                         غیرعلامتی حرف *بفر, size_t لمبائی)
{
  DEBUGASSERT(لمبائی == حجم(صحیح));
  اگر(Curl_ssl->random)
    واپس Curl_ssl->random(ڈیٹا, بفر, لمبائی);
  ورنہ
    واپس CURLE_NOT_BUILT_اندر;
}

/*
 * Public key pem to der conversion
 */

ساکن CURLcode pubkey_pem_مقصد_der(ثابت حرف *pem,
                                  غیرعلامتی حرف **der, size_t *der_len)
{
  ثابت حرف *begin_pos, *انتہا_pos;
  size_t pem_گنتی, pem_len;
  CURLcode نتیجہ;
  ساخت dynbuf pbuf;

  /* if no pem, exit. */
  اگر(!pem)
    واپس CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_سائز);

  begin_pos = لڑی_تلاش(pem, "-----BEGIN PUBLIC KEY-----");
  اگر(!begin_pos)
    واپس CURLE_BAD_CONTENT_ENCODING;

  pem_گنتی = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  اگر(pem_گنتی && '\n' != pem[pem_گنتی - 1])
    واپس CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_گنتی += 26;

  /* Invalid if not directly following \n */
  انتہا_pos = لڑی_تلاش(pem + pem_گنتی, "\n-----END PUBLIC KEY-----");
  اگر(!انتہا_pos)
    واپس CURLE_BAD_CONTENT_ENCODING;

  pem_len = انتہا_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  جب(pem_گنتی < pem_len) {
    اگر('\n' != pem[pem_گنتی] && '\r' != pem[pem_گنتی]) {
      نتیجہ = curlx_dyn_addn(&pbuf, &pem[pem_گنتی], 1);
      اگر(نتیجہ)
        واپس نتیجہ;
    }
    ++pem_گنتی;
  }

  اگر(curlx_dyn_len(&pbuf)) {
    نتیجہ = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  ورنہ
    نتیجہ = CURLE_BAD_CONTENT_ENCODING;

  واپس نتیجہ;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *pinnedpubkey,
                              ثابت غیرعلامتی حرف *pubkey, size_t pubkeylen)
{
  CURLcode نتیجہ = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  اگر(!pinnedpubkey)
    واپس CURLE_OK;
  اگر(!pubkey || !pubkeylen)
    واپس نتیجہ;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  اگر(!لڑی_موازنہ(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    حرف *cert_hash = تہی;
    ثابت حرف *pinned_hash, *انتہا_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    غیرعلامتی حرف *sha256sumdigest;

    اگر(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      واپس نتیجہ;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_لمبائی);
    اگر(!sha256sumdigest)
      واپس CURLE_برآمد_کا_حافظہ;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_لمبائی);

    اگر(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_لمبائی,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    اگر(encode)
      واپس encode;

    infof(ڈیٹا, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    جب(pinned_hash &&
          !لڑی_موازنہ(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      انتہا_pos = strchr(pinned_hash, ';');
      pinned_hash_len = انتہا_pos ?
                        (size_t)(انتہا_pos - pinned_hash) : لڑی_لمبائی(pinned_hash);

      /* compare base64 sha256 digests */
      اگر(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(ڈیٹا, "public key hash matches pinned value"));
        نتیجہ = CURLE_OK;
        توڑو;
      }

      DEBUGF(infof(ڈیٹا, "public key hash does not match 'sha256//%.*s'",
                   (صحیح)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = انتہا_pos ? (انتہا_pos + 1) : تہی;
    }
    curlx_safefree(cert_hash);
  }
  ورنہ {
    طویل filesize;
    size_t سائز, pem_len;
    CURLcode pem_read;
    ساخت dynbuf buf;
    غیرعلامتی حرف *pem_ptr = تہی;
    size_t بایاں;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    اگر(!fp)
      واپس نتیجہ;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_سائز);

    /* Determine the file's size */
    اگر(fseek(fp, 0, SEEK_END))
      جاؤ انتہا;
    filesize = ftell(fp);
    اگر(fseek(fp, 0, SEEK_SET))
      جاؤ انتہا;
    اگر(filesize < 0 || filesize > MAX_PINNED_PUBKEY_سائز)
      جاؤ انتہا;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    سائز = curlx_sotouz((curl_off_t)filesize);
    اگر(pubkeylen > سائز)
      جاؤ انتہا;

    /*
     * Read the file into the dynbuf
     */
    بایاں = سائز;
    کرو {
      حرف بفر[1024];
      size_t want = بایاں > حجم(بفر) ? حجم(بفر) : بایاں;
      اگر(fread(بفر, 1, want, fp) != want)
        جاؤ انتہا;
      اگر(curlx_dyn_addn(&buf, بفر, want))
        جاؤ انتہا;
      بایاں -= want;
    } جب(بایاں);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    اگر(pubkeylen == سائز) {
      اگر(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        نتیجہ = CURLE_OK;
      جاؤ انتہا;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_مقصد_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    اگر(pem_read)
      جاؤ انتہا;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    اگر(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      نتیجہ = CURLE_OK;
انتہا:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  واپس نتیجہ;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_حالت_request(خالی)
{
  اگر(Curl_ssl->cert_حالت_request)
    واپس Curl_ssl->cert_حالت_request();
  واپس FALSE;
}

ساکن صحیح multissl_init(خالی)
{
  اگر(multissl_تیاری(تہی))
    واپس 1;
  اگر(Curl_ssl->init)
    واپس Curl_ssl->init();
  واپس 1;
}

ساکن CURLcode multissl_random(ساخت Curl_easy *ڈیٹا,
                                غیرعلامتی حرف *entropy, size_t لمبائی)
{
  اگر(multissl_تیاری(تہی))
    واپس CURLE_FAILED_INIT;
  واپس Curl_ssl->random(ڈیٹا, entropy, لمبائی);
}

ساکن CURLcode multissl_connect(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  اگر(multissl_تیاری(تہی))
    واپس CURLE_FAILED_INIT;
  واپس Curl_ssl->do_connect(cf, ڈیٹا, مکمل);
}

ساکن CURLcode multissl_adjust_pollset(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت easy_pollset *ps)
{
  اگر(multissl_تیاری(تہی))
    واپس CURLE_OK;
  واپس Curl_ssl->adjust_pollset(cf, ڈیٹا, ps);
}

ساکن خالی *multissl_get_internals(ساخت ssl_connect_ڈیٹا *connssl,
                                    CURLINFO info)
{
  اگر(multissl_تیاری(تہی))
    واپس تہی;
  واپس Curl_ssl->get_internals(connssl, info);
}

ساکن خالی multissl_close(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  اگر(multissl_تیاری(تہی))
    واپس;
  Curl_ssl->close(cf, ڈیٹا);
}

ساکن CURLcode multissl_recv_plain(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    حرف *buf, size_t len, size_t *pnread)
{
  اگر(multissl_تیاری(تہی))
    واپس CURLE_FAILED_INIT;
  واپس Curl_ssl->recv_plain(cf, ڈیٹا, buf, len, pnread);
}

ساکن CURLcode multissl_send_plain(ساخت Curl_cfilter *cf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ثابت خالی *mem, size_t len,
                                    size_t *pnwritten)
{
  اگر(multissl_تیاری(تہی))
    واپس CURLE_FAILED_INIT;
  واپس Curl_ssl->send_plain(cf, ڈیٹا, mem, len, pnwritten);
}

ساکن ثابت ساخت Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  تہی,                              /* cleanup */
  multissl_نسخہ,                  /*نسخہ*/
  تہی,                              /* shutdown */
  تہی,                              /* data_pending */
  multissl_random,                   /* random */
  تہی,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  تہی,                              /* close_all */
  تہی,                              /* engines_list */
  تہی,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  تہی,                              /* get_channel_binding */
};

ثابت ساخت Curl_ssl *Curl_ssl =
#اگر_تعریف_شدہ CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#وگرنہ_اگر defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#وگرنہ_اگر defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#وگرنہ_اگر defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#وگرنہ_اگر defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#وگرنہ_اگر defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#وگرنہ_اگر defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#ورنہ
#غلطی "Missing struct Curl_ssl for selected SSL backend"
#اگر_ختم

ساکن ثابت ساخت Curl_ssl *دستیاب_backends[] = {
#اگر_تعریف_شدہ USE_WOLFSSL
  &Curl_ssl_wolfssl,
#اگر_ختم
#اگر_تعریف_شدہ USE_GNUTLS
  &Curl_ssl_gnutls,
#اگر_ختم
#اگر_تعریف_شدہ USE_MBEDTLS
  &Curl_ssl_mbedtls,
#اگر_ختم
#اگر_تعریف_شدہ USE_OPENSSL
  &Curl_ssl_openssl,
#اگر_ختم
#اگر_تعریف_شدہ USE_SCHANNEL
  &Curl_ssl_schannel,
#اگر_ختم
#اگر_تعریف_شدہ USE_RUSTLS
  &Curl_ssl_rustls,
#اگر_ختم
  تہی
};

/* Global cleanup */
خالی Curl_ssl_cleanup(خالی)
{
  اگر(init_ssl) {
    /* only cleanup if we did a previous init */
    اگر(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#اگر_تعریف_شدہ CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#اگر_ختم
    init_ssl = FALSE;
  }
}

ساکن size_t multissl_نسخہ(حرف *بفر, size_t سائز)
{
  ساکن ثابت ساخت Curl_ssl *selected;
  ساکن حرف backends[200];
  ساکن size_t backends_len;
  ثابت ساخت Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? دستیاب_backends[0] : Curl_ssl;

  اگر(current != selected) {
    حرف *p = backends;
    ثابت حرف *انتہا = backends + حجم(backends);
    صحیح i;

    selected = current;

    backends[0] = '\0';

    برائے(i = 0; دستیاب_backends[i]; ++i) {
      حرف vb[200];
      bool paren = (selected != دستیاب_backends[i]);

      اگر(دستیاب_backends[i]->نسخہ_2(vb, حجم(vb))) {
        p += curl_msnprintf(p, انتہا - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  اگر(سائز) {
    curlx_strcopy(بفر, سائز, backends, backends_len);
  }
  واپس 0;
}

ساکن صحیح multissl_تیاری(ثابت ساخت Curl_ssl *backend)
{
  صحیح i;
  حرف *env;

  اگر(Curl_ssl != &Curl_ssl_multi)
    واپس 1;

  اگر(backend) {
    Curl_ssl = backend;
    واپس 0;
  }

  اگر(!دستیاب_backends[0])
    واپس 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  اگر(env) {
    برائے(i = 0; دستیاب_backends[i]; i++) {
      اگر(curl_strequal(env, دستیاب_backends[i]->info.name)) {
        Curl_ssl = دستیاب_backends[i];
        curlx_free(env);
        واپس 0;
      }
    }
  }

#اگر_تعریف_شدہ CURL_DEFAULT_SSL_BACKEND
  برائے(i = 0; دستیاب_backends[i]; i++) {
    اگر(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     دستیاب_backends[i]->info.name)) {
      Curl_ssl = دستیاب_backends[i];
      curlx_free(env);
      واپس 0;
    }
  }
#اگر_ختم

  /* Fall back to first available backend */
  Curl_ssl = دستیاب_backends[0];
  curlx_free(env);
  واپس 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت حرف *name,
                                   ثابت curl_ssl_backend ***avail)
{
  صحیح i;

  اگر(avail)
    *avail = (ثابت curl_ssl_backend **)&دستیاب_backends;

  اگر(Curl_ssl != &Curl_ssl_multi)
    واپس id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#اگر_تعریف_شدہ CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#ورنہ
           CURLSSLSET_UNKNOWN_BACKEND;
#اگر_ختم

  برائے(i = 0; دستیاب_backends[i]; i++) {
    اگر(دستیاب_backends[i]->info.id == id ||
       (name && curl_strequal(دستیاب_backends[i]->info.name, name))) {
      multissl_تیاری(دستیاب_backends[i]);
      واپس CURLSSLSET_OK;
    }
  }

  واپس CURLSSLSET_UNKNOWN_BACKEND;
}

#ورنہ /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت حرف *name,
                                   ثابت curl_ssl_backend ***avail)
{
  (خالی)id;
  (خالی)name;
  (خالی)avail;
  واپس CURLSSLSET_NO_BACKENDS;
}

#اگر_ختم /* !USE_SSL */

#اگر_تعریف_شدہ USE_SSL

خالی Curl_ssl_peer_cleanup(ساخت ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_کلید);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

ساکن ssl_peer_type get_peer_type(ثابت حرف *hostname)
{
  اگر(hostname && hostname[0]) {
#اگر_تعریف_شدہ USE_IPV6
    ساخت in6_addr addr;
#ورنہ
    ساخت in_addr addr;
#اگر_ختم
    اگر(curlx_inet_pton(AF_INET, hostname, &addr))
      واپس CURL_SSL_PEER_IPV4;
#اگر_تعریف_شدہ USE_IPV6
    ورنہ اگر(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      واپس CURL_SSL_PEER_IPV6;
    }
#اگر_ختم
  }
  واپس CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(ساخت ssl_peer *ssl_peer,
                            ساخت Curl_peer *origin,
                            ساخت Curl_peer *peer,
                            ساخت ssl_filter_config *sslc,
                            ثابت حرف *tls_id,
                            uint8_t transport)
{
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  /* We expect a clean struct, e.g. called only ONCE */
  اگر(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    واپس CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  اگر(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = لڑی_لمبائی(origin->hostname);
    اگر(len && (origin->hostname[len - 1] == '.'))
      len--;
    اگر(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      اگر(!ssl_peer->sni)
        جاؤ برآمد;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  نتیجہ = Curl_ssl_peer_کلید_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_کلید);

برآمد:
  اگر(نتیجہ)
    Curl_ssl_peer_cleanup(ssl_peer);
  واپس نتیجہ;
}

ساکن خالی ssl_cf_destroy(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  اگر(connssl) {
    connssl->ssl_impl->close(cf, ڈیٹا);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = تہی;
  }
  cf->connected = FALSE;
}

ساکن CURLcode ssl_cf_connect(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ;

  اگر(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }

  اگر(!cf->اگلا || !connssl->peer.origin) {
    *مکمل = FALSE;
    واپس CURLE_FAILED_INIT;
  }

  اگر(!cf->اگلا->connected) {
    نتیجہ = cf->اگلا->cft->do_connect(cf->اگلا, ڈیٹا, مکمل);
    اگر(نتیجہ || !*مکمل)
      واپس نتیجہ;
  }

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  CURL_TRC_CF(ڈیٹا, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *مکمل = FALSE;

  اگر(!connssl->prefs_checked) {
    اگر(!ssl_prefs_check(cf, ڈیٹا)) {
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ برآمد;
    }
    connssl->prefs_checked = TRUE;
  }

  نتیجہ = connssl->ssl_impl->do_connect(cf, ڈیٹا, مکمل);

  اگر(!نتیجہ && *مکمل) {
    cf->connected = TRUE;
    اگر(connssl->state == ssl_connection_complete) {
      connssl->handshake_مکمل = *Curl_pgrs_now(ڈیٹا);
    }
    اگر(Curl_tls_keylog_enabled()) {
      infof(ڈیٹا, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#اگر_تعریف_شدہ LIBRESSL_نسخہ_نمبر
      infof(ڈیٹا, "Note LibreSSL only supports SSLKEYLOGFILE for TLS <= 1.2");
#اگر_ختم
    }
    /* Connection can be deferred when sending early data */
    DEBUGASSERT(connssl->state == ssl_connection_complete ||
                connssl->state == ssl_connection_deferred);
    DEBUGASSERT(connssl->state != ssl_connection_deferred ||
                connssl->earlydata_state > ssl_earlydata_none);
  }
برآمد:
  CURL_TRC_CF(ڈیٹا, cf, "cf_connect() -> %d, done=%d", (صحیح)نتیجہ, *مکمل);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode ssl_cf_سیٹ_earlydata(ساخت Curl_cfilter *cf,
                                     ساخت Curl_easy *ڈیٹا,
                                     ثابت خالی *buf, size_t blen)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_خالی_خانے(&connssl->earlydata));
  اگر(blen) {
    اگر(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    نتیجہ = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(ڈیٹا, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssl_cf_connect_deferred(ساخت Curl_cfilter *cf,
                                        ساخت Curl_easy *ڈیٹا,
                                        ثابت خالی *buf, size_t blen,
                                        bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *مکمل = FALSE;
  اگر(connssl->earlydata_state == ssl_earlydata_await) {
    نتیجہ = ssl_cf_سیٹ_earlydata(cf, ڈیٹا, buf, blen);
    اگر(نتیجہ)
      واپس نتیجہ;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  نتیجہ = ssl_cf_connect(cf, ڈیٹا, مکمل);

  اگر(!نتیجہ && *مکمل) {
    اگر(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrs_وقت_Was(ڈیٹا, TIMER_APPCONNECT, connssl->handshake_مکمل);
      connssl->stats_reported = TRUE;
    }
    انتخاب(connssl->earlydata_state) {
    حالت ssl_earlydata_none:
      توڑو;
    حالت ssl_earlydata_accepted:
      اگر(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_ڈیٹا(ڈیٹا, (curl_off_t)connssl->earlydata_skip);
      infof(ڈیٹا, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      توڑو;
    حالت ssl_earlydata_rejected:
      اگر(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_ڈیٹا(ڈیٹا, -(curl_off_t)connssl->earlydata_skip);
      infof(ڈیٹا, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      توڑو;
    طےشدہ:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(تہی);
      توڑو;
    }
  }
  واپس نتیجہ;
}

ساکن bool ssl_cf_ڈیٹا_pending(ساخت Curl_cfilter *cf,
                                ثابت ساخت Curl_easy *ڈیٹا)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  bool pending;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  اگر(connssl->ssl_impl->ڈیٹا_pending &&
     connssl->ssl_impl->ڈیٹا_pending(cf, ڈیٹا))
    pending = TRUE;
  ورنہ
    pending = cf->اگلا->cft->has_ڈیٹا_pending(cf->اگلا, ڈیٹا);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس pending;
}

ساکن CURLcode ssl_cf_send(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            ثابت uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ = CURLE_OK;

  (خالی)eos;
  *pnwritten = 0;
  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);

  اگر(connssl->state == ssl_connection_deferred) {
    bool مکمل = FALSE;
    نتیجہ = ssl_cf_connect_deferred(cf, ڈیٹا, buf, blen, &مکمل);
    اگر(نتیجہ)
      جاؤ برآمد;
    ورنہ اگر(!مکمل) {
      نتیجہ = CURLE_AGAIN;
      جاؤ برآمد;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  اگر(connssl->earlydata_skip) {
    اگر(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      نتیجہ = CURLE_OK;
      *pnwritten = blen;
      جاؤ برآمد;
    }
    ورنہ {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  اگر(blen > 0) {
    size_t nwritten;
    نتیجہ = connssl->ssl_impl->send_plain(cf, ڈیٹا, buf, blen, &nwritten);
    اگر(!نتیجہ)
      *pnwritten += nwritten;
  }

برآمد:
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode ssl_cf_recv(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا, حرف *buf, size_t len,
                            size_t *pnread)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ = CURLE_OK;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  *pnread = 0;
  اگر(connssl->state == ssl_connection_deferred) {
    bool مکمل = FALSE;
    نتیجہ = ssl_cf_connect_deferred(cf, ڈیٹا, تہی, 0, &مکمل);
    اگر(نتیجہ)
      جاؤ برآمد;
    ورنہ اگر(!مکمل) {
      نتیجہ = CURLE_AGAIN;
      جاؤ برآمد;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  نتیجہ = connssl->ssl_impl->recv_plain(cf, ڈیٹا, buf, len, pnread);

برآمد:
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode ssl_cf_shutdown(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  *مکمل = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  اگر(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_نیچے_کو) {
    ساخت cf_call_ڈیٹا save;

    CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
    نتیجہ = connssl->ssl_impl->shut_نیچے_کو(cf, ڈیٹا, TRUE, مکمل);
    CURL_TRC_CF(ڈیٹا, cf, "cf_shutdown -> %d, done=%d", (صحیح)نتیجہ, *مکمل);
    CF_ڈیٹا_RESTORE(cf, save);
    cf->shutdown = (نتیجہ || *مکمل);
  }
  واپس نتیجہ;
}

ساکن CURLcode ssl_cf_adjust_pollset(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت easy_pollset *ps)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ;

  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
  نتیجہ = connssl->ssl_impl->adjust_pollset(cf, ڈیٹا, ps);
  CF_ڈیٹا_RESTORE(cf, save);
  واپس نتیجہ;
}

ساکن CURLcode ssl_cf_query(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             صحیح query, صحیح *pres1, خالی *pres2)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_SSL_INFO:
  حالت CF_QUERY_SSL_CTX_INFO:
    اگر(!Curl_ssl_cf_is_proxy(cf)) {
      ساخت curl_tlssessioninfo *info = pres2;
      ساخت cf_call_ڈیٹا save;
      CF_ڈیٹا_SAVE(save, cf, ڈیٹا);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_ڈیٹا_RESTORE(cf, save);
      واپس CURLE_OK;
    }
    توڑو;
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(ڈیٹا, cf, "query ALPN: returning '%s'", *palpn);
    واپس CURLE_OK;
  }
  طےشدہ:
    توڑو;
  }
  واپس cf->اگلا ?
    cf->اگلا->cft->query(cf->اگلا, ڈیٹا, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساکن CURLcode ssl_cf_cntrl(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             صحیح event, صحیح arg1, خالی *arg2)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;

  (خالی)arg1;
  (خالی)arg2;
  (خالی)ڈیٹا;
  انتخاب(event) {
  حالت CF_CTRL_CONN_INFO_UPDATE:
    اگر(connssl->negotiated.alpn && !cf->sockindex) {
      اگر(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_نظر_آیا = 11;
      ورنہ اگر(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_نظر_آیا = 20;
      ورنہ اگر(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_نظر_آیا = 30;
    }
    توڑو;
  حالت CF_CTRL_REPORT_STATS:
    اگر(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_مکمل.tv_sec || connssl->handshake_مکمل.tv_usec)) {
      Curl_pgrs_وقت_Was(ڈیٹا, TIMER_APPCONNECT, connssl->handshake_مکمل);
      connssl->stats_reported = TRUE;
    }
    توڑو;
  }
  واپس CURLE_OK;
}

ساکن bool cf_ssl_is_alive(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا,
                            bool *داخلہ_pending)
{
  /*
   * This function tries to determine connection status.
   */
  واپس cf->اگلا ?
    cf->اگلا->cft->is_alive(cf->اگلا, ڈیٹا, داخلہ_pending) :
    FALSE; /* pessimistic in absence of data */
}

ساخت Curl_cftype Curl_cft_ssl = {
  "SSL",
  CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_ڈیٹا_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY

ساخت Curl_cftype Curl_cft_ssl_proxy = {
  "SSL-PROXY",
  CF_TYPE_SSL | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_ڈیٹا_pending,
  ssl_cf_send,
  ssl_cf_recv,
  Curl_cf_def_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#اگر_ختم /* !CURL_DISABLE_PROXY */

ساکن CURLcode cf_ssl_create(ساخت Curl_cfilter **pcf,
                              ساخت Curl_easy *ڈیٹا,
                              ساخت connectdata *conn)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت ssl_connect_ڈیٹا *ctx;
  CURLcode نتیجہ;

  DEBUGASSERT(ڈیٹا->conn);

#اگر_تعریف_شدہ CURL_DISABLE_HTTP
  (خالی)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->بٹ.tls_enable_alpn);
  ctx = cf_ctx_new(ڈیٹا, تہی);
#ورنہ
  ctx = cf_ctx_new(ڈیٹا, alpn_get_spec(ڈیٹا->state.http_neg.wanted,
                                       ڈیٹا->state.http_neg.preferred,
                                       (bool)ڈیٹا->state.http_neg.only_10,
                                       (bool)conn->بٹ.tls_enable_alpn));
#اگر_ختم
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  نتیجہ = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

برآمد:
  اگر(نتیجہ)
    cf_ctx_free(ctx);
  *pcf = نتیجہ ? تہی : cf;
  واپس نتیجہ;
}

ساکن CURLcode cf_ssl_peer_init(ساخت Curl_cfilter *cf,
                                 ساخت Curl_peer *origin,
                                 ساخت Curl_peer *peer,
                                 ساخت ssl_filter_config *sslc)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  حرف tls_id[80];
  connssl->ssl_impl->نسخہ_2(tls_id, حجم(tls_id) - 1);
  واپس Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_جوڑو(ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_peer *origin,
                              ساخت connectdata *conn,
                              int8_t sockindex)
{
  ساخت Curl_cfilter *cf;
  ساخت Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->بذریعہ_peer2 : conn->بذریعہ_peer;
  CURLcode نتیجہ;

  نتیجہ = cf_ssl_create(&cf, ڈیٹا, conn);
  اگر(!نتیجہ)
    نتیجہ = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  اگر(!نتیجہ)
    Curl_conn_cf_جوڑو(ڈیٹا, conn, sockindex, cf);
  ورنہ اگر(cf)
    Curl_conn_cf_discard_chain(&cf, ڈیٹا);
  واپس نتیجہ;
}

CURLcode Curl_cf_ssl_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت Curl_peer *origin,
                                  ساخت Curl_peer *peer)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;

  نتیجہ = cf_ssl_create(&cf, ڈیٹا, cf_at->conn);
  اگر(!نتیجہ)
    نتیجہ = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  اگر(!نتیجہ) {
    Curl_conn_cf_داخل_کرو_after(cf_at, cf);
#اگر defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    اگر((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      نتیجہ = Curl_conn_dns_جوڑو_https_resolve(ڈیٹا, cf->conn, cf->sockindex,
                                               origin);
    }
#اگر_ختم /* USE_HTTPSRR && USE_ECH */
  }
  ورنہ اگر(cf)
    Curl_conn_cf_discard_chain(&cf, ڈیٹا);
  واپس نتیجہ;
}

#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY

ساکن CURLcode cf_ssl_proxy_create(ساخت Curl_cfilter **pcf,
                                    ساخت Curl_easy *ڈیٹا,
                                    ساخت connectdata *conn)
{
  ساخت Curl_cfilter *cf = تہی;
  ساخت ssl_connect_ڈیٹا *ctx;
  CURLcode نتیجہ;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)ڈیٹا->سیٹ.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (خالی)conn;
#اگر_تعریف_شدہ USE_HTTP2
  اگر(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#اگر_ختم

  ctx = cf_ctx_new(ڈیٹا, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  اگر(!ctx) {
    نتیجہ = CURLE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }
  نتیجہ = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

برآمد:
  اگر(نتیجہ)
    cf_ctx_free(ctx);
  *pcf = نتیجہ ? تہی : cf;
  واپس نتیجہ;
}

CURLcode Curl_cf_ssl_proxy_داخل_کرو_after(ساخت Curl_cfilter *cf_at,
                                        ساخت Curl_easy *ڈیٹا,
                                        ساخت Curl_peer *peer)
{
  ساخت Curl_cfilter *cf;
  CURLcode نتیجہ;

  نتیجہ = cf_ssl_proxy_create(&cf, ڈیٹا, cf_at->conn);
  اگر(!نتیجہ)
    نتیجہ = cf_ssl_peer_init(cf, peer, تہی, &cf_at->conn->proxy_ssl_config);
  اگر(!نتیجہ)
    Curl_conn_cf_داخل_کرو_after(cf_at, cf);
  ورنہ اگر(cf)
    Curl_conn_cf_discard_chain(&cf, ڈیٹا);
  واپس نتیجہ;
}

#اگر_ختم /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(ساخت Curl_easy *ڈیٹا, غیرعلامتی صحیح ssl_option)
{
  (خالی)ڈیٹا;
  واپس (Curl_ssl->supports & ssl_option);
}

#اگر_تعریف_شدہ USE_ECH
bool Curl_ssl_ech_enabled(ساخت Curl_cfilter *cf)
{
  اگر(Curl_ssl->supports & SSLSUPP_ECH) {
    ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    واپس !!conn_config->ech;
  }
  واپس FALSE;
}

bool Curl_ssl_need_httpsrr(ساخت Curl_cfilter *cf)
{
  اگر(Curl_ssl->supports & SSLSUPP_ECH) {
    ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    اگر(!conn_config->ech)
      واپس FALSE;
    اگر((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      واپس FALSE;
    واپس TRUE;
  }
  واپس FALSE;
}
#اگر_ختم

ساکن CURLcode vtls_shutdown_blocking(ساخت Curl_cfilter *cf,
                                       ساخت Curl_easy *ڈیٹا,
                                       bool send_shutdown, bool *مکمل)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  ساخت cf_call_ڈیٹا save;
  CURLcode نتیجہ = CURLE_OK;
  timediff_t timeout_ms;
  صحیح what, loop = 10;

  اگر(cf->shutdown) {
    *مکمل = TRUE;
    واپس CURLE_OK;
  }
  CF_ڈیٹا_SAVE(save, cf, ڈیٹا);

  *مکمل = FALSE;
  جب(!نتیجہ && !*مکمل && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(ڈیٹا, cf->conn, cf->sockindex);

    اگر(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(ڈیٹا, "SSL shutdown timeout");
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      جاؤ برآمد;
    }

    نتیجہ = connssl->ssl_impl->shut_نیچے_کو(cf, ڈیٹا, send_shutdown, مکمل);
    اگر(نتیجہ || *مکمل)
      جاؤ برآمد;

    اگر(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, ڈیٹا, timeout_ms);
      اگر(what < 0) {
        /* fatal error */
        failf(ڈیٹا, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        نتیجہ = CURLE_RECV_ERROR;
        جاؤ برآمد;
      }
      ورنہ اگر(what == 0) {
        /* timeout */
        failf(ڈیٹا, "SSL shutdown timeout");
        نتیجہ = CURLE_OPERATION_TIMEDOUT;
        جاؤ برآمد;
      }
      /* socket is readable or writable */
    }
  }
برآمد:
  CF_ڈیٹا_RESTORE(cf, save);
  cf->shutdown = (نتیجہ || *مکمل);
  واپس نتیجہ;
}

CURLcode Curl_ssl_cfilter_remove(ساخت Curl_easy *ڈیٹا,
                                 int8_t sockindex, bool send_shutdown)
{
  ساخت Curl_cfilter *cf, *سر;
  CURLcode نتیجہ = CURLE_OK;

  سر = ڈیٹا->conn ? ڈیٹا->conn->cfilter[sockindex] : تہی;
  برائے(cf = سر; cf; cf = cf->اگلا) {
    اگر(cf->cft == &Curl_cft_ssl) {
      bool مکمل;
      CURL_TRC_CF(ڈیٹا, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_آغاز_timer(ڈیٹا, sockindex, 0);
      نتیجہ = vtls_shutdown_blocking(cf, ڈیٹا, send_shutdown, &مکمل);
      Curl_cshutdn_clear_timer(ڈیٹا, sockindex);
      اگر(!نتیجہ && !مکمل) /* blocking failed? */
        نتیجہ = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, ڈیٹا);
      CURL_TRC_CF(ڈیٹا, cf, "shutdown and remove SSL, done -> %d",
                  (صحیح)نتیجہ);
      توڑو;
    }
  }
  واپس نتیجہ;
}

bool Curl_ssl_cf_is_proxy(ساخت Curl_cfilter *cf)
{
  واپس (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

ساخت ssl_easy_config *
Curl_ssl_cf_get_easy_config(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا)
{
#اگر_تعریف_شدہ CURL_DISABLE_PROXY
  (خالی)cf;
  واپس &ڈیٹا->سیٹ.ssl;
#ورنہ
  واپس Curl_ssl_cf_is_proxy(cf) ? &ڈیٹا->سیٹ.proxy_ssl : &ڈیٹا->سیٹ.ssl;
#اگر_ختم
}

ساخت ssl_filter_config *
Curl_ssl_cf_get_filter_config(ساخت Curl_cfilter *cf)
{
#اگر_تعریف_شدہ CURL_DISABLE_PROXY
  واپس &cf->conn->ssl_config;
#ورنہ
  واپس Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#اگر_ختم
}

CURLcode Curl_alpn_مقصد_proto_buf(ساخت alpn_proto_buf *buf,
                                ثابت ساخت alpn_spec *spec)
{
  size_t i, len;
  صحیح off = 0;
  غیرعلامتی حرف blen;

  memset(buf, 0, حجم(*buf));
  برائے(i = 0; spec && i < spec->گنتی; ++i) {
    len = لڑی_لمبائی(spec->entries[i]);
    اگر(len >= ALPN_NAME_MAX)
      واپس CURLE_FAILED_INIT;
    blen = (غیرعلامتی حرف)len;
    اگر(off + blen + 1 >= (صحیح)حجم(buf->ڈیٹا))
      واپس CURLE_FAILED_INIT;
    buf->ڈیٹا[off++] = blen;
    حافظہ_نقل(buf->ڈیٹا + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  واپس CURLE_OK;
}

CURLcode Curl_alpn_مقصد_proto_str(ساخت alpn_proto_buf *buf,
                                ثابت ساخت alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, حجم(*buf));
  برائے(i = 0; spec && i < spec->گنتی; ++i) {
    len = لڑی_لمبائی(spec->entries[i]);
    اگر(len >= ALPN_NAME_MAX)
      واپس CURLE_FAILED_INIT;
    اگر(off + len + 2 >= حجم(buf->ڈیٹا))
      واپس CURLE_FAILED_INIT;
    اگر(off)
      buf->ڈیٹا[off++] = ',';
    حافظہ_نقل(buf->ڈیٹا + off, spec->entries[i], len);
    off += len;
  }
  buf->ڈیٹا[off] = '\0';
  buf->len = (صحیح)off;
  واپس CURLE_OK;
}

bool Curl_alpn_شامل_ہے_proto(ثابت ساخت alpn_spec *spec,
                              ثابت حرف *proto)
{
  size_t i, plen = proto ? لڑی_لمبائی(proto) : 0;
  برائے(i = 0; spec && plen && i < spec->گنتی; ++i) {
    size_t slen = لڑی_لمبائی(spec->entries[i]);
    اگر((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      واپس TRUE;
  }
  واپس FALSE;
}

خالی Curl_alpn_restrict_مقصد(ساخت alpn_spec *spec, ثابت حرف *proto)
{
  size_t plen = لڑی_لمبائی(proto);
  DEBUGASSERT(plen < حجم(spec->entries[0]));
  اگر(plen < حجم(spec->entries[0])) {
    حافظہ_نقل(spec->entries[0], proto, plen + 1);
    spec->گنتی = 1;
  }
}

خالی Curl_alpn_copy(ساخت alpn_spec *dest, ثابت ساخت alpn_spec *src)
{
  اگر(src)
    حافظہ_نقل(dest, src, حجم(*dest));
  ورنہ
    memset(dest, 0, حجم(*dest));
}

CURLcode Curl_alpn_سیٹ_negotiated(ساخت Curl_cfilter *cf,
                                  ساخت Curl_easy *ڈیٹا,
                                  ساخت ssl_connect_ڈیٹا *connssl,
                                  ثابت غیرعلامتی حرف *proto,
                                  size_t proto_len)
{
  CURLcode نتیجہ = CURLE_OK;
  (خالی)cf;

  اگر(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    اگر(!proto_len) {
      failf(ڈیٹا, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ برآمد;
    }
    ورنہ اگر(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ برآمد;
    }
    ورنہ اگر((لڑی_لمبائی(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(ڈیٹا, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (صحیح)proto_len, proto);
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ برآمد;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(ڈیٹا, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    جاؤ برآمد;
  }

  اگر(proto && proto_len) {
    اگر(memchr(proto, '\0', proto_len)) {
      failf(ڈیٹا, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      نتیجہ = CURLE_SSL_CONNECT_ERROR;
      جاؤ برآمد;
    }
    connssl->negotiated.alpn = curlx_memdup0((ثابت حرف *)proto, proto_len);
    اگر(!connssl->negotiated.alpn)
      واپس CURLE_برآمد_کا_حافظہ;
  }

  اگر(proto && proto_len) {
    اگر(connssl->state == ssl_connection_deferred)
      infof(ڈیٹا, VTLS_INFOF_ALPN_DEFERRED, (صحیح)proto_len, proto);
    ورنہ
      infof(ڈیٹا, VTLS_INFOF_ALPN_ACCEPTED, (صحیح)proto_len, proto);
  }
  ورنہ {
    اگر(connssl->state == ssl_connection_deferred)
      infof(ڈیٹا, VTLS_INFOF_NO_ALPN_DEFERRED);
    ورنہ
      infof(ڈیٹا, VTLS_INFOF_NO_ALPN);
  }

برآمد:
  واپس نتیجہ;
}

CURLcode Curl_on_session_reuse(ساخت Curl_cfilter *cf,
                               ساخت Curl_easy *ڈیٹا,
                               ساخت alpn_spec *alpns,
                               ساخت Curl_ssl_session *scs,
                               bool *do_early_ڈیٹا, bool early_ڈیٹا_allowed)
{
  ساخت ssl_connect_ڈیٹا *connssl = cf->ctx;
  CURLcode نتیجہ = CURLE_OK;

  *do_early_ڈیٹا = FALSE;

  اگر(!early_ڈیٹا_allowed) {
    CURL_TRC_CF(ڈیٹا, cf, "SSL session does not allow earlydata");
  }
  ورنہ اگر(!Curl_alpn_شامل_ہے_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(ڈیٹا, cf, "SSL session has different ALPN, no early data");
  }
  ورنہ {
    infof(ڈیٹا, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    نتیجہ = Curl_alpn_سیٹ_negotiated(cf, ڈیٹا, connssl,
                                      (ثابت غیرعلامتی حرف *)scs->alpn,
                                      scs->alpn ? لڑی_لمبائی(scs->alpn) : 0);
    *do_early_ڈیٹا = !نتیجہ;
  }
  واپس نتیجہ;
}

ساخت Curl_ssl_session *Curl_ssl_get_cf_session(ساخت Curl_easy *ڈیٹا,
                                                 ثابت ساخت Curl_cftype *cft,
                                                 int8_t sockindex)
{
  اگر(ڈیٹا->conn &&
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#ورنہ
     (cft == &Curl_cft_ssl)) {
#اگر_ختم
    ساخت Curl_cfilter *cf1 = ڈیٹا->conn->cfilter[sockindex];
    برائے(; cf1; cf1 = cf1->اگلا) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      اگر(cf1->cft == cft)
        توڑو;
    }
    اگر(cf1) {
      ساخت ssl_connect_ڈیٹا *connssl = cf1->ctx;
      اگر(connssl)
        واپس connssl->session;
    }
  }
  واپس تہی;
}

#اگر_ختم /* USE_SSL */
