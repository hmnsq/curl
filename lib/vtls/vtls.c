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

#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"

#अन्तर्भाव "vtls/vtls.h" /* generic SSL protos etc */
#अन्तर्भाव "vtls/vtls_int.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vtls/keylog.h"

#अन्तर्भाव "vtls/openssl.h"        /* OpenSSL versions */
#अन्तर्भाव "vtls/gtls.h"           /* GnuTLS versions */
#अन्तर्भाव "vtls/wolfssl.h"        /* wolfSSL versions */
#अन्तर्भाव "vtls/schannel.h"       /* Schannel SSPI version */
#अन्तर्भाव "vtls/mbedtls.h"        /* mbedTLS versions */
#अन्तर्भाव "vtls/rustls.h"         /* Rustls versions */

#अन्तर्भाव "slist.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "curlx/win32-fopen.h"
#अन्तर्भाव "curl_sha256.h"
#अन्तर्भाव "curlx/base64.h"
#अन्तर्भाव "curlx/inet_pton.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "setopt.h"
#अन्तर्भाव "vdns/cf-dns.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/strcopy.h"

#ifdef USE_APPLE_SECTRUST
#अन्तर्भाव <Security/Security.h>
#endif

#ifdef USE_SSL
#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

स्थिर ध्रुव संरचना alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  यदि(!use_alpn)
    प्रत्यागम रिक्त;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  यदि(only_http_10 && (wanted & CURL_HTTP_V1x))
    प्रत्यागम &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  यदि(wanted & CURL_HTTP_V2x) {
    यदि(wanted & CURL_HTTP_V1x)
      प्रत्यागम (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    प्रत्यागम &ALPN_SPEC_H2;
  }
#अन्यथा
  (शून्य)wanted;
  (शून्य)preferred;
#endif
  प्रत्यागम &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
स्थिर पूर्णाङ्क multissl_setup(ध्रुव संरचना Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(शून्य)
{
#ifdef USE_SSL
  multissl_setup(रिक्त);
  प्रत्यागम Curl_ssl->info.id;
#अन्यथा
  प्रत्यागम CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
स्थिर bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
पूर्णाङ्क Curl_ssl_init(शून्य)
{
  /* make sure this is only done once */
  यदि(init_ssl)
    प्रत्यागम 1;
  init_ssl = TRUE; /* never again */

  यदि(Curl_ssl->init)
    प्रत्यागम Curl_ssl->init();
  प्रत्यागम 1;
}

स्थिर bool ssl_prefs_check(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  यदि(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    प्रत्यागम FALSE;
  }

  विकल्प(conn_config->version_max) {
  पक्ष CURL_SSLVERSION_MAX_NONE:
  पक्ष CURL_SSLVERSION_MAX_DEFAULT:
    भङ्ग;

  स्वभाव:
    यदि((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      प्रत्यागम FALSE;
    }
  }

  प्रत्यागम TRUE;
}

स्थिर संरचना ssl_connect_data *cf_ctx_new(संरचना Curl_easy *data,
                                           ध्रुव संरचना alpn_spec *alpn)
{
  संरचना ssl_connect_data *ctx;

  (शून्य)data;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx)
    प्रत्यागम रिक्त;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  यदि(!ctx->backend) {
    curlx_free(ctx);
    प्रत्यागम रिक्त;
  }
  प्रत्यागम ctx;
}

स्थिर शून्य cf_ctx_free(संरचना ssl_connect_data *ctx)
{
  यदि(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(संरचना Curl_easy *data, int8_t sockindex,
                                      संरचना dynbuf *binding)
{
  यदि(Curl_ssl->get_channel_binding)
    प्रत्यागम Curl_ssl->get_channel_binding(data, sockindex, binding);
  प्रत्यागम CURLE_OK;
}

शून्य Curl_ssl_close_all(संरचना Curl_easy *data)
{
  यदि(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना easy_pollset *ps)
{
  संरचना ssl_connect_data *connssl = cf->ctx;

  यदि(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    यदि(sock != CURL_SOCKET_BAD) {
      यदि(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      अन्यथा {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    प्रत्यागम result;
  }
  प्रत्यागम CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
संरचना curl_slist *Curl_ssl_engines_list(संरचना Curl_easy *data)
{
  यदि(Curl_ssl->engines_list)
    प्रत्यागम Curl_ssl->engines_list(data);
  प्रत्यागम रिक्त;
}

स्थिर size_t multissl_version(वर्ण *buffer, size_t size);

शून्य Curl_ssl_version(वर्ण *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (शून्य)multissl_version(buffer, size);
#अन्यथा
  (शून्य)Curl_ssl->version(buffer, size);
#endif
}

शून्य Curl_ssl_free_certinfo(संरचना Curl_easy *data)
{
  संरचना curl_certinfo *ci = &data->info.certs;

  यदि(ci->अंक_of_certs) {
    /* free all individual lists used */
    पूर्णाङ्क i;
    प्रति(i = 0; i < ci->अंक_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = रिक्त;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->अंक_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(संरचना Curl_easy *data, पूर्णाङ्क अंक)
{
  संरचना curl_certinfo *ci = &data->info.certs;
  संरचना curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)अंक, परिमाण(संरचना curl_slist *));
  यदि(!table)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  ci->अंक_of_certs = अंक;
  ci->certinfo = table;

  प्रत्यागम CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ढकला_certinfo_len(संरचना Curl_easy *data,
                                    पूर्णाङ्क certnum,
                                    ध्रुव वर्ण *label,
                                    ध्रुव वर्ण *मूल्य,
                                    size_t valuelen)
{
  संरचना curl_certinfo *ci = &data->info.certs;
  संरचना curl_slist *nl;
  CURLcode result = CURLE_OK;
  संरचना dynbuf build;

  DEBUGASSERT(certnum < ci->अंक_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  यदि(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, मूल्य, valuelen))
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  यदि(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  प्रत्यागम result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(संरचना Curl_easy *data,
                         unsigned वर्ण *buffer, size_t length)
{
  DEBUGASSERT(length == परिमाण(पूर्णाङ्क));
  यदि(Curl_ssl->random)
    प्रत्यागम Curl_ssl->random(data, buffer, length);
  अन्यथा
    प्रत्यागम CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

स्थिर CURLcode pubkey_pem_to_der(ध्रुव वर्ण *pem,
                                  unsigned वर्ण **der, size_t *der_len)
{
  ध्रुव वर्ण *begin_pos, *end_pos;
  size_t pem_मोजणी, pem_len;
  CURLcode result;
  संरचना dynbuf pbuf;

  /* if no pem, exit. */
  यदि(!pem)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  यदि(!begin_pos)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  pem_मोजणी = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  यदि(pem_मोजणी && '\n' != pem[pem_मोजणी - 1])
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_मोजणी += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_मोजणी, "\n-----END PUBLIC KEY-----");
  यदि(!end_pos)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  यावत्(pem_मोजणी < pem_len) {
    यदि('\n' != pem[pem_मोजणी] && '\r' != pem[pem_मोजणी]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_मोजणी], 1);
      यदि(result)
        प्रत्यागम result;
    }
    ++pem_मोजणी;
  }

  यदि(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  अन्यथा
    result = CURLE_BAD_CONTENT_ENCODING;

  प्रत्यागम result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(संरचना Curl_easy *data,
                              ध्रुव वर्ण *pinnedpubkey,
                              ध्रुव unsigned वर्ण *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  यदि(!pinnedpubkey)
    प्रत्यागम CURLE_OK;
  यदि(!pubkey || !pubkeylen)
    प्रत्यागम result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  यदि(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    वर्ण *cert_hash = रिक्त;
    ध्रुव वर्ण *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned वर्ण *sha256sumdigest;

    यदि(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      प्रत्यागम result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    यदि(!sha256sumdigest)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    यदि(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    यदि(encode)
      प्रत्यागम encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    यावत्(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      यदि(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        भङ्ग;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (पूर्णाङ्क)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : रिक्त;
    }
    curlx_safefree(cert_hash);
  }
  अन्यथा {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    संरचना dynbuf buf;
    unsigned वर्ण *pem_ptr = रिक्त;
    size_t डावा;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    यदि(!fp)
      प्रत्यागम result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    यदि(fseek(fp, 0, SEEK_END))
      गच्छ end;
    filesize = ftell(fp);
    यदि(fseek(fp, 0, SEEK_SET))
      गच्छ end;
    यदि(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      गच्छ end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    यदि(pubkeylen > size)
      गच्छ end;

    /*
     * Read the file into the dynbuf
     */
    डावा = size;
    कुरु {
      वर्ण buffer[1024];
      size_t want = डावा > परिमाण(buffer) ? परिमाण(buffer) : डावा;
      यदि(fread(buffer, 1, want, fp) != want)
        गच्छ end;
      यदि(curlx_dyn_addn(&buf, buffer, want))
        गच्छ end;
      डावा -= want;
    } यावत्(डावा);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    यदि(pubkeylen == size) {
      यदि(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      गच्छ end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    यदि(pem_read)
      गच्छ end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    यदि(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  प्रत्यागम result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(शून्य)
{
  यदि(Curl_ssl->cert_status_request)
    प्रत्यागम Curl_ssl->cert_status_request();
  प्रत्यागम FALSE;
}

स्थिर पूर्णाङ्क multissl_init(शून्य)
{
  यदि(multissl_setup(रिक्त))
    प्रत्यागम 1;
  यदि(Curl_ssl->init)
    प्रत्यागम Curl_ssl->init();
  प्रत्यागम 1;
}

स्थिर CURLcode multissl_random(संरचना Curl_easy *data,
                                unsigned वर्ण *entropy, size_t length)
{
  यदि(multissl_setup(रिक्त))
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_ssl->random(data, entropy, length);
}

स्थिर CURLcode multissl_connect(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data, bool *done)
{
  यदि(multissl_setup(रिक्त))
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_ssl->do_connect(cf, data, done);
}

स्थिर CURLcode multissl_adjust_pollset(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना easy_pollset *ps)
{
  यदि(multissl_setup(रिक्त))
    प्रत्यागम CURLE_OK;
  प्रत्यागम Curl_ssl->adjust_pollset(cf, data, ps);
}

स्थिर शून्य *multissl_get_internals(संरचना ssl_connect_data *connssl,
                                    CURLINFO info)
{
  यदि(multissl_setup(रिक्त))
    प्रत्यागम रिक्त;
  प्रत्यागम Curl_ssl->get_internals(connssl, info);
}

स्थिर शून्य multissl_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  यदि(multissl_setup(रिक्त))
    प्रत्यागम;
  Curl_ssl->close(cf, data);
}

स्थिर CURLcode multissl_recv_plain(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    वर्ण *buf, size_t len, size_t *pnread)
{
  यदि(multissl_setup(रिक्त))
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

स्थिर CURLcode multissl_send_plain(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    ध्रुव शून्य *mem, size_t len,
                                    size_t *pnwritten)
{
  यदि(multissl_setup(रिक्त))
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

स्थिर ध्रुव संरचना Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  रिक्त,                              /* cleanup */
  multissl_version,                  /* version */
  रिक्त,                              /* shutdown */
  रिक्त,                              /* data_pending */
  multissl_random,                   /* random */
  रिक्त,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  रिक्त,                              /* close_all */
  रिक्त,                              /* engines_list */
  रिक्त,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  रिक्त,                              /* get_channel_binding */
};

ध्रुव संरचना Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#नोचेद्यदि defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#नोचेद्यदि defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#नोचेद्यदि defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#नोचेद्यदि defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#नोचेद्यदि defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#नोचेद्यदि defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#अन्यथा
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

स्थिर ध्रुव संरचना Curl_ssl *available_backends[] = {
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
  रिक्त
};

/* Global cleanup */
शून्य Curl_ssl_cleanup(शून्य)
{
  यदि(init_ssl) {
    /* only cleanup if we did a previous init */
    यदि(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

स्थिर size_t multissl_version(वर्ण *buffer, size_t size)
{
  स्थिर ध्रुव संरचना Curl_ssl *selected;
  स्थिर वर्ण backends[200];
  स्थिर size_t backends_len;
  ध्रुव संरचना Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  यदि(current != selected) {
    वर्ण *p = backends;
    ध्रुव वर्ण *end = backends + परिमाण(backends);
    पूर्णाङ्क i;

    selected = current;

    backends[0] = '\0';

    प्रति(i = 0; available_backends[i]; ++i) {
      वर्ण vb[200];
      bool paren = (selected != available_backends[i]);

      यदि(available_backends[i]->version(vb, परिमाण(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  यदि(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क multissl_setup(ध्रुव संरचना Curl_ssl *backend)
{
  पूर्णाङ्क i;
  वर्ण *env;

  यदि(Curl_ssl != &Curl_ssl_multi)
    प्रत्यागम 1;

  यदि(backend) {
    Curl_ssl = backend;
    प्रत्यागम 0;
  }

  यदि(!available_backends[0])
    प्रत्यागम 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  यदि(env) {
    प्रति(i = 0; available_backends[i]; i++) {
      यदि(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        प्रत्यागम 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  प्रति(i = 0; available_backends[i]; i++) {
    यदि(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      प्रत्यागम 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  प्रत्यागम 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ध्रुव वर्ण *name,
                                   ध्रुव curl_ssl_backend ***avail)
{
  पूर्णाङ्क i;

  यदि(avail)
    *avail = (ध्रुव curl_ssl_backend **)&available_backends;

  यदि(Curl_ssl != &Curl_ssl_multi)
    प्रत्यागम id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#अन्यथा
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  प्रति(i = 0; available_backends[i]; i++) {
    यदि(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      प्रत्यागम CURLSSLSET_OK;
    }
  }

  प्रत्यागम CURLSSLSET_UNKNOWN_BACKEND;
}

#अन्यथा /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ध्रुव वर्ण *name,
                                   ध्रुव curl_ssl_backend ***avail)
{
  (शून्य)id;
  (शून्य)name;
  (शून्य)avail;
  प्रत्यागम CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

शून्य Curl_ssl_peer_cleanup(संरचना ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_किल्ली);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

स्थिर ssl_peer_type get_peer_type(ध्रुव वर्ण *hostname)
{
  यदि(hostname && hostname[0]) {
#ifdef USE_IPV6
    संरचना in6_addr addr;
#अन्यथा
    संरचना in_addr addr;
#endif
    यदि(curlx_inet_pton(AF_INET, hostname, &addr))
      प्रत्यागम CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    अन्यथा यदि(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      प्रत्यागम CURL_SSL_PEER_IPV6;
    }
#endif
  }
  प्रत्यागम CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(संरचना ssl_peer *ssl_peer,
                            संरचना Curl_peer *origin,
                            संरचना Curl_peer *peer,
                            संरचना ssl_filter_config *sslc,
                            ध्रुव वर्ण *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  यदि(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    प्रत्यागम CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  यदि(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    यदि(len && (origin->hostname[len - 1] == '.'))
      len--;
    यदि(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      यदि(!ssl_peer->sni)
        गच्छ out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_किल्ली_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_किल्ली);

out:
  यदि(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  प्रत्यागम result;
}

स्थिर शून्य ssl_cf_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  यदि(connssl) {
    connssl->ssl_impl->close(cf, data);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = रिक्त;
  }
  cf->connected = FALSE;
}

स्थिर CURLcode ssl_cf_connect(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result;

  यदि(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }

  यदि(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    प्रत्यागम CURLE_FAILED_INIT;
  }

  यदि(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    यदि(result || !*done)
      प्रत्यागम result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  यदि(!connssl->prefs_checked) {
    यदि(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  यदि(!result && *done) {
    cf->connected = TRUE;
    यदि(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    यदि(Curl_tls_keylog_enabled()) {
      infof(data, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_संख्या
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (पूर्णाङ्क)result, *done);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode ssl_cf_set_earlydata(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data,
                                     ध्रुव शून्य *buf, size_t blen)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  यदि(blen) {
    यदि(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    यदि(result)
      प्रत्यागम result;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssl_cf_connect_deferred(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        ध्रुव शून्य *buf, size_t blen,
                                        bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  यदि(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    यदि(result)
      प्रत्यागम result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  यदि(!result && *done) {
    यदि(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    विकल्प(connssl->earlydata_state) {
    पक्ष ssl_earlydata_none:
      भङ्ग;
    पक्ष ssl_earlydata_accepted:
      यदि(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      भङ्ग;
    पक्ष ssl_earlydata_rejected:
      यदि(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      भङ्ग;
    स्वभाव:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(रिक्त);
      भङ्ग;
    }
  }
  प्रत्यागम result;
}

स्थिर bool ssl_cf_data_pending(संरचना Curl_cfilter *cf,
                                ध्रुव संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  यदि(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  अन्यथा
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम pending;
}

स्थिर CURLcode ssl_cf_send(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            ध्रुव uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result = CURLE_OK;

  (शून्य)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  यदि(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    यदि(result)
      गच्छ out;
    अन्यथा यदि(!done) {
      result = CURLE_AGAIN;
      गच्छ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  यदि(connssl->earlydata_skip) {
    यदि(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      गच्छ out;
    }
    अन्यथा {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  यदि(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    यदि(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode ssl_cf_recv(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data, वर्ण *buf, size_t len,
                            size_t *pnread)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  यदि(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, रिक्त, 0, &done);
    यदि(result)
      गच्छ out;
    अन्यथा यदि(!done) {
      result = CURLE_AGAIN;
      गच्छ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode ssl_cf_shutdown(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  यदि(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    संरचना cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (पूर्णाङ्क)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  प्रत्यागम result;
}

स्थिर CURLcode ssl_cf_adjust_pollset(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      संरचना easy_pollset *ps)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  प्रत्यागम result;
}

स्थिर CURLcode ssl_cf_query(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना ssl_connect_data *connssl = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_SSL_INFO:
  पक्ष CF_QUERY_SSL_CTX_INFO:
    यदि(!Curl_ssl_cf_is_proxy(cf)) {
      संरचना curl_tlssessioninfo *info = pres2;
      संरचना cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    प्रत्यागम CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode ssl_cf_cntrl(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना ssl_connect_data *connssl = cf->ctx;

  (शून्य)arg1;
  (शून्य)arg2;
  (शून्य)data;
  विकल्प(event) {
  पक्ष CF_CTRL_CONN_INFO_UPDATE:
    यदि(connssl->negotiated.alpn && !cf->sockindex) {
      यदि(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      अन्यथा यदि(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      अन्यथा यदि(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    भङ्ग;
  पक्ष CF_CTRL_REPORT_STATS:
    यदि(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर bool cf_ssl_is_alive(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                            bool *आदान_pending)
{
  /*
   * This function tries to determine connection status.
   */
  प्रत्यागम cf->next ?
    cf->next->cft->is_alive(cf->next, data, आदान_pending) :
    FALSE; /* pessimistic in absence of data */
}

संरचना Curl_cftype Curl_cft_ssl = {
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

संरचना Curl_cftype Curl_cft_ssl_proxy = {
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

स्थिर CURLcode cf_ssl_create(संरचना Curl_cfilter **pcf,
                              संरचना Curl_easy *data,
                              संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (शून्य)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, रिक्त);
#अन्यथा
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  यदि(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  यदि(result)
    cf_ctx_free(ctx);
  *pcf = result ? रिक्त : cf;
  प्रत्यागम result;
}

स्थिर CURLcode cf_ssl_peer_init(संरचना Curl_cfilter *cf,
                                 संरचना Curl_peer *origin,
                                 संरचना Curl_peer *peer,
                                 संरचना ssl_filter_config *sslc)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  वर्ण tls_id[80];
  connssl->ssl_impl->version(tls_id, परिमाण(tls_id) - 1);
  प्रत्यागम Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(संरचना Curl_easy *data,
                              संरचना Curl_peer *origin,
                              संरचना connectdata *conn,
                              int8_t sockindex)
{
  संरचना Curl_cfilter *cf;
  संरचना Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  यदि(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  यदि(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  अन्यथा यदि(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  प्रत्यागम result;
}

CURLcode Curl_cf_ssl_घाला_after(संरचना Curl_cfilter *cf_at,
                                  संरचना Curl_easy *data,
                                  संरचना Curl_peer *origin,
                                  संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  यदि(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  यदि(!result) {
    Curl_conn_cf_घाला_after(cf_at, cf);
#यदि defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    यदि((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  अन्यथा यदि(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  प्रत्यागम result;
}

#ifndef CURL_DISABLE_PROXY

स्थिर CURLcode cf_ssl_proxy_create(संरचना Curl_cfilter **pcf,
                                    संरचना Curl_easy *data,
                                    संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (शून्य)conn;
#ifdef USE_HTTP2
  यदि(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  यदि(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    गच्छ out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  यदि(result)
    cf_ctx_free(ctx);
  *pcf = result ? रिक्त : cf;
  प्रत्यागम result;
}

CURLcode Curl_cf_ssl_proxy_घाला_after(संरचना Curl_cfilter *cf_at,
                                        संरचना Curl_easy *data,
                                        संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  यदि(!result)
    result = cf_ssl_peer_init(cf, peer, रिक्त, &cf_at->conn->proxy_ssl_config);
  यदि(!result)
    Curl_conn_cf_घाला_after(cf_at, cf);
  अन्यथा यदि(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  प्रत्यागम result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(संरचना Curl_easy *data, unsigned पूर्णाङ्क ssl_option)
{
  (शून्य)data;
  प्रत्यागम (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(संरचना Curl_cfilter *cf)
{
  यदि(Curl_ssl->supports & SSLSUPP_ECH) {
    संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    प्रत्यागम !!conn_config->ech;
  }
  प्रत्यागम FALSE;
}

bool Curl_ssl_need_httpsrr(संरचना Curl_cfilter *cf)
{
  यदि(Curl_ssl->supports & SSLSUPP_ECH) {
    संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    यदि(!conn_config->ech)
      प्रत्यागम FALSE;
    यदि((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      प्रत्यागम FALSE;
    प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}
#endif

स्थिर CURLcode vtls_shutdown_blocking(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  पूर्णाङ्क what, loop = 10;

  यदि(cf->shutdown) {
    *done = TRUE;
    प्रत्यागम CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  यावत्(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    यदि(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      गच्छ out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    यदि(result || *done)
      गच्छ out;

    यदि(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      यदि(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        गच्छ out;
      }
      अन्यथा यदि(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        गच्छ out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  प्रत्यागम result;
}

CURLcode Curl_ssl_cfilter_remove(संरचना Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  संरचना Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : रिक्त;
  प्रति(cf = head; cf; cf = cf->next) {
    यदि(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      यदि(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (पूर्णाङ्क)result);
      भङ्ग;
    }
  }
  प्रत्यागम result;
}

bool Curl_ssl_cf_is_proxy(संरचना Curl_cfilter *cf)
{
  प्रत्यागम (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

संरचना ssl_easy_config *
Curl_ssl_cf_get_easy_config(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (शून्य)cf;
  प्रत्यागम &data->set.ssl;
#अन्यथा
  प्रत्यागम Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

संरचना ssl_filter_config *
Curl_ssl_cf_get_filter_config(संरचना Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  प्रत्यागम &cf->conn->ssl_config;
#अन्यथा
  प्रत्यागम Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(संरचना alpn_proto_buf *buf,
                                ध्रुव संरचना alpn_spec *spec)
{
  size_t i, len;
  पूर्णाङ्क off = 0;
  unsigned वर्ण blen;

  memset(buf, 0, परिमाण(*buf));
  प्रति(i = 0; spec && i < spec->मोजणी; ++i) {
    len = strlen(spec->entries[i]);
    यदि(len >= ALPN_NAME_MAX)
      प्रत्यागम CURLE_FAILED_INIT;
    blen = (unsigned वर्ण)len;
    यदि(off + blen + 1 >= (पूर्णाङ्क)परिमाण(buf->data))
      प्रत्यागम CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(संरचना alpn_proto_buf *buf,
                                ध्रुव संरचना alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, परिमाण(*buf));
  प्रति(i = 0; spec && i < spec->मोजणी; ++i) {
    len = strlen(spec->entries[i]);
    यदि(len >= ALPN_NAME_MAX)
      प्रत्यागम CURLE_FAILED_INIT;
    यदि(off + len + 2 >= परिमाण(buf->data))
      प्रत्यागम CURLE_FAILED_INIT;
    यदि(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (पूर्णाङ्क)off;
  प्रत्यागम CURLE_OK;
}

bool Curl_alpn_contains_proto(ध्रुव संरचना alpn_spec *spec,
                              ध्रुव वर्ण *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  प्रति(i = 0; spec && plen && i < spec->मोजणी; ++i) {
    size_t slen = strlen(spec->entries[i]);
    यदि((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

शून्य Curl_alpn_restrict_to(संरचना alpn_spec *spec, ध्रुव वर्ण *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < परिमाण(spec->entries[0]));
  यदि(plen < परिमाण(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->मोजणी = 1;
  }
}

शून्य Curl_alpn_copy(संरचना alpn_spec *dest, ध्रुव संरचना alpn_spec *src)
{
  यदि(src)
    memcpy(dest, src, परिमाण(*dest));
  अन्यथा
    memset(dest, 0, परिमाण(*dest));
}

CURLcode Curl_alpn_set_negotiated(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना ssl_connect_data *connssl,
                                  ध्रुव unsigned वर्ण *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (शून्य)cf;

  यदि(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    यदि(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    अन्यथा यदि(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    अन्यथा यदि((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (पूर्णाङ्क)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    गच्छ out;
  }

  यदि(proto && proto_len) {
    यदि(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      गच्छ out;
    }
    connssl->negotiated.alpn = curlx_memdup0((ध्रुव वर्ण *)proto, proto_len);
    यदि(!connssl->negotiated.alpn)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
  }

  यदि(proto && proto_len) {
    यदि(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (पूर्णाङ्क)proto_len, proto);
    अन्यथा
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (पूर्णाङ्क)proto_len, proto);
  }
  अन्यथा {
    यदि(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    अन्यथा
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  प्रत्यागम result;
}

CURLcode Curl_on_session_reuse(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *data,
                               संरचना alpn_spec *alpns,
                               संरचना Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  यदि(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  अन्यथा यदि(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  अन्यथा {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ध्रुव unsigned वर्ण *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  प्रत्यागम result;
}

संरचना Curl_ssl_session *Curl_ssl_get_cf_session(संरचना Curl_easy *data,
                                                 ध्रुव संरचना Curl_cftype *cft,
                                                 int8_t sockindex)
{
  यदि(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#अन्यथा
     (cft == &Curl_cft_ssl)) {
#endif
    संरचना Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    प्रति(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      यदि(cf1->cft == cft)
        भङ्ग;
    }
    यदि(cf1) {
      संरचना ssl_connect_data *connssl = cf1->ctx;
      यदि(connssl)
        प्रत्यागम connssl->session;
    }
  }
  प्रत्यागम रिक्त;
}

#endif /* USE_SSL */
