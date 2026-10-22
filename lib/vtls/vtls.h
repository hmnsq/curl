#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VTLS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VTLS_H
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

ਬਣਤਰ connectdata;
ਬਣਤਰ ssl_easy_config;
ਬਣਤਰ ssl_filter_config;
ਬਣਤਰ Curl_cfilter;
ਬਣਤਰ Curl_easy;
ਬਣਤਰ dynbuf;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_CA_ਰਸਤਾ      (1 << 0) /* supports CAPATH */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_CERTINFO     (1 << 1) /* supports CURLOPT_CERTINFO */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_PINNEDPUBKEY (1 << 2) /* supports CURLOPT_PINNEDPUBLICKEY */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_SSL_CTX      (1 << 3) /* supports CURLOPT_SSL_CTX */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_HTTPS_PROXY  (1 << 4) /* supports access via HTTPS proxies */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_TLS13_CIPHERSUITES (1 << 5) /* supports TLS 1.3 ciphersuites */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_CAINFO_BLOB  (1 << 6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_ECH          (1 << 7)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_CA_CACHE     (1 << 8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_CIPHER_LIST  (1 << 9) /* supports TLS 1.0-1.2 ciphersuites */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_SIGNATURE_ALGORITHMS (1 << 10) /* supports TLS sigalgs */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_ISSUERCERT   (1 << 11) /* supports CURLOPT_ISSUERCERT */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_SSL_EC_CURVES (1 << 12) /* supports CURLOPT_SSL_EC_CURVES */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_CRLFILE      (1 << 13) /* supports CURLOPT_CRLFILE */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSLSUPP_ISSUERCERT_BLOB (1 << 14) /* CURLOPT_ISSUERCERT_BLOB */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
/* CURLECH_ values for the ech option */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLECH_DISABLE    0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLECH_GREASE     1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLECH_ENABLE     2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURLECH_HARD       3

/* TRUE if ECH is enabled (and supported) */
bool Curl_ssl_ech_enabled(ਬਣਤਰ Curl_cfilter *cf);
/* TRUE if SSL ECH configuration requires DNS HTTPS-RR record */
bool Curl_ssl_need_httpsrr(ਬਣਤਰ Curl_cfilter *cf);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_ech_enabled(c)      FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_need_httpsrr(c)     FALSE
#ਜੇ_ਅੰਤ /* USE_ECH */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALPN_ACCEPTED "ALPN: server accepted "

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTLS_INFOF_NO_ALPN            \
  "ALPN: server did not agree on a protocol. Uses default."
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTLS_INFOF_ALPN_OFFER_1STR    \
  "ALPN: curl offers %s"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTLS_INFOF_ALPN_ACCEPTED      \
  ALPN_ACCEPTED "%.*s"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTLS_INFOF_NO_ALPN_DEFERRED   \
  "ALPN: deferred handshake for early data without specific protocol."
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTLS_INFOF_ALPN_DEFERRED      \
  "ALPN: deferred handshake for early data using '%.*s'."

/* IETF defined version numbers used in TLS protocol negotiation */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_IETF_PROTO_UNKNOWN       0x0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_IETF_PROTO_SSL3          0x0300
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_IETF_PROTO_TLS1          0x0301
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_IETF_PROTO_TLS1_1        0x0302
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_IETF_PROTO_TLS1_2        0x0303
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_IETF_PROTO_TLS1_3        0x0304
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_IETF_PROTO_DTLS1         0xFEFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_IETF_PROTO_DTLS1_2       0xFEFD

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_SSL_PEER_DNS,
  CURL_SSL_PEER_IPV4,
  CURL_SSL_PEER_IPV6
} ssl_peer_type;

ਬਣਤਰ ssl_peer {
  ਬਣਤਰ Curl_peer *origin; /* the authority we talk to */
  ਬਣਤਰ Curl_peer *peer;   /* the machine we are connected to */
  ਅੱਖਰ *sni;             /* SNI version of hostname or NULL if not usable */
  ਅੱਖਰ *scache_ਕੁੰਜੀ;      /* for lookups in session cache */
  ssl_peer_type type;    /* type of the peer information */
  uint8_t transport;     /* one of TRNSPRT_* defines */
};

CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ਅਚਲ ਅੱਖਰ *name,
                                   ਅਚਲ curl_ssl_backend ***avail);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_PINNED_PUBKEY_ਮਾਪ (1024 * 1024) /* 1 MiB */

curl_sslbackend Curl_ssl_backend(ਖਾਲੀ);

/**
 * Init SSL peer information for filter. Can be called repeatedly.
 */
CURLcode Curl_ssl_peer_init(ਬਣਤਰ ssl_peer *ssl_peer,
                            ਬਣਤਰ Curl_peer *origin,
                            ਬਣਤਰ Curl_peer *peer,
                            ਬਣਤਰ ssl_filter_config *sslc,
                            ਅਚਲ ਅੱਖਰ *tls_id,
                            uint8_t transport);
/**
 * Free all allocated data and reset peer information.
 */
ਖਾਲੀ Curl_ssl_peer_cleanup(ਬਣਤਰ ssl_peer *peer);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SSL
ਪੂਰਨ Curl_ssl_init(ਖਾਲੀ);
ਖਾਲੀ Curl_ssl_cleanup(ਖਾਲੀ);
/* tell the SSL stuff to close down all open information regarding
   connections (and thus session ID caching etc) */
ਖਾਲੀ Curl_ssl_close_all(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
CURLcode Curl_ssl_ਸੈੱਟ_engine(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *engine);
/* Sets engine as default for all SSL operations */
CURLcode Curl_ssl_ਸੈੱਟ_engine_default(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
ਬਣਤਰ curl_slist *Curl_ssl_engines_list(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

ਖਾਲੀ Curl_ssl_ਸੰਸਕਰਣ(ਅੱਖਰ *ਬਫਰ, size_t ਮਾਪ);

/* Certificate information list handling. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_X509_STR_MAX  100000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ALLOWED_CERT_AMOUNT 100

ਖਾਲੀ Curl_ssl_free_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
CURLcode Curl_ssl_init_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ ਅੰਕ);
CURLcode Curl_ssl_ਧੱਕੋ_certinfo_len(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ certnum,
                                    ਅਚਲ ਅੱਖਰ *label, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ,
                                    size_t valuelen);
CURLcode Curl_ssl_ਧੱਕੋ_certinfo(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ certnum,
                                ਅਚਲ ਅੱਖਰ *label, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);

/* Functions to be used by SSL library adaptation functions */

/* get N random bytes into the buffer */
CURLcode Curl_ssl_random(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਫਰ, size_t ਲੰਮਾਈ);
/* Check pinned public key. */
CURLcode Curl_pin_peer_pubkey(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਅਚਲ ਅੱਖਰ *pinnedpubkey,
                              ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pubkey, size_t pubkeylen);

bool Curl_ssl_cert_ਸਥਿਤੀ_request(ਖਾਲੀ);

/* The maximum size of the SSL channel binding is 85 bytes, as defined in
 * RFC 5929, Section 4.1. The 'tls-server-end-point:' prefix is 21 bytes,
 * and SHA-512 is the longest supported hash algorithm, with a digest length of
 * 64 bytes.
 * The maximum size of the channel binding is therefore 21 + 64 = 85 bytes.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_CB_MAX_ਮਾਪ 85

/* Return the tls-server-end-point channel binding, including the
 * 'tls-server-end-point:' prefix.
 * If successful, the data is written to the dynbuf, and CURLE_OK is returned.
 * The dynbuf MUST HAVE a minimum toobig size of SSL_CB_MAX_SIZE.
 * If the dynbuf is too small, CURLE_OUT_OF_MEMORY is returned.
 * If channel binding is not supported, binding stays empty and CURLE_OK is
 * returned.
 */
CURLcode Curl_ssl_get_channel_binding(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, int8_t sockindex,
                                      ਬਣਤਰ dynbuf *binding);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSL_SHUTDOWN_TIMEOUT 10000 /* ms */

CURLcode Curl_ssl_cfilter_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_peer *origin,
                              ਬਣਤਰ connectdata *conn,
                              int8_t sockindex);

CURLcode Curl_cf_ssl_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                  ਬਣਤਰ Curl_peer *origin,
                                  ਬਣਤਰ Curl_peer *peer);

CURLcode Curl_ssl_cfilter_remove(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 int8_t sockindex, bool send_shutdown);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
CURLcode Curl_cf_ssl_proxy_ਪਾਓ_after(ਬਣਤਰ Curl_cfilter *cf_at,
                                        ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                        ਬਣਤਰ Curl_peer *peer);
#ਜੇ_ਅੰਤ /* !CURL_DISABLE_PROXY */

/**
 * True iff the underlying SSL implementation supports the option.
 * Option is one of the defined SSLSUPP_* values.
 * `data` maybe NULL for the features of the default implementation.
 */
bool Curl_ssl_supports(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ssl_option);

/* Get the ssl_easy_config from `data` relevant for cfilter `cf`. */
ਬਣਤਰ ssl_easy_config *
Curl_ssl_cf_get_easy_config(ਬਣਤਰ Curl_cfilter *cf,
                            ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

/* Get the filter config relevant for cfilter `cf`. */
ਬਣਤਰ ssl_filter_config *
Curl_ssl_cf_get_filter_config(ਬਣਤਰ Curl_cfilter *cf);

ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_ssl;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_PROXY
ਬਾਹਰੀ ਬਣਤਰ Curl_cftype Curl_cft_ssl_proxy;
#ਜੇ_ਅੰਤ

#ਨਹੀਂਤਾਂ /* if not USE_SSL */

/* When SSL support is not present, define away these function calls */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_init() 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_cleanup() Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_close_all(x) Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_ਸੈੱਟ_engine(x, y) CURLE_NOT_BUILT_ਵਿੱਚ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_ਸੈੱਟ_engine_default(x) CURLE_NOT_BUILT_ਵਿੱਚ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_engines_list(x) ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_free_certinfo(x) Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_random(x, y, z) ((ਖਾਲੀ)(x), CURLE_NOT_BUILT_ਵਿੱਚ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_cert_ਸਥਿਤੀ_request() FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_supports(a, b) FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_cfilter_ਜੋੜੋ(a, b, c, d) CURLE_NOT_BUILT_ਵਿੱਚ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_cfilter_remove(a, b, c) CURLE_OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_cf_get_easy_config(a, b) ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_ssl_cf_get_filter_config(a) ਨਿਰਾ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_VTLS_H */
