#यदि_अपरिभाषितम् शीर्षकम्_CURL_SCHANNEL_INT_H
#परिभाषय शीर्षकम्_CURL_SCHANNEL_INT_H
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
#अन्तर्भाव "curl_setup.h"

#यदि_परिभाषितम् USE_SCHANNEL

#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "curl_sha256.h"

#यदि defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'LPCSTR' of greater size */
#अपरिभाषय CERT_STORE_PROV_स्मृतिः
#अपरिभाषय CERT_STORE_PROV_SYSTEM_A
#अपरिभाषय CERT_STORE_PROV_SYSTEM_W
#परिभाषय CERT_STORE_PROV_स्मृतिः    ((LPCSTR)(size_t)2)
#परिभाषय CERT_STORE_PROV_SYSTEM_A  ((LPCSTR)(size_t)9)
#परिभाषय CERT_STORE_PROV_SYSTEM_W  ((LPCSTR)(size_t)10)
#यदि_अन्तः

/* Offered by mingw-w64 v8+, MS SDK 10.0.17763.0/VS2017 15.8+ */
#यदि_अपरिभाषितम् SCH_CREDENTIALS_संस्करणम्
#परिभाषय SCH_CREDENTIALS_संस्करणम्  0x00000005

प्रकारनिर्देश गणना _eTlsAlgorithmUsage {
  Tls_Parameters_Cng_Alg_Usage_कुञ्चिका_Exchange,
  TlsParametersCngAlgUsageSignature,
  TlsParametersCngAlgUsageCipher,
  TlsParametersCngAlgUsageDigest,
  TlsParametersCngAlgUsageCertSig
} eTlsAlgorithmUsage;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकारनिर्देश संरचना _CRYPTO_SETTINGS {
  eTlsAlgorithmUsage eAlgorithmUsage;
  UNICODE_STRING strCngAlgId;
  DWORD cChainingModes;
  PUNICODE_STRING rgstrChainingModes; /* spellchecker:disable-line */
  DWORD dw_Min_Bit_दीर्घता;
  DWORD dw_Max_Bit_दीर्घता;
} CRYPTO_SETTINGS, *PCRYPTO_SETTINGS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकारनिर्देश संरचना _TLS_PARAMETERS {
  DWORD cAlpnIds;
  PUNICODE_STRING rgstrAlpnIds; /* spellchecker:disable-line */
  DWORD grbitDisabledProtocols;
  DWORD cDisabledCrypto;
  PCRYPTO_SETTINGS pDisabledCrypto;
  DWORD dwFlags;
} TLS_PARAMETERS, *PTLS_PARAMETERS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
प्रकारनिर्देश संरचना _SCH_CREDENTIALS {
  DWORD dw_संस्करणम्;
  DWORD dwCredFormat;
  DWORD cCreds;
  PCCERT_CONTEXT *paCred;
  HCERTSTORE h_मूलम्_Store;

  DWORD cMappers;
  संरचना _HMAPPER **aphMappers;

  DWORD dwSessionLifespan;
  DWORD dwFlags;
  DWORD cTlsParameters;
  PTLS_PARAMETERS pTlsParameters;
} SCH_CREDENTIALS, *PSCH_CREDENTIALS;

#यदि_अन्तः /* !SCH_CREDENTIALS_VERSION */

संरचना Curl_schannel_cred {
  CredHandle cred_handle;
  TCHAR *sni_hostname;
  HCERTSTORE client_cert_store;
  पूर्णाङ्क refcount;
};

संरचना Curl_schannel_ctxt {
  CtxtHandle ctxt_handle;
};

/* handle encoding/decoding buffers */
संरचना sbuffer {
  size_t दीर्घता;
  size_t विस्थापनम्;
  अचिह्नित वर्ण *बफर;
};

संरचना schannel_ssl_backend_दत्त {
  अचिह्नित वर्ण *send_बफर;
  size_t send_बफर_len;
  संरचना sbuffer encdata;
  संरचना sbuffer decdata;
  संरचना Curl_schannel_cred *cred;
  संरचना Curl_schannel_ctxt *ctxt;
  Sec_बफर shutdown_token;
  size_t shutdown_विस्थापनम्;
  Sec_Pkg_Context_प्रवाहः_Sizes प्रवाहः_sizes;
  अचिह्नित दीर्घ req_flags, ret_flags;
  CURLcode recv_unrecoverable_err; /* schannel_recv had an unrecoverable err */
  संरचना schannel_renegotiate_state {
    bool started;
    संरचना curltime start_time;
    पूर्णाङ्क io_need;
  } renegotiate_state;
  BIT(recv_sspi_close_notify); /* true if connection closed by close_notify */
  BIT(recv_connection_closed); /* true if connection closed, regardless how */
  BIT(recv_renegotiating);     /* true if recv is doing renegotiation */
  BIT(use_alpn); /* true if ALPN is used for this connection */
  BIT(use_manual_cred_validation); /* true if manual cred validation is used */
  BIT(प्रेषितम्_shutdown);
  BIT(shutdown_generated);
  /* encdata_is_incomplete: if encdata contains only a partial record that
     cannot be decrypted without another recv() (that is, status is
     SEC_E_INCOMPLETE_MESSAGE) then set this true. after recv() adds more
     bytes into encdata then set this back to false. */
  BIT(encdata_is_incomplete);
};

संरचना schannel_cert_share {
  अचिह्नित वर्ण CAinfo_blob_digest[CURL_SHA256_DIGEST_दीर्घता];
  size_t CAinfo_blob_आकार;           /* CA info blob size */
  वर्ण *CAfile;                      /* CAfile path used to generate
                                        certificate store */
  HCERTSTORE cert_store;             /* cached certificate store or
                                        NULL if none */
  संरचना curltime time;              /* when the cached store was created */
};

/* size of the structure: 20 bytes */
संरचना अङ्कः_ip_दत्त {
  DWORD आकार; /* 04 bytes */
  सङ्घ {
    संरचना in_addr  ia;  /* 04 bytes */
    संरचना in6_addr ia6; /* 16 bytes */
  } b_दत्त;
};

HCERTSTORE Curl_schannel_get_cached_cert_store(संरचना Curl_cfilter *cf,
                                               संरचना Curl_easy *दत्त);

bool Curl_schannel_समुच्चयः_cached_cert_store(संरचना Curl_cfilter *cf,
                                         संरचना Curl_easy *दत्त,
                                         HCERTSTORE cert_store);

#यदि_अन्तः /* USE_SCHANNEL */
#यदि_अन्तः /* HEADER_CURL_SCHANNEL_INT_H */
