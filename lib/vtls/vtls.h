#अगर_अघोषित HEADER_CURL_VTLS_H
#घोषणा HEADER_CURL_VTLS_H
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
#समावेश "curl_setup.h"

काष्ठा connectdata;
काष्ठा ssl_easy_config;
काष्ठा ssl_filter_config;
काष्ठा Curl_cfilter;
काष्ठा Curl_easy;
काष्ठा dynbuf;

#घोषणा SSLSUPP_CA_PATH      (1 << 0) /* supports CAPATH */
#घोषणा SSLSUPP_CERTINFO     (1 << 1) /* supports CURLOPT_CERTINFO */
#घोषणा SSLSUPP_PINNEDPUBKEY (1 << 2) /* supports CURLOPT_PINNEDPUBLICKEY */
#घोषणा SSLSUPP_SSL_CTX      (1 << 3) /* supports CURLOPT_SSL_CTX */
#घोषणा SSLSUPP_HTTPS_PROXY  (1 << 4) /* supports access via HTTPS proxies */
#घोषणा SSLSUPP_TLS13_CIPHERSUITES (1 << 5) /* supports TLS 1.3 ciphersuites */
#घोषणा SSLSUPP_CAINFO_BLOB  (1 << 6)
#घोषणा SSLSUPP_ECH          (1 << 7)
#घोषणा SSLSUPP_CA_CACHE     (1 << 8)
#घोषणा SSLSUPP_CIPHER_LIST  (1 << 9) /* supports TLS 1.0-1.2 ciphersuites */
#घोषणा SSLSUPP_SIGNATURE_ALGORITHMS (1 << 10) /* supports TLS sigalgs */
#घोषणा SSLSUPP_ISSUERCERT   (1 << 11) /* supports CURLOPT_ISSUERCERT */
#घोषणा SSLSUPP_SSL_EC_CURVES (1 << 12) /* supports CURLOPT_SSL_EC_CURVES */
#घोषणा SSLSUPP_CRLFILE      (1 << 13) /* supports CURLOPT_CRLFILE */
#घोषणा SSLSUPP_ISSUERCERT_BLOB (1 << 14) /* CURLOPT_ISSUERCERT_BLOB */

#अगर_घोषित USE_ECH
/* CURLECH_ values for the ech option */
#घोषणा CURLECH_DISABLE    0
#घोषणा CURLECH_GREASE     1
#घोषणा CURLECH_ENABLE     2
#घोषणा CURLECH_HARD       3

/* TRUE if ECH is enabled (and supported) */
bool Curl_ssl_ech_enabled(काष्ठा Curl_cfilter *cf);
/* TRUE if SSL ECH configuration requires DNS HTTPS-RR record */
bool Curl_ssl_need_httpsrr(काष्ठा Curl_cfilter *cf);
#अन्यथा
#घोषणा Curl_ssl_ech_enabled(c)      FALSE
#घोषणा Curl_ssl_need_httpsrr(c)     FALSE
#पूर्ण_अगर /* USE_ECH */

#घोषणा ALPN_ACCEPTED "ALPN: server accepted "

#घोषणा VTLS_INFOF_NO_ALPN            \
  "ALPN: server did not agree on a protocol. Uses default."
#घोषणा VTLS_INFOF_ALPN_OFFER_1STR    \
  "ALPN: curl offers %s"
#घोषणा VTLS_INFOF_ALPN_ACCEPTED      \
  ALPN_ACCEPTED "%.*s"

#घोषणा VTLS_INFOF_NO_ALPN_DEFERRED   \
  "ALPN: deferred handshake for early data without specific protocol."
#घोषणा VTLS_INFOF_ALPN_DEFERRED      \
  "ALPN: deferred handshake for early data using '%.*s'."

/* IETF defined version numbers used in TLS protocol negotiation */
#घोषणा CURL_IETF_PROTO_UNKNOWN       0x0
#घोषणा CURL_IETF_PROTO_SSL3          0x0300
#घोषणा CURL_IETF_PROTO_TLS1          0x0301
#घोषणा CURL_IETF_PROTO_TLS1_1        0x0302
#घोषणा CURL_IETF_PROTO_TLS1_2        0x0303
#घोषणा CURL_IETF_PROTO_TLS1_3        0x0304
#घोषणा CURL_IETF_PROTO_DTLS1         0xFEFF
#घोषणा CURL_IETF_PROTO_DTLS1_2       0xFEFD

प्रकार क्रमागत {
  CURL_SSL_PEER_DNS,
  CURL_SSL_PEER_IPV4,
  CURL_SSL_PEER_IPV6
} ssl_peer_type;

काष्ठा ssl_peer {
  काष्ठा Curl_peer *origin; /* the authority we talk to */
  काष्ठा Curl_peer *peer;   /* the machine we are connected to */
  अक्षर *sni;             /* SNI version of hostname or NULL if not usable */
  अक्षर *scache_कुंजी;      /* for lookups in session cache */
  ssl_peer_type type;    /* type of the peer information */
  uint8_t transport;     /* one of TRNSPRT_* defines */
};

CURLsslset Curl_init_sslset_nolock(curl_sslbackend id, स्थिर अक्षर *name,
                                   स्थिर curl_ssl_backend ***avail);

#घोषणा MAX_PINNED_PUBKEY_आकार (1024 * 1024) /* 1 MiB */

curl_sslbackend Curl_ssl_backend(व्योम);

/**
 * Init SSL peer information for filter. Can be called repeatedly.
 */
CURLcode Curl_ssl_peer_init(काष्ठा ssl_peer *ssl_peer,
                            काष्ठा Curl_peer *origin,
                            काष्ठा Curl_peer *peer,
                            काष्ठा ssl_filter_config *sslc,
                            स्थिर अक्षर *tls_id,
                            uint8_t transport);
/**
 * Free all allocated data and reset peer information.
 */
व्योम Curl_ssl_peer_cleanup(काष्ठा ssl_peer *peer);

#अगर_घोषित USE_SSL
पूर्णांक Curl_ssl_init(व्योम);
व्योम Curl_ssl_cleanup(व्योम);
/* tell the SSL stuff to close down all open information regarding
   connections (and thus session ID caching etc) */
व्योम Curl_ssl_close_all(काष्ठा Curl_easy *तथ्य);
CURLcode Curl_ssl_set_engine(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *engine);
/* Sets engine as default for all SSL operations */
CURLcode Curl_ssl_set_engine_default(काष्ठा Curl_easy *तथ्य);
काष्ठा curl_slist *Curl_ssl_engines_list(काष्ठा Curl_easy *तथ्य);

व्योम Curl_ssl_version(अक्षर *बफर, माप_प्रकार आकार);

/* Certificate information list handling. */
#घोषणा CURL_X509_STR_MAX  100000
#घोषणा MAX_ALLOWED_CERT_AMOUNT 100

व्योम Curl_ssl_free_certinfo(काष्ठा Curl_easy *तथ्य);
CURLcode Curl_ssl_init_certinfo(काष्ठा Curl_easy *तथ्य, पूर्णांक अंक);
CURLcode Curl_ssl_धकेलो_certinfo_len(काष्ठा Curl_easy *तथ्य, पूर्णांक certnum,
                                    स्थिर अक्षर *label, स्थिर अक्षर *मूल्य,
                                    माप_प्रकार valuelen);
CURLcode Curl_ssl_धकेलो_certinfo(काष्ठा Curl_easy *तथ्य, पूर्णांक certnum,
                                स्थिर अक्षर *label, स्थिर अक्षर *मूल्य);

/* Functions to be used by SSL library adaptation functions */

/* get N random bytes into the buffer */
CURLcode Curl_ssl_random(काष्ठा Curl_easy *तथ्य,
                         अचिन्हित अक्षर *बफर, माप_प्रकार लंबाई);
/* Check pinned public key. */
CURLcode Curl_pin_peer_pubkey(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर *pinnedpubkey,
                              स्थिर अचिन्हित अक्षर *pubkey, माप_प्रकार pubkeylen);

bool Curl_ssl_cert_status_request(व्योम);

/* The maximum size of the SSL channel binding is 85 bytes, as defined in
 * RFC 5929, Section 4.1. The 'tls-server-end-point:' prefix is 21 bytes,
 * and SHA-512 is the longest supported hash algorithm, with a digest length of
 * 64 bytes.
 * The maximum size of the channel binding is therefore 21 + 64 = 85 bytes.
 */
#घोषणा SSL_CB_MAX_आकार 85

/* Return the tls-server-end-point channel binding, including the
 * 'tls-server-end-point:' prefix.
 * If successful, the data is written to the dynbuf, and CURLE_OK is returned.
 * The dynbuf MUST HAVE a minimum toobig size of SSL_CB_MAX_SIZE.
 * If the dynbuf is too small, CURLE_OUT_OF_MEMORY is returned.
 * If channel binding is not supported, binding stays empty and CURLE_OK is
 * returned.
 */
CURLcode Curl_ssl_get_channel_binding(काष्ठा Curl_easy *तथ्य, int8_t sockindex,
                                      काष्ठा dynbuf *binding);

#घोषणा SSL_SHUTDOWN_TIMEOUT 10000 /* ms */

CURLcode Curl_ssl_cfilter_add(काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_peer *origin,
                              काष्ठा connectdata *conn,
                              int8_t sockindex);

CURLcode Curl_cf_ssl_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                  काष्ठा Curl_easy *तथ्य,
                                  काष्ठा Curl_peer *origin,
                                  काष्ठा Curl_peer *peer);

CURLcode Curl_ssl_cfilter_remove(काष्ठा Curl_easy *तथ्य,
                                 int8_t sockindex, bool send_shutdown);

#अगर_अघोषित CURL_DISABLE_PROXY
CURLcode Curl_cf_ssl_proxy_जोड़ो_after(काष्ठा Curl_cfilter *cf_at,
                                        काष्ठा Curl_easy *तथ्य,
                                        काष्ठा Curl_peer *peer);
#पूर्ण_अगर /* !CURL_DISABLE_PROXY */

/**
 * True iff the underlying SSL implementation supports the option.
 * Option is one of the defined SSLSUPP_* values.
 * `data` maybe NULL for the features of the default implementation.
 */
bool Curl_ssl_supports(काष्ठा Curl_easy *तथ्य, अचिन्हित पूर्णांक ssl_option);

/* Get the ssl_easy_config from `data` relevant for cfilter `cf`. */
काष्ठा ssl_easy_config *
Curl_ssl_cf_get_easy_config(काष्ठा Curl_cfilter *cf,
                            काष्ठा Curl_easy *तथ्य);

/* Get the filter config relevant for cfilter `cf`. */
काष्ठा ssl_filter_config *
Curl_ssl_cf_get_filter_config(काष्ठा Curl_cfilter *cf);

बाह्य काष्ठा Curl_cftype Curl_cft_ssl;
#अगर_अघोषित CURL_DISABLE_PROXY
बाह्य काष्ठा Curl_cftype Curl_cft_ssl_proxy;
#पूर्ण_अगर

#अन्यथा /* if not USE_SSL */

/* When SSL support is not present, define away these function calls */
#घोषणा Curl_ssl_init() 1
#घोषणा Curl_ssl_cleanup() Curl_nop_stmt
#घोषणा Curl_ssl_close_all(x) Curl_nop_stmt
#घोषणा Curl_ssl_set_engine(x, y) CURLE_NOT_BUILT_IN
#घोषणा Curl_ssl_set_engine_default(x) CURLE_NOT_BUILT_IN
#घोषणा Curl_ssl_engines_list(x) शून्य
#घोषणा Curl_ssl_free_certinfo(x) Curl_nop_stmt
#घोषणा Curl_ssl_random(x, y, z) ((व्योम)(x), CURLE_NOT_BUILT_IN)
#घोषणा Curl_ssl_cert_status_request() FALSE
#घोषणा Curl_ssl_supports(a, b) FALSE
#घोषणा Curl_ssl_cfilter_add(a, b, c, d) CURLE_NOT_BUILT_IN
#घोषणा Curl_ssl_cfilter_remove(a, b, c) CURLE_OK
#घोषणा Curl_ssl_cf_get_easy_config(a, b) शून्य
#घोषणा Curl_ssl_cf_get_filter_config(a) शून्य
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_VTLS_H */
