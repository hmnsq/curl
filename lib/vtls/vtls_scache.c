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

#جيڪڏهن_وصف_ٿيل USE_SSL

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
  حرف *ssl_peer_ڪنجي;      /* id for peer + relevant TLS configuration */
  حرف *clientcert;
  حرف *ڪنجي_passwd;
  بنية Curl_llist sessions;
  فراغ *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  بنانشان حرف ڪنجي_salt[CURL_SHA256_DIGEST_ڊيگهائي]; /* for entry export */
  بنانشان حرف ڪنجي_hmac[CURL_SHA256_DIGEST_ڊيگهائي]; /* for entry export */
  size_t max_sessions;
  ڊگهو age;                /* a number, the higher the more recent */
  ٻٽو_انگ(hmac_سيٽ);           /* if key_salt and key_hmac are present */
  ٻٽو_انگ(exportable);         /* sessions for this peer can be exported */
};

#وصف_ڪريو CURL_SCACHE_MAGIC 0x000e1551

#وصف_ڪريو GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ساكن CURLcode cf_ssl_peer_ڪنجي_جوڙيو_string(بنية dynbuf *buf,
                                           ثابت حرف *name,
                                           ثابت حرف *قدر)
{
  أرجع curlx_dyn_addf(buf, ":%s-%zu:%s", name, لڙي_ڊيگهه(قدر), قدر);
}

ساكن CURLcode cf_ssl_peer_ڪنجي_جوڙيو_رستو(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         ثابت حرف *رستو,
                                         bool *is_local)
{
  إذا(رستو && رستو[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#جيڪڏهن_وصف_ٿيل _WIN32
    حرف *abspath = _fullpath(عدم, رستو, 0);
    إذا(abspath) {
      CURLcode نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      حرر(abspath); /* allocated by CRT, use system free() */
      أرجع نتيجو;
    }
    *is_local = TRUE;
#وإلاإذا defined(HAVE_REALPATH)
    إذا(رستو[0] != '/') {
      حرف *abspath = realpath(رستو, عدم);
      إذا(abspath) {
        CURLcode نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        حرر(abspath); /* allocated by libc, free without memdebug */
        أرجع نتيجو;
      }
      *is_local = TRUE;
    }
#جيڪڏهن_پورو
    أرجع cf_ssl_peer_ڪنجي_جوڙيو_string(buf, name, رستو);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ssl_peer_ڪنجي_جوڙيو_hash(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         بنية curl_blob *blob)
{
  CURLcode نتيجو = CURLE_OK;
  إذا(blob && blob->len) {
    بنانشان حرف hash[CURL_SHA256_DIGEST_ڊيگهائي];
    size_t i;

    نتيجو = curlx_dyn_addf(buf, ":%s-", name);
    إذا(نتيجو)
      اذهب ٻاهريون;
    نتيجو = Curl_sha256it(hash, blob->معلومات, blob->len);
    إذا(نتيجو)
      اذهب ٻاهريون;
    لكل(i = 0; i < CURL_SHA256_DIGEST_ڊيگهائي; ++i) {
      نتيجو = curlx_dyn_addf(buf, "%02x", hash[i]);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
  }
ٻاهريون:
  أرجع نتيجو;
}

ساكن CURLcode cf_ssl_peer_ڪنجي_جوڙيو_mtls(بنية dynbuf *buf,
                                         بنية ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode نتيجو = CURLE_OK;
  إذا(ssl->clientcert && ssl->clientcert[0]) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_رستو(buf, "CCERT", ssl->clientcert, is_local);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->ڪنجي && ssl->ڪنجي[0]) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_رستو(buf, "KEY", ssl->ڪنجي, is_local);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->ڪنجي_blob) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_hash(buf, "KEYBlob", ssl->ڪنجي_blob);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    نتيجو = curlx_dyn_addf(buf, ":CT-%zu:", لڙي_ڊيگهه(ssl->cert_type));
    لكل(i = 0; !نتيجو && ssl->cert_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->cert_type[i]);
      نتيجو = curlx_dyn_addn(buf, &c, 1);
    }
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->ڪنجي_type && ssl->ڪنجي_type[0]) {
    size_t i;
    نتيجو = curlx_dyn_addf(buf, ":KT-%zu:", لڙي_ڊيگهه(ssl->ڪنجي_type));
    لكل(i = 0; !نتيجو && ssl->ڪنجي_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->ڪنجي_type[i]);
      نتيجو = curlx_dyn_addn(buf, &c, 1);
    }
  }
ٻاهريون:
  أرجع نتيجو;
}

#وصف_ڪريو CURL_SSLS_LOCAL_SUFFIX     ":L"
#وصف_ڪريو CURL_SSLS_GLOBAL_SUFFIX    ":G"

ساكن bool cf_ssl_peer_ڪنجي_is_global(ثابت حرف *peer_ڪنجي)
{
  size_t len = peer_ڪنجي ? لڙي_ڊيگهه(peer_ڪنجي) : 0;
  أرجع (len > 2) &&
         (peer_ڪنجي[len - 1] == 'G') &&
         (peer_ڪنجي[len - 2] == ':');
}

ساكن CURLcode ssl_peer_ڪنجي_جوڙيو_transport(بنية dynbuf *buf,
                                           uint8_t transport)
{
  بدل(transport) {
  حالة TRNSPRT_TCP:
    أرجع CURLE_OK;
  حالة TRNSPRT_UDP:
    أرجع curlx_dyn_جوڙيو(buf, ":UDP");
  حالة TRNSPRT_QUIC:
    أرجع curlx_dyn_جوڙيو(buf, ":QUIC");
  حالة TRNSPRT_UNIX:
    أرجع curlx_dyn_جوڙيو(buf, ":UNIX");
  افتراضي:
    أرجع curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ساكن CURLcode ssl_peer_ڪنجي_جوڙيو_vrfy(بنية dynbuf *buf,
                                      بنية ssl_filter_config *ssl,
                                      ثابت بنية ssl_peer *peer)
{
  CURLcode نتيجو;

  إذا(!ssl->verifypeer) {
    نتيجو = curlx_dyn_جوڙيو(buf, ":NO-VRFY-PEER");
    إذا(نتيجو)
      أرجع نتيجو;
  }
  إذا(!ssl->verifyhost) {
    نتيجو = curlx_dyn_جوڙيو(buf, ":NO-VRFY-HOST");
    إذا(نتيجو)
      أرجع نتيجو;
  }
  إذا(ssl->verifystatus) {
    نتيجو = curlx_dyn_جوڙيو(buf, ":VRFY-STATUS");
    إذا(نتيجو)
      أرجع نتيجو;
  }
  إذا((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_string(buf, "CHOST", peer->peer->hostname);
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->پورٽ);
    إذا(نتيجو)
      أرجع نتيجو;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssl_peer_ڪنجي_build(بنية ssl_filter_config *ssl,
                                   ثابت بنية ssl_peer *peer,
                                   ثابت حرف *tls_id,
                                   حرف **ppeer_ڪنجي)
{
  بنية dynbuf buf;
  size_t ڪنجي_len;
  bool is_local = FALSE;
  CURLcode نتيجو;

  *ppeer_ڪنجي = عدم;
  curlx_dyn_init(&buf, 10 * 1024);

  نتيجو = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->پورٽ);
  إذا(نتيجو)
    اذهب ٻاهريون;
  نتيجو = ssl_peer_ڪنجي_جوڙيو_transport(&buf, peer->transport);
  إذا(نتيجو)
    اذهب ٻاهريون;
  نتيجو = ssl_peer_ڪنجي_جوڙيو_vrfy(&buf, ssl, peer);
  إذا(نتيجو)
    اذهب ٻاهريون;
  إذا(ssl->نسخو_2 || ssl->نسخو_max) {
    نتيجو = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->نسخو_2,
                            (ssl->نسخو_max >> 16));
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->ssl_options) {
    نتيجو = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->cipher_list) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_string(&buf, "CIPHER", ssl->cipher_list);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->cipher_list13) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_string(&buf, "CIPHER13", ssl->cipher_list13);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->curves) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_string(&buf, "CURVES", ssl->curves);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->signature_algorithms) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->verifypeer) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_رستو(&buf, "CA", ssl->CAfile, &is_local);
    إذا(!نتيجو)
      نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_رستو(&buf, "CApath",
                                        ssl->CApath, &is_local);
    إذا(!نتيجو && ssl->native_ca_store)
      نتيجو = curlx_dyn_جوڙيو(&buf, ":NATIVE-CA");
    إذا(!نتيجو)
      نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_رستو(&buf, "CRL", ssl->CRLfile, &is_local);
    إذا(نتيجو)
      اذهب ٻاهريون;
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_رستو(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    إذا(نتيجو)
      اذهب ٻاهريون;
    إذا(ssl->ca_info_blob) {
      نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
    إذا(ssl->issuercert_blob) {
      نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      إذا(نتيجو)
        اذهب ٻاهريون;
    }
  }
  إذا(ssl->cert_blob) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_hash(&buf, "CertBlob", ssl->cert_blob);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  إذا(ssl->pinned_ڪنجي && ssl->pinned_ڪنجي[0]) {
    نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_string(&buf, "Pinned", ssl->pinned_ڪنجي);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }

  نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_mtls(&buf, ssl, &is_local);
  إذا(نتيجو)
    اذهب ٻاهريون;

  إذا(!tls_id || !tls_id[0]) {
    نتيجو = CURLE_FAILED_INIT;
    اذهب ٻاهريون;
  }
  نتيجو = cf_ssl_peer_ڪنجي_جوڙيو_string(&buf, "IMPL", tls_id);
  إذا(نتيجو)
    اذهب ٻاهريون;

  نتيجو = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  إذا(نتيجو)
    اذهب ٻاهريون;

  *ppeer_ڪنجي = curlx_dyn_take(&buf, &ڪنجي_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

ٻاهريون:
  curlx_dyn_free(&buf);
  أرجع نتيجو;
}

CURLcode Curl_ssl_peer_ڪنجي_make(ثابت بنية ssl_peer *peer,
                                بنية ssl_filter_config *sslc,
                                ثابت حرف *tls_id,
                                حرف **ppeer_ڪنجي)
{
  أرجع ssl_peer_ڪنجي_build(sslc, peer, tls_id, ppeer_ڪنجي);
}

بنية Curl_ssl_scache {
  بنانشان صحيح magic;
  بنية Curl_ssl_scache_peer *peers;
  size_t peer_ڳڻپ;
  صحيح default_lifetime_secs;
  ڊگهو age;
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#جيڪڏهن_پورو
  ٻٽو_انگ(is_locked);
};

ساكن بنية Curl_ssl_scache *cf_ssl_scache_get(بنية Curl_easy *معلومات)
{
  بنية Curl_ssl_scache *scache = عدم;
  /* If a share is present, its ssl_scache has preference over the multi */
  إذا(معلومات->share && معلومات->share->ssl_scache)
    scache = معلومات->share->ssl_scache;
  وإلا إذا(معلومات->multi && معلومات->multi->ssl_scache)
    scache = معلومات->multi->ssl_scache;
  إذا(scache && !GOOD_SCACHE(scache)) {
    failf(معلومات, "transfer would use an invalid scache at %p, denied",
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
                                  بنانشان حرف *quic_tp, size_t quic_tp_len,
                                  بنية Curl_ssl_session **psession)
{
  بنية Curl_ssl_session *s;

  إذا(!sdata || !sdata_len) {
    curlx_free(sdata);
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  *psession = عدم;
  s = curlx_calloc(1, حجم(*s));
  إذا(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
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
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }
  *psession = s;
  أرجع CURLE_OK;
}

فراغ Curl_ssl_session_destroy(بنية Curl_ssl_session *s)
{
  إذا(s) {
    /* if in the list, the list destructor takes care of it */
    إذا(Curl_ڳنڍ_llist(&s->list))
      Curl_ڳنڍ_remove(&s->list);
    وإلا {
      cf_ssl_scache_session_ldestroy(عدم, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(بنية Curl_ssl_session *src,
                              بنية Curl_ssl_session **pdest)
{
  بنية Curl_ssl_session *dest = عدم;
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  إذا(!src || !pdest)
    أرجع CURLE_BAD_FUNCTION_دليل;
  *pdest = عدم;

  dest = curlx_calloc(1, حجم(*dest));
  إذا(!dest)
    اذهب ٻاهريون;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  إذا(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    إذا(!dest->sdata)
      اذهب ٻاهريون;
    dest->sdata_len = src->sdata_len;
  }
  إذا(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    إذا(!dest->quic_tp)
      اذهب ٻاهريون;
    dest->quic_tp_len = src->quic_tp_len;
  }
  إذا(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    إذا(!dest->alpn)
      اذهب ٻاهريون;
  }
  نتيجو = CURLE_OK;

ٻاهريون:
  إذا(!نتيجو)
    *pdest = dest;
  وإلا {
    *pdest = عدم;
    إذا(dest)
      Curl_ssl_session_destroy(dest);
  }
  أرجع نتيجو;
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
  curlx_safefree(peer->ڪنجي_passwd);
  curlx_safefree(peer->ssl_peer_ڪنجي);
  peer->age = 0;
  peer->hmac_سيٽ = FALSE;
}

ساكن فراغ cf_ssl_scache_peer_سيٽ_obj(بنية Curl_ssl_scache_peer *peer,
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
  peer->exportable = (!peer->clientcert && !peer->ڪنجي_passwd &&
                      (!peer->ssl_peer_ڪنجي ||
                       cf_ssl_peer_ڪنجي_is_global(peer->ssl_peer_ڪنجي)));
}

ساكن CURLcode cf_ssl_scache_peer_init(بنية Curl_ssl_scache_peer *peer,
                                        ثابت حرف *ssl_peer_ڪنجي,
                                        ثابت حرف *clientcert,
                                        ثابت حرف *ڪنجي_passwd,
                                        ثابت بنانشان حرف *salt,
                                        ثابت بنانشان حرف *hmac)
{
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;

  DEBUGASSERT(!peer->ssl_peer_ڪنجي);
  إذا(ssl_peer_ڪنجي) {
    peer->ssl_peer_ڪنجي = curlx_strdup(ssl_peer_ڪنجي);
    إذا(!peer->ssl_peer_ڪنجي)
      اذهب ٻاهريون;
    peer->hmac_سيٽ = FALSE;
  }
  وإلا إذا(salt && hmac) {
    يادگيري_نقل(peer->ڪنجي_salt, salt, حجم(peer->ڪنجي_salt));
    يادگيري_نقل(peer->ڪنجي_hmac, hmac, حجم(peer->ڪنجي_hmac));
    peer->hmac_سيٽ = TRUE;
  }
  وإلا {
    نتيجو = CURLE_BAD_FUNCTION_دليل;
    اذهب ٻاهريون;
  }
  إذا(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    إذا(!peer->clientcert)
      اذهب ٻاهريون;
  }
  إذا(ڪنجي_passwd) {
    peer->ڪنجي_passwd = curlx_strdup(ڪنجي_passwd);
    إذا(!peer->ڪنجي_passwd)
      اذهب ٻاهريون;
  }
  cf_ssl_cache_peer_update(peer);
  نتيجو = CURLE_OK;
ٻاهريون:
  إذا(نتيجو)
    cf_ssl_scache_clear_peer(peer);
  أرجع نتيجو;
}

ساكن فراغ cf_scache_session_remove(بنية Curl_ssl_scache_peer *peer,
                                     بنية Curl_ssl_session *s)
{
  (فراغ)peer;
  DEBUGASSERT(Curl_ڳنڍ_llist(&s->list) == &peer->sessions);
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
  بنية Curl_llist_ڳنڍ *n = Curl_llist_سر(&peer->sessions);
  طالما(n) {
    بنية Curl_ssl_session *s = Curl_ڳنڍ_elem(n);
    n = Curl_ڳنڍ_ايندڙ(n);
    إذا(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ساكن فراغ cf_scache_peer_remove_non13(بنية Curl_ssl_scache_peer *peer)
{
  بنية Curl_llist_ڳنڍ *n = Curl_llist_سر(&peer->sessions);
  طالما(n) {
    بنية Curl_ssl_session *s = Curl_ڳنڍ_elem(n);
    n = Curl_ڳنڍ_ايندڙ(n);
    إذا(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_في_peer,
                                بنية Curl_ssl_scache **pscache)
{
  بنية Curl_ssl_scache *scache;
  بنية Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = عدم;
  peers = curlx_calloc(max_peers, حجم(*peers));
  إذا(!peers)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  scache = curlx_calloc(1, حجم(*scache));
  إذا(!scache) {
    curlx_free(peers);
    أرجع CURLE_ٻاهريون_جو_يادگيري;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_ڳڻپ = max_peers;
  scache->peers = peers;
  scache->age = 1;
  لكل(i = 0; i < scache->peer_ڳڻپ; ++i) {
    scache->peers[i].max_sessions = max_sessions_في_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#جيڪڏهن_پورو
  *pscache = scache;
  أرجع CURLE_OK;
}

فراغ Curl_ssl_scache_destroy(بنية Curl_ssl_scache *scache)
{
  إذا(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    لكل(i = 0; i < scache->peer_ڳڻپ; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#جيڪڏهن_وصف_ٿيل USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#جيڪڏهن_پورو
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(بنية Curl_cfilter *cf, بنية Curl_easy *معلومات)
{
  إذا(cf_ssl_scache_get(معلومات)) {
    بنية ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    أرجع conn_config ? conn_config->cache_session : FALSE;
  }
  أرجع FALSE;
}

/* Lock shared SSL session data */
فراغ Curl_ssl_scache_تالو(بنية Curl_easy *معلومات)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  إذا(scache) {
    إذا(CURL_SHARE_ssl_scache(معلومات))
      Curl_share_تالو(معلومات, CURL_تالو_معلومات_SSL_SESSION,
                      CURL_تالو_ACCESS_SINGLE);
#جيڪڏهن_وصف_ٿيل USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#وإلا
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#جيڪڏهن_پورو
  }
}

/* Unlock shared SSL session data */
فراغ Curl_ssl_scache_unlock(بنية Curl_easy *معلومات)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  إذا(scache) {
#جيڪڏهن_وصف_ٿيل USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#وإلا
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#جيڪڏهن_پورو
    إذا(CURL_SHARE_ssl_scache(معلومات))
      Curl_share_unlock(معلومات, CURL_تالو_معلومات_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(بنية Curl_easy *معلومات)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  bool locked = FALSE;
  إذا(!scache)
    أرجع FALSE;
#جيڪڏهن_وصف_ٿيل USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#وإلا
  locked = (bool)scache->is_locked;
#جيڪڏهن_پورو
  أرجع locked;
}

ساكن bool cf_ssl_scache_ميلاپ_auth(بنية Curl_ssl_scache_peer *peer,
                                     بنية ssl_filter_config *conn_config)
{
  إذا(!conn_config) {
    إذا(peer->clientcert || peer->ڪنجي_passwd)
      أرجع FALSE;
    أرجع TRUE;
  }
  وإلا إذا(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    أرجع FALSE;
  إذا(Curl_timestrcmp(peer->ڪنجي_passwd, conn_config->ڪنجي_passwd))
    أرجع FALSE;
  أرجع TRUE;
}

ساكن CURLcode cf_ssl_ڳوليو_peer_by_ڪنجي(بنية Curl_easy *معلومات,
                                        بنية Curl_ssl_scache *scache,
                                        ثابت حرف *ssl_peer_ڪنجي,
                                        بنية ssl_filter_config *conn_config,
                                        بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_ڪنجي_len = 0;
  CURLcode نتيجو = CURLE_OK;

  *ppeer = عدم;
  إذا(!GOOD_SCACHE(scache)) {
    أرجع CURLE_BAD_FUNCTION_دليل;
  }

  CURL_TRC_SSLS(معلومات, "find peer slot for %s among %zu slots",
                ssl_peer_ڪنجي, scache->peer_ڳڻپ);

  /* check for entries with known peer_key */
  لكل(i = 0; scache && i < scache->peer_ڳڻپ; i++) {
    إذا(scache->peers[i].ssl_peer_ڪنجي &&
       !strcmp(ssl_peer_ڪنجي, scache->peers[i].ssl_peer_ڪنجي) &&
       cf_ssl_scache_ميلاپ_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      اذهب ٻاهريون;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  لكل(i = 0; scache && i < scache->peer_ڳڻپ; i++) {
    إذا(!scache->peers[i].ssl_peer_ڪنجي &&
       scache->peers[i].hmac_سيٽ &&
       cf_ssl_scache_ميلاپ_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      بنانشان حرف my_hmac[CURL_SHA256_DIGEST_ڊيگهائي];
      إذا(!peer_ڪنجي_len) /* we are lazy */
        peer_ڪنجي_len = لڙي_ڊيگهه(ssl_peer_ڪنجي);
      نتيجو = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].ڪنجي_salt,
                           حجم(scache->peers[i].ڪنجي_salt),
                           (ثابت بنانشان حرف *)ssl_peer_ڪنجي,
                           peer_ڪنجي_len,
                           my_hmac);
      إذا(نتيجو)
        اذهب ٻاهريون;
      إذا(!memcmp(scache->peers[i].ڪنجي_hmac, my_hmac, حجم(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(معلومات, "peer entry %zu key recovered: %s",
                      i, ssl_peer_ڪنجي);
        scache->peers[i].ssl_peer_ڪنجي = curlx_strdup(ssl_peer_ڪنجي);
        إذا(!scache->peers[i].ssl_peer_ڪنجي) {
          نتيجو = CURLE_ٻاهريون_جو_يادگيري;
          اذهب ٻاهريون;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        اذهب ٻاهريون;
      }
    }
  }
  CURL_TRC_SSLS(معلومات, "peer not found for %s", ssl_peer_ڪنجي);
ٻاهريون:
  أرجع نتيجو;
}

ساكن بنية Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  بنية Curl_ssl_scache *scache)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  size_t i;

  /* find empty or oldest peer */
  لكل(i = 0; i < scache->peer_ڳڻپ; ++i) {
    /* free peer entry? */
    إذا(!scache->peers[i].ssl_peer_ڪنجي && !scache->peers[i].hmac_سيٽ) {
      peer = &scache->peers[i];
      اكسر;
    }
    /* peer without sessions and obj */
    إذا(!scache->peers[i].sobj &&
       !Curl_llist_ڳڻپ(&scache->peers[i].sessions)) {
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

ساكن CURLcode cf_ssl_جوڙيو_peer(بنية Curl_easy *معلومات,
                                بنية Curl_ssl_scache *scache,
                                ثابت حرف *ssl_peer_ڪنجي,
                                بنية ssl_filter_config *conn_config,
                                بنية Curl_ssl_scache_peer **ppeer)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتيجو = CURLE_OK;

  *ppeer = عدم;
  إذا(ssl_peer_ڪنجي) {
    نتيجو = cf_ssl_ڳوليو_peer_by_ڪنجي(معلومات, scache, ssl_peer_ڪنجي, conn_config,
                                     &peer);
    إذا(نتيجو || !scache->peer_ڳڻپ)
      أرجع نتيجو;
  }

  إذا(peer) {
    *ppeer = peer;
    أرجع CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  إذا(peer) {
    حرف بفر[64];
    ثابت حرف *ccert = conn_config ? conn_config->clientcert : عدم;
    ثابت حرف *kpasswd = conn_config ? conn_config->ڪنجي_passwd : عدم;
    إذا(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(بفر, حجم(بفر),
                     "cert-%p", conn_config->cert_blob->معلومات);
      ccert = بفر; /* data is strduped by cf_ssl_scache_peer_init */
    }
    نتيجو = cf_ssl_scache_peer_init(peer, ssl_peer_ڪنجي, ccert, kpasswd,
                                     عدم, عدم);
    إذا(نتيجو)
      اذهب ٻاهريون;
    /* all ready */
    *ppeer = peer;
    نتيجو = CURLE_OK;
  }

ٻاهريون:
  إذا(نتيجو) {
    cf_ssl_scache_clear_peer(peer);
  }
  أرجع نتيجو;
}

ساكن فراغ cf_scache_peer_جوڙيو_session(بنية Curl_ssl_scache_peer *peer,
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
    طالما(Curl_llist_ڳڻپ(&peer->sessions) > peer->max_sessions) {
      Curl_ڳنڍ_remove(Curl_llist_سر(&peer->sessions));
    }
  }
}

ساكن CURLcode cf_scache_جوڙيو_session(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      بنية Curl_ssl_scache *scache,
                                      ثابت حرف *ssl_peer_ڪنجي,
                                      بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتيجو = CURLE_ٻاهريون_جو_يادگيري;
  curl_off_t now = (curl_off_t)وقت(عدم);
  curl_off_t max_lifetime;

  إذا(!scache || !scache->peer_ڳڻپ) {
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
    CURL_TRC_SSLS(معلومات, "add, session already expired");
    Curl_ssl_session_destroy(s);
    أرجع CURLE_OK;
  }

  نتيجو = cf_ssl_جوڙيو_peer(معلومات, scache, ssl_peer_ڪنجي, conn_config, &peer);
  إذا(نتيجو || !peer) {
    CURL_TRC_SSLS(معلومات, "unable to add scache peer: %d", (صحيح)نتيجو);
    Curl_ssl_session_destroy(s);
    اذهب ٻاهريون;
  }

  cf_scache_peer_جوڙيو_session(peer, s, now);

ٻاهريون:
  إذا(نتيجو) {
    failf(معلومات, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_ڪنجي, (صحيح)نتيجو);
  }
  وإلا
    CURL_TRC_SSLS(معلومات, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_ڪنجي,
                  (بنانشان صحيح)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_ڳڻپ(&peer->sessions) : 0);
  أرجع نتيجو;
}

CURLcode Curl_ssl_scache_put(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             ثابت حرف *ssl_peer_ڪنجي,
                             بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتيجو;
  DEBUGASSERT(conn_config);

  إذا(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    أرجع CURLE_OK;
  }

  Curl_ssl_scache_تالو(معلومات);
  نتيجو = cf_scache_جوڙيو_session(cf, معلومات, scache, ssl_peer_ڪنجي, s);
  Curl_ssl_scache_unlock(معلومات);
  أرجع نتيجو;
}

فراغ Curl_ssl_scache_return(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            ثابت حرف *ssl_peer_ڪنجي,
                            بنية Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  إذا(s && s->ietf_tls_id < 0x304)
    (فراغ)Curl_ssl_scache_put(cf, معلومات, ssl_peer_ڪنجي, s);
  وإلا
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              ثابت حرف *ssl_peer_ڪنجي,
                              بنية Curl_ssl_session **ps)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_llist_ڳنڍ *n;
  بنية Curl_ssl_session *s = عدم;
  CURLcode نتيجو;

  *ps = عدم;
  إذا(!scache)
    أرجع CURLE_OK;

  Curl_ssl_scache_تالو(معلومات);
  نتيجو = cf_ssl_ڳوليو_peer_by_ڪنجي(معلومات, scache, ssl_peer_ڪنجي, conn_config,
                                   &peer);
  إذا(!نتيجو && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)وقت(عدم));
    n = Curl_llist_سر(&peer->sessions);
    إذا(n) {
      s = Curl_ڳنڍ_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  إذا(s) {
    *ps = s;
    CURL_TRC_SSLS(معلومات, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_ڪنجي, (بنانشان صحيح)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_ڳڻپ(&peer->sessions));
  }
  وإلا {
    CURL_TRC_SSLS(معلومات, "no cached session for %s", ssl_peer_ڪنجي);
  }
  Curl_ssl_scache_unlock(معلومات);
  أرجع نتيجو;
}

CURLcode Curl_ssl_scache_جوڙيو_obj(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 ثابت حرف *ssl_peer_ڪنجي,
                                 فراغ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتيجو;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  إذا(!scache) {
    نتيجو = CURLE_BAD_FUNCTION_دليل;
    اذهب ٻاهريون;
  }

  نتيجو = cf_ssl_جوڙيو_peer(معلومات, scache, ssl_peer_ڪنجي, conn_config, &peer);
  إذا(نتيجو || !peer) {
    CURL_TRC_SSLS(معلومات, "unable to add scache peer: %d", (صحيح)نتيجو);
    اذهب ٻاهريون;
  }

  cf_ssl_scache_peer_سيٽ_obj(peer, sobj, sobj_dtor_cb);
  sobj = عدم;  /* peer took ownership */

ٻاهريون:
  إذا(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  أرجع نتيجو;
}

فراغ *Curl_ssl_scache_get_obj(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              ثابت حرف *ssl_peer_ڪنجي)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتيجو;
  فراغ *sobj;

  إذا(!scache)
    أرجع عدم;

  نتيجو = cf_ssl_ڳوليو_peer_by_ڪنجي(معلومات, scache, ssl_peer_ڪنجي, conn_config,
                                   &peer);
  إذا(نتيجو)
    أرجع عدم;

  sobj = peer ? peer->sobj : عدم;

  CURL_TRC_SSLS(معلومات, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_ڪنجي);
  أرجع sobj;
}

فراغ Curl_ssl_scache_remove_all(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                ثابت حرف *ssl_peer_ڪنجي)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتيجو;

  (فراغ)cf;
  إذا(!scache)
    أرجع;

  Curl_ssl_scache_تالو(معلومات);
  نتيجو = cf_ssl_ڳوليو_peer_by_ڪنجي(معلومات, scache, ssl_peer_ڪنجي, conn_config,
                                   &peer);
  إذا(!نتيجو && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(معلومات);
}

#جيڪڏهن_وصف_ٿيل USE_SSLS_EXPORT

#وصف_ڪريو CURL_SSL_TICKET_MAX   (16 * 1024)

ساكن CURLcode cf_ssl_scache_peer_سيٽ_hmac(بنية Curl_ssl_scache_peer *peer)
{
  CURLcode نتيجو;

  DEBUGASSERT(peer);
  إذا(!peer->ssl_peer_ڪنجي)
    أرجع CURLE_BAD_FUNCTION_دليل;

  نتيجو = Curl_rand(عدم, peer->ڪنجي_salt, حجم(peer->ڪنجي_salt));
  إذا(نتيجو)
    أرجع نتيجو;

  نتيجو = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->ڪنجي_salt, حجم(peer->ڪنجي_salt),
                       (ثابت بنانشان حرف *)peer->ssl_peer_ڪنجي,
                       لڙي_ڊيگهه(peer->ssl_peer_ڪنجي),
                       peer->ڪنجي_hmac);
  إذا(!نتيجو)
    peer->hmac_سيٽ = TRUE;
  أرجع نتيجو;
}

ساكن CURLcode cf_ssl_ڳوليو_peer_by_hmac(بنية Curl_ssl_scache *scache,
                                         ثابت بنانشان حرف *salt,
                                         ثابت بنانشان حرف *hmac,
                                         بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode نتيجو = CURLE_OK;

  *ppeer = عدم;
  إذا(!GOOD_SCACHE(scache))
    أرجع CURLE_BAD_FUNCTION_دليل;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  لكل(i = 0; scache && i < scache->peer_ڳڻپ; i++) {
    بنية Curl_ssl_scache_peer *peer = &scache->peers[i];
    إذا(!cf_ssl_scache_ميلاپ_auth(peer, عدم))
      استمر;
    إذا(scache->peers[i].hmac_سيٽ &&
       !memcmp(peer->ڪنجي_salt, salt, حجم(peer->ڪنجي_salt)) &&
       !memcmp(peer->ڪنجي_hmac, hmac, حجم(peer->ڪنجي_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      اذهب ٻاهريون;
    }
    وإلا إذا(peer->ssl_peer_ڪنجي) {
      بنانشان حرف my_hmac[CURL_SHA256_DIGEST_ڊيگهائي];
      /* compute hmac for the passed salt */
      نتيجو = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, حجم(peer->ڪنجي_salt),
                           (ثابت بنانشان حرف *)peer->ssl_peer_ڪنجي,
                           لڙي_ڊيگهه(peer->ssl_peer_ڪنجي),
                           my_hmac);
      إذا(نتيجو)
        اذهب ٻاهريون;
      إذا(!memcmp(my_hmac, hmac, حجم(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        إذا(!peer->hmac_سيٽ) {
          يادگيري_نقل(peer->ڪنجي_salt, salt, حجم(peer->ڪنجي_salt));
          يادگيري_نقل(peer->ڪنجي_hmac, hmac, حجم(peer->ڪنجي_hmac));
          peer->hmac_سيٽ = TRUE;
        }
        *ppeer = peer;
        اذهب ٻاهريون;
      }
    }
  }
ٻاهريون:
  أرجع نتيجو;
}

CURLcode Curl_ssl_session_import(بنية Curl_easy *معلومات,
                                 ثابت حرف *ssl_peer_ڪنجي,
                                 ثابت بنانشان حرف *shmac, size_t shmac_len,
                                 ثابت فراغ *sdata, size_t sdata_len)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_ssl_session *s = عدم;
  bool have_تالو = FALSE;
  CURLcode نتيجو;

  إذا(!scache || !sdata || !sdata_len) {
    نتيجو = CURLE_BAD_FUNCTION_دليل;
    اذهب ٻاهريون;
  }
  إذا(!ssl_peer_ڪنجي && (!shmac || !shmac_len)) {
    نتيجو = CURLE_BAD_FUNCTION_دليل;
    اذهب ٻاهريون;
  }

  نتيجو = Curl_ssl_session_unpack(معلومات, sdata, sdata_len, &s);
  إذا(نتيجو)
    اذهب ٻاهريون;

  Curl_ssl_scache_تالو(معلومات);
  have_تالو = TRUE;

  إذا(ssl_peer_ڪنجي) {
    نتيجو = cf_ssl_جوڙيو_peer(معلومات, scache, ssl_peer_ڪنجي, عدم, &peer);
    إذا(نتيجو)
      اذهب ٻاهريون;
  }
  وإلا إذا(shmac_len != (حجم(peer->ڪنجي_salt) + حجم(peer->ڪنجي_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    نتيجو = CURLE_BAD_FUNCTION_دليل;
    اذهب ٻاهريون;
  }
  وإلا {
    ثابت بنانشان حرف *salt = shmac;
    ثابت بنانشان حرف *hmac = shmac + حجم(peer->ڪنجي_salt);

    نتيجو = cf_ssl_ڳوليو_peer_by_hmac(scache, salt, hmac, &peer);
    إذا(نتيجو)
      اذهب ٻاهريون;
    إذا(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      إذا(peer) {
        نتيجو = cf_ssl_scache_peer_init(peer, ssl_peer_ڪنجي, عدم, عدم,
                                         salt, hmac);
        إذا(نتيجو)
          اذهب ٻاهريون;
      }
    }
  }

  إذا(peer) {
    cf_scache_peer_جوڙيو_session(peer, s, وقت(عدم));
    s = عدم; /* peer is now owner */
    CURL_TRC_SSLS(معلومات, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_ڪنجي ? peer->ssl_peer_ڪنجي : "without key",
                  Curl_llist_ڳڻپ(&peer->sessions));
  }

ٻاهريون:
  إذا(scache && have_تالو)
    Curl_ssl_scache_unlock(معلومات);
  Curl_ssl_session_destroy(s);
  أرجع نتيجو;
}

CURLcode Curl_ssl_session_export(بنية Curl_easy *معلومات,
                                 curl_ssls_export_cb *export_fn,
                                 فراغ *userptr)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية Curl_ssl_scache_peer *peer;
  بنية dynbuf sbuf, hbuf;
  بنية Curl_llist_ڳنڍ *n;
  size_t i;
  curl_off_t now = وقت(عدم);
  CURLcode نتيجو = CURLE_OK;
#جيڪڏهن_وصف_ٿيل CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#جيڪڏهن_پورو

  إذا(!export_fn)
    أرجع CURLE_BAD_FUNCTION_دليل;

  Curl_ssl_scache_تالو(معلومات);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_ڊيگهائي * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  لكل(i = 0; scache && i < scache->peer_ڳڻپ; i++) {
    peer = &scache->peers[i];
    إذا(!peer->ssl_peer_ڪنجي && !peer->hmac_سيٽ)
      استمر; /* skip free entry */
    إذا(!peer->exportable)
      استمر;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_سر(&peer->sessions);
    إذا(n)
      VERBOSE(++npeers);
    طالما(n) {
      بنية Curl_ssl_session *s = Curl_ڳنڍ_elem(n);
      إذا(!peer->hmac_سيٽ) {
        نتيجو = cf_ssl_scache_peer_سيٽ_hmac(peer);
        إذا(نتيجو)
          اذهب ٻاهريون;
      }
      إذا(!curlx_dyn_len(&hbuf)) {
        نتيجو = curlx_dyn_addn(&hbuf, peer->ڪنجي_salt, حجم(peer->ڪنجي_salt));
        إذا(نتيجو)
          اذهب ٻاهريون;
        نتيجو = curlx_dyn_addn(&hbuf, peer->ڪنجي_hmac, حجم(peer->ڪنجي_hmac));
        إذا(نتيجو)
          اذهب ٻاهريون;
      }
      curlx_dyn_reset(&sbuf);
      نتيجو = Curl_ssl_session_pack(معلومات, s, &sbuf);
      إذا(نتيجو)
        اذهب ٻاهريون;

      نتيجو = export_fn(معلومات, userptr, peer->ssl_peer_ڪنجي,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      إذا(نتيجو)
        اذهب ٻاهريون;
      VERBOSE(++ntickets);
      n = Curl_ڳنڍ_ايندڙ(n);
    }
  }
  نتيجو = CURLE_OK;
  CURL_TRC_SSLS(معلومات, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

ٻاهريون:
  Curl_ssl_scache_unlock(معلومات);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  أرجع نتيجو;
}

#جيڪڏهن_پورو /* USE_SSLS_EXPORT */

#جيڪڏهن_پورو /* USE_SSL */
