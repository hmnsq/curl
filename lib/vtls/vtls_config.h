#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_VTLS_CONFIG_H
#وصف_ڪريو سرو_CURL_VTLS_CONFIG_H
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

بنية Curl_easy;
بنية connectdata;
بنية Curl_peer;

بنية ssl_filter_config {
  حرف *CApath;          /* certificate directory (does not work on Windows) */
  حرف *CAfile;          /* certificate to verify peer against */
  حرف *issuercert;      /* optional issuer certificate filename */
  حرف *clientcert;
  حرف *cipher_list;     /* list of ciphers to use */
  حرف *cipher_list13;   /* list of TLS 1.3 cipher suites to use */
  حرف *signature_algorithms; /* list of signature algorithms to use */
  حرف *pinned_ڪنجي;
  حرف *CRLfile;         /* CRL to check certificate revocation */
  حرف *cert_type;       /* format for certificate (default: PEM) */
  حرف *ڪنجي;             /* private key filename */
  حرف *ڪنجي_type;        /* format for private key (default: PEM) */
  حرف *ڪنجي_passwd;      /* plain text private key password */
#جيڪڏهن_وصف_ٿيل USE_ECH
  حرف *ech_config;      /* TLS ECH configuration, base64 string */
  حرف *ech_عوامي;      /* TLS ECH public DNS (SNI in outer ClientHello) */
#جيڪڏهن_پورو
  بنية curl_blob *cert_blob;
  بنية curl_blob *ca_info_blob;
  بنية curl_blob *issuercert_blob;
  بنية curl_blob *ڪنجي_blob;
  حرف *curves;          /* list of curves to use */
  uint32_t نسخو_max; /* max supported version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
  uint8_t نسخو_2;    /* what version the client wants to use */
#جيڪڏهن_وصف_ٿيل USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#جيڪڏهن_پورو
  ٻٽو_انگ(verifypeer);       /* set TRUE if this is desired */
  ٻٽو_انگ(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  ٻٽو_انگ(verifystatus);     /* set TRUE if certificate status must be checked */
  ٻٽو_انگ(native_ca_store);  /* use the native CA store of operating system */
  ٻٽو_انگ(cache_session);    /* cache session or not */
  ٻٽو_انگ(deep_copy);        /* members are deep copies, eg. owned here */
  ٻٽو_انگ(earlydata);        /* use TLS 1.3 early data */
  ٻٽو_انگ(auto_client_cert);   /* automatically locate and use a client
                              certificate for authentication (Schannel) */
  ٻٽو_انگ(enable_beast); /* allow this flaw for interoperability's sake */
  ٻٽو_انگ(no_partialchain); /* do not accept partial certificate chains */
  ٻٽو_انگ(no_revoke);    /* disable SSL certificate revocation checks */
  ٻٽو_انگ(revoke_بهترين_effort); /* ignore SSL revocation offline/missing revocation
                              list errors */
};

بنية ssl_easy_config {
  ڊگهو certverifyresult; /* result from the certificate verification */
  uint32_t نسخو_max; /* max supported version the client wants to use */
  uint8_t نسخو_2;    /* what version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
#جيڪڏهن_وصف_ٿيل USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#جيڪڏهن_پورو
  ٻٽو_انگ(verifypeer);       /* set TRUE if this is desired */
  ٻٽو_انگ(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  ٻٽو_انگ(verifystatus);     /* set TRUE if certificate status must be checked */
  ٻٽو_انگ(cache_session);    /* cache session or not */
  ٻٽو_انگ(certinfo);     /* gather lots of certificate info */
  ٻٽو_انگ(custom_cafile); /* application has set custom CA file */
  ٻٽو_انگ(custom_capath); /* application has set custom CA path */
  ٻٽو_انگ(custom_cablob); /* application has set custom CA blob */
};

فراغ Curl_ssl_config_init(بنية ssl_easy_config *sslc);
فراغ Curl_ssl_config_cleanup(بنية ssl_filter_config *sslc);

/* Init the SSL filter configs *shallow* for connection matching
 * with origin and data's configuration settings. */
CURLcode Curl_ssl_filter_config_tmp_init(
  بنية Curl_easy *معلومات, بنية Curl_peer *origin,
  بنية ssl_filter_config *ssl_origin,
  بنية ssl_filter_config *ssl_proxy);

/* Clone the given filter configs into the connection. */
CURLcode Curl_ssl_conn_config_clone(بنية ssl_filter_config *ssl_config,
                                    بنية ssl_filter_config *proxy_ssl_config,
                                    بنية connectdata *conn);

/**
 * Free allocated resources in SSL configs (main + proxy) for
 * the given connection.
 */
فراغ Curl_ssl_conn_config_cleanup(بنية connectdata *conn);

/**
 * Return TRUE if `conn_config` is functionally the
 * same as the one on `candidate`.
 * @param proxy   match the proxy SSL config or the main one
 */
bool Curl_ssl_conn_config_ميلاپ(بنية Curl_easy *معلومات,
                                بنية ssl_filter_config *conn_config,
                                بنية connectdata *candidate,
                                bool proxy);

/* Update certain connection SSL config flags after they have
 * been changed on the easy handle. Works for `verifypeer`,
 * `verifyhost` and `verifystatus`. */
فراغ Curl_ssl_conn_config_update(بنية Curl_easy *معلومات, bool for_proxy);

#جيڪڏهن_پورو /* HEADER_CURL_VTLS_CONFIG_H */
