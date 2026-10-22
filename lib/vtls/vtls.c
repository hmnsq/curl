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

#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"

#অন্তর্ভাব "vtls/vtls.h" /* generic SSL protos etc */
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vtls/keylog.h"

#অন্তর্ভাব "vtls/openssl.h"        /* OpenSSL versions */
#অন্তর্ভাব "vtls/gtls.h"           /* GnuTLS versions */
#অন্তর্ভাব "vtls/wolfssl.h"        /* wolfSSL versions */
#অন্তর্ভাব "vtls/schannel.h"       /* Schannel SSPI version */
#অন্তর্ভাব "vtls/mbedtls.h"        /* mbedTLS versions */
#অন্তর্ভাব "vtls/rustls.h"         /* Rustls versions */

#অন্তর্ভাব "slist.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curl_sha256.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "setopt.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/strcopy.h"

#ifdef USE_APPLE_SECTRUST
#অন্তর্ভাব <Security/Security.h>
#endif

#ifdef USE_SSL
#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
স্থির ধ্রুব সংরচনা alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
স্থির ধ্রুব সংরচনা alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
স্থির ধ্রুব সংরচনা alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
স্থির ধ্রুব সংরচনা alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
স্থির ধ্রুব সংরচনা alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

স্থির ধ্রুব সংরচনা alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  যদি(!use_alpn)
    প্রত্যাগম রিক্ত;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  যদি(only_http_10 && (wanted & CURL_HTTP_V1x))
    প্রত্যাগম &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  যদি(wanted & CURL_HTTP_V2x) {
    যদি(wanted & CURL_HTTP_V1x)
      প্রত্যাগম (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    প্রত্যাগম &ALPN_SPEC_H2;
  }
#অন্যথা
  (শূন্য)wanted;
  (শূন্য)preferred;
#endif
  প্রত্যাগম &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
স্থির পূর্ণাঙ্ক multissl_setup(ধ্রুব সংরচনা Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(শূন্য)
{
#ifdef USE_SSL
  multissl_setup(রিক্ত);
  প্রত্যাগম Curl_ssl->info.id;
#অন্যথা
  প্রত্যাগম CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
স্থির bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
পূর্ণাঙ্ক Curl_ssl_init(শূন্য)
{
  /* make sure this is only done once */
  যদি(init_ssl)
    প্রত্যাগম 1;
  init_ssl = TRUE; /* never again */

  যদি(Curl_ssl->init)
    প্রত্যাগম Curl_ssl->init();
  প্রত্যাগম 1;
}

স্থির bool ssl_prefs_check(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data)
{
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  যদি(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    প্রত্যাগম FALSE;
  }

  বিকল্প(conn_config->version_max) {
  পক্ষ CURL_SSLVERSION_MAX_NONE:
  পক্ষ CURL_SSLVERSION_MAX_DEFAULT:
    ভঙ্গ;

  স্বভাব:
    যদি((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      প্রত্যাগম FALSE;
    }
  }

  প্রত্যাগম TRUE;
}

স্থির সংরচনা ssl_connect_data *cf_ctx_new(সংরচনা Curl_easy *data,
                                           ধ্রুব সংরচনা alpn_spec *alpn)
{
  সংরচনা ssl_connect_data *ctx;

  (শূন্য)data;
  ctx = curlx_calloc(1, পরিমাণ(*ctx));
  যদি(!ctx)
    প্রত্যাগম রিক্ত;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  যদি(!ctx->backend) {
    curlx_free(ctx);
    প্রত্যাগম রিক্ত;
  }
  প্রত্যাগম ctx;
}

স্থির শূন্য cf_ctx_free(সংরচনা ssl_connect_data *ctx)
{
  যদি(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(সংরচনা Curl_easy *data, int8_t sockindex,
                                      সংরচনা dynbuf *binding)
{
  যদি(Curl_ssl->get_channel_binding)
    প্রত্যাগম Curl_ssl->get_channel_binding(data, sockindex, binding);
  প্রত্যাগম CURLE_OK;
}

শূন্য Curl_ssl_close_all(সংরচনা Curl_easy *data)
{
  যদি(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 সংরচনা easy_pollset *ps)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;

  যদি(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    যদি(sock != CURL_SOCKET_BAD) {
      যদি(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      অন্যথা {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    প্রত্যাগম result;
  }
  প্রত্যাগম CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
সংরচনা curl_slist *Curl_ssl_engines_list(সংরচনা Curl_easy *data)
{
  যদি(Curl_ssl->engines_list)
    প্রত্যাগম Curl_ssl->engines_list(data);
  প্রত্যাগম রিক্ত;
}

স্থির size_t multissl_version(বর্ণ *buffer, size_t size);

শূন্য Curl_ssl_version(বর্ণ *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (শূন্য)multissl_version(buffer, size);
#অন্যথা
  (শূন্য)Curl_ssl->version(buffer, size);
#endif
}

শূন্য Curl_ssl_free_certinfo(সংরচনা Curl_easy *data)
{
  সংরচনা curl_certinfo *ci = &data->info.certs;

  যদি(ci->অঙ্ক_of_certs) {
    /* free all individual lists used */
    পূর্ণাঙ্ক i;
    প্রতি(i = 0; i < ci->অঙ্ক_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = রিক্ত;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->অঙ্ক_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(সংরচনা Curl_easy *data, পূর্ণাঙ্ক অঙ্ক)
{
  সংরচনা curl_certinfo *ci = &data->info.certs;
  সংরচনা curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)অঙ্ক, পরিমাণ(সংরচনা curl_slist *));
  যদি(!table)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  ci->অঙ্ক_of_certs = অঙ্ক;
  ci->certinfo = table;

  প্রত্যাগম CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ঠেলো_certinfo_len(সংরচনা Curl_easy *data,
                                    পূর্ণাঙ্ক certnum,
                                    ধ্রুব বর্ণ *label,
                                    ধ্রুব বর্ণ *মান,
                                    size_t valuelen)
{
  সংরচনা curl_certinfo *ci = &data->info.certs;
  সংরচনা curl_slist *nl;
  CURLcode result = CURLE_OK;
  সংরচনা dynbuf build;

  DEBUGASSERT(certnum < ci->অঙ্ক_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  যদি(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, মান, valuelen))
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  যদি(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  প্রত্যাগম result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(সংরচনা Curl_easy *data,
                         unsigned বর্ণ *buffer, size_t length)
{
  DEBUGASSERT(length == পরিমাণ(পূর্ণাঙ্ক));
  যদি(Curl_ssl->random)
    প্রত্যাগম Curl_ssl->random(data, buffer, length);
  অন্যথা
    প্রত্যাগম CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

স্থির CURLcode pubkey_pem_to_der(ধ্রুব বর্ণ *pem,
                                  unsigned বর্ণ **der, size_t *der_len)
{
  ধ্রুব বর্ণ *begin_pos, *end_pos;
  size_t pem_গুনতি, pem_len;
  CURLcode result;
  সংরচনা dynbuf pbuf;

  /* if no pem, exit. */
  যদি(!pem)
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  যদি(!begin_pos)
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;

  pem_গুনতি = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  যদি(pem_গুনতি && '\n' != pem[pem_গুনতি - 1])
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_গুনতি += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_গুনতি, "\n-----END PUBLIC KEY-----");
  যদি(!end_pos)
    প্রত্যাগম CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  যাবত্(pem_গুনতি < pem_len) {
    যদি('\n' != pem[pem_গুনতি] && '\r' != pem[pem_গুনতি]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_গুনতি], 1);
      যদি(result)
        প্রত্যাগম result;
    }
    ++pem_গুনতি;
  }

  যদি(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  অন্যথা
    result = CURLE_BAD_CONTENT_ENCODING;

  প্রত্যাগম result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *pinnedpubkey,
                              ধ্রুব unsigned বর্ণ *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  যদি(!pinnedpubkey)
    প্রত্যাগম CURLE_OK;
  যদি(!pubkey || !pubkeylen)
    প্রত্যাগম result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  যদি(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    বর্ণ *cert_hash = রিক্ত;
    ধ্রুব বর্ণ *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned বর্ণ *sha256sumdigest;

    যদি(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      প্রত্যাগম result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    যদি(!sha256sumdigest)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    যদি(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    যদি(encode)
      প্রত্যাগম encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    যাবত্(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      যদি(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        ভঙ্গ;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (পূর্ণাঙ্ক)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : রিক্ত;
    }
    curlx_safefree(cert_hash);
  }
  অন্যথা {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    সংরচনা dynbuf buf;
    unsigned বর্ণ *pem_ptr = রিক্ত;
    size_t বাম;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    যদি(!fp)
      প্রত্যাগম result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    যদি(fseek(fp, 0, SEEK_END))
      গচ্ছ end;
    filesize = ftell(fp);
    যদি(fseek(fp, 0, SEEK_SET))
      গচ্ছ end;
    যদি(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      গচ্ছ end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    যদি(pubkeylen > size)
      গচ্ছ end;

    /*
     * Read the file into the dynbuf
     */
    বাম = size;
    কুরু {
      বর্ণ buffer[1024];
      size_t want = বাম > পরিমাণ(buffer) ? পরিমাণ(buffer) : বাম;
      যদি(fread(buffer, 1, want, fp) != want)
        গচ্ছ end;
      যদি(curlx_dyn_addn(&buf, buffer, want))
        গচ্ছ end;
      বাম -= want;
    } যাবত্(বাম);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    যদি(pubkeylen == size) {
      যদি(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      গচ্ছ end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    যদি(pem_read)
      গচ্ছ end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    যদি(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  প্রত্যাগম result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(শূন্য)
{
  যদি(Curl_ssl->cert_status_request)
    প্রত্যাগম Curl_ssl->cert_status_request();
  প্রত্যাগম FALSE;
}

স্থির পূর্ণাঙ্ক multissl_init(শূন্য)
{
  যদি(multissl_setup(রিক্ত))
    প্রত্যাগম 1;
  যদি(Curl_ssl->init)
    প্রত্যাগম Curl_ssl->init();
  প্রত্যাগম 1;
}

স্থির CURLcode multissl_random(সংরচনা Curl_easy *data,
                                unsigned বর্ণ *entropy, size_t length)
{
  যদি(multissl_setup(রিক্ত))
    প্রত্যাগম CURLE_FAILED_INIT;
  প্রত্যাগম Curl_ssl->random(data, entropy, length);
}

স্থির CURLcode multissl_connect(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data, bool *done)
{
  যদি(multissl_setup(রিক্ত))
    প্রত্যাগম CURLE_FAILED_INIT;
  প্রত্যাগম Curl_ssl->do_connect(cf, data, done);
}

স্থির CURLcode multissl_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        সংরচনা easy_pollset *ps)
{
  যদি(multissl_setup(রিক্ত))
    প্রত্যাগম CURLE_OK;
  প্রত্যাগম Curl_ssl->adjust_pollset(cf, data, ps);
}

স্থির শূন্য *multissl_get_internals(সংরচনা ssl_connect_data *connssl,
                                    CURLINFO info)
{
  যদি(multissl_setup(রিক্ত))
    প্রত্যাগম রিক্ত;
  প্রত্যাগম Curl_ssl->get_internals(connssl, info);
}

স্থির শূন্য multissl_close(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  যদি(multissl_setup(রিক্ত))
    প্রত্যাগম;
  Curl_ssl->close(cf, data);
}

স্থির CURLcode multissl_recv_plain(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    বর্ণ *buf, size_t len, size_t *pnread)
{
  যদি(multissl_setup(রিক্ত))
    প্রত্যাগম CURLE_FAILED_INIT;
  প্রত্যাগম Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

স্থির CURLcode multissl_send_plain(সংরচনা Curl_cfilter *cf,
                                    সংরচনা Curl_easy *data,
                                    ধ্রুব শূন্য *mem, size_t len,
                                    size_t *pnwritten)
{
  যদি(multissl_setup(রিক্ত))
    প্রত্যাগম CURLE_FAILED_INIT;
  প্রত্যাগম Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

স্থির ধ্রুব সংরচনা Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  রিক্ত,                              /* cleanup */
  multissl_version,                  /* version */
  রিক্ত,                              /* shutdown */
  রিক্ত,                              /* data_pending */
  multissl_random,                   /* random */
  রিক্ত,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  রিক্ত,                              /* close_all */
  রিক্ত,                              /* engines_list */
  রিক্ত,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  রিক্ত,                              /* get_channel_binding */
};

ধ্রুব সংরচনা Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#নোচেদ্যদি defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#নোচেদ্যদি defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#নোচেদ্যদি defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#নোচেদ্যদি defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#নোচেদ্যদি defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#নোচেদ্যদি defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#অন্যথা
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

স্থির ধ্রুব সংরচনা Curl_ssl *available_backends[] = {
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
  রিক্ত
};

/* Global cleanup */
শূন্য Curl_ssl_cleanup(শূন্য)
{
  যদি(init_ssl) {
    /* only cleanup if we did a previous init */
    যদি(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

স্থির size_t multissl_version(বর্ণ *buffer, size_t size)
{
  স্থির ধ্রুব সংরচনা Curl_ssl *selected;
  স্থির বর্ণ backends[200];
  স্থির size_t backends_len;
  ধ্রুব সংরচনা Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  যদি(current != selected) {
    বর্ণ *p = backends;
    ধ্রুব বর্ণ *end = backends + পরিমাণ(backends);
    পূর্ণাঙ্ক i;

    selected = current;

    backends[0] = '\0';

    প্রতি(i = 0; available_backends[i]; ++i) {
      বর্ণ vb[200];
      bool paren = (selected != available_backends[i]);

      যদি(available_backends[i]->version(vb, পরিমাণ(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  যদি(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক multissl_setup(ধ্রুব সংরচনা Curl_ssl *backend)
{
  পূর্ণাঙ্ক i;
  বর্ণ *env;

  যদি(Curl_ssl != &Curl_ssl_multi)
    প্রত্যাগম 1;

  যদি(backend) {
    Curl_ssl = backend;
    প্রত্যাগম 0;
  }

  যদি(!available_backends[0])
    প্রত্যাগম 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  যদি(env) {
    প্রতি(i = 0; available_backends[i]; i++) {
      যদি(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        প্রত্যাগম 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  প্রতি(i = 0; available_backends[i]; i++) {
    যদি(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      প্রত্যাগম 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  প্রত্যাগম 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ধ্রুব বর্ণ *name,
                                   ধ্রুব curl_ssl_backend ***avail)
{
  পূর্ণাঙ্ক i;

  যদি(avail)
    *avail = (ধ্রুব curl_ssl_backend **)&available_backends;

  যদি(Curl_ssl != &Curl_ssl_multi)
    প্রত্যাগম id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#অন্যথা
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  প্রতি(i = 0; available_backends[i]; i++) {
    যদি(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      প্রত্যাগম CURLSSLSET_OK;
    }
  }

  প্রত্যাগম CURLSSLSET_UNKNOWN_BACKEND;
}

#অন্যথা /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ধ্রুব বর্ণ *name,
                                   ধ্রুব curl_ssl_backend ***avail)
{
  (শূন্য)id;
  (শূন্য)name;
  (শূন্য)avail;
  প্রত্যাগম CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

শূন্য Curl_ssl_peer_cleanup(সংরচনা ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_চাবি);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

স্থির ssl_peer_type get_peer_type(ধ্রুব বর্ণ *hostname)
{
  যদি(hostname && hostname[0]) {
#ifdef USE_IPV6
    সংরচনা in6_addr addr;
#অন্যথা
    সংরচনা in_addr addr;
#endif
    যদি(curlx_inet_pton(AF_INET, hostname, &addr))
      প্রত্যাগম CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    অন্যথা যদি(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      প্রত্যাগম CURL_SSL_PEER_IPV6;
    }
#endif
  }
  প্রত্যাগম CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(সংরচনা ssl_peer *ssl_peer,
                            সংরচনা Curl_peer *origin,
                            সংরচনা Curl_peer *peer,
                            সংরচনা ssl_filter_config *sslc,
                            ধ্রুব বর্ণ *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  যদি(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    প্রত্যাগম CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  যদি(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    যদি(len && (origin->hostname[len - 1] == '.'))
      len--;
    যদি(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      যদি(!ssl_peer->sni)
        গচ্ছ out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_চাবি_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_চাবি);

out:
  যদি(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  প্রত্যাগম result;
}

স্থির শূন্য ssl_cf_destroy(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  যদি(connssl) {
    connssl->ssl_impl->close(cf, data);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = রিক্ত;
  }
  cf->connected = FALSE;
}

স্থির CURLcode ssl_cf_connect(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result;

  যদি(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }

  যদি(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    প্রত্যাগম CURLE_FAILED_INIT;
  }

  যদি(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    যদি(result || !*done)
      প্রত্যাগম result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  যদি(!connssl->prefs_checked) {
    যদি(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  যদি(!result && *done) {
    cf->connected = TRUE;
    যদি(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    যদি(Curl_tls_keylog_enabled()) {
      infof(data, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_সংখ্যা
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (পূর্ণাঙ্ক)result, *done);
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির CURLcode ssl_cf_set_earlydata(সংরচনা Curl_cfilter *cf,
                                     সংরচনা Curl_easy *data,
                                     ধ্রুব শূন্য *buf, size_t blen)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  যদি(blen) {
    যদি(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    যদি(result)
      প্রত্যাগম result;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode ssl_cf_connect_deferred(সংরচনা Curl_cfilter *cf,
                                        সংরচনা Curl_easy *data,
                                        ধ্রুব শূন্য *buf, size_t blen,
                                        bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  যদি(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    যদি(result)
      প্রত্যাগম result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  যদি(!result && *done) {
    যদি(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    বিকল্প(connssl->earlydata_state) {
    পক্ষ ssl_earlydata_none:
      ভঙ্গ;
    পক্ষ ssl_earlydata_accepted:
      যদি(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      ভঙ্গ;
    পক্ষ ssl_earlydata_rejected:
      যদি(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      ভঙ্গ;
    স্বভাব:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(রিক্ত);
      ভঙ্গ;
    }
  }
  প্রত্যাগম result;
}

স্থির bool ssl_cf_data_pending(সংরচনা Curl_cfilter *cf,
                                ধ্রুব সংরচনা Curl_easy *data)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  যদি(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  অন্যথা
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম pending;
}

স্থির CURLcode ssl_cf_send(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            ধ্রুব uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result = CURLE_OK;

  (শূন্য)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  যদি(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    যদি(result)
      গচ্ছ out;
    অন্যথা যদি(!done) {
      result = CURLE_AGAIN;
      গচ্ছ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  যদি(connssl->earlydata_skip) {
    যদি(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      গচ্ছ out;
    }
    অন্যথা {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  যদি(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    যদি(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির CURLcode ssl_cf_recv(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data, বর্ণ *buf, size_t len,
                            size_t *pnread)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  যদি(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, রিক্ত, 0, &done);
    যদি(result)
      গচ্ছ out;
    অন্যথা যদি(!done) {
      result = CURLE_AGAIN;
      গচ্ছ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির CURLcode ssl_cf_shutdown(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  যদি(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    সংরচনা cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (পূর্ণাঙ্ক)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  প্রত্যাগম result;
}

স্থির CURLcode ssl_cf_adjust_pollset(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      সংরচনা easy_pollset *ps)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  প্রত্যাগম result;
}

স্থির CURLcode ssl_cf_query(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             পূর্ণাঙ্ক query, পূর্ণাঙ্ক *pres1, শূন্য *pres2)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;

  বিকল্প(query) {
  পক্ষ CF_QUERY_SSL_INFO:
  পক্ষ CF_QUERY_SSL_CTX_INFO:
    যদি(!Curl_ssl_cf_is_proxy(cf)) {
      সংরচনা curl_tlssessioninfo *info = pres2;
      সংরচনা cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      প্রত্যাগম CURLE_OK;
    }
    ভঙ্গ;
  পক্ষ CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুব বর্ণ **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    প্রত্যাগম CURLE_OK;
  }
  স্বভাব:
    ভঙ্গ;
  }
  প্রত্যাগম cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode ssl_cf_cntrl(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             পূর্ণাঙ্ক event, পূর্ণাঙ্ক arg1, শূন্য *arg2)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;

  (শূন্য)arg1;
  (শূন্য)arg2;
  (শূন্য)data;
  বিকল্প(event) {
  পক্ষ CF_CTRL_CONN_INFO_UPDATE:
    যদি(connssl->negotiated.alpn && !cf->sockindex) {
      যদি(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      অন্যথা যদি(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      অন্যথা যদি(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    ভঙ্গ;
  পক্ষ CF_CTRL_REPORT_STATS:
    যদি(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    ভঙ্গ;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির bool cf_ssl_is_alive(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data,
                            bool *প্রবেশ_pending)
{
  /*
   * This function tries to determine connection status.
   */
  প্রত্যাগম cf->next ?
    cf->next->cft->is_alive(cf->next, data, প্রবেশ_pending) :
    FALSE; /* pessimistic in absence of data */
}

সংরচনা Curl_cftype Curl_cft_ssl = {
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

সংরচনা Curl_cftype Curl_cft_ssl_proxy = {
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

স্থির CURLcode cf_ssl_create(সংরচনা Curl_cfilter **pcf,
                              সংরচনা Curl_easy *data,
                              সংরচনা connectdata *conn)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (শূন্য)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, রিক্ত);
#অন্যথা
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  যদি(result)
    cf_ctx_free(ctx);
  *pcf = result ? রিক্ত : cf;
  প্রত্যাগম result;
}

স্থির CURLcode cf_ssl_peer_init(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_peer *origin,
                                 সংরচনা Curl_peer *peer,
                                 সংরচনা ssl_filter_config *sslc)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  বর্ণ tls_id[80];
  connssl->ssl_impl->version(tls_id, পরিমাণ(tls_id) - 1);
  প্রত্যাগম Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(সংরচনা Curl_easy *data,
                              সংরচনা Curl_peer *origin,
                              সংরচনা connectdata *conn,
                              int8_t sockindex)
{
  সংরচনা Curl_cfilter *cf;
  সংরচনা Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  যদি(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  যদি(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  অন্যথা যদি(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  প্রত্যাগম result;
}

CURLcode Curl_cf_ssl_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা Curl_peer *origin,
                                  সংরচনা Curl_peer *peer)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  যদি(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  যদি(!result) {
    Curl_conn_cf_ঢোকাও_after(cf_at, cf);
#যদি defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    যদি((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  অন্যথা যদি(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  প্রত্যাগম result;
}

#ifndef CURL_DISABLE_PROXY

স্থির CURLcode cf_ssl_proxy_create(সংরচনা Curl_cfilter **pcf,
                                    সংরচনা Curl_easy *data,
                                    সংরচনা connectdata *conn)
{
  সংরচনা Curl_cfilter *cf = রিক্ত;
  সংরচনা ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (শূন্য)conn;
#ifdef USE_HTTP2
  যদি(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  যদি(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    গচ্ছ out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  যদি(result)
    cf_ctx_free(ctx);
  *pcf = result ? রিক্ত : cf;
  প্রত্যাগম result;
}

CURLcode Curl_cf_ssl_proxy_ঢোকাও_after(সংরচনা Curl_cfilter *cf_at,
                                        সংরচনা Curl_easy *data,
                                        সংরচনা Curl_peer *peer)
{
  সংরচনা Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  যদি(!result)
    result = cf_ssl_peer_init(cf, peer, রিক্ত, &cf_at->conn->proxy_ssl_config);
  যদি(!result)
    Curl_conn_cf_ঢোকাও_after(cf_at, cf);
  অন্যথা যদি(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  প্রত্যাগম result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(সংরচনা Curl_easy *data, unsigned পূর্ণাঙ্ক ssl_option)
{
  (শূন্য)data;
  প্রত্যাগম (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(সংরচনা Curl_cfilter *cf)
{
  যদি(Curl_ssl->supports & SSLSUPP_ECH) {
    সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    প্রত্যাগম !!conn_config->ech;
  }
  প্রত্যাগম FALSE;
}

bool Curl_ssl_need_httpsrr(সংরচনা Curl_cfilter *cf)
{
  যদি(Curl_ssl->supports & SSLSUPP_ECH) {
    সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    যদি(!conn_config->ech)
      প্রত্যাগম FALSE;
    যদি((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      প্রত্যাগম FALSE;
    প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}
#endif

স্থির CURLcode vtls_shutdown_blocking(সংরচনা Curl_cfilter *cf,
                                       সংরচনা Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  সংরচনা cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  পূর্ণাঙ্ক what, loop = 10;

  যদি(cf->shutdown) {
    *done = TRUE;
    প্রত্যাগম CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  যাবত্(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    যদি(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      গচ্ছ out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    যদি(result || *done)
      গচ্ছ out;

    যদি(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      যদি(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        গচ্ছ out;
      }
      অন্যথা যদি(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        গচ্ছ out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  প্রত্যাগম result;
}

CURLcode Curl_ssl_cfilter_remove(সংরচনা Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  সংরচনা Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : রিক্ত;
  প্রতি(cf = head; cf; cf = cf->next) {
    যদি(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      যদি(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (পূর্ণাঙ্ক)result);
      ভঙ্গ;
    }
  }
  প্রত্যাগম result;
}

bool Curl_ssl_cf_is_proxy(সংরচনা Curl_cfilter *cf)
{
  প্রত্যাগম (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

সংরচনা ssl_easy_config *
Curl_ssl_cf_get_easy_config(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (শূন্য)cf;
  প্রত্যাগম &data->set.ssl;
#অন্যথা
  প্রত্যাগম Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

সংরচনা ssl_filter_config *
Curl_ssl_cf_get_filter_config(সংরচনা Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  প্রত্যাগম &cf->conn->ssl_config;
#অন্যথা
  প্রত্যাগম Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(সংরচনা alpn_proto_buf *buf,
                                ধ্রুব সংরচনা alpn_spec *spec)
{
  size_t i, len;
  পূর্ণাঙ্ক off = 0;
  unsigned বর্ণ blen;

  memset(buf, 0, পরিমাণ(*buf));
  প্রতি(i = 0; spec && i < spec->গুনতি; ++i) {
    len = strlen(spec->entries[i]);
    যদি(len >= ALPN_NAME_MAX)
      প্রত্যাগম CURLE_FAILED_INIT;
    blen = (unsigned বর্ণ)len;
    যদি(off + blen + 1 >= (পূর্ণাঙ্ক)পরিমাণ(buf->data))
      প্রত্যাগম CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(সংরচনা alpn_proto_buf *buf,
                                ধ্রুব সংরচনা alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, পরিমাণ(*buf));
  প্রতি(i = 0; spec && i < spec->গুনতি; ++i) {
    len = strlen(spec->entries[i]);
    যদি(len >= ALPN_NAME_MAX)
      প্রত্যাগম CURLE_FAILED_INIT;
    যদি(off + len + 2 >= পরিমাণ(buf->data))
      প্রত্যাগম CURLE_FAILED_INIT;
    যদি(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (পূর্ণাঙ্ক)off;
  প্রত্যাগম CURLE_OK;
}

bool Curl_alpn_contains_proto(ধ্রুব সংরচনা alpn_spec *spec,
                              ধ্রুব বর্ণ *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  প্রতি(i = 0; spec && plen && i < spec->গুনতি; ++i) {
    size_t slen = strlen(spec->entries[i]);
    যদি((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      প্রত্যাগম TRUE;
  }
  প্রত্যাগম FALSE;
}

শূন্য Curl_alpn_restrict_to(সংরচনা alpn_spec *spec, ধ্রুব বর্ণ *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < পরিমাণ(spec->entries[0]));
  যদি(plen < পরিমাণ(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->গুনতি = 1;
  }
}

শূন্য Curl_alpn_copy(সংরচনা alpn_spec *dest, ধ্রুব সংরচনা alpn_spec *src)
{
  যদি(src)
    memcpy(dest, src, পরিমাণ(*dest));
  অন্যথা
    memset(dest, 0, পরিমাণ(*dest));
}

CURLcode Curl_alpn_set_negotiated(সংরচনা Curl_cfilter *cf,
                                  সংরচনা Curl_easy *data,
                                  সংরচনা ssl_connect_data *connssl,
                                  ধ্রুব unsigned বর্ণ *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (শূন্য)cf;

  যদি(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    যদি(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ out;
    }
    অন্যথা যদি(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ out;
    }
    অন্যথা যদি((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (পূর্ণাঙ্ক)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    গচ্ছ out;
  }

  যদি(proto && proto_len) {
    যদি(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      গচ্ছ out;
    }
    connssl->negotiated.alpn = curlx_memdup0((ধ্রুব বর্ণ *)proto, proto_len);
    যদি(!connssl->negotiated.alpn)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  যদি(proto && proto_len) {
    যদি(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (পূর্ণাঙ্ক)proto_len, proto);
    অন্যথা
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (পূর্ণাঙ্ক)proto_len, proto);
  }
  অন্যথা {
    যদি(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    অন্যথা
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  প্রত্যাগম result;
}

CURLcode Curl_on_session_reuse(সংরচনা Curl_cfilter *cf,
                               সংরচনা Curl_easy *data,
                               সংরচনা alpn_spec *alpns,
                               সংরচনা Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  সংরচনা ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  যদি(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  অন্যথা যদি(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  অন্যথা {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ধ্রুব unsigned বর্ণ *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  প্রত্যাগম result;
}

সংরচনা Curl_ssl_session *Curl_ssl_get_cf_session(সংরচনা Curl_easy *data,
                                                 ধ্রুব সংরচনা Curl_cftype *cft,
                                                 int8_t sockindex)
{
  যদি(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#অন্যথা
     (cft == &Curl_cft_ssl)) {
#endif
    সংরচনা Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    প্রতি(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      যদি(cf1->cft == cft)
        ভঙ্গ;
    }
    যদি(cf1) {
      সংরচনা ssl_connect_data *connssl = cf1->ctx;
      যদি(connssl)
        প্রত্যাগম connssl->session;
    }
  }
  প্রত্যাগম রিক্ত;
}

#endif /* USE_SSL */
