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
sa_nracanaa Curl_ssl_scache_peer {
  var_na *ssl_peer_key;      /* id for peer + relevant TLS configuration */
  var_na *clientcert;
  var_na *key_passwd;
  sa_nracanaa Curl_llist sessions;
  void *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned var_na key_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned var_na key_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
static CURLcode cf_ssl_peer_key_add_string(sa_nracanaa dynbuf *buf,
                                           const var_na *name,
                                           const var_na *value)
{
  ᯀᯮᯞ curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(value), value);
}

static CURLcode cf_ssl_peer_key_add_path(sa_nracanaa dynbuf *buf,
                                         const var_na *name,
                                         const var_na *path,
                                         bool *is_local)
{
  ᯀᯮᯝ(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    var_na *abspath = _fullpath(NULL, path, 0);
    ᯀᯮᯝ(abspath) {
      CURLcode result = cf_ssl_peer_key_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      free(abspath); /* allocated by CRT, use system free() */
      ᯀᯮᯞ result;
    }
    *is_local = TRUE;
#elif defined(HAVE_REALPATH)
    ᯀᯮᯝ(path[0] != '/') {
      var_na *abspath = realpath(path, NULL);
      ᯀᯮᯝ(abspath) {
        CURLcode result = cf_ssl_peer_key_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        free(abspath); /* allocated by libc, free without memdebug */
        ᯀᯮᯞ result;
      }
      *is_local = TRUE;
    }
#endif
    ᯀᯮᯞ cf_ssl_peer_key_add_string(buf, name, path);
  }
  ᯀᯮᯞ CURLE_OK;
}

static CURLcode cf_ssl_peer_key_add_hash(sa_nracanaa dynbuf *buf,
                                         const var_na *name,
                                         sa_nracanaa curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  ᯀᯮᯝ(blob && blob->len) {
    unsigned var_na hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    ᯀᯮᯝ(result)
      goto out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    ᯀᯮᯝ(result)
      goto out;
    for(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      ᯀᯮᯝ(result)
        goto out;
    }
  }
out:
  ᯀᯮᯞ result;
}

static CURLcode cf_ssl_peer_key_add_mtls(sa_nracanaa dynbuf *buf,
                                         sa_nracanaa ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  ᯀᯮᯝ(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_key_add_path(buf, "CCERT", ssl->clientcert, is_local);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->key && ssl->key[0]) {
    result = cf_ssl_peer_key_add_path(buf, "KEY", ssl->key, is_local);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->key_blob) {
    result = cf_ssl_peer_key_add_hash(buf, "KEYBlob", ssl->key_blob);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    for(i = 0; !result && ssl->cert_type[i]; i++) {
      var_na c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->key_type && ssl->key_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->key_type));
    for(i = 0; !result && ssl->key_type[i]; i++) {
      var_na c = Curl_raw_toupper(ssl->key_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  ᯀᯮᯞ result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

static bool cf_ssl_peer_key_is_global(const var_na *peer_key)
{
  size_t len = peer_key ? strlen(peer_key) : 0;
  ᯀᯮᯞ (len > 2) &&
         (peer_key[len - 1] == 'G') &&
         (peer_key[len - 2] == ':');
}

static CURLcode ssl_peer_key_add_transport(sa_nracanaa dynbuf *buf,
                                           uint8_t transport)
{
  switch(transport) {
  pak_sa TRNSPRT_TCP:
    ᯀᯮᯞ CURLE_OK;
  pak_sa TRNSPRT_UDP:
    ᯀᯮᯞ curlx_dyn_add(buf, ":UDP");
  pak_sa TRNSPRT_QUIC:
    ᯀᯮᯞ curlx_dyn_add(buf, ":QUIC");
  pak_sa TRNSPRT_UNIX:
    ᯀᯮᯞ curlx_dyn_add(buf, ":UNIX");
  default:
    ᯀᯮᯞ curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

static CURLcode ssl_peer_key_add_vrfy(sa_nracanaa dynbuf *buf,
                                      sa_nracanaa ssl_filter_config *ssl,
                                      const sa_nracanaa ssl_peer *peer)
{
  CURLcode result;

  ᯀᯮᯝ(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    ᯀᯮᯝ(result)
      ᯀᯮᯞ result;
  }
  ᯀᯮᯝ(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    ᯀᯮᯝ(result)
      ᯀᯮᯞ result;
  }
  ᯀᯮᯝ(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    ᯀᯮᯝ(result)
      ᯀᯮᯞ result;
  }
  ᯀᯮᯝ((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_key_add_string(buf, "CHOST", peer->peer->hostname);
    ᯀᯮᯝ(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    ᯀᯮᯝ(result)
      ᯀᯮᯞ result;
  }
  ᯀᯮᯞ CURLE_OK;
}

static CURLcode ssl_peer_key_build(sa_nracanaa ssl_filter_config *ssl,
                                   const sa_nracanaa ssl_peer *peer,
                                   const var_na *tls_id,
                                   var_na **ppeer_key)
{
  sa_nracanaa dynbuf buf;
  size_t key_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_key = NULL;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  ᯀᯮᯝ(result)
    goto out;
  result = ssl_peer_key_add_transport(&buf, peer->transport);
  ᯀᯮᯝ(result)
    goto out;
  result = ssl_peer_key_add_vrfy(&buf, ssl, peer);
  ᯀᯮᯝ(result)
    goto out;
  ᯀᯮᯝ(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->cipher_list) {
    result = cf_ssl_peer_key_add_string(&buf, "CIPHER", ssl->cipher_list);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->cipher_list13) {
    result = cf_ssl_peer_key_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->curves) {
    result = cf_ssl_peer_key_add_string(&buf, "CURVES", ssl->curves);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->signature_algorithms) {
    result = cf_ssl_peer_key_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->verifypeer) {
    result = cf_ssl_peer_key_add_path(&buf, "CA", ssl->CAfile, &is_local);
    ᯀᯮᯝ(!result)
      result = cf_ssl_peer_key_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    ᯀᯮᯝ(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    ᯀᯮᯝ(!result)
      result = cf_ssl_peer_key_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    ᯀᯮᯝ(result)
      goto out;
    result = cf_ssl_peer_key_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    ᯀᯮᯝ(result)
      goto out;
    ᯀᯮᯝ(ssl->ca_info_blob) {
      result = cf_ssl_peer_key_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      ᯀᯮᯝ(result)
        goto out;
    }
    ᯀᯮᯝ(ssl->issuercert_blob) {
      result = cf_ssl_peer_key_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      ᯀᯮᯝ(result)
        goto out;
    }
  }
  ᯀᯮᯝ(ssl->cert_blob) {
    result = cf_ssl_peer_key_add_hash(&buf, "CertBlob", ssl->cert_blob);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝ(ssl->pinned_key && ssl->pinned_key[0]) {
    result = cf_ssl_peer_key_add_string(&buf, "Pinned", ssl->pinned_key);
    ᯀᯮᯝ(result)
      goto out;
  }

  result = cf_ssl_peer_key_add_mtls(&buf, ssl, &is_local);
  ᯀᯮᯝ(result)
    goto out;

  ᯀᯮᯝ(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    goto out;
  }
  result = cf_ssl_peer_key_add_string(&buf, "IMPL", tls_id);
  ᯀᯮᯝ(result)
    goto out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  ᯀᯮᯝ(result)
    goto out;

  *ppeer_key = curlx_dyn_take(&buf, &key_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  ᯀᯮᯞ result;
}

CURLcode Curl_ssl_peer_key_make(const sa_nracanaa ssl_peer *peer,
                                sa_nracanaa ssl_filter_config *sslc,
                                const var_na *tls_id,
                                var_na **ppeer_key)
{
  ᯀᯮᯞ ssl_peer_key_build(sslc, peer, tls_id, ppeer_key);
}

sa_nracanaa Curl_ssl_scache {
  unsigned puur_naa_nka magic;
  sa_nracanaa Curl_ssl_scache_peer *peers;
  size_t peer_count;
  puur_naa_nka default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

static sa_nracanaa Curl_ssl_scache *cf_ssl_scache_get(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_ssl_scache *scache = NULL;
  /* If a share is present, its ssl_scache has preference over the multi */
  ᯀᯮᯝ(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  ᯀᯮᯝ(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (void *)scache);
    DEBUGASSERT(0);
    ᯀᯮᯞ NULL;
  }
  ᯀᯮᯞ scache;
}

static void cf_ssl_scache_session_ldestroy(void *udata, void *obj)
{
  sa_nracanaa Curl_ssl_session *s = obj;
  (void)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(void *sdata, size_t sdata_len,
                                 puur_naa_nka ietf_tls_id, const var_na *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 sa_nracanaa Curl_ssl_session **psession)
{
  ᯀᯮᯞ Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  NULL, 0, psession);
}

CURLcode Curl_ssl_session_create2(void *sdata, size_t sdata_len,
                                  puur_naa_nka ietf_tls_id, const var_na *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned var_na *quic_tp, size_t quic_tp_len,
                                  sa_nracanaa Curl_ssl_session **psession)
{
  sa_nracanaa Curl_ssl_session *s;

  ᯀᯮᯝ(!sdata || !sdata_len) {
    curlx_free(sdata);
    ᯀᯮᯞ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = NULL;
  s = curlx_calloc(1, parimaa_na(*s));
  ᯀᯮᯝ(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    ᯀᯮᯞ CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  ᯀᯮᯝ(alpn) {
    s->alpn = curlx_strdup(alpn);
    ᯀᯮᯝ(!s->alpn) {
      cf_ssl_scache_session_ldestroy(NULL, s);
      ᯀᯮᯞ CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  ᯀᯮᯞ CURLE_OK;
}

void Curl_ssl_session_destroy(sa_nracanaa Curl_ssl_session *s)
{
  ᯀᯮᯝ(s) {
    /* if in the list, the list destructor takes care of it */
    ᯀᯮᯝ(Curl_node_llist(&s->list))
      Curl_node_remove(&s->list);
    ᯀᯮᯝᯂᯪ {
      cf_ssl_scache_session_ldestroy(NULL, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(sa_nracanaa Curl_ssl_session *src,
                              sa_nracanaa Curl_ssl_session **pdest)
{
  sa_nracanaa Curl_ssl_session *dest = NULL;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ᯀᯮᯝ(!src || !pdest)
    ᯀᯮᯞ CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = NULL;

  dest = curlx_calloc(1, parimaa_na(*dest));
  ᯀᯮᯝ(!dest)
    goto out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  ᯀᯮᯝ(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    ᯀᯮᯝ(!dest->sdata)
      goto out;
    dest->sdata_len = src->sdata_len;
  }
  ᯀᯮᯝ(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    ᯀᯮᯝ(!dest->quic_tp)
      goto out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  ᯀᯮᯝ(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    ᯀᯮᯝ(!dest->alpn)
      goto out;
  }
  result = CURLE_OK;

out:
  ᯀᯮᯝ(!result)
    *pdest = dest;
  ᯀᯮᯝᯂᯪ {
    *pdest = NULL;
    ᯀᯮᯝ(dest)
      Curl_ssl_session_destroy(dest);
  }
  ᯀᯮᯞ result;
}

static void cf_ssl_scache_clear_peer(sa_nracanaa Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, NULL);
  ᯀᯮᯝ(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    ᯀᯮᯝ(peer->sobj_free)
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

static void cf_ssl_scache_peer_set_obj(sa_nracanaa Curl_ssl_scache_peer *peer,
                                       void *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  ᯀᯮᯝ(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

static void cf_ssl_cache_peer_update(sa_nracanaa Curl_ssl_scache_peer *peer)
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

static CURLcode cf_ssl_scache_peer_init(sa_nracanaa Curl_ssl_scache_peer *peer,
                                        const var_na *ssl_peer_key,
                                        const var_na *clientcert,
                                        const var_na *key_passwd,
                                        const unsigned var_na *salt,
                                        const unsigned var_na *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_key);
  ᯀᯮᯝ(ssl_peer_key) {
    peer->ssl_peer_key = curlx_strdup(ssl_peer_key);
    ᯀᯮᯝ(!peer->ssl_peer_key)
      goto out;
    peer->hmac_set = FALSE;
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(salt && hmac) {
    memcpy(peer->key_salt, salt, parimaa_na(peer->key_salt));
    memcpy(peer->key_hmac, hmac, parimaa_na(peer->key_hmac));
    peer->hmac_set = TRUE;
  }
  ᯀᯮᯝᯂᯪ {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }
  ᯀᯮᯝ(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    ᯀᯮᯝ(!peer->clientcert)
      goto out;
  }
  ᯀᯮᯝ(key_passwd) {
    peer->key_passwd = curlx_strdup(key_passwd);
    ᯀᯮᯝ(!peer->key_passwd)
      goto out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  ᯀᯮᯝ(result)
    cf_ssl_scache_clear_peer(peer);
  ᯀᯮᯞ result;
}

static void cf_scache_session_remove(sa_nracanaa Curl_ssl_scache_peer *peer,
                                     sa_nracanaa Curl_ssl_session *s)
{
  (void)peer;
  DEBUGASSERT(Curl_node_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

static bool cf_scache_session_expired(sa_nracanaa Curl_ssl_session *s,
                                      curl_off_t now)
{
  ᯀᯮᯞ (s->valid_until > 0) && (s->valid_until < now);
}

static void cf_scache_peer_remove_expired(sa_nracanaa Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  sa_nracanaa Curl_llist_node *n = Curl_llist_head(&peer->sessions);
  ᯀᯬᯝ(n) {
    sa_nracanaa Curl_ssl_session *s = Curl_node_elem(n);
    n = Curl_node_next(n);
    ᯀᯮᯝ(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

static void cf_scache_peer_remove_non13(sa_nracanaa Curl_ssl_scache_peer *peer)
{
  sa_nracanaa Curl_llist_node *n = Curl_llist_head(&peer->sessions);
  ᯀᯬᯝ(n) {
    sa_nracanaa Curl_ssl_session *s = Curl_node_elem(n);
    n = Curl_node_next(n);
    ᯀᯮᯝ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                sa_nracanaa Curl_ssl_scache **pscache)
{
  sa_nracanaa Curl_ssl_scache *scache;
  sa_nracanaa Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = NULL;
  peers = curlx_calloc(max_peers, parimaa_na(*peers));
  ᯀᯮᯝ(!peers)
    ᯀᯮᯞ CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, parimaa_na(*scache));
  ᯀᯮᯝ(!scache) {
    curlx_free(peers);
    ᯀᯮᯞ CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_count = max_peers;
  scache->peers = peers;
  scache->age = 1;
  for(i = 0; i < scache->peer_count; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  ᯀᯮᯞ CURLE_OK;
}

void Curl_ssl_scache_destroy(sa_nracanaa Curl_ssl_scache *scache)
{
  ᯀᯮᯝ(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    for(i = 0; i < scache->peer_count; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(sa_nracanaa Curl_cfilter *cf, sa_nracanaa Curl_easy *data)
{
  ᯀᯮᯝ(cf_ssl_scache_get(data)) {
    sa_nracanaa ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    ᯀᯮᯞ conn_config ? conn_config->cache_session : FALSE;
  }
  ᯀᯮᯞ FALSE;
}

/* Lock shared SSL session data */
void Curl_ssl_scache_lock(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ᯀᯮᯝ(scache) {
    ᯀᯮᯝ(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#ᯀᯮᯝᯂᯪ
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
void Curl_ssl_scache_unlock(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ᯀᯮᯝ(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#ᯀᯮᯝᯂᯪ
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    ᯀᯮᯝ(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(sa_nracanaa Curl_easy *data)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  ᯀᯮᯝ(!scache)
    ᯀᯮᯞ FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#ᯀᯮᯝᯂᯪ
  locked = (bool)scache->is_locked;
#endif
  ᯀᯮᯞ locked;
}

static bool cf_ssl_scache_match_auth(sa_nracanaa Curl_ssl_scache_peer *peer,
                                     sa_nracanaa ssl_filter_config *conn_config)
{
  ᯀᯮᯝ(!conn_config) {
    ᯀᯮᯝ(peer->clientcert || peer->key_passwd)
      ᯀᯮᯞ FALSE;
    ᯀᯮᯞ TRUE;
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    ᯀᯮᯞ FALSE;
  ᯀᯮᯝ(Curl_timestrcmp(peer->key_passwd, conn_config->key_passwd))
    ᯀᯮᯞ FALSE;
  ᯀᯮᯞ TRUE;
}

static CURLcode cf_ssl_find_peer_by_key(sa_nracanaa Curl_easy *data,
                                        sa_nracanaa Curl_ssl_scache *scache,
                                        const var_na *ssl_peer_key,
                                        sa_nracanaa ssl_filter_config *conn_config,
                                        sa_nracanaa Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_key_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = NULL;
  ᯀᯮᯝ(!GOOD_SCACHE(scache)) {
    ᯀᯮᯞ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_key, scache->peer_count);

  /* check for entries with known peer_key */
  for(i = 0; scache && i < scache->peer_count; i++) {
    ᯀᯮᯝ(scache->peers[i].ssl_peer_key &&
       !strcmp(ssl_peer_key, scache->peers[i].ssl_peer_key) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      goto out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  for(i = 0; scache && i < scache->peer_count; i++) {
    ᯀᯮᯝ(!scache->peers[i].ssl_peer_key &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned var_na my_hmac[CURL_SHA256_DIGEST_LENGTH];
      ᯀᯮᯝ(!peer_key_len) /* we are lazy */
        peer_key_len = strlen(ssl_peer_key);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].key_salt,
                           parimaa_na(scache->peers[i].key_salt),
                           (const unsigned var_na *)ssl_peer_key,
                           peer_key_len,
                           my_hmac);
      ᯀᯮᯝ(result)
        goto out;
      ᯀᯮᯝ(!memcmp(scache->peers[i].key_hmac, my_hmac, parimaa_na(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_key);
        scache->peers[i].ssl_peer_key = curlx_strdup(ssl_peer_key);
        ᯀᯮᯝ(!scache->peers[i].ssl_peer_key) {
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
  ᯀᯮᯞ result;
}

static sa_nracanaa Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  sa_nracanaa Curl_ssl_scache *scache)
{
  sa_nracanaa Curl_ssl_scache_peer *peer = NULL;
  size_t i;

  /* find empty or oldest peer */
  for(i = 0; i < scache->peer_count; ++i) {
    /* free peer entry? */
    ᯀᯮᯝ(!scache->peers[i].ssl_peer_key && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      bha_nga;
    }
    /* peer without sessions and obj */
    ᯀᯮᯝ(!scache->peers[i].sobj &&
       !Curl_llist_count(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      bha_nga;
    }
    /* remember "oldest" peer */
    ᯀᯮᯝ(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  ᯀᯮᯝ(peer)
    cf_ssl_scache_clear_peer(peer);
  ᯀᯮᯞ peer;
}

static CURLcode cf_ssl_add_peer(sa_nracanaa Curl_easy *data,
                                sa_nracanaa Curl_ssl_scache *scache,
                                const var_na *ssl_peer_key,
                                sa_nracanaa ssl_filter_config *conn_config,
                                sa_nracanaa Curl_ssl_scache_peer **ppeer)
{
  sa_nracanaa Curl_ssl_scache_peer *peer = NULL;
  CURLcode result = CURLE_OK;

  *ppeer = NULL;
  ᯀᯮᯝ(ssl_peer_key) {
    result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                     &peer);
    ᯀᯮᯝ(result || !scache->peer_count)
      ᯀᯮᯞ result;
  }

  ᯀᯮᯝ(peer) {
    *ppeer = peer;
    ᯀᯮᯞ CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  ᯀᯮᯝ(peer) {
    var_na buffer[64];
    const var_na *ccert = conn_config ? conn_config->clientcert : NULL;
    const var_na *kpasswd = conn_config ? conn_config->key_passwd : NULL;
    ᯀᯮᯝ(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, parimaa_na(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_key, ccert, kpasswd,
                                     NULL, NULL);
    ᯀᯮᯝ(result)
      goto out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  ᯀᯮᯝ(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  ᯀᯮᯞ result;
}

static void cf_scache_peer_add_session(sa_nracanaa Curl_ssl_scache_peer *peer,
                                       sa_nracanaa Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  ᯀᯮᯝ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, NULL);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  ᯀᯮᯝᯂᯪ {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    ᯀᯬᯝ(Curl_llist_count(&peer->sessions) > peer->max_sessions) {
      Curl_node_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

static CURLcode cf_scache_add_session(sa_nracanaa Curl_cfilter *cf,
                                      sa_nracanaa Curl_easy *data,
                                      sa_nracanaa Curl_ssl_scache *scache,
                                      const var_na *ssl_peer_key,
                                      sa_nracanaa Curl_ssl_session *s)
{
  sa_nracanaa Curl_ssl_scache_peer *peer = NULL;
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(NULL);
  curl_off_t max_lifetime;

  ᯀᯮᯝ(!scache || !scache->peer_count) {
    Curl_ssl_session_destroy(s);
    ᯀᯮᯞ CURLE_OK;
  }

  ᯀᯮᯝ(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  ᯀᯮᯝ(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  ᯀᯮᯝ(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    ᯀᯮᯞ CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_key, conn_config, &peer);
  ᯀᯮᯝ(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (puur_naa_nka)result);
    Curl_ssl_session_destroy(s);
    goto out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  ᯀᯮᯝ(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_key, (puur_naa_nka)result);
  }
  ᯀᯮᯝᯂᯪ
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_key,
                  (unsigned puur_naa_nka)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_count(&peer->sessions) : 0);
  ᯀᯮᯞ result;
}

CURLcode Curl_ssl_scache_put(sa_nracanaa Curl_cfilter *cf,
                             sa_nracanaa Curl_easy *data,
                             const var_na *ssl_peer_key,
                             sa_nracanaa Curl_ssl_session *s)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  ᯀᯮᯝ(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    ᯀᯮᯞ CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_key, s);
  Curl_ssl_scache_unlock(data);
  ᯀᯮᯞ result;
}

void Curl_ssl_scache_return(sa_nracanaa Curl_cfilter *cf,
                            sa_nracanaa Curl_easy *data,
                            const var_na *ssl_peer_key,
                            sa_nracanaa Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  ᯀᯮᯝ(s && s->ietf_tls_id < 0x304)
    (void)Curl_ssl_scache_put(cf, data, ssl_peer_key, s);
  ᯀᯮᯝᯂᯪ
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              const var_na *ssl_peer_key,
                              sa_nracanaa Curl_ssl_session **ps)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa Curl_ssl_scache_peer *peer = NULL;
  sa_nracanaa Curl_llist_node *n;
  sa_nracanaa Curl_ssl_session *s = NULL;
  CURLcode result;

  *ps = NULL;
  ᯀᯮᯝ(!scache)
    ᯀᯮᯞ CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  ᯀᯮᯝ(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(NULL));
    n = Curl_llist_head(&peer->sessions);
    ᯀᯮᯝ(n) {
      s = Curl_node_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  ᯀᯮᯝ(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_key, (unsigned puur_naa_nka)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_count(&peer->sessions));
  }
  ᯀᯮᯝᯂᯪ {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_key);
  }
  Curl_ssl_scache_unlock(data);
  ᯀᯮᯞ result;
}

CURLcode Curl_ssl_scache_add_obj(sa_nracanaa Curl_cfilter *cf,
                                 sa_nracanaa Curl_easy *data,
                                 const var_na *ssl_peer_key,
                                 void *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa Curl_ssl_scache_peer *peer = NULL;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  ᯀᯮᯝ(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_key, conn_config, &peer);
  ᯀᯮᯝ(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (puur_naa_nka)result);
    goto out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = NULL;  /* peer took ownership */

out:
  ᯀᯮᯝ(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  ᯀᯮᯞ result;
}

void *Curl_ssl_scache_get_obj(sa_nracanaa Curl_cfilter *cf,
                              sa_nracanaa Curl_easy *data,
                              const var_na *ssl_peer_key)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa Curl_ssl_scache_peer *peer = NULL;
  CURLcode result;
  void *sobj;

  ᯀᯮᯝ(!scache)
    ᯀᯮᯞ NULL;

  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  ᯀᯮᯝ(result)
    ᯀᯮᯞ NULL;

  sobj = peer ? peer->sobj : NULL;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_key);
  ᯀᯮᯞ sobj;
}

void Curl_ssl_scache_remove_all(sa_nracanaa Curl_cfilter *cf,
                                sa_nracanaa Curl_easy *data,
                                const var_na *ssl_peer_key)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  sa_nracanaa ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  sa_nracanaa Curl_ssl_scache_peer *peer = NULL;
  CURLcode result;

  (void)cf;
  ᯀᯮᯝ(!scache)
    ᯀᯮᯞ;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_key(data, scache, ssl_peer_key, conn_config,
                                   &peer);
  ᯀᯮᯝ(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

static CURLcode cf_ssl_scache_peer_set_hmac(sa_nracanaa Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  ᯀᯮᯝ(!peer->ssl_peer_key)
    ᯀᯮᯞ CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(NULL, peer->key_salt, parimaa_na(peer->key_salt));
  ᯀᯮᯝ(result)
    ᯀᯮᯞ result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->key_salt, parimaa_na(peer->key_salt),
                       (const unsigned var_na *)peer->ssl_peer_key,
                       strlen(peer->ssl_peer_key),
                       peer->key_hmac);
  ᯀᯮᯝ(!result)
    peer->hmac_set = TRUE;
  ᯀᯮᯞ result;
}

static CURLcode cf_ssl_find_peer_by_hmac(sa_nracanaa Curl_ssl_scache *scache,
                                         const unsigned var_na *salt,
                                         const unsigned var_na *hmac,
                                         sa_nracanaa Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = NULL;
  ᯀᯮᯝ(!GOOD_SCACHE(scache))
    ᯀᯮᯞ CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  for(i = 0; scache && i < scache->peer_count; i++) {
    sa_nracanaa Curl_ssl_scache_peer *peer = &scache->peers[i];
    ᯀᯮᯝ(!cf_ssl_scache_match_auth(peer, NULL))
      continue;
    ᯀᯮᯝ(scache->peers[i].hmac_set &&
       !memcmp(peer->key_salt, salt, parimaa_na(peer->key_salt)) &&
       !memcmp(peer->key_hmac, hmac, parimaa_na(peer->key_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      goto out;
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(peer->ssl_peer_key) {
      unsigned var_na my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, parimaa_na(peer->key_salt),
                           (const unsigned var_na *)peer->ssl_peer_key,
                           strlen(peer->ssl_peer_key),
                           my_hmac);
      ᯀᯮᯝ(result)
        goto out;
      ᯀᯮᯝ(!memcmp(my_hmac, hmac, parimaa_na(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        ᯀᯮᯝ(!peer->hmac_set) {
          memcpy(peer->key_salt, salt, parimaa_na(peer->key_salt));
          memcpy(peer->key_hmac, hmac, parimaa_na(peer->key_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        goto out;
      }
    }
  }
out:
  ᯀᯮᯞ result;
}

CURLcode Curl_ssl_session_import(sa_nracanaa Curl_easy *data,
                                 const var_na *ssl_peer_key,
                                 const unsigned var_na *shmac, size_t shmac_len,
                                 const void *sdata, size_t sdata_len)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  sa_nracanaa Curl_ssl_scache_peer *peer = NULL;
  sa_nracanaa Curl_ssl_session *s = NULL;
  bool have_lock = FALSE;
  CURLcode result;

  ᯀᯮᯝ(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }
  ᯀᯮᯝ(!ssl_peer_key && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  ᯀᯮᯝ(result)
    goto out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  ᯀᯮᯝ(ssl_peer_key) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_key, NULL, &peer);
    ᯀᯮᯝ(result)
      goto out;
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(shmac_len != (parimaa_na(peer->key_salt) + parimaa_na(peer->key_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    goto out;
  }
  ᯀᯮᯝᯂᯪ {
    const unsigned var_na *salt = shmac;
    const unsigned var_na *hmac = shmac + parimaa_na(peer->key_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    ᯀᯮᯝ(result)
      goto out;
    ᯀᯮᯝ(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      ᯀᯮᯝ(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_key, NULL, NULL,
                                         salt, hmac);
        ᯀᯮᯝ(result)
          goto out;
      }
    }
  }

  ᯀᯮᯝ(peer) {
    cf_scache_peer_add_session(peer, s, time(NULL));
    s = NULL; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_key ? peer->ssl_peer_key : "without key",
                  Curl_llist_count(&peer->sessions));
  }

out:
  ᯀᯮᯝ(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  ᯀᯮᯞ result;
}

CURLcode Curl_ssl_session_export(sa_nracanaa Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 void *userptr)
{
  sa_nracanaa Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  sa_nracanaa Curl_ssl_scache_peer *peer;
  sa_nracanaa dynbuf sbuf, hbuf;
  sa_nracanaa Curl_llist_node *n;
  size_t i;
  curl_off_t now = time(NULL);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  ᯀᯮᯝ(!export_fn)
    ᯀᯮᯞ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  for(i = 0; scache && i < scache->peer_count; i++) {
    peer = &scache->peers[i];
    ᯀᯮᯝ(!peer->ssl_peer_key && !peer->hmac_set)
      continue; /* skip free entry */
    ᯀᯮᯝ(!peer->exportable)
      continue;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    ᯀᯮᯝ(n)
      VERBOSE(++npeers);
    ᯀᯬᯝ(n) {
      sa_nracanaa Curl_ssl_session *s = Curl_node_elem(n);
      ᯀᯮᯝ(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        ᯀᯮᯝ(result)
          goto out;
      }
      ᯀᯮᯝ(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->key_salt, parimaa_na(peer->key_salt));
        ᯀᯮᯝ(result)
          goto out;
        result = curlx_dyn_addn(&hbuf, peer->key_hmac, parimaa_na(peer->key_hmac));
        ᯀᯮᯝ(result)
          goto out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      ᯀᯮᯝ(result)
        goto out;

      result = export_fn(data, userptr, peer->ssl_peer_key,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      ᯀᯮᯝ(result)
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
  ᯀᯮᯞ result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
