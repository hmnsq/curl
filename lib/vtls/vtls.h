#جے_بنا_تعریف سرخی_CURL_VTLS_H
#تعریف_کرو سرخی_CURL_VTLS_H
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
#ضمن "curl_setup.h"

بنية connectdata;
بنية ssl_easy_config;
بنية ssl_filter_config;
بنية Curl_cfilter;
بنية Curl_easy;
بنية dynbuf;

#تعریف_کرو SSLSUPP_CA_رستہ      (1 << 0) /* supports CAPATH */
#تعریف_کرو SSLSUPP_CERTINFO     (1 << 1) /* supports CURLOPT_CERTINFO */
#تعریف_کرو SSLSUPP_PINNEDPUBKEY (1 << 2) /* supports CURLOPT_PINNEDPUBLICKEY */
#تعریف_کرو SSLSUPP_SSL_CTX      (1 << 3) /* supports CURLOPT_SSL_CTX */
#تعریف_کرو SSLSUPP_HTTPS_PROXY  (1 << 4) /* supports access via HTTPS proxies */
#تعریف_کرو SSLSUPP_TLS13_CIPHERSUITES (1 << 5) /* supports TLS 1.3 ciphersuites */
#تعریف_کرو SSLSUPP_CAINFO_BLOB  (1 << 6)
#تعریف_کرو SSLSUPP_ECH          (1 << 7)
#تعریف_کرو SSLSUPP_CA_CACHE     (1 << 8)
#تعریف_کرو SSLSUPP_CIPHER_LIST  (1 << 9) /* supports TLS 1.0-1.2 ciphersuites */
#تعریف_کرو SSLSUPP_SIGNATURE_ALGORITHMS (1 << 10) /* supports TLS sigalgs */
#تعریف_کرو SSLSUPP_ISSUERCERT   (1 << 11) /* supports CURLOPT_ISSUERCERT */
#تعریف_کرو SSLSUPP_SSL_EC_CURVES (1 << 12) /* supports CURLOPT_SSL_EC_CURVES */
#تعریف_کرو SSLSUPP_CRLFILE      (1 << 13) /* supports CURLOPT_CRLFILE */
#تعریف_کرو SSLSUPP_ISSUERCERT_BLOB (1 << 14) /* CURLOPT_ISSUERCERT_BLOB */

#جے_تعریف_شدہ USE_ECH
/* CURLECH_ values for the ech option */
#تعریف_کرو CURLECH_DISABLE    0
#تعریف_کرو CURLECH_GREASE     1
#تعریف_کرو CURLECH_ENABLE     2
#تعریف_کرو CURLECH_HARD       3

/* TRUE if ECH is enabled (and supported) */
bool Curl_ssl_ech_enabled(بنية Curl_cfilter *cf);
/* TRUE if SSL ECH configuration requires DNS HTTPS-RR record */
bool Curl_ssl_need_httpsrr(بنية Curl_cfilter *cf);
#وإلا
#تعریف_کرو Curl_ssl_ech_enabled(c)      FALSE
#تعریف_کرو Curl_ssl_need_httpsrr(c)     FALSE
#جے_انت /* USE_ECH */

#تعریف_کرو ALPN_ACCEPTED "ALPN: server accepted "

#تعریف_کرو VTLS_INFOF_NO_ALPN            \
  "ALPN: server did not agree on a protocol. Uses default."
#تعریف_کرو VTLS_INFOF_ALPN_OFFER_1STR    \
  "ALPN: curl offers %s"
#تعریف_کرو VTLS_INFOF_ALPN_ACCEPTED      \
  ALPN_ACCEPTED "%.*s"

#تعریف_کرو VTLS_INFOF_NO_ALPN_DEFERRED   \
  "ALPN: deferred handshake for early data without specific protocol."
#تعریف_کرو VTLS_INFOF_ALPN_DEFERRED      \
  "ALPN: deferred handshake for early data using '%.*s'."

/* IETF defined version numbers used in TLS protocol negotiation */
#تعریف_کرو CURL_IETF_PROTO_UNKNOWN       0x0
#تعریف_کرو CURL_IETF_PROTO_SSL3          0x0300
#تعریف_کرو CURL_IETF_PROTO_TLS1          0x0301
#تعریف_کرو CURL_IETF_PROTO_TLS1_1        0x0302
#تعریف_کرو CURL_IETF_PROTO_TLS1_2        0x0303
#تعریف_کرو CURL_IETF_PROTO_TLS1_3        0x0304
#تعریف_کرو CURL_IETF_PROTO_DTLS1         0xFEFF
#تعریف_کرو CURL_IETF_PROTO_DTLS1_2       0xFEFD

تعريفنوع تعداد {
  CURL_SSL_PEER_DNS,
  CURL_SSL_PEER_IPV4,
  CURL_SSL_PEER_IPV6
} ssl_peer_type;

بنية ssl_peer {
  بنية Curl_peer *origin; /* the authority we talk to */
  بنية Curl_peer *peer;   /* the machine we are connected to */
  حرف *sni;             /* SNI version of hostname or NULL if not usable */
  حرف *scache_کلید;      /* for lookups in session cache */
  ssl_peer_type type;    /* type of the peer information */
  uint8_t transport;     /* one of TRNSPRT_* defines */
};

CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, ثابت حرف *name,
                                   ثابت curl_ssl_backend ***avail);

#تعریف_کرو MAX_PINNED_PUBKEY_ناپ (1024 * 1024) /* 1 MiB */

curl_sslbackend Curl_ssl_backend(فراغ);

/**
 * Init SSL peer information for filter. Can be called repeatedly.
 */
CURLcode Curl_ssl_peer_init(بنية ssl_peer *ssl_peer,
                            بنية Curl_peer *origin,
                            بنية Curl_peer *peer,
                            بنية ssl_filter_config *sslc,
                            ثابت حرف *tls_id,
                            uint8_t transport);
/**
 * Free all allocated data and reset peer information.
 */
فراغ Curl_ssl_peer_cleanup(بنية ssl_peer *peer);

#جے_تعریف_شدہ USE_SSL
صحيح Curl_ssl_init(فراغ);
فراغ Curl_ssl_cleanup(فراغ);
/* tell the SSL stuff to close down all open information regarding
   connections (and thus session ID caching etc) */
فراغ Curl_ssl_close_all(بنية Curl_easy *انکڑے);
CURLcode Curl_ssl_سیٹ_engine(بنية Curl_easy *انکڑے, ثابت حرف *engine);
/* Sets engine as default for all SSL operations */
CURLcode Curl_ssl_سیٹ_engine_default(بنية Curl_easy *انکڑے);
بنية curl_slist *Curl_ssl_engines_list(بنية Curl_easy *انکڑے);

فراغ Curl_ssl_نسخہ(حرف *بفر, size_t ناپ);

/* Certificate information list handling. */
#تعریف_کرو CURL_X509_STR_MAX  100000
#تعریف_کرو MAX_ALLOWED_CERT_AMOUNT 100

فراغ Curl_ssl_free_certinfo(بنية Curl_easy *انکڑے);
CURLcode Curl_ssl_init_certinfo(بنية Curl_easy *انکڑے, صحيح عدد);
CURLcode Curl_ssl_ڈالو_certinfo_len(بنية Curl_easy *انکڑے, صحيح certnum,
                                    ثابت حرف *label, ثابت حرف *قدر,
                                    size_t valuelen);
CURLcode Curl_ssl_ڈالو_certinfo(بنية Curl_easy *انکڑے, صحيح certnum,
                                ثابت حرف *label, ثابت حرف *قدر);

/* Functions to be used by SSL library adaptation functions */

/* get N random bytes into the buffer */
CURLcode Curl_ssl_random(بنية Curl_easy *انکڑے,
                         بنانشانی حرف *بفر, size_t لمائی);
/* Check pinned public key. */
CURLcode Curl_pin_peer_pubkey(بنية Curl_easy *انکڑے,
                              ثابت حرف *pinnedpubkey,
                              ثابت بنانشانی حرف *pubkey, size_t pubkeylen);

bool Curl_ssl_cert_حالت_request(فراغ);

/* The maximum size of the SSL channel binding is 85 bytes, as defined in
 * RFC 5929, Section 4.1. The 'tls-server-end-point:' prefix is 21 bytes,
 * and SHA-512 is the longest supported hash algorithm, with a digest length of
 * 64 bytes.
 * The maximum size of the channel binding is therefore 21 + 64 = 85 bytes.
 */
#تعریف_کرو SSL_CB_MAX_ناپ 85

/* Return the tls-server-end-point channel binding, including the
 * 'tls-server-end-point:' prefix.
 * If successful, the data is written to the dynbuf, and CURLE_OK is returned.
 * The dynbuf MUST HAVE a minimum toobig size of SSL_CB_MAX_SIZE.
 * If the dynbuf is too small, CURLE_OUT_OF_MEMORY is returned.
 * If channel binding is not supported, binding stays empty and CURLE_OK is
 * returned.
 */
CURLcode Curl_ssl_get_channel_binding(بنية Curl_easy *انکڑے, int8_t sockindex,
                                      بنية dynbuf *binding);

#تعریف_کرو SSL_SHUTDOWN_TIMEOUT 10000 /* ms */

CURLcode Curl_ssl_cfilter_جوڑو(بنية Curl_easy *انکڑے,
                              بنية Curl_peer *origin,
                              بنية connectdata *conn,
                              int8_t sockindex);

CURLcode Curl_cf_ssl_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                  بنية Curl_easy *انکڑے,
                                  بنية Curl_peer *origin,
                                  بنية Curl_peer *peer);

CURLcode Curl_ssl_cfilter_remove(بنية Curl_easy *انکڑے,
                                 int8_t sockindex, bool send_shutdown);

#جے_بنا_تعریف CURL_DISABLE_PROXY
CURLcode Curl_cf_ssl_proxy_داخل_کرو_after(بنية Curl_cfilter *cf_at,
                                        بنية Curl_easy *انکڑے,
                                        بنية Curl_peer *peer);
#جے_انت /* !CURL_DISABLE_PROXY */

/**
 * True iff the underlying SSL implementation supports the option.
 * Option is one of the defined SSLSUPP_* values.
 * `data` maybe NULL for the features of the default implementation.
 */
bool Curl_ssl_supports(بنية Curl_easy *انکڑے, بنانشانی صحيح ssl_option);

/* Get the ssl_easy_config from `data` relevant for cfilter `cf`. */
بنية ssl_easy_config *
Curl_ssl_cf_get_easy_config(بنية Curl_cfilter *cf,
                            بنية Curl_easy *انکڑے);

/* Get the filter config relevant for cfilter `cf`. */
بنية ssl_filter_config *
Curl_ssl_cf_get_filter_config(بنية Curl_cfilter *cf);

خارجي بنية Curl_cftype Curl_cft_ssl;
#جے_بنا_تعریف CURL_DISABLE_PROXY
خارجي بنية Curl_cftype Curl_cft_ssl_proxy;
#جے_انت

#وإلا /* if not USE_SSL */

/* When SSL support is not present, define away these function calls */
#تعریف_کرو Curl_ssl_init() 1
#تعریف_کرو Curl_ssl_cleanup() Curl_nop_stmt
#تعریف_کرو Curl_ssl_close_all(x) Curl_nop_stmt
#تعریف_کرو Curl_ssl_سیٹ_engine(x, y) CURLE_NOT_BUILT_وچ
#تعریف_کرو Curl_ssl_سیٹ_engine_default(x) CURLE_NOT_BUILT_وچ
#تعریف_کرو Curl_ssl_engines_list(x) عدم
#تعریف_کرو Curl_ssl_free_certinfo(x) Curl_nop_stmt
#تعریف_کرو Curl_ssl_random(x, y, z) ((فراغ)(x), CURLE_NOT_BUILT_وچ)
#تعریف_کرو Curl_ssl_cert_حالت_request() FALSE
#تعریف_کرو Curl_ssl_supports(a, b) FALSE
#تعریف_کرو Curl_ssl_cfilter_جوڑو(a, b, c, d) CURLE_NOT_BUILT_وچ
#تعریف_کرو Curl_ssl_cfilter_remove(a, b, c) CURLE_OK
#تعریف_کرو Curl_ssl_cf_get_easy_config(a, b) عدم
#تعریف_کرو Curl_ssl_cf_get_filter_config(a) عدم
#جے_انت

#جے_انت /* HEADER_CURL_VTLS_H */
