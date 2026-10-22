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
#અન્તર્ભાવ "curl_setup.h"

સંરચના connectdata;
સંરચના ssl_easy_config;
સંરચના ssl_filter_config;
સંરચના Curl_cfilter;
સંરચના Curl_easy;
સંરચના dynbuf;

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
bool Curl_ssl_ech_enabled(સંરચના Curl_cfilter *cf);
/* TRUE if SSL ECH configuration requires DNS HTTPS-RR record */
bool Curl_ssl_need_httpsrr(સંરચના Curl_cfilter *cf);
#નહીંતર
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

પ્રકારવ્યાખ્યા ગણના {
  CURL_SSL_PEER_DNS,
  CURL_SSL_PEER_IPV4,
  CURL_SSL_PEER_IPV6
} ssl_peer_type;

સંરચના ssl_peer {
  સંરચના Curl_peer *origin; /* the authority we talk to */
  સંરચના Curl_peer *peer;   /* the machine we are connected to */
  અક્ષર *sni;             /* SNI version of hostname or NULL if not usable */
  અક્ષર *scache_ચાવી;      /* for lookups in session cache */
  ssl_peer_type type;    /* type of the peer information */
  uint8_t transport;     /* one of TRNSPRT_* defines */
};

CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, અચલ અક્ષર *name,
                                   અચલ curl_ssl_backend ***avail);

#define MAX_PINNED_PUBKEY_SIZE (1024 * 1024) /* 1 MiB */

curl_sslbackend Curl_ssl_backend(શૂન્ય);

/**
 * Init SSL peer information for filter. Can be called repeatedly.
 */
CURLcode Curl_ssl_peer_init(સંરચના ssl_peer *ssl_peer,
                            સંરચના Curl_peer *origin,
                            સંરચના Curl_peer *peer,
                            સંરચના ssl_filter_config *sslc,
                            અચલ અક્ષર *tls_id,
                            uint8_t transport);
/**
 * Free all allocated data and reset peer information.
 */
શૂન્ય Curl_ssl_peer_cleanup(સંરચના ssl_peer *peer);

#ifdef USE_SSL
પૂર્ણાંક Curl_ssl_init(શૂન્ય);
શૂન્ય Curl_ssl_cleanup(શૂન્ય);
/* tell the SSL stuff to close down all open information regarding
   connections (and thus session ID caching etc) */
શૂન્ય Curl_ssl_close_all(સંરચના Curl_easy *data);
CURLcode Curl_ssl_set_engine(સંરચના Curl_easy *data, અચલ અક્ષર *engine);
/* Sets engine as default for all SSL operations */
CURLcode Curl_ssl_set_engine_default(સંરચના Curl_easy *data);
સંરચના curl_slist *Curl_ssl_engines_list(સંરચના Curl_easy *data);

શૂન્ય Curl_ssl_version(અક્ષર *buffer, size_t size);

/* Certificate information list handling. */
#define CURL_X509_STR_MAX  100000
#define MAX_ALLOWED_CERT_AMOUNT 100

શૂન્ય Curl_ssl_free_certinfo(સંરચના Curl_easy *data);
CURLcode Curl_ssl_init_certinfo(સંરચના Curl_easy *data, પૂર્ણાંક અંક);
CURLcode Curl_ssl_ધકેલો_certinfo_len(સંરચના Curl_easy *data, પૂર્ણાંક certnum,
                                    અચલ અક્ષર *label, અચલ અક્ષર *કિંમત,
                                    size_t valuelen);
CURLcode Curl_ssl_ધકેલો_certinfo(સંરચના Curl_easy *data, પૂર્ણાંક certnum,
                                અચલ અક્ષર *label, અચલ અક્ષર *કિંમત);

/* Functions to be used by SSL library adaptation functions */

/* get N random bytes into the buffer */
CURLcode Curl_ssl_random(સંરચના Curl_easy *data,
                         unsigned અક્ષર *buffer, size_t length);
/* Check pinned public key. */
CURLcode Curl_pin_peer_pubkey(સંરચના Curl_easy *data,
                              અચલ અક્ષર *pinnedpubkey,
                              અચલ unsigned અક્ષર *pubkey, size_t pubkeylen);

bool Curl_ssl_cert_status_request(શૂન્ય);

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
CURLcode Curl_ssl_get_channel_binding(સંરચના Curl_easy *data, int8_t sockindex,
                                      સંરચના dynbuf *binding);

#define SSL_SHUTDOWN_TIMEOUT 10000 /* ms */

CURLcode Curl_ssl_cfilter_add(સંરચના Curl_easy *data,
                              સંરચના Curl_peer *origin,
                              સંરચના connectdata *conn,
                              int8_t sockindex);

CURLcode Curl_cf_ssl_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                  સંરચના Curl_easy *data,
                                  સંરચના Curl_peer *origin,
                                  સંરચના Curl_peer *peer);

CURLcode Curl_ssl_cfilter_remove(સંરચના Curl_easy *data,
                                 int8_t sockindex, bool send_shutdown);

#ifndef CURL_DISABLE_PROXY
CURLcode Curl_cf_ssl_proxy_ઉમેરો_after(સંરચના Curl_cfilter *cf_at,
                                        સંરચના Curl_easy *data,
                                        સંરચના Curl_peer *peer);
#endif /* !CURL_DISABLE_PROXY */

/**
 * True iff the underlying SSL implementation supports the option.
 * Option is one of the defined SSLSUPP_* values.
 * `data` maybe NULL for the features of the default implementation.
 */
bool Curl_ssl_supports(સંરચના Curl_easy *data, unsigned પૂર્ણાંક ssl_option);

/* Get the ssl_easy_config from `data` relevant for cfilter `cf`. */
સંરચના ssl_easy_config *
Curl_ssl_cf_get_easy_config(સંરચના Curl_cfilter *cf,
                            સંરચના Curl_easy *data);

/* Get the filter config relevant for cfilter `cf`. */
સંરચના ssl_filter_config *
Curl_ssl_cf_get_filter_config(સંરચના Curl_cfilter *cf);

બાહ્ય સંરચના Curl_cftype Curl_cft_ssl;
#ifndef CURL_DISABLE_PROXY
બાહ્ય સંરચના Curl_cftype Curl_cft_ssl_proxy;
#endif

#નહીંતર /* if not USE_SSL */

/* When SSL support is not present, define away these function calls */
#define Curl_ssl_init() 1
#define Curl_ssl_cleanup() Curl_nop_stmt
#define Curl_ssl_close_all(x) Curl_nop_stmt
#define Curl_ssl_set_engine(x, y) CURLE_NOT_BUILT_IN
#define Curl_ssl_set_engine_default(x) CURLE_NOT_BUILT_IN
#define Curl_ssl_engines_list(x) ખાલી
#define Curl_ssl_free_certinfo(x) Curl_nop_stmt
#define Curl_ssl_random(x, y, z) ((શૂન્ય)(x), CURLE_NOT_BUILT_IN)
#define Curl_ssl_cert_status_request() FALSE
#define Curl_ssl_supports(a, b) FALSE
#define Curl_ssl_cfilter_add(a, b, c, d) CURLE_NOT_BUILT_IN
#define Curl_ssl_cfilter_remove(a, b, c) CURLE_OK
#define Curl_ssl_cf_get_easy_config(a, b) ખાલી
#define Curl_ssl_cf_get_filter_config(a) ખાલી
#endif

#endif /* HEADER_CURL_VTLS_H */
