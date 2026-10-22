#ifndef HEADER_CURL_VTLS_CONFIG_H
#define HEADER_CURL_VTLS_CONFIG_H
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
#شامل "curl_setup.h"

ساختار Curl_easy;
ساختار connectdata;
ساختار Curl_peer;

ساختار ssl_filter_config {
  نویسه *CApath;          /* certificate directory (does not work on Windows) */
  نویسه *CAfile;          /* certificate to verify peer against */
  نویسه *issuercert;      /* optional issuer certificate filename */
  نویسه *clientcert;
  نویسه *cipher_list;     /* list of ciphers to use */
  نویسه *cipher_list13;   /* list of TLS 1.3 cipher suites to use */
  نویسه *signature_algorithms; /* list of signature algorithms to use */
  نویسه *pinned_کلید;
  نویسه *CRLfile;         /* CRL to check certificate revocation */
  نویسه *cert_type;       /* format for certificate (default: PEM) */
  نویسه *کلید;             /* private key filename */
  نویسه *کلید_type;        /* format for private key (default: PEM) */
  نویسه *کلید_passwd;      /* plain text private key password */
#ifdef USE_ECH
  نویسه *ech_config;      /* TLS ECH configuration, base64 string */
  نویسه *ech_public;      /* TLS ECH public DNS (SNI in outer ClientHello) */
#endif
  ساختار curl_blob *cert_blob;
  ساختار curl_blob *ca_info_blob;
  ساختار curl_blob *issuercert_blob;
  ساختار curl_blob *کلید_blob;
  نویسه *curves;          /* list of curves to use */
  uint32_t version_max; /* max supported version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
  uint8_t version;    /* what version the client wants to use */
#ifdef USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#endif
  BIT(verifypeer);       /* set TRUE if this is desired */
  BIT(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  BIT(verifystatus);     /* set TRUE if certificate status must be checked */
  BIT(native_ca_store);  /* use the native CA store of operating system */
  BIT(cache_session);    /* cache session or not */
  BIT(deep_copy);        /* members are deep copies, eg. owned here */
  BIT(earlydata);        /* use TLS 1.3 early data */
  BIT(auto_client_cert);   /* automatically locate and use a client
                              certificate for authentication (Schannel) */
  BIT(enable_beast); /* allow this flaw for interoperability's sake */
  BIT(no_partialchain); /* do not accept partial certificate chains */
  BIT(no_revoke);    /* disable SSL certificate revocation checks */
  BIT(revoke_best_effort); /* ignore SSL revocation offline/missing revocation
                              list errors */
};

ساختار ssl_easy_config {
  long certverifyresult; /* result from the certificate verification */
  uint32_t version_max; /* max supported version the client wants to use */
  uint8_t version;    /* what version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
#ifdef USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#endif
  BIT(verifypeer);       /* set TRUE if this is desired */
  BIT(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  BIT(verifystatus);     /* set TRUE if certificate status must be checked */
  BIT(cache_session);    /* cache session or not */
  BIT(certinfo);     /* gather lots of certificate info */
  BIT(custom_cafile); /* application has set custom CA file */
  BIT(custom_capath); /* application has set custom CA path */
  BIT(custom_cablob); /* application has set custom CA blob */
};

تهی Curl_ssl_config_init(ساختار ssl_easy_config *sslc);
تهی Curl_ssl_config_cleanup(ساختار ssl_filter_config *sslc);

/* Init the SSL filter configs *shallow* for connection matching
 * with origin and data's configuration settings. */
CURLcode Curl_ssl_filter_config_tmp_init(
  ساختار Curl_easy *data, ساختار Curl_peer *origin,
  ساختار ssl_filter_config *ssl_origin,
  ساختار ssl_filter_config *ssl_proxy);

/* Clone the given filter configs into the connection. */
CURLcode Curl_ssl_conn_config_clone(ساختار ssl_filter_config *ssl_config,
                                    ساختار ssl_filter_config *proxy_ssl_config,
                                    ساختار connectdata *conn);

/**
 * Free allocated resources in SSL configs (main + proxy) for
 * the given connection.
 */
تهی Curl_ssl_conn_config_cleanup(ساختار connectdata *conn);

/**
 * Return TRUE if `conn_config` is functionally the
 * same as the one on `candidate`.
 * @param proxy   match the proxy SSL config or the main one
 */
bool Curl_ssl_conn_config_match(ساختار Curl_easy *data,
                                ساختار ssl_filter_config *conn_config,
                                ساختار connectdata *candidate,
                                bool proxy);

/* Update certain connection SSL config flags after they have
 * been changed on the easy handle. Works for `verifypeer`,
 * `verifyhost` and `verifystatus`. */
تهی Curl_ssl_conn_config_update(ساختار Curl_easy *data, bool for_proxy);

#endif /* HEADER_CURL_VTLS_CONFIG_H */
