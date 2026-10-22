#अगर_अघोषित HEADER_CURL_VTLS_CONFIG_H
#घोषणा HEADER_CURL_VTLS_CONFIG_H
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

काष्ठा Curl_easy;
काष्ठा connectdata;
काष्ठा Curl_peer;

काष्ठा ssl_filter_config {
  अक्षर *CApath;          /* certificate directory (does not work on Windows) */
  अक्षर *CAfile;          /* certificate to verify peer against */
  अक्षर *issuercert;      /* optional issuer certificate filename */
  अक्षर *clientcert;
  अक्षर *cipher_list;     /* list of ciphers to use */
  अक्षर *cipher_list13;   /* list of TLS 1.3 cipher suites to use */
  अक्षर *signature_algorithms; /* list of signature algorithms to use */
  अक्षर *pinned_कुंजी;
  अक्षर *CRLfile;         /* CRL to check certificate revocation */
  अक्षर *cert_type;       /* format for certificate (default: PEM) */
  अक्षर *कुंजी;             /* private key filename */
  अक्षर *कुंजी_type;        /* format for private key (default: PEM) */
  अक्षर *कुंजी_passwd;      /* plain text private key password */
#अगर_घोषित USE_ECH
  अक्षर *ech_config;      /* TLS ECH configuration, base64 string */
  अक्षर *ech_public;      /* TLS ECH public DNS (SNI in outer ClientHello) */
#पूर्ण_अगर
  काष्ठा curl_blob *cert_blob;
  काष्ठा curl_blob *ca_info_blob;
  काष्ठा curl_blob *issuercert_blob;
  काष्ठा curl_blob *कुंजी_blob;
  अक्षर *curves;          /* list of curves to use */
  uint32_t version_max; /* max supported version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
  uint8_t version;    /* what version the client wants to use */
#अगर_घोषित USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#पूर्ण_अगर
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

काष्ठा ssl_easy_config {
  दीर्घ certverifyresult; /* result from the certificate verification */
  uint32_t version_max; /* max supported version the client wants to use */
  uint8_t version;    /* what version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
#अगर_घोषित USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#पूर्ण_अगर
  BIT(verifypeer);       /* set TRUE if this is desired */
  BIT(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  BIT(verifystatus);     /* set TRUE if certificate status must be checked */
  BIT(cache_session);    /* cache session or not */
  BIT(certinfo);     /* gather lots of certificate info */
  BIT(custom_cafile); /* application has set custom CA file */
  BIT(custom_capath); /* application has set custom CA path */
  BIT(custom_cablob); /* application has set custom CA blob */
};

व्योम Curl_ssl_config_init(काष्ठा ssl_easy_config *sslc);
व्योम Curl_ssl_config_cleanup(काष्ठा ssl_filter_config *sslc);

/* Init the SSL filter configs *shallow* for connection matching
 * with origin and data's configuration settings. */
CURLcode Curl_ssl_filter_config_tmp_init(
  काष्ठा Curl_easy *तथ्य, काष्ठा Curl_peer *origin,
  काष्ठा ssl_filter_config *ssl_origin,
  काष्ठा ssl_filter_config *ssl_proxy);

/* Clone the given filter configs into the connection. */
CURLcode Curl_ssl_conn_config_clone(काष्ठा ssl_filter_config *ssl_config,
                                    काष्ठा ssl_filter_config *proxy_ssl_config,
                                    काष्ठा connectdata *conn);

/**
 * Free allocated resources in SSL configs (main + proxy) for
 * the given connection.
 */
व्योम Curl_ssl_conn_config_cleanup(काष्ठा connectdata *conn);

/**
 * Return TRUE if `conn_config` is functionally the
 * same as the one on `candidate`.
 * @param proxy   match the proxy SSL config or the main one
 */
bool Curl_ssl_conn_config_match(काष्ठा Curl_easy *तथ्य,
                                काष्ठा ssl_filter_config *conn_config,
                                काष्ठा connectdata *candidate,
                                bool proxy);

/* Update certain connection SSL config flags after they have
 * been changed on the easy handle. Works for `verifypeer`,
 * `verifyhost` and `verifystatus`. */
व्योम Curl_ssl_conn_config_update(काष्ठा Curl_easy *तथ्य, bool for_proxy);

#पूर्ण_अगर /* HEADER_CURL_VTLS_CONFIG_H */
