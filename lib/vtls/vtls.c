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

#ifdef USE_APPLE_SECTRUST
#شامل <Security/Security.h>
#endif

#ifdef USE_SSL
#اگر !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ایستا ثابت ساختار alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
ایستا ثابت ساختار alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
ایستا ثابت ساختار alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
ایستا ثابت ساختار alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
ایستا ثابت ساختار alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

ایستا ثابت ساختار alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  اگر(!use_alpn)
    بازگشت پوچ;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  اگر(only_http_10 && (wanted & CURL_HTTP_V1x))
    بازگشت &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  اگر(wanted & CURL_HTTP_V2x) {
    اگر(wanted & CURL_HTTP_V1x)
      بازگشت (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    بازگشت &ALPN_SPEC_H2;
  }
#وگرنه
  (تهی)wanted;
  (تهی)preferred;
#endif
  بازگشت &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
ایستا صحیح multissl_setup(ثابت ساختار Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(تهی)
{
#ifdef USE_SSL
  multissl_setup(پوچ);
  بازگشت Curl_ssl->info.id;
#وگرنه
  بازگشت CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
ایستا bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
صحیح Curl_ssl_init(تهی)
{
  /* make sure this is only done once */
  اگر(init_ssl)
    بازگشت 1;
  init_ssl = TRUE; /* never again */

  اگر(Curl_ssl->init)
    بازگشت Curl_ssl->init();
  بازگشت 1;
}

ایستا bool ssl_prefs_check(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data)
{
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  اگر(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    بازگشت FALSE;
  }

  انتخاب(conn_config->version_max) {
  حالت CURL_SSLVERSION_MAX_NONE:
  حالت CURL_SSLVERSION_MAX_DEFAULT:
    بشکن;

  پیشفرض:
    اگر((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      بازگشت FALSE;
    }
  }

  بازگشت TRUE;
}

ایستا ساختار ssl_connect_data *cf_ctx_new(ساختار Curl_easy *data,
                                           ثابت ساختار alpn_spec *alpn)
{
  ساختار ssl_connect_data *ctx;

  (تهی)data;
  ctx = curlx_calloc(1, اندازه(*ctx));
  اگر(!ctx)
    بازگشت پوچ;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  اگر(!ctx->backend) {
    curlx_free(ctx);
    بازگشت پوچ;
  }
  بازگشت ctx;
}

ایستا تهی cf_ctx_free(ساختار ssl_connect_data *ctx)
{
  اگر(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(ساختار Curl_easy *data, int8_t sockindex,
                                      ساختار dynbuf *binding)
{
  اگر(Curl_ssl->get_channel_binding)
    بازگشت Curl_ssl->get_channel_binding(data, sockindex, binding);
  بازگشت CURLE_OK;
}

تهی Curl_ssl_close_all(ساختار Curl_easy *data)
{
  اگر(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ساختار easy_pollset *ps)
{
  ساختار ssl_connect_data *connssl = cf->ctx;

  اگر(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    اگر(sock != CURL_SOCKET_BAD) {
      اگر(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      وگرنه {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    بازگشت result;
  }
  بازگشت CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
ساختار curl_slist *Curl_ssl_engines_list(ساختار Curl_easy *data)
{
  اگر(Curl_ssl->engines_list)
    بازگشت Curl_ssl->engines_list(data);
  بازگشت پوچ;
}

ایستا size_t multissl_version(نویسه *buffer, size_t size);

تهی Curl_ssl_version(نویسه *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (تهی)multissl_version(buffer, size);
#وگرنه
  (تهی)Curl_ssl->version(buffer, size);
#endif
}

تهی Curl_ssl_free_certinfo(ساختار Curl_easy *data)
{
  ساختار curl_certinfo *ci = &data->info.certs;

  اگر(ci->رقم_of_certs) {
    /* free all individual lists used */
    صحیح i;
    برای(i = 0; i < ci->رقم_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = پوچ;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->رقم_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(ساختار Curl_easy *data, صحیح رقم)
{
  ساختار curl_certinfo *ci = &data->info.certs;
  ساختار curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)رقم, اندازه(ساختار curl_slist *));
  اگر(!table)
    بازگشت CURLE_OUT_OF_MEMORY;

  ci->رقم_of_certs = رقم;
  ci->certinfo = table;

  بازگشت CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_افزودن_certinfo_len(ساختار Curl_easy *data,
                                    صحیح certnum,
                                    ثابت نویسه *label,
                                    ثابت نویسه *مقدار,
                                    size_t valuelen)
{
  ساختار curl_certinfo *ci = &data->info.certs;
  ساختار curl_slist *nl;
  CURLcode result = CURLE_OK;
  ساختار dynbuf build;

  DEBUGASSERT(certnum < ci->رقم_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  اگر(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, مقدار, valuelen))
    بازگشت CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  اگر(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  بازگشت result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(ساختار Curl_easy *data,
                         unsigned نویسه *buffer, size_t length)
{
  DEBUGASSERT(length == اندازه(صحیح));
  اگر(Curl_ssl->random)
    بازگشت Curl_ssl->random(data, buffer, length);
  وگرنه
    بازگشت CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

ایستا CURLcode pubkey_pem_to_der(ثابت نویسه *pem,
                                  unsigned نویسه **der, size_t *der_len)
{
  ثابت نویسه *begin_pos, *end_pos;
  size_t pem_شمار, pem_len;
  CURLcode result;
  ساختار dynbuf pbuf;

  /* if no pem, exit. */
  اگر(!pem)
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  اگر(!begin_pos)
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  pem_شمار = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  اگر(pem_شمار && '\n' != pem[pem_شمار - 1])
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_شمار += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_شمار, "\n-----END PUBLIC KEY-----");
  اگر(!end_pos)
    بازگشت CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  وقتی(pem_شمار < pem_len) {
    اگر('\n' != pem[pem_شمار] && '\r' != pem[pem_شمار]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_شمار], 1);
      اگر(result)
        بازگشت result;
    }
    ++pem_شمار;
  }

  اگر(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  وگرنه
    result = CURLE_BAD_CONTENT_ENCODING;

  بازگشت result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(ساختار Curl_easy *data,
                              ثابت نویسه *pinnedpubkey,
                              ثابت unsigned نویسه *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  اگر(!pinnedpubkey)
    بازگشت CURLE_OK;
  اگر(!pubkey || !pubkeylen)
    بازگشت result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  اگر(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    نویسه *cert_hash = پوچ;
    ثابت نویسه *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned نویسه *sha256sumdigest;

    اگر(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      بازگشت result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    اگر(!sha256sumdigest)
      بازگشت CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    اگر(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    اگر(encode)
      بازگشت encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    وقتی(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      اگر(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        بشکن;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (صحیح)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : پوچ;
    }
    curlx_safefree(cert_hash);
  }
  وگرنه {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    ساختار dynbuf buf;
    unsigned نویسه *pem_ptr = پوچ;
    size_t چپ;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    اگر(!fp)
      بازگشت result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    اگر(fseek(fp, 0, SEEK_END))
      برو end;
    filesize = ftell(fp);
    اگر(fseek(fp, 0, SEEK_SET))
      برو end;
    اگر(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      برو end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    اگر(pubkeylen > size)
      برو end;

    /*
     * Read the file into the dynbuf
     */
    چپ = size;
    انجامبده {
      نویسه buffer[1024];
      size_t want = چپ > اندازه(buffer) ? اندازه(buffer) : چپ;
      اگر(fread(buffer, 1, want, fp) != want)
        برو end;
      اگر(curlx_dyn_addn(&buf, buffer, want))
        برو end;
      چپ -= want;
    } وقتی(چپ);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    اگر(pubkeylen == size) {
      اگر(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      برو end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    اگر(pem_read)
      برو end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    اگر(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  بازگشت result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(تهی)
{
  اگر(Curl_ssl->cert_status_request)
    بازگشت Curl_ssl->cert_status_request();
  بازگشت FALSE;
}

ایستا صحیح multissl_init(تهی)
{
  اگر(multissl_setup(پوچ))
    بازگشت 1;
  اگر(Curl_ssl->init)
    بازگشت Curl_ssl->init();
  بازگشت 1;
}

ایستا CURLcode multissl_random(ساختار Curl_easy *data,
                                unsigned نویسه *entropy, size_t length)
{
  اگر(multissl_setup(پوچ))
    بازگشت CURLE_FAILED_INIT;
  بازگشت Curl_ssl->random(data, entropy, length);
}

ایستا CURLcode multissl_connect(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data, bool *done)
{
  اگر(multissl_setup(پوچ))
    بازگشت CURLE_FAILED_INIT;
  بازگشت Curl_ssl->do_connect(cf, data, done);
}

ایستا CURLcode multissl_adjust_pollset(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        ساختار easy_pollset *ps)
{
  اگر(multissl_setup(پوچ))
    بازگشت CURLE_OK;
  بازگشت Curl_ssl->adjust_pollset(cf, data, ps);
}

ایستا تهی *multissl_get_internals(ساختار ssl_connect_data *connssl,
                                    CURLINFO info)
{
  اگر(multissl_setup(پوچ))
    بازگشت پوچ;
  بازگشت Curl_ssl->get_internals(connssl, info);
}

ایستا تهی multissl_close(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  اگر(multissl_setup(پوچ))
    بازگشت;
  Curl_ssl->close(cf, data);
}

ایستا CURLcode multissl_recv_plain(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    نویسه *buf, size_t len, size_t *pnread)
{
  اگر(multissl_setup(پوچ))
    بازگشت CURLE_FAILED_INIT;
  بازگشت Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

ایستا CURLcode multissl_send_plain(ساختار Curl_cfilter *cf,
                                    ساختار Curl_easy *data,
                                    ثابت تهی *mem, size_t len,
                                    size_t *pnwritten)
{
  اگر(multissl_setup(پوچ))
    بازگشت CURLE_FAILED_INIT;
  بازگشت Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

ایستا ثابت ساختار Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  پوچ,                              /* cleanup */
  multissl_version,                  /* version */
  پوچ,                              /* shutdown */
  پوچ,                              /* data_pending */
  multissl_random,                   /* random */
  پوچ,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  پوچ,                              /* close_all */
  پوچ,                              /* engines_list */
  پوچ,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  پوچ,                              /* get_channel_binding */
};

ثابت ساختار Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#وگرنهاگر defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#وگرنهاگر defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#وگرنهاگر defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#وگرنهاگر defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#وگرنهاگر defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#وگرنهاگر defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#وگرنه
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

ایستا ثابت ساختار Curl_ssl *available_backends[] = {
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
  پوچ
};

/* Global cleanup */
تهی Curl_ssl_cleanup(تهی)
{
  اگر(init_ssl) {
    /* only cleanup if we did a previous init */
    اگر(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

ایستا size_t multissl_version(نویسه *buffer, size_t size)
{
  ایستا ثابت ساختار Curl_ssl *selected;
  ایستا نویسه backends[200];
  ایستا size_t backends_len;
  ثابت ساختار Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  اگر(current != selected) {
    نویسه *p = backends;
    ثابت نویسه *end = backends + اندازه(backends);
    صحیح i;

    selected = current;

    backends[0] = '\0';

    برای(i = 0; available_backends[i]; ++i) {
      نویسه vb[200];
      bool paren = (selected != available_backends[i]);

      اگر(available_backends[i]->version(vb, اندازه(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  اگر(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  بازگشت 0;
}

ایستا صحیح multissl_setup(ثابت ساختار Curl_ssl *backend)
{
  صحیح i;
  نویسه *env;

  اگر(Curl_ssl != &Curl_ssl_multi)
    بازگشت 1;

  اگر(backend) {
    Curl_ssl = backend;
    بازگشت 0;
  }

  اگر(!available_backends[0])
    بازگشت 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  اگر(env) {
    برای(i = 0; available_backends[i]; i++) {
      اگر(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        بازگشت 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  برای(i = 0; available_backends[i]; i++) {
    اگر(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      بازگشت 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  بازگشت 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت نویسه *name,
                                   ثابت curl_ssl_backend ***avail)
{
  صحیح i;

  اگر(avail)
    *avail = (ثابت curl_ssl_backend **)&available_backends;

  اگر(Curl_ssl != &Curl_ssl_multi)
    بازگشت id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#وگرنه
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  برای(i = 0; available_backends[i]; i++) {
    اگر(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      بازگشت CURLSSLSET_OK;
    }
  }

  بازگشت CURLSSLSET_UNKNOWN_BACKEND;
}

#وگرنه /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت نویسه *name,
                                   ثابت curl_ssl_backend ***avail)
{
  (تهی)id;
  (تهی)name;
  (تهی)avail;
  بازگشت CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

تهی Curl_ssl_peer_cleanup(ساختار ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_کلید);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

ایستا ssl_peer_type get_peer_type(ثابت نویسه *hostname)
{
  اگر(hostname && hostname[0]) {
#ifdef USE_IPV6
    ساختار in6_addr addr;
#وگرنه
    ساختار in_addr addr;
#endif
    اگر(curlx_inet_pton(AF_INET, hostname, &addr))
      بازگشت CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    وگرنه اگر(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      بازگشت CURL_SSL_PEER_IPV6;
    }
#endif
  }
  بازگشت CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(ساختار ssl_peer *ssl_peer,
                            ساختار Curl_peer *origin,
                            ساختار Curl_peer *peer,
                            ساختار ssl_filter_config *sslc,
                            ثابت نویسه *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  اگر(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    بازگشت CURLE_FAILED_INIT;
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
    size_t len = strlen(origin->hostname);
    اگر(len && (origin->hostname[len - 1] == '.'))
      len--;
    اگر(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      اگر(!ssl_peer->sni)
        برو out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_کلید_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_کلید);

out:
  اگر(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  بازگشت result;
}

ایستا تهی ssl_cf_destroy(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  اگر(connssl) {
    connssl->ssl_impl->close(cf, data);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = پوچ;
  }
  cf->connected = FALSE;
}

ایستا CURLcode ssl_cf_connect(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               bool *done)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result;

  اگر(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }

  اگر(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    بازگشت CURLE_FAILED_INIT;
  }

  اگر(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    اگر(result || !*done)
      بازگشت result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  اگر(!connssl->prefs_checked) {
    اگر(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      برو out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  اگر(!result && *done) {
    cf->connected = TRUE;
    اگر(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    اگر(Curl_tls_keylog_enabled()) {
      infof(data, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_عدد
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (صحیح)result, *done);
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode ssl_cf_set_earlydata(ساختار Curl_cfilter *cf,
                                     ساختار Curl_easy *data,
                                     ثابت تهی *buf, size_t blen)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  اگر(blen) {
    اگر(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    اگر(result)
      بازگشت result;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssl_cf_connect_deferred(ساختار Curl_cfilter *cf,
                                        ساختار Curl_easy *data,
                                        ثابت تهی *buf, size_t blen,
                                        bool *done)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  اگر(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    اگر(result)
      بازگشت result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  اگر(!result && *done) {
    اگر(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    انتخاب(connssl->earlydata_state) {
    حالت ssl_earlydata_none:
      بشکن;
    حالت ssl_earlydata_accepted:
      اگر(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      بشکن;
    حالت ssl_earlydata_rejected:
      اگر(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      بشکن;
    پیشفرض:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(پوچ);
      بشکن;
    }
  }
  بازگشت result;
}

ایستا bool ssl_cf_data_pending(ساختار Curl_cfilter *cf,
                                ثابت ساختار Curl_easy *data)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  اگر(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  وگرنه
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  بازگشت pending;
}

ایستا CURLcode ssl_cf_send(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            ثابت uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result = CURLE_OK;

  (تهی)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  اگر(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    اگر(result)
      برو out;
    وگرنه اگر(!done) {
      result = CURLE_AGAIN;
      برو out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  اگر(connssl->earlydata_skip) {
    اگر(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      برو out;
    }
    وگرنه {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  اگر(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    اگر(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode ssl_cf_recv(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data, نویسه *buf, size_t len,
                            size_t *pnread)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  اگر(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, پوچ, 0, &done);
    اگر(result)
      برو out;
    وگرنه اگر(!done) {
      result = CURLE_AGAIN;
      برو out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode ssl_cf_shutdown(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                bool *done)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  اگر(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    ساختار cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (صحیح)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  بازگشت result;
}

ایستا CURLcode ssl_cf_adjust_pollset(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      ساختار easy_pollset *ps)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  بازگشت result;
}

ایستا CURLcode ssl_cf_query(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             صحیح query, صحیح *pres1, تهی *pres2)
{
  ساختار ssl_connect_data *connssl = cf->ctx;

  انتخاب(query) {
  حالت CF_QUERY_SSL_INFO:
  حالت CF_QUERY_SSL_CTX_INFO:
    اگر(!Curl_ssl_cf_is_proxy(cf)) {
      ساختار curl_tlssessioninfo *info = pres2;
      ساختار cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      بازگشت CURLE_OK;
    }
    بشکن;
  حالت CF_QUERY_ALPN_NEGOTIATED: {
    ثابت نویسه **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    بازگشت CURLE_OK;
  }
  پیشفرض:
    بشکن;
  }
  بازگشت cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ایستا CURLcode ssl_cf_cntrl(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             صحیح event, صحیح arg1, تهی *arg2)
{
  ساختار ssl_connect_data *connssl = cf->ctx;

  (تهی)arg1;
  (تهی)arg2;
  (تهی)data;
  انتخاب(event) {
  حالت CF_CTRL_CONN_INFO_UPDATE:
    اگر(connssl->negotiated.alpn && !cf->sockindex) {
      اگر(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      وگرنه اگر(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      وگرنه اگر(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    بشکن;
  حالت CF_CTRL_REPORT_STATS:
    اگر(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    بشکن;
  }
  بازگشت CURLE_OK;
}

ایستا bool cf_ssl_is_alive(ساختار Curl_cfilter *cf, ساختار Curl_easy *data,
                            bool *درون_داد_pending)
{
  /*
   * This function tries to determine connection status.
   */
  بازگشت cf->next ?
    cf->next->cft->is_alive(cf->next, data, درون_داد_pending) :
    FALSE; /* pessimistic in absence of data */
}

ساختار Curl_cftype Curl_cft_ssl = {
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

ساختار Curl_cftype Curl_cft_ssl_proxy = {
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

ایستا CURLcode cf_ssl_create(ساختار Curl_cfilter **pcf,
                              ساختار Curl_easy *data,
                              ساختار connectdata *conn)
{
  ساختار Curl_cfilter *cf = پوچ;
  ساختار ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (تهی)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, پوچ);
#وگرنه
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  اگر(result)
    cf_ctx_free(ctx);
  *pcf = result ? پوچ : cf;
  بازگشت result;
}

ایستا CURLcode cf_ssl_peer_init(ساختار Curl_cfilter *cf,
                                 ساختار Curl_peer *origin,
                                 ساختار Curl_peer *peer,
                                 ساختار ssl_filter_config *sslc)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  نویسه tls_id[80];
  connssl->ssl_impl->version(tls_id, اندازه(tls_id) - 1);
  بازگشت Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(ساختار Curl_easy *data,
                              ساختار Curl_peer *origin,
                              ساختار connectdata *conn,
                              int8_t sockindex)
{
  ساختار Curl_cfilter *cf;
  ساختار Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  اگر(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  اگر(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  وگرنه اگر(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  بازگشت result;
}

CURLcode Curl_cf_ssl_درج_after(ساختار Curl_cfilter *cf_at,
                                  ساختار Curl_easy *data,
                                  ساختار Curl_peer *origin,
                                  ساختار Curl_peer *peer)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  اگر(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  اگر(!result) {
    Curl_conn_cf_درج_after(cf_at, cf);
#اگر defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    اگر((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  وگرنه اگر(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  بازگشت result;
}

#ifndef CURL_DISABLE_PROXY

ایستا CURLcode cf_ssl_proxy_create(ساختار Curl_cfilter **pcf,
                                    ساختار Curl_easy *data,
                                    ساختار connectdata *conn)
{
  ساختار Curl_cfilter *cf = پوچ;
  ساختار ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (تهی)conn;
#ifdef USE_HTTP2
  اگر(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  اگر(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    برو out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  اگر(result)
    cf_ctx_free(ctx);
  *pcf = result ? پوچ : cf;
  بازگشت result;
}

CURLcode Curl_cf_ssl_proxy_درج_after(ساختار Curl_cfilter *cf_at,
                                        ساختار Curl_easy *data,
                                        ساختار Curl_peer *peer)
{
  ساختار Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  اگر(!result)
    result = cf_ssl_peer_init(cf, peer, پوچ, &cf_at->conn->proxy_ssl_config);
  اگر(!result)
    Curl_conn_cf_درج_after(cf_at, cf);
  وگرنه اگر(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  بازگشت result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(ساختار Curl_easy *data, unsigned صحیح ssl_option)
{
  (تهی)data;
  بازگشت (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(ساختار Curl_cfilter *cf)
{
  اگر(Curl_ssl->supports & SSLSUPP_ECH) {
    ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    بازگشت !!conn_config->ech;
  }
  بازگشت FALSE;
}

bool Curl_ssl_need_httpsrr(ساختار Curl_cfilter *cf)
{
  اگر(Curl_ssl->supports & SSLSUPP_ECH) {
    ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    اگر(!conn_config->ech)
      بازگشت FALSE;
    اگر((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      بازگشت FALSE;
    بازگشت TRUE;
  }
  بازگشت FALSE;
}
#endif

ایستا CURLcode vtls_shutdown_blocking(ساختار Curl_cfilter *cf,
                                       ساختار Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  ساختار cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  صحیح what, loop = 10;

  اگر(cf->shutdown) {
    *done = TRUE;
    بازگشت CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  وقتی(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    اگر(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      برو out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    اگر(result || *done)
      برو out;

    اگر(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      اگر(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        برو out;
      }
      وگرنه اگر(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        برو out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  بازگشت result;
}

CURLcode Curl_ssl_cfilter_remove(ساختار Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  ساختار Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : پوچ;
  برای(cf = head; cf; cf = cf->next) {
    اگر(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      اگر(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (صحیح)result);
      بشکن;
    }
  }
  بازگشت result;
}

bool Curl_ssl_cf_is_proxy(ساختار Curl_cfilter *cf)
{
  بازگشت (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

ساختار ssl_easy_config *
Curl_ssl_cf_get_easy_config(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (تهی)cf;
  بازگشت &data->set.ssl;
#وگرنه
  بازگشت Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

ساختار ssl_filter_config *
Curl_ssl_cf_get_filter_config(ساختار Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  بازگشت &cf->conn->ssl_config;
#وگرنه
  بازگشت Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(ساختار alpn_proto_buf *buf,
                                ثابت ساختار alpn_spec *spec)
{
  size_t i, len;
  صحیح off = 0;
  unsigned نویسه blen;

  memset(buf, 0, اندازه(*buf));
  برای(i = 0; spec && i < spec->شمار; ++i) {
    len = strlen(spec->entries[i]);
    اگر(len >= ALPN_NAME_MAX)
      بازگشت CURLE_FAILED_INIT;
    blen = (unsigned نویسه)len;
    اگر(off + blen + 1 >= (صحیح)اندازه(buf->data))
      بازگشت CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  بازگشت CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(ساختار alpn_proto_buf *buf,
                                ثابت ساختار alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, اندازه(*buf));
  برای(i = 0; spec && i < spec->شمار; ++i) {
    len = strlen(spec->entries[i]);
    اگر(len >= ALPN_NAME_MAX)
      بازگشت CURLE_FAILED_INIT;
    اگر(off + len + 2 >= اندازه(buf->data))
      بازگشت CURLE_FAILED_INIT;
    اگر(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (صحیح)off;
  بازگشت CURLE_OK;
}

bool Curl_alpn_contains_proto(ثابت ساختار alpn_spec *spec,
                              ثابت نویسه *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  برای(i = 0; spec && plen && i < spec->شمار; ++i) {
    size_t slen = strlen(spec->entries[i]);
    اگر((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      بازگشت TRUE;
  }
  بازگشت FALSE;
}

تهی Curl_alpn_restrict_to(ساختار alpn_spec *spec, ثابت نویسه *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < اندازه(spec->entries[0]));
  اگر(plen < اندازه(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->شمار = 1;
  }
}

تهی Curl_alpn_copy(ساختار alpn_spec *dest, ثابت ساختار alpn_spec *src)
{
  اگر(src)
    memcpy(dest, src, اندازه(*dest));
  وگرنه
    memset(dest, 0, اندازه(*dest));
}

CURLcode Curl_alpn_set_negotiated(ساختار Curl_cfilter *cf,
                                  ساختار Curl_easy *data,
                                  ساختار ssl_connect_data *connssl,
                                  ثابت unsigned نویسه *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (تهی)cf;

  اگر(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    اگر(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      برو out;
    }
    وگرنه اگر(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      برو out;
    }
    وگرنه اگر((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (صحیح)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      برو out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    برو out;
  }

  اگر(proto && proto_len) {
    اگر(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      برو out;
    }
    connssl->negotiated.alpn = curlx_memdup0((ثابت نویسه *)proto, proto_len);
    اگر(!connssl->negotiated.alpn)
      بازگشت CURLE_OUT_OF_MEMORY;
  }

  اگر(proto && proto_len) {
    اگر(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (صحیح)proto_len, proto);
    وگرنه
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (صحیح)proto_len, proto);
  }
  وگرنه {
    اگر(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    وگرنه
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  بازگشت result;
}

CURLcode Curl_on_session_reuse(ساختار Curl_cfilter *cf,
                               ساختار Curl_easy *data,
                               ساختار alpn_spec *alpns,
                               ساختار Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  ساختار ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  اگر(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  وگرنه اگر(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  وگرنه {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ثابت unsigned نویسه *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  بازگشت result;
}

ساختار Curl_ssl_session *Curl_ssl_get_cf_session(ساختار Curl_easy *data,
                                                 ثابت ساختار Curl_cftype *cft,
                                                 int8_t sockindex)
{
  اگر(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#وگرنه
     (cft == &Curl_cft_ssl)) {
#endif
    ساختار Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    برای(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      اگر(cf1->cft == cft)
        بشکن;
    }
    اگر(cf1) {
      ساختار ssl_connect_data *connssl = cf1->ctx;
      اگر(connssl)
        بازگشت connssl->session;
    }
  }
  بازگشت پوچ;
}

#endif /* USE_SSL */
