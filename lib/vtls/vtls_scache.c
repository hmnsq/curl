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

#جے_تعریف_شدہ USE_SSL

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
  بنانشانی حرف کلید_salt[CURL_SHA256_DIGEST_لمائی]; /* for entry export */
  بنانشانی حرف کلید_hmac[CURL_SHA256_DIGEST_لمائی]; /* for entry export */
  size_t max_sessions;
  لما age;                /* a number, the higher the more recent */
  ثنائی_ہندسہ(hmac_سیٹ);           /* if key_salt and key_hmac are present */
  ثنائی_ہندسہ(exportable);         /* sessions for this peer can be exported */
};

#تعریف_کرو CURL_SCACHE_MAGIC 0x000e1551

#تعریف_کرو GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ساكن CURLcode cf_ssl_peer_کلید_جوڑو_string(بنية dynbuf *buf,
                                           ثابت حرف *name,
                                           ثابت حرف *قدر)
{
  أرجع curlx_dyn_addf(buf, ":%s-%zu:%s", name, لڑی_لمبائی(قدر), قدر);
}

ساكن CURLcode cf_ssl_peer_کلید_جوڑو_رستہ(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         ثابت حرف *رستہ,
                                         bool *is_local)
{
  إذا(رستہ && رستہ[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#جے_تعریف_شدہ _WIN32
    حرف *abspath = _fullpath(عدم, رستہ, 0);
    إذا(abspath) {
      CURLcode نتیجہ = cf_ssl_peer_کلید_جوڑو_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      حرر(abspath); /* allocated by CRT, use system free() */
      أرجع نتیجہ;
    }
    *is_local = TRUE;
#وإلاإذا defined(HAVE_REALPATH)
    إذا(رستہ[0] != '/') {
      حرف *abspath = realpath(رستہ, عدم);
      إذا(abspath) {
        CURLcode نتیجہ = cf_ssl_peer_کلید_جوڑو_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        حرر(abspath); /* allocated by libc, free without memdebug */
        أرجع نتیجہ;
      }
      *is_local = TRUE;
    }
#جے_انت
    أرجع cf_ssl_peer_کلید_جوڑو_string(buf, name, رستہ);
  }
  أرجع CURLE_OK;
}

ساكن CURLcode cf_ssl_peer_کلید_جوڑو_hash(بنية dynbuf *buf,
                                         ثابت حرف *name,
                                         بنية curl_blob *blob)
{
  CURLcode نتیجہ = CURLE_OK;
  إذا(blob && blob->len) {
    بنانشانی حرف hash[CURL_SHA256_DIGEST_لمائی];
    size_t i;

    نتیجہ = curlx_dyn_addf(buf, ":%s-", name);
    إذا(نتیجہ)
      اذهب باہرلا;
    نتیجہ = Curl_sha256it(hash, blob->انکڑے, blob->len);
    إذا(نتیجہ)
      اذهب باہرلا;
    لكل(i = 0; i < CURL_SHA256_DIGEST_لمائی; ++i) {
      نتیجہ = curlx_dyn_addf(buf, "%02x", hash[i]);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
  }
باہرلا:
  أرجع نتیجہ;
}

ساكن CURLcode cf_ssl_peer_کلید_جوڑو_mtls(بنية dynbuf *buf,
                                         بنية ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode نتیجہ = CURLE_OK;
  إذا(ssl->clientcert && ssl->clientcert[0]) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_رستہ(buf, "CCERT", ssl->clientcert, is_local);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->کلید && ssl->کلید[0]) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_رستہ(buf, "KEY", ssl->کلید, is_local);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->کلید_blob) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_hash(buf, "KEYBlob", ssl->کلید_blob);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    نتیجہ = curlx_dyn_addf(buf, ":CT-%zu:", لڑی_لمبائی(ssl->cert_type));
    لكل(i = 0; !نتیجہ && ssl->cert_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->cert_type[i]);
      نتیجہ = curlx_dyn_addn(buf, &c, 1);
    }
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->کلید_type && ssl->کلید_type[0]) {
    size_t i;
    نتیجہ = curlx_dyn_addf(buf, ":KT-%zu:", لڑی_لمبائی(ssl->کلید_type));
    لكل(i = 0; !نتیجہ && ssl->کلید_type[i]; i++) {
      حرف c = Curl_raw_toupper(ssl->کلید_type[i]);
      نتیجہ = curlx_dyn_addn(buf, &c, 1);
    }
  }
باہرلا:
  أرجع نتیجہ;
}

#تعریف_کرو CURL_SSLS_LOCAL_SUFFIX     ":L"
#تعریف_کرو CURL_SSLS_GLOBAL_SUFFIX    ":G"

ساكن bool cf_ssl_peer_کلید_is_global(ثابت حرف *peer_کلید)
{
  size_t len = peer_کلید ? لڑی_لمبائی(peer_کلید) : 0;
  أرجع (len > 2) &&
         (peer_کلید[len - 1] == 'G') &&
         (peer_کلید[len - 2] == ':');
}

ساكن CURLcode ssl_peer_کلید_جوڑو_transport(بنية dynbuf *buf,
                                           uint8_t transport)
{
  بدل(transport) {
  حالة TRNSPRT_TCP:
    أرجع CURLE_OK;
  حالة TRNSPRT_UDP:
    أرجع curlx_dyn_جوڑو(buf, ":UDP");
  حالة TRNSPRT_QUIC:
    أرجع curlx_dyn_جوڑو(buf, ":QUIC");
  حالة TRNSPRT_UNIX:
    أرجع curlx_dyn_جوڑو(buf, ":UNIX");
  افتراضي:
    أرجع curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ساكن CURLcode ssl_peer_کلید_جوڑو_vrfy(بنية dynbuf *buf,
                                      بنية ssl_filter_config *ssl,
                                      ثابت بنية ssl_peer *peer)
{
  CURLcode نتیجہ;

  إذا(!ssl->verifypeer) {
    نتیجہ = curlx_dyn_جوڑو(buf, ":NO-VRFY-PEER");
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  إذا(!ssl->verifyhost) {
    نتیجہ = curlx_dyn_جوڑو(buf, ":NO-VRFY-HOST");
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  إذا(ssl->verifystatus) {
    نتیجہ = curlx_dyn_جوڑو(buf, ":VRFY-STATUS");
    إذا(نتیجہ)
      أرجع نتیجہ;
  }
  إذا((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(buf, "CHOST", peer->peer->hostname);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->پورٹ);
    إذا(نتیجہ)
      أرجع نتیجہ;
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
  CURLcode نتیجہ;

  *ppeer_کلید = عدم;
  curlx_dyn_init(&buf, 10 * 1024);

  نتیجہ = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->پورٹ);
  إذا(نتیجہ)
    اذهب باہرلا;
  نتیجہ = ssl_peer_کلید_جوڑو_transport(&buf, peer->transport);
  إذا(نتیجہ)
    اذهب باہرلا;
  نتیجہ = ssl_peer_کلید_جوڑو_vrfy(&buf, ssl, peer);
  إذا(نتیجہ)
    اذهب باہرلا;
  إذا(ssl->نسخہ_2 || ssl->نسخہ_max) {
    نتیجہ = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->نسخہ_2,
                            (ssl->نسخہ_max >> 16));
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->ssl_options) {
    نتیجہ = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->cipher_list) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "CIPHER", ssl->cipher_list);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->cipher_list13) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "CIPHER13", ssl->cipher_list13);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->curves) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "CURVES", ssl->curves);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->signature_algorithms) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->verifypeer) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_رستہ(&buf, "CA", ssl->CAfile, &is_local);
    إذا(!نتیجہ)
      نتیجہ = cf_ssl_peer_کلید_جوڑو_رستہ(&buf, "CApath",
                                        ssl->CApath, &is_local);
    إذا(!نتیجہ && ssl->native_ca_store)
      نتیجہ = curlx_dyn_جوڑو(&buf, ":NATIVE-CA");
    إذا(!نتیجہ)
      نتیجہ = cf_ssl_peer_کلید_جوڑو_رستہ(&buf, "CRL", ssl->CRLfile, &is_local);
    إذا(نتیجہ)
      اذهب باہرلا;
    نتیجہ = cf_ssl_peer_کلید_جوڑو_رستہ(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    إذا(نتیجہ)
      اذهب باہرلا;
    إذا(ssl->ca_info_blob) {
      نتیجہ = cf_ssl_peer_کلید_جوڑو_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
    إذا(ssl->issuercert_blob) {
      نتیجہ = cf_ssl_peer_کلید_جوڑو_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      إذا(نتیجہ)
        اذهب باہرلا;
    }
  }
  إذا(ssl->cert_blob) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_hash(&buf, "CertBlob", ssl->cert_blob);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  إذا(ssl->pinned_کلید && ssl->pinned_کلید[0]) {
    نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "Pinned", ssl->pinned_کلید);
    إذا(نتیجہ)
      اذهب باہرلا;
  }

  نتیجہ = cf_ssl_peer_کلید_جوڑو_mtls(&buf, ssl, &is_local);
  إذا(نتیجہ)
    اذهب باہرلا;

  إذا(!tls_id || !tls_id[0]) {
    نتیجہ = CURLE_FAILED_INIT;
    اذهب باہرلا;
  }
  نتیجہ = cf_ssl_peer_کلید_جوڑو_string(&buf, "IMPL", tls_id);
  إذا(نتیجہ)
    اذهب باہرلا;

  نتیجہ = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  إذا(نتیجہ)
    اذهب باہرلا;

  *ppeer_کلید = curlx_dyn_take(&buf, &کلید_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

باہرلا:
  curlx_dyn_free(&buf);
  أرجع نتیجہ;
}

CURLcode Curl_ssl_peer_کلید_make(ثابت بنية ssl_peer *peer,
                                بنية ssl_filter_config *sslc,
                                ثابت حرف *tls_id,
                                حرف **ppeer_کلید)
{
  أرجع ssl_peer_کلید_build(sslc, peer, tls_id, ppeer_کلید);
}

بنية Curl_ssl_scache {
  بنانشانی صحيح magic;
  بنية Curl_ssl_scache_peer *peers;
  size_t peer_گنتی;
  صحيح default_lifetime_secs;
  لما age;
#جے_تعریف_شدہ USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#جے_انت
  ثنائی_ہندسہ(is_locked);
};

ساكن بنية Curl_ssl_scache *cf_ssl_scache_get(بنية Curl_easy *انکڑے)
{
  بنية Curl_ssl_scache *scache = عدم;
  /* If a share is present, its ssl_scache has preference over the multi */
  إذا(انکڑے->share && انکڑے->share->ssl_scache)
    scache = انکڑے->share->ssl_scache;
  وإلا إذا(انکڑے->multi && انکڑے->multi->ssl_scache)
    scache = انکڑے->multi->ssl_scache;
  إذا(scache && !GOOD_SCACHE(scache)) {
    failf(انکڑے, "transfer would use an invalid scache at %p, denied",
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
                                  بنانشانی حرف *quic_tp, size_t quic_tp_len,
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
    أرجع CURLE_باہرلا_دا_حافظہ;
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
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
  }
  *psession = s;
  أرجع CURLE_OK;
}

فراغ Curl_ssl_session_destroy(بنية Curl_ssl_session *s)
{
  إذا(s) {
    /* if in the list, the list destructor takes care of it */
    إذا(Curl_گرہ_llist(&s->list))
      Curl_گرہ_remove(&s->list);
    وإلا {
      cf_ssl_scache_session_ldestroy(عدم, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(بنية Curl_ssl_session *src,
                              بنية Curl_ssl_session **pdest)
{
  بنية Curl_ssl_session *dest = عدم;
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  إذا(!src || !pdest)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  *pdest = عدم;

  dest = curlx_calloc(1, حجم(*dest));
  إذا(!dest)
    اذهب باہرلا;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  إذا(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    إذا(!dest->sdata)
      اذهب باہرلا;
    dest->sdata_len = src->sdata_len;
  }
  إذا(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    إذا(!dest->quic_tp)
      اذهب باہرلا;
    dest->quic_tp_len = src->quic_tp_len;
  }
  إذا(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    إذا(!dest->alpn)
      اذهب باہرلا;
  }
  نتیجہ = CURLE_OK;

باہرلا:
  إذا(!نتیجہ)
    *pdest = dest;
  وإلا {
    *pdest = عدم;
    إذا(dest)
      Curl_ssl_session_destroy(dest);
  }
  أرجع نتیجہ;
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
  peer->hmac_سیٹ = FALSE;
}

ساكن فراغ cf_ssl_scache_peer_سیٹ_obj(بنية Curl_ssl_scache_peer *peer,
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
                                        ثابت بنانشانی حرف *salt,
                                        ثابت بنانشانی حرف *hmac)
{
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;

  DEBUGASSERT(!peer->ssl_peer_کلید);
  إذا(ssl_peer_کلید) {
    peer->ssl_peer_کلید = curlx_strdup(ssl_peer_کلید);
    إذا(!peer->ssl_peer_کلید)
      اذهب باہرلا;
    peer->hmac_سیٹ = FALSE;
  }
  وإلا إذا(salt && hmac) {
    حافظہ_نقل(peer->کلید_salt, salt, حجم(peer->کلید_salt));
    حافظہ_نقل(peer->کلید_hmac, hmac, حجم(peer->کلید_hmac));
    peer->hmac_سیٹ = TRUE;
  }
  وإلا {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب باہرلا;
  }
  إذا(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    إذا(!peer->clientcert)
      اذهب باہرلا;
  }
  إذا(کلید_passwd) {
    peer->کلید_passwd = curlx_strdup(کلید_passwd);
    إذا(!peer->کلید_passwd)
      اذهب باہرلا;
  }
  cf_ssl_cache_peer_update(peer);
  نتیجہ = CURLE_OK;
باہرلا:
  إذا(نتیجہ)
    cf_ssl_scache_clear_peer(peer);
  أرجع نتیجہ;
}

ساكن فراغ cf_scache_session_remove(بنية Curl_ssl_scache_peer *peer,
                                     بنية Curl_ssl_session *s)
{
  (فراغ)peer;
  DEBUGASSERT(Curl_گرہ_llist(&s->list) == &peer->sessions);
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
  بنية Curl_llist_گرہ *n = Curl_llist_سر(&peer->sessions);
  طالما(n) {
    بنية Curl_ssl_session *s = Curl_گرہ_elem(n);
    n = Curl_گرہ_اگلا(n);
    إذا(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ساكن فراغ cf_scache_peer_remove_non13(بنية Curl_ssl_scache_peer *peer)
{
  بنية Curl_llist_گرہ *n = Curl_llist_سر(&peer->sessions);
  طالما(n) {
    بنية Curl_ssl_session *s = Curl_گرہ_elem(n);
    n = Curl_گرہ_اگلا(n);
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
    أرجع CURLE_باہرلا_دا_حافظہ;

  scache = curlx_calloc(1, حجم(*scache));
  إذا(!scache) {
    curlx_free(peers);
    أرجع CURLE_باہرلا_دا_حافظہ;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_گنتی = max_peers;
  scache->peers = peers;
  scache->age = 1;
  لكل(i = 0; i < scache->peer_گنتی; ++i) {
    scache->peers[i].max_sessions = max_sessions_فی_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#جے_تعریف_شدہ USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#جے_انت
  *pscache = scache;
  أرجع CURLE_OK;
}

فراغ Curl_ssl_scache_destroy(بنية Curl_ssl_scache *scache)
{
  إذا(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    لكل(i = 0; i < scache->peer_گنتی; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#جے_تعریف_شدہ USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#جے_انت
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(بنية Curl_cfilter *cf, بنية Curl_easy *انکڑے)
{
  إذا(cf_ssl_scache_get(انکڑے)) {
    بنية ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    أرجع conn_config ? conn_config->cache_session : FALSE;
  }
  أرجع FALSE;
}

/* Lock shared SSL session data */
فراغ Curl_ssl_scache_جندرا(بنية Curl_easy *انکڑے)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  إذا(scache) {
    إذا(CURL_SHARE_ssl_scache(انکڑے))
      Curl_share_جندرا(انکڑے, CURL_جندرا_انکڑے_SSL_SESSION,
                      CURL_جندرا_ACCESS_SINGLE);
#جے_تعریف_شدہ USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#وإلا
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#جے_انت
  }
}

/* Unlock shared SSL session data */
فراغ Curl_ssl_scache_unlock(بنية Curl_easy *انکڑے)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  إذا(scache) {
#جے_تعریف_شدہ USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#وإلا
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#جے_انت
    إذا(CURL_SHARE_ssl_scache(انکڑے))
      Curl_share_unlock(انکڑے, CURL_جندرا_انکڑے_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(بنية Curl_easy *انکڑے)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  bool locked = FALSE;
  إذا(!scache)
    أرجع FALSE;
#جے_تعریف_شدہ USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#وإلا
  locked = (bool)scache->is_locked;
#جے_انت
  أرجع locked;
}

ساكن bool cf_ssl_scache_میل_auth(بنية Curl_ssl_scache_peer *peer,
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

ساكن CURLcode cf_ssl_لبھو_peer_by_کلید(بنية Curl_easy *انکڑے,
                                        بنية Curl_ssl_scache *scache,
                                        ثابت حرف *ssl_peer_کلید,
                                        بنية ssl_filter_config *conn_config,
                                        بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_کلید_len = 0;
  CURLcode نتیجہ = CURLE_OK;

  *ppeer = عدم;
  إذا(!GOOD_SCACHE(scache)) {
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }

  CURL_TRC_SSLS(انکڑے, "find peer slot for %s among %zu slots",
                ssl_peer_کلید, scache->peer_گنتی);

  /* check for entries with known peer_key */
  لكل(i = 0; scache && i < scache->peer_گنتی; i++) {
    إذا(scache->peers[i].ssl_peer_کلید &&
       !strcmp(ssl_peer_کلید, scache->peers[i].ssl_peer_کلید) &&
       cf_ssl_scache_میل_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      اذهب باہرلا;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  لكل(i = 0; scache && i < scache->peer_گنتی; i++) {
    إذا(!scache->peers[i].ssl_peer_کلید &&
       scache->peers[i].hmac_سیٹ &&
       cf_ssl_scache_میل_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      بنانشانی حرف my_hmac[CURL_SHA256_DIGEST_لمائی];
      إذا(!peer_کلید_len) /* we are lazy */
        peer_کلید_len = لڑی_لمبائی(ssl_peer_کلید);
      نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].کلید_salt,
                           حجم(scache->peers[i].کلید_salt),
                           (ثابت بنانشانی حرف *)ssl_peer_کلید,
                           peer_کلید_len,
                           my_hmac);
      إذا(نتیجہ)
        اذهب باہرلا;
      إذا(!memcmp(scache->peers[i].کلید_hmac, my_hmac, حجم(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(انکڑے, "peer entry %zu key recovered: %s",
                      i, ssl_peer_کلید);
        scache->peers[i].ssl_peer_کلید = curlx_strdup(ssl_peer_کلید);
        إذا(!scache->peers[i].ssl_peer_کلید) {
          نتیجہ = CURLE_باہرلا_دا_حافظہ;
          اذهب باہرلا;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        اذهب باہرلا;
      }
    }
  }
  CURL_TRC_SSLS(انکڑے, "peer not found for %s", ssl_peer_کلید);
باہرلا:
  أرجع نتیجہ;
}

ساكن بنية Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  بنية Curl_ssl_scache *scache)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  size_t i;

  /* find empty or oldest peer */
  لكل(i = 0; i < scache->peer_گنتی; ++i) {
    /* free peer entry? */
    إذا(!scache->peers[i].ssl_peer_کلید && !scache->peers[i].hmac_سیٹ) {
      peer = &scache->peers[i];
      اكسر;
    }
    /* peer without sessions and obj */
    إذا(!scache->peers[i].sobj &&
       !Curl_llist_گنتی(&scache->peers[i].sessions)) {
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

ساكن CURLcode cf_ssl_جوڑو_peer(بنية Curl_easy *انکڑے,
                                بنية Curl_ssl_scache *scache,
                                ثابت حرف *ssl_peer_کلید,
                                بنية ssl_filter_config *conn_config,
                                بنية Curl_ssl_scache_peer **ppeer)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتیجہ = CURLE_OK;

  *ppeer = عدم;
  إذا(ssl_peer_کلید) {
    نتیجہ = cf_ssl_لبھو_peer_by_کلید(انکڑے, scache, ssl_peer_کلید, conn_config,
                                     &peer);
    إذا(نتیجہ || !scache->peer_گنتی)
      أرجع نتیجہ;
  }

  إذا(peer) {
    *ppeer = peer;
    أرجع CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  إذا(peer) {
    حرف بفر[64];
    ثابت حرف *ccert = conn_config ? conn_config->clientcert : عدم;
    ثابت حرف *kpasswd = conn_config ? conn_config->کلید_passwd : عدم;
    إذا(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(بفر, حجم(بفر),
                     "cert-%p", conn_config->cert_blob->انکڑے);
      ccert = بفر; /* data is strduped by cf_ssl_scache_peer_init */
    }
    نتیجہ = cf_ssl_scache_peer_init(peer, ssl_peer_کلید, ccert, kpasswd,
                                     عدم, عدم);
    إذا(نتیجہ)
      اذهب باہرلا;
    /* all ready */
    *ppeer = peer;
    نتیجہ = CURLE_OK;
  }

باہرلا:
  إذا(نتیجہ) {
    cf_ssl_scache_clear_peer(peer);
  }
  أرجع نتیجہ;
}

ساكن فراغ cf_scache_peer_جوڑو_session(بنية Curl_ssl_scache_peer *peer,
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
    طالما(Curl_llist_گنتی(&peer->sessions) > peer->max_sessions) {
      Curl_گرہ_remove(Curl_llist_سر(&peer->sessions));
    }
  }
}

ساكن CURLcode cf_scache_جوڑو_session(بنية Curl_cfilter *cf,
                                      بنية Curl_easy *انکڑے,
                                      بنية Curl_ssl_scache *scache,
                                      ثابت حرف *ssl_peer_کلید,
                                      بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ = CURLE_باہرلا_دا_حافظہ;
  curl_off_t now = (curl_off_t)ویلا(عدم);
  curl_off_t max_lifetime;

  إذا(!scache || !scache->peer_گنتی) {
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
    CURL_TRC_SSLS(انکڑے, "add, session already expired");
    Curl_ssl_session_destroy(s);
    أرجع CURLE_OK;
  }

  نتیجہ = cf_ssl_جوڑو_peer(انکڑے, scache, ssl_peer_کلید, conn_config, &peer);
  إذا(نتیجہ || !peer) {
    CURL_TRC_SSLS(انکڑے, "unable to add scache peer: %d", (صحيح)نتیجہ);
    Curl_ssl_session_destroy(s);
    اذهب باہرلا;
  }

  cf_scache_peer_جوڑو_session(peer, s, now);

باہرلا:
  إذا(نتیجہ) {
    failf(انکڑے, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_کلید, (صحيح)نتیجہ);
  }
  وإلا
    CURL_TRC_SSLS(انکڑے, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_کلید,
                  (بنانشانی صحيح)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_گنتی(&peer->sessions) : 0);
  أرجع نتیجہ;
}

CURLcode Curl_ssl_scache_put(بنية Curl_cfilter *cf,
                             بنية Curl_easy *انکڑے,
                             ثابت حرف *ssl_peer_کلید,
                             بنية Curl_ssl_session *s)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode نتیجہ;
  DEBUGASSERT(conn_config);

  إذا(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    أرجع CURLE_OK;
  }

  Curl_ssl_scache_جندرا(انکڑے);
  نتیجہ = cf_scache_جوڑو_session(cf, انکڑے, scache, ssl_peer_کلید, s);
  Curl_ssl_scache_unlock(انکڑے);
  أرجع نتیجہ;
}

فراغ Curl_ssl_scache_return(بنية Curl_cfilter *cf,
                            بنية Curl_easy *انکڑے,
                            ثابت حرف *ssl_peer_کلید,
                            بنية Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  إذا(s && s->ietf_tls_id < 0x304)
    (فراغ)Curl_ssl_scache_put(cf, انکڑے, ssl_peer_کلید, s);
  وإلا
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے,
                              ثابت حرف *ssl_peer_کلید,
                              بنية Curl_ssl_session **ps)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_llist_گرہ *n;
  بنية Curl_ssl_session *s = عدم;
  CURLcode نتیجہ;

  *ps = عدم;
  إذا(!scache)
    أرجع CURLE_OK;

  Curl_ssl_scache_جندرا(انکڑے);
  نتیجہ = cf_ssl_لبھو_peer_by_کلید(انکڑے, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  إذا(!نتیجہ && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)ویلا(عدم));
    n = Curl_llist_سر(&peer->sessions);
    إذا(n) {
      s = Curl_گرہ_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  إذا(s) {
    *ps = s;
    CURL_TRC_SSLS(انکڑے, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_کلید, (بنانشانی صحيح)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_گنتی(&peer->sessions));
  }
  وإلا {
    CURL_TRC_SSLS(انکڑے, "no cached session for %s", ssl_peer_کلید);
  }
  Curl_ssl_scache_unlock(انکڑے);
  أرجع نتیجہ;
}

CURLcode Curl_ssl_scache_جوڑو_obj(بنية Curl_cfilter *cf,
                                 بنية Curl_easy *انکڑے,
                                 ثابت حرف *ssl_peer_کلید,
                                 فراغ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتیجہ;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  إذا(!scache) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب باہرلا;
  }

  نتیجہ = cf_ssl_جوڑو_peer(انکڑے, scache, ssl_peer_کلید, conn_config, &peer);
  إذا(نتیجہ || !peer) {
    CURL_TRC_SSLS(انکڑے, "unable to add scache peer: %d", (صحيح)نتیجہ);
    اذهب باہرلا;
  }

  cf_ssl_scache_peer_سیٹ_obj(peer, sobj, sobj_dtor_cb);
  sobj = عدم;  /* peer took ownership */

باہرلا:
  إذا(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  أرجع نتیجہ;
}

فراغ *Curl_ssl_scache_get_obj(بنية Curl_cfilter *cf,
                              بنية Curl_easy *انکڑے,
                              ثابت حرف *ssl_peer_کلید)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتیجہ;
  فراغ *sobj;

  إذا(!scache)
    أرجع عدم;

  نتیجہ = cf_ssl_لبھو_peer_by_کلید(انکڑے, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  إذا(نتیجہ)
    أرجع عدم;

  sobj = peer ? peer->sobj : عدم;

  CURL_TRC_SSLS(انکڑے, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_کلید);
  أرجع sobj;
}

فراغ Curl_ssl_scache_remove_all(بنية Curl_cfilter *cf,
                                بنية Curl_easy *انکڑے,
                                ثابت حرف *ssl_peer_کلید)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  بنية ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  بنية Curl_ssl_scache_peer *peer = عدم;
  CURLcode نتیجہ;

  (فراغ)cf;
  إذا(!scache)
    أرجع;

  Curl_ssl_scache_جندرا(انکڑے);
  نتیجہ = cf_ssl_لبھو_peer_by_کلید(انکڑے, scache, ssl_peer_کلید, conn_config,
                                   &peer);
  إذا(!نتیجہ && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(انکڑے);
}

#جے_تعریف_شدہ USE_SSLS_EXPORT

#تعریف_کرو CURL_SSL_TICKET_MAX   (16 * 1024)

ساكن CURLcode cf_ssl_scache_peer_سیٹ_hmac(بنية Curl_ssl_scache_peer *peer)
{
  CURLcode نتیجہ;

  DEBUGASSERT(peer);
  إذا(!peer->ssl_peer_کلید)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  نتیجہ = Curl_rand(عدم, peer->کلید_salt, حجم(peer->کلید_salt));
  إذا(نتیجہ)
    أرجع نتیجہ;

  نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->کلید_salt, حجم(peer->کلید_salt),
                       (ثابت بنانشانی حرف *)peer->ssl_peer_کلید,
                       لڑی_لمبائی(peer->ssl_peer_کلید),
                       peer->کلید_hmac);
  إذا(!نتیجہ)
    peer->hmac_سیٹ = TRUE;
  أرجع نتیجہ;
}

ساكن CURLcode cf_ssl_لبھو_peer_by_hmac(بنية Curl_ssl_scache *scache,
                                         ثابت بنانشانی حرف *salt,
                                         ثابت بنانشانی حرف *hmac,
                                         بنية Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode نتیجہ = CURLE_OK;

  *ppeer = عدم;
  إذا(!GOOD_SCACHE(scache))
    أرجع CURLE_BAD_FUNCTION_دلیل;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  لكل(i = 0; scache && i < scache->peer_گنتی; i++) {
    بنية Curl_ssl_scache_peer *peer = &scache->peers[i];
    إذا(!cf_ssl_scache_میل_auth(peer, عدم))
      استمر;
    إذا(scache->peers[i].hmac_سیٹ &&
       !memcmp(peer->کلید_salt, salt, حجم(peer->کلید_salt)) &&
       !memcmp(peer->کلید_hmac, hmac, حجم(peer->کلید_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      اذهب باہرلا;
    }
    وإلا إذا(peer->ssl_peer_کلید) {
      بنانشانی حرف my_hmac[CURL_SHA256_DIGEST_لمائی];
      /* compute hmac for the passed salt */
      نتیجہ = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, حجم(peer->کلید_salt),
                           (ثابت بنانشانی حرف *)peer->ssl_peer_کلید,
                           لڑی_لمبائی(peer->ssl_peer_کلید),
                           my_hmac);
      إذا(نتیجہ)
        اذهب باہرلا;
      إذا(!memcmp(my_hmac, hmac, حجم(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        إذا(!peer->hmac_سیٹ) {
          حافظہ_نقل(peer->کلید_salt, salt, حجم(peer->کلید_salt));
          حافظہ_نقل(peer->کلید_hmac, hmac, حجم(peer->کلید_hmac));
          peer->hmac_سیٹ = TRUE;
        }
        *ppeer = peer;
        اذهب باہرلا;
      }
    }
  }
باہرلا:
  أرجع نتیجہ;
}

CURLcode Curl_ssl_session_import(بنية Curl_easy *انکڑے,
                                 ثابت حرف *ssl_peer_کلید,
                                 ثابت بنانشانی حرف *shmac, size_t shmac_len,
                                 ثابت فراغ *sdata, size_t sdata_len)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  بنية Curl_ssl_scache_peer *peer = عدم;
  بنية Curl_ssl_session *s = عدم;
  bool have_جندرا = FALSE;
  CURLcode نتیجہ;

  إذا(!scache || !sdata || !sdata_len) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب باہرلا;
  }
  إذا(!ssl_peer_کلید && (!shmac || !shmac_len)) {
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب باہرلا;
  }

  نتیجہ = Curl_ssl_session_unpack(انکڑے, sdata, sdata_len, &s);
  إذا(نتیجہ)
    اذهب باہرلا;

  Curl_ssl_scache_جندرا(انکڑے);
  have_جندرا = TRUE;

  إذا(ssl_peer_کلید) {
    نتیجہ = cf_ssl_جوڑو_peer(انکڑے, scache, ssl_peer_کلید, عدم, &peer);
    إذا(نتیجہ)
      اذهب باہرلا;
  }
  وإلا إذا(shmac_len != (حجم(peer->کلید_salt) + حجم(peer->کلید_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    نتیجہ = CURLE_BAD_FUNCTION_دلیل;
    اذهب باہرلا;
  }
  وإلا {
    ثابت بنانشانی حرف *salt = shmac;
    ثابت بنانشانی حرف *hmac = shmac + حجم(peer->کلید_salt);

    نتیجہ = cf_ssl_لبھو_peer_by_hmac(scache, salt, hmac, &peer);
    إذا(نتیجہ)
      اذهب باہرلا;
    إذا(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      إذا(peer) {
        نتیجہ = cf_ssl_scache_peer_init(peer, ssl_peer_کلید, عدم, عدم,
                                         salt, hmac);
        إذا(نتیجہ)
          اذهب باہرلا;
      }
    }
  }

  إذا(peer) {
    cf_scache_peer_جوڑو_session(peer, s, ویلا(عدم));
    s = عدم; /* peer is now owner */
    CURL_TRC_SSLS(انکڑے, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_کلید ? peer->ssl_peer_کلید : "without key",
                  Curl_llist_گنتی(&peer->sessions));
  }

باہرلا:
  إذا(scache && have_جندرا)
    Curl_ssl_scache_unlock(انکڑے);
  Curl_ssl_session_destroy(s);
  أرجع نتیجہ;
}

CURLcode Curl_ssl_session_export(بنية Curl_easy *انکڑے,
                                 curl_ssls_export_cb *export_fn,
                                 فراغ *userptr)
{
  بنية Curl_ssl_scache *scache = cf_ssl_scache_get(انکڑے);
  بنية Curl_ssl_scache_peer *peer;
  بنية dynbuf sbuf, hbuf;
  بنية Curl_llist_گرہ *n;
  size_t i;
  curl_off_t now = ویلا(عدم);
  CURLcode نتیجہ = CURLE_OK;
#جے_تعریف_شدہ CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#جے_انت

  إذا(!export_fn)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  Curl_ssl_scache_جندرا(انکڑے);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_لمائی * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  لكل(i = 0; scache && i < scache->peer_گنتی; i++) {
    peer = &scache->peers[i];
    إذا(!peer->ssl_peer_کلید && !peer->hmac_سیٹ)
      استمر; /* skip free entry */
    إذا(!peer->exportable)
      استمر;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_سر(&peer->sessions);
    إذا(n)
      VERBOSE(++npeers);
    طالما(n) {
      بنية Curl_ssl_session *s = Curl_گرہ_elem(n);
      إذا(!peer->hmac_سیٹ) {
        نتیجہ = cf_ssl_scache_peer_سیٹ_hmac(peer);
        إذا(نتیجہ)
          اذهب باہرلا;
      }
      إذا(!curlx_dyn_len(&hbuf)) {
        نتیجہ = curlx_dyn_addn(&hbuf, peer->کلید_salt, حجم(peer->کلید_salt));
        إذا(نتیجہ)
          اذهب باہرلا;
        نتیجہ = curlx_dyn_addn(&hbuf, peer->کلید_hmac, حجم(peer->کلید_hmac));
        إذا(نتیجہ)
          اذهب باہرلا;
      }
      curlx_dyn_reset(&sbuf);
      نتیجہ = Curl_ssl_session_pack(انکڑے, s, &sbuf);
      إذا(نتیجہ)
        اذهب باہرلا;

      نتیجہ = export_fn(انکڑے, userptr, peer->ssl_peer_کلید,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      إذا(نتیجہ)
        اذهب باہرلا;
      VERBOSE(++ntickets);
      n = Curl_گرہ_اگلا(n);
    }
  }
  نتیجہ = CURLE_OK;
  CURL_TRC_SSLS(انکڑے, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

باہرلا:
  Curl_ssl_scache_unlock(انکڑے);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  أرجع نتیجہ;
}

#جے_انت /* USE_SSLS_EXPORT */

#جے_انت /* USE_SSL */
