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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL

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
ರಚನೆ Curl_ssl_scache_peer {
  ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ;      /* id for peer + relevant TLS configuration */
  ಅಕ್ಷರ *clientcert;
  ಅಕ್ಷರ *ಕೀಲಿ_passwd;
  ರಚನೆ Curl_llist sessions;
  ಶೂನ್ಯ *sobj;              /* object instance or NULL */
  Curl_ssl_scache_obj_dtor *sobj_free; /* free `sobj` callback */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಕೀಲಿ_salt[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ]; /* for entry export */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಕೀಲಿ_hmac[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ]; /* for entry export */
  size_t max_sessions;
  ದೀರ್ಘ age;                /* a number, the higher the more recent */
  ಬಿಟ್(hmac_ಗಣ);           /* if key_salt and key_hmac are present */
  ಬಿಟ್(exportable);         /* sessions for this peer can be exported */
};

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SCACHE_MAGIC 0x000e1551

#ವ್ಯಾಖ್ಯಾನಿಸು GOOD_SCACHE(x) ((x) && (x)->magic == CURL_SCACHE_MAGIC)

/* Prefix the length to keep field boundaries unambiguous. */
ಸ್ಥಿರ CURLcode cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(ರಚನೆ dynbuf *buf,
                                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ)
{
  ಹಿಂತಿರುಗಿ curlx_dyn_addf(buf, ":%s-%zu:%s", name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ), ಮೌಲ್ಯ);
}

ಸ್ಥಿರ CURLcode cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_ಮಾರ್ಗ(ರಚನೆ dynbuf *buf,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                                         bool *is_local)
{
  ಆದರೆ(ಮಾರ್ಗ && ಮಾರ್ಗ[0]) {
    /* We try to add absolute paths, so that the session key can stay valid
     * when used in another process with different CWD. When a path does not
     * exist, this does not work. Then, we add the path as is. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
    ಅಕ್ಷರ *abspath = _fullpath(ಖಾಲಿ, ಮಾರ್ಗ, 0);
    ಆದರೆ(abspath) {
      CURLcode ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(buf, name, abspath);
      /* !checksrc! disable BANNEDFUNC 1 */
      ಬಿಡುಗಡೆ(abspath); /* allocated by CRT, use system free() */
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    *is_local = TRUE;
#ಅಥವಾಆದರೆ defined(HAVE_REALPATH)
    ಆದರೆ(ಮಾರ್ಗ[0] != '/') {
      ಅಕ್ಷರ *abspath = realpath(ಮಾರ್ಗ, ಖಾಲಿ);
      ಆದರೆ(abspath) {
        CURLcode ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(buf, name, abspath);
        /* !checksrc! disable BANNEDFUNC 1 */
        ಬಿಡುಗಡೆ(abspath); /* allocated by libc, free without memdebug */
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
      }
      *is_local = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(buf, name, ಮಾರ್ಗ);
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_hash(ರಚನೆ dynbuf *buf,
                                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                         ರಚನೆ curl_blob *blob)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(blob && blob->len) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ hash[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ];
    size_t i;

    ಫಲಿತಾಂಶ = curlx_dyn_addf(buf, ":%s-", name);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಫಲಿತಾಂಶ = Curl_sha256it(hash, blob->ದತ್ತಾಂಶ, blob->len);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಗಾಗಿ(i = 0; i < CURL_SHA256_DIGEST_ಉದ್ದಳತೆ; ++i) {
      ಫಲಿತಾಂಶ = curlx_dyn_addf(buf, "%02x", hash[i]);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_mtls(ರಚನೆ dynbuf *buf,
                                         ರಚನೆ ssl_filter_config *ssl,
                                         bool *is_local)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  ಆದರೆ(ssl->clientcert && ssl->clientcert[0]) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_ಮಾರ್ಗ(buf, "CCERT", ssl->clientcert, is_local);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->ಕೀಲಿ && ssl->ಕೀಲಿ[0]) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_ಮಾರ್ಗ(buf, "KEY", ssl->ಕೀಲಿ, is_local);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->ಕೀಲಿ_blob) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_hash(buf, "KEYBlob", ssl->ಕೀಲಿ_blob);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->cert_type && ssl->cert_type[0]) {
    size_t i;
    ಫಲಿತಾಂಶ = curlx_dyn_addf(buf, ":CT-%zu:", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ssl->cert_type));
    ಗಾಗಿ(i = 0; !ಫಲಿತಾಂಶ && ssl->cert_type[i]; i++) {
      ಅಕ್ಷರ c = Curl_raw_toupper(ssl->cert_type[i]);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(buf, &c, 1);
    }
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->ಕೀಲಿ_type && ssl->ಕೀಲಿ_type[0]) {
    size_t i;
    ಫಲಿತಾಂಶ = curlx_dyn_addf(buf, ":KT-%zu:", ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ssl->ಕೀಲಿ_type));
    ಗಾಗಿ(i = 0; !ಫಲಿತಾಂಶ && ssl->ಕೀಲಿ_type[i]; i++) {
      ಅಕ್ಷರ c = Curl_raw_toupper(ssl->ಕೀಲಿ_type[i]);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(buf, &c, 1);
    }
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SSLS_LOCAL_SUFFIX     ":L"
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SSLS_GLOBAL_SUFFIX    ":G"

ಸ್ಥಿರ bool cf_ssl_peer_ಕೀಲಿ_is_global(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *peer_ಕೀಲಿ)
{
  size_t len = peer_ಕೀಲಿ ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(peer_ಕೀಲಿ) : 0;
  ಹಿಂತಿರುಗಿ (len > 2) &&
         (peer_ಕೀಲಿ[len - 1] == 'G') &&
         (peer_ಕೀಲಿ[len - 2] == ':');
}

ಸ್ಥಿರ CURLcode ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_transport(ರಚನೆ dynbuf *buf,
                                           uint8_t transport)
{
  ಆಯ್ಕೆ(transport) {
  ಸ್ಥಿತಿ TRNSPRT_TCP:
    ಹಿಂತಿರುಗಿ CURLE_OK;
  ಸ್ಥಿತಿ TRNSPRT_UDP:
    ಹಿಂತಿರುಗಿ curlx_dyn_ಕೂಡಿಸು(buf, ":UDP");
  ಸ್ಥಿತಿ TRNSPRT_QUIC:
    ಹಿಂತಿರುಗಿ curlx_dyn_ಕೂಡಿಸು(buf, ":QUIC");
  ಸ್ಥಿತಿ TRNSPRT_UNIX:
    ಹಿಂತಿರುಗಿ curlx_dyn_ಕೂಡಿಸು(buf, ":UNIX");
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ curlx_dyn_addf(buf, ":TRNSPRT-%d", transport);
  }
}

ಸ್ಥಿರ CURLcode ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_vrfy(ರಚನೆ dynbuf *buf,
                                      ರಚನೆ ssl_filter_config *ssl,
                                      ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_peer *peer)
{
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!ssl->verifypeer) {
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(buf, ":NO-VRFY-PEER");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಆದರೆ(!ssl->verifyhost) {
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(buf, ":NO-VRFY-HOST");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಆದರೆ(ssl->verifystatus) {
    ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(buf, ":VRFY-STATUS");
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಆದರೆ((!ssl->verifypeer || !ssl->verifyhost) &&
     peer->peer && !Curl_peer_equal(peer->origin, peer->peer)) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(buf, "CHOST", peer->peer->hostname);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addf(buf, ":CPORT-%u", peer->peer->ಪೋರ್ಟ್);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode ssl_peer_ಕೀಲಿ_build(ರಚನೆ ssl_filter_config *ssl,
                                   ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_peer *peer,
                                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tls_id,
                                   ಅಕ್ಷರ **ppeer_ಕೀಲಿ)
{
  ರಚನೆ dynbuf buf;
  size_t ಕೀಲಿ_len;
  bool is_local = FALSE;
  CURLcode ಫಲಿತಾಂಶ;

  *ppeer_ಕೀಲಿ = ಖಾಲಿ;
  curlx_dyn_init(&buf, 10 * 1024);

  ಫಲಿತಾಂಶ = curlx_dyn_addf(&buf, "%s:%s:%d:KEY-V3",
                          peer->origin->scheme->name,
                          peer->origin->hostname, peer->origin->ಪೋರ್ಟ್);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಫಲಿತಾಂಶ = ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_transport(&buf, peer->transport);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಫಲಿತಾಂಶ = ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_vrfy(&buf, ssl, peer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;
  ಆದರೆ(ssl->ಆವೃತ್ತಿ_2 || ssl->ಆವೃತ್ತಿ_max) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&buf, ":TLSVER-%d-%u", ssl->ಆವೃತ್ತಿ_2,
                            (ssl->ಆವೃತ್ತಿ_max >> 16));
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->ssl_options) {
    ಫಲಿತಾಂಶ = curlx_dyn_addf(&buf, ":TLSOPT-%x", ssl->ssl_options);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->cipher_list) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(&buf, "CIPHER", ssl->cipher_list);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->cipher_list13) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(&buf, "CIPHER13", ssl->cipher_list13);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->curves) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(&buf, "CURVES", ssl->curves);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->signature_algorithms) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(&buf, "SIGALGS",
                                        ssl->signature_algorithms);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->verifypeer) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_ಮಾರ್ಗ(&buf, "CA", ssl->CAfile, &is_local);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_ಮಾರ್ಗ(&buf, "CApath",
                                        ssl->CApath, &is_local);
    ಆದರೆ(!ಫಲಿತಾಂಶ && ssl->native_ca_store)
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&buf, ":NATIVE-CA");
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_ಮಾರ್ಗ(&buf, "CRL", ssl->CRLfile, &is_local);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_ಮಾರ್ಗ(&buf, "Issuer", ssl->issuercert,
                                      &is_local);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಆದರೆ(ssl->ca_info_blob) {
      ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_hash(&buf, "CAInfoBlob", ssl->ca_info_blob);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(ssl->issuercert_blob) {
      ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_hash(&buf, "IssuerBlob",
                                        ssl->issuercert_blob);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  ಆದರೆ(ssl->cert_blob) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_hash(&buf, "CertBlob", ssl->cert_blob);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ssl->pinned_ಕೀಲಿ && ssl->pinned_ಕೀಲಿ[0]) {
    ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(&buf, "Pinned", ssl->pinned_ಕೀಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_mtls(&buf, ssl, &is_local);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಆದರೆ(!tls_id || !tls_id[0]) {
    ಫಲಿತಾಂಶ = CURLE_FAILED_INIT;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = cf_ssl_peer_ಕೀಲಿ_ಕೂಡಿಸು_string(&buf, "IMPL", tls_id);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  ಫಲಿತಾಂಶ = curlx_dyn_addf(&buf, is_local ?
                          CURL_SSLS_LOCAL_SUFFIX : CURL_SSLS_GLOBAL_SUFFIX);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  *ppeer_ಕೀಲಿ = curlx_dyn_take(&buf, &ಕೀಲಿ_len);
  /* we added printable char, and dynbuf always null-terminates, no need
   * to track length */

ಹೊರಹೋಗು:
  curlx_dyn_free(&buf);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_ssl_peer_ಕೀಲಿ_make(ಸ್ಥಿರಾಂಕ ರಚನೆ ssl_peer *peer,
                                ರಚನೆ ssl_filter_config *sslc,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tls_id,
                                ಅಕ್ಷರ **ppeer_ಕೀಲಿ)
{
  ಹಿಂತಿರುಗಿ ssl_peer_ಕೀಲಿ_build(sslc, peer, tls_id, ppeer_ಕೀಲಿ);
}

ರಚನೆ Curl_ssl_scache {
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ magic;
  ರಚನೆ Curl_ssl_scache_peer *peers;
  size_t peer_ಎಣಿಕೆ;
  ಪೂರ್ಣಾಂಕ default_lifetime_secs;
  ದೀರ್ಘ age;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  curl_mutex_t mutex;
  curl_thread_id_t locking_thread;
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(is_locked);
};

ಸ್ಥಿರ ರಚನೆ Curl_ssl_scache *cf_ssl_scache_get(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_ssl_scache *scache = ಖಾಲಿ;
  /* If a share is present, its ssl_scache has preference over the multi */
  ಆದರೆ(ದತ್ತಾಂಶ->share && ದತ್ತಾಂಶ->share->ssl_scache)
    scache = ದತ್ತಾಂಶ->share->ssl_scache;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ದತ್ತಾಂಶ->multi && ದತ್ತಾಂಶ->multi->ssl_scache)
    scache = ದತ್ತಾಂಶ->multi->ssl_scache;
  ಆದರೆ(scache && !GOOD_SCACHE(scache)) {
    failf(ದತ್ತಾಂಶ, "transfer would use an invalid scache at %p, denied",
          (ಶೂನ್ಯ *)scache);
    DEBUGASSERT(0);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ scache;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ssl_scache_session_ldestroy(ಶೂನ್ಯ *udata, ಶೂನ್ಯ *obj)
{
  ರಚನೆ Curl_ssl_session *s = obj;
  (ಶೂನ್ಯ)udata;
  curlx_free(s->sdata);
  curlx_free(s->quic_tp);
  curlx_free(s->alpn);
  curlx_free(s);
}

CURLcode Curl_ssl_session_create(ಶೂನ್ಯ *sdata, size_t sdata_len,
                                 ಪೂರ್ಣಾಂಕ ietf_tls_id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn,
                                 curl_off_t valid_until, size_t earlydata_max,
                                 ರಚನೆ Curl_ssl_session **psession)
{
  ಹಿಂತಿರುಗಿ Curl_ssl_session_create2(sdata, sdata_len, ietf_tls_id, alpn,
                                  valid_until, earlydata_max,
                                  ಖಾಲಿ, 0, psession);
}

CURLcode Curl_ssl_session_create2(ಶೂನ್ಯ *sdata, size_t sdata_len,
                                  ಪೂರ್ಣಾಂಕ ietf_tls_id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alpn,
                                  curl_off_t valid_until, size_t earlydata_max,
                                  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *quic_tp, size_t quic_tp_len,
                                  ರಚನೆ Curl_ssl_session **psession)
{
  ರಚನೆ Curl_ssl_session *s;

  ಆದರೆ(!sdata || !sdata_len) {
    curlx_free(sdata);
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  *psession = ಖಾಲಿ;
  s = curlx_calloc(1, ಗಾತ್ರ(*s));
  ಆದರೆ(!s) {
    curlx_free(sdata);
    curlx_free(quic_tp);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  s->ietf_tls_id = ietf_tls_id;
  s->valid_until = valid_until;
  s->earlydata_max = earlydata_max;
  s->sdata = sdata;
  s->sdata_len = sdata_len;
  s->quic_tp = quic_tp;
  s->quic_tp_len = quic_tp_len;
  ಆದರೆ(alpn) {
    s->alpn = curlx_strdup(alpn);
    ಆದರೆ(!s->alpn) {
      cf_ssl_scache_session_ldestroy(ಖಾಲಿ, s);
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  *psession = s;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssl_session_destroy(ರಚನೆ Curl_ssl_session *s)
{
  ಆದರೆ(s) {
    /* if in the list, the list destructor takes care of it */
    ಆದರೆ(Curl_ಗಂಟು_llist(&s->list))
      Curl_ಗಂಟು_remove(&s->list);
    ಇಲ್ಲದಿದ್ದರೆ {
      cf_ssl_scache_session_ldestroy(ಖಾಲಿ, s);
    }
  }
}

CURLcode Curl_ssl_session_dup(ರಚನೆ Curl_ssl_session *src,
                              ರಚನೆ Curl_ssl_session **pdest)
{
  ರಚನೆ Curl_ssl_session *dest = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  ಆದರೆ(!src || !pdest)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  *pdest = ಖಾಲಿ;

  dest = curlx_calloc(1, ಗಾತ್ರ(*dest));
  ಆದರೆ(!dest)
    ಹೋಗು ಹೊರಹೋಗು;

  dest->ietf_tls_id = src->ietf_tls_id;
  dest->valid_until = src->valid_until;
  dest->earlydata_max = src->earlydata_max;
  dest->sectrust_verified = src->sectrust_verified;
  ಆದರೆ(src->sdata_len) {
    dest->sdata = curlx_memdup(src->sdata, src->sdata_len);
    ಆದರೆ(!dest->sdata)
      ಹೋಗು ಹೊರಹೋಗು;
    dest->sdata_len = src->sdata_len;
  }
  ಆದರೆ(src->quic_tp_len) {
    dest->quic_tp = curlx_memdup(src->quic_tp, src->quic_tp_len);
    ಆದರೆ(!dest->quic_tp)
      ಹೋಗು ಹೊರಹೋಗು;
    dest->quic_tp_len = src->quic_tp_len;
  }
  ಆದರೆ(src->alpn) {
    dest->alpn = curlx_strdup(src->alpn);
    ಆದರೆ(!dest->alpn)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಫಲಿತಾಂಶ = CURLE_OK;

ಹೊರಹೋಗು:
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    *pdest = dest;
  ಇಲ್ಲದಿದ್ದರೆ {
    *pdest = ಖಾಲಿ;
    ಆದರೆ(dest)
      Curl_ssl_session_destroy(dest);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ssl_scache_clear_peer(ರಚನೆ Curl_ssl_scache_peer *peer)
{
  Curl_llist_destroy(&peer->sessions, ಖಾಲಿ);
  ಆದರೆ(peer->sobj) {
    DEBUGASSERT(peer->sobj_free);
    ಆದರೆ(peer->sobj_free)
      peer->sobj_free(peer->sobj);
    peer->sobj = ಖಾಲಿ;
  }
  peer->sobj_free = ಖಾಲಿ;
  curlx_safefree(peer->clientcert);
  curlx_safefree(peer->ಕೀಲಿ_passwd);
  curlx_safefree(peer->ssl_peer_ಕೀಲಿ);
  peer->age = 0;
  peer->hmac_ಗಣ = FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ssl_scache_peer_ಗಣ_obj(ರಚನೆ Curl_ssl_scache_peer *peer,
                                       ಶೂನ್ಯ *sobj,
                                       Curl_ssl_scache_obj_dtor *sobj_free)
{
  DEBUGASSERT(peer);
  ಆದರೆ(peer->sobj_free) {
    peer->sobj_free(peer->sobj);
  }
  peer->sobj = sobj;
  peer->sobj_free = sobj_free;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_ssl_cache_peer_update(ರಚನೆ Curl_ssl_scache_peer *peer)
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

ಸ್ಥಿರ CURLcode cf_ssl_scache_peer_init(ರಚನೆ Curl_ssl_scache_peer *peer,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *clientcert,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ_passwd,
                                        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *salt,
                                        ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *hmac)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  DEBUGASSERT(!peer->ssl_peer_ಕೀಲಿ);
  ಆದರೆ(ssl_peer_ಕೀಲಿ) {
    peer->ssl_peer_ಕೀಲಿ = curlx_strdup(ssl_peer_ಕೀಲಿ);
    ಆದರೆ(!peer->ssl_peer_ಕೀಲಿ)
      ಹೋಗು ಹೊರಹೋಗು;
    peer->hmac_ಗಣ = FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(salt && hmac) {
    ಸ್ಮರಣೆ_ನಕಲು(peer->ಕೀಲಿ_salt, salt, ಗಾತ್ರ(peer->ಕೀಲಿ_salt));
    ಸ್ಮರಣೆ_ನಕಲು(peer->ಕೀಲಿ_hmac, hmac, ಗಾತ್ರ(peer->ಕೀಲಿ_hmac));
    peer->hmac_ಗಣ = TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(clientcert) {
    peer->clientcert = curlx_strdup(clientcert);
    ಆದರೆ(!peer->clientcert)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(ಕೀಲಿ_passwd) {
    peer->ಕೀಲಿ_passwd = curlx_strdup(ಕೀಲಿ_passwd);
    ಆದರೆ(!peer->ಕೀಲಿ_passwd)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  cf_ssl_cache_peer_update(peer);
  ಫಲಿತಾಂಶ = CURLE_OK;
ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ)
    cf_ssl_scache_clear_peer(peer);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_scache_session_remove(ರಚನೆ Curl_ssl_scache_peer *peer,
                                     ರಚನೆ Curl_ssl_session *s)
{
  (ಶೂನ್ಯ)peer;
  DEBUGASSERT(Curl_ಗಂಟು_llist(&s->list) == &peer->sessions);
  Curl_ssl_session_destroy(s);
}

ಸ್ಥಿರ bool cf_scache_session_expired(ರಚನೆ Curl_ssl_session *s,
                                      curl_off_t now)
{
  ಹಿಂತಿರುಗಿ (s->valid_until > 0) && (s->valid_until < now);
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_scache_peer_remove_expired(ರಚನೆ Curl_ssl_scache_peer *peer,
                                          curl_off_t now)
{
  ರಚನೆ Curl_llist_ಗಂಟು *n = Curl_llist_ತಲೆ(&peer->sessions);
  ಆಗ(n) {
    ರಚನೆ Curl_ssl_session *s = Curl_ಗಂಟು_elem(n);
    n = Curl_ಗಂಟು_ಮುಂದಿನ(n);
    ಆದರೆ(cf_scache_session_expired(s, now))
      cf_scache_session_remove(peer, s);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_scache_peer_remove_non13(ರಚನೆ Curl_ssl_scache_peer *peer)
{
  ರಚನೆ Curl_llist_ಗಂಟು *n = Curl_llist_ತಲೆ(&peer->sessions);
  ಆಗ(n) {
    ರಚನೆ Curl_ssl_session *s = Curl_ಗಂಟು_elem(n);
    n = Curl_ಗಂಟು_ಮುಂದಿನ(n);
    ಆದರೆ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3)
      cf_scache_session_remove(peer, s);
  }
}

CURLcode Curl_ssl_scache_create(size_t max_peers,
                                size_t max_sessions_ಪ್ರತಿ_peer,
                                ರಚನೆ Curl_ssl_scache **pscache)
{
  ರಚನೆ Curl_ssl_scache *scache;
  ರಚನೆ Curl_ssl_scache_peer *peers;
  size_t i;

  *pscache = ಖಾಲಿ;
  peers = curlx_calloc(max_peers, ಗಾತ್ರ(*peers));
  ಆದರೆ(!peers)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  scache = curlx_calloc(1, ಗಾತ್ರ(*scache));
  ಆದರೆ(!scache) {
    curlx_free(peers);
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  scache->magic = CURL_SCACHE_MAGIC;
  scache->default_lifetime_secs = (24 * 60 * 60); /* 1 day */
  scache->peer_ಎಣಿಕೆ = max_peers;
  scache->peers = peers;
  scache->age = 1;
  ಗಾಗಿ(i = 0; i < scache->peer_ಎಣಿಕೆ; ++i) {
    scache->peers[i].max_sessions = max_sessions_ಪ್ರತಿ_peer;
    Curl_llist_init(&scache->peers[i].sessions,
                    cf_ssl_scache_session_ldestroy);
  }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  Curl_mutex_init(&scache->mutex);
#ಆದರೆ_ಅಂತ್ಯ
  *pscache = scache;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಶೂನ್ಯ Curl_ssl_scache_destroy(ರಚನೆ Curl_ssl_scache *scache)
{
  ಆದರೆ(GOOD_SCACHE(scache)) {
    size_t i;
    scache->magic = 0;
    ಗಾಗಿ(i = 0; i < scache->peer_ಎಣಿಕೆ; ++i) {
      cf_ssl_scache_clear_peer(&scache->peers[i]);
    }
    curlx_free(scache->peers);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
    Curl_mutex_destroy(&scache->mutex);
#ಆದರೆ_ಅಂತ್ಯ
    curlx_free(scache);
  }
}

bool Curl_ssl_scache_use(ರಚನೆ Curl_cfilter *cf, ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ಆದರೆ(cf_ssl_scache_get(ದತ್ತಾಂಶ)) {
    ರಚನೆ ssl_filter_config *conn_config =
      Curl_ssl_cf_get_filter_config(cf);
    ಹಿಂತಿರುಗಿ conn_config ? conn_config->cache_session : FALSE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/* Lock shared SSL session data */
ಶೂನ್ಯ Curl_ssl_scache_ಬೀಗ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  ಆದರೆ(scache) {
    ಆದರೆ(CURL_SHARE_ssl_scache(ದತ್ತಾಂಶ))
      Curl_share_ಬೀಗ(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_SSL_SESSION,
                      CURL_ಬೀಗ_ACCESS_SINGLE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = Curl_thread_get_current_id();
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
    Curl_mutex_release(&scache->mutex);
#ಇಲ್ಲದಿದ್ದರೆ
    DEBUGASSERT(!scache->is_locked);
    scache->is_locked = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
  }
}

/* Unlock shared SSL session data */
ಶೂನ್ಯ Curl_ssl_scache_unlock(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  ಆದರೆ(scache) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
    Curl_mutex_acquire(&scache->mutex);
    scache->locking_thread = 0;
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
    Curl_mutex_release(&scache->mutex);
#ಇಲ್ಲದಿದ್ದರೆ
    DEBUGASSERT(scache->is_locked);
    scache->is_locked = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ(CURL_SHARE_ssl_scache(ದತ್ತಾಂಶ))
      Curl_share_unlock(ದತ್ತಾಂಶ, CURL_ಬೀಗ_ದತ್ತಾಂಶ_SSL_SESSION);
  }
}

bool Curl_ssl_scache_is_locked_by_current_thread(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  bool locked = FALSE;
  ಆದರೆ(!scache)
    ಹಿಂತಿರುಗಿ FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_MUTEX
  Curl_mutex_acquire(&scache->mutex);
  locked = scache->is_locked && Curl_thread_is_current(scache->locking_thread);
  Curl_mutex_release(&scache->mutex);
#ಇಲ್ಲದಿದ್ದರೆ
  locked = (bool)scache->is_locked;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ locked;
}

ಸ್ಥಿರ bool cf_ssl_scache_ಹೊಂದಿಕೆ_auth(ರಚನೆ Curl_ssl_scache_peer *peer,
                                     ರಚನೆ ssl_filter_config *conn_config)
{
  ಆದರೆ(!conn_config) {
    ಆದರೆ(peer->clientcert || peer->ಕೀಲಿ_passwd)
      ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!Curl_safecmp(peer->clientcert, conn_config->clientcert))
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(Curl_timestrcmp(peer->ಕೀಲಿ_passwd, conn_config->ಕೀಲಿ_passwd))
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ TRUE;
}

ಸ್ಥಿರ CURLcode cf_ssl_ಕಂಡುಹಿಡಿ_peer_by_ಕೀಲಿ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ Curl_ssl_scache *scache,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                                        ರಚನೆ ssl_filter_config *conn_config,
                                        ರಚನೆ Curl_ssl_scache_peer **ppeer)
{
  size_t i, peer_ಕೀಲಿ_len = 0;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *ppeer = ಖಾಲಿ;
  ಆದರೆ(!GOOD_SCACHE(scache)) {
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
  }

  CURL_TRC_SSLS(ದತ್ತಾಂಶ, "find peer slot for %s among %zu slots",
                ssl_peer_ಕೀಲಿ, scache->peer_ಎಣಿಕೆ);

  /* check for entries with known peer_key */
  ಗಾಗಿ(i = 0; scache && i < scache->peer_ಎಣಿಕೆ; i++) {
    ಆದರೆ(scache->peers[i].ssl_peer_ಕೀಲಿ &&
       !strcmp(ssl_peer_ಕೀಲಿ, scache->peers[i].ssl_peer_ಕೀಲಿ) &&
       cf_ssl_scache_ಹೊಂದಿಕೆ_auth(&scache->peers[i], conn_config)) {
      /* yes, we have a cached session for this! */
      *ppeer = &scache->peers[i];
      ಹೋಗು ಹೊರಹೋಗು;
    }
  }
  /* check for entries with HMAC set but no known peer_key */
  ಗಾಗಿ(i = 0; scache && i < scache->peer_ಎಣಿಕೆ; i++) {
    ಆದರೆ(!scache->peers[i].ssl_peer_ಕೀಲಿ &&
       scache->peers[i].hmac_ಗಣ &&
       cf_ssl_scache_ಹೊಂದಿಕೆ_auth(&scache->peers[i], conn_config)) {
      /* possible entry with unknown peer_key, check hmac */
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ my_hmac[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ];
      ಆದರೆ(!peer_ಕೀಲಿ_len) /* we are lazy */
        peer_ಕೀಲಿ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ssl_peer_ಕೀಲಿ);
      ಫಲಿತಾಂಶ = Curl_hmacit(&Curl_HMAC_SHA256,
                           scache->peers[i].ಕೀಲಿ_salt,
                           ಗಾತ್ರ(scache->peers[i].ಕೀಲಿ_salt),
                           (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ssl_peer_ಕೀಲಿ,
                           peer_ಕೀಲಿ_len,
                           my_hmac);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      ಆದರೆ(!memcmp(scache->peers[i].ಕೀಲಿ_hmac, my_hmac, ಗಾತ್ರ(my_hmac))) {
        /* remember peer_key for future lookups */
        CURL_TRC_SSLS(ದತ್ತಾಂಶ, "peer entry %zu key recovered: %s",
                      i, ssl_peer_ಕೀಲಿ);
        scache->peers[i].ssl_peer_ಕೀಲಿ = curlx_strdup(ssl_peer_ಕೀಲಿ);
        ಆದರೆ(!scache->peers[i].ssl_peer_ಕೀಲಿ) {
          ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
          ಹೋಗು ಹೊರಹೋಗು;
        }
        cf_ssl_cache_peer_update(&scache->peers[i]);
        *ppeer = &scache->peers[i];
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
  }
  CURL_TRC_SSLS(ದತ್ತಾಂಶ, "peer not found for %s", ssl_peer_ಕೀಲಿ);
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ರಚನೆ Curl_ssl_scache_peer *cf_ssl_get_free_peer(
  ರಚನೆ Curl_ssl_scache *scache)
{
  ರಚನೆ Curl_ssl_scache_peer *peer = ಖಾಲಿ;
  size_t i;

  /* find empty or oldest peer */
  ಗಾಗಿ(i = 0; i < scache->peer_ಎಣಿಕೆ; ++i) {
    /* free peer entry? */
    ಆದರೆ(!scache->peers[i].ssl_peer_ಕೀಲಿ && !scache->peers[i].hmac_ಗಣ) {
      peer = &scache->peers[i];
      ನಿಲ್ಲಿಸು;
    }
    /* peer without sessions and obj */
    ಆದರೆ(!scache->peers[i].sobj &&
       !Curl_llist_ಎಣಿಕೆ(&scache->peers[i].sessions)) {
      peer = &scache->peers[i];
      ನಿಲ್ಲಿಸು;
    }
    /* remember "oldest" peer */
    ಆದರೆ(!peer || (scache->peers[i].age < peer->age)) {
      peer = &scache->peers[i];
    }
  }
  DEBUGASSERT(peer);
  ಆದರೆ(peer)
    cf_ssl_scache_clear_peer(peer);
  ಹಿಂತಿರುಗಿ peer;
}

ಸ್ಥಿರ CURLcode cf_ssl_ಕೂಡಿಸು_peer(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ Curl_ssl_scache *scache,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                                ರಚನೆ ssl_filter_config *conn_config,
                                ರಚನೆ Curl_ssl_scache_peer **ppeer)
{
  ರಚನೆ Curl_ssl_scache_peer *peer = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *ppeer = ಖಾಲಿ;
  ಆದರೆ(ssl_peer_ಕೀಲಿ) {
    ಫಲಿತಾಂಶ = cf_ssl_ಕಂಡುಹಿಡಿ_peer_by_ಕೀಲಿ(ದತ್ತಾಂಶ, scache, ssl_peer_ಕೀಲಿ, conn_config,
                                     &peer);
    ಆದರೆ(ಫಲಿತಾಂಶ || !scache->peer_ಎಣಿಕೆ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }

  ಆದರೆ(peer) {
    *ppeer = peer;
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  peer = cf_ssl_get_free_peer(scache);
  ಆದರೆ(peer) {
    ಅಕ್ಷರ ಬಫರ್[64];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ccert = conn_config ? conn_config->clientcert : ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *kpasswd = conn_config ? conn_config->ಕೀಲಿ_passwd : ಖಾಲಿ;
    ಆದರೆ(!ccert && conn_config && conn_config->cert_blob) {
      /* when using a client cert blob, create a name for it */
      curl_msnprintf(ಬಫರ್, ಗಾತ್ರ(ಬಫರ್),
                     "cert-%p", conn_config->cert_blob->ದತ್ತಾಂಶ);
      ccert = ಬಫರ್; /* data is strduped by cf_ssl_scache_peer_init */
    }
    ಫಲಿತಾಂಶ = cf_ssl_scache_peer_init(peer, ssl_peer_ಕೀಲಿ, ccert, kpasswd,
                                     ಖಾಲಿ, ಖಾಲಿ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    /* all ready */
    *ppeer = peer;
    ಫಲಿತಾಂಶ = CURLE_OK;
  }

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    cf_ssl_scache_clear_peer(peer);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಶೂನ್ಯ cf_scache_peer_ಕೂಡಿಸು_session(ರಚನೆ Curl_ssl_scache_peer *peer,
                                       ರಚನೆ Curl_ssl_session *s,
                                       curl_off_t now)
{
  /* A session not from TLSv1.3 replaces all other. */
  ಆದರೆ(s->ietf_tls_id != CURL_IETF_PROTO_TLS1_3) {
    Curl_llist_destroy(&peer->sessions, ಖಾಲಿ);
    Curl_llist_append(&peer->sessions, s, &s->list);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* Expire existing, append, trim from head to obey max_sessions */
    cf_scache_peer_remove_expired(peer, now);
    cf_scache_peer_remove_non13(peer);
    Curl_llist_append(&peer->sessions, s, &s->list);
    ಆಗ(Curl_llist_ಎಣಿಕೆ(&peer->sessions) > peer->max_sessions) {
      Curl_ಗಂಟು_remove(Curl_llist_ತಲೆ(&peer->sessions));
    }
  }
}

ಸ್ಥಿರ CURLcode cf_scache_ಕೂಡಿಸು_session(ರಚನೆ Curl_cfilter *cf,
                                      ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                      ರಚನೆ Curl_ssl_scache *scache,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                                      ರಚನೆ Curl_ssl_session *s)
{
  ರಚನೆ Curl_ssl_scache_peer *peer = ಖಾಲಿ;
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  curl_off_t now = (curl_off_t)ಸಮಯ(ಖಾಲಿ);
  curl_off_t max_lifetime;

  ಆದರೆ(!scache || !scache->peer_ಎಣಿಕೆ) {
    Curl_ssl_session_destroy(s);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಆದರೆ(s->valid_until <= 0)
    s->valid_until = now + scache->default_lifetime_secs;

  max_lifetime = (s->ietf_tls_id == CURL_IETF_PROTO_TLS1_3) ?
                 CURL_SCACHE_MAX_13_LIFETIME_SEC :
                 CURL_SCACHE_MAX_12_LIFETIME_SEC;
  ಆದರೆ(s->valid_until > (now + max_lifetime))
    s->valid_until = now + max_lifetime;

  ಆದರೆ(cf_scache_session_expired(s, now)) {
    CURL_TRC_SSLS(ದತ್ತಾಂಶ, "add, session already expired");
    Curl_ssl_session_destroy(s);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  ಫಲಿತಾಂಶ = cf_ssl_ಕೂಡಿಸು_peer(ದತ್ತಾಂಶ, scache, ssl_peer_ಕೀಲಿ, conn_config, &peer);
  ಆದರೆ(ಫಲಿತಾಂಶ || !peer) {
    CURL_TRC_SSLS(ದತ್ತಾಂಶ, "unable to add scache peer: %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    Curl_ssl_session_destroy(s);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  cf_scache_peer_ಕೂಡಿಸು_session(peer, s, now);

ಹೊರಹೋಗು:
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    failf(ದತ್ತಾಂಶ, "[SCACHE] failed to add session for %s, error=%d",
          ssl_peer_ಕೀಲಿ, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    CURL_TRC_SSLS(ದತ್ತಾಂಶ, "added session for %s [proto=0x%x, "
                  "valid_secs=%" FMT_OFF_T ", alpn=%s, earlydata=%zu, "
                  "quic_tp=%s], peer has %zu sessions now", ssl_peer_ಕೀಲಿ,
                  (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)s->ietf_tls_id, s->valid_until - now,
                  s->alpn, s->earlydata_max, s->quic_tp ? "yes" : "no",
                  peer ? Curl_llist_ಎಣಿಕೆ(&peer->sessions) : 0);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_ssl_scache_put(ರಚನೆ Curl_cfilter *cf,
                             ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                             ರಚನೆ Curl_ssl_session *s)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  CURLcode ಫಲಿತಾಂಶ;
  DEBUGASSERT(conn_config);

  ಆದರೆ(!scache || !conn_config->cache_session) {
    Curl_ssl_session_destroy(s);
    ಹಿಂತಿರುಗಿ CURLE_OK;
  }

  Curl_ssl_scache_ಬೀಗ(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = cf_scache_ಕೂಡಿಸು_session(cf, ದತ್ತಾಂಶ, scache, ssl_peer_ಕೀಲಿ, s);
  Curl_ssl_scache_unlock(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ Curl_ssl_scache_return(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                            ರಚನೆ Curl_ssl_session *s)
{
  /* See RFC 8446 C.4:
   * "Clients SHOULD NOT reuse a ticket for multiple connections." */
  ಆದರೆ(s && s->ietf_tls_id < 0x304)
    (ಶೂನ್ಯ)Curl_ssl_scache_put(cf, ದತ್ತಾಂಶ, ssl_peer_ಕೀಲಿ, s);
  ಇಲ್ಲದಿದ್ದರೆ
    Curl_ssl_session_destroy(s);
}

CURLcode Curl_ssl_scache_take(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                              ರಚನೆ Curl_ssl_session **ps)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ Curl_ssl_scache_peer *peer = ಖಾಲಿ;
  ರಚನೆ Curl_llist_ಗಂಟು *n;
  ರಚನೆ Curl_ssl_session *s = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  *ps = ಖಾಲಿ;
  ಆದರೆ(!scache)
    ಹಿಂತಿರುಗಿ CURLE_OK;

  Curl_ssl_scache_ಬೀಗ(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = cf_ssl_ಕಂಡುಹಿಡಿ_peer_by_ಕೀಲಿ(ದತ್ತಾಂಶ, scache, ssl_peer_ಕೀಲಿ, conn_config,
                                   &peer);
  ಆದರೆ(!ಫಲಿತಾಂಶ && peer) {
    cf_scache_peer_remove_expired(peer, (curl_off_t)ಸಮಯ(ಖಾಲಿ));
    n = Curl_llist_ತಲೆ(&peer->sessions);
    ಆದರೆ(n) {
      s = Curl_ಗಂಟು_take_elem(n);
      scache->age++;           /* increase general age */
      peer->age = scache->age; /* set this as used in this age */
    }
  }
  ಆದರೆ(s) {
    *ps = s;
    CURL_TRC_SSLS(ದತ್ತಾಂಶ, "took session for %s [proto=0x%x, "
                  "alpn=%s, earlydata=%zu, quic_tp=%s], %zu sessions remain",
                  ssl_peer_ಕೀಲಿ, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)s->ietf_tls_id, s->alpn,
                  s->earlydata_max, s->quic_tp ? "yes" : "no",
                  Curl_llist_ಎಣಿಕೆ(&peer->sessions));
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    CURL_TRC_SSLS(ದತ್ತಾಂಶ, "no cached session for %s", ssl_peer_ಕೀಲಿ);
  }
  Curl_ssl_scache_unlock(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_ssl_scache_ಕೂಡಿಸು_obj(ರಚನೆ Curl_cfilter *cf,
                                 ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                                 ಶೂನ್ಯ *sobj,
                                 Curl_ssl_scache_obj_dtor *sobj_dtor_cb)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ Curl_ssl_scache_peer *peer = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(sobj);
  DEBUGASSERT(sobj_dtor_cb);

  ಆದರೆ(!scache) {
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = cf_ssl_ಕೂಡಿಸು_peer(ದತ್ತಾಂಶ, scache, ssl_peer_ಕೀಲಿ, conn_config, &peer);
  ಆದರೆ(ಫಲಿತಾಂಶ || !peer) {
    CURL_TRC_SSLS(ದತ್ತಾಂಶ, "unable to add scache peer: %d", (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಹೋಗು ಹೊರಹೋಗು;
  }

  cf_ssl_scache_peer_ಗಣ_obj(peer, sobj, sobj_dtor_cb);
  sobj = ಖಾಲಿ;  /* peer took ownership */

ಹೊರಹೋಗು:
  ಆದರೆ(sobj && sobj_dtor_cb)
    sobj_dtor_cb(sobj);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ *Curl_ssl_scache_get_obj(ರಚನೆ Curl_cfilter *cf,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ Curl_ssl_scache_peer *peer = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;
  ಶೂನ್ಯ *sobj;

  ಆದರೆ(!scache)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಫಲಿತಾಂಶ = cf_ssl_ಕಂಡುಹಿಡಿ_peer_by_ಕೀಲಿ(ದತ್ತಾಂಶ, scache, ssl_peer_ಕೀಲಿ, conn_config,
                                   &peer);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  sobj = peer ? peer->sobj : ಖಾಲಿ;

  CURL_TRC_SSLS(ದತ್ತಾಂಶ, "%s cached session for '%s'",
                sobj ? "Found" : "No", ssl_peer_ಕೀಲಿ);
  ಹಿಂತಿರುಗಿ sobj;
}

ಶೂನ್ಯ Curl_ssl_scache_remove_all(ರಚನೆ Curl_cfilter *cf,
                                ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  ರಚನೆ ssl_filter_config *conn_config = Curl_ssl_cf_get_filter_config(cf);
  ರಚನೆ Curl_ssl_scache_peer *peer = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  (ಶೂನ್ಯ)cf;
  ಆದರೆ(!scache)
    ಹಿಂತಿರುಗಿ;

  Curl_ssl_scache_ಬೀಗ(ದತ್ತಾಂಶ);
  ಫಲಿತಾಂಶ = cf_ssl_ಕಂಡುಹಿಡಿ_peer_by_ಕೀಲಿ(ದತ್ತಾಂಶ, scache, ssl_peer_ಕೀಲಿ, conn_config,
                                   &peer);
  ಆದರೆ(!ಫಲಿತಾಂಶ && peer)
    cf_ssl_scache_clear_peer(peer);
  Curl_ssl_scache_unlock(ದತ್ತಾಂಶ);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSLS_EXPORT

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SSL_TICKET_MAX   (16 * 1024)

ಸ್ಥಿರ CURLcode cf_ssl_scache_peer_ಗಣ_hmac(ರಚನೆ Curl_ssl_scache_peer *peer)
{
  CURLcode ಫಲಿತಾಂಶ;

  DEBUGASSERT(peer);
  ಆದರೆ(!peer->ssl_peer_ಕೀಲಿ)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಫಲಿತಾಂಶ = Curl_rand(ಖಾಲಿ, peer->ಕೀಲಿ_salt, ಗಾತ್ರ(peer->ಕೀಲಿ_salt));
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಫಲಿತಾಂಶ = Curl_hmacit(&Curl_HMAC_SHA256,
                       peer->ಕೀಲಿ_salt, ಗಾತ್ರ(peer->ಕೀಲಿ_salt),
                       (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)peer->ssl_peer_ಕೀಲಿ,
                       ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(peer->ssl_peer_ಕೀಲಿ),
                       peer->ಕೀಲಿ_hmac);
  ಆದರೆ(!ಫಲಿತಾಂಶ)
    peer->hmac_ಗಣ = TRUE;
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode cf_ssl_ಕಂಡುಹಿಡಿ_peer_by_hmac(ರಚನೆ Curl_ssl_scache *scache,
                                         ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *salt,
                                         ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *hmac,
                                         ರಚನೆ Curl_ssl_scache_peer **ppeer)
{
  size_t i;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  *ppeer = ಖಾಲಿ;
  ಆದರೆ(!GOOD_SCACHE(scache))
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  /* look for an entry that matches salt+hmac exactly or has a known
   * ssl_peer_key which salt+hmac's to the same. */
  ಗಾಗಿ(i = 0; scache && i < scache->peer_ಎಣಿಕೆ; i++) {
    ರಚನೆ Curl_ssl_scache_peer *peer = &scache->peers[i];
    ಆದರೆ(!cf_ssl_scache_ಹೊಂದಿಕೆ_auth(peer, ಖಾಲಿ))
      ಮುಂದುವರಿ;
    ಆದರೆ(scache->peers[i].hmac_ಗಣ &&
       !memcmp(peer->ಕೀಲಿ_salt, salt, ಗಾತ್ರ(peer->ಕೀಲಿ_salt)) &&
       !memcmp(peer->ಕೀಲಿ_hmac, hmac, ಗಾತ್ರ(peer->ಕೀಲಿ_hmac))) {
      /* found exact match, return */
      *ppeer = peer;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(peer->ssl_peer_ಕೀಲಿ) {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ my_hmac[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ];
      /* compute hmac for the passed salt */
      ಫಲಿತಾಂಶ = Curl_hmacit(&Curl_HMAC_SHA256,
                           salt, ಗಾತ್ರ(peer->ಕೀಲಿ_salt),
                           (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)peer->ssl_peer_ಕೀಲಿ,
                           ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(peer->ssl_peer_ಕೀಲಿ),
                           my_hmac);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      ಆದರೆ(!memcmp(my_hmac, hmac, ಗಾತ್ರ(my_hmac))) {
        /* cryptohash match, take over salt+hmac if no set and return */
        ಆದರೆ(!peer->hmac_ಗಣ) {
          ಸ್ಮರಣೆ_ನಕಲು(peer->ಕೀಲಿ_salt, salt, ಗಾತ್ರ(peer->ಕೀಲಿ_salt));
          ಸ್ಮರಣೆ_ನಕಲು(peer->ಕೀಲಿ_hmac, hmac, ಗಾತ್ರ(peer->ಕೀಲಿ_hmac));
          peer->hmac_ಗಣ = TRUE;
        }
        *ppeer = peer;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
  }
ಹೊರಹೋಗು:
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_ssl_session_import(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ssl_peer_ಕೀಲಿ,
                                 ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *shmac, size_t shmac_len,
                                 ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *sdata, size_t sdata_len)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  ರಚನೆ Curl_ssl_scache_peer *peer = ಖಾಲಿ;
  ರಚನೆ Curl_ssl_session *s = ಖಾಲಿ;
  bool have_ಬೀಗ = FALSE;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(!scache || !sdata || !sdata_len) {
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಆದರೆ(!ssl_peer_ಕೀಲಿ && (!shmac || !shmac_len)) {
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಫಲಿತಾಂಶ = Curl_ssl_session_unpack(ದತ್ತಾಂಶ, sdata, sdata_len, &s);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹೋಗು ಹೊರಹೋಗು;

  Curl_ssl_scache_ಬೀಗ(ದತ್ತಾಂಶ);
  have_ಬೀಗ = TRUE;

  ಆದರೆ(ssl_peer_ಕೀಲಿ) {
    ಫಲಿತಾಂಶ = cf_ssl_ಕೂಡಿಸು_peer(ದತ್ತಾಂಶ, scache, ssl_peer_ಕೀಲಿ, ಖಾಲಿ, &peer);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(shmac_len != (ಗಾತ್ರ(peer->ಕೀಲಿ_salt) + ಗಾತ್ರ(peer->ಕೀಲಿ_hmac))) {
    /* Either salt+hmac was garbled by caller or is from a curl version
     * that does things differently */
    ಫಲಿತಾಂಶ = CURLE_BAD_FUNCTION_ವಾದ;
    ಹೋಗು ಹೊರಹೋಗು;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *salt = shmac;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *hmac = shmac + ಗಾತ್ರ(peer->ಕೀಲಿ_salt);

    ಫಲಿತಾಂಶ = cf_ssl_ಕಂಡುಹಿಡಿ_peer_by_hmac(scache, salt, hmac, &peer);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು ಹೊರಹೋಗು;
    ಆದರೆ(!peer) {
      peer = cf_ssl_get_free_peer(scache);
      ಆದರೆ(peer) {
        ಫಲಿತಾಂಶ = cf_ssl_scache_peer_init(peer, ssl_peer_ಕೀಲಿ, ಖಾಲಿ, ಖಾಲಿ,
                                         salt, hmac);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
      }
    }
  }

  ಆದರೆ(peer) {
    cf_scache_peer_ಕೂಡಿಸು_session(peer, s, ಸಮಯ(ಖಾಲಿ));
    s = ಖಾಲಿ; /* peer is now owner */
    CURL_TRC_SSLS(ದತ್ತಾಂಶ, "successfully imported ticket for peer %s, now "
                  "with %zu tickets",
                  peer->ssl_peer_ಕೀಲಿ ? peer->ssl_peer_ಕೀಲಿ : "without key",
                  Curl_llist_ಎಣಿಕೆ(&peer->sessions));
  }

ಹೊರಹೋಗು:
  ಆದರೆ(scache && have_ಬೀಗ)
    Curl_ssl_scache_unlock(ದತ್ತಾಂಶ);
  Curl_ssl_session_destroy(s);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

CURLcode Curl_ssl_session_export(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 curl_ssls_export_cb *export_fn,
                                 ಶೂನ್ಯ *userptr)
{
  ರಚನೆ Curl_ssl_scache *scache = cf_ssl_scache_get(ದತ್ತಾಂಶ);
  ರಚನೆ Curl_ssl_scache_peer *peer;
  ರಚನೆ dynbuf sbuf, hbuf;
  ರಚನೆ Curl_llist_ಗಂಟು *n;
  size_t i;
  curl_off_t now = ಸಮಯ(ಖಾಲಿ);
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURLVERBOSE
  size_t npeers = 0, ntickets = 0;
#ಆದರೆ_ಅಂತ್ಯ

  ಆದರೆ(!export_fn)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  Curl_ssl_scache_ಬೀಗ(ದತ್ತಾಂಶ);

  curlx_dyn_init(&hbuf, (CURL_SHA256_DIGEST_ಉದ್ದಳತೆ * 2) + 1);
  curlx_dyn_init(&sbuf, CURL_SSL_TICKET_MAX);

  ಗಾಗಿ(i = 0; scache && i < scache->peer_ಎಣಿಕೆ; i++) {
    peer = &scache->peers[i];
    ಆದರೆ(!peer->ssl_peer_ಕೀಲಿ && !peer->hmac_ಗಣ)
      ಮುಂದುವರಿ; /* skip free entry */
    ಆದರೆ(!peer->exportable)
      ಮುಂದುವರಿ;

    curlx_dyn_reset(&hbuf);
    cf_scache_peer_remove_expired(peer, now);
    n = Curl_llist_ತಲೆ(&peer->sessions);
    ಆದರೆ(n)
      VERBOSE(++npeers);
    ಆಗ(n) {
      ರಚನೆ Curl_ssl_session *s = Curl_ಗಂಟು_elem(n);
      ಆದರೆ(!peer->hmac_ಗಣ) {
        ಫಲಿತಾಂಶ = cf_ssl_scache_peer_ಗಣ_hmac(peer);
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
      }
      ಆದರೆ(!curlx_dyn_len(&hbuf)) {
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&hbuf, peer->ಕೀಲಿ_salt, ಗಾತ್ರ(peer->ಕೀಲಿ_salt));
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&hbuf, peer->ಕೀಲಿ_hmac, ಗಾತ್ರ(peer->ಕೀಲಿ_hmac));
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹೋಗು ಹೊರಹೋಗು;
      }
      curlx_dyn_reset(&sbuf);
      ಫಲಿತಾಂಶ = Curl_ssl_session_pack(ದತ್ತಾಂಶ, s, &sbuf);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;

      ಫಲಿತಾಂಶ = export_fn(ದತ್ತಾಂಶ, userptr, peer->ssl_peer_ಕೀಲಿ,
                         curlx_dyn_uptr(&hbuf), curlx_dyn_len(&hbuf),
                         curlx_dyn_uptr(&sbuf), curlx_dyn_len(&sbuf),
                         s->valid_until, s->ietf_tls_id,
                         s->alpn, s->earlydata_max);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹೋಗು ಹೊರಹೋಗು;
      VERBOSE(++ntickets);
      n = Curl_ಗಂಟು_ಮುಂದಿನ(n);
    }
  }
  ಫಲಿತಾಂಶ = CURLE_OK;
  CURL_TRC_SSLS(ದತ್ತಾಂಶ, "exported %zu session tickets for %zu peers",
                ntickets, npeers);

ಹೊರಹೋಗು:
  Curl_ssl_scache_unlock(ದತ್ತಾಂಶ);
  curlx_dyn_free(&hbuf);
  curlx_dyn_free(&sbuf);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_SSLS_EXPORT */

#ಆದರೆ_ಅಂತ್ಯ /* USE_SSL */
