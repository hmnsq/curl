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

#اگر_تعریف_شده USE_SSL

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
  حرف *ssl_peer_کلید;      /* id for peer + relevant TLS configuration */
  حرف *clientcert;
  حرف *کلید_passwd;
  بنية Curl_llist sessions;
  فراغ *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  بیعلامت حرف کلید_salt[CURL_SHA256_DIGEST_درازی]; /* for entry export */
  بیعلامت حرف کلید_hmac[CURL_SHA256_DIGEST_درازی]; /* for entry export */
  size_t max_sessions;
  دراز age;                /* a number, the higher the more recent */
  بیت(hmac_مجموعه);           /* if key_salt and key_hmac are present */
  بیت(exportable);         /* sessions for this peer can be exported */
};

#تعریف_کن CURL_SCACHE_MAGIC 0x000e1551

#تعریف_کن GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ساكن CURLcode cf_ssl_peer_کلید_جمع_کن_string(بنية dynbuf *buf,
                                           ثابت حرف *name,
                                           ثابت حرف *مقدار)
{
  أرجع curlx_dyn_addf(buf, ":%s-%zu:%s", name, طول_رشته(مقدار), مقدار);
}

ساكن CURLcode cf_ssl_peer_کلید_جمع_کن_مسیر(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         ثابت حرف *مسیر,
                                         bool *is_local)
{
  إذا(مسیر && مسیر[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#اگر_تعریف_شده _WIN32
    حرف *abspath = _fullpath(عدم, مسیر, 0);
    إذا(abspath) {
      CURLcode نتیجه = cf_ssl_peer_کلید_جمع_کن_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      حرر(abspath); /* allocated by CRT, use system free() */
      أرجع نتیجه;
    }
    *is_local = TRUE;
#وإلاإذا defined(HAVE_REALPATH)
    إذا(مسیر[0] != '/') {
      حرف *abspath = realpath(مسیر, عدم);
      إذا(abspath) {
        CURLcode نتیجه = cf_ssl_peer_کلید_جمع_کن_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        حرر(abspath); /* allocated by libc, free without memdebug */
        أرجع نتیجه;
      }
      *is_local = TRUE;
    }
#پایان_اگر
    أرجع cf_ssl_peer_کلید_جمع_کن_string(buf, name, مسیر);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ssl_peer_کلید_جمع_کن_hash(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         بنية curl_blob *blob)
{
  CURLcode نتیجه = CURLE_OK;
  إذا(blob && blob->len) {
    بیعلامت حرف hash[CURL_SHA256_DIGEST_درازی];
    size_t i;

    نتیجه = curlx_dyn_addf(buf, ":%s-", name);
    إذا(نتیجه)
      اذهب بیرون;
    نتیجه = Curl_sha256it(hash, blob->داده, blob->len);
    إذا(نتیجه)
      اذهب بیرون;
    لكل(i = 0; i < CURL_SHA256_DIGEST_درازی; ++i) {
      نتیجه = curlx_dyn_addf(buf, "%02x", hash[i]);
      إذا(نتیجه)
        اذهب بیرون;
    }
  }
بیرون:
  أرجع نتیجه;
}

ساكن CURLcode cf_ssl_peer_کلید_جمع_کن_mtls(بنية dynbuf *buf,
                                         بنية ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode نتیجه = CURLE_OK;
  إذا(ssl->clientcert && ssl->clientcert[0]) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_مسیر(buf, "CCERT", ssl->clientcert, is_local);
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->کلید && ssl->کلید[0]) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_مسیر(buf, "KEY", ssl->کلید, is_local);
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->کلید_blob) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_hash(buf, "KEYBlob", ssl->کلید_blob);
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    نتیجه = curlx_dyn_addf(buf, ":CT-%zu:", طول_رشته(ssl->cert_type));
    لكل(i = 0; !نتیجه && ssl->cert_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->cert_type[i]);
      نتیجه = curlx_dyn_addn(buf, &c, 1);
    }
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->کلید_type && ssl->کلید_type[0]) {
    size_t i;
    نتیجه = curlx_dyn_addf(buf, ":KT-%zu:", طول_رشته(ssl->کلید_type));
    لكل(i = 0; !نتیجه && ssl->کلید_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->کلید_type[i]);
      نتیجه = curlx_dyn_addn(buf, &c, 1);
    }
  }
بیرون:
  أرجع نتیجه;
}

#تعریف_کن CURL_SSLS_LOCAL_SUFFIX     ":L"
#تعریف_کن CURL_SSLS_GLOBAL_SUFFIX    ":G"

ساكن bool cf_ssl_peer_کلید_is_global(ثابت حرف *peer_کلید)
{
  size_t len = peer_کلید ? طول_رشته(peer_کلید) : 0;
  أرجع (len > 2) &&
         (peer_کلید[len - 1] == 'G') &&
         (peer_کلید[len - 2] == ':');
}

ساكن CURLcode ssl_peer_کلید_جمع_کن_transport(بنية dynbuf *buf,
                                           uint8_t transport)
{
  بدل(transport) {
  حالة TRNSPRT_TCP:
    أرجع CURLE_OK;
  حالة TRNSPRT_UDP:
    أرجع curlx_dyn_جمع_کن(buf, ":UDP");
  حالة TRNSPRT_QUIC:
    أرجع curlx_dyn_جمع_کن(buf, ":QUIC");
  حالة TRNSPRT_UNIX:
    أرجع curlx_dyn_جمع_کن(buf, ":UNIX");
  افتراضي:
    أرجع curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ساكن CURLcode ssl_peer_کلید_جمع_کن_vrfy(بنية dynbuf *buf,
                                      بنية ssl_filter_config *ssl,
                                      ثابت بنية ssl_peer *peer)
{
  CURLcode نتیجه;

  إذا(!ssl->verifypeer) {
    نتیجه = curlx_dyn_جمع_کن(buf, ":NO-VRFY-PEER");
    إذا(نتیجه)
      أرجع نتیجه;
  }
  إذا(!ssl->verifyhost) {
    نتیجه = curlx_dyn_جمع_کن(buf, ":NO-VRFY-HOST");
    إذا(نتیجه)
      أرجع نتیجه;
  }
  إذا(ssl->verifystatus) {
    نتیجه = curlx_dyn_جمع_کن(buf, ":VRFY-STATUS");
    إذا(نتیجه)
      أرجع نتیجه;
  }
  إذا((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_string(buf, "CHOST", peer->peer->hostname);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->پورت);
    إذا(نتیجه)
      أرجع نتیجه;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssl_peer_کلید_build(بنية ssl_filter_config *ssl,
                                   ثابت بنية ssl_peer *peer,
                                   ثابت حرف *tls_id,
                                   حرف **ppeer_کلید)
{
  بنية dynbuf buf;
  size_t کلید_len;
  bool is_local = FALSE;
  CURLcode نتیجه;

  *ppeer_کلید = عدم;
  curlx_dyn_init(&buf, 10 * 1024);

  نتیجه = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->پورت);
  إذا(نتیجه)
    اذهب بیرون;
  نتیجه = ssl_peer_کلید_جمع_کن_transport(&buf, peer->transport);
  إذا(نتیجه)
    اذهب بیرون;
  نتیجه = ssl_peer_کلید_جمع_کن_vrfy(&buf, ssl, peer);
  إذا(نتیجه)
    اذهب بیرون;
  إذا(ssl->نسخه_2 || ssl->نسخه_max) {
    نتیجه = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->نسخه_2,
                            (ssl->نسخه_max >> 16));
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->ssl_options) {
    نتیجه = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->cipher_list) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_string(&buf, "CIPHER", ssl->cipher_list);
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->cipher_list13) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_string(&buf, "CIPHER13", ssl->cipher_list13);
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->curves) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_string(&buf, "CURVES", ssl->curves);
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->signature_algorithms) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->verifypeer) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_مسیر(&buf, "CA", ssl->CAfile, &is_local);
    إذا(!نتیجه)
      نتیجه = cf_ssl_peer_کلید_جمع_کن_مسیر(&buf, "CApath",
                                        ssl->CApath, &is_local);
    إذا(!نتیجه && ssl->native_ca_store)
      نتیجه = curlx_dyn_جمع_کن(&buf, ":NATIVE-CA");
    إذا(!نتیجه)
      نتیجه = cf_ssl_peer_کلید_جمع_کن_مسیر(&buf, "CRL", ssl->CRLfile, &is_local);
    إذا(نتیجه)
      اذهب بیرون;
    نتیجه = cf_ssl_peer_کلید_جمع_کن_مسیر(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    إذا(نتیجه)
      اذهب بیرون;
    إذا(ssl->ca_info_blob) {
      نتیجه = cf_ssl_peer_کلید_جمع_کن_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      إذا(نتیجه)
        اذهب بیرون;
    }
    إذا(ssl->issuercert_blob) {
      نتیجه = cf_ssl_peer_کلید_جمع_کن_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      إذا(نتیجه)
        اذهب بیرون;
    }
  }
  إذا(ssl->cert_blob) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_hash(&buf, "CertBlob", ssl->cert_blob);
    إذا(نتیجه)
      اذهب بیرون;
  }
  إذا(ssl->pinned_کلید && ssl->pinned_کلید[0]) {
    نتیجه = cf_ssl_peer_کلید_جمع_کن_string(&buf, "Pinned", ssl->pinned_کلید);
    إذا(نتیجه)
      اذهب بیرون;
  }

  نتیجه = cf_ssl_peer_کلید_جمع_کن_mtls(&buf, ssl, &is_local);
  إذا(نتیجه)
    اذهب بیرون;

  إذا(!tls_id || !tls_id[0]) {
    نتیجه = CURLE_FAILED_INIT;
    اذهب بیرون;
  }
  نتیجه = cf_ssl_peer_کلید_جمع_کن_string(&buf, "IMPL", tls_id);
  إذا(نتیجه)
    اذهب بیرون;

  نتیجه = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  إذا(نتیجه)
    اذهب بیرون;

  *ppeer_کلید = curlx_dyn_take(&buf, &کلید_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

بیرون:
  curlx_dyn_free(&buf);
  أرجع نتیجه;
}

CURLcode Curl_ssl_peer_کلید_make(ثابت بنية ssl_peer *peer,
                                بنية ssl_filter_config *sslc,
                                ثابت حرف *tls_id,
                                حرف **ppeer_کلید)
{
  أرجع ssl_peer_کلید_build(sslc, peer, tls_id, ppeer_کلید);
}

بنية Curl_ssl_scache {
  بیعلامت صحيح magic;
  بنية Curl_ssl_scache_peer *peers;
  size_t peer_شمار;
  صحيح default_lifetime_secs;
  دراز age;
#اگر_تعریف_شده USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#پایان_اگر
  بیت(is_locked);
};

ساكن بنية Curl_ssl_scache *cf_ssl_scache_get(بنية Curl_easy *داده)
{
  بنية Curl_ssl_scache *scache = عدم;
  /* If a share is present, its ssl_scache has preference over the multi */
  إذا(داده->share && داده->share->ssl_scache)
    scache = داده->share->ssl_scache;
  وإلا إذا(داده->multi && داده->multi->ssl_scache)
    scache = داده->multi->ssl_scache;
  إذا(scache && !GOOD_SCACHE(scache)) {
    failf(داده, "transfer would use an invalid scache at %p, denied",
          (فراغ *)scache);
    DEBUGASSERT(0);
    أرجع عدم;
  }
  أرجع scache;
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
  أرجع Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  عدم, 0, psession);
}

CURLcode Curl_ssl_session_create2(فراغ *sdata, size_t sdata_len,
                                  صحيح ietf_tls_id, ثابت حرف *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  بیعلامت حرف *quic_tp, size_t quic_tp_len,
                                  بنية Curl_ssl_session **psession)
{
  بنية Curl_ssl_session *s;

  إذا(!sdata || !sdata_len) {
    curlx_free(sdata);
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  *psession = عدم;
  s = curlx_calloc(1, حجم(*s));
  إذا(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    أرجع CURLE_بیرون_از_حافظه;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  إذا(alpn) {
    s->alpn = curlx_strdup(alpn);
    إذا(!s->alpn) {
      cf_ssl_scache_session_ldestroy(عدم, s);
      أرجع CURLE_بیرون_از_حافظه;
    }
  }
  *psession = s;
  أرجع CURLE_OK;
}

فراغ Curl_ssl_session_destroy(بنية Curl_ssl_session *s)
{
  إذا(s) {
    /* if in the list, the list destructor takes care of it */
    إذا(Curl_گره_llist(&s->list))
      Curl_گره_remove(&s->list);
    وإلا {
      cf_ssl_scache_session_ldestroy(عدم, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(بنية Curl_ssl_session *src,
                              بنية Curl_ssl_session **pdest)
{
  بنية Curl_ssl_session *dest = عدم;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  إذا(!src || !pdest)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  *pdest = عدم;

  dest = curlx_calloc(1, حجم(*dest));
  إذا(!dest)
    اذهب بیرون;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  إذا(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    إذا(!dest->sdata)
      اذهب بیرون;
    dest->sdata_len = src->sdata_len;
  }
  إذا(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    إذا(!dest->quic_tp)
      اذهب بیرون;
    dest->quic_tp_len = src->quic_tp_len;
  }
  إذا(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    إذا(!dest->alpn)
      اذهب بیرون;
  }
  نتیجه = CURLE_OK;

بیرون:
  إذا(!نتیجه)
    *pdest = dest;
  وإلا {
    *pdest = عدم;
    إذا(dest)
      Curl_ssl_session_destroy(dest);
  }
  أرجع نتیجه;
}

ساكن فراغ cf_ssl_scache_clear_peer(بنية Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, عدم);
  إذا(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    إذا(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = عدم;
  }
  peer->sobj_free = عدم;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->کلید_passwd);
  curlx_safefree(peer->ssl_peer_کلید);
  peer->age = 0;
  peer->hmac_مجموعه = FALSE;
}

ساكن فراغ cf_ssl_scache_peer_مجموعه_obj(بنية Curl_ssl_scache_peer *peer,
                                       فراغ *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  إذا(peer->sobj_free) {
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
  peer->exportable = (!peer->clientcert && !peer->کلید_passwd &&
                      (!peer->ssl_peer_کلید ||
                       cf_ssl_peer_کلید_is_global(peer->ssl_peer_کلید)));
}

ساكن CURLcode cf_ssl_scache_peer_init(بنية Curl_ssl_scache_peer *peer,
                                        ثابت حرف *ssl_peer_کلید,
                                        ثابت حرف *clientcert,
                                        ثابت حرف *کلید_passwd,
                                        ثابت بیعلامت حرف *salt,
                                        ثابت بیعلامت حرف *hmac)
{
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  DEBUGASSERT(!peer->ssl_peer_کلید);
  إذا(ssl_peer_کلید) {
    peer->ssl_peer_کلید = curlx_strdup(ssl_peer_کلید);
    إذا(!peer->ssl_peer_کلید)
      اذهب بیرون;
    peer->hmac_مجموعه = FALSE;
  }
  وإلا إذا(salt && hmac) {
    کپی_حافظه(peer->کلید_salt, salt, حجم(peer->کلید_salt));
    کپی_حافظه(peer->کلید_hmac, hmac, حجم(peer->کلید_hmac));
    peer->hmac_مجموعه = TRUE;
  }
  وإلا {
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب بیرون;
  }
  إذا(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    إذا(!peer->clientcert)
      اذهب بیرون;
  }
  إذا(کلید_passwd) {
    peer->کلید_passwd = curlx_strdup(کلید_passwd);
    إذا(!peer->کلید_passwd)
      اذهب بیرون;
  }
  cf_ssl_cache_peer_update(peer);
  نتیجه = CURLE_OK;
بیرون:
  إذا(نتیجه)
    cf_ssl_scache_clear_peer(peer);
  أرجع نتیجه;
}

ساكن فراغ cf_scache_session_remove(بنية Curl_ssl_scache_peer *peer,
                                     بنية Curl_ssl_session *s)
{
  (فراغ)peer;
  DEBUGASSERT(Curl_گره_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

ساكن bool cf_scache_session_expired(بنية Curl_ssl_session *s,
                                      curl_off_t now)
{
  أرجع (s->valid_until > 0) && (s->valid_until < now);
}

ساكن فراغ cf_scache_peer_remove_expired(بنية Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  بنية Curl_llist_گره *n = Curl_llist_سر(&peer->sessions);
  طالما(n) {
    بنية Curl_ssl_session *s = Curl_گره_elem(n);
    n = Curl_گره_بعدی(n);
    إذا(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ساكن فراغ cf_scache_peer_remove_non13(بنية Curl_ssl_scache_peer *peer)
{
  بنية Curl_llist_گره *n = Curl_llist_سر(&peer->sessions);
  طالما(n) {
    بنية Curl_ssl_session *s = Curl_گره_elem(n);
    n = Curl_گره_بعدی(n);
    إذا(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_در_هر_peer,
                                بنية Curl_ssl_scache **pscache)
{
  بنية Curl_ssl_scache *scache;
  بنية Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = عدم;
  peers = curlx_calloc(max_peers, حجم(*peers));
  إذا(!peers)
    أرجع CURLE_بیرون_از_حافظه;

  scache = curlx_calloc(1, حجم(*scache));
  إذا(!scache) {
    curlx_free(peers);
    أرجع CURLE_بیرون_از_حافظه;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_شمار = max_peers;
  scache->peers = peers;
  scache->age = 1;
  لكل(i = 0; i < scache->peer_شمار; ++i) {
    scache->peers[i].max_sessions = max_sessions_در_هر_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#اگر_تعریف_شده USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#پایان_اگر
  *pscache = scache;
  أرجع CURLE_OK;
}

فراغ Curl_ssl_scache_destroy(بنية Curl_ssl_scache *scache)
{
  إذا(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    لكل(i = 0; i < scache->peer_شمار; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#اگر_تعریف_شده USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#پایان_اگر
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(بنية Curl_cfilter *cf, بنية Curl_easy *داده)
{
  إذا(cf_ssl_scache_get(داده)) {
    بنية ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    أرجع conn_config ? conn_config->cache_session : FALSE;
  }
  أرجع FALSE;
}

/* Lock shared SSL session data */
فراغ Curl_ssl_scache_قفل(بنية Curl_easy *داده)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  إذا(scache) {
    إذا(CURL_SHARE_ssl_scache(داده))
      Curl_share_قفل(داده, CURL_قفل_داده_SSL_SESSION,
                      CURL_قفل_ACCESS_SINGLE);
#اگر_تعریف_شده USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#وإلا
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#پایان_اگر
  }
}

/* Unlock shared SSL session data */
فراغ Curl_ssl_scache_unlock(بنية Curl_easy *داده)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  إذا(scache) {
#اگر_تعریف_شده USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#وإلا
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#پایان_اگر
    إذا(CURL_SHARE_ssl_scache(داده))
      Curl_share_unlock(داده, CURL_قفل_داده_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(بنية Curl_easy *داده)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  bool locked = FALSE;
  إذا(!scache)
    أرجع FALSE;
#اگر_تعریف_شده USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#وإلا
  locked = (bool)scache->is_locked;
#پایان_اگر
  أرجع locked;
}

ساكن bool cf_ssl_scache_تطابق_auth(بنية Curl_ssl_scache_peer *peer,
                                     بنية ssl_filter_config *conn_config)
{
  إذا(!conn_config) {
    إذا(peer->clientcert || peer->کلید_passwd)
      أرجع FALSE;
    أرجع TRUE;
  }
  وإلا إذا(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    أرجع FALSE;
  إذا(Curl_timestrcmp(peer->کلید_passwd, conn_config->کلید_passwd))
    أرجع FALSE;
  أرجع TRUE;
}

ساكن CURLcode cf_ssl_پیدا_کن_peer_by_کلید(بنية Curl_easy *داده,
                                        بنية Curl_ssl_scache *scache,
                                        ثابت حرف *ssl_peer_کلید,
                                        بنية ssl_filter_config *conn_config,
                                        بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_کلید_len = 0;
  CURLcode نتیجه = CURLE_OK;

  *ppeer = عدم;
  إذا(!GOOD_SCACHE(scache)) {
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }

  CURL_TRC_SSLS(داده, "find peer slot for %s among %zu slots",
                ssl_peer_کلید, scache->peer_شمار);

  /* check for entries with known peer_key */
  لكل(i = 0; scache && i < scache->peer_شمار; i++) {
    إذا(scache->peers[i].ssl_peer_کلید &&
       !strcmp(ssl_peer_کلید, scache->peers[i].ssl_peer_کلید) &&
       cf_ssl_scache_تطابق_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      اذهب بیرون;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  لكل(i = 0; scache && i < scache->peer_شمار; i++) {
    إذا(!scache->peers[i].ssl_peer_کلید &&
       scache->peers[i].hmac_مجموعه &&
       cf_ssl_scache_تطابق_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      بیعلامت حرف my_hmac[CURL_SHA256_DIGEST_درازی];
      إذا(!peer_کلید_len) /* we are lazy */
        peer_کلید_len = طول_رشته(ssl_peer_کلید);
      نتیجه = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].کلید_salt,
                           حجم(scache->peers[i].کلید_salt),
                           (ثابت بیعلامت حرف *)ssl_peer_کلید,
                           peer_کلید_len,
                           my_hmac);
      إذا(نتیجه)
        اذهب بیرون;
      إذا(!memcmp(scache->peers[i].کلید_hmac, my_hmac, حجم(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(داده, "peer entry %zu key recovered: %s",
                      i, ssl_peer_کلید);
        scache->peers[i].ssl_peer_کلید = curlx_strdup(ssl_peer_کلید);
        إذا(!scache->peers[i].ssl_peer_کلید) {
          نتیجه = CURLE_بیرون_از_حافظه;
          اذهب بیرون;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        اذهب بیرون;
      }
    }
  }
  CURL_TRC_SSLS(داده, "peer not found for %s", ssl_peer_کلید);
بیرون:
  أرجع نتیجه;
}

ساكن بنية Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  بنية Curl_ssl_scache *scache)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  size_t i;

  /* find empty or oldest peer */
  لكل(i = 0; i < scache->peer_شمار; ++i) {
    /* free peer entry? */
    إذا(!scache->peers[i].ssl_peer_کلید && !scache->peers[i].hmac_مجموعه) {
      peer = &scache->peers[i];
      اكسر;
    }
    /* peer without sessions and obj */
    إذا(!scache->peers[i].sobj &&
       !Curl_llist_شمار(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      اكسر;
    }
    /* remember "oldest" peer */
    إذا(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  إذا(peer)
    cf_ssl_scache_clear_peer(peer);
  أرجع peer;
}

ساكن CURLcode cf_ssl_جمع_کن_peer(بنية Curl_easy *داده,
                                بنية Curl_ssl_scache *scache,
                                ثابت حرف *ssl_peer_کلید,
                                بنية ssl_filter_config *conn_config,
                                بنية Curl_ssl_scache_peer **ppeer)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتیجه = CURLE_OK;

  *ppeer = عدم;
  إذا(ssl_peer_کلید) {
    نتیجه = cf_ssl_پیدا_کن_peer_by_کلید(داده, scache, ssl_peer_کلید, conn_config,
                                     &peer);
    إذا(نتیجه || !scache->peer_شمار)
      أرجع نتیجه;
  }

  إذا(peer) {
    *ppeer = peer;
    أرجع CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  إذا(peer) {
    حرف میانگیر[64];
    ثابت حرف *ccert = conn_config ? conn_config->clientcert : عدم;
    ثابت حرف *kpasswd = conn_config ? conn_config->کلید_passwd : عدم;
    إذا(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(میانگیر, حجم(میانگیر),
                     "cert-%p", conn_config->cert_blob->داده);
      ccert = میانگیر; /* data is strduped by cf_ssl_scache_peer_init */
    }
    نتیجه = cf_ssl_scache_peer_init(peer, ssl_peer_کلید, ccert, kpasswd,
                                     عدم, عدم);
    إذا(نتیجه)
      اذهب بیرون;
    /* all ready */
    *ppeer = peer;
    نتیجه = CURLE_OK;
  }

بیرون:
  إذا(نتیجه) {
    cf_ssl_scache_clear_peer(peer);
  }
  أرجع نتیجه;
}

ساكن فراغ cf_scache_peer_جمع_کن_session(بنية Curl_ssl_scache_peer *peer,
                                       بنية Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  إذا(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, عدم);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  وإلا {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    طالما(Curl_llist_شمار(&peer->sessions) > peer->max_sessions) {
      Curl_گره_remove(Curl_llist_سر(&peer->sessions));
    }
  }
}

ساكن CURLcode cf_scache_جمع_کن_session(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *داده,
                                      بنية Curl_ssl_scache *scache,
                                      ثابت حرف *ssl_peer_کلید,
                                      بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;
  curl_off_t now = (curl_off_t)زمان(عدم);
  curl_off_t max_lifetime;

  إذا(!scache || !scache->peer_شمار) {
    Curl_ssl_session_destroy(s);
    أرجع CURLE_OK;
  }

  إذا(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  إذا(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  إذا(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(داده, "add, session already expired");
    Curl_ssl_session_destroy(s);
    أرجع CURLE_OK;
  }

  نتیجه = cf_ssl_جمع_کن_peer(داده, scache, ssl_peer_کلید, conn_config, &peer);
  إذا(نتیجه || !peer) {
    CURL_TRC_SSLS(داده, "unable to add scache peer: %d", (صحيح)نتیجه);
    Curl_ssl_session_destroy(s);
    اذهب بیرون;
  }

  cf_scache_peer_جمع_کن_session(peer, s, now);

بیرون:
  إذا(نتیجه) {
    failf(داده, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_کلید, (صحيح)نتیجه);
  }
  وإلا
    CURL_TRC_SSLS(داده, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_کلید,
                  (بیعلامت صحيح)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_شمار(&peer->sessions) : 0);
  أرجع نتیجه;
}

CURLcode Curl_ssl_scache_put(بنية Curl_cfilter *cf,
                             بنية Curl_easy *داده,
                             ثابت حرف *ssl_peer_کلید,
                             بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجه;
  DEBUGASSERT(conn_config);

  إذا(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    أرجع CURLE_OK;
  }

  Curl_ssl_scache_قفل(داده);
  نتیجه = cf_scache_جمع_کن_session(cf, داده, scache, ssl_peer_کلید, s);
  Curl_ssl_scache_unlock(داده);
  أرجع نتیجه;
}

فراغ Curl_ssl_scache_return(بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده,
                            ثابت حرف *ssl_peer_کلید,
                            بنية Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  إذا(s && s->ietf_tls_id < 0x304)
    (فراغ)Curl_ssl_scache_put(cf, داده, ssl_peer_کلید, s);
  وإلا
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              ثابت حرف *ssl_peer_کلید,
                              بنية Curl_ssl_session **ps)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_llist_گره *n;
  بنية Curl_ssl_session *s = عدم;
  CURLcode نتیجه;

  *ps = عدم;
  إذا(!scache)
    أرجع CURLE_OK;

  Curl_ssl_scache_قفل(داده);
  نتیجه = cf_ssl_پیدا_کن_peer_by_کلید(داده, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  إذا(!نتیجه && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)زمان(عدم));
    n = Curl_llist_سر(&peer->sessions);
    إذا(n) {
      s = Curl_گره_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  إذا(s) {
    *ps = s;
    CURL_TRC_SSLS(داده, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_کلید, (بیعلامت صحيح)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_شمار(&peer->sessions));
  }
  وإلا {
    CURL_TRC_SSLS(داده, "no cached session for %s", ssl_peer_کلید);
  }
  Curl_ssl_scache_unlock(داده);
  أرجع نتیجه;
}

CURLcode Curl_ssl_scache_جمع_کن_obj(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *داده,
                                 ثابت حرف *ssl_peer_کلید,
                                 فراغ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتیجه;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  إذا(!scache) {
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب بیرون;
  }

  نتیجه = cf_ssl_جمع_کن_peer(داده, scache, ssl_peer_کلید, conn_config, &peer);
  إذا(نتیجه || !peer) {
    CURL_TRC_SSLS(داده, "unable to add scache peer: %d", (صحيح)نتیجه);
    اذهب بیرون;
  }

  cf_ssl_scache_peer_مجموعه_obj(peer, sobj, sobj_dtor_cb);
  sobj = عدم;  /* peer took ownership */

بیرون:
  إذا(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  أرجع نتیجه;
}

فراغ *Curl_ssl_scache_get_obj(بنية Curl_cfilter *cf,
                              بنية Curl_easy *داده,
                              ثابت حرف *ssl_peer_کلید)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتیجه;
  فراغ *sobj;

  إذا(!scache)
    أرجع عدم;

  نتیجه = cf_ssl_پیدا_کن_peer_by_کلید(داده, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  إذا(نتیجه)
    أرجع عدم;

  sobj = peer ? peer->sobj : عدم;

  CURL_TRC_SSLS(داده, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_کلید);
  أرجع sobj;
}

فراغ Curl_ssl_scache_remove_all(بنية Curl_cfilter *cf,
                                بنية Curl_easy *داده,
                                ثابت حرف *ssl_peer_کلید)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتیجه;

  (فراغ)cf;
  إذا(!scache)
    أرجع;

  Curl_ssl_scache_قفل(داده);
  نتیجه = cf_ssl_پیدا_کن_peer_by_کلید(داده, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  إذا(!نتیجه && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(داده);
}

#اگر_تعریف_شده USE_SSLS_EXPORT

#تعریف_کن CURL_SSL_TICKET_MAX   (16 * 1024)

ساكن CURLcode cf_ssl_scache_peer_مجموعه_hmac(بنية Curl_ssl_scache_peer *peer)
{
  CURLcode نتیجه;

  DEBUGASSERT(peer);
  إذا(!peer->ssl_peer_کلید)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  نتیجه = Curl_rand(عدم, peer->کلید_salt, حجم(peer->کلید_salt));
  إذا(نتیجه)
    أرجع نتیجه;

  نتیجه = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->کلید_salt, حجم(peer->کلید_salt),
                       (ثابت بیعلامت حرف *)peer->ssl_peer_کلید,
                       طول_رشته(peer->ssl_peer_کلید),
                       peer->کلید_hmac);
  إذا(!نتیجه)
    peer->hmac_مجموعه = TRUE;
  أرجع نتیجه;
}

ساكن CURLcode cf_ssl_پیدا_کن_peer_by_hmac(بنية Curl_ssl_scache *scache,
                                         ثابت بیعلامت حرف *salt,
                                         ثابت بیعلامت حرف *hmac,
                                         بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode نتیجه = CURLE_OK;

  *ppeer = عدم;
  إذا(!GOOD_SCACHE(scache))
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  لكل(i = 0; scache && i < scache->peer_شمار; i++) {
    بنية Curl_ssl_scache_peer *peer = &scache->peers[i];
    إذا(!cf_ssl_scache_تطابق_auth(peer, عدم))
      استمر;
    إذا(scache->peers[i].hmac_مجموعه &&
       !memcmp(peer->کلید_salt, salt, حجم(peer->کلید_salt)) &&
       !memcmp(peer->کلید_hmac, hmac, حجم(peer->کلید_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      اذهب بیرون;
    }
    وإلا إذا(peer->ssl_peer_کلید) {
      بیعلامت حرف my_hmac[CURL_SHA256_DIGEST_درازی];
      /* compute hmac for the passed salt */
      نتیجه = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, حجم(peer->کلید_salt),
                           (ثابت بیعلامت حرف *)peer->ssl_peer_کلید,
                           طول_رشته(peer->ssl_peer_کلید),
                           my_hmac);
      إذا(نتیجه)
        اذهب بیرون;
      إذا(!memcmp(my_hmac, hmac, حجم(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        إذا(!peer->hmac_مجموعه) {
          کپی_حافظه(peer->کلید_salt, salt, حجم(peer->کلید_salt));
          کپی_حافظه(peer->کلید_hmac, hmac, حجم(peer->کلید_hmac));
          peer->hmac_مجموعه = TRUE;
        }
        *ppeer = peer;
        اذهب بیرون;
      }
    }
  }
بیرون:
  أرجع نتیجه;
}

CURLcode Curl_ssl_session_import(بنية Curl_easy *داده,
                                 ثابت حرف *ssl_peer_کلید,
                                 ثابت بیعلامت حرف *shmac, size_t shmac_len,
                                 ثابت فراغ *sdata, size_t sdata_len)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_ssl_session *s = عدم;
  bool have_قفل = FALSE;
  CURLcode نتیجه;

  إذا(!scache || !sdata || !sdata_len) {
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب بیرون;
  }
  إذا(!ssl_peer_کلید && (!shmac || !shmac_len)) {
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب بیرون;
  }

  نتیجه = Curl_ssl_session_unpack(داده, sdata, sdata_len, &s);
  إذا(نتیجه)
    اذهب بیرون;

  Curl_ssl_scache_قفل(داده);
  have_قفل = TRUE;

  إذا(ssl_peer_کلید) {
    نتیجه = cf_ssl_جمع_کن_peer(داده, scache, ssl_peer_کلید, عدم, &peer);
    إذا(نتیجه)
      اذهب بیرون;
  }
  وإلا إذا(shmac_len != (حجم(peer->کلید_salt) + حجم(peer->کلید_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    نتیجه = CURLE_BAD_FUNCTION_آرگومان;
    اذهب بیرون;
  }
  وإلا {
    ثابت بیعلامت حرف *salt = shmac;
    ثابت بیعلامت حرف *hmac = shmac + حجم(peer->کلید_salt);

    نتیجه = cf_ssl_پیدا_کن_peer_by_hmac(scache, salt, hmac, &peer);
    إذا(نتیجه)
      اذهب بیرون;
    إذا(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      إذا(peer) {
        نتیجه = cf_ssl_scache_peer_init(peer, ssl_peer_کلید, عدم, عدم,
                                         salt, hmac);
        إذا(نتیجه)
          اذهب بیرون;
      }
    }
  }

  إذا(peer) {
    cf_scache_peer_جمع_کن_session(peer, s, زمان(عدم));
    s = عدم; /* peer is now owner */
    CURL_TRC_SSLS(داده, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_کلید ? peer->ssl_peer_کلید : "without key",
                  Curl_llist_شمار(&peer->sessions));
  }

بیرون:
  إذا(scache && have_قفل)
    Curl_ssl_scache_unlock(داده);
  Curl_ssl_session_destroy(s);
  أرجع نتیجه;
}

CURLcode Curl_ssl_session_export(بنية Curl_easy *داده,
                                 curl_ssls_export_cb *export_fn,
                                 فراغ *userptr)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(داده);
  بنية Curl_ssl_scache_peer *peer;
  بنية dynbuf sbuf, hbuf;
  بنية Curl_llist_گره *n;
  size_t i;
  curl_off_t now = زمان(عدم);
  CURLcode نتیجه = CURLE_OK;
#اگر_تعریف_شده CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#پایان_اگر

  إذا(!export_fn)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  Curl_ssl_scache_قفل(داده);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_درازی * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  لكل(i = 0; scache && i < scache->peer_شمار; i++) {
    peer = &scache->peers[i];
    إذا(!peer->ssl_peer_کلید && !peer->hmac_مجموعه)
      استمر; /* skip free entry */
    إذا(!peer->exportable)
      استمر;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_سر(&peer->sessions);
    إذا(n)
      VERBOSE(++npeers);
    طالما(n) {
      بنية Curl_ssl_session *s = Curl_گره_elem(n);
      إذا(!peer->hmac_مجموعه) {
        نتیجه = cf_ssl_scache_peer_مجموعه_hmac(peer);
        إذا(نتیجه)
          اذهب بیرون;
      }
      إذا(!curlx_dyn_len(&hbuf)) {
        نتیجه = curlx_dyn_addn(&hbuf, peer->کلید_salt, حجم(peer->کلید_salt));
        إذا(نتیجه)
          اذهب بیرون;
        نتیجه = curlx_dyn_addn(&hbuf, peer->کلید_hmac, حجم(peer->کلید_hmac));
        إذا(نتیجه)
          اذهب بیرون;
      }
      curlx_dyn_reset(&sbuf);
      نتیجه = Curl_ssl_session_pack(داده, s, &sbuf);
      إذا(نتیجه)
        اذهب بیرون;

      نتیجه = export_fn(داده, userptr, peer->ssl_peer_کلید,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      إذا(نتیجه)
        اذهب بیرون;
      VERBOSE(++ntickets);
      n = Curl_گره_بعدی(n);
    }
  }
  نتیجه = CURLE_OK;
  CURL_TRC_SSLS(داده, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

بیرون:
  Curl_ssl_scache_unlock(داده);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  أرجع نتیجه;
}

#پایان_اگر /* USE_SSLS_EXPORT */

#پایان_اگر /* USE_SSL */
