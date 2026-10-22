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
#если !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
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
  если(!use_alpn)
    вернуть NULL;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  если(only_http_10 && (wanted & CURL_HTTP_V1x))
    вернуть &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  если(wanted & CURL_HTTP_V2x) {
    если(wanted & CURL_HTTP_V1x)
      вернуть (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    вернуть &ALPN_SPEC_H2;
  }
#иначе
  (void)wanted;
  (void)preferred;
#endif
  вернуть &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
static целое multissl_setup(const struct Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(void)
{
#ifdef USE_SSL
  multissl_setup(NULL);
  вернуть Curl_ssl->info.id;
#иначе
  вернуть CURLSSLBACKEND_NONE;
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
целое Curl_ssl_init(void)
{
  /* make sure this is only done once */
  если(init_ssl)
    вернуть 1;
  init_ssl = TRUE; /* never again */

  если(Curl_ssl->init)
    вернуть Curl_ssl->init();
  вернуть 1;
}

static bool ssl_prefs_check(struct Curl_cfilter *cf,
                            struct Curl_easy *data)
{
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  если(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    вернуть FALSE;
  }

  switch(conn_config->version_max) {
  case CURL_SSLVERSION_MAX_NONE:
  case CURL_SSLVERSION_MAX_DEFAULT:
    break;

  default:
    если((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      вернуть FALSE;
    }
  }

  вернуть TRUE;
}

static struct ssl_connect_data *cf_ctx_new(struct Curl_easy *data,
                                           const struct alpn_spec *alpn)
{
  struct ssl_connect_data *ctx;

  (void)data;
  ctx = curlx_calloc(1, sizeof(*ctx));
  если(!ctx)
    вернуть NULL;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  если(!ctx->backend) {
    curlx_free(ctx);
    вернуть NULL;
  }
  вернуть ctx;
}

static void cf_ctx_free(struct ssl_connect_data *ctx)
{
  если(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(struct Curl_easy *data, int8_t sockindex,
                                      struct dynbuf *binding)
{
  если(Curl_ssl->get_channel_binding)
    вернуть Curl_ssl->get_channel_binding(data, sockindex, binding);
  вернуть CURLE_OK;
}

void Curl_ssl_close_all(struct Curl_easy *data)
{
  если(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 struct easy_pollset *ps)
{
  struct ssl_connect_data *connssl = cf->ctx;

  если(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    если(sock != CURL_SOCKET_BAD) {
      если(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
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
    вернуть result;
  }
  вернуть CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
struct curl_slist *Curl_ssl_engines_list(struct Curl_easy *data)
{
  если(Curl_ssl->engines_list)
    вернуть Curl_ssl->engines_list(data);
  вернуть NULL;
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

  если(ci->num_of_certs) {
    /* free all individual lists used */
    целое i;
    для(i = 0; i < ci->num_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = NULL;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->num_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(struct Curl_easy *data, целое num)
{
  struct curl_certinfo *ci = &data->info.certs;
  struct curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)num, sizeof(struct curl_slist *));
  если(!table)
    вернуть CURLE_OUT_OF_MEMORY;

  ci->num_of_certs = num;
  ci->certinfo = table;

  вернуть CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_push_certinfo_len(struct Curl_easy *data,
                                    целое certnum,
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

  если(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, value, valuelen))
    вернуть CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  если(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  вернуть result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(struct Curl_easy *data,
                         unsigned char *buffer, size_t length)
{
  DEBUGASSERT(length == sizeof(целое));
  если(Curl_ssl->random)
    вернуть Curl_ssl->random(data, buffer, length);
  иначе
    вернуть CURLE_NOT_BUILT_IN;
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
  если(!pem)
    вернуть CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  если(!begin_pos)
    вернуть CURLE_BAD_CONTENT_ENCODING;

  pem_count = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  если(pem_count && '\n' != pem[pem_count - 1])
    вернуть CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_count += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_count, "\n-----END PUBLIC KEY-----");
  если(!end_pos)
    вернуть CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  пока(pem_count < pem_len) {
    если('\n' != pem[pem_count] && '\r' != pem[pem_count]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_count], 1);
      если(result)
        вернуть result;
    }
    ++pem_count;
  }

  если(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  иначе
    result = CURLE_BAD_CONTENT_ENCODING;

  вернуть result;
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
  если(!pinnedpubkey)
    вернуть CURLE_OK;
  если(!pubkey || !pubkeylen)
    вернуть result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  если(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    char *cert_hash = NULL;
    const char *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned char *sha256sumdigest;

    если(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      вернуть result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    если(!sha256sumdigest)
      вернуть CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    если(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    если(encode)
      вернуть encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    пока(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      если(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        break;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (целое)pinned_hash_len, pinned_hash));
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
    если(!fp)
      вернуть result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    если(fseek(fp, 0, SEEK_END))
      goto end;
    filesize = ftell(fp);
    если(fseek(fp, 0, SEEK_SET))
      goto end;
    если(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      goto end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    если(pubkeylen > size)
      goto end;

    /*
     * Read the file into the dynbuf
     */
    left = size;
    do {
      char buffer[1024];
      size_t want = left > sizeof(buffer) ? sizeof(buffer) : left;
      если(fread(buffer, 1, want, fp) != want)
        goto end;
      если(curlx_dyn_addn(&buf, buffer, want))
        goto end;
      left -= want;
    } пока(left);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    если(pubkeylen == size) {
      если(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      goto end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    если(pem_read)
      goto end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    если(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  вернуть result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(void)
{
  если(Curl_ssl->cert_status_request)
    вернуть Curl_ssl->cert_status_request();
  вернуть FALSE;
}

static целое multissl_init(void)
{
  если(multissl_setup(NULL))
    вернуть 1;
  если(Curl_ssl->init)
    вернуть Curl_ssl->init();
  вернуть 1;
}

static CURLcode multissl_random(struct Curl_easy *data,
                                unsigned char *entropy, size_t length)
{
  если(multissl_setup(NULL))
    вернуть CURLE_FAILED_INIT;
  вернуть Curl_ssl->random(data, entropy, length);
}

static CURLcode multissl_connect(struct Curl_cfilter *cf,
                                 struct Curl_easy *data, bool *done)
{
  если(multissl_setup(NULL))
    вернуть CURLE_FAILED_INIT;
  вернуть Curl_ssl->do_connect(cf, data, done);
}

static CURLcode multissl_adjust_pollset(struct Curl_cfilter *cf,
                                        struct Curl_easy *data,
                                        struct easy_pollset *ps)
{
  если(multissl_setup(NULL))
    вернуть CURLE_OK;
  вернуть Curl_ssl->adjust_pollset(cf, data, ps);
}

static void *multissl_get_internals(struct ssl_connect_data *connssl,
                                    CURLINFO info)
{
  если(multissl_setup(NULL))
    вернуть NULL;
  вернуть Curl_ssl->get_internals(connssl, info);
}

static void multissl_close(struct Curl_cfilter *cf, struct Curl_easy *data)
{
  если(multissl_setup(NULL))
    вернуть;
  Curl_ssl->close(cf, data);
}

static CURLcode multissl_recv_plain(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    char *buf, size_t len, size_t *pnread)
{
  если(multissl_setup(NULL))
    вернуть CURLE_FAILED_INIT;
  вернуть Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

static CURLcode multissl_send_plain(struct Curl_cfilter *cf,
                                    struct Curl_easy *data,
                                    const void *mem, size_t len,
                                    size_t *pnwritten)
{
  если(multissl_setup(NULL))
    вернуть CURLE_FAILED_INIT;
  вернуть Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
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
  если(init_ssl) {
    /* only cleanup if we did a previous init */
    если(Curl_ssl->cleanup)
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

  если(current != selected) {
    char *p = backends;
    const char *end = backends + sizeof(backends);
    целое i;

    selected = current;

    backends[0] = '\0';

    для(i = 0; available_backends[i]; ++i) {
      char vb[200];
      bool paren = (selected != available_backends[i]);

      если(available_backends[i]->version(vb, sizeof(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  если(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  вернуть 0;
}

static целое multissl_setup(const struct Curl_ssl *backend)
{
  целое i;
  char *env;

  если(Curl_ssl != &Curl_ssl_multi)
    вернуть 1;

  если(backend) {
    Curl_ssl = backend;
    вернуть 0;
  }

  если(!available_backends[0])
    вернуть 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  если(env) {
    для(i = 0; available_backends[i]; i++) {
      если(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        вернуть 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  для(i = 0; available_backends[i]; i++) {
    если(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      вернуть 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  вернуть 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, const char *name,
                                   const curl_ssl_backend ***avail)
{
  целое i;

  если(avail)
    *avail = (const curl_ssl_backend **)&available_backends;

  если(Curl_ssl != &Curl_ssl_multi)
    вернуть id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#иначе
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  для(i = 0; available_backends[i]; i++) {
    если(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      вернуть CURLSSLSET_OK;
    }
  }

  вернуть CURLSSLSET_UNKNOWN_BACKEND;
}

#иначе /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, const char *name,
                                   const curl_ssl_backend ***avail)
{
  (void)id;
  (void)name;
  (void)avail;
  вернуть CURLSSLSET_NO_BACKENDS;
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
  если(hostname && hostname[0]) {
#ifdef USE_IPV6
    struct in6_addr addr;
#иначе
    struct in_addr addr;
#endif
    если(curlx_inet_pton(AF_INET, hostname, &addr))
      вернуть CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    иначе если(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      вернуть CURL_SSL_PEER_IPV6;
    }
#endif
  }
  вернуть CURL_SSL_PEER_DNS;
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
  если(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    вернуть CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  если(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    если(len && (origin->hostname[len - 1] == '.'))
      len--;
    если(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      если(!ssl_peer->sni)
        goto out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_key_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_key);

out:
  если(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  вернуть result;
}

static void ssl_cf_destroy(struct Curl_cfilter *cf, struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  если(connssl) {
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

  если(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    вернуть CURLE_OK;
  }

  если(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    вернуть CURLE_FAILED_INIT;
  }

  если(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    если(result || !*done)
      вернуть result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  если(!connssl->prefs_checked) {
    если(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  если(!result && *done) {
    cf->connected = TRUE;
    если(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    если(Curl_tls_keylog_enabled()) {
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (целое)result, *done);
  CF_DATA_RESTORE(cf, save);
  вернуть result;
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
  если(blen) {
    если(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    если(result)
      вернуть result;
  }
  вернуть CURLE_OK;
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
  если(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    если(result)
      вернуть result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  если(!result && *done) {
    если(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    switch(connssl->earlydata_state) {
    case ssl_earlydata_none:
      break;
    case ssl_earlydata_accepted:
      если(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      break;
    case ssl_earlydata_rejected:
      если(!Curl_ssl_cf_is_proxy(cf))
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
  вернуть result;
}

static bool ssl_cf_data_pending(struct Curl_cfilter *cf,
                                const struct Curl_easy *data)
{
  struct ssl_connect_data *connssl = cf->ctx;
  struct cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  если(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  иначе
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  вернуть pending;
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

  если(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    если(result)
      goto out;
    иначе если(!done) {
      result = CURLE_AGAIN;
      goto out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  если(connssl->earlydata_skip) {
    если(connssl->earlydata_skip >= blen) {
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
  если(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    если(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  вернуть result;
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
  если(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, NULL, 0, &done);
    если(result)
      goto out;
    иначе если(!done) {
      result = CURLE_AGAIN;
      goto out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  вернуть result;
}

static CURLcode ssl_cf_shutdown(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                bool *done)
{
  struct ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  если(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    struct cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (целое)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  вернуть result;
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
  вернуть result;
}

static CURLcode ssl_cf_query(struct Curl_cfilter *cf,
                             struct Curl_easy *data,
                             целое query, целое *pres1, void *pres2)
{
  struct ssl_connect_data *connssl = cf->ctx;

  switch(query) {
  case CF_QUERY_SSL_INFO:
  case CF_QUERY_SSL_CTX_INFO:
    если(!Curl_ssl_cf_is_proxy(cf)) {
      struct curl_tlssessioninfo *info = pres2;
      struct cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      вернуть CURLE_OK;
    }
    break;
  case CF_QUERY_ALPN_NEGOTIATED: {
    const char **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    вернуть CURLE_OK;
  }
  default:
    break;
  }
  вернуть cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

static CURLcode ssl_cf_cntrl(struct Curl_cfilter *cf,
                             struct Curl_easy *data,
                             целое event, целое arg1, void *arg2)
{
  struct ssl_connect_data *connssl = cf->ctx;

  (void)arg1;
  (void)arg2;
  (void)data;
  switch(event) {
  case CF_CTRL_CONN_INFO_UPDATE:
    если(connssl->negotiated.alpn && !cf->sockindex) {
      если(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      иначе если(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      иначе если(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    break;
  case CF_CTRL_REPORT_STATS:
    если(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    break;
  }
  вернуть CURLE_OK;
}

static bool cf_ssl_is_alive(struct Curl_cfilter *cf, struct Curl_easy *data,
                            bool *input_pending)
{
  /*
   * This function tries to determine connection status.
   */
  вернуть cf->next ?
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
  если(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  если(result)
    cf_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  вернуть result;
}

static CURLcode cf_ssl_peer_init(struct Curl_cfilter *cf,
                                 struct Curl_peer *origin,
                                 struct Curl_peer *peer,
                                 struct ssl_filter_config *sslc)
{
  struct ssl_connect_data *connssl = cf->ctx;
  char tls_id[80];
  connssl->ssl_impl->version(tls_id, sizeof(tls_id) - 1);
  вернуть Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
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
  если(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  если(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  иначе если(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  вернуть result;
}

CURLcode Curl_cf_ssl_insert_after(struct Curl_cfilter *cf_at,
                                  struct Curl_easy *data,
                                  struct Curl_peer *origin,
                                  struct Curl_peer *peer)
{
  struct Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  если(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  если(!result) {
    Curl_conn_cf_insert_after(cf_at, cf);
#если defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    если((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  иначе если(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  вернуть result;
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
  если(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  если(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    goto out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  если(result)
    cf_ctx_free(ctx);
  *pcf = result ? NULL : cf;
  вернуть result;
}

CURLcode Curl_cf_ssl_proxy_insert_after(struct Curl_cfilter *cf_at,
                                        struct Curl_easy *data,
                                        struct Curl_peer *peer)
{
  struct Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  если(!result)
    result = cf_ssl_peer_init(cf, peer, NULL, &cf_at->conn->proxy_ssl_config);
  если(!result)
    Curl_conn_cf_insert_after(cf_at, cf);
  иначе если(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  вернуть result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(struct Curl_easy *data, unsigned целое ssl_option)
{
  (void)data;
  вернуть (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(struct Curl_cfilter *cf)
{
  если(Curl_ssl->supports & SSLSUPP_ECH) {
    struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    вернуть !!conn_config->ech;
  }
  вернуть FALSE;
}

bool Curl_ssl_need_httpsrr(struct Curl_cfilter *cf)
{
  если(Curl_ssl->supports & SSLSUPP_ECH) {
    struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    если(!conn_config->ech)
      вернуть FALSE;
    если((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      вернуть FALSE;
    вернуть TRUE;
  }
  вернуть FALSE;
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
  целое what, loop = 10;

  если(cf->shutdown) {
    *done = TRUE;
    вернуть CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  пока(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    если(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      goto out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    если(result || *done)
      goto out;

    если(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      если(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        goto out;
      }
      иначе если(what == 0) {
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
  вернуть result;
}

CURLcode Curl_ssl_cfilter_remove(struct Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  struct Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : NULL;
  для(cf = head; cf; cf = cf->next) {
    если(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      если(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (целое)result);
      break;
    }
  }
  вернуть result;
}

bool Curl_ssl_cf_is_proxy(struct Curl_cfilter *cf)
{
  вернуть (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

struct ssl_easy_config *
Curl_ssl_cf_get_easy_config(struct Curl_cfilter *cf,
                            struct Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (void)cf;
  вернуть &data->set.ssl;
#иначе
  вернуть Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

struct ssl_filter_config *
Curl_ssl_cf_get_filter_config(struct Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  вернуть &cf->conn->ssl_config;
#иначе
  вернуть Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(struct alpn_proto_buf *buf,
                                const struct alpn_spec *spec)
{
  size_t i, len;
  целое off = 0;
  unsigned char blen;

  memset(buf, 0, sizeof(*buf));
  для(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    если(len >= ALPN_NAME_MAX)
      вернуть CURLE_FAILED_INIT;
    blen = (unsigned char)len;
    если(off + blen + 1 >= (целое)sizeof(buf->data))
      вернуть CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  вернуть CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(struct alpn_proto_buf *buf,
                                const struct alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, sizeof(*buf));
  для(i = 0; spec && i < spec->count; ++i) {
    len = strlen(spec->entries[i]);
    если(len >= ALPN_NAME_MAX)
      вернуть CURLE_FAILED_INIT;
    если(off + len + 2 >= sizeof(buf->data))
      вернуть CURLE_FAILED_INIT;
    если(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (целое)off;
  вернуть CURLE_OK;
}

bool Curl_alpn_contains_proto(const struct alpn_spec *spec,
                              const char *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  для(i = 0; spec && plen && i < spec->count; ++i) {
    size_t slen = strlen(spec->entries[i]);
    если((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      вернуть TRUE;
  }
  вернуть FALSE;
}

void Curl_alpn_restrict_to(struct alpn_spec *spec, const char *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < sizeof(spec->entries[0]));
  если(plen < sizeof(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->count = 1;
  }
}

void Curl_alpn_copy(struct alpn_spec *dest, const struct alpn_spec *src)
{
  если(src)
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

  если(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    если(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    иначе если(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    иначе если((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (целое)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    goto out;
  }

  если(proto && proto_len) {
    если(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      goto out;
    }
    connssl->negotiated.alpn = curlx_memdup0((const char *)proto, proto_len);
    если(!connssl->negotiated.alpn)
      вернуть CURLE_OUT_OF_MEMORY;
  }

  если(proto && proto_len) {
    если(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (целое)proto_len, proto);
    иначе
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (целое)proto_len, proto);
  }
  иначе {
    если(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    иначе
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  вернуть result;
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

  если(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  иначе если(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
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
  вернуть result;
}

struct Curl_ssl_session *Curl_ssl_get_cf_session(struct Curl_easy *data,
                                                 const struct Curl_cftype *cft,
                                                 int8_t sockindex)
{
  если(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#иначе
     (cft == &Curl_cft_ssl)) {
#endif
    struct Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    для(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      если(cf1->cft == cft)
        break;
    }
    если(cf1) {
      struct ssl_connect_data *connssl = cf1->ctx;
      если(connssl)
        вернуть connssl->session;
    }
  }
  вернуть NULL;
}

#endif /* USE_SSL */
