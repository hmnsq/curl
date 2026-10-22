#اگر_تعریف_نشده سرآیند_CURL_VTLS_H
#تعریف_کن سرآیند_CURL_VTLS_H
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

#تعریف_کن SSLSUPP_CA_مسیر      (1 << 0) /* supports CAPATH */
#تعریف_کن SSLSUPP_CERTINFO     (1 << 1) /* supports CURLOPT_CERTINFO */
#تعریف_کن SSLSUPP_PINNEDPUBKEY (1 << 2) /* supports CURLOPT_PINNEDPUBLICKEY */
#تعریف_کن SSLSUPP_SSL_CTX      (1 << 3) /* supports CURLOPT_SSL_CTX */
#تعریف_کن SSLSUPP_HTTPS_PROXY  (1 << 4) /* supports access via HTTPS proxies */
#تعریف_کن SSLSUPP_TLS13_CIPHERSUITES (1 << 5) /* supports TLS 1.3 ciphersuites */
#تعریف_کن SSLSUPP_CAINFO_BLOB  (1 << 6)
#تعریف_کن SSLSUPP_ECH          (1 << 7)
#تعریف_کن SSLSUPP_CA_CACHE     (1 << 8)
#تعریف_کن SSLSUPP_CIPHER_LIST  (1 << 9) /* supports TLS 1.0-1.2 ciphersuites */
#تعریف_کن SSLSUPP_SIGNATURE_ALGORITHMS (1 << 10) /* supports TLS sigalgs */
#تعریف_کن SSLSUPP_ISSUERCERT   (1 << 11) /* supports CURLOPT_ISSUERCERT */
#تعریف_کن SSLSUPP_SSL_EC_CURVES (1 << 12) /* supports CURLOPT_SSL_EC_CURVES */
#تعریف_کن SSLSUPP_CRLFILE      (1 << 13) /* supports CURLOPT_CRLFILE */
#تعریف_کن SSLSUPP_ISSUERCERT_BLOB (1 << 14) /* CURLOPT_ISSUERCERT_BLOB */

#اگر_تعریف_شده USE_ECH
/* CURLECH_ values for the ech option */
#تعریف_کن CURLECH_DISABLE    0
#تعریف_کن CURLECH_GREASE     1
#تعریف_کن CURLECH_ENABLE     2
#تعریف_کن CURLECH_HARD       3

/* TRUE if ECH is enabled (and supported) */
bool Curl_ssl_ech_enabled(بنية Curl_cfilter *cf);
/* TRUE if SSL ECH configuration requires DNS HTTPS-RR record */
bool Curl_ssl_need_httpsrr(بنية Curl_cfilter *cf);
#وإلا
#تعریف_کن Curl_ssl_ech_enabled(c)      FALSE
#تعریف_کن Curl_ssl_need_httpsrr(c)     FALSE
#پایان_اگر /* USE_ECH */

#تعریف_کن ALPN_ACCEPTED "ALPN: server accepted "

#تعریف_کن VTLS_INFOF_NO_ALPN            \
  "ALPN: server did not agree on a protocol. Uses default."
#تعریف_کن VTLS_INFOF_ALPN_OFFER_1STR    \
  "ALPN: curl offers %s"
#تعریف_کن VTLS_INFOF_ALPN_ACCEPTED      \
  ALPN_ACCEPTED "%.*s"

#تعریف_کن VTLS_INFOF_NO_ALPN_DEFERRED   \
  "ALPN: deferred handshake for early data without specific protocol."
#تعریف_کن VTLS_INFOF_ALPN_DEFERRED      \
  "ALPN: deferred handshake for early data using '%.*s'."

/* IETF defined version numbers used in TLS protocol negotiation */
#تعریف_کن CURL_IETF_PROTO_UNKNOWN       0x0
#تعریف_کن CURL_IETF_PROTO_SSL3          0x0300
#تعریف_کن CURL_IETF_PROTO_TLS1          0x0301
#تعریف_کن CURL_IETF_PROTO_TLS1_1        0x0302
#تعریف_کن CURL_IETF_PROTO_TLS1_2        0x0303
#تعریف_کن CURL_IETF_PROTO_TLS1_3        0x0304
#تعریف_کن CURL_IETF_PROTO_DTLS1         0xFEFF
#تعریف_کن CURL_IETF_PROTO_DTLS1_2       0xFEFD

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

#تعریف_کن MAX_PINNED_PUBKEY_اندازه (1024 * 1024) /* 1 MiB */

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

#اگر_تعریف_شده USE_SSL
صحيح Curl_ssl_init(فراغ);
فراغ Curl_ssl_cleanup(فراغ);
/* tell the SSL stuff to close down all open information regarding
   connections (and thus session ID caching etc) */
فراغ Curl_ssl_close_all(بنية Curl_easy *داده);
CURLcode Curl_ssl_مجموعه_engine(بنية Curl_easy *داده, ثابت حرف *engine);
/* Sets engine as default for all SSL operations */
CURLcode Curl_ssl_مجموعه_engine_default(بنية Curl_easy *داده);
بنية curl_slist *Curl_ssl_engines_list(بنية Curl_easy *داده);

فراغ Curl_ssl_نسخه(حرف *میانگیر, size_t اندازه);

/* Certificate information list handling. */
#تعریف_کن CURL_X509_STR_MAX  100000
#تعریف_کن MAX_ALLOWED_CERT_AMOUNT 100

فراغ Curl_ssl_free_certinfo(بنية Curl_easy *داده);
CURLcode Curl_ssl_init_certinfo(بنية Curl_easy *داده, صحيح رقم);
CURLcode Curl_ssl_افزودن_certinfo_len(بنية Curl_easy *داده, صحيح certnum,
                                    ثابت حرف *label, ثابت حرف *مقدار,
                                    size_t valuelen);
CURLcode Curl_ssl_افزودن_certinfo(بنية Curl_easy *داده, صحيح certnum,
                                ثابت حرف *label, ثابت حرف *مقدار);

/* Functions to be used by SSL library adaptation functions */

/* get N random bytes into the buffer */
CURLcode Curl_ssl_random(بنية Curl_easy *داده,
                         بیعلامت حرف *میانگیر, size_t درازی);
/* Check pinned public key. */
CURLcode Curl_pin_peer_pubkey(بنية Curl_easy *داده,
                              ثابت حرف *pinnedpubkey,
                              ثابت بیعلامت حرف *pubkey, size_t pubkeylen);

bool Curl_ssl_cert_وضعیت_request(فراغ);

/* The maximum size of the SSL channel binding is 85 bytes, as defined in
 * RFC 5929, Section 4.1. The 'tls-server-end-point:' prefix is 21 bytes,
 * and SHA-512 is the longest supported hash algorithm, with a digest length of
 * 64 bytes.
 * The maximum size of the channel binding is therefore 21 + 64 = 85 bytes.
 */
#تعریف_کن SSL_CB_MAX_اندازه 85

/* Return the tls-server-end-point channel binding, including the
 * 'tls-server-end-point:' prefix.
 * If successful, the data is written to the dynbuf, and CURLE_OK is returned.
 * The dynbuf MUST HAVE a minimum toobig size of SSL_CB_MAX_SIZE.
 * If the dynbuf is too small, CURLE_OUT_OF_MEMORY is returned.
 * If channel binding is not supported, binding stays empty and CURLE_OK is
 * returned.
 */
CURLcode Curl_ssl_get_channel_binding(بنية Curl_easy *داده, int8_t sockindex,
                                      بنية dynbuf *binding);

#تعریف_کن SSL_SHUTDOWN_TIMEOUT 10000 /* ms */

CURLcode Curl_ssl_cfilter_جمع_کن(بنية Curl_easy *داده,
                              بنية Curl_peer *origin,
                              بنية connectdata *conn,
                              int8_t sockindex);

CURLcode Curl_cf_ssl_درج_after(بنية Curl_cfilter *cf_at,
                                  بنية Curl_easy *داده,
                                  بنية Curl_peer *origin,
                                  بنية Curl_peer *peer);

CURLcode Curl_ssl_cfilter_remove(بنية Curl_easy *داده,
                                 int8_t sockindex, bool send_shutdown);

#اگر_تعریف_نشده CURL_DISABLE_PROXY
CURLcode Curl_cf_ssl_proxy_درج_after(بنية Curl_cfilter *cf_at,
                                        بنية Curl_easy *داده,
                                        بنية Curl_peer *peer);
#پایان_اگر /* !CURL_DISABLE_PROXY */

/**
 * True iff the underlying SSL implementation supports the option.
 * Option is one of the defined SSLSUPP_* values.
 * `data` maybe NULL for the features of the default implementation.
 */
bool Curl_ssl_supports(بنية Curl_easy *داده, بیعلامت صحيح ssl_option);

/* Get the ssl_easy_config from `data` relevant for cfilter `cf`. */
بنية ssl_easy_config *
Curl_ssl_cf_get_easy_config(بنية Curl_cfilter *cf,
                            بنية Curl_easy *داده);

/* Get the filter config relevant for cfilter `cf`. */
بنية ssl_filter_config *
Curl_ssl_cf_get_filter_config(بنية Curl_cfilter *cf);

خارجي بنية Curl_cftype Curl_cft_ssl;
#اگر_تعریف_نشده CURL_DISABLE_PROXY
خارجي بنية Curl_cftype Curl_cft_ssl_proxy;
#پایان_اگر

#وإلا /* if not USE_SSL */

/* When SSL support is not present, define away these function calls */
#تعریف_کن Curl_ssl_init() 1
#تعریف_کن Curl_ssl_cleanup() Curl_nop_stmt
#تعریف_کن Curl_ssl_close_all(x) Curl_nop_stmt
#تعریف_کن Curl_ssl_مجموعه_engine(x, y) CURLE_NOT_BUILT_درون
#تعریف_کن Curl_ssl_مجموعه_engine_default(x) CURLE_NOT_BUILT_درون
#تعریف_کن Curl_ssl_engines_list(x) عدم
#تعریف_کن Curl_ssl_free_certinfo(x) Curl_nop_stmt
#تعریف_کن Curl_ssl_random(x, y, z) ((فراغ)(x), CURLE_NOT_BUILT_درون)
#تعریف_کن Curl_ssl_cert_وضعیت_request() FALSE
#تعریف_کن Curl_ssl_supports(a, b) FALSE
#تعریف_کن Curl_ssl_cfilter_جمع_کن(a, b, c, d) CURLE_NOT_BUILT_درون
#تعریف_کن Curl_ssl_cfilter_remove(a, b, c) CURLE_OK
#تعریف_کن Curl_ssl_cf_get_easy_config(a, b) عدم
#تعریف_کن Curl_ssl_cf_get_filter_config(a) عدم
#پایان_اگر

#پایان_اگر /* HEADER_CURL_VTLS_H */
