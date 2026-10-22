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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ifdef USE_SSL

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cfilters.h"

#ಅನ್ತರ್ಭಾವ "vtls/vtls.h" /* generic SSL protos etc */
#ಅನ್ತರ್ಭಾವ "vtls/vtls_int.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_scache.h"
#ಅನ್ತರ್ಭಾವ "vtls/vtls_spack.h"

#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "curl_threads.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"
#ಅನ್ತರ್ಭಾವ "rand.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
ಸಂರಚನಾ Curl_ssl_scache_peer {
  ವರ್ಣ *ssl_peer_ಕೀಲಿ;      /* id for peer + relevant TLS configuration */
  ವರ್ಣ *clientcert;
  ವರ್ಣ *ಕೀಲಿ_passwd;
  ಸಂರಚನಾ Curl_llist sessions;
  ಶೂನ್ಯ *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  unsigned ವರ್ಣ ಕೀಲಿ_salt[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  unsigned ವರ್ಣ ಕೀಲಿ_hmac[CURL_SHA256_DIGEST_LENGTH]; /* for entry export */
  size_t max_sessions;
  long age;                /* a number, the higher the more recent */
  BIT(hmac_set);           /* if key_salt and key_hmac are present */
  BIT(exportable);         /* sessions for this peer can be exported */
};

#define CURL_SCACHE_MAGIC 0x000e1551

#define GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ಸ್ಥಿರ CURLcode cf_ssl_peer_ಕೀಲಿ_add_string(ಸಂರಚನಾ dynbuf *buf,
                                           ಧ್ರುವ ವರ್ಣ *name,
                                           ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_addf(buf, ":%s-%zu:%s", name, strlen(ಮೌಲ್ಯ), ಮೌಲ್ಯ);
}

ಸ್ಥಿರ CURLcode cf_ssl_peer_ಕೀಲಿ_add_path(ಸಂರಚನಾ dynbuf *buf,
                                         ಧ್ರುವ ವರ್ಣ *name,
                                         ಧ್ರುವ ವರ್ಣ *path,
                                         bool *is_local)
{
  ಯದಿ(path && path[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ifdef _WIN32
    ವರ್ಣ *abspath = _fullpath(ರಿಕ್ತ, path, 0);
    ಯದಿ(abspath) {
      CURLcode result = cf_ssl_peer_ಕೀಲಿ_add_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      ಮುಕ್ತ(abspath); /* allocated by CRT, use system free() */
      ಪ್ರತ್ಯಾಗಮ result;
    }
    *is_local = TRUE;
#ನೋಚೇದ್ಯದಿ defined(HAVE_REALPATH)
    ಯದಿ(path[0] != '/') {
      ವರ್ಣ *abspath = realpath(path, ರಿಕ್ತ);
      ಯದಿ(abspath) {
        CURLcode result = cf_ssl_peer_ಕೀಲಿ_add_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        ಮುಕ್ತ(abspath); /* allocated by libc, free without memdebug */
        ಪ್ರತ್ಯಾಗಮ result;
      }
      *is_local = TRUE;
    }
#endif
    ಪ್ರತ್ಯಾಗಮ cf_ssl_peer_ಕೀಲಿ_add_string(buf, name, path);
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_ssl_peer_ಕೀಲಿ_add_hash(ಸಂರಚನಾ dynbuf *buf,
                                         ಧ್ರುವ ವರ್ಣ *name,
                                         ಸಂರಚನಾ curl_blob *blob)
{
  CURLcode result = CURLE_OK;
  ಯದಿ(blob && blob->len) {
    unsigned ವರ್ಣ hash[CURL_SHA256_DIGEST_LENGTH];
    size_t i;

    result = curlx_dyn_addf(buf, ":%s-", name);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    result = Curl_sha256it(hash, blob->data, blob->len);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ಪ್ರತಿ(i = 0; i < CURL_SHA256_DIGEST_LENGTH; ++i) {
      result = curlx_dyn_addf(buf, "%02x", hash[i]);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_ssl_peer_ಕೀಲಿ_add_mtls(ಸಂರಚನಾ dynbuf *buf,
                                         ಸಂರಚನಾ ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode result = CURLE_OK;
  ಯದಿ(ssl->clientcert && ssl->clientcert[0]) {
    result = cf_ssl_peer_ಕೀಲಿ_add_path(buf, "CCERT", ssl->clientcert, is_local);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->ಕೀಲಿ && ssl->ಕೀಲಿ[0]) {
    result = cf_ssl_peer_ಕೀಲಿ_add_path(buf, "KEY", ssl->ಕೀಲಿ, is_local);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->ಕೀಲಿ_blob) {
    result = cf_ssl_peer_ಕೀಲಿ_add_hash(buf, "KEYBlob", ssl->ಕೀಲಿ_blob);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":CT-%zu:", strlen(ssl->cert_type));
    ಪ್ರತಿ(i = 0; !result && ssl->cert_type[i]; i++) {
      ವರ್ಣ c = Curl_raw_toupper(ssl->cert_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->ಕೀಲಿ_type && ssl->ಕೀಲಿ_type[0]) {
    size_t i;
    result = curlx_dyn_addf(buf, ":KT-%zu:", strlen(ssl->ಕೀಲಿ_type));
    ಪ್ರತಿ(i = 0; !result && ssl->ಕೀಲಿ_type[i]; i++) {
      ವರ್ಣ c = Curl_raw_toupper(ssl->ಕೀಲಿ_type[i]);
      result = curlx_dyn_addn(buf, &c, 1);
    }
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

#define CURL_SSLS_LOCAL_SUFFIX     ":L"
#define CURL_SSLS_GLOBAL_SUFFIX    ":G"

ಸ್ಥಿರ bool cf_ssl_peer_ಕೀಲಿ_is_global(ಧ್ರುವ ವರ್ಣ *peer_ಕೀಲಿ)
{
  size_t len = peer_ಕೀಲಿ ? strlen(peer_ಕೀಲಿ) : 0;
  ಪ್ರತ್ಯಾಗಮ (len > 2) &&
         (peer_ಕೀಲಿ[len - 1] == 'G') &&
         (peer_ಕೀಲಿ[len - 2] == ':');
}

ಸ್ಥಿರ CURLcode ssl_peer_ಕೀಲಿ_add_transport(ಸಂರಚನಾ dynbuf *buf,
                                           uint8_t transport)
{
  ವಿಕಲ್ಪ(transport) {
  ಪಕ್ಷ TRNSPRT_TCP:
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  ಪಕ್ಷ TRNSPRT_UDP:
    ಪ್ರತ್ಯಾಗಮ curlx_dyn_add(buf, ":UDP");
  ಪಕ್ಷ TRNSPRT_QUIC:
    ಪ್ರತ್ಯಾಗಮ curlx_dyn_add(buf, ":QUIC");
  ಪಕ್ಷ TRNSPRT_UNIX:
    ಪ್ರತ್ಯಾಗಮ curlx_dyn_add(buf, ":UNIX");
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ಸ್ಥಿರ CURLcode ssl_peer_ಕೀಲಿ_add_vrfy(ಸಂರಚನಾ dynbuf *buf,
                                      ಸಂರಚನಾ ssl_filter_config *ssl,
                                      ಧ್ರುವ ಸಂರಚನಾ ssl_peer *peer)
{
  CURLcode result;

  ಯದಿ(!ssl->verifypeer) {
    result = curlx_dyn_add(buf, ":NO-VRFY-PEER");
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಯದಿ(!ssl->verifyhost) {
    result = curlx_dyn_add(buf, ":NO-VRFY-HOST");
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಯದಿ(ssl->verifystatus) {
    result = curlx_dyn_add(buf, ":VRFY-STATUS");
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಯದಿ((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    result = cf_ssl_peer_ಕೀಲಿ_add_string(buf, "CHOST", peer->peer->hostname);
    ಯದಿ(!result)
      result = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->port);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ result;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssl_peer_ಕೀಲಿ_build(ಸಂರಚನಾ ssl_filter_config *ssl,
                                   ಧ್ರುವ ಸಂರಚನಾ ssl_peer *peer,
                                   ಧ್ರುವ ವರ್ಣ *tls_id,
                                   ವರ್ಣ **ppeer_ಕೀಲಿ)
{
  ಸಂರಚನಾ dynbuf buf;
  size_t ಕೀಲಿ_len;
  bool is_local = FALSE;
  CURLcode result;

  *ppeer_ಕೀಲಿ = ರಿಕ್ತ;
  curlx_dyn_init(&buf, 10 * 1024);

  result = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->port);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  result = ssl_peer_ಕೀಲಿ_add_transport(&buf, peer->transport);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  result = ssl_peer_ಕೀಲಿ_add_vrfy(&buf, ssl, peer);
  ಯದಿ(result)
    ಗಚ್ಛ out;
  ಯದಿ(ssl->version || ssl->version_max) {
    result = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->version,
                            (ssl->version_max >> 16));
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->ssl_options) {
    result = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->cipher_list) {
    result = cf_ssl_peer_ಕೀಲಿ_add_string(&buf, "CIPHER", ssl->cipher_list);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->cipher_list13) {
    result = cf_ssl_peer_ಕೀಲಿ_add_string(&buf, "CIPHER13", ssl->cipher_list13);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->curves) {
    result = cf_ssl_peer_ಕೀಲಿ_add_string(&buf, "CURVES", ssl->curves);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->signature_algorithms) {
    result = cf_ssl_peer_ಕೀಲಿ_add_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->verifypeer) {
    result = cf_ssl_peer_ಕೀಲಿ_add_path(&buf, "CA", ssl->CAfile, &is_local);
    ಯದಿ(!result)
      result = cf_ssl_peer_ಕೀಲಿ_add_path(&buf, "CApath",
                                        ssl->CApath, &is_local);
    ಯದಿ(!result && ssl->native_ca_store)
      result = curlx_dyn_add(&buf, ":NATIVE-CA");
    ಯದಿ(!result)
      result = cf_ssl_peer_ಕೀಲಿ_add_path(&buf, "CRL", ssl->CRLfile, &is_local);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    result = cf_ssl_peer_ಕೀಲಿ_add_path(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ಯದಿ(ssl->ca_info_blob) {
      result = cf_ssl_peer_ಕೀಲಿ_add_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
    ಯದಿ(ssl->issuercert_blob) {
      result = cf_ssl_peer_ಕೀಲಿ_add_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      ಯದಿ(result)
        ಗಚ್ಛ out;
    }
  }
  ಯದಿ(ssl->cert_blob) {
    result = cf_ssl_peer_ಕೀಲಿ_add_hash(&buf, "CertBlob", ssl->cert_blob);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ssl->pinned_ಕೀಲಿ && ssl->pinned_ಕೀಲಿ[0]) {
    result = cf_ssl_peer_ಕೀಲಿ_add_string(&buf, "Pinned", ssl->pinned_ಕೀಲಿ);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }

  result = cf_ssl_peer_ಕೀಲಿ_add_mtls(&buf, ssl, &is_local);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  ಯದಿ(!tls_id || !tls_id[0]) {
    result = CURLE_FAILED_INIT;
    ಗಚ್ಛ out;
  }
  result = cf_ssl_peer_ಕೀಲಿ_add_string(&buf, "IMPL", tls_id);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  result = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  *ppeer_ಕೀಲಿ = curlx_dyn_take(&buf, &ಕೀಲಿ_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

out:
  curlx_dyn_free(&buf);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_ssl_peer_ಕೀಲಿ_make(ಧ್ರುವ ಸಂರಚನಾ ssl_peer *peer,
                                ಸಂರಚನಾ ssl_filter_config *sslc,
                                ಧ್ರುವ ವರ್ಣ *tls_id,
                                ವರ್ಣ **ppeer_ಕೀಲಿ)
{
  ಪ್ರತ್ಯಾಗಮ ssl_peer_ಕೀಲಿ_build(sslc, peer, tls_id, ppeer_ಕೀಲಿ);
}

ಸಂರಚನಾ Curl_ssl_scache {
  unsigned ಪೂರ್ಣಾಙ್ಕ magic;
  ಸಂರಚನಾ Curl_ssl_scache_peer *peers;
  size_t peer_ಎಣಿಕೆ;
  ಪೂರ್ಣಾಙ್ಕ default_lifetime_secs;
  long age;
#ifdef USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#endif
  BIT(is_locked);
};

ಸ್ಥಿರ ಸಂರಚನಾ Curl_ssl_scache *cf_ssl_scache_get(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = ರಿಕ್ತ;
  /* If a share is present, its ssl_scache has preference over the multi */
  ಯದಿ(data->share && data->share->ssl_scache)
    scache = data->share->ssl_scache;
  ಅನ್ಯಥಾ ಯದಿ(data->multi && data->multi->ssl_scache)
    scache = data->multi->ssl_scache;
  ಯದಿ(scache && !GOOD_SCACHE(scache)) {
    failf(data, "transfer would use an invalid scache at %p, denied",
          (ಶೂನ್ಯ *)scache);
    DEBUGASSERT(0);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ scache;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ssl_scache_session_ldestroy(ಶೂನ್ಯ *udata, ಶೂನ್ಯ *obj)
{
  ಸಂರಚನಾ Curl_ssl_session *s = obj;
  (ಶೂನ್ಯ)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(ಶೂನ್ಯ *sdata, size_t sdata_len,
                                 ಪೂರ್ಣಾಙ್ಕ ietf_tls_id, ಧ್ರುವ ವರ್ಣ *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 ಸಂರಚನಾ Curl_ssl_session **psession)
{
  ಪ್ರತ್ಯಾಗಮ Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  ರಿಕ್ತ, 0, psession);
}

CURLcode Curl_ssl_session_create2(ಶೂನ್ಯ *sdata, size_t sdata_len,
                                  ಪೂರ್ಣಾಙ್ಕ ietf_tls_id, ಧ್ರುವ ವರ್ಣ *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  unsigned ವರ್ಣ *quic_tp, size_t quic_tp_len,
                                  ಸಂರಚನಾ Curl_ssl_session **psession)
{
  ಸಂರಚನಾ Curl_ssl_session *s;

  ಯದಿ(!sdata || !sdata_len) {
    curlx_free(sdata);
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  *psession = ರಿಕ್ತ;
  s = curlx_calloc(1, ಪರಿಮಾಣ(*s));
  ಯದಿ(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  ಯದಿ(alpn) {
    s->alpn = curlx_strdup(alpn);
    ಯದಿ(!s->alpn) {
      cf_ssl_scache_session_ldestroy(ರಿಕ್ತ, s);
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }
  *psession = s;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssl_session_destroy(ಸಂರಚನಾ Curl_ssl_session *s)
{
  ಯದಿ(s) {
    /* if in the list, the list destructor takes care of it */
    ಯದಿ(Curl_ಗಂಟು_llist(&s->list))
      Curl_ಗಂಟು_remove(&s->list);
    ಅನ್ಯಥಾ {
      cf_ssl_scache_session_ldestroy(ರಿಕ್ತ, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(ಸಂರಚನಾ Curl_ssl_session *src,
                              ಸಂರಚನಾ Curl_ssl_session **pdest)
{
  ಸಂರಚನಾ Curl_ssl_session *dest = ರಿಕ್ತ;
  CURLcode result = CURLE_OUT_OF_MEMORY;

  ಯದಿ(!src || !pdest)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  *pdest = ರಿಕ್ತ;

  dest = curlx_calloc(1, ಪರಿಮಾಣ(*dest));
  ಯದಿ(!dest)
    ಗಚ್ಛ out;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  ಯದಿ(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    ಯದಿ(!dest->sdata)
      ಗಚ್ಛ out;
    dest->sdata_len = src->sdata_len;
  }
  ಯದಿ(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    ಯದಿ(!dest->quic_tp)
      ಗಚ್ಛ out;
    dest->quic_tp_len = src->quic_tp_len;
  }
  ಯದಿ(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    ಯದಿ(!dest->alpn)
      ಗಚ್ಛ out;
  }
  result = CURLE_OK;

out:
  ಯದಿ(!result)
    *pdest = dest;
  ಅನ್ಯಥಾ {
    *pdest = ರಿಕ್ತ;
    ಯದಿ(dest)
      Curl_ssl_session_destroy(dest);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ssl_scache_clear_peer(ಸಂರಚನಾ Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, ರಿಕ್ತ);
  ಯದಿ(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    ಯದಿ(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = ರಿಕ್ತ;
  }
  peer->sobj_free = ರಿಕ್ತ;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->ಕೀಲಿ_passwd);
  curlx_safefree(peer->ssl_peer_ಕೀಲಿ);
  peer->age = 0;
  peer->hmac_set = FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ssl_scache_peer_set_obj(ಸಂರಚನಾ Curl_ssl_scache_peer *peer,
                                       ಶೂನ್ಯ *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  ಯದಿ(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ssl_cache_peer_update(ಸಂರಚನಾ Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->ಕೀಲಿ_passwd &&
                      (!peer->ssl_peer_ಕೀಲಿ ||
                       cf_ssl_peer_ಕೀಲಿ_is_global(peer->ssl_peer_ಕೀಲಿ)));
}

ಸ್ಥಿರ CURLcode cf_ssl_scache_peer_init(ಸಂರಚನಾ Curl_ssl_scache_peer *peer,
                                        ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                                        ಧ್ರುವ ವರ್ಣ *clientcert,
                                        ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ_passwd,
                                        ಧ್ರುವ unsigned ವರ್ಣ *salt,
                                        ಧ್ರುವ unsigned ವರ್ಣ *hmac)
{
  CURLcode result = CURLE_OUT_OF_MEMORY;

  DEBUGASSERT(!peer->ssl_peer_ಕೀಲಿ);
  ಯದಿ(ssl_peer_ಕೀಲಿ) {
    peer->ssl_peer_ಕೀಲಿ = curlx_strdup(ssl_peer_ಕೀಲಿ);
    ಯದಿ(!peer->ssl_peer_ಕೀಲಿ)
      ಗಚ್ಛ out;
    peer->hmac_set = FALSE;
  }
  ಅನ್ಯಥಾ ಯದಿ(salt && hmac) {
    memcpy(peer->ಕೀಲಿ_salt, salt, ಪರಿಮಾಣ(peer->ಕೀಲಿ_salt));
    memcpy(peer->ಕೀಲಿ_hmac, hmac, ಪರಿಮಾಣ(peer->ಕೀಲಿ_hmac));
    peer->hmac_set = TRUE;
  }
  ಅನ್ಯಥಾ {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }
  ಯದಿ(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    ಯದಿ(!peer->clientcert)
      ಗಚ್ಛ out;
  }
  ಯದಿ(ಕೀಲಿ_passwd) {
    peer->ಕೀಲಿ_passwd = curlx_strdup(ಕೀಲಿ_passwd);
    ಯದಿ(!peer->ಕೀಲಿ_passwd)
      ಗಚ್ಛ out;
  }
  cf_ssl_cache_peer_update(peer);
  result = CURLE_OK;
out:
  ಯದಿ(result)
    cf_ssl_scache_clear_peer(peer);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_scache_session_remove(ಸಂರಚನಾ Curl_ssl_scache_peer *peer,
                                     ಸಂರಚನಾ Curl_ssl_session *s)
{
  (ಶೂನ್ಯ)peer;
  DEBUGASSERT(Curl_ಗಂಟು_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

ಸ್ಥಿರ bool cf_scache_session_expired(ಸಂರಚನಾ Curl_ssl_session *s,
                                      curl_off_t now)
{
  ಪ್ರತ್ಯಾಗಮ (s->valid_until > 0) && (s->valid_until < now);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_scache_peer_remove_expired(ಸಂರಚನಾ Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n = Curl_llist_head(&peer->sessions);
  ಯಾವತ್(n) {
    ಸಂರಚನಾ Curl_ssl_session *s = Curl_ಗಂಟು_elem(n);
    n = Curl_ಗಂಟು_next(n);
    ಯದಿ(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_scache_peer_remove_non13(ಸಂರಚನಾ Curl_ssl_scache_peer *peer)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n = Curl_llist_head(&peer->sessions);
  ಯಾವತ್(n) {
    ಸಂರಚನಾ Curl_ssl_session *s = Curl_ಗಂಟು_elem(n);
    n = Curl_ಗಂಟು_next(n);
    ಯದಿ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_per_peer,
                                ಸಂರಚನಾ Curl_ssl_scache **pscache)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache;
  ಸಂರಚನಾ Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = ರಿಕ್ತ;
  peers = curlx_calloc(max_peers, ಪರಿಮಾಣ(*peers));
  ಯದಿ(!peers)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  scache = curlx_calloc(1, ಪರಿಮಾಣ(*scache));
  ಯದಿ(!scache) {
    curlx_free(peers);
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_ಎಣಿಕೆ = max_peers;
  scache->peers = peers;
  scache->age = 1;
  ಪ್ರತಿ(i = 0; i < scache->peer_ಎಣಿಕೆ; ++i) {
    scache->peers[i].max_sessions = max_sessions_per_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ifdef USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#endif
  *pscache = scache;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssl_scache_destroy(ಸಂರಚನಾ Curl_ssl_scache *scache)
{
  ಯದಿ(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    ಪ್ರತಿ(i = 0; i < scache->peer_ಎಣಿಕೆ; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ifdef USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#endif
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(ಸಂರಚನಾ Curl_cfilter *cf, ಸಂರಚನಾ Curl_easy *data)
{
  ಯದಿ(cf_ssl_scache_get(data)) {
    ಸಂರಚನಾ ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    ಪ್ರತ್ಯಾಗಮ conn_config ? conn_config->cache_session : FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/* Lock shared SSL session data */
ಶೂನ್ಯ Curl_ssl_scache_lock(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ಯದಿ(scache) {
    ಯದಿ(CURL_SHARE_ssl_scache(data))
      Curl_share_lock(data, CURL_LOCK_DATA_SSL_SESSION,
                      CURL_LOCK_ACCESS_SINGLE);
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#ಅನ್ಯಥಾ
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#endif
  }
}

/* Unlock shared SSL session data */
ಶೂನ್ಯ Curl_ssl_scache_unlock(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ಯದಿ(scache) {
#ifdef USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#ಅನ್ಯಥಾ
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#endif
    ಯದಿ(CURL_SHARE_ssl_scache(data))
      Curl_share_unlock(data, CURL_LOCK_DATA_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  bool locked = FALSE;
  ಯದಿ(!scache)
    ಪ್ರತ್ಯಾಗಮ FALSE;
#ifdef USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#ಅನ್ಯಥಾ
  locked = (bool)scache->is_locked;
#endif
  ಪ್ರತ್ಯಾಗಮ locked;
}

ಸ್ಥಿರ bool cf_ssl_scache_match_auth(ಸಂರಚನಾ Curl_ssl_scache_peer *peer,
                                     ಸಂರಚನಾ ssl_filter_config *conn_config)
{
  ಯದಿ(!conn_config) {
    ಯದಿ(peer->clientcert || peer->ಕೀಲಿ_passwd)
      ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(Curl_timestrcmp(peer->ಕೀಲಿ_passwd, conn_config->ಕೀಲಿ_passwd))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ CURLcode cf_ssl_find_peer_by_ಕೀಲಿ(ಸಂರಚನಾ Curl_easy *data,
                                        ಸಂರಚನಾ Curl_ssl_scache *scache,
                                        ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                                        ಸಂರಚನಾ ssl_filter_config *conn_config,
                                        ಸಂರಚನಾ Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_ಕೀಲಿ_len = 0;
  CURLcode result = CURLE_OK;

  *ppeer = ರಿಕ್ತ;
  ಯದಿ(!GOOD_SCACHE(scache)) {
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
  }

  CURL_TRC_SSLS(data, "find peer slot for %s among %zu slots",
                ssl_peer_ಕೀಲಿ, scache->peer_ಎಣಿಕೆ);

  /* check for entries with known peer_key */
  ಪ್ರತಿ(i = 0; scache && i < scache->peer_ಎಣಿಕೆ; i++) {
    ಯದಿ(scache->peers[i].ssl_peer_ಕೀಲಿ &&
       !strcmp(ssl_peer_ಕೀಲಿ, scache->peers[i].ssl_peer_ಕೀಲಿ) &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      ಗಚ್ಛ out;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  ಪ್ರತಿ(i = 0; scache && i < scache->peer_ಎಣಿಕೆ; i++) {
    ಯದಿ(!scache->peers[i].ssl_peer_ಕೀಲಿ &&
       scache->peers[i].hmac_set &&
       cf_ssl_scache_match_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      unsigned ವರ್ಣ my_hmac[CURL_SHA256_DIGEST_LENGTH];
      ಯದಿ(!peer_ಕೀಲಿ_len) /* we are lazy */
        peer_ಕೀಲಿ_len = strlen(ssl_peer_ಕೀಲಿ);
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].ಕೀಲಿ_salt,
                           ಪರಿಮಾಣ(scache->peers[i].ಕೀಲಿ_salt),
                           (ಧ್ರುವ unsigned ವರ್ಣ *)ssl_peer_ಕೀಲಿ,
                           peer_ಕೀಲಿ_len,
                           my_hmac);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      ಯದಿ(!memcmp(scache->peers[i].ಕೀಲಿ_hmac, my_hmac, ಪರಿಮಾಣ(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(data, "peer entry %zu key recovered: %s",
                      i, ssl_peer_ಕೀಲಿ);
        scache->peers[i].ssl_peer_ಕೀಲಿ = curlx_strdup(ssl_peer_ಕೀಲಿ);
        ಯದಿ(!scache->peers[i].ssl_peer_ಕೀಲಿ) {
          result = CURLE_OUT_OF_MEMORY;
          ಗಚ್ಛ out;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        ಗಚ್ಛ out;
      }
    }
  }
  CURL_TRC_SSLS(data, "peer not found for %s", ssl_peer_ಕೀಲಿ);
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಸಂರಚನಾ Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  ಸಂರಚನಾ Curl_ssl_scache *scache)
{
  ಸಂರಚನಾ Curl_ssl_scache_peer *peer = ರಿಕ್ತ;
  size_t i;

  /* find empty or oldest peer */
  ಪ್ರತಿ(i = 0; i < scache->peer_ಎಣಿಕೆ; ++i) {
    /* free peer entry? */
    ಯದಿ(!scache->peers[i].ssl_peer_ಕೀಲಿ && !scache->peers[i].hmac_set) {
      peer = &scache->peers[i];
      ಭಙ್ಗ;
    }
    /* peer without sessions and obj */
    ಯದಿ(!scache->peers[i].sobj &&
       !Curl_llist_ಎಣಿಕೆ(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      ಭಙ್ಗ;
    }
    /* remember "oldest" peer */
    ಯದಿ(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  ಯದಿ(peer)
    cf_ssl_scache_clear_peer(peer);
  ಪ್ರತ್ಯಾಗಮ peer;
}

ಸ್ಥಿರ CURLcode cf_ssl_add_peer(ಸಂರಚನಾ Curl_easy *data,
                                ಸಂರಚನಾ Curl_ssl_scache *scache,
                                ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                                ಸಂರಚನಾ ssl_filter_config *conn_config,
                                ಸಂರಚನಾ Curl_ssl_scache_peer **ppeer)
{
  ಸಂರಚನಾ Curl_ssl_scache_peer *peer = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  *ppeer = ರಿಕ್ತ;
  ಯದಿ(ssl_peer_ಕೀಲಿ) {
    result = cf_ssl_find_peer_by_ಕೀಲಿ(data, scache, ssl_peer_ಕೀಲಿ, conn_config,
                                     &peer);
    ಯದಿ(result || !scache->peer_ಎಣಿಕೆ)
      ಪ್ರತ್ಯಾಗಮ result;
  }

  ಯದಿ(peer) {
    *ppeer = peer;
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  ಯದಿ(peer) {
    ವರ್ಣ buffer[64];
    ಧ್ರುವ ವರ್ಣ *ccert = conn_config ? conn_config->clientcert : ರಿಕ್ತ;
    ಧ್ರುವ ವರ್ಣ *kpasswd = conn_config ? conn_config->ಕೀಲಿ_passwd : ರಿಕ್ತ;
    ಯದಿ(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(buffer, ಪರಿಮಾಣ(buffer),
                     "cert-%p", conn_config->cert_blob->data);
      ccert = buffer; /* data is strduped by cf_ssl_scache_peer_init */
    }
    result = cf_ssl_scache_peer_init(peer, ssl_peer_ಕೀಲಿ, ccert, kpasswd,
                                     ರಿಕ್ತ, ರಿಕ್ತ);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    /* all ready */
    *ppeer = peer;
    result = CURLE_OK;
  }

out:
  ಯದಿ(result) {
    cf_ssl_scache_clear_peer(peer);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_scache_peer_add_session(ಸಂರಚನಾ Curl_ssl_scache_peer *peer,
                                       ಸಂರಚನಾ Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  ಯದಿ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, ರಿಕ್ತ);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  ಅನ್ಯಥಾ {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    ಯಾವತ್(Curl_llist_ಎಣಿಕೆ(&peer->sessions) > peer->max_sessions) {
      Curl_ಗಂಟು_remove(Curl_llist_head(&peer->sessions));
    }
  }
}

ಸ್ಥಿರ CURLcode cf_scache_add_session(ಸಂರಚನಾ Curl_cfilter *cf,
                                      ಸಂರಚನಾ Curl_easy *data,
                                      ಸಂರಚನಾ Curl_ssl_scache *scache,
                                      ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                                      ಸಂರಚನಾ Curl_ssl_session *s)
{
  ಸಂರಚನಾ Curl_ssl_scache_peer *peer = ರಿಕ್ತ;
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result = CURLE_OUT_OF_MEMORY;
  curl_off_t now = (curl_off_t)time(ರಿಕ್ತ);
  curl_off_t max_lifetime;

  ಯದಿ(!scache || !scache->peer_ಎಣಿಕೆ) {
    Curl_ssl_session_destroy(s);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  ಯದಿ(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  ಯದಿ(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  ಯದಿ(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(data, "add, session already expired");
    Curl_ssl_session_destroy(s);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_ಕೀಲಿ, conn_config, &peer);
  ಯದಿ(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    Curl_ssl_session_destroy(s);
    ಗಚ್ಛ out;
  }

  cf_scache_peer_add_session(peer, s, now);

out:
  ಯದಿ(result) {
    failf(data, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_ಕೀಲಿ, (ಪೂರ್ಣಾಙ್ಕ)result);
  }
  ಅನ್ಯಥಾ
    CURL_TRC_SSLS(data, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_ಕೀಲಿ,
                  (unsigned ಪೂರ್ಣಾಙ್ಕ)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_ಎಣಿಕೆ(&peer->sessions) : 0);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_ssl_scache_put(ಸಂರಚನಾ Curl_cfilter *cf,
                             ಸಂರಚನಾ Curl_easy *data,
                             ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                             ಸಂರಚನಾ Curl_ssl_session *s)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode result;
  DEBUGASSERT(conn_config);

  ಯದಿ(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
  }

  Curl_ssl_scache_lock(data);
  result = cf_scache_add_session(cf, data, scache, ssl_peer_ಕೀಲಿ, s);
  Curl_ssl_scache_unlock(data);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ Curl_ssl_scache_return(ಸಂರಚನಾ Curl_cfilter *cf,
                            ಸಂರಚನಾ Curl_easy *data,
                            ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                            ಸಂರಚನಾ Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  ಯದಿ(s && s->ietf_tls_id < 0x304)
    (ಶೂನ್ಯ)Curl_ssl_scache_put(cf, data, ssl_peer_ಕೀಲಿ, s);
  ಅನ್ಯಥಾ
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                              ಸಂರಚನಾ Curl_ssl_session **ps)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ Curl_ssl_scache_peer *peer = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
  ಸಂರಚನಾ Curl_ssl_session *s = ರಿಕ್ತ;
  CURLcode result;

  *ps = ರಿಕ್ತ;
  ಯದಿ(!scache)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_ಕೀಲಿ(data, scache, ssl_peer_ಕೀಲಿ, conn_config,
                                   &peer);
  ಯದಿ(!result && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)time(ರಿಕ್ತ));
    n = Curl_llist_head(&peer->sessions);
    ಯದಿ(n) {
      s = Curl_ಗಂಟು_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  ಯದಿ(s) {
    *ps = s;
    CURL_TRC_SSLS(data, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_ಕೀಲಿ, (unsigned ಪೂರ್ಣಾಙ್ಕ)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_ಎಣಿಕೆ(&peer->sessions));
  }
  ಅನ್ಯಥಾ {
    CURL_TRC_SSLS(data, "no cached session for %s", ssl_peer_ಕೀಲಿ);
  }
  Curl_ssl_scache_unlock(data);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_ssl_scache_add_obj(ಸಂರಚನಾ Curl_cfilter *cf,
                                 ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                                 ಶೂನ್ಯ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ Curl_ssl_scache_peer *peer = ರಿಕ್ತ;
  CURLcode result;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  ಯದಿ(!scache) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }

  result = cf_ssl_add_peer(data, scache, ssl_peer_ಕೀಲಿ, conn_config, &peer);
  ಯದಿ(result || !peer) {
    CURL_TRC_SSLS(data, "unable to add scache peer: %d", (ಪೂರ್ಣಾಙ್ಕ)result);
    ಗಚ್ಛ out;
  }

  cf_ssl_scache_peer_set_obj(peer, sobj, sobj_dtor_cb);
  sobj = ರಿಕ್ತ;  /* peer took ownership */

out:
  ಯದಿ(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ *Curl_ssl_scache_get_obj(ಸಂರಚನಾ Curl_cfilter *cf,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ Curl_ssl_scache_peer *peer = ರಿಕ್ತ;
  CURLcode result;
  ಶೂನ್ಯ *sobj;

  ಯದಿ(!scache)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  result = cf_ssl_find_peer_by_ಕೀಲಿ(data, scache, ssl_peer_ಕೀಲಿ, conn_config,
                                   &peer);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  sobj = peer ? peer->sobj : ರಿಕ್ತ;

  CURL_TRC_SSLS(data, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_ಕೀಲಿ);
  ಪ್ರತ್ಯಾಗಮ sobj;
}

ಶೂನ್ಯ Curl_ssl_scache_remove_all(ಸಂರಚನಾ Curl_cfilter *cf,
                                ಸಂರಚನಾ Curl_easy *data,
                                ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ಸಂರಚನಾ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ಸಂರಚನಾ Curl_ssl_scache_peer *peer = ರಿಕ್ತ;
  CURLcode result;

  (ಶೂನ್ಯ)cf;
  ಯದಿ(!scache)
    ಪ್ರತ್ಯಾಗಮ;

  Curl_ssl_scache_lock(data);
  result = cf_ssl_find_peer_by_ಕೀಲಿ(data, scache, ssl_peer_ಕೀಲಿ, conn_config,
                                   &peer);
  ಯದಿ(!result && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(data);
}

#ifdef USE_SSLS_EXPORT

#define CURL_SSL_TICKET_MAX   (16 * 1024)

ಸ್ಥಿರ CURLcode cf_ssl_scache_peer_set_hmac(ಸಂರಚನಾ Curl_ssl_scache_peer *peer)
{
  CURLcode result;

  DEBUGASSERT(peer);
  ಯದಿ(!peer->ssl_peer_ಕೀಲಿ)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  result = Curl_rand(ರಿಕ್ತ, peer->ಕೀಲಿ_salt, ಪರಿಮಾಣ(peer->ಕೀಲಿ_salt));
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  result = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->ಕೀಲಿ_salt, ಪರಿಮಾಣ(peer->ಕೀಲಿ_salt),
                       (ಧ್ರುವ unsigned ವರ್ಣ *)peer->ssl_peer_ಕೀಲಿ,
                       strlen(peer->ssl_peer_ಕೀಲಿ),
                       peer->ಕೀಲಿ_hmac);
  ಯದಿ(!result)
    peer->hmac_set = TRUE;
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode cf_ssl_find_peer_by_hmac(ಸಂರಚನಾ Curl_ssl_scache *scache,
                                         ಧ್ರುವ unsigned ವರ್ಣ *salt,
                                         ಧ್ರುವ unsigned ವರ್ಣ *hmac,
                                         ಸಂರಚನಾ Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode result = CURLE_OK;

  *ppeer = ರಿಕ್ತ;
  ಯದಿ(!GOOD_SCACHE(scache))
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  ಪ್ರತಿ(i = 0; scache && i < scache->peer_ಎಣಿಕೆ; i++) {
    ಸಂರಚನಾ Curl_ssl_scache_peer *peer = &scache->peers[i];
    ಯದಿ(!cf_ssl_scache_match_auth(peer, ರಿಕ್ತ))
      ಅನುವರ್ತ;
    ಯದಿ(scache->peers[i].hmac_set &&
       !memcmp(peer->ಕೀಲಿ_salt, salt, ಪರಿಮಾಣ(peer->ಕೀಲಿ_salt)) &&
       !memcmp(peer->ಕೀಲಿ_hmac, hmac, ಪರಿಮಾಣ(peer->ಕೀಲಿ_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      ಗಚ್ಛ out;
    }
    ಅನ್ಯಥಾ ಯದಿ(peer->ssl_peer_ಕೀಲಿ) {
      unsigned ವರ್ಣ my_hmac[CURL_SHA256_DIGEST_LENGTH];
      /* compute hmac for the passed salt */
      result = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, ಪರಿಮಾಣ(peer->ಕೀಲಿ_salt),
                           (ಧ್ರುವ unsigned ವರ್ಣ *)peer->ssl_peer_ಕೀಲಿ,
                           strlen(peer->ssl_peer_ಕೀಲಿ),
                           my_hmac);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      ಯದಿ(!memcmp(my_hmac, hmac, ಪರಿಮಾಣ(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        ಯದಿ(!peer->hmac_set) {
          memcpy(peer->ಕೀಲಿ_salt, salt, ಪರಿಮಾಣ(peer->ಕೀಲಿ_salt));
          memcpy(peer->ಕೀಲಿ_hmac, hmac, ಪರಿಮಾಣ(peer->ಕೀಲಿ_hmac));
          peer->hmac_set = TRUE;
        }
        *ppeer = peer;
        ಗಚ್ಛ out;
      }
    }
  }
out:
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_ssl_session_import(ಸಂರಚನಾ Curl_easy *data,
                                 ಧ್ರುವ ವರ್ಣ *ssl_peer_ಕೀಲಿ,
                                 ಧ್ರುವ unsigned ವರ್ಣ *shmac, size_t shmac_len,
                                 ಧ್ರುವ ಶೂನ್ಯ *sdata, size_t sdata_len)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ಸಂರಚನಾ Curl_ssl_scache_peer *peer = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_ssl_session *s = ರಿಕ್ತ;
  bool have_lock = FALSE;
  CURLcode result;

  ಯದಿ(!scache || !sdata || !sdata_len) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }
  ಯದಿ(!ssl_peer_ಕೀಲಿ && (!shmac || !shmac_len)) {
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }

  result = Curl_ssl_session_unpack(data, sdata, sdata_len, &s);
  ಯದಿ(result)
    ಗಚ್ಛ out;

  Curl_ssl_scache_lock(data);
  have_lock = TRUE;

  ಯದಿ(ssl_peer_ಕೀಲಿ) {
    result = cf_ssl_add_peer(data, scache, ssl_peer_ಕೀಲಿ, ರಿಕ್ತ, &peer);
    ಯದಿ(result)
      ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ ಯದಿ(shmac_len != (ಪರಿಮಾಣ(peer->ಕೀಲಿ_salt) + ಪರಿಮಾಣ(peer->ಕೀಲಿ_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    result = CURLE_BAD_FUNCTION_ARGUMENT;
    ಗಚ್ಛ out;
  }
  ಅನ್ಯಥಾ {
    ಧ್ರುವ unsigned ವರ್ಣ *salt = shmac;
    ಧ್ರುವ unsigned ವರ್ಣ *hmac = shmac + ಪರಿಮಾಣ(peer->ಕೀಲಿ_salt);

    result = cf_ssl_find_peer_by_hmac(scache, salt, hmac, &peer);
    ಯದಿ(result)
      ಗಚ್ಛ out;
    ಯದಿ(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      ಯದಿ(peer) {
        result = cf_ssl_scache_peer_init(peer, ssl_peer_ಕೀಲಿ, ರಿಕ್ತ, ರಿಕ್ತ,
                                         salt, hmac);
        ಯದಿ(result)
          ಗಚ್ಛ out;
      }
    }
  }

  ಯದಿ(peer) {
    cf_scache_peer_add_session(peer, s, time(ರಿಕ್ತ));
    s = ರಿಕ್ತ; /* peer is now owner */
    CURL_TRC_SSLS(data, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_ಕೀಲಿ ? peer->ssl_peer_ಕೀಲಿ : "without key",
                  Curl_llist_ಎಣಿಕೆ(&peer->sessions));
  }

out:
  ಯದಿ(scache && have_lock)
    Curl_ssl_scache_unlock(data);
  Curl_ssl_session_destroy(s);
  ಪ್ರತ್ಯಾಗಮ result;
}

CURLcode Curl_ssl_session_export(ಸಂರಚನಾ Curl_easy *data,
                                 curl_ssls_export_cb *export_fn,
                                 ಶೂನ್ಯ *userptr)
{
  ಸಂರಚನಾ Curl_ssl_scache *scache = cf_ssl_scache_get(data);
  ಸಂರಚನಾ Curl_ssl_scache_peer *peer;
  ಸಂರಚನಾ dynbuf sbuf, hbuf;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
  size_t i;
  curl_off_t now = time(ರಿಕ್ತ);
  CURLcode result = CURLE_OK;
#ifdef CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#endif

  ಯದಿ(!export_fn)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  Curl_ssl_scache_lock(data);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_LENGTH * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  ಪ್ರತಿ(i = 0; scache && i < scache->peer_ಎಣಿಕೆ; i++) {
    peer = &scache->peers[i];
    ಯದಿ(!peer->ssl_peer_ಕೀಲಿ && !peer->hmac_set)
      ಅನುವರ್ತ; /* skip free entry */
    ಯದಿ(!peer->exportable)
      ಅನುವರ್ತ;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_head(&peer->sessions);
    ಯದಿ(n)
      VERBOSE(++npeers);
    ಯಾವತ್(n) {
      ಸಂರಚನಾ Curl_ssl_session *s = Curl_ಗಂಟು_elem(n);
      ಯದಿ(!peer->hmac_set) {
        result = cf_ssl_scache_peer_set_hmac(peer);
        ಯದಿ(result)
          ಗಚ್ಛ out;
      }
      ಯದಿ(!curlx_dyn_len(&hbuf)) {
        result = curlx_dyn_addn(&hbuf, peer->ಕೀಲಿ_salt, ಪರಿಮಾಣ(peer->ಕೀಲಿ_salt));
        ಯದಿ(result)
          ಗಚ್ಛ out;
        result = curlx_dyn_addn(&hbuf, peer->ಕೀಲಿ_hmac, ಪರಿಮಾಣ(peer->ಕೀಲಿ_hmac));
        ಯದಿ(result)
          ಗಚ್ಛ out;
      }
      curlx_dyn_reset(&sbuf);
      result = Curl_ssl_session_pack(data, s, &sbuf);
      ಯದಿ(result)
        ಗಚ್ಛ out;

      result = export_fn(data, userptr, peer->ssl_peer_ಕೀಲಿ,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      ಯದಿ(result)
        ಗಚ್ಛ out;
      VERBOSE(++ntickets);
      n = Curl_ಗಂಟು_next(n);
    }
  }
  result = CURLE_OK;
  CURL_TRC_SSLS(data, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

out:
  Curl_ssl_scache_unlock(data);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  ಪ್ರತ್ಯಾಗಮ result;
}

#endif /* USE_SSLS_EXPORT */

#endif /* USE_SSL */
