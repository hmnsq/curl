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

#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"

#അന്തര്ഭാവ "vtls/vtls.h" /* generic SSL protos etc */
#അന്തര്ഭാവ "vtls/vtls_int.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "vtls/keylog.h"

#അന്തര്ഭാവ "vtls/openssl.h"        /* OpenSSL versions */
#അന്തര്ഭാവ "vtls/gtls.h"           /* GnuTLS versions */
#അന്തര്ഭാവ "vtls/wolfssl.h"        /* wolfSSL versions */
#അന്തര്ഭാവ "vtls/schannel.h"       /* Schannel SSPI version */
#അന്തര്ഭാവ "vtls/mbedtls.h"        /* mbedTLS versions */
#അന്തര്ഭാവ "vtls/rustls.h"         /* Rustls versions */

#അന്തര്ഭാവ "slist.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "curlx/win32-fopen.h"
#അന്തര്ഭാവ "curl_sha256.h"
#അന്തര്ഭാവ "curlx/base64.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "setopt.h"
#അന്തര്ഭാവ "vdns/cf-dns.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

#ifdef USE_APPLE_SECTRUST
#അന്തര്ഭാവ <Security/Security.h>
#endif

#ifdef USE_SSL
#എങ്കിൽ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
സ്ഥിതം സ്ഥിരം ഘടന alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
സ്ഥിതം സ്ഥിരം ഘടന alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
സ്ഥിതം സ്ഥിരം ഘടന alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
സ്ഥിതം സ്ഥിരം ഘടന alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
സ്ഥിതം സ്ഥിരം ഘടന alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

സ്ഥിതം സ്ഥിരം ഘടന alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  എങ്കിൽ(!use_alpn)
    തിരികെ ശൂന്യ;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  എങ്കിൽ(only_http_10 && (wanted & CURL_HTTP_V1x))
    തിരികെ &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  എങ്കിൽ(wanted & CURL_HTTP_V2x) {
    എങ്കിൽ(wanted & CURL_HTTP_V1x)
      തിരികെ (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    തിരികെ &ALPN_SPEC_H2;
  }
#അല്ലെങ്കിൽ
  (ശൂന്യം)wanted;
  (ശൂന്യം)preferred;
#endif
  തിരികെ &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
സ്ഥിതം പൂർണ്ണസംഖ്യ multissl_setup(സ്ഥിരം ഘടന Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(ശൂന്യം)
{
#ifdef USE_SSL
  multissl_setup(ശൂന്യ);
  തിരികെ Curl_ssl->info.id;
#അല്ലെങ്കിൽ
  തിരികെ CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
സ്ഥിതം bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
പൂർണ്ണസംഖ്യ Curl_ssl_init(ശൂന്യം)
{
  /* make sure this is only done once */
  എങ്കിൽ(init_ssl)
    തിരികെ 1;
  init_ssl = TRUE; /* never again */

  എങ്കിൽ(Curl_ssl->init)
    തിരികെ Curl_ssl->init();
  തിരികെ 1;
}

സ്ഥിതം bool ssl_prefs_check(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data)
{
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  എങ്കിൽ(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    തിരികെ FALSE;
  }

  തിരഞ്ഞെടുപ്പ്(conn_config->version_max) {
  അവസ്ഥ CURL_SSLVERSION_MAX_NONE:
  അവസ്ഥ CURL_SSLVERSION_MAX_DEFAULT:
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    എങ്കിൽ((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      തിരികെ FALSE;
    }
  }

  തിരികെ TRUE;
}

സ്ഥിതം ഘടന ssl_connect_data *cf_ctx_new(ഘടന Curl_easy *data,
                                           സ്ഥിരം ഘടന alpn_spec *alpn)
{
  ഘടന ssl_connect_data *ctx;

  (ശൂന്യം)data;
  ctx = curlx_calloc(1, വലുപ്പം(*ctx));
  എങ്കിൽ(!ctx)
    തിരികെ ശൂന്യ;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  എങ്കിൽ(!ctx->backend) {
    curlx_free(ctx);
    തിരികെ ശൂന്യ;
  }
  തിരികെ ctx;
}

സ്ഥിതം ശൂന്യം cf_ctx_free(ഘടന ssl_connect_data *ctx)
{
  എങ്കിൽ(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(ഘടന Curl_easy *data, int8_t sockindex,
                                      ഘടന dynbuf *binding)
{
  എങ്കിൽ(Curl_ssl->get_channel_binding)
    തിരികെ Curl_ssl->get_channel_binding(data, sockindex, binding);
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_ssl_close_all(ഘടന Curl_easy *data)
{
  എങ്കിൽ(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 ഘടന easy_pollset *ps)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;

  എങ്കിൽ(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    എങ്കിൽ(sock != CURL_SOCKET_BAD) {
      എങ്കിൽ(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      അല്ലെങ്കിൽ {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    തിരികെ result;
  }
  തിരികെ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
ഘടന curl_slist *Curl_ssl_engines_list(ഘടന Curl_easy *data)
{
  എങ്കിൽ(Curl_ssl->engines_list)
    തിരികെ Curl_ssl->engines_list(data);
  തിരികെ ശൂന്യ;
}

സ്ഥിതം size_t multissl_version(അക്ഷരം *buffer, size_t size);

ശൂന്യം Curl_ssl_version(അക്ഷരം *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (ശൂന്യം)multissl_version(buffer, size);
#അല്ലെങ്കിൽ
  (ശൂന്യം)Curl_ssl->version(buffer, size);
#endif
}

ശൂന്യം Curl_ssl_free_certinfo(ഘടന Curl_easy *data)
{
  ഘടന curl_certinfo *ci = &data->info.certs;

  എങ്കിൽ(ci->അക്കം_of_certs) {
    /* free all individual lists used */
    പൂർണ്ണസംഖ്യ i;
    വേണ്ടി(i = 0; i < ci->അക്കം_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = ശൂന്യ;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->അക്കം_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ അക്കം)
{
  ഘടന curl_certinfo *ci = &data->info.certs;
  ഘടന curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)അക്കം, വലുപ്പം(ഘടന curl_slist *));
  എങ്കിൽ(!table)
    തിരികെ CURLE_OUT_OF_MEMORY;

  ci->അക്കം_of_certs = അക്കം;
  ci->certinfo = table;

  തിരികെ CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_തള്ളുക_certinfo_len(ഘടന Curl_easy *data,
                                    പൂർണ്ണസംഖ്യ certnum,
                                    സ്ഥിരം അക്ഷരം *label,
                                    സ്ഥിരം അക്ഷരം *മൂല്യം,
                                    size_t valuelen)
{
  ഘടന curl_certinfo *ci = &data->info.certs;
  ഘടന curl_slist *nl;
  CURLcode result = CURLE_OK;
  ഘടന dynbuf build;

  DEBUGASSERT(certnum < ci->അക്കം_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  എങ്കിൽ(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, മൂല്യം, valuelen))
    തിരികെ CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  എങ്കിൽ(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  തിരികെ result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(ഘടന Curl_easy *data,
                         unsigned അക്ഷരം *buffer, size_t length)
{
  DEBUGASSERT(length == വലുപ്പം(പൂർണ്ണസംഖ്യ));
  എങ്കിൽ(Curl_ssl->random)
    തിരികെ Curl_ssl->random(data, buffer, length);
  അല്ലെങ്കിൽ
    തിരികെ CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

സ്ഥിതം CURLcode pubkey_pem_to_der(സ്ഥിരം അക്ഷരം *pem,
                                  unsigned അക്ഷരം **der, size_t *der_len)
{
  സ്ഥിരം അക്ഷരം *begin_pos, *end_pos;
  size_t pem_എണ്ണം, pem_len;
  CURLcode result;
  ഘടന dynbuf pbuf;

  /* if no pem, exit. */
  എങ്കിൽ(!pem)
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  എങ്കിൽ(!begin_pos)
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  pem_എണ്ണം = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  എങ്കിൽ(pem_എണ്ണം && '\n' != pem[pem_എണ്ണം - 1])
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_എണ്ണം += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_എണ്ണം, "\n-----END PUBLIC KEY-----");
  എങ്കിൽ(!end_pos)
    തിരികെ CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  ആകുമ്പോൾ(pem_എണ്ണം < pem_len) {
    എങ്കിൽ('\n' != pem[pem_എണ്ണം] && '\r' != pem[pem_എണ്ണം]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_എണ്ണം], 1);
      എങ്കിൽ(result)
        തിരികെ result;
    }
    ++pem_എണ്ണം;
  }

  എങ്കിൽ(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  അല്ലെങ്കിൽ
    result = CURLE_BAD_CONTENT_ENCODING;

  തിരികെ result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *pinnedpubkey,
                              സ്ഥിരം unsigned അക്ഷരം *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  എങ്കിൽ(!pinnedpubkey)
    തിരികെ CURLE_OK;
  എങ്കിൽ(!pubkey || !pubkeylen)
    തിരികെ result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  എങ്കിൽ(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    അക്ഷരം *cert_hash = ശൂന്യ;
    സ്ഥിരം അക്ഷരം *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned അക്ഷരം *sha256sumdigest;

    എങ്കിൽ(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      തിരികെ result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    എങ്കിൽ(!sha256sumdigest)
      തിരികെ CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    എങ്കിൽ(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    എങ്കിൽ(encode)
      തിരികെ encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    ആകുമ്പോൾ(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      എങ്കിൽ(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        നിർത്തുക;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (പൂർണ്ണസംഖ്യ)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : ശൂന്യ;
    }
    curlx_safefree(cert_hash);
  }
  അല്ലെങ്കിൽ {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    ഘടന dynbuf buf;
    unsigned അക്ഷരം *pem_ptr = ശൂന്യ;
    size_t ഇടത്;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    എങ്കിൽ(!fp)
      തിരികെ result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    എങ്കിൽ(fseek(fp, 0, SEEK_END))
      പോകുക end;
    filesize = ftell(fp);
    എങ്കിൽ(fseek(fp, 0, SEEK_SET))
      പോകുക end;
    എങ്കിൽ(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      പോകുക end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    എങ്കിൽ(pubkeylen > size)
      പോകുക end;

    /*
     * Read the file into the dynbuf
     */
    ഇടത് = size;
    ചെയ്യുക {
      അക്ഷരം buffer[1024];
      size_t want = ഇടത് > വലുപ്പം(buffer) ? വലുപ്പം(buffer) : ഇടത്;
      എങ്കിൽ(fread(buffer, 1, want, fp) != want)
        പോകുക end;
      എങ്കിൽ(curlx_dyn_addn(&buf, buffer, want))
        പോകുക end;
      ഇടത് -= want;
    } ആകുമ്പോൾ(ഇടത്);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    എങ്കിൽ(pubkeylen == size) {
      എങ്കിൽ(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      പോകുക end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    എങ്കിൽ(pem_read)
      പോകുക end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    എങ്കിൽ(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  തിരികെ result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(ശൂന്യം)
{
  എങ്കിൽ(Curl_ssl->cert_status_request)
    തിരികെ Curl_ssl->cert_status_request();
  തിരികെ FALSE;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ multissl_init(ശൂന്യം)
{
  എങ്കിൽ(multissl_setup(ശൂന്യ))
    തിരികെ 1;
  എങ്കിൽ(Curl_ssl->init)
    തിരികെ Curl_ssl->init();
  തിരികെ 1;
}

സ്ഥിതം CURLcode multissl_random(ഘടന Curl_easy *data,
                                unsigned അക്ഷരം *entropy, size_t length)
{
  എങ്കിൽ(multissl_setup(ശൂന്യ))
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ Curl_ssl->random(data, entropy, length);
}

സ്ഥിതം CURLcode multissl_connect(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data, bool *done)
{
  എങ്കിൽ(multissl_setup(ശൂന്യ))
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ Curl_ssl->do_connect(cf, data, done);
}

സ്ഥിതം CURLcode multissl_adjust_pollset(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        ഘടന easy_pollset *ps)
{
  എങ്കിൽ(multissl_setup(ശൂന്യ))
    തിരികെ CURLE_OK;
  തിരികെ Curl_ssl->adjust_pollset(cf, data, ps);
}

സ്ഥിതം ശൂന്യം *multissl_get_internals(ഘടന ssl_connect_data *connssl,
                                    CURLINFO info)
{
  എങ്കിൽ(multissl_setup(ശൂന്യ))
    തിരികെ ശൂന്യ;
  തിരികെ Curl_ssl->get_internals(connssl, info);
}

സ്ഥിതം ശൂന്യം multissl_close(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  എങ്കിൽ(multissl_setup(ശൂന്യ))
    തിരികെ;
  Curl_ssl->close(cf, data);
}

സ്ഥിതം CURLcode multissl_recv_plain(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    അക്ഷരം *buf, size_t len, size_t *pnread)
{
  എങ്കിൽ(multissl_setup(ശൂന്യ))
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

സ്ഥിതം CURLcode multissl_send_plain(ഘടന Curl_cfilter *cf,
                                    ഘടന Curl_easy *data,
                                    സ്ഥിരം ശൂന്യം *mem, size_t len,
                                    size_t *pnwritten)
{
  എങ്കിൽ(multissl_setup(ശൂന്യ))
    തിരികെ CURLE_FAILED_INIT;
  തിരികെ Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  ശൂന്യ,                              /* cleanup */
  multissl_version,                  /* version */
  ശൂന്യ,                              /* shutdown */
  ശൂന്യ,                              /* data_pending */
  multissl_random,                   /* random */
  ശൂന്യ,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  ശൂന്യ,                              /* close_all */
  ശൂന്യ,                              /* engines_list */
  ശൂന്യ,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  ശൂന്യ,                              /* get_channel_binding */
};

സ്ഥിരം ഘടന Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#അല്ലെങ്കിലെങ്കിൽ defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#അല്ലെങ്കിൽ
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

സ്ഥിതം സ്ഥിരം ഘടന Curl_ssl *available_backends[] = {
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
  ശൂന്യ
};

/* Global cleanup */
ശൂന്യം Curl_ssl_cleanup(ശൂന്യം)
{
  എങ്കിൽ(init_ssl) {
    /* only cleanup if we did a previous init */
    എങ്കിൽ(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

സ്ഥിതം size_t multissl_version(അക്ഷരം *buffer, size_t size)
{
  സ്ഥിതം സ്ഥിരം ഘടന Curl_ssl *selected;
  സ്ഥിതം അക്ഷരം backends[200];
  സ്ഥിതം size_t backends_len;
  സ്ഥിരം ഘടന Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  എങ്കിൽ(current != selected) {
    അക്ഷരം *p = backends;
    സ്ഥിരം അക്ഷരം *end = backends + വലുപ്പം(backends);
    പൂർണ്ണസംഖ്യ i;

    selected = current;

    backends[0] = '\0';

    വേണ്ടി(i = 0; available_backends[i]; ++i) {
      അക്ഷരം vb[200];
      bool paren = (selected != available_backends[i]);

      എങ്കിൽ(available_backends[i]->version(vb, വലുപ്പം(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  എങ്കിൽ(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ multissl_setup(സ്ഥിരം ഘടന Curl_ssl *backend)
{
  പൂർണ്ണസംഖ്യ i;
  അക്ഷരം *env;

  എങ്കിൽ(Curl_ssl != &Curl_ssl_multi)
    തിരികെ 1;

  എങ്കിൽ(backend) {
    Curl_ssl = backend;
    തിരികെ 0;
  }

  എങ്കിൽ(!available_backends[0])
    തിരികെ 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  എങ്കിൽ(env) {
    വേണ്ടി(i = 0; available_backends[i]; i++) {
      എങ്കിൽ(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        തിരികെ 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  വേണ്ടി(i = 0; available_backends[i]; i++) {
    എങ്കിൽ(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      തിരികെ 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  തിരികെ 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, സ്ഥിരം അക്ഷരം *name,
                                   സ്ഥിരം curl_ssl_backend ***avail)
{
  പൂർണ്ണസംഖ്യ i;

  എങ്കിൽ(avail)
    *avail = (സ്ഥിരം curl_ssl_backend **)&available_backends;

  എങ്കിൽ(Curl_ssl != &Curl_ssl_multi)
    തിരികെ id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#അല്ലെങ്കിൽ
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  വേണ്ടി(i = 0; available_backends[i]; i++) {
    എങ്കിൽ(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      തിരികെ CURLSSLSET_OK;
    }
  }

  തിരികെ CURLSSLSET_UNKNOWN_BACKEND;
}

#അല്ലെങ്കിൽ /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, സ്ഥിരം അക്ഷരം *name,
                                   സ്ഥിരം curl_ssl_backend ***avail)
{
  (ശൂന്യം)id;
  (ശൂന്യം)name;
  (ശൂന്യം)avail;
  തിരികെ CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

ശൂന്യം Curl_ssl_peer_cleanup(ഘടന ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_താക്കോൽ);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

സ്ഥിതം ssl_peer_type get_peer_type(സ്ഥിരം അക്ഷരം *hostname)
{
  എങ്കിൽ(hostname && hostname[0]) {
#ifdef USE_IPV6
    ഘടന in6_addr addr;
#അല്ലെങ്കിൽ
    ഘടന in_addr addr;
#endif
    എങ്കിൽ(curlx_inet_pton(AF_INET, hostname, &addr))
      തിരികെ CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      തിരികെ CURL_SSL_PEER_IPV6;
    }
#endif
  }
  തിരികെ CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(ഘടന ssl_peer *ssl_peer,
                            ഘടന Curl_peer *origin,
                            ഘടന Curl_peer *peer,
                            ഘടന ssl_filter_config *sslc,
                            സ്ഥിരം അക്ഷരം *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  എങ്കിൽ(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    തിരികെ CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  എങ്കിൽ(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    എങ്കിൽ(len && (origin->hostname[len - 1] == '.'))
      len--;
    എങ്കിൽ(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      എങ്കിൽ(!ssl_peer->sni)
        പോകുക out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_താക്കോൽ_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_താക്കോൽ);

out:
  എങ്കിൽ(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം ssl_cf_destroy(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  എങ്കിൽ(connssl) {
    connssl->ssl_impl->close(cf, data);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = ശൂന്യ;
  }
  cf->connected = FALSE;
}

സ്ഥിതം CURLcode ssl_cf_connect(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               bool *done)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result;

  എങ്കിൽ(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }

  എങ്കിൽ(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    തിരികെ CURLE_FAILED_INIT;
  }

  എങ്കിൽ(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    എങ്കിൽ(result || !*done)
      തിരികെ result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  എങ്കിൽ(!connssl->prefs_checked) {
    എങ്കിൽ(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  എങ്കിൽ(!result && *done) {
    cf->connected = TRUE;
    എങ്കിൽ(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    എങ്കിൽ(Curl_tls_keylog_enabled()) {
      infof(data, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_സംഖ്യ
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (പൂർണ്ണസംഖ്യ)result, *done);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode ssl_cf_set_earlydata(ഘടന Curl_cfilter *cf,
                                     ഘടന Curl_easy *data,
                                     സ്ഥിരം ശൂന്യം *buf, size_t blen)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  എങ്കിൽ(blen) {
    എങ്കിൽ(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssl_cf_connect_deferred(ഘടന Curl_cfilter *cf,
                                        ഘടന Curl_easy *data,
                                        സ്ഥിരം ശൂന്യം *buf, size_t blen,
                                        bool *done)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  എങ്കിൽ(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    എങ്കിൽ(result)
      തിരികെ result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  എങ്കിൽ(!result && *done) {
    എങ്കിൽ(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    തിരഞ്ഞെടുപ്പ്(connssl->earlydata_state) {
    അവസ്ഥ ssl_earlydata_none:
      നിർത്തുക;
    അവസ്ഥ ssl_earlydata_accepted:
      എങ്കിൽ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      നിർത്തുക;
    അവസ്ഥ ssl_earlydata_rejected:
      എങ്കിൽ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(ശൂന്യ);
      നിർത്തുക;
    }
  }
  തിരികെ result;
}

സ്ഥിതം bool ssl_cf_data_pending(ഘടന Curl_cfilter *cf,
                                സ്ഥിരം ഘടന Curl_easy *data)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  എങ്കിൽ(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  അല്ലെങ്കിൽ
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  തിരികെ pending;
}

സ്ഥിതം CURLcode ssl_cf_send(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            സ്ഥിരം uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result = CURLE_OK;

  (ശൂന്യം)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  എങ്കിൽ(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    എങ്കിൽ(result)
      പോകുക out;
    അല്ലെങ്കിൽ എങ്കിൽ(!done) {
      result = CURLE_AGAIN;
      പോകുക out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  എങ്കിൽ(connssl->earlydata_skip) {
    എങ്കിൽ(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      പോകുക out;
    }
    അല്ലെങ്കിൽ {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  എങ്കിൽ(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    എങ്കിൽ(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode ssl_cf_recv(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data, അക്ഷരം *buf, size_t len,
                            size_t *pnread)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  എങ്കിൽ(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, ശൂന്യ, 0, &done);
    എങ്കിൽ(result)
      പോകുക out;
    അല്ലെങ്കിൽ എങ്കിൽ(!done) {
      result = CURLE_AGAIN;
      പോകുക out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode ssl_cf_shutdown(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                bool *done)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  എങ്കിൽ(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    ഘടന cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (പൂർണ്ണസംഖ്യ)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  തിരികെ result;
}

സ്ഥിതം CURLcode ssl_cf_adjust_pollset(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      ഘടന easy_pollset *ps)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  തിരികെ result;
}

സ്ഥിതം CURLcode ssl_cf_query(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             പൂർണ്ണസംഖ്യ query, പൂർണ്ണസംഖ്യ *pres1, ശൂന്യം *pres2)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;

  തിരഞ്ഞെടുപ്പ്(query) {
  അവസ്ഥ CF_QUERY_SSL_INFO:
  അവസ്ഥ CF_QUERY_SSL_CTX_INFO:
    എങ്കിൽ(!Curl_ssl_cf_is_proxy(cf)) {
      ഘടന curl_tlssessioninfo *info = pres2;
      ഘടന cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      തിരികെ CURLE_OK;
    }
    നിർത്തുക;
  അവസ്ഥ CF_QUERY_ALPN_NEGOTIATED: {
    സ്ഥിരം അക്ഷരം **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    തിരികെ CURLE_OK;
  }
  സ്ഥിരസ്ഥിതി:
    നിർത്തുക;
  }
  തിരികെ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

സ്ഥിതം CURLcode ssl_cf_cntrl(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             പൂർണ്ണസംഖ്യ event, പൂർണ്ണസംഖ്യ arg1, ശൂന്യം *arg2)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;

  (ശൂന്യം)arg1;
  (ശൂന്യം)arg2;
  (ശൂന്യം)data;
  തിരഞ്ഞെടുപ്പ്(event) {
  അവസ്ഥ CF_CTRL_CONN_INFO_UPDATE:
    എങ്കിൽ(connssl->negotiated.alpn && !cf->sockindex) {
      എങ്കിൽ(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      അല്ലെങ്കിൽ എങ്കിൽ(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      അല്ലെങ്കിൽ എങ്കിൽ(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    നിർത്തുക;
  അവസ്ഥ CF_CTRL_REPORT_STATS:
    എങ്കിൽ(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം bool cf_ssl_is_alive(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data,
                            bool *നിവേശം_pending)
{
  /*
   * This function tries to determine connection status.
   */
  തിരികെ cf->next ?
    cf->next->cft->is_alive(cf->next, data, നിവേശം_pending) :
    FALSE; /* pessimistic in absence of data */
}

ഘടന Curl_cftype Curl_cft_ssl = {
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

ഘടന Curl_cftype Curl_cft_ssl_proxy = {
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

സ്ഥിതം CURLcode cf_ssl_create(ഘടന Curl_cfilter **pcf,
                              ഘടന Curl_easy *data,
                              ഘടന connectdata *conn)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (ശൂന്യം)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, ശൂന്യ);
#അല്ലെങ്കിൽ
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  എങ്കിൽ(result)
    cf_ctx_free(ctx);
  *pcf = result ? ശൂന്യ : cf;
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_ssl_peer_init(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_peer *origin,
                                 ഘടന Curl_peer *peer,
                                 ഘടന ssl_filter_config *sslc)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  അക്ഷരം tls_id[80];
  connssl->ssl_impl->version(tls_id, വലുപ്പം(tls_id) - 1);
  തിരികെ Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(ഘടന Curl_easy *data,
                              ഘടന Curl_peer *origin,
                              ഘടന connectdata *conn,
                              int8_t sockindex)
{
  ഘടന Curl_cfilter *cf;
  ഘടന Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  എങ്കിൽ(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  എങ്കിൽ(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  അല്ലെങ്കിൽ എങ്കിൽ(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  തിരികെ result;
}

CURLcode Curl_cf_ssl_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                  ഘടന Curl_easy *data,
                                  ഘടന Curl_peer *origin,
                                  ഘടന Curl_peer *peer)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  എങ്കിൽ(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  എങ്കിൽ(!result) {
    Curl_conn_cf_തിരുകുക_after(cf_at, cf);
#എങ്കിൽ defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    എങ്കിൽ((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  അല്ലെങ്കിൽ എങ്കിൽ(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  തിരികെ result;
}

#ifndef CURL_DISABLE_PROXY

സ്ഥിതം CURLcode cf_ssl_proxy_create(ഘടന Curl_cfilter **pcf,
                                    ഘടന Curl_easy *data,
                                    ഘടന connectdata *conn)
{
  ഘടന Curl_cfilter *cf = ശൂന്യ;
  ഘടന ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (ശൂന്യം)conn;
#ifdef USE_HTTP2
  എങ്കിൽ(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  എങ്കിൽ(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    പോകുക out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  എങ്കിൽ(result)
    cf_ctx_free(ctx);
  *pcf = result ? ശൂന്യ : cf;
  തിരികെ result;
}

CURLcode Curl_cf_ssl_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                        ഘടന Curl_easy *data,
                                        ഘടന Curl_peer *peer)
{
  ഘടന Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  എങ്കിൽ(!result)
    result = cf_ssl_peer_init(cf, peer, ശൂന്യ, &cf_at->conn->proxy_ssl_config);
  എങ്കിൽ(!result)
    Curl_conn_cf_തിരുകുക_after(cf_at, cf);
  അല്ലെങ്കിൽ എങ്കിൽ(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  തിരികെ result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(ഘടന Curl_easy *data, unsigned പൂർണ്ണസംഖ്യ ssl_option)
{
  (ശൂന്യം)data;
  തിരികെ (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(ഘടന Curl_cfilter *cf)
{
  എങ്കിൽ(Curl_ssl->supports & SSLSUPP_ECH) {
    ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    തിരികെ !!conn_config->ech;
  }
  തിരികെ FALSE;
}

bool Curl_ssl_need_httpsrr(ഘടന Curl_cfilter *cf)
{
  എങ്കിൽ(Curl_ssl->supports & SSLSUPP_ECH) {
    ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    എങ്കിൽ(!conn_config->ech)
      തിരികെ FALSE;
    എങ്കിൽ((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      തിരികെ FALSE;
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}
#endif

സ്ഥിതം CURLcode vtls_shutdown_blocking(ഘടന Curl_cfilter *cf,
                                       ഘടന Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  ഘടന cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  പൂർണ്ണസംഖ്യ what, loop = 10;

  എങ്കിൽ(cf->shutdown) {
    *done = TRUE;
    തിരികെ CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  ആകുമ്പോൾ(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    എങ്കിൽ(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      പോകുക out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    എങ്കിൽ(result || *done)
      പോകുക out;

    എങ്കിൽ(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      എങ്കിൽ(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        പോകുക out;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        പോകുക out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  തിരികെ result;
}

CURLcode Curl_ssl_cfilter_remove(ഘടന Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  ഘടന Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : ശൂന്യ;
  വേണ്ടി(cf = head; cf; cf = cf->next) {
    എങ്കിൽ(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      എങ്കിൽ(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (പൂർണ്ണസംഖ്യ)result);
      നിർത്തുക;
    }
  }
  തിരികെ result;
}

bool Curl_ssl_cf_is_proxy(ഘടന Curl_cfilter *cf)
{
  തിരികെ (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

ഘടന ssl_easy_config *
Curl_ssl_cf_get_easy_config(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (ശൂന്യം)cf;
  തിരികെ &data->set.ssl;
#അല്ലെങ്കിൽ
  തിരികെ Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

ഘടന ssl_filter_config *
Curl_ssl_cf_get_filter_config(ഘടന Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  തിരികെ &cf->conn->ssl_config;
#അല്ലെങ്കിൽ
  തിരികെ Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(ഘടന alpn_proto_buf *buf,
                                സ്ഥിരം ഘടന alpn_spec *spec)
{
  size_t i, len;
  പൂർണ്ണസംഖ്യ off = 0;
  unsigned അക്ഷരം blen;

  memset(buf, 0, വലുപ്പം(*buf));
  വേണ്ടി(i = 0; spec && i < spec->എണ്ണം; ++i) {
    len = strlen(spec->entries[i]);
    എങ്കിൽ(len >= ALPN_NAME_MAX)
      തിരികെ CURLE_FAILED_INIT;
    blen = (unsigned അക്ഷരം)len;
    എങ്കിൽ(off + blen + 1 >= (പൂർണ്ണസംഖ്യ)വലുപ്പം(buf->data))
      തിരികെ CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  തിരികെ CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(ഘടന alpn_proto_buf *buf,
                                സ്ഥിരം ഘടന alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, വലുപ്പം(*buf));
  വേണ്ടി(i = 0; spec && i < spec->എണ്ണം; ++i) {
    len = strlen(spec->entries[i]);
    എങ്കിൽ(len >= ALPN_NAME_MAX)
      തിരികെ CURLE_FAILED_INIT;
    എങ്കിൽ(off + len + 2 >= വലുപ്പം(buf->data))
      തിരികെ CURLE_FAILED_INIT;
    എങ്കിൽ(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (പൂർണ്ണസംഖ്യ)off;
  തിരികെ CURLE_OK;
}

bool Curl_alpn_contains_proto(സ്ഥിരം ഘടന alpn_spec *spec,
                              സ്ഥിരം അക്ഷരം *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  വേണ്ടി(i = 0; spec && plen && i < spec->എണ്ണം; ++i) {
    size_t slen = strlen(spec->entries[i]);
    എങ്കിൽ((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

ശൂന്യം Curl_alpn_restrict_to(ഘടന alpn_spec *spec, സ്ഥിരം അക്ഷരം *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < വലുപ്പം(spec->entries[0]));
  എങ്കിൽ(plen < വലുപ്പം(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->എണ്ണം = 1;
  }
}

ശൂന്യം Curl_alpn_copy(ഘടന alpn_spec *dest, സ്ഥിരം ഘടന alpn_spec *src)
{
  എങ്കിൽ(src)
    memcpy(dest, src, വലുപ്പം(*dest));
  അല്ലെങ്കിൽ
    memset(dest, 0, വലുപ്പം(*dest));
}

CURLcode Curl_alpn_set_negotiated(ഘടന Curl_cfilter *cf,
                                  ഘടന Curl_easy *data,
                                  ഘടന ssl_connect_data *connssl,
                                  സ്ഥിരം unsigned അക്ഷരം *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (ശൂന്യം)cf;

  എങ്കിൽ(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    എങ്കിൽ(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (പൂർണ്ണസംഖ്യ)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    പോകുക out;
  }

  എങ്കിൽ(proto && proto_len) {
    എങ്കിൽ(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      പോകുക out;
    }
    connssl->negotiated.alpn = curlx_memdup0((സ്ഥിരം അക്ഷരം *)proto, proto_len);
    എങ്കിൽ(!connssl->negotiated.alpn)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }

  എങ്കിൽ(proto && proto_len) {
    എങ്കിൽ(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (പൂർണ്ണസംഖ്യ)proto_len, proto);
    അല്ലെങ്കിൽ
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (പൂർണ്ണസംഖ്യ)proto_len, proto);
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    അല്ലെങ്കിൽ
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  തിരികെ result;
}

CURLcode Curl_on_session_reuse(ഘടന Curl_cfilter *cf,
                               ഘടന Curl_easy *data,
                               ഘടന alpn_spec *alpns,
                               ഘടന Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  ഘടന ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  എങ്കിൽ(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  അല്ലെങ്കിൽ {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (സ്ഥിരം unsigned അക്ഷരം *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  തിരികെ result;
}

ഘടന Curl_ssl_session *Curl_ssl_get_cf_session(ഘടന Curl_easy *data,
                                                 സ്ഥിരം ഘടന Curl_cftype *cft,
                                                 int8_t sockindex)
{
  എങ്കിൽ(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#അല്ലെങ്കിൽ
     (cft == &Curl_cft_ssl)) {
#endif
    ഘടന Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    വേണ്ടി(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      എങ്കിൽ(cf1->cft == cft)
        നിർത്തുക;
    }
    എങ്കിൽ(cf1) {
      ഘടന ssl_connect_data *connssl = cf1->ctx;
      എങ്കിൽ(connssl)
        തിരികെ connssl->session;
    }
  }
  തിരികെ ശൂന്യ;
}

#endif /* USE_SSL */
