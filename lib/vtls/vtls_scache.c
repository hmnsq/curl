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
#ضمن "curl_setup.h"

#ifdef USE_SSL

#ضمن "urldata.h"
#ضمن "cfilters.h"

#ضمن "vtls/vtls.h" /* generic SSL protos etc */
#ضمن "vtls/vtls_int.h"
#ضمن "vtls/vtls_scache.h"
#ضمن "vtls/vtls_spack.h"

#ضمن "strcase.h"
#ضمن "url.h"
#ضمن "llist.h"
#ضمن "curl_share.h"
#ضمن "curl_threads.h"
#ضمن "curl_trc.h"
#ضمن "curl_sha256.h"
#ضمن "rand.h"
#ضمن "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
بنية Curl_ssl_scache_peer {
  حرف *ssl_peer_key;      /* id for peer + relevant TLS configuration */
  حرف *clientcert;
  حرف *key_passwd;
  بنية Curl_llist sessions;
  فراغ *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned حرف key_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned حرف key_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ساكن CURLcode cf_ssl_peer_key_add_string(بنية dynbuf *buf,
                                           ثابت حرف *name,
                                           ثابت حرف *value)
{
  החזר curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(value), value);
}

ساكن CURLcode cf_ssl_peer_key_add_path(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         ثابت حرف *path,
                                         bool *is_local)
{
  אם(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    حرف *abspath = _fullpath(عدم, path, 0);
    אם(abspath) {
      CURLcode result = cf_ssl_peer_key_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      حرر(abspath); /* allocated by CRT, use system free() */
      החזר result;
    }
    *is_local = TRUE;
#وإلاإذا defined(HAVE_REALPATH)
    אם(path[0] != '/') {
      حرف *abspath = realpath(path, عدم);
      אם(abspath) {
        CURLcode result = cf_ssl_peer_key_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        حرر(abspath); /* allocated by libc, free without memdebug */
        החזר result;
      }
      *is_local = TRUE;
    }
#endif
    החזר cf_ssl_peer_key_add_string(buf, name, path);
  }
  החזר CURLE_OK;
}

ساكن CURLcode cf_ssl_peer_key_add_hash(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         بنية curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  אם(blob && blob->len) {
    unsigned حرف hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    אם(result)
      اذهب out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    אם(result)
      اذهب out;
    עבור(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      אם(result)
        اذهب out;
    }
  }
out:
  החזר result;
}

ساكن CURLcode cf_ssl_peer_key_add_mtls(بنية dynbuf *buf,
                                         بنية ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  אם(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_key_add_path(buf, "CCERT", ssl->clientcert, is_local);
    אם(result)
      اذهب out;
  }
  אם(ssl->key && ssl->key[0]) {
    result = cf_ssl_peer_key_add_path(buf, "KEY", ssl->key, is_local);
    אם(result)
      اذهب out;
  }
  אם(ssl->key_blob) {
    result = cf_ssl_peer_key_add_hash(buf, "KEYBlob", ssl->key_blob);
    אם(result)
      اذهب out;
  }
  אם(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    עבור(i = 0; !result && ssl->cert_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    אם(result)
      اذهب out;
  }
  אם(ssl->key_type && ssl->key_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->key_type));
    עבור(i = 0; !result && ssl->key_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->key_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  החזר result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

ساكن bool cf_ssl_peer_key_is_global(ثابت حرف *peer_key)
{
  size_t len = peer_key ? strlen(peer_key) : 0;
  החזר (len > 2) &&
         (peer_key[len - 1] == 'G') &&
         (peer_key[len - 2] == ':');
}

ساكن CURLcode ssl_peer_key_add_transport(بنية dynbuf *buf,
                                           uint8_t transport)
{
  بدل(transport) {
  حالة TRNSPRT_TCP:
    החזר CURLE_OK;
  حالة TRNSPRT_UDP:
    החזר curlx_dyn_add(buf, ":UDP");
  حالة TRNSPRT_QUIC:
    החזר curlx_dyn_add(buf, ":QUIC");
  حالة TRNSPRT_UNIX:
    החזר curlx_dyn_add(buf, ":UNIX");
  افتراضي:
    החזר curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ساكن CURLcode ssl_peer_key_add_vrfy(بنية dynbuf *buf,
                                      بنية ssl_filter_config *ssl,
                                      ثابت بنية ssl_peer *peer)
{
  CURLcode result;

  אם(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    אם(result)
      החזר result;
  }
  אם(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    אם(result)
      החזר result;
  }
  אם(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    אם(result)
      החזר result;
  }
  אם((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_key_add_string(buf, "CHOST", peer->peer->hostname);
    אם(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    אם(result)
      החזר result;
  }
  החזר CURLE_OK;
}

ساكن CURLcode ssl_peer_key_build(بنية ssl_filter_config *ssl,
                                   ثابت بنية ssl_peer *peer,
                                   ثابت حرف *tls_id,
                                   حرف **ppeer_key)
{
  بنية dynbuf buf;
  size_t key_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_key = عدم;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  אם(result)
    اذهب out;
  result = ssl_peer_key_add_transport(&buf, peer->transport);
  אם(result)
    اذهب out;
  result = ssl_peer_key_add_vrfy(&buf, ssl, peer);
  אם(result)
    اذهب out;
  אם(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    אם(result)
      اذهب out;
  }
  אם(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    אם(result)
      اذهب out;
  }
  אם(ssl->cipher_list) {
    result = cf_ssl_peer_key_add_string(&buf, "CIPHER", ssl->cipher_list);
    אם(result)
      اذهب out;
  }
  אם(ssl->cipher_list13) {
    result = cf_ssl_peer_key_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    אם(result)
      اذهب out;
  }
  אם(ssl->curves) {
    result = cf_ssl_peer_key_add_string(&buf, "CURVES", ssl->curves);
    אם(result)
      اذهب out;
  }
  אם(ssl->signature_algorithms) {
    result = cf_ssl_peer_key_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    אם(result)
      اذهب out;
  }
  אם(ssl->verifypeer) {
    result = cf_ssl_peer_key_add_path(&buf, "CA", ssl->CAfile, &is_local);
    אם(!result)
      result = cf_ssl_peer_key_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    אם(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    אם(!result)
      result = cf_ssl_peer_key_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    אם(result)
      اذهب out;
    result = cf_ssl_peer_key_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    אם(result)
      اذهب out;
    אם(ssl->ca_info_blob) {
      result = cf_ssl_peer_key_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      אם(result)
        اذهب out;
    }
    אם(ssl->issuercert_blob) {
      result = cf_ssl_peer_key_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      אם(result)
        اذهب out;
    }
  }
  אם(ssl->cert_blob) {
    result = cf_ssl_peer_key_add_hash(&buf, "CertBlob", ssl->cert_blob);
    אם(result)
      اذهب out;
  }
  אם(ssl->pinned_key && ssl->pinned_key[0]) {
    result = cf_ssl_peer_key_add_string(&buf, "Pinned", ssl->pinned_key);
    אם(result)
      اذهب out;
  }

  result = cf_ssl_peer_key_add_mtls(&buf, ssl, &is_local);
  אם(result)
    اذهب out;

  אם(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    اذهب out;
  }
  result = cf_ssl_peer_key_add_string(&buf, "IMPL", tls_id);
  אם(result)
    اذهب out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  אם(result)
    اذهب out;

  *ppeer_key = curlx_dyn_take(&buf, &key_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  החזר result;
}

CURLcode Curl_ssl_peer_key_make(ثابت بنية ssl_peer *peer,
                                بنية ssl_filter_config *sslc,
                                ثابت حرف *tls_id,
                                حرف **ppeer_key)
{
  החזר ssl_peer_key_build(sslc, peer, tls_id, ppeer_key);
}

بنية Curl_ssl_scache {
  unsigned صحيح magic;
  بنية Curl_ssl_scache_peer *peers;
  size_t peer_count;
  صحيح default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

ساكن بنية Curl_ssl_scache *cf_ssl_scache_get(بنية Curl_easy *data)
{
  بنية Curl_ssl_scache *scache = عدم;
  /* If a share is present, its ssl_scache has preference over the multi */
  אם(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  אחרת אם(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  אם(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (فراغ *)scache);
    DEBUGASSERT(0);
    החזר عدم;
  }
  החזר scache;
}

ساكن فراغ cf_ssl_scache_session_ldestroy(فراغ *udata, فراغ *obj)
{
  بنية Curl_ssl_session *s = obj;
  (فراغ)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(فراغ *sdata, size_t sdata_len,
                                 صحيح ietf_tls_id, ثابت حرف *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 بنية Curl_ssl_session **psession)
{
  החזר Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  عدم, 0, psession);
}

CURLcode Curl_ssl_session_create2(فراغ *sdata, size_t sdata_len,
                                  صحيح ietf_tls_id, ثابت حرف *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned حرف *quic_tp, size_t quic_tp_len,
                                  بنية Curl_ssl_session **psession)
{
  بنية Curl_ssl_session *s;

  אם(!sdata || !sdata_len) {
    curlx_free(sdata);
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = عدم;
  s = curlx_calloc(1, حجم(*s));
  אם(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    החזר CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  אם(alpn) {
    s->alpn = curlx_strdup(alpn);
    אם(!s->alpn) {
      cf_ssl_scache_session_ldestroy(عدم, s);
      החזר CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  החזר CURLE_OK;
}

فراغ Curl_ssl_session_destroy(بنية Curl_ssl_session *s)
{
  אם(s) {
    /* if in the list, the list destructor takes care of it */
    אם(Curl_node_llist(&s->list))
      Curl_node_remove(&s->list);
    אחרת {
      cf_ssl_scache_session_ldestroy(عدم, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(بنية Curl_ssl_session *src,
                              بنية Curl_ssl_session **pdest)
{
  بنية Curl_ssl_session *dest = عدم;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  אם(!src || !pdest)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = عدم;

  dest = curlx_calloc(1, حجم(*dest));
  אם(!dest)
    اذهب out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  אם(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    אם(!dest->sdata)
      اذهب out;
    dest->sdata_len = src->sdata_len;
  }
  אם(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    אם(!dest->quic_tp)
      اذهب out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  אם(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    אם(!dest->alpn)
      اذهب out;
  }
  result = CURLE_OK;

out:
  אם(!result)
    *pdest = dest;
  אחרת {
    *pdest = عدم;
    אם(dest)
      Curl_ssl_session_destroy(dest);
  }
  החזר result;
}

ساكن فراغ cf_ssl_scache_clear_peer(بنية Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, عدم);
  אם(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    אם(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = عدم;
  }
  peer->sobj_free = عدم;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->key_passwd);
  curlx_safefree(peer->ssl_peer_key);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

ساكن فراغ cf_ssl_scache_peer_set_obj(بنية Curl_ssl_scache_peer *peer,
                                       فراغ *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  אם(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

ساكن فراغ cf_ssl_cache_peer_update(بنية Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->key_passwd &&
                      (!peer->ssl_peer_key ||
                       cf_ssl_peer_key_is_global(peer->ssl_peer_key)));
}

ساكن CURLcode cf_ssl_scache_peer_init(بنية Curl_ssl_scache_peer *peer,
                                        ثابت حرف *ssl_peer_key,
                                        ثابت حرف *clientcert,
                                        ثابت حرف *key_passwd,
                                        ثابت unsigned حرف *salt,
                                        ثابت unsigned حرف *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_key);
  אם(ssl_peer_key) {
    peer->ssl_peer_key = curlx_strdup(ssl_peer_key);
    אם(!peer->ssl_peer_key)
      اذهب out;
    peer->hmac_set = FALSE;
  }
  אחרת אם(salt && hmac) {
    memcpy(peer->key_salt, salt, حجم(peer->key_salt));
    memcpy(peer->key_hmac, hmac, حجم(peer->key_hmac));
    peer->hmac_set = TRUE;
  }
  אחרת {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }
  אם(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    אם(!peer->clientcert)
      اذهب out;
  }
  אם(key_passwd) {
    peer->key_passwd = curlx_strdup(key_passwd);
    אם(!peer->key_passwd)
      اذهب out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  אם(result)
    cf_ssl_scache_clear_peer(peer);
  החזר result;
}

ساكن فراغ cf_scache_session_remove(بنية Curl_ssl_scache_peer *peer,
                                     بنية Curl_ssl_session *s)
{
  (فراغ)peer;
  DEBUGASSERT(Curl_node_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

ساكن bool cf_scache_session_expired(بنية Curl_ssl_session *s,
                                      curl_off_t now)
{
  החזר (s->valid_until > 0) && (s->valid_until < now);
}

ساكن فراغ cf_scache_peer_remove_expired(بنية Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  بنية Curl_llist_node *n = Curl_llist_head(&peer->sessions);
  כלעוד(n) {
    بنية Curl_ssl_session *s = Curl_node_elem(n);
    n = Curl_node_next(n);
    אם(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ساكن فراغ cf_scache_peer_remove_non13(بنية Curl_ssl_scache_peer *peer)
{
  بنية Curl_llist_node *n = Curl_llist_head(&peer->sessions);
  כלעוד(n) {
    بنية Curl_ssl_session *s = Curl_node_elem(n);
    n = Curl_node_next(n);
    אם(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                بنية Curl_ssl_scache **pscache)
{
  بنية Curl_ssl_scache *scache;
  بنية Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = عدم;
  peers = curlx_calloc(max_peers, حجم(*peers));
  אם(!peers)
    החזר CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, حجم(*scache));
  אם(!scache) {
    curlx_free(peers);
    החזר CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_count = max_peers;
  scache->peers = peers;
  scache->age = 1;
  עבור(i = 0; i < scache->peer_count; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  החזר CURLE_OK;
}

فراغ Curl_ssl_scache_destroy(بنية Curl_ssl_scache *scache)
{
  אם(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    עבור(i = 0; i < scache->peer_count; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(بنية Curl_cfilter *cf, بنية Curl_easy *data)
{
  אם(cf_ssl_scache_get(data)) {
    بنية ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    החזר conn_config ? conn_config->cache_session : FALSE;
  }
  החזר FALSE;
}

/* Lock shared SSL session data */
فراغ Curl_ssl_scache_lock(بنية Curl_easy *data)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  אם(scache) {
    אם(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#אחרת
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
فراغ Curl_ssl_scache_unlock(بنية Curl_easy *data)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  אם(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#אחרת
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    אם(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(بنية Curl_easy *data)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  אם(!scache)
    החזר FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#אחרת
  locked = (bool)scache->is_locked;
#endif
  החזר locked;
}

ساكن bool cf_ssl_scache_match_auth(بنية Curl_ssl_scache_peer *peer,
                                     بنية ssl_filter_config *conn_config)
{
  אם(!conn_config) {
    אם(peer->clientcert || peer->key_passwd)
      החזר FALSE;
    החזר TRUE;
  }
  אחרת אם(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    החזר FALSE;
  אם(Curl_timestrcmp(peer->key_passwd, conn_config->key_passwd))
    החזר FALSE;
  החזר TRUE;
}

ساكن CURLcode cf_ssl_find_peer_by_key(بنية Curl_easy *data,
                                        بنية Curl_ssl_scache *scache,
                                        ثابت حرف *ssl_peer_key,
                                        بنية ssl_filter_config *conn_config,
                                        بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_key_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = عدم;
  אם(!GOOD_SCACHE(scache)) {
    החזר CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_key, scache->peer_count);

  /* check for entries with known peer_key */
  עבור(i = 0; scache && i < scache->peer_count; i++) {
    אם(scache->peers[i].ssl_peer_key &&
       !strcmp(ssl_peer_key, scache->peers[i].ssl_peer_key) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      اذهب out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  עבור(i = 0; scache && i < scache->peer_count; i++) {
    אם(!scache->peers[i].ssl_peer_key &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned حرف my_hmac[CURL_SHA256_DIGEST_LENGTH];
      אם(!peer_key_len) /* we are lazy */
        peer_key_len = strlen(ssl_peer_key);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].key_salt,
                           حجم(scache->peers[i].key_salt),
                           (ثابت unsigned حرف *)ssl_peer_key,
                           peer_key_len,
                           my_hmac);
      אם(result)
        اذهب out;
      אם(!memcmp(scache->peers[i].key_hmac, my_hmac, حجم(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_key);
        scache->peers[i].ssl_peer_key = curlx_strdup(ssl_peer_key);
        אם(!scache->peers[i].ssl_peer_key) {
          result = CURLE_OUT_OF_MEMORY;
          اذهب out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        اذهب out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_key);
out:
  החזר result;
}

ساكن بنية Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  بنية Curl_ssl_scache *scache)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  size_t i;

  /* find empty or oldest peer */
  עבור(i = 0; i < scache->peer_count; ++i) {
    /* free peer entry? */
    אם(!scache->peers[i].ssl_peer_key && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      اكسر;
    }
    /* peer without sessions and obj */
    אם(!scache->peers[i].sobj &&
       !Curl_llist_count(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      اكسر;
    }
    /* remember "oldest" peer */
    אם(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  אם(peer)
    cf_ssl_scache_clear_peer(peer);
  החזר peer;
}

ساكن CURLcode cf_ssl_add_peer(بنية Curl_easy *data,
                                بنية Curl_ssl_scache *scache,
                                ثابت حرف *ssl_peer_key,
                                بنية ssl_filter_config *conn_config,
                                بنية Curl_ssl_scache_peer **ppeer)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode result = CURLE_OK;

  *ppeer = عدم;
  אם(ssl_peer_key) {
    result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                     &peer);
    אם(result || !scache->peer_count)
      החזר result;
  }

  אם(peer) {
    *ppeer = peer;
    החזר CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  אם(peer) {
    حرف buffer[64];
    ثابت حرف *ccert = conn_config ? conn_config->clientcert : عدم;
    ثابت حرف *kpasswd = conn_config ? conn_config->key_passwd : عدم;
    אם(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, حجم(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_key, ccert, kpasswd,
                                     عدم, عدم);
    אם(result)
      اذهب out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  אם(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  החזר result;
}

ساكن فراغ cf_scache_peer_add_session(بنية Curl_ssl_scache_peer *peer,
                                       بنية Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  אם(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, عدم);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  אחרת {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    כלעוד(Curl_llist_count(&peer->sessions) > peer->max_sessions) {
      Curl_node_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

ساكن CURLcode cf_scache_add_session(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *data,
                                      بنية Curl_ssl_scache *scache,
                                      ثابت حرف *ssl_peer_key,
                                      بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(عدم);
  curl_off_t max_lifetime;

  אם(!scache || !scache->peer_count) {
    Curl_ssl_session_destroy(s);
    החזר CURLE_OK;
  }

  אם(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  אם(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  אם(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    החזר CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_key, conn_config, &peer);
  אם(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (صحيح)result);
    Curl_ssl_session_destroy(s);
    اذهب out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  אם(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_key, (صحيح)result);
  }
  אחרת
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_key,
                  (unsigned صحيح)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_count(&peer->sessions) : 0);
  החזר result;
}

CURLcode Curl_ssl_scache_put(بنية Curl_cfilter *cf,
                             بنية Curl_easy *data,
                             ثابت حرف *ssl_peer_key,
                             بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  אם(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    החזר CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_key, s);
  Curl_ssl_scache_unlock(data);
  החזר result;
}

فراغ Curl_ssl_scache_return(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            ثابت حرف *ssl_peer_key,
                            بنية Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  אם(s && s->ietf_tls_id < 0x304)
    (فراغ)Curl_ssl_scache_put(cf, data, ssl_peer_key, s);
  אחרת
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              ثابت حرف *ssl_peer_key,
                              بنية Curl_ssl_session **ps)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_llist_node *n;
  بنية Curl_ssl_session *s = عدم;
  CURLcode result;

  *ps = عدم;
  אם(!scache)
    החזר CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  אם(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(عدم));
    n = Curl_llist_head(&peer->sessions);
    אם(n) {
      s = Curl_node_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  אם(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_key, (unsigned صحيح)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_count(&peer->sessions));
  }
  אחרת {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_key);
  }
  Curl_ssl_scache_unlock(data);
  החזר result;
}

CURLcode Curl_ssl_scache_add_obj(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *data,
                                 ثابت حرف *ssl_peer_key,
                                 فراغ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  אם(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_key, conn_config, &peer);
  אם(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (صحيح)result);
    اذهب out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = عدم;  /* peer took ownership */

out:
  אם(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  החזר result;
}

فراغ *Curl_ssl_scache_get_obj(بنية Curl_cfilter *cf,
                              بنية Curl_easy *data,
                              ثابت حرف *ssl_peer_key)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode result;
  فراغ *sobj;

  אם(!scache)
    החזר عدم;

  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  אם(result)
    החזר عدم;

  sobj = peer ? peer->sobj : عدم;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_key);
  החזר sobj;
}

فراغ Curl_ssl_scache_remove_all(بنية Curl_cfilter *cf,
                                بنية Curl_easy *data,
                                ثابت حرف *ssl_peer_key)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode result;

  (فراغ)cf;
  אם(!scache)
    החזר;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  אם(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

ساكن CURLcode cf_ssl_scache_peer_set_hmac(بنية Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  אם(!peer->ssl_peer_key)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(عدم, peer->key_salt, حجم(peer->key_salt));
  אם(result)
    החזר result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->key_salt, حجم(peer->key_salt),
                       (ثابت unsigned حرف *)peer->ssl_peer_key,
                       strlen(peer->ssl_peer_key),
                       peer->key_hmac);
  אם(!result)
    peer->hmac_set = TRUE;
  החזר result;
}

ساكن CURLcode cf_ssl_find_peer_by_hmac(بنية Curl_ssl_scache *scache,
                                         ثابت unsigned حرف *salt,
                                         ثابت unsigned حرف *hmac,
                                         بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = عدم;
  אם(!GOOD_SCACHE(scache))
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  עבור(i = 0; scache && i < scache->peer_count; i++) {
    بنية Curl_ssl_scache_peer *peer = &scache->peers[i];
    אם(!cf_ssl_scache_match_auth(peer, عدم))
      استمر;
    אם(scache->peers[i].hmac_set &&
       !memcmp(peer->key_salt, salt, حجم(peer->key_salt)) &&
       !memcmp(peer->key_hmac, hmac, حجم(peer->key_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      اذهب out;
    }
    אחרת אם(peer->ssl_peer_key) {
      unsigned حرف my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, حجم(peer->key_salt),
                           (ثابت unsigned حرف *)peer->ssl_peer_key,
                           strlen(peer->ssl_peer_key),
                           my_hmac);
      אם(result)
        اذهب out;
      אם(!memcmp(my_hmac, hmac, حجم(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        אם(!peer->hmac_set) {
          memcpy(peer->key_salt, salt, حجم(peer->key_salt));
          memcpy(peer->key_hmac, hmac, حجم(peer->key_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        اذهب out;
      }
    }
  }
out:
  החזר result;
}

CURLcode Curl_ssl_session_import(بنية Curl_easy *data,
                                 ثابت حرف *ssl_peer_key,
                                 ثابت unsigned حرف *shmac, size_t shmac_len,
                                 ثابت فراغ *sdata, size_t sdata_len)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_ssl_session *s = عدم;
  bool have_lock = FALSE;
  CURLcode result;

  אם(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }
  אם(!ssl_peer_key && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  אם(result)
    اذهب out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  אם(ssl_peer_key) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_key, عدم, &peer);
    אם(result)
      اذهب out;
  }
  אחרת אם(shmac_len != (حجم(peer->key_salt) + حجم(peer->key_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }
  אחרת {
    ثابت unsigned حرف *salt = shmac;
    ثابت unsigned حرف *hmac = shmac + حجم(peer->key_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    אם(result)
      اذهب out;
    אם(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      אם(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_key, عدم, عدم,
                                         salt, hmac);
        אם(result)
          اذهب out;
      }
    }
  }

  אם(peer) {
    cf_scache_peer_add_session(peer, s, time(عدم));
    s = عدم; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_key ? peer->ssl_peer_key : "without key",
                  Curl_llist_count(&peer->sessions));
  }

out:
  אם(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  החזר result;
}

CURLcode Curl_ssl_session_export(بنية Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 فراغ *userptr)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  بنية Curl_ssl_scache_peer *peer;
  بنية dynbuf sbuf, hbuf;
  بنية Curl_llist_node *n;
  size_t i;
  curl_off_t now = time(عدم);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  אם(!export_fn)
    החזר CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  עבור(i = 0; scache && i < scache->peer_count; i++) {
    peer = &scache->peers[i];
    אם(!peer->ssl_peer_key && !peer->hmac_set)
      استمر; /* skip free entry */
    אם(!peer->exportable)
      استمر;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    אם(n)
      VERBOSE(++npeers);
    כלעוד(n) {
      بنية Curl_ssl_session *s = Curl_node_elem(n);
      אם(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        אם(result)
          اذهب out;
      }
      אם(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->key_salt, حجم(peer->key_salt));
        אם(result)
          اذهب out;
        result = curlx_dyn_addn(&hbuf, peer->key_hmac, حجم(peer->key_hmac));
        אם(result)
          اذهب out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      אם(result)
        اذهب out;

      result = export_fn(data, userptr, peer->ssl_peer_key,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      אם(result)
        اذهب out;
      VERBOSE(++ntickets);
      n = Curl_node_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  החזר result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
