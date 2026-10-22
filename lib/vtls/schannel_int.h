#अगर_अघोषित HEADER_CURL_SCHANNEL_INT_H
#घोषणा HEADER_CURL_SCHANNEL_INT_H
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
#समावेश "curl_setup.h"

#अगर_घोषित USE_SCHANNEL

#समावेश "vtls/vtls.h"
#समावेश "curl_sha256.h"

#अगर defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'LPCSTR' of greater size */
#अघोषित CERT_STORE_PROV_MEMORY
#अघोषित CERT_STORE_PROV_SYSTEM_A
#अघोषित CERT_STORE_PROV_SYSTEM_W
#घोषणा CERT_STORE_PROV_MEMORY    ((LPCSTR)(माप_प्रकार)2)
#घोषणा CERT_STORE_PROV_SYSTEM_A  ((LPCSTR)(माप_प्रकार)9)
#घोषणा CERT_STORE_PROV_SYSTEM_W  ((LPCSTR)(माप_प्रकार)10)
#पूर्ण_अगर

/* Offered by mingw-w64 v8+, MS SDK 10.0.17763.0/VS2017 15.8+ */
#अगर_अघोषित SCH_CREDENTIALS_VERSION
#घोषणा SCH_CREDENTIALS_VERSION  0x00000005

प्रकार क्रमागत _eTlsAlgorithmUsage {
  Tls_Parameters_Cng_Alg_Usage_कुंजी_Exchange,
  TlsParametersCngAlgUsageSignature,
  TlsParametersCngAlgUsageCipher,
  TlsParametersCngAlgUsageDigest,
  TlsParametersCngAlgUsageCertSig
} eTlsAlgorithmUsage;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकार काष्ठा _CRYPTO_SETTINGS {
  eTlsAlgorithmUsage eAlgorithmUsage;
  UNICODE_STRING strCngAlgId;
  DWORD cChainingModes;
  PUNICODE_STRING rgstrChainingModes; /* spellchecker:disable-line */
  DWORD dw_Min_Bit_लंबाई;
  DWORD dw_Max_Bit_लंबाई;
} CRYPTO_SETTINGS, *PCRYPTO_SETTINGS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकार काष्ठा _TLS_PARAMETERS {
  DWORD cAlpnIds;
  PUNICODE_STRING rgstrAlpnIds; /* spellchecker:disable-line */
  DWORD grbitDisabledProtocols;
  DWORD cDisabledCrypto;
  PCRYPTO_SETTINGS pDisabledCrypto;
  DWORD dwFlags;
} TLS_PARAMETERS, *PTLS_PARAMETERS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकार काष्ठा _SCH_CREDENTIALS {
  DWORD dwVersion;
  DWORD dwCredFormat;
  DWORD cCreds;
  PCCERT_CONTEXT *paCred;
  HCERTSTORE h_मूल_Store;

  DWORD cMappers;
  काष्ठा _HMAPPER **aphMappers;

  DWORD dwSessionLifespan;
  DWORD dwFlags;
  DWORD cTlsParameters;
  PTLS_PARAMETERS pTlsParameters;
} SCH_CREDENTIALS, *PSCH_CREDENTIALS;

#पूर्ण_अगर /* !SCH_CREDENTIALS_VERSION */

काष्ठा Curl_schannel_cred {
  CredHandle cred_handle;
  TCHAR *sni_hostname;
  HCERTSTORE client_cert_store;
  पूर्णांक refcount;
};

काष्ठा Curl_schannel_ctxt {
  CtxtHandle ctxt_handle;
};

/* handle encoding/decoding buffers */
काष्ठा sbuffer {
  माप_प्रकार लंबाई;
  माप_प्रकार offset;
  अचिन्हित अक्षर *बफर;
};

काष्ठा schannel_ssl_backend_तथ्य {
  अचिन्हित अक्षर *send_बफर;
  माप_प्रकार send_बफर_len;
  काष्ठा sbuffer encdata;
  काष्ठा sbuffer decdata;
  काष्ठा Curl_schannel_cred *cred;
  काष्ठा Curl_schannel_ctxt *ctxt;
  Sec_बफर shutdown_token;
  माप_प्रकार shutdown_offset;
  SecPkgContext_StreamSizes stream_sizes;
  अचिन्हित दीर्घ req_flags, ret_flags;
  CURLcode recv_unrecoverable_err; /* schannel_recv had an unrecoverable err */
  काष्ठा schannel_renegotiate_state {
    bool started;
    काष्ठा curltime start_time;
    पूर्णांक io_need;
  } renegotiate_state;
  BIT(recv_sspi_close_notify); /* true if connection closed by close_notify */
  BIT(recv_connection_closed); /* true if connection closed, regardless how */
  BIT(recv_renegotiating);     /* true if recv is doing renegotiation */
  BIT(use_alpn); /* true if ALPN is used for this connection */
  BIT(use_manual_cred_validation); /* true if manual cred validation is used */
  BIT(sent_shutdown);
  BIT(shutdown_generated);
  /* encdata_is_incomplete: if encdata contains only a partial record that
     cannot be decrypted without another recv() (that is, status is
     SEC_E_INCOMPLETE_MESSAGE) then set this true. after recv() adds more
     bytes into encdata then set this back to false. */
  BIT(encdata_is_incomplete);
};

काष्ठा schannel_cert_share {
  अचिन्हित अक्षर CAinfo_blob_digest[CURL_SHA256_DIGEST_लंबाई];
  माप_प्रकार CAinfo_blob_आकार;           /* CA info blob size */
  अक्षर *CAfile;                      /* CAfile path used to generate
                                        certificate store */
  HCERTSTORE cert_store;             /* cached certificate store or
                                        NULL if none */
  काष्ठा curltime समय;              /* when the cached store was created */
};

/* size of the structure: 20 bytes */
काष्ठा अंक_ip_तथ्य {
  DWORD आकार; /* 04 bytes */
  जोड़ {
    काष्ठा in_addr  ia;  /* 04 bytes */
    काष्ठा in6_addr ia6; /* 16 bytes */
  } b_तथ्य;
};

HCERTSTORE Curl_schannel_get_cached_cert_store(काष्ठा Curl_cfilter *cf,
                                               काष्ठा Curl_easy *तथ्य);

bool Curl_schannel_set_cached_cert_store(काष्ठा Curl_cfilter *cf,
                                         काष्ठा Curl_easy *तथ्य,
                                         HCERTSTORE cert_store);

#पूर्ण_अगर /* USE_SCHANNEL */
#पूर्ण_अगर /* HEADER_CURL_SCHANNEL_INT_H */
