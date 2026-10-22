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

#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"

#அந்தர்பாவ "vtls/vtls.h" /* generic SSL protos etc */
#அந்தர்பாவ "vtls/vtls_int.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "vtls/keylog.h"

#அந்தர்பாவ "vtls/openssl.h"        /* OpenSSL versions */
#அந்தர்பாவ "vtls/gtls.h"           /* GnuTLS versions */
#அந்தர்பாவ "vtls/wolfssl.h"        /* wolfSSL versions */
#அந்தர்பாவ "vtls/schannel.h"       /* Schannel SSPI version */
#அந்தர்பாவ "vtls/mbedtls.h"        /* mbedTLS versions */
#அந்தர்பாவ "vtls/rustls.h"         /* Rustls versions */

#அந்தர்பாவ "slist.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "progress.h"
#அந்தர்பாவ "curlx/win32-fopen.h"
#அந்தர்பாவ "curl_sha256.h"
#அந்தர்பாவ "curlx/base64.h"
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "setopt.h"
#அந்தர்பாவ "vdns/cf-dns.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "curlx/strcopy.h"

#ifdef USE_APPLE_SECTRUST
#அந்தர்பாவ <Security/Security.h>
#endif

#ifdef USE_SSL
#எனில் !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
நிலையான மாறிலி கட்டமைப்பு alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
நிலையான மாறிலி கட்டமைப்பு alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
நிலையான மாறிலி கட்டமைப்பு alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
நிலையான மாறிலி கட்டமைப்பு alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
நிலையான மாறிலி கட்டமைப்பு alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

நிலையான மாறிலி கட்டமைப்பு alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  எனில்(!use_alpn)
    திரும்பு வெற்று;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  எனில்(only_http_10 && (wanted & CURL_HTTP_V1x))
    திரும்பு &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  எனில்(wanted & CURL_HTTP_V2x) {
    எனில்(wanted & CURL_HTTP_V1x)
      திரும்பு (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    திரும்பு &ALPN_SPEC_H2;
  }
#இல்லையெனில்
  (வெறுமை)wanted;
  (வெறுமை)preferred;
#endif
  திரும்பு &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
நிலையான முழு multissl_setup(மாறிலி கட்டமைப்பு Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(வெறுமை)
{
#ifdef USE_SSL
  multissl_setup(வெற்று);
  திரும்பு Curl_ssl->info.id;
#இல்லையெனில்
  திரும்பு CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
நிலையான bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
முழு Curl_ssl_init(வெறுமை)
{
  /* make sure this is only done once */
  எனில்(init_ssl)
    திரும்பு 1;
  init_ssl = TRUE; /* never again */

  எனில்(Curl_ssl->init)
    திரும்பு Curl_ssl->init();
  திரும்பு 1;
}

நிலையான bool ssl_prefs_check(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  எனில்(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    திரும்பு FALSE;
  }

  தேர்வு(conn_config->version_max) {
  நிலை CURL_SSLVERSION_MAX_NONE:
  நிலை CURL_SSLVERSION_MAX_DEFAULT:
    நிறுத்து;

  இயல்பு:
    எனில்((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      திரும்பு FALSE;
    }
  }

  திரும்பு TRUE;
}

நிலையான கட்டமைப்பு ssl_connect_data *cf_ctx_new(கட்டமைப்பு Curl_easy *data,
                                           மாறிலி கட்டமைப்பு alpn_spec *alpn)
{
  கட்டமைப்பு ssl_connect_data *ctx;

  (வெறுமை)data;
  ctx = curlx_calloc(1, அளவு(*ctx));
  எனில்(!ctx)
    திரும்பு வெற்று;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  எனில்(!ctx->backend) {
    curlx_free(ctx);
    திரும்பு வெற்று;
  }
  திரும்பு ctx;
}

நிலையான வெறுமை cf_ctx_free(கட்டமைப்பு ssl_connect_data *ctx)
{
  எனில்(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(கட்டமைப்பு Curl_easy *data, int8_t sockindex,
                                      கட்டமைப்பு dynbuf *binding)
{
  எனில்(Curl_ssl->get_channel_binding)
    திரும்பு Curl_ssl->get_channel_binding(data, sockindex, binding);
  திரும்பு CURLE_OK;
}

வெறுமை Curl_ssl_close_all(கட்டமைப்பு Curl_easy *data)
{
  எனில்(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;

  எனில்(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    எனில்(sock != CURL_SOCKET_BAD) {
      எனில்(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      இல்லையெனில் {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    திரும்பு result;
  }
  திரும்பு CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
கட்டமைப்பு curl_slist *Curl_ssl_engines_list(கட்டமைப்பு Curl_easy *data)
{
  எனில்(Curl_ssl->engines_list)
    திரும்பு Curl_ssl->engines_list(data);
  திரும்பு வெற்று;
}

நிலையான size_t multissl_version(எழுத்து *buffer, size_t size);

வெறுமை Curl_ssl_version(எழுத்து *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (வெறுமை)multissl_version(buffer, size);
#இல்லையெனில்
  (வெறுமை)Curl_ssl->version(buffer, size);
#endif
}

வெறுமை Curl_ssl_free_certinfo(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு curl_certinfo *ci = &data->info.certs;

  எனில்(ci->இலக்கம்_of_certs) {
    /* free all individual lists used */
    முழு i;
    க்கு(i = 0; i < ci->இலக்கம்_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = வெற்று;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->இலக்கம்_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(கட்டமைப்பு Curl_easy *data, முழு இலக்கம்)
{
  கட்டமைப்பு curl_certinfo *ci = &data->info.certs;
  கட்டமைப்பு curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)இலக்கம், அளவு(கட்டமைப்பு curl_slist *));
  எனில்(!table)
    திரும்பு CURLE_OUT_OF_MEMORY;

  ci->இலக்கம்_of_certs = இலக்கம்;
  ci->certinfo = table;

  திரும்பு CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_தள்ளு_certinfo_len(கட்டமைப்பு Curl_easy *data,
                                    முழு certnum,
                                    மாறிலி எழுத்து *label,
                                    மாறிலி எழுத்து *மதிப்பு,
                                    size_t valuelen)
{
  கட்டமைப்பு curl_certinfo *ci = &data->info.certs;
  கட்டமைப்பு curl_slist *nl;
  CURLcode result = CURLE_OK;
  கட்டமைப்பு dynbuf build;

  DEBUGASSERT(certnum < ci->இலக்கம்_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  எனில்(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, மதிப்பு, valuelen))
    திரும்பு CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  எனில்(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  திரும்பு result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(கட்டமைப்பு Curl_easy *data,
                         unsigned எழுத்து *buffer, size_t length)
{
  DEBUGASSERT(length == அளவு(முழு));
  எனில்(Curl_ssl->random)
    திரும்பு Curl_ssl->random(data, buffer, length);
  இல்லையெனில்
    திரும்பு CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

நிலையான CURLcode pubkey_pem_to_der(மாறிலி எழுத்து *pem,
                                  unsigned எழுத்து **der, size_t *der_len)
{
  மாறிலி எழுத்து *begin_pos, *end_pos;
  size_t pem_மொத்தம், pem_len;
  CURLcode result;
  கட்டமைப்பு dynbuf pbuf;

  /* if no pem, exit. */
  எனில்(!pem)
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  எனில்(!begin_pos)
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  pem_மொத்தம் = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  எனில்(pem_மொத்தம் && '\n' != pem[pem_மொத்தம் - 1])
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_மொத்தம் += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_மொத்தம், "\n-----END PUBLIC KEY-----");
  எனில்(!end_pos)
    திரும்பு CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  போது(pem_மொத்தம் < pem_len) {
    எனில்('\n' != pem[pem_மொத்தம்] && '\r' != pem[pem_மொத்தம்]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_மொத்தம்], 1);
      எனில்(result)
        திரும்பு result;
    }
    ++pem_மொத்தம்;
  }

  எனில்(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  இல்லையெனில்
    result = CURLE_BAD_CONTENT_ENCODING;

  திரும்பு result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *pinnedpubkey,
                              மாறிலி unsigned எழுத்து *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  எனில்(!pinnedpubkey)
    திரும்பு CURLE_OK;
  எனில்(!pubkey || !pubkeylen)
    திரும்பு result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  எனில்(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    எழுத்து *cert_hash = வெற்று;
    மாறிலி எழுத்து *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned எழுத்து *sha256sumdigest;

    எனில்(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      திரும்பு result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    எனில்(!sha256sumdigest)
      திரும்பு CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    எனில்(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    எனில்(encode)
      திரும்பு encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    போது(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      எனில்(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        நிறுத்து;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (முழு)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : வெற்று;
    }
    curlx_safefree(cert_hash);
  }
  இல்லையெனில் {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    கட்டமைப்பு dynbuf buf;
    unsigned எழுத்து *pem_ptr = வெற்று;
    size_t இடது;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    எனில்(!fp)
      திரும்பு result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    எனில்(fseek(fp, 0, SEEK_END))
      செல் end;
    filesize = ftell(fp);
    எனில்(fseek(fp, 0, SEEK_SET))
      செல் end;
    எனில்(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      செல் end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    எனில்(pubkeylen > size)
      செல் end;

    /*
     * Read the file into the dynbuf
     */
    இடது = size;
    செய் {
      எழுத்து buffer[1024];
      size_t want = இடது > அளவு(buffer) ? அளவு(buffer) : இடது;
      எனில்(fread(buffer, 1, want, fp) != want)
        செல் end;
      எனில்(curlx_dyn_addn(&buf, buffer, want))
        செல் end;
      இடது -= want;
    } போது(இடது);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    எனில்(pubkeylen == size) {
      எனில்(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      செல் end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    எனில்(pem_read)
      செல் end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    எனில்(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  திரும்பு result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(வெறுமை)
{
  எனில்(Curl_ssl->cert_status_request)
    திரும்பு Curl_ssl->cert_status_request();
  திரும்பு FALSE;
}

நிலையான முழு multissl_init(வெறுமை)
{
  எனில்(multissl_setup(வெற்று))
    திரும்பு 1;
  எனில்(Curl_ssl->init)
    திரும்பு Curl_ssl->init();
  திரும்பு 1;
}

நிலையான CURLcode multissl_random(கட்டமைப்பு Curl_easy *data,
                                unsigned எழுத்து *entropy, size_t length)
{
  எனில்(multissl_setup(வெற்று))
    திரும்பு CURLE_FAILED_INIT;
  திரும்பு Curl_ssl->random(data, entropy, length);
}

நிலையான CURLcode multissl_connect(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data, bool *done)
{
  எனில்(multissl_setup(வெற்று))
    திரும்பு CURLE_FAILED_INIT;
  திரும்பு Curl_ssl->do_connect(cf, data, done);
}

நிலையான CURLcode multissl_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு easy_pollset *ps)
{
  எனில்(multissl_setup(வெற்று))
    திரும்பு CURLE_OK;
  திரும்பு Curl_ssl->adjust_pollset(cf, data, ps);
}

நிலையான வெறுமை *multissl_get_internals(கட்டமைப்பு ssl_connect_data *connssl,
                                    CURLINFO info)
{
  எனில்(multissl_setup(வெற்று))
    திரும்பு வெற்று;
  திரும்பு Curl_ssl->get_internals(connssl, info);
}

நிலையான வெறுமை multissl_close(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  எனில்(multissl_setup(வெற்று))
    திரும்பு;
  Curl_ssl->close(cf, data);
}

நிலையான CURLcode multissl_recv_plain(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    எழுத்து *buf, size_t len, size_t *pnread)
{
  எனில்(multissl_setup(வெற்று))
    திரும்பு CURLE_FAILED_INIT;
  திரும்பு Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

நிலையான CURLcode multissl_send_plain(கட்டமைப்பு Curl_cfilter *cf,
                                    கட்டமைப்பு Curl_easy *data,
                                    மாறிலி வெறுமை *mem, size_t len,
                                    size_t *pnwritten)
{
  எனில்(multissl_setup(வெற்று))
    திரும்பு CURLE_FAILED_INIT;
  திரும்பு Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

நிலையான மாறிலி கட்டமைப்பு Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  வெற்று,                              /* cleanup */
  multissl_version,                  /* version */
  வெற்று,                              /* shutdown */
  வெற்று,                              /* data_pending */
  multissl_random,                   /* random */
  வெற்று,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  வெற்று,                              /* close_all */
  வெற்று,                              /* engines_list */
  வெற்று,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  வெற்று,                              /* get_channel_binding */
};

மாறிலி கட்டமைப்பு Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#அல்லதெனில் defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#அல்லதெனில் defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#அல்லதெனில் defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#அல்லதெனில் defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#அல்லதெனில் defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#அல்லதெனில் defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#இல்லையெனில்
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

நிலையான மாறிலி கட்டமைப்பு Curl_ssl *available_backends[] = {
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
  வெற்று
};

/* Global cleanup */
வெறுமை Curl_ssl_cleanup(வெறுமை)
{
  எனில்(init_ssl) {
    /* only cleanup if we did a previous init */
    எனில்(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

நிலையான size_t multissl_version(எழுத்து *buffer, size_t size)
{
  நிலையான மாறிலி கட்டமைப்பு Curl_ssl *selected;
  நிலையான எழுத்து backends[200];
  நிலையான size_t backends_len;
  மாறிலி கட்டமைப்பு Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  எனில்(current != selected) {
    எழுத்து *p = backends;
    மாறிலி எழுத்து *end = backends + அளவு(backends);
    முழு i;

    selected = current;

    backends[0] = '\0';

    க்கு(i = 0; available_backends[i]; ++i) {
      எழுத்து vb[200];
      bool paren = (selected != available_backends[i]);

      எனில்(available_backends[i]->version(vb, அளவு(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  எனில்(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  திரும்பு 0;
}

நிலையான முழு multissl_setup(மாறிலி கட்டமைப்பு Curl_ssl *backend)
{
  முழு i;
  எழுத்து *env;

  எனில்(Curl_ssl != &Curl_ssl_multi)
    திரும்பு 1;

  எனில்(backend) {
    Curl_ssl = backend;
    திரும்பு 0;
  }

  எனில்(!available_backends[0])
    திரும்பு 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  எனில்(env) {
    க்கு(i = 0; available_backends[i]; i++) {
      எனில்(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        திரும்பு 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  க்கு(i = 0; available_backends[i]; i++) {
    எனில்(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      திரும்பு 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  திரும்பு 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, மாறிலி எழுத்து *name,
                                   மாறிலி curl_ssl_backend ***avail)
{
  முழு i;

  எனில்(avail)
    *avail = (மாறிலி curl_ssl_backend **)&available_backends;

  எனில்(Curl_ssl != &Curl_ssl_multi)
    திரும்பு id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#இல்லையெனில்
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  க்கு(i = 0; available_backends[i]; i++) {
    எனில்(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      திரும்பு CURLSSLSET_OK;
    }
  }

  திரும்பு CURLSSLSET_UNKNOWN_BACKEND;
}

#இல்லையெனில் /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, மாறிலி எழுத்து *name,
                                   மாறிலி curl_ssl_backend ***avail)
{
  (வெறுமை)id;
  (வெறுமை)name;
  (வெறுமை)avail;
  திரும்பு CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

வெறுமை Curl_ssl_peer_cleanup(கட்டமைப்பு ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_சாவி);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

நிலையான ssl_peer_type get_peer_type(மாறிலி எழுத்து *hostname)
{
  எனில்(hostname && hostname[0]) {
#ifdef USE_IPV6
    கட்டமைப்பு in6_addr addr;
#இல்லையெனில்
    கட்டமைப்பு in_addr addr;
#endif
    எனில்(curlx_inet_pton(AF_INET, hostname, &addr))
      திரும்பு CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    இல்லையெனில் எனில்(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      திரும்பு CURL_SSL_PEER_IPV6;
    }
#endif
  }
  திரும்பு CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(கட்டமைப்பு ssl_peer *ssl_peer,
                            கட்டமைப்பு Curl_peer *origin,
                            கட்டமைப்பு Curl_peer *peer,
                            கட்டமைப்பு ssl_filter_config *sslc,
                            மாறிலி எழுத்து *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  எனில்(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    திரும்பு CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  எனில்(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    எனில்(len && (origin->hostname[len - 1] == '.'))
      len--;
    எனில்(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      எனில்(!ssl_peer->sni)
        செல் out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_சாவி_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_சாவி);

out:
  எனில்(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  திரும்பு result;
}

நிலையான வெறுமை ssl_cf_destroy(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  எனில்(connssl) {
    connssl->ssl_impl->close(cf, data);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = வெற்று;
  }
  cf->connected = FALSE;
}

நிலையான CURLcode ssl_cf_connect(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result;

  எனில்(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }

  எனில்(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    திரும்பு CURLE_FAILED_INIT;
  }

  எனில்(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    எனில்(result || !*done)
      திரும்பு result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  எனில்(!connssl->prefs_checked) {
    எனில்(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      செல் out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  எனில்(!result && *done) {
    cf->connected = TRUE;
    எனில்(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    எனில்(Curl_tls_keylog_enabled()) {
      infof(data, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_எண்
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (முழு)result, *done);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode ssl_cf_set_earlydata(கட்டமைப்பு Curl_cfilter *cf,
                                     கட்டமைப்பு Curl_easy *data,
                                     மாறிலி வெறுமை *buf, size_t blen)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  எனில்(blen) {
    எனில்(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    எனில்(result)
      திரும்பு result;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssl_cf_connect_deferred(கட்டமைப்பு Curl_cfilter *cf,
                                        கட்டமைப்பு Curl_easy *data,
                                        மாறிலி வெறுமை *buf, size_t blen,
                                        bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  எனில்(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    எனில்(result)
      திரும்பு result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  எனில்(!result && *done) {
    எனில்(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    தேர்வு(connssl->earlydata_state) {
    நிலை ssl_earlydata_none:
      நிறுத்து;
    நிலை ssl_earlydata_accepted:
      எனில்(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      நிறுத்து;
    நிலை ssl_earlydata_rejected:
      எனில்(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      நிறுத்து;
    இயல்பு:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(வெற்று);
      நிறுத்து;
    }
  }
  திரும்பு result;
}

நிலையான bool ssl_cf_data_pending(கட்டமைப்பு Curl_cfilter *cf,
                                மாறிலி கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  எனில்(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  இல்லையெனில்
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  திரும்பு pending;
}

நிலையான CURLcode ssl_cf_send(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            மாறிலி uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result = CURLE_OK;

  (வெறுமை)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  எனில்(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    எனில்(result)
      செல் out;
    இல்லையெனில் எனில்(!done) {
      result = CURLE_AGAIN;
      செல் out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  எனில்(connssl->earlydata_skip) {
    எனில்(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      செல் out;
    }
    இல்லையெனில் {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  எனில்(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    எனில்(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode ssl_cf_recv(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data, எழுத்து *buf, size_t len,
                            size_t *pnread)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  எனில்(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, வெற்று, 0, &done);
    எனில்(result)
      செல் out;
    இல்லையெனில் எனில்(!done) {
      result = CURLE_AGAIN;
      செல் out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode ssl_cf_shutdown(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  எனில்(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    கட்டமைப்பு cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (முழு)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  திரும்பு result;
}

நிலையான CURLcode ssl_cf_adjust_pollset(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு easy_pollset *ps)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  திரும்பு result;
}

நிலையான CURLcode ssl_cf_query(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             முழு query, முழு *pres1, வெறுமை *pres2)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;

  தேர்வு(query) {
  நிலை CF_QUERY_SSL_INFO:
  நிலை CF_QUERY_SSL_CTX_INFO:
    எனில்(!Curl_ssl_cf_is_proxy(cf)) {
      கட்டமைப்பு curl_tlssessioninfo *info = pres2;
      கட்டமைப்பு cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  நிலை CF_QUERY_ALPN_NEGOTIATED: {
    மாறிலி எழுத்து **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    திரும்பு CURLE_OK;
  }
  இயல்பு:
    நிறுத்து;
  }
  திரும்பு cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

நிலையான CURLcode ssl_cf_cntrl(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             முழு event, முழு arg1, வெறுமை *arg2)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;

  (வெறுமை)arg1;
  (வெறுமை)arg2;
  (வெறுமை)data;
  தேர்வு(event) {
  நிலை CF_CTRL_CONN_INFO_UPDATE:
    எனில்(connssl->negotiated.alpn && !cf->sockindex) {
      எனில்(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      இல்லையெனில் எனில்(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      இல்லையெனில் எனில்(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    நிறுத்து;
  நிலை CF_CTRL_REPORT_STATS:
    எனில்(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    நிறுத்து;
  }
  திரும்பு CURLE_OK;
}

நிலையான bool cf_ssl_is_alive(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data,
                            bool *உள்ளீடு_pending)
{
  /*
   * This function tries to determine connection status.
   */
  திரும்பு cf->next ?
    cf->next->cft->is_alive(cf->next, data, உள்ளீடு_pending) :
    FALSE; /* pessimistic in absence of data */
}

கட்டமைப்பு Curl_cftype Curl_cft_ssl = {
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

கட்டமைப்பு Curl_cftype Curl_cft_ssl_proxy = {
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

நிலையான CURLcode cf_ssl_create(கட்டமைப்பு Curl_cfilter **pcf,
                              கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (வெறுமை)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, வெற்று);
#இல்லையெனில்
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  எனில்(result)
    cf_ctx_free(ctx);
  *pcf = result ? வெற்று : cf;
  திரும்பு result;
}

நிலையான CURLcode cf_ssl_peer_init(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_peer *origin,
                                 கட்டமைப்பு Curl_peer *peer,
                                 கட்டமைப்பு ssl_filter_config *sslc)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  எழுத்து tls_id[80];
  connssl->ssl_impl->version(tls_id, அளவு(tls_id) - 1);
  திரும்பு Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_peer *origin,
                              கட்டமைப்பு connectdata *conn,
                              int8_t sockindex)
{
  கட்டமைப்பு Curl_cfilter *cf;
  கட்டமைப்பு Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  எனில்(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  எனில்(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  இல்லையெனில் எனில்(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  திரும்பு result;
}

CURLcode Curl_cf_ssl_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு Curl_peer *origin,
                                  கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  எனில்(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  எனில்(!result) {
    Curl_conn_cf_செருகு_after(cf_at, cf);
#எனில் defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    எனில்((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  இல்லையெனில் எனில்(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  திரும்பு result;
}

#ifndef CURL_DISABLE_PROXY

நிலையான CURLcode cf_ssl_proxy_create(கட்டமைப்பு Curl_cfilter **pcf,
                                    கட்டமைப்பு Curl_easy *data,
                                    கட்டமைப்பு connectdata *conn)
{
  கட்டமைப்பு Curl_cfilter *cf = வெற்று;
  கட்டமைப்பு ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (வெறுமை)conn;
#ifdef USE_HTTP2
  எனில்(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  எனில்(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    செல் out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  எனில்(result)
    cf_ctx_free(ctx);
  *pcf = result ? வெற்று : cf;
  திரும்பு result;
}

CURLcode Curl_cf_ssl_proxy_செருகு_after(கட்டமைப்பு Curl_cfilter *cf_at,
                                        கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு Curl_peer *peer)
{
  கட்டமைப்பு Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  எனில்(!result)
    result = cf_ssl_peer_init(cf, peer, வெற்று, &cf_at->conn->proxy_ssl_config);
  எனில்(!result)
    Curl_conn_cf_செருகு_after(cf_at, cf);
  இல்லையெனில் எனில்(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  திரும்பு result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(கட்டமைப்பு Curl_easy *data, unsigned முழு ssl_option)
{
  (வெறுமை)data;
  திரும்பு (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(கட்டமைப்பு Curl_cfilter *cf)
{
  எனில்(Curl_ssl->supports & SSLSUPP_ECH) {
    கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    திரும்பு !!conn_config->ech;
  }
  திரும்பு FALSE;
}

bool Curl_ssl_need_httpsrr(கட்டமைப்பு Curl_cfilter *cf)
{
  எனில்(Curl_ssl->supports & SSLSUPP_ECH) {
    கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    எனில்(!conn_config->ech)
      திரும்பு FALSE;
    எனில்((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      திரும்பு FALSE;
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}
#endif

நிலையான CURLcode vtls_shutdown_blocking(கட்டமைப்பு Curl_cfilter *cf,
                                       கட்டமைப்பு Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  கட்டமைப்பு cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  முழு what, loop = 10;

  எனில்(cf->shutdown) {
    *done = TRUE;
    திரும்பு CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  போது(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    எனில்(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      செல் out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    எனில்(result || *done)
      செல் out;

    எனில்(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      எனில்(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        செல் out;
      }
      இல்லையெனில் எனில்(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        செல் out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  திரும்பு result;
}

CURLcode Curl_ssl_cfilter_remove(கட்டமைப்பு Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  கட்டமைப்பு Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : வெற்று;
  க்கு(cf = head; cf; cf = cf->next) {
    எனில்(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      எனில்(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (முழு)result);
      நிறுத்து;
    }
  }
  திரும்பு result;
}

bool Curl_ssl_cf_is_proxy(கட்டமைப்பு Curl_cfilter *cf)
{
  திரும்பு (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

கட்டமைப்பு ssl_easy_config *
Curl_ssl_cf_get_easy_config(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (வெறுமை)cf;
  திரும்பு &data->set.ssl;
#இல்லையெனில்
  திரும்பு Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

கட்டமைப்பு ssl_filter_config *
Curl_ssl_cf_get_filter_config(கட்டமைப்பு Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  திரும்பு &cf->conn->ssl_config;
#இல்லையெனில்
  திரும்பு Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(கட்டமைப்பு alpn_proto_buf *buf,
                                மாறிலி கட்டமைப்பு alpn_spec *spec)
{
  size_t i, len;
  முழு off = 0;
  unsigned எழுத்து blen;

  memset(buf, 0, அளவு(*buf));
  க்கு(i = 0; spec && i < spec->மொத்தம்; ++i) {
    len = strlen(spec->entries[i]);
    எனில்(len >= ALPN_NAME_MAX)
      திரும்பு CURLE_FAILED_INIT;
    blen = (unsigned எழுத்து)len;
    எனில்(off + blen + 1 >= (முழு)அளவு(buf->data))
      திரும்பு CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  திரும்பு CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(கட்டமைப்பு alpn_proto_buf *buf,
                                மாறிலி கட்டமைப்பு alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, அளவு(*buf));
  க்கு(i = 0; spec && i < spec->மொத்தம்; ++i) {
    len = strlen(spec->entries[i]);
    எனில்(len >= ALPN_NAME_MAX)
      திரும்பு CURLE_FAILED_INIT;
    எனில்(off + len + 2 >= அளவு(buf->data))
      திரும்பு CURLE_FAILED_INIT;
    எனில்(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (முழு)off;
  திரும்பு CURLE_OK;
}

bool Curl_alpn_contains_proto(மாறிலி கட்டமைப்பு alpn_spec *spec,
                              மாறிலி எழுத்து *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  க்கு(i = 0; spec && plen && i < spec->மொத்தம்; ++i) {
    size_t slen = strlen(spec->entries[i]);
    எனில்((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

வெறுமை Curl_alpn_restrict_to(கட்டமைப்பு alpn_spec *spec, மாறிலி எழுத்து *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < அளவு(spec->entries[0]));
  எனில்(plen < அளவு(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->மொத்தம் = 1;
  }
}

வெறுமை Curl_alpn_copy(கட்டமைப்பு alpn_spec *dest, மாறிலி கட்டமைப்பு alpn_spec *src)
{
  எனில்(src)
    memcpy(dest, src, அளவு(*dest));
  இல்லையெனில்
    memset(dest, 0, அளவு(*dest));
}

CURLcode Curl_alpn_set_negotiated(கட்டமைப்பு Curl_cfilter *cf,
                                  கட்டமைப்பு Curl_easy *data,
                                  கட்டமைப்பு ssl_connect_data *connssl,
                                  மாறிலி unsigned எழுத்து *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (வெறுமை)cf;

  எனில்(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    எனில்(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      செல் out;
    }
    இல்லையெனில் எனில்(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      செல் out;
    }
    இல்லையெனில் எனில்((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (முழு)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      செல் out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    செல் out;
  }

  எனில்(proto && proto_len) {
    எனில்(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      செல் out;
    }
    connssl->negotiated.alpn = curlx_memdup0((மாறிலி எழுத்து *)proto, proto_len);
    எனில்(!connssl->negotiated.alpn)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }

  எனில்(proto && proto_len) {
    எனில்(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (முழு)proto_len, proto);
    இல்லையெனில்
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (முழு)proto_len, proto);
  }
  இல்லையெனில் {
    எனில்(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    இல்லையெனில்
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  திரும்பு result;
}

CURLcode Curl_on_session_reuse(கட்டமைப்பு Curl_cfilter *cf,
                               கட்டமைப்பு Curl_easy *data,
                               கட்டமைப்பு alpn_spec *alpns,
                               கட்டமைப்பு Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  கட்டமைப்பு ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  எனில்(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  இல்லையெனில் எனில்(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  இல்லையெனில் {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (மாறிலி unsigned எழுத்து *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  திரும்பு result;
}

கட்டமைப்பு Curl_ssl_session *Curl_ssl_get_cf_session(கட்டமைப்பு Curl_easy *data,
                                                 மாறிலி கட்டமைப்பு Curl_cftype *cft,
                                                 int8_t sockindex)
{
  எனில்(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#இல்லையெனில்
     (cft == &Curl_cft_ssl)) {
#endif
    கட்டமைப்பு Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    க்கு(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      எனில்(cf1->cft == cft)
        நிறுத்து;
    }
    எனில்(cf1) {
      கட்டமைப்பு ssl_connect_data *connssl = cf1->ctx;
      எனில்(connssl)
        திரும்பு connssl->session;
    }
  }
  திரும்பு வெற்று;
}

#endif /* USE_SSL */
