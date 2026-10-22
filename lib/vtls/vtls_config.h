#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_VTLS_CONFIG_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_VTLS_CONFIG_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

ರಚನೆ Curl_easy;
ರಚನೆ connectdata;
ರಚನೆ Curl_peer;

ರಚನೆ ssl_filter_config {
  ಅಕ್ಷರ *CApath;          /* certificate directory (does not work on Windows) */
  ಅಕ್ಷರ *CAfile;          /* certificate to verify peer against */
  ಅಕ್ಷರ *issuercert;      /* optional issuer certificate filename */
  ಅಕ್ಷರ *clientcert;
  ಅಕ್ಷರ *cipher_list;     /* list of ciphers to use */
  ಅಕ್ಷರ *cipher_list13;   /* list of TLS 1.3 cipher suites to use */
  ಅಕ್ಷರ *signature_algorithms; /* list of signature algorithms to use */
  ಅಕ್ಷರ *pinned_ಕೀಲಿ;
  ಅಕ್ಷರ *CRLfile;         /* CRL to check certificate revocation */
  ಅಕ್ಷರ *cert_type;       /* format for certificate (default: PEM) */
  ಅಕ್ಷರ *ಕೀಲಿ;             /* private key filename */
  ಅಕ್ಷರ *ಕೀಲಿ_type;        /* format for private key (default: PEM) */
  ಅಕ್ಷರ *ಕೀಲಿ_passwd;      /* plain text private key password */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
  ಅಕ್ಷರ *ech_config;      /* TLS ECH configuration, base64 string */
  ಅಕ್ಷರ *ech_ಬಹಿರಂಗ;      /* TLS ECH public DNS (SNI in outer ClientHello) */
#ಆದರೆ_ಅಂತ್ಯ
  ರಚನೆ curl_blob *cert_blob;
  ರಚನೆ curl_blob *ca_info_blob;
  ರಚನೆ curl_blob *issuercert_blob;
  ರಚನೆ curl_blob *ಕೀಲಿ_blob;
  ಅಕ್ಷರ *curves;          /* list of curves to use */
  uint32_t ಆವೃತ್ತಿ_max; /* max supported version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
  uint8_t ಆವೃತ್ತಿ_2;    /* what version the client wants to use */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(verifypeer);       /* set TRUE if this is desired */
  ಬಿಟ್(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  ಬಿಟ್(verifystatus);     /* set TRUE if certificate status must be checked */
  ಬಿಟ್(native_ca_store);  /* use the native CA store of operating system */
  ಬಿಟ್(cache_session);    /* cache session or not */
  ಬಿಟ್(deep_copy);        /* members are deep copies, eg. owned here */
  ಬಿಟ್(earlydata);        /* use TLS 1.3 early data */
  ಬಿಟ್(auto_client_cert);   /* automatically locate and use a client
                              certificate for authentication (Schannel) */
  ಬಿಟ್(enable_beast); /* allow this flaw for interoperability's sake */
  ಬಿಟ್(no_partialchain); /* do not accept partial certificate chains */
  ಬಿಟ್(no_revoke);    /* disable SSL certificate revocation checks */
  ಬಿಟ್(revoke_ಉತ್ತಮ_effort); /* ignore SSL revocation offline/missing revocation
                              list errors */
};

ರಚನೆ ssl_easy_config {
  ದೀರ್ಘ certverifyresult; /* result from the certificate verification */
  uint32_t ಆವೃತ್ತಿ_max; /* max supported version the client wants to use */
  uint8_t ಆವೃತ್ತಿ_2;    /* what version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#ಆದರೆ_ಅಂತ್ಯ
  ಬಿಟ್(verifypeer);       /* set TRUE if this is desired */
  ಬಿಟ್(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  ಬಿಟ್(verifystatus);     /* set TRUE if certificate status must be checked */
  ಬಿಟ್(cache_session);    /* cache session or not */
  ಬಿಟ್(certinfo);     /* gather lots of certificate info */
  ಬಿಟ್(custom_cafile); /* application has set custom CA file */
  ಬಿಟ್(custom_capath); /* application has set custom CA path */
  ಬಿಟ್(custom_cablob); /* application has set custom CA blob */
};

ಶೂನ್ಯ Curl_ssl_config_init(ರಚನೆ ssl_easy_config *sslc);
ಶೂನ್ಯ Curl_ssl_config_cleanup(ರಚನೆ ssl_filter_config *sslc);

/* Init the SSL filter configs *shallow* for connection matching
 * with origin and data's configuration settings. */
CURLcode Curl_ssl_filter_config_tmp_init(
  ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ Curl_peer *origin,
  ರಚನೆ ssl_filter_config *ssl_origin,
  ರಚನೆ ssl_filter_config *ssl_proxy);

/* Clone the given filter configs into the connection. */
CURLcode Curl_ssl_conn_config_clone(ರಚನೆ ssl_filter_config *ssl_config,
                                    ರಚನೆ ssl_filter_config *proxy_ssl_config,
                                    ರಚನೆ connectdata *conn);

/**
 * Free allocated resources in SSL configs (main + proxy) for
 * the given connection.
 */
ಶೂನ್ಯ Curl_ssl_conn_config_cleanup(ರಚನೆ connectdata *conn);

/**
 * Return TRUE if `conn_config` is functionally the
 * same as the one on `candidate`.
 * @param proxy   match the proxy SSL config or the main one
 */
bool Curl_ssl_conn_config_ಹೊಂದಿಕೆ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                ರಚನೆ ssl_filter_config *conn_config,
                                ರಚನೆ connectdata *candidate,
                                bool proxy);

/* Update certain connection SSL config flags after they have
 * been changed on the easy handle. Works for `verifypeer`,
 * `verifyhost` and `verifystatus`. */
ಶೂನ್ಯ Curl_ssl_conn_config_update(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool for_proxy);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VTLS_CONFIG_H */
