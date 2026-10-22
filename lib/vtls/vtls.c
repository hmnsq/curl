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

#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"

#అన్తర్భావ "vtls/vtls.h" /* generic SSL protos etc */
#అన్తర్భావ "vtls/vtls_int.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "vtls/keylog.h"

#అన్తర్భావ "vtls/openssl.h"        /* OpenSSL versions */
#అన్తర్భావ "vtls/gtls.h"           /* GnuTLS versions */
#అన్తర్భావ "vtls/wolfssl.h"        /* wolfSSL versions */
#అన్తర్భావ "vtls/schannel.h"       /* Schannel SSPI version */
#అన్తర్భావ "vtls/mbedtls.h"        /* mbedTLS versions */
#అన్తర్భావ "vtls/rustls.h"         /* Rustls versions */

#అన్తర్భావ "slist.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "strcase.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "curlx/win32-fopen.h"
#అన్తర్భావ "curl_sha256.h"
#అన్తర్భావ "curlx/base64.h"
#అన్తర్భావ "curlx/inet_pton.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "setopt.h"
#అన్తర్భావ "vdns/cf-dns.h"
#అన్తర్భావ "curlx/strdup.h"
#అన్తర్భావ "curlx/strcopy.h"

#నిర్వచితమైతే USE_APPLE_SECTRUST
#అన్తర్భావ <Security/Security.h>
#అయితే_ముగింపు

#నిర్వచితమైతే USE_SSL
#ఒకవేళ !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
స్థిర స్థిరరాశి నిర్మాణం alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
స్థిర స్థిరరాశి నిర్మాణం alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#నిర్వచితమైతే USE_HTTP2
స్థిర స్థిరరాశి నిర్మాణం alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
స్థిర స్థిరరాశి నిర్మాణం alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
స్థిర స్థిరరాశి నిర్మాణం alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#అయితే_ముగింపు /* USE_HTTP2 */

స్థిర స్థిరరాశి నిర్మాణం alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  ఒకవేళ(!use_alpn)
    తిరిగి ఖాళీ;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  ఒకవేళ(only_http_10 && (wanted & CURL_HTTP_V1x))
    తిరిగి &ALPN_SPEC_H10_H11;
#నిర్వచితమైతే USE_HTTP2
  ఒకవేళ(wanted & CURL_HTTP_V2x) {
    ఒకవేళ(wanted & CURL_HTTP_V1x)
      తిరిగి (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    తిరిగి &ALPN_SPEC_H2;
  }
#లేకపోతే
  (శూన్య)wanted;
  (శూన్య)preferred;
#అయితే_ముగింపు
  తిరిగి &ALPN_SPEC_H11;
}
#అయితే_ముగింపు /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#అయితే_ముగింపు /* USE_SSL */

#నిర్వచితమైతే USE_SSL
స్థిర పూర్ణాంకం multissl_అమరిక(స్థిరరాశి నిర్మాణం Curl_ssl *backend);
#అయితే_ముగింపు

curl_sslbackend Curl_ssl_backend(శూన్య)
{
#నిర్వచితమైతే USE_SSL
  multissl_అమరిక(ఖాళీ);
  తిరిగి Curl_ssl->info.id;
#లేకపోతే
  తిరిగి CURLSSLBACKEND_NONE;
#అయితే_ముగింపు
}

#నిర్వచితమైతే USE_SSL

/* "global" init done? */
స్థిర bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
పూర్ణాంకం Curl_ssl_init(శూన్య)
{
  /* make sure this is only done once */
  ఒకవేళ(init_ssl)
    తిరిగి 1;
  init_ssl = TRUE; /* never again */

  ఒకవేళ(Curl_ssl->init)
    తిరిగి Curl_ssl->init();
  తిరిగి 1;
}

స్థిర bool ssl_prefs_check(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  ఒకవేళ(conn_config->సంస్కరణ_2 >= CURL_SSLVERSION_చివరి) {
    failf(దత్తాంశం, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    తిరిగి FALSE;
  }

  ఎంపిక(conn_config->సంస్కరణ_max) {
  స్థితి CURL_SSLVERSION_MAX_NONE:
  స్థితి CURL_SSLVERSION_MAX_DEFAULT:
    ఆపు;

  అప్రమేయ:
    ఒకవేళ((conn_config->సంస్కరణ_max >> 16) < conn_config->సంస్కరణ_2) {
      failf(దత్తాంశం, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      తిరిగి FALSE;
    }
  }

  తిరిగి TRUE;
}

స్థిర నిర్మాణం ssl_connect_దత్తాంశం *cf_ctx_new(నిర్మాణం Curl_easy *దత్తాంశం,
                                           స్థిరరాశి నిర్మాణం alpn_spec *alpn)
{
  నిర్మాణం ssl_connect_దత్తాంశం *ctx;

  (శూన్య)దత్తాంశం;
  ctx = curlx_calloc(1, పరిమాణం(*ctx));
  ఒకవేళ(!ctx)
    తిరిగి ఖాళీ;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_దత్తాంశం);
  ఒకవేళ(!ctx->backend) {
    curlx_free(ctx);
    తిరిగి ఖాళీ;
  }
  తిరిగి ctx;
}

స్థిర శూన్య cf_ctx_free(నిర్మాణం ssl_connect_దత్తాంశం *ctx)
{
  ఒకవేళ(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                                      నిర్మాణం dynbuf *binding)
{
  ఒకవేళ(Curl_ssl->get_channel_binding)
    తిరిగి Curl_ssl->get_channel_binding(దత్తాంశం, sockindex, binding);
  తిరిగి CURLE_OK;
}

శూన్య Curl_ssl_close_all(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(Curl_ssl->close_all)
    Curl_ssl->close_all(దత్తాంశం);
}

CURLcode Curl_ssl_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;

  ఒకవేళ(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->తదుపరి, దత్తాంశం);
    CURLcode ఫలితం = CURLE_OK;
    ఒకవేళ(sock != CURL_SOCKET_BAD) {
      ఒకవేళ(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        ఫలితం = Curl_pollset_సమితి_బహిర్గతం_only(దత్తాంశం, ps, sock);
        CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      లేకపోతే {
        ఫలితం = Curl_pollset_సమితి_లోపల_only(దత్తాంశం, ps, sock);
        CURL_TRC_CF(దత్తాంశం, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    తిరిగి ఫలితం;
  }
  తిరిగి CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
నిర్మాణం curl_slist *Curl_ssl_engines_list(నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(Curl_ssl->engines_list)
    తిరిగి Curl_ssl->engines_list(దత్తాంశం);
  తిరిగి ఖాళీ;
}

స్థిర size_t multissl_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత);

శూన్య Curl_ssl_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత)
{
#నిర్వచితమైతే CURL_WITH_MULTI_SSL
  (శూన్య)multissl_సంస్కరణ(బఫరు, కొలత);
#లేకపోతే
  (శూన్య)Curl_ssl->సంస్కరణ_2(బఫరు, కొలత);
#అయితే_ముగింపు
}

శూన్య Curl_ssl_free_certinfo(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం curl_certinfo *ci = &దత్తాంశం->info.certs;

  ఒకవేళ(ci->అంకె_యొక్క_certs) {
    /* free all individual lists used */
    పూర్ణాంకం i;
    కొరకు(i = 0; i < ci->అంకె_యొక్క_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = ఖాళీ;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->అంకె_యొక్క_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం అంకె)
{
  నిర్మాణం curl_certinfo *ci = &దత్తాంశం->info.certs;
  నిర్మాణం curl_slist **పట్టిక;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(దత్తాంశం);

  /* Allocate the required certificate information structures */
  పట్టిక = curlx_calloc((size_t)అంకె, పరిమాణం(నిర్మాణం curl_slist *));
  ఒకవేళ(!పట్టిక)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ci->అంకె_యొక్క_certs = అంకె;
  ci->certinfo = పట్టిక;

  తిరిగి CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_నెట్టు_certinfo_len(నిర్మాణం Curl_easy *దత్తాంశం,
                                    పూర్ణాంకం certnum,
                                    స్థిరరాశి అక్షరం *label,
                                    స్థిరరాశి అక్షరం *విలువ,
                                    size_t valuelen)
{
  నిర్మాణం curl_certinfo *ci = &దత్తాంశం->info.certs;
  నిర్మాణం curl_slist *nl;
  CURLcode ఫలితం = CURLE_OK;
  నిర్మాణం dynbuf build;

  DEBUGASSERT(certnum < ci->అంకె_యొక్క_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  ఒకవేళ(curlx_dyn_కూడు(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, విలువ, valuelen))
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  ఒకవేళ(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ci->certinfo[certnum] = nl;
  తిరిగి ఫలితం;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(నిర్మాణం Curl_easy *దత్తాంశం,
                         గుర్తులేని అక్షరం *బఫరు, size_t నిడివి)
{
  DEBUGASSERT(నిడివి == పరిమాణం(పూర్ణాంకం));
  ఒకవేళ(Curl_ssl->random)
    తిరిగి Curl_ssl->random(దత్తాంశం, బఫరు, నిడివి);
  లేకపోతే
    తిరిగి CURLE_NOT_BUILT_లోపల;
}

/*
 * Public key pem to der conversion
 */

స్థిర CURLcode pubkey_pem_గమ్యం_der(స్థిరరాశి అక్షరం *pem,
                                  గుర్తులేని అక్షరం **der, size_t *der_len)
{
  స్థిరరాశి అక్షరం *begin_pos, *చివర_pos;
  size_t pem_లెక్క, pem_len;
  CURLcode ఫలితం;
  నిర్మాణం dynbuf pbuf;

  /* if no pem, exit. */
  ఒకవేళ(!pem)
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_కొలత);

  begin_pos = అక్షరశ్రేణి_శోధన(pem, "-----BEGIN PUBLIC KEY-----");
  ఒకవేళ(!begin_pos)
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  pem_లెక్క = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  ఒకవేళ(pem_లెక్క && '\n' != pem[pem_లెక్క - 1])
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_లెక్క += 26;

  /* Invalid if not directly following \n */
  చివర_pos = అక్షరశ్రేణి_శోధన(pem + pem_లెక్క, "\n-----END PUBLIC KEY-----");
  ఒకవేళ(!చివర_pos)
    తిరిగి CURLE_BAD_CONTENT_ENCODING;

  pem_len = చివర_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  అయితే(pem_లెక్క < pem_len) {
    ఒకవేళ('\n' != pem[pem_లెక్క] && '\r' != pem[pem_లెక్క]) {
      ఫలితం = curlx_dyn_addn(&pbuf, &pem[pem_లెక్క], 1);
      ఒకవేళ(ఫలితం)
        తిరిగి ఫలితం;
    }
    ++pem_లెక్క;
  }

  ఒకవేళ(curlx_dyn_len(&pbuf)) {
    ఫలితం = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  లేకపోతే
    ఫలితం = CURLE_BAD_CONTENT_ENCODING;

  తిరిగి ఫలితం;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *pinnedpubkey,
                              స్థిరరాశి గుర్తులేని అక్షరం *pubkey, size_t pubkeylen)
{
  CURLcode ఫలితం = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  ఒకవేళ(!pinnedpubkey)
    తిరిగి CURLE_OK;
  ఒకవేళ(!pubkey || !pubkeylen)
    తిరిగి ఫలితం;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  ఒకవేళ(!అక్షరశ్రేణి_పోలిక(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    అక్షరం *cert_hash = ఖాళీ;
    స్థిరరాశి అక్షరం *pinned_hash, *చివర_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    గుర్తులేని అక్షరం *sha256sumdigest;

    ఒకవేళ(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      తిరిగి ఫలితం;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_నిడివి);
    ఒకవేళ(!sha256sumdigest)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_నిడివి);

    ఒకవేళ(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_నిడివి,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    ఒకవేళ(encode)
      తిరిగి encode;

    infof(దత్తాంశం, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    అయితే(pinned_hash &&
          !అక్షరశ్రేణి_పోలిక(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      చివర_pos = strchr(pinned_hash, ';');
      pinned_hash_len = చివర_pos ?
                        (size_t)(చివర_pos - pinned_hash) : అక్షరశ్రేణి_పొడవు(pinned_hash);

      /* compare base64 sha256 digests */
      ఒకవేళ(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(దత్తాంశం, "public key hash matches pinned value"));
        ఫలితం = CURLE_OK;
        ఆపు;
      }

      DEBUGF(infof(దత్తాంశం, "public key hash does not match 'sha256//%.*s'",
                   (పూర్ణాంకం)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = చివర_pos ? (చివర_pos + 1) : ఖాళీ;
    }
    curlx_safefree(cert_hash);
  }
  లేకపోతే {
    దీర్ఘ filesize;
    size_t కొలత, pem_len;
    CURLcode pem_read;
    నిర్మాణం dynbuf buf;
    గుర్తులేని అక్షరం *pem_ptr = ఖాళీ;
    size_t ఎడమ;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    ఒకవేళ(!fp)
      తిరిగి ఫలితం;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_కొలత);

    /* Determine the file's size */
    ఒకవేళ(fseek(fp, 0, SEEK_END))
      వెళ్ళు చివర;
    filesize = ftell(fp);
    ఒకవేళ(fseek(fp, 0, SEEK_SET))
      వెళ్ళు చివర;
    ఒకవేళ(filesize < 0 || filesize > MAX_PINNED_PUBKEY_కొలత)
      వెళ్ళు చివర;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    కొలత = curlx_sotouz((curl_off_t)filesize);
    ఒకవేళ(pubkeylen > కొలత)
      వెళ్ళు చివర;

    /*
     * Read the file into the dynbuf
     */
    ఎడమ = కొలత;
    చేయి {
      అక్షరం బఫరు[1024];
      size_t want = ఎడమ > పరిమాణం(బఫరు) ? పరిమాణం(బఫరు) : ఎడమ;
      ఒకవేళ(fread(బఫరు, 1, want, fp) != want)
        వెళ్ళు చివర;
      ఒకవేళ(curlx_dyn_addn(&buf, బఫరు, want))
        వెళ్ళు చివర;
      ఎడమ -= want;
    } అయితే(ఎడమ);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    ఒకవేళ(pubkeylen == కొలత) {
      ఒకవేళ(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        ఫలితం = CURLE_OK;
      వెళ్ళు చివర;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_గమ్యం_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    ఒకవేళ(pem_read)
      వెళ్ళు చివర;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    ఒకవేళ(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      ఫలితం = CURLE_OK;
చివర:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  తిరిగి ఫలితం;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_స్థితి_request(శూన్య)
{
  ఒకవేళ(Curl_ssl->cert_స్థితి_request)
    తిరిగి Curl_ssl->cert_స్థితి_request();
  తిరిగి FALSE;
}

స్థిర పూర్ణాంకం multissl_init(శూన్య)
{
  ఒకవేళ(multissl_అమరిక(ఖాళీ))
    తిరిగి 1;
  ఒకవేళ(Curl_ssl->init)
    తిరిగి Curl_ssl->init();
  తిరిగి 1;
}

స్థిర CURLcode multissl_random(నిర్మాణం Curl_easy *దత్తాంశం,
                                గుర్తులేని అక్షరం *entropy, size_t నిడివి)
{
  ఒకవేళ(multissl_అమరిక(ఖాళీ))
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_ssl->random(దత్తాంశం, entropy, నిడివి);
}

స్థిర CURLcode multissl_connect(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  ఒకవేళ(multissl_అమరిక(ఖాళీ))
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_ssl->do_connect(cf, దత్తాంశం, పూర్తి);
}

స్థిర CURLcode multissl_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం easy_pollset *ps)
{
  ఒకవేళ(multissl_అమరిక(ఖాళీ))
    తిరిగి CURLE_OK;
  తిరిగి Curl_ssl->adjust_pollset(cf, దత్తాంశం, ps);
}

స్థిర శూన్య *multissl_get_internals(నిర్మాణం ssl_connect_దత్తాంశం *connssl,
                                    CURLINFO info)
{
  ఒకవేళ(multissl_అమరిక(ఖాళీ))
    తిరిగి ఖాళీ;
  తిరిగి Curl_ssl->get_internals(connssl, info);
}

స్థిర శూన్య multissl_close(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(multissl_అమరిక(ఖాళీ))
    తిరిగి;
  Curl_ssl->close(cf, దత్తాంశం);
}

స్థిర CURLcode multissl_recv_plain(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    అక్షరం *buf, size_t len, size_t *pnread)
{
  ఒకవేళ(multissl_అమరిక(ఖాళీ))
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_ssl->recv_plain(cf, దత్తాంశం, buf, len, pnread);
}

స్థిర CURLcode multissl_send_plain(నిర్మాణం Curl_cfilter *cf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    స్థిరరాశి శూన్య *mem, size_t len,
                                    size_t *pnwritten)
{
  ఒకవేళ(multissl_అమరిక(ఖాళీ))
    తిరిగి CURLE_FAILED_INIT;
  తిరిగి Curl_ssl->send_plain(cf, దత్తాంశం, mem, len, pnwritten);
}

స్థిర స్థిరరాశి నిర్మాణం Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  ఖాళీ,                              /* cleanup */
  multissl_సంస్కరణ,                  /*సంస్కరణ*/
  ఖాళీ,                              /* shutdown */
  ఖాళీ,                              /* data_pending */
  multissl_random,                   /* random */
  ఖాళీ,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  ఖాళీ,                              /* close_all */
  ఖాళీ,                              /* engines_list */
  ఖాళీ,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  ఖాళీ,                              /* get_channel_binding */
};

స్థిరరాశి నిర్మాణం Curl_ssl *Curl_ssl =
#నిర్వచితమైతే CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#లేదావేళ defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#లేదావేళ defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#లేదావేళ defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#లేదావేళ defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#లేదావేళ defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#లేదావేళ defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#లేకపోతే
#దోషం "Missing struct Curl_ssl for selected SSL backend"
#అయితే_ముగింపు

స్థిర స్థిరరాశి నిర్మాణం Curl_ssl *అందుబాటు_backends[] = {
#నిర్వచితమైతే USE_WOLFSSL
  &Curl_ssl_wolfssl,
#అయితే_ముగింపు
#నిర్వచితమైతే USE_GNUTLS
  &Curl_ssl_gnutls,
#అయితే_ముగింపు
#నిర్వచితమైతే USE_MBEDTLS
  &Curl_ssl_mbedtls,
#అయితే_ముగింపు
#నిర్వచితమైతే USE_OPENSSL
  &Curl_ssl_openssl,
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SCHANNEL
  &Curl_ssl_schannel,
#అయితే_ముగింపు
#నిర్వచితమైతే USE_RUSTLS
  &Curl_ssl_rustls,
#అయితే_ముగింపు
  ఖాళీ
};

/* Global cleanup */
శూన్య Curl_ssl_cleanup(శూన్య)
{
  ఒకవేళ(init_ssl) {
    /* only cleanup if we did a previous init */
    ఒకవేళ(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#నిర్వచితమైతే CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#అయితే_ముగింపు
    init_ssl = FALSE;
  }
}

స్థిర size_t multissl_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత)
{
  స్థిర స్థిరరాశి నిర్మాణం Curl_ssl *selected;
  స్థిర అక్షరం backends[200];
  స్థిర size_t backends_len;
  స్థిరరాశి నిర్మాణం Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? అందుబాటు_backends[0] : Curl_ssl;

  ఒకవేళ(current != selected) {
    అక్షరం *p = backends;
    స్థిరరాశి అక్షరం *చివర = backends + పరిమాణం(backends);
    పూర్ణాంకం i;

    selected = current;

    backends[0] = '\0';

    కొరకు(i = 0; అందుబాటు_backends[i]; ++i) {
      అక్షరం vb[200];
      bool paren = (selected != అందుబాటు_backends[i]);

      ఒకవేళ(అందుబాటు_backends[i]->సంస్కరణ_2(vb, పరిమాణం(vb))) {
        p += curl_msnprintf(p, చివర - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  ఒకవేళ(కొలత) {
    curlx_strcopy(బఫరు, కొలత, backends, backends_len);
  }
  తిరిగి 0;
}

స్థిర పూర్ణాంకం multissl_అమరిక(స్థిరరాశి నిర్మాణం Curl_ssl *backend)
{
  పూర్ణాంకం i;
  అక్షరం *env;

  ఒకవేళ(Curl_ssl != &Curl_ssl_multi)
    తిరిగి 1;

  ఒకవేళ(backend) {
    Curl_ssl = backend;
    తిరిగి 0;
  }

  ఒకవేళ(!అందుబాటు_backends[0])
    తిరిగి 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  ఒకవేళ(env) {
    కొరకు(i = 0; అందుబాటు_backends[i]; i++) {
      ఒకవేళ(curl_strequal(env, అందుబాటు_backends[i]->info.name)) {
        Curl_ssl = అందుబాటు_backends[i];
        curlx_free(env);
        తిరిగి 0;
      }
    }
  }

#నిర్వచితమైతే CURL_DEFAULT_SSL_BACKEND
  కొరకు(i = 0; అందుబాటు_backends[i]; i++) {
    ఒకవేళ(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     అందుబాటు_backends[i]->info.name)) {
      Curl_ssl = అందుబాటు_backends[i];
      curlx_free(env);
      తిరిగి 0;
    }
  }
#అయితే_ముగింపు

  /* Fall back to first available backend */
  Curl_ssl = అందుబాటు_backends[0];
  curlx_free(env);
  తిరిగి 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, స్థిరరాశి అక్షరం *name,
                                   స్థిరరాశి curl_ssl_backend ***avail)
{
  పూర్ణాంకం i;

  ఒకవేళ(avail)
    *avail = (స్థిరరాశి curl_ssl_backend **)&అందుబాటు_backends;

  ఒకవేళ(Curl_ssl != &Curl_ssl_multi)
    తిరిగి id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#నిర్వచితమైతే CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#లేకపోతే
           CURLSSLSET_UNKNOWN_BACKEND;
#అయితే_ముగింపు

  కొరకు(i = 0; అందుబాటు_backends[i]; i++) {
    ఒకవేళ(అందుబాటు_backends[i]->info.id == id ||
       (name && curl_strequal(అందుబాటు_backends[i]->info.name, name))) {
      multissl_అమరిక(అందుబాటు_backends[i]);
      తిరిగి CURLSSLSET_OK;
    }
  }

  తిరిగి CURLSSLSET_UNKNOWN_BACKEND;
}

#లేకపోతే /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, స్థిరరాశి అక్షరం *name,
                                   స్థిరరాశి curl_ssl_backend ***avail)
{
  (శూన్య)id;
  (శూన్య)name;
  (శూన్య)avail;
  తిరిగి CURLSSLSET_NO_BACKENDS;
}

#అయితే_ముగింపు /* !USE_SSL */

#నిర్వచితమైతే USE_SSL

శూన్య Curl_ssl_peer_cleanup(నిర్మాణం ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_కీలకం);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

స్థిర ssl_peer_type get_peer_type(స్థిరరాశి అక్షరం *hostname)
{
  ఒకవేళ(hostname && hostname[0]) {
#నిర్వచితమైతే USE_IPV6
    నిర్మాణం in6_addr addr;
#లేకపోతే
    నిర్మాణం in_addr addr;
#అయితే_ముగింపు
    ఒకవేళ(curlx_inet_pton(AF_INET, hostname, &addr))
      తిరిగి CURL_SSL_PEER_IPV4;
#నిర్వచితమైతే USE_IPV6
    లేకపోతే ఒకవేళ(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      తిరిగి CURL_SSL_PEER_IPV6;
    }
#అయితే_ముగింపు
  }
  తిరిగి CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(నిర్మాణం ssl_peer *ssl_peer,
                            నిర్మాణం Curl_peer *origin,
                            నిర్మాణం Curl_peer *peer,
                            నిర్మాణం ssl_filter_config *sslc,
                            స్థిరరాశి అక్షరం *tls_id,
                            uint8_t transport)
{
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  /* We expect a clean struct, e.g. called only ONCE */
  ఒకవేళ(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    తిరిగి CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  ఒకవేళ(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = అక్షరశ్రేణి_పొడవు(origin->hostname);
    ఒకవేళ(len && (origin->hostname[len - 1] == '.'))
      len--;
    ఒకవేళ(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      ఒకవేళ(!ssl_peer->sni)
        వెళ్ళు బహిర్గతం;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  ఫలితం = Curl_ssl_peer_కీలకం_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_కీలకం);

బహిర్గతం:
  ఒకవేళ(ఫలితం)
    Curl_ssl_peer_cleanup(ssl_peer);
  తిరిగి ఫలితం;
}

స్థిర శూన్య ssl_cf_destroy(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  ఒకవేళ(connssl) {
    connssl->ssl_impl->close(cf, దత్తాంశం);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = ఖాళీ;
  }
  cf->connected = FALSE;
}

స్థిర CURLcode ssl_cf_connect(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం;

  ఒకవేళ(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(!cf->తదుపరి || !connssl->peer.origin) {
    *పూర్తి = FALSE;
    తిరిగి CURLE_FAILED_INIT;
  }

  ఒకవేళ(!cf->తదుపరి->connected) {
    ఫలితం = cf->తదుపరి->cft->do_connect(cf->తదుపరి, దత్తాంశం, పూర్తి);
    ఒకవేళ(ఫలితం || !*పూర్తి)
      తిరిగి ఫలితం;
  }

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  CURL_TRC_CF(దత్తాంశం, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *పూర్తి = FALSE;

  ఒకవేళ(!connssl->prefs_checked) {
    ఒకవేళ(!ssl_prefs_check(cf, దత్తాంశం)) {
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    connssl->prefs_checked = TRUE;
  }

  ఫలితం = connssl->ssl_impl->do_connect(cf, దత్తాంశం, పూర్తి);

  ఒకవేళ(!ఫలితం && *పూర్తి) {
    cf->connected = TRUE;
    ఒకవేళ(connssl->state == ssl_connection_complete) {
      connssl->handshake_పూర్తి = *Curl_pgrs_now(దత్తాంశం);
    }
    ఒకవేళ(Curl_tls_keylog_enabled()) {
      infof(దత్తాంశం, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#నిర్వచితమైతే LIBRESSL_సంస్కరణ_సంఖ్య
      infof(దత్తాంశం, "Note LibreSSL only supports SSLKEYLOGFILE for TLS <= 1.2");
#అయితే_ముగింపు
    }
    /* Connection can be deferred when sending early data */
    DEBUGASSERT(connssl->state == ssl_connection_complete ||
                connssl->state == ssl_connection_deferred);
    DEBUGASSERT(connssl->state != ssl_connection_deferred ||
                connssl->earlydata_state > ssl_earlydata_none);
  }
బహిర్గతం:
  CURL_TRC_CF(దత్తాంశం, cf, "cf_connect() -> %d, done=%d", (పూర్ణాంకం)ఫలితం, *పూర్తి);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssl_cf_సమితి_earlydata(నిర్మాణం Curl_cfilter *cf,
                                     నిర్మాణం Curl_easy *దత్తాంశం,
                                     స్థిరరాశి శూన్య *buf, size_t blen)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_ఖాళీలు(&connssl->earlydata));
  ఒకవేళ(blen) {
    ఒకవేళ(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    ఫలితం = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(దత్తాంశం, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssl_cf_connect_deferred(నిర్మాణం Curl_cfilter *cf,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        స్థిరరాశి శూన్య *buf, size_t blen,
                                        bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *పూర్తి = FALSE;
  ఒకవేళ(connssl->earlydata_state == ssl_earlydata_await) {
    ఫలితం = ssl_cf_సమితి_earlydata(cf, దత్తాంశం, buf, blen);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  ఫలితం = ssl_cf_connect(cf, దత్తాంశం, పూర్తి);

  ఒకవేళ(!ఫలితం && *పూర్తి) {
    ఒకవేళ(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_APPCONNECT, connssl->handshake_పూర్తి);
      connssl->stats_reported = TRUE;
    }
    ఎంపిక(connssl->earlydata_state) {
    స్థితి ssl_earlydata_none:
      ఆపు;
    స్థితి ssl_earlydata_accepted:
      ఒకవేళ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_దత్తాంశం(దత్తాంశం, (curl_off_t)connssl->earlydata_skip);
      infof(దత్తాంశం, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      ఆపు;
    స్థితి ssl_earlydata_rejected:
      ఒకవేళ(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_దత్తాంశం(దత్తాంశం, -(curl_off_t)connssl->earlydata_skip);
      infof(దత్తాంశం, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      ఆపు;
    అప్రమేయ:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(ఖాళీ);
      ఆపు;
    }
  }
  తిరిగి ఫలితం;
}

స్థిర bool ssl_cf_దత్తాంశం_pending(నిర్మాణం Curl_cfilter *cf,
                                స్థిరరాశి నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  bool pending;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  ఒకవేళ(connssl->ssl_impl->దత్తాంశం_pending &&
     connssl->ssl_impl->దత్తాంశం_pending(cf, దత్తాంశం))
    pending = TRUE;
  లేకపోతే
    pending = cf->తదుపరి->cft->has_దత్తాంశం_pending(cf->తదుపరి, దత్తాంశం);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి pending;
}

స్థిర CURLcode ssl_cf_send(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం = CURLE_OK;

  (శూన్య)eos;
  *pnwritten = 0;
  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);

  ఒకవేళ(connssl->state == ssl_connection_deferred) {
    bool పూర్తి = FALSE;
    ఫలితం = ssl_cf_connect_deferred(cf, దత్తాంశం, buf, blen, &పూర్తి);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    లేకపోతే ఒకవేళ(!పూర్తి) {
      ఫలితం = CURLE_AGAIN;
      వెళ్ళు బహిర్గతం;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ఒకవేళ(connssl->earlydata_skip) {
    ఒకవేళ(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      ఫలితం = CURLE_OK;
      *pnwritten = blen;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  ఒకవేళ(blen > 0) {
    size_t nwritten;
    ఫలితం = connssl->ssl_impl->send_plain(cf, దత్తాంశం, buf, blen, &nwritten);
    ఒకవేళ(!ఫలితం)
      *pnwritten += nwritten;
  }

బహిర్గతం:
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssl_cf_recv(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం, అక్షరం *buf, size_t len,
                            size_t *pnread)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం = CURLE_OK;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  *pnread = 0;
  ఒకవేళ(connssl->state == ssl_connection_deferred) {
    bool పూర్తి = FALSE;
    ఫలితం = ssl_cf_connect_deferred(cf, దత్తాంశం, ఖాళీ, 0, &పూర్తి);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    లేకపోతే ఒకవేళ(!పూర్తి) {
      ఫలితం = CURLE_AGAIN;
      వెళ్ళు బహిర్గతం;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ఫలితం = connssl->ssl_impl->recv_plain(cf, దత్తాంశం, buf, len, pnread);

బహిర్గతం:
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssl_cf_shutdown(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  *పూర్తి = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  ఒకవేళ(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_కిందికి) {
    నిర్మాణం cf_call_దత్తాంశం save;

    CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
    ఫలితం = connssl->ssl_impl->shut_కిందికి(cf, దత్తాంశం, TRUE, పూర్తి);
    CURL_TRC_CF(దత్తాంశం, cf, "cf_shutdown -> %d, done=%d", (పూర్ణాంకం)ఫలితం, *పూర్తి);
    CF_దత్తాంశం_RESTORE(cf, save);
    cf->shutdown = (ఫలితం || *పూర్తి);
  }
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssl_cf_adjust_pollset(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం easy_pollset *ps)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం;

  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
  ఫలితం = connssl->ssl_impl->adjust_pollset(cf, దత్తాంశం, ps);
  CF_దత్తాంశం_RESTORE(cf, save);
  తిరిగి ఫలితం;
}

స్థిర CURLcode ssl_cf_query(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             పూర్ణాంకం query, పూర్ణాంకం *pres1, శూన్య *pres2)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;

  ఎంపిక(query) {
  స్థితి CF_QUERY_SSL_INFO:
  స్థితి CF_QUERY_SSL_CTX_INFO:
    ఒకవేళ(!Curl_ssl_cf_is_proxy(cf)) {
      నిర్మాణం curl_tlssessioninfo *info = pres2;
      నిర్మాణం cf_call_దత్తాంశం save;
      CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_దత్తాంశం_RESTORE(cf, save);
      తిరిగి CURLE_OK;
    }
    ఆపు;
  స్థితి CF_QUERY_ALPN_NEGOTIATED: {
    స్థిరరాశి అక్షరం **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(దత్తాంశం, cf, "query ALPN: returning '%s'", *palpn);
    తిరిగి CURLE_OK;
  }
  అప్రమేయ:
    ఆపు;
  }
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->query(cf->తదుపరి, దత్తాంశం, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

స్థిర CURLcode ssl_cf_cntrl(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             పూర్ణాంకం event, పూర్ణాంకం arg1, శూన్య *arg2)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;

  (శూన్య)arg1;
  (శూన్య)arg2;
  (శూన్య)దత్తాంశం;
  ఎంపిక(event) {
  స్థితి CF_CTRL_CONN_INFO_UPDATE:
    ఒకవేళ(connssl->negotiated.alpn && !cf->sockindex) {
      ఒకవేళ(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_చూసిన = 11;
      లేకపోతే ఒకవేళ(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_చూసిన = 20;
      లేకపోతే ఒకవేళ(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_చూసిన = 30;
    }
    ఆపు;
  స్థితి CF_CTRL_REPORT_STATS:
    ఒకవేళ(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_పూర్తి.tv_sec || connssl->handshake_పూర్తి.tv_usec)) {
      Curl_pgrs_సమయం_Was(దత్తాంశం, TIMER_APPCONNECT, connssl->handshake_పూర్తి);
      connssl->stats_reported = TRUE;
    }
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర bool cf_ssl_is_alive(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం,
                            bool *ఆదానం_pending)
{
  /*
   * This function tries to determine connection status.
   */
  తిరిగి cf->తదుపరి ?
    cf->తదుపరి->cft->is_alive(cf->తదుపరి, దత్తాంశం, ఆదానం_pending) :
    FALSE; /* pessimistic in absence of data */
}

నిర్మాణం Curl_cftype Curl_cft_ssl = {
  "SSL",
  CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_దత్తాంశం_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

నిర్మాణం Curl_cftype Curl_cft_ssl_proxy = {
  "SSL-PROXY",
  CF_TYPE_SSL | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_దత్తాంశం_pending,
  ssl_cf_send,
  ssl_cf_recv,
  Curl_cf_def_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */

స్థిర CURLcode cf_ssl_create(నిర్మాణం Curl_cfilter **pcf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం connectdata *conn)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం ssl_connect_దత్తాంశం *ctx;
  CURLcode ఫలితం;

  DEBUGASSERT(దత్తాంశం->conn);

#నిర్వచితమైతే CURL_DISABLE_HTTP
  (శూన్య)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->బిట్లు.tls_enable_alpn);
  ctx = cf_ctx_new(దత్తాంశం, ఖాళీ);
#లేకపోతే
  ctx = cf_ctx_new(దత్తాంశం, alpn_get_spec(దత్తాంశం->state.http_neg.wanted,
                                       దత్తాంశం->state.http_neg.preferred,
                                       (bool)దత్తాంశం->state.http_neg.only_10,
                                       (bool)conn->బిట్లు.tls_enable_alpn));
#అయితే_ముగింపు
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

బహిర్గతం:
  ఒకవేళ(ఫలితం)
    cf_ctx_free(ctx);
  *pcf = ఫలితం ? ఖాళీ : cf;
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_ssl_peer_init(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_peer *origin,
                                 నిర్మాణం Curl_peer *peer,
                                 నిర్మాణం ssl_filter_config *sslc)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  అక్షరం tls_id[80];
  connssl->ssl_impl->సంస్కరణ_2(tls_id, పరిమాణం(tls_id) - 1);
  తిరిగి Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_peer *origin,
                              నిర్మాణం connectdata *conn,
                              int8_t sockindex)
{
  నిర్మాణం Curl_cfilter *cf;
  నిర్మాణం Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->ద్వారా_peer2 : conn->ద్వారా_peer;
  CURLcode ఫలితం;

  ఫలితం = cf_ssl_create(&cf, దత్తాంశం, conn);
  ఒకవేళ(!ఫలితం)
    ఫలితం = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  ఒకవేళ(!ఫలితం)
    Curl_conn_cf_కూడు(దత్తాంశం, conn, sockindex, cf);
  లేకపోతే ఒకవేళ(cf)
    Curl_conn_cf_discard_chain(&cf, దత్తాంశం);
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_ssl_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం Curl_peer *origin,
                                  నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;

  ఫలితం = cf_ssl_create(&cf, దత్తాంశం, cf_at->conn);
  ఒకవేళ(!ఫలితం)
    ఫలితం = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  ఒకవేళ(!ఫలితం) {
    Curl_conn_cf_చొప్పించు_after(cf_at, cf);
#ఒకవేళ defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    ఒకవేళ((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      ఫలితం = Curl_conn_dns_కూడు_https_resolve(దత్తాంశం, cf->conn, cf->sockindex,
                                               origin);
    }
#అయితే_ముగింపు /* USE_HTTPSRR && USE_ECH */
  }
  లేకపోతే ఒకవేళ(cf)
    Curl_conn_cf_discard_chain(&cf, దత్తాంశం);
  తిరిగి ఫలితం;
}

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY

స్థిర CURLcode cf_ssl_proxy_create(నిర్మాణం Curl_cfilter **pcf,
                                    నిర్మాణం Curl_easy *దత్తాంశం,
                                    నిర్మాణం connectdata *conn)
{
  నిర్మాణం Curl_cfilter *cf = ఖాళీ;
  నిర్మాణం ssl_connect_దత్తాంశం *ctx;
  CURLcode ఫలితం;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)దత్తాంశం->సమితి.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (శూన్య)conn;
#నిర్వచితమైతే USE_HTTP2
  ఒకవేళ(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#అయితే_ముగింపు

  ctx = cf_ctx_new(దత్తాంశం, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  ఒకవేళ(!ctx) {
    ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

బహిర్గతం:
  ఒకవేళ(ఫలితం)
    cf_ctx_free(ctx);
  *pcf = ఫలితం ? ఖాళీ : cf;
  తిరిగి ఫలితం;
}

CURLcode Curl_cf_ssl_proxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం Curl_peer *peer)
{
  నిర్మాణం Curl_cfilter *cf;
  CURLcode ఫలితం;

  ఫలితం = cf_ssl_proxy_create(&cf, దత్తాంశం, cf_at->conn);
  ఒకవేళ(!ఫలితం)
    ఫలితం = cf_ssl_peer_init(cf, peer, ఖాళీ, &cf_at->conn->proxy_ssl_config);
  ఒకవేళ(!ఫలితం)
    Curl_conn_cf_చొప్పించు_after(cf_at, cf);
  లేకపోతే ఒకవేళ(cf)
    Curl_conn_cf_discard_chain(&cf, దత్తాంశం);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని పూర్ణాంకం ssl_option)
{
  (శూన్య)దత్తాంశం;
  తిరిగి (Curl_ssl->supports & ssl_option);
}

#నిర్వచితమైతే USE_ECH
bool Curl_ssl_ech_enabled(నిర్మాణం Curl_cfilter *cf)
{
  ఒకవేళ(Curl_ssl->supports & SSLSUPP_ECH) {
    నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    తిరిగి !!conn_config->ech;
  }
  తిరిగి FALSE;
}

bool Curl_ssl_need_httpsrr(నిర్మాణం Curl_cfilter *cf)
{
  ఒకవేళ(Curl_ssl->supports & SSLSUPP_ECH) {
    నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ఒకవేళ(!conn_config->ech)
      తిరిగి FALSE;
    ఒకవేళ((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      తిరిగి FALSE;
    తిరిగి TRUE;
  }
  తిరిగి FALSE;
}
#అయితే_ముగింపు

స్థిర CURLcode vtls_shutdown_blocking(నిర్మాణం Curl_cfilter *cf,
                                       నిర్మాణం Curl_easy *దత్తాంశం,
                                       bool send_shutdown, bool *పూర్తి)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  నిర్మాణం cf_call_దత్తాంశం save;
  CURLcode ఫలితం = CURLE_OK;
  timediff_t timeout_ms;
  పూర్ణాంకం what, loop = 10;

  ఒకవేళ(cf->shutdown) {
    *పూర్తి = TRUE;
    తిరిగి CURLE_OK;
  }
  CF_దత్తాంశం_SAVE(save, cf, దత్తాంశం);

  *పూర్తి = FALSE;
  అయితే(!ఫలితం && !*పూర్తి && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(దత్తాంశం, cf->conn, cf->sockindex);

    ఒకవేళ(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(దత్తాంశం, "SSL shutdown timeout");
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      వెళ్ళు బహిర్గతం;
    }

    ఫలితం = connssl->ssl_impl->shut_కిందికి(cf, దత్తాంశం, send_shutdown, పూర్తి);
    ఒకవేళ(ఫలితం || *పూర్తి)
      వెళ్ళు బహిర్గతం;

    ఒకవేళ(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, దత్తాంశం, timeout_ms);
      ఒకవేళ(what < 0) {
        /* fatal error */
        failf(దత్తాంశం, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        ఫలితం = CURLE_RECV_ERROR;
        వెళ్ళు బహిర్గతం;
      }
      లేకపోతే ఒకవేళ(what == 0) {
        /* timeout */
        failf(దత్తాంశం, "SSL shutdown timeout");
        ఫలితం = CURLE_OPERATION_TIMEDOUT;
        వెళ్ళు బహిర్గతం;
      }
      /* socket is readable or writable */
    }
  }
బహిర్గతం:
  CF_దత్తాంశం_RESTORE(cf, save);
  cf->shutdown = (ఫలితం || *పూర్తి);
  తిరిగి ఫలితం;
}

CURLcode Curl_ssl_cfilter_remove(నిర్మాణం Curl_easy *దత్తాంశం,
                                 int8_t sockindex, bool send_shutdown)
{
  నిర్మాణం Curl_cfilter *cf, *తల;
  CURLcode ఫలితం = CURLE_OK;

  తల = దత్తాంశం->conn ? దత్తాంశం->conn->cfilter[sockindex] : ఖాళీ;
  కొరకు(cf = తల; cf; cf = cf->తదుపరి) {
    ఒకవేళ(cf->cft == &Curl_cft_ssl) {
      bool పూర్తి;
      CURL_TRC_CF(దత్తాంశం, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_ప్రారంభం_timer(దత్తాంశం, sockindex, 0);
      ఫలితం = vtls_shutdown_blocking(cf, దత్తాంశం, send_shutdown, &పూర్తి);
      Curl_cshutdn_clear_timer(దత్తాంశం, sockindex);
      ఒకవేళ(!ఫలితం && !పూర్తి) /* blocking failed? */
        ఫలితం = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, దత్తాంశం);
      CURL_TRC_CF(దత్తాంశం, cf, "shutdown and remove SSL, done -> %d",
                  (పూర్ణాంకం)ఫలితం);
      ఆపు;
    }
  }
  తిరిగి ఫలితం;
}

bool Curl_ssl_cf_is_proxy(నిర్మాణం Curl_cfilter *cf)
{
  తిరిగి (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

నిర్మాణం ssl_easy_config *
Curl_ssl_cf_get_easy_config(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం)
{
#నిర్వచితమైతే CURL_DISABLE_PROXY
  (శూన్య)cf;
  తిరిగి &దత్తాంశం->సమితి.ssl;
#లేకపోతే
  తిరిగి Curl_ssl_cf_is_proxy(cf) ? &దత్తాంశం->సమితి.proxy_ssl : &దత్తాంశం->సమితి.ssl;
#అయితే_ముగింపు
}

నిర్మాణం ssl_filter_config *
Curl_ssl_cf_get_filter_config(నిర్మాణం Curl_cfilter *cf)
{
#నిర్వచితమైతే CURL_DISABLE_PROXY
  తిరిగి &cf->conn->ssl_config;
#లేకపోతే
  తిరిగి Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#అయితే_ముగింపు
}

CURLcode Curl_alpn_గమ్యం_proto_buf(నిర్మాణం alpn_proto_buf *buf,
                                స్థిరరాశి నిర్మాణం alpn_spec *spec)
{
  size_t i, len;
  పూర్ణాంకం off = 0;
  గుర్తులేని అక్షరం blen;

  memset(buf, 0, పరిమాణం(*buf));
  కొరకు(i = 0; spec && i < spec->లెక్క; ++i) {
    len = అక్షరశ్రేణి_పొడవు(spec->entries[i]);
    ఒకవేళ(len >= ALPN_NAME_MAX)
      తిరిగి CURLE_FAILED_INIT;
    blen = (గుర్తులేని అక్షరం)len;
    ఒకవేళ(off + blen + 1 >= (పూర్ణాంకం)పరిమాణం(buf->దత్తాంశం))
      తిరిగి CURLE_FAILED_INIT;
    buf->దత్తాంశం[off++] = blen;
    జ్ఞాపకం_నకలు(buf->దత్తాంశం + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  తిరిగి CURLE_OK;
}

CURLcode Curl_alpn_గమ్యం_proto_str(నిర్మాణం alpn_proto_buf *buf,
                                స్థిరరాశి నిర్మాణం alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, పరిమాణం(*buf));
  కొరకు(i = 0; spec && i < spec->లెక్క; ++i) {
    len = అక్షరశ్రేణి_పొడవు(spec->entries[i]);
    ఒకవేళ(len >= ALPN_NAME_MAX)
      తిరిగి CURLE_FAILED_INIT;
    ఒకవేళ(off + len + 2 >= పరిమాణం(buf->దత్తాంశం))
      తిరిగి CURLE_FAILED_INIT;
    ఒకవేళ(off)
      buf->దత్తాంశం[off++] = ',';
    జ్ఞాపకం_నకలు(buf->దత్తాంశం + off, spec->entries[i], len);
    off += len;
  }
  buf->దత్తాంశం[off] = '\0';
  buf->len = (పూర్ణాంకం)off;
  తిరిగి CURLE_OK;
}

bool Curl_alpn_కలిగియుంది_proto(స్థిరరాశి నిర్మాణం alpn_spec *spec,
                              స్థిరరాశి అక్షరం *proto)
{
  size_t i, plen = proto ? అక్షరశ్రేణి_పొడవు(proto) : 0;
  కొరకు(i = 0; spec && plen && i < spec->లెక్క; ++i) {
    size_t slen = అక్షరశ్రేణి_పొడవు(spec->entries[i]);
    ఒకవేళ((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      తిరిగి TRUE;
  }
  తిరిగి FALSE;
}

శూన్య Curl_alpn_restrict_గమ్యం(నిర్మాణం alpn_spec *spec, స్థిరరాశి అక్షరం *proto)
{
  size_t plen = అక్షరశ్రేణి_పొడవు(proto);
  DEBUGASSERT(plen < పరిమాణం(spec->entries[0]));
  ఒకవేళ(plen < పరిమాణం(spec->entries[0])) {
    జ్ఞాపకం_నకలు(spec->entries[0], proto, plen + 1);
    spec->లెక్క = 1;
  }
}

శూన్య Curl_alpn_copy(నిర్మాణం alpn_spec *dest, స్థిరరాశి నిర్మాణం alpn_spec *src)
{
  ఒకవేళ(src)
    జ్ఞాపకం_నకలు(dest, src, పరిమాణం(*dest));
  లేకపోతే
    memset(dest, 0, పరిమాణం(*dest));
}

CURLcode Curl_alpn_సమితి_negotiated(నిర్మాణం Curl_cfilter *cf,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం ssl_connect_దత్తాంశం *connssl,
                                  స్థిరరాశి గుర్తులేని అక్షరం *proto,
                                  size_t proto_len)
{
  CURLcode ఫలితం = CURLE_OK;
  (శూన్య)cf;

  ఒకవేళ(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    ఒకవేళ(!proto_len) {
      failf(దత్తాంశం, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ((అక్షరశ్రేణి_పొడవు(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(దత్తాంశం, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (పూర్ణాంకం)proto_len, proto);
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(దత్తాంశం, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    వెళ్ళు బహిర్గతం;
  }

  ఒకవేళ(proto && proto_len) {
    ఒకవేళ(memchr(proto, '\0', proto_len)) {
      failf(దత్తాంశం, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      ఫలితం = CURLE_SSL_CONNECT_ERROR;
      వెళ్ళు బహిర్గతం;
    }
    connssl->negotiated.alpn = curlx_memdup0((స్థిరరాశి అక్షరం *)proto, proto_len);
    ఒకవేళ(!connssl->negotiated.alpn)
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  ఒకవేళ(proto && proto_len) {
    ఒకవేళ(connssl->state == ssl_connection_deferred)
      infof(దత్తాంశం, VTLS_INFOF_ALPN_DEFERRED, (పూర్ణాంకం)proto_len, proto);
    లేకపోతే
      infof(దత్తాంశం, VTLS_INFOF_ALPN_ACCEPTED, (పూర్ణాంకం)proto_len, proto);
  }
  లేకపోతే {
    ఒకవేళ(connssl->state == ssl_connection_deferred)
      infof(దత్తాంశం, VTLS_INFOF_NO_ALPN_DEFERRED);
    లేకపోతే
      infof(దత్తాంశం, VTLS_INFOF_NO_ALPN);
  }

బహిర్గతం:
  తిరిగి ఫలితం;
}

CURLcode Curl_on_session_reuse(నిర్మాణం Curl_cfilter *cf,
                               నిర్మాణం Curl_easy *దత్తాంశం,
                               నిర్మాణం alpn_spec *alpns,
                               నిర్మాణం Curl_ssl_session *scs,
                               bool *do_early_దత్తాంశం, bool early_దత్తాంశం_allowed)
{
  నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf->ctx;
  CURLcode ఫలితం = CURLE_OK;

  *do_early_దత్తాంశం = FALSE;

  ఒకవేళ(!early_దత్తాంశం_allowed) {
    CURL_TRC_CF(దత్తాంశం, cf, "SSL session does not allow earlydata");
  }
  లేకపోతే ఒకవేళ(!Curl_alpn_కలిగియుంది_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(దత్తాంశం, cf, "SSL session has different ALPN, no early data");
  }
  లేకపోతే {
    infof(దత్తాంశం, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    ఫలితం = Curl_alpn_సమితి_negotiated(cf, దత్తాంశం, connssl,
                                      (స్థిరరాశి గుర్తులేని అక్షరం *)scs->alpn,
                                      scs->alpn ? అక్షరశ్రేణి_పొడవు(scs->alpn) : 0);
    *do_early_దత్తాంశం = !ఫలితం;
  }
  తిరిగి ఫలితం;
}

నిర్మాణం Curl_ssl_session *Curl_ssl_get_cf_session(నిర్మాణం Curl_easy *దత్తాంశం,
                                                 స్థిరరాశి నిర్మాణం Curl_cftype *cft,
                                                 int8_t sockindex)
{
  ఒకవేళ(దత్తాంశం->conn &&
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#లేకపోతే
     (cft == &Curl_cft_ssl)) {
#అయితే_ముగింపు
    నిర్మాణం Curl_cfilter *cf1 = దత్తాంశం->conn->cfilter[sockindex];
    కొరకు(; cf1; cf1 = cf1->తదుపరి) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      ఒకవేళ(cf1->cft == cft)
        ఆపు;
    }
    ఒకవేళ(cf1) {
      నిర్మాణం ssl_connect_దత్తాంశం *connssl = cf1->ctx;
      ఒకవేళ(connssl)
        తిరిగి connssl->session;
    }
  }
  తిరిగి ఖాళీ;
}

#అయితే_ముగింపు /* USE_SSL */
