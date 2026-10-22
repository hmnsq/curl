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
#شامل "curl_setup.h"

#ifdef USE_SSL

#شامل "urldata.h"
#شامل "cfilters.h"

#شامل "vtls/vtls.h" /* generic SSL protos etc */
#شامل "vtls/vtls_int.h"
#شامل "vtls/vtls_scache.h"
#شامل "vtls/vtls_spack.h"

#شامل "strcase.h"
#شامل "url.h"
#شامل "llist.h"
#شامل "curl_share.h"
#شامل "curl_threads.h"
#شامل "curl_trc.h"
#شامل "curl_sha256.h"
#شامل "rand.h"
#شامل "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
ساختار Curl_ssl_scache_peer {
  نویسه *ssl_peer_کلید;      /* id for peer + relevant TLS configuration */
  نویسه *clientcert;
  نویسه *کلید_passwd;
  ساختار Curl_llist sessions;
  تهی *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned نویسه کلید_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned نویسه کلید_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ایستا CURLcode cf_ssl_peer_کلید_add_string(ساختار dynbuf *buf,
                                           ثابت نویسه *name,
                                           ثابت نویسه *مقدار)
{
  بازگشت curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(مقدار), مقدار);
}

ایستا CURLcode cf_ssl_peer_کلید_add_path(ساختار dynbuf *buf,
                                         ثابت نویسه *name,
                                         ثابت نویسه *path,
                                         bool *is_local)
{
  اگر(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    نویسه *abspath = _fullpath(پوچ, path, 0);
    اگر(abspath) {
      CURLcode result = cf_ssl_peer_کلید_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      آزاد(abspath); /* allocated by CRT, use system free() */
      بازگشت result;
    }
    *is_local = TRUE;
#وگرنهاگر defined(HAVE_REALPATH)
    اگر(path[0] != '/') {
      نویسه *abspath = realpath(path, پوچ);
      اگر(abspath) {
        CURLcode result = cf_ssl_peer_کلید_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        آزاد(abspath); /* allocated by libc, free without memdebug */
        بازگشت result;
      }
      *is_local = TRUE;
    }
#endif
    بازگشت cf_ssl_peer_کلید_add_string(buf, name, path);
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode cf_ssl_peer_کلید_add_hash(ساختار dynbuf *buf,
                                         ثابت نویسه *name,
                                         ساختار curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  اگر(blob && blob->len) {
    unsigned نویسه hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    اگر(result)
      برو out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    اگر(result)
      برو out;
    برای(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      اگر(result)
        برو out;
    }
  }
out:
  بازگشت result;
}

ایستا CURLcode cf_ssl_peer_کلید_add_mtls(ساختار dynbuf *buf,
                                         ساختار ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  اگر(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_کلید_add_path(buf, "CCERT", ssl->clientcert, is_local);
    اگر(result)
      برو out;
  }
  اگر(ssl->کلید && ssl->کلید[0]) {
    result = cf_ssl_peer_کلید_add_path(buf, "KEY", ssl->کلید, is_local);
    اگر(result)
      برو out;
  }
  اگر(ssl->کلید_blob) {
    result = cf_ssl_peer_کلید_add_hash(buf, "KEYBlob", ssl->کلید_blob);
    اگر(result)
      برو out;
  }
  اگر(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    برای(i = 0; !result && ssl->cert_type[i]; i++) {
      نویسه c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    اگر(result)
      برو out;
  }
  اگر(ssl->کلید_type && ssl->کلید_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->کلید_type));
    برای(i = 0; !result && ssl->کلید_type[i]; i++) {
      نویسه c = Curl_raw_toupper(ssl->کلید_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  بازگشت result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

ایستا bool cf_ssl_peer_کلید_is_global(ثابت نویسه *peer_کلید)
{
  size_t len = peer_کلید ? strlen(peer_کلید) : 0;
  بازگشت (len > 2) &&
         (peer_کلید[len - 1] == 'G') &&
         (peer_کلید[len - 2] == ':');
}

ایستا CURLcode ssl_peer_کلید_add_transport(ساختار dynbuf *buf,
                                           uint8_t transport)
{
  انتخاب(transport) {
  حالت TRNSPRT_TCP:
    بازگشت CURLE_OK;
  حالت TRNSPRT_UDP:
    بازگشت curlx_dyn_add(buf, ":UDP");
  حالت TRNSPRT_QUIC:
    بازگشت curlx_dyn_add(buf, ":QUIC");
  حالت TRNSPRT_UNIX:
    بازگشت curlx_dyn_add(buf, ":UNIX");
  پیشفرض:
    بازگشت curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ایستا CURLcode ssl_peer_کلید_add_vrfy(ساختار dynbuf *buf,
                                      ساختار ssl_filter_config *ssl,
                                      ثابت ساختار ssl_peer *peer)
{
  CURLcode result;

  اگر(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    اگر(result)
      بازگشت result;
  }
  اگر(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    اگر(result)
      بازگشت result;
  }
  اگر(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    اگر(result)
      بازگشت result;
  }
  اگر((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_کلید_add_string(buf, "CHOST", peer->peer->hostname);
    اگر(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    اگر(result)
      بازگشت result;
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode ssl_peer_کلید_build(ساختار ssl_filter_config *ssl,
                                   ثابت ساختار ssl_peer *peer,
                                   ثابت نویسه *tls_id,
                                   نویسه **ppeer_کلید)
{
  ساختار dynbuf buf;
  size_t کلید_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_کلید = پوچ;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  اگر(result)
    برو out;
  result = ssl_peer_کلید_add_transport(&buf, peer->transport);
  اگر(result)
    برو out;
  result = ssl_peer_کلید_add_vrfy(&buf, ssl, peer);
  اگر(result)
    برو out;
  اگر(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    اگر(result)
      برو out;
  }
  اگر(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    اگر(result)
      برو out;
  }
  اگر(ssl->cipher_list) {
    result = cf_ssl_peer_کلید_add_string(&buf, "CIPHER", ssl->cipher_list);
    اگر(result)
      برو out;
  }
  اگر(ssl->cipher_list13) {
    result = cf_ssl_peer_کلید_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    اگر(result)
      برو out;
  }
  اگر(ssl->curves) {
    result = cf_ssl_peer_کلید_add_string(&buf, "CURVES", ssl->curves);
    اگر(result)
      برو out;
  }
  اگر(ssl->signature_algorithms) {
    result = cf_ssl_peer_کلید_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    اگر(result)
      برو out;
  }
  اگر(ssl->verifypeer) {
    result = cf_ssl_peer_کلید_add_path(&buf, "CA", ssl->CAfile, &is_local);
    اگر(!result)
      result = cf_ssl_peer_کلید_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    اگر(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    اگر(!result)
      result = cf_ssl_peer_کلید_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    اگر(result)
      برو out;
    result = cf_ssl_peer_کلید_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    اگر(result)
      برو out;
    اگر(ssl->ca_info_blob) {
      result = cf_ssl_peer_کلید_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      اگر(result)
        برو out;
    }
    اگر(ssl->issuercert_blob) {
      result = cf_ssl_peer_کلید_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      اگر(result)
        برو out;
    }
  }
  اگر(ssl->cert_blob) {
    result = cf_ssl_peer_کلید_add_hash(&buf, "CertBlob", ssl->cert_blob);
    اگر(result)
      برو out;
  }
  اگر(ssl->pinned_کلید && ssl->pinned_کلید[0]) {
    result = cf_ssl_peer_کلید_add_string(&buf, "Pinned", ssl->pinned_کلید);
    اگر(result)
      برو out;
  }

  result = cf_ssl_peer_کلید_add_mtls(&buf, ssl, &is_local);
  اگر(result)
    برو out;

  اگر(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    برو out;
  }
  result = cf_ssl_peer_کلید_add_string(&buf, "IMPL", tls_id);
  اگر(result)
    برو out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  اگر(result)
    برو out;

  *ppeer_کلید = curlx_dyn_take(&buf, &کلید_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  بازگشت result;
}

CURLcode Curl_ssl_peer_کلید_make(ثابت ساختار ssl_peer *peer,
                                ساختار ssl_filter_config *sslc,
                                ثابت نویسه *tls_id,
                                نویسه **ppeer_کلید)
{
  بازگشت ssl_peer_کلید_build(sslc, peer, tls_id, ppeer_کلید);
}

ساختار Curl_ssl_scache {
  unsigned صحیح magic;
  ساختار Curl_ssl_scache_peer *peers;
  size_t peer_شمار;
  صحیح default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

ایستا ساختار Curl_ssl_scache *cf_ssl_scache_get(ساختار Curl_easy *data)
{
  ساختار Curl_ssl_scache *scache = پوچ;
  /* If a share is present, its ssl_scache has preference over the multi */
  اگر(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  وگرنه اگر(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  اگر(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (تهی *)scache);
    DEBUGASSERT(0);
    بازگشت پوچ;
  }
  بازگشت scache;
}

ایستا تهی cf_ssl_scache_session_ldestroy(تهی *udata, تهی *obj)
{
  ساختار Curl_ssl_session *s = obj;
  (تهی)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(تهی *sdata, size_t sdata_len,
                                 صحیح ietf_tls_id, ثابت نویسه *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 ساختار Curl_ssl_session **psession)
{
  بازگشت Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  پوچ, 0, psession);
}

CURLcode Curl_ssl_session_create2(تهی *sdata, size_t sdata_len,
                                  صحیح ietf_tls_id, ثابت نویسه *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned نویسه *quic_tp, size_t quic_tp_len,
                                  ساختار Curl_ssl_session **psession)
{
  ساختار Curl_ssl_session *s;

  اگر(!sdata || !sdata_len) {
    curlx_free(sdata);
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = پوچ;
  s = curlx_calloc(1, اندازه(*s));
  اگر(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  اگر(alpn) {
    s->alpn = curlx_strdup(alpn);
    اگر(!s->alpn) {
      cf_ssl_scache_session_ldestroy(پوچ, s);
      بازگشت CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  بازگشت CURLE_OK;
}

تهی Curl_ssl_session_destroy(ساختار Curl_ssl_session *s)
{
  اگر(s) {
    /* if in the list, the list destructor takes care of it */
    اگر(Curl_گره_llist(&s->list))
      Curl_گره_remove(&s->list);
    وگرنه {
      cf_ssl_scache_session_ldestroy(پوچ, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(ساختار Curl_ssl_session *src,
                              ساختار Curl_ssl_session **pdest)
{
  ساختار Curl_ssl_session *dest = پوچ;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  اگر(!src || !pdest)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = پوچ;

  dest = curlx_calloc(1, اندازه(*dest));
  اگر(!dest)
    برو out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  اگر(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    اگر(!dest->sdata)
      برو out;
    dest->sdata_len = src->sdata_len;
  }
  اگر(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    اگر(!dest->quic_tp)
      برو out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  اگر(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    اگر(!dest->alpn)
      برو out;
  }
  result = CURLE_OK;

out:
  اگر(!result)
    *pdest = dest;
  وگرنه {
    *pdest = پوچ;
    اگر(dest)
      Curl_ssl_session_destroy(dest);
  }
  بازگشت result;
}

ایستا تهی cf_ssl_scache_clear_peer(ساختار Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, پوچ);
  اگر(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    اگر(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = پوچ;
  }
  peer->sobj_free = پوچ;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->کلید_passwd);
  curlx_safefree(peer->ssl_peer_کلید);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

ایستا تهی cf_ssl_scache_peer_set_obj(ساختار Curl_ssl_scache_peer *peer,
                                       تهی *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  اگر(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

ایستا تهی cf_ssl_cache_peer_update(ساختار Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->کلید_passwd &&
                      (!peer->ssl_peer_کلید ||
                       cf_ssl_peer_کلید_is_global(peer->ssl_peer_کلید)));
}

ایستا CURLcode cf_ssl_scache_peer_init(ساختار Curl_ssl_scache_peer *peer,
                                        ثابت نویسه *ssl_peer_کلید,
                                        ثابت نویسه *clientcert,
                                        ثابت نویسه *کلید_passwd,
                                        ثابت unsigned نویسه *salt,
                                        ثابت unsigned نویسه *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_کلید);
  اگر(ssl_peer_کلید) {
    peer->ssl_peer_کلید = curlx_strdup(ssl_peer_کلید);
    اگر(!peer->ssl_peer_کلید)
      برو out;
    peer->hmac_set = FALSE;
  }
  وگرنه اگر(salt && hmac) {
    memcpy(peer->کلید_salt, salt, اندازه(peer->کلید_salt));
    memcpy(peer->کلید_hmac, hmac, اندازه(peer->کلید_hmac));
    peer->hmac_set = TRUE;
  }
  وگرنه {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو out;
  }
  اگر(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    اگر(!peer->clientcert)
      برو out;
  }
  اگر(کلید_passwd) {
    peer->کلید_passwd = curlx_strdup(کلید_passwd);
    اگر(!peer->کلید_passwd)
      برو out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  اگر(result)
    cf_ssl_scache_clear_peer(peer);
  بازگشت result;
}

ایستا تهی cf_scache_session_remove(ساختار Curl_ssl_scache_peer *peer,
                                     ساختار Curl_ssl_session *s)
{
  (تهی)peer;
  DEBUGASSERT(Curl_گره_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

ایستا bool cf_scache_session_expired(ساختار Curl_ssl_session *s,
                                      curl_off_t now)
{
  بازگشت (s->valid_until > 0) && (s->valid_until < now);
}

ایستا تهی cf_scache_peer_remove_expired(ساختار Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  ساختار Curl_llist_گره *n = Curl_llist_head(&peer->sessions);
  وقتی(n) {
    ساختار Curl_ssl_session *s = Curl_گره_elem(n);
    n = Curl_گره_next(n);
    اگر(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ایستا تهی cf_scache_peer_remove_non13(ساختار Curl_ssl_scache_peer *peer)
{
  ساختار Curl_llist_گره *n = Curl_llist_head(&peer->sessions);
  وقتی(n) {
    ساختار Curl_ssl_session *s = Curl_گره_elem(n);
    n = Curl_گره_next(n);
    اگر(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                ساختار Curl_ssl_scache **pscache)
{
  ساختار Curl_ssl_scache *scache;
  ساختار Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = پوچ;
  peers = curlx_calloc(max_peers, اندازه(*peers));
  اگر(!peers)
    بازگشت CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, اندازه(*scache));
  اگر(!scache) {
    curlx_free(peers);
    بازگشت CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_شمار = max_peers;
  scache->peers = peers;
  scache->age = 1;
  برای(i = 0; i < scache->peer_شمار; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  بازگشت CURLE_OK;
}

تهی Curl_ssl_scache_destroy(ساختار Curl_ssl_scache *scache)
{
  اگر(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    برای(i = 0; i < scache->peer_شمار; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(ساختار Curl_cfilter *cf, ساختار Curl_easy *data)
{
  اگر(cf_ssl_scache_get(data)) {
    ساختار ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    بازگشت conn_config ? conn_config->cache_session : FALSE;
  }
  بازگشت FALSE;
}

/* Lock shared SSL session data */
تهی Curl_ssl_scache_lock(ساختار Curl_easy *data)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  اگر(scache) {
    اگر(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#وگرنه
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
تهی Curl_ssl_scache_unlock(ساختار Curl_easy *data)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  اگر(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#وگرنه
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    اگر(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(ساختار Curl_easy *data)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  اگر(!scache)
    بازگشت FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#وگرنه
  locked = (bool)scache->is_locked;
#endif
  بازگشت locked;
}

ایستا bool cf_ssl_scache_match_auth(ساختار Curl_ssl_scache_peer *peer,
                                     ساختار ssl_filter_config *conn_config)
{
  اگر(!conn_config) {
    اگر(peer->clientcert || peer->کلید_passwd)
      بازگشت FALSE;
    بازگشت TRUE;
  }
  وگرنه اگر(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    بازگشت FALSE;
  اگر(Curl_timestrcmp(peer->کلید_passwd, conn_config->کلید_passwd))
    بازگشت FALSE;
  بازگشت TRUE;
}

ایستا CURLcode cf_ssl_find_peer_by_کلید(ساختار Curl_easy *data,
                                        ساختار Curl_ssl_scache *scache,
                                        ثابت نویسه *ssl_peer_کلید,
                                        ساختار ssl_filter_config *conn_config,
                                        ساختار Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_کلید_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = پوچ;
  اگر(!GOOD_SCACHE(scache)) {
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_کلید, scache->peer_شمار);

  /* check for entries with known peer_key */
  برای(i = 0; scache && i < scache->peer_شمار; i++) {
    اگر(scache->peers[i].ssl_peer_کلید &&
       !strcmp(ssl_peer_کلید, scache->peers[i].ssl_peer_کلید) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      برو out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  برای(i = 0; scache && i < scache->peer_شمار; i++) {
    اگر(!scache->peers[i].ssl_peer_کلید &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned نویسه my_hmac[CURL_SHA256_DIGEST_LENGTH];
      اگر(!peer_کلید_len) /* we are lazy */
        peer_کلید_len = strlen(ssl_peer_کلید);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].کلید_salt,
                           اندازه(scache->peers[i].کلید_salt),
                           (ثابت unsigned نویسه *)ssl_peer_کلید,
                           peer_کلید_len,
                           my_hmac);
      اگر(result)
        برو out;
      اگر(!memcmp(scache->peers[i].کلید_hmac, my_hmac, اندازه(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_کلید);
        scache->peers[i].ssl_peer_کلید = curlx_strdup(ssl_peer_کلید);
        اگر(!scache->peers[i].ssl_peer_کلید) {
          result = CURLE_OUT_OF_MEMORY;
          برو out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        برو out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_کلید);
out:
  بازگشت result;
}

ایستا ساختار Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  ساختار Curl_ssl_scache *scache)
{
  ساختار Curl_ssl_scache_peer *peer = پوچ;
  size_t i;

  /* find empty or oldest peer */
  برای(i = 0; i < scache->peer_شمار; ++i) {
    /* free peer entry? */
    اگر(!scache->peers[i].ssl_peer_کلید && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      بشکن;
    }
    /* peer without sessions and obj */
    اگر(!scache->peers[i].sobj &&
       !Curl_llist_شمار(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      بشکن;
    }
    /* remember "oldest" peer */
    اگر(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  اگر(peer)
    cf_ssl_scache_clear_peer(peer);
  بازگشت peer;
}

ایستا CURLcode cf_ssl_add_peer(ساختار Curl_easy *data,
                                ساختار Curl_ssl_scache *scache,
                                ثابت نویسه *ssl_peer_کلید,
                                ساختار ssl_filter_config *conn_config,
                                ساختار Curl_ssl_scache_peer **ppeer)
{
  ساختار Curl_ssl_scache_peer *peer = پوچ;
  CURLcode result = CURLE_OK;

  *ppeer = پوچ;
  اگر(ssl_peer_کلید) {
    result = cf_ssl_find_peer_by_کلید(data, scache, ssl_peer_کلید, conn_config,
                                     &peer);
    اگر(result || !scache->peer_شمار)
      بازگشت result;
  }

  اگر(peer) {
    *ppeer = peer;
    بازگشت CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  اگر(peer) {
    نویسه buffer[64];
    ثابت نویسه *ccert = conn_config ? conn_config->clientcert : پوچ;
    ثابت نویسه *kpasswd = conn_config ? conn_config->کلید_passwd : پوچ;
    اگر(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, اندازه(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_کلید, ccert, kpasswd,
                                     پوچ, پوچ);
    اگر(result)
      برو out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  اگر(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  بازگشت result;
}

ایستا تهی cf_scache_peer_add_session(ساختار Curl_ssl_scache_peer *peer,
                                       ساختار Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  اگر(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, پوچ);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  وگرنه {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    وقتی(Curl_llist_شمار(&peer->sessions) > peer->max_sessions) {
      Curl_گره_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

ایستا CURLcode cf_scache_add_session(ساختار Curl_cfilter *cf,
                                      ساختار Curl_easy *data,
                                      ساختار Curl_ssl_scache *scache,
                                      ثابت نویسه *ssl_peer_کلید,
                                      ساختار Curl_ssl_session *s)
{
  ساختار Curl_ssl_scache_peer *peer = پوچ;
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(پوچ);
  curl_off_t max_lifetime;

  اگر(!scache || !scache->peer_شمار) {
    Curl_ssl_session_destroy(s);
    بازگشت CURLE_OK;
  }

  اگر(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  اگر(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  اگر(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    بازگشت CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_کلید, conn_config, &peer);
  اگر(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (صحیح)result);
    Curl_ssl_session_destroy(s);
    برو out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  اگر(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_کلید, (صحیح)result);
  }
  وگرنه
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_کلید,
                  (unsigned صحیح)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_شمار(&peer->sessions) : 0);
  بازگشت result;
}

CURLcode Curl_ssl_scache_put(ساختار Curl_cfilter *cf,
                             ساختار Curl_easy *data,
                             ثابت نویسه *ssl_peer_کلید,
                             ساختار Curl_ssl_session *s)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  اگر(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    بازگشت CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_کلید, s);
  Curl_ssl_scache_unlock(data);
  بازگشت result;
}

تهی Curl_ssl_scache_return(ساختار Curl_cfilter *cf,
                            ساختار Curl_easy *data,
                            ثابت نویسه *ssl_peer_کلید,
                            ساختار Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  اگر(s && s->ietf_tls_id < 0x304)
    (تهی)Curl_ssl_scache_put(cf, data, ssl_peer_کلید, s);
  وگرنه
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              ثابت نویسه *ssl_peer_کلید,
                              ساختار Curl_ssl_session **ps)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار Curl_ssl_scache_peer *peer = پوچ;
  ساختار Curl_llist_گره *n;
  ساختار Curl_ssl_session *s = پوچ;
  CURLcode result;

  *ps = پوچ;
  اگر(!scache)
    بازگشت CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_کلید(data, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  اگر(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(پوچ));
    n = Curl_llist_head(&peer->sessions);
    اگر(n) {
      s = Curl_گره_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  اگر(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_کلید, (unsigned صحیح)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_شمار(&peer->sessions));
  }
  وگرنه {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_کلید);
  }
  Curl_ssl_scache_unlock(data);
  بازگشت result;
}

CURLcode Curl_ssl_scache_add_obj(ساختار Curl_cfilter *cf,
                                 ساختار Curl_easy *data,
                                 ثابت نویسه *ssl_peer_کلید,
                                 تهی *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار Curl_ssl_scache_peer *peer = پوچ;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  اگر(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_کلید, conn_config, &peer);
  اگر(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (صحیح)result);
    برو out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = پوچ;  /* peer took ownership */

out:
  اگر(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  بازگشت result;
}

تهی *Curl_ssl_scache_get_obj(ساختار Curl_cfilter *cf,
                              ساختار Curl_easy *data,
                              ثابت نویسه *ssl_peer_کلید)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار Curl_ssl_scache_peer *peer = پوچ;
  CURLcode result;
  تهی *sobj;

  اگر(!scache)
    بازگشت پوچ;

  result = cf_ssl_find_peer_by_کلید(data, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  اگر(result)
    بازگشت پوچ;

  sobj = peer ? peer->sobj : پوچ;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_کلید);
  بازگشت sobj;
}

تهی Curl_ssl_scache_remove_all(ساختار Curl_cfilter *cf,
                                ساختار Curl_easy *data,
                                ثابت نویسه *ssl_peer_کلید)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ساختار ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساختار Curl_ssl_scache_peer *peer = پوچ;
  CURLcode result;

  (تهی)cf;
  اگر(!scache)
    بازگشت;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_کلید(data, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  اگر(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

ایستا CURLcode cf_ssl_scache_peer_set_hmac(ساختار Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  اگر(!peer->ssl_peer_کلید)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(پوچ, peer->کلید_salt, اندازه(peer->کلید_salt));
  اگر(result)
    بازگشت result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->کلید_salt, اندازه(peer->کلید_salt),
                       (ثابت unsigned نویسه *)peer->ssl_peer_کلید,
                       strlen(peer->ssl_peer_کلید),
                       peer->کلید_hmac);
  اگر(!result)
    peer->hmac_set = TRUE;
  بازگشت result;
}

ایستا CURLcode cf_ssl_find_peer_by_hmac(ساختار Curl_ssl_scache *scache,
                                         ثابت unsigned نویسه *salt,
                                         ثابت unsigned نویسه *hmac,
                                         ساختار Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = پوچ;
  اگر(!GOOD_SCACHE(scache))
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  برای(i = 0; scache && i < scache->peer_شمار; i++) {
    ساختار Curl_ssl_scache_peer *peer = &scache->peers[i];
    اگر(!cf_ssl_scache_match_auth(peer, پوچ))
      ادامه;
    اگر(scache->peers[i].hmac_set &&
       !memcmp(peer->کلید_salt, salt, اندازه(peer->کلید_salt)) &&
       !memcmp(peer->کلید_hmac, hmac, اندازه(peer->کلید_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      برو out;
    }
    وگرنه اگر(peer->ssl_peer_کلید) {
      unsigned نویسه my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, اندازه(peer->کلید_salt),
                           (ثابت unsigned نویسه *)peer->ssl_peer_کلید,
                           strlen(peer->ssl_peer_کلید),
                           my_hmac);
      اگر(result)
        برو out;
      اگر(!memcmp(my_hmac, hmac, اندازه(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        اگر(!peer->hmac_set) {
          memcpy(peer->کلید_salt, salt, اندازه(peer->کلید_salt));
          memcpy(peer->کلید_hmac, hmac, اندازه(peer->کلید_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        برو out;
      }
    }
  }
out:
  بازگشت result;
}

CURLcode Curl_ssl_session_import(ساختار Curl_easy *data,
                                 ثابت نویسه *ssl_peer_کلید,
                                 ثابت unsigned نویسه *shmac, size_t shmac_len,
                                 ثابت تهی *sdata, size_t sdata_len)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ساختار Curl_ssl_scache_peer *peer = پوچ;
  ساختار Curl_ssl_session *s = پوچ;
  bool have_lock = FALSE;
  CURLcode result;

  اگر(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو out;
  }
  اگر(!ssl_peer_کلید && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  اگر(result)
    برو out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  اگر(ssl_peer_کلید) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_کلید, پوچ, &peer);
    اگر(result)
      برو out;
  }
  وگرنه اگر(shmac_len != (اندازه(peer->کلید_salt) + اندازه(peer->کلید_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    برو out;
  }
  وگرنه {
    ثابت unsigned نویسه *salt = shmac;
    ثابت unsigned نویسه *hmac = shmac + اندازه(peer->کلید_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    اگر(result)
      برو out;
    اگر(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      اگر(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_کلید, پوچ, پوچ,
                                         salt, hmac);
        اگر(result)
          برو out;
      }
    }
  }

  اگر(peer) {
    cf_scache_peer_add_session(peer, s, time(پوچ));
    s = پوچ; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_کلید ? peer->ssl_peer_کلید : "without key",
                  Curl_llist_شمار(&peer->sessions));
  }

out:
  اگر(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  بازگشت result;
}

CURLcode Curl_ssl_session_export(ساختار Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 تهی *userptr)
{
  ساختار Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ساختار Curl_ssl_scache_peer *peer;
  ساختار dynbuf sbuf, hbuf;
  ساختار Curl_llist_گره *n;
  size_t i;
  curl_off_t now = time(پوچ);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  اگر(!export_fn)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  برای(i = 0; scache && i < scache->peer_شمار; i++) {
    peer = &scache->peers[i];
    اگر(!peer->ssl_peer_کلید && !peer->hmac_set)
      ادامه; /* skip free entry */
    اگر(!peer->exportable)
      ادامه;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    اگر(n)
      VERBOSE(++npeers);
    وقتی(n) {
      ساختار Curl_ssl_session *s = Curl_گره_elem(n);
      اگر(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        اگر(result)
          برو out;
      }
      اگر(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->کلید_salt, اندازه(peer->کلید_salt));
        اگر(result)
          برو out;
        result = curlx_dyn_addn(&hbuf, peer->کلید_hmac, اندازه(peer->کلید_hmac));
        اگر(result)
          برو out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      اگر(result)
        برو out;

      result = export_fn(data, userptr, peer->ssl_peer_کلید,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      اگر(result)
        برو out;
      VERBOSE(++ntickets);
      n = Curl_گره_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  بازگشت result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
