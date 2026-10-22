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
#অন্তর্ভাব "curl_setup.h"

#ifdef USE_SSL

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "cfilters.h"

#অন্তর্ভাব "vtls/vtls.h" /* generic SSL protos etc */
#অন্তর্ভাব "vtls/vtls_int.h"
#অন্তর্ভাব "vtls/vtls_scache.h"
#অন্তর্ভাব "vtls/vtls_spack.h"

#অন্তর্ভাব "strcase.h"
#অন্তর্ভাব "url.h"
#অন্তর্ভাব "llist.h"
#অন্তর্ভাব "curl_share.h"
#অন্তর্ভাব "curl_threads.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "curl_sha256.h"
#অন্তর্ভাব "rand.h"
#অন্তর্ভাব "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
সংরচনা Curl_ssl_scache_peer {
  বর্ণ *ssl_peer_চাবি;      /* id for peer + relevant TLS configuration */
  বর্ণ *clientcert;
  বর্ণ *চাবি_passwd;
  সংরচনা Curl_llist sessions;
  শূন্য *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned বর্ণ চাবি_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned বর্ণ চাবি_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
স্থির CURLcode cf_ssl_peer_চাবি_add_string(সংরচনা dynbuf *buf,
                                           ধ্রুব বর্ণ *name,
                                           ধ্রুব বর্ণ *মান)
{
  প্রত্যাগম curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(মান), মান);
}

স্থির CURLcode cf_ssl_peer_চাবি_add_path(সংরচনা dynbuf *buf,
                                         ধ্রুব বর্ণ *name,
                                         ধ্রুব বর্ণ *path,
                                         bool *is_local)
{
  যদি(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    বর্ণ *abspath = _fullpath(রিক্ত, path, 0);
    যদি(abspath) {
      CURLcode result = cf_ssl_peer_চাবি_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      মুক্ত(abspath); /* allocated by CRT, use system free() */
      প্রত্যাগম result;
    }
    *is_local = TRUE;
#নোচেদ্যদি defined(HAVE_REALPATH)
    যদি(path[0] != '/') {
      বর্ণ *abspath = realpath(path, রিক্ত);
      যদি(abspath) {
        CURLcode result = cf_ssl_peer_চাবি_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        মুক্ত(abspath); /* allocated by libc, free without memdebug */
        প্রত্যাগম result;
      }
      *is_local = TRUE;
    }
#endif
    প্রত্যাগম cf_ssl_peer_চাবি_add_string(buf, name, path);
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode cf_ssl_peer_চাবি_add_hash(সংরচনা dynbuf *buf,
                                         ধ্রুব বর্ণ *name,
                                         সংরচনা curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  যদি(blob && blob->len) {
    unsigned বর্ণ hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    যদি(result)
      গচ্ছ out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    যদি(result)
      গচ্ছ out;
    প্রতি(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      যদি(result)
        গচ্ছ out;
    }
  }
out:
  প্রত্যাগম result;
}

স্থির CURLcode cf_ssl_peer_চাবি_add_mtls(সংরচনা dynbuf *buf,
                                         সংরচনা ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  যদি(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_চাবি_add_path(buf, "CCERT", ssl->clientcert, is_local);
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->চাবি && ssl->চাবি[0]) {
    result = cf_ssl_peer_চাবি_add_path(buf, "KEY", ssl->চাবি, is_local);
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->চাবি_blob) {
    result = cf_ssl_peer_চাবি_add_hash(buf, "KEYBlob", ssl->চাবি_blob);
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    প্রতি(i = 0; !result && ssl->cert_type[i]; i++) {
      বর্ণ c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->চাবি_type && ssl->চাবি_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->চাবি_type));
    প্রতি(i = 0; !result && ssl->চাবি_type[i]; i++) {
      বর্ণ c = Curl_raw_toupper(ssl->চাবি_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  প্রত্যাগম result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

স্থির bool cf_ssl_peer_চাবি_is_global(ধ্রুব বর্ণ *peer_চাবি)
{
  size_t len = peer_চাবি ? strlen(peer_চাবি) : 0;
  প্রত্যাগম (len > 2) &&
         (peer_চাবি[len - 1] == 'G') &&
         (peer_চাবি[len - 2] == ':');
}

স্থির CURLcode ssl_peer_চাবি_add_transport(সংরচনা dynbuf *buf,
                                           uint8_t transport)
{
  বিকল্প(transport) {
  পক্ষ TRNSPRT_TCP:
    প্রত্যাগম CURLE_OK;
  পক্ষ TRNSPRT_UDP:
    প্রত্যাগম curlx_dyn_add(buf, ":UDP");
  পক্ষ TRNSPRT_QUIC:
    প্রত্যাগম curlx_dyn_add(buf, ":QUIC");
  পক্ষ TRNSPRT_UNIX:
    প্রত্যাগম curlx_dyn_add(buf, ":UNIX");
  স্বভাব:
    প্রত্যাগম curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

স্থির CURLcode ssl_peer_চাবি_add_vrfy(সংরচনা dynbuf *buf,
                                      সংরচনা ssl_filter_config *ssl,
                                      ধ্রুব সংরচনা ssl_peer *peer)
{
  CURLcode result;

  যদি(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    যদি(result)
      প্রত্যাগম result;
  }
  যদি(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    যদি(result)
      প্রত্যাগম result;
  }
  যদি(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    যদি(result)
      প্রত্যাগম result;
  }
  যদি((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_চাবি_add_string(buf, "CHOST", peer->peer->hostname);
    যদি(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    যদি(result)
      প্রত্যাগম result;
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode ssl_peer_চাবি_build(সংরচনা ssl_filter_config *ssl,
                                   ধ্রুব সংরচনা ssl_peer *peer,
                                   ধ্রুব বর্ণ *tls_id,
                                   বর্ণ **ppeer_চাবি)
{
  সংরচনা dynbuf buf;
  size_t চাবি_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_চাবি = রিক্ত;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  যদি(result)
    গচ্ছ out;
  result = ssl_peer_চাবি_add_transport(&buf, peer->transport);
  যদি(result)
    গচ্ছ out;
  result = ssl_peer_চাবি_add_vrfy(&buf, ssl, peer);
  যদি(result)
    গচ্ছ out;
  যদি(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->cipher_list) {
    result = cf_ssl_peer_চাবি_add_string(&buf, "CIPHER", ssl->cipher_list);
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->cipher_list13) {
    result = cf_ssl_peer_চাবি_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->curves) {
    result = cf_ssl_peer_চাবি_add_string(&buf, "CURVES", ssl->curves);
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->signature_algorithms) {
    result = cf_ssl_peer_চাবি_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->verifypeer) {
    result = cf_ssl_peer_চাবি_add_path(&buf, "CA", ssl->CAfile, &is_local);
    যদি(!result)
      result = cf_ssl_peer_চাবি_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    যদি(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    যদি(!result)
      result = cf_ssl_peer_চাবি_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    যদি(result)
      গচ্ছ out;
    result = cf_ssl_peer_চাবি_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    যদি(result)
      গচ্ছ out;
    যদি(ssl->ca_info_blob) {
      result = cf_ssl_peer_চাবি_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      যদি(result)
        গচ্ছ out;
    }
    যদি(ssl->issuercert_blob) {
      result = cf_ssl_peer_চাবি_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      যদি(result)
        গচ্ছ out;
    }
  }
  যদি(ssl->cert_blob) {
    result = cf_ssl_peer_চাবি_add_hash(&buf, "CertBlob", ssl->cert_blob);
    যদি(result)
      গচ্ছ out;
  }
  যদি(ssl->pinned_চাবি && ssl->pinned_চাবি[0]) {
    result = cf_ssl_peer_চাবি_add_string(&buf, "Pinned", ssl->pinned_চাবি);
    যদি(result)
      গচ্ছ out;
  }

  result = cf_ssl_peer_চাবি_add_mtls(&buf, ssl, &is_local);
  যদি(result)
    গচ্ছ out;

  যদি(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    গচ্ছ out;
  }
  result = cf_ssl_peer_চাবি_add_string(&buf, "IMPL", tls_id);
  যদি(result)
    গচ্ছ out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  যদি(result)
    গচ্ছ out;

  *ppeer_চাবি = curlx_dyn_take(&buf, &চাবি_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  প্রত্যাগম result;
}

CURLcode Curl_ssl_peer_চাবি_make(ধ্রুব সংরচনা ssl_peer *peer,
                                সংরচনা ssl_filter_config *sslc,
                                ধ্রুব বর্ণ *tls_id,
                                বর্ণ **ppeer_চাবি)
{
  প্রত্যাগম ssl_peer_চাবি_build(sslc, peer, tls_id, ppeer_চাবি);
}

সংরচনা Curl_ssl_scache {
  unsigned পূর্ণাঙ্ক magic;
  সংরচনা Curl_ssl_scache_peer *peers;
  size_t peer_গুনতি;
  পূর্ণাঙ্ক default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

স্থির সংরচনা Curl_ssl_scache *cf_ssl_scache_get(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_ssl_scache *scache = রিক্ত;
  /* If a share is present, its ssl_scache has preference over the multi */
  যদি(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  অন্যথা যদি(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  যদি(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (শূন্য *)scache);
    DEBUGASSERT(0);
    প্রত্যাগম রিক্ত;
  }
  প্রত্যাগম scache;
}

স্থির শূন্য cf_ssl_scache_session_ldestroy(শূন্য *udata, শূন্য *obj)
{
  সংরচনা Curl_ssl_session *s = obj;
  (শূন্য)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(শূন্য *sdata, size_t sdata_len,
                                 পূর্ণাঙ্ক ietf_tls_id, ধ্রুব বর্ণ *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 সংরচনা Curl_ssl_session **psession)
{
  প্রত্যাগম Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  রিক্ত, 0, psession);
}

CURLcode Curl_ssl_session_create2(শূন্য *sdata, size_t sdata_len,
                                  পূর্ণাঙ্ক ietf_tls_id, ধ্রুব বর্ণ *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned বর্ণ *quic_tp, size_t quic_tp_len,
                                  সংরচনা Curl_ssl_session **psession)
{
  সংরচনা Curl_ssl_session *s;

  যদি(!sdata || !sdata_len) {
    curlx_free(sdata);
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = রিক্ত;
  s = curlx_calloc(1, পরিমাণ(*s));
  যদি(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  যদি(alpn) {
    s->alpn = curlx_strdup(alpn);
    যদি(!s->alpn) {
      cf_ssl_scache_session_ldestroy(রিক্ত, s);
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  প্রত্যাগম CURLE_OK;
}

শূন্য Curl_ssl_session_destroy(সংরচনা Curl_ssl_session *s)
{
  যদি(s) {
    /* if in the list, the list destructor takes care of it */
    যদি(Curl_গ্রন্থি_llist(&s->list))
      Curl_গ্রন্থি_remove(&s->list);
    অন্যথা {
      cf_ssl_scache_session_ldestroy(রিক্ত, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(সংরচনা Curl_ssl_session *src,
                              সংরচনা Curl_ssl_session **pdest)
{
  সংরচনা Curl_ssl_session *dest = রিক্ত;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  যদি(!src || !pdest)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = রিক্ত;

  dest = curlx_calloc(1, পরিমাণ(*dest));
  যদি(!dest)
    গচ্ছ out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  যদি(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    যদি(!dest->sdata)
      গচ্ছ out;
    dest->sdata_len = src->sdata_len;
  }
  যদি(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    যদি(!dest->quic_tp)
      গচ্ছ out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  যদি(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    যদি(!dest->alpn)
      গচ্ছ out;
  }
  result = CURLE_OK;

out:
  যদি(!result)
    *pdest = dest;
  অন্যথা {
    *pdest = রিক্ত;
    যদি(dest)
      Curl_ssl_session_destroy(dest);
  }
  প্রত্যাগম result;
}

স্থির শূন্য cf_ssl_scache_clear_peer(সংরচনা Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, রিক্ত);
  যদি(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    যদি(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = রিক্ত;
  }
  peer->sobj_free = রিক্ত;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->চাবি_passwd);
  curlx_safefree(peer->ssl_peer_চাবি);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

স্থির শূন্য cf_ssl_scache_peer_set_obj(সংরচনা Curl_ssl_scache_peer *peer,
                                       শূন্য *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  যদি(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

স্থির শূন্য cf_ssl_cache_peer_update(সংরচনা Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->চাবি_passwd &&
                      (!peer->ssl_peer_চাবি ||
                       cf_ssl_peer_চাবি_is_global(peer->ssl_peer_চাবি)));
}

স্থির CURLcode cf_ssl_scache_peer_init(সংরচনা Curl_ssl_scache_peer *peer,
                                        ধ্রুব বর্ণ *ssl_peer_চাবি,
                                        ধ্রুব বর্ণ *clientcert,
                                        ধ্রুব বর্ণ *চাবি_passwd,
                                        ধ্রুব unsigned বর্ণ *salt,
                                        ধ্রুব unsigned বর্ণ *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_চাবি);
  যদি(ssl_peer_চাবি) {
    peer->ssl_peer_চাবি = curlx_strdup(ssl_peer_চাবি);
    যদি(!peer->ssl_peer_চাবি)
      গচ্ছ out;
    peer->hmac_set = FALSE;
  }
  অন্যথা যদি(salt && hmac) {
    memcpy(peer->চাবি_salt, salt, পরিমাণ(peer->চাবি_salt));
    memcpy(peer->চাবি_hmac, hmac, পরিমাণ(peer->চাবি_hmac));
    peer->hmac_set = TRUE;
  }
  অন্যথা {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ out;
  }
  যদি(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    যদি(!peer->clientcert)
      গচ্ছ out;
  }
  যদি(চাবি_passwd) {
    peer->চাবি_passwd = curlx_strdup(চাবি_passwd);
    যদি(!peer->চাবি_passwd)
      গচ্ছ out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  যদি(result)
    cf_ssl_scache_clear_peer(peer);
  প্রত্যাগম result;
}

স্থির শূন্য cf_scache_session_remove(সংরচনা Curl_ssl_scache_peer *peer,
                                     সংরচনা Curl_ssl_session *s)
{
  (শূন্য)peer;
  DEBUGASSERT(Curl_গ্রন্থি_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

স্থির bool cf_scache_session_expired(সংরচনা Curl_ssl_session *s,
                                      curl_off_t now)
{
  প্রত্যাগম (s->valid_until > 0) && (s->valid_until < now);
}

স্থির শূন্য cf_scache_peer_remove_expired(সংরচনা Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  সংরচনা Curl_llist_গ্রন্থি *n = Curl_llist_head(&peer->sessions);
  যাবত্(n) {
    সংরচনা Curl_ssl_session *s = Curl_গ্রন্থি_elem(n);
    n = Curl_গ্রন্থি_next(n);
    যদি(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

স্থির শূন্য cf_scache_peer_remove_non13(সংরচনা Curl_ssl_scache_peer *peer)
{
  সংরচনা Curl_llist_গ্রন্থি *n = Curl_llist_head(&peer->sessions);
  যাবত্(n) {
    সংরচনা Curl_ssl_session *s = Curl_গ্রন্থি_elem(n);
    n = Curl_গ্রন্থি_next(n);
    যদি(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                সংরচনা Curl_ssl_scache **pscache)
{
  সংরচনা Curl_ssl_scache *scache;
  সংরচনা Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = রিক্ত;
  peers = curlx_calloc(max_peers, পরিমাণ(*peers));
  যদি(!peers)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, পরিমাণ(*scache));
  যদি(!scache) {
    curlx_free(peers);
    প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_গুনতি = max_peers;
  scache->peers = peers;
  scache->age = 1;
  প্রতি(i = 0; i < scache->peer_গুনতি; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  প্রত্যাগম CURLE_OK;
}

শূন্য Curl_ssl_scache_destroy(সংরচনা Curl_ssl_scache *scache)
{
  যদি(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    প্রতি(i = 0; i < scache->peer_গুনতি; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(সংরচনা Curl_cfilter *cf, সংরচনা Curl_easy *data)
{
  যদি(cf_ssl_scache_get(data)) {
    সংরচনা ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    প্রত্যাগম conn_config ? conn_config->cache_session : FALSE;
  }
  প্রত্যাগম FALSE;
}

/* Lock shared SSL session data */
শূন্য Curl_ssl_scache_lock(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  যদি(scache) {
    যদি(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#অন্যথা
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
শূন্য Curl_ssl_scache_unlock(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  যদি(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#অন্যথা
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    যদি(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(সংরচনা Curl_easy *data)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  যদি(!scache)
    প্রত্যাগম FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#অন্যথা
  locked = (bool)scache->is_locked;
#endif
  প্রত্যাগম locked;
}

স্থির bool cf_ssl_scache_match_auth(সংরচনা Curl_ssl_scache_peer *peer,
                                     সংরচনা ssl_filter_config *conn_config)
{
  যদি(!conn_config) {
    যদি(peer->clientcert || peer->চাবি_passwd)
      প্রত্যাগম FALSE;
    প্রত্যাগম TRUE;
  }
  অন্যথা যদি(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    প্রত্যাগম FALSE;
  যদি(Curl_timestrcmp(peer->চাবি_passwd, conn_config->চাবি_passwd))
    প্রত্যাগম FALSE;
  প্রত্যাগম TRUE;
}

স্থির CURLcode cf_ssl_find_peer_by_চাবি(সংরচনা Curl_easy *data,
                                        সংরচনা Curl_ssl_scache *scache,
                                        ধ্রুব বর্ণ *ssl_peer_চাবি,
                                        সংরচনা ssl_filter_config *conn_config,
                                        সংরচনা Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_চাবি_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = রিক্ত;
  যদি(!GOOD_SCACHE(scache)) {
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_চাবি, scache->peer_গুনতি);

  /* check for entries with known peer_key */
  প্রতি(i = 0; scache && i < scache->peer_গুনতি; i++) {
    যদি(scache->peers[i].ssl_peer_চাবি &&
       !strcmp(ssl_peer_চাবি, scache->peers[i].ssl_peer_চাবি) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      গচ্ছ out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  প্রতি(i = 0; scache && i < scache->peer_গুনতি; i++) {
    যদি(!scache->peers[i].ssl_peer_চাবি &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned বর্ণ my_hmac[CURL_SHA256_DIGEST_LENGTH];
      যদি(!peer_চাবি_len) /* we are lazy */
        peer_চাবি_len = strlen(ssl_peer_চাবি);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].চাবি_salt,
                           পরিমাণ(scache->peers[i].চাবি_salt),
                           (ধ্রুব unsigned বর্ণ *)ssl_peer_চাবি,
                           peer_চাবি_len,
                           my_hmac);
      যদি(result)
        গচ্ছ out;
      যদি(!memcmp(scache->peers[i].চাবি_hmac, my_hmac, পরিমাণ(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_চাবি);
        scache->peers[i].ssl_peer_চাবি = curlx_strdup(ssl_peer_চাবি);
        যদি(!scache->peers[i].ssl_peer_চাবি) {
          result = CURLE_OUT_OF_MEMORY;
          গচ্ছ out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        গচ্ছ out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_চাবি);
out:
  প্রত্যাগম result;
}

স্থির সংরচনা Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  সংরচনা Curl_ssl_scache *scache)
{
  সংরচনা Curl_ssl_scache_peer *peer = রিক্ত;
  size_t i;

  /* find empty or oldest peer */
  প্রতি(i = 0; i < scache->peer_গুনতি; ++i) {
    /* free peer entry? */
    যদি(!scache->peers[i].ssl_peer_চাবি && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      ভঙ্গ;
    }
    /* peer without sessions and obj */
    যদি(!scache->peers[i].sobj &&
       !Curl_llist_গুনতি(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      ভঙ্গ;
    }
    /* remember "oldest" peer */
    যদি(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  যদি(peer)
    cf_ssl_scache_clear_peer(peer);
  প্রত্যাগম peer;
}

স্থির CURLcode cf_ssl_add_peer(সংরচনা Curl_easy *data,
                                সংরচনা Curl_ssl_scache *scache,
                                ধ্রুব বর্ণ *ssl_peer_চাবি,
                                সংরচনা ssl_filter_config *conn_config,
                                সংরচনা Curl_ssl_scache_peer **ppeer)
{
  সংরচনা Curl_ssl_scache_peer *peer = রিক্ত;
  CURLcode result = CURLE_OK;

  *ppeer = রিক্ত;
  যদি(ssl_peer_চাবি) {
    result = cf_ssl_find_peer_by_চাবি(data, scache, ssl_peer_চাবি, conn_config,
                                     &peer);
    যদি(result || !scache->peer_গুনতি)
      প্রত্যাগম result;
  }

  যদি(peer) {
    *ppeer = peer;
    প্রত্যাগম CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  যদি(peer) {
    বর্ণ buffer[64];
    ধ্রুব বর্ণ *ccert = conn_config ? conn_config->clientcert : রিক্ত;
    ধ্রুব বর্ণ *kpasswd = conn_config ? conn_config->চাবি_passwd : রিক্ত;
    যদি(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, পরিমাণ(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_চাবি, ccert, kpasswd,
                                     রিক্ত, রিক্ত);
    যদি(result)
      গচ্ছ out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  যদি(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  প্রত্যাগম result;
}

স্থির শূন্য cf_scache_peer_add_session(সংরচনা Curl_ssl_scache_peer *peer,
                                       সংরচনা Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  যদি(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, রিক্ত);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  অন্যথা {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    যাবত্(Curl_llist_গুনতি(&peer->sessions) > peer->max_sessions) {
      Curl_গ্রন্থি_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

স্থির CURLcode cf_scache_add_session(সংরচনা Curl_cfilter *cf,
                                      সংরচনা Curl_easy *data,
                                      সংরচনা Curl_ssl_scache *scache,
                                      ধ্রুব বর্ণ *ssl_peer_চাবি,
                                      সংরচনা Curl_ssl_session *s)
{
  সংরচনা Curl_ssl_scache_peer *peer = রিক্ত;
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(রিক্ত);
  curl_off_t max_lifetime;

  যদি(!scache || !scache->peer_গুনতি) {
    Curl_ssl_session_destroy(s);
    প্রত্যাগম CURLE_OK;
  }

  যদি(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  যদি(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  যদি(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    প্রত্যাগম CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_চাবি, conn_config, &peer);
  যদি(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (পূর্ণাঙ্ক)result);
    Curl_ssl_session_destroy(s);
    গচ্ছ out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  যদি(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_চাবি, (পূর্ণাঙ্ক)result);
  }
  অন্যথা
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_চাবি,
                  (unsigned পূর্ণাঙ্ক)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_গুনতি(&peer->sessions) : 0);
  প্রত্যাগম result;
}

CURLcode Curl_ssl_scache_put(সংরচনা Curl_cfilter *cf,
                             সংরচনা Curl_easy *data,
                             ধ্রুব বর্ণ *ssl_peer_চাবি,
                             সংরচনা Curl_ssl_session *s)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  যদি(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    প্রত্যাগম CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_চাবি, s);
  Curl_ssl_scache_unlock(data);
  প্রত্যাগম result;
}

শূন্য Curl_ssl_scache_return(সংরচনা Curl_cfilter *cf,
                            সংরচনা Curl_easy *data,
                            ধ্রুব বর্ণ *ssl_peer_চাবি,
                            সংরচনা Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  যদি(s && s->ietf_tls_id < 0x304)
    (শূন্য)Curl_ssl_scache_put(cf, data, ssl_peer_চাবি, s);
  অন্যথা
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *ssl_peer_চাবি,
                              সংরচনা Curl_ssl_session **ps)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা Curl_ssl_scache_peer *peer = রিক্ত;
  সংরচনা Curl_llist_গ্রন্থি *n;
  সংরচনা Curl_ssl_session *s = রিক্ত;
  CURLcode result;

  *ps = রিক্ত;
  যদি(!scache)
    প্রত্যাগম CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_চাবি(data, scache, ssl_peer_চাবি, conn_config,
                                   &peer);
  যদি(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(রিক্ত));
    n = Curl_llist_head(&peer->sessions);
    যদি(n) {
      s = Curl_গ্রন্থি_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  যদি(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_চাবি, (unsigned পূর্ণাঙ্ক)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_গুনতি(&peer->sessions));
  }
  অন্যথা {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_চাবি);
  }
  Curl_ssl_scache_unlock(data);
  প্রত্যাগম result;
}

CURLcode Curl_ssl_scache_add_obj(সংরচনা Curl_cfilter *cf,
                                 সংরচনা Curl_easy *data,
                                 ধ্রুব বর্ণ *ssl_peer_চাবি,
                                 শূন্য *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা Curl_ssl_scache_peer *peer = রিক্ত;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  যদি(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_চাবি, conn_config, &peer);
  যদি(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (পূর্ণাঙ্ক)result);
    গচ্ছ out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = রিক্ত;  /* peer took ownership */

out:
  যদি(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  প্রত্যাগম result;
}

শূন্য *Curl_ssl_scache_get_obj(সংরচনা Curl_cfilter *cf,
                              সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ *ssl_peer_চাবি)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা Curl_ssl_scache_peer *peer = রিক্ত;
  CURLcode result;
  শূন্য *sobj;

  যদি(!scache)
    প্রত্যাগম রিক্ত;

  result = cf_ssl_find_peer_by_চাবি(data, scache, ssl_peer_চাবি, conn_config,
                                   &peer);
  যদি(result)
    প্রত্যাগম রিক্ত;

  sobj = peer ? peer->sobj : রিক্ত;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_চাবি);
  প্রত্যাগম sobj;
}

শূন্য Curl_ssl_scache_remove_all(সংরচনা Curl_cfilter *cf,
                                সংরচনা Curl_easy *data,
                                ধ্রুব বর্ণ *ssl_peer_চাবি)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  সংরচনা ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  সংরচনা Curl_ssl_scache_peer *peer = রিক্ত;
  CURLcode result;

  (শূন্য)cf;
  যদি(!scache)
    প্রত্যাগম;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_চাবি(data, scache, ssl_peer_চাবি, conn_config,
                                   &peer);
  যদি(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

স্থির CURLcode cf_ssl_scache_peer_set_hmac(সংরচনা Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  যদি(!peer->ssl_peer_চাবি)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(রিক্ত, peer->চাবি_salt, পরিমাণ(peer->চাবি_salt));
  যদি(result)
    প্রত্যাগম result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->চাবি_salt, পরিমাণ(peer->চাবি_salt),
                       (ধ্রুব unsigned বর্ণ *)peer->ssl_peer_চাবি,
                       strlen(peer->ssl_peer_চাবি),
                       peer->চাবি_hmac);
  যদি(!result)
    peer->hmac_set = TRUE;
  প্রত্যাগম result;
}

স্থির CURLcode cf_ssl_find_peer_by_hmac(সংরচনা Curl_ssl_scache *scache,
                                         ধ্রুব unsigned বর্ণ *salt,
                                         ধ্রুব unsigned বর্ণ *hmac,
                                         সংরচনা Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = রিক্ত;
  যদি(!GOOD_SCACHE(scache))
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  প্রতি(i = 0; scache && i < scache->peer_গুনতি; i++) {
    সংরচনা Curl_ssl_scache_peer *peer = &scache->peers[i];
    যদি(!cf_ssl_scache_match_auth(peer, রিক্ত))
      অনুবর্ত;
    যদি(scache->peers[i].hmac_set &&
       !memcmp(peer->চাবি_salt, salt, পরিমাণ(peer->চাবি_salt)) &&
       !memcmp(peer->চাবি_hmac, hmac, পরিমাণ(peer->চাবি_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      গচ্ছ out;
    }
    অন্যথা যদি(peer->ssl_peer_চাবি) {
      unsigned বর্ণ my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, পরিমাণ(peer->চাবি_salt),
                           (ধ্রুব unsigned বর্ণ *)peer->ssl_peer_চাবি,
                           strlen(peer->ssl_peer_চাবি),
                           my_hmac);
      যদি(result)
        গচ্ছ out;
      যদি(!memcmp(my_hmac, hmac, পরিমাণ(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        যদি(!peer->hmac_set) {
          memcpy(peer->চাবি_salt, salt, পরিমাণ(peer->চাবি_salt));
          memcpy(peer->চাবি_hmac, hmac, পরিমাণ(peer->চাবি_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        গচ্ছ out;
      }
    }
  }
out:
  প্রত্যাগম result;
}

CURLcode Curl_ssl_session_import(সংরচনা Curl_easy *data,
                                 ধ্রুব বর্ণ *ssl_peer_চাবি,
                                 ধ্রুব unsigned বর্ণ *shmac, size_t shmac_len,
                                 ধ্রুব শূন্য *sdata, size_t sdata_len)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  সংরচনা Curl_ssl_scache_peer *peer = রিক্ত;
  সংরচনা Curl_ssl_session *s = রিক্ত;
  bool have_lock = FALSE;
  CURLcode result;

  যদি(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ out;
  }
  যদি(!ssl_peer_চাবি && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  যদি(result)
    গচ্ছ out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  যদি(ssl_peer_চাবি) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_চাবি, রিক্ত, &peer);
    যদি(result)
      গচ্ছ out;
  }
  অন্যথা যদি(shmac_len != (পরিমাণ(peer->চাবি_salt) + পরিমাণ(peer->চাবি_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    গচ্ছ out;
  }
  অন্যথা {
    ধ্রুব unsigned বর্ণ *salt = shmac;
    ধ্রুব unsigned বর্ণ *hmac = shmac + পরিমাণ(peer->চাবি_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    যদি(result)
      গচ্ছ out;
    যদি(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      যদি(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_চাবি, রিক্ত, রিক্ত,
                                         salt, hmac);
        যদি(result)
          গচ্ছ out;
      }
    }
  }

  যদি(peer) {
    cf_scache_peer_add_session(peer, s, time(রিক্ত));
    s = রিক্ত; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_চাবি ? peer->ssl_peer_চাবি : "without key",
                  Curl_llist_গুনতি(&peer->sessions));
  }

out:
  যদি(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  প্রত্যাগম result;
}

CURLcode Curl_ssl_session_export(সংরচনা Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 শূন্য *userptr)
{
  সংরচনা Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  সংরচনা Curl_ssl_scache_peer *peer;
  সংরচনা dynbuf sbuf, hbuf;
  সংরচনা Curl_llist_গ্রন্থি *n;
  size_t i;
  curl_off_t now = time(রিক্ত);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  যদি(!export_fn)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  প্রতি(i = 0; scache && i < scache->peer_গুনতি; i++) {
    peer = &scache->peers[i];
    যদি(!peer->ssl_peer_চাবি && !peer->hmac_set)
      অনুবর্ত; /* skip free entry */
    যদি(!peer->exportable)
      অনুবর্ত;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    যদি(n)
      VERBOSE(++npeers);
    যাবত্(n) {
      সংরচনা Curl_ssl_session *s = Curl_গ্রন্থি_elem(n);
      যদি(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        যদি(result)
          গচ্ছ out;
      }
      যদি(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->চাবি_salt, পরিমাণ(peer->চাবি_salt));
        যদি(result)
          গচ্ছ out;
        result = curlx_dyn_addn(&hbuf, peer->চাবি_hmac, পরিমাণ(peer->চাবি_hmac));
        যদি(result)
          গচ্ছ out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      যদি(result)
        গচ্ছ out;

      result = export_fn(data, userptr, peer->ssl_peer_চাবি,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      যদি(result)
        গচ্ছ out;
      VERBOSE(++ntickets);
      n = Curl_গ্রন্থি_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  প্রত্যাগম result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
