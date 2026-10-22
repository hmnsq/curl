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
#समावेश "curl_setup.h"

#अगर_घोषित USE_SSL

#समावेश "urldata.h"
#समावेश "cfilters.h"

#समावेश "vtls/vtls.h" /* generic SSL protos etc */
#समावेश "vtls/vtls_int.h"
#समावेश "vtls/vtls_scache.h"
#समावेश "vtls/vtls_spack.h"

#समावेश "strcase.h"
#समावेश "url.h"
#समावेश "llist.h"
#समावेश "curl_share.h"
#समावेश "curl_threads.h"
#समावेश "curl_trc.h"
#समावेश "curl_sha256.h"
#समावेश "rand.h"
#समावेश "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
काष्ठा Curl_ssl_scache_peer {
  अक्षर *ssl_peer_कुंजी;      /* id for peer + relevant TLS configuration */
  अक्षर *clientcert;
  अक्षर *कुंजी_passwd;
  काष्ठा Curl_llist sessions;
  व्योम *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  अचिन्हित अक्षर कुंजी_salt[CURL_SHA256_DIGEST_लंबाई]; /* for entry export */
  अचिन्हित अक्षर कुंजी_hmac[CURL_SHA256_DIGEST_लंबाई]; /* for entry export */
  माप_प्रकार max_sessions;
  दीर्घ age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#घोषणा CURL_SCACHE_MAGIC 0x000e1551

#घोषणा GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
अटल CURLcode cf_ssl_peer_कुंजी_add_string(काष्ठा dynbuf *buf,
                                           स्थिर अक्षर *name,
                                           स्थिर अक्षर *मूल्य)
{
  वापस curlx_dyn_addf(buf, ":%s-%zu:%s", name, म_माप(मूल्य), मूल्य);
}

अटल CURLcode cf_ssl_peer_कुंजी_add_path(काष्ठा dynbuf *buf,
                                         स्थिर अक्षर *name,
                                         स्थिर अक्षर *path,
                                         bool *is_local)
{
  अगर(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#अगर_घोषित _WIN32
    अक्षर *abspath = _fullpath(शून्य, path, 0);
    अगर(abspath) {
      CURLcode परिणाम = cf_ssl_peer_कुंजी_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      मुक्त(abspath); /* allocated by CRT, use system free() */
      वापस परिणाम;
    }
    *is_local = TRUE;
#या_अगर defined(HAVE_REALPATH)
    अगर(path[0] != '/') {
      अक्षर *abspath = realpath(path, शून्य);
      अगर(abspath) {
        CURLcode परिणाम = cf_ssl_peer_कुंजी_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        मुक्त(abspath); /* allocated by libc, free without memdebug */
        वापस परिणाम;
      }
      *is_local = TRUE;
    }
#पूर्ण_अगर
    वापस cf_ssl_peer_कुंजी_add_string(buf, name, path);
  }
  वापस CURLE_OK;
}

अटल CURLcode cf_ssl_peer_कुंजी_add_hash(काष्ठा dynbuf *buf,
                                         स्थिर अक्षर *name,
                                         काष्ठा curl_blob *blob)
{
  CURLcode परिणाम = CURLE_OK;
  अगर(blob && blob->len) {
    अचिन्हित अक्षर hash[CURL_SHA256_DIGEST_लंबाई];
    माप_प्रकार i;

    परिणाम = curlx_dyn_addf(buf, ":%s-", name);
    अगर(परिणाम)
      जाओ out;
    परिणाम = Curl_sha256it(hash, blob->तथ्य, blob->len);
    अगर(परिणाम)
      जाओ out;
    क्रम(i = 0; i < CURL_SHA256_DIGEST_लंबाई; ++i) {
      परिणाम = curlx_dyn_addf(buf, "%02x", hash[i]);
      अगर(परिणाम)
        जाओ out;
    }
  }
out:
  वापस परिणाम;
}

अटल CURLcode cf_ssl_peer_कुंजी_add_mtls(काष्ठा dynbuf *buf,
                                         काष्ठा ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode परिणाम = CURLE_OK;
  अगर(ssl->clientcert && ssl->clientcert[0]) {
    परिणाम = cf_ssl_peer_कुंजी_add_path(buf, "CCERT", ssl->clientcert, is_local);
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->कुंजी && ssl->कुंजी[0]) {
    परिणाम = cf_ssl_peer_कुंजी_add_path(buf, "KEY", ssl->कुंजी, is_local);
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->कुंजी_blob) {
    परिणाम = cf_ssl_peer_कुंजी_add_hash(buf, "KEYBlob", ssl->कुंजी_blob);
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->cert_type && ssl->cert_type[0]) {
    माप_प्रकार i;
    परिणाम = curlx_dyn_addf(buf, ":CT-%zu:", म_माप(ssl->cert_type));
    क्रम(i = 0; !परिणाम && ssl->cert_type[i]; i++) {
      अक्षर c = Curl_raw_toupper(ssl->cert_type[i]);
      परिणाम = curlx_dyn_addn(buf, &c, 1);
    }
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->कुंजी_type && ssl->कुंजी_type[0]) {
    माप_प्रकार i;
    परिणाम = curlx_dyn_addf(buf, ":KT-%zu:", म_माप(ssl->कुंजी_type));
    क्रम(i = 0; !परिणाम && ssl->कुंजी_type[i]; i++) {
      अक्षर c = Curl_raw_toupper(ssl->कुंजी_type[i]);
      परिणाम = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  वापस परिणाम;
}

#घोषणा CURL_SSLS_LOCAL_SUFFIX     ":L"
#घोषणा CURL_SSLS_GLOBAL_SUFFIX    ":G"

अटल bool cf_ssl_peer_कुंजी_is_global(स्थिर अक्षर *peer_कुंजी)
{
  माप_प्रकार len = peer_कुंजी ? म_माप(peer_कुंजी) : 0;
  वापस (len > 2) &&
         (peer_कुंजी[len - 1] == 'G') &&
         (peer_कुंजी[len - 2] == ':');
}

अटल CURLcode ssl_peer_कुंजी_add_transport(काष्ठा dynbuf *buf,
                                           uint8_t transport)
{
  चयन(transport) {
  हाल TRNSPRT_TCP:
    वापस CURLE_OK;
  हाल TRNSPRT_UDP:
    वापस curlx_dyn_add(buf, ":UDP");
  हाल TRNSPRT_QUIC:
    वापस curlx_dyn_add(buf, ":QUIC");
  हाल TRNSPRT_UNIX:
    वापस curlx_dyn_add(buf, ":UNIX");
  शेष:
    वापस curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

अटल CURLcode ssl_peer_कुंजी_add_vrfy(काष्ठा dynbuf *buf,
                                      काष्ठा ssl_filter_config *ssl,
                                      स्थिर काष्ठा ssl_peer *peer)
{
  CURLcode परिणाम;

  अगर(!ssl->verifypeer) {
    परिणाम = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    अगर(परिणाम)
      वापस परिणाम;
  }
  अगर(!ssl->verifyhost) {
    परिणाम = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    अगर(परिणाम)
      वापस परिणाम;
  }
  अगर(ssl->verifystatus) {
    परिणाम = curlx_dyn_add(buf, ":VRFY-STATUS");
    अगर(परिणाम)
      वापस परिणाम;
  }
  अगर((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    परिणाम = cf_ssl_peer_कुंजी_add_string(buf, "CHOST", peer->peer->hostname);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    अगर(परिणाम)
      वापस परिणाम;
  }
  वापस CURLE_OK;
}

अटल CURLcode ssl_peer_कुंजी_build(काष्ठा ssl_filter_config *ssl,
                                   स्थिर काष्ठा ssl_peer *peer,
                                   स्थिर अक्षर *tls_id,
                                   अक्षर **ppeer_कुंजी)
{
  काष्ठा dynbuf buf;
  माप_प्रकार कुंजी_len;
  bool is_local = FALSE;
  CURLcode परिणाम;

  *ppeer_कुंजी = शून्य;
  curlx_dyn_init(&buf, 10 * 1024);

  परिणाम = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  अगर(परिणाम)
    जाओ out;
  परिणाम = ssl_peer_कुंजी_add_transport(&buf, peer->transport);
  अगर(परिणाम)
    जाओ out;
  परिणाम = ssl_peer_कुंजी_add_vrfy(&buf, ssl, peer);
  अगर(परिणाम)
    जाओ out;
  अगर(ssl->version || ssl->version_max) {
    परिणाम = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->ssl_options) {
    परिणाम = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->cipher_list) {
    परिणाम = cf_ssl_peer_कुंजी_add_string(&buf, "CIPHER", ssl->cipher_list);
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->cipher_list13) {
    परिणाम = cf_ssl_peer_कुंजी_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->curves) {
    परिणाम = cf_ssl_peer_कुंजी_add_string(&buf, "CURVES", ssl->curves);
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->signature_algorithms) {
    परिणाम = cf_ssl_peer_कुंजी_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->verifypeer) {
    परिणाम = cf_ssl_peer_कुंजी_add_path(&buf, "CA", ssl->CAfile, &is_local);
    अगर(!परिणाम)
      परिणाम = cf_ssl_peer_कुंजी_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    अगर(!परिणाम && ssl->native_ca_store)
      परिणाम = curlx_dyn_add(&buf, ":NATIVE-CA");
    अगर(!परिणाम)
      परिणाम = cf_ssl_peer_कुंजी_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    अगर(परिणाम)
      जाओ out;
    परिणाम = cf_ssl_peer_कुंजी_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    अगर(परिणाम)
      जाओ out;
    अगर(ssl->ca_info_blob) {
      परिणाम = cf_ssl_peer_कुंजी_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      अगर(परिणाम)
        जाओ out;
    }
    अगर(ssl->issuercert_blob) {
      परिणाम = cf_ssl_peer_कुंजी_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      अगर(परिणाम)
        जाओ out;
    }
  }
  अगर(ssl->cert_blob) {
    परिणाम = cf_ssl_peer_कुंजी_add_hash(&buf, "CertBlob", ssl->cert_blob);
    अगर(परिणाम)
      जाओ out;
  }
  अगर(ssl->pinned_कुंजी && ssl->pinned_कुंजी[0]) {
    परिणाम = cf_ssl_peer_कुंजी_add_string(&buf, "Pinned", ssl->pinned_कुंजी);
    अगर(परिणाम)
      जाओ out;
  }

  परिणाम = cf_ssl_peer_कुंजी_add_mtls(&buf, ssl, &is_local);
  अगर(परिणाम)
    जाओ out;

  अगर(!tls_id || !tls_id[0]) {
    परिणाम = CURLE_FAILED_INIT;
    जाओ out;
  }
  परिणाम = cf_ssl_peer_कुंजी_add_string(&buf, "IMPL", tls_id);
  अगर(परिणाम)
    जाओ out;

  परिणाम = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  अगर(परिणाम)
    जाओ out;

  *ppeer_कुंजी = curlx_dyn_take(&buf, &कुंजी_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  वापस परिणाम;
}

CURLcode Curl_ssl_peer_कुंजी_make(स्थिर काष्ठा ssl_peer *peer,
                                काष्ठा ssl_filter_config *sslc,
                                स्थिर अक्षर *tls_id,
                                अक्षर **ppeer_कुंजी)
{
  वापस ssl_peer_कुंजी_build(sslc, peer, tls_id, ppeer_कुंजी);
}

काष्ठा Curl_ssl_scache {
  अचिन्हित पूर्णांक magic;
  काष्ठा Curl_ssl_scache_peer *peers;
  माप_प्रकार peer_गिनती;
  पूर्णांक default_lifetime_secs;
  दीर्घ age;
#अगर_घोषित USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#पूर्ण_अगर
  BIT(is_locked);
};

अटल काष्ठा Curl_ssl_scache *cf_ssl_scache_get(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_ssl_scache *scache = शून्य;
  /* If a share is present, its ssl_scache has preference over the multi */
  अगर(तथ्य->share && तथ्य->share->ssl_scache)
    scache = तथ्य->share->ssl_scache;
  अन्यथा अगर(तथ्य->multi && तथ्य->multi->ssl_scache)
    scache = तथ्य->multi->ssl_scache;
  अगर(scache && !GOOD_SCACHE(scache)) {
    failf(तथ्य, "transfer would use an invalid scache at %p, denied",
          (व्योम *)scache);
    DEBUGASSERT(0);
    वापस शून्य;
  }
  वापस scache;
}

अटल व्योम cf_ssl_scache_session_ldestroy(व्योम *udata, व्योम *obj)
{
  काष्ठा Curl_ssl_session *s = obj;
  (व्योम)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(व्योम *sdata, माप_प्रकार sdata_len,
                                 पूर्णांक ietf_tls_id, स्थिर अक्षर *alpn,
                                 curl_off_t valid_until, माप_प्रकार earlydata_max,
                                 काष्ठा Curl_ssl_session **psession)
{
  वापस Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  शून्य, 0, psession);
}

CURLcode Curl_ssl_session_create2(व्योम *sdata, माप_प्रकार sdata_len,
                                  पूर्णांक ietf_tls_id, स्थिर अक्षर *alpn,
                                  curl_off_t valid_until, माप_प्रकार earlydata_max,
                                  अचिन्हित अक्षर *quic_tp, माप_प्रकार quic_tp_len,
                                  काष्ठा Curl_ssl_session **psession)
{
  काष्ठा Curl_ssl_session *s;

  अगर(!sdata || !sdata_len) {
    curlx_free(sdata);
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = शून्य;
  s = curlx_calloc(1, माप(*s));
  अगर(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    वापस CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  अगर(alpn) {
    s->alpn = curlx_strdup(alpn);
    अगर(!s->alpn) {
      cf_ssl_scache_session_ldestroy(शून्य, s);
      वापस CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  वापस CURLE_OK;
}

व्योम Curl_ssl_session_destroy(काष्ठा Curl_ssl_session *s)
{
  अगर(s) {
    /* if in the list, the list destructor takes care of it */
    अगर(Curl_गाँठ_llist(&s->list))
      Curl_गाँठ_remove(&s->list);
    अन्यथा {
      cf_ssl_scache_session_ldestroy(शून्य, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(काष्ठा Curl_ssl_session *src,
                              काष्ठा Curl_ssl_session **pdest)
{
  काष्ठा Curl_ssl_session *dest = शून्य;
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  अगर(!src || !pdest)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = शून्य;

  dest = curlx_calloc(1, माप(*dest));
  अगर(!dest)
    जाओ out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  अगर(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    अगर(!dest->sdata)
      जाओ out;
    dest->sdata_len = src->sdata_len;
  }
  अगर(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    अगर(!dest->quic_tp)
      जाओ out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  अगर(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    अगर(!dest->alpn)
      जाओ out;
  }
  परिणाम = CURLE_OK;

out:
  अगर(!परिणाम)
    *pdest = dest;
  अन्यथा {
    *pdest = शून्य;
    अगर(dest)
      Curl_ssl_session_destroy(dest);
  }
  वापस परिणाम;
}

अटल व्योम cf_ssl_scache_clear_peer(काष्ठा Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, शून्य);
  अगर(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    अगर(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = शून्य;
  }
  peer->sobj_free = शून्य;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->कुंजी_passwd);
  curlx_safefree(peer->ssl_peer_कुंजी);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

अटल व्योम cf_ssl_scache_peer_set_obj(काष्ठा Curl_ssl_scache_peer *peer,
                                       व्योम *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  अगर(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

अटल व्योम cf_ssl_cache_peer_update(काष्ठा Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->कुंजी_passwd &&
                      (!peer->ssl_peer_कुंजी ||
                       cf_ssl_peer_कुंजी_is_global(peer->ssl_peer_कुंजी)));
}

अटल CURLcode cf_ssl_scache_peer_init(काष्ठा Curl_ssl_scache_peer *peer,
                                        स्थिर अक्षर *ssl_peer_कुंजी,
                                        स्थिर अक्षर *clientcert,
                                        स्थिर अक्षर *कुंजी_passwd,
                                        स्थिर अचिन्हित अक्षर *salt,
                                        स्थिर अचिन्हित अक्षर *hmac)
{
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_कुंजी);
  अगर(ssl_peer_कुंजी) {
    peer->ssl_peer_कुंजी = curlx_strdup(ssl_peer_कुंजी);
    अगर(!peer->ssl_peer_कुंजी)
      जाओ out;
    peer->hmac_set = FALSE;
  }
  अन्यथा अगर(salt && hmac) {
    स_नकल(peer->कुंजी_salt, salt, माप(peer->कुंजी_salt));
    स_नकल(peer->कुंजी_hmac, hmac, माप(peer->कुंजी_hmac));
    peer->hmac_set = TRUE;
  }
  अन्यथा {
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }
  अगर(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    अगर(!peer->clientcert)
      जाओ out;
  }
  अगर(कुंजी_passwd) {
    peer->कुंजी_passwd = curlx_strdup(कुंजी_passwd);
    अगर(!peer->कुंजी_passwd)
      जाओ out;
  }
  cf_ssl_cache_peer_update(peer);
  परिणाम = CURLE_OK;
out:
  अगर(परिणाम)
    cf_ssl_scache_clear_peer(peer);
  वापस परिणाम;
}

अटल व्योम cf_scache_session_remove(काष्ठा Curl_ssl_scache_peer *peer,
                                     काष्ठा Curl_ssl_session *s)
{
  (व्योम)peer;
  DEBUGASSERT(Curl_गाँठ_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

अटल bool cf_scache_session_expired(काष्ठा Curl_ssl_session *s,
                                      curl_off_t now)
{
  वापस (s->valid_until > 0) && (s->valid_until < now);
}

अटल व्योम cf_scache_peer_remove_expired(काष्ठा Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  काष्ठा Curl_llist_गाँठ *n = Curl_llist_शीर्ष(&peer->sessions);
  जबतक(n) {
    काष्ठा Curl_ssl_session *s = Curl_गाँठ_elem(n);
    n = Curl_गाँठ_अग्र(n);
    अगर(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

अटल व्योम cf_scache_peer_remove_non13(काष्ठा Curl_ssl_scache_peer *peer)
{
  काष्ठा Curl_llist_गाँठ *n = Curl_llist_शीर्ष(&peer->sessions);
  जबतक(n) {
    काष्ठा Curl_ssl_session *s = Curl_गाँठ_elem(n);
    n = Curl_गाँठ_अग्र(n);
    अगर(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(माप_प्रकार max_peers,
                                माप_प्रकार max_sessions_per_peer,
                                काष्ठा Curl_ssl_scache **pscache)
{
  काष्ठा Curl_ssl_scache *scache;
  काष्ठा Curl_ssl_scache_peer *peers;
  माप_प्रकार i;

  *pscache = शून्य;
  peers = curlx_calloc(max_peers, माप(*peers));
  अगर(!peers)
    वापस CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, माप(*scache));
  अगर(!scache) {
    curlx_free(peers);
    वापस CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_गिनती = max_peers;
  scache->peers = peers;
  scache->age = 1;
  क्रम(i = 0; i < scache->peer_गिनती; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#अगर_घोषित USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#पूर्ण_अगर
  *pscache = scache;
  वापस CURLE_OK;
}

व्योम Curl_ssl_scache_destroy(काष्ठा Curl_ssl_scache *scache)
{
  अगर(GOOD_SCACHE(scache)) {
    माप_प्रकार i;
    scache->magic = 0;
    क्रम(i = 0; i < scache->peer_गिनती; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#अगर_घोषित USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#पूर्ण_अगर
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(काष्ठा Curl_cfilter *cf, काष्ठा Curl_easy *तथ्य)
{
  अगर(cf_ssl_scache_get(तथ्य)) {
    काष्ठा ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    वापस conn_config ? conn_config->cache_session : FALSE;
  }
  वापस FALSE;
}

/* Lock shared SSL session data */
व्योम Curl_ssl_scache_lock(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  अगर(scache) {
    अगर(CURL_SHARE_ssl_scache(तथ्य))
      Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#अगर_घोषित USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#अन्यथा
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#पूर्ण_अगर
  }
}

/* Unlock shared SSL session data */
व्योम Curl_ssl_scache_unlock(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  अगर(scache) {
#अगर_घोषित USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#अन्यथा
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#पूर्ण_अगर
    अगर(CURL_SHARE_ssl_scache(तथ्य))
      Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  bool locked = FALSE;
  अगर(!scache)
    वापस FALSE;
#अगर_घोषित USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#अन्यथा
  locked = (bool)scache->is_locked;
#पूर्ण_अगर
  वापस locked;
}

अटल bool cf_ssl_scache_match_auth(काष्ठा Curl_ssl_scache_peer *peer,
                                     काष्ठा ssl_filter_config *conn_config)
{
  अगर(!conn_config) {
    अगर(peer->clientcert || peer->कुंजी_passwd)
      वापस FALSE;
    वापस TRUE;
  }
  अन्यथा अगर(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    वापस FALSE;
  अगर(Curl_timestrcmp(peer->कुंजी_passwd, conn_config->कुंजी_passwd))
    वापस FALSE;
  वापस TRUE;
}

अटल CURLcode cf_ssl_find_peer_by_कुंजी(काष्ठा Curl_easy *तथ्य,
                                        काष्ठा Curl_ssl_scache *scache,
                                        स्थिर अक्षर *ssl_peer_कुंजी,
                                        काष्ठा ssl_filter_config *conn_config,
                                        काष्ठा Curl_ssl_scache_peer **ppeer)
{
  माप_प्रकार i, peer_कुंजी_len = 0;
  CURLcode परिणाम = CURLE_OK;

  *ppeer = शून्य;
  अगर(!GOOD_SCACHE(scache)) {
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(तथ्य, "find peer slot for %s among %zu slots",
                ssl_peer_कुंजी, scache->peer_गिनती);

  /* check for entries with known peer_key */
  क्रम(i = 0; scache && i < scache->peer_गिनती; i++) {
    अगर(scache->peers[i].ssl_peer_कुंजी &&
       !म_भेद(ssl_peer_कुंजी, scache->peers[i].ssl_peer_कुंजी) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      जाओ out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  क्रम(i = 0; scache && i < scache->peer_गिनती; i++) {
    अगर(!scache->peers[i].ssl_peer_कुंजी &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      अचिन्हित अक्षर my_hmac[CURL_SHA256_DIGEST_लंबाई];
      अगर(!peer_कुंजी_len) /* we are lazy */
        peer_कुंजी_len = म_माप(ssl_peer_कुंजी);
      परिणाम = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].कुंजी_salt,
                           माप(scache->peers[i].कुंजी_salt),
                           (स्थिर अचिन्हित अक्षर *)ssl_peer_कुंजी,
                           peer_कुंजी_len,
                           my_hmac);
      अगर(परिणाम)
        जाओ out;
      अगर(!स_भेद(scache->peers[i].कुंजी_hmac, my_hmac, माप(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(तथ्य, "peer entry %zu key recovered: %s",
                      i, ssl_peer_कुंजी);
        scache->peers[i].ssl_peer_कुंजी = curlx_strdup(ssl_peer_कुंजी);
        अगर(!scache->peers[i].ssl_peer_कुंजी) {
          परिणाम = CURLE_OUT_OF_MEMORY;
          जाओ out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        जाओ out;
      }
    }
  }
  CURL_TRC_SSLS(तथ्य, "peer not found for %s", ssl_peer_कुंजी);
out:
  वापस परिणाम;
}

अटल काष्ठा Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  काष्ठा Curl_ssl_scache *scache)
{
  काष्ठा Curl_ssl_scache_peer *peer = शून्य;
  माप_प्रकार i;

  /* find empty or oldest peer */
  क्रम(i = 0; i < scache->peer_गिनती; ++i) {
    /* free peer entry? */
    अगर(!scache->peers[i].ssl_peer_कुंजी && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      अवरोध;
    }
    /* peer without sessions and obj */
    अगर(!scache->peers[i].sobj &&
       !Curl_llist_गिनती(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      अवरोध;
    }
    /* remember "oldest" peer */
    अगर(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  अगर(peer)
    cf_ssl_scache_clear_peer(peer);
  वापस peer;
}

अटल CURLcode cf_ssl_add_peer(काष्ठा Curl_easy *तथ्य,
                                काष्ठा Curl_ssl_scache *scache,
                                स्थिर अक्षर *ssl_peer_कुंजी,
                                काष्ठा ssl_filter_config *conn_config,
                                काष्ठा Curl_ssl_scache_peer **ppeer)
{
  काष्ठा Curl_ssl_scache_peer *peer = शून्य;
  CURLcode परिणाम = CURLE_OK;

  *ppeer = शून्य;
  अगर(ssl_peer_कुंजी) {
    परिणाम = cf_ssl_find_peer_by_कुंजी(तथ्य, scache, ssl_peer_कुंजी, conn_config,
                                     &peer);
    अगर(परिणाम || !scache->peer_गिनती)
      वापस परिणाम;
  }

  अगर(peer) {
    *ppeer = peer;
    वापस CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  अगर(peer) {
    अक्षर बफर[64];
    स्थिर अक्षर *ccert = conn_config ? conn_config->clientcert : शून्य;
    स्थिर अक्षर *kpasswd = conn_config ? conn_config->कुंजी_passwd : शून्य;
    अगर(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(बफर, माप(बफर),
                     "cert-%p", conn_config->cert_blob->तथ्य);
      ccert = बफर; /* data is strduped by cf_ssl_scache_peer_init */
    }
    परिणाम = cf_ssl_scache_peer_init(peer, ssl_peer_कुंजी, ccert, kpasswd,
                                     शून्य, शून्य);
    अगर(परिणाम)
      जाओ out;
    /* all ready */
    *ppeer = peer;
    परिणाम = CURLE_OK;
  }

out:
  अगर(परिणाम) {
    cf_ssl_scache_clear_peer(peer);
  }
  वापस परिणाम;
}

अटल व्योम cf_scache_peer_add_session(काष्ठा Curl_ssl_scache_peer *peer,
                                       काष्ठा Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  अगर(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, शून्य);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  अन्यथा {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    जबतक(Curl_llist_गिनती(&peer->sessions) > peer->max_sessions) {
      Curl_गाँठ_remove(Curl_llist_शीर्ष(&peer->sessions));
    }
  }
}

अटल CURLcode cf_scache_add_session(काष्ठा Curl_cfilter *cf,
                                      काष्ठा Curl_easy *तथ्य,
                                      काष्ठा Curl_ssl_scache *scache,
                                      स्थिर अक्षर *ssl_peer_कुंजी,
                                      काष्ठा Curl_ssl_session *s)
{
  काष्ठा Curl_ssl_scache_peer *peer = शून्य;
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)समय(शून्य);
  curl_off_t max_lifetime;

  अगर(!scache || !scache->peer_गिनती) {
    Curl_ssl_session_destroy(s);
    वापस CURLE_OK;
  }

  अगर(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  अगर(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  अगर(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(तथ्य, "add, session already expired");
    Curl_ssl_session_destroy(s);
    वापस CURLE_OK;
  }

  परिणाम = cf_ssl_add_peer(तथ्य, scache, ssl_peer_कुंजी, conn_config, &peer);
  अगर(परिणाम || !peer) {
    CURL_TRC_SSLS(तथ्य, "unable to add scache peer: %d", (पूर्णांक)परिणाम);
    Curl_ssl_session_destroy(s);
    जाओ out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  अगर(परिणाम) {
    failf(तथ्य, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_कुंजी, (पूर्णांक)परिणाम);
  }
  अन्यथा
    CURL_TRC_SSLS(तथ्य, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_कुंजी,
                  (अचिन्हित पूर्णांक)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_गिनती(&peer->sessions) : 0);
  वापस परिणाम;
}

CURLcode Curl_ssl_scache_put(काष्ठा Curl_cfilter *cf,
                             काष्ठा Curl_easy *तथ्य,
                             स्थिर अक्षर *ssl_peer_कुंजी,
                             काष्ठा Curl_ssl_session *s)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode परिणाम;
  DEBUGASSERT(conn_config);

  अगर(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    वापस CURLE_OK;
  }

  Curl_ssl_scache_lock(तथ्य);
  परिणाम = cf_scache_add_session(cf, तथ्य, scache, ssl_peer_कुंजी, s);
  Curl_ssl_scache_unlock(तथ्य);
  वापस परिणाम;
}

व्योम Curl_ssl_scache_return(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य,
                            स्थिर अक्षर *ssl_peer_कुंजी,
                            काष्ठा Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  अगर(s && s->ietf_tls_id < 0x304)
    (व्योम)Curl_ssl_scache_put(cf, तथ्य, ssl_peer_कुंजी, s);
  अन्यथा
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *ssl_peer_कुंजी,
                              काष्ठा Curl_ssl_session **ps)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा Curl_ssl_scache_peer *peer = शून्य;
  काष्ठा Curl_llist_गाँठ *n;
  काष्ठा Curl_ssl_session *s = शून्य;
  CURLcode परिणाम;

  *ps = शून्य;
  अगर(!scache)
    वापस CURLE_OK;

  Curl_ssl_scache_lock(तथ्य);
  परिणाम = cf_ssl_find_peer_by_कुंजी(तथ्य, scache, ssl_peer_कुंजी, conn_config,
                                   &peer);
  अगर(!परिणाम && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)समय(शून्य));
    n = Curl_llist_शीर्ष(&peer->sessions);
    अगर(n) {
      s = Curl_गाँठ_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  अगर(s) {
    *ps = s;
    CURL_TRC_SSLS(तथ्य, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_कुंजी, (अचिन्हित पूर्णांक)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_गिनती(&peer->sessions));
  }
  अन्यथा {
    CURL_TRC_SSLS(तथ्य, "no cached session for %s", ssl_peer_कुंजी);
  }
  Curl_ssl_scache_unlock(तथ्य);
  वापस परिणाम;
}

CURLcode Curl_ssl_scache_add_obj(काष्ठा Curl_cfilter *cf,
                                 काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *ssl_peer_कुंजी,
                                 व्योम *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा Curl_ssl_scache_peer *peer = शून्य;
  CURLcode परिणाम;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  अगर(!scache) {
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }

  परिणाम = cf_ssl_add_peer(तथ्य, scache, ssl_peer_कुंजी, conn_config, &peer);
  अगर(परिणाम || !peer) {
    CURL_TRC_SSLS(तथ्य, "unable to add scache peer: %d", (पूर्णांक)परिणाम);
    जाओ out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = शून्य;  /* peer took ownership */

out:
  अगर(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  वापस परिणाम;
}

व्योम *Curl_ssl_scache_get_obj(काष्ठा Curl_cfilter *cf,
                              काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *ssl_peer_कुंजी)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा Curl_ssl_scache_peer *peer = शून्य;
  CURLcode परिणाम;
  व्योम *sobj;

  अगर(!scache)
    वापस शून्य;

  परिणाम = cf_ssl_find_peer_by_कुंजी(तथ्य, scache, ssl_peer_कुंजी, conn_config,
                                   &peer);
  अगर(परिणाम)
    वापस शून्य;

  sobj = peer ? peer->sobj : शून्य;

  CURL_TRC_SSLS(तथ्य, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_कुंजी);
  वापस sobj;
}

व्योम Curl_ssl_scache_remove_all(काष्ठा Curl_cfilter *cf,
                                काष्ठा Curl_easy *तथ्य,
                                स्थिर अक्षर *ssl_peer_कुंजी)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  काष्ठा ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  काष्ठा Curl_ssl_scache_peer *peer = शून्य;
  CURLcode परिणाम;

  (व्योम)cf;
  अगर(!scache)
    वापस;

  Curl_ssl_scache_lock(तथ्य);
  परिणाम = cf_ssl_find_peer_by_कुंजी(तथ्य, scache, ssl_peer_कुंजी, conn_config,
                                   &peer);
  अगर(!परिणाम && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(तथ्य);
}

#अगर_घोषित USE_SSLS_EXPORT

#घोषणा CURL_SSL_TICKET_MAX   (16 * 1024)

अटल CURLcode cf_ssl_scache_peer_set_hmac(काष्ठा Curl_ssl_scache_peer *peer)
{
  CURLcode परिणाम;

  DEBUGASSERT(peer);
  अगर(!peer->ssl_peer_कुंजी)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  परिणाम = Curl_rand(शून्य, peer->कुंजी_salt, माप(peer->कुंजी_salt));
  अगर(परिणाम)
    वापस परिणाम;

  परिणाम = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->कुंजी_salt, माप(peer->कुंजी_salt),
                       (स्थिर अचिन्हित अक्षर *)peer->ssl_peer_कुंजी,
                       म_माप(peer->ssl_peer_कुंजी),
                       peer->कुंजी_hmac);
  अगर(!परिणाम)
    peer->hmac_set = TRUE;
  वापस परिणाम;
}

अटल CURLcode cf_ssl_find_peer_by_hmac(काष्ठा Curl_ssl_scache *scache,
                                         स्थिर अचिन्हित अक्षर *salt,
                                         स्थिर अचिन्हित अक्षर *hmac,
                                         काष्ठा Curl_ssl_scache_peer **ppeer)
{
  माप_प्रकार i;
  CURLcode परिणाम = CURLE_OK;

  *ppeer = शून्य;
  अगर(!GOOD_SCACHE(scache))
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  क्रम(i = 0; scache && i < scache->peer_गिनती; i++) {
    काष्ठा Curl_ssl_scache_peer *peer = &scache->peers[i];
    अगर(!cf_ssl_scache_match_auth(peer, शून्य))
      जारी;
    अगर(scache->peers[i].hmac_set &&
       !स_भेद(peer->कुंजी_salt, salt, माप(peer->कुंजी_salt)) &&
       !स_भेद(peer->कुंजी_hmac, hmac, माप(peer->कुंजी_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      जाओ out;
    }
    अन्यथा अगर(peer->ssl_peer_कुंजी) {
      अचिन्हित अक्षर my_hmac[CURL_SHA256_DIGEST_लंबाई];
      /* compute hmac for the passed salt */
      परिणाम = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, माप(peer->कुंजी_salt),
                           (स्थिर अचिन्हित अक्षर *)peer->ssl_peer_कुंजी,
                           म_माप(peer->ssl_peer_कुंजी),
                           my_hmac);
      अगर(परिणाम)
        जाओ out;
      अगर(!स_भेद(my_hmac, hmac, माप(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        अगर(!peer->hmac_set) {
          स_नकल(peer->कुंजी_salt, salt, माप(peer->कुंजी_salt));
          स_नकल(peer->कुंजी_hmac, hmac, माप(peer->कुंजी_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        जाओ out;
      }
    }
  }
out:
  वापस परिणाम;
}

CURLcode Curl_ssl_session_import(काष्ठा Curl_easy *तथ्य,
                                 स्थिर अक्षर *ssl_peer_कुंजी,
                                 स्थिर अचिन्हित अक्षर *shmac, माप_प्रकार shmac_len,
                                 स्थिर व्योम *sdata, माप_प्रकार sdata_len)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  काष्ठा Curl_ssl_scache_peer *peer = शून्य;
  काष्ठा Curl_ssl_session *s = शून्य;
  bool have_lock = FALSE;
  CURLcode परिणाम;

  अगर(!scache || !sdata || !sdata_len) {
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }
  अगर(!ssl_peer_कुंजी && (!shmac || !shmac_len)) {
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }

  परिणाम = Curl_ssl_session_unpack(तथ्य, sdata, sdata_len, &s);
  अगर(परिणाम)
    जाओ out;

  Curl_ssl_scache_lock(तथ्य);
  have_lock = TRUE;

  अगर(ssl_peer_कुंजी) {
    परिणाम = cf_ssl_add_peer(तथ्य, scache, ssl_peer_कुंजी, शून्य, &peer);
    अगर(परिणाम)
      जाओ out;
  }
  अन्यथा अगर(shmac_len != (माप(peer->कुंजी_salt) + माप(peer->कुंजी_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;
    जाओ out;
  }
  अन्यथा {
    स्थिर अचिन्हित अक्षर *salt = shmac;
    स्थिर अचिन्हित अक्षर *hmac = shmac + माप(peer->कुंजी_salt);

    परिणाम = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    अगर(परिणाम)
      जाओ out;
    अगर(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      अगर(peer) {
        परिणाम = cf_ssl_scache_peer_init(peer, ssl_peer_कुंजी, शून्य, शून्य,
                                         salt, hmac);
        अगर(परिणाम)
          जाओ out;
      }
    }
  }

  अगर(peer) {
    cf_scache_peer_add_session(peer, s, समय(शून्य));
    s = शून्य; /* peer is now owner */
    CURL_TRC_SSLS(तथ्य, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_कुंजी ? peer->ssl_peer_कुंजी : "without key",
                  Curl_llist_गिनती(&peer->sessions));
  }

out:
  अगर(scache && have_lock)
    Curl_ssl_scache_unlock(तथ्य);
  Curl_ssl_session_destroy(s);
  वापस परिणाम;
}

CURLcode Curl_ssl_session_export(काष्ठा Curl_easy *तथ्य,
                                 curl_ssls_export_cb *export_fn,
                                 व्योम *userptr)
{
  काष्ठा Curl_ssl_scache *scache = cf_ssl_scache_get(तथ्य);
  काष्ठा Curl_ssl_scache_peer *peer;
  काष्ठा dynbuf sbuf, hbuf;
  काष्ठा Curl_llist_गाँठ *n;
  माप_प्रकार i;
  curl_off_t now = समय(शून्य);
  CURLcode परिणाम = CURLE_OK;
#अगर_घोषित CURLVERBOSE
  माप_प्रकार npeers = 0, ntickets = 0;
#पूर्ण_अगर

  अगर(!export_fn)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(तथ्य);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_लंबाई * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  क्रम(i = 0; scache && i < scache->peer_गिनती; i++) {
    peer = &scache->peers[i];
    अगर(!peer->ssl_peer_कुंजी && !peer->hmac_set)
      जारी; /* skip free entry */
    अगर(!peer->exportable)
      जारी;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_शीर्ष(&peer->sessions);
    अगर(n)
      VERBOSE(++npeers);
    जबतक(n) {
      काष्ठा Curl_ssl_session *s = Curl_गाँठ_elem(n);
      अगर(!peer->hmac_set) {
        परिणाम = cf_ssl_scache_peer_set_hmac(peer);
        अगर(परिणाम)
          जाओ out;
      }
      अगर(!curlx_dyn_len(&hbuf)) {
        परिणाम = curlx_dyn_addn(&hbuf, peer->कुंजी_salt, माप(peer->कुंजी_salt));
        अगर(परिणाम)
          जाओ out;
        परिणाम = curlx_dyn_addn(&hbuf, peer->कुंजी_hmac, माप(peer->कुंजी_hmac));
        अगर(परिणाम)
          जाओ out;
      }
      curlx_dyn_reset(&sbuf);
      परिणाम = Curl_ssl_session_pack(तथ्य, s, &sbuf);
      अगर(परिणाम)
        जाओ out;

      परिणाम = export_fn(तथ्य, userptr, peer->ssl_peer_कुंजी,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      अगर(परिणाम)
        जाओ out;
      VERBOSE(++ntickets);
      n = Curl_गाँठ_अग्र(n);
    }
  }
  परिणाम = CURLE_OK;
  CURL_TRC_SSLS(तथ्य, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(तथ्य);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  वापस परिणाम;
}

#पूर्ण_अगर /* USE_SSLS_EXPORT */

#पूर्ण_अगर /* USE_SSL */
