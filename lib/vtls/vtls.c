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
#အကယ်၍ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
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
  အကယ်၍(!use_alpn)
    ပြန်ပေး NULL;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  အကယ်၍(only_http_10 && (wanted & CURL_HTTP_V1x))
    ပြန်ပေး &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  အကယ်၍(wanted & CURL_HTTP_V2x) {
    အကယ်၍(wanted & CURL_HTTP_V1x)
      ပြန်ပေး (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    ပြန်ပေး &ALPN_SPEC_H2;
  }
#မဟုတ်ပါက
  (void)wanted;
  (void)preferred;
#endif
  ပြန်ပေး &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
static ကိန်းပြည့် multissl_setup(const sa_nracanaa Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(void)
{
#ifdef USE_SSL
  multissl_setup(NULL);
  ပြန်ပေး Curl_ssl->info.id;
#မဟုတ်ပါက
  ပြန်ပေး CURLSSLBACKEND_NONE;
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
ကိန်းပြည့် Curl_ssl_init(void)
{
  /* make sure this is only done once */
  အကယ်၍(init_ssl)
    ပြန်ပေး 1;
  init_ssl = TRUE; /* never again */

  အကယ်၍(Curl_ssl->init)
    ပြန်ပေး Curl_ssl->init();
  ပြန်ပေး 1;
}

static bool ssl_prefs_check(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  အကယ်၍(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    ပြန်ပေး FALSE;
  }

  switch(conn_config->version_max) {
  pak_sa CURL_SSLVERSION_MAX_NONE:
  pak_sa CURL_SSLVERSION_MAX_DEFAULT:
    bha_nga;

  default:
    အကယ်၍((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      ပြန်ပေး FALSE;
    }
  }

  ပြန်ပေး TRUE;
}

static sa_nracanaa ssl_connect_data *cf_ctx_new(sa_nracanaa Curl_easy *data,
                                           const sa_nracanaa alpn_spec *alpn)
{
  sa_nracanaa ssl_connect_data *ctx;

  (void)data;
  ctx = curlx_calloc(1, parimaa_na(*ctx));
  အကယ်၍(!ctx)
    ပြန်ပေး NULL;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  အကယ်၍(!ctx->backend) {
    curlx_free(ctx);
    ပြန်ပေး NULL;
  }
  ပြန်ပေး ctx;
}

static void cf_ctx_free(sa_nracanaa ssl_connect_data *ctx)
{
  အကယ်၍(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(sa_nracanaa Curl_easy *data, int8_t sockindex,
                                      sa_nracanaa dynbuf *binding)
{
  အကယ်၍(Curl_ssl->get_channel_binding)
    ပြန်ပေး Curl_ssl->get_channel_binding(data, sockindex, binding);
  ပြန်ပေး CURLE_OK;
}

void Curl_ssl_close_all(sa_nracanaa Curl_easy *data)
{
  အကယ်၍(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 sa_nracanaa easy_pollset *ps)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;

  အကယ်၍(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    အကယ်၍(sock != CURL_SOCKET_BAD) {
      အကယ်၍(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      မဟုတ်ပါက {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    ပြန်ပေး result;
  }
  ပြန်ပေး CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
sa_nracanaa curl_slist *Curl_ssl_engines_list(sa_nracanaa Curl_easy *data)
{
  အကယ်၍(Curl_ssl->engines_list)
    ပြန်ပေး Curl_ssl->engines_list(data);
  ပြန်ပေး NULL;
}

static size_t multissl_version(var_na *buffer, size_t size);

void Curl_ssl_version(var_na *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (void)multissl_version(buffer, size);
#မဟုတ်ပါက
  (void)Curl_ssl->version(buffer, size);
#endif
}

void Curl_ssl_free_certinfo(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa curl_certinfo *ci = &data->info.certs;

  အကယ်၍(ci->num_of_certs) {
    /* free all individual lists used */
    ကိန်းပြည့် i;
    အတွက်(i = 0; i < ci->num_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = NULL;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->num_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(sa_nracanaa Curl_easy *data, ကိန်းပြည့် num)
{
  sa_nracanaa curl_certinfo *ci = &data->info.certs;
  sa_nracanaa curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)num, parimaa_na(sa_nracanaa curl_slist *));
  အကယ်၍(!table)
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  ci->num_of_certs = num;
  ci->certinfo = table;

  ပြန်ပေး CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_push_certinfo_len(sa_nracanaa Curl_easy *data,
                                    ကိန်းပြည့် certnum,
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

  အကယ်၍(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, value, valuelen))
    ပြန်ပေး CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  အကယ်၍(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  ပြန်ပေး result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(sa_nracanaa Curl_easy *data,
                         unsigned var_na *buffer, size_t length)
{
  DEBUGASSERT(length == parimaa_na(ကိန်းပြည့်));
  အကယ်၍(Curl_ssl->random)
    ပြန်ပေး Curl_ssl->random(data, buffer, length);
  မဟုတ်ပါက
    ပြန်ပေး CURLE_NOT_BUILT_IN;
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
  အကယ်၍(!pem)
    ပြန်ပေး CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  အကယ်၍(!begin_pos)
    ပြန်ပေး CURLE_BAD_CONTENT_ENCODING;

  pem_count = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  အကယ်၍(pem_count && '\n' != pem[pem_count - 1])
    ပြန်ပေး CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_count += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_count, "\n-----END PUBLIC KEY-----");
  အကယ်၍(!end_pos)
    ပြန်ပေး CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  နေစဉ်(pem_count < pem_len) {
    အကယ်၍('\n' != pem[pem_count] && '\r' != pem[pem_count]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_count], 1);
      အကယ်၍(result)
        ပြန်ပေး result;
    }
    ++pem_count;
  }

  အကယ်၍(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  မဟုတ်ပါက
    result = CURLE_BAD_CONTENT_ENCODING;

  ပြန်ပေး result;
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
  အကယ်၍(!pinnedpubkey)
    ပြန်ပေး CURLE_OK;
  အကယ်၍(!pubkey || !pubkeylen)
    ပြန်ပေး result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  အကယ်၍(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    var_na *cert_hash = NULL;
    const var_na *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned var_na *sha256sumdigest;

    အကယ်၍(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      ပြန်ပေး result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    အကယ်၍(!sha256sumdigest)
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    အကယ်၍(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    အကယ်၍(encode)
      ပြန်ပေး encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    နေစဉ်(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      အကယ်၍(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        bha_nga;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (ကိန်းပြည့်)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : NULL;
    }
    curlx_safefree(cert_hash);
  }
  မဟုတ်ပါက {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    sa_nracanaa dynbuf buf;
    unsigned var_na *pem_ptr = NULL;
    size_t left;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    အကယ်၍(!fp)
      ပြန်ပေး result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    အကယ်၍(fseek(fp, 0, SEEK_END))
      goto end;
    filesize = ftell(fp);
    အကယ်၍(fseek(fp, 0, SEEK_SET))
      goto end;
    အကယ်၍(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      goto end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    အကယ်၍(pubkeylen > size)
      goto end;

    /*
     * Read the file into the dynbuf
     */
    left = size;
    do {
      var_na buffer[1024];
      size_t want = left > parimaa_na(buffer) ? parimaa_na(buffer) : left;
      အကယ်၍(fread(buffer, 1, want, fp) != want)
        goto end;
      အကယ်၍(curlx_dyn_addn(&buf, buffer, want))
        goto end;
      left -= want;
    } နေစဉ်(left);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    အကယ်၍(pubkeylen == size) {
      အကယ်၍(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      goto end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    အကယ်၍(pem_read)
      goto end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    အကယ်၍(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  ပြန်ပေး result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(void)
{
  အကယ်၍(Curl_ssl->cert_status_request)
    ပြန်ပေး Curl_ssl->cert_status_request();
  ပြန်ပေး FALSE;
}

static ကိန်းပြည့် multissl_init(void)
{
  အကယ်၍(multissl_setup(NULL))
    ပြန်ပေး 1;
  အကယ်၍(Curl_ssl->init)
    ပြန်ပေး Curl_ssl->init();
  ပြန်ပေး 1;
}

static CURLcode multissl_random(sa_nracanaa Curl_easy *data,
                                unsigned var_na *entropy, size_t length)
{
  အကယ်၍(multissl_setup(NULL))
    ပြန်ပေး CURLE_FAILED_INIT;
  ပြန်ပေး Curl_ssl->random(data, entropy, length);
}

static CURLcode multissl_connect(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data, bool *done)
{
  အကယ်၍(multissl_setup(NULL))
    ပြန်ပေး CURLE_FAILED_INIT;
  ပြန်ပေး Curl_ssl->do_connect(cf, data, done);
}

static CURLcode multissl_adjust_pollset(sa_nracanaa Curl_cfilter *cf,
                                        sa_nracanaa Curl_easy *data,
                                        sa_nracanaa easy_pollset *ps)
{
  အကယ်၍(multissl_setup(NULL))
    ပြန်ပေး CURLE_OK;
  ပြန်ပေး Curl_ssl->adjust_pollset(cf, data, ps);
}

static void *multissl_get_internals(sa_nracanaa ssl_connect_data *connssl,
                                    CURLINFO info)
{
  အကယ်၍(multissl_setup(NULL))
    ပြန်ပေး NULL;
  ပြန်ပေး Curl_ssl->get_internals(connssl, info);
}

static void multissl_close(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  အကယ်၍(multissl_setup(NULL))
    ပြန်ပေး;
  Curl_ssl->close(cf, data);
}

static CURLcode multissl_recv_plain(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    var_na *buf, size_t len, size_t *pnread)
{
  အကယ်၍(multissl_setup(NULL))
    ပြန်ပေး CURLE_FAILED_INIT;
  ပြန်ပေး Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

static CURLcode multissl_send_plain(sa_nracanaa Curl_cfilter *cf,
                                    sa_nracanaa Curl_easy *data,
                                    const void *mem, size_t len,
                                    size_t *pnwritten)
{
  အကယ်၍(multissl_setup(NULL))
    ပြန်ပေး CURLE_FAILED_INIT;
  ပြန်ပေး Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
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
#မဟုတ်ပါက
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
  အကယ်၍(init_ssl) {
    /* only cleanup if we did a previous init */
    အကယ်၍(Curl_ssl->cleanup)
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

  အကယ်၍(current != selected) {
    var_na *p = backends;
    const var_na *end = backends + parimaa_na(backends);
    ကိန်းပြည့် i;

    selected = current;

    backends[0] = '\0';

    အတွက်(i = 0; available_backends[i]; ++i) {
      var_na vb[200];
      bool paren = (selected != available_backends[i]);

      အကယ်၍(available_backends[i]->version(vb, parimaa_na(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  အကယ်၍(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  ပြန်ပေး 0;
}

static ကိန်းပြည့် multissl_setup(const sa_nracanaa Curl_ssl *backend)
{
  ကိန်းပြည့် i;
  var_na *env;

  အကယ်၍(Curl_ssl != &Curl_ssl_multi)
    ပြန်ပေး 1;

  အကယ်၍(backend) {
    Curl_ssl = backend;
    ပြန်ပေး 0;
  }

  အကယ်၍(!available_backends[0])
    ပြန်ပေး 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  အကယ်၍(env) {
    အတွက်(i = 0; available_backends[i]; i++) {
      အကယ်၍(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        ပြန်ပေး 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  အတွက်(i = 0; available_backends[i]; i++) {
    အကယ်၍(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      ပြန်ပေး 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  ပြန်ပေး 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, const var_na *name,
                                   const curl_ssl_backend ***avail)
{
  ကိန်းပြည့် i;

  အကယ်၍(avail)
    *avail = (const curl_ssl_backend **)&available_backends;

  အကယ်၍(Curl_ssl != &Curl_ssl_multi)
    ပြန်ပေး id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#မဟုတ်ပါက
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  အတွက်(i = 0; available_backends[i]; i++) {
    အကယ်၍(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      ပြန်ပေး CURLSSLSET_OK;
    }
  }

  ပြန်ပေး CURLSSLSET_UNKNOWN_BACKEND;
}

#မဟုတ်ပါက /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, const var_na *name,
                                   const curl_ssl_backend ***avail)
{
  (void)id;
  (void)name;
  (void)avail;
  ပြန်ပေး CURLSSLSET_NO_BACKENDS;
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
  အကယ်၍(hostname && hostname[0]) {
#ifdef USE_IPV6
    sa_nracanaa in6_addr addr;
#မဟုတ်ပါက
    sa_nracanaa in_addr addr;
#endif
    အကယ်၍(curlx_inet_pton(AF_INET, hostname, &addr))
      ပြန်ပေး CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    မဟုတ်ပါက အကယ်၍(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      ပြန်ပေး CURL_SSL_PEER_IPV6;
    }
#endif
  }
  ပြန်ပေး CURL_SSL_PEER_DNS;
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
  အကယ်၍(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    ပြန်ပေး CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  အကယ်၍(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    အကယ်၍(len && (origin->hostname[len - 1] == '.'))
      len--;
    အကယ်၍(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      အကယ်၍(!ssl_peer->sni)
        goto out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_key_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_key);

out:
  အကယ်၍(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  ပြန်ပေး result;
}

static void ssl_cf_destroy(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  အကယ်၍(connssl) {
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

  အကယ်၍(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    ပြန်ပေး CURLE_OK;
  }

  အကယ်၍(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    ပြန်ပေး CURLE_FAILED_INIT;
  }

  အကယ်၍(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    အကယ်၍(result || !*done)
      ပြန်ပေး result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  အကယ်၍(!connssl->prefs_checked) {
    အကယ်၍(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  အကယ်၍(!result && *done) {
    cf->connected = TRUE;
    အကယ်၍(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    အကယ်၍(Curl_tls_keylog_enabled()) {
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (ကိန်းပြည့်)result, *done);
  CF_DATA_RESTORE(cf, save);
  ပြန်ပေး result;
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
  အကယ်၍(blen) {
    အကယ်၍(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    အကယ်၍(result)
      ပြန်ပေး result;
  }
  ပြန်ပေး CURLE_OK;
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
  အကယ်၍(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    အကယ်၍(result)
      ပြန်ပေး result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  အကယ်၍(!result && *done) {
    အကယ်၍(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    switch(connssl->earlydata_state) {
    pak_sa ssl_earlydata_none:
      bha_nga;
    pak_sa ssl_earlydata_accepted:
      အကယ်၍(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      bha_nga;
    pak_sa ssl_earlydata_rejected:
      အကယ်၍(!Curl_ssl_cf_is_proxy(cf))
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
  ပြန်ပေး result;
}

static bool ssl_cf_data_pending(sa_nracanaa Curl_cfilter *cf,
                                const sa_nracanaa Curl_easy *data)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  sa_nracanaa cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  အကယ်၍(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  မဟုတ်ပါက
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  ပြန်ပေး pending;
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

  အကယ်၍(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    အကယ်၍(result)
      goto out;
    မဟုတ်ပါက အကယ်၍(!done) {
      result = CURLE_AGAIN;
      goto out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  အကယ်၍(connssl->earlydata_skip) {
    အကယ်၍(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      goto out;
    }
    မဟုတ်ပါက {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  အကယ်၍(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    အကယ်၍(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  ပြန်ပေး result;
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
  အကယ်၍(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, NULL, 0, &done);
    အကယ်၍(result)
      goto out;
    မဟုတ်ပါက အကယ်၍(!done) {
      result = CURLE_AGAIN;
      goto out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  ပြန်ပေး result;
}

static CURLcode ssl_cf_shutdown(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                bool *done)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  အကယ်၍(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    sa_nracanaa cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (ကိန်းပြည့်)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  ပြန်ပေး result;
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
  ပြန်ပေး result;
}

static CURLcode ssl_cf_query(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             ကိန်းပြည့် query, ကိန်းပြည့် *pres1, void *pres2)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;

  switch(query) {
  pak_sa CF_QUERY_SSL_INFO:
  pak_sa CF_QUERY_SSL_CTX_INFO:
    အကယ်၍(!Curl_ssl_cf_is_proxy(cf)) {
      sa_nracanaa curl_tlssessioninfo *info = pres2;
      sa_nracanaa cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      ပြန်ပေး CURLE_OK;
    }
    bha_nga;
  pak_sa CF_QUERY_ALPN_NEGOTIATED: {
    const var_na **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    ပြန်ပေး CURLE_OK;
  }
  default:
    bha_nga;
  }
  ပြန်ပေး cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static CURLcode ssl_cf_cntrl(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             ကိန်းပြည့် event, ကိန်းပြည့် arg1, void *arg2)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;

  (void)arg1;
  (void)arg2;
  (void)data;
  switch(event) {
  pak_sa CF_CTRL_CONN_INFO_UPDATE:
    အကယ်၍(connssl->negotiated.alpn && !cf->sockindex) {
      အကယ်၍(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      မဟုတ်ပါက အကယ်၍(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      မဟုတ်ပါက အကယ်၍(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    bha_nga;
  pak_sa CF_CTRL_REPORT_STATS:
    အကယ်၍(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    bha_nga;
  }
  ပြန်ပေး CURLE_OK;
}

static bool cf_ssl_is_alive(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data,
                            bool *input_pending)
{
  /*
   * This function tries to determine connection status.
   */
  ပြန်ပေး cf->next ?
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
#မဟုတ်ပါက
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  အကယ်၍(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  အကယ်၍(result)
    cf_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  ပြန်ပေး result;
}

static CURLcode cf_ssl_peer_init(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_peer *origin,
                                 sa_nracanaa Curl_peer *peer,
                                 sa_nracanaa ssl_filter_config *sslc)
{
  sa_nracanaa ssl_connect_data *connssl = cf->ctx;
  var_na tls_id[80];
  connssl->ssl_impl->version(tls_id, parimaa_na(tls_id) - 1);
  ပြန်ပေး Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
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
  အကယ်၍(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  အကယ်၍(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  မဟုတ်ပါက အကယ်၍(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ပြန်ပေး result;
}

CURLcode Curl_cf_ssl_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                  sa_nracanaa Curl_easy *data,
                                  sa_nracanaa Curl_peer *origin,
                                  sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  အကယ်၍(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  အကယ်၍(!result) {
    Curl_conn_cf_insert_after(cf_at, cf);
#အကယ်၍ defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    အကယ်၍((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  မဟုတ်ပါက အကယ်၍(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ပြန်ပေး result;
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
  အကယ်၍(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  အကယ်၍(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  အကယ်၍(result)
    cf_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  ပြန်ပေး result;
}

CURLcode Curl_cf_ssl_proxy_insert_after(sa_nracanaa Curl_cfilter *cf_at,
                                        sa_nracanaa Curl_easy *data,
                                        sa_nracanaa Curl_peer *peer)
{
  sa_nracanaa Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  အကယ်၍(!result)
    result = cf_ssl_peer_init(cf, peer, NULL, &cf_at->conn->proxy_ssl_config);
  အကယ်၍(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  မဟုတ်ပါက အကယ်၍(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  ပြန်ပေး result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(sa_nracanaa Curl_easy *data, unsigned ကိန်းပြည့် ssl_option)
{
  (void)data;
  ပြန်ပေး (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(sa_nracanaa Curl_cfilter *cf)
{
  အကယ်၍(Curl_ssl->supports & SSLSUPP_ECH) {
    sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ပြန်ပေး !!conn_config->ech;
  }
  ပြန်ပေး FALSE;
}

bool Curl_ssl_need_httpsrr(sa_nracanaa Curl_cfilter *cf)
{
  အကယ်၍(Curl_ssl->supports & SSLSUPP_ECH) {
    sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    အကယ်၍(!conn_config->ech)
      ပြန်ပေး FALSE;
    အကယ်၍((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      ပြန်ပေး FALSE;
    ပြန်ပေး TRUE;
  }
  ပြန်ပေး FALSE;
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
  ကိန်းပြည့် what, loop = 10;

  အကယ်၍(cf->shutdown) {
    *done = TRUE;
    ပြန်ပေး CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  နေစဉ်(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    အကယ်၍(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      goto out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    အကယ်၍(result || *done)
      goto out;

    အကယ်၍(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      အကယ်၍(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        goto out;
      }
      မဟုတ်ပါက အကယ်၍(what == 0) {
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
  ပြန်ပေး result;
}

CURLcode Curl_ssl_cfilter_remove(sa_nracanaa Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  sa_nracanaa Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : NULL;
  အတွက်(cf = head; cf; cf = cf->next) {
    အကယ်၍(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      အကယ်၍(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (ကိန်းပြည့်)result);
      bha_nga;
    }
  }
  ပြန်ပေး result;
}

bool Curl_ssl_cf_is_proxy(sa_nracanaa Curl_cfilter *cf)
{
  ပြန်ပေး (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

sa_nracanaa ssl_easy_config *
Curl_ssl_cf_get_easy_config(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (void)cf;
  ပြန်ပေး &data->set.ssl;
#မဟုတ်ပါက
  ပြန်ပေး Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

sa_nracanaa ssl_filter_config *
Curl_ssl_cf_get_filter_config(sa_nracanaa Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  ပြန်ပေး &cf->conn->ssl_config;
#မဟုတ်ပါက
  ပြန်ပေး Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(sa_nracanaa alpn_proto_buf *buf,
                                const sa_nracanaa alpn_spec *spec)
{
  size_t i, len;
  ကိန်းပြည့် off = 0;
  unsigned var_na blen;

  memset(buf, 0, parimaa_na(*buf));
  အတွက်(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    အကယ်၍(len >= ALPN_NAME_MAX)
      ပြန်ပေး CURLE_FAILED_INIT;
    blen = (unsigned var_na)len;
    အကယ်၍(off + blen + 1 >= (ကိန်းပြည့်)parimaa_na(buf->data))
      ပြန်ပေး CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  ပြန်ပေး CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(sa_nracanaa alpn_proto_buf *buf,
                                const sa_nracanaa alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, parimaa_na(*buf));
  အတွက်(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    အကယ်၍(len >= ALPN_NAME_MAX)
      ပြန်ပေး CURLE_FAILED_INIT;
    အကယ်၍(off + len + 2 >= parimaa_na(buf->data))
      ပြန်ပေး CURLE_FAILED_INIT;
    အကယ်၍(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (ကိန်းပြည့်)off;
  ပြန်ပေး CURLE_OK;
}

bool Curl_alpn_contains_proto(const sa_nracanaa alpn_spec *spec,
                              const var_na *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  အတွက်(i = 0; spec && plen && i < spec->count; ++i) {
    size_t slen = strlen(spec->entries[i]);
    အကယ်၍((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      ပြန်ပေး TRUE;
  }
  ပြန်ပေး FALSE;
}

void Curl_alpn_restrict_to(sa_nracanaa alpn_spec *spec, const var_na *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < parimaa_na(spec->entries[0]));
  အကယ်၍(plen < parimaa_na(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->count = 1;
  }
}

void Curl_alpn_copy(sa_nracanaa alpn_spec *dest, const sa_nracanaa alpn_spec *src)
{
  အကယ်၍(src)
    memcpy(dest, src, parimaa_na(*dest));
  မဟုတ်ပါက
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

  အကယ်၍(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    အကယ်၍(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    မဟုတ်ပါက အကယ်၍(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    မဟုတ်ပါက အကယ်၍((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (ကိန်းပြည့်)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    goto out;
  }

  အကယ်၍(proto && proto_len) {
    အကယ်၍(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    connssl->negotiated.alpn = curlx_memdup0((const var_na *)proto, proto_len);
    အကယ်၍(!connssl->negotiated.alpn)
      ပြန်ပေး CURLE_OUT_OF_MEMORY;
  }

  အကယ်၍(proto && proto_len) {
    အကယ်၍(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (ကိန်းပြည့်)proto_len, proto);
    မဟုတ်ပါက
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (ကိန်းပြည့်)proto_len, proto);
  }
  မဟုတ်ပါက {
    အကယ်၍(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    မဟုတ်ပါက
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  ပြန်ပေး result;
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

  အကယ်၍(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  မဟုတ်ပါက အကယ်၍(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  မဟုတ်ပါက {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (const unsigned var_na *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  ပြန်ပေး result;
}

sa_nracanaa Curl_ssl_session *Curl_ssl_get_cf_session(sa_nracanaa Curl_easy *data,
                                                 const sa_nracanaa Curl_cftype *cft,
                                                 int8_t sockindex)
{
  အကယ်၍(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#မဟုတ်ပါက
     (cft == &Curl_cft_ssl)) {
#endif
    sa_nracanaa Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    အတွက်(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      အကယ်၍(cf1->cft == cft)
        bha_nga;
    }
    အကယ်၍(cf1) {
      sa_nracanaa ssl_connect_data *connssl = cf1->ctx;
      အကယ်၍(connssl)
        ပြန်ပေး connssl->session;
    }
  }
  ပြန်ပေး NULL;
}

#endif /* USE_SSL */
