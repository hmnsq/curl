#यदि_अपरिभाषितम् शीर्षकम्_CURL_VTLS_H
#परिभाषय शीर्षकम्_CURL_VTLS_H
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
#अन्तर्भाव "curl_setup.h"

संरचना connectdata;
संरचना ssl_easy_config;
संरचना ssl_filter_config;
संरचना Curl_cfilter;
संरचना Curl_easy;
संरचना dynbuf;

#परिभाषय SSLSUPP_CA_मार्गः      (1 << 0) /* supports CAPATH */
#परिभाषय SSLSUPP_CERTINFO     (1 << 1) /* supports CURLOPT_CERTINFO */
#परिभाषय SSLSUPP_PINNEDPUBKEY (1 << 2) /* supports CURLOPT_PINNEDPUBLICKEY */
#परिभाषय SSLSUPP_SSL_CTX      (1 << 3) /* supports CURLOPT_SSL_CTX */
#परिभाषय SSLSUPP_HTTPS_PROXY  (1 << 4) /* supports access via HTTPS proxies */
#परिभाषय SSLSUPP_TLS13_CIPHERSUITES (1 << 5) /* supports TLS 1.3 ciphersuites */
#परिभाषय SSLSUPP_CAINFO_BLOB  (1 << 6)
#परिभाषय SSLSUPP_ECH          (1 << 7)
#परिभाषय SSLSUPP_CA_CACHE     (1 << 8)
#परिभाषय SSLSUPP_CIPHER_LIST  (1 << 9) /* supports TLS 1.0-1.2 ciphersuites */
#परिभाषय SSLSUPP_SIGNATURE_ALGORITHMS (1 << 10) /* supports TLS sigalgs */
#परिभाषय SSLSUPP_ISSUERCERT   (1 << 11) /* supports CURLOPT_ISSUERCERT */
#परिभाषय SSLSUPP_SSL_EC_CURVES (1 << 12) /* supports CURLOPT_SSL_EC_CURVES */
#परिभाषय SSLSUPP_CRLFILE      (1 << 13) /* supports CURLOPT_CRLFILE */
#परिभाषय SSLSUPP_ISSUERCERT_BLOB (1 << 14) /* CURLOPT_ISSUERCERT_BLOB */

#यदि_परिभाषितम् USE_ECH
/* CURLECH_ values for the ech option */
#परिभाषय CURLECH_DISABLE    0
#परिभाषय CURLECH_GREASE     1
#परिभाषय CURLECH_ENABLE     2
#परिभाषय CURLECH_HARD       3

/* TRUE if ECH is enabled (and supported) */
bool Curl_ssl_ech_enabled(संरचना Curl_cfilter *cf);
/* TRUE if SSL ECH configuration requires DNS HTTPS-RR record */
bool Curl_ssl_need_httpsrr(संरचना Curl_cfilter *cf);
#अन्यथा
#परिभाषय Curl_ssl_ech_enabled(c)      FALSE
#परिभाषय Curl_ssl_need_httpsrr(c)     FALSE
#यदि_अन्तः /* USE_ECH */

#परिभाषय ALPN_ACCEPTED "ALPN: server accepted "

#परिभाषय VTLS_INFOF_NO_ALPN            \
  "ALPN: server did not agree on a protocol. Uses default."
#परिभाषय VTLS_INFOF_ALPN_OFFER_1STR    \
  "ALPN: curl offers %s"
#परिभाषय VTLS_INFOF_ALPN_ACCEPTED      \
  ALPN_ACCEPTED "%.*s"

#परिभाषय VTLS_INFOF_NO_ALPN_DEFERRED   \
  "ALPN: deferred handshake for early data without specific protocol."
#परिभाषय VTLS_INFOF_ALPN_DEFERRED      \
  "ALPN: deferred handshake for early data using '%.*s'."

/* IETF defined version numbers used in TLS protocol negotiation */
#परिभाषय CURL_IETF_PROTO_UNKNOWN       0x0
#परिभाषय CURL_IETF_PROTO_SSL3          0x0300
#परिभाषय CURL_IETF_PROTO_TLS1          0x0301
#परिभाषय CURL_IETF_PROTO_TLS1_1        0x0302
#परिभाषय CURL_IETF_PROTO_TLS1_2        0x0303
#परिभाषय CURL_IETF_PROTO_TLS1_3        0x0304
#परिभाषय CURL_IETF_PROTO_DTLS1         0xFEFF
#परिभाषय CURL_IETF_PROTO_DTLS1_2       0xFEFD

प्रकारनिर्देश गणना {
  CURL_SSL_PEER_DNS,
  CURL_SSL_PEER_IPV4,
  CURL_SSL_PEER_IPV6
} ssl_peer_type;

संरचना ssl_peer {
  संरचना Curl_peer *origin; /* the authority we talk to */
  संरचना Curl_peer *peer;   /* the machine we are connected to */
  वर्ण *sni;             /* SNI version of hostname or NULL if not usable */
  वर्ण *scache_कुञ्चिका;      /* for lookups in session cache */
  ssl_peer_type type;    /* type of the peer information */
  uint8_t transport;     /* one of TRNSPRT_* defines */
};

CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ध्रुव वर्ण *name,
                                   ध्रुव curl_ssl_backend ***avail);

#परिभाषय MAX_PINNED_PUBKEY_आकार (1024 * 1024) /* 1 MiB */

curl_sslbackend Curl_ssl_backend(शून्य);

/**
 * Init SSL peer information for filter. Can be called repeatedly.
 */
CURLcode Curl_ssl_peer_init(संरचना ssl_peer *ssl_peer,
                            संरचना Curl_peer *origin,
                            संरचना Curl_peer *peer,
                            संरचना ssl_filter_config *sslc,
                            ध्रुव वर्ण *tls_id,
                            uint8_t transport);
/**
 * Free all allocated data and reset peer information.
 */
शून्य Curl_ssl_peer_cleanup(संरचना ssl_peer *peer);

#यदि_परिभाषितम् USE_SSL
पूर्णाङ्क Curl_ssl_init(शून्य);
शून्य Curl_ssl_cleanup(शून्य);
/* tell the SSL stuff to close down all open information regarding
   connections (and thus session ID caching etc) */
शून्य Curl_ssl_close_all(संरचना Curl_easy *दत्त);
CURLcode Curl_ssl_समुच्चयः_engine(संरचना Curl_easy *दत्त, ध्रुव वर्ण *engine);
/* Sets engine as default for all SSL operations */
CURLcode Curl_ssl_समुच्चयः_engine_default(संरचना Curl_easy *दत्त);
संरचना curl_slist *Curl_ssl_engines_list(संरचना Curl_easy *दत्त);

शून्य Curl_ssl_संस्करणम्(वर्ण *बफर, size_t आकार);

/* Certificate information list handling. */
#परिभाषय CURL_X509_STR_MAX  100000
#परिभाषय MAX_ALLOWED_CERT_AMOUNT 100

शून्य Curl_ssl_free_certinfo(संरचना Curl_easy *दत्त);
CURLcode Curl_ssl_init_certinfo(संरचना Curl_easy *दत्त, पूर्णाङ्क अङ्कः);
CURLcode Curl_ssl_प्रक्षिप_certinfo_len(संरचना Curl_easy *दत्त, पूर्णाङ्क certnum,
                                    ध्रुव वर्ण *label, ध्रुव वर्ण *मानम्,
                                    size_t valuelen);
CURLcode Curl_ssl_प्रक्षिप_certinfo(संरचना Curl_easy *दत्त, पूर्णाङ्क certnum,
                                ध्रुव वर्ण *label, ध्रुव वर्ण *मानम्);

/* Functions to be used by SSL library adaptation functions */

/* get N random bytes into the buffer */
CURLcode Curl_ssl_random(संरचना Curl_easy *दत्त,
                         अचिह्नित वर्ण *बफर, size_t दीर्घता);
/* Check pinned public key. */
CURLcode Curl_pin_peer_pubkey(संरचना Curl_easy *दत्त,
                              ध्रुव वर्ण *pinnedpubkey,
                              ध्रुव अचिह्नित वर्ण *pubkey, size_t pubkeylen);

bool Curl_ssl_cert_स्थितिः_request(शून्य);

/* The maximum size of the SSL channel binding is 85 bytes, as defined in
 * RFC 5929, Section 4.1. The 'tls-server-end-point:' prefix is 21 bytes,
 * and SHA-512 is the longest supported hash algorithm, with a digest length of
 * 64 bytes.
 * The maximum size of the channel binding is therefore 21 + 64 = 85 bytes.
 */
#परिभाषय SSL_CB_MAX_आकार 85

/* Return the tls-server-end-point channel binding, including the
 * 'tls-server-end-point:' prefix.
 * If successful, the data is written to the dynbuf, and CURLE_OK is returned.
 * The dynbuf MUST HAVE a minimum toobig size of SSL_CB_MAX_SIZE.
 * If the dynbuf is too small, CURLE_OUT_OF_MEMORY is returned.
 * If channel binding is not supported, binding stays empty and CURLE_OK is
 * returned.
 */
CURLcode Curl_ssl_get_channel_binding(संरचना Curl_easy *दत्त, int8_t sockindex,
                                      संरचना dynbuf *binding);

#परिभाषय SSL_SHUTDOWN_TIMEOUT 10000 /* ms */

CURLcode Curl_ssl_cfilter_सङ्कलय(संरचना Curl_easy *दत्त,
                              संरचना Curl_peer *origin,
                              संरचना connectdata *conn,
                              int8_t sockindex);

CURLcode Curl_cf_ssl_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                  संरचना Curl_easy *दत्त,
                                  संरचना Curl_peer *origin,
                                  संरचना Curl_peer *peer);

CURLcode Curl_ssl_cfilter_remove(संरचना Curl_easy *दत्त,
                                 int8_t sockindex, bool send_shutdown);

#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
CURLcode Curl_cf_ssl_proxy_निवेशय_after(संरचना Curl_cfilter *cf_at,
                                        संरचना Curl_easy *दत्त,
                                        संरचना Curl_peer *peer);
#यदि_अन्तः /* !CURL_DISABLE_PROXY */

/**
 * True iff the underlying SSL implementation supports the option.
 * Option is one of the defined SSLSUPP_* values.
 * `data` maybe NULL for the features of the default implementation.
 */
bool Curl_ssl_supports(संरचना Curl_easy *दत्त, अचिह्नित पूर्णाङ्क ssl_option);

/* Get the ssl_easy_config from `data` relevant for cfilter `cf`. */
संरचना ssl_easy_config *
Curl_ssl_cf_get_easy_config(संरचना Curl_cfilter *cf,
                            संरचना Curl_easy *दत्त);

/* Get the filter config relevant for cfilter `cf`. */
संरचना ssl_filter_config *
Curl_ssl_cf_get_filter_config(संरचना Curl_cfilter *cf);

बाह्य संरचना Curl_cftype Curl_cft_ssl;
#यदि_अपरिभाषितम् CURL_DISABLE_PROXY
बाह्य संरचना Curl_cftype Curl_cft_ssl_proxy;
#यदि_अन्तः

#अन्यथा /* if not USE_SSL */

/* When SSL support is not present, define away these function calls */
#परिभाषय Curl_ssl_init() 1
#परिभाषय Curl_ssl_cleanup() Curl_nop_stmt
#परिभाषय Curl_ssl_close_all(x) Curl_nop_stmt
#परिभाषय Curl_ssl_समुच्चयः_engine(x, y) CURLE_NOT_BUILT_अन्तर्
#परिभाषय Curl_ssl_समुच्चयः_engine_default(x) CURLE_NOT_BUILT_अन्तर्
#परिभाषय Curl_ssl_engines_list(x) रिक्त
#परिभाषय Curl_ssl_free_certinfo(x) Curl_nop_stmt
#परिभाषय Curl_ssl_random(x, y, z) ((शून्य)(x), CURLE_NOT_BUILT_अन्तर्)
#परिभाषय Curl_ssl_cert_स्थितिः_request() FALSE
#परिभाषय Curl_ssl_supports(a, b) FALSE
#परिभाषय Curl_ssl_cfilter_सङ्कलय(a, b, c, d) CURLE_NOT_BUILT_अन्तर्
#परिभाषय Curl_ssl_cfilter_remove(a, b, c) CURLE_OK
#परिभाषय Curl_ssl_cf_get_easy_config(a, b) रिक्त
#परिभाषय Curl_ssl_cf_get_filter_config(a) रिक्त
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_VTLS_H */
