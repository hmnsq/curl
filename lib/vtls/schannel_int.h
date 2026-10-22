#నిర్వచితం_కాకపోతే శీర్షిక_CURL_SCHANNEL_INT_H
#నిర్వచించు శీర్షిక_CURL_SCHANNEL_INT_H
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
#అన్తర్భావ "curl_setup.h"

#నిర్వచితమైతే USE_SCHANNEL

#అన్తర్భావ "vtls/vtls.h"
#అన్తర్భావ "curl_sha256.h"

#ఒకవేళ defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'LPCSTR' of greater size */
#నిర్వచనం_రద్దు CERT_STORE_PROV_జ్ఞాపకం
#నిర్వచనం_రద్దు CERT_STORE_PROV_SYSTEM_A
#నిర్వచనం_రద్దు CERT_STORE_PROV_SYSTEM_W
#నిర్వచించు CERT_STORE_PROV_జ్ఞాపకం    ((LPCSTR)(size_t)2)
#నిర్వచించు CERT_STORE_PROV_SYSTEM_A  ((LPCSTR)(size_t)9)
#నిర్వచించు CERT_STORE_PROV_SYSTEM_W  ((LPCSTR)(size_t)10)
#అయితే_ముగింపు

/* Offered by mingw-w64 v8+, MS SDK 10.0.17763.0/VS2017 15.8+ */
#నిర్వచితం_కాకపోతే SCH_CREDENTIALS_సంస్కరణ
#నిర్వచించు SCH_CREDENTIALS_సంస్కరణ  0x00000005

రకనిర్వచనం గణన _eTlsAlgorithmUsage {
  Tls_Parameters_Cng_Alg_Usage_కీలకం_Exchange,
  TlsParametersCngAlgUsageSignature,
  TlsParametersCngAlgUsageCipher,
  TlsParametersCngAlgUsageDigest,
  TlsParametersCngAlgUsageCertSig
} eTlsAlgorithmUsage;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
రకనిర్వచనం నిర్మాణం _CRYPTO_SETTINGS {
  eTlsAlgorithmUsage eAlgorithmUsage;
  UNICODE_STRING strCngAlgId;
  DWORD cChainingModes;
  PUNICODE_STRING rgstrChainingModes; /* spellchecker:disable-line */
  DWORD dw_Min_బిట్టు_నిడివి;
  DWORD dw_Max_బిట్టు_నిడివి;
} CRYPTO_SETTINGS, *PCRYPTO_SETTINGS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
రకనిర్వచనం నిర్మాణం _TLS_PARAMETERS {
  DWORD cAlpnIds;
  PUNICODE_STRING rgstrAlpnIds; /* spellchecker:disable-line */
  DWORD grbitDisabledProtocols;
  DWORD cDisabledCrypto;
  PCRYPTO_SETTINGS pDisabledCrypto;
  DWORD dwFlags;
} TLS_PARAMETERS, *PTLS_PARAMETERS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
రకనిర్వచనం నిర్మాణం _SCH_CREDENTIALS {
  DWORD dw_సంస్కరణ;
  DWORD dwCredFormat;
  DWORD cCreds;
  PCCERT_CONTEXT *paCred;
  HCERTSTORE h_మూలం_Store;

  DWORD cMappers;
  నిర్మాణం _HMAPPER **aphMappers;

  DWORD dwSessionLifespan;
  DWORD dwFlags;
  DWORD cTlsParameters;
  PTLS_PARAMETERS pTlsParameters;
} SCH_CREDENTIALS, *PSCH_CREDENTIALS;

#అయితే_ముగింపు /* !SCH_CREDENTIALS_VERSION */

నిర్మాణం Curl_schannel_cred {
  CredHandle cred_handle;
  TCHAR *sni_hostname;
  HCERTSTORE client_cert_store;
  పూర్ణాంకం refcount;
};

నిర్మాణం Curl_schannel_ctxt {
  CtxtHandle ctxt_handle;
};

/* handle encoding/decoding buffers */
నిర్మాణం sbuffer {
  size_t నిడివి;
  size_t స్థానభ్రంశం;
  గుర్తులేని అక్షరం *బఫరు;
};

నిర్మాణం schannel_ssl_backend_దత్తాంశం {
  గుర్తులేని అక్షరం *send_బఫరు;
  size_t send_బఫరు_len;
  నిర్మాణం sbuffer encdata;
  నిర్మాణం sbuffer decdata;
  నిర్మాణం Curl_schannel_cred *cred;
  నిర్మాణం Curl_schannel_ctxt *ctxt;
  Sec_బఫరు shutdown_token;
  size_t shutdown_స్థానభ్రంశం;
  Sec_Pkg_Context_ప్రవాహం_Sizes ప్రవాహం_sizes;
  గుర్తులేని దీర్ఘ req_flags, ret_flags;
  CURLcode recv_unrecoverable_err; /* schannel_recv had an unrecoverable err */
  నిర్మాణం schannel_renegotiate_state {
    bool started;
    నిర్మాణం curltime ప్రారంభం_సమయం;
    పూర్ణాంకం io_need;
  } renegotiate_state;
  బిట్టు(recv_sspi_close_notify); /* true if connection closed by close_notify */
  బిట్టు(recv_connection_closed); /* true if connection closed, regardless how */
  బిట్టు(recv_renegotiating);     /* true if recv is doing renegotiation */
  బిట్టు(use_alpn); /* true if ALPN is used for this connection */
  బిట్టు(use_manual_cred_validation); /* true if manual cred validation is used */
  బిట్టు(పంపిన_shutdown);
  బిట్టు(shutdown_generated);
  /* encdata_is_incomplete: if encdata contains only a partial record that
     cannot be decrypted without another recv() (that is, status is
     SEC_E_INCOMPLETE_MESSAGE) then set this true. after recv() adds more
     bytes into encdata then set this back to false. */
  బిట్టు(encdata_is_incomplete);
};

నిర్మాణం schannel_cert_share {
  గుర్తులేని అక్షరం CAinfo_blob_digest[CURL_SHA256_DIGEST_నిడివి];
  size_t CAinfo_blob_కొలత;           /* CA info blob size */
  అక్షరం *CAfile;                      /* CAfile path used to generate
                                        certificate store */
  HCERTSTORE cert_store;             /* cached certificate store or
                                        NULL if none */
  నిర్మాణం curltime సమయం;              /* when the cached store was created */
};

/* size of the structure: 20 bytes */
నిర్మాణం అంకె_ip_దత్తాంశం {
  DWORD కొలత; /* 04 bytes */
  సంఘం {
    నిర్మాణం in_addr  ia;  /* 04 bytes */
    నిర్మాణం in6_addr ia6; /* 16 bytes */
  } b_దత్తాంశం;
};

HCERTSTORE Curl_schannel_get_cached_cert_store(నిర్మాణం Curl_cfilter *cf,
                                               నిర్మాణం Curl_easy *దత్తాంశం);

bool Curl_schannel_సమితి_cached_cert_store(నిర్మాణం Curl_cfilter *cf,
                                         నిర్మాణం Curl_easy *దత్తాంశం,
                                         HCERTSTORE cert_store);

#అయితే_ముగింపు /* USE_SCHANNEL */
#అయితే_ముగింపు /* HEADER_CURL_SCHANNEL_INT_H */
