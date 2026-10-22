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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_SSL

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cfilters.h"

#अन्तर्भाव "vtls/vtls.h" /* generic SSL protos etc */
#अन्तर्भाव "vtls/vtls_int.h"
#अन्तर्भाव "vtls/vtls_scache.h"
#अन्तर्भाव "vtls/vtls_spack.h"

#अन्तर्भाव "strcase.h"
#अन्तर्भाव "url.h"
#अन्तर्भाव "llist.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "curl_threads.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "curl_sha256.h"
#अन्तर्भाव "rand.h"
#अन्तर्भाव "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
संरचना Curl_ssl_scache_peer {
  वर्ण *ssl_peer_कुञ्चिका;      /* id for peer + relevant TLS configuration */
  वर्ण *clientcert;
  वर्ण *कुञ्चिका_passwd;
  संरचना Curl_llist sessions;
  शून्य *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  अचिह्नित वर्ण कुञ्चिका_salt[CURL_SHA256_DIGEST_दीर्घता]; /* for entry export */
  अचिह्नित वर्ण कुञ्चिका_hmac[CURL_SHA256_DIGEST_दीर्घता]; /* for entry export */
  size_t max_sessions;
  दीर्घ age;                /* a number, the higher the more recent */
  BIT(hmac_समुच्चयः);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#परिभाषय CURL_SCACHE_MAGIC 0x000e1551

#परिभाषय GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
स्थिर CURLcode cf_ssl_peer_कुञ्चिका_सङ्कलय_string(संरचना dynbuf *buf,
                                           ध्रुव वर्ण *name,
                                           ध्रुव वर्ण *मानम्)
{
  प्रत्यागम curlx_dyn_addf(buf, ":%s-%zu:%s", name, वाक्यदैर्घ्यम्(मानम्), मानम्);
}

स्थिर CURLcode cf_ssl_peer_कुञ्चिका_सङ्कलय_मार्गः(संरचना dynbuf *buf,
                                         ध्रुव वर्ण *name,
                                         ध्रुव वर्ण *मार्गः,
                                         bool *is_local)
{
  यदि(मार्गः && मार्गः[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#यदि_परिभाषितम् _WIN32
    वर्ण *abspath = _fullpath(रिक्त, मार्गः, 0);
    यदि(abspath) {
      CURLcode परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      मुक्त(abspath); /* allocated by CRT, use system free() */
      प्रत्यागम परिणाम;
    }
    *is_local = TRUE;
#नोचेद्यदि defined(HAVE_REALPATH)
    यदि(मार्गः[0] != '/') {
      वर्ण *abspath = realpath(मार्गः, रिक्त);
      यदि(abspath) {
        CURLcode परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        मुक्त(abspath); /* allocated by libc, free without memdebug */
        प्रत्यागम परिणाम;
      }
      *is_local = TRUE;
    }
#यदि_अन्तः
    प्रत्यागम cf_ssl_peer_कुञ्चिका_सङ्कलय_string(buf, name, मार्गः);
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode cf_ssl_peer_कुञ्चिका_सङ्कलय_hash(संरचना dynbuf *buf,
                                         ध्रुव वर्ण *name,
                                         संरचना curl_blob *blob)
{
  CURLcode परिणाम = CURLE_OK;
  यदि(blob && blob->len) {
    अचिह्नित वर्ण hash[CURL_SHA256_DIGEST_दीर्घता];
    size_t i;

    परिणाम = curlx_dyn_addf(buf, ":%s-", name);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    परिणाम = Curl_sha256it(hash, blob->दत्त, blob->len);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    प्रति(i = 0; i < CURL_SHA256_DIGEST_दीर्घता; ++i) {
      परिणाम = curlx_dyn_addf(buf, "%02x", hash[i]);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_ssl_peer_कुञ्चिका_सङ्कलय_mtls(संरचना dynbuf *buf,
                                         संरचना ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode परिणाम = CURLE_OK;
  यदि(ssl->clientcert && ssl->clientcert[0]) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_मार्गः(buf, "CCERT", ssl->clientcert, is_local);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->कुञ्चिका && ssl->कुञ्चिका[0]) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_मार्गः(buf, "KEY", ssl->कुञ्चिका, is_local);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->कुञ्चिका_blob) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_hash(buf, "KEYBlob", ssl->कुञ्चिका_blob);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    परिणाम = curlx_dyn_addf(buf, ":CT-%zu:", वाक्यदैर्घ्यम्(ssl->cert_type));
    प्रति(i = 0; !परिणाम && ssl->cert_type[i]; i++) {
      वर्ण c = Curl_raw_toupper(ssl->cert_type[i]);
      परिणाम = curlx_dyn_addn(buf, &c, 1);
    }
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->कुञ्चिका_type && ssl->कुञ्चिका_type[0]) {
    size_t i;
    परिणाम = curlx_dyn_addf(buf, ":KT-%zu:", वाक्यदैर्घ्यम्(ssl->कुञ्चिका_type));
    प्रति(i = 0; !परिणाम && ssl->कुञ्चिका_type[i]; i++) {
      वर्ण c = Curl_raw_toupper(ssl->कुञ्चिका_type[i]);
      परिणाम = curlx_dyn_addn(buf, &c, 1);
    }
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

#परिभाषय CURL_SSLS_LOCAL_SUFFIX     ":L"
#परिभाषय CURL_SSLS_GLOBAL_SUFFIX    ":G"

स्थिर bool cf_ssl_peer_कुञ्चिका_is_global(ध्रुव वर्ण *peer_कुञ्चिका)
{
  size_t len = peer_कुञ्चिका ? वाक्यदैर्घ्यम्(peer_कुञ्चिका) : 0;
  प्रत्यागम (len > 2) &&
         (peer_कुञ्चिका[len - 1] == 'G') &&
         (peer_कुञ्चिका[len - 2] == ':');
}

स्थिर CURLcode ssl_peer_कुञ्चिका_सङ्कलय_transport(संरचना dynbuf *buf,
                                           uint8_t transport)
{
  विकल्प(transport) {
  पक्ष TRNSPRT_TCP:
    प्रत्यागम CURLE_OK;
  पक्ष TRNSPRT_UDP:
    प्रत्यागम curlx_dyn_सङ्कलय(buf, ":UDP");
  पक्ष TRNSPRT_QUIC:
    प्रत्यागम curlx_dyn_सङ्कलय(buf, ":QUIC");
  पक्ष TRNSPRT_UNIX:
    प्रत्यागम curlx_dyn_सङ्कलय(buf, ":UNIX");
  स्वभाव:
    प्रत्यागम curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

स्थिर CURLcode ssl_peer_कुञ्चिका_सङ्कलय_vrfy(संरचना dynbuf *buf,
                                      संरचना ssl_filter_config *ssl,
                                      ध्रुव संरचना ssl_peer *peer)
{
  CURLcode परिणाम;

  यदि(!ssl->verifypeer) {
    परिणाम = curlx_dyn_सङ्कलय(buf, ":NO-VRFY-PEER");
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  यदि(!ssl->verifyhost) {
    परिणाम = curlx_dyn_सङ्कलय(buf, ":NO-VRFY-HOST");
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  यदि(ssl->verifystatus) {
    परिणाम = curlx_dyn_सङ्कलय(buf, ":VRFY-STATUS");
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  यदि((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_string(buf, "CHOST", peer->peer->hostname);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->पत्तनम्);
    यदि(परिणाम)
      प्रत्यागम परिणाम;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode ssl_peer_कुञ्चिका_build(संरचना ssl_filter_config *ssl,
                                   ध्रुव संरचना ssl_peer *peer,
                                   ध्रुव वर्ण *tls_id,
                                   वर्ण **ppeer_कुञ्चिका)
{
  संरचना dynbuf buf;
  size_t कुञ्चिका_len;
  bool is_local = FALSE;
  CURLcode परिणाम;

  *ppeer_कुञ्चिका = रिक्त;
  curlx_dyn_init(&buf, 10 * 1024);

  परिणाम = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->पत्तनम्);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  परिणाम = ssl_peer_कुञ्चिका_सङ्कलय_transport(&buf, peer->transport);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  परिणाम = ssl_peer_कुञ्चिका_सङ्कलय_vrfy(&buf, ssl, peer);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;
  यदि(ssl->संस्करणम्_2 || ssl->संस्करणम्_max) {
    परिणाम = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->संस्करणम्_2,
                            (ssl->संस्करणम्_max >> 16));
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->ssl_options) {
    परिणाम = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->cipher_list) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_string(&buf, "CIPHER", ssl->cipher_list);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->cipher_list13) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_string(&buf, "CIPHER13", ssl->cipher_list13);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->curves) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_string(&buf, "CURVES", ssl->curves);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->signature_algorithms) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->verifypeer) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_मार्गः(&buf, "CA", ssl->CAfile, &is_local);
    यदि(!परिणाम)
      परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_मार्गः(&buf, "CApath",
                                        ssl->CApath, &is_local);
    यदि(!परिणाम && ssl->native_ca_store)
      परिणाम = curlx_dyn_सङ्कलय(&buf, ":NATIVE-CA");
    यदि(!परिणाम)
      परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_मार्गः(&buf, "CRL", ssl->CRLfile, &is_local);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_मार्गः(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    यदि(ssl->ca_info_blob) {
      परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
    यदि(ssl->issuercert_blob) {
      परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
    }
  }
  यदि(ssl->cert_blob) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_hash(&buf, "CertBlob", ssl->cert_blob);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  यदि(ssl->pinned_कुञ्चिका && ssl->pinned_कुञ्चिका[0]) {
    परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_string(&buf, "Pinned", ssl->pinned_कुञ्चिका);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }

  परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_mtls(&buf, ssl, &is_local);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  यदि(!tls_id || !tls_id[0]) {
    परिणाम = CURLE_FAILED_INIT;
    गच्छ बहिर्गमनम्;
  }
  परिणाम = cf_ssl_peer_कुञ्चिका_सङ्कलय_string(&buf, "IMPL", tls_id);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  परिणाम = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  *ppeer_कुञ्चिका = curlx_dyn_take(&buf, &कुञ्चिका_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

बहिर्गमनम्:
  curlx_dyn_free(&buf);
  प्रत्यागम परिणाम;
}

CURLcode Curl_ssl_peer_कुञ्चिका_make(ध्रुव संरचना ssl_peer *peer,
                                संरचना ssl_filter_config *sslc,
                                ध्रुव वर्ण *tls_id,
                                वर्ण **ppeer_कुञ्चिका)
{
  प्रत्यागम ssl_peer_कुञ्चिका_build(sslc, peer, tls_id, ppeer_कुञ्चिका);
}

संरचना Curl_ssl_scache {
  अचिह्नित पूर्णाङ्क magic;
  संरचना Curl_ssl_scache_peer *peers;
  size_t peer_गणनम्;
  पूर्णाङ्क default_lifetime_secs;
  दीर्घ age;
#यदि_परिभाषितम् USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#यदि_अन्तः
  BIT(is_locked);
};

स्थिर संरचना Curl_ssl_scache *cf_ssl_scache_get(संरचना Curl_easy *दत्त)
{
  संरचना Curl_ssl_scache *scache = रिक्त;
  /* If a share is present, its ssl_scache has preference over the multi */
  यदि(दत्त->share && दत्त->share->ssl_scache)
    scache = दत्त->share->ssl_scache;
  अन्यथा यदि(दत्त->multi && दत्त->multi->ssl_scache)
    scache = दत्त->multi->ssl_scache;
  यदि(scache && !GOOD_SCACHE(scache)) {
    failf(दत्त, "transfer would use an invalid scache at %p, denied",
          (शून्य *)scache);
    DEBUGASSERT(0);
    प्रत्यागम रिक्त;
  }
  प्रत्यागम scache;
}

स्थिर शून्य cf_ssl_scache_session_ldestroy(शून्य *udata, शून्य *obj)
{
  संरचना Curl_ssl_session *s = obj;
  (शून्य)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(शून्य *sdata, size_t sdata_len,
                                 पूर्णाङ्क ietf_tls_id, ध्रुव वर्ण *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 संरचना Curl_ssl_session **psession)
{
  प्रत्यागम Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  रिक्त, 0, psession);
}

CURLcode Curl_ssl_session_create2(शून्य *sdata, size_t sdata_len,
                                  पूर्णाङ्क ietf_tls_id, ध्रुव वर्ण *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  अचिह्नित वर्ण *quic_tp, size_t quic_tp_len,
                                  संरचना Curl_ssl_session **psession)
{
  संरचना Curl_ssl_session *s;

  यदि(!sdata || !sdata_len) {
    curlx_free(sdata);
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  *psession = रिक्त;
  s = curlx_calloc(1, परिमाण(*s));
  यदि(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  यदि(alpn) {
    s->alpn = curlx_strdup(alpn);
    यदि(!s->alpn) {
      cf_ssl_scache_session_ldestroy(रिक्त, s);
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
  }
  *psession = s;
  प्रत्यागम CURLE_OK;
}

शून्य Curl_ssl_session_destroy(संरचना Curl_ssl_session *s)
{
  यदि(s) {
    /* if in the list, the list destructor takes care of it */
    यदि(Curl_ग्रन्थिः_llist(&s->list))
      Curl_ग्रन्थिः_remove(&s->list);
    अन्यथा {
      cf_ssl_scache_session_ldestroy(रिक्त, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(संरचना Curl_ssl_session *src,
                              संरचना Curl_ssl_session **pdest)
{
  संरचना Curl_ssl_session *dest = रिक्त;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  यदि(!src || !pdest)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  *pdest = रिक्त;

  dest = curlx_calloc(1, परिमाण(*dest));
  यदि(!dest)
    गच्छ बहिर्गमनम्;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  यदि(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    यदि(!dest->sdata)
      गच्छ बहिर्गमनम्;
    dest->sdata_len = src->sdata_len;
  }
  यदि(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    यदि(!dest->quic_tp)
      गच्छ बहिर्गमनम्;
    dest->quic_tp_len = src->quic_tp_len;
  }
  यदि(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    यदि(!dest->alpn)
      गच्छ बहिर्गमनम्;
  }
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(!परिणाम)
    *pdest = dest;
  अन्यथा {
    *pdest = रिक्त;
    यदि(dest)
      Curl_ssl_session_destroy(dest);
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_ssl_scache_clear_peer(संरचना Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, रिक्त);
  यदि(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    यदि(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = रिक्त;
  }
  peer->sobj_free = रिक्त;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->कुञ्चिका_passwd);
  curlx_safefree(peer->ssl_peer_कुञ्चिका);
  peer->age = 0;
  peer->hmac_समुच्चयः = FALSE;
}

स्थिर शून्य cf_ssl_scache_peer_समुच्चयः_obj(संरचना Curl_ssl_scache_peer *peer,
                                       शून्य *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  यदि(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

स्थिर शून्य cf_ssl_cache_peer_update(संरचना Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->कुञ्चिका_passwd &&
                      (!peer->ssl_peer_कुञ्चिका ||
                       cf_ssl_peer_कुञ्चिका_is_global(peer->ssl_peer_कुञ्चिका)));
}

स्थिर CURLcode cf_ssl_scache_peer_init(संरचना Curl_ssl_scache_peer *peer,
                                        ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                        ध्रुव वर्ण *clientcert,
                                        ध्रुव वर्ण *कुञ्चिका_passwd,
                                        ध्रुव अचिह्नित वर्ण *salt,
                                        ध्रुव अचिह्नित वर्ण *hmac)
{
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  DEBUGASSERT(!peer->ssl_peer_कुञ्चिका);
  यदि(ssl_peer_कुञ्चिका) {
    peer->ssl_peer_कुञ्चिका = curlx_strdup(ssl_peer_कुञ्चिका);
    यदि(!peer->ssl_peer_कुञ्चिका)
      गच्छ बहिर्गमनम्;
    peer->hmac_समुच्चयः = FALSE;
  }
  अन्यथा यदि(salt && hmac) {
    स्मृतिं_प्रतिलिख(peer->कुञ्चिका_salt, salt, परिमाण(peer->कुञ्चिका_salt));
    स्मृतिं_प्रतिलिख(peer->कुञ्चिका_hmac, hmac, परिमाण(peer->कुञ्चिका_hmac));
    peer->hmac_समुच्चयः = TRUE;
  }
  अन्यथा {
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }
  यदि(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    यदि(!peer->clientcert)
      गच्छ बहिर्गमनम्;
  }
  यदि(कुञ्चिका_passwd) {
    peer->कुञ्चिका_passwd = curlx_strdup(कुञ्चिका_passwd);
    यदि(!peer->कुञ्चिका_passwd)
      गच्छ बहिर्गमनम्;
  }
  cf_ssl_cache_peer_update(peer);
  परिणाम = CURLE_OK;
बहिर्गमनम्:
  यदि(परिणाम)
    cf_ssl_scache_clear_peer(peer);
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_scache_session_remove(संरचना Curl_ssl_scache_peer *peer,
                                     संरचना Curl_ssl_session *s)
{
  (शून्य)peer;
  DEBUGASSERT(Curl_ग्रन्थिः_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

स्थिर bool cf_scache_session_expired(संरचना Curl_ssl_session *s,
                                      curl_off_t now)
{
  प्रत्यागम (s->valid_until > 0) && (s->valid_until < now);
}

स्थिर शून्य cf_scache_peer_remove_expired(संरचना Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  संरचना Curl_llist_ग्रन्थिः *n = Curl_llist_शीर्ष(&peer->sessions);
  यावत्(n) {
    संरचना Curl_ssl_session *s = Curl_ग्रन्थिः_elem(n);
    n = Curl_ग्रन्थिः_अग्र(n);
    यदि(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

स्थिर शून्य cf_scache_peer_remove_non13(संरचना Curl_ssl_scache_peer *peer)
{
  संरचना Curl_llist_ग्रन्थिः *n = Curl_llist_शीर्ष(&peer->sessions);
  यावत्(n) {
    संरचना Curl_ssl_session *s = Curl_ग्रन्थिः_elem(n);
    n = Curl_ग्रन्थिः_अग्र(n);
    यदि(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_प्रत्येकम्_peer,
                                संरचना Curl_ssl_scache **pscache)
{
  संरचना Curl_ssl_scache *scache;
  संरचना Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = रिक्त;
  peers = curlx_calloc(max_peers, परिमाण(*peers));
  यदि(!peers)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  scache = curlx_calloc(1, परिमाण(*scache));
  यदि(!scache) {
    curlx_free(peers);
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_गणनम् = max_peers;
  scache->peers = peers;
  scache->age = 1;
  प्रति(i = 0; i < scache->peer_गणनम्; ++i) {
    scache->peers[i].max_sessions = max_sessions_प्रत्येकम्_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#यदि_परिभाषितम् USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#यदि_अन्तः
  *pscache = scache;
  प्रत्यागम CURLE_OK;
}

शून्य Curl_ssl_scache_destroy(संरचना Curl_ssl_scache *scache)
{
  यदि(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    प्रति(i = 0; i < scache->peer_गणनम्; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#यदि_परिभाषितम् USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#यदि_अन्तः
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(संरचना Curl_cfilter *cf, संरचना Curl_easy *दत्त)
{
  यदि(cf_ssl_scache_get(दत्त)) {
    संरचना ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    प्रत्यागम conn_config ? conn_config->cache_session : FALSE;
  }
  प्रत्यागम FALSE;
}

/* Lock shared SSL session data */
शून्य Curl_ssl_scache_तालकम्(संरचना Curl_easy *दत्त)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  यदि(scache) {
    यदि(CURL_SHARE_ssl_scache(दत्त))
      Curl_share_तालकम्(दत्त, CURL_तालकम्_दत्त_SSL_SESSION,
                      CURL_तालकम्_ACCESS_SINGLE);
#यदि_परिभाषितम् USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#अन्यथा
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#यदि_अन्तः
  }
}

/* Unlock shared SSL session data */
शून्य Curl_ssl_scache_unlock(संरचना Curl_easy *दत्त)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  यदि(scache) {
#यदि_परिभाषितम् USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#अन्यथा
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#यदि_अन्तः
    यदि(CURL_SHARE_ssl_scache(दत्त))
      Curl_share_unlock(दत्त, CURL_तालकम्_दत्त_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(संरचना Curl_easy *दत्त)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  bool locked = FALSE;
  यदि(!scache)
    प्रत्यागम FALSE;
#यदि_परिभाषितम् USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#अन्यथा
  locked = (bool)scache->is_locked;
#यदि_अन्तः
  प्रत्यागम locked;
}

स्थिर bool cf_ssl_scache_मेलनम्_auth(संरचना Curl_ssl_scache_peer *peer,
                                     संरचना ssl_filter_config *conn_config)
{
  यदि(!conn_config) {
    यदि(peer->clientcert || peer->कुञ्चिका_passwd)
      प्रत्यागम FALSE;
    प्रत्यागम TRUE;
  }
  अन्यथा यदि(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    प्रत्यागम FALSE;
  यदि(Curl_timestrcmp(peer->कुञ्चिका_passwd, conn_config->कुञ्चिका_passwd))
    प्रत्यागम FALSE;
  प्रत्यागम TRUE;
}

स्थिर CURLcode cf_ssl_find_peer_by_कुञ्चिका(संरचना Curl_easy *दत्त,
                                        संरचना Curl_ssl_scache *scache,
                                        ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                        संरचना ssl_filter_config *conn_config,
                                        संरचना Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_कुञ्चिका_len = 0;
  CURLcode परिणाम = CURLE_OK;

  *ppeer = रिक्त;
  यदि(!GOOD_SCACHE(scache)) {
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }

  CURL_TRC_SSLS(दत्त, "find peer slot for %s among %zu slots",
                ssl_peer_कुञ्चिका, scache->peer_गणनम्);

  /* check for entries with known peer_key */
  प्रति(i = 0; scache && i < scache->peer_गणनम्; i++) {
    यदि(scache->peers[i].ssl_peer_कुञ्चिका &&
       !वाक्यं_तुलय(ssl_peer_कुञ्चिका, scache->peers[i].ssl_peer_कुञ्चिका) &&
       cf_ssl_scache_मेलनम्_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      गच्छ बहिर्गमनम्;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  प्रति(i = 0; scache && i < scache->peer_गणनम्; i++) {
    यदि(!scache->peers[i].ssl_peer_कुञ्चिका &&
       scache->peers[i].hmac_समुच्चयः &&
       cf_ssl_scache_मेलनम्_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      अचिह्नित वर्ण my_hmac[CURL_SHA256_DIGEST_दीर्घता];
      यदि(!peer_कुञ्चिका_len) /* we are lazy */
        peer_कुञ्चिका_len = वाक्यदैर्घ्यम्(ssl_peer_कुञ्चिका);
      परिणाम = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].कुञ्चिका_salt,
                           परिमाण(scache->peers[i].कुञ्चिका_salt),
                           (ध्रुव अचिह्नित वर्ण *)ssl_peer_कुञ्चिका,
                           peer_कुञ्चिका_len,
                           my_hmac);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      यदि(!memcmp(scache->peers[i].कुञ्चिका_hmac, my_hmac, परिमाण(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(दत्त, "peer entry %zu key recovered: %s",
                      i, ssl_peer_कुञ्चिका);
        scache->peers[i].ssl_peer_कुञ्चिका = curlx_strdup(ssl_peer_कुञ्चिका);
        यदि(!scache->peers[i].ssl_peer_कुञ्चिका) {
          परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
          गच्छ बहिर्गमनम्;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        गच्छ बहिर्गमनम्;
      }
    }
  }
  CURL_TRC_SSLS(दत्त, "peer not found for %s", ssl_peer_कुञ्चिका);
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

स्थिर संरचना Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  संरचना Curl_ssl_scache *scache)
{
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  size_t i;

  /* find empty or oldest peer */
  प्रति(i = 0; i < scache->peer_गणनम्; ++i) {
    /* free peer entry? */
    यदि(!scache->peers[i].ssl_peer_कुञ्चिका && !scache->peers[i].hmac_समुच्चयः) {
      peer = &scache->peers[i];
      भङ्ग;
    }
    /* peer without sessions and obj */
    यदि(!scache->peers[i].sobj &&
       !Curl_llist_गणनम्(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      भङ्ग;
    }
    /* remember "oldest" peer */
    यदि(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  यदि(peer)
    cf_ssl_scache_clear_peer(peer);
  प्रत्यागम peer;
}

स्थिर CURLcode cf_ssl_सङ्कलय_peer(संरचना Curl_easy *दत्त,
                                संरचना Curl_ssl_scache *scache,
                                ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                संरचना ssl_filter_config *conn_config,
                                संरचना Curl_ssl_scache_peer **ppeer)
{
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode परिणाम = CURLE_OK;

  *ppeer = रिक्त;
  यदि(ssl_peer_कुञ्चिका) {
    परिणाम = cf_ssl_find_peer_by_कुञ्चिका(दत्त, scache, ssl_peer_कुञ्चिका, conn_config,
                                     &peer);
    यदि(परिणाम || !scache->peer_गणनम्)
      प्रत्यागम परिणाम;
  }

  यदि(peer) {
    *ppeer = peer;
    प्रत्यागम CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  यदि(peer) {
    वर्ण बफर[64];
    ध्रुव वर्ण *ccert = conn_config ? conn_config->clientcert : रिक्त;
    ध्रुव वर्ण *kpasswd = conn_config ? conn_config->कुञ्चिका_passwd : रिक्त;
    यदि(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(बफर, परिमाण(बफर),
                     "cert-%p", conn_config->cert_blob->दत्त);
      ccert = बफर; /* data is strduped by cf_ssl_scache_peer_init */
    }
    परिणाम = cf_ssl_scache_peer_init(peer, ssl_peer_कुञ्चिका, ccert, kpasswd,
                                     रिक्त, रिक्त);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    /* all ready */
    *ppeer = peer;
    परिणाम = CURLE_OK;
  }

बहिर्गमनम्:
  यदि(परिणाम) {
    cf_ssl_scache_clear_peer(peer);
  }
  प्रत्यागम परिणाम;
}

स्थिर शून्य cf_scache_peer_सङ्कलय_session(संरचना Curl_ssl_scache_peer *peer,
                                       संरचना Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  यदि(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, रिक्त);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  अन्यथा {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    यावत्(Curl_llist_गणनम्(&peer->sessions) > peer->max_sessions) {
      Curl_ग्रन्थिः_remove(Curl_llist_शीर्ष(&peer->sessions));
    }
  }
}

स्थिर CURLcode cf_scache_सङ्कलय_session(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *दत्त,
                                      संरचना Curl_ssl_scache *scache,
                                      ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                      संरचना Curl_ssl_session *s)
{
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
  curl_off_t now = (curl_off_t)time(रिक्त);
  curl_off_t max_lifetime;

  यदि(!scache || !scache->peer_गणनम्) {
    Curl_ssl_session_destroy(s);
    प्रत्यागम CURLE_OK;
  }

  यदि(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  यदि(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  यदि(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(दत्त, "add, session already expired");
    Curl_ssl_session_destroy(s);
    प्रत्यागम CURLE_OK;
  }

  परिणाम = cf_ssl_सङ्कलय_peer(दत्त, scache, ssl_peer_कुञ्चिका, conn_config, &peer);
  यदि(परिणाम || !peer) {
    CURL_TRC_SSLS(दत्त, "unable to add scache peer: %d", (पूर्णाङ्क)परिणाम);
    Curl_ssl_session_destroy(s);
    गच्छ बहिर्गमनम्;
  }

  cf_scache_peer_सङ्कलय_session(peer, s, now);

बहिर्गमनम्:
  यदि(परिणाम) {
    failf(दत्त, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_कुञ्चिका, (पूर्णाङ्क)परिणाम);
  }
  अन्यथा
    CURL_TRC_SSLS(दत्त, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_कुञ्चिका,
                  (अचिह्नित पूर्णाङ्क)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_गणनम्(&peer->sessions) : 0);
  प्रत्यागम परिणाम;
}

CURLcode Curl_ssl_scache_put(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *दत्त,
                             ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                             संरचना Curl_ssl_session *s)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम;
  DEBUGASSERT(conn_config);

  यदि(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    प्रत्यागम CURLE_OK;
  }

  Curl_ssl_scache_तालकम्(दत्त);
  परिणाम = cf_scache_सङ्कलय_session(cf, दत्त, scache, ssl_peer_कुञ्चिका, s);
  Curl_ssl_scache_unlock(दत्त);
  प्रत्यागम परिणाम;
}

शून्य Curl_ssl_scache_return(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त,
                            ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                            संरचना Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  यदि(s && s->ietf_tls_id < 0x304)
    (शून्य)Curl_ssl_scache_put(cf, दत्त, ssl_peer_कुञ्चिका, s);
  अन्यथा
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                              संरचना Curl_ssl_session **ps)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  संरचना Curl_llist_ग्रन्थिः *n;
  संरचना Curl_ssl_session *s = रिक्त;
  CURLcode परिणाम;

  *ps = रिक्त;
  यदि(!scache)
    प्रत्यागम CURLE_OK;

  Curl_ssl_scache_तालकम्(दत्त);
  परिणाम = cf_ssl_find_peer_by_कुञ्चिका(दत्त, scache, ssl_peer_कुञ्चिका, conn_config,
                                   &peer);
  यदि(!परिणाम && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(रिक्त));
    n = Curl_llist_शीर्ष(&peer->sessions);
    यदि(n) {
      s = Curl_ग्रन्थिः_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  यदि(s) {
    *ps = s;
    CURL_TRC_SSLS(दत्त, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_कुञ्चिका, (अचिह्नित पूर्णाङ्क)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_गणनम्(&peer->sessions));
  }
  अन्यथा {
    CURL_TRC_SSLS(दत्त, "no cached session for %s", ssl_peer_कुञ्चिका);
  }
  Curl_ssl_scache_unlock(दत्त);
  प्रत्यागम परिणाम;
}

CURLcode Curl_ssl_scache_सङ्कलय_obj(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 शून्य *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode परिणाम;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  यदि(!scache) {
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = cf_ssl_सङ्कलय_peer(दत्त, scache, ssl_peer_कुञ्चिका, conn_config, &peer);
  यदि(परिणाम || !peer) {
    CURL_TRC_SSLS(दत्त, "unable to add scache peer: %d", (पूर्णाङ्क)परिणाम);
    गच्छ बहिर्गमनम्;
  }

  cf_ssl_scache_peer_समुच्चयः_obj(peer, sobj, sobj_dtor_cb);
  sobj = रिक्त;  /* peer took ownership */

बहिर्गमनम्:
  यदि(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  प्रत्यागम परिणाम;
}

शून्य *Curl_ssl_scache_get_obj(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *ssl_peer_कुञ्चिका)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode परिणाम;
  शून्य *sobj;

  यदि(!scache)
    प्रत्यागम रिक्त;

  परिणाम = cf_ssl_find_peer_by_कुञ्चिका(दत्त, scache, ssl_peer_कुञ्चिका, conn_config,
                                   &peer);
  यदि(परिणाम)
    प्रत्यागम रिक्त;

  sobj = peer ? peer->sobj : रिक्त;

  CURL_TRC_SSLS(दत्त, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_कुञ्चिका);
  प्रत्यागम sobj;
}

शून्य Curl_ssl_scache_remove_all(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *दत्त,
                                ध्रुव वर्ण *ssl_peer_कुञ्चिका)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode परिणाम;

  (शून्य)cf;
  यदि(!scache)
    प्रत्यागम;

  Curl_ssl_scache_तालकम्(दत्त);
  परिणाम = cf_ssl_find_peer_by_कुञ्चिका(दत्त, scache, ssl_peer_कुञ्चिका, conn_config,
                                   &peer);
  यदि(!परिणाम && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(दत्त);
}

#यदि_परिभाषितम् USE_SSLS_EXPORT

#परिभाषय CURL_SSL_TICKET_MAX   (16 * 1024)

स्थिर CURLcode cf_ssl_scache_peer_समुच्चयः_hmac(संरचना Curl_ssl_scache_peer *peer)
{
  CURLcode परिणाम;

  DEBUGASSERT(peer);
  यदि(!peer->ssl_peer_कुञ्चिका)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  परिणाम = Curl_rand(रिक्त, peer->कुञ्चिका_salt, परिमाण(peer->कुञ्चिका_salt));
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  परिणाम = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->कुञ्चिका_salt, परिमाण(peer->कुञ्चिका_salt),
                       (ध्रुव अचिह्नित वर्ण *)peer->ssl_peer_कुञ्चिका,
                       वाक्यदैर्घ्यम्(peer->ssl_peer_कुञ्चिका),
                       peer->कुञ्चिका_hmac);
  यदि(!परिणाम)
    peer->hmac_समुच्चयः = TRUE;
  प्रत्यागम परिणाम;
}

स्थिर CURLcode cf_ssl_find_peer_by_hmac(संरचना Curl_ssl_scache *scache,
                                         ध्रुव अचिह्नित वर्ण *salt,
                                         ध्रुव अचिह्नित वर्ण *hmac,
                                         संरचना Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode परिणाम = CURLE_OK;

  *ppeer = रिक्त;
  यदि(!GOOD_SCACHE(scache))
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  प्रति(i = 0; scache && i < scache->peer_गणनम्; i++) {
    संरचना Curl_ssl_scache_peer *peer = &scache->peers[i];
    यदि(!cf_ssl_scache_मेलनम्_auth(peer, रिक्त))
      अनुवर्त;
    यदि(scache->peers[i].hmac_समुच्चयः &&
       !memcmp(peer->कुञ्चिका_salt, salt, परिमाण(peer->कुञ्चिका_salt)) &&
       !memcmp(peer->कुञ्चिका_hmac, hmac, परिमाण(peer->कुञ्चिका_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      गच्छ बहिर्गमनम्;
    }
    अन्यथा यदि(peer->ssl_peer_कुञ्चिका) {
      अचिह्नित वर्ण my_hmac[CURL_SHA256_DIGEST_दीर्घता];
      /* compute hmac for the passed salt */
      परिणाम = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, परिमाण(peer->कुञ्चिका_salt),
                           (ध्रुव अचिह्नित वर्ण *)peer->ssl_peer_कुञ्चिका,
                           वाक्यदैर्घ्यम्(peer->ssl_peer_कुञ्चिका),
                           my_hmac);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      यदि(!memcmp(my_hmac, hmac, परिमाण(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        यदि(!peer->hmac_समुच्चयः) {
          स्मृतिं_प्रतिलिख(peer->कुञ्चिका_salt, salt, परिमाण(peer->कुञ्चिका_salt));
          स्मृतिं_प्रतिलिख(peer->कुञ्चिका_hmac, hmac, परिमाण(peer->कुञ्चिका_hmac));
          peer->hmac_समुच्चयः = TRUE;
        }
        *ppeer = peer;
        गच्छ बहिर्गमनम्;
      }
    }
  }
बहिर्गमनम्:
  प्रत्यागम परिणाम;
}

CURLcode Curl_ssl_session_import(संरचना Curl_easy *दत्त,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 ध्रुव अचिह्नित वर्ण *shmac, size_t shmac_len,
                                 ध्रुव शून्य *sdata, size_t sdata_len)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  संरचना Curl_ssl_session *s = रिक्त;
  bool have_तालकम् = FALSE;
  CURLcode परिणाम;

  यदि(!scache || !sdata || !sdata_len) {
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }
  यदि(!ssl_peer_कुञ्चिका && (!shmac || !shmac_len)) {
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }

  परिणाम = Curl_ssl_session_unpack(दत्त, sdata, sdata_len, &s);
  यदि(परिणाम)
    गच्छ बहिर्गमनम्;

  Curl_ssl_scache_तालकम्(दत्त);
  have_तालकम् = TRUE;

  यदि(ssl_peer_कुञ्चिका) {
    परिणाम = cf_ssl_सङ्कलय_peer(दत्त, scache, ssl_peer_कुञ्चिका, रिक्त, &peer);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
  }
  अन्यथा यदि(shmac_len != (परिमाण(peer->कुञ्चिका_salt) + परिमाण(peer->कुञ्चिका_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    परिणाम = CURLE_BAD_FUNCTION_तर्कः;
    गच्छ बहिर्गमनम्;
  }
  अन्यथा {
    ध्रुव अचिह्नित वर्ण *salt = shmac;
    ध्रुव अचिह्नित वर्ण *hmac = shmac + परिमाण(peer->कुञ्चिका_salt);

    परिणाम = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    यदि(परिणाम)
      गच्छ बहिर्गमनम्;
    यदि(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      यदि(peer) {
        परिणाम = cf_ssl_scache_peer_init(peer, ssl_peer_कुञ्चिका, रिक्त, रिक्त,
                                         salt, hmac);
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
      }
    }
  }

  यदि(peer) {
    cf_scache_peer_सङ्कलय_session(peer, s, time(रिक्त));
    s = रिक्त; /* peer is now owner */
    CURL_TRC_SSLS(दत्त, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_कुञ्चिका ? peer->ssl_peer_कुञ्चिका : "without key",
                  Curl_llist_गणनम्(&peer->sessions));
  }

बहिर्गमनम्:
  यदि(scache && have_तालकम्)
    Curl_ssl_scache_unlock(दत्त);
  Curl_ssl_session_destroy(s);
  प्रत्यागम परिणाम;
}

CURLcode Curl_ssl_session_export(संरचना Curl_easy *दत्त,
                                 curl_ssls_export_cb *export_fn,
                                 शून्य *userptr)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(दत्त);
  संरचना Curl_ssl_scache_peer *peer;
  संरचना dynbuf sbuf, hbuf;
  संरचना Curl_llist_ग्रन्थिः *n;
  size_t i;
  curl_off_t now = time(रिक्त);
  CURLcode परिणाम = CURLE_OK;
#यदि_परिभाषितम् CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#यदि_अन्तः

  यदि(!export_fn)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;

  Curl_ssl_scache_तालकम्(दत्त);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_दीर्घता * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  प्रति(i = 0; scache && i < scache->peer_गणनम्; i++) {
    peer = &scache->peers[i];
    यदि(!peer->ssl_peer_कुञ्चिका && !peer->hmac_समुच्चयः)
      अनुवर्त; /* skip free entry */
    यदि(!peer->exportable)
      अनुवर्त;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_शीर्ष(&peer->sessions);
    यदि(n)
      VERBOSE(++npeers);
    यावत्(n) {
      संरचना Curl_ssl_session *s = Curl_ग्रन्थिः_elem(n);
      यदि(!peer->hmac_समुच्चयः) {
        परिणाम = cf_ssl_scache_peer_समुच्चयः_hmac(peer);
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
      }
      यदि(!curlx_dyn_len(&hbuf)) {
        परिणाम = curlx_dyn_addn(&hbuf, peer->कुञ्चिका_salt, परिमाण(peer->कुञ्चिका_salt));
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
        परिणाम = curlx_dyn_addn(&hbuf, peer->कुञ्चिका_hmac, परिमाण(peer->कुञ्चिका_hmac));
        यदि(परिणाम)
          गच्छ बहिर्गमनम्;
      }
      curlx_dyn_reset(&sbuf);
      परिणाम = Curl_ssl_session_pack(दत्त, s, &sbuf);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;

      परिणाम = export_fn(दत्त, userptr, peer->ssl_peer_कुञ्चिका,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      यदि(परिणाम)
        गच्छ बहिर्गमनम्;
      VERBOSE(++ntickets);
      n = Curl_ग्रन्थिः_अग्र(n);
    }
  }
  परिणाम = CURLE_OK;
  CURL_TRC_SSLS(दत्त, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

बहिर्गमनम्:
  Curl_ssl_scache_unlock(दत्त);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  प्रत्यागम परिणाम;
}

#यदि_अन्तः /* USE_SSLS_EXPORT */

#यदि_अन्तः /* USE_SSL */
