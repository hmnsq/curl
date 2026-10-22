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

#ifdef USE_SSL

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
  unsigned वर्ण कुञ्चिका_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned वर्ण कुञ्चिका_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
स्थिर CURLcode cf_ssl_peer_कुञ्चिका_add_string(संरचना dynbuf *buf,
                                           ध्रुव वर्ण *name,
                                           ध्रुव वर्ण *मानम्)
{
  पटिनिवत्त curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(मानम्), मानम्);
}

स्थिर CURLcode cf_ssl_peer_कुञ्चिका_add_path(संरचना dynbuf *buf,
                                         ध्रुव वर्ण *name,
                                         ध्रुव वर्ण *path,
                                         bool *is_local)
{
  सचे(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    वर्ण *abspath = _fullpath(रिक्त, path, 0);
    सचे(abspath) {
      CURLcode result = cf_ssl_peer_कुञ्चिका_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      मुक्त(abspath); /* allocated by CRT, use system free() */
      पटिनिवत्त result;
    }
    *is_local = TRUE;
#नोचेद्यदि defined(HAVE_REALPATH)
    सचे(path[0] != '/') {
      वर्ण *abspath = realpath(path, रिक्त);
      सचे(abspath) {
        CURLcode result = cf_ssl_peer_कुञ्चिका_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        मुक्त(abspath); /* allocated by libc, free without memdebug */
        पटिनिवत्त result;
      }
      *is_local = TRUE;
    }
#endif
    पटिनिवत्त cf_ssl_peer_कुञ्चिका_add_string(buf, name, path);
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode cf_ssl_peer_कुञ्चिका_add_hash(संरचना dynbuf *buf,
                                         ध्रुव वर्ण *name,
                                         संरचना curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  सचे(blob && blob->len) {
    unsigned वर्ण hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    सचे(result)
      गच्छ out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    सचे(result)
      गच्छ out;
    अत्थाय(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      सचे(result)
        गच्छ out;
    }
  }
out:
  पटिनिवत्त result;
}

स्थिर CURLcode cf_ssl_peer_कुञ्चिका_add_mtls(संरचना dynbuf *buf,
                                         संरचना ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  सचे(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_कुञ्चिका_add_path(buf, "CCERT", ssl->clientcert, is_local);
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->कुञ्चिका && ssl->कुञ्चिका[0]) {
    result = cf_ssl_peer_कुञ्चिका_add_path(buf, "KEY", ssl->कुञ्चिका, is_local);
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->कुञ्चिका_blob) {
    result = cf_ssl_peer_कुञ्चिका_add_hash(buf, "KEYBlob", ssl->कुञ्चिका_blob);
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    अत्थाय(i = 0; !result && ssl->cert_type[i]; i++) {
      वर्ण c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->कुञ्चिका_type && ssl->कुञ्चिका_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->कुञ्चिका_type));
    अत्थाय(i = 0; !result && ssl->कुञ्चिका_type[i]; i++) {
      वर्ण c = Curl_raw_toupper(ssl->कुञ्चिका_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  पटिनिवत्त result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

स्थिर bool cf_ssl_peer_कुञ्चिका_is_global(ध्रुव वर्ण *peer_कुञ्चिका)
{
  size_t len = peer_कुञ्चिका ? strlen(peer_कुञ्चिका) : 0;
  पटिनिवत्त (len > 2) &&
         (peer_कुञ्चिका[len - 1] == 'G') &&
         (peer_कुञ्चिका[len - 2] == ':');
}

स्थिर CURLcode ssl_peer_कुञ्चिका_add_transport(संरचना dynbuf *buf,
                                           uint8_t transport)
{
  विकल्प(transport) {
  पक्ष TRNSPRT_TCP:
    पटिनिवत्त CURLE_OK;
  पक्ष TRNSPRT_UDP:
    पटिनिवत्त curlx_dyn_add(buf, ":UDP");
  पक्ष TRNSPRT_QUIC:
    पटिनिवत्त curlx_dyn_add(buf, ":QUIC");
  पक्ष TRNSPRT_UNIX:
    पटिनिवत्त curlx_dyn_add(buf, ":UNIX");
  स्वभाव:
    पटिनिवत्त curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

स्थिर CURLcode ssl_peer_कुञ्चिका_add_vrfy(संरचना dynbuf *buf,
                                      संरचना ssl_filter_config *ssl,
                                      ध्रुव संरचना ssl_peer *peer)
{
  CURLcode result;

  सचे(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    सचे(result)
      पटिनिवत्त result;
  }
  सचे(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    सचे(result)
      पटिनिवत्त result;
  }
  सचे(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    सचे(result)
      पटिनिवत्त result;
  }
  सचे((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_कुञ्चिका_add_string(buf, "CHOST", peer->peer->hostname);
    सचे(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    सचे(result)
      पटिनिवत्त result;
  }
  पटिनिवत्त CURLE_OK;
}

स्थिर CURLcode ssl_peer_कुञ्चिका_build(संरचना ssl_filter_config *ssl,
                                   ध्रुव संरचना ssl_peer *peer,
                                   ध्रुव वर्ण *tls_id,
                                   वर्ण **ppeer_कुञ्चिका)
{
  संरचना dynbuf buf;
  size_t कुञ्चिका_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_कुञ्चिका = रिक्त;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  सचे(result)
    गच्छ out;
  result = ssl_peer_कुञ्चिका_add_transport(&buf, peer->transport);
  सचे(result)
    गच्छ out;
  result = ssl_peer_कुञ्चिका_add_vrfy(&buf, ssl, peer);
  सचे(result)
    गच्छ out;
  सचे(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->cipher_list) {
    result = cf_ssl_peer_कुञ्चिका_add_string(&buf, "CIPHER", ssl->cipher_list);
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->cipher_list13) {
    result = cf_ssl_peer_कुञ्चिका_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->curves) {
    result = cf_ssl_peer_कुञ्चिका_add_string(&buf, "CURVES", ssl->curves);
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->signature_algorithms) {
    result = cf_ssl_peer_कुञ्चिका_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->verifypeer) {
    result = cf_ssl_peer_कुञ्चिका_add_path(&buf, "CA", ssl->CAfile, &is_local);
    सचे(!result)
      result = cf_ssl_peer_कुञ्चिका_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    सचे(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    सचे(!result)
      result = cf_ssl_peer_कुञ्चिका_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    सचे(result)
      गच्छ out;
    result = cf_ssl_peer_कुञ्चिका_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    सचे(result)
      गच्छ out;
    सचे(ssl->ca_info_blob) {
      result = cf_ssl_peer_कुञ्चिका_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      सचे(result)
        गच्छ out;
    }
    सचे(ssl->issuercert_blob) {
      result = cf_ssl_peer_कुञ्चिका_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      सचे(result)
        गच्छ out;
    }
  }
  सचे(ssl->cert_blob) {
    result = cf_ssl_peer_कुञ्चिका_add_hash(&buf, "CertBlob", ssl->cert_blob);
    सचे(result)
      गच्छ out;
  }
  सचे(ssl->pinned_कुञ्चिका && ssl->pinned_कुञ्चिका[0]) {
    result = cf_ssl_peer_कुञ्चिका_add_string(&buf, "Pinned", ssl->pinned_कुञ्चिका);
    सचे(result)
      गच्छ out;
  }

  result = cf_ssl_peer_कुञ्चिका_add_mtls(&buf, ssl, &is_local);
  सचे(result)
    गच्छ out;

  सचे(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    गच्छ out;
  }
  result = cf_ssl_peer_कुञ्चिका_add_string(&buf, "IMPL", tls_id);
  सचे(result)
    गच्छ out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  सचे(result)
    गच्छ out;

  *ppeer_कुञ्चिका = curlx_dyn_take(&buf, &कुञ्चिका_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  पटिनिवत्त result;
}

CURLcode Curl_ssl_peer_कुञ्चिका_make(ध्रुव संरचना ssl_peer *peer,
                                संरचना ssl_filter_config *sslc,
                                ध्रुव वर्ण *tls_id,
                                वर्ण **ppeer_कुञ्चिका)
{
  पटिनिवत्त ssl_peer_कुञ्चिका_build(sslc, peer, tls_id, ppeer_कुञ्चिका);
}

संरचना Curl_ssl_scache {
  unsigned पूर्णाङ्क magic;
  संरचना Curl_ssl_scache_peer *peers;
  size_t peer_गणनम्;
  पूर्णाङ्क default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

स्थिर संरचना Curl_ssl_scache *cf_ssl_scache_get(संरचना Curl_easy *data)
{
  संरचना Curl_ssl_scache *scache = रिक्त;
  /* If a share is present, its ssl_scache has preference over the multi */
  सचे(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  अञ्ञथा सचे(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  सचे(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (शून्य *)scache);
    DEBUGASSERT(0);
    पटिनिवत्त रिक्त;
  }
  पटिनिवत्त scache;
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
  पटिनिवत्त Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  रिक्त, 0, psession);
}

CURLcode Curl_ssl_session_create2(शून्य *sdata, size_t sdata_len,
                                  पूर्णाङ्क ietf_tls_id, ध्रुव वर्ण *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned वर्ण *quic_tp, size_t quic_tp_len,
                                  संरचना Curl_ssl_session **psession)
{
  संरचना Curl_ssl_session *s;

  सचे(!sdata || !sdata_len) {
    curlx_free(sdata);
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = रिक्त;
  s = curlx_calloc(1, परिमाण(*s));
  सचे(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  सचे(alpn) {
    s->alpn = curlx_strdup(alpn);
    सचे(!s->alpn) {
      cf_ssl_scache_session_ldestroy(रिक्त, s);
      पटिनिवत्त CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  पटिनिवत्त CURLE_OK;
}

शून्य Curl_ssl_session_destroy(संरचना Curl_ssl_session *s)
{
  सचे(s) {
    /* if in the list, the list destructor takes care of it */
    सचे(Curl_ग्रन्थिः_llist(&s->list))
      Curl_ग्रन्थिः_remove(&s->list);
    अञ्ञथा {
      cf_ssl_scache_session_ldestroy(रिक्त, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(संरचना Curl_ssl_session *src,
                              संरचना Curl_ssl_session **pdest)
{
  संरचना Curl_ssl_session *dest = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  सचे(!src || !pdest)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = रिक्त;

  dest = curlx_calloc(1, परिमाण(*dest));
  सचे(!dest)
    गच्छ out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  सचे(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    सचे(!dest->sdata)
      गच्छ out;
    dest->sdata_len = src->sdata_len;
  }
  सचे(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    सचे(!dest->quic_tp)
      गच्छ out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  सचे(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    सचे(!dest->alpn)
      गच्छ out;
  }
  result = CURLE_OK;

out:
  सचे(!result)
    *pdest = dest;
  अञ्ञथा {
    *pdest = रिक्त;
    सचे(dest)
      Curl_ssl_session_destroy(dest);
  }
  पटिनिवत्त result;
}

स्थिर शून्य cf_ssl_scache_clear_peer(संरचना Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, रिक्त);
  सचे(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    सचे(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = रिक्त;
  }
  peer->sobj_free = रिक्त;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->कुञ्चिका_passwd);
  curlx_safefree(peer->ssl_peer_कुञ्चिका);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

स्थिर शून्य cf_ssl_scache_peer_set_obj(संरचना Curl_ssl_scache_peer *peer,
                                       शून्य *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  सचे(peer->sobj_free) {
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
                                        ध्रुव unsigned वर्ण *salt,
                                        ध्रुव unsigned वर्ण *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_कुञ्चिका);
  सचे(ssl_peer_कुञ्चिका) {
    peer->ssl_peer_कुञ्चिका = curlx_strdup(ssl_peer_कुञ्चिका);
    सचे(!peer->ssl_peer_कुञ्चिका)
      गच्छ out;
    peer->hmac_set = FALSE;
  }
  अञ्ञथा सचे(salt && hmac) {
    memcpy(peer->कुञ्चिका_salt, salt, परिमाण(peer->कुञ्चिका_salt));
    memcpy(peer->कुञ्चिका_hmac, hmac, परिमाण(peer->कुञ्चिका_hmac));
    peer->hmac_set = TRUE;
  }
  अञ्ञथा {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ out;
  }
  सचे(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    सचे(!peer->clientcert)
      गच्छ out;
  }
  सचे(कुञ्चिका_passwd) {
    peer->कुञ्चिका_passwd = curlx_strdup(कुञ्चिका_passwd);
    सचे(!peer->कुञ्चिका_passwd)
      गच्छ out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  सचे(result)
    cf_ssl_scache_clear_peer(peer);
  पटिनिवत्त result;
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
  पटिनिवत्त (s->valid_until > 0) && (s->valid_until < now);
}

स्थिर शून्य cf_scache_peer_remove_expired(संरचना Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  संरचना Curl_llist_ग्रन्थिः *n = Curl_llist_head(&peer->sessions);
  याव(n) {
    संरचना Curl_ssl_session *s = Curl_ग्रन्थिः_elem(n);
    n = Curl_ग्रन्थिः_next(n);
    सचे(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

स्थिर शून्य cf_scache_peer_remove_non13(संरचना Curl_ssl_scache_peer *peer)
{
  संरचना Curl_llist_ग्रन्थिः *n = Curl_llist_head(&peer->sessions);
  याव(n) {
    संरचना Curl_ssl_session *s = Curl_ग्रन्थिः_elem(n);
    n = Curl_ग्रन्थिः_next(n);
    सचे(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                संरचना Curl_ssl_scache **pscache)
{
  संरचना Curl_ssl_scache *scache;
  संरचना Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = रिक्त;
  peers = curlx_calloc(max_peers, परिमाण(*peers));
  सचे(!peers)
    पटिनिवत्त CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, परिमाण(*scache));
  सचे(!scache) {
    curlx_free(peers);
    पटिनिवत्त CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_गणनम् = max_peers;
  scache->peers = peers;
  scache->age = 1;
  अत्थाय(i = 0; i < scache->peer_गणनम्; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  पटिनिवत्त CURLE_OK;
}

शून्य Curl_ssl_scache_destroy(संरचना Curl_ssl_scache *scache)
{
  सचे(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    अत्थाय(i = 0; i < scache->peer_गणनम्; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(संरचना Curl_cfilter *cf, संरचना Curl_easy *data)
{
  सचे(cf_ssl_scache_get(data)) {
    संरचना ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    पटिनिवत्त conn_config ? conn_config->cache_session : FALSE;
  }
  पटिनिवत्त FALSE;
}

/* Lock shared SSL session data */
शून्य Curl_ssl_scache_lock(संरचना Curl_easy *data)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  सचे(scache) {
    सचे(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#अञ्ञथा
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
शून्य Curl_ssl_scache_unlock(संरचना Curl_easy *data)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  सचे(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#अञ्ञथा
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    सचे(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(संरचना Curl_easy *data)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  सचे(!scache)
    पटिनिवत्त FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#अञ्ञथा
  locked = (bool)scache->is_locked;
#endif
  पटिनिवत्त locked;
}

स्थिर bool cf_ssl_scache_match_auth(संरचना Curl_ssl_scache_peer *peer,
                                     संरचना ssl_filter_config *conn_config)
{
  सचे(!conn_config) {
    सचे(peer->clientcert || peer->कुञ्चिका_passwd)
      पटिनिवत्त FALSE;
    पटिनिवत्त TRUE;
  }
  अञ्ञथा सचे(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    पटिनिवत्त FALSE;
  सचे(Curl_timestrcmp(peer->कुञ्चिका_passwd, conn_config->कुञ्चिका_passwd))
    पटिनिवत्त FALSE;
  पटिनिवत्त TRUE;
}

स्थिर CURLcode cf_ssl_find_peer_by_कुञ्चिका(संरचना Curl_easy *data,
                                        संरचना Curl_ssl_scache *scache,
                                        ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                        संरचना ssl_filter_config *conn_config,
                                        संरचना Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_कुञ्चिका_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = रिक्त;
  सचे(!GOOD_SCACHE(scache)) {
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_कुञ्चिका, scache->peer_गणनम्);

  /* check for entries with known peer_key */
  अत्थाय(i = 0; scache && i < scache->peer_गणनम्; i++) {
    सचे(scache->peers[i].ssl_peer_कुञ्चिका &&
       !strcmp(ssl_peer_कुञ्चिका, scache->peers[i].ssl_peer_कुञ्चिका) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      गच्छ out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  अत्थाय(i = 0; scache && i < scache->peer_गणनम्; i++) {
    सचे(!scache->peers[i].ssl_peer_कुञ्चिका &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned वर्ण my_hmac[CURL_SHA256_DIGEST_LENGTH];
      सचे(!peer_कुञ्चिका_len) /* we are lazy */
        peer_कुञ्चिका_len = strlen(ssl_peer_कुञ्चिका);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].कुञ्चिका_salt,
                           परिमाण(scache->peers[i].कुञ्चिका_salt),
                           (ध्रुव unsigned वर्ण *)ssl_peer_कुञ्चिका,
                           peer_कुञ्चिका_len,
                           my_hmac);
      सचे(result)
        गच्छ out;
      सचे(!memcmp(scache->peers[i].कुञ्चिका_hmac, my_hmac, परिमाण(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_कुञ्चिका);
        scache->peers[i].ssl_peer_कुञ्चिका = curlx_strdup(ssl_peer_कुञ्चिका);
        सचे(!scache->peers[i].ssl_peer_कुञ्चिका) {
          result = CURLE_OUT_OF_MEMORY;
          गच्छ out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        गच्छ out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_कुञ्चिका);
out:
  पटिनिवत्त result;
}

स्थिर संरचना Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  संरचना Curl_ssl_scache *scache)
{
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  size_t i;

  /* find empty or oldest peer */
  अत्थाय(i = 0; i < scache->peer_गणनम्; ++i) {
    /* free peer entry? */
    सचे(!scache->peers[i].ssl_peer_कुञ्चिका && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      भङ्ग;
    }
    /* peer without sessions and obj */
    सचे(!scache->peers[i].sobj &&
       !Curl_llist_गणनम्(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      भङ्ग;
    }
    /* remember "oldest" peer */
    सचे(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  सचे(peer)
    cf_ssl_scache_clear_peer(peer);
  पटिनिवत्त peer;
}

स्थिर CURLcode cf_ssl_add_peer(संरचना Curl_easy *data,
                                संरचना Curl_ssl_scache *scache,
                                ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                संरचना ssl_filter_config *conn_config,
                                संरचना Curl_ssl_scache_peer **ppeer)
{
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode result = CURLE_OK;

  *ppeer = रिक्त;
  सचे(ssl_peer_कुञ्चिका) {
    result = cf_ssl_find_peer_by_कुञ्चिका(data, scache, ssl_peer_कुञ्चिका, conn_config,
                                     &peer);
    सचे(result || !scache->peer_गणनम्)
      पटिनिवत्त result;
  }

  सचे(peer) {
    *ppeer = peer;
    पटिनिवत्त CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  सचे(peer) {
    वर्ण buffer[64];
    ध्रुव वर्ण *ccert = conn_config ? conn_config->clientcert : रिक्त;
    ध्रुव वर्ण *kpasswd = conn_config ? conn_config->कुञ्चिका_passwd : रिक्त;
    सचे(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, परिमाण(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_कुञ्चिका, ccert, kpasswd,
                                     रिक्त, रिक्त);
    सचे(result)
      गच्छ out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  सचे(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  पटिनिवत्त result;
}

स्थिर शून्य cf_scache_peer_add_session(संरचना Curl_ssl_scache_peer *peer,
                                       संरचना Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  सचे(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, रिक्त);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  अञ्ञथा {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    याव(Curl_llist_गणनम्(&peer->sessions) > peer->max_sessions) {
      Curl_ग्रन्थिः_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

स्थिर CURLcode cf_scache_add_session(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      संरचना Curl_ssl_scache *scache,
                                      ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                      संरचना Curl_ssl_session *s)
{
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(रिक्त);
  curl_off_t max_lifetime;

  सचे(!scache || !scache->peer_गणनम्) {
    Curl_ssl_session_destroy(s);
    पटिनिवत्त CURLE_OK;
  }

  सचे(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  सचे(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  सचे(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    पटिनिवत्त CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_कुञ्चिका, conn_config, &peer);
  सचे(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (पूर्णाङ्क)result);
    Curl_ssl_session_destroy(s);
    गच्छ out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  सचे(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_कुञ्चिका, (पूर्णाङ्क)result);
  }
  अञ्ञथा
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_कुञ्चिका,
                  (unsigned पूर्णाङ्क)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_गणनम्(&peer->sessions) : 0);
  पटिनिवत्त result;
}

CURLcode Curl_ssl_scache_put(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                             संरचना Curl_ssl_session *s)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  सचे(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    पटिनिवत्त CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_कुञ्चिका, s);
  Curl_ssl_scache_unlock(data);
  पटिनिवत्त result;
}

शून्य Curl_ssl_scache_return(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                            संरचना Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  सचे(s && s->ietf_tls_id < 0x304)
    (शून्य)Curl_ssl_scache_put(cf, data, ssl_peer_कुञ्चिका, s);
  अञ्ञथा
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                              संरचना Curl_ssl_session **ps)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  संरचना Curl_llist_ग्रन्थिः *n;
  संरचना Curl_ssl_session *s = रिक्त;
  CURLcode result;

  *ps = रिक्त;
  सचे(!scache)
    पटिनिवत्त CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_कुञ्चिका(data, scache, ssl_peer_कुञ्चिका, conn_config,
                                   &peer);
  सचे(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(रिक्त));
    n = Curl_llist_head(&peer->sessions);
    सचे(n) {
      s = Curl_ग्रन्थिः_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  सचे(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_कुञ्चिका, (unsigned पूर्णाङ्क)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_गणनम्(&peer->sessions));
  }
  अञ्ञथा {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_कुञ्चिका);
  }
  Curl_ssl_scache_unlock(data);
  पटिनिवत्त result;
}

CURLcode Curl_ssl_scache_add_obj(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 शून्य *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  सचे(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_कुञ्चिका, conn_config, &peer);
  सचे(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (पूर्णाङ्क)result);
    गच्छ out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = रिक्त;  /* peer took ownership */

out:
  सचे(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  पटिनिवत्त result;
}

शून्य *Curl_ssl_scache_get_obj(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              ध्रुव वर्ण *ssl_peer_कुञ्चिका)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode result;
  शून्य *sobj;

  सचे(!scache)
    पटिनिवत्त रिक्त;

  result = cf_ssl_find_peer_by_कुञ्चिका(data, scache, ssl_peer_कुञ्चिका, conn_config,
                                   &peer);
  सचे(result)
    पटिनिवत्त रिक्त;

  sobj = peer ? peer->sobj : रिक्त;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_कुञ्चिका);
  पटिनिवत्त sobj;
}

शून्य Curl_ssl_scache_remove_all(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                ध्रुव वर्ण *ssl_peer_कुञ्चिका)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode result;

  (शून्य)cf;
  सचे(!scache)
    पटिनिवत्त;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_कुञ्चिका(data, scache, ssl_peer_कुञ्चिका, conn_config,
                                   &peer);
  सचे(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

स्थिर CURLcode cf_ssl_scache_peer_set_hmac(संरचना Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  सचे(!peer->ssl_peer_कुञ्चिका)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(रिक्त, peer->कुञ्चिका_salt, परिमाण(peer->कुञ्चिका_salt));
  सचे(result)
    पटिनिवत्त result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->कुञ्चिका_salt, परिमाण(peer->कुञ्चिका_salt),
                       (ध्रुव unsigned वर्ण *)peer->ssl_peer_कुञ्चिका,
                       strlen(peer->ssl_peer_कुञ्चिका),
                       peer->कुञ्चिका_hmac);
  सचे(!result)
    peer->hmac_set = TRUE;
  पटिनिवत्त result;
}

स्थिर CURLcode cf_ssl_find_peer_by_hmac(संरचना Curl_ssl_scache *scache,
                                         ध्रुव unsigned वर्ण *salt,
                                         ध्रुव unsigned वर्ण *hmac,
                                         संरचना Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = रिक्त;
  सचे(!GOOD_SCACHE(scache))
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  अत्थाय(i = 0; scache && i < scache->peer_गणनम्; i++) {
    संरचना Curl_ssl_scache_peer *peer = &scache->peers[i];
    सचे(!cf_ssl_scache_match_auth(peer, रिक्त))
      अनुवर्त;
    सचे(scache->peers[i].hmac_set &&
       !memcmp(peer->कुञ्चिका_salt, salt, परिमाण(peer->कुञ्चिका_salt)) &&
       !memcmp(peer->कुञ्चिका_hmac, hmac, परिमाण(peer->कुञ्चिका_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      गच्छ out;
    }
    अञ्ञथा सचे(peer->ssl_peer_कुञ्चिका) {
      unsigned वर्ण my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, परिमाण(peer->कुञ्चिका_salt),
                           (ध्रुव unsigned वर्ण *)peer->ssl_peer_कुञ्चिका,
                           strlen(peer->ssl_peer_कुञ्चिका),
                           my_hmac);
      सचे(result)
        गच्छ out;
      सचे(!memcmp(my_hmac, hmac, परिमाण(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        सचे(!peer->hmac_set) {
          memcpy(peer->कुञ्चिका_salt, salt, परिमाण(peer->कुञ्चिका_salt));
          memcpy(peer->कुञ्चिका_hmac, hmac, परिमाण(peer->कुञ्चिका_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        गच्छ out;
      }
    }
  }
out:
  पटिनिवत्त result;
}

CURLcode Curl_ssl_session_import(संरचना Curl_easy *data,
                                 ध्रुव वर्ण *ssl_peer_कुञ्चिका,
                                 ध्रुव unsigned वर्ण *shmac, size_t shmac_len,
                                 ध्रुव शून्य *sdata, size_t sdata_len)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  संरचना Curl_ssl_session *s = रिक्त;
  bool have_lock = FALSE;
  CURLcode result;

  सचे(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ out;
  }
  सचे(!ssl_peer_कुञ्चिका && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  सचे(result)
    गच्छ out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  सचे(ssl_peer_कुञ्चिका) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_कुञ्चिका, रिक्त, &peer);
    सचे(result)
      गच्छ out;
  }
  अञ्ञथा सचे(shmac_len != (परिमाण(peer->कुञ्चिका_salt) + परिमाण(peer->कुञ्चिका_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    गच्छ out;
  }
  अञ्ञथा {
    ध्रुव unsigned वर्ण *salt = shmac;
    ध्रुव unsigned वर्ण *hmac = shmac + परिमाण(peer->कुञ्चिका_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    सचे(result)
      गच्छ out;
    सचे(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      सचे(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_कुञ्चिका, रिक्त, रिक्त,
                                         salt, hmac);
        सचे(result)
          गच्छ out;
      }
    }
  }

  सचे(peer) {
    cf_scache_peer_add_session(peer, s, time(रिक्त));
    s = रिक्त; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_कुञ्चिका ? peer->ssl_peer_कुञ्चिका : "without key",
                  Curl_llist_गणनम्(&peer->sessions));
  }

out:
  सचे(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  पटिनिवत्त result;
}

CURLcode Curl_ssl_session_export(संरचना Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 शून्य *userptr)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना Curl_ssl_scache_peer *peer;
  संरचना dynbuf sbuf, hbuf;
  संरचना Curl_llist_ग्रन्थिः *n;
  size_t i;
  curl_off_t now = time(रिक्त);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  सचे(!export_fn)
    पटिनिवत्त CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  अत्थाय(i = 0; scache && i < scache->peer_गणनम्; i++) {
    peer = &scache->peers[i];
    सचे(!peer->ssl_peer_कुञ्चिका && !peer->hmac_set)
      अनुवर्त; /* skip free entry */
    सचे(!peer->exportable)
      अनुवर्त;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    सचे(n)
      VERBOSE(++npeers);
    याव(n) {
      संरचना Curl_ssl_session *s = Curl_ग्रन्थिः_elem(n);
      सचे(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        सचे(result)
          गच्छ out;
      }
      सचे(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->कुञ्चिका_salt, परिमाण(peer->कुञ्चिका_salt));
        सचे(result)
          गच्छ out;
        result = curlx_dyn_addn(&hbuf, peer->कुञ्चिका_hmac, परिमाण(peer->कुञ्चिका_hmac));
        सचे(result)
          गच्छ out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      सचे(result)
        गच्छ out;

      result = export_fn(data, userptr, peer->ssl_peer_कुञ्चिका,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      सचे(result)
        गच्छ out;
      VERBOSE(++ntickets);
      n = Curl_ग्रन्थिः_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  पटिनिवत्त result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
