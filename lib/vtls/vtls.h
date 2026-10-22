#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VTLS_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VTLS_H
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

ರಚನೆ connectdata;
ರಚನೆ ssl_easy_config;
ರಚನೆ ssl_filter_config;
ರಚನೆ Curl_cfilter;
ರಚನೆ Curl_easy;
ರಚನೆ dynbuf;

#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_CA_ಮಾರ್ಗ      (1 << 0) /* supports CAPATH */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_CERTINFO     (1 << 1) /* supports CURLOPT_CERTINFO */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_PINNEDPUBKEY (1 << 2) /* supports CURLOPT_PINNEDPUBLICKEY */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_SSL_CTX      (1 << 3) /* supports CURLOPT_SSL_CTX */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_HTTPS_PROXY  (1 << 4) /* supports access via HTTPS proxies */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_TLS13_CIPHERSUITES (1 << 5) /* supports TLS 1.3 ciphersuites */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_CAINFO_BLOB  (1 << 6)
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_ECH          (1 << 7)
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_CA_CACHE     (1 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_CIPHER_LIST  (1 << 9) /* supports TLS 1.0-1.2 ciphersuites */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_SIGNATURE_ALGORITHMS (1 << 10) /* supports TLS sigalgs */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_ISSUERCERT   (1 << 11) /* supports CURLOPT_ISSUERCERT */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_SSL_EC_CURVES (1 << 12) /* supports CURLOPT_SSL_EC_CURVES */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_CRLFILE      (1 << 13) /* supports CURLOPT_CRLFILE */
#ವ್ಯಾಖ್ಯಾನಿಸು SSLSUPP_ISSUERCERT_BLOB (1 << 14) /* CURLOPT_ISSUERCERT_BLOB */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
/* CURLECH_ values for the ech option */
#ವ್ಯಾಖ್ಯಾನಿಸು CURLECH_DISABLE    0
#ವ್ಯಾಖ್ಯಾನಿಸು CURLECH_GREASE     1
#ವ್ಯಾಖ್ಯಾನಿಸು CURLECH_ENABLE     2
#ವ್ಯಾಖ್ಯಾನಿಸು CURLECH_HARD       3

/* TRUE if ECH is enabled (and supported) */
bool Curl_ssl_ech_enabled(ರಚನೆ Curl_cfilter *cf);
/* TRUE if SSL ECH configuration requires DNS HTTPS-RR record */
bool Curl_ssl_need_httpsrr(ರಚನೆ Curl_cfilter *cf);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_ech_enabled(c)      FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_need_httpsrr(c)     FALSE
#ಆದರೆ_ಅಂತ್ಯ /* USE_ECH */

#ವ್ಯಾಖ್ಯಾನಿಸು ALPN_ACCEPTED "ALPN: server accepted "

#ವ್ಯಾಖ್ಯಾನಿಸು VTLS_INFOF_NO_ALPN            \
  "ALPN: server did not agree on a protocol. Uses default."
#ವ್ಯಾಖ್ಯಾನಿಸು VTLS_INFOF_ALPN_OFFER_1STR    \
  "ALPN: curl offers %s"
#ವ್ಯಾಖ್ಯಾನಿಸು VTLS_INFOF_ALPN_ACCEPTED      \
  ALPN_ACCEPTED "%.*s"

#ವ್ಯಾಖ್ಯಾನಿಸು VTLS_INFOF_NO_ALPN_DEFERRED   \
  "ALPN: deferred handshake for early data without specific protocol."
#ವ್ಯಾಖ್ಯಾನಿಸು VTLS_INFOF_ALPN_DEFERRED      \
  "ALPN: deferred handshake for early data using '%.*s'."

/* IETF defined version numbers used in TLS protocol negotiation */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_IETF_PROTO_UNKNOWN       0x0
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_IETF_PROTO_SSL3          0x0300
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_IETF_PROTO_TLS1          0x0301
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_IETF_PROTO_TLS1_1        0x0302
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_IETF_PROTO_TLS1_2        0x0303
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_IETF_PROTO_TLS1_3        0x0304
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_IETF_PROTO_DTLS1         0xFEFF
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_IETF_PROTO_DTLS1_2       0xFEFD

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURL_SSL_PEER_DNS,
  CURL_SSL_PEER_IPV4,
  CURL_SSL_PEER_IPV6
} ssl_peer_type;

ರಚನೆ ssl_peer {
  ರಚನೆ Curl_peer *origin; /* the authority we talk to */
  ರಚನೆ Curl_peer *peer;   /* the machine we are connected to */
  ಅಕ್ಷರ *sni;             /* SNI version of hostname or NULL if not usable */
  ಅಕ್ಷರ *scache_ಕೀಲಿ;      /* for lookups in session cache */
  ssl_peer_type type;    /* type of the peer information */
  uint8_t transport;     /* one of TRNSPRT_* defines */
};

CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                                   ಸ್ಥಿರಾಂಕ curl_ssl_backend ***avail);

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_PINNED_PUBKEY_ಗಾತ್ರ (1024 * 1024) /* 1 MiB */

curl_sslbackend Curl_ssl_backend(ಶೂನ್ಯ);

/**
 * Init SSL peer information for filter. Can be called repeatedly.
 */
CURLcode Curl_ssl_peer_init(ರಚನೆ ssl_peer *ssl_peer,
                            ರಚನೆ Curl_peer *origin,
                            ರಚನೆ Curl_peer *peer,
                            ರಚನೆ ssl_filter_config *sslc,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tls_id,
                            uint8_t transport);
/**
 * Free all allocated data and reset peer information.
 */
ಶೂನ್ಯ Curl_ssl_peer_cleanup(ರಚನೆ ssl_peer *peer);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SSL
ಪೂರ್ಣಾಂಕ Curl_ssl_init(ಶೂನ್ಯ);
ಶೂನ್ಯ Curl_ssl_cleanup(ಶೂನ್ಯ);
/* tell the SSL stuff to close down all open information regarding
   connections (and thus session ID caching etc) */
ಶೂನ್ಯ Curl_ssl_close_all(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLcode Curl_ssl_ಗಣ_engine(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *engine);
/* Sets engine as default for all SSL operations */
CURLcode Curl_ssl_ಗಣ_engine_default(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
ರಚನೆ curl_slist *Curl_ssl_engines_list(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

ಶೂನ್ಯ Curl_ssl_ಆವೃತ್ತಿ(ಅಕ್ಷರ *ಬಫರ್, size_t ಗಾತ್ರ_2);

/* Certificate information list handling. */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_X509_STR_MAX  100000
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ALLOWED_CERT_AMOUNT 100

ಶೂನ್ಯ Curl_ssl_free_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLcode Curl_ssl_init_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ ಅಂಕೆ);
CURLcode Curl_ssl_ತಳ್ಳು_certinfo_len(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ certnum,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ,
                                    size_t valuelen);
CURLcode Curl_ssl_ತಳ್ಳು_certinfo(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ certnum,
                                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);

/* Functions to be used by SSL library adaptation functions */

/* get N random bytes into the buffer */
CURLcode Curl_ssl_random(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಬಫರ್, size_t ಉದ್ದಳತೆ);
/* Check pinned public key. */
CURLcode Curl_pin_peer_pubkey(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pinnedpubkey,
                              ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pubkey, size_t pubkeylen);

bool Curl_ssl_cert_ಸ್ಥಿತಿ_request(ಶೂನ್ಯ);

/* The maximum size of the SSL channel binding is 85 bytes, as defined in
 * RFC 5929, Section 4.1. The 'tls-server-end-point:' prefix is 21 bytes,
 * and SHA-512 is the longest supported hash algorithm, with a digest length of
 * 64 bytes.
 * The maximum size of the channel binding is therefore 21 + 64 = 85 bytes.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು SSL_CB_MAX_ಗಾತ್ರ 85

/* Return the tls-server-end-point channel binding, including the
 * 'tls-server-end-point:' prefix.
 * If successful, the data is written to the dynbuf, and CURLE_OK is returned.
 * The dynbuf MUST HAVE a minimum toobig size of SSL_CB_MAX_SIZE.
 * If the dynbuf is too small, CURLE_OUT_OF_MEMORY is returned.
 * If channel binding is not supported, binding stays empty and CURLE_OK is
 * returned.
 */
CURLcode Curl_ssl_get_channel_binding(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, int8_t sockindex,
                                      ರಚನೆ dynbuf *binding);

#ವ್ಯಾಖ್ಯಾನಿಸು SSL_SHUTDOWN_TIMEOUT 10000 /* ms */

CURLcode Curl_ssl_cfilter_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_peer *origin,
                              ರಚನೆ connectdata *conn,
                              int8_t sockindex);

CURLcode Curl_cf_ssl_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                  ರಚನೆ Curl_peer *origin,
                                  ರಚನೆ Curl_peer *peer);

CURLcode Curl_ssl_cfilter_remove(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 int8_t sockindex, bool send_shutdown);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
CURLcode Curl_cf_ssl_proxy_ಸೇರಿಸು_after(ರಚನೆ Curl_cfilter *cf_at,
                                        ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                        ರಚನೆ Curl_peer *peer);
#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_PROXY */

/**
 * True iff the underlying SSL implementation supports the option.
 * Option is one of the defined SSLSUPP_* values.
 * `data` maybe NULL for the features of the default implementation.
 */
bool Curl_ssl_supports(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ssl_option);

/* Get the ssl_easy_config from `data` relevant for cfilter `cf`. */
ರಚನೆ ssl_easy_config *
Curl_ssl_cf_get_easy_config(ರಚನೆ Curl_cfilter *cf,
                            ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

/* Get the filter config relevant for cfilter `cf`. */
ರಚನೆ ssl_filter_config *
Curl_ssl_cf_get_filter_config(ರಚನೆ Curl_cfilter *cf);

ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_ssl;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_PROXY
ಬಾಹ್ಯ ರಚನೆ Curl_cftype Curl_cft_ssl_proxy;
#ಆದರೆ_ಅಂತ್ಯ

#ಇಲ್ಲದಿದ್ದರೆ /* if not USE_SSL */

/* When SSL support is not present, define away these function calls */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_init() 1
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_cleanup() Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_close_all(x) Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_ಗಣ_engine(x, y) CURLE_NOT_BUILT_ಒಳ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_ಗಣ_engine_default(x) CURLE_NOT_BUILT_ಒಳ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_engines_list(x) ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_free_certinfo(x) Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_random(x, y, z) ((ಶೂನ್ಯ)(x), CURLE_NOT_BUILT_ಒಳ)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_cert_ಸ್ಥಿತಿ_request() FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_supports(a, b) FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_cfilter_ಕೂಡಿಸು(a, b, c, d) CURLE_NOT_BUILT_ಒಳ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_cfilter_remove(a, b, c) CURLE_OK
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_cf_get_easy_config(a, b) ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_ssl_cf_get_filter_config(a) ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VTLS_H */
