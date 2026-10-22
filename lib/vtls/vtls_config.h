#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_VTLS_CONFIG_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_VTLS_CONFIG_H
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

ਬਣਤਰ Curl_easy;
ਬਣਤਰ connectdata;
ਬਣਤਰ Curl_peer;

ਬਣਤਰ ssl_filter_config {
  ਅੱਖਰ *CApath;          /* certificate directory (does not work on Windows) */
  ਅੱਖਰ *CAfile;          /* certificate to verify peer against */
  ਅੱਖਰ *issuercert;      /* optional issuer certificate filename */
  ਅੱਖਰ *clientcert;
  ਅੱਖਰ *cipher_list;     /* list of ciphers to use */
  ਅੱਖਰ *cipher_list13;   /* list of TLS 1.3 cipher suites to use */
  ਅੱਖਰ *signature_algorithms; /* list of signature algorithms to use */
  ਅੱਖਰ *pinned_ਕੁੰਜੀ;
  ਅੱਖਰ *CRLfile;         /* CRL to check certificate revocation */
  ਅੱਖਰ *cert_type;       /* format for certificate (default: PEM) */
  ਅੱਖਰ *ਕੁੰਜੀ;             /* private key filename */
  ਅੱਖਰ *ਕੁੰਜੀ_type;        /* format for private key (default: PEM) */
  ਅੱਖਰ *ਕੁੰਜੀ_passwd;      /* plain text private key password */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
  ਅੱਖਰ *ech_config;      /* TLS ECH configuration, base64 string */
  ਅੱਖਰ *ech_ਜਨਤਕ;      /* TLS ECH public DNS (SNI in outer ClientHello) */
#ਜੇ_ਅੰਤ
  ਬਣਤਰ curl_blob *cert_blob;
  ਬਣਤਰ curl_blob *ca_info_blob;
  ਬਣਤਰ curl_blob *issuercert_blob;
  ਬਣਤਰ curl_blob *ਕੁੰਜੀ_blob;
  ਅੱਖਰ *curves;          /* list of curves to use */
  uint32_t ਸੰਸਕਰਣ_max; /* max supported version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
  uint8_t ਸੰਸਕਰਣ_2;    /* what version the client wants to use */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(verifypeer);       /* set TRUE if this is desired */
  ਦੋਆਧਾਰੀ_ਅੰਕ(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  ਦੋਆਧਾਰੀ_ਅੰਕ(verifystatus);     /* set TRUE if certificate status must be checked */
  ਦੋਆਧਾਰੀ_ਅੰਕ(native_ca_store);  /* use the native CA store of operating system */
  ਦੋਆਧਾਰੀ_ਅੰਕ(cache_session);    /* cache session or not */
  ਦੋਆਧਾਰੀ_ਅੰਕ(deep_copy);        /* members are deep copies, eg. owned here */
  ਦੋਆਧਾਰੀ_ਅੰਕ(earlydata);        /* use TLS 1.3 early data */
  ਦੋਆਧਾਰੀ_ਅੰਕ(auto_client_cert);   /* automatically locate and use a client
                              certificate for authentication (Schannel) */
  ਦੋਆਧਾਰੀ_ਅੰਕ(enable_beast); /* allow this flaw for interoperability's sake */
  ਦੋਆਧਾਰੀ_ਅੰਕ(no_partialchain); /* do not accept partial certificate chains */
  ਦੋਆਧਾਰੀ_ਅੰਕ(no_revoke);    /* disable SSL certificate revocation checks */
  ਦੋਆਧਾਰੀ_ਅੰਕ(revoke_ਉੱਤਮ_effort); /* ignore SSL revocation offline/missing revocation
                              list errors */
};

ਬਣਤਰ ssl_easy_config {
  ਲੰਮਾ certverifyresult; /* result from the certificate verification */
  uint32_t ਸੰਸਕਰਣ_max; /* max supported version the client wants to use */
  uint8_t ਸੰਸਕਰਣ_2;    /* what version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#ਜੇ_ਅੰਤ
  ਦੋਆਧਾਰੀ_ਅੰਕ(verifypeer);       /* set TRUE if this is desired */
  ਦੋਆਧਾਰੀ_ਅੰਕ(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  ਦੋਆਧਾਰੀ_ਅੰਕ(verifystatus);     /* set TRUE if certificate status must be checked */
  ਦੋਆਧਾਰੀ_ਅੰਕ(cache_session);    /* cache session or not */
  ਦੋਆਧਾਰੀ_ਅੰਕ(certinfo);     /* gather lots of certificate info */
  ਦੋਆਧਾਰੀ_ਅੰਕ(custom_cafile); /* application has set custom CA file */
  ਦੋਆਧਾਰੀ_ਅੰਕ(custom_capath); /* application has set custom CA path */
  ਦੋਆਧਾਰੀ_ਅੰਕ(custom_cablob); /* application has set custom CA blob */
};

ਖਾਲੀ Curl_ssl_config_init(ਬਣਤਰ ssl_easy_config *sslc);
ਖਾਲੀ Curl_ssl_config_cleanup(ਬਣਤਰ ssl_filter_config *sslc);

/* Init the SSL filter configs *shallow* for connection matching
 * with origin and data's configuration settings. */
CURLcode Curl_ssl_filter_config_tmp_init(
  ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ Curl_peer *origin,
  ਬਣਤਰ ssl_filter_config *ssl_origin,
  ਬਣਤਰ ssl_filter_config *ssl_proxy);

/* Clone the given filter configs into the connection. */
CURLcode Curl_ssl_conn_config_clone(ਬਣਤਰ ssl_filter_config *ssl_config,
                                    ਬਣਤਰ ssl_filter_config *proxy_ssl_config,
                                    ਬਣਤਰ connectdata *conn);

/**
 * Free allocated resources in SSL configs (main + proxy) for
 * the given connection.
 */
ਖਾਲੀ Curl_ssl_conn_config_cleanup(ਬਣਤਰ connectdata *conn);

/**
 * Return TRUE if `conn_config` is functionally the
 * same as the one on `candidate`.
 * @param proxy   match the proxy SSL config or the main one
 */
bool Curl_ssl_conn_config_ਮੇਲ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                ਬਣਤਰ ssl_filter_config *conn_config,
                                ਬਣਤਰ connectdata *candidate,
                                bool proxy);

/* Update certain connection SSL config flags after they have
 * been changed on the easy handle. Works for `verifypeer`,
 * `verifyhost` and `verifystatus`. */
ਖਾਲੀ Curl_ssl_conn_config_update(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool for_proxy);

#ਜੇ_ਅੰਤ /* HEADER_CURL_VTLS_CONFIG_H */
