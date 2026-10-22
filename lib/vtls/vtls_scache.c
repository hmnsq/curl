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
  ܦܢܐ curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(value), value);
}

ساكن CURLcode cf_ssl_peer_key_add_path(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         ثابت حرف *path,
                                         bool *is_local)
{
  ܐܢ(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    حرف *abspath = _fullpath(عدم, path, 0);
    ܐܢ(abspath) {
      CURLcode result = cf_ssl_peer_key_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      حرر(abspath); /* allocated by CRT, use system free() */
      ܦܢܐ result;
    }
    *is_local = TRUE;
#وإلاإذا defined(HAVE_REALPATH)
    ܐܢ(path[0] != '/') {
      حرف *abspath = realpath(path, عدم);
      ܐܢ(abspath) {
        CURLcode result = cf_ssl_peer_key_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        حرر(abspath); /* allocated by libc, free without memdebug */
        ܦܢܐ result;
      }
      *is_local = TRUE;
    }
#endif
    ܦܢܐ cf_ssl_peer_key_add_string(buf, name, path);
  }
  ܦܢܐ CURLE_OK;
}

ساكن CURLcode cf_ssl_peer_key_add_hash(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         بنية curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  ܐܢ(blob && blob->len) {
    unsigned حرف hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    ܐܢ(result)
      اذهب out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    ܐܢ(result)
      اذهب out;
    لكل(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      ܐܢ(result)
        اذهب out;
    }
  }
out:
  ܦܢܐ result;
}

ساكن CURLcode cf_ssl_peer_key_add_mtls(بنية dynbuf *buf,
                                         بنية ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  ܐܢ(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_key_add_path(buf, "CCERT", ssl->clientcert, is_local);
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->key && ssl->key[0]) {
    result = cf_ssl_peer_key_add_path(buf, "KEY", ssl->key, is_local);
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->key_blob) {
    result = cf_ssl_peer_key_add_hash(buf, "KEYBlob", ssl->key_blob);
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    لكل(i = 0; !result && ssl->cert_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->key_type && ssl->key_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->key_type));
    لكل(i = 0; !result && ssl->key_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->key_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  ܦܢܐ result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

ساكن bool cf_ssl_peer_key_is_global(ثابت حرف *peer_key)
{
  size_t len = peer_key ? strlen(peer_key) : 0;
  ܦܢܐ (len > 2) &&
         (peer_key[len - 1] == 'G') &&
         (peer_key[len - 2] == ':');
}

ساكن CURLcode ssl_peer_key_add_transport(بنية dynbuf *buf,
                                           uint8_t transport)
{
  بدل(transport) {
  حالة TRNSPRT_TCP:
    ܦܢܐ CURLE_OK;
  حالة TRNSPRT_UDP:
    ܦܢܐ curlx_dyn_add(buf, ":UDP");
  حالة TRNSPRT_QUIC:
    ܦܢܐ curlx_dyn_add(buf, ":QUIC");
  حالة TRNSPRT_UNIX:
    ܦܢܐ curlx_dyn_add(buf, ":UNIX");
  افتراضي:
    ܦܢܐ curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ساكن CURLcode ssl_peer_key_add_vrfy(بنية dynbuf *buf,
                                      بنية ssl_filter_config *ssl,
                                      ثابت بنية ssl_peer *peer)
{
  CURLcode result;

  ܐܢ(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    ܐܢ(result)
      ܦܢܐ result;
  }
  ܐܢ(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    ܐܢ(result)
      ܦܢܐ result;
  }
  ܐܢ(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    ܐܢ(result)
      ܦܢܐ result;
  }
  ܐܢ((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_key_add_string(buf, "CHOST", peer->peer->hostname);
    ܐܢ(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    ܐܢ(result)
      ܦܢܐ result;
  }
  ܦܢܐ CURLE_OK;
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
  ܐܢ(result)
    اذهب out;
  result = ssl_peer_key_add_transport(&buf, peer->transport);
  ܐܢ(result)
    اذهب out;
  result = ssl_peer_key_add_vrfy(&buf, ssl, peer);
  ܐܢ(result)
    اذهب out;
  ܐܢ(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->cipher_list) {
    result = cf_ssl_peer_key_add_string(&buf, "CIPHER", ssl->cipher_list);
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->cipher_list13) {
    result = cf_ssl_peer_key_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->curves) {
    result = cf_ssl_peer_key_add_string(&buf, "CURVES", ssl->curves);
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->signature_algorithms) {
    result = cf_ssl_peer_key_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->verifypeer) {
    result = cf_ssl_peer_key_add_path(&buf, "CA", ssl->CAfile, &is_local);
    ܐܢ(!result)
      result = cf_ssl_peer_key_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    ܐܢ(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    ܐܢ(!result)
      result = cf_ssl_peer_key_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    ܐܢ(result)
      اذهب out;
    result = cf_ssl_peer_key_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    ܐܢ(result)
      اذهب out;
    ܐܢ(ssl->ca_info_blob) {
      result = cf_ssl_peer_key_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      ܐܢ(result)
        اذهب out;
    }
    ܐܢ(ssl->issuercert_blob) {
      result = cf_ssl_peer_key_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      ܐܢ(result)
        اذهب out;
    }
  }
  ܐܢ(ssl->cert_blob) {
    result = cf_ssl_peer_key_add_hash(&buf, "CertBlob", ssl->cert_blob);
    ܐܢ(result)
      اذهب out;
  }
  ܐܢ(ssl->pinned_key && ssl->pinned_key[0]) {
    result = cf_ssl_peer_key_add_string(&buf, "Pinned", ssl->pinned_key);
    ܐܢ(result)
      اذهب out;
  }

  result = cf_ssl_peer_key_add_mtls(&buf, ssl, &is_local);
  ܐܢ(result)
    اذهب out;

  ܐܢ(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    اذهب out;
  }
  result = cf_ssl_peer_key_add_string(&buf, "IMPL", tls_id);
  ܐܢ(result)
    اذهب out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  ܐܢ(result)
    اذهب out;

  *ppeer_key = curlx_dyn_take(&buf, &key_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  ܦܢܐ result;
}

CURLcode Curl_ssl_peer_key_make(ثابت بنية ssl_peer *peer,
                                بنية ssl_filter_config *sslc,
                                ثابت حرف *tls_id,
                                حرف **ppeer_key)
{
  ܦܢܐ ssl_peer_key_build(sslc, peer, tls_id, ppeer_key);
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
  ܐܢ(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  ܐܚܪܢܐ ܐܢ(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  ܐܢ(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (فراغ *)scache);
    DEBUGASSERT(0);
    ܦܢܐ عدم;
  }
  ܦܢܐ scache;
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
  ܦܢܐ Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
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

  ܐܢ(!sdata || !sdata_len) {
    curlx_free(sdata);
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = عدم;
  s = curlx_calloc(1, حجم(*s));
  ܐܢ(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  ܐܢ(alpn) {
    s->alpn = curlx_strdup(alpn);
    ܐܢ(!s->alpn) {
      cf_ssl_scache_session_ldestroy(عدم, s);
      ܦܢܐ CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  ܦܢܐ CURLE_OK;
}

فراغ Curl_ssl_session_destroy(بنية Curl_ssl_session *s)
{
  ܐܢ(s) {
    /* if in the list, the list destructor takes care of it */
    ܐܢ(Curl_node_llist(&s->list))
      Curl_node_remove(&s->list);
    ܐܚܪܢܐ {
      cf_ssl_scache_session_ldestroy(عدم, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(بنية Curl_ssl_session *src,
                              بنية Curl_ssl_session **pdest)
{
  بنية Curl_ssl_session *dest = عدم;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ܐܢ(!src || !pdest)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = عدم;

  dest = curlx_calloc(1, حجم(*dest));
  ܐܢ(!dest)
    اذهب out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  ܐܢ(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    ܐܢ(!dest->sdata)
      اذهب out;
    dest->sdata_len = src->sdata_len;
  }
  ܐܢ(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    ܐܢ(!dest->quic_tp)
      اذهب out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  ܐܢ(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    ܐܢ(!dest->alpn)
      اذهب out;
  }
  result = CURLE_OK;

out:
  ܐܢ(!result)
    *pdest = dest;
  ܐܚܪܢܐ {
    *pdest = عدم;
    ܐܢ(dest)
      Curl_ssl_session_destroy(dest);
  }
  ܦܢܐ result;
}

ساكن فراغ cf_ssl_scache_clear_peer(بنية Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, عدم);
  ܐܢ(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    ܐܢ(peer->sobj_free)
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
  ܐܢ(peer->sobj_free) {
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
  ܐܢ(ssl_peer_key) {
    peer->ssl_peer_key = curlx_strdup(ssl_peer_key);
    ܐܢ(!peer->ssl_peer_key)
      اذهب out;
    peer->hmac_set = FALSE;
  }
  ܐܚܪܢܐ ܐܢ(salt && hmac) {
    memcpy(peer->key_salt, salt, حجم(peer->key_salt));
    memcpy(peer->key_hmac, hmac, حجم(peer->key_hmac));
    peer->hmac_set = TRUE;
  }
  ܐܚܪܢܐ {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }
  ܐܢ(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    ܐܢ(!peer->clientcert)
      اذهب out;
  }
  ܐܢ(key_passwd) {
    peer->key_passwd = curlx_strdup(key_passwd);
    ܐܢ(!peer->key_passwd)
      اذهب out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  ܐܢ(result)
    cf_ssl_scache_clear_peer(peer);
  ܦܢܐ result;
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
  ܦܢܐ (s->valid_until > 0) && (s->valid_until < now);
}

ساكن فراغ cf_scache_peer_remove_expired(بنية Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  بنية Curl_llist_node *n = Curl_llist_head(&peer->sessions);
  ܟܡܐ(n) {
    بنية Curl_ssl_session *s = Curl_node_elem(n);
    n = Curl_node_next(n);
    ܐܢ(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ساكن فراغ cf_scache_peer_remove_non13(بنية Curl_ssl_scache_peer *peer)
{
  بنية Curl_llist_node *n = Curl_llist_head(&peer->sessions);
  ܟܡܐ(n) {
    بنية Curl_ssl_session *s = Curl_node_elem(n);
    n = Curl_node_next(n);
    ܐܢ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
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
  ܐܢ(!peers)
    ܦܢܐ CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, حجم(*scache));
  ܐܢ(!scache) {
    curlx_free(peers);
    ܦܢܐ CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_count = max_peers;
  scache->peers = peers;
  scache->age = 1;
  لكل(i = 0; i < scache->peer_count; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  ܦܢܐ CURLE_OK;
}

فراغ Curl_ssl_scache_destroy(بنية Curl_ssl_scache *scache)
{
  ܐܢ(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    لكل(i = 0; i < scache->peer_count; ++i) {
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
  ܐܢ(cf_ssl_scache_get(data)) {
    بنية ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    ܦܢܐ conn_config ? conn_config->cache_session : FALSE;
  }
  ܦܢܐ FALSE;
}

/* Lock shared SSL session data */
فراغ Curl_ssl_scache_lock(بنية Curl_easy *data)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ܐܢ(scache) {
    ܐܢ(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#ܐܚܪܢܐ
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
فراغ Curl_ssl_scache_unlock(بنية Curl_easy *data)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ܐܢ(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#ܐܚܪܢܐ
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    ܐܢ(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(بنية Curl_easy *data)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  ܐܢ(!scache)
    ܦܢܐ FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#ܐܚܪܢܐ
  locked = (bool)scache->is_locked;
#endif
  ܦܢܐ locked;
}

ساكن bool cf_ssl_scache_match_auth(بنية Curl_ssl_scache_peer *peer,
                                     بنية ssl_filter_config *conn_config)
{
  ܐܢ(!conn_config) {
    ܐܢ(peer->clientcert || peer->key_passwd)
      ܦܢܐ FALSE;
    ܦܢܐ TRUE;
  }
  ܐܚܪܢܐ ܐܢ(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    ܦܢܐ FALSE;
  ܐܢ(Curl_timestrcmp(peer->key_passwd, conn_config->key_passwd))
    ܦܢܐ FALSE;
  ܦܢܐ TRUE;
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
  ܐܢ(!GOOD_SCACHE(scache)) {
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_key, scache->peer_count);

  /* check for entries with known peer_key */
  لكل(i = 0; scache && i < scache->peer_count; i++) {
    ܐܢ(scache->peers[i].ssl_peer_key &&
       !strcmp(ssl_peer_key, scache->peers[i].ssl_peer_key) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      اذهب out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  لكل(i = 0; scache && i < scache->peer_count; i++) {
    ܐܢ(!scache->peers[i].ssl_peer_key &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned حرف my_hmac[CURL_SHA256_DIGEST_LENGTH];
      ܐܢ(!peer_key_len) /* we are lazy */
        peer_key_len = strlen(ssl_peer_key);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].key_salt,
                           حجم(scache->peers[i].key_salt),
                           (ثابت unsigned حرف *)ssl_peer_key,
                           peer_key_len,
                           my_hmac);
      ܐܢ(result)
        اذهب out;
      ܐܢ(!memcmp(scache->peers[i].key_hmac, my_hmac, حجم(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_key);
        scache->peers[i].ssl_peer_key = curlx_strdup(ssl_peer_key);
        ܐܢ(!scache->peers[i].ssl_peer_key) {
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
  ܦܢܐ result;
}

ساكن بنية Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  بنية Curl_ssl_scache *scache)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  size_t i;

  /* find empty or oldest peer */
  لكل(i = 0; i < scache->peer_count; ++i) {
    /* free peer entry? */
    ܐܢ(!scache->peers[i].ssl_peer_key && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      اكسر;
    }
    /* peer without sessions and obj */
    ܐܢ(!scache->peers[i].sobj &&
       !Curl_llist_count(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      اكسر;
    }
    /* remember "oldest" peer */
    ܐܢ(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  ܐܢ(peer)
    cf_ssl_scache_clear_peer(peer);
  ܦܢܐ peer;
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
  ܐܢ(ssl_peer_key) {
    result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                     &peer);
    ܐܢ(result || !scache->peer_count)
      ܦܢܐ result;
  }

  ܐܢ(peer) {
    *ppeer = peer;
    ܦܢܐ CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  ܐܢ(peer) {
    حرف buffer[64];
    ثابت حرف *ccert = conn_config ? conn_config->clientcert : عدم;
    ثابت حرف *kpasswd = conn_config ? conn_config->key_passwd : عدم;
    ܐܢ(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, حجم(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_key, ccert, kpasswd,
                                     عدم, عدم);
    ܐܢ(result)
      اذهب out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  ܐܢ(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  ܦܢܐ result;
}

ساكن فراغ cf_scache_peer_add_session(بنية Curl_ssl_scache_peer *peer,
                                       بنية Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  ܐܢ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, عدم);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  ܐܚܪܢܐ {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    ܟܡܐ(Curl_llist_count(&peer->sessions) > peer->max_sessions) {
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

  ܐܢ(!scache || !scache->peer_count) {
    Curl_ssl_session_destroy(s);
    ܦܢܐ CURLE_OK;
  }

  ܐܢ(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  ܐܢ(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  ܐܢ(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    ܦܢܐ CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_key, conn_config, &peer);
  ܐܢ(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (صحيح)result);
    Curl_ssl_session_destroy(s);
    اذهب out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  ܐܢ(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_key, (صحيح)result);
  }
  ܐܚܪܢܐ
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_key,
                  (unsigned صحيح)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_count(&peer->sessions) : 0);
  ܦܢܐ result;
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

  ܐܢ(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    ܦܢܐ CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_key, s);
  Curl_ssl_scache_unlock(data);
  ܦܢܐ result;
}

فراغ Curl_ssl_scache_return(بنية Curl_cfilter *cf,
                            بنية Curl_easy *data,
                            ثابت حرف *ssl_peer_key,
                            بنية Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  ܐܢ(s && s->ietf_tls_id < 0x304)
    (فراغ)Curl_ssl_scache_put(cf, data, ssl_peer_key, s);
  ܐܚܪܢܐ
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
  ܐܢ(!scache)
    ܦܢܐ CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  ܐܢ(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(عدم));
    n = Curl_llist_head(&peer->sessions);
    ܐܢ(n) {
      s = Curl_node_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  ܐܢ(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_key, (unsigned صحيح)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_count(&peer->sessions));
  }
  ܐܚܪܢܐ {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_key);
  }
  Curl_ssl_scache_unlock(data);
  ܦܢܐ result;
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

  ܐܢ(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_key, conn_config, &peer);
  ܐܢ(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (صحيح)result);
    اذهب out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = عدم;  /* peer took ownership */

out:
  ܐܢ(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  ܦܢܐ result;
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

  ܐܢ(!scache)
    ܦܢܐ عدم;

  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  ܐܢ(result)
    ܦܢܐ عدم;

  sobj = peer ? peer->sobj : عدم;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_key);
  ܦܢܐ sobj;
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
  ܐܢ(!scache)
    ܦܢܐ;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  ܐܢ(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

ساكن CURLcode cf_ssl_scache_peer_set_hmac(بنية Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  ܐܢ(!peer->ssl_peer_key)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(عدم, peer->key_salt, حجم(peer->key_salt));
  ܐܢ(result)
    ܦܢܐ result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->key_salt, حجم(peer->key_salt),
                       (ثابت unsigned حرف *)peer->ssl_peer_key,
                       strlen(peer->ssl_peer_key),
                       peer->key_hmac);
  ܐܢ(!result)
    peer->hmac_set = TRUE;
  ܦܢܐ result;
}

ساكن CURLcode cf_ssl_find_peer_by_hmac(بنية Curl_ssl_scache *scache,
                                         ثابت unsigned حرف *salt,
                                         ثابت unsigned حرف *hmac,
                                         بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = عدم;
  ܐܢ(!GOOD_SCACHE(scache))
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  لكل(i = 0; scache && i < scache->peer_count; i++) {
    بنية Curl_ssl_scache_peer *peer = &scache->peers[i];
    ܐܢ(!cf_ssl_scache_match_auth(peer, عدم))
      استمر;
    ܐܢ(scache->peers[i].hmac_set &&
       !memcmp(peer->key_salt, salt, حجم(peer->key_salt)) &&
       !memcmp(peer->key_hmac, hmac, حجم(peer->key_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      اذهب out;
    }
    ܐܚܪܢܐ ܐܢ(peer->ssl_peer_key) {
      unsigned حرف my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, حجم(peer->key_salt),
                           (ثابت unsigned حرف *)peer->ssl_peer_key,
                           strlen(peer->ssl_peer_key),
                           my_hmac);
      ܐܢ(result)
        اذهب out;
      ܐܢ(!memcmp(my_hmac, hmac, حجم(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        ܐܢ(!peer->hmac_set) {
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
  ܦܢܐ result;
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

  ܐܢ(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }
  ܐܢ(!ssl_peer_key && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  ܐܢ(result)
    اذهب out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  ܐܢ(ssl_peer_key) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_key, عدم, &peer);
    ܐܢ(result)
      اذهب out;
  }
  ܐܚܪܢܐ ܐܢ(shmac_len != (حجم(peer->key_salt) + حجم(peer->key_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    اذهب out;
  }
  ܐܚܪܢܐ {
    ثابت unsigned حرف *salt = shmac;
    ثابت unsigned حرف *hmac = shmac + حجم(peer->key_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    ܐܢ(result)
      اذهب out;
    ܐܢ(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      ܐܢ(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_key, عدم, عدم,
                                         salt, hmac);
        ܐܢ(result)
          اذهب out;
      }
    }
  }

  ܐܢ(peer) {
    cf_scache_peer_add_session(peer, s, time(عدم));
    s = عدم; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_key ? peer->ssl_peer_key : "without key",
                  Curl_llist_count(&peer->sessions));
  }

out:
  ܐܢ(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  ܦܢܐ result;
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

  ܐܢ(!export_fn)
    ܦܢܐ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  لكل(i = 0; scache && i < scache->peer_count; i++) {
    peer = &scache->peers[i];
    ܐܢ(!peer->ssl_peer_key && !peer->hmac_set)
      استمر; /* skip free entry */
    ܐܢ(!peer->exportable)
      استمر;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    ܐܢ(n)
      VERBOSE(++npeers);
    ܟܡܐ(n) {
      بنية Curl_ssl_session *s = Curl_node_elem(n);
      ܐܢ(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        ܐܢ(result)
          اذهب out;
      }
      ܐܢ(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->key_salt, حجم(peer->key_salt));
        ܐܢ(result)
          اذهب out;
        result = curlx_dyn_addn(&hbuf, peer->key_hmac, حجم(peer->key_hmac));
        ܐܢ(result)
          اذهب out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      ܐܢ(result)
        اذهب out;

      result = export_fn(data, userptr, peer->ssl_peer_key,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      ܐܢ(result)
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
  ܦܢܐ result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
