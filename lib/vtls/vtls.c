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

#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"

#অন্তর্ভাব "vtls/vtls.h" /* generic SSL protos etc */
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vtls/keylog.h"

#অন্তর্ভাব "vtls/openssl.h"        /* OpenSSL versions */
#অন্তর্ভাব "vtls/gtls.h"           /* GnuTLS versions */
#অন্তর্ভাব "vtls/wolfssl.h"        /* wolfSSL versions */
#অন্তর্ভাব "vtls/schannel.h"       /* Schannel SSPI version */
#অন্তর্ভাব "vtls/mbedtls.h"        /* mbedTLS versions */
#অন্তর্ভাব "vtls/rustls.h"         /* Rustls versions */

#অন্তর্ভাব "slist.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "curlx/win32-fopen.h"
#অন্তর্ভাব "curl_sha256.h"
#অন্তর্ভাব "curlx/base64.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "setopt.h"
#অন্তর্ভাব "vdns/cf-dns.h"
#অন্তর্ভাব "curlx/strdup.h"
#অন্তর্ভাব "curlx/strcopy.h"

#ifdef USE_APPLE_SECTRUST
#অন্তর্ভাব <Security/Security.h>
#endif

#ifdef USE_SSL
#যদি !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
স্থির ধ্রুবক কাঠামো alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
স্থির ধ্রুবক কাঠামো alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#ifdef USE_HTTP2
স্থির ধ্রুবক কাঠামো alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
স্থির ধ্রুবক কাঠামো alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
স্থির ধ্রুবক কাঠামো alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#endif /* USE_HTTP2 */

স্থির ধ্রুবক কাঠামো alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  যদি(!use_alpn)
    ফেরত ফাঁকা;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  যদি(only_http_10 && (wanted & CURL_HTTP_V1x))
    ফেরত &ALPN_SPEC_H10_H11;
#ifdef USE_HTTP2
  যদি(wanted & CURL_HTTP_V2x) {
    যদি(wanted & CURL_HTTP_V1x)
      ফেরত (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    ফেরত &ALPN_SPEC_H2;
  }
#অন্যথা
  (শূন্য)wanted;
  (শূন্য)preferred;
#endif
  ফেরত &ALPN_SPEC_H11;
}
#endif /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#endif /* USE_SSL */

#ifdef USE_SSL
স্থির পূর্ণসংখ্যা multissl_setup(ধ্রুবক কাঠামো Curl_ssl *backend);
#endif

curl_sslbackend Curl_ssl_backend(শূন্য)
{
#ifdef USE_SSL
  multissl_setup(ফাঁকা);
  ফেরত Curl_ssl->info.id;
#অন্যথা
  ফেরত CURLSSLBACKEND_NONE;
#endif
}

#ifdef USE_SSL

/* "global" init done? */
স্থির bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
পূর্ণসংখ্যা Curl_ssl_init(শূন্য)
{
  /* make sure this is only done once */
  যদি(init_ssl)
    ফেরত 1;
  init_ssl = TRUE; /* never again */

  যদি(Curl_ssl->init)
    ফেরত Curl_ssl->init();
  ফেরত 1;
}

স্থির bool ssl_prefs_check(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  যদি(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(তথ্য, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    ফেরত FALSE;
  }

  নির্বাচন(conn_config->version_max) {
  ক্ষেত্র CURL_SSLVERSION_MAX_NONE:
  ক্ষেত্র CURL_SSLVERSION_MAX_DEFAULT:
    বিরতি;

  পূর্বনির্ধারিত:
    যদি((conn_config->version_max >> 16) < conn_config->version) {
      failf(তথ্য, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      ফেরত FALSE;
    }
  }

  ফেরত TRUE;
}

স্থির কাঠামো ssl_connect_তথ্য *cf_ctx_new(কাঠামো Curl_easy *তথ্য,
                                           ধ্রুবক কাঠামো alpn_spec *alpn)
{
  কাঠামো ssl_connect_তথ্য *ctx;

  (শূন্য)তথ্য;
  ctx = curlx_calloc(1, আকার(*ctx));
  যদি(!ctx)
    ফেরত ফাঁকা;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_তথ্য);
  যদি(!ctx->backend) {
    curlx_free(ctx);
    ফেরত ফাঁকা;
  }
  ফেরত ctx;
}

স্থির শূন্য cf_ctx_free(কাঠামো ssl_connect_তথ্য *ctx)
{
  যদি(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(কাঠামো Curl_easy *তথ্য, int8_t sockindex,
                                      কাঠামো dynbuf *binding)
{
  যদি(Curl_ssl->get_channel_binding)
    ফেরত Curl_ssl->get_channel_binding(তথ্য, sockindex, binding);
  ফেরত CURLE_OK;
}

শূন্য Curl_ssl_close_all(কাঠামো Curl_easy *তথ্য)
{
  যদি(Curl_ssl->close_all)
    Curl_ssl->close_all(তথ্য);
}

CURLcode Curl_ssl_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 কাঠামো easy_pollset *ps)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;

  যদি(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->পরবর্তী, তথ্য);
    CURLcode ফলাফল = CURLE_OK;
    যদি(sock != CURL_SOCKET_BAD) {
      যদি(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        ফলাফল = Curl_pollset_set_out_only(তথ্য, ps, sock);
        CURL_TRC_CF(তথ্য, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      অন্যথা {
        ফলাফল = Curl_pollset_set_in_only(তথ্য, ps, sock);
        CURL_TRC_CF(তথ্য, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    ফেরত ফলাফল;
  }
  ফেরত CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
কাঠামো curl_slist *Curl_ssl_engines_list(কাঠামো Curl_easy *তথ্য)
{
  যদি(Curl_ssl->engines_list)
    ফেরত Curl_ssl->engines_list(তথ্য);
  ফেরত ফাঁকা;
}

স্থির size_t multissl_version(অক্ষর *বাফার, size_t আকার_2);

শূন্য Curl_ssl_version(অক্ষর *বাফার, size_t আকার_2)
{
#ifdef CURL_WITH_MULTI_SSL
  (শূন্য)multissl_version(বাফার, আকার_2);
#অন্যথা
  (শূন্য)Curl_ssl->version(বাফার, আকার_2);
#endif
}

শূন্য Curl_ssl_free_certinfo(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো curl_certinfo *ci = &তথ্য->info.certs;

  যদি(ci->অঙ্ক_of_certs) {
    /* free all individual lists used */
    পূর্ণসংখ্যা i;
    জন্য(i = 0; i < ci->অঙ্ক_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = ফাঁকা;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->অঙ্ক_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা অঙ্ক)
{
  কাঠামো curl_certinfo *ci = &তথ্য->info.certs;
  কাঠামো curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(তথ্য);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((size_t)অঙ্ক, আকার(কাঠামো curl_slist *));
  যদি(!table)
    ফেরত CURLE_OUT_OF_MEMORY;

  ci->অঙ্ক_of_certs = অঙ্ক;
  ci->certinfo = table;

  ফেরত CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_ঠেলো_certinfo_len(কাঠামো Curl_easy *তথ্য,
                                    পূর্ণসংখ্যা certnum,
                                    ধ্রুবক অক্ষর *label,
                                    ধ্রুবক অক্ষর *মান,
                                    size_t valuelen)
{
  কাঠামো curl_certinfo *ci = &তথ্য->info.certs;
  কাঠামো curl_slist *nl;
  CURLcode ফলাফল = CURLE_OK;
  কাঠামো dynbuf build;

  DEBUGASSERT(certnum < ci->অঙ্ক_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  যদি(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, মান, valuelen))
    ফেরত CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  যদি(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    ফলাফল = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  ফেরত ফলাফল;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(কাঠামো Curl_easy *তথ্য,
                         unsigned অক্ষর *বাফার, size_t দৈর্ঘ্য)
{
  DEBUGASSERT(দৈর্ঘ্য == আকার(পূর্ণসংখ্যা));
  যদি(Curl_ssl->random)
    ফেরত Curl_ssl->random(তথ্য, বাফার, দৈর্ঘ্য);
  অন্যথা
    ফেরত CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

স্থির CURLcode pubkey_pem_to_der(ধ্রুবক অক্ষর *pem,
                                  unsigned অক্ষর **der, size_t *der_len)
{
  ধ্রুবক অক্ষর *begin_pos, *end_pos;
  size_t pem_গুনতি, pem_len;
  CURLcode ফলাফল;
  কাঠামো dynbuf pbuf;

  /* if no pem, exit. */
  যদি(!pem)
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_আকার);

  begin_pos = strstr(pem, "-----BEGIN PUBLIC KEY-----");
  যদি(!begin_pos)
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  pem_গুনতি = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  যদি(pem_গুনতি && '\n' != pem[pem_গুনতি - 1])
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_গুনতি += 26;

  /* Invalid if not directly following \n */
  end_pos = strstr(pem + pem_গুনতি, "\n-----END PUBLIC KEY-----");
  যদি(!end_pos)
    ফেরত CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  যখন(pem_গুনতি < pem_len) {
    যদি('\n' != pem[pem_গুনতি] && '\r' != pem[pem_গুনতি]) {
      ফলাফল = curlx_dyn_addn(&pbuf, &pem[pem_গুনতি], 1);
      যদি(ফলাফল)
        ফেরত ফলাফল;
    }
    ++pem_গুনতি;
  }

  যদি(curlx_dyn_len(&pbuf)) {
    ফলাফল = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  অন্যথা
    ফলাফল = CURLE_BAD_CONTENT_ENCODING;

  ফেরত ফলাফল;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *pinnedpubkey,
                              ধ্রুবক unsigned অক্ষর *pubkey, size_t pubkeylen)
{
  CURLcode ফলাফল = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  যদি(!pinnedpubkey)
    ফেরত CURLE_OK;
  যদি(!pubkey || !pubkeylen)
    ফেরত ফলাফল;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  যদি(!strncmp(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    অক্ষর *cert_hash = ফাঁকা;
    ধ্রুবক অক্ষর *pinned_hash, *end_pos;
    size_t cert_hash_len = 0, pinned_hash_len;
    unsigned অক্ষর *sha256sumdigest;

    যদি(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      ফেরত ফলাফল;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_দৈর্ঘ্য);
    যদি(!sha256sumdigest)
      ফেরত CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_দৈর্ঘ্য);

    যদি(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_দৈর্ঘ্য,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    যদি(encode)
      ফেরত encode;

    infof(তথ্য, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    যখন(pinned_hash &&
          !strncmp(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = strchr(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (size_t)(end_pos - pinned_hash) : strlen(pinned_hash);

      /* compare base64 sha256 digests */
      যদি(cert_hash_len == pinned_hash_len &&
         !memcmp(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(তথ্য, "public key hash matches pinned value"));
        ফলাফল = CURLE_OK;
        বিরতি;
      }

      DEBUGF(infof(তথ্য, "public key hash does not match 'sha256//%.*s'",
                   (পূর্ণসংখ্যা)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : ফাঁকা;
    }
    curlx_safefree(cert_hash);
  }
  অন্যথা {
    long filesize;
    size_t আকার_2, pem_len;
    CURLcode pem_read;
    কাঠামো dynbuf buf;
    unsigned অক্ষর *pem_ptr = ফাঁকা;
    size_t বাম;
    FILE *fp = curlx_fopen(pinnedpubkey, "rb");
    যদি(!fp)
      ফেরত ফলাফল;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_আকার);

    /* Determine the file's size */
    যদি(fseek(fp, 0, SEEK_END))
      যাও end;
    filesize = ftell(fp);
    যদি(fseek(fp, 0, SEEK_SET))
      যাও end;
    যদি(filesize < 0 || filesize > MAX_PINNED_PUBKEY_আকার)
      যাও end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    আকার_2 = curlx_sotouz((curl_off_t)filesize);
    যদি(pubkeylen > আকার_2)
      যাও end;

    /*
     * Read the file into the dynbuf
     */
    বাম = আকার_2;
    করো {
      অক্ষর বাফার[1024];
      size_t want = বাম > আকার(বাফার) ? আকার(বাফার) : বাম;
      যদি(fread(বাফার, 1, want, fp) != want)
        যাও end;
      যদি(curlx_dyn_addn(&buf, বাফার, want))
        যাও end;
      বাম -= want;
    } যখন(বাম);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    যদি(pubkeylen == আকার_2) {
      যদি(!memcmp(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        ফলাফল = CURLE_OK;
      যাও end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    যদি(pem_read)
      যাও end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    যদি(pubkeylen == pem_len && !memcmp(pubkey, pem_ptr, pubkeylen))
      ফলাফল = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  ফেরত ফলাফল;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(শূন্য)
{
  যদি(Curl_ssl->cert_status_request)
    ফেরত Curl_ssl->cert_status_request();
  ফেরত FALSE;
}

স্থির পূর্ণসংখ্যা multissl_init(শূন্য)
{
  যদি(multissl_setup(ফাঁকা))
    ফেরত 1;
  যদি(Curl_ssl->init)
    ফেরত Curl_ssl->init();
  ফেরত 1;
}

স্থির CURLcode multissl_random(কাঠামো Curl_easy *তথ্য,
                                unsigned অক্ষর *entropy, size_t দৈর্ঘ্য)
{
  যদি(multissl_setup(ফাঁকা))
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_ssl->random(তথ্য, entropy, দৈর্ঘ্য);
}

স্থির CURLcode multissl_connect(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য, bool *done)
{
  যদি(multissl_setup(ফাঁকা))
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_ssl->do_connect(cf, তথ্য, done);
}

স্থির CURLcode multissl_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো easy_pollset *ps)
{
  যদি(multissl_setup(ফাঁকা))
    ফেরত CURLE_OK;
  ফেরত Curl_ssl->adjust_pollset(cf, তথ্য, ps);
}

স্থির শূন্য *multissl_get_internals(কাঠামো ssl_connect_তথ্য *connssl,
                                    CURLINFO info)
{
  যদি(multissl_setup(ফাঁকা))
    ফেরত ফাঁকা;
  ফেরত Curl_ssl->get_internals(connssl, info);
}

স্থির শূন্য multissl_close(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  যদি(multissl_setup(ফাঁকা))
    ফেরত;
  Curl_ssl->close(cf, তথ্য);
}

স্থির CURLcode multissl_recv_plain(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    অক্ষর *buf, size_t len, size_t *pnread)
{
  যদি(multissl_setup(ফাঁকা))
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_ssl->recv_plain(cf, তথ্য, buf, len, pnread);
}

স্থির CURLcode multissl_send_plain(কাঠামো Curl_cfilter *cf,
                                    কাঠামো Curl_easy *তথ্য,
                                    ধ্রুবক শূন্য *mem, size_t len,
                                    size_t *pnwritten)
{
  যদি(multissl_setup(ফাঁকা))
    ফেরত CURLE_FAILED_INIT;
  ফেরত Curl_ssl->send_plain(cf, তথ্য, mem, len, pnwritten);
}

স্থির ধ্রুবক কাঠামো Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (size_t)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  ফাঁকা,                              /* cleanup */
  multissl_version,                  /* version */
  ফাঁকা,                              /* shutdown */
  ফাঁকা,                              /* data_pending */
  multissl_random,                   /* random */
  ফাঁকা,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  ফাঁকা,                              /* close_all */
  ফাঁকা,                              /* engines_list */
  ফাঁকা,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  ফাঁকা,                              /* get_channel_binding */
};

ধ্রুবক কাঠামো Curl_ssl *Curl_ssl =
#ifdef CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#নতুবা_যদি defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#নতুবা_যদি defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#নতুবা_যদি defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#নতুবা_যদি defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#নতুবা_যদি defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#নতুবা_যদি defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#অন্যথা
#error "Missing struct Curl_ssl for selected SSL backend"
#endif

স্থির ধ্রুবক কাঠামো Curl_ssl *available_backends[] = {
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
  ফাঁকা
};

/* Global cleanup */
শূন্য Curl_ssl_cleanup(শূন্য)
{
  যদি(init_ssl) {
    /* only cleanup if we did a previous init */
    যদি(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#ifdef CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#endif
    init_ssl = FALSE;
  }
}

স্থির size_t multissl_version(অক্ষর *বাফার, size_t আকার_2)
{
  স্থির ধ্রুবক কাঠামো Curl_ssl *selected;
  স্থির অক্ষর backends[200];
  স্থির size_t backends_len;
  ধ্রুবক কাঠামো Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  যদি(current != selected) {
    অক্ষর *p = backends;
    ধ্রুবক অক্ষর *end = backends + আকার(backends);
    পূর্ণসংখ্যা i;

    selected = current;

    backends[0] = '\0';

    জন্য(i = 0; available_backends[i]; ++i) {
      অক্ষর vb[200];
      bool paren = (selected != available_backends[i]);

      যদি(available_backends[i]->version(vb, আকার(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  যদি(আকার_2) {
    curlx_strcopy(বাফার, আকার_2, backends, backends_len);
  }
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা multissl_setup(ধ্রুবক কাঠামো Curl_ssl *backend)
{
  পূর্ণসংখ্যা i;
  অক্ষর *env;

  যদি(Curl_ssl != &Curl_ssl_multi)
    ফেরত 1;

  যদি(backend) {
    Curl_ssl = backend;
    ফেরত 0;
  }

  যদি(!available_backends[0])
    ফেরত 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  যদি(env) {
    জন্য(i = 0; available_backends[i]; i++) {
      যদি(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        ফেরত 0;
      }
    }
  }

#ifdef CURL_DEFAULT_SSL_BACKEND
  জন্য(i = 0; available_backends[i]; i++) {
    যদি(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      ফেরত 0;
    }
  }
#endif

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  ফেরত 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ধ্রুবক অক্ষর *name,
                                   ধ্রুবক curl_ssl_backend ***avail)
{
  পূর্ণসংখ্যা i;

  যদি(avail)
    *avail = (ধ্রুবক curl_ssl_backend **)&available_backends;

  যদি(Curl_ssl != &Curl_ssl_multi)
    ফেরত id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#ifdef CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#অন্যথা
           CURLSSLSET_UNKNOWN_BACKEND;
#endif

  জন্য(i = 0; available_backends[i]; i++) {
    যদি(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      ফেরত CURLSSLSET_OK;
    }
  }

  ফেরত CURLSSLSET_UNKNOWN_BACKEND;
}

#অন্যথা /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ধ্রুবক অক্ষর *name,
                                   ধ্রুবক curl_ssl_backend ***avail)
{
  (শূন্য)id;
  (শূন্য)name;
  (শূন্য)avail;
  ফেরত CURLSSLSET_NO_BACKENDS;
}

#endif /* !USE_SSL */

#ifdef USE_SSL

শূন্য Curl_ssl_peer_cleanup(কাঠামো ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_চাবি);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

স্থির ssl_peer_type get_peer_type(ধ্রুবক অক্ষর *hostname)
{
  যদি(hostname && hostname[0]) {
#ifdef USE_IPV6
    কাঠামো in6_addr addr;
#অন্যথা
    কাঠামো in_addr addr;
#endif
    যদি(curlx_inet_pton(AF_INET, hostname, &addr))
      ফেরত CURL_SSL_PEER_IPV4;
#ifdef USE_IPV6
    অন্যথা যদি(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      ফেরত CURL_SSL_PEER_IPV6;
    }
#endif
  }
  ফেরত CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(কাঠামো ssl_peer *ssl_peer,
                            কাঠামো Curl_peer *origin,
                            কাঠামো Curl_peer *peer,
                            কাঠামো ssl_filter_config *sslc,
                            ধ্রুবক অক্ষর *tls_id,
                            uint8_t transport)
{
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  যদি(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    ফেরত CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  যদি(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    size_t len = strlen(origin->hostname);
    যদি(len && (origin->hostname[len - 1] == '.'))
      len--;
    যদি(len < USHRT_MAX) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      যদি(!ssl_peer->sni)
        যাও out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  ফলাফল = Curl_ssl_peer_চাবি_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_চাবি);

out:
  যদি(ফলাফল)
    Curl_ssl_peer_cleanup(ssl_peer);
  ফেরত ফলাফল;
}

স্থির শূন্য ssl_cf_destroy(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  যদি(connssl) {
    connssl->ssl_impl->close(cf, তথ্য);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = ফাঁকা;
  }
  cf->connected = FALSE;
}

স্থির CURLcode ssl_cf_connect(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল;

  যদি(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }

  যদি(!cf->পরবর্তী || !connssl->peer.origin) {
    *done = FALSE;
    ফেরত CURLE_FAILED_INIT;
  }

  যদি(!cf->পরবর্তী->connected) {
    ফলাফল = cf->পরবর্তী->cft->do_connect(cf->পরবর্তী, তথ্য, done);
    যদি(ফলাফল || !*done)
      ফেরত ফলাফল;
  }

  CF_তথ্য_SAVE(save, cf, তথ্য);
  CURL_TRC_CF(তথ্য, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  যদি(!connssl->prefs_checked) {
    যদি(!ssl_prefs_check(cf, তথ্য)) {
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও out;
    }
    connssl->prefs_checked = TRUE;
  }

  ফলাফল = connssl->ssl_impl->do_connect(cf, তথ্য, done);

  যদি(!ফলাফল && *done) {
    cf->connected = TRUE;
    যদি(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(তথ্য);
    }
    যদি(Curl_tls_keylog_enabled()) {
      infof(তথ্য, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#ifdef LIBRESSL_VERSION_সংখ্যা
      infof(তথ্য, "Note LibreSSL only supports SSLKEYLOGFILE for TLS <= 1.2");
#endif
    }
    /* Connection can be deferred when sending early data */
    DEBUGASSERT(connssl->state == ssl_connection_complete ||
                connssl->state == ssl_connection_deferred);
    DEBUGASSERT(connssl->state != ssl_connection_deferred ||
                connssl->earlydata_state > ssl_earlydata_none);
  }
out:
  CURL_TRC_CF(তথ্য, cf, "cf_connect() -> %d, done=%d", (পূর্ণসংখ্যা)ফলাফল, *done);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode ssl_cf_set_earlydata(কাঠামো Curl_cfilter *cf,
                                     কাঠামো Curl_easy *তথ্য,
                                     ধ্রুবক শূন্য *buf, size_t blen)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  size_t nwritten = 0;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  যদি(blen) {
    যদি(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    ফলাফল = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(তথ্য, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssl_cf_connect_deferred(কাঠামো Curl_cfilter *cf,
                                        কাঠামো Curl_easy *তথ্য,
                                        ধ্রুবক শূন্য *buf, size_t blen,
                                        bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  যদি(connssl->earlydata_state == ssl_earlydata_await) {
    ফলাফল = ssl_cf_set_earlydata(cf, তথ্য, buf, blen);
    যদি(ফলাফল)
      ফেরত ফলাফল;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  ফলাফল = ssl_cf_connect(cf, তথ্য, done);

  যদি(!ফলাফল && *done) {
    যদি(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(তথ্য, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    নির্বাচন(connssl->earlydata_state) {
    ক্ষেত্র ssl_earlydata_none:
      বিরতি;
    ক্ষেত্র ssl_earlydata_accepted:
      যদি(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_তথ্য(তথ্য, (curl_off_t)connssl->earlydata_skip);
      infof(তথ্য, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      বিরতি;
    ক্ষেত্র ssl_earlydata_rejected:
      যদি(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_তথ্য(তথ্য, -(curl_off_t)connssl->earlydata_skip);
      infof(তথ্য, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      বিরতি;
    পূর্বনির্ধারিত:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(ফাঁকা);
      বিরতি;
    }
  }
  ফেরত ফলাফল;
}

স্থির bool ssl_cf_তথ্য_pending(কাঠামো Curl_cfilter *cf,
                                ধ্রুবক কাঠামো Curl_easy *তথ্য)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  bool pending;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  যদি(connssl->ssl_impl->তথ্য_pending &&
     connssl->ssl_impl->তথ্য_pending(cf, তথ্য))
    pending = TRUE;
  অন্যথা
    pending = cf->পরবর্তী->cft->has_তথ্য_pending(cf->পরবর্তী, তথ্য);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত pending;
}

স্থির CURLcode ssl_cf_send(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক uint8_t *buf, size_t blen,
                            bool eos, size_t *pnwritten)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল = CURLE_OK;

  (শূন্য)eos;
  *pnwritten = 0;
  CF_তথ্য_SAVE(save, cf, তথ্য);

  যদি(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    ফলাফল = ssl_cf_connect_deferred(cf, তথ্য, buf, blen, &done);
    যদি(ফলাফল)
      যাও out;
    অন্যথা যদি(!done) {
      ফলাফল = CURLE_AGAIN;
      যাও out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  যদি(connssl->earlydata_skip) {
    যদি(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      ফলাফল = CURLE_OK;
      *pnwritten = blen;
      যাও out;
    }
    অন্যথা {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  যদি(blen > 0) {
    size_t nwritten;
    ফলাফল = connssl->ssl_impl->send_plain(cf, তথ্য, buf, blen, &nwritten);
    যদি(!ফলাফল)
      *pnwritten += nwritten;
  }

out:
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode ssl_cf_recv(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য, অক্ষর *buf, size_t len,
                            size_t *pnread)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল = CURLE_OK;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  *pnread = 0;
  যদি(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    ফলাফল = ssl_cf_connect_deferred(cf, তথ্য, ফাঁকা, 0, &done);
    যদি(ফলাফল)
      যাও out;
    অন্যথা যদি(!done) {
      ফলাফল = CURLE_AGAIN;
      যাও out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  ফলাফল = connssl->ssl_impl->recv_plain(cf, তথ্য, buf, len, pnread);

out:
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode ssl_cf_shutdown(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  যদি(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    কাঠামো cf_call_তথ্য save;

    CF_তথ্য_SAVE(save, cf, তথ্য);
    ফলাফল = connssl->ssl_impl->shut_down(cf, তথ্য, TRUE, done);
    CURL_TRC_CF(তথ্য, cf, "cf_shutdown -> %d, done=%d", (পূর্ণসংখ্যা)ফলাফল, *done);
    CF_তথ্য_RESTORE(cf, save);
    cf->shutdown = (ফলাফল || *done);
  }
  ফেরত ফলাফল;
}

স্থির CURLcode ssl_cf_adjust_pollset(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো easy_pollset *ps)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল;

  CF_তথ্য_SAVE(save, cf, তথ্য);
  ফলাফল = connssl->ssl_impl->adjust_pollset(cf, তথ্য, ps);
  CF_তথ্য_RESTORE(cf, save);
  ফেরত ফলাফল;
}

স্থির CURLcode ssl_cf_query(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             পূর্ণসংখ্যা query, পূর্ণসংখ্যা *pres1, শূন্য *pres2)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;

  নির্বাচন(query) {
  ক্ষেত্র CF_QUERY_SSL_INFO:
  ক্ষেত্র CF_QUERY_SSL_CTX_INFO:
    যদি(!Curl_ssl_cf_is_proxy(cf)) {
      কাঠামো curl_tlssessioninfo *info = pres2;
      কাঠামো cf_call_তথ্য save;
      CF_তথ্য_SAVE(save, cf, তথ্য);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_তথ্য_RESTORE(cf, save);
      ফেরত CURLE_OK;
    }
    বিরতি;
  ক্ষেত্র CF_QUERY_ALPN_NEGOTIATED: {
    ধ্রুবক অক্ষর **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(তথ্য, cf, "query ALPN: returning '%s'", *palpn);
    ফেরত CURLE_OK;
  }
  পূর্বনির্ধারিত:
    বিরতি;
  }
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->query(cf->পরবর্তী, তথ্য, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

স্থির CURLcode ssl_cf_cntrl(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             পূর্ণসংখ্যা event, পূর্ণসংখ্যা arg1, শূন্য *arg2)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;

  (শূন্য)arg1;
  (শূন্য)arg2;
  (শূন্য)তথ্য;
  নির্বাচন(event) {
  ক্ষেত্র CF_CTRL_CONN_INFO_UPDATE:
    যদি(connssl->negotiated.alpn && !cf->sockindex) {
      যদি(!strcmp("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      অন্যথা যদি(!strcmp("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      অন্যথা যদি(!strcmp("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    বিরতি;
  ক্ষেত্র CF_CTRL_REPORT_STATS:
    যদি(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(তথ্য, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির bool cf_ssl_is_alive(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য,
                            bool *প্রবেশ_pending)
{
  /*
   * This function tries to determine connection status.
   */
  ফেরত cf->পরবর্তী ?
    cf->পরবর্তী->cft->is_alive(cf->পরবর্তী, তথ্য, প্রবেশ_pending) :
    FALSE; /* pessimistic in absence of data */
}

কাঠামো Curl_cftype Curl_cft_ssl = {
  "SSL",
  CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_তথ্য_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#ifndef CURL_DISABLE_PROXY

কাঠামো Curl_cftype Curl_cft_ssl_proxy = {
  "SSL-PROXY",
  CF_TYPE_SSL | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_তথ্য_pending,
  ssl_cf_send,
  ssl_cf_recv,
  Curl_cf_def_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#endif /* !CURL_DISABLE_PROXY */

স্থির CURLcode cf_ssl_create(কাঠামো Curl_cfilter **pcf,
                              কাঠামো Curl_easy *তথ্য,
                              কাঠামো connectdata *conn)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো ssl_connect_তথ্য *ctx;
  CURLcode ফলাফল;

  DEBUGASSERT(তথ্য->conn);

#ifdef CURL_DISABLE_HTTP
  (শূন্য)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(তথ্য, ফাঁকা);
#অন্যথা
  ctx = cf_ctx_new(তথ্য, alpn_get_spec(তথ্য->state.http_neg.wanted,
                                       তথ্য->state.http_neg.preferred,
                                       (bool)তথ্য->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#endif
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }

  ফলাফল = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  যদি(ফলাফল)
    cf_ctx_free(ctx);
  *pcf = ফলাফল ? ফাঁকা : cf;
  ফেরত ফলাফল;
}

স্থির CURLcode cf_ssl_peer_init(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_peer *origin,
                                 কাঠামো Curl_peer *peer,
                                 কাঠামো ssl_filter_config *sslc)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  অক্ষর tls_id[80];
  connssl->ssl_impl->version(tls_id, আকার(tls_id) - 1);
  ফেরত Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(কাঠামো Curl_easy *তথ্য,
                              কাঠামো Curl_peer *origin,
                              কাঠামো connectdata *conn,
                              int8_t sockindex)
{
  কাঠামো Curl_cfilter *cf;
  কাঠামো Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode ফলাফল;

  ফলাফল = cf_ssl_create(&cf, তথ্য, conn);
  যদি(!ফলাফল)
    ফলাফল = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  যদি(!ফলাফল)
    Curl_conn_cf_add(তথ্য, conn, sockindex, cf);
  অন্যথা যদি(cf)
    Curl_conn_cf_discard_chain(&cf, তথ্য);
  ফেরত ফলাফল;
}

CURLcode Curl_cf_ssl_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো Curl_peer *origin,
                                  কাঠামো Curl_peer *peer)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;

  ফলাফল = cf_ssl_create(&cf, তথ্য, cf_at->conn);
  যদি(!ফলাফল)
    ফলাফল = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  যদি(!ফলাফল) {
    Curl_conn_cf_ঢোকাও_after(cf_at, cf);
#যদি defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    যদি((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      ফলাফল = Curl_conn_dns_add_https_resolve(তথ্য, cf->conn, cf->sockindex,
                                               origin);
    }
#endif /* USE_HTTPSRR && USE_ECH */
  }
  অন্যথা যদি(cf)
    Curl_conn_cf_discard_chain(&cf, তথ্য);
  ফেরত ফলাফল;
}

#ifndef CURL_DISABLE_PROXY

স্থির CURLcode cf_ssl_proxy_create(কাঠামো Curl_cfilter **pcf,
                                    কাঠামো Curl_easy *তথ্য,
                                    কাঠামো connectdata *conn)
{
  কাঠামো Curl_cfilter *cf = ফাঁকা;
  কাঠামো ssl_connect_তথ্য *ctx;
  CURLcode ফলাফল;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)তথ্য->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (শূন্য)conn;
#ifdef USE_HTTP2
  যদি(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#endif

  ctx = cf_ctx_new(তথ্য, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  যদি(!ctx) {
    ফলাফল = CURLE_OUT_OF_MEMORY;
    যাও out;
  }
  ফলাফল = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  যদি(ফলাফল)
    cf_ctx_free(ctx);
  *pcf = ফলাফল ? ফাঁকা : cf;
  ফেরত ফলাফল;
}

CURLcode Curl_cf_ssl_proxy_ঢোকাও_after(কাঠামো Curl_cfilter *cf_at,
                                        কাঠামো Curl_easy *তথ্য,
                                        কাঠামো Curl_peer *peer)
{
  কাঠামো Curl_cfilter *cf;
  CURLcode ফলাফল;

  ফলাফল = cf_ssl_proxy_create(&cf, তথ্য, cf_at->conn);
  যদি(!ফলাফল)
    ফলাফল = cf_ssl_peer_init(cf, peer, ফাঁকা, &cf_at->conn->proxy_ssl_config);
  যদি(!ফলাফল)
    Curl_conn_cf_ঢোকাও_after(cf_at, cf);
  অন্যথা যদি(cf)
    Curl_conn_cf_discard_chain(&cf, তথ্য);
  ফেরত ফলাফল;
}

#endif /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(কাঠামো Curl_easy *তথ্য, unsigned পূর্ণসংখ্যা ssl_option)
{
  (শূন্য)তথ্য;
  ফেরত (Curl_ssl->supports & ssl_option);
}

#ifdef USE_ECH
bool Curl_ssl_ech_enabled(কাঠামো Curl_cfilter *cf)
{
  যদি(Curl_ssl->supports & SSLSUPP_ECH) {
    কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    ফেরত !!conn_config->ech;
  }
  ফেরত FALSE;
}

bool Curl_ssl_need_httpsrr(কাঠামো Curl_cfilter *cf)
{
  যদি(Curl_ssl->supports & SSLSUPP_ECH) {
    কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    যদি(!conn_config->ech)
      ফেরত FALSE;
    যদি((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      ফেরত FALSE;
    ফেরত TRUE;
  }
  ফেরত FALSE;
}
#endif

স্থির CURLcode vtls_shutdown_blocking(কাঠামো Curl_cfilter *cf,
                                       কাঠামো Curl_easy *তথ্য,
                                       bool send_shutdown, bool *done)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  কাঠামো cf_call_তথ্য save;
  CURLcode ফলাফল = CURLE_OK;
  timediff_t timeout_ms;
  পূর্ণসংখ্যা what, loop = 10;

  যদি(cf->shutdown) {
    *done = TRUE;
    ফেরত CURLE_OK;
  }
  CF_তথ্য_SAVE(save, cf, তথ্য);

  *done = FALSE;
  যখন(!ফলাফল && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(তথ্য, cf->conn, cf->sockindex);

    যদি(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(তথ্য, "SSL shutdown timeout");
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      যাও out;
    }

    ফলাফল = connssl->ssl_impl->shut_down(cf, তথ্য, send_shutdown, done);
    যদি(ফলাফল || *done)
      যাও out;

    যদি(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, তথ্য, timeout_ms);
      যদি(what < 0) {
        /* fatal error */
        failf(তথ্য, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        ফলাফল = CURLE_RECV_ERROR;
        যাও out;
      }
      অন্যথা যদি(what == 0) {
        /* timeout */
        failf(তথ্য, "SSL shutdown timeout");
        ফলাফল = CURLE_OPERATION_TIMEDOUT;
        যাও out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_তথ্য_RESTORE(cf, save);
  cf->shutdown = (ফলাফল || *done);
  ফেরত ফলাফল;
}

CURLcode Curl_ssl_cfilter_remove(কাঠামো Curl_easy *তথ্য,
                                 int8_t sockindex, bool send_shutdown)
{
  কাঠামো Curl_cfilter *cf, *শীর্ষ;
  CURLcode ফলাফল = CURLE_OK;

  শীর্ষ = তথ্য->conn ? তথ্য->conn->cfilter[sockindex] : ফাঁকা;
  জন্য(cf = শীর্ষ; cf; cf = cf->পরবর্তী) {
    যদি(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(তথ্য, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(তথ্য, sockindex, 0);
      ফলাফল = vtls_shutdown_blocking(cf, তথ্য, send_shutdown, &done);
      Curl_cshutdn_clear_timer(তথ্য, sockindex);
      যদি(!ফলাফল && !done) /* blocking failed? */
        ফলাফল = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, তথ্য);
      CURL_TRC_CF(তথ্য, cf, "shutdown and remove SSL, done -> %d",
                  (পূর্ণসংখ্যা)ফলাফল);
      বিরতি;
    }
  }
  ফেরত ফলাফল;
}

bool Curl_ssl_cf_is_proxy(কাঠামো Curl_cfilter *cf)
{
  ফেরত (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

কাঠামো ssl_easy_config *
Curl_ssl_cf_get_easy_config(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য)
{
#ifdef CURL_DISABLE_PROXY
  (শূন্য)cf;
  ফেরত &তথ্য->set.ssl;
#অন্যথা
  ফেরত Curl_ssl_cf_is_proxy(cf) ? &তথ্য->set.proxy_ssl : &তথ্য->set.ssl;
#endif
}

কাঠামো ssl_filter_config *
Curl_ssl_cf_get_filter_config(কাঠামো Curl_cfilter *cf)
{
#ifdef CURL_DISABLE_PROXY
  ফেরত &cf->conn->ssl_config;
#অন্যথা
  ফেরত Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#endif
}

CURLcode Curl_alpn_to_proto_buf(কাঠামো alpn_proto_buf *buf,
                                ধ্রুবক কাঠামো alpn_spec *spec)
{
  size_t i, len;
  পূর্ণসংখ্যা off = 0;
  unsigned অক্ষর blen;

  memset(buf, 0, আকার(*buf));
  জন্য(i = 0; spec && i < spec->গুনতি; ++i) {
    len = strlen(spec->entries[i]);
    যদি(len >= ALPN_NAME_MAX)
      ফেরত CURLE_FAILED_INIT;
    blen = (unsigned অক্ষর)len;
    যদি(off + blen + 1 >= (পূর্ণসংখ্যা)আকার(buf->তথ্য))
      ফেরত CURLE_FAILED_INIT;
    buf->তথ্য[off++] = blen;
    memcpy(buf->তথ্য + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  ফেরত CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(কাঠামো alpn_proto_buf *buf,
                                ধ্রুবক কাঠামো alpn_spec *spec)
{
  size_t i, len;
  size_t off = 0;

  memset(buf, 0, আকার(*buf));
  জন্য(i = 0; spec && i < spec->গুনতি; ++i) {
    len = strlen(spec->entries[i]);
    যদি(len >= ALPN_NAME_MAX)
      ফেরত CURLE_FAILED_INIT;
    যদি(off + len + 2 >= আকার(buf->তথ্য))
      ফেরত CURLE_FAILED_INIT;
    যদি(off)
      buf->তথ্য[off++] = ',';
    memcpy(buf->তথ্য + off, spec->entries[i], len);
    off += len;
  }
  buf->তথ্য[off] = '\0';
  buf->len = (পূর্ণসংখ্যা)off;
  ফেরত CURLE_OK;
}

bool Curl_alpn_contains_proto(ধ্রুবক কাঠামো alpn_spec *spec,
                              ধ্রুবক অক্ষর *proto)
{
  size_t i, plen = proto ? strlen(proto) : 0;
  জন্য(i = 0; spec && plen && i < spec->গুনতি; ++i) {
    size_t slen = strlen(spec->entries[i]);
    যদি((slen == plen) && !memcmp(proto, spec->entries[i], plen))
      ফেরত TRUE;
  }
  ফেরত FALSE;
}

শূন্য Curl_alpn_restrict_to(কাঠামো alpn_spec *spec, ধ্রুবক অক্ষর *proto)
{
  size_t plen = strlen(proto);
  DEBUGASSERT(plen < আকার(spec->entries[0]));
  যদি(plen < আকার(spec->entries[0])) {
    memcpy(spec->entries[0], proto, plen + 1);
    spec->গুনতি = 1;
  }
}

শূন্য Curl_alpn_copy(কাঠামো alpn_spec *dest, ধ্রুবক কাঠামো alpn_spec *src)
{
  যদি(src)
    memcpy(dest, src, আকার(*dest));
  অন্যথা
    memset(dest, 0, আকার(*dest));
}

CURLcode Curl_alpn_set_negotiated(কাঠামো Curl_cfilter *cf,
                                  কাঠামো Curl_easy *তথ্য,
                                  কাঠামো ssl_connect_তথ্য *connssl,
                                  ধ্রুবক unsigned অক্ষর *proto,
                                  size_t proto_len)
{
  CURLcode ফলাফল = CURLE_OK;
  (শূন্য)cf;

  যদি(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    যদি(!proto_len) {
      failf(তথ্য, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও out;
    }
    অন্যথা যদি(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও out;
    }
    অন্যথা যদি((strlen(connssl->negotiated.alpn) != proto_len) ||
            memcmp(connssl->negotiated.alpn, proto, proto_len)) {
      failf(তথ্য, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (পূর্ণসংখ্যা)proto_len, proto);
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(তথ্য, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    যাও out;
  }

  যদি(proto && proto_len) {
    যদি(memchr(proto, '\0', proto_len)) {
      failf(তথ্য, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      ফলাফল = CURLE_SSL_CONNECT_ERROR;
      যাও out;
    }
    connssl->negotiated.alpn = curlx_memdup0((ধ্রুবক অক্ষর *)proto, proto_len);
    যদি(!connssl->negotiated.alpn)
      ফেরত CURLE_OUT_OF_MEMORY;
  }

  যদি(proto && proto_len) {
    যদি(connssl->state == ssl_connection_deferred)
      infof(তথ্য, VTLS_INFOF_ALPN_DEFERRED, (পূর্ণসংখ্যা)proto_len, proto);
    অন্যথা
      infof(তথ্য, VTLS_INFOF_ALPN_ACCEPTED, (পূর্ণসংখ্যা)proto_len, proto);
  }
  অন্যথা {
    যদি(connssl->state == ssl_connection_deferred)
      infof(তথ্য, VTLS_INFOF_NO_ALPN_DEFERRED);
    অন্যথা
      infof(তথ্য, VTLS_INFOF_NO_ALPN);
  }

out:
  ফেরত ফলাফল;
}

CURLcode Curl_on_session_reuse(কাঠামো Curl_cfilter *cf,
                               কাঠামো Curl_easy *তথ্য,
                               কাঠামো alpn_spec *alpns,
                               কাঠামো Curl_ssl_session *scs,
                               bool *do_early_তথ্য, bool early_তথ্য_allowed)
{
  কাঠামো ssl_connect_তথ্য *connssl = cf->ctx;
  CURLcode ফলাফল = CURLE_OK;

  *do_early_তথ্য = FALSE;

  যদি(!early_তথ্য_allowed) {
    CURL_TRC_CF(তথ্য, cf, "SSL session does not allow earlydata");
  }
  অন্যথা যদি(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(তথ্য, cf, "SSL session has different ALPN, no early data");
  }
  অন্যথা {
    infof(তথ্য, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    ফলাফল = Curl_alpn_set_negotiated(cf, তথ্য, connssl,
                                      (ধ্রুবক unsigned অক্ষর *)scs->alpn,
                                      scs->alpn ? strlen(scs->alpn) : 0);
    *do_early_তথ্য = !ফলাফল;
  }
  ফেরত ফলাফল;
}

কাঠামো Curl_ssl_session *Curl_ssl_get_cf_session(কাঠামো Curl_easy *তথ্য,
                                                 ধ্রুবক কাঠামো Curl_cftype *cft,
                                                 int8_t sockindex)
{
  যদি(তথ্য->conn &&
#ifndef CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#অন্যথা
     (cft == &Curl_cft_ssl)) {
#endif
    কাঠামো Curl_cfilter *cf1 = তথ্য->conn->cfilter[sockindex];
    জন্য(; cf1; cf1 = cf1->পরবর্তী) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      যদি(cf1->cft == cft)
        বিরতি;
    }
    যদি(cf1) {
      কাঠামো ssl_connect_তথ্য *connssl = cf1->ctx;
      যদি(connssl)
        ফেরত connssl->session;
    }
  }
  ফেরত ফাঁকা;
}

#endif /* USE_SSL */
