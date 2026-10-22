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
#ако !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
static const struct alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
static const struct alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
static const struct alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
static const struct alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
static const struct alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

static const struct alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  ако(!use_alpn)
    върни NULL;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  ако(only_http_10 && (wanted & CURL_HTTP_V1x))
    върни &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  ако(wanted & CURL_HTTP_V2x) {
    ако(wanted & CURL_HTTP_V1x)
      върни (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    върни &ALPN_SPEC_H2;
  }
#иначе
  (void)wanted;
  (void)preferred;
#endif
  върни &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
static цяло multissl_setup(const struct Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(void)
{
#ifdef USE_SSL
  multissl_setup(NULL);
  върни Curl_ssl->info.id;
#иначе
  върни CURLSSLBACKEND_NONE;
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
цяло Curl_ssl_init(void)
{
  /* make sure this is only done once */
  ако(init_ssl)
    върни 1;
  init_ssl = TRUE; /* never again */

  ако(Curl_ssl->init)
    върни Curl_ssl->init();
  върни 1;
}

static bool ssl_prefs_check(struct Curl_cfilter *cf,
                            struct Curl_easy *data)
{
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  ако(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    върни FALSE;
  }

  switch(conn_config->version_max) {
  case CURL_SSLVERSION_MAX_NONE:
  case CURL_SSLVERSION_MAX_DEFAULT:
    break;

  default:
    ако((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      върни FALSE;
    }
  }

  върни TRUE;
}

static struct ssl_connect_data *cf_ctx_new(struct Curl_easy *data,
                                           const struct alpn_spec *alpn)
{
  struct ssl_connect_data *ctx;

  (void)data;
  ctx = curlx_calloc(1, sizeof(*ctx));
  ако(!ctx)
    върни NULL;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  ако(!ctx->backend) {
    curlx_free(ctx);
    върни NULL;
  }
  върни ctx;
}

static void cf_ctx_free(struct ssl_connect_data *ctx)
{
  ако(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(struct Curl_easy *data, int8_t sockindex,
                                      struct dynbuf *binding)
{
  ако(Curl_ssl->get_channel_binding)
    върни Curl_ssl->get_channel_binding(data, sockindex, binding);
  върни CURLE_OK;
}

void Curl_ssl_close_all(struct Curl_easy *data)
{
  ако(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 struct easy_pollset *ps)
{
  struct ssl_connect_data *connssl = cf->ctx;

  ако(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    ако(sock != CURL_SOCKET_BAD) {
      ако(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      иначе {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    върни result;
  }
  върни CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
struct curl_slist *Curl_ssl_engines_list(struct Curl_easy *data)
{
  ако(Curl_ssl->engines_list)
    върни Curl_ssl->engines_list(data);
  върни NULL;
}

static size_t multissl_version(char *buffer, size_t size);

void Curl_ssl_version(char *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (void)multissl_version(buffer, size);
#иначе
  (void)Curl_ssl->version(buffer, size);
#endif
}

void Curl_ssl_free_certinfo(struct Curl_easy *data)
{
  struct curl_certinfo *ci = &data->info.certs;

  ако(ci->num_of_certs) {
    /* free all individual lists used */
    цяло i;
    за(i = 0; i < ci->num_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = NULL;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->num_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(struct Curl_easy *data, цяло num)
{
  struct curl_certinfo *ci = &data->info.certs;
  struct curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)num, sizeof(struct curl_slist *));
  ако(!table)
    върни CURLE_OUT_OF_MEMORY;

  ci->num_of_certs = num;
  ci->certinfo = table;

  върни CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_push_certinfo_len(struct Curl_easy *data,
                                    цяло certnum,
                                    const char *label,
                                    const char *value,
                                    size_t valuelen)
{
  struct curl_certinfo *ci = &data->info.certs;
  struct curl_slist *nl;
  CURLcode result = CURLE_OK;
  struct dynbuf build;

  DEBUGASSERT(certnum < ci->num_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  ако(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, value, valuelen))
    върни CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  ако(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  върни result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(struct Curl_easy *data,
                         unsigned char *buffer, size_t length)
{
  DEBUGASSERT(length == sizeof(цяло));
  ако(Curl_ssl->random)
    върни Curl_ssl->random(data, buffer, length);
  иначе
    върни CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

static CURLcode pubkey_pem_to_der(const char *pem,
                                  unsigned char **der, size_t *der_len)
{
  const char *begin_pos, *end_pos;
  size_t pem_count, pem_len;
  CURLcode result;
  struct dynbuf pbuf;

  /* if no pem, exit. */
  ако(!pem)
    върни CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  ако(!begin_pos)
    върни CURLE_BAD_CONTENT_ENCODING;

  pem_count = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  ако(pem_count && '\n' != pem[pem_count - 1])
    върни CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_count += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_count, "\n-----END PUBLIC KEY-----");
  ако(!end_pos)
    върни CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  докато(pem_count < pem_len) {
    ако('\n' != pem[pem_count] && '\r' != pem[pem_count]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_count], 1);
      ако(result)
        върни result;
    }
    ++pem_count;
  }

  ако(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  иначе
    result = CURLE_BAD_CONTENT_ENCODING;

  върни result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(struct Curl_easy *data,
                              const char *pinnedpubkey,
                              const unsigned char *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ако(!pinnedpubkey)
    върни CURLE_OK;
  ако(!pubkey || !pubkeylen)
    върни result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  ако(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    char *cert_hash = NULL;
    const char *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned char *sha256sumdigest;

    ако(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      върни result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    ако(!sha256sumdigest)
      върни CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    ако(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    ако(encode)
      върни encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    докато(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      ако(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        break;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (цяло)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : NULL;
    }
    curlx_safefree(cert_hash);
  }
  иначе {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    struct dynbuf buf;
    unsigned char *pem_ptr = NULL;
    size_t left;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    ако(!fp)
      върни result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    ако(fseek(fp, 0, SEEK_END))
      goto end;
    filesize = ftell(fp);
    ако(fseek(fp, 0, SEEK_SET))
      goto end;
    ако(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      goto end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    ако(pubkeylen > size)
      goto end;

    /*
     * Read the file into the dynbuf
     */
    left = size;
    do {
      char buffer[1024];
      size_t want = left > sizeof(buffer) ? sizeof(buffer) : left;
      ако(fread(buffer, 1, want, fp) != want)
        goto end;
      ако(curlx_dyn_addn(&buf, buffer, want))
        goto end;
      left -= want;
    } докато(left);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    ако(pubkeylen == size) {
      ако(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      goto end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    ако(pem_read)
      goto end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    ако(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  върни result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(void)
{
  ако(Curl_ssl->cert_status_request)
    върни Curl_ssl->cert_status_request();
  върни FALSE;
}

static цяло multissl_init(void)
{
  ако(multissl_setup(NULL))
    върни 1;
  ако(Curl_ssl->init)
    върни Curl_ssl->init();
  върни 1;
}

static CURLcode multissl_random(struct Curl_easy *data,
                                unsigned char *entropy, size_t length)
{
  ако(multissl_setup(NULL))
    върни CURLE_FAILED_INIT;
  върни Curl_ssl->random(data, entropy, length);
}

static CURLcode multissl_connect(struct Curl_cfilter *cf,
                                 struct Curl_easy *data, bool *done)
{
  ако(multissl_setup(NULL))
    върни CURLE_FAILED_INIT;
  върни Curl_ssl->do_connect(cf, data, done);
}

static CURLcode multissl_adjust_pollset(struct Curl_cfilter *cf,
                                        struct Curl_easy *data,
                                        struct easy_pollset *ps)
{
  ако(multissl_setup(NULL))
    върни CURLE_OK;
  върни Curl_ssl->adjust_pollset(cf, data, ps);
}

static void *multissl_get_internals(struct ssl_connect_data *connssl,
                                    CURLINFO info)
{
  ако(multissl_setup(NULL))
    върни NULL;
  върни Curl_ssl->get_internals(connssl, info);
}

static void multissl_close(struct Curl_cfilter *cf, struct Curl_easy *data)
{
  ако(multissl_setup(NULL))
    върни;
  Curl_ssl->close(cf, data);
}

static CURLcode multissl_recv_plain(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    char *buf, size_t len, size_t *pnread)
{
  ако(multissl_setup(NULL))
    върни CURLE_FAILED_INIT;
  върни Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

static CURLcode multissl_send_plain(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    const void *mem, size_t len,
                                    size_t *pnwritten)
{
  ако(multissl_setup(NULL))
    върни CURLE_FAILED_INIT;
  върни Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

static const struct Curl_ssl Curl_ssl_multi = {
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

const struct Curl_ssl *Curl_ssl =
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
#иначе
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

static const struct Curl_ssl *available_backends[] = {
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
  ако(init_ssl) {
    /* only cleanup if we did a previous init */
    ако(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

static size_t multissl_version(char *buffer, size_t size)
{
  static const struct Curl_ssl *selected;
  static char backends[200];
  static size_t backends_len;
  const struct Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  ако(current != selected) {
    char *p = backends;
    const char *end = backends + sizeof(backends);
    цяло i;

    selected = current;

    backends[0] = '\0';

    за(i = 0; available_backends[i]; ++i) {
      char vb[200];
      bool paren = (selected != available_backends[i]);

      ако(available_backends[i]->version(vb, sizeof(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  ако(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  върни 0;
}

static цяло multissl_setup(const struct Curl_ssl *backend)
{
  цяло i;
  char *env;

  ако(Curl_ssl != &Curl_ssl_multi)
    върни 1;

  ако(backend) {
    Curl_ssl = backend;
    върни 0;
  }

  ако(!available_backends[0])
    върни 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  ако(env) {
    за(i = 0; available_backends[i]; i++) {
      ако(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        върни 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  за(i = 0; available_backends[i]; i++) {
    ако(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      върни 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  върни 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, const char *name,
                                   const curl_ssl_backend ***avail)
{
  цяло i;

  ако(avail)
    *avail = (const curl_ssl_backend **)&available_backends;

  ако(Curl_ssl != &Curl_ssl_multi)
    върни id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#иначе
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  за(i = 0; available_backends[i]; i++) {
    ако(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      върни CURLSSLSET_OK;
    }
  }

  върни CURLSSLSET_UNKNOWN_BACKEND;
}

#иначе /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, const char *name,
                                   const curl_ssl_backend ***avail)
{
  (void)id;
  (void)name;
  (void)avail;
  върни CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

void Curl_ssl_peer_cleanup(struct ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_key);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

static ssl_peer_type get_peer_type(const char *hostname)
{
  ако(hostname && hostname[0]) {
#ifdef USE_IPV6
    struct in6_addr addr;
#иначе
    struct in_addr addr;
#endif
    ако(curlx_inet_pton(AF_INET, hostname, &addr))
      върни CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    иначе ако(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      върни CURL_SSL_PEER_IPV6;
    }
#endif
  }
  върни CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(struct ssl_peer *ssl_peer,
                            struct Curl_peer *origin,
                            struct Curl_peer *peer,
                            struct ssl_filter_config *sslc,
                            const char *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  ако(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    върни CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  ако(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    ако(len && (origin->hostname[len - 1] == '.'))
      len--;
    ако(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      ако(!ssl_peer->sni)
        goto out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_key_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_key);

out:
  ако(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  върни result;
}

static void ssl_cf_destroy(struct Curl_cfilter *cf, struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  ако(connssl) {
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

static CURLcode ssl_cf_connect(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               bool *done)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct cf_call_data save;
  CURLcode result;

  ако(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    върни CURLE_OK;
  }

  ако(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    върни CURLE_FAILED_INIT;
  }

  ако(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    ако(result || !*done)
      върни result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  ако(!connssl->prefs_checked) {
    ако(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  ако(!result && *done) {
    cf->connected = TRUE;
    ако(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    ако(Curl_tls_keylog_enabled()) {
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (цяло)result, *done);
  CF_DATA_RESTORE(cf, save);
  върни result;
}

static CURLcode ssl_cf_set_earlydata(struct Curl_cfilter *cf,
                                     struct Curl_easy *data,
                                     const void *buf, size_t blen)
{
  struct ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  ако(blen) {
    ако(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    ако(result)
      върни result;
  }
  върни CURLE_OK;
}

static CURLcode ssl_cf_connect_deferred(struct Curl_cfilter *cf,
                                        struct Curl_easy *data,
                                        const void *buf, size_t blen,
                                        bool *done)
{
  struct ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  ако(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    ако(result)
      върни result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  ако(!result && *done) {
    ако(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    switch(connssl->earlydata_state) {
    case ssl_earlydata_none:
      break;
    case ssl_earlydata_accepted:
      ако(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      break;
    case ssl_earlydata_rejected:
      ако(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      break;
    default:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(NULL);
      break;
    }
  }
  върни result;
}

static bool ssl_cf_data_pending(struct Curl_cfilter *cf,
                                const struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  ако(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  иначе
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  върни pending;
}

static CURLcode ssl_cf_send(struct Curl_cfilter *cf,
                            struct Curl_easy *data,
                            const uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct cf_call_data save;
  CURLcode result = CURLE_OK;

  (void)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  ако(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    ако(result)
      goto out;
    иначе ако(!done) {
      result = CURLE_AGAIN;
      goto out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ако(connssl->earlydata_skip) {
    ако(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      goto out;
    }
    иначе {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  ако(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    ако(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  върни result;
}

static CURLcode ssl_cf_recv(struct Curl_cfilter *cf,
                            struct Curl_easy *data, char *buf, size_t len,
                            size_t *pnread)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  ако(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, NULL, 0, &done);
    ако(result)
      goto out;
    иначе ако(!done) {
      result = CURLE_AGAIN;
      goto out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  върни result;
}

static CURLcode ssl_cf_shutdown(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                bool *done)
{
  struct ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  ако(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    struct cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (цяло)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  върни result;
}

static CURLcode ssl_cf_adjust_pollset(struct Curl_cfilter *cf,
                                      struct Curl_easy *data,
                                      struct easy_pollset *ps)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  върни result;
}

static CURLcode ssl_cf_query(struct Curl_cfilter *cf,
                             struct Curl_easy *data,
                             цяло query, цяло *pres1, void *pres2)
{
  struct ssl_connect_data *connssl = cf->ctx;

  switch(query) {
  case CF_QUERY_SSL_INFO:
  case CF_QUERY_SSL_CTX_INFO:
    ако(!Curl_ssl_cf_is_proxy(cf)) {
      struct curl_tlssessioninfo *info = pres2;
      struct cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      върни CURLE_OK;
    }
    break;
  case CF_QUERY_ALPN_NEGOTIATED: {
    const char **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    върни CURLE_OK;
  }
  default:
    break;
  }
  върни cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static CURLcode ssl_cf_cntrl(struct Curl_cfilter *cf,
                             struct Curl_easy *data,
                             цяло event, цяло arg1, void *arg2)
{
  struct ssl_connect_data *connssl = cf->ctx;

  (void)arg1;
  (void)arg2;
  (void)data;
  switch(event) {
  case CF_CTRL_CONN_INFO_UPDATE:
    ако(connssl->negotiated.alpn && !cf->sockindex) {
      ако(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      иначе ако(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      иначе ако(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    break;
  case CF_CTRL_REPORT_STATS:
    ако(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    break;
  }
  върни CURLE_OK;
}

static bool cf_ssl_is_alive(struct Curl_cfilter *cf, struct Curl_easy *data,
                            bool *input_pending)
{
  /*
   * This function tries to determine connection status.
   */
  върни cf->next ?
    cf->next->cft->is_alive(cf->next, data, input_pending) :
    FALSE; /* pessimistic in absence of data */
}

struct Curl_cftype Curl_cft_ssl = {
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

struct Curl_cftype Curl_cft_ssl_proxy = {
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

static CURLcode cf_ssl_create(struct Curl_cfilter **pcf,
                              struct Curl_easy *data,
                              struct connectdata *conn)
{
  struct Curl_cfilter *cf = NULL;
  struct ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (void)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, NULL);
#иначе
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  ако(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  ако(result)
    cf_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  върни result;
}

static CURLcode cf_ssl_peer_init(struct Curl_cfilter *cf,
                                 struct Curl_peer *origin,
                                 struct Curl_peer *peer,
                                 struct ssl_filter_config *sslc)
{
  struct ssl_connect_data *connssl = cf->ctx;
  char tls_id[80];
  connssl->ssl_impl->version(tls_id, sizeof(tls_id) - 1);
  върни Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(struct Curl_easy *data,
                              struct Curl_peer *origin,
                              struct connectdata *conn,
                              int8_t sockindex)
{
  struct Curl_cfilter *cf;
  struct Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  ако(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  ако(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  иначе ако(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  върни result;
}

CURLcode Curl_cf_ssl_insert_after(struct Curl_cfilter *cf_at,
                                  struct Curl_easy *data,
                                  struct Curl_peer *origin,
                                  struct Curl_peer *peer)
{
  struct Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  ако(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  ако(!result) {
    Curl_conn_cf_insert_after(cf_at, cf);
#ако defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    ако((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  иначе ако(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  върни result;
}

#ifndef CURL_DISABLE_PROXY

static CURLcode cf_ssl_proxy_create(struct Curl_cfilter **pcf,
                                    struct Curl_easy *data,
                                    struct connectdata *conn)
{
  struct Curl_cfilter *cf = NULL;
  struct ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (void)conn;
#ifdef USE_HTTP2
  ако(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  ако(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  ако(result)
    cf_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  върни result;
}

CURLcode Curl_cf_ssl_proxy_insert_after(struct Curl_cfilter *cf_at,
                                        struct Curl_easy *data,
                                        struct Curl_peer *peer)
{
  struct Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  ако(!result)
    result = cf_ssl_peer_init(cf, peer, NULL, &cf_at->conn->proxy_ssl_config);
  ако(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  иначе ако(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  върни result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(struct Curl_easy *data, unsigned цяло ssl_option)
{
  (void)data;
  върни (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(struct Curl_cfilter *cf)
{
  ако(Curl_ssl->supports & SSLSUPP_ECH) {
    struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    върни !!conn_config->ech;
  }
  върни FALSE;
}

bool Curl_ssl_need_httpsrr(struct Curl_cfilter *cf)
{
  ако(Curl_ssl->supports & SSLSUPP_ECH) {
    struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ако(!conn_config->ech)
      върни FALSE;
    ако((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      върни FALSE;
    върни TRUE;
  }
  върни FALSE;
}
#endif

static CURLcode vtls_shutdown_blocking(struct Curl_cfilter *cf,
                                       struct Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  цяло what, loop = 10;

  ако(cf->shutdown) {
    *done = TRUE;
    върни CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  докато(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    ако(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      goto out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    ако(result || *done)
      goto out;

    ако(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      ако(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        goto out;
      }
      иначе ако(what == 0) {
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
  върни result;
}

CURLcode Curl_ssl_cfilter_remove(struct Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  struct Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : NULL;
  за(cf = head; cf; cf = cf->next) {
    ако(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      ако(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (цяло)result);
      break;
    }
  }
  върни result;
}

bool Curl_ssl_cf_is_proxy(struct Curl_cfilter *cf)
{
  върни (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

struct ssl_easy_config *
Curl_ssl_cf_get_easy_config(struct Curl_cfilter *cf,
                            struct Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (void)cf;
  върни &data->set.ssl;
#иначе
  върни Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

struct ssl_filter_config *
Curl_ssl_cf_get_filter_config(struct Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  върни &cf->conn->ssl_config;
#иначе
  върни Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(struct alpn_proto_buf *buf,
                                const struct alpn_spec *spec)
{
  size_t i, len;
  цяло off = 0;
  unsigned char blen;

  memset(buf, 0, sizeof(*buf));
  за(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    ако(len >= ALPN_NAME_MAX)
      върни CURLE_FAILED_INIT;
    blen = (unsigned char)len;
    ако(off + blen + 1 >= (цяло)sizeof(buf->data))
      върни CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  върни CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(struct alpn_proto_buf *buf,
                                const struct alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, sizeof(*buf));
  за(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    ако(len >= ALPN_NAME_MAX)
      върни CURLE_FAILED_INIT;
    ако(off + len + 2 >= sizeof(buf->data))
      върни CURLE_FAILED_INIT;
    ако(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (цяло)off;
  върни CURLE_OK;
}

bool Curl_alpn_contains_proto(const struct alpn_spec *spec,
                              const char *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  за(i = 0; spec && plen && i < spec->count; ++i) {
    size_t slen = strlen(spec->entries[i]);
    ако((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      върни TRUE;
  }
  върни FALSE;
}

void Curl_alpn_restrict_to(struct alpn_spec *spec, const char *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < sizeof(spec->entries[0]));
  ако(plen < sizeof(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->count = 1;
  }
}

void Curl_alpn_copy(struct alpn_spec *dest, const struct alpn_spec *src)
{
  ако(src)
    memcpy(dest, src, sizeof(*dest));
  иначе
    memset(dest, 0, sizeof(*dest));
}

CURLcode Curl_alpn_set_negotiated(struct Curl_cfilter *cf,
                                  struct Curl_easy *data,
                                  struct ssl_connect_data *connssl,
                                  const unsigned char *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (void)cf;

  ако(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    ако(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    иначе ако(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    иначе ако((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (цяло)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    goto out;
  }

  ако(proto && proto_len) {
    ако(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    connssl->negotiated.alpn = curlx_memdup0((const char *)proto, proto_len);
    ако(!connssl->negotiated.alpn)
      върни CURLE_OUT_OF_MEMORY;
  }

  ако(proto && proto_len) {
    ако(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (цяло)proto_len, proto);
    иначе
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (цяло)proto_len, proto);
  }
  иначе {
    ако(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    иначе
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  върни result;
}

CURLcode Curl_on_session_reuse(struct Curl_cfilter *cf,
                               struct Curl_easy *data,
                               struct alpn_spec *alpns,
                               struct Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  struct ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  ако(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  иначе ако(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  иначе {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (const unsigned char *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  върни result;
}

struct Curl_ssl_session *Curl_ssl_get_cf_session(struct Curl_easy *data,
                                                 const struct Curl_cftype *cft,
                                                 int8_t sockindex)
{
  ако(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#иначе
     (cft == &Curl_cft_ssl)) {
#endif
    struct Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    за(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      ако(cf1->cft == cft)
        break;
    }
    ако(cf1) {
      struct ssl_connect_data *connssl = cf1->ctx;
      ако(connssl)
        върни connssl->session;
    }
  }
  върни NULL;
}

#endif /* USE_SSL */
