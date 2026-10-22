#నిర్వచితం_కాకపోతే శీర్షిక_CURL_VTLS_CONFIG_H
#నిర్వచించు శీర్షిక_CURL_VTLS_CONFIG_H
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

నిర్మాణం Curl_easy;
నిర్మాణం connectdata;
నిర్మాణం Curl_peer;

నిర్మాణం ssl_filter_config {
  అక్షరం *CApath;          /* certificate directory (does not work on Windows) */
  అక్షరం *CAfile;          /* certificate to verify peer against */
  అక్షరం *issuercert;      /* optional issuer certificate filename */
  అక్షరం *clientcert;
  అక్షరం *cipher_list;     /* list of ciphers to use */
  అక్షరం *cipher_list13;   /* list of TLS 1.3 cipher suites to use */
  అక్షరం *signature_algorithms; /* list of signature algorithms to use */
  అక్షరం *pinned_కీలకం;
  అక్షరం *CRLfile;         /* CRL to check certificate revocation */
  అక్షరం *cert_type;       /* format for certificate (default: PEM) */
  అక్షరం *కీలకం;             /* private key filename */
  అక్షరం *కీలకం_type;        /* format for private key (default: PEM) */
  అక్షరం *కీలకం_passwd;      /* plain text private key password */
#నిర్వచితమైతే USE_ECH
  అక్షరం *ech_config;      /* TLS ECH configuration, base64 string */
  అక్షరం *ech_ప్రజా;      /* TLS ECH public DNS (SNI in outer ClientHello) */
#అయితే_ముగింపు
  నిర్మాణం curl_blob *cert_blob;
  నిర్మాణం curl_blob *ca_info_blob;
  నిర్మాణం curl_blob *issuercert_blob;
  నిర్మాణం curl_blob *కీలకం_blob;
  అక్షరం *curves;          /* list of curves to use */
  uint32_t సంస్కరణ_max; /* max supported version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
  uint8_t సంస్కరణ_2;    /* what version the client wants to use */
#నిర్వచితమైతే USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#అయితే_ముగింపు
  బిట్టు(verifypeer);       /* set TRUE if this is desired */
  బిట్టు(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  బిట్టు(verifystatus);     /* set TRUE if certificate status must be checked */
  బిట్టు(native_ca_store);  /* use the native CA store of operating system */
  బిట్టు(cache_session);    /* cache session or not */
  బిట్టు(deep_copy);        /* members are deep copies, eg. owned here */
  బిట్టు(earlydata);        /* use TLS 1.3 early data */
  బిట్టు(auto_client_cert);   /* automatically locate and use a client
                              certificate for authentication (Schannel) */
  బిట్టు(enable_beast); /* allow this flaw for interoperability's sake */
  బిట్టు(no_partialchain); /* do not accept partial certificate chains */
  బిట్టు(no_revoke);    /* disable SSL certificate revocation checks */
  బిట్టు(revoke_ఉత్తమ_effort); /* ignore SSL revocation offline/missing revocation
                              list errors */
};

నిర్మాణం ssl_easy_config {
  దీర్ఘ certverifyresult; /* result from the certificate verification */
  uint32_t సంస్కరణ_max; /* max supported version the client wants to use */
  uint8_t సంస్కరణ_2;    /* what version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
#నిర్వచితమైతే USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#అయితే_ముగింపు
  బిట్టు(verifypeer);       /* set TRUE if this is desired */
  బిట్టు(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  బిట్టు(verifystatus);     /* set TRUE if certificate status must be checked */
  బిట్టు(cache_session);    /* cache session or not */
  బిట్టు(certinfo);     /* gather lots of certificate info */
  బిట్టు(custom_cafile); /* application has set custom CA file */
  బిట్టు(custom_capath); /* application has set custom CA path */
  బిట్టు(custom_cablob); /* application has set custom CA blob */
};

శూన్య Curl_ssl_config_init(నిర్మాణం ssl_easy_config *sslc);
శూన్య Curl_ssl_config_cleanup(నిర్మాణం ssl_filter_config *sslc);

/* Init the SSL filter configs *shallow* for connection matching
 * with origin and data's configuration settings. */
CURLcode Curl_ssl_filter_config_tmp_init(
  నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం Curl_peer *origin,
  నిర్మాణం ssl_filter_config *ssl_origin,
  నిర్మాణం ssl_filter_config *ssl_proxy);

/* Clone the given filter configs into the connection. */
CURLcode Curl_ssl_conn_config_clone(నిర్మాణం ssl_filter_config *ssl_config,
                                    నిర్మాణం ssl_filter_config *proxy_ssl_config,
                                    నిర్మాణం connectdata *conn);

/**
 * Free allocated resources in SSL configs (main + proxy) for
 * the given connection.
 */
శూన్య Curl_ssl_conn_config_cleanup(నిర్మాణం connectdata *conn);

/**
 * Return TRUE if `conn_config` is functionally the
 * same as the one on `candidate`.
 * @param proxy   match the proxy SSL config or the main one
 */
bool Curl_ssl_conn_config_సరిపోలిక(నిర్మాణం Curl_easy *దత్తాంశం,
                                నిర్మాణం ssl_filter_config *conn_config,
                                నిర్మాణం connectdata *candidate,
                                bool proxy);

/* Update certain connection SSL config flags after they have
 * been changed on the easy handle. Works for `verifypeer`,
 * `verifyhost` and `verifystatus`. */
శూన్య Curl_ssl_conn_config_update(నిర్మాణం Curl_easy *దత్తాంశం, bool for_proxy);

#అయితే_ముగింపు /* HEADER_CURL_VTLS_CONFIG_H */
