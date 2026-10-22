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
#include "curl_setup.h"

#ifdef USE_SSL

#include "urldata.h"
#include "cfilters.h"

#include "vtls/vtls.h" /* generic SSL protos etc */
#include "vtls/vtls_int.h"
#include "vtls/vtls_scache.h"
#include "vtls/vtls_spack.h"

#include "strcase.h"
#include "url.h"
#include "llist.h"
#include "curl_share.h"
#include "curl_threads.h"
#include "curl_trc.h"
#include "curl_sha256.h"
#include "rand.h"
#include "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
struct Curl_ssl_scache_peer {
  char *ssl_peer_key;      /* id for peer + relevant TLS configuration */
  char *clientcert;
  char *key_passwd;
  struct Curl_llist sessions;
  void *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned char key_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned char key_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
static CURLcode cf_ssl_peer_key_add_string(struct dynbuf *buf,
                                           const char *name,
                                           const char *value)
{
  вернуть curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(value), value);
}

static CURLcode cf_ssl_peer_key_add_path(struct dynbuf *buf,
                                         const char *name,
                                         const char *path,
                                         bool *is_local)
{
  если(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    char *abspath = _fullpath(NULL, path, 0);
    если(abspath) {
      CURLcode result = cf_ssl_peer_key_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      free(abspath); /* allocated by CRT, use system free() */
      вернуть result;
    }
    *is_local = TRUE;
#elif defined(HAVE_REALPATH)
    если(path[0] != '/') {
      char *abspath = realpath(path, NULL);
      если(abspath) {
        CURLcode result = cf_ssl_peer_key_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        free(abspath); /* allocated by libc, free without memdebug */
        вернуть result;
      }
      *is_local = TRUE;
    }
#endif
    вернуть cf_ssl_peer_key_add_string(buf, name, path);
  }
  вернуть CURLE_OK;
}

static CURLcode cf_ssl_peer_key_add_hash(struct dynbuf *buf,
                                         const char *name,
                                         struct curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  если(blob && blob->len) {
    unsigned char hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    если(result)
      goto out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    если(result)
      goto out;
    для(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      если(result)
        goto out;
    }
  }
out:
  вернуть result;
}

static CURLcode cf_ssl_peer_key_add_mtls(struct dynbuf *buf,
                                         struct ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  если(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_key_add_path(buf, "CCERT", ssl->clientcert, is_local);
    если(result)
      goto out;
  }
  если(ssl->key && ssl->key[0]) {
    result = cf_ssl_peer_key_add_path(buf, "KEY", ssl->key, is_local);
    если(result)
      goto out;
  }
  если(ssl->key_blob) {
    result = cf_ssl_peer_key_add_hash(buf, "KEYBlob", ssl->key_blob);
    если(result)
      goto out;
  }
  если(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    для(i = 0; !result && ssl->cert_type[i]; i++) {
      char c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    если(result)
      goto out;
  }
  если(ssl->key_type && ssl->key_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->key_type));
    для(i = 0; !result && ssl->key_type[i]; i++) {
      char c = Curl_raw_toupper(ssl->key_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  вернуть result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

static bool cf_ssl_peer_key_is_global(const char *peer_key)
{
  size_t len = peer_key ? strlen(peer_key) : 0;
  вернуть (len > 2) &&
         (peer_key[len - 1] == 'G') &&
         (peer_key[len - 2] == ':');
}

static CURLcode ssl_peer_key_add_transport(struct dynbuf *buf,
                                           uint8_t transport)
{
  switch(transport) {
  case TRNSPRT_TCP:
    вернуть CURLE_OK;
  case TRNSPRT_UDP:
    вернуть curlx_dyn_add(buf, ":UDP");
  case TRNSPRT_QUIC:
    вернуть curlx_dyn_add(buf, ":QUIC");
  case TRNSPRT_UNIX:
    вернуть curlx_dyn_add(buf, ":UNIX");
  default:
    вернуть curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

static CURLcode ssl_peer_key_add_vrfy(struct dynbuf *buf,
                                      struct ssl_filter_config *ssl,
                                      const struct ssl_peer *peer)
{
  CURLcode result;

  если(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    если(result)
      вернуть result;
  }
  если(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    если(result)
      вернуть result;
  }
  если(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    если(result)
      вернуть result;
  }
  если((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_key_add_string(buf, "CHOST", peer->peer->hostname);
    если(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    если(result)
      вернуть result;
  }
  вернуть CURLE_OK;
}

static CURLcode ssl_peer_key_build(struct ssl_filter_config *ssl,
                                   const struct ssl_peer *peer,
                                   const char *tls_id,
                                   char **ppeer_key)
{
  struct dynbuf buf;
  size_t key_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_key = NULL;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  если(result)
    goto out;
  result = ssl_peer_key_add_transport(&buf, peer->transport);
  если(result)
    goto out;
  result = ssl_peer_key_add_vrfy(&buf, ssl, peer);
  если(result)
    goto out;
  если(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    если(result)
      goto out;
  }
  если(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    если(result)
      goto out;
  }
  если(ssl->cipher_list) {
    result = cf_ssl_peer_key_add_string(&buf, "CIPHER", ssl->cipher_list);
    если(result)
      goto out;
  }
  если(ssl->cipher_list13) {
    result = cf_ssl_peer_key_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    если(result)
      goto out;
  }
  если(ssl->curves) {
    result = cf_ssl_peer_key_add_string(&buf, "CURVES", ssl->curves);
    если(result)
      goto out;
  }
  если(ssl->signature_algorithms) {
    result = cf_ssl_peer_key_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    если(result)
      goto out;
  }
  если(ssl->verifypeer) {
    result = cf_ssl_peer_key_add_path(&buf, "CA", ssl->CAfile, &is_local);
    если(!result)
      result = cf_ssl_peer_key_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    если(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    если(!result)
      result = cf_ssl_peer_key_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    если(result)
      goto out;
    result = cf_ssl_peer_key_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    если(result)
      goto out;
    если(ssl->ca_info_blob) {
      result = cf_ssl_peer_key_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      если(result)
        goto out;
    }
    если(ssl->issuercert_blob) {
      result = cf_ssl_peer_key_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      если(result)
        goto out;
    }
  }
  если(ssl->cert_blob) {
    result = cf_ssl_peer_key_add_hash(&buf, "CertBlob", ssl->cert_blob);
    если(result)
      goto out;
  }
  если(ssl->pinned_key && ssl->pinned_key[0]) {
    result = cf_ssl_peer_key_add_string(&buf, "Pinned", ssl->pinned_key);
    если(result)
      goto out;
  }

  result = cf_ssl_peer_key_add_mtls(&buf, ssl, &is_local);
  если(result)
    goto out;

  если(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    goto out;
  }
  result = cf_ssl_peer_key_add_string(&buf, "IMPL", tls_id);
  если(result)
    goto out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  если(result)
    goto out;

  *ppeer_key = curlx_dyn_take(&buf, &key_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  вернуть result;
}

CURLcode Curl_ssl_peer_key_make(const struct ssl_peer *peer,
                                struct ssl_filter_config *sslc,
                                const char *tls_id,
                                char **ppeer_key)
{
  вернуть ssl_peer_key_build(sslc, peer, tls_id, ppeer_key);
}

struct Curl_ssl_scache {
  unsigned целое magic;
  struct Curl_ssl_scache_peer *peers;
  size_t peer_count;
  целое default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

static struct Curl_ssl_scache *cf_ssl_scache_get(struct Curl_easy *data)
{
  struct Curl_ssl_scache *scache = NULL;
  /* If a share is present, its ssl_scache has preference over the multi */
  если(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  иначе если(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  если(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (void *)scache);
    DEBUGASSERT(0);
    вернуть NULL;
  }
  вернуть scache;
}

static void cf_ssl_scache_session_ldestroy(void *udata, void *obj)
{
  struct Curl_ssl_session *s = obj;
  (void)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(void *sdata, size_t sdata_len,
                                 целое ietf_tls_id, const char *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 struct Curl_ssl_session **psession)
{
  вернуть Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  NULL, 0, psession);
}

CURLcode Curl_ssl_session_create2(void *sdata, size_t sdata_len,
                                  целое ietf_tls_id, const char *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned char *quic_tp, size_t quic_tp_len,
                                  struct Curl_ssl_session **psession)
{
  struct Curl_ssl_session *s;

  если(!sdata || !sdata_len) {
    curlx_free(sdata);
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = NULL;
  s = curlx_calloc(1, sizeof(*s));
  если(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    вернуть CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  если(alpn) {
    s->alpn = curlx_strdup(alpn);
    если(!s->alpn) {
      cf_ssl_scache_session_ldestroy(NULL, s);
      вернуть CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  вернуть CURLE_OK;
}

void Curl_ssl_session_destroy(struct Curl_ssl_session *s)
{
  если(s) {
    /* if in the list, the list destructor takes care of it */
    если(Curl_node_llist(&s->list))
      Curl_node_remove(&s->list);
    иначе {
      cf_ssl_scache_session_ldestroy(NULL, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(struct Curl_ssl_session *src,
                              struct Curl_ssl_session **pdest)
{
  struct Curl_ssl_session *dest = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  если(!src || !pdest)
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = NULL;

  dest = curlx_calloc(1, sizeof(*dest));
  если(!dest)
    goto out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  если(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    если(!dest->sdata)
      goto out;
    dest->sdata_len = src->sdata_len;
  }
  если(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    если(!dest->quic_tp)
      goto out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  если(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    если(!dest->alpn)
      goto out;
  }
  result = CURLE_OK;

out:
  если(!result)
    *pdest = dest;
  иначе {
    *pdest = NULL;
    если(dest)
      Curl_ssl_session_destroy(dest);
  }
  вернуть result;
}

static void cf_ssl_scache_clear_peer(struct Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, NULL);
  если(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    если(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = NULL;
  }
  peer->sobj_free = NULL;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->key_passwd);
  curlx_safefree(peer->ssl_peer_key);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

static void cf_ssl_scache_peer_set_obj(struct Curl_ssl_scache_peer *peer,
                                       void *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  если(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

static void cf_ssl_cache_peer_update(struct Curl_ssl_scache_peer *peer)
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

static CURLcode cf_ssl_scache_peer_init(struct Curl_ssl_scache_peer *peer,
                                        const char *ssl_peer_key,
                                        const char *clientcert,
                                        const char *key_passwd,
                                        const unsigned char *salt,
                                        const unsigned char *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_key);
  если(ssl_peer_key) {
    peer->ssl_peer_key = curlx_strdup(ssl_peer_key);
    если(!peer->ssl_peer_key)
      goto out;
    peer->hmac_set = FALSE;
  }
  иначе если(salt && hmac) {
    memcpy(peer->key_salt, salt, sizeof(peer->key_salt));
    memcpy(peer->key_hmac, hmac, sizeof(peer->key_hmac));
    peer->hmac_set = TRUE;
  }
  иначе {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }
  если(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    если(!peer->clientcert)
      goto out;
  }
  если(key_passwd) {
    peer->key_passwd = curlx_strdup(key_passwd);
    если(!peer->key_passwd)
      goto out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  если(result)
    cf_ssl_scache_clear_peer(peer);
  вернуть result;
}

static void cf_scache_session_remove(struct Curl_ssl_scache_peer *peer,
                                     struct Curl_ssl_session *s)
{
  (void)peer;
  DEBUGASSERT(Curl_node_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

static bool cf_scache_session_expired(struct Curl_ssl_session *s,
                                      curl_off_t now)
{
  вернуть (s->valid_until > 0) && (s->valid_until < now);
}

static void cf_scache_peer_remove_expired(struct Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  struct Curl_llist_node *n = Curl_llist_head(&peer->sessions);
  пока(n) {
    struct Curl_ssl_session *s = Curl_node_elem(n);
    n = Curl_node_next(n);
    если(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

static void cf_scache_peer_remove_non13(struct Curl_ssl_scache_peer *peer)
{
  struct Curl_llist_node *n = Curl_llist_head(&peer->sessions);
  пока(n) {
    struct Curl_ssl_session *s = Curl_node_elem(n);
    n = Curl_node_next(n);
    если(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                struct Curl_ssl_scache **pscache)
{
  struct Curl_ssl_scache *scache;
  struct Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = NULL;
  peers = curlx_calloc(max_peers, sizeof(*peers));
  если(!peers)
    вернуть CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, sizeof(*scache));
  если(!scache) {
    curlx_free(peers);
    вернуть CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_count = max_peers;
  scache->peers = peers;
  scache->age = 1;
  для(i = 0; i < scache->peer_count; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  вернуть CURLE_OK;
}

void Curl_ssl_scache_destroy(struct Curl_ssl_scache *scache)
{
  если(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    для(i = 0; i < scache->peer_count; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(struct Curl_cfilter *cf, struct Curl_easy *data)
{
  если(cf_ssl_scache_get(data)) {
    struct ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    вернуть conn_config ? conn_config->cache_session : FALSE;
  }
  вернуть FALSE;
}

/* Lock shared SSL session data */
void Curl_ssl_scache_lock(struct Curl_easy *data)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  если(scache) {
    если(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#иначе
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
void Curl_ssl_scache_unlock(struct Curl_easy *data)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  если(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#иначе
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    если(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(struct Curl_easy *data)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  если(!scache)
    вернуть FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#иначе
  locked = (bool)scache->is_locked;
#endif
  вернуть locked;
}

static bool cf_ssl_scache_match_auth(struct Curl_ssl_scache_peer *peer,
                                     struct ssl_filter_config *conn_config)
{
  если(!conn_config) {
    если(peer->clientcert || peer->key_passwd)
      вернуть FALSE;
    вернуть TRUE;
  }
  иначе если(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    вернуть FALSE;
  если(Curl_timestrcmp(peer->key_passwd, conn_config->key_passwd))
    вернуть FALSE;
  вернуть TRUE;
}

static CURLcode cf_ssl_find_peer_by_key(struct Curl_easy *data,
                                        struct Curl_ssl_scache *scache,
                                        const char *ssl_peer_key,
                                        struct ssl_filter_config *conn_config,
                                        struct Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_key_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = NULL;
  если(!GOOD_SCACHE(scache)) {
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_key, scache->peer_count);

  /* check for entries with known peer_key */
  для(i = 0; scache && i < scache->peer_count; i++) {
    если(scache->peers[i].ssl_peer_key &&
       !strcmp(ssl_peer_key, scache->peers[i].ssl_peer_key) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      goto out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  для(i = 0; scache && i < scache->peer_count; i++) {
    если(!scache->peers[i].ssl_peer_key &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned char my_hmac[CURL_SHA256_DIGEST_LENGTH];
      если(!peer_key_len) /* we are lazy */
        peer_key_len = strlen(ssl_peer_key);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].key_salt,
                           sizeof(scache->peers[i].key_salt),
                           (const unsigned char *)ssl_peer_key,
                           peer_key_len,
                           my_hmac);
      если(result)
        goto out;
      если(!memcmp(scache->peers[i].key_hmac, my_hmac, sizeof(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_key);
        scache->peers[i].ssl_peer_key = curlx_strdup(ssl_peer_key);
        если(!scache->peers[i].ssl_peer_key) {
          result = CURLE_OUT_OF_MEMORY;
          goto out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        goto out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_key);
out:
  вернуть result;
}

static struct Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  struct Curl_ssl_scache *scache)
{
  struct Curl_ssl_scache_peer *peer = NULL;
  size_t i;

  /* find empty or oldest peer */
  для(i = 0; i < scache->peer_count; ++i) {
    /* free peer entry? */
    если(!scache->peers[i].ssl_peer_key && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      break;
    }
    /* peer without sessions and obj */
    если(!scache->peers[i].sobj &&
       !Curl_llist_count(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      break;
    }
    /* remember "oldest" peer */
    если(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  если(peer)
    cf_ssl_scache_clear_peer(peer);
  вернуть peer;
}

static CURLcode cf_ssl_add_peer(struct Curl_easy *data,
                                struct Curl_ssl_scache *scache,
                                const char *ssl_peer_key,
                                struct ssl_filter_config *conn_config,
                                struct Curl_ssl_scache_peer **ppeer)
{
  struct Curl_ssl_scache_peer *peer = NULL;
  CURLcode result = CURLE_OK;

  *ppeer = NULL;
  если(ssl_peer_key) {
    result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                     &peer);
    если(result || !scache->peer_count)
      вернуть result;
  }

  если(peer) {
    *ppeer = peer;
    вернуть CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  если(peer) {
    char buffer[64];
    const char *ccert = conn_config ? conn_config->clientcert : NULL;
    const char *kpasswd = conn_config ? conn_config->key_passwd : NULL;
    если(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, sizeof(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_key, ccert, kpasswd,
                                     NULL, NULL);
    если(result)
      goto out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  если(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  вернуть result;
}

static void cf_scache_peer_add_session(struct Curl_ssl_scache_peer *peer,
                                       struct Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  если(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, NULL);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  иначе {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    пока(Curl_llist_count(&peer->sessions) > peer->max_sessions) {
      Curl_node_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

static CURLcode cf_scache_add_session(struct Curl_cfilter *cf,
                                      struct Curl_easy *data,
                                      struct Curl_ssl_scache *scache,
                                      const char *ssl_peer_key,
                                      struct Curl_ssl_session *s)
{
  struct Curl_ssl_scache_peer *peer = NULL;
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(NULL);
  curl_off_t max_lifetime;

  если(!scache || !scache->peer_count) {
    Curl_ssl_session_destroy(s);
    вернуть CURLE_OK;
  }

  если(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  если(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  если(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    вернуть CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_key, conn_config, &peer);
  если(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (целое)result);
    Curl_ssl_session_destroy(s);
    goto out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  если(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_key, (целое)result);
  }
  иначе
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_key,
                  (unsigned целое)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_count(&peer->sessions) : 0);
  вернуть result;
}

CURLcode Curl_ssl_scache_put(struct Curl_cfilter *cf,
                             struct Curl_easy *data,
                             const char *ssl_peer_key,
                             struct Curl_ssl_session *s)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  если(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    вернуть CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_key, s);
  Curl_ssl_scache_unlock(data);
  вернуть result;
}

void Curl_ssl_scache_return(struct Curl_cfilter *cf,
                            struct Curl_easy *data,
                            const char *ssl_peer_key,
                            struct Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  если(s && s->ietf_tls_id < 0x304)
    (void)Curl_ssl_scache_put(cf, data, ssl_peer_key, s);
  иначе
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(struct Curl_cfilter *cf,
                              struct Curl_easy *data,
                              const char *ssl_peer_key,
                              struct Curl_ssl_session **ps)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  struct Curl_ssl_scache_peer *peer = NULL;
  struct Curl_llist_node *n;
  struct Curl_ssl_session *s = NULL;
  CURLcode result;

  *ps = NULL;
  если(!scache)
    вернуть CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  если(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(NULL));
    n = Curl_llist_head(&peer->sessions);
    если(n) {
      s = Curl_node_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  если(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_key, (unsigned целое)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_count(&peer->sessions));
  }
  иначе {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_key);
  }
  Curl_ssl_scache_unlock(data);
  вернуть result;
}

CURLcode Curl_ssl_scache_add_obj(struct Curl_cfilter *cf,
                                 struct Curl_easy *data,
                                 const char *ssl_peer_key,
                                 void *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  struct Curl_ssl_scache_peer *peer = NULL;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  если(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_key, conn_config, &peer);
  если(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (целое)result);
    goto out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = NULL;  /* peer took ownership */

out:
  если(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  вернуть result;
}

void *Curl_ssl_scache_get_obj(struct Curl_cfilter *cf,
                              struct Curl_easy *data,
                              const char *ssl_peer_key)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  struct Curl_ssl_scache_peer *peer = NULL;
  CURLcode result;
  void *sobj;

  если(!scache)
    вернуть NULL;

  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  если(result)
    вернуть NULL;

  sobj = peer ? peer->sobj : NULL;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_key);
  вернуть sobj;
}

void Curl_ssl_scache_remove_all(struct Curl_cfilter *cf,
                                struct Curl_easy *data,
                                const char *ssl_peer_key)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  struct ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  struct Curl_ssl_scache_peer *peer = NULL;
  CURLcode result;

  (void)cf;
  если(!scache)
    вернуть;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  если(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

static CURLcode cf_ssl_scache_peer_set_hmac(struct Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  если(!peer->ssl_peer_key)
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(NULL, peer->key_salt, sizeof(peer->key_salt));
  если(result)
    вернуть result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->key_salt, sizeof(peer->key_salt),
                       (const unsigned char *)peer->ssl_peer_key,
                       strlen(peer->ssl_peer_key),
                       peer->key_hmac);
  если(!result)
    peer->hmac_set = TRUE;
  вернуть result;
}

static CURLcode cf_ssl_find_peer_by_hmac(struct Curl_ssl_scache *scache,
                                         const unsigned char *salt,
                                         const unsigned char *hmac,
                                         struct Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = NULL;
  если(!GOOD_SCACHE(scache))
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  для(i = 0; scache && i < scache->peer_count; i++) {
    struct Curl_ssl_scache_peer *peer = &scache->peers[i];
    если(!cf_ssl_scache_match_auth(peer, NULL))
      continue;
    если(scache->peers[i].hmac_set &&
       !memcmp(peer->key_salt, salt, sizeof(peer->key_salt)) &&
       !memcmp(peer->key_hmac, hmac, sizeof(peer->key_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      goto out;
    }
    иначе если(peer->ssl_peer_key) {
      unsigned char my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, sizeof(peer->key_salt),
                           (const unsigned char *)peer->ssl_peer_key,
                           strlen(peer->ssl_peer_key),
                           my_hmac);
      если(result)
        goto out;
      если(!memcmp(my_hmac, hmac, sizeof(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        если(!peer->hmac_set) {
          memcpy(peer->key_salt, salt, sizeof(peer->key_salt));
          memcpy(peer->key_hmac, hmac, sizeof(peer->key_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        goto out;
      }
    }
  }
out:
  вернуть result;
}

CURLcode Curl_ssl_session_import(struct Curl_easy *data,
                                 const char *ssl_peer_key,
                                 const unsigned char *shmac, size_t shmac_len,
                                 const void *sdata, size_t sdata_len)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  struct Curl_ssl_scache_peer *peer = NULL;
  struct Curl_ssl_session *s = NULL;
  bool have_lock = FALSE;
  CURLcode result;

  если(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }
  если(!ssl_peer_key && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  если(result)
    goto out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  если(ssl_peer_key) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_key, NULL, &peer);
    если(result)
      goto out;
  }
  иначе если(shmac_len != (sizeof(peer->key_salt) + sizeof(peer->key_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }
  иначе {
    const unsigned char *salt = shmac;
    const unsigned char *hmac = shmac + sizeof(peer->key_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    если(result)
      goto out;
    если(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      если(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_key, NULL, NULL,
                                         salt, hmac);
        если(result)
          goto out;
      }
    }
  }

  если(peer) {
    cf_scache_peer_add_session(peer, s, time(NULL));
    s = NULL; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_key ? peer->ssl_peer_key : "without key",
                  Curl_llist_count(&peer->sessions));
  }

out:
  если(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  вернуть result;
}

CURLcode Curl_ssl_session_export(struct Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 void *userptr)
{
  struct Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  struct Curl_ssl_scache_peer *peer;
  struct dynbuf sbuf, hbuf;
  struct Curl_llist_node *n;
  size_t i;
  curl_off_t now = time(NULL);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  если(!export_fn)
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  для(i = 0; scache && i < scache->peer_count; i++) {
    peer = &scache->peers[i];
    если(!peer->ssl_peer_key && !peer->hmac_set)
      continue; /* skip free entry */
    если(!peer->exportable)
      continue;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    если(n)
      VERBOSE(++npeers);
    пока(n) {
      struct Curl_ssl_session *s = Curl_node_elem(n);
      если(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        если(result)
          goto out;
      }
      если(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->key_salt, sizeof(peer->key_salt));
        если(result)
          goto out;
        result = curlx_dyn_addn(&hbuf, peer->key_hmac, sizeof(peer->key_hmac));
        если(result)
          goto out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      если(result)
        goto out;

      result = export_fn(data, userptr, peer->ssl_peer_key,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      если(result)
        goto out;
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
  вернуть result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
