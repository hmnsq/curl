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
কাঠামো Curl_ssl_scache_peer {
  অক্ষর *ssl_peer_চাবি;      /* id for peer + relevant TLS configuration */
  অক্ষর *clientcert;
  অক্ষর *চাবি_passwd;
  কাঠামো Curl_llist sessions;
  শূন্য *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned অক্ষর চাবি_salt[CURL_SHA256_DIGEST_দৈর্ঘ্য]; /* for entry export */
  unsigned অক্ষর চাবি_hmac[CURL_SHA256_DIGEST_দৈর্ঘ্য]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
স্থির CURLcode cf_ssl_peer_চাবি_add_string(কাঠামো dynbuf *buf,
                                           ধ্রুবক অক্ষর *name,
                                           ধ্রুবক অক্ষর *মান)
{
  ফেরত curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(মান), মান);
}

স্থির CURLcode cf_ssl_peer_চাবি_add_path(কাঠামো dynbuf *buf,
                                         ধ্রুবক অক্ষর *name,
                                         ধ্রুবক অক্ষর *path,
                                         bool *is_local)
{
  যদি(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    অক্ষর *abspath = _fullpath(ফাঁকা, path, 0);
    যদি(abspath) {
      CURLcode ফলাফল = cf_ssl_peer_চাবি_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      মুক্ত(abspath); /* allocated by CRT, use system free() */
      ফেরত ফলাফল;
    }
    *is_local = TRUE;
#নতুবা_যদি defined(HAVE_REALPATH)
    যদি(path[0] != '/') {
      অক্ষর *abspath = realpath(path, ফাঁকা);
      যদি(abspath) {
        CURLcode ফলাফল = cf_ssl_peer_চাবি_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        মুক্ত(abspath); /* allocated by libc, free without memdebug */
        ফেরত ফলাফল;
      }
      *is_local = TRUE;
    }
#endif
    ফেরত cf_ssl_peer_চাবি_add_string(buf, name, path);
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode cf_ssl_peer_চাবি_add_hash(কাঠামো dynbuf *buf,
                                         ধ্রুবক অক্ষর *name,
                                         কাঠামো curl_blob *blob)
{
  CURLcode ফলাফল = CURLE_OK;
  যদি(blob && blob->len) {
    unsigned অক্ষর hash[CURL_SHA256_DIGEST_দৈর্ঘ্য];
    size_t i;

    ফলাফল = curlx_dyn_addf(buf, ":%s-", name);
    যদি(ফলাফল)
      যাও out;
    ফলাফল = Curl_sha256it(hash, blob->তথ্য, blob->len);
    যদি(ফলাফল)
      যাও out;
    জন্য(i = 0; i < CURL_SHA256_DIGEST_দৈর্ঘ্য; ++i) {
      ফলাফল = curlx_dyn_addf(buf, "%02x", hash[i]);
      যদি(ফলাফল)
        যাও out;
    }
  }
out:
  ফেরত ফলাফল;
}

স্থির CURLcode cf_ssl_peer_চাবি_add_mtls(কাঠামো dynbuf *buf,
                                         কাঠামো ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode ফলাফল = CURLE_OK;
  যদি(ssl->clientcert && ssl->clientcert[0]) {
    ফলাফল = cf_ssl_peer_চাবি_add_path(buf, "CCERT", ssl->clientcert, is_local);
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->চাবি && ssl->চাবি[0]) {
    ফলাফল = cf_ssl_peer_চাবি_add_path(buf, "KEY", ssl->চাবি, is_local);
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->চাবি_blob) {
    ফলাফল = cf_ssl_peer_চাবি_add_hash(buf, "KEYBlob", ssl->চাবি_blob);
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    ফলাফল = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    জন্য(i = 0; !ফলাফল && ssl->cert_type[i]; i++) {
      অক্ষর c = Curl_raw_toupper(ssl->cert_type[i]);
      ফলাফল = curlx_dyn_addn(buf, &c, 1);
    }
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->চাবি_type && ssl->চাবি_type[0]) {
    size_t i;
    ফলাফল = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->চাবি_type));
    জন্য(i = 0; !ফলাফল && ssl->চাবি_type[i]; i++) {
      অক্ষর c = Curl_raw_toupper(ssl->চাবি_type[i]);
      ফলাফল = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  ফেরত ফলাফল;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

স্থির bool cf_ssl_peer_চাবি_is_global(ধ্রুবক অক্ষর *peer_চাবি)
{
  size_t len = peer_চাবি ? strlen(peer_চাবি) : 0;
  ফেরত (len > 2) &&
         (peer_চাবি[len - 1] == 'G') &&
         (peer_চাবি[len - 2] == ':');
}

স্থির CURLcode ssl_peer_চাবি_add_transport(কাঠামো dynbuf *buf,
                                           uint8_t transport)
{
  নির্বাচন(transport) {
  ক্ষেত্র TRNSPRT_TCP:
    ফেরত CURLE_OK;
  ক্ষেত্র TRNSPRT_UDP:
    ফেরত curlx_dyn_add(buf, ":UDP");
  ক্ষেত্র TRNSPRT_QUIC:
    ফেরত curlx_dyn_add(buf, ":QUIC");
  ক্ষেত্র TRNSPRT_UNIX:
    ফেরত curlx_dyn_add(buf, ":UNIX");
  পূর্বনির্ধারিত:
    ফেরত curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

স্থির CURLcode ssl_peer_চাবি_add_vrfy(কাঠামো dynbuf *buf,
                                      কাঠামো ssl_filter_config *ssl,
                                      ধ্রুবক কাঠামো ssl_peer *peer)
{
  CURLcode ফলাফল;

  যদি(!ssl->verifypeer) {
    ফলাফল = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  যদি(!ssl->verifyhost) {
    ফলাফল = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  যদি(ssl->verifystatus) {
    ফলাফল = curlx_dyn_add(buf, ":VRFY-STATUS");
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  যদি((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    ফলাফল = cf_ssl_peer_চাবি_add_string(buf, "CHOST", peer->peer->hostname);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    যদি(ফলাফল)
      ফেরত ফলাফল;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode ssl_peer_চাবি_build(কাঠামো ssl_filter_config *ssl,
                                   ধ্রুবক কাঠামো ssl_peer *peer,
                                   ধ্রুবক অক্ষর *tls_id,
                                   অক্ষর **ppeer_চাবি)
{
  কাঠামো dynbuf buf;
  size_t চাবি_len;
  bool is_local = FALSE;
  CURLcode ফলাফল;

  *ppeer_চাবি = ফাঁকা;
  curlx_dyn_init(&buf, 10 * 1024);

  ফলাফল = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  যদি(ফলাফল)
    যাও out;
  ফলাফল = ssl_peer_চাবি_add_transport(&buf, peer->transport);
  যদি(ফলাফল)
    যাও out;
  ফলাফল = ssl_peer_চাবি_add_vrfy(&buf, ssl, peer);
  যদি(ফলাফল)
    যাও out;
  যদি(ssl->version || ssl->version_max) {
    ফলাফল = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->ssl_options) {
    ফলাফল = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->cipher_list) {
    ফলাফল = cf_ssl_peer_চাবি_add_string(&buf, "CIPHER", ssl->cipher_list);
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->cipher_list13) {
    ফলাফল = cf_ssl_peer_চাবি_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->curves) {
    ফলাফল = cf_ssl_peer_চাবি_add_string(&buf, "CURVES", ssl->curves);
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->signature_algorithms) {
    ফলাফল = cf_ssl_peer_চাবি_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->verifypeer) {
    ফলাফল = cf_ssl_peer_চাবি_add_path(&buf, "CA", ssl->CAfile, &is_local);
    যদি(!ফলাফল)
      ফলাফল = cf_ssl_peer_চাবি_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    যদি(!ফলাফল && ssl->native_ca_store)
      ফলাফল = curlx_dyn_add(&buf, ":NATIVE-CA");
    যদি(!ফলাফল)
      ফলাফল = cf_ssl_peer_চাবি_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    যদি(ফলাফল)
      যাও out;
    ফলাফল = cf_ssl_peer_চাবি_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    যদি(ফলাফল)
      যাও out;
    যদি(ssl->ca_info_blob) {
      ফলাফল = cf_ssl_peer_চাবি_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      যদি(ফলাফল)
        যাও out;
    }
    যদি(ssl->issuercert_blob) {
      ফলাফল = cf_ssl_peer_চাবি_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      যদি(ফলাফল)
        যাও out;
    }
  }
  যদি(ssl->cert_blob) {
    ফলাফল = cf_ssl_peer_চাবি_add_hash(&buf, "CertBlob", ssl->cert_blob);
    যদি(ফলাফল)
      যাও out;
  }
  যদি(ssl->pinned_চাবি && ssl->pinned_চাবি[0]) {
    ফলাফল = cf_ssl_peer_চাবি_add_string(&buf, "Pinned", ssl->pinned_চাবি);
    যদি(ফলাফল)
      যাও out;
  }

  ফলাফল = cf_ssl_peer_চাবি_add_mtls(&buf, ssl, &is_local);
  যদি(ফলাফল)
    যাও out;

  যদি(!tls_id || !tls_id[0]) {
    ফলাফল = CURLE_FAILED_INIT;
    যাও out;
  }
  ফলাফল = cf_ssl_peer_চাবি_add_string(&buf, "IMPL", tls_id);
  যদি(ফলাফল)
    যাও out;

  ফলাফল = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  যদি(ফলাফল)
    যাও out;

  *ppeer_চাবি = curlx_dyn_take(&buf, &চাবি_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  ফেরত ফলাফল;
}

CURLcode Curl_ssl_peer_চাবি_make(ধ্রুবক কাঠামো ssl_peer *peer,
                                কাঠামো ssl_filter_config *sslc,
                                ধ্রুবক অক্ষর *tls_id,
                                অক্ষর **ppeer_চাবি)
{
  ফেরত ssl_peer_চাবি_build(sslc, peer, tls_id, ppeer_চাবি);
}

কাঠামো Curl_ssl_scache {
  unsigned পূর্ণসংখ্যা magic;
  কাঠামো Curl_ssl_scache_peer *peers;
  size_t peer_গুনতি;
  পূর্ণসংখ্যা default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

স্থির কাঠামো Curl_ssl_scache *cf_ssl_scache_get(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_ssl_scache *scache = ফাঁকা;
  /* If a share is present, its ssl_scache has preference over the multi */
  যদি(তথ্য->share && তথ্য->share->ssl_scache)
    scache = তথ্য->share->ssl_scache;
  অন্যথা যদি(তথ্য->multi && তথ্য->multi->ssl_scache)
    scache = তথ্য->multi->ssl_scache;
  যদি(scache && !GOOD_SCACHE(scache)) {
    failf(তথ্য, "transfer would use an invalid scache at %p, denied",
          (শূন্য *)scache);
    DEBUGASSERT(0);
    ফেরত ফাঁকা;
  }
  ফেরত scache;
}

স্থির শূন্য cf_ssl_scache_session_ldestroy(শূন্য *udata, শূন্য *obj)
{
  কাঠামো Curl_ssl_session *s = obj;
  (শূন্য)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(শূন্য *sdata, size_t sdata_len,
                                 পূর্ণসংখ্যা ietf_tls_id, ধ্রুবক অক্ষর *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 কাঠামো Curl_ssl_session **psession)
{
  ফেরত Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  ফাঁকা, 0, psession);
}

CURLcode Curl_ssl_session_create2(শূন্য *sdata, size_t sdata_len,
                                  পূর্ণসংখ্যা ietf_tls_id, ধ্রুবক অক্ষর *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned অক্ষর *quic_tp, size_t quic_tp_len,
                                  কাঠামো Curl_ssl_session **psession)
{
  কাঠামো Curl_ssl_session *s;

  যদি(!sdata || !sdata_len) {
    curlx_free(sdata);
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = ফাঁকা;
  s = curlx_calloc(1, আকার(*s));
  যদি(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    ফেরত CURLE_OUT_OF_MEMORY;
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
      cf_ssl_scache_session_ldestroy(ফাঁকা, s);
      ফেরত CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  ফেরত CURLE_OK;
}

শূন্য Curl_ssl_session_destroy(কাঠামো Curl_ssl_session *s)
{
  যদি(s) {
    /* if in the list, the list destructor takes care of it */
    যদি(Curl_গ্রন্থি_llist(&s->list))
      Curl_গ্রন্থি_remove(&s->list);
    অন্যথা {
      cf_ssl_scache_session_ldestroy(ফাঁকা, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(কাঠামো Curl_ssl_session *src,
                              কাঠামো Curl_ssl_session **pdest)
{
  কাঠামো Curl_ssl_session *dest = ফাঁকা;
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  যদি(!src || !pdest)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = ফাঁকা;

  dest = curlx_calloc(1, আকার(*dest));
  যদি(!dest)
    যাও out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  যদি(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    যদি(!dest->sdata)
      যাও out;
    dest->sdata_len = src->sdata_len;
  }
  যদি(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    যদি(!dest->quic_tp)
      যাও out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  যদি(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    যদি(!dest->alpn)
      যাও out;
  }
  ফলাফল = CURLE_OK;

out:
  যদি(!ফলাফল)
    *pdest = dest;
  অন্যথা {
    *pdest = ফাঁকা;
    যদি(dest)
      Curl_ssl_session_destroy(dest);
  }
  ফেরত ফলাফল;
}

স্থির শূন্য cf_ssl_scache_clear_peer(কাঠামো Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, ফাঁকা);
  যদি(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    যদি(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = ফাঁকা;
  }
  peer->sobj_free = ফাঁকা;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->চাবি_passwd);
  curlx_safefree(peer->ssl_peer_চাবি);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

স্থির শূন্য cf_ssl_scache_peer_set_obj(কাঠামো Curl_ssl_scache_peer *peer,
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

স্থির শূন্য cf_ssl_cache_peer_update(কাঠামো Curl_ssl_scache_peer *peer)
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

স্থির CURLcode cf_ssl_scache_peer_init(কাঠামো Curl_ssl_scache_peer *peer,
                                        ধ্রুবক অক্ষর *ssl_peer_চাবি,
                                        ধ্রুবক অক্ষর *clientcert,
                                        ধ্রুবক অক্ষর *চাবি_passwd,
                                        ধ্রুবক unsigned অক্ষর *salt,
                                        ধ্রুবক unsigned অক্ষর *hmac)
{
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_চাবি);
  যদি(ssl_peer_চাবি) {
    peer->ssl_peer_চাবি = curlx_strdup(ssl_peer_চাবি);
    যদি(!peer->ssl_peer_চাবি)
      যাও out;
    peer->hmac_set = FALSE;
  }
  অন্যথা যদি(salt && hmac) {
    memcpy(peer->চাবি_salt, salt, আকার(peer->চাবি_salt));
    memcpy(peer->চাবি_hmac, hmac, আকার(peer->চাবি_hmac));
    peer->hmac_set = TRUE;
  }
  অন্যথা {
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }
  যদি(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    যদি(!peer->clientcert)
      যাও out;
  }
  যদি(চাবি_passwd) {
    peer->চাবি_passwd = curlx_strdup(চাবি_passwd);
    যদি(!peer->চাবি_passwd)
      যাও out;
  }
  cf_ssl_cache_peer_update(peer);
  ফলাফল = CURLE_OK;
out:
  যদি(ফলাফল)
    cf_ssl_scache_clear_peer(peer);
  ফেরত ফলাফল;
}

স্থির শূন্য cf_scache_session_remove(কাঠামো Curl_ssl_scache_peer *peer,
                                     কাঠামো Curl_ssl_session *s)
{
  (শূন্য)peer;
  DEBUGASSERT(Curl_গ্রন্থি_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

স্থির bool cf_scache_session_expired(কাঠামো Curl_ssl_session *s,
                                      curl_off_t now)
{
  ফেরত (s->valid_until > 0) && (s->valid_until < now);
}

স্থির শূন্য cf_scache_peer_remove_expired(কাঠামো Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  কাঠামো Curl_llist_গ্রন্থি *n = Curl_llist_শীর্ষ(&peer->sessions);
  যখন(n) {
    কাঠামো Curl_ssl_session *s = Curl_গ্রন্থি_elem(n);
    n = Curl_গ্রন্থি_পরবর্তী(n);
    যদি(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

স্থির শূন্য cf_scache_peer_remove_non13(কাঠামো Curl_ssl_scache_peer *peer)
{
  কাঠামো Curl_llist_গ্রন্থি *n = Curl_llist_শীর্ষ(&peer->sessions);
  যখন(n) {
    কাঠামো Curl_ssl_session *s = Curl_গ্রন্থি_elem(n);
    n = Curl_গ্রন্থি_পরবর্তী(n);
    যদি(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                কাঠামো Curl_ssl_scache **pscache)
{
  কাঠামো Curl_ssl_scache *scache;
  কাঠামো Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = ফাঁকা;
  peers = curlx_calloc(max_peers, আকার(*peers));
  যদি(!peers)
    ফেরত CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, আকার(*scache));
  যদি(!scache) {
    curlx_free(peers);
    ফেরত CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_গুনতি = max_peers;
  scache->peers = peers;
  scache->age = 1;
  জন্য(i = 0; i < scache->peer_গুনতি; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  ফেরত CURLE_OK;
}

শূন্য Curl_ssl_scache_destroy(কাঠামো Curl_ssl_scache *scache)
{
  যদি(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    জন্য(i = 0; i < scache->peer_গুনতি; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(কাঠামো Curl_cfilter *cf, কাঠামো Curl_easy *তথ্য)
{
  যদি(cf_ssl_scache_get(তথ্য)) {
    কাঠামো ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    ফেরত conn_config ? conn_config->cache_session : FALSE;
  }
  ফেরত FALSE;
}

/* Lock shared SSL session data */
শূন্য Curl_ssl_scache_lock(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
  যদি(scache) {
    যদি(CURL_SHARE_ssl_scache(তথ্য))
      Curl_share_lock(তথ্য, CURL_LOCK_তথ্য_SSL_SESSION,
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
শূন্য Curl_ssl_scache_unlock(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
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
    যদি(CURL_SHARE_ssl_scache(তথ্য))
      Curl_share_unlock(তথ্য, CURL_LOCK_তথ্য_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
  bool locked = FALSE;
  যদি(!scache)
    ফেরত FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#অন্যথা
  locked = (bool)scache->is_locked;
#endif
  ফেরত locked;
}

স্থির bool cf_ssl_scache_match_auth(কাঠামো Curl_ssl_scache_peer *peer,
                                     কাঠামো ssl_filter_config *conn_config)
{
  যদি(!conn_config) {
    যদি(peer->clientcert || peer->চাবি_passwd)
      ফেরত FALSE;
    ফেরত TRUE;
  }
  অন্যথা যদি(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    ফেরত FALSE;
  যদি(Curl_timestrcmp(peer->চাবি_passwd, conn_config->চাবি_passwd))
    ফেরত FALSE;
  ফেরত TRUE;
}

স্থির CURLcode cf_ssl_find_peer_by_চাবি(কাঠামো Curl_easy *তথ্য,
                                        কাঠামো Curl_ssl_scache *scache,
                                        ধ্রুবক অক্ষর *ssl_peer_চাবি,
                                        কাঠামো ssl_filter_config *conn_config,
                                        কাঠামো Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_চাবি_len = 0;
  CURLcode ফলাফল = CURLE_OK;

  *ppeer = ফাঁকা;
  যদি(!GOOD_SCACHE(scache)) {
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(তথ্য, "find peer slot for %s among %zu slots",
                ssl_peer_চাবি, scache->peer_গুনতি);

  /* check for entries with known peer_key */
  জন্য(i = 0; scache && i < scache->peer_গুনতি; i++) {
    যদি(scache->peers[i].ssl_peer_চাবি &&
       !strcmp(ssl_peer_চাবি, scache->peers[i].ssl_peer_চাবি) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      যাও out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  জন্য(i = 0; scache && i < scache->peer_গুনতি; i++) {
    যদি(!scache->peers[i].ssl_peer_চাবি &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned অক্ষর my_hmac[CURL_SHA256_DIGEST_দৈর্ঘ্য];
      যদি(!peer_চাবি_len) /* we are lazy */
        peer_চাবি_len = strlen(ssl_peer_চাবি);
      ফলাফল = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].চাবি_salt,
                           আকার(scache->peers[i].চাবি_salt),
                           (ধ্রুবক unsigned অক্ষর *)ssl_peer_চাবি,
                           peer_চাবি_len,
                           my_hmac);
      যদি(ফলাফল)
        যাও out;
      যদি(!memcmp(scache->peers[i].চাবি_hmac, my_hmac, আকার(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(তথ্য, "peer entry %zu key recovered: %s",
                      i, ssl_peer_চাবি);
        scache->peers[i].ssl_peer_চাবি = curlx_strdup(ssl_peer_চাবি);
        যদি(!scache->peers[i].ssl_peer_চাবি) {
          ফলাফল = CURLE_OUT_OF_MEMORY;
          যাও out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        যাও out;
      }
    }
  }
  CURL_TRC_SSLS(তথ্য, "peer not found for %s", ssl_peer_চাবি);
out:
  ফেরত ফলাফল;
}

স্থির কাঠামো Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  কাঠামো Curl_ssl_scache *scache)
{
  কাঠামো Curl_ssl_scache_peer *peer = ফাঁকা;
  size_t i;

  /* find empty or oldest peer */
  জন্য(i = 0; i < scache->peer_গুনতি; ++i) {
    /* free peer entry? */
    যদি(!scache->peers[i].ssl_peer_চাবি && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      বিরতি;
    }
    /* peer without sessions and obj */
    যদি(!scache->peers[i].sobj &&
       !Curl_llist_গুনতি(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      বিরতি;
    }
    /* remember "oldest" peer */
    যদি(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  যদি(peer)
    cf_ssl_scache_clear_peer(peer);
  ফেরত peer;
}

স্থির CURLcode cf_ssl_add_peer(কাঠামো Curl_easy *তথ্য,
                                কাঠামো Curl_ssl_scache *scache,
                                ধ্রুবক অক্ষর *ssl_peer_চাবি,
                                কাঠামো ssl_filter_config *conn_config,
                                কাঠামো Curl_ssl_scache_peer **ppeer)
{
  কাঠামো Curl_ssl_scache_peer *peer = ফাঁকা;
  CURLcode ফলাফল = CURLE_OK;

  *ppeer = ফাঁকা;
  যদি(ssl_peer_চাবি) {
    ফলাফল = cf_ssl_find_peer_by_চাবি(তথ্য, scache, ssl_peer_চাবি, conn_config,
                                     &peer);
    যদি(ফলাফল || !scache->peer_গুনতি)
      ফেরত ফলাফল;
  }

  যদি(peer) {
    *ppeer = peer;
    ফেরত CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  যদি(peer) {
    অক্ষর বাফার[64];
    ধ্রুবক অক্ষর *ccert = conn_config ? conn_config->clientcert : ফাঁকা;
    ধ্রুবক অক্ষর *kpasswd = conn_config ? conn_config->চাবি_passwd : ফাঁকা;
    যদি(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(বাফার, আকার(বাফার),
                     "cert-%p", conn_config->cert_blob->তথ্য);
      ccert = বাফার; /* data is strduped by cf_ssl_scache_peer_init */
    }
    ফলাফল = cf_ssl_scache_peer_init(peer, ssl_peer_চাবি, ccert, kpasswd,
                                     ফাঁকা, ফাঁকা);
    যদি(ফলাফল)
      যাও out;
    /* all ready */
    *ppeer = peer;
    ফলাফল = CURLE_OK;
  }

out:
  যদি(ফলাফল) {
    cf_ssl_scache_clear_peer(peer);
  }
  ফেরত ফলাফল;
}

স্থির শূন্য cf_scache_peer_add_session(কাঠামো Curl_ssl_scache_peer *peer,
                                       কাঠামো Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  যদি(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, ফাঁকা);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  অন্যথা {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    যখন(Curl_llist_গুনতি(&peer->sessions) > peer->max_sessions) {
      Curl_গ্রন্থি_remove(Curl_llist_শীর্ষ(&peer->sessions));
    }
  }
}

স্থির CURLcode cf_scache_add_session(কাঠামো Curl_cfilter *cf,
                                      কাঠামো Curl_easy *তথ্য,
                                      কাঠামো Curl_ssl_scache *scache,
                                      ধ্রুবক অক্ষর *ssl_peer_চাবি,
                                      কাঠামো Curl_ssl_session *s)
{
  কাঠামো Curl_ssl_scache_peer *peer = ফাঁকা;
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ফলাফল = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(ফাঁকা);
  curl_off_t max_lifetime;

  যদি(!scache || !scache->peer_গুনতি) {
    Curl_ssl_session_destroy(s);
    ফেরত CURLE_OK;
  }

  যদি(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  যদি(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  যদি(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(তথ্য, "add, session already expired");
    Curl_ssl_session_destroy(s);
    ফেরত CURLE_OK;
  }

  ফলাফল = cf_ssl_add_peer(তথ্য, scache, ssl_peer_চাবি, conn_config, &peer);
  যদি(ফলাফল || !peer) {
    CURL_TRC_SSLS(তথ্য, "unable to add scache peer: %d", (পূর্ণসংখ্যা)ফলাফল);
    Curl_ssl_session_destroy(s);
    যাও out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  যদি(ফলাফল) {
    failf(তথ্য, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_চাবি, (পূর্ণসংখ্যা)ফলাফল);
  }
  অন্যথা
    CURL_TRC_SSLS(তথ্য, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_চাবি,
                  (unsigned পূর্ণসংখ্যা)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_গুনতি(&peer->sessions) : 0);
  ফেরত ফলাফল;
}

CURLcode Curl_ssl_scache_put(কাঠামো Curl_cfilter *cf,
                             কাঠামো Curl_easy *তথ্য,
                             ধ্রুবক অক্ষর *ssl_peer_চাবি,
                             কাঠামো Curl_ssl_session *s)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ফলাফল;
  DEBUGASSERT(conn_config);

  যদি(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    ফেরত CURLE_OK;
  }

  Curl_ssl_scache_lock(তথ্য);
  ফলাফল = cf_scache_add_session(cf, তথ্য, scache, ssl_peer_চাবি, s);
  Curl_ssl_scache_unlock(তথ্য);
  ফেরত ফলাফল;
}

শূন্য Curl_ssl_scache_return(কাঠামো Curl_cfilter *cf,
                            কাঠামো Curl_easy *তথ্য,
                            ধ্রুবক অক্ষর *ssl_peer_চাবি,
                            কাঠামো Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  যদি(s && s->ietf_tls_id < 0x304)
    (শূন্য)Curl_ssl_scache_put(cf, তথ্য, ssl_peer_চাবি, s);
  অন্যথা
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *ssl_peer_চাবি,
                              কাঠামো Curl_ssl_session **ps)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো Curl_ssl_scache_peer *peer = ফাঁকা;
  কাঠামো Curl_llist_গ্রন্থি *n;
  কাঠামো Curl_ssl_session *s = ফাঁকা;
  CURLcode ফলাফল;

  *ps = ফাঁকা;
  যদি(!scache)
    ফেরত CURLE_OK;

  Curl_ssl_scache_lock(তথ্য);
  ফলাফল = cf_ssl_find_peer_by_চাবি(তথ্য, scache, ssl_peer_চাবি, conn_config,
                                   &peer);
  যদি(!ফলাফল && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(ফাঁকা));
    n = Curl_llist_শীর্ষ(&peer->sessions);
    যদি(n) {
      s = Curl_গ্রন্থি_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  যদি(s) {
    *ps = s;
    CURL_TRC_SSLS(তথ্য, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_চাবি, (unsigned পূর্ণসংখ্যা)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_গুনতি(&peer->sessions));
  }
  অন্যথা {
    CURL_TRC_SSLS(তথ্য, "no cached session for %s", ssl_peer_চাবি);
  }
  Curl_ssl_scache_unlock(তথ্য);
  ফেরত ফলাফল;
}

CURLcode Curl_ssl_scache_add_obj(কাঠামো Curl_cfilter *cf,
                                 কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক অক্ষর *ssl_peer_চাবি,
                                 শূন্য *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো Curl_ssl_scache_peer *peer = ফাঁকা;
  CURLcode ফলাফল;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  যদি(!scache) {
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }

  ফলাফল = cf_ssl_add_peer(তথ্য, scache, ssl_peer_চাবি, conn_config, &peer);
  যদি(ফলাফল || !peer) {
    CURL_TRC_SSLS(তথ্য, "unable to add scache peer: %d", (পূর্ণসংখ্যা)ফলাফল);
    যাও out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = ফাঁকা;  /* peer took ownership */

out:
  যদি(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  ফেরত ফলাফল;
}

শূন্য *Curl_ssl_scache_get_obj(কাঠামো Curl_cfilter *cf,
                              কাঠামো Curl_easy *তথ্য,
                              ধ্রুবক অক্ষর *ssl_peer_চাবি)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো Curl_ssl_scache_peer *peer = ফাঁকা;
  CURLcode ফলাফল;
  শূন্য *sobj;

  যদি(!scache)
    ফেরত ফাঁকা;

  ফলাফল = cf_ssl_find_peer_by_চাবি(তথ্য, scache, ssl_peer_চাবি, conn_config,
                                   &peer);
  যদি(ফলাফল)
    ফেরত ফাঁকা;

  sobj = peer ? peer->sobj : ফাঁকা;

  CURL_TRC_SSLS(তথ্য, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_চাবি);
  ফেরত sobj;
}

শূন্য Curl_ssl_scache_remove_all(কাঠামো Curl_cfilter *cf,
                                কাঠামো Curl_easy *তথ্য,
                                ধ্রুবক অক্ষর *ssl_peer_চাবি)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
  কাঠামো ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  কাঠামো Curl_ssl_scache_peer *peer = ফাঁকা;
  CURLcode ফলাফল;

  (শূন্য)cf;
  যদি(!scache)
    ফেরত;

  Curl_ssl_scache_lock(তথ্য);
  ফলাফল = cf_ssl_find_peer_by_চাবি(তথ্য, scache, ssl_peer_চাবি, conn_config,
                                   &peer);
  যদি(!ফলাফল && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(তথ্য);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

স্থির CURLcode cf_ssl_scache_peer_set_hmac(কাঠামো Curl_ssl_scache_peer *peer)
{
  CURLcode ফলাফল;

  DEBUGASSERT(peer);
  যদি(!peer->ssl_peer_চাবি)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  ফলাফল = Curl_rand(ফাঁকা, peer->চাবি_salt, আকার(peer->চাবি_salt));
  যদি(ফলাফল)
    ফেরত ফলাফল;

  ফলাফল = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->চাবি_salt, আকার(peer->চাবি_salt),
                       (ধ্রুবক unsigned অক্ষর *)peer->ssl_peer_চাবি,
                       strlen(peer->ssl_peer_চাবি),
                       peer->চাবি_hmac);
  যদি(!ফলাফল)
    peer->hmac_set = TRUE;
  ফেরত ফলাফল;
}

স্থির CURLcode cf_ssl_find_peer_by_hmac(কাঠামো Curl_ssl_scache *scache,
                                         ধ্রুবক unsigned অক্ষর *salt,
                                         ধ্রুবক unsigned অক্ষর *hmac,
                                         কাঠামো Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode ফলাফল = CURLE_OK;

  *ppeer = ফাঁকা;
  যদি(!GOOD_SCACHE(scache))
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  জন্য(i = 0; scache && i < scache->peer_গুনতি; i++) {
    কাঠামো Curl_ssl_scache_peer *peer = &scache->peers[i];
    যদি(!cf_ssl_scache_match_auth(peer, ফাঁকা))
      চালু;
    যদি(scache->peers[i].hmac_set &&
       !memcmp(peer->চাবি_salt, salt, আকার(peer->চাবি_salt)) &&
       !memcmp(peer->চাবি_hmac, hmac, আকার(peer->চাবি_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      যাও out;
    }
    অন্যথা যদি(peer->ssl_peer_চাবি) {
      unsigned অক্ষর my_hmac[CURL_SHA256_DIGEST_দৈর্ঘ্য];
      /* compute hmac for the passed salt */
      ফলাফল = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, আকার(peer->চাবি_salt),
                           (ধ্রুবক unsigned অক্ষর *)peer->ssl_peer_চাবি,
                           strlen(peer->ssl_peer_চাবি),
                           my_hmac);
      যদি(ফলাফল)
        যাও out;
      যদি(!memcmp(my_hmac, hmac, আকার(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        যদি(!peer->hmac_set) {
          memcpy(peer->চাবি_salt, salt, আকার(peer->চাবি_salt));
          memcpy(peer->চাবি_hmac, hmac, আকার(peer->চাবি_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        যাও out;
      }
    }
  }
out:
  ফেরত ফলাফল;
}

CURLcode Curl_ssl_session_import(কাঠামো Curl_easy *তথ্য,
                                 ধ্রুবক অক্ষর *ssl_peer_চাবি,
                                 ধ্রুবক unsigned অক্ষর *shmac, size_t shmac_len,
                                 ধ্রুবক শূন্য *sdata, size_t sdata_len)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
  কাঠামো Curl_ssl_scache_peer *peer = ফাঁকা;
  কাঠামো Curl_ssl_session *s = ফাঁকা;
  bool have_lock = FALSE;
  CURLcode ফলাফল;

  যদি(!scache || !sdata || !sdata_len) {
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }
  যদি(!ssl_peer_চাবি && (!shmac || !shmac_len)) {
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }

  ফলাফল = Curl_ssl_session_unpack(তথ্য, sdata, sdata_len, &s);
  যদি(ফলাফল)
    যাও out;

  Curl_ssl_scache_lock(তথ্য);
  have_lock = TRUE;

  যদি(ssl_peer_চাবি) {
    ফলাফল = cf_ssl_add_peer(তথ্য, scache, ssl_peer_চাবি, ফাঁকা, &peer);
    যদি(ফলাফল)
      যাও out;
  }
  অন্যথা যদি(shmac_len != (আকার(peer->চাবি_salt) + আকার(peer->চাবি_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    ফলাফল = CURLE_BAD_FUNCTION_ARGUMENT;
    যাও out;
  }
  অন্যথা {
    ধ্রুবক unsigned অক্ষর *salt = shmac;
    ধ্রুবক unsigned অক্ষর *hmac = shmac + আকার(peer->চাবি_salt);

    ফলাফল = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    যদি(ফলাফল)
      যাও out;
    যদি(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      যদি(peer) {
        ফলাফল = cf_ssl_scache_peer_init(peer, ssl_peer_চাবি, ফাঁকা, ফাঁকা,
                                         salt, hmac);
        যদি(ফলাফল)
          যাও out;
      }
    }
  }

  যদি(peer) {
    cf_scache_peer_add_session(peer, s, time(ফাঁকা));
    s = ফাঁকা; /* peer is now owner */
    CURL_TRC_SSLS(তথ্য, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_চাবি ? peer->ssl_peer_চাবি : "without key",
                  Curl_llist_গুনতি(&peer->sessions));
  }

out:
  যদি(scache && have_lock)
    Curl_ssl_scache_unlock(তথ্য);
  Curl_ssl_session_destroy(s);
  ফেরত ফলাফল;
}

CURLcode Curl_ssl_session_export(কাঠামো Curl_easy *তথ্য,
                                 curl_ssls_export_cb *export_fn,
                                 শূন্য *userptr)
{
  কাঠামো Curl_ssl_scache *scache = cf_ssl_scache_get(তথ্য);
  কাঠামো Curl_ssl_scache_peer *peer;
  কাঠামো dynbuf sbuf, hbuf;
  কাঠামো Curl_llist_গ্রন্থি *n;
  size_t i;
  curl_off_t now = time(ফাঁকা);
  CURLcode ফলাফল = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  যদি(!export_fn)
    ফেরত CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(তথ্য);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_দৈর্ঘ্য * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  জন্য(i = 0; scache && i < scache->peer_গুনতি; i++) {
    peer = &scache->peers[i];
    যদি(!peer->ssl_peer_চাবি && !peer->hmac_set)
      চালু; /* skip free entry */
    যদি(!peer->exportable)
      চালু;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_শীর্ষ(&peer->sessions);
    যদি(n)
      VERBOSE(++npeers);
    যখন(n) {
      কাঠামো Curl_ssl_session *s = Curl_গ্রন্থি_elem(n);
      যদি(!peer->hmac_set) {
        ফলাফল = cf_ssl_scache_peer_set_hmac(peer);
        যদি(ফলাফল)
          যাও out;
      }
      যদি(!curlx_dyn_len(&hbuf)) {
        ফলাফল = curlx_dyn_addn(&hbuf, peer->চাবি_salt, আকার(peer->চাবি_salt));
        যদি(ফলাফল)
          যাও out;
        ফলাফল = curlx_dyn_addn(&hbuf, peer->চাবি_hmac, আকার(peer->চাবি_hmac));
        যদি(ফলাফল)
          যাও out;
      }
      curlx_dyn_reset(&sbuf);
      ফলাফল = Curl_ssl_session_pack(তথ্য, s, &sbuf);
      যদি(ফলাফল)
        যাও out;

      ফলাফল = export_fn(তথ্য, userptr, peer->ssl_peer_চাবি,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      যদি(ফলাফল)
        যাও out;
      VERBOSE(++ntickets);
      n = Curl_গ্রন্থি_পরবর্তী(n);
    }
  }
  ফলাফল = CURLE_OK;
  CURL_TRC_SSLS(তথ্য, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(তথ্য);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  ফেরত ফলাফল;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
