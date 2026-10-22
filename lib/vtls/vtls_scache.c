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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ifdef USE_SSL

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"

#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h" /* generic SSL protos etc */
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_int.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_scache.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls_spack.h"

#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "llist.h"
#ଅନ୍ତର୍ଭାଵ "curl_share.h"
#ଅନ୍ତର୍ଭାଵ "curl_threads.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "curl_sha256.h"
#ଅନ୍ତର୍ଭାଵ "rand.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
ସଂରଚନା Curl_ssl_scache_peer {
  ଅକ୍ଷର *ssl_peer_ଚାବି;      /* id for peer + relevant TLS configuration */
  ଅକ୍ଷର *clientcert;
  ଅକ୍ଷର *ଚାବି_passwd;
  ସଂରଚନା Curl_llist sessions;
  ଶୂନ୍ୟ *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned ଅକ୍ଷର ଚାବି_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned ଅକ୍ଷର ଚାବି_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ସ୍ଥିର CURLcode cf_ssl_peer_ଚାବି_add_string(ସଂରଚନା dynbuf *buf,
                                           ଅଚଳ ଅକ୍ଷର *name,
                                           ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ)
{
  ଫେରନ୍ତୁ curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(ମୂଲ୍ୟ), ମୂଲ୍ୟ);
}

ସ୍ଥିର CURLcode cf_ssl_peer_ଚାବି_add_path(ସଂରଚନା dynbuf *buf,
                                         ଅଚଳ ଅକ୍ଷର *name,
                                         ଅଚଳ ଅକ୍ଷର *path,
                                         bool *is_local)
{
  ଯଦି(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    ଅକ୍ଷର *abspath = _fullpath(ଖାଲି, path, 0);
    ଯଦି(abspath) {
      CURLcode result = cf_ssl_peer_ଚାବି_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      ମୁକ୍ତ(abspath); /* allocated by CRT, use system free() */
      ଫେରନ୍ତୁ result;
    }
    *is_local = TRUE;
#ନଚେତଯଦି defined(HAVE_REALPATH)
    ଯଦି(path[0] != '/') {
      ଅକ୍ଷର *abspath = realpath(path, ଖାଲି);
      ଯଦି(abspath) {
        CURLcode result = cf_ssl_peer_ଚାବି_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        ମୁକ୍ତ(abspath); /* allocated by libc, free without memdebug */
        ଫେରନ୍ତୁ result;
      }
      *is_local = TRUE;
    }
#endif
    ଫେରନ୍ତୁ cf_ssl_peer_ଚାବି_add_string(buf, name, path);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode cf_ssl_peer_ଚାବି_add_hash(ସଂରଚନା dynbuf *buf,
                                         ଅଚଳ ଅକ୍ଷର *name,
                                         ସଂରଚନା curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  ଯଦି(blob && blob->len) {
    unsigned ଅକ୍ଷର hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    ଯଦି(result)
      ଯାଅ out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    ଯଦି(result)
      ଯାଅ out;
    ପାଇଁ(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      ଯଦି(result)
        ଯାଅ out;
    }
  }
out:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_ssl_peer_ଚାବି_add_mtls(ସଂରଚନା dynbuf *buf,
                                         ସଂରଚନା ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  ଯଦି(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_ଚାବି_add_path(buf, "CCERT", ssl->clientcert, is_local);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->ଚାବି && ssl->ଚାବି[0]) {
    result = cf_ssl_peer_ଚାବି_add_path(buf, "KEY", ssl->ଚାବି, is_local);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->ଚାବି_blob) {
    result = cf_ssl_peer_ଚାବି_add_hash(buf, "KEYBlob", ssl->ଚାବି_blob);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    ପାଇଁ(i = 0; !result && ssl->cert_type[i]; i++) {
      ଅକ୍ଷର c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->ଚାବି_type && ssl->ଚାବି_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->ଚାବି_type));
    ପାଇଁ(i = 0; !result && ssl->ଚାବି_type[i]; i++) {
      ଅକ୍ଷର c = Curl_raw_toupper(ssl->ଚାବି_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  ଫେରନ୍ତୁ result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

ସ୍ଥିର bool cf_ssl_peer_ଚାବି_is_global(ଅଚଳ ଅକ୍ଷର *peer_ଚାବି)
{
  size_t len = peer_ଚାବି ? strlen(peer_ଚାବି) : 0;
  ଫେରନ୍ତୁ (len > 2) &&
         (peer_ଚାବି[len - 1] == 'G') &&
         (peer_ଚାବି[len - 2] == ':');
}

ସ୍ଥିର CURLcode ssl_peer_ଚାବି_add_transport(ସଂରଚନା dynbuf *buf,
                                           uint8_t transport)
{
  ବାଛ(transport) {
  ସ୍ଥିତି TRNSPRT_TCP:
    ଫେରନ୍ତୁ CURLE_OK;
  ସ୍ଥିତି TRNSPRT_UDP:
    ଫେରନ୍ତୁ curlx_dyn_add(buf, ":UDP");
  ସ୍ଥିତି TRNSPRT_QUIC:
    ଫେରନ୍ତୁ curlx_dyn_add(buf, ":QUIC");
  ସ୍ଥିତି TRNSPRT_UNIX:
    ଫେରନ୍ତୁ curlx_dyn_add(buf, ":UNIX");
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ସ୍ଥିର CURLcode ssl_peer_ଚାବି_add_vrfy(ସଂରଚନା dynbuf *buf,
                                      ସଂରଚନା ssl_filter_config *ssl,
                                      ଅଚଳ ସଂରଚନା ssl_peer *peer)
{
  CURLcode result;

  ଯଦି(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଯଦି(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଯଦି(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଯଦି((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_ଚାବି_add_string(buf, "CHOST", peer->peer->hostname);
    ଯଦି(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode ssl_peer_ଚାବି_build(ସଂରଚନା ssl_filter_config *ssl,
                                   ଅଚଳ ସଂରଚନା ssl_peer *peer,
                                   ଅଚଳ ଅକ୍ଷର *tls_id,
                                   ଅକ୍ଷର **ppeer_ଚାବି)
{
  ସଂରଚନା dynbuf buf;
  size_t ଚାବି_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_ଚାବି = ଖାଲି;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  ଯଦି(result)
    ଯାଅ out;
  result = ssl_peer_ଚାବି_add_transport(&buf, peer->transport);
  ଯଦି(result)
    ଯାଅ out;
  result = ssl_peer_ଚାବି_add_vrfy(&buf, ssl, peer);
  ଯଦି(result)
    ଯାଅ out;
  ଯଦି(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->cipher_list) {
    result = cf_ssl_peer_ଚାବି_add_string(&buf, "CIPHER", ssl->cipher_list);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->cipher_list13) {
    result = cf_ssl_peer_ଚାବି_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->curves) {
    result = cf_ssl_peer_ଚାବି_add_string(&buf, "CURVES", ssl->curves);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->signature_algorithms) {
    result = cf_ssl_peer_ଚାବି_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->verifypeer) {
    result = cf_ssl_peer_ଚାବି_add_path(&buf, "CA", ssl->CAfile, &is_local);
    ଯଦି(!result)
      result = cf_ssl_peer_ଚାବି_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    ଯଦି(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    ଯଦି(!result)
      result = cf_ssl_peer_ଚାବି_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    ଯଦି(result)
      ଯାଅ out;
    result = cf_ssl_peer_ଚାବି_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    ଯଦି(result)
      ଯାଅ out;
    ଯଦି(ssl->ca_info_blob) {
      result = cf_ssl_peer_ଚାବି_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      ଯଦି(result)
        ଯାଅ out;
    }
    ଯଦି(ssl->issuercert_blob) {
      result = cf_ssl_peer_ଚାବି_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      ଯଦି(result)
        ଯାଅ out;
    }
  }
  ଯଦି(ssl->cert_blob) {
    result = cf_ssl_peer_ଚାବି_add_hash(&buf, "CertBlob", ssl->cert_blob);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଯଦି(ssl->pinned_ଚାବି && ssl->pinned_ଚାବି[0]) {
    result = cf_ssl_peer_ଚାବି_add_string(&buf, "Pinned", ssl->pinned_ଚାବି);
    ଯଦି(result)
      ଯାଅ out;
  }

  result = cf_ssl_peer_ଚାବି_add_mtls(&buf, ssl, &is_local);
  ଯଦି(result)
    ଯାଅ out;

  ଯଦି(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    ଯାଅ out;
  }
  result = cf_ssl_peer_ଚାବି_add_string(&buf, "IMPL", tls_id);
  ଯଦି(result)
    ଯାଅ out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  ଯଦି(result)
    ଯାଅ out;

  *ppeer_ଚାବି = curlx_dyn_take(&buf, &ଚାବି_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_ssl_peer_ଚାବି_make(ଅଚଳ ସଂରଚନା ssl_peer *peer,
                                ସଂରଚନା ssl_filter_config *sslc,
                                ଅଚଳ ଅକ୍ଷର *tls_id,
                                ଅକ୍ଷର **ppeer_ଚାବି)
{
  ଫେରନ୍ତୁ ssl_peer_ଚାବି_build(sslc, peer, tls_id, ppeer_ଚାବି);
}

ସଂରଚନା Curl_ssl_scache {
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା magic;
  ସଂରଚନା Curl_ssl_scache_peer *peers;
  size_t peer_ଗଣନା;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

ସ୍ଥିର ସଂରଚନା Curl_ssl_scache *cf_ssl_scache_get(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_ssl_scache *scache = ଖାଲି;
  /* If a share is present, its ssl_scache has preference over the multi */
  ଯଦି(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  ଅନ୍ୟଥା ଯଦି(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  ଯଦି(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (ଶୂନ୍ୟ *)scache);
    DEBUGASSERT(0);
    ଫେରନ୍ତୁ ଖାଲି;
  }
  ଫେରନ୍ତୁ scache;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ssl_scache_session_ldestroy(ଶୂନ୍ୟ *udata, ଶୂନ୍ୟ *obj)
{
  ସଂରଚନା Curl_ssl_session *s = obj;
  (ଶୂନ୍ୟ)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(ଶୂନ୍ୟ *sdata, size_t sdata_len,
                                 ପୂର୍ଣ୍ଣସଂଖ୍ୟା ietf_tls_id, ଅଚଳ ଅକ୍ଷର *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 ସଂରଚନା Curl_ssl_session **psession)
{
  ଫେରନ୍ତୁ Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  ଖାଲି, 0, psession);
}

CURLcode Curl_ssl_session_create2(ଶୂନ୍ୟ *sdata, size_t sdata_len,
                                  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ietf_tls_id, ଅଚଳ ଅକ୍ଷର *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned ଅକ୍ଷର *quic_tp, size_t quic_tp_len,
                                  ସଂରଚନା Curl_ssl_session **psession)
{
  ସଂରଚନା Curl_ssl_session *s;

  ଯଦି(!sdata || !sdata_len) {
    curlx_free(sdata);
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = ଖାଲି;
  s = curlx_calloc(1, ଆକାର(*s));
  ଯଦି(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  ଯଦି(alpn) {
    s->alpn = curlx_strdup(alpn);
    ଯଦି(!s->alpn) {
      cf_ssl_scache_session_ldestroy(ଖାଲି, s);
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_ssl_session_destroy(ସଂରଚନା Curl_ssl_session *s)
{
  ଯଦି(s) {
    /* if in the list, the list destructor takes care of it */
    ଯଦି(Curl_ଗଣ୍ଠି_llist(&s->list))
      Curl_ଗଣ୍ଠି_remove(&s->list);
    ଅନ୍ୟଥା {
      cf_ssl_scache_session_ldestroy(ଖାଲି, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(ସଂରଚନା Curl_ssl_session *src,
                              ସଂରଚନା Curl_ssl_session **pdest)
{
  ସଂରଚନା Curl_ssl_session *dest = ଖାଲି;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ଯଦି(!src || !pdest)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = ଖାଲି;

  dest = curlx_calloc(1, ଆକାର(*dest));
  ଯଦି(!dest)
    ଯାଅ out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  ଯଦି(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    ଯଦି(!dest->sdata)
      ଯାଅ out;
    dest->sdata_len = src->sdata_len;
  }
  ଯଦି(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    ଯଦି(!dest->quic_tp)
      ଯାଅ out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  ଯଦି(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    ଯଦି(!dest->alpn)
      ଯାଅ out;
  }
  result = CURLE_OK;

out:
  ଯଦି(!result)
    *pdest = dest;
  ଅନ୍ୟଥା {
    *pdest = ଖାଲି;
    ଯଦି(dest)
      Curl_ssl_session_destroy(dest);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ssl_scache_clear_peer(ସଂରଚନା Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, ଖାଲି);
  ଯଦି(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    ଯଦି(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = ଖାଲି;
  }
  peer->sobj_free = ଖାଲି;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->ଚାବି_passwd);
  curlx_safefree(peer->ssl_peer_ଚାବି);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ssl_scache_peer_set_obj(ସଂରଚନା Curl_ssl_scache_peer *peer,
                                       ଶୂନ୍ୟ *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  ଯଦି(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_ssl_cache_peer_update(ସଂରଚନା Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->ଚାବି_passwd &&
                      (!peer->ssl_peer_ଚାବି ||
                       cf_ssl_peer_ଚାବି_is_global(peer->ssl_peer_ଚାବି)));
}

ସ୍ଥିର CURLcode cf_ssl_scache_peer_init(ସଂରଚନା Curl_ssl_scache_peer *peer,
                                        ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                        ଅଚଳ ଅକ୍ଷର *clientcert,
                                        ଅଚଳ ଅକ୍ଷର *ଚାବି_passwd,
                                        ଅଚଳ unsigned ଅକ୍ଷର *salt,
                                        ଅଚଳ unsigned ଅକ୍ଷର *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_ଚାବି);
  ଯଦି(ssl_peer_ଚାବି) {
    peer->ssl_peer_ଚାବି = curlx_strdup(ssl_peer_ଚାବି);
    ଯଦି(!peer->ssl_peer_ଚାବି)
      ଯାଅ out;
    peer->hmac_set = FALSE;
  }
  ଅନ୍ୟଥା ଯଦି(salt && hmac) {
    memcpy(peer->ଚାବି_salt, salt, ଆକାର(peer->ଚାବି_salt));
    memcpy(peer->ଚାବି_hmac, hmac, ଆକାର(peer->ଚାବି_hmac));
    peer->hmac_set = TRUE;
  }
  ଅନ୍ୟଥା {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }
  ଯଦି(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    ଯଦି(!peer->clientcert)
      ଯାଅ out;
  }
  ଯଦି(ଚାବି_passwd) {
    peer->ଚାବି_passwd = curlx_strdup(ଚାବି_passwd);
    ଯଦି(!peer->ଚାବି_passwd)
      ଯାଅ out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  ଯଦି(result)
    cf_ssl_scache_clear_peer(peer);
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_scache_session_remove(ସଂରଚନା Curl_ssl_scache_peer *peer,
                                     ସଂରଚନା Curl_ssl_session *s)
{
  (ଶୂନ୍ୟ)peer;
  DEBUGASSERT(Curl_ଗଣ୍ଠି_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

ସ୍ଥିର bool cf_scache_session_expired(ସଂରଚନା Curl_ssl_session *s,
                                      curl_off_t now)
{
  ଫେରନ୍ତୁ (s->valid_until > 0) && (s->valid_until < now);
}

ସ୍ଥିର ଶୂନ୍ୟ cf_scache_peer_remove_expired(ସଂରଚନା Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n = Curl_llist_head(&peer->sessions);
  ଯେତେବେଳେ(n) {
    ସଂରଚନା Curl_ssl_session *s = Curl_ଗଣ୍ଠି_elem(n);
    n = Curl_ଗଣ୍ଠି_next(n);
    ଯଦି(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ cf_scache_peer_remove_non13(ସଂରଚନା Curl_ssl_scache_peer *peer)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n = Curl_llist_head(&peer->sessions);
  ଯେତେବେଳେ(n) {
    ସଂରଚନା Curl_ssl_session *s = Curl_ଗଣ୍ଠି_elem(n);
    n = Curl_ଗଣ୍ଠି_next(n);
    ଯଦି(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                ସଂରଚନା Curl_ssl_scache **pscache)
{
  ସଂରଚନା Curl_ssl_scache *scache;
  ସଂରଚନା Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = ଖାଲି;
  peers = curlx_calloc(max_peers, ଆକାର(*peers));
  ଯଦି(!peers)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, ଆକାର(*scache));
  ଯଦି(!scache) {
    curlx_free(peers);
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_ଗଣନା = max_peers;
  scache->peers = peers;
  scache->age = 1;
  ପାଇଁ(i = 0; i < scache->peer_ଗଣନା; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  ଫେରନ୍ତୁ CURLE_OK;
}

ଶୂନ୍ୟ Curl_ssl_scache_destroy(ସଂରଚନା Curl_ssl_scache *scache)
{
  ଯଦି(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    ପାଇଁ(i = 0; i < scache->peer_ଗଣନା; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(ସଂରଚନା Curl_cfilter *cf, ସଂରଚନା Curl_easy *data)
{
  ଯଦି(cf_ssl_scache_get(data)) {
    ସଂରଚନା ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    ଫେରନ୍ତୁ conn_config ? conn_config->cache_session : FALSE;
  }
  ଫେରନ୍ତୁ FALSE;
}

/* Lock shared SSL session data */
ଶୂନ୍ୟ Curl_ssl_scache_lock(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ଯଦି(scache) {
    ଯଦି(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#ଅନ୍ୟଥା
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
ଶୂନ୍ୟ Curl_ssl_scache_unlock(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ଯଦି(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#ଅନ୍ୟଥା
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    ଯଦି(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  ଯଦି(!scache)
    ଫେରନ୍ତୁ FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#ଅନ୍ୟଥା
  locked = (bool)scache->is_locked;
#endif
  ଫେରନ୍ତୁ locked;
}

ସ୍ଥିର bool cf_ssl_scache_match_auth(ସଂରଚନା Curl_ssl_scache_peer *peer,
                                     ସଂରଚନା ssl_filter_config *conn_config)
{
  ଯଦି(!conn_config) {
    ଯଦି(peer->clientcert || peer->ଚାବି_passwd)
      ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(Curl_timestrcmp(peer->ଚାବି_passwd, conn_config->ଚାବି_passwd))
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର CURLcode cf_ssl_find_peer_by_ଚାବି(ସଂରଚନା Curl_easy *data,
                                        ସଂରଚନା Curl_ssl_scache *scache,
                                        ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                        ସଂରଚନା ssl_filter_config *conn_config,
                                        ସଂରଚନା Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_ଚାବି_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = ଖାଲି;
  ଯଦି(!GOOD_SCACHE(scache)) {
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_ଚାବି, scache->peer_ଗଣନା);

  /* check for entries with known peer_key */
  ପାଇଁ(i = 0; scache && i < scache->peer_ଗଣନା; i++) {
    ଯଦି(scache->peers[i].ssl_peer_ଚାବି &&
       !strcmp(ssl_peer_ଚାବି, scache->peers[i].ssl_peer_ଚାବି) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      ଯାଅ out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  ପାଇଁ(i = 0; scache && i < scache->peer_ଗଣନା; i++) {
    ଯଦି(!scache->peers[i].ssl_peer_ଚାବି &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned ଅକ୍ଷର my_hmac[CURL_SHA256_DIGEST_LENGTH];
      ଯଦି(!peer_ଚାବି_len) /* we are lazy */
        peer_ଚାବି_len = strlen(ssl_peer_ଚାବି);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].ଚାବି_salt,
                           ଆକାର(scache->peers[i].ଚାବି_salt),
                           (ଅଚଳ unsigned ଅକ୍ଷର *)ssl_peer_ଚାବି,
                           peer_ଚାବି_len,
                           my_hmac);
      ଯଦି(result)
        ଯାଅ out;
      ଯଦି(!memcmp(scache->peers[i].ଚାବି_hmac, my_hmac, ଆକାର(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_ଚାବି);
        scache->peers[i].ssl_peer_ଚାବି = curlx_strdup(ssl_peer_ଚାବି);
        ଯଦି(!scache->peers[i].ssl_peer_ଚାବି) {
          result = CURLE_OUT_OF_MEMORY;
          ଯାଅ out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        ଯାଅ out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_ଚାବି);
out:
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ସଂରଚନା Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  ସଂରଚନା Curl_ssl_scache *scache)
{
  ସଂରଚନା Curl_ssl_scache_peer *peer = ଖାଲି;
  size_t i;

  /* find empty or oldest peer */
  ପାଇଁ(i = 0; i < scache->peer_ଗଣନା; ++i) {
    /* free peer entry? */
    ଯଦି(!scache->peers[i].ssl_peer_ଚାବି && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      ଭାଙ୍ଗ;
    }
    /* peer without sessions and obj */
    ଯଦି(!scache->peers[i].sobj &&
       !Curl_llist_ଗଣନା(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      ଭାଙ୍ଗ;
    }
    /* remember "oldest" peer */
    ଯଦି(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  ଯଦି(peer)
    cf_ssl_scache_clear_peer(peer);
  ଫେରନ୍ତୁ peer;
}

ସ୍ଥିର CURLcode cf_ssl_add_peer(ସଂରଚନା Curl_easy *data,
                                ସଂରଚନା Curl_ssl_scache *scache,
                                ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                ସଂରଚନା ssl_filter_config *conn_config,
                                ସଂରଚନା Curl_ssl_scache_peer **ppeer)
{
  ସଂରଚନା Curl_ssl_scache_peer *peer = ଖାଲି;
  CURLcode result = CURLE_OK;

  *ppeer = ଖାଲି;
  ଯଦି(ssl_peer_ଚାବି) {
    result = cf_ssl_find_peer_by_ଚାବି(data, scache, ssl_peer_ଚାବି, conn_config,
                                     &peer);
    ଯଦି(result || !scache->peer_ଗଣନା)
      ଫେରନ୍ତୁ result;
  }

  ଯଦି(peer) {
    *ppeer = peer;
    ଫେରନ୍ତୁ CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  ଯଦି(peer) {
    ଅକ୍ଷର buffer[64];
    ଅଚଳ ଅକ୍ଷର *ccert = conn_config ? conn_config->clientcert : ଖାଲି;
    ଅଚଳ ଅକ୍ଷର *kpasswd = conn_config ? conn_config->ଚାବି_passwd : ଖାଲି;
    ଯଦି(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, ଆକାର(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_ଚାବି, ccert, kpasswd,
                                     ଖାଲି, ଖାଲି);
    ଯଦି(result)
      ଯାଅ out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  ଯଦି(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ cf_scache_peer_add_session(ସଂରଚନା Curl_ssl_scache_peer *peer,
                                       ସଂରଚନା Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  ଯଦି(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, ଖାଲି);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  ଅନ୍ୟଥା {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    ଯେତେବେଳେ(Curl_llist_ଗଣନା(&peer->sessions) > peer->max_sessions) {
      Curl_ଗଣ୍ଠି_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

ସ୍ଥିର CURLcode cf_scache_add_session(ସଂରଚନା Curl_cfilter *cf,
                                      ସଂରଚନା Curl_easy *data,
                                      ସଂରଚନା Curl_ssl_scache *scache,
                                      ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                      ସଂରଚନା Curl_ssl_session *s)
{
  ସଂରଚନା Curl_ssl_scache_peer *peer = ଖାଲି;
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(ଖାଲି);
  curl_off_t max_lifetime;

  ଯଦି(!scache || !scache->peer_ଗଣନା) {
    Curl_ssl_session_destroy(s);
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  ଯଦି(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  ଯଦି(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    ଫେରନ୍ତୁ CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_ଚାବି, conn_config, &peer);
  ଯଦି(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    Curl_ssl_session_destroy(s);
    ଯାଅ out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  ଯଦି(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_ଚାବି, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
  }
  ଅନ୍ୟଥା
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_ଚାବି,
                  (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_ଗଣନା(&peer->sessions) : 0);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_ssl_scache_put(ସଂରଚନା Curl_cfilter *cf,
                             ସଂରଚନା Curl_easy *data,
                             ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                             ସଂରଚନା Curl_ssl_session *s)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  ଯଦି(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    ଫେରନ୍ତୁ CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_ଚାବି, s);
  Curl_ssl_scache_unlock(data);
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ Curl_ssl_scache_return(ସଂରଚନା Curl_cfilter *cf,
                            ସଂରଚନା Curl_easy *data,
                            ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                            ସଂରଚନା Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  ଯଦି(s && s->ietf_tls_id < 0x304)
    (ଶୂନ୍ୟ)Curl_ssl_scache_put(cf, data, ssl_peer_ଚାବି, s);
  ଅନ୍ୟଥା
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                              ସଂରଚନା Curl_ssl_session **ps)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା Curl_ssl_scache_peer *peer = ଖାଲି;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
  ସଂରଚନା Curl_ssl_session *s = ଖାଲି;
  CURLcode result;

  *ps = ଖାଲି;
  ଯଦି(!scache)
    ଫେରନ୍ତୁ CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_ଚାବି(data, scache, ssl_peer_ଚାବି, conn_config,
                                   &peer);
  ଯଦି(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(ଖାଲି));
    n = Curl_llist_head(&peer->sessions);
    ଯଦି(n) {
      s = Curl_ଗଣ୍ଠି_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  ଯଦି(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_ଚାବି, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_ଗଣନା(&peer->sessions));
  }
  ଅନ୍ୟଥା {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_ଚାବି);
  }
  Curl_ssl_scache_unlock(data);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_ssl_scache_add_obj(ସଂରଚନା Curl_cfilter *cf,
                                 ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                 ଶୂନ୍ୟ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା Curl_ssl_scache_peer *peer = ଖାଲି;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  ଯଦି(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_ଚାବି, conn_config, &peer);
  ଯଦି(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)result);
    ଯାଅ out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = ଖାଲି;  /* peer took ownership */

out:
  ଯଦି(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ *Curl_ssl_scache_get_obj(ସଂରଚନା Curl_cfilter *cf,
                              ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା Curl_ssl_scache_peer *peer = ଖାଲି;
  CURLcode result;
  ଶୂନ୍ୟ *sobj;

  ଯଦି(!scache)
    ଫେରନ୍ତୁ ଖାଲି;

  result = cf_ssl_find_peer_by_ଚାବି(data, scache, ssl_peer_ଚାବି, conn_config,
                                   &peer);
  ଯଦି(result)
    ଫେରନ୍ତୁ ଖାଲି;

  sobj = peer ? peer->sobj : ଖାଲି;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_ଚାବି);
  ଫେରନ୍ତୁ sobj;
}

ଶୂନ୍ୟ Curl_ssl_scache_remove_all(ସଂରଚନା Curl_cfilter *cf,
                                ସଂରଚନା Curl_easy *data,
                                ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ସଂରଚନା ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ସଂରଚନା Curl_ssl_scache_peer *peer = ଖାଲି;
  CURLcode result;

  (ଶୂନ୍ୟ)cf;
  ଯଦି(!scache)
    ଫେରନ୍ତୁ;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_ଚାବି(data, scache, ssl_peer_ଚାବି, conn_config,
                                   &peer);
  ଯଦି(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

ସ୍ଥିର CURLcode cf_ssl_scache_peer_set_hmac(ସଂରଚନା Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  ଯଦି(!peer->ssl_peer_ଚାବି)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(ଖାଲି, peer->ଚାବି_salt, ଆକାର(peer->ଚାବି_salt));
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->ଚାବି_salt, ଆକାର(peer->ଚାବି_salt),
                       (ଅଚଳ unsigned ଅକ୍ଷର *)peer->ssl_peer_ଚାବି,
                       strlen(peer->ssl_peer_ଚାବି),
                       peer->ଚାବି_hmac);
  ଯଦି(!result)
    peer->hmac_set = TRUE;
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode cf_ssl_find_peer_by_hmac(ସଂରଚନା Curl_ssl_scache *scache,
                                         ଅଚଳ unsigned ଅକ୍ଷର *salt,
                                         ଅଚଳ unsigned ଅକ୍ଷର *hmac,
                                         ସଂରଚନା Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = ଖାଲି;
  ଯଦି(!GOOD_SCACHE(scache))
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  ପାଇଁ(i = 0; scache && i < scache->peer_ଗଣନା; i++) {
    ସଂରଚନା Curl_ssl_scache_peer *peer = &scache->peers[i];
    ଯଦି(!cf_ssl_scache_match_auth(peer, ଖାଲି))
      ଜାରି;
    ଯଦି(scache->peers[i].hmac_set &&
       !memcmp(peer->ଚାବି_salt, salt, ଆକାର(peer->ଚାବି_salt)) &&
       !memcmp(peer->ଚାବି_hmac, hmac, ଆକାର(peer->ଚାବି_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      ଯାଅ out;
    }
    ଅନ୍ୟଥା ଯଦି(peer->ssl_peer_ଚାବି) {
      unsigned ଅକ୍ଷର my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, ଆକାର(peer->ଚାବି_salt),
                           (ଅଚଳ unsigned ଅକ୍ଷର *)peer->ssl_peer_ଚାବି,
                           strlen(peer->ssl_peer_ଚାବି),
                           my_hmac);
      ଯଦି(result)
        ଯାଅ out;
      ଯଦି(!memcmp(my_hmac, hmac, ଆକାର(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        ଯଦି(!peer->hmac_set) {
          memcpy(peer->ଚାବି_salt, salt, ଆକାର(peer->ଚାବି_salt));
          memcpy(peer->ଚାବି_hmac, hmac, ଆକାର(peer->ଚାବି_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        ଯାଅ out;
      }
    }
  }
out:
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_ssl_session_import(ସଂରଚନା Curl_easy *data,
                                 ଅଚଳ ଅକ୍ଷର *ssl_peer_ଚାବି,
                                 ଅଚଳ unsigned ଅକ୍ଷର *shmac, size_t shmac_len,
                                 ଅଚଳ ଶୂନ୍ୟ *sdata, size_t sdata_len)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ସଂରଚନା Curl_ssl_scache_peer *peer = ଖାଲି;
  ସଂରଚନା Curl_ssl_session *s = ଖାଲି;
  bool have_lock = FALSE;
  CURLcode result;

  ଯଦି(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }
  ଯଦି(!ssl_peer_ଚାବି && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  ଯଦି(result)
    ଯାଅ out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  ଯଦି(ssl_peer_ଚାବି) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_ଚାବି, ଖାଲି, &peer);
    ଯଦି(result)
      ଯାଅ out;
  }
  ଅନ୍ୟଥା ଯଦି(shmac_len != (ଆକାର(peer->ଚାବି_salt) + ଆକାର(peer->ଚାବି_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ଯାଅ out;
  }
  ଅନ୍ୟଥା {
    ଅଚଳ unsigned ଅକ୍ଷର *salt = shmac;
    ଅଚଳ unsigned ଅକ୍ଷର *hmac = shmac + ଆକାର(peer->ଚାବି_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    ଯଦି(result)
      ଯାଅ out;
    ଯଦି(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      ଯଦି(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_ଚାବି, ଖାଲି, ଖାଲି,
                                         salt, hmac);
        ଯଦି(result)
          ଯାଅ out;
      }
    }
  }

  ଯଦି(peer) {
    cf_scache_peer_add_session(peer, s, time(ଖାଲି));
    s = ଖାଲି; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_ଚାବି ? peer->ssl_peer_ଚାବି : "without key",
                  Curl_llist_ଗଣନା(&peer->sessions));
  }

out:
  ଯଦି(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  ଫେରନ୍ତୁ result;
}

CURLcode Curl_ssl_session_export(ସଂରଚନା Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 ଶୂନ୍ୟ *userptr)
{
  ସଂରଚନା Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ସଂରଚନା Curl_ssl_scache_peer *peer;
  ସଂରଚନା dynbuf sbuf, hbuf;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
  size_t i;
  curl_off_t now = time(ଖାଲି);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  ଯଦି(!export_fn)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  ପାଇଁ(i = 0; scache && i < scache->peer_ଗଣନା; i++) {
    peer = &scache->peers[i];
    ଯଦି(!peer->ssl_peer_ଚାବି && !peer->hmac_set)
      ଜାରି; /* skip free entry */
    ଯଦି(!peer->exportable)
      ଜାରି;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    ଯଦି(n)
      VERBOSE(++npeers);
    ଯେତେବେଳେ(n) {
      ସଂରଚନା Curl_ssl_session *s = Curl_ଗଣ୍ଠି_elem(n);
      ଯଦି(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        ଯଦି(result)
          ଯାଅ out;
      }
      ଯଦି(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->ଚାବି_salt, ଆକାର(peer->ଚାବି_salt));
        ଯଦି(result)
          ଯାଅ out;
        result = curlx_dyn_addn(&hbuf, peer->ଚାବି_hmac, ଆକାର(peer->ଚାବି_hmac));
        ଯଦି(result)
          ଯାଅ out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      ଯଦି(result)
        ଯାଅ out;

      result = export_fn(data, userptr, peer->ssl_peer_ଚାବି,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      ଯଦି(result)
        ଯାଅ out;
      VERBOSE(++ntickets);
      n = Curl_ଗଣ୍ଠି_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  ଫେରନ୍ତୁ result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
