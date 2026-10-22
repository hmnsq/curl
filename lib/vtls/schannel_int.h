#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_SCHANNEL_INT_H
#وصف_ڪريو سرو_CURL_SCHANNEL_INT_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Marc Hoersken, <info@marc-hoersken.de>, et al.
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

#جيڪڏهن_وصف_ٿيل USE_SCHANNEL

#ضمن "vtls/vtls.h"
#ضمن "curl_sha256.h"

#إذا defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'LPCSTR' of greater size */
#وصف_رد CERT_STORE_PROV_يادگيري
#وصف_رد CERT_STORE_PROV_SYSTEM_A
#وصف_رد CERT_STORE_PROV_SYSTEM_W
#وصف_ڪريو CERT_STORE_PROV_يادگيري    ((LPCSTR)(size_t)2)
#وصف_ڪريو CERT_STORE_PROV_SYSTEM_A  ((LPCSTR)(size_t)9)
#وصف_ڪريو CERT_STORE_PROV_SYSTEM_W  ((LPCSTR)(size_t)10)
#جيڪڏهن_پورو

/* Offered by mingw-w64 v8+, MS SDK 10.0.17763.0/VS2017 15.8+ */
#جيڪڏهن_وصف_نه_ٿيل SCH_CREDENTIALS_نسخو
#وصف_ڪريو SCH_CREDENTIALS_نسخو  0x00000005

تعريفنوع تعداد _eTlsAlgorithmUsage {
  Tls_Parameters_Cng_Alg_Usage_ڪنجي_Exchange,
  TlsParametersCngAlgUsageSignature,
  TlsParametersCngAlgUsageCipher,
  TlsParametersCngAlgUsageDigest,
  TlsParametersCngAlgUsageCertSig
} eTlsAlgorithmUsage;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية _CRYPTO_SETTINGS {
  eTlsAlgorithmUsage eAlgorithmUsage;
  UNICODE_STRING strCngAlgId;
  DWORD cChainingModes;
  PUNICODE_STRING rgstrChainingModes; /* spellchecker:disable-line */
  DWORD dw_Min_ٻٽو_انگ_ڊيگهائي;
  DWORD dw_Max_ٻٽو_انگ_ڊيگهائي;
} CRYPTO_SETTINGS, *PCRYPTO_SETTINGS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية _TLS_PARAMETERS {
  DWORD cAlpnIds;
  PUNICODE_STRING rgstrAlpnIds; /* spellchecker:disable-line */
  DWORD grbitDisabledProtocols;
  DWORD cDisabledCrypto;
  PCRYPTO_SETTINGS pDisabledCrypto;
  DWORD dwFlags;
} TLS_PARAMETERS, *PTLS_PARAMETERS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
تعريفنوع بنية _SCH_CREDENTIALS {
  DWORD dw_نسخو;
  DWORD dwCredFormat;
  DWORD cCreds;
  PCCERT_CONTEXT *paCred;
  HCERTSTORE h_پاڙ_Store;

  DWORD cMappers;
  بنية _HMAPPER **aphMappers;

  DWORD dwSessionLifespan;
  DWORD dwFlags;
  DWORD cTlsParameters;
  PTLS_PARAMETERS pTlsParameters;
} SCH_CREDENTIALS, *PSCH_CREDENTIALS;

#جيڪڏهن_پورو /* !SCH_CREDENTIALS_VERSION */

بنية Curl_schannel_cred {
  CredHandle cred_handle;
  TCHAR *sni_hostname;
  HCERTSTORE client_cert_store;
  صحيح refcount;
};

بنية Curl_schannel_ctxt {
  CtxtHandle ctxt_handle;
};

/* handle encoding/decoding buffers */
بنية sbuffer {
  size_t ڊيگهائي;
  size_t وٿي;
  بنانشان حرف *بفر;
};

بنية schannel_ssl_backend_معلومات {
  بنانشان حرف *send_بفر;
  size_t send_بفر_len;
  بنية sbuffer encdata;
  بنية sbuffer decdata;
  بنية Curl_schannel_cred *cred;
  بنية Curl_schannel_ctxt *ctxt;
  Sec_بفر shutdown_token;
  size_t shutdown_وٿي;
  Sec_Pkg_Context_وهڪرو_Sizes وهڪرو_sizes;
  بنانشان ڊگهو req_flags, ret_flags;
  CURLcode recv_unrecoverable_err; /* schannel_recv had an unrecoverable err */
  بنية schannel_renegotiate_state {
    bool started;
    بنية curltime شروعات_وقت;
    صحيح io_need;
  } renegotiate_state;
  ٻٽو_انگ(recv_sspi_close_notify); /* true if connection closed by close_notify */
  ٻٽو_انگ(recv_connection_closed); /* true if connection closed, regardless how */
  ٻٽو_انگ(recv_renegotiating);     /* true if recv is doing renegotiation */
  ٻٽو_انگ(use_alpn); /* true if ALPN is used for this connection */
  ٻٽو_انگ(use_manual_cred_validation); /* true if manual cred validation is used */
  ٻٽو_انگ(موڪليل_shutdown);
  ٻٽو_انگ(shutdown_generated);
  /* encdata_is_incomplete: if encdata contains only a partial record that
     cannot be decrypted without another recv() (that is, status is
     SEC_E_INCOMPLETE_MESSAGE) then set this true. after recv() adds more
     bytes into encdata then set this back to false. */
  ٻٽو_انگ(encdata_is_incomplete);
};

بنية schannel_cert_share {
  بنانشان حرف CAinfo_blob_digest[CURL_SHA256_DIGEST_ڊيگهائي];
  size_t CAinfo_blob_سائيز;           /* CA info blob size */
  حرف *CAfile;                      /* CAfile path used to generate
                                        certificate store */
  HCERTSTORE cert_store;             /* cached certificate store or
                                        NULL if none */
  بنية curltime وقت;              /* when the cached store was created */
};

/* size of the structure: 20 bytes */
بنية انگ_ip_معلومات {
  DWORD سائيز; /* 04 bytes */
  اتحاد {
    بنية in_addr  ia;  /* 04 bytes */
    بنية in6_addr ia6; /* 16 bytes */
  } b_معلومات;
};

HCERTSTORE Curl_schannel_get_cached_cert_store(بنية Curl_cfilter *cf,
                                               بنية Curl_easy *معلومات);

bool Curl_schannel_سيٽ_cached_cert_store(بنية Curl_cfilter *cf,
                                         بنية Curl_easy *معلومات,
                                         HCERTSTORE cert_store);

#جيڪڏهن_پورو /* USE_SCHANNEL */
#جيڪڏهن_پورو /* HEADER_CURL_SCHANNEL_INT_H */
