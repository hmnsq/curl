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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_SSL

#అన్తర్భావ "urldata.h"
#అన్తర్భావ "cfilters.h"

#అన్తర్భావ "vtls/vtls.h" /* generic SSL protos etc */
#అన్తర్భావ "vtls/vtls_int.h"
#అన్తర్భావ "vtls/vtls_scache.h"
#అన్తర్భావ "vtls/vtls_spack.h"

#అన్తర్భావ "strcase.h"
#అన్తర్భావ "url.h"
#అన్తర్భావ "llist.h"
#అన్తర్భావ "curl_share.h"
#అన్తర్భావ "curl_threads.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "curl_sha256.h"
#అన్తర్భావ "rand.h"
#అన్తర్భావ "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
నిర్మాణం Curl_ssl_scache_peer {
  అక్షరం *ssl_peer_కీలకం;      /* id for peer + relevant TLS configuration */
  అక్షరం *clientcert;
  అక్షరం *కీలకం_passwd;
  నిర్మాణం Curl_llist sessions;
  శూన్య *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  గుర్తులేని అక్షరం కీలకం_salt[CURL_SHA256_DIGEST_నిడివి]; /* for entry export */
  గుర్తులేని అక్షరం కీలకం_hmac[CURL_SHA256_DIGEST_నిడివి]; /* for entry export */
  size_t max_sessions;
  దీర్ఘ age;                /* a number, the higher the more recent */
  బిట్టు(hmac_సమితి);           /* if key_salt and key_hmac are present */
  బిట్టు(exportable);         /* sessions for this peer can be exported */
};

#నిర్వచించు CURL_SCACHE_MAGIC 0x000e1551

#నిర్వచించు GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
స్థిర CURLcode cf_ssl_peer_కీలకం_కూడు_string(నిర్మాణం dynbuf *buf,
                                           స్థిరరాశి అక్షరం *name,
                                           స్థిరరాశి అక్షరం *విలువ)
{
  తిరిగి curlx_dyn_addf(buf, ":%s-%zu:%s", name, అక్షరశ్రేణి_పొడవు(విలువ), విలువ);
}

స్థిర CURLcode cf_ssl_peer_కీలకం_కూడు_మార్గం(నిర్మాణం dynbuf *buf,
                                         స్థిరరాశి అక్షరం *name,
                                         స్థిరరాశి అక్షరం *మార్గం,
                                         bool *is_local)
{
  ఒకవేళ(మార్గం && మార్గం[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#నిర్వచితమైతే _WIN32
    అక్షరం *abspath = _fullpath(ఖాళీ, మార్గం, 0);
    ఒకవేళ(abspath) {
      CURLcode ఫలితం = cf_ssl_peer_కీలకం_కూడు_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      విడుదల(abspath); /* allocated by CRT, use system free() */
      తిరిగి ఫలితం;
    }
    *is_local = TRUE;
#లేదావేళ defined(HAVE_REALPATH)
    ఒకవేళ(మార్గం[0] != '/') {
      అక్షరం *abspath = realpath(మార్గం, ఖాళీ);
      ఒకవేళ(abspath) {
        CURLcode ఫలితం = cf_ssl_peer_కీలకం_కూడు_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        విడుదల(abspath); /* allocated by libc, free without memdebug */
        తిరిగి ఫలితం;
      }
      *is_local = TRUE;
    }
#అయితే_ముగింపు
    తిరిగి cf_ssl_peer_కీలకం_కూడు_string(buf, name, మార్గం);
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode cf_ssl_peer_కీలకం_కూడు_hash(నిర్మాణం dynbuf *buf,
                                         స్థిరరాశి అక్షరం *name,
                                         నిర్మాణం curl_blob *blob)
{
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(blob && blob->len) {
    గుర్తులేని అక్షరం hash[CURL_SHA256_DIGEST_నిడివి];
    size_t i;

    ఫలితం = curlx_dyn_addf(buf, ":%s-", name);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఫలితం = Curl_sha256it(hash, blob->దత్తాంశం, blob->len);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    కొరకు(i = 0; i < CURL_SHA256_DIGEST_నిడివి; ++i) {
      ఫలితం = curlx_dyn_addf(buf, "%02x", hash[i]);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
  }
బహిర్గతం:
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_ssl_peer_కీలకం_కూడు_mtls(నిర్మాణం dynbuf *buf,
                                         నిర్మాణం ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode ఫలితం = CURLE_OK;
  ఒకవేళ(ssl->clientcert && ssl->clientcert[0]) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_మార్గం(buf, "CCERT", ssl->clientcert, is_local);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->కీలకం && ssl->కీలకం[0]) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_మార్గం(buf, "KEY", ssl->కీలకం, is_local);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->కీలకం_blob) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_hash(buf, "KEYBlob", ssl->కీలకం_blob);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    ఫలితం = curlx_dyn_addf(buf, ":CT-%zu:", అక్షరశ్రేణి_పొడవు(ssl->cert_type));
    కొరకు(i = 0; !ఫలితం && ssl->cert_type[i]; i++) {
      అక్షరం c = Curl_raw_toupper(ssl->cert_type[i]);
      ఫలితం = curlx_dyn_addn(buf, &c, 1);
    }
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->కీలకం_type && ssl->కీలకం_type[0]) {
    size_t i;
    ఫలితం = curlx_dyn_addf(buf, ":KT-%zu:", అక్షరశ్రేణి_పొడవు(ssl->కీలకం_type));
    కొరకు(i = 0; !ఫలితం && ssl->కీలకం_type[i]; i++) {
      అక్షరం c = Curl_raw_toupper(ssl->కీలకం_type[i]);
      ఫలితం = curlx_dyn_addn(buf, &c, 1);
    }
  }
బహిర్గతం:
  తిరిగి ఫలితం;
}

#నిర్వచించు CURL_SSLS_LOCAL_SUFFIX     ":L"
#నిర్వచించు CURL_SSLS_GLOBAL_SUFFIX    ":G"

స్థిర bool cf_ssl_peer_కీలకం_is_global(స్థిరరాశి అక్షరం *peer_కీలకం)
{
  size_t len = peer_కీలకం ? అక్షరశ్రేణి_పొడవు(peer_కీలకం) : 0;
  తిరిగి (len > 2) &&
         (peer_కీలకం[len - 1] == 'G') &&
         (peer_కీలకం[len - 2] == ':');
}

స్థిర CURLcode ssl_peer_కీలకం_కూడు_transport(నిర్మాణం dynbuf *buf,
                                           uint8_t transport)
{
  ఎంపిక(transport) {
  స్థితి TRNSPRT_TCP:
    తిరిగి CURLE_OK;
  స్థితి TRNSPRT_UDP:
    తిరిగి curlx_dyn_కూడు(buf, ":UDP");
  స్థితి TRNSPRT_QUIC:
    తిరిగి curlx_dyn_కూడు(buf, ":QUIC");
  స్థితి TRNSPRT_UNIX:
    తిరిగి curlx_dyn_కూడు(buf, ":UNIX");
  అప్రమేయ:
    తిరిగి curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

స్థిర CURLcode ssl_peer_కీలకం_కూడు_vrfy(నిర్మాణం dynbuf *buf,
                                      నిర్మాణం ssl_filter_config *ssl,
                                      స్థిరరాశి నిర్మాణం ssl_peer *peer)
{
  CURLcode ఫలితం;

  ఒకవేళ(!ssl->verifypeer) {
    ఫలితం = curlx_dyn_కూడు(buf, ":NO-VRFY-PEER");
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  ఒకవేళ(!ssl->verifyhost) {
    ఫలితం = curlx_dyn_కూడు(buf, ":NO-VRFY-HOST");
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  ఒకవేళ(ssl->verifystatus) {
    ఫలితం = curlx_dyn_కూడు(buf, ":VRFY-STATUS");
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  ఒకవేళ((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_string(buf, "CHOST", peer->peer->hostname);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->పోర్టు);
    ఒకవేళ(ఫలితం)
      తిరిగి ఫలితం;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode ssl_peer_కీలకం_build(నిర్మాణం ssl_filter_config *ssl,
                                   స్థిరరాశి నిర్మాణం ssl_peer *peer,
                                   స్థిరరాశి అక్షరం *tls_id,
                                   అక్షరం **ppeer_కీలకం)
{
  నిర్మాణం dynbuf buf;
  size_t కీలకం_len;
  bool is_local = FALSE;
  CURLcode ఫలితం;

  *ppeer_కీలకం = ఖాళీ;
  curlx_dyn_init(&buf, 10 * 1024);

  ఫలితం = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->పోర్టు);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఫలితం = ssl_peer_కీలకం_కూడు_transport(&buf, peer->transport);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఫలితం = ssl_peer_కీలకం_కూడు_vrfy(&buf, ssl, peer);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;
  ఒకవేళ(ssl->సంస్కరణ_2 || ssl->సంస్కరణ_max) {
    ఫలితం = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->సంస్కరణ_2,
                            (ssl->సంస్కరణ_max >> 16));
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->ssl_options) {
    ఫలితం = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->cipher_list) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_string(&buf, "CIPHER", ssl->cipher_list);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->cipher_list13) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_string(&buf, "CIPHER13", ssl->cipher_list13);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->curves) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_string(&buf, "CURVES", ssl->curves);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->signature_algorithms) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->verifypeer) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_మార్గం(&buf, "CA", ssl->CAfile, &is_local);
    ఒకవేళ(!ఫలితం)
      ఫలితం = cf_ssl_peer_కీలకం_కూడు_మార్గం(&buf, "CApath",
                                        ssl->CApath, &is_local);
    ఒకవేళ(!ఫలితం && ssl->native_ca_store)
      ఫలితం = curlx_dyn_కూడు(&buf, ":NATIVE-CA");
    ఒకవేళ(!ఫలితం)
      ఫలితం = cf_ssl_peer_కీలకం_కూడు_మార్గం(&buf, "CRL", ssl->CRLfile, &is_local);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_మార్గం(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఒకవేళ(ssl->ca_info_blob) {
      ఫలితం = cf_ssl_peer_కీలకం_కూడు_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ(ssl->issuercert_blob) {
      ఫలితం = cf_ssl_peer_కీలకం_కూడు_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
    }
  }
  ఒకవేళ(ssl->cert_blob) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_hash(&buf, "CertBlob", ssl->cert_blob);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(ssl->pinned_కీలకం && ssl->pinned_కీలకం[0]) {
    ఫలితం = cf_ssl_peer_కీలకం_కూడు_string(&buf, "Pinned", ssl->pinned_కీలకం);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }

  ఫలితం = cf_ssl_peer_కీలకం_కూడు_mtls(&buf, ssl, &is_local);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఒకవేళ(!tls_id || !tls_id[0]) {
    ఫలితం = CURLE_FAILED_INIT;
    వెళ్ళు బహిర్గతం;
  }
  ఫలితం = cf_ssl_peer_కీలకం_కూడు_string(&buf, "IMPL", tls_id);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  ఫలితం = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  *ppeer_కీలకం = curlx_dyn_take(&buf, &కీలకం_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

బహిర్గతం:
  curlx_dyn_free(&buf);
  తిరిగి ఫలితం;
}

CURLcode Curl_ssl_peer_కీలకం_make(స్థిరరాశి నిర్మాణం ssl_peer *peer,
                                నిర్మాణం ssl_filter_config *sslc,
                                స్థిరరాశి అక్షరం *tls_id,
                                అక్షరం **ppeer_కీలకం)
{
  తిరిగి ssl_peer_కీలకం_build(sslc, peer, tls_id, ppeer_కీలకం);
}

నిర్మాణం Curl_ssl_scache {
  గుర్తులేని పూర్ణాంకం magic;
  నిర్మాణం Curl_ssl_scache_peer *peers;
  size_t peer_లెక్క;
  పూర్ణాంకం default_lifetime_secs;
  దీర్ఘ age;
#నిర్వచితమైతే USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#అయితే_ముగింపు
  బిట్టు(is_locked);
};

స్థిర నిర్మాణం Curl_ssl_scache *cf_ssl_scache_get(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_ssl_scache *scache = ఖాళీ;
  /* If a share is present, its ssl_scache has preference over the multi */
  ఒకవేళ(దత్తాంశం->share && దత్తాంశం->share->ssl_scache)
    scache = దత్తాంశం->share->ssl_scache;
  లేకపోతే ఒకవేళ(దత్తాంశం->multi && దత్తాంశం->multi->ssl_scache)
    scache = దత్తాంశం->multi->ssl_scache;
  ఒకవేళ(scache && !GOOD_SCACHE(scache)) {
    failf(దత్తాంశం, "transfer would use an invalid scache at %p, denied",
          (శూన్య *)scache);
    DEBUGASSERT(0);
    తిరిగి ఖాళీ;
  }
  తిరిగి scache;
}

స్థిర శూన్య cf_ssl_scache_session_ldestroy(శూన్య *udata, శూన్య *obj)
{
  నిర్మాణం Curl_ssl_session *s = obj;
  (శూన్య)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(శూన్య *sdata, size_t sdata_len,
                                 పూర్ణాంకం ietf_tls_id, స్థిరరాశి అక్షరం *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 నిర్మాణం Curl_ssl_session **psession)
{
  తిరిగి Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  ఖాళీ, 0, psession);
}

CURLcode Curl_ssl_session_create2(శూన్య *sdata, size_t sdata_len,
                                  పూర్ణాంకం ietf_tls_id, స్థిరరాశి అక్షరం *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  గుర్తులేని అక్షరం *quic_tp, size_t quic_tp_len,
                                  నిర్మాణం Curl_ssl_session **psession)
{
  నిర్మాణం Curl_ssl_session *s;

  ఒకవేళ(!sdata || !sdata_len) {
    curlx_free(sdata);
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  *psession = ఖాళీ;
  s = curlx_calloc(1, పరిమాణం(*s));
  ఒకవేళ(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  ఒకవేళ(alpn) {
    s->alpn = curlx_strdup(alpn);
    ఒకవేళ(!s->alpn) {
      cf_ssl_scache_session_ldestroy(ఖాళీ, s);
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
  }
  *psession = s;
  తిరిగి CURLE_OK;
}

శూన్య Curl_ssl_session_destroy(నిర్మాణం Curl_ssl_session *s)
{
  ఒకవేళ(s) {
    /* if in the list, the list destructor takes care of it */
    ఒకవేళ(Curl_కణుపు_llist(&s->list))
      Curl_కణుపు_remove(&s->list);
    లేకపోతే {
      cf_ssl_scache_session_ldestroy(ఖాళీ, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(నిర్మాణం Curl_ssl_session *src,
                              నిర్మాణం Curl_ssl_session **pdest)
{
  నిర్మాణం Curl_ssl_session *dest = ఖాళీ;
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  ఒకవేళ(!src || !pdest)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  *pdest = ఖాళీ;

  dest = curlx_calloc(1, పరిమాణం(*dest));
  ఒకవేళ(!dest)
    వెళ్ళు బహిర్గతం;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  ఒకవేళ(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    ఒకవేళ(!dest->sdata)
      వెళ్ళు బహిర్గతం;
    dest->sdata_len = src->sdata_len;
  }
  ఒకవేళ(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    ఒకవేళ(!dest->quic_tp)
      వెళ్ళు బహిర్గతం;
    dest->quic_tp_len = src->quic_tp_len;
  }
  ఒకవేళ(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    ఒకవేళ(!dest->alpn)
      వెళ్ళు బహిర్గతం;
  }
  ఫలితం = CURLE_OK;

బహిర్గతం:
  ఒకవేళ(!ఫలితం)
    *pdest = dest;
  లేకపోతే {
    *pdest = ఖాళీ;
    ఒకవేళ(dest)
      Curl_ssl_session_destroy(dest);
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_ssl_scache_clear_peer(నిర్మాణం Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, ఖాళీ);
  ఒకవేళ(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    ఒకవేళ(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = ఖాళీ;
  }
  peer->sobj_free = ఖాళీ;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->కీలకం_passwd);
  curlx_safefree(peer->ssl_peer_కీలకం);
  peer->age = 0;
  peer->hmac_సమితి = FALSE;
}

స్థిర శూన్య cf_ssl_scache_peer_సమితి_obj(నిర్మాణం Curl_ssl_scache_peer *peer,
                                       శూన్య *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  ఒకవేళ(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

స్థిర శూన్య cf_ssl_cache_peer_update(నిర్మాణం Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->కీలకం_passwd &&
                      (!peer->ssl_peer_కీలకం ||
                       cf_ssl_peer_కీలకం_is_global(peer->ssl_peer_కీలకం)));
}

స్థిర CURLcode cf_ssl_scache_peer_init(నిర్మాణం Curl_ssl_scache_peer *peer,
                                        స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                                        స్థిరరాశి అక్షరం *clientcert,
                                        స్థిరరాశి అక్షరం *కీలకం_passwd,
                                        స్థిరరాశి గుర్తులేని అక్షరం *salt,
                                        స్థిరరాశి గుర్తులేని అక్షరం *hmac)
{
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  DEBUGASSERT(!peer->ssl_peer_కీలకం);
  ఒకవేళ(ssl_peer_కీలకం) {
    peer->ssl_peer_కీలకం = curlx_strdup(ssl_peer_కీలకం);
    ఒకవేళ(!peer->ssl_peer_కీలకం)
      వెళ్ళు బహిర్గతం;
    peer->hmac_సమితి = FALSE;
  }
  లేకపోతే ఒకవేళ(salt && hmac) {
    జ్ఞాపకం_నకలు(peer->కీలకం_salt, salt, పరిమాణం(peer->కీలకం_salt));
    జ్ఞాపకం_నకలు(peer->కీలకం_hmac, hmac, పరిమాణం(peer->కీలకం_hmac));
    peer->hmac_సమితి = TRUE;
  }
  లేకపోతే {
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    ఒకవేళ(!peer->clientcert)
      వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(కీలకం_passwd) {
    peer->కీలకం_passwd = curlx_strdup(కీలకం_passwd);
    ఒకవేళ(!peer->కీలకం_passwd)
      వెళ్ళు బహిర్గతం;
  }
  cf_ssl_cache_peer_update(peer);
  ఫలితం = CURLE_OK;
బహిర్గతం:
  ఒకవేళ(ఫలితం)
    cf_ssl_scache_clear_peer(peer);
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_scache_session_remove(నిర్మాణం Curl_ssl_scache_peer *peer,
                                     నిర్మాణం Curl_ssl_session *s)
{
  (శూన్య)peer;
  DEBUGASSERT(Curl_కణుపు_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

స్థిర bool cf_scache_session_expired(నిర్మాణం Curl_ssl_session *s,
                                      curl_off_t now)
{
  తిరిగి (s->valid_until > 0) && (s->valid_until < now);
}

స్థిర శూన్య cf_scache_peer_remove_expired(నిర్మాణం Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  నిర్మాణం Curl_llist_కణుపు *n = Curl_llist_తల(&peer->sessions);
  అయితే(n) {
    నిర్మాణం Curl_ssl_session *s = Curl_కణుపు_elem(n);
    n = Curl_కణుపు_తదుపరి(n);
    ఒకవేళ(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

స్థిర శూన్య cf_scache_peer_remove_non13(నిర్మాణం Curl_ssl_scache_peer *peer)
{
  నిర్మాణం Curl_llist_కణుపు *n = Curl_llist_తల(&peer->sessions);
  అయితే(n) {
    నిర్మాణం Curl_ssl_session *s = Curl_కణుపు_elem(n);
    n = Curl_కణుపు_తదుపరి(n);
    ఒకవేళ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_చొప్పున_peer,
                                నిర్మాణం Curl_ssl_scache **pscache)
{
  నిర్మాణం Curl_ssl_scache *scache;
  నిర్మాణం Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = ఖాళీ;
  peers = curlx_calloc(max_peers, పరిమాణం(*peers));
  ఒకవేళ(!peers)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  scache = curlx_calloc(1, పరిమాణం(*scache));
  ఒకవేళ(!scache) {
    curlx_free(peers);
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_లెక్క = max_peers;
  scache->peers = peers;
  scache->age = 1;
  కొరకు(i = 0; i < scache->peer_లెక్క; ++i) {
    scache->peers[i].max_sessions = max_sessions_చొప్పున_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#నిర్వచితమైతే USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#అయితే_ముగింపు
  *pscache = scache;
  తిరిగి CURLE_OK;
}

శూన్య Curl_ssl_scache_destroy(నిర్మాణం Curl_ssl_scache *scache)
{
  ఒకవేళ(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    కొరకు(i = 0; i < scache->peer_లెక్క; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#నిర్వచితమైతే USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#అయితే_ముగింపు
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(నిర్మాణం Curl_cfilter *cf, నిర్మాణం Curl_easy *దత్తాంశం)
{
  ఒకవేళ(cf_ssl_scache_get(దత్తాంశం)) {
    నిర్మాణం ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    తిరిగి conn_config ? conn_config->cache_session : FALSE;
  }
  తిరిగి FALSE;
}

/* Lock shared SSL session data */
శూన్య Curl_ssl_scache_తాళం(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  ఒకవేళ(scache) {
    ఒకవేళ(CURL_SHARE_ssl_scache(దత్తాంశం))
      Curl_share_తాళం(దత్తాంశం, CURL_తాళం_దత్తాంశం_SSL_SESSION,
                      CURL_తాళం_ACCESS_SINGLE);
#నిర్వచితమైతే USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#లేకపోతే
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#అయితే_ముగింపు
  }
}

/* Unlock shared SSL session data */
శూన్య Curl_ssl_scache_unlock(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  ఒకవేళ(scache) {
#నిర్వచితమైతే USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#లేకపోతే
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#అయితే_ముగింపు
    ఒకవేళ(CURL_SHARE_ssl_scache(దత్తాంశం))
      Curl_share_unlock(దత్తాంశం, CURL_తాళం_దత్తాంశం_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  bool locked = FALSE;
  ఒకవేళ(!scache)
    తిరిగి FALSE;
#నిర్వచితమైతే USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#లేకపోతే
  locked = (bool)scache->is_locked;
#అయితే_ముగింపు
  తిరిగి locked;
}

స్థిర bool cf_ssl_scache_సరిపోలిక_auth(నిర్మాణం Curl_ssl_scache_peer *peer,
                                     నిర్మాణం ssl_filter_config *conn_config)
{
  ఒకవేళ(!conn_config) {
    ఒకవేళ(peer->clientcert || peer->కీలకం_passwd)
      తిరిగి FALSE;
    తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    తిరిగి FALSE;
  ఒకవేళ(Curl_timestrcmp(peer->కీలకం_passwd, conn_config->కీలకం_passwd))
    తిరిగి FALSE;
  తిరిగి TRUE;
}

స్థిర CURLcode cf_ssl_కనుగొను_peer_by_కీలకం(నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం Curl_ssl_scache *scache,
                                        స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                                        నిర్మాణం ssl_filter_config *conn_config,
                                        నిర్మాణం Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_కీలకం_len = 0;
  CURLcode ఫలితం = CURLE_OK;

  *ppeer = ఖాళీ;
  ఒకవేళ(!GOOD_SCACHE(scache)) {
    తిరిగి CURLE_BAD_FUNCTION_వాదం;
  }

  CURL_TRC_SSLS(దత్తాంశం, "find peer slot for %s among %zu slots",
                ssl_peer_కీలకం, scache->peer_లెక్క);

  /* check for entries with known peer_key */
  కొరకు(i = 0; scache && i < scache->peer_లెక్క; i++) {
    ఒకవేళ(scache->peers[i].ssl_peer_కీలకం &&
       !strcmp(ssl_peer_కీలకం, scache->peers[i].ssl_peer_కీలకం) &&
       cf_ssl_scache_సరిపోలిక_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      వెళ్ళు బహిర్గతం;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  కొరకు(i = 0; scache && i < scache->peer_లెక్క; i++) {
    ఒకవేళ(!scache->peers[i].ssl_peer_కీలకం &&
       scache->peers[i].hmac_సమితి &&
       cf_ssl_scache_సరిపోలిక_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      గుర్తులేని అక్షరం my_hmac[CURL_SHA256_DIGEST_నిడివి];
      ఒకవేళ(!peer_కీలకం_len) /* we are lazy */
        peer_కీలకం_len = అక్షరశ్రేణి_పొడవు(ssl_peer_కీలకం);
      ఫలితం = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].కీలకం_salt,
                           పరిమాణం(scache->peers[i].కీలకం_salt),
                           (స్థిరరాశి గుర్తులేని అక్షరం *)ssl_peer_కీలకం,
                           peer_కీలకం_len,
                           my_hmac);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      ఒకవేళ(!memcmp(scache->peers[i].కీలకం_hmac, my_hmac, పరిమాణం(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(దత్తాంశం, "peer entry %zu key recovered: %s",
                      i, ssl_peer_కీలకం);
        scache->peers[i].ssl_peer_కీలకం = curlx_strdup(ssl_peer_కీలకం);
        ఒకవేళ(!scache->peers[i].ssl_peer_కీలకం) {
          ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
          వెళ్ళు బహిర్గతం;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        వెళ్ళు బహిర్గతం;
      }
    }
  }
  CURL_TRC_SSLS(దత్తాంశం, "peer not found for %s", ssl_peer_కీలకం);
బహిర్గతం:
  తిరిగి ఫలితం;
}

స్థిర నిర్మాణం Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  నిర్మాణం Curl_ssl_scache *scache)
{
  నిర్మాణం Curl_ssl_scache_peer *peer = ఖాళీ;
  size_t i;

  /* find empty or oldest peer */
  కొరకు(i = 0; i < scache->peer_లెక్క; ++i) {
    /* free peer entry? */
    ఒకవేళ(!scache->peers[i].ssl_peer_కీలకం && !scache->peers[i].hmac_సమితి) {
      peer = &scache->peers[i];
      ఆపు;
    }
    /* peer without sessions and obj */
    ఒకవేళ(!scache->peers[i].sobj &&
       !Curl_llist_లెక్క(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      ఆపు;
    }
    /* remember "oldest" peer */
    ఒకవేళ(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  ఒకవేళ(peer)
    cf_ssl_scache_clear_peer(peer);
  తిరిగి peer;
}

స్థిర CURLcode cf_ssl_కూడు_peer(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం Curl_ssl_scache *scache,
                                స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                                నిర్మాణం ssl_filter_config *conn_config,
                                నిర్మాణం Curl_ssl_scache_peer **ppeer)
{
  నిర్మాణం Curl_ssl_scache_peer *peer = ఖాళీ;
  CURLcode ఫలితం = CURLE_OK;

  *ppeer = ఖాళీ;
  ఒకవేళ(ssl_peer_కీలకం) {
    ఫలితం = cf_ssl_కనుగొను_peer_by_కీలకం(దత్తాంశం, scache, ssl_peer_కీలకం, conn_config,
                                     &peer);
    ఒకవేళ(ఫలితం || !scache->peer_లెక్క)
      తిరిగి ఫలితం;
  }

  ఒకవేళ(peer) {
    *ppeer = peer;
    తిరిగి CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  ఒకవేళ(peer) {
    అక్షరం బఫరు[64];
    స్థిరరాశి అక్షరం *ccert = conn_config ? conn_config->clientcert : ఖాళీ;
    స్థిరరాశి అక్షరం *kpasswd = conn_config ? conn_config->కీలకం_passwd : ఖాళీ;
    ఒకవేళ(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(బఫరు, పరిమాణం(బఫరు),
                     "cert-%p", conn_config->cert_blob->దత్తాంశం);
      ccert = బఫరు; /* data is strduped by cf_ssl_scache_peer_init */
    }
    ఫలితం = cf_ssl_scache_peer_init(peer, ssl_peer_కీలకం, ccert, kpasswd,
                                     ఖాళీ, ఖాళీ);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    /* all ready */
    *ppeer = peer;
    ఫలితం = CURLE_OK;
  }

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    cf_ssl_scache_clear_peer(peer);
  }
  తిరిగి ఫలితం;
}

స్థిర శూన్య cf_scache_peer_కూడు_session(నిర్మాణం Curl_ssl_scache_peer *peer,
                                       నిర్మాణం Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  ఒకవేళ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, ఖాళీ);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  లేకపోతే {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    అయితే(Curl_llist_లెక్క(&peer->sessions) > peer->max_sessions) {
      Curl_కణుపు_remove(Curl_llist_తల(&peer->sessions));
    }
  }
}

స్థిర CURLcode cf_scache_కూడు_session(నిర్మాణం Curl_cfilter *cf,
                                      నిర్మాణం Curl_easy *దత్తాంశం,
                                      నిర్మాణం Curl_ssl_scache *scache,
                                      స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                                      నిర్మాణం Curl_ssl_session *s)
{
  నిర్మాణం Curl_ssl_scache_peer *peer = ఖాళీ;
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
  curl_off_t now = (curl_off_t)సమయం(ఖాళీ);
  curl_off_t max_lifetime;

  ఒకవేళ(!scache || !scache->peer_లెక్క) {
    Curl_ssl_session_destroy(s);
    తిరిగి CURLE_OK;
  }

  ఒకవేళ(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  ఒకవేళ(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  ఒకవేళ(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(దత్తాంశం, "add, session already expired");
    Curl_ssl_session_destroy(s);
    తిరిగి CURLE_OK;
  }

  ఫలితం = cf_ssl_కూడు_peer(దత్తాంశం, scache, ssl_peer_కీలకం, conn_config, &peer);
  ఒకవేళ(ఫలితం || !peer) {
    CURL_TRC_SSLS(దత్తాంశం, "unable to add scache peer: %d", (పూర్ణాంకం)ఫలితం);
    Curl_ssl_session_destroy(s);
    వెళ్ళు బహిర్గతం;
  }

  cf_scache_peer_కూడు_session(peer, s, now);

బహిర్గతం:
  ఒకవేళ(ఫలితం) {
    failf(దత్తాంశం, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_కీలకం, (పూర్ణాంకం)ఫలితం);
  }
  లేకపోతే
    CURL_TRC_SSLS(దత్తాంశం, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_కీలకం,
                  (గుర్తులేని పూర్ణాంకం)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_లెక్క(&peer->sessions) : 0);
  తిరిగి ఫలితం;
}

CURLcode Curl_ssl_scache_put(నిర్మాణం Curl_cfilter *cf,
                             నిర్మాణం Curl_easy *దత్తాంశం,
                             స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                             నిర్మాణం Curl_ssl_session *s)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ఫలితం;
  DEBUGASSERT(conn_config);

  ఒకవేళ(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    తిరిగి CURLE_OK;
  }

  Curl_ssl_scache_తాళం(దత్తాంశం);
  ఫలితం = cf_scache_కూడు_session(cf, దత్తాంశం, scache, ssl_peer_కీలకం, s);
  Curl_ssl_scache_unlock(దత్తాంశం);
  తిరిగి ఫలితం;
}

శూన్య Curl_ssl_scache_return(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం,
                            స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                            నిర్మాణం Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  ఒకవేళ(s && s->ietf_tls_id < 0x304)
    (శూన్య)Curl_ssl_scache_put(cf, దత్తాంశం, ssl_peer_కీలకం, s);
  లేకపోతే
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                              నిర్మాణం Curl_ssl_session **ps)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం Curl_ssl_scache_peer *peer = ఖాళీ;
  నిర్మాణం Curl_llist_కణుపు *n;
  నిర్మాణం Curl_ssl_session *s = ఖాళీ;
  CURLcode ఫలితం;

  *ps = ఖాళీ;
  ఒకవేళ(!scache)
    తిరిగి CURLE_OK;

  Curl_ssl_scache_తాళం(దత్తాంశం);
  ఫలితం = cf_ssl_కనుగొను_peer_by_కీలకం(దత్తాంశం, scache, ssl_peer_కీలకం, conn_config,
                                   &peer);
  ఒకవేళ(!ఫలితం && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)సమయం(ఖాళీ));
    n = Curl_llist_తల(&peer->sessions);
    ఒకవేళ(n) {
      s = Curl_కణుపు_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  ఒకవేళ(s) {
    *ps = s;
    CURL_TRC_SSLS(దత్తాంశం, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_కీలకం, (గుర్తులేని పూర్ణాంకం)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_లెక్క(&peer->sessions));
  }
  లేకపోతే {
    CURL_TRC_SSLS(దత్తాంశం, "no cached session for %s", ssl_peer_కీలకం);
  }
  Curl_ssl_scache_unlock(దత్తాంశం);
  తిరిగి ఫలితం;
}

CURLcode Curl_ssl_scache_కూడు_obj(నిర్మాణం Curl_cfilter *cf,
                                 నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                                 శూన్య *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం Curl_ssl_scache_peer *peer = ఖాళీ;
  CURLcode ఫలితం;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  ఒకవేళ(!scache) {
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = cf_ssl_కూడు_peer(దత్తాంశం, scache, ssl_peer_కీలకం, conn_config, &peer);
  ఒకవేళ(ఫలితం || !peer) {
    CURL_TRC_SSLS(దత్తాంశం, "unable to add scache peer: %d", (పూర్ణాంకం)ఫలితం);
    వెళ్ళు బహిర్గతం;
  }

  cf_ssl_scache_peer_సమితి_obj(peer, sobj, sobj_dtor_cb);
  sobj = ఖాళీ;  /* peer took ownership */

బహిర్గతం:
  ఒకవేళ(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  తిరిగి ఫలితం;
}

శూన్య *Curl_ssl_scache_get_obj(నిర్మాణం Curl_cfilter *cf,
                              నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *ssl_peer_కీలకం)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం Curl_ssl_scache_peer *peer = ఖాళీ;
  CURLcode ఫలితం;
  శూన్య *sobj;

  ఒకవేళ(!scache)
    తిరిగి ఖాళీ;

  ఫలితం = cf_ssl_కనుగొను_peer_by_కీలకం(దత్తాంశం, scache, ssl_peer_కీలకం, conn_config,
                                   &peer);
  ఒకవేళ(ఫలితం)
    తిరిగి ఖాళీ;

  sobj = peer ? peer->sobj : ఖాళీ;

  CURL_TRC_SSLS(దత్తాంశం, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_కీలకం);
  తిరిగి sobj;
}

శూన్య Curl_ssl_scache_remove_all(నిర్మాణం Curl_cfilter *cf,
                                నిర్మాణం Curl_easy *దత్తాంశం,
                                స్థిరరాశి అక్షరం *ssl_peer_కీలకం)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  నిర్మాణం ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  నిర్మాణం Curl_ssl_scache_peer *peer = ఖాళీ;
  CURLcode ఫలితం;

  (శూన్య)cf;
  ఒకవేళ(!scache)
    తిరిగి;

  Curl_ssl_scache_తాళం(దత్తాంశం);
  ఫలితం = cf_ssl_కనుగొను_peer_by_కీలకం(దత్తాంశం, scache, ssl_peer_కీలకం, conn_config,
                                   &peer);
  ఒకవేళ(!ఫలితం && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(దత్తాంశం);
}

#నిర్వచితమైతే USE_SSLS_EXPORT

#నిర్వచించు CURL_SSL_TICKET_MAX   (16 * 1024)

స్థిర CURLcode cf_ssl_scache_peer_సమితి_hmac(నిర్మాణం Curl_ssl_scache_peer *peer)
{
  CURLcode ఫలితం;

  DEBUGASSERT(peer);
  ఒకవేళ(!peer->ssl_peer_కీలకం)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  ఫలితం = Curl_rand(ఖాళీ, peer->కీలకం_salt, పరిమాణం(peer->కీలకం_salt));
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఫలితం = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->కీలకం_salt, పరిమాణం(peer->కీలకం_salt),
                       (స్థిరరాశి గుర్తులేని అక్షరం *)peer->ssl_peer_కీలకం,
                       అక్షరశ్రేణి_పొడవు(peer->ssl_peer_కీలకం),
                       peer->కీలకం_hmac);
  ఒకవేళ(!ఫలితం)
    peer->hmac_సమితి = TRUE;
  తిరిగి ఫలితం;
}

స్థిర CURLcode cf_ssl_కనుగొను_peer_by_hmac(నిర్మాణం Curl_ssl_scache *scache,
                                         స్థిరరాశి గుర్తులేని అక్షరం *salt,
                                         స్థిరరాశి గుర్తులేని అక్షరం *hmac,
                                         నిర్మాణం Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode ఫలితం = CURLE_OK;

  *ppeer = ఖాళీ;
  ఒకవేళ(!GOOD_SCACHE(scache))
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  కొరకు(i = 0; scache && i < scache->peer_లెక్క; i++) {
    నిర్మాణం Curl_ssl_scache_peer *peer = &scache->peers[i];
    ఒకవేళ(!cf_ssl_scache_సరిపోలిక_auth(peer, ఖాళీ))
      కొనసాగు;
    ఒకవేళ(scache->peers[i].hmac_సమితి &&
       !memcmp(peer->కీలకం_salt, salt, పరిమాణం(peer->కీలకం_salt)) &&
       !memcmp(peer->కీలకం_hmac, hmac, పరిమాణం(peer->కీలకం_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      వెళ్ళు బహిర్గతం;
    }
    లేకపోతే ఒకవేళ(peer->ssl_peer_కీలకం) {
      గుర్తులేని అక్షరం my_hmac[CURL_SHA256_DIGEST_నిడివి];
      /* compute hmac for the passed salt */
      ఫలితం = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, పరిమాణం(peer->కీలకం_salt),
                           (స్థిరరాశి గుర్తులేని అక్షరం *)peer->ssl_peer_కీలకం,
                           అక్షరశ్రేణి_పొడవు(peer->ssl_peer_కీలకం),
                           my_hmac);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      ఒకవేళ(!memcmp(my_hmac, hmac, పరిమాణం(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        ఒకవేళ(!peer->hmac_సమితి) {
          జ్ఞాపకం_నకలు(peer->కీలకం_salt, salt, పరిమాణం(peer->కీలకం_salt));
          జ్ఞాపకం_నకలు(peer->కీలకం_hmac, hmac, పరిమాణం(peer->కీలకం_hmac));
          peer->hmac_సమితి = TRUE;
        }
        *ppeer = peer;
        వెళ్ళు బహిర్గతం;
      }
    }
  }
బహిర్గతం:
  తిరిగి ఫలితం;
}

CURLcode Curl_ssl_session_import(నిర్మాణం Curl_easy *దత్తాంశం,
                                 స్థిరరాశి అక్షరం *ssl_peer_కీలకం,
                                 స్థిరరాశి గుర్తులేని అక్షరం *shmac, size_t shmac_len,
                                 స్థిరరాశి శూన్య *sdata, size_t sdata_len)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  నిర్మాణం Curl_ssl_scache_peer *peer = ఖాళీ;
  నిర్మాణం Curl_ssl_session *s = ఖాళీ;
  bool have_తాళం = FALSE;
  CURLcode ఫలితం;

  ఒకవేళ(!scache || !sdata || !sdata_len) {
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }
  ఒకవేళ(!ssl_peer_కీలకం && (!shmac || !shmac_len)) {
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }

  ఫలితం = Curl_ssl_session_unpack(దత్తాంశం, sdata, sdata_len, &s);
  ఒకవేళ(ఫలితం)
    వెళ్ళు బహిర్గతం;

  Curl_ssl_scache_తాళం(దత్తాంశం);
  have_తాళం = TRUE;

  ఒకవేళ(ssl_peer_కీలకం) {
    ఫలితం = cf_ssl_కూడు_peer(దత్తాంశం, scache, ssl_peer_కీలకం, ఖాళీ, &peer);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
  }
  లేకపోతే ఒకవేళ(shmac_len != (పరిమాణం(peer->కీలకం_salt) + పరిమాణం(peer->కీలకం_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    ఫలితం = CURLE_BAD_FUNCTION_వాదం;
    వెళ్ళు బహిర్గతం;
  }
  లేకపోతే {
    స్థిరరాశి గుర్తులేని అక్షరం *salt = shmac;
    స్థిరరాశి గుర్తులేని అక్షరం *hmac = shmac + పరిమాణం(peer->కీలకం_salt);

    ఫలితం = cf_ssl_కనుగొను_peer_by_hmac(scache, salt, hmac, &peer);
    ఒకవేళ(ఫలితం)
      వెళ్ళు బహిర్గతం;
    ఒకవేళ(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      ఒకవేళ(peer) {
        ఫలితం = cf_ssl_scache_peer_init(peer, ssl_peer_కీలకం, ఖాళీ, ఖాళీ,
                                         salt, hmac);
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
      }
    }
  }

  ఒకవేళ(peer) {
    cf_scache_peer_కూడు_session(peer, s, సమయం(ఖాళీ));
    s = ఖాళీ; /* peer is now owner */
    CURL_TRC_SSLS(దత్తాంశం, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_కీలకం ? peer->ssl_peer_కీలకం : "without key",
                  Curl_llist_లెక్క(&peer->sessions));
  }

బహిర్గతం:
  ఒకవేళ(scache && have_తాళం)
    Curl_ssl_scache_unlock(దత్తాంశం);
  Curl_ssl_session_destroy(s);
  తిరిగి ఫలితం;
}

CURLcode Curl_ssl_session_export(నిర్మాణం Curl_easy *దత్తాంశం,
                                 curl_ssls_export_cb *export_fn,
                                 శూన్య *userptr)
{
  నిర్మాణం Curl_ssl_scache *scache = cf_ssl_scache_get(దత్తాంశం);
  నిర్మాణం Curl_ssl_scache_peer *peer;
  నిర్మాణం dynbuf sbuf, hbuf;
  నిర్మాణం Curl_llist_కణుపు *n;
  size_t i;
  curl_off_t now = సమయం(ఖాళీ);
  CURLcode ఫలితం = CURLE_OK;
#నిర్వచితమైతే CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#అయితే_ముగింపు

  ఒకవేళ(!export_fn)
    తిరిగి CURLE_BAD_FUNCTION_వాదం;

  Curl_ssl_scache_తాళం(దత్తాంశం);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_నిడివి * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  కొరకు(i = 0; scache && i < scache->peer_లెక్క; i++) {
    peer = &scache->peers[i];
    ఒకవేళ(!peer->ssl_peer_కీలకం && !peer->hmac_సమితి)
      కొనసాగు; /* skip free entry */
    ఒకవేళ(!peer->exportable)
      కొనసాగు;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_తల(&peer->sessions);
    ఒకవేళ(n)
      VERBOSE(++npeers);
    అయితే(n) {
      నిర్మాణం Curl_ssl_session *s = Curl_కణుపు_elem(n);
      ఒకవేళ(!peer->hmac_సమితి) {
        ఫలితం = cf_ssl_scache_peer_సమితి_hmac(peer);
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
      }
      ఒకవేళ(!curlx_dyn_len(&hbuf)) {
        ఫలితం = curlx_dyn_addn(&hbuf, peer->కీలకం_salt, పరిమాణం(peer->కీలకం_salt));
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
        ఫలితం = curlx_dyn_addn(&hbuf, peer->కీలకం_hmac, పరిమాణం(peer->కీలకం_hmac));
        ఒకవేళ(ఫలితం)
          వెళ్ళు బహిర్గతం;
      }
      curlx_dyn_reset(&sbuf);
      ఫలితం = Curl_ssl_session_pack(దత్తాంశం, s, &sbuf);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;

      ఫలితం = export_fn(దత్తాంశం, userptr, peer->ssl_peer_కీలకం,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      ఒకవేళ(ఫలితం)
        వెళ్ళు బహిర్గతం;
      VERBOSE(++ntickets);
      n = Curl_కణుపు_తదుపరి(n);
    }
  }
  ఫలితం = CURLE_OK;
  CURL_TRC_SSLS(దత్తాంశం, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

బహిర్గతం:
  Curl_ssl_scache_unlock(దత్తాంశం);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* USE_SSLS_EXPORT */

#అయితే_ముగింపు /* USE_SSL */
