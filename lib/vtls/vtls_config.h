#اگر_غیر_تعریف_شدہ سرخی_CURL_VTLS_CONFIG_H
#تعریف_کرو سرخی_CURL_VTLS_CONFIG_H
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

ساخت Curl_easy;
ساخت connectdata;
ساخت Curl_peer;

ساخت ssl_filter_config {
  حرف *CApath;          /* certificate directory (does not work on Windows) */
  حرف *CAfile;          /* certificate to verify peer against */
  حرف *issuercert;      /* optional issuer certificate filename */
  حرف *clientcert;
  حرف *cipher_list;     /* list of ciphers to use */
  حرف *cipher_list13;   /* list of TLS 1.3 cipher suites to use */
  حرف *signature_algorithms; /* list of signature algorithms to use */
  حرف *pinned_کلید;
  حرف *CRLfile;         /* CRL to check certificate revocation */
  حرف *cert_type;       /* format for certificate (default: PEM) */
  حرف *کلید;             /* private key filename */
  حرف *کلید_type;        /* format for private key (default: PEM) */
  حرف *کلید_passwd;      /* plain text private key password */
#اگر_تعریف_شدہ USE_ECH
  حرف *ech_config;      /* TLS ECH configuration, base64 string */
  حرف *ech_عوامی;      /* TLS ECH public DNS (SNI in outer ClientHello) */
#اگر_ختم
  ساخت curl_blob *cert_blob;
  ساخت curl_blob *ca_info_blob;
  ساخت curl_blob *issuercert_blob;
  ساخت curl_blob *کلید_blob;
  حرف *curves;          /* list of curves to use */
  uint32_t نسخہ_max; /* max supported version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
  uint8_t نسخہ_2;    /* what version the client wants to use */
#اگر_تعریف_شدہ USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#اگر_ختم
  ثنائی_ہندسہ(verifypeer);       /* set TRUE if this is desired */
  ثنائی_ہندسہ(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  ثنائی_ہندسہ(verifystatus);     /* set TRUE if certificate status must be checked */
  ثنائی_ہندسہ(native_ca_store);  /* use the native CA store of operating system */
  ثنائی_ہندسہ(cache_session);    /* cache session or not */
  ثنائی_ہندسہ(deep_copy);        /* members are deep copies, eg. owned here */
  ثنائی_ہندسہ(earlydata);        /* use TLS 1.3 early data */
  ثنائی_ہندسہ(auto_client_cert);   /* automatically locate and use a client
                              certificate for authentication (Schannel) */
  ثنائی_ہندسہ(enable_beast); /* allow this flaw for interoperability's sake */
  ثنائی_ہندسہ(no_partialchain); /* do not accept partial certificate chains */
  ثنائی_ہندسہ(no_revoke);    /* disable SSL certificate revocation checks */
  ثنائی_ہندسہ(revoke_بہترین_effort); /* ignore SSL revocation offline/missing revocation
                              list errors */
};

ساخت ssl_easy_config {
  طویل certverifyresult; /* result from the certificate verification */
  uint32_t نسخہ_max; /* max supported version the client wants to use */
  uint8_t نسخہ_2;    /* what version the client wants to use */
  uint8_t ssl_options;  /* the CURLOPT_SSL_OPTIONS bitmask */
#اگر_تعریف_شدہ USE_ECH
  uint8_t ech;           /* TLS ECH CURLECH_((DIS|EN)ABLE|GREASE) */
#اگر_ختم
  ثنائی_ہندسہ(verifypeer);       /* set TRUE if this is desired */
  ثنائی_ہندسہ(verifyhost);       /* set TRUE if CN/SAN must match hostname */
  ثنائی_ہندسہ(verifystatus);     /* set TRUE if certificate status must be checked */
  ثنائی_ہندسہ(cache_session);    /* cache session or not */
  ثنائی_ہندسہ(certinfo);     /* gather lots of certificate info */
  ثنائی_ہندسہ(custom_cafile); /* application has set custom CA file */
  ثنائی_ہندسہ(custom_capath); /* application has set custom CA path */
  ثنائی_ہندسہ(custom_cablob); /* application has set custom CA blob */
};

خالی Curl_ssl_config_init(ساخت ssl_easy_config *sslc);
خالی Curl_ssl_config_cleanup(ساخت ssl_filter_config *sslc);

/* Init the SSL filter configs *shallow* for connection matching
 * with origin and data's configuration settings. */
CURLcode Curl_ssl_filter_config_tmp_init(
  ساخت Curl_easy *ڈیٹا, ساخت Curl_peer *origin,
  ساخت ssl_filter_config *ssl_origin,
  ساخت ssl_filter_config *ssl_proxy);

/* Clone the given filter configs into the connection. */
CURLcode Curl_ssl_conn_config_clone(ساخت ssl_filter_config *ssl_config,
                                    ساخت ssl_filter_config *proxy_ssl_config,
                                    ساخت connectdata *conn);

/**
 * Free allocated resources in SSL configs (main + proxy) for
 * the given connection.
 */
خالی Curl_ssl_conn_config_cleanup(ساخت connectdata *conn);

/**
 * Return TRUE if `conn_config` is functionally the
 * same as the one on `candidate`.
 * @param proxy   match the proxy SSL config or the main one
 */
bool Curl_ssl_conn_config_مطابقت(ساخت Curl_easy *ڈیٹا,
                                ساخت ssl_filter_config *conn_config,
                                ساخت connectdata *candidate,
                                bool proxy);

/* Update certain connection SSL config flags after they have
 * been changed on the easy handle. Works for `verifypeer`,
 * `verifyhost` and `verifystatus`. */
خالی Curl_ssl_conn_config_update(ساخت Curl_easy *ڈیٹا, bool for_proxy);

#اگر_ختم /* HEADER_CURL_VTLS_CONFIG_H */
