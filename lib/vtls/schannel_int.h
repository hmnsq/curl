#اگر_غیر_تعریف_شدہ سرخی_CURL_SCHANNEL_INT_H
#تعریف_کرو سرخی_CURL_SCHANNEL_INT_H
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
#شامل "curl_setup.h"

#اگر_تعریف_شدہ USE_SCHANNEL

#شامل "vtls/vtls.h"
#شامل "curl_sha256.h"

#اگر defined(_MSC_VER) && (_MSC_VER <= 1700)
/* Workaround for warning:
   'type cast' : conversion from 'int' to 'LPCSTR' of greater size */
#تعریف_منسوخ CERT_STORE_PROV_حافظہ
#تعریف_منسوخ CERT_STORE_PROV_SYSTEM_A
#تعریف_منسوخ CERT_STORE_PROV_SYSTEM_W
#تعریف_کرو CERT_STORE_PROV_حافظہ    ((LPCSTR)(size_t)2)
#تعریف_کرو CERT_STORE_PROV_SYSTEM_A  ((LPCSTR)(size_t)9)
#تعریف_کرو CERT_STORE_PROV_SYSTEM_W  ((LPCSTR)(size_t)10)
#اگر_ختم

/* Offered by mingw-w64 v8+, MS SDK 10.0.17763.0/VS2017 15.8+ */
#اگر_غیر_تعریف_شدہ SCH_CREDENTIALS_نسخہ
#تعریف_کرو SCH_CREDENTIALS_نسخہ  0x00000005

قسمتعریف شمار _eTlsAlgorithmUsage {
  Tls_Parameters_Cng_Alg_Usage_کلید_Exchange,
  TlsParametersCngAlgUsageSignature,
  TlsParametersCngAlgUsageCipher,
  TlsParametersCngAlgUsageDigest,
  TlsParametersCngAlgUsageCertSig
} eTlsAlgorithmUsage;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
قسمتعریف ساخت _CRYPTO_SETTINGS {
  eTlsAlgorithmUsage eAlgorithmUsage;
  UNICODE_STRING strCngAlgId;
  DWORD cChainingModes;
  PUNICODE_STRING rgstrChainingModes; /* spellchecker:disable-line */
  DWORD dw_Min_ثنائی_ہندسہ_لمبائی;
  DWORD dw_Max_ثنائی_ہندسہ_لمبائی;
} CRYPTO_SETTINGS, *PCRYPTO_SETTINGS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
قسمتعریف ساخت _TLS_PARAMETERS {
  DWORD cAlpnIds;
  PUNICODE_STRING rgstrAlpnIds; /* spellchecker:disable-line */
  DWORD grbitDisabledProtocols;
  DWORD cDisabledCrypto;
  PCRYPTO_SETTINGS pDisabledCrypto;
  DWORD dwFlags;
} TLS_PARAMETERS, *PTLS_PARAMETERS;

/* !checksrc! disable TYPEDEFSTRUCT 1 */
قسمتعریف ساخت _SCH_CREDENTIALS {
  DWORD dw_نسخہ;
  DWORD dwCredFormat;
  DWORD cCreds;
  PCCERT_CONTEXT *paCred;
  HCERTSTORE h_جڑ_Store;

  DWORD cMappers;
  ساخت _HMAPPER **aphMappers;

  DWORD dwSessionLifespan;
  DWORD dwFlags;
  DWORD cTlsParameters;
  PTLS_PARAMETERS pTlsParameters;
} SCH_CREDENTIALS, *PSCH_CREDENTIALS;

#اگر_ختم /* !SCH_CREDENTIALS_VERSION */

ساخت Curl_schannel_cred {
  CredHandle cred_handle;
  TCHAR *sni_hostname;
  HCERTSTORE client_cert_store;
  صحیح refcount;
};

ساخت Curl_schannel_ctxt {
  CtxtHandle ctxt_handle;
};

/* handle encoding/decoding buffers */
ساخت sbuffer {
  size_t لمبائی;
  size_t انحراف;
  غیرعلامتی حرف *بفر;
};

ساخت schannel_ssl_backend_ڈیٹا {
  غیرعلامتی حرف *send_بفر;
  size_t send_بفر_len;
  ساخت sbuffer encdata;
  ساخت sbuffer decdata;
  ساخت Curl_schannel_cred *cred;
  ساخت Curl_schannel_ctxt *ctxt;
  Sec_بفر shutdown_token;
  size_t shutdown_انحراف;
  Sec_Pkg_Context_دھارا_Sizes دھارا_sizes;
  غیرعلامتی طویل req_flags, ret_flags;
  CURLcode recv_unrecoverable_err; /* schannel_recv had an unrecoverable err */
  ساخت schannel_renegotiate_state {
    bool started;
    ساخت curltime آغاز_وقت;
    صحیح io_need;
  } renegotiate_state;
  ثنائی_ہندسہ(recv_sspi_close_notify); /* true if connection closed by close_notify */
  ثنائی_ہندسہ(recv_connection_closed); /* true if connection closed, regardless how */
  ثنائی_ہندسہ(recv_renegotiating);     /* true if recv is doing renegotiation */
  ثنائی_ہندسہ(use_alpn); /* true if ALPN is used for this connection */
  ثنائی_ہندسہ(use_manual_cred_validation); /* true if manual cred validation is used */
  ثنائی_ہندسہ(بھیجا_shutdown);
  ثنائی_ہندسہ(shutdown_generated);
  /* encdata_is_incomplete: if encdata contains only a partial record that
     cannot be decrypted without another recv() (that is, status is
     SEC_E_INCOMPLETE_MESSAGE) then set this true. after recv() adds more
     bytes into encdata then set this back to false. */
  ثنائی_ہندسہ(encdata_is_incomplete);
};

ساخت schannel_cert_share {
  غیرعلامتی حرف CAinfo_blob_digest[CURL_SHA256_DIGEST_لمبائی];
  size_t CAinfo_blob_سائز;           /* CA info blob size */
  حرف *CAfile;                      /* CAfile path used to generate
                                        certificate store */
  HCERTSTORE cert_store;             /* cached certificate store or
                                        NULL if none */
  ساخت curltime وقت;              /* when the cached store was created */
};

/* size of the structure: 20 bytes */
ساخت عدد_ip_ڈیٹا {
  DWORD سائز; /* 04 bytes */
  اتحاد {
    ساخت in_addr  ia;  /* 04 bytes */
    ساخت in6_addr ia6; /* 16 bytes */
  } b_ڈیٹا;
};

HCERTSTORE Curl_schannel_get_cached_cert_store(ساخت Curl_cfilter *cf,
                                               ساخت Curl_easy *ڈیٹا);

bool Curl_schannel_سیٹ_cached_cert_store(ساخت Curl_cfilter *cf,
                                         ساخت Curl_easy *ڈیٹا,
                                         HCERTSTORE cert_store);

#اگر_ختم /* USE_SCHANNEL */
#اگر_ختم /* HEADER_CURL_SCHANNEL_INT_H */
