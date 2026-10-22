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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cfilters.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h" /* generic SSL protos etc */
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_scache.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls_spack.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_share.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_threads.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha256.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rand.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"

/* a peer+tls-config we cache sessions for */
ਬਣਤਰ Curl_ssl_scache_peer {
  ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ;      /* id for peer + relevant TLS configuration */
  ਅੱਖਰ *clientcert;
  ਅੱਖਰ *ਕੁੰਜੀ_passwd;
  ਬਣਤਰ Curl_llist sessions;
  ਖਾਲੀ *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਕੁੰਜੀ_salt[CURL_SHA256_DIGEST_ਲੰਮਾਈ]; /* for entry export */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਕੁੰਜੀ_hmac[CURL_SHA256_DIGEST_ਲੰਮਾਈ]; /* for entry export */
  size_t max_sessions;
  ਲੰਮਾ age;                /* a number, the higher the more recent */
  ਦੋਆਧਾਰੀ_ਅੰਕ(hmac_ਸੈੱਟ);           /* if key_salt and key_hmac are present */
  ਦੋਆਧਾਰੀ_ਅੰਕ(exportable);         /* sessions for this peer can be exported */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SCACHE_MAGIC 0x000e1551

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ਸਥਿਰ CURLcode cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(ਬਣਤਰ dynbuf *buf,
                                           ਅਚਲ ਅੱਖਰ *name,
                                           ਅਚਲ ਅੱਖਰ *ਮੁੱਲ)
{
  ਵਾਪਸ curlx_dyn_addf(buf, ":%s-%zu:%s", name, ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ), ਮੁੱਲ);
}

ਸਥਿਰ CURLcode cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_ਰਸਤਾ(ਬਣਤਰ dynbuf *buf,
                                         ਅਚਲ ਅੱਖਰ *name,
                                         ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                                         bool *is_local)
{
  ਜੇ(ਰਸਤਾ && ਰਸਤਾ[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
    ਅੱਖਰ *abspath = _fullpath(ਨਿਰਾ, ਰਸਤਾ, 0);
    ਜੇ(abspath) {
      CURLcode ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      ਮੁਕਤ(abspath); /* allocated by CRT, use system free() */
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    *is_local = TRUE;
#ਜਾਂਜੇ defined(HAVE_REALPATH)
    ਜੇ(ਰਸਤਾ[0] != '/') {
      ਅੱਖਰ *abspath = realpath(ਰਸਤਾ, ਨਿਰਾ);
      ਜੇ(abspath) {
        CURLcode ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        ਮੁਕਤ(abspath); /* allocated by libc, free without memdebug */
        ਵਾਪਸ ਨਤੀਜਾ;
      }
      *is_local = TRUE;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(buf, name, ਰਸਤਾ);
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_hash(ਬਣਤਰ dynbuf *buf,
                                         ਅਚਲ ਅੱਖਰ *name,
                                         ਬਣਤਰ curl_blob *blob)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(blob && blob->len) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ hash[CURL_SHA256_DIGEST_ਲੰਮਾਈ];
    size_t i;

    ਨਤੀਜਾ = curlx_dyn_addf(buf, ":%s-", name);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਨਤੀਜਾ = Curl_sha256it(hash, blob->ਅੰਕੜੇ, blob->len);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਲਈ(i = 0; i < CURL_SHA256_DIGEST_ਲੰਮਾਈ; ++i) {
      ਨਤੀਜਾ = curlx_dyn_addf(buf, "%02x", hash[i]);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_mtls(ਬਣਤਰ dynbuf *buf,
                                         ਬਣਤਰ ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  ਜੇ(ssl->clientcert && ssl->clientcert[0]) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_ਰਸਤਾ(buf, "CCERT", ssl->clientcert, is_local);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->ਕੁੰਜੀ && ssl->ਕੁੰਜੀ[0]) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_ਰਸਤਾ(buf, "KEY", ssl->ਕੁੰਜੀ, is_local);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->ਕੁੰਜੀ_blob) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_hash(buf, "KEYBlob", ssl->ਕੁੰਜੀ_blob);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    ਨਤੀਜਾ = curlx_dyn_addf(buf, ":CT-%zu:", ਲੜੀ_ਲੰਬਾਈ(ssl->cert_type));
    ਲਈ(i = 0; !ਨਤੀਜਾ && ssl->cert_type[i]; i++) {
      ਅੱਖਰ c = Curl_raw_toupper(ssl->cert_type[i]);
      ਨਤੀਜਾ = curlx_dyn_addn(buf, &c, 1);
    }
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->ਕੁੰਜੀ_type && ssl->ਕੁੰਜੀ_type[0]) {
    size_t i;
    ਨਤੀਜਾ = curlx_dyn_addf(buf, ":KT-%zu:", ਲੜੀ_ਲੰਬਾਈ(ssl->ਕੁੰਜੀ_type));
    ਲਈ(i = 0; !ਨਤੀਜਾ && ssl->ਕੁੰਜੀ_type[i]; i++) {
      ਅੱਖਰ c = Curl_raw_toupper(ssl->ਕੁੰਜੀ_type[i]);
      ਨਤੀਜਾ = curlx_dyn_addn(buf, &c, 1);
    }
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SSLS_LOCAL_SUFFIX     ":L"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SSLS_GLOBAL_SUFFIX    ":G"

ਸਥਿਰ bool cf_ssl_peer_ਕੁੰਜੀ_is_global(ਅਚਲ ਅੱਖਰ *peer_ਕੁੰਜੀ)
{
  size_t len = peer_ਕੁੰਜੀ ? ਲੜੀ_ਲੰਬਾਈ(peer_ਕੁੰਜੀ) : 0;
  ਵਾਪਸ (len > 2) &&
         (peer_ਕੁੰਜੀ[len - 1] == 'G') &&
         (peer_ਕੁੰਜੀ[len - 2] == ':');
}

ਸਥਿਰ CURLcode ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_transport(ਬਣਤਰ dynbuf *buf,
                                           uint8_t transport)
{
  ਚੋਣ(transport) {
  ਹਾਲਤ TRNSPRT_TCP:
    ਵਾਪਸ CURLE_OK;
  ਹਾਲਤ TRNSPRT_UDP:
    ਵਾਪਸ curlx_dyn_ਜੋੜੋ(buf, ":UDP");
  ਹਾਲਤ TRNSPRT_QUIC:
    ਵਾਪਸ curlx_dyn_ਜੋੜੋ(buf, ":QUIC");
  ਹਾਲਤ TRNSPRT_UNIX:
    ਵਾਪਸ curlx_dyn_ਜੋੜੋ(buf, ":UNIX");
  ਮੂਲ:
    ਵਾਪਸ curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ਸਥਿਰ CURLcode ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_vrfy(ਬਣਤਰ dynbuf *buf,
                                      ਬਣਤਰ ssl_filter_config *ssl,
                                      ਅਚਲ ਬਣਤਰ ssl_peer *peer)
{
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!ssl->verifypeer) {
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(buf, ":NO-VRFY-PEER");
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਜੇ(!ssl->verifyhost) {
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(buf, ":NO-VRFY-HOST");
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਜੇ(ssl->verifystatus) {
    ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(buf, ":VRFY-STATUS");
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਜੇ((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(buf, "CHOST", peer->peer->hostname);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->ਪੋਰਟ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode ssl_peer_ਕੁੰਜੀ_build(ਬਣਤਰ ssl_filter_config *ssl,
                                   ਅਚਲ ਬਣਤਰ ssl_peer *peer,
                                   ਅਚਲ ਅੱਖਰ *tls_id,
                                   ਅੱਖਰ **ppeer_ਕੁੰਜੀ)
{
  ਬਣਤਰ dynbuf buf;
  size_t ਕੁੰਜੀ_len;
  bool is_local = FALSE;
  CURLcode ਨਤੀਜਾ;

  *ppeer_ਕੁੰਜੀ = ਨਿਰਾ;
  curlx_dyn_init(&buf, 10 * 1024);

  ਨਤੀਜਾ = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->ਪੋਰਟ);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਨਤੀਜਾ = ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_transport(&buf, peer->transport);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਨਤੀਜਾ = ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_vrfy(&buf, ssl, peer);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;
  ਜੇ(ssl->ਸੰਸਕਰਣ_2 || ssl->ਸੰਸਕਰਣ_max) {
    ਨਤੀਜਾ = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->ਸੰਸਕਰਣ_2,
                            (ssl->ਸੰਸਕਰਣ_max >> 16));
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->ssl_options) {
    ਨਤੀਜਾ = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->cipher_list) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(&buf, "CIPHER", ssl->cipher_list);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->cipher_list13) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(&buf, "CIPHER13", ssl->cipher_list13);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->curves) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(&buf, "CURVES", ssl->curves);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->signature_algorithms) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->verifypeer) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_ਰਸਤਾ(&buf, "CA", ssl->CAfile, &is_local);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_ਰਸਤਾ(&buf, "CApath",
                                        ssl->CApath, &is_local);
    ਜੇ(!ਨਤੀਜਾ && ssl->native_ca_store)
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&buf, ":NATIVE-CA");
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_ਰਸਤਾ(&buf, "CRL", ssl->CRLfile, &is_local);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_ਰਸਤਾ(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਜੇ(ssl->ca_info_blob) {
      ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(ssl->issuercert_blob) {
      ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  ਜੇ(ssl->cert_blob) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_hash(&buf, "CertBlob", ssl->cert_blob);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ssl->pinned_ਕੁੰਜੀ && ssl->pinned_ਕੁੰਜੀ[0]) {
    ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(&buf, "Pinned", ssl->pinned_ਕੁੰਜੀ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_mtls(&buf, ssl, &is_local);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਜੇ(!tls_id || !tls_id[0]) {
    ਨਤੀਜਾ = CURLE_FAILED_INIT;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = cf_ssl_peer_ਕੁੰਜੀ_ਜੋੜੋ_string(&buf, "IMPL", tls_id);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  ਨਤੀਜਾ = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  *ppeer_ਕੁੰਜੀ = curlx_dyn_take(&buf, &ਕੁੰਜੀ_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

ਬਾਹਰਲਾ:
  curlx_dyn_free(&buf);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_ssl_peer_ਕੁੰਜੀ_make(ਅਚਲ ਬਣਤਰ ssl_peer *peer,
                                ਬਣਤਰ ssl_filter_config *sslc,
                                ਅਚਲ ਅੱਖਰ *tls_id,
                                ਅੱਖਰ **ppeer_ਕੁੰਜੀ)
{
  ਵਾਪਸ ssl_peer_ਕੁੰਜੀ_build(sslc, peer, tls_id, ppeer_ਕੁੰਜੀ);
}

ਬਣਤਰ Curl_ssl_scache {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ magic;
  ਬਣਤਰ Curl_ssl_scache_peer *peers;
  size_t peer_ਗਿਣਤੀ;
  ਪੂਰਨ default_lifetime_secs;
  ਲੰਮਾ age;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(is_locked);
};

ਸਥਿਰ ਬਣਤਰ Curl_ssl_scache *cf_ssl_scache_get(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_ssl_scache *scache = ਨਿਰਾ;
  /* If a share is present, its ssl_scache has preference over the multi */
  ਜੇ(ਅੰਕੜੇ->share && ਅੰਕੜੇ->share->ssl_scache)
    scache = ਅੰਕੜੇ->share->ssl_scache;
  ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕੜੇ->multi && ਅੰਕੜੇ->multi->ssl_scache)
    scache = ਅੰਕੜੇ->multi->ssl_scache;
  ਜੇ(scache && !GOOD_SCACHE(scache)) {
    failf(ਅੰਕੜੇ, "transfer would use an invalid scache at %p, denied",
          (ਖਾਲੀ *)scache);
    DEBUGASSERT(0);
    ਵਾਪਸ ਨਿਰਾ;
  }
  ਵਾਪਸ scache;
}

ਸਥਿਰ ਖਾਲੀ cf_ssl_scache_session_ldestroy(ਖਾਲੀ *udata, ਖਾਲੀ *obj)
{
  ਬਣਤਰ Curl_ssl_session *s = obj;
  (ਖਾਲੀ)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(ਖਾਲੀ *sdata, size_t sdata_len,
                                 ਪੂਰਨ ietf_tls_id, ਅਚਲ ਅੱਖਰ *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 ਬਣਤਰ Curl_ssl_session **psession)
{
  ਵਾਪਸ Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  ਨਿਰਾ, 0, psession);
}

CURLcode Curl_ssl_session_create2(ਖਾਲੀ *sdata, size_t sdata_len,
                                  ਪੂਰਨ ietf_tls_id, ਅਚਲ ਅੱਖਰ *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *quic_tp, size_t quic_tp_len,
                                  ਬਣਤਰ Curl_ssl_session **psession)
{
  ਬਣਤਰ Curl_ssl_session *s;

  ਜੇ(!sdata || !sdata_len) {
    curlx_free(sdata);
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  *psession = ਨਿਰਾ;
  s = curlx_calloc(1, ਆਕਾਰ(*s));
  ਜੇ(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  ਜੇ(alpn) {
    s->alpn = curlx_strdup(alpn);
    ਜੇ(!s->alpn) {
      cf_ssl_scache_session_ldestroy(ਨਿਰਾ, s);
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  *psession = s;
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_ssl_session_destroy(ਬਣਤਰ Curl_ssl_session *s)
{
  ਜੇ(s) {
    /* if in the list, the list destructor takes care of it */
    ਜੇ(Curl_ਗੰਢ_llist(&s->list))
      Curl_ਗੰਢ_remove(&s->list);
    ਨਹੀਂਤਾਂ {
      cf_ssl_scache_session_ldestroy(ਨਿਰਾ, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(ਬਣਤਰ Curl_ssl_session *src,
                              ਬਣਤਰ Curl_ssl_session **pdest)
{
  ਬਣਤਰ Curl_ssl_session *dest = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  ਜੇ(!src || !pdest)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  *pdest = ਨਿਰਾ;

  dest = curlx_calloc(1, ਆਕਾਰ(*dest));
  ਜੇ(!dest)
    ਜਾਓ ਬਾਹਰਲਾ;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  ਜੇ(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    ਜੇ(!dest->sdata)
      ਜਾਓ ਬਾਹਰਲਾ;
    dest->sdata_len = src->sdata_len;
  }
  ਜੇ(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    ਜੇ(!dest->quic_tp)
      ਜਾਓ ਬਾਹਰਲਾ;
    dest->quic_tp_len = src->quic_tp_len;
  }
  ਜੇ(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    ਜੇ(!dest->alpn)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਤੀਜਾ = CURLE_OK;

ਬਾਹਰਲਾ:
  ਜੇ(!ਨਤੀਜਾ)
    *pdest = dest;
  ਨਹੀਂਤਾਂ {
    *pdest = ਨਿਰਾ;
    ਜੇ(dest)
      Curl_ssl_session_destroy(dest);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_ssl_scache_clear_peer(ਬਣਤਰ Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, ਨਿਰਾ);
  ਜੇ(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    ਜੇ(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = ਨਿਰਾ;
  }
  peer->sobj_free = ਨਿਰਾ;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->ਕੁੰਜੀ_passwd);
  curlx_safefree(peer->ssl_peer_ਕੁੰਜੀ);
  peer->age = 0;
  peer->hmac_ਸੈੱਟ = FALSE;
}

ਸਥਿਰ ਖਾਲੀ cf_ssl_scache_peer_ਸੈੱਟ_obj(ਬਣਤਰ Curl_ssl_scache_peer *peer,
                                       ਖਾਲੀ *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  ਜੇ(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

ਸਥਿਰ ਖਾਲੀ cf_ssl_cache_peer_update(ਬਣਤਰ Curl_ssl_scache_peer *peer)
{
  /* The sessions of this peer are exportable if
   * - it has no confidential information
   * - its peer key is not yet known, because sessions were
   *   imported using only the salt+hmac
   * - the peer key is global, e.g. carrying no relative paths */
  peer->exportable = (!peer->clientcert && !peer->ਕੁੰਜੀ_passwd &&
                      (!peer->ssl_peer_ਕੁੰਜੀ ||
                       cf_ssl_peer_ਕੁੰਜੀ_is_global(peer->ssl_peer_ਕੁੰਜੀ)));
}

ਸਥਿਰ CURLcode cf_ssl_scache_peer_init(ਬਣਤਰ Curl_ssl_scache_peer *peer,
                                        ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                                        ਅਚਲ ਅੱਖਰ *clientcert,
                                        ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ_passwd,
                                        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *salt,
                                        ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *hmac)
{
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  DEBUGASSERT(!peer->ssl_peer_ਕੁੰਜੀ);
  ਜੇ(ssl_peer_ਕੁੰਜੀ) {
    peer->ssl_peer_ਕੁੰਜੀ = curlx_strdup(ssl_peer_ਕੁੰਜੀ);
    ਜੇ(!peer->ssl_peer_ਕੁੰਜੀ)
      ਜਾਓ ਬਾਹਰਲਾ;
    peer->hmac_ਸੈੱਟ = FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(salt && hmac) {
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(peer->ਕੁੰਜੀ_salt, salt, ਆਕਾਰ(peer->ਕੁੰਜੀ_salt));
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(peer->ਕੁੰਜੀ_hmac, hmac, ਆਕਾਰ(peer->ਕੁੰਜੀ_hmac));
    peer->hmac_ਸੈੱਟ = TRUE;
  }
  ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    ਜੇ(!peer->clientcert)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(ਕੁੰਜੀ_passwd) {
    peer->ਕੁੰਜੀ_passwd = curlx_strdup(ਕੁੰਜੀ_passwd);
    ਜੇ(!peer->ਕੁੰਜੀ_passwd)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  cf_ssl_cache_peer_update(peer);
  ਨਤੀਜਾ = CURLE_OK;
ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ)
    cf_ssl_scache_clear_peer(peer);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_scache_session_remove(ਬਣਤਰ Curl_ssl_scache_peer *peer,
                                     ਬਣਤਰ Curl_ssl_session *s)
{
  (ਖਾਲੀ)peer;
  DEBUGASSERT(Curl_ਗੰਢ_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

ਸਥਿਰ bool cf_scache_session_expired(ਬਣਤਰ Curl_ssl_session *s,
                                      curl_off_t now)
{
  ਵਾਪਸ (s->valid_until > 0) && (s->valid_until < now);
}

ਸਥਿਰ ਖਾਲੀ cf_scache_peer_remove_expired(ਬਣਤਰ Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *n = Curl_llist_ਸਿਰ(&peer->sessions);
  ਜਦੋਂ(n) {
    ਬਣਤਰ Curl_ssl_session *s = Curl_ਗੰਢ_elem(n);
    n = Curl_ਗੰਢ_ਅਗਲਾ(n);
    ਜੇ(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ਸਥਿਰ ਖਾਲੀ cf_scache_peer_remove_non13(ਬਣਤਰ Curl_ssl_scache_peer *peer)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *n = Curl_llist_ਸਿਰ(&peer->sessions);
  ਜਦੋਂ(n) {
    ਬਣਤਰ Curl_ssl_session *s = Curl_ਗੰਢ_elem(n);
    n = Curl_ਗੰਢ_ਅਗਲਾ(n);
    ਜੇ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_ਪ੍ਰਤੀ_peer,
                                ਬਣਤਰ Curl_ssl_scache **pscache)
{
  ਬਣਤਰ Curl_ssl_scache *scache;
  ਬਣਤਰ Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = ਨਿਰਾ;
  peers = curlx_calloc(max_peers, ਆਕਾਰ(*peers));
  ਜੇ(!peers)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  scache = curlx_calloc(1, ਆਕਾਰ(*scache));
  ਜੇ(!scache) {
    curlx_free(peers);
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_ਗਿਣਤੀ = max_peers;
  scache->peers = peers;
  scache->age = 1;
  ਲਈ(i = 0; i < scache->peer_ਗਿਣਤੀ; ++i) {
    scache->peers[i].max_sessions = max_sessions_ਪ੍ਰਤੀ_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#ਜੇ_ਅੰਤ
  *pscache = scache;
  ਵਾਪਸ CURLE_OK;
}

ਖਾਲੀ Curl_ssl_scache_destroy(ਬਣਤਰ Curl_ssl_scache *scache)
{
  ਜੇ(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    ਲਈ(i = 0; i < scache->peer_ਗਿਣਤੀ; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#ਜੇ_ਅੰਤ
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(ਬਣਤਰ Curl_cfilter *cf, ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਜੇ(cf_ssl_scache_get(ਅੰਕੜੇ)) {
    ਬਣਤਰ ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    ਵਾਪਸ conn_config ? conn_config->cache_session : FALSE;
  }
  ਵਾਪਸ FALSE;
}

/* Lock shared SSL session data */
ਖਾਲੀ Curl_ssl_scache_ਤਾਲਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  ਜੇ(scache) {
    ਜੇ(CURL_SHARE_ssl_scache(ਅੰਕੜੇ))
      Curl_share_ਤਾਲਾ(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_SSL_SESSION,
                      CURL_ਤਾਲਾ_ACCESS_SINGLE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#ਨਹੀਂਤਾਂ
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#ਜੇ_ਅੰਤ
  }
}

/* Unlock shared SSL session data */
ਖਾਲੀ Curl_ssl_scache_unlock(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  ਜੇ(scache) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#ਨਹੀਂਤਾਂ
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#ਜੇ_ਅੰਤ
    ਜੇ(CURL_SHARE_ssl_scache(ਅੰਕੜੇ))
      Curl_share_unlock(ਅੰਕੜੇ, CURL_ਤਾਲਾ_ਅੰਕੜੇ_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  bool locked = FALSE;
  ਜੇ(!scache)
    ਵਾਪਸ FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#ਨਹੀਂਤਾਂ
  locked = (bool)scache->is_locked;
#ਜੇ_ਅੰਤ
  ਵਾਪਸ locked;
}

ਸਥਿਰ bool cf_ssl_scache_ਮੇਲ_auth(ਬਣਤਰ Curl_ssl_scache_peer *peer,
                                     ਬਣਤਰ ssl_filter_config *conn_config)
{
  ਜੇ(!conn_config) {
    ਜੇ(peer->clientcert || peer->ਕੁੰਜੀ_passwd)
      ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    ਵਾਪਸ FALSE;
  ਜੇ(Curl_timestrcmp(peer->ਕੁੰਜੀ_passwd, conn_config->ਕੁੰਜੀ_passwd))
    ਵਾਪਸ FALSE;
  ਵਾਪਸ TRUE;
}

ਸਥਿਰ CURLcode cf_ssl_ਲੱਭੋ_peer_by_ਕੁੰਜੀ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ Curl_ssl_scache *scache,
                                        ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                                        ਬਣਤਰ ssl_filter_config *conn_config,
                                        ਬਣਤਰ Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_ਕੁੰਜੀ_len = 0;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *ppeer = ਨਿਰਾ;
  ਜੇ(!GOOD_SCACHE(scache)) {
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
  }

  CURL_TRC_SSLS(ਅੰਕੜੇ, "find peer slot for %s among %zu slots",
                ssl_peer_ਕੁੰਜੀ, scache->peer_ਗਿਣਤੀ);

  /* check for entries with known peer_key */
  ਲਈ(i = 0; scache && i < scache->peer_ਗਿਣਤੀ; i++) {
    ਜੇ(scache->peers[i].ssl_peer_ਕੁੰਜੀ &&
       !strcmp(ssl_peer_ਕੁੰਜੀ, scache->peers[i].ssl_peer_ਕੁੰਜੀ) &&
       cf_ssl_scache_ਮੇਲ_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      ਜਾਓ ਬਾਹਰਲਾ;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  ਲਈ(i = 0; scache && i < scache->peer_ਗਿਣਤੀ; i++) {
    ਜੇ(!scache->peers[i].ssl_peer_ਕੁੰਜੀ &&
       scache->peers[i].hmac_ਸੈੱਟ &&
       cf_ssl_scache_ਮੇਲ_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ my_hmac[CURL_SHA256_DIGEST_ਲੰਮਾਈ];
      ਜੇ(!peer_ਕੁੰਜੀ_len) /* we are lazy */
        peer_ਕੁੰਜੀ_len = ਲੜੀ_ਲੰਬਾਈ(ssl_peer_ਕੁੰਜੀ);
      ਨਤੀਜਾ = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].ਕੁੰਜੀ_salt,
                           ਆਕਾਰ(scache->peers[i].ਕੁੰਜੀ_salt),
                           (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ssl_peer_ਕੁੰਜੀ,
                           peer_ਕੁੰਜੀ_len,
                           my_hmac);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      ਜੇ(!memcmp(scache->peers[i].ਕੁੰਜੀ_hmac, my_hmac, ਆਕਾਰ(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(ਅੰਕੜੇ, "peer entry %zu key recovered: %s",
                      i, ssl_peer_ਕੁੰਜੀ);
        scache->peers[i].ssl_peer_ਕੁੰਜੀ = curlx_strdup(ssl_peer_ਕੁੰਜੀ);
        ਜੇ(!scache->peers[i].ssl_peer_ਕੁੰਜੀ) {
          ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
          ਜਾਓ ਬਾਹਰਲਾ;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
  }
  CURL_TRC_SSLS(ਅੰਕੜੇ, "peer not found for %s", ssl_peer_ਕੁੰਜੀ);
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਬਣਤਰ Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  ਬਣਤਰ Curl_ssl_scache *scache)
{
  ਬਣਤਰ Curl_ssl_scache_peer *peer = ਨਿਰਾ;
  size_t i;

  /* find empty or oldest peer */
  ਲਈ(i = 0; i < scache->peer_ਗਿਣਤੀ; ++i) {
    /* free peer entry? */
    ਜੇ(!scache->peers[i].ssl_peer_ਕੁੰਜੀ && !scache->peers[i].hmac_ਸੈੱਟ) {
      peer = &scache->peers[i];
      ਰੋਕੋ;
    }
    /* peer without sessions and obj */
    ਜੇ(!scache->peers[i].sobj &&
       !Curl_llist_ਗਿਣਤੀ(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      ਰੋਕੋ;
    }
    /* remember "oldest" peer */
    ਜੇ(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  ਜੇ(peer)
    cf_ssl_scache_clear_peer(peer);
  ਵਾਪਸ peer;
}

ਸਥਿਰ CURLcode cf_ssl_ਜੋੜੋ_peer(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ Curl_ssl_scache *scache,
                                ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                                ਬਣਤਰ ssl_filter_config *conn_config,
                                ਬਣਤਰ Curl_ssl_scache_peer **ppeer)
{
  ਬਣਤਰ Curl_ssl_scache_peer *peer = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *ppeer = ਨਿਰਾ;
  ਜੇ(ssl_peer_ਕੁੰਜੀ) {
    ਨਤੀਜਾ = cf_ssl_ਲੱਭੋ_peer_by_ਕੁੰਜੀ(ਅੰਕੜੇ, scache, ssl_peer_ਕੁੰਜੀ, conn_config,
                                     &peer);
    ਜੇ(ਨਤੀਜਾ || !scache->peer_ਗਿਣਤੀ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }

  ਜੇ(peer) {
    *ppeer = peer;
    ਵਾਪਸ CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  ਜੇ(peer) {
    ਅੱਖਰ ਬਫਰ[64];
    ਅਚਲ ਅੱਖਰ *ccert = conn_config ? conn_config->clientcert : ਨਿਰਾ;
    ਅਚਲ ਅੱਖਰ *kpasswd = conn_config ? conn_config->ਕੁੰਜੀ_passwd : ਨਿਰਾ;
    ਜੇ(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(ਬਫਰ, ਆਕਾਰ(ਬਫਰ),
                     "cert-%p", conn_config->cert_blob->ਅੰਕੜੇ);
      ccert = ਬਫਰ; /* data is strduped by cf_ssl_scache_peer_init */
    }
    ਨਤੀਜਾ = cf_ssl_scache_peer_init(peer, ssl_peer_ਕੁੰਜੀ, ccert, kpasswd,
                                     ਨਿਰਾ, ਨਿਰਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    /* all ready */
    *ppeer = peer;
    ਨਤੀਜਾ = CURLE_OK;
  }

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    cf_ssl_scache_clear_peer(peer);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਖਾਲੀ cf_scache_peer_ਜੋੜੋ_session(ਬਣਤਰ Curl_ssl_scache_peer *peer,
                                       ਬਣਤਰ Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  ਜੇ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, ਨਿਰਾ);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  ਨਹੀਂਤਾਂ {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    ਜਦੋਂ(Curl_llist_ਗਿਣਤੀ(&peer->sessions) > peer->max_sessions) {
      Curl_ਗੰਢ_remove(Curl_llist_ਸਿਰ(&peer->sessions));
    }
  }
}

ਸਥਿਰ CURLcode cf_scache_ਜੋੜੋ_session(ਬਣਤਰ Curl_cfilter *cf,
                                      ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                      ਬਣਤਰ Curl_ssl_scache *scache,
                                      ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                                      ਬਣਤਰ Curl_ssl_session *s)
{
  ਬਣਤਰ Curl_ssl_scache_peer *peer = ਨਿਰਾ;
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  curl_off_t now = (curl_off_t)ਸਮਾਂ(ਨਿਰਾ);
  curl_off_t max_lifetime;

  ਜੇ(!scache || !scache->peer_ਗਿਣਤੀ) {
    Curl_ssl_session_destroy(s);
    ਵਾਪਸ CURLE_OK;
  }

  ਜੇ(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  ਜੇ(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  ਜੇ(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(ਅੰਕੜੇ, "add, session already expired");
    Curl_ssl_session_destroy(s);
    ਵਾਪਸ CURLE_OK;
  }

  ਨਤੀਜਾ = cf_ssl_ਜੋੜੋ_peer(ਅੰਕੜੇ, scache, ssl_peer_ਕੁੰਜੀ, conn_config, &peer);
  ਜੇ(ਨਤੀਜਾ || !peer) {
    CURL_TRC_SSLS(ਅੰਕੜੇ, "unable to add scache peer: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    Curl_ssl_session_destroy(s);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  cf_scache_peer_ਜੋੜੋ_session(peer, s, now);

ਬਾਹਰਲਾ:
  ਜੇ(ਨਤੀਜਾ) {
    failf(ਅੰਕੜੇ, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_ਕੁੰਜੀ, (ਪੂਰਨ)ਨਤੀਜਾ);
  }
  ਨਹੀਂਤਾਂ
    CURL_TRC_SSLS(ਅੰਕੜੇ, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_ਕੁੰਜੀ,
                  (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_ਗਿਣਤੀ(&peer->sessions) : 0);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_ssl_scache_put(ਬਣਤਰ Curl_cfilter *cf,
                             ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                             ਬਣਤਰ Curl_ssl_session *s)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ਨਤੀਜਾ;
  DEBUGASSERT(conn_config);

  ਜੇ(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    ਵਾਪਸ CURLE_OK;
  }

  Curl_ssl_scache_ਤਾਲਾ(ਅੰਕੜੇ);
  ਨਤੀਜਾ = cf_scache_ਜੋੜੋ_session(cf, ਅੰਕੜੇ, scache, ssl_peer_ਕੁੰਜੀ, s);
  Curl_ssl_scache_unlock(ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ Curl_ssl_scache_return(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                            ਬਣਤਰ Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  ਜੇ(s && s->ietf_tls_id < 0x304)
    (ਖਾਲੀ)Curl_ssl_scache_put(cf, ਅੰਕੜੇ, ssl_peer_ਕੁੰਜੀ, s);
  ਨਹੀਂਤਾਂ
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                              ਬਣਤਰ Curl_ssl_session **ps)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ Curl_ssl_scache_peer *peer = ਨਿਰਾ;
  ਬਣਤਰ Curl_llist_ਗੰਢ *n;
  ਬਣਤਰ Curl_ssl_session *s = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  *ps = ਨਿਰਾ;
  ਜੇ(!scache)
    ਵਾਪਸ CURLE_OK;

  Curl_ssl_scache_ਤਾਲਾ(ਅੰਕੜੇ);
  ਨਤੀਜਾ = cf_ssl_ਲੱਭੋ_peer_by_ਕੁੰਜੀ(ਅੰਕੜੇ, scache, ssl_peer_ਕੁੰਜੀ, conn_config,
                                   &peer);
  ਜੇ(!ਨਤੀਜਾ && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)ਸਮਾਂ(ਨਿਰਾ));
    n = Curl_llist_ਸਿਰ(&peer->sessions);
    ਜੇ(n) {
      s = Curl_ਗੰਢ_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  ਜੇ(s) {
    *ps = s;
    CURL_TRC_SSLS(ਅੰਕੜੇ, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_ਕੁੰਜੀ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_ਗਿਣਤੀ(&peer->sessions));
  }
  ਨਹੀਂਤਾਂ {
    CURL_TRC_SSLS(ਅੰਕੜੇ, "no cached session for %s", ssl_peer_ਕੁੰਜੀ);
  }
  Curl_ssl_scache_unlock(ਅੰਕੜੇ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_ssl_scache_ਜੋੜੋ_obj(ਬਣਤਰ Curl_cfilter *cf,
                                 ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                                 ਖਾਲੀ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ Curl_ssl_scache_peer *peer = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  ਜੇ(!scache) {
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = cf_ssl_ਜੋੜੋ_peer(ਅੰਕੜੇ, scache, ssl_peer_ਕੁੰਜੀ, conn_config, &peer);
  ਜੇ(ਨਤੀਜਾ || !peer) {
    CURL_TRC_SSLS(ਅੰਕੜੇ, "unable to add scache peer: %d", (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  cf_ssl_scache_peer_ਸੈੱਟ_obj(peer, sobj, sobj_dtor_cb);
  sobj = ਨਿਰਾ;  /* peer took ownership */

ਬਾਹਰਲਾ:
  ਜੇ(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ *Curl_ssl_scache_get_obj(ਬਣਤਰ Curl_cfilter *cf,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ Curl_ssl_scache_peer *peer = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;
  ਖਾਲੀ *sobj;

  ਜੇ(!scache)
    ਵਾਪਸ ਨਿਰਾ;

  ਨਤੀਜਾ = cf_ssl_ਲੱਭੋ_peer_by_ਕੁੰਜੀ(ਅੰਕੜੇ, scache, ssl_peer_ਕੁੰਜੀ, conn_config,
                                   &peer);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਿਰਾ;

  sobj = peer ? peer->sobj : ਨਿਰਾ;

  CURL_TRC_SSLS(ਅੰਕੜੇ, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_ਕੁੰਜੀ);
  ਵਾਪਸ sobj;
}

ਖਾਲੀ Curl_ssl_scache_remove_all(ਬਣਤਰ Curl_cfilter *cf,
                                ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  ਬਣਤਰ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ਬਣਤਰ Curl_ssl_scache_peer *peer = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  (ਖਾਲੀ)cf;
  ਜੇ(!scache)
    ਵਾਪਸ;

  Curl_ssl_scache_ਤਾਲਾ(ਅੰਕੜੇ);
  ਨਤੀਜਾ = cf_ssl_ਲੱਭੋ_peer_by_ਕੁੰਜੀ(ਅੰਕੜੇ, scache, ssl_peer_ਕੁੰਜੀ, conn_config,
                                   &peer);
  ਜੇ(!ਨਤੀਜਾ && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(ਅੰਕੜੇ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSLS_EXPORT

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SSL_TICKET_MAX   (16 * 1024)

ਸਥਿਰ CURLcode cf_ssl_scache_peer_ਸੈੱਟ_hmac(ਬਣਤਰ Curl_ssl_scache_peer *peer)
{
  CURLcode ਨਤੀਜਾ;

  DEBUGASSERT(peer);
  ਜੇ(!peer->ssl_peer_ਕੁੰਜੀ)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਨਤੀਜਾ = Curl_rand(ਨਿਰਾ, peer->ਕੁੰਜੀ_salt, ਆਕਾਰ(peer->ਕੁੰਜੀ_salt));
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਨਤੀਜਾ = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->ਕੁੰਜੀ_salt, ਆਕਾਰ(peer->ਕੁੰਜੀ_salt),
                       (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)peer->ssl_peer_ਕੁੰਜੀ,
                       ਲੜੀ_ਲੰਬਾਈ(peer->ssl_peer_ਕੁੰਜੀ),
                       peer->ਕੁੰਜੀ_hmac);
  ਜੇ(!ਨਤੀਜਾ)
    peer->hmac_ਸੈੱਟ = TRUE;
  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode cf_ssl_ਲੱਭੋ_peer_by_hmac(ਬਣਤਰ Curl_ssl_scache *scache,
                                         ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *salt,
                                         ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *hmac,
                                         ਬਣਤਰ Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  *ppeer = ਨਿਰਾ;
  ਜੇ(!GOOD_SCACHE(scache))
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  ਲਈ(i = 0; scache && i < scache->peer_ਗਿਣਤੀ; i++) {
    ਬਣਤਰ Curl_ssl_scache_peer *peer = &scache->peers[i];
    ਜੇ(!cf_ssl_scache_ਮੇਲ_auth(peer, ਨਿਰਾ))
      ਜਾਰੀ;
    ਜੇ(scache->peers[i].hmac_ਸੈੱਟ &&
       !memcmp(peer->ਕੁੰਜੀ_salt, salt, ਆਕਾਰ(peer->ਕੁੰਜੀ_salt)) &&
       !memcmp(peer->ਕੁੰਜੀ_hmac, hmac, ਆਕਾਰ(peer->ਕੁੰਜੀ_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(peer->ssl_peer_ਕੁੰਜੀ) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ my_hmac[CURL_SHA256_DIGEST_ਲੰਮਾਈ];
      /* compute hmac for the passed salt */
      ਨਤੀਜਾ = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, ਆਕਾਰ(peer->ਕੁੰਜੀ_salt),
                           (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)peer->ssl_peer_ਕੁੰਜੀ,
                           ਲੜੀ_ਲੰਬਾਈ(peer->ssl_peer_ਕੁੰਜੀ),
                           my_hmac);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      ਜੇ(!memcmp(my_hmac, hmac, ਆਕਾਰ(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        ਜੇ(!peer->hmac_ਸੈੱਟ) {
          ਸਮ੍ਰਿਤੀ_ਨਕਲ(peer->ਕੁੰਜੀ_salt, salt, ਆਕਾਰ(peer->ਕੁੰਜੀ_salt));
          ਸਮ੍ਰਿਤੀ_ਨਕਲ(peer->ਕੁੰਜੀ_hmac, hmac, ਆਕਾਰ(peer->ਕੁੰਜੀ_hmac));
          peer->hmac_ਸੈੱਟ = TRUE;
        }
        *ppeer = peer;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
  }
ਬਾਹਰਲਾ:
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_ssl_session_import(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਅਚਲ ਅੱਖਰ *ssl_peer_ਕੁੰਜੀ,
                                 ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *shmac, size_t shmac_len,
                                 ਅਚਲ ਖਾਲੀ *sdata, size_t sdata_len)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  ਬਣਤਰ Curl_ssl_scache_peer *peer = ਨਿਰਾ;
  ਬਣਤਰ Curl_ssl_session *s = ਨਿਰਾ;
  bool have_ਤਾਲਾ = FALSE;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(!scache || !sdata || !sdata_len) {
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਜੇ(!ssl_peer_ਕੁੰਜੀ && (!shmac || !shmac_len)) {
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਨਤੀਜਾ = Curl_ssl_session_unpack(ਅੰਕੜੇ, sdata, sdata_len, &s);
  ਜੇ(ਨਤੀਜਾ)
    ਜਾਓ ਬਾਹਰਲਾ;

  Curl_ssl_scache_ਤਾਲਾ(ਅੰਕੜੇ);
  have_ਤਾਲਾ = TRUE;

  ਜੇ(ssl_peer_ਕੁੰਜੀ) {
    ਨਤੀਜਾ = cf_ssl_ਜੋੜੋ_peer(ਅੰਕੜੇ, scache, ssl_peer_ਕੁੰਜੀ, ਨਿਰਾ, &peer);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ ਜੇ(shmac_len != (ਆਕਾਰ(peer->ਕੁੰਜੀ_salt) + ਆਕਾਰ(peer->ਕੁੰਜੀ_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    ਨਤੀਜਾ = CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }
  ਨਹੀਂਤਾਂ {
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *salt = shmac;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *hmac = shmac + ਆਕਾਰ(peer->ਕੁੰਜੀ_salt);

    ਨਤੀਜਾ = cf_ssl_ਲੱਭੋ_peer_by_hmac(scache, salt, hmac, &peer);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਜੇ(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      ਜੇ(peer) {
        ਨਤੀਜਾ = cf_ssl_scache_peer_init(peer, ssl_peer_ਕੁੰਜੀ, ਨਿਰਾ, ਨਿਰਾ,
                                         salt, hmac);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
  }

  ਜੇ(peer) {
    cf_scache_peer_ਜੋੜੋ_session(peer, s, ਸਮਾਂ(ਨਿਰਾ));
    s = ਨਿਰਾ; /* peer is now owner */
    CURL_TRC_SSLS(ਅੰਕੜੇ, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_ਕੁੰਜੀ ? peer->ssl_peer_ਕੁੰਜੀ : "without key",
                  Curl_llist_ਗਿਣਤੀ(&peer->sessions));
  }

ਬਾਹਰਲਾ:
  ਜੇ(scache && have_ਤਾਲਾ)
    Curl_ssl_scache_unlock(ਅੰਕੜੇ);
  Curl_ssl_session_destroy(s);
  ਵਾਪਸ ਨਤੀਜਾ;
}

CURLcode Curl_ssl_session_export(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 curl_ssls_export_cb *export_fn,
                                 ਖਾਲੀ *userptr)
{
  ਬਣਤਰ Curl_ssl_scache *scache = cf_ssl_scache_get(ਅੰਕੜੇ);
  ਬਣਤਰ Curl_ssl_scache_peer *peer;
  ਬਣਤਰ dynbuf sbuf, hbuf;
  ਬਣਤਰ Curl_llist_ਗੰਢ *n;
  size_t i;
  curl_off_t now = ਸਮਾਂ(ਨਿਰਾ);
  CURLcode ਨਤੀਜਾ = CURLE_OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#ਜੇ_ਅੰਤ

  ਜੇ(!export_fn)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  Curl_ssl_scache_ਤਾਲਾ(ਅੰਕੜੇ);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_ਲੰਮਾਈ * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  ਲਈ(i = 0; scache && i < scache->peer_ਗਿਣਤੀ; i++) {
    peer = &scache->peers[i];
    ਜੇ(!peer->ssl_peer_ਕੁੰਜੀ && !peer->hmac_ਸੈੱਟ)
      ਜਾਰੀ; /* skip free entry */
    ਜੇ(!peer->exportable)
      ਜਾਰੀ;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_ਸਿਰ(&peer->sessions);
    ਜੇ(n)
      VERBOSE(++npeers);
    ਜਦੋਂ(n) {
      ਬਣਤਰ Curl_ssl_session *s = Curl_ਗੰਢ_elem(n);
      ਜੇ(!peer->hmac_ਸੈੱਟ) {
        ਨਤੀਜਾ = cf_ssl_scache_peer_ਸੈੱਟ_hmac(peer);
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
      }
      ਜੇ(!curlx_dyn_len(&hbuf)) {
        ਨਤੀਜਾ = curlx_dyn_addn(&hbuf, peer->ਕੁੰਜੀ_salt, ਆਕਾਰ(peer->ਕੁੰਜੀ_salt));
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
        ਨਤੀਜਾ = curlx_dyn_addn(&hbuf, peer->ਕੁੰਜੀ_hmac, ਆਕਾਰ(peer->ਕੁੰਜੀ_hmac));
        ਜੇ(ਨਤੀਜਾ)
          ਜਾਓ ਬਾਹਰਲਾ;
      }
      curlx_dyn_reset(&sbuf);
      ਨਤੀਜਾ = Curl_ssl_session_pack(ਅੰਕੜੇ, s, &sbuf);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;

      ਨਤੀਜਾ = export_fn(ਅੰਕੜੇ, userptr, peer->ssl_peer_ਕੁੰਜੀ,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      ਜੇ(ਨਤੀਜਾ)
        ਜਾਓ ਬਾਹਰਲਾ;
      VERBOSE(++ntickets);
      n = Curl_ਗੰਢ_ਅਗਲਾ(n);
    }
  }
  ਨਤੀਜਾ = CURLE_OK;
  CURL_TRC_SSLS(ਅੰਕੜੇ, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

ਬਾਹਰਲਾ:
  Curl_ssl_scache_unlock(ਅੰਕੜੇ);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* USE_SSLS_EXPORT */

#ਜੇ_ਅੰਤ /* USE_SSL */
