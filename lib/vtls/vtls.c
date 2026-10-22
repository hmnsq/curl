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

#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "cfilters.h"

#समावेश "vtls/vtls.h" /* generic SSL protos etc */
#समावेश "vtls/vtls_int.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "vtls/keylog.h"

#समावेश "vtls/openssl.h"        /* OpenSSL versions */
#समावेश "vtls/gtls.h"           /* GnuTLS versions */
#समावेश "vtls/wolfssl.h"        /* wolfSSL versions */
#समावेश "vtls/schannel.h"       /* Schannel SSPI version */
#समावेश "vtls/mbedtls.h"        /* mbedTLS versions */
#समावेश "vtls/rustls.h"         /* Rustls versions */

#समावेश "slist.h"
#समावेश "curl_trc.h"
#समावेश "strcase.h"
#समावेश "url.h"
#समावेश "progress.h"
#समावेश "curlx/win32-fopen.h"
#समावेश "curl_sha256.h"
#समावेश "curlx/base64.h"
#समावेश "curlx/inet_pton.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "setopt.h"
#समावेश "vdns/cf-dns.h"
#समावेश "curlx/strdup.h"
#समावेश "curlx/strcopy.h"

#अगर_घोषित USE_APPLE_SECTRUST
#समावेश <Security/Security.h>
#पूर्ण_अगर

#अगर_घोषित USE_SSL
#अगर !defined(CURL_DISABLE_HTTP) || !defined(CURL_DISABLE_PROXY)
अटल स्थिर काष्ठा alpn_spec ALPN_SPEC_H11 = {
  { ALPN_HTTP_1_1 }, 1
};
अटल स्थिर काष्ठा alpn_spec ALPN_SPEC_H10_H11 = {
  { ALPN_HTTP_1_0, ALPN_HTTP_1_1 }, 2
};
#अगर_घोषित USE_HTTP2
अटल स्थिर काष्ठा alpn_spec ALPN_SPEC_H2 = {
  { ALPN_H2 }, 1
};
अटल स्थिर काष्ठा alpn_spec ALPN_SPEC_H2_H11 = {
  { ALPN_H2, ALPN_HTTP_1_1 }, 2
};
अटल स्थिर काष्ठा alpn_spec ALPN_SPEC_H11_H2 = {
  { ALPN_HTTP_1_1, ALPN_H2 }, 2
};
#पूर्ण_अगर /* USE_HTTP2 */

अटल स्थिर काष्ठा alpn_spec *alpn_get_spec(http_majors wanted,
                                             http_majors preferred,
                                             bool only_http_10,
                                             bool use_alpn)
{
  अगर(!use_alpn)
    वापस शून्य;
  /* If HTTP/1.0 is the wanted protocol then use ALPN http/1.0 and http/1.1.
     This is for compatibility reasons since some HTTP/1.0 servers with old
     ALPN implementations understand ALPN http/1.1 but not http/1.0. */
  अगर(only_http_10 && (wanted & CURL_HTTP_V1x))
    वापस &ALPN_SPEC_H10_H11;
#अगर_घोषित USE_HTTP2
  अगर(wanted & CURL_HTTP_V2x) {
    अगर(wanted & CURL_HTTP_V1x)
      वापस (preferred == CURL_HTTP_V1x) ?
        &ALPN_SPEC_H11_H2 : &ALPN_SPEC_H2_H11;
    वापस &ALPN_SPEC_H2;
  }
#अन्यथा
  (व्योम)wanted;
  (व्योम)preferred;
#पूर्ण_अगर
  वापस &ALPN_SPEC_H11;
}
#पूर्ण_अगर /* !CURL_DISABLE_HTTP || !CURL_DISABLE_PROXY */
#पूर्ण_अगर /* USE_SSL */

#अगर_घोषित USE_SSL
अटल पूर्णांक multissl_setup(स्थिर काष्ठा Curl_ssl *backend);
#पूर्ण_अगर

curl_sslbackend Curl_ssl_backend(व्योम)
{
#अगर_घोषित USE_SSL
  multissl_setup(शून्य);
  वापस Curl_ssl->info.id;
#अन्यथा
  वापस CURLSSLBACKEND_NONE;
#पूर्ण_अगर
}

#अगर_घोषित USE_SSL

/* "global" init done? */
अटल bool init_ssl = FALSE;

/**
 * Global SSL init
 *
 * @retval 0 error initializing SSL
 * @retval 1 SSL initialized successfully
 */
पूर्णांक Curl_ssl_init(व्योम)
{
  /* make sure this is only done once */
  अगर(init_ssl)
    वापस 1;
  init_ssl = TRUE; /* never again */

  अगर(Curl_ssl->init)
    वापस Curl_ssl->init();
  वापस 1;
}

अटल bool ssl_prefs_check(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  /* check for CURLOPT_SSLVERSION invalid parameter value */
  अगर(conn_config->version >= CURL_SSLVERSION_LAST) {
    failf(तथ्य, "Unrecognized parameter value passed via CURLOPT_SSLVERSION");
    वापस FALSE;
  }

  चयन(conn_config->version_max) {
  हाल CURL_SSLVERSION_MAX_NONE:
  हाल CURL_SSLVERSION_MAX_DEFAULT:
    अवरोध;

  शेष:
    अगर((conn_config->version_max >> 16) < conn_config->version) {
      failf(तथ्य, "CURL_SSLVERSION_MAX incompatible with CURL_SSLVERSION");
      वापस FALSE;
    }
  }

  वापस TRUE;
}

अटल काष्ठा ssl_connect_तथ्य *cf_ctx_new(काष्ठा Curl_easy *तथ्य,
                                           स्थिर काष्ठा alpn_spec *alpn)
{
  काष्ठा ssl_connect_तथ्य *ctx;

  (व्योम)तथ्य;
  ctx = curlx_calloc(1, माप(*ctx));
  अगर(!ctx)
    वापस शून्य;

  ctx->ssl_impl = Curl_ssl;
  ctx->alpn = alpn;
  Curl_bufq_init2(&ctx->earlydata, CURL_SSL_EARLY_MAX, 1, BUFQ_OPT_NO_SPARES);
  ctx->backend = curlx_calloc(1, ctx->ssl_impl->sizeof_ssl_backend_तथ्य);
  अगर(!ctx->backend) {
    curlx_free(ctx);
    वापस शून्य;
  }
  वापस ctx;
}

अटल व्योम cf_ctx_free(काष्ठा ssl_connect_तथ्य *ctx)
{
  अगर(ctx) {
    curlx_safefree(ctx->negotiated.alpn);
    Curl_bufq_free(&ctx->earlydata);
    curlx_free(ctx->backend);
    curlx_free(ctx);
  }
}

CURLcode Curl_ssl_get_channel_binding(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                                      काष्ठा dynbuf *binding)
{
  अगर(Curl_ssl->get_channel_binding)
    वापस Curl_ssl->get_channel_binding(तथ्य, sockindex, binding);
  वापस CURLE_OK;
}

व्योम Curl_ssl_close_all(काष्ठा Curl_easy *तथ्य)
{
  अगर(Curl_ssl->close_all)
    Curl_ssl->close_all(तथ्य);
}

CURLcode Curl_ssl_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 काष्ठा easy_pollset *ps)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;

  अगर(connssl->io_need) {
    curl_socket_t sock = Curl_conn_cf_get_socket(cf->अग्र, तथ्य);
    CURLcode परिणाम = CURLE_OK;
    अगर(sock != CURL_SOCKET_BAD) {
      अगर(connssl->io_need & CURL_SSL_IO_NEED_SEND) {
        परिणाम = Curl_pollset_set_out_only(तथ्य, ps, sock);
        CURL_TRC_CF(तथ्य, cf, "adjust_pollset, POLLOUT fd=%" FMT_SOCKET_T,
                    sock);
      }
      अन्यथा {
        परिणाम = Curl_pollset_set_in_only(तथ्य, ps, sock);
        CURL_TRC_CF(तथ्य, cf, "adjust_pollset, POLLIN fd=%" FMT_SOCKET_T,
                    sock);
      }
    }
    वापस परिणाम;
  }
  वापस CURLE_OK;
}

/* Return list of OpenSSL crypto engine names. */
काष्ठा curl_slist *Curl_ssl_engines_list(काष्ठा Curl_easy *तथ्य)
{
  अगर(Curl_ssl->engines_list)
    वापस Curl_ssl->engines_list(तथ्य);
  वापस शून्य;
}

अटल माप_प्रकार multissl_version(अक्षर *बफर, माप_प्रकार आकार);

व्योम Curl_ssl_version(अक्षर *बफर, माप_प्रकार आकार)
{
#अगर_घोषित CURL_WITH_MULTI_SSL
  (व्योम)multissl_version(बफर, आकार);
#अन्यथा
  (व्योम)Curl_ssl->version(बफर, आकार);
#पूर्ण_अगर
}

व्योम Curl_ssl_free_certinfo(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा curl_certinfo *ci = &तथ्य->info.certs;

  अगर(ci->अंक_of_certs) {
    /* free all individual lists used */
    पूर्णांक i;
    क्रम(i = 0; i < ci->अंक_of_certs; i++) {
      curl_slist_free_all(ci->certinfo[i]);
      ci->certinfo[i] = शून्य;
    }

    curlx_safefree(ci->certinfo); /* free the actual array too */
    ci->अंक_of_certs = 0;
  }
}

CURLcode Curl_ssl_init_certinfo(काष्ठा Curl_easy *तथ्य, पूर्णांक अंक)
{
  काष्ठा curl_certinfo *ci = &तथ्य->info.certs;
  काष्ठा curl_slist **table;

  /* Free any previous certificate information structures */
  Curl_ssl_free_certinfo(तथ्य);

  /* Allocate the required certificate information structures */
  table = curlx_calloc((माप_प्रकार)अंक, माप(काष्ठा curl_slist *));
  अगर(!table)
    वापस CURLE_OUT_OF_MEMORY;

  ci->अंक_of_certs = अंक;
  ci->certinfo = table;

  वापस CURLE_OK;
}

/*
 * 'value' is NOT a null-terminated string
 */
CURLcode Curl_ssl_धकेलो_certinfo_len(काष्ठा Curl_easy *तथ्य,
                                    पूर्णांक certnum,
                                    स्थिर अक्षर *label,
                                    स्थिर अक्षर *मूल्य,
                                    माप_प्रकार valuelen)
{
  काष्ठा curl_certinfo *ci = &तथ्य->info.certs;
  काष्ठा curl_slist *nl;
  CURLcode परिणाम = CURLE_OK;
  काष्ठा dynbuf build;

  DEBUGASSERT(certnum < ci->अंक_of_certs);

  curlx_dyn_init(&build, CURL_X509_STR_MAX);

  अगर(curlx_dyn_add(&build, label) ||
     curlx_dyn_addn(&build, ":", 1) ||
     curlx_dyn_addn(&build, मूल्य, valuelen))
    वापस CURLE_OUT_OF_MEMORY;

  nl = Curl_slist_append_nodup(ci->certinfo[certnum], curlx_dyn_ptr(&build));
  अगर(!nl) {
    curlx_dyn_free(&build);
    curl_slist_free_all(ci->certinfo[certnum]);
    परिणाम = CURLE_OUT_OF_MEMORY;
  }

  ci->certinfo[certnum] = nl;
  वापस परिणाम;
}

/* get length bytes of randomness */
CURLcode Curl_ssl_random(काष्ठा Curl_easy *तथ्य,
                         अचिन्हित अक्षर *बफर, माप_प्रकार लंबाई)
{
  DEBUGASSERT(लंबाई == माप(पूर्णांक));
  अगर(Curl_ssl->random)
    वापस Curl_ssl->random(तथ्य, बफर, लंबाई);
  अन्यथा
    वापस CURLE_NOT_BUILT_IN;
}

/*
 * Public key pem to der conversion
 */

अटल CURLcode pubkey_pem_to_der(स्थिर अक्षर *pem,
                                  अचिन्हित अक्षर **der, माप_प्रकार *der_len)
{
  स्थिर अक्षर *begin_pos, *end_pos;
  माप_प्रकार pem_गिनती, pem_len;
  CURLcode परिणाम;
  काष्ठा dynbuf pbuf;

  /* if no pem, exit. */
  अगर(!pem)
    वापस CURLE_BAD_CONTENT_ENCODING;

  curlx_dyn_init(&pbuf, MAX_PINNED_PUBKEY_आकार);

  begin_pos = म_माला(pem, "-----BEGIN PUBLIC KEY-----");
  अगर(!begin_pos)
    वापस CURLE_BAD_CONTENT_ENCODING;

  pem_गिनती = begin_pos - pem;
  /* Invalid if not at beginning AND not directly following \n */
  अगर(pem_गिनती && '\n' != pem[pem_गिनती - 1])
    वापस CURLE_BAD_CONTENT_ENCODING;

  /* 26 is length of "-----BEGIN PUBLIC KEY-----" */
  pem_गिनती += 26;

  /* Invalid if not directly following \n */
  end_pos = म_माला(pem + pem_गिनती, "\n-----END PUBLIC KEY-----");
  अगर(!end_pos)
    वापस CURLE_BAD_CONTENT_ENCODING;

  pem_len = end_pos - pem;

  /*
   * Here we loop through the pem array one character at a time between the
   * correct indices, and place each character that is not '\n' or '\r'
   * into the stripped_pem array, which should represent the raw base64 string
   */
  जबतक(pem_गिनती < pem_len) {
    अगर('\n' != pem[pem_गिनती] && '\r' != pem[pem_गिनती]) {
      परिणाम = curlx_dyn_addn(&pbuf, &pem[pem_गिनती], 1);
      अगर(परिणाम)
        वापस परिणाम;
    }
    ++pem_गिनती;
  }

  अगर(curlx_dyn_len(&pbuf)) {
    परिणाम = curlx_base64_decode(curlx_dyn_ptr(&pbuf), der, der_len);
    curlx_dyn_free(&pbuf);
  }
  अन्यथा
    परिणाम = CURLE_BAD_CONTENT_ENCODING;

  वापस परिणाम;
}

/*
 * Generic pinned public key check.
 */

CURLcode Curl_pin_peer_pubkey(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *pinnedpubkey,
                              स्थिर अचिन्हित अक्षर *pubkey, माप_प्रकार pubkeylen)
{
  CURLcode परिणाम = CURLE_SSL_PINNEDPUBKEYNOTMATCH;

  /* if a path was not specified, do not pin */
  अगर(!pinnedpubkey)
    वापस CURLE_OK;
  अगर(!pubkey || !pubkeylen)
    वापस परिणाम;

  /* only do this if pinnedpubkey starts with "sha256//", length 8 */
  अगर(!म_भेदन(pinnedpubkey, "sha256//", 8)) {
    CURLcode encode;
    अक्षर *cert_hash = शून्य;
    स्थिर अक्षर *pinned_hash, *end_pos;
    माप_प्रकार cert_hash_len = 0, pinned_hash_len;
    अचिन्हित अक्षर *sha256sumdigest;

    अगर(!Curl_ssl->sha256sum) {
      /* without sha256 support, this cannot match */
      वापस परिणाम;
    }

    /* compute sha256sum of public key */
    sha256sumdigest = curlx_malloc(CURL_SHA256_DIGEST_लंबाई);
    अगर(!sha256sumdigest)
      वापस CURLE_OUT_OF_MEMORY;
    encode = Curl_ssl->sha256sum(pubkey, pubkeylen,
                                 sha256sumdigest, CURL_SHA256_DIGEST_लंबाई);

    अगर(!encode)
      encode = curlx_base64_encode(sha256sumdigest,
                                   CURL_SHA256_DIGEST_लंबाई,
                                   &cert_hash, &cert_hash_len);
    curlx_safefree(sha256sumdigest);

    अगर(encode)
      वापस encode;

    infof(तथ्य, " public key hash: sha256//%s", cert_hash);

    pinned_hash = pinnedpubkey;
    जबतक(pinned_hash &&
          !म_भेदन(pinned_hash, "sha256//", CURL_CSTRLEN("sha256//"))) {
      pinned_hash = pinned_hash + CURL_CSTRLEN("sha256//");
      end_pos = म_अक्षर(pinned_hash, ';');
      pinned_hash_len = end_pos ?
                        (माप_प्रकार)(end_pos - pinned_hash) : म_माप(pinned_hash);

      /* compare base64 sha256 digests */
      अगर(cert_hash_len == pinned_hash_len &&
         !स_भेद(cert_hash, pinned_hash, cert_hash_len)) {
        DEBUGF(infof(तथ्य, "public key hash matches pinned value"));
        परिणाम = CURLE_OK;
        अवरोध;
      }

      DEBUGF(infof(तथ्य, "public key hash does not match 'sha256//%.*s'",
                   (पूर्णांक)pinned_hash_len, pinned_hash));
      /* next one or we are at the end */
      pinned_hash = end_pos ? (end_pos + 1) : शून्य;
    }
    curlx_safefree(cert_hash);
  }
  अन्यथा {
    दीर्घ filesize;
    माप_प्रकार आकार, pem_len;
    CURLcode pem_read;
    काष्ठा dynbuf buf;
    अचिन्हित अक्षर *pem_ptr = शून्य;
    माप_प्रकार बायाँ;
    खाता *fp = curlx_fopen(pinnedpubkey, "rb");
    अगर(!fp)
      वापस परिणाम;

    curlx_dyn_init(&buf, MAX_PINNED_PUBKEY_आकार);

    /* Determine the file's size */
    अगर(ख_जाओ(fp, 0, अंत_से))
      जाओ end;
    filesize = ख_बताओ(fp);
    अगर(ख_जाओ(fp, 0, शुरू_से))
      जाओ end;
    अगर(filesize < 0 || filesize > MAX_PINNED_PUBKEY_आकार)
      जाओ end;

    /*
     * if the size of our certificate is bigger than the file
     * size then it cannot match
     */
    आकार = curlx_sotouz((curl_off_t)filesize);
    अगर(pubkeylen > आकार)
      जाओ end;

    /*
     * Read the file into the dynbuf
     */
    बायाँ = आकार;
    करो {
      अक्षर बफर[1024];
      माप_प्रकार want = बायाँ > माप(बफर) ? माप(बफर) : बायाँ;
      अगर(ख_पढ़ो(बफर, 1, want, fp) != want)
        जाओ end;
      अगर(curlx_dyn_addn(&buf, बफर, want))
        जाओ end;
      बायाँ -= want;
    } जबतक(बायाँ);

    /* If the sizes are the same, it cannot be base64 encoded, must be der */
    अगर(pubkeylen == आकार) {
      अगर(!स_भेद(pubkey, curlx_dyn_ptr(&buf), pubkeylen))
        परिणाम = CURLE_OK;
      जाओ end;
    }

    /*
     * Otherwise we assume it is PEM and try to decode it after placing
     * null-terminator
     */
    pem_read = pubkey_pem_to_der(curlx_dyn_ptr(&buf), &pem_ptr, &pem_len);
    /* if it was not read successfully, exit */
    अगर(pem_read)
      जाओ end;

    /*
     * if the size of our certificate does not match the size of
     * the decoded file, they cannot be the same, otherwise compare
     */
    अगर(pubkeylen == pem_len && !स_भेद(pubkey, pem_ptr, pubkeylen))
      परिणाम = CURLE_OK;
end:
    curlx_dyn_free(&buf);
    curlx_safefree(pem_ptr);
    curlx_fclose(fp);
  }

  वापस परिणाम;
}

/*
 * Check whether the SSL backend supports the status_request extension.
 */
bool Curl_ssl_cert_status_request(व्योम)
{
  अगर(Curl_ssl->cert_status_request)
    वापस Curl_ssl->cert_status_request();
  वापस FALSE;
}

अटल पूर्णांक multissl_init(व्योम)
{
  अगर(multissl_setup(शून्य))
    वापस 1;
  अगर(Curl_ssl->init)
    वापस Curl_ssl->init();
  वापस 1;
}

अटल CURLcode multissl_random(काष्ठा Curl_easy *तथ्य,
                                अचिन्हित अक्षर *entropy, माप_प्रकार लंबाई)
{
  अगर(multissl_setup(शून्य))
    वापस CURLE_FAILED_INIT;
  वापस Curl_ssl->random(तथ्य, entropy, लंबाई);
}

अटल CURLcode multissl_connect(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य, bool *done)
{
  अगर(multissl_setup(शून्य))
    वापस CURLE_FAILED_INIT;
  वापस Curl_ssl->do_connect(cf, तथ्य, done);
}

अटल CURLcode multissl_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा easy_pollset *ps)
{
  अगर(multissl_setup(शून्य))
    वापस CURLE_OK;
  वापस Curl_ssl->adjust_pollset(cf, तथ्य, ps);
}

अटल व्योम *multissl_get_internals(काष्ठा ssl_connect_तथ्य *connssl,
                                    CURLINFO info)
{
  अगर(multissl_setup(शून्य))
    वापस शून्य;
  वापस Curl_ssl->get_internals(connssl, info);
}

अटल व्योम multissl_close(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  अगर(multissl_setup(शून्य))
    वापस;
  Curl_ssl->बंद(cf, तथ्य);
}

अटल CURLcode multissl_recv_plain(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    अक्षर *buf, माप_प्रकार len, माप_प्रकार *pnread)
{
  अगर(multissl_setup(शून्य))
    वापस CURLE_FAILED_INIT;
  वापस Curl_ssl->recv_plain(cf, तथ्य, buf, len, pnread);
}

अटल CURLcode multissl_send_plain(काष्ठा Curl_cfilter *cf,
                                    काष्ठा Curl_easy *तथ्य,
                                    स्थिर व्योम *mem, माप_प्रकार len,
                                    माप_प्रकार *pnwritten)
{
  अगर(multissl_setup(शून्य))
    वापस CURLE_FAILED_INIT;
  वापस Curl_ssl->send_plain(cf, तथ्य, mem, len, pnwritten);
}

अटल स्थिर काष्ठा Curl_ssl Curl_ssl_multi = {
  { CURLSSLBACKEND_NONE, "multi" },  /* info */
  0, /* supports nothing */
  (माप_प्रकार)-1, /* something insanely large to be on the safe side */

  multissl_init,                     /* init */
  शून्य,                              /* cleanup */
  multissl_version,                  /* version */
  शून्य,                              /* shutdown */
  शून्य,                              /* data_pending */
  multissl_random,                   /* random */
  शून्य,                              /* cert_status_request */
  multissl_connect,                  /* connect */
  multissl_adjust_pollset,           /* adjust_pollset */
  multissl_get_internals,            /* get_internals */
  multissl_close,                    /* close_one */
  शून्य,                              /* close_all */
  शून्य,                              /* engines_list */
  शून्य,                              /* sha256sum */
  multissl_recv_plain,               /* recv decrypted data */
  multissl_send_plain,               /* send data to encrypt */
  शून्य,                              /* get_channel_binding */
};

स्थिर काष्ठा Curl_ssl *Curl_ssl =
#अगर_घोषित CURL_WITH_MULTI_SSL
  &Curl_ssl_multi;
#या_अगर defined(USE_WOLFSSL)
  &Curl_ssl_wolfssl;
#या_अगर defined(USE_GNUTLS)
  &Curl_ssl_gnutls;
#या_अगर defined(USE_MBEDTLS)
  &Curl_ssl_mbedtls;
#या_अगर defined(USE_RUSTLS)
  &Curl_ssl_rustls;
#या_अगर defined(USE_OPENSSL)
  &Curl_ssl_openssl;
#या_अगर defined(USE_SCHANNEL)
  &Curl_ssl_schannel;
#अन्यथा
#त्रुटि "Missing struct Curl_ssl for selected SSL backend"
#पूर्ण_अगर

अटल स्थिर काष्ठा Curl_ssl *available_backends[] = {
#अगर_घोषित USE_WOLFSSL
  &Curl_ssl_wolfssl,
#पूर्ण_अगर
#अगर_घोषित USE_GNUTLS
  &Curl_ssl_gnutls,
#पूर्ण_अगर
#अगर_घोषित USE_MBEDTLS
  &Curl_ssl_mbedtls,
#पूर्ण_अगर
#अगर_घोषित USE_OPENSSL
  &Curl_ssl_openssl,
#पूर्ण_अगर
#अगर_घोषित USE_SCHANNEL
  &Curl_ssl_schannel,
#पूर्ण_अगर
#अगर_घोषित USE_RUSTLS
  &Curl_ssl_rustls,
#पूर्ण_अगर
  शून्य
};

/* Global cleanup */
व्योम Curl_ssl_cleanup(व्योम)
{
  अगर(init_ssl) {
    /* only cleanup if we did a previous init */
    अगर(Curl_ssl->cleanup)
      Curl_ssl->cleanup();
#अगर_घोषित CURL_WITH_MULTI_SSL
    Curl_ssl = &Curl_ssl_multi;
#पूर्ण_अगर
    init_ssl = FALSE;
  }
}

अटल माप_प्रकार multissl_version(अक्षर *बफर, माप_प्रकार आकार)
{
  अटल स्थिर काष्ठा Curl_ssl *selected;
  अटल अक्षर backends[200];
  अटल माप_प्रकार backends_len;
  स्थिर काष्ठा Curl_ssl *current;

  current = Curl_ssl == &Curl_ssl_multi ? available_backends[0] : Curl_ssl;

  अगर(current != selected) {
    अक्षर *p = backends;
    स्थिर अक्षर *end = backends + माप(backends);
    पूर्णांक i;

    selected = current;

    backends[0] = '\0';

    क्रम(i = 0; available_backends[i]; ++i) {
      अक्षर vb[200];
      bool paren = (selected != available_backends[i]);

      अगर(available_backends[i]->version(vb, माप(vb))) {
        p += curl_msnprintf(p, end - p, "%s%s%s%s", (p != backends ? " " : ""),
                            (paren ? "(" : ""), vb, (paren ? ")" : ""));
      }
    }

    backends_len = p - backends;
  }

  अगर(आकार) {
    curlx_strcopy(बफर, आकार, backends, backends_len);
  }
  वापस 0;
}

अटल पूर्णांक multissl_setup(स्थिर काष्ठा Curl_ssl *backend)
{
  पूर्णांक i;
  अक्षर *env;

  अगर(Curl_ssl != &Curl_ssl_multi)
    वापस 1;

  अगर(backend) {
    Curl_ssl = backend;
    वापस 0;
  }

  अगर(!available_backends[0])
    वापस 1;

  env = curl_getenv("CURL_SSL_BACKEND");
  अगर(env) {
    क्रम(i = 0; available_backends[i]; i++) {
      अगर(curl_strequal(env, available_backends[i]->info.name)) {
        Curl_ssl = available_backends[i];
        curlx_free(env);
        वापस 0;
      }
    }
  }

#अगर_घोषित CURL_DEFAULT_SSL_BACKEND
  क्रम(i = 0; available_backends[i]; i++) {
    अगर(curl_strequal(CURL_DEFAULT_SSL_BACKEND,
                     available_backends[i]->info.name)) {
      Curl_ssl = available_backends[i];
      curlx_free(env);
      वापस 0;
    }
  }
#पूर्ण_अगर

  /* Fall back to first available backend */
  Curl_ssl = available_backends[0];
  curlx_free(env);
  वापस 0;
}

/* This function is used to select the SSL backend to use. It is called by
   curl_global_sslset (easy.c) which uses the global init lock. */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, स्थिर अक्षर *name,
                                   स्थिर curl_ssl_backend ***avail)
{
  पूर्णांक i;

  अगर(avail)
    *avail = (स्थिर curl_ssl_backend **)&available_backends;

  अगर(Curl_ssl != &Curl_ssl_multi)
    वापस id == Curl_ssl->info.id ||
           (name && curl_strequal(name, Curl_ssl->info.name)) ?
           CURLSSLSET_OK :
#अगर_घोषित CURL_WITH_MULTI_SSL
           CURLSSLSET_TOO_LATE;
#अन्यथा
           CURLSSLSET_UNKNOWN_BACKEND;
#पूर्ण_अगर

  क्रम(i = 0; available_backends[i]; i++) {
    अगर(available_backends[i]->info.id == id ||
       (name && curl_strequal(available_backends[i]->info.name, name))) {
      multissl_setup(available_backends[i]);
      वापस CURLSSLSET_OK;
    }
  }

  वापस CURLSSLSET_UNKNOWN_BACKEND;
}

#अन्यथा /* USE_SSL */
CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, स्थिर अक्षर *name,
                                   स्थिर curl_ssl_backend ***avail)
{
  (व्योम)id;
  (व्योम)name;
  (व्योम)avail;
  वापस CURLSSLSET_NO_BACKENDS;
}

#पूर्ण_अगर /* !USE_SSL */

#अगर_घोषित USE_SSL

व्योम Curl_ssl_peer_cleanup(काष्ठा ssl_peer *peer)
{
  Curl_peer_unlink(&peer->origin);
  Curl_peer_unlink(&peer->peer);
  curlx_safefree(peer->sni);
  curlx_safefree(peer->scache_कुंजी);
  peer->transport = TRNSPRT_NONE;
  peer->type = CURL_SSL_PEER_DNS;
}

अटल ssl_peer_type get_peer_type(स्थिर अक्षर *hostname)
{
  अगर(hostname && hostname[0]) {
#अगर_घोषित USE_IPV6
    काष्ठा in6_addr addr;
#अन्यथा
    काष्ठा in_addr addr;
#पूर्ण_अगर
    अगर(curlx_inet_pton(AF_INET, hostname, &addr))
      वापस CURL_SSL_PEER_IPV4;
#अगर_घोषित USE_IPV6
    अन्यथा अगर(curlx_inet_pton(AF_INET6, hostname, &addr)) {
      वापस CURL_SSL_PEER_IPV6;
    }
#पूर्ण_अगर
  }
  वापस CURL_SSL_PEER_DNS;
}

CURLcode Curl_ssl_peer_init(काष्ठा ssl_peer *ssl_peer,
                            काष्ठा Curl_peer *origin,
                            काष्ठा Curl_peer *peer,
                            काष्ठा ssl_filter_config *sslc,
                            स्थिर अक्षर *tls_id,
                            uint8_t transport)
{
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  /* We expect a clean struct, e.g. called only ONCE */
  अगर(!ssl_peer || !origin) {
    DEBUGASSERT(0);
    वापस CURLE_FAILED_INIT;
  }
  DEBUGASSERT(!ssl_peer->origin);
  DEBUGASSERT(!ssl_peer->peer);
  DEBUGASSERT(!ssl_peer->sni);
  ssl_peer->transport = transport;

  Curl_peer_link(&ssl_peer->origin, origin);
  Curl_peer_link(&ssl_peer->peer, peer);
  ssl_peer->type = get_peer_type(origin->hostname);
  अगर(ssl_peer->type == CURL_SSL_PEER_DNS) {
    /* not an IP address, normalize according to RCC 6066 ch. 3,
     * max len of SNI is 2^16-1, no trailing dot */
    माप_प्रकार len = म_माप(origin->hostname);
    अगर(len && (origin->hostname[len - 1] == '.'))
      len--;
    अगर(len < अच_लघु_उच्च) {
      ssl_peer->sni = curlx_calloc(1, len + 1);
      अगर(!ssl_peer->sni)
        जाओ out;
      Curl_strntolower(ssl_peer->sni, origin->hostname, len);
      ssl_peer->sni[len] = 0;
    }
  }

  परिणाम = Curl_ssl_peer_कुंजी_make(ssl_peer, sslc, tls_id,
                                  &ssl_peer->scache_कुंजी);

out:
  अगर(परिणाम)
    Curl_ssl_peer_cleanup(ssl_peer);
  वापस परिणाम;
}

अटल व्योम ssl_cf_destroy(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  अगर(connssl) {
    connssl->ssl_impl->बंद(cf, तथ्य);
    connssl->state = ssl_connection_none;
    connssl->connecting_state = ssl_connect_1;
    connssl->prefs_checked = FALSE;
    Curl_ssl_peer_cleanup(&connssl->peer);
    Curl_ssl_session_destroy(connssl->session);
    cf_ctx_free(connssl);
    cf->ctx = शून्य;
  }
  cf->connected = FALSE;
}

अटल CURLcode ssl_cf_connect(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम;

  अगर(cf->connected && (connssl->state != ssl_connection_deferred)) {
    *done = TRUE;
    वापस CURLE_OK;
  }

  अगर(!cf->अग्र || !connssl->peer.origin) {
    *done = FALSE;
    वापस CURLE_FAILED_INIT;
  }

  अगर(!cf->अग्र->connected) {
    परिणाम = cf->अग्र->cft->do_connect(cf->अग्र, तथ्य, done);
    अगर(परिणाम || !*done)
      वापस परिणाम;
  }

  CF_तथ्य_SAVE(save, cf, तथ्य);
  CURL_TRC_CF(तथ्य, cf, "cf_connect()");
  DEBUGASSERT(connssl);

  *done = FALSE;

  अगर(!connssl->prefs_checked) {
    अगर(!ssl_prefs_check(cf, तथ्य)) {
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ out;
    }
    connssl->prefs_checked = TRUE;
  }

  परिणाम = connssl->ssl_impl->do_connect(cf, तथ्य, done);

  अगर(!परिणाम && *done) {
    cf->connected = TRUE;
    अगर(connssl->state == ssl_connection_complete) {
      connssl->handshake_done = *Curl_pgrs_now(तथ्य);
    }
    अगर(Curl_tls_keylog_enabled()) {
      infof(तथ्य, "SSLKEYLOGFILE set, all TLS secrets are logged to '%s'",
            Curl_tls_keylog_file_name());
#अगर_घोषित LIBRESSL_VERSION_संख्या
      infof(तथ्य, "Note LibreSSL only supports SSLKEYLOGFILE for TLS <= 1.2");
#पूर्ण_अगर
    }
    /* Connection can be deferred when sending early data */
    DEBUGASSERT(connssl->state == ssl_connection_complete ||
                connssl->state == ssl_connection_deferred);
    DEBUGASSERT(connssl->state != ssl_connection_deferred ||
                connssl->earlydata_state > ssl_earlydata_none);
  }
out:
  CURL_TRC_CF(तथ्य, cf, "cf_connect() -> %d, done=%d", (पूर्णांक)परिणाम, *done);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode ssl_cf_set_earlydata(काष्ठा Curl_cfilter *cf,
                                     काष्ठा Curl_easy *तथ्य,
                                     स्थिर व्योम *buf, माप_प्रकार blen)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  माप_प्रकार nwritten = 0;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(connssl->earlydata_state == ssl_earlydata_await);
  DEBUGASSERT(Curl_bufq_is_empty(&connssl->earlydata));
  अगर(blen) {
    अगर(blen > connssl->earlydata_max)
      blen = connssl->earlydata_max;
    परिणाम = Curl_bufq_write(&connssl->earlydata, buf, blen, &nwritten);
    CURL_TRC_CF(तथ्य, cf, "ssl_cf_set_earlydata(len=%zu) -> %zu",
                blen, nwritten);
    अगर(परिणाम)
      वापस परिणाम;
  }
  वापस CURLE_OK;
}

अटल CURLcode ssl_cf_connect_deferred(काष्ठा Curl_cfilter *cf,
                                        काष्ठा Curl_easy *तथ्य,
                                        स्थिर व्योम *buf, माप_प्रकार blen,
                                        bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  DEBUGASSERT(connssl->state == ssl_connection_deferred);
  *done = FALSE;
  अगर(connssl->earlydata_state == ssl_earlydata_await) {
    परिणाम = ssl_cf_set_earlydata(cf, तथ्य, buf, blen);
    अगर(परिणाम)
      वापस परिणाम;
    /* we buffered any early data we would like to send. Actually
     * do the connect now which sends it and performs the handshake. */
    connssl->earlydata_state = ssl_earlydata_sending;
    connssl->earlydata_skip = Curl_bufq_len(&connssl->earlydata);
  }

  परिणाम = ssl_cf_connect(cf, तथ्य, done);

  अगर(!परिणाम && *done) {
    अगर(!connssl->stats_reported && (cf->cft == &Curl_cft_ssl)) {
      Curl_pgrsTimeWas(तथ्य, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    चयन(connssl->earlydata_state) {
    हाल ssl_earlydata_none:
      अवरोध;
    हाल ssl_earlydata_accepted:
      अगर(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_तथ्य(तथ्य, (curl_off_t)connssl->earlydata_skip);
      infof(तथ्य, "Server accepted %zu bytes of TLS early data.",
            connssl->earlydata_skip);
      अवरोध;
    हाल ssl_earlydata_rejected:
      अगर(!Curl_ssl_cf_is_proxy(cf))
        Curl_pgrs_Early_तथ्य(तथ्य, -(curl_off_t)connssl->earlydata_skip);
      infof(तथ्य, "Server rejected TLS early data.");
      connssl->earlydata_skip = 0;
      अवरोध;
    शेष:
      /* This should not happen. Either we do not use early data or we
       * should know if it was accepted or not. */
      DEBUGASSERT(शून्य);
      अवरोध;
    }
  }
  वापस परिणाम;
}

अटल bool ssl_cf_तथ्य_pending(काष्ठा Curl_cfilter *cf,
                                स्थिर काष्ठा Curl_easy *तथ्य)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  bool pending;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  अगर(connssl->ssl_impl->तथ्य_pending &&
     connssl->ssl_impl->तथ्य_pending(cf, तथ्य))
    pending = TRUE;
  अन्यथा
    pending = cf->अग्र->cft->has_तथ्य_pending(cf->अग्र, तथ्य);
  CF_तथ्य_RESTORE(cf, save);
  वापस pending;
}

अटल CURLcode ssl_cf_send(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            स्थिर uint8_t *buf, माप_प्रकार blen,
                            bool eos, माप_प्रकार *pnwritten)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम = CURLE_OK;

  (व्योम)eos;
  *pnwritten = 0;
  CF_तथ्य_SAVE(save, cf, तथ्य);

  अगर(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    परिणाम = ssl_cf_connect_deferred(cf, तथ्य, buf, blen, &done);
    अगर(परिणाम)
      जाओ out;
    अन्यथा अगर(!done) {
      परिणाम = CURLE_AGAIN;
      जाओ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  अगर(connssl->earlydata_skip) {
    अगर(connssl->earlydata_skip >= blen) {
      connssl->earlydata_skip -= blen;
      परिणाम = CURLE_OK;
      *pnwritten = blen;
      जाओ out;
    }
    अन्यथा {
      *pnwritten = connssl->earlydata_skip;
      buf = buf + connssl->earlydata_skip;
      blen -= connssl->earlydata_skip;
      connssl->earlydata_skip = 0;
    }
  }

  /* OpenSSL and maybe other TLS libs do not like 0-length writes. Skip. */
  अगर(blen > 0) {
    माप_प्रकार nwritten;
    परिणाम = connssl->ssl_impl->send_plain(cf, तथ्य, buf, blen, &nwritten);
    अगर(!परिणाम)
      *pnwritten += nwritten;
  }

out:
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode ssl_cf_recv(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य, अक्षर *buf, माप_प्रकार len,
                            माप_प्रकार *pnread)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम = CURLE_OK;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  *pnread = 0;
  अगर(connssl->state == ssl_connection_deferred) {
    bool done = FALSE;
    परिणाम = ssl_cf_connect_deferred(cf, तथ्य, शून्य, 0, &done);
    अगर(परिणाम)
      जाओ out;
    अन्यथा अगर(!done) {
      परिणाम = CURLE_AGAIN;
      जाओ out;
    }
    DEBUGASSERT(connssl->state == ssl_connection_complete);
  }

  परिणाम = connssl->ssl_impl->recv_plain(cf, तथ्य, buf, len, pnread);

out:
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode ssl_cf_shutdown(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  *done = TRUE;
  /* If we have done the SSL handshake, shut down the connection cleanly */
  अगर(cf->connected && (connssl->state == ssl_connection_complete) &&
     !cf->shutdown && Curl_ssl->shut_down) {
    काष्ठा cf_call_तथ्य save;

    CF_तथ्य_SAVE(save, cf, तथ्य);
    परिणाम = connssl->ssl_impl->shut_down(cf, तथ्य, TRUE, done);
    CURL_TRC_CF(तथ्य, cf, "cf_shutdown -> %d, done=%d", (पूर्णांक)परिणाम, *done);
    CF_तथ्य_RESTORE(cf, save);
    cf->shutdown = (परिणाम || *done);
  }
  वापस परिणाम;
}

अटल CURLcode ssl_cf_adjust_pollset(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा easy_pollset *ps)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम;

  CF_तथ्य_SAVE(save, cf, तथ्य);
  परिणाम = connssl->ssl_impl->adjust_pollset(cf, तथ्य, ps);
  CF_तथ्य_RESTORE(cf, save);
  वापस परिणाम;
}

अटल CURLcode ssl_cf_query(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             पूर्णांक query, पूर्णांक *pres1, व्योम *pres2)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;

  चयन(query) {
  हाल CF_QUERY_SSL_INFO:
  हाल CF_QUERY_SSL_CTX_INFO:
    अगर(!Curl_ssl_cf_is_proxy(cf)) {
      काष्ठा curl_tlssessioninfo *info = pres2;
      काष्ठा cf_call_तथ्य save;
      CF_तथ्य_SAVE(save, cf, तथ्य);
      info->backend = Curl_ssl_backend();
      info->internals = connssl->ssl_impl->get_internals(
        cf->ctx, (query == CF_QUERY_SSL_INFO) ?
        CURLINFO_TLS_SSL_PTR : CURLINFO_TLS_SESSION);
      CF_तथ्य_RESTORE(cf, save);
      वापस CURLE_OK;
    }
    अवरोध;
  हाल CF_QUERY_ALPN_NEGOTIATED: {
    स्थिर अक्षर **palpn = pres2;
    DEBUGASSERT(palpn);
    *palpn = connssl->negotiated.alpn;
    CURL_TRC_CF(तथ्य, cf, "query ALPN: returning '%s'", *palpn);
    वापस CURLE_OK;
  }
  शेष:
    अवरोध;
  }
  वापस cf->अग्र ?
    cf->अग्र->cft->query(cf->अग्र, तथ्य, query, pres1, pres2) :
    CURLE_UNKNOWN_OPTION;
}

अटल CURLcode ssl_cf_cntrl(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             पूर्णांक event, पूर्णांक arg1, व्योम *arg2)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;

  (व्योम)arg1;
  (व्योम)arg2;
  (व्योम)तथ्य;
  चयन(event) {
  हाल CF_CTRL_CONN_INFO_UPDATE:
    अगर(connssl->negotiated.alpn && !cf->sockindex) {
      अगर(!म_भेद("http/1.1", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 11;
      अन्यथा अगर(!म_भेद("h2", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 20;
      अन्यथा अगर(!म_भेद("h3", connssl->negotiated.alpn))
        cf->conn->httpversion_seen = 30;
    }
    अवरोध;
  हाल CF_CTRL_REPORT_STATS:
    अगर(cf->connected && !connssl->stats_reported &&
       (cf->cft == &Curl_cft_ssl) &&
       (connssl->handshake_done.tv_sec || connssl->handshake_done.tv_usec)) {
      Curl_pgrsTimeWas(तथ्य, TIMER_APPCONNECT, connssl->handshake_done);
      connssl->stats_reported = TRUE;
    }
    अवरोध;
  }
  वापस CURLE_OK;
}

अटल bool cf_ssl_is_alive(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य,
                            bool *निवेश_pending)
{
  /*
   * This function tries to determine connection status.
   */
  वापस cf->अग्र ?
    cf->अग्र->cft->is_alive(cf->अग्र, तथ्य, निवेश_pending) :
    FALSE; /* pessimistic in absence of data */
}

काष्ठा Curl_cftype Curl_cft_ssl = {
  "SSL",
  CF_TYPE_SSL,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_तथ्य_pending,
  ssl_cf_send,
  ssl_cf_recv,
  ssl_cf_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#अगर_अघोषित CURL_DISABLE_PROXY

काष्ठा Curl_cftype Curl_cft_ssl_proxy = {
  "SSL-PROXY",
  CF_TYPE_SSL | CF_TYPE_PROXY,
  CURL_LOG_LVL_NONE,
  ssl_cf_destroy,
  ssl_cf_connect,
  ssl_cf_shutdown,
  ssl_cf_adjust_pollset,
  ssl_cf_तथ्य_pending,
  ssl_cf_send,
  ssl_cf_recv,
  Curl_cf_def_cntrl,
  cf_ssl_is_alive,
  Curl_cf_def_conn_keep_alive,
  ssl_cf_query,
};

#पूर्ण_अगर /* !CURL_DISABLE_PROXY */

अटल CURLcode cf_ssl_create(काष्ठा Curl_cfilter **pcf,
                              काष्ठा Curl_easy *तथ्य,
                              काष्ठा connectdata *conn)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा ssl_connect_तथ्य *ctx;
  CURLcode परिणाम;

  DEBUGASSERT(तथ्य->conn);

#अगर_घोषित CURL_DISABLE_HTTP
  (व्योम)conn;
  /* We only support ALPN for HTTP so far. */
  DEBUGASSERT(!conn->bits.tls_enable_alpn);
  ctx = cf_ctx_new(तथ्य, शून्य);
#अन्यथा
  ctx = cf_ctx_new(तथ्य, alpn_get_spec(तथ्य->state.http_neg.wanted,
                                       तथ्य->state.http_neg.preferred,
                                       (bool)तथ्य->state.http_neg.only_10,
                                       (bool)conn->bits.tls_enable_alpn));
#पूर्ण_अगर
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }

  परिणाम = Curl_cf_create(&cf, &Curl_cft_ssl, ctx);

out:
  अगर(परिणाम)
    cf_ctx_free(ctx);
  *pcf = परिणाम ? शून्य : cf;
  वापस परिणाम;
}

अटल CURLcode cf_ssl_peer_init(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_peer *origin,
                                 काष्ठा Curl_peer *peer,
                                 काष्ठा ssl_filter_config *sslc)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  अक्षर tls_id[80];
  connssl->ssl_impl->version(tls_id, माप(tls_id) - 1);
  वापस Curl_ssl_peer_init(&connssl->peer, origin, peer, sslc,
                            tls_id, TRNSPRT_TCP);
}

CURLcode Curl_ssl_cfilter_add(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_peer *origin,
                              काष्ठा connectdata *conn,
                              int8_t sockindex)
{
  काष्ठा Curl_cfilter *cf;
  काष्ठा Curl_peer *peer = (sockindex == SECONDARYSOCKET) ?
    conn->via_peer2 : conn->via_peer;
  CURLcode परिणाम;

  परिणाम = cf_ssl_create(&cf, तथ्य, conn);
  अगर(!परिणाम)
    परिणाम = cf_ssl_peer_init(cf, origin, peer, &conn->ssl_config);
  अगर(!परिणाम)
    Curl_conn_cf_add(तथ्य, conn, sockindex, cf);
  अन्यथा अगर(cf)
    Curl_conn_cf_discard_chain(&cf, तथ्य);
  वापस परिणाम;
}

CURLcode Curl_cf_ssl_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा Curl_peer *origin,
                                  काष्ठा Curl_peer *peer)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_ssl_create(&cf, तथ्य, cf_at->conn);
  अगर(!परिणाम)
    परिणाम = cf_ssl_peer_init(cf, origin, peer, &cf_at->conn->ssl_config);
  अगर(!परिणाम) {
    Curl_conn_cf_जोड़ो_after(cf_at, cf);
#अगर defined(USE_HTTPSRR) && defined(USE_ECH)
    /* When using ECH, kick off the HTTPS-RR resolve */
    अगर((origin->scheme->family == CURLPROTO_HTTP) &&
       Curl_ssl_need_httpsrr(cf)) {
      परिणाम = Curl_conn_dns_add_https_resolve(तथ्य, cf->conn, cf->sockindex,
                                               origin);
    }
#पूर्ण_अगर /* USE_HTTPSRR && USE_ECH */
  }
  अन्यथा अगर(cf)
    Curl_conn_cf_discard_chain(&cf, तथ्य);
  वापस परिणाम;
}

#अगर_अघोषित CURL_DISABLE_PROXY

अटल CURLcode cf_ssl_proxy_create(काष्ठा Curl_cfilter **pcf,
                                    काष्ठा Curl_easy *तथ्य,
                                    काष्ठा connectdata *conn)
{
  काष्ठा Curl_cfilter *cf = शून्य;
  काष्ठा ssl_connect_तथ्य *ctx;
  CURLcode परिणाम;
  /* ALPN is default, but if user explicitly disables it, obey */
  bool use_alpn = (bool)तथ्य->set.ssl_enable_alpn;
  http_majors wanted = CURL_HTTP_V1x;

  (व्योम)conn;
#अगर_घोषित USE_HTTP2
  अगर(conn->http_proxy.proxytype == CURLPROXY_HTTPS2) {
    use_alpn = TRUE;
    wanted = (CURL_HTTP_V1x | CURL_HTTP_V2x);
  }
#पूर्ण_अगर

  ctx = cf_ctx_new(तथ्य, alpn_get_spec(wanted, 0, FALSE, use_alpn));
  अगर(!ctx) {
    परिणाम = CURLE_OUT_OF_MEMORY;
    जाओ out;
  }
  परिणाम = Curl_cf_create(&cf, &Curl_cft_ssl_proxy, ctx);

out:
  अगर(परिणाम)
    cf_ctx_free(ctx);
  *pcf = परिणाम ? शून्य : cf;
  वापस परिणाम;
}

CURLcode Curl_cf_ssl_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा Curl_peer *peer)
{
  काष्ठा Curl_cfilter *cf;
  CURLcode परिणाम;

  परिणाम = cf_ssl_proxy_create(&cf, तथ्य, cf_at->conn);
  अगर(!परिणाम)
    परिणाम = cf_ssl_peer_init(cf, peer, शून्य, &cf_at->conn->proxy_ssl_config);
  अगर(!परिणाम)
    Curl_conn_cf_जोड़ो_after(cf_at, cf);
  अन्यथा अगर(cf)
    Curl_conn_cf_discard_chain(&cf, तथ्य);
  वापस परिणाम;
}

#पूर्ण_अगर /* !CURL_DISABLE_PROXY */

bool Curl_ssl_supports(काष्ठा Curl_easy *तथ्य, अचिन्हित पूर्णांक ssl_option)
{
  (व्योम)तथ्य;
  वापस (Curl_ssl->supports & ssl_option);
}

#अगर_घोषित USE_ECH
bool Curl_ssl_ech_enabled(काष्ठा Curl_cfilter *cf)
{
  अगर(Curl_ssl->supports & SSLSUPP_ECH) {
    काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    वापस !!conn_config->ech;
  }
  वापस FALSE;
}

bool Curl_ssl_need_httpsrr(काष्ठा Curl_cfilter *cf)
{
  अगर(Curl_ssl->supports & SSLSUPP_ECH) {
    काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
    अगर(!conn_config->ech)
      वापस FALSE;
    अगर((conn_config->ech == CURLECH_GREASE) || conn_config->ech_config)
      वापस FALSE;
    वापस TRUE;
  }
  वापस FALSE;
}
#पूर्ण_अगर

अटल CURLcode vtls_shutdown_blocking(काष्ठा Curl_cfilter *cf,
                                       काष्ठा Curl_easy *तथ्य,
                                       bool send_shutdown, bool *done)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  काष्ठा cf_call_तथ्य save;
  CURLcode परिणाम = CURLE_OK;
  timediff_t timeout_ms;
  पूर्णांक what, loop = 10;

  अगर(cf->shutdown) {
    *done = TRUE;
    वापस CURLE_OK;
  }
  CF_तथ्य_SAVE(save, cf, तथ्य);

  *done = FALSE;
  जबतक(!परिणाम && !*done && loop--) {
    timeout_ms = Curl_cshutdn_timeleft_ms(तथ्य, cf->conn, cf->sockindex);

    अगर(timeout_ms < 0) {
      /* no need to continue if time is already up */
      failf(तथ्य, "SSL shutdown timeout");
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      जाओ out;
    }

    परिणाम = connssl->ssl_impl->shut_down(cf, तथ्य, send_shutdown, done);
    अगर(परिणाम || *done)
      जाओ out;

    अगर(connssl->io_need) {
      what = Curl_conn_cf_poll(cf, तथ्य, timeout_ms);
      अगर(what < 0) {
        /* fatal error */
        failf(तथ्य, "select/poll on SSL socket, errno: %d", SOCKERRNO);
        परिणाम = CURLE_RECV_ERROR;
        जाओ out;
      }
      अन्यथा अगर(what == 0) {
        /* timeout */
        failf(तथ्य, "SSL shutdown timeout");
        परिणाम = CURLE_OPERATION_TIMEDOUT;
        जाओ out;
      }
      /* socket is readable or writable */
    }
  }
out:
  CF_तथ्य_RESTORE(cf, save);
  cf->shutdown = (परिणाम || *done);
  वापस परिणाम;
}

CURLcode Curl_ssl_cfilter_remove(काष्ठा Curl_easy *तथ्य,
                                 int8_t sockindex, bool send_shutdown)
{
  काष्ठा Curl_cfilter *cf, *शीर्ष;
  CURLcode परिणाम = CURLE_OK;

  शीर्ष = तथ्य->conn ? तथ्य->conn->cfilter[sockindex] : शून्य;
  क्रम(cf = शीर्ष; cf; cf = cf->अग्र) {
    अगर(cf->cft == &Curl_cft_ssl) {
      bool done;
      CURL_TRC_CF(तथ्य, cf, "shutdown and remove SSL, start");
      Curl_cshutdn_start_timer(तथ्य, sockindex, 0);
      परिणाम = vtls_shutdown_blocking(cf, तथ्य, send_shutdown, &done);
      Curl_cshutdn_clear_timer(तथ्य, sockindex);
      अगर(!परिणाम && !done) /* blocking failed? */
        परिणाम = CURLE_SSL_SHUTDOWN_FAILED;
      Curl_conn_cf_discard(&cf, तथ्य);
      CURL_TRC_CF(तथ्य, cf, "shutdown and remove SSL, done -> %d",
                  (पूर्णांक)परिणाम);
      अवरोध;
    }
  }
  वापस परिणाम;
}

bool Curl_ssl_cf_is_proxy(काष्ठा Curl_cfilter *cf)
{
  वापस (cf->cft->flags & CF_TYPE_SSL) && (cf->cft->flags & CF_TYPE_PROXY);
}

काष्ठा ssl_easy_config *
Curl_ssl_cf_get_easy_config(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य)
{
#अगर_घोषित CURL_DISABLE_PROXY
  (व्योम)cf;
  वापस &तथ्य->set.ssl;
#अन्यथा
  वापस Curl_ssl_cf_is_proxy(cf) ? &तथ्य->set.proxy_ssl : &तथ्य->set.ssl;
#पूर्ण_अगर
}

काष्ठा ssl_filter_config *
Curl_ssl_cf_get_filter_config(काष्ठा Curl_cfilter *cf)
{
#अगर_घोषित CURL_DISABLE_PROXY
  वापस &cf->conn->ssl_config;
#अन्यथा
  वापस Curl_ssl_cf_is_proxy(cf) ?
    &cf->conn->proxy_ssl_config : &cf->conn->ssl_config;
#पूर्ण_अगर
}

CURLcode Curl_alpn_to_proto_buf(काष्ठा alpn_proto_buf *buf,
                                स्थिर काष्ठा alpn_spec *spec)
{
  माप_प्रकार i, len;
  पूर्णांक off = 0;
  अचिन्हित अक्षर blen;

  स_रखो(buf, 0, माप(*buf));
  क्रम(i = 0; spec && i < spec->गिनती; ++i) {
    len = म_माप(spec->entries[i]);
    अगर(len >= ALPN_NAME_MAX)
      वापस CURLE_FAILED_INIT;
    blen = (अचिन्हित अक्षर)len;
    अगर(off + blen + 1 >= (पूर्णांक)माप(buf->तथ्य))
      वापस CURLE_FAILED_INIT;
    buf->तथ्य[off++] = blen;
    स_नकल(buf->तथ्य + off, spec->entries[i], blen);
    off += blen;
  }
  buf->len = off;
  वापस CURLE_OK;
}

CURLcode Curl_alpn_to_proto_str(काष्ठा alpn_proto_buf *buf,
                                स्थिर काष्ठा alpn_spec *spec)
{
  माप_प्रकार i, len;
  माप_प्रकार off = 0;

  स_रखो(buf, 0, माप(*buf));
  क्रम(i = 0; spec && i < spec->गिनती; ++i) {
    len = म_माप(spec->entries[i]);
    अगर(len >= ALPN_NAME_MAX)
      वापस CURLE_FAILED_INIT;
    अगर(off + len + 2 >= माप(buf->तथ्य))
      वापस CURLE_FAILED_INIT;
    अगर(off)
      buf->तथ्य[off++] = ',';
    स_नकल(buf->तथ्य + off, spec->entries[i], len);
    off += len;
  }
  buf->तथ्य[off] = '\0';
  buf->len = (पूर्णांक)off;
  वापस CURLE_OK;
}

bool Curl_alpn_contains_proto(स्थिर काष्ठा alpn_spec *spec,
                              स्थिर अक्षर *proto)
{
  माप_प्रकार i, plen = proto ? म_माप(proto) : 0;
  क्रम(i = 0; spec && plen && i < spec->गिनती; ++i) {
    माप_प्रकार slen = म_माप(spec->entries[i]);
    अगर((slen == plen) && !स_भेद(proto, spec->entries[i], plen))
      वापस TRUE;
  }
  वापस FALSE;
}

व्योम Curl_alpn_restrict_to(काष्ठा alpn_spec *spec, स्थिर अक्षर *proto)
{
  माप_प्रकार plen = म_माप(proto);
  DEBUGASSERT(plen < माप(spec->entries[0]));
  अगर(plen < माप(spec->entries[0])) {
    स_नकल(spec->entries[0], proto, plen + 1);
    spec->गिनती = 1;
  }
}

व्योम Curl_alpn_copy(काष्ठा alpn_spec *dest, स्थिर काष्ठा alpn_spec *src)
{
  अगर(src)
    स_नकल(dest, src, माप(*dest));
  अन्यथा
    स_रखो(dest, 0, माप(*dest));
}

CURLcode Curl_alpn_set_negotiated(काष्ठा Curl_cfilter *cf,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा ssl_connect_तथ्य *connssl,
                                  स्थिर अचिन्हित अक्षर *proto,
                                  माप_प्रकार proto_len)
{
  CURLcode परिणाम = CURLE_OK;
  (व्योम)cf;

  अगर(connssl->negotiated.alpn) {
    /* When we ask for a specific ALPN protocol, we need the confirmation
     * of it by the server, as we have installed protocol handler and
     * connection filter chain for exactly this protocol. */
    अगर(!proto_len) {
      failf(तथ्य, "ALPN: asked for '%s' from previous session, "
            "but server did not confirm it. Refusing to continue.",
            connssl->negotiated.alpn);
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ out;
    }
    अन्यथा अगर(!proto) {
      DEBUGASSERT(0); /* with length, we need a pointer */
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ out;
    }
    अन्यथा अगर((म_माप(connssl->negotiated.alpn) != proto_len) ||
            स_भेद(connssl->negotiated.alpn, proto, proto_len)) {
      failf(तथ्य, "ALPN: asked for '%s' from previous session, but server "
            "selected '%.*s'. Refusing to continue.",
            connssl->negotiated.alpn, (पूर्णांक)proto_len, proto);
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ out;
    }
    /* ALPN is exactly what we asked for, done. */
    infof(तथ्य, "ALPN: server confirmed to use '%s'",
          connssl->negotiated.alpn);
    जाओ out;
  }

  अगर(proto && proto_len) {
    अगर(स_प्रथम(proto, '\0', proto_len)) {
      failf(तथ्य, "ALPN: server selected protocol contains NUL. "
                  "Refusing to continue.");
      परिणाम = CURLE_SSL_CONNECT_ERROR;
      जाओ out;
    }
    connssl->negotiated.alpn = curlx_memdup0((स्थिर अक्षर *)proto, proto_len);
    अगर(!connssl->negotiated.alpn)
      वापस CURLE_OUT_OF_MEMORY;
  }

  अगर(proto && proto_len) {
    अगर(connssl->state == ssl_connection_deferred)
      infof(तथ्य, VTLS_INFOF_ALPN_DEFERRED, (पूर्णांक)proto_len, proto);
    अन्यथा
      infof(तथ्य, VTLS_INFOF_ALPN_ACCEPTED, (पूर्णांक)proto_len, proto);
  }
  अन्यथा {
    अगर(connssl->state == ssl_connection_deferred)
      infof(तथ्य, VTLS_INFOF_NO_ALPN_DEFERRED);
    अन्यथा
      infof(तथ्य, VTLS_INFOF_NO_ALPN);
  }

out:
  वापस परिणाम;
}

CURLcode Curl_on_session_reuse(काष्ठा Curl_cfilter *cf,
                               काष्ठा Curl_easy *तथ्य,
                               काष्ठा alpn_spec *alpns,
                               काष्ठा Curl_ssl_session *scs,
                               bool *do_early_तथ्य, bool early_तथ्य_allowed)
{
  काष्ठा ssl_connect_तथ्य *connssl = cf->ctx;
  CURLcode परिणाम = CURLE_OK;

  *do_early_तथ्य = FALSE;

  अगर(!early_तथ्य_allowed) {
    CURL_TRC_CF(तथ्य, cf, "SSL session does not allow earlydata");
  }
  अन्यथा अगर(!Curl_alpn_contains_proto(alpns, scs->alpn)) {
    CURL_TRC_CF(तथ्य, cf, "SSL session has different ALPN, no early data");
  }
  अन्यथा {
    infof(तथ्य, "SSL session allows %zu bytes of early data, "
          "reusing ALPN '%s'", connssl->earlydata_max, scs->alpn);
    connssl->earlydata_state = ssl_earlydata_await;
    connssl->state = ssl_connection_deferred;
    परिणाम = Curl_alpn_set_negotiated(cf, तथ्य, connssl,
                                      (स्थिर अचिन्हित अक्षर *)scs->alpn,
                                      scs->alpn ? म_माप(scs->alpn) : 0);
    *do_early_तथ्य = !परिणाम;
  }
  वापस परिणाम;
}

काष्ठा Curl_ssl_session *Curl_ssl_get_cf_session(काष्ठा Curl_easy *तथ्य,
                                                 स्थिर काष्ठा Curl_cftype *cft,
                                                 int8_t sockindex)
{
  अगर(तथ्य->conn &&
#अगर_अघोषित CURL_DISABLE_PROXY
     ((cft == &Curl_cft_ssl) || (cft == &Curl_cft_ssl_proxy))) {
#अन्यथा
     (cft == &Curl_cft_ssl)) {
#पूर्ण_अगर
    काष्ठा Curl_cfilter *cf1 = तथ्य->conn->cfilter[sockindex];
    क्रम(; cf1; cf1 = cf1->अग्र) {
      /* A tunneling proxy does not offer end2end encryption, even if
       * it does SSL itself (e.g. QUIC H3 proxy) */
      अगर(cf1->cft == cft)
        अवरोध;
    }
    अगर(cf1) {
      काष्ठा ssl_connect_तथ्य *connssl = cf1->ctx;
      अगर(connssl)
        वापस connssl->session;
    }
  }
  वापस शून्य;
}

#पूर्ण_अगर /* USE_SSL */
