#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_SCHANNEL_INT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_SCHANNEL_INT_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SCHANNEL

#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/vtls.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_sha256.h"

#ਜੇ defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'LPCSTR' of greater size */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CERT_STORE_PROV_ਸਮ੍ਰਿਤੀ
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CERT_STORE_PROV_SYSTEM_A
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ CERT_STORE_PROV_SYSTEM_W
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CERT_STORE_PROV_ਸਮ੍ਰਿਤੀ    ((LPCSTR)(size_t)2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CERT_STORE_PROV_SYSTEM_A  ((LPCSTR)(size_t)9)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CERT_STORE_PROV_SYSTEM_W  ((LPCSTR)(size_t)10)
#ਜੇ_ਅੰਤ

/* Offered by mingw-w64 v8+, MS SDK 10.0.17763.0/VS2017 15.8+ */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SCH_CREDENTIALS_ਸੰਸਕਰਣ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCH_CREDENTIALS_ਸੰਸਕਰਣ  0x00000005

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ _eTlsAlgorithmUsage {
  Tls_Parameters_Cng_Alg_Usage_ਕੁੰਜੀ_Exchange,
  TlsParametersCngAlgUsageSignature,
  TlsParametersCngAlgUsageCipher,
  TlsParametersCngAlgUsageDigest,
  TlsParametersCngAlgUsageCertSig
} eTlsAlgorithmUsage;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _CRYPTO_SETTINGS {
  eTlsAlgorithmUsage eAlgorithmUsage;
  UNICODE_STRING strCngAlgId;
  DWORD cChainingModes;
  PUNICODE_STRING rgstrChainingModes; /* spellchecker:disable-line */
  DWORD dw_Min_ਦੋਆਧਾਰੀ_ਅੰਕ_ਲੰਮਾਈ;
  DWORD dw_Max_ਦੋਆਧਾਰੀ_ਅੰਕ_ਲੰਮਾਈ;
} CRYPTO_SETTINGS, *PCRYPTO_SETTINGS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _TLS_PARAMETERS {
  DWORD cAlpnIds;
  PUNICODE_STRING rgstrAlpnIds; /* spellchecker:disable-line */
  DWORD grbitDisabledProtocols;
  DWORD cDisabledCrypto;
  PCRYPTO_SETTINGS pDisabledCrypto;
  DWORD dwFlags;
} TLS_PARAMETERS, *PTLS_PARAMETERS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _SCH_CREDENTIALS {
  DWORD dw_ਸੰਸਕਰਣ;
  DWORD dwCredFormat;
  DWORD cCreds;
  PCCERT_CONTEXT *paCred;
  HCERTSTORE h_ਜੜ੍ਹ_Store;

  DWORD cMappers;
  ਬਣਤਰ _HMAPPER **aphMappers;

  DWORD dwSessionLifespan;
  DWORD dwFlags;
  DWORD cTlsParameters;
  PTLS_PARAMETERS pTlsParameters;
} SCH_CREDENTIALS, *PSCH_CREDENTIALS;

#ਜੇ_ਅੰਤ /* !SCH_CREDENTIALS_VERSION */

ਬਣਤਰ Curl_schannel_cred {
  CredHandle cred_handle;
  TCHAR *sni_hostname;
  HCERTSTORE client_cert_store;
  ਪੂਰਨ refcount;
};

ਬਣਤਰ Curl_schannel_ctxt {
  CtxtHandle ctxt_handle;
};

/* handle encoding/decoding buffers */
ਬਣਤਰ sbuffer {
  size_t ਲੰਮਾਈ;
  size_t ਵਿੱਥ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਫਰ;
};

ਬਣਤਰ schannel_ssl_backend_ਅੰਕੜੇ {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *send_ਬਫਰ;
  size_t send_ਬਫਰ_len;
  ਬਣਤਰ sbuffer encdata;
  ਬਣਤਰ sbuffer decdata;
  ਬਣਤਰ Curl_schannel_cred *cred;
  ਬਣਤਰ Curl_schannel_ctxt *ctxt;
  Sec_ਬਫਰ shutdown_token;
  size_t shutdown_ਵਿੱਥ;
  Sec_Pkg_Context_ਧਾਰਾ_Sizes ਧਾਰਾ_sizes;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ req_flags, ret_flags;
  CURLcode recv_unrecoverable_err; /* schannel_recv had an unrecoverable err */
  ਬਣਤਰ schannel_renegotiate_state {
    bool started;
    ਬਣਤਰ curltime ਸ਼ੁਰੂ_ਸਮਾਂ;
    ਪੂਰਨ io_need;
  } renegotiate_state;
  ਦੋਆਧਾਰੀ_ਅੰਕ(recv_sspi_close_notify); /* true if connection closed by close_notify */
  ਦੋਆਧਾਰੀ_ਅੰਕ(recv_connection_closed); /* true if connection closed, regardless how */
  ਦੋਆਧਾਰੀ_ਅੰਕ(recv_renegotiating);     /* true if recv is doing renegotiation */
  ਦੋਆਧਾਰੀ_ਅੰਕ(use_alpn); /* true if ALPN is used for this connection */
  ਦੋਆਧਾਰੀ_ਅੰਕ(use_manual_cred_validation); /* true if manual cred validation is used */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਭੇਜਿਆ_shutdown);
  ਦੋਆਧਾਰੀ_ਅੰਕ(shutdown_generated);
  /* encdata_is_incomplete: if encdata contains only a partial record that
     cannot be decrypted without another recv() (that is, status is
     SEC_E_INCOMPLETE_MESSAGE) then set this true. after recv() adds more
     bytes into encdata then set this back to false. */
  ਦੋਆਧਾਰੀ_ਅੰਕ(encdata_is_incomplete);
};

ਬਣਤਰ schannel_cert_share {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ CAinfo_blob_digest[CURL_SHA256_DIGEST_ਲੰਮਾਈ];
  size_t CAinfo_blob_ਮਾਪ;           /* CA info blob size */
  ਅੱਖਰ *CAfile;                      /* CAfile path used to generate
                                        certificate store */
  HCERTSTORE cert_store;             /* cached certificate store or
                                        NULL if none */
  ਬਣਤਰ curltime ਸਮਾਂ;              /* when the cached store was created */
};

/* size of the structure: 20 bytes */
ਬਣਤਰ ਅੰਕ_ip_ਅੰਕੜੇ {
  DWORD ਮਾਪ; /* 04 bytes */
  ਸੰਘ {
    ਬਣਤਰ in_addr  ia;  /* 04 bytes */
    ਬਣਤਰ in6_addr ia6; /* 16 bytes */
  } b_ਅੰਕੜੇ;
};

HCERTSTORE Curl_schannel_get_cached_cert_store(ਬਣਤਰ Curl_cfilter *cf,
                                               ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);

bool Curl_schannel_ਸੈੱਟ_cached_cert_store(ਬਣਤਰ Curl_cfilter *cf,
                                         ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                         HCERTSTORE cert_store);

#ਜੇ_ਅੰਤ /* USE_SCHANNEL */
#ਜੇ_ਅੰਤ /* HEADER_CURL_SCHANNEL_INT_H */
