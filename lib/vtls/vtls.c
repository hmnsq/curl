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
#जर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
स्थिर स्थिरांक संरचना alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
स्थिर स्थिरांक संरचना alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
स्थिर स्थिरांक संरचना alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
स्थिर स्थिरांक संरचना alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
स्थिर स्थिरांक संरचना alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

स्थिर स्थिरांक संरचना alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  जर(!use_alpn)
    परत रिक्त;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  जर(only_http_10 && (wanted & CURL_HTTP_V1x))
    परत &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  जर(wanted & CURL_HTTP_V2x) {
    जर(wanted & CURL_HTTP_V1x)
      परत (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    परत &ALPN_SPEC_H2;
  }
#अन्यथा
  (शून्य)wanted;
  (शून्य)preferred;
#endif
  परत &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
स्थिर पूर्णांक multissl_setup(स्थिरांक संरचना Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(शून्य)
{
#ifdef USE_SSL
  multissl_setup(रिक्त);
  परत Curl_ssl->info.id;
#अन्यथा
  परत CURLSSLBACKEND_NONE;
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
पूर्णांक Curl_ssl_init(शून्य)
{
  /* make sure this is only done once */
  जर(init_ssl)
    परत 1;
  init_ssl = TRUE; /* never again */

  जर(Curl_ssl->init)
    परत Curl_ssl->init();
  परत 1;
}

स्थिर bool ssl_prefs_check(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  जर(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(data, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    परत FALSE;
  }

  निवड(conn_config->version_max) {
  स्थिती CURL_SSLVERSION_MAX_NONE:
  स्थिती CURL_SSLVERSION_MAX_DEFAULT:
    थांबा;

  पूर्वनिर्धारित:
    जर((conn_config->version_max >> 16) < conn_config->version) {
      failf(data, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      परत FALSE;
    }
  }

  परत TRUE;
}

स्थिर संरचना ssl_connect_data *cf_ctx_new(संरचना Curl_easy *data,
                                           स्थिरांक संरचना alpn_spec *alpn)
{
  संरचना ssl_connect_data *ctx;

  (शून्य)data;
  ctx = curlx_calloc(1, आकार(*ctx));
  जर(!ctx)
    परत रिक्त;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_data);
  जर(!ctx->backend) {
    curlx_free(ctx);
    परत रिक्त;
  }
  परत ctx;
}

स्थिर शून्य cf_ctx_free(संरचना ssl_connect_data *ctx)
{
  जर(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(संरचना Curl_easy *data, int8_t sockindex,
                                      संरचना dynbuf *binding)
{
  जर(Curl_ssl->get_channel_binding)
    परत Curl_ssl->get_channel_binding(data, sockindex, binding);
  परत CURLE_OK;
}

शून्य Curl_ssl_close_all(संरचना Curl_easy *data)
{
  जर(Curl_ssl->close_all)
    Curl_ssl->close_all(data);
}

CURLcode Curl_ssl_adjust_pollset(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 संरचना easy_pollset *ps)
{
  संरचना ssl_connect_data *connssl = cf->ctx;

  जर(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->next, data);
    CURLcode result = CURLE_OK;
    जर(sock != CURL_SOCKET_BAD) {
      जर(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
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
    परत result;
  }
  परत CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
संरचना curl_slist *Curl_ssl_engines_list(संरचना Curl_easy *data)
{
  जर(Curl_ssl->engines_list)
    परत Curl_ssl->engines_list(data);
  परत रिक्त;
}

स्थिर माप_प्रकार multissl_version(अक्षर *buffer, माप_प्रकार size);

शून्य Curl_ssl_version(अक्षर *buffer, माप_प्रकार size)
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

  जर(ci->अंक_of_certs) {
    /* free all individual lists used */
    पूर्णांक i;
    साठी(i = 0; i < ci->अंक_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = रिक्त;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->अंक_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(संरचना Curl_easy *data, पूर्णांक अंक)
{
  संरचना curl_certinfo *ci = &data->info.certs;
  संरचना curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(data);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((माप_प्रकार)अंक, आकार(संरचना curl_slist *));
  जर(!table)
    परत CURLE_OUT_OF_MEMORY;

  ci->अंक_of_certs = अंक;
  ci->certinfo = table;

  परत CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ढकला_certinfo_len(संरचना Curl_easy *data,
                                    पूर्णांक certnum,
                                    स्थिरांक अक्षर *label,
                                    स्थिरांक अक्षर *मूल्य,
                                    माप_प्रकार valuelen)
{
  संरचना curl_certinfo *ci = &data->info.certs;
  संरचना curl_slist *nl;
  CURLcode result = CURLE_OK;
  संरचना dynbuf build;

  DEBUGASSERT(certnum < ci->अंक_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  जर(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, मूल्य, valuelen))
    परत CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  जर(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    result = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  परत result;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(संरचना Curl_easy *data,
                         अचिन्हित अक्षर *buffer, माप_प्रकार length)
{
  DEBUGASSERT(length == आकार(पूर्णांक));
  जर(Curl_ssl->random)
    परत Curl_ssl->random(data, buffer, length);
  अन्यथा
    परत CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

स्थिर CURLcode pubkey_pem_to_der(स्थिरांक अक्षर *pem,
                                  अचिन्हित अक्षर **der, माप_प्रकार *der_len)
{
  स्थिरांक अक्षर *begin_pos, *end_pos;
  माप_प्रकार pem_मोजणी, pem_len;
  CURLcode result;
  संरचना dynbuf pbuf;

  /* if no pem, exit. */
  जर(!pem)
    परत CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_SIZE);

  begin_pos = म_माला(pem, "-----BEGIN PUBLIC KEY-----");
  जर(!begin_pos)
    परत CURLE_BAD_CONTENT_ENCODING;

  pem_मोजणी = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  जर(pem_मोजणी && '\n' != pem[pem_मोजणी - 1])
    परत CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_मोजणी += 26;

  /* Invalid if not directly following \n */
  end_pos = म_माला(pem + pem_मोजणी, "\n-----END PUBLIC KEY-----");
  जर(!end_pos)
    परत CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  पर्यंत(pem_मोजणी < pem_len) {
    जर('\n' != pem[pem_मोजणी] && '\r' != pem[pem_मोजणी]) {
      result = curlx_dyn_addn(&pbuf, &pem[pem_मोजणी], 1);
      जर(result)
        परत result;
    }
    ++pem_मोजणी;
  }

  जर(curlx_dyn_len(&pbuf)) {
    result = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  अन्यथा
    result = CURLE_BAD_CONTENT_ENCODING;

  परत result;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(संरचना Curl_easy *data,
                              स्थिरांक अक्षर *pinnedpubkey,
                              स्थिरांक अचिन्हित अक्षर *pubkey, माप_प्रकार pubkeylen)
{
  CURLcode result = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  जर(!pinnedpubkey)
    परत CURLE_OK;
  जर(!pubkey || !pubkeylen)
    परत result;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  जर(!म_भेदन(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    अक्षर *cert_hash = रिक्त;
    स्थिरांक अक्षर *pinned_hash, *end_pos;
    माप_प्रकार cert_hash_len = 0, pinned_hash_len;
    अचिन्हित अक्षर *sha256sumdigest;

    जर(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      परत result;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_LENGTH);
    जर(!sha256sumdigest)
      परत CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_LENGTH);

    जर(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_LENGTH,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    जर(encode)
      परत encode;

    infof(data, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    पर्यंत(pinned_hash &&
          !म_भेदन(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = म_अक्षर(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (माप_प्रकार)(end_pos - pinned_hash) : म_माप(pinned_hash);

      /* compare base64 sha256 digests */
      जर(cert_hash_len == pinned_hash_len &&
         !स_भेद(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(data, "public key hash matches pinned value"));
        result = CURLE_OK;
        थांबा;
      }

      DEBUGF(infof(data, "public key hash does not match 'sha256//%.*s'",
                   (पूर्णांक)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : रिक्त;
    }
    curlx_safefree(cert_hash);
  }
  अन्यथा {
    दीर्घ filesize;
    माप_प्रकार size, pem_len;
    CURLcode pem_read;
    संरचना dynbuf buf;
    अचिन्हित अक्षर *pem_ptr = रिक्त;
    माप_प्रकार डावा;
    खाता *fp = curlx_fopen(pinnedpubkey, "rb");
    जर(!fp)
      परत result;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_SIZE);

    /* Determine the file's size */
    जर(ख_जाओ(fp, 0, अंत_से))
      जा end;
    filesize = ख_बताओ(fp);
    जर(ख_जाओ(fp, 0, शुरू_से))
      जा end;
    जर(filesize < 0 || filesize > MAX_PINNED_PUBKEY_SIZE)
      जा end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    size = curlx_sotouz((curl_off_t)filesize);
    जर(pubkeylen > size)
      जा end;

    /*
     * Read the file into the dynbuf
     */
    डावा = size;
    करा {
      अक्षर buffer[1024];
      माप_प्रकार want = डावा > आकार(buffer) ? आकार(buffer) : डावा;
      जर(ख_पढ़ो(buffer, 1, want, fp) != want)
        जा end;
      जर(curlx_dyn_addn(&buf, buffer, want))
        जा end;
      डावा -= want;
    } पर्यंत(डावा);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    जर(pubkeylen == size) {
      जर(!स_भेद(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        result = CURLE_OK;
      जा end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    जर(pem_read)
      जा end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    जर(pubkeylen == pem_len && !स_भेद(pubkey, pem_ptr, pubkeylen))
      result = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  परत result;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(शून्य)
{
  जर(Curl_ssl->cert_status_request)
    परत Curl_ssl->cert_status_request();
  परत FALSE;
}

स्थिर पूर्णांक multissl_init(शून्य)
{
  जर(multissl_setup(रिक्त))
    परत 1;
  जर(Curl_ssl->init)
    परत Curl_ssl->init();
  परत 1;
}

स्थिर CURLcode multissl_random(संरचना Curl_easy *data,
                                अचिन्हित अक्षर *entropy, माप_प्रकार length)
{
  जर(multissl_setup(रिक्त))
    परत CURLE_FAILED_INIT;
  परत Curl_ssl->random(data, entropy, length);
}

स्थिर CURLcode multissl_connect(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data, bool *done)
{
  जर(multissl_setup(रिक्त))
    परत CURLE_FAILED_INIT;
  परत Curl_ssl->do_connect(cf, data, done);
}

स्थिर CURLcode multissl_adjust_pollset(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        संरचना easy_pollset *ps)
{
  जर(multissl_setup(रिक्त))
    परत CURLE_OK;
  परत Curl_ssl->adjust_pollset(cf, data, ps);
}

स्थिर शून्य *multissl_get_internals(संरचना ssl_connect_data *connssl,
                                    CURLINFO info)
{
  जर(multissl_setup(रिक्त))
    परत रिक्त;
  परत Curl_ssl->get_internals(connssl, info);
}

स्थिर शून्य multissl_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  जर(multissl_setup(रिक्त))
    परत;
  Curl_ssl->बंद(cf, data);
}

स्थिर CURLcode multissl_recv_plain(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  जर(multissl_setup(रिक्त))
    परत CURLE_FAILED_INIT;
  परत Curl_ssl->recv_plain(cf, data, buf, len, pnread);
}

स्थिर CURLcode multissl_send_plain(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *data,
                                    स्थिरांक शून्य *mem, माप_प्रकार len,
                                    माप_प्रकार *pnwritten)
{
  जर(multissl_setup(रिक्त))
    परत CURLE_FAILED_INIT;
  परत Curl_ssl->send_plain(cf, data, mem, len, pnwritten);
}

स्थिर स्थिरांक संरचना Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (माप_प्रकार)-1, /* something insanely large to be on the safe side */

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

स्थिरांक संरचना Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#किंवाजर defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#किंवाजर defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#किंवाजर defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#किंवाजर defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#किंवाजर defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#किंवाजर defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#अन्यथा
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

स्थिर स्थिरांक संरचना Curl_ssl *available_backends[] = {
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
  जर(init_ssl) {
    /* only cleanup if we did a previous init */
    जर(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

स्थिर माप_प्रकार multissl_version(अक्षर *buffer, माप_प्रकार size)
{
  स्थिर स्थिरांक संरचना Curl_ssl *selected;
  स्थिर अक्षर backends[200];
  स्थिर माप_प्रकार backends_len;
  स्थिरांक संरचना Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  जर(current != selected) {
    अक्षर *p = backends;
    स्थिरांक अक्षर *end = backends + आकार(backends);
    पूर्णांक i;

    selected = current;

    backends[0] = '\0';

    साठी(i = 0; available_backends[i]; ++i) {
      अक्षर vb[200];
      bool paren = (selected != available_backends[i]);

      जर(available_backends[i]->version(vb, आकार(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  जर(size) {
    curlx_strcopy(buffer, size, backends, backends_len);
  }
  परत 0;
}

स्थिर पूर्णांक multissl_setup(स्थिरांक संरचना Curl_ssl *backend)
{
  पूर्णांक i;
  अक्षर *env;

  जर(Curl_ssl != &Curl_ssl_multi)
    परत 1;

  जर(backend) {
    Curl_ssl = backend;
    परत 0;
  }

  जर(!available_backends[0])
    परत 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  जर(env) {
    साठी(i = 0; available_backends[i]; i++) {
      जर(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        परत 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  साठी(i = 0; available_backends[i]; i++) {
    जर(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      परत 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  परत 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, स्थिरांक अक्षर *name,
                                   स्थिरांक curl_ssl_backend ***avail)
{
  पूर्णांक i;

  जर(avail)
    *avail = (स्थिरांक curl_ssl_backend **)&available_backends;

  जर(Curl_ssl != &Curl_ssl_multi)
    परत id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#अन्यथा
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  साठी(i = 0; available_backends[i]; i++) {
    जर(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      परत CURLSSLSET_OK;
    }
  }

  परत CURLSSLSET_UNKNOWN_BACKEND;
}

#अन्यथा /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, स्थिरांक अक्षर *name,
                                   स्थिरांक curl_ssl_backend ***avail)
{
  (शून्य)id;
  (शून्य)name;
  (शून्य)avail;
  परत CURLSSLSET_NO_BACKENDS;
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

स्थिर ssl_peer_type get_peer_type(स्थिरांक अक्षर *hostname)
{
  जर(hostname && hostname[0]) {
#ifdef USE_IPV6
    संरचना in6_addr addr;
#अन्यथा
    संरचना in_addr addr;
#endif
    जर(curlx_inet_pton(AF_INET, hostname, &addr))
      परत CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    अन्यथा जर(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      परत CURL_SSL_PEER_IPV6;
    }
#endif
  }
  परत CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(संरचना ssl_peer *ssl_peer,
                            संरचना Curl_peer *origin,
                            संरचना Curl_peer *peer,
                            संरचना ssl_filter_config *sslc,
                            स्थिरांक अक्षर *tls_id,
                            uint8_t transport)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  जर(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    परत CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  जर(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    माप_प्रकार len = म_माप(origin->hostname);
    जर(len && (origin->hostname[len - 1] == '.'))
      len--;
    जर(len < अच_लघु_उच्च) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      जर(!ssl_peer->sni)
        जा out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  result = Curl_ssl_peer_किल्ली_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_किल्ली);

out:
  जर(result)
    Curl_ssl_peer_cleanup(ssl_peer);
  परत result;
}

स्थिर शून्य ssl_cf_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  जर(connssl) {
    connssl->ssl_impl->बंद(cf, data);
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

  जर(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    परत CURLE_OK;
  }

  जर(!cf->next || !connssl->peer.origin) {
    *done = FALSE;
    परत CURLE_FAILED_INIT;
  }

  जर(!cf->next->connected) {
    result = cf->next->cft->do_connect(cf->next, data, done);
    जर(result || !*done)
      परत result;
  }

  CF_DATA_SAVE(save, cf, data);
  CURL_TRC_CF(data, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  जर(!connssl->prefs_checked) {
    जर(!ssl_prefs_check(cf, data)) {
      result = CURLE_SSL_CONNECT_ERROR;
      जा out;
    }
    connssl->prefs_checked = TRUE;
  }

  result = connssl->ssl_impl->do_connect(cf, data, done);

  जर(!result && *done) {
    cf->connected = TRUE;
    जर(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(data);
    }
    जर(Curl_tls_keylog_enabled()) {
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
  CURL_TRC_CF(data, cf, "cf_connect() -> %d, done=%d", (पूर्णांक)result, *done);
  CF_DATA_RESTORE(cf, save);
  परत result;
}

स्थिर CURLcode ssl_cf_set_earlydata(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *data,
                                     स्थिरांक शून्य *buf, माप_प्रकार blen)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  माप_प्रकार nwritten = 0;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  जर(blen) {
    जर(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    result = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(data, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    जर(result)
      परत result;
  }
  परत CURLE_OK;
}

स्थिर CURLcode ssl_cf_connect_deferred(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *data,
                                        स्थिरांक शून्य *buf, माप_प्रकार blen,
                                        bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  जर(connssl->earlydata_state == ssl_earlydata_await) {
    result = ssl_cf_set_earlydata(cf, data, buf, blen);
    जर(result)
      परत result;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  result = ssl_cf_connect(cf, data, done);

  जर(!result && *done) {
    जर(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    निवड(connssl->earlydata_state) {
    स्थिती ssl_earlydata_none:
      थांबा;
    स्थिती ssl_earlydata_accepted:
      जर(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, (curl_off_t)connssl->earlydata_skip);
      infof(data, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      थांबा;
    स्थिती ssl_earlydata_rejected:
      जर(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrsEarlyData(data, -(curl_off_t)connssl->earlydata_skip);
      infof(data, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      थांबा;
    पूर्वनिर्धारित:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(रिक्त);
      थांबा;
    }
  }
  परत result;
}

स्थिर bool ssl_cf_data_pending(संरचना Curl_cfilter *cf,
                                स्थिरांक संरचना Curl_easy *data)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना cf_call_data save;
  bool pending;

  CF_DATA_SAVE(save, cf, data);
  जर(connssl->ssl_impl->data_pending &&
     connssl->ssl_impl->data_pending(cf, data))
    pending = TRUE;
  अन्यथा
    pending = cf->next->cft->has_data_pending(cf->next, data);
  CF_DATA_RESTORE(cf, save);
  परत pending;
}

स्थिर CURLcode ssl_cf_send(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            स्थिरांक uint8_t *buf, माप_प्रकार blen,
                            bool eos, माप_प्रकार *pnwritten)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result = CURLE_OK;

  (शून्य)eos;
  *pnwritten = 0;
  CF_DATA_SAVE(save, cf, data);

  जर(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, buf, blen, &done);
    जर(result)
      जा out;
    अन्यथा जर(!done) {
      result = CURLE_AGAIN;
      जा out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  जर(connssl->earlydata_skip) {
    जर(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      result = CURLE_OK;
      *pnwritten = blen;
      जा out;
    }
    अन्यथा {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  जर(blen > 0) {
    माप_प्रकार nwritten;
    result = connssl->ssl_impl->send_plain(cf, data, buf, blen, &nwritten);
    जर(!result)
      *pnwritten += nwritten;
  }

out:
  CF_DATA_RESTORE(cf, save);
  परत result;
}

स्थिर CURLcode ssl_cf_recv(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data, अक्षर *buf, माप_प्रकार len,
                            माप_प्रकार *pnread)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  संरचना cf_call_data save;
  CURLcode result = CURLE_OK;

  CF_DATA_SAVE(save, cf, data);
  *pnread = 0;
  जर(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    result = ssl_cf_connect_deferred(cf, data, रिक्त, 0, &done);
    जर(result)
      जा out;
    अन्यथा जर(!done) {
      result = CURLE_AGAIN;
      जा out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  result = connssl->ssl_impl->recv_plain(cf, data, buf, len, pnread);

out:
  CF_DATA_RESTORE(cf, save);
  परत result;
}

स्थिर CURLcode ssl_cf_shutdown(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                bool *done)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  CURLcode result = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  जर(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    संरचना cf_call_data save;

    CF_DATA_SAVE(save, cf, data);
    result = connssl->ssl_impl->shut_down(cf, data, TRUE, done);
    CURL_TRC_CF(data, cf, "cf_shutdown -> %d, done=%d", (पूर्णांक)result, *done);
    CF_DATA_RESTORE(cf, save);
    cf->shutdown = (result || *done);
  }
  परत result;
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
  परत result;
}

स्थिर CURLcode ssl_cf_query(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             पूर्णांक query, पूर्णांक *pres1, शून्य *pres2)
{
  संरचना ssl_connect_data *connssl = cf->ctx;

  निवड(query) {
  स्थिती CF_QUERY_SSL_INFO:
  स्थिती CF_QUERY_SSL_CTX_INFO:
    जर(!Curl_ssl_cf_is_proxy(cf)) {
      संरचना curl_tlssessioninfo *info = pres2;
      संरचना cf_call_data save;
      CF_DATA_SAVE(save, cf, data);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_DATA_RESTORE(cf, save);
      परत CURLE_OK;
    }
    थांबा;
  स्थिती CF_QUERY_ALPN_NEGOTIATED: {
    स्थिरांक अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(data, cf, "query ALPN: returning '%s'", *palpn);
    परत CURLE_OK;
  }
  पूर्वनिर्धारित:
    थांबा;
  }
  परत cf->next ?
    cf->next->cft->query(cf->next, data, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode ssl_cf_cntrl(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             पूर्णांक event, पूर्णांक arg1, शून्य *arg2)
{
  संरचना ssl_connect_data *connssl = cf->ctx;

  (शून्य)arg1;
  (शून्य)arg2;
  (शून्य)data;
  निवड(event) {
  स्थिती CF_CTRL_CONN_INFO_UPDATE:
    जर(connssl->negotiated.alpn && !cf->sockindex) {
      जर(!म_भेद("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      अन्यथा जर(!म_भेद("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      अन्यथा जर(!म_भेद("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    थांबा;
  स्थिती CF_CTRL_REPORT_STATS:
    जर(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(data, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    थांबा;
  }
  परत CURLE_OK;
}

स्थिर bool cf_ssl_is_alive(संरचना Curl_cfilter *cf, संरचना Curl_easy *data,
                            bool *आदान_pending)
{
  /*
   * This function tries to determine connection status.
   */
  परत cf->next ?
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
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }

  result = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  जर(result)
    cf_ctx_free(ctx);
  *pcf = result ? रिक्त : cf;
  परत result;
}

स्थिर CURLcode cf_ssl_peer_init(संरचना Curl_cfilter *cf,
                                 संरचना Curl_peer *origin,
                                 संरचना Curl_peer *peer,
                                 संरचना ssl_filter_config *sslc)
{
  संरचना ssl_connect_data *connssl = cf->ctx;
  अक्षर tls_id[80];
  connssl->ssl_impl->version(tls_id, आकार(tls_id) - 1);
  परत Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
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
  जर(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  जर(!result)
    Curl_conn_cf_add(data, conn, sockindex, cf);
  अन्यथा जर(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  परत result;
}

CURLcode Curl_cf_ssl_घाला_after(संरचना Curl_cfilter *cf_at,
                                  संरचना Curl_easy *data,
                                  संरचना Curl_peer *origin,
                                  संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_create(&cf, data, cf_at->conn);
  जर(!result)
    result = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  जर(!result) {
    Curl_conn_cf_घाला_after(cf_at, cf);
#जर defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    जर((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      result = Curl_conn_dns_add_https_resolve(data, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  अन्यथा जर(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  परत result;
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
  जर(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(data, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  जर(!ctx) {
    result = CURLE_OUT_OF_MEMORY;
    जा out;
  }
  result = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  जर(result)
    cf_ctx_free(ctx);
  *pcf = result ? रिक्त : cf;
  परत result;
}

CURLcode Curl_cf_ssl_proxy_घाला_after(संरचना Curl_cfilter *cf_at,
                                        संरचना Curl_easy *data,
                                        संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf;
  CURLcode result;

  result = cf_ssl_proxy_create(&cf, data, cf_at->conn);
  जर(!result)
    result = cf_ssl_peer_init(cf, peer, रिक्त, &cf_at->conn->proxy_ssl_config);
  जर(!result)
    Curl_conn_cf_घाला_after(cf_at, cf);
  अन्यथा जर(cf)
    Curl_conn_cf_discard_chain(&cf, data);
  परत result;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(संरचना Curl_easy *data, अचिन्हित पूर्णांक ssl_option)
{
  (शून्य)data;
  परत (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(संरचना Curl_cfilter *cf)
{
  जर(Curl_ssl->supports & SSLSUPP_ECH) {
    संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    परत !!conn_config->ech;
  }
  परत FALSE;
}

bool Curl_ssl_need_httpsrr(संरचना Curl_cfilter *cf)
{
  जर(Curl_ssl->supports & SSLSUPP_ECH) {
    संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    जर(!conn_config->ech)
      परत FALSE;
    जर((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      परत FALSE;
    परत TRUE;
  }
  परत FALSE;
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
  पूर्णांक what, loop = 10;

  जर(cf->shutdown) {
    *done = TRUE;
    परत CURLE_OK;
  }
  CF_DATA_SAVE(save, cf, data);

  *done = FALSE;
  पर्यंत(!result && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(data, cf->conn, cf->sockindex);

    जर(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(data, "SSL shutdown timeout");
      result = CURLE_OPERATION_TIMEDOUT;
      जा out;
    }

    result = connssl->ssl_impl->shut_down(cf, data, send_shutdown, done);
    जर(result || *done)
      जा out;

    जर(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, data, timeout_ms);
      जर(what < 0) {
        /* fatal error */
        failf(data, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        result = CURLE_RECV_ERROR;
        जा out;
      }
      अन्यथा जर(what == 0) {
        /* timeout */
        failf(data, "SSL shutdown timeout");
        result = CURLE_OPERATION_TIMEDOUT;
        जा out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_DATA_RESTORE(cf, save);
  cf->shutdown = (result || *done);
  परत result;
}

CURLcode Curl_ssl_cfilter_remove(संरचना Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown)
{
  संरचना Curl_cfilter *cf, *head;
  CURLcode result = CURLE_OK;

  head = data->conn ? data->conn->cfilter[sockindex] : रिक्त;
  साठी(cf = head; cf; cf = cf->next) {
    जर(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(data, sockindex, 0);
      result = vtls_shutdown_blocking(cf, data, send_shutdown, &done);
      Curl_cshutdn_clear_timer(data, sockindex);
      जर(!result && !done) /* blocking failed? */
        result = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, data);
      CURL_TRC_CF(data, cf, "shutdown and remove SSL, done -> %d",
                  (पूर्णांक)result);
      थांबा;
    }
  }
  परत result;
}

bool Curl_ssl_cf_is_proxy(संरचना Curl_cfilter *cf)
{
  परत (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

संरचना ssl_easy_config *
Curl_ssl_cf_get_easy_config(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data)
{
#ifdef CURL_DISABLE_PROXY
  (शून्य)cf;
  परत &data->set.ssl;
#अन्यथा
  परत Curl_ssl_cf_is_proxy(cf) ? &data->set.proxy_ssl : &data->set.ssl;
#endif
}

संरचना ssl_filter_config *
Curl_ssl_cf_get_filter_config(संरचना Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  परत &cf->conn->ssl_config;
#अन्यथा
  परत Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(संरचना alpn_proto_buf *buf,
                                स्थिरांक संरचना alpn_spec *spec)
{
  माप_प्रकार i, len;
  पूर्णांक off = 0;
  अचिन्हित अक्षर blen;

  स_रखो(buf, 0, आकार(*buf));
  साठी(i = 0; spec && i < spec->मोजणी; ++i) {
    len = म_माप(spec->entries[i]);
    जर(len >= ALPN_NAME_MAX)
      परत CURLE_FAILED_INIT;
    blen = (अचिन्हित अक्षर)len;
    जर(off + blen + 1 >= (पूर्णांक)आकार(buf->data))
      परत CURLE_FAILED_INIT;
    buf->data[off++] = blen;
    स_नकल(buf->data + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  परत CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(संरचना alpn_proto_buf *buf,
                                स्थिरांक संरचना alpn_spec *spec)
{
  माप_प्रकार i, len;
  माप_प्रकार off = 0;

  स_रखो(buf, 0, आकार(*buf));
  साठी(i = 0; spec && i < spec->मोजणी; ++i) {
    len = म_माप(spec->entries[i]);
    जर(len >= ALPN_NAME_MAX)
      परत CURLE_FAILED_INIT;
    जर(off + len + 2 >= आकार(buf->data))
      परत CURLE_FAILED_INIT;
    जर(off)
      buf->data[off++] = ',';
    स_नकल(buf->data + off, spec->entries[i], len);
    off += len;
  }
  buf->data[off] = '\0';
  buf->len = (पूर्णांक)off;
  परत CURLE_OK;
}

bool Curl_alpn_contains_proto(स्थिरांक संरचना alpn_spec *spec,
                              स्थिरांक अक्षर *proto)
{
  माप_प्रकार i, plen = proto ? म_माप(proto) : 0;
  साठी(i = 0; spec && plen && i < spec->मोजणी; ++i) {
    माप_प्रकार slen = म_माप(spec->entries[i]);
    जर((slen == plen) && !स_भेद(proto, spec->entries[i], plen))
      परत TRUE;
  }
  परत FALSE;
}

शून्य Curl_alpn_restrict_to(संरचना alpn_spec *spec, स्थिरांक अक्षर *proto)
{
  माप_प्रकार plen = म_माप(proto);
  DEBUGASSERT(plen < आकार(spec->entries[0]));
  जर(plen < आकार(spec->entries[0])) {
    स_नकल(spec->entries[0], proto, plen + 1);
    spec->मोजणी = 1;
  }
}

शून्य Curl_alpn_copy(संरचना alpn_spec *dest, स्थिरांक संरचना alpn_spec *src)
{
  जर(src)
    स_नकल(dest, src, आकार(*dest));
  अन्यथा
    स_रखो(dest, 0, आकार(*dest));
}

CURLcode Curl_alpn_set_negotiated(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *data,
                                  संरचना ssl_connect_data *connssl,
                                  स्थिरांक अचिन्हित अक्षर *proto,
                                  माप_प्रकार proto_len)
{
  CURLcode result = CURLE_OK;
  (शून्य)cf;

  जर(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    जर(!proto_len) {
      failf(data, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      result = CURLE_SSL_CONNECT_ERROR;
      जा out;
    }
    अन्यथा जर(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      result = CURLE_SSL_CONNECT_ERROR;
      जा out;
    }
    अन्यथा जर((म_माप(connssl->negotiated.alpn) != proto_len) ||
            स_भेद(connssl->negotiated.alpn, proto, proto_len)) {
      failf(data, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (पूर्णांक)proto_len, proto);
      result = CURLE_SSL_CONNECT_ERROR;
      जा out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(data, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    जा out;
  }

  जर(proto && proto_len) {
    जर(स_प्रथम(proto, '\0', proto_len)) {
      failf(data, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      result = CURLE_SSL_CONNECT_ERROR;
      जा out;
    }
    connssl->negotiated.alpn = curlx_memdup0((स्थिरांक अक्षर *)proto, proto_len);
    जर(!connssl->negotiated.alpn)
      परत CURLE_OUT_OF_MEMORY;
  }

  जर(proto && proto_len) {
    जर(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_ALPN_DEFERRED, (पूर्णांक)proto_len, proto);
    अन्यथा
      infof(data, VTLS_INFOF_ALPN_ACCEPTED, (पूर्णांक)proto_len, proto);
  }
  अन्यथा {
    जर(connssl->state == ssl_connection_deferred)
      infof(data, VTLS_INFOF_NO_ALPN_DEFERRED);
    अन्यथा
      infof(data, VTLS_INFOF_NO_ALPN);
  }

out:
  परत result;
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

  जर(!early_data_allowed) {
    CURL_TRC_CF(data, cf, "SSL session does not allow earlydata");
  }
  अन्यथा जर(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(data, cf, "SSL session has different ALPN, no early data");
  }
  अन्यथा {
    infof(data, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    result = Curl_alpn_set_negotiated(cf, data, connssl,
                                      (स्थिरांक अचिन्हित अक्षर *)scs->alpn,
                                      scs->alpn ? म_माप(scs->alpn) : 0);
    *do_early_data = !result;
  }
  परत result;
}

संरचना Curl_ssl_session *Curl_ssl_get_cf_session(संरचना Curl_easy *data,
                                                 स्थिरांक संरचना Curl_cftype *cft,
                                                 int8_t sockindex)
{
  जर(data->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#अन्यथा
     (cft == &Curl_cft_ssl)) {
#endif
    संरचना Curl_cfilter *cf1 = data->conn->cfilter[sockindex];
    साठी(; cf1; cf1 = cf1->next) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      जर(cf1->cft == cft)
        थांबा;
    }
    जर(cf1) {
      संरचना ssl_connect_data *connssl = cf1->ctx;
      जर(connssl)
        परत connssl->session;
    }
  }
  परत रिक्त;
}

#endif /* USE_SSL */
