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

#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"

#અન્તર્ભાવ "vtls/vtls.h" /* generic SSL protos etc */
#અન્તર્ભાવ "vtls/vtls_int.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "vtls/keylog.h"

#અન્તર્ભાવ "vtls/openssl.h"        /* OpenSSL versions */
#અન્તર્ભાવ "vtls/gtls.h"           /* GnuTLS versions */
#અન્તર્ભાવ "vtls/wolfssl.h"        /* wolfSSL versions */
#અન્તર્ભાવ "vtls/schannel.h"       /* Schannel SSPI version */
#અન્તર્ભાવ "vtls/mbedtls.h"        /* mbedTLS versions */
#અન્તર્ભાવ "vtls/rustls.h"         /* Rustls versions */

#અન્તર્ભાવ "slist.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "progress.h"
#અન્તર્ભાવ "curlx/win32-fopen.h"
#અન્તર્ભાવ "curl_sha256.h"
#અન્તર્ભાવ "curlx/base64.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "setopt.h"
#અન્તર્ભાવ "vdns/cf-dns.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "curlx/strcopy.h"

#ifdef USE_APPLE_SECTRUST
#અન્તર્ભાવ <Security/Security.h>
#endif

#ifdef USE_SSL
#જો !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
સ્થિર અચલ સંરચના alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
સ્થિર અચલ સંરચના alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
સ્થિર અચલ સંરચના alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
સ્થિર અચલ સંરચના alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
સ્થિર અચલ સંરચના alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

સ્થિર અચલ સંરચના alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  જો(!use_alpn)
    પાછા ખાલી;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  જો(only_http_10 && (wanted & CURL_HTTP_V1x))
    પાછા &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  જો(wanted & CURL_HTTP_V2x) {
    જો(wanted & CURL_HTTP_V1x)
      પાછા (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    પાછા &ALPN_SPEC_H2;
  }
#નહીંતર
  (શૂન્ય)wanted;
  (શૂન્ય)preferred;
#endif
  પાછા &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
સ્થિર પૂર્ણાંક multissl_setup(અચલ સંરચના Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(શૂન્ય)
{
#ifdef USE_SSL
  multissl_setup(ખાલી);
  પાછા Curl_ssl->info.id;
#નહીંતર
  પાછા CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
સ્થિર bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
પૂર્ણાંક Curl_ssl_init(શૂન્ય)
{
  /* make sure this is only done once */
  જો(init_ssl)
    પાછા 1;
  init_ssl = TRUE; /* never again */

  જો(Curl_ssl->init)
    પાછા Curl_ssl->init();
  પાછા 1;
}

સ્થિર bool ssl_prefs_check(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data)
{
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  જો(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    પાછા FALSE;
  }

  પસંદગી(conn_config->version_max) {
  સ્થિતિ CURL_SSLVERSION_MAX_NONE:
  સ્થિતિ CURL_SSLVERSION_MAX_DEFAULT:
    તોડો;

  મૂળભૂત:
    જો((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      પાછા FALSE;
    }
  }

  પાછા TRUE;
}

સ્થિર સંરચના ssl_connect_data *cf_ctx_new(સંરચના Curl_easy *data,
                                           અચલ સંરચના alpn_spec *alpn)
{
  સંરચના ssl_connect_data *ctx;

  (શૂન્ય)data;
  ctx = curlx_calloc(1, કદ(*ctx));
  જો(!ctx)
    પાછા ખાલી;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  જો(!ctx->backend) {
    curlx_free(ctx);
    પાછા ખાલી;
  }
  પાછા ctx;
}

સ્થિર શૂન્ય cf_ctx_free(સંરચના ssl_connect_data *ctx)
{
  જો(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(સંરચના Curl_easy *data, int8_t sockindex,
                                      સંરચના dynbuf *binding)
{
  જો(Curl_ssl->get_channel_binding)
    પાછા Curl_ssl->get_channel_binding(data, sockindex, binding);
  પાછા CURLE_OK;
}

શૂન્ય Curl_ssl_close_all(સંરચના Curl_easy *data)
{
  જો(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 સંરચના easy_pollset *ps)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;

  જો(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    જો(sock != CURL_SOCKET_BAD) {
      જો(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        result = Curl_pollset_set_out_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      નહીંતર {
        result = Curl_pollset_set_in_only(data, ps, sock);
        CURL_TRC_CF(data, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    પાછા result;
  }
  પાછા CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
સંરચના curl_slist *Curl_ssl_engines_list(સંરચના Curl_easy *data)
{
  જો(Curl_ssl->engines_list)
    પાછા Curl_ssl->engines_list(data);
  પાછા ખાલી;
}

સ્થિર size_t multissl_version(અક્ષર *buffer, size_t size);

શૂન્ય Curl_ssl_version(અક્ષર *buffer, size_t size)
{
#ifdef CURL_WITH_MULTI_SSL
  (શૂન્ય)multissl_version(buffer, size);
#નહીંતર
  (શૂન્ય)Curl_ssl->version(buffer, size);
#endif
}

શૂન્ય Curl_ssl_free_certinfo(સંરચના Curl_easy *data)
{
  સંરચના curl_certinfo *ci = &data->info.certs;

  જો(ci->અંક_of_certs) {
    /* free all individual lists used */
    પૂર્ણાંક i;
    માટે(i = 0; i < ci->અંક_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = ખાલી;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->અંક_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(સંરચના Curl_easy *data, પૂર્ણાંક અંક)
{
  સંરચના curl_certinfo *ci = &data->info.certs;
  સંરચના curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)અંક, કદ(સંરચના curl_slist *));
  જો(!table)
    પાછા CURLE_OUT_OF_MEMORY;

  ci->અંક_of_certs = અંક;
  ci->certinfo = table;

  પાછા CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ધકેલો_certinfo_len(સંરચના Curl_easy *data,
                                    પૂર્ણાંક certnum,
                                    અચલ અક્ષર *label,
                                    અચલ અક્ષર *કિંમત,
                                    size_t valuelen)
{
  સંરચના curl_certinfo *ci = &data->info.certs;
  સંરચના curl_slist *nl;
  CURLcode result = CURLE_OK;
  સંરચના dynbuf build;

  DEBUGASSERT(certnum < ci->અંક_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  જો(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, કિંમત, valuelen))
    પાછા CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  જો(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  પાછા result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(સંરચના Curl_easy *data,
                         unsigned અક્ષર *buffer, size_t length)
{
  DEBUGASSERT(length == કદ(પૂર્ણાંક));
  જો(Curl_ssl->random)
    પાછા Curl_ssl->random(data, buffer, length);
  નહીંતર
    પાછા CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

સ્થિર CURLcode pubkey_pem_to_der(અચલ અક્ષર *pem,
                                  unsigned અક્ષર **der, size_t *der_len)
{
  અચલ અક્ષર *begin_pos, *end_pos;
  size_t pem_ગણતરી, pem_len;
  CURLcode result;
  સંરચના dynbuf pbuf;

  /* if no pem, exit. */
  જો(!pem)
    પાછા CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  જો(!begin_pos)
    પાછા CURLE_BAD_CONTENT_ENCODING;

  pem_ગણતરી = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  જો(pem_ગણતરી && '\n' != pem[pem_ગણતરી - 1])
    પાછા CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_ગણતરી += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_ગણતરી, "\n-----END PUBLIC KEY-----");
  જો(!end_pos)
    પાછા CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  જ્યારે(pem_ગણતરી < pem_len) {
    જો('\n' != pem[pem_ગણતરી] && '\r' != pem[pem_ગણતરી]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_ગણતરી], 1);
      જો(result)
        પાછા result;
    }
    ++pem_ગણતરી;
  }

  જો(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  નહીંતર
    result = CURLE_BAD_CONTENT_ENCODING;

  પાછા result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(સંરચના Curl_easy *data,
                              અચલ અક્ષર *pinnedpubkey,
                              અચલ unsigned અક્ષર *pubkey, size_t pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  જો(!pinnedpubkey)
    પાછા CURLE_OK;
  જો(!pubkey || !pubkeylen)
    પાછા result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  જો(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    અક્ષર *cert_hash = ખાલી;
    અચલ અક્ષર *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned અક્ષર *sha256sumdigest;

    જો(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      પાછા result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    જો(!sha256sumdigest)
      પાછા CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    જો(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    જો(encode)
      પાછા encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    જ્યારે(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      જો(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        તોડો;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (પૂર્ણાંક)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : ખાલી;
    }
    curlx_safefree(cert_hash);
  }
  નહીંતર {
    long filesize;
    size_t size, pem_len;
    CURLcode pem_read;
    સંરચના dynbuf buf;
    unsigned અક્ષર *pem_ptr = ખાલી;
    size_t ડાબું;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    જો(!fp)
      પાછા result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    જો(fseek(fp, 0, SEEK_END))
      જાઓ end;
    filesize = ftell(fp);
    જો(fseek(fp, 0, SEEK_SET))
      જાઓ end;
    જો(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      જાઓ end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    જો(pubkeylen > size)
      જાઓ end;

    /*
     * Read the file into the dynbuf
     */
    ડાબું = size;
    કરો {
      અક્ષર buffer[1024];
      size_t want = ડાબું > કદ(buffer) ? કદ(buffer) : ડાબું;
      જો(fread(buffer, 1, want, fp) != want)
        જાઓ end;
      જો(curlx_dyn_addn(&buf, buffer, want))
        જાઓ end;
      ડાબું -= want;
    } જ્યારે(ડાબું);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    જો(pubkeylen == size) {
      જો(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      જાઓ end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    જો(pem_read)
      જાઓ end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    જો(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  પાછા result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(શૂન્ય)
{
  જો(Curl_ssl->cert_status_request)
    પાછા Curl_ssl->cert_status_request();
  પાછા FALSE;
}

સ્થિર પૂર્ણાંક multissl_init(શૂન્ય)
{
  જો(multissl_setup(ખાલી))
    પાછા 1;
  જો(Curl_ssl->init)
    પાછા Curl_ssl->init();
  પાછા 1;
}

સ્થિર CURLcode multissl_random(સંરચના Curl_easy *data,
                                unsigned અક્ષર *entropy, size_t length)
{
  જો(multissl_setup(ખાલી))
    પાછા CURLE_FAILED_INIT;
  પાછા Curl_ssl->random(data, entropy, length);
}

સ્થિર CURLcode multissl_connect(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data, bool *done)
{
  જો(multissl_setup(ખાલી))
    પાછા CURLE_FAILED_INIT;
  પાછા Curl_ssl->do_connect(cf, data, done);
}

સ્થિર CURLcode multissl_adjust_pollset(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        સંરચના easy_pollset *ps)
{
  જો(multissl_setup(ખાલી))
    પાછા CURLE_OK;
  પાછા Curl_ssl->adjust_pollset(cf, data, ps);
}

સ્થિર શૂન્ય *multissl_get_internals(સંરચના ssl_connect_data *connssl,
                                    CURLINFO info)
{
  જો(multissl_setup(ખાલી))
    પાછા ખાલી;
  પાછા Curl_ssl->get_internals(connssl, info);
}

સ્થિર શૂન્ય multissl_close(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  જો(multissl_setup(ખાલી))
    પાછા;
  Curl_ssl->close(cf, data);
}

સ્થિર CURLcode multissl_recv_plain(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    અક્ષર *buf, size_t len, size_t *pnread)
{
  જો(multissl_setup(ખાલી))
    પાછા CURLE_FAILED_INIT;
  પાછા Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

સ્થિર CURLcode multissl_send_plain(સંરચના Curl_cfilter *cf,
                                    સંરચના Curl_easy *data,
                                    અચલ શૂન્ય *mem, size_t len,
                                    size_t *pnwritten)
{
  જો(multissl_setup(ખાલી))
    પાછા CURLE_FAILED_INIT;
  પાછા Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

સ્થિર અચલ સંરચના Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  ખાલી,                              /* cleanup */
  multissl_version,                  /* version */
  ખાલી,                              /* shutdown */
  ખાલી,                              /* data_pending */
  multissl_random,                   /* random */
  ખાલી,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  ખાલી,                              /* close_all */
  ખાલી,                              /* engines_list */
  ખાલી,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  ખાલી,                              /* get_channel_binding */
};

અચલ સંરચના Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#અથવાજો defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#અથવાજો defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#અથવાજો defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#અથવાજો defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#અથવાજો defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#અથવાજો defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#નહીંતર
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

સ્થિર અચલ સંરચના Curl_ssl *available_backends[] = {
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
  ખાલી
};

/* Global cleanup */
શૂન્ય Curl_ssl_cleanup(શૂન્ય)
{
  જો(init_ssl) {
    /* only cleanup if we did a previous init */
    જો(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

સ્થિર size_t multissl_version(અક્ષર *buffer, size_t size)
{
  સ્થિર અચલ સંરચના Curl_ssl *selected;
  સ્થિર અક્ષર backends[200];
  સ્થિર size_t backends_len;
  અચલ સંરચના Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  જો(current != selected) {
    અક્ષર *p = backends;
    અચલ અક્ષર *end = backends + કદ(backends);
    પૂર્ણાંક i;

    selected = current;

    backends[0] = '\0';

    માટે(i = 0; available_backends[i]; ++i) {
      અક્ષર vb[200];
      bool paren = (selected != available_backends[i]);

      જો(available_backends[i]->version(vb, કદ(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  જો(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  પાછા 0;
}

સ્થિર પૂર્ણાંક multissl_setup(અચલ સંરચના Curl_ssl *backend)
{
  પૂર્ણાંક i;
  અક્ષર *env;

  જો(Curl_ssl != &Curl_ssl_multi)
    પાછા 1;

  જો(backend) {
    Curl_ssl = backend;
    પાછા 0;
  }

  જો(!available_backends[0])
    પાછા 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  જો(env) {
    માટે(i = 0; available_backends[i]; i++) {
      જો(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        પાછા 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  માટે(i = 0; available_backends[i]; i++) {
    જો(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      પાછા 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  પાછા 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, અચલ અક્ષર *name,
                                   અચલ curl_ssl_backend ***avail)
{
  પૂર્ણાંક i;

  જો(avail)
    *avail = (અચલ curl_ssl_backend **)&available_backends;

  જો(Curl_ssl != &Curl_ssl_multi)
    પાછા id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#નહીંતર
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  માટે(i = 0; available_backends[i]; i++) {
    જો(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      પાછા CURLSSLSET_OK;
    }
  }

  પાછા CURLSSLSET_UNKNOWN_BACKEND;
}

#નહીંતર /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, અચલ અક્ષર *name,
                                   અચલ curl_ssl_backend ***avail)
{
  (શૂન્ય)id;
  (શૂન્ય)name;
  (શૂન્ય)avail;
  પાછા CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

શૂન્ય Curl_ssl_peer_cleanup(સંરચના ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_ચાવી);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

સ્થિર ssl_peer_type get_peer_type(અચલ અક્ષર *hostname)
{
  જો(hostname && hostname[0]) {
#ifdef USE_IPV6
    સંરચના in6_addr addr;
#નહીંતર
    સંરચના in_addr addr;
#endif
    જો(curlx_inet_pton(AF_INET, hostname, &addr))
      પાછા CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    નહીંતર જો(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      પાછા CURL_SSL_PEER_IPV6;
    }
#endif
  }
  પાછા CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(સંરચના ssl_peer *ssl_peer,
                            સંરચના Curl_peer *origin,
                            સંરચના Curl_peer *peer,
                            સંરચના ssl_filter_config *sslc,
                            અચલ અક્ષર *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  જો(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    પાછા CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  જો(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    જો(len && (origin->hostname[len - 1] == '.'))
      len--;
    જો(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      જો(!ssl_peer->sni)
        જાઓ out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_ચાવી_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_ચાવી);

out:
  જો(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  પાછા result;
}

સ્થિર શૂન્ય ssl_cf_destroy(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  જો(connssl) {
    connssl->ssl_impl->close(cf, data);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = ખાલી;
  }
  cf->connected = FALSE;
}

સ્થિર CURLcode ssl_cf_connect(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               bool *done)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result;

  જો(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    પાછા CURLE_OK;
  }

  જો(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    પાછા CURLE_FAILED_INIT;
  }

  જો(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    જો(result || !*done)
      પાછા result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  જો(!connssl->prefs_checked) {
    જો(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  જો(!result && *done) {
    cf->connected = TRUE;
    જો(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    જો(Curl_tls_keylog_enabled()) {
      infof(data, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_સંખ્યા
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (પૂર્ણાંક)result, *done);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode ssl_cf_set_earlydata(સંરચના Curl_cfilter *cf,
                                     સંરચના Curl_easy *data,
                                     અચલ શૂન્ય *buf, size_t blen)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  જો(blen) {
    જો(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    જો(result)
      પાછા result;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssl_cf_connect_deferred(સંરચના Curl_cfilter *cf,
                                        સંરચના Curl_easy *data,
                                        અચલ શૂન્ય *buf, size_t blen,
                                        bool *done)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  જો(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    જો(result)
      પાછા result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  જો(!result && *done) {
    જો(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    પસંદગી(connssl->earlydata_state) {
    સ્થિતિ ssl_earlydata_none:
      તોડો;
    સ્થિતિ ssl_earlydata_accepted:
      જો(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      તોડો;
    સ્થિતિ ssl_earlydata_rejected:
      જો(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      તોડો;
    મૂળભૂત:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(ખાલી);
      તોડો;
    }
  }
  પાછા result;
}

સ્થિર bool ssl_cf_data_pending(સંરચના Curl_cfilter *cf,
                                અચલ સંરચના Curl_easy *data)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  જો(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  નહીંતર
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  પાછા pending;
}

સ્થિર CURLcode ssl_cf_send(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            અચલ uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result = CURLE_OK;

  (શૂન્ય)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  જો(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    જો(result)
      જાઓ out;
    નહીંતર જો(!done) {
      result = CURLE_AGAIN;
      જાઓ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  જો(connssl->earlydata_skip) {
    જો(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      જાઓ out;
    }
    નહીંતર {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  જો(blen > 0) {
    size_t nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    જો(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode ssl_cf_recv(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data, અક્ષર *buf, size_t len,
                            size_t *pnread)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  જો(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, ખાલી, 0, &done);
    જો(result)
      જાઓ out;
    નહીંતર જો(!done) {
      result = CURLE_AGAIN;
      જાઓ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode ssl_cf_shutdown(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                bool *done)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  જો(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    સંરચના cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (પૂર્ણાંક)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  પાછા result;
}

સ્થિર CURLcode ssl_cf_adjust_pollset(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      સંરચના easy_pollset *ps)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result;

  CF_DATA_SAVE(save, cf, data);
  result = connssl->ssl_impl->adjust_pollset(cf, data, ps);
  CF_DATA_RESTORE(cf, save);
  પાછા result;
}

સ્થિર CURLcode ssl_cf_query(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             પૂર્ણાંક query, પૂર્ણાંક *pres1, શૂન્ય *pres2)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;

  પસંદગી(query) {
  સ્થિતિ CF_QUERY_SSL_INFO:
  સ્થિતિ CF_QUERY_SSL_CTX_INFO:
    જો(!Curl_ssl_cf_is_proxy(cf)) {
      સંરચના curl_tlssessioninfo *info = pres2;
      સંરચના cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      પાછા CURLE_OK;
    }
    તોડો;
  સ્થિતિ CF_QUERY_ALPN_NEGOTIATED: {
    અચલ અક્ષર **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    પાછા CURLE_OK;
  }
  મૂળભૂત:
    તોડો;
  }
  પાછા cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

સ્થિર CURLcode ssl_cf_cntrl(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             પૂર્ણાંક event, પૂર્ણાંક arg1, શૂન્ય *arg2)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;

  (શૂન્ય)arg1;
  (શૂન્ય)arg2;
  (શૂન્ય)data;
  પસંદગી(event) {
  સ્થિતિ CF_CTRL_CONN_INFO_UPDATE:
    જો(connssl->negotiated.alpn && !cf->sockindex) {
      જો(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      નહીંતર જો(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      નહીંતર જો(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    તોડો;
  સ્થિતિ CF_CTRL_REPORT_STATS:
    જો(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    તોડો;
  }
  પાછા CURLE_OK;
}

સ્થિર bool cf_ssl_is_alive(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data,
                            bool *આગત_pending)
{
  /*
   * This function tries to determine connection status.
   */
  પાછા cf->next ?
    cf->next->cft->is_alive(cf->next, data, આગત_pending) :
    FALSE; /* pessimistic in absence of data */
}

સંરચના Curl_cftype Curl_cft_ssl = {
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

સંરચના Curl_cftype Curl_cft_ssl_proxy = {
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

સ્થિર CURLcode cf_ssl_create(સંરચના Curl_cfilter **pcf,
                              સંરચના Curl_easy *data,
                              સંરચના connectdata *conn)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના ssl_connect_data *ctx;
  CURLcode result;

  DEBUGASSERT(data->conn);

#ifdef CURL_DISABLE_HTTP
  (શૂન્ય)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(data, ખાલી);
#નહીંતર
  ctx = cf_ctx_new(data, alpn_get_spec(data->state.http_neg.wanted,
                                       data->state.http_neg.preferred,
                                       (bool)data->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  જો(result)
    cf_ctx_free(ctx);
  *pcf = result ? ખાલી : cf;
  પાછા result;
}

સ્થિર CURLcode cf_ssl_peer_init(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_peer *origin,
                                 સંરચના Curl_peer *peer,
                                 સંરચના ssl_filter_config *sslc)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  અક્ષર tls_id[80];
  connssl->ssl_impl->version(tls_id, કદ(tls_id) - 1);
  પાછા Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(સંરચના Curl_easy *data,
                              સંરચના Curl_peer *origin,
                              સંરચના connectdata *conn,
                              int8_t sockindex)
{
  સંરચના Curl_cfilter *cf;
  સંરચના Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode result;

  result = cf_ssl_create(&cf, data, conn);
  જો(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  જો(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  નહીંતર જો(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  પાછા result;
}

CURLcode Curl_cf_ssl_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                  સંરચના Curl_easy *data,
                                  સંરચના Curl_peer *origin,
                                  સંરચના Curl_peer *peer)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  જો(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  જો(!result) {
    Curl_conn_cf_ઉમેરો_after(cf_at, cf);
#જો defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    જો((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  નહીંતર જો(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  પાછા result;
}

#ifndef CURL_DISABLE_PROXY

સ્થિર CURLcode cf_ssl_proxy_create(સંરચના Curl_cfilter **pcf,
                                    સંરચના Curl_easy *data,
                                    સંરચના connectdata *conn)
{
  સંરચના Curl_cfilter *cf = ખાલી;
  સંરચના ssl_connect_data *ctx;
  CURLcode result;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)data->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (શૂન્ય)conn;
#ifdef USE_HTTP2
  જો(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  જો(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    જાઓ out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  જો(result)
    cf_ctx_free(ctx);
  *pcf = result ? ખાલી : cf;
  પાછા result;
}

CURLcode Curl_cf_ssl_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                        સંરચના Curl_easy *data,
                                        સંરચના Curl_peer *peer)
{
  સંરચના Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  જો(!result)
    result = cf_ssl_peer_init(cf, peer, ખાલી, &cf_at->conn->proxy_ssl_config);
  જો(!result)
    Curl_conn_cf_ઉમેરો_after(cf_at, cf);
  નહીંતર જો(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  પાછા result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(સંરચના Curl_easy *data, unsigned પૂર્ણાંક ssl_option)
{
  (શૂન્ય)data;
  પાછા (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(સંરચના Curl_cfilter *cf)
{
  જો(Curl_ssl->supports & SSLSUPP_ECH) {
    સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    પાછા !!conn_config->ech;
  }
  પાછા FALSE;
}

bool Curl_ssl_need_httpsrr(સંરચના Curl_cfilter *cf)
{
  જો(Curl_ssl->supports & SSLSUPP_ECH) {
    સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    જો(!conn_config->ech)
      પાછા FALSE;
    જો((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      પાછા FALSE;
    પાછા TRUE;
  }
  પાછા FALSE;
}
#endif

સ્થિર CURLcode vtls_shutdown_blocking(સંરચના Curl_cfilter *cf,
                                       સંરચના Curl_easy *data,
                                       bool send_shutdown, bool *done)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  સંરચના cf_call_data save;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  પૂર્ણાંક what, loop = 10;

  જો(cf->shutdown) {
    *done = TRUE;
    પાછા CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  જ્યારે(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    જો(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      જાઓ out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    જો(result || *done)
      જાઓ out;

    જો(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      જો(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        જાઓ out;
      }
      નહીંતર જો(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        જાઓ out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  પાછા result;
}

CURLcode Curl_ssl_cfilter_remove(સંરચના Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  સંરચના Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : ખાલી;
  માટે(cf = head; cf; cf = cf->next) {
    જો(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      જો(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (પૂર્ણાંક)result);
      તોડો;
    }
  }
  પાછા result;
}

bool Curl_ssl_cf_is_proxy(સંરચના Curl_cfilter *cf)
{
  પાછા (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

સંરચના ssl_easy_config *
Curl_ssl_cf_get_easy_config(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (શૂન્ય)cf;
  પાછા &data->set.ssl;
#નહીંતર
  પાછા Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

સંરચના ssl_filter_config *
Curl_ssl_cf_get_filter_config(સંરચના Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  પાછા &cf->conn->ssl_config;
#નહીંતર
  પાછા Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(સંરચના alpn_proto_buf *buf,
                                અચલ સંરચના alpn_spec *spec)
{
  size_t i, len;
  પૂર્ણાંક off = 0;
  unsigned અક્ષર blen;

  memset(buf, 0, કદ(*buf));
  માટે(i = 0; spec && i < spec->ગણતરી; ++i) {
    len = strlen(spec->entries[i]);
    જો(len >= ALPN_NAME_MAX)
      પાછા CURLE_FAILED_INIT;
    blen = (unsigned અક્ષર)len;
    જો(off + blen + 1 >= (પૂર્ણાંક)કદ(buf->data))
      પાછા CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    memcpy(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  પાછા CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(સંરચના alpn_proto_buf *buf,
                                અચલ સંરચના alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, કદ(*buf));
  માટે(i = 0; spec && i < spec->ગણતરી; ++i) {
    len = strlen(spec->entries[i]);
    જો(len >= ALPN_NAME_MAX)
      પાછા CURLE_FAILED_INIT;
    જો(off + len + 2 >= કદ(buf->data))
      પાછા CURLE_FAILED_INIT;
    જો(off)
      buf->data[off++] = ',';
    memcpy(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (પૂર્ણાંક)off;
  પાછા CURLE_OK;
}

bool Curl_alpn_contains_proto(અચલ સંરચના alpn_spec *spec,
                              અચલ અક્ષર *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  માટે(i = 0; spec && plen && i < spec->ગણતરી; ++i) {
    size_t slen = strlen(spec->entries[i]);
    જો((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      પાછા TRUE;
  }
  પાછા FALSE;
}

શૂન્ય Curl_alpn_restrict_to(સંરચના alpn_spec *spec, અચલ અક્ષર *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < કદ(spec->entries[0]));
  જો(plen < કદ(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->ગણતરી = 1;
  }
}

શૂન્ય Curl_alpn_copy(સંરચના alpn_spec *dest, અચલ સંરચના alpn_spec *src)
{
  જો(src)
    memcpy(dest, src, કદ(*dest));
  નહીંતર
    memset(dest, 0, કદ(*dest));
}

CURLcode Curl_alpn_set_negotiated(સંરચના Curl_cfilter *cf,
                                  સંરચના Curl_easy *data,
                                  સંરચના ssl_connect_data *connssl,
                                  અચલ unsigned અક્ષર *proto,
                                  size_t proto_len)
{
  CURLcode result = CURLE_OK;
  (શૂન્ય)cf;

  જો(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    જો(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ out;
    }
    નહીંતર જો(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ out;
    }
    નહીંતર જો((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (પૂર્ણાંક)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    જાઓ out;
  }

  જો(proto && proto_len) {
    જો(memchr(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      જાઓ out;
    }
    connssl->negotiated.alpn = curlx_memdup0((અચલ અક્ષર *)proto, proto_len);
    જો(!connssl->negotiated.alpn)
      પાછા CURLE_OUT_OF_MEMORY;
  }

  જો(proto && proto_len) {
    જો(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (પૂર્ણાંક)proto_len, proto);
    નહીંતર
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (પૂર્ણાંક)proto_len, proto);
  }
  નહીંતર {
    જો(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    નહીંતર
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  પાછા result;
}

CURLcode Curl_on_session_reuse(સંરચના Curl_cfilter *cf,
                               સંરચના Curl_easy *data,
                               સંરચના alpn_spec *alpns,
                               સંરચના Curl_ssl_session *scs,
                               bool *do_early_data, bool early_data_allowed)
{
  સંરચના ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *do_early_data = FALSE;

  જો(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  નહીંતર જો(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  નહીંતર {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (અચલ unsigned અક્ષર *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_data = !result;
  }
  પાછા result;
}

સંરચના Curl_ssl_session *Curl_ssl_get_cf_session(સંરચના Curl_easy *data,
                                                 અચલ સંરચના Curl_cftype *cft,
                                                 int8_t sockindex)
{
  જો(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#નહીંતર
     (cft == &Curl_cft_ssl)) {
#endif
    સંરચના Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    માટે(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      જો(cf1->cft == cft)
        તોડો;
    }
    જો(cf1) {
      સંરચના ssl_connect_data *connssl = cf1->ctx;
      જો(connssl)
        પાછા connssl->session;
    }
  }
  પાછા ખાલી;
}

#endif /* USE_SSL */
