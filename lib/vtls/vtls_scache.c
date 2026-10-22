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
#અન્તર્ભાવ "curl_setup.h"

#ifdef USE_SSL

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "cfilters.h"

#અન્તર્ભાવ "vtls/vtls.h" /* generic SSL protos etc */
#અન્તર્ભાવ "vtls/vtls_int.h"
#અન્તર્ભાવ "vtls/vtls_scache.h"
#અન્તર્ભાવ "vtls/vtls_spack.h"

#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "llist.h"
#અન્તર્ભાવ "curl_share.h"
#અન્તર્ભાવ "curl_threads.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "curl_sha256.h"
#અન્તર્ભાવ "rand.h"
#અન્તર્ભાવ "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
સંરચના Curl_ssl_scache_peer {
  અક્ષર *ssl_peer_ચાવી;      /* id for peer + relevant TLS configuration */
  અક્ષર *clientcert;
  અક્ષર *ચાવી_passwd;
  સંરચના Curl_llist sessions;
  શૂન્ય *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned અક્ષર ચાવી_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned અક્ષર ચાવી_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
સ્થિર CURLcode cf_ssl_peer_ચાવી_add_string(સંરચના dynbuf *buf,
                                           અચલ અક્ષર *name,
                                           અચલ અક્ષર *કિંમત)
{
  પાછા curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(કિંમત), કિંમત);
}

સ્થિર CURLcode cf_ssl_peer_ચાવી_add_path(સંરચના dynbuf *buf,
                                         અચલ અક્ષર *name,
                                         અચલ અક્ષર *path,
                                         bool *is_local)
{
  જો(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    અક્ષર *abspath = _fullpath(ખાલી, path, 0);
    જો(abspath) {
      CURLcode result = cf_ssl_peer_ચાવી_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      મુક્ત(abspath); /* allocated by CRT, use system free() */
      પાછા result;
    }
    *is_local = TRUE;
#અથવાજો defined(HAVE_REALPATH)
    જો(path[0] != '/') {
      અક્ષર *abspath = realpath(path, ખાલી);
      જો(abspath) {
        CURLcode result = cf_ssl_peer_ચાવી_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        મુક્ત(abspath); /* allocated by libc, free without memdebug */
        પાછા result;
      }
      *is_local = TRUE;
    }
#endif
    પાછા cf_ssl_peer_ચાવી_add_string(buf, name, path);
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode cf_ssl_peer_ચાવી_add_hash(સંરચના dynbuf *buf,
                                         અચલ અક્ષર *name,
                                         સંરચના curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  જો(blob && blob->len) {
    unsigned અક્ષર hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    જો(result)
      જાઓ out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    જો(result)
      જાઓ out;
    માટે(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      જો(result)
        જાઓ out;
    }
  }
out:
  પાછા result;
}

સ્થિર CURLcode cf_ssl_peer_ચાવી_add_mtls(સંરચના dynbuf *buf,
                                         સંરચના ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  જો(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_ચાવી_add_path(buf, "CCERT", ssl->clientcert, is_local);
    જો(result)
      જાઓ out;
  }
  જો(ssl->ચાવી && ssl->ચાવી[0]) {
    result = cf_ssl_peer_ચાવી_add_path(buf, "KEY", ssl->ચાવી, is_local);
    જો(result)
      જાઓ out;
  }
  જો(ssl->ચાવી_blob) {
    result = cf_ssl_peer_ચાવી_add_hash(buf, "KEYBlob", ssl->ચાવી_blob);
    જો(result)
      જાઓ out;
  }
  જો(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    માટે(i = 0; !result && ssl->cert_type[i]; i++) {
      અક્ષર c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    જો(result)
      જાઓ out;
  }
  જો(ssl->ચાવી_type && ssl->ચાવી_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->ચાવી_type));
    માટે(i = 0; !result && ssl->ચાવી_type[i]; i++) {
      અક્ષર c = Curl_raw_toupper(ssl->ચાવી_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  પાછા result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

સ્થિર bool cf_ssl_peer_ચાવી_is_global(અચલ અક્ષર *peer_ચાવી)
{
  size_t len = peer_ચાવી ? strlen(peer_ચાવી) : 0;
  પાછા (len > 2) &&
         (peer_ચાવી[len - 1] == 'G') &&
         (peer_ચાવી[len - 2] == ':');
}

સ્થિર CURLcode ssl_peer_ચાવી_add_transport(સંરચના dynbuf *buf,
                                           uint8_t transport)
{
  પસંદગી(transport) {
  સ્થિતિ TRNSPRT_TCP:
    પાછા CURLE_OK;
  સ્થિતિ TRNSPRT_UDP:
    પાછા curlx_dyn_add(buf, ":UDP");
  સ્થિતિ TRNSPRT_QUIC:
    પાછા curlx_dyn_add(buf, ":QUIC");
  સ્થિતિ TRNSPRT_UNIX:
    પાછા curlx_dyn_add(buf, ":UNIX");
  મૂળભૂત:
    પાછા curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

સ્થિર CURLcode ssl_peer_ચાવી_add_vrfy(સંરચના dynbuf *buf,
                                      સંરચના ssl_filter_config *ssl,
                                      અચલ સંરચના ssl_peer *peer)
{
  CURLcode result;

  જો(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    જો(result)
      પાછા result;
  }
  જો(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    જો(result)
      પાછા result;
  }
  જો(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    જો(result)
      પાછા result;
  }
  જો((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_ચાવી_add_string(buf, "CHOST", peer->peer->hostname);
    જો(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    જો(result)
      પાછા result;
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode ssl_peer_ચાવી_build(સંરચના ssl_filter_config *ssl,
                                   અચલ સંરચના ssl_peer *peer,
                                   અચલ અક્ષર *tls_id,
                                   અક્ષર **ppeer_ચાવી)
{
  સંરચના dynbuf buf;
  size_t ચાવી_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_ચાવી = ખાલી;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  જો(result)
    જાઓ out;
  result = ssl_peer_ચાવી_add_transport(&buf, peer->transport);
  જો(result)
    જાઓ out;
  result = ssl_peer_ચાવી_add_vrfy(&buf, ssl, peer);
  જો(result)
    જાઓ out;
  જો(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    જો(result)
      જાઓ out;
  }
  જો(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    જો(result)
      જાઓ out;
  }
  જો(ssl->cipher_list) {
    result = cf_ssl_peer_ચાવી_add_string(&buf, "CIPHER", ssl->cipher_list);
    જો(result)
      જાઓ out;
  }
  જો(ssl->cipher_list13) {
    result = cf_ssl_peer_ચાવી_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    જો(result)
      જાઓ out;
  }
  જો(ssl->curves) {
    result = cf_ssl_peer_ચાવી_add_string(&buf, "CURVES", ssl->curves);
    જો(result)
      જાઓ out;
  }
  જો(ssl->signature_algorithms) {
    result = cf_ssl_peer_ચાવી_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    જો(result)
      જાઓ out;
  }
  જો(ssl->verifypeer) {
    result = cf_ssl_peer_ચાવી_add_path(&buf, "CA", ssl->CAfile, &is_local);
    જો(!result)
      result = cf_ssl_peer_ચાવી_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    જો(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    જો(!result)
      result = cf_ssl_peer_ચાવી_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    જો(result)
      જાઓ out;
    result = cf_ssl_peer_ચાવી_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    જો(result)
      જાઓ out;
    જો(ssl->ca_info_blob) {
      result = cf_ssl_peer_ચાવી_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      જો(result)
        જાઓ out;
    }
    જો(ssl->issuercert_blob) {
      result = cf_ssl_peer_ચાવી_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      જો(result)
        જાઓ out;
    }
  }
  જો(ssl->cert_blob) {
    result = cf_ssl_peer_ચાવી_add_hash(&buf, "CertBlob", ssl->cert_blob);
    જો(result)
      જાઓ out;
  }
  જો(ssl->pinned_ચાવી && ssl->pinned_ચાવી[0]) {
    result = cf_ssl_peer_ચાવી_add_string(&buf, "Pinned", ssl->pinned_ચાવી);
    જો(result)
      જાઓ out;
  }

  result = cf_ssl_peer_ચાવી_add_mtls(&buf, ssl, &is_local);
  જો(result)
    જાઓ out;

  જો(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    જાઓ out;
  }
  result = cf_ssl_peer_ચાવી_add_string(&buf, "IMPL", tls_id);
  જો(result)
    જાઓ out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  જો(result)
    જાઓ out;

  *ppeer_ચાવી = curlx_dyn_take(&buf, &ચાવી_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  પાછા result;
}

CURLcode Curl_ssl_peer_ચાવી_make(અચલ સંરચના ssl_peer *peer,
                                સંરચના ssl_filter_config *sslc,
                                અચલ અક્ષર *tls_id,
                                અક્ષર **ppeer_ચાવી)
{
  પાછા ssl_peer_ચાવી_build(sslc, peer, tls_id, ppeer_ચાવી);
}

સંરચના Curl_ssl_scache {
  unsigned પૂર્ણાંક magic;
  સંરચના Curl_ssl_scache_peer *peers;
  size_t peer_ગણતરી;
  પૂર્ણાંક default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

સ્થિર સંરચના Curl_ssl_scache *cf_ssl_scache_get(સંરચના Curl_easy *data)
{
  સંરચના Curl_ssl_scache *scache = ખાલી;
  /* If a share is present, its ssl_scache has preference over the multi */
  જો(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  નહીંતર જો(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  જો(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (શૂન્ય *)scache);
    DEBUGASSERT(0);
    પાછા ખાલી;
  }
  પાછા scache;
}

સ્થિર શૂન્ય cf_ssl_scache_session_ldestroy(શૂન્ય *udata, શૂન્ય *obj)
{
  સંરચના Curl_ssl_session *s = obj;
  (શૂન્ય)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(શૂન્ય *sdata, size_t sdata_len,
                                 પૂર્ણાંક ietf_tls_id, અચલ અક્ષર *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 સંરચના Curl_ssl_session **psession)
{
  પાછા Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  ખાલી, 0, psession);
}

CURLcode Curl_ssl_session_create2(શૂન્ય *sdata, size_t sdata_len,
                                  પૂર્ણાંક ietf_tls_id, અચલ અક્ષર *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned અક્ષર *quic_tp, size_t quic_tp_len,
                                  સંરચના Curl_ssl_session **psession)
{
  સંરચના Curl_ssl_session *s;

  જો(!sdata || !sdata_len) {
    curlx_free(sdata);
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = ખાલી;
  s = curlx_calloc(1, કદ(*s));
  જો(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    પાછા CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  જો(alpn) {
    s->alpn = curlx_strdup(alpn);
    જો(!s->alpn) {
      cf_ssl_scache_session_ldestroy(ખાલી, s);
      પાછા CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  પાછા CURLE_OK;
}

શૂન્ય Curl_ssl_session_destroy(સંરચના Curl_ssl_session *s)
{
  જો(s) {
    /* if in the list, the list destructor takes care of it */
    જો(Curl_ગાંઠ_llist(&s->list))
      Curl_ગાંઠ_remove(&s->list);
    નહીંતર {
      cf_ssl_scache_session_ldestroy(ખાલી, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(સંરચના Curl_ssl_session *src,
                              સંરચના Curl_ssl_session **pdest)
{
  સંરચના Curl_ssl_session *dest = ખાલી;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  જો(!src || !pdest)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = ખાલી;

  dest = curlx_calloc(1, કદ(*dest));
  જો(!dest)
    જાઓ out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  જો(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    જો(!dest->sdata)
      જાઓ out;
    dest->sdata_len = src->sdata_len;
  }
  જો(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    જો(!dest->quic_tp)
      જાઓ out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  જો(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    જો(!dest->alpn)
      જાઓ out;
  }
  result = CURLE_OK;

out:
  જો(!result)
    *pdest = dest;
  નહીંતર {
    *pdest = ખાલી;
    જો(dest)
      Curl_ssl_session_destroy(dest);
  }
  પાછા result;
}

સ્થિર શૂન્ય cf_ssl_scache_clear_peer(સંરચના Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, ખાલી);
  જો(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    જો(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = ખાલી;
  }
  peer->sobj_free = ખાલી;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->ચાવી_passwd);
  curlx_safefree(peer->ssl_peer_ચાવી);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

સ્થિર શૂન્ય cf_ssl_scache_peer_set_obj(સંરચના Curl_ssl_scache_peer *peer,
                                       શૂન્ય *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  જો(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

સ્થિર શૂન્ય cf_ssl_cache_peer_update(સંરચના Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->ચાવી_passwd &&
                      (!peer->ssl_peer_ચાવી ||
                       cf_ssl_peer_ચાવી_is_global(peer->ssl_peer_ચાવી)));
}

સ્થિર CURLcode cf_ssl_scache_peer_init(સંરચના Curl_ssl_scache_peer *peer,
                                        અચલ અક્ષર *ssl_peer_ચાવી,
                                        અચલ અક્ષર *clientcert,
                                        અચલ અક્ષર *ચાવી_passwd,
                                        અચલ unsigned અક્ષર *salt,
                                        અચલ unsigned અક્ષર *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_ચાવી);
  જો(ssl_peer_ચાવી) {
    peer->ssl_peer_ચાવી = curlx_strdup(ssl_peer_ચાવી);
    જો(!peer->ssl_peer_ચાવી)
      જાઓ out;
    peer->hmac_set = FALSE;
  }
  નહીંતર જો(salt && hmac) {
    memcpy(peer->ચાવી_salt, salt, કદ(peer->ચાવી_salt));
    memcpy(peer->ચાવી_hmac, hmac, કદ(peer->ચાવી_hmac));
    peer->hmac_set = TRUE;
  }
  નહીંતર {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ out;
  }
  જો(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    જો(!peer->clientcert)
      જાઓ out;
  }
  જો(ચાવી_passwd) {
    peer->ચાવી_passwd = curlx_strdup(ચાવી_passwd);
    જો(!peer->ચાવી_passwd)
      જાઓ out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  જો(result)
    cf_ssl_scache_clear_peer(peer);
  પાછા result;
}

સ્થિર શૂન્ય cf_scache_session_remove(સંરચના Curl_ssl_scache_peer *peer,
                                     સંરચના Curl_ssl_session *s)
{
  (શૂન્ય)peer;
  DEBUGASSERT(Curl_ગાંઠ_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

સ્થિર bool cf_scache_session_expired(સંરચના Curl_ssl_session *s,
                                      curl_off_t now)
{
  પાછા (s->valid_until > 0) && (s->valid_until < now);
}

સ્થિર શૂન્ય cf_scache_peer_remove_expired(સંરચના Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  સંરચના Curl_llist_ગાંઠ *n = Curl_llist_head(&peer->sessions);
  જ્યારે(n) {
    સંરચના Curl_ssl_session *s = Curl_ગાંઠ_elem(n);
    n = Curl_ગાંઠ_next(n);
    જો(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

સ્થિર શૂન્ય cf_scache_peer_remove_non13(સંરચના Curl_ssl_scache_peer *peer)
{
  સંરચના Curl_llist_ગાંઠ *n = Curl_llist_head(&peer->sessions);
  જ્યારે(n) {
    સંરચના Curl_ssl_session *s = Curl_ગાંઠ_elem(n);
    n = Curl_ગાંઠ_next(n);
    જો(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                સંરચના Curl_ssl_scache **pscache)
{
  સંરચના Curl_ssl_scache *scache;
  સંરચના Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = ખાલી;
  peers = curlx_calloc(max_peers, કદ(*peers));
  જો(!peers)
    પાછા CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, કદ(*scache));
  જો(!scache) {
    curlx_free(peers);
    પાછા CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_ગણતરી = max_peers;
  scache->peers = peers;
  scache->age = 1;
  માટે(i = 0; i < scache->peer_ગણતરી; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  પાછા CURLE_OK;
}

શૂન્ય Curl_ssl_scache_destroy(સંરચના Curl_ssl_scache *scache)
{
  જો(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    માટે(i = 0; i < scache->peer_ગણતરી; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(સંરચના Curl_cfilter *cf, સંરચના Curl_easy *data)
{
  જો(cf_ssl_scache_get(data)) {
    સંરચના ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    પાછા conn_config ? conn_config->cache_session : FALSE;
  }
  પાછા FALSE;
}

/* Lock shared SSL session data */
શૂન્ય Curl_ssl_scache_lock(સંરચના Curl_easy *data)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  જો(scache) {
    જો(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#નહીંતર
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
શૂન્ય Curl_ssl_scache_unlock(સંરચના Curl_easy *data)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  જો(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#નહીંતર
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    જો(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(સંરચના Curl_easy *data)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  જો(!scache)
    પાછા FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#નહીંતર
  locked = (bool)scache->is_locked;
#endif
  પાછા locked;
}

સ્થિર bool cf_ssl_scache_match_auth(સંરચના Curl_ssl_scache_peer *peer,
                                     સંરચના ssl_filter_config *conn_config)
{
  જો(!conn_config) {
    જો(peer->clientcert || peer->ચાવી_passwd)
      પાછા FALSE;
    પાછા TRUE;
  }
  નહીંતર જો(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    પાછા FALSE;
  જો(Curl_timestrcmp(peer->ચાવી_passwd, conn_config->ચાવી_passwd))
    પાછા FALSE;
  પાછા TRUE;
}

સ્થિર CURLcode cf_ssl_find_peer_by_ચાવી(સંરચના Curl_easy *data,
                                        સંરચના Curl_ssl_scache *scache,
                                        અચલ અક્ષર *ssl_peer_ચાવી,
                                        સંરચના ssl_filter_config *conn_config,
                                        સંરચના Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_ચાવી_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = ખાલી;
  જો(!GOOD_SCACHE(scache)) {
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_ચાવી, scache->peer_ગણતરી);

  /* check for entries with known peer_key */
  માટે(i = 0; scache && i < scache->peer_ગણતરી; i++) {
    જો(scache->peers[i].ssl_peer_ચાવી &&
       !strcmp(ssl_peer_ચાવી, scache->peers[i].ssl_peer_ચાવી) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      જાઓ out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  માટે(i = 0; scache && i < scache->peer_ગણતરી; i++) {
    જો(!scache->peers[i].ssl_peer_ચાવી &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned અક્ષર my_hmac[CURL_SHA256_DIGEST_LENGTH];
      જો(!peer_ચાવી_len) /* we are lazy */
        peer_ચાવી_len = strlen(ssl_peer_ચાવી);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].ચાવી_salt,
                           કદ(scache->peers[i].ચાવી_salt),
                           (અચલ unsigned અક્ષર *)ssl_peer_ચાવી,
                           peer_ચાવી_len,
                           my_hmac);
      જો(result)
        જાઓ out;
      જો(!memcmp(scache->peers[i].ચાવી_hmac, my_hmac, કદ(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_ચાવી);
        scache->peers[i].ssl_peer_ચાવી = curlx_strdup(ssl_peer_ચાવી);
        જો(!scache->peers[i].ssl_peer_ચાવી) {
          result = CURLE_OUT_OF_MEMORY;
          જાઓ out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        જાઓ out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_ચાવી);
out:
  પાછા result;
}

સ્થિર સંરચના Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  સંરચના Curl_ssl_scache *scache)
{
  સંરચના Curl_ssl_scache_peer *peer = ખાલી;
  size_t i;

  /* find empty or oldest peer */
  માટે(i = 0; i < scache->peer_ગણતરી; ++i) {
    /* free peer entry? */
    જો(!scache->peers[i].ssl_peer_ચાવી && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      તોડો;
    }
    /* peer without sessions and obj */
    જો(!scache->peers[i].sobj &&
       !Curl_llist_ગણતરી(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      તોડો;
    }
    /* remember "oldest" peer */
    જો(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  જો(peer)
    cf_ssl_scache_clear_peer(peer);
  પાછા peer;
}

સ્થિર CURLcode cf_ssl_add_peer(સંરચના Curl_easy *data,
                                સંરચના Curl_ssl_scache *scache,
                                અચલ અક્ષર *ssl_peer_ચાવી,
                                સંરચના ssl_filter_config *conn_config,
                                સંરચના Curl_ssl_scache_peer **ppeer)
{
  સંરચના Curl_ssl_scache_peer *peer = ખાલી;
  CURLcode result = CURLE_OK;

  *ppeer = ખાલી;
  જો(ssl_peer_ચાવી) {
    result = cf_ssl_find_peer_by_ચાવી(data, scache, ssl_peer_ચાવી, conn_config,
                                     &peer);
    જો(result || !scache->peer_ગણતરી)
      પાછા result;
  }

  જો(peer) {
    *ppeer = peer;
    પાછા CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  જો(peer) {
    અક્ષર buffer[64];
    અચલ અક્ષર *ccert = conn_config ? conn_config->clientcert : ખાલી;
    અચલ અક્ષર *kpasswd = conn_config ? conn_config->ચાવી_passwd : ખાલી;
    જો(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, કદ(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_ચાવી, ccert, kpasswd,
                                     ખાલી, ખાલી);
    જો(result)
      જાઓ out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  જો(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  પાછા result;
}

સ્થિર શૂન્ય cf_scache_peer_add_session(સંરચના Curl_ssl_scache_peer *peer,
                                       સંરચના Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  જો(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, ખાલી);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  નહીંતર {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    જ્યારે(Curl_llist_ગણતરી(&peer->sessions) > peer->max_sessions) {
      Curl_ગાંઠ_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

સ્થિર CURLcode cf_scache_add_session(સંરચના Curl_cfilter *cf,
                                      સંરચના Curl_easy *data,
                                      સંરચના Curl_ssl_scache *scache,
                                      અચલ અક્ષર *ssl_peer_ચાવી,
                                      સંરચના Curl_ssl_session *s)
{
  સંરચના Curl_ssl_scache_peer *peer = ખાલી;
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(ખાલી);
  curl_off_t max_lifetime;

  જો(!scache || !scache->peer_ગણતરી) {
    Curl_ssl_session_destroy(s);
    પાછા CURLE_OK;
  }

  જો(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  જો(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  જો(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    પાછા CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_ચાવી, conn_config, &peer);
  જો(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (પૂર્ણાંક)result);
    Curl_ssl_session_destroy(s);
    જાઓ out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  જો(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_ચાવી, (પૂર્ણાંક)result);
  }
  નહીંતર
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_ચાવી,
                  (unsigned પૂર્ણાંક)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_ગણતરી(&peer->sessions) : 0);
  પાછા result;
}

CURLcode Curl_ssl_scache_put(સંરચના Curl_cfilter *cf,
                             સંરચના Curl_easy *data,
                             અચલ અક્ષર *ssl_peer_ચાવી,
                             સંરચના Curl_ssl_session *s)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  જો(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    પાછા CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_ચાવી, s);
  Curl_ssl_scache_unlock(data);
  પાછા result;
}

શૂન્ય Curl_ssl_scache_return(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data,
                            અચલ અક્ષર *ssl_peer_ચાવી,
                            સંરચના Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  જો(s && s->ietf_tls_id < 0x304)
    (શૂન્ય)Curl_ssl_scache_put(cf, data, ssl_peer_ચાવી, s);
  નહીંતર
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              અચલ અક્ષર *ssl_peer_ચાવી,
                              સંરચના Curl_ssl_session **ps)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના Curl_ssl_scache_peer *peer = ખાલી;
  સંરચના Curl_llist_ગાંઠ *n;
  સંરચના Curl_ssl_session *s = ખાલી;
  CURLcode result;

  *ps = ખાલી;
  જો(!scache)
    પાછા CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_ચાવી(data, scache, ssl_peer_ચાવી, conn_config,
                                   &peer);
  જો(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(ખાલી));
    n = Curl_llist_head(&peer->sessions);
    જો(n) {
      s = Curl_ગાંઠ_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  જો(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_ચાવી, (unsigned પૂર્ણાંક)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_ગણતરી(&peer->sessions));
  }
  નહીંતર {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_ચાવી);
  }
  Curl_ssl_scache_unlock(data);
  પાછા result;
}

CURLcode Curl_ssl_scache_add_obj(સંરચના Curl_cfilter *cf,
                                 સંરચના Curl_easy *data,
                                 અચલ અક્ષર *ssl_peer_ચાવી,
                                 શૂન્ય *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના Curl_ssl_scache_peer *peer = ખાલી;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  જો(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_ચાવી, conn_config, &peer);
  જો(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (પૂર્ણાંક)result);
    જાઓ out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = ખાલી;  /* peer took ownership */

out:
  જો(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  પાછા result;
}

શૂન્ય *Curl_ssl_scache_get_obj(સંરચના Curl_cfilter *cf,
                              સંરચના Curl_easy *data,
                              અચલ અક્ષર *ssl_peer_ચાવી)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના Curl_ssl_scache_peer *peer = ખાલી;
  CURLcode result;
  શૂન્ય *sobj;

  જો(!scache)
    પાછા ખાલી;

  result = cf_ssl_find_peer_by_ચાવી(data, scache, ssl_peer_ચાવી, conn_config,
                                   &peer);
  જો(result)
    પાછા ખાલી;

  sobj = peer ? peer->sobj : ખાલી;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_ચાવી);
  પાછા sobj;
}

શૂન્ય Curl_ssl_scache_remove_all(સંરચના Curl_cfilter *cf,
                                સંરચના Curl_easy *data,
                                અચલ અક્ષર *ssl_peer_ચાવી)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  સંરચના ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  સંરચના Curl_ssl_scache_peer *peer = ખાલી;
  CURLcode result;

  (શૂન્ય)cf;
  જો(!scache)
    પાછા;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_ચાવી(data, scache, ssl_peer_ચાવી, conn_config,
                                   &peer);
  જો(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

સ્થિર CURLcode cf_ssl_scache_peer_set_hmac(સંરચના Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  જો(!peer->ssl_peer_ચાવી)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(ખાલી, peer->ચાવી_salt, કદ(peer->ચાવી_salt));
  જો(result)
    પાછા result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->ચાવી_salt, કદ(peer->ચાવી_salt),
                       (અચલ unsigned અક્ષર *)peer->ssl_peer_ચાવી,
                       strlen(peer->ssl_peer_ચાવી),
                       peer->ચાવી_hmac);
  જો(!result)
    peer->hmac_set = TRUE;
  પાછા result;
}

સ્થિર CURLcode cf_ssl_find_peer_by_hmac(સંરચના Curl_ssl_scache *scache,
                                         અચલ unsigned અક્ષર *salt,
                                         અચલ unsigned અક્ષર *hmac,
                                         સંરચના Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = ખાલી;
  જો(!GOOD_SCACHE(scache))
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  માટે(i = 0; scache && i < scache->peer_ગણતરી; i++) {
    સંરચના Curl_ssl_scache_peer *peer = &scache->peers[i];
    જો(!cf_ssl_scache_match_auth(peer, ખાલી))
      ચાલુ;
    જો(scache->peers[i].hmac_set &&
       !memcmp(peer->ચાવી_salt, salt, કદ(peer->ચાવી_salt)) &&
       !memcmp(peer->ચાવી_hmac, hmac, કદ(peer->ચાવી_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      જાઓ out;
    }
    નહીંતર જો(peer->ssl_peer_ચાવી) {
      unsigned અક્ષર my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, કદ(peer->ચાવી_salt),
                           (અચલ unsigned અક્ષર *)peer->ssl_peer_ચાવી,
                           strlen(peer->ssl_peer_ચાવી),
                           my_hmac);
      જો(result)
        જાઓ out;
      જો(!memcmp(my_hmac, hmac, કદ(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        જો(!peer->hmac_set) {
          memcpy(peer->ચાવી_salt, salt, કદ(peer->ચાવી_salt));
          memcpy(peer->ચાવી_hmac, hmac, કદ(peer->ચાવી_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        જાઓ out;
      }
    }
  }
out:
  પાછા result;
}

CURLcode Curl_ssl_session_import(સંરચના Curl_easy *data,
                                 અચલ અક્ષર *ssl_peer_ચાવી,
                                 અચલ unsigned અક્ષર *shmac, size_t shmac_len,
                                 અચલ શૂન્ય *sdata, size_t sdata_len)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  સંરચના Curl_ssl_scache_peer *peer = ખાલી;
  સંરચના Curl_ssl_session *s = ખાલી;
  bool have_lock = FALSE;
  CURLcode result;

  જો(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ out;
  }
  જો(!ssl_peer_ચાવી && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  જો(result)
    જાઓ out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  જો(ssl_peer_ચાવી) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_ચાવી, ખાલી, &peer);
    જો(result)
      જાઓ out;
  }
  નહીંતર જો(shmac_len != (કદ(peer->ચાવી_salt) + કદ(peer->ચાવી_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    જાઓ out;
  }
  નહીંતર {
    અચલ unsigned અક્ષર *salt = shmac;
    અચલ unsigned અક્ષર *hmac = shmac + કદ(peer->ચાવી_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    જો(result)
      જાઓ out;
    જો(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      જો(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_ચાવી, ખાલી, ખાલી,
                                         salt, hmac);
        જો(result)
          જાઓ out;
      }
    }
  }

  જો(peer) {
    cf_scache_peer_add_session(peer, s, time(ખાલી));
    s = ખાલી; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_ચાવી ? peer->ssl_peer_ચાવી : "without key",
                  Curl_llist_ગણતરી(&peer->sessions));
  }

out:
  જો(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  પાછા result;
}

CURLcode Curl_ssl_session_export(સંરચના Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 શૂન્ય *userptr)
{
  સંરચના Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  સંરચના Curl_ssl_scache_peer *peer;
  સંરચના dynbuf sbuf, hbuf;
  સંરચના Curl_llist_ગાંઠ *n;
  size_t i;
  curl_off_t now = time(ખાલી);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  જો(!export_fn)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  માટે(i = 0; scache && i < scache->peer_ગણતરી; i++) {
    peer = &scache->peers[i];
    જો(!peer->ssl_peer_ચાવી && !peer->hmac_set)
      ચાલુ; /* skip free entry */
    જો(!peer->exportable)
      ચાલુ;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    જો(n)
      VERBOSE(++npeers);
    જ્યારે(n) {
      સંરચના Curl_ssl_session *s = Curl_ગાંઠ_elem(n);
      જો(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        જો(result)
          જાઓ out;
      }
      જો(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->ચાવી_salt, કદ(peer->ચાવી_salt));
        જો(result)
          જાઓ out;
        result = curlx_dyn_addn(&hbuf, peer->ચાવી_hmac, કદ(peer->ચાવી_hmac));
        જો(result)
          જાઓ out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      જો(result)
        જાઓ out;

      result = export_fn(data, userptr, peer->ssl_peer_ચાવી,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      જો(result)
        જાઓ out;
      VERBOSE(++ntickets);
      n = Curl_ગાંઠ_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  પાછા result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
