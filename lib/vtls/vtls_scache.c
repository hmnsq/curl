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

#اگر_تعریف_شدہ USE_SSL

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
ساخت Curl_ssl_scache_peer {
  حرف *ssl_peer_کلید;      /* id for peer + relevant TLS configuration */
  حرف *clientcert;
  حرف *کلید_passwd;
  ساخت Curl_llist sessions;
  خالی *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  غیرعلامتی حرف کلید_salt[CURL_SHA256_DIGEST_لمبائی]; /* for entry export */
  غیرعلامتی حرف کلید_hmac[CURL_SHA256_DIGEST_لمبائی]; /* for entry export */
  size_t max_sessions;
  طویل age;                /* a number, the higher the more recent */
  ثنائی_ہندسہ(hmac_سیٹ);           /* if key_salt and key_hmac are present */
  ثنائی_ہندسہ(exportable);         /* sessions for this peer can be exported */
};

#تعریف_کرو CURL_SCACHE_MAGIC 0x000e1551

#تعریف_کرو GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ساکن CURLcode cf_ssl_peer_کلید_جوڑو_string(ساخت dynbuf *buf,
                                           ثابت حرف *name,
                                           ثابت حرف *قدر)
{
  واپس curlx_dyn_addf(buf, ":%s-%zu:%s", name, لڑی_لمبائی(قدر), قدر);
}

ساکن CURLcode cf_ssl_peer_کلید_جوڑو_راستہ(ساخت dynbuf *buf,
                                         ثابت حرف *name,
                                         ثابت حرف *راستہ,
                                         bool *is_local)
{
  اگر(راستہ && راستہ[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#اگر_تعریف_شدہ _WIN32
    حرف *abspath = _fullpath(تہی, راستہ, 0);
    اگر(abspath) {
      CURLcode نتیجہ = cf_ssl_peer_کلید_جوڑو_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      آزاد(abspath); /* allocated by CRT, use system free() */
      واپس نتیجہ;
    }
    *is_local = TRUE;
#وگرنہ_اگر defined(HAVE_REALPATH)
    اگر(راستہ[0] != '/') {
      حرف *abspath = realpath(راستہ, تہی);
      اگر(abspath) {
        CURLcode نتیجہ = cf_ssl_peer_کلید_جوڑو_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        آزاد(abspath); /* allocated by libc, free without memdebug */
        واپس نتیجہ;
      }
      *is_local = TRUE;
    }
#اگر_ختم
    واپس cf_ssl_peer_کلید_جوڑو_string(buf, name, راستہ);
  }
  واپس CURLE_OK;
}

ساکن CURLcode cf_ssl_peer_کلید_جوڑو_hash(ساخت dynbuf *buf,
                                         ثابت حرف *name,
                                         ساخت curl_blob *blob)
{
  CURLcode نتیجہ = CURLE_OK;
  اگر(blob && blob->len) {
    غیرعلامتی حرف hash[CURL_SHA256_DIGEST_لمبائی];
    size_t i;

    نتیجہ = curlx_dyn_addf(buf, ":%s-", name);
    اگر(نتیجہ)
      جاؤ برآمد;
    نتیجہ = Curl_sha256it(hash, blob->ڈیٹا, blob->len);
    اگر(نتیجہ)
      جاؤ برآمد;
    برائے(i = 0; i < CURL_SHA256_DIGEST_لمبائی; ++i) {
      نتیجہ = curlx_dyn_addf(buf, "%02x", hash[i]);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
  }
برآمد:
  واپس نتیجہ;
}

ساکن CURLcode cf_ssl_peer_کلید_جوڑو_mtls(ساخت dynbuf *buf,
                                         ساخت ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode نتیجہ = CURLE_OK;
  اگر(ssl->clientcert && ssl->clientcert[0]) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_راستہ(buf, "CCERT", ssl->clientcert, is_local);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->کلید && ssl->کلید[0]) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_راستہ(buf, "KEY", ssl->کلید, is_local);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->کلید_blob) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_hash(buf, "KEYBlob", ssl->کلید_blob);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    نتیجہ = curlx_dyn_addf(buf, ":CT-%zu:", لڑی_لمبائی(ssl->cert_type));
    برائے(i = 0; !نتیجہ && ssl->cert_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->cert_type[i]);
      نتیجہ = curlx_dyn_addn(buf, &c, 1);
    }
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->کلید_type && ssl->کلید_type[0]) {
    size_t i;
    نتیجہ = curlx_dyn_addf(buf, ":KT-%zu:", لڑی_لمبائی(ssl->کلید_type));
    برائے(i = 0; !نتیجہ && ssl->کلید_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->کلید_type[i]);
      نتیجہ = curlx_dyn_addn(buf, &c, 1);
    }
  }
برآمد:
  واپس نتیجہ;
}

#تعریف_کرو CURL_SSLS_LOCAL_SUFFIX     ":L"
#تعریف_کرو CURL_SSLS_GLOBAL_SUFFIX    ":G"

ساکن bool cf_ssl_peer_کلید_is_global(ثابت حرف *peer_کلید)
{
  size_t len = peer_کلید ? لڑی_لمبائی(peer_کلید) : 0;
  واپس (len > 2) &&
         (peer_کلید[len - 1] == 'G') &&
         (peer_کلید[len - 2] == ':');
}

ساکن CURLcode ssl_peer_کلید_جوڑو_transport(ساخت dynbuf *buf,
                                           uint8_t transport)
{
  انتخاب(transport) {
  حالت TRNSPRT_TCP:
    واپس CURLE_OK;
  حالت TRNSPRT_UDP:
    واپس curlx_dyn_جوڑو(buf, ":UDP");
  حالت TRNSPRT_QUIC:
    واپس curlx_dyn_جوڑو(buf, ":QUIC");
  حالت TRNSPRT_UNIX:
    واپس curlx_dyn_جوڑو(buf, ":UNIX");
  طےشدہ:
    واپس curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ساکن CURLcode ssl_peer_کلید_جوڑو_vrfy(ساخت dynbuf *buf,
                                      ساخت ssl_filter_config *ssl,
                                      ثابت ساخت ssl_peer *peer)
{
  CURLcode نتیجہ;

  اگر(!ssl->verifypeer) {
    نتیجہ = curlx_dyn_جوڑو(buf, ":NO-VRFY-PEER");
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  اگر(!ssl->verifyhost) {
    نتیجہ = curlx_dyn_جوڑو(buf, ":NO-VRFY-HOST");
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  اگر(ssl->verifystatus) {
    نتیجہ = curlx_dyn_جوڑو(buf, ":VRFY-STATUS");
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  اگر((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(buf, "CHOST", peer->peer->hostname);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->پورٹ);
    اگر(نتیجہ)
      واپس نتیجہ;
  }
  واپس CURLE_OK;
}

ساکن CURLcode ssl_peer_کلید_build(ساخت ssl_filter_config *ssl,
                                   ثابت ساخت ssl_peer *peer,
                                   ثابت حرف *tls_id,
                                   حرف **ppeer_کلید)
{
  ساخت dynbuf buf;
  size_t کلید_len;
  bool is_local = FALSE;
  CURLcode نتیجہ;

  *ppeer_کلید = تہی;
  curlx_dyn_init(&buf, 10 * 1024);

  نتیجہ = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->پورٹ);
  اگر(نتیجہ)
    جاؤ برآمد;
  نتیجہ = ssl_peer_کلید_جوڑو_transport(&buf, peer->transport);
  اگر(نتیجہ)
    جاؤ برآمد;
  نتیجہ = ssl_peer_کلید_جوڑو_vrfy(&buf, ssl, peer);
  اگر(نتیجہ)
    جاؤ برآمد;
  اگر(ssl->نسخہ_2 || ssl->نسخہ_max) {
    نتیجہ = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->نسخہ_2,
                            (ssl->نسخہ_max >> 16));
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->ssl_options) {
    نتیجہ = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->cipher_list) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "CIPHER", ssl->cipher_list);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->cipher_list13) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "CIPHER13", ssl->cipher_list13);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->curves) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "CURVES", ssl->curves);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->signature_algorithms) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->verifypeer) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_راستہ(&buf, "CA", ssl->CAfile, &is_local);
    اگر(!نتیجہ)
      نتیجہ = cf_ssl_peer_کلید_جوڑو_راستہ(&buf, "CApath",
                                        ssl->CApath, &is_local);
    اگر(!نتیجہ && ssl->native_ca_store)
      نتیجہ = curlx_dyn_جوڑو(&buf, ":NATIVE-CA");
    اگر(!نتیجہ)
      نتیجہ = cf_ssl_peer_کلید_جوڑو_راستہ(&buf, "CRL", ssl->CRLfile, &is_local);
    اگر(نتیجہ)
      جاؤ برآمد;
    نتیجہ = cf_ssl_peer_کلید_جوڑو_راستہ(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    اگر(نتیجہ)
      جاؤ برآمد;
    اگر(ssl->ca_info_blob) {
      نتیجہ = cf_ssl_peer_کلید_جوڑو_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
    اگر(ssl->issuercert_blob) {
      نتیجہ = cf_ssl_peer_کلید_جوڑو_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      اگر(نتیجہ)
        جاؤ برآمد;
    }
  }
  اگر(ssl->cert_blob) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_hash(&buf, "CertBlob", ssl->cert_blob);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  اگر(ssl->pinned_کلید && ssl->pinned_کلید[0]) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "Pinned", ssl->pinned_کلید);
    اگر(نتیجہ)
      جاؤ برآمد;
  }

  نتیجہ = cf_ssl_peer_کلید_جوڑو_mtls(&buf, ssl, &is_local);
  اگر(نتیجہ)
    جاؤ برآمد;

  اگر(!tls_id || !tls_id[0]) {
    نتیجہ = CURLE_FAILED_INIT;
    جاؤ برآمد;
  }
  نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "IMPL", tls_id);
  اگر(نتیجہ)
    جاؤ برآمد;

  نتیجہ = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  اگر(نتیجہ)
    جاؤ برآمد;

  *ppeer_کلید = curlx_dyn_take(&buf, &کلید_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

برآمد:
  curlx_dyn_free(&buf);
  واپس نتیجہ;
}

CURLcode Curl_ssl_peer_کلید_make(ثابت ساخت ssl_peer *peer,
                                ساخت ssl_filter_config *sslc,
                                ثابت حرف *tls_id,
                                حرف **ppeer_کلید)
{
  واپس ssl_peer_کلید_build(sslc, peer, tls_id, ppeer_کلید);
}

ساخت Curl_ssl_scache {
  غیرعلامتی صحیح magic;
  ساخت Curl_ssl_scache_peer *peers;
  size_t peer_گنتی;
  صحیح default_lifetime_secs;
  طویل age;
#اگر_تعریف_شدہ USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#اگر_ختم
  ثنائی_ہندسہ(is_locked);
};

ساکن ساخت Curl_ssl_scache *cf_ssl_scache_get(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_ssl_scache *scache = تہی;
  /* If a share is present, its ssl_scache has preference over the multi */
  اگر(ڈیٹا->share && ڈیٹا->share->ssl_scache)
    scache = ڈیٹا->share->ssl_scache;
  ورنہ اگر(ڈیٹا->multi && ڈیٹا->multi->ssl_scache)
    scache = ڈیٹا->multi->ssl_scache;
  اگر(scache && !GOOD_SCACHE(scache)) {
    failf(ڈیٹا, "transfer would use an invalid scache at %p, denied",
          (خالی *)scache);
    DEBUGASSERT(0);
    واپس تہی;
  }
  واپس scache;
}

ساکن خالی cf_ssl_scache_session_ldestroy(خالی *udata, خالی *obj)
{
  ساخت Curl_ssl_session *s = obj;
  (خالی)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(خالی *sdata, size_t sdata_len,
                                 صحیح ietf_tls_id, ثابت حرف *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 ساخت Curl_ssl_session **psession)
{
  واپس Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  تہی, 0, psession);
}

CURLcode Curl_ssl_session_create2(خالی *sdata, size_t sdata_len,
                                  صحیح ietf_tls_id, ثابت حرف *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  غیرعلامتی حرف *quic_tp, size_t quic_tp_len,
                                  ساخت Curl_ssl_session **psession)
{
  ساخت Curl_ssl_session *s;

  اگر(!sdata || !sdata_len) {
    curlx_free(sdata);
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  *psession = تہی;
  s = curlx_calloc(1, حجم(*s));
  اگر(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    واپس CURLE_برآمد_کا_حافظہ;
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
      cf_ssl_scache_session_ldestroy(تہی, s);
      واپس CURLE_برآمد_کا_حافظہ;
    }
  }
  *psession = s;
  واپس CURLE_OK;
}

خالی Curl_ssl_session_destroy(ساخت Curl_ssl_session *s)
{
  اگر(s) {
    /* if in the list, the list destructor takes care of it */
    اگر(Curl_گرہ_llist(&s->list))
      Curl_گرہ_remove(&s->list);
    ورنہ {
      cf_ssl_scache_session_ldestroy(تہی, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(ساخت Curl_ssl_session *src,
                              ساخت Curl_ssl_session **pdest)
{
  ساخت Curl_ssl_session *dest = تہی;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  اگر(!src || !pdest)
    واپس CURLE_BAD_FUNCTION_دلیل;
  *pdest = تہی;

  dest = curlx_calloc(1, حجم(*dest));
  اگر(!dest)
    جاؤ برآمد;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  اگر(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    اگر(!dest->sdata)
      جاؤ برآمد;
    dest->sdata_len = src->sdata_len;
  }
  اگر(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    اگر(!dest->quic_tp)
      جاؤ برآمد;
    dest->quic_tp_len = src->quic_tp_len;
  }
  اگر(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    اگر(!dest->alpn)
      جاؤ برآمد;
  }
  نتیجہ = CURLE_OK;

برآمد:
  اگر(!نتیجہ)
    *pdest = dest;
  ورنہ {
    *pdest = تہی;
    اگر(dest)
      Curl_ssl_session_destroy(dest);
  }
  واپس نتیجہ;
}

ساکن خالی cf_ssl_scache_clear_peer(ساخت Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, تہی);
  اگر(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    اگر(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = تہی;
  }
  peer->sobj_free = تہی;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->کلید_passwd);
  curlx_safefree(peer->ssl_peer_کلید);
  peer->age = 0;
  peer->hmac_سیٹ = FALSE;
}

ساکن خالی cf_ssl_scache_peer_سیٹ_obj(ساخت Curl_ssl_scache_peer *peer,
                                       خالی *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  اگر(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

ساکن خالی cf_ssl_cache_peer_update(ساخت Curl_ssl_scache_peer *peer)
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

ساکن CURLcode cf_ssl_scache_peer_init(ساخت Curl_ssl_scache_peer *peer,
                                        ثابت حرف *ssl_peer_کلید,
                                        ثابت حرف *clientcert,
                                        ثابت حرف *کلید_passwd,
                                        ثابت غیرعلامتی حرف *salt,
                                        ثابت غیرعلامتی حرف *hmac)
{
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  DEBUGASSERT(!peer->ssl_peer_کلید);
  اگر(ssl_peer_کلید) {
    peer->ssl_peer_کلید = curlx_strdup(ssl_peer_کلید);
    اگر(!peer->ssl_peer_کلید)
      جاؤ برآمد;
    peer->hmac_سیٹ = FALSE;
  }
  ورنہ اگر(salt && hmac) {
    حافظہ_نقل(peer->کلید_salt, salt, حجم(peer->کلید_salt));
    حافظہ_نقل(peer->کلید_hmac, hmac, حجم(peer->کلید_hmac));
    peer->hmac_سیٹ = TRUE;
  }
  ورنہ {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }
  اگر(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    اگر(!peer->clientcert)
      جاؤ برآمد;
  }
  اگر(کلید_passwd) {
    peer->کلید_passwd = curlx_strdup(کلید_passwd);
    اگر(!peer->کلید_passwd)
      جاؤ برآمد;
  }
  cf_ssl_cache_peer_update(peer);
  نتیجہ = CURLE_OK;
برآمد:
  اگر(نتیجہ)
    cf_ssl_scache_clear_peer(peer);
  واپس نتیجہ;
}

ساکن خالی cf_scache_session_remove(ساخت Curl_ssl_scache_peer *peer,
                                     ساخت Curl_ssl_session *s)
{
  (خالی)peer;
  DEBUGASSERT(Curl_گرہ_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

ساکن bool cf_scache_session_expired(ساخت Curl_ssl_session *s,
                                      curl_off_t now)
{
  واپس (s->valid_until > 0) && (s->valid_until < now);
}

ساکن خالی cf_scache_peer_remove_expired(ساخت Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  ساخت Curl_llist_گرہ *n = Curl_llist_سر(&peer->sessions);
  جب(n) {
    ساخت Curl_ssl_session *s = Curl_گرہ_elem(n);
    n = Curl_گرہ_اگلا(n);
    اگر(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ساکن خالی cf_scache_peer_remove_non13(ساخت Curl_ssl_scache_peer *peer)
{
  ساخت Curl_llist_گرہ *n = Curl_llist_سر(&peer->sessions);
  جب(n) {
    ساخت Curl_ssl_session *s = Curl_گرہ_elem(n);
    n = Curl_گرہ_اگلا(n);
    اگر(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_فی_peer,
                                ساخت Curl_ssl_scache **pscache)
{
  ساخت Curl_ssl_scache *scache;
  ساخت Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = تہی;
  peers = curlx_calloc(max_peers, حجم(*peers));
  اگر(!peers)
    واپس CURLE_برآمد_کا_حافظہ;

  scache = curlx_calloc(1, حجم(*scache));
  اگر(!scache) {
    curlx_free(peers);
    واپس CURLE_برآمد_کا_حافظہ;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_گنتی = max_peers;
  scache->peers = peers;
  scache->age = 1;
  برائے(i = 0; i < scache->peer_گنتی; ++i) {
    scache->peers[i].max_sessions = max_sessions_فی_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#اگر_تعریف_شدہ USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#اگر_ختم
  *pscache = scache;
  واپس CURLE_OK;
}

خالی Curl_ssl_scache_destroy(ساخت Curl_ssl_scache *scache)
{
  اگر(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    برائے(i = 0; i < scache->peer_گنتی; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#اگر_تعریف_شدہ USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#اگر_ختم
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(ساخت Curl_cfilter *cf, ساخت Curl_easy *ڈیٹا)
{
  اگر(cf_ssl_scache_get(ڈیٹا)) {
    ساخت ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    واپس conn_config ? conn_config->cache_session : FALSE;
  }
  واپس FALSE;
}

/* Lock shared SSL session data */
خالی Curl_ssl_scache_تالا(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  اگر(scache) {
    اگر(CURL_SHARE_ssl_scache(ڈیٹا))
      Curl_share_تالا(ڈیٹا, CURL_تالا_ڈیٹا_SSL_SESSION,
                      CURL_تالا_ACCESS_SINGLE);
#اگر_تعریف_شدہ USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#ورنہ
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#اگر_ختم
  }
}

/* Unlock shared SSL session data */
خالی Curl_ssl_scache_unlock(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  اگر(scache) {
#اگر_تعریف_شدہ USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#ورنہ
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#اگر_ختم
    اگر(CURL_SHARE_ssl_scache(ڈیٹا))
      Curl_share_unlock(ڈیٹا, CURL_تالا_ڈیٹا_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(ساخت Curl_easy *ڈیٹا)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  bool locked = FALSE;
  اگر(!scache)
    واپس FALSE;
#اگر_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#ورنہ
  locked = (bool)scache->is_locked;
#اگر_ختم
  واپس locked;
}

ساکن bool cf_ssl_scache_مطابقت_auth(ساخت Curl_ssl_scache_peer *peer,
                                     ساخت ssl_filter_config *conn_config)
{
  اگر(!conn_config) {
    اگر(peer->clientcert || peer->کلید_passwd)
      واپس FALSE;
    واپس TRUE;
  }
  ورنہ اگر(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    واپس FALSE;
  اگر(Curl_timestrcmp(peer->کلید_passwd, conn_config->کلید_passwd))
    واپس FALSE;
  واپس TRUE;
}

ساکن CURLcode cf_ssl_ڈھونڈو_peer_by_کلید(ساخت Curl_easy *ڈیٹا,
                                        ساخت Curl_ssl_scache *scache,
                                        ثابت حرف *ssl_peer_کلید,
                                        ساخت ssl_filter_config *conn_config,
                                        ساخت Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_کلید_len = 0;
  CURLcode نتیجہ = CURLE_OK;

  *ppeer = تہی;
  اگر(!GOOD_SCACHE(scache)) {
    واپس CURLE_BAD_FUNCTION_دلیل;
  }

  CURL_TRC_SSLS(ڈیٹا, "find peer slot for %s among %zu slots",
                ssl_peer_کلید, scache->peer_گنتی);

  /* check for entries with known peer_key */
  برائے(i = 0; scache && i < scache->peer_گنتی; i++) {
    اگر(scache->peers[i].ssl_peer_کلید &&
       !strcmp(ssl_peer_کلید, scache->peers[i].ssl_peer_کلید) &&
       cf_ssl_scache_مطابقت_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      جاؤ برآمد;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  برائے(i = 0; scache && i < scache->peer_گنتی; i++) {
    اگر(!scache->peers[i].ssl_peer_کلید &&
       scache->peers[i].hmac_سیٹ &&
       cf_ssl_scache_مطابقت_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      غیرعلامتی حرف my_hmac[CURL_SHA256_DIGEST_لمبائی];
      اگر(!peer_کلید_len) /* we are lazy */
        peer_کلید_len = لڑی_لمبائی(ssl_peer_کلید);
      نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].کلید_salt,
                           حجم(scache->peers[i].کلید_salt),
                           (ثابت غیرعلامتی حرف *)ssl_peer_کلید,
                           peer_کلید_len,
                           my_hmac);
      اگر(نتیجہ)
        جاؤ برآمد;
      اگر(!memcmp(scache->peers[i].کلید_hmac, my_hmac, حجم(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(ڈیٹا, "peer entry %zu key recovered: %s",
                      i, ssl_peer_کلید);
        scache->peers[i].ssl_peer_کلید = curlx_strdup(ssl_peer_کلید);
        اگر(!scache->peers[i].ssl_peer_کلید) {
          نتیجہ = CURLE_برآمد_کا_حافظہ;
          جاؤ برآمد;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        جاؤ برآمد;
      }
    }
  }
  CURL_TRC_SSLS(ڈیٹا, "peer not found for %s", ssl_peer_کلید);
برآمد:
  واپس نتیجہ;
}

ساکن ساخت Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  ساخت Curl_ssl_scache *scache)
{
  ساخت Curl_ssl_scache_peer *peer = تہی;
  size_t i;

  /* find empty or oldest peer */
  برائے(i = 0; i < scache->peer_گنتی; ++i) {
    /* free peer entry? */
    اگر(!scache->peers[i].ssl_peer_کلید && !scache->peers[i].hmac_سیٹ) {
      peer = &scache->peers[i];
      توڑو;
    }
    /* peer without sessions and obj */
    اگر(!scache->peers[i].sobj &&
       !Curl_llist_گنتی(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      توڑو;
    }
    /* remember "oldest" peer */
    اگر(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  اگر(peer)
    cf_ssl_scache_clear_peer(peer);
  واپس peer;
}

ساکن CURLcode cf_ssl_جوڑو_peer(ساخت Curl_easy *ڈیٹا,
                                ساخت Curl_ssl_scache *scache,
                                ثابت حرف *ssl_peer_کلید,
                                ساخت ssl_filter_config *conn_config,
                                ساخت Curl_ssl_scache_peer **ppeer)
{
  ساخت Curl_ssl_scache_peer *peer = تہی;
  CURLcode نتیجہ = CURLE_OK;

  *ppeer = تہی;
  اگر(ssl_peer_کلید) {
    نتیجہ = cf_ssl_ڈھونڈو_peer_by_کلید(ڈیٹا, scache, ssl_peer_کلید, conn_config,
                                     &peer);
    اگر(نتیجہ || !scache->peer_گنتی)
      واپس نتیجہ;
  }

  اگر(peer) {
    *ppeer = peer;
    واپس CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  اگر(peer) {
    حرف بفر[64];
    ثابت حرف *ccert = conn_config ? conn_config->clientcert : تہی;
    ثابت حرف *kpasswd = conn_config ? conn_config->کلید_passwd : تہی;
    اگر(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(بفر, حجم(بفر),
                     "cert-%p", conn_config->cert_blob->ڈیٹا);
      ccert = بفر; /* data is strduped by cf_ssl_scache_peer_init */
    }
    نتیجہ = cf_ssl_scache_peer_init(peer, ssl_peer_کلید, ccert, kpasswd,
                                     تہی, تہی);
    اگر(نتیجہ)
      جاؤ برآمد;
    /* all ready */
    *ppeer = peer;
    نتیجہ = CURLE_OK;
  }

برآمد:
  اگر(نتیجہ) {
    cf_ssl_scache_clear_peer(peer);
  }
  واپس نتیجہ;
}

ساکن خالی cf_scache_peer_جوڑو_session(ساخت Curl_ssl_scache_peer *peer,
                                       ساخت Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  اگر(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, تہی);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  ورنہ {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    جب(Curl_llist_گنتی(&peer->sessions) > peer->max_sessions) {
      Curl_گرہ_remove(Curl_llist_سر(&peer->sessions));
    }
  }
}

ساکن CURLcode cf_scache_جوڑو_session(ساخت Curl_cfilter *cf,
                                      ساخت Curl_easy *ڈیٹا,
                                      ساخت Curl_ssl_scache *scache,
                                      ثابت حرف *ssl_peer_کلید,
                                      ساخت Curl_ssl_session *s)
{
  ساخت Curl_ssl_scache_peer *peer = تہی;
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;
  curl_off_t now = (curl_off_t)وقت(تہی);
  curl_off_t max_lifetime;

  اگر(!scache || !scache->peer_گنتی) {
    Curl_ssl_session_destroy(s);
    واپس CURLE_OK;
  }

  اگر(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  اگر(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  اگر(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(ڈیٹا, "add, session already expired");
    Curl_ssl_session_destroy(s);
    واپس CURLE_OK;
  }

  نتیجہ = cf_ssl_جوڑو_peer(ڈیٹا, scache, ssl_peer_کلید, conn_config, &peer);
  اگر(نتیجہ || !peer) {
    CURL_TRC_SSLS(ڈیٹا, "unable to add scache peer: %d", (صحیح)نتیجہ);
    Curl_ssl_session_destroy(s);
    جاؤ برآمد;
  }

  cf_scache_peer_جوڑو_session(peer, s, now);

برآمد:
  اگر(نتیجہ) {
    failf(ڈیٹا, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_کلید, (صحیح)نتیجہ);
  }
  ورنہ
    CURL_TRC_SSLS(ڈیٹا, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_کلید,
                  (غیرعلامتی صحیح)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_گنتی(&peer->sessions) : 0);
  واپس نتیجہ;
}

CURLcode Curl_ssl_scache_put(ساخت Curl_cfilter *cf,
                             ساخت Curl_easy *ڈیٹا,
                             ثابت حرف *ssl_peer_کلید,
                             ساخت Curl_ssl_session *s)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ;
  DEBUGASSERT(conn_config);

  اگر(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    واپس CURLE_OK;
  }

  Curl_ssl_scache_تالا(ڈیٹا);
  نتیجہ = cf_scache_جوڑو_session(cf, ڈیٹا, scache, ssl_peer_کلید, s);
  Curl_ssl_scache_unlock(ڈیٹا);
  واپس نتیجہ;
}

خالی Curl_ssl_scache_return(ساخت Curl_cfilter *cf,
                            ساخت Curl_easy *ڈیٹا,
                            ثابت حرف *ssl_peer_کلید,
                            ساخت Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  اگر(s && s->ietf_tls_id < 0x304)
    (خالی)Curl_ssl_scache_put(cf, ڈیٹا, ssl_peer_کلید, s);
  ورنہ
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *ssl_peer_کلید,
                              ساخت Curl_ssl_session **ps)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت Curl_ssl_scache_peer *peer = تہی;
  ساخت Curl_llist_گرہ *n;
  ساخت Curl_ssl_session *s = تہی;
  CURLcode نتیجہ;

  *ps = تہی;
  اگر(!scache)
    واپس CURLE_OK;

  Curl_ssl_scache_تالا(ڈیٹا);
  نتیجہ = cf_ssl_ڈھونڈو_peer_by_کلید(ڈیٹا, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  اگر(!نتیجہ && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)وقت(تہی));
    n = Curl_llist_سر(&peer->sessions);
    اگر(n) {
      s = Curl_گرہ_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  اگر(s) {
    *ps = s;
    CURL_TRC_SSLS(ڈیٹا, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_کلید, (غیرعلامتی صحیح)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_گنتی(&peer->sessions));
  }
  ورنہ {
    CURL_TRC_SSLS(ڈیٹا, "no cached session for %s", ssl_peer_کلید);
  }
  Curl_ssl_scache_unlock(ڈیٹا);
  واپس نتیجہ;
}

CURLcode Curl_ssl_scache_جوڑو_obj(ساخت Curl_cfilter *cf,
                                 ساخت Curl_easy *ڈیٹا,
                                 ثابت حرف *ssl_peer_کلید,
                                 خالی *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت Curl_ssl_scache_peer *peer = تہی;
  CURLcode نتیجہ;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  اگر(!scache) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }

  نتیجہ = cf_ssl_جوڑو_peer(ڈیٹا, scache, ssl_peer_کلید, conn_config, &peer);
  اگر(نتیجہ || !peer) {
    CURL_TRC_SSLS(ڈیٹا, "unable to add scache peer: %d", (صحیح)نتیجہ);
    جاؤ برآمد;
  }

  cf_ssl_scache_peer_سیٹ_obj(peer, sobj, sobj_dtor_cb);
  sobj = تہی;  /* peer took ownership */

برآمد:
  اگر(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  واپس نتیجہ;
}

خالی *Curl_ssl_scache_get_obj(ساخت Curl_cfilter *cf,
                              ساخت Curl_easy *ڈیٹا,
                              ثابت حرف *ssl_peer_کلید)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت Curl_ssl_scache_peer *peer = تہی;
  CURLcode نتیجہ;
  خالی *sobj;

  اگر(!scache)
    واپس تہی;

  نتیجہ = cf_ssl_ڈھونڈو_peer_by_کلید(ڈیٹا, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  اگر(نتیجہ)
    واپس تہی;

  sobj = peer ? peer->sobj : تہی;

  CURL_TRC_SSLS(ڈیٹا, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_کلید);
  واپس sobj;
}

خالی Curl_ssl_scache_remove_all(ساخت Curl_cfilter *cf,
                                ساخت Curl_easy *ڈیٹا,
                                ثابت حرف *ssl_peer_کلید)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  ساخت ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ساخت Curl_ssl_scache_peer *peer = تہی;
  CURLcode نتیجہ;

  (خالی)cf;
  اگر(!scache)
    واپس;

  Curl_ssl_scache_تالا(ڈیٹا);
  نتیجہ = cf_ssl_ڈھونڈو_peer_by_کلید(ڈیٹا, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  اگر(!نتیجہ && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(ڈیٹا);
}

#اگر_تعریف_شدہ USE_SSLS_EXPORT

#تعریف_کرو CURL_SSL_TICKET_MAX   (16 * 1024)

ساکن CURLcode cf_ssl_scache_peer_سیٹ_hmac(ساخت Curl_ssl_scache_peer *peer)
{
  CURLcode نتیجہ;

  DEBUGASSERT(peer);
  اگر(!peer->ssl_peer_کلید)
    واپس CURLE_BAD_FUNCTION_دلیل;

  نتیجہ = Curl_rand(تہی, peer->کلید_salt, حجم(peer->کلید_salt));
  اگر(نتیجہ)
    واپس نتیجہ;

  نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->کلید_salt, حجم(peer->کلید_salt),
                       (ثابت غیرعلامتی حرف *)peer->ssl_peer_کلید,
                       لڑی_لمبائی(peer->ssl_peer_کلید),
                       peer->کلید_hmac);
  اگر(!نتیجہ)
    peer->hmac_سیٹ = TRUE;
  واپس نتیجہ;
}

ساکن CURLcode cf_ssl_ڈھونڈو_peer_by_hmac(ساخت Curl_ssl_scache *scache,
                                         ثابت غیرعلامتی حرف *salt,
                                         ثابت غیرعلامتی حرف *hmac,
                                         ساخت Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode نتیجہ = CURLE_OK;

  *ppeer = تہی;
  اگر(!GOOD_SCACHE(scache))
    واپس CURLE_BAD_FUNCTION_دلیل;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  برائے(i = 0; scache && i < scache->peer_گنتی; i++) {
    ساخت Curl_ssl_scache_peer *peer = &scache->peers[i];
    اگر(!cf_ssl_scache_مطابقت_auth(peer, تہی))
      جاری;
    اگر(scache->peers[i].hmac_سیٹ &&
       !memcmp(peer->کلید_salt, salt, حجم(peer->کلید_salt)) &&
       !memcmp(peer->کلید_hmac, hmac, حجم(peer->کلید_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      جاؤ برآمد;
    }
    ورنہ اگر(peer->ssl_peer_کلید) {
      غیرعلامتی حرف my_hmac[CURL_SHA256_DIGEST_لمبائی];
      /* compute hmac for the passed salt */
      نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, حجم(peer->کلید_salt),
                           (ثابت غیرعلامتی حرف *)peer->ssl_peer_کلید,
                           لڑی_لمبائی(peer->ssl_peer_کلید),
                           my_hmac);
      اگر(نتیجہ)
        جاؤ برآمد;
      اگر(!memcmp(my_hmac, hmac, حجم(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        اگر(!peer->hmac_سیٹ) {
          حافظہ_نقل(peer->کلید_salt, salt, حجم(peer->کلید_salt));
          حافظہ_نقل(peer->کلید_hmac, hmac, حجم(peer->کلید_hmac));
          peer->hmac_سیٹ = TRUE;
        }
        *ppeer = peer;
        جاؤ برآمد;
      }
    }
  }
برآمد:
  واپس نتیجہ;
}

CURLcode Curl_ssl_session_import(ساخت Curl_easy *ڈیٹا,
                                 ثابت حرف *ssl_peer_کلید,
                                 ثابت غیرعلامتی حرف *shmac, size_t shmac_len,
                                 ثابت خالی *sdata, size_t sdata_len)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  ساخت Curl_ssl_scache_peer *peer = تہی;
  ساخت Curl_ssl_session *s = تہی;
  bool have_تالا = FALSE;
  CURLcode نتیجہ;

  اگر(!scache || !sdata || !sdata_len) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }
  اگر(!ssl_peer_کلید && (!shmac || !shmac_len)) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }

  نتیجہ = Curl_ssl_session_unpack(ڈیٹا, sdata, sdata_len, &s);
  اگر(نتیجہ)
    جاؤ برآمد;

  Curl_ssl_scache_تالا(ڈیٹا);
  have_تالا = TRUE;

  اگر(ssl_peer_کلید) {
    نتیجہ = cf_ssl_جوڑو_peer(ڈیٹا, scache, ssl_peer_کلید, تہی, &peer);
    اگر(نتیجہ)
      جاؤ برآمد;
  }
  ورنہ اگر(shmac_len != (حجم(peer->کلید_salt) + حجم(peer->کلید_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    جاؤ برآمد;
  }
  ورنہ {
    ثابت غیرعلامتی حرف *salt = shmac;
    ثابت غیرعلامتی حرف *hmac = shmac + حجم(peer->کلید_salt);

    نتیجہ = cf_ssl_ڈھونڈو_peer_by_hmac(scache, salt, hmac, &peer);
    اگر(نتیجہ)
      جاؤ برآمد;
    اگر(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      اگر(peer) {
        نتیجہ = cf_ssl_scache_peer_init(peer, ssl_peer_کلید, تہی, تہی,
                                         salt, hmac);
        اگر(نتیجہ)
          جاؤ برآمد;
      }
    }
  }

  اگر(peer) {
    cf_scache_peer_جوڑو_session(peer, s, وقت(تہی));
    s = تہی; /* peer is now owner */
    CURL_TRC_SSLS(ڈیٹا, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_کلید ? peer->ssl_peer_کلید : "without key",
                  Curl_llist_گنتی(&peer->sessions));
  }

برآمد:
  اگر(scache && have_تالا)
    Curl_ssl_scache_unlock(ڈیٹا);
  Curl_ssl_session_destroy(s);
  واپس نتیجہ;
}

CURLcode Curl_ssl_session_export(ساخت Curl_easy *ڈیٹا,
                                 curl_ssls_export_cb *export_fn,
                                 خالی *userptr)
{
  ساخت Curl_ssl_scache *scache = cf_ssl_scache_get(ڈیٹا);
  ساخت Curl_ssl_scache_peer *peer;
  ساخت dynbuf sbuf, hbuf;
  ساخت Curl_llist_گرہ *n;
  size_t i;
  curl_off_t now = وقت(تہی);
  CURLcode نتیجہ = CURLE_OK;
#اگر_تعریف_شدہ CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#اگر_ختم

  اگر(!export_fn)
    واپس CURLE_BAD_FUNCTION_دلیل;

  Curl_ssl_scache_تالا(ڈیٹا);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_لمبائی * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  برائے(i = 0; scache && i < scache->peer_گنتی; i++) {
    peer = &scache->peers[i];
    اگر(!peer->ssl_peer_کلید && !peer->hmac_سیٹ)
      جاری; /* skip free entry */
    اگر(!peer->exportable)
      جاری;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_سر(&peer->sessions);
    اگر(n)
      VERBOSE(++npeers);
    جب(n) {
      ساخت Curl_ssl_session *s = Curl_گرہ_elem(n);
      اگر(!peer->hmac_سیٹ) {
        نتیجہ = cf_ssl_scache_peer_سیٹ_hmac(peer);
        اگر(نتیجہ)
          جاؤ برآمد;
      }
      اگر(!curlx_dyn_len(&hbuf)) {
        نتیجہ = curlx_dyn_addn(&hbuf, peer->کلید_salt, حجم(peer->کلید_salt));
        اگر(نتیجہ)
          جاؤ برآمد;
        نتیجہ = curlx_dyn_addn(&hbuf, peer->کلید_hmac, حجم(peer->کلید_hmac));
        اگر(نتیجہ)
          جاؤ برآمد;
      }
      curlx_dyn_reset(&sbuf);
      نتیجہ = Curl_ssl_session_pack(ڈیٹا, s, &sbuf);
      اگر(نتیجہ)
        جاؤ برآمد;

      نتیجہ = export_fn(ڈیٹا, userptr, peer->ssl_peer_کلید,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      اگر(نتیجہ)
        جاؤ برآمد;
      VERBOSE(++ntickets);
      n = Curl_گرہ_اگلا(n);
    }
  }
  نتیجہ = CURLE_OK;
  CURL_TRC_SSLS(ڈیٹا, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

برآمد:
  Curl_ssl_scache_unlock(ڈیٹا);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  واپس نتیجہ;
}

#اگر_ختم /* USE_SSLS_EXPORT */

#اگر_ختم /* USE_SSL */
