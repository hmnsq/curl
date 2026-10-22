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
#અન્તર્ભાવ "curl_setup.h"

સંરચના Curl_easy;
સંરચના connectdata;
સંરચના Curl_peer;

સંરચના ssl_filter_config {
  અક્ષર *CApath;          /* certificate directory (does not work on Windows) */
  અક્ષર *CAfile;          /* certificate to verify peer against */
  અક્ષર *issuercert;      /* optional issuer certificate filename */
  અક્ષર *clientcert;
  અક્ષર *cipher_list;     /* list of ciphers to use */
  અક્ષર *cipher_list13;   /* list of TLS 1.3 cipher suites to use */
  અક્ષર *signature_algorithms; /* list of signature algorithms to use */
  અક્ષર *pinned_ચાવી;
  અક્ષર *CRLfile;         /* CRL to check certificate revocation */
  અક્ષર *cert_type;       /* format for certificate (default: PEM) */
  અક્ષર *ચાવી;             /* private key filename */
  અક્ષર *ચાવી_type;        /* format for private key (default: PEM) */
  અક્ષર *ચાવી_passwd;      /* plain text private key password */
#ifdef USE_ECH
  અક્ષર *ech_config;      /* TLS ECH configuration, base64 string */
  અક્ષર *ech_public;      /* TLS ECH public DNS (SNI in outer ClientHello) */
#endif
  સંરચના curl_blob *cert_blob;
  સંરચના curl_blob *ca_info_blob;
  સંરચના curl_blob *issuercert_blob;
  સંરચના curl_blob *ચાવી_blob;
  અક્ષર *curves;          /* list of curves to use */
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

સંરચના ssl_easy_config {
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

શૂન્ય Curl_ssl_config_init(સંરચના ssl_easy_config *sslc);
શૂન્ય Curl_ssl_config_cleanup(સંરચના ssl_filter_config *sslc);

/* Init the SSL filter configs *shallow* for connection matching
 * with origin and data's configuration settings. */
CURLcode Curl_ssl_filter_config_tmp_init(
  સંરચના Curl_easy *data, સંરચના Curl_peer *origin,
  સંરચના ssl_filter_config *ssl_origin,
  સંરચના ssl_filter_config *ssl_proxy);

/* Clone the given filter configs into the connection. */
CURLcode Curl_ssl_conn_config_clone(સંરચના ssl_filter_config *ssl_config,
                                    સંરચના ssl_filter_config *proxy_ssl_config,
                                    સંરચના connectdata *conn);

/**
 * Free allocated resources in SSL configs (main + proxy) for
 * the given connection.
 */
શૂન્ય Curl_ssl_conn_config_cleanup(સંરચના connectdata *conn);

/**
 * Return TRUE if `conn_config` is functionally the
 * same as the one on `candidate`.
 * @param proxy   match the proxy SSL config or the main one
 */
bool Curl_ssl_conn_config_match(સંરચના Curl_easy *data,
                                સંરચના ssl_filter_config *conn_config,
                                સંરચના connectdata *candidate,
                                bool proxy);

/* Update certain connection SSL config flags after they have
 * been changed on the easy handle. Works for `verifypeer`,
 * `verifyhost` and `verifystatus`. */
શૂન્ય Curl_ssl_conn_config_update(સંરચના Curl_easy *data, bool for_proxy);

#endif /* HEADER_CURL_VTLS_CONFIG_H */
