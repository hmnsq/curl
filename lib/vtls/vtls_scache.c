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
#அந்தர்பாவ "curl_setup.h"

#ifdef USE_SSL

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "cfilters.h"

#அந்தர்பாவ "vtls/vtls.h" /* generic SSL protos etc */
#அந்தர்பாவ "vtls/vtls_int.h"
#அந்தர்பாவ "vtls/vtls_scache.h"
#அந்தர்பாவ "vtls/vtls_spack.h"

#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "llist.h"
#அந்தர்பாவ "curl_share.h"
#அந்தர்பாவ "curl_threads.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "curl_sha256.h"
#அந்தர்பாவ "rand.h"
#அந்தர்பாவ "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
கட்டமைப்பு Curl_ssl_scache_peer {
  எழுத்து *ssl_peer_சாவி;      /* id for peer + relevant TLS configuration */
  எழுத்து *clientcert;
  எழுத்து *சாவி_passwd;
  கட்டமைப்பு Curl_llist sessions;
  வெறுமை *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned எழுத்து சாவி_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned எழுத்து சாவி_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
நிலையான CURLcode cf_ssl_peer_சாவி_add_string(கட்டமைப்பு dynbuf *buf,
                                           மாறிலி எழுத்து *name,
                                           மாறிலி எழுத்து *மதிப்பு)
{
  திரும்பு curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(மதிப்பு), மதிப்பு);
}

நிலையான CURLcode cf_ssl_peer_சாவி_add_path(கட்டமைப்பு dynbuf *buf,
                                         மாறிலி எழுத்து *name,
                                         மாறிலி எழுத்து *path,
                                         bool *is_local)
{
  எனில்(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    எழுத்து *abspath = _fullpath(வெற்று, path, 0);
    எனில்(abspath) {
      CURLcode result = cf_ssl_peer_சாவி_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      விடுவி(abspath); /* allocated by CRT, use system free() */
      திரும்பு result;
    }
    *is_local = TRUE;
#அல்லதெனில் defined(HAVE_REALPATH)
    எனில்(path[0] != '/') {
      எழுத்து *abspath = realpath(path, வெற்று);
      எனில்(abspath) {
        CURLcode result = cf_ssl_peer_சாவி_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        விடுவி(abspath); /* allocated by libc, free without memdebug */
        திரும்பு result;
      }
      *is_local = TRUE;
    }
#endif
    திரும்பு cf_ssl_peer_சாவி_add_string(buf, name, path);
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode cf_ssl_peer_சாவி_add_hash(கட்டமைப்பு dynbuf *buf,
                                         மாறிலி எழுத்து *name,
                                         கட்டமைப்பு curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  எனில்(blob && blob->len) {
    unsigned எழுத்து hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    எனில்(result)
      செல் out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    எனில்(result)
      செல் out;
    க்கு(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      எனில்(result)
        செல் out;
    }
  }
out:
  திரும்பு result;
}

நிலையான CURLcode cf_ssl_peer_சாவி_add_mtls(கட்டமைப்பு dynbuf *buf,
                                         கட்டமைப்பு ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  எனில்(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_சாவி_add_path(buf, "CCERT", ssl->clientcert, is_local);
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->சாவி && ssl->சாவி[0]) {
    result = cf_ssl_peer_சாவி_add_path(buf, "KEY", ssl->சாவி, is_local);
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->சாவி_blob) {
    result = cf_ssl_peer_சாவி_add_hash(buf, "KEYBlob", ssl->சாவி_blob);
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    க்கு(i = 0; !result && ssl->cert_type[i]; i++) {
      எழுத்து c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->சாவி_type && ssl->சாவி_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->சாவி_type));
    க்கு(i = 0; !result && ssl->சாவி_type[i]; i++) {
      எழுத்து c = Curl_raw_toupper(ssl->சாவி_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  திரும்பு result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

நிலையான bool cf_ssl_peer_சாவி_is_global(மாறிலி எழுத்து *peer_சாவி)
{
  size_t len = peer_சாவி ? strlen(peer_சாவி) : 0;
  திரும்பு (len > 2) &&
         (peer_சாவி[len - 1] == 'G') &&
         (peer_சாவி[len - 2] == ':');
}

நிலையான CURLcode ssl_peer_சாவி_add_transport(கட்டமைப்பு dynbuf *buf,
                                           uint8_t transport)
{
  தேர்வு(transport) {
  நிலை TRNSPRT_TCP:
    திரும்பு CURLE_OK;
  நிலை TRNSPRT_UDP:
    திரும்பு curlx_dyn_add(buf, ":UDP");
  நிலை TRNSPRT_QUIC:
    திரும்பு curlx_dyn_add(buf, ":QUIC");
  நிலை TRNSPRT_UNIX:
    திரும்பு curlx_dyn_add(buf, ":UNIX");
  இயல்பு:
    திரும்பு curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

நிலையான CURLcode ssl_peer_சாவி_add_vrfy(கட்டமைப்பு dynbuf *buf,
                                      கட்டமைப்பு ssl_filter_config *ssl,
                                      மாறிலி கட்டமைப்பு ssl_peer *peer)
{
  CURLcode result;

  எனில்(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    எனில்(result)
      திரும்பு result;
  }
  எனில்(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    எனில்(result)
      திரும்பு result;
  }
  எனில்(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    எனில்(result)
      திரும்பு result;
  }
  எனில்((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_சாவி_add_string(buf, "CHOST", peer->peer->hostname);
    எனில்(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    எனில்(result)
      திரும்பு result;
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode ssl_peer_சாவி_build(கட்டமைப்பு ssl_filter_config *ssl,
                                   மாறிலி கட்டமைப்பு ssl_peer *peer,
                                   மாறிலி எழுத்து *tls_id,
                                   எழுத்து **ppeer_சாவி)
{
  கட்டமைப்பு dynbuf buf;
  size_t சாவி_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_சாவி = வெற்று;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  எனில்(result)
    செல் out;
  result = ssl_peer_சாவி_add_transport(&buf, peer->transport);
  எனில்(result)
    செல் out;
  result = ssl_peer_சாவி_add_vrfy(&buf, ssl, peer);
  எனில்(result)
    செல் out;
  எனில்(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->cipher_list) {
    result = cf_ssl_peer_சாவி_add_string(&buf, "CIPHER", ssl->cipher_list);
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->cipher_list13) {
    result = cf_ssl_peer_சாவி_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->curves) {
    result = cf_ssl_peer_சாவி_add_string(&buf, "CURVES", ssl->curves);
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->signature_algorithms) {
    result = cf_ssl_peer_சாவி_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->verifypeer) {
    result = cf_ssl_peer_சாவி_add_path(&buf, "CA", ssl->CAfile, &is_local);
    எனில்(!result)
      result = cf_ssl_peer_சாவி_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    எனில்(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    எனில்(!result)
      result = cf_ssl_peer_சாவி_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    எனில்(result)
      செல் out;
    result = cf_ssl_peer_சாவி_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    எனில்(result)
      செல் out;
    எனில்(ssl->ca_info_blob) {
      result = cf_ssl_peer_சாவி_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      எனில்(result)
        செல் out;
    }
    எனில்(ssl->issuercert_blob) {
      result = cf_ssl_peer_சாவி_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      எனில்(result)
        செல் out;
    }
  }
  எனில்(ssl->cert_blob) {
    result = cf_ssl_peer_சாவி_add_hash(&buf, "CertBlob", ssl->cert_blob);
    எனில்(result)
      செல் out;
  }
  எனில்(ssl->pinned_சாவி && ssl->pinned_சாவி[0]) {
    result = cf_ssl_peer_சாவி_add_string(&buf, "Pinned", ssl->pinned_சாவி);
    எனில்(result)
      செல் out;
  }

  result = cf_ssl_peer_சாவி_add_mtls(&buf, ssl, &is_local);
  எனில்(result)
    செல் out;

  எனில்(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    செல் out;
  }
  result = cf_ssl_peer_சாவி_add_string(&buf, "IMPL", tls_id);
  எனில்(result)
    செல் out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  எனில்(result)
    செல் out;

  *ppeer_சாவி = curlx_dyn_take(&buf, &சாவி_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  திரும்பு result;
}

CURLcode Curl_ssl_peer_சாவி_make(மாறிலி கட்டமைப்பு ssl_peer *peer,
                                கட்டமைப்பு ssl_filter_config *sslc,
                                மாறிலி எழுத்து *tls_id,
                                எழுத்து **ppeer_சாவி)
{
  திரும்பு ssl_peer_சாவி_build(sslc, peer, tls_id, ppeer_சாவி);
}

கட்டமைப்பு Curl_ssl_scache {
  unsigned முழு magic;
  கட்டமைப்பு Curl_ssl_scache_peer *peers;
  size_t peer_மொத்தம்;
  முழு default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

நிலையான கட்டமைப்பு Curl_ssl_scache *cf_ssl_scache_get(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_ssl_scache *scache = வெற்று;
  /* If a share is present, its ssl_scache has preference over the multi */
  எனில்(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  இல்லையெனில் எனில்(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  எனில்(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (வெறுமை *)scache);
    DEBUGASSERT(0);
    திரும்பு வெற்று;
  }
  திரும்பு scache;
}

நிலையான வெறுமை cf_ssl_scache_session_ldestroy(வெறுமை *udata, வெறுமை *obj)
{
  கட்டமைப்பு Curl_ssl_session *s = obj;
  (வெறுமை)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(வெறுமை *sdata, size_t sdata_len,
                                 முழு ietf_tls_id, மாறிலி எழுத்து *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 கட்டமைப்பு Curl_ssl_session **psession)
{
  திரும்பு Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  வெற்று, 0, psession);
}

CURLcode Curl_ssl_session_create2(வெறுமை *sdata, size_t sdata_len,
                                  முழு ietf_tls_id, மாறிலி எழுத்து *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned எழுத்து *quic_tp, size_t quic_tp_len,
                                  கட்டமைப்பு Curl_ssl_session **psession)
{
  கட்டமைப்பு Curl_ssl_session *s;

  எனில்(!sdata || !sdata_len) {
    curlx_free(sdata);
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = வெற்று;
  s = curlx_calloc(1, அளவு(*s));
  எனில்(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  எனில்(alpn) {
    s->alpn = curlx_strdup(alpn);
    எனில்(!s->alpn) {
      cf_ssl_scache_session_ldestroy(வெற்று, s);
      திரும்பு CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  திரும்பு CURLE_OK;
}

வெறுமை Curl_ssl_session_destroy(கட்டமைப்பு Curl_ssl_session *s)
{
  எனில்(s) {
    /* if in the list, the list destructor takes care of it */
    எனில்(Curl_முனை_llist(&s->list))
      Curl_முனை_remove(&s->list);
    இல்லையெனில் {
      cf_ssl_scache_session_ldestroy(வெற்று, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(கட்டமைப்பு Curl_ssl_session *src,
                              கட்டமைப்பு Curl_ssl_session **pdest)
{
  கட்டமைப்பு Curl_ssl_session *dest = வெற்று;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  எனில்(!src || !pdest)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = வெற்று;

  dest = curlx_calloc(1, அளவு(*dest));
  எனில்(!dest)
    செல் out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  எனில்(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    எனில்(!dest->sdata)
      செல் out;
    dest->sdata_len = src->sdata_len;
  }
  எனில்(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    எனில்(!dest->quic_tp)
      செல் out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  எனில்(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    எனில்(!dest->alpn)
      செல் out;
  }
  result = CURLE_OK;

out:
  எனில்(!result)
    *pdest = dest;
  இல்லையெனில் {
    *pdest = வெற்று;
    எனில்(dest)
      Curl_ssl_session_destroy(dest);
  }
  திரும்பு result;
}

நிலையான வெறுமை cf_ssl_scache_clear_peer(கட்டமைப்பு Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, வெற்று);
  எனில்(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    எனில்(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = வெற்று;
  }
  peer->sobj_free = வெற்று;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->சாவி_passwd);
  curlx_safefree(peer->ssl_peer_சாவி);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

நிலையான வெறுமை cf_ssl_scache_peer_set_obj(கட்டமைப்பு Curl_ssl_scache_peer *peer,
                                       வெறுமை *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  எனில்(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

நிலையான வெறுமை cf_ssl_cache_peer_update(கட்டமைப்பு Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->சாவி_passwd &&
                      (!peer->ssl_peer_சாவி ||
                       cf_ssl_peer_சாவி_is_global(peer->ssl_peer_சாவி)));
}

நிலையான CURLcode cf_ssl_scache_peer_init(கட்டமைப்பு Curl_ssl_scache_peer *peer,
                                        மாறிலி எழுத்து *ssl_peer_சாவி,
                                        மாறிலி எழுத்து *clientcert,
                                        மாறிலி எழுத்து *சாவி_passwd,
                                        மாறிலி unsigned எழுத்து *salt,
                                        மாறிலி unsigned எழுத்து *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_சாவி);
  எனில்(ssl_peer_சாவி) {
    peer->ssl_peer_சாவி = curlx_strdup(ssl_peer_சாவி);
    எனில்(!peer->ssl_peer_சாவி)
      செல் out;
    peer->hmac_set = FALSE;
  }
  இல்லையெனில் எனில்(salt && hmac) {
    memcpy(peer->சாவி_salt, salt, அளவு(peer->சாவி_salt));
    memcpy(peer->சாவி_hmac, hmac, அளவு(peer->சாவி_hmac));
    peer->hmac_set = TRUE;
  }
  இல்லையெனில் {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }
  எனில்(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    எனில்(!peer->clientcert)
      செல் out;
  }
  எனில்(சாவி_passwd) {
    peer->சாவி_passwd = curlx_strdup(சாவி_passwd);
    எனில்(!peer->சாவி_passwd)
      செல் out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  எனில்(result)
    cf_ssl_scache_clear_peer(peer);
  திரும்பு result;
}

நிலையான வெறுமை cf_scache_session_remove(கட்டமைப்பு Curl_ssl_scache_peer *peer,
                                     கட்டமைப்பு Curl_ssl_session *s)
{
  (வெறுமை)peer;
  DEBUGASSERT(Curl_முனை_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

நிலையான bool cf_scache_session_expired(கட்டமைப்பு Curl_ssl_session *s,
                                      curl_off_t now)
{
  திரும்பு (s->valid_until > 0) && (s->valid_until < now);
}

நிலையான வெறுமை cf_scache_peer_remove_expired(கட்டமைப்பு Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  கட்டமைப்பு Curl_llist_முனை *n = Curl_llist_head(&peer->sessions);
  போது(n) {
    கட்டமைப்பு Curl_ssl_session *s = Curl_முனை_elem(n);
    n = Curl_முனை_next(n);
    எனில்(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

நிலையான வெறுமை cf_scache_peer_remove_non13(கட்டமைப்பு Curl_ssl_scache_peer *peer)
{
  கட்டமைப்பு Curl_llist_முனை *n = Curl_llist_head(&peer->sessions);
  போது(n) {
    கட்டமைப்பு Curl_ssl_session *s = Curl_முனை_elem(n);
    n = Curl_முனை_next(n);
    எனில்(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                கட்டமைப்பு Curl_ssl_scache **pscache)
{
  கட்டமைப்பு Curl_ssl_scache *scache;
  கட்டமைப்பு Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = வெற்று;
  peers = curlx_calloc(max_peers, அளவு(*peers));
  எனில்(!peers)
    திரும்பு CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, அளவு(*scache));
  எனில்(!scache) {
    curlx_free(peers);
    திரும்பு CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_மொத்தம் = max_peers;
  scache->peers = peers;
  scache->age = 1;
  க்கு(i = 0; i < scache->peer_மொத்தம்; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  திரும்பு CURLE_OK;
}

வெறுமை Curl_ssl_scache_destroy(கட்டமைப்பு Curl_ssl_scache *scache)
{
  எனில்(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    க்கு(i = 0; i < scache->peer_மொத்தம்; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(கட்டமைப்பு Curl_cfilter *cf, கட்டமைப்பு Curl_easy *data)
{
  எனில்(cf_ssl_scache_get(data)) {
    கட்டமைப்பு ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    திரும்பு conn_config ? conn_config->cache_session : FALSE;
  }
  திரும்பு FALSE;
}

/* Lock shared SSL session data */
வெறுமை Curl_ssl_scache_lock(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  எனில்(scache) {
    எனில்(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#இல்லையெனில்
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
வெறுமை Curl_ssl_scache_unlock(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  எனில்(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#இல்லையெனில்
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    எனில்(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  எனில்(!scache)
    திரும்பு FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#இல்லையெனில்
  locked = (bool)scache->is_locked;
#endif
  திரும்பு locked;
}

நிலையான bool cf_ssl_scache_match_auth(கட்டமைப்பு Curl_ssl_scache_peer *peer,
                                     கட்டமைப்பு ssl_filter_config *conn_config)
{
  எனில்(!conn_config) {
    எனில்(peer->clientcert || peer->சாவி_passwd)
      திரும்பு FALSE;
    திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    திரும்பு FALSE;
  எனில்(Curl_timestrcmp(peer->சாவி_passwd, conn_config->சாவி_passwd))
    திரும்பு FALSE;
  திரும்பு TRUE;
}

நிலையான CURLcode cf_ssl_find_peer_by_சாவி(கட்டமைப்பு Curl_easy *data,
                                        கட்டமைப்பு Curl_ssl_scache *scache,
                                        மாறிலி எழுத்து *ssl_peer_சாவி,
                                        கட்டமைப்பு ssl_filter_config *conn_config,
                                        கட்டமைப்பு Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_சாவி_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = வெற்று;
  எனில்(!GOOD_SCACHE(scache)) {
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_சாவி, scache->peer_மொத்தம்);

  /* check for entries with known peer_key */
  க்கு(i = 0; scache && i < scache->peer_மொத்தம்; i++) {
    எனில்(scache->peers[i].ssl_peer_சாவி &&
       !strcmp(ssl_peer_சாவி, scache->peers[i].ssl_peer_சாவி) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      செல் out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  க்கு(i = 0; scache && i < scache->peer_மொத்தம்; i++) {
    எனில்(!scache->peers[i].ssl_peer_சாவி &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned எழுத்து my_hmac[CURL_SHA256_DIGEST_LENGTH];
      எனில்(!peer_சாவி_len) /* we are lazy */
        peer_சாவி_len = strlen(ssl_peer_சாவி);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].சாவி_salt,
                           அளவு(scache->peers[i].சாவி_salt),
                           (மாறிலி unsigned எழுத்து *)ssl_peer_சாவி,
                           peer_சாவி_len,
                           my_hmac);
      எனில்(result)
        செல் out;
      எனில்(!memcmp(scache->peers[i].சாவி_hmac, my_hmac, அளவு(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_சாவி);
        scache->peers[i].ssl_peer_சாவி = curlx_strdup(ssl_peer_சாவி);
        எனில்(!scache->peers[i].ssl_peer_சாவி) {
          result = CURLE_OUT_OF_MEMORY;
          செல் out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        செல் out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_சாவி);
out:
  திரும்பு result;
}

நிலையான கட்டமைப்பு Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  கட்டமைப்பு Curl_ssl_scache *scache)
{
  கட்டமைப்பு Curl_ssl_scache_peer *peer = வெற்று;
  size_t i;

  /* find empty or oldest peer */
  க்கு(i = 0; i < scache->peer_மொத்தம்; ++i) {
    /* free peer entry? */
    எனில்(!scache->peers[i].ssl_peer_சாவி && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      நிறுத்து;
    }
    /* peer without sessions and obj */
    எனில்(!scache->peers[i].sobj &&
       !Curl_llist_மொத்தம்(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      நிறுத்து;
    }
    /* remember "oldest" peer */
    எனில்(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  எனில்(peer)
    cf_ssl_scache_clear_peer(peer);
  திரும்பு peer;
}

நிலையான CURLcode cf_ssl_add_peer(கட்டமைப்பு Curl_easy *data,
                                கட்டமைப்பு Curl_ssl_scache *scache,
                                மாறிலி எழுத்து *ssl_peer_சாவி,
                                கட்டமைப்பு ssl_filter_config *conn_config,
                                கட்டமைப்பு Curl_ssl_scache_peer **ppeer)
{
  கட்டமைப்பு Curl_ssl_scache_peer *peer = வெற்று;
  CURLcode result = CURLE_OK;

  *ppeer = வெற்று;
  எனில்(ssl_peer_சாவி) {
    result = cf_ssl_find_peer_by_சாவி(data, scache, ssl_peer_சாவி, conn_config,
                                     &peer);
    எனில்(result || !scache->peer_மொத்தம்)
      திரும்பு result;
  }

  எனில்(peer) {
    *ppeer = peer;
    திரும்பு CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  எனில்(peer) {
    எழுத்து buffer[64];
    மாறிலி எழுத்து *ccert = conn_config ? conn_config->clientcert : வெற்று;
    மாறிலி எழுத்து *kpasswd = conn_config ? conn_config->சாவி_passwd : வெற்று;
    எனில்(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, அளவு(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_சாவி, ccert, kpasswd,
                                     வெற்று, வெற்று);
    எனில்(result)
      செல் out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  எனில்(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  திரும்பு result;
}

நிலையான வெறுமை cf_scache_peer_add_session(கட்டமைப்பு Curl_ssl_scache_peer *peer,
                                       கட்டமைப்பு Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  எனில்(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, வெற்று);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  இல்லையெனில் {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    போது(Curl_llist_மொத்தம்(&peer->sessions) > peer->max_sessions) {
      Curl_முனை_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

நிலையான CURLcode cf_scache_add_session(கட்டமைப்பு Curl_cfilter *cf,
                                      கட்டமைப்பு Curl_easy *data,
                                      கட்டமைப்பு Curl_ssl_scache *scache,
                                      மாறிலி எழுத்து *ssl_peer_சாவி,
                                      கட்டமைப்பு Curl_ssl_session *s)
{
  கட்டமைப்பு Curl_ssl_scache_peer *peer = வெற்று;
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(வெற்று);
  curl_off_t max_lifetime;

  எனில்(!scache || !scache->peer_மொத்தம்) {
    Curl_ssl_session_destroy(s);
    திரும்பு CURLE_OK;
  }

  எனில்(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  எனில்(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  எனில்(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    திரும்பு CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_சாவி, conn_config, &peer);
  எனில்(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (முழு)result);
    Curl_ssl_session_destroy(s);
    செல் out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  எனில்(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_சாவி, (முழு)result);
  }
  இல்லையெனில்
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_சாவி,
                  (unsigned முழு)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_மொத்தம்(&peer->sessions) : 0);
  திரும்பு result;
}

CURLcode Curl_ssl_scache_put(கட்டமைப்பு Curl_cfilter *cf,
                             கட்டமைப்பு Curl_easy *data,
                             மாறிலி எழுத்து *ssl_peer_சாவி,
                             கட்டமைப்பு Curl_ssl_session *s)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  எனில்(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    திரும்பு CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_சாவி, s);
  Curl_ssl_scache_unlock(data);
  திரும்பு result;
}

வெறுமை Curl_ssl_scache_return(கட்டமைப்பு Curl_cfilter *cf,
                            கட்டமைப்பு Curl_easy *data,
                            மாறிலி எழுத்து *ssl_peer_சாவி,
                            கட்டமைப்பு Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  எனில்(s && s->ietf_tls_id < 0x304)
    (வெறுமை)Curl_ssl_scache_put(cf, data, ssl_peer_சாவி, s);
  இல்லையெனில்
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *ssl_peer_சாவி,
                              கட்டமைப்பு Curl_ssl_session **ps)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு Curl_ssl_scache_peer *peer = வெற்று;
  கட்டமைப்பு Curl_llist_முனை *n;
  கட்டமைப்பு Curl_ssl_session *s = வெற்று;
  CURLcode result;

  *ps = வெற்று;
  எனில்(!scache)
    திரும்பு CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_சாவி(data, scache, ssl_peer_சாவி, conn_config,
                                   &peer);
  எனில்(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(வெற்று));
    n = Curl_llist_head(&peer->sessions);
    எனில்(n) {
      s = Curl_முனை_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  எனில்(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_சாவி, (unsigned முழு)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_மொத்தம்(&peer->sessions));
  }
  இல்லையெனில் {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_சாவி);
  }
  Curl_ssl_scache_unlock(data);
  திரும்பு result;
}

CURLcode Curl_ssl_scache_add_obj(கட்டமைப்பு Curl_cfilter *cf,
                                 கட்டமைப்பு Curl_easy *data,
                                 மாறிலி எழுத்து *ssl_peer_சாவி,
                                 வெறுமை *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு Curl_ssl_scache_peer *peer = வெற்று;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  எனில்(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_சாவி, conn_config, &peer);
  எனில்(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (முழு)result);
    செல் out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = வெற்று;  /* peer took ownership */

out:
  எனில்(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  திரும்பு result;
}

வெறுமை *Curl_ssl_scache_get_obj(கட்டமைப்பு Curl_cfilter *cf,
                              கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து *ssl_peer_சாவி)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு Curl_ssl_scache_peer *peer = வெற்று;
  CURLcode result;
  வெறுமை *sobj;

  எனில்(!scache)
    திரும்பு வெற்று;

  result = cf_ssl_find_peer_by_சாவி(data, scache, ssl_peer_சாவி, conn_config,
                                   &peer);
  எனில்(result)
    திரும்பு வெற்று;

  sobj = peer ? peer->sobj : வெற்று;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_சாவி);
  திரும்பு sobj;
}

வெறுமை Curl_ssl_scache_remove_all(கட்டமைப்பு Curl_cfilter *cf,
                                கட்டமைப்பு Curl_easy *data,
                                மாறிலி எழுத்து *ssl_peer_சாவி)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  கட்டமைப்பு ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  கட்டமைப்பு Curl_ssl_scache_peer *peer = வெற்று;
  CURLcode result;

  (வெறுமை)cf;
  எனில்(!scache)
    திரும்பு;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_சாவி(data, scache, ssl_peer_சாவி, conn_config,
                                   &peer);
  எனில்(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

நிலையான CURLcode cf_ssl_scache_peer_set_hmac(கட்டமைப்பு Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  எனில்(!peer->ssl_peer_சாவி)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(வெற்று, peer->சாவி_salt, அளவு(peer->சாவி_salt));
  எனில்(result)
    திரும்பு result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->சாவி_salt, அளவு(peer->சாவி_salt),
                       (மாறிலி unsigned எழுத்து *)peer->ssl_peer_சாவி,
                       strlen(peer->ssl_peer_சாவி),
                       peer->சாவி_hmac);
  எனில்(!result)
    peer->hmac_set = TRUE;
  திரும்பு result;
}

நிலையான CURLcode cf_ssl_find_peer_by_hmac(கட்டமைப்பு Curl_ssl_scache *scache,
                                         மாறிலி unsigned எழுத்து *salt,
                                         மாறிலி unsigned எழுத்து *hmac,
                                         கட்டமைப்பு Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = வெற்று;
  எனில்(!GOOD_SCACHE(scache))
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  க்கு(i = 0; scache && i < scache->peer_மொத்தம்; i++) {
    கட்டமைப்பு Curl_ssl_scache_peer *peer = &scache->peers[i];
    எனில்(!cf_ssl_scache_match_auth(peer, வெற்று))
      தொடர்;
    எனில்(scache->peers[i].hmac_set &&
       !memcmp(peer->சாவி_salt, salt, அளவு(peer->சாவி_salt)) &&
       !memcmp(peer->சாவி_hmac, hmac, அளவு(peer->சாவி_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      செல் out;
    }
    இல்லையெனில் எனில்(peer->ssl_peer_சாவி) {
      unsigned எழுத்து my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, அளவு(peer->சாவி_salt),
                           (மாறிலி unsigned எழுத்து *)peer->ssl_peer_சாவி,
                           strlen(peer->ssl_peer_சாவி),
                           my_hmac);
      எனில்(result)
        செல் out;
      எனில்(!memcmp(my_hmac, hmac, அளவு(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        எனில்(!peer->hmac_set) {
          memcpy(peer->சாவி_salt, salt, அளவு(peer->சாவி_salt));
          memcpy(peer->சாவி_hmac, hmac, அளவு(peer->சாவி_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        செல் out;
      }
    }
  }
out:
  திரும்பு result;
}

CURLcode Curl_ssl_session_import(கட்டமைப்பு Curl_easy *data,
                                 மாறிலி எழுத்து *ssl_peer_சாவி,
                                 மாறிலி unsigned எழுத்து *shmac, size_t shmac_len,
                                 மாறிலி வெறுமை *sdata, size_t sdata_len)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  கட்டமைப்பு Curl_ssl_scache_peer *peer = வெற்று;
  கட்டமைப்பு Curl_ssl_session *s = வெற்று;
  bool have_lock = FALSE;
  CURLcode result;

  எனில்(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }
  எனில்(!ssl_peer_சாவி && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  எனில்(result)
    செல் out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  எனில்(ssl_peer_சாவி) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_சாவி, வெற்று, &peer);
    எனில்(result)
      செல் out;
  }
  இல்லையெனில் எனில்(shmac_len != (அளவு(peer->சாவி_salt) + அளவு(peer->சாவி_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    செல் out;
  }
  இல்லையெனில் {
    மாறிலி unsigned எழுத்து *salt = shmac;
    மாறிலி unsigned எழுத்து *hmac = shmac + அளவு(peer->சாவி_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    எனில்(result)
      செல் out;
    எனில்(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      எனில்(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_சாவி, வெற்று, வெற்று,
                                         salt, hmac);
        எனில்(result)
          செல் out;
      }
    }
  }

  எனில்(peer) {
    cf_scache_peer_add_session(peer, s, time(வெற்று));
    s = வெற்று; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_சாவி ? peer->ssl_peer_சாவி : "without key",
                  Curl_llist_மொத்தம்(&peer->sessions));
  }

out:
  எனில்(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  திரும்பு result;
}

CURLcode Curl_ssl_session_export(கட்டமைப்பு Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 வெறுமை *userptr)
{
  கட்டமைப்பு Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  கட்டமைப்பு Curl_ssl_scache_peer *peer;
  கட்டமைப்பு dynbuf sbuf, hbuf;
  கட்டமைப்பு Curl_llist_முனை *n;
  size_t i;
  curl_off_t now = time(வெற்று);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  எனில்(!export_fn)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  க்கு(i = 0; scache && i < scache->peer_மொத்தம்; i++) {
    peer = &scache->peers[i];
    எனில்(!peer->ssl_peer_சாவி && !peer->hmac_set)
      தொடர்; /* skip free entry */
    எனில்(!peer->exportable)
      தொடர்;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    எனில்(n)
      VERBOSE(++npeers);
    போது(n) {
      கட்டமைப்பு Curl_ssl_session *s = Curl_முனை_elem(n);
      எனில்(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        எனில்(result)
          செல் out;
      }
      எனில்(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->சாவி_salt, அளவு(peer->சாவி_salt));
        எனில்(result)
          செல் out;
        result = curlx_dyn_addn(&hbuf, peer->சாவி_hmac, அளவு(peer->சாவி_hmac));
        எனில்(result)
          செல் out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      எனில்(result)
        செல் out;

      result = export_fn(data, userptr, peer->ssl_peer_சாவி,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      எனில்(result)
        செல் out;
      VERBOSE(++ntickets);
      n = Curl_முனை_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  திரும்பு result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
