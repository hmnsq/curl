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
#𐤀𐤌 !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
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
  𐤀𐤌(!use_alpn)
    أرجع عدم;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  𐤀𐤌(only_http_10 && (wanted & CURL_HTTP_V1x))
    أرجع &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  𐤀𐤌(wanted & CURL_HTTP_V2x) {
    𐤀𐤌(wanted & CURL_HTTP_V1x)
      أرجع (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    أرجع &ALPN_SPEC_H2;
  }
#وإلا
  (فراغ)wanted;
  (فراغ)preferred;
#endif
  أرجع &ALPN_SPEC_H11;
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
  أرجع Curl_ssl->info.id;
#وإلا
  أرجع CURLSSLBACKEND_NONE;
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
  𐤀𐤌(init_ssl)
    أرجع 1;
  init_ssl = TRUE; /* never again */

  𐤀𐤌(Curl_ssl->init)
    أرجع Curl_ssl->init();
  أرجع 1;
}

ساكن bool ssl_prefs_check(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data)
{
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  𐤀𐤌(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    أرجع FALSE;
  }

  بدل(conn_config->version_max) {
  حالة CURL_SSLVERSION_MAX_NONE:
  حالة CURL_SSLVERSION_MAX_DEFAULT:
    اكسر;

  افتراضي:
    𐤀𐤌((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      أرجع FALSE;
    }
  }

  أرجع TRUE;
}

ساكن بنية ssl_connect_data *cf_ctx_new(بنية Curl_easy *data,
                                           ثابت بنية alpn_spec *alpn)
{
  بنية ssl_connect_data *ctx;

  (فراغ)data;
  ctx = curlx_calloc(1, حجم(*ctx));
  𐤀𐤌(!ctx)
    أرجع عدم;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  𐤀𐤌(!ctx->backend) {
    curlx_free(ctx);
    أرجع عدم;
  }
  أرجع ctx;
}

ساكن فراغ cf_ctx_free(بنية ssl_connect_data *ctx)
{
  𐤀𐤌(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(بنية Curl_easy *data, int8_t sockindex,
                                      بنية dynbuf *binding)
{
  𐤀𐤌(Curl_ssl->get_channel_binding)
    أرجع Curl_ssl->get_channel_binding(data, sockindex, binding);
  أرجع CURLE_OK;
}

فراغ Curl_ssl_close_all(بنية Curl_easy *data)
{
  𐤀𐤌(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 بنية easy_pollset *ps)
{
  بنية ssl_connect_data *connssl = cf->ctx;

  𐤀𐤌(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    𐤀𐤌(sock != CURL_SOCKET_BAD) {
      𐤀𐤌(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      وإلا {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    أرجع result;
  }
  أرجع CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
بنية curl_slist *Curl_ssl_engines_list(بنية Curl_easy *data)
{
  𐤀𐤌(Curl_ssl->engines_list)
    أرجع Curl_ssl->engines_list(data);
  أرجع عدم;
}

ساكن size_t multissl_version(حرف *buffer, size_t size);

فراغ Curl_ssl_version(حرف *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (فراغ)multissl_version(buffer, size);
#وإلا
  (فراغ)Curl_ssl->version(buffer, size);
#endif
}

فراغ Curl_ssl_free_certinfo(بنية Curl_easy *data)
{
  بنية curl_certinfo *ci = &data->info.certs;

  𐤀𐤌(ci->num_of_certs) {
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
  𐤀𐤌(!table)
    أرجع CURLE_OUT_OF_MEMORY;

  ci->num_of_certs = num;
  ci->certinfo = table;

  أرجع CURLE_OK;
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

  𐤀𐤌(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, value, valuelen))
    أرجع CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  𐤀𐤌(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  أرجع result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(بنية Curl_easy *data,
                         unsigned حرف *buffer, size_t length)
{
  DEBUGASSERT(length == حجم(صحيح));
  𐤀𐤌(Curl_ssl->random)
    أرجع Curl_ssl->random(data, buffer, length);
  وإلا
    أرجع CURLE_NOT_BUILT_IN;
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
  𐤀𐤌(!pem)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  𐤀𐤌(!begin_pos)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  pem_count = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  𐤀𐤌(pem_count && '\n' != pem[pem_count - 1])
    أرجع CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_count += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_count, "\n-----END PUBLIC KEY-----");
  𐤀𐤌(!end_pos)
    أرجع CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  طالما(pem_count < pem_len) {
    𐤀𐤌('\n' != pem[pem_count] && '\r' != pem[pem_count]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_count], 1);
      𐤀𐤌(result)
        أرجع result;
    }
    ++pem_count;
  }

  𐤀𐤌(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  وإلا
    result = CURLE_BAD_CONTENT_ENCODING;

  أرجع result;
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
  𐤀𐤌(!pinnedpubkey)
    أرجع CURLE_OK;
  𐤀𐤌(!pubkey || !pubkeylen)
    أرجع result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  𐤀𐤌(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    حرف *cert_hash = عدم;
    ثابت حرف *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned حرف *sha256sumdigest;

    𐤀𐤌(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      أرجع result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    𐤀𐤌(!sha256sumdigest)
      أرجع CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    𐤀𐤌(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    𐤀𐤌(encode)
      أرجع encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    طالما(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      𐤀𐤌(cert_hash_len == pinned_hash_len &&
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
  وإلا {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    بنية dynbuf buf;
    unsigned حرف *pem_ptr = عدم;
    size_t left;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    𐤀𐤌(!fp)
      أرجع result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    𐤀𐤌(fseek(fp, 0, SEEK_END))
      اذهب end;
    filesize = ftell(fp);
    𐤀𐤌(fseek(fp, 0, SEEK_SET))
      اذهب end;
    𐤀𐤌(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      اذهب end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    𐤀𐤌(pubkeylen > size)
      اذهب end;

    /*
     * Read the file into the dynbuf
     */
    left = size;
    𐤐𐤏𐤋 {
      حرف buffer[1024];
      size_t want = left > حجم(buffer) ? حجم(buffer) : left;
      𐤀𐤌(fread(buffer, 1, want, fp) != want)
        اذهب end;
      𐤀𐤌(curlx_dyn_addn(&buf, buffer, want))
        اذهب end;
      left -= want;
    } طالما(left);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    𐤀𐤌(pubkeylen == size) {
      𐤀𐤌(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      اذهب end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    𐤀𐤌(pem_read)
      اذهب end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    𐤀𐤌(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  أرجع result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(فراغ)
{
  𐤀𐤌(Curl_ssl->cert_status_request)
    أرجع Curl_ssl->cert_status_request();
  أرجع FALSE;
}

ساكن صحيح multissl_init(فراغ)
{
  𐤀𐤌(multissl_setup(عدم))
    أرجع 1;
  𐤀𐤌(Curl_ssl->init)
    أرجع Curl_ssl->init();
  أرجع 1;
}

ساكن CURLcode multissl_random(بنية Curl_easy *data,
                                unsigned حرف *entropy, size_t length)
{
  𐤀𐤌(multissl_setup(عدم))
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_ssl->random(data, entropy, length);
}

ساكن CURLcode multissl_connect(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data, bool *done)
{
  𐤀𐤌(multissl_setup(عدم))
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_ssl->do_connect(cf, data, done);
}

ساكن CURLcode multissl_adjust_pollset(بنية Curl_cfilter *cf,
                                        بنية Curl_easy *data,
                                        بنية easy_pollset *ps)
{
  𐤀𐤌(multissl_setup(عدم))
    أرجع CURLE_OK;
  أرجع Curl_ssl->adjust_pollset(cf, data, ps);
}

ساكن فراغ *multissl_get_internals(بنية ssl_connect_data *connssl,
                                    CURLINFO info)
{
  𐤀𐤌(multissl_setup(عدم))
    أرجع عدم;
  أرجع Curl_ssl->get_internals(connssl, info);
}

ساكن فراغ multissl_close(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  𐤀𐤌(multissl_setup(عدم))
    أرجع;
  Curl_ssl->close(cf, data);
}

ساكن CURLcode multissl_recv_plain(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    حرف *buf, size_t len, size_t *pnread)
{
  𐤀𐤌(multissl_setup(عدم))
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

ساكن CURLcode multissl_send_plain(بنية Curl_cfilter *cf,
                                    بنية Curl_easy *data,
                                    ثابت فراغ *mem, size_t len,
                                    size_t *pnwritten)
{
  𐤀𐤌(multissl_setup(عدم))
    أرجع CURLE_FAILED_INIT;
  أرجع Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
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
#وإلا
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
  𐤀𐤌(init_ssl) {
    /* only cleanup if we did a previous init */
    𐤀𐤌(Curl_ssl->cleanup)
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

  𐤀𐤌(current != selected) {
    حرف *p = backends;
    ثابت حرف *end = backends + حجم(backends);
    صحيح i;

    selected = current;

    backends[0] = '\0';

    لكل(i = 0; available_backends[i]; ++i) {
      حرف vb[200];
      bool paren = (selected != available_backends[i]);

      𐤀𐤌(available_backends[i]->version(vb, حجم(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  𐤀𐤌(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  أرجع 0;
}

ساكن صحيح multissl_setup(ثابت بنية Curl_ssl *backend)
{
  صحيح i;
  حرف *env;

  𐤀𐤌(Curl_ssl != &Curl_ssl_multi)
    أرجع 1;

  𐤀𐤌(backend) {
    Curl_ssl = backend;
    أرجع 0;
  }

  𐤀𐤌(!available_backends[0])
    أرجع 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  𐤀𐤌(env) {
    لكل(i = 0; available_backends[i]; i++) {
      𐤀𐤌(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        أرجع 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  لكل(i = 0; available_backends[i]; i++) {
    𐤀𐤌(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      أرجع 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  أرجع 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت حرف *name,
                                   ثابت curl_ssl_backend ***avail)
{
  صحيح i;

  𐤀𐤌(avail)
    *avail = (ثابت curl_ssl_backend **)&available_backends;

  𐤀𐤌(Curl_ssl != &Curl_ssl_multi)
    أرجع id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#وإلا
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  لكل(i = 0; available_backends[i]; i++) {
    𐤀𐤌(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
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
  𐤀𐤌(hostname && hostname[0]) {
#ifdef USE_IPV6
    بنية in6_addr addr;
#وإلا
    بنية in_addr addr;
#endif
    𐤀𐤌(curlx_inet_pton(AF_INET, hostname, &addr))
      أرجع CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    وإلا 𐤀𐤌(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      أرجع CURL_SSL_PEER_IPV6;
    }
#endif
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
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  𐤀𐤌(!ssl_peer || !origin) {
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
  𐤀𐤌(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    𐤀𐤌(len && (origin->hostname[len - 1] == '.'))
      len--;
    𐤀𐤌(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      𐤀𐤌(!ssl_peer->sni)
        اذهب out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_key_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_key);

out:
  𐤀𐤌(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  أرجع result;
}

ساكن فراغ ssl_cf_destroy(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  𐤀𐤌(connssl) {
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

  𐤀𐤌(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    أرجع CURLE_OK;
  }

  𐤀𐤌(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    أرجع CURLE_FAILED_INIT;
  }

  𐤀𐤌(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    𐤀𐤌(result || !*done)
      أرجع result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  𐤀𐤌(!connssl->prefs_checked) {
    𐤀𐤌(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  𐤀𐤌(!result && *done) {
    cf->connected = TRUE;
    𐤀𐤌(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    𐤀𐤌(Curl_tls_keylog_enabled()) {
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
  أرجع result;
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
  𐤀𐤌(blen) {
    𐤀𐤌(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    𐤀𐤌(result)
      أرجع result;
  }
  أرجع CURLE_OK;
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
  𐤀𐤌(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    𐤀𐤌(result)
      أرجع result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  𐤀𐤌(!result && *done) {
    𐤀𐤌(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    بدل(connssl->earlydata_state) {
    حالة ssl_earlydata_none:
      اكسر;
    حالة ssl_earlydata_accepted:
      𐤀𐤌(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      اكسر;
    حالة ssl_earlydata_rejected:
      𐤀𐤌(!Curl_ssl_cf_is_proxy(cf))
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
  أرجع result;
}

ساكن bool ssl_cf_data_pending(بنية Curl_cfilter *cf,
                                ثابت بنية Curl_easy *data)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  بنية cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  𐤀𐤌(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  وإلا
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  أرجع pending;
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

  𐤀𐤌(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    𐤀𐤌(result)
      اذهب out;
    وإلا 𐤀𐤌(!done) {
      result = CURLE_AGAIN;
      اذهب out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  𐤀𐤌(connssl->earlydata_skip) {
    𐤀𐤌(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      اذهب out;
    }
    وإلا {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  𐤀𐤌(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    𐤀𐤌(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  أرجع result;
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
  𐤀𐤌(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, عدم, 0, &done);
    𐤀𐤌(result)
      اذهب out;
    وإلا 𐤀𐤌(!done) {
      result = CURLE_AGAIN;
      اذهب out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  أرجع result;
}

ساكن CURLcode ssl_cf_shutdown(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                bool *done)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  𐤀𐤌(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    بنية cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (صحيح)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  أرجع result;
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
  أرجع result;
}

ساكن CURLcode ssl_cf_query(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             صحيح query, صحيح *pres1, فراغ *pres2)
{
  بنية ssl_connect_data *connssl = cf->ctx;

  بدل(query) {
  حالة CF_QUERY_SSL_INFO:
  حالة CF_QUERY_SSL_CTX_INFO:
    𐤀𐤌(!Curl_ssl_cf_is_proxy(cf)) {
      بنية curl_tlssessioninfo *info = pres2;
      بنية cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CF_QUERY_ALPN_NEGOTIATED: {
    ثابت حرف **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    أرجع CURLE_OK;
  }
  افتراضي:
    اكسر;
  }
  أرجع cf->next ?
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
    𐤀𐤌(connssl->negotiated.alpn && !cf->sockindex) {
      𐤀𐤌(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      وإلا 𐤀𐤌(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      وإلا 𐤀𐤌(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    اكسر;
  حالة CF_CTRL_REPORT_STATS:
    𐤀𐤌(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن bool cf_ssl_is_alive(بنية Curl_cfilter *cf, بنية Curl_easy *data,
                            bool *input_pending)
{
  /*
   * This function tries to determine connection status.
   */
  أرجع cf->next ?
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
#وإلا
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  𐤀𐤌(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  𐤀𐤌(result)
    cf_ctx_free(ctx);
  *pcf = result ? عدم : cf;
  أرجع result;
}

ساكن CURLcode cf_ssl_peer_init(بنية Curl_cfilter *cf,
                                 بنية Curl_peer *origin,
                                 بنية Curl_peer *peer,
                                 بنية ssl_filter_config *sslc)
{
  بنية ssl_connect_data *connssl = cf->ctx;
  حرف tls_id[80];
  connssl->ssl_impl->version(tls_id, حجم(tls_id) - 1);
  أرجع Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
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
  𐤀𐤌(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  𐤀𐤌(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  وإلا 𐤀𐤌(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  أرجع result;
}

CURLcode Curl_cf_ssl_insert_after(بنية Curl_cfilter *cf_at,
                                  بنية Curl_easy *data,
                                  بنية Curl_peer *origin,
                                  بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  𐤀𐤌(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  𐤀𐤌(!result) {
    Curl_conn_cf_insert_after(cf_at, cf);
#𐤀𐤌 defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    𐤀𐤌((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  وإلا 𐤀𐤌(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  أرجع result;
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
  𐤀𐤌(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  𐤀𐤌(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    اذهب out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  𐤀𐤌(result)
    cf_ctx_free(ctx);
  *pcf = result ? عدم : cf;
  أرجع result;
}

CURLcode Curl_cf_ssl_proxy_insert_after(بنية Curl_cfilter *cf_at,
                                        بنية Curl_easy *data,
                                        بنية Curl_peer *peer)
{
  بنية Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  𐤀𐤌(!result)
    result = cf_ssl_peer_init(cf, peer, عدم, &cf_at->conn->proxy_ssl_config);
  𐤀𐤌(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  وإلا 𐤀𐤌(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  أرجع result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(بنية Curl_easy *data, unsigned صحيح ssl_option)
{
  (فراغ)data;
  أرجع (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(بنية Curl_cfilter *cf)
{
  𐤀𐤌(Curl_ssl->supports & SSLSUPP_ECH) {
    بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    أرجع !!conn_config->ech;
  }
  أرجع FALSE;
}

bool Curl_ssl_need_httpsrr(بنية Curl_cfilter *cf)
{
  𐤀𐤌(Curl_ssl->supports & SSLSUPP_ECH) {
    بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    𐤀𐤌(!conn_config->ech)
      أرجع FALSE;
    𐤀𐤌((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      أرجع FALSE;
    أرجع TRUE;
  }
  أرجع FALSE;
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

  𐤀𐤌(cf->shutdown) {
    *done = TRUE;
    أرجع CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  طالما(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    𐤀𐤌(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      اذهب out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    𐤀𐤌(result || *done)
      اذهب out;

    𐤀𐤌(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      𐤀𐤌(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        اذهب out;
      }
      وإلا 𐤀𐤌(what == 0) {
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
  أرجع result;
}

CURLcode Curl_ssl_cfilter_remove(بنية Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  بنية Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : عدم;
  لكل(cf = head; cf; cf = cf->next) {
    𐤀𐤌(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      𐤀𐤌(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (صحيح)result);
      اكسر;
    }
  }
  أرجع result;
}

bool Curl_ssl_cf_is_proxy(بنية Curl_cfilter *cf)
{
  أرجع (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

بنية ssl_easy_config *
Curl_ssl_cf_get_easy_config(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (فراغ)cf;
  أرجع &data->set.ssl;
#وإلا
  أرجع Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

بنية ssl_filter_config *
Curl_ssl_cf_get_filter_config(بنية Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  أرجع &cf->conn->ssl_config;
#وإلا
  أرجع Curl_ssl_cf_is_proxy(cf) ?
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
    𐤀𐤌(len >= ALPN_NAME_MAX)
      أرجع CURLE_FAILED_INIT;
    blen = (unsigned حرف)len;
    𐤀𐤌(off + blen + 1 >= (صحيح)حجم(buf->data))
      أرجع CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  أرجع CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(بنية alpn_proto_buf *buf,
                                ثابت بنية alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, حجم(*buf));
  لكل(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    𐤀𐤌(len >= ALPN_NAME_MAX)
      أرجع CURLE_FAILED_INIT;
    𐤀𐤌(off + len + 2 >= حجم(buf->data))
      أرجع CURLE_FAILED_INIT;
    𐤀𐤌(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (صحيح)off;
  أرجع CURLE_OK;
}

bool Curl_alpn_contains_proto(ثابت بنية alpn_spec *spec,
                              ثابت حرف *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  لكل(i = 0; spec && plen && i < spec->count; ++i) {
    size_t slen = strlen(spec->entries[i]);
    𐤀𐤌((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      أرجع TRUE;
  }
  أرجع FALSE;
}

فراغ Curl_alpn_restrict_to(بنية alpn_spec *spec, ثابت حرف *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < حجم(spec->entries[0]));
  𐤀𐤌(plen < حجم(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->count = 1;
  }
}

فراغ Curl_alpn_copy(بنية alpn_spec *dest, ثابت بنية alpn_spec *src)
{
  𐤀𐤌(src)
    memcpy(dest, src, حجم(*dest));
  وإلا
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

  𐤀𐤌(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    𐤀𐤌(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    وإلا 𐤀𐤌(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    وإلا 𐤀𐤌((strlen(connssl->negotiated.alpn) != proto_len) ||
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

  𐤀𐤌(proto && proto_len) {
    𐤀𐤌(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      اذهب out;
    }
    connssl->negotiated.alpn = curlx_memdup0((ثابت حرف *)proto, proto_len);
    𐤀𐤌(!connssl->negotiated.alpn)
      أرجع CURLE_OUT_OF_MEMORY;
  }

  𐤀𐤌(proto && proto_len) {
    𐤀𐤌(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (صحيح)proto_len, proto);
    وإلا
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (صحيح)proto_len, proto);
  }
  وإلا {
    𐤀𐤌(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    وإلا
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  أرجع result;
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

  𐤀𐤌(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  وإلا 𐤀𐤌(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  وإلا {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ثابت unsigned حرف *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  أرجع result;
}

بنية Curl_ssl_session *Curl_ssl_get_cf_session(بنية Curl_easy *data,
                                                 ثابت بنية Curl_cftype *cft,
                                                 int8_t sockindex)
{
  𐤀𐤌(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#وإلا
     (cft == &Curl_cft_ssl)) {
#endif
    بنية Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    لكل(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      𐤀𐤌(cf1->cft == cft)
        اكسر;
    }
    𐤀𐤌(cf1) {
      بنية ssl_connect_data *connssl = cf1->ctx;
      𐤀𐤌(connssl)
        أرجع connssl->session;
    }
  }
  أرجع عدم;
}

#endif /* USE_SSL */
