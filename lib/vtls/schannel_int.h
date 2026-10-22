#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_SCHANNEL_INT_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_SCHANNEL_INT_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SCHANNEL

#ಅನ್ತರ್ಭಾವ "vtls/vtls.h"
#ಅನ್ತರ್ಭಾವ "curl_sha256.h"

#ಆದರೆ defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'LPCSTR' of greater size */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CERT_STORE_PROV_ಸ್ಮರಣೆ
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CERT_STORE_PROV_SYSTEM_A
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು CERT_STORE_PROV_SYSTEM_W
#ವ್ಯಾಖ್ಯಾನಿಸು CERT_STORE_PROV_ಸ್ಮರಣೆ    ((LPCSTR)(size_t)2)
#ವ್ಯಾಖ್ಯಾನಿಸು CERT_STORE_PROV_SYSTEM_A  ((LPCSTR)(size_t)9)
#ವ್ಯಾಖ್ಯಾನಿಸು CERT_STORE_PROV_SYSTEM_W  ((LPCSTR)(size_t)10)
#ಆದರೆ_ಅಂತ್ಯ

/* Offered by mingw-w64 v8+, MS SDK 10.0.17763.0/VS2017 15.8+ */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SCH_CREDENTIALS_ಆವೃತ್ತಿ
#ವ್ಯಾಖ್ಯಾನಿಸು SCH_CREDENTIALS_ಆವೃತ್ತಿ  0x00000005

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ _eTlsAlgorithmUsage {
  Tls_Parameters_Cng_Alg_Usage_ಕೀಲಿ_Exchange,
  TlsParametersCngAlgUsageSignature,
  TlsParametersCngAlgUsageCipher,
  TlsParametersCngAlgUsageDigest,
  TlsParametersCngAlgUsageCertSig
} eTlsAlgorithmUsage;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _CRYPTO_SETTINGS {
  eTlsAlgorithmUsage eAlgorithmUsage;
  UNICODE_STRING strCngAlgId;
  DWORD cChainingModes;
  PUNICODE_STRING rgstrChainingModes; /* spellchecker:disable-line */
  DWORD dw_Min_ಬಿಟ್_ಉದ್ದಳತೆ;
  DWORD dw_Max_ಬಿಟ್_ಉದ್ದಳತೆ;
} CRYPTO_SETTINGS, *PCRYPTO_SETTINGS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _TLS_PARAMETERS {
  DWORD cAlpnIds;
  PUNICODE_STRING rgstrAlpnIds; /* spellchecker:disable-line */
  DWORD grbitDisabledProtocols;
  DWORD cDisabledCrypto;
  PCRYPTO_SETTINGS pDisabledCrypto;
  DWORD dwFlags;
} TLS_PARAMETERS, *PTLS_PARAMETERS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _SCH_CREDENTIALS {
  DWORD dw_ಆವೃತ್ತಿ;
  DWORD dwCredFormat;
  DWORD cCreds;
  PCCERT_CONTEXT *paCred;
  HCERTSTORE h_ಬೇರು_Store;

  DWORD cMappers;
  ರಚನೆ _HMAPPER **aphMappers;

  DWORD dwSessionLifespan;
  DWORD dwFlags;
  DWORD cTlsParameters;
  PTLS_PARAMETERS pTlsParameters;
} SCH_CREDENTIALS, *PSCH_CREDENTIALS;

#ಆದರೆ_ಅಂತ್ಯ /* !SCH_CREDENTIALS_VERSION */

ರಚನೆ Curl_schannel_cred {
  CredHandle cred_handle;
  TCHAR *sni_hostname;
  HCERTSTORE client_cert_store;
  ಪೂರ್ಣಾಂಕ refcount;
};

ರಚನೆ Curl_schannel_ctxt {
  CtxtHandle ctxt_handle;
};

/* handle encoding/decoding buffers */
ರಚನೆ sbuffer {
  size_t ಉದ್ದಳತೆ;
  size_t ಸ್ಥಾನಾಂತರ;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಬಫರ್;
};

ರಚನೆ schannel_ssl_backend_ದತ್ತಾಂಶ {
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *send_ಬಫರ್;
  size_t send_ಬಫರ್_len;
  ರಚನೆ sbuffer encdata;
  ರಚನೆ sbuffer decdata;
  ರಚನೆ Curl_schannel_cred *cred;
  ರಚನೆ Curl_schannel_ctxt *ctxt;
  Sec_ಬಫರ್ shutdown_token;
  size_t shutdown_ಸ್ಥಾನಾಂತರ;
  Sec_Pkg_Context_ಹರಿವು_Sizes ಹರಿವು_sizes;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ req_flags, ret_flags;
  CURLcode recv_unrecoverable_err; /* schannel_recv had an unrecoverable err */
  ರಚನೆ schannel_renegotiate_state {
    bool started;
    ರಚನೆ curltime ಆರಂಭ_ಸಮಯ;
    ಪೂರ್ಣಾಂಕ io_need;
  } renegotiate_state;
  ಬಿಟ್(recv_sspi_close_notify); /* true if connection closed by close_notify */
  ಬಿಟ್(recv_connection_closed); /* true if connection closed, regardless how */
  ಬಿಟ್(recv_renegotiating);     /* true if recv is doing renegotiation */
  ಬಿಟ್(use_alpn); /* true if ALPN is used for this connection */
  ಬಿಟ್(use_manual_cred_validation); /* true if manual cred validation is used */
  ಬಿಟ್(ಕಳುಹಿಸಿದ_shutdown);
  ಬಿಟ್(shutdown_generated);
  /* encdata_is_incomplete: if encdata contains only a partial record that
     cannot be decrypted without another recv() (that is, status is
     SEC_E_INCOMPLETE_MESSAGE) then set this true. after recv() adds more
     bytes into encdata then set this back to false. */
  ಬಿಟ್(encdata_is_incomplete);
};

ರಚನೆ schannel_cert_share {
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ CAinfo_blob_digest[CURL_SHA256_DIGEST_ಉದ್ದಳತೆ];
  size_t CAinfo_blob_ಗಾತ್ರ;           /* CA info blob size */
  ಅಕ್ಷರ *CAfile;                      /* CAfile path used to generate
                                        certificate store */
  HCERTSTORE cert_store;             /* cached certificate store or
                                        NULL if none */
  ರಚನೆ curltime ಸಮಯ;              /* when the cached store was created */
};

/* size of the structure: 20 bytes */
ರಚನೆ ಅಂಕೆ_ip_ದತ್ತಾಂಶ {
  DWORD ಗಾತ್ರ_2; /* 04 bytes */
  ಒಕ್ಕೂಟ {
    ರಚನೆ in_addr  ia;  /* 04 bytes */
    ರಚನೆ in6_addr ia6; /* 16 bytes */
  } b_ದತ್ತಾಂಶ;
};

HCERTSTORE Curl_schannel_get_cached_cert_store(ರಚನೆ Curl_cfilter *cf,
                                               ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

bool Curl_schannel_ಗಣ_cached_cert_store(ರಚನೆ Curl_cfilter *cf,
                                         ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                         HCERTSTORE cert_store);

#ಆದರೆ_ಅಂತ್ಯ /* USE_SCHANNEL */
#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_SCHANNEL_INT_H */
