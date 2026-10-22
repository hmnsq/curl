#ifndef HEADER_CURL_VTLS_H
#define HEADER_CURL_VTLS_H
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

ഘടന connectdata;
ഘടന ssl_easy_config;
ഘടന ssl_filter_config;
ഘടന Curl_cfilter;
ഘടന Curl_easy;
ഘടന dynbuf;

#define SSLSUPP_CA_PATH      (1 << 0) /* supports CAPATH */
#define SSLSUPP_CERTINFO     (1 << 1) /* supports CURLOPT_CERTINFO */
#define SSLSUPP_PINNEDPUBKEY (1 << 2) /* supports CURLOPT_PINNEDPUBLICKEY */
#define SSLSUPP_SSL_CTX      (1 << 3) /* supports CURLOPT_SSL_CTX */
#define SSLSUPP_HTTPS_PROXY  (1 << 4) /* supports access via HTTPS proxies */
#define SSLSUPP_TLS13_CIPHERSUITES (1 << 5) /* supports TLS 1.3 ciphersuites */
#define SSLSUPP_CAINFO_BLOB  (1 << 6)
#define SSLSUPP_ECH          (1 << 7)
#define SSLSUPP_CA_CACHE     (1 << 8)
#define SSLSUPP_CIPHER_LIST  (1 << 9) /* supports TLS 1.0-1.2 ciphersuites */
#define SSLSUPP_SIGNATURE_ALGORITHMS (1 << 10) /* supports TLS sigalgs */
#define SSLSUPP_ISSUERCERT   (1 << 11) /* supports CURLOPT_ISSUERCERT */
#define SSLSUPP_SSL_EC_CURVES (1 << 12) /* supports CURLOPT_SSL_EC_CURVES */
#define SSLSUPP_CRLFILE      (1 << 13) /* supports CURLOPT_CRLFILE */
#define SSLSUPP_ISSUERCERT_BLOB (1 << 14) /* CURLOPT_ISSUERCERT_BLOB */

#ifdef USE_ECH
/* CURLECH_ values for the ech option */
#define CURLECH_DISABLE    0
#define CURLECH_GREASE     1
#define CURLECH_ENABLE     2
#define CURLECH_HARD       3

/* TRUE if ECH is enabled (and supported) */
bool Curl_ssl_ech_enabled(ഘടന Curl_cfilter *cf);
/* TRUE if SSL ECH configuration requires DNS HTTPS-RR record */
bool Curl_ssl_need_httpsrr(ഘടന Curl_cfilter *cf);
#അല്ലെങ്കിൽ
#define Curl_ssl_ech_enabled(c)      FALSE
#define Curl_ssl_need_httpsrr(c)     FALSE
#endif /* USE_ECH */

#define ALPN_ACCEPTED "ALPN: server accepted "

#define VTLS_INFOF_NO_ALPN            \
  "ALPN: server did not agree on a protocol. Uses default."
#define VTLS_INFOF_ALPN_OFFER_1STR    \
  "ALPN: curl offers %s"
#define VTLS_INFOF_ALPN_ACCEPTED      \
  ALPN_ACCEPTED "%.*s"

#define VTLS_INFOF_NO_ALPN_DEFERRED   \
  "ALPN: deferred handshake for early data without specific protocol."
#define VTLS_INFOF_ALPN_DEFERRED      \
  "ALPN: deferred handshake for early data using '%.*s'."

/* IETF defined version numbers used in TLS protocol negotiation */
#define CURL_IETF_PROTO_UNKNOWN       0x0
#define CURL_IETF_PROTO_SSL3          0x0300
#define CURL_IETF_PROTO_TLS1          0x0301
#define CURL_IETF_PROTO_TLS1_1        0x0302
#define CURL_IETF_PROTO_TLS1_2        0x0303
#define CURL_IETF_PROTO_TLS1_3        0x0304
#define CURL_IETF_PROTO_DTLS1         0xFEFF
#define CURL_IETF_PROTO_DTLS1_2       0xFEFD

തരനിർവചനം എണ്ണൽ {
  CURL_SSL_PEER_DNS,
  CURL_SSL_PEER_IPV4,
  CURL_SSL_PEER_IPV6
} ssl_peer_type;

ഘടന ssl_peer {
  ഘടന Curl_peer *origin; /* the authority we talk to */
  ഘടന Curl_peer *peer;   /* the machine we are connected to */
  അക്ഷരം *sni;             /* SNI version of hostname or NULL if not usable */
  അക്ഷരം *scache_താക്കോൽ;      /* for lookups in session cache */
  ssl_peer_type type;    /* type of the peer information */
  uint8_t transport;     /* one of TRNSPRT_* defines */
};

CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, സ്ഥിരം അക്ഷരം *name,
                                   സ്ഥിരം curl_ssl_backend ***avail);

#define MAX_PINNED_PUBKEY_SIZE (1024 * 1024) /* 1 MiB */

curl_sslbackend Curl_ssl_backend(ശൂന്യം);

/**
 * Init SSL peer information for filter. Can be called repeatedly.
 */
CURLcode Curl_ssl_peer_init(ഘടന ssl_peer *ssl_peer,
                            ഘടന Curl_peer *origin,
                            ഘടന Curl_peer *peer,
                            ഘടന ssl_filter_config *sslc,
                            സ്ഥിരം അക്ഷരം *tls_id,
                            uint8_t transport);
/**
 * Free all allocated data and reset peer information.
 */
ശൂന്യം Curl_ssl_peer_cleanup(ഘടന ssl_peer *peer);

#ifdef USE_SSL
പൂർണ്ണസംഖ്യ Curl_ssl_init(ശൂന്യം);
ശൂന്യം Curl_ssl_cleanup(ശൂന്യം);
/* tell the SSL stuff to close down all open information regarding
   connections (and thus session ID caching etc) */
ശൂന്യം Curl_ssl_close_all(ഘടന Curl_easy *data);
CURLcode Curl_ssl_set_engine(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *engine);
/* Sets engine as default for all SSL operations */
CURLcode Curl_ssl_set_engine_default(ഘടന Curl_easy *data);
ഘടന curl_slist *Curl_ssl_engines_list(ഘടന Curl_easy *data);

ശൂന്യം Curl_ssl_version(അക്ഷരം *buffer, size_t size);

/* Certificate information list handling. */
#define CURL_X509_STR_MAX  100000
#define MAX_ALLOWED_CERT_AMOUNT 100

ശൂന്യം Curl_ssl_free_certinfo(ഘടന Curl_easy *data);
CURLcode Curl_ssl_init_certinfo(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ അക്കം);
CURLcode Curl_ssl_തള്ളുക_certinfo_len(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ certnum,
                                    സ്ഥിരം അക്ഷരം *label, സ്ഥിരം അക്ഷരം *മൂല്യം,
                                    size_t valuelen);
CURLcode Curl_ssl_തള്ളുക_certinfo(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ certnum,
                                സ്ഥിരം അക്ഷരം *label, സ്ഥിരം അക്ഷരം *മൂല്യം);

/* Functions to be used by SSL library adaptation functions */

/* get N random bytes into the buffer */
CURLcode Curl_ssl_random(ഘടന Curl_easy *data,
                         unsigned അക്ഷരം *buffer, size_t length);
/* Check pinned public key. */
CURLcode Curl_pin_peer_pubkey(ഘടന Curl_easy *data,
                              സ്ഥിരം അക്ഷരം *pinnedpubkey,
                              സ്ഥിരം unsigned അക്ഷരം *pubkey, size_t pubkeylen);

bool Curl_ssl_cert_status_request(ശൂന്യം);

/* The maximum size of the SSL channel binding is 85 bytes, as defined in
 * RFC 5929, Section 4.1. The 'tls-server-end-point:' prefix is 21 bytes,
 * and SHA-512 is the longest supported hash algorithm, with a digest length of
 * 64 bytes.
 * The maximum size of the channel binding is therefore 21 + 64 = 85 bytes.
 */
#define SSL_CB_MAX_SIZE 85

/* Return the tls-server-end-point channel binding, including the
 * 'tls-server-end-point:' prefix.
 * If successful, the data is written to the dynbuf, and CURLE_OK is returned.
 * The dynbuf MUST HAVE a minimum toobig size of SSL_CB_MAX_SIZE.
 * If the dynbuf is too small, CURLE_OUT_OF_MEMORY is returned.
 * If channel binding is not supported, binding stays empty and CURLE_OK is
 * returned.
 */
CURLcode Curl_ssl_get_channel_binding(ഘടന Curl_easy *data, int8_t sockindex,
                                      ഘടന dynbuf *binding);

#define SSL_SHUTDOWN_TIMEOUT 10000 /* ms */

CURLcode Curl_ssl_cfilter_add(ഘടന Curl_easy *data,
                              ഘടന Curl_peer *origin,
                              ഘടന connectdata *conn,
                              int8_t sockindex);

CURLcode Curl_cf_ssl_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                  ഘടന Curl_easy *data,
                                  ഘടന Curl_peer *origin,
                                  ഘടന Curl_peer *peer);

CURLcode Curl_ssl_cfilter_remove(ഘടന Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown);

#ifndef CURL_DISABLE_PROXY
CURLcode Curl_cf_ssl_proxy_തിരുകുക_after(ഘടന Curl_cfilter *cf_at,
                                        ഘടന Curl_easy *data,
                                        ഘടന Curl_peer *peer);
#endif /* !CURL_DISABLE_PROXY */

/**
 * True iff the underlying SSL implementation supports the option.
 * Option is one of the defined SSLSUPP_* values.
 * `data` maybe NULL for the features of the default implementation.
 */
bool Curl_ssl_supports(ഘടന Curl_easy *data, unsigned പൂർണ്ണസംഖ്യ ssl_option);

/* Get the ssl_easy_config from `data` relevant for cfilter `cf`. */
ഘടന ssl_easy_config *
Curl_ssl_cf_get_easy_config(ഘടന Curl_cfilter *cf,
                            ഘടന Curl_easy *data);

/* Get the filter config relevant for cfilter `cf`. */
ഘടന ssl_filter_config *
Curl_ssl_cf_get_filter_config(ഘടന Curl_cfilter *cf);

ബാഹ്യ ഘടന Curl_cftype Curl_cft_ssl;
#ifndef CURL_DISABLE_PROXY
ബാഹ്യ ഘടന Curl_cftype Curl_cft_ssl_proxy;
#endif

#അല്ലെങ്കിൽ /* if not USE_SSL */

/* When SSL support is not present, define away these function calls */
#define Curl_ssl_init() 1
#define Curl_ssl_cleanup() Curl_nop_stmt
#define Curl_ssl_close_all(x) Curl_nop_stmt
#define Curl_ssl_set_engine(x, y) CURLE_NOT_BUILT_IN
#define Curl_ssl_set_engine_default(x) CURLE_NOT_BUILT_IN
#define Curl_ssl_engines_list(x) ശൂന്യ
#define Curl_ssl_free_certinfo(x) Curl_nop_stmt
#define Curl_ssl_random(x, y, z) ((ശൂന്യം)(x), CURLE_NOT_BUILT_IN)
#define Curl_ssl_cert_status_request() FALSE
#define Curl_ssl_supports(a, b) FALSE
#define Curl_ssl_cfilter_add(a, b, c, d) CURLE_NOT_BUILT_IN
#define Curl_ssl_cfilter_remove(a, b, c) CURLE_OK
#define Curl_ssl_cf_get_easy_config(a, b) ശൂന്യ
#define Curl_ssl_cf_get_filter_config(a) ശൂന്യ
#endif

#endif /* HEADER_CURL_VTLS_H */
