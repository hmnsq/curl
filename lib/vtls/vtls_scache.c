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
  अक्षर *ssl_peer_किल्ली;      /* id for peer + relevant TLS configuration */
  अक्षर *clientcert;
  अक्षर *किल्ली_passwd;
  संरचना Curl_llist sessions;
  शून्य *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  अचिन्हित अक्षर किल्ली_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  अचिन्हित अक्षर किल्ली_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  माप_प्रकार max_sessions;
  दीर्घ age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
स्थिर CURLcode cf_ssl_peer_किल्ली_add_string(संरचना dynbuf *buf,
                                           स्थिरांक अक्षर *name,
                                           स्थिरांक अक्षर *मूल्य)
{
  परत curlx_dyn_addf(buf, ":%s-%zu:%s", name, म_माप(मूल्य), मूल्य);
}

स्थिर CURLcode cf_ssl_peer_किल्ली_add_path(संरचना dynbuf *buf,
                                         स्थिरांक अक्षर *name,
                                         स्थिरांक अक्षर *path,
                                         bool *is_local)
{
  जर(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    अक्षर *abspath = _fullpath(रिक्त, path, 0);
    जर(abspath) {
      CURLcode result = cf_ssl_peer_किल्ली_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      मुक्त(abspath); /* allocated by CRT, use system free() */
      परत result;
    }
    *is_local = TRUE;
#किंवाजर defined(HAVE_REALPATH)
    जर(path[0] != '/') {
      अक्षर *abspath = realpath(path, रिक्त);
      जर(abspath) {
        CURLcode result = cf_ssl_peer_किल्ली_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        मुक्त(abspath); /* allocated by libc, free without memdebug */
        परत result;
      }
      *is_local = TRUE;
    }
#endif
    परत cf_ssl_peer_किल्ली_add_string(buf, name, path);
  }
  परत CURLE_OK;
}

स्थिर CURLcode cf_ssl_peer_किल्ली_add_hash(संरचना dynbuf *buf,
                                         स्थिरांक अक्षर *name,
                                         संरचना curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  जर(blob && blob->len) {
    अचिन्हित अक्षर hash[CURL_SHA256_DIGEST_LENGTH];
    माप_प्रकार i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    जर(result)
      जा out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    जर(result)
      जा out;
    साठी(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      जर(result)
        जा out;
    }
  }
out:
  परत result;
}

स्थिर CURLcode cf_ssl_peer_किल्ली_add_mtls(संरचना dynbuf *buf,
                                         संरचना ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  जर(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_किल्ली_add_path(buf, "CCERT", ssl->clientcert, is_local);
    जर(result)
      जा out;
  }
  जर(ssl->किल्ली && ssl->किल्ली[0]) {
    result = cf_ssl_peer_किल्ली_add_path(buf, "KEY", ssl->किल्ली, is_local);
    जर(result)
      जा out;
  }
  जर(ssl->किल्ली_blob) {
    result = cf_ssl_peer_किल्ली_add_hash(buf, "KEYBlob", ssl->किल्ली_blob);
    जर(result)
      जा out;
  }
  जर(ssl->cert_type && ssl->cert_type[0]) {
    माप_प्रकार i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", म_माप(ssl->cert_type));
    साठी(i = 0; !result && ssl->cert_type[i]; i++) {
      अक्षर c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    जर(result)
      जा out;
  }
  जर(ssl->किल्ली_type && ssl->किल्ली_type[0]) {
    माप_प्रकार i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", म_माप(ssl->किल्ली_type));
    साठी(i = 0; !result && ssl->किल्ली_type[i]; i++) {
      अक्षर c = Curl_raw_toupper(ssl->किल्ली_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  परत result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

स्थिर bool cf_ssl_peer_किल्ली_is_global(स्थिरांक अक्षर *peer_किल्ली)
{
  माप_प्रकार len = peer_किल्ली ? म_माप(peer_किल्ली) : 0;
  परत (len > 2) &&
         (peer_किल्ली[len - 1] == 'G') &&
         (peer_किल्ली[len - 2] == ':');
}

स्थिर CURLcode ssl_peer_किल्ली_add_transport(संरचना dynbuf *buf,
                                           uint8_t transport)
{
  निवड(transport) {
  स्थिती TRNSPRT_TCP:
    परत CURLE_OK;
  स्थिती TRNSPRT_UDP:
    परत curlx_dyn_add(buf, ":UDP");
  स्थिती TRNSPRT_QUIC:
    परत curlx_dyn_add(buf, ":QUIC");
  स्थिती TRNSPRT_UNIX:
    परत curlx_dyn_add(buf, ":UNIX");
  पूर्वनिर्धारित:
    परत curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

स्थिर CURLcode ssl_peer_किल्ली_add_vrfy(संरचना dynbuf *buf,
                                      संरचना ssl_filter_config *ssl,
                                      स्थिरांक संरचना ssl_peer *peer)
{
  CURLcode result;

  जर(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    जर(result)
      परत result;
  }
  जर(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    जर(result)
      परत result;
  }
  जर(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    जर(result)
      परत result;
  }
  जर((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_किल्ली_add_string(buf, "CHOST", peer->peer->hostname);
    जर(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    जर(result)
      परत result;
  }
  परत CURLE_OK;
}

स्थिर CURLcode ssl_peer_किल्ली_build(संरचना ssl_filter_config *ssl,
                                   स्थिरांक संरचना ssl_peer *peer,
                                   स्थिरांक अक्षर *tls_id,
                                   अक्षर **ppeer_किल्ली)
{
  संरचना dynbuf buf;
  माप_प्रकार किल्ली_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_किल्ली = रिक्त;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  जर(result)
    जा out;
  result = ssl_peer_किल्ली_add_transport(&buf, peer->transport);
  जर(result)
    जा out;
  result = ssl_peer_किल्ली_add_vrfy(&buf, ssl, peer);
  जर(result)
    जा out;
  जर(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    जर(result)
      जा out;
  }
  जर(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    जर(result)
      जा out;
  }
  जर(ssl->cipher_list) {
    result = cf_ssl_peer_किल्ली_add_string(&buf, "CIPHER", ssl->cipher_list);
    जर(result)
      जा out;
  }
  जर(ssl->cipher_list13) {
    result = cf_ssl_peer_किल्ली_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    जर(result)
      जा out;
  }
  जर(ssl->curves) {
    result = cf_ssl_peer_किल्ली_add_string(&buf, "CURVES", ssl->curves);
    जर(result)
      जा out;
  }
  जर(ssl->signature_algorithms) {
    result = cf_ssl_peer_किल्ली_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    जर(result)
      जा out;
  }
  जर(ssl->verifypeer) {
    result = cf_ssl_peer_किल्ली_add_path(&buf, "CA", ssl->CAfile, &is_local);
    जर(!result)
      result = cf_ssl_peer_किल्ली_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    जर(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    जर(!result)
      result = cf_ssl_peer_किल्ली_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    जर(result)
      जा out;
    result = cf_ssl_peer_किल्ली_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    जर(result)
      जा out;
    जर(ssl->ca_info_blob) {
      result = cf_ssl_peer_किल्ली_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      जर(result)
        जा out;
    }
    जर(ssl->issuercert_blob) {
      result = cf_ssl_peer_किल्ली_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      जर(result)
        जा out;
    }
  }
  जर(ssl->cert_blob) {
    result = cf_ssl_peer_किल्ली_add_hash(&buf, "CertBlob", ssl->cert_blob);
    जर(result)
      जा out;
  }
  जर(ssl->pinned_किल्ली && ssl->pinned_किल्ली[0]) {
    result = cf_ssl_peer_किल्ली_add_string(&buf, "Pinned", ssl->pinned_किल्ली);
    जर(result)
      जा out;
  }

  result = cf_ssl_peer_किल्ली_add_mtls(&buf, ssl, &is_local);
  जर(result)
    जा out;

  जर(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    जा out;
  }
  result = cf_ssl_peer_किल्ली_add_string(&buf, "IMPL", tls_id);
  जर(result)
    जा out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  जर(result)
    जा out;

  *ppeer_किल्ली = curlx_dyn_take(&buf, &किल्ली_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  परत result;
}

CURLcode Curl_ssl_peer_किल्ली_make(स्थिरांक संरचना ssl_peer *peer,
                                संरचना ssl_filter_config *sslc,
                                स्थिरांक अक्षर *tls_id,
                                अक्षर **ppeer_किल्ली)
{
  परत ssl_peer_किल्ली_build(sslc, peer, tls_id, ppeer_किल्ली);
}

संरचना Curl_ssl_scache {
  अचिन्हित पूर्णांक magic;
  संरचना Curl_ssl_scache_peer *peers;
  माप_प्रकार peer_मोजणी;
  पूर्णांक default_lifetime_secs;
  दीर्घ age;
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
  जर(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  अन्यथा जर(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  जर(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (शून्य *)scache);
    DEBUGASSERT(0);
    परत रिक्त;
  }
  परत scache;
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

CURLcode Curl_ssl_session_create(शून्य *sdata, माप_प्रकार sdata_len,
                                 पूर्णांक ietf_tls_id, स्थिरांक अक्षर *alpn,
                                 curl_off_t valid_until, माप_प्रकार earlydata_max,
                                 संरचना Curl_ssl_session **psession)
{
  परत Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  रिक्त, 0, psession);
}

CURLcode Curl_ssl_session_create2(शून्य *sdata, माप_प्रकार sdata_len,
                                  पूर्णांक ietf_tls_id, स्थिरांक अक्षर *alpn,
                                  curl_off_t valid_until, माप_प्रकार earlydata_max,
                                  अचिन्हित अक्षर *quic_tp, माप_प्रकार quic_tp_len,
                                  संरचना Curl_ssl_session **psession)
{
  संरचना Curl_ssl_session *s;

  जर(!sdata || !sdata_len) {
    curlx_free(sdata);
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = रिक्त;
  s = curlx_calloc(1, आकार(*s));
  जर(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    परत CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  जर(alpn) {
    s->alpn = curlx_strdup(alpn);
    जर(!s->alpn) {
      cf_ssl_scache_session_ldestroy(रिक्त, s);
      परत CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  परत CURLE_OK;
}

शून्य Curl_ssl_session_destroy(संरचना Curl_ssl_session *s)
{
  जर(s) {
    /* if in the list, the list destructor takes care of it */
    जर(Curl_गाठ_llist(&s->list))
      Curl_गाठ_remove(&s->list);
    अन्यथा {
      cf_ssl_scache_session_ldestroy(रिक्त, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(संरचना Curl_ssl_session *src,
                              संरचना Curl_ssl_session **pdest)
{
  संरचना Curl_ssl_session *dest = रिक्त;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  जर(!src || !pdest)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = रिक्त;

  dest = curlx_calloc(1, आकार(*dest));
  जर(!dest)
    जा out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  जर(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    जर(!dest->sdata)
      जा out;
    dest->sdata_len = src->sdata_len;
  }
  जर(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    जर(!dest->quic_tp)
      जा out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  जर(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    जर(!dest->alpn)
      जा out;
  }
  result = CURLE_OK;

out:
  जर(!result)
    *pdest = dest;
  अन्यथा {
    *pdest = रिक्त;
    जर(dest)
      Curl_ssl_session_destroy(dest);
  }
  परत result;
}

स्थिर शून्य cf_ssl_scache_clear_peer(संरचना Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, रिक्त);
  जर(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    जर(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = रिक्त;
  }
  peer->sobj_free = रिक्त;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->किल्ली_passwd);
  curlx_safefree(peer->ssl_peer_किल्ली);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

स्थिर शून्य cf_ssl_scache_peer_set_obj(संरचना Curl_ssl_scache_peer *peer,
                                       शून्य *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  जर(peer->sobj_free) {
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
  peer->exportable = (!peer->clientcert && !peer->किल्ली_passwd &&
                      (!peer->ssl_peer_किल्ली ||
                       cf_ssl_peer_किल्ली_is_global(peer->ssl_peer_किल्ली)));
}

स्थिर CURLcode cf_ssl_scache_peer_init(संरचना Curl_ssl_scache_peer *peer,
                                        स्थिरांक अक्षर *ssl_peer_किल्ली,
                                        स्थिरांक अक्षर *clientcert,
                                        स्थिरांक अक्षर *किल्ली_passwd,
                                        स्थिरांक अचिन्हित अक्षर *salt,
                                        स्थिरांक अचिन्हित अक्षर *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_किल्ली);
  जर(ssl_peer_किल्ली) {
    peer->ssl_peer_किल्ली = curlx_strdup(ssl_peer_किल्ली);
    जर(!peer->ssl_peer_किल्ली)
      जा out;
    peer->hmac_set = FALSE;
  }
  अन्यथा जर(salt && hmac) {
    स_नकल(peer->किल्ली_salt, salt, आकार(peer->किल्ली_salt));
    स_नकल(peer->किल्ली_hmac, hmac, आकार(peer->किल्ली_hmac));
    peer->hmac_set = TRUE;
  }
  अन्यथा {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    जा out;
  }
  जर(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    जर(!peer->clientcert)
      जा out;
  }
  जर(किल्ली_passwd) {
    peer->किल्ली_passwd = curlx_strdup(किल्ली_passwd);
    जर(!peer->किल्ली_passwd)
      जा out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  जर(result)
    cf_ssl_scache_clear_peer(peer);
  परत result;
}

स्थिर शून्य cf_scache_session_remove(संरचना Curl_ssl_scache_peer *peer,
                                     संरचना Curl_ssl_session *s)
{
  (शून्य)peer;
  DEBUGASSERT(Curl_गाठ_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

स्थिर bool cf_scache_session_expired(संरचना Curl_ssl_session *s,
                                      curl_off_t now)
{
  परत (s->valid_until > 0) && (s->valid_until < now);
}

स्थिर शून्य cf_scache_peer_remove_expired(संरचना Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  संरचना Curl_llist_गाठ *n = Curl_llist_head(&peer->sessions);
  पर्यंत(n) {
    संरचना Curl_ssl_session *s = Curl_गाठ_elem(n);
    n = Curl_गाठ_next(n);
    जर(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

स्थिर शून्य cf_scache_peer_remove_non13(संरचना Curl_ssl_scache_peer *peer)
{
  संरचना Curl_llist_गाठ *n = Curl_llist_head(&peer->sessions);
  पर्यंत(n) {
    संरचना Curl_ssl_session *s = Curl_गाठ_elem(n);
    n = Curl_गाठ_next(n);
    जर(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(माप_प्रकार max_peers,
                                माप_प्रकार max_sessions_per_peer,
                                संरचना Curl_ssl_scache **pscache)
{
  संरचना Curl_ssl_scache *scache;
  संरचना Curl_ssl_scache_peer *peers;
  माप_प्रकार i;

  *pscache = रिक्त;
  peers = curlx_calloc(max_peers, आकार(*peers));
  जर(!peers)
    परत CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, आकार(*scache));
  जर(!scache) {
    curlx_free(peers);
    परत CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_मोजणी = max_peers;
  scache->peers = peers;
  scache->age = 1;
  साठी(i = 0; i < scache->peer_मोजणी; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  परत CURLE_OK;
}

शून्य Curl_ssl_scache_destroy(संरचना Curl_ssl_scache *scache)
{
  जर(GOOD_SCACHE(scache)) {
    माप_प्रकार i;
    scache->magic = 0;
    साठी(i = 0; i < scache->peer_मोजणी; ++i) {
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
  जर(cf_ssl_scache_get(data)) {
    संरचना ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    परत conn_config ? conn_config->cache_session : FALSE;
  }
  परत FALSE;
}

/* Lock shared SSL session data */
शून्य Curl_ssl_scache_lock(संरचना Curl_easy *data)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  जर(scache) {
    जर(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#अन्यथा
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
शून्य Curl_ssl_scache_unlock(संरचना Curl_easy *data)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  जर(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#अन्यथा
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    जर(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(संरचना Curl_easy *data)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  जर(!scache)
    परत FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#अन्यथा
  locked = (bool)scache->is_locked;
#endif
  परत locked;
}

स्थिर bool cf_ssl_scache_match_auth(संरचना Curl_ssl_scache_peer *peer,
                                     संरचना ssl_filter_config *conn_config)
{
  जर(!conn_config) {
    जर(peer->clientcert || peer->किल्ली_passwd)
      परत FALSE;
    परत TRUE;
  }
  अन्यथा जर(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    परत FALSE;
  जर(Curl_timestrcmp(peer->किल्ली_passwd, conn_config->किल्ली_passwd))
    परत FALSE;
  परत TRUE;
}

स्थिर CURLcode cf_ssl_find_peer_by_किल्ली(संरचना Curl_easy *data,
                                        संरचना Curl_ssl_scache *scache,
                                        स्थिरांक अक्षर *ssl_peer_किल्ली,
                                        संरचना ssl_filter_config *conn_config,
                                        संरचना Curl_ssl_scache_peer **ppeer)
{
  माप_प्रकार i, peer_किल्ली_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = रिक्त;
  जर(!GOOD_SCACHE(scache)) {
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_किल्ली, scache->peer_मोजणी);

  /* check for entries with known peer_key */
  साठी(i = 0; scache && i < scache->peer_मोजणी; i++) {
    जर(scache->peers[i].ssl_peer_किल्ली &&
       !म_भेद(ssl_peer_किल्ली, scache->peers[i].ssl_peer_किल्ली) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      जा out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  साठी(i = 0; scache && i < scache->peer_मोजणी; i++) {
    जर(!scache->peers[i].ssl_peer_किल्ली &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      अचिन्हित अक्षर my_hmac[CURL_SHA256_DIGEST_LENGTH];
      जर(!peer_किल्ली_len) /* we are lazy */
        peer_किल्ली_len = म_माप(ssl_peer_किल्ली);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].किल्ली_salt,
                           आकार(scache->peers[i].किल्ली_salt),
                           (स्थिरांक अचिन्हित अक्षर *)ssl_peer_किल्ली,
                           peer_किल्ली_len,
                           my_hmac);
      जर(result)
        जा out;
      जर(!स_भेद(scache->peers[i].किल्ली_hmac, my_hmac, आकार(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_किल्ली);
        scache->peers[i].ssl_peer_किल्ली = curlx_strdup(ssl_peer_किल्ली);
        जर(!scache->peers[i].ssl_peer_किल्ली) {
          result = CURLE_OUT_OF_MEMORY;
          जा out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        जा out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_किल्ली);
out:
  परत result;
}

स्थिर संरचना Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  संरचना Curl_ssl_scache *scache)
{
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  माप_प्रकार i;

  /* find empty or oldest peer */
  साठी(i = 0; i < scache->peer_मोजणी; ++i) {
    /* free peer entry? */
    जर(!scache->peers[i].ssl_peer_किल्ली && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      थांबा;
    }
    /* peer without sessions and obj */
    जर(!scache->peers[i].sobj &&
       !Curl_llist_मोजणी(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      थांबा;
    }
    /* remember "oldest" peer */
    जर(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  जर(peer)
    cf_ssl_scache_clear_peer(peer);
  परत peer;
}

स्थिर CURLcode cf_ssl_add_peer(संरचना Curl_easy *data,
                                संरचना Curl_ssl_scache *scache,
                                स्थिरांक अक्षर *ssl_peer_किल्ली,
                                संरचना ssl_filter_config *conn_config,
                                संरचना Curl_ssl_scache_peer **ppeer)
{
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode result = CURLE_OK;

  *ppeer = रिक्त;
  जर(ssl_peer_किल्ली) {
    result = cf_ssl_find_peer_by_किल्ली(data, scache, ssl_peer_किल्ली, conn_config,
                                     &peer);
    जर(result || !scache->peer_मोजणी)
      परत result;
  }

  जर(peer) {
    *ppeer = peer;
    परत CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  जर(peer) {
    अक्षर buffer[64];
    स्थिरांक अक्षर *ccert = conn_config ? conn_config->clientcert : रिक्त;
    स्थिरांक अक्षर *kpasswd = conn_config ? conn_config->किल्ली_passwd : रिक्त;
    जर(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, आकार(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_किल्ली, ccert, kpasswd,
                                     रिक्त, रिक्त);
    जर(result)
      जा out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  जर(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  परत result;
}

स्थिर शून्य cf_scache_peer_add_session(संरचना Curl_ssl_scache_peer *peer,
                                       संरचना Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  जर(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, रिक्त);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  अन्यथा {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    पर्यंत(Curl_llist_मोजणी(&peer->sessions) > peer->max_sessions) {
      Curl_गाठ_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

स्थिर CURLcode cf_scache_add_session(संरचना Curl_cfilter *cf,
                                      संरचना Curl_easy *data,
                                      संरचना Curl_ssl_scache *scache,
                                      स्थिरांक अक्षर *ssl_peer_किल्ली,
                                      संरचना Curl_ssl_session *s)
{
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)समय(रिक्त);
  curl_off_t max_lifetime;

  जर(!scache || !scache->peer_मोजणी) {
    Curl_ssl_session_destroy(s);
    परत CURLE_OK;
  }

  जर(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  जर(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  जर(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    परत CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_किल्ली, conn_config, &peer);
  जर(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (पूर्णांक)result);
    Curl_ssl_session_destroy(s);
    जा out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  जर(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_किल्ली, (पूर्णांक)result);
  }
  अन्यथा
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_किल्ली,
                  (अचिन्हित पूर्णांक)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_मोजणी(&peer->sessions) : 0);
  परत result;
}

CURLcode Curl_ssl_scache_put(संरचना Curl_cfilter *cf,
                             संरचना Curl_easy *data,
                             स्थिरांक अक्षर *ssl_peer_किल्ली,
                             संरचना Curl_ssl_session *s)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  जर(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    परत CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_किल्ली, s);
  Curl_ssl_scache_unlock(data);
  परत result;
}

शून्य Curl_ssl_scache_return(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *data,
                            स्थिरांक अक्षर *ssl_peer_किल्ली,
                            संरचना Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  जर(s && s->ietf_tls_id < 0x304)
    (शून्य)Curl_ssl_scache_put(cf, data, ssl_peer_किल्ली, s);
  अन्यथा
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              स्थिरांक अक्षर *ssl_peer_किल्ली,
                              संरचना Curl_ssl_session **ps)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  संरचना Curl_llist_गाठ *n;
  संरचना Curl_ssl_session *s = रिक्त;
  CURLcode result;

  *ps = रिक्त;
  जर(!scache)
    परत CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_किल्ली(data, scache, ssl_peer_किल्ली, conn_config,
                                   &peer);
  जर(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)समय(रिक्त));
    n = Curl_llist_head(&peer->sessions);
    जर(n) {
      s = Curl_गाठ_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  जर(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_किल्ली, (अचिन्हित पूर्णांक)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_मोजणी(&peer->sessions));
  }
  अन्यथा {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_किल्ली);
  }
  Curl_ssl_scache_unlock(data);
  परत result;
}

CURLcode Curl_ssl_scache_add_obj(संरचना Curl_cfilter *cf,
                                 संरचना Curl_easy *data,
                                 स्थिरांक अक्षर *ssl_peer_किल्ली,
                                 शून्य *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  जर(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    जा out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_किल्ली, conn_config, &peer);
  जर(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (पूर्णांक)result);
    जा out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = रिक्त;  /* peer took ownership */

out:
  जर(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  परत result;
}

शून्य *Curl_ssl_scache_get_obj(संरचना Curl_cfilter *cf,
                              संरचना Curl_easy *data,
                              स्थिरांक अक्षर *ssl_peer_किल्ली)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode result;
  शून्य *sobj;

  जर(!scache)
    परत रिक्त;

  result = cf_ssl_find_peer_by_किल्ली(data, scache, ssl_peer_किल्ली, conn_config,
                                   &peer);
  जर(result)
    परत रिक्त;

  sobj = peer ? peer->sobj : रिक्त;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_किल्ली);
  परत sobj;
}

शून्य Curl_ssl_scache_remove_all(संरचना Curl_cfilter *cf,
                                संरचना Curl_easy *data,
                                स्थिरांक अक्षर *ssl_peer_किल्ली)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  CURLcode result;

  (शून्य)cf;
  जर(!scache)
    परत;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_किल्ली(data, scache, ssl_peer_किल्ली, conn_config,
                                   &peer);
  जर(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

स्थिर CURLcode cf_ssl_scache_peer_set_hmac(संरचना Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  जर(!peer->ssl_peer_किल्ली)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(रिक्त, peer->किल्ली_salt, आकार(peer->किल्ली_salt));
  जर(result)
    परत result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->किल्ली_salt, आकार(peer->किल्ली_salt),
                       (स्थिरांक अचिन्हित अक्षर *)peer->ssl_peer_किल्ली,
                       म_माप(peer->ssl_peer_किल्ली),
                       peer->किल्ली_hmac);
  जर(!result)
    peer->hmac_set = TRUE;
  परत result;
}

स्थिर CURLcode cf_ssl_find_peer_by_hmac(संरचना Curl_ssl_scache *scache,
                                         स्थिरांक अचिन्हित अक्षर *salt,
                                         स्थिरांक अचिन्हित अक्षर *hmac,
                                         संरचना Curl_ssl_scache_peer **ppeer)
{
  माप_प्रकार i;
  CURLcode result = CURLE_OK;

  *ppeer = रिक्त;
  जर(!GOOD_SCACHE(scache))
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  साठी(i = 0; scache && i < scache->peer_मोजणी; i++) {
    संरचना Curl_ssl_scache_peer *peer = &scache->peers[i];
    जर(!cf_ssl_scache_match_auth(peer, रिक्त))
      सुरूठेवा;
    जर(scache->peers[i].hmac_set &&
       !स_भेद(peer->किल्ली_salt, salt, आकार(peer->किल्ली_salt)) &&
       !स_भेद(peer->किल्ली_hmac, hmac, आकार(peer->किल्ली_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      जा out;
    }
    अन्यथा जर(peer->ssl_peer_किल्ली) {
      अचिन्हित अक्षर my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, आकार(peer->किल्ली_salt),
                           (स्थिरांक अचिन्हित अक्षर *)peer->ssl_peer_किल्ली,
                           म_माप(peer->ssl_peer_किल्ली),
                           my_hmac);
      जर(result)
        जा out;
      जर(!स_भेद(my_hmac, hmac, आकार(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        जर(!peer->hmac_set) {
          स_नकल(peer->किल्ली_salt, salt, आकार(peer->किल्ली_salt));
          स_नकल(peer->किल्ली_hmac, hmac, आकार(peer->किल्ली_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        जा out;
      }
    }
  }
out:
  परत result;
}

CURLcode Curl_ssl_session_import(संरचना Curl_easy *data,
                                 स्थिरांक अक्षर *ssl_peer_किल्ली,
                                 स्थिरांक अचिन्हित अक्षर *shmac, माप_प्रकार shmac_len,
                                 स्थिरांक शून्य *sdata, माप_प्रकार sdata_len)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना Curl_ssl_scache_peer *peer = रिक्त;
  संरचना Curl_ssl_session *s = रिक्त;
  bool have_lock = FALSE;
  CURLcode result;

  जर(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    जा out;
  }
  जर(!ssl_peer_किल्ली && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    जा out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  जर(result)
    जा out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  जर(ssl_peer_किल्ली) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_किल्ली, रिक्त, &peer);
    जर(result)
      जा out;
  }
  अन्यथा जर(shmac_len != (आकार(peer->किल्ली_salt) + आकार(peer->किल्ली_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    जा out;
  }
  अन्यथा {
    स्थिरांक अचिन्हित अक्षर *salt = shmac;
    स्थिरांक अचिन्हित अक्षर *hmac = shmac + आकार(peer->किल्ली_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    जर(result)
      जा out;
    जर(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      जर(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_किल्ली, रिक्त, रिक्त,
                                         salt, hmac);
        जर(result)
          जा out;
      }
    }
  }

  जर(peer) {
    cf_scache_peer_add_session(peer, s, समय(रिक्त));
    s = रिक्त; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_किल्ली ? peer->ssl_peer_किल्ली : "without key",
                  Curl_llist_मोजणी(&peer->sessions));
  }

out:
  जर(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  परत result;
}

CURLcode Curl_ssl_session_export(संरचना Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 शून्य *userptr)
{
  संरचना Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  संरचना Curl_ssl_scache_peer *peer;
  संरचना dynbuf sbuf, hbuf;
  संरचना Curl_llist_गाठ *n;
  माप_प्रकार i;
  curl_off_t now = समय(रिक्त);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  माप_प्रकार npeers = 0, ntickets = 0;
#endif

  जर(!export_fn)
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  साठी(i = 0; scache && i < scache->peer_मोजणी; i++) {
    peer = &scache->peers[i];
    जर(!peer->ssl_peer_किल्ली && !peer->hmac_set)
      सुरूठेवा; /* skip free entry */
    जर(!peer->exportable)
      सुरूठेवा;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    जर(n)
      VERBOSE(++npeers);
    पर्यंत(n) {
      संरचना Curl_ssl_session *s = Curl_गाठ_elem(n);
      जर(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        जर(result)
          जा out;
      }
      जर(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->किल्ली_salt, आकार(peer->किल्ली_salt));
        जर(result)
          जा out;
        result = curlx_dyn_addn(&hbuf, peer->किल्ली_hmac, आकार(peer->किल्ली_hmac));
        जर(result)
          जा out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      जर(result)
        जा out;

      result = export_fn(data, userptr, peer->ssl_peer_किल्ली,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      जर(result)
        जा out;
      VERBOSE(++ntickets);
      n = Curl_गाठ_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  परत result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
