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

#ifdef USE_APPLE_SECTRUST
#ضمن <Security/Security.h>
#endif

#ifdef USE_SSL
#ܐܢ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ساكن ثابت بنية alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
ساكن ثابت بنية alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
ساكن ثابت بنية alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
ساكن ثابت بنية alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
ساكن ثابت بنية alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

ساكن ثابت بنية alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  ܐܢ(!use_alpn)
    ܦܢܐ عدم;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  ܐܢ(only_http_10 && (wanted & CURL_HTTP_V1x))
    ܦܢܐ &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  ܐܢ(wanted & CURL_HTTP_V2x) {
    ܐܢ(wanted & CURL_HTTP_V1x)
      ܦܢܐ (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    ܦܢܐ &ALPN_SPEC_H2;
  }
#ܐܚܪܢܐ
  (فراغ)wanted;
  (فراغ)preferred;
#endif
  ܦܢܐ &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
ساكن صحيح multissl_setup(ثابت بنية Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(فراغ)
{
#ifdef USE_SSL
  multissl_setup(عدم);
  ܦܢܐ Curl_ssl->info.id;
#ܐܚܪܢܐ
  ܦܢܐ CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

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
  ܐܢ(init_ssl)
    ܦܢܐ 1;
  init_ssl = TRUE; /* never again */

  ܐܢ(Curl_ssl->init)
    ܦܢܐ Curl_ssl->init();
  ܦܢܐ 1;
}

ساكن bool ssl_prefs_check(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  ܐܢ(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    ܦܢܐ FALSE;
  }

  بدل(conn_config->version_max) {
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_DEFAULT:
    اكسر;

  افتراضي:
    ܐܢ((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      ܦܢܐ FALSE;
    }
  }

  ܦܢܐ TRUE;
}

ساكن بنية ssl_connect_data *cf_ctx_new(بنية Curl_easy *data,
                                           ثابت بنية alpn_spec *alpn)
{
  بنية ssl_connect_data *ctx;

  (فراغ)data;
  ctx = curlx_calloc(1, حجم(*ctx));
  ܐܢ(!ctx)
    ܦܢܐ عدم;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  ܐܢ(!ctx->backend) {
    curlx_free(ctx);
    ܦܢܐ عدم;
  }
  ܦܢܐ ctx;
}

ساكن فراغ cf_ctx_free(بنية ssl_connect_data *ctx)
{
  ܐܢ(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(بنية Curl_easy *data, int8_t sockindex,
                                      بنية dynbuf *binding)
{
  ܐܢ(Curl_ssl->get_channel_binding)
    ܦܢܐ Curl_ssl->get_channel_binding(data, sockindex, binding);
  ܦܢܐ CURLE_OK;
}

فراغ Curl_ssl_close_all(بنية Curl_easy *data)
{
  ܐܢ(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية easy_pollset *ps)
{
  بنية ssl_connect_data *connssl = cf->ctx;

  ܐܢ(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    ܐܢ(sock != CURL_SOCKET_BAD) {
      ܐܢ(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      ܐܚܪܢܐ {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    ܦܢܐ result;
  }
  ܦܢܐ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
بنية curl_slist *Curl_ssl_engines_list(بنية Curl_easy *data)
{
  ܐܢ(Curl_ssl->engines_list)
    ܦܢܐ Curl_ssl->engines_list(data);
  ܦܢܐ عدم;
}

ساكن size_t multissl_version(حرف *buffer, size_t size);

فراغ Curl_ssl_version(حرف *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (فراغ)multissl_version(buffer, size);
#ܐܚܪܢܐ
  (فراغ)Curl_ssl->version(buffer, size);
#endif
}

فراغ Curl_ssl_free_certinfo(بنية Curl_easy *data)
{
  بنية curl_certinfo *ci = &data->info.certs;

  ܐܢ(ci->num_of_certs) {
    /* free all individual lists used */
    صحيح i;
    لكل(i = 0; i < ci->num_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = عدم;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->num_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(بنية Curl_easy *data, صحيح num)
{
  بنية curl_certinfo *ci = &data->info.certs;
  بنية curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)num, حجم(بنية curl_slist *));
  ܐܢ(!table)
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  ci->num_of_certs = num;
  ci->certinfo = table;

  ܦܢܐ CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_push_certinfo_len(بنية Curl_easy *data,
                                    صحيح certnum,
                                    ثابت حرف *label,
                                    ثابت حرف *value,
                                    size_t valuelen)
{
  بنية curl_certinfo *ci = &data->info.certs;
  بنية curl_slist *nl;
  CURLcode result = CURLE_OK;
  بنية dynbuf build;

  DEBUGASSERT(certnum < ci->num_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  ܐܢ(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, value, valuelen))
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  ܐܢ(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  ܦܢܐ result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(بنية Curl_easy *data,
                         unsigned حرف *buffer, size_t length)
{
  DEBUGASSERT(length == حجم(صحيح));
  ܐܢ(Curl_ssl->random)
    ܦܢܐ Curl_ssl->random(data, buffer, length);
  ܐܚܪܢܐ
    ܦܢܐ CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

ساكن CURLcode pubkey_pem_to_der(ثابت حرف *pem,
                                  unsigned حرف **der, size_t *der_len)
{
  ثابت حرف *begin_pos, *end_pos;
  size_t pem_count, pem_len;
  CURLcode result;
  بنية dynbuf pbuf;

  /* if no pem, exit. */
  ܐܢ(!pem)
    ܦܢܐ CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  ܐܢ(!begin_pos)
    ܦܢܐ CURLE_BAD_CONTENT_ENCODING;

  pem_count = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  ܐܢ(pem_count && '\n' != pem[pem_count - 1])
    ܦܢܐ CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_count += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_count, "\n-----END PUBLIC KEY-----");
  ܐܢ(!end_pos)
    ܦܢܐ CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  ܟܡܐ(pem_count < pem_len) {
    ܐܢ('\n' != pem[pem_count] && '\r' != pem[pem_count]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_count], 1);
      ܐܢ(result)
        ܦܢܐ result;
    }
    ++pem_count;
  }

  ܐܢ(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  ܐܚܪܢܐ
    result = CURLE_BAD_CONTENT_ENCODING;

  ܦܢܐ result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(بنية Curl_easy *data,
                              ثابت حرف *pinnedpubkey,
                              ثابت unsigned حرف *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ܐܢ(!pinnedpubkey)
    ܦܢܐ CURLE_OK;
  ܐܢ(!pubkey || !pubkeylen)
    ܦܢܐ result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  ܐܢ(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    حرف *cert_hash = عدم;
    ثابت حرف *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned حرف *sha256sumdigest;

    ܐܢ(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      ܦܢܐ result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    ܐܢ(!sha256sumdigest)
      ܦܢܐ CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    ܐܢ(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    ܐܢ(encode)
      ܦܢܐ encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    ܟܡܐ(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      ܐܢ(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        اكسر;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (صحيح)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : عدم;
    }
    curlx_safefree(cert_hash);
  }
  ܐܚܪܢܐ {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    بنية dynbuf buf;
    unsigned حرف *pem_ptr = عدم;
    size_t left;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    ܐܢ(!fp)
      ܦܢܐ result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    ܐܢ(fseek(fp, 0, SEEK_END))
      اذهب end;
    filesize = ftell(fp);
    ܐܢ(fseek(fp, 0, SEEK_SET))
      اذهب end;
    ܐܢ(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      اذهب end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    ܐܢ(pubkeylen > size)
      اذهب end;

    /*
     * Read the file into the dynbuf
     */
    left = size;
    ܥܒܕ {
      حرف buffer[1024];
      size_t want = left > حجم(buffer) ? حجم(buffer) : left;
      ܐܢ(fread(buffer, 1, want, fp) != want)
        اذهب end;
      ܐܢ(curlx_dyn_addn(&buf, buffer, want))
        اذهب end;
      left -= want;
    } ܟܡܐ(left);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    ܐܢ(pubkeylen == size) {
      ܐܢ(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      اذهب end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    ܐܢ(pem_read)
      اذهب end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    ܐܢ(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  ܦܢܐ result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(فراغ)
{
  ܐܢ(Curl_ssl->cert_status_request)
    ܦܢܐ Curl_ssl->cert_status_request();
  ܦܢܐ FALSE;
}

ساكن صحيح multissl_init(فراغ)
{
  ܐܢ(multissl_setup(عدم))
    ܦܢܐ 1;
  ܐܢ(Curl_ssl->init)
    ܦܢܐ Curl_ssl->init();
  ܦܢܐ 1;
}

ساكن CURLcode multissl_random(بنية Curl_easy *data,
                                unsigned حرف *entropy, size_t length)
{
  ܐܢ(multissl_setup(عدم))
    ܦܢܐ CURLE_FAILED_INIT;
  ܦܢܐ Curl_ssl->random(data, entropy, length);
}

ساكن CURLcode multissl_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data, bool *done)
{
  ܐܢ(multissl_setup(عدم))
    ܦܢܐ CURLE_FAILED_INIT;
  ܦܢܐ Curl_ssl->do_connect(cf, data, done);
}

ساكن CURLcode multissl_adjust_pollset(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data,
                                        بنية easy_pollset *ps)
{
  ܐܢ(multissl_setup(عدم))
    ܦܢܐ CURLE_OK;
  ܦܢܐ Curl_ssl->adjust_pollset(cf, data, ps);
}

ساكن فراغ *multissl_get_internals(بنية ssl_connect_data *connssl,
                                    CURLINFO info)
{
  ܐܢ(multissl_setup(عدم))
    ܦܢܐ عدم;
  ܦܢܐ Curl_ssl->get_internals(connssl, info);
}

ساكن فراغ multissl_close(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  ܐܢ(multissl_setup(عدم))
    ܦܢܐ;
  Curl_ssl->close(cf, data);
}

ساكن CURLcode multissl_recv_plain(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    حرف *buf, size_t len, size_t *pnread)
{
  ܐܢ(multissl_setup(عدم))
    ܦܢܐ CURLE_FAILED_INIT;
  ܦܢܐ Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

ساكن CURLcode multissl_send_plain(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    ثابت فراغ *mem, size_t len,
                                    size_t *pnwritten)
{
  ܐܢ(multissl_setup(عدم))
    ܦܢܐ CURLE_FAILED_INIT;
  ܦܢܐ Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

ساكن ثابت بنية Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  عدم,                              /* cleanup */
  multissl_version,                  /* version */
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
#ifdef CURL_WITH_MULTI_SSL
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
#ܐܚܪܢܐ
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

ساكن ثابت بنية Curl_ssl *available_backends[] = {
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
  عدم
};

/* Global cleanup */
فراغ Curl_ssl_cleanup(فراغ)
{
  ܐܢ(init_ssl) {
    /* only cleanup if we did a previous init */
    ܐܢ(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

ساكن size_t multissl_version(حرف *buffer, size_t size)
{
  ساكن ثابت بنية Curl_ssl *selected;
  ساكن حرف backends[200];
  ساكن size_t backends_len;
  ثابت بنية Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  ܐܢ(current != selected) {
    حرف *p = backends;
    ثابت حرف *end = backends + حجم(backends);
    صحيح i;

    selected = current;

    backends[0] = '\0';

    لكل(i = 0; available_backends[i]; ++i) {
      حرف vb[200];
      bool paren = (selected != available_backends[i]);

      ܐܢ(available_backends[i]->version(vb, حجم(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  ܐܢ(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  ܦܢܐ 0;
}

ساكن صحيح multissl_setup(ثابت بنية Curl_ssl *backend)
{
  صحيح i;
  حرف *env;

  ܐܢ(Curl_ssl != &Curl_ssl_multi)
    ܦܢܐ 1;

  ܐܢ(backend) {
    Curl_ssl = backend;
    ܦܢܐ 0;
  }

  ܐܢ(!available_backends[0])
    ܦܢܐ 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  ܐܢ(env) {
    لكل(i = 0; available_backends[i]; i++) {
      ܐܢ(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        ܦܢܐ 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  لكل(i = 0; available_backends[i]; i++) {
    ܐܢ(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      ܦܢܐ 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  ܦܢܐ 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت حرف *name,
                                   ثابت curl_ssl_backend ***avail)
{
  صحيح i;

  ܐܢ(avail)
    *avail = (ثابت curl_ssl_backend **)&available_backends;

  ܐܢ(Curl_ssl != &Curl_ssl_multi)
    ܦܢܐ id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#ܐܚܪܢܐ
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  لكل(i = 0; available_backends[i]; i++) {
    ܐܢ(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      ܦܢܐ CURLSSLSET_OK;
    }
  }

  ܦܢܐ CURLSSLSET_UNKNOWN_BACKEND;
}

#ܐܚܪܢܐ /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت حرف *name,
                                   ثابت curl_ssl_backend ***avail)
{
  (فراغ)id;
  (فراغ)name;
  (فراغ)avail;
  ܦܢܐ CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

فراغ Curl_ssl_peer_cleanup(بنية ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_key);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

ساكن ssl_peer_type get_peer_type(ثابت حرف *hostname)
{
  ܐܢ(hostname && hostname[0]) {
#ifdef USE_IPV6
    بنية in6_addr addr;
#ܐܚܪܢܐ
    بنية in_addr addr;
#endif
    ܐܢ(curlx_inet_pton(AF_INET, hostname, &addr))
      ܦܢܐ CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    ܐܚܪܢܐ ܐܢ(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      ܦܢܐ CURL_SSL_PEER_IPV6;
    }
#endif
  }
  ܦܢܐ CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(بنية ssl_peer *ssl_peer,
                            بنية Curl_peer *origin,
                            بنية Curl_peer *peer,
                            بنية ssl_filter_config *sslc,
                            ثابت حرف *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  ܐܢ(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    ܦܢܐ CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  ܐܢ(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    ܐܢ(len && (origin->hostname[len - 1] == '.'))
      len--;
    ܐܢ(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      ܐܢ(!ssl_peer->sni)
        اذهب out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_key_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_key);

out:
  ܐܢ(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  ܦܢܐ result;
}

ساكن فراغ ssl_cf_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  ܐܢ(connssl) {
    connssl->ssl_impl->close(cf, data);
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
                               بنية Curl_easy *data,
                               bool *done)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية cf_call_data save;
  CURLcode result;

  ܐܢ(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    ܦܢܐ CURLE_OK;
  }

  ܐܢ(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    ܦܢܐ CURLE_FAILED_INIT;
  }

  ܐܢ(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ܐܢ(result || !*done)
      ܦܢܐ result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  ܐܢ(!connssl->prefs_checked) {
    ܐܢ(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  ܐܢ(!result && *done) {
    cf->connected = TRUE;
    ܐܢ(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    ܐܢ(Curl_tls_keylog_enabled()) {
      infof(data, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_NUMBER
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (صحيح)result, *done);
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن CURLcode ssl_cf_set_earlydata(بنية Curl_cfilter *cf,
                                     بنية Curl_easy *data,
                                     ثابت فراغ *buf, size_t blen)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  ܐܢ(blen) {
    ܐܢ(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    ܐܢ(result)
      ܦܢܐ result;
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode ssl_cf_connect_deferred(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data,
                                        ثابت فراغ *buf, size_t blen,
                                        bool *done)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  ܐܢ(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    ܐܢ(result)
      ܦܢܐ result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  ܐܢ(!result && *done) {
    ܐܢ(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    بدل(connssl->earlydata_state) {
    حالة ssl_earlydata_none:
      اكسر;
    حالة ssl_earlydata_accepted:
      ܐܢ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      اكسر;
    حالة ssl_earlydata_rejected:
      ܐܢ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      اكسر;
    افتراضي:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(عدم);
      اكسر;
    }
  }
  ܦܢܐ result;
}

ساكن bool ssl_cf_data_pending(بنية Curl_cfilter *cf,
                                ثابت بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  ܐܢ(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  ܐܚܪܢܐ
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ pending;
}

ساكن CURLcode ssl_cf_send(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            ثابت uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية cf_call_data save;
  CURLcode result = CURLE_OK;

  (فراغ)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  ܐܢ(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    ܐܢ(result)
      اذهب out;
    ܐܚܪܢܐ ܐܢ(!done) {
      result = CURLE_AGAIN;
      اذهب out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ܐܢ(connssl->earlydata_skip) {
    ܐܢ(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      اذهب out;
    }
    ܐܚܪܢܐ {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  ܐܢ(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    ܐܢ(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن CURLcode ssl_cf_recv(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data, حرف *buf, size_t len,
                            size_t *pnread)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  ܐܢ(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, عدم, 0, &done);
    ܐܢ(result)
      اذهب out;
    ܐܚܪܢܐ ܐܢ(!done) {
      result = CURLE_AGAIN;
      اذهب out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن CURLcode ssl_cf_shutdown(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                bool *done)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  ܐܢ(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    بنية cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (صحيح)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  ܦܢܐ result;
}

ساكن CURLcode ssl_cf_adjust_pollset(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      بنية easy_pollset *ps)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  ܦܢܐ result;
}

ساكن CURLcode ssl_cf_query(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية ssl_connect_data *connssl = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_SSL_INFO:
  حالة CF_QUERY_SSL_CTX_INFO:
    ܐܢ(!Curl_ssl_cf_is_proxy(cf)) {
      بنية curl_tlssessioninfo *info = pres2;
      بنية cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      ܦܢܐ CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    ܦܢܐ CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  ܦܢܐ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ساكن CURLcode ssl_cf_cntrl(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             صحيح event, صحيح arg1, فراغ *arg2)
{
  بنية ssl_connect_data *connssl = cf->ctx;

  (فراغ)arg1;
  (فراغ)arg2;
  (فراغ)data;
  بدل(event) {
  حالة CF_CTRL_CONN_INFO_UPDATE:
    ܐܢ(connssl->negotiated.alpn && !cf->sockindex) {
      ܐܢ(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      ܐܚܪܢܐ ܐܢ(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      ܐܚܪܢܐ ܐܢ(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    اكسر;
  حالة CF_CTRL_REPORT_STATS:
    ܐܢ(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    اكسر;
  }
  ܦܢܐ CURLE_OK;
}

ساكن bool cf_ssl_is_alive(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                            bool *input_pending)
{
  /*
   * This function tries to determine connection status.
   */
  ܦܢܐ cf->next ?
    cf->next->cft->is_alive(cf->next, data, input_pending) :
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
  ssl_cf_data_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#ifndef CURL_DISABLE_PROXY

بنية Curl_cftype Curl_cft_ssl_proxy = {
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

ساكن CURLcode cf_ssl_create(بنية Curl_cfilter **pcf,
                              بنية Curl_easy *data,
                              بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = عدم;
  بنية ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (فراغ)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, عدم);
#ܐܚܪܢܐ
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  ܐܢ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  ܐܢ(result)
    cf_ctx_free(ctx);
  *pcf = result ? عدم : cf;
  ܦܢܐ result;
}

ساكن CURLcode cf_ssl_peer_init(بنية Curl_cfilter *cf,
                                 بنية Curl_peer *origin,
                                 بنية Curl_peer *peer,
                                 بنية ssl_filter_config *sslc)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  حرف tls_id[80];
  connssl->ssl_impl->version(tls_id, حجم(tls_id) - 1);
  ܦܢܐ Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(بنية Curl_easy *data,
                              بنية Curl_peer *origin,
                              بنية connectdata *conn,
                              int8_t sockindex)
{
  بنية Curl_cfilter *cf;
  بنية Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  ܐܢ(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  ܐܢ(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  ܐܚܪܢܐ ܐܢ(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ܦܢܐ result;
}

CURLcode Curl_cf_ssl_insert_after(بنية Curl_cfilter *cf_at,
                                  بنية Curl_easy *data,
                                  بنية Curl_peer *origin,
                                  بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  ܐܢ(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  ܐܢ(!result) {
    Curl_conn_cf_insert_after(cf_at, cf);
#ܐܢ defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    ܐܢ((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  ܐܚܪܢܐ ܐܢ(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ܦܢܐ result;
}

#ifndef CURL_DISABLE_PROXY

ساكن CURLcode cf_ssl_proxy_create(بنية Curl_cfilter **pcf,
                                    بنية Curl_easy *data,
                                    بنية connectdata *conn)
{
  بنية Curl_cfilter *cf = عدم;
  بنية ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (فراغ)conn;
#ifdef USE_HTTP2
  ܐܢ(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  ܐܢ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  ܐܢ(result)
    cf_ctx_free(ctx);
  *pcf = result ? عدم : cf;
  ܦܢܐ result;
}

CURLcode Curl_cf_ssl_proxy_insert_after(بنية Curl_cfilter *cf_at,
                                        بنية Curl_easy *data,
                                        بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  ܐܢ(!result)
    result = cf_ssl_peer_init(cf, peer, عدم, &cf_at->conn->proxy_ssl_config);
  ܐܢ(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  ܐܚܪܢܐ ܐܢ(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ܦܢܐ result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(بنية Curl_easy *data, unsigned صحيح ssl_option)
{
  (فراغ)data;
  ܦܢܐ (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(بنية Curl_cfilter *cf)
{
  ܐܢ(Curl_ssl->supports & SSLSUPP_ECH) {
    بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ܦܢܐ !!conn_config->ech;
  }
  ܦܢܐ FALSE;
}

bool Curl_ssl_need_httpsrr(بنية Curl_cfilter *cf)
{
  ܐܢ(Curl_ssl->supports & SSLSUPP_ECH) {
    بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ܐܢ(!conn_config->ech)
      ܦܢܐ FALSE;
    ܐܢ((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      ܦܢܐ FALSE;
    ܦܢܐ TRUE;
  }
  ܦܢܐ FALSE;
}
#endif

ساكن CURLcode vtls_shutdown_blocking(بنية Curl_cfilter *cf,
                                       بنية Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  صحيح what, loop = 10;

  ܐܢ(cf->shutdown) {
    *done = TRUE;
    ܦܢܐ CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  ܟܡܐ(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    ܐܢ(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      اذهب out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    ܐܢ(result || *done)
      اذهب out;

    ܐܢ(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      ܐܢ(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        اذهب out;
      }
      ܐܚܪܢܐ ܐܢ(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        اذهب out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ܦܢܐ result;
}

CURLcode Curl_ssl_cfilter_remove(بنية Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  بنية Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : عدم;
  لكل(cf = head; cf; cf = cf->next) {
    ܐܢ(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      ܐܢ(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (صحيح)result);
      اكسر;
    }
  }
  ܦܢܐ result;
}

bool Curl_ssl_cf_is_proxy(بنية Curl_cfilter *cf)
{
  ܦܢܐ (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

بنية ssl_easy_config *
Curl_ssl_cf_get_easy_config(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (فراغ)cf;
  ܦܢܐ &data->set.ssl;
#ܐܚܪܢܐ
  ܦܢܐ Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

بنية ssl_filter_config *
Curl_ssl_cf_get_filter_config(بنية Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  ܦܢܐ &cf->conn->ssl_config;
#ܐܚܪܢܐ
  ܦܢܐ Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(بنية alpn_proto_buf *buf,
                                ثابت بنية alpn_spec *spec)
{
  size_t i, len;
  صحيح off = 0;
  unsigned حرف blen;

  memset(buf, 0, حجم(*buf));
  لكل(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    ܐܢ(len >= ALPN_NAME_MAX)
      ܦܢܐ CURLE_FAILED_INIT;
    blen = (unsigned حرف)len;
    ܐܢ(off + blen + 1 >= (صحيح)حجم(buf->data))
      ܦܢܐ CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  ܦܢܐ CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(بنية alpn_proto_buf *buf,
                                ثابت بنية alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, حجم(*buf));
  لكل(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    ܐܢ(len >= ALPN_NAME_MAX)
      ܦܢܐ CURLE_FAILED_INIT;
    ܐܢ(off + len + 2 >= حجم(buf->data))
      ܦܢܐ CURLE_FAILED_INIT;
    ܐܢ(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (صحيح)off;
  ܦܢܐ CURLE_OK;
}

bool Curl_alpn_contains_proto(ثابت بنية alpn_spec *spec,
                              ثابت حرف *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  لكل(i = 0; spec && plen && i < spec->count; ++i) {
    size_t slen = strlen(spec->entries[i]);
    ܐܢ((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      ܦܢܐ TRUE;
  }
  ܦܢܐ FALSE;
}

فراغ Curl_alpn_restrict_to(بنية alpn_spec *spec, ثابت حرف *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < حجم(spec->entries[0]));
  ܐܢ(plen < حجم(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->count = 1;
  }
}

فراغ Curl_alpn_copy(بنية alpn_spec *dest, ثابت بنية alpn_spec *src)
{
  ܐܢ(src)
    memcpy(dest, src, حجم(*dest));
  ܐܚܪܢܐ
    memset(dest, 0, حجم(*dest));
}

CURLcode Curl_alpn_set_negotiated(بنية Curl_cfilter *cf,
                                  بنية Curl_easy *data,
                                  بنية ssl_connect_data *connssl,
                                  ثابت unsigned حرف *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (فراغ)cf;

  ܐܢ(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    ܐܢ(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    ܐܚܪܢܐ ܐܢ(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    ܐܚܪܢܐ ܐܢ((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (صحيح)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    اذهب out;
  }

  ܐܢ(proto && proto_len) {
    ܐܢ(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    connssl->negotiated.alpn = curlx_memdup0((ثابت حرف *)proto, proto_len);
    ܐܢ(!connssl->negotiated.alpn)
      ܦܢܐ CURLE_OUT_OF_MEMORY;
  }

  ܐܢ(proto && proto_len) {
    ܐܢ(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (صحيح)proto_len, proto);
    ܐܚܪܢܐ
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (صحيح)proto_len, proto);
  }
  ܐܚܪܢܐ {
    ܐܢ(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    ܐܚܪܢܐ
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  ܦܢܐ result;
}

CURLcode Curl_on_session_reuse(بنية Curl_cfilter *cf,
                               بنية Curl_easy *data,
                               بنية alpn_spec *alpns,
                               بنية Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  ܐܢ(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  ܐܚܪܢܐ ܐܢ(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  ܐܚܪܢܐ {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ثابت unsigned حرف *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  ܦܢܐ result;
}

بنية Curl_ssl_session *Curl_ssl_get_cf_session(بنية Curl_easy *data,
                                                 ثابت بنية Curl_cftype *cft,
                                                 int8_t sockindex)
{
  ܐܢ(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#ܐܚܪܢܐ
     (cft == &Curl_cft_ssl)) {
#endif
    بنية Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    لكل(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      ܐܢ(cf1->cft == cft)
        اكسر;
    }
    ܐܢ(cf1) {
      بنية ssl_connect_data *connssl = cf1->ctx;
      ܐܢ(connssl)
        ܦܢܐ connssl->session;
    }
  }
  ܦܢܐ عدم;
}

#endif /* USE_SSL */
