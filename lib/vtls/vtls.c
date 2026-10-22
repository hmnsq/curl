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

#include "curl_setup.h"

#include "urldata.h"
#include "cfilters.h"

#include "vtls/vtls.h" /* generic SSL protos etc */
#include "vtls/vtls_int.h"
#include "vtls/vtls_scache.h"
#include "vtls/keylog.h"

#include "vtls/openssl.h"        /* OpenSSL versions */
#include "vtls/gtls.h"           /* GnuTLS versions */
#include "vtls/wolfssl.h"        /* wolfSSL versions */
#include "vtls/schannel.h"       /* Schannel SSPI version */
#include "vtls/mbedtls.h"        /* mbedTLS versions */
#include "vtls/rustls.h"         /* Rustls versions */

#include "slist.h"
#include "curl_trc.h"
#include "strcase.h"
#include "url.h"
#include "progress.h"
#include "curlx/win32-fopen.h"
#include "curl_sha256.h"
#include "curlx/base64.h"
#include "curlx/inet_pton.h"
#include "connect.h"
#include "select.h"
#include "setopt.h"
#include "vdns/cf-dns.h"
#include "curlx/strdup.h"
#include "curlx/strcopy.h"

#ifdef USE_APPLE_SECTRUST
#include <Security/Security.h>
#endif

#ifdef USE_SSL
#ပေႃး !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
static const sa_nracanaa alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
static const sa_nracanaa alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
static const sa_nracanaa alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
static const sa_nracanaa alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
static const sa_nracanaa alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

static const sa_nracanaa alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  ပေႃး(!use_alpn)
    ပွၵ်ႈ NULL;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  ပေႃး(only_http_10 && (wanted & CURL_HTTP_V1x))
    ပွၵ်ႈ &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  ပေႃး(wanted & CURL_HTTP_V2x) {
    ပေႃး(wanted & CURL_HTTP_V1x)
      ပွၵ်ႈ (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    ပွၵ်ႈ &ALPN_SPEC_H2;
  }
#ပေႃးဢမ်ႇ
  (void)wanted;
  (void)preferred;
#endif
  ပွၵ်ႈ &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
static puur_naa_nka multissl_setup(const sa_nracanaa Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(void)
{
#ifdef USE_SSL
  multissl_setup(NULL);
  ပွၵ်ႈ Curl_ssl->info.id;
#ပေႃးဢမ်ႇ
  ပွၵ်ႈ CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
static bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
puur_naa_nka Curl_ssl_init(void)
{
  /* make sure this is only done once */
  ပေႃး(init_ssl)
    ပွၵ်ႈ 1;
  init_ssl = TRUE; /* never again */

  ပေႃး(Curl_ssl->init)
    ပွၵ်ႈ Curl_ssl->init();
  ပွၵ်ႈ 1;
}

static bool ssl_prefs_check(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  ပေႃး(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    ပွၵ်ႈ FALSE;
  }

  switch(conn_config->version_max) {
  pak_sa CURL_SSLVERSION_MAX_NONE:
  pak_sa CURL_SSLVERSION_MAX_DEFAULT:
    bha_nga;

  default:
    ပေႃး((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      ပွၵ်ႈ FALSE;
    }
  }

  ပွၵ်ႈ TRUE;
}

static sa_nracanaa ssl_connect_data *cf_ctx_new(sa_nracanaa Curl_easy *data,
                                           const sa_nracanaa alpn_spec *alpn)
{
  sa_nracanaa ssl_connect_data *ctx;

  (void)data;
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  ပေႃး(!ctx)
    ပွၵ်ႈ NULL;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  ပေႃး(!ctx->backend) {
    curlx_free(ctx);
    ပွၵ်ႈ NULL;
  }
  ပွၵ်ႈ ctx;
}

static void cf_ctx_free(sa_nracanaa ssl_connect_data *ctx)
{
  ပေႃး(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(sa_nracanaa Curl_easy *data, int8_t sockindex,
                                      sa_nracanaa dynbuf *binding)
{
  ပေႃး(Curl_ssl->get_channel_binding)
    ပွၵ်ႈ Curl_ssl->get_channel_binding(data, sockindex, binding);
  ပွၵ်ႈ CURLE_OK;
}

void Curl_ssl_close_all(sa_nracanaa Curl_easy *data)
{
  ပေႃး(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;

  ပေႃး(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    ပေႃး(sock != CURL_SOCKET_BAD) {
      ပေႃး(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      ပေႃးဢမ်ႇ {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    ပွၵ်ႈ result;
  }
  ပွၵ်ႈ CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
sa_nracanaa curl_slist *Curl_ssl_engines_list(sa_nracanaa Curl_easy *data)
{
  ပေႃး(Curl_ssl->engines_list)
    ပွၵ်ႈ Curl_ssl->engines_list(data);
  ပွၵ်ႈ NULL;
}

static size_t multissl_version(var_na *buffer, size_t size);

void Curl_ssl_version(var_na *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (void)multissl_version(buffer, size);
#ပေႃးဢမ်ႇ
  (void)Curl_ssl->version(buffer, size);
#endif
}

void Curl_ssl_free_certinfo(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa curl_certinfo *ci = &data->info.certs;

  ပေႃး(ci->num_of_certs) {
    /* free all individual lists used */
    puur_naa_nka i;
    for(i = 0; i < ci->num_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = NULL;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->num_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(sa_nracanaa Curl_easy *data, puur_naa_nka num)
{
  sa_nracanaa curl_certinfo *ci = &data->info.certs;
  sa_nracanaa curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)num, parimaa_na(sa_nracanaa curl_slist *));
  ပေႃး(!table)
    ပွၵ်ႈ CURLE_OUT_OF_MEMORY;

  ci->num_of_certs = num;
  ci->certinfo = table;

  ပွၵ်ႈ CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_push_certinfo_len(sa_nracanaa Curl_easy *data,
                                    puur_naa_nka certnum,
                                    const var_na *label,
                                    const var_na *value,
                                    size_t valuelen)
{
  sa_nracanaa curl_certinfo *ci = &data->info.certs;
  sa_nracanaa curl_slist *nl;
  CURLcode result = CURLE_OK;
  sa_nracanaa dynbuf build;

  DEBUGASSERT(certnum < ci->num_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  ပေႃး(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, value, valuelen))
    ပွၵ်ႈ CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  ပေႃး(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  ပွၵ်ႈ result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(sa_nracanaa Curl_easy *data,
                         unsigned var_na *buffer, size_t length)
{
  DEBUGASSERT(length == parimaa_na(puur_naa_nka));
  ပေႃး(Curl_ssl->random)
    ပွၵ်ႈ Curl_ssl->random(data, buffer, length);
  ပေႃးဢမ်ႇ
    ပွၵ်ႈ CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

static CURLcode pubkey_pem_to_der(const var_na *pem,
                                  unsigned var_na **der, size_t *der_len)
{
  const var_na *begin_pos, *end_pos;
  size_t pem_count, pem_len;
  CURLcode result;
  sa_nracanaa dynbuf pbuf;

  /* if no pem, exit. */
  ပေႃး(!pem)
    ပွၵ်ႈ CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  ပေႃး(!begin_pos)
    ပွၵ်ႈ CURLE_BAD_CONTENT_ENCODING;

  pem_count = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  ပေႃး(pem_count && '\n' != pem[pem_count - 1])
    ပွၵ်ႈ CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_count += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_count, "\n-----END PUBLIC KEY-----");
  ပေႃး(!end_pos)
    ပွၵ်ႈ CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  ၶၢဝ်း(pem_count < pem_len) {
    ပေႃး('\n' != pem[pem_count] && '\r' != pem[pem_count]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_count], 1);
      ပေႃး(result)
        ပွၵ်ႈ result;
    }
    ++pem_count;
  }

  ပေႃး(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  ပေႃးဢမ်ႇ
    result = CURLE_BAD_CONTENT_ENCODING;

  ပွၵ်ႈ result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(sa_nracanaa Curl_easy *data,
                              const var_na *pinnedpubkey,
                              const unsigned var_na *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ပေႃး(!pinnedpubkey)
    ပွၵ်ႈ CURLE_OK;
  ပေႃး(!pubkey || !pubkeylen)
    ပွၵ်ႈ result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  ပေႃး(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    var_na *cert_hash = NULL;
    const var_na *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned var_na *sha256sumdigest;

    ပေႃး(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      ပွၵ်ႈ result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    ပေႃး(!sha256sumdigest)
      ပွၵ်ႈ CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    ပေႃး(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    ပေႃး(encode)
      ပွၵ်ႈ encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    ၶၢဝ်း(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      ပေႃး(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        bha_nga;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (puur_naa_nka)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : NULL;
    }
    curlx_safefree(cert_hash);
  }
  ပေႃးဢမ်ႇ {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    sa_nracanaa dynbuf buf;
    unsigned var_na *pem_ptr = NULL;
    size_t left;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    ပေႃး(!fp)
      ပွၵ်ႈ result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    ပေႃး(fseek(fp, 0, SEEK_END))
      goto end;
    filesize = ftell(fp);
    ပေႃး(fseek(fp, 0, SEEK_SET))
      goto end;
    ပေႃး(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      goto end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    ပေႃး(pubkeylen > size)
      goto end;

    /*
     * Read the file into the dynbuf
     */
    left = size;
    do {
      var_na buffer[1024];
      size_t want = left > parimaa_na(buffer) ? parimaa_na(buffer) : left;
      ပေႃး(fread(buffer, 1, want, fp) != want)
        goto end;
      ပေႃး(curlx_dyn_addn(&buf, buffer, want))
        goto end;
      left -= want;
    } ၶၢဝ်း(left);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    ပေႃး(pubkeylen == size) {
      ပေႃး(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      goto end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    ပေႃး(pem_read)
      goto end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    ပေႃး(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  ပွၵ်ႈ result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(void)
{
  ပေႃး(Curl_ssl->cert_status_request)
    ပွၵ်ႈ Curl_ssl->cert_status_request();
  ပွၵ်ႈ FALSE;
}

static puur_naa_nka multissl_init(void)
{
  ပေႃး(multissl_setup(NULL))
    ပွၵ်ႈ 1;
  ပေႃး(Curl_ssl->init)
    ပွၵ်ႈ Curl_ssl->init();
  ပွၵ်ႈ 1;
}

static CURLcode multissl_random(sa_nracanaa Curl_easy *data,
                                unsigned var_na *entropy, size_t length)
{
  ပေႃး(multissl_setup(NULL))
    ပွၵ်ႈ CURLE_FAILED_INIT;
  ပွၵ်ႈ Curl_ssl->random(data, entropy, length);
}

static CURLcode multissl_connect(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data, bool *done)
{
  ပေႃး(multissl_setup(NULL))
    ပွၵ်ႈ CURLE_FAILED_INIT;
  ပွၵ်ႈ Curl_ssl->do_connect(cf, data, done);
}

static CURLcode multissl_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        sa_nracanaa easy_pollset *ps)
{
  ပေႃး(multissl_setup(NULL))
    ပွၵ်ႈ CURLE_OK;
  ပွၵ်ႈ Curl_ssl->adjust_pollset(cf, data, ps);
}

static void *multissl_get_internals(sa_nracanaa ssl_connect_data *connssl,
                                    CURLINFO info)
{
  ပေႃး(multissl_setup(NULL))
    ပွၵ်ႈ NULL;
  ပွၵ်ႈ Curl_ssl->get_internals(connssl, info);
}

static void multissl_close(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  ပေႃး(multissl_setup(NULL))
    ပွၵ်ႈ;
  Curl_ssl->close(cf, data);
}

static CURLcode multissl_recv_plain(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    var_na *buf, size_t len, size_t *pnread)
{
  ပေႃး(multissl_setup(NULL))
    ပွၵ်ႈ CURLE_FAILED_INIT;
  ပွၵ်ႈ Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

static CURLcode multissl_send_plain(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    const void *mem, size_t len,
                                    size_t *pnwritten)
{
  ပေႃး(multissl_setup(NULL))
    ပွၵ်ႈ CURLE_FAILED_INIT;
  ပွၵ်ႈ Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

static const sa_nracanaa Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  NULL,                              /* cleanup */
  multissl_version,                  /* version */
  NULL,                              /* shutdown */
  NULL,                              /* data_pending */
  multissl_random,                   /* random */
  NULL,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  NULL,                              /* close_all */
  NULL,                              /* engines_list */
  NULL,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  NULL,                              /* get_channel_binding */
};

const sa_nracanaa Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#elif defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#elif defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#elif defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#elif defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#elif defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#elif defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#ပေႃးဢမ်ႇ
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

static const sa_nracanaa Curl_ssl *available_backends[] = {
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
  NULL
};

/* Global cleanup */
void Curl_ssl_cleanup(void)
{
  ပေႃး(init_ssl) {
    /* only cleanup if we did a previous init */
    ပေႃး(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

static size_t multissl_version(var_na *buffer, size_t size)
{
  static const sa_nracanaa Curl_ssl *selected;
  static var_na backends[200];
  static size_t backends_len;
  const sa_nracanaa Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  ပေႃး(current != selected) {
    var_na *p = backends;
    const var_na *end = backends + parimaa_na(backends);
    puur_naa_nka i;

    selected = current;

    backends[0] = '\0';

    for(i = 0; available_backends[i]; ++i) {
      var_na vb[200];
      bool paren = (selected != available_backends[i]);

      ပေႃး(available_backends[i]->version(vb, parimaa_na(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  ပေႃး(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  ပွၵ်ႈ 0;
}

static puur_naa_nka multissl_setup(const sa_nracanaa Curl_ssl *backend)
{
  puur_naa_nka i;
  var_na *env;

  ပေႃး(Curl_ssl != &Curl_ssl_multi)
    ပွၵ်ႈ 1;

  ပေႃး(backend) {
    Curl_ssl = backend;
    ပွၵ်ႈ 0;
  }

  ပေႃး(!available_backends[0])
    ပွၵ်ႈ 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  ပေႃး(env) {
    for(i = 0; available_backends[i]; i++) {
      ပေႃး(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        ပွၵ်ႈ 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  for(i = 0; available_backends[i]; i++) {
    ပေႃး(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      ပွၵ်ႈ 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  ပွၵ်ႈ 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, const var_na *name,
                                   const curl_ssl_backend ***avail)
{
  puur_naa_nka i;

  ပေႃး(avail)
    *avail = (const curl_ssl_backend **)&available_backends;

  ပေႃး(Curl_ssl != &Curl_ssl_multi)
    ပွၵ်ႈ id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#ပေႃးဢမ်ႇ
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  for(i = 0; available_backends[i]; i++) {
    ပေႃး(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      ပွၵ်ႈ CURLSSLSET_OK;
    }
  }

  ပွၵ်ႈ CURLSSLSET_UNKNOWN_BACKEND;
}

#ပေႃးဢမ်ႇ /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, const var_na *name,
                                   const curl_ssl_backend ***avail)
{
  (void)id;
  (void)name;
  (void)avail;
  ပွၵ်ႈ CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

void Curl_ssl_peer_cleanup(sa_nracanaa ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_key);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

static ssl_peer_type get_peer_type(const var_na *hostname)
{
  ပေႃး(hostname && hostname[0]) {
#ifdef USE_IPV6
    sa_nracanaa in6_addr addr;
#ပေႃးဢမ်ႇ
    sa_nracanaa in_addr addr;
#endif
    ပေႃး(curlx_inet_pton(AF_INET, hostname, &addr))
      ပွၵ်ႈ CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    ပေႃးဢမ်ႇ ပေႃး(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      ပွၵ်ႈ CURL_SSL_PEER_IPV6;
    }
#endif
  }
  ပွၵ်ႈ CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(sa_nracanaa ssl_peer *ssl_peer,
                            sa_nracanaa Curl_peer *origin,
                            sa_nracanaa Curl_peer *peer,
                            sa_nracanaa ssl_filter_config *sslc,
                            const var_na *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  ပေႃး(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    ပွၵ်ႈ CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  ပေႃး(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    ပေႃး(len && (origin->hostname[len - 1] == '.'))
      len--;
    ပေႃး(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      ပေႃး(!ssl_peer->sni)
        goto out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_key_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_key);

out:
  ပေႃး(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  ပွၵ်ႈ result;
}

static void ssl_cf_destroy(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  ပေႃး(connssl) {
    connssl->ssl_impl->close(cf, data);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = NULL;
  }
  cf->connected = FALSE;
}

static CURLcode ssl_cf_connect(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result;

  ပေႃး(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    ပွၵ်ႈ CURLE_OK;
  }

  ပေႃး(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    ပွၵ်ႈ CURLE_FAILED_INIT;
  }

  ပေႃး(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ပေႃး(result || !*done)
      ပွၵ်ႈ result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  ပေႃး(!connssl->prefs_checked) {
    ပေႃး(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  ပေႃး(!result && *done) {
    cf->connected = TRUE;
    ပေႃး(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    ပေႃး(Curl_tls_keylog_enabled()) {
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (puur_naa_nka)result, *done);
  CF_DATA_RESTORE(cf, save);
  ပွၵ်ႈ result;
}

static CURLcode ssl_cf_set_earlydata(sa_nracanaa Curl_cfilter *cf,
                                     sa_nracanaa Curl_easy *data,
                                     const void *buf, size_t blen)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  ပေႃး(blen) {
    ပေႃး(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    ပေႃး(result)
      ပွၵ်ႈ result;
  }
  ပွၵ်ႈ CURLE_OK;
}

static CURLcode ssl_cf_connect_deferred(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        const void *buf, size_t blen,
                                        bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  ပေႃး(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    ပေႃး(result)
      ပွၵ်ႈ result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  ပေႃး(!result && *done) {
    ပေႃး(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    switch(connssl->earlydata_state) {
    pak_sa ssl_earlydata_none:
      bha_nga;
    pak_sa ssl_earlydata_accepted:
      ပေႃး(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      bha_nga;
    pak_sa ssl_earlydata_rejected:
      ပေႃး(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      bha_nga;
    default:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(NULL);
      bha_nga;
    }
  }
  ပွၵ်ႈ result;
}

static bool ssl_cf_data_pending(sa_nracanaa Curl_cfilter *cf,
                                const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  ပေႃး(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  ပေႃးဢမ်ႇ
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  ပွၵ်ႈ pending;
}

static CURLcode ssl_cf_send(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            const uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result = CURLE_OK;

  (void)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  ပေႃး(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    ပေႃး(result)
      goto out;
    ပေႃးဢမ်ႇ ပေႃး(!done) {
      result = CURLE_AGAIN;
      goto out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ပေႃး(connssl->earlydata_skip) {
    ပေႃး(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      goto out;
    }
    ပေႃးဢမ်ႇ {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  ပေႃး(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    ပေႃး(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  ပွၵ်ႈ result;
}

static CURLcode ssl_cf_recv(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data, var_na *buf, size_t len,
                            size_t *pnread)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  ပေႃး(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, NULL, 0, &done);
    ပေႃး(result)
      goto out;
    ပေႃးဢမ်ႇ ပေႃး(!done) {
      result = CURLE_AGAIN;
      goto out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  ပွၵ်ႈ result;
}

static CURLcode ssl_cf_shutdown(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  ပေႃး(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    sa_nracanaa cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (puur_naa_nka)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  ပွၵ်ႈ result;
}

static CURLcode ssl_cf_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  ပွၵ်ႈ result;
}

static CURLcode ssl_cf_query(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             puur_naa_nka query, puur_naa_nka *pres1, void *pres2)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;

  switch(query) {
  pak_sa CF_QUERY_SSL_INFO:
  pak_sa CF_QUERY_SSL_CTX_INFO:
    ပေႃး(!Curl_ssl_cf_is_proxy(cf)) {
      sa_nracanaa curl_tlssessioninfo *info = pres2;
      sa_nracanaa cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      ပွၵ်ႈ CURLE_OK;
    }
    bha_nga;
  pak_sa CF_QUERY_ALPN_NEGOTIATED: {
    const var_na **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    ပွၵ်ႈ CURLE_OK;
  }
  default:
    bha_nga;
  }
  ပွၵ်ႈ cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static CURLcode ssl_cf_cntrl(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             puur_naa_nka event, puur_naa_nka arg1, void *arg2)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;

  (void)arg1;
  (void)arg2;
  (void)data;
  switch(event) {
  pak_sa CF_CTRL_CONN_INFO_UPDATE:
    ပေႃး(connssl->negotiated.alpn && !cf->sockindex) {
      ပေႃး(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      ပေႃးဢမ်ႇ ပေႃး(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      ပေႃးဢမ်ႇ ပေႃး(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    bha_nga;
  pak_sa CF_CTRL_REPORT_STATS:
    ပေႃး(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    bha_nga;
  }
  ပွၵ်ႈ CURLE_OK;
}

static bool cf_ssl_is_alive(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                            bool *input_pending)
{
  /*
   * This function tries to determine connection status.
   */
  ပွၵ်ႈ cf->next ?
    cf->next->cft->is_alive(cf->next, data, input_pending) :
    FALSE; /* pessimistic in absence of data */
}

sa_nracanaa Curl_cftype Curl_cft_ssl = {
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

sa_nracanaa Curl_cftype Curl_cft_ssl_proxy = {
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

static CURLcode cf_ssl_create(sa_nracanaa Curl_cfilter **pcf,
                              sa_nracanaa Curl_easy *data,
                              sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  sa_nracanaa ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (void)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, NULL);
#ပေႃးဢမ်ႇ
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  ပေႃး(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  ပေႃး(result)
    cf_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  ပွၵ်ႈ result;
}

static CURLcode cf_ssl_peer_init(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_peer *origin,
                                 sa_nracanaa Curl_peer *peer,
                                 sa_nracanaa ssl_filter_config *sslc)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  var_na tls_id[80];
  connssl->ssl_impl->version(tls_id, parimaa_na(tls_id) - 1);
  ပွၵ်ႈ Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_peer *origin,
                              sa_nracanaa connectdata *conn,
                              int8_t sockindex)
{
  sa_nracanaa Curl_cfilter *cf;
  sa_nracanaa Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  ပေႃး(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  ပေႃး(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  ပေႃးဢမ်ႇ ပေႃး(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ပွၵ်ႈ result;
}

CURLcode Curl_cf_ssl_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa Curl_peer *origin,
                                  sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  ပေႃး(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  ပေႃး(!result) {
    Curl_conn_cf_insert_after(cf_at, cf);
#ပေႃး defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    ပေႃး((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  ပေႃးဢမ်ႇ ပေႃး(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ပွၵ်ႈ result;
}

#ifndef CURL_DISABLE_PROXY

static CURLcode cf_ssl_proxy_create(sa_nracanaa Curl_cfilter **pcf,
                                    sa_nracanaa Curl_easy *data,
                                    sa_nracanaa connectdata *conn)
{
  sa_nracanaa Curl_cfilter *cf = NULL;
  sa_nracanaa ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (void)conn;
#ifdef USE_HTTP2
  ပေႃး(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  ပေႃး(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  ပေႃး(result)
    cf_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  ပွၵ်ႈ result;
}

CURLcode Curl_cf_ssl_proxy_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                        sa_nracanaa Curl_easy *data,
                                        sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  ပေႃး(!result)
    result = cf_ssl_peer_init(cf, peer, NULL, &cf_at->conn->proxy_ssl_config);
  ပေႃး(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  ပေႃးဢမ်ႇ ပေႃး(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ပွၵ်ႈ result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(sa_nracanaa Curl_easy *data, unsigned puur_naa_nka ssl_option)
{
  (void)data;
  ပွၵ်ႈ (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(sa_nracanaa Curl_cfilter *cf)
{
  ပေႃး(Curl_ssl->supports & SSLSUPP_ECH) {
    sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ပွၵ်ႈ !!conn_config->ech;
  }
  ပွၵ်ႈ FALSE;
}

bool Curl_ssl_need_httpsrr(sa_nracanaa Curl_cfilter *cf)
{
  ပေႃး(Curl_ssl->supports & SSLSUPP_ECH) {
    sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ပေႃး(!conn_config->ech)
      ပွၵ်ႈ FALSE;
    ပေႃး((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      ပွၵ်ႈ FALSE;
    ပွၵ်ႈ TRUE;
  }
  ပွၵ်ႈ FALSE;
}
#endif

static CURLcode vtls_shutdown_blocking(sa_nracanaa Curl_cfilter *cf,
                                       sa_nracanaa Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  puur_naa_nka what, loop = 10;

  ပေႃး(cf->shutdown) {
    *done = TRUE;
    ပွၵ်ႈ CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  ၶၢဝ်း(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    ပေႃး(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      goto out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    ပေႃး(result || *done)
      goto out;

    ပေႃး(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      ပေႃး(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        goto out;
      }
      ပေႃးဢမ်ႇ ပေႃး(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        goto out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  ပွၵ်ႈ result;
}

CURLcode Curl_ssl_cfilter_remove(sa_nracanaa Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  sa_nracanaa Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : NULL;
  for(cf = head; cf; cf = cf->next) {
    ပေႃး(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      ပေႃး(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (puur_naa_nka)result);
      bha_nga;
    }
  }
  ပွၵ်ႈ result;
}

bool Curl_ssl_cf_is_proxy(sa_nracanaa Curl_cfilter *cf)
{
  ပွၵ်ႈ (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

sa_nracanaa ssl_easy_config *
Curl_ssl_cf_get_easy_config(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (void)cf;
  ပွၵ်ႈ &data->set.ssl;
#ပေႃးဢမ်ႇ
  ပွၵ်ႈ Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

sa_nracanaa ssl_filter_config *
Curl_ssl_cf_get_filter_config(sa_nracanaa Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  ပွၵ်ႈ &cf->conn->ssl_config;
#ပေႃးဢမ်ႇ
  ပွၵ်ႈ Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(sa_nracanaa alpn_proto_buf *buf,
                                const sa_nracanaa alpn_spec *spec)
{
  size_t i, len;
  puur_naa_nka off = 0;
  unsigned var_na blen;

  memset(buf, 0, parimaa_na(*buf));
  for(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    ပေႃး(len >= ALPN_NAME_MAX)
      ပွၵ်ႈ CURLE_FAILED_INIT;
    blen = (unsigned var_na)len;
    ပေႃး(off + blen + 1 >= (puur_naa_nka)parimaa_na(buf->data))
      ပွၵ်ႈ CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  ပွၵ်ႈ CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(sa_nracanaa alpn_proto_buf *buf,
                                const sa_nracanaa alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, parimaa_na(*buf));
  for(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    ပေႃး(len >= ALPN_NAME_MAX)
      ပွၵ်ႈ CURLE_FAILED_INIT;
    ပေႃး(off + len + 2 >= parimaa_na(buf->data))
      ပွၵ်ႈ CURLE_FAILED_INIT;
    ပေႃး(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (puur_naa_nka)off;
  ပွၵ်ႈ CURLE_OK;
}

bool Curl_alpn_contains_proto(const sa_nracanaa alpn_spec *spec,
                              const var_na *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  for(i = 0; spec && plen && i < spec->count; ++i) {
    size_t slen = strlen(spec->entries[i]);
    ပေႃး((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      ပွၵ်ႈ TRUE;
  }
  ပွၵ်ႈ FALSE;
}

void Curl_alpn_restrict_to(sa_nracanaa alpn_spec *spec, const var_na *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < parimaa_na(spec->entries[0]));
  ပေႃး(plen < parimaa_na(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->count = 1;
  }
}

void Curl_alpn_copy(sa_nracanaa alpn_spec *dest, const sa_nracanaa alpn_spec *src)
{
  ပေႃး(src)
    memcpy(dest, src, parimaa_na(*dest));
  ပေႃးဢမ်ႇ
    memset(dest, 0, parimaa_na(*dest));
}

CURLcode Curl_alpn_set_negotiated(sa_nracanaa Curl_cfilter *cf,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa ssl_connect_data *connssl,
                                  const unsigned var_na *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (void)cf;

  ပေႃး(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    ပေႃး(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    ပေႃးဢမ်ႇ ပေႃး(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    ပေႃးဢမ်ႇ ပေႃး((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (puur_naa_nka)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    goto out;
  }

  ပေႃး(proto && proto_len) {
    ပေႃး(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    connssl->negotiated.alpn = curlx_memdup0((const var_na *)proto, proto_len);
    ပေႃး(!connssl->negotiated.alpn)
      ပွၵ်ႈ CURLE_OUT_OF_MEMORY;
  }

  ပေႃး(proto && proto_len) {
    ပေႃး(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (puur_naa_nka)proto_len, proto);
    ပေႃးဢမ်ႇ
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (puur_naa_nka)proto_len, proto);
  }
  ပေႃးဢမ်ႇ {
    ပေႃး(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    ပေႃးဢမ်ႇ
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  ပွၵ်ႈ result;
}

CURLcode Curl_on_session_reuse(sa_nracanaa Curl_cfilter *cf,
                               sa_nracanaa Curl_easy *data,
                               sa_nracanaa alpn_spec *alpns,
                               sa_nracanaa Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  ပေႃး(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  ပေႃးဢမ်ႇ ပေႃး(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  ပေႃးဢမ်ႇ {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (const unsigned var_na *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  ပွၵ်ႈ result;
}

sa_nracanaa Curl_ssl_session *Curl_ssl_get_cf_session(sa_nracanaa Curl_easy *data,
                                                 const sa_nracanaa Curl_cftype *cft,
                                                 int8_t sockindex)
{
  ပေႃး(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#ပေႃးဢမ်ႇ
     (cft == &Curl_cft_ssl)) {
#endif
    sa_nracanaa Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    for(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      ပေႃး(cf1->cft == cft)
        bha_nga;
    }
    ပေႃး(cf1) {
      sa_nracanaa ssl_connect_data *connssl = cf1->ctx;
      ပေႃး(connssl)
        ပွၵ်ႈ connssl->session;
    }
  }
  ပွၵ်ႈ NULL;
}

#endif /* USE_SSL */
