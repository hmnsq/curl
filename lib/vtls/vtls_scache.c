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

#که_تعریف_شوی USE_SSL

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
  حرف *ssl_peer_کلی;      /* id for peer + relevant TLS configuration */
  حرف *clientcert;
  حرف *کلی_passwd;
  بنية Curl_llist sessions;
  فراغ *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  بینښه حرف کلی_salt[CURL_SHA256_DIGEST_اوږدتیا]; /* for entry export */
  بینښه حرف کلی_hmac[CURL_SHA256_DIGEST_اوږدتیا]; /* for entry export */
  size_t max_sessions;
  اوږد age;                /* a number, the higher the more recent */
  بټ(hmac_سېټ);           /* if key_salt and key_hmac are present */
  بټ(exportable);         /* sessions for this peer can be exported */
};

#تعریف_کړه CURL_SCACHE_MAGIC 0x000e1551

#تعریف_کړه GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ساكن CURLcode cf_ssl_peer_کلی_جمع_کړه_string(بنية dynbuf *buf,
                                           ثابت حرف *name,
                                           ثابت حرف *ارزښت)
{
  أرجع curlx_dyn_addf(buf, ":%s-%zu:%s", name, مزي_اوږدوالی(ارزښت), ارزښت);
}

ساكن CURLcode cf_ssl_peer_کلی_جمع_کړه_لار(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         ثابت حرف *لار,
                                         bool *is_local)
{
  إذا(لار && لار[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#که_تعریف_شوی _WIN32
    حرف *abspath = _fullpath(عدم, لار, 0);
    إذا(abspath) {
      CURLcode پایله = cf_ssl_peer_کلی_جمع_کړه_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      حرر(abspath); /* allocated by CRT, use system free() */
      أرجع پایله;
    }
    *is_local = TRUE;
#وإلاإذا defined(HAVE_REALPATH)
    إذا(لار[0] != '/') {
      حرف *abspath = realpath(لار, عدم);
      إذا(abspath) {
        CURLcode پایله = cf_ssl_peer_کلی_جمع_کړه_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        حرر(abspath); /* allocated by libc, free without memdebug */
        أرجع پایله;
      }
      *is_local = TRUE;
    }
#د_که_پای
    أرجع cf_ssl_peer_کلی_جمع_کړه_string(buf, name, لار);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ssl_peer_کلی_جمع_کړه_hash(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         بنية curl_blob *blob)
{
  CURLcode پایله = CURLE_OK;
  إذا(blob && blob->len) {
    بینښه حرف hash[CURL_SHA256_DIGEST_اوږدتیا];
    size_t i;

    پایله = curlx_dyn_addf(buf, ":%s-", name);
    إذا(پایله)
      اذهب بهر;
    پایله = Curl_sha256it(hash, blob->معلومات, blob->len);
    إذا(پایله)
      اذهب بهر;
    لكل(i = 0; i < CURL_SHA256_DIGEST_اوږدتیا; ++i) {
      پایله = curlx_dyn_addf(buf, "%02x", hash[i]);
      إذا(پایله)
        اذهب بهر;
    }
  }
بهر:
  أرجع پایله;
}

ساكن CURLcode cf_ssl_peer_کلی_جمع_کړه_mtls(بنية dynbuf *buf,
                                         بنية ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode پایله = CURLE_OK;
  إذا(ssl->clientcert && ssl->clientcert[0]) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_لار(buf, "CCERT", ssl->clientcert, is_local);
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->کلی && ssl->کلی[0]) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_لار(buf, "KEY", ssl->کلی, is_local);
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->کلی_blob) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_hash(buf, "KEYBlob", ssl->کلی_blob);
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    پایله = curlx_dyn_addf(buf, ":CT-%zu:", مزي_اوږدوالی(ssl->cert_type));
    لكل(i = 0; !پایله && ssl->cert_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->cert_type[i]);
      پایله = curlx_dyn_addn(buf, &c, 1);
    }
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->کلی_type && ssl->کلی_type[0]) {
    size_t i;
    پایله = curlx_dyn_addf(buf, ":KT-%zu:", مزي_اوږدوالی(ssl->کلی_type));
    لكل(i = 0; !پایله && ssl->کلی_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->کلی_type[i]);
      پایله = curlx_dyn_addn(buf, &c, 1);
    }
  }
بهر:
  أرجع پایله;
}

#تعریف_کړه CURL_SSLS_LOCAL_SUFFIX     ":L"
#تعریف_کړه CURL_SSLS_GLOBAL_SUFFIX    ":G"

ساكن bool cf_ssl_peer_کلی_is_global(ثابت حرف *peer_کلی)
{
  size_t len = peer_کلی ? مزي_اوږدوالی(peer_کلی) : 0;
  أرجع (len > 2) &&
         (peer_کلی[len - 1] == 'G') &&
         (peer_کلی[len - 2] == ':');
}

ساكن CURLcode ssl_peer_کلی_جمع_کړه_transport(بنية dynbuf *buf,
                                           uint8_t transport)
{
  بدل(transport) {
  حالة TRNSPRT_TCP:
    أرجع CURLE_OK;
  حالة TRNSPRT_UDP:
    أرجع curlx_dyn_جمع_کړه(buf, ":UDP");
  حالة TRNSPRT_QUIC:
    أرجع curlx_dyn_جمع_کړه(buf, ":QUIC");
  حالة TRNSPRT_UNIX:
    أرجع curlx_dyn_جمع_کړه(buf, ":UNIX");
  افتراضي:
    أرجع curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ساكن CURLcode ssl_peer_کلی_جمع_کړه_vrfy(بنية dynbuf *buf,
                                      بنية ssl_filter_config *ssl,
                                      ثابت بنية ssl_peer *peer)
{
  CURLcode پایله;

  إذا(!ssl->verifypeer) {
    پایله = curlx_dyn_جمع_کړه(buf, ":NO-VRFY-PEER");
    إذا(پایله)
      أرجع پایله;
  }
  إذا(!ssl->verifyhost) {
    پایله = curlx_dyn_جمع_کړه(buf, ":NO-VRFY-HOST");
    إذا(پایله)
      أرجع پایله;
  }
  إذا(ssl->verifystatus) {
    پایله = curlx_dyn_جمع_کړه(buf, ":VRFY-STATUS");
    إذا(پایله)
      أرجع پایله;
  }
  إذا((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_string(buf, "CHOST", peer->peer->hostname);
    إذا(!پایله)
      پایله = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->پورټ);
    إذا(پایله)
      أرجع پایله;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode ssl_peer_کلی_build(بنية ssl_filter_config *ssl,
                                   ثابت بنية ssl_peer *peer,
                                   ثابت حرف *tls_id,
                                   حرف **ppeer_کلی)
{
  بنية dynbuf buf;
  size_t کلی_len;
  bool is_local = FALSE;
  CURLcode پایله;

  *ppeer_کلی = عدم;
  curlx_dyn_init(&buf, 10 * 1024);

  پایله = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->پورټ);
  إذا(پایله)
    اذهب بهر;
  پایله = ssl_peer_کلی_جمع_کړه_transport(&buf, peer->transport);
  إذا(پایله)
    اذهب بهر;
  پایله = ssl_peer_کلی_جمع_کړه_vrfy(&buf, ssl, peer);
  إذا(پایله)
    اذهب بهر;
  إذا(ssl->نسخه_2 || ssl->نسخه_max) {
    پایله = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->نسخه_2,
                            (ssl->نسخه_max >> 16));
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->ssl_options) {
    پایله = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->cipher_list) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_string(&buf, "CIPHER", ssl->cipher_list);
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->cipher_list13) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_string(&buf, "CIPHER13", ssl->cipher_list13);
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->curves) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_string(&buf, "CURVES", ssl->curves);
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->signature_algorithms) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->verifypeer) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_لار(&buf, "CA", ssl->CAfile, &is_local);
    إذا(!پایله)
      پایله = cf_ssl_peer_کلی_جمع_کړه_لار(&buf, "CApath",
                                        ssl->CApath, &is_local);
    إذا(!پایله && ssl->native_ca_store)
      پایله = curlx_dyn_جمع_کړه(&buf, ":NATIVE-CA");
    إذا(!پایله)
      پایله = cf_ssl_peer_کلی_جمع_کړه_لار(&buf, "CRL", ssl->CRLfile, &is_local);
    إذا(پایله)
      اذهب بهر;
    پایله = cf_ssl_peer_کلی_جمع_کړه_لار(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    إذا(پایله)
      اذهب بهر;
    إذا(ssl->ca_info_blob) {
      پایله = cf_ssl_peer_کلی_جمع_کړه_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      إذا(پایله)
        اذهب بهر;
    }
    إذا(ssl->issuercert_blob) {
      پایله = cf_ssl_peer_کلی_جمع_کړه_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      إذا(پایله)
        اذهب بهر;
    }
  }
  إذا(ssl->cert_blob) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_hash(&buf, "CertBlob", ssl->cert_blob);
    إذا(پایله)
      اذهب بهر;
  }
  إذا(ssl->pinned_کلی && ssl->pinned_کلی[0]) {
    پایله = cf_ssl_peer_کلی_جمع_کړه_string(&buf, "Pinned", ssl->pinned_کلی);
    إذا(پایله)
      اذهب بهر;
  }

  پایله = cf_ssl_peer_کلی_جمع_کړه_mtls(&buf, ssl, &is_local);
  إذا(پایله)
    اذهب بهر;

  إذا(!tls_id || !tls_id[0]) {
    پایله = CURLE_FAILED_INIT;
    اذهب بهر;
  }
  پایله = cf_ssl_peer_کلی_جمع_کړه_string(&buf, "IMPL", tls_id);
  إذا(پایله)
    اذهب بهر;

  پایله = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  إذا(پایله)
    اذهب بهر;

  *ppeer_کلی = curlx_dyn_take(&buf, &کلی_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

بهر:
  curlx_dyn_free(&buf);
  أرجع پایله;
}

CURLcode Curl_ssl_peer_کلی_make(ثابت بنية ssl_peer *peer,
                                بنية ssl_filter_config *sslc,
                                ثابت حرف *tls_id,
                                حرف **ppeer_کلی)
{
  أرجع ssl_peer_کلی_build(sslc, peer, tls_id, ppeer_کلی);
}

بنية Curl_ssl_scache {
  بینښه صحيح magic;
  بنية Curl_ssl_scache_peer *peers;
  size_t peer_شمېر;
  صحيح default_lifetime_secs;
  اوږد age;
#که_تعریف_شوی USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#د_که_پای
  بټ(is_locked);
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
                                  بینښه حرف *quic_tp, size_t quic_tp_len,
                                  بنية Curl_ssl_session **psession)
{
  بنية Curl_ssl_session *s;

  إذا(!sdata || !sdata_len) {
    curlx_free(sdata);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  *psession = عدم;
  s = curlx_calloc(1, حجم(*s));
  إذا(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    أرجع CURLE_بهر_د_حافظه;
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
      أرجع CURLE_بهر_د_حافظه;
    }
  }
  *psession = s;
  أرجع CURLE_OK;
}

فراغ Curl_ssl_session_destroy(بنية Curl_ssl_session *s)
{
  إذا(s) {
    /* if in the list, the list destructor takes care of it */
    إذا(Curl_غوټه_llist(&s->list))
      Curl_غوټه_remove(&s->list);
    وإلا {
      cf_ssl_scache_session_ldestroy(عدم, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(بنية Curl_ssl_session *src,
                              بنية Curl_ssl_session **pdest)
{
  بنية Curl_ssl_session *dest = عدم;
  CURLcode پایله = CURLE_بهر_د_حافظه;

  إذا(!src || !pdest)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  *pdest = عدم;

  dest = curlx_calloc(1, حجم(*dest));
  إذا(!dest)
    اذهب بهر;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  إذا(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    إذا(!dest->sdata)
      اذهب بهر;
    dest->sdata_len = src->sdata_len;
  }
  إذا(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    إذا(!dest->quic_tp)
      اذهب بهر;
    dest->quic_tp_len = src->quic_tp_len;
  }
  إذا(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    إذا(!dest->alpn)
      اذهب بهر;
  }
  پایله = CURLE_OK;

بهر:
  إذا(!پایله)
    *pdest = dest;
  وإلا {
    *pdest = عدم;
    إذا(dest)
      Curl_ssl_session_destroy(dest);
  }
  أرجع پایله;
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
  curlx_safefree(peer->کلی_passwd);
  curlx_safefree(peer->ssl_peer_کلی);
  peer->age = 0;
  peer->hmac_سېټ = FALSE;
}

ساكن فراغ cf_ssl_scache_peer_سېټ_obj(بنية Curl_ssl_scache_peer *peer,
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
  peer->exportable = (!peer->clientcert && !peer->کلی_passwd &&
                      (!peer->ssl_peer_کلی ||
                       cf_ssl_peer_کلی_is_global(peer->ssl_peer_کلی)));
}

ساكن CURLcode cf_ssl_scache_peer_init(بنية Curl_ssl_scache_peer *peer,
                                        ثابت حرف *ssl_peer_کلی,
                                        ثابت حرف *clientcert,
                                        ثابت حرف *کلی_passwd,
                                        ثابت بینښه حرف *salt,
                                        ثابت بینښه حرف *hmac)
{
  CURLcode پایله = CURLE_بهر_د_حافظه;

  DEBUGASSERT(!peer->ssl_peer_کلی);
  إذا(ssl_peer_کلی) {
    peer->ssl_peer_کلی = curlx_strdup(ssl_peer_کلی);
    إذا(!peer->ssl_peer_کلی)
      اذهب بهر;
    peer->hmac_سېټ = FALSE;
  }
  وإلا إذا(salt && hmac) {
    حافظه_نقل(peer->کلی_salt, salt, حجم(peer->کلی_salt));
    حافظه_نقل(peer->کلی_hmac, hmac, حجم(peer->کلی_hmac));
    peer->hmac_سېټ = TRUE;
  }
  وإلا {
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }
  إذا(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    إذا(!peer->clientcert)
      اذهب بهر;
  }
  إذا(کلی_passwd) {
    peer->کلی_passwd = curlx_strdup(کلی_passwd);
    إذا(!peer->کلی_passwd)
      اذهب بهر;
  }
  cf_ssl_cache_peer_update(peer);
  پایله = CURLE_OK;
بهر:
  إذا(پایله)
    cf_ssl_scache_clear_peer(peer);
  أرجع پایله;
}

ساكن فراغ cf_scache_session_remove(بنية Curl_ssl_scache_peer *peer,
                                     بنية Curl_ssl_session *s)
{
  (فراغ)peer;
  DEBUGASSERT(Curl_غوټه_llist(&s->list) == &peer->sessions);
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
  بنية Curl_llist_غوټه *n = Curl_llist_سر(&peer->sessions);
  طالما(n) {
    بنية Curl_ssl_session *s = Curl_غوټه_elem(n);
    n = Curl_غوټه_بل(n);
    إذا(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ساكن فراغ cf_scache_peer_remove_non13(بنية Curl_ssl_scache_peer *peer)
{
  بنية Curl_llist_غوټه *n = Curl_llist_سر(&peer->sessions);
  طالما(n) {
    بنية Curl_ssl_session *s = Curl_غوټه_elem(n);
    n = Curl_غوټه_بل(n);
    إذا(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_فی_peer,
                                بنية Curl_ssl_scache **pscache)
{
  بنية Curl_ssl_scache *scache;
  بنية Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = عدم;
  peers = curlx_calloc(max_peers, حجم(*peers));
  إذا(!peers)
    أرجع CURLE_بهر_د_حافظه;

  scache = curlx_calloc(1, حجم(*scache));
  إذا(!scache) {
    curlx_free(peers);
    أرجع CURLE_بهر_د_حافظه;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_شمېر = max_peers;
  scache->peers = peers;
  scache->age = 1;
  لكل(i = 0; i < scache->peer_شمېر; ++i) {
    scache->peers[i].max_sessions = max_sessions_فی_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#که_تعریف_شوی USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#د_که_پای
  *pscache = scache;
  أرجع CURLE_OK;
}

فراغ Curl_ssl_scache_destroy(بنية Curl_ssl_scache *scache)
{
  إذا(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    لكل(i = 0; i < scache->peer_شمېر; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#که_تعریف_شوی USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#د_که_پای
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
فراغ Curl_ssl_scache_قلف(بنية Curl_easy *معلومات)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  إذا(scache) {
    إذا(CURL_SHARE_ssl_scache(معلومات))
      Curl_share_قلف(معلومات, CURL_قلف_معلومات_SSL_SESSION,
                      CURL_قلف_ACCESS_SINGLE);
#که_تعریف_شوی USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#وإلا
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#د_که_پای
  }
}

/* Unlock shared SSL session data */
فراغ Curl_ssl_scache_unlock(بنية Curl_easy *معلومات)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  إذا(scache) {
#که_تعریف_شوی USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#وإلا
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#د_که_پای
    إذا(CURL_SHARE_ssl_scache(معلومات))
      Curl_share_unlock(معلومات, CURL_قلف_معلومات_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(بنية Curl_easy *معلومات)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  bool locked = FALSE;
  إذا(!scache)
    أرجع FALSE;
#که_تعریف_شوی USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#وإلا
  locked = (bool)scache->is_locked;
#د_که_پای
  أرجع locked;
}

ساكن bool cf_ssl_scache_سمون_auth(بنية Curl_ssl_scache_peer *peer,
                                     بنية ssl_filter_config *conn_config)
{
  إذا(!conn_config) {
    إذا(peer->clientcert || peer->کلی_passwd)
      أرجع FALSE;
    أرجع TRUE;
  }
  وإلا إذا(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    أرجع FALSE;
  إذا(Curl_timestrcmp(peer->کلی_passwd, conn_config->کلی_passwd))
    أرجع FALSE;
  أرجع TRUE;
}

ساكن CURLcode cf_ssl_پیدا_کړه_peer_by_کلی(بنية Curl_easy *معلومات,
                                        بنية Curl_ssl_scache *scache,
                                        ثابت حرف *ssl_peer_کلی,
                                        بنية ssl_filter_config *conn_config,
                                        بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_کلی_len = 0;
  CURLcode پایله = CURLE_OK;

  *ppeer = عدم;
  إذا(!GOOD_SCACHE(scache)) {
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  CURL_TRC_SSLS(معلومات, "find peer slot for %s among %zu slots",
                ssl_peer_کلی, scache->peer_شمېر);

  /* check for entries with known peer_key */
  لكل(i = 0; scache && i < scache->peer_شمېر; i++) {
    إذا(scache->peers[i].ssl_peer_کلی &&
       !strcmp(ssl_peer_کلی, scache->peers[i].ssl_peer_کلی) &&
       cf_ssl_scache_سمون_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      اذهب بهر;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  لكل(i = 0; scache && i < scache->peer_شمېر; i++) {
    إذا(!scache->peers[i].ssl_peer_کلی &&
       scache->peers[i].hmac_سېټ &&
       cf_ssl_scache_سمون_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      بینښه حرف my_hmac[CURL_SHA256_DIGEST_اوږدتیا];
      إذا(!peer_کلی_len) /* we are lazy */
        peer_کلی_len = مزي_اوږدوالی(ssl_peer_کلی);
      پایله = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].کلی_salt,
                           حجم(scache->peers[i].کلی_salt),
                           (ثابت بینښه حرف *)ssl_peer_کلی,
                           peer_کلی_len,
                           my_hmac);
      إذا(پایله)
        اذهب بهر;
      إذا(!memcmp(scache->peers[i].کلی_hmac, my_hmac, حجم(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(معلومات, "peer entry %zu key recovered: %s",
                      i, ssl_peer_کلی);
        scache->peers[i].ssl_peer_کلی = curlx_strdup(ssl_peer_کلی);
        إذا(!scache->peers[i].ssl_peer_کلی) {
          پایله = CURLE_بهر_د_حافظه;
          اذهب بهر;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        اذهب بهر;
      }
    }
  }
  CURL_TRC_SSLS(معلومات, "peer not found for %s", ssl_peer_کلی);
بهر:
  أرجع پایله;
}

ساكن بنية Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  بنية Curl_ssl_scache *scache)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  size_t i;

  /* find empty or oldest peer */
  لكل(i = 0; i < scache->peer_شمېر; ++i) {
    /* free peer entry? */
    إذا(!scache->peers[i].ssl_peer_کلی && !scache->peers[i].hmac_سېټ) {
      peer = &scache->peers[i];
      اكسر;
    }
    /* peer without sessions and obj */
    إذا(!scache->peers[i].sobj &&
       !Curl_llist_شمېر(&scache->peers[i].sessions)) {
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

ساكن CURLcode cf_ssl_جمع_کړه_peer(بنية Curl_easy *معلومات,
                                بنية Curl_ssl_scache *scache,
                                ثابت حرف *ssl_peer_کلی,
                                بنية ssl_filter_config *conn_config,
                                بنية Curl_ssl_scache_peer **ppeer)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode پایله = CURLE_OK;

  *ppeer = عدم;
  إذا(ssl_peer_کلی) {
    پایله = cf_ssl_پیدا_کړه_peer_by_کلی(معلومات, scache, ssl_peer_کلی, conn_config,
                                     &peer);
    إذا(پایله || !scache->peer_شمېر)
      أرجع پایله;
  }

  إذا(peer) {
    *ppeer = peer;
    أرجع CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  إذا(peer) {
    حرف بفر[64];
    ثابت حرف *ccert = conn_config ? conn_config->clientcert : عدم;
    ثابت حرف *kpasswd = conn_config ? conn_config->کلی_passwd : عدم;
    إذا(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(بفر, حجم(بفر),
                     "cert-%p", conn_config->cert_blob->معلومات);
      ccert = بفر; /* data is strduped by cf_ssl_scache_peer_init */
    }
    پایله = cf_ssl_scache_peer_init(peer, ssl_peer_کلی, ccert, kpasswd,
                                     عدم, عدم);
    إذا(پایله)
      اذهب بهر;
    /* all ready */
    *ppeer = peer;
    پایله = CURLE_OK;
  }

بهر:
  إذا(پایله) {
    cf_ssl_scache_clear_peer(peer);
  }
  أرجع پایله;
}

ساكن فراغ cf_scache_peer_جمع_کړه_session(بنية Curl_ssl_scache_peer *peer,
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
    طالما(Curl_llist_شمېر(&peer->sessions) > peer->max_sessions) {
      Curl_غوټه_remove(Curl_llist_سر(&peer->sessions));
    }
  }
}

ساكن CURLcode cf_scache_جمع_کړه_session(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *معلومات,
                                      بنية Curl_ssl_scache *scache,
                                      ثابت حرف *ssl_peer_کلی,
                                      بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode پایله = CURLE_بهر_د_حافظه;
  curl_off_t now = (curl_off_t)وخت(عدم);
  curl_off_t max_lifetime;

  إذا(!scache || !scache->peer_شمېر) {
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

  پایله = cf_ssl_جمع_کړه_peer(معلومات, scache, ssl_peer_کلی, conn_config, &peer);
  إذا(پایله || !peer) {
    CURL_TRC_SSLS(معلومات, "unable to add scache peer: %d", (صحيح)پایله);
    Curl_ssl_session_destroy(s);
    اذهب بهر;
  }

  cf_scache_peer_جمع_کړه_session(peer, s, now);

بهر:
  إذا(پایله) {
    failf(معلومات, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_کلی, (صحيح)پایله);
  }
  وإلا
    CURL_TRC_SSLS(معلومات, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_کلی,
                  (بینښه صحيح)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_شمېر(&peer->sessions) : 0);
  أرجع پایله;
}

CURLcode Curl_ssl_scache_put(بنية Curl_cfilter *cf,
                             بنية Curl_easy *معلومات,
                             ثابت حرف *ssl_peer_کلی,
                             بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode پایله;
  DEBUGASSERT(conn_config);

  إذا(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    أرجع CURLE_OK;
  }

  Curl_ssl_scache_قلف(معلومات);
  پایله = cf_scache_جمع_کړه_session(cf, معلومات, scache, ssl_peer_کلی, s);
  Curl_ssl_scache_unlock(معلومات);
  أرجع پایله;
}

فراغ Curl_ssl_scache_return(بنية Curl_cfilter *cf,
                            بنية Curl_easy *معلومات,
                            ثابت حرف *ssl_peer_کلی,
                            بنية Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  إذا(s && s->ietf_tls_id < 0x304)
    (فراغ)Curl_ssl_scache_put(cf, معلومات, ssl_peer_کلی, s);
  وإلا
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              ثابت حرف *ssl_peer_کلی,
                              بنية Curl_ssl_session **ps)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_llist_غوټه *n;
  بنية Curl_ssl_session *s = عدم;
  CURLcode پایله;

  *ps = عدم;
  إذا(!scache)
    أرجع CURLE_OK;

  Curl_ssl_scache_قلف(معلومات);
  پایله = cf_ssl_پیدا_کړه_peer_by_کلی(معلومات, scache, ssl_peer_کلی, conn_config,
                                   &peer);
  إذا(!پایله && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)وخت(عدم));
    n = Curl_llist_سر(&peer->sessions);
    إذا(n) {
      s = Curl_غوټه_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  إذا(s) {
    *ps = s;
    CURL_TRC_SSLS(معلومات, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_کلی, (بینښه صحيح)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_شمېر(&peer->sessions));
  }
  وإلا {
    CURL_TRC_SSLS(معلومات, "no cached session for %s", ssl_peer_کلی);
  }
  Curl_ssl_scache_unlock(معلومات);
  أرجع پایله;
}

CURLcode Curl_ssl_scache_جمع_کړه_obj(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *معلومات,
                                 ثابت حرف *ssl_peer_کلی,
                                 فراغ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode پایله;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  إذا(!scache) {
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }

  پایله = cf_ssl_جمع_کړه_peer(معلومات, scache, ssl_peer_کلی, conn_config, &peer);
  إذا(پایله || !peer) {
    CURL_TRC_SSLS(معلومات, "unable to add scache peer: %d", (صحيح)پایله);
    اذهب بهر;
  }

  cf_ssl_scache_peer_سېټ_obj(peer, sobj, sobj_dtor_cb);
  sobj = عدم;  /* peer took ownership */

بهر:
  إذا(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  أرجع پایله;
}

فراغ *Curl_ssl_scache_get_obj(بنية Curl_cfilter *cf,
                              بنية Curl_easy *معلومات,
                              ثابت حرف *ssl_peer_کلی)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode پایله;
  فراغ *sobj;

  إذا(!scache)
    أرجع عدم;

  پایله = cf_ssl_پیدا_کړه_peer_by_کلی(معلومات, scache, ssl_peer_کلی, conn_config,
                                   &peer);
  إذا(پایله)
    أرجع عدم;

  sobj = peer ? peer->sobj : عدم;

  CURL_TRC_SSLS(معلومات, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_کلی);
  أرجع sobj;
}

فراغ Curl_ssl_scache_remove_all(بنية Curl_cfilter *cf,
                                بنية Curl_easy *معلومات,
                                ثابت حرف *ssl_peer_کلی)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode پایله;

  (فراغ)cf;
  إذا(!scache)
    أرجع;

  Curl_ssl_scache_قلف(معلومات);
  پایله = cf_ssl_پیدا_کړه_peer_by_کلی(معلومات, scache, ssl_peer_کلی, conn_config,
                                   &peer);
  إذا(!پایله && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(معلومات);
}

#که_تعریف_شوی USE_SSLS_EXPORT

#تعریف_کړه CURL_SSL_TICKET_MAX   (16 * 1024)

ساكن CURLcode cf_ssl_scache_peer_سېټ_hmac(بنية Curl_ssl_scache_peer *peer)
{
  CURLcode پایله;

  DEBUGASSERT(peer);
  إذا(!peer->ssl_peer_کلی)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  پایله = Curl_rand(عدم, peer->کلی_salt, حجم(peer->کلی_salt));
  إذا(پایله)
    أرجع پایله;

  پایله = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->کلی_salt, حجم(peer->کلی_salt),
                       (ثابت بینښه حرف *)peer->ssl_peer_کلی,
                       مزي_اوږدوالی(peer->ssl_peer_کلی),
                       peer->کلی_hmac);
  إذا(!پایله)
    peer->hmac_سېټ = TRUE;
  أرجع پایله;
}

ساكن CURLcode cf_ssl_پیدا_کړه_peer_by_hmac(بنية Curl_ssl_scache *scache,
                                         ثابت بینښه حرف *salt,
                                         ثابت بینښه حرف *hmac,
                                         بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode پایله = CURLE_OK;

  *ppeer = عدم;
  إذا(!GOOD_SCACHE(scache))
    أرجع CURLE_BAD_FUNCTION_دلیل;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  لكل(i = 0; scache && i < scache->peer_شمېر; i++) {
    بنية Curl_ssl_scache_peer *peer = &scache->peers[i];
    إذا(!cf_ssl_scache_سمون_auth(peer, عدم))
      استمر;
    إذا(scache->peers[i].hmac_سېټ &&
       !memcmp(peer->کلی_salt, salt, حجم(peer->کلی_salt)) &&
       !memcmp(peer->کلی_hmac, hmac, حجم(peer->کلی_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      اذهب بهر;
    }
    وإلا إذا(peer->ssl_peer_کلی) {
      بینښه حرف my_hmac[CURL_SHA256_DIGEST_اوږدتیا];
      /* compute hmac for the passed salt */
      پایله = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, حجم(peer->کلی_salt),
                           (ثابت بینښه حرف *)peer->ssl_peer_کلی,
                           مزي_اوږدوالی(peer->ssl_peer_کلی),
                           my_hmac);
      إذا(پایله)
        اذهب بهر;
      إذا(!memcmp(my_hmac, hmac, حجم(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        إذا(!peer->hmac_سېټ) {
          حافظه_نقل(peer->کلی_salt, salt, حجم(peer->کلی_salt));
          حافظه_نقل(peer->کلی_hmac, hmac, حجم(peer->کلی_hmac));
          peer->hmac_سېټ = TRUE;
        }
        *ppeer = peer;
        اذهب بهر;
      }
    }
  }
بهر:
  أرجع پایله;
}

CURLcode Curl_ssl_session_import(بنية Curl_easy *معلومات,
                                 ثابت حرف *ssl_peer_کلی,
                                 ثابت بینښه حرف *shmac, size_t shmac_len,
                                 ثابت فراغ *sdata, size_t sdata_len)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_ssl_session *s = عدم;
  bool have_قلف = FALSE;
  CURLcode پایله;

  إذا(!scache || !sdata || !sdata_len) {
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }
  إذا(!ssl_peer_کلی && (!shmac || !shmac_len)) {
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }

  پایله = Curl_ssl_session_unpack(معلومات, sdata, sdata_len, &s);
  إذا(پایله)
    اذهب بهر;

  Curl_ssl_scache_قلف(معلومات);
  have_قلف = TRUE;

  إذا(ssl_peer_کلی) {
    پایله = cf_ssl_جمع_کړه_peer(معلومات, scache, ssl_peer_کلی, عدم, &peer);
    إذا(پایله)
      اذهب بهر;
  }
  وإلا إذا(shmac_len != (حجم(peer->کلی_salt) + حجم(peer->کلی_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    پایله = CURLE_BAD_FUNCTION_دلیل;
    اذهب بهر;
  }
  وإلا {
    ثابت بینښه حرف *salt = shmac;
    ثابت بینښه حرف *hmac = shmac + حجم(peer->کلی_salt);

    پایله = cf_ssl_پیدا_کړه_peer_by_hmac(scache, salt, hmac, &peer);
    إذا(پایله)
      اذهب بهر;
    إذا(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      إذا(peer) {
        پایله = cf_ssl_scache_peer_init(peer, ssl_peer_کلی, عدم, عدم,
                                         salt, hmac);
        إذا(پایله)
          اذهب بهر;
      }
    }
  }

  إذا(peer) {
    cf_scache_peer_جمع_کړه_session(peer, s, وخت(عدم));
    s = عدم; /* peer is now owner */
    CURL_TRC_SSLS(معلومات, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_کلی ? peer->ssl_peer_کلی : "without key",
                  Curl_llist_شمېر(&peer->sessions));
  }

بهر:
  إذا(scache && have_قلف)
    Curl_ssl_scache_unlock(معلومات);
  Curl_ssl_session_destroy(s);
  أرجع پایله;
}

CURLcode Curl_ssl_session_export(بنية Curl_easy *معلومات,
                                 curl_ssls_export_cb *export_fn,
                                 فراغ *userptr)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(معلومات);
  بنية Curl_ssl_scache_peer *peer;
  بنية dynbuf sbuf, hbuf;
  بنية Curl_llist_غوټه *n;
  size_t i;
  curl_off_t now = وخت(عدم);
  CURLcode پایله = CURLE_OK;
#که_تعریف_شوی CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#د_که_پای

  إذا(!export_fn)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  Curl_ssl_scache_قلف(معلومات);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_اوږدتیا * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  لكل(i = 0; scache && i < scache->peer_شمېر; i++) {
    peer = &scache->peers[i];
    إذا(!peer->ssl_peer_کلی && !peer->hmac_سېټ)
      استمر; /* skip free entry */
    إذا(!peer->exportable)
      استمر;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_سر(&peer->sessions);
    إذا(n)
      VERBOSE(++npeers);
    طالما(n) {
      بنية Curl_ssl_session *s = Curl_غوټه_elem(n);
      إذا(!peer->hmac_سېټ) {
        پایله = cf_ssl_scache_peer_سېټ_hmac(peer);
        إذا(پایله)
          اذهب بهر;
      }
      إذا(!curlx_dyn_len(&hbuf)) {
        پایله = curlx_dyn_addn(&hbuf, peer->کلی_salt, حجم(peer->کلی_salt));
        إذا(پایله)
          اذهب بهر;
        پایله = curlx_dyn_addn(&hbuf, peer->کلی_hmac, حجم(peer->کلی_hmac));
        إذا(پایله)
          اذهب بهر;
      }
      curlx_dyn_reset(&sbuf);
      پایله = Curl_ssl_session_pack(معلومات, s, &sbuf);
      إذا(پایله)
        اذهب بهر;

      پایله = export_fn(معلومات, userptr, peer->ssl_peer_کلی,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      إذا(پایله)
        اذهب بهر;
      VERBOSE(++ntickets);
      n = Curl_غوټه_بل(n);
    }
  }
  پایله = CURLE_OK;
  CURL_TRC_SSLS(معلومات, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

بهر:
  Curl_ssl_scache_unlock(معلومات);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  أرجع پایله;
}

#د_که_پای /* USE_SSLS_EXPORT */

#د_که_پای /* USE_SSL */
