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
#അന്തര്ഭാവ "curl_setup.h"

#ifdef USE_SSL

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "cfilters.h"

#അന്തര്ഭാവ "vtls/vtls.h" /* generic SSL protos etc */
#അന്തര്ഭാവ "vtls/vtls_int.h"
#അന്തര്ഭാവ "vtls/vtls_scache.h"
#അന്തര്ഭാവ "vtls/vtls_spack.h"

#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "llist.h"
#അന്തര്ഭാവ "curl_share.h"
#അന്തര്ഭാവ "curl_threads.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "curl_sha256.h"
#അന്തര്ഭാവ "rand.h"
#അന്തര്ഭാവ "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
ഘടന Curl_ssl_scache_peer {
  അക്ഷരം *ssl_peer_താക്കോൽ;      /* id for peer + relevant TLS configuration */
  അക്ഷരം *clientcert;
  അക്ഷരം *താക്കോൽ_passwd;
  ഘടന Curl_llist sessions;
  ശൂന്യം *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned അക്ഷരം താക്കോൽ_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned അക്ഷരം താക്കോൽ_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
സ്ഥിതം CURLcode cf_ssl_peer_താക്കോൽ_add_string(ഘടന dynbuf *buf,
                                           സ്ഥിരം അക്ഷരം *name,
                                           സ്ഥിരം അക്ഷരം *മൂല്യം)
{
  തിരികെ curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(മൂല്യം), മൂല്യം);
}

സ്ഥിതം CURLcode cf_ssl_peer_താക്കോൽ_add_path(ഘടന dynbuf *buf,
                                         സ്ഥിരം അക്ഷരം *name,
                                         സ്ഥിരം അക്ഷരം *path,
                                         bool *is_local)
{
  എങ്കിൽ(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    അക്ഷരം *abspath = _fullpath(ശൂന്യ, path, 0);
    എങ്കിൽ(abspath) {
      CURLcode result = cf_ssl_peer_താക്കോൽ_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      സ്വതന്ത്രം(abspath); /* allocated by CRT, use system free() */
      തിരികെ result;
    }
    *is_local = TRUE;
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_REALPATH)
    എങ്കിൽ(path[0] != '/') {
      അക്ഷരം *abspath = realpath(path, ശൂന്യ);
      എങ്കിൽ(abspath) {
        CURLcode result = cf_ssl_peer_താക്കോൽ_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        സ്വതന്ത്രം(abspath); /* allocated by libc, free without memdebug */
        തിരികെ result;
      }
      *is_local = TRUE;
    }
#endif
    തിരികെ cf_ssl_peer_താക്കോൽ_add_string(buf, name, path);
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode cf_ssl_peer_താക്കോൽ_add_hash(ഘടന dynbuf *buf,
                                         സ്ഥിരം അക്ഷരം *name,
                                         ഘടന curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  എങ്കിൽ(blob && blob->len) {
    unsigned അക്ഷരം hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    എങ്കിൽ(result)
      പോകുക out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    എങ്കിൽ(result)
      പോകുക out;
    വേണ്ടി(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      എങ്കിൽ(result)
        പോകുക out;
    }
  }
out:
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_ssl_peer_താക്കോൽ_add_mtls(ഘടന dynbuf *buf,
                                         ഘടന ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  എങ്കിൽ(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_താക്കോൽ_add_path(buf, "CCERT", ssl->clientcert, is_local);
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->താക്കോൽ && ssl->താക്കോൽ[0]) {
    result = cf_ssl_peer_താക്കോൽ_add_path(buf, "KEY", ssl->താക്കോൽ, is_local);
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->താക്കോൽ_blob) {
    result = cf_ssl_peer_താക്കോൽ_add_hash(buf, "KEYBlob", ssl->താക്കോൽ_blob);
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    വേണ്ടി(i = 0; !result && ssl->cert_type[i]; i++) {
      അക്ഷരം c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->താക്കോൽ_type && ssl->താക്കോൽ_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->താക്കോൽ_type));
    വേണ്ടി(i = 0; !result && ssl->താക്കോൽ_type[i]; i++) {
      അക്ഷരം c = Curl_raw_toupper(ssl->താക്കോൽ_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  തിരികെ result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

സ്ഥിതം bool cf_ssl_peer_താക്കോൽ_is_global(സ്ഥിരം അക്ഷരം *peer_താക്കോൽ)
{
  size_t len = peer_താക്കോൽ ? strlen(peer_താക്കോൽ) : 0;
  തിരികെ (len > 2) &&
         (peer_താക്കോൽ[len - 1] == 'G') &&
         (peer_താക്കോൽ[len - 2] == ':');
}

സ്ഥിതം CURLcode ssl_peer_താക്കോൽ_add_transport(ഘടന dynbuf *buf,
                                           uint8_t transport)
{
  തിരഞ്ഞെടുപ്പ്(transport) {
  അവസ്ഥ TRNSPRT_TCP:
    തിരികെ CURLE_OK;
  അവസ്ഥ TRNSPRT_UDP:
    തിരികെ curlx_dyn_add(buf, ":UDP");
  അവസ്ഥ TRNSPRT_QUIC:
    തിരികെ curlx_dyn_add(buf, ":QUIC");
  അവസ്ഥ TRNSPRT_UNIX:
    തിരികെ curlx_dyn_add(buf, ":UNIX");
  സ്ഥിരസ്ഥിതി:
    തിരികെ curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

സ്ഥിതം CURLcode ssl_peer_താക്കോൽ_add_vrfy(ഘടന dynbuf *buf,
                                      ഘടന ssl_filter_config *ssl,
                                      സ്ഥിരം ഘടന ssl_peer *peer)
{
  CURLcode result;

  എങ്കിൽ(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    എങ്കിൽ(result)
      തിരികെ result;
  }
  എങ്കിൽ(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    എങ്കിൽ(result)
      തിരികെ result;
  }
  എങ്കിൽ(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    എങ്കിൽ(result)
      തിരികെ result;
  }
  എങ്കിൽ((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_താക്കോൽ_add_string(buf, "CHOST", peer->peer->hostname);
    എങ്കിൽ(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    എങ്കിൽ(result)
      തിരികെ result;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode ssl_peer_താക്കോൽ_build(ഘടന ssl_filter_config *ssl,
                                   സ്ഥിരം ഘടന ssl_peer *peer,
                                   സ്ഥിരം അക്ഷരം *tls_id,
                                   അക്ഷരം **ppeer_താക്കോൽ)
{
  ഘടന dynbuf buf;
  size_t താക്കോൽ_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_താക്കോൽ = ശൂന്യ;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  എങ്കിൽ(result)
    പോകുക out;
  result = ssl_peer_താക്കോൽ_add_transport(&buf, peer->transport);
  എങ്കിൽ(result)
    പോകുക out;
  result = ssl_peer_താക്കോൽ_add_vrfy(&buf, ssl, peer);
  എങ്കിൽ(result)
    പോകുക out;
  എങ്കിൽ(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->cipher_list) {
    result = cf_ssl_peer_താക്കോൽ_add_string(&buf, "CIPHER", ssl->cipher_list);
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->cipher_list13) {
    result = cf_ssl_peer_താക്കോൽ_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->curves) {
    result = cf_ssl_peer_താക്കോൽ_add_string(&buf, "CURVES", ssl->curves);
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->signature_algorithms) {
    result = cf_ssl_peer_താക്കോൽ_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->verifypeer) {
    result = cf_ssl_peer_താക്കോൽ_add_path(&buf, "CA", ssl->CAfile, &is_local);
    എങ്കിൽ(!result)
      result = cf_ssl_peer_താക്കോൽ_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    എങ്കിൽ(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    എങ്കിൽ(!result)
      result = cf_ssl_peer_താക്കോൽ_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    എങ്കിൽ(result)
      പോകുക out;
    result = cf_ssl_peer_താക്കോൽ_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    എങ്കിൽ(result)
      പോകുക out;
    എങ്കിൽ(ssl->ca_info_blob) {
      result = cf_ssl_peer_താക്കോൽ_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      എങ്കിൽ(result)
        പോകുക out;
    }
    എങ്കിൽ(ssl->issuercert_blob) {
      result = cf_ssl_peer_താക്കോൽ_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      എങ്കിൽ(result)
        പോകുക out;
    }
  }
  എങ്കിൽ(ssl->cert_blob) {
    result = cf_ssl_peer_താക്കോൽ_add_hash(&buf, "CertBlob", ssl->cert_blob);
    എങ്കിൽ(result)
      പോകുക out;
  }
  എങ്കിൽ(ssl->pinned_താക്കോൽ && ssl->pinned_താക്കോൽ[0]) {
    result = cf_ssl_peer_താക്കോൽ_add_string(&buf, "Pinned", ssl->pinned_താക്കോൽ);
    എങ്കിൽ(result)
      പോകുക out;
  }

  result = cf_ssl_peer_താക്കോൽ_add_mtls(&buf, ssl, &is_local);
  എങ്കിൽ(result)
    പോകുക out;

  എങ്കിൽ(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    പോകുക out;
  }
  result = cf_ssl_peer_താക്കോൽ_add_string(&buf, "IMPL", tls_id);
  എങ്കിൽ(result)
    പോകുക out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  എങ്കിൽ(result)
    പോകുക out;

  *ppeer_താക്കോൽ = curlx_dyn_take(&buf, &താക്കോൽ_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  തിരികെ result;
}

CURLcode Curl_ssl_peer_താക്കോൽ_make(സ്ഥിരം ഘടന ssl_peer *peer,
                                ഘടന ssl_filter_config *sslc,
                                സ്ഥിരം അക്ഷരം *tls_id,
                                അക്ഷരം **ppeer_താക്കോൽ)
{
  തിരികെ ssl_peer_താക്കോൽ_build(sslc, peer, tls_id, ppeer_താക്കോൽ);
}

ഘടന Curl_ssl_scache {
  unsigned പൂർണ്ണസംഖ്യ magic;
  ഘടന Curl_ssl_scache_peer *peers;
  size_t peer_എണ്ണം;
  പൂർണ്ണസംഖ്യ default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

സ്ഥിതം ഘടന Curl_ssl_scache *cf_ssl_scache_get(ഘടന Curl_easy *data)
{
  ഘടന Curl_ssl_scache *scache = ശൂന്യ;
  /* If a share is present, its ssl_scache has preference over the multi */
  എങ്കിൽ(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  അല്ലെങ്കിൽ എങ്കിൽ(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  എങ്കിൽ(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (ശൂന്യം *)scache);
    DEBUGASSERT(0);
    തിരികെ ശൂന്യ;
  }
  തിരികെ scache;
}

സ്ഥിതം ശൂന്യം cf_ssl_scache_session_ldestroy(ശൂന്യം *udata, ശൂന്യം *obj)
{
  ഘടന Curl_ssl_session *s = obj;
  (ശൂന്യം)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(ശൂന്യം *sdata, size_t sdata_len,
                                 പൂർണ്ണസംഖ്യ ietf_tls_id, സ്ഥിരം അക്ഷരം *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 ഘടന Curl_ssl_session **psession)
{
  തിരികെ Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  ശൂന്യ, 0, psession);
}

CURLcode Curl_ssl_session_create2(ശൂന്യം *sdata, size_t sdata_len,
                                  പൂർണ്ണസംഖ്യ ietf_tls_id, സ്ഥിരം അക്ഷരം *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned അക്ഷരം *quic_tp, size_t quic_tp_len,
                                  ഘടന Curl_ssl_session **psession)
{
  ഘടന Curl_ssl_session *s;

  എങ്കിൽ(!sdata || !sdata_len) {
    curlx_free(sdata);
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = ശൂന്യ;
  s = curlx_calloc(1, വലുപ്പം(*s));
  എങ്കിൽ(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  എങ്കിൽ(alpn) {
    s->alpn = curlx_strdup(alpn);
    എങ്കിൽ(!s->alpn) {
      cf_ssl_scache_session_ldestroy(ശൂന്യ, s);
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_ssl_session_destroy(ഘടന Curl_ssl_session *s)
{
  എങ്കിൽ(s) {
    /* if in the list, the list destructor takes care of it */
    എങ്കിൽ(Curl_കണ്ണി_llist(&s->list))
      Curl_കണ്ണി_remove(&s->list);
    അല്ലെങ്കിൽ {
      cf_ssl_scache_session_ldestroy(ശൂന്യ, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(ഘടന Curl_ssl_session *src,
                              ഘടന Curl_ssl_session **pdest)
{
  ഘടന Curl_ssl_session *dest = ശൂന്യ;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  എങ്കിൽ(!src || !pdest)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = ശൂന്യ;

  dest = curlx_calloc(1, വലുപ്പം(*dest));
  എങ്കിൽ(!dest)
    പോകുക out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  എങ്കിൽ(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    എങ്കിൽ(!dest->sdata)
      പോകുക out;
    dest->sdata_len = src->sdata_len;
  }
  എങ്കിൽ(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    എങ്കിൽ(!dest->quic_tp)
      പോകുക out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  എങ്കിൽ(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    എങ്കിൽ(!dest->alpn)
      പോകുക out;
  }
  result = CURLE_OK;

out:
  എങ്കിൽ(!result)
    *pdest = dest;
  അല്ലെങ്കിൽ {
    *pdest = ശൂന്യ;
    എങ്കിൽ(dest)
      Curl_ssl_session_destroy(dest);
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_ssl_scache_clear_peer(ഘടന Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, ശൂന്യ);
  എങ്കിൽ(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    എങ്കിൽ(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = ശൂന്യ;
  }
  peer->sobj_free = ശൂന്യ;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->താക്കോൽ_passwd);
  curlx_safefree(peer->ssl_peer_താക്കോൽ);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

സ്ഥിതം ശൂന്യം cf_ssl_scache_peer_set_obj(ഘടന Curl_ssl_scache_peer *peer,
                                       ശൂന്യം *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  എങ്കിൽ(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

സ്ഥിതം ശൂന്യം cf_ssl_cache_peer_update(ഘടന Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->താക്കോൽ_passwd &&
                      (!peer->ssl_peer_താക്കോൽ ||
                       cf_ssl_peer_താക്കോൽ_is_global(peer->ssl_peer_താക്കോൽ)));
}

സ്ഥിതം CURLcode cf_ssl_scache_peer_init(ഘടന Curl_ssl_scache_peer *peer,
                                        സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                                        സ്ഥിരം അക്ഷരം *clientcert,
                                        സ്ഥിരം അക്ഷരം *താക്കോൽ_passwd,
                                        സ്ഥിരം unsigned അക്ഷരം *salt,
                                        സ്ഥിരം unsigned അക്ഷരം *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_താക്കോൽ);
  എങ്കിൽ(ssl_peer_താക്കോൽ) {
    peer->ssl_peer_താക്കോൽ = curlx_strdup(ssl_peer_താക്കോൽ);
    എങ്കിൽ(!peer->ssl_peer_താക്കോൽ)
      പോകുക out;
    peer->hmac_set = FALSE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(salt && hmac) {
    memcpy(peer->താക്കോൽ_salt, salt, വലുപ്പം(peer->താക്കോൽ_salt));
    memcpy(peer->താക്കോൽ_hmac, hmac, വലുപ്പം(peer->താക്കോൽ_hmac));
    peer->hmac_set = TRUE;
  }
  അല്ലെങ്കിൽ {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക out;
  }
  എങ്കിൽ(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    എങ്കിൽ(!peer->clientcert)
      പോകുക out;
  }
  എങ്കിൽ(താക്കോൽ_passwd) {
    peer->താക്കോൽ_passwd = curlx_strdup(താക്കോൽ_passwd);
    എങ്കിൽ(!peer->താക്കോൽ_passwd)
      പോകുക out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  എങ്കിൽ(result)
    cf_ssl_scache_clear_peer(peer);
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_scache_session_remove(ഘടന Curl_ssl_scache_peer *peer,
                                     ഘടന Curl_ssl_session *s)
{
  (ശൂന്യം)peer;
  DEBUGASSERT(Curl_കണ്ണി_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

സ്ഥിതം bool cf_scache_session_expired(ഘടന Curl_ssl_session *s,
                                      curl_off_t now)
{
  തിരികെ (s->valid_until > 0) && (s->valid_until < now);
}

സ്ഥിതം ശൂന്യം cf_scache_peer_remove_expired(ഘടന Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  ഘടന Curl_llist_കണ്ണി *n = Curl_llist_head(&peer->sessions);
  ആകുമ്പോൾ(n) {
    ഘടന Curl_ssl_session *s = Curl_കണ്ണി_elem(n);
    n = Curl_കണ്ണി_next(n);
    എങ്കിൽ(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

സ്ഥിതം ശൂന്യം cf_scache_peer_remove_non13(ഘടന Curl_ssl_scache_peer *peer)
{
  ഘടന Curl_llist_കണ്ണി *n = Curl_llist_head(&peer->sessions);
  ആകുമ്പോൾ(n) {
    ഘടന Curl_ssl_session *s = Curl_കണ്ണി_elem(n);
    n = Curl_കണ്ണി_next(n);
    എങ്കിൽ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                ഘടന Curl_ssl_scache **pscache)
{
  ഘടന Curl_ssl_scache *scache;
  ഘടന Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = ശൂന്യ;
  peers = curlx_calloc(max_peers, വലുപ്പം(*peers));
  എങ്കിൽ(!peers)
    തിരികെ CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, വലുപ്പം(*scache));
  എങ്കിൽ(!scache) {
    curlx_free(peers);
    തിരികെ CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_എണ്ണം = max_peers;
  scache->peers = peers;
  scache->age = 1;
  വേണ്ടി(i = 0; i < scache->peer_എണ്ണം; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  തിരികെ CURLE_OK;
}

ശൂന്യം Curl_ssl_scache_destroy(ഘടന Curl_ssl_scache *scache)
{
  എങ്കിൽ(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    വേണ്ടി(i = 0; i < scache->peer_എണ്ണം; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(ഘടന Curl_cfilter *cf, ഘടന Curl_easy *data)
{
  എങ്കിൽ(cf_ssl_scache_get(data)) {
    ഘടന ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    തിരികെ conn_config ? conn_config->cache_session : FALSE;
  }
  തിരികെ FALSE;
}

/* Lock shared SSL session data */
ശൂന്യം Curl_ssl_scache_lock(ഘടന Curl_easy *data)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  എങ്കിൽ(scache) {
    എങ്കിൽ(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#അല്ലെങ്കിൽ
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
ശൂന്യം Curl_ssl_scache_unlock(ഘടന Curl_easy *data)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  എങ്കിൽ(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#അല്ലെങ്കിൽ
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    എങ്കിൽ(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(ഘടന Curl_easy *data)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  എങ്കിൽ(!scache)
    തിരികെ FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#അല്ലെങ്കിൽ
  locked = (bool)scache->is_locked;
#endif
  തിരികെ locked;
}

സ്ഥിതം bool cf_ssl_scache_match_auth(ഘടന Curl_ssl_scache_peer *peer,
                                     ഘടന ssl_filter_config *conn_config)
{
  എങ്കിൽ(!conn_config) {
    എങ്കിൽ(peer->clientcert || peer->താക്കോൽ_passwd)
      തിരികെ FALSE;
    തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    തിരികെ FALSE;
  എങ്കിൽ(Curl_timestrcmp(peer->താക്കോൽ_passwd, conn_config->താക്കോൽ_passwd))
    തിരികെ FALSE;
  തിരികെ TRUE;
}

സ്ഥിതം CURLcode cf_ssl_find_peer_by_താക്കോൽ(ഘടന Curl_easy *data,
                                        ഘടന Curl_ssl_scache *scache,
                                        സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                                        ഘടന ssl_filter_config *conn_config,
                                        ഘടന Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_താക്കോൽ_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = ശൂന്യ;
  എങ്കിൽ(!GOOD_SCACHE(scache)) {
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_താക്കോൽ, scache->peer_എണ്ണം);

  /* check for entries with known peer_key */
  വേണ്ടി(i = 0; scache && i < scache->peer_എണ്ണം; i++) {
    എങ്കിൽ(scache->peers[i].ssl_peer_താക്കോൽ &&
       !strcmp(ssl_peer_താക്കോൽ, scache->peers[i].ssl_peer_താക്കോൽ) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      പോകുക out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  വേണ്ടി(i = 0; scache && i < scache->peer_എണ്ണം; i++) {
    എങ്കിൽ(!scache->peers[i].ssl_peer_താക്കോൽ &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned അക്ഷരം my_hmac[CURL_SHA256_DIGEST_LENGTH];
      എങ്കിൽ(!peer_താക്കോൽ_len) /* we are lazy */
        peer_താക്കോൽ_len = strlen(ssl_peer_താക്കോൽ);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].താക്കോൽ_salt,
                           വലുപ്പം(scache->peers[i].താക്കോൽ_salt),
                           (സ്ഥിരം unsigned അക്ഷരം *)ssl_peer_താക്കോൽ,
                           peer_താക്കോൽ_len,
                           my_hmac);
      എങ്കിൽ(result)
        പോകുക out;
      എങ്കിൽ(!memcmp(scache->peers[i].താക്കോൽ_hmac, my_hmac, വലുപ്പം(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_താക്കോൽ);
        scache->peers[i].ssl_peer_താക്കോൽ = curlx_strdup(ssl_peer_താക്കോൽ);
        എങ്കിൽ(!scache->peers[i].ssl_peer_താക്കോൽ) {
          result = CURLE_OUT_OF_MEMORY;
          പോകുക out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        പോകുക out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_താക്കോൽ);
out:
  തിരികെ result;
}

സ്ഥിതം ഘടന Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  ഘടന Curl_ssl_scache *scache)
{
  ഘടന Curl_ssl_scache_peer *peer = ശൂന്യ;
  size_t i;

  /* find empty or oldest peer */
  വേണ്ടി(i = 0; i < scache->peer_എണ്ണം; ++i) {
    /* free peer entry? */
    എങ്കിൽ(!scache->peers[i].ssl_peer_താക്കോൽ && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      നിർത്തുക;
    }
    /* peer without sessions and obj */
    എങ്കിൽ(!scache->peers[i].sobj &&
       !Curl_llist_എണ്ണം(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      നിർത്തുക;
    }
    /* remember "oldest" peer */
    എങ്കിൽ(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  എങ്കിൽ(peer)
    cf_ssl_scache_clear_peer(peer);
  തിരികെ peer;
}

സ്ഥിതം CURLcode cf_ssl_add_peer(ഘടന Curl_easy *data,
                                ഘടന Curl_ssl_scache *scache,
                                സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                                ഘടന ssl_filter_config *conn_config,
                                ഘടന Curl_ssl_scache_peer **ppeer)
{
  ഘടന Curl_ssl_scache_peer *peer = ശൂന്യ;
  CURLcode result = CURLE_OK;

  *ppeer = ശൂന്യ;
  എങ്കിൽ(ssl_peer_താക്കോൽ) {
    result = cf_ssl_find_peer_by_താക്കോൽ(data, scache, ssl_peer_താക്കോൽ, conn_config,
                                     &peer);
    എങ്കിൽ(result || !scache->peer_എണ്ണം)
      തിരികെ result;
  }

  എങ്കിൽ(peer) {
    *ppeer = peer;
    തിരികെ CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  എങ്കിൽ(peer) {
    അക്ഷരം buffer[64];
    സ്ഥിരം അക്ഷരം *ccert = conn_config ? conn_config->clientcert : ശൂന്യ;
    സ്ഥിരം അക്ഷരം *kpasswd = conn_config ? conn_config->താക്കോൽ_passwd : ശൂന്യ;
    എങ്കിൽ(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, വലുപ്പം(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_താക്കോൽ, ccert, kpasswd,
                                     ശൂന്യ, ശൂന്യ);
    എങ്കിൽ(result)
      പോകുക out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  എങ്കിൽ(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  തിരികെ result;
}

സ്ഥിതം ശൂന്യം cf_scache_peer_add_session(ഘടന Curl_ssl_scache_peer *peer,
                                       ഘടന Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  എങ്കിൽ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, ശൂന്യ);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  അല്ലെങ്കിൽ {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    ആകുമ്പോൾ(Curl_llist_എണ്ണം(&peer->sessions) > peer->max_sessions) {
      Curl_കണ്ണി_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

സ്ഥിതം CURLcode cf_scache_add_session(ഘടന Curl_cfilter *cf,
                                      ഘടന Curl_easy *data,
                                      ഘടന Curl_ssl_scache *scache,
                                      സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                                      ഘടന Curl_ssl_session *s)
{
  ഘടന Curl_ssl_scache_peer *peer = ശൂന്യ;
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(ശൂന്യ);
  curl_off_t max_lifetime;

  എങ്കിൽ(!scache || !scache->peer_എണ്ണം) {
    Curl_ssl_session_destroy(s);
    തിരികെ CURLE_OK;
  }

  എങ്കിൽ(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  എങ്കിൽ(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  എങ്കിൽ(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    തിരികെ CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_താക്കോൽ, conn_config, &peer);
  എങ്കിൽ(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (പൂർണ്ണസംഖ്യ)result);
    Curl_ssl_session_destroy(s);
    പോകുക out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  എങ്കിൽ(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_താക്കോൽ, (പൂർണ്ണസംഖ്യ)result);
  }
  അല്ലെങ്കിൽ
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_താക്കോൽ,
                  (unsigned പൂർണ്ണസംഖ്യ)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_എണ്ണം(&peer->sessions) : 0);
  തിരികെ result;
}

CURLcode Curl_ssl_scache_put(ഘടന Curl_cfilter *cf,
                             ഘടന Curl_easy *data,
                             സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                             ഘടന Curl_ssl_session *s)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  എങ്കിൽ(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    തിരികെ CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_താക്കോൽ, s);
  Curl_ssl_scache_unlock(data);
  തിരികെ result;
}

ശൂന്യം Curl_ssl_scache_return(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data,
                            സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                            ഘടന Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  എങ്കിൽ(s && s->ietf_tls_id < 0x304)
    (ശൂന്യം)Curl_ssl_scache_put(cf, data, ssl_peer_താക്കോൽ, s);
  അല്ലെങ്കിൽ
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                              ഘടന Curl_ssl_session **ps)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന Curl_ssl_scache_peer *peer = ശൂന്യ;
  ഘടന Curl_llist_കണ്ണി *n;
  ഘടന Curl_ssl_session *s = ശൂന്യ;
  CURLcode result;

  *ps = ശൂന്യ;
  എങ്കിൽ(!scache)
    തിരികെ CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_താക്കോൽ(data, scache, ssl_peer_താക്കോൽ, conn_config,
                                   &peer);
  എങ്കിൽ(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(ശൂന്യ));
    n = Curl_llist_head(&peer->sessions);
    എങ്കിൽ(n) {
      s = Curl_കണ്ണി_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  എങ്കിൽ(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_താക്കോൽ, (unsigned പൂർണ്ണസംഖ്യ)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_എണ്ണം(&peer->sessions));
  }
  അല്ലെങ്കിൽ {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_താക്കോൽ);
  }
  Curl_ssl_scache_unlock(data);
  തിരികെ result;
}

CURLcode Curl_ssl_scache_add_obj(ഘടന Curl_cfilter *cf,
                                 ഘടന Curl_easy *data,
                                 സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                                 ശൂന്യം *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന Curl_ssl_scache_peer *peer = ശൂന്യ;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  എങ്കിൽ(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_താക്കോൽ, conn_config, &peer);
  എങ്കിൽ(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (പൂർണ്ണസംഖ്യ)result);
    പോകുക out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = ശൂന്യ;  /* peer took ownership */

out:
  എങ്കിൽ(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  തിരികെ result;
}

ശൂന്യം *Curl_ssl_scache_get_obj(ഘടന Curl_cfilter *cf,
                              ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന Curl_ssl_scache_peer *peer = ശൂന്യ;
  CURLcode result;
  ശൂന്യം *sobj;

  എങ്കിൽ(!scache)
    തിരികെ ശൂന്യ;

  result = cf_ssl_find_peer_by_താക്കോൽ(data, scache, ssl_peer_താക്കോൽ, conn_config,
                                   &peer);
  എങ്കിൽ(result)
    തിരികെ ശൂന്യ;

  sobj = peer ? peer->sobj : ശൂന്യ;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_താക്കോൽ);
  തിരികെ sobj;
}

ശൂന്യം Curl_ssl_scache_remove_all(ഘടന Curl_cfilter *cf,
                                ഘടന Curl_easy *data,
                                സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ഘടന ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ഘടന Curl_ssl_scache_peer *peer = ശൂന്യ;
  CURLcode result;

  (ശൂന്യം)cf;
  എങ്കിൽ(!scache)
    തിരികെ;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_താക്കോൽ(data, scache, ssl_peer_താക്കോൽ, conn_config,
                                   &peer);
  എങ്കിൽ(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

സ്ഥിതം CURLcode cf_ssl_scache_peer_set_hmac(ഘടന Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  എങ്കിൽ(!peer->ssl_peer_താക്കോൽ)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(ശൂന്യ, peer->താക്കോൽ_salt, വലുപ്പം(peer->താക്കോൽ_salt));
  എങ്കിൽ(result)
    തിരികെ result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->താക്കോൽ_salt, വലുപ്പം(peer->താക്കോൽ_salt),
                       (സ്ഥിരം unsigned അക്ഷരം *)peer->ssl_peer_താക്കോൽ,
                       strlen(peer->ssl_peer_താക്കോൽ),
                       peer->താക്കോൽ_hmac);
  എങ്കിൽ(!result)
    peer->hmac_set = TRUE;
  തിരികെ result;
}

സ്ഥിതം CURLcode cf_ssl_find_peer_by_hmac(ഘടന Curl_ssl_scache *scache,
                                         സ്ഥിരം unsigned അക്ഷരം *salt,
                                         സ്ഥിരം unsigned അക്ഷരം *hmac,
                                         ഘടന Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = ശൂന്യ;
  എങ്കിൽ(!GOOD_SCACHE(scache))
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  വേണ്ടി(i = 0; scache && i < scache->peer_എണ്ണം; i++) {
    ഘടന Curl_ssl_scache_peer *peer = &scache->peers[i];
    എങ്കിൽ(!cf_ssl_scache_match_auth(peer, ശൂന്യ))
      തുടരുക;
    എങ്കിൽ(scache->peers[i].hmac_set &&
       !memcmp(peer->താക്കോൽ_salt, salt, വലുപ്പം(peer->താക്കോൽ_salt)) &&
       !memcmp(peer->താക്കോൽ_hmac, hmac, വലുപ്പം(peer->താക്കോൽ_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      പോകുക out;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(peer->ssl_peer_താക്കോൽ) {
      unsigned അക്ഷരം my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, വലുപ്പം(peer->താക്കോൽ_salt),
                           (സ്ഥിരം unsigned അക്ഷരം *)peer->ssl_peer_താക്കോൽ,
                           strlen(peer->ssl_peer_താക്കോൽ),
                           my_hmac);
      എങ്കിൽ(result)
        പോകുക out;
      എങ്കിൽ(!memcmp(my_hmac, hmac, വലുപ്പം(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        എങ്കിൽ(!peer->hmac_set) {
          memcpy(peer->താക്കോൽ_salt, salt, വലുപ്പം(peer->താക്കോൽ_salt));
          memcpy(peer->താക്കോൽ_hmac, hmac, വലുപ്പം(peer->താക്കോൽ_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        പോകുക out;
      }
    }
  }
out:
  തിരികെ result;
}

CURLcode Curl_ssl_session_import(ഘടന Curl_easy *data,
                                 സ്ഥിരം അക്ഷരം *ssl_peer_താക്കോൽ,
                                 സ്ഥിരം unsigned അക്ഷരം *shmac, size_t shmac_len,
                                 സ്ഥിരം ശൂന്യം *sdata, size_t sdata_len)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ഘടന Curl_ssl_scache_peer *peer = ശൂന്യ;
  ഘടന Curl_ssl_session *s = ശൂന്യ;
  bool have_lock = FALSE;
  CURLcode result;

  എങ്കിൽ(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക out;
  }
  എങ്കിൽ(!ssl_peer_താക്കോൽ && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  എങ്കിൽ(result)
    പോകുക out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  എങ്കിൽ(ssl_peer_താക്കോൽ) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_താക്കോൽ, ശൂന്യ, &peer);
    എങ്കിൽ(result)
      പോകുക out;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(shmac_len != (വലുപ്പം(peer->താക്കോൽ_salt) + വലുപ്പം(peer->താക്കോൽ_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    പോകുക out;
  }
  അല്ലെങ്കിൽ {
    സ്ഥിരം unsigned അക്ഷരം *salt = shmac;
    സ്ഥിരം unsigned അക്ഷരം *hmac = shmac + വലുപ്പം(peer->താക്കോൽ_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    എങ്കിൽ(result)
      പോകുക out;
    എങ്കിൽ(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      എങ്കിൽ(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_താക്കോൽ, ശൂന്യ, ശൂന്യ,
                                         salt, hmac);
        എങ്കിൽ(result)
          പോകുക out;
      }
    }
  }

  എങ്കിൽ(peer) {
    cf_scache_peer_add_session(peer, s, time(ശൂന്യ));
    s = ശൂന്യ; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_താക്കോൽ ? peer->ssl_peer_താക്കോൽ : "without key",
                  Curl_llist_എണ്ണം(&peer->sessions));
  }

out:
  എങ്കിൽ(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  തിരികെ result;
}

CURLcode Curl_ssl_session_export(ഘടന Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 ശൂന്യം *userptr)
{
  ഘടന Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ഘടന Curl_ssl_scache_peer *peer;
  ഘടന dynbuf sbuf, hbuf;
  ഘടന Curl_llist_കണ്ണി *n;
  size_t i;
  curl_off_t now = time(ശൂന്യ);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  എങ്കിൽ(!export_fn)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  വേണ്ടി(i = 0; scache && i < scache->peer_എണ്ണം; i++) {
    peer = &scache->peers[i];
    എങ്കിൽ(!peer->ssl_peer_താക്കോൽ && !peer->hmac_set)
      തുടരുക; /* skip free entry */
    എങ്കിൽ(!peer->exportable)
      തുടരുക;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    എങ്കിൽ(n)
      VERBOSE(++npeers);
    ആകുമ്പോൾ(n) {
      ഘടന Curl_ssl_session *s = Curl_കണ്ണി_elem(n);
      എങ്കിൽ(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        എങ്കിൽ(result)
          പോകുക out;
      }
      എങ്കിൽ(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->താക്കോൽ_salt, വലുപ്പം(peer->താക്കോൽ_salt));
        എങ്കിൽ(result)
          പോകുക out;
        result = curlx_dyn_addn(&hbuf, peer->താക്കോൽ_hmac, വലുപ്പം(peer->താക്കോൽ_hmac));
        എങ്കിൽ(result)
          പോകുക out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      എങ്കിൽ(result)
        പോകുക out;

      result = export_fn(data, userptr, peer->ssl_peer_താക്കോൽ,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      എങ്കിൽ(result)
        പോകുക out;
      VERBOSE(++ntickets);
      n = Curl_കണ്ണി_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  തിരികെ result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
