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

#यदि_परिभाषितम् USE_APPLE_SECTRUST
#अन्तर्भाव <Security/Security.h>
#यदि_अन्तः

#यदि_परिभाषितम् USE_SSL
#यदि !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#यदि_परिभाषितम् USE_HTTP2
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
स्थिर ध्रुव संरचना alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#यदि_अन्तः /* USE_HTTP2 */

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
#यदि_परिभाषितम् USE_HTTP2
  यदि(wanted & CURL_HTTP_V2x) {
    यदि(wanted & CURL_HTTP_V1x)
      प्रत्यागम (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    प्रत्यागम &ALPN_SPEC_H2;
  }
#अन्यथा
  (शून्य)wanted;
  (शून्य)preferred;
#यदि_अन्तः
  प्रत्यागम &ALPN_SPEC_H11;
}
#यदि_अन्तः /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#यदि_अन्तः /* USE_SSL */

#यदि_परिभाषितम् USE_SSL
स्थिर पूर्णाङ्क multissl_सज्जता(ध्रुव संरचना Curl_ssl *backend);
#यदि_अन्तः

curl_sslbackend Curl_ssl_backend(शून्य)
{
#यदि_परिभाषितम् USE_SSL
  multissl_सज्जता(रिक्त);
  प्रत्यागम Curl_ssl->info.id;
#अन्यथा
  प्रत्यागम CURLSSLBACKEND_NONE;
#यदि_अन्तः
}

#यदि_परिभाषितम् USE_SSL

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
                            संरचना Curl_easy *दत्त)
{
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  यदि(conn_config->संस्करणम्_2 >= CURL_SSLVERSION_चरमः) {
    failf(दत्त, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    प्रत्यागम FALSE;
  }

  विकल्प(conn_config->संस्करणम्_max) {
  पक्ष CURL_SSLVERSION_MAX_NONE:
  पक्ष CURL_SSLVERSION_MAX_DEFAULT:
    भङ्ग;

  स्वभाव:
    यदि((conn_config->संस्करणम्_max >> 16) < conn_config->संस्करणम्_2) {
      failf(दत्त, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      प्रत्यागम FALSE;
    }
  }

  प्रत्यागम TRUE;
}

स्थिर संरचना ssl_connect_दत्त *cf_ctx_new(संरचना Curl_easy *दत्त,
                                           ध्रुव संरचना alpn_spec *alpn)
{
  संरचना ssl_connect_दत्त *ctx;

  (शून्य)दत्त;
  ctx = curlx_calloc(1, परिमाण(*ctx));
  यदि(!ctx)
    प्रत्यागम रिक्त;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_दत्त);
  यदि(!ctx->backend) {
    curlx_free(ctx);
    प्रत्यागम रिक्त;
  }
  प्रत्यागम ctx;
}

स्थिर शून्य cf_ctx_free(संरचना ssl_connect_दत्त *ctx)
{
  यदि(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(संरचना Curl_easy *दत्त, int8_t sockindex,
                                      संरचना dynbuf *binding)
{
  यदि(Curl_ssl->get_channel_binding)
    प्रत्यागम Curl_ssl->get_channel_binding(दत्त, sockindex, binding);
  प्रत्यागम CURLE_OK;
}

शून्य Curl_ssl_close_all(संरचना Curl_easy *दत्त)
{
  यदि(Curl_ssl->close_all)
    Curl_ssl->close_all(दत्त);
}

CURLcode Curl_ssl_adjust_pollset(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 संरचना easy_pollset *ps)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;

  यदि(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->अग्र, दत्त);
    CURLcode परिणाम = CURLE_OK;
    यदि(sock != CURL_SOCKET_BAD) {
      यदि(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        परिणाम = Curl_pollset_समुच्चयः_बहिर्गमनम्_only(दत्त, ps, sock);
        CURL_TRC_CF(दत्त, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      अन्यथा {
        परिणाम = Curl_pollset_समुच्चयः_अन्तर्_only(दत्त, ps, sock);
        CURL_TRC_CF(दत्त, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    प्रत्यागम परिणाम;
  }
  प्रत्यागम CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
संरचना curl_slist *Curl_ssl_engines_list(संरचना Curl_easy *दत्त)
{
  यदि(Curl_ssl->engines_list)
    प्रत्यागम Curl_ssl->engines_list(दत्त);
  प्रत्यागम रिक्त;
}

स्थिर size_t multissl_संस्करणम्(वर्ण *बफर, size_t आकार);

शून्य Curl_ssl_संस्करणम्(वर्ण *बफर, size_t आकार)
{
#यदि_परिभाषितम् CURL_WITH_MULTI_SSL
  (शून्य)multissl_संस्करणम्(बफर, आकार);
#अन्यथा
  (शून्य)Curl_ssl->संस्करणम्_2(बफर, आकार);
#यदि_अन्तः
}

शून्य Curl_ssl_free_certinfo(संरचना Curl_easy *दत्त)
{
  संरचना curl_certinfo *ci = &दत्त->info.certs;

  यदि(ci->अङ्कः_सम्बन्धि_certs) {
    /* free all individual lists used */
    पूर्णाङ्क i;
    प्रति(i = 0; i < ci->अङ्कः_सम्बन्धि_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = रिक्त;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->अङ्कः_सम्बन्धि_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(संरचना Curl_easy *दत्त, पूर्णाङ्क अङ्कः)
{
  संरचना curl_certinfo *ci = &दत्त->info.certs;
  संरचना curl_slist **सारणी;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(दत्त);

  /* Allocate the required certificate information structures */
  सारणी = curlx_calloc((size_t)अङ्कः, परिमाण(संरचना curl_slist *));
  यदि(!सारणी)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  ci->अङ्कः_सम्बन्धि_certs = अङ्कः;
  ci->certinfo = सारणी;

  प्रत्यागम CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_प्रक्षिप_certinfo_len(संरचना Curl_easy *दत्त,
                                    पूर्णाङ्क certnum,
                                    ध्रुव वर्ण *label,
                                    ध्रुव वर्ण *मानम्,
                                    size_t valuelen)
{
  संरचना curl_certinfo *ci = &दत्त->info.certs;
  संरचना curl_slist *nl;
  CURLcode परिणाम = CURLE_OK;
  संरचना dynbuf build;

  DEBUGASSERT(certnum < ci->अङ्कः_सम्बन्धि_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  यदि(curlx_dyn_सङ्कलय(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, मानम्, valuelen))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  यदि(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  ci->certinfo[certnum] = nl;
  प्रत्यागम परिणाम;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(संरचना Curl_easy *दत्त,
                         अचिह्नित वर्ण *बफर, size_t दीर्घता)
{
  DEBUGASSERT(दीर्घता == परिमाण(पूर्णाङ्क));
  यदि(Curl_ssl->random)
    प्रत्यागम Curl_ssl->random(दत्त, बफर, दीर्घता);
  अन्यथा
    प्रत्यागम CURLE_NOT_BUILT_अन्तर्;
}

/*
 * Public key pem to der conversion
 */

स्थिर CURLcode pubkey_pem_गन्तव्यम्_der(ध्रुव वर्ण *pem,
                                  अचिह्नित वर्ण **der, size_t *der_len)
{
  ध्रुव वर्ण *begin_pos, *अन्तः_pos;
  size_t pem_गणनम्, pem_len;
  CURLcode परिणाम;
  संरचना dynbuf pbuf;

  /* if no pem, exit. */
  यदि(!pem)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_आकार);

  begin_pos = वाक्ये_अन्विष(pem, "-----BEGIN PUBLIC KEY-----");
  यदि(!begin_pos)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  pem_गणनम् = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  यदि(pem_गणनम् && '\n' != pem[pem_गणनम् - 1])
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_गणनम् += 26;

  /* Invalid if not directly following \n */
  अन्तः_pos = वाक्ये_अन्विष(pem + pem_गणनम्, "\n-----END PUBLIC KEY-----");
  यदि(!अन्तः_pos)
    प्रत्यागम CURLE_BAD_CONTENT_ENCODING;

  pem_len = अन्तः_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  यावत्(pem_गणनम् < pem_len) {
    यदि('\n' != pem[pem_गणनम्] && '\r' != pem[pem_गणनम्]) {
      परिणाम = curlx_dyn_addn(&pbuf, &pem[pem_गणनम्], 1);
      यदि(परिणाम)
        प्रत्यागम परिणाम;
    }
    ++pem_गणनम्;
  }

  यदि(curlx_dyn_len(&pbuf)) {
    परिणाम = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  अन्यथा
    परिणाम = CURLE_BAD_CONTENT_ENCODING;

  प्रत्यागम परिणाम;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *pinnedpubkey,
                              ध्रुव अचिह्नित वर्ण *pubkey, size_t pubkeylen)
{
  CURLcode परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  यदि(!pinnedpubkey)
    प्रत्यागम CURLE_OK;
  यदि(!pubkey || !pubkeylen)
    प्रत्यागम परिणाम;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  यदि(!वाक्ये_तुलय(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    वर्ण *cert_hash = रिक्त;
    ध्रुव वर्ण *pinned_hash, *अन्तः_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    अचिह्नित वर्ण *sha256sumdigest;

    यदि(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      प्रत्यागम परिणाम;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_दीर्घता);
    यदि(!sha256sumdigest)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_दीर्घता);

    यदि(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_दीर्घता,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    यदि(encode)
      प्रत्यागम encode;

    infof(दत्त, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    यावत्(pinned_hash &&
          !वाक्ये_तुलय(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      अन्तः_pos = strchr(pinned_hash, ';');
      pinned_hash_len = अन्तः_pos ?
                        (size_t)(अन्तः_pos - pinned_hash) : वाक्यदैर्घ्यम्(pinned_hash);

      /* compare base64 sha256 digests */
      यदि(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(दत्त, "public key hash matches pinned value"));
        परिणाम = CURLE_OK;
        भङ्ग;
      }

      DEBUGF(infof(दत्त, "public key hash does not match 'sha256//%.*s'",
                   (पूर्णाङ्क)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = अन्तः_pos ? (अन्तः_pos + 1) : रिक्त;
    }
    curlx_safefree(cert_hash);
  }
  अन्यथा {
    दीर्घ filesize;
    size_t आकार, pem_len;
    CURLcode pem_read;
    संरचना dynbuf buf;
    अचिह्नित वर्ण *pem_ptr = रिक्त;
    size_t वामः;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    यदि(!fp)
      प्रत्यागम परिणाम;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_आकार);

    /* Determine the file's size */
    यदि(fseek(fp, 0, SEEK_END))
      गच्छ अन्तः;
    filesize = ftell(fp);
    यदि(fseek(fp, 0, SEEK_SET))
      गच्छ अन्तः;
    यदि(filesize < 0 || filesize > MAX_PINNED_PUBKEY_आकार)
      गच्छ अन्तः;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    आकार = curlx_sotouz((curl_off_t)filesize);
    यदि(pubkeylen > आकार)
      गच्छ अन्तः;

    /*
     * Read the file into the dynbuf
     */
    वामः = आकार;
    कुरु {
      वर्ण बफर[1024];
      size_t want = वामः > परिमाण(बफर) ? परिमाण(बफर) : वामः;
      यदि(fread(बफर, 1, want, fp) != want)
        गच्छ अन्तः;
      यदि(curlx_dyn_addn(&buf, बफर, want))
        गच्छ अन्तः;
      वामः -= want;
    } यावत्(वामः);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    यदि(pubkeylen == आकार) {
      यदि(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        परिणाम = CURLE_OK;
      गच्छ अन्तः;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_गन्तव्यम्_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    यदि(pem_read)
      गच्छ अन्तः;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    यदि(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      परिणाम = CURLE_OK;
अन्तः:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  प्रत्यागम परिणाम;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_स्थितिः_request(शून्य)
{
  यदि(Curl_ssl->cert_स्थितिः_request)
    प्रत्यागम Curl_ssl->cert_स्थितिः_request();
  प्रत्यागम FALSE;
}

स्थिर पूर्णाङ्क multissl_init(शून्य)
{
  यदि(multissl_सज्जता(रिक्त))
    प्रत्यागम 1;
  यदि(Curl_ssl->init)
    प्रत्यागम Curl_ssl->init();
  प्रत्यागम 1;
}

स्थिर CURLcode multissl_random(संरचना Curl_easy *दत्त,
                                अचिह्नित वर्ण *entropy, size_t दीर्घता)
{
  यदि(multissl_सज्जता(रिक्त))
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_ssl->random(दत्त, entropy, दीर्घता);
}

स्थिर CURLcode multissl_connect(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  यदि(multissl_सज्जता(रिक्त))
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_ssl->do_connect(cf, दत्त, समाप्तम्);
}

स्थिर CURLcode multissl_adjust_pollset(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        संरचना easy_pollset *ps)
{
  यदि(multissl_सज्जता(रिक्त))
    प्रत्यागम CURLE_OK;
  प्रत्यागम Curl_ssl->adjust_pollset(cf, दत्त, ps);
}

स्थिर शून्य *multissl_get_internals(संरचना ssl_connect_दत्त *connssl,
                                    CURLINFO info)
{
  यदि(multissl_सज्जता(रिक्त))
    प्रत्यागम रिक्त;
  प्रत्यागम Curl_ssl->get_internals(connssl, info);
}

स्थिर शून्य multissl_close(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  यदि(multissl_सज्जता(रिक्त))
    प्रत्यागम;
  Curl_ssl->close(cf, दत्त);
}

स्थिर CURLcode multissl_recv_plain(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    वर्ण *buf, size_t len, size_t *pnread)
{
  यदि(multissl_सज्जता(रिक्त))
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_ssl->recv_plain(cf, दत्त, buf, len, pnread);
}

स्थिर CURLcode multissl_send_plain(संरचना Curl_cfilter *cf,
                                    संरचना Curl_easy *दत्त,
                                    ध्रुव शून्य *mem, size_t len,
                                    size_t *pnwritten)
{
  यदि(multissl_सज्जता(रिक्त))
    प्रत्यागम CURLE_FAILED_INIT;
  प्रत्यागम Curl_ssl->send_plain(cf, दत्त, mem, len, pnwritten);
}

स्थिर ध्रुव संरचना Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  रिक्त,                              /* cleanup */
  multissl_संस्करणम्,                  /*संस्करणम्*/
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
#यदि_परिभाषितम् CURL_WITH_MULTI_SSL
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
#दोषः "Missing struct Curl_ssl for selected SSL backend"
#यदि_अन्तः

स्थिर ध्रुव संरचना Curl_ssl *उपलब्धम्_backends[] = {
#यदि_परिभाषितम् USE_WOLFSSL
  &Curl_ssl_wolfssl,
#यदि_अन्तः
#यदि_परिभाषितम् USE_GNUTLS
  &Curl_ssl_gnutls,
#यदि_अन्तः
#यदि_परिभाषितम् USE_MBEDTLS
  &Curl_ssl_mbedtls,
#यदि_अन्तः
#यदि_परिभाषितम् USE_OPENSSL
  &Curl_ssl_openssl,
#यदि_अन्तः
#यदि_परिभाषितम् USE_SCHANNEL
  &Curl_ssl_schannel,
#यदि_अन्तः
#यदि_परिभाषितम् USE_RUSTLS
  &Curl_ssl_rustls,
#यदि_अन्तः
  रिक्त
};

/* Global cleanup */
शून्य Curl_ssl_cleanup(शून्य)
{
  यदि(init_ssl) {
    /* only cleanup if we did a previous init */
    यदि(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#यदि_परिभाषितम् CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#यदि_अन्तः
    init_ssl = FALSE;
  }
}

स्थिर size_t multissl_संस्करणम्(वर्ण *बफर, size_t आकार)
{
  स्थिर ध्रुव संरचना Curl_ssl *selected;
  स्थिर वर्ण backends[200];
  स्थिर size_t backends_len;
  ध्रुव संरचना Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? उपलब्धम्_backends[0] : Curl_ssl;

  यदि(current != selected) {
    वर्ण *p = backends;
    ध्रुव वर्ण *अन्तः = backends + परिमाण(backends);
    पूर्णाङ्क i;

    selected = current;

    backends[0] = '\0';

    प्रति(i = 0; उपलब्धम्_backends[i]; ++i) {
      वर्ण vb[200];
      bool paren = (selected != उपलब्धम्_backends[i]);

      यदि(उपलब्धम्_backends[i]->संस्करणम्_2(vb, परिमाण(vb))) {
        p += curl_msnprintf(p, अन्तः - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  यदि(आकार) {
    curlx_strcopy(बफर, आकार, backends, backends_len);
  }
  प्रत्यागम 0;
}

स्थिर पूर्णाङ्क multissl_सज्जता(ध्रुव संरचना Curl_ssl *backend)
{
  पूर्णाङ्क i;
  वर्ण *env;

  यदि(Curl_ssl != &Curl_ssl_multi)
    प्रत्यागम 1;

  यदि(backend) {
    Curl_ssl = backend;
    प्रत्यागम 0;
  }

  यदि(!उपलब्धम्_backends[0])
    प्रत्यागम 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  यदि(env) {
    प्रति(i = 0; उपलब्धम्_backends[i]; i++) {
      यदि(curl_strequal(env, उपलब्धम्_backends[i]->info.name)) {
        Curl_ssl = उपलब्धम्_backends[i];
        curlx_free(env);
        प्रत्यागम 0;
      }
    }
  }

#यदि_परिभाषितम् CURL_DEFAULT_SSL_BACKEND
  प्रति(i = 0; उपलब्धम्_backends[i]; i++) {
    यदि(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     उपलब्धम्_backends[i]->info.name)) {
      Curl_ssl = उपलब्धम्_backends[i];
      curlx_free(env);
      प्रत्यागम 0;
    }
  }
#यदि_अन्तः

  /* Fall back to first available backend */
  Curl_ssl = उपलब्धम्_backends[0];
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
    *avail = (ध्रुव curl_ssl_backend **)&उपलब्धम्_backends;

  यदि(Curl_ssl != &Curl_ssl_multi)
    प्रत्यागम id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#यदि_परिभाषितम् CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#अन्यथा
           CURLSSLSET_UNKNOWN_BACKEND;
#यदि_अन्तः

  प्रति(i = 0; उपलब्धम्_backends[i]; i++) {
    यदि(उपलब्धम्_backends[i]->info.id == id ||
       (name && curl_strequal(उपलब्धम्_backends[i]->info.name, name))) {
      multissl_सज्जता(उपलब्धम्_backends[i]);
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

#यदि_अन्तः /* !USE_SSL */

#यदि_परिभाषितम् USE_SSL

शून्य Curl_ssl_peer_cleanup(संरचना ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_कुञ्चिका);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

स्थिर ssl_peer_type get_peer_type(ध्रुव वर्ण *hostname)
{
  यदि(hostname && hostname[0]) {
#यदि_परिभाषितम् USE_IPV6
    संरचना in6_addr addr;
#अन्यथा
    संरचना in_addr addr;
#यदि_अन्तः
    यदि(curlx_inet_pton(AF_INET, hostname, &addr))
      प्रत्यागम CURL_SSL_PEER_IPV4;
#यदि_परिभाषितम् USE_IPV6
    अन्यथा यदि(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      प्रत्यागम CURL_SSL_PEER_IPV6;
    }
#यदि_अन्तः
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
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

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
    size_t len = वाक्यदैर्घ्यम्(origin->hostname);
    यदि(len && (origin->hostname[len - 1] == '.'))
      len--;
    यदि(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      यदि(!ssl_peer->sni)
        गच्छ बहिर्गमनम्;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  परिणाम = Curl_ssl_peer_कुञ्चिका_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_कुञ्चिका);

बहिर्गमनम्:
  यदि(परिणाम)
    Curl_ssl_peer_cleanup(ssl_peer);
  प्रत्यागम परिणाम;
}

स्थिर शून्य ssl_cf_destroy(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  यदि(connssl) {
    connssl->ssl_impl->close(cf, दत्त);
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
                               संरचना Curl_easy *दत्त,
                               bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम;

  यदि(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }

  यदि(!cf->अग्र || !connssl->peer.origin) {
    *समाप्तम् = FALSE;
    प्रत्यागम CURLE_FAILED_INIT;
  }

  यदि(!cf->अग्र->connected) {
    परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, दत्त, समाप्तम्);
    यदि(परिणाम || !*समाप्तम्)
      प्रत्यागम परिणाम;
  }

  CF_दत्त_SAVE(save, cf, दत्त);
  CURL_TRC_CF(दत्त, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *समाप्तम् = FALSE;

  यदि(!connssl->prefs_checked) {
    यदि(!ssl_prefs_check(cf, दत्त)) {
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ बहिर्गमनम्;
    }
    connssl->prefs_checked = TRUE;
  }

  परिणाम = connssl->ssl_impl->do_connect(cf, दत्त, समाप्तम्);

  यदि(!परिणाम && *समाप्तम्) {
    cf->connected = TRUE;
    यदि(connssl->state == ssl_connection_complete) {
      connssl->handshake_समाप्तम् = *Curl_pgrs_now(दत्त);
    }
    यदि(Curl_tls_keylog_enabled()) {
      infof(दत्त, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#यदि_परिभाषितम् LIBRESSL_संस्करणम्_सङ्ख्या
      infof(दत्त, "Note LibreSSL only supports SSLKEYLOGFILE for TLS <= 1.2");
#यदि_अन्तः
    }
    /* Connection can be deferred when sending early data */
    DEBUGASSERT(connssl->state == ssl_connection_complete ||
                connssl->state == ssl_connection_deferred);
    DEBUGASSERT(connssl->state != ssl_connection_deferred ||
                connssl->earlydata_state > ssl_earlydata_none);
  }
बहिर्गमनम्:
  CURL_TRC_CF(दत्त, cf, "cf_connect() -> %d, done=%d", (पूर्णाङ्क)परिणाम, *समाप्तम्);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssl_cf_समुच्चयः_earlydata(संरचना Curl_cfilter *cf,
                                     संरचना Curl_easy *दत्त,
                                     ध्रुव शून्य *buf, size_t blen)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_रिक्तानि(&connssl->earlydata));
  यदि(blen) {
    यदि(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    परिणाम = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(दत्त, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssl_cf_connect_deferred(संरचना Curl_cfilter *cf,
                                        संरचना Curl_easy *दत्त,
                                        ध्रुव शून्य *buf, size_t blen,
                                        bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *समाप्तम् = FALSE;
  यदि(connssl->earlydata_state == ssl_earlydata_await) {
    परिणाम = ssl_cf_समुच्चयः_earlydata(cf, दत्त, buf, blen);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  परिणाम = ssl_cf_connect(cf, दत्त, समाप्तम्);

  यदि(!परिणाम && *समाप्तम्) {
    यदि(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(दत्त, TIMER_APPCONNECT, connssl->handshake_समाप्तम्);
      connssl->stats_reported = TRUE;
    }
    विकल्प(connssl->earlydata_state) {
    पक्ष ssl_earlydata_none:
      भङ्ग;
    पक्ष ssl_earlydata_accepted:
      यदि(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_दत्त(दत्त, (curl_off_t)connssl->earlydata_skip);
      infof(दत्त, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      भङ्ग;
    पक्ष ssl_earlydata_rejected:
      यदि(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_दत्त(दत्त, -(curl_off_t)connssl->earlydata_skip);
      infof(दत्त, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      भङ्ग;
    स्वभाव:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(रिक्त);
      भङ्ग;
    }
  }
  प्रत्यागम परिणाम;
}

स्थिर bool ssl_cf_दत्त_pending(संरचना Curl_cfilter *cf,
                                ध्रुव संरचना Curl_easy *दत्त)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना cf_call_दत्त save;
  bool pending;

  CF_दत्त_SAVE(save, cf, दत्त);
  यदि(connssl->ssl_impl->दत्त_pending &&
     connssl->ssl_impl->दत्त_pending(cf, दत्त))
    pending = TRUE;
  अन्यथा
    pending = cf->अग्र->cft->has_दत्त_pending(cf->अग्र, दत्त);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम pending;
}

स्थिर CURLcode ssl_cf_send(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            ध्रुव uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम = CURLE_OK;

  (शून्य)eos;
  *pnwritten = 0;
  CF_दत्त_SAVE(save, cf, दत्त);

  यदि(connssl->state == ssl_connection_deferred) {
    bool समाप्तम् = FALSE;
    परिणाम = ssl_cf_connect_deferred(cf, दत्त, buf, blen, &समाप्तम्);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    अन्यथा यदि(!समाप्तम्) {
      परिणाम = CURLE_AGAIN;
      गच्छ बहिर्गमनम्;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  यदि(connssl->earlydata_skip) {
    यदि(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      परिणाम = CURLE_OK;
      *pnwritten = blen;
      गच्छ बहिर्गमनम्;
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
    परिणाम = connssl->ssl_impl->send_plain(cf, दत्त, buf, blen, &nwritten);
    यदि(!परिणाम)
      *pnwritten += nwritten;
  }

बहिर्गमनम्:
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssl_cf_recv(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त, वर्ण *buf, size_t len,
                            size_t *pnread)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम = CURLE_OK;

  CF_दत्त_SAVE(save, cf, दत्त);
  *pnread = 0;
  यदि(connssl->state == ssl_connection_deferred) {
    bool समाप्तम् = FALSE;
    परिणाम = ssl_cf_connect_deferred(cf, दत्त, रिक्त, 0, &समाप्तम्);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    अन्यथा यदि(!समाप्तम्) {
      परिणाम = CURLE_AGAIN;
      गच्छ बहिर्गमनम्;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  परिणाम = connssl->ssl_impl->recv_plain(cf, दत्त, buf, len, pnread);

बहिर्गमनम्:
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssl_cf_shutdown(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  *समाप्तम् = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  यदि(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_अधस्तात्) {
    संरचना cf_call_दत्त save;

    CF_दत्त_SAVE(save, cf, दत्त);
    परिणाम = connssl->ssl_impl->shut_अधस्तात्(cf, दत्त, TRUE, समाप्तम्);
    CURL_TRC_CF(दत्त, cf, "cf_shutdown -> %d, done=%d", (पूर्णाङ्क)परिणाम, *समाप्तम्);
    CF_दत्त_RESTORE(cf, save);
    cf->shutdown = (परिणाम || *समाप्तम्);
  }
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssl_cf_adjust_pollset(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      संरचना easy_pollset *ps)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम;

  CF_दत्त_SAVE(save, cf, दत्त);
  परिणाम = connssl->ssl_impl->adjust_pollset(cf, दत्त, ps);
  CF_दत्त_RESTORE(cf, save);
  प्रत्यागम परिणाम;
}

स्थिर CURLcode ssl_cf_query(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             पूर्णाङ्क query, पूर्णाङ्क *pres1, शून्य *pres2)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;

  विकल्प(query) {
  पक्ष CF_QUERY_SSL_INFO:
  पक्ष CF_QUERY_SSL_CTX_INFO:
    यदि(!Curl_ssl_cf_is_proxy(cf)) {
      संरचना curl_tlssessioninfo *info = pres2;
      संरचना cf_call_दत्त save;
      CF_दत्त_SAVE(save, cf, दत्त);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_दत्त_RESTORE(cf, save);
      प्रत्यागम CURLE_OK;
    }
    भङ्ग;
  पक्ष CF_QUERY_ALPN_NEGOTIATED: {
    ध्रुव वर्ण **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(दत्त, cf, "query ALPN: returning '%s'", *palpn);
    प्रत्यागम CURLE_OK;
  }
  स्वभाव:
    भङ्ग;
  }
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, दत्त, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

स्थिर CURLcode ssl_cf_cntrl(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             पूर्णाङ्क event, पूर्णाङ्क arg1, शून्य *arg2)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;

  (शून्य)arg1;
  (शून्य)arg2;
  (शून्य)दत्त;
  विकल्प(event) {
  पक्ष CF_CTRL_CONN_INFO_UPDATE:
    यदि(connssl->negotiated.alpn && !cf->sockindex) {
      यदि(!वाक्यं_तुलय("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_आलोकितम् = 11;
      अन्यथा यदि(!वाक्यं_तुलय("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_आलोकितम् = 20;
      अन्यथा यदि(!वाक्यं_तुलय("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_आलोकितम् = 30;
    }
    भङ्ग;
  पक्ष CF_CTRL_REPORT_STATS:
    यदि(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_समाप्तम्.tv_sec || connssl->handshake_समाप्तम्.tv_usec)) {
      Curl_pgrsTimeWas(दत्त, TIMER_APPCONNECT, connssl->handshake_समाप्तम्);
      connssl->stats_reported = TRUE;
    }
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर bool cf_ssl_is_alive(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त,
                            bool *निवेशः_pending)
{
  /*
   * This function tries to determine connection status.
   */
  प्रत्यागम cf->अग्र ?
    cf->अग्र->cft->is_alive(cf->अग्र, दत्त, निवेशः_pending) :
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
  ssl_cf_दत्त_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY

संरचना Curl_cftype Curl_cft_ssl_proxy = {
  "SSL-PROXY",
  CF_TYPE_SSL | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_दत्त_pending,
  ssl_cf_send,
  ssl_cf_recv,
  Curl_cf_def_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#यदि_अन्तः /* !CURL_DISABLE_PROXY */

स्थिर CURLcode cf_ssl_create(संरचना Curl_cfilter **pcf,
                              संरचना Curl_easy *दत्त,
                              संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना ssl_connect_दत्त *ctx;
  CURLcode परिणाम;

  DEBUGASSERT(दत्त->conn);

#यदि_परिभाषितम् CURL_DISABLE_HTTP
  (शून्य)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->द्वयङ्काः.tls_enable_alpn);
  ctx = cf_ctx_new(दत्त, रिक्त);
#अन्यथा
  ctx = cf_ctx_new(दत्त, alpn_get_spec(दत्त->state.http_neg.wanted,
                                       दत्त->state.http_neg.preferred,
                                       (bool)दत्त->state.http_neg.only_10,
                                       (bool)conn->द्वयङ्काः.tls_enable_alpn));
#यदि_अन्तः
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

बहिर्गमनम्:
  यदि(परिणाम)
    cf_ctx_free(ctx);
  *pcf = परिणाम ? रिक्त : cf;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_ssl_peer_init(संरचना Curl_cfilter *cf,
                                 संरचना Curl_peer *origin,
                                 संरचना Curl_peer *peer,
                                 संरचना ssl_filter_config *sslc)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  वर्ण tls_id[80];
  connssl->ssl_impl->संस्करणम्_2(tls_id, परिमाण(tls_id) - 1);
  प्रत्यागम Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_सङ्कलय(संरचना Curl_easy *दत्त,
                              संरचना Curl_peer *origin,
                              संरचना connectdata *conn,
                              int8_t sockindex)
{
  संरचना Curl_cfilter *cf;
  संरचना Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->द्वारा_peer2 : conn->द्वारा_peer;
  CURLcode परिणाम;

  परिणाम = cf_ssl_create(&cf, दत्त, conn);
  यदि(!परिणाम)
    परिणाम = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  यदि(!परिणाम)
    Curl_conn_cf_सङ्कलय(दत्त, conn, sockindex, cf);
  अन्यथा यदि(cf)
    Curl_conn_cf_discard_chain(&cf, दत्त);
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_ssl_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                  संरचना Curl_easy *दत्त,
                                  संरचना Curl_peer *origin,
                                  संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_ssl_create(&cf, दत्त, cf_at->conn);
  यदि(!परिणाम)
    परिणाम = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  यदि(!परिणाम) {
    Curl_conn_cf_निवेशय_after(cf_at, cf);
#यदि defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    यदि((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      परिणाम = Curl_conn_dns_सङ्कलय_https_resolve(दत्त, cf->conn, cf->sockindex,
                                               origin);
    }
#यदि_अन्तः /* USE_HTTPSRR && USE_ECH */
  }
  अन्यथा यदि(cf)
    Curl_conn_cf_discard_chain(&cf, दत्त);
  प्रत्यागम परिणाम;
}

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY

स्थिर CURLcode cf_ssl_proxy_create(संरचना Curl_cfilter **pcf,
                                    संरचना Curl_easy *दत्त,
                                    संरचना connectdata *conn)
{
  संरचना Curl_cfilter *cf = रिक्त;
  संरचना ssl_connect_दत्त *ctx;
  CURLcode परिणाम;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)दत्त->समुच्चयः.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (शून्य)conn;
#यदि_परिभाषितम् USE_HTTP2
  यदि(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#यदि_अन्तः

  ctx = cf_ctx_new(दत्त, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  यदि(!ctx) {
    परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

बहिर्गमनम्:
  यदि(परिणाम)
    cf_ctx_free(ctx);
  *pcf = परिणाम ? रिक्त : cf;
  प्रत्यागम परिणाम;
}

CURLcode Curl_cf_ssl_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                        संरचना Curl_easy *दत्त,
                                        संरचना Curl_peer *peer)
{
  संरचना Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_ssl_proxy_create(&cf, दत्त, cf_at->conn);
  यदि(!परिणाम)
    परिणाम = cf_ssl_peer_init(cf, peer, रिक्त, &cf_at->conn->proxy_ssl_config);
  यदि(!परिणाम)
    Curl_conn_cf_निवेशय_after(cf_at, cf);
  अन्यथा यदि(cf)
    Curl_conn_cf_discard_chain(&cf, दत्त);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(संरचना Curl_easy *दत्त, अचिह्नित पूर्णाङ्क ssl_option)
{
  (शून्य)दत्त;
  प्रत्यागम (Curl_ssl->supports & ssl_option);
}

#यदि_परिभाषितम् USE_ECH
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
#यदि_अन्तः

स्थिर CURLcode vtls_shutdown_blocking(संरचना Curl_cfilter *cf,
                                       संरचना Curl_easy *दत्त,
                                       bool send_shutdown, bool *समाप्तम्)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  संरचना cf_call_दत्त save;
  CURLcode परिणाम = CURLE_OK;
  timediff_t timeout_ms;
  पूर्णाङ्क what, loop = 10;

  यदि(cf->shutdown) {
    *समाप्तम् = TRUE;
    प्रत्यागम CURLE_OK;
  }
  CF_दत्त_SAVE(save, cf, दत्त);

  *समाप्तम् = FALSE;
  यावत्(!परिणाम && !*समाप्तम् && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(दत्त, cf->conn, cf->sockindex);

    यदि(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(दत्त, "SSL shutdown timeout");
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      गच्छ बहिर्गमनम्;
    }

    परिणाम = connssl->ssl_impl->shut_अधस्तात्(cf, दत्त, send_shutdown, समाप्तम्);
    यदि(परिणाम || *समाप्तम्)
      गच्छ बहिर्गमनम्;

    यदि(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, दत्त, timeout_ms);
      यदि(what < 0) {
        /* fatal error */
        failf(दत्त, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        परिणाम = CURLE_RECV_ERROR;
        गच्छ बहिर्गमनम्;
      }
      अन्यथा यदि(what == 0) {
        /* timeout */
        failf(दत्त, "SSL shutdown timeout");
        परिणाम = CURLE_OPERATION_TIMEDOUT;
        गच्छ बहिर्गमनम्;
      }
      /* socket is readable or writable */
    }
  }
बहिर्गमनम्:
  CF_दत्त_RESTORE(cf, save);
  cf->shutdown = (परिणाम || *समाप्तम्);
  प्रत्यागम परिणाम;
}

CURLcode Curl_ssl_cfilter_remove(संरचना Curl_easy *दत्त,
                                 int8_t sockindex, bool send_shutdown)
{
  संरचना Curl_cfilter *cf, *शीर्ष;
  CURLcode परिणाम = CURLE_OK;

  शीर्ष = दत्त->conn ? दत्त->conn->cfilter[sockindex] : रिक्त;
  प्रति(cf = शीर्ष; cf; cf = cf->अग्र) {
    यदि(cf->cft == &Curl_cft_ssl) {
      bool समाप्तम्;
      CURL_TRC_CF(दत्त, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(दत्त, sockindex, 0);
      परिणाम = vtls_shutdown_blocking(cf, दत्त, send_shutdown, &समाप्तम्);
      Curl_cshutdn_clear_timer(दत्त, sockindex);
      यदि(!परिणाम && !समाप्तम्) /* blocking failed? */
        परिणाम = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, दत्त);
      CURL_TRC_CF(दत्त, cf, "shutdown and remove SSL, done -> %d",
                  (पूर्णाङ्क)परिणाम);
      भङ्ग;
    }
  }
  प्रत्यागम परिणाम;
}

bool Curl_ssl_cf_is_proxy(संरचना Curl_cfilter *cf)
{
  प्रत्यागम (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

संरचना ssl_easy_config *
Curl_ssl_cf_get_easy_config(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त)
{
#यदि_परिभाषितम् CURL_DISABLE_PROXY
  (शून्य)cf;
  प्रत्यागम &दत्त->समुच्चयः.ssl;
#अन्यथा
  प्रत्यागम Curl_ssl_cf_is_proxy(cf) ? &दत्त->समुच्चयः.proxy_ssl : &दत्त->समुच्चयः.ssl;
#यदि_अन्तः
}

संरचना ssl_filter_config *
Curl_ssl_cf_get_filter_config(संरचना Curl_cfilter *cf)
{
#यदि_परिभाषितम् CURL_DISABLE_PROXY
  प्रत्यागम &cf->conn->ssl_config;
#अन्यथा
  प्रत्यागम Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#यदि_अन्तः
}

CURLcode Curl_alpn_गन्तव्यम्_proto_buf(संरचना alpn_proto_buf *buf,
                                ध्रुव संरचना alpn_spec *spec)
{
  size_t i, len;
  पूर्णाङ्क off = 0;
  अचिह्नित वर्ण blen;

  स्मृतिं_पूरय(buf, 0, परिमाण(*buf));
  प्रति(i = 0; spec && i < spec->गणनम्; ++i) {
    len = वाक्यदैर्घ्यम्(spec->entries[i]);
    यदि(len >= ALPN_NAME_MAX)
      प्रत्यागम CURLE_FAILED_INIT;
    blen = (अचिह्नित वर्ण)len;
    यदि(off + blen + 1 >= (पूर्णाङ्क)परिमाण(buf->दत्त))
      प्रत्यागम CURLE_FAILED_INIT;
    buf->दत्त[off++] = blen;
    स्मृतिं_प्रतिलिख(buf->दत्त + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_alpn_गन्तव्यम्_proto_str(संरचना alpn_proto_buf *buf,
                                ध्रुव संरचना alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  स्मृतिं_पूरय(buf, 0, परिमाण(*buf));
  प्रति(i = 0; spec && i < spec->गणनम्; ++i) {
    len = वाक्यदैर्घ्यम्(spec->entries[i]);
    यदि(len >= ALPN_NAME_MAX)
      प्रत्यागम CURLE_FAILED_INIT;
    यदि(off + len + 2 >= परिमाण(buf->दत्त))
      प्रत्यागम CURLE_FAILED_INIT;
    यदि(off)
      buf->दत्त[off++] = ',';
    स्मृतिं_प्रतिलिख(buf->दत्त + off, spec->entries[i], len);
    off += len;
  }
  buf->दत्त[off] = '\0';
  buf->len = (पूर्णाङ्क)off;
  प्रत्यागम CURLE_OK;
}

bool Curl_alpn_अन्तर्भवति_proto(ध्रुव संरचना alpn_spec *spec,
                              ध्रुव वर्ण *proto)
{
  size_t i, plen = proto ? वाक्यदैर्घ्यम्(proto) : 0;
  प्रति(i = 0; spec && plen && i < spec->गणनम्; ++i) {
    size_t slen = वाक्यदैर्घ्यम्(spec->entries[i]);
    यदि((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      प्रत्यागम TRUE;
  }
  प्रत्यागम FALSE;
}

शून्य Curl_alpn_restrict_गन्तव्यम्(संरचना alpn_spec *spec, ध्रुव वर्ण *proto)
{
  size_t plen = वाक्यदैर्घ्यम्(proto);
  DEBUGASSERT(plen < परिमाण(spec->entries[0]));
  यदि(plen < परिमाण(spec->entries[0])) {
    स्मृतिं_प्रतिलिख(spec->entries[0], proto, plen + 1);
    spec->गणनम् = 1;
  }
}

शून्य Curl_alpn_copy(संरचना alpn_spec *dest, ध्रुव संरचना alpn_spec *src)
{
  यदि(src)
    स्मृतिं_प्रतिलिख(dest, src, परिमाण(*dest));
  अन्यथा
    स्मृतिं_पूरय(dest, 0, परिमाण(*dest));
}

CURLcode Curl_alpn_समुच्चयः_negotiated(संरचना Curl_cfilter *cf,
                                  संरचना Curl_easy *दत्त,
                                  संरचना ssl_connect_दत्त *connssl,
                                  ध्रुव अचिह्नित वर्ण *proto,
                                  size_t proto_len)
{
  CURLcode परिणाम = CURLE_OK;
  (शून्य)cf;

  यदि(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    यदि(!proto_len) {
      failf(दत्त, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि((वाक्यदैर्घ्यम्(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(दत्त, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (पूर्णाङ्क)proto_len, proto);
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ बहिर्गमनम्;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(दत्त, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    गच्छ बहिर्गमनम्;
  }

  यदि(proto && proto_len) {
    यदि(memchr(proto, '\0', proto_len)) {
      failf(दत्त, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      गच्छ बहिर्गमनम्;
    }
    connssl->negotiated.alpn = curlx_memdup0((ध्रुव वर्ण *)proto, proto_len);
    यदि(!connssl->negotiated.alpn)
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  यदि(proto && proto_len) {
    यदि(connssl->state == ssl_connection_deferred)
      infof(दत्त, VTLS_INFOF_ALPN_DEFERRED, (पूर्णाङ्क)proto_len, proto);
    अन्यथा
      infof(दत्त, VTLS_INFOF_ALPN_ACCEPTED, (पूर्णाङ्क)proto_len, proto);
  }
  अन्यथा {
    यदि(connssl->state == ssl_connection_deferred)
      infof(दत्त, VTLS_INFOF_NO_ALPN_DEFERRED);
    अन्यथा
      infof(दत्त, VTLS_INFOF_NO_ALPN);
  }

बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

CURLcode Curl_on_session_reuse(संरचना Curl_cfilter *cf,
                               संरचना Curl_easy *दत्त,
                               संरचना alpn_spec *alpns,
                               संरचना Curl_ssl_session *scs,
                               bool *do_early_दत्त, bool early_दत्त_allowed)
{
  संरचना ssl_connect_दत्त *connssl = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  *do_early_दत्त = FALSE;

  यदि(!early_दत्त_allowed) {
    CURL_TRC_CF(दत्त, cf, "SSL session does not allow earlydata");
  }
  अन्यथा यदि(!Curl_alpn_अन्तर्भवति_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(दत्त, cf, "SSL session has different ALPN, no early data");
  }
  अन्यथा {
    infof(दत्त, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    परिणाम = Curl_alpn_समुच्चयः_negotiated(cf, दत्त, connssl,
                                      (ध्रुव अचिह्नित वर्ण *)scs->alpn,
                                      scs->alpn ? वाक्यदैर्घ्यम्(scs->alpn) : 0);
    *do_early_दत्त = !परिणाम;
  }
  प्रत्यागम परिणाम;
}

संरचना Curl_ssl_session *Curl_ssl_get_cf_session(संरचना Curl_easy *दत्त,
                                                 ध्रुव संरचना Curl_cftype *cft,
                                                 int8_t sockindex)
{
  यदि(दत्त->conn &&
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#अन्यथा
     (cft == &Curl_cft_ssl)) {
#यदि_अन्तः
    संरचना Curl_cfilter *cf1 = दत्त->conn->cfilter[sockindex];
    प्रति(; cf1; cf1 = cf1->अग्र) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      यदि(cf1->cft == cft)
        भङ्ग;
    }
    यदि(cf1) {
      संरचना ssl_connect_दत्त *connssl = cf1->ctx;
      यदि(connssl)
        प्रत्यागम connssl->session;
    }
  }
  प्रत्यागम रिक्त;
}

#यदि_अन्तः /* USE_SSL */
