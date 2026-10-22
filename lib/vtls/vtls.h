#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VTLS_H
#నిర్వచించు శీర్షిక_CURL_VTLS_H
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

నిర్మాణం connectdata;
నిర్మాణం ssl_easy_config;
నిర్మాణం ssl_filter_config;
నిర్మాణం Curl_cfilter;
నిర్మాణం Curl_easy;
నిర్మాణం dynbuf;

#నిర్వచించు SSLSUPP_CA_మార్గం      (1 << 0) /* supports CAPATH */
#నిర్వచించు SSLSUPP_CERTINFO     (1 << 1) /* supports CURLOPT_CERTINFO */
#నిర్వచించు SSLSUPP_PINNEDPUBKEY (1 << 2) /* supports CURLOPT_PINNEDPUBLICKEY */
#నిర్వచించు SSLSUPP_SSL_CTX      (1 << 3) /* supports CURLOPT_SSL_CTX */
#నిర్వచించు SSLSUPP_HTTPS_PROXY  (1 << 4) /* supports access via HTTPS proxies */
#నిర్వచించు SSLSUPP_TLS13_CIPHERSUITES (1 << 5) /* supports TLS 1.3 ciphersuites */
#నిర్వచించు SSLSUPP_CAINFO_BLOB  (1 << 6)
#నిర్వచించు SSLSUPP_ECH          (1 << 7)
#నిర్వచించు SSLSUPP_CA_CACHE     (1 << 8)
#నిర్వచించు SSLSUPP_CIPHER_LIST  (1 << 9) /* supports TLS 1.0-1.2 ciphersuites */
#నిర్వచించు SSLSUPP_SIGNATURE_ALGORITHMS (1 << 10) /* supports TLS sigalgs */
#నిర్వచించు SSLSUPP_ISSUERCERT   (1 << 11) /* supports CURLOPT_ISSUERCERT */
#నిర్వచించు SSLSUPP_SSL_EC_CURVES (1 << 12) /* supports CURLOPT_SSL_EC_CURVES */
#నిర్వచించు SSLSUPP_CRLFILE      (1 << 13) /* supports CURLOPT_CRLFILE */
#నిర్వచించు SSLSUPP_ISSUERCERT_BLOB (1 << 14) /* CURLOPT_ISSUERCERT_BLOB */

#నిర్వచితమైతే USE_ECH
/* CURLECH_ values for the ech option */
#నిర్వచించు CURLECH_DISABLE    0
#నిర్వచించు CURLECH_GREASE     1
#నిర్వచించు CURLECH_ENABLE     2
#నిర్వచించు CURLECH_HARD       3

/* TRUE if ECH is enabled (and supported) */
bool Curl_ssl_ech_enabled(నిర్మాణం Curl_cfilter *cf);
/* TRUE if SSL ECH configuration requires DNS HTTPS-RR record */
bool Curl_ssl_need_httpsrr(నిర్మాణం Curl_cfilter *cf);
#లేకపోతే
#నిర్వచించు Curl_ssl_ech_enabled(c)      FALSE
#నిర్వచించు Curl_ssl_need_httpsrr(c)     FALSE
#అయితే_ముగింపు /* USE_ECH */

#నిర్వచించు ALPN_ACCEPTED "ALPN: server accepted "

#నిర్వచించు VTLS_INFOF_NO_ALPN            \
  "ALPN: server did not agree on a protocol. Uses default."
#నిర్వచించు VTLS_INFOF_ALPN_OFFER_1STR    \
  "ALPN: curl offers %s"
#నిర్వచించు VTLS_INFOF_ALPN_ACCEPTED      \
  ALPN_ACCEPTED "%.*s"

#నిర్వచించు VTLS_INFOF_NO_ALPN_DEFERRED   \
  "ALPN: deferred handshake for early data without specific protocol."
#నిర్వచించు VTLS_INFOF_ALPN_DEFERRED      \
  "ALPN: deferred handshake for early data using '%.*s'."

/* IETF defined version numbers used in TLS protocol negotiation */
#నిర్వచించు CURL_IETF_PROTO_UNKNOWN       0x0
#నిర్వచించు CURL_IETF_PROTO_SSL3          0x0300
#నిర్వచించు CURL_IETF_PROTO_TLS1          0x0301
#నిర్వచించు CURL_IETF_PROTO_TLS1_1        0x0302
#నిర్వచించు CURL_IETF_PROTO_TLS1_2        0x0303
#నిర్వచించు CURL_IETF_PROTO_TLS1_3        0x0304
#నిర్వచించు CURL_IETF_PROTO_DTLS1         0xFEFF
#నిర్వచించు CURL_IETF_PROTO_DTLS1_2       0xFEFD

రకనిర్వచనం గణన {
  CURL_SSL_PEER_DNS,
  CURL_SSL_PEER_IPV4,
  CURL_SSL_PEER_IPV6
} ssl_peer_type;

నిర్మాణం ssl_peer {
  నిర్మాణం Curl_peer *origin; /* the authority we talk to */
  నిర్మాణం Curl_peer *peer;   /* the machine we are connected to */
  అక్షరం *sni;             /* SNI version of hostname or NULL if not usable */
  అక్షరం *scache_కీలకం;      /* for lookups in session cache */
  ssl_peer_type type;    /* type of the peer information */
  uint8_t transport;     /* one of TRNSPRT_* defines */
};

CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, స్థిరరాశి అక్షరం *name,
                                   స్థిరరాశి curl_ssl_backend ***avail);

#నిర్వచించు MAX_PINNED_PUBKEY_కొలత (1024 * 1024) /* 1 MiB */

curl_sslbackend Curl_ssl_backend(శూన్య);

/**
 * Init SSL peer information for filter. Can be called repeatedly.
 */
CURLcode Curl_ssl_peer_init(నిర్మాణం ssl_peer *ssl_peer,
                            నిర్మాణం Curl_peer *origin,
                            నిర్మాణం Curl_peer *peer,
                            నిర్మాణం ssl_filter_config *sslc,
                            స్థిరరాశి అక్షరం *tls_id,
                            uint8_t transport);
/**
 * Free all allocated data and reset peer information.
 */
శూన్య Curl_ssl_peer_cleanup(నిర్మాణం ssl_peer *peer);

#నిర్వచితమైతే USE_SSL
పూర్ణాంకం Curl_ssl_init(శూన్య);
శూన్య Curl_ssl_cleanup(శూన్య);
/* tell the SSL stuff to close down all open information regarding
   connections (and thus session ID caching etc) */
శూన్య Curl_ssl_close_all(నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_ssl_సమితి_engine(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *engine);
/* Sets engine as default for all SSL operations */
CURLcode Curl_ssl_సమితి_engine_default(నిర్మాణం Curl_easy *దత్తాంశం);
నిర్మాణం curl_slist *Curl_ssl_engines_list(నిర్మాణం Curl_easy *దత్తాంశం);

శూన్య Curl_ssl_సంస్కరణ(అక్షరం *బఫరు, size_t కొలత);

/* Certificate information list handling. */
#నిర్వచించు CURL_X509_STR_MAX  100000
#నిర్వచించు MAX_ALLOWED_CERT_AMOUNT 100

శూన్య Curl_ssl_free_certinfo(నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_ssl_init_certinfo(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం అంకె);
CURLcode Curl_ssl_నెట్టు_certinfo_len(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం certnum,
                                    స్థిరరాశి అక్షరం *label, స్థిరరాశి అక్షరం *విలువ,
                                    size_t valuelen);
CURLcode Curl_ssl_నెట్టు_certinfo(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం certnum,
                                స్థిరరాశి అక్షరం *label, స్థిరరాశి అక్షరం *విలువ);

/* Functions to be used by SSL library adaptation functions */

/* get N random bytes into the buffer */
CURLcode Curl_ssl_random(నిర్మాణం Curl_easy *దత్తాంశం,
                         గుర్తులేని అక్షరం *బఫరు, size_t నిడివి);
/* Check pinned public key. */
CURLcode Curl_pin_peer_pubkey(నిర్మాణం Curl_easy *దత్తాంశం,
                              స్థిరరాశి అక్షరం *pinnedpubkey,
                              స్థిరరాశి గుర్తులేని అక్షరం *pubkey, size_t pubkeylen);

bool Curl_ssl_cert_స్థితి_request(శూన్య);

/* The maximum size of the SSL channel binding is 85 bytes, as defined in
 * RFC 5929, Section 4.1. The 'tls-server-end-point:' prefix is 21 bytes,
 * and SHA-512 is the longest supported hash algorithm, with a digest length of
 * 64 bytes.
 * The maximum size of the channel binding is therefore 21 + 64 = 85 bytes.
 */
#నిర్వచించు SSL_CB_MAX_కొలత 85

/* Return the tls-server-end-point channel binding, including the
 * 'tls-server-end-point:' prefix.
 * If successful, the data is written to the dynbuf, and CURLE_OK is returned.
 * The dynbuf MUST HAVE a minimum toobig size of SSL_CB_MAX_SIZE.
 * If the dynbuf is too small, CURLE_OUT_OF_MEMORY is returned.
 * If channel binding is not supported, binding stays empty and CURLE_OK is
 * returned.
 */
CURLcode Curl_ssl_get_channel_binding(నిర్మాణం Curl_easy *దత్తాంశం, int8_t sockindex,
                                      నిర్మాణం dynbuf *binding);

#నిర్వచించు SSL_SHUTDOWN_TIMEOUT 10000 /* ms */

CURLcode Curl_ssl_cfilter_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                              నిర్మాణం Curl_peer *origin,
                              నిర్మాణం connectdata *conn,
                              int8_t sockindex);

CURLcode Curl_cf_ssl_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                  నిర్మాణం Curl_easy *దత్తాంశం,
                                  నిర్మాణం Curl_peer *origin,
                                  నిర్మాణం Curl_peer *peer);

CURLcode Curl_ssl_cfilter_remove(నిర్మాణం Curl_easy *దత్తాంశం,
                                 int8_t sockindex, bool send_shutdown);

#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
CURLcode Curl_cf_ssl_proxy_చొప్పించు_after(నిర్మాణం Curl_cfilter *cf_at,
                                        నిర్మాణం Curl_easy *దత్తాంశం,
                                        నిర్మాణం Curl_peer *peer);
#అయితే_ముగింపు /* !CURL_DISABLE_PROXY */

/**
 * True iff the underlying SSL implementation supports the option.
 * Option is one of the defined SSLSUPP_* values.
 * `data` maybe NULL for the features of the default implementation.
 */
bool Curl_ssl_supports(నిర్మాణం Curl_easy *దత్తాంశం, గుర్తులేని పూర్ణాంకం ssl_option);

/* Get the ssl_easy_config from `data` relevant for cfilter `cf`. */
నిర్మాణం ssl_easy_config *
Curl_ssl_cf_get_easy_config(నిర్మాణం Curl_cfilter *cf,
                            నిర్మాణం Curl_easy *దత్తాంశం);

/* Get the filter config relevant for cfilter `cf`. */
నిర్మాణం ssl_filter_config *
Curl_ssl_cf_get_filter_config(నిర్మాణం Curl_cfilter *cf);

బాహ్య నిర్మాణం Curl_cftype Curl_cft_ssl;
#నిర్వచితం_కాకపోతే CURL_DISABLE_PROXY
బాహ్య నిర్మాణం Curl_cftype Curl_cft_ssl_proxy;
#అయితే_ముగింపు

#లేకపోతే /* if not USE_SSL */

/* When SSL support is not present, define away these function calls */
#నిర్వచించు Curl_ssl_init() 1
#నిర్వచించు Curl_ssl_cleanup() Curl_nop_stmt
#నిర్వచించు Curl_ssl_close_all(x) Curl_nop_stmt
#నిర్వచించు Curl_ssl_సమితి_engine(x, y) CURLE_NOT_BUILT_లోపల
#నిర్వచించు Curl_ssl_సమితి_engine_default(x) CURLE_NOT_BUILT_లోపల
#నిర్వచించు Curl_ssl_engines_list(x) ఖాళీ
#నిర్వచించు Curl_ssl_free_certinfo(x) Curl_nop_stmt
#నిర్వచించు Curl_ssl_random(x, y, z) ((శూన్య)(x), CURLE_NOT_BUILT_లోపల)
#నిర్వచించు Curl_ssl_cert_స్థితి_request() FALSE
#నిర్వచించు Curl_ssl_supports(a, b) FALSE
#నిర్వచించు Curl_ssl_cfilter_కూడు(a, b, c, d) CURLE_NOT_BUILT_లోపల
#నిర్వచించు Curl_ssl_cfilter_remove(a, b, c) CURLE_OK
#నిర్వచించు Curl_ssl_cf_get_easy_config(a, b) ఖాళీ
#నిర్వచించు Curl_ssl_cf_get_filter_config(a) ఖాళీ
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_VTLS_H */
