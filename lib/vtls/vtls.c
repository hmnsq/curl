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

#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"

#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h" /* generic SSL protos etc */
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_int.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "vtls/keylog.h"

#ଅନ୍ତର୍ଭାଵ "vtls/openssl.h"        /* OpenSSL versions */
#ଅନ୍ତର୍ଭାଵ "vtls/gtls.h"           /* GnuTLS versions */
#ଅନ୍ତର୍ଭାଵ "vtls/wolfssl.h"        /* wolfSSL versions */
#ଅନ୍ତର୍ଭାଵ "vtls/schannel.h"       /* Schannel SSPI version */
#ଅନ୍ତର୍ଭାଵ "vtls/mbedtls.h"        /* mbedTLS versions */
#ଅନ୍ତର୍ଭାଵ "vtls/rustls.h"         /* Rustls versions */

#ଅନ୍ତର୍ଭାଵ "slist.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "curlx/win32-fopen.h"
#ଅନ୍ତର୍ଭାଵ "curl_sha256.h"
#ଅନ୍ତର୍ଭାଵ "curlx/base64.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "setopt.h"
#ଅନ୍ତର୍ଭାଵ "vdns/cf-dns.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

#ifdef USE_APPLE_SECTRUST
#ଅନ୍ତର୍ଭାଵ <Security/Security.h>
#endif

#ifdef USE_SSL
#ଯଦି !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
ସ୍ଥିର ଅଚଳ ସଂରଚନା alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
ସ୍ଥିର ଅଚଳ ସଂରଚନା alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
ସ୍ଥିର ଅଚଳ ସଂରଚନା alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
ସ୍ଥିର ଅଚଳ ସଂରଚନା alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
ସ୍ଥିର ଅଚଳ ସଂରଚନା alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

ସ୍ଥିର ଅଚଳ ସଂରଚନା alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  ଯଦି(!use_alpn)
    ଫେରନ୍ତୁ ଖାଲି;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  ଯଦି(only_http_10 && (wanted & CURL_HTTP_V1x))
    ଫେରନ୍ତୁ &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  ଯଦି(wanted & CURL_HTTP_V2x) {
    ଯଦି(wanted & CURL_HTTP_V1x)
      ଫେରନ୍ତୁ (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    ଫେରନ୍ତୁ &ALPN_SPEC_H2;
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)wanted;
  (ଶୂନ୍ୟ)preferred;
#endif
  ଫେରନ୍ତୁ &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା multissl_setup(ଅଚଳ ସଂରଚନା Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(ଶୂନ୍ୟ)
{
#ifdef USE_SSL
  multissl_setup(ଖାଲି);
  ଫେରନ୍ତୁ Curl_ssl->info.id;
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
ସ୍ଥିର bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_ssl_init(ଶୂନ୍ୟ)
{
  /* make sure this is only done once */
  ଯଦି(init_ssl)
    ଫେରନ୍ତୁ 1;
  init_ssl = TRUE; /* never again */

  ଯଦି(Curl_ssl->init)
    ଫେରନ୍ତୁ Curl_ssl->init();
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର bool ssl_prefs_check(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  ଯଦି(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    ଫେରନ୍ତୁ FALSE;
  }

  ବାଛ(conn_config->version_max) {
  ସ୍ଥିତି CURL_SSLVERSION_MAX_NONE:
  ସ୍ଥିତି CURL_SSLVERSION_MAX_DEFAULT:
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      ଫେରନ୍ତୁ FALSE;
    }
  }

  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର ସଂରଚନା ssl_connect_data *cf_ctx_new(ସଂରଚନା Curl_easy *data,
                                           ଅଚଳ ସଂରଚନା alpn_spec *alpn)
{
  ସଂରଚନା ssl_connect_data *ctx;

  (ଶୂନ୍ୟ)data;
  ctx = curlx_calloc(1, ଆକାର(*ctx));
  ଯଦି(!ctx)
    ଫେରନ୍ତୁ ଖାଲି;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  ଯଦି(!ctx->backend) {
    curlx_free(ctx);
    ଫେରନ୍ତୁ ଖାଲି;
  }
  ଫେରନ୍ତୁ ctx;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ctx_free(ସଂରଚନା ssl_connect_data *ctx)
{
  ଯଦି(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(ସଂରଚନା Curl_easy *data, int8_t sockindex,
                                      ସଂରଚନା dynbuf *binding)
{
  ଯଦି(Curl_ssl->get_channel_binding)
    ଫେରନ୍ତୁ Curl_ssl->get_channel_binding(data, sockindex, binding);
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_ssl_close_all(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;

  ଯଦି(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    ଯଦି(sock != CURL_SOCKET_BAD) {
      ଯଦି(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      ଅନ୍ୟଥା {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
ସଂରଚନା curl_slist *Curl_ssl_engines_list(ସଂରଚନା Curl_easy *data)
{
  ଯଦି(Curl_ssl->engines_list)
    ଫେରନ୍ତୁ Curl_ssl->engines_list(data);
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର size_t multissl_version(ଅକ୍ଷର *buffer, size_t size);

ଶୂନ୍ୟ Curl_ssl_version(ଅକ୍ଷର *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (ଶୂନ୍ୟ)multissl_version(buffer, size);
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)Curl_ssl->version(buffer, size);
#endif
}

ଶୂନ୍ୟ Curl_ssl_free_certinfo(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା curl_certinfo *ci = &data->info.certs;

  ଯଦି(ci->ଅଙ୍କ_of_certs) {
    /* free all individual lists used */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପାଇଁ(i = 0; i < ci->ଅଙ୍କ_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = ଖାଲି;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->ଅଙ୍କ_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ)
{
  ସଂରଚନା curl_certinfo *ci = &data->info.certs;
  ସଂରଚନା curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)ଅଙ୍କ, ଆକାର(ସଂରଚନା curl_slist *));
  ଯଦି(!table)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  ci->ଅଙ୍କ_of_certs = ଅଙ୍କ;
  ci->certinfo = table;

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ଠେଲ_certinfo_len(ସଂରଚନା Curl_easy *data,
                                    ପୂର୍ଣ୍ଣସଂଖ୍ୟା certnum,
                                    ଅଚଳ ଅକ୍ଷର *label,
                                    ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ,
                                    size_t valuelen)
{
  ସଂରଚନା curl_certinfo *ci = &data->info.certs;
  ସଂରଚନା curl_slist *nl;
  CURLcode result = CURLE_OK;
  ସଂରଚନା dynbuf build;

  DEBUGASSERT(certnum < ci->ଅଙ୍କ_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  ଯଦି(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, ମୂଲ୍ୟ, valuelen))
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  ଯଦି(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  ଫେରନ୍ତୁ result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(ସଂରଚନା Curl_easy *data,
                         unsigned ଅକ୍ଷର *buffer, size_t length)
{
  DEBUGASSERT(length == ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
  ଯଦି(Curl_ssl->random)
    ଫେରନ୍ତୁ Curl_ssl->random(data, buffer, length);
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

ସ୍ଥିର CURLcode pubkey_pem_to_der(ଅଚଳ ଅକ୍ଷର *pem,
                                  unsigned ଅକ୍ଷର **der, size_t *der_len)
{
  ଅଚଳ ଅକ୍ଷର *begin_pos, *end_pos;
  size_t pem_ଗଣନା, pem_len;
  CURLcode result;
  ସଂରଚନା dynbuf pbuf;

  /* if no pem, exit. */
  ଯଦି(!pem)
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  ଯଦି(!begin_pos)
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  pem_ଗଣନା = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  ଯଦି(pem_ଗଣନା && '\n' != pem[pem_ଗଣନା - 1])
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_ଗଣନା += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_ଗଣନା, "\n-----END PUBLIC KEY-----");
  ଯଦି(!end_pos)
    ଫେରନ୍ତୁ CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  ଯେତେବେଳେ(pem_ଗଣନା < pem_len) {
    ଯଦି('\n' != pem[pem_ଗଣନା] && '\r' != pem[pem_ଗଣନା]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_ଗଣନା], 1);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
    ++pem_ଗଣନା;
  }

  ଯଦି(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  ଅନ୍ୟଥା
    result = CURLE_BAD_CONTENT_ENCODING;

  ଫେରନ୍ତୁ result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *pinnedpubkey,
                              ଅଚଳ unsigned ଅକ୍ଷର *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ଯଦି(!pinnedpubkey)
    ଫେରନ୍ତୁ CURLE_OK;
  ଯଦି(!pubkey || !pubkeylen)
    ଫେରନ୍ତୁ result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  ଯଦି(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    ଅକ୍ଷର *cert_hash = ଖାଲି;
    ଅଚଳ ଅକ୍ଷର *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned ଅକ୍ଷର *sha256sumdigest;

    ଯଦି(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      ଫେରନ୍ତୁ result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    ଯଦି(!sha256sumdigest)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    ଯଦି(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    ଯଦି(encode)
      ଫେରନ୍ତୁ encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    ଯେତେବେଳେ(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      ଯଦି(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        ଭାଙ୍ଗ;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : ଖାଲି;
    }
    curlx_safefree(cert_hash);
  }
  ଅନ୍ୟଥା {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    ସଂରଚନା dynbuf buf;
    unsigned ଅକ୍ଷର *pem_ptr = ଖାଲି;
    size_t ବାମ;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    ଯଦି(!fp)
      ଫେରନ୍ତୁ result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    ଯଦି(fseek(fp, 0, SEEK_END))
      ଯାଅ end;
    filesize = ftell(fp);
    ଯଦି(fseek(fp, 0, SEEK_SET))
      ଯାଅ end;
    ଯଦି(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      ଯାଅ end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    ଯଦି(pubkeylen > size)
      ଯାଅ end;

    /*
     * Read the file into the dynbuf
     */
    ବାମ = size;
    କର {
      ଅକ୍ଷର buffer[1024];
      size_t want = ବାମ > ଆକାର(buffer) ? ଆକାର(buffer) : ବାମ;
      ଯଦି(fread(buffer, 1, want, fp) != want)
        ଯାଅ end;
      ଯଦି(curlx_dyn_addn(&buf, buffer, want))
        ଯାଅ end;
      ବାମ -= want;
    } ଯେତେବେଳେ(ବାମ);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    ଯଦି(pubkeylen == size) {
      ଯଦି(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      ଯାଅ end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    ଯଦି(pem_read)
      ଯାଅ end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    ଯଦି(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  ଫେରନ୍ତୁ result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(ଶୂନ୍ୟ)
{
  ଯଦି(Curl_ssl->cert_status_request)
    ଫେରନ୍ତୁ Curl_ssl->cert_status_request();
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା multissl_init(ଶୂନ୍ୟ)
{
  ଯଦି(multissl_setup(ଖାଲି))
    ଫେରନ୍ତୁ 1;
  ଯଦି(Curl_ssl->init)
    ଫେରନ୍ତୁ Curl_ssl->init();
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର CURLcode multissl_random(ସଂରଚନା Curl_easy *data,
                                unsigned ଅକ୍ଷର *entropy, size_t length)
{
  ଯଦି(multissl_setup(ଖାଲି))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_ssl->random(data, entropy, length);
}

ସ୍ଥିର CURLcode multissl_connect(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data, bool *done)
{
  ଯଦି(multissl_setup(ଖାଲି))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_ssl->do_connect(cf, data, done);
}

ସ୍ଥିର CURLcode multissl_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା easy_pollset *ps)
{
  ଯଦି(multissl_setup(ଖାଲି))
    ଫେରନ୍ତୁ CURLE_OK;
  ଫେରନ୍ତୁ Curl_ssl->adjust_pollset(cf, data, ps);
}

ସ୍ଥିର ଶୂନ୍ୟ *multissl_get_internals(ସଂରଚନା ssl_connect_data *connssl,
                                    CURLINFO info)
{
  ଯଦି(multissl_setup(ଖାଲି))
    ଫେରନ୍ତୁ ଖାଲି;
  ଫେରନ୍ତୁ Curl_ssl->get_internals(connssl, info);
}

ସ୍ଥିର ଶୂନ୍ୟ multissl_close(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ଯଦି(multissl_setup(ଖାଲି))
    ଫେରନ୍ତୁ;
  Curl_ssl->close(cf, data);
}

ସ୍ଥିର CURLcode multissl_recv_plain(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ଅକ୍ଷର *buf, size_t len, size_t *pnread)
{
  ଯଦି(multissl_setup(ଖାଲି))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

ସ୍ଥିର CURLcode multissl_send_plain(ସଂରଚନା Curl_cfilter *cf,
                                    ସଂରଚନା Curl_easy *data,
                                    ଅଚଳ ଶୂନ୍ୟ *mem, size_t len,
                                    size_t *pnwritten)
{
  ଯଦି(multissl_setup(ଖାଲି))
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  ଫେରନ୍ତୁ Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  ଖାଲି,                              /* cleanup */
  multissl_version,                  /* version */
  ଖାଲି,                              /* shutdown */
  ଖାଲି,                              /* data_pending */
  multissl_random,                   /* random */
  ଖାଲି,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  ଖାଲି,                              /* close_all */
  ଖାଲି,                              /* engines_list */
  ଖାଲି,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  ଖାଲି,                              /* get_channel_binding */
};

ଅଚଳ ସଂରଚନା Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#ନଚେତଯଦି defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#ନଚେତଯଦି defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#ନଚେତଯଦି defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#ନଚେତଯଦି defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#ନଚେତଯଦି defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#ନଚେତଯଦି defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#ଅନ୍ୟଥା
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_ssl *available_backends[] = {
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
  ଖାଲି
};

/* Global cleanup */
ଶୂନ୍ୟ Curl_ssl_cleanup(ଶୂନ୍ୟ)
{
  ଯଦି(init_ssl) {
    /* only cleanup if we did a previous init */
    ଯଦି(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

ସ୍ଥିର size_t multissl_version(ଅକ୍ଷର *buffer, size_t size)
{
  ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_ssl *selected;
  ସ୍ଥିର ଅକ୍ଷର backends[200];
  ସ୍ଥିର size_t backends_len;
  ଅଚଳ ସଂରଚନା Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  ଯଦି(current != selected) {
    ଅକ୍ଷର *p = backends;
    ଅଚଳ ଅକ୍ଷର *end = backends + ଆକାର(backends);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    selected = current;

    backends[0] = '\0';

    ପାଇଁ(i = 0; available_backends[i]; ++i) {
      ଅକ୍ଷର vb[200];
      bool paren = (selected != available_backends[i]);

      ଯଦି(available_backends[i]->version(vb, ଆକାର(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  ଯଦି(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା multissl_setup(ଅଚଳ ସଂରଚନା Curl_ssl *backend)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ଅକ୍ଷର *env;

  ଯଦି(Curl_ssl != &Curl_ssl_multi)
    ଫେରନ୍ତୁ 1;

  ଯଦି(backend) {
    Curl_ssl = backend;
    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(!available_backends[0])
    ଫେରନ୍ତୁ 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  ଯଦି(env) {
    ପାଇଁ(i = 0; available_backends[i]; i++) {
      ଯଦି(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        ଫେରନ୍ତୁ 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  ପାଇଁ(i = 0; available_backends[i]; i++) {
    ଯଦି(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      ଫେରନ୍ତୁ 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  ଫେରନ୍ତୁ 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ଅଚଳ ଅକ୍ଷର *name,
                                   ଅଚଳ curl_ssl_backend ***avail)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ଯଦି(avail)
    *avail = (ଅଚଳ curl_ssl_backend **)&available_backends;

  ଯଦି(Curl_ssl != &Curl_ssl_multi)
    ଫେରନ୍ତୁ id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#ଅନ୍ୟଥା
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  ପାଇଁ(i = 0; available_backends[i]; i++) {
    ଯଦି(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      ଫେରନ୍ତୁ CURLSSLSET_OK;
    }
  }

  ଫେରନ୍ତୁ CURLSSLSET_UNKNOWN_BACKEND;
}

#ଅନ୍ୟଥା /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ଅଚଳ ଅକ୍ଷର *name,
                                   ଅଚଳ curl_ssl_backend ***avail)
{
  (ଶୂନ୍ୟ)id;
  (ଶୂନ୍ୟ)name;
  (ଶୂନ୍ୟ)avail;
  ଫେରନ୍ତୁ CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

ଶୂନ୍ୟ Curl_ssl_peer_cleanup(ସଂରଚନା ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_ଚାବି);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

ସ୍ଥିର ssl_peer_type get_peer_type(ଅଚଳ ଅକ୍ଷର *hostname)
{
  ଯଦି(hostname && hostname[0]) {
#ifdef USE_IPV6
    ସଂରଚନା in6_addr addr;
#ଅନ୍ୟଥା
    ସଂରଚନା in_addr addr;
#endif
    ଯଦି(curlx_inet_pton(AF_INET, hostname, &addr))
      ଫେରନ୍ତୁ CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    ଅନ୍ୟଥା ଯଦି(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      ଫେରନ୍ତୁ CURL_SSL_PEER_IPV6;
    }
#endif
  }
  ଫେରନ୍ତୁ CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(ସଂରଚନା ssl_peer *ssl_peer,
                            ସଂରଚନା Curl_peer *origin,
                            ସଂରଚନା Curl_peer *peer,
                            ସଂରଚନା ssl_filter_config *sslc,
                            ଅଚଳ ଅକ୍ଷର *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  ଯଦି(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  ଯଦି(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    ଯଦି(len && (origin->hostname[len - 1] == '.'))
      len--;
    ଯଦି(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      ଯଦି(!ssl_peer->sni)
        ଯାଅ out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_ଚାବି_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_ଚାବି);

out:
  ଯଦି(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ ssl_cf_destroy(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ଯଦି(connssl) {
    connssl->ssl_impl->close(cf, data);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = ଖାଲି;
  }
  cf->connected = FALSE;
}

ସ୍ଥିର CURLcode ssl_cf_connect(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               bool *done)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result;

  ଯଦି(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  ଯଦି(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ଯଦି(result || !*done)
      ଫେରନ୍ତୁ result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  ଯଦି(!connssl->prefs_checked) {
    ଯଦି(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  ଯଦି(!result && *done) {
    cf->connected = TRUE;
    ଯଦି(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    ଯଦି(Curl_tls_keylog_enabled()) {
      infof(data, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_ସଂଖ୍ୟା
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *done);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssl_cf_set_earlydata(ସଂରଚନା Curl_cfilter *cf,
                                     ସଂରଚନା Curl_easy *data,
                                     ଅଚଳ ଶୂନ୍ୟ *buf, size_t blen)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  ଯଦି(blen) {
    ଯଦି(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssl_cf_connect_deferred(ସଂରଚନା Curl_cfilter *cf,
                                        ସଂରଚନା Curl_easy *data,
                                        ଅଚଳ ଶୂନ୍ୟ *buf, size_t blen,
                                        bool *done)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  ଯଦି(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  ଯଦି(!result && *done) {
    ଯଦି(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    ବାଛ(connssl->earlydata_state) {
    ସ୍ଥିତି ssl_earlydata_none:
      ଭାଙ୍ଗ;
    ସ୍ଥିତି ssl_earlydata_accepted:
      ଯଦି(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି ssl_earlydata_rejected:
      ଯଦି(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(ଖାଲି);
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର bool ssl_cf_data_pending(ସଂରଚନା Curl_cfilter *cf,
                                ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  ଯଦି(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  ଅନ୍ୟଥା
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ pending;
}

ସ୍ଥିର CURLcode ssl_cf_send(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ଅଚଳ uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result = CURLE_OK;

  (ଶୂନ୍ୟ)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  ଯଦି(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    ଯଦି(result)
      ଯାଅ out;
    ଅନ୍ୟଥା ଯଦି(!done) {
      result = CURLE_AGAIN;
      ଯାଅ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ଯଦି(connssl->earlydata_skip) {
    ଯଦି(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  ଯଦି(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    ଯଦି(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssl_cf_recv(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data, ଅକ୍ଷର *buf, size_t len,
                            size_t *pnread)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  ଯଦି(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, ଖାଲି, 0, &done);
    ଯଦି(result)
      ଯାଅ out;
    ଅନ୍ୟଥା ଯଦି(!done) {
      result = CURLE_AGAIN;
      ଯାଅ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssl_cf_shutdown(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                bool *done)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  ଯଦି(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    ସଂରଚନା cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssl_cf_adjust_pollset(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା easy_pollset *ps)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode ssl_cf_query(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା query, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pres1, ଶୂନ୍ୟ *pres2)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;

  ବାଛ(query) {
  ସ୍ଥିତି CF_QUERY_SSL_INFO:
  ସ୍ଥିତି CF_QUERY_SSL_CTX_INFO:
    ଯଦି(!Curl_ssl_cf_is_proxy(cf)) {
      ସଂରଚନା curl_tlssessioninfo *info = pres2;
      ସଂରଚନା cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_QUERY_ALPN_NEGOTIATED: {
    ଅଚଳ ଅକ୍ଷର **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    ଫେରନ୍ତୁ CURLE_OK;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

ସ୍ଥିର CURLcode ssl_cf_cntrl(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg1, ଶୂନ୍ୟ *arg2)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;

  (ଶୂନ୍ୟ)arg1;
  (ଶୂନ୍ୟ)arg2;
  (ଶୂନ୍ୟ)data;
  ବାଛ(event) {
  ସ୍ଥିତି CF_CTRL_CONN_INFO_UPDATE:
    ଯଦି(connssl->negotiated.alpn && !cf->sockindex) {
      ଯଦି(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      ଅନ୍ୟଥା ଯଦି(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      ଅନ୍ୟଥା ଯଦି(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CF_CTRL_REPORT_STATS:
    ଯଦି(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool cf_ssl_is_alive(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data,
                            bool *ନିବେଶ_pending)
{
  /*
   * This function tries to determine connection status.
   */
  ଫେରନ୍ତୁ cf->next ?
    cf->next->cft->is_alive(cf->next, data, ନିବେଶ_pending) :
    FALSE; /* pessimistic in absence of data */
}

ସଂରଚନା Curl_cftype Curl_cft_ssl = {
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

ସଂରଚନା Curl_cftype Curl_cft_ssl_proxy = {
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

ସ୍ଥିର CURLcode cf_ssl_create(ସଂରଚନା Curl_cfilter **pcf,
                              ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (ଶୂନ୍ୟ)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, ଖାଲି);
#ଅନ୍ୟଥା
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  ଯଦି(result)
    cf_ctx_free(ctx);
  *pcf = result ? ଖାଲି : cf;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_ssl_peer_init(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_peer *origin,
                                 ସଂରଚନା Curl_peer *peer,
                                 ସଂରଚନା ssl_filter_config *sslc)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ଅକ୍ଷର tls_id[80];
  connssl->ssl_impl->version(tls_id, ଆକାର(tls_id) - 1);
  ଫେରନ୍ତୁ Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_peer *origin,
                              ସଂରଚନା connectdata *conn,
                              int8_t sockindex)
{
  ସଂରଚନା Curl_cfilter *cf;
  ସଂରଚନା Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  ଯଦି(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  ଯଦି(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  ଅନ୍ୟଥା ଯଦି(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_ssl_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା Curl_peer *origin,
                                  ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  ଯଦି(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  ଯଦି(!result) {
    Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);
#ଯଦି defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    ଯଦି((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  ଅନ୍ୟଥା ଯଦି(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ଫେରନ୍ତୁ result;
}

#ifndef CURL_DISABLE_PROXY

ସ୍ଥିର CURLcode cf_ssl_proxy_create(ସଂରଚନା Curl_cfilter **pcf,
                                    ସଂରଚନା Curl_easy *data,
                                    ସଂରଚନା connectdata *conn)
{
  ସଂରଚନା Curl_cfilter *cf = ଖାଲି;
  ସଂରଚନା ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (ଶୂନ୍ୟ)conn;
#ifdef USE_HTTP2
  ଯଦି(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  ଯଦି(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  ଯଦି(result)
    cf_ctx_free(ctx);
  *pcf = result ? ଖାଲି : cf;
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_cf_ssl_proxy_ଭର୍ତ୍ତି_after(ସଂରଚନା Curl_cfilter *cf_at,
                                        ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା Curl_peer *peer)
{
  ସଂରଚନା Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  ଯଦି(!result)
    result = cf_ssl_peer_init(cf, peer, ଖାଲି, &cf_at->conn->proxy_ssl_config);
  ଯଦି(!result)
    Curl_conn_cf_ଭର୍ତ୍ତି_after(cf_at, cf);
  ଅନ୍ୟଥା ଯଦି(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ଫେରନ୍ତୁ result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(ସଂରଚନା Curl_easy *data, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ssl_option)
{
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(ସଂରଚନା Curl_cfilter *cf)
{
  ଯଦି(Curl_ssl->supports & SSLSUPP_ECH) {
    ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ଫେରନ୍ତୁ !!conn_config->ech;
  }
  ଫେରନ୍ତୁ FALSE;
}

bool Curl_ssl_need_httpsrr(ସଂରଚନା Curl_cfilter *cf)
{
  ଯଦି(Curl_ssl->supports & SSLSUPP_ECH) {
    ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ଯଦି(!conn_config->ech)
      ଫେରନ୍ତୁ FALSE;
    ଯଦି((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}
#endif

ସ୍ଥିର CURLcode vtls_shutdown_blocking(ସଂରଚନା Curl_cfilter *cf,
                                       ସଂରଚନା Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  ସଂରଚନା cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା what, loop = 10;

  ଯଦି(cf->shutdown) {
    *done = TRUE;
    ଫେରନ୍ତୁ CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  ଯେତେବେଳେ(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    ଯଦି(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      ଯାଅ out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    ଯଦି(result || *done)
      ଯାଅ out;

    ଯଦି(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      ଯଦି(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        ଯାଅ out;
      }
      ଅନ୍ୟଥା ଯଦି(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        ଯାଅ out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_ssl_cfilter_remove(ସଂରଚନା Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  ସଂରଚନା Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : ଖାଲି;
  ପାଇଁ(cf = head; cf; cf = cf->next) {
    ଯଦି(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      ଯଦି(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
      ଭାଙ୍ଗ;
    }
  }
  ଫେରନ୍ତୁ result;
}

bool Curl_ssl_cf_is_proxy(ସଂରଚନା Curl_cfilter *cf)
{
  ଫେରନ୍ତୁ (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

ସଂରଚନା ssl_easy_config *
Curl_ssl_cf_get_easy_config(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (ଶୂନ୍ୟ)cf;
  ଫେରନ୍ତୁ &data->set.ssl;
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

ସଂରଚନା ssl_filter_config *
Curl_ssl_cf_get_filter_config(ସଂରଚନା Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  ଫେରନ୍ତୁ &cf->conn->ssl_config;
#ଅନ୍ୟଥା
  ଫେରନ୍ତୁ Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(ସଂରଚନା alpn_proto_buf *buf,
                                ଅଚଳ ସଂରଚନା alpn_spec *spec)
{
  size_t i, len;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା off = 0;
  unsigned ଅକ୍ଷର blen;

  memset(buf, 0, ଆକାର(*buf));
  ପାଇଁ(i = 0; spec && i < spec->ଗଣନା_2; ++i) {
    len = strlen(spec->entries[i]);
    ଯଦି(len >= ALPN_NAME_MAX)
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    blen = (unsigned ଅକ୍ଷର)len;
    ଯଦି(off + blen + 1 >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(buf->data))
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(ସଂରଚନା alpn_proto_buf *buf,
                                ଅଚଳ ସଂରଚନା alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, ଆକାର(*buf));
  ପାଇଁ(i = 0; spec && i < spec->ଗଣନା_2; ++i) {
    len = strlen(spec->entries[i]);
    ଯଦି(len >= ALPN_NAME_MAX)
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    ଯଦି(off + len + 2 >= ଆକାର(buf->data))
      ଫେରନ୍ତୁ CURLE_FAILED_INIT;
    ଯଦି(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)off;
  ଫେରନ୍ତୁ CURLE_OK;
}

bool Curl_alpn_contains_proto(ଅଚଳ ସଂରଚନା alpn_spec *spec,
                              ଅଚଳ ଅକ୍ଷର *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  ପାଇଁ(i = 0; spec && plen && i < spec->ଗଣନା_2; ++i) {
    size_t slen = strlen(spec->entries[i]);
    ଯଦି((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ଶୂନ୍ୟ Curl_alpn_restrict_to(ସଂରଚନା alpn_spec *spec, ଅଚଳ ଅକ୍ଷର *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < ଆକାର(spec->entries[0]));
  ଯଦି(plen < ଆକାର(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->ଗଣନା_2 = 1;
  }
}

ଶୂନ୍ୟ Curl_alpn_copy(ସଂରଚନା alpn_spec *dest, ଅଚଳ ସଂରଚନା alpn_spec *src)
{
  ଯଦି(src)
    memcpy(dest, src, ଆକାର(*dest));
  ଅନ୍ୟଥା
    memset(dest, 0, ଆକାର(*dest));
}

CURLcode Curl_alpn_set_negotiated(ସଂରଚନା Curl_cfilter *cf,
                                  ସଂରଚନା Curl_easy *data,
                                  ସଂରଚନା ssl_connect_data *connssl,
                                  ଅଚଳ unsigned ଅକ୍ଷର *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (ଶୂନ୍ୟ)cf;

  ଯଦି(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    ଯଦି(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    ଯାଅ out;
  }

  ଯଦି(proto && proto_len) {
    ଯଦି(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      ଯାଅ out;
    }
    connssl->negotiated.alpn = curlx_memdup0((ଅଚଳ ଅକ୍ଷର *)proto, proto_len);
    ଯଦି(!connssl->negotiated.alpn)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  ଯଦି(proto && proto_len) {
    ଯଦି(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)proto_len, proto);
    ଅନ୍ୟଥା
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)proto_len, proto);
  }
  ଅନ୍ୟଥା {
    ଯଦି(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    ଅନ୍ୟଥା
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_on_session_reuse(ସଂରଚନା Curl_cfilter *cf,
                               ସଂରଚନା Curl_easy *data,
                               ସଂରଚନା alpn_spec *alpns,
                               ସଂରଚନା Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  ସଂରଚନା ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  ଯଦି(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  ଅନ୍ୟଥା ଯଦି(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  ଅନ୍ୟଥା {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (ଅଚଳ unsigned ଅକ୍ଷର *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  ଫେରନ୍ତୁ result;
}

ସଂରଚନା Curl_ssl_session *Curl_ssl_get_cf_session(ସଂରଚନା Curl_easy *data,
                                                 ଅଚଳ ସଂରଚନା Curl_cftype *cft,
                                                 int8_t sockindex)
{
  ଯଦି(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#ଅନ୍ୟଥା
     (cft == &Curl_cft_ssl)) {
#endif
    ସଂରଚନା Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    ପାଇଁ(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      ଯଦି(cf1->cft == cft)
        ଭାଙ୍ଗ;
    }
    ଯଦି(cf1) {
      ସଂରଚନା ssl_connect_data *connssl = cf1->ctx;
      ଯଦି(connssl)
        ଫେରନ୍ତୁ connssl->session;
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

#endif /* USE_SSL */
